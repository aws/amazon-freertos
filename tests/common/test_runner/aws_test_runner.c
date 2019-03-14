/*
 * Amazon FreeRTOS Test Runner V1.1.4
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


/**
 * @file aws_test_runner.c
 * @brief The function to be called to run all the tests.
 */
#ifdef IOT_CONFIG_FILE
    #include IOT_CONFIG_FILE
#endif
/* Test runner interface includes. */
#include "aws_test_runner.h"

/* MQTT v4 header must be included if its tests are enabled. */
#if ( testrunnerFULL_MQTTv4_ENABLED == 1 )
    #include "iot_mqtt.h"
#endif

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Unity framework includes. */
#include "unity_fixture.h"
#include "unity_internals.h"
#include "iot_common.h"
/* Application version info. */
#include "aws_application_version.h"

const AppVersion32_t xAppFirmwareVersion =
{
    .u.x.ucMajor = APP_VERSION_MAJOR,
    .u.x.ucMinor = APP_VERSION_MINOR,
    .u.x.usBuild = APP_VERSION_BUILD,
};

char cBuffer[ testrunnerBUFFER_SIZE ];

/* Heap leak variables. */
unsigned int xHeapBefore;
unsigned int xHeapAfter;
/*-----------------------------------------------------------*/

/* This function will be generated by the test automation framework,
 * do not change the signature of it. You could, however, add or remove
 * RUN_TEST_GROUP statements.
 */
static void RunTests( void )
{
    /* Tests can be disabled in aws_test_runner_config.h */

    /* The Amazon FreeRTOS qualification program requires that Wi-Fi and TCP be the
     * first tests in this function. */
    #if ( testrunnerFULL_WIFI_ENABLED == 1 )
        RUN_TEST_GROUP( Full_WiFi );
    #endif

    #if ( testrunnnerFULL_WIFI_PROVISIONING_ENABLED == 1 )
        RUN_TEST_GROUP( Full_WiFi_Provisioning );
    #endif

    #if ( testrunnerFULL_TCP_ENABLED == 1 )
        RUN_TEST_GROUP( Full_TCP );
    #endif

    #if ( testrunnerFULL_GGD_ENABLED == 1 )
        RUN_TEST_GROUP( Full_GGD );
    #endif

    #if ( testrunnerFULL_GGD_HELPER_ENABLED == 1 )
        RUN_TEST_GROUP( Full_GGD_Helper );
    #endif

    #if ( testrunnerFULL_SHADOW_ENABLED == 1 )
        RUN_TEST_GROUP( Full_Shadow );
    #endif

    #if ( testrunnerFULL_SHADOWv4_ENABLED == 1 )

        /* The Shadow v4 tests perform their own initialization and cleanup. Clean
         * up the MQTT library here to avoid memory leaks. */
        AwsIotMqtt_Cleanup();

        RUN_TEST_GROUP( Shadow_Unit_Parser );
        RUN_TEST_GROUP( Shadow_Unit_API );
        RUN_TEST_GROUP( Shadow_System );

        /* Initialize the MQTT library for any tests that come after. */
        configASSERT( AwsIotMqtt_Init() == AWS_IOT_MQTT_SUCCESS );
    #endif /* if ( testrunnerFULL_SHADOWv4_ENABLED == 1 ) */

    #if ( testrunnerFULL_MQTTv4_ENABLED == 1 )

        /* The MQTT v4 tests perform their own initialization and cleanup. Clean
         * up the MQTT library here to avoid memory leaks. */
        IotCommon_Cleanup();
        IotMqtt_Cleanup();

        RUN_TEST_GROUP( MQTT_Unit_Validate );
        RUN_TEST_GROUP( MQTT_Unit_Subscription );
        RUN_TEST_GROUP( MQTT_Unit_Receive );
        RUN_TEST_GROUP( MQTT_Unit_API );
        RUN_TEST_GROUP( MQTT_System );

        IotCommon_Init();
        /* Initialize the MQTT library for any tests that come after. */
        configASSERT( IotMqtt_Init() == IOT_MQTT_SUCCESS );
    #endif /* if ( testrunnerFULL_MQTTv4_ENABLED == 1 ) */

    #if ( testrunnerFULL_MQTT_STRESS_TEST_ENABLED == 1 )
        RUN_TEST_GROUP( Full_MQTT_Agent_Stress_Tests );
    #endif

    #if ( testrunnerFULL_MQTT_AGENT_ENABLED == 1 )
        RUN_TEST_GROUP( Full_MQTT_Agent );
    #endif

    #if ( testrunnerFULL_MQTT_ALPN_ENABLED == 1 )
        RUN_TEST_GROUP( Full_MQTT_Agent_ALPN );
    #endif

    #if ( testrunnerFULL_OTA_CBOR_ENABLED == 1 )
        RUN_TEST_GROUP( Full_OTA_CBOR );
    #endif

    #if ( testrunnerFULL_OTA_AGENT_ENABLED == 1 )
        RUN_TEST_GROUP( Full_OTA_AGENT );
    #endif

    #if ( testrunnerFULL_OTA_PAL_ENABLED == 1 )
        RUN_TEST_GROUP( Full_OTA_PAL );
    #endif

    #if ( testrunnerFULL_PKCS11_ENABLED == 1 )
        RUN_TEST_GROUP( Full_PKCS11_CryptoOperation );
        RUN_TEST_GROUP( Full_PKCS11_GeneralPurpose );
    #endif

    #if ( testrunnerFULL_CRYPTO_ENABLED == 1 )
        RUN_TEST_GROUP( Full_CRYPTO );
    #endif

    #if ( testrunnerFULL_TLS_ENABLED == 1 )
        RUN_TEST_GROUP( Full_TLS );
    #endif

    #if ( testrunnerFULL_CBOR_ENABLED == 1 )
        RUN_TEST_GROUP( Full_CBOR );
    #endif

    #if ( testrunnerFULL_DEFENDER_ENABLED == 1 )
        RUN_TEST_GROUP( Full_DEFENDER );
        RUN_TEST_GROUP( Full_DEFENDER_OLD );
    #endif

    #if ( testrunnerFULL_POSIX_ENABLED == 1 )
        RUN_TEST_GROUP( Full_POSIX_CLOCK );
        RUN_TEST_GROUP( Full_POSIX_MQUEUE );
        RUN_TEST_GROUP( Full_POSIX_PTHREAD );
        RUN_TEST_GROUP( Full_POSIX_SEMAPHORE );
        RUN_TEST_GROUP( Full_POSIX_TIMER );
        RUN_TEST_GROUP( Full_POSIX_UTILS );
        RUN_TEST_GROUP( Full_POSIX_UNISTD );
        RUN_TEST_GROUP( Full_POSIX_STRESS );
    #endif

    #if ( testrunnerFULL_BLE_ENABLED == 1 )
        //RUN_TEST_GROUP( MQTT_Unit_BLE_Serialize );
        RUN_TEST_GROUP( Full_BLE );
    #endif

#if ( testrunnerFULL_BLE_END_TO_END_TEST_ENABLED == 1 )
    RUN_TEST_GROUP( Full_BLE_END_TO_END );
#endif

    #if ( testrunnerFULL_FREERTOS_TCP_ENABLED == 1 )
        RUN_TEST_GROUP( Full_FREERTOS_TCP );
    #endif

    #if ( testrunnerOTA_END_TO_END_ENABLED == 1 )
        extern void vStartOTAUpdateDemoTask( void );
        vStartOTAUpdateDemoTask();
    #endif

    #if ( testrunnerFULL_SERIALIZER_ENABLED == 1 )
        RUN_TEST_GROUP(Full_Serializer_CBOR);
        RUN_TEST_GROUP(Full_Serializer_JSON);
    #endif
}
/*-----------------------------------------------------------*/

void TEST_RUNNER_RunTests_task( void * pvParameters )
{
    /* Initialize unity. */
    UnityFixture.Verbose = 1;
    UnityFixture.GroupFilter = 0;
    UnityFixture.NameFilter = testrunnerTEST_FILTER;
    UnityFixture.RepeatCount = 1;

    UNITY_BEGIN();

    /* Give the print buffer time to empty */
    vTaskDelay( pdMS_TO_TICKS( 500) );
    /* Measure the heap size before any tests are run. */
    #if ( testrunnerFULL_MEMORYLEAK_ENABLED == 1 )
        xHeapBefore = xPortGetFreeHeapSize();
    #endif

    RunTests();

    #if ( testrunnerFULL_MEMORYLEAK_ENABLED == 1 )

        /* Measure the heap size after tests are done running.
         * This test must run last. */

        /* Perform any global resource cleanup necessary to avoid memory leaks. */
        #ifdef testrunnerMEMORYLEAK_CLEANUP
            testrunnerMEMORYLEAK_CLEANUP();
        #endif

        /* Give the print buffer time to empty */
        vTaskDelay( pdMS_TO_TICKS( 500) );
        xHeapAfter = xPortGetFreeHeapSize();
        RUN_TEST_GROUP( Full_MemoryLeak );
    #endif /* if ( testrunnerFULL_MEMORYLEAK_ENABLED == 1 ) */

    /* Currently disabled. Will be enabled after cleanup. */
    UNITY_END();

    #ifdef CODE_COVERAGE
        exit( 0 );
    #endif

    /* This task has finished.  FreeRTOS does not allow a task to run off the
     * end of its implementing function, so the task must be deleted. */
    vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/
