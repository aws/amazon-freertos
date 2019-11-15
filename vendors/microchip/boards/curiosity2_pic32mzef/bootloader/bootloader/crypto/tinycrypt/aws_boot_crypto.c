/*
 * Amazon FreeRTOS Demo Bootloader V1.4.8
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */


/**
 * @file aws_boot_crypto.c
 * @brief Boot crypto implementation.
 */

/* Standard includes.*/
#include "string.h"

/* Bootloader includes.*/
#include "aws_boot_types.h"
#include "aws_boot_crypto.h"

// Moved content from boot_codesigner_public_key.h.

const unsigned char pucCodeSignPublicKey[] = {
    0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86,
    0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x08, 0x2a,
    0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03,
    0x42, 0x00, 0x04, 0x9f, 0x24, 0x8b, 0x02, 0xd3,
    0x53, 0x9e, 0x49, 0x68, 0x11, 0x65, 0xfe, 0x4b,
    0xed, 0x1a, 0x5d, 0x24, 0x20, 0xd0, 0xa2, 0x4d,
    0x85, 0x8f, 0xd5, 0x0e, 0xa6, 0x56, 0xf2, 0x43,
    0x86, 0xf5, 0xba, 0xef, 0xb2, 0xef, 0x88, 0xf2,
    0x34, 0x56, 0x2c, 0xe3, 0x8a, 0xe1, 0x8c, 0xb1,
    0xa6, 0x6d, 0x50, 0x69, 0x50, 0x67, 0xc0, 0xc0,
    0xc9, 0x63, 0x1f, 0x7b, 0x65, 0xcf, 0x25, 0xda,
    0xb7, 0xbb, 0xb3,
    };
const unsigned int ulCodeSignPublickeyLength = 91;

/* Crypto includes.*/
#include "tinycrypt/sha256.h"
#include "tinycrypt/ecc.h"
#include "tinycrypt/constants.h"
#include "tinycrypt/ecc_dsa.h"

/* ASN.1 parser include.*/
#include "asn1.h"
#include "asn1utility.h"


/*-----------------------------------------------------------*/

BaseType_t BOOT_CRYPTO_Init( void )
{
    /*
     * Bootloader crypto module initializations here.
     */

    /*
     * No initialization for tinycrypt library.
     */

    return pdTRUE;
}

/*-----------------------------------------------------------*/

BaseType_t BOOT_CRYPTO_Verify( const uint8_t * pucData,
                               uint32_t ulSize,
                               const uint8_t * pucSignature,
                               uint32_t ulSignatureSize )
{
    BaseType_t xResult = pdFALSE;
    int32_t lReturn;
    uint32_t ulBitStringPos = 0;

    uint8_t pucHash[ TC_SHA256_DIGEST_SIZE ];

    /* ASN.1 encodes signature.*/
    uint8_t pucSignatureEncoded[ BOOT_ECC_SIGNATURE_SIZE_MAX ];

    /* Decoded signature containing required elements on the curve.*/
    uint8_t pucSignatureDecoded[ ECC_NUM_SIG_COMPONENTS * ECC_NUM_BYTES_PER_SIG_COMPONENT ];

    /*
     * Hashing context.
     */
    struct tc_sha256_state_struct xCtx;

    /*
     * Initialize the context.
     */
    if( TC_CRYPTO_SUCCESS == tc_sha256_init( &xCtx ) )
    {
        /*
         * Update with data and size.
         */
        if( TC_CRYPTO_SUCCESS == tc_sha256_update( &xCtx,
                                                   pucData,
                                                   ulSize ) )
        {
            /*
             * Finalize the hash.
             */
            if( TC_CRYPTO_SUCCESS == tc_sha256_final( pucHash, &xCtx ) )
            {
                xResult = pdTRUE;
            }
            else
            {
                xResult = pdFALSE;
            }
        }
        else
        {
            xResult = pdFALSE;
        }
    }
    else
    {
        xResult = pdFALSE;
    }

    /*
     * Copy signature in coded form to local buffer.
     */
    if( ( xResult == pdTRUE ) && ( ulSignatureSize <= BOOT_ECC_SIGNATURE_SIZE_MAX ) )
    {
        memcpy( pucSignatureEncoded, pucSignature, ulSignatureSize );
    }
    else
    {
        xResult = pdFALSE;
    }

    /*
     * Decode ASN.1 signature.
     */
    if( xResult == pdTRUE )
    {
        xResult = asn1_decodeSignature( pucSignatureDecoded,
                                        pucSignatureEncoded,
                                        pucSignatureEncoded + ulSignatureSize );
    }

    /*
     * Verify crypto signature.
     */
    if( xResult == pdTRUE )
    {
        /* Skip the ASN.1 tags to get public key */
        ulBitStringPos = ulCodeSignPublickeyLength - ECC_PUBKEY_BIT_STRING_SIZE;

        lReturn = uECC_verify( pucCodeSignPublicKey + ulBitStringPos,
                               pucHash,
                               TC_SHA256_DIGEST_SIZE,
                               pucSignatureDecoded,
                               uECC_secp256r1() );

        if( lReturn == 1 )
        {
            xResult = pdTRUE;
        }
        else
        {
            xResult = pdFALSE;
        }
    }

    return xResult;
}
