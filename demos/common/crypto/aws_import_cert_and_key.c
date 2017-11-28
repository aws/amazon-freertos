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
 * @brief A task is created to import the default AWS IoT TLS client certificate
 * and into storage on the host device.
 */

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Crypto includes. */
#include "FreeRTOS_Crypto.h"
#include "FreeRTOS_PKCS11.h"
#include "aws_clientcredential.h"
/*-----------------------------------------------------------*/

/**
 * @brief Import the default certificate and private key into device storage.
 */
static void prvImportCredentialsTask( void * pvParameters )
{
    BaseType_t xResult = 0;
    CK_C_GetFunctionList pxCkGetFunctionList = NULL;
    CK_FUNCTION_LIST_PTR pxP11FunctionList = NULL;
    CK_SLOT_ID xSlotId = 0;
    CK_ULONG ulCount = 1;
    CK_SESSION_HANDLE xP11Session = NULL;
    CK_OBJECT_HANDLE xP11Object = NULL;
    CK_OBJECT_CLASS xPrivateKeyClass = CKO_PRIVATE_KEY;
    CK_OBJECT_CLASS xCertificateClass = CKO_CERTIFICATE;
    CK_KEY_TYPE xPrivateKeyType = CKK_RSA;
    CK_BBOOL xCanSign = CK_TRUE;
    CK_ATTRIBUTE xPrivateKeyTemplate[] =
    {
        { CKA_CLASS,    &xPrivateKeyClass, sizeof( xPrivateKeyClass ) },
        { CKA_KEY_TYPE, &xPrivateKeyType,  sizeof( xPrivateKeyType )  },
        { CKA_SIGN,     &xCanSign,         sizeof( xCanSign )         },
        { CKA_VALUE,
          clientcredentialCLIENT_PRIVATE_KEY_PEM,
          clientcredentialCLIENT_PRIVATE_KEY_LENGTH }
    };
    CK_ATTRIBUTE xCertificateTemplate[] =
    {
        { CKA_CLASS, &xCertificateClass, sizeof( xCertificateClass ) },
        { CKA_VALUE,
          clientcredentialCLIENT_CERTIFICATE_PEM,
          clientcredentialCLIENT_CERTIFICATE_LENGTH }
    };

    /* Unreferenced parameter. */
    ( void * ) pvParameters;

    /* Ensure that the FreeRTOS heap is used. */
    CRYPTO_ConfigureHeap();

    /* Ensure that the PKCS#11 module is initialized. */
    if( 0 == xResult )
    {
        pxCkGetFunctionList = C_GetFunctionList;
        xResult = pxCkGetFunctionList( &pxP11FunctionList );
    }

    if( 0 == xResult )
    {
        xResult = pxP11FunctionList->C_Initialize( NULL );
    }

    /* Get the default private key storage ID. */
    if( 0 == xResult )
    {
        xResult = pxP11FunctionList->C_GetSlotList(
            CK_TRUE,
            &xSlotId,
            &ulCount );
    }

    /* Start a private session with the PKCS#11 module. */
    if( 0 == xResult )
    {
        xResult = pxP11FunctionList->C_OpenSession(
            xSlotId,
            0,
            NULL,
            NULL,
            &xP11Session );
    }

    /* Create an object using the encoded private key. */
    if( 0 == xResult )
    {
        xResult = pxP11FunctionList->C_CreateObject(
            xP11Session,
            xPrivateKeyTemplate,
            sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
            &xP11Object );
    }

    /* Free the private key. */
    if( 0 == xResult )
    {
        xResult = pxP11FunctionList->C_DestroyObject(
            xP11Session,
            xP11Object );
        xP11Object = NULL;
    }

    /* Create an object using the encoded certificate. */
    if( 0 == xResult )
    {
        xResult = pxP11FunctionList->C_CreateObject(
            xP11Session,
            xCertificateTemplate,
            sizeof( xCertificateTemplate ) / sizeof( CK_ATTRIBUTE ),
            &xP11Object );
    }

    /* Free the certificate. */
    if( 0 == xResult )
    {
        xResult = pxP11FunctionList->C_DestroyObject(
            xP11Session,
            xP11Object );
        xP11Object = NULL;
    }

    /* Cleanup PKCS#11. */
    if( ( NULL != pxP11FunctionList ) &&
        ( NULL != pxP11FunctionList->C_CloseSession ) )
    {
        pxP11FunctionList->C_CloseSession( xP11Session );
        pxP11FunctionList->C_Finalize( NULL );
    }
}
/*-----------------------------------------------------------*/

/**
 * @brief Starts a task to import a certificate and private key.
 */
void vStartImportCertificateAndKeyTask( uint16_t usTaskStackSize,
                                        UBaseType_t uxTaskPriority )
{
    xTaskCreate(
        prvImportCredentialsTask, /* The function that implements the task. */
        "Import",                 /* Just a text name for the task to aid debugging. */
        usTaskStackSize,          /* The stack size is defined in FreeRTOSIPConfig.h. */
        NULL,                     /* The task parameter, not used in this case. */
        uxTaskPriority,           /* The priority assigned to the task is defined in FreeRTOSConfig.h. */
        NULL );                   /* The task handle is not used. */
}
/*-----------------------------------------------------------*/
