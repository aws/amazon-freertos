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
    #define LIBRARY_LOG_LEVEL    IOT_LOG_ERROR
#endif

#define LIBRARY_LOG_NAME         ( "COMM_IF_ST_DEVICE" )
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

/**
 * @brief Control pin definitions.
 */
#define CELLULAR_PWR_EN_GPIO_Port                GPIOB
#define CELLULAR_PWR_EN_Pin                      GPIO_PIN_4
#define CELLULAR_RST_GPIO_Port                   GPIOA
#define CELLULAR_RST_Pin                         GPIO_PIN_15
#define CELLULAR_STATUS_GPIO_Port                GPIOC
#define CELLULAR_STATUS_Pin                      GPIO_PIN_3

#define CELLULAR_DTR_GPIO_Port                   GPIOB
#define CELLULAR_DTR_Pin                         GPIO_PIN_1
#define CELLULAR_RI_GPIO_Port                    GPIOC
#define CELLULAR_RI_Pin                          GPIO_PIN_1

#define CELLULAR_SIM_SELECT_0_GPIO_Port          GPIOA
#define CELLULAR_SIM_SELECT_0_Pin                GPIO_PIN_4
#define CELLULAR_SIM_SELECT_1_GPIO_Port          GPIOA
#define CELLULAR_SIM_SELECT_1_Pin                GPIO_PIN_3

#define CELLULAR_SIM_RST_GPIO_Port               GPIOA
#define CELLULAR_SIM_RST_Pin                     GPIO_PIN_2
#define CELLULAR_SIM_CLK_GPIO_Port               GPIOC
#define CELLULAR_SIM_CLK_Pin                     GPIO_PIN_0
#define CELLULAR_SIM_DATA_GPIO_Port              GPIOB
#define CELLULAR_SIM_DATA_Pin                    GPIO_PIN_0

/**
 * @brief UART pin configuration.
 */
#define CELLULAR_UART_MAIN_TX_GPIO_Port          GPIOA
#define CELLULAR_UART_MAIN_TX_Pin                GPIO_PIN_0
#define CELLULAR_UART_MAIN_RX_GPIO_Port          GPIOA
#define CELLULAR_UART_MAIN_RX_Pin                GPIO_PIN_1
#define CELLULAR_UART_MAIN_RTS_GPIO_Port         GPIOC
#define CELLULAR_UART_MAIN_RTS_Pin               GPIO_PIN_4
#define CELLULAR_UART_MAIN_CTS_GPIO_Port         GPIOC
#define CELLULAR_UART_MAIN_CTS_Pin               GPIO_PIN_5

#define CELLULAR_UART_PINS_MAX                   ( 4U )

/**
 * @brief Device GPIO timing parameters.
 */
#define HAL_GPIO_DEVICE_OFF_TIME                 ( 150 )
#define HAL_GPIO_RESET_DEVICE_ON_TIME            ( 100 )
#define HAL_GPIO_EN_DEVICE_ON_TIME               ( 200 )
#define HAL_GPIO_DTR_DEVICE_ON_TIME              ( 100 )
#define DEVICE_ON_WAIT_TIME_LOOP                 ( 50 )
#define DEVICE_ON_WAIT_TIME_LOOP_INTERVAL_MS     ( 100 )
#define HAL_GPIO_EN_DEVICE_OFF_TIME              ( 1000 )
#define DEVICE_OFF_WAIT_TIME_LOOP                ( 200 )
#define DEVICE_OFF_WAIT_TIME_LOOP_INTERVAL_MS    ( 100 )

/*-----------------------------------------------------------*/

/**
 * @brief CELLULAR General Purpose I/O and GPIO Init structure definition.
 */
typedef struct CellularGpioInitStruct
{
    GPIO_TypeDef * Port;
    GPIO_InitTypeDef Init;
} CellularGpioInitStruct_t;

/**
 * @brief Number of supported communication interfaces
 */
enum
{
    CELLULAR_INTERFACE_MAIN_UART = 0, /* The main UART for Cellular interface is UART4. */
    CELLULAR_INTERFACE_MAX
};

/*-----------------------------------------------------------*/

/**
 * @brief Initialize UART pins.
 */
static void prvUartPinInit( void );

/**
 * @brief Deinitialize UART pins.
 */
static void prvUartPinDeInit( void );

/**
 * @brief Initialize device control pins.
 */
static void prvDevicePinInit( void );

/*-----------------------------------------------------------*/

/**
 * @brief Control pin configuration.
 */
/* This structure is defined as configuration. Doesn't belong to specific function. */
/* coverity[misra_c_2012_rule_8_9_violation].*/
static const CellularGpioInitStruct_t _cellularGpioInitStruct[] =
{
    /* Port     Pin                   Mode                  Pull            Speed                   Alternate */
    { .Port = CELLULAR_RST_GPIO_Port,
      .Init ={ CELLULAR_RST_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0 } },
    { .Port = CELLULAR_DTR_GPIO_Port,
      .Init ={ CELLULAR_DTR_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0 } },
    { .Port = CELLULAR_PWR_EN_GPIO_Port,
      .Init ={ CELLULAR_PWR_EN_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0 } },
    { .Port = CELLULAR_STATUS_GPIO_Port,
      .Init ={ CELLULAR_STATUS_Pin, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_HIGH, 0 } },
    { .Port = CELLULAR_RI_GPIO_Port,
      .Init ={ CELLULAR_RI_Pin, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_HIGH, 0 } },
    { .Port = CELLULAR_SIM_SELECT_0_GPIO_Port,
      .Init ={ CELLULAR_SIM_SELECT_0_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0 } },
    { .Port = CELLULAR_SIM_SELECT_1_GPIO_Port,
      .Init ={ CELLULAR_SIM_SELECT_1_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0 } },
    { .Port = CELLULAR_SIM_RST_GPIO_Port,
      .Init ={ CELLULAR_SIM_RST_Pin, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0 } },
    { .Port = CELLULAR_SIM_CLK_GPIO_Port,
      .Init ={ CELLULAR_SIM_CLK_Pin, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0 } },
    { .Port = CELLULAR_SIM_DATA_GPIO_Port,
      .Init ={ CELLULAR_SIM_DATA_Pin, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0 } },
    { NULL }
};

/**
 * @brief UART pin configuration.
 */
static const CellularGpioInitStruct_t _cellularUartInitStruct[ CELLULAR_INTERFACE_MAX ][ CELLULAR_UART_PINS_MAX ] =
{
    /*   Port     Pin                   Mode                Pull            Speed                         Alternate */
    { { .Port = CELLULAR_UART_MAIN_TX_GPIO_Port,
        .Init ={ CELLULAR_UART_MAIN_TX_Pin, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF8_UART4 } },
        { .Port = CELLULAR_UART_MAIN_RX_GPIO_Port,
        .Init = { CELLULAR_UART_MAIN_RX_Pin, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF8_UART4 } },
        { .Port = CELLULAR_UART_MAIN_RTS_GPIO_Port,
        .Init = { CELLULAR_UART_MAIN_RTS_Pin, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF8_UART4 } },
        { .Port = CELLULAR_UART_MAIN_CTS_GPIO_Port,
        .Init = { CELLULAR_UART_MAIN_CTS_Pin, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_VERY_HIGH, GPIO_AF8_UART4 } } },
};

/*-----------------------------------------------------------*/

static void prvUartPinInit( void )
{
    const CellularGpioInitStruct_t * pCellularGpioInitStruct = &_cellularUartInitStruct[ CELLULAR_INTERFACE_MAIN_UART ][ 0 ];
    uint8_t count = 0;

    /* UART pin init. */
    for( count = 0; count < CELLULAR_UART_PINS_MAX; count++ )
    {
        if( pCellularGpioInitStruct->Port != NULL )
        {
            HAL_GPIO_Init( ( GPIO_TypeDef * ) pCellularGpioInitStruct->Port,
                           ( GPIO_InitTypeDef * ) &pCellularGpioInitStruct->Init );
        }

        pCellularGpioInitStruct++;
    }
}

/*-----------------------------------------------------------*/

static void prvUartPinDeInit( void )
{
    const CellularGpioInitStruct_t * pCellularGpioInitStruct = &_cellularUartInitStruct[ CELLULAR_INTERFACE_MAIN_UART ][ 0 ];
    uint8_t count = 0;

    /* UART pin deinit. */
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

/*-----------------------------------------------------------*/

static void prvDevicePinInit( void )
{
    const CellularGpioInitStruct_t * pCellularGpioInitStruct = _cellularGpioInitStruct;

    /* Enable GPIO Ports Clock. */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Set RST and PWR_EN to high in order the modem stays reset state. */
    HAL_GPIO_WritePin( CELLULAR_RST_GPIO_Port, CELLULAR_RST_Pin, GPIO_PIN_SET );
    HAL_GPIO_WritePin( CELLULAR_PWR_EN_GPIO_Port, CELLULAR_PWR_EN_Pin, GPIO_PIN_SET );

    /* Reset DTR. */
    HAL_GPIO_WritePin( CELLULAR_DTR_GPIO_Port, CELLULAR_DTR_Pin, GPIO_PIN_RESET );

    /**********************************************************************
    *  SIM selection table
    **********************************************************************
    *  CELLULAR_SIM_SELECT_0_Pin     CELLULAR_SIM_SELECT_1_Pin    Which SIM
    **********************************************************************
    *  GPIO_PIN_RESET           GPIO_PIN_RESET          SOCKET SIM
    *  GPIO_PIN_RESET           GPIO_PIN_SET            EMBEDDED SIM
    *  GPIO_PIN_SET             GPIO_PIN_RESET          STM32 SIM
    **********************************************************************/
    /* Select socket SIM. */
    HAL_GPIO_WritePin( CELLULAR_SIM_SELECT_0_GPIO_Port, CELLULAR_SIM_SELECT_0_Pin, GPIO_PIN_RESET );
    HAL_GPIO_WritePin( CELLULAR_SIM_SELECT_1_GPIO_Port, CELLULAR_SIM_SELECT_1_Pin, GPIO_PIN_RESET );

    while( pCellularGpioInitStruct->Port != NULL )
    {
        HAL_GPIO_Init( ( GPIO_TypeDef * ) pCellularGpioInitStruct->Port,
                       ( GPIO_InitTypeDef * ) &pCellularGpioInitStruct->Init );
        pCellularGpioInitStruct++;
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Power on the cellular device.
 *
 * @return IOT_COMM_INTERFACE_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
CellularCommInterfaceError_t CellularDevice_PowerOn( void )
{
    CellularCommInterfaceError_t ret = IOT_COMM_INTERFACE_SUCCESS;
    uint8_t count = 0, countLimit = 0;
    GPIO_PinState pinStatus = GPIO_PIN_SET;

    IotLogDebug( "CellularDevice_PowerOn +" );

    /* Device pin init. */
    prvDevicePinInit();

    /* UART pin init. */
    prvUartPinInit();

    /* Turn it off first when it already turned on. */
    HAL_GPIO_WritePin( CELLULAR_PWR_EN_GPIO_Port, CELLULAR_PWR_EN_Pin, GPIO_PIN_SET );
    HAL_GPIO_WritePin( CELLULAR_RST_GPIO_Port, CELLULAR_RST_Pin, GPIO_PIN_SET );
    HAL_Delay( HAL_GPIO_DEVICE_OFF_TIME );

    /* Turn it on. */
    HAL_GPIO_WritePin( CELLULAR_PWR_EN_GPIO_Port, CELLULAR_PWR_EN_Pin, GPIO_PIN_RESET );
    HAL_GPIO_WritePin( CELLULAR_RST_GPIO_Port, CELLULAR_RST_Pin, GPIO_PIN_RESET );
    HAL_Delay( HAL_GPIO_RESET_DEVICE_ON_TIME );

    HAL_GPIO_WritePin( CELLULAR_PWR_EN_GPIO_Port, CELLULAR_PWR_EN_Pin, GPIO_PIN_SET );
    HAL_Delay( HAL_GPIO_EN_DEVICE_ON_TIME );
    HAL_GPIO_WritePin( CELLULAR_PWR_EN_GPIO_Port, CELLULAR_PWR_EN_Pin, GPIO_PIN_RESET );

    /* Wait up to 5 seconds. */
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
        /* Set DTR to low in order to prevent the module enters sleep state. */
        HAL_GPIO_WritePin( CELLULAR_DTR_GPIO_Port, CELLULAR_DTR_Pin, GPIO_PIN_SET );
        HAL_Delay( HAL_GPIO_DTR_DEVICE_ON_TIME );
        HAL_GPIO_WritePin( CELLULAR_DTR_GPIO_Port, CELLULAR_DTR_Pin, GPIO_PIN_RESET );
        ret = IOT_COMM_INTERFACE_SUCCESS;
    }
    else
    {
        IotLogError( "CellularDevice_PowerOn - turn on failed" );
        ret = IOT_COMM_INTERFACE_FAILURE;
    }

    IotLogDebug( "CellularDevice_PowerOn -" );

    return ret;
}

/*-----------------------------------------------------------*/

/**
 * @brief Power off the cellular device.
 *
 * @return IOT_COMM_INTERFACE_SUCCESS if the operation is successful, otherwise an error
 * code indicating the cause of the error.
 */
void CellularDevice_PowerOff( void )
{
    uint8_t count = 0, countLimit = 0;
    GPIO_PinState pinStatus = GPIO_PIN_SET;

    IotLogDebug( "CellularDevice_PowerOff +" );

    /* Turn it off. */
    pinStatus = HAL_GPIO_ReadPin( CELLULAR_STATUS_GPIO_Port, CELLULAR_STATUS_Pin );

    if( pinStatus == GPIO_PIN_SET )
    {
        HAL_Delay( HAL_GPIO_EN_DEVICE_OFF_TIME );
        HAL_GPIO_WritePin( CELLULAR_PWR_EN_GPIO_Port, CELLULAR_PWR_EN_Pin, GPIO_PIN_SET );
        HAL_Delay( HAL_GPIO_EN_DEVICE_OFF_TIME );
        HAL_GPIO_WritePin( CELLULAR_PWR_EN_GPIO_Port, CELLULAR_PWR_EN_Pin, GPIO_PIN_RESET );

        /* Wait up to 20 seconds. */
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
            IotLogDebug( "CellularDevice_PowerOff - timed out for normal power down" );
        }
    }

    HAL_GPIO_WritePin( CELLULAR_RST_GPIO_Port, CELLULAR_RST_Pin, GPIO_PIN_SET );
    HAL_GPIO_WritePin( CELLULAR_PWR_EN_GPIO_Port, CELLULAR_PWR_EN_Pin, GPIO_PIN_SET );

    /* Reset DTR. */
    HAL_GPIO_WritePin( CELLULAR_DTR_GPIO_Port, CELLULAR_DTR_Pin, GPIO_PIN_RESET );

    /* UART pin deinit. */
    prvUartPinDeInit();

    IotLogDebug( "CellularDevice_PowerOff -" );
}

/*-----------------------------------------------------------*/
