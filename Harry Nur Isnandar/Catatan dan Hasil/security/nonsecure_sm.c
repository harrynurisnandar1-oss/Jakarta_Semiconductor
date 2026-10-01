#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ECPoint Structure
// Represents a simplified elliptic-curve point. 
// In this prototype: 
// x = X coordinate & y = Y coordinate 
// Output:  
// An ECPoint object containing the X and Y coordinates of a public key.

typedef struct {
    unsigned long long x;
    unsigned long long y;
} ECPoint;

// SECURE WORLD FUNCTION DECLARATIONS
// These functions are implemented in the Secure World / TrustZone code.
// The Non-Secure application can request security-sensitive operations through these interfaces without directly implementing the private-key operation itself.
// SECURE_GetOrCreateKeypair(): 
// Retrieves an existing private key or creates a new one, then returns the corresponding public key. 
// SECURE_DecryptPayload(): 
// Uses the private key and peer public key to decrypt the encrypted payload.  
// The actual implementation is located in secure_sm.c.

extern ECPoint SECURE_GetOrCreateKeypair(const char *sgc, int krn, int kt, unsigned long long *out_prvkey);
extern void SECURE_DecryptPayload(const char *hex_cipher, unsigned long long prvkey, 
                                  ECPoint peer_pubkey, char *out_plain);


// FUNCTION: NONSECURE_SaveOrUpdatePubkey()
// Purpose: Stores or updates the Smart Meter public key in the Non-Secure 
// configuration file "sm_pubkey.env".
// Inputs: 
//      sgc - Supply Group Code 
//      krn - Key Revision Number 
//      kt - Key Type 
//      pubkey - Public key represented by an ECPoint structure
// Processing:
//      1. Creates unique variable names using SGC, KRN, and KT.
//      2. Reads the existing "sm_pubkey.env" file. // 
//      3. Searches for existing X and Y public-key entries.
//      4. Updates the entries if they already exist.
//      5. Adds new entries if they do not exist. 
//      6. Rewrites the file with the updated information. 
//  Output:
//  The public key is stored in the following format: 
//
//  SGC_xxx_KRN_x_KT_x_SM_PUBKEY_X=0x... 
//  SGC_xxx_KRN_x_KT_x_SM_PUBKEY_Y=0x... 
// 
//  IMPORTANT: 
//  Only the public key is stored in the Non-Secure area.
void NONSECURE_SaveOrUpdatePubkey(const char *sgc, int krn, int kt, ECPoint pubkey) {
    char key_x_var[128], key_y_var[128];
    // Create unique variable names for the X and Y coordinates.
    // SGC, KRN, and KT are included so that different key records can coexist in the same file.
    // Example
    // SGC_999999_KRN_1_KT_2_SM_PUBKEY_X 
    // SGC_999999_KRN_1_KT_2_SM_PUBKEY_Y
    snprintf(key_x_var, sizeof(key_x_var), "SGC_%s_KRN_%d_KT_%d_SM_PUBKEY_X", sgc, krn, kt);
    snprintf(key_y_var, sizeof(key_y_var), "SGC_%s_KRN_%d_KT_%d_SM_PUBKEY_Y", sgc, krn, kt);

    // Open the existing public-key file for reading.
    // If the file does not exist, the function will create it later.
    FILE *pub_file = fopen("sm_pubkey.env", "r");
    char lines[100][256];
    // Number of existing lines currently stored in the file.
    int line_count = 0,
        // Flags indicating whether X and Y public-key entries were found. 
        x_found = 0, 
        y_found = 0;

    // Read existing public-key records.
    if (pub_file != NULL) {
        while (fgets(lines[line_count], sizeof(lines[0]), pub_file) && line_count < 100) {
            // Remove newline characters from the end of the line.
            lines[line_count][strcspn(lines[line_count], "\r\n")] = 0;
            // Check whether the current line contains the X coordinate belonging to the requested SGC/KRN/KT record.
            if (strncmp(lines[line_count], key_x_var, strlen(key_x_var)) == 0) {
                // Replace the existing X coordinate with the new value.
                snprintf(lines[line_count], sizeof(lines[0]), "%s=0x%LLX", key_x_var, pubkey.x);
                x_found = 1;
                // Check whether the current line contains the Y coordinate.
            } else if (strncmp(lines[line_count], key_y_var, strlen(key_y_var)) == 0) {
                // Replace the existing Y coordinate with the new value.
                snprintf(lines[line_count], sizeof(lines[0]), "%s=0x%LLX", key_y_var, pubkey.y);
                y_found = 1;
            }
            line_count++;
        }
        fclose(pub_file);
    }

    // Reopen the file in write mode.
    //"w" replaces the previous file contents.
    // Existing records are written back first, followed by any new key entries that were not found.
    pub_file = fopen("sm_pubkey.env", "w");
    if (pub_file != NULL) {
        // Write all previously existing records back to the file.
        for (int i = 0; i < line_count; i++) {
            fprintf(pub_file, "%s\n", lines[i]);
        }
        // If the X coordinate did not previously exist, add it.
        if (!x_found) fprintf(pub_file, "%s=0x%LLX\n", key_x_var, pubkey.x);
        // If the Y coordinate did not previously exist, add it.
        if (!y_found) fprintf(pub_file, "%s=0x%LLX\n", key_y_var, pubkey.y);
        fclose(pub_file);
        // Report successful public-key storage.
        printf("[NON-SECURE] Public Key berhasil disimpan/diperbarui di 'sm_pubkey.env'\n");
    }
}

// FUNCTION: main()
// Purpose: 
// Executes the Non-Secure Smart Meter key-management workflow. 
// Main workflow:
// 1. Ask the user for SGC, KRN, and KT. 
// 2. Search kmc_database.db for the matching record. 
// 3. Retrieve the encrypted Vending Key (VK). 
// 4. Request the Smart Meter key pair from the Secure World.
// 5. Store the resulting public key in the Non-Secure file. 
// 6. Construct the KMC public key. 
// 7. Request VK decryption through the Secure World. 
// 8. Display the decrypted plaintext VK. 
//
// Output: 
// The program displays the encrypted VK and, after successful 
// Secure World processing, the decrypted plaintext VK.
int main() {
    // Initialize the pseudo-random number generator.  
    // The Secure World implementation may use rand() when generating a new private key in this prototype.
    srand((unsigned int)time(NULL));
    
    // Display application header.
    printf("====================================================\n");
    printf("  SELECT RECORD (SGC, KRN, KT) FOR KEY GENERATION   \n");
    printf("====================================================\n");

    // 1. GET RECORD IDENTIFICATION FROM USER
    // The three parameters identify which Supply Group key record should be retrieved from the KMC database.
    char target_sgc[10];
    int target_krn, target_kt;

    printf("Masukkan SGC  (contoh: 999999): ");
    scanf("%9s", target_sgc);
    printf("Masukkan KRN  (contoh: 1)     : ");
    scanf("%d", &target_krn);
    printf("Masukkan KT   (contoh: 2)     : ");
    scanf("%d", &target_kt);

    // 2. SEARCH FOR THE RECORD IN THE KMC DATABASE
    // The database is opened in read-only mode. 
    // Expected record format: 
    // SGC,KRN,KT,EncryptedVK 
    // Example: 
    // 999999,1,2,A1B2C3D4...
    FILE *db_file = fopen("kmc_database.db", "r");
    // Check whether the database could be opened.
    if (db_file == NULL) {
        printf("[-] Error: 'kmc_database.db' tidak ditemukan.\n");
        return 1;
    }
    // Buffers used to store the current database record.
    char line[256], sgc[10], enc_vk[64];
    // Variables for the KRN and KT values read from the database and indicates whether a matching database record has been found.
    int krn, kt, found = 0;

    // Read the database line by line.
    while (fgets(line, sizeof(line), db_file)) {
        // Remove the newline character from the record.
        line[strcspn(line, "\r\n")] = 0;
        // Parse the database record. 
        // Expected format: 
        // SGC,KRN,KT,EncryptedVK 
        // sscanf() must successfully read all four fields.
        if (sscanf(line, "%[^,],%d,%d,%s", sgc, &krn, &kt, enc_vk) == 4) {
            // Compare all three identifying parameters: 
            // SGC KRN KT 
            // A record is considered a match only when all three values are identical to the user's requested values.
            if (strcmp(sgc, target_sgc) == 0 && krn == target_krn && kt == target_kt) {
                found = 1;
                // Stop searching after finding the matching record.
                break;
            }
        }
    }
    // Close the database after the search is complete.
    fclose(db_file);
    // CHECK SEARCH RESULT 
    // If no matching SGC/KRN/KT combination was found, the application cannot continue because there is no encrypted VK to process.
    if (!found) {
        printf("[-] Record dengan SGC='%s', KRN=%d, KT=%d TIDAK ditemukan di kmc_database.db!\n", 
                target_sgc, target_krn, target_kt);
        return 1;
    }
    // Matching record successfully found. 
    // enc_vk contains the encrypted VK retrieved from the database.
    printf("\n[+] Record Cocok Ditemukan di Database!\n");
    printf("    Payload Encrypted VK: %s\n\n", enc_vk);

    // 3. REQUEST THE KEY PAIR FROM THE SECURE WORLD

    // The private key variable is initialized to zero.
    // SECURE_GetOrCreateKeypair(): 
    // 	- Searches for an existing private key. 
    // 	- Creates one if it does not exist. 
    // 	- Returns the corresponding public key. 
    // 	- Writes the private key through the out_prvkey pointer. 
    // Output: 
    // sm_prvkey = Smart Meter private key 
    // sm_pubkey = Smart Meter public key

    unsigned long long sm_prvkey = 0;
    ECPoint sm_pubkey = SECURE_GetOrCreateKeypair(target_sgc, target_krn, target_kt, &sm_prvkey);

    // 4. STORE THE PUBLIC KEY IN THE NON-SECURE AREA
    // Only the public key is passed to this Non-Secure storage function. 
    // The private key remains associated with the Secure World operation.
    NONSECURE_SaveOrUpdatePubkey(target_sgc, target_krn, target_kt, sm_pubkey);

    // 5. PREPARE THE KMC PUBLIC KEY
    // G represents the simplified generator point used by this source code. 
    // The KMC public key is derived using the same simplified mathematical representation used by the prototype.
    
    ECPoint G = {0x6B17D1F2, 0x37B39D54};
    ECPoint kmc_pubkey = { (G.x * 12345 + 123) % 0xFFFFFFFFFFFFFFF1ULL, 
                           (G.y * 12345 + 987) % 0xFFFFFFFFFFFFFFF1ULL };
    
    // 6. DECRYPT THE VENDING KEY THROUGH THE SECURE WORLD 
    // The encrypted VK is passed to the Secure World together with: 
    // 	- Smart Meter private key 
    // 	- KMC public key 
    // The decrypted plaintext is written into decrypted_vk. 
    // Output: 
    // decrypted_vk contains the plaintext Vending Key.
    char decrypted_vk[32];
    SECURE_DecryptPayload(enc_vk, sm_prvkey, kmc_pubkey, decrypted_vk);
    
    // 7. DISPLAY FINAL RESULT
    // The final output shows the plaintext VK returned from the Secure World decryption function
    printf("\n[+] Hasil Akhir Non-Secure App:\n");
    printf("    -> Decrypted Plaintext VK: %s\n", decrypted_vk);
    printf("====================================================\n");

    return 0;
}