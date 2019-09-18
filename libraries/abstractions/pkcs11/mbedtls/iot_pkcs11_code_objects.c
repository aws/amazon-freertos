/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "iot_pkcs11_config.h"
#include "iot_pkcs11.h"
#include "iot_pki_utils.h"
#include "iot_pkcs11_code_objects.h"


/* Credential includes. */
#include "aws_clientcredential.h"
#include "aws_clientcredential_keys.h"
#include "iot_default_root_certificates.h"


/* C runtime includes. */
#include <stdio.h>
#include <string.h>

#if ( pkcs11configOTA_SUPPORTED == 1 )
    #include "aws_ota_codesigner_certificate.h"
#endif



enum eObjectHandles
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eJITPCertificate = 1,
    eRootCertificate = 2,
    eAwsCodeSigningKey = 3
};

extern int convert_pem_to_der( const unsigned char * pucInput,
                               size_t xLen,
                               unsigned char * pucOutput,
                               size_t * pxOlen );

void prvLookupObjectByHandle( CK_OBJECT_HANDLE xHandle,
                              uint8_t ** ppucCertData,
                              size_t * pCertLength )
{
    *ppucCertData = NULL;
    *pCertLength = 0;

    if( xHandle == eJITPCertificate )
    {
        if( NULL != keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM )
        {
            *ppucCertData = ( uint8_t * ) keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM;
            *pCertLength = sizeof( keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM );
        }
        else
        {
            configPRINTF( ( "ERROR: JITP Certificate not specified.\r\n" ) );
        }
    }
    else if( xHandle == eRootCertificate )
    {
        /* Use either Verisign or Starfield root CA,
         * depending on whether this is an ATS endpoint. */
        if( strstr( clientcredentialMQTT_BROKER_ENDPOINT, "-ats.iot" ) == NULL )
        {
            *ppucCertData = ( uint8_t * ) tlsVERISIGN_ROOT_CERTIFICATE_PEM;
            *pCertLength = tlsVERISIGN_ROOT_CERTIFICATE_LENGTH;
        }
        else
        {
            *ppucCertData = ( uint8_t * ) tlsSTARFIELD_ROOT_CERTIFICATE_PEM;
            *pCertLength = tlsSTARFIELD_ROOT_CERTIFICATE_LENGTH;
        }
    }

    #if ( pkcs11configOTA_SUPPORTED == 1 )
        else if( 0 == memcmp( pucLabel, pkcs11configLABEL_CODE_VERIFICATION_KEY, strlen( ( char * ) pkcs11configLABEL_CODE_VERIFICATION_KEY ) ) )
        {
            *ppucCertData = ( uint8_t * ) signingcredentialSIGNING_CERTIFICATE_PEM;
            *pCertLength = sizeof( signingcredentialSIGNING_CERTIFICATE_PEM );
            xPalHandle = eAwsCodeSigningKey;
        }
    #endif
}


CK_OBJECT_HANDLE prvLookupObjectByLabel( uint8_t * pucLabel,
                                         size_t xLength,
                                         uint8_t ** ppucCertData,
                                         size_t * pCertLength )
{
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;

    if( 0 == memcmp( pucLabel, pkcs11configLABEL_JITP_CERTIFICATE, strlen( ( char * ) pkcs11configLABEL_JITP_CERTIFICATE ) ) )
    {
        if( NULL != keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM )
        {
            *ppucCertData = ( uint8_t * ) keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM;
            *pCertLength = sizeof( keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM );
            xPalHandle = eJITPCertificate;
        }
        else
        {
            configPRINTF( ( "ERROR: JITP Certificate not specified.\r\n" ) );
        }
    }
    else if( 0 == memcmp( pucLabel, pkcs11configLABEL_ROOT_CERTIFICATE, strlen( ( char * ) pkcs11configLABEL_ROOT_CERTIFICATE ) ) )
    {
        /* Use either Verisign or Starfield root CA,
         * depending on whether this is an ATS endpoint. */
        if( strstr( clientcredentialMQTT_BROKER_ENDPOINT, "-ats.iot" ) == NULL )
        {
            *ppucCertData = ( uint8_t * ) tlsVERISIGN_ROOT_CERTIFICATE_PEM;
            *pCertLength = tlsVERISIGN_ROOT_CERTIFICATE_LENGTH;
        }
        else
        {
            *ppucCertData = ( uint8_t * ) tlsSTARFIELD_ROOT_CERTIFICATE_PEM;
            *pCertLength = tlsSTARFIELD_ROOT_CERTIFICATE_LENGTH;
        }

        xPalHandle = eRootCertificate;
    }

    #if ( pkcs11configOTA_SUPPORTED == 1 )
        else if( 0 == memcmp( pucLabel, pkcs11configLABEL_CODE_VERIFICATION_KEY, strlen( ( char * ) pkcs11configLABEL_CODE_VERIFICATION_KEY ) ) )
        {
            *ppucCertData = ( uint8_t * ) signingcredentialSIGNING_CERTIFICATE_PEM;
            *pCertLength = sizeof( signingcredentialSIGNING_CERTIFICATE_PEM );
            xPalHandle = eAwsCodeSigningKey;
        }
    #endif


    return xPalHandle;
}


CK_OBJECT_HANDLE PKCS11_Code_SaveObject( CK_ATTRIBUTE_PTR pxLabel,
                                         uint8_t * pucData,
                                         uint32_t ulDataSize )
{
    CK_OBJECT_HANDLE xPalHandle = CK_INVALID_HANDLE;
    uint8_t * pInCodeObjectData;
    size_t inCodeObjectDataLength;


    xPalHandle = prvLookupObjectByLabel( pxLabel->pValue, pxLabel->ulValueLen, &pInCodeObjectData, &inCodeObjectDataLength );

    if( xPalHandle != CK_INVALID_HANDLE )
    {
        /*TODO: Check that cert being imported is the same as the one already in code. */
    }

    return xPalHandle;
}

CK_OBJECT_HANDLE PKCS11_Code_FindObject( uint8_t * pLabel,
                                         uint8_t usLength )
{
    CK_OBJECT_HANDLE handle;
    uint8_t * pCertificate;
    size_t certLength;

    handle = prvLookupObjectByLabel( pLabel,
                                     usLength,
                                     &pCertificate,
                                     &certLength );

    if( ( pCertificate == "" ) || ( pCertificate == NULL ) || ( certLength == 0 ) )
    {
        handle = CK_INVALID_HANDLE;
    }

    return handle;
}


CK_RV PKCS11_Code_GetObjectValue( CK_OBJECT_HANDLE xPalHandle,
                                  uint8_t ** ppucData,
                                  uint32_t * pulDataSize,
                                  CK_BBOOL * pIsPrivate )
{
    CK_RV xResult = CKR_OK;
    uint8_t * pCertificate;
    uint8_t * beginningOfInputBuffer;
    size_t certLength;
    char pemBegin[] = "-----BEGIN";
    int convertResult = 0;


    prvLookupObjectByHandle( xPalHandle,
                             &pCertificate,
                             &certLength );

    *ppucData = pvPortMalloc( *pulDataSize );
    beginningOfInputBuffer = *ppucData;

    /* If the certificate is in PEM format, convert to DER. */
    if( memcmp( pemBegin, pCertificate, sizeof( pemBegin ) - 1 ) == 0 )
    {
        convertResult = convert_pem_to_der( pCertificate,
                                            certLength,
                                            beginningOfInputBuffer,
                                            pulDataSize );

        /* Conversion places data starting at the end of the buffer. */
        if( convertResult == 0 )
        {
            memmove( *ppucData, beginningOfInputBuffer, *pulDataSize );
        }
        else
        {
            xResult = CKR_OBJECT_HANDLE_INVALID;
        }
    }
    else
    {
        *ppucData = pCertificate;
        *pulDataSize = certLength;
    }

    return xResult;
}

void PKCS11_Code_GetObjectValueCleanup( uint8_t * pucData,
                                        uint32_t ulDataSize )
{
    if( pucData != NULL )
    {
        vPortFree( pucData );
    }
}

CK_RV PKCS11_Code_DestroyObject( CK_OBJECT_HANDLE xHandle )
{
    /* This object cannot be destroyed. */
    CK_RV xResult = CKR_ACTION_PROHIBITED;

    return xResult;
}
