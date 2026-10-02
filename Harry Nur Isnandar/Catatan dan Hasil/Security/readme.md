# STS Secure Crypto Reference

This project upgrades the original STS prototype from custom XOR/fake-ECC primitives to a host-side cryptographic reference implementation using OpenSSL 3.x.

## Cryptography

- CSPRNG: OpenSSL `RAND_bytes`
- Key agreement: ECDH over NIST P-256 (`prime256v1`)
- KDF: HKDF-SHA-256
- Authenticated encryption: AES-256-GCM
- Private-key-at-rest protection in the host simulator: AES-256-GCM using a 32-byte secret supplied through `STS_STORAGE_KEY_HEX`
- Private key is never returned by the Secure World API

## Important security boundary

The host implementation is a reference/simulator. `STS_STORAGE_KEY_HEX` represents a secret that, on a real meter, must be provided by a hardware-backed secure provisioning mechanism, secure element, HSM, TPM, or MCU secure storage. Do not put this value in source control.

The code does not claim compliance with a specific PLN/SPLN/STS security profile. Algorithm choice, key sizes, key hierarchy, provisioning, authentication, rotation, replay protection, and protocol fields must be mapped to the applicable product specification before production use.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

## Run

Set a 256-bit host storage secret:

```bash
export STS_STORAGE_KEY_HEX=00112233445566778899AABBCCDDEEFF00112233445566778899AABBCCDDEEFF
```

Provision a meter keypair and publish only the public key:

```bash
./build/sts_meter provision meter.key meter_public.db 999999 1 2
```

Create an encrypted VK at the KMC:

```bash
./build/sts_kmc 999999 1 2 meter_public.db kmc_database.db
```

Decrypt it through the Secure World API:

```bash
./build/sts_meter decrypt meter.key kmc_database.db 999999 1 2
```

The database stores ciphertext, nonce, authentication tag, and the KMC public ECDH key. The plaintext VK is never written to the database.

## CTest security cases

The test suite covers:

1. P-256 key generation.
2. Secure private-key persistence.
3. Public-key export/import.
4. ECDH + HKDF key agreement.
5. AES-256-GCM encryption/decryption.
6. Database round-trip.
7. Authentication failure when the GCM tag is modified.

## GitHub

`.github/workflows/ci.yml` builds and runs CTest on Linux, macOS, and Windows.