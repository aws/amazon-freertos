/*
 * Amazon FreeRTOS V1.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
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
 * @file aws_dev_mode_key_provisioning.c
 * @brief Simple key provisioning example using pkcs#11
 *
 * A simple example to demonstrate key and certificate provisioning in
 * flash using PKCS#11 interface. This should be replaced
 * by production ready key provisioning mechanism.
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* pkcs11 includes */
#include "aws_pkcs11.h"

/* client credential includes */
#include "aws_clientcredential.h"
#include "aws_default_root_certificates.h"

/*key provisioning includes */
#include "aws_dev_mode_key_provisioning.h"

/*-----------------------------------------------------------*/

static CK_RV prvInitialize( CK_FUNCTION_LIST_PTR * ppxFunctionList,
                            CK_SLOT_ID * pxSlotId,
                            CK_SESSION_HANDLE * pxSession )
{
    CK_RV xResult = 0;
    CK_C_GetFunctionList pxCkGetFunctionList = NULL;
    CK_ULONG ulCount = 1;

    ( void ) ( pxSession );

    /* Ensure that the PKCS#11 module is initialized. */
    if( 0 == xResult )
    {
        pxCkGetFunctionList = C_GetFunctionList;
        xResult = pxCkGetFunctionList( ppxFunctionList );
    }

    if( 0 == xResult )
    {
        xResult = ( *ppxFunctionList )->C_Initialize( NULL );
    }

    /* Get the default private key storage ID. */
    if( 0 == xResult )
    {
        xResult = ( *ppxFunctionList )->C_GetSlotList( CK_TRUE,
                                                       pxSlotId,
                                                       &ulCount );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

void vDevModeKeyProvisioning( void )
{
    CK_RV xResult = 0;
    CK_FUNCTION_LIST_PTR pxFunctionList = NULL;
    CK_SLOT_ID xSlotId = 0;
    CK_SESSION_HANDLE xSession = 0;
    CK_OBJECT_HANDLE xObject = 0;
    CK_OBJECT_CLASS xPrivateKeyClass = CKO_PRIVATE_KEY;
    CK_OBJECT_CLASS xCertificateClass = CKO_CERTIFICATE;
    CK_OBJECT_CLASS xDeviceCertificateType = pkcs11CERTIFICATE_TYPE_USER;
    CK_OBJECT_CLASS xRootCertificateType = pkcs11CERTIFICATE_TYPE_ROOT;
    CK_KEY_TYPE xPrivateKeyType = CKK_RSA;
    CK_BBOOL xCanSign = CK_TRUE;

    configPRINTF( ( "Starting key provisioning...\r\n" ) );

    CK_ATTRIBUTE xPrivateKeyTemplate[] =
    {
        { CKA_CLASS,    &xPrivateKeyClass, sizeof( xPrivateKeyClass ) },
        { CKA_KEY_TYPE, &xPrivateKeyType,  sizeof( xPrivateKeyType )  },
        { CKA_SIGN,     &xCanSign,         sizeof( xCanSign )         },
        { CKA_VALUE,
          ( CK_VOID_PTR ) clientcredentialCLIENT_PRIVATE_KEY_PEM,
          ( CK_ULONG ) clientcredentialCLIENT_PRIVATE_KEY_LENGTH }
    };

    CK_ATTRIBUTE xCertificateTemplate[] =
    {
        { CKA_CLASS,            &xCertificateClass,      sizeof( xCertificateClass )      },
        { CKA_VALUE,
          ( CK_VOID_PTR ) clientcredentialCLIENT_CERTIFICATE_PEM,
          ( CK_ULONG ) clientcredentialCLIENT_CERTIFICATE_LENGTH },
        { CKA_CERTIFICATE_TYPE, &xDeviceCertificateType, sizeof( xDeviceCertificateType ) }
    };

    CK_ATTRIBUTE xRootCertificateTemplate[] =
    {
        { CKA_CLASS,            &xCertificateClass,    sizeof( xCertificateClass )    },

        { CKA_VALUE,
          ( CK_VOID_PTR ) tlsVERISIGN_ROOT_CERTIFICATE_PEM,
          ( CK_ULONG ) tlsVERISIGN_ROOT_CERTIFICATE_LENGTH },
        { CKA_CERTIFICATE_TYPE, &xRootCertificateType, sizeof( xRootCertificateType ) }
    };

    /* Initialize the module. */
    xResult = prvInitialize( &pxFunctionList,
                             &xSlotId,
                             &xSession );

    /** \brief Check that a certificate and private key can be imported into
     *
     * persistent storage.
     *
     */

    /* Create an object using the encoded root certificate. */
    if( 0 == xResult )
    {
        configPRINTF( ( "Write root certificate...\r\n" ) );

        xResult = pxFunctionList->C_CreateObject( xSession,
                                                  xRootCertificateTemplate,
                                                  sizeof( xRootCertificateTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                  &xObject );
    }

    /* Free the certificate. */
    if( 0 == xResult )
    {
        xResult = pxFunctionList->C_DestroyObject( xSession,
                                                   xObject );
        xObject = 0;
    }

    /* Create an object using the encoded private key. */
    if( 0 == xResult )
    {
        configPRINTF( ( "Write device private key...\r\n" ) );

        xResult = pxFunctionList->C_CreateObject( xSession,
                                                  xPrivateKeyTemplate,
                                                  sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                  &xObject );
    }

    /* Free the private key. */
    if( 0 == xResult )
    {
        xResult = pxFunctionList->C_DestroyObject( xSession,
                                                   xObject );

        xObject = 0;
    }

    /* Create an object using the encoded client certificate. */
    if( 0 == xResult )
    {
        configPRINTF( ( "Write device certificate...\r\n" ) );

        xResult = pxFunctionList->C_CreateObject( xSession,
                                                  xCertificateTemplate,
                                                  sizeof( xCertificateTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                  &xObject );
    }

    /* Free the certificate. */
    if( 0 == xResult )
    {
        xResult = pxFunctionList->C_DestroyObject( xSession,
                                                   xObject );
        xObject = 0;
    }

    configPRINTF( ( "Key provisioning done...\r\n" ) );
}

/*-----------------------------------------------------------*/
