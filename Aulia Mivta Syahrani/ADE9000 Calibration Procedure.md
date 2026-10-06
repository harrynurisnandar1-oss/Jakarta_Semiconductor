# PROSEDUR KALIBRASI ADE9000
**Three Phase AMI Smart Meter – STM32U585VIT6**  
*Untuk host-side dry-run dan persiapan implementasi hardware*

---

## 1. Tujuan
Prosedur ini disusun sebagai acuan untuk melakukan kalibrasi jalur metrologi ADE9000 pada smart meter tiga fasa. Kalibrasi bertujuan mengurangi kesalahan pengukuran akibat perbedaan gain, offset, dan phase sehingga hasil pengukuran tegangan, arus, daya, dan energi dapat dibandingkan dengan nilai referensi.

Pada tahap sebelum hardware ADE9000 tersedia, prosedur ini digunakan untuk menentukan urutan proses, parameter yang diperlukan, register yang digunakan, serta perhitungan *calibration coefficient* melalui *device mock*. Setelah hardware tersedia, prosedur yang sama akan diterapkan pada ADE9000 melalui STM32U585VIT6 dan antarmuka SPI.

---

## 2. Ruang Lingkup
Prosedur kalibrasi mencakup:
* Inisialisasi dan persiapan konfigurasi ADE9000.
* Kalibrasi gain arus RMS.
* Kalibrasi gain tegangan RMS.
* Kalibrasi offset arus RMS.
* Kalibrasi offset tegangan RMS.
* Kalibrasi phase.
* Kalibrasi power gain.
* Kalibrasi power offset.
* Verifikasi hasil kalibrasi.
* *Dry-run* menggunakan ADE9000 *device mock*.
* Transfer prosedur dari host/mock ke STM32U585VIT6 dan ADE9000.

---

## 3. Konsep Dasar Kalibrasi
Jalur pengukuran secara umum:
$$\text{Sinyal listrik} \rightarrow \text{sensor/AFE} \rightarrow \text{ADC ADE9000} \rightarrow \text{raw measurement} \rightarrow \text{calibration} \rightarrow \text{nilai metrologi}$$

Kalibrasi dilakukan dengan memberikan input yang nilainya diketahui, membaca hasil pengukuran ADE9000, membandingkan hasil tersebut dengan nilai yang seharusnya, kemudian menghitung *correction coefficient*. *Coefficient* tersebut selanjutnya ditulis ke register kalibrasi ADE9000.

### 3.1 Rumus Konversi Kode ke Besaran Fisik (*Conversion Constant*)
Setelah register kalibrasi diterapkan, nilai bacaan dari register ADE9000 dikonversikan menjadi besaran listrik menggunakan persamaan berikut:

$$V = x\text{VRMS} \times \text{Voltage Conversion Constant} \times 10^{-6}$$

$$I = x\text{IRMS} \times \text{Current Conversion Constant} \times 10^{-6}$$

$$P = x\text{WATT} \times \text{Power Conversion Constant} \times 10^{-3}$$

$$E = x\text{WATTHR\ HI} \times \text{Energy Conversion Constant} \times 10^{-6}$$

---

## 4. Register Kalibrasi

| Parameter | Phase A | Phase B | Phase C | Fungsi |
| :--- | :--- | :--- | :--- | :--- |
| **Current gain** | `AIGAIN` | `BIGAIN` | `CIGAIN` | Koreksi gain arus |
| **Voltage gain** | `AVGAIN` | `BVGAIN` | `CVGAIN` | Koreksi gain tegangan |
| **Current RMS offset** | `AIRMSOS` | `BIRMSOS` | `CIRMSOS` | Koreksi offset arus RMS |
| **Voltage RMS offset** | `AVRMSOS` | `BVRMSOS` | `CVRMSOS` | Koreksi offset tegangan RMS |
| **Phase calibration** | `APHCAL0` | `BPHCAL0` | `CPHCAL0` | Koreksi eror phase |
| **Power gain** | `APGAIN` | `BPGAIN` | `CPGAIN` | Koreksi gain daya |
| **Active power offset** | `AWATTOS` | `BWATTOS` | `CWATTOS` | Koreksi offset daya aktif |
| **Reactive power offset**| `AVAROS` | `BVAROS` | `CVAROS` | Koreksi offset daya reaktif |

> Untuk tahap awal ini, metode *single-point calibration* dapat digunakan terlebih dahulu. Penggunaan *multipoint calibration* perlu ditetapkan kemudian apabila proyek membutuhkan kompensasi berdasarkan beberapa rentang arus.

---

## 5. Peralatan dan Kondisi Pengujian

Pada tahap **implementasi hardware**, diperlukan:
* STM32U585VIT6
* ADE9000
* Sumber tegangan dan arus AC yang dapat dikendalikan
* Reference meter atau power analyzer
* Beban atau perangkat pengaturan *power factor*
* Sistem komunikasi SPI
* Perangkat host untuk *logging* dan analisis data

Pada tahap **host-side**, sumber listrik dan ADE9000 aktual digantikan oleh *device mock* yang menghasilkan *register value* sintetis dan eror yang dapat dikendalikan.

| Parameter | Nilai |
| :--- | :--- |
| **Tegangan nominal** | Menyesuaikan hardware |
| **Arus nominal** | Menyesuaikan hardware |
| **Frekuensi** | Menyesuaikan hardware |
| **PF phase calibration** | 0,5 lagging |
| **PF power gain** | 1 |
| **Current transfer function** | Menyesuaikan hardware |
| **Voltage transfer function** | Menyesuaikan hardware |
| **PGA (Programmable Gain Amplifier) gain** | Menyesuaikan hardware |
| **Full-scale RMS code** | 52.702.092 |
| **Full-scale power code** | 20.694.066 |
| **Acceptance limit** | Mengikuti spesifikasi proyek |

---

## 6. Prosedur Kalibrasi

### Inisialisasi
1. Pastikan STM32U585VIT6 telah dapat menjalankan firmware yang diperlukan.
2. Pastikan komunikasi SPI antara STM32U585VIT6 dan ADE9000 telah diverifikasi pada tahap hardware.
3. Pada tahap mock, pastikan SPI *transaction layer* dan *device mock* dapat melakukan transaksi register *read/write*.
4. Pastikan register kalibrasi berada pada kondisi awal yang diketahui.
5. Konfigurasikan PGA, filter, *metrology configuration*, *frequency setting*, dan konfigurasi lain sesuai konfigurasi proyek.
6. Konfigurasikan *energy accumulation* yang diperlukan untuk pengukuran daya dan phase.
7. Pastikan konfigurasi tiga fase sesuai dengan channel A, B, dan C.
8. Catat seluruh parameter konfigurasi sebelum proses kalibrasi dimulai.

---

## 7. Kalibrasi Gain RMS

### 7.1 Kalibrasi Gain Arus
1. Terapkan arus nominal yang diketahui pada channel yang akan dikalibrasi.
2. Tunggu hingga pengukuran stabil.
3. Baca register `AIRMS` untuk Phase A.
4. Hitung *expected RMS code* berdasarkan input dan *full-scale*.
5. Bandingkan *expected code* dengan *measured code*.
6. Hitung *calibration coefficient*.
7. Tulis *coefficient* ke `AIGAIN`.
8. Baca kembali `AIGAIN`.
9. Ulangi pengukuran `AIRMS`.
10. Bandingkan hasil dengan *reference value*.
11. Ulangi untuk `BIGAIN` dan `CIGAIN`.

$$I_{\text{expected}} = \frac{I}{I_{\text{FS}}} \times \text{RMS}_{\text{FS}}$$

> *Nilai final coefficient belum ditetapkan karena bergantung pada transfer function, PGA, full-scale, dan rangkaian analog aktual.*

### 7.2 Kalibrasi Gain Tegangan
1. Terapkan tegangan nominal.
2. Baca `AVRMS`.
3. Tentukan *expected RMS code*.
4. Hitung *voltage gain correction*.
5. Tulis hasil ke `AVGAIN`.
6. *Read-back register*.
7. Ulangi pengukuran.
8. Verifikasi terhadap *reference voltage*.
9. Ulangi untuk `BVGAIN` dan `CVGAIN`.

---

## 8. Kalibrasi RMS Offset

### 8.1 Current RMS Offset
1. Terapkan tegangan sesuai kondisi pengujian.
2. Berikan arus kalibrasi kecil.
3. Baca `AIRMS`.
4. Tentukan *expected RMS code*.
5. Hitung `AIRMSOS`.
6. Tulis nilai ke `AIRMSOS`.
7. *Read-back register*.
8. Ulangi pengukuran.
9. Verifikasi eror.
10. Ulangi untuk `BIRMSOS` dan `CIRMSOS`.

$$I_{\text{expected}} = \frac{I_{\text{calibration}}}{I_{\text{FS}}} \times \text{RMS}_{\text{FS}}$$

$$\text{AIRMSOS} = \frac{I_{\text{expected}}^2 - I_{\text{measured}}^2}{2^{15}}$$

### 8.2 Voltage RMS Offset
1. Terapkan kondisi input tegangan sesuai titik offset yang ditentukan.
2. Baca `AVRMS`.
3. Tentukan *expected RMS code*.
4. Hitung `AVRMSOS`.
5. Tulis `AVRMSOS`.
6. *Read-back*.
7. Ulangi pengukuran.
8. Verifikasi hasil.
9. Ulangi untuk `BVRMSOS` dan `CVRMSOS`.

---

## 9. Kalibrasi Phase

Phase calibration dilakukan untuk mengoreksi perbedaan phase antara jalur tegangan dan arus yang dapat memengaruhi pengukuran *active power*, *reactive power*, *power factor*, dan *energy*.

Kondisi pengujian menggunakan tegangan dan arus nominal dengan **power factor = 0,5 lagging**.

1. Terapkan tegangan nominal.
2. Terapkan arus nominal.
3. Atur *power factor* menjadi 0,5 lagging.
4. Konfigurasikan *energy accumulation* pada register `EP_CFG` serta pastikan register threshold energi telah diatur ke nilai rekomendasi (`WTHR` / `VARTHR` = `0x00100000`).
5. Jalankan akumulasi selama waktu yang ditentukan.
6. Baca `AWATTHR_HI`.
7. Baca `AVARHR_HI`.
8. Hitung *phase error*.
9. Hitung *phase calibration coefficient*.
10. Tulis *coefficient* ke `APHCAL0`.
11. *Read-back register*.
12. Ulangi pengukuran.
13. Verifikasi error phase.
14. Ulangi untuk Phase B dan C:
    * Phase A $\rightarrow$ `APHCAL0`
    * Phase B $\rightarrow$ `BPHCAL0`
    * Phase C $\rightarrow$ `CPHCAL0`

> Rumus *phase error* yang digunakan dalam implementasi C harus diambil langsung dari versi UG-1098 yang menjadi acuan proyek. Rumus dari contoh kode lain tidak langsung digunakan sebelum dilakukan review.

---

## 10. Kalibrasi Power Gain

**Kondisi pengujian:** Tegangan nominal, arus nominal, dan **power factor = 1**.

**Langkah-langkah:**
1. Terapkan tegangan nominal.
2. Terapkan arus nominal.
3. Atur PF = 1.
4. Konfigurasikan *energy accumulation* pada register `EP_CFG` serta pastikan register threshold energi telah diatur ke nilai rekomendasi (`WTHR` / `VARTHR` = `0x00100000`).
5. Jalankan akumulasi selama waktu yang ditentukan.
6. Baca `AWATTHR_HI`.
7. Hitung *expected energy code*.
8. Bandingkan *expected* dengan *measured*.
9. Hitung `APGAIN`.
10. Tulis `APGAIN`.
11. *Read-back*.
12. Ulangi pengukuran.
13. Verifikasi hasil.
14. Ulangi untuk Phase B dan C.

$$\text{APGAIN} = \frac{\text{AWATTHR HI}_{\text{expected}} - \text{AWATTHR HI}_{\text{measured}}}{\text{AWATTHR HI}_{\text{measured}}} \times 2^{27}$$

---

## 11. Kalibrasi Power Offset

Power offset dilakukan untuk mengurangi eror pada daerah pengukuran daya rendah.

### 11.1 Active Power Offset
1. Terapkan tegangan nominal.
2. Terapkan arus kalibrasi kecil.
3. Konfigurasikan *energy accumulation* pada register `EP_CFG` serta pastikan register threshold energi telah diatur ke nilai rekomendasi (`WTHR` / `VARTHR` = `0x00100000`).
4. Baca `AWATTHR_HI`.
5. Hitung *expected energy code*.
6. Hitung `AWATTOS`.
7. Tulis `AWATTOS`.
8. *Read-back*.
9. Ulangi pengukuran.
10. Verifikasi hasil.
11. Ulangi untuk `BWATTOS` dan `CWATTOS`.

### 11.2 Reactive Power Offset
**Kondisi pengujian:** Tegangan nominal, arus kalibrasi, dan **PF = 0**.

1. Baca register energi/daya reaktif.
2. Hitung *expected value*.
3. Hitung `AVAROS`.
4. Tulis `AVAROS`.
5. *Read-back*.
6. Verifikasi.
7. Ulangi untuk `BVAROS` dan `CVAROS`.

---

## 12. Urutan Keseluruhan Kalibrasi

$$\text{Init Configuration} \longrightarrow \text{Current Gain (AIGAIN/BIGAIN/CIGAIN)} \longrightarrow \text{Voltage Gain (AVGAIN/BVGAIN/CVGAIN)}$$
$$\downarrow$$
$$\text{RMS Offset (Current and Voltage Offset)} \longrightarrow \text{Phase Calibration (APHCAL0/BPHCAL0/CPHCAL0)}$$
$$\downarrow$$
$$\text{Power Gain (APGAIN/BPGAIN/CPGAIN)} \longrightarrow \text{Power Offset (Active and Reactive Power Offset)}$$
$$\downarrow$$
$$\text{Verification} \longrightarrow \text{Save Calibration Result}$$

---

## 13. Dry-Run pada Device Mock

Saat hardware belum tersedia, prosedur belum dijalankan pada ADE9000 fisik. *Dry-run* dilakukan dengan *device mock*. Mock harus menyediakan:
* Nilai register metrologi sintetis.
* Eror gain yang dapat diatur.
* Eror offset yang dapat diatur.
* Eror phase yang dapat diatur.
* Eror power gain/offset.
* Register kalibrasi yang dapat ditulis.
* Register kalibrasi yang dapat dibaca kembali.
* Output pengukuran yang berubah setelah *calibration coefficient* diterapkan.

$$\text{Reference Input} \rightarrow \text{Simulated ADE9000 Measurement} \rightarrow \text{Inject Measurement Error} \rightarrow \text{Read Mock Register}$$
$$\downarrow$$
$$\text{PASS / FAIL} \leftarrow \text{Compare With Reference} \leftarrow \text{Read Measurement Again} \leftarrow \text{Apply Correction} \leftarrow \text{Write Calibration Register} \leftarrow \text{Calibration Calculation}$$

| Test | Kondisi Mock | Hasil yang Diharapkan |
| :--- | :--- | :--- |
| **Current gain** | Measurement < expected | Correction gain positif |
| **Voltage gain** | Measurement > expected | Correction gain negatif |
| **RMS offset** | Ada residual error | Offset correction dihitung |
| **Phase** | Phase error disimulasikan | `APHCAL0` menghasilkan koreksi |
| **Power gain** | Measured power < expected | `APGAIN` mengoreksi skala |
| **Power offset** | Residual power pada input rendah | `AWATTOS`/`AVAROS` mengoreksi residual |

---

## 14. Verifikasi

Setelah seluruh *calibration coefficient* diterapkan:
1. Baca kembali seluruh register kalibrasi.
2. Lakukan pengukuran ulang.
3. Bandingkan hasil ADE9000 dengan nilai referensi.
4. Hitung *measurement error*.
5. Uji pada beberapa kondisi input yang ditetapkan.
6. Catat hasil pengukuran.
7. Evaluasi terhadap *acceptance limit* yang telah ditetapkan oleh proyek.

$$\text{Persentase Eror %\} = \frac{X_{\text{ADE9000}} - X_{\text{ref}}}{X_{\text{ref}}} \times 100\%$$ %

> Untuk kondisi referensi yang mendekati nol, evaluasi tidak sebaiknya menggunakan persentase relatif biasa; batas *absolute error/tolerance* perlu ditentukan secara terpisah. Batas kelulusan tidak ditetapkan pada draft ini karena *acceptance limit* harus mengikuti spesifikasi proyek dan standar yang berlaku.

---

## 15. Data yang Dicatat

| Data | Keterangan |
| :--- | :--- |
| **Test ID** | Identitas pengujian |
| **Channel** | A/B/C |
| **V reference** | Tegangan referensi |
| **I reference** | Arus referensi |
| **PF** | Power factor |
| **f** | Frekuensi |
| **Raw register** | Nilai sebelum kalibrasi |
| **Expected code** | Nilai yang diharapkan |
| **Calibration coefficient** | Hasil perhitungan |
| **Register address** | Register tujuan |
| **Written value** | Nilai yang ditulis |
| **Read-back value** | Nilai hasil pembacaan ulang |
| **Corrected measurement** | Hasil setelah koreksi |
| **Reference measurement** | Nilai referensi |
| **Eror** | Selisih/eror |
| **Pass/Fail** | Hasil verifikasi |

---

## 16. Transfer dari Mock ke Hardware

**Pada tahap host-side:**
$$\text{Host Test} \rightarrow \text{Device Mock} \rightarrow \text{Calibration Engine}$$

**Setelah dry-run dinyatakan benar:**
$$\text{STM32U585VIT6} \rightarrow \text{SPI Transaction Layer} \rightarrow \text{ADE9000 Register} \rightarrow \text{Metrology Measurement} \rightarrow \text{Calibration Engine} \rightarrow \text{Calibration Register}$$

> Konsep perhitungan tidak berubah, tetapi *interface device* diganti dari *mock* menjadi *real SPI driver*.
