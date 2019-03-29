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
 * @file iot_demo_afr.c
 * @brief Generic demo runner for C SDK libraries on Amazon FreeRTOS.
 */

/* Build using a config header, if provided. */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif

/* FreeRTOS demo configuration. */
#include "aws_demo_config.h"

/* Amazon FreeRTOS credentials include. */
#include "aws_clientcredential.h"

/* Demo logging include. */
#include "iot_demo_logging.h"

/* Amazon FreeRTOS network include. */
#include "platform/iot_network_afr.h"

/* C SDK library includes. */
#include "iot_common.h"
#include "iot_mqtt.h"

/*-----------------------------------------------------------*/

/**
 * @brief All C SDK demo functions have this signature.
 */
typedef int (* _demoFunction_t)( bool awsIotMqttMode,
                                 const char * pIdentifier,
                                 void * pNetworkServerInfo,
                                 void * pNetworkCredentialInfo,
                                 const IotNetworkInterface_t * pNetworkInterface );

/*-----------------------------------------------------------*/

/* C SDK demo functions. */
extern int RunMqttDemo( bool awsIotMqttMode,
                        const char * pIdentifier,
                        void * pNetworkServerInfo,
                        void * pNetworkCredentialInfo,
                        const IotNetworkInterface_t * pNetworkInterface );

/*-----------------------------------------------------------*/

static void _runDemoFunction( _demoFunction_t demoFunction )
{
    /* On Amazon FreeRTOS, credentials and server info are defined in a header
     * and set by the initializers. */
    IotNetworkServerInfoAfr_t serverInfo = AWS_IOT_NETWORK_SERVER_INFO_AFR_INITIALIZER;
    IotNetworkCredentialsAfr_t credentials = AWS_IOT_NETWORK_CREDENTIALS_AFR_INITIALIZER;

    /* ALPN only works over port 443. Disable it otherwise. */
    if( serverInfo.port != 443 )
    {
        credentials.pAlpnProtos = NULL;
    }

    /* Run the demo. */
    demoFunction( true,
                  clientcredentialIOT_THING_NAME,
                  &serverInfo,
                  &credentials,
                  IOT_NETWORK_INTERFACE_AFR );

    /* Report heap usage. */
    IotLogInfo( "Demo minimum ever free heap: %lu bytes.",
                ( unsigned long ) xPortGetMinimumEverFreeHeapSize() );
}

/*-----------------------------------------------------------*/

static void _mqttDemoTask( void * pArgument )
{
    ( void ) pArgument;

    _runDemoFunction( RunMqttDemo );

    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/

void vStartMQTTDemo( void )
{
    ( void ) xTaskCreate( _mqttDemoTask,
                          "MQTTDemo",
                          IOT_THREAD_DEFAULT_STACK_SIZE,
                          NULL,
                          IOT_THREAD_DEFAULT_PRIORITY,
                          NULL );
}

/*-----------------------------------------------------------*/
