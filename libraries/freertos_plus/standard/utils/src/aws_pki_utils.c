/*
* Amazon FreeRTOS PKI Utilities V1.0.0
* Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* Amazon FreeRTOS includes. */
#include "FreeRTOS.h"
#include "aws_pki_utils.h"

/* CRT includes. */
#include <stdio.h>
#include <string.h>


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
