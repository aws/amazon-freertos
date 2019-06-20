#ifndef BLE_CUS_H__
#define BLE_CUS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

/**@brief   Macro for defining instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_CUS_DEF( _name )                         \
    ble_cus_t _name;                                 \
    NRF_SDH_BLE_OBSERVER( _name ## _obs,             \
                          BLE_HRS_BLE_OBSERVER_PRIO, \
                          ble_cus_on_ble_evt, &_name )



#define CUSTOM_SERVICE_UUID_BASE            { 0x00, 0xFF, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }
#define deviceInfoCHAR_AFR_VER_UUID         { 0x01, 0xFF, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }
#define deviceInfoBROKER_ENDPOINT_UUID      { 0x02, 0xFF, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }
#define deviceInfoCHAR_MTU_UUID             { 0x03, 0xFF, 0x32, 0xF9, 0x79, 0xE6, 0xB5, 0x83, 0xFB, 0x4E, 0xAF, 0x48, 0x68, 0x11, 0x7F, 0x8A }
#define deviceInfoCLIENT_CHAR_CFG_UUID      ( 0x2902 )

#define mqttProxySVC_UUID                   { 0x00, 0xFF, 0xC3, 0x4C, 0x04, 0x48, 0x02, 0xA0, 0xA9, 0x40, 0x2E, 0xD7, 0x6A, 0x16, 0xD7, 0xA9 }
#define mqttProxyCHAR_CONTROL_UUID          { 0x01, 0xFF, 0xC3, 0x4C, 0x04, 0x48, 0x02, 0xA0, 0xA9, 0x40, 0x2E, 0xD7, 0x6A, 0x16, 0xD7, 0xA9 }
#define mqttProxyCHAR_TX_MESG_UUID          { 0x02, 0xFF, 0xC3, 0x4C, 0x04, 0x48, 0x02, 0xA0, 0xA9, 0x40, 0x2E, 0xD7, 0x6A, 0x16, 0xD7, 0xA9 }
#define mqttProxyCHAR_RX_MESG_UUID          { 0x03, 0xFF, 0xC3, 0x4C, 0x04, 0x48, 0x02, 0xA0, 0xA9, 0x40, 0x2E, 0xD7, 0x6A, 0x16, 0xD7, 0xA9 }
#define mqttProxyCHAR_TX_LARGE_MESG_UUID    { 0x04, 0xFF, 0xC3, 0x4C, 0x04, 0x48, 0x02, 0xA0, 0xA9, 0x40, 0x2E, 0xD7, 0x6A, 0x16, 0xD7, 0xA9 }
#define mqttProxyCHAR_RX_LARGE_MESG_UUID    { 0x05, 0xFF, 0xC3, 0x4C, 0x04, 0x48, 0x02, 0xA0, 0xA9, 0x40, 0x2E, 0xD7, 0x6A, 0x16, 0xD7, 0xA9 }
#define mqttProxyCCFG_UUID                  ( 0x2902 )

#define CUSTOM_SERVICE_UUID                 ( 0x1168 )
#define MQTT_CUSTOM_SERVICE_UUID            ( 0x166A )


/**@brief Custom Service event type. */
typedef enum
{
    BLE_CUS_EVT_NOTIFICATION_ENABLED,  /**< Custom value notification enabled event. */
    BLE_CUS_EVT_NOTIFICATION_DISABLED, /**< Custom value notification disabled event. */
    BLE_CUS_EVT_DISCONNECTED,
    BLE_CUS_EVT_CONNECTED
} ble_cus_evt_type_t;

/**@brief Custom Service event. */
typedef struct
{
    ble_cus_evt_type_t evt_type; /**< Type of event. */
} ble_cus_evt_t;

/* Forward declaration of the ble_cus_t type. */
typedef struct ble_cus_s ble_cus_t;


/**@brief Custom Service event handler type. */
typedef void (* ble_cus_evt_handler_t) ( ble_cus_t * p_bas,
                                         ble_cus_evt_t * p_evt );

/**@brief Battery Service init structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_cus_evt_handler_t evt_handler;                      /**< Event handler to be called for handling events in the Custom Service. */
    uint8_t initial_custom_value;                           /**< Initial custom value */
    ble_srv_cccd_security_mode_t custom_value_char_attr_md; /**< Initial security level for Custom characteristics attribute */
} ble_cus_init_t;

/**@brief Custom Service structure. This contains various status information for the service. */
struct ble_cus_s
{
    ble_cus_evt_handler_t evt_handler;             /**< Event handler to be called for handling events in the Custom Service. */
    uint16_t service_handle;                       /**< Handle of Custom Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t custom_value_handles; /**< Handles related to the Custom Value characteristic. */
    uint16_t conn_handle;                          /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t uuid_type;
};

/**@brief Function for initializing the Custom Service.
 *
 * @param[out]  p_cus       Custom Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_cus_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_cus_init( ble_cus_t * p_cus,
                       const ble_cus_init_t * p_cus_init );

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the Battery Service.
 *
 * @note
 *
 * @param[in]   p_cus      Custom Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_cus_on_ble_evt( ble_evt_t const * p_ble_evt,
                         void * p_context );

/**@brief Function for updating the custom value.
 *
 * @details The application calls this function when the cutom value should be updated. If
 *          notification has been enabled, the custom value characteristic is sent to the client.
 *
 * @note
 *
 * @param[in]   p_bas          Custom Service structure.
 * @param[in]   Custom value
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */

uint32_t ble_cus_custom_value_update( ble_cus_t * p_cus,
                                      uint8_t custom_value );

#endif /* BLE_CUS_H__ */
