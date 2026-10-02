#ifndef STS_H
#define STS_H

#include <stddef.h>
#include <stdint.h>

#define STS_KEY_LEN 32
#define STS_GCM_NONCE_LEN 12
#define STS_GCM_TAG_LEN 16
#define STS_VK_LEN 16
#define STS_MAX_HEX 4096
#define STS_SGC_LEN 6

/* Opaque public-key encoding. P-256 uncompressed point = 65 bytes. */
#define STS_EC_PUBLIC_KEY_LEN 128

typedef struct {
    char sgc[STS_SGC_LEN + 1];
    int krn;
    int kt;
} SupplyGroup;

typedef struct {
    unsigned char kmc_public_der[STS_EC_PUBLIC_KEY_LEN];
    size_t kmc_public_len;
    unsigned char nonce[STS_GCM_NONCE_LEN];
    unsigned char ciphertext[STS_VK_LEN];
    unsigned char tag[STS_GCM_TAG_LEN];
    size_t ciphertext_len;
} EncryptedVK;

/* ---------------- Cryptographic primitives ---------------- */
/* Generate secure random bytes. Output: fills 'out'; returns 1 on success, 0 on failure. */
int sts_random_bytes(unsigned char *out, size_t len);

/* Encrypt with AES-256-GCM. Output: ciphertext length and tag; returns 1/0. */
int sts_aes256_gcm_encrypt(const unsigned char key[STS_KEY_LEN],
                           const unsigned char nonce[STS_GCM_NONCE_LEN],
                           const unsigned char *aad, size_t aad_len,
                           const unsigned char *plaintext, size_t plaintext_len,
                           unsigned char *ciphertext, size_t *ciphertext_len,
                           unsigned char tag[STS_GCM_TAG_LEN]);

/* Decrypt/authenticate AES-256-GCM data. Output: plaintext and length; returns 1/0. */
int sts_aes256_gcm_decrypt(const unsigned char key[STS_KEY_LEN],
                           const unsigned char nonce[STS_GCM_NONCE_LEN],
                           const unsigned char *aad, size_t aad_len,
                           const unsigned char *ciphertext, size_t ciphertext_len,
                           const unsigned char tag[STS_GCM_TAG_LEN],
                           unsigned char *plaintext, size_t *plaintext_len);

/* P-256 ECDH. Private keys are opaque EVP_PKEY objects. */
typedef struct sts_keypair STSKeyPair;

/* Generate a P-256 ECDH key pair. Output: key through '*out'; returns 1/0. */
int sts_ecdh_generate(STSKeyPair **out);
/* Free an ECDH key pair. Output: releases the key; returns void. */
void sts_keypair_free(STSKeyPair *kp);
/* Export a public key as DER. Output: bytes and length; returns 1/0. */
int sts_keypair_public_der(const STSKeyPair *kp, unsigned char *out, size_t *out_len);
/* Export a private key as DER. Output: bytes and length; returns 1/0. */
int sts_keypair_private_der(const STSKeyPair *kp, unsigned char *out, size_t *out_len);
/* Reconstruct a key pair from private DER. Output: key through '*out'; returns 1/0. */
int sts_keypair_from_private_der(const unsigned char *der, size_t der_len, STSKeyPair **out);
/* Derive an ECDH shared secret. Output: secret and length; returns 1/0. */
int sts_ecdh_derive(const STSKeyPair *private_kp,
                    const unsigned char *peer_public_der, size_t peer_public_len,
                    unsigned char *shared_secret, size_t *shared_secret_len);

/* Derive key material with HKDF-SHA256. Output: bytes in 'out'; returns 1/0. */
int sts_hkdf_sha256(const unsigned char *ikm, size_t ikm_len,
                    const unsigned char *salt, size_t salt_len,
                    const unsigned char *info, size_t info_len,
                    unsigned char *out, size_t out_len);

/* ---------------- KMC ---------------- */
/* Generate a vending key. Output: STS_VK_LEN bytes in 'vk'; returns 1/0. */
int kmc_generate_vending_key(const SupplyGroup *sg, unsigned char vk[STS_VK_LEN]);
/* Encrypt a vending key. Output: populated EncryptedVK in 'out'; returns 1/0. */
int kmc_encrypt_vending_key(const SupplyGroup *sg,
                            const STSKeyPair *kmc_keypair,
                            const unsigned char *meter_public_der, size_t meter_public_len,
                            EncryptedVK *out);

/* Derive the transport key used by KMC. */
/* Derive the transport key. Output: STS_KEY_LEN bytes in 'key'; returns 1/0. */
int kmc_derive_transport_key(const SupplyGroup *sg,
                             const STSKeyPair *kmc_keypair,
                             const unsigned char *meter_public_der, size_t meter_public_len,
                             unsigned char key[STS_KEY_LEN]);

/* ---------------- Secure Smart Meter API ---------------- */
/* No API returns the meter private key. */
/* Initialize secure-meter state. Output: meter key is loaded/generated; returns 1/0. */
int secure_sm_init(const char *storage_path);
/* Shut down secure-meter state. Output: releases key state; returns void. */
void secure_sm_shutdown(void);
/* Get the meter public key. Output: DER bytes and length; returns 1/0. */
int secure_sm_get_public_key(unsigned char out[STS_EC_PUBLIC_KEY_LEN], size_t *out_len);
/* Decrypt the vending key. Output: plaintext VK and length; returns 1/0. */
int secure_sm_decrypt_vending_key(const SupplyGroup *sg,
                                  const unsigned char *kmc_public_der, size_t kmc_public_len,
                                  const EncryptedVK *encrypted,
                                  unsigned char out_vk[STS_VK_LEN], size_t *out_len);

/* ---------------- Non-secure storage ---------------- */
/* Store a meter public key. Output: database record is appended; returns 1/0. */
int nonsecure_save_public_key(const char *path, const SupplyGroup *sg,
                              const unsigned char *public_der, size_t public_len);
/* Load a meter public key. Output: decoded key and length; returns 1/0. */
int nonsecure_load_public_key(const char *path, const SupplyGroup *sg,
                              unsigned char *out, size_t *out_len);

/* ---------------- KMC database ---------------- */
/* Append an encrypted VK record. Output: database is updated; returns 1/0. */
int kmc_database_append(const char *path, const SupplyGroup *sg,
                        const EncryptedVK *encrypted);
/* Load the latest matching VK record. Output: record in 'out'; returns 1/0. */
int kmc_database_load_latest(const char *path, const SupplyGroup *sg,
                             EncryptedVK *out);

/* Encode bytes as hexadecimal. Output: null-terminated string; returns 1/0. */
int sts_hex_encode(const unsigned char *in, size_t len, char *out, size_t out_size);
/* Decode hexadecimal. Output: bytes and length; returns 1/0. */
int sts_hex_decode(const char *hex, unsigned char *out, size_t out_size, size_t *out_len);

#endif
