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

/**
 * @brief Information of hardware pin connections.
 *
 ***********************************************************************************************************
 *  Name                MCU     B-L475E-IOT01A  X-NUCLEO-STMODA1            P-L496G-CELL02  BG96 (Name)
 ***********************************************************************************************************
 *  CELLULAR_PWR_EN          PB4     CN3.6           CN9.6 - JP14                9               15 (PWRKEY)
 *  CELLULAR_RST             PA15    CN1.2           CN5.2 - JP7                 12              17 (RESET_N)
 *  CELLULAR_STATUS          PC3     CN4.3           CN8.3 - JP13                20              20 (STATUS)
 *  CELLULAR_DTR             PB1     CN3.7           CN9.7 - JP9                 14              30 (DTR)
 *  CELLULAR_RI              PC1     CN4.5           CN8.5 - JP6                 11              39 (RI)
 *  CELLULAR_SIM_SELECT_0    PA4     CN3.8           CN9.8 - JP11                18              N/A
 *  CELLULAR_SIM_SELECT_1    PA3     CN3.5           CN9.5 - JP15                8               N/A
 *  CELLULAR_SIM_RST         PA2     CN1.3           CN5.3 - JP10                17              44 (USIM_RST)
 *  CELLULAR_SIM_CLK         PC0     CN4.6           CN8.6 - JP8                 13              46 (USIM_CLK)
 *  CELLULAR_SIM_DATA        PB0     CN3.4           CN9.4 - JP12                19              45 (USIM_DATA)
 *  CELLULAR_UART_TX         PA0     CN3.2           CN9.2 - JP1(1-2),JP3(1-2)   2               34 (UART-RXD)
 *  CELLULAR_UART_RX         PA1     CN3.1           CN9.1 - JP2(1-2),JP4(1-2)   3               35 (UART-TXD)
 *  CELLULAR_UART_RTS        PC4     CN4.2           CN8.2 - JP5(1-2)            4               37 (UART_RTS)
 *  CELLULAR_UART_CTS        PC5     CN4.1           CN8.1                       1               36 (UART-RCS)
 ***********************************************************************************************************
 *
 * Refer the links below for details.
 *   https://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-mpu-eval-tools/stm32-mcu-mpu-eval-tools/stm32-discovery-kits/b-l475e-iot01a.html
 *   https://www.st.com/content/st_com/en/products/ecosystems/stm32-open-development-environment/stm32-nucleo-expansion-boards/stm32-ode-translate-hw/x-nucleo-stmoda1.html
 *   https://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-mpu-eval-tools/stm32-mcu-mpu-eval-tools/stm32-discovery-kits/p-l496g-cell02.html
 *   https://www.quectel.com/product/bg96.htm
 */

#define CELLULAR_PWR_EN_GPIO_Port        GPIOB
#define CELLULAR_PWR_EN_Pin              GPIO_PIN_4
#define CELLULAR_RST_GPIO_Port           GPIOA
#define CELLULAR_RST_Pin                 GPIO_PIN_15
#define CELLULAR_STATUS_GPIO_Port        GPIOC
#define CELLULAR_STATUS_Pin              GPIO_PIN_3

#define CELLULAR_DTR_GPIO_Port           GPIOB
#define CELLULAR_DTR_Pin                 GPIO_PIN_1
#define CELLULAR_RI_GPIO_Port            GPIOC
#define CELLULAR_RI_Pin                  GPIO_PIN_1

#define CELLULAR_SIM_SELECT_0_GPIO_Port  GPIOA
#define CELLULAR_SIM_SELECT_0_Pin        GPIO_PIN_4
#define CELLULAR_SIM_SELECT_1_GPIO_Port  GPIOA
#define CELLULAR_SIM_SELECT_1_Pin        GPIO_PIN_3

#define CELLULAR_SIM_RST_GPIO_Port       GPIOA
#define CELLULAR_SIM_RST_Pin             GPIO_PIN_2
#define CELLULAR_SIM_CLK_GPIO_Port       GPIOC
#define CELLULAR_SIM_CLK_Pin             GPIO_PIN_0
#define CELLULAR_SIM_DATA_GPIO_Port      GPIOB
#define CELLULAR_SIM_DATA_Pin            GPIO_PIN_0

#define CELLULAR_UART_MAIN_TX_GPIO_Port  GPIOA
#define CELLULAR_UART_MAIN_TX_Pin        GPIO_PIN_0
#define CELLULAR_UART_MAIN_RX_GPIO_Port  GPIOA
#define CELLULAR_UART_MAIN_RX_Pin        GPIO_PIN_1
#define CELLULAR_UART_MAIN_RTS_GPIO_Port GPIOC
#define CELLULAR_UART_MAIN_RTS_Pin       GPIO_PIN_4
#define CELLULAR_UART_MAIN_CTS_GPIO_Port GPIOC
#define CELLULAR_UART_MAIN_CTS_Pin       GPIO_PIN_5

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

#define CELLULAR_UART_PINS_MAX           ( 4U )

#define DEFAULT_WAIT_INTERVAL       ( 20UL )
#define DEFAULT_RECV_WAIT_INTERVAL  ( 5UL )
#define DEFAULT_RECV_TIMEOUT        ( 1000UL )


/**
 * @brief Device GPIO timing parameters.
 */
#define HAL_GPIO_DEVICE_OFF_TIME                ( 150 )
#define HAL_GPIO_RESET_DEVICE_ON_TIME           ( 100 )
#define HAL_GPIO_EN_DEVICE_ON_TIME              ( 200 )
#define HAL_GPIO_DTR_DEVICE_ON_TIME             ( 100 )
#define DEVICE_ON_WAIT_TIME_LOOP                ( 50 )
#define DEVICE_ON_WAIT_TIME_LOOP_INTERVAL_MS    ( 100 )

#define HAL_GPIO_EN_DEVICE_OFF_TIME             ( 1000 )
#define DEVICE_OFF_WAIT_TIME_LOOP               ( 200 )
#define DEVICE_OFF_WAIT_TIME_LOOP_INTERVAL_MS   ( 100 )

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

/**
 * @brief Number of supported communication interfaces
 */
enum
{
    CELLULAR_INTERFACE_MAIN_UART = 0, /* UART4 */
    CELLULAR_INTERFACE_MAX
};

/**
 * @brief CELLULAR General Purpose I/O and GPIO Init structure definition.
 */
typedef struct CellularGpioInitStruct
{
    GPIO_TypeDef * Port;
    GPIO_InitTypeDef Init;
} CellularGpioInitStruct_t;

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
static CellularCommInterfaceError_t prvCellularPowerOn( void );
static void prvCellularPowerOff( void );

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

/* Control pin configuration */
/* This structure is defined as configuration. Doesn't belong to specific function. */
/* coverity[misra_c_2012_rule_8_9_violation].*/
static const CellularGpioInitStruct_t _cellularGpioInitStruct[ ] =
{
    /* Port     Pin                   Mode                  Pull            Speed                   Alternate */
    { .Port = CELLULAR_RST_GPIO_Port,
      .Init = { CELLULAR_RST_Pin,          GPIO_MODE_OUTPUT_PP,  GPIO_NOPULL,    GPIO_SPEED_FREQ_LOW,    0 } },
    { .Port = CELLULAR_DTR_GPIO_Port,
      .Init = { CELLULAR_DTR_Pin,          GPIO_MODE_OUTPUT_PP,  GPIO_NOPULL,    GPIO_SPEED_FREQ_LOW,    0 } },
    { .Port = CELLULAR_PWR_EN_GPIO_Port,
      .Init = { CELLULAR_PWR_EN_Pin,       GPIO_MODE_OUTPUT_PP,  GPIO_NOPULL,    GPIO_SPEED_FREQ_LOW,    0 } },
    { .Port = CELLULAR_STATUS_GPIO_Port,
      .Init = { CELLULAR_STATUS_Pin,       GPIO_MODE_INPUT,      GPIO_PULLDOWN,  GPIO_SPEED_FREQ_HIGH,   0 } },
    { .Port = CELLULAR_RI_GPIO_Port,
      .Init = { CELLULAR_RI_Pin,           GPIO_MODE_INPUT,      GPIO_PULLDOWN,  GPIO_SPEED_FREQ_HIGH,   0 } },
    { .Port = CELLULAR_SIM_SELECT_0_GPIO_Port,
      .Init = { CELLULAR_SIM_SELECT_0_Pin, GPIO_MODE_OUTPUT_PP,  GPIO_NOPULL,    GPIO_SPEED_FREQ_LOW,    0 } },
    { .Port = CELLULAR_SIM_SELECT_1_GPIO_Port,
      .Init = { CELLULAR_SIM_SELECT_1_Pin, GPIO_MODE_OUTPUT_PP,  GPIO_NOPULL,    GPIO_SPEED_FREQ_LOW,    0 } },
    { .Port = CELLULAR_SIM_RST_GPIO_Port,
      .Init = { CELLULAR_SIM_RST_Pin,      GPIO_MODE_INPUT,      GPIO_NOPULL,    GPIO_SPEED_FREQ_LOW,    0 } },
    { .Port = CELLULAR_SIM_CLK_GPIO_Port,
      .Init = { CELLULAR_SIM_CLK_Pin,      GPIO_MODE_INPUT,      GPIO_NOPULL,    GPIO_SPEED_FREQ_LOW,    0 } },
    { .Port = CELLULAR_SIM_DATA_GPIO_Port,
      .Init = { CELLULAR_SIM_DATA_Pin,     GPIO_MODE_INPUT,      GPIO_NOPULL,    GPIO_SPEED_FREQ_LOW,    0 } },
    { NULL }
};

/* UART pin configuration */
static const CellularGpioInitStruct_t _cellularUartInitStruct[ CELLULAR_INTERFACE_MAX ][ CELLULAR_UART_PINS_MAX ] =
{
    /*   Port     Pin                   Mode                Pull            Speed                         Alternate */
    { { .Port = CELLULAR_UART_MAIN_TX_GPIO_Port,
        .Init = { CELLULAR_UART_MAIN_TX_Pin, GPIO_MODE_AF_PP,    GPIO_PULLUP,    GPIO_SPEED_FREQ_VERY_HIGH,    GPIO_AF8_UART4 } },
      { .Port = CELLULAR_UART_MAIN_RX_GPIO_Port,
        .Init = { CELLULAR_UART_MAIN_RX_Pin, GPIO_MODE_AF_PP,    GPIO_PULLUP,    GPIO_SPEED_FREQ_VERY_HIGH,    GPIO_AF8_UART4 } },
      { .Port = CELLULAR_UART_MAIN_RTS_GPIO_Port,
        .Init = { CELLULAR_UART_MAIN_RTS_Pin,GPIO_MODE_AF_PP,    GPIO_PULLUP,    GPIO_SPEED_FREQ_VERY_HIGH,    GPIO_AF8_UART4 } },
      { .Port = CELLULAR_UART_MAIN_CTS_GPIO_Port,
        .Init = { CELLULAR_UART_MAIN_CTS_Pin,GPIO_MODE_AF_PP,    GPIO_PULLUP,    GPIO_SPEED_FREQ_VERY_HIGH,    GPIO_AF8_UART4 } }
    },
};

static CellularCommInterfaceContext _iotCommIntfCtx = { 0 };
static UART_HandleTypeDef _iotCommIntfPhy = { 0 };

/*-----------------------------------------------------------*/

static void prvCellularGpioInit( void )
{
    const CellularGpioInitStruct_t * pCellularGpioInitStruct = _cellularGpioInitStruct;

    /* Enable GPIO Ports Clock */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Set RST and PWR_EN to high in order the modem stays reset state */
    HAL_GPIO_WritePin( CELLULAR_RST_GPIO_Port, CELLULAR_RST_Pin, GPIO_PIN_SET );
    HAL_GPIO_WritePin( CELLULAR_PWR_EN_GPIO_Port, CELLULAR_PWR_EN_Pin, GPIO_PIN_SET );

    /* Reset DTR */
    HAL_GPIO_WritePin( CELLULAR_DTR_GPIO_Port, CELLULAR_DTR_Pin, GPIO_PIN_RESET );

    /**********************************************************************
       SIM selection table
     **********************************************************************
       CELLULAR_SIM_SELECT_0_Pin     CELLULAR_SIM_SELECT_1_Pin    Which SIM
     **********************************************************************
       GPIO_PIN_RESET           GPIO_PIN_RESET          SOCKET SIM
       GPIO_PIN_RESET           GPIO_PIN_SET            EMBEDDED SIM
       GPIO_PIN_SET             GPIO_PIN_RESET          STM32 SIM
     **********************************************************************/
    /* Select socket SIM */
    HAL_GPIO_WritePin( CELLULAR_SIM_SELECT_0_GPIO_Port, CELLULAR_SIM_SELECT_0_Pin, GPIO_PIN_RESET );
    HAL_GPIO_WritePin( CELLULAR_SIM_SELECT_1_GPIO_Port, CELLULAR_SIM_SELECT_1_Pin, GPIO_PIN_RESET );

    while( pCellularGpioInitStruct->Port != NULL )
    {
        HAL_GPIO_Init( ( GPIO_TypeDef * ) pCellularGpioInitStruct->Port,
                       ( GPIO_InitTypeDef * ) & pCellularGpioInitStruct->Init );
        pCellularGpioInitStruct++;
    }
}

/* Override HAL_UART_MspInit() */
void HAL_UART_MspInit( UART_HandleTypeDef * hUart )
{
    const CellularGpioInitStruct_t * pCellularGpioInitStruct = & _cellularUartInitStruct[ CELLULAR_INTERFACE_MAIN_UART ][ 0 ];
    uint8_t count = 0;

    if( ( hUart != NULL ) && ( hUart->Instance == CELLULAR_UART_MAIN ) )
    {
        /* Enable UART Clock */
        CELLULAR_UART_MAIN_CLK_ENABLE();

        /* UART pin init */
        for( count = 0; count < CELLULAR_UART_PINS_MAX; count++ )
        {
            if( pCellularGpioInitStruct->Port != NULL )
                HAL_GPIO_Init( ( GPIO_TypeDef * ) pCellularGpioInitStruct->Port,
                               ( GPIO_InitTypeDef * ) & pCellularGpioInitStruct->Init );
            pCellularGpioInitStruct++;
        }

        /* UART interrupt init */
        HAL_NVIC_SetPriority (CELLULAR_UART_MAIN_IRQn, 5, 0 );
        HAL_NVIC_EnableIRQ( CELLULAR_UART_MAIN_IRQn );
    }
}

/* Override HAL_UART_MspDeInit() */
void HAL_UART_MspDeInit( UART_HandleTypeDef * hUart )
{
    const CellularGpioInitStruct_t * pCellularGpioInitStruct = & _cellularUartInitStruct[ CELLULAR_INTERFACE_MAIN_UART ][ 0 ];
    uint8_t count = 0;

    if( ( hUart != NULL ) && ( hUart->Instance == CELLULAR_UART_MAIN ) )
    {
        /* UART interrupt deinit */
        HAL_NVIC_DisableIRQ( CELLULAR_UART_MAIN_IRQn );

        /* Disable UART Clock */
        CELLULAR_UART_MAIN_CLK_DISABLE();

        /* UART pin deinit */
        for( count = 0; count < CELLULAR_UART_PINS_MAX; count++ )
        {
            if( pCellularGpioInitStruct->Port != NULL )
            {
                HAL_GPIO_DeInit( ( GPIO_TypeDef * ) pCellularGpioInitStruct->Port,
                                 pCellularGpioInitStruct->Init.Pin );
            }
            pCellularGpioInitStruct++;
        }
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
                    pIotCommIntfCtx->pRecvCB( pIotCommIntfCtx->pUserData,
                                              ( CellularCommInterfaceHandle_t ) pIotCommIntfCtx );
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
        /* Init the GPIO. */
        prvCellularGpioInit();

        /* Power on the device. */
        ret = prvCellularPowerOn();
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
            prvCellularPowerOff();
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
            prvCellularPowerOff();
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
        prvCellularPowerOff();

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

static CellularCommInterfaceError_t prvCellularPowerOn( void )
{
    CellularCommInterfaceError_t ret = IOT_COMM_INTERFACE_SUCCESS;
    uint8_t count = 0, countLimit = 0;
    GPIO_PinState pinStatus = GPIO_PIN_SET;

    IotLogDebug( "CellularPowerOn +" );

    /* Turn it off first when it already turned on */
    HAL_GPIO_WritePin( CELLULAR_PWR_EN_GPIO_Port, CELLULAR_PWR_EN_Pin, GPIO_PIN_SET );
    HAL_GPIO_WritePin( CELLULAR_RST_GPIO_Port, CELLULAR_RST_Pin, GPIO_PIN_SET );
    HAL_Delay( HAL_GPIO_DEVICE_OFF_TIME );

    /* Turn it on */
    HAL_GPIO_WritePin( CELLULAR_PWR_EN_GPIO_Port, CELLULAR_PWR_EN_Pin, GPIO_PIN_RESET );
    HAL_GPIO_WritePin( CELLULAR_RST_GPIO_Port, CELLULAR_RST_Pin, GPIO_PIN_RESET );
    HAL_Delay( HAL_GPIO_RESET_DEVICE_ON_TIME );

    HAL_GPIO_WritePin( CELLULAR_PWR_EN_GPIO_Port, CELLULAR_PWR_EN_Pin, GPIO_PIN_SET );
    HAL_Delay( HAL_GPIO_EN_DEVICE_ON_TIME );
    HAL_GPIO_WritePin( CELLULAR_PWR_EN_GPIO_Port, CELLULAR_PWR_EN_Pin, GPIO_PIN_RESET );

    /* Wait up to 5 seconds */
    countLimit = DEVICE_ON_WAIT_TIME_LOOP;
    for( count = 0; count < countLimit; count++ )
    {
        pinStatus = HAL_GPIO_ReadPin( CELLULAR_STATUS_GPIO_Port, CELLULAR_STATUS_Pin );
        if( pinStatus == GPIO_PIN_SET )
        {
            break;
        }
        vTaskDelay( pdMS_TO_TICKS( DEVICE_ON_WAIT_TIME_LOOP_INTERVAL_MS ) );
    }

    if( count < countLimit )
    {
        /* Set DTR to low in order to prevent the module enters sleep state */
        HAL_GPIO_WritePin( CELLULAR_DTR_GPIO_Port, CELLULAR_DTR_Pin, GPIO_PIN_SET );
        HAL_Delay( HAL_GPIO_DTR_DEVICE_ON_TIME );
        HAL_GPIO_WritePin( CELLULAR_DTR_GPIO_Port, CELLULAR_DTR_Pin, GPIO_PIN_RESET );
        ret = IOT_COMM_INTERFACE_SUCCESS;
    }
    else
    {
        IotLogError( "CellularPowerOn - turn on failed" );
        ret = IOT_COMM_INTERFACE_FAILURE;
    }

    IotLogDebug( "CellularPowerOn -" );

    return ret;
}

static void prvCellularPowerOff( void )
{
    uint8_t count = 0, countLimit = 0;
    GPIO_PinState pinStatus = GPIO_PIN_SET;

    IotLogDebug( "CellularPowerOff +" );

    /* Turn it off */
    pinStatus = HAL_GPIO_ReadPin( CELLULAR_STATUS_GPIO_Port, CELLULAR_STATUS_Pin );
    if( pinStatus == GPIO_PIN_SET )
    {
        HAL_Delay( HAL_GPIO_EN_DEVICE_OFF_TIME );
        HAL_GPIO_WritePin( CELLULAR_PWR_EN_GPIO_Port, CELLULAR_PWR_EN_Pin, GPIO_PIN_SET );
        HAL_Delay( HAL_GPIO_EN_DEVICE_OFF_TIME );
        HAL_GPIO_WritePin( CELLULAR_PWR_EN_GPIO_Port, CELLULAR_PWR_EN_Pin, GPIO_PIN_RESET );

        /* Wait up to 20 seconds */
        countLimit = DEVICE_OFF_WAIT_TIME_LOOP;
        for( count = 0; count < countLimit; count++ )
        {
            pinStatus = HAL_GPIO_ReadPin( CELLULAR_STATUS_GPIO_Port, CELLULAR_STATUS_Pin );
            if( pinStatus == GPIO_PIN_RESET )
            {
                break;
            }
            vTaskDelay( pdMS_TO_TICKS( DEVICE_OFF_WAIT_TIME_LOOP_INTERVAL_MS ) );
        }

        if( count >= countLimit )
        {
            IotLogDebug( "CellularPowerOff - timed out for normal power down" );
        }
    }

    HAL_GPIO_WritePin( CELLULAR_RST_GPIO_Port, CELLULAR_RST_Pin, GPIO_PIN_SET );
    HAL_GPIO_WritePin( CELLULAR_PWR_EN_GPIO_Port, CELLULAR_PWR_EN_Pin, GPIO_PIN_SET );

    /* Reset DTR */
    HAL_GPIO_WritePin( CELLULAR_DTR_GPIO_Port, CELLULAR_DTR_Pin, GPIO_PIN_RESET );

    IotLogDebug( "CellularPowerOff -" );
}
