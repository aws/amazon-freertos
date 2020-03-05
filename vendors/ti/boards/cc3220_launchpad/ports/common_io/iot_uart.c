/*
 * FreeRTOS Common IO V0.1.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file iot_uart.c
 * @brief file containing the definition of UART APIs calling TI drivers.
 */
#include "Board.h"
#include "iot_uart.h"
#include <string.h>
#include <ti/devices/cc32xx/inc/hw_memmap.h>
#include <ti/devices/cc32xx/inc/hw_ints.h>
#include <ti/devices/cc32xx/inc/hw_types.h>
#include <ti/drivers/UART.h>
#include <ti/devices/cc32xx/driverlib/uart.h>
#include <ti/drivers/uart/UARTCC32XX.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief The number of USART ports supported by CC3220 board.
 */
#define IOT_USART_PORTS    ( 2 )

#define _getHWAttrs( tiUartHandle )      ( ( UARTCC32XX_HWAttrsV1 const * ) tiUartHandle->hwAttrs )

#define _isBusy( tiUartHandle )          ( MAP_UARTBusy( _getHWAttrs( tiUartHandle )->baseAddr ) || MAP_UARTCharsAvail( _getHWAttrs( tiUartHandle )->baseAddr ) )
#define _isIdle( tiUartHandle )          ( _isBusy( tiUartHandle ) == false )
#define _toUartHandle( tiUartHandle )    ( _getHWAttrs( tiUartHandle )->baseAddr == UARTA0_BASE ? pxUarts[ 0 ] : pxUarts[ 1 ] )

/**
 * @brief The callback function defined at I/O level indicating completion of write operation.
 *
 * When the TI driver is in UART_MODE_CALLBACK mode, on completion of transmission of data,
 * this function will be called.
 *
 * @param[in] pxUartHandle The handle passed by the driver which has all information about the write operation.
 * @param[in] pvBuffer The pointer to write buffer.
 * @param[in] xBytes The number of bytes written.
 */
static void _writeCallback( UART_Handle tiUartHandle,
                            void * pBuffer,
                            size_t bytes );

/**
 * @brief The callback function defined at I/O level indicating completion of read operation.
 *
 * When the TI driver is in UART_MODE_CALLBACK mode, on completion of reception of data,
 * this function will be called.
 *
 * @param[in] pxUartHandle The handle passed by the driver which has all information about the read operation.
 * @param[out] pvBuffer The pointer to read buffer.
 * @param[in] xBytes The number of bytes read.
 */
static void _readCallback( UART_Handle tiUartHandle,
                           void * pBuffer,
                           size_t bytes );

static int8_t _indexOf( const uint32_t * pArray,
                        int8_t len,
                        uint32_t val );

/**
 * @brief TI CC3220 HAL Context abstracted in the Peripheral_Descriptor_t.
 *
 * Contains all the parameters needed for working with TI CC3220 driver.
 */
typedef struct IotUARTDescriptor
{
    UART_Handle tiUartHandle;    /**< TI UART driver handle for different I/O functions. */
    UART_Params * pTiUartParams; /**< Parameters to pass to the TI UART driver. */
    uint_least8_t tiUartIndex;   /**< The UART port number to use. */
    uint16_t txNoOfbytes;
    uint16_t rxNoOfbytes;
    IotUARTCallback_t uartCallback;
    void * pUserCallbackContext;
} IotUARTDescriptor_t;

/**
 * @brief Statically initialized map of TI CC3220 parameters for 2 ports.
 */
static UART_Params xUartParamsMap[ IOT_USART_PORTS ] =
{
    {
        .readMode = UART_MODE_CALLBACK,
        .writeMode = UART_MODE_CALLBACK,
        .readTimeout = 30000,
        .writeTimeout = 30000,
        .readCallback = _readCallback,
        .writeCallback = _writeCallback,
        .writeDataMode = UART_DATA_BINARY,
        .readDataMode = UART_DATA_BINARY,
        .readReturnMode = UART_RETURN_FULL,
        .readEcho = UART_ECHO_OFF,
        .baudRate = IOT_UART_BAUD_RATE_DEFAULT,
        .dataLength = UART_LEN_8,
        .parityType = UART_PAR_NONE,
        .stopBits = UART_STOP_ONE,
        .custom = NULL,
    },
    {
        .readMode = UART_MODE_CALLBACK,
        .writeMode = UART_MODE_CALLBACK,
        .readTimeout = 30000,
        .writeTimeout = 30000,
        .readCallback = _readCallback,
        .writeCallback = _writeCallback,
        .writeDataMode = UART_DATA_BINARY,
        .readDataMode = UART_DATA_BINARY,
        .readReturnMode = UART_RETURN_FULL,
        .readEcho = UART_ECHO_OFF,
        .baudRate = IOT_UART_BAUD_RATE_DEFAULT,
        .dataLength = UART_LEN_8,
        .parityType = UART_PAR_NONE,
        .stopBits = UART_STOP_ONE,
        .custom = NULL,
    }
};

/**
 * @brief static Peripheral Descriptors to return to the POSIX open() function
 */
static IotUARTDescriptor_t xUart0 =
{
    .tiUartHandle         = NULL,
    .pTiUartParams        = &xUartParamsMap[ 0 ],
    .tiUartIndex          = ( uint_least8_t ) Board_UART0,
    .txNoOfbytes          = 0,
    .rxNoOfbytes          = 0,
    .uartCallback         = NULL,
    .pUserCallbackContext = NULL
};
static IotUARTDescriptor_t xUart1 =
{
    .tiUartHandle         = NULL,
    .pTiUartParams        = &xUartParamsMap[ 1 ],
    .tiUartIndex          = ( uint_least8_t ) Board_UART1,
    .txNoOfbytes          = 0,
    .rxNoOfbytes          = 0,
    .uartCallback         = NULL,
    .pUserCallbackContext = NULL
};

static IotUARTDescriptor_t * pxUarts[] = { &xUart0, &xUart1 };

static const uint32_t dataLength[] =
{
    UART_CONFIG_WLEN_5, /* UART_LEN_5 */
    UART_CONFIG_WLEN_6, /* UART_LEN_6 */
    UART_CONFIG_WLEN_7, /* UART_LEN_7 */
    UART_CONFIG_WLEN_8  /* UART_LEN_8 */
};

static const uint32_t stopBits[] =
{
    UART_CONFIG_STOP_ONE, /* UART_STOP_ONE */
    UART_CONFIG_STOP_TWO  /* UART_STOP_TWO */
};

static const uint32_t parityType[] =
{
    UART_CONFIG_PAR_NONE, /* UART_PAR_NONE */
    UART_CONFIG_PAR_ODD,  /* UART_PAR_ODD */
    UART_CONFIG_PAR_EVEN, /* UART_PAR_EVEN */
};

/*-----------------------------------------------------------*/

IotUARTHandle_t iot_uart_open( int32_t lUartInstance )
{
    /* If instance number is out of range. */
    if( ( lUartInstance < 0 ) && ( lUartInstance >= IOT_USART_PORTS ) )
    {
        return NULL;
    }

    /* If it is already open. */
    if( pxUarts[ lUartInstance ]->tiUartHandle != NULL )
    {
        return NULL;
    }

    IotUARTHandle_t xHandle = NULL;

    UART_init();

    pxUarts[ lUartInstance ]->tiUartHandle = UART_open( pxUarts[ lUartInstance ]->tiUartIndex, pxUarts[ lUartInstance ]->pTiUartParams );
    xHandle = pxUarts[ lUartInstance ]->tiUartHandle == NULL ? NULL : pxUarts[ lUartInstance ];

    return xHandle;
}

/*-----------------------------------------------------------*/

void iot_uart_set_callback( IotUARTHandle_t const uartHandle,
                            IotUARTCallback_t callback,
                            void * pUserCallbackContext )
{
    if( uartHandle != NULL )
    {
        uartHandle->uartCallback = callback;
        uartHandle->pUserCallbackContext = pUserCallbackContext;
    }
}

/*-----------------------------------------------------------*/

int32_t iot_uart_read_sync( IotUARTHandle_t const uartHandle,
                            uint8_t * const pBuffer,
                            size_t bytes )
{
    if( ( uartHandle == NULL ) || ( pBuffer == NULL ) || ( bytes == 0 ) || ( uartHandle->tiUartHandle == NULL ) )
    {
        return IOT_UART_INVALID_VALUE;
    }

    int32_t ret = IOT_UART_READ_FAILED;

    int_fast32_t readBytes = UART_readPolling( uartHandle->tiUartHandle, pBuffer, bytes );

    if( readBytes > 0 )
    {
        uartHandle->rxNoOfbytes = readBytes;
        ret = IOT_UART_SUCCESS;
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_uart_write_sync( IotUARTHandle_t const uartHandle,
                             uint8_t * const pBuffer,
                             size_t bytes )
{
    if( ( uartHandle == NULL ) || ( pBuffer == NULL ) || ( bytes == 0 ) || ( uartHandle->tiUartHandle == NULL ) )
    {
        return IOT_UART_INVALID_VALUE;
    }

    int32_t ret = IOT_UART_WRITE_FAILED;

    int_fast32_t writtenBytes = UART_writePolling( uartHandle->tiUartHandle, pBuffer, bytes );

    if( writtenBytes > 0 )
    {
        uartHandle->txNoOfbytes = writtenBytes;
        ret = IOT_UART_SUCCESS;
    }

    return ret;
}

/*-----------------------------------------------------------*/
int32_t iot_uart_read_async( IotUARTHandle_t const uartHandle,
                             uint8_t * const pBuffer,
                             size_t bytes )
{
    if( ( uartHandle == NULL ) || ( pBuffer == NULL ) || ( bytes == 0 ) || ( uartHandle->tiUartHandle == NULL ) )
    {
        return IOT_UART_INVALID_VALUE;
    }

    int32_t ret = IOT_UART_SUCCESS;

    if( UART_read( uartHandle->tiUartHandle, pBuffer, bytes ) < 0 )
    {
        ret = IOT_UART_READ_FAILED;
    }

    return ret;
}

/*-----------------------------------------------------------*/
int32_t iot_uart_write_async( IotUARTHandle_t const uartHandle,
                              uint8_t * const pBuffer,
                              size_t bytes )
{
    if( ( uartHandle == NULL ) || ( pBuffer == NULL ) || ( bytes == 0 ) || ( uartHandle->tiUartHandle == NULL ) )
    {
        return IOT_UART_INVALID_VALUE;
    }

    int32_t ret = IOT_UART_SUCCESS;

    if( UART_write( uartHandle->tiUartHandle, pBuffer, bytes ) < 0 )
    {
        ret = IOT_UART_WRITE_FAILED;
    }

    return ret;
}

/*-----------------------------------------------------------*/
int32_t iot_uart_close( IotUARTHandle_t const uartHandle )
{
    if( ( uartHandle == NULL ) || ( uartHandle->tiUartHandle == NULL ) )
    {
        return IOT_UART_INVALID_VALUE;
    }

    /* TI Driver requires to cancel on-going transfer before calling close. */
    iot_uart_cancel( uartHandle );

    UART_close( uartHandle->tiUartHandle );
    uartHandle->tiUartHandle = NULL;

    return IOT_UART_SUCCESS;
}

/*-----------------------------------------------------------*/

int32_t iot_uart_ioctl( IotUARTHandle_t const uartHandle,
                        IotUARTIoctlRequest_t uartRequest,
                        void * const pBuffer )
{
    if( ( uartHandle == NULL ) || ( pBuffer == NULL ) || ( uartHandle->tiUartHandle == NULL ) )
    {
        return IOT_UART_INVALID_VALUE;
    }

    int32_t ret = IOT_UART_SUCCESS;
    const UARTCC32XX_HWAttrsV1 * hwAttrs = _getHWAttrs( uartHandle->tiUartHandle );

    ClockP_FreqHz freq;
    ClockP_getCpuFreq( &freq );

    switch( uartRequest )
    {
        case eUartSetConfig:
           {
               if( _isBusy( uartHandle->tiUartHandle ) )
               {
                   ret = IOT_UART_BUSY;
                   break;
               }

               IotUARTConfig_t * pConfig = ( IotUARTConfig_t * ) pBuffer;
               unsigned long tiFlowMode = pConfig->ucFlowControl == 0 ? UART_FLOWCONTROL_NONE : UART_FLOWCONTROL_TX | UART_FLOWCONTROL_RX;
               unsigned long tiConfig = dataLength[ pConfig->ucWordlength - 5 ] | stopBits[ pConfig->xStopbits ] | parityType[ pConfig->xParity ];

               MAP_UARTFlowControlSet( hwAttrs->baseAddr, tiFlowMode );
               MAP_UARTConfigSetExpClk( hwAttrs->baseAddr, freq.lo, pConfig->ulBaudrate, tiConfig );

               break;
           }

        case eUartGetConfig:
           {
               IotUARTConfig_t * pConfig = ( IotUARTConfig_t * ) pBuffer;
               unsigned long baud = 0;
               unsigned long tiConfig = 0;
               unsigned long tiFlowMode = 0;

               tiFlowMode = MAP_UARTFlowControlGet( hwAttrs->baseAddr );
               MAP_UARTConfigGetExpClk( hwAttrs->baseAddr, freq.lo, &baud, &tiConfig );

               pConfig->ucFlowControl = tiFlowMode == UART_FLOWCONTROL_NONE ? 0 : 1;
               pConfig->ulBaudrate = baud;
               pConfig->ucWordlength = ( uint8_t ) _indexOf( dataLength, sizeof( dataLength ), ( tiConfig & UART_CONFIG_WLEN_MASK ) ) + 5;
               pConfig->xStopbits = ( IotUARTStopBits_t ) _indexOf( stopBits, sizeof( stopBits ), ( tiConfig & UART_CONFIG_STOP_MASK ) );
               pConfig->xParity = ( IotUARTParity_t ) _indexOf( parityType, sizeof( parityType ), ( tiConfig & UART_CONFIG_PAR_MASK ) );

               break;
           }

        case eGetTxNoOfbytes:
            *( uint16_t * ) pBuffer = uartHandle->txNoOfbytes;
            break;

        case eGetRxNoOfbytes:
            *( uint16_t * ) pBuffer = uartHandle->rxNoOfbytes;
            break;

        default:
            ret = IOT_UART_INVALID_VALUE;
            break;
    }

    return ret;
}

/*-----------------------------------------------------------*/

int32_t iot_uart_cancel( IotUARTHandle_t const uartHandle )
{
    if( ( uartHandle == NULL ) || ( uartHandle->tiUartHandle == NULL ) )
    {
        return IOT_UART_INVALID_VALUE;
    }

    if( _isIdle( uartHandle->tiUartHandle ) )
    {
        return IOT_UART_NOTHING_TO_CANCEL;
    }

    UART_writeCancel( uartHandle->tiUartHandle );
    UART_readCancel( uartHandle->tiUartHandle );

    /* Have to sleep for a short period to let cancel finish. */
    vTaskDelay( pdMS_TO_TICKS( 1000 ) );

    return IOT_UART_SUCCESS;
}

/*-----------------------------------------------------------*/

static void _readCallback( UART_Handle tiUartHandle,
                           void * pBuffer,
                           size_t bytes )
{
    IotUARTHandle_t uartHandle = _toUartHandle( tiUartHandle );

    uartHandle->rxNoOfbytes = bytes;

    if( uartHandle->uartCallback != NULL )
    {
        uartHandle->uartCallback( eUartReadCompleted, uartHandle->pUserCallbackContext );
    }
}

/*-----------------------------------------------------------*/

static void _writeCallback( UART_Handle tiUartHandle,
                            void * pBuffer,
                            size_t bytes )
{
    IotUARTHandle_t uartHandle = _toUartHandle( tiUartHandle );

    uartHandle->txNoOfbytes = bytes;

    if( uartHandle->uartCallback != NULL )
    {
        uartHandle->uartCallback( eUartWriteCompleted, uartHandle->pUserCallbackContext );
    }
}

static int8_t _indexOf( const uint32_t * pArray,
                        int8_t len,
                        uint32_t val )
{
    int8_t i = 0;

    for( ; i < len; i++ )
    {
        if( pArray[ i ] == val )
        {
            return i;
        }
    }

    return -1;
}
