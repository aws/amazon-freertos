/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
#pragma once

#ifndef PLATFORM_CRYPTO_H_
#define PLATFORM_CRYPTO_H_

#include "crypto_structures.h"
#include "hw_crypto_api_external.h"

#include "crypto_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          AES context structure
 *
 * \note           buf is able to hold 32 extra bytes, which can be used:
 *                 - for alignment purposes if VIA padlock is used, and/or
 *                 - to simplify key expansion in the 256-bit case by
 *                 generating an extra round key
 */

typedef hw_aes_context_t           wiced_hw_aes_context;
typedef hw_des_context_t           wiced_hw_des_context;
typedef md5_hw_context             wiced_hw_md5_context;
typedef sha256_hw_context          wiced_hw_sha256_context;
typedef sha1_hw_context            wiced_hw_sha1_context;
typedef sha1_hmac_hw_context       wiced_hw_sha1_hmac_context;
typedef sha2_hmac_hw_context       wiced_hw_sha256_hmac_context;
typedef md5_hmac_hw_context        wiced_hw_md5_hmac_context;

void wiced_hwcrypto_init( void );


void wiced_hw_aes_init( wiced_hw_aes_context *ctx );

/**
 * \brief          Clear AES context
 *
 * \param ctx      AES context to be cleared
 */
void wiced_hw_aes_free( wiced_hw_aes_context *ctx );

/**
 * \brief          AES set key schedule (encryption or decryption)
 *
 * \param ctx      AES context to be initialized
 * \param key      encryption key
 * \param keybits  must be 128, 192 or 256
 *
 * \return         0 if successful, or ERR_AES_INVALID_KEY_LENGTH
 */
int wiced_hw_aes_setkey_enc( wiced_hw_aes_context *ctx, const unsigned char *key, unsigned int keybits );

/**
 * \brief          AES set key schedule (encryption or decryption)
 *
 * \param ctx      AES context to be initialized
 * \param key      encryption key
 * \param keybits  must be 128, 192 or 256
 *
 * \return         0 if successful, or ERR_AES_INVALID_KEY_LENGTH
 */
int wiced_hw_aes_setkey_dec( wiced_hw_aes_context *ctx, const unsigned char *key, unsigned int keybits );

/**
 * \brief          AES-ECB block encryption/decryption
 *
 * \param ctx      AES context
 * \param mode     AES_ENCRYPT or AES_DECRYPT
 * \param input    16-byte input block
 * \param output   16-byte output block
 *
 * \return         0 if successful
 */
int wiced_hw_aes_crypt_ecb( wiced_hw_aes_context *ctx, int mode, const unsigned char input[16], unsigned char output[16] );

/**
 * \brief          AES-CBC buffer encryption/decryption
 *                 Length should be a multiple of the block
 *                 size (16 bytes)
 *
 * \note           Upon exit, the content of the IV is updated so that you can
 *                 call the function same function again on the following
 *                 block(s) of data and get the same result as if it was
 *                 encrypted in one call. This allows a "streaming" usage.
 *                 If on the other hand you need to retain the contents of the
 *                 IV, you should either save it manually or use the cipher
 *                 module instead.
 *
 * \param ctx      AES context
 * \param mode     AES_ENCRYPT or AES_DECRYPT
 * \param length   length of the input data
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 *
 * \return         0 if successful, or ERR_AES_INVALID_INPUT_LENGTH
 */
int wiced_hw_aes_crypt_cbc( wiced_hw_aes_context *ctx,
                       int mode,
                       size_t length,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output );


/**
 * \brief          AES-CFB128 buffer encryption/decryption.
 *
 * Note: Due to the nature of CFB you should use the same key schedule for
 * both encryption and decryption. So a context initialized with
 * wiced_hw_aes_setkey_enc() for both AES_ENCRYPT and AES_DECRYPT.
 *
 * \note           Upon exit, the content of the IV is updated so that you can
 *                 call the function same function again on the following
 *                 block(s) of data and get the same result as if it was
 *                 encrypted in one call. This allows a "streaming" usage.
 *                 If on the other hand you need to retain the contents of the
 *                 IV, you should either save it manually or use the cipher
 *                 module instead.
 *
 * \param ctx      AES context
 * \param mode     AES_ENCRYPT or AES_DECRYPT
 * \param length   length of the input data
 * \param iv_off   offset in IV (updated after use)
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 *
 * \return         0 if successful
 */
int wiced_hw_aes_crypt_cfb128( wiced_hw_aes_context *ctx,
                          int mode,
                          size_t length,
                          size_t *iv_off,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output );

/**
 * \brief          AES-CFB8 buffer encryption/decryption.
 *
 * Note: Due to the nature of CFB you should use the same key schedule for
 * both encryption and decryption. So a context initialized with
 * wiced_hw_aes_setkey_enc() for both AES_ENCRYPT and AES_DECRYPT.
 *
 * \note           Upon exit, the content of the IV is updated so that you can
 *                 call the function same function again on the following
 *                 block(s) of data and get the same result as if it was
 *                 encrypted in one call. This allows a "streaming" usage.
 *                 If on the other hand you need to retain the contents of the
 *                 IV, you should either save it manually or use the cipher
 *                 module instead.
 *
 * \param ctx      AES context
 * \param mode     AES_ENCRYPT or AES_DECRYPT
 * \param length   length of the input data
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 *
 * \return         0 if successful
 */
int wiced_hw_aes_crypt_cfb8( wiced_hw_aes_context *ctx,
                        int mode,
                        size_t length,
                        unsigned char iv[16],
                        const unsigned char *input,
                        unsigned char *output );

/**
 * \brief               AES-CTR buffer encryption/decryption
 *
 * Warning: You have to keep the maximum use of your counter in mind!
 *
 * Note: Due to the nature of CTR you should use the same key schedule for
 * both encryption and decryption. So a context initialized with
 * wiced_hw_aes_setkey_enc() for both AES_ENCRYPT and AES_DECRYPT.
 *
 * \param ctx           AES context
 * \param length        The length of the data
 * \param nc_off        The offset in the current stream_block (for resuming
 *                      within current cipher stream). The offset pointer to
 *                      should be 0 at the start of a stream.
 * \param nonce_counter The 128-bit nonce and counter.
 * \param stream_block  The saved stream-block for resuming. Is overwritten
 *                      by the function.
 * \param input         The input data stream
 * \param output        The output data stream
 *
 * \return         0 if successful
 */
int wiced_hw_aes_crypt_ctr( wiced_hw_aes_context *ctx,
                       size_t length,
                       size_t *nc_off,
                       unsigned char nonce_counter[16],
                       unsigned char stream_block[16],
                       const unsigned char *input,
                       unsigned char *output );


/**
 * \brief           Internal AES block encryption function
 *                  (Only exposed to allow overriding it,
 *                  see AES_ENCRYPT_ALT)
 *
 * \param ctx       AES context
 * \param input     Plaintext block
 * \param output    Output (ciphertext) block
 */
void wiced_hw_aes_encrypt( wiced_hw_aes_context *ctx, const unsigned char input[16], unsigned char output[16] );

/**
 * \brief           Internal AES block decryption function
 *                  (Only exposed to allow overriding it,
 *                  see AES_DECRYPT_ALT)
 *
 * \param ctx       AES context
 * \param input     Ciphertext block
 * \param output    Output (plaintext) block
 */
void wiced_hw_aes_decrypt( wiced_hw_aes_context *ctx, const unsigned char input[16], unsigned char output[16] );



void wiced_hw_des_init( wiced_hw_des_context *ctx );
void wiced_hw_des_free( wiced_hw_des_context *ctx );
void wiced_hw_des3_init( wiced_hw_des_context *ctx );
void wiced_hw_des3_free( wiced_hw_des_context *ctx );
int wiced_hw_des_setkey( wiced_hw_des_context *ctx, const unsigned char key[ MBEDTLS_DES_KEY_SIZE ] );
int wiced_hw_des3_set2key( wiced_hw_des_context *ctx, const unsigned char key[ MBEDTLS_DES_KEY_SIZE * 2 ] );
int wiced_hw_des3_set3key( wiced_hw_des_context *ctx, const unsigned char key[ MBEDTLS_DES_KEY_SIZE * 3 ] );
int wiced_hw_des_crypt_ecb( wiced_hw_des_context *ctx, const unsigned char input[ 8 ], unsigned char output[ 8 ] );
int wiced_hw_des3_crypt_ecb( wiced_hw_des_context *ctx, const unsigned char input[ 8 ], unsigned char output[ 8 ] );
int wiced_hw_des_crypt_cbc( wiced_hw_des_context *ctx, int mode, size_t length, unsigned char iv[ 8 ], const unsigned char *input, unsigned char *output );
int wiced_hw_des3_crypt_cbc( wiced_hw_des_context *ctx, int mode, size_t length, unsigned char iv[ 8 ], const unsigned char *input, unsigned char *output );


void wiced_hw_md5_init( wiced_hw_md5_context *ctx );

void wiced_hw_md5_free( wiced_hw_md5_context *ctx );
void wiced_hw_md5_clone( wiced_hw_md5_context *dst, const wiced_hw_md5_context *src );
void wiced_hw_md5_starts( wiced_hw_md5_context *ctx );
void wiced_hw_md5_update( wiced_hw_md5_context *ctx, const unsigned char *input, size_t ilen );
void wiced_hw_md5_finish( wiced_hw_md5_context *ctx, unsigned char output[ 16 ] );
void wiced_md5_hmac_starts( wiced_hw_md5_hmac_context * ctx, const unsigned char *key, uint32_t keylen );
void wiced_md5_hmac_update( wiced_hw_md5_hmac_context * ctx, const unsigned char *input, uint32_t ilen );
void wiced_md5_hmac_finish( wiced_hw_md5_hmac_context * ctx, unsigned char output[ 16 ] );
void wiced_md5_hmac( const unsigned char *key, int32_t keylen, const unsigned char *input, int32_t ilen, unsigned char output[ 16 ] );

void wiced_hw_md5_process( wiced_hw_md5_context *ctx, const unsigned char data[ 64 ] );
void wiced_hw_md5( const unsigned char *input, size_t ilen, unsigned char output[ 16 ] );


void wiced_hw_sha256_init( wiced_hw_sha256_context *ctx );

void wiced_hw_sha256_free( wiced_hw_sha256_context *ctx );
void wiced_hw_sha256_clone( wiced_hw_sha256_context *dst, const wiced_hw_sha256_context *src );
void wiced_hw_sha256_starts( wiced_hw_sha256_context *ctx, int is224 );
void wiced_hw_sha256_update( wiced_hw_sha256_context *ctx, const unsigned char *input, size_t ilen );
void wiced_hw_sha256_finish( wiced_hw_sha256_context *ctx, unsigned char output[ 32 ] );
void wiced_sha256_hmac_starts( wiced_hw_sha256_hmac_context * ctx, const unsigned char *key, uint32_t keylen, int32_t is224 );
void wiced_sha256_hmac_update( wiced_hw_sha256_hmac_context * ctx, const unsigned char *input, uint32_t ilen );
void wiced_sha256_hmac_finish( wiced_hw_sha256_hmac_context * ctx, unsigned char output[ 32 ] );
void wiced_sha256_hmac( const unsigned char *key, int32_t keylen, const unsigned char *input, int32_t ilen, unsigned char output[ 32 ], int32_t is224 );

void wiced_hw_sha256_process( wiced_hw_sha256_context *ctx, const unsigned char data[ 64 ] );
void wiced_hw_sha256( const unsigned char *input, size_t ilen, unsigned char output[32], int is224 );

void wiced_hw_sha1_init( wiced_hw_sha1_context *ctx );

void wiced_hw_sha1_free( wiced_hw_sha1_context *ctx );
void wiced_hw_sha1_clone( wiced_hw_sha1_context *dst, const wiced_hw_sha1_context *src );
void wiced_hw_sha1_starts( wiced_hw_sha1_context *ctx );
void wiced_hw_sha1_update( wiced_hw_sha1_context *ctx, const unsigned char *input, size_t ilen );
void wiced_hw_sha1_finish( wiced_hw_sha1_context *ctx, unsigned char output[ 20 ] );
void wiced_sha1_hmac_starts( wiced_hw_sha1_hmac_context * ctx, const unsigned char *key, uint32_t keylen );
void wiced_sha1_hmac_update( wiced_hw_sha1_hmac_context * ctx, const unsigned char *input, uint32_t ilen );
void wiced_sha1_hmac_finish( wiced_hw_sha1_hmac_context * ctx, unsigned char output[ 20 ] );
void wiced_sha1_hmac( const unsigned char *key, int32_t keylen, const unsigned char *input, int32_t ilen, unsigned char output[ 20 ] );

void wiced_hw_sha1_process( wiced_hw_sha1_context *ctx, const unsigned char data[ 64 ] );
void wiced_hw_sha1( const unsigned char *input, size_t ilen, unsigned char output[20] );


#ifdef __cplusplus
}
#endif
#endif /* PLATFORM_CRYPTO_H_ */
