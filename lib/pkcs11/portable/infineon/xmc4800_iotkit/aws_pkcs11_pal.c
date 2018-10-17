/*
 * Amazon FreeRTOS PKCS#11 for XMC4800 IoT Kit V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Copyright (c) 2018, Infineon Technologies AG
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 * products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes with
 * Infineon Technologies AG dave@infineon.com).
 */


/**
 * @file aws_pkcs11_pal.c
 * @brief Amazon FreeRTOS device specific helper functions for
 * PKCS#11 implementation based on mbedTLS.  This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS#11 standard.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "task.h"
#include "semphr.h"

#include "aws_crypto.h"
#include "aws_pkcs11.h"
#include "aws_pkcs11_config.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/* flash driver includes. */
#include "e_eeprom_xmc4.h"

#define pkcs11OBJECT_CERTIFICATE_MAX_SIZE     2048

#define pkcs11OBJECT_FLASH_CERT_PRESENT       ( 0xABCDEFuL )

/**
 * @brief Structure for certificates/key storage.
 */
typedef struct
{
    CK_CHAR cDeviceCertificate[ pkcs11OBJECT_CERTIFICATE_MAX_SIZE ];
    CK_CHAR cDeviceKey[ pkcs11OBJECT_CERTIFICATE_MAX_SIZE ];
    CK_ULONG ulDeviceCertificateMark;
    CK_ULONG ulDeviceKeyMark;
} P11KeyConfig_t;

/**
 * @brief Certificates/key storage in flash.
 */
static P11KeyConfig_t P11KeyConfig;

static E_EEPROM_XMC4_t e_eeprom;

/*-----------------------------------------------------------*/

/**
 * @brief Writes a file to local storage.
 *
 * Port-specific file write for crytographic information.
 *
 * @param[in] pcFileName    The name of the file to be written to.
 * @param[in] pucData       Data buffer to be written to file
 * @param[in] pulDataSize   Size (in bytes) of file data.
 *
 * @return pdTRUE if data was saved successfully to file,
 * pdFALSE otherwise.
 */
BaseType_t PKCS11_PAL_SaveFile( char * pcFileName,
                                uint8_t * pucData,
                                uint32_t ulDataSize )
{
    CK_RV xUpdateFlash = pdFALSE;
    CK_RV xResult = pdFALSE;
    CK_ULONG ulFlashMark = pkcs11OBJECT_FLASH_CERT_PRESENT;

    /*
     * write client certificate.
     */

    if( strncmp( pcFileName,
    		     pkcs11configFILE_NAME_CLIENT_CERTIFICATE,
                 strlen( pkcs11configFILE_NAME_CLIENT_CERTIFICATE ) ) == 0 )
    {
        E_EEPROM_XMC4_WriteArray( offsetof(P11KeyConfig_t, cDeviceCertificate), pucData, ( ulDataSize + 1 ) ); /*Include '\0'*/
        /*change flash written mark'*/
        E_EEPROM_XMC4_WriteArray( offsetof(P11KeyConfig_t, ulDeviceCertificateMark), (uint8_t *const)&ulFlashMark, sizeof( CK_ULONG ));

        xUpdateFlash = pdTRUE;
    }

    /*
     * write client key.
     */

    if( strncmp( pcFileName,
    		     pkcs11configFILE_NAME_KEY,
                 strlen( pkcs11configFILE_NAME_KEY ) ) == 0 )
    {
        E_EEPROM_XMC4_WriteArray( offsetof(P11KeyConfig_t, cDeviceKey), pucData, ( ulDataSize + 1 ) ); /*Include '\0'*/
        /*change flash written mark'*/
        E_EEPROM_XMC4_WriteArray( offsetof(P11KeyConfig_t, ulDeviceKeyMark), (uint8_t *const)&ulFlashMark, sizeof( CK_ULONG ));

        xUpdateFlash = pdTRUE;
    }

    if (xUpdateFlash == pdTRUE)
    {
      taskENTER_CRITICAL();
      E_EEPROM_XMC4_STATUS_t status = E_EEPROM_XMC4_UpdateFlashContents();
      taskEXIT_CRITICAL();

   	  if (status == E_EEPROM_XMC4_STATUS_OK)
   	  {
   	    E_EEPROM_XMC4_ReadArray(0, (uint8_t *const)&P11KeyConfig, sizeof(P11KeyConfig_t));
   		xResult = pdTRUE;
   	  }
    }

    return xResult;
}

/*-----------------------------------------------------------*/

/**
 * @brief Reads a file from local storage.
 *
 * Port-specific file access for crytographic information.
 *
 * @param[in] pcFileName    The name of the file to be read.
 * @param[out] ppucData     Pointer to buffer for file data.
 * @param[out] pulDataSize  Size (in bytes) of data located in file.
 *
 * @return pdTRUE if data was retrieved successfully from files,
 * pdFALSE otherwise.
 */
BaseType_t PKCS11_PAL_ReadFile( char * pcFileName,
                                uint8_t ** ppucData,
                                uint32_t * pulDataSize )
{
    CK_RV xResult = pdFALSE;

    /*
     * Read client certificate.
     */

    if( strncmp( pcFileName,
    		     pkcs11configFILE_NAME_CLIENT_CERTIFICATE,
                 strlen( pkcs11configFILE_NAME_CLIENT_CERTIFICATE ) ) == 0 )
    {
        /*
         * return reference and size only if certificates are present in flash
         */
        if( P11KeyConfig.ulDeviceCertificateMark == pkcs11OBJECT_FLASH_CERT_PRESENT )
        {
            *ppucData = P11KeyConfig.cDeviceCertificate;
            *pulDataSize = ( uint32_t ) strlen( ( const char * ) P11KeyConfig.cDeviceCertificate ) + 1;
            xResult = pdTRUE;
        }
    }

    /*
     * Read client key.
     */

    if( strncmp( pcFileName,
    		     pkcs11configFILE_NAME_KEY,
                 strlen( pkcs11configFILE_NAME_KEY ) ) == 0 )
    {
        /*
         * return reference and size only if certificates are present in flash
         */
        if( P11KeyConfig.ulDeviceKeyMark == pkcs11OBJECT_FLASH_CERT_PRESENT )
        {
            *ppucData = P11KeyConfig.cDeviceKey;
            *pulDataSize = ( uint32_t ) strlen( ( const char * ) P11KeyConfig.cDeviceKey ) + 1;
            xResult = pdTRUE;
        }
    }

    return xResult;
}

/*-----------------------------------------------------------*/

/**
 * @brief Cleanup after ReadFile.
 *
 * @param[in] pucBuffer The buffer to free.
 * @param[in] ulBufferSize The length of the above buffer.
 */
void PKCS11_PAL_ReleaseFileData( uint8_t * pucBuffer,
                                 uint32_t ulBufferSize )
{
    /* Unused parameters. */
    ( void ) pucBuffer;
    ( void ) ulBufferSize;

    /* Since no buffer was allocated on heap, there is no cleanup
     * to be done. */
}
/*-----------------------------------------------------------*/

/**
 * @brief Initialize the Cryptoki module for use.
 */
CK_DEFINE_FUNCTION( CK_RV, C_Initialize )( CK_VOID_PTR pvInitArgs )
{   /*lint !e9072 It's OK to have different parameter name. */
    ( void ) ( pvInitArgs );

    /* Ensure that the FreeRTOS heap is used. */
    CRYPTO_ConfigureHeap();

    E_EEPROM_XMC4_Init(&e_eeprom, sizeof(P11KeyConfig_t));

    if (E_EEPROM_XMC4_IsFlashEmpty() == false)
    {
      E_EEPROM_XMC4_ReadArray(0, (uint8_t *const)&P11KeyConfig, sizeof(P11KeyConfig_t));
    }

    return CKR_OK;
}

