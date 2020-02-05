//
// Created by parallels on 1/30/20.
//

#include <stdbool.h>
#include <pthread.h>
#include <unity.h>
#include <bt_hal_manager_types.h>


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
static uint32_t n_dummy_callback_calls = 0;




/*******************************************************************************
 * Internal helpers
 ******************************************************************************/
typedef enum IotBleDataTransferAttributes
{
    IOT_BLE_DATA_TRANSFER_SERVICE = 0,
    IOT_BLE_DATA_TRANSFER_CONTROL_CHAR,
    IOT_BLE_DATA_TRANSFER_TX_CHAR,             /*!< IOT_BLE_DATA_TRANSFER_TX_CHAR Characteristic to send message. */
    IOT_BLE_DATA_TRANSFER_TX_CHAR_DESCR,       /*!< IOT_BLE_DATA_TRANSFER_TX_CHAR_DESCR Characteristic descriptor. */
    IOT_BLE_DATA_TRANSFER_RX_CHAR,             /*!< IOT_BLE_DATA_TRANSFER_RX_CHAR Characteristic to receive a message.*/
    IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR,       /*!< IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR Characteristic to send a large message (> BLE MTU Size). */
    IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR_DESCR, /*!< IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR_DESCR Characteristic descriptor. */
    IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR,       /*!< IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR Characteristic to receive large message (> BLE MTU Size). */
    IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES       /*!< IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES Max number of attributes for ble data transfer service. */
} IotBleDataTransferAttributes_t;


/*
 * Don't have access to internal callbacks hooked with this call, but we can intercept them and track thereafter
 */
typedef struct {
    BTService_t * gatt_service;
    IotBleAttributeEventCallback_t * attr_callbacks;
    IotBleDataTransferChannelCallback_t channel_callback;
    bool enabled;
} LOTServiceVariantTracker_t;

typedef struct {
    LOTServiceVariantTracker_t wifi_provisioning;
    LOTServiceVariantTracker_t mqtt;
} LOTServiceTracker_t;

static LOTServiceTracker_t lot_service = {
        .wifi_provisioning = {NULL, NULL, false},
        .mqtt              = {NULL, NULL, false}
};

static bool
cleanup_transfers()
{
    bool ret = false;

    IotBle_UnRegisterEventCb_ExpectAnyArgsAndReturn(eBTStatusSuccess);
    IotBle_UnRegisterEventCb_ExpectAnyArgsAndReturn(eBTStatusSuccess);
    IotBle_DeleteService_ExpectAnyArgsAndReturn(eBTStatusSuccess);
    ret = IotBleDataTransfer_Cleanup();
    TEST_ASSERT_MESSAGE(ret, "Failed to cleanup and reinitialize before testing channel operation");
}

/*
 * Individual tests need a way to restore to a known state so they don't stomp on each other
 */
static bool
init_transfers()
{
    bool ret = false;

    cleanup_transfers();

    IotBle_RegisterEventCb_ExpectAnyArgsAndReturn(eBTStatusSuccess);
    IotBle_RegisterEventCb_ExpectAnyArgsAndReturn(eBTStatusSuccess);
    ret = IotBleDataTransfer_Init();
    TEST_ASSERT_TRUE_MESSAGE(ret, "Failed to initialize data");
    return ret;
}


/*
 * Simulates event that receiving side has sent a write for Control Attribute=1 of the LOT service,
 * indicating that they are ready to receive data i.e. IotBle_DataTransfer_Send can succeed in sending bytes
 */

void *
generate_client_ready_event(uint16_t service) {
    LOTServiceVariantTracker_t *service_variant = NULL;
    if (service == IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_WIFI_PROVISIONING) {
        service_variant = &lot_service.wifi_provisioning;
    } else if (service == IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT) {
        service_variant = &lot_service.mqtt;
    } else {
        TEST_ASSERT_MESSAGE(false, "Attempt to generate LOT event for unknown variant. "
                                   "Variants include: mqtt, wifi-provisioning");
    }

    uint8_t client_ready = true;
    IotBleWriteEventParams_t params = {
            .transId = 0,
            .pRemoteBdAddr = 0,
            .pValue = &client_ready,
            .length = 1,
            .connId = 0, // A fake connection with mocks. This doesn't matter
            .attrHandle = IOT_BLE_DATA_TRANSFER_CONTROL_CHAR,
            .offset = 0,
            .needRsp = false,
            .isPrep = 0

    };

    IotBleAttributeEvent_t event = {
            .pParamWrite = &params,
            .xEventType = eBLEWrite
    };

    IotBleAttributeEventCallback_t callback = service_variant->attr_callbacks[IOT_BLE_DATA_TRANSFER_CONTROL_CHAR];
    callback(&event);
}


/*
 * Get a channel. For some tests it doesn't really matter which one it is
 */
static IotBleDataTransferChannel_t *
get_open_channel(uint16_t service_variant)
{
    IotBleDataTransferChannel_t *channel = 0;

    // Client must be ready for data before byte can get accross
    generate_client_ready_event(service_variant);
    channel = IotBleDataTransfer_Open(service_variant);

    TEST_ASSERT_MESSAGE(channel != NULL, "Failed to attain valid channel. Testing requires a some LOT"
                                         "variant enabled. Variants:\n"
                                         "    IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT\n"
                                         "    IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_WIFI_PROVISIONINGT");
}


/*
 * Channel callback used for LOT Service tracker. Shared by both variants
 */

void
channel_callback(IotBleDataTransferChannelEvent_t event,
                 IotBleDataTransferChannel_t * pChannel,
                 void * pContext)
{
    n_dummy_callback_calls++;
}
/*******************************************************************************
 * Unity Callbacks
 ******************************************************************************/
BTStatus_t
IotBle_RegisterEventCb_Callback(IotBleEvents_t event,
                               IotBleEventsCallbacks_t bleEventsCallbacks,
                               int n_calls)
{
    return eBTStatusSuccess;
}


bool
IotSemaphore_Create_Callback(IotSemaphore_t * pNewSemaphore,
                            uint32_t initialValue,
                            uint32_t maxValue,
                            int n_calls)
{
    return true;
}


void
IotSemaphore_Post_Callback(IotSemaphore_t * pSemaphore,
                          int n_calls)
{

}

void
IotSemaphore_Destroy_Callback(IotSemaphore_t * pSemaphore,
                             int n_calls)
{

}

/*
 * Tracking the created service is handy for injecting calls to its event handlers, as though an event occurred
 */
BTStatus_t
IotBle_CreateService_Callback(BTService_t * pService,
                              IotBleAttributeEventCallback_t pEventsCallbacks[],
                              int n_calls)
{
    TEST_ASSERT_MESSAGE(pService && pService->pxBLEAttributes, "Attempt to track invalid service");
    uint8_t service_id = pService->pxBLEAttributes[0].xServiceUUID.uu.uu128[1];
    LOTServiceVariantTracker_t *service_variant = NULL;
    if (service_id == IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_WIFI_PROVISIONING) {
        service_variant = &lot_service.wifi_provisioning;
    } else if (service_id == IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT) {
        service_variant = &lot_service.mqtt;
    } else {
        TEST_ASSERT_MESSAGE(false, "Attempt to generate LOT event for unknown variant. "
                                   "Variants include: mqtt, wifi-provisioning");
    }

    service_variant->gatt_service = pService;
    service_variant->attr_callbacks = pEventsCallbacks;
    service_variant->enabled = true;

    // Recreate values that would normally be set via port bluetooth code
    for (IotBleDataTransferAttributes_t attr = IOT_BLE_DATA_TRANSFER_SERVICE;
            attr < IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES;
            attr++) {
        pService->pusHandlesBuffer[attr] = attr;
    }

    return eBTStatusSuccess;
}

/*
 * Can track responses here
 */
BTStatus_t
IotBle_SendResponse_Callback(IotBleEventResponse_t *resp,
                             uint16_t connId,
                             uint32_t transId,
                             int n_calls)
{
    return eBTStatusSuccess;
}


BTStatus_t
IotBle_SendIndication_Callback(IotBleEventResponse_t * pResp,
                               uint16_t conndId,
                               bool confirm,
                               int n_calls)
{
    return eBTStatusSuccess;
}

bool
IotSemaphore_TimedWait_Callback(IotSemaphore_t * pSem,
                                uint32_t timeoutMs,
                                int n_calls)
{
    return true;
}
/*******************************************************************************
 * Unity fixtures
 ******************************************************************************/
void setUp( void )
{
    // There's several functions which will be called frequently, and need stubs
    IotSemaphore_Create_Stub( IotSemaphore_Create_Callback );
    IotSemaphore_Post_Stub( IotSemaphore_Post_Callback );
    IotSemaphore_Destroy_Stub( IotSemaphore_Destroy_Callback );
    IotSemaphore_TimedWait_Stub( IotSemaphore_TimedWait_Callback );

    IotBle_CreateService_Stub( IotBle_CreateService_Callback );

    IotBle_SendResponse_Stub( IotBle_SendResponse_Callback );
    IotBle_SendIndication_Stub ( IotBle_SendIndication_Callback );


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
    n_dummy_callback_calls = 0;
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
/* TODO: Will need to cause fail mode here, despite stub always returning Success
void
test_01_IotBleDataTransfer_Init( void )
{
    bool ret = false;

    IotBle_RegisterEventCb_ExpectAnyArgsAndReturn(eBTStatusSuccess);
    IotBle_RegisterEventCb_ExpectAnyArgsAndReturn(eBTStatusSuccess);
    IotBle_RegisterEventCb_ExpectAnyArgsAndReturn(eBTStatusFail);
    ret = IotBleDataTransfer_Init();
    TEST_ASSERT_FALSE(ret);
}
*/


/*******************************************************************************
 * IotBleDataTransfer_Open
 ******************************************************************************/
/*
 * Assume that requested service has ready/available channels, but not whether the services exist
 */
void
test_00_IotBleDataTransfer_Open( void )
{
    init_transfers();

    IotBleDataTransferChannel_t *channel = 0;
    #if ( IOT_BLE_ENABLE_DATA_TRANSFER_SERVICE == 1 )
        #if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 1 )
            channel = IotBleDataTransfer_Open(IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_WIFI_PROVISIONING);
            TEST_ASSERT_MESSAGE(channel, "Attempting to open uninitialized wifi-provisioning service");
        #endif
        #if ( IOT_BLE_ENABLE_MQTT == 1 )
            channel = IotBleDataTransfer_Open(IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT);
            TEST_ASSERT_MESSAGE(channel, "Attempting to open uninitialized wifi-provisioning service");
        #endif
    #endif
}


/*******************************************************************************
 * IotBleDataTransfer_SetCallback
 ******************************************************************************/
/*
 * Happy path with ready channel. Don't care for context here
 */
void
test_00_IotBleDataTransfer_SetCallback( void )
{
    bool ret = false;

    init_transfers();
    IotBleDataTransferChannel_t *pChannel = get_open_channel(IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT);
    ret = IotBleDataTransfer_SetCallback(pChannel,
                                   channel_callback,
                                   NULL);
    TEST_ASSERT(ret);
}
/*******************************************************************************
 * IotBleDataTransfer_Send
 ******************************************************************************/
/*
 * Happy path
 */
void
test_00_IotBleDataTransfer_Send( void )
{
    size_t n_sent = 0;
    const uint16_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    init_transfers();
    IotBleDataTransferChannel_t *pChannel = get_open_channel(service_variant);


    char msg[] = "I am test data";
    n_sent = IotBleDataTransfer_Send(pChannel,
                                     msg,
                                     sizeof(msg));
    printf("%d/%d\n", n_sent, sizeof(msg));
    TEST_ASSERT(n_sent > 0);
}
/*******************************************************************************
 * IotBleDataTransfer_Receive
 ******************************************************************************/
/*
 * Happy path
 */



/*******************************************************************************
 * IotBleDataTransfer_PeekReceiveBuffer
 ******************************************************************************/
/*
 * Happy path
 */

/*******************************************************************************
 * IotBleDataTransfer_Close
 ******************************************************************************/
/*
 * Happy path
 */

/*******************************************************************************
 * IotBleDataTransfer_Reset
 ******************************************************************************/
/*
 * Happy path
 */

/*******************************************************************************
 * IotBleDataTransfer_Cleanup
 ******************************************************************************/
/*
 * Happy path
 */
void
test_00_IotBleDataTransfer_Cleanup( void )
{
    bool ret = false;

    IotBle_UnRegisterEventCb_ExpectAnyArgsAndReturn(eBTStatusSuccess);
    IotBle_UnRegisterEventCb_ExpectAnyArgsAndReturn(eBTStatusSuccess);
    IotBle_DeleteService_ExpectAnyArgsAndReturn(eBTStatusSuccess);
    ret = IotBleDataTransfer_Cleanup();

    TEST_ASSERT(ret);
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
