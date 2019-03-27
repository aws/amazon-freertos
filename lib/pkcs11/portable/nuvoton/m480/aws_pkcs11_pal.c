/*
 * Amazon FreeRTOS PKCS#11 for Numaker_PFM_M487
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
 * @brief Amazon FreeRTOS device specific helper functions for
 * PKCS#11 implementation based on mbedTLS.  This
 * file deviates from the FreeRTOS style standard for some function names and
 * data types in order to maintain compliance with the PKCS#11 standard.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "task.h"
#include "aws_pkcs11.h"
#include "aws_pkcs11_config.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/* flash driver includes. */
#include "NuMicro.h"

#define pkcs11FILE_NAME_PUBLISHER_CERTIFICATE    "FreeRTOS_Publisher_Certificate.dat"
#define pkcs11FILE_NAME_PUBLISHER_KEY            "FreeRTOS_Publisher_Key.dat"

#define pkcs11OBJECT_CERTIFICATE_MAX_SIZE    2048
#define pkcs11OBJECT_FLASH_CERT_PRESENT      ( 0x1A2B3C4DUL )


/**
 * @brief Structure for certificates/key storage.
 */

typedef struct
{
    CK_ULONG ulDeviceCertificateMark;
    CK_ULONG ulCertificateSize;
    CK_CHAR cCertificateData[ pkcs11OBJECT_CERTIFICATE_MAX_SIZE ];
} P11CertData_t;

typedef struct
{
    uint32_t DeviceCertificate;
    uint32_t DeviceKey;
    uint32_t PublisherCertificate;
    uint32_t PublisherKey;
} P11KeyConfig_t;

/**
 * @brief Certificates/key storage in flash.
 */
#define NVT_STORE_BASE             ( 0x7B000UL )      /* local storage start address in APROM  */

/* Set last 4 pages for certification storage, FMC_FLASH_PAGE_SIZE=4KB */
static P11KeyConfig_t P11KeyConfig ={ NVT_STORE_BASE, 
                                      NVT_STORE_BASE + FMC_FLASH_PAGE_SIZE,
                                      NVT_STORE_BASE + FMC_FLASH_PAGE_SIZE*2,
                                      NVT_STORE_BASE + FMC_FLASH_PAGE_SIZE*3};

static P11CertData_t P11CertDataSave;
                                
/*-----------------------------------------------------------*/


static CK_RV prvFLASH_update(uint32_t u32StartAddr, uint8_t * pucData, uint32_t ulDataSize)
{
    uint32_t    u32Addr;               /* flash address */
    uint32_t    u32data;               /* flash data    */
    uint32_t    *pDataSrc;             /* flash data    */    
    uint32_t    u32EndAddr = (u32StartAddr + sizeof(P11CertData_t));
    uint32_t    u32Pattern = 0xFFFFFFFF;

    
    FMC_Erase(u32StartAddr);    
     /* Verify if each data word from flash u32StartAddr to u32EndAddr be 0xFFFFFFFF.  */
    for (u32Addr = u32StartAddr; u32Addr < u32EndAddr; u32Addr += 4)
    {
        u32data = FMC_Read(u32Addr);   /* Read a flash word from address u32Addr. */

        if (u32data != u32Pattern )     /* Verify if data matched. */
        {
            printf("\nFMC_Read data verify failed at address 0x%x, read=0x%x, expect=0x%x\n", u32Addr, u32data, u32Pattern);
            return -1;                 /* data verify failed */
        }
    }
    
    memcpy( P11CertDataSave.cCertificateData, pucData, ulDataSize );
    P11CertDataSave.ulDeviceCertificateMark = pkcs11OBJECT_FLASH_CERT_PRESENT;
    P11CertDataSave.ulCertificateSize = ulDataSize;
    pDataSrc = (uint32_t *) &P11CertDataSave;
    /* Fill flash range from u32StartAddr to u32EndAddr with data word u32Pattern. */
    for (u32Addr = u32StartAddr; u32Addr < u32EndAddr; u32Addr += 4)
    {
        FMC_Write(u32Addr, *pDataSrc);          /* Program flash */
        pDataSrc++;
    }    
    
    return ulDataSize;
    
}

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
    CK_RV xResult = pdFALSE;
    uint32_t certFlashAddr = 0;
    CK_RV xBytesWritten = 0;
    CK_ULONG ulFlashMark = pkcs11OBJECT_FLASH_CERT_PRESENT;
    const P11CertData_t * pCertFlash;
    P11CertData_t * pCertSave = 0;
    
    /* enough room to store the certificate */
    if( ulDataSize > pkcs11OBJECT_CERTIFICATE_MAX_SIZE )
    {
        return xResult;   
    }
    
    /*
     * write client certificate.
     */
    SYS_UnlockReg();                   /* Unlock register lock protect */
    FMC_Open();                        /* Enable FMC ISP function */
    FMC_ENABLE_AP_UPDATE();            /* Enable APROM update. */

    if( strcmp( pcFileName, pkcs11configFILE_NAME_CLIENT_CERTIFICATE ) == 0 )
    {
        certFlashAddr = P11KeyConfig.DeviceCertificate;
    }
    else if( strcmp( pcFileName, pkcs11configFILE_NAME_KEY ) == 0 )
    {
        certFlashAddr = P11KeyConfig.DeviceKey;
    }
    else if( strcmp( pcFileName, pkcs11FILE_NAME_PUBLISHER_CERTIFICATE ) == 0 )
    {
        certFlashAddr = P11KeyConfig.PublisherCertificate;        
    }
    else if( strcmp( pcFileName, pkcs11FILE_NAME_PUBLISHER_KEY ) == 0 )
    {
        certFlashAddr = P11KeyConfig.PublisherKey;
    }

    if( certFlashAddr !=0 )
    {
        xBytesWritten = prvFLASH_update( certFlashAddr, pucData, ulDataSize );
        if( xBytesWritten == ulDataSize )
        {
            xResult = pdTRUE;
        }
    }            

    FMC_DISABLE_AP_UPDATE();           /* Disable APROM update. */
    SYS_LockReg();                     /* Lock protected registers */   
    
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
    const P11CertData_t * pCertFlash = 0;
    uint32_t certSize = 0;
    uint8_t * pCertData = 0;
    /*
     * Read client certificate.
     */
    if( strcmp( pcFileName, pkcs11configFILE_NAME_CLIENT_CERTIFICATE ) == 0 )
    {
        pCertFlash = (P11CertData_t *)P11KeyConfig.DeviceCertificate;
    }
    else if( strcmp( pcFileName, pkcs11configFILE_NAME_KEY ) == 0 )
    {
        pCertFlash = (P11CertData_t *)P11KeyConfig.DeviceKey;
    }
    else if( strcmp( pcFileName, pkcs11FILE_NAME_PUBLISHER_CERTIFICATE ) == 0 )
    {
        pCertFlash = (P11CertData_t *)P11KeyConfig.PublisherCertificate;        
    }
    else if( strcmp( pcFileName, pkcs11FILE_NAME_PUBLISHER_KEY ) == 0 )
    {
        pCertFlash = (P11CertData_t *)P11KeyConfig.PublisherKey;
    }

    if( ( pCertFlash !=0 ) && ( pCertFlash->ulDeviceCertificateMark == pkcs11OBJECT_FLASH_CERT_PRESENT ) )
    {
        pCertData = ( uint8_t * ) pCertFlash->cCertificateData;
        certSize = pCertFlash->ulCertificateSize;
        xResult = pdTRUE;
    }
    
    *pulDataSize = certSize;
    *ppucData = pCertData;    
    
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
