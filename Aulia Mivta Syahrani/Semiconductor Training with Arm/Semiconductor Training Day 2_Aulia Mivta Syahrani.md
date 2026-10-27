**PELATIHAN SEMIKONDUKTOR BERSAMA ARM UNTUK TALENTA INDONESIA**  
**Hari ke-2**

1. **PENDAHULUAN**  
   Waktu dan Tempat: Grha ITS Surabaya, 21 Oktober 2026 pukul 10.00 – 17.30 WIB.  
   Topik: Pengembangan Teknologi Semikonduktor Berbasis Arm.  
     
2. **RANGKAIAN KEGIATAN DAN MATERI PELATIHAN**  
   2.1 ***Introduction to Arm NPU and AI Framework***

   Tim Manasveta

* Perkembangan AI meningkatkan kebutuhan komputasi dan kompleksitas perangkat lunak.  
* Tantangan utama dalam pengembangan AI: Rentang kebutuhan yang beragam, *security*, desain berdaya rendah, integrasi *machine learning*, pemrograman perangkat lunak, dan dukungan ekosistem yang luas.  
* Arm membuka potensi AI Edge

  ***Face recognition***: Pemrosesan pengenalan wajah dengan target 30 FPS.

  ***Image specification dan keyword spotting***: Pemrosesan citra dan pengenalan kata kunci.

  ***User interface***: Mendukung interaksi pengguna dengan sistem AI.

  ***Collaborative robots dan autonomous systems***: Penerapan AI pada robot kolaboratif dan sistem otonom.

  ***Visual inspection*** dan ***gas control***: Penerapan AI untuk inspeksi visual dan pengendalian gas.

  ***Real-time audio processing***: Pemrosesan audio secara waktu nyata.

* Perhitungan performa komputasi

  GDPS \= MACs x 2 x f

  Keterangan: 

  GDPS: *Giga Operations Per Second*

  MACs: jumlah operasi *multiply-accumulate*

  Faktor 2: satu operasi MAC dihitung sebagai dua operasi aritmetika, yaitu perkalian dan penjumlahan.

  f: frekuensi operasi

* ***Helium Vector Extension*** **(MVE**): Ekstensi instruksi pemrosesan vektor untuk meningkatkan kemampuan pemrosesan sinyal dan AI pada prosesor Arm Cortex-M.

  Diperkenalkan dalam arsitektur Armv8.1-M sebagai *M-profile Vector Extension*.

  Pemrosesan vektor memungkinkan satu instruksi mengolah beberapa elemen data sehingga mendukung efisiensi komputasi.

* *Machine learning* pada Cortex-A mendukung eksekusi AI pada sistem dengan *rich operating system*.  
* Ethos-U85 NPU dalam sistem

  **Arm Ethos-U85** merupakan NPU untuk mempercepat inferensi *machine learning* pada perangkat *Edge* AI. Berikut contoh penerapannya:

  1\. *Endpoint AI* 🡪 Sistem berbasis Cortex-M dengan interkoneksi ke System SRAM dan System Flash.

  2\. *Machine learning inline* 🡪 Sistem berbasis Cortex-A yang dapat menggunakan System SRAM, System Flash, dan DRAM.

  3\. Cortex-A JR Drive 🡪 Pendekatan ketiga yang disebutkan dalam materi; istilah lengkapnya perlu dicocokkan dengan diagram presentasi.

  **Alur data Ethos-U85**

  Model AI diproses melalui CPU dan NPU.

  Operasi yang didukung NPU dijalankan dengan akselerasi perangkat keras.

  Operasi yang tidak didukung NPU dapat dijalankan oleh CPU melalui perangkat lunak.

  *Runtime* mengatur pengiriman perintah pemrosesan ke NPU.

* Alur kerja pengembangan aplikasi AI  
1. Melatih atau memilih model (*train or select model*).  
2. Mengoptimalkan model untuk *Edge* AI.  
3. Mengompilasi model untuk IP target.  
4. Mengintegrasikan model ke dalam *runtime* dan aplikasi.  
5. Memvalidasi model pada platform virtual atau simulasi.  
6. Menerapkan model pada perangkat keras (*deploy on hardware*).

   *Framework*: Tensor Flow dan PyTorch.

   **Framework** merupakan struktur atau fondasi yang berisi kode, komponen, dan aturan siap pakai yang membantu developer membangun aplikasi.

* *Machine learning workload flow* pada Cortex-M dan Ethos-U NPU

  *Quantization:* mengurangi presisi numerik model, misalnya menggunakan INT8, agar lebih efisien untuk perangkat target.

  *Vela Compiler*: mengompilasi model untuk Ethos-U NPU.

  *Runtime*: mengelola eksekusi model serta komunikasi dengan NPU.

  CMSIS-NN: menyediakan kernel neural network yang dioptimalkan untuk prosesor Cortex-M, terutama untuk operasi yang dijalankan pada CPU.

* CMSIS-NN, TOSA, dan dukungan operator Ethos-U85

  **CMSIS-NN**: Kumpulan kernel *machine learning* yang dioptimalkan untuk Cortex-M guna meningkatkan performa dan mengurangi penggunaan memori.

  **CMSIS-Ethos-U**: Driver untuk menghubungkan *runtime* inferensi dengan NPU Ethos-U, termasuk pengelolaan inisialisasi, eksekusi, interupsi, dan pemantauan performa. 

  **TOSA**: Standar operasi tensor yang digunakan dalam alur optimasi dan kompilasi model AI.

* Arm Kleidi

  **Arm Kleidi** adalah sekumpulan pustaka perangkat lunak *(software library*) *open source* dari Arm yang dirancang untuk mempercepat kinerja AI dan *computer vision* pada prosesor berbasis arsitektur Arm.

  Domain aplikasi AI: *image segmentation, object detection, hand tracking,* dan *face tracking.*


  **Kesimpulan**

* Arm menyediakan solusi Edge AI yang menggabungkan prosesor Cortex-A atau Cortex-M, dan NPU Ethos.  
* *Helium Vector Extension* dan CMSIS-NN membantu mempercepat pemrosesan AI pada Cortex-M.  
* Pengembangan model meliputi optimasi, kuantisasi, kompilasi dengan Vela, integrasi *runtime* dan penerapan pada perangkat target.  
* Dukungan standar TOSA, framework AI, dan berbagai operator memungkinkan pengembangan aplikasi AI yang lebih terintegrasi.


  2.2 ***Company Showcase*** **– Nisclab**

  Joddy Tjackrawinata

> > > * Perusahaan *fabless deep tech* yang mengembangkan perangkat untuk sistem fotonik dan semikonduktor.  
> > > * Produk: sistem kontrol, driver atau DACs, serta otomasi untuk chip fotonik. Contoh produk: XDAC Series, Mini-ATE, dan XSOM.  
> > > * Bidang penerapan: T2 instrument, Dation, quantum NNI, komunikasi optik, dan *data center*.  
> > > * Nicslab menyediakan solusi kontrol multikanal yang disesuaikan untuk sistem dan lingkungan PIC (*Photonic Integrated Circuit*)  tertentu. 


  2.3 **Cortex-A CPU Deep-dive**

  **Part II-DSU and Cortex-A76**

  Yun Yi, *Application Engineer*, Arm.

> > > * **DSU (*DynamIQ Shared Unit*)**

  DSU merupakan komponen yang menghubungkan beberapa core CPU dalam satu cluster dan menyediakan sumber daya bersama, termasuk *cache* L3.

  SU mendukung konfigurasi *multicore* yang fleksibel untuk kebutuhan desain SoC.

  Pada konfigurasi tertentu, DSU dapat mendukung hingga 8 core CPU.

  DSU mengatur komunikasi antar-core, akses memori bersama, pengelolaan *cache*, serta manajemen daya.

> > > * ***DSU Cluster Components dan Memory Interface***

  *Memory interface* menghubungkan cluster CPU dengan sistem memori melalui antarmuka koheren.

  **CHI** (*Coherent Hub Interface*): protokol AMBA 5 untuk komunikasi koheren dengan kemampuan skalabilitas dan performa tinggi.

  **ACP** (*Accelerator Coherency Port*): memungkinkan akselerator mengakses sistem memori koheren melalui antarmuka yang disediakan pada konfigurasi DSU tertentu.

> > > * ***L3 Cache*** **dan *Cache Allocation***

  ***L3 cache***: *cache* bersama yang dapat mengurangi akses ke memori eksternal dan meningkatkan efisiensi akses data antar-*core*.

  Ukuran *cache* dapat dikonfigurasi hingga 4 MB.

  *Cache allocation policy*: mengatur penempatan data dalam *cache*.

  *Cache flushing*: menghapus atau membersihkan *entri cache* sesuai kebutuhan sistem.

  *Cache slicing*: *cache* dapat dibagi menjadi beberapa *slice* sesuai konfigurasi.

> > > * ***DSU Clock, Power Management,*** **dan *CPU Bridges***

  DSU mendukung pengelolaan clock dan daya untuk memenuhi kebutuhan performa serta efisiensi energi.

  *Power management* mengatur transisi komponen menuju kondisi daya rendah.

  WFE (*Wait For Event*): instruksi yang memungkinkan *core* menunggu suatu peristiwa dan mendukung mekanisme penghematan daya.

  CPU *bridges* mengatur buffering dan sinkronisasi komunikasi antara *core* CPU dan DSU.

> > > * **Debug, RAS, PMU, dan GIC Interfcae**

  DSU menyediakan dukungan debug untuk memeriksa kondisi prosesor.

  Debugger eksternal digunakan untuk memeriksa target yang sedang di-debug melalui antarmuka debug.

  **RAS** (*Reliability, Availability, and Serviceability*): mekanisme untuk mendeteksi dan menangani kesalahan perangkat keras.

  **PMU** (*Performance Monitoring Unit*) memantau aktivitas perangkat keras untuk menganalisis performa CPU serta memori.

  ***GIC interface*** menghubungkan mekanisme interupsi untuk mendukung pengiriman dan pengelolaan interupsi pada sistem.

> > > * **Cotrtex-A76**

  Cortex-A76 merupakan prosesor Arm Cortex-A berperforma tinggi yang dirancang untuk sistem berbasis SoC.

  Mendukung arsitektur Armv8.2-A, dengan AArch64 dan dukungan AArch32 terbatas pada tingkat exception EL0.

  Unit utama dalam arsitektur prosesor meliputi:

1. *Instruction Fetch Unit* (IFU) 🡪 mengambil instruksi dari *instruction cache*.  
2. *Instruction Decode Unit* 🡪 menerjemahkan instruksi agar dapat diproses oleh unit eksekusi.  
3. *Register Rename Unit* 🡪 memetakan register arsitektural ke register fisik untuk mendukung eksekusi instruksi.  
4. *Instruction Issue Unit* 🡪 mengatur pengiriman instruksi ke unit eksekusi.  
5. *Integer Execution Unit* 🡪 menjalankan operasi aritmetika dan logika.  
6. L1 dan L2 *cache* 🡪 mendukung penyimpanan data dan instruksi agar dapat diakses lebih cepat  
> > > * ***Memory Management Unit*** **(MMU)**

  MMU mengelola penerjemahan alamat dan akses memori. Fungsi utama:

1. Menerjemahkan *virtual address* (VA) menjadi *physical address* (PA).  
2. Mendukung proses *hardware page-table* *walk* untuk mencari pemetaan alamat.  
3. Mengatur atribut dan hak akses memori melalui pemetaan alamat virtual ke fisik.

   **TLB** (*Translation Lookaside Buffer*) menyimpan hasil penerjemahan alamat yang digunakan untuk mengurangi kebutuhan pencarian pada tabel halaman. 

   

   **Kesimpulan**

> > > * Memahami DSU, *cache*, memory interface, dan protokol AMBA menjadi dasar dalam memahami integrasi IP CPU ke dalam desain SoC.  
> > > * Pemahaman Cortex-A76 membantu mengenali alur eksekusi instruksi, pengelolaan memori, serta hubungan antarkomponen prosesor.  
> > > * Fitur debug, RAS, PMU, dan manajemen daya relevan untuk proses integrasi, pengujian, dan analisis performa sistem berbasis prosesor Arm.


  2.4 ***Company Showcase*** **– Peruri**

  **Chip e-paspor PERURI**

  Pak Adi

> > > * Chip e-passpor

  PERURI (Perusahaan Umum Percetakan Uang Republik Indonesia) berperan dalam pengembangan teknologi keamanan dan mendukung penguatan kemandirian teknologi nasional.

  *Electronic passport* (e-passpor) dilengkapi chip elektronik yang menyimpan data identitas dan biometrik pemegang paspor.

  Standar: ICAO Doc 9303, mengatur spesifikasi dokumen perjalanan terbaca mesin, penyimpanan data biometrik, dan mekanisme keamanan.

  Chip e-passpor mendukung autentikasi keaslian dokumen serta perlindungan data biometrik.

> > > * Komponen utama chip

  AFE, RF, CPU, ROM, RAM, NVM (*Non-Volatile Memory*), *Cryptographic Engine.*

  TRNG (*True Random Number Generator*) yang menghasilkan bilangan acak untuk kebutuhan keamanan.

> > > * PERURI mengembangkan Sandbox Desain Chip Merah Putih bersama INA Digital. Sandbox adalah lingkungan simulasi atau pengujian.  
> > > * Alur desain dan verifikasi chip

  RTL (*Register-Transfer Level*) mendeskripsikan fungsi dan aliran data rangkaian digital menggunakan bahasa seperti Verilog.

  Simulasi dan verifikasi

  Validasi desain.


  2.5 ***Fundamental of Silicon on Chip (SoC)***

  Tim Menasveta, *FAE Director*

> > > * ***System-on-Chip*** (SoC) adalah sistem yang mengintegrasikan berbagai komponen, seperti prosesor, memori, antarmuka komunikasi, dan komponen pendukung lainnya, ke dalam satu chip. Tahapan:  
1. *Architecture Definition and Specification* 🡪 menentukan arsitektur, kebutuhan, fungsi, dan spesifikasi sistem.  
2. *IP Selection* dan Integrasi🡪  memilih dan mengintegrasikan *Intellectual Property* (IP) yang sesuai dengan kebutuhan SoC.  
3. *RTL Design* 🡪 merancang perilaku logika digital menggunakan bahasa deskripsi perangkat keras, seperti Verilog.  
4. *Verification* 🡪 menguji desain untuk memastikan fungsinya sesuai spesifikasi.  
5. *Logic Synthesis* 🡪 mengubah desain RTL menjadi *netlist* berupa gerbang logika.  
6. *Physical Implementation* 🡪 melakukan perencanaan tata letak (*floorplanning*), penempatan komponen (*placement*), dan perutean koneksi (*routing*).  
7. *Physical Verification* 🡪 memeriksa apakah tata letak memenuhi aturan desain dan persyaratan implementasi.  
8. *Tapeout* 🡪 menyiapkan dan menyerahkan data desain final untuk proses pembuatan chip.  
> > > * Pengembangan SoC melibatkan 2 alur, sebagai berikut:  
1. *Hardware development flow* 🡪 dimulai dari kebutuhan dan arsitektur sistem, dilanjutkan dengan pemilihan IP, integrasi SoC, desain RTL, verifikasi, sintesis logika, dan implementasi fisik.  
2. *Software development flow* 🡪 mengembangkan perangkat lunak yang akan berjalan pada prosesor dan perangkat keras SoC.  
> > > * Komponen utama Arm-Based SoC  
1. Arm CPU

   **Cortex-A**: untuk aplikasi yang membutuhkan kemampuan pemrosesan lebih tinggi, seperti perangkat yang menjalankan sistem operasi.

   **Cortex-M**: untuk mikrokontroler, sistem *embedded*, dan aplikasi dengan efisiensi daya.

   **Cortex-R**: untuk sistem yang membutuhkan respons waktu nyata (*real-time*) dan karakteristik keandalan tertentu.

2. *AMBA Interconnect Fabric*

   Menghubungkan CPU, memori, dan komponen IP agar dapat saling bertukar data. **AMBA** merupakan standar antarmuka komunikasi yang dikembangkan Arm.

3. *System IP* dan *Corstone Subsystem*

   Menyediakan komponen pendukung sistem dan fondasi desain yang membantu proses integrasi SoC.

4. *Peripherals* dan *I/O Interfaces*

   Mendukung komunikasi SoC dengan perangkat atau komponen eksternal.

5. AI/ML dan *Hardware Accelerators*

   Mempercepat pemrosesan tertentu, termasuk beban kerja AI.

6. *Hardware Security*

   Mendukung perlindungan sistem dan isolasi sumber daya, misalnya melalui teknologi Arm TrustZone.

7. *Power* and *Clock Management*

   Mengatur kebutuhan daya dan sinyal clock sistem.

8. Debug and *Trace Infrastructure*

   Membantu proses pemeriksaan, pelacakan, dan pencarian kesalahan saat pengembangan.

> > > * **Arm Corstone Subsystem** merupakan paket referensi yang menyediakan subsystem, System IP, serta dukungan perangkat lunak untuk membantu perancangan SoC berbasis Arm.

  Corstone-300 🡪 berbasis prosesor Cortex-M55 dan mendukung integrasi NPU Ethos-U55 untuk aplikasi edge AI dengan kebutuhan efisiensi daya.

  Corstone-1000 🡪 menyediakan subsistem terverifikasi yang dapat dikonfigurasi untuk mempercepat pengembangan SoC yang mendukung OS seperti Linux.

> > > * *SoC Reference Chip Design*

  Desain referensi ini dapat digunakan sebagai titik awal untuk mengembangkan SoC. 

  Contoh penggunaan model AI adalah mendeteksi objek menggunakan YOLO, termasuk deteksi gender dan apel.


  2.6 ***Introduction to Arm Corston Compute Subsystem***

  Tim Menasveta, *FAE Director*

> > > * Arm Corstone merupakan paket referensi yang menggabungkan subsistem, IP pendukung, perangkat lunak, dan perangkat pengembangan untuk membantu mempercepat pengembangan SoC serta mengurangi *time to market* (TTM).  
> > > * Corstone dapat digunakan sebagai reference design yang menjadi titik awal pengembangan SoC, kemudian disesuaikan dengan kebutuhan produk.  
> > > * **Corstone-201**

  Menggunakan dua prosesor Cortex-M33 yang dapat dikonfigurasi secara terpisah untuk mendukung kebutuhan sistem yang berbeda.

  Mendukung *instruction cache* serta infrastruktur debug dan trace.

> > > * **Corstone-1000**

  Menyediakan subsistem yang dapat dikonfigurasi untuk membantu pengembangan SoC yang mendukung Linux.

  Menggabungkan prosesor Cortex-A dan Cortex-M.

> > > * Simulasi dan Prototyping

  *Fast Models* / FVP (*Fixed Virtual Platform*) memungkinkan pengujian perangkat lunak pada model virtual platform Arm sebelum perangkat keras fisik tersedia.

  *MPS3 FPGA board* dapat digunakan untuk prototyping dan pengujian desain pada perangkat keras berbasis FPGA.

> > > * System IP: CoreSight

  Arm CoreSight menyediakan infrastruktur debug dan trace untuk melacak eksekusi program, dan menemukan kesalahan selama pengembangan SoC.

  2.7 ***Panel Discussion: Career path sharing / Ask me anything***

  Andre Baker, Hayden Lim, Gracia Lin, Tim Menasveta, Matt Rushton, dan Yun Yi

  Berbagi pengalaman serta wawasan mengenai perjalanan karier di industri semikonduktor.

* Gracia Lin berasal dari Taiwan, memiliki pengalaman di industri semikonduktor, khususnya bidang ODM.  
* Diskusi menyinggung perkembangan era kecerdasan buatan (AI) dan peluang yang muncul bagi industri semikonduktor.  
* Terdapat peluang kerja sama antara Arm dan universitas di Indonesia melalui Danantara Indonesia untuk mendukung pengembangan talenta di bidang semikonduktor.
