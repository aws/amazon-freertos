/*
Amazon FreeRTOS OTA PAL for STM32L4 Discovery kit IoT node V0.9.1
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

/*
	Platform abstraction layer for Amazon FreeRTOS OTA agent.

	This file contains the OTA support functions specific to the STM32L4xx target platform.

	TARGET_PLATFORM_STM32L4XX
*/

#ifdef INCLUDE_FROM_OTA_AGENT

#include "stm32l475xx.h"
#include "stm32l4xx_hal.h"
#include "flash.h"
#include "aws_crypto.h"
#include "stdbool.h"

/* Specify the OTA signature algorithm we support on this platform. */
static const char acOTA_JSON_FileSignatureKey[] = "sig-sha256-ecdsa";

/* Internal function to write the authentication certificate to flash. */
static uint32_t prvWriteAuthCertificate( void );

#define CRYPTO

  static const char pcClientCertificatePem[] =
	"-----BEGIN CERTIFICATE-----\r\n"
	"MIIDtzCCAp+gAwIBAgIJAMAJ51I17/tXMA0GCSqGSIb3DQEBCwUAMHIxCzAJBgNV\n"
	"BAYTAlVTMQswCQYDVQQIDAJXQTEQMA4GA1UEBwwHU2VhdHRsZTEMMAoGA1UECgwD\n"
	"QVdTMQwwCgYDVQQLDANJb1QxDDAKBgNVBAMMA0RhbjEaMBgGCSqGSIb3DQEJARYL\n"
	"ZGFuQGZvby5jb20wHhcNMTcwNzMxMjI0OTE3WhcNMTgwNzMxMjI0OTE3WjByMQsw\n"
	"CQYDVQQGEwJVUzELMAkGA1UECAwCV0ExEDAOBgNVBAcMB1NlYXR0bGUxDDAKBgNV\n"
	"BAoMA0FXUzEMMAoGA1UECwwDSW9UMQwwCgYDVQQDDANEYW4xGjAYBgkqhkiG9w0B\n"
	"CQEWC2RhbkBmb28uY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA\n"
	"rQB70DjusUYtNRgGfnrt9bzVT6K50TEwC22FTTmMFzc2TayGZLyXvVEvpaT3RSQp\n"
	"WPXtSN6PI9P/r9t1281UqfeBkt/0Mg5p1Yqd7XhXdziREi/pcKrj/umk1AsrgL7t\n"
	"9XuwIFX5hluej6MyDDAuhtPFAdu7WGAdZr+aBrZyohAgfFcBSNs9/bLoU36FxoWM\n"
	"0iByBztnEdy3k+b+JqwAUOxYLnZTSD28eO4AwXj1WZ2VSLBs+DeDL6SAjRy7v7DF\n"
	"+vrqw82OoYmSk5nzLBYf16H8cEW/zVeWlP+Liw0WTJNGGp2YCwlNx20ywHiNBZDd\n"
	"f/Djw7GsObG5oRs7939kJwIDAQABo1AwTjAdBgNVHQ4EFgQUokZzCOJv6EW6dUHn\n"
	"oDAAQG5BKYcwHwYDVR0jBBgwFoAUokZzCOJv6EW6dUHnoDAAQG5BKYcwDAYDVR0T\n"
	"BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAXO/ggGB8ykOozOs6mciY2kH1l6yI\n"
	"ldnvXn6ilsfICWxj0Xvd+HoDe64ycUmcLUmN2U4NVD42+l9AoZJ6KaewTSiqWo15\n"
	"cMxUs+huysXYS3Kynv/TUTvhqh8yia1mkTFetfDWzgjVsaV0vb9KJcziQXIzAaLc\n"
	"O7WjvCW90YZpxlAwlJgtLfCpD0T1V4vzgT7w2sDMmE8637+3/eDNtP+U/CK+eo0w\n"
	"fjBdyj+3TOG3F6/X4+SFrk+64++CoWuD/y9yJ8FBpPEv2jW2EDWL6quWyqLs1Qye\n"
	"KiOc4Xj4X034Bm9JF5tnaj51igCWrXBcxUHrh2xfkMuQvKyIu6Bkz0IbgQ==\n"
	"-----END CERTIFICATE-----\n";


/**
  ******************************************************************************
  * @file    iot_flash_config.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    12-April-2017
  * @brief   Header for configuration
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __iot_flash_config_H
#define __iot_flash_config_H
#ifdef __cplusplus
 extern "C" {
#endif

#define USER_CONF_WIFI_SSID_MAX_LENGTH  32
#define USER_CONF_WIFI_PSK_MAX_LENGTH   64
#define USER_CONF_DEVICE_NAME_LENGTH    32
#define USER_CONF_TLS_OBJECT_MAX_SIZE   2048
#define USER_CONF_MAGIC                 0x0123456789ABCDEFuLL

typedef struct {
  uint64_t magic;                             /**< The USER_CONF_MAGIC magic word signals that the structure was once written to FLASH. */
  char ssid[USER_CONF_WIFI_SSID_MAX_LENGTH];  /**< Wifi network SSID. */
  char psk[USER_CONF_WIFI_PSK_MAX_LENGTH];    /**< Wifi network PSK. */
  uint8_t security_mode;                      /**< Wifi network security mode. See @ref wifi_network_security_t definition. */
} wifi_config_t;

typedef struct {
  uint64_t magic;                             /**< The USER_CONF_MAGIC magic word signals that the structure was once written to FLASH. */
  char device_name[USER_CONF_DEVICE_NAME_LENGTH];
} iot_config_t;

/** Static user configuration data which must survive reboot and firmware update. */
/* Do not chnage field order , due to firewall constraint the tls_device_key size must be place 64 bit boundary */
/* its size sould also be multiple of 64 bits.                                                                  */
typedef struct {
  char tls_root_ca_cert[USER_CONF_TLS_OBJECT_MAX_SIZE];
  char tls_device_cert[USER_CONF_TLS_OBJECT_MAX_SIZE];
  // Above structure member must be aligned on 256 bye  boundary
   // to match firewall constraint , same for the size.
  char tls_device_key[USER_CONF_TLS_OBJECT_MAX_SIZE];
  wifi_config_t wifi_config;
  iot_config_t iot_config;
  uint64_t tls_magic;                          /**< The USER_CONF_MAGIC magic word signals that the TLS strings above was once written to FLASH. */

} user_config_t;



#ifdef __cplusplus
}
#endif
#endif /* __iot_flash_config_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

 static int prvCheckTLSCredentials( void );

 /* Private variables ---------------------------------------------------------*/
 /** Do not zero-initialize the static user configuration.
  *  Otherwise, it must be entered manually each time the device FW is updated by STLink.
  */

 #ifdef __ICCARM__  /* IAR */
 __no_init const user_config_t lUserConfig @ "UNINIT_FIXED_LOC";
 #elif defined ( __CC_ARM   )/* Keil / armcc */
 user_config_t lUserConfig __attribute__((section("UNINIT_FIXED_LOC"), zero_init));
 #elif defined ( __GNUC__ )      /*GNU Compiler */
 user_config_t lUserConfig __attribute__((section("UNINIT_FIXED_LOC")));
 #endif
 /************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/



/* Abort receiving the specified OTA update by closing the file.
 * This shall do whatever is necessary to clean up the file on the STM32L4xx platform.
 * Today this is does not require much of anything.
 */

static OTA_Err_t prvAbort(OTA_FileContext_t * const C)
{
	s32 iStatus = 0;
	C->iFileHandle = (s32) NULL;
	return iStatus;
}


/* Attempt to create a new receive file for the OTA update. */

static bool_t prvCreateFileForRx(OTA_FileContext_t * const C)
{
	uint8_t	*pucFile;
	bool bResult = pdFALSE;

	// ToDo: Put authentication certificate in flash.
	// ToDo: This should support receiving certificate files instead of built-in as here.
	/*if (prvCheckTLSCredentials() == -1)
	{
		prvWriteAuthCertificate();
	}*/

	pucFile = ( uint8_t* ) FLASH_get_alternate_bank_addr( );
	OTA_PRINT( "[OTA] Preparing receive file at 0x%x...\r\n", pucFile );

	// ToDo: This should erase the entire flash code area to prepare for a new image.
	//FLASH_erase_bank( pucFile );
	if ( pucFile != NULL)
	{
		/* ToDo: Mark file as the firmware image. This should come from the OTA meta data. */
		//C->mFlags = kOTA_Flags_Firmware;
		/* Save pointer to the file storage location in flash. */
		C->pucFile = pucFile;
		bResult = pdTRUE;
	}

	return bResult;
}


/* Close the specified file. This may authenticate the file if it is marked as secure. */

static OTA_Err_t prvCloseFile(OTA_FileContext_t * const C)
{
	s32 err;

	/* The complete file is now received so verify the file signature and return the result. */
	err = prvCheckFileSignature( C );
	if ( err == kOTA_Err_None )
	{
			/* Activate the new image by resetting and to activate bootloader. */
		     prvActivateNewImage();

	}
	return err;
}


static OTA_Err_t prvActivateNewImage(void)
{

	/*Reset the system to activate new image*/

	NVIC_SystemReset();

	/* ToDo: Never returns. */
	return kOTA_Err_None;
}

/* Set the final state of the last transferred (final) OTA file.
 */

static OTA_Err_t prvSetImageState (OTA_ImageState_t eState)
{
	return kOTA_Err_None;
}



/* Verify the signature of the specified file. */

static OTA_Err_t prvCheckFileSignature(OTA_FileContext_t * const C)
{
	OTA_Err_t err = kOTA_Err_None;
	s32		lSignerCertSize;
	uint8_t	*pucBuf, *pucSignerCert;
	void	*pvSigVerifyContext;

	/* Verify an RSA-SHA1 signature. */
#ifdef CRYPTO
	if (pdFALSE == CRYPTO_SignatureVerificationStart( &pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_ECDSA, cryptoHASH_ALGORITHM_SHA256))
	{
		OTA_PRINT("[OTA] CRYPTO_SignatureVerificationStart failed for RSA SHA1\r\n");
	}
	else
	{
		OTA_PRINT("[OTA] Started RSA SHA1 signature verification\r\n");
	}
#endif

	pucSignerCert = prvReadAndAssumeCertificate(C->pacCertFilepath, &lSignerCertSize);
	if (pucSignerCert != NULL)
	{
		{
			/* Determine which flash bank we're in and prepare to authenticate the alternate. */
			pucBuf = ( uint8_t* ) FLASH_get_alternate_bank_addr( );
			if (pucBuf != NULL)
			{
				/* Call the crypto code with a pointer to the MCU code and the authentication certificate. */
#ifdef CRYPTO
				CRYPTO_SignatureVerificationUpdate(pvSigVerifyContext, pucBuf, C->iFileSize);
				if (pdFALSE == CRYPTO_SignatureVerificationFinal(pvSigVerifyContext, (char*)pucSignerCert, lSignerCertSize, C->pacSignature, C->usSigSize))
				{
					err = kOTA_Err_SignatureCheckFailed;
				}
#endif
			}
			else
			{
				err = kOTA_Err_SignatureCheckFailed;
			}
		}
		/* No need to free the certificate since it is in flash memory. */
	}
	else
	{
		err = kOTA_Err_BadSignerCert;
	}
	return err;
}


/* Read the specified signer certificate from the file system into a local buffer. The allocated
   memory becomes the property of the caller who is responsible for freeing it.
 */

static u8 * prvReadAndAssumeCertificate(const u8 * const pucCertName, s32 * const lSignerCertSize)
{
	/* The crypto code requires the terminating zero to be part of the length so add 1. */
	*lSignerCertSize = strlen(pcClientCertificatePem) + 1;
	return (u8*)pcClientCertificatePem;
}


/* Write a block of data to the specified file. */

static s16 prvWriteBlock(OTA_FileContext_t * const C, s32 iOffset, u8 * const pacData, u32 iBlockSize)
{
	u8 *pucBuf = C->pucFile;	// FLASH_get_alternate_bank_addr( );
	if (pucBuf != NULL)
	{
		iOffset += (s32)pucBuf;
		s16	sBytesWritten = (s16) FLASH_update((uint32_t)iOffset, pacData, iBlockSize);
		return sBytesWritten;
	}
	/* Temp return error code on a failed write. */
	return -1;
}


/*---------------------------------------------------------------------------*/
/*				ST code to manage security credentials in flash.		     */
/*---------------------------------------------------------------------------*/

/**
  * @brief  Check if the TLS credentials are present in FLASH memory.
  * @param  void
  * @retval 0 TLS credentials are configured,
           -1 No TLS credentials found.
  */
static int prvCheckTLSCredentials( void )
{
  return (lUserConfig.tls_magic == USER_CONF_MAGIC) ? 0 : -1;
}

/**
  * @brief  Write the OTA authentication certificate to internal FLASH memory.
  * @param  none
  * @retval	Error code
  * 			0	Success
  * 			<0	Unrecoverable error
  */
static uint32_t prvWriteAuthCertificate( void )
{
	int ret = 0;

	/* Write certificate to flash */
	ret = FLASH_update((uint32_t)lUserConfig.tls_device_cert, pcClientCertificatePem, strlen(pcClientCertificatePem) + 1);  /* +1: Include the closing \0 */
	if (ret == 0)
	{
		/* Write the magic number indicating we have an authentication certificate in flash. */
		uint64_t magic = USER_CONF_MAGIC;
		ret = FLASH_update((uint32_t)&lUserConfig.tls_magic, &magic, sizeof(uint64_t));
	}
	return ret;
}


#endif /* TARGET_PLATFORM_STM32L4XX */

