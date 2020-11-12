/*
 * Amazon FreeRTOS CELLULAR Preview Release
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

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* Driver model includes. */
#include "iot_uart.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "event_groups.h"

#include "platform/iot_threads.h"
#include "iot_fifo.h"

/* Cellular includes. */
#include "aws_cellular_config.h"
#include "cellular_config_defaults.h"
#include "cellular_comm_interface.h"

/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_GLOBAL
    #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
#else
    #define LIBRARY_LOG_LEVEL    IOT_LOG_ERROR
#endif

#define LIBRARY_LOG_NAME         ( "COMM_IF_UART" )
#include "iot_logging_setup.h"

/*-----------------------------------------------------------*/

/**
 * @brief Common IO UART configurations.
 */
#define CELLULAR_COMM_IF_UART_PORT           ( 3U )
#define CELLULAR_COMM_IF_UART_BAUDRATE       ( 115200 )
#define CELLULAR_COMM_IF_UART_WORD_LENGTH    ( 0 )                       /* UART_WORDLENGTH_8B. */

/**
 * @brief Comm events.
 */
#define COMM_EVT_MASK_TX_DONE                ( 0x0001U )
#define COMM_EVT_MASK_TX_ERROR               ( 0x0002U )
#define COMM_EVT_MASK_TX_ABORTED             ( 0x0004U )
#define COMM_EVT_MASK_RX_DONE                ( 0x0008U )
#define COMM_EVT_MASK_RX_ERROR               ( 0x0010U )
#define COMM_EVT_MASK_RX_ABORTED             ( 0x0020U )

/**
 * @brief Internal FIFO size of received bytes.
 */
#define COMM_IF_FIFO_BUFFER_SIZE             ( 1600 )

/**
 * @brief Ticks to ms convert.
 */
#define TICKS_TO_MS( xTicks )    ( ( ( xTicks ) * 1000U ) / ( ( uint32_t ) configTICK_RATE_HZ ) )

/**
 * @brief UART send and receive timing parameters.
 */
#define DEFAULT_WAIT_INTERVAL         ( 20UL )
#define DEFAULT_RECV_WAIT_INTERVAL    ( 5UL )
#define DEFAULT_RECV_TIMEOUT          ( 1000UL )

/*-----------------------------------------------------------*/

/**
 * @brief A context of the communication interface.
 */
typedef struct CellularCommInterfaceContextStruct
{
    EventGroupHandle_t pEventGroup;                 /**< Event group for TX or RX callback event to wakeup receiver's thread. */
    uint8_t uartRxChar[ 1 ];                        /**< 1 byte receive buffer. */
    CellularCommInterfaceReceiveCallback_t pRecvCB; /**< CellularCommInterfaceReceiveCallback_t callback function set in open function. */
    IotUARTHandle_t uartHandle;                     /**< Common IO uart handler. */
    void * pUserData;                               /**< pUserData in CellularCommInterfaceReceiveCallback_t callback function. */
    uint8_t uartBusyFlag;                           /**< Flag for whether the physical interface is busy or not. */

    IotFifo_t rxFifo;                               /**< Internal FIFO for recevied data from UART. */
    uint8_t fifoBuffer[ COMM_IF_FIFO_BUFFER_SIZE ]; /**< FIFO buffer for internal FIFO. */
    uint8_t rxFifoReadingFlag;                      /**< Indicate the reader is reading the FIFO in recv function. */
    bool ifOpen;                                    /**< Communicate interface open status. */
} CellularCommInterfaceContext;

/*-----------------------------------------------------------*/

/**
 * @brief Cellular comm if open function implementation.
 * Reference CellularCommInterfaceOpen_t in cellular_comm_interface.h.
 */
static CellularCommInterfaceError_t prvCellularOpen( CellularCommInterfaceReceiveCallback_t receiveCallback,
                                                     void * pUserData,
                                                     CellularCommInterfaceHandle_t * pIotCommIntfCtx );

/**
 * @brief Cellular comm if close function implementation.
 * Reference CellularCommInterfaceClose_t in cellular_comm_interface.h.
 */
static CellularCommInterfaceError_t prvCellularClose( CellularCommInterfaceHandle_t commInterfaceHandle );

/**
 * @brief Cellular comm if receive function implementation.
 * Reference CellularCommInterfaceRecv_t in cellular_comm_interface.h.
 */
static CellularCommInterfaceError_t prvCellularReceive( CellularCommInterfaceHandle_t commInterfaceHandle,
                                                        uint8_t * pBuffer,
                                                        uint32_t bufferLength,
                                                        uint32_t timeoutMilliseconds,
                                                        uint32_t * pDataReceivedLength );

/**
 * @brief Cellular comm if send function implementation.
 * Reference CellularCommInterfaceSend_t in cellular_comm_interface.h.
 */
static CellularCommInterfaceError_t prvCellularSend( CellularCommInterfaceHandle_t commInterfaceHandle,
                                                     const uint8_t * pData,
                                                     uint32_t dataLength,
                                                     uint32_t timeoutMilliseconds,
                                                     uint32_t * pDataSentLength );

/**
 * @brief Cellular comm interface write complete function.
 *
 * @param[in] pIotCommIntfCtx Comm interface context for event group.
 */
static void prvWriteComplete( CellularCommInterfaceContext * pIotCommIntfCtx );

/**
 * @brief Cellular comm interface read complete function
 *
 * @param[in] pIotCommIntfCtx Comm interface context for event group.
 */
static void prvReadComplete( CellularCommInterfaceContext * pIotCommIntfCtx );

/**
 * @brief The callback function passed to iot_uart_set_callback function.
 * Reference iot_uart.h for definication.
 */
static void prvReadWriteCallback( IotUARTOperationStatus_t xOpStatus,
                                  void * pvParams );

/*-----------------------------------------------------------*/

/**
 * @brief Device power on function defined in device_control.c.
 */
extern CellularCommInterfaceError_t CellularDevice_PowerOn( void );

/**
 * @brief Device power off function defined in device_control.c.
 */
extern void CellularDevice_PowerOff( void );

/*-----------------------------------------------------------*/

/**
 * @brief Comm interface for cellular HAL. Reference cellular_comm_interface.h for definition.
 */
CellularCommInterface_t CellularCommInterface =
{
    .open  = prvCellularOpen,
    .send  = prvCellularSend,
    .recv  = prvCellularReceive,
    .close = prvCellularClose
};

/**
 * @brief Common IO UART configuration. Reference iot_uart.h for definition.
 */
static IotUARTConfig_t _commIfUartConfig =
{
    .ulBaudrate    = CELLULAR_COMM_IF_UART_BAUDRATE,
    .ucFlowControl = 0, /* No flow control. */
    .xParity       = eUartParityNone,
    .xStopbits     = eUartStopBitsOne,
    .ucWordlength  = CELLULAR_COMM_IF_UART_WORD_LENGTH,
};

/**
 * @brief Comm interface context.
 */
static CellularCommInterfaceContext _iotCommIntfCtx = { 0 };

/*-----------------------------------------------------------*/

static void prvWriteComplete( CellularCommInterfaceContext * pIotCommIntfCtx )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult = pdPASS;

    xResult = xEventGroupSetBitsFromISR( pIotCommIntfCtx->pEventGroup,
                                         COMM_EVT_MASK_TX_DONE,
                                         &xHigherPriorityTaskWoken );

    if( xResult == pdPASS )
    {
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
}

/*-----------------------------------------------------------*/

static void prvReadComplete( CellularCommInterfaceContext * pIotCommIntfCtx )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult = pdPASS;
    CellularCommInterfaceError_t retComm = IOT_COMM_INTERFACE_SUCCESS;

    if( IotFifo_Put( &pIotCommIntfCtx->rxFifo, &pIotCommIntfCtx->uartRxChar[ 0 ] ) == true )
    {
        /* rxFifoReadingFlag indicate the reader is reading the FIFO in recv function.
         * Don't call the callback function until the reader finish read. */
        if( pIotCommIntfCtx->rxFifoReadingFlag == 0U )
        {
            if( pIotCommIntfCtx->pRecvCB != NULL )
            {
                retComm = pIotCommIntfCtx->pRecvCB( pIotCommIntfCtx->pUserData,
                                                    ( CellularCommInterfaceHandle_t ) pIotCommIntfCtx );

                if( retComm == IOT_COMM_INTERFACE_SUCCESS )
                {
                    portYIELD_FROM_ISR( pdTRUE );
                }
            }
        }
        else
        {
            xResult = xEventGroupSetBitsFromISR( pIotCommIntfCtx->pEventGroup,
                                                 COMM_EVT_MASK_RX_DONE,
                                                 &xHigherPriorityTaskWoken );

            if( xResult == pdPASS )
            {
                portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
            }
        }

        /* Re-enable RX interrupt. */
        if( iot_uart_read_async( pIotCommIntfCtx->uartHandle, &pIotCommIntfCtx->uartRxChar[ 0 ], 1U ) != IOT_UART_SUCCESS )
        {
            /* Set busy flag. The interrupt will be enabled at the send function. */
            pIotCommIntfCtx->uartBusyFlag = 1;
        }
    }
    else
    {
        /* RX buffer overrun. */
        configASSERT( pdFALSE );
    }
}

/*-----------------------------------------------------------*/

static void prvReadWriteCallback( IotUARTOperationStatus_t xOpStatus,
                                  void * pvParams )
{
    CellularCommInterfaceContext * pIotCommIntfCtx = ( CellularCommInterfaceContext * ) pvParams;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult = pdPASS;

    if( ( pIotCommIntfCtx != NULL ) && ( pIotCommIntfCtx->pEventGroup != NULL ) )
    {
        if( xOpStatus == eUartReadCompleted )
        {
            prvReadComplete( pIotCommIntfCtx );
        }
        else if( xOpStatus == eUartWriteCompleted )
        {
            prvWriteComplete( pIotCommIntfCtx );
        }
        else if( xOpStatus == eUartLastWriteFailed )
        {
            xEventGroupSetBitsFromISR( pIotCommIntfCtx->pEventGroup,
                                       COMM_EVT_MASK_TX_ABORTED,
                                       &xHigherPriorityTaskWoken );

            if( xResult == pdPASS )
            {
                portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
            }
        }
        else if( xOpStatus == eUartLastReadFailed )
        {
            xEventGroupSetBitsFromISR( pIotCommIntfCtx->pEventGroup,
                                       COMM_EVT_MASK_RX_ABORTED,
                                       &xHigherPriorityTaskWoken );

            if( xResult == pdPASS )
            {
                portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
            }
        }
        else
        {
            /* empty else. */
        }
    }
}

/*-----------------------------------------------------------*/

static CellularCommInterfaceError_t prvCellularOpen( CellularCommInterfaceReceiveCallback_t receiveCallback,
                                                     void * pUserData,
                                                     CellularCommInterfaceHandle_t * pCommInterfaceHandle )
{
    CellularCommInterfaceError_t commifStatus = IOT_COMM_INTERFACE_SUCCESS;
    int32_t uartStatus = IOT_UART_SUCCESS;

    commifStatus = CellularDevice_PowerOn();

    if( commifStatus == IOT_COMM_INTERFACE_SUCCESS )
    {
        /* Open UART device. */
        _iotCommIntfCtx.uartHandle = iot_uart_open( CELLULAR_COMM_IF_UART_PORT );

        if( _iotCommIntfCtx.uartHandle == NULL )
        {
            IotLogError( "prvCellularOpen : open uart failed" );
            commifStatus = IOT_COMM_INTERFACE_FAILURE;
        }
    }
    else
    {
        IotLogError( "prvCellularOpen : CellularDevice_PowerOn failed" );
    }

    if( commifStatus == IOT_COMM_INTERFACE_SUCCESS )
    {
        /* Set UART configuration. */
        uartStatus = iot_uart_ioctl( _iotCommIntfCtx.uartHandle, eUartSetConfig, &_commIfUartConfig );

        if( uartStatus == IOT_UART_SUCCESS )
        {
            /* Create event group for send receive callback. */
            _iotCommIntfCtx.pEventGroup = xEventGroupCreate();

            if( _iotCommIntfCtx.pEventGroup == NULL )
            {
                IotLogError( "prvCellularOpen : create event group failed" );
                commifStatus = IOT_COMM_INTERFACE_NO_MEMORY;
            }
        }
        else
        {
            IotLogError( "prvCellularOpen : uart config failed" );
            commifStatus = IOT_COMM_INTERFACE_FAILURE;
        }
    }

    if( commifStatus == IOT_COMM_INTERFACE_SUCCESS )
    {
        /* Create Fifo for read. */
        IotFifo_Init( &_iotCommIntfCtx.rxFifo, &_iotCommIntfCtx.fifoBuffer, COMM_IF_FIFO_BUFFER_SIZE,
                      sizeof( uint8_t ) );

        /* Set the callback function. */
        _iotCommIntfCtx.pRecvCB = receiveCallback;
        _iotCommIntfCtx.pUserData = pUserData;
        iot_uart_set_callback( _iotCommIntfCtx.uartHandle, prvReadWriteCallback, &_iotCommIntfCtx );
        _iotCommIntfCtx.ifOpen = true;
        *pCommInterfaceHandle = ( CellularCommInterfaceHandle_t ) &_iotCommIntfCtx;

        /* Enable RX events. */
        uartStatus = iot_uart_read_async( _iotCommIntfCtx.uartHandle, _iotCommIntfCtx.uartRxChar, 1U );

        if( uartStatus != IOT_UART_SUCCESS )
        {
            IotLogError( "prvCellularOpen : read async failed" );
            commifStatus = IOT_COMM_INTERFACE_FAILURE;
        }
    }

    if( commifStatus != IOT_COMM_INTERFACE_SUCCESS )
    {
        IotLogError( "prvCellularOpen : clean device" );

        if( _iotCommIntfCtx.ifOpen == true )
        {
            /* Clear callback. */
            _iotCommIntfCtx.pRecvCB = NULL;
            _iotCommIntfCtx.pUserData = NULL;
            iot_uart_set_callback( _iotCommIntfCtx.uartHandle, NULL, NULL );
            _iotCommIntfCtx.ifOpen = false;
            *pCommInterfaceHandle = NULL;

            /* Deinit fifo. */
            IotFifo_DeInit( &_iotCommIntfCtx.rxFifo );
        }

        if( _iotCommIntfCtx.pEventGroup != NULL )
        {
            /* Delete event group. */
            vEventGroupDelete( _iotCommIntfCtx.pEventGroup );
            _iotCommIntfCtx.pEventGroup = NULL;
        }

        if( _iotCommIntfCtx.uartHandle != NULL )
        {
            /* Close UART device. */
            ( void ) iot_uart_close( _iotCommIntfCtx.uartHandle );
            _iotCommIntfCtx.uartHandle = NULL;
        }

        CellularDevice_PowerOff();
    }

    IotLogDebug( "prvCellularOpen : status %d", commifStatus );

    return commifStatus;
}

/*-----------------------------------------------------------*/

static CellularCommInterfaceError_t prvCellularClose( CellularCommInterfaceHandle_t commInterfaceHandle )
{
    CellularCommInterfaceContext * pIotCommIntfCtx = ( CellularCommInterfaceContext * ) commInterfaceHandle;
    CellularCommInterfaceError_t commifStatus = IOT_COMM_INTERFACE_SUCCESS;
    int32_t uartStatus = IOT_UART_SUCCESS;

    if( pIotCommIntfCtx != NULL )
    {
        /* Cancel the current operation. */
        uartStatus = iot_uart_cancel( pIotCommIntfCtx->uartHandle );

        if( uartStatus == IOT_UART_INVALID_VALUE )
        {
            IotLogError( "prvCellularClose : UART device is not opened yet" );
        }

        /* Clear callback function. */
        iot_uart_set_callback( _iotCommIntfCtx.uartHandle, NULL, NULL );
        _iotCommIntfCtx.ifOpen = false;
        pIotCommIntfCtx->pRecvCB = NULL;
        pIotCommIntfCtx->pUserData = NULL;

        /* Deinit Fifo. */
        IotFifo_DeInit( &pIotCommIntfCtx->rxFifo );

        /* Delete event group. */
        if( pIotCommIntfCtx->pEventGroup != NULL )
        {
            vEventGroupDelete( pIotCommIntfCtx->pEventGroup );
            pIotCommIntfCtx->pEventGroup = NULL;
        }

        /* Close UART device. */
        if( pIotCommIntfCtx->uartHandle != NULL )
        {
            uartStatus = iot_uart_close( pIotCommIntfCtx->uartHandle );

            if( uartStatus == IOT_UART_INVALID_VALUE )
            {
                IotLogError( "prvCellularClose : UART device is not opened yet" );
            }

            pIotCommIntfCtx->uartHandle = NULL;
        }

        /* Power off. */
        CellularDevice_PowerOff();
    }

    return commifStatus;
}

/*-----------------------------------------------------------*/

/* prvCellularReceive waits for the data at most timeoutMilliseconds.
 * This function returns after timeoutMilliseconds if no data is received.
 * If the data is received from FIFO, this function will continue to read data
 * until the elapsed time exceeds the timeoutMilliseconds or there is no more
 * data received from FIFO during DEFAULT_RECV_WAIT_INTERVAL.
 */
static CellularCommInterfaceError_t prvCellularReceive( CellularCommInterfaceHandle_t commInterfaceHandle,
                                                        uint8_t * pBuffer,
                                                        uint32_t bufferLength,
                                                        uint32_t timeoutMilliseconds,
                                                        uint32_t * pDataReceivedLength )
{
    CellularCommInterfaceError_t ret = IOT_COMM_INTERFACE_SUCCESS;
    CellularCommInterfaceContext * pIotCommIntfCtx = ( CellularCommInterfaceContext * ) commInterfaceHandle;
    const uint32_t waitInterval = DEFAULT_RECV_WAIT_INTERVAL;
    EventBits_t uxBits = 0;
    uint8_t rxChar = 0;
    uint32_t rxCount = 0;
    uint32_t waitTimeMs = 0, elapsedTimeMs = 0;
    uint32_t remainTimeMs = timeoutMilliseconds;
    uint32_t startTimeMs = TICKS_TO_MS( xTaskGetTickCount() );

    if( ( pIotCommIntfCtx == NULL ) || ( pBuffer == NULL ) || ( bufferLength == 0 ) )
    {
        IotLogError( "pIotCommIntfCtx, pBuffer or bufferLength is invalid" );
        ret = IOT_COMM_INTERFACE_BAD_PARAMETER;
    }
    else if( pIotCommIntfCtx->ifOpen == false )
    {
        IotLogError( "pIotCommIntfCtx is not opened before" );
        ret = IOT_COMM_INTERFACE_BAD_PARAMETER;
    }
    else
    {
        /* Set this flag to inform interrupt handler to stop calling callback function. */
        pIotCommIntfCtx->rxFifoReadingFlag = 1U;

        ( void ) xEventGroupClearBits( pIotCommIntfCtx->pEventGroup,
                                       COMM_EVT_MASK_RX_DONE |
                                       COMM_EVT_MASK_RX_ERROR |
                                       COMM_EVT_MASK_RX_ABORTED );

        while( rxCount < bufferLength )
        {
            /* If data received, reset timeout. */
            if( IotFifo_Get( &pIotCommIntfCtx->rxFifo, &rxChar ) == true )
            {
                *pBuffer = rxChar;
                pBuffer++;
                rxCount++;
            }
            else if( remainTimeMs > 0U )
            {
                if( rxCount > 0U )
                {
                    /* If bytes received, wait at most waitInterval. */
                    waitTimeMs = ( remainTimeMs > waitInterval ) ? waitInterval : remainTimeMs;
                }
                else
                {
                    waitTimeMs = remainTimeMs;
                }

                uxBits = xEventGroupWaitBits( pIotCommIntfCtx->pEventGroup,
                                              COMM_EVT_MASK_RX_DONE |
                                              COMM_EVT_MASK_RX_ERROR |
                                              COMM_EVT_MASK_RX_ABORTED,
                                              pdTRUE,
                                              pdFALSE,
                                              pdMS_TO_TICKS( waitTimeMs ) );

                if( ( uxBits & ( COMM_EVT_MASK_RX_ERROR | COMM_EVT_MASK_RX_ABORTED ) ) != 0U )
                {
                    ret = IOT_COMM_INTERFACE_DRIVER_ERROR;
                }
                else if( uxBits == 0U )
                {
                    ret = IOT_COMM_INTERFACE_TIMEOUT;
                }
                else
                {
                    elapsedTimeMs = TICKS_TO_MS( xTaskGetTickCount() ) - startTimeMs;

                    if( timeoutMilliseconds > elapsedTimeMs )
                    {
                        remainTimeMs = timeoutMilliseconds - elapsedTimeMs;
                    }
                    else
                    {
                        remainTimeMs = 0U;
                    }
                }
            }
            else
            {
                /* The timeout case. */
                ret = IOT_COMM_INTERFACE_TIMEOUT;
            }

            if( ret != IOT_COMM_INTERFACE_SUCCESS )
            {
                break;
            }
        }

        /* Clear this flag to inform interrupt handler to call callback function. */
        pIotCommIntfCtx->rxFifoReadingFlag = 0U;

        *pDataReceivedLength = rxCount;

        /* Return success if bytes received. Even if timeout or RX error. */
        if( rxCount > 0 )
        {
            ret = IOT_COMM_INTERFACE_SUCCESS;
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

static CellularCommInterfaceError_t prvCellularSend( CellularCommInterfaceHandle_t commInterfaceHandle,
                                                     const uint8_t * pData,
                                                     uint32_t dataLength,
                                                     uint32_t timeoutMilliseconds,
                                                     uint32_t * pDataSentLength )
{
    CellularCommInterfaceError_t ret = IOT_COMM_INTERFACE_BUSY;
    CellularCommInterfaceContext * pIotCommIntfCtx = ( CellularCommInterfaceContext * ) commInterfaceHandle;
    uint32_t timeoutCountLimit = ( timeoutMilliseconds / DEFAULT_WAIT_INTERVAL ) + 1;
    uint32_t timeoutCount = timeoutCountLimit;
    EventBits_t uxBits = 0;
    uint32_t transferSize = 0;
    int32_t uartStatus = IOT_UART_SUCCESS;
    uint8_t * const pLocalData = ( uint8_t * const ) pData;

    if( ( pIotCommIntfCtx == NULL ) || ( pData == NULL ) || ( dataLength == 0 ) )
    {
        IotLogError( "pIotCommIntfCtx, pData or dataLength is invalid" );
        ret = IOT_COMM_INTERFACE_BAD_PARAMETER;
    }
    else if( pIotCommIntfCtx->ifOpen == false )
    {
        IotLogError( "pIotCommIntfCtx is not opened before" );
        ret = IOT_COMM_INTERFACE_BAD_PARAMETER;
    }
    else
    {
        ( void ) xEventGroupClearBits( pIotCommIntfCtx->pEventGroup,
                                       COMM_EVT_MASK_TX_DONE |
                                       COMM_EVT_MASK_TX_ERROR |
                                       COMM_EVT_MASK_TX_ABORTED );

        while( timeoutCount > 0 )
        {
            uartStatus = iot_uart_write_async( pIotCommIntfCtx->uartHandle, pLocalData, dataLength );

            if( uartStatus == IOT_UART_SUCCESS )
            {
                uxBits = xEventGroupWaitBits( pIotCommIntfCtx->pEventGroup,
                                              COMM_EVT_MASK_TX_DONE |
                                              COMM_EVT_MASK_TX_ERROR |
                                              COMM_EVT_MASK_TX_ABORTED,
                                              pdTRUE,
                                              pdFALSE,
                                              pdMS_TO_TICKS( timeoutMilliseconds ) );

                if( ( uxBits & ( COMM_EVT_MASK_TX_ERROR | COMM_EVT_MASK_TX_ABORTED ) ) != 0U )
                {
                    transferSize = 0UL;
                    ret = IOT_COMM_INTERFACE_DRIVER_ERROR;
                }
                else
                {
                    uartStatus = iot_uart_ioctl( pIotCommIntfCtx->uartHandle, eGetTxNoOfbytes, &transferSize );

                    if( uartStatus != IOT_UART_SUCCESS )
                    {
                        IotLogError( "iot_uart_ioctl eGetTxNoOfbytes returns error %d", uartStatus );
                        ret = IOT_COMM_INTERFACE_DRIVER_ERROR;
                    }
                    else if( transferSize >= dataLength )
                    {
                        ret = IOT_COMM_INTERFACE_SUCCESS;
                    }
                    else
                    {
                        ret = IOT_COMM_INTERFACE_TIMEOUT;
                    }
                }

                if( ( pDataSentLength != NULL ) && ( ret == IOT_COMM_INTERFACE_SUCCESS ) )
                {
                    *pDataSentLength = transferSize;
                }

                break;
            }
            else
            {
                timeoutCount--;
                vTaskDelay( pdMS_TO_TICKS( DEFAULT_WAIT_INTERVAL ) );
            }
        }

        /* Enable UART RX interrupt if not enabled due to driver busy. */
        if( pIotCommIntfCtx->uartBusyFlag == 1 )
        {
            timeoutCount = DEFAULT_RECV_TIMEOUT / DEFAULT_WAIT_INTERVAL;

            while( timeoutCount > 0 )
            {
                uartStatus = iot_uart_read_async( pIotCommIntfCtx->uartHandle, pIotCommIntfCtx->uartRxChar, 1U );

                if( uartStatus == IOT_UART_SUCCESS )
                {
                    pIotCommIntfCtx->uartBusyFlag = 0;
                    break;
                }
                else
                {
                    timeoutCount--;
                    vTaskDelay( pdMS_TO_TICKS( DEFAULT_WAIT_INTERVAL ) );
                }
            }
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/
