#ifndef AWS_BLE_GAP_CONFIG_H
#define AWS_BLE_GAP_CONFIG_H

#define aws_ble_gap_configAPP_BLE_OBSERVER_PRIO             3                                 /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define aws_ble_gap_configAPP_BLE_CONN_CFG_TAG              1                                 /**< A tag identifying the SoftDevice BLE configuration. */

#define aws_ble_gap_configFIRST_CONN_PARAMS_UPDATE_DELAY    1000                              /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define aws_ble_gap_configNEXT_CONN_PARAMS_UPDATE_DELAY     3000                              /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define aws_ble_gap_configMAX_CONN_PARAMS_UPDATE_COUNT      3                                 /**< Number of attempts before giving up the connection parameter negotiation. */

#define aws_ble_gap_configADV_DURATION                      18000                             /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define aws_ble_gap_configMIN_CONN_INTERVAL                 MSEC_TO_UNITS( 20, UNIT_1_25_MS ) /**< Minimum acceptable connection interval (0.5 seconds). */
#define aws_ble_gap_configMAX_CONN_INTERVAL                 MSEC_TO_UNITS( 30, UNIT_1_25_MS ) /**< Maximum acceptable connection interval (1 second). */
#define aws_ble_gap_configSLAVE_LATENCY                     0                                 /**< Slave latency. */
#define aws_ble_gap_configCONN_SUP_TIMEOUT                  MSEC_TO_UNITS( 4000, UNIT_10_MS ) /**< Connection supervisory time-out (4 seconds). */


#endif /* AWS_BLE_GAP_CONFIG_H */
