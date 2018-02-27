/*
Amazon FreeRTOS OTA PAL for CC3220SF-LAUNCHXL V0.9.2
Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
*/

/* OTA PAL implementation for TI CC3220SF platform. */

#ifdef INCLUDE_FROM_OTA_AGENT

/* TI Simplelink includes */
#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/devices/cc32xx/driverlib/rom_map.h>
extern void PRCMHibernateCycleTrigger(void);

/* Specify the OTA signature algorithm we support on this platform. */
static const char acOTA_JSON_FileSignatureKey[] = "sig-sha1-rsa";

#define OTA_FW_IMAGE_FILENAME "/sys/mcuflashimg.bin"					/* This is the MCU image filename on the CC3220SF. */
#define OTA_VENDOR_TOKEN	0x74594452
#define OTA_MAX_MCU_IMAGE_SIZE		(512 * 1024)						/* Maximum allowed size of an MCU update image for this platform. */
static const char acOTA_TI_AbortSig[] = "A";                          /* Use this signature to abort a file transfer on the TI CC3220 platform. */

/* TI CC3220SF file rollback API. */
static bool_t prvRollbackRxFile(OTA_FileContext_t *C);


/* Abort access to an existing open file. This is only valid after a job starts successfully. */

static OTA_Err_t prvAbort(OTA_FileContext_t *C)
{
	int32_t result;

	/* Check for null file handle since we may call this before a file is actually opened. */
	if (C->iFileHandle != NULL)
	{
		result = sl_FsClose(C->iFileHandle, NULL, (uint8_t*)acOTA_TI_AbortSig, CONST_STRLEN(acOTA_TI_AbortSig));
		C->iFileHandle = NULL;
		if ( result != 0 )
		{
			return kOTA_Err_FileAbort | (result & kOTA_PAL_ErrMask);
		}
		else
		{
			/* Nothing special on success. */
		}
	}
	return kOTA_Err_None;
}


/* Attempt to create a new receive file to write the file chunks to as they come in. */

static bool_t prvCreateFileForRx(OTA_FileContext_t *C)
{
	uint32_t    ulToken = OTA_VENDOR_TOKEN;	/* TI platform requires file tokens. We use a vendor token. */
	uint32_t    iFlags;                      /* Flags used when opening the OTA FW image file. */
	int32_t		result;
	bool_t		bRetry;

	do {
		bRetry = pdFALSE;
		iFlags = SL_FS_CREATE | SL_FS_OVERWRITE | SL_FS_CREATE_FAILSAFE | SL_FS_CREATE_PUBLIC_WRITE |
			SL_FS_CREATE_SECURE | SL_FS_CREATE_VENDOR_TOKEN | SL_FS_CREATE_MAX_SIZE(OTA_MAX_MCU_IMAGE_SIZE);
		C->iFileHandle = sl_FsOpen( (_u8*)C->pacFilepath, iFlags, (_u32*)&ulToken );
		if (C->iFileHandle > 0)
		{
			OTA_PRINT("[OTA] file token: %08x\r\n", ulToken);
			result = sl_FsClose(C->iFileHandle, NULL, (uint8_t*)acOTA_TI_AbortSig, CONST_STRLEN(acOTA_TI_AbortSig));
			if ( result == 0 )
			{
				iFlags = SL_FS_WRITE | SL_FS_WRITE_MUST_COMMIT;
				C->iFileHandle = sl_FsOpen( (_u8*)C->pacFilepath, iFlags, (_u32*)&ulToken );
				if (C->iFileHandle > 0)
				{
					OTA_PRINT("[OTA] file ready for access.\r\n");
				}
				else
				{
					/* Show error message and nullify the file handle. */
					OTA_PRINT("[OTA] Error (%d) trying to re-open for access.\r\n", C->iFileHandle);
	                C->iFileHandle = NULL;
				}
			}
			else
			{
				OTA_PRINT("[OTA] Error (%d) when closing new file.\r\n", result);
				/* Invalidate the file handle. */
				C->iFileHandle = NULL;
			}
		}
		else {
			OTA_PRINT("[OTA] Error (%d) trying to create receive file.\r\n", C->iFileHandle);
			if ( C->iFileHandle == SL_ERROR_FS_FILE_IS_PENDING_COMMIT )
			{
				if ( prvRollbackRxFile(C) == pdTRUE )
				{
					bRetry = pdTRUE;
				}
			}
			else if ( C->iFileHandle == SL_ERROR_FS_FILE_IS_ALREADY_OPENED )
			{
				/* System is in an inconsistent state and must be rebooted. */
				prvActivateNewImage();
			}
			else
			{
			    /* Nothing special. */
			}
            C->iFileHandle = NULL;      /* Nullify the file handle in all error cases. */
		}
	} while ( bRetry == pdTRUE );

	return ( C->iFileHandle > 0 ) ? pdTRUE : pdFALSE;
}


static bool_t prvRollbackRxFile(OTA_FileContext_t *C)
{
	int32_t result;
	uint32_t ulNewToken;			/* New token is not retained. */
	SlFsControl_t FsControl;

	FsControl.IncludeFilters = 0;
	result = sl_FsCtl(SL_FS_CTL_ROLLBACK, OTA_VENDOR_TOKEN, (_u8*)C->pacFilepath ,(_u8*)&FsControl, sizeof(SlFsControl_t), NULL, 0, (_u32*) &ulNewToken );
	if (result != 0)
	{
		OTA_PRINT("[OTA] Rollback %s failed (%d).\r\n", C->pacFilepath, result);
		return pdFALSE;
	}
	else
	{
		OTA_PRINT("[OTA] Rolled back %s\r\n", C->pacFilepath);
		return pdTRUE;
	}

}


/* Close the specified file. This will also authenticate the file if it is marked as secure. */

static OTA_Err_t prvCloseFile(OTA_FileContext_t *C)
{
	int32_t result;

	/* Let SimpleLink API handle error checks so we get an error code for free. */
	OTA_PRINT("[OTA] Authenticating and closing file.\r\n");
	result = sl_FsClose(C->iFileHandle, (uint8_t*)C->pacCertFilepath, (uint8_t*)C->pacSignature, C->usSigSize);
	if ( result != 0 )
	{
		return kOTA_Err_FileClose | (result & kOTA_PAL_ErrMask);
	}
	else
	{
		return kOTA_Err_None;
	}
}


static OTA_Err_t prvActivateNewImage(void)
{
    /* Reset the MCU so we can test the new image. Short delay for debug log output. */
 	OTA_PRINT("[OTA] Resetting MCU to activate new image.\r\n");
    vTaskDelay( kOTA_HalfSecondDelay );
    MAP_PRCMHibernateCycleTrigger();

    /* We shouldn't actually get here if the board supports the auto reset.
     * But, it doesn't hurt anything if we do although someone will need to
     * reset the device for the new image to boot. */
	return kOTA_Err_None;
}


/* Set the final state of the last transferred (final) OTA file.
 * For the TI CC3220SF, commit the image if the state == eOTA_ImageState_Accepted.
 */

static OTA_Err_t prvSetImageState (OTA_ImageState_t eState)
{
	int32_t result;
	uint32_t ulNewToken;			/* New token is not retained. */

	if (eState == eOTA_ImageState_Accepted)
	{
		SlFsControl_t FsControl;
		FsControl.IncludeFilters = 0;
		OTA_PRINT("[OTA] Accepted final image. Commit.\r\n");
		result = sl_FsCtl(SL_FS_CTL_COMMIT, OTA_VENDOR_TOKEN, OTA_FW_IMAGE_FILENAME ,(_u8*)&FsControl, sizeof(SlFsControl_t), NULL, 0, (_u32*) &ulNewToken );
		if (result != 0)
		{
			OTA_PRINT("[OTA] Commit failed (%d).\r\n", result);
			return kOTA_Err_CommitFailed | (result & kOTA_PAL_ErrMask);
		}
		else
		{
			/* Nothing special to do on success. */
		}
	}
	return kOTA_Err_None;
}


/* Write a block of data to the specified file.
 * Returns the number of bytes written on success or negative error code.
 */

int16_t prvWriteBlock(OTA_FileContext_t *C, int32_t iOffset, uint8_t* pacData, uint32_t iBlockSize)
{
	return (int16_t)sl_FsWrite(C->iFileHandle, iOffset, pacData, iBlockSize);
}


#else
void dummy_function_to_avoid_empty_translation_unit_warning (void);
#endif /* INCLUDE_FROM_OTA_AGENT */

