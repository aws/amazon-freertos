/*
 * Amazon FreeRTOS OTA PAL V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_ota_pal.c
 * @brief Platform Abstraction layer for AWS OTA
 *
 */

/* C Runtime includes. */
#include <string.h>

/* Amazon FreeRTOS include. */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "aws_iot_ota_pal.h"
#include "bootutil/bootutil.h"
#include "sysflash/sysflash.h"
#include "flash_map_backend/flash_map_backend.h"
#include "cy_pdl.h"
#include "aws_application_version.h"
#include "aws_ota_codesigner_certificate.h"
#include "mbedtls/base64.h"
#include "mbedtls/sha256.h"
#include "iot_crypto.h"


/**
 * @brief File Signature Key
 *
 * The OTA signature algorithm we support on this platform.
 */
const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";

/**
 * @brief File Signature Key
 *
 * Enable to test version # on receiving the header of the OTA Image.
 * Otherwise, the entire OTA Image is downloaded, the copy/swap is
 * performed, and then the version is checked.
 */
// #define OTA_TEST_VERSION_EARLY  1
#if OTA_TEST_VERSION_EARLY
/* Defines & structures from mcuboot
 * We use them here for checking version during download.
 */
#define IMAGE_MAGIC                 0x96f3b83d

struct image_version {
    uint8_t iv_major;
    uint8_t iv_minor;
    uint16_t iv_revision;
    uint32_t iv_build_num;
};

/* Image header.  All fields are in little endian byte order. */
struct image_header {
    uint32_t ih_magic;
    uint32_t ih_load_addr;
    uint16_t ih_hdr_size;           /* Size of image header (bytes). */
    uint16_t ih_protect_tlv_size;   /* Size of protected TLV area (bytes). */
    uint32_t ih_img_size;           /* Does not include header. */
    uint32_t ih_flags;              /* IMAGE_F_[...]. */
    struct image_version ih_ver;
    uint32_t _pad1;
};
static char ota_version_checked;
#endif

/***********************************************************************
 *
 * defines
 *
 **********************************************************************/
static const char pcOTA_PAL_CERT_BEGIN[] = "-----BEGIN CERTIFICATE-----";
static const char pcOTA_PAL_CERT_END[] = "-----END CERTIFICATE-----";

/***********************************************************************
 *
 * Macros
 *
 **********************************************************************/
/* Debugging macros */
#if 1
/**
 * @brief Macro to print system context information
 */
#define PRINT_SYSTEM_CONTEXT_PTR()
/**
 * @brief Macro to print Boot swap type
 */
#define PRINT_BOOT_SWAP_TYPE( title, boot_type )
/**
 * @brief Macro to print eState value
 */
#define PRINT_eSTATE( title, eSTATE )
/**
 * @brief Macro to print PAL state
 */
#define PRINT_PAL_STATE( title, palState )
#else
#define PRINT_SYSTEM_CONTEXT_PTR()  configPRINTF(("   sys_ctx: %p\r\n", sys_ctx));

#define PRINT_BOOT_SWAP_TYPE( title, boot_type ) \
    configPRINTF( ( "%s:: boot_swap_type:%d (%s)\r\n", ((title == NULL)?"":title), \
             boot_type, ((boot_type == 0) ? "Unknown" :             \
              (boot_type == BOOT_SWAP_TYPE_NONE) ? "None" :         \
              (boot_type == BOOT_SWAP_TYPE_TEST) ? "Test" :         \
              (boot_type == BOOT_SWAP_TYPE_PERM) ? "Permanent" :    \
              (boot_type == BOOT_SWAP_TYPE_REVERT) ? "Revert" :     \
              (boot_type == BOOT_SWAP_TYPE_FAIL) ? "FAIL" :         \
              (boot_type == BOOT_SWAP_TYPE_PANIC) ? "PANIC !!" : "Bad State")   ) );

#define PRINT_eSTATE( title, state ) \
    configPRINTF( ( "%s::   Sys Image State: 0x%lx (%s) \r\n", ((title == NULL)?"":title), \
             state, ((state == eOTA_ImageState_Unknown) ? "Unknown" :  \
             (state == eOTA_ImageState_Testing) ? "Testing" :          \
             (state == eOTA_ImageState_Accepted) ? "Accepted" :        \
             (state == eOTA_ImageState_Rejected) ? "Rejected" :        \
             (state == eOTA_ImageState_Aborted) ? "Aborted" : "ERROR") ) );

#define PRINT_PAL_STATE( title, palState ) \
    configPRINTF( ( "%s::   PAL Image State: 0x%lx (%s) \r\n", ((title == NULL)?"":title), \
            palState, ((palState == eOTA_PAL_ImageState_Unknown) ? "Unknown" :  \
             (palState == eOTA_PAL_ImageState_PendingCommit) ? "Pending" :          \
             (palState == eOTA_PAL_ImageState_Valid) ? "Valid" :        \
             (palState == eOTA_PAL_ImageState_Invalid) ? "Invalid" : "ERROR") ) );

#endif
/***********************************************************************
 *
 * Structures
 *
 **********************************************************************/

/***********************************************************************
 *
 * Variables
 *
 **********************************************************************/
/**
 * @brief Copy of system context
 *
 * Keep track of system context
 */
const OTA_FileContext_t *sys_ctx;

/**
 * @brief Current OTA Image State
 *
 * Keep track of the state OTA Agent wants us to be in.
 */
static OTA_ImageState_t current_OTA_ImageState = eOTA_ImageState_Unknown;


/**
 * @brief Current OTA Image State
 *
 * buffer for reading image to verify signature
 */
static uint8_t buffer[256];

/***********************************************************************
 *
 * Functions
 *
 **********************************************************************/

/**
 * @brief Read the specified signer certificate from the filesystem into a local buffer.
 *
 * The allocated memory returned becomes the property of the caller who is responsible for freeing it.
 *
 * This function is called from prvPAL_CheckFileSignature(). It should be implemented if signature
 * verification is not offloaded to non-volatile memory io function.
 *
 * @param[in] pucCertName The file path of the certificate file.
 * @param[out] ulSignerCertSize The size of the certificate file read.
 *
 * @return A pointer to the signer certificate in the file system. NULL if the certificate cannot be read.
 * This returned pointer is the responsibility of the caller; if the memory is allocated the caller must free it.
 */
static uint8_t * prvPAL_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize )
{
    uint8_t *pucCertEnd;
    uint8_t *pucDecodedCertificate ;
    size_t ulDecodedCertificateSize;

    *ulSignerCertSize = sizeof(signingcredentialSIGNING_CERTIFICATE_PEM);
    /* Skip the "BEGIN CERTIFICATE" */
    uint8_t* pucCertBegin = (uint8_t *)strstr (signingcredentialSIGNING_CERTIFICATE_PEM, pcOTA_PAL_CERT_BEGIN) ;
    if (pucCertBegin == NULL)
    {
        configPRINTF( ( "%s() No Begin found for Certificate\r\n", __func__) );
        return NULL;
    }
    pucCertBegin += sizeof(pcOTA_PAL_CERT_BEGIN);

    /* Find the "END CERTIFICATE" */
    pucCertEnd =  (uint8_t *)strstr((char *)pucCertBegin, pcOTA_PAL_CERT_END);
    if (pucCertEnd == NULL)
    {
        configPRINTF( ( "%s() No END found for Certificate\r\n", __func__) );
        return NULL;
    }

    mbedtls_base64_decode(NULL, 0, &ulDecodedCertificateSize, pucCertBegin, pucCertEnd - pucCertBegin);
    pucDecodedCertificate = (uint8_t *) pvPortMalloc(ulDecodedCertificateSize);
    if (pucDecodedCertificate == NULL)
    {
        configPRINTF( ( "%s() Failed to decode the Certificate\r\n", __func__) );
        return NULL;
    }
    mbedtls_base64_decode(pucDecodedCertificate, ulDecodedCertificateSize, &ulDecodedCertificateSize, pucCertBegin, pucCertEnd - pucCertBegin);

    return pucDecodedCertificate;
}

/**
 * @brief erase the second slot to prepare for writing OTA'ed application
 *
 * @param   N/A
 *
 * @return   0 on success
 *          -1 on error
 */
static int eraseSlotTwo( void )
{
    const struct flash_area *fap ;

    if (flash_area_open(FLASH_AREA_IMAGE_SECONDARY(0), &fap) != 0)
    {
        configPRINTF( ( "%s() flash_area_open(FLASH_AREA_IMAGE_SECONDARY(0)) failed\r\n", __func__) );
        return -1 ;
    }
    if (flash_area_erase(fap, 0, fap->fa_size) != 0)
    {
        configPRINTF( ( "%s() flash_area_erase(fap, 0) failed\r\n", __func__) );
        return -1 ;
    }

    flash_area_close(fap) ;

    return 0 ;
}

/**
 * @brief Verify the signature of the specified file.
 *
 * This function should be implemented if signature verification is not offloaded
 * to non-volatile memory io functions.
 *
 * This function is called from prvPAL_Close().
 *
 * @param[in] C OTA file context information.
 *
 * @return Below are the valid return values for this function.
 * kOTA_Err_None if the signature verification passes.
 * kOTA_Err_SignatureCheckFailed if the signature verification fails.
 * kOTA_Err_BadSignerCert if the if the signature verification certificate cannot be read.
 */
static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * const C )
{
    OTA_Err_t   result = kOTA_Err_None;
    uint32_t    ulSignerCertSize;
    void        *pvSigVerifyContext;
    u8          *pucSignerCert = 0;
    uint32_t    addr;
    const struct flash_area *fap;

    if ( (C == NULL) || (C->lFileHandle == 0) || (C != sys_ctx ) )
    {
        configPRINTF( ("%s() Bad Args C:%p\r\n", __func__, C) );
        return kOTA_Err_SignatureCheckFailed;
    }

    fap = (const struct flash_area *)C->lFileHandle ;

    /* Verify an ECDSA-SHA256 signature. */
    if( CRYPTO_SignatureVerificationStart( &pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_ECDSA,
                                           cryptoHASH_ALGORITHM_SHA256 ) == pdFALSE )
    {
        configPRINTF( ( "%s() CRYPTO_SignatureVerificationStart() failed\r\n", __func__) );
        return kOTA_Err_SignatureCheckFailed;
    }

    pucSignerCert = prvPAL_ReadAndAssumeCertificate( ( const u8 * const ) C->pucCertFilepath, &ulSignerCertSize );

    if( pucSignerCert == NULL )
    {
        configPRINTF( ( "%s() prvPAL_ReadAndAssumeCertificate() failed\r\n", __func__) );
        return kOTA_Err_BadSignerCert;
    }

    addr = 0 ;
    while (addr < C->ulFileSize)
    {
        uint32_t toread = C->ulFileSize - addr ;
        if (toread > sizeof(buffer))
            toread = sizeof(buffer) ;

        if (flash_area_read(fap, addr, buffer, toread) < 0)
        {
            configPRINTF( ( "%s() flash_area_read() failed\r\n", __func__) );
            return kOTA_Err_SignatureCheckFailed ;
        }

        CRYPTO_SignatureVerificationUpdate( pvSigVerifyContext, buffer, toread);

        addr += toread ;

        /* Yield for a bit */
        vTaskDelay(0);
    }

    if( CRYPTO_SignatureVerificationFinal( pvSigVerifyContext, ( char * ) pucSignerCert, ulSignerCertSize,
                                           C->pxSignature->ucData, C->pxSignature->usSize ) == pdFALSE )
    {
        configPRINTF( ( "%s() CRYPTO_SignatureVerificationFinal() failed\r\n", __func__) );
        result = kOTA_Err_SignatureCheckFailed;
    }

    /* Free the signer certificate that we now own after prvReadAndAssumeCertificate(). */
    if( pucSignerCert != NULL )
    {
        vPortFree( pucSignerCert );
    }

    return result;
}

/**
 * @brief Abort an OTA transfer.
 *
 * Aborts access to an existing open file represented by the OTA file context C. This is only valid
 * for jobs that started successfully.
 *
 * @note The input OTA_FileContext_t C is checked for NULL by the OTA agent before this
 * function is called.
 * This function may be called before the file is opened, so the file pointer C->lFileHandle may be NULL
 * when this function is called.
 *
 * @param[in] C OTA file context information.
 *
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent
 * error codes information in aws_ota_agent.h.
 *
 * The file pointer will be set to NULL after this function returns.
 * kOTA_Err_None is returned when aborting access to the open file was successful.
 * kOTA_Err_FileAbort is returned when aborting access to the open file context was unsuccessful.
 */
OTA_Err_t prvPAL_Abort( OTA_FileContext_t * const C )
{
    OTA_Err_t   result = kOTA_Err_None;
    const struct flash_area *fap;

    if ( (C == NULL) || (C != sys_ctx ) )
    {
        configPRINTF( ( "%s() BAD ARGS\r\n", __func__) );
        return kOTA_Err_FileAbort;
    }

    fap = (const struct flash_area *)C->lFileHandle ;
    if (fap != NULL)
    {
        flash_area_close(fap);  /* May have been closed already */
    }

    C->lFileHandle = 0;
    sys_ctx = 0;

    return result;
}

/**
 * @brief Create a new receive file for the data chunks as they come in.
 *
 * @note Opens the file indicated in the OTA file context in the MCU file system.
 *
 * @note The previous image may be present in the designated image download partition or file, so the partition or file
 * must be completely erased or overwritten in this routine.
 *
 * @note The input OTA_FileContext_t C is checked for NULL by the OTA agent before this
 * function is called.
 * The device file path is a required field in the OTA job document, so C->pucFilePath is
 * checked for NULL by the OTA agent before this function is called.
 *
 * @param[in] C OTA file context information.
 *
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent
 * error codes information in aws_ota_agent.h.
 *
 * kOTA_Err_None is returned when file creation is successful.
 * kOTA_Err_RxFileTooLarge is returned if the file to be created exceeds the device's non-volatile memory size contraints.
 * kOTA_Err_BootInfoCreateFailed is returned if the bootloader information file creation fails.
 * kOTA_Err_RxFileCreateFailed is returned for other errors creating the file in the device's non-volatile memory.
 */
OTA_Err_t prvPAL_CreateFileForRx( OTA_FileContext_t * const C )
{
    const struct flash_area *fap;

    if (C == NULL)
    {
        configPRINTF( ( "%s() BAD ARGS\r\n", __func__) );
        return kOTA_Err_RxFileCreateFailed;
    }

    /* prepare the slot for writing */
    configPRINTF( ("%s() erase slot two for file %s\r\n", __func__, C->pucFilePath) );
    eraseSlotTwo() ;
    if (flash_area_open(FLASH_AREA_IMAGE_SECONDARY(0), &fap) != 0)
    {
        configPRINTF( ( "%s() flash_area_open() failed\r\n", __func__) );
        return kOTA_Err_RxFileCreateFailed;
    }

    C->lFileHandle = (int32_t)fap;
    sys_ctx = C;

#if OTA_TEST_VERSION_EARLY
    /* for checking OTA image version asap while downloading */
    ota_version_checked = 0;
#endif

    return kOTA_Err_None;
}

/**
 * @brief Authenticate and close the underlying receive file in the specified OTA context.
 *
 * @note The input OTA_FileContext_t C is checked for NULL by the OTA agent before this
 * function is called. This function is called only at the end of block ingestion.
 * prvPAL_CreateFileForRx() must succeed before this function is reached, so
 * C->lFileHandle(or C->pucFile) is never NULL.
 * The certificate path on the device is a required job document field in the OTA Agent,
 * so C->pucCertFilepath is never NULL.
 * The file signature key is required job document field in the OTA Agent, so C->pxSignature will
 * never be NULL.
 *
 * If the signature verification fails, file close should still be attempted.
 *
 * @param[in] C OTA file context information.
 *
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent
 * error codes information in aws_ota_agent.h.
 *
 * kOTA_Err_None is returned on success.
 * kOTA_Err_SignatureCheckFailed is returned when cryptographic signature verification fails.
 * kOTA_Err_BadSignerCert is returned for errors in the certificate itself.
 * kOTA_Err_FileClose is returned when closing the file fails.
 */
OTA_Err_t prvPAL_CloseFile( OTA_FileContext_t * const C )
{
    const struct flash_area *fap ;
    OTA_Err_t               result = kOTA_Err_None;

    if ( (C == NULL) || (C != sys_ctx ) || (C->lFileHandle == 0))
    {
        configPRINTF( ( "%s() BAD ARGS\r\n", __func__) );
        return kOTA_Err_FileClose;
    }

    configPRINTF( ( "%s()\r\n", __func__) );

    /* we got the data, try to verify it */
    if (C->pxSignature == NULL)
    {
        configPRINTF( ( "%s() No pxSignature from AWS\r\n", __func__) );
        result = kOTA_Err_SignatureCheckFailed ;
    }
    else
    {
        result = prvPAL_CheckFileSignature(C);
        if (result == kOTA_Err_None)
        {
            /* Mark this new OTA image as pending, it will be the permanent
            * bootable image going forward.
            */
            configPRINTF( ("%s() prvPAL_CheckFileSignature() GOOD\r\n", __func__) );
            boot_set_pending(0);
        }
        else
        {
            configPRINTF( ( "%s() prvPAL_CheckFileSignature() failed\r\n", __func__) );
            result = kOTA_Err_SignatureCheckFailed ;
        }
    }

    fap = (const struct flash_area *)C->lFileHandle ;
    flash_area_close(fap);

    return result;
}

/**
 * @brief Write a block of data to the specified file at the given offset.
 *
 * @note The input OTA_FileContext_t C is checked for NULL by the OTA agent before this
 * function is called.
 * The file pointer/handle C->pucFile, is checked for NULL by the OTA agent before this
 * function is called.
 * pcData is checked for NULL by the OTA agent before this function is called.
 * ulBlockSize is validated for range by the OTA agent before this function is called.
 * ulBlockIndex is validated by the OTA agent before this function is called.
 *
 * @param[in] C OTA file context information.
 * @param[in] ulOffset Byte offset to write to from the beginning of the file.
 * @param[in] pcData Pointer to the byte array of data to write.
 * @param[in] ulBlockSize The number of bytes to write.
 *
 * @return The number of bytes written on a success, or a negative error code from the platform abstraction layer.
 */
int16_t prvPAL_WriteBlock( OTA_FileContext_t * const C,
                           uint32_t ulOffset,
                           uint8_t * const pcData,
                           uint32_t ulBlockSize )
{
    const struct flash_area *fap ;

    if ( (C == NULL) || (C != sys_ctx ) || (C->lFileHandle == 0))
    {
        configPRINTF( ( "%s() BAD ARGS\r\n", __func__ ) );
        return -1;
    }

    fap = (const struct flash_area *)C->lFileHandle ;
    if (flash_area_write(fap, ulOffset, pcData, ulBlockSize) != 0)
    {
        configPRINTF( ( "%s() flash_area_write() failed\r\n", __func__) );
        return -1;
    }

#if OTA_TEST_VERSION_EARLY
    /* Check for version of incoming image.
     * If version <= current version, bail on the update.
     */
    if ( (ulOffset + ulBlockSize) > sizeof(struct image_header) && (ota_version_checked == 0))
    {
        struct image_header *hdr;
        const struct flash_area *fap;

        if (flash_area_open(FLASH_AREA_IMAGE_SECONDARY(0), &fap) == 0)
        {
            flash_area_close(fap);

            hdr = (struct image_header *)fap->fa_off;
            if (hdr->ih_magic == IMAGE_MAGIC)
            {
                if ( (hdr->ih_ver.iv_major < APP_VERSION_MAJOR) ||
                     ( (hdr->ih_ver.iv_major == APP_VERSION_MAJOR) &&
                       (hdr->ih_ver.iv_minor < APP_VERSION_MINOR)) ||
                     ( (hdr->ih_ver.iv_major == APP_VERSION_MAJOR) &&
                       (hdr->ih_ver.iv_minor == APP_VERSION_MINOR) &&
                       (hdr->ih_ver.iv_build_num <= APP_VERSION_BUILD)))
                {
                    configPRINTF( ( "%s() OTA image version %d.%d.%d <= current %d.%d.%d-- bail!\r\n", __func__,
                            hdr->ih_ver.iv_major, hdr->ih_ver.iv_minor, hdr->ih_ver.iv_build_num,
                            APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_BUILD) );

                    return -1;
                }
                /* we've checked the version, don't need to do it again */
                ota_version_checked = 1;
            }
            else
            {
                configPRINTF( ( "%s() OTA image magic invalid 0x%x != 0x%x-- bail!\r\n", __func__,
                        hdr->ih_magic, IMAGE_MAGIC) );

                return -1;
            }
        }
    }
#endif

    return ulBlockSize;
}

/**
 * @brief Activate the newest MCU image received via OTA.
 *
 * This function shall do whatever is necessary to activate the newest MCU
 * firmware received via OTA. It is typically just a reset of the device.
 *
 * @note This function SHOULD not return. If it does, the platform doesn't support
 * an automatic reset or an error occurred.
 *
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent
 * error codes information in aws_ota_agent.h.
 */
OTA_Err_t prvPAL_ActivateNewImage( void )
{
    configPRINTF( ("%s() \r\n", __func__) );
    prvPAL_ResetDevice();
    return kOTA_Err_None;
}

/**
 * @brief Reset the device.
 *
 * This function shall reset the MCU and cause a reboot of the system.
 *
 * @note This function SHOULD not return. If it does, the platform doesn't support
 * an automatic reset or an error occurred.
 *
 * @return The OTA PAL layer error code combined with the MCU specific error code. See OTA Agent
 * error codes information in aws_ota_agent.h.
 */
OTA_Err_t prvPAL_ResetDevice( void )
{
    /* we want to wait a bit when in DEBUG builds so the logging mechanism can finish before resetting */
    vTaskDelay(pdMS_TO_TICKS( 1000UL ));
    printf("%s()   RESETTING NOW !!!!\r\n", __func__);
    vTaskDelay(pdMS_TO_TICKS( 100UL ));
    NVIC_SystemReset() ;
    return kOTA_Err_None ;
}

/**
 * @brief Attempt to set the state of the OTA update image.
 *
 * Do whatever is required by the platform to Accept/Reject the OTA update image (or bundle).
 * Refer to the PAL implementation to determine what happens on your platform.
 *
 * @param[in] eState The desired state of the OTA update image.
 *
 * @return The OTA_Err_t error code combined with the MCU specific error code. See aws_ota_agent.h for
 *         OTA major error codes and your specific PAL implementation for the sub error code.
 *
 * Major error codes returned are:
 *
 *   kOTA_Err_None on success.
 *   kOTA_Err_BadImageState: if you specify an invalid OTA_ImageState_t. No sub error code.
 *   kOTA_Err_AbortFailed: failed to roll back the update image as requested by eOTA_ImageState_Aborted.
 *   kOTA_Err_RejectFailed: failed to roll back the update image as requested by eOTA_ImageState_Rejected.
 *   kOTA_Err_CommitFailed: failed to make the update image permanent as requested by eOTA_ImageState_Accepted.
 *
 *
 *   IMPORTANT NOTES:
 *
 *   Our implementation checks the signature on the call to prvPAL_CloseFile().
 *   We assume that if the updated app is running in slot 0 that it is acceptable.
 *
 *   if prvPAL_SetPlatformImageState() is called and the sys_ctx is NULL, we are not in the process of downloading a new image.
 *      if the eState is Rejected or Aborted, it pertains to the in-process download (or invalid job) for Secondary Slot.
 *      We could erase the secondary slot, but we do that each new download start, and will not consider new download OK if not complete.
 *
 */
OTA_Err_t prvPAL_SetPlatformImageState( OTA_ImageState_t eState )
{
    OTA_Err_t   result = kOTA_Err_None;

    PRINT_eSTATE( "-------------------------> prvPAL_SetPlatformImageState() curr eState:", current_OTA_ImageState);
    PRINT_eSTATE( "-------------------------> prvPAL_SetPlatformImageState() new  eSTATE:", eState);
    PRINT_SYSTEM_CONTEXT_PTR();
    PRINT_BOOT_SWAP_TYPE(NULL, boot_swap_type());


    if (sys_ctx == NULL)
    {
        /* We are not currently loading an OTA image. This state change pertains to Slot 0 image */
        switch (eState)
        {
        case eOTA_ImageState_Accepted:
            /* Mark Slot 0 image as valid */
            boot_set_confirmed();
//            configPRINTF( ("%s() Accepted <----------------------------------<<\r\n", __func__) );
            break;
        case eOTA_ImageState_Rejected:
        case eOTA_ImageState_Aborted:
            /* We are not actively downloading, this pertains to an invalid download job
             * or an aborted job. We erase secondary slot when we start a new download,
             * and do not consider secondary slot ready unless dowlnoad completes and
             * signature checked. nothing to do here, but set our state to Accepted so
             * we store it for GetImageState().
             */
            eState = eOTA_ImageState_Accepted;
            break;
        case eOTA_ImageState_Testing:
//            configPRINTF( ("%s() OTA Image in TESTING.\r\n", __func__) );
            break;
        default:
            configPRINTF( ("%s() New In-process OTA Unknown state %d.\r\n", __func__, eState) );
            result = kOTA_Err_BadImageState;
            break;
        }
    }
    else
    {
        /* We are in the middle of a new download
         * Abort or Reject refers to the new download, not the primary slot.
         * Not what to do here, if anything...
         */
    }

    /* keep track of the state OTA Agent sent. */
    current_OTA_ImageState = eState;

    PRINT_BOOT_SWAP_TYPE(NULL, boot_type);
    PRINT_eSTATE( "-------------------------> prvPAL_SetPlatformImageState() current  eSTATE:", eState);
    PRINT_SYSTEM_CONTEXT_PTR();
    PRINT_BOOT_SWAP_TYPE(NULL, boot_type);

    return result;
}


/**
 * @brief Get the state of the OTA update image.
 *
 * We read this at OTA_Init time and when the latest OTA job reports itself in self
 * test. If the update image is in the "pending commit" state, we start a self test
 * timer to assure that we can successfully connect to the OTA services and accept
 * the OTA update image within a reasonable amount of time (user configurable). If
 * we don't satisfy that requirement, we assume there is something wrong with the
 * firmware and automatically reset the device, causing it to roll back to the
 * previously known working code.
 *
 * If the update image state is not in "pending commit," the self test timer is
 * not started.
 *
 * @return An OTA_PAL_ImageState_t. One of the following:
 *   eOTA_PAL_ImageState_PendingCommit (the new firmware image is in the self test phase)
 *   eOTA_PAL_ImageState_Valid         (the new firmware image is already committed)
 *   eOTA_PAL_ImageState_Invalid       (the new firmware image is invalid or non-existent)
 *
 *   NOTE: eOTA_PAL_ImageState_Unknown should NEVER be returned and indicates an implementation error.
 *
 * We are testing the New application.
 * BOOT_SWAP_TYPE_NONE     1
 * BOOT_SWAP_TYPE_TEST     2    Swap to the secondary slot. Absent a confirm command (STDE - Valid?), revert back on next boot.
 * BOOT_SWAP_TYPE_PERM     3    Swap to the secondary slot, and permanently switch to booting its contents.
 * BOOT_SWAP_TYPE_REVERT   4    Swap back to alternate slot.  A confirm (STDE - Valid?) changes this state to NONE.
 * BOOT_SWAP_TYPE_FAIL     5    Swap failed because image to be run is not valid
 * BOOT_SWAP_TYPE_PANIC    0xff Swapping encountered an unrecoverable error
 */

OTA_PAL_ImageState_t prvPAL_GetPlatformImageState( void )
{
    OTA_PAL_ImageState_t result = eOTA_PAL_ImageState_Unknown;

    PRINT_eSTATE(  "<------------------------- prvPAL_GetPlatformImageState() current  eSTATE:", current_OTA_ImageState);
    PRINT_SYSTEM_CONTEXT_PTR();
    PRINT_BOOT_SWAP_TYPE(NULL, boot_swap_type());

    /**
     * After swap/copy of secondary slot to primary slot, boot_swap_type() returns NONE.
     * It does not reflect the fact we may be in self-test mode.
     * Use the saved value from SetImageState() to report our status.
     */
    if( current_OTA_ImageState == eOTA_ImageState_Testing )
    {
        /* in self-test, report Pending. */
        result = eOTA_PAL_ImageState_PendingCommit;
    }
    else
    {
        result = eOTA_PAL_ImageState_Valid;
    }

    PRINT_PAL_STATE( "<------------------------- prvPAL_GetPlatformImageState() DONE: ", result);
    return result ;
}
