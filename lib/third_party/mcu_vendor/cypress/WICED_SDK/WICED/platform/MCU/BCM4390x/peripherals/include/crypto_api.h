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
#ifndef _CRYPTO_API_H
#define _CRYPTO_API_H

#include "typedefs.h"
#include <sbhnddma.h>
#include <platform_cache_def.h>
#include "platform_constants.h"
#include "wiced_utilities.h"
#include "hw_crypto_api_external.h"

/******************************************************
 *                      Macros
 ******************************************************/

/*****************************************************
 *                    Constants
 ******************************************************/
#define DES_IV_LEN                      8
#define DES_BLOCK_SZ                    8
#define AES_IV_LEN                      16
#define AES_BLOCK_SZ                    16
#define AES128_KEY_LEN                  16
#define HMAC256_KEY_LEN                 32
#define HMAC224_KEY_LEN                 28
#define HMAC160_KEY_LEN                 20
#define HMAC128_KEY_LEN                 16
#define HMAC_BLOCK_SIZE                 64
#define HMAC_IPAD                       0x36
#define HMAC_OPAD                       0x5C
#define SHA_BLOCK_SIZE                  64 /* 64bytes = 512 bits */
#define SHA_BLOCK_SIZE_MASK             ( SHA_BLOCK_SIZE - 1 ) /* SHA1/SHA2 Input data should be at least 64bytes aligned */
#define SHA_PADDING_MAX                 ( SHA_BLOCK_SIZE - 1 )
#define MD5_BLOCK_SIZE                  64 /* 64bytes = 512 bits */
#define MD5_BLOCK_SIZE_MASK             ( MD5_BLOCK_SIZE - 1 ) /* SHA1/SHA2 Input data should be at least 64bytes aligned */
#define MD5_PADDING_MAX                 ( MD5_BLOCK_SIZE - 1 )

#define HWCRYPTO_MAX_PAYLOAD_SIZE       PLATFORM_L1_CACHE_ROUND_UP( 63*1024 )

/******************************************************
 *                   Enumerations
 ******************************************************/
typedef enum
{
    ENCR_AUTH = 0,
    AUTH_ENCR = 1
} sctx_order_t;
typedef enum
{
    OUTBOUND    = 0,
    INBOUND     = 1
} sctx_inbound_t;
typedef enum
{
    CRYPT_NULL  = 0,
    RC4         = 1,
    DES         = 2,
    _3DES       = 3,
    AES         = 4
} sctx_crypt_algo_t;

typedef enum
{
    ECB = 0,
    CBC = 1,
    OFB = 2,
    CFB = 3,
    CTR = 4,
    CCM = 5,
    GCM = 6,
    XTS = 7
} sctx_crypt_mode_t;

typedef enum
{
    NULL_   = 0,
    MD5     = 1,
    SHA1    = 2,
    SHA224  = 3,
    SHA256  = 4,
    AES_H   = 5,
    FHMAC   = 6
} sctx_hash_algo_t;

typedef enum
{
    HASH=0,
    CTXT=1,
    HMAC=2,
    CCM_H=5,
    GCM_H=6
} sctx_hash_mode_t;

typedef enum
{
    HASH_FULL   = 0,
    HASH_INIT   = 1,
    HASH_UPDT   = 2,
    HASH_FINAL  = 3
} sctx_hash_optype_t;

typedef enum
{
    HWCRYPTO_ENCR_ALG_NONE = 0,
    HWCRYPTO_ENCR_ALG_AES_128,
    HWCRYPTO_ENCR_ALG_AES_192,
    HWCRYPTO_ENCR_ALG_AES_256,
    HWCRYPTO_ENCR_ALG_DES,
    HWCRYPTO_ENCR_ALG_3DES,
    HWCRYPTO_ENCR_ALG_RC4_INIT,
    HWCRYPTO_ENCR_ALG_RC4_UPDT
} hwcrypto_encr_alg_t;

typedef enum
{
    HWCRYPTO_ENCR_MODE_NONE = 0,
    HWCRYPTO_ENCR_MODE_CBC,
    HWCRYPTO_ENCR_MODE_ECB ,
    HWCRYPTO_ENCR_MODE_CTR,
    HWCRYPTO_ENCR_MODE_CCM = 5,
    HWCRYPTO_ENCR_MODE_CMAC,
    HWCRYPTO_ENCR_MODE_OFB,
    HWCRYPTO_ENCR_MODE_CFB,
    HWCRYPTO_ENCR_MODE_GCM,
    HWCRYPTO_ENCR_MODE_XTS
} hwcrypto_encr_mode_t;

typedef enum
{
      HWCRYPTO_AUTH_ALG_NULL = 0,
      HWCRYPTO_AUTH_ALG_MD5 ,
      HWCRYPTO_AUTH_ALG_SHA1,
      HWCRYPTO_AUTH_ALG_SHA224,
      HWCRYPTO_AUTH_ALG_SHA256,
      HWCRYPTO_AUTH_ALG_AES
} hwcrypto_auth_alg_t;

typedef enum
{
      HWCRYPTO_AUTH_MODE_HASH = 0,
      HWCRYPTO_AUTH_MODE_CTXT,
      HWCRYPTO_AUTH_MODE_HMAC,
      HWCRYPTO_AUTH_MODE_FHMAC,
      HWCRYPTO_AUTH_MODE_CCM,
      HWCRYPTO_AUTH_MODE_GCM,
      HWCRYPTO_AUTH_MODE_XCBCMAC,
      HWCRYPTO_AUTH_MODE_CMAC
} hwcrypto_auth_mode_t;

typedef enum
{
      HWCRYPTO_AUTH_OPTYPE_FULL = 0,
      HWCRYPTO_AUTH_OPTYPE_INIT,
      HWCRYPTO_AUTH_OPTYPE_UPDATE,
      HWCRYPTO_AUTH_OPTYPE_FINAL,
      HWCRYPTO_AUTH_OPTYPE_HMAC_HASH
} hwcrypto_auth_optype_t;

typedef enum
{
    HWCRYPTO_CIPHER_MODE_NULL = 0,
    HWCRYPTO_CIPHER_MODE_ENCRYPT,
    HWCRYPTO_CIPHER_MODE_DECRYPT,
    HWCRYPTO_CIPHER_MODE_AUTHONLY
} hwcrypto_cipher_mode_t;

typedef enum
{
    HWCRYPTO_CIPHER_ORDER_NULL = 0,
    HWCRYPTO_CIPHER_ORDER_AUTH_CRYPT,
    HWCRYPTO_CIPHER_ORDER_CRYPT_AUTH
} hwcrypto_cipher_order_t;

typedef enum
{
    HWCRYPTO_CIPHER_OPTYPE_RC4_OPTYPE_INIT = 0,
    HWCRYPTO_CIPHER_OPTYPE_RC4_OPTYPE_UPDT,
    HWCRYPTO_CIPHER_OPTYPE_DES_OPTYPE_K56,
    HWCRYPTO_CIPHER_OPTYPE_3DES_OPTYPE_K168EDE,
    HWCRYPTO_CIPHER_OPTYPE_AES_OPTYPE_K128,
    HWCRYPTO_CIPHER_OPTYPE_AES_OPTYPE_K192,
    HWCRYPTO_CIPHER_OPTYPE_AES_OPTYPE_K256
} hwcrypto_cipher_optype_t;

typedef enum
{
    HWCRYPTO_HASHMODE_HASH_HASH = 0,
    HWCRYPTO_HASHMODE_HASH_CTXT,
    HWCRYPTO_HASHMODE_HASH_HMAC,
    HWCRYPTO_HASHMODE_HASH_FHMAC,
    HWCRYPTO_HASHMODE_AES_HASH_XCBC_MAC,
    HWCRYPTO_HASHMODE_AES_HASH_CMAC,
    HWCRYPTO_HASHMODE_AES_HASH_CCM,
    HWCRYPTO_HASHMODE_AES_HASH_GCM
}  hwcrypto_hash_mode_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/



typedef union
{
    uint32_t raw;
    struct
    {
        unsigned int reserved3      : 16;   /* [ 15:0 ] */
        unsigned int opcode         : 8;    /* [ 23:16] */
        unsigned int supdt_present  : 1;    /* [ 24:1 ] */
        unsigned int reserved2      : 1;    /* [ 25:1 ] */
        unsigned int hash_present   : 1;    /* [ 26:1 ] */
        unsigned int bd_present     : 1;    /* [ 27:1 ] */
        unsigned int mfm_present    : 1;    /* [ 28:1 ] */
        unsigned int bdesc_present  : 1;    /* [ 29:1 ] */
        unsigned int reserved1      : 1;    /* [ 30:1 ] */
        unsigned int sctx_present   : 1;    /* [ 31:1 ] */
    } bits;
} cryptofield_message_header_t;

typedef union
{
    uint32_t raw;
    struct
    {
        unsigned int sctx_size      : 8;    /* [ 7:0   ]  */
        unsigned int reserved       : 22;   /* [ 29:8  ]  */
        unsigned int sctx_type      : 2;    /* [ 31:30 ]  */
    } bits;
} cryptofield_sctx1_header_t;

typedef union
{
    uint32_t raw;
    struct
    {
        unsigned int updt_ofst      : 8;    /* [ 7:0   ]  */
        unsigned int hash_optype    : 2;    /* [ 9:8   ]  */
        unsigned int hash_mode      : 3;    /* [ 12:10 ]  */
        unsigned int hash_algo      : 3;    /* [ 15:13 ]  */
        unsigned int crypt_optype   : 2;    /* [ 17:16 ]  */
        unsigned int crypt_mode     : 3;    /* [ 20:18 ]  */
        unsigned int crypt_algo     : 3;    /* [ 23:21 ]  */
        unsigned int reserved       : 6;    /* [ 29:24 ]  */
        unsigned int order          : 1;    /* [ 30    ]  */
        unsigned int inbound        : 1;    /* [ 31    ]  */
    } bits;
} cryptofield_sctx2_header_t;

typedef union
{
    uint32_t raw;
    struct
    {
        unsigned int exp_iv_size    : 3;    /* [ 2:0   ]  */
        unsigned int iv_ov_ofst     : 2;    /* [ 4:3   ]  */
        unsigned int iv_flags       : 3;    /* [ 7:5   ]  */
        unsigned int icv_size       : 4;    /* [ 11:8  ]  */
        unsigned int icv_flags      : 2;    /* [ 13:12 ]  */
        unsigned int reserved1      : 6;    /* [ 19:14 ]  */
        unsigned int key_handle     : 9;    /* [ 28:20 ]  */
        unsigned int reserved2      : 2;    /* [ 30:29 ]  */
        unsigned int protected_key  : 1;    /* [ 31    ]  */
    } bits;
} cryptofield_sctx3_header_t;

typedef struct
{
    unsigned int length_crypto      : 16;    /* [ 15:0  ]  */
    unsigned int offset_crypto      : 16;    /* [ 31:16 ]  */
} cryptofield_bdesc_crypto_t;

typedef struct
{
    unsigned int length_mac         : 16;    /* [ 15:0  ]  */
    unsigned int offset_mac         : 16;    /* [ 31:16 ]  */
} cryptofield_bdesc_mac_t;

typedef struct
{
    unsigned int offset_iv          : 16;    /* [ 15:0  ]  */
    unsigned int offset_icv         : 16;    /* [ 31:16 ]  */
} cryptofield_bdesc_iv_t;

typedef struct
{
    cryptofield_bdesc_iv_t          iv;
    cryptofield_bdesc_crypto_t      crypto;
    cryptofield_bdesc_mac_t         mac;
} cryptofield_bdesc_t;

typedef struct
{
    unsigned int prev_length        : 16;    /* [ 15:0  ]  */
    unsigned int size               : 16;    /* [ 31:16 ]  */
} cryptofield_bd_t;

typedef struct
{
    cryptofield_message_header_t    message_header;
    uint32_t                        extended_header;
    cryptofield_sctx1_header_t      sctx1;
    cryptofield_sctx2_header_t      sctx2;
    cryptofield_sctx3_header_t      sctx3;
    cryptofield_bdesc_t             bdesc;
    cryptofield_bd_t                bd;
} spum_message_fields_t;

typedef struct crypto_cmd
{
    uint8_t*  source;          /* input buffer */
    uint8_t*  output;          /* output buffer */
    uint8_t*  hash_output;     /* buffer to store hash output
                                   (If NULL, hash output is stored at the end of output payload)*/
    uint8_t*  hash_key;        /* HMAC Key / HASH State (For incremental Hash operations) */
    uint32_t  hash_key_len;    /* HMAC Key / HASH State length */
    uint8_t*  crypt_key;       /* Crypt Key */
    uint32_t  crypt_key_len;   /* Crypt Key length */
    uint8_t*  crypt_iv;        /* Crypt IV */
    uint32_t  crypt_iv_len;    /* Crypt IV length */
    spum_message_fields_t msg; /* SPU-M (HWcrypto) message structure */

} crypto_cmd_t;

typedef struct rommed_crypto_cmd
{
    uint8_t   *source;
    uint32_t  crypt_size;
    uint32_t  hash_size;
    uint8_t   *hash_key;
    uint32_t  hash_key_len;
    uint8_t   *crypt_key;
    uint32_t  crypt_key_len;
    uint8_t   *crypt_iv;
    uint32_t  crypt_iv_len;
    uint8_t   *output;
    uint8_t   *hash_output;
    uint8_t   crypt_algo;
    uint8_t   crypt_mode;
    uint8_t   crypt_optype;
    uint8_t   hash_algo;
    uint8_t   hash_mode;
    uint8_t   hash_optype;
    uint8_t   order;
    uint8_t   inbound;
    uint8_t   icv_size;
    uint32_t  prev_len;

} rommed_crypto_cmd_t;

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
/**
 * Creates a SPU-M (HWCrypto) Message and Transfers it to SPU-M
 * using M2MDma.
 * Results of the HWCrypto Operation are stored in the buffers
 * specified in cmd.output
 * @param[in] cmd   :   HWCrypto command
 *
 * @return @ref platform_result_t
 *
 */
platform_result_t platform_hwcrypto_execute(crypto_cmd_t cmd);
#endif  /*_CRYPTO_API_H */

