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
* @{
*/

#include "mbedtls/config.h"

#if defined(MBEDTLS_ECDH_C)

#include "mbedtls/ecdh.h"

#include <string.h>

#include "optiga/optiga_crypt.h"
#include "optiga/optiga_util.h"


#ifdef MBEDTLS_ECDH_GEN_PUBLIC_ALT
/*
 * Generate public key: simple wrapper around mbedtls_ecp_gen_keypair
 */
int mbedtls_ecdh_gen_public( mbedtls_ecp_group *grp, mbedtls_mpi *d, mbedtls_ecp_point *Q,
                     int (*f_rng)(void *, unsigned char *, size_t),
                     void *p_rng )
{
    optiga_lib_status_t status;
    uint8_t public_key [200];
    size_t public_key_len = sizeof( public_key );
    optiga_ecc_curve_t curve_id;
    optiga_key_id_t optiga_key_id = OPTIGA_KEY_STORE_ID_E0F3;

    //checking group against the supported curves of Optiga Trust X
    if ( ( grp->id != MBEDTLS_ECP_DP_SECP256R1 ) &&
		 ( grp->id != MBEDTLS_ECP_DP_SECP384R1 ) )
	{
		return 1;
	}

	grp->id == MBEDTLS_ECP_DP_SECP256R1 ? ( curve_id = OPTIGA_ECC_NIST_P_256 )
                                                : ( curve_id = OPTIGA_ECC_NIST_P_384 );
    //invoke optiga command to generate a key pair.
	status = optiga_crypt_ecc_generate_keypair( curve_id,
                                                (optiga_key_usage_t)( OPTIGA_KEY_USAGE_KEY_AGREEMENT | OPTIGA_KEY_USAGE_AUTHENTICATION ),
												FALSE,
												&optiga_key_id,
												public_key,
												(uint16_t *)&public_key_len ) ;
	if ( status != OPTIGA_LIB_SUCCESS )
    {
		status = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    //store public key generated from optiga into mbedtls structure .
	if ( mbedtls_ecp_point_read_binary( grp, Q,(unsigned char *)&public_key[3],(size_t )public_key_len-3 ) != 0 )
	{
		return 1;
	}

	return (mbedtls_ecp_point_read_binary(grp, Q, &public_key[3], public_key_len-3));
}
#endif

#ifdef MBEDTLS_ECDH_COMPUTE_SHARED_ALT
/*
 * Compute shared secret (SEC1 3.3.1)
 */
int mbedtls_ecdh_compute_shared( mbedtls_ecp_group *grp, mbedtls_mpi *z,
                         const mbedtls_ecp_point *Q, const mbedtls_mpi *d,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng )
{
	uint32_t status;
    public_key_from_host_t publickey;
    uint8_t public_key_out[100];
    size_t public_key_length;
    uint8_t buf[100];

    //Step1: Prepare the public key material as expected by security chip
    //checking gid against the supported curves of OPTIGA Trust X
    if( (grp->id == MBEDTLS_ECP_DP_SECP256R1) || (grp->id  == MBEDTLS_ECP_DP_SECP384R1))
    {
    	grp->id == MBEDTLS_ECP_DP_SECP256R1 ? (publickey.curve = OPTIGA_ECC_NIST_P_256)
                                                : (publickey.curve = OPTIGA_ECC_NIST_P_384);

		mbedtls_ecp_point_write_binary(grp, Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &public_key_length, &public_key_out[3], 100);

		if(publickey.curve == OPTIGA_ECC_NIST_P_256)
        {
            public_key_out[0] = 0x03;
            public_key_out[1] = 0x42;
            public_key_out[2] = 0x00;
        }
        else
        {
            public_key_out[0] = 0x03;
            public_key_out[1] = 0x62;
            public_key_out[2] = 0x00;
        }

		publickey.public_key = public_key_out;
		publickey.length = public_key_length + 3;

        //Invoke optiga command to generate shared secret and store in the OID/buffer.
        status = optiga_crypt_ecdh(OPTIGA_KEY_STORE_ID_E0F3,
        						   &publickey,
        						   1,
                                   buf);

        if ( status != OPTIGA_LIB_SUCCESS )
        {
            status = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        }

		mbedtls_mpi_read_binary( z, buf, mbedtls_mpi_size( &grp->P ) );
    }
    else
    {
        //error state set indicates unexpected gid to OPTIGA Trust X
        status = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    return status;
}
#endif

#endif
/**
* @}
*/
