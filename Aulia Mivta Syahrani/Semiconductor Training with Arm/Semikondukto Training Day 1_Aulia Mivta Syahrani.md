**PELATIHAN SEMIKONDUKTOR BERSAMA ARM UNTUK TALENTA INDONESIA**  
**Hari Pertama**

1. **PENDAHULUAN**  
   Waktu dan Tempat: Grha ITS Surabaya, 20 Oktober 2026 pukul 08.00 – 17.00 WIB.  
   Topik: Mengenal Teknologi Arm, Arsitektur Prosesor, dan Penerapannya Dalam Pengembangan Teknologi Semikonduktor.  
     
2. **RANGKAIAN KEGIATAN DAN MATERI PELATIHAN**  
   2.1 **Pembukaan Program Pelatihan**  
> > > * Sambutan oleh Dwi Susanto, Ph.D. *Managing Director*, *Technology Advancement* Danantara Indonesia

  Pelatihan ini diselenggarakan agar Indonesia bisa merancang chip secara mandiri dan tidak bergantung pada chip impor.

  Industri semikonduktor terdiri dari Desain 🡪 Foundry 🡪 ATP

> > > * Sambutan oleh Bapak Heri Kuswantoro, Direktur Bina Talenta Penelitian dan Pengembangan Ditjen Diktisaintek.

  Setelah mengikuti pelatihan luring selama tiga hari, peserta akan melanjutkan pembelajaran secara mandiri melalui pelatihan daring. Oleh karena itu, diperlukan konsistensi dan keberlanjutan dalam mengikuti setiap pembelajaran agar materi yang diperoleh dapat dipahami dengan baik.

> > > * *Opening remarks* oleh Rifqi Ansari, S.TP., M.T., MBA, Direktur Industri Elektronika dan Telematika.

  Terbatasnya pasokan chip dan prosesor di Indonesia mengakibatkan biaya peralatan elektronik meningkat. Oleh karena itu, Indonesia perlu mengejar ketertinggalan dalam hal tersebut produksi chip dalam negeri.

> > > * Sambutan oleh Prof. Dr. (HC). Ir. Bambang Pramujati, S.T., M.Sc.Eng., Ph.D., IPU, ASEAN.Eng. selaku Rektor ITS.

  Diperlukan pengembangan SDM yang kompeten dan peningkatan kemampuan dalam desain chip agar Indonesia dapat berkontribusi dalam industri semikonduktor.


  2.2 ***Introduction to Arm***

  Matt Rushton selaku Director, *Arm Training Services*

> > > * Arm merupakan perusahaan teknologi yang berasal dari Inggris dan memiliki jaringan kantor di berbagai negara di dunia.  
> > > * Arm mengedepankan efisiensi daya (*power efficiency*), model bisnis berbasis lisensi (*licensing model*), dan ekosistem teknologi yang luas.  
> > > * Teknologi Arm mencakup berbagai segmen pasar, mulai dari perangkat *edge*, kecerdasan buatan, PC, komputasi awan (*cloud computing*), hingga *Internet of Things* (IoT).  
> > > * Arm didirikan pada tahun **1990\.**  
> > > * Arsitektur Arm, IP, Compute Subsystem and Silicon

  Arsitektur Arm mencakup spesifikasi arsitektur yang didokumentasikan dalam Arm Architecture Reference Manual.

  IP Arm meliputi CPU, GPU, NPU, dan system IP yang mendukung pengembangan sistem berbasis semikonduktor.

  Pengmbangan silicon untuk Arm AGI CPU.

> > > * Sejak 1900 sampai sekarang Arm telah mendistribusikan lebih dari **350 miliar chip.**  
> > > * Arm merupakan platform komputasi yang paling tersebar luas di dunia, dengan teknologi yang menjangkau **100 %** populasi global.


  2.3 ***Introduction to Arm CPU*** **– Cortex-A**

  Yun Yi, *Application Engineer*, Arm.

> > > * **Arm Cortex-A** merupakan kelompok prosesor Arm yang dirancang untuk perangkat komputasi **aplikasi** dan umumnya mampu menjalankan sistem operasi lengkap (*full operating system*), seperti Linux.  
> > > * Arsitektur Arm mengenal **set instruksi ARM dan Thumb**. **Instruksi ARM** secara klasik menggunakan panjang instruksi **32-bit**, sedangkan **Thumb** pada awalnya memperkenalkan instruksi **16-bit** untuk meningkatkan kepadatan kode program.  
> > > * **SIMD**

  *Single Instruction, Multiple Data* (SIMD) memungkinkan satu instruksi melakukan operasi yang sama terhadap beberapa elemen data secara bersamaan. SIMD bermanfaat untuk mempercepat pemrosesan multimedia, gambar, dan data numerik.

> > > * **NEON**

  NEON merupakan teknologi Advanced SIMD pada sejumlah prosesor Arm yang mendukung pemrosesan data paralel. Teknologi ini membantu mempercepat pemrosesan sinyal, dan aplikasi lain yang dapat memanfaatkan operasi vektor.

> > > * **ARMv4, ARMv5, dan ARMv6**

  Generasi arsitektur ini membawa pengembangan kemampuan instruksi secara bertahap, termasuk **dukungan Thumb** **pada ARMv4T** serta peningkatan instruksi pemrosesan sinyal dan multimedia pada generasi berikutnya.

> > > * **Armv8-A memperkenalkan 2 *execution state***, yaitu **AArch32** untuk pemrosesan 32-bit dan **AArch64** untuk pemrosesan 64-bit. Perkembangan ini memungkinkan prosesor mendukung operasi data dan pengalamatan memori yang lebih luas.  
> > > * **Perbedaan AArch32 dan AArch64**

  **AArch32** menggunakan **General Purpose Register / GPR dengan lebar 32-bit** dan mendukung set instruksi A32 serta T32 (*Thumb*), sedangkan **AArch64** menggunakan **register 64-bit** dan set instruksi A64.

> > > * **AArch64 memiliki 31 GPR**, yaitu X0 hingga X30, yang masing-masing berukuran 64-bit.  
> > > * Model pemrograman arsitektur Arm

  **Arsitektur prosesor** mendefinisikan **cara perangkat lunak berinteraksi dengan CPU, termasuk register, set instruksi, model memori, dan mekanisme penanganan pengecualian** (*exception*). 

> > > * Exception Level pada AArch64

  **AArch64** menyediakan 4 tingkat pengecualian, yaitu **EL0, EL1, EL2, dan** EL3, yang memiliki tingkat hak akses berbeda.

  EL0 digunakan untuk aplikasi.

  EL1 untuk sistem operasi.

  EL2 untuk pengelolaan virtualisasi.

  EL3 untuk fungsi pengelolaan keamanan tingkat tertinggi.

> > > * **Armv9-A** merupakan pengembangan dari Armv8-A yang membawa **perluasan kemampuan arsitektur,** termasuk dukungan untuk komputasi vektor, pemrosesan AI, dan peningkatan keamanan sistem.


  2.4 ***Company Showcase*** **– Polytron**

  Joegianto, Senior Manager Business Development Polytron

> > > * POLYTRON berdiri pada 18 September 1975 di Kudus, Jawa Tengah.  
> > > * Chip semikonduktor merupakan komponen penting dalam berbagai perangkat elektronik.  
> > > * Fokus teknologi *meliputi electric vehicle, renewable energy, solar technology*, Silicon Carbide (SiC) 🡪 material semikonduktor yang digunakan dalam elektronika daya, terutama untuk aplikasi tegangan dan daya tinggi.  
> > > * Polytron mendukung pengembangan talenta desain ship di Indonesia. Salah satu tujuan jangka panjangnya adalah mendorong tumbuhnya *startup* yang bergerak di bidang semikonduktor.  
    
  2.5 ***Introduction to Arm CPU*** **– Cortex-R and Cortex-M**

  Tim Menasveta, *FAE Director*

> > > * *Functional Safety* berdasarkan **ISO 26262**

  *Functional safety* merupakan bagian dari keselamatan sistem yang berkaitan dengan risiko bahaya akibat kegagalan fungsi sistem. Dalam ISO 26262, didefinisikan sebagai kondisi tidak adanya risiko yang tidak dapat diterima akibat bahaya yang disebabkan oleh malfungsi sistem.

> > > * *Arm Functional Safety Package*

  Arm menyediakan paket dokumentasi keselamatan fungsional untuk membantu pengembang mengevaluasi teknologi prosesor. Paket ini mencakup *Safety Manual*, *Verification Summary Report*, FMEDA, dan *Dependent Failure Analysis* (DFA) *Reporttional Safety* berdasarkan ISO 26262\.

  **Safety manual berisi panduan mengenai penggunaan komponen** atau teknologi prosesor dalam sistem yang menerapkan persyaratan keselamatan fungsional.

  **FMEDA** (*Failure Modes, Effects and Diagnostic Analysis*) **merupakan analisis terhadap kemungkinan mode kegagalan** suatu komponen, dampak yang ditimbulkannya.

  **DFA Report** (*Dependent Failure Analysis Report*) membahas analisis kegagalan yang saling bergantung, yaitu kondisi ketika satu penyebab kegagalan dapat memengaruhi beberapa bagian sistem sekaligus.

  Prosesor yang termasuk dalam perkembangan arsitektur ini adalah **Cortex-M55, Cortex-M85, dan Cortex-M52** 🡪 *Ease of development, energy-efficient compute, increased throughput, enchanced security, TrustZone*.

> > > * **Armv8.1-M** memperkenalkan teknologi **Helium**, yaitu ekstensi instruksi vektor yang dikenal sebagai ***M-Profile Vector Extension*** (MVE).  
> > > * Peningkatan fitur pada Cortex-M85, Cortex-M55, dan Cortex-M52 mencakup pengembangan kemampuan komputasi, keamanan melalui TrustZone dan PACBTI, serta aspek keandalan sistem yang berkaitan dengan RAS (*Reliability, Availability, and Serviceability*).  
> > > * Keunggulan arsitektur Cortex-M meliputi efisiensi biaya dan luas area chip, low power, kemampuan pemrosesan sinyal digital (DSP), *security*, kinerja pemrosesan (*performance*), serta dukungan *machine learning*.  
> > > * Ekstensi keamanan pada arsitektur Arm Cortex-M memungkinkan **pemisahan perangkat lunak dan sumber daya** sistem ke dalam lingkungan **Secure dan Non-secure**.  
> > > * Dalam pengembangan **aplikasi berbasis TrustZone**, **Secure Project menangani fungsi keamanan**, seperti secure boot, pengelolaan kunci kriptografi, sertifikat, dan pustaka kriptografi. Sementara itu, **User Project berisi aplikasi pengguna** dan komponen perangkat lunak lain yang berjalan dalam lingkungan **Non-secure**. 

  Keduanya dapat berkomunikasi melalui pemanggilan fungsi yang diizinkan (**function calls**). 

  **Secure boot**: mekanisme untuk memverifikasi keaslian perangkat lunak sebelum dijalankan. Bootloader memastikan bahwa firmware yang dimuat sesuai dengan persyaratan keamanan perangkat. Setelah inisialisasi, sistem dapat menjalankan RTOS (*Real-Time Operating System*) untuk mengelola tugas, penjadwalan, dan sumber daya perangkat.

> > > * Arm Tools untuk Cortex-M

  **Arm Keil dan Sistem Operasi (OS)** 🡪 Arm Keil menyediakan perangkat pengembangan perangkat lunak untuk prosesor Cortex-M, termasuk penulisan kode program, kompilasi, proses *debugging*, dan pengujian sistem *embedded*. Pengembangan juga dapat memanfaatkan RTOS.

  **CMSIS** (*Common Microcontroller Software Interface Standard*) 🡪 CMSIS merupakan standar antarmuka perangkat lunak yang dikembangkan Arm untuk mempermudah pengembangan dan penggunaan ulang kode pada mikrokontroler berbasis prosesor Cortex.

  **Arm Models** 🡪 Arm menyediakan model virtual prosesor untuk menyimulasikan perangkat lunak sebelum diterapkan pada perangkat keras, sehingga pengujian awal dapat dilakukan tanpa menggunakan **board** langsung.


  2.6 ***Introduction to Arm Mali: Mobile GPU Architecture and Graphics Fundamentals***

  Hayden Lim, **Software Engineer**

  **GPU** pada perangkat seluler digunakan untuk berbagai kebutuhan pemrosesan visual dan komputasi, antara lain:

> > > * User Interface (UI) 🡪 membantu pemrosesan grafis antarmuka pengguna dan komposisi gambar pada layar.  
> > > * 2D Graphics 🡪 memproses gambar dan elemen grafis dua dimensi.  
> > > * 3D Graphics dan Gaming 🡪 menghasilkan objek tiga dimensi, pencahayaan, tekstur, dan animasi dalam gim.  
> > > * Multimedia 🡪 mendukung pemrosesan grafis untuk konten multimedia.  
> > > * Computer Vision 🡪 membantu pemrosesan visual, seperti analisis gambar.  
> > > * Machine Learning (ML) 🡪 mempercepat operasi komputasi tertentu untuk aplikasi kecerdasan buatan.

  ***Graphics Rendering Pipeline***

  ![][image1]


  ![][image2]

  **Gambar 2.1 Proses *rendering pipeline***


  CPU bertugas menyiapkan dan mengatur pekerjaan, sedangkan GPU menjalankan pemrosesan grafis secara paralel.

  **Rendering pipeline** merupakan tahapan pemrosesan untuk mengubah data dan perintah grafis menjadi gambar di atas. Alurnya sebagai berikut:

* CPU CPU mengirimkan perintah (*commands*) serta referensi sumber daya yang diperlukan GPU.  
* GPU memproses geometri termasuk posisi dan bentuk objek 3D. Geometri diubah menjadi fragmen yang berkaitan dengan piksel (*Rasterization*).  
* Pada *pixel processing*, GPU menghitung warna dan atribut fragmen, termasuk penggunaan tekstur serta efek visual.  
* Hasil pemrosesan disimpan sebagai data gambar untuk ditampilkan. **Framebuffer** adalah bagian dari memori yang menyimpan data piksel dari sebuah tampilan video sebelum dipindai dan ditampilkan ke layar monitor.


  Salah satu karakteristik penting GPU Arm Mali adalah penggunaan *tile-based rendering*, yaitu membagi area gambar menjadi bagian-bagian kecil (*tile*). Tahapan:

1. GPU memproses geometri dan menentukan objek yang pada setiap *tile*.  
2. GPU memproses fragmen untuk setiap *tile*.  
3. Data sementara, seperti warna serta informasi kedalaman, dapat disimpan dalam memori lokal pada chip.  
4. Hasil *tile* ditulis ke memori untuk membentuk gambar akhir.

   

   Konsumsi Daya

   **Tabel 2.1 Konsumsi daya komponen dan sistem desktop**

| Komponen/sistem | Konsumsi daya |
| ----- | ----- |
| CPU desktop | 65–170 W |
| GPU dan VRAM desktop | 220–350 W |
| DRAM desktop | 20–40 W |
| Total sistem dekstop | Sekitar 300–600 W |
| Platform seluler | 4–8 W; juga dicatat 10–16 W sebagai nilai puncak (peak) |

   

   **Kurva Daya Prosessor**

   ![][image3]

   **Gambar 2.2 Grafik *performance versus power*.**

   

   Dalam desain GPU, peningkatan frekuensi atau aktivitas pemrosesan dapat meningkatkan performa, tetapi juga dapat meningkatkan daya dan suhu.

   

   Arsitektur GPU Arm Generasi ke-5

* *Execution Cores*

  Mendukung pemrosesan grafis yang lebih kompleks untuk gim dan aplikasi 3D secara real-time.

* *Memory Subsystem*

  Mengoptimalkan penggunaan *bandwidth* memori untuk mengurangi perpindahan data.

  Mengurangi kebutuhan *bandwidth* yang berkaitan dengan pemrosesan geometri melalui *Deferred Vertex Shading* (DVS).

* *Graphics Pipeline Traffic*

  Memperkenalkan *Deferred Vertex Shading* (DVS) untuk mengubah alur data dalam graphics pipeline.

* *Power Efficiency*

  Untuk meningkatkan efisiensi daya GPU dan sistem secara keseluruhan.

* *Scalability*

  DVS memungkinkan arsitektur mendukung konfigurasi dengan jumlah *core* lebih besar.

  Mendukung pengembangan GPU menuju tingkat performa yang lebih tinggi.

**Kesimpulan**

1. GPU mendukung grafis antarmuka pengguna, grafis 2D/3D, dan berbagai aplikasi visual.  
2. GPU seluler harus memenuhi kebutuhan performa dalam batas daya dan suhu perangkat.  
3. Pemrosesan per *tile* membantu mengurangi akses ke memori eksternal.  
4. GPU memanfaatkan pemrosesan paralel untuk menangani pekerjaan grafis dan komputasi.  
5. Performa, konsumsi daya, dan luas silikon merupakan pertimbangan penting dalam pengembangan produk.

			

2.7 **What Arm Technology Enables**

   Christion Jardine*, Software engineer, Developer relations*

   Teknologi Arm dan Perkembangan AI

* Teknologi Arm digunakan dalam berbagai segmen komputasi, termasuk perangkat seluler, PC, pusat data, dan sistem AI di edge.  
* Kebutuhan komputasi AI terus meningkat, termasuk penggunaan AI pada perangkat secara lokal (*on-device AI*) 🡪 mendukung pemrosesan dengan keterbatasan daya, memori, dan kebutuhan respons waktu nyata.  
  **Alif Ensemble E8 DevKit**  
> > > * Alif Ensemble E8 DevKit merupakan platform pengembangan sistem embedded yang menggabungkan prosesor berbasis Arm dengan akselerator neural network untuk aplikasi AI.  
> > > * CPU**: Arm Cortex-A32** dan **dua prosesor Arm Cortex-M55**.  
> > > * NPU: **satu Arm Ethos-U85** dan **dua Arm Ethos-U55**.  
> > > * Fungsi CPU: menjalankan program utama, mengatur sistem, dan mengoordinasikan pekerjaan AI.  
> > > * Fungsi NPU: mempercepat komputasi neural network untuk menjalankan inferensi AI secara efisien.

  ***Accelerating AI Across Frameworks and Ecosystem***

> > > * Model AI dapat dikembangkan menggunakan framework seperti **PyTorch** dan **diterapkan melalui** teknologi seperti **ExecuTorch**.  
> > > * Kompiler dan perangkat lunak pendukung membantu mengoptimalkan model agar sesuai dengan kemampuan CPU dan NPU target.  
  **Edge AI Constraints**  
  Penerapan AI pada perangkat *edge* menghadapi beberapa keterbatasan:  
> > > * *More inference* 🡪 meningkatnya kebutuhan menjalankan inferensi AI.  
> > > * *More contextual data* 🡪 kebutuhan memproses lebih banyak data kontekstual.  
> > > * *Always on* 🡪 sistem harus dapat beroperasi secara terus-menerus.  
> > > * *Limited power* 🡪 konsumsi daya harus dibatasi.  
> > > * *Real-time execution* 🡪 hasil pemrosesan perlu tersedia sesuai batas waktu yang dibutuhkan.  
> > > * *Finite resources* 🡪 kapasitas komputasi, memori, dan penyimpanan terbatas.


  **Can useful AI fit within a compute, memory, and power budget?**

  AI pada perangkat *edge* perlu dirancang agar dapat memberikan manfaat dengan tetap memenuhi keterbatasan komputasi, memori, dan daya, mencakup:
> > > * *Edge AI* 🡪 menjalankan inferensi AI pada perangkat *edge* untuk mendukung pemrosesan data sesuai kebutuhan aplikasi.  
> > > * *Local by design* 🡪 merancang sistem agar pemrosesan dapat dilakukan secara lokal pada perangkat.  
> > > * *Powered by arm* 🡪 memanfaatkan teknologi prosesor dan akselerator Arm untuk mendukung pemrosesan AI yang efisien sesuai kebutuhan perangkat.