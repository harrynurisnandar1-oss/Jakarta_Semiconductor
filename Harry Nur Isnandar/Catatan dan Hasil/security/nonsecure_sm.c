#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    unsigned long long x;
    unsigned long long y;
} ECPoint;

// Deklarasi fungsi dari secure.c (Secure World / TrustZone)
extern ECPoint SECURE_GetOrCreateKeypair(const char *sgc, int krn, int kt, unsigned long long *out_prvkey);
extern void SECURE_DecryptPayload(const char *hex_cipher, unsigned long long prvkey, 
                                  ECPoint peer_pubkey, char *out_plain);

// Fungsi untuk menyimpan/memperbarui Public Key di sm_pubkey.env
void NONSECURE_SaveOrUpdatePubkey(const char *sgc, int krn, int kt, ECPoint pubkey) {
    char key_x_var[128], key_y_var[128];
    // Format label kunci menyertakan SGC, KRN, dan KT agar unik
    snprintf(key_x_var, sizeof(key_x_var), "SGC_%s_KRN_%d_KT_%d_SM_PUBKEY_X", sgc, krn, kt);
    snprintf(key_y_var, sizeof(key_y_var), "SGC_%s_KRN_%d_KT_%d_SM_PUBKEY_Y", sgc, krn, kt);

    FILE *pub_file = fopen("sm_pubkey.env", "r");
    char lines[100][256];
    int line_count = 0, x_found = 0, y_found = 0;

    if (pub_file != NULL) {
        while (fgets(lines[line_count], sizeof(lines[0]), pub_file) && line_count < 100) {
            lines[line_count][strcspn(lines[line_count], "\r\n")] = 0;
            if (strncmp(lines[line_count], key_x_var, strlen(key_x_var)) == 0) {
                snprintf(lines[line_count], sizeof(lines[0]), "%s=0x%LLX", key_x_var, pubkey.x);
                x_found = 1;
            } else if (strncmp(lines[line_count], key_y_var, strlen(key_y_var)) == 0) {
                snprintf(lines[line_count], sizeof(lines[0]), "%s=0x%LLX", key_y_var, pubkey.y);
                y_found = 1;
            }
            line_count++;
        }
        fclose(pub_file);
    }

    pub_file = fopen("sm_pubkey.env", "w");
    if (pub_file != NULL) {
        for (int i = 0; i < line_count; i++) {
            fprintf(pub_file, "%s\n", lines[i]);
        }
        if (!x_found) fprintf(pub_file, "%s=0x%LLX\n", key_x_var, pubkey.x);
        if (!y_found) fprintf(pub_file, "%s=0x%LLX\n", key_y_var, pubkey.y);
        fclose(pub_file);
        printf("[NON-SECURE] Public Key berhasil disimpan/diperbarui di 'sm_pubkey.env'\n");
    }
}

int main() {
    srand((unsigned int)time(NULL));

    printf("====================================================\n");
    printf("  SELECT RECORD (SGC, KRN, KT) FOR KEY GENERATION   \n");
    printf("====================================================\n");

    // 1. Input specific search criteria from the user.
    char target_sgc[10];
    int target_krn, target_kt;

    printf("Masukkan SGC  (contoh: 999999): ");
    scanf("%9s", target_sgc);
    printf("Masukkan KRN  (contoh: 1)     : ");
    scanf("%d", &target_krn);
    printf("Masukkan KT   (contoh: 2)     : ");
    scanf("%d", &target_kt);

    // 2. Seacrh SGC, KRN, KT in kmc_database.db
    FILE *db_file = fopen("kmc_database.db", "r");
    if (db_file == NULL) {
        printf("[-] Error: 'kmc_database.db' tidak ditemukan.\n");
        return 1;
    }

    char line[256], sgc[10], enc_vk[64];
    int krn, kt, found = 0;

    while (fgets(line, sizeof(line), db_file)) {
        line[strcspn(line, "\r\n")] = 0;
        if (sscanf(line, "%[^,],%d,%d,%s", sgc, &krn, &kt, enc_vk) == 4) {
            // Evaluasi ketiga parameter sekaligus
            if (strcmp(sgc, target_sgc) == 0 && krn == target_krn && kt == target_kt) {
                found = 1;
                break;
            }
        }
    }
    fclose(db_file);

    if (!found) {
        printf("[-] Record dengan SGC='%s', KRN=%d, KT=%d TIDAK ditemukan di kmc_database.db!\n", 
                target_sgc, target_krn, target_kt);
        return 1;
    }

    printf("\n[+] Record Cocok Ditemukan di Database!\n");
    printf("    Payload Encrypted VK: %s\n\n", enc_vk);

    // 3. Panggil Secure Function untuk Private Key
    unsigned long long sm_prvkey = 0;
    ECPoint sm_pubkey = SECURE_GetOrCreateKeypair(target_sgc, target_krn, target_kt, &sm_prvkey);

    // 4. Simpan Public Key di Non-Secure File
    NONSECURE_SaveOrUpdatePubkey(target_sgc, target_krn, target_kt, sm_pubkey);

    // 5. Dekripsi VK via Secure World
    ECPoint G = {0x6B17D1F2, 0x37B39D54};
    ECPoint kmc_pubkey = { (G.x * 12345 + 123) % 0xFFFFFFFFFFFFFFF1ULL, 
                           (G.y * 12345 + 987) % 0xFFFFFFFFFFFFFFF1ULL };

    char decrypted_vk[32];
    SECURE_DecryptPayload(enc_vk, sm_prvkey, kmc_pubkey, decrypted_vk);

    printf("\n[+] Hasil Akhir Non-Secure App:\n");
    printf("    -> Decrypted Plaintext VK: %s\n", decrypted_vk);
    printf("====================================================\n");

    return 0;
}