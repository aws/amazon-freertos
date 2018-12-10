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

#if defined(MBEDTLS_ECDSA_C)

#include "mbedtls/ecdsa.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/pk.h"

#include <string.h>


#include "optiga/optiga_crypt.h"
#include "optiga/optiga_util.h"

#if defined(MBEDTLS_ECDSA_SIGN_ALT)


int mbedtls_ecdsa_sign( mbedtls_ecp_group *grp, mbedtls_mpi *r, mbedtls_mpi *s,
                const mbedtls_mpi *d, const unsigned char *buf, size_t blen,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
	int ret;
	uint8_t der_signature[110];
	uint16_t dslen = sizeof(der_signature);
    unsigned char *p = der_signature;
    const unsigned char *end = der_signature + dslen;

    if(optiga_crypt_ecdsa_sign((unsigned char *)buf, blen, OPTIGA_KEY_STORE_ID_E0F1, der_signature, &dslen) != OPTIGA_LIB_SUCCESS)
    {
		ret = MBEDTLS_ERR_PK_BAD_INPUT_DATA;
		goto cleanup;
    }
	
	MBEDTLS_MPI_CHK( mbedtls_asn1_get_mpi( &p, end, r ) );
	MBEDTLS_MPI_CHK( mbedtls_asn1_get_mpi( &p, end, s ) );
	
cleanup:
    return ret;
}
#endif


#if defined(MBEDTLS_ECDSA_VERIFY_ALT)
int mbedtls_ecdsa_verify( mbedtls_ecp_group *grp,
                  const unsigned char *buf, size_t blen,
                  const mbedtls_ecp_point *Q, const mbedtls_mpi *r, const mbedtls_mpi *s)
{
	optiga_lib_status_t status = OPTIGA_LIB_ERROR;
    public_key_from_host_t public_key;
    uint8_t public_key_out [100];
	uint8_t signature [110];
    uint8_t *p = signature + sizeof(signature);
	size_t  signature_len = 0;
	size_t public_key_len = 0;
	uint8_t truncated_hash_length;
	
	signature_len = mbedtls_asn1_write_mpi( &p, signature, s );
    signature_len+= mbedtls_asn1_write_mpi( &p, signature, r );
	
    public_key.public_key = public_key_out;
	public_key.length     = sizeof( public_key_out );

	if (mbedtls_ecp_point_write_binary( grp, Q,
                                        MBEDTLS_ECP_PF_UNCOMPRESSED, &public_key_len,
                                        &public_key_out[3], sizeof( public_key_out ) ) != 0 )
    {
        return 1;
    }

    if ( ( grp->id !=  MBEDTLS_ECP_DP_SECP256R1 ) && 
         ( grp->id  != MBEDTLS_ECP_DP_SECP384R1 ) )
    {
        return 1;
    }

    grp->id == MBEDTLS_ECP_DP_SECP256R1 ? ( public_key.curve = OPTIGA_ECC_NIST_P_256 )
                                            : ( public_key.curve = OPTIGA_ECC_NIST_P_384 );

    public_key_out [0] = 0x03;
    public_key_out [1] = public_key_len + 1;
    public_key_out [2] = 0x00;
    public_key.length = public_key_len + 3;//including 3 bytes overhead

    if ( public_key.curve == OPTIGA_ECC_NIST_P_256 )
    {
        truncated_hash_length = 32; //maximum bytes of hash length for the curve
    }
    else
    {
        truncated_hash_length = 48; //maximum bytes of hash length for the curve
    }

    // If the length of the digest is larger than 
    // key length of the group order, then truncate the digest to key length.
    if ( blen > truncated_hash_length )
    {
        blen = truncated_hash_length;
    }

    status = optiga_crypt_ecdsa_verify ( (uint8_t *) buf, blen,
                                         (uint8_t *) p, signature_len,
										 OPTIGA_CRYPT_HOST_DATA, (void *)&public_key );
    if ( status != OPTIGA_LIB_SUCCESS )
    {
       return ( MBEDTLS_ERR_PK_BAD_INPUT_DATA );
    }
	
    return status;
}
#endif

#if defined(MBEDTLS_ECDSA_GENKEY_ALT)
int mbedtls_ecdsa_genkey( mbedtls_ecdsa_context *ctx, mbedtls_ecp_group_id gid,
                  int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
	optiga_lib_status_t status;
    uint8_t public_key [100];
    size_t public_key_len = sizeof( public_key );
    optiga_ecc_curve_t curve_id;
	mbedtls_ecp_group *grp = &ctx->grp;
	uint16_t privkey_oid = OPTIGA_KEY_STORE_ID_E0F0;
 
	mbedtls_ecp_group_load( &ctx->grp, gid );
 
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
												&privkey_oid,
												public_key,
												(uint16_t *)&public_key_len ) ;
	if ( status != OPTIGA_LIB_SUCCESS )
    {
		status = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    //store public key generated from optiga into mbedtls structure .
	if ( mbedtls_ecp_point_read_binary( grp, &ctx->Q,(unsigned char *)&public_key[3],(size_t )public_key_len-3 ) != 0 )
	{
		return 1;
	}

    return status;
}					  
#endif

#endif
/**
* @}
*/
