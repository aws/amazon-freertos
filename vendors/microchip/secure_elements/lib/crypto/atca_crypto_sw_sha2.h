/**
 * \file
 * \brief  Wrapper API for software SHA 256 routines
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

#ifndef ATCA_CRYPTO_SW_SHA2_H
#define ATCA_CRYPTO_SW_SHA2_H

#include "atca_crypto_sw.h"
#include <stddef.h>
#include <stdint.h>

/** \defgroup atcac_ Software crypto methods (atcac_)
 *
 * \brief
 * These methods provide a software implementation of various crypto
 * algorithms
 *
   @{ */

#ifdef __cplusplus
extern "C" {
#endif

int atcac_sw_sha2_256_init(atcac_sha2_256_ctx* ctx);
int atcac_sw_sha2_256_update(atcac_sha2_256_ctx* ctx, const uint8_t* data, size_t data_size);
int atcac_sw_sha2_256_finish(atcac_sha2_256_ctx * ctx, uint8_t digest[ATCA_SHA2_256_DIGEST_SIZE]);
int atcac_sw_sha2_256(const uint8_t * data, size_t data_size, uint8_t digest[ATCA_SHA2_256_DIGEST_SIZE]);

ATCA_STATUS atcac_sha256_hmac_init(atcac_hmac_sha256_ctx* ctx, const uint8_t* key, const uint8_t key_len);
ATCA_STATUS atcac_sha256_hmac_update(atcac_hmac_sha256_ctx* ctx, const uint8_t* data, size_t data_size);
ATCA_STATUS atcac_sha256_hmac_finish(atcac_hmac_sha256_ctx* ctx, uint8_t* digest, size_t* digest_len);
ATCA_STATUS atcac_sha256_hmac_counter(atcac_hmac_sha256_ctx* ctx, uint8_t* label, size_t label_len, uint8_t* data, size_t data_len, uint8_t* digest, size_t diglen);

#ifdef __cplusplus
}
#endif

/** @} */
#endif