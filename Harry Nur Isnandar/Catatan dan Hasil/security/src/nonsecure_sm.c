#include "sts.h"

#include <stdio.h>
#include <string.h>

/*
 * Validate the fields of a SupplyGroup used by non-secure storage.
 * Output: Returns 1 for a valid SGC, KRN, and KT; otherwise returns 0.
 */
static int valid_sg(const SupplyGroup *sg) {
    if (!sg || strlen(sg->sgc) != STS_SGC_LEN) return 0;
    for (size_t i = 0; i < STS_SGC_LEN; ++i) if (sg->sgc[i] < '0' || sg->sgc[i] > '9') return 0;
    return sg->krn >= 0 && sg->kt >= 0;
}

/*
 * Append a smart-meter public key record to the non-secure key database.
 * Output: Returns 1 when the key is encoded and stored successfully;
 *         returns 0 when validation, encoding, or file writing fails.
 */
int nonsecure_save_public_key(const char *path, const SupplyGroup *sg,
                              const unsigned char *public_der, size_t public_len) {
    if (!path || !valid_sg(sg) || !public_der || public_len == 0 || public_len > STS_EC_PUBLIC_KEY_LEN) return 0;
    FILE *f = fopen(path, "a");
    if (!f) return 0;
    char hex[STS_EC_PUBLIC_KEY_LEN * 2 + 1];
    int ok = sts_hex_encode(public_der, public_len, hex, sizeof(hex));
    if (ok) ok = fprintf(f, "%s,%d,%d,%s\n", sg->sgc, sg->krn, sg->kt, hex) > 0;
    fclose(f);
    return ok;
}

/*
 * Find and decode the public key belonging to a specific supply group.
 * Output: Writes the decoded key to 'out', updates '*out_len', and returns 1
 *         when a matching valid record is found; otherwise returns 0.
 */
int nonsecure_load_public_key(const char *path, const SupplyGroup *sg,
                              unsigned char *out, size_t *out_len) {
    if (!path || !valid_sg(sg) || !out || !out_len) return 0;
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    char line[512], r_sgc[32], hex[300];
    int r_krn, r_kt;
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "%31[^,],%d,%d,%299s", r_sgc, &r_krn, &r_kt, hex) != 4) continue;
        if (strcmp(r_sgc, sg->sgc) == 0 && r_krn == sg->krn && r_kt == sg->kt) {
            if (sts_hex_decode(hex, out, *out_len, out_len)) found = 1;
        }
    }
    fclose(f);
    return found;
}
