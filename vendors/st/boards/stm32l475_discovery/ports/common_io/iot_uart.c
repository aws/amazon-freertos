/**
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
 * @brief file containing the implementation of UART APIs calling STM drivers.
 */
/* ST HAL API include */
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_uart.h"
#include "stm32l475e_iot01.h"

/* common-io UART include */
#include "iot_uart.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "semphr.h"

/**
 * @brief STM UART Descriptor.
 *
 */
typedef struct IotUARTDescriptor
{
    UART_HandleTypeDef * pxHuart; /**< STM UART handle for all the configuration. */
    IRQn_Type eIrqNum;            /**< Interrupt number for the UART. */
    IotUARTCallback_t xUartCallback;
    void * pvUserCallbackContext;
    SemaphoreHandle_t xSemphr;
    StaticSemaphore_t xSemphrBuffer;
    uint8_t sOpened;
} IotUARTDescriptor_t;


/**
 * @brief The number of USART ports on this ST microcontroller.
 */
#define IOT_UART_BLOCKING_TIMEOUT    ( ( uint32_t ) 3000UL )
#define IOT_UART_CLOSED              ( ( uint8_t ) 0 )
#define IOT_UART_OPENED              ( ( uint8_t ) 1 )

/**
 * @brief Statically initialized map of STM UART Handle for all 5 ports.
 *
 */
static UART_HandleTypeDef xUartHandleMap[] =
{
    {
        .Instance = USART1,
        .Init =
        {
            .BaudRate       = IOT_UART_BAUD_RATE_DEFAULT,
            .WordLength     = UART_WORDLENGTH_8B,
            .StopBits       = UART_STOPBITS_1,
            .Parity         = UART_PARITY_NONE,
            .Mode           = UART_MODE_TX_RX,
            .HwFlowCtl      = UART_HWCONTROL_NONE,
            .OverSampling   = UART_OVERSAMPLING_16,
            .OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE
        },
        .AdvancedInit =
        {
            .AdvFeatureInit = UART_ADVFEATURE_NO_INIT
        },
        .pTxBuffPtr = NULL,
        .TxXferSize = 0,
        .TxXferCount = 0,
        .pRxBuffPtr = NULL,
        .RxXferSize = 0,
        .RxXferCount = 0
    },
    {
        .Instance = USART2,
        .Init =
        {
            .BaudRate       = IOT_UART_BAUD_RATE_DEFAULT,
            .WordLength     = UART_WORDLENGTH_8B,
            .StopBits       = UART_STOPBITS_1,
            .Parity         = UART_PARITY_NONE,
            .Mode           = UART_MODE_TX_RX,
            .HwFlowCtl      = UART_HWCONTROL_NONE,
            .OverSampling   = UART_OVERSAMPLING_16,
            .OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE
        },
        .AdvancedInit =
        {
            .AdvFeatureInit = UART_ADVFEATURE_NO_INIT
        },
        .pTxBuffPtr = NULL,
        .TxXferSize = 0,
        .TxXferCount = 0,
        .pRxBuffPtr = NULL,
        .RxXferSize = 0,
        .RxXferCount = 0
    },
    {
        .Instance = USART3,
        .Init =
        {
            .BaudRate       = IOT_UART_BAUD_RATE_DEFAULT,
            .WordLength     = UART_WORDLENGTH_8B,
            .StopBits       = UART_STOPBITS_1,
            .Parity         = UART_PARITY_NONE,
            .Mode           = UART_MODE_TX_RX,
            .HwFlowCtl      = UART_HWCONTROL_NONE,
            .OverSampling   = UART_OVERSAMPLING_16,
            .OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE
        },
        .AdvancedInit =
        {
            .AdvFeatureInit = UART_ADVFEATURE_NO_INIT
        },
        .pTxBuffPtr = NULL,
        .TxXferSize = 0,
        .TxXferCount = 0,
        .pRxBuffPtr = NULL,
        .RxXferSize = 0,
        .RxXferCount = 0
    },
    {
        .Instance = UART4,
        .Init =
        {
            .BaudRate       = IOT_UART_BAUD_RATE_DEFAULT,
            .WordLength     = UART_WORDLENGTH_8B,
            .StopBits       = UART_STOPBITS_1,
            .Parity         = UART_PARITY_NONE,
            .Mode           = UART_MODE_TX_RX,
            .HwFlowCtl      = UART_HWCONTROL_NONE,
            .OverSampling   = UART_OVERSAMPLING_16,
            .OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE
        },
        .AdvancedInit =
        {
            .AdvFeatureInit = UART_ADVFEATURE_NO_INIT
        },
        .pTxBuffPtr = NULL,
        .TxXferSize = 0,
        .TxXferCount = 0,
        .pRxBuffPtr = NULL,
        .RxXferSize = 0,
        .RxXferCount = 0
    },
    {
        .Instance = UART5,
        .Init =
        {
            .BaudRate       = IOT_UART_BAUD_RATE_DEFAULT,
            .WordLength     = UART_WORDLENGTH_8B,
            .StopBits       = UART_STOPBITS_1,
            .Parity         = UART_PARITY_NONE,
            .Mode           = UART_MODE_TX_RX,
            .HwFlowCtl      = UART_HWCONTROL_NONE,
            .OverSampling   = UART_OVERSAMPLING_16,
            .OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE
        },
        .AdvancedInit =
        {
            .AdvFeatureInit = UART_ADVFEATURE_NO_INIT
        },
        .pTxBuffPtr = NULL,
        .TxXferSize = 0,
        .TxXferCount = 0,
        .pRxBuffPtr = NULL,
        .RxXferSize = 0,
        .RxXferCount = 0
    },
};

static IotUARTDescriptor_t xUart0 =
{
    .pvUserCallbackContext = NULL,
    .pxHuart               = &xUartHandleMap[ 0 ],
    .eIrqNum               = USART1_IRQn,
    .xUartCallback         = NULL,
    .xSemphr               = NULL,
    .sOpened               = IOT_UART_CLOSED,
};

static IotUARTDescriptor_t xUart1 =
{
    .pvUserCallbackContext = NULL,
    .pxHuart               = &xUartHandleMap[ 1 ],
    .eIrqNum               = USART2_IRQn,
    .xUartCallback         = NULL,
    .xSemphr               = NULL,
    .sOpened               = IOT_UART_CLOSED,
};

static IotUARTDescriptor_t xUart2 =
{
    .pvUserCallbackContext = NULL,
    .pxHuart               = &xUartHandleMap[ 2 ],
    .eIrqNum               = USART3_IRQn,
    .xUartCallback         = NULL,
    .xSemphr               = NULL,
    .sOpened               = IOT_UART_CLOSED,
};

static IotUARTDescriptor_t xUart3 =
{
    .pvUserCallbackContext = NULL,
    .pxHuart               = &xUartHandleMap[ 3 ],
    .eIrqNum               = UART4_IRQn,
    .xUartCallback         = NULL,
    .xSemphr               = NULL,
    .sOpened               = IOT_UART_CLOSED,
};

static IotUARTDescriptor_t xUart4 =
{
    .pvUserCallbackContext = NULL,
    .pxHuart               = &xUartHandleMap[ 4 ],
    .eIrqNum               = UART5_IRQn,
    .xUartCallback         = NULL,
    .xSemphr               = NULL,
    .sOpened               = IOT_UART_CLOSED,
};

static IotUARTHandle_t const pxUarts[] = { &xUart0, &xUart1, &xUart2, &xUart3, &xUart4 };

IotUARTHandle_t iot_uart_open( int32_t lUartInstance )
{
    IotUARTHandle_t xHandle = NULL;

    if( ( lUartInstance >= 0 ) && ( lUartInstance < sizeof(pxUarts) / sizeof(IotUARTHandle_t) ) )
    {
        xHandle = pxUarts[ lUartInstance ];

        if( xHandle->sOpened == IOT_UART_CLOSED )
        {
            xHandle->xSemphr = xSemaphoreCreateBinaryStatic( &( xHandle->xSemphrBuffer ) );

            /* Special setup routine for USART1 that includes relevant GPIO initialization. */
            if( pxUarts[ lUartInstance ]->pxHuart->Instance == USART1 )
            {
                BSP_COM_Init( COM1, pxUarts[ lUartInstance ]->pxHuart );
                xHandle->sOpened = IOT_UART_OPENED;
            }
            else if( HAL_UART_Init( pxUarts[ lUartInstance ]->pxHuart ) == HAL_OK )
            {
                xHandle->sOpened = IOT_UART_OPENED;
            }
            else
            {
                xHandle = NULL;
            }
        }
        else
        {
            xHandle = NULL;
        }
    }

    return xHandle;
}
/*-----------------------------------------------------------*/

void iot_uart_set_callback( IotUARTHandle_t const pxUartPeripheral,
                            IotUARTCallback_t xCallback,
                            void * pvUserCallbackContext )
{
    if( ( pxUartPeripheral != NULL ) )
    {
        pxUartPeripheral->xUartCallback = xCallback;
        pxUartPeripheral->pvUserCallbackContext = pvUserCallbackContext;
    }
}
/*-----------------------------------------------------------*/

int32_t iot_uart_read_async( IotUARTHandle_t const pxUartPeripheral,
                             uint8_t * const pvBuffer,
                             size_t xBytes )
{
    int32_t lError = IOT_UART_SUCCESS;

    if( ( pvBuffer == NULL ) || ( xBytes == 0 ) || ( pxUartPeripheral == NULL ) || ( pxUartPeripheral->sOpened == IOT_UART_CLOSED ) )
    {
        lError = IOT_UART_INVALID_VALUE;
    }
    else
    {
        if( HAL_UART_GetState( pxUartPeripheral->pxHuart ) == HAL_UART_STATE_BUSY_RX )
        {
            lError = IOT_UART_BUSY;
        }
        else
        {
            HAL_NVIC_SetPriority( pxUartPeripheral->eIrqNum, 1, 0 );
            HAL_NVIC_EnableIRQ( pxUartPeripheral->eIrqNum );

            if( HAL_UART_Receive_IT( pxUartPeripheral->pxHuart, pvBuffer, ( uint16_t ) xBytes ) != HAL_OK )
            {
                lError = IOT_UART_READ_FAILED;
            }
        }
    }

    return lError;
}
/*-----------------------------------------------------------*/

int32_t iot_uart_write_async( IotUARTHandle_t const pxUartPeripheral,
                              uint8_t * const pvBuffer,
                              size_t xBytes )
{
    int32_t lError = IOT_UART_SUCCESS;

    if( ( pvBuffer == NULL ) || ( xBytes == 0 ) || ( pxUartPeripheral == NULL ) || ( pxUartPeripheral->sOpened == IOT_UART_CLOSED ) )
    {
        lError = IOT_UART_INVALID_VALUE;
    }
    else
    {
        if( HAL_UART_GetState( pxUartPeripheral->pxHuart ) == HAL_UART_STATE_BUSY_TX )
        {
            lError = IOT_UART_BUSY;
        }
        else
        {
            HAL_NVIC_SetPriority( pxUartPeripheral->eIrqNum, 1, 0 );
            HAL_NVIC_EnableIRQ( pxUartPeripheral->eIrqNum );

            if( HAL_UART_Transmit_IT( pxUartPeripheral->pxHuart, pvBuffer, ( uint16_t ) xBytes ) != HAL_OK )
            {
                lError = IOT_UART_WRITE_FAILED;
            }
        }
    }

    return lError;
}
/*-----------------------------------------------------------*/

int32_t iot_uart_read_sync( IotUARTHandle_t const pxUartPeripheral,
                            uint8_t * const pvBuffer,
                            size_t xBytes )
{
    int32_t lError = IOT_UART_SUCCESS;

    lError = iot_uart_read_async( pxUartPeripheral, pvBuffer, xBytes );

    if( lError == IOT_UART_SUCCESS )
    {
        if( xSemaphoreTake( pxUartPeripheral->xSemphr, pdMS_TO_TICKS( IOT_UART_BLOCKING_TIMEOUT ) ) == pdFALSE )
        {
            HAL_UART_Abort( pxUartPeripheral->pxHuart );
        }
    }

    return lError;
}
/*-----------------------------------------------------------*/

int32_t iot_uart_write_sync( IotUARTHandle_t const pxUartPeripheral,
                             uint8_t * const pvBuffer,
                             size_t xBytes )
{
    int32_t lError = IOT_UART_SUCCESS;
    HAL_StatusTypeDef status = HAL_OK;

    if( ( pvBuffer == NULL ) || ( xBytes == 0 ) || ( pxUartPeripheral == NULL ) || ( pxUartPeripheral->sOpened == IOT_UART_CLOSED ) )
    {
        lError = IOT_UART_INVALID_VALUE;
    }
    else
    {
        if( HAL_UART_GetState( pxUartPeripheral->pxHuart ) == HAL_UART_STATE_BUSY_TX )
        {
            lError = IOT_UART_BUSY;
        }
        else
        {
            status = HAL_UART_Transmit( pxUartPeripheral->pxHuart, pvBuffer, ( uint16_t ) xBytes, IOT_UART_BLOCKING_TIMEOUT );

            if( status != HAL_OK )
            {
                lError = IOT_UART_WRITE_FAILED;
            }
        }
    }

    return lError;
}
/*-----------------------------------------------------------*/

int32_t iot_uart_close( IotUARTHandle_t const pxUartPeripheral )
{
    int32_t lError = IOT_UART_SUCCESS;

    if( ( pxUartPeripheral == NULL ) || ( pxUartPeripheral->sOpened != IOT_UART_OPENED ) )
    {
        lError = IOT_UART_INVALID_VALUE;
    }
    else
    {
        if( HAL_UART_Abort( pxUartPeripheral->pxHuart ) != HAL_OK )
        {
            lError = IOT_UART_BUSY;
        }
        else if( HAL_UART_DeInit( pxUartPeripheral->pxHuart ) != HAL_OK )
        {
            lError = IOT_UART_BUSY;
        }
        else
        {
            vSemaphoreDelete( pxUartPeripheral->xSemphr );
            lError = IOT_UART_SUCCESS;
            pxUartPeripheral->sOpened = IOT_UART_CLOSED;
        }
    }

    return lError;
}
/*-----------------------------------------------------------*/

int32_t iot_uart_ioctl( IotUARTHandle_t const pxUartPeripheral,
                        IotUARTIoctlRequest_t xUartRequest,
                        void * const pvBuffer )
{
    int32_t lError = IOT_UART_INVALID_VALUE;

    if( ( pxUartPeripheral != NULL ) && ( pxUartPeripheral->sOpened == IOT_UART_OPENED ) && ( pvBuffer != NULL ) )
    {
        switch( xUartRequest )
        {
            case eUartSetConfig:

                if( HAL_UART_GetState( pxUartPeripheral->pxHuart ) == HAL_UART_STATE_READY )
                {
                    /* Set the user configurations on STM UART. */
                    pxUartPeripheral->pxHuart->Init.BaudRate = ( ( IotUARTConfig_t * ) pvBuffer )->ulBaudrate;
                    pxUartPeripheral->pxHuart->Init.Parity = ( ( IotUARTConfig_t * ) pvBuffer )->xParity;
                    pxUartPeripheral->pxHuart->Init.WordLength = ( ( IotUARTConfig_t * ) pvBuffer )->ucWordlength;
                    pxUartPeripheral->pxHuart->Init.StopBits = ( ( IotUARTConfig_t * ) pvBuffer )->xStopbits;
                    pxUartPeripheral->pxHuart->Init.HwFlowCtl = ( ( IotUARTConfig_t * ) pvBuffer )->ucFlowControl;

                    if( UART_SetConfig( pxUartPeripheral->pxHuart ) == HAL_OK )
                    {
                        lError = IOT_UART_SUCCESS;
                    }
                }
                else
                {
                    lError = IOT_UART_BUSY;
                }

                break;

            case eUartGetConfig:

                ( ( IotUARTConfig_t * ) pvBuffer )->ulBaudrate = pxUartPeripheral->pxHuart->Init.BaudRate;
                ( ( IotUARTConfig_t * ) pvBuffer )->xParity = pxUartPeripheral->pxHuart->Init.Parity;
                ( ( IotUARTConfig_t * ) pvBuffer )->ucWordlength = pxUartPeripheral->pxHuart->Init.WordLength;
                ( ( IotUARTConfig_t * ) pvBuffer )->xStopbits = pxUartPeripheral->pxHuart->Init.StopBits;
                ( ( IotUARTConfig_t * ) pvBuffer )->ucFlowControl = pxUartPeripheral->pxHuart->Init.HwFlowCtl;

                lError = IOT_UART_SUCCESS;

                break;

            case eGetTxNoOfbytes:

                *( int32_t * ) pvBuffer = ( int32_t ) ( pxUartPeripheral->pxHuart->TxXferSize - pxUartPeripheral->pxHuart->TxXferCount );
                lError = IOT_UART_SUCCESS;

                break;

            case eGetRxNoOfbytes:

                *( int32_t * ) pvBuffer = ( int32_t ) ( pxUartPeripheral->pxHuart->RxXferSize - pxUartPeripheral->pxHuart->RxXferCount );
                lError = IOT_UART_SUCCESS;

                break;

            default:
                break;
        }
    }

    return lError;
}
/*-----------------------------------------------------------*/

int32_t iot_uart_cancel( IotUARTHandle_t const pxUartPeripheral )
{
    int32_t lError = IOT_UART_INVALID_VALUE;

    if( ( pxUartPeripheral != NULL ) && ( pxUartPeripheral->sOpened == IOT_UART_OPENED ) )
    {
        if( HAL_UART_GetState( pxUartPeripheral->pxHuart ) == HAL_UART_STATE_READY )
        {
            lError = IOT_UART_NOTHING_TO_CANCEL;
        }
        else if( HAL_UART_Abort( pxUartPeripheral->pxHuart ) == HAL_OK )
        {
            lError = IOT_UART_SUCCESS;
        }
    }

    return lError;
}
/*-----------------------------------------------------------*/

void HAL_UART_ErrorCallback( UART_HandleTypeDef * huart )
{
    /* Propagate error message back to user.
     * Does not use common-io standard API that signals if error was write or read,
     * but this is not possible with ST API in Full-Duplex mode, as it is unknown if error came from Rx or Tx */
    uint32_t error = HAL_UART_GetError( huart );
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    uint32_t i = 0;

    for( ; i < sizeof( xUartHandleMap ) / sizeof( UART_HandleTypeDef ); i++ )
    {
        if( huart->Instance == xUartHandleMap[ i ].Instance )
        {
            if( pxUarts[ i ]->xUartCallback != NULL )
            {
                pxUarts[ i ]->xUartCallback( huart->ErrorCode, pxUarts[ i ]->pvUserCallbackContext );
            }

            /* Clear overrun error flag. */
            if( error == HAL_UART_ERROR_ORE )
            {
                __HAL_UART_CLEAR_OREFLAG( huart );
            }
            else if( error == HAL_UART_ERROR_FE )
            {
                __HAL_UART_CLEAR_FEFLAG( huart );
            }

            xSemaphoreGiveFromISR( pxUarts[ i ]->xSemphr, &higherPriorityTaskWoken );
            portYIELD_FROM_ISR( higherPriorityTaskWoken );
            break;
        }
    }
}
/*-----------------------------------------------------------*/

void HAL_UART_RxCpltCallback( UART_HandleTypeDef * huart )
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    uint32_t i = 0;

    for( ; i < sizeof( xUartHandleMap ) / sizeof( UART_HandleTypeDef ); i++ )
    {
        if( huart->Instance == xUartHandleMap[ i ].Instance )
        {
            if( pxUarts[ i ]->xUartCallback != NULL )
            {
                pxUarts[ i ]->xUartCallback( eUartReadCompleted, pxUarts[ i ]->pvUserCallbackContext );
            }

            xSemaphoreGiveFromISR( pxUarts[ i ]->xSemphr, &higherPriorityTaskWoken );
            portYIELD_FROM_ISR( higherPriorityTaskWoken );
            break;
        }
    }
}
/*-----------------------------------------------------------*/

void HAL_UART_TxCpltCallback( UART_HandleTypeDef * huart )
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    uint32_t i = 0;

    for( ; i < sizeof( xUartHandleMap ) / sizeof( UART_HandleTypeDef ); i++ )
    {
        if( huart->Instance == xUartHandleMap[ i ].Instance )
        {
            if( pxUarts[ i ]->xUartCallback != NULL )
            {
                pxUarts[ i ]->xUartCallback( eUartWriteCompleted, pxUarts[ i ]->pvUserCallbackContext );
            }

            xSemaphoreGiveFromISR( pxUarts[ i ]->xSemphr, &higherPriorityTaskWoken );
            portYIELD_FROM_ISR( higherPriorityTaskWoken );
            break;
        }
    }
}

/*-----------------------------------------------------------*/

void USART1_IRQHandler( void )
{
    HAL_UART_IRQHandler( pxUarts[ 0 ]->pxHuart );
}
/*-----------------------------------------------------------*/

void USART2_IRQHandler( void )
{
    HAL_UART_IRQHandler( pxUarts[ 1 ]->pxHuart );
}
/*-----------------------------------------------------------*/

void USART3_IRQHandler( void )
{
    HAL_UART_IRQHandler( pxUarts[ 2 ]->pxHuart );
}
/*-----------------------------------------------------------*/

void UART4_IRQHandler( void )
{
    HAL_UART_IRQHandler( pxUarts[ 3 ]->pxHuart );
}
/*-----------------------------------------------------------*/

void UART5_IRQHandler( void )
{
    HAL_UART_IRQHandler( pxUarts[ 4 ]->pxHuart );
}
/*-----------------------------------------------------------*/
