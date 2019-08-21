keys used for SecureBoot, SecureSflash

SecureBoot       : 2nd Stage Bootloader is encrypted(AES128-CBC) and signed (HMAC-SHA256 OR RSA)
SecureSFlash     : Sflash (except 2nd stage Bootloader) is encrypted (AES128-CBC) and signed (HMAC-SHA256)
                   sector by sector.

AES128-CBC key   : keys/<any_sub_dir>/boot_aes.key
HMAC-SHA256 key  : keys/<any_sub_dir>/boot_sha.key
RSA Private key  : keys/<any_sub_dir>/rsa_key (Use rsa_key_gen.py described below)
RSA Public key   : keys/<any_sub_dir>/rsa_key

To Generate RSA keys use 
WICED/platform/MCU/BCM4390x/rsa_key_gen.py

rsa_key_gen.py needs : openssl, python, ssh-keygen
(In Windows, use CygWin to install these packages)

Usage:
rsa_key_gen.py rsa_key

- Copy rsa_key and rsa_key.n to platforms/common/BCM94390x/keys/<keys_dir>/
- Copy aes key to platforms/common/BCM94390x/keys/<keys_dir>/boot_aes.key

- Build with SECURE_BOOT=1 KEYS=RSA in the build string
  for example (test.console-BCM943909WCD1_3 download SECURE_BOOT=1 SECURE_BOOT_SIGN=RSA KEYS=<keys_dir>)

- Program OTP bits for secureboot options
- Program OTP bits for RSA public key hash using rsa_key.n.hash
