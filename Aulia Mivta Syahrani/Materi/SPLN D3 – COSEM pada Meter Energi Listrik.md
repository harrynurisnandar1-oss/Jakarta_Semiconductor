#### **Materi SPLN D3 – COSEM pada Meter Energi Listrik**



###### **SPLN (Standar Perusahaan Listrik Negara)**



**SPLN** → menetapkan standar/kriteria teknis yang harus dipenuhi perangkat kelistrikan PLN → Len mengembangkan/menyediakan teknologi dan perangkat yang memenuhi kebutuhan tersebut.

**Meter Statik Pascabayar Fase Tiga** adalah alat pengukur konsumsi energi listrik berbasis elektronik (solid-state) yang menggunakan sistem tiga arus bolak-balik (tiga fase) dengan sistem pembayaran tagihan di akhir bulan berdasarkan jumlah pemakaian (pascabayar).

SPLN ini membahas COSEM serta Penerapan IEC 62056-6-2.



**COSEM** (Companion Specification for Energy Metering) adalah standar/komponen dari DLMS/COSEM yang digunakan untuk mendefinisikan cara data pada meter energi (smart meter) direpresentasikan dan diakses.

COSEM: aturan/model untuk menggambarkan objek dan data yang ada di energy meter agar bisa dibaca atau dikendalikan melalui komunikasi standar DLMS.

Contohnya, data seperti energi aktif (kWh), tegangan, arus, dan daya dapat direpresentasikan sebagai objek COSEM sehingga sistem lain dapat mengaksesnya secara terstruktur.



**Acuan Normatif**

SPLN D3

IEC



**Istilah dan Definisi**

Client: Perangkat yang meminta data atau layanan. Aplikasi pembaca meter secara umum disebut sebagai client.

Server: Perangkat yang menyediakan data atau layanan. Meter secara umum disebut sebagai server.



**Daftar singkatan**

AES : Advanced Encryption Standard

APDUs : Application Protocol Data Units

COSEM : Companion Specification for Energy Metering

CTT : Conformance Test Tool

HDLC : High-level Data Link Control

HES : Head End System

HLS : High Level Security

IC : Interface Class

ID : Identifier

LLS : Low Level Security

LN : Logical Name

MAC : Medium Access Control

OBIS : Object Identification System

PDU : Protocol Data Unit

SN : Short Name

xDlms : eXtended DLMS



**OBIS** adalah kode identifikasi yang digunakan objek COSEM pada meter.

**Kode OBIS harus unik** untuk setiap data yang dimiliki oleh meter.

**Kode OBIS terdiri dari enam (6) grup** sebagai berikut:

Grup A mengatur pengkodean objek (kode 0 untuk objek abstrak dan kode 1 untuk electricity).

Grup B mengatur penentuan channel.

Grup C mengatur besaran fisika (arus, tegangan, daya, dan lain-lain).

Grup D mengatur hal-hal yang spesifik, seperti energi billing yang digunakan untuk perhitungan tagihan.

Grup E mengatur tipe pengukuran misalkan pada rentang tertentu.

Grup F mengatur pemisahan hasil.



Berikut **struktur dari buffer data historikal**:

array        capture\_object\_definition

capture\_object\_definition ::= structure

{

&#x20;   class\_id:          long-unsigned;

&#x20;   logical\_name:      octet-string;

&#x20;   attribute\_index:   integer;

&#x20;   data\_index:        long-unsigned;

}



“class\_id” merupakan class\_id untuk elemen dari objek yang direkam.

“logical\_name” merupakan kode OBIS dari objek yang direkam.

“attribute\_index” merupakan nomor index atribut dari objek yang direkam.

“data\_index” merupakan nomor index dari data objek tersebut.



**Tegangan swell (voltage swell)** adalah peningkatan nilai tegangan listrik secara tiba-tiba di atas batas normal yang berlangsung dalam waktu singkat.

Menurut standar IEEE 1159-1995, voltage swell adalah kenaikan tegangan RMS (Root Mean Square) sebesar 110% hingga 180% dari tegangan nominal.



**FIFO** : first in first out

digunakan untuk skema capture object

**LIFO** : last in first out



Dalam tabel Atribut dari objek "Alarm register 1 (PLN specific)

R = Read --> logical\_name --> Operator, Supervisor, Administration

W = Write --> value --> W

Dalam objek alarm register, ditentukan default untuk bit yang dimonitor sehingga akan menyebabkan push (sebagai Alarm Filter) adalah 5, 7, 8, 9, 12, 15, 19, dan 20.



Objek untuk kebutuhan proses push alarm register yang harus disediakan meter:

Objek alarm register --> alarm descriptor, alarm filter, alarm monitor, alarm push on/off script



**AR** (Auto Reclose) --> prosesnya atau (Auto Recloser) --> alat relainya adalah Fitur proteksi yang bekerja membuka sirkuit saat mendeteksi gangguan arus lebih (seperti korsleting akibat petir atau dahan pohon), lalu secara otomatis menutup kembali (reclose) jaringan listrik dalam hitungan detik setelah gangguan sementara itu hilang.



**Proses Firmware Upgrade**

* Fasa inisialisasi
* Firmware (Image) Transfer
* Firmware (Image) Check
* Firmware (Image) Activation



**Protokol komunikasi antara meter dengan modem** adalah komunikasi 3 layer. connection oriented berbasis HDLC.

**Objek HDLC setup** merupakan objek yang berfungsi untuk memberikan informasi terkait dengan port komunikasi optical/RS-485/RS-232 pada kofigurasi HDLC protocol serta sebagai interface/antarmuka konfigurasi port komunikasi tersebut. Objek HDLC setup ini dimodelkan dengan class\_id 23.

