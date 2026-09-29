#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    unsigned long long x;
    unsigned long long y;
} ECPoint;

// External Secure Functions (NSC Gateways)
extern ECPoint SECURE_GetOrCreateKeypair(const char *sgc, unsigned long long *out_prvkey);
extern void SECURE_DecryptPayload(const char *hex_cipher, unsigned long long prvkey, 
                                  ECPoint peer_pubkey, char *out_plain);

// Update/Save Public Key ke Non-Secure Storage (sm_pubkey.env)
void NONSECURE_SaveOrUpdatePubkey(const char *sgc, ECPoint pubkey) {
    char key_x_var[64], key_y_var[64];
    snprintf(key_x_var, sizeof(key_x_var), "SGC_%s_SM_PUBKEY_X", sgc);
    snprintf(key_y_var, sizeof(key_y_var), "SGC_%s_SM_PUBKEY_Y", sgc);

    FILE *pub_file = fopen("sm_pubkey.env", "r");
    char lines[100][128];
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
        printf("[NON-SECURE] Public Key tersinkronisasi di 'sm_pubkey.env'\n");
    }
}

int main() {
    srand((unsigned int)time(NULL));

    printf("====================================================\n");
    printf("     NON-SECURE APPLICATION - TRUSTZONE DEMO        \n");
    printf("====================================================\n");

    char target_sgc[10];
    printf("Masukkan SGC yang ingin diproses (contoh: 112233): ");
    scanf("%9s", target_sgc);

    // 1. Cari SGC di kmc_database.db
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
            if (strcmp(sgc, target_sgc) == 0) {
                found = 1;
                break;
            }
        }
    }
    fclose(db_file);

    if (!found) {
        printf("[-] SGC '%s' tidak ditemukan dalam kmc_database.db.\n", target_sgc);
        return 1;
    }

    // 2. Panggil Secure Function via NSC Gate
    unsigned long long sm_prvkey = 0;
    ECPoint sm_pubkey = SECURE_GetOrCreateKeypair(target_sgc, &sm_prvkey);

    // 3. Simpan/Update Public Key di Non-Secure File
    NONSECURE_SaveOrUpdatePubkey(target_sgc, sm_pubkey);

    // Simulasi KMC Ephemeral Keypair
    ECPoint G = {0x6B17D1F2, 0x37B39D54};
    ECPoint kmc_pubkey = { (G.x * 12345 + 123) % 0xFFFFFFFFFFFFFFF1ULL, 
                           (G.y * 12345 + 987) % 0xFFFFFFFFFFFFFFF1ULL };

    // 4. Dekripsi via Secure Function
    char decrypted_vk[32];
    SECURE_DecryptPayload(enc_vk, sm_prvkey, kmc_pubkey, decrypted_vk);

    printf("\n[+] Hasil Akhir Non-Secure App:\n");
    printf("    -> Encrypted VK Cipher   : %s\n", enc_vk);
    printf("    -> Decrypted Plaintext VK: %s\n", decrypted_vk);
    printf("====================================================\n");

    return 0;
}