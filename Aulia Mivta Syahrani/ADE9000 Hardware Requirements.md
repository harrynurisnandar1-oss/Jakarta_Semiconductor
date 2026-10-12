# **KEBUTUHAN HARDWARE UNTUK METROLOGI DAN KALIBRASI ADE9000**

Jumlah perangkat di bawah merupakan usulan untuk satu set pengujian, kecuali jumlah Spin 1 dan Spin 2 yang mengacu pada rencana kerja.

# **1\. Perangkat keras utama dan instrumen pengujian**

| No. | Perangkat | Jumlah | Fungsi |
| :---: | :---- | :---- | :---- |
| 1 | PCB Spin 1 — board evaluasi tegangan rendah yang memuat ADE9000 dan mikrokontroler STM32U585VIT6 | 1 unit untuk satu set pengujian; berdasarkan rencana kerja 🡪  8–10 unit | Pengembangan dan pengujian awal ADE9000 pada kondisi tegangan rendah. Digunakan untuk memeriksa komunikasi SPI, interrupt, pembacaan register, rangkaian masukan, dan firmware tanpa menghubungkan board ke tegangan jala-jala. |
| 2 | PCB Spin 2 — board smart meter 3-fasa lengkap untuk pengujian tegangan jala-jala | 1 unit untuk pengujian; berdasarkan rencana kerja 🡪  5–6 unit | Pengujian rancangan yang lebih lengkap, termasuk masukan 3-fasa, catu daya, proteksi, terminal, serta fitur tambahan sesuai rancangan. Pengoperasian harus mengikuti prosedur keselamatan dan protokol pengujian. |
| 3 | Catu daya DC (bench DC power supply) |  | Memberikan catu DC yang terkontrol untuk board Spin 1 dan rangkaian pendukung selama bring-up serta pengujian firmware. 🡪  untuk menyalakan PCB ADE9000 |
| 4 | Sumber AC tegangan rendah terisolasi |  | Memberikan sinyal AC terisolasi untuk pengujian awal masukan pengukuran pada Spin 1 dengan risiko lebih rendah daripada pengujian langsung pada tegangan jala-jala. 🡪 input pengukuran (sinyal yang akan diukur) |
| 5 | Perangkat injeksi sinyal (signal injection rig) \~ signal generator | 1 | Menginjeksikan sinyal uji ke masukan tegangan atau arus sesuai metode pengujian. Membantu menguji respons kanal, pembacaan ADE9000, serta algoritma metrologi secara terkendali. |
| 6 | Sumber tegangan dan arus AC 3-fasa |  | Menghasilkan kondisi uji 3-fasa yang terkontrol untuk memeriksa pembacaan tegangan, arus, daya, energi, dan parameter metrologi pada Spin 2\. |
| 7 | Meter referensi 3-fasa | 1 unit | Menjadi pembanding nilai ukur smart meter yang diuji. Spesifikasi dan kemampuan meter referensi perlu dipastikan sesuai besaran, kelas akurasi, serta rentang harmonisa yang disyaratkan. |
| 8 | Precision Digital Power Analyzer, misalnya Yokogawa WT3000E | 1 unit bila dibutuhkan untuk pengujian terkait (usulan) | Mengukur parameter daya secara presisi. Dokumen spesifikasi menyebut WT3000E dalam konteks verifikasi konsumsi daya. 🡪 mengukur konsumsi daya listrik smart meter itu sendiri.   |
| 9 | Debugger/programmer SWD untuk STM32 \~ ST-LINK | 1 | Memprogram mikrokontroler, melakukan debugging firmware, serta memeriksa alur eksekusi dan komunikasi dengan ADE9000. |
| 10 | Multimeter digital | 1 | Memeriksa tegangan DC, kontinuitas, dan kondisi dasar rangkaian ketika perakitan, bring-up, serta troubleshooting. |
| 11 | Osiloskop beserta probe | 1 | Mengamati bentuk gelombang, level logika, sinyal SPI, sinyal interrupt, dan kestabilan sinyal pada titik uji. |
| 12 | Kabel uji, probe, dan konektor |  | Menghubungkan sumber sinyal, board uji, dan instrumen referensi dengan aman serta sesuai konfigurasi pengujian. |

# 

# **2\. Komponen yang perlu tersedia pada board rangkaian**

Komponen berikut merupakan bagian dari rancangan board, bukan instrumen seluruhnya. Jumlah akhirnya mengikuti skema dan revisi BOM (Bill of Materials) yang disetujui.

| Komponen | Jumlah/acuan | Fungsi |
| :---- | :---- | :---- |
| ADE9000 | 1 IC per PCB | IC metrologi untuk akuisisi dan pengolahan pengukuran listrik, termasuk parameter tegangan, arus, daya, energi, serta fitur analisis yang didukung konfigurasi. |
| STM32U585VIT6 | 1 mikrokontroler per PCB | Menjalankan firmware, berkomunikasi dengan ADE9000 melalui SPI, membaca status/interrupt, melakukan pemrosesan lanjutan, dan mengelola komunikasi sistem. |
| Rangkaian masukan tegangan dan pembagi tegangan \~ voltage divider | Kanal fasa sesuai rancangan, umumnya A/B/C | Menyesuaikan sinyal tegangan masukan agar berada pada rentang yang dapat diterima rangkaian pengukuran. |
| Antarmuka masukan arus / conditioning CT | Kanal sesuai rancangan, termasuk netral bila disyaratkan | Mengondisikan sinyal arus dari CT eksternal agar sesuai untuk masukan ADE9000. |
| FRAM | 1 komponen per PCB sesuai rancangan | Menyimpan data atau konfigurasi yang perlu dipertahankan sesuai implementasi firmware. |
| Test point dan jumper/tautan 0-ohm. Titik uji pada PCB. Resistor 0 Ohm sebagai penghubung jalur listrik PCB. | Pada titik penting sesuai layout | Memudahkan pengukuran, isolasi bagian rangkaian, pemeriksaan sinyal, dan proses bring-up/debugging. |

# **3\. Urutan penggunaan perangkat**

1. Pengujian awal/bring-up: gunakan PCB Spin 1, catu daya DC, sumber AC tegangan rendah terisolasi atau perangkat injeksi sinyal, multimeter, osiloskop, serta debugger SWD.  
2. Verifikasi metrologi: berikan kondisi uji yang diketahui, baca hasil ukur ADE9000 melalui firmware, lalu bandingkan hasilnya dengan meter referensi atau instrumen acuan yang sesuai.  
3. Pengujian 3-fasa: setelah prosedur keselamatan dan kelayakan board dikonfirmasi, gunakan sumber tegangan/arus AC 3-fasa untuk menguji PCB Spin 2\.  
4. Kalibrasi: tentukan parameter yang akan dikalibrasi (misalnya gain, offset, atau phase), lakukan pengaturan berdasarkan hasil pembandingan terhadap acuan, kemudian ulangi pengukuran untuk memverifikasi hasil.  
5. Pencatatan hasil: dokumentasikan konfigurasi sumber, nilai referensi, nilai yang dibaca smart meter, selisih/error, kondisi pengujian, serta hasil sebelum dan sesudah kalibrasi.

# **Dokumen acuan**

* Dokumen rencana kerja 26 minggu (revisi D, tanpa evaluation board).  
* Proposal proyek smart meter 3-fasa.  
* Dokumen spesifikasi smart meter 3-fasa.