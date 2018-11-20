/*
 * Amazon FreeRTOS Demo Bootloader V1.4.4
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
 * @file aws_boot_flash.c
 * @brief Boot flash implementation.
 */

/* Microchip includes.*/
#include <sys/kmem.h>
#include "system_config.h"
#include "system_definitions.h"
#include "peripheral/nvm/plib_nvm.h"
#include "aws_boot_nvm.h"

/* Bootloader includes.*/
#include "aws_boot_types.h"
#include "aws_boot_flash.h"
#include "aws_boot_loader.h"
#include "aws_boot_partition.h"
#include "aws_boot_flash_info.h"
#include "aws_boot_log.h"

/*-----------------------------------------------------------*/

BaseType_t BOOT_FLASH_Write( const uint32_t * pulAddress,
                             const uint32_t * pulData,
                             int lLength )
{
    BaseType_t xReturn = pdFALSE;

    if( ( lLength  % AWS_NVM_QUAD_SIZE ) == 0 )
    {
        /* Use quad word write. */
        xReturn = AWS_NVM_QuadWordWrite( pulAddress, pulData, lLength / AWS_NVM_QUAD_SIZE );
    }

    return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t BOOT_FLASH_EraseHeader( const BOOTImageDescriptor_t * pxAppDescriptor )
{
    DEFINE_BOOT_METHOD_NAME( "BOOT_FLASH_EraseHeader" );

    BaseType_t xReturn = pdFALSE;

    BOOTImageDescriptor_t XEmptyDesc;
    memset( &XEmptyDesc, 0xff, sizeof( XEmptyDesc ) );

    if( memcmp( &XEmptyDesc, pxAppDescriptor, sizeof( XEmptyDesc ) ) != 0 )
    {
        memset( &XEmptyDesc, 0, sizeof( XEmptyDesc ) );

        if( memcmp( &XEmptyDesc, pxAppDescriptor, sizeof( XEmptyDesc ) ) != 0 )
        {
            if( !AWS_NVM_QuadWordWrite( pxAppDescriptor->xImageHeader.ulAlign,
                                        XEmptyDesc.xImageHeader.ulAlign,
                                        sizeof( XEmptyDesc ) / AWS_NVM_QUAD_SIZE ) )
            {
                BOOT_LOG_L1( "[%s] Failed to erase descriptor 0x%08x\r\n",
                             BOOT_METHOD_NAME,
                             pxAppDescriptor );
                xReturn = pdFALSE;
            }
            else
            {
                BOOT_LOG_L3( "[%s] Erased descriptor 0x%08x\r\n",
                             BOOT_METHOD_NAME,
                             pxAppDescriptor );
                xReturn = pdTRUE;
            }
        }
        else
        {
            BOOT_LOG_L3( "[%s] Skipped erase descriptor 0x%08x\r\n",
                         BOOT_METHOD_NAME,
                         pxAppDescriptor );
        }

        xReturn = pdTRUE;
    }

    BOOT_LOG_L3( "[%s] Descriptor already erased: 0x%08x\r\n",
                 BOOT_METHOD_NAME,
                 pxAppDescriptor );

    return xReturn;
}

/*-----------------------------------------------------------*/

BaseType_t BOOT_FLASH_EraseBank( const BOOTImageDescriptor_t * pxAppDescriptor )
{
    DEFINE_BOOT_METHOD_NAME( "BOOT_FLASH_EraseBank" );
    
    BaseType_t xReturn = pdFALSE;
    uint8_t ucFlashArea;

    ucFlashArea = BOOT_FLASH_GetFlashArea( pxAppDescriptor );

    if( ucFlashArea == FLASH_PARTITION_IMAGE_0 )
    {
        xReturn = AWS_FlashErase( LOWER_FLASH_REGION_ERASE_OPERATION );
    }

    if( ucFlashArea == FLASH_PARTITION_IMAGE_1 )
    {
        xReturn = AWS_FlashErase( UPPER_FLASH_REGION_ERASE_OPERATION );
    }
    
    if(xReturn == pdTRUE)
    {
        BOOT_LOG_L2( "[%s] Bank erased at : 0x%08x\r\n",BOOT_METHOD_NAME, pxAppDescriptor);
    }
    else
    {
        BOOT_LOG_L2( "[%s] Bank erase failed at : 0x%08x\r\n",BOOT_METHOD_NAME, pxAppDescriptor); 
    }

    return xReturn;
}

/*-----------------------------------------------------------*/

BaseType_t BOOT_FLASH_ValidateAddress( const BOOTImageDescriptor_t * pxAppDescriptor )
{
    DEFINE_BOOT_METHOD_NAME( "BOOT_FLASH_ValidateAddress" );

    BaseType_t xReturn = pdFALSE;

    /* Application start address needs to be >= than this limit. */
    const void * const pvStartAddressLimit = ( const void * ) ( FLASH_DEVICE_BASE + FLASH_PARTITION_OFFSET_IMAGE_0 + sizeof( BOOTImageHeader_t ) );

    /* End address needs to be < than this limit.*/
    const void * const pvEndAddressLimit = ( const void * ) ( FLASH_DEVICE_BASE + FLASH_IMAGE_SIZE_MAX );

    /* Validate the start address.*/
    xReturn = ( ( pxAppDescriptor->pvStartAddress >= pvStartAddressLimit ) &&
                ( pxAppDescriptor->pvStartAddress < pvEndAddressLimit ) );

    BOOT_LOG_L3( "[%s] Start Address: 0x%08x\r\n",
                 BOOT_METHOD_NAME,
                 pxAppDescriptor->pvStartAddress );

    /* Validate the end address.*/
    xReturn = xReturn && ( ( pxAppDescriptor->pvEndAddress > pvStartAddressLimit ) &&
                           ( pxAppDescriptor->pvEndAddress <= pvEndAddressLimit ) );

    BOOT_LOG_L3( "[%s] End Address: 0x%08x\r\n",
                 BOOT_METHOD_NAME,
                 pxAppDescriptor->pvEndAddress );


    /* Validate the execution address.*/
    xReturn = xReturn && ( ( pxAppDescriptor->pvExecAddress >= pxAppDescriptor->pvStartAddress ) &&
                           ( pxAppDescriptor->pvExecAddress < pxAppDescriptor->pvEndAddress ) );

    BOOT_LOG_L3( "[%s] Exe Address: 0x%08x\r\n",
                 BOOT_METHOD_NAME,
                 pxAppDescriptor->pvExecAddress );
    
     /* Validate the start address is less than end address.*/
    xReturn = xReturn &&  ( pxAppDescriptor->pvStartAddress < pxAppDescriptor->pvEndAddress );

    return xReturn;
}
