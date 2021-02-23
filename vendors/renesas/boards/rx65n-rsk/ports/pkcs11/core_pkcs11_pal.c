/*
 * FreeRTOS PKCS #11 PAL for Renesas Starter Kit+ for RX65N-2MB V1.0.0
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

/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/**
 * @file core_pkcs11_pal.c
 * @brief Device specific helpers for PKCS11 Interface.
 */

/* Amazon FreeRTOS Includes. */
#include "core_pkcs11.h"
#include "core_pkcs11_pal.h"
#include "iot_crypto.h"
#include "core_pkcs11_config.h"
#include "FreeRTOS.h"
#include "mbedtls/sha256.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/* Renesas RX platform includes */
#include "platform.h"
#include "r_flash_rx_if.h"

typedef struct _pkcs_data
{
    CK_ATTRIBUTE Label;
    uint32_t local_storage_index;
    uint32_t ulDataSize;
    uint32_t status;
    CK_OBJECT_HANDLE xHandle;
} PKCS_DATA;

#define PKCS_DATA_STATUS_EMPTY 0
#define PKCS_DATA_STATUS_REGISTERED 1
#define PKCS_DATA_STATUS_HIT 2

#define PKCS_HANDLES_LABEL_MAX_LENGTH 40
#define PKCS_OBJECT_HANDLES_NUM 5
#define PKCS_SHA256_LENGTH 32

#define MAX_CHECK_DATAFLASH_AREA_RETRY_COUNT 3

#if defined (BSP_MCU_RX65N) || (BSP_MCU_RX651)
#define PKCS_CONTROL_BLOCK_INITIAL_DATA \
    {\
        /* uint8_t local_storage[((FLASH_DF_BLOCK_SIZE * FLASH_NUM_BLOCKS_DF) / 4) - (sizeof(PKCS_DATA) * PKCS_OBJECT_HANDLES_NUM) - PKCS_SHA256_LENGTH]; */\
        {0x00},\
        /* PKCS_DATA pkcs_data[PKCS_OBJECT_HANDLES_NUM]; */\
        {0x00},\
    },\
    /* uint8_t hash_sha256[PKCS_SHA256_LENGTH]; */\
    {0xea, 0x57, 0x12, 0x9a, 0x18, 0x10, 0x83, 0x80, 0x88, 0x80, 0x40, 0x1f, 0xae, 0xb2, 0xd2, 0xff, 0x1c, 0x14, 0x5e, 0x81, 0x22, 0x6b, 0x9d, 0x93, 0x21, 0xf8, 0x0c, 0xc1, 0xda, 0x29, 0x61, 0x64},
#elif defined (BSP_MCU_RX64M)
#define PKCS_CONTROL_BLOCK_INITIAL_DATA \
    {\
        /* uint8_t local_storage[((FLASH_DF_BLOCK_SIZE * FLASH_NUM_BLOCKS_DF) / 4) - (sizeof(PKCS_DATA) * PKCS_OBJECT_HANDLES_NUM) - PKCS_SHA256_LENGTH]; */\
        {0x00},\
        /* PKCS_DATA pkcs_data[PKCS_OBJECT_HANDLES_NUM]; */\
        {0x00},\
    },\
    /* uint8_t hash_sha256[PKCS_SHA256_LENGTH]; */\
    {0x62, 0x90, 0xe6, 0x59, 0x20, 0x47, 0xec, 0x80, 0x14, 0x0a, 0x12, 0x52, 0xd3, 0x1a, 0x8b, 0xa8, 0xa3, 0x3a, 0x34, 0xcf, 0x57, 0x8b, 0x5c, 0x8c, 0x4a, 0x08, 0x17, 0x06, 0xb9, 0x41, 0x6f, 0xa6},
#elif defined (BSP_MCU_RX63N) || (BSP_MCU_RX631) || (BSP_MCU_RX630)
#define PKCS_CONTROL_BLOCK_INITIAL_DATA \
    {\
        /* uint8_t local_storage[((FLASH_DF_BLOCK_SIZE * FLASH_NUM_BLOCKS_DF) / 4) - (sizeof(PKCS_DATA) * PKCS_OBJECT_HANDLES_NUM) - PKCS_SHA256_LENGTH]; */\
        {0x00},\
        /* PKCS_DATA pkcs_data[PKCS_OBJECT_HANDLES_NUM]; */\
        {0x00},\
    },\
    /* uint8_t hash_sha256[PKCS_SHA256_LENGTH]; */\
    {0xea, 0x57, 0x12, 0x9a, 0x18, 0x10, 0x83, 0x80, 0x88, 0x80, 0x40, 0x1f, 0xae, 0xb2, 0xd2, 0xff, 0x1c, 0x14, 0x5e, 0x81, 0x22, 0x6b, 0x9d, 0x93, 0x21, 0xf8, 0x0c, 0xc1, 0xda, 0x29, 0x61, 0x64},
#else
#error "core_pkcs11_pal.c does not support your MCU"
#endif

#define PKCS11_PAL_DEBUG_PRINT( X )      /* configPRINTF( X ) */

#define DATA_FLASH_UPDATE_STATE_INITIALIZE 0
#define DATA_FLASH_UPDATE_STATE_ERASE 1
#define DATA_FLASH_UPDATE_STATE_ERASE_WAIT_COMPLETE 2
#define DATA_FLASH_UPDATE_STATE_WRITE 3
#define DATA_FLASH_UPDATE_STATE_WRITE_WAIT_COMPLETE 4
#define DATA_FLASH_UPDATE_STATE_FINALIZE 5
#define DATA_FLASH_UPDATE_STATE_FINALIZE_COMPLETED 6
#define DATA_FLASH_UPDATE_STATE_ERROR 103

typedef struct _pkcs_storage_control_block_sub
{
    uint8_t local_storage[((FLASH_DF_BLOCK_SIZE * FLASH_NUM_BLOCKS_DF) / 4) - (sizeof(PKCS_DATA) * PKCS_OBJECT_HANDLES_NUM) - PKCS_SHA256_LENGTH]; /* RX65N case: 8KB */
    PKCS_DATA pkcs_data[PKCS_OBJECT_HANDLES_NUM];
} PKCS_STORAGE_CONTROL_BLOCK_SUB;

typedef struct _PKCS_CONTROL_BLOCK
{
    PKCS_STORAGE_CONTROL_BLOCK_SUB data;
    uint8_t hash_sha256[PKCS_SHA256_LENGTH];
} PKCS_CONTROL_BLOCK;

enum eObjectHandles
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey,
    eAwsDeviceCertificate,
    eAwsCodeSigningKey,
    //eAwsRootCertificate
};

uint8_t object_handle_dictionary[PKCS_OBJECT_HANDLES_NUM][PKCS_HANDLES_LABEL_MAX_LENGTH] =
{
    "",
    pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
    pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
    pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
    pkcs11configLABEL_CODE_VERIFICATION_KEY,
    //pkcs11configLABEL_ROOT_CERTIFICATE,
};

static PKCS_CONTROL_BLOCK pkcs_control_block_data_image;        /* RX65N case: 8KB, RX64M case: 16KB, RX63N case: 8KB */

R_BSP_ATTRIB_SECTION_CHANGE(C, _PKCS11_STORAGE, 1)
static const PKCS_CONTROL_BLOCK pkcs_control_block_data = {PKCS_CONTROL_BLOCK_INITIAL_DATA};
R_BSP_ATTRIB_SECTION_CHANGE_END

R_BSP_ATTRIB_SECTION_CHANGE(C, _PKCS11_STORAGE_MIRROR, 1)
static const PKCS_CONTROL_BLOCK pkcs_control_block_data_mirror = {PKCS_CONTROL_BLOCK_INITIAL_DATA};
R_BSP_ATTRIB_SECTION_CHANGE_END

static void update_dataflash_data_from_image(void);
static void update_dataflash_data_mirror_from_image(void);
static void check_dataflash_area(uint32_t retry_counter);

void data_flash_update_status_initialize(void);
static void update_data_flash_callback_function(void *event);

typedef struct _update_data_flash_control_block {
	uint32_t status;
}UPDATA_DATA_FLASH_CONTROL_BLOCK;

/******************************************************************************
 Private global variables
 ******************************************************************************/
static UPDATA_DATA_FLASH_CONTROL_BLOCK update_data_flash_control_block;

CK_RV PKCS11_PAL_Initialize( CK_VOID_PTR pvInitArgs )
{
    CK_RV xResult = CKR_OK;

    CRYPTO_Init();
    R_FLASH_Open();

#if defined (BSP_MCU_RX63N) || (BSP_MCU_RX631) || (BSP_MCU_RX630)
    flash_access_window_config_t flash_access_window_config;
    flash_access_window_config.read_en_mask = 0xffff;
    flash_access_window_config.write_en_mask = 0xffff;
    R_FLASH_Control(FLASH_CMD_ACCESSWINDOW_SET, &flash_access_window_config);
#endif

    /* check the hash */
    check_dataflash_area(0);

    /* copy data from storage to ram */
    memcpy(&pkcs_control_block_data_image, (void *)&pkcs_control_block_data, sizeof(pkcs_control_block_data_image));

    R_FLASH_Close();

    return xResult;
}

/**
* @brief Writes a file to local storage.
*
* Port-specific file write for crytographic information.
*
* @param[in] pxLabel       Label of the object to be saved.
* @param[in] pucData       Data buffer to be written to file
* @param[in] ulDataSize    Size (in bytes) of data to be saved.
*
* @return The file handle of the object that was stored.
*/
CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( CK_ATTRIBUTE_PTR pxLabel,
                                        uint8_t * pucData,
                                        uint32_t ulDataSize )
{
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    int i;
    uint8_t hash_sha256[PKCS_SHA256_LENGTH];
    mbedtls_sha256_context ctx;

    mbedtls_sha256_init(&ctx);
    R_FLASH_Open();

#if defined (BSP_MCU_RX63N) || (BSP_MCU_RX631) || (BSP_MCU_RX630)
    flash_access_window_config_t flash_access_window_config;
    flash_access_window_config.read_en_mask = 0xffff;
    flash_access_window_config.write_en_mask = 0xffff;
    R_FLASH_Control(FLASH_CMD_ACCESSWINDOW_SET, &flash_access_window_config);
#endif

    /* check the hash */
    check_dataflash_area(0);

    /* copy data from storage to ram */
    memcpy(&pkcs_control_block_data_image, (void *)&pkcs_control_block_data, sizeof(pkcs_control_block_data_image));

    /* search specified label value from object_handle_dictionary */
    for (i = 1; i < PKCS_OBJECT_HANDLES_NUM; i++)
    {
        if (!strcmp((char * )&object_handle_dictionary[i], pxLabel->pValue))
        {
            xHandle = i;
        }
    }

    if (xHandle != eInvalidHandle)
    {

        /* pre-calculate -> total stored data size : pre-calculate phase */
        uint32_t total_stored_data_size = 0;

        for (int i = 1; i < PKCS_OBJECT_HANDLES_NUM; i++)
        {
            if (pkcs_control_block_data_image.data.pkcs_data[i].status == PKCS_DATA_STATUS_REGISTERED)
            {
                total_stored_data_size += pkcs_control_block_data_image.data.pkcs_data[i].ulDataSize;
            }
        }

        /* remove current xHandle from pkcs_data */
        if (pkcs_control_block_data_image.data.pkcs_data[xHandle].status == PKCS_DATA_STATUS_REGISTERED)
        {

            uint32_t move_target_xHandle = 0, move_target_index = 0;

            uint32_t delete_target_index = pkcs_control_block_data_image.data.pkcs_data[xHandle].local_storage_index;
            uint32_t delete_target_data_size = pkcs_control_block_data_image.data.pkcs_data[xHandle].ulDataSize;

            /* Search move target index and handle  */
            for (int i = 1; i < PKCS_OBJECT_HANDLES_NUM; i++)
            {

                if ((pkcs_control_block_data_image.data.pkcs_data[i].status == PKCS_DATA_STATUS_REGISTERED)
                        && (pkcs_control_block_data_image.data.pkcs_data[i].local_storage_index == (delete_target_index + delete_target_data_size)))
                {
                    move_target_xHandle = i;
                    move_target_index = pkcs_control_block_data_image.data.pkcs_data[i].local_storage_index;
                    break;
                }

            }

            if (move_target_xHandle != 0)
            {

                /* Move target index to delete target index */
                memmove(
                    (void * )&pkcs_control_block_data_image.data.local_storage[delete_target_index],
                    (void * )&pkcs_control_block_data_image.data.local_storage[move_target_index],
                    (size_t )total_stored_data_size - move_target_index);

                /* Fix index of all moved data  */
                for (int i = 1; i < PKCS_OBJECT_HANDLES_NUM; i++)
                {

                    if (pkcs_control_block_data_image.data.pkcs_data[i].local_storage_index > delete_target_index)
                    {
                        pkcs_control_block_data_image.data.pkcs_data[i].local_storage_index -= delete_target_data_size;
                    }

                }

            }

            /* Recalculate the end of data storage  */
            total_stored_data_size -= delete_target_data_size;

            pkcs_control_block_data_image.data.pkcs_data[xHandle].local_storage_index = 0;
            pkcs_control_block_data_image.data.pkcs_data[xHandle].ulDataSize = 0;

        }

        pkcs_control_block_data_image.data.pkcs_data[xHandle].Label.type = pxLabel->type;
        pkcs_control_block_data_image.data.pkcs_data[xHandle].Label.ulValueLen = pxLabel->ulValueLen;
        pkcs_control_block_data_image.data.pkcs_data[xHandle].local_storage_index = total_stored_data_size;
        pkcs_control_block_data_image.data.pkcs_data[xHandle].ulDataSize = ulDataSize;
        pkcs_control_block_data_image.data.pkcs_data[xHandle].status = PKCS_DATA_STATUS_REGISTERED;
        pkcs_control_block_data_image.data.pkcs_data[xHandle].xHandle = xHandle;
        memcpy(&pkcs_control_block_data_image.data.local_storage[total_stored_data_size], pucData, ulDataSize);

        /* update the hash */
        mbedtls_sha256_starts_ret(&ctx, 0); /* 0 means SHA256 context */
        mbedtls_sha256_update_ret(&ctx, (unsigned char *)&pkcs_control_block_data_image.data, sizeof(pkcs_control_block_data.data));
        mbedtls_sha256_finish_ret(&ctx, hash_sha256);
        memcpy(pkcs_control_block_data_image.hash_sha256, hash_sha256, sizeof(hash_sha256));

        /* update data from ram to storage */
        data_flash_update_status_initialize();
        while ( update_data_flash_control_block.status < DATA_FLASH_UPDATE_STATE_FINALIZE_COMPLETED )
        {
            update_dataflash_data_from_image();
            vTaskDelay(1);
        }
        if (update_data_flash_control_block.status == DATA_FLASH_UPDATE_STATE_ERROR)
        {
            PKCS11_PAL_DEBUG_PRINT(("ERROR: Update data flash data from image\r\n"));
            while(1);
        }
        data_flash_update_status_initialize();
        while ( update_data_flash_control_block.status < DATA_FLASH_UPDATE_STATE_FINALIZE_COMPLETED )
        {
            update_dataflash_data_mirror_from_image();
            vTaskDelay(1);
        }
        if (update_data_flash_control_block.status == DATA_FLASH_UPDATE_STATE_ERROR)
        {
            PKCS11_PAL_DEBUG_PRINT(("ERROR: Update data flash data mirror from image\r\n"));
            while(1);
        }
    }

    R_FLASH_Close();

    return xHandle;

}

/**
* @brief Translates a PKCS #11 label into an object handle.
*
* Port-specific object handle retrieval.
*
*
* @param[in] pLabel         Pointer to the label of the object
*                           who's handle should be found.
* @param[in] usLength       The length of the label, in bytes.
*
* @return The object handle if operation was successful.
* Returns eInvalidHandle if unsuccessful.
*/
CK_OBJECT_HANDLE PKCS11_PAL_FindObject( uint8_t * pLabel,
                                        uint8_t usLength )
{
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    CK_BYTE_PTR pxZeroedData = NULL;
    CK_BYTE_PTR pxObject = NULL;
    CK_BBOOL xIsPrivate = ( CK_BBOOL ) CK_TRUE;
    CK_ULONG ulObjectLength = sizeof( CK_BYTE );
    CK_RV xResult = CKR_OK;
    int i;

    for(i = 1; i < PKCS_OBJECT_HANDLES_NUM; i++)
    {
        if(!strcmp((char *)&object_handle_dictionary[i], (char *)pLabel))
        {
            if(pkcs_control_block_data_image.data.pkcs_data[i].status == PKCS_DATA_STATUS_REGISTERED)
            {
                xHandle = i;
            }
        }
    }

    if( xHandle != eInvalidHandle)
    {
        xResult = PKCS11_PAL_GetObjectValue( xHandle, &pxObject, &ulObjectLength, &xIsPrivate );

        /* Zeroed out object means it has been destroyed. */
        if( ( xResult != CKR_OK ) || ( pxObject[ 0 ] == 0x00 ) )
        {
            xHandle = eInvalidHandle;
        }

        PKCS11_PAL_GetObjectValueCleanup( pxObject, ulObjectLength );
    }

    return xHandle;
}

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
CK_RV PKCS11_PAL_GetObjectValue( CK_OBJECT_HANDLE xHandle,
                                 uint8_t ** ppucData,
                                 uint32_t * pulDataSize,
                                 CK_BBOOL * pIsPrivate )
{
    CK_RV xReturn = CKR_FUNCTION_FAILED;
    CK_OBJECT_HANDLE xHandleStorage = xHandle;


    if (xHandleStorage == eAwsDevicePublicKey)
    {
        xHandleStorage = eAwsDevicePrivateKey;
    }


    if (xHandle != eInvalidHandle)
    {
        *ppucData = &pkcs_control_block_data_image.data.local_storage[pkcs_control_block_data_image.data.pkcs_data[xHandleStorage].local_storage_index];
        *pulDataSize = pkcs_control_block_data_image.data.pkcs_data[xHandleStorage].ulDataSize;

        if (xHandle == eAwsDevicePrivateKey)
        {
            *pIsPrivate = CK_TRUE;
        }
        else
        {
            *pIsPrivate = CK_FALSE;
        }
        xReturn = CKR_OK;
    }

    return xReturn;

}


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
    /* todo: nothing to do in now. Now, pkcs_data exists as static. I will fix this function when I will port this to heap memory. (Renesas/Ishiguro) */
}

static void update_dataflash_data_from_image(void)
{
	flash_interrupt_config_t cb_func_info;
    static uint32_t required_dataflash_block_num;
    flash_err_t flash_error_code = FLASH_SUCCESS;

    switch(update_data_flash_control_block.status)
    {
        case DATA_FLASH_UPDATE_STATE_INITIALIZE: /* initialize */
        	cb_func_info.pcallback = update_data_flash_callback_function;
        	cb_func_info.int_priority = 15;
        	R_FLASH_Control(FLASH_CMD_SET_BGO_CALLBACK, (void *)&cb_func_info);
            required_dataflash_block_num = sizeof(PKCS_CONTROL_BLOCK) / FLASH_DF_BLOCK_SIZE;
            if(sizeof(PKCS_CONTROL_BLOCK) % FLASH_DF_BLOCK_SIZE)
            {
                required_dataflash_block_num++;
            }
            update_data_flash_control_block.status = DATA_FLASH_UPDATE_STATE_ERASE;
            break;
        case DATA_FLASH_UPDATE_STATE_ERASE: /* erase bank0 (0xFFE00000-0xFFEF0000) */
            PKCS11_PAL_DEBUG_PRINT(("erase dataflash(main)...\r\n"));
            R_BSP_InterruptsDisable();
            flash_error_code = R_FLASH_Erase((flash_block_address_t)&pkcs_control_block_data, required_dataflash_block_num);
            R_BSP_InterruptsEnable();
            if(FLASH_SUCCESS == flash_error_code)
            {
                 PKCS11_PAL_DEBUG_PRINT(("OK\r\n"));
            }
            else
            {
                PKCS11_PAL_DEBUG_PRINT(("NG\r\n"));
                PKCS11_PAL_DEBUG_PRINT(("R_FLASH_Erase() returns error code = %d.\r\n", flash_error_code));
            }
            update_data_flash_control_block.status = DATA_FLASH_UPDATE_STATE_ERASE_WAIT_COMPLETE;
            break;
        case DATA_FLASH_UPDATE_STATE_ERASE_WAIT_COMPLETE:
            /* this state will be changed by callback routine */
            break;
        case DATA_FLASH_UPDATE_STATE_WRITE:
            PKCS11_PAL_DEBUG_PRINT(("write dataflash(main)...\r\n"));
            R_BSP_InterruptsDisable();
            flash_error_code = R_FLASH_Write((flash_block_address_t)&pkcs_control_block_data_image, (flash_block_address_t)&pkcs_control_block_data, FLASH_DF_BLOCK_SIZE * required_dataflash_block_num);
            R_BSP_InterruptsEnable();
            if(FLASH_SUCCESS == flash_error_code)
            {
                PKCS11_PAL_DEBUG_PRINT(("OK\r\n"));
            }
            else
            {
            	PKCS11_PAL_DEBUG_PRINT(("NG\r\n"));
            	PKCS11_PAL_DEBUG_PRINT(("R_FLASH_Write() returns error code = %d.\r\n", flash_error_code));
            	return;
            }
            update_data_flash_control_block.status = DATA_FLASH_UPDATE_STATE_WRITE_WAIT_COMPLETE;
            break;
        case DATA_FLASH_UPDATE_STATE_WRITE_WAIT_COMPLETE:
            /* this state will be changed by callback routine */
            break;
        case DATA_FLASH_UPDATE_STATE_FINALIZE: /* finalize */
            update_data_flash_control_block.status = DATA_FLASH_UPDATE_STATE_FINALIZE_COMPLETED;
            break;
        default:
            break;
    }
    return;
}

static void update_dataflash_data_mirror_from_image(void)
{
	flash_interrupt_config_t cb_func_info;
    static uint32_t required_dataflash_block_num;
    flash_err_t flash_error_code = FLASH_SUCCESS;

    switch(update_data_flash_control_block.status)
    {
    	case DATA_FLASH_UPDATE_STATE_INITIALIZE: /* initialize */
    		cb_func_info.pcallback = update_data_flash_callback_function;
    		cb_func_info.int_priority = 15;
    		R_FLASH_Control(FLASH_CMD_SET_BGO_CALLBACK, (void *)&cb_func_info);
            required_dataflash_block_num = sizeof(PKCS_CONTROL_BLOCK) / FLASH_DF_BLOCK_SIZE;
            if(sizeof(PKCS_CONTROL_BLOCK) % FLASH_DF_BLOCK_SIZE)
            {
                required_dataflash_block_num++;
            }

            update_data_flash_control_block.status = DATA_FLASH_UPDATE_STATE_ERASE;
            break;
        case DATA_FLASH_UPDATE_STATE_ERASE: /* erase bank0 (0xFFE00000-0xFFEF0000) */
            PKCS11_PAL_DEBUG_PRINT(("erase dataflash(mirror)...\r\n"));
            R_BSP_InterruptsDisable();
            flash_error_code = R_FLASH_Erase((flash_block_address_t)&pkcs_control_block_data_mirror, required_dataflash_block_num);
            R_BSP_InterruptsEnable();
            if(FLASH_SUCCESS == flash_error_code)
            {
                PKCS11_PAL_DEBUG_PRINT(("OK\r\n"));
            }
            else
            {
                PKCS11_PAL_DEBUG_PRINT(("NG\r\n"));
                PKCS11_PAL_DEBUG_PRINT(("R_FLASH_Erase() returns error code = %d.\r\n", flash_error_code));
                return;
            }

            update_data_flash_control_block.status = DATA_FLASH_UPDATE_STATE_ERASE_WAIT_COMPLETE;
            break;
        case DATA_FLASH_UPDATE_STATE_ERASE_WAIT_COMPLETE:
            /* this state will be changed by callback routine */
            break;
        case DATA_FLASH_UPDATE_STATE_WRITE:
            PKCS11_PAL_DEBUG_PRINT(("write dataflash(mirror)...\r\n"));
            R_BSP_InterruptsDisable();
            flash_error_code = R_FLASH_Write((flash_block_address_t)&pkcs_control_block_data_image, (flash_block_address_t)&pkcs_control_block_data_mirror, FLASH_DF_BLOCK_SIZE * required_dataflash_block_num);
            R_BSP_InterruptsEnable();
            if(FLASH_SUCCESS == flash_error_code)
            {
                PKCS11_PAL_DEBUG_PRINT(("OK\r\n"));
            }
            else
            {
                PKCS11_PAL_DEBUG_PRINT(("NG\r\n"));
                PKCS11_PAL_DEBUG_PRINT(("R_FLASH_Write() returns error code = %d.\r\n", flash_error_code));
                return;
            }
            update_data_flash_control_block.status = DATA_FLASH_UPDATE_STATE_WRITE_WAIT_COMPLETE;
            break;
        case DATA_FLASH_UPDATE_STATE_WRITE_WAIT_COMPLETE:
            /* this state will be changed by callback routine */
            break;
        case DATA_FLASH_UPDATE_STATE_FINALIZE: /* finalize */

            if(!memcmp(&pkcs_control_block_data, &pkcs_control_block_data_mirror, sizeof(PKCS_CONTROL_BLOCK)))
            {
                PKCS11_PAL_DEBUG_PRINT(("data flash setting OK.\r\n"));
            }
            else
            {
                PKCS11_PAL_DEBUG_PRINT(("data flash setting NG.\r\n"));
                return;
            }
            update_data_flash_control_block.status = DATA_FLASH_UPDATE_STATE_FINALIZE_COMPLETED;
            break;
        default:
    	    break;
    }
    return;
}

static void check_dataflash_area(uint32_t retry_counter)
{
    uint8_t hash_sha256[PKCS_SHA256_LENGTH];
    mbedtls_sha256_context ctx;

    mbedtls_sha256_init(&ctx);

    if(retry_counter)
    {
        PKCS11_PAL_DEBUG_PRINT(("recover retry count = %d.\r\n", retry_counter));
        if(retry_counter == MAX_CHECK_DATAFLASH_AREA_RETRY_COUNT)
        {
            PKCS11_PAL_DEBUG_PRINT(("retry over the limit.\r\n"));
            while(1);
        }
    }
    PKCS11_PAL_DEBUG_PRINT(("data flash(main) hash check...\r\n"));
    mbedtls_sha256_starts_ret(&ctx, 0); /* 0 means SHA256 context */
    mbedtls_sha256_update_ret(&ctx, (unsigned char *)&pkcs_control_block_data.data, sizeof(pkcs_control_block_data.data));
    mbedtls_sha256_finish_ret(&ctx, hash_sha256);
    if(!memcmp(pkcs_control_block_data.hash_sha256, hash_sha256, sizeof(hash_sha256)))
    {
        PKCS11_PAL_DEBUG_PRINT(("OK\r\n"));
        PKCS11_PAL_DEBUG_PRINT(("data flash(mirror) hash check...\r\n"));
        mbedtls_sha256_starts_ret(&ctx, 0); /* 0 means SHA256 context */
        mbedtls_sha256_update_ret(&ctx, (unsigned char *)&pkcs_control_block_data_mirror.data, sizeof(pkcs_control_block_data_mirror.data));
        mbedtls_sha256_finish_ret(&ctx, hash_sha256);
        if(!memcmp(pkcs_control_block_data_mirror.hash_sha256, hash_sha256, sizeof(hash_sha256)))
        {
            PKCS11_PAL_DEBUG_PRINT(("OK\r\n"));
        }
        else
        {
            PKCS11_PAL_DEBUG_PRINT(("NG\r\n"));
            PKCS11_PAL_DEBUG_PRINT(("recover mirror from main.\r\n"));
            memcpy(&pkcs_control_block_data_image, (void *)&pkcs_control_block_data, sizeof(pkcs_control_block_data));

            data_flash_update_status_initialize();
            while ( update_data_flash_control_block.status < DATA_FLASH_UPDATE_STATE_FINALIZE_COMPLETED )
            {
                update_dataflash_data_mirror_from_image();
            }
            if (update_data_flash_control_block.status == DATA_FLASH_UPDATE_STATE_ERROR)
            {
                PKCS11_PAL_DEBUG_PRINT(("ERROR: Update data flash data mirror from image\r\n"));
                while(1);
            }
            check_dataflash_area(retry_counter + 1);
        }
    }
    else
    {
        PKCS11_PAL_DEBUG_PRINT(("NG\r\n"));
        PKCS11_PAL_DEBUG_PRINT(("data flash(mirror) hash check...\r\n"));
        mbedtls_sha256_starts_ret(&ctx, 0); /* 0 means SHA256 context */
        mbedtls_sha256_update_ret(&ctx, (unsigned char *)&pkcs_control_block_data_mirror.data, sizeof(pkcs_control_block_data_mirror.data));
        mbedtls_sha256_finish_ret(&ctx, hash_sha256);
        if(!memcmp(pkcs_control_block_data_mirror.hash_sha256, hash_sha256, sizeof(hash_sha256)))
        {
            PKCS11_PAL_DEBUG_PRINT(("OK\r\n"));
            PKCS11_PAL_DEBUG_PRINT(("recover main from mirror.\r\n"));
            memcpy(&pkcs_control_block_data_image, (void *)&pkcs_control_block_data_mirror, sizeof(pkcs_control_block_data_mirror));
            data_flash_update_status_initialize();
            while ( update_data_flash_control_block.status < DATA_FLASH_UPDATE_STATE_FINALIZE_COMPLETED )
            {
                update_dataflash_data_from_image();
            }
            if (update_data_flash_control_block.status == DATA_FLASH_UPDATE_STATE_ERROR)
            {
                PKCS11_PAL_DEBUG_PRINT(("ERROR: Update data flash data from image\r\n"));
                while(1);
            }
            check_dataflash_area(retry_counter + 1);
        }
        else
        {
            PKCS11_PAL_DEBUG_PRINT(("NG\r\n"));
            while(1)
            {
                vTaskDelay(10000);
                PKCS11_PAL_DEBUG_PRINT(("------------------------------------------------\r\n"));
                PKCS11_PAL_DEBUG_PRINT(("Data flash is completely broken.\r\n"));
                PKCS11_PAL_DEBUG_PRINT(("Please erase all code flash.\r\n"));
                PKCS11_PAL_DEBUG_PRINT(("And, write initial firmware using debugger/rom writer.\r\n"));
                PKCS11_PAL_DEBUG_PRINT(("------------------------------------------------\r\n"));
            }
        }
    }
}

void data_flash_update_status_initialize(void)
{
	update_data_flash_control_block.status = DATA_FLASH_UPDATE_STATE_INITIALIZE;
}

void update_data_flash_callback_function(void *event)
{
	uint32_t event_code;
	event_code = *((uint32_t*)event);

	switch(event_code)
	{
		case FLASH_INT_EVENT_ERASE_COMPLETE:
			if(DATA_FLASH_UPDATE_STATE_ERASE_WAIT_COMPLETE == update_data_flash_control_block.status)
			{
				update_data_flash_control_block.status = DATA_FLASH_UPDATE_STATE_WRITE;
			}
			else
			{
				update_data_flash_control_block.status = DATA_FLASH_UPDATE_STATE_ERROR;
			}
			break;
		case FLASH_INT_EVENT_WRITE_COMPLETE:
			if(DATA_FLASH_UPDATE_STATE_WRITE_WAIT_COMPLETE == update_data_flash_control_block.status)
			{

				update_data_flash_control_block.status = DATA_FLASH_UPDATE_STATE_FINALIZE;
			}
			else
			{
				update_data_flash_control_block.status = DATA_FLASH_UPDATE_STATE_ERROR;
			}
			break;
		default:
			break;
	}
}

/* Converts a handle to its respective label. */
void prvHandleToLabel( char ** pcLabel,
                       CK_OBJECT_HANDLE xHandle )
{
    if( pcLabel != NULL )
    {
        switch( xHandle )
        {
            case eAwsDeviceCertificate:
                *pcLabel = ( char * ) pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;
                break;

            case eAwsDevicePrivateKey:
                *pcLabel = ( char * ) pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
                break;

            case eAwsDevicePublicKey:
                *pcLabel = ( char * ) pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS;
                break;

            case eAwsCodeSigningKey:
                *pcLabel = ( char * ) pkcs11configLABEL_CODE_VERIFICATION_KEY;
                break;

            default:
                *pcLabel = NULL;
                break;
        }
    }
}

CK_RV PKCS11_PAL_DestroyObject( CK_OBJECT_HANDLE xHandle )
{
    CK_RV xResult = CKR_OK;
    CK_BYTE_PTR pxZeroedData = NULL;
    CK_BYTE_PTR pxObject = NULL;
    CK_BBOOL xIsPrivate = ( CK_BBOOL ) CK_TRUE;
    CK_OBJECT_HANDLE xPalHandle2 = CK_INVALID_HANDLE;
    CK_ULONG ulObjectLength = sizeof( CK_BYTE );
    char * pcLabel = NULL;
    CK_ATTRIBUTE xLabel;

    prvHandleToLabel( &pcLabel, xHandle );

    if( pcLabel != NULL )
    {
        xLabel.type = CKA_LABEL;
        xLabel.pValue = pcLabel;
        xLabel.ulValueLen = strlen( pcLabel );

        xResult = PKCS11_PAL_GetObjectValue( xHandle, &pxObject, &ulObjectLength, &xIsPrivate );
    }   
    else
    {
        xResult = CKR_OBJECT_HANDLE_INVALID;
    }   

    if( xResult == CKR_OK )
    {
        /* Some ports return a pointer to memory for which using memset directly won't work. */
        pxZeroedData = pvPortMalloc( ulObjectLength );

        if( NULL != pxZeroedData )
        {
            /* Zero out the object. */
            ( void ) memset( pxZeroedData, 0x0, ulObjectLength );

            /* Overwrite the object in NVM with zeros. */
            xPalHandle2 = PKCS11_PAL_SaveObject( &xLabel, pxZeroedData, ( size_t ) ulObjectLength );

            if( xPalHandle2 != xHandle )
            {
                xResult = CKR_GENERAL_ERROR;
            }

            vPortFree( pxZeroedData );
        }
        else
        {
            xResult = CKR_HOST_MEMORY;
        }

        PKCS11_PAL_GetObjectValueCleanup( pxObject, ulObjectLength );
    }
    else
    {
        xResult = CKR_GENERAL_ERROR;
    }

    return xResult;
}
