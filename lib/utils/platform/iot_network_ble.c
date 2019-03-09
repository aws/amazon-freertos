/*
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
 */

/**
 * @file iot_network_afr.c
 * @brief Implementation of the network-related functions from iot_network_afr.h
 * for Amazon FreeRTOS secure sockets.
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>
#include "iot_ble_mqtt.h"
#include "platform/iot_network_ble.h"
/* Error handling include. */
#include "private/iot_error.h"

/*-----------------------------------------------------------*/

/**
 * @brief An #IotNetworkInterface_t that uses the functions in this file.
 */
const IotNetworkInterface_t _IotNetworkBle =
{
    .create             = IotNetworkBle_Create,
    .setReceiveCallback = IotNetworkBle_SetReceiveCallback,
    .send               = IotNetworkBle_Send,
    .receive            = IotNetworkBle_Receive,
    .close              = IotNetworkBle_Close,
    .destroy            = IotNetworkBle_Destroy
};

/*-----------------------------------------------------------*/

IotNetworkError_t IotNetworkBle_Create( void * pConnectionInfo,
                                        void * pCredentialInfo,
                                        void * pConnection )
{
	(void)pCredentialInfo;

	return IotBleMqtt_CreateConnection( pConnectionInfo, pConnection );
}

/*-----------------------------------------------------------*/

IotNetworkError_t IotNetworkBle_SetReceiveCallback( void * pConnection,
                                                    IotNetworkReceiveCallback_t receiveCallback,
                                                    void * pContext )
{
	(void)pConnection;
    (void)receiveCallback;
    (void)pContext;

    /* pContext contains MQTT connection info. */
    return IOT_NETWORK_SUCCESS;
}

/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

size_t IotNetworkBle_Receive( void * pConnection,
                              uint8_t * pBuffer,
                              size_t bytesRequested )
{
    size_t bytesReceived = 0;

    /* Receive Doesn't work on BLE. It is coupled with MQTT. Whenever something is received, it is pushed directly to MQTT. */
    return bytesReceived;
}


IotNetworkError_t IotNetworkBle_Close( void * pConnection )
{
	IotBleMqtt_CloseConnection(pConnection);
    return IOT_NETWORK_SUCCESS;
}


IotNetworkError_t IotNetworkBle_Destroy( void * pConnection )
{
	IotBleMqtt_DestroyConnection(pConnection);
    return IOT_NETWORK_SUCCESS;
}

size_t IotNetworkBle_Send( void * pConnection,
                           const uint8_t * pMessage,
                           size_t messageLength )
{
	return IotBleMqtt_Send(pConnection, pMessage, messageLength);
}
