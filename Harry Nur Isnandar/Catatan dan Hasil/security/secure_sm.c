// The Secure World code contains the private-key generation/storage logic,
//  public-key derivation, and payload decryption function.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ECPoint Structure
// Represents a simplified elliptic-curve point.
// x = X coordinate & y = Y coordinate
// Output: 
// An ECPoint containing the calculated X and Y coordinates.
typedef struct {
    unsigned long long x;
    unsigned long long y;
} ECPoint;

// ELLIPTIC-CURVE GENERATOR POINT
// G is the generator point used by the simplified ECC calculation.
static const ECPoint G = {0x6B17D1F2, 0x37B39D54};

// FUNCTION: generate_private_key()
// Purpose: 
//      Generates a pseudo-random private key for the Smart Meter. 
// Input: 
//      None. 
// Processing: 
//      Uses rand() to generate a value in the configured range. 
// Output: 
//      Returns an unsigned long long value representing the generated private key. 
// Example: 
// Possible output: 
//      0x12345678 
// SECURITY NOTE: 
//      rand() is not a cryptographically secure random-number generator.
static unsigned long long generate_private_key() {
    return (unsigned long long)(rand() % 1000000000 + 100000);
}

// FUNCTION: ecdh_scalar_multiply()
// Purpose: 
//  Generates a simplified public key from a private key and generator point. 
// Inputs: 
//  priv_key    - Private key value. 
//   generator  - Generator point G. 
// Processing: 
//  The prototype calculates the X and Y coordinates using: 
//      X = (G.x * private_key + 123456789) mod P 
//      Y = (G.y * private_key + 987654321) mod P 
// Output: 
//  Returns an ECPoint containing the calculated public-key coordinates. 

static ECPoint ecdh_scalar_multiply(unsigned long long priv_key, ECPoint generator) {
    ECPoint pub_key;
    // Calculate the simplified public-key X coordinate.
    pub_key.x = (generator.x * priv_key + 123456789) % 0xFFFFFFFFFFFFFFF1ULL;
    // Calculate the simplified public-key Y coordinate.
    pub_key.y = (generator.y * priv_key + 987654321) % 0xFFFFFFFFFFFFFFF1ULL;
    return pub_key;
}


// NSC Function with identification SGC, KRN, dan KT
// Purpose: 
// Retrieves an existing Smart Meter private key for a specific SGC/KRN/KT combination or 
// generates a new private key if one does not already exist. 
// 
// Inputs: 
//  sgc         - Supply Group Code. 
//  krn         - Key Revision Number. 
//  kt          - Key Type. 
//  out_prvkey  - Pointer where the private key will be returned. 
// 
// Processing: 
// 1. Build a unique private-key identifier. 
// 2. Search "sm_prvkey.env" for an existing private key. 
// 3. If found, load the existing private key. 
// 4. If not found, generate a new private key. 
// 5. Store the new private key in the file. 
// 6. Calculate the corresponding public key. 
// 7. Return the public key. 
// Output: 
//  Return value: 
//      Smart Meter public key as an ECPoint. 
//  out_prvkey: 
//      The corresponding private key. 
// 
// Example identifier: 
//      SGC_999999_KRN_1_KT_2_SM_PRVKEY

ECPoint SECURE_GetOrCreateKeypair(const char *sgc, int krn, int kt, unsigned long long *out_prvkey) {
    char key_var[128];
    // Build a unique variable name for the private key.
    // The SGC, KRN, and KT values identify which Smart Meter key record is being accessed.
    snprintf(key_var, sizeof(key_var), "SGC_%s_KRN_%d_KT_%d_SM_PRVKEY", sgc, krn, kt);

    // Initialize the private key to zero.
    // A value of zero is used to indicate that no existing key has been found.
    unsigned long long prvkey = 0;

    // Open the private-key file for reading.
    FILE *prv_file = fopen("sm_prvkey.env", "r");
    
    // SEARCH FOR AN EXISTING PRIVATE KEY
    if (prv_file != NULL) {
        char line[256];
        // Read the file one line at a time.
        while (fgets(line, sizeof(line), prv_file)) {
            // Check whether this line belongs to the requested SGC/KRN/KT.
            if (strncmp(line, key_var, strlen(key_var)) == 0) {
                // Extract the hexadecimal private-key value after '='.
                sscanf(line, "%*[^=]=0x%LLX", &prvkey);
                break;
            }
        }
        fclose(prv_file);
    }

    // CREATE A NEW PRIVATE KEY IF NONE WAS FOUND
    if (prvkey == 0) {
        // Generate a new pseudo-random private key.
        prvkey = generate_private_key();
        // Open the file in append mode so existing key records are preserved.
        prv_file = fopen("sm_prvkey.env", "a");
        if (prv_file != NULL) {
            fprintf(prv_file, "%s=0x%LLX\n", key_var, prvkey);
            // Store the new private key using the generated identifier.
            fclose(prv_file);
            printf("[SECURE TRUSTZONE] Private Key Baru Dibuat untuk (SGC:%s, KRN:%d, KT:%d)\n", sgc, krn, kt);
        }

    // EXISTING PRIVATE KEY FOUND
    } else {
        // Inform the user that the existing private key is being reused.
        printf("[SECURE TRUSTZONE] Private Key Eksisting Ditemukan untuk (SGC:%s, KRN:%d, KT:%d)\n", sgc, krn, kt);
    }

    // RETURN THE PRIVATE KEY TO THE CALLER
    // out_prvkey is an output parameter.
    // If the caller supplied a valid pointer, store the private key there.
    if (out_prvkey) *out_prvkey = prvkey;
    // ECPoint containing the public-key coordinates.
    return ecdh_scalar_multiply(prvkey, G);
}

// FUNCTION: SECURE_DecryptPayload()
// Purpose: 
//      Decrypts an encrypted hexadecimal payload using a simplified shared 
// secret derived from the Smart Meter private key and the peer public key. 
// Inputs: 
// hex_cipher   - Encrypted payload represented as hexadecimal text. 
// prvkey       - Smart Meter private key. 
// peer_pubkey  - Peer/KMC public key. 
// out_plain    - Output buffer where the plaintext will be written. 
// Processing: 
// 1. Calculate a simplified shared secret. 
// 2. Extract the lowest 8 bits of the shared secret as the XOR key byte. 
// 3. Read the encrypted payload two hexadecimal characters at a time. 
// 4. Convert each hexadecimal pair into a byte. 
// 5. XOR the encrypted byte with the derived key byte. 
// 6. Store the resulting plaintext byte in out_plain. 
// 7. Add a null terminator to the plaintext string.
// Output: 
// out_plain contains the decrypted plaintext string.

void SECURE_DecryptPayload(const char *hex_cipher, unsigned long long prvkey, 
                           ECPoint peer_pubkey, char *out_plain) {
    // The resulting value is used to derive a single-byte XOR key.
    unsigned long long shared_secret = (peer_pubkey.x * prvkey + peer_pubkey.y) % 0xFFFFFFFFFFFFFFF1ULL;
    // Determine the number of hexadecimal characters in the encrypted input.
    size_t len = strlen(hex_cipher);
    // Use the lowest 8 bits of the shared secret as the decryption key byte.
    unsigned char key_byte = (unsigned char)(shared_secret & 0xFF);
    // DECRYPT THE HEXADECIMAL PAYLOAD
    // Each encrypted byte is represented by two hexadecimal characters.
    for (size_t i = 0; i < len; i += 2) {
        unsigned int byte_val;
        // Convert two hexadecimal characters into a numerical byte value.
        sscanf(&hex_cipher[i], "%02x", &byte_val);
        // XOR the encrypted byte with the derived key byte. The resulting byte is written into the plaintext buffer.
        out_plain[i / 2] = (char)(byte_val ^ key_byte);
    }
    // Add the null terminator so that out_plain can be used as a C string.
    out_plain[len / 2] = '\0';
}