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


/* Bootloader includes.*/
#include "aws_boot_partition.h"
#include "aws_boot_types.h"
#include "aws_boot_flash_info.h"

/* Default partition table for PIC32MZ2048EFM100.
 * The device support two memory banks which can be toggled.
 */
static FLASHPartition_t axPartitonTable[] =
{
    {
        .ulMagic = FLASH_PARTITION_MAGIC_CODE,
        .ucPartitionLabel = "OTA Image 0",
        .ucPartitionType = FLASH_PARTITION_TYPE_APP,
        .ucPartitionSubType = ( FLASH_PARTITION_SUBTYPE_APP_OTA | FLASH_PARTITION_SUBTYPE_APP_DEFAULT ),
        .xFlashArea =
        {
            .ucFlashAreaId   = FLASH_PARTITION_IMAGE_0,
            .ucFlashDeviceId = FLASH_DEVICE_INTERNAL,
            .ulOffset        = FLASH_PARTITION_OFFSET_IMAGE_0,
            .ulSize          = FLASH_IMAGE_SIZE_MAX,
        },
        .ulFlags = 0xFFFFFFFF,
    },
    {
        .ulMagic = FLASH_PARTITION_MAGIC_CODE,
        .ucPartitionLabel = "OTA Image 1",
        .ucPartitionType = FLASH_PARTITION_TYPE_APP,
        .ucPartitionSubType = FLASH_PARTITION_SUBTYPE_APP_OTA,
        .xFlashArea =
        {
            .ucFlashAreaId   = FLASH_PARTITION_IMAGE_1,
            .ucFlashDeviceId = FLASH_DEVICE_INTERNAL,
            .ulOffset        = FLASH_PARTITION_OFFSET_IMAGE_1,
            .ulSize          = FLASH_IMAGE_SIZE_MAX,
        },
        .ulFlags = 0xFFFFFFFF,
    }
};

/*-----------------------------------------------------------*/

uint8_t BOOT_FLASH_GetFlashArea( const BOOTImageDescriptor_t * pxAppDesc )
{
    uint8_t ucIndex;
    uint8_t ucFlashArea = 0xFF;

    void * pvAppDesc = ( void * ) pxAppDesc;

    for( ucIndex = 0; ucIndex < ( sizeof( axPartitonTable ) / sizeof( FLASHPartition_t ) ); ucIndex++ )
    {
        if( axPartitonTable[ ucIndex ].ucPartitionType == FLASH_PARTITION_TYPE_APP )
        {
            if( pvAppDesc == ( void * ) ( FLASH_DEVICE_BASE + axPartitonTable[ ucIndex ].xFlashArea.ulOffset ) )
            {
                ucFlashArea = axPartitonTable[ ucIndex ].xFlashArea.ucFlashAreaId;
                break;
            }
        }
    }

    return ucFlashArea;
}

/*-----------------------------------------------------------*/

BaseType_t BOOT_FLASH_ReadPartitionTable( BOOTPartition_Info_t * xPartitionInfo )
{
    uint8_t i;

    xPartitionInfo->ucNumOfApps = 0;
    
    for( i = 0; i < ( sizeof( axPartitonTable ) / sizeof( FLASHPartition_t ) ); i++ )
    {
        switch( axPartitonTable[ i ].ucPartitionType )
        {
            case FLASH_PARTITION_TYPE_APP:
                xPartitionInfo->ucNumOfApps++;

                if( axPartitonTable[ i ].ucPartitionSubType & FLASH_PARTITION_SUBTYPE_APP_OTA )
                {
                    xPartitionInfo->paxOTAAppDescriptor[ i ] =
                        ( void * ) ( FLASH_DEVICE_BASE + axPartitonTable[ i ].xFlashArea.ulOffset );
                }

                if( axPartitonTable[ i ].ucPartitionSubType & FLASH_PARTITION_SUBTYPE_APP_DEFAULT )
                {
                    xPartitionInfo->pvDefaultAppExecAddress =
                        ( void * ) ( FLASH_DEVICE_BASE + axPartitonTable[ i ].xFlashArea.ulOffset + sizeof( BOOTImageDescriptor_t ) );
                }

                break;

            case FLASH_PARTITION_TYPE_DATA:
                /* Not used.*/
                break;

            default:

                /* Incorrect partition type.*/
                return pdFALSE;
        }
    }

    return pdTRUE;
}
