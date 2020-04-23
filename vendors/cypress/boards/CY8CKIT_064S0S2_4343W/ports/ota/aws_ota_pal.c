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
 * NOTE: This implementation will handle a single NSPE binary file (normal application) or
 *       a tarball which has a "components.json" describing the files contained therein
 *       "components.json" is generated during the build process and added to the tarball.
 *
 * NOTE: When using the tarball OTA file, you must use HTTP transport (not MQTT)
 *       When using HTTP transport, the AFRTOS system heap must be increased (300k seems to work well)
 *       see projects/cypress/<board>/mtb/<appname>/config_files/FreeRTOSConfig.h
 *       #define configTOTAL_HEAP_SIZE                      ( ( size_t ) ( 300 * 1024 ) )
 */

/* C Runtime includes. */
#include <stdlib.h>
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

/* For aws_tests support */
#include "aws_ota_agent_config.h"

/* Added for Flash Read */
#include "flash_map_backend/flash_map_backend.h"
#include "flash_qspi/flash_qspi.h"
#include "cy_smif_psoc6.h"

#include "untar.h"

/* For tarball support */
/**
 * @brief Tarball support file types recognized in components.json file
 *
 * The file type in the tarball
 */
#define CY_FILE_TYPE_SPE        "SPE"       /**< Secure Programming Environment (TFM) code type               */
#define CY_FILE_TYPE_NSPE       "NSPE"      /**< Non-Secure Programming Environment (application) code type   */

/**
 * @brief File Signature Key
 *
 * The OTA signature algorithm we support on this platform.
 */
const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";

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

#if 1   /* Debugging macros */
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
#define PRINT_SYSTEM_CONTEXT_PTR()  configPRINTF(("   sys_ctx: %p\n", sys_ctx));

#define PRINT_BOOT_SWAP_TYPE( title, boot_type ) \
    configPRINTF( ( "%s:: boot_swap_type:%d (%s)\n", ((title == NULL)?"":title), \
             boot_type, ((boot_type == 0) ? "Unknown" :             \
              (boot_type == BOOT_SWAP_TYPE_NONE) ? "None" :         \
              (boot_type == BOOT_SWAP_TYPE_TEST) ? "Test" :         \
              (boot_type == BOOT_SWAP_TYPE_PERM) ? "Permanent" :    \
              (boot_type == BOOT_SWAP_TYPE_REVERT) ? "Revert" :     \
              (boot_type == BOOT_SWAP_TYPE_FAIL) ? "FAIL" :         \
              (boot_type == BOOT_SWAP_TYPE_PANIC) ? "PANIC !!" : "Bad State")   ) );

#define PRINT_eSTATE( title, state ) \
    configPRINTF( ( "%s::   Sys Image State: 0x%lx (%s) \n", ((title == NULL)?"":title), \
             state, ((state == eOTA_ImageState_Unknown) ? "Unknown" :  \
             (state == eOTA_ImageState_Testing) ? "Testing" :          \
             (state == eOTA_ImageState_Accepted) ? "Accepted" :        \
             (state == eOTA_ImageState_Rejected) ? "Rejected" :        \
             (state == eOTA_ImageState_Aborted) ? "Aborted" : "ERROR") ) );

#define PRINT_PAL_STATE( title, palState ) \
    configPRINTF( ( "%s::   PAL Image State: 0x%lx (%s) \n", ((title == NULL)?"":title), \
            palState, ((palState == eOTA_PAL_ImageState_Unknown) ? "Unknown" :  \
             (palState == eOTA_PAL_ImageState_PendingCommit) ? "Pending" :          \
             (palState == eOTA_PAL_ImageState_Valid) ? "Valid" :        \
             (palState == eOTA_PAL_ImageState_Invalid) ? "Invalid" : "ERROR") ) );

#endif  /* Debugging Macros */

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
 * @brief Ptr to system context
 *
 * Keep track of system context between calls from the OTA Agent
 *
 */
const OTA_FileContext_t *sys_ctx;

/**
 * @brief Current OTA Image State
 *
 * Keep track of the state OTA Agent wants us to be in.
 */
static OTA_ImageState_t current_OTA_ImageState = eOTA_ImageState_Unknown;

/**
 * @brief Last time we ran a signature check
 *
 * Keep track of the last signature check value for prvPAL_SetPlatformImageState
 */
static OTA_Err_t last_signature_check;
/***********************************************************************
 *
 * UNTAR variables
 *
 **********************************************************************/

/**
 * @brief Context structure for parsing the tar file
 */
cy_untar_context_t  ota_untar_context;

/**
 * @brief Flag to denote this is a tar file
 *
 * We use this flag on subsequent chunks of file to know how to handle the data
 */
int ota_is_tar_archive;

/**
 * @brief Signature Verification Context
 */
void        *pvSigVerifyContext = NULL;

/**
 * @brief Signature Certificate size
 */
uint32_t    ulSignerCertSize;

/**
 * @brief pointer to Signer Certificate
 */
u8          *pucSignerCert = NULL;

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
    uint8_t *pucDecodedCertificate;
    size_t ulDecodedCertificateSize;

    *ulSignerCertSize = sizeof(signingcredentialSIGNING_CERTIFICATE_PEM);
    /* Skip the "BEGIN CERTIFICATE" */
    uint8_t* pucCertBegin = (uint8_t *)strstr (signingcredentialSIGNING_CERTIFICATE_PEM, pcOTA_PAL_CERT_BEGIN);
    if (pucCertBegin == NULL)
    {
        configPRINTF( ( "%s() No Begin found for Certificate\n", __func__) );
        return NULL;
    }
    pucCertBegin += sizeof(pcOTA_PAL_CERT_BEGIN);

    /* Find the "END CERTIFICATE" */
    pucCertEnd =  (uint8_t *)strstr((char *)pucCertBegin, pcOTA_PAL_CERT_END);
    if (pucCertEnd == NULL)
    {
        configPRINTF( ( "%s() No END found for Certificate\n", __func__) );
        return NULL;
    }

    mbedtls_base64_decode(NULL, 0, &ulDecodedCertificateSize, pucCertBegin, pucCertEnd - pucCertBegin);
    pucDecodedCertificate = (uint8_t *) pvPortMalloc(ulDecodedCertificateSize);
    if (pucDecodedCertificate == NULL)
    {
        configPRINTF( ( "%s() Failed to decode the Certificate\n", __func__) );
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
    const struct flash_area *fap;

    if (flash_area_open(FLASH_AREA_IMAGE_SECONDARY(0), &fap) != 0)
    {
        configPRINTF( ( "%s() flash_area_open(FLASH_AREA_IMAGE_SECONDARY(0)) failed\n", __func__) );
        return -1;
    }
    if (flash_area_erase(fap, 0, fap->fa_size) != 0)
    {
        configPRINTF( ( "%s() flash_area_erase(fap, 0) failed\n", __func__) );
        return -1;
    }

    flash_area_close(fap);

    return 0;
}

/**
 * @brief callback to handle tar data
 *
 * @param ctxt          untar context
 * @param file_index    index into ctxt->files for the data
 * @param buffer        data to use
 * @param file_offset   offset into the file to store data
 * @param chunk_size    amount of data in buffer to use
 * @param cb_arg        argument passed into initialization
 *
 * return   CY_RSLT_SUCCESS
 *          CY_RSLT_TYPE_ERROR
 */
cy_untar_result_t ota_untar_write_callback(cy_untar_context_ptr ctxt,
                                   uint16_t file_index,
                                   uint8_t *buffer,
                                   uint32_t file_offset,
                                   uint32_t chunk_size,
                                   void *cb_arg)
{
    int type = 0;
    const struct flash_area *fap;
    OTA_FileContext_t * const C = (OTA_FileContext_t *)cb_arg;

    if ( (ctxt == NULL) || (buffer == NULL) || (C == NULL) )
    {
        return CY_UNTAR_ERROR;
    }

    //configPRINTF( ("%d:%s FILE %d name %s : %s\n", __LINE__, __func__, file_index, ctxt->files[file_index].name, ctxt->files[file_index].type) );
    if ( strncmp(ctxt->files[file_index].type, CY_FILE_TYPE_SPE, strlen(CY_FILE_TYPE_SPE)) == 0)
    {

        type = 1;
    }
    else if ( strncmp(ctxt->files[file_index].type, CY_FILE_TYPE_NSPE, strlen(CY_FILE_TYPE_NSPE)) == 0)
    {
        type = 0;
    }
    else
    {
        /* unknown file type */
        configPRINTF( ("%d:%s BAD FILE TYPE : >%s<\n", __LINE__, __func__, ctxt->files[file_index].type) );
        return CY_UNTAR_ERROR;
    }

    if (flash_area_open(FLASH_AREA_IMAGE_SECONDARY(type), &fap) != 0)
    {
        configPRINTF( ( "%s() flash_area_open(%d) failed\n", __func__, type) );
        return CY_UNTAR_ERROR;
    }
    if (flash_area_write(fap, file_offset, buffer, chunk_size) != 0)
    {
        configPRINTF( ( "%s() flash_area_write() failed\n", __func__) );
        flash_area_close(fap);
        return CY_UNTAR_ERROR;
    }
    flash_area_close(fap);

    return CY_UNTAR_SUCCESS;
}

/**
 * @brief Initialization routine for handling tarball OTA file
 *
 * @param ctxt[in,out]  pointer to untar context to be initialized
 * @param C[in]         pointer to system OTA context
 *
 */
cy_rslt_t ota_untar_init_context( cy_untar_context_t* ctxt, OTA_FileContext_t * const C )
{
    if (cy_untar_init( ctxt, ota_untar_write_callback, (void *)C ) == CY_RSLT_SUCCESS)
    {
        ota_is_tar_archive  = 1;
        return CY_RSLT_SUCCESS;
    }
    return CY_RSLT_TYPE_ERROR;
}

/**
 * @brief De Initialize the signature checking data
 *
 */
static void prvPAL_FileSignatureCheckDeinit( void )
{
    pvSigVerifyContext = NULL;
    ulSignerCertSize = 0;
    if (pucSignerCert  != NULL)
    {
        vPortFree( pucSignerCert );
        pucSignerCert = NULL;
    }
}

/**
 * @brief Initialize the signature checking for tarball OTA
 *        We keep track of the interim values for subsequent WriteBlock calls.
 *
 * NOTE: This function is used for a tarball OTA file.
 *
 * This function is called from prvPAL_CreateFile().
 *
 * @param[in] C OTA file context information.
 *
 * @return Below are the valid return values for this function.
 * kOTA_Err_None if the signature verification passes.
 * kOTA_Err_SignatureCheckFailed if the signature verification fails.
 * kOTA_Err_BadSignerCert if the if the signature verification certificate cannot be read.
 */
static OTA_Err_t prvPAL_FileSignatureCheckInit( OTA_FileContext_t * const C )
{
    if ( C == NULL )
    {
        configPRINTF( ("%s() Bad Args C:%p\n", __func__, C) );
        return kOTA_Err_SignatureCheckFailed;
    }

    /* Reset globals for incremental sign check and handle re-entrant calls */
    prvPAL_FileSignatureCheckDeinit();

    /* Verify an ECDSA-SHA256 signature. */
    if( CRYPTO_SignatureVerificationStart( &pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_ECDSA,
                                           cryptoHASH_ALGORITHM_SHA256 ) == pdFALSE )
    {
        configPRINTF( ( "%s() CRYPTO_SignatureVerificationStart() failed\n", __func__) );
        return kOTA_Err_SignatureCheckFailed;
    }

    pucSignerCert = prvPAL_ReadAndAssumeCertificate( ( const u8 * const ) C->pucCertFilepath, &ulSignerCertSize );
    if( pucSignerCert == NULL )
    {
        configPRINTF( ( "%s() prvPAL_ReadAndAssumeCertificate() failed\n", __func__) );
        return kOTA_Err_BadSignerCert;
    }
    return kOTA_Err_None;
}

/**
 * @brief incremental update of the signature for tarball OTA
 *
 * NOTE: This function is used for a tarball OTA file.
 *
 * This function is called from prvPAL_WriteBlock().
 *
 * @param[in] C         OTA file context information.
 * @param[in] buffer    current buffer to include in signature check
 * @param[in] size      size of buffer
 *
 * @return Below are the valid return values for this function.
 * kOTA_Err_None if the signature verification passes.
 * kOTA_Err_SignatureCheckFailed if the signature verification fails.
 * kOTA_Err_BadSignerCert if the if the signature verification certificate cannot be read.
 */
static OTA_Err_t prvPAL_FileSignatureCheckStep( OTA_FileContext_t * const C, uint8_t * buffer, uint32_t size )
{
    if ( (C == NULL) || (C != sys_ctx ) || (pvSigVerifyContext == NULL) )
    {
        configPRINTF( ("%s() Bad Args C:%p\n", __func__, C) );
        /* Free the signer certificate that we now own after prvReadAndAssumeCertificate(). */
        prvPAL_FileSignatureCheckDeinit();
        if (pvSigVerifyContext == NULL)
        {
            return kOTA_Err_BadSignerCert;
        }
        return kOTA_Err_SignatureCheckFailed;
    }
    CRYPTO_SignatureVerificationUpdate( pvSigVerifyContext, buffer, size);
    return kOTA_Err_None;
}

/**
 * @brief Do the final Signature Check for tarball OTA
 *
 * NOTE: This function is used for a tarball OTA file.
 *
 * This function is called from prvPAL_CloseFile().
 *
 * @param[in] C         OTA file context information.
 *
 * @return Below are the valid return values for this function.
 * kOTA_Err_None if the signature verification passes.
 * kOTA_Err_SignatureCheckFailed if the signature verification fails.
 * kOTA_Err_BadSignerCert if the if the signature verification certificate cannot be read.
 */
static OTA_Err_t prvPAL_FileSignatureCheckFinal( OTA_FileContext_t * const C )
{
    OTA_Err_t   result = kOTA_Err_None;

    if ( (C == NULL) || (C != sys_ctx ) || (pvSigVerifyContext == NULL) || (pucSignerCert == NULL) || (ulSignerCertSize == 0) )
    {
        /* Free the signer certificate that we now own after prvReadAndAssumeCertificate(). */
        prvPAL_FileSignatureCheckDeinit();
        configPRINTF( ( "%s() failed\n", __func__) );
        return kOTA_Err_BadSignerCert;
    }

    if( CRYPTO_SignatureVerificationFinal( pvSigVerifyContext, ( char * ) pucSignerCert, ulSignerCertSize,
                                           C->pxSignature->ucData, C->pxSignature->usSize ) == pdFALSE )
    {
        configPRINTF( ( "%s() CRYPTO_SignatureVerificationFinal() failed\n", __func__) );
        result = kOTA_Err_SignatureCheckFailed;
    }
    last_signature_check = result;

    /* Free the signer certificate that we now own after prvReadAndAssumeCertificate(). */
    prvPAL_FileSignatureCheckDeinit();
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

    if ( (C != NULL) && (C->lFileHandle == (uint32_t)NULL) )
    {
        return kOTA_Err_None;
    }

    if ( (C == NULL) || (C != sys_ctx ) )
    {
        configPRINTF( ( "%s() BAD ARGS\n", __func__) );
        return kOTA_Err_FileAbort;
    }

    fap = (const struct flash_area *)C->lFileHandle;
    if (fap != NULL)
    {
        flash_area_close(fap);  /* May have been closed already */
    }

    /* reset our globals */
    C->lFileHandle = 0;
    sys_ctx = 0;

    /* Free the signer certificate that we now own after prvReadAndAssumeCertificate(). */
    prvPAL_FileSignatureCheckDeinit();
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
    OTA_Err_t   result = kOTA_Err_None;
    const struct flash_area *fap;

    if (C == NULL)
    {
        configPRINTF( ( "%s() BAD ARGS\n", __func__) );
        return kOTA_Err_RxFileCreateFailed;
    }

    /* prepare the slot for writing */
    eraseSlotTwo();

    /* Must set something into lFileHandle and we use for MQTT downloads */
    if (flash_area_open(FLASH_AREA_IMAGE_SECONDARY(0), &fap) != 0)
    {
        configPRINTF( ( "%s() flash_area_open() failed\n", __func__) );
        return kOTA_Err_RxFileCreateFailed;
    }

    /* NOTE: lFileHandle MUST be non-NULL of the OTA Agent will error out */
    C->lFileHandle = (int32_t)fap;
    sys_ctx = C;

    /* initialize these for checking later */
    last_signature_check = kOTA_Err_Uninitialized;
    pvSigVerifyContext = NULL;
    pvSigVerifyContext = NULL;
    pucSignerCert = NULL;
    ulSignerCertSize = 0;
    return result;
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
    OTA_Err_t               result = kOTA_Err_None;

    if ( (C == NULL) || (C != sys_ctx ) || (C->lFileHandle == 0))
    {
        configPRINTF( ( "%s() BAD ARGS\n", __func__) );
        result = kOTA_Err_FileClose;
        goto _exit_CloseFile;
    }

    /* we got the data, try to verify it */
    if (C->pxSignature == NULL)
    {
        configPRINTF( ( "%s() No pxSignature from AWS\n", __func__) );
        result = kOTA_Err_SignatureCheckFailed;
        goto _exit_CloseFile;
    }

    /* For TAR files, we must use HTTP transport so that the file blocks come down
     * in sequential order - we need start of TAR (components.json) first, and we
     * do not store all of the blocks. Signature must be checked in sequential order.
     */
    if (ota_is_tar_archive == 1)
    {
        /* Do the final check for signature */
        if (prvPAL_FileSignatureCheckFinal(C) == kOTA_Err_None)
        {
            /* Mark this new OTA image as pending, it will be the permanent
            * bootable image going forward.
            */
            configPRINTF( ("%s() TAR prvPAL_FileSignatureCheckFinal() GOOD\n", __func__) );
            boot_set_pending(0);
        }
        else
        {
            configPRINTF( ( "%s() TAR prvPAL_FileSignatureCheckFinal() failed\n", __func__) );
            eraseSlotTwo();
            result = kOTA_Err_SignatureCheckFailed;
        }
    }
    else
    {
        /* non-tar OTA, we downloaded a single file, might have been MQTT.
         * If MQTT, the file blocks may come in non-sequential order.
         * We need to run signature check across entire downloaded file at once.
         */
        const struct flash_area *fap;

        //configPRINTF( ( "%s() BINARY file, check whole file for signature\n", __func__) );
        fap = (const struct flash_area *)C->lFileHandle;
        if (fap == 0)
        {
            configPRINTF( ( "%s() ERROR: fap == NULL\n", __func__) );
            result = kOTA_Err_SignatureCheckFailed;
            goto _exit_CloseFile;
        }

        result = prvPAL_FileSignatureCheckInit( C );
        if (result == kOTA_Err_None)
        {
            uint32_t addr;
            addr = 0;
            while (addr < C->ulFileSize)
            {
                uint32_t toread = C->ulFileSize - addr;
                /* Re-use coalesce buffer as we have completed download (it is not used for non-TAR OTA) */
                if (toread > sizeof(ota_untar_context.coalesce_buffer) )
                    toread = sizeof(ota_untar_context.coalesce_buffer);

                if (flash_area_read(fap, addr, ota_untar_context.coalesce_buffer, toread) < 0)
                {
                    configPRINTF( ( "%s() flash_area_read() failed for signature check\r\n", __func__) );
                    result = kOTA_Err_SignatureCheckFailed;
                    goto _exit_CloseFile;
                }

                CRYPTO_SignatureVerificationUpdate( pvSigVerifyContext, ota_untar_context.coalesce_buffer, toread);

                addr += toread;
            }
            if (prvPAL_FileSignatureCheckFinal(C) == kOTA_Err_None)
            {
                /* Mark this new OTA image as pending, it will be the permanent
                * bootable image going forward.
                */
                configPRINTF( ("%s() BIN prvPAL_FileSignatureCheckFinal() GOOD\n", __func__) );
                boot_set_pending(0);
            }
            else
            {
                configPRINTF( ( "%s() BIN prvPAL_FileSignatureCheckFinal() failed\n", __func__) );
                eraseSlotTwo();
                result = kOTA_Err_SignatureCheckFailed;
            }
        }
    }

_exit_CloseFile:

    if ( C->lFileHandle != 0 )
    {
        const struct flash_area *fap;
        fap = (const struct flash_area *)C->lFileHandle;
        flash_area_close(fap);
    }

    /* Free the signer certificate that we now own after prvReadAndAssumeCertificate(). */
    prvPAL_FileSignatureCheckDeinit();

    if (result != kOTA_Err_None)
    {
        current_OTA_ImageState = eOTA_ImageState_Unknown;
    }
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

    if ( (C == NULL) || (C != sys_ctx ) )
    {
        configPRINTF( ( "%s() BAD ARGS\n", __func__ ) );
        return -1;
    }

    /* we need to check some things when we receive the first block */
    if (ulOffset == 0UL)
    {

        /*
         * initialize file signature checking
         */
        if (prvPAL_FileSignatureCheckInit(C) != kOTA_Err_None)
        {
            configPRINTF( ( "%s() prvPAL_FileSignatureCheckInit() FAILED\n", __func__ ) );
            return -1;
        }

        /*
         * Check for incoming tarball (as opposed to a single file OTA)
         */
        if (cy_is_tar_header( pcData, ulBlockSize) == CY_UNTAR_SUCCESS)
        {
            if (ota_untar_init_context(&ota_untar_context, C) != CY_RSLT_SUCCESS)
            {
                configPRINTF( ( "%s() ota_untar_init_context() FAILED! \n", __func__) );
                return -1;
            }
        }
    }

    /* treat a tar file differently from a "normal" OTA */
    if (ota_is_tar_archive != 0)
    {
        uint32_t consumed = 0;

        /* check the signature incrementally over every block received */
        if (prvPAL_FileSignatureCheckStep( C, pcData, ulBlockSize) != kOTA_Err_None)
        {
            configPRINTF( ( "%s() FileSignatureCheckStep() offset:%ld FAILED!\n", __func__, ulOffset) );
            return -1;
        }

        while( consumed < ulBlockSize )
        {
            cy_untar_result_t result;
            result = cy_untar_parse(&ota_untar_context, (ulOffset + consumed), &pcData[consumed], (ulBlockSize- consumed), &consumed);
            if ( (result == CY_UNTAR_ERROR) || (result == CY_UNTAR_INVALID))
            {
                configPRINTF( ( "%s() cy_untar_parse() FAIL consumed: %ld sz:%ld result:%ld)!\n", __func__, consumed, ulBlockSize, result) );
                return -1;
            }
            /* Yield for a bit */
            vTaskDelay(1);
        }

        /* with the tarball we get a version - check if it is > current so we can bail early */
        if (ota_untar_context.version[0] != 0)
        {
            /* example version string "<major>.<minor>.<build>" */
            uint16_t major = 0;
            uint16_t minor = 0;
            uint16_t build = 0;
            char *dot;
            major = atoi(ota_untar_context.version);
            dot = strstr(ota_untar_context.version, ".");
            if (dot != NULL)
            {
                dot++;
                minor = atoi(dot);
                dot = strstr(dot, ".");
                if (dot != NULL)
                {
                    dot++;
                    build = atoi(dot);

                    if ( (major < APP_VERSION_MAJOR) ||
                          ( (major == APP_VERSION_MAJOR) &&
                            (minor < APP_VERSION_MINOR)) ||
                          ( (major == APP_VERSION_MAJOR) &&
                            (minor == APP_VERSION_MINOR) &&
                            (build <= APP_VERSION_BUILD)))
                     {
                         configPRINTF( ( "%s() OTA image version %d.%d.%d <= current %d.%d.%d-- bail!\r\n", __func__,
                                 major, minor, build,
                                 APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_BUILD) );

                         return -1;
                     }
                }
            }
        }
    }
    else
    {
        /* non-tarball OTA here */
        const struct flash_area *fap;
        fap = (const struct flash_area *)C->lFileHandle;
        if (fap == NULL)
        {
            configPRINTF( ( "%s() flash_area_pointer is NULL\n", __func__) );
            return -1;
        }
        if (flash_area_write(fap, ulOffset, pcData, ulBlockSize) != 0)
        {
            configPRINTF( ( "%s() flash_area_write() FAILED\n", __func__) );
            return -1;
        }
    }
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
    configPRINTF( ("%s() \n", __func__) );
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
    configPRINTF( ("%s()   RESETTING NOW !!!!\n", __func__) );
    vTaskDelay(pdMS_TO_TICKS( 1000UL ));
    NVIC_SystemReset();
    return kOTA_Err_None;
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

    if( eState == eOTA_ImageState_Unknown || eState > eOTA_LastImageState )
    {
        return kOTA_Err_BadImageState;
    }

    if (sys_ctx == NULL)
    {
        /* We are not currently loading an OTA image. This state change pertains to Slot 0 image */
        switch (eState)
        {
        case eOTA_ImageState_Accepted:
            /* Mark Slot 0 image as valid */
            // We need to know if the last check was good...
            if (last_signature_check == kOTA_Err_None)
            {
                boot_set_confirmed();
            }
            else
            {
                result = kOTA_Err_CommitFailed;
            }
            break;
        case eOTA_ImageState_Rejected:
            /* we haven't closed the file, and the OTA Agent has rejected the download */
            result = kOTA_Err_None;
            break;
        case eOTA_ImageState_Aborted:
            /* We are not actively downloading, this pertains to an invalid download job
             * or an aborted job. We erase secondary slot when we start a new download,
             * and do not consider secondary slot ready unless download completes and
             * signature checked. nothing to do here, but set our state to Accepted so
             * we store it for GetImageState().
             */
            eState = eOTA_ImageState_Accepted;
            break;
        case eOTA_ImageState_Testing:
            break;
        default:
            result = kOTA_Err_BadImageState;
            break;
        }
    }
    else
    {
        /* We are in the middle of a new download
         * Abort or Reject refers to the new download, not the primary slot.
         * Nothing to do here.
         */
        if (eState == eOTA_ImageState_Accepted)
        {
            if (sys_ctx->lFileHandle != (uint32_t)NULL)
            {
                result = kOTA_Err_CommitFailed;
            }
        }
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
    if (current_OTA_ImageState == eOTA_ImageState_Testing)
    {
        /* in self-test, report Pending. */
        result = eOTA_PAL_ImageState_PendingCommit;
    }
    else if ( (current_OTA_ImageState == eOTA_ImageState_Rejected ) ||
              (current_OTA_ImageState == eOTA_ImageState_Aborted ) )
    {
        result = eOTA_PAL_ImageState_Invalid;
    }
    else if (current_OTA_ImageState == eOTA_ImageState_Accepted)
    {
        result = eOTA_PAL_ImageState_Valid;
    }
    else
    {
        //result = eOTA_PAL_ImageState_Valid;
        result = eOTA_ImageState_Unknown;
    }

    PRINT_PAL_STATE( "<------------------------- prvPAL_GetPlatformImageState() DONE: ", result);
    return result;
}

/*-----------------------------------------------------------*/

/* Provide access to private members for testing. */
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
    #include "aws_ota_pal_test_access_define.h"
#endif
