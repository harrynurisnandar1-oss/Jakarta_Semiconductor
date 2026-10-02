#include "sts.h"

#include <stdio.h>
#include <string.h>

/*
 * Validate a SupplyGroup before it is used by KMC operations.
 * Output: Returns 1 for a valid supply group; otherwise returns 0.
 */
static int validate_sg(const SupplyGroup *sg) {
    if (!sg) return 0;
    if (strlen(sg->sgc) != STS_SGC_LEN) return 0;
    for (size_t i = 0; i < STS_SGC_LEN; ++i)
        if (sg->sgc[i] < '0' || sg->sgc[i] > '9') return 0;
    return sg->krn >= 0 && sg->kt >= 0;
}

/*
 * Build the AES-GCM Additional Authenticated Data (AAD) for a supply group.
 * Output: Writes the formatted AAD to 'out' and returns its length.
 */
static size_t build_aad(const SupplyGroup *sg, unsigned char out[64]) {
    return (size_t)snprintf((char *)out, 64, "SGC=%s;KRN=%d;KT=%d", sg->sgc, sg->krn, sg->kt);
}

/*
 * Generate a fresh random vending key for the specified supply group.
 * Output: Writes STS_VK_LEN random bytes to 'vk' and returns 1 on success;
 *         returns 0 when validation or random generation fails.
 */
int kmc_generate_vending_key(const SupplyGroup *sg, unsigned char vk[STS_VK_LEN]) {
    if (!validate_sg(sg) || !vk) return 0;
    /* The VK is generated independently from SGC; SGC is authenticated as AAD. */
    return sts_random_bytes(vk, STS_VK_LEN);
}

/*
 * Derive the AES-256 transport key shared between the KMC and smart meter.
 * Output: Writes a STS_KEY_LEN-byte key to 'key' and returns 1 on success;
 *         returns 0 when validation, ECDH, randomness, or HKDF fails.
 */
int kmc_derive_transport_key(const SupplyGroup *sg,
                             const STSKeyPair *kmc_keypair,
                             const unsigned char *meter_public_der, size_t meter_public_len,
                             unsigned char key[STS_KEY_LEN]) {
    if (!validate_sg(sg) || !kmc_keypair || !meter_public_der || !key) return 0;
    unsigned char shared[128];
    size_t shared_len = sizeof(shared);
    unsigned char salt[32];
    if (!sts_random_bytes(salt, sizeof(salt))) return 0;
    /* Salt is not sent here, so it must be deterministic for this simulator. */
    memset(salt, 0, sizeof(salt));
    unsigned char info[128];
    size_t info_len = (size_t)snprintf((char *)info, sizeof(info),
                                       "STS-KMC-METER-AES256-GCM|SGC=%s|KRN=%d|KT=%d",
                                       sg->sgc, sg->krn, sg->kt);
    if (!sts_ecdh_derive(kmc_keypair, meter_public_der, meter_public_len, shared, &shared_len)) return 0;
    int ok = sts_hkdf_sha256(shared, shared_len, salt, sizeof(salt), info, info_len, key, STS_KEY_LEN);
    memset(shared, 0, sizeof(shared));
    return ok;
}

/*
 * Generate and encrypt a vending key for a smart meter using AES-256-GCM.
 * Output: Fills 'out' with the KMC public key, nonce, ciphertext, and tag;
 *         returns 1 on success and 0 on failure.
 */
int kmc_encrypt_vending_key(const SupplyGroup *sg,
                            const STSKeyPair *kmc_keypair,
                            const unsigned char *meter_public_der, size_t meter_public_len,
                            EncryptedVK *out) {
    if (!validate_sg(sg) || !kmc_keypair || !meter_public_der || !out) return 0;
    unsigned char vk[STS_VK_LEN];
    unsigned char key[STS_KEY_LEN];
    unsigned char aad[64];
    size_t aad_len = build_aad(sg, aad);
    if (!kmc_generate_vending_key(sg, vk)) return 0;
    if (!kmc_derive_transport_key(sg, kmc_keypair, meter_public_der, meter_public_len, key)) return 0;
    out->kmc_public_len = sizeof(out->kmc_public_der);
    if (!sts_keypair_public_der(kmc_keypair, out->kmc_public_der, &out->kmc_public_len)) { memset(key, 0, sizeof(key)); memset(vk, 0, sizeof(vk)); return 0; }
    if (!sts_random_bytes(out->nonce, sizeof(out->nonce))) { memset(key, 0, sizeof(key)); return 0; }
    out->ciphertext_len = 0;
    int ok = sts_aes256_gcm_encrypt(key, out->nonce, aad, aad_len, vk, sizeof(vk),
                                    out->ciphertext, &out->ciphertext_len, out->tag);
    memset(key, 0, sizeof(key));
    memset(vk, 0, sizeof(vk));
    return ok;
}

/*
 * Append an encrypted vending-key record to the KMC database file.
 * Output: Returns 1 when the record is encoded and written successfully;
 *         returns 0 for invalid input, encoding failure, or file-write failure.
 */
int kmc_database_append(const char *path, const SupplyGroup *sg, const EncryptedVK *encrypted) {
    if (!path || !validate_sg(sg) || !encrypted || encrypted->ciphertext_len > STS_VK_LEN) return 0;
    FILE *f = fopen(path, "a");
    if (!f) return 0;
    char nonce[STS_GCM_NONCE_LEN * 2 + 1];
    char cipher[STS_VK_LEN * 2 + 1];
    char tag[STS_GCM_TAG_LEN * 2 + 1];
    int ok = sts_hex_encode(encrypted->nonce, sizeof(encrypted->nonce), nonce, sizeof(nonce)) &&
             sts_hex_encode(encrypted->ciphertext, encrypted->ciphertext_len, cipher, sizeof(cipher)) &&
             sts_hex_encode(encrypted->tag, sizeof(encrypted->tag), tag, sizeof(tag));
    char kmc_pub[STS_EC_PUBLIC_KEY_LEN * 2 + 1];
    if (ok) ok = sts_hex_encode(encrypted->kmc_public_der, encrypted->kmc_public_len, kmc_pub, sizeof(kmc_pub));
    if (ok) ok = fprintf(f, "%s,%d,%d,AES-256-GCM,%s,%s,%s,%s\n", sg->sgc, sg->krn, sg->kt, kmc_pub, nonce, cipher, tag) > 0;
    fclose(f);
    return ok;
}

/*
 * Load the latest matching encrypted vending-key record from the database.
 * Output: Copies the last matching record into 'out' and returns 1 when found;
 *         returns 0 when no valid matching record can be loaded.
 */
int kmc_database_load_latest(const char *path, const SupplyGroup *sg, EncryptedVK *out) {
    if (!path || !validate_sg(sg) || !out) return 0;
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    char line[1024];
    EncryptedVK tmp;
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        char r_sgc[32], algo[32], kmc_pub[300], nonce[64], cipher[128], tag[64];
        int krn, kt;
        if (sscanf(line, "%31[^,],%d,%d,%31[^,],%299[^,],%63[^,],%127[^,],%63s", r_sgc, &krn, &kt, algo, kmc_pub, nonce, cipher, tag) != 8) continue;
        if (strcmp(r_sgc, sg->sgc) != 0 || krn != sg->krn || kt != sg->kt || strcmp(algo, "AES-256-GCM") != 0) continue;
        size_t np, n1, n2, n3;
        if (!sts_hex_decode(kmc_pub, tmp.kmc_public_der, sizeof(tmp.kmc_public_der), &np) || np == 0) continue;
        tmp.kmc_public_len = np;
        if (!sts_hex_decode(nonce, tmp.nonce, sizeof(tmp.nonce), &n1) || n1 != sizeof(tmp.nonce)) continue;
        if (!sts_hex_decode(cipher, tmp.ciphertext, sizeof(tmp.ciphertext), &n2) || n2 != STS_VK_LEN) continue;
        if (!sts_hex_decode(tag, tmp.tag, sizeof(tmp.tag), &n3) || n3 != sizeof(tmp.tag)) continue;
        tmp.ciphertext_len = n2;
        *out = tmp;
        found = 1;
    }
    fclose(f);
    return found;
}
