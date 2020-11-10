/*
 * FreeRTOS BLE HAL V5.1.0
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

/**
 * @file iot_test_ble_hal_config_defaults.h
 * @brief
 */

#ifndef _IOT_TEST_BLE_HAL_CONFIG_DEFAULTS_H
#define _IOT_TEST_BLE_HAL_CONFIG_DEFAULTS_H

/* Enable/Disable test cases */
/* DQP_FR test */
#define ENABLE_TC_AFQP_WRITE_LONG                                             ( 1 )
#define ENABLE_TC_AFQP_READ_LONG                                              ( 1 )
#define ENABLE_TC_AFQP_ADD_INCLUDED_SERVICE                                   ( 0 )
#define ENABLE_TC_AFQP_SECONDARY_SERVICE                                      ( 0 )
/* Integration test */
#define ENABLE_TC_INTEGRATION_ADD_CHARACTERISTIC_IN_CALLBACK                  ( 1 )
#define ENABLE_TC_INTEGRATION_ADVERTISE_WITHOUT_PROPERTY                      ( 1 )
#define ENABLE_TC_INTEGRATION_ADVERTISE_WITH_16BIT_SERVICEUUID                ( 1 )
#define ENABLE_TC_INTEGRATION_ADVERTISE_WITH_MANUFACTUREDATA                  ( 1 )
#define ENABLE_TC_INTEGRATION_ADVERTISE_WITH_SERVICEDATA                      ( 1 )
#define ENABLE_TC_INTEGRATION_ADVERTISE_INTERVAL_CONSISTENT_AFTER_BT_RESET    ( 1 )
#define ENABLE_TC_INTEGRATION_WRITE_NOTIFICATION_SIZE_GREATER_THAN_MTU_3      ( 1 )
#define ENABLE_TC_INTEGRATION_CONNECTION_TIMEOUT                              ( 1 )
#define ENABLE_TC_INTEGRATION_SEND_DATA_AFTER_DISCONNECTED                    ( 1 )
#define ENABLE_TC_INTEGRATION_CALLBACK_NULL_CHECK                             ( 1 )
#define ENABLE_TC_INTEGRATION_INIT_ENABLE_TWICE                               ( 1 )
#define ENABLE_TC_INTEGRATION_CHECK_BOND_STATE                                ( 1 )
#define ENABLE_TC_INTEGRATION_ENABLE_DISABLE_BT_MODULE                        ( 1 )
#define ENABLE_TC_INTEGRATION_CHANGE_MTU_SIZE                                 ( 1 )

#endif /* ifndef _IOT_TEST_BLE_HAL_CONFIG_DEFAULTS_H */
