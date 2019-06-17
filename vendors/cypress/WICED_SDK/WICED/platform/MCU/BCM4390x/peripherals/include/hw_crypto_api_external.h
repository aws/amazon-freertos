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
#ifndef _CRYPTO_STRUCTURES_H
#define _CRYPTO_STRUCTURES_H
#include <platform_cache_def.h>

/******************************************************
 *                      Macros
 ******************************************************/
#define HWCRYPTO_BLOCK_SIZE         512 /* Should be multiple of 64, and increasing
                                           the value of it will increase the stack
                                           and heap usage */

/******************************************************
 *                    Constants
 ******************************************************/
#define HWCRYPTO_ALIGNMENT          16
#if defined( PLATFORM_L1_CACHE_SHIFT )
#define HWCRYPTO_ALIGNMENT_BYTES    MAX(HWCRYPTO_ALIGNMENT, PLATFORM_L1_CACHE_BYTES)
#else
#define HWCRYPTO_ALIGNMENT_BYTES    HWCRYPTO_ALIGNMENT
#endif /* defined( PLATFORM_L1_CACHE_SHIFT ) */
#define HWCRYPTO_DESCRIPTOR_ALIGNMENT_BYTES HWCRYPTO_ALIGNMENT_BYTES

#define AES_MAX_KEY_LEN                 32
#define AES128_KEYSIZE_BITS             128
#define AES192_KEYSIZE_BITS             192
#define AES256_KEYSIZE_BITS             256
#define AES_BLOCK_SIZE                  16
#define DES_MAX_KEY_LEN                 24
#define SHA256_HASH_SIZE                32
#define SHA224_HASH_SIZE                28
#define SHA1_HASH_SIZE                  20
#define MD5_HASH_SIZE                   16
#define HMAC256_OUTPUT_SIZE             32
#define HMAC224_OUTPUT_SIZE             28
#define HMAC_INNERHASHCONTEXT_SIZE      64
#define HMAC_OUTERHASHCONTEXT_SIZE      64

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef enum
{
    HW_AES_ENCRYPT     = 0,
    HW_AES_DECRYPT     = 1
} hw_aes_mode_type_t;

typedef enum
{
    HW_DES_ENCRYPT     = 0,
    HW_DES_DECRYPT     = 1
} hw_des_mode_type_t;

typedef enum
{
    HASH_INITIAL = 0,
    HASH_STARTS = 1,
    HASH_UPDATE = 2,
    HASH_FINISH = 3,
    HASH_NONE = 4,
} hash_state_t;

typedef struct
{
            uint8_t     state[ SHA256_HASH_SIZE ];               /* HMAC Key/ Result of previous HASH (Used in HASH_UPDT/HASH_FINISH) */
            uint8_t*    payload_buffer;                          /* Buffer to store Output payload
                                                                    HWCrypto engine outputs the input payload + HASH result */
            uint8_t     hash_optype;                             /* HASH_INIT/HASH_UPDT/HASH_FINISH, Used for incremental hash operations */
            uint32_t    prev_len;                                /* Used for HASH_UPDT/HASH_FISH, length of data hashed till now */
            int32_t     is224;                                   /* 0 : SHA256, 1: SHA256 */
            uint8_t     i_key_pad[ HMAC_INNERHASHCONTEXT_SIZE ]; /* Used for HMAC > HWCRYPTO_MAX_PAYLOAD_SIZE */
            uint8_t     o_key_pad[ HMAC_INNERHASHCONTEXT_SIZE ]; /* Used for HMAC > HWCRYPTO_MAX_PAYLOAD_SIZE */
} sha256_context_t;

typedef struct
{
            uint8_t     state[ SHA256_HASH_SIZE ];               /* HMAC Key/ Result of previous HASH (Used in HASH_UPDT/HASH_FINISH); must be 32bits for HMAC 256 */
            uint8_t     hash_optype;                             /* HASH_INIT/HASH_UPDT/HASH_FINISH, Used for incremental hash operations */
            uint32_t    prev_len;                                /* Used for HASH_UPDT/HASH_FISH, length of data hashed till now */

            uint8_t     i_key_pad[ HMAC_INNERHASHCONTEXT_SIZE ]; /* Used for HMAC > HWCRYPTO_MAX_PAYLOAD_SIZE */
            uint8_t     o_key_pad[ HMAC_INNERHASHCONTEXT_SIZE ]; /* Used for HMAC > HWCRYPTO_MAX_PAYLOAD_SIZE */
            uint8_t*    payload_buffer;                          /* Buffer to store Output payload
                                                                    HWCrypto engine outputs the input payload + HASH result */

} sha1_context_t;

typedef struct
{
            uint8_t     state[ SHA256_HASH_SIZE ];               /* HMAC Key/ Result of previous HASH (Used in HASH_UPDT/HASH_FINISH); must be 32bits for HMAC 256 */
            uint8_t*    payload_buffer;                          /* Buffer to store Output payload
                                                                    HWCrypto engine outputs the input payload + HASH result */
            uint8_t     hash_optype;                             /* HASH_INIT/HASH_UPDT/HASH_FINISH, Used for incremental hash operations */
            uint32_t    prev_len;                                /* Used for HASH_UPDT/HASH_FISH, length of data hashed till now */

            uint8_t     i_key_pad[ HMAC_INNERHASHCONTEXT_SIZE ]; /* Used for HMAC > HWCRYPTO_MAX_PAYLOAD_SIZE */
            uint8_t     o_key_pad[ HMAC_INNERHASHCONTEXT_SIZE ]; /* Used for HMAC > HWCRYPTO_MAX_PAYLOAD_SIZE */
} md5_context_t;

typedef struct
{
            uint8_t       key[ AES_MAX_KEY_LEN ];   /* AES Key */
            uint32_t      keylen;                   /* AES Key length */
            uint32_t      direction;                /* OUTBOUND : HW_AES_ENCRYPT, INBOUND : HW_AES_DECRYPT */
            uint32_t      cipher_mode;              /* CBC/ECB/CTR/OFB */
} hw_aes_context_t;

typedef struct
{
            uint8_t     key[DES_MAX_KEY_LEN];        /* DES Key */
            uint32_t    keylen;                      /* DES Key length */
            uint32_t    direction;                   /* OUTBOUND : HW_DES_ENCRYPT, INBOUND : HW_DES_DECRYPT */
            uint32_t    cipher_mode;                 /* CBC/ECB */
            uint32_t    crypt_algo;
} hw_des_context_t;

typedef struct
{
    hash_state_t flags;
    uint32_t total;
    uint32_t last;
    uint8_t    payload_buffer[HWCRYPTO_BLOCK_SIZE + SHA1_HASH_SIZE + HWCRYPTO_ALIGNMENT_BYTES - 1];
    uint8_t left_over_bytes[64];
    sha1_context_t hw_ctx;
}sha1_hw_context;

typedef struct
{
    sha1_hw_context ctx;
}sha1_hmac_hw_context;

typedef struct
{
    hash_state_t flags;
    uint32_t total;
    uint32_t last;
    uint8_t  payload_buffer[ HWCRYPTO_BLOCK_SIZE + SHA256_HASH_SIZE + HWCRYPTO_ALIGNMENT_BYTES - 1];
    uint8_t  left_over_bytes[64];
    sha256_context_t hw_ctx;
} sha256_hw_context;

typedef struct
{
    sha256_hw_context ctx;
} sha2_hmac_hw_context;

typedef struct
{
    hash_state_t flags;
    uint32_t total;
    uint32_t last;
    uint8_t left_over_bytes[64];
    uint8_t    payload_buffer[HWCRYPTO_BLOCK_SIZE + MD5_HASH_SIZE + HWCRYPTO_ALIGNMENT_BYTES - 1];
    md5_context_t hw_ctx;
} md5_hw_context;

typedef struct
{
    md5_hw_context ctx;
} md5_hmac_hw_context;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/* Initialize the Crypto Core */
void platform_hwcrypto_init( void );

/**
 * SHA256 HASH on data size < HWCRYPTO_MAX_PAYLOAD_SIZE
 * @Deprecated, use platform_hwcrypto_sha2() instead
 * @Assumption
 * Size is < HWCRYPTO_MAX_PAYLOAD_SIZE
 * output is aligned to CRYPTO_OPTIMIZED_DESCRIPTOR_ALIGNMENT
 */
platform_result_t platform_hwcrypto_sha256_hash( uint8_t* source, uint32_t size, uint8_t* output_payload_buffer, uint8_t* hash_output );

/**
 * SHA256 INIT
 * @Assumption : input_len is multiple of 64
 * @param[in]   ctx             sha256_context
 *                              HWcrypto Outputs the PAYLOAD + HASH RESULT
 *                              ctx->payload_buffer should be initialized to point to a buffer big
 *                              enough to contain the input payload
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[in]   is224           1: SHA224 0: SHA256
 *
 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_sha2_starts( sha256_context_t* ctx, const unsigned char* input, uint32_t ilen, int32_t is224 );

/**
 * SHA256 UPDATE
 * @Assumption : input_len is multiple of 64
 *
 * @param[in]   ctx             sha256_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated

 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_sha2_update( sha256_context_t* ctx, const unsigned char* input, uint32_t ilen );

/**
 * SHA256 FINISH
 * @param[in]   ctx             sha256_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 */
platform_result_t platform_hwcrypto_sha2_finish( sha256_context_t* ctx, const unsigned char* input, uint32_t ilen, unsigned char output[ SHA256_HASH_SIZE ] );

/**
 * SPU-M (HWCrypto Engine) can only hash HWCRYPTO_MAX_PAYLOAD_SIZE payload at a time.
 * for larger payloads, divide the hash into HASH_INIT, HASH_UPDT, HASH_FINISH.
 * SPU-M User guide Rev0.31 Page 41 Section 3.4.1
 *
 * @param[in]   input           pointer to input data/payload
 * @param[in]   ilen            length of input data/payload
 * @param[out]  output          buffer to store output data
 * @param[in]   is224           1: SHA224 0: SHA256
 */
platform_result_t platform_hwcrypto_sha2_incremental( const unsigned char* input, uint32_t ilen, unsigned char output[ SHA256_HASH_SIZE ], int32_t is224 );

/**
 * SHA256
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 * @param[in]   is224           0 : SHA256 1: SHA224
 */
platform_result_t platform_hwcrypto_sha2( const unsigned char* input, uint32_t ilen, unsigned char hash_output[ SHA256_HASH_SIZE ], unsigned char *payload_buffer, int32_t is224 );

/**
 * SHA256 HMAC on data size < HWCRYPTO_MAX_PAYLOAD_SIZE
 * @Deprecated, use platform_hwcrypto_sha2_hmac() instead
 * @Assumption
 * Size is < HWCRYPTO_MAX_PAYLOAD_SIZE
 * output is aligned to CRYPTO_OPTIMIZED_DESCRIPTOR_ALIGNMENT
 */

platform_result_t platform_hwcrypto_sha256_hmac( uint8_t *hmac_key, uint32_t keysize, uint8_t *source, uint32_t size, uint8_t *payload_buffer, uint8_t *hash_output );

/**
 * SHA256 HMAC INIT
 * Calculates HASH(key XOR ipad)
 * HASH(key XOR ipad || data)
 *
 * @param[in]   ctx             sha256_context containing the previous sha result
 *                              HWcrypto Outputs the PAYLOAD + HASH RESULT
 *                              ctx->payload_buffer should be initialized to point to a buffer big
 *                              enough to contain the input payload
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 * @param[in]   is224           1: SHA224 0: SHA256
 *
 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_sha2_hmac_starts( sha256_context_t* ctx, const unsigned char* key, uint32_t keylen, int32_t is224 );

/**
 * SHA256 HMAC UPDATE
 * HASH(key XOR ipad || data)
 * @Assumption : input_len is multiple of 64 */
/*
 * @param[in]   ctx             sha256_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated

 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_sha2_hmac_update( sha256_context_t* ctx, const unsigned char* input, int32_t ilen );

/**
 * SHA256 HMAC FINISH
 * @param[in]   ctx             sha256_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 */
platform_result_t platform_hwcrypto_sha2_hmac_finish( sha256_context_t* ctx, uint8_t* input, int32_t ilen, unsigned char output[ SHA256_HASH_SIZE ] );

/**
 * SHA256HMAC Authentication
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 * @param[in]   is224           0 : SHA256 1: SHA224
 */
platform_result_t platform_hwcrypto_sha2_hmac( const unsigned char* key, uint32_t keylen, const unsigned char* input, uint32_t ilen, unsigned char output[ SHA256_HASH_SIZE ], unsigned char *payload_buffer, int32_t is224 );

/**
 * SHA256HMAC Authentication For lengths > HWCRYPTO_MAX_PAYLOAD_SIZE
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 * @param[in]   is224           0 : SHA256 1: SHA224
 *
 * SPU-M does not support SHA-256 HMAC of > 64K , but it supports SHA-256 Hash for > 64K data
 * So HMAC is computed by using Hash as described below.
 * HMAC = Hash(key XOR opad || HASH(key XOR ipad || data))
 * (|| -> append, ipad -> 64 byte array of 0x3C, opad -> 64 byte array of 0x5c )
 * InnexHashContext -> key XOR ipad
 * OuterHashContext -> key XOR opad
 *
 * For Details Refer to :
 * APPENDIX B: Summary of Hash Modes and
 * 3.4.1 : Code authentication Using Incremental Hash operation
 *
 * HWcrypto Outputs the PAYLOAD + HASH RESULT
 * ctx->payload_buffer has the output payload
 * output has the HASH result in case of init/updt/final
 */
platform_result_t platform_hwcrypto_sha2_hmac_incremental( const unsigned char* key, uint32_t keylen, const unsigned char* input, uint32_t ilen, unsigned char output[ SHA256_HASH_SIZE ], int32_t is224 );

/**
 * Combo AES128 CBC decryption + SHA256HMAC Authentication
 * For data size < ( HWCRYPTO_MAX_PAYLOAD_SIZE )
 * @param[in]   crypt_key       AES key
 * @param[in]   crypt_iv        AES IV
 * @param[in]   crypt_size      Size of data to be decrypted
 * @param[in]   auth_size       Size of data to be authenticated
 * @param[in]   hmac_key        HMAC key
 * @param[in]   hmac_key_len    HMAC key length
 * @param[in]   src             input data
 * @param[out]  crypt_dest      Result of decryption
 * @param[out]  hash_dest       Result of authentication
 */
platform_result_t platform_hwcrypto_aescbc_decrypt_sha256_hmac( uint8_t* crypt_key, uint8_t* crypt_iv, uint32_t crypt_size, uint32_t auth_size, uint8_t* hmac_key, uint32_t hmac_key_len, uint8_t* src, uint8_t* crypt_dest, uint8_t* hash_dest );

/**
 * Combo SHA256HMAC Authentication + AES128 CBC encryption
 * For data size < ( HWCRYPTO_MAX_PAYLOAD_SIZE )
 * @param[in]   crypt_key       AES key
 * @param[in]   crypt_iv        AES IV
 * @param[in]   crypt_size      Size of data to be encrypted
 * @param[in]   auth_size       Size of data to be authenticated
 * @param[in]   hmac_key        HMAC key
 * @param[in]   hmac_key_len    HMAC key length
 * @param[in]   src             input data
 * @param[out]  crypt_dest      Result of encryption
 * @param[out]  hash_dest       Result of authentication
 */
platform_result_t platform_hwcrypto_sha256_hmac_aescbc_encrypt( uint8_t* crypt_key, uint8_t* crypt_iv, uint32_t crypt_size, uint32_t auth_size, uint8_t* hmac_key, uint32_t hmac_key_len, uint8_t* src, uint8_t* crypt_dest, uint8_t* hash_dest );

/**
 * AES128 CBC encryption
 * Deprecated, use hw_aes_crypt_cbc() instead
 * For data size < ( HWCRYPTO_MAX_PAYLOAD_SIZE )
 * @param[in]   crypt_key       AES key
 * @param[in]   crypt_iv        AES IV
 * @param[in]   crypt_size      Size of data to be encrypted
 * @param[in]   src             input data
 * @param[out]  crypt_dest      Result of encryption
 */
platform_result_t platform_hwcrypto_aescbc_encrypt( uint8_t* crypt_key, uint8_t* crypt_iv, uint32_t crypt_size, uint8_t* src, uint8_t* crypt_dest );

/**
 * AES128 CBC decryption
 * Deprecated, use hw_aes_crypt_cbc() instead
 * For data size < ( HWCRYPTO_MAX_PAYLOAD_SIZE )
 * @param[in]   key             AES key
 * @param[in]   keysize         AES key length
 * @param[in]   iv              AES IV
 * @param[in]   size            Size of data to be decrypted
 * @param[in]   src             input data
 * @param[out]  crypt_dest      Result of encryption
 */
platform_result_t platform_hwcrypto_aes128cbc_decrypt( uint8_t* key, uint32_t keysize, uint8_t* iv, uint32_t size, uint8_t* src, uint8_t* dest );

/**
 * SHA1 INIT
 * @Assumption : input_len is multiple of 64
 * @param[in]   ctx             sha1_context
 *                              HWcrypto Outputs the PAYLOAD + HASH RESULT
 *                              ctx->payload_buffer should be initialized to point to a buffer big
 *                              enough to contain the input payload
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 *
 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_sha1_starts( sha1_context_t* ctx, const unsigned char* input, uint32_t ilen );

/**
 * SHA1 UPDATE
 * @Assumption : input_len is multiple of 64
 *
 * @param[in]   ctx             sha1_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated

 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_sha1_update( sha1_context_t* ctx, const unsigned char* input, uint32_t ilen );

/**
 * SHA1 FINISH
 * @param[in]   ctx             sha1_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 */
platform_result_t platform_hwcrypto_sha1_finish( sha1_context_t* ctx, const unsigned char* input, uint32_t ilen, unsigned char output[ SHA1_HASH_SIZE ] );

/**
 * SPU-M (HWCrypto Engine) can only hash HWCRYPTO_MAX_PAYLOAD_SIZE payload at a time.
 * for larger payloads, divide the hash into HASH_INIT, HASH_UPDT, HASH_FINISH.
 * SPU-M User guide Rev0.31 Page 41 Section 3.4.1
 *
 * @param[in]   input           pointer to input data/payload
 * @param[in]   ilen            length of input data/payload
 * @param[out]  output          buffer to store output data
 */
platform_result_t platform_hwcrypto_sha1_incremental( const unsigned char* input, uint32_t ilen, unsigned char output[ SHA1_HASH_SIZE ] );

/**
 * SHA1
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 */
platform_result_t platform_hwcrypto_sha1( const unsigned char* input, uint32_t ilen, unsigned char hash_output[ SHA1_HASH_SIZE ], unsigned char* payload_buffer );

/**
 * SHA1 HMAC INIT
 * Calculates HASH(key XOR ipad)
 * HASH(key XOR ipad || data)
 *
 * @param[in]   ctx             sha1_context containing the previous sha result
 *                              HWcrypto Outputs the PAYLOAD + HASH RESULT
 *                              ctx->payload_buffer should be initialized to point to a buffer big
 *                              enough to contain the input payload
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 *
 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_sha1_hmac_starts( sha1_context_t* ctx, const unsigned char* key, uint32_t keylen );

/**
 * SHA1 HMAC UPDATE
 * HASH(key XOR ipad || data)
 * @Assumption : input_len is multiple of 64 */
/*
 * @param[in]   ctx             sha1_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated

 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_sha1_hmac_update( sha1_context_t* ctx, const unsigned char* input, int32_t ilen );

/**
 * SHA1 HMAC FINISH
 * @param[in]   ctx             sha1_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 */
platform_result_t platform_hwcrypto_sha1_hmac_finish( sha1_context_t* ctx, uint8_t* input, int32_t ilen, unsigned char output[ SHA1_HASH_SIZE ] );

/**
 * SHA1HMAC Authentication
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 */
platform_result_t platform_hwcrypto_sha1_hmac( const unsigned char* key, uint32_t keylen, const unsigned char* input, uint32_t ilen, unsigned char output[ SHA1_HASH_SIZE ], unsigned char* payload_buffer );

/**
 * SHA1HMAC Authentication For lengths > HWCRYPTO_MAX_PAYLOAD_SIZE
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 *
 * SPU-M does not support SHA-1 HMAC of > 64K , but it supports SHA-1 Hash for > 64K data
 * So HMAC is computed by using Hash as described below.
 * HMAC = Hash(key XOR opad || HASH(key XOR ipad || data))
 * (|| -> append, ipad -> 64 byte array of 0x3C, opad -> 64 byte array of 0x5c )
 * InnexHashContext -> key XOR ipad
 * OuterHashContext -> key XOR opad
 *
 * For Details Refer to :
 * APPENDIX B: Summary of Hash Modes and
 * 3.4.1 : Code authentication Using Incremental Hash operation
 *
 * HWcrypto Outputs the PAYLOAD + HASH RESULT
 * ctx->payload_buffer has the output payload
 * output has the HASH result in case of init/updt/final
 */
platform_result_t platform_hwcrypto_sha1_hmac_incremental( const unsigned char* key, uint32_t keylen, const unsigned char* input, uint32_t ilen, unsigned char output[ SHA1_HASH_SIZE ] );

/**
 * MD5 INIT
 * @Assumption : input_len is multiple of 64
 * @param[in]   ctx             md5_context
 *                              HWcrypto Outputs the PAYLOAD + HASH RESULT
 *                              ctx->payload_buffer should be initialized to point to a buffer big
 *                              enough to contain the input payload
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 *
 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_md5_starts( md5_context_t* ctx, const unsigned char* input, uint32_t ilen );

/**
 * MD5 UPDATE
 * @Assumption : input_len is multiple of 64
 *
 * @param[in]   ctx             md5_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated

 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_md5_update( md5_context_t* ctx, const unsigned char* input, uint32_t ilen );

/**
 * MD5 FINISH
 * @param[in]   ctx             md5_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 */
platform_result_t platform_hwcrypto_md5_finish( md5_context_t* ctx, const unsigned char* input, uint32_t ilen, unsigned char output[ MD5_HASH_SIZE ] );

/**
 * SPU-M (HWCrypto Engine) can only hash HWCRYPTO_MAX_PAYLOAD_SIZE payload at a time.
 * for larger payloads, divide the hash into HASH_INIT, HASH_UPDT, HASH_FINISH.
 * SPU-M User guide Rev0.31 Page 41 Section 3.4.1
 *
 * @param[in]   input           pointer to input data/payload
 * @param[in]   ilen            length of input data/payload
 * @param[out]  output          buffer to store output data
 */
platform_result_t platform_hwcrypto_md5_incremental( const unsigned char* input, uint32_t ilen, unsigned char output[ MD5_HASH_SIZE ] );

/**
 * MD5
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 */
platform_result_t platform_hwcrypto_md5( const unsigned char* input, uint32_t ilen, unsigned char hash_output[ MD5_HASH_SIZE ], unsigned char* payload_buffer );

/**
 * MD5 HMAC INIT
 * Calculates HASH(key XOR ipad)
 * HASH(key XOR ipad || data)
 *
 * @param[in]   ctx             md5_context containing the previous sha result
 *                              HWcrypto Outputs the PAYLOAD + HASH RESULT
 *                              ctx->payload_buffer should be initialized to point to a buffer big
 *                              enough to contain the input payload
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 *
 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_md5_hmac_starts( md5_context_t* ctx, const unsigned char* key, uint32_t keylen );

/**
 * MD5 HMAC UPDATE
 * HASH(key XOR ipad || data)
 * @Assumption : input_len is multiple of 64 */
/*
 * @param[in]   ctx             md5_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated

 * The result of hash is stored in ctx->state
 */
platform_result_t platform_hwcrypto_md5_hmac_update( md5_context_t* ctx, const unsigned char* input, int32_t ilen );

/**
 * MD5 HMAC FINISH
 * @param[in]   ctx             md5_context containing the previous sha result
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 */
platform_result_t platform_hwcrypto_md5_hmac_finish( md5_context_t* ctx, uint8_t* input, int32_t ilen, unsigned char output[ MD5_HASH_SIZE ] );

/**
 * MD5HMAC Authentication
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 */
platform_result_t platform_hwcrypto_md5_hmac( const unsigned char* key, uint32_t keylen, const unsigned char* input, uint32_t ilen, unsigned char output[ MD5_HASH_SIZE ], unsigned char* payload_buffer );

/**
 * MD5HMAC Authentication For lengths > HWCRYPTO_MAX_PAYLOAD_SIZE
 * @param[in]   key             HMAC key
 * @param[in]   keylen          HMAC key length
 * @param[in]   input           input data
 * @param[in]   ilen            size of data to be authenticated
 * @param[out]  output          buffer to hold result of authentication
 *
 * SPU-M does not support MD5 HMAC of > 64K , but it supports MD5 Hash for > 64K data
 * So HMAC is computed by using Hash as described below.
 * HMAC = Hash(key XOR opad || HASH(key XOR ipad || data))
 * (|| -> append, ipad -> 64 byte array of 0x3C, opad -> 64 byte array of 0x5c )
 * InnexHashContext -> key XOR ipad
 * OuterHashContext -> key XOR opad
 *
 * For Details Refer to :
 * APPENDIX B: Summary of Hash Modes and
 * 3.4.1 : Code authentication Using Incremental Hash operation
 *
 * HWcrypto Outputs the PAYLOAD + HASH RESULT
 * ctx->payload_buffer has the output payload
 * output has the HASH result in case of init/updt/final
 */
platform_result_t platform_hwcrypto_md5_hmac_incremental( const unsigned char* key, uint32_t keylen, const unsigned char* input, uint32_t ilen, unsigned char output[ MD5_HASH_SIZE ] );

/**
 * Populate the hw_aes_context_t with key and keysize
 * @param[in]   ctx
 * @param[in]   key
 * @param[in]   keysize_bits
 */
void hw_aes_setkey_enc( hw_aes_context_t* ctx, unsigned char* key, uint32_t keysize_bits );

/**
 * Populate the hw_aes_context_t with key and keysize
 * @param[in]   ctx
 * @param[in]   key
 * @param[in]   keysize_bits
 */
void hw_aes_setkey_dec( hw_aes_context_t* ctx, unsigned char* key, uint32_t keysize_bits );

/**
 * AES CBC Encryption/Decryption
 * @param[in]   ctx         hw_aes_context
 * @param[in]   mode        mode HW_AES_ENCRYPT/HW_AES_DECRYPT
 * @param[in]   length      length of input data
 * @param[in]   iv          aes_iv used for AES
 * @param[in]   input       input data
 * @param[out]  output      output of the AES encryption/decryption
 */
void hw_aes_crypt_cbc( hw_aes_context_t* ctx, hw_aes_mode_type_t mode, uint32_t length, unsigned char iv[ 16 ], const unsigned char* input, unsigned char* output );

/**
 * AES ECB Encryption/Decryption
 * @param[in]   ctx         hw_aes_context
 * @param[in]   mode        mode HW_AES_ENCRYPT/HW_AES_DECRYPT
 * @param[in]   length      length of input data
 * @param[in]   iv          aes_iv used for AES
 * @param[in]   input       input data of size AES_BLOCK_SIZE
 * @param[out]  output      output of the AES encryption/decryption
 */
void hw_aes_crypt_ecb( hw_aes_context_t* ctx, hw_aes_mode_type_t mode, const unsigned char input[ 16 ], unsigned char output[ 16 ] );

/**
 * AES CFB Encryption/Decryption
 * @param[in]   ctx         hw_aes_context
 * @param[in]   mode        mode HW_AES_ENCRYPT/HW_AES_DECRYPT
 * @param[in]   length      length of input data
 * @param[in]   iv          aes_iv used for AES
 * @param[in]   input       input data
 * @param[out]  output      output of the AES encryption/decryption
 */
void hw_aes_crypt_cfb( hw_aes_context_t* ctx, hw_aes_mode_type_t mode, uint32_t length, uint32_t* iv_off, unsigned char iv[ 16 ], const unsigned char* input, unsigned char* output );

/**
 * AES CTR Encryption/Decryption
 * @param[in]   ctx         hw_aes_context
 * @param[in]   mode        mode HW_AES_ENCRYPT/HW_AES_DECRYPT
 * @param[in]   length      length of input data
 * @param[in]   iv          aes_iv used for AES
 * @param[in]   input       input data
 * @param[out]  output      output of the AES encryption/decryption
 */
void hw_aes_crypt_ctr( hw_aes_context_t* ctx, hw_aes_mode_type_t mode, uint32_t length, unsigned char iv[ 16 ], const unsigned char* input, unsigned char* output );

/**
 * DES/DES3 56 bit Key encryption/decryption
 * K1=K2=K3
 * @param[in]   ctx
 * @param[in]   key
 * */
void hw_des_setkey( hw_des_context_t* ctx, unsigned char* key );

/**
 * DES3 112 bit Key encryption/decryption
 * K3 = K1
 * @param[in]   ctx
 * @param[in]   key
 * */
void hw_des3_set2key( hw_des_context_t* ctx, unsigned char* key );

/**
 * DES3 168 bit Key encryption/decryption
 * @param[in]   ctx
 * @param[in]   key
 * */
void hw_des3_set3key( hw_des_context_t* ctx, unsigned char* key );

/**
 * DES ECB Encryption/Decryption
 * @param[in]   ctx         hw_aes_context
 * @param[in]   mode        mode HW_DES_ENCRYPT/HW_DES_DECRYPT
 * @param[in]   input       input data of size DES_BLOCK_SIZE
 * @param[out]  output      buffer to store result of DES encryption/decryption
 */
void hw_des_crypt_ecb( hw_des_context_t* ctx, hw_des_mode_type_t mode, const unsigned char input[ 8 ], unsigned char output[ 8 ] );

/**
 * DES CBC Encryption/Decryption
 * @param[in]   ctx         hw_aes_context
 * @param[in]   mode        mode HW_DES_ENCRYPT/HW_DES_DECRYPT
 * @param[in]   length      length of input data
 * @param[in]   iv          iv used for DES
 * @param[in]   input       input data
 * @param[out]  output      buffer to store result of DES encryption/decryption
 */
void hw_des_crypt_cbc( hw_des_context_t* ctx, hw_des_mode_type_t mode, uint32_t length, unsigned char iv[ 8 ], const unsigned char* input, unsigned char* output );

/**
 * DES3 ECB Encryption/Decryption
 * @param[in]   ctx         hw_aes_context
 * @param[in]   mode        mode HW_DES_ENCRYPT/HW_DES_DECRYPT
 * @param[in]   input       input data of size DES_BLOCK_SIZE
 * @param[out]  output      buffer to store result of DES encryption/decryption
 */
void hw_des3_crypt_ecb( hw_des_context_t* ctx, hw_des_mode_type_t mode, const unsigned char input[ 8 ], unsigned char output[ 8 ] );

/**
 * DES3 CBC Encryption/Decryption
 * @param[in]   ctx         hw_aes_context
 * @param[in]   mode        mode HW_DES_ENCRYPT/HW_DES_DECRYPT
 * @param[in]   length      length of input data
 * @param[in]   iv          iv used for DES
 * @param[in]   input       input data
 * @param[out]  output      buffer to store result of DES encryption/decryption
 */
void hw_des3_crypt_cbc( hw_des_context_t* ctx, hw_des_mode_type_t mode, uint32_t length, unsigned char iv[ 8 ], const unsigned char* input, unsigned char* output );

#endif  /*_CRYPTO_STRUCTURES_H */

