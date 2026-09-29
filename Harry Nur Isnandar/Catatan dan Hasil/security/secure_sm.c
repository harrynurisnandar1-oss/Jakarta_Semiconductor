#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned long long x;
    unsigned long long y;
} ECPoint;

// Generator Point G
static const ECPoint G = {0x6B17D1F2, 0x37B39D54};

// Internal Private Key Generator
static unsigned long long generate_private_key() {
    return (unsigned long long)(rand() % 1000000000 + 100000);
}

// Internal Scalar Multiplication (Q = d * G)
static ECPoint ecdh_scalar_multiply(unsigned long long priv_key, ECPoint generator) {
    ECPoint pub_key;
    pub_key.x = (generator.x * priv_key + 123456789) % 0xFFFFFFFFFFFFFFF1ULL;
    pub_key.y = (generator.y * priv_key + 987654321) % 0xFFFFFFFFFFFFFFF1ULL;
    return pub_key;
}

// === NSC (NON-SECURE CALLABLE) API: Dipanggil dari Non-Secure World ===

// API 1: Mengambil atau membuat Private Key & menghitung Public Key
ECPoint SECURE_GetOrCreateKeypair(const char *sgc, unsigned long long *out_prvkey) {
    char key_var[64];
    snprintf(key_var, sizeof(key_var), "SGC_%s_SM_PRVKEY", sgc);

    unsigned long long prvkey = 0;
    FILE *prv_file = fopen("sm_prvkey.env", "r");
    
    // Cek jika Private Key sudah ada di Secure Storage
    if (prv_file != NULL) {
        char line[128];
        while (fgets(line, sizeof(line), prv_file)) {
            if (strncmp(line, key_var, strlen(key_var)) == 0) {
                sscanf(line, "%*[^=]=0x%LLX", &prvkey);
                break;
            }
        }
        fclose(prv_file);
    }

    // Jika belum ada, buat Private Key baru dan simpan ke sm_prvkey.env
    if (prvkey == 0) {
        prvkey = generate_private_key();
        prv_file = fopen("sm_prvkey.env", "a");
        if (prv_file != NULL) {
            fprintf(prv_file, "%s=0x%LLX\n", key_var, prvkey);
            fclose(prv_file);
            printf("[SECURE TRUSTZONE] Private Key Baru Dibuat & Disimpan di Secure Storage.\n");
        }
    } else {
        printf("[SECURE TRUSTZONE] Private Key Lama Ditemukan di Secure Storage.\n");
    }

    if (out_prvkey) *out_prvkey = prvkey;
    return ecdh_scalar_multiply(prvkey, G);
}

// API 2: Dekripsi Payload VK menggunakan Shared Secret ECDH
void SECURE_DecryptPayload(const char *hex_cipher, unsigned long long prvkey, 
                           ECPoint peer_pubkey, char *out_plain) {
    // Derive Shared Secret inside Secure World
    unsigned long long shared_secret = (peer_pubkey.x * prvkey + peer_pubkey.y) % 0xFFFFFFFFFFFFFFF1ULL;
    
    size_t len = strlen(hex_cipher);
    unsigned char key_byte = (unsigned char)(shared_secret & 0xFF);

    for (size_t i = 0; i < len; i += 2) {
        unsigned int byte_val;
        sscanf(&hex_cipher[i], "%02x", &byte_val);
        out_plain[i / 2] = (char)(byte_val ^ key_byte);
    }
    out_plain[len / 2] = '\0';
}