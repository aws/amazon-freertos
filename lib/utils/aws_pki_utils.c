/* These helper routines are from mbedTLS. The function names have been renamed
 * for Amazon FreeRTOS library consistency. Otherwise, the implementations
 * are unchanged. */

/*
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
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
 *  https://tls.mbed.org
 */

/* Amazon FreeRTOS includes. */
#include "FreeRTOS.h"
#include "aws_pki_utils.h"

/* mbedTLS include. */
#include "mbedtls/base64.h"
#include "mbedtls/md.h"
#include "mbedtls/oid.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/x509_crt.h"

/*-----------------------------------------------------------*/

/* \brief Converts Privacy Enhanced Mail (PEM) formatted data to Distinguished
 * Encoding Rules (ASN.1 DER) format. 
 */
int PKI_ConvertPEMToDER( const unsigned char *input, 
                         size_t ilen,
                         unsigned char *output, 
                         size_t *olen )
{
    int ret;
    const unsigned char *s1, *s2, *end = input + ilen;
    size_t len = 0;

    s1 = ( unsigned char * )strstr( ( const char * )input, "-----BEGIN" );
    if( s1 == NULL )
        return( -1 );

    s2 = ( unsigned char * )strstr( ( const char * )input, "-----END" );
    if( s2 == NULL )
        return( -1 );

    s1 += 10;
    while( s1 < end && *s1 != '-' )
        s1++;
    while( s1 < end && *s1 == '-' )
        s1++;
    if( *s1 == '\r' ) s1++;
    if( *s1 == '\n' ) s1++;

    if( s2 <= s1 || s2 > end )
        return( -1 );

    ret = mbedtls_base64_decode( NULL, 0, &len, ( const unsigned char * )s1, s2 - s1 );
    if( ret == MBEDTLS_ERR_BASE64_INVALID_CHARACTER )
        return( ret );

    if( len > *olen )
        return( -1 );

    if( ( ret = mbedtls_base64_decode( output, len, &len, ( const unsigned char * )s1,
                                       s2 - s1 ) ) != 0 )
    {
        return( ret );
    }

    *olen = len;

    return( 0 );
}

/*-----------------------------------------------------------*/

/* \brief Formats cryptographically hashed data for RSA signing in accordance 
 * with PKCS #1 version 1.5.
 *
 * Currently assumes SHA-256. 
 */
int PKI_RSA_RSASSA_PKCS1_v15_Encode( const unsigned char *hash,
                                     size_t dst_len,
                                     unsigned char *dst )
{
    size_t oid_size = 0;
    size_t nb_pad = dst_len;
    unsigned char *p = dst;
    const char *oid = NULL;
    mbedtls_md_type_t md_alg = MBEDTLS_MD_SHA256;
    unsigned int hashlen = 0;

    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type( md_alg );
    if( md_info == NULL )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    if( mbedtls_oid_get_oid_by_md( md_alg, &oid, &oid_size ) != 0 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    hashlen = mbedtls_md_get_size( md_info );

    /* Double-check that 8 + hashlen + oid_size can be used as a
    * 1-byte ASN.1 length encoding and that there's no overflow. */
    if( 8 + hashlen + oid_size >= 0x80 ||
        10 + hashlen < hashlen ||
        10 + hashlen + oid_size < 10 + hashlen )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    /*
    * Static bounds check:
    * - Need 10 bytes for five tag-length pairs.
    *   (Insist on 1-byte length encodings to protect against variants of
    *    Bleichenbacher's forgery attack against lax PKCS#1v1.5 verification)
    * - Need hashlen bytes for hash
    * - Need oid_size bytes for hash alg OID.
    */
    if( nb_pad < 10 + hashlen + oid_size )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    nb_pad -= 10 + hashlen + oid_size;

    /* Need space for signature header and padding delimiter (3 bytes),
    * and 8 bytes for the minimal padding */
    if( nb_pad < 3 + 8 )
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    nb_pad -= 3;

    /* Now nb_pad is the amount of memory to be filled
    * with padding, and at least 8 bytes long. */

    /* Write signature header and padding */
    *p++ = 0;
    *p++ = MBEDTLS_RSA_SIGN;
    memset( p, 0xFF, nb_pad );
    p += nb_pad;
    *p++ = 0;

    /* Are we signing raw data? */
    if( md_alg == MBEDTLS_MD_NONE )
    {
        memcpy( p, hash, hashlen );
        return( 0 );
    }

    /* Signing hashed data, add corresponding ASN.1 structure
    *
    * DigestInfo ::= SEQUENCE {
    *   digestAlgorithm DigestAlgorithmIdentifier,
    *   digest Digest }
    * DigestAlgorithmIdentifier ::= AlgorithmIdentifier
    * Digest ::= OCTET STRING
    *
    * Schematic:
    * TAG-SEQ + LEN [ TAG-SEQ + LEN [ TAG-OID  + LEN [ OID  ]
    *                                 TAG-NULL + LEN [ NULL ] ]
    *                 TAG-OCTET + LEN [ HASH ] ]
    */
    *p++ = MBEDTLS_ASN1_SEQUENCE | MBEDTLS_ASN1_CONSTRUCTED;
    *p++ = ( unsigned char )( 0x08 + oid_size + hashlen );
    *p++ = MBEDTLS_ASN1_SEQUENCE | MBEDTLS_ASN1_CONSTRUCTED;
    *p++ = ( unsigned char )( 0x04 + oid_size );
    *p++ = MBEDTLS_ASN1_OID;
    *p++ = ( unsigned char )oid_size;
    memcpy( p, oid, oid_size );
    p += oid_size;
    *p++ = MBEDTLS_ASN1_NULL;
    *p++ = 0x00;
    *p++ = MBEDTLS_ASN1_OCTET_STRING;
    *p++ = ( unsigned char )hashlen;
    memcpy( p, hash, hashlen );
    p += hashlen;

    /* Just a sanity-check, should be automatic
    * after the initial bounds check. */
    if( p != dst + dst_len )
    {
        mbedtls_platform_zeroize( dst, dst_len );
        return( MBEDTLS_ERR_RSA_BAD_INPUT_DATA );
    }

    return( 0 );
}

/*-----------------------------------------------------------*/

void PKI_mbedTLSSignatureToPkcs11Signature( uint8_t * pxSignaturePKCS,
                                            uint8_t * pxMbedSignature )
{
    /* Reconstruct the signature in PKCS #11 format. */
    uint8_t * pxNextLength;
    uint8_t ucSigComponentLength = pxMbedSignature[ 3 ];

    memset( pxSignaturePKCS, 0, 64 );

    /* R Component. */
    if( ucSigComponentLength == 33 )
    {
        memcpy( pxSignaturePKCS, &pxMbedSignature[ 5 ], 32 ); /* Chop off the leading zero. */
        pxNextLength = pxMbedSignature + 5 /* Sequence, length, integer, length, leading zero */ + 32 /*(R) */ + 1 /*(S's integer tag) */;
    }
    else
    {
        memcpy( &pxSignaturePKCS[ 32 - ucSigComponentLength ], &pxMbedSignature[ 4 ], ucSigComponentLength );
        pxNextLength = pxMbedSignature + 4 + ucSigComponentLength + 1;
    }

    /* S Component. */
    ucSigComponentLength = pxNextLength[ 0 ];

    if( ucSigComponentLength == 33 )
    {
        memcpy( &pxSignaturePKCS[ 32 ], &pxNextLength[ 2 ], 32 ); /* Skip leading zero. */
    }
    else
    {
        memcpy( &pxSignaturePKCS[ 64 - ucSigComponentLength ], &pxNextLength[ 1 ], ucSigComponentLength );
    }
}
