/*
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
 */

/**
 * @file iot_network_afr.h
 * @brief Declares the network stack functions specified in iot_network.h for
 * Amazon FreeRTOS Secure Sockets.
 */

#ifndef _IOT_NETWORK_AFR_H_
#define _IOT_NETWORK_AFR_H_

/* Standard includes. */
#include <stdbool.h>

/* Platform network include. */
#include "platform/iot_network.h"

/* Amazon FreeRTOS Secure Sockets include. */
#include "aws_secure_sockets.h"

/* Credentials include. */
#include "aws_clientcredential.h"

typedef struct IotNetworkConnectionAfr
{
    Socket_t socket;
    StaticSemaphore_t socketMutex;
    StaticEventGroup_t connectionFlags;

    uint8_t * pReceiveBuffer;
    IotNetworkReceiveCallback_t receiveCallback;
    void * pReceiveContext;
} IotNetworkConnectionAfr_t;

typedef struct IotNetworkServerInfoAfr
{
    const char * pHostName;
    uint16_t port;
} IotNetworkServerInfoAfr_t;

typedef struct IotNetworkCredentialsAfr
{
    bool disableSni;

    const char * pAlpnProtos;

    const char * pRootCa;
    const char * pClientCert;
    const char * pPrivateKey;

    size_t rootCaSize;
    size_t clientCertSize;
    size_t privateKeySize;
} IotNetworkCredentialsAfr_t;

#define IOT_NETWORK_CONNECTION_AFR_INITIALIZER     { 0 }

#define IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER    { 0 }

#define AWS_IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER        \
    {                                                      \
        .pHostName = clientcredentialMQTT_BROKER_ENDPOINT, \
        .port = clientcredentialMQTT_BROKER_PORT           \
    }

#define IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER    { 0 }

#define AWS_IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER                             \
    {                                                                           \
        .disableSni = false,                                                    \
        .pAlpnProtos = socketsAWS_IOT_ALPN_MQTT,                                \
        .pRootCa = NULL,                                                        \
        .pClientCert = clientcredentialCLIENT_CERTIFICATE_PEM,                  \
        .pPrivateKey = clientcredentialCLIENT_PRIVATE_KEY_PEM,                  \
        .rootCaSize = 0,                                                        \
        .clientCertSize = ( size_t ) clientcredentialCLIENT_CERTIFICATE_LENGTH, \
        .privateKeySize = ( size_t ) clientcredentialCLIENT_PRIVATE_KEY_LENGTH  \
    }

#define IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER    { 0 }

IotNetworkError_t IotNetworkAfr_Create( void * pConnectionInfo,
                                        void * pCredentialInfo,
                                        void * const pConnection );

IotNetworkError_t IotNetworkAfr_SetReceiveCallback( void * pConnection,
                                                    IotNetworkReceiveCallback_t receiveCallback,
                                                    void * pContext );

size_t IotNetworkAfr_Send( void * pConnection,
                           const uint8_t * pMessage,
                           size_t messageLength );

size_t IotNetworkAfr_Receive( void * pConnection,
                              uint8_t * pBuffer,
                              size_t bytesRequested );

IotNetworkError_t IotNetworkAfr_Close( void * pConnection );

IotNetworkError_t IotNetworkAfr_Destroy( void * pConnection );

extern const IotNetworkInterface_t _IotNetworkAfr;

#endif /* ifndef _IOT_NETWORK_AFR_H_ */
