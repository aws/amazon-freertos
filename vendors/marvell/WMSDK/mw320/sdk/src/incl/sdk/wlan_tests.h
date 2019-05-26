/*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
*
*  MARVELL CONFIDENTIAL
*  The source code contained or described herein and all documents related to
*  the source code ("Material") are owned by Marvell International Ltd or its
*  suppliers or licensors. Title to the Material remains with Marvell
*  International Ltd or its suppliers and licensors. The Material contains
*  trade secrets and proprietary and confidential information of Marvell or its
*  suppliers and licensors. The Material is protected by worldwide copyright
*  and trade secret laws and treaty provisions. No part of the Material may be
*  used, copied, reproduced, modified, published, uploaded, posted,
*  transmitted, distributed, or disclosed in any way without Marvell's prior
*  express written permission.
*
*  No license under any patent, copyright, trade secret or other intellectual
*  property right is granted to or conferred upon you by disclosure or delivery
*  of the Materials, either expressly, by implication, inducement, estoppel or
*  otherwise. Any license under such intellectual property rights must be
*  express and approved by Marvell in writing.
*
*/

/*! \file wlan_tests.h
 *  \brief WLAN Connection Manager Tests
 */

#ifndef __WLAN_TESTS_H__
#define __WLAN_TESTS_H__

void test_wlan_scan(int argc, char **argv);
void test_wlan_disconnect(int argc, char **argv);
int get_security(int argc, char **argv, enum wlan_security_type type,
			struct wlan_network_security *sec);
void cmd_wlan_get_rf_channel(int argc, char **argv);
void cmd_wlan_set_rf_channel(int argc, char **argv);
void cmd_wlan_get_tx_power(int argc, char **argv);
void cmd_wlan_deepsleep(int argc, char **argv);
void cmd_wlan_pscfg(int argc, char **argv);
void print_trpc(chan_trpc_t chan_trpc);
#endif /* WLAN_TESTS_H */

