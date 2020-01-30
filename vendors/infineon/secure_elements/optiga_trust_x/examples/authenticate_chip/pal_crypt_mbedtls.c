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
* \file pal_crypt_mbedtls.c
*
* \brief   This file provides pal crypo interface to the crypto library
*
*
* \ingroup
* @{
*/

#include "optiga/pal/pal_os_timer.h"
#include "optiga/common/MemoryMgmt.h"
#include "optiga/optiga_crypt.h"

#include "ecdsa_utils.h"
#include "pal_crypt.h"

/// @cond hidden

//lint --e{123,617,537} suppress "Suppress ctype.h in Keil + Warning mpi_class.h is both a module and an include file + Repeated include"
#include "mbedtls/ecp.h"
#include "mbedtls/sha256.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/x509.h"
#include "mbedtls/x509_crt.h"

// MAximum size of the ssignature (for P256 0x40)
#define LENGTH_MAX_SIGNATURE				0x40

mbedtls_entropy_context 					entropy;
mbedtls_ctr_drbg_context 					ctr_drbg;


/**
 * \brief Verifies the ECC signature using the given public key.
 */
static int32_t __verify_ecc_signature(const uint8_t* p_pubkey, uint16_t pubkey_size,
                                      const uint8_t* p_signature, uint16_t signature_size,
                                            uint8_t* p_digest, uint16_t digest_size)
{
    int32_t  status = (int32_t)CRYPTO_LIB_VERIFY_SIGN_FAIL;
    uint8_t   signature_rs[LENGTH_MAX_SIGNATURE];
    size_t    signature_rs_size = LENGTH_MAX_SIGNATURE;
    const uint8_t* p_pk = p_pubkey;

    mbedtls_ecp_group grp;
    // Public Key
    mbedtls_ecp_point Q;
    mbedtls_mpi r;
    mbedtls_mpi s;

    mbedtls_ecp_point_init( &Q );
    mbedtls_mpi_init( &r );
    mbedtls_mpi_init( &s );
    mbedtls_ecp_group_init( &grp );

    mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);

    do
    {
        if((NULL == p_pubkey)||(NULL == p_digest)||(NULL == p_signature))
        {
            status = (int32_t)CRYPTO_LIB_NULL_PARAM;
            break;
        }

        //Import the public key
        mbedtls_ecp_point_read_binary(&grp, &Q, p_pk, pubkey_size);

        //Import the signature
        asn1_to_ecdsa_rs(p_signature, signature_size, signature_rs, &signature_rs_size);
        mbedtls_mpi_read_binary(&r, signature_rs, LENGTH_MAX_SIGNATURE/2);
        mbedtls_mpi_read_binary(&s, signature_rs + LENGTH_MAX_SIGNATURE/2, LENGTH_MAX_SIGNATURE/2);

        //Verify generated hash with given public key
        status = mbedtls_ecdsa_verify(&grp, p_digest, digest_size, &Q, &r, &s);

        if ( MBEDTLS_ERR_ECP_VERIFY_FAILED == status)
        {
            status = (int32_t)CRYPTO_LIB_VERIFY_SIGN_FAIL;
            break;
        }

        status = CRYPTO_LIB_OK;
    }while(FALSE);

    return status;
}


optiga_lib_status_t pal_crypt_generate_sha256(uint8_t* p_input, uint16_t inlen, uint8_t* p_digest)
{
    mbedtls_sha256_ret(p_input, inlen, p_digest, 0);
    return CRYPTO_LIB_OK;
}

optiga_lib_status_t  pal_crypt_verify_certificate(const uint8_t* p_cacert, uint16_t cacert_size,
		                                            const uint8_t* p_cert, uint16_t cert_size)
{
    int32_t status  = (int32_t)CRYPTO_LIB_ERROR;
    int32_t ret;
    mbedtls_x509_crt mbedtls_cert;
    mbedtls_x509_crt mbedtls_cacert;
    uint32_t mbedtls_flags;

    do
    {
        if((NULL == p_cacert) || (NULL == p_cert) )
        {
        	status = (int32_t)CRYPTO_LIB_NULL_PARAM;
            break;
        }

        //Check for length equal to zero
        if( (0 == cacert_size) || (0 == cert_size) )
        {
        	status = (int32_t)CRYPTO_LIB_LENZERO_ERROR;
            break;
        }

        //Initialise certificates
        mbedtls_x509_crt_init(&mbedtls_cacert);
        mbedtls_x509_crt_init(&mbedtls_cert);

        if ( (ret = mbedtls_x509_crt_parse_der(&mbedtls_cacert, p_cacert, cacert_size)) != 0 )
		{
			status = (int32_t)CRYPTO_LIB_CERT_PARSE_FAIL;
			break;
		}

        if ( (ret = mbedtls_x509_crt_parse_der(&mbedtls_cert, p_cert, cert_size)) != 0 )
		{
			status = (int32_t)CRYPTO_LIB_CERT_PARSE_FAIL;
			break;
		}

        if( ( ret = mbedtls_x509_crt_verify( &mbedtls_cert, &mbedtls_cacert,
        		                             NULL, NULL, &mbedtls_flags,
                                             NULL, NULL ) ) != 0 )
		{
        	status = (int32_t)CRYPTO_LIB_VERIFY_SIGN_FAIL;
			break;
		}

        status =   CRYPTO_LIB_OK;
    }while(FALSE);

    return status;
}


optiga_lib_status_t pal_crypt_get_public_key(const uint8_t* p_cert, uint16_t cert_size,
		                                           uint8_t* p_pubkey, uint16_t* p_pubkey_size)
{
    int32_t status  = (int32_t)CRYPTO_LIB_ERROR;
    int32_t ret;
    mbedtls_x509_crt mbedtls_cert;
    size_t pubkey_size = 0;
    // We know, that we will work with ECC
    mbedtls_ecp_keypair * mbedtls_keypair = NULL;

    do
    {
        if((NULL == p_cert) || (NULL == p_pubkey) || (NULL == p_pubkey_size))
        {
        	status = (int32_t)CRYPTO_LIB_NULL_PARAM;
            break;
        }

        //Check for length equal to zero
        if( (0 == cert_size) || (0 == *p_pubkey_size))
        {
        	status = (int32_t)CRYPTO_LIB_LENZERO_ERROR;
            break;
        }

        //Initialise certificates
        mbedtls_x509_crt_init(&mbedtls_cert);

        if ( (ret = mbedtls_x509_crt_parse_der(&mbedtls_cert, p_cert, cert_size)) != 0 )
		{
			status = (int32_t)CRYPTO_LIB_CERT_PARSE_FAIL;
			break;
		}

        mbedtls_keypair = (mbedtls_ecp_keypair* )mbedtls_cert.pk.pk_ctx;
        if ( (ret = mbedtls_ecp_point_write_binary(&mbedtls_keypair->grp, &mbedtls_keypair->Q,
        		                                   MBEDTLS_ECP_PF_UNCOMPRESSED, &pubkey_size,
												   p_pubkey, *p_pubkey_size)) != 0 )
        {
			status = (int32_t)CRYPTO_LIB_CERT_PARSE_FAIL;
			break;
        }
        *p_pubkey_size = pubkey_size;

        status =   CRYPTO_LIB_OK;
    }while(FALSE);

    return status;
}


optiga_lib_status_t  pal_crypt_verify_signature(const uint8_t* p_pubkey, uint16_t pubkey_size,
		                                        const uint8_t* p_signature, uint16_t signature_size,
									                  uint8_t* p_digest, uint16_t digest_size)
{
    int32_t status  = (int32_t)OPTIGA_LIB_ERROR;
    do
    {
        if((NULL == p_pubkey)|| (NULL == p_signature) || (NULL == p_pubkey))
        {
        	status = (int32_t)CRYPTO_LIB_NULL_PARAM;
            break;
        }
        //check if length is equal to zero
        if((0 == digest_size) || (0 == signature_size) || (0 == pubkey_size))
        {
        	status = (int32_t)CRYPTO_LIB_LENZERO_ERROR;
            break;
        }

        status = __verify_ecc_signature(p_pubkey, pubkey_size,
        		                        p_signature, signature_size,
										p_digest, digest_size);
    }while(FALSE);
    return status;
}

optiga_lib_status_t pal_crypt_init(void)
{
	int32_t status  = (int32_t)CRYPTO_LIB_OK;

	mbedtls_entropy_init( &entropy );
	uint8_t personalization[32];

	optiga_crypt_random(eDRNG, personalization, 32);

	mbedtls_ctr_drbg_init( &ctr_drbg );

	status = mbedtls_ctr_drbg_seed( &ctr_drbg , mbedtls_entropy_func, &entropy,
	                     (const unsigned char *) personalization,
	                     sizeof( personalization ) );

	if( status != 0 )
	{
		status  = (int32_t)CRYPTO_LIB_NULL_PARAM;
	}

	return status;
}

optiga_lib_status_t  pal_crypt_random(uint16_t random_size, uint8_t* p_random)
{
    int32_t status  = (int32_t)CRYPTO_LIB_NULL_PARAM;

    do
    {
        if((NULL == p_random))
        {
            break;
        }
        //Check for length equal to zero
        if(0==random_size)
        {
            status = (int32_t)CRYPTO_LIB_LENZERO_ERROR;
            break;
        }

        //Generate random data block
        if(0 != mbedtls_ctr_drbg_random(&ctr_drbg, p_random, random_size))
        {
            status = (int32_t)CRYPTO_LIB_ERROR;
            break;
        }

    	mbedtls_ctr_drbg_reseed( &ctr_drbg , p_random, random_size);

        status = CRYPTO_LIB_OK;
    }while(FALSE);
    return status;
}



