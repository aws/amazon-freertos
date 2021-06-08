/*
 * FreeRTOS Common IO V0.1.3
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

/*******************************************************************************
 * @file test_iot_sdio.c
 * @brief Functional Unit Test - SDIO
 *******************************************************************************
 */

/* Test includes */
#include "unity.h"
#include "unity_fixture.h"

/* Driver includes */
#include "iot_sdio.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/*-----------------------------------------------------------*/
#define testIotSdio_CMD52TESTADDR              ( 0x04 )
#define testIotSdio_EHS_ADDR                   ( 0x13 )
#define testIotSdio_EHS_ADDR                   ( 0x13 )
#define testIotSdio_SHS_MASK                   ( 0x01 )
#define testIotSdio_EHS_MASK                   ( 0x02 )
#define testIotSdio_DTS_ADDR                   ( 0x15 )
#define testIotSdio_DTS_MASK                   ( 0x30 )
#define testIotSdio_DTS_BITS                   ( 0x04 )
#define testIotSdio_DTS_TYPE_MASK              ( 0x01 )
#define testIotSdio_IO_EN_ADDR                 ( 0x02 )
#define testIotSdio_IO1_EN_MASK                ( 0x02 )
#define testIotSdio_FUNC_SEL_ADDR              ( 0x0d )
#define testIotSdio_FUNC_SEL_MASK              ( 0x0f )
#define testIotSdio_BUS_CTL_ADDR               ( 0x07 )
#define testIotSdio_BUS_WIDTH_MASK             ( 0x03 )


#define testIotSdio_FUNC1_CIS_PTR_ADDR         ( 0x0109 )
#define testIotSdio_FUNC1_BLK_SIZ_ADDR         ( 0x0110 )

#define testIotSdio_TPL_CODE_FUNCID            ( 0x21 )
#define testIotSdio_TPL_CODE_FUNCE             ( 0x22 )
#define testIotSdio_FUNC_TPL_CNT               ( 0x2 )

#define testIotSdio_OFFSET_LEN                 ( 0x3 )
#define testIotSdio_BLKSIZ_LEN                 ( 0x2 )
#define testIotSdio_EXTENDED_READ_TEST_SIZE    ( 0x31 )

#define testIotSdio_HANDLE_HOST_NUM            ( 2 )
#define testIotSdio_HANDLE_SLOT_NUM            ( 7 )
#define testIotSdio_HOST_INSTANCE              uctestIotSdioValidHostIdx
#define testIotSdio_SLOT_INSTANCE              uctestIotSdioValidSlotIdx

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/** function declarations */
/*-----------------------------------------------------------*/
void prvSdioCallback( IotSdioSlotHandle_t const pxSdioHandle,
                      IotSdioEventType_t eSdioEvent,
                      void * pvUserContext );


/*-----------------------------------------------------------*/

/* Globals values which can be overwritten by the test
 * framework invoking these tests */
/*-----------------------------------------------------------*/
uint8_t uctestIotSdioValidHostIdx = 0;                        /**< A valid host index number. */
uint8_t uctestIotSdioInvalidHostIdx = 2;                      /**< A invalid host index number. */
uint8_t uctestIotSdioValidSlotIdx = 0;                        /**< A valid Slot index number. */
uint8_t uctestIotSdioInvalidSlotIdx = 1;                      /**< A invalid Slot index number. */
uint8_t uctestIotSdioInvalidFuncNmbr = 9;                     /**< A invalid i/o function number. */
IotSdioCallback_t xtestIotSdioTestCallback = prvSdioCallback; /**< A sdio callback function. */
IotSdioCardDetectParam_t xtestIotSdioCardDetectParam =
{ eDetectCardByHostCD, false, false };                        /**< A sdio card detection params. */
IotSdioBusWidth_t etestIotSdioBusWidth = eDataBus4Bit;        /**< sdio data bus width */
IotSdioSdDriverStrength_t etestIotSdioDriverType =
    eSdDriverStrengthTypeD;                                   /**< Driver strength type. */
bool btestIotSdioVerifyInactive = true;                       /**< Verify card inactive state or not. */


/*-----------------------------------------------------------*/
/** Static globals */
/*-----------------------------------------------------------*/
static const uint32_t g_funcTupleList[ testIotSdio_FUNC_TPL_CNT ] =
{
    testIotSdio_TPL_CODE_FUNCID, testIotSdio_TPL_CODE_FUNCE,
};

extern IotSdioSlotHandle_t gIotSdioHandle[ testIotSdio_HANDLE_HOST_NUM ][ testIotSdio_HANDLE_SLOT_NUM ];
/*-----------------------------------------------------------*/

/* Define Test Group. */
TEST_GROUP( TEST_IOT_SDIO );

/*-----------------------------------------------------------*/

/**
 * @brief Setup function called before each test in this group is executed.
 */
TEST_SETUP( TEST_IOT_SDIO )
{
    IotSdioSlotHandle_t xGlobalSdioHandle = gIotSdioHandle[ testIotSdio_HOST_INSTANCE ][ testIotSdio_SLOT_INSTANCE ];
    int32_t lRetVal;

    /* Close global handle it was opened. */
    if( xGlobalSdioHandle != NULL )
    {
        lRetVal = iot_i2c_close( xGlobalSdioHandle );
        TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

        gIotSdioHandle[ testIotSdio_HOST_INSTANCE ][ testIotSdio_SLOT_INSTANCE ] = NULL;
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Tear down function called after each test in this group is executed.
 */
TEST_TEAR_DOWN( TEST_IOT_SDIO )
{
}

/*-----------------------------------------------------------*/

/**
 * @brief user callback function
 */
void prvSdioCallback( IotSdioSlotHandle_t const pxSdioHandle,
                      IotSdioEventType_t eSdioEvent,
                      void * pvUserContext )
{
    switch( eSdioEvent )
    {
        case eSdioCardInterruptEvent:
            break;

        case eSdioCardInsertedEvent:
            break;

        case eSdioCardRemovedEvent:
            break;

        case eSdioCardPowerOnEvent:
            break;

        case eSdioCardPowerOffEvent:
            break;

        default:
            break;
    }
}

/*-----------------------------------------------------------*/

/**
 * @brief Function to define which tests to execute as part of this group.
 */
TEST_GROUP_RUNNER( TEST_IOT_SDIO )
{
    RUN_TEST_CASE( TEST_IOT_SDIO, AFQP_IotSdioOpenClose );
    RUN_TEST_CASE( TEST_IOT_SDIO, AFQP_IotSdioDoubleOpenDoubleClose );
    RUN_TEST_CASE( TEST_IOT_SDIO, AFQP_IotSdioOpenCloseFuzzing );
    RUN_TEST_CASE( TEST_IOT_SDIO, AFQP_IotSdioCardConnectDisconnect );
    RUN_TEST_CASE( TEST_IOT_SDIO, AFQP_IotSdioCardConnectDisconnectFuzzing );
    RUN_TEST_CASE( TEST_IOT_SDIO, AFQP_IotSdioWriteReadDirect );
    RUN_TEST_CASE( TEST_IOT_SDIO, AFQP_IotSdioWriteReadDirectFuzzing );
    RUN_TEST_CASE( TEST_IOT_SDIO, AFQP_IotSdioWriteReadExtended );
    RUN_TEST_CASE( TEST_IOT_SDIO, AFQP_IotSdioWriteReadExtendedFuzzing );
    RUN_TEST_CASE( TEST_IOT_SDIO, AFQP_IotSdioIoctl );
    RUN_TEST_CASE( TEST_IOT_SDIO, AFQP_IotSdioIoctlFuzzing );
}

/*-----------------------------------------------------------*/

/**
 * @brief Test Function to test iot_sdio_open and iot_sdio_close.
 *
 */
TEST( TEST_IOT_SDIO, AFQP_IotSdioOpenClose )
{
    IotSdioSlotHandle_t xSdioHandle = NULL;

    int32_t lRetVal;

    /* Open with valid host index and slot index should return a non NULL handle. */
    xSdioHandle = iot_sdio_open( uctestIotSdioValidHostIdx,
                                 uctestIotSdioValidSlotIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xSdioHandle );

    /* Closing a valid handle should return success. */
    lRetVal = iot_sdio_close( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test double open and double close.
 *
 */
TEST( TEST_IOT_SDIO, AFQP_IotSdioDoubleOpenDoubleClose )
{
    IotSdioSlotHandle_t xSdioHandle = NULL;
    IotSdioSlotHandle_t xSdioHandle2 = NULL;

    int32_t lRetVal;

    /* Open with valid host index and slot index should return a non NULL handle. */
    xSdioHandle = iot_sdio_open( uctestIotSdioValidHostIdx,
                                 uctestIotSdioValidSlotIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xSdioHandle );

    /* Try to open the same card slot again should return NULL. */
    xSdioHandle2 = iot_sdio_open( uctestIotSdioValidHostIdx,
                                  uctestIotSdioValidSlotIdx );
    TEST_ASSERT_EQUAL( NULL, xSdioHandle2 );

    /* Closing a valid handle should return success. */
    lRetVal = iot_sdio_close( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Closing an already closed handle should return INVALID_VALUE. */
    lRetVal = iot_sdio_close( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );
}

/**
 * @brief Test Function to test iot_sdio_open with invalid inputs.
 *
 */
TEST( TEST_IOT_SDIO, AFQP_IotSdioOpenCloseFuzzing )
{
    IotSdioSlotHandle_t xSdioHandle = NULL;
    int32_t lRetVal;

    /* Open with invalid host index returns NULL. */
    xSdioHandle = iot_sdio_open( uctestIotSdioInvalidHostIdx,
                                 uctestIotSdioValidSlotIdx );
    TEST_ASSERT_EQUAL( NULL, xSdioHandle );

    /* Open with invalid card slot index returns NULL. */
    xSdioHandle = iot_sdio_open( uctestIotSdioValidHostIdx,
                                 uctestIotSdioInvalidSlotIdx );
    TEST_ASSERT_EQUAL( NULL, xSdioHandle );

    /* Open with invalid host index and invalid card slot index returns NULL. */
    xSdioHandle = iot_sdio_open( uctestIotSdioInvalidHostIdx,
                                 uctestIotSdioInvalidSlotIdx );
    TEST_ASSERT_EQUAL( NULL, xSdioHandle );

    /* Try to close a null handle should return INVALID_VALUE. */
    lRetVal = iot_sdio_close( NULL );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );
}

/**
 * @brief Test Function to test sdio card connect and disconnect.
 *
 */
TEST( TEST_IOT_SDIO, AFQP_IotSdioCardConnectDisconnect )
{
    IotSdioSlotHandle_t xSdioHandle = NULL;
    int32_t lRetVal;
    bool bCardPresent = false;
    uint8_t ucReadVal = 0;
    IoTSdioPowerOnOffCard_t xCardPowerCtl = { false, false };

    xSdioHandle = iot_sdio_open( uctestIotSdioValidHostIdx,
                                 uctestIotSdioValidSlotIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xSdioHandle );

    lRetVal = iot_sdio_set_sdio_callback( xSdioHandle, xtestIotSdioTestCallback, NULL );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    lRetVal = iot_sdio_ioctl( xSdioHandle, eSDIOSetCardDetectParams, ( void * ) &xtestIotSdioCardDetectParam );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* some card needs to be powered off to be detected. */
    xCardPowerCtl.bPowerOn = false;
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOPowerOnOffCard,
                              ( void * ) &xCardPowerCtl );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* detect card */
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOCheckCardPresence,
                              ( void * ) &bCardPresent );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
    TEST_ASSERT_EQUAL( true, bCardPresent );

    /* Power on card. Power on is validated later by card connect. */
    xCardPowerCtl.bPowerOn = true;
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOPowerOnOffCard,
                              ( void * ) &xCardPowerCtl );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Try to access card before card connect should fail */
    lRetVal = iot_sdio_io_read_direct( xSdioHandle,
                                       eSdioFunction0,
                                       testIotSdio_CMD52TESTADDR,
                                       ( uint8_t * ) &ucReadVal );
    TEST_ASSERT_NOT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    lRetVal = iot_sdio_card_connect( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Try to access card after card connect should succeed */
    lRetVal = iot_sdio_io_read_direct( xSdioHandle,
                                       eSdioFunction0,
                                       testIotSdio_CMD52TESTADDR,
                                       ( uint8_t * ) &ucReadVal );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    lRetVal = iot_sdio_card_disconnect( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Try to access card after card disconnect should fail */
    lRetVal = iot_sdio_io_read_direct( xSdioHandle,
                                       eSdioFunction0,
                                       testIotSdio_CMD52TESTADDR,
                                       ( uint8_t * ) &ucReadVal );
    TEST_ASSERT_NOT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Closing a valid handle should return success. */
    lRetVal = iot_sdio_close( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Calling card_connect() after iot_sdio_close() should fail */
    lRetVal = iot_sdio_card_connect( xSdioHandle );
    TEST_ASSERT_NOT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test sdio card connect and disconnect with invalid input.
 *
 */
TEST( TEST_IOT_SDIO, AFQP_IotSdioCardConnectDisconnectFuzzing )
{
    int32_t lRetVal;

    /* Should return IOT_SDIO_INVALID_VALUE on NULL handle */
    lRetVal = iot_sdio_card_connect( NULL );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );
    /* Should return IOT_SDIO_INVALID_VALUE on NULL handle */
    lRetVal = iot_sdio_card_disconnect( NULL );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );
}


/**
 * @brief Test Function to test iot_sdio_io_read_direct()
 * iot_sdio_io_write_direct().
 *
 */
TEST( TEST_IOT_SDIO, AFQP_IotSdioWriteReadDirect )
{
    IotSdioSlotHandle_t xSdioHandle = NULL;
    int32_t lRetVal;
    uint8_t ucBackupVal = 0, ucReadVal = 0, ucWriteVal = 0;
    bool bCardPresent = false;
    IoTSdioPowerOnOffCard_t xCardPowerCtl = { false, false };

    /* Open with valid host index and slot index should return a non NULL handle. */
    xSdioHandle = iot_sdio_open( uctestIotSdioValidHostIdx,
                                 uctestIotSdioValidSlotIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xSdioHandle );

    lRetVal = iot_sdio_set_sdio_callback( xSdioHandle, xtestIotSdioTestCallback, NULL );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    lRetVal = iot_sdio_ioctl( xSdioHandle, eSDIOSetCardDetectParams, ( void * ) &xtestIotSdioCardDetectParam );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* some card needs to be powered off to be detected. */
    xCardPowerCtl.bPowerOn = false;
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOPowerOnOffCard,
                              ( void * ) &xCardPowerCtl );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* detect card */
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOCheckCardPresence,
                              ( void * ) &bCardPresent );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
    TEST_ASSERT_EQUAL( true, bCardPresent );

    /* Power on card. Power on is validated later by card connect. */
    xCardPowerCtl.bPowerOn = true;
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOPowerOnOffCard,
                              ( void * ) &xCardPowerCtl );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    lRetVal = iot_sdio_card_connect( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Read orginal value from register and save it */
    lRetVal = iot_sdio_io_read_direct( xSdioHandle,
                                       eSdioFunction0,
                                       testIotSdio_CMD52TESTADDR,
                                       ( uint8_t * ) &ucBackupVal );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Write a different value (bit-0 flipped) to register */
    ucWriteVal = ( ucBackupVal & 0xFE ) | ( ( ~ucBackupVal ) & 0x01 );
    lRetVal = iot_sdio_io_write_direct( xSdioHandle,
                                        eSdioFunction0,
                                        testIotSdio_CMD52TESTADDR,
                                        ( uint8_t * ) &ucWriteVal,
                                        false );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Read it back and compare */
    lRetVal = iot_sdio_io_read_direct( xSdioHandle,
                                       eSdioFunction0,
                                       testIotSdio_CMD52TESTADDR,
                                       ( uint8_t * ) &ucReadVal );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
    TEST_ASSERT_EQUAL( ucReadVal, ucWriteVal );


    /* Write back the original value */
    lRetVal = iot_sdio_io_write_direct( xSdioHandle,
                                        eSdioFunction0,
                                        testIotSdio_CMD52TESTADDR,
                                        ( uint8_t * ) &ucBackupVal,
                                        false );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    lRetVal = iot_sdio_card_disconnect( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Closing a valid handle should return success. */
    lRetVal = iot_sdio_close( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test iot_sdio_io_read_direct()
 * iot_sdio_io_write_direct() with invalid inputs.
 *
 */
TEST( TEST_IOT_SDIO, AFQP_IotSdioWriteReadDirectFuzzing )
{
    IotSdioSlotHandle_t xSdioHandle = NULL;
    int32_t lRetVal;
    uint8_t ucReadVal = 0, ucWriteVal = 0;

    /* Open with valid host index and slot index should return a non NULL handle. */
    xSdioHandle = iot_sdio_open( uctestIotSdioValidHostIdx,
                                 uctestIotSdioValidSlotIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xSdioHandle );

    /* Should return IOT_SDIO_INVALID_VALUE on NULL handle */
    lRetVal = iot_sdio_io_read_direct( NULL,
                                       eSdioFunction0,
                                       testIotSdio_CMD52TESTADDR,
                                       ( uint8_t * ) &ucReadVal );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );

    /* Should return IOT_SDIO_INVALID_VALUE on invalid i/o function number */
    lRetVal = iot_sdio_io_read_direct( xSdioHandle,
                                       -1,
                                       testIotSdio_CMD52TESTADDR,
                                       ( uint8_t * ) &ucReadVal );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );

    /* Should return IOT_SDIO_INVALID_VALUE on NULL read buffer */
    lRetVal = iot_sdio_io_read_direct( xSdioHandle,
                                       eSdioFunction0,
                                       testIotSdio_CMD52TESTADDR,
                                       NULL );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );

    /* Should return IOT_SDIO_INVALID_VALUE on NULL handle */
    lRetVal = iot_sdio_io_write_direct( NULL,
                                        eSdioFunction0,
                                        testIotSdio_CMD52TESTADDR,
                                        ( uint8_t * ) &ucWriteVal,
                                        false );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );

    /* Should return IOT_SDIO_INVALID_VALUE on invalid i/o function number */
    lRetVal = iot_sdio_io_write_direct( xSdioHandle,
                                        -1,
                                        testIotSdio_CMD52TESTADDR,
                                        ( uint8_t * ) &ucWriteVal,
                                        false );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );

    /* Should return IOT_SDIO_INVALID_VALUE on NULL write buffer */
    lRetVal = iot_sdio_io_write_direct( xSdioHandle,
                                        eSdioFunction0,
                                        testIotSdio_CMD52TESTADDR,
                                        NULL,
                                        false );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );

    /* Closing a valid handle should return success. */
    lRetVal = iot_sdio_close( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test iot_sdio_io_read_extended()
 * and iot_sdio_io_write_extended().
 *
 */
TEST( TEST_IOT_SDIO, AFQP_IotSdioWriteReadExtended )
{
    IotSdioSlotHandle_t xSdioHandle = NULL;
    int32_t lRetVal;
    uint8_t i;
    bool bCardPresent = false;
    uint8_t extend_read_buf[ testIotSdio_EXTENDED_READ_TEST_SIZE ] = { 0 };          /*all zeros */
    uint8_t extend_block_read_buf[ testIotSdio_EXTENDED_READ_TEST_SIZE ] = { 0xff }; /*all 0xffs */
    uint8_t ucCISOffset[ testIotSdio_OFFSET_LEN ] = { 0 };
    uint32_t lCISOffset = 0;
    uint8_t ucBlkSzSave[ testIotSdio_BLKSIZ_LEN ] = { 0 };
    uint8_t ucWriteData[ testIotSdio_BLKSIZ_LEN ] = { 0x5a, 0x5a };
    uint8_t ucReadData[ testIotSdio_BLKSIZ_LEN ] = { 0 };
    IotSdioFuncBlkSize_t xFuncBlkSz = { eSdioFunction0, 0 };
    IotSdioFunction_t efunc = eSdioFunction0;
    IotSdioPerFuncEnable_t xPerFuncEnable = { eSdioFunction0, true };
    bool bEnableAsync = false;
    IotSdioReadCis_t xReadCisParam = { eSdioFunction1, g_funcTupleList, testIotSdio_FUNC_TPL_CNT };
    IoTSdioPowerOnOffCard_t xCardPowerCtl = { false, false };
    IotSdioFunction_t eFunc = eSdioFunction0;

    /* Open with valid host index and slot index should return a non NULL handle. */
    xSdioHandle = iot_sdio_open( uctestIotSdioValidHostIdx,
                                 uctestIotSdioValidSlotIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xSdioHandle );

    lRetVal = iot_sdio_set_sdio_callback( xSdioHandle, xtestIotSdioTestCallback, NULL );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    lRetVal = iot_sdio_ioctl( xSdioHandle, eSDIOSetCardDetectParams, ( void * ) &xtestIotSdioCardDetectParam );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* some card needs to be powered off to be detected. */
    xCardPowerCtl.bPowerOn = false;
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOPowerOnOffCard,
                              ( void * ) &xCardPowerCtl );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* detect card */
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOCheckCardPresence,
                              ( void * ) &bCardPresent );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
    TEST_ASSERT_EQUAL( true, bCardPresent );

    /* Power on card. Power on is validated later by card connect. */
    xCardPowerCtl.bPowerOn = true;
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOPowerOnOffCard,
                              ( void * ) &xCardPowerCtl );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    lRetVal = iot_sdio_card_connect( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );


    xPerFuncEnable.eFunc = eSdioFunction1;
    xPerFuncEnable.bEnable = true;
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOEnableIo,
                              ( void * ) &xPerFuncEnable );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    bEnableAsync = true;
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOEnableAsyncIrqMode,
                              ( void * ) &bEnableAsync );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    efunc = eSdioFunction1;
    lRetVal = iot_sdio_ioctl( xSdioHandle, eSDIOGetCardCapabilities, ( void * ) &efunc );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOReadCis,
                              ( void * ) &xReadCisParam );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    xPerFuncEnable.eFunc = eSdioFunction1;
    xPerFuncEnable.bEnable = true;
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOEnableIOFunctionIrq,
                              ( void * ) &xPerFuncEnable );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* 1a. read offset of function 1 Card Information Structure (CIS) */
    lRetVal = iot_sdio_io_read_extended( xSdioHandle,
                                         eSdioFunction0,
                                         false,
                                         true,
                                         testIotSdio_FUNC1_CIS_PTR_ADDR,
                                         ucCISOffset,
                                         testIotSdio_OFFSET_LEN );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    lCISOffset = ucCISOffset[ 0 ] | ( ucCISOffset[ 1 ] << 8U ) | ( ucCISOffset[ 2 ] << 16U );

    /* 1b. read CIS contents using extended multi-byte read*/
    lRetVal = iot_sdio_io_read_extended( xSdioHandle,
                                         eSdioFunction0,
                                         false,
                                         true,
                                         lCISOffset,
                                         extend_read_buf,
                                         sizeof( extend_read_buf ) );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* 1c. set block size for block transfer */
    xFuncBlkSz.eFunc = eSdioFunction0;
    xFuncBlkSz.ulBlockSize = sizeof( extend_read_buf );
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOSetFuncBlockSize,
                              ( void * ) &xFuncBlkSz );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* 1d. read CIS contents using extended block read*/
    lRetVal = iot_sdio_io_read_extended( xSdioHandle,
                                         eSdioFunction0,
                                         true,
                                         true,
                                         lCISOffset,
                                         extend_block_read_buf,
                                         1U );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* 1e. data from both version of extended reads shoud match */
    for( i = 0; i < sizeof( extend_read_buf ); i++ )
    {
        TEST_ASSERT_EQUAL( extend_read_buf[ i ], extend_block_read_buf[ i ] );
    }

    /* 2a. read and save original data from a target write area */
    lRetVal = iot_sdio_io_read_extended( xSdioHandle,
                                         eSdioFunction0,
                                         false,
                                         true,
                                         testIotSdio_FUNC1_BLK_SIZ_ADDR,
                                         ucBlkSzSave,
                                         testIotSdio_BLKSIZ_LEN );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* 2b. write test data using extended multi-byte write */
    lRetVal = iot_sdio_io_write_extended( xSdioHandle,
                                          eSdioFunction0,
                                          false,
                                          true,
                                          testIotSdio_FUNC1_BLK_SIZ_ADDR,
                                          ucWriteData,
                                          testIotSdio_BLKSIZ_LEN );
    /* 2c. read back test data using extended multi-byte read */
    lRetVal = iot_sdio_io_read_extended( xSdioHandle,
                                         eSdioFunction0,
                                         false,
                                         true,
                                         testIotSdio_FUNC1_BLK_SIZ_ADDR,
                                         ucReadData,
                                         testIotSdio_BLKSIZ_LEN );

    /* 2d. campare read data with written data*/
    for( i = 0; i < testIotSdio_BLKSIZ_LEN; i++ )
    {
        TEST_ASSERT_EQUAL( ucReadData[ i ], ucWriteData[ i ] );
    }

    /* 3a. set block size for block transfer */
    xFuncBlkSz.eFunc = eSdioFunction0;
    xFuncBlkSz.ulBlockSize = testIotSdio_BLKSIZ_LEN;
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOSetFuncBlockSize,
                              ( void * ) &xFuncBlkSz );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* 3b. start a infinite block transfer */
    lRetVal = iot_sdio_io_write_extended( xSdioHandle,
                                          eSdioFunction0,
                                          true,
                                          false,
                                          testIotSdio_FUNC1_BLK_SIZ_ADDR,
                                          ucBlkSzSave,
                                          0U );

    /* 3c. abort infinite block transfer */
    eFunc = eSdioFunction0;
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOAbortIo,
                              ( void * ) &eFunc );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* 3d. restore orignal data using extended block write */
    lRetVal = iot_sdio_io_write_extended( xSdioHandle,
                                          eSdioFunction0,
                                          true,
                                          true,
                                          testIotSdio_FUNC1_BLK_SIZ_ADDR,
                                          ucBlkSzSave,
                                          1U );

    /* 3e. read back restored data using extended block read */
    lRetVal = iot_sdio_io_read_extended( xSdioHandle,
                                         eSdioFunction0,
                                         true,
                                         true,
                                         testIotSdio_FUNC1_BLK_SIZ_ADDR,
                                         ucReadData,
                                         1U );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* 3f. campare read data with written data*/
    for( i = 0; i < testIotSdio_BLKSIZ_LEN; i++ )
    {
        TEST_ASSERT_EQUAL( ucReadData[ i ], ucBlkSzSave[ i ] );
    }

    lRetVal = iot_sdio_card_disconnect( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Closing a valid handle should return success. */
    lRetVal = iot_sdio_close( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
}


/**
 * @brief Test Function to test iot_sdio_io_read_extended()
 * and iot_sdio_io_write_extended() with invalid inputs.
 *
 */
TEST( TEST_IOT_SDIO, AFQP_IotSdioWriteReadExtendedFuzzing )
{
    IotSdioSlotHandle_t xSdioHandle = NULL;
    int32_t lRetVal;
    uint8_t extend_read_buf[ testIotSdio_EXTENDED_READ_TEST_SIZE ] = { 0 }; /*all zeros */
    uint8_t ucWriteData[ testIotSdio_BLKSIZ_LEN ] = { 0x5a, 0x5a };

    /* Open with valid host index and slot index should return a non NULL handle. */
    xSdioHandle = iot_sdio_open( uctestIotSdioValidHostIdx,
                                 uctestIotSdioValidSlotIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xSdioHandle );

    /* Should return IOT_SDIO_INVALID_VALUE on NULL handle */
    lRetVal = iot_sdio_io_read_extended( NULL,
                                         eSdioFunction0,
                                         false,
                                         true,
                                         testIotSdio_FUNC1_CIS_PTR_ADDR,
                                         extend_read_buf,
                                         testIotSdio_EXTENDED_READ_TEST_SIZE );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );

    /* Should return IOT_SDIO_INVALID_VALUE on invalid i/o function number */
    lRetVal = iot_sdio_io_read_extended( xSdioHandle,
                                         -1,
                                         false,
                                         true,
                                         testIotSdio_FUNC1_CIS_PTR_ADDR,
                                         extend_read_buf,
                                         testIotSdio_EXTENDED_READ_TEST_SIZE );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );

    /* Should return IOT_SDIO_INVALID_VALUE on NULL read buffer */
    lRetVal = iot_sdio_io_read_extended( xSdioHandle,
                                         eSdioFunction0,
                                         false,
                                         true,
                                         testIotSdio_FUNC1_CIS_PTR_ADDR,
                                         NULL,
                                         testIotSdio_EXTENDED_READ_TEST_SIZE );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );

    /* Should return IOT_SDIO_INVALID_VALUE on invalid data length */
    lRetVal = iot_sdio_io_read_extended( xSdioHandle,
                                         eSdioFunction0,
                                         false,
                                         true,
                                         testIotSdio_FUNC1_CIS_PTR_ADDR,
                                         extend_read_buf,
                                         0xffff );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );

    /* Should return IOT_SDIO_INVALID_VALUE on NULL handle */
    lRetVal = iot_sdio_io_write_extended( NULL,
                                          eSdioFunction0,
                                          false,
                                          true,
                                          testIotSdio_FUNC1_BLK_SIZ_ADDR,
                                          ucWriteData,
                                          testIotSdio_BLKSIZ_LEN );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );

    /* Should return IOT_SDIO_INVALID_VALUE on invalid i/o function number */
    lRetVal = iot_sdio_io_write_extended( xSdioHandle,
                                          -1,
                                          false,
                                          true,
                                          testIotSdio_FUNC1_BLK_SIZ_ADDR,
                                          ucWriteData,
                                          testIotSdio_BLKSIZ_LEN );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );

    /* Should return IOT_SDIO_INVALID_VALUE on NULL write buffer */
    lRetVal = iot_sdio_io_write_extended( xSdioHandle,
                                          eSdioFunction0,
                                          false,
                                          true,
                                          testIotSdio_FUNC1_BLK_SIZ_ADDR,
                                          NULL,
                                          testIotSdio_BLKSIZ_LEN );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );

    /* Should return IOT_SDIO_INVALID_VALUE on invalid data length */
    lRetVal = iot_sdio_io_write_extended( xSdioHandle,
                                          eSdioFunction0,
                                          false,
                                          true,
                                          testIotSdio_FUNC1_BLK_SIZ_ADDR,
                                          ucWriteData,
                                          0xffff );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );

    /* Closing a valid handle should return success. */
    lRetVal = iot_sdio_close( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
}


/**
 * @brief Test Function to test iot_sdio_ioctl().
 *
 */
TEST( TEST_IOT_SDIO, AFQP_IotSdioIoctl )
{
    IotSdioSlotHandle_t xSdioHandle = NULL;
    int32_t lRetVal;
    uint8_t ucReadVal = 0;
    bool bCardPresent = false;
    IotSdioPerFuncEnable_t xPerFuncEnable = { eSdioFunction0, true };
    IoTSdioPowerOnOffCard_t xCardPowerCtl = { false, false };
    IotSdioFunction_t eFunc = eSdioFunction0;

    /* Open with valid host index and slot index should return a non NULL handle. */
    xSdioHandle = iot_sdio_open( uctestIotSdioValidHostIdx,
                                 uctestIotSdioValidSlotIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xSdioHandle );

    lRetVal = iot_sdio_set_sdio_callback( xSdioHandle, xtestIotSdioTestCallback, NULL );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    lRetVal = iot_sdio_ioctl( xSdioHandle, eSDIOSetCardDetectParams, ( void * ) &xtestIotSdioCardDetectParam );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* some card needs to be powered off to be detected. */
    xCardPowerCtl.bPowerOn = false;
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOPowerOnOffCard,
                              ( void * ) &xCardPowerCtl );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* detect card */
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOCheckCardPresence,
                              ( void * ) &bCardPresent );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
    TEST_ASSERT_EQUAL( true, bCardPresent );

    /* Power on card. Power on is validated later by card connect. */
    xCardPowerCtl.bPowerOn = true;
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOPowerOnOffCard,
                              ( void * ) &xCardPowerCtl );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    lRetVal = iot_sdio_card_connect( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Switch to high speed */
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOSwitchToHighSpeed,
                              NULL );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Verify that EHS bit in Bus Speed Register is set */
    lRetVal = iot_sdio_io_read_direct( xSdioHandle,
                                       eSdioFunction0,
                                       testIotSdio_EHS_ADDR,
                                       ( uint8_t * ) &ucReadVal );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    if( testIotSdio_SHS_MASK & ucReadVal )
    {
        /* EHS bit must be set if card supports high speed */
        TEST_ASSERT_EQUAL( testIotSdio_EHS_MASK, testIotSdio_EHS_MASK & ucReadVal );
    }
    else
    {
        /* EHS bit must not be set if card does not supports high speed */
        TEST_ASSERT_NOT_EQUAL( testIotSdio_EHS_MASK, testIotSdio_EHS_MASK & ucReadVal );
    }

    /* Set driver strength */
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOSetDriverStrength,
                              ( void * ) &etestIotSdioDriverType );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Verify driver strength */
    lRetVal = iot_sdio_io_read_direct( xSdioHandle,
                                       eSdioFunction0,
                                       testIotSdio_DTS_ADDR,
                                       ( uint8_t * ) &ucReadVal );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    if( ( etestIotSdioDriverType != eSdDriverStrengthTypeB ) &&
        ( ( testIotSdio_DTS_TYPE_MASK << ( etestIotSdioDriverType - 1 ) ) & ucReadVal ) )
    {
        /* Requested driver strength type must be set if it is not type B, and is supported by card */
        TEST_ASSERT_EQUAL( etestIotSdioDriverType, ( testIotSdio_DTS_MASK & ucReadVal ) >> testIotSdio_DTS_BITS );
    }
    else
    {
        /* Driver strength type B (default) must be set if the requested driver strenghth type was
         * type B or a type that was not supported by card */
        TEST_ASSERT_EQUAL( eSdDriverStrengthTypeB, ( testIotSdio_DTS_MASK & ucReadVal ) >> testIotSdio_DTS_BITS );
    }

    /* Set data bus width */
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOSetDataBusWidth,
                              ( void * ) &etestIotSdioBusWidth );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Verify data bus width */
    lRetVal = iot_sdio_io_read_direct( xSdioHandle,
                                       eSdioFunction0,
                                       testIotSdio_BUS_CTL_ADDR,
                                       ( uint8_t * ) &ucReadVal );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
    TEST_ASSERT_EQUAL( etestIotSdioBusWidth, testIotSdio_BUS_WIDTH_MASK & ucReadVal );

    /* Select function for suspend/resume */
    eFunc = eSdioFunction1;
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOSelectIo,
                              ( void * ) &eFunc );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Verify select function */
    lRetVal = iot_sdio_io_read_direct( xSdioHandle,
                                       eSdioFunction0,
                                       testIotSdio_FUNC_SEL_ADDR,
                                       ( uint8_t * ) &ucReadVal );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
    TEST_ASSERT_EQUAL( eFunc, testIotSdio_FUNC_SEL_MASK & ucReadVal );

    /* Io function enable */
    xPerFuncEnable.eFunc = eSdioFunction1;
    xPerFuncEnable.bEnable = true;
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOEnableIo,
                              ( void * ) &xPerFuncEnable );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Verify io function enable */
    lRetVal = iot_sdio_io_read_direct( xSdioHandle,
                                       eSdioFunction0,
                                       testIotSdio_IO_EN_ADDR,
                                       ( uint8_t * ) &ucReadVal );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
    TEST_ASSERT_EQUAL( testIotSdio_IO1_EN_MASK, testIotSdio_IO1_EN_MASK & ucReadVal );

    /* Io function disable */
    xPerFuncEnable.eFunc = eSdioFunction1;
    xPerFuncEnable.bEnable = false;
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOEnableIo,
                              ( void * ) &xPerFuncEnable );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Verify io function disable */
    lRetVal = iot_sdio_io_read_direct( xSdioHandle,
                                       eSdioFunction0,
                                       testIotSdio_IO_EN_ADDR,
                                       ( uint8_t * ) &ucReadVal );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
    TEST_ASSERT_EQUAL( 0, testIotSdio_IO1_EN_MASK & ucReadVal );


    /* Set card inactive */
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOSetCardInactive,
                              NULL );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    if( btestIotSdioVerifyInactive )
    {
        /* Verify card is in inactive state */
        lRetVal = iot_sdio_io_read_direct( xSdioHandle,
                                           eSdioFunction0,
                                           testIotSdio_CMD52TESTADDR,
                                           ( uint8_t * ) &ucReadVal );
        TEST_ASSERT_NOT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
    }

    lRetVal = iot_sdio_card_disconnect( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );

    /* Closing a valid handle should return success. */
    lRetVal = iot_sdio_close( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
}

/**
 * @brief Test Function to test iot_sdio_ioctl() with invalid inputs.
 *
 */
TEST( TEST_IOT_SDIO, AFQP_IotSdioIoctlFuzzing )
{
    IotSdioSlotHandle_t xSdioHandle = NULL;
    int32_t lRetVal;
    uint8_t ucReadVal = 0;
    bool bCardPresent = false;
    IotSdioPerFuncEnable_t xPerFuncEnable = { eSdioFunction0, true };
    IoTSdioPowerOnOffCard_t xCardPowerCtl = { false, false };
    IotSdioFunction_t eFunc = eSdioFunction0;

    /* Open with valid host index and slot index should return a non NULL handle. */
    xSdioHandle = iot_sdio_open( uctestIotSdioValidHostIdx,
                                 uctestIotSdioValidSlotIdx );
    TEST_ASSERT_NOT_EQUAL( NULL, xSdioHandle );

    /* Should return IOT_SDIO_INVALID_VALUE on NULL handle */
    lRetVal = iot_sdio_ioctl( NULL,
                              eSDIOSetDriverStrength,
                              ( void * ) &etestIotSdioDriverType );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );

    /* Should return IOT_SDIO_INVALID_VALUE on invlaid ioctl request enum number */
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              -1,
                              ( void * ) &etestIotSdioDriverType );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );

    /* Should return IOT_SDIO_INVALID_VALUE on NULL input buffer */
    lRetVal = iot_sdio_ioctl( xSdioHandle,
                              eSDIOSetDriverStrength,
                              NULL );
    TEST_ASSERT_EQUAL( IOT_SDIO_INVALID_VALUE, lRetVal );

    /* Closing a valid handle should return success. */
    lRetVal = iot_sdio_close( xSdioHandle );
    TEST_ASSERT_EQUAL( IOT_SDIO_SUCCESS, lRetVal );
}
