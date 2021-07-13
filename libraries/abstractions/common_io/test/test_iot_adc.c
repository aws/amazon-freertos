/*
 * FreeRTOS Common IO V0.1.3
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/*******************************************************************************
 * IOT Functional Unit Test - ADC
 *******************************************************************************
 */

/* Test includes */
#include "unity.h"
#include "unity_fixture.h"

/* ADC HAL driver include */
#include "iot_adc.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "semphr.h"
#include "string.h"

/* ADC test user context type */
typedef struct IotAdcTestUserContext_s
{
    IotAdcHandle_t xAdcHandle;
    uint8_t ucAdcChannel;
    uint8_t ucDataSize;
} testIotAdcTestUserContext_t;

#define testIotADC_CH_DATA_BUF_LEN    5
static SemaphoreHandle_t xtestIotAdcTestSemaphore = NULL;

/* global variables for channel test control */
/* ADC module instance, default to 0 */
uint8_t uctestIotAdcInstance = 0;
/* Number of ADC channels used for testing, defined by board specific test code */
uint8_t uctestIotAdcChListLen = 1;
/* ADC Channel list used for testing, the list is defined by board specific test code */
uint8_t * puctestIotAdcChList = NULL;
/* ADC module instance for assisted test, need to be configured to the channel with external access */
uint8_t ucAssistedTestIotAdcChannel = 0;

/* ADC Channel Scan Wait time in ms */
uint32_t ltestIotAdcTestChWaitTime = 1000;

/* Define Test Group. */
TEST_GROUP( TEST_IOT_ADC );
/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_ADC )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_ADC )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_ADC )
{
    xtestIotAdcTestSemaphore = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_EQUAL( NULL, xtestIotAdcTestSemaphore );

    RUN_TEST_CASE( TEST_IOT_ADC, AFQP_IotAdcOpenClose );
    RUN_TEST_CASE( TEST_IOT_ADC, AFQP_IotAdcConfig );
    RUN_TEST_CASE( TEST_IOT_ADC, AFQP_IotAdcGetChStatus );
    RUN_TEST_CASE( TEST_IOT_ADC, AFQP_IotAdcReadSample );
    RUN_TEST_CASE( TEST_IOT_ADC, AFQP_IotAdcChBuffer );
    RUN_TEST_CASE( TEST_IOT_ADC, AFQP_IotAdcSetChain );
    RUN_TEST_CASE( TEST_IOT_ADC, AFQP_IotAdcOperation );
    RUN_TEST_CASE( TEST_IOT_ADC, AFQP_IotAdcOpenCloseFuzzy );
    RUN_TEST_CASE( TEST_IOT_ADC, AFQP_IotAdcIoctlSetConfigFuzzy );
    RUN_TEST_CASE( TEST_IOT_ADC, AFQP_IotAdcSetChainFuzzy );
    RUN_TEST_CASE( TEST_IOT_ADC, AFQP_IotAdcOperationFuzzy );
}

/*-----------------------------------------------------------*/

/* ADC channel callback function */
static void prvAdcChCallback( uint16_t * pusConvertedData,
                              void * pvUserContext )
{
    testIotAdcTestUserContext_t * pxUserCntx = ( testIotAdcTestUserContext_t * ) pvUserContext;
    BaseType_t xHigherPriorityTaskWoken;
    uint8_t i;

    for( i = 0; i < pxUserCntx->ucDataSize; i++ )
    {
        TEST_ASSERT_NOT_EQUAL( 0, pusConvertedData[ i ] );
    }

    xSemaphoreGiveFromISR( xtestIotAdcTestSemaphore, &xHigherPriorityTaskWoken );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for iot_adc_open and iot_adc_close.
 */
TEST( TEST_IOT_ADC, AFQP_IotAdcOpenClose )
{
    IotAdcHandle_t xAdcHandle;
    int32_t lRetVal;

    xAdcHandle = iot_adc_open( uctestIotAdcInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xAdcHandle );

    lRetVal = iot_adc_close( xAdcHandle );
    TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
}


/*-----------------------------------------------------------*/

/**
 * @brief Test Function for ADC ioctl eAdcSetConfig/eAdcGetConfig
 */
TEST( TEST_IOT_ADC, AFQP_IotAdcConfig )
{
    IotAdcHandle_t xAdcHandle;
    IotAdcConfig_t xAdcConfig;
    int32_t lRetVal;

    xAdcHandle = iot_adc_open( uctestIotAdcInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xAdcHandle );

    if( TEST_PROTECT() )
    {
        xAdcConfig.ulAdcSampleTime = 100;
        xAdcConfig.ucAdcResolution = 12;
        lRetVal = iot_adc_ioctl( xAdcHandle, eSetAdcConfig, &xAdcConfig );
        TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );

        xAdcConfig.ulAdcSampleTime = 0;
        xAdcConfig.ucAdcResolution = 0;
        lRetVal = iot_adc_ioctl( xAdcHandle, eGetAdcConfig, &xAdcConfig );
        TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
        TEST_ASSERT_GREATER_THAN( 0, xAdcConfig.ulAdcSampleTime );
        TEST_ASSERT_GREATER_THAN( 0, xAdcConfig.ucAdcResolution );
    }

    lRetVal = iot_adc_close( xAdcHandle );
    TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for ADC ioctl eGetChStatus
 */
TEST( TEST_IOT_ADC, AFQP_IotAdcGetChStatus )
{
    IotAdcChStatus_t xAdcChStatus;
    testIotAdcTestUserContext_t xUserCntx;
    int32_t lRetVal;
    uint8_t ucChX, ucChY;

    TEST_ASSERT_GREATER_THAN( 1, uctestIotAdcChListLen );
    TEST_ASSERT_NOT_EQUAL( NULL, puctestIotAdcChList );
    ucChX = puctestIotAdcChList[ 0 ];
    ucChY = puctestIotAdcChList[ 1 ];

    xUserCntx.xAdcHandle = iot_adc_open( uctestIotAdcInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xUserCntx.xAdcHandle );

    if( TEST_PROTECT() )
    {
        xUserCntx.ucAdcChannel = ucChX;
        xUserCntx.ucDataSize = 1;
        /* set channel callback */
        iot_adc_set_callback( xUserCntx.xAdcHandle, xUserCntx.ucAdcChannel, prvAdcChCallback, &xUserCntx );

        /* start channel data scan on channel x */
        lRetVal = iot_adc_start( xUserCntx.xAdcHandle, xUserCntx.ucAdcChannel );
        TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );

        /* query channel state for channel x */
        xAdcChStatus.ucAdcChannel = ucChX;
        lRetVal = iot_adc_ioctl( xUserCntx.xAdcHandle, eGetChStatus, &xAdcChStatus );
        TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
        /* since channel x is used, expected to return busy */
        TEST_ASSERT_EQUAL( ucChX, xAdcChStatus.ucAdcChannel );
        TEST_ASSERT_EQUAL( eChStateBusy, xAdcChStatus.xAdcChState );

        /* query channel state for channel y */
        xAdcChStatus.ucAdcChannel = ucChY;
        lRetVal = iot_adc_ioctl( xUserCntx.xAdcHandle, eGetChStatus, &xAdcChStatus );
        TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
        /* since channel y is not used, expected to return idle */
        TEST_ASSERT_EQUAL( ucChY, xAdcChStatus.ucAdcChannel );
        TEST_ASSERT_EQUAL( eChStateIdle, xAdcChStatus.xAdcChState );

        /* stop channel data scan on channel x */
        lRetVal = iot_adc_stop( xUserCntx.xAdcHandle, xUserCntx.ucAdcChannel );
        TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );

        /* consume all semaphores given due to Async read callbacks which the test didn't care about */
        while( pdTRUE == xSemaphoreTake( xtestIotAdcTestSemaphore, 0 ) )
        {
        }

        /* query channel state for channel x */
        xAdcChStatus.ucAdcChannel = ucChX;
        lRetVal = iot_adc_ioctl( xUserCntx.xAdcHandle, eGetChStatus, &xAdcChStatus );
        TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
        /* since channel x is stop, expected to return idle */
        TEST_ASSERT_EQUAL( ucChX, xAdcChStatus.ucAdcChannel );
        TEST_ASSERT_EQUAL( eChStateIdle, xAdcChStatus.xAdcChState );
    }

    lRetVal = iot_adc_close( xUserCntx.xAdcHandle );
    TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function for ADC read sample.
 */
TEST( TEST_IOT_ADC, AFQP_IotAdcReadSample )
{
    IotAdcHandle_t xAdcHandle;
    int32_t lRetVal;
    uint16_t usSample;
    int8_t i;

    /* make sure ADC channel list has been configured */
    TEST_ASSERT_NOT_EQUAL( NULL, puctestIotAdcChList );

    xAdcHandle = iot_adc_open( uctestIotAdcInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xAdcHandle );

    if( TEST_PROTECT() )
    {
        /* read sample from ADC channels */
        for( i = 0; i < uctestIotAdcChListLen; i++ )
        {
            lRetVal = iot_adc_read_sample( xAdcHandle, puctestIotAdcChList[ i ], &usSample );
            TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
            TEST_ASSERT_GREATER_THAN( 0, usSample );
        }
    }

    lRetVal = iot_adc_close( xAdcHandle );
    TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to print ADC read sample for assisted test.
 */
TEST( TEST_IOT_ADC, AFQP_IotAdcPrintReadSample )
{
    IotAdcHandle_t xAdcHandle;
    int32_t lRetVal;
    uint16_t usSample;

    /* make sure ADC channel list has been configured */
    TEST_ASSERT_NOT_EQUAL( NULL, puctestIotAdcChList );

    xAdcHandle = iot_adc_open( uctestIotAdcInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xAdcHandle );

    if( TEST_PROTECT() )
    {
        /* read sample from ADC channels */
        lRetVal = iot_adc_read_sample( xAdcHandle, ucAssistedTestIotAdcChannel, &usSample );
        TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
    }

    lRetVal = iot_adc_close( xAdcHandle );
    TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );

    /* print ADC sample with an assert failure */
    TEST_ASSERT_EQUAL_INT( -1, usSample );
}

/*-----------------------------------------------------------*/

TEST( TEST_IOT_ADC, AFQP_IotAdcChBuffer )
{
    testIotAdcTestUserContext_t xUserCntx;
    IotAdcChBuffer_t xChBuf;
    uint16_t usChDataBuf[ testIotADC_CH_DATA_BUF_LEN ];
    uint8_t ucCh;
    int32_t lRetVal;
    int8_t i;

    TEST_ASSERT_NOT_EQUAL( NULL, puctestIotAdcChList );

    /* open ADC module for operation */
    xUserCntx.xAdcHandle = iot_adc_open( uctestIotAdcInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xUserCntx.xAdcHandle );

    if( TEST_PROTECT() )
    {
        for( i = 0; i < uctestIotAdcChListLen; i++ )
        {
            ucCh = puctestIotAdcChList[ i ];

            /* configure channel buffer */
            xChBuf.ucAdcChannel = ucCh;
            xChBuf.pvBuffer = usChDataBuf;
            xChBuf.ucBufLen = sizeof( usChDataBuf );
            lRetVal = iot_adc_ioctl( xUserCntx.xAdcHandle, eSetChBuffer, &xChBuf );
            TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );

            /* set channel callback */
            xUserCntx.ucAdcChannel = ucCh;
            xUserCntx.ucDataSize = testIotADC_CH_DATA_BUF_LEN;
            iot_adc_set_callback( xUserCntx.xAdcHandle, xUserCntx.ucAdcChannel, prvAdcChCallback, &xUserCntx );

            /* start channel data scan on channel */
            memset( usChDataBuf, 0, sizeof( usChDataBuf ) );
            lRetVal = iot_adc_start( xUserCntx.xAdcHandle, xUserCntx.ucAdcChannel );
            TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
            lRetVal = xSemaphoreTake( xtestIotAdcTestSemaphore, ltestIotAdcTestChWaitTime );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

            /* start channel data scan on channel again to make sure we can sample repeatedly */
            memset( usChDataBuf, 0, sizeof( usChDataBuf ) );
            lRetVal = iot_adc_start( xUserCntx.xAdcHandle, xUserCntx.ucAdcChannel );
            TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
            lRetVal = xSemaphoreTake( xtestIotAdcTestSemaphore, ltestIotAdcTestChWaitTime );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

            /* stop channel data scan */
            lRetVal = iot_adc_stop( xUserCntx.xAdcHandle, xUserCntx.ucAdcChannel );
            TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
        }
    }

    /* close ADC module */
    lRetVal = iot_adc_close( xUserCntx.xAdcHandle );
    TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

TEST( TEST_IOT_ADC, AFQP_IotAdcSetChain )
{
    testIotAdcTestUserContext_t xUserCntx;
    IotAdcChain_t xAdcChain;
    uint16_t usChainBuf[ 16 ];
    uint8_t ucCh;
    int32_t lRetVal;
    int8_t i;

    /* open ADC module for operation */
    xUserCntx.xAdcHandle = iot_adc_open( uctestIotAdcInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xUserCntx.xAdcHandle );

    if( TEST_PROTECT() )
    {
        /* Chain size has to be greater than 1, otherwise no need to use Chain operation */
        TEST_ASSERT_GREATER_THAN( 1, uctestIotAdcChListLen );
        TEST_ASSERT_NOT_EQUAL( NULL, puctestIotAdcChList );
        ucCh = puctestIotAdcChList[ 0 ];

        /* configure ADC chain */
        xAdcChain.ucAdcChannel = ucCh;
        xAdcChain.pvBuffer = usChainBuf;
        /* TODO: need better solution. expect uctestIotAdcChListLen samples, each sample takes 2 bytes */
        xAdcChain.ucBufLen = uctestIotAdcChListLen * 2;
        xAdcChain.usChainMask = 0;

        for( i = 0; i < uctestIotAdcChListLen; i++ )
        {
            xAdcChain.usChainMask |= 1 << puctestIotAdcChList[ i ];
        }

        lRetVal = iot_adc_ioctl( xUserCntx.xAdcHandle, eSetAdcChain, &xAdcChain );

        if( lRetVal != IOT_ADC_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );

            /* set chain callback */
            xUserCntx.ucAdcChannel = ucCh;
            xUserCntx.ucDataSize = uctestIotAdcChListLen;
            iot_adc_set_callback( xUserCntx.xAdcHandle, xUserCntx.ucAdcChannel, prvAdcChCallback, &xUserCntx );

            /* start chain scan */
            memset( usChainBuf, 0, sizeof( usChainBuf ) );
            lRetVal = iot_adc_start( xUserCntx.xAdcHandle, xUserCntx.ucAdcChannel );
            TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
            lRetVal = xSemaphoreTake( xtestIotAdcTestSemaphore, ltestIotAdcTestChWaitTime );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

            /* start chain scan to make sure we can sample repeatedly */
            memset( usChainBuf, 0, sizeof( usChainBuf ) );
            lRetVal = iot_adc_stop( xUserCntx.xAdcHandle, xUserCntx.ucAdcChannel );
            TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
            lRetVal = iot_adc_start( xUserCntx.xAdcHandle, xUserCntx.ucAdcChannel );
            TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
            lRetVal = xSemaphoreTake( xtestIotAdcTestSemaphore, ltestIotAdcTestChWaitTime );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

            /* stop chain scan */
            lRetVal = iot_adc_stop( xUserCntx.xAdcHandle, xUserCntx.ucAdcChannel );
            TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
        }
    }

    /* close ADC module */
    lRetVal = iot_adc_close( xUserCntx.xAdcHandle );
    TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

TEST( TEST_IOT_ADC, AFQP_IotAdcOperation )
{
    testIotAdcTestUserContext_t xUserCntx;
    uint8_t ucCh;
    int32_t lRetVal;
    int8_t i;

    TEST_ASSERT_NOT_EQUAL( NULL, puctestIotAdcChList );

    /* open ADC module for operation */
    xUserCntx.xAdcHandle = iot_adc_open( uctestIotAdcInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xUserCntx.xAdcHandle );

    if( TEST_PROTECT() )
    {
        for( i = 0; i < uctestIotAdcChListLen; i++ )
        {
            ucCh = puctestIotAdcChList[ i ];

            /* set channel callback */
            xUserCntx.ucAdcChannel = ucCh;
            xUserCntx.ucDataSize = 1;
            iot_adc_set_callback( xUserCntx.xAdcHandle, xUserCntx.ucAdcChannel, prvAdcChCallback, &xUserCntx );

            /* start channel data scan on channel */
            lRetVal = iot_adc_start( xUserCntx.xAdcHandle, xUserCntx.ucAdcChannel );
            TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
            lRetVal = xSemaphoreTake( xtestIotAdcTestSemaphore, ltestIotAdcTestChWaitTime );
            TEST_ASSERT_EQUAL( pdTRUE, lRetVal );

            /* stop channel data scan */
            lRetVal = iot_adc_stop( xUserCntx.xAdcHandle, xUserCntx.ucAdcChannel );
            TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
        }
    }

    /* close ADC module */
    lRetVal = iot_adc_close( xUserCntx.xAdcHandle );
    TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Fuzzy Test Function for iot_adc_open and iot_adc_close.
 */
TEST( TEST_IOT_ADC, AFQP_IotAdcOpenCloseFuzzy )
{
    IotAdcHandle_t xAdcHandle, xAdcHandleTmp;
    int32_t lRetVal;

    /* invliad ADC instance number */
    xAdcHandle = iot_adc_open( 0xFFFFFFFF );
    TEST_ASSERT_EQUAL( NULL, xAdcHandle );

    xAdcHandle = iot_adc_open( -1 );
    TEST_ASSERT_EQUAL( NULL, xAdcHandle );

    /* open same ADC instance twice */
    xAdcHandle = iot_adc_open( uctestIotAdcInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xAdcHandle );

    if( TEST_PROTECT() )
    {
        xAdcHandleTmp = iot_adc_open( uctestIotAdcInstance );
        TEST_ASSERT_EQUAL( NULL, xAdcHandleTmp );

        /* close ADC instance with NULL handle */
        lRetVal = iot_adc_close( NULL );
        TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );
    }

    /* close same ADC instance twice */
    lRetVal = iot_adc_close( xAdcHandle );
    TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );

    lRetVal = iot_adc_close( xAdcHandle );
    TEST_ASSERT_EQUAL( IOT_ADC_NOT_OPEN, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Fuzzy Test Function for ioctl eSetAdcConfig
 */
TEST( TEST_IOT_ADC, AFQP_IotAdcIoctlSetConfigFuzzy )
{
    IotAdcHandle_t xAdcHandle;
    IotAdcConfig_t xAdcConfig;
    int32_t lRetVal;

    xAdcHandle = iot_adc_open( uctestIotAdcInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xAdcHandle );

    if( TEST_PROTECT() )
    {
        xAdcConfig.ucAdcResolution = 100; /* no such resolution */
        lRetVal = iot_adc_ioctl( xAdcHandle, eSetAdcConfig, &xAdcConfig );
        TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );

        xAdcConfig.ulAdcSampleTime = 0xFFFFFFFF; /* not expecting such large value for sample time */
        lRetVal = iot_adc_ioctl( xAdcHandle, eSetAdcConfig, &xAdcConfig );
        TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );

        /* use a set of valid sample time and resolution */
        xAdcConfig.ulAdcSampleTime = 3;
        xAdcConfig.ucAdcResolution = 12;
        lRetVal = iot_adc_ioctl( xAdcHandle, eSetAdcConfig, &xAdcConfig );
        TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );

        lRetVal = iot_adc_ioctl( xAdcHandle, eGetAdcConfig, &xAdcConfig );
        TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
        TEST_ASSERT_TRUE( xAdcConfig.ulAdcSampleTime >= 3 );
        TEST_ASSERT_TRUE( xAdcConfig.ucAdcResolution >= 12 );
    }

    lRetVal = iot_adc_close( xAdcHandle );
    TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Fuzzy Test Function for iot_adc_open and iot_adc_close.
 */
TEST( TEST_IOT_ADC, AFQP_IotAdcSetChainFuzzy )
{
    testIotAdcTestUserContext_t xUserCntx;
    IotAdcChain_t xAdcChain;
    uint16_t usChainBuf[ 16 ];
    uint8_t ucCh;
    int32_t lRetVal;
    int8_t i;

    /* open ADC module for operation */
    xUserCntx.xAdcHandle = iot_adc_open( uctestIotAdcInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xUserCntx.xAdcHandle );

    if( TEST_PROTECT() )
    {
        /* Chain size has to be greater than 1, otherwise no need to use Chain operation */
        TEST_ASSERT_GREATER_THAN( 1, uctestIotAdcChListLen );
        TEST_ASSERT_NOT_EQUAL( NULL, puctestIotAdcChList );
        ucCh = puctestIotAdcChList[ 0 ];

        /* configure ADC chain */
        xAdcChain.ucAdcChannel = ucCh;
        xAdcChain.pvBuffer = usChainBuf;
        xAdcChain.ucBufLen = sizeof( usChainBuf );
        xAdcChain.usChainMask = 0;

        /* ChainMask invalid */
        lRetVal = iot_adc_ioctl( xUserCntx.xAdcHandle, eSetAdcChain, &xAdcChain );

        if( lRetVal != IOT_ADC_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );

            for( i = 0; i < uctestIotAdcChListLen; i++ )
            {
                xAdcChain.usChainMask |= 1 << puctestIotAdcChList[ i ];
            }

            /* chain's logical channel has to be marked with lsb, below match msb */
            xAdcChain.ucAdcChannel = puctestIotAdcChList[ uctestIotAdcChListLen - 1 ];

            lRetVal = iot_adc_ioctl( xUserCntx.xAdcHandle, eSetAdcChain, &xAdcChain );

            if( lRetVal != IOT_ADC_FUNCTION_NOT_SUPPORTED )
            {
                TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );
            }
        }
    }

    /* close ADC module */
    lRetVal = iot_adc_close( xUserCntx.xAdcHandle );
    TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/

/**
 * @brief Fuzzy Test Function for iot_adc_open and iot_adc_close.
 */
TEST( TEST_IOT_ADC, AFQP_IotAdcOperationFuzzy )
{
    IotAdcHandle_t xAdcHandle = NULL;
    uint8_t ucAdcChannel = 0xFF; /* fake channel number */
    IotAdcConfig_t xAdcConfig;
    IotAdcChBuffer_t xAdcChBuffer = { ucAdcChannel, NULL, 0 };
    IotAdcChain_t xAdcChain = { ucAdcChannel, NULL, 0, 0x0 };
    uint16_t usDataBuf[ 5 ];
    int32_t lRetVal;

    lRetVal = iot_adc_start( xAdcHandle, ucAdcChannel );
    TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );

    lRetVal = iot_adc_stop( xAdcHandle, ucAdcChannel );
    TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );

    lRetVal = iot_adc_read_sample( xAdcHandle, ucAdcChannel, usDataBuf );
    TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );

    lRetVal = iot_adc_ioctl( xAdcHandle, eGetAdcConfig, NULL );
    TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );

    /* open ADC instance so we have valid xAdcHandle, channel still invalid */
    xAdcHandle = iot_adc_open( uctestIotAdcInstance );
    TEST_ASSERT_NOT_EQUAL( NULL, xAdcHandle );

    if( TEST_PROTECT() )
    {
        lRetVal = iot_adc_start( xAdcHandle, ucAdcChannel );
        TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );

        lRetVal = iot_adc_stop( xAdcHandle, ucAdcChannel );
        TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );

        lRetVal = iot_adc_read_sample( xAdcHandle, ucAdcChannel, usDataBuf );
        TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );

        lRetVal = iot_adc_ioctl( xAdcHandle, eGetAdcConfig, &xAdcConfig );
        TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );

        /* NULL pointer for ioctl */
        lRetVal = iot_adc_ioctl( xAdcHandle, eSetAdcConfig, NULL );
        TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );

        lRetVal = iot_adc_ioctl( xAdcHandle, eGetAdcConfig, NULL );
        TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );

        lRetVal = iot_adc_ioctl( xAdcHandle, eGetChStatus, NULL );
        TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );

        lRetVal = iot_adc_ioctl( xAdcHandle, eSetChBuffer, NULL );
        TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );

        lRetVal = iot_adc_ioctl( xAdcHandle, eSetAdcChain, NULL );

        if( lRetVal != IOT_ADC_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );
        }

        /* pick a valid ucAdcChannel from a global ADC channel list which is configured by the test based on board */
        TEST_ASSERT_NOT_EQUAL( NULL, puctestIotAdcChList );
        ucAdcChannel = puctestIotAdcChList[ 0 ];

        /* NULL pointer for data buffer */
        /* assign a valid channel number to xAdcChBuffer while keep xAdcChBuffer.pvBuffer = NULL */
        xAdcChBuffer.ucAdcChannel = ucAdcChannel;

        lRetVal = iot_adc_ioctl( xAdcHandle, eSetChBuffer, &xAdcChBuffer );
        TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );

        /* assign a valid channel number to xAdcChain while keep xAdcChain.pvBuffer = NULL */
        xAdcChain.ucAdcChannel = ucAdcChannel;

        lRetVal = iot_adc_ioctl( xAdcHandle, eSetAdcChain, &xAdcChain );

        if( lRetVal != IOT_ADC_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );
        }

        /* zero data buffer length */
        /* assign a valid data buffer pointer to  xAdcChBuffer while keep xAdcChBuffer.ucBufLen = 0 */
        xAdcChBuffer.pvBuffer = ( void * ) usDataBuf;

        lRetVal = iot_adc_ioctl( xAdcHandle, eSetChBuffer, &xAdcChBuffer );
        TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );

        /* assign a valid data buffer pointer to xAdcChain while keep xAdcChain.ucBufLen = 0 */
        xAdcChain.pvBuffer = ( void * ) usDataBuf;

        lRetVal = iot_adc_ioctl( xAdcHandle, eSetAdcChain, &xAdcChain );

        if( lRetVal != IOT_ADC_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );
        }

        /* invalid ADC chain mask */
        /* assign valid data buffer length while keep xAdcChain.usChainMask = 0x0 */
        xAdcChain.ucBufLen = sizeof( usDataBuf );

        lRetVal = iot_adc_ioctl( xAdcHandle, eSetAdcChain, &xAdcChain );

        if( lRetVal != IOT_ADC_FUNCTION_NOT_SUPPORTED )
        {
            TEST_ASSERT_EQUAL( IOT_ADC_INVALID_VALUE, lRetVal );
        }

        /* start shall fail without setting callback successfully */
        lRetVal = iot_adc_start( xAdcHandle, ucAdcChannel );
        TEST_ASSERT_EQUAL( IOT_ADC_FAILED, lRetVal );

        /* stop shall succeed unless handle is not valid */
        lRetVal = iot_adc_stop( xAdcHandle, ucAdcChannel );
        TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
    }

    lRetVal = iot_adc_close( xAdcHandle );
    TEST_ASSERT_EQUAL( IOT_ADC_SUCCESS, lRetVal );
}

/*-----------------------------------------------------------*/
