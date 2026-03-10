# Sandesh Audio Vault 🛡️

### Project Overview
**Sandesh Audio Vault** is designed to transmit highly sensitive audio messages for high-level security applications. By combining **steganography** (hiding data) and **cryptography** (encrypting data), it provides a dual layer of protection. The system utilizes military-grade **AES-256** encryption and is entirely decentralized—meaning there is no database. Communication remains strictly between the sender and the receiver.

### Key Features
* **Military-Grade Security:** Uses AES-256 for encryption and SHA-256 for digital fingerprinting (integrity check).
* **Brute-Force Protection:** Users enter a human-rememberable passcode, which a mathematical formula transforms into a 256-bit cryptographic key. This makes brute-force attacks mathematically infeasible ($2^{256}$ combinations).
* **Auto-Destruct Mechanism:** If an incorrect passcode is entered, the message is protected from unauthorized decoding.
* **Privacy-Centric:** Zero database architecture ensures total anonymity and security.

### Technical Requirements
To ensure the tool functions correctly, both the **cover** and **secret** audio files must be in the following format:
* **Format:** WAV (16-bit PCM)
* **Sample Rate:** 44,100 Hz

> [!TIP]
> Use this [Online Audio Converter](https://audio.online-convert.com/convert-to-wav) to prepare your files.
