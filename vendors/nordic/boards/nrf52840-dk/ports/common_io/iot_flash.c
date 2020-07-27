/*
 * FreeRTOS Common IO V0.1.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file iot_flash.c
 * @brief HAL Flash implementation on NRF52840 Development Kit
 */

/*
 * TODO:
 *       - Need mech for user to specify application base addr for flash. Tests use: ultestIotFlashStartOffset
 *       - Verify each function has appropriate logging
 *       - Test with soft-device disabled
 */

#include "FreeRTOS.h"
#include "task.h"

/* Nordic Board includes. */
#ifdef SOFTDEVICE_PRESENT
    #include "nrf_soc.h"
    #include "nrf_sdh.h"
    #include "nrf_sdm.h"
    #include "nrf_mbr.h"
    #include "nrf_sdh_ble.h"
    #include "nrf_fstorage_sd.h"
#else
    #include "nrf_drv_clock.h"
    #include "nrf_fstorage_nvmc.h"
#endif
#include "nrf52840_peripherals.h"

/* Main includes. */
#include "iot_config.h"
#include "iot_flash.h"
#include "iot_flash_config.h"

#ifdef IOT_LOG_LEVEL_GLOBAL
    #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
#else
    #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_IOT_FLASH
#endif

#define LIBRARY_LOG_NAME         ( "IOT_FLASH" )
#include "iot_logging_setup.h"


/* Flash constants */
#define FLASH_PAGE_SIZE         0x1000     /* 4k bytes */
#define FLASH_SECTOR_SIZE       0x1000     /* 4k bytes */
#define FLASH_BLOCK_SIZE        0x1000     /* 4k bytes */
#define FLASH_SECTOR_MASK_4K    0x0FFF     /* 4k mask */

#define MAX_WR_SIZE             NRF_FSTORAGE_SD_MAX_WRITE_SIZE
#define FLASH_PADDING_BYTE      0xFF       /* NAND-flash default logic value */

static void prvFlashEventHandler( nrf_fstorage_evt_t * p_evt );

NRF_FSTORAGE_DEF( nrf_fstorage_t iot_fstorage ) =
{
    /* Set a handler for fstorage events. */
    .evt_handler = prvFlashEventHandler,
};

typedef enum
{
    IOT_FLASH_CLOSED = 0u,
    IOT_FLASH_OPENED = 1u
} IotFlashState_t;

/* @brief flash context */
typedef struct IotFlashDescriptor
{
    IotFlashInfo_t xFlashInfo;    /* flash info structure */
    IotFlashCallback_t xCallback; /* callback */
    void * pvUserContext;         /* user context to provide in callback */
    uint8_t ucState;
} IotFlashDescriptor_t;

#define DEFAULT_FLASH_DESC                          \
    ( IotFlashDescriptor_t )                        \
    {                                               \
        .xFlashInfo =                               \
        {                                           \
            .ulFlashSize       = 0,                 \
            .ulBlockSize       = FLASH_BLOCK_SIZE,  \
            .ulSectorSize      = FLASH_SECTOR_SIZE, \
            .ulPageSize        = FLASH_PAGE_SIZE,   \
            .ulLockSupportSize = 0,                 \
            .ucAsyncSupported  = 0,                 \
        },                                          \
        .xCallback = NULL,                          \
        .pvUserContext = NULL,                      \
        .ucState = IOT_FLASH_CLOSED                 \
    }

static IotFlashDescriptor_t xFlashDesc = DEFAULT_FLASH_DESC;

/*---------------------------------------------------------------------------------*
*                                Private Helpers                                  *
*---------------------------------------------------------------------------------*/
static inline uint32_t prvIsAligned4K( uint32_t ulAddr )
{
    return ( ( ulAddr ) & ( FLASH_SECTOR_MASK_4K ) ) == 0 ? 1 : 0;
}
static void prvWaitUntilFlashReady( nrf_fstorage_t const * p_fstorage )
{
    while( nrf_fstorage_is_busy( p_fstorage ) )
    {
        vTaskDelay( pdMS_TO_TICKS( 1 ) );
    }
}
static void prvFlashEventHandler( nrf_fstorage_evt_t * p_evt )
{
    IotFlashOperationStatus_t xStatus = IOT_FLASH_SUCCESS;

    if( p_evt->result != NRF_SUCCESS )
    {
        switch( p_evt->id )
        {
            case NRF_FSTORAGE_EVT_WRITE_RESULT:
                xStatus = IOT_FLASH_WRITE_FAILED;
                break;

            case NRF_FSTORAGE_EVT_ERASE_RESULT:
                xStatus = IOT_FLASH_ERASE_FAILED;
                break;

            default:
                break;
        }
    }

    if( xFlashDesc.xCallback != NULL )
    {
        xFlashDesc.xCallback( xStatus, xFlashDesc.pvUserContext );
    }
}

static void prvGetFlashInfo( void )
{
    uint32_t const bootloader_addr = NRF_UICR->NRFFW[ 0 ];
    uint32_t const page_sz = NRF_FICR->CODEPAGESIZE;
    uint32_t const code_sz = NRF_FICR->CODESIZE;
    uint32_t const total_flash = code_sz * page_sz;

    /*End of flash */
    uint32_t flash_end_addr = ( bootloader_addr != 0xFFFFFFFF ) ? bootloader_addr : ( total_flash );

    /* Set last valid address for MCU to write jsut below bootloader which allows */
    /* this instance to completely erase config storage as well */
    iot_fstorage.end_addr = bootloader_addr - 1;
    iot_fstorage.start_addr = IOT_COMMON_IO_FLASH_BASE_ADDRESS;

    IotLogDebug( "Available Flash: [0x%x, 0x%x]", iot_fstorage.start_addr, iot_fstorage.end_addr );
    IotLogDebug( "Total Flash: %d %s -- Bootloader @ 0x%x -- Flash End @ 0x%x",
                 ( total_flash % 1024 ) ? total_flash : total_flash / 1024,
                 ( total_flash % 1024 ) ? "B" : "KB",
                 bootloader_addr,
                 flash_end_addr );
}

bool prvIsOperableHandle( IotFlashHandle_t const pxFlashHandle )
{
    return ( pxFlashHandle != NULL ) && ( pxFlashHandle->ucState == IOT_FLASH_OPENED );
}

/* Shared reaction for blocking APIs*/
int32_t prvMemoryOpProcess( ret_code_t xReturnCode_NRF,
                            uint32_t ulAddress,
                            const char * op )
{
    int32_t lReturnCode = IOT_FLASH_INVALID_VALUE;
}

/*---------------------------------------------------------------------------------*
*                               API Implementation                                *
*---------------------------------------------------------------------------------*/
IotFlashHandle_t iot_flash_open( int32_t lFlashInstance )
{
    IotFlashHandle_t xHandle = NULL;

    if( ( lFlashInstance == 0 ) && ( xFlashDesc.ucState == IOT_FLASH_CLOSED ) )
    {
        /* Calculate flash addresses */
        prvGetFlashInfo();
        #ifdef SOFTDEVICE_PRESENT

            /* Initialize an fstorage instance using the nrf_fstorage_sd backend.
             * nrf_fstorage_sd uses the SoftDevice to write to flash. This implementation can safely be
             * used whenever there is a SoftDevice, regardless of its status (enabled/disabled). */
            nrf_fstorage_api_t * p_fs_api = &nrf_fstorage_sd;
        #else

            /* Initialize an fstorage instance using the nrf_fstorage_nvmc backend.
             * nrf_fstorage_nvmc uses the NVMC peripheral. This implementation can be used when the
             * SoftDevice is disabled or not present.
             *
             * Using this implementation when the SoftDevice is enabled results in a hardfault. */
            nrf_fstorage_api_t * p_fs_api = &nrf_fstorage_nvmc;
        #endif

        xFlashDesc = DEFAULT_FLASH_DESC;
        ret_code_t xReturnCode_NRF = nrf_fstorage_init( &iot_fstorage, p_fs_api, NULL );

        if( xReturnCode_NRF == NRF_SUCCESS )
        {
            xFlashDesc.xFlashInfo.ulFlashSize = iot_fstorage.end_addr - iot_fstorage.start_addr + 1;
            xFlashDesc.ucState = IOT_FLASH_OPENED;
            xHandle = &xFlashDesc;
        }
        else
        {
            IotLogError( "%s: Internal NRF error", __func__ );
        }
    }

    return xHandle;
}

IotFlashInfo_t * iot_flash_getinfo( IotFlashHandle_t const pxFlashHandle )
{
    IotFlashInfo_t * pxFlashInfo = NULL;

    if( prvIsOperableHandle( pxFlashHandle ) )
    {
        pxFlashInfo = &( xFlashDesc.xFlashInfo );
    }

    return pxFlashInfo;
}

void iot_flash_set_callback( IotFlashHandle_t const pxFlashHandle,
                             IotFlashCallback_t xCallback,
                             void * pvUserContext )
{
    if( prvIsOperableHandle( pxFlashHandle ) )
    {
        pxFlashHandle->xCallback = xCallback;
        pxFlashHandle->pvUserContext = pvUserContext;
    }
    else
    {
        IotLogWarn( "%s: Null or closed IotFlashHandle_t(0x%x)", __func__, pxFlashHandle );
    }
}

int32_t iot_flash_ioctl( IotFlashHandle_t const pxFlashHandle,
                         IotFlashIoctlRequest_t xRequest,
                         void * const pvBuffer )
{
    int32_t lReturnCode = IOT_FLASH_INVALID_VALUE;

    if( !prvIsOperableHandle( pxFlashHandle ) || ( pvBuffer == NULL ) )
    {
        lReturnCode = IOT_FLASH_INVALID_VALUE;
    }
    else
    {
        switch( xRequest )
        {
            case eGetFlashTxNoOfbytes:

                if( iot_fstorage.p_flash_info )
                {
                    memcpy( pvBuffer, &iot_fstorage.p_flash_info->program_unit, sizeof( uint32_t ) );
                    lReturnCode = IOT_FLASH_SUCCESS;
                }

                break;

            case eGetFlashRxNoOfbytes:
                lReturnCode = IOT_FLASH_SUCCESS;
                uint32_t min_read_size = 1u;
                memcpy( pvBuffer, &min_read_size, sizeof( uint32_t ) );
                break;

            default:
                lReturnCode = IOT_FLASH_FUNCTION_NOT_SUPPORTED;
                break;
        }
    }

    return lReturnCode;
}

int32_t iot_flash_erase_sectors( IotFlashHandle_t const pxFlashHandle,
                                 uint32_t ulStartAddress,
                                 size_t xSize )
{
    int32_t lReturnCode = IOT_FLASH_INVALID_VALUE;

    if( !prvIsOperableHandle( pxFlashHandle ) || !prvIsAligned4K( ulStartAddress ) || ( xSize % FLASH_PAGE_SIZE ) )
    {
        lReturnCode = IOT_FLASH_INVALID_VALUE;
    }
    else if( nrf_fstorage_is_busy( &iot_fstorage ) )
    {
        lReturnCode = IOT_FLASH_DEVICE_BUSY;
    }
    else if( xSize == 0 )
    {
        lReturnCode = IOT_FLASH_SUCCESS;
    }
    else
    {
        ret_code_t xReturnCode_NRF = nrf_fstorage_erase( &iot_fstorage, ulStartAddress, xSize / FLASH_PAGE_SIZE, NULL );

        switch( xReturnCode_NRF )
        {
            case NRF_SUCCESS:
                prvWaitUntilFlashReady( &iot_fstorage );
                lReturnCode = IOT_FLASH_SUCCESS;
                break;

            case NRF_ERROR_NO_MEM:
                IotLogError( "Erase: Insufficient memory to queue operation. Review sdk_config.h:FDS_OP_QUEUE_SIZE" );
                lReturnCode = IOT_FLASH_ERASE_FAILED;
                break;

            case NRF_ERROR_INVALID_ADDR:
                IotLogError( "Erase: Out-of-bounds access (0x%x)", ulStartAddress );
                lReturnCode = IOT_FLASH_INVALID_VALUE;
                break;

            default:
                lReturnCode = IOT_FLASH_INVALID_VALUE;
                break;
        }
    }

    return lReturnCode;
}


int32_t iot_flash_erase_chip( IotFlashHandle_t const pxFlashHandle )
{
    return IOT_FLASH_FUNCTION_NOT_SUPPORTED;
}


int32_t iot_flash_write_sync( IotFlashHandle_t const pxFlashHandle,
                              uint32_t ulAddress,
                              uint8_t * const pvBuffer,
                              size_t xBytes )
{
    int32_t lReturnCode = IOT_FLASH_INVALID_VALUE;

    if( !prvIsOperableHandle( pxFlashHandle ) || ( pvBuffer == NULL ) || ( xBytes > MAX_WR_SIZE ) )
    {
        lReturnCode = IOT_FLASH_INVALID_VALUE;
    }
    else if( nrf_fstorage_is_busy( &iot_fstorage ) )
    {
        lReturnCode = IOT_FLASH_DEVICE_BUSY;
    }
    else
    {
        /* Prepare data for flash write. Must be word-multiple (4 B). Padded if necessary.
         *
         * Note: User can use eGetFlashTxNoOfbytes to read then form a word-multiple write,
         *       and preserve any existing flash data that would otherwise be overwritten with pad-value
         */
        size_t write_size = 4 * ( ( xBytes + 3 ) / 4 ); /* pad size to multiple of a word */
        uint8_t write_buf[ write_size ] __attribute__( ( aligned( 4 ) ) );
        memcpy( write_buf, pvBuffer, xBytes );
        memset( write_buf + xBytes, FLASH_PADDING_BYTE, write_size - xBytes );

        /* Write and check status*/
        ret_code_t xReturnCode_NRF = nrf_fstorage_write( &iot_fstorage, ulAddress, write_buf, write_size, NULL );

        switch( xReturnCode_NRF )
        {
            case NRF_SUCCESS:
                prvWaitUntilFlashReady( &iot_fstorage );
                lReturnCode = IOT_FLASH_SUCCESS;
                break;

            case NRF_ERROR_NO_MEM:
                IotLogError( "Write: Insufficient memory to queue operation. Review sdk_config.h:FDS_OP_QUEUE_SIZE" );
                lReturnCode = IOT_FLASH_ERASE_FAILED;
                break;

            case NRF_ERROR_INVALID_ADDR:
                IotLogError( "Write: Out-of-bounds access (0x%x)", ulAddress );
                lReturnCode = IOT_FLASH_INVALID_VALUE;
                break;

            default:
                lReturnCode = IOT_FLASH_INVALID_VALUE;
                break;
        }
    }

    return lReturnCode;
}
int32_t iot_flash_read_sync( IotFlashHandle_t const pxFlashHandle,
                             uint32_t ulAddress,
                             uint8_t * const pvBuffer,
                             size_t xBytes )
{
    int32_t lReturnCode = IOT_FLASH_INVALID_VALUE;

    if( !prvIsOperableHandle( pxFlashHandle ) )
    {
        lReturnCode = IOT_FLASH_INVALID_VALUE;
    }
    else if( nrf_fstorage_is_busy( &iot_fstorage ) )
    {
        lReturnCode = IOT_FLASH_DEVICE_BUSY;
    }
    else
    {
        ret_code_t xReturnCode_NRF = nrf_fstorage_read( &iot_fstorage, ulAddress, pvBuffer, xBytes );

        switch( xReturnCode_NRF )
        {
            case NRF_SUCCESS:
                lReturnCode = IOT_FLASH_SUCCESS;
                break;

            case NRF_ERROR_NO_MEM:
                IotLogError( "Read: Insufficient memory to queue operation. Review sdk_config.h:FDS_OP_QUEUE_SIZE" );
                lReturnCode = IOT_FLASH_ERASE_FAILED;
                break;

            case NRF_ERROR_INVALID_ADDR:
                IotLogError( "Read: Out-of-bounds access (0x%x)", ulAddress );
                lReturnCode = IOT_FLASH_INVALID_VALUE;
                break;

            default:
                lReturnCode = IOT_FLASH_INVALID_VALUE;
                break;
        }
    }

    return lReturnCode;
}

/* If a soft-device is present nrf_fstorage_sd is used, else nrf_fstorage_nvmc is used.
 * While nrf_fstorage_sd does provide proper async functionality, nrf_fstorage_nvmc does not.
 * See nrf_fstorage_nvmc:write(...)
 */
int32_t iot_flash_write_async( IotFlashHandle_t const pxFlashHandle,
                               uint32_t ulAddress,
                               uint8_t * const pvBuffer,
                               size_t xBytes )
{
    return IOT_FLASH_FUNCTION_NOT_SUPPORTED;
}

int32_t iot_flash_read_async( IotFlashHandle_t const pxFlashHandle,
                              uint32_t ulAddress,
                              uint8_t * const pvBuffer,
                              size_t xBytes )
{
    return IOT_FLASH_FUNCTION_NOT_SUPPORTED;
}

int32_t iot_flash_close( IotFlashHandle_t const pxFlashHandle )
{
    int32_t lReturnCode = IOT_FLASH_INVALID_VALUE;

    if( prvIsOperableHandle( pxFlashHandle ) )
    {
        if( NRF_SUCCESS == nrf_fstorage_uninit( &iot_fstorage, NULL ) )
        {
            *pxFlashHandle = DEFAULT_FLASH_DESC;
            lReturnCode = IOT_FLASH_SUCCESS;
        }
        else
        {
            IotLogError( "Internal NRF error. Can not close handle" );
        }
    }

    return lReturnCode;
}
