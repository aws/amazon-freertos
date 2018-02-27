/*
Amazon FreeRTOS OTA PAL for Windows Simulator V0.9.1
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

/* OTA PAL implementation for Windows platform. */

#ifdef INCLUDE_FROM_OTA_AGENT

#include <stdio.h>
#include "aws_crypto.h"

/* Specify the OTA signature algorithm we support on this platform. */
static const char acOTA_JSON_FileSignatureKey[] = "sig-sha256-rsa";

/* Abort receiving the specified OTA update by closing the file. */

static OTA_Err_t prvAbort(OTA_FileContext_t * const C)
{
	/* Set default return status to success. */
	s32 iStatus = 0;

	if (C->pstFile != NULL)
	{
		iStatus = fclose(C->pstFile);
		C->pstFile = NULL;
	}
	return iStatus;
}


/* Attempt to create a new receive file for the file chunks as they come in. */

static bool_t prvCreateFileForRx(OTA_FileContext_t * const C)
{
    bool_t xStatus;

    C->pstFile = fopen((const char*)C->pacFilepath, "w+b");
	if ( C->pstFile != NULL)
	{
		xStatus = pdTRUE;
		OTA_PRINT("[OTA] file handle: %08x\r\n", (u32)C->pstFile );
	}
	else {
		xStatus = pdFALSE;
		OTA_PRINT("[OTA] Error (%d) trying to open local receive file.\r\n", (s32)C->pstFile );
	}
	return xStatus;
}


/* Close the specified file. This shall authenticate the file if it is marked as secure. */

static OTA_Err_t prvCloseFile(OTA_FileContext_t * const C)
{
	OTA_Err_t result;

	if (C->pacSignature != NULL)
	{
		/* Verify the file signature, close the file and return the signature verification result. */
		result = prvCheckFileSignature (C);
	}
	else
	{
		result = kOTA_Err_SignatureCheckFailed;
	}
	if (C->pstFile != NULL)
	{
		fclose(C->pstFile);
		C->pstFile = NULL;
	}
	if (result == kOTA_Err_None)
	{
		OTA_PRINT ("[OTA] %s signature verification passed.\r\n", acOTA_JSON_FileSignatureKey);
	}
	else
	{
		OTA_PRINT ("[OTA] ERROR: Failed to start %s signature verification.\r\n", acOTA_JSON_FileSignatureKey);
	}
	return result;
}


static OTA_Err_t prvActivateNewImage(void)
{
	/* Return no error. Windows implementation simply does nothing on activate. */
	return kOTA_Err_None;
}


/* Set the final state of the last transferred (final) OTA file (or bundle).
   On Windows, there is nothing special to do at the platform layer.
 */

static OTA_Err_t prvSetImageState (OTA_ImageState_t eState)
{
	(void)eState;
	return kOTA_Err_None;
}


/* Verify the signature of the specified file. */

static OTA_Err_t prvCheckFileSignature(OTA_FileContext_t * const C)
{
	OTA_Err_t err = kOTA_Err_None;
	u32		ulBytesRead;
	u32		ulTotalBytes = 0;
	s32		lSignerCertSize;
	u8		*pucBuf, *pucSignerCert;
	void	*pvSigVerifyContext;

	/* Verify an RSA-SHA256 signature. */
	if (pdFALSE == CRYPTO_SignatureVerificationStart( &pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_RSA, cryptoHASH_ALGORITHM_SHA256))
	{
		err = kOTA_Err_SignatureCheckFailed;
	}
	else
	{
		OTA_PRINT("[OTA] Started %s signature verification\r\n", acOTA_JSON_FileSignatureKey);
		pucSignerCert = prvReadAndAssumeCertificate((const u8* const)C->pacCertFilepath, &lSignerCertSize);
		if (pucSignerCert != NULL)
		{
			pucBuf = pvPortMalloc(kOTA_FileBlockSize);
			if ((pucSignerCert != NULL) && (pucBuf != NULL))
			{
				if (C->pstFile != NULL) {
					/* Rewind the received file to the beginning. */
					if (fseek(C->pstFile, 0L, SEEK_SET) == 0)
					{
						do
						{
							ulBytesRead = fread(pucBuf, 1, kOTA_FileBlockSize, C->pstFile);
							ulTotalBytes += ulBytesRead;
							/* Include the file chunk in the signature validation. Zero size is OK. */
							CRYPTO_SignatureVerificationUpdate(pvSigVerifyContext, pucBuf, ulBytesRead);

						} while (ulBytesRead > 0);
						if (pdFALSE == CRYPTO_SignatureVerificationFinal(pvSigVerifyContext, (char*)pucSignerCert, lSignerCertSize, C->pacSignature, C->usSigSize))
						{
							err = kOTA_Err_SignatureCheckFailed;
						}
					}
					else
					{
						/* Nothing special to do. */
					}
				}
				else
				{
					err = kOTA_Err_NullFilePtr;
				}
				/* Free the temporary file page buffer. */
				vPortFree(pucBuf);
			}
			else
			{
				err = kOTA_Err_OutOfMemory;
			}
			/* Free the signer certificate that we now own after prvReadAndAssumeCertificate(). */
			vPortFree(pucSignerCert);
		}
		else
		{
			err = kOTA_Err_BadSignerCert;
		}
	}
	return err;
}


/* Read the specified signer certificate from the filesystem into a local buffer. The allocated
   memory becomes the property of the caller who is responsible for freeing it.
 */

static u8 * prvReadAndAssumeCertificate(const u8 * const pucCertName, s32 * const lSignerCertSize)
{
	FILE	*pstFile;
	u8		*pucSignerCert = NULL;
	s32		lSize;

	pstFile = fopen((const char*)pucCertName, "rb");
	if (pstFile != NULL) {

		fseek(pstFile, 0, SEEK_END);
		lSize = (s32) ftell(pstFile);
		fseek(pstFile, 0, SEEK_SET);
		/* Allocate memory for the signer certificate plus a terminating zero so we can load it and return to the caller. */
		pucSignerCert = pvPortMalloc(lSize + 1);
		if (pucSignerCert != NULL)
		{
			if (fread(pucSignerCert, 1, lSize, pstFile) == (size_t)lSize)
			{
				/* The crypto code requires the terminating zero to be part of the length so add 1 to the size. */
				*lSignerCertSize = lSize + 1;
                pucSignerCert[lSize] = '\0';
			}
			else
			{	/* There was a problem reading the certificate file so free the memory and abort. */
				free(pucSignerCert);
				pucSignerCert = NULL;
			}
		}
		else
		{
			/* Nothing special to do. */
		}
		fclose(pstFile);
	}
	return pucSignerCert;
}


/* Write a block of data to the specified file. */

static s16 prvWriteBlock(OTA_FileContext_t * const C, s32 iOffset, u8* const pacData, u32 iBlockSize)
{
	fseek(C->pstFile, iOffset, SEEK_SET);
	return (s16) fwrite(pacData, 1, iBlockSize, C->pstFile);
}

#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
#include "aws_ota_pal_internal.c"
#endif

#else
void dummy_function_to_avoid_empty_translation_unit_warning (void);
#endif /* INCLUDE_FROM_OTA_AGENT */
