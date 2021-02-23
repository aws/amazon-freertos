/*
 * FreeRTOS V202012.00
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
 * @file aws_ble_numericComparison.c
 * @brief Tasks and callbacks for user confirmation for BLE passkey.
 *
 * This file contains the task for printing the BLE passkey and
 * accepts the confirmation from user over UART. This file also
 * contains implementations of callback when BLE numeric comparison
 * passkey is available and a callback when there is a state change
 * for BLE GAP Pairing.
 */

#include <string.h>


/* The config header is always included first. */
#include "iot_config.h"
#include "iot_ble_config.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "iot_ble.h"
#include "iot_ble_numericComparison.h"


/**
 * @brief Stack size for the numeric comparsion task.
 * Task waits on user input for BLE passkey confirmation by calling
 * board specific uart read. It then calls BLE API to confirm wether the
 * passkeys are accepted or rejected.
 */
#define IOT_BLE_NUMERIC_COMPARISON_TASK_STACK_SIZE    ( 2048 )

/**
 * @brief Task priority for the numeric comparsion task.
 * Task waits on user input for BLE passkey confirmation by calling
 * board specific uart read. It then calls BLE API to confirm wether the
 * passkeys are accepted or rejected.
 */
#define IOT_BLE_NUMERIC_COMPARISON_TASK_PRIORITY      ( tskIDLE_PRIORITY )

/**
 * @brief Size of the numeric comaprison queue.
 * Setting it to 1 as only one message is allowed anytime.
 */
#define IOT_BLE_NUMERIC_COMPARISON_QUEUE_SIZE         ( 1 )

/**
 * @brief Maximum number of tries for confirmation.
 */
#define IOT_BLE_MAX_NUMERIC_COMPARISON_TRIES          ( 5 )

/**
 * @brief Structure defines each element of Numeric comparison queue.
 */
typedef struct
{
    uint32_t ulPassKey;
    BTBdaddr_t xAddress;
} BLEPassKeyConfirm_t;

/**
 * @brief Handle for numeric comparison queue.
 */
static QueueHandle_t xNumericComparisonQueue = NULL;

void vDemoBLEGAPPairingStateChangedCb( BTStatus_t xStatus,
                                       BTBdaddr_t * pxRemoteBdAddr,
                                       BTBondState_t bondState,
                                       BTSecurityLevel_t xSecurityLevel,
                                       BTAuthFailureReason_t xReason )
{
}

void vDemoBLENumericComparisonCb( BTBdaddr_t * pxRemoteBdAddr,
                                  uint32_t ulPassKey )
{
    BLEPassKeyConfirm_t xPassKeyConfirm;

    if( pxRemoteBdAddr != NULL )
    {
        xPassKeyConfirm.ulPassKey = ulPassKey;
        memcpy( &xPassKeyConfirm.xAddress, pxRemoteBdAddr, sizeof( BTBdaddr_t ) );

        xQueueSend( xNumericComparisonQueue, ( void * ) &xPassKeyConfirm, ( portTickType ) portMAX_DELAY );
    }
}

static void prvNumericComparisonTask( void * pvParameters )
{
    char userInput;
    BLEPassKeyConfirm_t xPassKeyConfirm;
    TickType_t xAuthTimeout = pdMS_TO_TICKS( IOT_BLE_NUMERIC_COMPARISON_TIMEOUT_SEC * 1000 );
    uint32_t ulTries;

    for( ; ; )
    {
        if( xQueueReceive( xNumericComparisonQueue, ( void * ) &xPassKeyConfirm, portMAX_DELAY ) )
        {
            configPRINTF( ( "Numeric comparison:%ld\n", xPassKeyConfirm.ulPassKey ) );
            configPRINTF( ( "Press 'y' to confirm, 'n' to reject\n" ) );

            for( ulTries = 0; ulTries < IOT_BLE_MAX_NUMERIC_COMPARISON_TRIES; ulTries++ )
            {
                /* Waiting for UART event. */
                if( xPortGetUserInput( ( uint8_t * ) &userInput, 1, xAuthTimeout ) > 0 )
                {
                    if( ( userInput == 'y' ) || ( userInput == 'Y' ) )
                    {
                        configPRINTF( ( "Key accepted\n" ) );
                        IotBle_ConfirmNumericComparisonKeys( &xPassKeyConfirm.xAddress, true );
                        break;
                    }
                    else if( ( userInput == 'n' ) || ( userInput == 'N' ) )
                    {
                        configPRINTF( ( "Key Rejected\n" ) );
                        IotBle_ConfirmNumericComparisonKeys( &xPassKeyConfirm.xAddress, false );
                        break;
                    }
                    else
                    {
                        configPRINTF( ( "Wrong key pressed.\n" ) );
                        configPRINTF( ( "Press 'y' to confirm, 'n' to reject\n" ) );
                    }
                }
                else
                {
                    configPRINTF( ( "Error reading user input for numeric comparison\r\n" ) );
                    break;
                }
            }
        }
    }
}

void vDemoBLENumericComparisonInit( void )
{
    /* Create a queue that will pass in the code to the UART task and wait validation from the user. */
    xNumericComparisonQueue = xQueueCreate( IOT_BLE_NUMERIC_COMPARISON_QUEUE_SIZE, sizeof( BLEPassKeyConfirm_t ) );

    xTaskCreate( prvNumericComparisonTask,
                 "NumericComparisonTask",
                 IOT_BLE_NUMERIC_COMPARISON_TASK_STACK_SIZE,
                 NULL,
                 IOT_BLE_NUMERIC_COMPARISON_TASK_PRIORITY,
                 NULL );
}
