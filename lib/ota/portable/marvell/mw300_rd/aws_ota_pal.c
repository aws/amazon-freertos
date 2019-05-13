/*
 * Amazon FreeRTOS OTA PAL
 *
 * (C) Copyright 2018-2019 Marvell International Ltd. All Rights Reserved.
 *
 * MARVELL CONFIDENTIAL
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Marvell International Ltd or its
 * suppliers or licensors. Title to the Material remains with Marvell
 * International Ltd or its suppliers and licensors. The Material contains
 * trade secrets and proprietary and confidential information of Marvell or its
 * suppliers and licensors. The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may be
 * used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Marvell's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Marvell in writing.
 */

/* C Runtime includes. */
#include <stdio.h>
#include <stdlib.h>

/* Amazon FreeRTOS include. */
#include "FreeRTOS.h"
#include "aws_ota_pal.h"
#include "aws_ota_agent_internal.h"
#include "aws_pkcs11.h"
#include "aws_ota_agent.h"
#include "aws_crypto.h"
#include "aws_ota_codesigner_certificate.h"

#include "wmerrno.h"
#include "rfget.h"
#include "psm-v2.h"
#include "boot_flags.h"
#include "pwrmgr.h"

typedef enum {
    PENDING_VERIFY_OTA_IMG  = 0x0U,
    VALID_OTA_IMG           = 0x1U,
    INVALID_OTA_IMG         = 0x2U,
    OTA_IMG_ABORTED         = 0x3U,
} ota_img_states_t;

typedef struct {
    update_desc_t ud;
    const OTA_FileContext_t *cur_ota;
    uint8_t image_state;
    bool is_image_valid;
    uint32_t written_data;
} ota_info_t;

extern psm_hnd_t psm_hnd;
extern os_timer_t ota_timer;
/* Specify the OTA signature algorithm we support on this platform. */
const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";   /* FIX ME. */

/************************/
static ota_info_t ota_info;
int set_image_state(uint8_t state);
/* The static functions below (prvPAL_CheckFileSignature and prvPAL_ReadAndAssumeCertificate)
 * are optionally implemented. If these functions are implemented then please set the following macros in
 * aws_test_ota_config.h to 1:
 * otatestpalCHECK_FILE_SIGNATURE_SUPPORTED
 * otatestpalREAD_AND_ASSUME_CERTIFICATE_SUPPORTED
 */
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
 *
 */
static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * const C );

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
                                                  uint32_t * const ulSignerCertSize );
static void clear_ota_info(ota_info_t *ota_info)
{
    if (ota_info != NULL) {
        memset(ota_info, 0, sizeof(ota_info_t));
    }
}

static bool check_ota_info(OTA_FileContext_t *C)
{
    return (C != NULL && ota_info.cur_ota == C);
}

static void delete_ota_info(OTA_FileContext_t *C)
{
    if (C != NULL) {
        C->pucFile = 0;
    }
    ota_info.cur_ota = 0;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_CreateFileForRx( OTA_FileContext_t * const C )
{
    struct partition_entry *p = rfget_get_passive_firmware();
    int ret;
    OTA_Err_t ota_ret;

    if (NULL == C || NULL == C->pucFilePath) {
        return kOTA_Err_RxFileCreateFailed;
    }

    /* Verify firmware length first before proceeding further */
    if (C->ulFileSize > p->size) {
        rf_e("Firmware binary too large %d > %d",
                C->ulFileSize, p->size);
        return kOTA_Err_RxFileTooLarge;
    }

    rfget_init();
    ret = rfget_update_begin(&ota_info.ud, p);
    if (ret) {
        rf_e("Failed to erase firmware area");
        ota_ret = kOTA_Err_RxFileCreateFailed;
    } else {
        ota_ret = kOTA_Err_None;
    }
    ota_info.cur_ota = C;

    C->pucFile = (uint8_t *)&ota_info;
    ota_info.is_image_valid = false;
    ota_info.written_data = 0;
    rf_d("OTA Started\r\n");
    return ota_ret;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_Abort( OTA_FileContext_t * const C )
{
    int ret;

    if (C->pucFile ==  NULL)
        return kOTA_Err_None;

    if (check_ota_info(C)) {
	delete_ota_info(C);
        ret = rfget_update_abort(&ota_info.ud);
        if (ret)
            return kOTA_Err_FileAbort;
        else
            return kOTA_Err_None;
    }

    return kOTA_Err_FileAbort;
}
/*-----------------------------------------------------------*/

/* Write a block of data to the specified file. */
int16_t prvPAL_WriteBlock( OTA_FileContext_t * const C,
                           uint32_t ulOffset,
                           uint8_t * const pacData,
                           uint32_t ulBlockSize )
{
    int ret;
    struct partition_entry *p = rfget_get_passive_firmware();
    if (check_ota_info(C)) {
        ota_info.ud.addr = p->start + ulOffset;
        ret = rfget_update_data(&ota_info.ud, (char *)pacData, ulBlockSize);
        if (ret != WM_SUCCESS) {
            rf_e("Failed to write firmware data");
            rfget_update_abort(&ota_info.ud);
            return -1;
        }
	ota_info.written_data += ulBlockSize;
    } else {
        rf_d("couldnt verify ota info\r\n");
        return -1;
    }

    return ulBlockSize;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_CloseFile( OTA_FileContext_t * const C )
{
    int ret;
    OTA_Err_t ota_ret = kOTA_Err_None;

    if (C->pxSignature == NULL) {
        rf_e("Image Signature not available\r\n");
        clear_ota_info(&ota_info);
        ota_ret = kOTA_Err_SignatureCheckFailed;
    } else if (ota_info.written_data == 0) {
        rf_d("No data written to partition\r\n");
        ota_ret = kOTA_Err_SignatureCheckFailed;
    } else {
        ret = flash_drv_close(ota_info.ud.dev);
        if (ret != WM_SUCCESS) {
            ota_ret = kOTA_Err_FileClose;
        } else {
            ota_ret = prvPAL_CheckFileSignature(C);
            if (ota_ret == kOTA_Err_None) {
                set_image_state(PENDING_VERIFY_OTA_IMG);
                ota_info.is_image_valid = true;
            }
        }
    }
    return ota_ret;
}
/*-----------------------------------------------------------*/

static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * const C )
{
    OTA_Err_t result;
    uint32_t ulSignerCertSize;
    void *pvSigVerifyContext;
    u8 *pucSignerCert = 0;
    mdev_t *flash_dev = NULL;
    struct partition_entry *p = rfget_get_passive_firmware();
    int ret;
    void *buf = NULL;
    uint32_t addr, data_remaining;

    /* Verify an ECDSA-SHA256 signature. */
    if (CRYPTO_SignatureVerificationStart(&pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_ECDSA,
                                            cryptoHASH_ALGORITHM_SHA256) == pdFALSE) {
        rf_e("signature verification start failed");
        return kOTA_Err_SignatureCheckFailed;
    }

    pucSignerCert = prvPAL_ReadAndAssumeCertificate((const u8* const)C->pucCertFilepath, &ulSignerCertSize);
    if (pucSignerCert == NULL) {
        rf_e("cert read failed");
        return kOTA_Err_BadSignerCert;
    }

    flash_dev = flash_drv_open(p->device);
    if (flash_dev == NULL) {
        rf_e("Flash driver init is required before open");
        result = kOTA_Err_SignatureCheckFailed;
        goto end;
    }

    data_remaining = ota_info.written_data;
    addr = p->start;
    buf = pvPortMalloc(512);
    if (buf == NULL) {
        rf_e("Failed to allocate buffer for signature verification");
        flash_drv_close(flash_dev);
        result = kOTA_Err_SignatureCheckFailed;
        goto end;
    }

    while (data_remaining > 0) {
        int data_to_read = data_remaining > 512? 512 : data_remaining;

        if (flash_drv_read(flash_dev, (void *)buf, data_to_read, addr) != 0) {
            rf_e("Failed to read update data");
            flash_drv_close(flash_dev);
            result = kOTA_Err_SignatureCheckFailed;
            goto end;
        }

        CRYPTO_SignatureVerificationUpdate(pvSigVerifyContext, buf, data_to_read);

        data_remaining -= data_to_read;
        addr += data_to_read;
    }

    ret = flash_drv_close(flash_dev);
    if (ret != WM_SUCCESS) {
        result = kOTA_Err_SignatureCheckFailed;
        goto end;
    }

    if (CRYPTO_SignatureVerificationFinal(pvSigVerifyContext, (char *)pucSignerCert, ulSignerCertSize,
                                            C->pxSignature->ucData, C->pxSignature->usSize) == pdFALSE) {
        rf_e("signature verification failed");
        result = kOTA_Err_SignatureCheckFailed;
    } else  {
        result = kOTA_Err_None;
    }

end:
    if (buf != NULL) {
        vPortFree(buf);
    }
    /* Free the signer certificate that we now own after prvReadAndAssumeCertificate(). */
    if (pucSignerCert != NULL) {
        vPortFree(pucSignerCert);
    }
    return result;
}
/*-----------------------------------------------------------*/

static CK_RV prvGetCertificateHandle(CK_FUNCTION_LIST_PTR pxFunctionList,
                                     CK_SESSION_HANDLE xSession,
                                     const char * pcLabelName,
                                     CK_OBJECT_HANDLE_PTR pxCertHandle)
{
    CK_ATTRIBUTE xTemplate;
    CK_RV xResult = CKR_OK;
    CK_ULONG ulCount = 0;
    CK_BBOOL xFindInit = CK_FALSE;

    /* Get the certificate handle. */
    if (0 == xResult) {
        xTemplate.type = CKA_LABEL;
        xTemplate.ulValueLen = strlen(pcLabelName) + 1;
        xTemplate.pValue = (char *) pcLabelName;
        xResult = pxFunctionList->C_FindObjectsInit(xSession, &xTemplate, 1);
    }

    if (0 == xResult) {
        xFindInit = CK_TRUE;
        xResult = pxFunctionList->C_FindObjects(xSession,
                        (CK_OBJECT_HANDLE_PTR) pxCertHandle, 1, &ulCount);
    }

    if (CK_TRUE == xFindInit) {
        xResult = pxFunctionList->C_FindObjectsFinal(xSession);
    }

    return xResult;
}

/* Note that this function mallocs a buffer for the certificate to reside in,
 * and it is the responsibility of the caller to free the buffer. */
static CK_RV prvGetCertificate(const char *pcLabelName,
                               uint8_t **ppucData,
                               uint32_t *pulDataSize)
{
    /* Find the certificate */
    CK_OBJECT_HANDLE xHandle;
    CK_RV xResult;
    CK_FUNCTION_LIST_PTR xFunctionList;
    CK_SLOT_ID xSlotId;
    CK_ULONG xCount = 1;
    CK_SESSION_HANDLE xSession;
    CK_ATTRIBUTE xTemplate = { 0 };
    uint8_t * pucCert = NULL;
    CK_BBOOL xSessionOpen = CK_FALSE;

    xResult = C_GetFunctionList(&xFunctionList);

    if (CKR_OK == xResult)
    {
        xResult = xFunctionList->C_Initialize(NULL);
    }

    if ((CKR_OK == xResult) || (CKR_CRYPTOKI_ALREADY_INITIALIZED == xResult)) {
        xResult = xFunctionList->C_GetSlotList(CK_TRUE, &xSlotId, &xCount);
    }

    if (CKR_OK == xResult) {
        xResult = xFunctionList->C_OpenSession(xSlotId, CKF_SERIAL_SESSION, NULL, NULL, &xSession);
    }

    if (CKR_OK == xResult) {
        xSessionOpen = CK_TRUE;
        xResult = prvGetCertificateHandle(xFunctionList, xSession, pcLabelName, &xHandle);
    }

    if ((xHandle != 0) && (xResult == CKR_OK)) /* 0 is an invalid handle */
    {
        /* Get the length of the certificate */
        xTemplate.type = CKA_VALUE;
        xTemplate.pValue = NULL;
        xResult = xFunctionList->C_GetAttributeValue(xSession, xHandle, &xTemplate, xCount);

        if (xResult == CKR_OK) {
            pucCert = pvPortMalloc(xTemplate.ulValueLen);
        }

        if ((xResult == CKR_OK) && (pucCert == NULL)) {
            xResult = CKR_HOST_MEMORY;
        }

        if (xResult == CKR_OK) {
            xTemplate.pValue = pucCert;
            xResult = xFunctionList->C_GetAttributeValue(xSession, xHandle, &xTemplate, xCount);

            if (xResult == CKR_OK) {
                *ppucData = pucCert;
                *pulDataSize = xTemplate.ulValueLen;
            } else {
                vPortFree(pucCert);
            }
        }
    } else { /* Certificate was not found. */
        *ppucData = NULL;
        *pulDataSize = 0;
    }

    if (xSessionOpen == CK_TRUE) {
        (void) xFunctionList->C_CloseSession(xSession);
    }

    return xResult;
}

static uint8_t * prvPAL_ReadAndAssumeCertificate(const uint8_t *const pucCertName,
                                                 uint32_t *const ulSignerCertSize)
{
    uint8_t *pucCertData;
    uint32_t ulCertSize;
    uint8_t *pucSignerCert = NULL;
    CK_RV xResult;

    xResult = prvGetCertificate((const char *) pucCertName, &pucSignerCert, ulSignerCertSize);

    if ((xResult == CKR_OK) && (pucSignerCert != NULL)) {
        rf_d("Using cert with label: %s OK\r\n", (const char *) pucCertName);
    } else {
        rf_d("No such certificate file: %s. Using aws_ota_codesigner_certificate.h.\r\n",
                  (const char *) pucCertName);

        /* Allocate memory for the signer certificate plus a terminating zero so we can copy it and return to the caller. */
        ulCertSize = sizeof(signingcredentialSIGNING_CERTIFICATE_PEM);
        pucSignerCert = pvPortMalloc(ulCertSize);                           /*lint !e9029 !e9079 !e838 malloc proto requires void*. */
        pucCertData = (uint8_t *) signingcredentialSIGNING_CERTIFICATE_PEM; /*lint !e9005 we don't modify the cert but it could be set by PKCS11 so it's not const. */

        if (pucSignerCert != NULL) {
            memcpy(pucSignerCert, pucCertData, ulCertSize);
            *ulSignerCertSize = ulCertSize;
        } else {
            rf_e("Error: No memory for certificate in prvPAL_ReadAndAssumeCertificate!\r\n");
        }
    }

    return pucSignerCert;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_ResetDevice( void )
{

    rf_d("Rebooting...");
    pm_reboot_soc();
    /* Ideally we should not come here */
    return kOTA_Err_ResetNotSupported;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_ActivateNewImage( void )
{
    int ret;

    rf_d("Activating new image \r\n");
    if (ota_info.cur_ota != NULL) {
        ret = part_set_active_partition(ota_info.ud.pe);
        if (ret != WM_SUCCESS) {
            rf_e("active partition set failed \r\n");
            clear_ota_info(&ota_info);
            prvPAL_ResetDevice();
        }
    }
    clear_ota_info(&ota_info);
    prvPAL_ResetDevice();

    return kOTA_Err_None ;
}

int get_image_state(void)
{
    int ret = psm_get_variable(psm_hnd, "ota_info.image_state",
        &ota_info.image_state, sizeof(ota_info.image_state));

    if (ret < 0) {
        rf_e("Error getting state \r\n");
        return ret;
    }

    return WM_SUCCESS;
}

int set_image_state(uint8_t state)
{
    ota_info.image_state = state;
    /* This function sets the value of a variable in psm  */
    int ret = psm_set_variable(psm_hnd, "ota_info.image_state",
            &ota_info.image_state, sizeof(ota_info.image_state));
    if (ret) {
        rf_e("Error writing value in PSM\r\n");
        return -WM_FAIL;
    }

    return WM_SUCCESS;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_SetPlatformImageState( OTA_ImageState_t eState )
{

    int ret, state;
    OTA_Err_t ota_ret = kOTA_Err_None;
    rf_d("Set Platform state \r\n");
    switch (eState) {
        case eOTA_ImageState_Accepted:
            rf_d("Valid Image\r\n");
            state = VALID_OTA_IMG;
            break;
        case eOTA_ImageState_Rejected:
            rf_d("Invalid Image\r\n");
            state = INVALID_OTA_IMG;
            break;
        case eOTA_ImageState_Aborted:
            rf_d("Image Aborted\r\n");
            state = OTA_IMG_ABORTED;
            break;
        case eOTA_ImageState_Testing:
            rf_d("Image Testing\r\n");
            return kOTA_Err_None;
        default:
            rf_d("Invalid Image State\r\n");
            return kOTA_Err_BadImageState;
    }

    /* Get current active (running) firmware image flags */
    ret = get_image_state();

    /* If this is first request to set platform state, post bootup */
    if (ota_info.cur_ota == NULL && ota_info.written_data == 0) {
        if (ota_info.image_state == PENDING_VERIFY_OTA_IMG) {
            if (state == INVALID_OTA_IMG) {
                ret = part_set_active_partition(rfget_get_passive_firmware());
                if (ret != WM_SUCCESS) {
                    rf_e("active partition set failed \r\n");
                    clear_ota_info(&ota_info);
                    prvPAL_ResetDevice();
                }
            }
            os_timer_deactivate(&ota_timer);
            ret = set_image_state(state);
            if (ret != WM_SUCCESS) {
                rf_e("failed to set ota flags\r\n");
                return kOTA_Err_CommitFailed;
            }
        } else {
            rf_d("Image not in self test mode\r\n");
            ota_ret = ota_info.image_state == VALID_OTA_IMG ? kOTA_Err_None : kOTA_Err_CommitFailed;
        }
    } else {
        if (eState == eOTA_ImageState_Accepted && ota_info.is_image_valid == false) {
            /* Incorrect update image or not yet validated */
            return kOTA_Err_CommitFailed;
        }
        if (state != VALID_OTA_IMG) {
            rf_d("currently executing firmware not marked as valid, abort");
            return kOTA_Err_None;
        }
        ret = set_image_state(state);
        if (ret != WM_SUCCESS) {
            rf_e("failed to set ota flags\r\n");
            return kOTA_Err_CommitFailed;
        }
    }

    return ota_ret;
}
/*-----------------------------------------------------------*/

OTA_PAL_ImageState_t prvPAL_GetPlatformImageState( void )
{

    OTA_PAL_ImageState_t eImageState = eOTA_PAL_ImageState_Unknown;

    if (ota_info.cur_ota != NULL && ota_info.written_data != 0) {
        /* Firmware update is complete or on-going, retrieve its status */
	    ota_info.image_state = ota_info.is_image_valid == true ? VALID_OTA_IMG : INVALID_OTA_IMG;
    } else {
        int ret = get_image_state();
        if (ret != WM_SUCCESS) {
            rf_e("failed to get ota flags\r\n");
            return eImageState;
        }
    }

    switch (ota_info.image_state) {
        case PENDING_VERIFY_OTA_IMG:
            /* Pending Commit means we're in the Self Test phase. */
            eImageState = eOTA_PAL_ImageState_PendingCommit;
            break;
        case VALID_OTA_IMG:
            eImageState = eOTA_PAL_ImageState_Valid;
            break;
        default:
            eImageState = eOTA_PAL_ImageState_Invalid;
            break;
    }

    return eImageState;
}

/*-----------------------------------------------------------*/

/* Provide access to private members for testing. */
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
    #include "aws_ota_pal_test_access_define.h"
#endif
