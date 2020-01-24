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

/* C Runtime includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Amazon FreeRTOS include. */
#include "FreeRTOS.h"
#include "aws_iot_ota_agent.h"
#include "iot_crypto.h"
#include "iot_pkcs11.h"
#include "aws_ota_codesigner_certificate.h"
#include "aws_ota_agent_config.h"

/* Renesas RX Driver Package include */
#include "platform.h"
#include "r_flash_rx_if.h"

/* Specify the OTA signature algorithm we support on this platform. */
const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";


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

/*------------------------------------------ firmware update configuration (start) --------------------------------------------*/
/* R_FLASH_Write() arguments: specify "low address" and process to "high address" */
#define BOOT_LOADER_LOW_ADDRESS FLASH_CF_BLOCK_13
#define BOOT_LOADER_MIRROR_LOW_ADDRESS FLASH_CF_BLOCK_51

/* R_FLASH_Erase() arguments: specify "high address (low block number)" and process to "low address (high block number)" */
#define BOOT_LOADER_MIRROR_HIGH_ADDRESS FLASH_CF_BLOCK_38
#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS FLASH_CF_BLOCK_52

#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL 8
#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM 6

#define FLASH_INTERRUPT_PRIORITY 4	/* 0(low) - 15(high) */
/*------------------------------------------ firmware update configuration (end) --------------------------------------------*/

#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS FLASH_CF_LO_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS FLASH_CF_HI_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER (FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM)

//#define otaconfigMAX_NUM_BLOCKS_REQUEST        	128U	/* this value will be appeared after 201908.00 in aws_ota_agent_config.h */

#define BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH 0x200
#define BOOT_LOADER_USER_FIRMWARE_DESCRIPTOR_LENGTH 0x100

#define LIFECYCLE_STATE_BLANK		(0xff)
#define LIFECYCLE_STATE_TESTING		(0xfe)
#define LIFECYCLE_STATE_VALID		(0xfc)
#define LIFECYCLE_STATE_INVALID		(0xf8)

#define OTA_PAL_SUCCESS     (0)
#define OTA_PAL_FAIL        (-1)

#define OTA_FLASHING_IN_PROGRESS    (0)
#define OTA_FLASHING_COMPLETE       (1)

#define OTA_SIGUNATURE_SEQUENCE_TOP_VALUE 					(0x30)
#define OTA_SIGUNATURE_INTEGER_VALUE						(0x02)
#define OTA_SIGUNATURE_NOT_INCLUDE_NEGATIVE_NUMBER_VALUE	(0x20)
#define OTA_SIGUNATURE_INCLUDE_NEGATIVE_NUMBER_VALUE		(0x21)
#define OTA_SIGUNATURE_DATA_HALF_LENGTH						(32)
#define OTA_SIGUNATURE_SEQUENCE_INFO_LENGTH					(2)
#define OTA_SIGUNATURE_SKIP									(2)

typedef struct _load_firmware_control_block {
	uint32_t status;
	uint32_t processed_byte;
	OTA_ImageState_t eSavedAgentState;
	OTA_FileContext_t OTA_FileContext;
}LOAD_FIRMWARE_CONTROL_BLOCK;

typedef struct _packet_block_for_queue
{
	uint32_t ulOffset;
	uint32_t length;
	uint8_t *p_packet;
}PACKET_BLOCK_FOR_QUEUE;

typedef struct _firmware_update_control_block
{
	uint8_t magic_code[7];
    uint8_t image_flag;
    uint8_t signature_type[32];
    uint32_t signature_size;
    uint8_t signature[256];
    uint32_t dataflash_flag;
    uint32_t dataflash_start_address;
    uint32_t dataflash_end_address;
    uint8_t reserved1[200];
    uint32_t sequence_number;
    uint32_t start_address;
    uint32_t end_address;
    uint32_t execution_address;
    uint32_t hardware_id;
    uint8_t reserved2[236];
}FIRMWARE_UPDATE_CONTROL_BLOCK;

static int32_t ota_context_validate( OTA_FileContext_t * C );
static int32_t ota_context_update_user_firmware_header( OTA_FileContext_t * C );
static void ota_context_close( OTA_FileContext_t * C );
static void ota_flashing_task( void * pvParameters );
static void ota_flashing_callback(void *event);
static void ota_header_flashing_callback(void *event);
static CK_RV prvGetCertificateHandle( CK_FUNCTION_LIST_PTR pxFunctionList,
                                      CK_SESSION_HANDLE xSession,
                                      const char * pcLabelName,
                                      CK_OBJECT_HANDLE_PTR pxCertHandle );
static CK_RV prvGetCertificate( const char * pcLabelName,
                                uint8_t ** ppucData,
                                uint32_t * pulDataSize );

static QueueHandle_t xQueue;
static TaskHandle_t xTask;
xSemaphoreHandle xSemaphoreFlashig;
xSemaphoreHandle xSemaphoreWriteBlock;
static volatile LOAD_FIRMWARE_CONTROL_BLOCK load_firmware_control_block;
static PACKET_BLOCK_FOR_QUEUE packet_block_for_queue1;
static PACKET_BLOCK_FOR_QUEUE packet_block_for_queue2;
static FIRMWARE_UPDATE_CONTROL_BLOCK *firmware_update_control_block_bank0 = (FIRMWARE_UPDATE_CONTROL_BLOCK*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS;
static volatile uint32_t gs_header_flashing_task;

/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_CreateFileForRx( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CreateFileForRx" );
    OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);
    OTA_LOG_L1("Compiled in [%s] [%s].\r\n", __DATE__, __TIME__);
    OTA_Err_t eResult = kOTA_Err_Uninitialized;
    static uint8_t *dummy_file_pointer;
    static uint8_t dummy_file;
    dummy_file_pointer = &dummy_file;
    flash_err_t flash_err;
    flash_interrupt_config_t cb_func_info;

    if( C != NULL )
    {
        if( C->pucFilePath != NULL )
        {
			C->pucFile = dummy_file_pointer;

			/* create task/queue/semaphore for flashing */
			xQueue = xQueueCreate(otaconfigMAX_NUM_BLOCKS_REQUEST, sizeof(PACKET_BLOCK_FOR_QUEUE));
			xTaskCreate(ota_flashing_task, "OTA_FLASHING_TASK", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES, &xTask);
			xSemaphoreFlashig = xSemaphoreCreateMutex();

			xSemaphoreWriteBlock = xSemaphoreCreateMutex();
//		    xSemaphoreGive(xSemaphoreFlashig);
//			xSemaphoreGive(xSemaphoreWriteBlock);

			flash_err = R_FLASH_Open();
			cb_func_info.pcallback = ota_flashing_callback;
			cb_func_info.int_priority = FLASH_INTERRUPT_PRIORITY;
			R_FLASH_Control(FLASH_CMD_SET_BGO_CALLBACK, (void *)&cb_func_info);

			if(flash_err == FLASH_SUCCESS)
			{
				eResult = kOTA_Err_None;
				OTA_LOG_L1( "[%s] Receive file created.\r\n", OTA_METHOD_NAME );
			}
			else
			{
				eResult = kOTA_Err_RxFileCreateFailed;
				OTA_LOG_L1( "[%s] ERROR - R_FLASH_Open() returns error.\r\n", OTA_METHOD_NAME );
			}
		}
        else
        {
            eResult = kOTA_Err_RxFileCreateFailed;
            OTA_LOG_L1( "[%s] ERROR - Invalid context provided.\r\n", OTA_METHOD_NAME );
        }
    }
    else
    {
        eResult = kOTA_Err_RxFileCreateFailed;
        OTA_LOG_L1( "[%s] ERROR - Invalid context provided.\r\n", OTA_METHOD_NAME );
    }

    return eResult;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_Abort( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_Abort" );

    OTA_Err_t eResult = kOTA_Err_None;

    if( ota_context_validate(C) == OTA_PAL_FAIL )
    {
        eResult = kOTA_Err_FileClose;
    }

	if (kOTA_Err_None == eResult)
	{
		/* delete task/queue for flashing */
		if (NULL != xTask)
		{
			vTaskDelete(xTask);
			xTask = NULL;
		}
		if (NULL != xQueue)
		{
			vQueueDelete(xQueue);
			xQueue = NULL;
		}
		if (NULL != xSemaphoreFlashig)
		{
			vSemaphoreDelete(xSemaphoreFlashig);
			xSemaphoreFlashig = NULL;
		}
		if (NULL != xSemaphoreWriteBlock)
		{
			vSemaphoreDelete(xSemaphoreWriteBlock);
			xSemaphoreWriteBlock = NULL;
		}

		R_FLASH_Close();
	}

	ota_context_close(C);
	OTA_LOG_L1( "[%s] OK\r\n", OTA_METHOD_NAME );
    return kOTA_Err_None;
}
/*-----------------------------------------------------------*/

/* Write a block of data to the specified file. */
int16_t prvPAL_WriteBlock( OTA_FileContext_t * const C,
                           uint32_t ulOffset,
                           uint8_t * const pacData,
                           uint32_t ulBlockSize )
{
    OTA_Err_t eResult = kOTA_Err_Uninitialized;
	uint8_t *packet_buffer;

    DEFINE_OTA_METHOD_NAME( "prvPAL_WriteBlock" );
	OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);
//	xSemaphoreGive(xSemaphoreFlashig);
	xSemaphoreTake(xSemaphoreWriteBlock, portMAX_DELAY);

	packet_buffer = pvPortMalloc(ulBlockSize);
    memcpy(packet_buffer, pacData, ulBlockSize);
    packet_block_for_queue1.p_packet = packet_buffer;
    packet_block_for_queue1.ulOffset = ulOffset + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH;
    packet_block_for_queue1.length = ulBlockSize;
	if(xQueueSend(xQueue, &packet_block_for_queue1, NULL) == pdPASS)
	{
		eResult = ( int16_t ) ulBlockSize;
	}
	else
	{
		vPortFree(packet_block_for_queue1.p_packet);
		OTA_LOG_L1("OTA flashing queue send error.\r\n");
		eResult = kOTA_Err_OutOfMemory;
	}
	
	xSemaphoreGive(xSemaphoreWriteBlock);
	
	return eResult;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_CloseFile( OTA_FileContext_t * const C )
{
	OTA_Err_t eResult = kOTA_Err_None;
    DEFINE_OTA_METHOD_NAME( "prvPAL_CloseFile" );

    if( ota_context_validate(C) == OTA_PAL_FAIL )
    {
        eResult = kOTA_Err_FileClose;
    }

    if( C->pxSignature != NULL )
    {
		eResult = prvPAL_CheckFileSignature(C);
	}
	else
    {
        eResult = kOTA_Err_SignatureCheckFailed;
    }

	if (kOTA_Err_None == eResult)
	{
		/* Update the user firmware header. */
		if (ota_context_update_user_firmware_header(C) == OTA_PAL_SUCCESS)
		{
			OTA_LOG_L1( "[%s] User firmware header updated.\r\n", OTA_METHOD_NAME );
		}
		else
		{
			OTA_LOG_L1( "[%s] ERROR: Failed to update the image header.\r\n", OTA_METHOD_NAME );
			eResult = kOTA_Err_FileClose;
		}
		/* delete task/queue for flashing */
		if (NULL != xTask)
		{
			vTaskDelete(xTask);
			xTask = NULL;
		}
		if (NULL != xQueue)
		{
			do
			{
				if (errQUEUE_FULL == xQueueReceive(xQueue, &packet_block_for_queue2, 0))
				{
					break;
				}
				else
				{
					vPortFree(packet_block_for_queue2.p_packet);
				}
			}
			while(1);
			vQueueDelete(xQueue);
			xQueue = NULL;
		}
		if (NULL != xSemaphoreFlashig)
		{
			vSemaphoreDelete(xSemaphoreFlashig);
			xSemaphoreFlashig = NULL;
		}
		if (NULL != xSemaphoreWriteBlock)
		{
			vSemaphoreDelete(xSemaphoreWriteBlock);
			xSemaphoreWriteBlock = NULL;
		}

		R_FLASH_Close();
	}

	ota_context_close(C);
	OTA_LOG_L1( "[%s] OK\r\n", OTA_METHOD_NAME );
	return eResult;
}
/*-----------------------------------------------------------*/


static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CheckFileSignature" );

    OTA_Err_t eResult;
    uint32_t ulSignerCertSize;
    void * pvSigVerifyContext;
    uint8_t * pucSignerCert = NULL;

    /* Verify an ECDSA-SHA256 signature. */
    if( CRYPTO_SignatureVerificationStart( &pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_ECDSA,
                                           cryptoHASH_ALGORITHM_SHA256 ) == pdFALSE )
    {
        return kOTA_Err_SignatureCheckFailed;
    }
    else
    {
        OTA_LOG_L1( "[%s] Started %s signature verification, file: %s\r\n", OTA_METHOD_NAME,
                    cOTA_JSON_FileSignatureKey, ( const char * ) C->pucCertFilepath );
        pucSignerCert = prvPAL_ReadAndAssumeCertificate( C->pucCertFilepath, &ulSignerCertSize );

        if( pucSignerCert == NULL )
        {
            eResult = kOTA_Err_BadSignerCert;
        }
        else
        {
            CRYPTO_SignatureVerificationUpdate( pvSigVerifyContext,
                                                (const uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
                                                (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);

            if( CRYPTO_SignatureVerificationFinal( pvSigVerifyContext, ( char * ) pucSignerCert, ulSignerCertSize,
                                                   C->pxSignature->ucData, C->pxSignature->usSize ) == pdFALSE )
            {
                eResult = kOTA_Err_SignatureCheckFailed;
            }
            else
            {
                eResult = kOTA_Err_None;
            }
        }
    }

    /* Free the signer certificate that we now own after prvPAL_ReadAndAssumeCertificate(). */
    if( pucSignerCert != NULL )
    {
        vPortFree( pucSignerCert );
    }

    return eResult;
}
/*-----------------------------------------------------------*/

static uint8_t * prvPAL_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_ReadAndAssumeCertificate" );

    uint8_t * pucCertData;
    uint32_t ulCertSize;
    uint8_t * pucSignerCert = NULL;
    CK_RV xResult;

    xResult = prvGetCertificate( ( const char * ) pucCertName, &pucSignerCert, ulSignerCertSize );

    if( ( xResult == CKR_OK ) && ( pucSignerCert != NULL ) )
    {
        OTA_LOG_L1( "[%s] Using cert with label: %s OK\r\n", OTA_METHOD_NAME, ( const char * ) pucCertName );
    }
    else
    {
        OTA_LOG_L1( "[%s] No such certificate file: %s. Using aws_ota_codesigner_certificate.h.\r\n", OTA_METHOD_NAME,
                    ( const char * ) pucCertName );

        /* Allocate memory for the signer certificate plus a terminating zero so we can copy it and return to the caller. */
        ulCertSize = sizeof( signingcredentialSIGNING_CERTIFICATE_PEM );
        pucSignerCert = pvPortMalloc( ulCertSize + 1 );                       /*lint !e9029 !e9079 !e838 malloc proto requires void*. */
        pucCertData = ( uint8_t * ) signingcredentialSIGNING_CERTIFICATE_PEM; /*lint !e9005 we don't modify the cert but it could be set by PKCS11 so it's not const. */

        if( pucSignerCert != NULL )
        {
            memcpy( pucSignerCert, pucCertData, ulCertSize );
            /* The crypto code requires the terminating zero to be part of the length so add 1 to the size. */
            pucSignerCert[ ulCertSize ] = 0U;
            *ulSignerCertSize = ulCertSize + 1U;
        }
        else
        {
            OTA_LOG_L1( "[%s] Error: No memory for certificate of size %d!\r\n", OTA_METHOD_NAME, ulCertSize );
        }
    }

    return pucSignerCert;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_ResetDevice( void )
{
    DEFINE_OTA_METHOD_NAME("prvPAL_ResetDevice");

    OTA_LOG_L1( "[%s] Resetting the device.\r\n", OTA_METHOD_NAME );

    /* Software reset issued (Not bank swap) */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
    SYSTEM.SWRR = 0xa501;
	while(1);	/* software reset */

    /* We shouldn't actually get here if the board supports the auto reset.
     * But, it doesn't hurt anything if we do although someone will need to
     * reset the device for the new image to boot. */
    return kOTA_Err_None;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_ActivateNewImage( void )
{
    DEFINE_OTA_METHOD_NAME("prvPAL_ActivateNewImage");

    OTA_LOG_L1( "[%s] Changing the Startup Bank\r\n", OTA_METHOD_NAME );

    /* reset for self testing */
	vTaskDelay(5000);
	prvPAL_ResetDevice();	/* no return from this function */

    return kOTA_Err_None;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_SetPlatformImageState( OTA_ImageState_t eState )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_SetPlatformImageState" );
    OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);

    OTA_Err_t eResult = kOTA_Err_None;

    if( eState != eOTA_ImageState_Unknown && eState <= eOTA_LastImageState )
    {
    	/* Save the image state to eSavedAgentState. */
    	load_firmware_control_block.eSavedAgentState = eState;
    }
    else /* Image state invalid. */
    {
        OTA_LOG_L1( "[%s] ERROR - Invalid image state provided.\r\n", OTA_METHOD_NAME );
        eResult = kOTA_Err_BadImageState;
    }

    return eResult;
}
/*-----------------------------------------------------------*/

OTA_PAL_ImageState_t prvPAL_GetPlatformImageState( void )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_GetPlatformImageState" );
    OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);

	OTA_PAL_ImageState_t ePalState = eOTA_PAL_ImageState_Unknown;

	switch (load_firmware_control_block.eSavedAgentState)
	{
		case eOTA_ImageState_Testing:
			ePalState = eOTA_PAL_ImageState_PendingCommit;
			break;
		case eOTA_ImageState_Accepted:
			ePalState = eOTA_PAL_ImageState_Valid;
			break;
		case eOTA_ImageState_Unknown: // Uninitialized image state, assume a factory image
			ePalState = eOTA_PAL_ImageState_Valid;
			break;
		case eOTA_ImageState_Rejected:
		case eOTA_ImageState_Aborted:
		default:
			ePalState = eOTA_PAL_ImageState_Invalid;
			break;
	}

	OTA_LOG_L1("Function call: prvPAL_GetPlatformImageState: [%d]\r\n", ePalState);
    return ePalState; /*lint !e64 !e480 !e481 I/O calls and return type are used per design. */
}
/*-----------------------------------------------------------*/

/* Provide access to private members for testing. */
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
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

static int32_t ota_context_validate( OTA_FileContext_t * C )
{
	return ( NULL != C );
}

static int32_t ota_context_update_user_firmware_header( OTA_FileContext_t * C )
{
    int32_t ret = OTA_PAL_FAIL;
	uint8_t block[BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH];
	FIRMWARE_UPDATE_CONTROL_BLOCK * p_block_header;
	uint32_t length;
    flash_err_t flash_err;
    flash_interrupt_config_t cb_func_info;
	uint8_t *source_pointer, *destination_pointer;
	uint8_t data_length = 0;

	memcpy(block, (const void *)firmware_update_control_block_bank0, BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
	p_block_header = (FIRMWARE_UPDATE_CONTROL_BLOCK *)block;

	/* Update image flag. */
	p_block_header->image_flag = LIFECYCLE_STATE_TESTING;

	/* Parse the signature and extract ECDSA-SHA256 signature data. */
	source_pointer = C->pxSignature->ucData;
	destination_pointer = p_block_header->signature;
	data_length = *(source_pointer + 1) + OTA_SIGUNATURE_SEQUENCE_INFO_LENGTH;
	memset(destination_pointer, 0, sizeof(destination_pointer));
	if (OTA_SIGUNATURE_SEQUENCE_TOP_VALUE == *source_pointer)
	{
		source_pointer += OTA_SIGUNATURE_SEQUENCE_INFO_LENGTH;
		while(1)
		{
			if (OTA_SIGUNATURE_INTEGER_VALUE == *source_pointer)
			{
				source_pointer++;
				if (OTA_SIGUNATURE_INCLUDE_NEGATIVE_NUMBER_VALUE == *source_pointer)
				{
					source_pointer += OTA_SIGUNATURE_SKIP;
				}
				else
				{
					source_pointer++;
				}
				memcpy(destination_pointer, source_pointer, OTA_SIGUNATURE_DATA_HALF_LENGTH);
				source_pointer += OTA_SIGUNATURE_DATA_HALF_LENGTH;
				destination_pointer += OTA_SIGUNATURE_DATA_HALF_LENGTH;
				if ((source_pointer - C->pxSignature->ucData) == data_length)
				{
					ret = OTA_PAL_SUCCESS;
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

	if (OTA_PAL_SUCCESS == ret)
	{
		xSemaphoreTake(xSemaphoreFlashig, portMAX_DELAY);
		R_FLASH_Close();
		R_FLASH_Open();
		cb_func_info.pcallback = ota_header_flashing_callback;
		cb_func_info.int_priority = FLASH_INTERRUPT_PRIORITY;
		R_FLASH_Control(FLASH_CMD_SET_BGO_CALLBACK, (void *)&cb_func_info);

		gs_header_flashing_task = OTA_FLASHING_IN_PROGRESS;
		length = BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH;
		flash_err = R_FLASH_Write((uint32_t)block, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, length);
		if(flash_err != FLASH_SUCCESS)
		{
			R_BSP_NOP();
		}
		while (OTA_FLASHING_IN_PROGRESS == gs_header_flashing_task);
	}

	return ret;
}

static void ota_context_close( OTA_FileContext_t * C )
{
    if( NULL != C )
    {
        C->pucFile = NULL;
    }
}


static void ota_flashing_task( void * pvParameters )
{
	flash_err_t flash_err;
	static uint8_t block[(1 << otaconfigLOG2_FILE_BLOCK_SIZE)];
	static uint32_t ulOffset;
	static uint32_t length;

	while(1)
	{

		xQueueReceive(xQueue, &packet_block_for_queue2, portMAX_DELAY);
		DEFINE_OTA_METHOD_NAME( "ota_flashing_task" );
		OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);
//		xSemaphoreGive(xSemaphoreWriteBlock);
		xSemaphoreTake(xSemaphoreFlashig, portMAX_DELAY);
		memcpy(block, packet_block_for_queue2.p_packet, sizeof(block));
		ulOffset = packet_block_for_queue2.ulOffset;
		length = packet_block_for_queue2.length;
		flash_err = R_FLASH_Write((uint32_t)block, ulOffset + (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, length);
		if(packet_block_for_queue2.length != 1024)
		{
			R_BSP_NOP();
		}
		if(flash_err != FLASH_SUCCESS)
		{
			R_BSP_NOP();
		}
		vPortFree(packet_block_for_queue2.p_packet);

	}
}

static void ota_flashing_callback(void *event)
{
	uint32_t event_code;
	event_code = *((uint32_t*)event);

    if((event_code != FLASH_INT_EVENT_WRITE_COMPLETE) || (event_code == FLASH_INT_EVENT_ERASE_COMPLETE))
    {
    	R_BSP_NOP(); /* trap */
    }
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xSemaphoreGiveFromISR(xSemaphoreFlashig, &xHigherPriorityTaskWoken);
}

static void ota_header_flashing_callback(void *event)
{
	uint32_t event_code;
	event_code = *((uint32_t*)event);

	gs_header_flashing_task = OTA_FLASHING_COMPLETE;

    if((event_code != FLASH_INT_EVENT_WRITE_COMPLETE) || (event_code == FLASH_INT_EVENT_ERASE_COMPLETE))
    {
    	R_BSP_NOP(); /* trap */
    }
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xSemaphoreGiveFromISR(xSemaphoreFlashig, &xHigherPriorityTaskWoken);
}
