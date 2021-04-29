#ifndef _CALIB_H
#define _CALIB_H

#include "calib_command.h"
#include "calib_execution.h"

/** \defgroup calib_ Basic Crypto API methods for CryptoAuth Devices (calib_)
 *
 * \brief
 * These methods provide a simple API to CryptoAuth chips
 *
   @{ */

#ifdef __cplusplus
extern "C" {
#endif

ATCA_STATUS calib_wakeup(ATCADevice device);
ATCA_STATUS calib_idle(ATCADevice device);
ATCA_STATUS calib_sleep(ATCADevice device);
ATCA_STATUS _calib_exit(ATCADevice device);
ATCA_STATUS calib_get_addr(uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, uint16_t* addr);
ATCA_STATUS calib_get_zone_size(ATCADevice device, uint8_t zone, uint16_t slot, size_t* size);
ATCA_STATUS calib_ecc204_get_addr(uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, uint16_t* addr);

//AES command functions
ATCA_STATUS calib_aes(ATCADevice device, uint8_t mode, uint16_t key_id, const uint8_t* aes_in, uint8_t* aes_out);
ATCA_STATUS calib_aes_encrypt(ATCADevice device, uint16_t key_id, uint8_t key_block, const uint8_t* plaintext, uint8_t* ciphertext);
ATCA_STATUS calib_aes_decrypt(ATCADevice device, uint16_t key_id, uint8_t key_block, const uint8_t* ciphertext, uint8_t* plaintext);
ATCA_STATUS calib_aes_gfm(ATCADevice device, const uint8_t* h, const uint8_t* input, uint8_t* output);

//CheckMAC command functions
ATCA_STATUS calib_checkmac(ATCADevice device, uint8_t mode, uint16_t key_id, const uint8_t *challenge, const uint8_t *response, const uint8_t *other_data);

// Counter command functions
ATCA_STATUS calib_counter(ATCADevice device, uint8_t mode, uint16_t counter_id, uint32_t* counter_value);
ATCA_STATUS calib_counter_increment(ATCADevice device, uint16_t counter_id, uint32_t* counter_value);
ATCA_STATUS calib_counter_read(ATCADevice device, uint16_t counter_id, uint32_t* counter_value);

// DeriveKey command functions
ATCA_STATUS calib_derivekey(ATCADevice device, uint8_t mode, uint16_t key_id, const uint8_t* mac);

// ECDH command functions
ATCA_STATUS calib_ecdh_base(ATCADevice device, uint8_t mode, uint16_t key_id, const uint8_t* public_key, uint8_t* pms, uint8_t* out_nonce);
ATCA_STATUS calib_ecdh(ATCADevice device, uint16_t key_id, const uint8_t* public_key, uint8_t* pms);

#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS calib_ecdh_enc(ATCADevice device, uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* read_key, uint16_t read_key_id);
#else
ATCA_STATUS calib_ecdh_enc(ATCADevice device, uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* read_key, uint16_t read_key_id, const uint8_t num_in[NONCE_NUMIN_SIZE]);
#endif

ATCA_STATUS calib_ecdh_ioenc(ATCADevice device, uint16_t key_id, const uint8_t* public_key, uint8_t* pms, const uint8_t* io_key);
ATCA_STATUS calib_ecdh_tempkey(ATCADevice device, const uint8_t* public_key, uint8_t* pms);
ATCA_STATUS calib_ecdh_tempkey_ioenc(ATCADevice device, const uint8_t* public_key, uint8_t* pms, const uint8_t* io_key);

// GenDig command functions
ATCA_STATUS calib_gendig(ATCADevice device, uint8_t zone, uint16_t key_id, const uint8_t *other_data, uint8_t other_data_size);

// GenKey command functions
ATCA_STATUS calib_genkey_base(ATCADevice device, uint8_t mode, uint16_t key_id, const uint8_t* other_data, uint8_t* public_key);
ATCA_STATUS calib_genkey(ATCADevice device, uint16_t key_id, uint8_t* public_key);
ATCA_STATUS calib_get_pubkey(ATCADevice device, uint16_t key_id, uint8_t* public_key);
ATCA_STATUS calib_genkey_mac(ATCADevice device, uint8_t* public_key, uint8_t* mac);

// HMAC command functions
ATCA_STATUS calib_hmac(ATCADevice device, uint8_t mode, uint16_t key_id, uint8_t* digest);

// Info command functions
ATCA_STATUS calib_info_base(ATCADevice device, uint8_t mode, uint16_t param2, uint8_t* out_data);
ATCA_STATUS calib_info(ATCADevice device, uint8_t* revision);
ATCA_STATUS calib_info_set_latch(ATCADevice device, bool state);
ATCA_STATUS calib_info_get_latch(ATCADevice device, bool* state);
ATCA_STATUS calib_info_privkey_valid(ATCADevice device, uint16_t key_id, uint8_t* is_valid);
ATCA_STATUS calib_info_lock_status(ATCADevice device, uint16_t param2, uint8_t* is_locked);
ATCA_STATUS calib_ecc204_is_locked(ATCADevice device, uint8_t zone, bool* is_locked);
ATCA_STATUS calib_ecc204_is_data_locked(ATCADevice device, bool* is_locked);
ATCA_STATUS calib_ecc204_is_config_locked(ATCADevice device, bool* is_locked);

// KDF command functions
ATCA_STATUS calib_kdf(ATCADevice device, uint8_t mode, uint16_t key_id, const uint32_t details, const uint8_t* message, uint8_t* out_data, uint8_t* out_nonce);

// Lock command functions
ATCA_STATUS calib_lock(ATCADevice device, uint8_t mode, uint16_t summary_crc);
ATCA_STATUS calib_lock_config_zone(ATCADevice device);
ATCA_STATUS calib_lock_config_zone_crc(ATCADevice device, uint16_t summary_crc);
ATCA_STATUS calib_lock_data_zone(ATCADevice device);
ATCA_STATUS calib_lock_data_zone_crc(ATCADevice device, uint16_t summary_crc);
ATCA_STATUS calib_lock_data_slot(ATCADevice device, uint16_t slot);
// Lock ECC204 command functions
ATCA_STATUS calib_ecc204_lock_config_slot(ATCADevice device, uint8_t slot, uint16_t summary_crc);
ATCA_STATUS calib_ecc204_lock_config_zone(ATCADevice device);
ATCA_STATUS calib_ecc204_lock_data_slot(ATCADevice device, uint8_t slot);

// MAC command functions
ATCA_STATUS calib_mac(ATCADevice device, uint8_t mode, uint16_t key_id, const uint8_t* challenge, uint8_t* digest);

// Nonce command functions
ATCA_STATUS calib_nonce_base(ATCADevice device, uint8_t mode, uint16_t zero, const uint8_t *num_in, uint8_t* rand_out);
ATCA_STATUS calib_nonce(ATCADevice device, const uint8_t *num_in);
ATCA_STATUS calib_nonce_load(ATCADevice device, uint8_t target, const uint8_t *num_in, uint16_t num_in_size);
ATCA_STATUS calib_nonce_rand(ATCADevice device, const uint8_t *num_in, uint8_t* rand_out);
ATCA_STATUS calib_challenge(ATCADevice device, const uint8_t *num_in);
ATCA_STATUS calib_challenge_seed_update(ATCADevice device, const uint8_t *num_in, uint8_t* rand_out);
ATCA_STATUS calib_nonce_gen_session_key(ATCADevice device, uint16_t param2, uint8_t* num_in,
                                        uint8_t* rand_out);

// PrivWrite command functions

#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS calib_priv_write(ATCADevice device, uint16_t key_id, const uint8_t priv_key[36], uint16_t write_key_id, const uint8_t write_key[32]);
#else
ATCA_STATUS calib_priv_write(ATCADevice device, uint16_t key_id, const uint8_t priv_key[36], uint16_t write_key_id, const uint8_t write_key[32], const uint8_t num_in[NONCE_NUMIN_SIZE]);
#endif
// Random command functions
ATCA_STATUS calib_random(ATCADevice device, uint8_t* rand_out);

// Read command functions
ATCA_STATUS calib_read_zone(ATCADevice device, uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, uint8_t *data, uint8_t len);
ATCA_STATUS calib_is_locked(ATCADevice device, uint8_t zone, bool *is_locked);
ATCA_STATUS calib_is_slot_locked(ATCADevice device, uint16_t slot, bool *is_locked);
ATCA_STATUS calib_read_bytes_zone(ATCADevice device, uint8_t zone, uint16_t slot, size_t offset, uint8_t *data, size_t length);
ATCA_STATUS calib_read_serial_number(ATCADevice device, uint8_t* serial_number);
ATCA_STATUS calib_read_pubkey(ATCADevice device, uint16_t slot, uint8_t *public_key);
ATCA_STATUS calib_read_sig(ATCADevice device, uint16_t slot, uint8_t *sig);
ATCA_STATUS calib_read_config_zone(ATCADevice device, uint8_t* config_data);
ATCA_STATUS calib_cmp_config_zone(ATCADevice device, uint8_t* config_data, bool* same_config);
// ECC204 Read command functions
ATCA_STATUS calib_ecc204_read_zone(ATCADevice device, uint8_t zone, uint8_t slot, uint8_t block, size_t offset,
                                   uint8_t* data, uint8_t len);
ATCA_STATUS calib_ecc204_read_config_zone(ATCADevice device, uint8_t* config_data);
ATCA_STATUS calib_ecc204_read_serial_number(ATCADevice device, uint8_t* serial_number);
ATCA_STATUS calib_ecc204_read_bytes_zone(ATCADevice device, uint8_t zone, uint16_t slot,
                                         size_t block, uint8_t* data, size_t length);
ATCA_STATUS calib_ecc204_cmp_config_zone(ATCADevice device, uint8_t* config_data, bool* same_config);

#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS calib_read_enc(ATCADevice device, uint16_t key_id, uint8_t block, uint8_t *data, const uint8_t* enc_key, const uint16_t enc_key_id);
#else
ATCA_STATUS calib_read_enc(ATCADevice device, uint16_t key_id, uint8_t block, uint8_t *data, const uint8_t* enc_key, const uint16_t enc_key_id, const uint8_t num_in[NONCE_NUMIN_SIZE]);
#endif

// SecureBoot command functions
ATCA_STATUS calib_secureboot(ATCADevice device, uint8_t mode, uint16_t param2, const uint8_t* digest, const uint8_t* signature, uint8_t* mac);
ATCA_STATUS calib_secureboot_mac(ATCADevice device, uint8_t mode, const uint8_t* digest, const uint8_t* signature, const uint8_t* num_in, const uint8_t* io_key, bool* is_verified);

// SelfTest command functions
ATCA_STATUS calib_selftest(ATCADevice device, uint8_t mode, uint16_t param2, uint8_t* result);

// SHA command functions
typedef struct atca_sha256_ctx
{
    uint32_t total_msg_size;                    //!< Total number of message bytes processed
    uint32_t block_size;                        //!< Number of bytes in current block
    uint8_t  block[ATCA_SHA256_BLOCK_SIZE * 2]; //!< Unprocessed message storage
} atca_sha256_ctx_t;

typedef atca_sha256_ctx_t atca_hmac_sha256_ctx_t;

ATCA_STATUS calib_sha_base(ATCADevice device, uint8_t mode, uint16_t length, const uint8_t* data_in, uint8_t* data_out, uint16_t* data_out_size);
ATCA_STATUS calib_sha_start(ATCADevice device);
ATCA_STATUS calib_sha_update(ATCADevice device, const uint8_t* message);
ATCA_STATUS calib_sha_end(ATCADevice device, uint8_t *digest, uint16_t length, const uint8_t *message);
ATCA_STATUS calib_sha_read_context(ATCADevice device, uint8_t* context, uint16_t* context_size);
ATCA_STATUS calib_sha_write_context(ATCADevice device, const uint8_t* context, uint16_t context_size);
ATCA_STATUS calib_sha(ATCADevice device, uint16_t length, const uint8_t *message, uint8_t *digest);
ATCA_STATUS calib_hw_sha2_256(ATCADevice device, const uint8_t * data, size_t data_size, uint8_t* digest);
ATCA_STATUS calib_hw_sha2_256_init(ATCADevice device, atca_sha256_ctx_t* ctx);
ATCA_STATUS calib_hw_sha2_256_update(ATCADevice device, atca_sha256_ctx_t* ctx, const uint8_t* data, size_t data_size);
ATCA_STATUS calib_hw_sha2_256_finish(ATCADevice device, atca_sha256_ctx_t* ctx, uint8_t* digest);
ATCA_STATUS calib_sha_hmac_init(ATCADevice device, atca_hmac_sha256_ctx_t* ctx, uint16_t key_slot);
ATCA_STATUS calib_sha_hmac_update(ATCADevice device, atca_hmac_sha256_ctx_t* ctx, const uint8_t* data, size_t data_size);
ATCA_STATUS calib_sha_hmac_finish(ATCADevice device, atca_hmac_sha256_ctx_t* ctx, uint8_t* digest, uint8_t target);
ATCA_STATUS calib_sha_hmac(ATCADevice device, const uint8_t * data, size_t data_size, uint16_t key_slot, uint8_t* digest, uint8_t target);

// Sign command functions
ATCA_STATUS calib_sign_base(ATCADevice device, uint8_t mode, uint16_t key_id, uint8_t *signature);
ATCA_STATUS calib_sign(ATCADevice device, uint16_t key_id, const uint8_t *msg, uint8_t *signature);
ATCA_STATUS calib_sign_internal(ATCADevice device, uint16_t key_id, bool is_invalidate, bool is_full_sn, uint8_t *signature);
// ECC204 Sign command functions
ATCA_STATUS calib_ecc204_sign(ATCADevice device, uint16_t key_id, const uint8_t* msg, uint8_t* signature);

// UpdateExtra command functions
ATCA_STATUS calib_updateextra(ATCADevice device, uint8_t mode, uint16_t new_value);

// Verify command functions
ATCA_STATUS calib_verify(ATCADevice device, uint8_t mode, uint16_t key_id, const uint8_t* signature, const uint8_t* public_key, const uint8_t* other_data, uint8_t* mac);
ATCA_STATUS calib_verify_extern(ATCADevice device, const uint8_t *message, const uint8_t *signature, const uint8_t *public_key, bool *is_verified);
ATCA_STATUS calib_verify_extern_mac(ATCADevice device, const uint8_t *message, const uint8_t* signature, const uint8_t* public_key, const uint8_t* num_in, const uint8_t* io_key, bool* is_verified);
ATCA_STATUS calib_verify_stored(ATCADevice device, const uint8_t *message, const uint8_t *signature, uint16_t key_id, bool *is_verified);
ATCA_STATUS calib_verify_stored_mac(ATCADevice device, const uint8_t *message, const uint8_t *signature, uint16_t key_id, const uint8_t* num_in, const uint8_t* io_key, bool* is_verified);
ATCA_STATUS calib_verify_validate(ATCADevice device, uint16_t key_id, const uint8_t *signature, const uint8_t *other_data, bool *is_verified);
ATCA_STATUS calib_verify_invalidate(ATCADevice device, uint16_t key_id, const uint8_t *signature, const uint8_t *other_data, bool *is_verified);

// Write command functions
ATCA_STATUS calib_write(ATCADevice device, uint8_t zone, uint16_t address, const uint8_t *value, const uint8_t *mac);
ATCA_STATUS calib_write_zone(ATCADevice device, uint8_t zone, uint16_t slot, uint8_t block, uint8_t offset, const uint8_t *data, uint8_t len);
ATCA_STATUS calib_write_bytes_zone(ATCADevice device, uint8_t zone, uint16_t slot, size_t offset_bytes, const uint8_t *data, size_t length);
ATCA_STATUS calib_write_pubkey(ATCADevice device, uint16_t slot, const uint8_t *public_key);
ATCA_STATUS calib_write_config_zone(ATCADevice device, const uint8_t* config_data);
// ECC204 Write command functions
ATCA_STATUS calib_ecc204_write(ATCADevice device, uint8_t zone, uint16_t address, const uint8_t *value,
                               const uint8_t *mac);
ATCA_STATUS calib_ecc204_write_zone(ATCADevice device, uint8_t zone, uint16_t slot, uint8_t block,
                                    uint8_t offset, const uint8_t *data, uint8_t len);
ATCA_STATUS calib_ecc204_write_config_zone(ATCADevice device, uint8_t* config_data);
ATCA_STATUS calib_ecc204_write_bytes_zone(ATCADevice device, uint8_t zone, uint16_t slot, size_t block,
                                          const uint8_t *data, size_t length);

#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS calib_write_enc(ATCADevice device, uint16_t key_id, uint8_t block, const uint8_t *data, const uint8_t* enc_key, const uint16_t enc_key_id);
#else
ATCA_STATUS calib_write_enc(ATCADevice device, uint16_t key_id, uint8_t block, const uint8_t *data, const uint8_t* enc_key, const uint16_t enc_key_id, const uint8_t num_in[NONCE_NUMIN_SIZE]);
#endif

#if defined(ATCA_USE_CONSTANT_HOST_NONCE)
ATCA_STATUS calib_ecc204_write_enc(ATCADevice device, uint8_t slot, uint8_t* data, uint8_t* transport_key,
                                   uint8_t key_id);
#else
ATCA_STATUS calib_ecc204_write_enc(ATCADevice device, uint8_t slot, uint8_t* data, uint8_t* transport_key,
                                   uint8_t key_id, uint8_t num_in[NONCE_NUMIN_SIZE]);
#endif

ATCA_STATUS calib_write_config_counter(ATCADevice device, uint16_t counter_id, uint32_t counter_value);

#ifdef __cplusplus
}
#endif

/** @} */

#endif