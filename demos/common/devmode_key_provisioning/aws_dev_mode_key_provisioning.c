/*
 * Amazon FreeRTOS V1.2.5
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

/* PKCS#11 includes. */
#include "aws_pkcs11.h"

/* Client credential includes. */
#include "aws_clientcredential.h"
#include "aws_default_root_certificates.h"

/* Key provisioning includes. */
#include "aws_dev_mode_key_provisioning.h"

/* Key provisioning helper defines. */
#define provisioningPRIVATE_KEY_TEMPLATE_COUNT         4
#define provisioningCERTIFICATE_TEMPLATE_COUNT         3
#define provisioningROOT_CERTIFICATE_TEMPLATE_COUNT    3


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

void vAlternateKeyProvisioning( ProvisioningParams_t * xParams )
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
    CK_KEY_TYPE xPrivateKeyType = ( CK_KEY_TYPE ) xParams->ulClientPrivateKeyType;
    CK_BBOOL xCanSign = CK_TRUE;
    CK_ATTRIBUTE xPrivateKeyTemplate[ provisioningPRIVATE_KEY_TEMPLATE_COUNT ];
    CK_ATTRIBUTE xCertificateTemplate[ provisioningCERTIFICATE_TEMPLATE_COUNT ];
    CK_ATTRIBUTE xRootCertificateTemplate[ provisioningROOT_CERTIFICATE_TEMPLATE_COUNT ];

    configPRINTF( ( "Starting key provisioning...\r\n" ) );

    /* Initialize the device private key template. */
    xPrivateKeyTemplate[ 0 ].type = CKA_CLASS;
    xPrivateKeyTemplate[ 0 ].pValue = &xPrivateKeyClass;
    xPrivateKeyTemplate[ 0 ].ulValueLen = sizeof( xPrivateKeyClass );
    xPrivateKeyTemplate[ 1 ].type = CKA_KEY_TYPE;
    xPrivateKeyTemplate[ 1 ].pValue = &xPrivateKeyType;
    xPrivateKeyTemplate[ 1 ].ulValueLen = sizeof( xPrivateKeyType );
    xPrivateKeyTemplate[ 2 ].type = CKA_SIGN;
    xPrivateKeyTemplate[ 2 ].pValue = &xCanSign;
    xPrivateKeyTemplate[ 2 ].ulValueLen = sizeof( xCanSign );
    xPrivateKeyTemplate[ 3 ].type = CKA_VALUE;
    xPrivateKeyTemplate[ 3 ].pValue = ( CK_VOID_PTR ) xParams->pcClientPrivateKey;
    xPrivateKeyTemplate[ 3 ].ulValueLen = ( CK_ULONG ) xParams->ulClientPrivateKeyLength;

    /* Initialize the client certificate template. */
    xCertificateTemplate[ 0 ].type = CKA_CLASS;
    xCertificateTemplate[ 0 ].pValue = &xCertificateClass;
    xCertificateTemplate[ 0 ].ulValueLen = sizeof( xCertificateClass );
    xCertificateTemplate[ 1 ].type = CKA_VALUE;
    xCertificateTemplate[ 1 ].pValue = ( CK_VOID_PTR ) xParams->pcClientCertificate;
    xCertificateTemplate[ 1 ].ulValueLen = ( CK_ULONG ) xParams->ulClientCertificateLength;
    xCertificateTemplate[ 2 ].type = CKA_CERTIFICATE_TYPE;
    xCertificateTemplate[ 2 ].pValue = &xDeviceCertificateType;
    xCertificateTemplate[ 2 ].ulValueLen = sizeof( xDeviceCertificateType );

    /* Initialize the root certificate template. */
    xRootCertificateTemplate[ 0 ].type = CKA_CLASS;
    xRootCertificateTemplate[ 0 ].pValue = &xCertificateClass;
    xRootCertificateTemplate[ 0 ].ulValueLen = sizeof( xCertificateClass );
    xRootCertificateTemplate[ 1 ].type = CKA_VALUE;
    xRootCertificateTemplate[ 1 ].pValue = ( CK_VOID_PTR ) tlsVERISIGN_ROOT_CERTIFICATE_PEM;
    xRootCertificateTemplate[ 1 ].ulValueLen = ( CK_ULONG ) tlsVERISIGN_ROOT_CERTIFICATE_LENGTH;
    xRootCertificateTemplate[ 2 ].type = CKA_CERTIFICATE_TYPE;
    xRootCertificateTemplate[ 2 ].pValue = &xRootCertificateType;
    xRootCertificateTemplate[ 2 ].ulValueLen = sizeof( xRootCertificateType );

    /* Initialize the module. */
    xResult = prvInitialize( &pxFunctionList,
                             &xSlotId,
                             &xSession );

    /*
     * Check that a certificate and private key can be imported into
     * persistent storage.
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

void vDevModeKeyProvisioning( void )
{
    ProvisioningParams_t xParams;

    xParams.ulClientPrivateKeyType = CKK_RSA;
    xParams.pcClientPrivateKey = ( uint8_t * ) clientcredentialCLIENT_PRIVATE_KEY_PEM;
    xParams.ulClientPrivateKeyLength = clientcredentialCLIENT_PRIVATE_KEY_LENGTH;
    xParams.pcClientCertificate = ( uint8_t * ) clientcredentialCLIENT_CERTIFICATE_PEM;
    xParams.ulClientCertificateLength = clientcredentialCLIENT_CERTIFICATE_LENGTH;

    vAlternateKeyProvisioning( &xParams );
}

/*-----------------------------------------------------------*/
