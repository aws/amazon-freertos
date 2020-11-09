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

#include "platform/iot_threads.h"
#include "iot_fifo.h"
#include "event_groups.h"

/* Cellular includes. */
#include "aws_cellular_config.h"
#include "cellular_config_defaults.h"
#include "cellular_comm_interface.h"

#include "stm32l4xx.h"

/* Configure logs for the functions in this file. */
#ifdef IOT_LOG_LEVEL_GLOBAL
    #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
#else
    #define LIBRARY_LOG_LEVEL    IOT_LOG_NONE
#endif

#define LIBRARY_LOG_NAME    ( "COMM_IF_ST" )
#include "iot_logging_setup.h"

/*-----------------------------------------------------------*/

/* CELLULAR uses UART4 as main communication */
#define CELLULAR_UART_MAIN               UART4
#define CELLULAR_UART_MAIN_IRQn          UART4_IRQn
#define CELLULAR_UART_MAIN_CLK_ENABLE    __HAL_RCC_UART4_CLK_ENABLE
#define CELLULAR_UART_MAIN_CLK_DISABLE   __HAL_RCC_UART4_CLK_DISABLE

/**
 * @brief Comm events.
 */
#define COMM_EVT_MASK_TX_DONE       ( 0x0001U )
#define COMM_EVT_MASK_TX_ERROR      ( 0x0002U )
#define COMM_EVT_MASK_TX_ABORTED    ( 0x0004U )
#define COMM_EVT_MASK_RX_DONE       ( 0x0008U )
#define COMM_EVT_MASK_RX_ERROR      ( 0x0010U )
#define COMM_EVT_MASK_RX_ABORTED    ( 0x0020U )

#define DEFAULT_WAIT_INTERVAL       ( 20UL )
#define DEFAULT_RECV_WAIT_INTERVAL  ( 5UL )
#define DEFAULT_RECV_TIMEOUT        ( 1000UL )

#define COMM_IF_FIFO_BUFFER_SIZE                ( 1600 )

#define TICKS_TO_MS( xTicks )                   ( ( ( xTicks ) * 1000U ) / ( ( uint32_t ) configTICK_RATE_HZ ) )

/*-----------------------------------------------------------*/

/**
 * @brief A context of the communication interface.
 */
typedef struct CellularCommInterfaceContextStruct
{
    UART_HandleTypeDef * pPhy;                      /**< Handle of physical interface. */
    EventGroupHandle_t pEventGroup;                 /**< EventGroup for processing tx/rx. */
    uint8_t uartRxChar[ 1 ];                        /**< Single buffer to put a received byte at RX ISR. */
    uint32_t lastErrorCode;                         /**< Last error codes (bit-wised) of physical interface. */
    uint8_t uartBusyFlag;                           /**< Flag for whether the physical interface is busy or not. */
    CellularCommInterfaceReceiveCallback_t pRecvCB; /**< Callback function of notify RX data. */
    void * pUserData;                               /**< Userdata to be provided in the callback. */
    IotFifo_t rxFifo;                               /**< Ring buffer for put and get received data. */
    uint8_t fifoBuffer[ COMM_IF_FIFO_BUFFER_SIZE ]; /**< Buffer for ring buffer. */
    uint8_t rxFifoReadingFlag;                      /**< Flag for whether the receiver is currently reading the buffer. */
    bool ifOpen;                                    /**< Communicate interface open status. */
} CellularCommInterfaceContext;

/*-----------------------------------------------------------*/

extern void CellularDevice_Init( void );
extern CellularCommInterfaceError_t CellularDevice_PowerOn( void );
extern void CellularDevice_PowerOff( void );

/*-----------------------------------------------------------*/

static CellularCommInterfaceError_t prvCellularOpen( CellularCommInterfaceReceiveCallback_t receiveCallback,
                                           void * pUserData,
                                           CellularCommInterfaceHandle_t * pCommInterfaceHandle );
static CellularCommInterfaceError_t prvCellularClose( CellularCommInterfaceHandle_t commInterfaceHandle );
static CellularCommInterfaceError_t prvCellularReceive( CellularCommInterfaceHandle_t commInterfaceHandle,
                                              uint8_t * pBuffer,
                                              uint32_t bufferLength,
                                              uint32_t timeoutMilliseconds,
                                              uint32_t * pDataReceivedLength );
static CellularCommInterfaceError_t prvCellularSend( CellularCommInterfaceHandle_t commInterfaceHandle,
                                           const uint8_t * pData,
                                           uint32_t dataLength,
                                           uint32_t timeoutMilliseconds,
                                           uint32_t * pDataSentLength );

/*-----------------------------------------------------------*/

/* Static Linked communication interface. */
/* This variable is defined as communication interface. */
/* coverity[misra_c_2012_rule_8_7_violation]. */
CellularCommInterface_t CellularCommInterface =
{
    .open = prvCellularOpen,
    .send = prvCellularSend,
    .recv = prvCellularReceive,
    .close = prvCellularClose
};

/*-----------------------------------------------------------*/

static CellularCommInterfaceContext _iotCommIntfCtx = { 0 };
static UART_HandleTypeDef _iotCommIntfPhy = { 0 };

/*-----------------------------------------------------------*/

/* Override HAL_UART_MspInit() */
void HAL_UART_MspInit( UART_HandleTypeDef * hUart )
{
    if( ( hUart != NULL ) && ( hUart->Instance == CELLULAR_UART_MAIN ) )
    {
        /* Enable UART Clock */
        CELLULAR_UART_MAIN_CLK_ENABLE();

        /* UART interrupt init */
        HAL_NVIC_SetPriority (CELLULAR_UART_MAIN_IRQn, 5, 0 );
        HAL_NVIC_EnableIRQ( CELLULAR_UART_MAIN_IRQn );
    }
}

/* Override HAL_UART_MspDeInit() */
void HAL_UART_MspDeInit( UART_HandleTypeDef * hUart )
{
    if( ( hUart != NULL ) && ( hUart->Instance == CELLULAR_UART_MAIN ) )
    {
        /* UART interrupt deinit */
        HAL_NVIC_DisableIRQ( CELLULAR_UART_MAIN_IRQn );

        /* Disable UART Clock */
        CELLULAR_UART_MAIN_CLK_DISABLE();
    }
}

/* Override HAL_UART_TxCpltCallback */
void HAL_UART_TxCpltCallback( UART_HandleTypeDef * hUart )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult = pdPASS;
    CellularCommInterfaceContext * pIotCommIntfCtx = & _iotCommIntfCtx;

    if( hUart != NULL )
    {
        xResult = xEventGroupSetBitsFromISR( pIotCommIntfCtx->pEventGroup,
                                             COMM_EVT_MASK_TX_DONE,
                                             & xHigherPriorityTaskWoken );
        if( xResult == pdPASS )
        {
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        }
    }
}

/* Override HAL_UART_RxCpltCallback() */
void HAL_UART_RxCpltCallback( UART_HandleTypeDef * hUart )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult = pdPASS;
    CellularCommInterfaceContext * pIotCommIntfCtx = & _iotCommIntfCtx;
    CellularCommInterfaceError_t retComm = IOT_COMM_INTERFACE_SUCCESS;

    if( hUart != NULL )
    {
        if( IotFifo_Put( & pIotCommIntfCtx->rxFifo, & pIotCommIntfCtx->uartRxChar[ 0 ] ) == true )
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
                                                     & xHigherPriorityTaskWoken );
                if( xResult == pdPASS )
                {
                    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
                }
            }

            /* Re-enable RX interrupt */
            if( HAL_UART_Receive_IT( pIotCommIntfCtx->pPhy, & pIotCommIntfCtx->uartRxChar[ 0 ], 1U ) != HAL_OK )
            {
                /* Set busy flag. The interrupt will be enabled at the send function */
                pIotCommIntfCtx->uartBusyFlag = 1;
            }
        }
        else
        {
            /* RX buffer overrun. */
            configASSERT( pdFALSE );
        }
    }
}

/* Override HAL_UART_ErrorCallback(). */
void HAL_UART_ErrorCallback( UART_HandleTypeDef * hUart )
{
    CellularCommInterfaceContext * pIotCommIntfCtx = & _iotCommIntfCtx;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult = pdPASS;

    if( ( hUart != NULL ) && ( pIotCommIntfCtx->pEventGroup != NULL ) )
    {
        pIotCommIntfCtx->lastErrorCode = hUart->ErrorCode;
        if( ( hUart->ErrorCode &
            ( HAL_UART_ERROR_PE | /*!< Parity error        */
              HAL_UART_ERROR_NE | /*!< Noise error         */
              HAL_UART_ERROR_FE | /*!< frame error         */
              HAL_UART_ERROR_ORE  /*!< Overrun error       */
            ) ) != 0 )
        {
            xHigherPriorityTaskWoken = pdFALSE;
            xResult = xEventGroupSetBitsFromISR( pIotCommIntfCtx->pEventGroup,
                                                 COMM_EVT_MASK_RX_ERROR,
                                                 & xHigherPriorityTaskWoken );
            if( xResult == pdPASS )
            {
                portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
            }
        }

        /* Re-enable RX interrupt */
        if( HAL_UART_Receive_IT( pIotCommIntfCtx->pPhy,
            & pIotCommIntfCtx->uartRxChar[ 0 ], 1U ) != HAL_OK )
        {
            /* Set busy flag. The interrupt will be enabled at the send function */
            pIotCommIntfCtx->uartBusyFlag = 1;
        }
    }
}

/* Override HAL_UART_AbortCpltCallback() */
void HAL_UART_AbortCpltCallback( UART_HandleTypeDef * hUart )
{
    CellularCommInterfaceContext * pIotCommIntfCtx = & _iotCommIntfCtx;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult = pdPASS;

    if( ( hUart != NULL ) && ( pIotCommIntfCtx->pEventGroup != NULL ) )
    {
        xHigherPriorityTaskWoken = pdFALSE;
        xResult = xEventGroupSetBitsFromISR( pIotCommIntfCtx->pEventGroup,
                                             COMM_EVT_MASK_TX_ABORTED |
                                             COMM_EVT_MASK_RX_ABORTED,
                                             & xHigherPriorityTaskWoken );
        if( xResult == pdPASS )
        {
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        }
    }
}

/* Override HAL_UART_AbortTransmitCpltCallback() */
void HAL_UART_AbortTransmitCpltCallback( UART_HandleTypeDef * hUart )
{
    CellularCommInterfaceContext * pIotCommIntfCtx = & _iotCommIntfCtx;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult = pdPASS;

    if( ( hUart != NULL ) && ( pIotCommIntfCtx->pEventGroup != NULL ) )
    {
        xHigherPriorityTaskWoken = pdFALSE;
        xResult = xEventGroupSetBitsFromISR( pIotCommIntfCtx->pEventGroup,
                                             COMM_EVT_MASK_TX_ABORTED,
                                             & xHigherPriorityTaskWoken );
        if( xResult == pdPASS )
        {
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        }
    }
}

/* Override HAL_UART_AbortReceiveCpltCallback() */
void HAL_UART_AbortReceiveCpltCallback( UART_HandleTypeDef * hUart )
{
    CellularCommInterfaceContext * pIotCommIntfCtx = & _iotCommIntfCtx;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE, xResult = pdPASS;

    if( ( hUart != NULL ) && ( pIotCommIntfCtx->pEventGroup != NULL ) )
    {
        xHigherPriorityTaskWoken = pdFALSE;
        xResult = xEventGroupSetBitsFromISR( pIotCommIntfCtx->pEventGroup,
                                             COMM_EVT_MASK_RX_ABORTED,
                                             & xHigherPriorityTaskWoken );
        if( xResult == pdPASS )
        {
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        }
    }
}

/* Override UART4_IRQHandler() */
void UART4_IRQHandler( void )
{
    if( _iotCommIntfCtx.pPhy != NULL )
    {
        HAL_UART_IRQHandler( _iotCommIntfCtx.pPhy );
    }
}

static HAL_StatusTypeDef prvCellularUartInit( UART_HandleTypeDef * hUart )
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    if( hUart != NULL )
    {
        hUart->Instance = CELLULAR_UART_MAIN;
        hUart->Init.BaudRate = 115200;
        hUart->Init.WordLength = UART_WORDLENGTH_8B;
        hUart->Init.StopBits = UART_STOPBITS_1;
        hUart->Init.Parity = UART_PARITY_NONE;
        hUart->Init.Mode = UART_MODE_TX_RX;
        hUart->Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
        hUart->Init.OverSampling = UART_OVERSAMPLING_16;
        hUart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
        hUart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
        ret = HAL_UART_Init( hUart );
    }

    return ret;
}

static HAL_StatusTypeDef prvCellularUartDeInit( UART_HandleTypeDef * hUart )
{
    HAL_StatusTypeDef ret = HAL_ERROR;

    if( hUart != NULL )
    {
        ret = HAL_UART_DeInit( hUart );
    }

    return ret;
}

static CellularCommInterfaceError_t prvCellularOpen( CellularCommInterfaceReceiveCallback_t receiveCallback,
                                           void * pUserData,
                                           CellularCommInterfaceHandle_t * pCommInterfaceHandle )
{
    CellularCommInterfaceError_t ret = IOT_COMM_INTERFACE_SUCCESS;
    CellularCommInterfaceContext * pIotCommIntfCtx = & _iotCommIntfCtx;

    /* check input parameter. */
    if( ( pCommInterfaceHandle == NULL ) || ( receiveCallback == NULL ) )
    {
        ret = IOT_COMM_INTERFACE_BAD_PARAMETER;
    }
    else if( pIotCommIntfCtx->ifOpen == true )
    {
        ret = IOT_COMM_INTERFACE_FAILURE;
    }
    else
    {
        /* Initialize the context structure. */
        ( void ) memset( pIotCommIntfCtx, 0, sizeof( CellularCommInterfaceContext ) );
    }

    if( ret == IOT_COMM_INTERFACE_SUCCESS )
    {
        /* Power on the device. */
        ret = CellularDevice_PowerOn();
    }

    /* Setup the read FIFO. */
    if( ret == IOT_COMM_INTERFACE_SUCCESS )
    {
        IotFifo_Init( & pIotCommIntfCtx->rxFifo, & pIotCommIntfCtx->fifoBuffer, COMM_IF_FIFO_BUFFER_SIZE,
            sizeof( uint8_t ) );
        pIotCommIntfCtx->pEventGroup = xEventGroupCreate();
        if( pIotCommIntfCtx->pEventGroup == NULL )
        {
            IotLogError( "EventGroup create failed" );
            IotFifo_DeInit( & pIotCommIntfCtx->rxFifo );
            CellularDevice_PowerOff();
            ret = IOT_COMM_INTERFACE_NO_MEMORY;
        }
    }

    /* Setup the phy device. */
    if( ret == IOT_COMM_INTERFACE_SUCCESS )
    {
        pIotCommIntfCtx->pPhy = & _iotCommIntfPhy;
        ( void ) memset( pIotCommIntfCtx->pPhy, 0, sizeof( UART_HandleTypeDef ) );
        if( prvCellularUartInit( pIotCommIntfCtx->pPhy ) != HAL_OK )
        {
            IotLogError( "UART init failed" );
            vEventGroupDelete( pIotCommIntfCtx->pEventGroup );
            IotFifo_DeInit( & pIotCommIntfCtx->rxFifo );
            CellularDevice_PowerOff();
            ret = IOT_COMM_INTERFACE_DRIVER_ERROR;
        }
    }

    /* setup callback function and userdata. */
    if( ret == IOT_COMM_INTERFACE_SUCCESS )
    {
        pIotCommIntfCtx->pRecvCB = receiveCallback;
        pIotCommIntfCtx->pUserData = pUserData;
        * pCommInterfaceHandle = ( CellularCommInterfaceHandle_t ) pIotCommIntfCtx;

        /* Enable UART RX interrupt. */
        if( HAL_UART_Receive_IT( pIotCommIntfCtx->pPhy, & pIotCommIntfCtx->uartRxChar[ 0 ], 1U ) != HAL_OK )
        {
            /* Set busy flag. The interrupt will be enabled at the send function. */
            pIotCommIntfCtx->uartBusyFlag = 1;
        }
        pIotCommIntfCtx->ifOpen = true;
    }

    return ret;
}

static CellularCommInterfaceError_t prvCellularClose( CellularCommInterfaceHandle_t commInterfaceHandle )
{
    CellularCommInterfaceError_t ret = IOT_COMM_INTERFACE_BAD_PARAMETER;
    CellularCommInterfaceContext * pIotCommIntfCtx = ( CellularCommInterfaceContext * ) commInterfaceHandle;

    if( pIotCommIntfCtx == NULL )
    {
        ret = IOT_COMM_INTERFACE_BAD_PARAMETER;
    }
    else if( pIotCommIntfCtx->ifOpen == false )
    {
        ret = IOT_COMM_INTERFACE_FAILURE;
    }
    else
    {
        /* Disable UART RX and TX interrupts */
        if( pIotCommIntfCtx->pPhy != NULL )
        {
            ( void ) HAL_UART_Abort_IT( pIotCommIntfCtx->pPhy );
            /* Wait for abort complete event */
            ( void ) xEventGroupWaitBits( pIotCommIntfCtx->pEventGroup,
                                          COMM_EVT_MASK_TX_ABORTED | COMM_EVT_MASK_RX_ABORTED,
                                          pdTRUE, pdTRUE, pdMS_TO_TICKS( 500 ) );

            ( void ) prvCellularUartDeInit( pIotCommIntfCtx->pPhy );
            pIotCommIntfCtx->pPhy = NULL;
        }

        /* Clean event group. */
        if( pIotCommIntfCtx->pEventGroup != NULL )
        {
            vEventGroupDelete( pIotCommIntfCtx->pEventGroup );
            pIotCommIntfCtx->pEventGroup = NULL;
        }

        /* Clean the rx FIFO. This API only clean the data structure. */
        IotFifo_DeInit( & pIotCommIntfCtx->rxFifo );

        /* Power off. */
        CellularDevice_PowerOff();

        /* Set the device open status to false. */
        pIotCommIntfCtx->ifOpen = false;

        ret = IOT_COMM_INTERFACE_SUCCESS;
    }

    return ret;
}

static CellularCommInterfaceError_t prvCellularSend( CellularCommInterfaceHandle_t commInterfaceHandle,
                                           const uint8_t * pData,
                                           uint32_t dataLength,
                                           uint32_t timeoutMilliseconds,
                                           uint32_t * pDataSentLength )
{
    CellularCommInterfaceError_t ret = IOT_COMM_INTERFACE_BUSY;
    HAL_StatusTypeDef err = HAL_OK;
    CellularCommInterfaceContext * pIotCommIntfCtx = ( CellularCommInterfaceContext * ) commInterfaceHandle;
    uint32_t timeoutCountLimit = ( timeoutMilliseconds / DEFAULT_WAIT_INTERVAL ) + 1;
    uint32_t timeoutCount = timeoutCountLimit;
    EventBits_t uxBits = 0;
    uint32_t transferSize = 0;

    if( ( pIotCommIntfCtx == NULL ) || ( pData == NULL ) || ( dataLength == 0 ) )
    {
        ret = IOT_COMM_INTERFACE_BAD_PARAMETER;
    }
    else if( pIotCommIntfCtx->ifOpen == false )
    {
        ret = IOT_COMM_INTERFACE_FAILURE;
    }
    else
    {
        ( void ) xEventGroupClearBits( pIotCommIntfCtx->pEventGroup,
                                       COMM_EVT_MASK_TX_DONE |
                                       COMM_EVT_MASK_TX_ERROR |
                                       COMM_EVT_MASK_TX_ABORTED );

        while( timeoutCount > 0 )
        {
            err = HAL_UART_Transmit_IT( pIotCommIntfCtx->pPhy, ( uint8_t * ) pData, ( uint16_t ) dataLength );
            if( err != HAL_BUSY )
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
                    transferSize = ( uint32_t ) ( pIotCommIntfCtx->pPhy->TxXferSize - pIotCommIntfCtx->pPhy->TxXferCount );
                    if( transferSize >= dataLength )
                    {
                        ret = IOT_COMM_INTERFACE_SUCCESS;
                    }
                    else
                    {
                        ret = IOT_COMM_INTERFACE_TIMEOUT;
                    }
                }

                if( pDataSentLength != NULL )
                {
                    * pDataSentLength = transferSize;
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
                err = HAL_UART_Receive_IT( pIotCommIntfCtx->pPhy, & pIotCommIntfCtx->uartRxChar[ 0 ], 1U );
                if( err == HAL_OK )
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
        ret = IOT_COMM_INTERFACE_BAD_PARAMETER;
    }
    else if( pIotCommIntfCtx->ifOpen == false )
    {
        ret = IOT_COMM_INTERFACE_FAILURE;
    }
    else
    {
        /* Set this flag to inform interrupt handler to stop callling callback function. */
        pIotCommIntfCtx->rxFifoReadingFlag = 1U;

        ( void ) xEventGroupClearBits( pIotCommIntfCtx->pEventGroup,
                                       COMM_EVT_MASK_RX_DONE |
                                       COMM_EVT_MASK_RX_ERROR |
                                       COMM_EVT_MASK_RX_ABORTED );

        while( rxCount < bufferLength )
        {
            /* If data received, reset timeout. */
            if( IotFifo_Get( & pIotCommIntfCtx->rxFifo, & rxChar ) == true )
            {
                * pBuffer = rxChar;
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

        * pDataReceivedLength = rxCount;
        /* Return success if bytes received. Even if timeout or RX error. */
        if( rxCount > 0 )
        {
            ret = IOT_COMM_INTERFACE_SUCCESS;
        }
    }

    return ret;
}
