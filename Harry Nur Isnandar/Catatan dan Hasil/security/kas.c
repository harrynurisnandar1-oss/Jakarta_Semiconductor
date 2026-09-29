// Key Agreement Scheme dari Key Management Center

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// ============================================================================
// 1. Kriptografi Kurva Elips (ECC / ECDH) Murni dalam C (Secp256k1 Modulo p)
// ============================================================================
#define CURVE_P 29  // Prime modulus sederhana untuk demonstrasi matematika kurva
#define CURVE_A 4
#define CURVE_B 20

typedef struct {
    int64_t x;
    int64_t y;
    int is_infinity;
} Point;

// Modulo arsitektur yang aman untuk angka negatif
int64_t mod(int64_t a, int64_t m) {
    int64_t r = a % m;
    return r < 0 ? r + m : r;
}

// Invers Modulo dengan Extended Euclidean Algorithm
int64_t modInverse(int64_t a, int64_t m) {
    int64_t m0 = m, t, q;
    int64_t x0 = 0, x1 = 1;
    if (m == 1) return 0;
    a = mod(a, m);
    while (a > 1) {
        q = a / m;
        t = m; m = a % m; a = t;
        t = x0; x0 = x1 - q * x0; x1 = t;
    }
    if (x1 < 0) x1 += m0;
    return x1;
}

// Penjumlahan Titik Kurva Elips (Point Addition)
Point point_add(Point P, Point Q) {
    Point R;
    if (P.is_infinity) return Q;
    if (Q.is_infinity) return P;

    if (P.x == Q.x && mod(P.y + Q.y, CURVE_P) == 0) {
        R.is_infinity = 1;
        return R;
    }

    int64_t lambda;
    if (P.x == Q.x && P.y == Q.y) {
        // Point Doubling
        int64_t num = mod(3 * P.x * P.x + CURVE_A, CURVE_P);
        int64_t den = modInverse(2 * P.y, CURVE_P);
        lambda = mod(num * den, CURVE_P);
    } else {
        // Point Addition biasa
        int64_t num = mod(Q.y - P.y, CURVE_P);
        int64_t den = modInverse(Q.x - P.x, CURVE_P);
        lambda = mod(num * den, CURVE_P);
    }

    R.x = mod(lambda * lambda - P.x - Q.x, CURVE_P);
    R.y = mod(lambda * (P.x - R.x) - P.y, CURVE_P);
    R.is_infinity = 0;
    return R;
}

// Perkalian Skalar Titik (Scalar Multiplication / ECDH Core)
Point scalar_mult(int64_t k, Point P) {
    Point R = {0, 0, 1}; // Point at Infinity
    Point Q = P;
    while (k > 0) {
        if (k & 1) R = point_add(R, Q);
        Q = point_add(Q, Q);
        k >>= 1;
    }
    return R;
}

// Base Point / Generator Point (G) pada kurva
const Point G = {1, 5, 0};

// ============================================================================
// 2. Simulasi KMC & Security Module (SM) Key Agreement
// ============================================================================

// Fungsi XOR Cipher untuk masking VK dengan Shared Secret ECDH
void cipher_xor(const char *in, char *out, int len, int key) {
    for (int i = 0; i < len; i++) {
        out[i] = in[i] ^ (key & 0xFF);
    }
}

int main() {
    printf("=====================================================\n");
    printf("   KMC - SM KEY AGREEMENT SCHEME (ECDH Simulation)   \n");
    printf("=====================================================\n\n");

    // -------------------------------------------------------------
    // STEP 1: Pengambilan Data VK dari File Database KMC (kmc_database.db)
    // -------------------------------------------------------------
    FILE *db = fopen("kmc_database.db", "r");
    if (!db) {
        printf("[ERROR] Database 'kmc_database.db' tidak ditemukan!\n");
        printf("Jalankan program KMC sebelumnya terlebih dahulu.\n");
        return 1;
    }

    char sgc[7], enc_vk_hex[32];
    int krn, kt;
    // KODE BARU (PERBAIKAN):
    fscanf(db, "%[^,],%d,%d,%s", sgc, &krn, &kt, enc_vk_hex);;
    fclose(db);

    printf("[1] KMC Read Database:\n");
    printf("    -> SGC: %s | KRN: %d | KT: %d\n", sgc, krn, kt);
    printf("    -> Encrypted VK: %s\n\n", enc_vk_hex);

    // -------------------------------------------------------------
    // STEP 2: Inisialisasi Kunci ECDH (KMC & SM)
    // -------------------------------------------------------------
    int64_t kmc_prvkey = 7;                   // Private Key KMC
    Point kmc_pubkey = scalar_mult(kmc_prvkey, G); // Public Key KMC

    int64_t sm_prvkey = 11;                  // Private Key SM (Security Module)
    Point sm_pubkey = scalar_mult(sm_prvkey, G);   // Public Key SM

    printf("[2] ECDH Key Pair Generation:\n");
    printf("    -> KMC Public Key : (%lld, %lld)\n", kmc_pubkey.x, kmc_pubkey.y);
    printf("    -> SM Public Key  : (%lld, %lld)\n\n", sm_pubkey.x, sm_pubkey.y);

    // -------------------------------------------------------------
    // STEP 3: KMC Menghitung Shared Secret & Membaca VK
    // Shared Secret = kmc_prvkey * sm_pubkey
    // -------------------------------------------------------------
    Point kmc_shared_point = scalar_mult(kmc_prvkey, sm_pubkey);
    int kmc_shared_secret = (int)kmc_shared_point.x;

    // Masking enc_vk_hex menggunakan Shared Secret untuk membuat Keyload File (Vkloadresp)
    int vk_len = (int)strlen(enc_vk_hex);
    char vkloadresp[64] = {0};
    cipher_xor(enc_vk_hex, vkloadresp, vk_len, kmc_shared_secret);

    // Simpan Keyload File ke Disk
    FILE *f_load = fopen("Vkloadresp.bin", "wb");
    fwrite(vkloadresp, 1, vk_len, f_load);
    fclose(f_load);

    printf("[3] KMC Process (Vkloadresp Generation):\n");
    printf("    -> KMC Computed Shared Secret : %d\n", kmc_shared_secret);
    printf("    -> Keyload File 'Vkloadresp.bin' successfully generated.\n\n");

    // -------------------------------------------------------------
    // STEP 4: Security Module (SM) Menerima & Dekripsi Keyload File
    // Shared Secret = sm_prvkey * kmc_pubkey
    // -------------------------------------------------------------
    Point sm_shared_point = scalar_mult(sm_prvkey, kmc_pubkey);
    int sm_shared_secret = (int)sm_shared_point.x;

    // Membaca file Vkloadresp.bin
    FILE *f_recv = fopen("Vkloadresp.bin", "rb");
    char recv_buf[64] = {0};
    fread(recv_buf, 1, vk_len, f_recv);
    fclose(f_recv);

    // SM Mendekripsi Vkloadresp dengan Shared Secret miliknya
    char decrypted_vk_hex[64] = {0};
    cipher_xor(recv_buf, decrypted_vk_hex, vk_len, sm_shared_secret);

    printf("[4] SM Process (Key Agreement & Recovery):\n");
    printf("    -> SM Computed Shared Secret  : %d\n", sm_shared_secret);
    printf("    -> Recovered VK Hex           : %s\n\n", decrypted_vk_hex);

    // -------------------------------------------------------------
    // STEP 5: Verifikasi Hasil Pertukaran Kunci
    // -------------------------------------------------------------
    if (kmc_shared_secret == sm_shared_secret && strcmp(enc_vk_hex, decrypted_vk_hex) == 0) {
        printf("=====================================================\n");
        printf(" [SUCCESS] Key Agreement Scheme Berhasil!          \n");
        printf(" Vending Key ($VK$) berhasil diterima dan didekripsi \n");
        printf(" oleh Security Module (SM) via ECDH.               \n");
        printf("=====================================================\n");
    } else {
        printf("[FAILED] Pertukaran Kunci Gagal!\n");
    }

    return 0;
}