/*
 * FreeRTOS PKCS #11 V2.2.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file iot_pkcs11_mbedtls.c
 * @brief mbedTLS-based PKCS#11 implementation for software keys. This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS #11 standard.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "core_pkcs11_config.h"

#include "iot_crypto.h"
#include "core_pkcs11.h"
#include "core_pkcs11_pal.h"
#include "core_pki_utils.h"

#include <string.h>

#ifdef FREERTOS_ENABLE_UNIT_TESTS
    #include "core_test_pkcs11_config.h"
#endif

/* Credential includes. */
#include "aws_clientcredential.h"
#include "aws_clientcredential_keys.h"
#include "iot_default_root_certificates.h"

/* Microchip Secure Element */
#include "atca_config.h"
#include "atca_cert_chain.h"
#include "pkcs11_config.h"
#include "pkcs11/pkcs11_object.h"
#include "pkcs11/pkcs11_slot.h"

/*
 * PKCS#11 module implementation.
 */

/**
 * @brief PKCS#11 interface functions implemented by this Cryptoki module.
 */
CK_FUNCTION_LIST pkcs11FunctionList =
{
    { CRYPTOKI_VERSION_MAJOR, CRYPTOKI_VERSION_MINOR },
    C_Initialize,
    C_Finalize,
    NULL, /*C_GetInfo */
    C_GetFunctionList,
    C_GetSlotList,
    NULL, /*C_GetSlotInfo*/
    C_GetTokenInfo,
    NULL, /*C_GetMechanismList*/
    C_GetMechanismInfo,
    C_InitToken,
    NULL, /*C_InitPIN*/
    NULL, /*C_SetPIN*/
    C_OpenSession,
    C_CloseSession,
    NULL,    /*C_CloseAllSessions*/
    NULL,    /*C_GetSessionInfo*/
    NULL,    /*C_GetOperationState*/
    NULL,    /*C_SetOperationState*/
    C_Login, /*C_Login*/
    NULL,    /*C_Logout*/
    C_CreateObject,
    NULL,    /*C_CopyObject*/
    C_DestroyObject,
    NULL,    /*C_GetObjectSize*/
    C_GetAttributeValue,
    NULL,    /*C_SetAttributeValue*/
    C_FindObjectsInit,
    C_FindObjects,
    C_FindObjectsFinal,
    NULL, /*C_EncryptInit*/
    NULL, /*C_Encrypt*/
    NULL, /*C_EncryptUpdate*/
    NULL, /*C_EncryptFinal*/
    NULL, /*C_DecryptInit*/
    NULL, /*C_Decrypt*/
    NULL, /*C_DecryptUpdate*/
    NULL, /*C_DecryptFinal*/
    C_DigestInit,
    NULL, /*C_Digest*/
    C_DigestUpdate,
    NULL, /* C_DigestKey*/
    C_DigestFinal,
    C_SignInit,
    C_Sign,
    NULL, /*C_SignUpdate*/
    NULL, /*C_SignFinal*/
    NULL, /*C_SignRecoverInit*/
    NULL, /*C_SignRecover*/
    C_VerifyInit,
    C_Verify,
    NULL, /*C_VerifyUpdate*/
    NULL, /*C_VerifyFinal*/
    NULL, /*C_VerifyRecoverInit*/
    NULL, /*C_VerifyRecover*/
    NULL, /*C_DigestEncryptUpdate*/
    NULL, /*C_DecryptDigestUpdate*/
    NULL, /*C_SignEncryptUpdate*/
    NULL, /*C_DecryptVerifyUpdate*/
    NULL, /*C_GenerateKey*/
    C_GenerateKeyPair,
    NULL, /*C_WrapKey*/
    NULL, /*C_UnwrapKey*/
    NULL, /*C_DeriveKey*/
    NULL, /*C_SeedRandom*/
    C_GenerateRandom,
    NULL, /*C_GetFunctionStatus*/
    NULL, /*C_CancelFunction*/
    NULL  /*C_WaitForSlotEvent*/
};

static char gcPkcs11ThingNameCache[ 33 ];

const char * pcPkcs11GetThingName( void )
{
    if( 0 == gcPkcs11ThingNameCache[ 0 ] )
    {
        CK_RV xResult;
        CK_FUNCTION_LIST_PTR pxFunctionList;
        CK_SLOT_ID * pxSlotId = NULL;
        CK_ULONG xSlotCount;
        CK_TOKEN_INFO_PTR pxTokenInfo = NULL;

        xResult = C_GetFunctionList( &pxFunctionList );

        if( xResult == CKR_OK )
        {
            xResult = xInitializePKCS11();
        }

        if( ( xResult == CKR_OK ) || ( xResult == CKR_CRYPTOKI_ALREADY_INITIALIZED ) )
        {
            xResult = xGetSlotList( &pxSlotId, &xSlotCount );
        }

        if( xResult == CKR_OK )
        {
            /* Check if token is initialized */
            pxTokenInfo = pvPortMalloc( sizeof( CK_TOKEN_INFO ) );

            if( pxTokenInfo != NULL )
            {
                /* We will take the first slot available.
                 * If your application has multiple slots, insert logic
                 * for selecting an appropriate slot here.
                 * TODO: Consider a define here instead.
                 */
                xResult = pxFunctionList->C_GetTokenInfo( pxSlotId[ 0 ], pxTokenInfo );
            }
            else
            {
                xResult = CKR_HOST_MEMORY;
            }
        }

        if( 0 == xResult )
        {
            CK_CHAR_PTR end = memchr( pxTokenInfo->label, ' ', sizeof( pxTokenInfo->label ) );
            memcpy( gcPkcs11ThingNameCache, pxTokenInfo->label, end - pxTokenInfo->label );
        }

        if( pxTokenInfo != NULL )
        {
            vPortFree( pxTokenInfo );
        }

        if( pxSlotId != NULL )
        {
            vPortFree( pxSlotId );
        }
    }

    return gcPkcs11ThingNameCache;
}

#ifdef ATCA_HAL_I2C
/** \brief default configuration for an ECCx08A device */
    ATCAIfaceCfg cfg_ateccx08a_i2c_default =
    {
        .iface_type            = ATCA_I2C_IFACE,
        .devtype               = ATECC608A,
        .atcai2c.slave_address = 0xC0,
        .atcai2c.bus           = 1,
        .atcai2c.baud          = 400000,
        .wake_delay            = 1500,
        .rx_retries            = 20
    };
#endif /* ifdef ATCA_HAL_I2C */

#ifdef ATCA_HAL_KIT_HID
/** \brief default configuration for Kit protocol over the device's async interface */
    ATCAIfaceCfg cfg_ateccx08a_kithid_default =
    {
        .iface_type         = ATCA_HID_IFACE,
        .devtype            = ATECC608A,
        .atcahid.idx        = 0,
        .atcahid.vid        = 0x03EB,
        .atcahid.pid        = 0x2312,
        .atcahid.packetsize = 64,
        .atcahid.guid       = { 0x4d,        0x1e,0x55, 0xb2, 0xf1, 0x6f, 0x11, 0xcf, 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 },
    };
#endif /* ifdef ATCA_HAL_KIT_HID */

#ifdef FREERTOS_ENABLE_UNIT_TESTS

/* This configuration is only suitable for testing of a device and does not enforce good
 * security policies that should be practiced in production */
    const uint8_t atecc608_config[] =
    {
        0x01, 0x23, 0x00, 0x00,                         /* SN03 */
        0x00, 0x00, 0x60, 0x01,                         /* RevNum */
        0x00, 0x00, 0x00, 0x00,                         /* SN47 */
        0xEE,                                           /* SN8 */
        0x01,                                           /* AES_Enable */
        0x01,                                           /* I2C */
        0x00,                                           /* Reserved */
        ATCA_I2C_ECC_ADDRESS,                           /* I2C Address */
        0x00,                                           /* Reserved 2 */
        0x00,                                           /* Count match */
        0x01,                                           /* Chip mode */
        0x85, 0x66,                                     /* SlotConfig 0 */
        0x82, 0x00,                                     /*1 */
        0x85, 0x66,                                     /*2 */
        0x85, 0x20,                                     /*3 */
        0x85, 0x20,                                     /*4 */
        0xC6, 0x46,                                     /*5 */
        0x8F, 0x0F,                                     /*6 */
        0x9F, 0x8F,                                     /*7 */
        0x0F, 0x0F,                                     /*8 */
        0x8F, 0x0F,                                     /*9 */
        0x0F, 0x0F,                                     /*10 */
        0x0F, 0x0F,                                     /*11 */
        0x0F, 0x0F,                                     /*12 */
        0x0F, 0x0F,                                     /*13 */
        0x0D, 0x1F,                                     /*14 */
        0x0F, 0x0F,                                     /*15 */
        0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, /* Counter0 */
        0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, /* Counter1 */
        0x00,                                           /* Use Lock */
        0x00,                                           /* Volatile Key Permission */
        0x03, 0xF7,
        0x00,
        0x69, 0x76,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Reserve3 */
        0x00,                                                 /* User Extra */
        0x00,                                                 /* UserExtraAdd */
        0x55,                                                 /* Lock Value */
        0x55,                                                 /* Lock Config */
        0xFF, 0xFF,                                           /* Slot locked. */
        0x0E, 0x60,                                           /* Chip options */
        0x00, 0x00, 0x00, 0x00,                               /* X509 Format */
        0x53, 0x00,                                           /* KeyConfig[0] */
        0x53, 0x00,                                           /*1 */
        0x53, 0x00,                                           /*2 */
        0x73, 0x00,                                           /*3 */
        0x73, 0x00,                                           /*4 */
        0x38, 0x00,                                           /*5 */
        0x7C, 0x00,                                           /*6 */
        0x1C, 0x00,                                           /*7 */
        0x3C, 0x00,                                           /*8 */
        0x1A, 0x00,                                           /*9 */
        0x3C, 0x00,                                           /*10 */
        0x30, 0x00,                                           /*11 */
        0x3C, 0x00,                                           /*12 */
        0x30, 0x00,                                           /*13 */
        0x12, 0x00,                                           /*14 */
        0x30, 0x00                                            /*15 */
    };
#else /* ifdef FREERTOS_ENABLE_UNIT_TESTS */
/** Standard Configuration Structure for ATECC608A devices */
    const uint8_t atecc608_config[] =
    {
        0x01,                 0x23, 0x00, 0x00, 0x00, 0x00, 0x60, 0x01, 0x00, 0x00, 0x00, 0x00, 0xEE, 0x01, 0x01, 0x00,
        ATCA_I2C_ECC_ADDRESS, 0x00, 0x00, 0x01, 0x8F, 0x20, 0xC4, 0x44, 0x87, 0x20, 0x87, 0x20, 0x8F, 0x0F, 0xC4, 0x36,
        0x9F,                 0x0F, 0x82, 0x20, 0x0F, 0x0F, 0xC4, 0x44, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
        0x0F,                 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00,                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,                 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x33,                 0x00, 0x1C, 0x00, 0x13, 0x00, 0x13, 0x00, 0x7C, 0x00, 0x1C, 0x00, 0x3C, 0x00, 0x33, 0x00,
        0x3C,                 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x30, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x30, 0x00,
    };
#endif /* ifdef FREERTOS_ENABLE_UNIT_TESTS */


/* Helper function to assign the proper fields to an certificate object from a cert def */
CK_RV pkcs11_config_cert( pkcs11_lib_ctx_ptr pLibCtx,
                          pkcs11_slot_ctx_ptr pSlot,
                          pkcs11_object_ptr pObject,
                          CK_ATTRIBUTE_PTR pLabel )
{
    CK_RV rv = CKR_OK;
    size_t len;

    ( void ) pLibCtx;
    ( void ) pSlot;

    if( !pObject || !pLabel )
    {
        return CKR_ARGUMENTS_BAD;
    }

    if( pLabel->ulValueLen >= PKCS11_MAX_LABEL_SIZE )
    {
        return CKR_ARGUMENTS_BAD;
    }

    if( !strncmp( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, ( char * ) pLabel->pValue, pLabel->ulValueLen ) )
    {
        /* Slot 10 - Device Cert for Slot 0*/
        pkcs11_config_init_cert( pObject, pLabel->pValue, pLabel->ulValueLen );
        pObject->slot = 10;
        pObject->class_type = CK_CERTIFICATE_CATEGORY_TOKEN_USER;
        pObject->size = g_cert_def_2_device.cert_template_size;
        pObject->data = &g_cert_def_2_device;
        #ifdef FREERTOS_ENABLE_UNIT_TESTS
            pObject->flags = PKCS11_OBJECT_FLAG_DESTROYABLE;
        #endif
    }
    else if( !strncmp( pkcs11configLABEL_JITP_CERTIFICATE, ( char * ) pLabel->pValue, pLabel->ulValueLen ) )
    {
        /* Slot 12 - Signer Cert for Slot 10 */
        pkcs11_config_init_cert( pObject, pLabel->pValue, pLabel->ulValueLen );
        pObject->slot = 12;
        pObject->class_type = CK_CERTIFICATE_CATEGORY_AUTHORITY;
        pObject->size = g_cert_def_1_signer.cert_template_size;
        pObject->data = &g_cert_def_1_signer;
        #ifdef FREERTOS_ENABLE_UNIT_TESTS
            pObject->flags = PKCS11_OBJECT_FLAG_DESTROYABLE;
        #endif
    }

    #ifdef pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS
        else if( !strncmp( pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS, ( char * ) pLabel->pValue, pLabel->ulValueLen ) )
        {
            /* Slot 10 - Device Cert for Slot 0*/
            pkcs11_config_init_cert( pObject, pLabel->pValue, pLabel->ulValueLen );
            pObject->slot = 2;
            pObject->class_type = CK_CERTIFICATE_CATEGORY_TOKEN_USER;
            pObject->size = g_cert_def_3_test.cert_template_size;
            pObject->data = &g_cert_def_3_test;
            pObject->flags = PKCS11_OBJECT_FLAG_DESTROYABLE;
        }
    #endif /* ifdef pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS */
    else
    {
        rv = CKR_ARGUMENTS_BAD;
    }

    return rv;
}

/* Helper function to assign the proper fields to a key object */
CK_RV pkcs11_config_key( pkcs11_lib_ctx_ptr pLibCtx,
                         pkcs11_slot_ctx_ptr pSlot,
                         pkcs11_object_ptr pObject,
                         CK_ATTRIBUTE_PTR pLabel )
{
    CK_RV rv = CKR_OK;
    size_t len;

    ( void ) pLibCtx;

    if( !pObject || !pLabel || !pSlot )
    {
        return CKR_ARGUMENTS_BAD;
    }

    if( pLabel->ulValueLen >= PKCS11_MAX_LABEL_SIZE )
    {
        return CKR_ARGUMENTS_BAD;
    }

    if( !strncmp( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, ( char * ) pLabel->pValue, pLabel->ulValueLen ) )
    {
        /* Slot 0 - Device Private Key */
        pkcs11_config_init_private( pObject, pLabel->pValue, pLabel->ulValueLen );
        pObject->slot = 0;
        pObject->config = &pSlot->cfg_zone;
        #ifdef FREERTOS_ENABLE_UNIT_TESTS
            pObject->flags = PKCS11_OBJECT_FLAG_DESTROYABLE;
        #endif
    }
    else if( !strncmp( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, ( char * ) pLabel->pValue, pLabel->ulValueLen ) )
    {
        /* Slot 0 - Device Private Key */
        pkcs11_config_init_public( pObject, pLabel->pValue, pLabel->ulValueLen );
        pObject->slot = 0;
        pObject->config = &pSlot->cfg_zone;
        #ifdef FREERTOS_ENABLE_UNIT_TESTS
            pObject->flags = PKCS11_OBJECT_FLAG_DESTROYABLE;
        #endif
    }

    #ifdef pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS
        else if( !strncmp( pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, ( char * ) pLabel->pValue, pLabel->ulValueLen ) )
        {
            /* Slot 0 - Device Private Key */
            pkcs11_config_init_private( pObject, pLabel->pValue, pLabel->ulValueLen );
            pObject->slot = 2;
            pObject->config = &pSlot->cfg_zone;
            pObject->flags = PKCS11_OBJECT_FLAG_DESTROYABLE;
        }
    #endif
    #ifdef pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS
        else if( !strncmp( pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, ( char * ) pLabel->pValue, pLabel->ulValueLen ) )
        {
            /* Slot 0 - Device Private Key */
            pkcs11_config_init_public( pObject, pLabel->pValue, pLabel->ulValueLen );
            pObject->slot = 2;
            pObject->config = &pSlot->cfg_zone;
            pObject->flags = PKCS11_OBJECT_FLAG_DESTROYABLE;
        }
    #endif
    else
    {
        rv = CKR_ARGUMENTS_BAD;
    }

    return rv;
}

CK_RV pkcs11_config_load_objects( pkcs11_slot_ctx_ptr pSlot )
{
    pkcs11_object_ptr pObject;
    CK_RV rv = CKR_OK;
    CK_ATTRIBUTE xLabel;

    if( CKR_OK == rv )
    {
        rv = pkcs11_object_alloc( &pObject );

        if( pObject )
        {
            /* Slot 0 - Device Private Key */
            xLabel.pValue = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
            xLabel.ulValueLen = strlen( xLabel.pValue );
            xLabel.type = CKA_LABEL;
            pkcs11_config_key( NULL, pSlot, pObject, &xLabel );
        }
    }

    if( CKR_OK == rv )
    {
        rv = pkcs11_object_alloc( &pObject );

        if( pObject )
        {
            /* Slot 0 - Device Public Key */
            xLabel.pValue = pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS;
            xLabel.ulValueLen = strlen( xLabel.pValue );
            xLabel.type = CKA_LABEL;
            pkcs11_config_key( NULL, pSlot, pObject, &xLabel );
        }
    }

    if( CKR_OK == rv )
    {
        rv = pkcs11_object_alloc( &pObject );

        if( pObject )
        {
            /* Slot 0 - Device Public Key */
            xLabel.pValue = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;
            xLabel.ulValueLen = strlen( xLabel.pValue );
            xLabel.type = CKA_LABEL;
            pkcs11_config_cert( NULL, pSlot, pObject, &xLabel );
        }
    }

    if( CKR_OK == rv )
    {
        rv = pkcs11_object_alloc( &pObject );

        if( pObject )
        {
            /* Slot 0 - Device Public Key */
            xLabel.pValue = pkcs11configLABEL_JITP_CERTIFICATE;
            xLabel.ulValueLen = strlen( xLabel.pValue );
            xLabel.type = CKA_LABEL;
            pkcs11_config_cert( NULL, pSlot, pObject, &xLabel );
        }
    }

    #ifdef pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS
        if( CKR_OK == rv )
        {
            rv = pkcs11_object_alloc( &pObject );

            if( pObject )
            {
                /* Slot 0 - Device Private Key */
                xLabel.pValue = pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
                xLabel.ulValueLen = strlen( xLabel.pValue );
                xLabel.type = CKA_LABEL;
                pkcs11_config_key( NULL, pSlot, pObject, &xLabel );
            }
        }
    #endif /* ifdef pkcs11testLABEL_DEVICE_PRIVATE_KEY_FOR_TLS */
    #ifdef pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS
        if( CKR_OK == rv )
        {
            rv = pkcs11_object_alloc( &pObject );

            if( pObject )
            {
                /* Slot 0 - Device Public Key */
                xLabel.pValue = pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS;
                xLabel.ulValueLen = strlen( xLabel.pValue );
                xLabel.type = CKA_LABEL;
                pkcs11_config_key( NULL, pSlot, pObject, &xLabel );
            }
        }
    #endif /* ifdef pkcs11testLABEL_DEVICE_PUBLIC_KEY_FOR_TLS */
    #ifdef pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS
        if( CKR_OK == rv )
        {
            rv = pkcs11_object_alloc( &pObject );

            if( pObject )
            {
                /* Slot 0 - Device Public Key */
                xLabel.pValue = pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS;
                xLabel.ulValueLen = strlen( xLabel.pValue );
                xLabel.type = CKA_LABEL;
                pkcs11_config_cert( NULL, pSlot, pObject, &xLabel );
            }
        }
    #endif /* ifdef pkcs11testLABEL_DEVICE_CERTIFICATE_FOR_TLS */

    return rv;
}
