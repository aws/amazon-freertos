/**
 * \file
 * \brief Common defines for CryptoAuthLib software crypto wrappers.
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

#ifndef ATCA_CRYPTO_SW_H
#define ATCA_CRYPTO_SW_H

#include <stdint.h>
#include <stdlib.h>

#include "atca_config.h"
#include "atca_status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ATCA_SHA1_DIGEST_SIZE       (20)
#define ATCA_SHA2_256_DIGEST_SIZE   (32)
#define ATCA_SHA2_256_BLOCK_SIZE    (64)

#if defined(ATCA_MBEDTLS)
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#ifndef MBEDTLS_CMAC_C
#define MBEDTLS_CMAC_C
#endif

#include <mbedtls/cipher.h>
#include <mbedtls/md.h>
#include <mbedtls/pk.h>
typedef mbedtls_cipher_context_t atcac_aes_cmac_ctx;
typedef mbedtls_md_context_t atcac_hmac_sha256_ctx;
typedef mbedtls_cipher_context_t atcac_aes_gcm_ctx;
typedef mbedtls_md_context_t atcac_sha1_ctx;
typedef mbedtls_md_context_t atcac_sha2_256_ctx;
typedef mbedtls_pk_context atcac_pk_ctx;

#elif defined(ATCA_OPENSSL)
typedef struct
{
    void* ptr;
} atca_evp_ctx;
typedef atca_evp_ctx atcac_aes_gcm_ctx;
typedef atca_evp_ctx atcac_sha1_ctx;
typedef atca_evp_ctx atcac_sha2_256_ctx;
typedef atca_evp_ctx atcac_aes_cmac_ctx;
typedef atca_evp_ctx atcac_hmac_sha256_ctx;
typedef atca_evp_ctx atcac_pk_ctx;
#elif defined(ATCA_WOLFSSL)
#include "wolfssl/wolfcrypt/types.h"
#ifndef WOLFSSL_CMAC
#define WOLFSSL_CMAC
#endif
#ifndef HAVE_AESGCM
#define HAVE_AESGCM
#endif
#include "wolfssl/wolfcrypt/aes.h"
#include "wolfssl/wolfcrypt/cmac.h"
#include "wolfssl/wolfcrypt/hmac.h"
#include "wolfssl/wolfcrypt/sha.h"
#include "wolfssl/wolfcrypt/sha256.h"
#include "wolfssl/wolfcrypt/error-crypt.h"
#include "wolfssl/wolfcrypt/asn_public.h"
#include "wolfssl/wolfcrypt/asn.h"
#include "wolfssl/wolfcrypt/ecc.h"

typedef struct
{
    Aes      aes;
    uint8_t  iv[AES_BLOCK_SIZE];
    uint16_t iv_len;
} atcac_aes_gcm_ctx;

typedef struct
{
    void* ptr;
} atca_wc_ctx;

typedef wc_Sha atcac_sha1_ctx;
typedef wc_Sha256 atcac_sha2_256_ctx;
typedef Cmac atcac_aes_cmac_ctx;
typedef Hmac atcac_hmac_sha256_ctx;
typedef atca_wc_ctx atcac_pk_ctx;

#else
#ifndef ATCA_ENABLE_SHA1_IMPL
#define ATCA_ENABLE_SHA1_IMPL       1
#endif

#ifndef ATCA_ENABLE_SHA256_IMPL
#define ATCA_ENABLE_SHA256_IMPL     1
#endif


typedef struct
{
    uint32_t pad[32]; //!< Filler value to make sure the actual implementation has enough room to store its context. uint32_t is used to remove some alignment warnings.
} atcac_sha1_ctx;

typedef struct
{
    uint32_t pad[48]; //!< Filler value to make sure the actual implementation has enough room to store its context. uint32_t is used to remove some alignment warnings.
} atcac_sha2_256_ctx;

typedef struct
{
    atcac_sha2_256_ctx sha256_ctx;
    uint8_t            ipad[ATCA_SHA2_256_BLOCK_SIZE];
    uint8_t            opad[ATCA_SHA2_256_BLOCK_SIZE];
} atcac_hmac_sha256_ctx;
#endif

#if defined(ATCA_MBEDTLS) || defined(ATCA_OPENSSL) || defined(ATCA_WOLFSSL)
ATCA_STATUS atcac_aes_gcm_encrypt_start(atcac_aes_gcm_ctx* ctx, const uint8_t* key, const uint8_t key_len, const uint8_t* iv, const uint8_t iv_len);
ATCA_STATUS atcac_aes_gcm_decrypt_start(atcac_aes_gcm_ctx* ctx, const uint8_t* key, const uint8_t key_len, const uint8_t* iv, const uint8_t iv_len);

ATCA_STATUS atcac_aes_cmac_init(atcac_aes_cmac_ctx* ctx, const uint8_t* key, const uint8_t key_len);
ATCA_STATUS atcac_aes_cmac_update(atcac_aes_cmac_ctx* ctx, const uint8_t* data, const size_t data_size);
ATCA_STATUS atcac_aes_cmac_finish(atcac_aes_cmac_ctx* ctx, uint8_t* cmac, size_t* cmac_size);

ATCA_STATUS atcac_pk_init(atcac_pk_ctx* ctx, uint8_t* buf, size_t buflen, uint8_t key_type, bool pubkey);
ATCA_STATUS atcac_pk_init_pem(atcac_pk_ctx* ctx, uint8_t* buf, size_t buflen, bool pubkey);
ATCA_STATUS atcac_pk_free(atcac_pk_ctx* ctx);
ATCA_STATUS atcac_pk_public(atcac_pk_ctx* ctx, uint8_t* buf, size_t* buflen);
ATCA_STATUS atcac_pk_sign(atcac_pk_ctx* ctx, uint8_t* digest, size_t dig_len, uint8_t* signature, size_t* sig_len);
ATCA_STATUS atcac_pk_verify(atcac_pk_ctx* ctx, uint8_t* digest, size_t dig_len, uint8_t* signature, size_t sig_len);
ATCA_STATUS atcac_pk_derive(atcac_pk_ctx* private_ctx, atcac_pk_ctx* public_ctx, uint8_t* buf, size_t* buflen);
#endif

#if defined(ATCA_MBEDTLS) || defined(ATCA_OPENSSL)
ATCA_STATUS atcac_aes_gcm_aad_update(atcac_aes_gcm_ctx* ctx, const uint8_t* aad, const size_t aad_len);

ATCA_STATUS atcac_aes_gcm_encrypt_update(atcac_aes_gcm_ctx* ctx, const uint8_t* plaintext, const size_t pt_len, uint8_t* ciphertext, size_t* ct_len);
ATCA_STATUS atcac_aes_gcm_encrypt_finish(atcac_aes_gcm_ctx* ctx, uint8_t* tag, size_t tag_len);

ATCA_STATUS atcac_aes_gcm_decrypt_update(atcac_aes_gcm_ctx* ctx, const uint8_t* ciphertext, const size_t ct_len, uint8_t* plaintext, size_t* pt_len);
ATCA_STATUS atcac_aes_gcm_decrypt_finish(atcac_aes_gcm_ctx* ctx, const uint8_t* tag, size_t tag_len, bool* is_verified);

#elif defined(ATCA_WOLFSSL)
ATCA_STATUS atcac_aes_gcm_encrypt(atcac_aes_gcm_ctx* ctx, const uint8_t* plaintext, const size_t pt_len, uint8_t* ciphertext, uint8_t* tag,
                                  size_t tag_len, const uint8_t* aad, const size_t aad_len);


ATCA_STATUS atcac_aes_gcm_decrypt(atcac_aes_gcm_ctx* ctx, const uint8_t* ciphertext, const size_t ct_len, uint8_t* plaintext, const uint8_t* tag,
                                  size_t tag_len, const uint8_t* aad, const size_t aad_len, bool* is_verified);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ATCA_CRYPTO_SW_H */