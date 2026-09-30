# Fitur 28/09/2026
# KEY MANAGEMENT CENTER

- Mencoba melakukan pengujian degan cryptography (TANPA OPENSSL).
- Supply Group Code (SGC) adalah kode angka 6 digit yang dipakai pada sistem meter prabayar (seperti kWh meter listrik) untuk menandai wilayah atau area geografis tertentu dari instalasi meter. Kode ini memastikan bahwa token isi ulang hanya bisa digunakan pada meter yang memiliki SGC yang sama.
- Try Using Key Management Center.
- Membuat uji KEY MANAGEMENT SYSTEM .

## Rincian fungsi dari masing-masing parameter di dalam SupplyGroup:
| Parameter| Nama, Ekstensi | Fungsi Utama |
-------- |----------------| ------------ |
| sg.sgc | Supply Group Code ("110224") | Kode unik 6 digit yang mengidentifikasi penyedia layanan (utilitas/PLN) atau wilayah geografis tertentu. Ini juga menjadi bahan dasar utama (seed) yang digabungkan dengan nilai acak (rand_val) untuk membentuk Vending Key Plaintext (vk_plain). |
| sg.krn | Key Revision Number (1) | Nomor revisi kunci. Digunakan untuk menandai versi algoritma atau versi kunci master yang sedang berlaku, sehingga sistem tahu apakah meteran perlu diperbarui kuncinya (Key Change Token). |
| sg.kt | Key Type (2) | Menentukan tipe/peran kunci yang dihasilkan (misalnya: kunci standar untuk pengisian token, kunci perubahan tarif, atau kunci teknis/pemeliharaan). |

## Rincian fungsi dari masing-masing parameter di dalam Key Type:

Nilai Key Type (KT) | Nama / Peran | Fungsi Utama |
------------------- |----------------| ------------ |
KT = 1   | Single Key / Unique Key | Kunci Unik per Meteran. Digunakan khusus untuk transaksi yang sifatnya sangat spesifik ke satu nomor seri meteran tertentu (misalnya token pengisian kredit/listrik harian). Token dibuat menggunakan kombinasi VK unik meteran tersebut sehingga tidak bisa dipakai di meteran milik orang lain.
KT = 2 | Group Key / Common Key | "Kunci Kelompok. Digunakan untuk operasi yang berlaku serentak pada seluruh meteran dalam satu wilayah/Supply Group Code (SGC) yang sama. Contohnya: token untuk perubahan tarif serentak (Tariff Index Update), token uji coba teknis, atau token pengaturan zona waktu." | 
KT = 3 | Key Change Key | Kunci Perubahan Versi. Kunci khusus yang digunakan saat sistem ingin mengganti Vending Key pada meteran (misalnya menaikkan KRN dari 1 ke 2). Token jenis ini memuat enkripsi kunci baru yang akan disimpan ke dalam memori permanen meteran. | 

### Cara Menghapus isi dari sebuah sebuah file dari 
Mengosongkan isi file (Reset tanpa menghapus file):
> 1. PowerShell
> 2. Clear-Content kmc_database.db

Menghapus file database sepenuhnya:
> 1. PowerShell
> 2. Remove-Item kmc_database.db
___

### SGC Dalam satu negara kodenya berbeda-beda berdasarkan wilayah geografisnya.

Perusahaan listrik besar membagi kode SGC ke dalam area/region yang berbeda (misalnya per Provinsi atau per Unit Induk Distribusi) karena alasan berikut
- Keamanan Kunci Enkripsi   : SGC digunakan sebagai basis untuk membuat kunci transaksi (vending key). Jika sistem di suatu daerah diretas atau kuncinya bocor, token ilegal yang dibuat oleh peretas hanya bisa mengacaukan meteran di wilayah SGC itu saja, sementara meteran di wilayah SGC lain di seluruh Indonesia tetap aman.
- Mencegah Salah Sasaran / Penyelundupan Token: Token listrik yang dibeli di suatu wilayah geografis tidak akan bisa dipakai di wilayah lain.
- Kemudahan Manajemen Kontraktor: PLN bisa menunjuk vendor penjualan token yang berbeda-beda untuk mengelola wilayah (vending management) tertentu berdasarkan kode SGC-nya.

ECDH STS merujuk pada gabungan antara protokol pertukaran kunci Elliptic Curve Diffie-Hellman (ECDH) dengan skema autentikasi Station-to-Station (STS) protocol.Secara mendasar, gabungan ini digunakan untuk menghasilkan kunci enkripsi bersama (shared session key) secara aman sekaligus mencegah serangan Man-in-the-Middle (MITM).

Elliptic Curve Diffie-Hellman (ECDH) adalah protokol kriptografi kunci asimetris yang memungkinkan dua pihak (misalnya Alice dan Bob) membuat satu kunci rahasia yang sama (shared secret) melalui saluran komunikasi yang tidak aman. Kunci ini nantinya digunakan untuk mengenkripsi pesan menggunakan algoritma simetris seperti AES.
    
Kelemahan dasar ECDH:
Secara bawaan, ECDH standar bersifat unauthenticated (tidak terautentikasi). Artinya, jika ada peretas di tengah jalur (MITM), peretas tersebut bisa menyamar menjadi Bob di depan Alice, dan menyamar menjadi Alice di depan Bob tanpa ketahuan.

Station-to-Station (STS) protocol adalah protokol kesepakatan kunci yang mengombinasikan skema Diffie-Hellman dengan tanda tangan digital (digital signature) untuk memverifikasi identitas kedua belah pihak.

Bagaimana ECDH STS Bekerja?

Ketika ECDH digabungkan dengan STS, kelemahan utama ECDH tertutupi:
- Pertukaran Kunci Publik: Alice dan Bob saling mengirimkan kunci publik ECDH mereka.
- Autentikasi Digital: Bersamaan dengan itu, mereka juga saling mengirimkan tanda tangan digital (signature) yang dibuat menggunakan kunci privat jangka panjang mereka beserta sertifikat identitasnya.
- Verifikasi: Masing-masing pihak memverifikasi tanda tangan tersebut. Jika tanda tangan valid, identitas mereka terkonfirmasi asli.
- Pembuatan Kunci: Setelah aman dari intersepsi, kedua pihak memproses parameter kurva eliptik untuk menghasilkan kunci enkripsi yang sama secara independen.

Mengapa Protokol Ini Digunakan?
- Keamanan dari MITM: Penyerang tidak bisa memalsukan tanda tangan digital karena tidak memiliki kunci privat asli milik instansi/pengguna tersebut.
- Efisiensi Tinggi: Menggunakan kurva eliptik (ECDH) membutuhkan ukuran kunci yang jauh lebih kecil dibandingkan RSA tradisional, sehingga komputasinya sangat cepat dan hemat memori.
- PFS (Perfect Forward Secrecy): Jika suatu saat kunci utama bocor, peretas tetap tidak bisa membuka riwayat pesan di masa lalu karena setiap sesi komunikasi menggunakan kunci acak yang berbeda.

# Fitur 29/09/2026
### Security Modul

- Fitur yang ditambahkan adalah Key Agreement Scheme (TANPA OPENSSL).

- Menambahkan Fitur Security Module, dimana VK (Vending Key) di load dengan ECDH yang diterima dalam security module yang di di cocokan dengan SM prvkey yang sudah.

- Menambahkan fitur pembacaan publik key dan private key dari pengujian TrustZone

### POS
Dalam sistem prabayar dan enkripsi token berbasis **SPLN** (yang mengacu pada standar **STS / IEC 62055-41**), jenis-jenis **Key Type (KT)**, **Key Revision Number (KRN)**, dan **Supply Group Code (SGC)** diklasifikasikan sebagai berikut:

---

### 1. Jenis Key Type (KT)
**Key Type (KT)** diwakili oleh angka 1-digit desimal dengan rentang **0 hingga 3**. Atribut KT menentukan sifat operasional dan tingkat diversifikasi kunci vending (*VendingKey*) serta kunci decoder (*DecoderKey*):

* **KT = 0 (Initialization / DITK)**
  * **Kunci**: *Decoder Initialisation Transfer Key* (DITK).
  * **Fungsi**: Digunakan oleh pabrikan meteran untuk pemuatan kunci awal dan pengujian sebelum meteran keluar dari pabrik.
* **KT = 1 (Default / VDDK & DDTK)**
  * **Kunci**: *Vending Default DES Key* (VDDK) dan *Decoder Default Transfer Key* (DDTK).
  * **Fungsi**: Digunakan untuk meteran baru atau meteran yang disimpan di gudang. Pada KT 1, sistem **hanya diizinkan memproses token pergantian kunci (*keychange*) dan token manajemen**, serta **dilarang menerbitkan atau menerima token isi ulang kredit**.
* **KT = 2 (Unique / VUDK & DUTK)**
  * **Kunci**: *Vending Unique DES Key* (VUDK) dan *Decoder Unique Transfer Key* (DUTK).
  * **Fungsi**: Merupakan tipe operasional standar untuk transaksi pelanggan. Kunci didiversifikasikan secara unik untuk setiap meteran berdasarkan *MeterPAN/DRN*, sehingga token kredit yang diterbitkan hanya berlaku pada satu meteran spesifik.
* **KT = 3 (Common / VCDK & DCTK)**
  * **Kunci**: *Vending Common DES Key* (VCDK) dan *Decoder Common Transfer Key* (DCTK).
  * **Fungsi**: Digunakan untuk kelompok meteran yang menggunakan kunci bersama (misalnya pada media token kartu magnetik yang dapat ditulis/dihapus ulang).

---

### 2. Jenis & Ketentuan Key Revision Number (KRN)
**Key Revision Number (KRN)** diwakili oleh angka 1-digit desimal dengan rentang **1 hingga 9**:

* **KRN = 1 hingga 9 (Untuk VendingKey & DecoderKey Operasional)**:
  * Menunjukkan urutan atau revisi versi kunci aktif pada suatu SGC.
  * Nilai KRN diawali dari **1** untuk kunci pertama suatu SGC, kemudian dinaikkan secara berurutan setiap kali terjadi pergantian kunci (*keychange*), dan berulang kembali ke **1** setelah mencapai angka 9.
* **KRN = 0 (Khusus DITK)**:
  * Konsep revisi kunci tidak berlaku untuk kunci inisialisasi pabrikan (*DITK*), sehingga nilai KRN pada meteran yang memuat DITK selalu diatur ke **0**.

---

### 3. Jenis & Klasifikasi Supply Group Code (SGC)
**Supply Group Code (SGC)** adalah kode angka unik **6-digit desimal** (rentang `000001` hingga `999999`) yang dialokasikan oleh *Key Management Centre* (KMC) untuk mengelompokkan meteran secara geografis atau administratif:

* **Default SGC (KT = 1)**:
  * SGC yang dikaitkan dengan kunci VDDK/DDTK. Digunakan untuk pengiriman meteran dari pabrik ke gudang PLN. Hanya diizinkan untuk eksekusi *keychange* ke SGC operasional.
* **Unique SGC (KT = 2)**:
  * SGC standar unit/wilayah operasional PLN yang dikaitkan dengan kunci VUDK/DUTK per pelanggan. Digunakan untuk transaksi pembuatan token kredit unik per meteran.
* **Common SGC (KT = 3)**:
  * SGC grup yang dikaitkan dengan kunci VCDK/DCTK untuk skema kunci kelompok/umum.


### Pengklasifikasian SGC

Dalam sistem enkripsi meteran STS (Standard Transfer Specification) PLN, contoh kode SGC (Supply Group Code) yang umum digunakan untuk wilayah Jakarta Selatan (khususnya area di bawah UP3 Bulungan / UID Jakarta Raya) adalah:

• *000305* atau variasi regional DKI Jakarta lainnya seperti *000300* hingga *000309*.

SGC merupakan kode 6 digit. Angka spesifik meteran Anda bisa saja berbeda tergantung pada generator sistem saat meteran prabayar tersebut dipasang atau dilakukan update (KCT/Key Change Token).

# FITUR 30/09/2026

### **1. Fitur pada Domain Secure (`secure_sm.txt`)**
* **Manajemen Kunci Terisolasi Berdasarkan Triple Parameter (SGC, KRN, KT)**:
  * Fungsi `SECURE_GetOrCreateKeypair` memeriksa file `sm_prvkey.env` untuk mencari **Private Key** menggunakan format penamaan variabel unik `SGC_<sgc>_KRN_<krn>_KT_<kt>_SM_PRVKEY`.
  * Jika Private Key belum ada, sistem membuat kunci privat baru (`generate_private_key`) dan menyimpannya ke dalam file lingkungan aman.
  * Menghasilkan **Public Key** terkait melalui perkalian skalar kurva eliptik (`ecdh_scalar_multiply`) dari titik generator (***G***) dan Private Key.
* **Dekripsi Payload Terisolasi (`SECURE_DecryptPayload`)**:
  * Mengisolasi proses dekripsi *ciphertext* (`hex_cipher`) di dalam domain TrustZone dengan menghitung *shared secret* berbasis `peer_pubkey` dan `prvkey` menggunakan mekanisme XOR masking.

---

### **2. Fitur pada Domain Non-Secure (`nonsecure_sm.txt`)**
* **Pencarian Database Multi-Kriteria**:
  * Meminta input pencarian spesifik dari pengguna yang mencakup **Supply Group Code (SGC)**, **Key Revision Number (KRN)**, dan **Key Type (KT)**.
  * Membaca file `kmc_database.db` dan mencocokkan ketiga parameter tersebut secara bersamaan untuk menemukan *Encrypted Vending Key* yang sesuai.
* **Penyimpanan dan Pembaruan Public Key (`NONSECURE_SaveOrUpdatePubkey`)**:
  * Menyimpan atau memperbarui koordinat Kunci Publik (***X***) dan (***Y***) di dalam file `sm_pubkey.env` dengan variabel berformat unik berbasis SGC, KRN, dan KT.
* **Integrasi Lintas Domain (Non-Secure Callable / NSC)**:
  * Menghubungkan aplikasi Non-Secure ke Secure World dengan memanggil fungsi `SECURE_GetOrCreateKeypair` dan `SECURE_DecryptPayload`.
* **Eksekusi dan Penayangan Plaintext VK**:
  * Menampilkan Vending Key dalam bentuk *plaintext* hasil dekripsi aman dari Secure World.
