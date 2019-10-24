#ifndef _IOT_TEST_BLE_HAL_CONFIG_DEFAULTS_H
#define _IOT_TEST_BLE_HAL_CONFIG_DEFAULTS_H

/* Enable/Disable test cases */
/* AFQP test */
#define ENABLE_TC_AFQP_WRITE_LONG                                             ( 1 )
#define ENABLE_TC_AFQP_ADD_INCLUDED_SERVICE                                   ( 0 )
#define ENABLE_TC_AFQP_SECONDARY_SERVICE                                      ( 0 )
/* Integration test */
#define ENABLE_TC_INTEGRATION_CALLBACK_NULL_CHECK                             ( 1 )
#define ENABLE_TC_INTEGRATION_ADD_CHARACTERISTIC_IN_CALLBACK                  ( 0 )
#define ENABLE_TC_INTEGRATION_ADVERTISE_WITHOUT_PROPERTY                      ( 1 )
#define ENABLE_TC_INTEGRATION_ADVERTISE_WITH_16BIT_SERVICEUUID                ( 1 )
#define ENABLE_TC_INTEGRATION_ADVERTISE_WITH_MANUFACTUREDATA                  ( 1 )
#define ENABLE_TC_INTEGRATION_ADVERTISE_INTERVAL_CONSISTENT_AFTER_BT_RESET    ( 1 )
#define ENABLE_TC_INTEGRATION_WRITE_NOTIFICATION_SIZE_GREATER_THAN_MTU_3      ( 1 )
#define ENABLE_TC_INTEGRATION_CONNECTION_TIMEOUT                              ( 1 )
#define ENABLE_TC_INTEGRATION_SEND_DATA_AFTER_DISCONNECTED                    ( 1 )
#define ENABLE_TC_INTEGRATION_INIT_ENABLE_TWICE                               ( 1 )

#endif /* ifndef _IOT_TEST_BLE_HAL_CONFIG_DEFAULTS_H */
