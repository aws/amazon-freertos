/*
 * Amazon FreeRTOS PKCS#11 for CC3220SF-LAUNCHXL V1.0.2
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
 * @file aws_pkcs11_pal.c
 * @brief PKCS#11 implementation for CC3220SF-LAUNCHXL. This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS#11 standard.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "task.h"
#include "aws_pkcs11.h"
#include "aws_clientcredential.h"

/* flash driver includes. */
#include <ti/drivers/net/wifi/simplelink.h>

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/**
 * @brief File storage location definitions.
 */
#define pkcs11FILE_NAME_CLIENT_CERTIFICATE      "/certs/ClientCert.crt"
#define pkcs11FILE_NAME_KEY                     "/certs/PrivateKey.key"
#define pkcs11FILE_NAME_KEY_ROOT_CERTIFICATE    "/certs/RootCA.crt"


/**
 * @brief Cryptoki module attribute definitions.
 */
#define pkcs11SLOT_ID    1

/**
 * @brief Key structure.
 */
typedef struct P11Key
{
  /* IAR complains about the empty structure. */
  uint8_t ucFakeValue;
} P11Key_t, * P11KeyPtr_t;

/**
 * @brief Session structure.
 */
typedef struct P11Session
{
    P11KeyPtr_t pxCurrentKey;
    CK_ULONG ulState;
    CK_BBOOL xOpened;
    CK_BBOOL xFindObjectInit;
    CK_BBOOL xFindObjectComplete;
    CK_OBJECT_CLASS xFindObjectClass;
} P11Session_t, * P11SessionPtr_t;

/**
 * @brief Save file to flash
 */
static BaseType_t prvSaveFile( char * pcFileName,
                               char * pucData,
                               uint32_t ulDataSize );

/**
 * @brief Maps an opaque caller session handle into its internal state structure.
 */
static P11SessionPtr_t prvSessionPointerFromHandle( CK_SESSION_HANDLE xSession )
{
    return ( P11SessionPtr_t ) xSession; /*lint !e923 Allow casting integer type to pointer for handle. */
}

/*-----------------------------------------------------------*/

static BaseType_t prvSaveFile( char * pcFileName,
                               char * pucData,
                               uint32_t ulDataSize )
{
    CK_RV xResult = pdPASS;
    int32_t lFileHdl;
    unsigned long ulMasterToken = 0;
    const uint16_t usMaxSize = 2000;
    BaseType_t xCheck;

    lFileHdl = sl_FsOpen( ( const unsigned char * ) pcFileName,
                          SL_FS_CREATE | SL_FS_CREATE_NOSIGNATURE | SL_FS_OVERWRITE | SL_FS_CREATE_MAX_SIZE( usMaxSize ), /*lint !e9029 !e9027  offending macros in a TI file. */
                          &ulMasterToken );

    if( lFileHdl < 0 )
    {
        xResult = pdFAIL;
        configPRINTF( ( "Failed creating file %s...\r\n", pcFileName ) );
    }

    if( xResult == pdPASS )
    {
        xCheck = sl_FsWrite( lFileHdl,
                             0U,
                             ( unsigned char * ) pucData, /*lint !e605 cert bytes must be cast as unsigned for compliance with TI file. */
                             ulDataSize );

        if( xCheck < 0 )
        {
            xResult = pdFAIL;
        }
    }

    /* Does the file need to be closed if the open fails? */
    xCheck = sl_FsClose( lFileHdl, NULL, NULL, 0 );

    if( xCheck < 0 )
    {
        xResult = pdFAIL;
    }

    return xResult;
}

/*
 * PKCS#11 module implementation.
 */

/**
 * @brief PKCS#11 interface functions implemented by this Cryptoki module.
 */
static CK_FUNCTION_LIST prvP11FunctionList =
{
    { CRYPTOKI_VERSION_MAJOR, CRYPTOKI_VERSION_MINOR },
    C_Initialize,
    C_Finalize,
    NULL,
    C_GetFunctionList,
    C_GetSlotList,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    C_OpenSession,
    C_CloseSession,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    C_CreateObject,
    NULL,
    C_DestroyObject,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

/**
 * @brief Initialize the Cryptoki module for use.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Initialize )( CK_VOID_PTR pvInitArgs )
{   /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( pvInitArgs );

    return CKR_OK;
}

/**
 * @brief Un-initialize the Cryptoki module.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Finalize )( CK_VOID_PTR pvReserved )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;

    if( NULL != pvReserved )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    return xResult;
}

/**
 * @brief Query the list of interface function pointers.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetFunctionList )( CK_FUNCTION_LIST_PTR_PTR ppxFunctionList )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;

    if( NULL == ppxFunctionList )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    else
    {
        *ppxFunctionList = &prvP11FunctionList;
    }

    return xResult;
}

/**
 * @brief Query the list of slots. A single default slot is implemented.
 */
CK_DEFINE_FUNCTION( CK_RV, C_GetSlotList )( CK_BBOOL xTokenPresent,
                                            CK_SLOT_ID_PTR pxSlotList,
                                            CK_ULONG_PTR pulCount )
{   /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( xTokenPresent );

    if( NULL == pxSlotList )
    {
        *pulCount = 1;
    }
    else
    {
        if( 0u == *pulCount )
        {
            return CKR_BUFFER_TOO_SMALL;
        }

        pxSlotList[ 0 ] = pkcs11SLOT_ID;
        *pulCount = 1;
    }

    return CKR_OK;
}


/**
 * @brief Start a session for a cryptographic command sequence.
 */
CK_DEFINE_FUNCTION( CK_RV, C_OpenSession )( CK_SLOT_ID xSlotID,
                                            CK_FLAGS xFlags,
                                            CK_VOID_PTR pvApplication,
                                            CK_NOTIFY xNotify,
                                            CK_SESSION_HANDLE_PTR pxSession )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSessionObj = NULL;

    ( void ) ( xSlotID );
    ( void ) ( pvApplication );
    ( void ) ( xNotify );

    /* Check arguments. */
    if( NULL == pxSession )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    /* For legacy reasons, the CKF_SERIAL_SESSION bit MUST always be set. */
    if( 0 == ( CKF_SERIAL_SESSION & xFlags ) )
    {
        xResult = CKR_SESSION_PARALLEL_NOT_SUPPORTED;
    }

    /*
     * Make space for the context.
     */
    if( CKR_OK == xResult )
    {
        pxSessionObj = ( P11SessionPtr_t ) pvPortMalloc( sizeof( P11Session_t ) ); /*lint !e9087 Allow casting void* to other types. */

        if( NULL == pxSessionObj )
        {
            xResult = CKR_HOST_MEMORY;
        }
    }

    if( CKR_OK == xResult )
    {
        /*
         * Assign the session.
         */

        pxSessionObj->ulState =
            0u != ( xFlags & CKF_RW_SESSION ) ? CKS_RW_PUBLIC_SESSION : CKS_RO_PUBLIC_SESSION;
        pxSessionObj->xOpened = CK_TRUE;

        /*
         * Return the session.
         */

        *pxSession = ( CK_SESSION_HANDLE ) pxSessionObj; /*lint !e923 Allow casting pointer to integer type for handle. */
    }

    if( ( NULL != pxSessionObj ) && ( CKR_OK != xResult ) )
    {
        vPortFree( pxSessionObj );
    }

    return xResult;
}

/**
 * @brief Terminate a session and release resources.
 */
CK_DEFINE_FUNCTION( CK_RV, C_CloseSession )( CK_SESSION_HANDLE xSession )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;
    P11SessionPtr_t pxSession = prvSessionPointerFromHandle( xSession );

    if( NULL != pxSession )
    {
        /*
         * Tear down the session.
         */

        vPortFree( pxSession );
    }

    return xResult;
}

/**
 * @brief Import a private key, client certificate, or root certificate.
 */
CK_DEFINE_FUNCTION( CK_RV, C_CreateObject )( CK_SESSION_HANDLE xSession,
                                             CK_ATTRIBUTE_PTR pxTemplate,
                                             CK_ULONG ulCount,
                                             CK_OBJECT_HANDLE_PTR pxObject )
{   /*lint !e9072 It's OK to have different parameter name. */
    CK_RV xResult = CKR_OK;

    /*
     * Check parameters.
     */

    if( ( 2 > ulCount ) ||
        ( NULL == pxTemplate ) ||
        ( NULL == pxObject ) )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    if( CKR_OK == xResult )
    {
        if( ( CKA_CLASS != pxTemplate[ 0 ].type ) ||
            ( sizeof( CK_OBJECT_CLASS ) != pxTemplate[ 0 ].ulValueLen ) )
        {
            xResult = CKR_ARGUMENTS_BAD;
        }
    }

    /*
     * Handle the object by class.
     */

    if( CKR_OK == xResult )
    {
        switch( *( ( uint32_t * ) pxTemplate[ 0 ].pValue ) )
        {
            case CKO_CERTIFICATE:

                /* Validate the attribute count for this object class. */
                if( 3 != ulCount )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                /* Validate the next attribute type. */
                if( CKA_VALUE )
                {
                    if( CKA_VALUE != pxTemplate[ 1 ].type )
                    {
                        xResult = CKR_ARGUMENTS_BAD;
                        break;
                    }
                }

                /* Validate the next attribute type. */
                if( CKA_CERTIFICATE_TYPE )
                {
                    if( CKA_CERTIFICATE_TYPE != pxTemplate[ 2 ].type )
                    {
                        xResult = CKR_ARGUMENTS_BAD;
                        break;
                    }
                }

                if( *( ( uint32_t * ) pxTemplate[ 2 ].pValue ) == pkcs11CERTIFICATE_TYPE_USER )
                {
                    /* Write out the device certificate. */
                    if( pdFALSE == prvSaveFile( pkcs11FILE_NAME_CLIENT_CERTIFICATE,
                                                pxTemplate[ 1 ].pValue,
                                                pxTemplate[ 1 ].ulValueLen - 1 ) )
                    {
                        xResult = CKR_DEVICE_ERROR;
                        break;
                    }
                }
                else if( *( ( uint32_t * ) pxTemplate[ 2 ].pValue ) == pkcs11CERTIFICATE_TYPE_ROOT )
                {
                    /* Write out the root certificate. */
                    if( pdFALSE == prvSaveFile( pkcs11FILE_NAME_KEY_ROOT_CERTIFICATE,
                                                pxTemplate[ 1 ].pValue,
                                                pxTemplate[ 1 ].ulValueLen - 1 ) )
                    {
                        xResult = CKR_DEVICE_ERROR;
                        break;
                    }
                }

                /* Create a certificate handle to return. */
                if( CKR_OK == xResult )
                {
                    pxObject = NULL;
                }

                break;

            case CKO_PRIVATE_KEY:

                /* Validate the attribute count for this object class. */
                if( 4 != ulCount )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                /* Find the key bytes. */
                if( CKA_VALUE )
                {
                    if( CKA_VALUE != pxTemplate[ 3 ].type )
                    {
                        xResult = CKR_ARGUMENTS_BAD;
                        break;
                    }
                }

                /* Write out the key. */
                if( pdFALSE == prvSaveFile( pkcs11FILE_NAME_KEY,
                                            pxTemplate[ 3 ].pValue,
                                            pxTemplate[ 3 ].ulValueLen - 1 ) )
                {
                    xResult = CKR_DEVICE_ERROR;
                    break;
                }

                /* Create a key handle to return. */
                if( CKR_OK == xResult )
                {
                    pxObject = NULL;
                }

                break;

            default:
                xResult = CKR_ARGUMENTS_BAD;
        }
    }

    return xResult;
}

/**
 * @brief Not supported by this Cryptoki module.
 */
CK_DEFINE_FUNCTION( CK_RV, C_DestroyObject )( CK_SESSION_HANDLE xSession,
                                              CK_OBJECT_HANDLE xObject )
{   /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( xSession );
    ( void ) ( xObject );

    /*
     * This implementation uses virtual handles, and the certificate and
     * private key data are attached to the session, so nothing to do here.
     */
    return CKR_OK;
}
