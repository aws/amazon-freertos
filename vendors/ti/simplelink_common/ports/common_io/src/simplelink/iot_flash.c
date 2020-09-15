/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    iot_flash.c
 * @brief   This file contains the definitions of flash APIs using TI drivers.
 */
#include <stdint.h>
#include <unistd.h>

/* Common IO Header */
#include <iot_flash.h>

/* TI Drivers */
#include <ti/drivers/NVS.h>

/* Driver config */
#include <ti_drivers_config.h>

/**
 * @brief TI Simplelink NVS HAL context abstracted in the Peripheral_Descriptor_t.
 *
 * Contains all the parameters needed for working with TI Simplelink NVS driver.
 */
typedef struct
{
    NVS_Handle tiNvsHandle;      /**< TI driver handle used with TI Driver API. */
    IotFlashInfo_t xFlashInfo;   /**< Flash info. */
    IotFlashStatus_t xStatus;    /**< Current Flash status. */
    uint32_t ulBytesWrittenLast; /**< Number of bytes written during the last operation. */
    uint32_t ulBytesReadLast;    /**< Number of bytes read during the last operation. */
} IotFlashDescriptor_t;

/**
 * @brief Static Flash descriptor table
 */
static IotFlashDescriptor_t xFlash[ CONFIG_TI_DRIVERS_NVS_COUNT ];

static bool xIsInitialized = false;

/*-----------------------------------------------------------*/

IotFlashHandle_t iot_flash_open( int32_t lFlashInstance )
{
    NVS_Handle xTiHandle = NULL;
    IotFlashHandle_t xHandle = NULL;

    if( false == xIsInitialized )
    {
        NVS_init();
        xIsInitialized = true;
    }

    NVS_Params xFlashParams;
    NVS_Params_init( &xFlashParams );
    xTiHandle = NVS_open( lFlashInstance, &xFlashParams );

    if( xTiHandle )
    {
        NVS_Attrs xRegionAttrs;
        NVS_getAttrs( xTiHandle, &xRegionAttrs );
        xFlash[ lFlashInstance ].xFlashInfo.ulFlashSize = xRegionAttrs.regionSize;

        /* We only got sector size no matter the NVS implementation used.
         * Use this value for block, sector and page */
        xFlash[ lFlashInstance ].xFlashInfo.ulBlockSize = xRegionAttrs.sectorSize;
        xFlash[ lFlashInstance ].xFlashInfo.ulSectorSize = xRegionAttrs.sectorSize;
        xFlash[ lFlashInstance ].xFlashInfo.ulPageSize = xRegionAttrs.sectorSize;

        /* The NVS driver do not support setting protection */
        xFlash[ lFlashInstance ].xFlashInfo.ulLockSupportSize = 0;
        /* Always operating in blocking mode */
        xFlash[ lFlashInstance ].xFlashInfo.ucAsyncSupported = 0;

        xFlash[ lFlashInstance ].tiNvsHandle = xTiHandle;
        xFlash[ lFlashInstance ].xStatus = eFlashIdle;
        xFlash[ lFlashInstance ].ulBytesWrittenLast = 0;
        xFlash[ lFlashInstance ].ulBytesReadLast = 0;

        xHandle = ( IotFlashHandle_t ) &xFlash[ lFlashInstance ];
    }

    return xHandle;
}

/*-----------------------------------------------------------*/

IotFlashInfo_t * iot_flash_getinfo( IotFlashHandle_t const pxFlashHandle )
{
    IotFlashInfo_t * xInfo = NULL;
    IotFlashDescriptor_t * pxFlashDesc = ( IotFlashDescriptor_t * ) pxFlashHandle;

    if( pxFlashDesc )
    {
        xInfo = &( pxFlashDesc->xFlashInfo );
    }

    return xInfo;
}

/*-----------------------------------------------------------*/

void iot_flash_set_callback( IotFlashHandle_t const pxFlashHandle,
                             IotFlashCallback_t xCallback,
                             void * pvUserContext )
{
    /* As async calls is not supported, this API has no effect. */
}

/*-----------------------------------------------------------*/

int32_t iot_flash_ioctl( IotFlashHandle_t const pxFlashHandle,
                         IotFlashIoctlRequest_t xRequest,
                         void * const pvBuffer )
{
    int32_t ret = IOT_FLASH_SUCCESS;
    IotFlashDescriptor_t * pxFlashDesc = ( IotFlashDescriptor_t * ) pxFlashHandle;

    if( ( NULL == pxFlashDesc ) || ( NULL == pvBuffer ) )
    {
        ret = IOT_FLASH_INVALID_VALUE;
    }
    else
    {
        switch( xRequest )
        {
            case eGetFlashStatus:
                *( IotFlashStatus_t * ) pvBuffer = ( IotFlashStatus_t ) pxFlashDesc->xStatus;
                break;

            case eGetFlashTxNoOfbytes:
                *( uint32_t * ) pvBuffer = pxFlashDesc->ulBytesWrittenLast;
                break;

            case eGetFlashRxNoOfbytes:
                *( uint32_t * ) pvBuffer = pxFlashDesc->ulBytesReadLast;
                break;

            case eSetFlashBlockProtection:
            case eGetFlashBlockProtection:
            case eSuspendFlashProgramErase:
            case eResumeFlashProgramErase:
                ret = IOT_FLASH_FUNCTION_NOT_SUPPORTED;
                break;

            default:
                ret = IOT_FLASH_INVALID_VALUE;
                break;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_flash_erase_sectors( IotFlashHandle_t const pxFlashHandle,
                                 uint32_t ulStartAddress,
                                 size_t xSize )
{
    int32_t ret = IOT_FLASH_SUCCESS;
    IotFlashDescriptor_t * pxFlashDesc = ( IotFlashDescriptor_t * ) pxFlashHandle;

    if( NULL == pxFlashDesc )
    {
        ret = IOT_FLASH_INVALID_VALUE;
    }
    else
    {
        int16_t usStatus = NVS_erase( pxFlashDesc->tiNvsHandle, ulStartAddress, xSize );

        if( NVS_STATUS_SUCCESS != usStatus )
        {
            if( NVS_STATUS_ERROR == usStatus )
            {
                ret = IOT_FLASH_ERASE_FAILED;
            }
            else
            {
                ret = IOT_FLASH_INVALID_VALUE;
            }
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_flash_erase_chip( IotFlashHandle_t const pxFlashHandle )
{
    int32_t ret = IOT_FLASH_SUCCESS;
    IotFlashDescriptor_t * pxFlashDesc = ( IotFlashDescriptor_t * ) pxFlashHandle;

    if( NULL == pxFlashDesc )
    {
        ret = IOT_FLASH_INVALID_VALUE;
    }
    else
    {
        ret = iot_flash_erase_sectors( pxFlashHandle, 0, pxFlashDesc->xFlashInfo.ulFlashSize );
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_flash_write_sync( IotFlashHandle_t const pxFlashHandle,
                              uint32_t ulAddress,
                              uint8_t * const pvBuffer,
                              size_t xBytes )
{
    int32_t ret = IOT_FLASH_SUCCESS;
    IotFlashDescriptor_t * pxFlashDesc = ( IotFlashDescriptor_t * ) pxFlashHandle;

    if( ( NULL == pxFlashDesc ) || ( NULL == pvBuffer ) )
    {
        ret = IOT_FLASH_INVALID_VALUE;
    }
    else
    {
        /* There is no requirement to perform erase or verification
         * prior to the write operation. The user is assumed to have erased the
         * flash prior to writing it. Failing to do so would fail the write. */
        uint16_t usStatus = NVS_write( pxFlashDesc->tiNvsHandle, ulAddress,
                                       pvBuffer, xBytes,
                                       NVS_WRITE_POST_VERIFY );

        if( NVS_STATUS_SUCCESS != usStatus )
        {
            if( NVS_STATUS_ERROR == usStatus )
            {
                ret = IOT_FLASH_WRITE_FAILED;
            }
            else
            {
                ret = IOT_FLASH_INVALID_VALUE;
            }
        }
        else
        {
            pxFlashDesc->ulBytesWrittenLast = xBytes;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_flash_read_sync( IotFlashHandle_t const pxFlashHandle,
                             uint32_t ulAddress,
                             uint8_t * const pvBuffer,
                             size_t xBytes )
{
    int32_t ret = IOT_FLASH_SUCCESS;
    IotFlashDescriptor_t * pxFlashDesc = ( IotFlashDescriptor_t * ) pxFlashHandle;

    if( ( NULL == pxFlashDesc ) || ( NULL == pvBuffer ) || ( ~0u == ulAddress ) )
    {
        ret = IOT_FLASH_INVALID_VALUE;
    }
    else
    {
        uint16_t usStatus = NVS_read( pxFlashDesc->tiNvsHandle, ulAddress,
                                      pvBuffer, xBytes );

        if( NVS_STATUS_SUCCESS != usStatus )
        {
            if( NVS_STATUS_ERROR == usStatus )
            {
                ret = IOT_FLASH_READ_FAILED;
            }
            else
            {
                ret = IOT_FLASH_INVALID_VALUE;
            }
        }
        else
        {
            pxFlashDesc->ulBytesReadLast = xBytes;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_flash_write_async( IotFlashHandle_t const pxFlashHandle,
                               uint32_t ulAddress,
                               uint8_t * const pvBuffer,
                               size_t xBytes )
{
    /* Only blocking operations are supported */
    return IOT_FLASH_FUNCTION_NOT_SUPPORTED;
}

/*-----------------------------------------------------------*/

int32_t iot_flash_read_async( IotFlashHandle_t const pxFlashHandle,
                              uint32_t ulAddress,
                              uint8_t * const pvBuffer,
                              size_t xBytes )
{
    /* Only blocking operations are supported */
    return IOT_FLASH_FUNCTION_NOT_SUPPORTED;
}

/*-----------------------------------------------------------*/

int32_t iot_flash_close( IotFlashHandle_t const pxFlashHandle )
{
    int32_t ret = IOT_FLASH_SUCCESS;
    IotFlashDescriptor_t * pxFlashDesc = ( IotFlashDescriptor_t * ) pxFlashHandle;

    if( ( NULL == pxFlashDesc ) || ( NULL == pxFlashDesc->tiNvsHandle ) )
    {
        ret = IOT_FLASH_INVALID_VALUE;
    }
    else
    {
        NVS_close( pxFlashDesc->tiNvsHandle );
        pxFlashDesc->tiNvsHandle = NULL;
    }

    return ret;
}
