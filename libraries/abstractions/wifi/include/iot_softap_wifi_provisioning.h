/*
 * FreeRTOS WiFi V2.0.0
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
 * @file iot_softap_wifi_provisioning.h
 * @brief Wi-Fi SoftAP Provisioning Interface.
 */

#ifndef _AWS_SOFTAP_WIFI_PROVISIONING_H_
#define _AWS_SOFTAP_WIFI_PROVISIONING_H_

#include <stdint.h>

/**
 * @ingroup wifi_data_types_enums
 * @brief This enumeration defines the different types of request processed by the WiFi provisioning library.
 */
typedef enum
{
    IotWiFiSoftAPProvRequestInvalid, /**< Type used to denote an invalid request. */
    IotWiFiSoftAPProvRequestStop     /**< Request sent from an application task to stop WiFi provisioning loop. */
} IotWiFiSoftAPProvRequest_t;

/**
 * @brief Initialize wifi provisioning over SoftAP.
 *
 * @return true if the initialization succeeded.
 *         false if the initialization failed.
 */
/* @[declare_iotwifisoftapprov_init] */
bool IotWifiSoftAPProv_Init( void );

/**
 * @brief Function which runs the process loop for Wifi provisioning.
 * Process loop can be run within a task, it waits for the incoming requests from the
 * transport interface as well as commands from the user application. Process loop terminates when
 * a stop command is sent from the application.
 *
 * @return true if the process loop function ran successfully. false if process loop terminated due
 *         to an error.
 */
/* @[declare_iotwifisoftapprov_init] */
bool IotWifiSoftAPProv_RunProcessLoop( void );

/**
 * @brief Gets the total number of provisioned networks.
 *
 * @return Number of provisioned networks
 */
/* @[declare_iotwifisoftapprov_getnumnetworks] */
uint32_t IotWifiSoftAPProv_GetNumNetworks( void );

/**
 * @brief Connects to one of the saved networks in priority order.
 *
 * @return Returns true if connection succeeded, else false.
 */
/* @[declare_iotwifisoftapprov_connect] */
bool IotWifiSoftAPProv_Connect( uint32_t networkIndex );

/**
 * @brief Stop the WiFi provisionig process loop function.
 * This enqueues a command to stop the WiFi provisioning process loop function.
 *
 * @return true if succesfully enqueued command to stop WiFi provisioning loop.
 */
/* @[declare_iotwifisoftapprov_stop] */
bool IotWifiSoftAPProv_Stop( void );

/**
 * @brief Tear down WIFI provisioning service
 *
 */
/* @[declare_iotwifisoftprov_Deinit] */
void IotWifiSoftAPProv_Deinit( void );

#endif /* ifndef _AWS_SOFTAP_WIFI_PROVISIONING_H_ */
