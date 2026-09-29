// KEY MANAGEMENT CENTER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Struct untuk Supply Group (SGC, KRN, KT)
typedef struct {
    char sgc[7];  // Supply Group Code (6 digit)
    int krn;      // Key Revision Number
    int kt;       // Key Type
} SupplyGroup;

// Generasi random bytes
void generate_random_bytes(unsigned char *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        buf[i] = (unsigned char)(rand() % 256);
    }
}

// Simulasi Enkripsi AES-192-like Masking (24-byte SMK)
void encrypt_simulated_aes192(const unsigned char *input, size_t in_len, 
                              const unsigned char *key, unsigned char *output) {
    for (size_t i = 0; i < in_len; i++) {
        output[i] = input[i] ^ key[i % 24];
    }
}

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));
    SupplyGroup sg;

    // 1. Dapatkan Input: Dari CLI Arguments ATAU Terminal Interactive Input
    if (argc == 4) {
        // Opsi A: Mengambil dari Command Line Argument (misal: .\kmc_sim.exe 999999 1 2)
        strncpy(sg.sgc, argv[1], 6);
        sg.sgc[6] = '\0';
        sg.krn = atoi(argv[2]);
        sg.kt = atoi(argv[3]);
        printf("[+] Menggunakan input dari Argumen CLI.\n");
    } else {
        // Opsi B: Meminta Input Interaktif via Terminal
        printf("=== INPUT PARAMETER SUPPLY GROUP ===\n");
        printf("Masukkan SGC (6 Digit Code, contoh: 999999): ");
        scanf("%6s", sg.sgc);

        printf("Masukkan KRN (Key Revision Number, contoh: 1): ");
        scanf("%d", &sg.krn);

        printf("Masukkan KT  (Key Type, contoh: 2): ");
        scanf("%d", &sg.kt);
        printf("------------------------------------\n");
    }

    // System Master Key (SMK) 192-bit (24 Bytes)
    unsigned char smk[24] = "MasterKeyKMC192BitSecret";

    // 2. Generasi Data Acak (rand) & Vending Key (VK)
    unsigned char rand_val[2];
    generate_random_bytes(rand_val, sizeof(rand_val));

    char vk_plain[16];
    snprintf(vk_plain, sizeof(vk_plain), "%s%02X%02X", sg.sgc, rand_val[0], rand_val[1]);

    printf("\n=== KMC (Key Management Centre) ===\n");
    printf("[1] Supply Group Params -> SGC: %s | KRN: %d | KT: %d\n", sg.sgc, sg.krn, sg.kt);
    printf("[2] Raw Vending Key (VK Plaintext): %s\n", vk_plain);

    // 3. Enkripsi VK dengan SMK
    size_t vk_len = strlen(vk_plain);
    unsigned char vk_encrypted[16];
    encrypt_simulated_aes192((unsigned char*)vk_plain, vk_len, smk, vk_encrypted);

    printf("[3] Encrypted VK (Hex): ");
    for (size_t i = 0; i < vk_len; i++) {
        printf("%02X", vk_encrypted[i]);
    }
    printf("\n");

    // 4. Simpan ke File Database (kmc_database.db)
    FILE *db_file = fopen("kmc_database.db", "a");
    if (db_file == NULL) {
        printf("[-] Gagal membuka file database!\n");
        return 1;
    }

    fprintf(db_file, "%s,%d,%d,", sg.sgc, sg.krn, sg.kt);
    for (size_t i = 0; i < vk_len; i++) {
        fprintf(db_file, "%02X", vk_encrypted[i]);
    }
    fprintf(db_file, "\n");

    fclose(db_file);
    printf("[4] Berhasil disimpan ke file 'kmc_database.db'\n");

    return 0;
}