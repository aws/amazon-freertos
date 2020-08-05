/*
 * $ Copyright Cypress Semiconductor $
 */

#ifndef WICED_BT_DYNAMIC_GATTDB_H_INCLUDED
#define WICED_BT_DYNAMIC_GATTDB_H_INCLUDED

/**
 * [wiced_bt_dynamic_gattdb_add_service description]
 * @param  num_of_handles  Number of handles in this service
 * @param  service_handle  pointer to the service_handle (service handle will be assigned by this function)
 * @param  uuid            service UUID
 * @param  primary_service true , if primary service. false otherwise
 * @return WICED_SUCCESS if no error
 */
wiced_result_t wiced_bt_dynamic_gattdb_add_service( uint16_t num_of_handles, uint8_t* service_handle, wiced_bt_uuid_t* uuid, wiced_bool_t primary_service );

/**
 * [wiced_bt_dynamic_gattdb_delete_service description]
 * @param  service_handle service_handle of the service to be deleted
 * @return                WICED_SUCCESS if no error
 */
wiced_result_t wiced_bt_dynamic_gattdb_delete_service( uint8_t service_handle );

/**
 * [wiced_bt_dynamic_gattdb_add_included_service description]
 * @param  parent_service_handle             handle of the parent service
 * @param  service_handle                    pointer to the service_handle (service handle will be assigned by this function)
 * @param  included_service_attribute_handle handle of the service to be included
 * @param  end_group_handle                  last handle of the service to be included
 * @param  uuid                              uuid of the service to be included
 * @return                                   WICED_SUCCESS if no error
 */
wiced_result_t wiced_bt_dynamic_gattdb_add_included_service( uint8_t parent_service_handle, uint8_t *service_handle, uint8_t included_service_attribute_handle, uint16_t end_group_handle, wiced_bt_uuid_t* uuid );

/**
 * [wiced_bt_dynamic_gattdb_add_characteristic description]
 * @param  service_handle service_handle of the parent service
 * @param  char_handle    pointer to the char_hadle (char handle will be assigned by this function)
 * @param  uuid           char UUID
 * @param  property       properties of this characteristic
 * @param  permission     permissions for this characteristic
 * @return                WICED_SUCCESS if no error
 */
wiced_result_t wiced_bt_dynamic_gattdb_add_characteristic( uint8_t service_handle, uint8_t* char_handle, wiced_bt_uuid_t* uuid, uint8_t property, uint8_t permission );

/**
 * [wiced_bt_dynamic_gattdb_add_descriptor description]
 * @param  service_handle service_handle of the parent service
 * @param  desc_handle    pointer to the desc (desc handle will be assigned by this function)
 * @param  uuid           desc UUID
 * @param  permission     permissions for this descriptor
 * @return                WICED_SUCCESS if no error
 */
wiced_result_t wiced_bt_dynamic_gattdb_add_descriptor( uint8_t service_handle, uint8_t* desc_handle, wiced_bt_uuid_t* uuid, uint8_t permission );

/**
 * [wiced_bt_dynamic_gattdb_start_service description]
 * @param  service_handle service_handle of the service to be started
 * @return                WICED_SUCCESS if no error
 */
wiced_result_t wiced_bt_dynamic_gattdb_start_service( uint8_t service_handle );

/**
 * [wiced_bt_dynamic_gattdb_util_get_UUID description]
 * @param  service_handle handle of the service whose UUID is to be returned
 * @param  service_UUID   UUID if found, NULL otherwise
 * @return                WICED_SUCCESS if no error
 */
wiced_result_t wiced_bt_dynamic_gattdb_util_get_UUID( uint8_t service_handle, wiced_bt_uuid_t* service_UUID );

/**
 * [wiced_bt_dynamic_gattdb_util_get_service_end_handle description]
 * @param  service_handle handle of the service whose end handle is to be returned
 * @return                handle value if found, 0 otherwise
 */
uint16_t wiced_bt_dynamic_gattdb_util_get_service_end_handle( uint8_t service_handle ) ;

/**
 * Function     wiced_bt_gatt_db_reset
 *
 * Clear the gatt database
 *
 * @return      Return WICED_TRUE if database is cleared, otherwise WICED_FALSE
 */
wiced_bool_t wiced_bt_gatt_db_reset( void );

#endif // WICED_BT_DYNAMIC_GATTDB_H_INCLUDED
