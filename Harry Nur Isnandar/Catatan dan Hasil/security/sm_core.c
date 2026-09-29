#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct to store records from the database
typedef struct {
    char sgc[7];
    int krn;
    int kt;
    char enc_hex[64];
} KMCRecord;

// ... [Include ECDH & decrypt_simulated_aes192 functions from the previous code] ...

int main(int argc, char *argv[]) {
    char target_sgc[7];

    // 1. Get the target SGC to search for
    if (argc == 2) {
        // Read from CLI arguments (e.g., .\sm_sim.exe 999999)
        strncpy(target_sgc, argv[1], 6);
        target_sgc[6] = '\0';
    } else {
        // Manual input via Terminal
        printf("Masukkan SGC yang ingin diproses oleh SM (contoh: 999999): ");
        scanf("%6s", target_sgc);
    }

    // 2. Search for Data in kmc_database.db
    FILE *db_file = fopen("kmc_database.db", "r");
    if (db_file == NULL) {
        printf("[-] Gagal membuka file kmc_database.db!\n");
        return 1;
    }

    KMCRecord rec;
    int found = 0;
    char line[256];

    // Read line-by-line and match SGC
    while (fgets(line, sizeof(line), db_file)) {
        if (sscanf(line, "%6[^,],%d,%d,%s", rec.sgc, &rec.krn, &rec.kt, rec.enc_hex) == 4) {
            if (strcmp(rec.sgc, target_sgc) == 0) {
                found = 1;
                break; // Data found, exit loop
            }
        }
    }
    fclose(db_file);

    // 3. Validate Search Results
    if (!found) {
        printf("[-] Data dengan SGC '%s' tidak ditemukan di database!\n", target_sgc);
        return 1;
    }

    printf("\n[+] Data Ditemukan di Database:\n");
    printf("    - SGC : %s\n", rec.sgc);
    printf("    - KRN : %d\n", rec.krn);
    printf("    - KT  : %d\n", rec.kt);
    printf("    - Encrypted VK (Hex) : %s\n", rec.enc_hex);

    // 4. Vending Key (VK) Decryption Process
    unsigned char smk[24] = "MasterKeyKMC192BitSecret";
    size_t hex_len = strlen(rec.enc_hex) / 2;
    
    unsigned char vk_encrypted_bytes[32];
    unsigned char vk_decrypted[32];
    
    // Convert Hex to Bytes & Decrypt
    for (size_t i = 0; i < hex_len; i++) {
        sscanf(rec.enc_hex + 2 * i, "%02hhX", &vk_encrypted_bytes[i]);
    }
    
    for (size_t i = 0; i < hex_len; i++) {
        vk_decrypted[i] = vk_encrypted_bytes[i] ^ smk[i % 24];
    }
    vk_decrypted[hex_len] = '\0';

    printf("\n[+] Success! Decrypted Vending Key (VK): %s\n", vk_decrypted);

    return 0;
}