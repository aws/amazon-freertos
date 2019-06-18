/** @file wifi.c
 *
 *  @brief  This file provides WiFi Core API
 *
 *  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved.
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

#define INCLUDE_FROM_MLAN
#include <mlan_wmsdk.h>

#include <string.h>
#include <wifi.h>
#include <wmstdio.h>
#include <wm_os.h>
//#include <wm_net.h>
#include <pwrmgr.h>

#include "wifi-internal.h"
#include "wifi-sdio.h"

#define WIFI_COMMAND_RESPONSE_WAIT_MS 10000
#define WIFI_CORE_STACK_SIZE		(350)
#define WIFI_CORE_TASK_PRIORITY		(configMAX_PRIORITIES - 2)
/* We don't see events coming in quick succession,
 * MAX_EVENTS = 10 is fairly big value */
#define MAX_EVENTS 20
#define MAX_MCAST_LEN (MLAN_MAX_MULTICAST_LIST_SIZE * MLAN_MAC_ADDR_LENGTH)
#ifdef CONFIG_WiFi_878x
#define MAX_WAIT_TIME 20
#else
#define MAX_WAIT_TIME 35
#endif

static char wifi_init_done;
static char wifi_core_init_done;

#ifdef CONFIG_STA_AMPDU_TX
bool sta_ampdu_tx_enable = true;
#endif

#ifdef CONFIG_STA_AMPDU_RX
bool sta_ampdu_rx_enable = true;
#endif

/* Definitions for error constants. */
#define ERROR_INPROGRESS   -5    /* Operation in progress    */

static int retry_attempts;
wm_wifi_t wm_wifi;
static bool xfer_pending;

typedef enum __mlan_status {
	MLAN_CARD_NOT_DETECTED = 3,
	MLAN_STATUS_FW_DNLD_FAILED,
	MLAN_STATUS_FW_NOT_DETECTED,
	MLAN_STATUS_FW_NOT_READY,
	MLAN_STATUS_FW_XZ_FAILED,
	MLAN_CARD_CMD_TIMEOUT
} __mlan_status;


static os_thread_stack_define(wifi_core_stack, WIFI_CORE_STACK_SIZE *
				sizeof(portSTACK_TYPE));
static os_thread_stack_define(wifi_drv_stack, 1024);
static os_queue_pool_define(g_io_events_queue_data,
			    sizeof(struct bus_message) * MAX_EVENTS);

int wifi_set_mac_multicast_addr(const char *mlist, uint32_t num_of_addr);
int wrapper_get_wpa_ie_in_assoc(uint8_t *wpa_ie);
KEY_TYPE_ID get_sec_info();
mlan_status wlan_process_int_status(void *pmadapter);
//void handle_data_packet(t_u8 interface, t_u8 *rcvdata, t_u16 datalen);

unsigned wifi_get_last_cmd_sent_ms()
{
	return wm_wifi.last_sent_cmd_msec;
}

uint32_t wifi_get_value1()
{
	return wifi_get_device_value1();
}

/* When Wi-Fi card is in IEEE PS and sleeping
 * CMD or Data cannot be transmited.
 * The card must be woken up.
 * So data or command trasnfer is temporarily kept
 * in pending state. This function returns value
 * of pending flag true/false.
 */
bool wifi_get_xfer_pending()
{
	return xfer_pending;
}
/*
 * This function sets the flag value
 */
void wifi_set_xfer_pending(bool xfer_val)
{
	xfer_pending = xfer_val;
}


void wifi_update_last_cmd_sent_ms()
{
	wm_wifi.last_sent_cmd_msec = os_ticks_to_msec(os_ticks_get());
}

static int wifi_get_command_resp_sem(unsigned long wait)
{
	wifi_d("Get Command resp sem");
	return os_semaphore_get(&wm_wifi.command_resp_sem, wait);
}

static int wifi_put_command_resp_sem(void)
{
	wifi_d("Put Command resp sem");
	return os_semaphore_put(&wm_wifi.command_resp_sem);
}

#define WL_ID_WIFI_CMD		"wifi_cmd"

int wifi_get_command_lock()
{
	wifi_d("Get Command lock");
	int rv = wakelock_get(WL_ID_WIFI_CMD);
	if (rv == WM_SUCCESS)
		rv = os_mutex_get(&wm_wifi.command_lock, OS_WAIT_FOREVER);

	return rv;
}

int wifi_get_mcastf_lock(void)
{
	wifi_d("Get Command lock");
	return os_mutex_get(&wm_wifi.mcastf_mutex, OS_WAIT_FOREVER);
}

int wifi_put_mcastf_lock(void)
{
	wifi_d("Put Command lock");
	return os_mutex_put(&wm_wifi.mcastf_mutex);
}

int wifi_put_command_lock()
{
	int rv = WM_SUCCESS;
	wifi_d("Put Command lock");
	rv = wakelock_put(WL_ID_WIFI_CMD);
	if (rv == WM_SUCCESS)
		rv = os_mutex_put(&wm_wifi.command_lock);

	return rv;
}

int wifi_wait_for_cmdresp(void *cmd_resp_priv)
{
	int ret;
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();
	int buf_len = MLAN_SDIO_BLOCK_SIZE;

#if defined(CONFIG_ENABLE_WARNING_LOGS) || defined(CONFIG_WIFI_CMD_RESP_DEBUG)

	wcmdr_d("[%-8u] CMD --- : 0x%-2x Size: %-4d Seq: %-2d",
		os_get_timestamp(),
		cmd->command, cmd->size, cmd->seq_num);
#endif /* CONFIG_ENABLE_WARNING_LOGS || CONFIG_WIFI_CMD_RESP_DEBUG*/

	if (cmd->size > MLAN_SDIO_BLOCK_SIZE) {

		buf_len = 2 * MLAN_SDIO_BLOCK_SIZE;

		if (cmd->size > 2 * MLAN_SDIO_BLOCK_SIZE) {
			/*
			 * This is a error added to be flagged during
			 * development cycle. It is not expected to
			 * occur in production. The legacy code below
			 * only sents out MLAN_SDIO_BLOCK_SIZE or 2 *
			 * MLAN_SDIO_BLOCK_SIZE sized packet. If ever
			 * in future greater packet size generated then
			 * this error will help to localize the problem.
			 */
			wifi_e("cmd size greater than twice of "
				"MLAN_SDIO_BLOCK_SIZE");
			return -WM_FAIL;
		}
	}

	/*
	 * This is the private pointer. Only the command response handler
	 * function knows what it means or where it points to. It can be
	 * NULL.
	 */
	wm_wifi.cmd_resp_priv = cmd_resp_priv;

	wifi_send_cmdbuffer(buf_len);
	/* Wait max 10 sec for the command response */
	ret = wifi_get_command_resp_sem(WIFI_COMMAND_RESPONSE_WAIT_MS);
	if (ret == WM_SUCCESS) {
		wifi_d("Got the command resp lock");
	} else {
#ifdef CONFIG_ENABLE_WARNING_LOGS
		wifi_w("Command response timed out. command = 0x%x",
		       cmd->command);
#endif /* CONFIG_ENABLE_WARNING_LOGS */
	}

	wm_wifi.cmd_resp_priv = NULL;
	wifi_put_command_lock();
	return ret;
}

#ifdef CONFIG_P2P
void wifi_wfd_event(bool peer_event, bool action_frame, void *data)
{
	struct wifi_wfd_event event;

	if (wm_wifi.wfd_event_queue) {
		event.peer_event = peer_event;
		event.action_frame = action_frame;
		event.data = data;
		os_queue_send(wm_wifi.wfd_event_queue, &event, OS_NO_WAIT);
	}
}
#endif

void wifi_event_completion(int event, enum wifi_event_reason result, void *data)
{
	struct wifi_message msg;
	if (!wm_wifi.wlc_mgr_event_queue) {
		wifi_d("No queue is registered. Ignoring");
		return;
	}

	msg.data = data;
	msg.reason = result;
	msg.event = event;
	os_queue_send(wm_wifi.wlc_mgr_event_queue, &msg, OS_NO_WAIT);
}

static int cmp_mac_addr(uint8_t *mac_addr1, uint8_t *mac_addr2)
{
	int i = 0;

	if (!mac_addr1 || !mac_addr2)
		return 1;

	for (i = 0; i < MLAN_MAC_ADDR_LENGTH; i++)
		if (mac_addr1[i] != mac_addr2[i])
			return 1;
	return 0;
}

static int add_mcast_ip(uint8_t *mac_addr)
{
	mcast_filter *node_t, *new_node;
	wifi_get_mcastf_lock();
	node_t = wm_wifi.start_list;
	if (wm_wifi.start_list == NULL) {
		new_node = os_mem_alloc(sizeof(mcast_filter));
		if (new_node == NULL) {
			wifi_put_mcastf_lock();
			return -WM_FAIL;
		}
		memcpy(new_node->mac_addr, mac_addr, MLAN_MAC_ADDR_LENGTH);
		new_node->next = NULL;
		wm_wifi.start_list = new_node;
		wifi_put_mcastf_lock();
		return WM_SUCCESS;
	}
	while (node_t->next != NULL && cmp_mac_addr(node_t->mac_addr, mac_addr))
		node_t = node_t->next;

	if (!cmp_mac_addr(node_t->mac_addr, mac_addr)) {
		wifi_put_mcastf_lock();
		return -WM_FAIL;
	}
	new_node = os_mem_alloc(sizeof(mcast_filter));
	if (new_node == NULL) {
		wifi_put_mcastf_lock();
		return -WM_FAIL;
	}
	memcpy(new_node->mac_addr, mac_addr, MLAN_MAC_ADDR_LENGTH);
	new_node->next = NULL;
	node_t->next = new_node;
	wifi_put_mcastf_lock();
	return WM_SUCCESS;
}

static int remove_mcast_ip(uint8_t *mac_addr)
{
	mcast_filter *curr_node, *prev_node;
	wifi_get_mcastf_lock();
	curr_node = wm_wifi.start_list->next;
	prev_node = wm_wifi.start_list;
	if (wm_wifi.start_list == NULL) {
		wifi_put_mcastf_lock();
		return -WM_FAIL;
	}
	if (curr_node == NULL && cmp_mac_addr(prev_node->mac_addr, mac_addr)) {
		os_mem_free(prev_node);
		wm_wifi.start_list = NULL;
		wifi_put_mcastf_lock();
		return WM_SUCCESS;
	}
	/* If search element is at first location */
	if (!cmp_mac_addr(prev_node->mac_addr, mac_addr)) {
		wm_wifi.start_list = prev_node->next;
		os_mem_free(prev_node);
		wifi_put_mcastf_lock();
		return WM_SUCCESS;
	}
	/* Find node in linked list */
	while (cmp_mac_addr(curr_node->mac_addr, mac_addr)
	       && curr_node->next != NULL) {
		prev_node = curr_node;
		curr_node = curr_node->next;
	}
	if (!cmp_mac_addr(curr_node->mac_addr, mac_addr)) {
		prev_node->next = curr_node->next;
		os_mem_free(curr_node);
		wifi_put_mcastf_lock();
		return WM_SUCCESS;
	}
	wifi_put_mcastf_lock();
	return -WM_FAIL;
}

static int make_filter_list(char *mlist, int maxlen)
{
	mcast_filter *node_t;
	uint8_t maddr_cnt = 0;
	wifi_get_mcastf_lock();
	node_t = wm_wifi.start_list;
	while (node_t != NULL) {
		memcpy(mlist, node_t->mac_addr, MLAN_MAC_ADDR_LENGTH);
		node_t = (struct mcast_filter *)node_t->next;
		mlist = mlist + MLAN_MAC_ADDR_LENGTH;
		maddr_cnt++;
		if (maddr_cnt > (maxlen / 6))
			break;
	}
	wifi_put_mcastf_lock();
	return maddr_cnt;
}

void wifi_get_ipv4_multicast_mac(uint32_t ipaddr, uint8_t *mac_addr)
{
	int i = 0, j = 0;
	uint32_t mac_addr_r = 0x01005E;
	ipaddr = ipaddr & 0x7FFFFF;
	/* Generate Multicast Mapped Mac Address for IPv4
	 * To get Multicast Mapped MAC address,
	 * To calculate 6 byte Multicast-Mapped MAC Address.
	 * 1) Fill higher 24-bits with IANA Multicast OUI (01-00-5E)
	 * 2) Set 24th bit as Zero
	 * 3) Fill lower 23-bits with from IP address (ignoring higher
	 * 9bits).
	 */
	for (i = 2; i >= 0; i--, j++)
		mac_addr[j] = (char)(mac_addr_r >> 8 * i) & 0xFF;

	for (i = 2; i >= 0; i--, j++)
		mac_addr[j] = (char)(ipaddr >> 8 * i) & 0xFF;
}

#ifdef CONFIG_IPV6
void wifi_get_ipv6_multicast_mac(uint32_t ipaddr, uint8_t *mac_addr)
{
	int i = 0, j = 0;
	uint32_t mac_addr_r = 0x3333;
	/* Generate Multicast Mapped Mac Address for IPv6
	 * To get Multicast Mapped MAC address,
	 * To calculate 6 byte Multicast-Mapped MAC Address.
	 * 1) Fill higher 16-bits with IANA Multicast OUI (33-33)
	 * 2) Fill lower 24-bits with from IP address
	 */
	for (i = 1; i >= 0; i--, j++)
		mac_addr[j] = (char)(mac_addr_r >> 8 * i) & 0xFF;

	for (i = 3; i >= 0; i--, j++)
		mac_addr[j] = (char)(ipaddr >> 8 * i) & 0xFF;
}
#endif /* CONFIG_IPV6 */

int wifi_add_mcast_filter(uint8_t *mac_addr)
{
	char mlist[MAX_MCAST_LEN];
	int len, ret;
	/* If MAC address is 00:11:22:33:44:55,
	 * then pass mac_addr array in following format:
	 * mac_addr[0] = 00
	 * mac_addr[1] = 11
	 * mac_addr[2] = 22
	 * mac_addr[3] = 33
	 * mac_addr[4] = 44
	 * mac_addr[5] = 55
	 */
	ret = add_mcast_ip(mac_addr);
	if (ret != WM_SUCCESS)
		return ret;
	len = make_filter_list(mlist, MAX_MCAST_LEN);
	return wifi_set_mac_multicast_addr(mlist, len);
}

int wifi_remove_mcast_filter(uint8_t *mac_addr)
{
	char mlist[MAX_MCAST_LEN];
	int len, ret;
	/* If MAC address is 00:11:22:33:44:55,
	 * then pass mac_addr array in following format:
	 * mac_addr[0] = 00
	 * mac_addr[1] = 11
	 * mac_addr[2] = 22
	 * mac_addr[3] = 33
	 * mac_addr[4] = 44
	 * mac_addr[5] = 55
	 */
	ret = remove_mcast_ip(mac_addr);
	if (ret != WM_SUCCESS)
		return ret;
	len = make_filter_list(mlist, MAX_MCAST_LEN);
	ret = wifi_set_mac_multicast_addr(mlist, len);
	return ret;
}

/* Since we do not have the descriptor list we will using this adaptor function */
int wrapper_bssdesc_first_set(int bss_index,
			      uint8_t *BssId,
			      bool *is_ibss_bit_set,
			      int *ssid_len, uint8_t *ssid,
			      uint8_t *Channel,
			      uint8_t *RSSI,
			      _SecurityMode_t *WPA_WPA2_WEP,
			      _Cipher_t *wpa_mcstCipher,
			      _Cipher_t *wpa_ucstCipher,
			      _Cipher_t *rsn_mcstCipher,
			      _Cipher_t *rsn_ucstCipher,
			      bool *is_pmf_required);

int wrapper_bssdesc_second_set(int bss_index,
			       bool *phtcap_ie_present,
			       bool *phtinfo_ie_present,
			       bool *wmm_ie_present,
			       uint8_t *band,
			       bool *wps_IE_exist,
			       uint16_t *wps_session, bool *wpa2_entp_IE_exist,
			       uint8_t *trans_mode, uint8_t *trans_bssid,
			       int *trans_ssid_len, uint8_t *trans_ssid);

static struct wifi_scan_result common_desc;
int wifi_get_scan_result(unsigned int index, struct wifi_scan_result **desc)
{
	memset(&common_desc, 0x00, sizeof(struct wifi_scan_result));
	int rv = wrapper_bssdesc_first_set(index,
					   common_desc.bssid,
					   &common_desc.is_ibss_bit_set,
					   &common_desc.ssid_len,
					   common_desc.ssid,
					   &common_desc.Channel,
					   &common_desc.RSSI,
					   &common_desc.WPA_WPA2_WEP,
					   &common_desc.wpa_mcstCipher,
					   &common_desc.wpa_ucstCipher,
					   &common_desc.rsn_mcstCipher,
					   &common_desc.rsn_ucstCipher,
					   &common_desc.is_pmf_required);
	if (rv != WM_SUCCESS) {
		wifi_e("wifi_get_scan_result failed");
		return rv;
	}

	/* Country info not populated */
	rv = wrapper_bssdesc_second_set(index,
					&common_desc.phtcap_ie_present,
					&common_desc.phtinfo_ie_present,
					&common_desc.wmm_ie_present,
					&common_desc.band,
					&common_desc.wps_IE_exist,
					&common_desc.wps_session,
					&common_desc.wpa2_entp_IE_exist,
					&common_desc.trans_mode,
					common_desc.trans_bssid,
					&common_desc.trans_ssid_len,
					common_desc.trans_ssid);

	if (rv != WM_SUCCESS) {
		wifi_e("wifi_get_scan_result failed");
		return rv;
	}

	*desc = &common_desc;

	return WM_SUCCESS;
}

int wifi_register_event_queue(os_queue_t *event_queue)
{
	if (!event_queue)
		return -WM_E_INVAL;

	if (wm_wifi.wlc_mgr_event_queue)
		return -WM_FAIL;

	wm_wifi.wlc_mgr_event_queue = event_queue;
	return WM_SUCCESS;
}

int wifi_unregister_event_queue(os_queue_t * event_queue)
{
	if (!wm_wifi.wlc_mgr_event_queue
	    || wm_wifi.wlc_mgr_event_queue != event_queue)
		return -WM_FAIL;

	wm_wifi.wlc_mgr_event_queue = NULL;
	return WM_SUCCESS;
}

#ifdef CONFIG_P2P
int wifi_register_wfd_event_queue(os_queue_t *event_queue)
{

	if (wm_wifi.wfd_event_queue)
		return -WM_FAIL;

	wm_wifi.wfd_event_queue = event_queue;
	return WM_SUCCESS;
}

int wifi_unregister_wfd_event_queue(os_queue_t *event_queue)
{
	if (!wm_wifi.wfd_event_queue || wm_wifi.wfd_event_queue != event_queue)
		return -WM_FAIL;

	wm_wifi.wfd_event_queue = NULL;
	return WM_SUCCESS;
}
#endif

int wifi_get_wpa_ie_in_assoc(uint8_t *wpa_ie)
{
	return wrapper_get_wpa_ie_in_assoc(wpa_ie);
}

#define WL_ID_WIFI_MAIN_LOOP	"wifi_main_loop"

static void wifi_driver_main_loop(void *argv)
{
	int ret;
	struct bus_message msg;

	/* Main Loop */
	while (1) {
		wifi_d("wifi driver : waiting on Q\r\n");
		ret = os_queue_recv(&wm_wifi.io_events, &msg, OS_WAIT_FOREVER);
		wifi_d("wifi driver : got from Q\r\n");

		if (ret == WM_SUCCESS) {
			wakelock_get(WL_ID_WIFI_MAIN_LOOP);

			wifi_d("wifi driver : got lock\r\n");

			if (msg.event == MLAN_TYPE_EVENT) {
				wifi_handle_fw_event(&msg);
				/*
				 * Free the buffer after the event is
				 * handled.
				 */
				wifi_free_eventbuf(msg.data);
			} else if (msg.event == MLAN_TYPE_CMD) {
				wifi_process_cmd_response(
					(HostCmd_DS_COMMAND *)(
					(uint8_t *) msg.data
					+ INTF_HEADER_LEN));
				wifi_update_last_cmd_sent_ms();
				wifi_put_command_resp_sem();
			}

			wakelock_put(WL_ID_WIFI_MAIN_LOOP);
		}
	}
}

#define WL_ID_WIFI_CORE_INPUT	"wifi_core_input"
/**
 * This function should be called when a packet is ready to be read
 * from the interface.
 */
static void wifi_core_input()
{
	int sta;

	for (;;) {
		sta = os_enter_critical_section();
		/* Allow interrupt handler to deliver us a packet */
		g_txrx_flag = true;
		SDIOC_IntMask(SDIOC_INT_CDINT, UNMASK);
		SDIOC_IntSigMask(SDIOC_INT_CDINT, UNMASK);
		os_exit_critical_section(sta);

		/* Wait till we receive a packet from SDIO */
		os_event_notify_get(OS_WAIT_FOREVER);
		wakelock_get(WL_ID_WIFI_CORE_INPUT);

		/* Protect the SDIO from other parallel activities */
		wifi_sdio_lock();

		wlan_process_int_status(mlan_adap);

		wifi_sdio_unlock();
		wakelock_put(WL_ID_WIFI_CORE_INPUT);
	}			/* for ;; */
}

static void wifi_core_deinit();
static int wifi_low_level_input(const uint8_t interface, const uint8_t *buffer,
				const uint16_t len);

static int wifi_core_init(void)
{
	int ret;

	if (wifi_core_init_done)
		return WM_SUCCESS;

	ret = os_mutex_create(&wm_wifi.command_lock,
				"command lock", OS_MUTEX_INHERIT);

	if (ret != WM_SUCCESS) {
		wifi_e("Create command_lock failed");
		goto fail;
	}

	ret = os_semaphore_create(&wm_wifi.command_resp_sem,
				"command resp sem");

	if (ret != WM_SUCCESS) {
		wifi_e("Create command resp sem failed");
		goto fail;
	}

	ret = os_mutex_create(&wm_wifi.mcastf_mutex, "mcastf-mutex",
				OS_MUTEX_INHERIT);
	if (ret != WM_SUCCESS) {
		wifi_e("Create mcastf mutex failed");
		goto fail;
	}

	/*
	 * Take the cmd resp lock immediately so that we can later block on
	 * it.
	 */
	wifi_get_command_resp_sem(OS_WAIT_FOREVER);
	wm_wifi.io_events_queue_data = g_io_events_queue_data;

	ret = os_queue_create(&wm_wifi.io_events, "io-events",
				  sizeof(struct bus_message),
				  &wm_wifi.io_events_queue_data);
	if (ret != WM_SUCCESS) {
		wifi_e("Create io events queue failed");
		goto fail;
	}

	ret = bus_register_event_queue(&wm_wifi.io_events);
	if (ret != WM_SUCCESS) {
		wifi_e("Register io events queue failed");
		goto fail;
	}

	ret = os_thread_create(&wm_wifi.wm_wifi_main_thread,
			       "wifi_driver",
			       wifi_driver_main_loop, NULL,
				&wifi_drv_stack, OS_PRIO_1);
	if (ret !=  WM_SUCCESS) {
		wifi_e("Create wifi driver thread failed");
		goto fail;
	}

	ret = bus_register_data_input_function(&wifi_low_level_input);
	if (ret != WM_SUCCESS) {
		wifi_e("Register wifi low level input failed");
		goto fail;
	}

	ret = os_thread_create(&wm_wifi.wm_wifi_core_thread,
			"stack_dispatcher",
			(void (*)(os_thread_arg_t)) wifi_core_input,
			NULL,
			&wifi_core_stack, WIFI_CORE_TASK_PRIORITY);

	if (ret != WM_SUCCESS) {
		wifi_e("Create stack dispatcher thread failed");
		goto fail;
	}

	wifi_core_thread = wm_wifi.wm_wifi_core_thread;
	wifi_core_init_done = 1;

	return WM_SUCCESS;

fail:

	wifi_core_deinit();

	return -WM_FAIL;
}

static void wifi_core_deinit()
{

	wifi_core_init_done = 0;

	bus_deregister_event_queue();
	bus_deregister_data_input_funtion();

	if (wm_wifi.io_events) {
		os_queue_delete(&wm_wifi.io_events);
		wm_wifi.io_events = NULL;
	}
	if (wm_wifi.mcastf_mutex) {
		os_mutex_delete(&wm_wifi.mcastf_mutex);
		wm_wifi.mcastf_mutex = NULL;
	}
	if (wm_wifi.command_resp_sem) {
		os_semaphore_delete(&wm_wifi.command_resp_sem);
		wm_wifi.command_resp_sem = NULL;
	}
	if (wm_wifi.command_lock) {
		os_mutex_delete(&wm_wifi.command_lock);
		wm_wifi.command_lock = NULL;
	}
	if (wm_wifi.wm_wifi_main_thread) {
		os_thread_delete(&wm_wifi.wm_wifi_main_thread);
		wm_wifi.wm_wifi_main_thread = NULL;
	}
	if (wm_wifi.wm_wifi_core_thread) {
		os_thread_delete(&wm_wifi.wm_wifi_core_thread);
		wm_wifi.wm_wifi_core_thread = NULL;
		wifi_core_thread = NULL;
	}
}

int wifi_init(const flash_desc_t *fl)
{
	static flash_desc_t saved_fl;

	if (wifi_init_done)
		return WM_SUCCESS;

	if (fl)
		saved_fl = *fl;

	int ret = sd_wifi_init(WLAN_TYPE_NORMAL, WLAN_FW_IN_FLASH,
				&saved_fl, NULL);
	if (ret) {
		wifi_e("sd_wifi_init failed. status code %d", ret);
		switch (ret) {
		case MLAN_CARD_CMD_TIMEOUT:
		case MLAN_CARD_NOT_DETECTED:
			ret = -WIFI_ERROR_CARD_NOT_DETECTED;
			break;
		case MLAN_STATUS_FW_DNLD_FAILED:
			ret = -WIFI_ERROR_FW_DNLD_FAILED;
			break;
		case MLAN_STATUS_FW_NOT_DETECTED:
			ret = -WIFI_ERROR_FW_NOT_DETECTED;
			break;
		case MLAN_STATUS_FW_XZ_FAILED:
			ret = -WIFI_ERROR_FW_XZ_FAILED;
			break;
		case MLAN_STATUS_FW_NOT_READY:
			ret = -WIFI_ERROR_FW_NOT_READY;
			break;
		}
		return ret;
	}

	ret = wifi_core_init();
	if (ret) {
		wifi_e("wifi_core_init failed. status code %d", ret);
	}

	if (ret == WM_SUCCESS)
		wifi_init_done = 1;

	return ret;
}

void wifi_deinit(void)
{
	wifi_init_done = 0;

	sd_wifi_deinit();
	wifi_core_deinit();
}

void wifi_set_packet_retry_count(const int count)
{
	retry_attempts = count;
}

#ifdef CONFIG_STA_AMPDU_TX
void wifi_sta_ampdu_tx_enable(void)
{
	sta_ampdu_tx_enable = true;
}

void wifi_sta_ampdu_tx_disable(void)
{
	sta_ampdu_tx_enable = false;
}
#else
void wifi_sta_ampdu_tx_enable(void)
{
}

void wifi_sta_ampdu_tx_disable(void)
{
}
#endif /* CONFIG_STA_AMPDU_TX */

#ifdef CONFIG_STA_AMPDU_RX
void wifi_sta_ampdu_rx_enable(void)
{
	sta_ampdu_rx_enable = true;
}

void wifi_sta_ampdu_rx_disable(void)
{
	sta_ampdu_rx_enable = false;
}
#else
void wifi_sta_ampdu_rx_enable(void)
{
}

void wifi_sta_ampdu_rx_disable(void)
{
}
#endif /* CONFIG_STA_AMPDU_RX */

int wifi_register_data_input_callback(void (*data_intput_callback)
				(const uint8_t interface, const uint8_t *buffer,
				const uint16_t len))
{
	if (wm_wifi.data_intput_callback)
		return -WM_FAIL;

	wm_wifi.data_intput_callback = data_intput_callback;

	return WM_SUCCESS;
}

void wifi_deregister_data_input_callback()
{
	wm_wifi.data_intput_callback = NULL;
}

static int wifi_low_level_input(const uint8_t interface, const uint8_t *buffer,
				const uint16_t len)
{
	if (wm_wifi.data_intput_callback) {
		wm_wifi.data_intput_callback(interface, buffer, len);
		return WM_SUCCESS;
	}

	return -WM_FAIL;
}

#define WL_ID_LL_OUTPUT		"wifi_low_level_output"

int wifi_low_level_output(const uint8_t interface, const uint8_t *buffer,
				const uint16_t len)
{
	int i, ret, retry = retry_attempts;
	u32_t pkt_len;

	wakelock_get(WL_ID_LL_OUTPUT);
	ret = os_rwlock_read_lock(&ps_rwlock, MAX_WAIT_TIME);
	if (ret != WM_SUCCESS) {
		wakelock_put(WL_ID_LL_OUTPUT);
		return ERROR_INPROGRESS;
	}

#ifdef CONFIG_STA_AMPDU_TX
	if (interface == BSS_TYPE_STA && sta_ampdu_tx_enable) {
		if (net_is_ip_or_ipv6(buffer))
			wrapper_wlan_sta_ampdu_enable();
	}
#endif

#ifdef CONFIG_UAP_AMPDU_TX
	if (interface == BSS_TYPE_UAP) {
		if (net_is_ip_or_ipv6(buffer))
			wrapper_wlan_upa_ampdu_enable((uint8_t *) buffer);
	}
#endif
	wifi_sdio_lock();

	pkt_len = sizeof(TxPD) + INTF_HEADER_LEN;

retry_xmit:
	i = wlan_xmit_pkt(pkt_len + len, interface);
	wifi_sdio_unlock();
	if (i != MLAN_STATUS_SUCCESS) {
		if (i == MLAN_STATUS_FAILURE) {
			ret = -WM_E_NOMEM;
			goto exit_fn;
		} else if (i == MLAN_STATUS_RESOURCE) {
			if (!retry) {
				ret = -WM_E_BUSY;
				goto exit_fn;
			} else {
				retry--;
				/* Allow the other thread to run and hence
				* update the write bitmap so that pkt
				* can be sent to FW */
				os_thread_sleep(1);
				wifi_sdio_lock();
				goto retry_xmit;
			}
		}
	}

	ret = WM_SUCCESS;

exit_fn:
	os_rwlock_read_unlock(&ps_rwlock);
	wifi_set_xfer_pending(false);
	wakelock_put(WL_ID_LL_OUTPUT);

	return ret;
}
