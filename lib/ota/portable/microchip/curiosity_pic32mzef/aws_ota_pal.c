/*
Amazon FreeRTOS OTA PAL for Curiosity PIC32MZEF V0.9.0
Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

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

/* OTA PAL implementation for Microchip PIC32MZ platform. */

#ifdef INCLUDE_FROM_OTA_AGENT
#include <sys/kmem.h>

#include "aws_nvm.h"
#include "aws_crypto.h"
#include "aws_codesign_keys.h"

#include "system/reset/sys_reset.h"

/* Specify the OTA signature algorithm we support on this platform. */
static const char acOTA_JSON_FileSignatureKey[] = "sig-sha256-ecdsa";

// definitions shared with the resident bootloader 
#define AWS_BOOT_IMAGE_SIGNATURE  "@MCHP_@"

// the current running image is always in the lower flash bank
#define     AWS_FLASH_LOWER_BANK_START  (__KSEG0_PROGRAM_MEM_BASE)

// the new image is always programmed in the upper flash bank
#define     AWS_FLASH_UPPER_BANK_START  (__KSEG0_PROGRAM_MEM_BASE + __KSEG0_PROGRAM_MEM_LENGTH / 2)
#define     AWS_FLASH_IMAGE_START       (AWS_FLASH_UPPER_BANK_START + sizeof(AWS_BOOT_IMAGE_DCPT))
#define     AWS_IMAGE_MAX_SIZE          (__KSEG0_PROGRAM_MEM_LENGTH / 2 - sizeof(AWS_BOOT_IMAGE_DCPT))

// the new image is always built in the lower flash bank
#define     AWS_IMAGE_EXEC_START        (__KSEG0_PROGRAM_MEM_BASE + sizeof(AWS_BOOT_IMAGE_DCPT))

// the new image execution: assume it always starts at the lowest build address
#define     AWS_IMAGE_EXEC_ADDRESS      (__KSEG0_PROGRAM_MEM_BASE + sizeof(AWS_BOOT_IMAGE_DCPT))



#define AWS_BOOT_CRC_SEED           0xffffffff


typedef enum
{
    AWS_BOOT_FLAG_IMG_NEW    = 0xff,    // indicates that this is a a brand new image, never run before
                                        // it can be run just once
    AWS_BOOT_FLAG_IMG_TEST   = 0xfe,    // indicates that this has been launched in the test run
    AWS_BOOT_FLAG_IMG_VALID  = 0xfc,    // the test run validated the image; this can be run normally


}AWS_BOOT_IMAGE_FLAGS;

typedef union 
{
    uint32_t    align[2];
    struct
    {
        char    img_sign[7];    // signature identifying a valid application: AWS_BOOT_IMAGE_SIGNATURE
        uint8_t img_flags;      // a AWS_BOOT_IMAGE_FLAGS value 
    };
}AWS_BOOT_IMG_HEADER;


// boot application image descriptor
// this is the descriptor used by the bootloader
// to maintain the application images
typedef struct
{

    AWS_BOOT_IMG_HEADER header;         // header identifying a valid application: signature (AWS_BOOT_IMAGE_SIGNATURE) + flags
    uint32_t            version;        // version of the image. The higher, the newer
    void*               start_address;  // image start address
    void*               end_address;    // image end address
    void*               entry_address;  // execution start address
    uint32_t            crc;            // binary image CRC
    uint32_t            reserved;
}AWS_BOOT_IMAGE_DCPT;


typedef struct
{
    const OTA_FileContext_t*    currOtaFile;        // current OTA file to be processed
    uint32_t                    low_image_offset;   // lowest offset/address in the application image
    uint32_t                    high_image_offset;  // highest offset/address in the application image
}AWS_OTA_OPER_DCPT;


static AWS_OTA_OPER_DCPT    aws_ota_dcpt;       // current OTA operation in progress
static AWS_OTA_OPER_DCPT*   pOtaDcpt = 0;       // pointer to current OTA operation

static __inline__ bool OTA_ContextValidate(OTA_FileContext_t *C)
{
	return (pOtaDcpt != 0 && pOtaDcpt->currOtaFile == C && C->pucFile == (u8*)pOtaDcpt);
}

static __inline__ void OTA_ContextClose(OTA_FileContext_t *C)
{
    C->pucFile = 0;
    aws_ota_dcpt.currOtaFile = 0;
    pOtaDcpt = 0;
}

static bool OTA_ContextUpdateImageHeader(OTA_FileContext_t *C)
{
    AWS_BOOT_IMAGE_DCPT imgDcpt;

    const AWS_BOOT_IMAGE_DCPT*  app_img_low_ptr = (const AWS_BOOT_IMAGE_DCPT*)(AWS_FLASH_LOWER_BANK_START);

    memcpy(imgDcpt.header.img_sign, AWS_BOOT_IMAGE_SIGNATURE, sizeof(imgDcpt.header.img_sign));
    imgDcpt.header.img_flags = AWS_BOOT_FLAG_IMG_NEW;
    imgDcpt.version = app_img_low_ptr->version + 1;
    if(imgDcpt.version == 0)
    {
        imgDcpt.version++;
    }
    imgDcpt.start_address = (void*)(AWS_IMAGE_EXEC_START + pOtaDcpt->low_image_offset);
    imgDcpt.end_address = (void*)(AWS_IMAGE_EXEC_START + pOtaDcpt->high_image_offset);
    imgDcpt.entry_address = (void*)(AWS_IMAGE_EXEC_ADDRESS + pOtaDcpt->low_image_offset);
    
    // calculate the CRC of the just programmed flash
    const uint8_t* flash_address = (const uint8_t*)KVA0_TO_KVA1(AWS_FLASH_IMAGE_START + pOtaDcpt->low_image_offset);
    AWS_BootCrcInit(AWS_BOOT_CRC_SEED);
    AWS_BootCrcAddBuffer(flash_address, (uint8_t*)imgDcpt.end_address - (uint8_t*)imgDcpt.start_address);
    imgDcpt.crc = AWS_BootCrcResult();

    // pointer to the app descriptor in the flash upper page
    const AWS_BOOT_IMAGE_DCPT*  app_img_up_ptr = (const AWS_BOOT_IMAGE_DCPT*)(AWS_FLASH_UPPER_BANK_START);
    bool flashRes = AWS_FlashProgramBlock((const uint8_t*)app_img_up_ptr, (const uint8_t*)&imgDcpt, sizeof(imgDcpt));

    OTA_PRINT("[OTA-MCHP] Image - version: %d\r\n", imgDcpt.version);
    OTA_PRINT("[OTA-MCHP] Image - start: 0x%08x, end: 0x%08x\r\n", imgDcpt.start_address, imgDcpt.end_address);
    OTA_PRINT("[OTA-MCHP] Image - crc: 0x%08x, end-start: %d\r\n", imgDcpt.crc, (uint8_t*)imgDcpt.end_address - (uint8_t*)imgDcpt.start_address);

    return flashRes;
}

/* Attempt to create a new receive file to write the file chunks to as they come in. */

static bool_t prvCreateFileForRx(OTA_FileContext_t *C)
{

    if(pOtaDcpt != 0 || aws_ota_dcpt.currOtaFile != 0)
    {
        OTA_PRINT("[OTA-MCHP] Create - Failed to start operation: already active!\r\n");
        return pdFALSE;
    }

    // program this new file in the upper flash bank
    if(!AWS_FlashErase())
    {
        OTA_PRINT("[OTA-MCHP] Create - Failed to erase the flash!\r\n");
        return pdFALSE;
    }


    pOtaDcpt = &aws_ota_dcpt;
    pOtaDcpt->currOtaFile = C;
    pOtaDcpt->low_image_offset = AWS_IMAGE_MAX_SIZE;
    pOtaDcpt->high_image_offset = 0;

    C->pucFile = (u8*)pOtaDcpt;
    
    OTA_PRINT("[OTA-MCHP] Create - Erased the flash OK\r\n");

	return pdTRUE;

}


/* Abort access to an existing open file. This is only valid after a job starts successfully. */

static OTA_Err_t prvAbort(OTA_FileContext_t *C)
{
	/* Check for null file handle since we may call this before a file is actually opened. */
    //
	if (OTA_ContextValidate(C))
	{
        OTA_ContextClose(C);
        OTA_PRINT("[OTA-MCHP] Abort - OK\r\n");
        return kOTA_Err_None;
    }

	return kOTA_Err_FileAbort;
}

/* Write a block of data to the specified file.
 * Returns the number of bytes written on success or negative error code.
 */

int16_t prvWriteBlock(OTA_FileContext_t *C, int32_t iOffset, uint8_t* pacData, uint32_t iBlockSize)
{
    if(!OTA_ContextValidate(C))
    {
        return -1;
    }

    if(iOffset < 0 || (iOffset + iBlockSize) > AWS_IMAGE_MAX_SIZE)
    {   // invalid address range
        return -2;
    }
  
    // update the image offsets
    if(iOffset < pOtaDcpt->low_image_offset)
    {
        pOtaDcpt->low_image_offset = iOffset;
    }
    if(iOffset + iBlockSize > pOtaDcpt->high_image_offset)
    {
        pOtaDcpt->high_image_offset = iOffset + iBlockSize;
    }

    const uint8_t* flash_address = (const uint8_t*)(AWS_FLASH_IMAGE_START + iOffset);

    if(!AWS_FlashProgramBlock(flash_address, pacData, iBlockSize))
    {
        return -3;   // programming error
    }

    // all good
    return iBlockSize;
}

/* Close the specified file. This will also authenticate the file if it is marked as secure. */

static OTA_Err_t prvCloseFile(OTA_FileContext_t *C)
{
	OTA_Err_t result;


	if (!OTA_ContextValidate(C))
    {
        return kOTA_Err_FileClose;
    }

	OTA_PRINT("[OTA-MCHP] Authenticating and closing file.\r\n");

	if (C->pacSignature != NULL)
	{
		/* Verify the file signature, close the file and return the signature verification result. */
		result = prvCheckFileSignature (C);
	}
	else
	{
		result = kOTA_Err_SignatureCheckFailed;
	}

	if (result == kOTA_Err_None)
	{   // update the image header
		OTA_PRINT ("[OTA-MCHP] %s signature verification passed.\r\n", acOTA_JSON_FileSignatureKey);
        if(OTA_ContextUpdateImageHeader(C))
        {
            OTA_PRINT ("[OTA-MCHP] image header updated.\r\n");
        }
        else
        {
            OTA_PRINT ("[OTA-MCHP] ERROR: Failed to update the image header.\r\n");
            result = kOTA_Err_FileClose;
        }
	}
	else
	{
		OTA_PRINT ("[OTA-MCHP] ERROR: Failed to pass %s signature verification: %d.\r\n", acOTA_JSON_FileSignatureKey, result);
	}

    OTA_ContextClose(C);
	return result;
}

static OTA_Err_t prvCheckFileSignature(OTA_FileContext_t * const C)
{
    OTA_Err_t result;
    s32		lSignerCertSize;
    void	*pvSigVerifyContext;
    u8		*pucSignerCert = 0;

    while(true)
    {
        /* Verify an ECDSA-SHA256 signature. */
        if (CRYPTO_SignatureVerificationStart( &pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_ECDSA, cryptoHASH_ALGORITHM_SHA256) == pdFALSE)
        {
            result = kOTA_Err_SignatureCheckFailed;
            break;
        }

        OTA_PRINT("[OTA-MCHP] Started %s signature verification, file: %s\r\n", acOTA_JSON_FileSignatureKey, (const char*)C->pacCertFilepath);
        pucSignerCert = prvReadAndAssumeCertificate((const u8* const)C->pacCertFilepath, &lSignerCertSize);
        if (pucSignerCert == NULL)
        {
            result = kOTA_Err_BadSignerCert;
            break;
        }


        uint8_t* flash_address = (uint8_t*)KVA0_TO_KVA1(AWS_FLASH_IMAGE_START + pOtaDcpt->low_image_offset);
        CRYPTO_SignatureVerificationUpdate(pvSigVerifyContext, flash_address, pOtaDcpt->high_image_offset - pOtaDcpt->low_image_offset);

        if (CRYPTO_SignatureVerificationFinal(pvSigVerifyContext, (char*)pucSignerCert, lSignerCertSize, C->pacSignature, C->usSigSize) == pdFALSE)
        {
            result = kOTA_Err_SignatureCheckFailed;
        }
        else
        {
            result = kOTA_Err_None;
        }

        break;
    }

    /* Free the signer certificate that we now own after prvReadAndAssumeCertificate(). */
    if(pucSignerCert != 0)
    {
        vPortFree(pucSignerCert);
    }

    return result;

}

/* Read the specified signer certificate from the filesystem into a local buffer. The allocated
   memory becomes the property of the caller who is responsible for freeing it.
 */

static u8 * prvReadAndAssumeCertificate(const u8 * const pucCertName, s32 * const lSignerCertSize)
{
    uint8_t*    pCertData;
    uint32_t    certSize;
	u8		    *pucSignerCert = NULL;
    
    extern BaseType_t prvReadFile( char * pcFileName,
                               uint8_t ** ppucData,
                               uint32_t * pulDataSize );

    if(prvReadFile( pucCertName, &pCertData, &certSize) != pdTRUE)
    {   // use the back up "codesign_keys.h" file if the signing credentials haven't been saved in the device
        pCertData = (uint8_t*)signingcredentialSIGNING_CERTIFICATE_PEM;
        certSize = sizeof(signingcredentialSIGNING_CERTIFICATE_PEM);
        OTA_PRINT("[OTA-MCHP] Assume Cert - No such file: %s. Using header file\r\n", (const char*)pucCertName);
    }
    else
    {
        OTA_PRINT("[OTA-MCHP] Assume Cert - file: %s OK\r\n", (const char*)pucCertName);
    }

    /* Allocate memory for the signer certificate plus a terminating zero so we can load it and return to the caller. */
    pucSignerCert = pvPortMalloc(certSize + 1);
    if (pucSignerCert != NULL)
    {
        memcpy(pucSignerCert, pCertData, certSize);
        /* The crypto code requires the terminating zero to be part of the length so add 1 to the size. */
        pucSignerCert[certSize] = '\0';
        *lSignerCertSize = certSize + 1;
    }

	return pucSignerCert;
}


static OTA_Err_t prvActivateNewImage(void)
{
    /* Reset the MCU so we can test the new image. Short delay for debug log output. */
 	OTA_PRINT("[OTA-MCHP] Resetting MCU to activate new image.\r\n");
    vTaskDelay( kOTA_HalfSecondDelay );
    SYS_RESET_SoftwareReset();

    /* We shouldn't actually get here if the board supports the auto reset.
     * But, it doesn't hurt anything if we do although someone will need to
     * reset the device for the new image to boot. */
	return kOTA_Err_None;
}


/* Set the final state of the last transferred (final) OTA file.
 * Commit the image if the state == eOTA_ImageState_Accepted.
 */

static OTA_Err_t prvSetImageState (OTA_ImageState_t eState)
{
    AWS_BOOT_IMAGE_DCPT copy_dcpt;
	OTA_Err_t result = kOTA_Err_None;

	while (eState == eOTA_ImageState_Accepted)
    {
        const AWS_BOOT_IMAGE_DCPT*  app_img_ptr = (const AWS_BOOT_IMAGE_DCPT*)KVA0_TO_KVA1(AWS_FLASH_LOWER_BANK_START);
        copy_dcpt = *app_img_ptr;

        // this should be an image launched in the test run!
        if(copy_dcpt.header.img_flags == AWS_BOOT_FLAG_IMG_TEST)
        {   // mark the image as valid
            copy_dcpt.header.img_flags = AWS_BOOT_FLAG_IMG_VALID;

            if(AWS_NVM_QuadWordWrite(app_img_ptr->header.align, copy_dcpt.header.align, sizeof(copy_dcpt) / AWS_NVM_QUAD_SIZE))
            {
                OTA_PRINT("[OTA-MCHP] Accepted final image. Committed.\r\n");
            }
            else
            {
                OTA_PRINT("[OTA-MCHP] Commit failed (%d).\r\n", -3);
                result = kOTA_Err_CommitFailed | (-3 & kOTA_PAL_ErrMask);
            }
        }
        else
        {
            OTA_PRINT("[OTA-MCHP] Committing the wrong image? (0x%02x)\r\n", copy_dcpt.header.img_flags);
            result = copy_dcpt.header.img_flags == AWS_BOOT_FLAG_IMG_VALID ? kOTA_Err_None : kOTA_Err_CommitFailed | (-2 & kOTA_PAL_ErrMask);
        }

        break;
    }

	return result;
}


#else
void dummy_function_to_avoid_empty_translation_unit_warning (void);
#endif /* INCLUDE_FROM_OTA_AGENT */

