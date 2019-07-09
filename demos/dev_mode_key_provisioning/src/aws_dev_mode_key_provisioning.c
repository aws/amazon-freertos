/*
 * Amazon FreeRTOS Provisioning V1.0.0
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
#include "iot_pkcs11.h"
#include "iot_pkcs11_config.h"

/* Client credential includes. */
#include "aws_clientcredential.h"
#include "aws_clientcredential_keys.h"
#include "iot_default_root_certificates.h"

/* Key provisioning include. */
#include "aws_dev_mode_key_provisioning.h"

/* Utilities include. */
#include "iot_pki_utils.h"

/* mbedTLS includes. */
#include "mbedtls/pk.h"
#include "mbedtls/oid.h"


/*-----------------------------------------------------------*/


CK_RV xProvisionPrivateKey( CK_SESSION_HANDLE xSession,
                            uint8_t * pucPrivateKey,
                            size_t xPrivateKeyLength,
                            uint8_t * pucLabel,
                            CK_OBJECT_HANDLE_PTR pxObjectHandle )
{
    CK_RV xResult = CKR_OK;
    CK_OBJECT_CLASS xPrivateKeyClass = CKO_PRIVATE_KEY;
    CK_FUNCTION_LIST_PTR pxFunctionList;
    CK_BBOOL xTokenStorage = CK_TRUE;
    mbedtls_pk_type_t xMbedKeyType;
    CK_KEY_TYPE xPrivateKeyType;

    xResult = C_GetFunctionList( &pxFunctionList );

    int lMbedResult = 0;
    mbedtls_pk_context xMbedPkContext;

    mbedtls_pk_init( &xMbedPkContext );
    lMbedResult = mbedtls_pk_parse_key( &xMbedPkContext, pucPrivateKey, xPrivateKeyLength, NULL, 0 );

    if( lMbedResult != 0 )
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    /* Determine whether the key to be imported is RSA or EC. */
    if( xResult == CKR_OK )
    {
        xMbedKeyType = mbedtls_pk_get_type( &xMbedPkContext );

        if( xMbedKeyType == MBEDTLS_PK_RSA )
        {
            xPrivateKeyType = CKK_RSA;
        }
        else if( ( xMbedKeyType == MBEDTLS_PK_ECDSA ) || ( xMbedKeyType == MBEDTLS_PK_ECKEY ) || ( xMbedKeyType == MBEDTLS_PK_ECKEY_DH ) )
        {
            xPrivateKeyType = CKK_EC;
        }
        else
        {
            xPrivateKeyType = 0xFFFFFFFF;
            xResult = CKR_ARGUMENTS_BAD;
        }
    }

    /* EC Keys. */
    if( xPrivateKeyType == CKK_EC )
    {
        PKCS11_PrivateEcKeyTemplate_t xPrivateKeyTemplate;
        CK_BYTE * pxD;               /* Private value D. */
        CK_BYTE * pxEcParams = NULL; /* DER-encoding of an ANSI X9.62 Parameters value */

#define EC_PARAMS_LENGTH    10
#define EC_D_LENGTH         32

        pxD = pvPortMalloc( EC_D_LENGTH );

        if( ( pxD == NULL ) )
        {
            xResult = CKR_HOST_MEMORY;
        }

        if( xResult == CKR_OK )
        {
            mbedtls_ecp_keypair * pxKeyPair = ( mbedtls_ecp_keypair * ) xMbedPkContext.pk_ctx;
            lMbedResult = mbedtls_mpi_write_binary( &( pxKeyPair->d ), pxD, EC_D_LENGTH );

            if( pxKeyPair->grp.id == MBEDTLS_ECP_DP_SECP256R1 )
            {
                pxEcParams = ( CK_BYTE * ) ( "\x06\x08" MBEDTLS_OID_EC_GRP_SECP256R1 );
            }
            else
            {
                xResult = CKR_CURVE_NOT_SUPPORTED;
            }

            if( xResult == CKR_OK )
            {
                xPrivateKeyTemplate.xObjectClass.type = CKA_CLASS;
                xPrivateKeyTemplate.xObjectClass.pValue = &xPrivateKeyClass;
                xPrivateKeyTemplate.xObjectClass.ulValueLen = sizeof( xPrivateKeyClass );
                xPrivateKeyTemplate.xKeyType.type = CKA_KEY_TYPE;
                xPrivateKeyTemplate.xKeyType.pValue = &xPrivateKeyType;
                xPrivateKeyTemplate.xKeyType.ulValueLen = sizeof( xPrivateKeyType );
                xPrivateKeyTemplate.xLabel.type = CKA_LABEL;
                xPrivateKeyTemplate.xLabel.pValue = pucLabel;
                xPrivateKeyTemplate.xLabel.ulValueLen = strlen( ( const char * ) pucLabel );
                xPrivateKeyTemplate.xEcParams.type = CKA_EC_PARAMS;
                xPrivateKeyTemplate.xEcParams.pValue = pxEcParams;
                xPrivateKeyTemplate.xEcParams.ulValueLen = EC_PARAMS_LENGTH;
                xPrivateKeyTemplate.xValue.type = CKA_VALUE;
                xPrivateKeyTemplate.xValue.pValue = pxD;
                xPrivateKeyTemplate.xValue.ulValueLen = EC_D_LENGTH;
                xPrivateKeyTemplate.xTokenObject.type = CKA_TOKEN;
                xPrivateKeyTemplate.xTokenObject.pValue = &xTokenStorage;
                xPrivateKeyTemplate.xTokenObject.ulValueLen = sizeof( xTokenStorage );

                xResult = pxFunctionList->C_CreateObject( xSession,
                                                          ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                                                          sizeof( PKCS11_PrivateEcKeyTemplate_t ) / sizeof( CK_ATTRIBUTE ),
                                                          pxObjectHandle );
            }
        }

        if( pxD != NULL )
        {
            vPortFree( pxD );
        }
    }

    /* RSA Keys. */
    if( xPrivateKeyType == CKK_RSA )
    {
        PKCS11_PrivateRsaKeyTemplate_t xPrivateKeyTemplate;
        mbedtls_rsa_context * xRsaContext = xMbedPkContext.pk_ctx;
        CK_BBOOL xTrue = CK_TRUE;
        CK_BYTE * pxModulus;
        CK_BYTE * pxE;
        CK_BYTE * pxD;
        CK_BYTE * pxPrime1;
        CK_BYTE * pxPrime2;
        CK_BYTE * pxExp1;
        CK_BYTE * pxExp2;
        CK_BYTE * pxCoefficient;
#define MODULUS_LENGTH        256
#define E_LENGTH              3
#define D_LENGTH              256
#define PRIME_1_LENGTH        128
#define PRIME_2_LENGTH        128
#define EXPONENT_1_LENGTH     128
#define EXPONENT_2_LENGTH     128
#define COEFFICIENT_LENGTH    128

        /* Adding one to all of the lengths because ASN1 may pad a leading 0 byte
         * to numbers that could be interpreted as negative */
        pxModulus = pvPortMalloc( MODULUS_LENGTH + 1 );
        pxE = pvPortMalloc( E_LENGTH + 1 );
        pxD = pvPortMalloc( D_LENGTH + 1 );
        pxPrime1 = pvPortMalloc( PRIME_1_LENGTH + 1 );
        pxPrime2 = pvPortMalloc( PRIME_2_LENGTH + 1 );
        pxExp1 = pvPortMalloc( EXPONENT_1_LENGTH + 1 );
        pxExp2 = pvPortMalloc( EXPONENT_2_LENGTH + 1 );
        pxCoefficient = pvPortMalloc( COEFFICIENT_LENGTH + 1 );

        if( ( pxModulus == NULL ) || ( pxE == NULL ) || ( pxD == NULL ) || ( pxPrime1 == NULL ) || ( pxPrime2 == NULL ) || ( pxExp1 == NULL ) || ( pxExp2 == NULL ) )
        {
            xResult = CKR_HOST_MEMORY;
        }

        if( xResult == CKR_OK )
        {
            lMbedResult = mbedtls_rsa_export_raw( xRsaContext,
                                                  pxModulus, MODULUS_LENGTH + 1,
                                                  pxPrime1, PRIME_1_LENGTH + 1,
                                                  pxPrime2, PRIME_2_LENGTH + 1,
                                                  pxD, D_LENGTH + 1,
                                                  pxE, E_LENGTH + 1 );

            if( lMbedResult != 0 )
            {
                configPRINTF( ( "Failed to parse RSA private key components. \r\n" ) );
                xResult = CKR_ATTRIBUTE_VALUE_INVALID;
            }

            /* Export Exponent 1, Exponent 2, Coefficient. */
            lMbedResult |= mbedtls_mpi_write_binary( ( mbedtls_mpi const * ) &xRsaContext->DP, pxExp1, EXPONENT_1_LENGTH + 1 );
            lMbedResult |= mbedtls_mpi_write_binary( ( mbedtls_mpi const * ) &xRsaContext->DQ, pxExp2, EXPONENT_2_LENGTH + 1 );
            lMbedResult |= mbedtls_mpi_write_binary( ( mbedtls_mpi const * ) &xRsaContext->QP, pxCoefficient, COEFFICIENT_LENGTH + 1 );

            if( lMbedResult != 0 )
            {
                configPRINTF( ( "Failed to parse RSA private key Chinese Remainder Theorem variables. \r\n" ) );
                xResult = CKR_ATTRIBUTE_VALUE_INVALID;
            }
        }

        if( xResult == CKR_OK )
        {
            /* When importing the fields, the pointer is incremented by 1
             * to remove the leading 0 padding (if it existed) and the original field length is used */
            xPrivateKeyTemplate.xObjectClass.type = CKA_CLASS;
            xPrivateKeyTemplate.xObjectClass.pValue = &xPrivateKeyClass;
            xPrivateKeyTemplate.xObjectClass.ulValueLen = sizeof( xPrivateKeyClass );
            xPrivateKeyTemplate.xKeyType.type = CKA_KEY_TYPE;
            xPrivateKeyTemplate.xKeyType.pValue = &xPrivateKeyType;
            xPrivateKeyTemplate.xKeyType.ulValueLen = sizeof( xPrivateKeyType );
            xPrivateKeyTemplate.xLabel.type = CKA_LABEL;
            xPrivateKeyTemplate.xLabel.pValue = ( CK_VOID_PTR ) pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
            xPrivateKeyTemplate.xLabel.ulValueLen = ( CK_ULONG ) strlen( ( const char * ) pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS );
            xPrivateKeyTemplate.xCanSign.type = CKA_SIGN;
            xPrivateKeyTemplate.xCanSign.pValue = &xTrue;
            xPrivateKeyTemplate.xCanSign.ulValueLen = sizeof( CK_BBOOL );
            xPrivateKeyTemplate.xModulus.type = CKA_MODULUS;
            xPrivateKeyTemplate.xModulus.pValue = pxModulus + 1;
            xPrivateKeyTemplate.xModulus.ulValueLen = MODULUS_LENGTH;
            xPrivateKeyTemplate.xPrivateExponent.type = CKA_PRIVATE_EXPONENT;
            xPrivateKeyTemplate.xPrivateExponent.pValue = pxD + 1;
            xPrivateKeyTemplate.xPrivateExponent.ulValueLen = D_LENGTH;
            xPrivateKeyTemplate.xPublicExponent.type = CKA_PUBLIC_EXPONENT;
            xPrivateKeyTemplate.xPublicExponent.pValue = pxE + 1;
            xPrivateKeyTemplate.xPublicExponent.ulValueLen = E_LENGTH;
            xPrivateKeyTemplate.xPrime1.type = CKA_PRIME_1;
            xPrivateKeyTemplate.xPrime1.pValue = pxPrime1 + 1;
            xPrivateKeyTemplate.xPrime1.ulValueLen = PRIME_1_LENGTH;
            xPrivateKeyTemplate.xPrime2.type = CKA_PRIME_2;
            xPrivateKeyTemplate.xPrime2.pValue = pxPrime2 + 1;
            xPrivateKeyTemplate.xPrime2.ulValueLen = PRIME_2_LENGTH;
            xPrivateKeyTemplate.xExp1.type = CKA_EXPONENT_1;
            xPrivateKeyTemplate.xExp1.pValue = pxExp1 + 1;
            xPrivateKeyTemplate.xExp1.ulValueLen = EXPONENT_1_LENGTH;
            xPrivateKeyTemplate.xExp2.type = CKA_EXPONENT_2;
            xPrivateKeyTemplate.xExp2.pValue = pxExp2 + 1;
            xPrivateKeyTemplate.xExp2.ulValueLen = EXPONENT_2_LENGTH;
            xPrivateKeyTemplate.xCoefficient.type = CKA_COEFFICIENT;
            xPrivateKeyTemplate.xCoefficient.pValue = pxCoefficient + 1;
            xPrivateKeyTemplate.xCoefficient.ulValueLen = COEFFICIENT_LENGTH;
            xPrivateKeyTemplate.xTokenObject.type = CKA_TOKEN;
            xPrivateKeyTemplate.xTokenObject.pValue = &xTokenStorage;
            xPrivateKeyTemplate.xTokenObject.ulValueLen = sizeof( xTokenStorage );
        }

        xResult = pxFunctionList->C_CreateObject( xSession,
                                                  ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                                                  sizeof( PKCS11_PrivateRsaKeyTemplate_t ) / sizeof( CK_ATTRIBUTE ),
                                                  pxObjectHandle );

        if( NULL != pxModulus )
        {
            vPortFree( pxModulus );
        }

        if( NULL != pxE )
        {
            vPortFree( pxE );
        }

        if( NULL != pxD )
        {
            vPortFree( pxD );
        }

        if( NULL != pxPrime1 )
        {
            vPortFree( pxPrime1 );
        }

        if( NULL != pxPrime2 )
        {
            vPortFree( pxPrime2 );
        }

        if( NULL != pxExp1 )
        {
            vPortFree( pxExp1 );
        }

        if( NULL != pxExp2 )
        {
            vPortFree( pxExp2 );
        }

        if( NULL != pxCoefficient )
        {
            vPortFree( pxCoefficient );
        }
    }

    mbedtls_pk_free( &xMbedPkContext );

    return xResult;
}


CK_RV xProvisionPublicKey( CK_SESSION_HANDLE xSession,
                           uint8_t * pucKey,
                           size_t xKeyLength,
                           CK_KEY_TYPE xPublicKeyType,
                           uint8_t * pucPublicKeyLabel,
                           CK_OBJECT_HANDLE_PTR pxPublicKeyHandle )
{
    CK_RV xResult;
    CK_BBOOL xTrue = CK_TRUE;
    CK_FUNCTION_LIST_PTR pxFunctionList;
    CK_OBJECT_CLASS xClass = CKO_PUBLIC_KEY;

    xResult = C_GetFunctionList( &pxFunctionList );

    int lMbedResult = 0;
    mbedtls_pk_context xMbedPkContext;

    mbedtls_pk_init( &xMbedPkContext );
    lMbedResult = mbedtls_pk_parse_key( &xMbedPkContext, pucKey, xKeyLength, NULL, 0 );

    if( lMbedResult != 0 )
    {
        lMbedResult = mbedtls_pk_parse_public_key( &xMbedPkContext, pucKey, xKeyLength );
    }

    if( xPublicKeyType == CKK_RSA )
    {
        CK_BYTE xPublicExponent[] = { 0x01, 0x00, 0x01 };
        CK_BYTE xModulus[ MODULUS_LENGTH + 1 ] = { 0 };

        lMbedResult = mbedtls_rsa_export_raw( ( mbedtls_rsa_context * ) xMbedPkContext.pk_ctx,
                                              ( unsigned char * ) &xModulus, MODULUS_LENGTH + 1,
                                              NULL, 0,
                                              NULL, 0,
                                              NULL, 0,
                                              NULL, 0 );
        CK_ATTRIBUTE xPublicKeyTemplate[] =
        {
            { CKA_CLASS,           &xClass,           sizeof( CK_OBJECT_CLASS )                    },
            { CKA_KEY_TYPE,        &xPublicKeyType,   sizeof( CK_KEY_TYPE )                        },
            { CKA_TOKEN,           &xTrue,            sizeof( xTrue )                              },
            { CKA_MODULUS,         &xModulus + 1,     MODULUS_LENGTH                               }, /* Extra byte allocated at beginning for 0 padding. */
            { CKA_VERIFY,          &xTrue,            sizeof( xTrue )                              },
            { CKA_PUBLIC_EXPONENT, xPublicExponent,   sizeof( xPublicExponent )                    },
            { CKA_LABEL,           pucPublicKeyLabel, strlen( ( const char * ) pucPublicKeyLabel ) }
        };

        xResult = pxFunctionList->C_CreateObject( xSession,
                                                  ( CK_ATTRIBUTE_PTR ) xPublicKeyTemplate,
                                                  sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                  pxPublicKeyHandle );
    }
    else if( xPublicKeyType == CKK_EC )
    {
        CK_BYTE xEcParams[] = pkcs11DER_ENCODED_OID_P256;
        size_t xLength;
        CK_BYTE xEcPoint[ 256 ] = { 0 };

        mbedtls_ecdsa_context * pxEcdsaContext;
        pxEcdsaContext = ( mbedtls_ecdsa_context * ) xMbedPkContext.pk_ctx;

        /* DER encoded EC point. Leave 2 bytes for the tag and length. */
        lMbedResult = mbedtls_ecp_point_write_binary( &pxEcdsaContext->grp, &pxEcdsaContext->Q,
                                                      MBEDTLS_ECP_PF_UNCOMPRESSED, &xLength,
                                                      xEcPoint + 2, sizeof( xEcPoint ) - 2 );
        xEcPoint[ 0 ] = 0x04; /* Octet string. */
        xEcPoint[ 1 ] = ( CK_BYTE ) xLength;

        CK_ATTRIBUTE xPublicKeyTemplate[] =
        {
            { CKA_CLASS,     &xClass,           sizeof( xClass )                             },
            { CKA_KEY_TYPE,  &xPublicKeyType,   sizeof( xPublicKeyType )                     },
            { CKA_TOKEN,     &xTrue,            sizeof( xTrue )                              },
            { CKA_VERIFY,    &xTrue,            sizeof( xTrue )                              },
            { CKA_EC_PARAMS, xEcParams,         sizeof( xEcParams )                          },
            { CKA_EC_POINT,  xEcPoint,          xLength + 2                                  },
            { CKA_LABEL,     pucPublicKeyLabel, strlen( ( const char * ) pucPublicKeyLabel ) }
        };

        xResult = pxFunctionList->C_CreateObject( xSession,
                                                  ( CK_ATTRIBUTE_PTR ) xPublicKeyTemplate,
                                                  sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                  pxPublicKeyHandle );
    }
    else
    {
        xResult = CKR_ATTRIBUTE_VALUE_INVALID;
        configPRINTF( ( "Invalid key type. Supported options are CKK_RSA and CKK_EC" ) );
    }

    mbedtls_pk_free( &xMbedPkContext );

    return xResult;
}


/*-----------------------------------------------------------*/

CK_RV xProvisionGenerateKeyPairRSA( CK_SESSION_HANDLE xSession,
                                    uint8_t * pucPrivateKeyLabel,
                                    uint8_t * pucPublicKeyLabel,
                                    CK_OBJECT_HANDLE_PTR pxPrivateKeyHandle,
                                    CK_OBJECT_HANDLE_PTR pxPublicKeyHandle )
{
    CK_RV xResult;
    CK_MECHANISM xMechanism =
    {
        CKM_RSA_PKCS_KEY_PAIR_GEN, NULL_PTR, 0
    };
    CK_FUNCTION_LIST_PTR pxFunctionList;
    CK_ULONG xModulusBits = 2048;
    CK_BYTE xPublicExponent[] = { 0x01, 0x00, 0x01 };
    CK_BYTE xSubject[] = { 0x01, 0x02 };

    CK_BBOOL xTrue = CK_TRUE;
    CK_ATTRIBUTE xPublicKeyTemplate[] =
    {
        { CKA_ENCRYPT,         &xTrue,            sizeof( xTrue )                              },
        { CKA_VERIFY,          &xTrue,            sizeof( xTrue )                              },
        { CKA_MODULUS_BITS,    &xModulusBits,     sizeof( xModulusBits )                       },
        { CKA_PUBLIC_EXPONENT, xPublicExponent,   sizeof( xPublicExponent )                    },
        { CKA_LABEL,           pucPublicKeyLabel, strlen( ( const char * ) pucPublicKeyLabel ) }
    };

    CK_ATTRIBUTE xPrivateKeyTemplate[] =
    {
        { CKA_TOKEN,   &xTrue,             sizeof( xTrue )                               },
        { CKA_PRIVATE, &xTrue,             sizeof( xTrue )                               },
        { CKA_SUBJECT, xSubject,           sizeof( xSubject )                            },
        { CKA_DECRYPT, &xTrue,             sizeof( xTrue )                               },
        { CKA_SIGN,    &xTrue,             sizeof( xTrue )                               },
        { CKA_LABEL,   pucPrivateKeyLabel, strlen( ( const char * ) pucPrivateKeyLabel ) }
    };

    xResult = C_GetFunctionList( &pxFunctionList );

    xResult = pxFunctionList->C_GenerateKeyPair( xSession,
                                                 &xMechanism,
                                                 xPublicKeyTemplate,
                                                 sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                 xPrivateKeyTemplate, sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                 pxPublicKeyHandle,
                                                 pxPrivateKeyHandle );

    return xResult;
}

/*-----------------------------------------------------------*/

CK_RV xProvisionGenerateKeyPairEC( CK_SESSION_HANDLE xSession,
                                   uint8_t * pucPrivateKeyLabel,
                                   uint8_t * pucPublicKeyLabel,
                                   CK_OBJECT_HANDLE_PTR pxPrivateKeyHandle,
                                   CK_OBJECT_HANDLE_PTR pxPublicKeyHandle )
{
    CK_RV xResult;
    CK_MECHANISM xMechanism =
    {
        CKM_EC_KEY_PAIR_GEN, NULL_PTR, 0
    };
    CK_FUNCTION_LIST_PTR pxFunctionList;
    CK_BYTE xEcParams[] = pkcs11DER_ENCODED_OID_P256; /* prime256v1 */
    CK_KEY_TYPE xKeyType = CKK_EC;

    CK_BBOOL xTrue = CK_TRUE;
    CK_ATTRIBUTE xPublicKeyTemplate[] =
    {
        { CKA_KEY_TYPE,  &xKeyType,         sizeof( xKeyType )                           },
        { CKA_VERIFY,    &xTrue,            sizeof( xTrue )                              },
        { CKA_EC_PARAMS, xEcParams,         sizeof( xEcParams )                          },
        { CKA_LABEL,     pucPublicKeyLabel, strlen( ( const char * ) pucPublicKeyLabel ) }
    };

    CK_ATTRIBUTE xPrivateKeyTemplate[] =
    {
        { CKA_KEY_TYPE, &xKeyType,          sizeof( xKeyType )                            },
        { CKA_TOKEN,    &xTrue,             sizeof( xTrue )                               },
        { CKA_PRIVATE,  &xTrue,             sizeof( xTrue )                               },
        { CKA_SIGN,     &xTrue,             sizeof( xTrue )                               },
        { CKA_LABEL,    pucPrivateKeyLabel, strlen( ( const char * ) pucPrivateKeyLabel ) }
    };

    xResult = C_GetFunctionList( &pxFunctionList );

    xResult = pxFunctionList->C_GenerateKeyPair( xSession,
                                                 &xMechanism,
                                                 xPublicKeyTemplate,
                                                 sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                 xPrivateKeyTemplate, sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                 pxPublicKeyHandle,
                                                 pxPrivateKeyHandle );

    return xResult;
}

/*-----------------------------------------------------------*/

extern int convert_pem_to_der( const unsigned char * pucInput,
                               size_t xLen,
                               unsigned char * pucOutput,
                               size_t * pxOlen);
                               

CK_RV xProvisionCertificate( CK_SESSION_HANDLE xSession,
                             uint8_t * pucCertificate,
                             size_t xCertificateLength,
                             uint8_t * pucLabel,
                             CK_OBJECT_HANDLE_PTR pxObjectHandle )
{
    PKCS11_CertificateTemplate_t xCertificateTemplate;
    CK_OBJECT_CLASS xCertificateClass = CKO_CERTIFICATE;
    CK_CERTIFICATE_TYPE xCertificateType = CKC_X_509;
    CK_FUNCTION_LIST_PTR pxFunctionList;
    CK_RV xResult;
    uint8_t * pucDerObject = NULL;
    int32_t lConversionReturn = 0;
    size_t xDerLen = 0;
    CK_BBOOL xTokenStorage = CK_TRUE;

    CK_BYTE xSubject[] = "TestSubject";
    

    /* Initialize the client certificate template. */
    xCertificateTemplate.xObjectClass.type = CKA_CLASS;
    xCertificateTemplate.xObjectClass.pValue = &xCertificateClass;
    xCertificateTemplate.xObjectClass.ulValueLen = sizeof( xCertificateClass );
    xCertificateTemplate.xSubject.type = CKA_SUBJECT;
    xCertificateTemplate.xSubject.pValue = xSubject;
    xCertificateTemplate.xSubject.ulValueLen = strlen( ( const char * ) xSubject );
    xCertificateTemplate.xValue.type = CKA_VALUE;
    xCertificateTemplate.xValue.pValue = ( CK_VOID_PTR ) pucCertificate;
    xCertificateTemplate.xValue.ulValueLen = ( CK_ULONG ) xCertificateLength;
    xCertificateTemplate.xLabel.type = CKA_LABEL;
    xCertificateTemplate.xLabel.pValue = ( CK_VOID_PTR ) pucLabel;
    xCertificateTemplate.xLabel.ulValueLen = strlen( ( const char * ) pucLabel );
    xCertificateTemplate.xCertificateType.type = CKA_CERTIFICATE_TYPE;
    xCertificateTemplate.xCertificateType.pValue = &xCertificateType;
    xCertificateTemplate.xCertificateType.ulValueLen = sizeof( CK_CERTIFICATE_TYPE );
    xCertificateTemplate.xTokenObject.type = CKA_TOKEN;
    xCertificateTemplate.xTokenObject.pValue = &xTokenStorage;
    xCertificateTemplate.xTokenObject.ulValueLen = sizeof( xTokenStorage );

    xResult = C_GetFunctionList( &pxFunctionList );

    /* Litmus test for valid certificiate.  0x2d is '-' as in ----- BEGIN CERTIFICATE ----- */
    if( ( pucCertificate == NULL ) || ( pucCertificate[ 0 ] != 0x2d ) )
    {
        xResult = CKR_ATTRIBUTE_VALUE_INVALID;
    }

    if( xResult == CKR_OK )
    {
        /* Convert the certificate to DER format if it was in PEM. */
        /* The DER key should be about 3/4 the size of the PEM key, so mallocing the PEM key size is sufficient. */
        pucDerObject = pvPortMalloc( xCertificateTemplate.xValue.ulValueLen );
        xDerLen = xCertificateTemplate.xValue.ulValueLen;

        if( pucDerObject != NULL )
        {
            lConversionReturn = convert_pem_to_der( xCertificateTemplate.xValue.pValue,
                                                     xCertificateTemplate.xValue.ulValueLen,
                                                     pucDerObject,
                                                     &xDerLen );

            if( 0 != lConversionReturn )
            {
                xResult = CKR_ARGUMENTS_BAD;
            }
        }
        else
        {
            xResult = CKR_DEVICE_MEMORY;
        }
    }

    if( xResult == CKR_OK )
    {
        /* Set the template pointers to refer to the DER converted objects. */
        xCertificateTemplate.xValue.pValue = pucDerObject;
        xCertificateTemplate.xValue.ulValueLen = xDerLen;
    }

    /* Create an object using the encoded client certificate. */
    if( xResult == CKR_OK )
    {
        configPRINTF( ( "Write code signing certificate...\r\n" ) );

        xResult = pxFunctionList->C_CreateObject( xSession,
                                                  ( CK_ATTRIBUTE_PTR ) &xCertificateTemplate,
                                                  sizeof( xCertificateTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                  pxObjectHandle );
    }

    if( pucDerObject != NULL )
    {
        vPortFree( pucDerObject );
    }

    return xResult;
}

CK_RV xDestroyCredentials( CK_SESSION_HANDLE xSession )
{
    CK_RV xResult;
    CK_FUNCTION_LIST_PTR pxFunctionList;
    CK_OBJECT_HANDLE xObjectHandle;
    CK_BYTE * pxLabel;
    uint32_t uiIndex = 0;
    CK_BYTE * pxPkcsLabels[] =
    {
        ( CK_BYTE * ) pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
        ( CK_BYTE * ) pkcs11configLABEL_CODE_VERIFICATION_KEY,
        ( CK_BYTE * ) pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
        ( CK_BYTE * ) pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS
    };
    CK_OBJECT_CLASS xClass[] =
    {
        CKO_CERTIFICATE,
        CKO_PUBLIC_KEY,
        CKO_PRIVATE_KEY,
        CKO_PUBLIC_KEY
    };

    xResult = C_GetFunctionList( &pxFunctionList );

    for( ; uiIndex < sizeof( pxPkcsLabels ) / sizeof( pxPkcsLabels[ 0 ] ); uiIndex++ )
    {
        pxLabel = pxPkcsLabels[ uiIndex ];

        xResult = xFindObjectWithLabelAndClass( xSession,
                                                ( const char * ) pxLabel,
                                                xClass[ uiIndex ],
                                                &xObjectHandle );

        if( ( xResult == CKR_OK ) && ( xObjectHandle != CK_INVALID_HANDLE ) )
        {
            while( ( xResult == CKR_OK ) && ( xObjectHandle != CK_INVALID_HANDLE ) )
            {
                xResult = pxFunctionList->C_DestroyObject( xSession, xObjectHandle );

                /* PKCS #11 allows a module to maintain multiple objects with the same
                 * label and type. The intent of this loop is to try to delete all of them.
                 * However, to avoid getting stuck, we won't try to find another object
                 * of the same label/type if the previous delete failed. */
                if( xResult == CKR_OK )
                {
                    xResult = xFindObjectWithLabelAndClass( xSession,
                                                            ( const char * ) pxLabel,
                                                            xClass[ uiIndex ],
                                                            &xObjectHandle );
                }
                else
                {
                    break;
                }
            }
        }

        if( xResult == CKR_FUNCTION_NOT_SUPPORTED )
        {
            break;
        }
    }

    return xResult;
}

/*-----------------------------------------------------------*/

CK_RV xProvisionDevice( CK_SESSION_HANDLE xSession,
                        ProvisioningParams_t * pxParams )
{
    CK_RV xResult;
    CK_FUNCTION_LIST_PTR pxFunctionList;
    CK_OBJECT_HANDLE xObject = 0;

    xResult = C_GetFunctionList( &pxFunctionList );

    #if ( pkcs11configIMPORT_PRIVATE_KEYS_SUPPORTED == 1 )
        if( xResult == CKR_OK )
        {
            xResult = xProvisionCertificate( xSession,
                                             pxParams->pucClientCertificate,
                                             pxParams->ulClientCertificateLength,
                                             ( uint8_t * ) pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                                             &xObject );

            if( ( xResult != CKR_OK ) || ( xObject == CK_INVALID_HANDLE ) )
            {
                configPRINTF( ( "ERROR: Failed to provision device certificate. %d \r\n", xResult ) );
            }
        }

        if( xResult == CKR_OK )
        {
            xResult = xProvisionPrivateKey( xSession,
                                            pxParams->pucClientPrivateKey,
                                            pxParams->ulClientPrivateKeyLength,
                                            ( uint8_t * ) pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                                            &xObject );

            if( ( xResult != CKR_OK ) || ( xObject == CK_INVALID_HANDLE ) )
            {
                configPRINTF( ( "ERROR: Failed to provision device private key. %d \r\n", xResult ) );
            }
        }
    #endif /* if ( pkcs11configIMPORT_PRIVATE_KEYS_SUPPORTED == 1 ) */

    if( xResult == CKR_OK )
    {
        if( pxParams->ulJITPCertifiateLength != 0 )
        {
            xResult = xProvisionCertificate( xSession,
                                             pxParams->pucJITPCertificate,
                                             pxParams->ulJITPCertifiateLength,
                                             ( uint8_t * ) pkcs11configLABEL_JITP_CERTIFICATE,
                                             &xObject );

            if( xResult == CKR_DEVICE_MEMORY )
            {
                xResult = CKR_OK;
                configPRINTF( ( "Warning: Device does not have memory allocated for JITP certificate.  Certificate from aws_clientcredential_keys.h will be used for JITR. \r\n" ) );
            }
        }
    }

    if( xResult == CKR_OK )
    {
        configPRINTF( ( "Device credential provisioning succeeded.\r\n" ) );
    }
    else
    {
        configPRINTF( ( "Device credential provisioning failed.\r\n" ) );
    }

    return xResult;
}

/*-----------------------------------------------------------*/

void vAlternateKeyProvisioning( ProvisioningParams_t * xParams )
{
    CK_RV xResult = CKR_OK;
    CK_FUNCTION_LIST_PTR pxFunctionList = NULL;
    CK_SESSION_HANDLE xSession = 0;

    xResult = C_GetFunctionList( &pxFunctionList );

    /* Initialize the PKCS Module */
    if( xResult == CKR_OK )
    {
        xResult = xInitializePkcs11Session( &xSession );
    }

    if( xResult == CKR_OK )
    {
        #if ( pkcs11configIMPORT_PRIVATE_KEYS_SUPPORTED == 1 )
            xDestroyCredentials( xSession );
        #endif

        xResult = xProvisionDevice( xSession, xParams );

        pxFunctionList->C_CloseSession( xSession );
    }
}
/*-----------------------------------------------------------*/

void vDevModeKeyProvisioning( void )
{
    ProvisioningParams_t xParams;

    xParams.pucClientPrivateKey = ( uint8_t * ) keyCLIENT_PRIVATE_KEY_PEM;
    xParams.pucClientCertificate = ( uint8_t * ) keyCLIENT_CERTIFICATE_PEM;
    xParams.pucJITPCertificate = ( uint8_t * ) keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM;

    /* If using a JITR flow, a JITR certificate must be supplied. If using credentials generated by
     * AWS, this certificate is not needed. */
    if( ( NULL == xParams.pucJITPCertificate ) ||
        ( 0 == strcmp( "", ( const char * ) xParams.pucJITPCertificate ) ) )
    {
        xParams.ulJITPCertifiateLength = 0;
        configPRINTF( ( "No JITP certificate supplied (JITP certificate is not necessary when using AWS issued credentials). \r\n" ) );
    }
    else
    {
        xParams.ulJITPCertifiateLength = 1 + strlen( ( const char * ) xParams.pucJITPCertificate );
    }

    if( ( NULL == xParams.pucClientPrivateKey ) ||
        ( 0 == strcmp( "", ( const char * ) xParams.pucClientPrivateKey ) ) ||
        ( NULL == xParams.pucClientCertificate ) ||
        ( 0 == strcmp( "", ( const char * ) xParams.pucClientCertificate ) ) )
    {
        configPRINTF( ( "ERROR: the vDevModeKeyProvisioning function requires a valid device private key and certificate.\r\n" ) );
        configASSERT( pdFALSE );
    }
    else
    {
        xParams.ulClientPrivateKeyLength = 1 + strlen( ( const char * ) xParams.pucClientPrivateKey );
        xParams.ulClientCertificateLength = 1 + strlen( ( const char * ) xParams.pucClientCertificate );
        vAlternateKeyProvisioning( &xParams );
    }
}

/*-----------------------------------------------------------*/
