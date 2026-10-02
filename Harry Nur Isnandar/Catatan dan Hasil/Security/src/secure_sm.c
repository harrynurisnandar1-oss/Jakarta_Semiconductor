#include "sts.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static STSKeyPair *g_meter_key = NULL;
static char g_storage[512];

/*
 * Read and decode the secure storage master key from the environment.
 * Output: Writes a STS_KEY_LEN-byte key to 'key' and returns 1 when valid;
 *         otherwise returns 0.
 */
static int storage_key(unsigned char key[STS_KEY_LEN]) {
    const char *env = getenv("STS_STORAGE_KEY_HEX");
    if (!env) return 0;
    size_t n = 0;
    return sts_hex_decode(env, key, STS_KEY_LEN, &n) && n == STS_KEY_LEN;
}

/*
 * Encrypt and persist the smart-meter private key using the storage key.
 * Output: Writes an AES-256-GCM protected key record to 'path' and returns 1
 *         on success; returns 0 on any failure.
 */
static int save_private_key(const char *path, const STSKeyPair *kp) {
    unsigned char master[STS_KEY_LEN], nonce[STS_GCM_NONCE_LEN], tag[STS_GCM_TAG_LEN];
    unsigned char der[256], cipher[256];
    size_t der_len = sizeof(der), cipher_len = 0;
    if (!storage_key(master) || !sts_keypair_private_der(kp, der, &der_len) || !sts_random_bytes(nonce, sizeof(nonce))) return 0;
    if (!sts_aes256_gcm_encrypt(master, nonce, NULL, 0, der, der_len, cipher, &cipher_len, tag)) goto fail;
    char hn[32], hc[600], ht[64];
    int ok = sts_hex_encode(nonce, sizeof(nonce), hn, sizeof(hn)) &&
             sts_hex_encode(cipher, cipher_len, hc, sizeof(hc)) &&
             sts_hex_encode(tag, sizeof(tag), ht, sizeof(ht));
    if (!ok) goto fail;
    FILE *f = fopen(path, "w");
    if (!f) goto fail;
    ok = fprintf(f, "AES-256-GCM,%s,%s,%s\n", hn, hc, ht) > 0;
    fclose(f);
    memset(master, 0, sizeof(master)); memset(der, 0, sizeof(der)); memset(cipher, 0, sizeof(cipher));
    return ok;
fail:
    memset(master, 0, sizeof(master)); memset(der, 0, sizeof(der)); memset(cipher, 0, sizeof(cipher));
    return 0;
}

/*
 * Load, authenticate, decrypt, and reconstruct a stored meter private key.
 * Output: Stores a reconstructed STSKeyPair through 'out' and returns 1 on
 *         success; returns 0 if the record is missing, invalid, or unauthenticated.
 */
static int load_private_key(const char *path, STSKeyPair **out) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    char line[1200], algo[32], hn[64], hc[700], ht[64];
    int ok = 0;
    if (!fgets(line, sizeof(line), f)) { fclose(f); return 0; }
    fclose(f);
    if (sscanf(line, "%31[^,],%63[^,],%699[^,],%63s", algo, hn, hc, ht) != 4 || strcmp(algo, "AES-256-GCM") != 0) return 0;
    unsigned char master[STS_KEY_LEN], nonce[STS_GCM_NONCE_LEN], tag[STS_GCM_TAG_LEN], cipher[256], der[256];
    size_t nnonce, ntag, ncipher, nder = sizeof(der);
    if (!storage_key(master) || !sts_hex_decode(hn, nonce, sizeof(nonce), &nnonce) || nnonce != sizeof(nonce) ||
        !sts_hex_decode(ht, tag, sizeof(tag), &ntag) || ntag != sizeof(tag) ||
        !sts_hex_decode(hc, cipher, sizeof(cipher), &ncipher)) goto done;
    if (!sts_aes256_gcm_decrypt(master, nonce, NULL, 0, cipher, ncipher, tag, der, &nder)) goto done;
    ok = sts_keypair_from_private_der(der, nder, out);
done:
    memset(master, 0, sizeof(master)); memset(cipher, 0, sizeof(cipher)); memset(der, 0, sizeof(der));
    return ok;
}

/*
 * Initialize secure smart-meter state from persistent key storage.
 * Output: Loads the existing meter key or generates and saves a new one;
 *         returns 1 on success and 0 on failure.
 */
int secure_sm_init(const char *storage_path) {
    if (!storage_path || strlen(storage_path) >= sizeof(g_storage)) return 0;
    secure_sm_shutdown();
    strcpy(g_storage, storage_path);
    if (load_private_key(g_storage, &g_meter_key)) return 1;
    if (!sts_ecdh_generate(&g_meter_key)) return 0;
    if (!save_private_key(g_storage, g_meter_key)) { secure_sm_shutdown(); return 0; }
    return 1;
}

/*
 * Release the secure smart-meter key and clear its stored path.
 * Output: Leaves the secure-meter global state uninitialized and returns void.
 */
void secure_sm_shutdown(void) {
    sts_keypair_free(g_meter_key);
    g_meter_key = NULL;
    g_storage[0] = '\0';
}

/*
 * Export the smart-meter public key in DER encoding.
 * Output: Writes the key to 'out', updates '*out_len', and returns 1 on
 *         success; returns 0 when the key is unavailable or export fails.
 */
int secure_sm_get_public_key(unsigned char out[STS_EC_PUBLIC_KEY_LEN], size_t *out_len) {
    if (!g_meter_key) return 0;
    return sts_keypair_public_der(g_meter_key, out, out_len);
}

/*
 * Build the AES-GCM Additional Authenticated Data for a supply group.
 * Output: Writes the formatted AAD to 'out' and returns its length.
 */
static size_t aad_for(const SupplyGroup *sg, unsigned char out[64]) {
    return (size_t)snprintf((char *)out, 64, "SGC=%s;KRN=%d;KT=%d", sg->sgc, sg->krn, sg->kt);
}

/*
 * Derive the meter-side transport key and authenticate/decrypt the vending key.
 * Output: Writes the plaintext vending key to 'out_vk', updates '*out_len',
 *         and returns 1 only when authentication and decryption succeed.
 *         On failure, '*out_len' is set to 0 and 0 is returned.
 */
int secure_sm_decrypt_vending_key(const SupplyGroup *sg,
                                  const unsigned char *kmc_public_der, size_t kmc_public_len,
                                  const EncryptedVK *encrypted,
                                  unsigned char out_vk[STS_VK_LEN], size_t *out_len) {
    if (!sg || !kmc_public_der || !encrypted || !out_vk || !out_len || !g_meter_key) return 0;
    unsigned char shared[128], key[STS_KEY_LEN], aad[64];
    size_t shared_len = sizeof(shared);
    size_t aad_len = aad_for(sg, aad);
    if (!sts_ecdh_derive(g_meter_key, kmc_public_der, kmc_public_len, shared, &shared_len)) return 0;
    unsigned char salt[32] = {0};
    unsigned char info[128];
    size_t info_len = (size_t)snprintf((char *)info, sizeof(info),
                                       "STS-KMC-METER-AES256-GCM|SGC=%s|KRN=%d|KT=%d",
                                       sg->sgc, sg->krn, sg->kt);
    int ok = sts_hkdf_sha256(shared, shared_len, salt, sizeof(salt), info, info_len, key, sizeof(key));
    if (ok) ok = sts_aes256_gcm_decrypt(key, encrypted->nonce, aad, aad_len,
                                        encrypted->ciphertext, encrypted->ciphertext_len,
                                        encrypted->tag, out_vk, out_len);
    memset(shared, 0, sizeof(shared)); memset(key, 0, sizeof(key));
    if (!ok) *out_len = 0;
    return ok;
}
