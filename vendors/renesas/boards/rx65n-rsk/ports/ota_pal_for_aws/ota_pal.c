/*
 * FreeRTOS OTA PAL for Renesas RX65N-RSK V2.0.0
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/* C Runtime includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS include. */
#include "FreeRTOS.h"

/* OTA library includes. */
#include "ota.h"
#include "ota_private.h"

/* OTA PAL Port include. */
#include "ota_pal.h"

/* OTA PAL test config file include. */
#include "ota_config.h"

/* Amazon FreeRTOS include. */
#include "iot_crypto.h"
#include "core_pkcs11.h"

/* Renesas RX Driver Package include */
#include "platform.h"
#include "r_flash_rx_if.h"

/* Specify the OTA signature algorithm we support on this platform. */
const char OTA_JsonFileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";

/* The static functions below (otaPAL_CheckFileSignature and otaPAL_ReadAndAssumeCertificate)
 * are optionally implemented. If these functions are implemented then please set the following
 * macros in aws_test_ota_config.h to 1:
 *   otatestpalCHECK_FILE_SIGNATURE_SUPPORTED
 *   otatestpalREAD_AND_ASSUME_CERTIFICATE_SUPPORTED
 */

/**
 * @brief Verify the signature of the specified file.
 *
 * This function should be implemented if signature verification is not offloaded to non-volatile
 * memory io functions.
 *
 * This function is expected to be called from otaPal_CloseFile().
 *
 * @param[in] pFileContext OTA file context information.
 *
 * @return The OtaPalStatus_t error code combined with the MCU specific error code. See
 *         ota_platform_interface.h for OTA major error codes and your specific PAL implementation
 *         for the sub error code.
 *
 * Major error codes returned are:
 *   OtaPalSuccess: if the signature verification passes.
 *   OtaPalSignatureCheckFailed: if the signature verification fails.
 *   OtaPalBadSignerCert: if the signature verification certificate cannot be read.
 */
static OtaPalStatus_t otaPal_CheckFileSignature( OtaFileContext_t * const pFileContext );

/**
 * @brief Read the specified signer certificate from the filesystem into a local buffer.
 *
 * The allocated memory returned becomes the property of the caller who is responsible for freeing it.
 *
 * This function is called from otaPAL_CheckFileSignature(). It should be implemented if signature
 * verification is not offloaded to non-volatile memory io function.
 *
 * @param[in] pucCertName The file path of the certificate file.
 * @param[out] ulSignerCertSize The size of the certificate file read.
 *
 * @return A pointer to the signer certificate in the file system. NULL if the certificate cannot be read.
 * This returned pointer is the responsibility of the caller; if the memory is allocated the caller must free it.
 */
static uint8_t * otaPal_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize );

/*------------------------------------------ firmware update configuration (start) --------------------------------------------*/
/* R_FLASH_Write() arguments: specify "low address" and process to "high address" */
#if defined( BSP_MCU_RX65N ) || ( BSP_MCU_RX651 ) && ( FLASH_CFG_CODE_FLASH_BGO == 1 ) && ( BSP_CFG_MCU_PART_MEMORY_SIZE == 0xe ) /* OTA supported device */
/*------------------------------------------ firmware update configuration (start) --------------------------------------------*/
/* R_FLASH_Write() arguments: specify "low address" and process to "high address" */

    #define BOOT_LOADER_LOW_ADDRESS                           FLASH_CF_BLOCK_13
    #define BOOT_LOADER_MIRROR_LOW_ADDRESS                    FLASH_CF_BLOCK_51

/* R_FLASH_Erase() arguments: specify "high address (low block number)" and process to "low address (high block number)" */
    #define BOOT_LOADER_MIRROR_HIGH_ADDRESS                   FLASH_CF_BLOCK_38
    #define BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS    FLASH_CF_BLOCK_52

    #define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL            8
    #define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM           6

    #define FLASH_INTERRUPT_PRIORITY                          configMAX_SYSCALL_INTERRUPT_PRIORITY /* 0(low) - 15(high) */
/*------------------------------------------ firmware update configuration (end) --------------------------------------------*/

    #define BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS     ( uint32_t ) FLASH_CF_LO_BANK_LO_ADDR
    #define BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS       ( uint32_t ) FLASH_CF_HI_BANK_LO_ADDR
    #define BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER            ( uint32_t ) ( FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM )

#else /* Not OTA supported device */
    #define DUMMY                                             0
    #define BOOT_LOADER_LOW_ADDRESS                           DUMMY
    #define BOOT_LOADER_MIRROR_LOW_ADDRESS                    DUMMY
    #define BOOT_LOADER_MIRROR_HIGH_ADDRESS                   DUMMY
    #define BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS    DUMMY
    #define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL            DUMMY
    #define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM           DUMMY
    #define FLASH_INTERRUPT_PRIORITY                          DUMMY /* 0(low) - 15(high) */
    #define BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS     DUMMY
    #define BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS       DUMMY
    #define BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER            DUMMY
#endif /* if defined( BSP_MCU_RX65N ) || ( BSP_MCU_RX651 ) && ( FLASH_CFG_CODE_FLASH_BGO == 1 ) && ( BSP_CFG_MCU_PART_MEMORY_SIZE == 0xe ) */

#define BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH               0x200
#define BOOT_LOADER_USER_FIRMWARE_DESCRIPTOR_LENGTH           0x100

#define LIFECYCLE_STATE_BLANK                                 ( 0xff )
#define LIFECYCLE_STATE_TESTING                               ( 0xfe )
#define LIFECYCLE_STATE_INSTALLING                            ( 0xfc )
#define LIFECYCLE_STATE_VALID                                 ( 0xf8 )
#define LIFECYCLE_STATE_INVALID                               ( 0xf0 )

#define R_OTA_ERR_NONE                                        ( 0 )
#define R_OTA_ERR_INVALID_CONTEXT                             ( -1 )
#define R_OTA_ERR_QUEUE_SEND_FAIL                             ( -2 )

#define OTA_FLASHING_IN_PROGRESS                              ( 0 )
#define OTA_FLASHING_COMPLETE                                 ( 1 )

#define OTA_SIGUNATURE_SEQUENCE_TOP_VALUE                     ( 0x30 )
#define OTA_SIGUNATURE_INTEGER_VALUE                          ( 0x02 )
#define OTA_SIGUNATURE_NOT_INCLUDE_NEGATIVE_NUMBER_VALUE      ( 0x20 )
#define OTA_SIGUNATURE_INCLUDE_NEGATIVE_NUMBER_VALUE          ( 0x21 )
#define OTA_SIGUNATURE_DATA_HALF_LENGTH                       ( 32 )
#define OTA_SIGUNATURE_SEQUENCE_INFO_LENGTH                   ( 2 )
#define OTA_SIGUNATURE_SKIP                                   ( 2 )

#define OTA_FLASH_MIN_PGM_SIZE_MASK                           ( 0xFFFFFFFF - FLASH_CF_MIN_PGM_SIZE + 1 )

typedef struct _load_firmware_control_block
{
    uint32_t status;
    uint32_t processed_byte;
    uint32_t total_image_length;
    OtaImageState_t eSavedAgentState;
    OtaFileContext_t * OtaFileContext;
} LOAD_FIRMWARE_CONTROL_BLOCK;

typedef struct _flash_block
{
    uint32_t offset;
    uint8_t * binary;
    uint32_t length;
} FLASH_BLOCK;

typedef struct _fragmented_flash_block_list
{
    FLASH_BLOCK content;
    struct _fragmented_flash_block_list * next;
} FRAGMENTED_FLASH_BLOCK_LIST;

typedef struct _packet_block_for_queue
{
    uint32_t ulOffset;
    uint32_t length;
    uint8_t * p_packet;
} PACKET_BLOCK_FOR_QUEUE;

typedef struct _firmware_update_control_block
{
    uint8_t magic_code[ 7 ];
    uint8_t image_flag;
    uint8_t signature_type[ 32 ];
    uint32_t signature_size;
    uint8_t signature[ 256 ];
    uint32_t dataflash_flag;
    uint32_t dataflash_start_address;
    uint32_t dataflash_end_address;
    uint8_t reserved1[ 200 ];
    uint32_t sequence_number;
    uint32_t start_address;
    uint32_t end_address;
    uint32_t execution_address;
    uint32_t hardware_id;
    uint8_t reserved2[ 236 ];
} FIRMWARE_UPDATE_CONTROL_BLOCK;

static int32_t ota_context_validate( OtaFileContext_t * pFileContext );
static int32_t ota_context_update_user_firmware_header( OtaFileContext_t * pFileContext );
static void ota_context_close( OtaFileContext_t * pFileContext );
static void ota_flashing_task( void * pvParameters );
static void ota_flashing_callback( void * event );
static void ota_header_flashing_callback( void * event );
static CK_RV prvGetCertificateHandle( CK_FUNCTION_LIST_PTR pxFunctionList,
                                      CK_SESSION_HANDLE xSession,
                                      const char * pcLabelName,
                                      CK_OBJECT_HANDLE_PTR pxCertHandle );
static CK_RV prvGetCertificate( const char * pcLabelName,
                                uint8_t ** ppucData,
                                uint32_t * pulDataSize );

static FRAGMENTED_FLASH_BLOCK_LIST * fragmented_flash_block_list_insert( FRAGMENTED_FLASH_BLOCK_LIST * head,
                                                                         uint32_t offset,
                                                                         uint8_t * binary,
                                                                         uint32_t length );
static FRAGMENTED_FLASH_BLOCK_LIST * fragmented_flash_block_list_delete( FRAGMENTED_FLASH_BLOCK_LIST * head,
                                                                         uint32_t offset );
static FRAGMENTED_FLASH_BLOCK_LIST * fragmented_flash_block_list_print( FRAGMENTED_FLASH_BLOCK_LIST * head );
static FRAGMENTED_FLASH_BLOCK_LIST * fragmented_flash_block_list_assemble( FRAGMENTED_FLASH_BLOCK_LIST * head,
                                                                           FLASH_BLOCK * flash_block );

static QueueHandle_t xQueue;
static TaskHandle_t xTask;
static xSemaphoreHandle xSemaphoreFlashig;
static xSemaphoreHandle xSemaphoreWriteBlock;
static volatile LOAD_FIRMWARE_CONTROL_BLOCK load_firmware_control_block;
static FRAGMENTED_FLASH_BLOCK_LIST * fragmented_flash_block_list;
static PACKET_BLOCK_FOR_QUEUE packet_block_for_queue1;
static PACKET_BLOCK_FOR_QUEUE packet_block_for_queue2;
static FIRMWARE_UPDATE_CONTROL_BLOCK * firmware_update_control_block_bank0 = ( FIRMWARE_UPDATE_CONTROL_BLOCK * ) BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS;
static volatile uint32_t gs_header_flashing_task;

/*-----------------------------------------------------------*/

OtaPalStatus_t otaPal_CreateFileForRx( OtaFileContext_t * const pFileContext )
{
    LogDebug( ( "otaPal_CreateFileForRx is called." ) );
    LogDebug( ( "Compiled in [%s] [%s].", __DATE__, __TIME__ ) );
    OtaPalMainStatus_t eResult = OtaPalUninitialized;
    flash_interrupt_config_t cb_func_info;

    if( pFileContext != NULL )
    {
        if( pFileContext->pFilePath != NULL )
        {
            /* create task/queue/semaphore for flashing */
            xQueue = xQueueCreate( otaconfigMAX_NUM_BLOCKS_REQUEST, sizeof( PACKET_BLOCK_FOR_QUEUE ) );
            xTaskCreate( ota_flashing_task, "OTA_FLASHING_TASK", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES, &xTask );
            xSemaphoreFlashig = xSemaphoreCreateBinary();
            xSemaphoreGive( xSemaphoreFlashig );
            xSemaphoreWriteBlock = xSemaphoreCreateMutex();
            xSemaphoreGive( xSemaphoreWriteBlock );
            fragmented_flash_block_list = NULL;

            R_FLASH_Close();

            if( R_FLASH_Open() == FLASH_SUCCESS )
            {
                cb_func_info.pcallback = ota_header_flashing_callback;
                cb_func_info.int_priority = FLASH_INTERRUPT_PRIORITY;
                R_FLASH_Control( FLASH_CMD_SET_BGO_CALLBACK, ( void * ) &cb_func_info );
                gs_header_flashing_task = OTA_FLASHING_IN_PROGRESS;

                if( R_FLASH_Erase( ( flash_block_address_t ) BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER ) != FLASH_SUCCESS )
                {
                    eResult = OtaPalRxFileCreateFailed;
                    LogError( ( "Failed to erase the flash: R_FLASH_Erase() returns error." ) );
                }

                while( OTA_FLASHING_IN_PROGRESS == gs_header_flashing_task )
                {
                }

                R_FLASH_Close();
                R_FLASH_Open();
                cb_func_info.pcallback = ota_flashing_callback;
                cb_func_info.int_priority = FLASH_INTERRUPT_PRIORITY;
                R_FLASH_Control( FLASH_CMD_SET_BGO_CALLBACK, ( void * ) &cb_func_info );
                load_firmware_control_block.OtaFileContext = pFileContext;
                load_firmware_control_block.total_image_length = 0;
                load_firmware_control_block.eSavedAgentState = OtaImageStateUnknown;
                LogInfo( ( "Receive file created." ) );
                pFileContext->pFile = ( uint8_t * ) &load_firmware_control_block;
                eResult = OtaPalSuccess;
            }
            else
            {
                eResult = OtaPalRxFileCreateFailed;
                LogError( ( "Failed to create a file: R_FLASH_Open() returns error." ) );
            }
        }
        else
        {
            eResult = OtaPalRxFileCreateFailed;
            LogError( ( "Failed to create a file: Invalid file context provided." ) );
        }
    }
    else
    {
        eResult = OtaPalRxFileCreateFailed;
        LogError( ( "Failed to create a file: Invalid file context provided." ) );
    }

    return OTA_PAL_COMBINE_ERR( eResult, 0 );
}
/*-----------------------------------------------------------*/

OtaPalStatus_t otaPal_Abort( OtaFileContext_t * const pFileContext )
{
    LogDebug( ( "otaPal_Abort is called." ) );

    OtaPalMainStatus_t eResult = OtaPalSuccess;

    if( ota_context_validate( pFileContext ) == R_OTA_ERR_INVALID_CONTEXT )
    {
        eResult = OtaPalFileClose;
    }

    if( OtaPalSuccess == eResult )
    {
        /* delete task/queue for flashing */
        if( NULL != xTask )
        {
            vTaskDelete( xTask );
            xTask = NULL;
        }

        if( NULL != xQueue )
        {
            vQueueDelete( xQueue );
            xQueue = NULL;
        }

        if( NULL != xSemaphoreFlashig )
        {
            vSemaphoreDelete( xSemaphoreFlashig );
            xSemaphoreFlashig = NULL;
        }

        if( NULL != xSemaphoreWriteBlock )
        {
            vSemaphoreDelete( xSemaphoreWriteBlock );
            xSemaphoreWriteBlock = NULL;
        }

        R_FLASH_Close();
    }

    ota_context_close( pFileContext );
    return OTA_PAL_COMBINE_ERR( eResult, 0 );
}
/*-----------------------------------------------------------*/

/* Write a block of data to the specified file. */
int16_t otaPal_WriteBlock( OtaFileContext_t * const pFileContext,
                           uint32_t ulOffset,
                           uint8_t * const pData,
                           uint32_t ulBlockSize )
{
    int16_t sNumBytesWritten = R_OTA_ERR_QUEUE_SEND_FAIL;
    FLASH_BLOCK flash_block;
    static uint8_t flash_block_array[ FLASH_CF_MIN_PGM_SIZE ];
    uint8_t * packet_buffer;

    LogDebug( ( "otaPal_WriteBlock is called." ) );

    xSemaphoreTake( xSemaphoreWriteBlock, portMAX_DELAY );

    if( ( !( ulOffset % FLASH_CF_MIN_PGM_SIZE ) ) && ( !( ulBlockSize % FLASH_CF_MIN_PGM_SIZE ) ) )
    {
        packet_buffer = pvPortMalloc( ulBlockSize );
        memcpy( packet_buffer, pData, ulBlockSize );
        packet_block_for_queue1.p_packet = packet_buffer;
        packet_block_for_queue1.ulOffset = ulOffset;
        packet_block_for_queue1.length = ulBlockSize;

        if( xQueueSend( xQueue, &packet_block_for_queue1, NULL ) == pdPASS )
        {
            sNumBytesWritten = ( int16_t ) ulBlockSize;
        }
        else
        {
            vPortFree( packet_block_for_queue1.p_packet );
            LogError( ( "Failed to write a block: OTA flashing queue send error." ) );
            sNumBytesWritten = R_OTA_ERR_QUEUE_SEND_FAIL;
        }
    }
    else
    {
        flash_block.binary = flash_block_array;

        fragmented_flash_block_list = fragmented_flash_block_list_insert( fragmented_flash_block_list, ulOffset, pData, ulBlockSize );

        if( fragmented_flash_block_list != NULL )
        {
            while( 1 )
            {
                fragmented_flash_block_list = fragmented_flash_block_list_assemble( fragmented_flash_block_list, &flash_block );

                if( flash_block.binary != NULL )
                {
                    packet_buffer = pvPortMalloc( flash_block.length );
                    memcpy( packet_buffer, flash_block.binary, flash_block.length );
                    packet_block_for_queue1.p_packet = packet_buffer;
                    packet_block_for_queue1.ulOffset = flash_block.offset;
                    packet_block_for_queue1.length = flash_block.length;

                    if( xQueueSend( xQueue, &packet_block_for_queue1, NULL ) != pdPASS )
                    {
                        vPortFree( packet_block_for_queue1.p_packet );
                        LogError( ( "Failed to write a block: OTA flashing queue send error." ) );
                        sNumBytesWritten = R_OTA_ERR_QUEUE_SEND_FAIL;
                        break;
                    }
                }
                else
                {
                    sNumBytesWritten = ( int16_t ) ulBlockSize;
                    break;
                }
            }

            /*----------- finalize phase ----------*/
            fragmented_flash_block_list_print( fragmented_flash_block_list );
        }
        else
        {
            sNumBytesWritten = ( int16_t ) ulBlockSize;
        }
    }

    xSemaphoreGive( xSemaphoreWriteBlock );

    return sNumBytesWritten;
}
/*-----------------------------------------------------------*/

OtaPalStatus_t otaPal_CloseFile( OtaFileContext_t * const pFileContext )
{
    OtaPalMainStatus_t eResult = OtaPalSuccess;

    if( ota_context_validate( pFileContext ) == R_OTA_ERR_INVALID_CONTEXT )
    {
        eResult = OtaPalFileClose;
    }

    if( pFileContext->pSignature != NULL )
    {
        eResult = OTA_PAL_MAIN_ERR( otaPal_CheckFileSignature( pFileContext ) );
    }
    else
    {
        eResult = OtaPalSignatureCheckFailed;
    }

    if( OtaPalSuccess == eResult )
    {
        /* Update the user firmware header. */
        if( ota_context_update_user_firmware_header( pFileContext ) == R_OTA_ERR_NONE )
        {
            LogInfo( ( "User firmware image header updated." ) );
        }
        else
        {
            LogError( ( "Failed to update the firmware image header." ) );
            eResult = OtaPalFileClose;
        }

        /* delete task/queue for flashing */
        if( NULL != xTask )
        {
            vTaskDelete( xTask );
            xTask = NULL;
        }

        if( NULL != xQueue )
        {
            do
            {
                if( errQUEUE_FULL == xQueueReceive( xQueue, &packet_block_for_queue2, 0 ) )
                {
                    break;
                }
                else
                {
                    vPortFree( packet_block_for_queue2.p_packet );
                }
            }
            while( 1 );

            vQueueDelete( xQueue );
            xQueue = NULL;
        }

        if( NULL != xSemaphoreFlashig )
        {
            vSemaphoreDelete( xSemaphoreFlashig );
            xSemaphoreFlashig = NULL;
        }

        if( NULL != xSemaphoreWriteBlock )
        {
            vSemaphoreDelete( xSemaphoreWriteBlock );
            xSemaphoreWriteBlock = NULL;
        }

        R_FLASH_Close();
    }
    else
    {
        load_firmware_control_block.eSavedAgentState = OtaImageStateRejected;
    }

    ota_context_close( pFileContext );
    return OTA_PAL_COMBINE_ERR( eResult, 0 );
}
/*-----------------------------------------------------------*/

static OtaPalStatus_t otaPal_CheckFileSignature( OtaFileContext_t * const pFileContext )
{
    OtaPalMainStatus_t eResult = OtaPalUninitialized;
    uint32_t ulSignerCertSize;
    void * pvSigVerifyContext;
    uint8_t * pucSignerCert = NULL;
    uint32_t flash_aligned_address = 0;
    uint8_t assembled_flash_buffer[ FLASH_CF_MIN_PGM_SIZE ];
    flash_err_t flash_err;
    flash_interrupt_config_t cb_func_info;

    if( fragmented_flash_block_list != NULL )
    {
        FRAGMENTED_FLASH_BLOCK_LIST * tmp = fragmented_flash_block_list;

        do
        {
            /* Read one page from flash memory. */
            flash_aligned_address = ( ( tmp->content.offset & OTA_FLASH_MIN_PGM_SIZE_MASK ) + BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH );
            memcpy( ( uint8_t * ) assembled_flash_buffer, ( uint8_t * ) flash_aligned_address, FLASH_CF_MIN_PGM_SIZE );
            /* Replace length bytes from offset. */
            memcpy( &assembled_flash_buffer[ tmp->content.offset ], tmp->content.binary, tmp->content.length );
            /* Flashing memory. */
            xSemaphoreTake( xSemaphoreFlashig, portMAX_DELAY );
            R_FLASH_Close();
            R_FLASH_Open();
            cb_func_info.pcallback = ota_header_flashing_callback;
            cb_func_info.int_priority = FLASH_INTERRUPT_PRIORITY;
            R_FLASH_Control( FLASH_CMD_SET_BGO_CALLBACK, ( void * ) &cb_func_info );
            gs_header_flashing_task = OTA_FLASHING_IN_PROGRESS;
            flash_err = R_FLASH_Write( ( uint32_t ) assembled_flash_buffer, ( uint32_t ) flash_aligned_address, FLASH_CF_MIN_PGM_SIZE );

            if( flash_err != FLASH_SUCCESS )
            {
                nop();
            }

            while( OTA_FLASHING_IN_PROGRESS == gs_header_flashing_task )
            {
            }

            xSemaphoreGive( xSemaphoreFlashig );
            load_firmware_control_block.total_image_length += tmp->content.length;
            tmp = fragmented_flash_block_list_delete( tmp, tmp->content.offset );
        }
        while( tmp != NULL );
    }

    /* Verify an ECDSA-SHA256 signature. */
    if( CRYPTO_SignatureVerificationStart( &pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_ECDSA,
                                           cryptoHASH_ALGORITHM_SHA256 ) == pdFALSE )
    {
        return OTA_PAL_COMBINE_ERR( OtaPalSignatureCheckFailed, 0 );
    }
    else
    {
        LogInfo( ( "Started %s signature verification, file: %s",
                   OTA_JsonFileSignatureKey, ( const char * ) pFileContext->pCertFilepath ) );
        pucSignerCert = otaPal_ReadAndAssumeCertificate( pFileContext->pCertFilepath, &ulSignerCertSize );

        if( pucSignerCert == NULL )
        {
            eResult = OtaPalBadSignerCert;
        }
        else
        {
            CRYPTO_SignatureVerificationUpdate( pvSigVerifyContext,
                                                ( const uint8_t * ) BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
                                                load_firmware_control_block.total_image_length );

            if( CRYPTO_SignatureVerificationFinal( pvSigVerifyContext, ( char * ) pucSignerCert, ulSignerCertSize,
                                                   pFileContext->pSignature->data, pFileContext->pSignature->size ) == pdFALSE )
            {
                LogError( ( "Finished %s signature verification, but signature verification failed",
                            OTA_JsonFileSignatureKey ) );
                eResult = OtaPalSignatureCheckFailed;
            }
            else
            {
                LogInfo( ( "Finished %s signature verification, signature verification passed",
                           OTA_JsonFileSignatureKey ) );
                eResult = OtaPalSuccess;
            }
        }
    }

    /* Free the signer certificate that we now own after otaPal_ReadAndAssumeCertificate(). */
    if( pucSignerCert != NULL )
    {
        vPortFree( pucSignerCert );
    }

    return OTA_PAL_COMBINE_ERR( eResult, 0 );
}
/*-----------------------------------------------------------*/

static uint8_t * otaPal_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize )
{
    uint8_t * pucCertData;
    uint32_t ulCertSize;
    uint8_t * pucSignerCert = NULL;
    CK_RV xResult;

    xResult = prvGetCertificate( ( const char * ) pucCertName, &pucSignerCert, ulSignerCertSize );

    if( ( xResult == CKR_OK ) && ( pucSignerCert != NULL ) )
    {
        LogInfo( ( "otaPal_ReadAndAssumeCertificate using cert with label: %s", ( const char * ) pucCertName ) );
    }
    else
    {
        LogWarn( ( "No such certificate file: %s. Using certificate defined by the otapalconfigCODE_SIGNING_CERTIFICATE macro instead",
                   ( const char * ) pucCertName ) );

        /* Allocate memory for the signer certificate plus a terminating zero so we can copy it and return to the caller. */
        ulCertSize = sizeof( otapalconfigCODE_SIGNING_CERTIFICATE );
        pucSignerCert = pvPortMalloc( ulCertSize + 1 );                   /*lint !e9029 !e9079 !e838 malloc proto requires void*. */
        pucCertData = ( uint8_t * ) otapalconfigCODE_SIGNING_CERTIFICATE; /*lint !e9005 we don't modify the cert but it could be set by PKCS11 so it's not const. */

        if( pucSignerCert != NULL )
        {
            memcpy( pucSignerCert, pucCertData, ulCertSize );
            /* The crypto code requires the terminating zero to be part of the length so add 1 to the size. */
            pucSignerCert[ ulCertSize ] = 0U;
            *ulSignerCertSize = ulCertSize + 1U;
        }
        else
        {
            LogError( ( "Failed to store the certificate in dynamic memory: pvPortMalloc failed to allocate a buffer of size %d", ulCertSize ) );
        }
    }

    return pucSignerCert;
}
/*-----------------------------------------------------------*/

OtaPalStatus_t otaPal_ResetDevice( OtaFileContext_t * const pFileContext )
{
    ( void ) pFileContext;

    LogInfo( ( "Resetting the device." ) );

    if( ( OtaImageStateAccepted == load_firmware_control_block.eSavedAgentState ) ||
        ( OtaImageStateTesting == load_firmware_control_block.eSavedAgentState ) )
    {
        /* Software reset issued (Not swap bank) */
        set_psw( 0 );
        R_BSP_InterruptsDisable();
        R_BSP_RegisterProtectDisable( BSP_REG_PROTECT_LPC_CGC_SWR );
        SYSTEM.SWRR = 0xa501;

        while( 1 ) /* software reset */
        {
        }
    }
    else
    {
        /* If the status is rejected, aborted, or error, swap bank and return to the previous image.
         * Then the boot loader will start and erase the image that failed to update. */
        set_psw( 0 );
        R_BSP_InterruptsDisable();
        R_FLASH_Control( FLASH_CMD_BANK_TOGGLE, NULL );
        R_BSP_RegisterProtectDisable( BSP_REG_PROTECT_LPC_CGC_SWR );
        SYSTEM.SWRR = 0xa501;

        while( 1 ) /* software reset */
        {
        }
    }

    /* We shouldn't actually get here if the board supports the auto reset.
     * But, it doesn't hurt anything if we do although someone will need to
     * reset the device for the new image to boot. */
    return OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );
}
/*-----------------------------------------------------------*/

OtaPalStatus_t otaPal_ActivateNewImage( OtaFileContext_t * const pFileContext )
{
    LogInfo( ( "Changing the Startup Bank" ) );

    /* reset for self testing */
    vTaskDelay( 5000 );
    otaPal_ResetDevice( pFileContext ); /* no return from this function */

    return OTA_PAL_COMBINE_ERR( OtaPalSuccess, 0 );
}
/*-----------------------------------------------------------*/

OtaPalStatus_t otaPal_SetPlatformImageState( OtaFileContext_t * const pFileContext,
                                             OtaImageState_t eState )
{
    flash_interrupt_config_t cb_func_info;

    LogDebug( ( "otaPal_SetPlatformImageState is called." ) );

    OtaPalMainStatus_t eResult = OtaPalUninitialized;

    /* Save the image state to eSavedAgentState. */
    if( OtaImageStateTesting == load_firmware_control_block.eSavedAgentState )
    {
        switch( eState )
        {
            case OtaImageStateAccepted:
                R_FLASH_Close();
                R_FLASH_Open();
                cb_func_info.pcallback = ota_header_flashing_callback;
                cb_func_info.int_priority = FLASH_INTERRUPT_PRIORITY;
                R_FLASH_Control( FLASH_CMD_SET_BGO_CALLBACK, ( void * ) &cb_func_info );
                gs_header_flashing_task = OTA_FLASHING_IN_PROGRESS;

                if( R_FLASH_Erase( ( flash_block_address_t ) BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER ) == FLASH_SUCCESS )
                {
                    while( OTA_FLASHING_IN_PROGRESS == gs_header_flashing_task )
                    {
                    }

                    LogInfo( ( "Erase install area (code flash):OK" ) );
                    LogInfo( ( "Accepted and committed final image." ) );
                    eResult = OtaPalSuccess;
                }
                else
                {
                    LogError( ( "Erase install area (code flash):NG" ) );
                    LogError( ( "Accepted final image but commit failed." ) );
                    eResult = OtaPalCommitFailed;
                }

                break;

            case OtaImageStateRejected:
                LogInfo( ( "Rejected image." ) );
                eResult = OtaPalSuccess;
                break;

            case OtaImageStateAborted:
                LogInfo( ( "Aborted image." ) );
                eResult = OtaPalSuccess;
                break;

            case OtaImageStateTesting:
                LogInfo( ( "Testing." ) );
                eResult = OtaPalSuccess;
                break;

            default:
                LogError( ( "Unknown state received %d.", ( int32_t ) eState ) );
                eResult = OtaPalBadImageState;
                break;
        }
    }
    else
    {
        switch( eState )
        {
            case OtaImageStateAccepted:
                LogError( ( "Not in commit pending so can not mark image valid (%d)." ) );
                eResult = OtaPalCommitFailed;
                break;

            case OtaImageStateRejected:
                LogInfo( ( "Rejected image." ) );
                eResult = OtaPalSuccess;
                break;

            case OtaImageStateAborted:
                LogInfo( ( "Aborted image." ) );
                eResult = OtaPalSuccess;
                break;

            case OtaImageStateTesting:
                LogInfo( ( "Testing." ) );
                eResult = OtaPalSuccess;
                break;

            default:
                LogError( ( "Unknown state received %d.", ( int32_t ) eState ) );
                eResult = OtaPalBadImageState;
                break;
        }
    }

    load_firmware_control_block.eSavedAgentState = eState;

    return OTA_PAL_COMBINE_ERR( eResult, 0 );
}
/*-----------------------------------------------------------*/

OtaPalImageState_t otaPal_GetPlatformImageState( OtaFileContext_t * const pFileContext )
{
    ( void ) pFileContext;

    LogDebug( ( "otaPal_GetPlatformImageState is called." ) );

    OtaPalImageState_t ePalState = OtaPalImageStateUnknown;

    switch( load_firmware_control_block.eSavedAgentState )
    {
        case OtaImageStateTesting:
            ePalState = OtaPalImageStatePendingCommit;
            break;

        case OtaImageStateAccepted:
            ePalState = OtaPalImageStateValid;
            break;

        case OtaImageStateUnknown: /* Uninitialized image state, assume a factory image */
            ePalState = OtaPalImageStateValid;
            break;

        case OtaImageStateRejected:
        case OtaImageStateAborted:
        default:
            ePalState = OtaPalImageStateInvalid;
            break;
    }

    LogDebug( ( "Function called is otaPal_GetPlatformImageState: Platform State is [%d]", ePalState ) );
    return ePalState; /*lint !e64 !e480 !e481 I/O calls and return type are used per design. */
}
/*-----------------------------------------------------------*/

/* Provide access to private members for testing. */
#ifdef FREERTOS_ENABLE_UNIT_TESTS
    #include "aws_ota_pal_test_access_define.h"
#endif

static CK_RV prvGetCertificateHandle( CK_FUNCTION_LIST_PTR pxFunctionList,
                                      CK_SESSION_HANDLE xSession,
                                      const char * pcLabelName,
                                      CK_OBJECT_HANDLE_PTR pxCertHandle )
{
    CK_ATTRIBUTE xTemplate;
    CK_RV xResult = CKR_OK;
    CK_ULONG ulCount = 0;
    CK_BBOOL xFindInit = CK_FALSE;

    /* Get the certificate handle. */
    if( 0 == xResult )
    {
        xTemplate.type = CKA_LABEL;
        xTemplate.ulValueLen = strlen( pcLabelName ) + 1;
        xTemplate.pValue = ( char * ) pcLabelName;
        xResult = pxFunctionList->C_FindObjectsInit( xSession, &xTemplate, 1 );
    }

    if( 0 == xResult )
    {
        xFindInit = CK_TRUE;
        xResult = pxFunctionList->C_FindObjects( xSession,
                                                 ( CK_OBJECT_HANDLE_PTR ) pxCertHandle,
                                                 1,
                                                 &ulCount );
    }

    if( CK_TRUE == xFindInit )
    {
        xResult = pxFunctionList->C_FindObjectsFinal( xSession );
    }

    return xResult;
}

/* Note that this function mallocs a buffer for the certificate to reside in,
 * and it is the responsibility of the caller to free the buffer. */
static CK_RV prvGetCertificate( const char * pcLabelName,
                                uint8_t ** ppucData,
                                uint32_t * pulDataSize )
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

    xResult = C_GetFunctionList( &xFunctionList );

    if( CKR_OK == xResult )
    {
        xResult = xFunctionList->C_Initialize( NULL );
    }

    if( ( CKR_OK == xResult ) || ( CKR_CRYPTOKI_ALREADY_INITIALIZED == xResult ) )
    {
        xResult = xFunctionList->C_GetSlotList( CK_TRUE, &xSlotId, &xCount );
    }

    if( CKR_OK == xResult )
    {
        xResult = xFunctionList->C_OpenSession( xSlotId, CKF_SERIAL_SESSION, NULL, NULL, &xSession );
    }

    if( CKR_OK == xResult )
    {
        xSessionOpen = CK_TRUE;
        xResult = prvGetCertificateHandle( xFunctionList, xSession, pcLabelName, &xHandle );
    }

    if( ( xHandle != 0 ) && ( xResult == CKR_OK ) ) /* 0 is an invalid handle */
    {
        /* Get the length of the certificate */
        xTemplate.type = CKA_VALUE;
        xTemplate.pValue = NULL;
        xResult = xFunctionList->C_GetAttributeValue( xSession, xHandle, &xTemplate, xCount );

        if( xResult == CKR_OK )
        {
            pucCert = pvPortMalloc( xTemplate.ulValueLen );
        }

        if( ( xResult == CKR_OK ) && ( pucCert == NULL ) )
        {
            xResult = CKR_HOST_MEMORY;
        }

        if( xResult == CKR_OK )
        {
            xTemplate.pValue = pucCert;
            xResult = xFunctionList->C_GetAttributeValue( xSession, xHandle, &xTemplate, xCount );

            if( xResult == CKR_OK )
            {
                *ppucData = pucCert;
                *pulDataSize = xTemplate.ulValueLen;
            }
            else
            {
                vPortFree( pucCert );
            }
        }
    }
    else /* Certificate was not found. */
    {
        *ppucData = NULL;
        *pulDataSize = 0;
    }

    if( xSessionOpen == CK_TRUE )
    {
        ( void ) xFunctionList->C_CloseSession( xSession );
    }

    return xResult;
}

static int32_t ota_context_validate( OtaFileContext_t * pFileContext )
{
    return( NULL != pFileContext );
}

static int32_t ota_context_update_user_firmware_header( OtaFileContext_t * pFileContext )
{
    int32_t ret = R_OTA_ERR_INVALID_CONTEXT;
    uint8_t block[ BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH ];
    FIRMWARE_UPDATE_CONTROL_BLOCK * p_block_header;
    uint32_t length;
    flash_err_t flash_err;
    flash_interrupt_config_t cb_func_info;
    uint8_t * source_pointer, * destination_pointer;
    uint8_t data_length = 0;

    memcpy( block, ( const void * ) firmware_update_control_block_bank0, BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH );
    p_block_header = ( FIRMWARE_UPDATE_CONTROL_BLOCK * ) block;

    /* Update image flag. */
    p_block_header->image_flag = LIFECYCLE_STATE_TESTING;

    /* Update signature type. */
    memcpy( p_block_header->signature_type, OTA_JsonFileSignatureKey, sizeof( OTA_JsonFileSignatureKey ) );

    /* Parse the signature and extract ECDSA-SHA256 signature data. */
    source_pointer = pFileContext->pSignature->data;
    destination_pointer = p_block_header->signature;
    data_length = *( source_pointer + 1 ) + OTA_SIGUNATURE_SEQUENCE_INFO_LENGTH;
    memset( destination_pointer, 0, sizeof( destination_pointer ) );

    if( OTA_SIGUNATURE_SEQUENCE_TOP_VALUE == *source_pointer )
    {
        source_pointer += OTA_SIGUNATURE_SEQUENCE_INFO_LENGTH;

        while( 1 )
        {
            if( OTA_SIGUNATURE_INTEGER_VALUE == *source_pointer )
            {
                source_pointer++;

                if( OTA_SIGUNATURE_INCLUDE_NEGATIVE_NUMBER_VALUE == *source_pointer )
                {
                    source_pointer += OTA_SIGUNATURE_SKIP;
                }
                else
                {
                    source_pointer++;
                }

                memcpy( destination_pointer, source_pointer, OTA_SIGUNATURE_DATA_HALF_LENGTH );
                source_pointer += OTA_SIGUNATURE_DATA_HALF_LENGTH;
                destination_pointer += OTA_SIGUNATURE_DATA_HALF_LENGTH;

                if( ( source_pointer - pFileContext->pSignature->data ) == data_length )
                {
                    ret = R_OTA_ERR_NONE;
                    break;
                }
            }
            else
            {
                /* parsing error */
                break;
            }
        }
    }

    if( R_OTA_ERR_NONE == ret )
    {
        R_FLASH_Close();
        R_FLASH_Open();
        cb_func_info.pcallback = ota_header_flashing_callback;
        cb_func_info.int_priority = FLASH_INTERRUPT_PRIORITY;
        R_FLASH_Control( FLASH_CMD_SET_BGO_CALLBACK, ( void * ) &cb_func_info );
        gs_header_flashing_task = OTA_FLASHING_IN_PROGRESS;
        length = BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH;
        flash_err = R_FLASH_Write( ( uint32_t ) block, ( uint32_t ) BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, length );

        if( flash_err != FLASH_SUCCESS )
        {
            nop();
        }

        while( OTA_FLASHING_IN_PROGRESS == gs_header_flashing_task )
        {
        }
    }

    return ret;
}

static void ota_context_close( OtaFileContext_t * pFileContext )
{
    if( NULL != pFileContext )
    {
        pFileContext->pFile = NULL;
    }
}

static FRAGMENTED_FLASH_BLOCK_LIST * fragmented_flash_block_list_insert( FRAGMENTED_FLASH_BLOCK_LIST * head,
                                                                         uint32_t offset,
                                                                         uint8_t * binary,
                                                                         uint32_t length )
{
    FRAGMENTED_FLASH_BLOCK_LIST * tmp, * current, * previous, * new;

    new = pvPortMalloc( sizeof( FLASH_BLOCK ) );
    new->content.binary = pvPortMalloc( length );

    if( ( new != NULL ) && ( new->content.binary != NULL ) )
    {
        memcpy( new->content.binary, binary, length );
        new->content.offset = offset;
        new->content.length = length;
        new->next = NULL;

        /* new head would be returned when head would be specified as NULL. */
        if( head == NULL )
        {
            tmp = new;
        }
        else
        {
            /* search the list to insert new node */
            current = head;

            while( 1 )
            {
                if( ( new->content.offset < current->content.offset ) || ( current == NULL ) )
                {
                    break;
                }

                previous = current;
                current = current->next;
            }

            /* insert the searched node when current != head */
            if( current != head )
            {
                previous->next = new;
                new->next = current;
                tmp = head;
            }
            else
            {
                new->next = current;
                tmp = new;
            }
        }
    }
    else
    {
        tmp = NULL;
    }

    return tmp;
}

static FRAGMENTED_FLASH_BLOCK_LIST * fragmented_flash_block_list_delete( FRAGMENTED_FLASH_BLOCK_LIST * head,
                                                                         uint32_t offset )
{
    FRAGMENTED_FLASH_BLOCK_LIST * tmp = head, * previous = NULL;

    if( head != NULL )
    {
        while( 1 )
        {
            if( tmp->content.offset == offset )
            {
                break;
            }

            if( tmp->next == NULL )
            {
                tmp = NULL;
                break;
            }

            previous = tmp;
            tmp = tmp->next;
        }

        if( tmp != NULL )
        {
            /* delete target exists on not head, remove specified and return head in this case */
            if( previous != NULL )
            {
                previous->next = tmp->next;
                vPortFree( tmp->content.binary );
                vPortFree( tmp );
                tmp = head;
            }
            else
            {
                /* delete target exists on head with subsequent data, remove head and return specified (as new head) in this case */
                if( head->next != NULL )
                {
                    tmp = head->next;
                }
                /* delete target exists on head without subsequent data, remove head and return NULL in this case */
                else
                {
                    tmp = NULL;
                }

                vPortFree( head->content.binary );
                vPortFree( head );
            }
        }
    }

    return tmp;
}

static FRAGMENTED_FLASH_BLOCK_LIST * fragmented_flash_block_list_print( FRAGMENTED_FLASH_BLOCK_LIST * head )
{
    FRAGMENTED_FLASH_BLOCK_LIST * tmp = head;
    uint32_t total_heap_length, total_list_count;

    total_heap_length = 0;
    total_list_count = 0;

    if( head != NULL )
    {
        while( 1 )
        {
            total_heap_length += sizeof( FRAGMENTED_FLASH_BLOCK_LIST );
            total_heap_length += tmp->content.length;
            total_list_count++;

            if( tmp->next == NULL )
            {
                break;
            }

            tmp = tmp->next;
        }
    }

    LogDebug( ( "FRAGMENTED_FLASH_BLOCK_LIST: total_heap_length = [%d], total_list_count = [%d].", total_heap_length, total_list_count ) );

    return tmp;
}

static FRAGMENTED_FLASH_BLOCK_LIST * fragmented_flash_block_list_assemble( FRAGMENTED_FLASH_BLOCK_LIST * head,
                                                                           FLASH_BLOCK * flash_block )
{
    FRAGMENTED_FLASH_BLOCK_LIST * tmp = head;
    FRAGMENTED_FLASH_BLOCK_LIST * flash_block_candidate[ FLASH_CF_MIN_PGM_SIZE ];
    uint32_t assembled_length = 0;
    uint32_t fragmented_length = 0;
    uint32_t loop_counter = 0;
    uint32_t index = 0;

    /* search aligned FLASH_CF_MIN_PGM_SIZE top offset */
    while( 1 )
    {
        if( !( tmp->content.offset % FLASH_CF_MIN_PGM_SIZE ) )
        {
            /* extract continuous flash_block candidate */
            assembled_length = 0;
            loop_counter = 0;

            while( 1 )
            {
                if( ( tmp != NULL ) && ( assembled_length < FLASH_CF_MIN_PGM_SIZE ) )
                {
                    if( loop_counter < FLASH_CF_MIN_PGM_SIZE )
                    {
                        if( ( tmp->content.offset + tmp->content.length ) > ( ( tmp->content.offset & OTA_FLASH_MIN_PGM_SIZE_MASK ) + FLASH_CF_MIN_PGM_SIZE ) )
                        {
                            fragmented_length = ( FLASH_CF_MIN_PGM_SIZE - assembled_length );
                            assembled_length += fragmented_length;
                            flash_block_candidate[ loop_counter ] = tmp;
                            tmp->next = fragmented_flash_block_list_insert( tmp->next, tmp->content.offset + fragmented_length, &tmp->content.binary[ fragmented_length ], tmp->content.length - fragmented_length );
                            tmp->content.length = fragmented_length;
                        }
                        else
                        {
                            assembled_length += tmp->content.length;
                            flash_block_candidate[ loop_counter ] = tmp;
                        }
                    }
                    else
                    {
                        break;
                    }

                    tmp = tmp->next;
                    loop_counter++;
                }
                else
                {
                    break;
                }
            }
        }

        /* break if found completed flash_block_candidate or found end of list */
        if( ( assembled_length == FLASH_CF_MIN_PGM_SIZE ) || ( tmp == NULL ) )
        {
            break;
        }
        /* search next candidate */
        else
        {
            tmp = tmp->next;
        }
    }

    /* assemble flash_block */
    if( assembled_length == FLASH_CF_MIN_PGM_SIZE )
    {
        tmp = head;
        /* remove flash_block from list */
        flash_block->offset = flash_block_candidate[ 0 ]->content.offset;
        flash_block->length = assembled_length;

        for( uint32_t i = 0; i < loop_counter; i++ )
        {
            memcpy( &flash_block->binary[ index ], flash_block_candidate[ i ]->content.binary, flash_block_candidate[ i ]->content.length );
            index = flash_block_candidate[ i ]->content.length;
            tmp = fragmented_flash_block_list_delete( tmp, flash_block_candidate[ i ]->content.offset );
        }
    }
    else
    {
        tmp = head;
        flash_block->offset = NULL;
        flash_block->binary = NULL;
        flash_block->length = NULL;
    }

    return tmp;
}

static void ota_flashing_task( void * pvParameters )
{
    flash_err_t flash_err;
    static uint8_t block[ ( 1 << otaconfigLOG2_FILE_BLOCK_SIZE ) ];
    static uint32_t ulOffset;
    static uint32_t length;

    while( 1 )
    {
        xQueueReceive( xQueue, &packet_block_for_queue2, portMAX_DELAY );
        xSemaphoreTake( xSemaphoreFlashig, portMAX_DELAY );
        memcpy( block, packet_block_for_queue2.p_packet, packet_block_for_queue2.length );
        ulOffset = packet_block_for_queue2.ulOffset;
        length = packet_block_for_queue2.length;
        flash_err = R_FLASH_Write( ( uint32_t ) block, ulOffset + BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH, length );

        if( packet_block_for_queue2.length != 1024 )
        {
            nop();
        }

        if( flash_err != FLASH_SUCCESS )
        {
            nop();
        }

        load_firmware_control_block.total_image_length += length;
        vPortFree( packet_block_for_queue2.p_packet );
    }
}

static void ota_flashing_callback( void * event )
{
    uint32_t event_code;

    event_code = *( ( uint32_t * ) event );

    if( ( event_code != FLASH_INT_EVENT_WRITE_COMPLETE ) || ( event_code == FLASH_INT_EVENT_ERASE_COMPLETE ) )
    {
        nop(); /* trap */
    }

    static portBASE_TYPE xHigherPriorityTaskWoken;
    xSemaphoreGiveFromISR( xSemaphoreFlashig, &xHigherPriorityTaskWoken );
}

static void ota_header_flashing_callback( void * event )
{
    uint32_t event_code;

    event_code = *( ( uint32_t * ) event );

    gs_header_flashing_task = OTA_FLASHING_COMPLETE;

    if( ( event_code != FLASH_INT_EVENT_WRITE_COMPLETE ) || ( event_code == FLASH_INT_EVENT_ERASE_COMPLETE ) )
    {
        nop(); /* trap */
    }
}
