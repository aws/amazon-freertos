/* */
/* Created by parallels on 1/30/20. */
/* */

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
#include "mock_portable.h"

/* ******TODO TODO TODO: Ideally, every test case has something to assert on */

#ifndef IOT_BLE_PREFERRED_MTU_SIZE
    #define IOT_BLE_PREFERRED_MTU_SIZE    ( 512 )
#endif

#define MAX_XMIT_DATA_SIZE                ( IOT_BLE_PREFERRED_MTU_SIZE - 3 )

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void initCallbacks();



/*******************************************************************************
 * Global Variables
 ******************************************************************************/

static int32_t malloc_free_calls = 0;
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
typedef struct
{
    BTService_t * gatt_service;
    IotBleAttributeEventCallback_t * attr_callbacks;
    IotBleDataTransferChannelCallback_t channel_callback;
    bool enabled;
} LOTServiceVariantTracker_t;


typedef struct
{
    LOTServiceVariantTracker_t wifi_provisioning;
    LOTServiceVariantTracker_t mqtt;
    bool connected;
    IotBle_ConnectionCallback_t connection_callback;
    size_t mtu;
    IotBle_MtuChangedCallback_t mtu_changed_callback;
} LOTServiceTracker_t;

#define SERVICE_VARIANT_INITIALIZER  \
    ( LOTServiceTracker_t )          \
    {                                \
        { NULL, NULL, NULL, false }, \
        { NULL, NULL, NULL, false }, \
        false,                       \
        NULL,                        \
        IOT_BLE_PREFERRED_MTU_SIZE,  \
        NULL                         \
    }

static LOTServiceTracker_t lot_service = SERVICE_VARIANT_INITIALIZER;

bool cleanup_transfers()
{
    bool ret = false;

    ret = IotBleDataTransfer_Cleanup();
    TEST_ASSERT_MESSAGE( ret, "Failed to cleanup and reinitialize before testing channel operation" );

    return ret;
}

/*
 * Individual tests need a way to restore to a known state so they don't stomp on each other
 */
bool init_transfers()
{
    bool ret = false;

    cleanup_transfers();
    ret = IotBleDataTransfer_Init();
    TEST_ASSERT_TRUE_MESSAGE( ret, "Failed to initialize data" );
    return ret;
}

void init_mtu()
{
    lot_service.mtu = IOT_BLE_PREFERRED_MTU_SIZE;
}

LOTServiceVariantTracker_t * get_service_tracker_from_short_id( uint8_t service )
{
    TEST_ASSERT_MESSAGE( service == IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_WIFI_PROVISIONING ||
                         service == IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT,
                         "Attempt to generate LOT event for unknown variant. "
                         "Variants include: mqtt, wifi-provisioning" );

    return service == IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT ? &lot_service.mqtt : &lot_service.wifi_provisioning;
}

LOTServiceVariantTracker_t * get_service_tracker( BTService_t * pService )
{
    TEST_ASSERT_MESSAGE( pService && pService->pxBLEAttributes, "Null input arguments" );
    uint8_t service_id = pService->pxBLEAttributes[ 0 ].xServiceUUID.uu.uu128[ 1 ];

    return get_service_tracker_from_short_id( service_id );
}

size_t get_mtu()
{
    return lot_service.mtu;
}

size_t get_max_data_len()
{
    return get_mtu() - 3;
}

bool is_connected()
{
    return lot_service.connected;
}


void generate_client_write_event( uint8_t service_id,
                                         IotBleDataTransferAttributes_t attr,
                                         uint8_t * msg,
                                         uint32_t len,
                                         bool response_required)
{
    LOTServiceVariantTracker_t * service_variant = get_service_tracker_from_short_id( service_id );

    IotBleWriteEventParams_t params =
    {
        .transId       = 0,
        .pRemoteBdAddr = 0,
        .pValue        = msg,
        .length        = len,
        .connId        = 0,
        .attrHandle    = attr,
        .offset        = 0,
        .needRsp       = response_required,
        .isPrep        = 0
    };

    IotBleAttributeEvent_t event =
    {
        .pParamWrite = &params,
        .xEventType  = response_required ? eBLEWrite : eBLEWriteNoResponse
    };

    IotBleAttributeEventCallback_t callback = service_variant->attr_callbacks[ attr ];

    callback( &event );
}

void generate_client_read_event( uint8_t service_id,
                                        IotBleDataTransferAttributes_t attr )
{
    LOTServiceVariantTracker_t * service_variant = get_service_tracker_from_short_id( service_id );

    IotBleReadEventParams_t params =
    {
        .connId        = 0,
        .transId       = 0,
        .pRemoteBdAddr = 0,
        .attrHandle    = attr,
        .offset        = 0
    };

    IotBleAttributeEvent_t event =
    {
        .pParamRead = &params,
        .xEventType = eBLERead
    };

    IotBleAttributeEventCallback_t callback = service_variant->attr_callbacks[ attr ];

    callback( &event );
}


/*
 * Simulates event that receiving side has sent a write for Control Attribute=1 of the LOT service,
 * indicating that they are ready to receive data i.e. IotBle_DataTransfer_Send can succeed in sending bytes
 */
void generate_client_ready_event( uint16_t service )
{
    uint8_t client_ready = true;

    generate_client_write_event( service, IOT_BLE_DATA_TRANSFER_CONTROL_CHAR, &client_ready, 1 , false);
}

void generate_connect_event(BTStatus_t status)
{
    TEST_ASSERT_MESSAGE( lot_service.connection_callback, "Event can't be tied to unregistered callback" )
    lot_service.connection_callback( status, 0, true, NULL );
}

void generate_disconnect_event(BTStatus_t status)
{
    TEST_ASSERT_MESSAGE( lot_service.connection_callback, "Event can't be tied to unregistered callback" )
    lot_service.connection_callback( status, 0, false, NULL );
}

void generate_mtu_changed_event( size_t new_mtu )
{
    TEST_ASSERT_MESSAGE( lot_service.mtu_changed_callback, "Event can't be tied to unregistered callback" )
    lot_service.mtu_changed_callback( 0, new_mtu );
}

/*
 * Passes an out of range attr handle through server callback assigned for attr arg
 */
void generate_event_with_bad_handle(uint8_t service_id, IotBleDataTransferAttributes_t attr, bool is_read)
{

    LOTServiceVariantTracker_t * service_tracker = get_service_tracker_from_short_id( service_id );
    IotBleAttributeEvent_t event;
    if (is_read) {
        IotBleReadEventParams_t read_params = {
                .connId        = 0,
                .transId       = 0,
                .pRemoteBdAddr = 0,
                .attrHandle    = 0,
                .offset        = 0
        };

        event.pParamRead = &read_params;
        event.xEventType = eBLERead;
    } else {
        uint8_t dummy = 0xAA;
        IotBleWriteEventParams_t write_params = {
                .transId = 0,
                .pRemoteBdAddr = 0,
                .pValue = &dummy,
                .length = 1,
                .connId = 0,
                .attrHandle = 0,
                .offset = 0,
                .needRsp = 0,
                .isPrep = 0
        };

        event.pParamWrite = &write_params;
        event.xEventType  = eBLEWrite;
    }

    IotBleAttributeEventCallback_t callback = service_tracker->attr_callbacks[ attr ];
    callback( &event );
}

void generate_event_with_bad_type(uint8_t service_id, IotBleDataTransferAttributes_t attr)
{
    LOTServiceVariantTracker_t * service_tracker = get_service_tracker_from_short_id( service_id );
    IotBleAttributeEvent_t event;
    event.xEventType = -1;
    IotBleAttributeEventCallback_t callback = service_tracker->attr_callbacks[ attr ];
    callback( &event );
}

/*
 * Get a channel. For some tests it doesn't really matter which one it is
 */
IotBleDataTransferChannel_t * get_open_channel( uint16_t service_variant )
{
    IotBleDataTransferChannel_t * channel = 0;

    /* Client must be ready for data before byte can get accross */
    generate_client_ready_event( service_variant );
    channel = IotBleDataTransfer_Open( service_variant );

    TEST_ASSERT_MESSAGE( channel != NULL, "Failed to attain valid channel. Testing requires a some LOT"
                                          "variant enabled. Variants:\n"
                                          "    IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT\n"
                                          "    IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_WIFI_PROVISIONINGT" );

    return channel;
}


/*
 * Channel callback used for LOT Service tracker. Shared by both variants
 */

static void channel_callback( IotBleDataTransferChannelEvent_t event,
                              IotBleDataTransferChannel_t * pChannel,
                              void * pContext )
{
    n_dummy_callback_calls++;

    switch( event )
    {
        case IOT_BLE_DATA_TRANSFER_CHANNEL_OPENED:
            break;

        case IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_RECEIVED:
            break;

        case IOT_BLE_DATA_TRANSFER_CHANNEL_DATA_SENT:
            break;

        case IOT_BLE_DATA_TRANSFER_CHANNEL_CLOSED:
            break;

        default:
            break;
    }
}

/*******************************************************************************
 * Unity Callbacks
 ******************************************************************************/
bool IotSemaphore_Create_Callback( IotSemaphore_t * pNewSemaphore,
                                          uint32_t initialValue,
                                          uint32_t maxValue,
                                          int n_calls )
{
    return true;
}


void IotSemaphore_Post_Callback( IotSemaphore_t * pSemaphore,
                                        int n_calls )
{
}

void IotSemaphore_Destroy_Callback( IotSemaphore_t * pSemaphore,
                                           int n_calls )
{
}

/*
 * Tracking the created service is handy for injecting calls to its event handlers, as though an event occurred
 */
static BTStatus_t IotBle_CreateService_Callback( BTService_t * pService,
                                                 IotBleAttributeEventCallback_t pEventsCallbacks[],
                                                 int n_calls )
{
    TEST_ASSERT_MESSAGE( pService && pService->pxBLEAttributes, "Null input arguments" );
    uint8_t service_id = pService->pxBLEAttributes[ 0 ].xServiceUUID.uu.uu128[ 1 ];
    LOTServiceVariantTracker_t * service_variant = get_service_tracker_from_short_id( service_id );

    service_variant->gatt_service = pService;
    service_variant->attr_callbacks = pEventsCallbacks;
    service_variant->enabled = true;

    /* Recreate values that would normally be set via port bluetooth code */
    for( IotBleDataTransferAttributes_t attr = IOT_BLE_DATA_TRANSFER_SERVICE;
         attr < IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES;
         attr++ )
    {
        pService->pusHandlesBuffer[ attr ] = attr;
    }

    return eBTStatusSuccess;
}

static BTStatus_t IotBle_DeleteService_Callback( BTService_t * pService,
                                                 int n_calls )
{
    TEST_ASSERT_MESSAGE( pService && pService->pxBLEAttributes, "Null input arguments" );

    lot_service = SERVICE_VARIANT_INITIALIZER;

    return eBTStatusSuccess;
}



static bool IotSemaphore_TimedWait_Callback( IotSemaphore_t * pSem,
                                             uint32_t timeoutMs,
                                             int n_calls )
{
    return true;
}

static void * pvPortMalloc_Callback( size_t xSize,
                                     int n_calls )
{
    malloc_free_calls++; /* Free + malloc calls should cancel out in the end */

    void * pNew = malloc( xSize );
    TEST_ASSERT_MESSAGE( pNew, "Test Stub for malloc failed!" );

    return pNew;
}

static void vPortFree_Callback( void * pMem,
                                int n_calls )
{
    malloc_free_calls--;
    free( pMem );
}

static BTStatus_t IotBle_RegisterEventCb_Callback( IotBleEvents_t event,
                                                   IotBleEventsCallbacks_t cb,
                                                   int n_calls )
{
    switch( event )
    {
        case eBLEConnection:
            TEST_ASSERT( cb.pConnectionCb );
            lot_service.connection_callback = cb.pConnectionCb;
            break;

        case eBLEMtuChanged:
            TEST_ASSERT( cb.pMtuChangedCb );
            lot_service.mtu_changed_callback = cb.pMtuChangedCb;
            break;

        default:
            /* Don't want to assert. New event callbacks may be added in the future */
            return eBTStatusFail;
    }

    return eBTStatusSuccess;
}


static BTStatus_t IotBle_UnregisterEventCb_Callback( IotBleEvents_t event,
                                                     IotBleEventsCallbacks_t cb,
                                                     int n_calls )
{
    switch( event )
    {
        case eBLEConnection:
            TEST_ASSERT( cb.pConnectionCb );
            lot_service.connection_callback = NULL;
            break;

        case eBLEMtuChanged:
            TEST_ASSERT( cb.pMtuChangedCb );
            lot_service.mtu_changed_callback = NULL;
            break;

        default:
            /* Don't want to assert. New event callbacks may be added in the future */
            return eBTStatusFail;
    }

    return eBTStatusSuccess;
}

/*******************************************************************************
 * Unity fixtures
 ******************************************************************************/
void setUp( void )
{
    /* There's several functions which will be called frequently, and need stubs */
    IotSemaphore_Create_Stub( IotSemaphore_Create_Callback );
    IotSemaphore_Post_Stub( IotSemaphore_Post_Callback );
    IotSemaphore_Destroy_Stub( IotSemaphore_Destroy_Callback );
    IotSemaphore_TimedWait_Stub( IotSemaphore_TimedWait_Callback );

    IotBle_CreateService_Stub( IotBle_CreateService_Callback );
    IotBle_DeleteService_Stub( IotBle_DeleteService_Callback );

    pvPortMalloc_Stub( pvPortMalloc_Callback );
    vPortFree_Stub( vPortFree_Callback );

    IotBle_RegisterEventCb_Stub( IotBle_RegisterEventCb_Callback );
    IotBle_UnRegisterEventCb_Stub( IotBle_UnregisterEventCb_Callback );

    IotLog_Generic_Ignore();
}

/* called before each testcase */
void tearDown( void )
{
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
void test_00_IotBleDataTransfer_Init( void )
{
    bool ret = false;

    /* Happy path */
    ret = IotBleDataTransfer_Init();
    TEST_ASSERT_TRUE_MESSAGE( ret, "Failed to initialize data" );
}

/*
 * Force failure of callback registration within _registerCallbacks
 */
void test_01_IotBleDataTransfer_Init( void )
{
    bool ret = false;

    IotBle_RegisterEventCb_Stub( NULL );

    IotBle_RegisterEventCb_ExpectAnyArgsAndReturn( eBTStatusFail );
    ret = IotBleDataTransfer_Init();
    TEST_ASSERT_FALSE( ret );

    IotBle_RegisterEventCb_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    IotBle_RegisterEventCb_ExpectAnyArgsAndReturn( eBTStatusFail );
    ret = IotBleDataTransfer_Init();
    TEST_ASSERT_FALSE( ret );

    IotBle_RegisterEventCb_Stub( IotBle_RegisterEventCb_Callback );
}

/*
 * Force failure of channel initialization
 */
void test_02_IotBleDataTransfer_Init( void )
{
    bool ret = false;

    IotSemaphore_Create_Stub( NULL );

    IotSemaphore_Create_IgnoreAndReturn( false ); /* Can be called variably, depending on # of services */
    ret = IotBleDataTransfer_Init();
    TEST_ASSERT_FALSE( ret );

    IotSemaphore_Create_Stub( IotSemaphore_Create_Callback );
}

/*
 * Service creation fails
 */
void test_03_IotBleDataTransfer_Init( void )
{
    bool ret = false;

    IotBle_CreateService_Stub( NULL );

    IotBle_CreateService_ExpectAnyArgsAndReturn( eBTStatusFail );
    ret = IotBleDataTransfer_Init();
    TEST_ASSERT_FALSE(ret);

    IotBle_CreateService_Stub( IotBle_CreateService_Callback );
}
/*******************************************************************************
 * IotBleDataTransfer_Open
 ******************************************************************************/

/*
 * Happy path for opening a service. Then try to reopen it again
 */
void test_00_IotBleDataTransfer_Open( void )
{
    uint8_t service_variant = -1;
    IotBleDataTransferChannel_t * channel = NULL;
    init_transfers();

    #if ( IOT_BLE_ENABLE_DATA_TRANSFER_SERVICE == 1 )
        #if ( IOT_BLE_ENABLE_WIFI_PROVISIONING == 1 )
            service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_WIFI_PROVISIONING;
        #endif
        #if ( IOT_BLE_ENABLE_MQTT == 1 )
            service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;
        #endif
    #endif

    channel = IotBleDataTransfer_Open(service_variant);
    TEST_ASSERT(channel);

    // Exercise attempt to reopen
    IotBleDataTransferChannel_t * reopened_channel = IotBleDataTransfer_Open(service_variant);
    TEST_ASSERT(reopened_channel == NULL);
}

/*
 * Try opening a service that doesn't exist
 */
void test_01_IotBleDataTransfer_Open( void )
{
    IotBleDataTransferChannel_t * pChannel = IotBleDataTransfer_Open(-1);
    TEST_ASSERT(pChannel == NULL);
}
/*******************************************************************************
 * IotBleDataTransfer_SetCallback
 ******************************************************************************/

/*
 * Happy path with ready channel. Don't care for context here
 */
void test_00_IotBleDataTransfer_SetCallback( void )
{
    bool ret = false;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT );
    ret = IotBleDataTransfer_SetCallback( pChannel, channel_callback, NULL );

    TEST_ASSERT( ret );
}


/*
 * Exercise the fail paths for this call
 */
void test_01_IotBleDataTransfer_SetCallback( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;
    bool ret = false;
    init_transfers();

    ret = IotBleDataTransfer_SetCallback( NULL, channel_callback, NULL );
    TEST_ASSERT_FALSE(ret);

    IotBleDataTransferChannel_t *pChannel = IotBleDataTransfer_Open(service_variant);
    IotBleDataTransfer_Reset(pChannel);
    ret = IotBleDataTransfer_SetCallback( pChannel, channel_callback, NULL );
    TEST_ASSERT_FALSE(ret);

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    pChannel = get_open_channel(service_variant);
    ret = IotBleDataTransfer_SetCallback( pChannel, NULL, NULL );
    TEST_ASSERT_FALSE(ret);

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    pChannel = get_open_channel(service_variant);
    ret = IotBleDataTransfer_SetCallback( pChannel, channel_callback, NULL );
    ret = IotBleDataTransfer_SetCallback( pChannel, channel_callback, NULL );
    TEST_ASSERT_FALSE(ret);
}

/*******************************************************************************
 * IotBleDataTransfer_Send
 ******************************************************************************/

/*
 * Happy path
 */
void test_00_IotBleDataTransfer_Send( void )
{
    size_t n_sent = 0;
    const uint16_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );

    uint8_t msg[] = "I am test data";
    IotBle_SendIndication_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    n_sent = IotBleDataTransfer_Send( pChannel, msg, sizeof( msg ) );
    TEST_ASSERT_MESSAGE( n_sent == sizeof( msg ), "Did not send all expected bytes" );

    IotBle_SendIndication_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    IotBleDataTransfer_SetCallback( pChannel, channel_callback, NULL );
    n_sent = IotBleDataTransfer_Send( pChannel, msg, sizeof( msg ) );
    TEST_ASSERT_MESSAGE( n_sent == sizeof( msg ), "Did not send all expected bytes" );

    uint8_t full_msg[get_max_data_len()];
    memset(full_msg, 0xDC, sizeof(full_msg));
    IotBle_SendIndication_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    n_sent = IotBleDataTransfer_Send( pChannel, full_msg, sizeof( full_msg ) );
    TEST_ASSERT_MESSAGE( n_sent == sizeof( full_msg ), "Did not send all expected bytes" );
}

/*
 * Client sends some data but attempt to send indication fails
 */
void test_01_IotBleDataTransfer_Send( void )
{
    size_t n_sent = 0;
    const uint16_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );
    IotBleDataTransfer_SetCallback( pChannel, channel_callback, NULL );

    IotBle_SendIndication_ExpectAnyArgsAndReturn( eBTStatusFail );
    uint8_t msg[] = "I am test data";
    n_sent = IotBleDataTransfer_Send( pChannel, msg, sizeof( msg ) );
    TEST_ASSERT( n_sent == 0 );
}

/*
 * Attempt to send a message that is larger than the current negotiated MTU size such that it exercises
 */
void test_02_IotBleDataTransfer_Send( void )
{
    size_t n_sent = 0;
    const uint16_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );

    IotBle_SendIndication_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    uint8_t msg[ get_max_data_len() + 1 ];
    memset( msg, 0xDC, sizeof( msg ) );
    n_sent = IotBleDataTransfer_Send( pChannel, msg, sizeof( msg ) );
    TEST_ASSERT_MESSAGE( n_sent == sizeof( msg ), "Did not send all expected bytes" );
}

/*
 * Send max-capacity packet, then needs to resize send buffer to send remainder of message, but buffer resizing fails
 * Currently this means that the remainder issued by call the _Send
 */
void test_03_IotBleDataTransfer_Send( void )
{
    size_t n_sent = 0;
    const uint16_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );


    pvPortMalloc_Stub( NULL );
    IotBle_SendIndication_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    pvPortMalloc_ExpectAnyArgsAndReturn( NULL );
    uint8_t msg[ get_max_data_len() + 1 ];
    memset( msg, 0xDC, sizeof( msg ) );
    n_sent = IotBleDataTransfer_Send( pChannel, msg, sizeof( msg ) );
    TEST_ASSERT( n_sent == get_max_data_len() );
    pvPortMalloc_Stub( pvPortMalloc_Callback );
}

/*
 * Send a large message over multiple packets, such that a remainder needs to be stored in the send buffer.
 * Force failure of buffer resizing
 */
void test_04_IotBleDataTransfer_Send( void )
{
    size_t n_sent = 0;
    const uint16_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );

    /* First send a large message so that the send buffer is created */
    IotBle_SendIndication_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    uint8_t msg[ 4 * get_max_data_len() + 1 ];
    memset( msg, 0xDC, sizeof( msg ) );
    n_sent = IotBleDataTransfer_Send( pChannel, msg, get_max_data_len() + 1 );
    TEST_ASSERT( n_sent == get_max_data_len() + 1 );

    /* Now that the send buffer exists, we can exercices its resizing but allocation fails at first*/
    pvPortMalloc_Stub( NULL );
    IotBle_SendIndication_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    pvPortMalloc_ExpectAnyArgsAndReturn( NULL );
    n_sent = IotBleDataTransfer_Send( pChannel, msg, sizeof( msg ) );
    TEST_ASSERT( n_sent == get_max_data_len() );
    pvPortMalloc_Stub( pvPortMalloc_Callback );

}

/*
 * Send a large message that requires multiple packet transmissions, but call to internal _send helper fails
 * because SendIndication fails
 */
void test_05_IotBleDataTransfer_Send( void )
{
    size_t n_sent = 0;
    const uint16_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );

    IotBle_SendIndication_ExpectAnyArgsAndReturn( eBTStatusFail );
    uint8_t msg[ get_max_data_len() + 1 ];
    memset( msg, 0xDC, sizeof( msg ) );
    n_sent = IotBleDataTransfer_Send( pChannel, msg, sizeof( msg ) );
    TEST_ASSERT( n_sent == 0 );
}


/*
 * Send a large message that requires multiple packet transmissions but there's a timeout after waiting for the
 * sendComplete semaphore to be available
 */
void test_06_IotBleDataTransfer_Send( void )
{
    size_t n_sent = 0;
    const uint16_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );

    IotSemaphore_TimedWait_Stub( NULL );
    IotSemaphore_TimedWait_ExpectAnyArgsAndReturn( false );
    uint8_t msg[ get_max_data_len() + 1 ];
    memset( msg, 0xDC, sizeof( msg ) );
    n_sent = IotBleDataTransfer_Send( pChannel, msg, sizeof( msg ) );
    IotSemaphore_TimedWait_Stub( IotSemaphore_TimedWait_Callback );
    TEST_ASSERT( n_sent == 0 );
}

/*
 * Attempt to send when channel doesn't exist or wasn't opened
 */
void test_07_IotBleDataTransfer_Send( void )
{
    size_t n_sent = 0;
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    init_transfers();
    IotBleDataTransferChannel_t *pChannel = IotBleDataTransfer_Open( service_variant );

    IotBleDataTransfer_Send(NULL, NULL, 0);
    TEST_ASSERT(n_sent == 0);

    IotBleDataTransfer_Send(pChannel, NULL, 0);
    TEST_ASSERT(n_sent == 0);
}

/*******************************************************************************
 * IotBleDataTransfer_Receive
 ******************************************************************************/

/*
 * Happy path making sure that
 */
void test_00_IotBleDataTransfer_Receive( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    uint8_t msg_out[] = "Test data from fake client";
    uint8_t msg_out_size = sizeof( msg_out );
    generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_RX_CHAR, msg_out, msg_out_size, true);

    uint8_t msg_in[ sizeof( msg_out ) ] = { 0 };
    uint8_t msg_in_size = IotBleDataTransfer_Receive( pChannel, msg_in, msg_out_size );

    TEST_ASSERT_MESSAGE( msg_in_size == msg_out_size, "Did not receive entirety of expected message" );
    TEST_ASSERT_EQUAL_UINT8_ARRAY( msg_out, msg_in, msg_out_size );
}

/*
 * Server receives more bytes than it request to read, then flushes an empty rx buffer
 */
void test_01_IotBleDataTransfer_Receive( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    uint8_t msg_out[] = "Test data from fake client";
    uint8_t msg_out_size = sizeof( msg_out );
    generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_RX_CHAR, msg_out, msg_out_size, true );

    const size_t requested_in_size = msg_out_size / 2;
    const size_t remaining_in_size = msg_out_size - requested_in_size;
    uint8_t msg_in[ sizeof( msg_out ) ] = { 0 };
    uint8_t msg_in_size = IotBleDataTransfer_Receive( pChannel, msg_in, requested_in_size );
    TEST_ASSERT( msg_in_size == requested_in_size );
    TEST_ASSERT_EQUAL_UINT8_ARRAY( msg_out, msg_in, requested_in_size );

    msg_in_size = IotBleDataTransfer_Receive(pChannel, NULL, msg_out_size);
    TEST_ASSERT(msg_in_size == remaining_in_size);
}

/*******************************************************************************
 * IotBleDataTransfer_PeekReceiveBuffer
 ******************************************************************************/

/*
 * Happy path
 */
void test_00_IotBleDataTransfer_PeekReceiveBuffer( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    uint8_t msg_out[] = "Test data from fake client";
    uint8_t msg_out_size = sizeof( msg_out );
    generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_RX_CHAR, msg_out, msg_out_size, true );

    const uint8_t * msg_in = NULL;
    size_t msg_in_size = 0;
    IotBleDataTransfer_PeekReceiveBuffer( pChannel, &msg_in, &msg_in_size );

    TEST_ASSERT( msg_in );
    TEST_ASSERT_MESSAGE( msg_in_size == msg_out_size, "Did not receive entirety of expected message" );
    TEST_ASSERT_EQUAL_UINT8_ARRAY( msg_out, msg_in, msg_out_size );
}


/*
 * Client tries to peak into null receive buffer
 */
void test_01_IotBleDataTransfer_PeekReceiveBuffer( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );

    const uint8_t * msg_in = NULL;
    size_t msg_in_size = 0;
    IotBleDataTransfer_PeekReceiveBuffer( pChannel, &msg_in, &msg_in_size );

    TEST_ASSERT( msg_in == NULL );
    TEST_ASSERT( msg_in_size == 0 );
}

/*******************************************************************************
 * IotBleDataTransfer_Close
 ******************************************************************************/

/*
 * Happy path
 */
void test_00_IotBleDataTransfer_Close( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );
    IotBleDataTransfer_Close( pChannel );
}

/*
 * Close during pending _Send, which will finish before the timeout period
 */

/*
 * Close during pending _Send, which will timeout
 */

/*******************************************************************************
 * IotBleDataTransfer_Reset
 ******************************************************************************/

/*
 * Happy path
 */
void test_00_IotBleDataTransfer_Reset( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );
    IotBleDataTransfer_Close( pChannel );
}

/*******************************************************************************
 * IotBleDataTransfer_Cleanup
 ******************************************************************************/

/*
 * Happy path
 */
void test_00_IotBleDataTransfer_Cleanup( void )
{
    bool ret = false;

    /* To reuse Cmocks forecasted returns, must uninstall any stubs */
    IotBle_UnRegisterEventCb_Stub( NULL );

    IotBle_UnRegisterEventCb_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    IotBle_UnRegisterEventCb_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    ret = IotBleDataTransfer_Cleanup();
    TEST_ASSERT( ret );

    /* Reinstall callback that most other tests use */
    IotBle_UnRegisterEventCb_Stub( IotBle_UnregisterEventCb_Callback );
}

/*
 * First call to unregister fails and short circuits failure
 */
void test_01_IotBleDataTransfer_Cleanup( void )
{
    bool ret = false;

    IotBle_UnRegisterEventCb_Stub( NULL );

    IotBle_UnRegisterEventCb_ExpectAnyArgsAndReturn( eBTStatusFail );
    ret = IotBleDataTransfer_Cleanup();
    TEST_ASSERT_FALSE( ret );

    IotBle_UnRegisterEventCb_Stub( IotBle_UnregisterEventCb_Callback );
}

/*
 * Last call to unregister fails
 */
void test_02_IotBleDataTransfer_Cleanup( void )
{
    bool ret = false;

    IotBle_UnRegisterEventCb_Stub( NULL );

    IotBle_UnRegisterEventCb_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    IotBle_UnRegisterEventCb_ExpectAnyArgsAndReturn( eBTStatusFail );
    ret = IotBleDataTransfer_Cleanup();
    TEST_ASSERT_FALSE( ret );

    IotBle_UnRegisterEventCb_Stub( IotBle_UnregisterEventCb_Callback );
}



/*
 * Channel cleanup fails
 */
void test_03_IotBleDataTransfer_Cleanup( void )
{
    bool ret = false;

    IotBle_DeleteService_Stub( NULL );

    IotBle_DeleteService_IgnoreAndReturn( eBTStatusFail );
    ret = IotBleDataTransfer_Cleanup();
    TEST_ASSERT_FALSE( ret );

    IotBle_DeleteService_Stub( IotBle_DeleteService_Callback );
}

/*******************************************************************************
 * Internal Callback Exercises
 ******************************************************************************/

/*
 * Generate the connect/disconnect event to exercise internal callback for connection events
 */
void test_00_connectionCallback( void )
{
    bool ret = false;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT );
    ret = IotBleDataTransfer_SetCallback( pChannel, channel_callback, NULL );
    TEST_ASSERT( ret );


    generate_connect_event(eBTStatusSuccess);
    generate_disconnect_event(eBTStatusSuccess);

    generate_connect_event(eBTStatusFail);
}


/*
 * Generate the MTU changed event to exercies internal callbaks
 */
void test_00_MTUChangedCallback( void )
{
    bool ret = false;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT );
    ret = IotBleDataTransfer_SetCallback( pChannel, channel_callback, NULL );
    TEST_ASSERT( ret );

    size_t mtu = get_mtu();
    generate_mtu_changed_event( get_mtu() * 2 );
    generate_mtu_changed_event( mtu );
    generate_mtu_changed_event( mtu );
}

/*
 * No server-tx data that wasn't fit into an indication, but client reads attempts a small read any way
 */
void test_00_TXMesgCharCallback( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;
    init_transfers();

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    generate_client_read_event( service_variant, IOT_BLE_DATA_TRANSFER_TX_CHAR );
}

/*
 * More reasonably, large data couldn't fit in the small char characteristic and in one indication. So client
 * asks for the rest of the data and server sends over multiple, fully filled per mtu, packets
 */
void test_00_TXLargeMesgCharCallback( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );
    IotBleDataTransfer_SetCallback( pChannel, channel_callback, NULL );

    IotBle_SendIndication_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    uint8_t msg[ get_max_data_len() + 1 ];
    memset( msg, 0xDC, sizeof( msg ) );
    IotBleDataTransfer_Send( pChannel, msg, sizeof( msg ) );

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    generate_client_read_event( service_variant, IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR );
    generate_client_read_event( service_variant, IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR );
}

/*
 * Client reads remaining data from server, but remainder is still too large to fit within mtu
 */
void test_01_TXLargeMesgCharCallback( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );

    IotBle_SendIndication_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    uint8_t msg[ 2 * get_max_data_len() + 1 ];
    memset( msg, 0xDC, sizeof( msg ) );
    IotBleDataTransfer_Send( pChannel, msg, sizeof( msg ) );

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    generate_client_read_event( service_variant, IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR );
    generate_client_read_event( service_variant, IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR );
}

/*
 * Client tries to read data that requires 2 separatre packets. The first one is successful, the second is not
 */
void test_02_TXLargeMesgCharCallback( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );
    IotBleDataTransfer_SetCallback( pChannel, channel_callback, NULL );

    IotBle_SendIndication_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    uint8_t msg[ 2 * get_max_data_len() + 1 ];
    memset( msg, 0xDC, sizeof( msg ) );
    IotBleDataTransfer_Send( pChannel, msg, sizeof( msg ) );

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusFail );
    generate_client_read_event( service_variant, IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR );
    generate_client_read_event( service_variant, IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR );
}

/*
 * Client tries to read from closed channel. They've yet to indicate that they are ready
 */
void test_03_TXLargeMesgCharCallback( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    generate_client_read_event( service_variant, IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR );
}

/*
 * Server gets a malrouted event
 */
void test_04_TXLargeMesgCharCallback( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;
    init_transfers();

    LOTServiceVariantTracker_t * service_tracker = get_service_tracker_from_short_id( service_variant );
    uint8_t dummy = 0xAA;
    IotBleWriteEventParams_t write_params = {
            .transId = 0,
            .pRemoteBdAddr = 0,
            .pValue = &dummy,
            .length = 1,
            .connId = 0,
            .attrHandle = 0,
            .offset = 0,
            .needRsp = 0,
            .isPrep = 0
    };

    IotBleAttributeEvent_t event =
            {
                .pParamWrite = &write_params,
                .xEventType = -1
            };

    IotBleAttributeEventCallback_t callback = service_tracker->attr_callbacks[ IOT_BLE_DATA_TRANSFER_TX_CHAR ];
    callback( &event );

    callback = service_tracker->attr_callbacks[ IOT_BLE_DATA_TRANSFER_TX_LARGE_CHAR ];
    callback( &event );
}

/*
 * Client sends some data to server
 */
void test_00_RXMesgCharCallback( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );
    IotBleDataTransfer_SetCallback( pChannel, channel_callback, NULL );

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    uint8_t msg[] = "Hi server, this is client";
    generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_RX_CHAR, msg, sizeof( msg ), true );
}


/*
 * Client sends message that was couldn't fit within current MTU. They should have written to RXLargeMessage
 * which can accumulate large messages over multiple packets, but instead they send it to RXMessage
 */
void test_01_RXMesgCharCallback( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );
    IotBleDataTransfer_SetCallback( pChannel, channel_callback, NULL );

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    uint8_t msg[ 4 * get_max_data_len() ];
    memset( msg, 0xDC, sizeof( msg ) );
    generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_RX_CHAR, msg, sizeof( msg ), true);
}


/*
 * Client sends message that was couldn't fit within current MTU.
 */
void test_02_RXMesgCharCallback( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;
    init_transfers();
    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );
    TEST_ASSERT(pChannel);

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    uint8_t msg[ 4 * get_max_data_len() ];
    memset( msg, 0xDC, sizeof( msg ) );
    generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_RX_CHAR, msg, sizeof( msg ), true );
    generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_RX_CHAR, msg, sizeof( msg ), false );
}

/*
 * At the time of writing this test, if a client sent a large enough first message it could cause a heap overflow
 */
/*#define HEAP_OVERFLOW_0 */
void test_00_RXLargeMesgCharCallback( void )
{
    #ifdef HEAP_OVERFLOW_0
        const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

        IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
        init_transfers();
        IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );

        IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
        uint8_t msg[ 2 * IOT_BLE_DATA_TRANSFER_RX_BUFFER_SIZE ];
        memset( msg, 0xDC, sizeof( msg ) );
        generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR, msg, sizeof( msg ), true);
    #endif /* ifdef HEAP_OVERFLOW_0 */
}

/*
 * First client write large message that can fit into the #defined initial receive buffer size
 */
void test_01_RXLargeMesgCharCallback( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );
    IotBleDataTransfer_SetCallback( pChannel, channel_callback, NULL );

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    uint8_t msg[ get_max_data_len() / 2 ];
    memset( msg, 0xDC, sizeof( msg ) );
    generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR, msg, sizeof( msg ), true);
}


/*
 * First client large write can fit into the #defined initial receive buffer size, but a subsequent one can't.
 * Currently, this results in a buffer realloc resizing
 */
void test_02_RXLargeMesgCharCallback( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;
    init_transfers();
    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );
    TEST_ASSERT(pChannel);

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    uint8_t msg[ IOT_BLE_DATA_TRANSFER_RX_BUFFER_SIZE ];
    memset( msg, 0xDC, sizeof( msg ) );
    generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR, msg, sizeof( msg ), true );
    memset( msg, ~0xDC, sizeof( msg ) );
    generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR, msg, sizeof( msg ), true);
}

/*
 * The incoming message is larger than currently allocated receive buf. A resize is necessary but fails
 */
void test_03_RXLargeMesgCharCallback( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    init_transfers();
    IotBleDataTransferChannel_t *pChannel = get_open_channel(service_variant);
    TEST_ASSERT(pChannel);

    pvPortMalloc_Stub( NULL );
    pvPortMalloc_ExpectAnyArgsAndReturn( NULL );
    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    pvPortMalloc_ExpectAnyArgsAndReturn( NULL );
    uint8_t msg[ 2 * get_max_data_len() ];
    memset( msg, ~0xDC, sizeof( msg ) );
    generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR, msg, sizeof( msg ) , true);
    generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR, msg, sizeof( msg ) , false);
    pvPortMalloc_Stub( pvPortMalloc_Callback );
}


/*
 * Client writes to large char characteristic but the service hasn't been created yet
 */
void test_04_RXLargeMesgCharCallback( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;

    IotBle_SendResponse_ExpectAnyArgsAndReturn(eBTStatusSuccess);
    generate_client_write_event(service_variant, IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR, NULL, 0, true);
    generate_client_write_event(service_variant, IOT_BLE_DATA_TRANSFER_RX_LARGE_CHAR, NULL, 0, false);
}

/*
 * Client sends appropriately long data to update the configuration descriptor
 */
void test_00_clientCharCfgDescrCallback( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;
    init_transfers();

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    uint8_t config[ 2 ] = { 0 };
    generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_TX_CHAR_DESCR, config, sizeof( config ) , true );
    generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_TX_CHAR_DESCR, config, sizeof( config ) , false );
}

/*
 * Client attempts to write excessively long data into configuration descriptor
 */
void test_01_clientCharCfgDescrCallback( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;
    init_transfers();

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    uint8_t config[ 4 ] = { 0 };
    generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_TX_CHAR_DESCR, config, sizeof( config ) , true );
}


/*
 * Client reads configuration descriptor
 */
void test_02_clientCharCfgDescrCallback( void )
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;
    init_transfers();

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    generate_client_read_event( service_variant, IOT_BLE_DATA_TRANSFER_TX_CHAR_DESCR );
}

/*
 * Client writes control CONTROL CHAR characteristic with callback installed
 */
void test_00_ControlCharCallback()
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;
    init_transfers();

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );
    IotBleDataTransfer_SetCallback( pChannel, channel_callback, NULL );

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    uint8_t config[ 2 ] = { 0xDC, 0xDC };
    generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_CONTROL_CHAR, config, sizeof( config ), true);
    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    generate_client_write_event( service_variant, IOT_BLE_DATA_TRANSFER_CONTROL_CHAR, config, sizeof( config ), false);
}

/*
 * Client reads control CONTROL CHAR characteristic but provides invalid attr handle.
 */
void test_01_ControlCharCallback()
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;
    init_transfers();

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    IotBleDataTransferChannel_t * pChannel = get_open_channel( service_variant );
    IotBleDataTransfer_SetCallback( pChannel, channel_callback, NULL );

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    generate_client_read_event( service_variant, IOT_BLE_DATA_TRANSFER_CONTROL_CHAR );
}

/*
* Write when channel isn't ready
*/
void test_03_ControlCharCallback()
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;
    init_transfers();

    // Install callback to unready channel
    IotBleDataTransferChannel_t *pChannel = IotBleDataTransfer_Open(service_variant);
    IotBleDataTransfer_SetCallback( pChannel, channel_callback, NULL );
    TEST_ASSERT(pChannel);

    // Write with valid handle but channel is not ready, then try again when it is
    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    uint8_t ready = 0;
    generate_client_write_event(service_variant, IOT_BLE_DATA_TRANSFER_CONTROL_CHAR, &ready, 1, false);

    // Now write when ready
    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    ready = 1;
    generate_client_write_event(service_variant, IOT_BLE_DATA_TRANSFER_CONTROL_CHAR, &ready, 1, false);

}


/*
 * Send malrouted events to various gatt server characteristic callbacks
 */
void test_ServerCallbacks_With_MalroutedEvents()
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;
    init_transfers();

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    IotBleDataTransferChannel_t *pChannel = get_open_channel(service_variant);
    TEST_ASSERT(pChannel);

    for (IotBleDataTransferAttributes_t attr=IOT_BLE_DATA_TRANSFER_CONTROL_CHAR;
            attr < IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES;
            attr++) {
        generate_event_with_bad_type(service_variant, attr);
    }
}

/*
 * Send various read/write events to gatt server characteristic callbacks with bad handles
 */
void test_ServerCallbacks_With_BadAttrHandles()
{
    const uint8_t service_variant = IOT_BLE_DATA_TRANSFER_SERVICE_TYPE_MQTT;
    init_transfers();

    IotBle_SendResponse_ExpectAnyArgsAndReturn( eBTStatusSuccess );
    IotBleDataTransferChannel_t *pChannel = get_open_channel(service_variant);
    TEST_ASSERT(pChannel);


    IotBle_SendResponse_IgnoreAndReturn(eBTStatusSuccess);
    for (IotBleDataTransferAttributes_t attr=IOT_BLE_DATA_TRANSFER_CONTROL_CHAR;
            attr < IOT_BLE_DATA_TRANSFER_MAX_ATTRIBUTES;
            attr++) {
        generate_event_with_bad_handle(service_variant, attr, true);
        generate_event_with_bad_handle(service_variant, attr, false);
    }
}


