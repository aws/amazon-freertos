/*
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

/* AWS Flash and Boot configuration for Microchip SAME70 platform. */


#ifndef _AWS_FLASH_CONFIG_H_
#define _AWS_FLASH_CONFIG_H_

#include <stdint.h>
#include <stdbool.h>

#include "aws_nvm.h" 

// flash section where the certificates are stored
// reserve the last lock region (AWS_NVM_LOCK_REGION_PAGES/16 KB) at the end of the flash
// this is the last AWS_NVM_LOCK_REGION_PAGES/32 flash pages (512 Bytes in size each) at the end of the flash
// keep it always page aligned and multiple of AWS_NVM_ERASE_PAGES_MIN!

#define PKCS11_CERTIFICATE_FLASH_PAGES              (32)    
#define PKCS11_CERTIFICATE_SECTION_SIZE             (IFLASH_PAGE_SIZE * PKCS11_CERTIFICATE_FLASH_PAGES)    
#define PKCS11_CERTIFICATE_SECTION_START_ADDRESS    (IFLASH_ADDR + IFLASH_SIZE - PKCS11_CERTIFICATE_SECTION_SIZE )    

// MPU region to use for the PKCS11 certificates
// Note: regions 0 - 11 are used by default in the system_core_mpu.c as part of
// system_startup.c ARCH_CORE_MPU_Initialize() call.
#define PKCS11_CERTIFICATE_FLASH_MPU_REGION         12



// definitions shared with the resident bootloader 

#define AWS_BOOTLOADER_VERSION    0x0100    // Bootloader version: major.minor

#define AWS_BOOT_IMAGE_SIGNATURE  "@MCHP_@"

typedef enum
{
    AWS_BOOT_FLAG_IMG_UNKNOWN   = 0xff, // uninitialized/invalid image
    AWS_BOOT_FLAG_IMG_NEW       = 0xfe, // indicates that this is a a brand new image, never run before
                                        // it can be run just once
    AWS_BOOT_FLAG_IMG_TEST      = 0xf8, // indicates that this has been launched in the test run
    AWS_BOOT_FLAG_IMG_VALID     = 0xf0, // the test run validated the image; this can be run normally
    AWS_BOOT_FLAG_IMG_INVALID   = 0x00,  // the test run invalidated the image


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

#define SIZEOF_AWS_BOOT_IMAGE_DCPT      AWS_NVM_PAGE_SIZE    //20 // preprocessor doesn't know sizeof()

// size of the AWS bootloader
// NOTE: always 32 pages (16 KB) to be multiple of the lock region size!
#define AWS_BOOTLOADER_16KB_UNITS       4       // 64 KB
#define AWS_BOOTLOADER_SIZE     (AWS_BOOTLOADER_16KB_UNITS * 16 * 1024)

// start of the current running image descriptor
#define AWS_FLASH_RUN_DCPT_START        (IFLASH_ADDR + AWS_BOOTLOADER_SIZE)

// the bootloader size also enforces the start of the application descriptor to be 16 pages (8KB) aligned
// (16 pages is the minimum size of flash that can be independently erased on this part)
#if ((AWS_FLASH_RUN_DCPT_START & ((1U << 14) - 1)) != 0)
#error "AWS bootloader size should be multiple of 16 KB!"
#endif

// space for the AWS_BOOT_IMAGE_DCPT at the beginning of the running image
// keep it in its own 512 bytes page so it's processed independently
// from the app image itself
#define AWS_IMAGE_RUN_DCPT_SIZE         IFLASH_PAGE_SIZE         

// the current running image is always in the lower part of the flash
// there is only one flash bank on E70!
#define AWS_FLASH_RUN_APP_START  (AWS_FLASH_RUN_DCPT_START + AWS_IMAGE_RUN_DCPT_SIZE)

// it should be already properly 512 bytes aligned
// (IVT alignment requirement: 16 handlers + 71 irqs = 87 words = 348 bytes < 2 ^ 9)
// but just in case, extra sanity check
#if ((AWS_FLASH_RUN_APP_START & ((1U << 9) - 1)) != 0)
#error "Application flash image should be 512 bytes aligned!"
#endif

// calculate the maximum size of an application
// Note that the new application and save application images
// do NOT need to be 512 B aligned since they will be moved anyway
// for execution
// But the beginning of the new/save app descriptor needs to be 16 pages/8KB aligned
// so it can be erased/programmed by itself

// how many sections we divide in flash
//  2 if only the Run App and New App + their DCPTs
//  3 if the Save App and DCPT is also needed.
//      This should be the default since there's no way to revert back to the old Run App
//      if the New App doesn't pass the run-time test!
//      However, this makes the image size smaller!
#define AWS_FLASH_SECTIONS      3

// constant for truncating/rounding up to 8KB
#define AWS_FLASH_ROUND_UP_FACTOR      ((8 * 1024) / 2)

// end of the flash available to these apps
#define AWS_FLASH_END_ADDR      (IFLASH_ADDR + IFLASH_SIZE - PKCS11_CERTIFICATE_SECTION_SIZE)    

// all available size for the application
#if (AWS_FLASH_SECTIONS == 2)
#define AWS_IMAGES_AVAILABLE_SIZE    ((AWS_FLASH_END_ADDR - AWS_FLASH_RUN_APP_START - SIZEOF_AWS_BOOT_IMAGE_DCPT)) / 2
#elif (AWS_FLASH_SECTIONS == 3)
#define AWS_IMAGES_AVAILABLE_SIZE    ((AWS_FLASH_END_ADDR - AWS_FLASH_RUN_APP_START - 2 * SIZEOF_AWS_BOOT_IMAGE_DCPT)) / 3
#else
#error "Only 2 or 3 Flash sections are currently supported!"
#endif

#if ((AWS_IMAGES_AVAILABLE_SIZE & ((1U << 14))) != 0)
#error "AWS Available size should be multiple of 16 KB!"
#endif


// location where the new application descriptor starts
// 8 KB aligned
#define AWS_FLASH_NEW_DCPT_START    ((AWS_FLASH_RUN_APP_START + AWS_IMAGES_AVAILABLE_SIZE + AWS_FLASH_ROUND_UP_FACTOR) & (~((1U << 13) - 1)))

// location where the run application ends
#define AWS_FLASH_RUN_APP_END       (AWS_FLASH_NEW_DCPT_START)

// location where the new application starts
#define AWS_FLASH_NEW_APP_START (AWS_FLASH_NEW_DCPT_START + SIZEOF_AWS_BOOT_IMAGE_DCPT)


#if (AWS_FLASH_SECTIONS == 3)
// location where the save application descriptor starts
// 8 KB aligned
#define AWS_FLASH_SAVE_DCPT_START    ((AWS_FLASH_NEW_DCPT_START + SIZEOF_AWS_BOOT_IMAGE_DCPT + AWS_IMAGES_AVAILABLE_SIZE + AWS_FLASH_ROUND_UP_FACTOR) & (~((1U << 13) - 1)))

// location where the new application ends
#define AWS_FLASH_NEW_APP_END  (AWS_FLASH_SAVE_DCPT_START)

// location where the save application starts 
#define AWS_FLASH_SAVE_APP_START    (AWS_FLASH_SAVE_DCPT_START +SIZEOF_AWS_BOOT_IMAGE_DCPT)

// location where the save application ends 
#define AWS_FLASH_SAVE_APP_END      (AWS_FLASH_END_ADDR)
#else
// location where the new application ends
#define AWS_FLASH_NEW_APP_END  (AWS_FLASH_END_ADDR)
#endif // (AWS_FLASH_SECTIONS == 3)

// Size of the Run App space after truncations:
#define AWS_FLASH_RUN_APP_SIZE   (AWS_FLASH_RUN_APP_END - AWS_FLASH_RUN_APP_START) 

// Size of the New App space after truncations:
#define AWS_FLASH_NEW_APP_SIZE  (AWS_FLASH_NEW_APP_END - AWS_FLASH_NEW_APP_START)

// Size of the Save App space after truncations:
#if (AWS_FLASH_SECTIONS == 3)
#define AWS_FLASH_SAVE_APP_SIZE  (AWS_FLASH_SAVE_APP_END - AWS_FLASH_SAVE_APP_START)

// one more sanity check: PKCS11 section selected so that the AWS_FLASH_END_ADDR/AWS_FLASH_SAVE_APP_END
// guarantee that AWS_FLASH_SAVE_APP_END (and the save app size) is multiple of 16 pages/8KB 
// (the AWS_FLASH_SAVE_DCPT_START is already multiple of 16 pages/8KB)
#if ((AWS_FLASH_SAVE_APP_END & ((1U << 13) - 1)) != 0)
#error "PKCS11 Certificate Section should be 8 KB aligned!"
#endif

#endif // (AWS_FLASH_SECTIONS == 3)

// finally, calculate the maximum space for an app
#if (AWS_FLASH_RUN_APP_SIZE > AWS_FLASH_NEW_APP_SIZE)
#define AWS_FLASH_RUN_NEW_SIZE  AWS_FLASH_NEW_APP_SIZE
#else
#define AWS_FLASH_RUN_NEW_SIZE  AWS_FLASH_RUN_APP_SIZE
#endif

#if (AWS_FLASH_SECTIONS == 3)
#if (AWS_FLASH_RUN_NEW_SIZE > AWS_FLASH_SAVE_APP_SIZE)
#define AWS_FLASH_RUN_NEW_SAVE_SIZE  AWS_FLASH_SAVE_APP_SIZE
#else
#define AWS_FLASH_RUN_NEW_SAVE_SIZE  AWS_FLASH_RUN_NEW_SIZE
#endif
#define AWS_IMAGE_MAX_SIZE          AWS_FLASH_RUN_NEW_SAVE_SIZE
#else
#define AWS_IMAGE_MAX_SIZE          AWS_FLASH_RUN_NEW_SIZE
#endif



#endif // _AWS_FLASH_CONFIG_H_

