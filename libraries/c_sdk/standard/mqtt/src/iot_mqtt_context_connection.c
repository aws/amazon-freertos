/*
 * FreeRTOS MQTT V2.3.1
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
 * https://github.com/freertos
 * https://www.FreeRTOS.org
 */

/**
 * @file iot_mqtt_context_connection.c
 * @brief Implements data structure for storing the mapping of the MQTT Context and
 * MQTT Connection.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* MQTT internal includes. */
#include "private/iot_mqtt_internal.h"

/* Platform layer includes. */
#include "platform/iot_threads.h"

/* Using initialized connToContext variable. */
extern _connContext_t connToContext[ MAX_NO_OF_MQTT_CONNECTIONS ];


/*-----------------------------------------------------------*/

int8_t _IotMqtt_getFreeIndexFromContextConnectionArray( void )
{
    int8_t contextIndex = 0;

    /* Getting the free index from the mapping data structure to hold the new mapping. */
    while( contextIndex < MAX_NO_OF_MQTT_CONNECTIONS )
    {
        if( connToContext[ contextIndex ].mqttConnection == NULL )
        {
            break;
        }

        contextIndex = contextIndex + 1;
    }

    if( contextIndex == MAX_NO_OF_MQTT_CONNECTIONS )
    {
        contextIndex = -1;
    }

    return contextIndex;
}

/*-----------------------------------------------------------*/

int8_t _IotMqtt_getContextIndexFromConnection( IotMqttConnection_t mqttConnection )
{
    int8_t contextIndex = 0;

    IotMqtt_Assert( mqttConnection != NULL );

    /* Getting the index of context from the mapping data structure for the given MQTT Connection. */
    while( contextIndex < MAX_NO_OF_MQTT_CONNECTIONS )
    {
        if( connToContext[ contextIndex ].mqttConnection == mqttConnection )
        {
            break;
        }

        contextIndex = contextIndex + 1;
    }

    if( contextIndex == MAX_NO_OF_MQTT_CONNECTIONS )
    {
        contextIndex = -1;
    }

    return contextIndex;
}

/*-----------------------------------------------------------*/

void _IotMqtt_removeContext( IotMqttConnection_t mqttConnection )
{
    int8_t contextIndex = 0;

    IotMqtt_Assert( mqttConnection != NULL );

    /* Finding the index corresponding to the given MQTT Connection. */
    while( contextIndex < MAX_NO_OF_MQTT_CONNECTIONS )
    {
        if( connToContext[ contextIndex ].mqttConnection == mqttConnection )
        {
            memset( &connToContext[ contextIndex ], 0x00, sizeof( _connContext_t ) );
            break;
        }

        contextIndex = contextIndex + 1;
    }
}

/*-----------------------------------------------------------*/

/* Provide access to internal functions and variables if testing. */
#if IOT_BUILD_TESTS == 1
    #include "iot_test_access_mqtt_context_connection.c"
#endif
