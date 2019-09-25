/**
* MIT License
*
* Copyright (c) 2018 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file 
*
* \brief   This file defines APIs, types and data structures for Crypto Wrapper.
*
* \ingroup  grCryptoLibWrapper
* @{
*/
#ifndef _PAL_CRYPT_H_
#define _PAL_CRYPT_H_

#include <stdint.h>
#include "optiga/common/Datatypes.h"

///Requested operation completed without any error
#define CRYPTO_LIB_OK                               OPTIGA_LIB_SUCCESS

///Null parameter(s)
#define CRYPTO_LIB_NULL_PARAM                       0x80003001

///Certificate parse failure
#define CRYPTO_LIB_CERT_PARSE_FAIL                  (CRYPTO_LIB_NULL_PARAM + 1)

///Signature Verification failure
#define CRYPTO_LIB_VERIFY_SIGN_FAIL                 (CRYPTO_LIB_NULL_PARAM + 2)

///SHA256 generation failure
#define CRYPTO_LIB_SHA256_FAIL                      (CRYPTO_LIB_NULL_PARAM + 3)

///Length of input is zero
#define CRYPTO_LIB_LENZERO_ERROR                    (CRYPTO_LIB_NULL_PARAM + 4)

///Length of Parameters are zero
#define CRYPTO_LIB_LENMISMATCH_ERROR                (CRYPTO_LIB_NULL_PARAM + 5)

///Memory allocation failure
#define CRYPTO_LIB_MEMORY_FAIL                      (CRYPTO_LIB_NULL_PARAM + 6)

///Insufficient memory
#define CRYPTO_LIB_INSUFFICIENT_MEMORY              (CRYPTO_LIB_NULL_PARAM + 7)

///Generic error condition
#define CRYPTO_LIB_ERROR                            0xF1743903

/****************************************************************************
 *
 * Common data structure used across all functions.
 *
 ****************************************************************************/

/**
 * \brief Structure which holds parsed certificate data.
 */
typedef struct sCertificate_d
{
    ///Public Key
	const uint8_t* p_pubkey;
	uint16_t pubkey_size;

	///Certificate Signature
	const uint8_t* p_signature;
	uint16_t signature_size;

    /// Issuer ID
    uint8_t*  p_issuer_id;
    uint16_t issuer_id_size;

    /// Subject ID
    uint8_t*  p_subj_id;
    uint16_t subj_id_size;

    ///Certificate Data start
	const uint8_t* p_cert_data;
	uint16_t cert_data_size;

}sCertificate_d;


/****************************************************************************
 *
 * Definitions for Crypto Libray APIs.
 *
 ****************************************************************************/
/**
 * \brief Generate a SHA256 hash on the message.
 */
optiga_lib_status_t pal_crypt_generate_sha256(uint8_t* p_input, uint16_t inlen, uint8_t* p_digest);

/**
* Extract public key from the certificate using crypto library
*
*\param[in] p_cert					Pointer to the certificate buffer
*\param[in] cert_size				Certificate buffer size
*\param[in] p_pubkey				Pointer to the buffer where to store a public key
*\param[in][out] p_pubkey_size		Variable where to store Public Key
*
*\retval  #CRYPTO_LIB_OK
*\retval  #CRYPTO_LIB_ERROR
*\retval  #CRYPTO_LIB_CERT_PARSE_FAIL
*\retval  #CRYPTO_LIB_NULL_PARAM
*\retval  #CRYPTO_LIB_LENZERO_ERROR
*/
optiga_lib_status_t pal_crypt_get_public_key(const uint8_t* p_cert, uint16_t cert_size,
		                                           uint8_t* p_pubkey, uint16_t* p_pubkey_size);

/**
* \brief Parses raw X509 v3 certificate into a custom defined certificate structure.
*
*\param[in] p_cacert				Pointer to the CA certificate buffer. Should be DER encoded binary certificate. Should start from 0x30
*\param[in] cacert_size				CA Certificate buffer size
*\param[in] p_cert					Pointer to the certificate buffer. This certificate is verified against the given CA. Should start from 0x30
*\param[in][out] cert_size			Certificate buffer size
*
*\retval  #CRYPTO_LIB_OK
*\retval  #CRYPTO_LIB_ERROR
*\retval  #CRYPTO_LIB_CERT_PARSE_FAIL
*\retval  #CRYPTO_LIB_NULL_PARAM
*\retval  #CRYPTO_LIB_LENZERO_ERROR
 */
optiga_lib_status_t  pal_crypt_verify_certificate(const uint8_t* p_cacert, uint16_t cacert_size,
                                                  const uint8_t* p_cert, uint16_t cert_size);

/**
* Verifies the signature using the given public key.
*
*
*\param[in] p_pubkey				Pointer to the public key buffer used to verify the signature
*\param[in] pubkey_size				Public key buffer size
*\param[in] p_signature				Pointer to the signature buffer
*\param[in] signature_size			Signature buffer size
*\param[in] p_digest				Pointer to the digest buffer (Should be sha256 hash)
*\param[in] digest_size				Digest buffer size
*
*\retval  #CRYPTO_LIB_OK
*\retval  #CRYPTO_LIB_ERROR
*\retval  #CRYPTO_LIB_SHA256_FAIL
*\retval  #CRYPTO_LIB_VERIFY_SIGN_FAIL
*\retval  #CRYPTO_LIB_NULL_PARAM
*\retval  #CRYPTO_LIB_LENZERO_ERROR
*/
optiga_lib_status_t  pal_crypt_verify_signature(const uint8_t* p_pubkey, uint16_t pubkey_size,
		                                        const uint8_t* p_signature, uint16_t signature_size,
									                  uint8_t* p_digest, uint16_t digest_size);
/**
* Initislise underlying crypto library if required
*
*\retval  #CRYPTO_LIB_OK
*\retval  #CRYPTO_LIB_NULL_PARAM
*/
optiga_lib_status_t pal_crypt_init(void);

/**
* Generates random data bytes of requested length. <br>
* - If the requested length is less than 32 bytes, random data bytes of 32 bytes,<br>
*   will be generated and the requested number of bytes will be returned. <br>
* - The crypto random number generator is initialized and seeded only once. <br>
* - For subsequent seeding,the first 32 bytes of the generated random data bytes will be fed back as seed. <br>
*
*\param[in] random_size      Number of random bytes requested
*\param[in,out] p_random      Pointer to random buffer
*
*\retval  #CRYPTO_LIB_OK
*\retval  #CRYPTO_LIB_NULL_PARAM
*\retval  #CRYPTO_LIB_LENZERO_ERROR
*\retval  #CMD_DEV_ERROR
*/
optiga_lib_status_t  pal_crypt_random(uint16_t random_size, uint8_t* p_random);


#endif //PAL_CRYPT






























