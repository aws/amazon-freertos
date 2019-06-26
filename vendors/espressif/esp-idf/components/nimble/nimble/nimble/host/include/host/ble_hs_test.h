/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef H_HOST_TEST_
#define H_HOST_TEST_

#include <inttypes.h>
#ifdef __cplusplus
extern "C" {
#endif

struct os_mbuf;

int ble_att_clt_test_all(void);
int ble_att_svr_test_all(void);
int ble_gap_test_all(void);
int ble_gatt_conn_test_all(void);
int ble_gatt_disc_c_test_all(void);
int ble_gatt_disc_d_test_all(void);
int ble_gatt_disc_s_test_all(void);
int ble_gatt_find_s_test_all(void);
int ble_gatt_read_test_all(void);
int ble_gatt_write_test_all(void);
int ble_gatts_notify_test_all(void);
int ble_gatts_read_test_suite(void);
int ble_gatts_reg_test_all(void);
int ble_hs_adv_test_all(void);
int ble_hs_conn_test_all(void);
int ble_hs_hci_test_all(void);
int ble_hs_id_test_all(void);
int ble_l2cap_test_all(void);
int ble_os_test_all(void);
int ble_hs_pvcy_test_all(void);
int ble_sm_lgcy_test_suite(void);
int ble_sm_sc_test_suite(void);
int ble_sm_test_all(void);
int ble_stop_test_all(void);
int ble_store_test_all(void);
int ble_uuid_test_all(void);

#ifdef __cplusplus
}
#endif

#endif
