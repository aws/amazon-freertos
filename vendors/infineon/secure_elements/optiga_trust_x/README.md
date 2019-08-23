# Infineon's OPTIGA&trade; Trust X hostcode

## Description

<img src="https://github.com/Infineon/Assets/blob/master/Pictures/OPTIGA-Trust-X.png">

This repository contains a target-agnostic C library for the [OPTIGA™ Trust X](https://www.infineon.com/optiga-trust-x) security chip.

## Summary
The [OPTIGA™ Trust X](https://www.infineon.com/dgdl/Infineon-DPS310-DS-v01_00-EN.pdf) is a security solution based on a secure micrcontroller. Each device is shipped with a unique elliptic-curve keypair and a corresponding X.509 certificate. OPTIGA™ Trust X enables easy integration into existing PKI infrastructure.

## Key Features and Benefits
* High-end security controller
* Turnkey solution
* One-way authentication using ECDSA
* Mutual authentication using DTLS client (IETF standard RFC 6347)
* Secure communication using DTLS
* Compliant with the USB Type-C™ Authentication standard
* I2C interface
* Up to 10 KB user memory
* Cryptographic support: ECC256, AES128, SHA-256, TRNG, DRNG
* PG-USON-10-2 package (3 x 3 mm)
* Standard & extended temperature ranges
* Full system integration support
* Common Criteria Certified EAL6+ (high) hardware
* Cryptographic Tool Box based on ECC NIST P256, P384 and SHA256 (sign, verify, key generation, ECDH, session key derivation)   

## Overview

![hostcode architecture](https://raw.githubusercontent.com/Infineon/Assets/master/Pictures/optiga_trust_x_stack_generic.jpg)

1. See [OPTIGA Crypt API](https://github.com/Infineon/optiga-trust-x/wiki/OPTIGA-Crypt-API) and [OPTIGA Util API](https://github.com/Infineon/optiga-trust-x/wiki/OPTIGA-Util-API) to know more about optiga_crypt and optiga_util modules
2. Information about the OPTIGA™ Trust X Command Library can be found in the [Solution Reference Manual v1.35](https://github.com/Infineon/Assets/raw/master/PDFs/OPTIGA_Trust_X_SolutionReferenceManual_v1.35.pdf)
In the same document you can find explanation of all Object IDs (OIDs) available for users as well as detailed technical explanation for all features and envisioned use cases.
3. Infineon I<sup>2</sup>C Protocol implementation details can be found [here](https://github.com/Infineon/Assets/raw/master/PDFs/IFXI2CProtocol_v1.65.pdf)
4. Platform Abstraction Layer (PAL) overview and Porting Guide are presented in the [Wiki](https://github.com/Infineon/optiga-trust-x/wiki/Porting-Guide)

For more information please refer to the [Wiki page](https://github.com/Infineon/optiga-trust-x/wiki) of this project

## Get started and Application Notes
Get started guide is based on the OPTIGA™ Trust X evaluation kit hardware and DAVE™ 4 (Digital Application Virtual Engineer) IDE. The detailed description you can find in the respective GitHub [repository](https://github.com/Infineon/getstarted-optiga-trust-x).

Infineon's GitHub offers various application notes for OPTIGA™ Trust X security solutions, you can find get started guide mentioned above there as well. The up-to-date list of application notes is [here](https://github.com/Infineon/appnotes-optiga-trust-x)

## Evaluation and developement kits
* [OPTIGA™ Trust X evaluation and demonstration kit](https://www.infineon.com/cms/en/product/evaluation-boards/optiga-trust-x-eval-kit/)
* OPTIGA™ Trust X Shield2Go (link pending)

## Porting Guide
You might find a list of supported Platforms [here](https://github.com/Infineon/optiga-trust-x/tree/master/pal). These are:
* Espressif ESP32 with Amazon freeRTOS
* Espressif ESP32 with freeRTOS
* Nordic NRF52
* Raspberry Pi (Embedded Linux)
* Infineon XMC microcontrollers

For other platform please refer to our porting guide on [Wiki](https://github.com/Infineon/optiga-trust-x/wiki/Porting-Guide) page

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
