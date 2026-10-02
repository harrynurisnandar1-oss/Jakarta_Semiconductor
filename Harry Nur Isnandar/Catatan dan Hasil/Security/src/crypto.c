#include "sts.h"

#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <string.h>
#include <stdlib.h>

struct sts_keypair { EVP_PKEY *pkey; };

/*
 * Generate cryptographically secure random bytes using OpenSSL RAND_bytes.
 * Output: Fills 'out' with 'len' random bytes and returns 1 on success;
 *         returns 0 for invalid input or random-generation failure.
 */
int sts_random_bytes(unsigned char *out, size_t len) {
    if (!out || len == 0) return 0;
    return RAND_bytes(out, (int)len) == 1;
}

/*
 * Encrypt plaintext with AES-256-GCM and produce an authentication tag.
 * Output: Writes ciphertext to 'ciphertext', its length to '*ciphertext_len',
 *         and the authentication tag to 'tag'; returns 1 on success or 0 on failure.
 */
int sts_aes256_gcm_encrypt(const unsigned char key[STS_KEY_LEN],
                           const unsigned char nonce[STS_GCM_NONCE_LEN],
                           const unsigned char *aad, size_t aad_len,
                           const unsigned char *plaintext, size_t plaintext_len,
                           unsigned char *ciphertext, size_t *ciphertext_len,
                           unsigned char tag[STS_GCM_TAG_LEN]) {
    if (!key || !nonce || (!plaintext && plaintext_len) || !ciphertext || !ciphertext_len || !tag) return 0;
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return 0;
    int len = 0, total = 0, ok = 0;
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) goto done;
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, STS_GCM_NONCE_LEN, NULL) != 1) goto done;
    if (EVP_EncryptInit_ex(ctx, NULL, NULL, key, nonce) != 1) goto done;
    if (aad_len && (!aad || EVP_EncryptUpdate(ctx, NULL, &len, aad, (int)aad_len) != 1)) goto done;
    if (plaintext_len && EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, (int)plaintext_len) != 1) goto done;
    total = len;
    if (EVP_EncryptFinal_ex(ctx, ciphertext + total, &len) != 1) goto done;
    total += len;
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, STS_GCM_TAG_LEN, tag) != 1) goto done;
    *ciphertext_len = (size_t)total;
    ok = 1;
done:
    EVP_CIPHER_CTX_free(ctx);
    return ok;
}

/*
 * Authenticate and decrypt AES-256-GCM ciphertext.
 * Output: Writes plaintext to 'plaintext', its length to '*plaintext_len',
 *         and returns 1 only when authentication/decryption succeeds. On
 *         failure, the plaintext buffer is cleared and 0 is returned.
 */
int sts_aes256_gcm_decrypt(const unsigned char key[STS_KEY_LEN],
                           const unsigned char nonce[STS_GCM_NONCE_LEN],
                           const unsigned char *aad, size_t aad_len,
                           const unsigned char *ciphertext, size_t ciphertext_len,
                           const unsigned char tag[STS_GCM_TAG_LEN],
                           unsigned char *plaintext, size_t *plaintext_len) {
    if (!key || !nonce || (!ciphertext && ciphertext_len) || !tag || !plaintext || !plaintext_len) return 0;
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return 0;
    int len = 0, total = 0, ok = 0;
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) goto done;
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, STS_GCM_NONCE_LEN, NULL) != 1) goto done;
    if (EVP_DecryptInit_ex(ctx, NULL, NULL, key, nonce) != 1) goto done;
    if (aad_len && (!aad || EVP_DecryptUpdate(ctx, NULL, &len, aad, (int)aad_len) != 1)) goto done;
    if (ciphertext_len && EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, (int)ciphertext_len) != 1) goto done;
    total = len;
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, STS_GCM_TAG_LEN, (void *)tag) != 1) goto done;
    if (EVP_DecryptFinal_ex(ctx, plaintext + total, &len) != 1) goto done;
    total += len;
    *plaintext_len = (size_t)total;
    ok = 1;
done:
    if (!ok && plaintext && ciphertext_len) memset(plaintext, 0, ciphertext_len);
    EVP_CIPHER_CTX_free(ctx);
    return ok;
}

/*
 * Generate a P-256 elliptic-curve Diffie-Hellman key pair.
 * Output: Allocates an STSKeyPair and stores it through '*out'; returns 1
 *         on success and 0 on allocation or OpenSSL failure.
 */
int sts_ecdh_generate(STSKeyPair **out) {
    if (!out) return 0;
    *out = NULL;
    STSKeyPair *kp = calloc(1, sizeof(*kp));
    if (!kp) return 0;
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
    if (!ctx) { free(kp); return 0; }
    int ok = 0;
    if (EVP_PKEY_keygen_init(ctx) != 1) goto done;
    if (EVP_PKEY_CTX_set_group_name(ctx, "prime256v1") != 1) goto done;
    if (EVP_PKEY_generate(ctx, &kp->pkey) != 1) goto done;
    *out = kp;
    kp = NULL;
    ok = 1;
done:
    EVP_PKEY_CTX_free(ctx);
    if (kp) { EVP_PKEY_free(kp->pkey); free(kp); }
    return ok;
}

/*
 * Release an STSKeyPair and its underlying OpenSSL key.
 * Output: Frees the supplied key pair and returns void.
 */
void sts_keypair_free(STSKeyPair *kp) {
    if (!kp) return;
    EVP_PKEY_free(kp->pkey);
    free(kp);
}

/*
 * Export an STS public key in DER format.
 * Output: Writes DER bytes to 'out' and updates '*out_len'; when 'out' is
 *         NULL, only the required length is returned. Returns 1 on success.
 */
int sts_keypair_public_der(const STSKeyPair *kp, unsigned char *out, size_t *out_len) {
    if (!kp || !kp->pkey || !out_len) return 0;
    int len = i2d_PUBKEY(kp->pkey, NULL);
    if (len <= 0) return 0;
    if (!out) { *out_len = (size_t)len; return 1; }
    if (*out_len < (size_t)len) return 0;
    unsigned char *p = out;
    if (i2d_PUBKEY(kp->pkey, &p) != len) return 0;
    *out_len = (size_t)len;
    return 1;
}

/*
 * Export an STS private key in DER format.
 * Output: Writes DER bytes to 'out' and updates '*out_len'; when 'out' is
 *         NULL, only the required length is returned. Returns 1 on success.
 */
int sts_keypair_private_der(const STSKeyPair *kp, unsigned char *out, size_t *out_len) {
    if (!kp || !kp->pkey || !out_len) return 0;
    int len = i2d_PrivateKey(kp->pkey, NULL);
    if (len <= 0) return 0;
    if (!out) { *out_len = (size_t)len; return 1; }
    if (*out_len < (size_t)len) return 0;
    unsigned char *p = out;
    if (i2d_PrivateKey(kp->pkey, &p) != len) return 0;
    *out_len = (size_t)len;
    return 1;
}

/*
 * Reconstruct an STSKeyPair from DER-encoded private-key data.
 * Output: Allocates and stores the reconstructed key through '*out' and
 *         returns 1 on success; returns 0 when decoding or allocation fails.
 */
int sts_keypair_from_private_der(const unsigned char *der, size_t der_len, STSKeyPair **out) {
    if (!der || !out) return 0;
    *out = NULL;
    const unsigned char *p = der;
    EVP_PKEY *pkey = d2i_AutoPrivateKey(NULL, &p, (long)der_len);
    if (!pkey) return 0;
    STSKeyPair *kp = calloc(1, sizeof(*kp));
    if (!kp) { EVP_PKEY_free(pkey); return 0; }
    kp->pkey = pkey;
    *out = kp;
    return 1;
}

/*
 * Derive an ECDH shared secret from a private key and a peer public key.
 * Output: Writes the shared secret to 'shared_secret', updates
 *         '*shared_secret_len', and returns 1 on success; returns 0 on failure.
 */
int sts_ecdh_derive(const STSKeyPair *private_kp,
                    const unsigned char *peer_public_der, size_t peer_public_len,
                    unsigned char *shared_secret, size_t *shared_secret_len) {
    if (!private_kp || !private_kp->pkey || !peer_public_der || !shared_secret_len) return 0;
    const unsigned char *p = peer_public_der;
    EVP_PKEY *peer = d2i_PUBKEY(NULL, &p, (long)peer_public_len);
    if (!peer) return 0;
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(private_kp->pkey, NULL);
    if (!ctx) { EVP_PKEY_free(peer); return 0; }
    int ok = 0;
    size_t len = 0;
    if (EVP_PKEY_derive_init(ctx) != 1) goto done;
    if (EVP_PKEY_derive_set_peer(ctx, peer) != 1) goto done;
    if (EVP_PKEY_derive(ctx, NULL, &len) != 1) goto done;
    if (!shared_secret) { *shared_secret_len = len; ok = 1; goto done; }
    if (*shared_secret_len < len) goto done;
    if (EVP_PKEY_derive(ctx, shared_secret, &len) != 1) goto done;
    *shared_secret_len = len;
    ok = 1;
done:
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(peer);
    return ok;
}

/*
 * Derive key material with HKDF using SHA-256.
 * Output: Writes 'out_len' derived bytes to 'out' and returns 1 when HKDF
 *         succeeds; returns 0 for invalid input or OpenSSL failure.
 */
int sts_hkdf_sha256(const unsigned char *ikm, size_t ikm_len,
                    const unsigned char *salt, size_t salt_len,
                    const unsigned char *info, size_t info_len,
                    unsigned char *out, size_t out_len) {
    if (!ikm || !out || out_len == 0) return 0;
    EVP_KDF *kdf = EVP_KDF_fetch(NULL, "HKDF", NULL);
    EVP_KDF_CTX *ctx = kdf ? EVP_KDF_CTX_new(kdf) : NULL;
    if (!kdf || !ctx) { EVP_KDF_free(kdf); EVP_KDF_CTX_free(ctx); return 0; }
    int ok = 0;
    OSSL_PARAM params[6];
    size_t n = 0;
    const char *digest = "SHA256";
    const char mode[] = "EXTRACT_AND_EXPAND";
    params[n++] = OSSL_PARAM_construct_utf8_string("digest", (char *)digest, 0);
    params[n++] = OSSL_PARAM_construct_utf8_string("mode", (char *)mode, 0);
    params[n++] = OSSL_PARAM_construct_octet_string("key", (void *)ikm, ikm_len);
    if (salt && salt_len) params[n++] = OSSL_PARAM_construct_octet_string("salt", (void *)salt, salt_len);
    if (info && info_len) params[n++] = OSSL_PARAM_construct_octet_string("info", (void *)info, info_len);
    params[n] = OSSL_PARAM_construct_end();
    if (EVP_KDF_derive(ctx, out, out_len, params) == 1) ok = 1;
    EVP_KDF_CTX_free(ctx);
    EVP_KDF_free(kdf);
    return ok;
}

/*
 * Encode binary data as an uppercase hexadecimal string.
 * Output: Writes a null-terminated hexadecimal string to 'out' and returns
 *         1 on success; returns 0 when the output buffer is invalid or too small.
 */
int sts_hex_encode(const unsigned char *in, size_t len, char *out, size_t out_size) {
    static const char hex[] = "0123456789ABCDEF";
    if (!in || !out || out_size < len * 2 + 1) return 0;
    for (size_t i = 0; i < len; ++i) { out[2*i] = hex[in[i] >> 4]; out[2*i+1] = hex[in[i] & 0x0F]; }
    out[len * 2] = '\0';
    return 1;
}

/*
 * Convert one hexadecimal character to its numeric value.
 * Output: Returns 0 through 15 for a valid hexadecimal digit, or -1 otherwise.
 */
static int hexval(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

/*
 * Decode a hexadecimal string into binary bytes.
 * Output: Writes decoded bytes to 'out', stores the byte count in '*out_len',
 *         and returns 1 on success; returns 0 for invalid input or bad sizing.
 */
int sts_hex_decode(const char *hex, unsigned char *out, size_t out_size, size_t *out_len) {
    if (!hex || !out || !out_len) return 0;
    size_t n = strlen(hex);
    if ((n & 1u) || n / 2 > out_size) return 0;
    for (size_t i = 0; i < n; i += 2) {
        int a = hexval(hex[i]), b = hexval(hex[i+1]);
        if (a < 0 || b < 0) return 0;
        out[i/2] = (unsigned char)((a << 4) | b);
    }
    *out_len = n / 2;
    return 1;
}
