//
// Created by parallels on 1/30/20.
//

#include <stdbool.h>
#include <pthread.h>
#include <unity.h>


#include "iot_ble_data_transfer.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "types/iot_platform_types.h"
#include "iot_ble_config.h"

#include "mock_iot_ble.h"
#include "mock_iot_threads.h"
#include "mock_iot_logging.h"
#include "../../../../abstractions/ble_hal/include/bt_hal_manager_types.h"


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void initCallbacks();


/*******************************************************************************
 * Global Variables
 ******************************************************************************/

static uint16_t malloc_free_calls = 0;

/*******************************************************************************
 * Unity Callbacks
 ******************************************************************************/
BTStatus_t
mock_IotBle_RegisterEventCb(IotBleEvents_t event,
                            IotBleEventsCallbacks_t bleEventsCallbacks,
                            int n_calls)
{
    return eBTStatusSuccess;
}


bool
mock_IotSemaphore_Create( IotSemaphore_t * pNewSemaphore,
                          uint32_t initialValue,
                          uint32_t maxValue,
                          int n_calls)
{
    return true;
}


void
mock_IotSemaphore_Post( IotSemaphore_t * pSemaphore,
                        int n_calls)
{

}

/* ============================   UNITY FIXTURES ============================ */
void setUp( void )
{
    // There's several functions which will be called frequently, and need stubs
    IotSemaphore_Create_Stub( mock_IotSemaphore_Create );
    IotSemaphore_Post_Stub( mock_IotSemaphore_Post );

    IotLog_Generic_Ignore();
}

/* called before each testcase */
void tearDown( void )
{
   IotLog_Generic_Ignore();
}

/* called at the beginning of the whole suite */
void suiteSetUp()
{
}

/* called at the end of the whole suite */
int suiteTearDown( int numFailures )
{
    return( numFailures > 0 );
}


/*******************************************************************************
 * IotBleDataTransfer_Init
 ******************************************************************************/

/*
 * Combos of RegisterCallback failing/passing
 */
void
test_00_IotBleDataTransfer_Init( void )
{
    bool ret = false;
    IotBle_CreateService_IgnoreAndReturn(eBTStatusSuccess);

    // Happy path
    IotBle_RegisterEventCb_ExpectAnyArgsAndReturn(eBTStatusSuccess);
    IotBle_RegisterEventCb_ExpectAnyArgsAndReturn(eBTStatusSuccess);
    ret = IotBleDataTransfer_Init();
    TEST_ASSERT_TRUE_MESSAGE(ret, "Failed to initialize data");


    // Check different combos of subroutine results
    IotBle_RegisterEventCb_ExpectAnyArgsAndReturn(eBTStatusFail);
    ret = IotBleDataTransfer_Init();
    TEST_ASSERT_FALSE(ret);

    IotBle_RegisterEventCb_ExpectAnyArgsAndReturn(eBTStatusSuccess);
    IotBle_RegisterEventCb_ExpectAnyArgsAndReturn(eBTStatusFail);
    ret = IotBleDataTransfer_Init();
    TEST_ASSERT_FALSE(ret);
}

/*
 * Failure at service creation
 */
void
test_01_IotBleDataTransfer_Init( void )
{
    bool ret = false;
    IotBle_CreateService_IgnoreAndReturn(eBTStatusFail);

    IotBle_RegisterEventCb_ExpectAnyArgsAndReturn(eBTStatusSuccess);
    IotBle_RegisterEventCb_ExpectAnyArgsAndReturn(eBTStatusSuccess);
    ret = IotBleDataTransfer_Init();
    TEST_ASSERT_FALSE(ret);
}


/*
void
test_IotBleDataTransfer_Send_NULL_Args()
{
    int32_t ret = 0;
    const size_t rx_buf_size = 32;
    uint8_t rx_buf[rx_buf_size] = { 0 };
    IotBleDataChannelBuffer_t rx_channel_buf = {
            .pBuffer = &rx_buf,
            .head    = 0,
            .tail    = 0,
            .bufferLength = rx_buf_size
    };

    const size_t tx_buf_size = 32;
    uint8_t tx_buf[tx_buf_size] = { 0 };

    const size_t lot_buf_size = 256;
    uint8_t lot_buf[lot_buf_size] = { 0 };

    IotBleDataTransferChannel_t channel = {
        .lotBuffer = {
            .pBuffer = &lot_buf,
            .head = 0,
            .tail = 0,
            .bufferLength = lot_buf_size
        },
        .pReceiveBuffer = &rx_channel_buf,
        .sendBuffer = {
            .pBuffer = &tx_buf,
            .head    = 0,
            .tail    = 0,
            .bufferLength = tx_buf_size
        },

    };

    size_t tx_buf_size = 256;
    uint8_t tx_buf[tx_buf_size] = { 0 };
    IotBleDataTransferChannel_t channel = {
            .lotBuff
    };

    ret = IotBleDataTransfer_Send()
}
 */
