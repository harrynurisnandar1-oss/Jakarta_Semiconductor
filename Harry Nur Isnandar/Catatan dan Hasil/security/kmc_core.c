// KEY MANAGEMENT CENTER

// This proram makes Basic workflow a Key Management Center (KMC) :
// 1. Receive Supply Group parameters (SGC, KRN, KT).
// 2. Generate a random value.
// 3. Generate a plaintext Vending Key (VK).
// 4. Encrypt the VK using a simulated AES-192-like operation.
// 5. Store the encrypted VK and related parameters in a database file.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 1. SUPPLY GROUP STRUCTURE
// Stores the parameters associated with a Supply Group
// Output:
// A SupplyGroup structure containing all three parameters.

typedef struct {
    char sgc[7];  // Supply Group Code (6 digit)
    int krn;      // Key Revision Number
    int kt;       // Key Type
} SupplyGroup;

// FUNCTION: generate_random_bytes()
// Purpose:
// Generates pseudo-random byte values and stores them in the provided buffer.
// 
// Parameters:
// buf - Pointer to the output buffer where random bytes are stored.
// len - Number of random bytes to generate.
//
// Output:
// The buffer pointed to by 'buf' is filled with random values ranging from 0x00 to 0xFF.
// 
// NOTE:
// rand() is a pseudo-random generator and is NOT suitable for production-grade cryptographic key generation

void generate_random_bytes(unsigned char *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        buf[i] = (unsigned char)(rand() % 256);
    }
}

// FUNCTION: encrypt_simulated_aes192() (24-byte SMK)
// Purpose
//     Simulates encryption of input data using a 25 byte key.
// Parameter
//     input  - Pointer to the plaintext input data.
//     in_len - Length of the input data in bytes.
//     key    - Pointer to the encryption key.
//     output - Pointer to the buffer where encrypted data is stored.
//
// Processing:
//     Each input byte is XORed with one byte from the 24-byte key.
//
//     output[i] = input[i] XOR key[i % 24]
//
// Output:
//     The encrypted byte sequence is stored in 'output'.

void encrypt_simulated_aes192(const unsigned char *input, size_t in_len, 
                              const unsigned char *key, unsigned char *output) {
    for (size_t i = 0; i < in_len; i++) {
        output[i] = input[i] ^ key[i % 24];
    }
}

// Main Program
// Purpose:
//      Executes the complete KMC simulation workflow.
// Input:
//      The program accepts Supply Group parameters in two ways:
//     1. Command-line arguments:
//        kmc_sim.exe 999999 1 2
//        argv[1] = SGC
//        argv[2] = KRN
//        argv[3] = KT
//     2. Interactive terminal input.
// Output:
//     The program displays:
//     - Supply Group parameters
//     - Plaintext Vending Key
//     - Encrypted Vending Key in hexadecimal format
//     - Database storage status
// Database output format:
//     SGC,KRN,KT,ENCRYPTED_VK

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));
    SupplyGroup sg;

    // The program supports two input methods:
    // A. Command-line arguments
    // B. Interactive terminal input

    // 1. GET SUPPLY GROUP INPUT
    if (argc == 4) {
        // OPTION A: Read parameters from command-line arguments.
        // Example
        // kmc_sim.exe 999999 1 2
        // Result:
        // SGC = 999999
        // KRN = 1
        // KT  = 2
        // Copy the first 6 characters into the SGC field.
        strncpy(sg.sgc, argv[1], 6);
        // Ensure the string is null-terminated.
        sg.sgc[6] = '\0';
        // Convert KRN from string to integer.
        sg.krn = atoi(argv[2]);
        // Convert KT from string to integer.
        sg.kt = atoi(argv[3]);
        printf("[+] Menggunakan input dari Argumen CLI.\n");
    } else {
        // OPTION B: Read parameters interactively from the terminal.

        printf("=== INPUT PARAMETER SUPPLY GROUP ===\n");

        // Read the 6-character Supply Group Code.
        printf("Masukkan SGC (6 Digit Code, contoh: 999999): ");
        scanf("%6s", sg.sgc);
        
        // Read the Key Revision Number.
        printf("Masukkan KRN (Key Revision Number, contoh: 1): ");
        scanf("%d", &sg.krn);

        // Read the Key Type.
        printf("Masukkan KT  (Key Type, contoh: 2): ");
        scanf("%d", &sg.kt);
        printf("------------------------------------\n");
    }

    // System Master Key (SMK) 192-bit (24 Bytes)
    // The SMK is represented as a 24-byte value, corresponding to 192 bits.
    // 24 bytes × 8 bits = 192 bits
    // NOTE:
    // This is a hard-coded demonstration key.
    // A production KMC should NOT store cryptographic master keys directly in source code.

    unsigned char smk[24] = "MasterKeyKMC192BitSecret";

    // 2. GENERATE RANDOM VALUE AND VENDING KEY\
    // Generate a 2-byte random value.
    unsigned char rand_val[2];
    generate_random_bytes(rand_val, sizeof(rand_val));
    // Allocate a buffer for the plaintext Vending Key.
    char vk_plain[16];
    // Build the plaintext Vending Key using:
    // SGC + Random Byte 0 + Random Byte 1
    snprintf(vk_plain, sizeof(vk_plain), "%s%02X%02X", sg.sgc, rand_val[0], rand_val[1]);
    
    // DISPLAY KMC INFORMATION 
    printf("\n=== KMC (Key Management Centre) ===\n");
    // Display the Supply Group parameters.
    printf("[1] Supply Group Params -> SGC: %s | KRN: %d | KT: %d\n", sg.sgc, sg.krn, sg.kt);
    // Display the generated plaintext Vending Key.
    printf("[2] Raw Vending Key (VK Plaintext): %s\n", vk_plain);

    // 3. ENCRYPT VENDING KEY
    // Determine the actual length of the plaintext VK.
    size_t vk_len = strlen(vk_plain);
    // Buffer for the encrypted VK.
    unsigned char vk_encrypted[16];
    // Encrypt the plaintext VK using the simulated AES-192-like function.
    encrypt_simulated_aes192((unsigned char*)vk_plain, vk_len, smk, vk_encrypted);

    // DISPLAY ENCRYPTED VENDING KEY
    // The encrypted data is binary, so it is displayed as hexadecimal.
    printf("[3] Encrypted VK (Hex): ");
    for (size_t i = 0; i < vk_len; i++) {
        printf("%02X", vk_encrypted[i]);
    }
    printf("\n");

    //  4. STORE DATA IN KMC DATABASE
    // Open the database file in append mode.
    //  "a" means:
    //  - Existing data is preserved.
    //  - New records are added at the end of the file.
    FILE *db_file = fopen("kmc_database.db", "a");
    // Check whether the database file was successfully opened.
    if (db_file == NULL) {
        printf("[-] Gagal membuka file database!\n");
        // Return a non-zero value to indicate an error.
        return 1;
    }

    // Write the Supply Group parameters to the database.
    // Format:
    //      SGC,KRN,KT,
    // Example:
    //      999999,1,2,

    fprintf(db_file, "%s,%d,%d,", sg.sgc, sg.krn, sg.kt);
    // Write the encrypted VK as hexadecimal characters.
    // Each byte is converted into two hexadecimal characters.
    //
    // Example:
    //     Binary : 0xA3 0x7F
    //     Stored : A37F
    //
    for (size_t i = 0; i < vk_len; i++) {
        fprintf(db_file, "%02X", vk_encrypted[i]);
    }
    // Finish the database record with a newline.
    fprintf(db_file, "\n");
    // Close the database file.
    fclose(db_file);
    // Inform the user that the operation completed successfully
    printf("[4] Berhasil disimpan ke file 'kmc_database.db'\n");
    // Return 0 to indicate successful program execution.
    return 0;
}