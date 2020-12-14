#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "wiced_bt_types.h"
#include "wiced_result.h"
#include "bt_target.h"
/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/


/******************************************************
 *               External Function Declarations
 ******************************************************/
/******************************************************
 *               Variable Definitions
 ******************************************************/


/******************************************************
 *               Function Definitions
 ******************************************************/
/**
 * \brief Definition of the callback function to send proxy packet.
 * \details Application implements that function if it want to use
 * GATT notification or even external function (for example MeshController).
 * Called by core to send packet to the proxy client.
 *
 * @param[in]   packet          :Packet to send
 * @param[in]   packet_len      :Length of the packet to send
 *
 * @return      None
 */
typedef void(*wiced_bt_mesh_core_gatt_send_cb_t)(const uint8_t *packet, uint32_t packet_len);

/**
 * \brief Definition of the callback function on provisioning end.
 * \details Provisioner or/and provisioning application implements that function to be called on successfull or failed end of provisioning.
 *
 * @param[in]   conn_id     :Connection ID of the provisioning connection
 * @param[in]   result      ::Provisioning Result code (see @ref BT_MESH_PROVISION_RESULT "Provisioning Result codes")
 *
 * @return   None
 */
typedef void (*wiced_bt_mesh_provision_end_cb_t)(
    uint32_t  conn_id,
    uint8_t   result);

typedef void (*wiced_bt_mesh_write_nvram_data_cb_t) (int id, uint8_t *payload, uint32_t payload_len);

/**
 * \brief Definition of the callback function of mesh status
 * \details Application implements the function to know the current status of the mesh.
 *
  * @param[in]   status     :: Mesh status
 *
 * @return   None
 */
typedef void (*wiced_bt_mesh_status_cb_t)(uint8_t status);


wiced_result_t wiced_bt_mesh_connect( wiced_bt_device_address_t bd_address );
wiced_result_t wiced_bt_mesh_uart_init();
wiced_result_t wiced_bt_mesh_init(wiced_bt_mesh_provision_end_cb_t prov_end_cb,wiced_bt_mesh_core_gatt_send_cb_t proxy_data_cb, wiced_bt_mesh_write_nvram_data_cb_t write_nvram_data_cb, wiced_bt_mesh_status_cb_t mesh_status_cb);
wiced_result_t wiced_bt_mesh_send_proxy_packet(uint8_t* p_data, uint8_t data_len);
wiced_result_t wiced_bt_mesh_proxy_connect(uint8_t connection_state);
wiced_result_t wiced_bt_mesh_push_nvram_data(uint8_t *data , uint8_t data_len , uint8_t idx);
wiced_result_t wiced_bt_mesh_reboot();


#ifdef __cplusplus
} /*extern "C" */
#endif

