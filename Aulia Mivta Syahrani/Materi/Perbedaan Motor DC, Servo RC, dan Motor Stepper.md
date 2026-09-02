### **Perbedaan Motor DC, Servo RC, dan Motor Stepper**



|Jenis Motor|Cara Kerja|Gerakan|Ciri Utama|
|-|-|-|-|
|Motor DC|Tegangan dan PWM|Berputar kontinu|Kecepatan tinggi, kontrol posisi kurang baik.|
|Servo RC|Sinyal PWM dengan variasi lebar pulsa|Umumnya terbatas sekitar 180° atau 210°|Memiliki feedback dan gearbox.|
|Motor Stepper|Pulsa digital melalui driver/translator|Berputar per langkah (step), dapat 360°|Torsi besar pada kecepatan rendah.|



**Motor DC**
<img width="554" height="554" alt="motor dc" src="https://github.com/user-attachments/assets/5ae23ce1-6aea-4ae0-9a4a-c87442e6cd48" />
(Sumber: https://automationindo.com/thermal-imager/berdayakan-bisnis-anda-dengan-motor-dc-dan-inverter-motor-12v-yang-penuh-power/)

Motor DC merupakan motor sederhana dengan dua terminal. Motor akan berputar ketika diberi tegangan. Arah putaran dapat diubah dengan membalik polaritas tegangan. Motor DC mampu menghasilkan RPM tinggi, tetapi memiliki kontrol posisi yang rendah, terutama pada kecepatan rendah. Kecepatannya dapat dikendalikan menggunakan PWM.



**Servo RC (Radio-Control Servo)**
<img width="751" height="460" alt="RC-servo-motor-48" src="https://github.com/user-attachments/assets/6c8d8c20-d5c6-4a97-9dec-d4a033a59998" />
(Sumber: https://www.researchgate.net/figure/RC-servo-motor-48_fig12_274373572)

Servo RC dirancang khusus untuk mengontrol posisi sudut poros. Posisi diatur menggunakan sinyal PWM, di mana perubahan lebar pulsa menentukan posisi poros. Di dalam servo terdapat motor DC, gearbox, sensor feedback (potensiometer), dan rangkaian kontrol.
<img width="269" height="119" alt="timing diagram rc servo" src="https://github.com/user-attachments/assets/af674226-d3f8-4157-9f24-23b0812375f8" />

Contohnya, pada servo dengan rentang 0–180°:

sekitar 1,5 ms → posisi poros di tengah (90°)

pulsa lebih lebar → bergerak ke salah satu arah

pulsa lebih sempit → bergerak ke arah sebaliknya.

RC Servo membuat poros berada pada sudut tertentu.



**Motor Stepper**
<img width="730" height="730" alt="motor stepper" src="https://github.com/user-attachments/assets/dc227f7e-5416-4c60-a73b-c66866ee1b51" />
(Sumber: https://id.gnscomponent.com/development-board/28byj-48-dc-gear-step-stepper-motor-uln2003.html)
Motor Stepper adalah motor brushless yang dikendalikan secara digital. Setiap pulsa clock menyebabkan motor berputar sejumlah sudut tertentu yang disebut step. Misalnya, motor dapat memiliki resolusi 15° atau 30° per step. Stepper dapat berputar hingga 360° secara kontinu dengan kontrol digital yang sesuai.

Stepper memiliki torsi besar pada kecepatan rendah, sehingga cocok untuk aplikasi yang membutuhkan posisi presisi, seperti printer, plotter, dan pengaturan posisi sensor.



Motor DC → mengatur kecepatan

RC Servo → mengatur posisi sudut

Motor Stepper → mengatur posisi berdasarkan langkah







