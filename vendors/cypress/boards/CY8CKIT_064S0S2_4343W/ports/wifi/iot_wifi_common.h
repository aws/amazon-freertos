/*
 * Amazon FreeRTOS Wi-Fi V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
#pragma once

/* Socket and Wi-Fi interface includes. */
#include <iot_wifi.h>

/* Wi-Fi Host driver includes. */
#include <whd.h>
#include <whd_wifi_api.h>
#include <whd_network_types.h>

/* Board and Abstraction layer includes */
#include <cyabs_rtos.h>

extern whd_interface_t primaryInterface;
extern cy_mutex_t wifiMutex;
extern WIFIDeviceMode_t devMode;
extern bool isConnected;
extern bool isPoweredUp;
extern bool isMutexInitialized;


typedef void (*cy_network_activity_event_callback_t)(bool callback_arg);

extern void cy_check_network_params(const WIFINetworkParams_t * const pxNetworkParams);
extern void cy_convert_network_params(
    const WIFINetworkParams_t * const pxNetworkParams,
    whd_ssid_t *ssid,
    uint8_t **key,
    uint8_t *keylen,
    whd_security_t *security,
    uint8_t *channel);

/**
 * Register network activity callback
 *
 * @param cb : callback function of type cy_network_activity_event_callback_t
 *
 */
void cy_network_activity_register_cb(cy_network_activity_event_callback_t cb);

/**
 * Return the single LwIP network interface.
 *
 * @return netif structure of the WHD interface
 */
struct netif *cy_lwip_get_interface() ;

/**
 * lwip bringup wrapper function
 *
 * @return CY_RSLT_SUCCESS if lwip bringup is successful, failure code otherwise.
 */
cy_rslt_t cy_lwip_bringup_interface( void );

/**
 * lwip bringdown wrapper function
 *
 * @return CY_RSLT_SUCCESS if lwip bringdown is successful, failure code otherwise.
 */
cy_rslt_t cy_lwip_bringdown_interface( void );
