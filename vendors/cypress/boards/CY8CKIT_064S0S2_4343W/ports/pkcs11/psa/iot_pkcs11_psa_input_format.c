/*
 *  Copyright (C) 2006-2020, Arm Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

/**
 * @file iot_pkcs11_psa_input_format.c
 * @brief Functions to get the PSA API format input.
 */

#include <string.h>
#include "iot_pkcs11_psa_input_format.h"

/* Get a PK algorithm identifier
 *
 *  AlgorithmIdentifier  ::=  SEQUENCE  {
 *       algorithm               OBJECT IDENTIFIER,
 *       parameters              ANY DEFINED BY algorithm OPTIONAL  }
 */
int pk_get_pk_alg( unsigned char **p,
                          const unsigned char *end,
                          mbedtls_pk_type_t *pk_alg, mbedtls_asn1_buf *params )
{
    int ret;
    mbedtls_asn1_buf alg_oid;

    memset( params, 0, sizeof(mbedtls_asn1_buf) );

    if( ( ret = mbedtls_asn1_get_alg( p, end, &alg_oid, params ) ) != 0 )
        return( MBEDTLS_ERR_PK_INVALID_ALG + ret );

    if( mbedtls_oid_get_pk_alg( &alg_oid, pk_alg ) != 0 )
        return( MBEDTLS_ERR_PK_UNKNOWN_PK_ALG );

    /*
     * No parameters with RSA (only for EC)
     */
    if( *pk_alg == MBEDTLS_PK_RSA &&
            ( ( params->tag != MBEDTLS_ASN1_NULL && params->tag != 0 ) ||
                params->len != 0 ) )
    {
        return( MBEDTLS_ERR_PK_INVALID_ALG );
    }

    return( 0 );
}

/*
 * Get the content of the privateKey field of the ECPrivateKey
 * format defined by RFC 5915.
 *
 * Notes:
 *
 * - This function does not own the key buffer. It is the
 *   responsibility of the caller to take care of zeroizing
 *   and freeing it after use.
 *
 * - The function is responsible for freeing the provided
 *   PK context on failure.
 *
 */
int get_ECPrivateKey_sequence_privatekey_sec1_der(
                                    const unsigned char* key,
                                    size_t keylen,
                                    unsigned char **privatekeystart,
                                    size_t * privatekeyderlength )
{
    int ret;
    int version;
    size_t len;
    unsigned char *p = (unsigned char *) key;
    unsigned char *end = p + keylen;

    /*
     * RFC 5915, or SEC1 Appendix C.4
     *
     * ECPrivateKey ::= SEQUENCE {
     *      version        INTEGER { ecPrivkeyVer1(1) } (ecPrivkeyVer1),
     *      privateKey     OCTET STRING,
     *      parameters [0] ECParameters {{ NamedCurve }} OPTIONAL,
     *      publicKey  [1] BIT STRING OPTIONAL
     *    }
     */
    if( ( ret = mbedtls_asn1_get_tag( &p, end, &len,
            MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE ) ) != 0 )
    {
        return( MBEDTLS_ERR_PK_KEY_INVALID_FORMAT + ret );
    }

    end = p + len;

    if( ( ret = mbedtls_asn1_get_int( &p, end, &version ) ) != 0 )
        return( MBEDTLS_ERR_PK_KEY_INVALID_FORMAT + ret );

    if( version != 1 )
        return( MBEDTLS_ERR_PK_KEY_INVALID_VERSION );

    if( ( ret = mbedtls_asn1_get_tag( &p, end, &len, MBEDTLS_ASN1_OCTET_STRING ) ) != 0 )
        return( MBEDTLS_ERR_PK_KEY_INVALID_FORMAT + ret );

    /*
        * Finally get the start address of privatekey and its length.
        */
    *privatekeystart = p;
    *privatekeyderlength = len;
    return 0;
}

/*
 * Get the uncompressed representation defined by SEC1 §2.3.3
 * as the content of an ECPoint.
 */
int get_public_key_ECPoint( const unsigned char *key,
                            size_t keylen,
                            unsigned char **startaddr,
                            size_t *length )
{
    int ret;
    size_t len;
    mbedtls_asn1_buf alg_params;
    mbedtls_pk_type_t pk_alg = MBEDTLS_PK_NONE;
    unsigned char *p = (unsigned char *) key;
    const unsigned char *end = key + keylen;

    if( ( ret = mbedtls_asn1_get_tag( &p, end, &len,
                    MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE ) ) != 0 )
    {
        return( MBEDTLS_ERR_PK_KEY_INVALID_FORMAT + ret );
    }

    end = p + len;

    if( ( ret = pk_get_pk_alg( &p, end, &pk_alg, &alg_params ) ) != 0 )
        return( ret );

    if( ( ret = mbedtls_asn1_get_bitstring_null( &p, end, &len ) ) != 0 )
        return( MBEDTLS_ERR_PK_INVALID_PUBKEY + ret );

    if( p + len != end )
        return( MBEDTLS_ERR_PK_INVALID_PUBKEY +
                MBEDTLS_ERR_ASN1_LENGTH_MISMATCH );

    /**
     * Get the start address of public key and its length.
     */
    if( pk_alg == MBEDTLS_PK_ECKEY_DH || pk_alg == MBEDTLS_PK_ECKEY )
    {
        *startaddr = p;
        *length = end - p;
    }
    return 0;
}
