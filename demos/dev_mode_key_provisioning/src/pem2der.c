/*
 *  Convert PEM to DER
 *
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
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

/**
 * @file pem2der.c.c
 * @brief Certificate conversion from PEM to DER format
 */

/* Standard includes. */
#include <string.h>

/* mbedTLS includes. */
#include "mbedtls/base64.h"
/*-----------------------------------------------------------*/

/* @brief Converts PEM documents into DER formatted byte arrays.
 * This is a helper function from mbedTLS util pem2der.c
 * (https://github.com/ARMmbed/mbedtls/blob/development/programs/util/pem2der.c#L75)
 *
 * \param pucInput[in]       Pointer to PEM object
 * \param xLen[in]           Length of PEM object
 * \param pucOutput[out]     Pointer to buffer where DER object will be placed
 * \param pxOlen[in/out]     Pointer to length of DER buffer.  This value is updated
 *                          to contain the actual length of the converted DER object.
 *
 * \return 0 if successful.  Negative if conversion failed.  If buffer is not
 * large enough to hold converted object, pxOlen is still updated but -1 is returned.
 *
 */
int convert_pem_to_der( const unsigned char * pucInput,
                        size_t xLen,
                        unsigned char * pucOutput,
                        size_t * pxOlen )
{
    int lRet;
    const unsigned char * pucS1;
    const unsigned char * pucS2;
    const unsigned char * pucEnd = pucInput + xLen;
    size_t xOtherLen = 0;

    pucS1 = ( unsigned char * ) strstr( ( const char * ) pucInput, "-----BEGIN" );

    if( pucS1 == NULL )
    {
        return( -1 );
    }

    pucS2 = ( unsigned char * ) strstr( ( const char * ) pucInput, "-----END" );

    if( pucS2 == NULL )
    {
        return( -1 );
    }

    pucS1 += 10;

    while( pucS1 < pucEnd && *pucS1 != '-' )
    {
        pucS1++;
    }

    while( pucS1 < pucEnd && *pucS1 == '-' )
    {
        pucS1++;
    }

    if( *pucS1 == '\r' )
    {
        pucS1++;
    }

    if( *pucS1 == '\n' )
    {
        pucS1++;
    }

    if( ( pucS2 <= pucS1 ) || ( pucS2 > pucEnd ) )
    {
        return( -1 );
    }

    lRet = mbedtls_base64_decode( NULL, 0, &xOtherLen, ( const unsigned char * ) pucS1, pucS2 - pucS1 );

    if( lRet == MBEDTLS_ERR_BASE64_INVALID_CHARACTER )
    {
        return( lRet );
    }

    if( xOtherLen > *pxOlen )
    {
        return( -1 );
    }

    if( ( lRet = mbedtls_base64_decode( pucOutput, xOtherLen, &xOtherLen, ( const unsigned char * ) pucS1,
                                        pucS2 - pucS1 ) ) != 0 )
    {
        return( lRet );
    }

    *pxOlen = xOtherLen;

    return( 0 );
}
/*-----------------------------------------------------------*/

