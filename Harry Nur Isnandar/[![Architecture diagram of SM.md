[![Architecture diagram of harrynurisnandar1-oss/stm32u585vit6_sm](https://gitdiagram.com/harrynurisnandar1-oss/stm32u585vit6_sm/diagram.png)](https://gitdiagram.com/harrynurisnandar1-oss/stm32u585vit6_sm?utm_source=readme&utm_medium=picture)


# 📐 Diagram Arsitektur Lengkap Sistem Smart Meter 3-Fasa AMI
**Target: STM32U585VIT6 (ARM Cortex-M33 TrustZone) + ADE9000 AFE**  
*Kepatuhan Standar: SPLN D3.006:2021, SPLN D5.002:2024, IEC 62056 (DLMS/COSEM)*

---

## 1. Diagram Blok Sistem End-to-End (Topologi Fisik & Logika)

```mermaid
flowchart TB
    subgraph OUTSIDE["🌐 DUNIA LUAR / EKSTERNAL"]
        HES["🏢 PLN Head-End System (HES) / MDMS"]
        HHU["📱 Handheld Unit (HHU) / Teknisi P2TL"]
        GRID["⚡ Jala-Jala Listrik 3-Fasa 230/400V AC"]
    end

    subgraph INTERFACE["🔌 ANTARMUKA FISIK & SENSOR (SPIN 1 / SPIN 2)"]
        MODEM["📡 Kompartemen Modem Modular\n(4G Cat-M1 / NB-IoT / RF Mesh)\nRJ-45 8-Pin RS-232 (19.200 bps)"]
        OPTICAL["🔦 Port Optik Inframerah Depan\nIEC 62056-21 (9.600 bps)\nIsolasi Galvanis 4 kV"]
        AFE["⚡ Analog Front-End ADE9000\n7-Kanal 24-bit ADC, SPI Terisolasi\nCT + Voltage Divider Sensing"]
        BREAKER["🔘 Contactor / Relay Breaker\nDisconnect Control (Class 70)"]
    end

    HES <-->|Seluler / Nirkabel| MODEM
    HHU <-->|Inframerah 900nm| OPTICAL
    GRID -->|Tegangan & Arus| AFE
    BREAKER -->|Putus / Sambung Aliran Listrik| GRID

    subgraph MCU["🔲 MIKROKONTROLER STM32U585VIT6 (ARM Cortex-M33 TrustZone)"]
        
        subgraph NONSECURE["🟢 NON-SECURE WORLD (Aplikasi Meter & Protokol)"]
            subgraph DRIVERS["Hardware / BSP Drivers"]
                UART_DRV["Driver UART & DMA Circular Buffer\n(ami_bsp_uart.c)\nLPUART1 / USART2 & USART3"]
                ADE_DRV["Driver Metrologi ADE9000 SPI\n(ami_ade9000.c)\nFail-Closed Snapshot Feeder"]
                RELAY_DRV["Driver Aktuator Disconnect Control\n(ami_disconnect_control.c)"]
            end

            subgraph RTOS["FreeRTOS Kernel Tasks"]
                DLMS_TASK["Task Protokol DLMS/COSEM\n(ami_dlms_port.c)\nStreamBuffer & Message Queue"]
                MET_TASK["Task Metrologi & Sampling\nPeriodik Baca RMS, Daya, Energi"]
            end

            subgraph DLMS_STACK["DLMS/COSEM Protocol Engine"]
                HDLC["HDLC Parser Type 3 (0xA0)\nCRC-16 HCS/FCS, Transparent 0x7E\nZero-Heap O(1)"]
                ACSE["ACSE Association Machine\nAARQ/AARE, Max PDU, Dynamic Conformance"]
                ABF["Anti-Brute-Force & Lockout\nPenalti Eksponensial (5s..40s)\nHard Lockout 5x Gagal (SPLN D5.002)"]
                GBT["General Block Transfer (GBT)\nxDLMS Datablock (0xC0/0xC4 0x02)\nSegmentasi Data Profil Besar"]
                COSEM["COSEM Object Model (49 Objek PLN)\n- Load Profile 15-menit (23 Kanal)\n- Tamper Logbook (9 Modus PLN)\n- Billing Profile (Class 7)\n- Multi-Tarif WBP/LWBP (Tarif 1..4)\n- Demand Limiter (Class 71)"]
            end

            UART_DRV -->|Byte Stream IRQ| RTOS
            ADE_DRV -->|Snapshot Data| RTOS
            RTOS --> HDLC --> ACSE --> DLMS_STACK
            DLMS_STACK --> RELAY_DRV
        end

        subgraph GATEWAY["🔐 GERBANG NON-SECURE CALLABLE (NSC)"]
            NSC_API["secure_nsc.c / secure_nsc.h\n- cmse_check_address_range()\n- Atomic Single-Fetch Copy Parameter\n- Strict Pointer Validation"]
        end

        subgraph SECURE["🔴 SECURE WORLD (TrustZone Protected Domain)"]
            SAU["Security Attribution Unit (SAU)\nIsolasi Memori SRAM3 & Flash"]
            CRYPTO["Core Kriptografi Suite 0\n- AES-128-GCM (glo-get/set/action)\n- HLS 5 GMAC (Mutual 4-Pass Auth)\n- Key Unwrap RFC 3394 (Master Key KEK)\n- Memory Zeroization (secure_zeroize)"]
            TRNG["Hardware TRNG Fail-Closed\nGenerator Acak 16-Byte Challenge"]
            NVM_SEC["Wear-Leveling Anti-Replay Engine\n- Strictly Monotonic Invocation Counter\n- TAMP Backup Register / NVM\n- Saturation Guard (0xFFFFFFFF Fail-Closed)"]
        end

        DLMS_STACK <-->|Permintaan Kripto / Auth / Key| NSC_API
        NSC_API <-->|Eksekusi Terisolasi| CRYPTO
        NSC_API <-->|Nomor Acak Challenge| TRNG
        NSC_API <-->|Pencatatan Counter Monotonik| NVM_SEC
    end

    MODEM <--> UART_DRV
    OPTICAL <--> UART_DRV
    AFE <--> ADE_DRV
    RELAY_DRV <--> BREAKER
```

---

## 2. Diagram Aliran Data Protokol & Otentikasi HLS5 (GMAC)

```mermaid
sequenceDiagram
    autonumber
    participant Client as 🏢 HES / DLMS Client (SAP 16)
    participant HDLC as 🟢 HDLC Layer (NonSecure)
    participant Port as 🟢 DLMS Port / ACSE
    participant NSC as 🔐 TrustZone NSC Gateway
    participant Secure as 🔴 Secure World (Crypto Core)

    Note over Client,HDLC: 1. Handshake HDLC Link Layer
    Client->>HDLC: SNRM Frame (Format 3, Client=16, Server=1)
    HDLC->>Client: UA Frame (Negosiasi Max Frame Size 512B)

    Note over Client,Port: 2. Handshake Asosiasi ACSE
    Client->>HDLC: I-Frame: AARQ (Mechanism OID: HLS 5 GMAC)
    HDLC->>Port: Ekstrak APDU AARQ
    Port->>NSC: SECURE_TRNG_GetBytes(16 bytes)
    NSC->>Secure: Baca TRNG Hardware STM32U5
    Secure-->>Port: 16-Byte Server Challenge (StoC)
    Port-->>HDLC: Bentuk APDU AARE (Status: Auth_Pending, StoC)
    HDLC-->>Client: I-Frame: AARE (Membawa Server Challenge)

    Note over Client,Secure: 3. Pembuktian Bersama HLS 5 (Mutual Proof Exchange)
    Client->>HDLC: I-Frame: Action Request (reply_to_HLS, Client Proof CtoS)
    HDLC->>Port: Action Method 1 (Association LN)
    Port->>NSC: SECURE_DLMS_HLS5_VerifyAndGenerateProof()
    Note over Secure: Validasi GMAC CtoS (Constant-time)<br/>Jika valid, bangkitkan GMAC StoC
    alt Jika Otentikasi Gagal
        Secure-->>Port: Error Auth Failed
        Note over Port: Pemicu Anti-Brute-Force:<br/>failed_attempts++ (Penalti 5s..40s atau Lockout)
        Port-->>Client: Penolakan AARE Rejection Permanent
    else Jika Otentikasi Sukses
        Secure-->>Port: Success + Server Proof (StoC)
        Port-->>HDLC: Action Response (Server Proof)
        HDLC-->>Client: I-Frame: HLS 5 Sesi Terbuka Penuh!
    end

    Note over Client,Secure: 4. Transaksi Terenkripsi (Suite 0 AES-128-GCM)
    Client->>HDLC: GLO-GET-Request (0xC8, SC=0x30, Nonce, Ciphertext, Tag)
    HDLC->>Port: Teruskan ke Cipher Engine
    Port->>NSC: SECURE_DLMS_AES_GCM_Process(Decrypt)
    Secure->>Secure: Cek Monotonik RX Counter (Anti-Replay)
    Secure->>Secure: Dekripsi AES-GCM & Verifikasi Tag 16-Byte
    Secure-->>Port: Plaintext APDU GET
    Port->>Port: Baca Objek COSEM (misal Load Profile 23-Kanal)
    Port->>NSC: SECURE_DLMS_AES_GCM_Process(Encrypt Respons)
    Secure->>Secure: Alokasi TX Monotonic Counter + Enkripsi
    Secure-->>Port: GLO-GET-Response (0xCC)
    Port-->>HDLC: Bungkus I-Frame HDLC
    HDLC-->>Client: Respons Terenkripsi Sampai di HES
```

---

## 3. Diagram Struktur Pengujian & CI/CD Pipeline

```mermaid
flowchart LR
    subgraph DEV["💻 WORKSPACE DEVELOPMENT"]
        CODE["Source Code\n(NonSecure, Secure, Tests)"]
    end

    subgraph LOCAL_TEST["🧪 VERIFIKASI HOST LOKAL (Ninja / CMake)"]
        direction TB
        C1["AmiCosemTest"]
        C2["AmiSecurityTest"]
        C3["AmiDlmsHdlcTest"]
        C4["AmiDlmsApduTest"]
        C5["AmiAde9000Test"]
        C6["AmiAntiBruteforceTest"]
        C7["AmiBlockTransferTest"]
        C8["AmiDisconnectControlTest"]
        C9["(Total 14/14 Suites PASS)"]
        SIM["Gurux Simulator TCP (Port 4059)\nUji GXDLMSDirector E2E"]
    end

    subgraph DOCKER_BOX["🐳 SINGLE UNIFIED TEST CONTAINER (Alpine Linux)"]
        DK_BUILD["Stage 1: Multi-stage Build\n(GCC 15, CMake, Ninja, -O3 Release)"]
        DK_RUN["Stage 2: Runtime Runner\n1. Jalankan 14 CTest Suites (100%)\n2. Jalankan Background Simulator\n3. Jalankan Python E2E Client Verification"]
        DK_BUILD --> DK_RUN
    end

    subgraph CI_PIPELINE["🚀 GITHUB ACTIONS PRODUCTION CI"]
        direction TB
        W1["Host Tests CI (Ubuntu Linux & Windows)"]
        W2["STM32U585 FreeRTOS Firmware CI\n(ARM GNU GCC 13.3.Rel1 Toolchain)"]
        W3["CodeQL Static Analysis (C/C++ Security SAST)"]
        ARTIFACTS["📦 Firmware Artifacts\n(.elf, .bin, .hex, .map)"]
        W2 --> ARTIFACTS
    end

    CODE --> LOCAL_TEST
    CODE --> DOCKER_BOX
    CODE --> CI_PIPELINE
```
