### **Perbedaan Motor DC, Servo RC, dan Motor Stepper**



|Jenis Motor|Cara Kerja|Gerakan|Ciri Utama|
|-|-|-|-|
|Motor DC|Tegangan dan PWM|Berputar kontinu|Kecepatan tinggi, kontrol posisi kurang baik.|
|Servo RC|Sinyal PWM dengan variasi lebar pulsa|Umumnya terbatas sekitar 180° atau 210°|Memiliki feedback dan gearbox.|
|Motor Stepper|Pulsa digital melalui driver/translator|Berputar per langkah (step), dapat 360°|Torsi besar pada kecepatan rendah.|



**Motor DC**

Motor DC merupakan motor sederhana dengan dua terminal. Motor akan berputar ketika diberi tegangan. Arah putaran dapat diubah dengan membalik polaritas tegangan. Motor DC mampu menghasilkan RPM tinggi, tetapi memiliki kontrol posisi yang rendah, terutama pada kecepatan rendah. Kecepatannya dapat dikendalikan menggunakan PWM.



**Servo RC (Radio-Control Servo)**

Servo RC dirancang khusus untuk mengontrol posisi sudut poros. Posisi diatur menggunakan sinyal PWM, di mana perubahan lebar pulsa menentukan posisi poros. Di dalam servo terdapat motor DC, gearbox, sensor feedback (potensiometer), dan rangkaian kontrol.

Contohnya, pada servo dengan rentang 0–180°:

sekitar 1,5 ms → posisi tengah (90°)

pulsa lebih lebar → bergerak ke salah satu arah

pulsa lebih sempit → bergerak ke arah sebaliknya.

RC Servo membuat poros berada pada sudut tertentu.



**Motor Stepper**

Motor Stepper adalah motor brushless yang dikendalikan secara digital. Setiap pulsa clock menyebabkan motor berputar sejumlah sudut tertentu yang disebut step. Misalnya, motor dapat memiliki resolusi 15° atau 30° per step. Stepper dapat berputar hingga 360° secara kontinu dengan kontrol digital yang sesuai.

Stepper memiliki torsi besar pada kecepatan rendah, sehingga cocok untuk aplikasi yang membutuhkan posisi presisi, seperti printer, plotter, dan pengaturan posisi sensor.



Motor DC → mengatur kecepatan

RC Servo → mengatur posisi sudut

Motor Stepper → mengatur posisi berdasarkan langkah







