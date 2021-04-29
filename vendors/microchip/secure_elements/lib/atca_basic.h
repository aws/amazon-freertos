/**
 * \file
 * \brief CryptoAuthLib Basic API methods - a simple crypto authentication API.
 * These methods manage a global ATCADevice object behind the scenes. They also
 * manage the wake/idle state transitions so callers don't need to.
 *
 * \copyright (c) 2015-2020 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#ifndef ATCA_BASIC_H_
#define ATCA_BASIC_H_
/*lint +flb */

#include "cryptoauthlib.h"
#include "crypto/atca_crypto_sw_sha2.h"

/** \defgroup atcab_ Basic Crypto API methods (atcab_)
 *
 * \brief
 * These methods provide the most convenient, simple API to CryptoAuth chips
 *
   @{ */

#ifdef __cplusplus
extern "C" {
#endif

ATCA_DLL ATCADevice _gDevice;

// Basic global methods
ATCA_STATUS atcab_version(char *ver_str);
ATCA_STATUS atcab_init_ext(ATCADevice* device, ATCAIfaceCfg* cfg);
ATCA_STATUS atcab_init(ATCAIfaceCfg *cfg);
ATCA_STATUS atcab_init_device(ATCADevice ca_device);
ATCA_STATUS atcab_release_ext(ATCADevice* device);
ATCA_STATUS atcab_release(void);
ATCADevice atcab_get_device(void);
ATCADeviceType atcab_get_device_type_ext(ATCADevice device);
ATCADeviceType atcab_get_device_type(void);
uint8_t atcab_get_device_address(ATCADevice device);

bool atcab_is_ca_device(ATCADeviceType dev_type);
bool atcab_is_ta_device(ATCADeviceType dev_type);

#define atcab_get_addr(...)                     calib_get_addr(__VA_ARGS__)
#define atca_execute_command(...)               calib_execute_command(__VA_ARGS__)


// AES Mode functions
#include "crypto/atca_crypto_hw_aes.h"
ATCA_STATUS atcab_aes_cbc_init_ext(ATCADevice device, atca_aes_cbc_ctx_t* ctx, uint16_t key_id, uint8_t key_block, const uint8_t* iv);
ATCA_STATUS atcab_aes_cbc_init(atca_aes_cbc_ctx_t* ctx, uint16_t key_id, uint8_t key_block,   const uint8_t* iv);
ATCA_STATUS atcab_aes_cbc_encrypt_block(atca_aes_cbc_ctx_t* ctx, const uint8_t* plaintext, uint8_t* ciphertext);
ATCA_STATUS atcab_aes_cbc_decrypt_block(atca_aes_cbc_ctx_t* ctx, const uint8_t* ciphertext, uint8_t* plaintext);

ATCA_STATUS atcab_aes_cbcmac_init_ext(ATCADevice device, atca_aes_cbcmac_ctx_t* ctx, uint16_t key_id, uint8_t key_block);
ATCA_STATUS atcab_aes_cbcmac_init(atca_aes_cbcmac_ctx_t* ctx, uint16_t key_id, uint8_t key_block);
ATCA_STATUS atcab_aes_cbcmac_update(atca_aes_cbcmac_ctx_t* ctx, const uint8_t* data, uint32_t data_size);
ATCA_STATUS atcab_aes_cbcmac_finish(atca_aes_cbcmac_ctx_t* ctx, uint8_t* mac, uint32_t mac_size);

ATCA_STATUS atcab_aes_cmac_init_ext(ATCADevice device, atca_aes_cmac_ctx_t* ctx, uint16_t key_id, uint8_t key_block);
ATCA_STATUS atcab_aes_cmac_init(atca_aes_cmac_ctx_t* ctx, uint16_t key_id, uint8_t key_block);
ATCA_STATUS atcab_aes_cmac_update(atca_aes_cmac_ctx_t* ctx, const uint8_t* data, uint32_t data_size);
ATCA_STATUS atcab_aes_cmac_finish(atca_aes_cmac_ctx_t* ctx, uint8_t* cmac, uint32_t cmac_size);

ATCA_STATUS atcab_aes_ctr_init_ext(ATCADevice device, atca_aes_ctr_ctx_t* ctx, uint16_t key_id, uint8_t key_block, uint8_t counter_size, const uint8_t* iv);
ATCA_STATUS atcab_aes_ctr_init(atca_aes_ctr_ctx_t* ctx, uint16_t key_id, uint8_t key_block, uint8_t counter_size, const uint8_t* iv);
ATCA_STATUS atcab_aes_ctr_init_rand_ext(ATCADevice device, atca_aes_ctr_ctx_t* ctx, uint16_t key_id, uint8_t key_block, uint8_t counter_size, uint8_t* iv);
ATCA_STATUS atcab_aes_ctr_init_rand(atca_aes_ctr_ctx_t* ctx, uint16_t key_id, uint8_t key_block, uint8_t counter_size, uint8_t* iv);
ATCA_STATUS atcab_aes_ctr_block(atca_aes_ctr_ctx_t* ctx, const uint8_t* input, uint8_t* output);
ATCA_STATUS atcab_aes_ctr_encrypt_block(atca_aes_ctr_ctx_t* ctx, const uint8_t* plaintext, uint8_t* ciphertext);
ATCA_STATUS atcab_aes_ctr_decrypt_block(atca_aes_ctr_ctx_t* ctx, const uint8_t* ciphertext, uint8_t* plaintext);
ATCA_STATUS atcab_aes_ctr_increment(atca_aes_ctr_ctx_t* ctx);

ATCA_STATUS atcab_aes_ccm_init_ext(ATCADevice device, atca_aes_ccm_ctx_t* ctx, uint16_t key_id, uint8_t key_block, uint8_t* iv, size_t iv_size, size_t aad_size, size_t text_size, size_t tag_size);
ATCA_STATUS atcab_aes_ccm_init(atca_aes_ccm_ctx_t* ctx, uint16_t key_id, uint8_t key_block, uint8_t* iv, size_t iv_size, size_t aad_size, size_t text_size, size_t tag_size);
ATCA_STATUS atcab_aes_ccm_init_rand_ext(ATCADevice device, atca_aes_ccm_ctx_t* ctx, uint16_t key_id, uint8_t key_block, uint8_t* iv, size_t iv_size, size_t aad_size, size_t text_size, size_t tag_size);
ATCA_STATUS atcab_aes_ccm_init_rand(atca_aes_ccm_ctx_t* ctx, uint16_t key_id, uint8_t key_block, uint8_t* iv, size_t iv_size, size_t aad_size, size_t text_size, size_t tag_size);
ATCA_STATUS atcab_aes_ccm_aad_update(atca_aes_ccm_ctx_t* ctx, const uint8_t* aad, size_t aad_size);
ATCA_STATUS atcab_aes_ccm_aad_finish(atca_aes_ccm_ctx_t* ctx);
ATCA_STATUS atcab_aes_ccm_encrypt_update(atca_aes_ccm_ctx_t* ctx, const uint8_t* plaintext, uint32_t plaintext_size, uint8_t* ciphertext);
ATCA_STATUS atcab_aes_ccm_decrypt_update(atca_aes_ccm_ctx_t* ctx, const uint8_t* ciphertext, uint32_t ciphertext_size, uint8_t* plaintext);
ATCA_STATUS atcab_aes_ccm_encrypt_finish(atca_aes_ccm_ctx_t* ctx, uint8_t* tag, uint8_t* tag_size);
ATCA_STATUS atcab_aes_ccm_decrypt_finish(atca_aes_ccm_ctx_t* ctx, const uint8_t* tag, bool* is_verified);



#if ATCA_CA_SUPPORT && !ATCA_TA_SUPPORT && !defined(ATCA_USE_ATCAB_FUNCTIONS) && !defined(ATCA_ECC204_SUPPORT)

#define atcab_wakeup()                          calib_wakeup(_gDevice)
#define atcab_idle()                            calib_idle(_gDevice)
#define atcab_sleep()                           calib_sleep(_gDevice)
#define _atcab_exit(...)                         _calib_exit(_gDevice, __VA_ARGS__)
#define atcab_get_zone_size(...)                calib_get_zone_size(_gDevice, __VA_ARGS__)


// AES command functions
#define atcab_aes(...)                          calib_aes(_gDevice, __VA_ARGS__)
#define atcab_aes_encrypt(...)                  calib_aes_encrypt(_gDevice, __VA_ARGS__)
#define atcab_aes_encrypt_ext                   calib_aes_encrypt
#define atcab_aes_decrypt(...)                  calib_aes_decrypt(_gDevice, __VA_ARGS__)
#define atcab_aes_decrypt_ext                   calib_aes_decrypt
#define atcab_aes_gfm(...)                      calib_aes_gfm(_gDevice, __VA_ARGS__)

#define atcab_aes_gcm_init(...)                 calib_aes_gcm_init(_gDevice, __VA_ARGS__)
#define atcab_aes_gcm_init_rand(...)            calib_aes_gcm_init_rand(_gDevice, __VA_ARGS__)
#define atcab_aes_gcm_aad_update(...)           calib_aes_gcm_aad_update(_gDevice, __VA_ARGS__)
#define atcab_aes_gcm_encrypt_update(...)       calib_aes_gcm_encrypt_update(_gDevice, __VA_ARGS__)
#define atcab_aes_gcm_encrypt_finish(...)       calib_aes_gcm_encrypt_finish(_gDevice, __VA_ARGS__)
#define atcab_aes_gcm_decrypt_update(...)       calib_aes_gcm_decrypt_update(_gDevice, __VA_ARGS__)
#define atcab_aes_gcm_decrypt_finish(...)       calib_aes_gcm_decrypt_finish(_gDevice, __VA_ARGS__)

// CheckMAC command functions
#define atcab_checkmac(...)                     calib_checkmac(_gDevice, __VA_ARGS__)

// Counter command functions
#define atcab_counter(...)                      calib_counter(_gDevice, __VA_ARGS__)
#define atcab_counter_increment(...)            calib_counter_increment(_gDevice, __VA_ARGS__)
#define atcab_counter_read(...)                 calib_counter_read(_gDevice, __VA_ARGS__)

// DeriveKey command functions
#define atcab_derivekey(...)                    calib_derivekey(_gDevice, __VA_ARGS__)

// ECDH command functions
#define atcab_ecdh_base(...)                    calib_ecdh_base(_gDevice, __VA_ARGS__)
#define atcab_ecdh(...)                         calib_ecdh(_gDevice, __VA_ARGS__)
#define atcab_ecdh_enc(...)                     calib_ecdh_enc(_gDevice, __VA_ARGS__)
#define atcab_ecdh_ioenc(...)                   calib_ecdh_ioenc(_gDevice, __VA_ARGS__)
#define atcab_ecdh_tempkey(...)                 calib_ecdh_tempkey(_gDevice, __VA_ARGS__)
#define atcab_ecdh_tempkey_ioenc(...)           calib_ecdh_tempkey_ioenc(_gDevice, __VA_ARGS__)

// GenDig command functions
#define atcab_gendig(...)                       calib_gendig(_gDevice, __VA_ARGS__)

// GenKey command functions
#define atcab_genkey_base(...)                  calib_genkey_base(_gDevice, __VA_ARGS__)
#define atcab_genkey(...)                       calib_genkey(_gDevice, __VA_ARGS__)
#define atcab_get_pubkey(...)                   calib_get_pubkey(_gDevice, __VA_ARGS__)
#define atcab_get_pubkey_ext                    calib_get_pubkey

// HMAC command functions
#define atcab_hmac(...)                         calib_hmac(_gDevice, __VA_ARGS__)

// Info command functions
#define atcab_info_base(...)                    calib_info_base(_gDevice, __VA_ARGS__)
#define atcab_info(...)                         calib_info(_gDevice, __VA_ARGS__)
#define atcab_info_get_latch(...)               calib_info_get_latch(_gDevice, __VA_ARGS__)
#define atcab_info_set_latch(...)               calib_info_set_latch(_gDevice, __VA_ARGS__)

// KDF command functions
#define atcab_kdf(...)                          calib_kdf(_gDevice, __VA_ARGS__)

// Lock command functions
#define atcab_lock(...)                          calib_lock(_gDevice, __VA_ARGS__)
#define atcab_lock_config_zone()                 calib_lock_config_zone(_gDevice)
#define atcab_lock_config_zone_crc(...)          calib_lock_config_zone_crc(_gDevice, __VA_ARGS__)
#define atcab_lock_data_zone()                   calib_lock_data_zone(_gDevice)
#define atcab_lock_data_zone_crc(...)            calib_lock_data_zone_crc(_gDevice, __VA_ARGS__)
#define atcab_lock_data_slot(...)                calib_lock_data_slot(_gDevice, __VA_ARGS__)

// MAC command functions
#define atcab_mac(...)                          calib_mac(_gDevice, __VA_ARGS__)

// Nonce command functions
#define atcab_nonce_base(...)                   calib_nonce_base(_gDevice, __VA_ARGS__)
#define atcab_nonce(...)                        calib_nonce(_gDevice, __VA_ARGS__)
#define atcab_nonce_load(...)                   calib_nonce_load(_gDevice, __VA_ARGS__)
#define atcab_nonce_rand(...)                   calib_nonce_rand(_gDevice, __VA_ARGS__)
#define atcab_challenge(...)                    calib_challenge(_gDevice, __VA_ARGS__)
#define atcab_challenge_seed_update(...)        calib_challenge_seed_update(_gDevice, __VA_ARGS__)

// PrivWrite command functions
#define atcab_priv_write(...)                   calib_priv_write(_gDevice, __VA_ARGS__)


// Random command functions
#define atcab_random(...)                       calib_random(_gDevice, __VA_ARGS__)
#define atcab_random_ext                        calib_random

// Read command functions
#define atcab_read_zone(...)                    calib_read_zone(_gDevice, __VA_ARGS__)
#define atcab_is_locked(...)                    calib_is_locked(_gDevice, __VA_ARGS__)
#define atcab_is_config_locked(...)             calib_is_locked(_gDevice, LOCK_ZONE_CONFIG, __VA_ARGS__)
#define atcab_is_data_locked(...)               calib_is_locked(_gDevice, LOCK_ZONE_DATA, __VA_ARGS__)
#define atcab_is_slot_locked(...)               calib_is_slot_locked(_gDevice, __VA_ARGS__)
#define atcab_read_bytes_zone(...)              calib_read_bytes_zone(_gDevice, __VA_ARGS__)
#define atcab_read_serial_number(...)           calib_read_serial_number(_gDevice, __VA_ARGS__)
#define atcab_read_pubkey(...)                  calib_read_pubkey(_gDevice, __VA_ARGS__)
#define atcab_read_sig(...)                     calib_read_sig(_gDevice, __VA_ARGS__)
#define atcab_read_config_zone(...)             calib_read_config_zone(_gDevice, __VA_ARGS__)
#define atcab_cmp_config_zone(...)              calib_cmp_config_zone(_gDevice, __VA_ARGS__)
#define atcab_read_enc(...)                     calib_read_enc(_gDevice, __VA_ARGS__)


// SecureBoot command functions
#define atcab_secureboot(...)                   calib_secureboot(_gDevice, __VA_ARGS__)
#define atcab_secureboot_mac(...)               calib_secureboot_mac(_gDevice, __VA_ARGS__)

// SelfTest command functions
#define atcab_selftest(...)                     calib_selftest(_gDevice, __VA_ARGS__)

// SHA command functions
#define atcab_sha_base(...)                     calib_sha_base(_gDevice, __VA_ARGS__)
#define atcab_sha_start()                       calib_sha_start(_gDevice)
#define atcab_sha_update(...)                   calib_sha_update(_gDevice, __VA_ARGS__)
#define atcab_sha_end(...)                      calib_sha_end(_gDevice, __VA_ARGS__)
#define atcab_sha_read_context(...)             calib_sha_read_context(_gDevice, __VA_ARGS__)
#define atcab_sha_write_context(...)            calib_sha_write_context(_gDevice, __VA_ARGS__)
#define atcab_sha(...)                          calib_sha(_gDevice, __VA_ARGS__)
#define atcab_hw_sha2_256(...)                  calib_hw_sha2_256(_gDevice, __VA_ARGS__)
#define atcab_hw_sha2_256_init(...)             calib_hw_sha2_256_init(_gDevice, __VA_ARGS__)
#define atcab_hw_sha2_256_update(...)           calib_hw_sha2_256_update(_gDevice, __VA_ARGS__)
#define atcab_hw_sha2_256_finish(...)           calib_hw_sha2_256_finish(_gDevice, __VA_ARGS__)
#define atcab_sha_hmac_init(...)                calib_sha_hmac_init(_gDevice, __VA_ARGS__)
#define atcab_sha_hmac_update(...)              calib_sha_hmac_update(_gDevice, __VA_ARGS__)
#define atcab_sha_hmac_finish(...)              calib_sha_hmac_finish(_gDevice, __VA_ARGS__)
#define atcab_sha_hmac(...)                     calib_sha_hmac(_gDevice, __VA_ARGS__)
#define SHA_CONTEXT_MAX_SIZE                    (99)

// Sign command functions
#define atcab_sign_base(...)                    calib_sign_base(_gDevice, __VA_ARGS__)
#define atcab_sign(...)                         calib_sign(_gDevice, __VA_ARGS__)
#define atcab_sign_ext                          calib_sign
#define atcab_sign_internal(...)                calib_sign_internal(_gDevice, __VA_ARGS__)

// UpdateExtra command functions
#define atcab_updateextra(...)                  calib_updateextra(_gDevice, __VA_ARGS__)

// Verify command functions
#define atcab_verify(...)                       calib_verify(_gDevice, __VA_ARGS__)
#define atcab_verify_extern(...)                calib_verify_extern(_gDevice, __VA_ARGS__)
#define atcab_verify_extern_ext                 calib_verify_extern
#define atcab_verify_extern_mac(...)            calib_verify_extern_mac(_gDevice, __VA_ARGS__)
#define atcab_verify_stored(...)                calib_verify_stored(_gDevice, __VA_ARGS__)
#define atcab_verify_stored_ext                 calib_verify_stored
#define atcab_verify_stored_mac(...)            calib_verify_stored_mac(_gDevice, __VA_ARGS__)
#define atcab_verify_validate(...)              calib_verify_validate(_gDevice, __VA_ARGS__)
#define atcab_verify_invalidate(...)            calib_verify_invalidate(_gDevice, __VA_ARGS__)

// Write command functions
#define atcab_write(...)                        calib_write(_gDevice, __VA_ARGS__)
#define atcab_write_zone(...)                   calib_write_zone(_gDevice, __VA_ARGS__)
#define atcab_write_bytes_zone(...)             calib_write_bytes_zone(_gDevice, __VA_ARGS__)
#define atcab_write_pubkey(...)                 calib_write_pubkey(_gDevice, __VA_ARGS__)
#define atcab_write_config_zone(...)            calib_write_config_zone(_gDevice, __VA_ARGS__)
#define atcab_write_enc(...)                    calib_write_enc(_gDevice, __VA_ARGS__)
#define atcab_write_config_counter(...)         calib_write_config_counter(_gDevice, __VA_ARGS__)

#elif ATCA_TA_SUPPORT && !ATCA_CA_SUPPORT && !defined(ATCA_USE_ATCAB_FUNCTIONS)

#define atcab_wakeup(...)                       (0)
#define atcab_idle(...)                         (0)
#define atcab_sleep(...)                        (0)
#define _atcab_exit(...)                        (1)
#define atcab_get_zone_size(...)                talib_get_zone_size(_gDevice, __VA_ARGS__)
//#define atcab_get_addr(...)                     (1)

// AES command functions
#define atcab_aes(...)                          (1)
#define atcab_aes_encrypt(...)                  talib_aes_encrypt(_gDevice, __VA_ARGS__)
#define atcab_aes_encrypt_ext                   talib_aes_encrypt
#define atcab_aes_decrypt(...)                  talib_aes_decrypt(_gDevice, __VA_ARGS__)
#define atcab_aes_decrypt_ext                   talib_aes_decrypt
#define atcab_aes_gfm(...)                      (1)

#define atcab_aes_gcm_init(...)                 (1)
#define atcab_aes_gcm_init_rand(...)            (1)
#define atcab_aes_gcm_aad_update(...)           (1)
#define atcab_aes_gcm_encrypt_update(...)       (1)
#define atcab_aes_gcm_encrypt_finish(...)       (1)
#define atcab_aes_gcm_decrypt_update(...)       (1)
#define atcab_aes_gcm_decrypt_finish(...)       (1)

// CheckMAC command functions
#define atcab_checkmac(...)                     (1)

// Counter command functions
#define atcab_counter(...)                      talib_counter(_gDevice, __VA_ARGS__)
#define atcab_counter_increment(...)            talib_counter_increment(_gDevice, __VA_ARGS__)
#define atcab_counter_read(...)                 talib_counter_read(_gDevice, __VA_ARGS__)

// DeriveKey command functions
#define atcab_derivekey(...)                    (1)

// ECDH command functions
#define atcab_ecdh_base(...)                    (1)
#define atcab_ecdh(...)                         talib_ecdh_compat(_gDevice, __VA_ARGS__)
#define atcab_ecdh_enc(...)                     (ATCA_UNIMPLEMENTED)
#define atcab_ecdh_ioenc(...)                   (ATCA_UNIMPLEMENTED)
#define atcab_ecdh_tempkey(...)                 (1)
#define atcab_ecdh_tempkey_ioenc(...)           (ATCA_UNIMPLEMENTED)

// GenDig command functions
#define atcab_gendig(...)                       (1)

// GenKey command functions
#define atcab_genkey_base(...)                  (ATCA_UNIMPLEMENTED)
#define atcab_genkey(...)                       talib_genkey_compat(_gDevice, __VA_ARGS__)
#define atcab_get_pubkey(...)                   talib_get_pubkey_compat(_gDevice, __VA_ARGS__)
#define atcab_get_pubkey_ext                    talib_get_pubkey_compat

// HMAC command functions
#define atcab_hmac(...)                         (ATCA_UNIMPLEMENTED)

// Info command functions
#define atcab_info_base(...)                    talib_info_base(_gDevice, __VA_ARGS__)
#define atcab_info(...)                         talib_info_compat(_gDevice, __VA_ARGS__)
#define atcab_info_get_latch(...)               (1)
#define atcab_info_set_latch(...)               (1)
//#define atcab_info_get_latch(...)               talib_info_get_latch(_gDevice, __VA_ARGS__)
//#define atcab_info_set_latch(...)               talib_info_set_latch(_gDevice, __VA_ARGS__)

// KDF command functions
#define atcab_kdf(...)                          (1)

// Lock command functions
#define atcab_lock(...)                         (1)
#define atcab_lock_config_zone()                talib_lock_config(_gDevice)
#define atcab_lock_config_zone_crc(...)         talib_lock_config_with_crc(_gDevice, __VA_ARGS__)
#define atcab_lock_data_zone()                  talib_lock_setup(_gDevice)
#define atcab_lock_data_zone_crc(...)           (1)
#define atcab_lock_data_slot(...)               talib_lock_handle(_gDevice, __VA_ARGS__)

// MAC command functions
#define atcab_mac(...)                          (ATCA_UNIMPLEMENTED)

// Nonce command functions
#define atcab_nonce_base(...)                   (1)
#define atcab_nonce(...)                        (1)
#define atcab_nonce_load(...)                   (1)
#define atcab_nonce_rand(...)                   (1)
#define atcab_challenge(...)                    (1)
#define atcab_challenge_seed_update(...)        (1)

// PrivWrite command functionsoc
#define atcab_priv_write(...)                   (1)

// Random command functions
#define atcab_random(...)                       talib_random_compat(_gDevice, __VA_ARGS__)
#define atcab_random_ext                        talib_random_compat

// Read command functions
#define atcab_read_zone(...)                    (ATCA_UNIMPLEMENTED)
#define atcab_is_locked(...)                    talib_is_locked_compat(_gDevice, __VA_ARGS__)
#define atcab_is_config_locked(...)             talib_is_config_locked(_gDevice, __VA_ARGS__)
#define atcab_is_data_locked(...)               talib_is_setup_locked(_gDevice, __VA_ARGS__)
#define atcab_is_slot_locked(...)               talib_is_handle_locked(_gDevice, __VA_ARGS__)
#define atcab_read_bytes_zone(...)              talib_read_bytes_zone(_gDevice, __VA_ARGS__)
#define atcab_read_serial_number(...)           talib_info_serial_number_compat(_gDevice, __VA_ARGS__)
#define atcab_read_pubkey(...)                  talib_read_pubkey_compat(_gDevice, __VA_ARGS__)
#define atcab_read_sig(...)                     talib_read_sig_compat(_gDevice, __VA_ARGS__)
#define atcab_read_config_zone(...)             talib_read_config_zone(_gDevice, __VA_ARGS__)
#define atcab_cmp_config_zone(...)              talib_cmp_config_zone(_gDevice, __VA_ARGS__)
#define atcab_read_enc(...)                     (ATCA_UNIMPLEMENTED)


// SecureBoot command functions
#define atcab_secureboot(...)                   (ATCA_UNIMPLEMENTED)
#define atcab_secureboot_mac(...)               (ATCA_UNIMPLEMENTED)

// SelfTest command functions
#define atcab_selftest(...)                     (1)

// SHA command functions
#define atcab_sha_base(...)                     talib_sha_base_compat(_gDevice, __VA_ARGS__)
#define atcab_sha_start()                       talib_sha_start(_gDevice)
#define atcab_sha_update(...)                   talib_sha_update_compat(_gDevice, __VA_ARGS__)
#define atcab_sha_end(...)                      talib_sha_end_compat(_gDevice, __VA_ARGS__)
#define atcab_sha_read_context(...)             talib_sha_read_context(_gDevice, __VA_ARGS__)
#define atcab_sha_write_context(...)            talib_sha_write_context(_gDevice, __VA_ARGS__)
#define atcab_sha(...)                          talib_sha(_gDevice, __VA_ARGS__)
#define atcab_hw_sha2_256(...)                  (1)
#define atcab_hw_sha2_256_init(...)             (1)
#define atcab_hw_sha2_256_update(...)           (1)
#define atcab_hw_sha2_256_finish(...)           (1)
#define atcab_sha_hmac_init(...)                (ATCA_UNIMPLEMENTED)
#define atcab_sha_hmac_update(...)              (ATCA_UNIMPLEMENTED)
#define atcab_sha_hmac_finish(...)              (ATCA_UNIMPLEMENTED)
#define atcab_sha_hmac(...)                     talib_hmac_compat(_gDevice, __VA_ARGS__)
#define SHA_CONTEXT_MAX_SIZE                    (109)

// Sign command functions
#define atcab_sign_base(...)                    (1)
#define atcab_sign(...)                         talib_sign_compat(_gDevice, __VA_ARGS__)
#define atcab_sign_ext                          talib_sign_compat
#define atcab_sign_internal(...)                (1)

// UpdateExtra command functions
#define atcab_updateextra(...)                  (1)

// Verify command functions
#define atcab_verify(...)                       (1)
#define atcab_verify_extern(...)                talib_verify_extern_compat(_gDevice, __VA_ARGS__)
#define atcab_verify_extern_ext                 talib_verify_extern_compat
#define atcab_verify_extern_mac(...)            (ATCA_UNIMPLEMENTED)
#define atcab_verify_stored(...)                talib_verify_stored_compat(_gDevice, __VA_ARGS__)
#define atcab_verify_stored_ext                 talib_verify_stored_compat
#define atcab_verify_stored_mac(...)            (ATCA_UNIMPLEMENTED)
#define atcab_verify_validate(...)              (ATCA_UNIMPLEMENTED)
#define atcab_verify_invalidate(...)            (ATCA_UNIMPLEMENTED)

// Write command functions
#define atcab_write(...)                        (ATCA_UNIMPLEMENTED)
#define atcab_write_zone(...)                   talib_write_zone(_gDevice, __VA_ARGS__)
#define atcab_write_bytes_zone(...)             talib_write_bytes_zone(_gDevice, __VA_ARGS__)
#define atcab_write_pubkey(...)                 talib_write_pubkey_compat(_gDevice, __VA_ARGS__)
#define atcab_write_config_zone(...)            talib_write_config_zone(_gDevice, __VA_ARGS__)
#define atcab_write_enc(...)                    (ATCA_UNIMPLEMENTED)
#define atcab_write_config_counter(...)         (ATCA_UNIMPLEMENTED)

#endif

#if (ATCA_TA_SUPPORT && ATCA_CA_SUPPORT) || defined(ATCA_USE_ATCAB_FUNCTIONS) || defined(ATCA_ECC204_SUPPORT)

/* Basic global methods */
ATCA_STATUS _atcab_exit(void);
ATCA_STATUS atcab_wakeup(void);
ATCA_STATUS atcab_idle(void);
ATCA_STATUS atcab_sleep(void);
//ATCA_STATUS atcab_get_addr(uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, uint16_t* addr);
ATCA_STATUS atcab_get_zone_size(uint8_t zone, uint16_t slot, size_t* size);

// AES command functions
ATCA_STATUS atcab_aes(uint8_t mode, uint16_t key_id, const uint8_t* aes_in, uint8_t* aes_out);
ATCA_STATUS atcab_aes_encrypt(uint16_t key_id, uint8_t key_block, const uint8_t* plaintext, uint8_t* ciphertext);
ATCA_STATUS atcab_aes_encrypt_ext(ATCADevice device, uint16_t key_id, uint8_t key_block, const uint8_t* plaintext, uint8_t* ciphertext);
ATCA_STATUS atcab_aes_decrypt(uint16_t key_id, uint8_t key_block, const uint8_t* ciphertext, uint8_t* plaintext);
ATCA_STATUS atcab_aes_decrypt_ext(ATCADevice device, uint16_t key_id, uint8_t key_block, const uint8_t* ciphertext, uint8_t* plaintext);
ATCA_STATUS atcab_aes_gfm(const uint8_t* h, const uint8_t* input, uint8_t* output);

/* AES GCM */
ATCA_STATUS atcab_aes_gcm_init(atca_aes_gcm_ctx_t* ctx, uint16_t key_id, uint8_t key_block, const uint8_t* iv, size_t iv_size);
ATCA_STATUS atcab_aes_gcm_init_rand(atca_aes_gcm_ctx_t* ctx, uint16_t key_id, uint8_t key_block, size_t rand_size,
                                    const uint8_t* free_field, size_t free_field_size, uint8_t* iv);
ATCA_STATUS atcab_aes_gcm_aad_update(atca_aes_gcm_ctx_t* ctx, const uint8_t* aad, uint32_t aad_size);
ATCA_STATUS atcab_aes_gcm_encrypt_update(atca_aes_gcm_ctx_t* ctx, const uint8_t* plaintext, uint32_t plaintext_size, uint8_t* ciphertext);
ATCA_STATUS atcab_aes_gcm_encrypt_finish(atca_aes_gcm_ctx_t* ctx, uint8_t* tag, size_t tag_size);
ATCA_STATUS atcab_aes_gcm_decrypt_update(atca_aes_gcm_ctx_t* ctx, const uint8_t* ciphertext, uint32_t ciphertext_size, uint8_t* plaintext);
ATCA_STATUS atcab_aes_gcm_decrypt_finish(atca_aes_gcm_ctx_t* ctx, const uint8_t* tag, size_t tag_size, bool* is_verified);

/* CheckMAC command */
ATCA_STATUS atcab_checkmac(uint8_t mode, uint16_t key_id, const uint8_t* challenge, const uint8_t* response, const uint8_t* other_data);

/* Counter command */
ATCA_STATUS atcab_counter(uint8_t mode, uint16_t counter_id, uint32_t* counter_value);
ATCA_STATUS atcab_counter_increment(uint16_t counter_id, uint32_t* counter_value);
ATCA_STATUS atcab_counter_read(uint16_t counter_id, uint32_t* counter_value);

/* DeriveKey command */
ATCA_STATUS atcab_derivekey(uint8_t mode, uint16_t key_id, const uint8_t* mac);

/* ECDH command */
ATCA_STATUS atcab_ecdh_base(uint8_t mode, uint16_t key_id, const uint8_t* public_key, uint8_t* pms, uint8_t* out_nonce);
ATCA_STATUS atcab_ecdh(uint16_t key_id, const uint8_t* public_key, uint8_t* pms);

#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS atcab_ecdh_enc(uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* read_key, uint16_t read_key_id);
#else
ATCA_STATUS atcab_ecdh_enc(uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* read_key, uint16_t read_key_id, const uint8_t num_in[NONCE_NUMIN_SIZE]);
#endif

ATCA_STATUS atcab_ecdh_ioenc(uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* io_key);
ATCA_STATUS atcab_ecdh_tempkey(const uint8_t* public_key, uint8_t* pms);
ATCA_STATUS atcab_ecdh_tempkey_ioenc(const uint8_t* public_key, uint8_t* pms, const uint8_t* io_key);

// GenDig command functions
ATCA_STATUS atcab_gendig(uint8_t zone, uint16_t key_id, const uint8_t* other_data, uint8_t other_data_size);

// GenKey command functions
ATCA_STATUS atcab_genkey_base(uint8_t mode, uint16_t key_id, const uint8_t* other_data, uint8_t* public_key);
ATCA_STATUS atcab_genkey(uint16_t key_id, uint8_t* public_key);
ATCA_STATUS atcab_get_pubkey(uint16_t key_id, uint8_t* public_key);
ATCA_STATUS atcab_get_pubkey_ext(ATCADevice device, uint16_t key_id, uint8_t* public_key);

// HMAC command functions
ATCA_STATUS atcab_hmac(uint8_t mode, uint16_t key_id, uint8_t* digest);

// Info command functions
ATCA_STATUS atcab_info_base(uint8_t mode, uint16_t param2, uint8_t* out_data);
ATCA_STATUS atcab_info(uint8_t* revision);
ATCA_STATUS atcab_info_set_latch(bool state);
ATCA_STATUS atcab_info_get_latch(bool* state);

// KDF command functions
ATCA_STATUS atcab_kdf(uint8_t mode, uint16_t key_id, const uint32_t details, const uint8_t* message, uint8_t* out_data, uint8_t* out_nonce);

// Lock command functions
ATCA_STATUS atcab_lock(uint8_t mode, uint16_t summary_crc);
ATCA_STATUS atcab_lock_config_zone(void);
ATCA_STATUS atcab_lock_config_zone_crc(uint16_t summary_crc);
ATCA_STATUS atcab_lock_data_zone(void);
ATCA_STATUS atcab_lock_data_zone_crc(uint16_t summary_crc);
ATCA_STATUS atcab_lock_data_slot(uint16_t slot);

// MAC command functions
ATCA_STATUS atcab_mac(uint8_t mode, uint16_t key_id, const uint8_t* challenge, uint8_t* digest);

// Nonce command functions
ATCA_STATUS atcab_nonce_base(uint8_t mode, uint16_t zero, const uint8_t* num_in, uint8_t* rand_out);
ATCA_STATUS atcab_nonce(const uint8_t* num_in);
ATCA_STATUS atcab_nonce_load(uint8_t target, const uint8_t* num_in, uint16_t num_in_size);
ATCA_STATUS atcab_nonce_rand(const uint8_t* num_in, uint8_t* rand_out);
ATCA_STATUS atcab_challenge(const uint8_t* num_in);
ATCA_STATUS atcab_challenge_seed_update(const uint8_t* num_in, uint8_t* rand_out);

// PrivWrite command functions
#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS atcab_priv_write(uint16_t key_id, const uint8_t priv_key[36], uint16_t write_key_id, const uint8_t write_key[32]);
#else
ATCA_STATUS atcab_priv_write(uint16_t key_id, const uint8_t priv_key[36], uint16_t write_key_id, const uint8_t write_key[32], const uint8_t num_in[NONCE_NUMIN_SIZE]);
#endif

// Random command functions
ATCA_STATUS atcab_random(uint8_t* rand_out);
ATCA_STATUS atcab_random_ext(ATCADevice device, uint8_t* rand_out);

// Read command functions
ATCA_STATUS atcab_read_zone(uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, uint8_t* data, uint8_t len);
ATCA_STATUS atcab_is_locked(uint8_t zone, bool* is_locked);
ATCA_STATUS atcab_is_config_locked(bool* is_locked);
ATCA_STATUS atcab_is_data_locked(bool* is_locked);
ATCA_STATUS atcab_is_slot_locked(uint16_t slot, bool* is_locked);
ATCA_STATUS atcab_read_bytes_zone(uint8_t zone, uint16_t slot, size_t offset, uint8_t* data, size_t length);
ATCA_STATUS atcab_read_serial_number(uint8_t* serial_number);
ATCA_STATUS atcab_read_pubkey(uint16_t slot, uint8_t* public_key);
ATCA_STATUS atcab_read_sig(uint16_t slot, uint8_t* sig);
ATCA_STATUS atcab_read_config_zone(uint8_t* config_data);
ATCA_STATUS atcab_cmp_config_zone(uint8_t* config_data, bool* same_config);

#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS atcab_read_enc(uint16_t key_id, uint8_t block, uint8_t* data, const uint8_t* enc_key, const uint16_t enc_key_id);
#else
ATCA_STATUS atcab_read_enc(uint16_t key_id, uint8_t block, uint8_t* data, const uint8_t* enc_key, const uint16_t enc_key_id, const uint8_t num_in[NONCE_NUMIN_SIZE]);
#endif

// SecureBoot command functions
ATCA_STATUS atcab_secureboot(uint8_t mode, uint16_t param2, const uint8_t* digest, const uint8_t* signature, uint8_t* mac);
ATCA_STATUS atcab_secureboot_mac(uint8_t mode, const uint8_t* digest, const uint8_t* signature, const uint8_t* num_in, const uint8_t* io_key, bool* is_verified);

/* SelfTest Command */
ATCA_STATUS atcab_selftest(uint8_t mode, uint16_t param2, uint8_t* result);

/* SHA Command  */
#define SHA_CONTEXT_MAX_SIZE                    (109)
ATCA_STATUS atcab_sha_base(uint8_t mode, uint16_t length, const uint8_t* data_in, uint8_t* data_out, uint16_t* data_out_size);
ATCA_STATUS atcab_sha_start(void);
ATCA_STATUS atcab_sha_update(const uint8_t* message);
ATCA_STATUS atcab_sha_end(uint8_t* digest, uint16_t length, const uint8_t* message);
ATCA_STATUS atcab_sha_read_context(uint8_t* context, uint16_t* context_size);
ATCA_STATUS atcab_sha_write_context(const uint8_t* context, uint16_t context_size);
ATCA_STATUS atcab_sha(uint16_t length, const uint8_t* message, uint8_t* digest);
ATCA_STATUS atcab_hw_sha2_256(const uint8_t* data, size_t data_size, uint8_t* digest);

ATCA_STATUS atcab_hw_sha2_256_init(atca_sha256_ctx_t* ctx);
ATCA_STATUS atcab_hw_sha2_256_update(atca_sha256_ctx_t* ctx, const uint8_t* data, size_t data_size);
ATCA_STATUS atcab_hw_sha2_256_finish(atca_sha256_ctx_t* ctx, uint8_t* digest);
ATCA_STATUS atcab_sha_hmac_init(atca_hmac_sha256_ctx_t* ctx, uint16_t key_slot);
ATCA_STATUS atcab_sha_hmac_update(atca_hmac_sha256_ctx_t* ctx, const uint8_t* data, size_t data_size);
ATCA_STATUS atcab_sha_hmac_finish(atca_hmac_sha256_ctx_t* ctx, uint8_t* digest, uint8_t target);

ATCA_STATUS atcab_sha_hmac(const uint8_t* data, size_t data_size, uint16_t key_slot, uint8_t* digest, uint8_t target);

/* Sign command */
ATCA_STATUS atcab_sign_base(uint8_t mode, uint16_t key_id, uint8_t* signature);
ATCA_STATUS atcab_sign(uint16_t key_id, const uint8_t* msg, uint8_t* signature);
ATCA_STATUS atcab_sign_ext(ATCADevice device, uint16_t key_id, const uint8_t* msg, uint8_t* signature);
ATCA_STATUS atcab_sign_internal(uint16_t key_id, bool is_invalidate, bool is_full_sn, uint8_t* signature);

/* UpdateExtra command */
ATCA_STATUS atcab_updateextra(uint8_t mode, uint16_t new_value);

/* Verify command */
ATCA_STATUS atcab_verify(uint8_t mode, uint16_t key_id, const uint8_t* signature, const uint8_t* public_key, const uint8_t* other_data, uint8_t* mac);
ATCA_STATUS atcab_verify_extern(const uint8_t* message, const uint8_t* signature, const uint8_t* public_key, bool* is_verified);
ATCA_STATUS atcab_verify_extern_ext(ATCADevice device, const uint8_t* message, const uint8_t* signature, const uint8_t* public_key, bool* is_verified);
ATCA_STATUS atcab_verify_extern_mac(const uint8_t* message, const uint8_t* signature, const uint8_t* public_key, const uint8_t* num_in, const uint8_t* io_key, bool* is_verified);
ATCA_STATUS atcab_verify_stored(const uint8_t* message, const uint8_t* signature, uint16_t key_id, bool* is_verified);
ATCA_STATUS atcab_verify_stored_ext(ATCADevice device, const uint8_t* message, const uint8_t* signature, uint16_t key_id, bool* is_verified);
ATCA_STATUS atcab_verify_stored_mac(const uint8_t* message, const uint8_t* signature, uint16_t key_id, const uint8_t* num_in, const uint8_t* io_key, bool* is_verified);

ATCA_STATUS atcab_verify_validate(uint16_t key_id, const uint8_t* signature, const uint8_t* other_data, bool* is_verified);
ATCA_STATUS atcab_verify_invalidate(uint16_t key_id, const uint8_t* signature, const uint8_t* other_data, bool* is_verified);

/* Write command functions */
ATCA_STATUS atcab_write(uint8_t zone, uint16_t address, const uint8_t* value, const uint8_t* mac);
ATCA_STATUS atcab_write_zone(uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, const uint8_t* data, uint8_t len);
ATCA_STATUS atcab_write_bytes_zone(uint8_t zone, uint16_t slot, size_t offset_bytes, const uint8_t* data, size_t length);
ATCA_STATUS atcab_write_pubkey(uint16_t slot, const uint8_t* public_key);
ATCA_STATUS atcab_write_config_zone(const uint8_t* config_data);

#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS atcab_write_enc(uint16_t key_id, uint8_t block, const uint8_t* data, const uint8_t* enc_key, const uint16_t enc_key_id);
#else
ATCA_STATUS atcab_write_enc(uint16_t key_id, uint8_t block, const uint8_t* data, const uint8_t* enc_key, const uint16_t enc_key_id, const uint8_t num_in[NONCE_NUMIN_SIZE]);
#endif

ATCA_STATUS atcab_write_config_counter(uint16_t counter_id, uint32_t counter_value);


#endif /* ATCA_TA_SUPPORT && ATCA_CA_SUPPORT */

#ifdef __cplusplus
}
#endif

/** @} */
/*lint -flb*/
#endif /* ATCA_BASIC_H_ */
