#include "sts.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Print the command-line usage information for the non-secure application.
 * Output: Writes the accepted command syntax to standard error.
 */
static void usage(const char *p) {
    fprintf(stderr,
        "Usage:\n"
        "  %s provision <secure_key_file> <public_key_file> <SGC> <KRN> <KT>\n"
        "  %s decrypt   <secure_key_file> <kmc_database.db> <SGC> <KRN> <KT>\n", p, p);
}

/*
 * Build a SupplyGroup structure from command-line string values.
 * Output: Initializes 'sg' and returns 1 when the SGC has the required length;
 *         returns 0 otherwise.
 */
static int build_sg(const char *sgc, const char *krn, const char *kt, SupplyGroup *sg) {
    memset(sg, 0, sizeof(*sg));
    strncpy(sg->sgc, sgc, STS_SGC_LEN); sg->sgc[STS_SGC_LEN] = '\0';
    sg->krn = atoi(krn); sg->kt = atoi(kt);
    return strlen(sg->sgc) == STS_SGC_LEN;
}

/*
 * Run the non-secure application commands for provisioning or decryption.
 * Output: Prints operation status and returns 0 on success, 1 on operational
 *         failure, or 2 on invalid usage.
 */
int main(int argc, char **argv) {
    if (argc != 7) { usage(argv[0]); return 2; }
    SupplyGroup sg;
    if (!build_sg(argv[4], argv[5], argv[6], &sg)) return 2;
    if (!secure_sm_init(argv[2])) { fprintf(stderr, "ERROR: Secure World initialization failed. Set STS_STORAGE_KEY_HEX to a 64-hex-character secret.\n"); return 1; }
    if (strcmp(argv[1], "provision") == 0) {
        unsigned char pub[STS_EC_PUBLIC_KEY_LEN]; size_t len = sizeof(pub);
        int ok = secure_sm_get_public_key(pub, &len) && nonsecure_save_public_key(argv[3], &sg, pub, len);
        if (ok) printf("Secure World provisioned. Public key stored in %s\n", argv[3]);
        else fprintf(stderr, "ERROR: provisioning failed.\n");
        secure_sm_shutdown();
        return ok ? 0 : 1;
    }
    if (strcmp(argv[1], "decrypt") == 0) {
        EncryptedVK record;
        if (!kmc_database_load_latest(argv[3], &sg, &record)) { fprintf(stderr, "ERROR: encrypted VK record not found.\n"); secure_sm_shutdown(); return 1; }
        unsigned char vk[STS_VK_LEN]; size_t vk_len = sizeof(vk);
        int ok = secure_sm_decrypt_vending_key(&sg, record.kmc_public_der, record.kmc_public_len, &record, vk, &vk_len);
        if (!ok) fprintf(stderr, "AUTHENTICATION/DECRYPTION FAILED\n");
        else { char hex[STS_VK_LEN*2+1]; sts_hex_encode(vk, vk_len, hex, sizeof(hex)); printf("Decrypted VK = %s\n", hex); memset(vk, 0, sizeof(vk)); }
        secure_sm_shutdown();
        return ok ? 0 : 1;
    }
    usage(argv[0]); secure_sm_shutdown(); return 2;
}
