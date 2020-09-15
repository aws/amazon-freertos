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
 * @file    iot_uart.c
 * @brief   This file contains the definitions of UART APIs using TI drivers.
 *          The implementation does not support partial return.
 */
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

/* Common IO Header */
#include <iot_uart.h>

/* TI Drivers and DPL */
#include <ti/drivers/UART.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#if ( defined DeviceFamily_CC13X2 ) || ( defined DeviceFamily_CC26X2 ) || ( defined DeviceFamily_CC13X2_CC26X2 )
#include <ti/drivers/uart/UARTCC26XX.h>
#endif

/* Driver config */
#include <ti_drivers_config.h>

/**
 * @brief Internal UART status values
 */
typedef enum
{
    eUartIdle,
    eUartActiveAsync,
    eUartActiveSync,
} UartStatus_t;

/**
 * @brief TI Simplelink UART HAL context abstracted in the Peripheral_Descriptor_t.
 *
 * Contains all the parameters needed for working with TI Simplelink UART driver.
 */
typedef struct
{
    UART_Handle tiUartHandle;        /**< TI driver handle used with TI Driver API. */
    IotUARTConfig_t xUartParams;     /**< Active configuration. */
    IotUARTCallback_t xUartCallback; /**< User registered callback. */
    void * pvUserCallbackContext;    /**< User callback cotnext. */
    SemaphoreP_Struct xSyncReadSem;  /**< Semaphore used for sync write/read calls */
    SemaphoreP_Struct xSyncWriteSem; /**< Semaphore used for sync write/read calls */
    size_t xLastReadSize;            /**< Last complete number of bytes read. */
    size_t xLastWriteSize;           /**< Last complete number of bytes writte.n */
    uint8_t ucReadStatus;            /**< Status of the UART read operation. */
    uint8_t ucWriteStatus;           /**< Status of the UART write operation. */
} IotUARTDescriptor_t;

static void prvWriteCallback( UART_Handle pxUartHandle,
                              void * pvBuffer,
                              size_t xBytes );
static void prvReadCallback( UART_Handle pxUartHandle,
                             void * pvBuffer,
                             size_t xBytes );

/**
 * @brief Default UART parameters structure used when opening the interface for
 *        the first time.
 */
const UART_Params xUartDefaultParams =
{
    UART_MODE_CALLBACK,         /* readMode */
    UART_MODE_CALLBACK,         /* writeMode */
    UART_WAIT_FOREVER,          /* readTimeout */
    UART_WAIT_FOREVER,          /* writeTimeout */
    prvReadCallback,            /* readCallback */
    prvWriteCallback,           /* writeCallback */
    UART_RETURN_NEWLINE,        /* readReturnMode */
    UART_DATA_BINARY,           /* readDataMode */
    UART_DATA_BINARY,           /* writeDataMode */
    UART_ECHO_OFF,              /* readEcho */
    IOT_UART_BAUD_RATE_DEFAULT, /* baudRate */
    UART_LEN_8,                 /* ulDataLength */
    UART_STOP_ONE,              /* ulStopBits */
    UART_PAR_NONE,              /* ulParityType */
    NULL                        /* custom */
};

/**
 * @brief TI UART Driver parameter to iot_uart.g definition LUTs.
 */
static const uint32_t ulDataLength[] =
{
    UART_LEN_5,  /* UART_LEN_5 */
    UART_LEN_6,  /* UART_LEN_6 */
    UART_LEN_7,  /* UART_LEN_7 */
    UART_LEN_8   /* UART_LEN_8 */
};

static const uint32_t ulStopBits[] =
{
    UART_STOP_ONE,  /* UART_STOP_ONE */
    UART_STOP_TWO   /* UART_STOP_TWO */
};

static const uint32_t ulParityType[] =
{
    UART_PAR_NONE, /* UART_PAR_NONE */
    UART_PAR_ODD,  /* UART_PAR_ODD */
    UART_PAR_EVEN, /* UART_PAR_EVEN */
};

/**
 * @brief Static UART descriptor table
 */
static IotUARTDescriptor_t xUart[ CONFIG_TI_DRIVERS_UART_COUNT ];

static bool xIsInitialized = false;

/*-----------------------------------------------------------*/

IotUARTHandle_t iot_uart_open( int32_t lUartInstance )
{
    UART_Handle xTiHandle = NULL;
    IotUARTHandle_t xHandle = NULL;

    if( false == xIsInitialized )
    {
        UART_init();
        xIsInitialized = true;
    }

    xTiHandle = UART_open( lUartInstance, ( UART_Params * ) &xUartDefaultParams );

    if( xTiHandle )
    {
        xUart[ lUartInstance ].tiUartHandle = xTiHandle;
        xUart[ lUartInstance ].xUartCallback = NULL;
        xUart[ lUartInstance ].pvUserCallbackContext = NULL;
        xUart[ lUartInstance ].xLastReadSize = 0;
        xUart[ lUartInstance ].xLastWriteSize = 0;
        xUart[ lUartInstance ].ucReadStatus = eUartIdle;
        xUart[ lUartInstance ].ucWriteStatus = eUartIdle;
        xUart[ lUartInstance ].xUartParams.ucFlowControl = 0;
        xUart[ lUartInstance ].xUartParams.ulBaudrate = IOT_UART_BAUD_RATE_DEFAULT;
        xUart[ lUartInstance ].xUartParams.ucWordlength = 8;
        xUart[ lUartInstance ].xUartParams.xStopbits = eUartStopBitsOne;
        xUart[ lUartInstance ].xUartParams.xParity = eUartParityNone;

        SemaphoreP_constructBinary( &( xUart[ lUartInstance ].xSyncReadSem ), 0 );
        SemaphoreP_constructBinary( &( xUart[ lUartInstance ].xSyncWriteSem ), 0 );

        xHandle = ( IotUARTHandle_t ) &xUart[ lUartInstance ];
    }

    return xHandle;
}

/*-----------------------------------------------------------*/

void iot_uart_set_callback( IotUARTHandle_t const pxUartPeripheral,
                            IotUARTCallback_t xCallback,
                            void * pvUserContext )
{
    IotUARTDescriptor_t * pxUartDesc = ( IotUARTDescriptor_t * ) pxUartPeripheral;

    if( pxUartDesc )
    {
        pxUartDesc->xUartCallback = xCallback;
        pxUartDesc->pvUserCallbackContext = pvUserContext;
    }
}

/*-----------------------------------------------------------*/

int32_t iot_uart_read_sync( IotUARTHandle_t const pxUartPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes )
{
    int32_t ret = IOT_UART_READ_FAILED;
    IotUARTDescriptor_t * pxUartDesc = ( IotUARTDescriptor_t * ) pxUartPeripheral;

    if( ( NULL == pxUartDesc ) || ( NULL == pvBuffer ) || ( 0 == xBytes ) ||
        ( NULL == pxUartDesc->tiUartHandle ) )
    {
        ret = IOT_UART_INVALID_VALUE;
    }
    else if( UART_ERROR != UART_read( pxUartDesc->tiUartHandle, pvBuffer, xBytes ) )
    {
        pxUartDesc->ucReadStatus = eUartActiveSync;
        pxUartDesc->xLastWriteSize = 0;

        if( SemaphoreP_OK == SemaphoreP_pend( &( pxUartDesc->xSyncReadSem ), UART_WAIT_FOREVER ) )
        {
            ret = IOT_UART_SUCCESS;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_uart_write_sync( IotUARTHandle_t const pxUartPeripheral,
                             uint8_t * const pvBuffer,
                             size_t xBytes )
{
    int32_t ret = IOT_UART_WRITE_FAILED;
    IotUARTDescriptor_t * pxUartDesc = ( IotUARTDescriptor_t * ) pxUartPeripheral;

    if( ( NULL == pxUartDesc ) || ( NULL == pvBuffer ) || ( 0 == xBytes ) ||
        ( NULL == pxUartDesc->tiUartHandle ) )
    {
        ret = IOT_UART_INVALID_VALUE;
    }
    else if( UART_ERROR != UART_write( pxUartDesc->tiUartHandle, pvBuffer, xBytes ) )
    {
        pxUartDesc->ucWriteStatus = eUartActiveSync;
        pxUartDesc->xLastReadSize = 0;

        if( SemaphoreP_OK == SemaphoreP_pend( &( pxUartDesc->xSyncWriteSem ), UART_WAIT_FOREVER ) )
        {
            ret = IOT_UART_SUCCESS;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_uart_read_async( IotUARTHandle_t const pxUartPeripheral,
                             uint8_t * const pvBuffer,
                             size_t xBytes )
{
    int32_t ret = IOT_UART_READ_FAILED;
    IotUARTDescriptor_t * pxUartDesc = ( IotUARTDescriptor_t * ) pxUartPeripheral;

    if( ( NULL == pxUartDesc ) || ( NULL == pvBuffer ) || ( 0 == xBytes ) ||
        ( NULL == pxUartDesc->tiUartHandle ) )
    {
        ret = IOT_UART_INVALID_VALUE;
    }
    else
    {
        if( eUartIdle == pxUartDesc->ucReadStatus )
        {
            pxUartDesc->ucReadStatus = eUartActiveAsync;

            if( UART_ERROR != UART_read( pxUartDesc->tiUartHandle, pvBuffer, xBytes ) )
            {
                pxUartDesc->xLastWriteSize = 0;
                ret = IOT_UART_SUCCESS;
            }
            else
            {
                pxUartDesc->ucReadStatus = eUartIdle;
            }
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_uart_write_async( IotUARTHandle_t const pxUartPeripheral,
                              uint8_t * const pvBuffer,
                              size_t xBytes )
{
    int32_t ret = IOT_UART_WRITE_FAILED;
    IotUARTDescriptor_t * pxUartDesc = ( IotUARTDescriptor_t * ) pxUartPeripheral;

    if( ( NULL == pxUartDesc ) || ( NULL == pvBuffer ) || ( 0 == xBytes ) ||
        ( NULL == pxUartDesc->tiUartHandle ) )
    {
        ret = IOT_UART_INVALID_VALUE;
    }
    else
    {
        if( eUartIdle == pxUartDesc->ucWriteStatus )
        {
            pxUartDesc->ucWriteStatus = eUartActiveAsync;

            if( UART_ERROR != UART_write( pxUartDesc->tiUartHandle, pvBuffer, xBytes ) )
            {
                pxUartDesc->xLastReadSize = 0;
                ret = IOT_UART_SUCCESS;
            }
            else
            {
                pxUartDesc->ucWriteStatus = eUartIdle;
            }
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_uart_ioctl( IotUARTHandle_t const pxUartPeripheral,
                        IotUARTIoctlRequest_t xUartRequest,
                        void * const pvBuffer )
{
    int32_t ret = IOT_UART_SUCCESS;
    IotUARTDescriptor_t * pxUartDesc = ( IotUARTDescriptor_t * ) pxUartPeripheral;

    if( ( NULL == pxUartDesc ) || ( NULL == pvBuffer ) || ( NULL == pxUartDesc->tiUartHandle ) )
    {
        ret = IOT_UART_INVALID_VALUE;
    }
    else
    {
        switch( xUartRequest )
        {
            case eUartSetConfig:

                if( ( eUartIdle != pxUartDesc->ucWriteStatus ) ||
                    ( eUartIdle != pxUartDesc->ucReadStatus ) )
                {
                    ret = IOT_UART_BUSY;
                }
                else
                {
                    IotUARTConfig_t * pxConfig = ( IotUARTConfig_t * ) pvBuffer;

                    /* Check index ranges to that of the LUTs. Always deny flow control
                     * as this is configured in the board file and we can't control this
                     * without making the implementation device specific. */
                    if( ( ( ( pxConfig->ucWordlength - 5 ) < 0 ) || ( ( pxConfig->ucWordlength - 5 ) > 3 ) ) ||
                        ( ( pxConfig->xParity < 0 ) || ( pxConfig->xParity > 2 ) ) ||
                        ( ( pxConfig->xStopbits < 0 ) || ( pxConfig->xStopbits > 1 ) ) ||
                        ( pxConfig->ucFlowControl != 0 ) )
                    {
                        ret = IOT_UART_FUNCTION_NOT_SUPPORTED;
                    }
                    else
                    {
                        UART_Params xNewParams = xUartDefaultParams;
                        xNewParams.baudRate = pxConfig->ulBaudrate;
                        xNewParams.parityType = ulParityType[ pxConfig->xParity ];
                        xNewParams.stopBits = ulStopBits[ pxConfig->xStopbits ];
                        xNewParams.dataLength = ulDataLength[ pxConfig->ucWordlength - 5 ];

                        /* Close and re-open the same instance */
                        uint32_t i;

                        for( i = 0; i < CONFIG_TI_DRIVERS_UART_COUNT; i++ )
                        {
                            if( &xUart[ i ] == pxUartDesc )
                            {
                                UART_close( pxUartDesc->tiUartHandle );
                                pxUartDesc->tiUartHandle = UART_open( i, &xNewParams );

                                pxUartDesc->xUartParams.ucFlowControl = pxConfig->ucFlowControl;
                                pxUartDesc->xUartParams.ulBaudrate = pxConfig->ulBaudrate;
                                pxUartDesc->xUartParams.xParity = pxConfig->xParity;
                                pxUartDesc->xUartParams.xStopbits = pxConfig->xStopbits;
                                pxUartDesc->xUartParams.ucWordlength = pxConfig->ucWordlength;

                                if( NULL == pxUartDesc->tiUartHandle )
                                {
                                    ret = IOT_UART_INVALID_VALUE;
                                }

                                break;
                            }
                        }
                    }
                }

                break;

            case eUartGetConfig:
               {
                   IotUARTConfig_t * pxConfig = ( IotUARTConfig_t * ) pvBuffer;

                   pxConfig->ucFlowControl = pxUartDesc->xUartParams.ucFlowControl;
                   pxConfig->ulBaudrate = pxUartDesc->xUartParams.ulBaudrate;
                   pxConfig->xParity = pxUartDesc->xUartParams.xParity;
                   pxConfig->xStopbits = pxUartDesc->xUartParams.xStopbits;
                   pxConfig->ucWordlength = pxUartDesc->xUartParams.ucWordlength;

                   break;
               }

            case eGetTxNoOfbytes:
                /* For CC13X2 and CC26X2 devices we can check writeCount
                 * instead of "lastWriteSize" */
                #if ( defined DeviceFamily_CC13X2 ) || ( defined DeviceFamily_CC26X2 ) || ( defined DeviceFamily_CC13X2_CC26X2 )
                    *( uint16_t * ) pvBuffer = ((UARTCC26XX_Object *) pxUartDesc->tiUartHandle)->writeCount;
                #else
                    *( uint16_t * ) pvBuffer = pxUartDesc->xLastWriteSize;
                #endif
                break;

            case eGetRxNoOfbytes:
                /* For CC13X2 and CC26X2 devices we can check readCount
                 * instead of "lastReadSize" */
                #if ( defined DeviceFamily_CC13X2 ) || ( defined DeviceFamily_CC26X2 ) || ( defined DeviceFamily_CC13X2_CC26X2 )
                    *( uint16_t * ) pvBuffer = ((UARTCC26XX_Object *) pxUartDesc->tiUartHandle)->readCount;
                #else
                    *( uint16_t * ) pvBuffer = pxUartDesc->xLastReadSize;
                #endif
                break;

            default:
                ret = IOT_UART_INVALID_VALUE;
                break;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_uart_cancel( IotUARTHandle_t const pxUartPeripheral )
{
    int32_t ret = IOT_UART_SUCCESS;
    IotUARTDescriptor_t * pxUartDesc = ( IotUARTDescriptor_t * ) pxUartPeripheral;

    if( ( NULL == pxUartDesc ) || ( NULL == pxUartDesc->tiUartHandle ) )
    {
        ret = IOT_UART_INVALID_VALUE;
    }
    else if( ( eUartIdle == pxUartDesc->ucWriteStatus ) &&
             ( eUartIdle == pxUartDesc->ucReadStatus ) )
    {
        ret = IOT_UART_NOTHING_TO_CANCEL;
    }
    else
    {
        pxUartDesc->ucWriteStatus = eUartIdle;
        UART_writeCancel( pxUartDesc->tiUartHandle );
        pxUartDesc->ucReadStatus = eUartIdle;
        UART_readCancel( pxUartDesc->tiUartHandle );
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_uart_close( IotUARTHandle_t const pxUartPeripheral )
{
    int32_t ret = IOT_UART_SUCCESS;
    IotUARTDescriptor_t * pxUartDesc = ( IotUARTDescriptor_t * ) pxUartPeripheral;

    if( ( NULL == pxUartDesc ) || ( NULL == pxUartDesc->tiUartHandle ) )
    {
        ret = IOT_UART_INVALID_VALUE;
    }
    else
    {
        pxUartDesc->ucWriteStatus = eUartIdle;
        pxUartDesc->ucReadStatus = eUartIdle;

        UART_close( pxUartDesc->tiUartHandle );
        SemaphoreP_destruct( &( pxUartDesc->xSyncWriteSem ) );
        SemaphoreP_destruct( &( pxUartDesc->xSyncReadSem ) );

        pxUartDesc->tiUartHandle = NULL;
    }

    return ret;
}

/*-----------------------------------------------------------*/

static void prvWriteCallback( UART_Handle pxUartHandle,
                              void * pvBuffer,
                              size_t xBytes )
{
    uint32_t i;

    for( i = 0; i < CONFIG_TI_DRIVERS_UART_COUNT; i++ )
    {
        if( xUart[ i ].tiUartHandle == pxUartHandle )
        {
            if( xUart[ i ].ucWriteStatus == eUartActiveAsync )
            {
                if( xUart[ i ].xUartCallback )
                {
                    /* We can't tell if we got the callback due to the operation
                     * failing or not, assume complete. */
                    xUart[ i ].xUartCallback( eUartWriteCompleted, xUart[ i ].pvUserCallbackContext );
                }
            }
            else
            {
                SemaphoreP_post( &( xUart[ i ].xSyncWriteSem ) );
            }

            xUart[ i ].ucWriteStatus = eUartIdle;
            xUart[ i ].xLastWriteSize = xBytes;
            break;
        }
    }
}

/*-----------------------------------------------------------*/

static void prvReadCallback( UART_Handle pxUartHandle,
                             void * pvBuffer,
                             size_t xBytes )
{
    uint32_t i;

    for( i = 0; i < CONFIG_TI_DRIVERS_UART_COUNT; i++ )
    {
        if( xUart[ i ].tiUartHandle == pxUartHandle )
        {
            if( xUart[ i ].ucReadStatus == eUartActiveAsync )
            {
                if( xUart[ i ].xUartCallback )
                {
                    /* We can't tell if we got the callback due to the operation
                     * failing or not, assume complete. */
                    xUart[ i ].xUartCallback( eUartReadCompleted, xUart[ i ].pvUserCallbackContext );
                }
            }
            else
            {
                SemaphoreP_post( &( xUart[ i ].xSyncReadSem ) );
            }

            xUart[ i ].ucReadStatus = eUartIdle;
            xUart[ i ].xLastReadSize = xBytes;
            break;
        }
    }
}
