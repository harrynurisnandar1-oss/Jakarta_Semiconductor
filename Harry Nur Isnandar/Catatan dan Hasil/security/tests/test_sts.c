#define _POSIX_C_SOURCE 200809L
#include "sts.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static int failures = 0;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL: %s:%d: %s\n", __FILE__, __LINE__, #x); failures++; } } while (0)

/*
 * Configure the test storage key in the process environment.
 * Output: Sets STS_STORAGE_KEY_HEX and returns void.
 */
static void set_storage_key(void) {
#ifdef _WIN32
    _putenv_s("STS_STORAGE_KEY_HEX", "00112233445566778899AABBCCDDEEFF00112233445566778899AABBCCDDEEFF");
#else
    setenv("STS_STORAGE_KEY_HEX", "00112233445566778899AABBCCDDEEFF00112233445566778899AABBCCDDEEFF", 1);
#endif
}

/*
 * Execute the end-to-end STS cryptographic test sequence.
 * Output: Prints a success message when all assertions pass; otherwise prints
 *         the number of failed assertions and returns 1. Returns 0 on success.
 */
int main(void) {
    set_storage_key();
    const char *keyfile = "test_meter_secure.key";
    const char *pubfile = "test_public.db";
    const char *dbfile = "test_kmc.db";
    remove(keyfile); remove(pubfile); remove(dbfile);

    SupplyGroup sg = {"999999", 1, 2};
    STSKeyPair *kmc = NULL;
    CHECK(sts_ecdh_generate(&kmc));

    CHECK(secure_sm_init(keyfile));
    unsigned char meter_pub[STS_EC_PUBLIC_KEY_LEN]; size_t meter_pub_len = sizeof(meter_pub);
    CHECK(secure_sm_get_public_key(meter_pub, &meter_pub_len));
    CHECK(nonsecure_save_public_key(pubfile, &sg, meter_pub, meter_pub_len));
    secure_sm_shutdown();

    unsigned char loaded_pub[STS_EC_PUBLIC_KEY_LEN]; size_t loaded_len = sizeof(loaded_pub);
    CHECK(nonsecure_load_public_key(pubfile, &sg, loaded_pub, &loaded_len));
    CHECK(loaded_len == meter_pub_len);
    CHECK(memcmp(loaded_pub, meter_pub, meter_pub_len) == 0);

    EncryptedVK enc = {0};
    CHECK(kmc_encrypt_vending_key(&sg, kmc, loaded_pub, loaded_len, &enc));
    CHECK(enc.ciphertext_len == STS_VK_LEN);
    CHECK(enc.kmc_public_len > 0);
    CHECK(kmc_database_append(dbfile, &sg, &enc));

    EncryptedVK dbrec = {0};
    CHECK(kmc_database_load_latest(dbfile, &sg, &dbrec));
    CHECK(dbrec.ciphertext_len == STS_VK_LEN);

    CHECK(secure_sm_init(keyfile));
    unsigned char vk[STS_VK_LEN]; size_t vk_len = sizeof(vk);
    CHECK(secure_sm_decrypt_vending_key(&sg, dbrec.kmc_public_der, dbrec.kmc_public_len, &dbrec, vk, &vk_len));
    CHECK(vk_len == STS_VK_LEN);

    dbrec.tag[0] ^= 0x01;
    vk_len = sizeof(vk);
    CHECK(!secure_sm_decrypt_vending_key(&sg, dbrec.kmc_public_der, dbrec.kmc_public_len, &dbrec, vk, &vk_len));
    secure_sm_shutdown();

    sts_keypair_free(kmc);
    remove(keyfile); remove(pubfile); remove(dbfile);

    if (failures) { fprintf(stderr, "%d test assertion(s) failed.\n", failures); return 1; }
    printf("All STS cryptographic tests passed.\n");
    return 0;
}
