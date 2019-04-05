/*
 * Amazon FreeRTOS PKCS #11 PAL for CC3220SF-LAUNCHXL V1.0.3
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
#include "aws_secure_sockets_config.h"
#include "aws_pkcs11_config.h"
#include "aws_pkcs11_pal.h"

/* flash driver includes. */
#include <ti/drivers/net/wifi/simplelink.h>

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/* mbedTLS includes. */
#include "mbedtls/pk.h"
#include "mbedtls/base64.h"
#include "mbedtls/platform.h"

enum eObjectHandles
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey,
    eAwsDeviceCertificate,
    eAwsCodeSigningKey,
    eAwsTrustedServerCertificate,
    eAwsJITPCertificate
};

/**
 * @brief Save file to flash
 */
static BaseType_t prvSaveFile( char * pcFileName,
                               char * pucData,
                               uint32_t ulDataSize );

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

    if( lFileHdl >= 0 )
    {
        xCheck = sl_FsClose( lFileHdl, NULL, NULL, 0 );
    }

    if( xCheck < 0 )
    {
        xResult = pdFAIL;
    }

    return xResult;
}

#define BEGIN_EC_PRIVATE_KEY     "-----BEGIN EC PRIVATE KEY-----\n"
#define END_EC_PRIVATE_KEY       "-----END EC PRIVATE KEY-----\n"
#define BEGIN_RSA_PRIVATE_KEY    "-----BEGIN RSA PRIVATE KEY-----\n"
#define END_RSA_PRIVATE_KEY      "-----END RSA PRIVATE KEY-----\n"
#define BEGIN_CERTIFICATE        "-----BEGIN CERTIFICATE-----\n"
#define END_CERTIFICATE          "-----END CERTIFICATE-----\n"
#define NUM_CHAR_PER_PEM_LINE    64

/*
 * @brief Converts DER objects to PEM objects.
 *
 * \note Only elliptic curve and RSA private keys are supported.
 *
 * \param[in]   pDerBuffer      A pointer the DER object.
 * \param[in]   xDerLength      The length of the DER object (bytes)
 * \param[out]  ppcPemBuffer    A pointer to the buffer that will be allocated
 *                              for the PEM object.  This function performs
 *                              a memory allocation for this buffer, and the
 *                              caller MUST free the buffer.
 * \param[out]  pPemLength      Length of the PEM object
 * \param[in]   xObjectType     Type of object being converted to PEM.
 *                              Valid values are CKO_PRIVATE_KEY and
 *                              CKO_CERTIFICATE.
 */
static CK_RV prvDerToPem( uint8_t * pDerBuffer,
                          size_t xDerLength,
                          char ** ppcPemBuffer,
                          size_t * pPemLength,
                          CK_OBJECT_CLASS xObjectType )
{
    CK_RV xReturn = CKR_OK;
    size_t xTotalPemLength = 0;
    mbedtls_pk_context pk;
    mbedtls_pk_type_t xKeyType;
    char * pcHeader;
    char * pcFooter;
    unsigned char * pemBodyBuffer;
    uint8_t * pFinalBufferPlaceholder;
    int ulLengthOfContentsCopiedSoFar = 0;
    int ulBytesInLine = 0;
    int ulBytesRemaining;

    if( xObjectType == CKO_PRIVATE_KEY )
    {
        mbedtls_pk_init( &pk );
        /* Parse key. */
        xReturn = mbedtls_pk_parse_key( &pk, pDerBuffer, xDerLength, NULL, 0 );

        if( xReturn != 0 )
        {
            xReturn = CKR_ATTRIBUTE_VALUE_INVALID;
        }

        /* Get key algorithm. */
        xKeyType = mbedtls_pk_get_type( &pk );

        switch( xKeyType )
        {
            case ( MBEDTLS_PK_RSA ):
            case ( MBEDTLS_PK_RSA_ALT ):
                pcHeader = BEGIN_RSA_PRIVATE_KEY;
                pcFooter = END_RSA_PRIVATE_KEY;
                xTotalPemLength = strlen( BEGIN_RSA_PRIVATE_KEY ) + strlen( END_RSA_PRIVATE_KEY );
                break;

            case ( MBEDTLS_PK_ECKEY ):
            case ( MBEDTLS_PK_ECKEY_DH ):
            case ( MBEDTLS_PK_ECDSA ):
                pcHeader = BEGIN_EC_PRIVATE_KEY;
                pcFooter = END_EC_PRIVATE_KEY;
                xTotalPemLength = strlen( BEGIN_EC_PRIVATE_KEY ) + strlen( END_EC_PRIVATE_KEY );
                break;

            default:
                xReturn = CKR_ATTRIBUTE_VALUE_INVALID;
                break;
        }

        mbedtls_pk_free( &pk );
    }
    else /* Certificate object. */
    {
        pcHeader = BEGIN_CERTIFICATE;
        pcFooter = END_CERTIFICATE;
        xTotalPemLength = strlen( BEGIN_CERTIFICATE ) + strlen( END_CERTIFICATE );
    }

    if( xReturn == CKR_OK )
    {
        /* A PEM object has a header, body, and footer. */

        /*
         * ------- BEGIN SOMETHING --------\n
         * BODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODY\n
         * BODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODY\n
         * BODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODYBODY\n
         * ....
         * ------- END SOMETHING ---------\n
         */

        /* Determine the length of the Base 64 encoded body. */
        xReturn = mbedtls_base64_encode( NULL, 0, pPemLength, pDerBuffer, xDerLength );

        if( ( int32_t ) xReturn != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL )
        {
            xReturn = CKR_ATTRIBUTE_VALUE_INVALID;
        }
        else
        {
            xReturn = 0;
        }

        if( xReturn == 0 )
        {
            /* Allocate memory for the PEM contents (excluding header, footer, newlines). */
            pemBodyBuffer = pvPortMalloc( *pPemLength );

            if( pemBodyBuffer == NULL )
            {
                xReturn = CKR_DEVICE_MEMORY;
            }
        }

        if( xReturn == 0 )
        {
            /* Convert the body contents from DER to PEM. */
            xReturn = mbedtls_base64_encode( pemBodyBuffer,
                                             *pPemLength,
                                             pPemLength,
                                             pDerBuffer,
                                             xDerLength );
        }

        if( xReturn == 0 )
        {
            /* Calculate the length required for the entire PEM object. */
            xTotalPemLength += *pPemLength;
            /* Add in space for the newlines. */
            xTotalPemLength += ( *pPemLength ) / NUM_CHAR_PER_PEM_LINE;

            if( ( *pPemLength ) % NUM_CHAR_PER_PEM_LINE != 0 )
            {
                xTotalPemLength += 1;
            }

            /* Allocate space for the full PEM certificate, including header, footer, and newlines.
             * This space must be freed by the application. */
            *ppcPemBuffer = pvPortMalloc( xTotalPemLength );

            if( *ppcPemBuffer == NULL )
            {
                xReturn = CKR_DEVICE_MEMORY;
            }
        }

        if( xReturn == 0 )
        {
            /* Copy the header. */
            pFinalBufferPlaceholder = ( uint8_t * ) *ppcPemBuffer;
            memcpy( pFinalBufferPlaceholder, pcHeader, strlen( pcHeader ) );
            pFinalBufferPlaceholder += strlen( pcHeader );

            /* Copy the Base64 encoded contents into the final buffer 64 bytes at a time, adding newlines */
            while( ulLengthOfContentsCopiedSoFar < *pPemLength )
            {
                ulBytesRemaining = *pPemLength - ulLengthOfContentsCopiedSoFar;
                ulBytesInLine = ( ulBytesRemaining > NUM_CHAR_PER_PEM_LINE ) ? NUM_CHAR_PER_PEM_LINE : ulBytesRemaining;
                memcpy( pFinalBufferPlaceholder,
                        pemBodyBuffer + ulLengthOfContentsCopiedSoFar,
                        ulBytesInLine );
                pFinalBufferPlaceholder += ulBytesInLine;
                *pFinalBufferPlaceholder = '\n';
                pFinalBufferPlaceholder++;

                ulLengthOfContentsCopiedSoFar += ulBytesInLine;
            }
        }

        if( pemBodyBuffer != NULL )
        {
            vPortFree( pemBodyBuffer );
        }

        /* Copy the footer. */
        memcpy( pFinalBufferPlaceholder, pcFooter, strlen( pcFooter ) );

        /* Update the total length of the PEM object returned by the function. */
        *pPemLength = xTotalPemLength;
    }

    return xReturn;
}

void prvHandleToFileName( CK_OBJECT_HANDLE pxHandle,
                          char ** pcFileName )
{
    switch( pxHandle )
    {
        case ( eAwsDeviceCertificate ):
            *pcFileName = socketsconfigSECURE_FILE_NAME_CLIENTCERT;
            break;

        case ( eAwsDevicePrivateKey ):
            *pcFileName = socketsconfigSECURE_FILE_NAME_PRIVATEKEY;
            break;

        case ( eAwsTrustedServerCertificate ):
            *pcFileName = socketsconfigSECURE_FILE_NAME_ROOTCA;
            break;

        default:
            *pcFileName = NULL;
            break;
    }
}

/*-----------------------------------------------------------*/

/* PKCS #11 PAL Implementation. */

/**
 * @brief Delete an object from non-volatile storage.
 *
 * @param[in] xHandle    Handle of the object to be destroyed.
 *
 * @return CKR_OK if object was successfully destroyed.
 * Otherwise, PKCS #11-style error code.
 *
 */
CK_RV PKCS11_PAL_DestroyObject( CK_OBJECT_HANDLE xHandle )
{
    char * pcFileName;
    CK_RV xResult;

    prvHandleToFileName( xHandle, &pcFileName );

    if( 0 == sl_FsDel( ( const unsigned char * )pcFileName, 0 ) )
    {
        xResult = CKR_OK;
    }
    else
    {
        xResult = CKR_OBJECT_HANDLE_INVALID;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

/**
 * @brief Translates a PKCS #11 label into an object handle.
 *
 * Port-specific object handle retrieval.
 *
 *
 * @param[in] pxTemplate     Pointer to the template of the object
 *                           who's handle should be found.
 * @param[in] usLength       The length of the label, in bytes.
 *
 * @return The object handle if operation was successful.
 * Returns eInvalidHandle if unsuccessful.
 */

CK_OBJECT_HANDLE PKCS11_PAL_FindObject( SearchableAttributes_t * pxTemplate )
{
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    char * pcFileName = NULL;
    int16_t iStatus = 0;
    SlFsFileInfo_t FsFileInfo = { 0 };

    /* Converts a label to its respective filename and handle. */
    prvLabelToFilenameHandle( pxTemplate->cLabel,
                              &pcFileName,
                              &xHandle );

    /* Check if object exists/has been created before returning. */
    iStatus = sl_FsGetInfo( ( const unsigned char * )pcFileName, 0, &FsFileInfo );
    if( SL_ERROR_FS_FILE_NOT_EXISTS == iStatus )
    {
        xHandle = eInvalidHandle;
    }

    return xHandle;
}

/*-----------------------------------------------------------*/

/**
 * @brief Gets the value of an object in storage, by handle.
 *
 * Port-specific file access for cryptographic information.
 *
 * This call dynamically allocates the buffer which object value
 * data is copied into.  PKCS11_PAL_GetObjectValueCleanup()
 * should be called after each use to free the dynamically allocated
 * buffer.
 *
 * @sa PKCS11_PAL_GetObjectValueCleanup
 *
 * @param[in] pcFileName    The name of the file to be read.
 * @param[out] ppucData     Pointer to buffer for file data.
 * @param[out] pulDataSize  Size (in bytes) of data located in file.
 * @param[out] pIsPrivate   Boolean indicating if value is private (CK_TRUE)
 *                          or exportable (CK_FALSE)
 *
 * @return CKR_OK if operation was successful.  CKR_KEY_HANDLE_INVALID if
 * no such object handle was found, CKR_DEVICE_MEMORY if memory for
 * buffer could not be allocated, CKR_FUNCTION_FAILED for device driver
 * error.
 */
BaseType_t PKCS11_PAL_GetObjectValue( CK_OBJECT_HANDLE xHandle,
                                      uint8_t ** ppucData,
                                      uint32_t * pulDataSize,
                                      CK_BBOOL * xIsPrivate )
{
    CK_RV ulReturn = CKR_OK;
    uint32_t ulDriverReturn = 0;
    int32_t iReadBytes = 0;
    int32_t iFile = 0;
    char * pcFileName = NULL;
    SlFsFileInfo_t FsFileInfo = { 0 };

    prvHandleToFileName( xHandle, &pcFileName );

    if( xHandle == eAwsDevicePrivateKey )
    {
        *pIsPrivate = CK_TRUE;
    }
    else
    {
        *pIsPrivate = CK_FALSE;
    }

    if( pcFileName == NULL )
    {
        ulReturn = CKR_FUNCTION_FAILED;
    }

    if( 0 == ulReturn )
    {
        /* Check if the file exists and, if so, how big it is. */
        if( 0 != sl_FsGetInfo( ( const unsigned char * )pcFileName, 0, &FsFileInfo ) )
        {
            ulReturn = CKR_FUNCTION_FAILED;
        }
    }

    if( 0 == ulReturn )
    {
        /* Create a buffer. */
        *pulDataSize = FsFileInfo.Len;
        *ppucData = pvPortMalloc( *pulDataSize );
        if( NULL == *ppucData )
        {
            ulReturn = CKR_DEVICE_MEMORY;
        }
    }

    if( 0 == ulReturn )
    {
        /* Open the file. */
        iFile = sl_FsOpen( ( const unsigned char * )pcFileName, SL_FS_READ, 0 );
        if( iFile < 0 )
        {
            ulReturn = CKR_FUNCTION_FAILED;
        }
    }

    if( 0 == ulReturn )
    {
        /* Read the file. */
        iReadBytes = sl_FsRead( iFile, 0, *ppucData, *pulDataSize );
        if( iReadBytes != *pulDataSize )
        {
            ulReturn = CKR_FUNCTION_FAILED;
        }
    }

    if( 0 != iFile )
    {
        sl_FsClose( iFile, 0, 0 );
    }

    return ( BaseType_t )ulReturn;
}

/*-----------------------------------------------------------*/

/**
 * @brief Cleanup after PKCS11_GetObjectValue().
 *
 * @param[in] pucData       The buffer to free.
 *                          (*ppucData from PKCS11_PAL_GetObjectValue())
 * @param[in] ulDataSize    The length of the buffer to free.
 *                          (*pulDataSize from PKCS11_PAL_GetObjectValue())
 */
void PKCS11_PAL_GetObjectValueCleanup( uint8_t * pucData,
                                       uint32_t ulDataSize )
{
    /* Unused parameters. */
    ( void ) ulDataSize;

    if( NULL != pucData )
    {
        vPortFree( pucData );
    }
}

/*-----------------------------------------------------------*/

#if 0
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

    mbedtls_platform_set_calloc_free( prvCalloc, vPortFree ); /*lint !e534 This function always return 0. */

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
    PKCS11_KeyTemplatePtr_t pxKeyTemplate = NULL;
    PKCS11_CertificateTemplatePtr_t pxCertificateTemplate = NULL;
    uint8_t * pucFileName = NULL;
    uint8_t * pcPemBuffer = NULL;
    size_t pemLength;

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

                pxCertificateTemplate = ( PKCS11_CertificateTemplatePtr_t ) pxTemplate;

                /* Validate the attribute count for this object class. */
                if( sizeof( PKCS11_CertificateTemplate_t ) / sizeof( CK_ATTRIBUTE ) != ulCount )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                /* Validate the attribute template. */
                if( ( CKA_VALUE != pxCertificateTemplate->xValue.type ) ||
                    ( CKA_LABEL != pxCertificateTemplate->xLabel.type ) ||
                    ( NULL == pxCertificateTemplate->xValue.pValue ) )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                /* Convert to PEM. */
                prvDerToPem( pxCertificateTemplate->xValue.pValue,
                             pxCertificateTemplate->xValue.ulValueLen,
                             ( char ** ) &pcPemBuffer,
                             &pemLength,
                             CKO_CERTIFICATE );

                if( 0 == memcmp( pxCertificateTemplate->xLabel.pValue, pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, strlen( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
                {
                    pucFileName = socketsconfigSECURE_FILE_NAME_CLIENTCERT;
                }
                else if( 0 == memcmp( pxCertificateTemplate->xLabel.pValue, pkcs11configLABEL_ROOT_CERTIFICATE, strlen( pkcs11configLABEL_ROOT_CERTIFICATE ) ) )
                {
                    pucFileName = socketsconfigSECURE_FILE_NAME_ROOTCA;
                }
                else
                {
                    pucFileName = pxCertificateTemplate->xLabel.pValue;
                }

                /* Write out the device certificate. */
                if( pdFALSE == prvSaveFile( ( char * ) pucFileName,
                                            ( char * ) pcPemBuffer,
                                            pemLength ) )
                {
                    xResult = CKR_DEVICE_ERROR;
                    break;
                }

                vPortFree( pcPemBuffer );

                /* Create a certificate handle to return. */
                if( CKR_OK == xResult )
                {
                    pxObject = NULL;
                }

                break;

            case CKO_PRIVATE_KEY:

                /* Cast the template for easy field access. */
                pxKeyTemplate = ( PKCS11_KeyTemplatePtr_t ) pxTemplate;

                /* Validate the attribute count for this object class. */
                if( ( sizeof( PKCS11_KeyTemplate_t ) / sizeof( CK_ATTRIBUTE ) ) != ulCount )
                {
                    xResult = CKR_ARGUMENTS_BAD;
                    break;
                }

                /* Confirm that the template is formatted as expected for a private key. */
                if( ( CKA_VALUE != pxKeyTemplate->xValue.type ) ||
                    ( CKA_KEY_TYPE != pxKeyTemplate->xKeyType.type ) ||
                    ( CKA_LABEL != pxKeyTemplate->xLabel.type ) )
                {
                    xResult = CKR_ATTRIBUTE_TYPE_INVALID;
                    break;
                }

                prvDerToPem( pxKeyTemplate->xValue.pValue,
                             pxKeyTemplate->xValue.ulValueLen,
                             ( char ** ) &pcPemBuffer,
                             &pemLength,
                             CKO_PRIVATE_KEY );

                /* Write out the key. */
                if( pdFALSE == prvSaveFile( socketsconfigSECURE_FILE_NAME_PRIVATEKEY,
                                            ( char * ) pcPemBuffer,
                                            pemLength ) )
                {
                    xResult = CKR_DEVICE_ERROR;
                    break;
                }

                vPortFree( pcPemBuffer );

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
#endif
