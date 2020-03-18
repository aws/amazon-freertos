/*
 * Amazon FreeRTOS V201908.00
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_ble_coex_console_demo.h
 * @brief Sample demo for a BLE COEX CONSOLE server
 */
#ifndef AWS_BLE_COEX_CONSOLE_H_
#define AWS_BLE_COEX_CONSOLE_H_

#include "FreeRTOS.h"
/* The config header is always included first. */
#include "iot_config.h"
#include "platform/iot_network.h"

extern TimerHandle_t timerHandle;

// timer call back function after the timer expires
void TimerCbackFunc(TimerHandle_t);

// function to display Server throughput
void vCalculateServerThroughput(void);

/**
 * @brief  GATT service, characteristics and descriptor UUIDs used by the sample.
 */
#define CoexConsoleDemoSVC_UUID                 { 0x00, 0xFF, 0x69, 0xD6, 0xC6, 0xBF, 0x14, 0x90, 0x25, 0x41, 0xE7, 0x49, 0xE3, 0xD9, 0xF2, 0xC6 }
#define CoexConsoleCHAR_UUID_MASK                0x69, 0xD6, 0xC6, 0xBF, 0x14, 0x90, 0x25, 0x41, 0xE7, 0x49, 0xE3, 0xD9, 0xF2, 0xC6
#define CoexConsoleDemoCHAR_COUNTER_UUID        { 0x01, 0xFF, CoexConsoleCHAR_UUID_MASK }
#define CoexConsoleDemoCHAR_CONTROL_UUID        { 0x02, 0xFF, CoexConsoleCHAR_UUID_MASK }
#define CoexConsoleDemoCLIENT_CHAR_CFG_UUID     ( 0x2902 )

/**
 * @brief Enable Notification and Enable Indication values as defined by GATT spec.
 */
#define ENABLE_NOTIFICATION              ( 0x01 )
#define ENABLE_INDICATION                ( 0x02 )

/**
 * @brief Number of characteristics, descriptors and included services for GATT sample service.
 */
#define gattDemoNUM_CHARS                ( 2 )
#define gattDemoNUM_CHAR_DESCRS          ( 1 )
#define gattDemoNUM_INCLUDED_SERVICES    ( 0 )


/**
 * @brief Characteristics used by the GATT sample service.
 */
typedef enum
{
	eCoexConsoleDemoService = 0,      /**< COEX CONSOLE demo service. */
    eCoexConsoleDemoCharCounter,      /**< Keeps track of a counter value which is incremented periodically and optionally sent as notification to a GATT client */
    eCoexConsoleDemoCharDescrCounter, /**< Client characteristic configuration descriptor used by the GATT client to enable notifications on Counter characteristic */
    //eCoecConsoleDemoCharControl,      /**< Accepts commands from a GATT client to stop/start/reset the counter value */
    eCoexConsoleDemoNbAttributes
} coexConsoleDemoAttributes_t;

/**
 * @brief Events generated for the commands sent from GATT client over Control characteristic
 */
typedef enum
{
    eCoexConsoleDemoStart = 0, /**< Starts/resumes the counter value update. */
    eCoexConsoleDemoStop  = 1,  /**< Stops counter value update. Also stops sending notifications to GATT client if its already enabled. */
    eCoexConsoleDemoReset = 2, /**< Resets the counter value to zero */
} COexConsoleDemoEvent_t;

#define EVENT_BIT( event )    ( ( uint32_t ) 0x1 << event )

/**
 * @brief Creates and starts COEX CONSOLE demo service.
 *
 * @return pdTRUE if the COEX CONSOLE Service is successfully initialized, pdFALSE otherwise
 */
int vCoexConsoleDemoSvcInit( bool awsIotMqttMode,
                      const char * pIdentifier,
                      void * pNetworkServerInfo,
                      void * pNetworkCredentialInfo,
                      const IotNetworkInterface_t * pNetworkInterface );

/**
 * @brief Starts the COEX CONSOLE demo service.
 */
void vCoexConsoleDemoSvcStart( void );

/**
 * @brief Stops the COEX CONSOLE demo service.
 */
void vCoexConsoleDemoSvcStop( void );

/**
 * @brief Calculates the throughput.
 */
void vCalculateServerThroughput(void);

#endif /* AWS_BLE_COEX_CONSOLE_DEMO_H_ */
