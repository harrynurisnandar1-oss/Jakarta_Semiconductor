#include "sts.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Print the command-line usage information for the KMC application.
 * Output: Writes the expected command syntax to standard error.
 */
static void usage(const char *p) {
    fprintf(stderr, "Usage: %s <SGC> <KRN> <KT> <meter_pubkey.db> <kmc_database.db>\n", p);
}

/*
 * Run the KMC vending-key generation, encryption, and database-storage flow.
 * Output: Prints operation status and returns 0 on success, 1 on operational
 *         failure, or 2 on invalid arguments.
 */
int main(int argc, char **argv) {
    if (argc != 6) { usage(argv[0]); return 2; }
    SupplyGroup sg = {0};
    strncpy(sg.sgc, argv[1], STS_SGC_LEN); sg.sgc[STS_SGC_LEN] = '\0';
    sg.krn = atoi(argv[2]); sg.kt = atoi(argv[3]);
    unsigned char meter_pub[STS_EC_PUBLIC_KEY_LEN];
    size_t meter_pub_len = sizeof(meter_pub);
    if (!nonsecure_load_public_key(argv[4], &sg, meter_pub, &meter_pub_len)) {
        fprintf(stderr, "ERROR: meter public key not found.\n"); return 1;
    }
    STSKeyPair *kmc = NULL;
    if (!sts_ecdh_generate(&kmc)) { fprintf(stderr, "ERROR: KMC key generation failed.\n"); return 1; }
    EncryptedVK record = {0};
    if (!kmc_encrypt_vending_key(&sg, kmc, meter_pub, meter_pub_len, &record)) {
        fprintf(stderr, "ERROR: VK encryption failed.\n"); sts_keypair_free(kmc); return 1;
    }
    if (!kmc_database_append(argv[5], &sg, &record)) {
        fprintf(stderr, "ERROR: database write failed.\n"); sts_keypair_free(kmc); return 1;
    }
    char cipher[STS_VK_LEN * 2 + 1];
    sts_hex_encode(record.ciphertext, record.ciphertext_len, cipher, sizeof(cipher));
    printf("KMC: Supply Group = %s / KRN=%d / KT=%d\n", sg.sgc, sg.krn, sg.kt);
    printf("KMC: AES-256-GCM ciphertext = %s\n", cipher);
    printf("KMC: encrypted VK record stored in %s\n", argv[5]);
    sts_keypair_free(kmc);
    return 0;
}
