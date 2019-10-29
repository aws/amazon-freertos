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

#ifndef H_BLETEST_PRIV_
#define H_BLETEST_PRIV_

#ifdef __cplusplus
extern "C" {
#endif

void bletest_send_conn_update(uint16_t handle);

#if (MYNEWT_VAL(BLE_LL_CFG_FEAT_LE_ENCRYPTION) == 1)
void bletest_ltk_req_reply(uint16_t handle);
int bletest_send_ltk_req_neg_reply(uint16_t handle);
int bletest_send_ltk_req_reply(uint16_t handle);
int bletest_hci_le_start_encrypt(struct hci_start_encrypt *cmd);
int bletest_hci_le_encrypt(uint8_t *key, uint8_t *pt);
#endif

int bletest_hci_reset_ctlr(void);
int bletest_hci_rd_bd_addr(void);
int bletest_hci_le_set_datalen(uint16_t handle, uint16_t txoctets,
                               uint16_t txtime);
int bletest_hci_le_write_sugg_datalen(uint16_t txoctets, uint16_t txtime);
int bletest_hci_le_rd_sugg_datalen(void);
int bletest_hci_rd_local_version(void);
int bletest_hci_rd_local_feat(void);
int bletest_hci_rd_local_supp_cmd(void);
int bletest_hci_le_read_supp_states(void);
int bletest_hci_le_rd_max_datalen(void);
int bletest_hci_le_read_rem_used_feat(uint16_t handle);
int bletest_hci_le_set_rand_addr(uint8_t *addr);
int bletest_hci_rd_rem_version(uint16_t handle);
int bletest_hci_le_set_host_chan_class(uint8_t *chanmap);
int bletest_hci_le_rd_chanmap(uint16_t handle);
int bletest_hci_le_set_event_mask(uint64_t event_mask);
int bletest_hci_set_event_mask(uint64_t event_mask);
int bletest_hci_le_add_to_whitelist(uint8_t *addr, uint8_t addr_type);
int bletest_hci_le_set_scan_enable(uint8_t enable, uint8_t filter_dups);
int bletest_hci_le_create_connection(struct hci_create_conn *hcc);
int bletest_hci_le_set_scan_params(uint8_t scan_type, uint16_t scan_itvl,
                                   uint16_t scan_window, uint8_t own_addr_type,
                                   uint8_t filter_policy);
int bletest_hci_le_add_resolv_list(uint8_t *local_irk, uint8_t *peer_irk,
                                   uint8_t *peer_ident_addr, uint8_t addr_type);
int bletest_hci_le_enable_resolv_list(uint8_t enable);

#if MYNEWT_VAL(BLE_ANDROID_MULTI_ADV_SUPPORT)
int bletest_hci_le_set_multi_rand_addr(uint8_t *addr, uint8_t instance);
int bletest_hci_le_set_multi_adv_data(uint8_t *data, uint8_t len,
                                      uint8_t instance);
int bletest_hci_le_set_multi_adv_params(struct hci_multi_adv_params *adv,
                                        uint8_t instance);
int bletest_hci_le_set_multi_adv_enable(uint8_t enable, uint8_t instance);
int bletest_hci_le_set_multi_scan_rsp_data(uint8_t *data, uint8_t len,
                                           uint8_t instance);
#else
int bletest_hci_le_set_adv_data(uint8_t *data, uint8_t len);
int bletest_hci_le_set_adv_params(struct hci_adv_params *adv);
int bletest_hci_le_set_adv_enable(uint8_t enable);
int bletest_hci_le_set_scan_rsp_data(uint8_t *data, uint8_t len);
#endif


#ifdef __cplusplus
}
#endif

#endif  /* H_BLETEST_PRIV_*/
