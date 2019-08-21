/** @file mlan_glue.c
 *
 *  @brief This file acts as a glue between legacy wlan code and mlan based wlan
 *  code
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

#include <mlan_wmsdk.h>

/* Additional WMSDK header files */
#include <wmstdio.h>
#include <wmassert.h>
#include <wmerrno.h>
#include <wm_os.h>
#include <wm_net.h>
#include <wifi_events.h>
//#include "lwip/pbuf.h"
#include <wifi-debug.h>

#include <wifi.h>
#include "wifi-internal.h"
#include "wifi-sdio.h"
#include <compat_attribute.h>

/* fixme: remove this as soon as there is no dependancy */
#define INCLUDE_FROM_MLAN
#include <wlan.h>

#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"

const uint8_t wpa2_akmp_oui[4] = { 0x00, 0x0f, 0xac, 0x01 };

#ifdef CONFIG_WiFi_8801
bool low_power_mode;
#endif
bool ed_mac_mode;
short ed_mac_offset;
bool cal_data_valid;
uint8_t *cal_data;
uint32_t cal_data_len;

bool mac_addr_valid;
uint8_t *mac_addr;
uint8_t trpc_country;
#ifdef CONFIG_WPA2_ENTP
bool scan_enable_wpa2_enterprise_ap_only;
#endif
/* fixme: This structure seems to have been removed from mlan. This was
   copied from userif_ext.h file temporarily. Change the handling of events to
   make it similar to what mlan does */
/** Event structure */
typedef MLAN_PACK_START struct _Event_Ext_t
{
	/** No of bytes in packet including this field */
	uint16_t length;
	/** Type: Event (3) */
	uint16_t type;
	/** Event ID */
	uint16_t event_id;
	/** BSS index number for multiple BSS support */
	uint8_t bss_index;
	/** BSS type */
	uint8_t bss_type;
	/** Reason code */
	uint16_t reason_code;
	/** Source MAC address */
	uint8_t src_mac_addr[MLAN_MAC_ADDR_LENGTH];
} MLAN_PACK_END Event_Ext_t;

/*
 *  fixme: This entry is not present in mlan_fw.h. It is likely possible
 *  that this code is no-op in the firmware. Keeping it as it is for
 *  maintaining backward compatibility. Can be removed later.
 */
#define host_CMD_802_11_AUTHENTICATE 0x0011

/* Following is allocated in mlan_register */
mlan_adapter *mlan_adap;

mlan_device mlan_dev;

uint16_t g_data_nf_last;
uint16_t g_data_snr_last;

uint8_t g_rssi;
int16_t g_bcn_nf_last;

/* fixme: This global variable is needed
 *  to save the correct event since SLP_CFRM command
 *  returns incorrect bitmap. This will be  further investigated
 *  and if possile global variable will be removed.*/
static t_u16 ps_event;

#ifdef CONFIG_11N
//track whether ampdu is enabled
static t_u8 ampdu_status_flag = MFALSE;
#endif

//void deliver_packet_above(struct pbuf *p, int recv_interface);
struct pbuf *gen_pbuf_from_data(t_u8 *payload, t_u16 datalen);

#ifdef CONFIG_11N
/*
 * The command event received from the firmware (e.g. EVENT_ADDBA) cannot
 * be processed immediately. We need to do it the WLC manager context. For
 * that we will save the buffer lest some other event destroy it. WLC
 * manager will be notified of this event.
 *
 * Returns NULL if buffer allocation failed.
 */
static void *wifi_11n_save_request(Event_Ext_t *evt)
{
	void *dot11n_eventbuf = os_mem_alloc(evt->length);

	if (!dot11n_eventbuf) {
		wifi_w("11n eventbuf alloc failed %d", evt->length);
		return NULL;
	}

	return memcpy(dot11n_eventbuf, evt, evt->length);
}
#endif /* CONFIG_11N */

bool mlan_is_amsdu(const t_u8 *rcvdata) // abhi
{
	RxPD *rxpd = (RxPD *) ((t_u8 *) rcvdata + INTF_HEADER_LEN);

	if (rxpd->rx_pkt_type == PKT_TYPE_AMSDU){
		return true;
	}
	else
		return false;
}

t_u8 *mlan_get_payload(const t_u8 *rcvdata, t_u16 *payload_len, int *interface)
{
	RxPD *rxpd = (RxPD *) ((t_u8 *) rcvdata + INTF_HEADER_LEN);
	int recv_interface = rxpd->bss_type;
	t_u8 *payload = (t_u8 *) rxpd + rxpd->rx_pkt_offset;

	*interface = recv_interface;
	*payload_len = rxpd->rx_pkt_length;

	 if (recv_interface == MLAN_BSS_TYPE_STA ||
		recv_interface == MLAN_BSS_TYPE_UAP) {
		g_data_nf_last = rxpd->nf;
		g_data_snr_last = rxpd->snr;
	}

	return payload;
}

void wrapper_deliver_amsdu_subframe(pmlan_buffer amsdu_pmbuf,
			       t_u8 *data, t_u16 pkt_len)
{
	NetworkBufferDescriptor_t *pxNetworkBuffer;
	IPStackEvent_t xRxEvent = { eNetworkRxEvent, NULL };
	const TickType_t xDescriptorWaitTime = pdMS_TO_TICKS( 250 );

	pxNetworkBuffer = pxGetNetworkBufferWithDescriptor(pkt_len, xDescriptorWaitTime);

	if (pxNetworkBuffer != NULL) {
		pxNetworkBuffer->xDataLength = pkt_len;
		memcpy(pxNetworkBuffer->pucEthernetBuffer, data, pkt_len);
		xRxEvent.pvData = (void *) pxNetworkBuffer;

		if ( xSendEventStructToIPTask( &xRxEvent, xDescriptorWaitTime) == pdFAIL ) {
			vReleaseNetworkBufferAndDescriptor(pxNetworkBuffer);
			return;
		}
		return;
	}
}

static mlan_status wrapper_moal_recv_packet(IN t_void * pmoal_handle,
				     IN pmlan_buffer pmbuf)
{
	NetworkBufferDescriptor_t *pxNetworkBuffer;
	IPStackEvent_t xRxEvent = { eNetworkRxEvent, NULL };
	const TickType_t xDescriptorWaitTime = pdMS_TO_TICKS( 250 );
	t_u8 *data;
	int pkt_len;

	ASSERT(pmbuf != NULL);

	if (pmbuf->data_offset != 0) {
		wifi_e("pmbuf->data_offset != 0 (%d)?", pmbuf->data_offset);
		wmpanic();
	}

	data = (t_u8 *) (pmbuf->pbuf + pmbuf->data_offset);
	pkt_len = pmbuf->data_len;

        pxNetworkBuffer = pxGetNetworkBufferWithDescriptor(pkt_len, xDescriptorWaitTime);

        if (pxNetworkBuffer != NULL) {
                pxNetworkBuffer->xDataLength = pkt_len;
                memcpy(pxNetworkBuffer->pucEthernetBuffer, data, pkt_len);
                xRxEvent.pvData = (void *) pxNetworkBuffer;

                if ( xSendEventStructToIPTask( &xRxEvent, xDescriptorWaitTime) == pdFAIL ) {
                        vReleaseNetworkBufferAndDescriptor(pxNetworkBuffer);
                }
        }

	os_mem_free(pmbuf->pbuf);
	os_mem_free(pmbuf);
	return MLAN_STATUS_SUCCESS;
}

/** moal_init_timer*/
static mlan_status wrapper_moal_init_timer(IN t_void *pmoal_handle
				    __UNUSED__,
				    OUT t_void ** pptimer,
				    IN t_void(*callback) (t_void * pcontext),
				    IN t_void * pcontext)
{
	w_tmr_d("Creating timer");

	/*
	 * As can be seen from the arguments to our function, we do not
	 * have a timer period parameter. Nor do we have a parameter to
	 * specify whether the user wants a periodic or a one-shot
	 * timer. We have to wait for these paramters till the next call to
	 * wrapper_moal_start_timer.
	 *
	 * But a timer object cannot be created in
	 * wrapper_moal_start_timer. Because the timer handle is passed by
	 * value.
	 *
	 * Considering all this we will need to create the timer object in the
	 * current function. We will provide a dummy time here one and
	 * change in later. One assumption from the current observation is
	 * that only one shot timers are created. So we will assume this
	 * and raise assert in *timer_start if this is not the case.
	 */
#define DUMMY_TIME_PARAM 10

	int rv = os_timer_create((os_timer_t *)pptimer, NULL, DUMMY_TIME_PARAM,
				 (TimerCallbackFunction_t)callback, pcontext,
				 OS_TIMER_ONE_SHOT, OS_TIMER_NO_ACTIVATE);
	if (rv != WM_SUCCESS) {
		w_tmr_e("Unable to create timer");
		return MLAN_STATUS_FAILURE;
	}

	return MLAN_STATUS_SUCCESS;
}

/** moal_start_timer*/
static mlan_status wrapper_moal_start_timer(IN t_void * pmoal_handle
				     __UNUSED__,
				     IN t_void *ptimer,
				     IN t_u8 periodic, IN t_u32 msec)
{
	w_tmr_d("Starting timer: %p for %d mS", ptimer, msec);

	if (periodic == MTRUE) {
		/* Check note in wrapper_moal_init_timer for details */
		w_tmr_w("Periodic is not supported temporarily");
		wmpanic();
	}

	int rv = os_timer_change((os_timer_t *)&ptimer, os_msec_to_ticks(msec), 0);
	if (rv != WM_SUCCESS) {
		w_tmr_e("Unable to change period in timer");
		return MLAN_STATUS_FAILURE;
	}

	rv = os_timer_activate((os_timer_t *)&ptimer);
	if (rv != WM_SUCCESS) {
		w_tmr_e("Unable to start timer");
		return MLAN_STATUS_FAILURE;
	}

	return MLAN_STATUS_SUCCESS;
}

/** moal_stop_timer*/
static mlan_status wrapper_moal_stop_timer(IN t_void * pmoal_handle
				    __UNUSED__,
				    IN t_void *ptimer)
{
	w_tmr_d("Stopping timer: %p", ptimer);

	int rv = os_timer_deactivate((os_timer_t *)&ptimer);
	if (rv != WM_SUCCESS) {
		w_tmr_e("Unable to stop timer.");
		return MLAN_STATUS_FAILURE;
	}

	return MLAN_STATUS_SUCCESS;
}

/** moal_free_timer */
static mlan_status wrapper_moal_free_timer(IN t_void * pmoal_handle
				    __UNUSED__,
				    IN t_void ** pptimer)
{
	w_tmr_d("Deleting timer: %p", *((uint32_t *)pptimer));

	int rv = os_timer_delete((os_timer_t *)pptimer);
	if (rv != WM_SUCCESS) {
		w_tmr_e("Unable to delete timer.");
		return MLAN_STATUS_FAILURE;
	}

	return MLAN_STATUS_SUCCESS;
}

static mlan_status wrapper_moal_init_lock(IN t_void * pmoal_handle,
				   OUT t_void ** pplock)
{
#ifdef DEBUG_DEVELOPMENT
	wmprintf("%s: Locks not needed for now\n\r", __func__);
#endif /* DEBUG_DEVELOPMENT */
	return MLAN_STATUS_SUCCESS;
}

/** moal_free_lock */
static mlan_status wrapper_moal_free_lock(IN t_void *pmoal_handle, IN t_void *plock)
{
#ifdef DEBUG_DEVELOPMENT
	wmprintf("%s: Locks not needed for now\n\r", __func__);
#endif /* DEBUG_DEVELOPMENT */
	return MLAN_STATUS_SUCCESS;
}

/** moal_spin_lock */
static mlan_status wrapper_moal_spin_lock(IN t_void * pmoal_handle,
					  IN t_void * plock)
{
#ifdef DEBUG_DEVELOPMENT
	wmprintf("%s: Locks not needed for now\n\r", __func__);
#endif /* DEBUG_DEVELOPMENT */
	return MLAN_STATUS_SUCCESS;
}

/** moal_spin_unlock */
static mlan_status wrapper_moal_spin_unlock(IN t_void * pmoal_handle,
				     IN t_void *plock)
{
#ifdef DEBUG_DEVELOPMENT
	wmprintf("%s: Locks not needed for now\n\r", __func__);
#endif /* DEBUG_DEVELOPMENT */
	return MLAN_STATUS_SUCCESS;
}

/** woal_callbacks */
static mlan_callbacks woal_callbacks = {
	.moal_malloc = wrapper_moal_malloc,
	.moal_mfree = wrapper_moal_mfree,
	.moal_recv_packet = wrapper_moal_recv_packet,
	.moal_init_timer = wrapper_moal_init_timer,
	.moal_free_timer = wrapper_moal_free_timer,
	.moal_start_timer = wrapper_moal_start_timer,
	.moal_stop_timer = wrapper_moal_stop_timer,
	.moal_init_lock = wrapper_moal_init_lock,
	.moal_free_lock = wrapper_moal_free_lock,
	.moal_spin_lock = wrapper_moal_spin_lock,
	.moal_spin_unlock = wrapper_moal_spin_unlock,
};

int mlan_subsys_init()
{
	memcpy(&mlan_dev.callbacks, &woal_callbacks, sizeof(mlan_callbacks));

	/* The mlinux driver has an array of these which is dynamically allocated
	 * in function woal_update_drv_tbl (moal_main.c). We have only one.
	 */
	/** BSS type */
	mlan_dev.bss_attr[0].bss_type = MLAN_BSS_TYPE_STA;
	/** Data frame type: Ethernet II, 802.11, etc. */
	mlan_dev.bss_attr[0].frame_type = MLAN_DATA_FRAME_TYPE_ETH_II;
	/** The BSS is active (non-0) or not (0). */
	mlan_dev.bss_attr[0].active = MTRUE;
	/** BSS Priority */
	mlan_dev.bss_attr[0].bss_priority = 0;
	/** BSS number */
	mlan_dev.bss_attr[0].bss_num = 0;
	/* 11d initialization */
	/* mlan_dev.cfg_11d = 0; */

	mlan_dev.bss_attr[1].bss_type = MLAN_BSS_TYPE_UAP;
	/** Data frame type: Ethernet II, 802.11, etc. */
	mlan_dev.bss_attr[1].frame_type = MLAN_DATA_FRAME_TYPE_ETH_II;
	/** The BSS is active (non-0) or not (0). */
	mlan_dev.bss_attr[1].active = MTRUE;
	/** BSS Priority */
	mlan_dev.bss_attr[1].bss_priority = 0;
	/** BSS number */
	mlan_dev.bss_attr[1].bss_num = 0;

	/* fixme: check return value above status */
	mlan_register(&mlan_dev, (t_void **) & mlan_adap);

	mlan_init_fw(mlan_adap);
	/* wlan_init_adapter(mlan_adap); */

	return WM_SUCCESS;
}

#ifdef DEBUG_11N_AGGR
int wifi_get_amsdu_status()
{
	wifi_get_command_lock();

	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	memset(cmd, 0x00, sizeof(HostCmd_DS_COMMAND));

	cmd->seq_num = 0x0;
	cmd->result = 0x0;

	wlan_cmd_amsdu_aggr_ctrl((mlan_private *) mlan_adap->priv[0],
				 cmd, HostCmd_ACT_GEN_GET, NULL);

	wmprintf("Waiting for cmdresp\n\r");
	wifi_wait_for_cmdresp(NULL);
	wmprintf("DONE\n\r");

	mlan_ds_11n_tx_cfg txcfg;
	memset(&txcfg, 0x00, sizeof(mlan_ds_11n_tx_cfg));

	wifi_get_command_lock();
	wlan_cmd_11n_cfg((mlan_private *) mlan_adap->priv[0],
			 cmd, HostCmd_ACT_GEN_GET, &txcfg);

	wmprintf("Waiting for cmdresp\n\r");
	wifi_wait_for_cmdresp(NULL);
	wmprintf("DONE\n\r");

	return 0;
}
#endif /* DEBUG_11N_AGGR */

#ifdef CONFIG_11N
/* Note: The saved_event_buff is allocated by the cmd resp/event handler
   thread. We need to free it in this function */
int wrapper_wlan_cmd_11n_addba_rspgen(void *saved_event_buff)
{
	ASSERT(saved_event_buff != NULL);

	Event_Ext_t *evt = (Event_Ext_t *)saved_event_buff;

	wifi_get_command_lock();

	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	memset(cmd, 0x00, sizeof(HostCmd_DS_COMMAND));

	if (evt->bss_type==BSS_TYPE_STA) {
		cmd->seq_num = HostCmd_SET_SEQ_NO_BSS_INFO(
				0 /* seq_num */,
				0 /* bss_num */,
				BSS_TYPE_STA);
		wlan_cmd_11n_addba_rspgen((mlan_private *)
				mlan_adap->priv[evt->bss_type],
				cmd, &evt->reason_code);
	} else if (evt->bss_type == BSS_TYPE_UAP) {
		cmd->seq_num = HostCmd_SET_SEQ_NO_BSS_INFO(
				0 /* seq_num */,
				0 /* bss_num */,
				BSS_TYPE_UAP);

		wlan_cmd_11n_uap_addba_rspgen((mlan_private *)
				mlan_adap->priv[evt->bss_type],
				cmd, &evt->reason_code);
	}

#ifdef DEBUG_11N_AGGR
	wmprintf("ADDBA response\n\r");
	dump_addba_req_rsp_packet(cmd);
	wmprintf("Waiting for cmd resp\n\r");
#endif /* DEBUG_11N_AGGR */

	os_mem_free(saved_event_buff);
	wifi_wait_for_cmdresp(NULL);

	return 0;
}

int wrapper_wlan_cmd_11n_ba_stream_timeout(void *saved_event_buff)
{
	ASSERT(saved_event_buff != NULL);

	Event_Ext_t *evt = (Event_Ext_t *)saved_event_buff;

	wlan_11n_ba_stream_timeout(mlan_adap->priv[evt->bss_type],
		(HostCmd_DS_11N_BATIMEOUT*) &evt->reason_code);

	os_mem_free(saved_event_buff);

	return 0;
}

int wrapper_wlan_cmd_11n_delba_rspgen(void *saved_event_buff)
{
	ASSERT(saved_event_buff != NULL);

	Event_Ext_t *evt = (Event_Ext_t *)saved_event_buff;

	wlan_11n_delete_bastream((mlan_private *) mlan_adap->priv[0],
				 (t_u8 *)&evt->reason_code);

	os_mem_free(saved_event_buff);

	return 0;
}

struct uap_ampdu_stat_t {
	uint8_t mac_addr[MLAN_MAC_ADDR_LENGTH];
	uint8_t ampudu_stat;
	uint8_t ampudu_supported;
};

static struct uap_ampdu_stat_t uap_ampdu_stat_array[32];

static int wlan_find_ampud_info(uint8_t *addr,
		struct uap_ampdu_stat_t **ampdu_info)
{
	int i;

	for (i = 0; i < 32; i++) {
		if(!memcmp(addr, uap_ampdu_stat_array[i].mac_addr, MLAN_MAC_ADDR_LENGTH)) {
			*ampdu_info = &uap_ampdu_stat_array[i];
			return MLAN_STATUS_SUCCESS;
		}
	}

	return MLAN_STATUS_FAILURE;
}

static void wlan_update_uap_ampdu_stat(uint8_t*addr, uint8_t status)
{
	struct uap_ampdu_stat_t *ampdu_info;
	if (!wlan_find_ampud_info(addr, &ampdu_info)) {
		ampdu_info->ampudu_stat= status;
	} else {
		wifi_e("failed to udpate uap ampdu status");
	}
}

static void wlan_update_uap_ampdu_supported(uint8_t *addr, uint8_t supported)
{
	struct uap_ampdu_stat_t *ampdu_info;
	if (!wlan_find_ampud_info(addr, &ampdu_info)) {
		ampdu_info->ampudu_supported = supported;
	} else {
		wifi_e("failed to udpate uap ampdu supported");
	}
}

void wlan_update_uap_ampdu_info(uint8_t *addr, uint8_t action)
{
	struct uap_ampdu_stat_t *ampdu_info;
	uint8_t temp_addr[MLAN_MAC_ADDR_LENGTH];
#ifdef DUMP_PACKET_MAC
	dump_mac_addr("wlan_update_uap_ampdu_info", addr);
#endif
	if (!wlan_find_ampud_info(addr, &ampdu_info)) {
		if(action) {
			wifi_w("already exist for adding uap ampdu info");
		}else {
			memset(ampdu_info->mac_addr, 0, MLAN_MAC_ADDR_LENGTH);
		}
	} else {
		if(action) {
			memset(temp_addr, 0, MLAN_MAC_ADDR_LENGTH);
			if (!wlan_find_ampud_info(temp_addr, &ampdu_info)) {
				memcpy(ampdu_info->mac_addr, addr,
					MLAN_MAC_ADDR_LENGTH);
				ampdu_info->ampudu_stat = MFALSE;
				ampdu_info->ampudu_supported = MTRUE;
			} else {
				wifi_w("not available for adding "
					"uap ampdu info");
			}
		}else {
			wifi_w("uap ampdu info has already removed");
		}
	}
}

int wrapper_wlan_upa_ampdu_enable(uint8_t *addr)
{
	int ret;
	struct uap_ampdu_stat_t *ampdu_info;

	if (!wlan_find_ampud_info(addr, &ampdu_info)) {
		if (!ampdu_info->ampudu_stat && ampdu_info->ampudu_supported) {
			ret = wlan_send_addba(mlan_adap->priv[1], 0, addr);
			if (ret) {
				wifi_e("failed to send addba req");
				return MLAN_STATUS_FAILURE;
			}
		}
	}
	return MLAN_STATUS_SUCCESS;
}

static mlan_status do_wlan_ret_11n_addba_resp(HostCmd_DS_COMMAND *resp)
{
	mlan_status rv;
#ifdef DEBUG_11N_AGGR
	wmprintf("ADDBA response result\n\r");
	dump_addba_req_rsp_packet(resp);
#endif /* DEBUG_11N_AGGR */

	if (resp->result != HostCmd_RESULT_OK) {
		wifi_e("Failed: ADDBA RESULT: %d", resp->result);
		return -WM_FAIL;
	}

	int bss_type = HostCmd_GET_BSS_TYPE(resp->seq_num);

	if (bss_type == BSS_TYPE_STA) {
		mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];
		rv = wlan_ret_11n_addba_resp(pmpriv, resp);
	}
#ifdef DEBUG_11N_AGGR
	wmprintf("ADDBA RESP RESP: %d\n\r", resp->result);
#endif /* DEBUG_11N_AGGR */

	return rv;
}

static mlan_status do_wlan_ret_11n_addba_req(mlan_private * priv,HostCmd_DS_COMMAND *resp)
{
	t_u8 tid;
	HostCmd_DS_11N_ADDBA_RSP *padd_ba_rsp =
		(HostCmd_DS_11N_ADDBA_RSP *) & resp->params.add_ba_rsp;
	padd_ba_rsp->status_code = wlan_le16_to_cpu(padd_ba_rsp->status_code);

	tid = (padd_ba_rsp->block_ack_param_set & BLOCKACKPARAM_TID_MASK)
		>> BLOCKACKPARAM_TID_POS;

	int bss_type = HostCmd_GET_BSS_TYPE(resp->seq_num);
	if (padd_ba_rsp->status_code == BA_RESULT_SUCCESS) {
		if (bss_type == MLAN_BSS_TYPE_STA)
			ampdu_status_flag = MTRUE;
		else if (bss_type == MLAN_BSS_TYPE_UAP)
			wlan_update_uap_ampdu_stat(padd_ba_rsp->peer_mac_addr,
							MTRUE);
	} else {
		if (padd_ba_rsp->add_rsp_result != BA_RESULT_TIMEOUT) {
			wifi_e("Failed: ADDBA req: %d",
				padd_ba_rsp->add_rsp_result);
			if (bss_type == MLAN_BSS_TYPE_STA)
				priv->aggr_prio_tbl[tid].ampdu_ap =
					BA_STREAM_NOT_ALLOWED;
			else if (bss_type == MLAN_BSS_TYPE_UAP)
				wlan_update_uap_ampdu_supported(padd_ba_rsp->
					peer_mac_addr, MFALSE);
		}
	}
	return MLAN_STATUS_SUCCESS;
}


static mlan_status do_wlan_ret_11n_delba(mlan_private *priv,
		HostCmd_DS_COMMAND *resp)
{
	HostCmd_DS_11N_DELBA *pdel_ba =
		(HostCmd_DS_11N_DELBA *) &resp->params.del_ba;
	int bss_type = HostCmd_GET_BSS_TYPE(resp->seq_num);
	if (pdel_ba->del_result == BA_RESULT_SUCCESS) {
		if (bss_type == MLAN_BSS_TYPE_STA)
			ampdu_status_flag = MFALSE;
		else if (bss_type == MLAN_BSS_TYPE_UAP)
			wlan_update_uap_ampdu_stat(pdel_ba->peer_mac_addr,
							MFALSE);
	} else {
		wifi_e("Failed: DELBA: %d", pdel_ba->del_result);
	}

	return MLAN_STATUS_SUCCESS;

}

static t_u8 wlan_is_ampdu_allowed(mlan_private *priv,int tid)
{
	return (priv->aggr_prio_tbl[tid].ampdu_ap != BA_STREAM_NOT_ALLOWED)
		? MTRUE : MFALSE;
}

//Only Enable AMPDU for station interface
int wrapper_wlan_sta_ampdu_enable()
{
	int ret;
	mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];
	t_u8 cur_mac[MLAN_MAC_ADDR_LENGTH];

	if (!ampdu_status_flag &&
			wlan_is_ampdu_allowed(mlan_adap->priv[0], 0)) {
		if (pmpriv->media_connected == MTRUE) {
			memcpy(cur_mac, pmpriv->curr_bss_params.
				bss_descriptor.mac_address,
				MLAN_MAC_ADDR_LENGTH);
		} else {
			wifi_e("failed to send addba req");
			return MLAN_STATUS_FAILURE;
		}

		ret = wlan_send_addba(mlan_adap->priv[0], 0,
				(t_u8*) cur_mac);
		if (ret) {
			wifi_e("failed to send addba req");
			return MLAN_STATUS_FAILURE;
		}
	}

	return MLAN_STATUS_SUCCESS;
}
#endif /* CONFIG_11N */


int wrapper_wlan_cmd_11d_enable(HostCmd_DS_COMMAND *cmd)
{
	t_u32 enable_11d = ENABLE_11D;
	memset(cmd, 0x00, sizeof(HostCmd_DS_COMMAND));
	cmd->seq_num = 0x0;
	cmd->result = 0x0;
	return  wlan_ops_sta_prepare_cmd((mlan_private *) mlan_adap->priv[0],
					 HostCmd_CMD_802_11_SNMP_MIB,
					 HostCmd_ACT_GEN_SET,
					 Dot11D_i,
					 NULL, &enable_11d, cmd);
}

int wrapper_wlan_11d_enable()
{
	t_u32 enable_11d = ENABLE_11D;

	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();
	memset(cmd, 0x00, sizeof(HostCmd_DS_COMMAND));

	cmd->seq_num = 0x0;
	cmd->result = 0x0;

	wlan_ops_sta_prepare_cmd((mlan_private *) mlan_adap->priv[0],
					 HostCmd_CMD_802_11_SNMP_MIB,
					 HostCmd_ACT_GEN_SET,
					 Dot11D_i,
					 NULL, &enable_11d, cmd);

	return wifi_wait_for_cmdresp(NULL);
}

int wrapper_wlan_cmd_get_hw_spec()
{
	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();
	memset(cmd, 0x00, sizeof(HostCmd_DS_COMMAND));
	cmd->seq_num = 0x0;
	cmd->result = 0x0;
	wlan_cmd_get_hw_spec((mlan_private *) mlan_adap->priv[0],
			     cmd);
	wifi_wait_for_cmdresp(NULL);
	return 0;
}

int wrapper_wlan_cmd_mgmt_ie(int bss_type, void *buffer,
				unsigned int len, unsigned int action)
{
	void *pdata_buf = NULL;
	HostCmd_DS_MGMT_IE_LIST_CFG ds_mgmt_ie_list_cfg;

	wifi_get_command_lock();

	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();
	memset(cmd, 0x00, sizeof(HostCmd_DS_COMMAND));
#ifdef CONFIG_P2P
	cmd->seq_num = HostCmd_SET_SEQ_NO_BSS_INFO(
						     0 /* seq_num */,
						     0 /* bss_num */,
						     MLAN_BSS_TYPE_WIFIDIRECT);
#else
	cmd->seq_num = HostCmd_SET_SEQ_NO_BSS_INFO(
						   0 /* seq_num */,
						   0 /* bss_num */,
						   bss_type);
#endif
	cmd->result = 0x0;

	memset(&ds_mgmt_ie_list_cfg, 0x00, sizeof(HostCmd_DS_MGMT_IE_LIST_CFG));

	memcpy(&ds_mgmt_ie_list_cfg, buffer, len);

	pdata_buf = &ds_mgmt_ie_list_cfg;

	mlan_status status =
		wlan_ops_sta_prepare_cmd(
					 (mlan_private *) mlan_adap->priv[0],
					 HostCmd_CMD_MGMT_IE_LIST,
					 action,
					 0, NULL, pdata_buf, cmd);

	wifi_wait_for_cmdresp(buffer);
	return status;
}

// need not handle struct pbuf or NetworkBufferDescriptor_t at all
// normallu this is utilized in moal_recv_packet but we have changed it
//int wrapper_wlan_handle_rx_packet(t_u16 datalen, RxPD *rxpd, struct pbuf *p)
//int wrapper_wlan_handle_rx_packet(const t_u16 datalen, const t_u8 *rcvdata,  NetworkBufferDescriptor_t *pxNetworkBuffer)
int wrapper_wlan_handle_rx_packet(const t_u16 datalen, const t_u8 *rcvdata)
{
	RxPD *rxpd = (RxPD *) ((t_u8 *) rcvdata + INTF_HEADER_LEN);

	t_u8 *payload = (t_u8 *) rxpd + rxpd->rx_pkt_offset;

	w_pkt_d("[recv]                  : %p T: %d L: %d", payload,
		rxpd->rx_pkt_type, rxpd->rx_pkt_length);

	/* fixme: Check if mlan buffer can be allocated from standard mlan
	   function */
	pmlan_buffer pmbuf = os_mem_alloc(sizeof(mlan_buffer));
	if (!pmbuf) {
		/* No mlan_buffer available. Drop this packet */
		/* fixme: Need to note this event. */
		wifi_w("No memory available. Have to drop packet.");
		return -WM_FAIL;
	}
	memset(pmbuf, 0x00, sizeof(mlan_buffer));

	/** Buffer descriptor, e.g. skb in Linux */
	/* Note: We are storing payload member here. We need to unwind
	   pointer when passing pbuf to lwip */
	//pmbuf->pdesc = p->payload;
	pmbuf->pdesc = (t_u8 *) payload; // abhi replacement of above
	//t_u8 *payload = (t_u8 *) rxpd + rxpd->rx_pkt_offset

	/* Store it for later retrivval. mlan stack does not know about this */
	//moal_recv_packet has been modified not to use it
	//pmbuf->lwip_pbuf = p;


	/* fixme: check that no data is accessed after this pointer
	  Note: Important: mlan requires RxPD in number of places. We
	already have duplicated the ethernet packet in the pbuf. We cannot
	afford to duplicate the entire SDIO buffer and reduplicate the
	ethernet part of it into pbuf which the lwip requires. So, we will
	duplicate only the RxPD for ourselves. We will have to change all
	the code which assumes that there is ethernet packet after RxPD */
	/** Pointer to buffer */
	/* fixme: CHK this*/
	pmbuf->pbuf = (t_u8 *) os_mem_alloc(sizeof(RxPD));
	if (!pmbuf->pbuf) {
		/* No buffer available. Drop this packet */
		/* fixme: Need to note this event. */
		os_mem_free(pmbuf);
		wifi_w("No memory available. Have to drop packet.");
		return -WM_FAIL;
	}
	memcpy(pmbuf->pbuf, rxpd, sizeof(RxPD));

	/** Offset to data */
	/* This should ideally be INTF_HEADER_LEN. But we not be storing
	   initial INTF_HEADER_LEN bytes. We will store RxPD directly */
	/* fixme: CHK this*/
	pmbuf->data_offset = 0;

	/** Data length */
	/* fixme: CHK this*/
	pmbuf->data_len = datalen;
	/** Buffer type: data, cmd, event etc. */
	pmbuf->buf_type = MLAN_BUF_TYPE_DATA;
	/** Fields below are valid for data packet only */
	/** QoS priority */
	/* t_u32 priority; */
	/** Time stamp when packet is received (seconds) */
	/* t_u32 in_ts_sec; */
	/** Time stamp when packet is received (micro seconds) */
	/* t_u32 in_ts_usec; */
	/** Time stamp when packet is processed (seconds) */
	/* t_u32 out_ts_sec; */
	/** Time stamp when packet is processed (micro seconds) */
	/* t_u32 out_ts_usec; */

	/** Fields below are valid for MLAN module only */
	/** Pointer to parent mlan_buffer */
	/* struct _mlan_buffer *pparent; */
	/** Use count for this buffer */
	/* t_u32 use_count; */

	mlan_status rv = wlan_handle_rx_packet(mlan_adap, pmbuf);
	if (rv != MLAN_STATUS_SUCCESS) {
		/* mlan did not accept this packet for some reason. Some
		   probable reasons are - duplicate packet - packet outside
		   the current 11n aggregation window.

		   We need to free allocated structures.
		*/
		os_mem_free(pmbuf->pbuf);
		os_mem_free(pmbuf);
		return -WM_FAIL;
	}

	return WM_SUCCESS;
}

/* fixme: This AMSDU special handling needs to be redesigned. We could
   create a copy of the SDIO packet buffer for _every_ packet AMSDU and
   non-AMSDU. We could then use RAW_PBUF allocation to allocate pbufs
   during final step of packet delivery to lwip stack.

   Note: This is not AMSDU inside AMPDU. This is a pure AMSDU.
   Note: We will not be duplicating the AMSDU packet from the SDIO
   buffer. So, we have to ensure that there are no race
   conditions. i.e. the SDIO buffer is not overwritten in the background.
*/
int wrapper_wlan_handle_amsdu_rx_packet(const t_u8 *rcvdata, const t_u16 datalen)
{
	w_pkt_d("[amsdu] [recv]       : L: %d", datalen);

	RxPD *rxpd = (RxPD *) ((t_u8 *) rcvdata + INTF_HEADER_LEN);

	ASSERT(rxpd->rx_pkt_type == PKT_TYPE_AMSDU);

	/* fixme: Check if mlan buffer can be allocated from standard mlan
	   function */
	pmlan_buffer pmbuf = os_mem_alloc(sizeof(mlan_buffer));
	if (!pmbuf) {
		/* No mlan_buffer available. Drop this packet */
		/* fixme: Need to note this event. */
		w_pkt_e("[amsdu] No memory available. Have to drop packet");
		return -WM_FAIL;
	}
	memset(pmbuf, 0x00, sizeof(mlan_buffer));

	/** Buffer descriptor, e.g. skb in Linux */
	/* Note: We are storing payload member here. We need to unwind
	   pointer when passing pbuf to lwip */
	pmbuf->pdesc = ((t_u8 *) rcvdata + INTF_HEADER_LEN + rxpd->rx_pkt_offset);

	/* Store it for later retrivval. mlan stack does not know about this */
	pmbuf->lwip_pbuf = NULL;

	/* Since this is an AMSDU packet we have the entire SDIO buffer
	   with us. So we do not need to duplicate RxPD
	*/
	pmbuf->pbuf = (t_u8 *) rxpd;
	/** Offset to data */
	pmbuf->data_offset = 0;

	/** Data length */
	/* fixme: CHK this*/
	pmbuf->data_len = datalen;
	/** Buffer type: data, cmd, event etc. */
	pmbuf->buf_type = MLAN_BUF_TYPE_DATA;
	/** Fields below are valid for data packet only */
	/** QoS priority */
	/* t_u32 priority; */
	/** Time stamp when packet is received (seconds) */
	/* t_u32 in_ts_sec; */
	/** Time stamp when packet is received (micro seconds) */
	/* t_u32 in_ts_usec; */
	/** Time stamp when packet is processed (seconds) */
	/* t_u32 out_ts_sec; */
	/** Time stamp when packet is processed (micro seconds) */
	/* t_u32 out_ts_usec; */

	/** Fields below are valid for MLAN module only */
	/** Pointer to parent mlan_buffer */
	/* struct _mlan_buffer *pparent; */
	/** Use count for this buffer */
	/* t_u32 use_count; */

	if (rxpd->bss_type == MLAN_BSS_ROLE_STA) {
		wlan_handle_rx_packet(mlan_adap, pmbuf);
	} else {
		pmlan_private priv = wlan_get_priv(mlan_adap, MLAN_BSS_ROLE_UAP);
		pmbuf->data_len = rxpd->rx_pkt_length;
		pmbuf->data_offset += rxpd->rx_pkt_offset;
		wlan_11n_deaggregate_pkt(priv, pmbuf);
	}
	/* if (rv != MLAN_STATUS_SUCCESS) { */
	/*
	  We need to free allocated structures. In case of AMSDU this pmbuf
	  is not freed inside mlan
	*/
	os_mem_free(pmbuf);
	/* return -WM_FAIL; */
	/* } */

	return WM_SUCCESS;
}

void wrapper_wlan_cmd_11n_cfg(HostCmd_DS_COMMAND * cmd) {

	mlan_ds_11n_tx_cfg txcfg = {
		/** HTTxCap */
		.httxcap = DOT11N_CFG_ENABLE_GREENFIELD_XMIT |
		DOT11N_CFG_ENABLE_SHORT_GI_20MHZ |
		DOT11N_CFG_ENABLE_SHORT_GI_40MHZ,
		/** HTTxInfo */
		.httxinfo = DOT11N_CFG_ENABLE_RIFS,
		/** Band selection */
		.misc_cfg = BAND_SELECT_BOTH
	};

	wlan_cmd_11n_cfg((mlan_private *) mlan_adap->priv[0],
			 cmd, HostCmd_ACT_GEN_SET, &txcfg);
}

uint8_t txpwrlimit_no_of_channels;
uint8_t **txpwrlimit_data;
bool txpwrlimit_data_valid;

/*
  fixme: Following three functions will not be needed when
  wlan_11h_handle_event_chanswann is used to handle the relevant event */
void set_event_chanswann()
{
	mlan_adap->state_11h.recvd_chanswann_event = MTRUE;
}

void clear_event_chanswann()
{
	mlan_adap->state_11h.recvd_chanswann_event = MFALSE;
}

void wrapper_wlan_11d_clear_parsedtable()
{
	mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];
	wlan_11d_clear_parsedtable(pmpriv);
}

/*
  This was added to solve the radar DFS issue. The function
  wlan_11h_radar_detect_required() checks for this and sets the scan to
  active if media is connected. The temporary problem with our current port
  of mlan is that after a deauth is sent after channel switch event from
  the AP, the media connected event is still MTRUE. Hence, during scan the
  old channel of the AP is marked as active scan which is not a correct way
  of scanning on that channel (since radar interference is detected on
  it). So, for now we clear this bit manually to avoid active scan on the
  old channel. This bit will be set again at the next association completion.

  fixme: Check this later.
 */
void wrapper_clear_media_connected_event()
{
	mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];
	pmpriv->media_connected = MFALSE;
}

mlan_status wifi_prepare_and_send_cmd(IN mlan_private * pmpriv,
				      IN t_u16 cmd_no,
				      IN t_u16 cmd_action,
				      IN t_u32 cmd_oid,
				      IN t_void * pioctl_buf,
				      IN t_void * pdata_buf,
				      int bss_type, void *priv)
{
	pmlan_ioctl_req pioctl_req = (mlan_ioctl_req *) pioctl_buf;
	mlan_status rv = MLAN_STATUS_SUCCESS;

	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();

	if (pioctl_req)
		if (pioctl_req->bss_index == 1)
			bss_type = MLAN_BSS_TYPE_UAP;

#ifdef CONFIG_P2P
	cmd->seq_num = HostCmd_SET_SEQ_NO_BSS_INFO(
						   0 /* seq_num */,
						   0 /* bss_num */,
						   MLAN_BSS_TYPE_WIFIDIRECT);
#else
	cmd->seq_num = HostCmd_SET_SEQ_NO_BSS_INFO(
						   0 /* seq_num */,
						   0 /* bss_num */,
						   bss_type);
#endif /* CONFIG_P2P */
	cmd->result = 0x0;

	if (bss_type == MLAN_BSS_TYPE_UAP)
		rv = wlan_ops_uap_prepare_cmd(pmpriv, cmd_no,
				  cmd_action, cmd_oid, pioctl_buf, pdata_buf, cmd);
	else
		rv = wlan_ops_sta_prepare_cmd(pmpriv, cmd_no,
				  cmd_action, cmd_oid, pioctl_buf, pdata_buf, cmd);

	if (rv != MLAN_STATUS_SUCCESS) {
		wifi_e("Failed to prepare cmd.");
		return rv;
	}

	wifi_wait_for_cmdresp(priv);
	return MLAN_STATUS_SUCCESS;
}

bool wmsdk_is_11N_enabled()
{
#ifdef CONFIG_11N
	return true;
#else
	return false;
#endif /* CONFIG_11N */
}

/* static */ int wifi_send_bss_ioctl(mlan_ds_bss *bss)
{
	/* fixme: check if this needs to go on heap */
	mlan_ioctl_req req;

	memset(&req, 0x00, sizeof(mlan_ioctl_req));
	req.pbuf = (t_u8 *)bss;
	req.buf_len = sizeof(mlan_ds_bss);
	req.bss_index = 0;
	req.req_id = MLAN_IOCTL_BSS;

	mlan_status rv = wlan_ops_sta_ioctl(mlan_adap, &req);
	if (rv != MLAN_STATUS_SUCCESS && rv != MLAN_STATUS_PENDING) {
		return -WM_FAIL;
	}

	return WM_SUCCESS;
}

int wrapper_get_wpa_ie_in_assoc(uint8_t *wpa_ie)
{
	mlan_private *priv = (mlan_private *) mlan_adap->priv[0];

	/* Reset all state variables */
	memcpy(wpa_ie, &priv->wpa_ie, sizeof(priv->wpa_ie));

	return priv->wpa_ie_len;
}

static int wifi_send_tx_power_cfg_ioctl(int action, mlan_ds_power_cfg *ds_power_cfg)
{
	/* fixme: check if this needs to go on heap */
	mlan_ioctl_req req;

	memset(&req, 0x00, sizeof(mlan_ioctl_req));
	req.pbuf = (t_u8 *) ds_power_cfg;
	req.buf_len = sizeof(mlan_ds_power_cfg);
	req.bss_index = 0;
	req.req_id = MLAN_IOCTL_POWER_CFG;
	req.action = action;

	mlan_status rv = wlan_ops_sta_ioctl(mlan_adap, &req);
	if (rv != MLAN_STATUS_SUCCESS && rv != MLAN_STATUS_PENDING) {
		return -WM_FAIL;
	}

	return WM_SUCCESS;
}

int wifi_set_tx_power(int power_level)
{
	mlan_ds_power_cfg ds_power_cfg;

	memset(&ds_power_cfg, 0x00, sizeof(mlan_ds_power_cfg));

	ds_power_cfg.sub_command = MLAN_OID_POWER_CFG;
	ds_power_cfg.param.power_cfg.is_power_auto = false;
	ds_power_cfg.param.power_cfg.power_level = power_level;

	return wifi_send_tx_power_cfg_ioctl(MLAN_ACT_SET, &ds_power_cfg);
}

int wifi_set_tx_power_ext(uint32_t len, uint32_t *power_data)
{
	mlan_ds_power_cfg ds_power_cfg;

	memset(&ds_power_cfg, 0x00, sizeof(mlan_ds_power_cfg));

	ds_power_cfg.sub_command = MLAN_OID_POWER_CFG_EXT;
	ds_power_cfg.param.power_ext.len = len;
	memcpy(ds_power_cfg.param.power_ext.power_data, power_data, len);

	return wifi_send_tx_power_cfg_ioctl(MLAN_ACT_SET, &ds_power_cfg);
}

int wifi_get_tx_power()
{
	mlan_ds_power_cfg ds_power_cfg;

	memset(&ds_power_cfg, 0x00, sizeof(mlan_ds_power_cfg));

	ds_power_cfg.sub_command = MLAN_OID_POWER_CFG;
	ds_power_cfg.param.power_cfg.is_power_auto = false;

	return wifi_send_tx_power_cfg_ioctl(MLAN_ACT_GET, &ds_power_cfg);
}

static int wifi_send_tx_rate_cfg_ioctl(int action, mlan_ds_rate *ds_rate_cfg)
{
	/* fixme: check if this needs to go on heap */
	mlan_ioctl_req req;

	memset(&req, 0x00, sizeof(mlan_ioctl_req));
	req.pbuf = (t_u8 *) ds_rate_cfg;
	req.buf_len = sizeof(mlan_ds_rate);
	req.bss_index = 0;
	req.req_id = MLAN_IOCTL_RATE;
	req.action = action;

	mlan_status rv = wlan_ops_sta_ioctl(mlan_adap, &req);
	if (rv != MLAN_STATUS_SUCCESS && rv != MLAN_STATUS_PENDING) {
		return -WM_FAIL;
	}

	return WM_SUCCESS;
}

int wifi_set_tx_rate_index(int rate_index)
{
	mlan_ds_rate ds_rate_cfg;

	memset(&ds_rate_cfg, 0x00, sizeof(mlan_ds_rate));

	ds_rate_cfg.sub_command = MLAN_OID_RATE_CFG;
	ds_rate_cfg.param.rate_cfg.is_rate_auto = false;
	ds_rate_cfg.param.rate_cfg.rate_type = MLAN_RATE_INDEX;
	ds_rate_cfg.param.rate_cfg.rate = rate_index;

	return wifi_send_tx_rate_cfg_ioctl(MLAN_ACT_SET, &ds_rate_cfg);
}

int wifi_get_tx_rate_index()
{
	mlan_ds_rate ds_rate_cfg;

	memset(&ds_rate_cfg, 0x00, sizeof(mlan_ds_rate));

	ds_rate_cfg.sub_command = MLAN_OID_RATE_CFG;
	ds_rate_cfg.param.rate_cfg.is_rate_auto = false;

	return wifi_send_tx_rate_cfg_ioctl(MLAN_ACT_GET, &ds_rate_cfg);
}

bool _wifi_get_tx_rate_index(int *rate_index)
{
	mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];

	*rate_index = pmpriv->data_rate_index;

	return pmpriv->is_data_rate_auto;
}

bool wrapper_wlan_11d_support_is_enabled()
{
	mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];

	return wlan_11d_support_is_enabled(pmpriv);
}

/*
 * fixme: This function is temporarily present till the mlan transition is complete.
 */
int wrapper_wifi_assoc(const unsigned char *bssid, int wlan_security,
			bool is_wpa_tkip, unsigned int owe_trans_mode)
{
	mlan_private *priv = (mlan_private *) mlan_adap->priv[0];
	/* BSSDescriptor_t *bssDesc = os_mem_alloc(sizeof(BSSDescriptor_t)); */
	/* if (!bssDesc) */
	/* 	return -WM_FAIL; */
	int idx = wlan_find_bssid_in_list(priv, (unsigned char*)bssid, -1);
	if (idx == -1) {
		wifi_w("Could not find BSSID in mlan scan list");
		return -WLAN_BSSID_NOT_FOUND_IN_SCAN_LIST;
	}

	/* Reset all state variables */
	memset(&priv->wpa_ie, 0, sizeof(priv->wpa_ie));
	priv->wpa_ie_len = 0;
	priv->sec_info.wpa2_enabled = false;
	priv->sec_info.wapi_enabled = false;
	priv->sec_info.ewpa_enabled = false;
	priv->sec_info.wpa_enabled = false;

	BSSDescriptor_t *d = &mlan_adap->pscan_table[idx];
	/* fixme: This code is quite hacky and is present only because
	 * security part is yet not fully integrated into mlan. This will
	 * not be necessary after the integration is complete.
	*/
#ifdef CONFIG_WPA2_ENTP
	 if (d->prsn_ie && (wlan_security == WLAN_SECURITY_EAP_TLS ||
		wlan_security == WLAN_SECURITY_PEAP_MSCHAPV2)) {
		priv->sec_info.wpa2_enabled = true;
		if (d->rsn_ie_buff_len <= sizeof(priv->wpa_ie)) {
			memcpy(priv->wpa_ie, d->rsn_ie_buff, d->rsn_ie_buff_len);
			priv->wpa_ie_len = d->rsn_ie_buff_len;
		}
		else {
			wifi_e("Failed to copy RSN IE");
			return -WM_FAIL;
		}
	} else
#endif
	if (d->pwpa_ie && d->prsn_ie && (wlan_security ==
				WLAN_SECURITY_WPA_WPA2_MIXED)) {
		priv->sec_info.is_wpa_tkip = is_wpa_tkip;
		priv->sec_info.wpa2_enabled = true;
		if (d->rsn_ie_buff_len <= sizeof(priv->wpa_ie)) {
			memcpy(priv->wpa_ie, d->rsn_ie_buff, d->rsn_ie_buff_len);
			priv->wpa_ie_len = d->rsn_ie_buff_len;
		}
		else {
			wifi_e("Failed to copy RSN IE");
			return -WM_FAIL;
		}
	} else if (d->pwpa_ie && (wlan_security == WLAN_SECURITY_WPA ||
			wlan_security == WLAN_SECURITY_WPA_WPA2_MIXED)) {
		priv->sec_info.is_wpa_tkip = is_wpa_tkip;
		priv->sec_info.wpa_enabled = true;
		if (d->wpa_ie_buff_len <= sizeof(priv->wpa_ie)) {
			memcpy(priv->wpa_ie, d->wpa_ie_buff, d->wpa_ie_buff_len);
			priv->wpa_ie_len = d->wpa_ie_buff_len;
		}
		else {
			wifi_e("Failed to copy WPA IE");
			return -WM_FAIL;
		}
	} else if (d->prsn_ie && (wlan_security == WLAN_SECURITY_WPA2 ||
			wlan_security == WLAN_SECURITY_WPA_WPA2_MIXED ||
			owe_trans_mode == OWE_TRANS_MODE_OWE ||
			wlan_security == WLAN_SECURITY_OWE_ONLY ||
			wlan_security == WLAN_SECURITY_WPA3_SAE))  {
		if (wlan_security == WLAN_SECURITY_WPA3_SAE)
			priv->sec_info.authentication_mode = MLAN_AUTH_MODE_SAE;
		else if (owe_trans_mode == OWE_TRANS_MODE_OWE ||
			wlan_security == WLAN_SECURITY_OWE_ONLY)
			priv->sec_info.authentication_mode = MLAN_AUTH_MODE_OWE;
		priv->sec_info.is_wpa_tkip = is_wpa_tkip;
		priv->sec_info.wpa2_enabled = true;
		if (d->rsn_ie_buff_len <= sizeof(priv->wpa_ie)) {
			memcpy(priv->wpa_ie, d->rsn_ie_buff, d->rsn_ie_buff_len);
			priv->wpa_ie_len = d->rsn_ie_buff_len;
		}
		else {
			wifi_e("Failed to copy RSN IE.");
			return -WM_FAIL;
		}
	}

	/* The original assoc cmd handling function of mlan sends
	   additional two commands to the firmware; both
	   HostCmd_CMD_802_11D_DOMAIN_INFO. In the current wmsdk wifi
	   driver design we have only one command buffer. Thus, we cannot
	   new cmd while the current cmd is being sent. We will change the
	   default flow a little and send the 801.11D domain info commands
	   first and skip that step later */
	if (wlan_11d_support_is_enabled(priv)) {
		if (priv->support_11d) {
			if (priv->support_11d->
				wlan_11d_create_dnld_countryinfo_p(priv,
						(t_u8) d->bss_band)) {
				PRINTM(MERROR, "Dnld_countryinfo_11d failed\n");
				return MLAN_STATUS_FAILURE;
			}

			if (priv->support_11d->
				wlan_11d_parse_dnld_countryinfo_p(priv, d))
				return MLAN_STATUS_FAILURE;
		}
	}
	/* fixme: check if this needs to go on heap */
	mlan_ds_bss bss;
	memset(&bss, 0x00, sizeof(mlan_ds_bss));
	bss.sub_command = MLAN_OID_BSS_START;
	bss.param.ssid_bssid.idx = idx + 1; /* + 1 req. by mlan */
	return wifi_send_bss_ioctl(&bss);
}

static void compute_rssi_values(HostCmd_DS_802_11_RSSI_INFO_RSP *prssi_info_rsp,
				wifi_rssi_info_t *rssi_info)
{
	g_rssi = prssi_info_rsp->bcn_rssi_last;
	g_bcn_nf_last = prssi_info_rsp->bcn_nf_last;

	rssi_info->bcn_rssi_last = prssi_info_rsp->bcn_rssi_last;
	rssi_info->bcn_rssi_avg = prssi_info_rsp->bcn_rssi_avg;
	rssi_info->data_rssi_last = prssi_info_rsp->data_rssi_last;
	rssi_info->data_rssi_avg = prssi_info_rsp->data_rssi_avg;

	rssi_info->bcn_snr_last = CAL_SNR(prssi_info_rsp->bcn_rssi_last,
					prssi_info_rsp->bcn_nf_last);
	rssi_info->bcn_snr_avg = CAL_SNR(prssi_info_rsp->bcn_rssi_avg,
					prssi_info_rsp->bcn_nf_avg);
	rssi_info->data_snr_last = CAL_SNR(prssi_info_rsp->data_rssi_last,
					prssi_info_rsp->data_nf_last);
	rssi_info->data_snr_avg = CAL_SNR(prssi_info_rsp->data_rssi_avg,
					prssi_info_rsp->data_nf_avg);

	rssi_info->bcn_nf_last = prssi_info_rsp->bcn_nf_last;
	rssi_info->bcn_nf_avg = prssi_info_rsp->bcn_nf_avg;
	rssi_info->data_nf_last = prssi_info_rsp->data_nf_last;
	rssi_info->data_nf_avg = prssi_info_rsp->data_nf_avg;

}
static void compute_tx_rate(const mlan_private *pmpriv,
		const HostCmd_TX_RATE_QUERY *datarate, wifi_tx_rate_t *tx_r)
{
	if (datarate->ht_info & MBIT(0)) {
		/* HT rates detected */
		tx_r->tx_rate = datarate->tx_rate + MLAN_RATE_INDEX_MCS0;
	} else {
		/*
		 * For HostCmd_CMD_802_11_TX_RATE_QUERY, there is a hole in
		 * the rate table between HR/DSSS and OFDM rates, so minus
		 * 1 for OFDM rate index.
		 */
		tx_r->tx_rate = (datarate->tx_rate > MLAN_RATE_INDEX_OFDM0) ?
			datarate->tx_rate - 1 : datarate->tx_rate;
	}
	if (pmpriv->rxpd_htinfo & MBIT(0)) {
		tx_r->rx_rate = pmpriv->rxpd_rate + MLAN_RATE_INDEX_MCS0;
	} else {
		/*
		 * For rate index in RxPD, there is a hole in the
		 * rate table between HR/DSSS and OFDM rates, so
		 * minus 1 for OFDM rate index
		 */
		tx_r->rx_rate = (pmpriv->rxpd_rate > MLAN_RATE_INDEX_OFDM0)
			? pmpriv->rxpd_rate - 1 : pmpriv->rxpd_rate;
	}
}


static void load_bss_list(const HostCmd_DS_STA_LIST *sta_list)
{
	if (!wm_wifi.cmd_resp_priv)
		return;

	/* Initialize user pointer */
	*(sta_list_t **)wm_wifi.cmd_resp_priv = NULL;

	t_u16 c = sta_list->sta_count;
	if (c > MAX_NUM_CLIENTS) {
		wifi_w("Too many clients: %d", c);
		return;
	}

	sta_list_t *sl = os_mem_alloc(sizeof(sta_list_t) +
				     c * sizeof(sta_info_t));

	if (!sl)
		return;

	sl->count = c;
	sta_info_t *sta = (sta_info_t *)(&sl->count + 1);

	int i;
	const MrvlIEtypes_sta_info_t *si =
		(MrvlIEtypes_sta_info_t *)(&sta_list->sta_count + 1);
	for (i = 0; i < c && i < MAX_NUM_CLIENTS; i++) {
		if (si[i].rssi & 0x80)
			sta[i].rssi = -(256 - si[i].rssi);
		else
			sta[i].rssi = si[i].rssi;

		memcpy(sta[i].mac, si[i].mac_address, MLAN_MAC_ADDR_LENGTH);
		sta[i].power_mgmt_status = si[i].power_mfg_status;

		wifi_d("RSSI: 0x%x %d dbm",
			 sta[i].rssi, sta[i].rssi);
	}

	/* Note: Caller needs to free this */
	*(sta_list_t **)wm_wifi.cmd_resp_priv = sl;

	wm_wifi.cmd_resp_status = WM_SUCCESS;
	wm_wifi.cmd_resp_priv = NULL;
}

static void load_ver_ext(HostCmd_DS_COMMAND *resp)
{
	if (!wm_wifi.cmd_resp_priv)
		return;

	HostCmd_DS_VERSION_EXT *ver_ext = &resp->params.verext;
	wifi_fw_version_ext_t *user_ver_ext =
		(wifi_fw_version_ext_t *)wm_wifi.cmd_resp_priv;

	memcpy(user_ver_ext->version_str, ver_ext->version_str,
		resp->size - 10);

	wm_wifi.cmd_resp_status = WM_SUCCESS;
	wm_wifi.cmd_resp_priv = NULL;
}

static void load_rf_antenna_mode(HostCmd_DS_COMMAND *resp)
{
	if (!wm_wifi.cmd_resp_priv)
		return;

	/* For antenna "mode set" wm_wifi.cmd_resp_priv will be NULL */

	HostCmd_DS_802_11_RF_ANTENNA *antcfg =
		(HostCmd_DS_802_11_RF_ANTENNA *)
		&resp->params;

	uint16_t *ant_mode = (uint16_t *)wm_wifi.cmd_resp_priv;

	*ant_mode = antcfg->antenna_mode;

	wm_wifi.cmd_resp_status = WM_SUCCESS;
	wm_wifi.cmd_resp_priv = NULL;
}

int wifi_process_cmd_response(HostCmd_DS_COMMAND *resp)
{
	mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];
	mlan_status rv = MLAN_STATUS_SUCCESS;
	int result = WIFI_EVENT_REASON_FAILURE;
	void *arg;
	t_u8 *sta_addr;

	int command = (resp->command & HostCmd_CMD_ID_MASK);

	wcmdr_d("[%-8u] CMD-RESP: 0x%-2x Size: %-4d Seq: %-4d Result: %d",
		os_get_timestamp(), command,
		resp->size, resp->seq_num, resp->result);

	switch (command) {
	case HOST_CMD_SMART_MODE_CFG: {
#ifdef CONFIG_WIFI_EXTRA_DEBUG
		HostCmd_DS_SYS_CONFIG *sys_config_cmd =
            (HostCmd_DS_SYS_CONFIG *) &resp->params.sys_config;

		if (sys_config_cmd->action == HostCmd_ACT_GEN_GET)
			dump_hex(resp, resp->size);
#endif
	}
		break;
	case HOST_CMD_APCMD_BSS_STOP: {
		pmpriv->uap_bss_started = MFALSE;
		int bss_type = HostCmd_GET_BSS_TYPE(resp->seq_num);
		if ((bss_type == MLAN_BSS_TYPE_UAP)
#ifdef CONFIG_P2P
		    || (bss_type == MLAN_BSS_TYPE_WIFIDIRECT)
#endif /* CONFIG_P2P*/
		)
			wifi_event_completion(WIFI_EVENT_UAP_STOPPED,
					      WIFI_EVENT_REASON_SUCCESS,
					      NULL);
	}
		break;
	case HOST_CMD_APCMD_BSS_START: {
		int bss_type = HostCmd_GET_BSS_TYPE(resp->seq_num);
		if ((bss_type == MLAN_BSS_TYPE_UAP)
#ifdef CONFIG_P2P
		|| (bss_type == MLAN_BSS_TYPE_WIFIDIRECT)
#endif /* CONFIG_P2P*/
		)
			wifi_event_completion(WIFI_EVENT_UAP_STARTED,
					      WIFI_EVENT_REASON_SUCCESS, NULL);
	}
		break;
	case HOST_CMD_APCMD_STA_LIST: {
		const HostCmd_DS_STA_LIST *sta_list = &resp->params.sta_list;
		load_bss_list(sta_list);
	}
		break;
	case HostCmd_CMD_802_11_TX_RATE_QUERY: {
		rv = wlan_ops_sta_process_cmdresp(pmpriv, command,
						  resp, NULL);
		if (rv != MLAN_STATUS_SUCCESS)
			return -WM_FAIL;
		if (wm_wifi.cmd_resp_priv) {
			wifi_tx_rate_t *tx_r = (wifi_tx_rate_t *)
					wm_wifi.cmd_resp_priv;
			compute_tx_rate(pmpriv, &resp->params.tx_rate,
				tx_r);
		}
		wm_wifi.cmd_resp_status = WM_SUCCESS;
	}
		break;
	case HostCmd_CMD_802_11_RF_TX_POWER: {
		const HostCmd_DS_802_11_RF_TX_POWER *txp = &resp->params.txp;
		if (txp->action == HostCmd_ACT_GEN_GET) {
			if (wm_wifi.cmd_resp_priv) {
				wifi_tx_power_t *tx_p = (wifi_tx_power_t *)
					wm_wifi.cmd_resp_priv;
				tx_p->current_level = txp->current_level;
				tx_p->max_power = txp->max_power;
				tx_p->min_power = txp->min_power;
			}
		}
		wm_wifi.cmd_resp_status = WM_SUCCESS;
	}
		break;
	case HostCmd_CMD_802_11D_DOMAIN_INFO:
		/* No processing needed as of now */
		break;
	case HostCmd_CMD_GET_HW_SPEC:
		rv = wlan_ops_sta_process_cmdresp(pmpriv, command,
						  resp, NULL);
		if (rv != MLAN_STATUS_SUCCESS)
			return -WM_FAIL;

		wifi_event_completion(WIFI_EVENT_GET_HW_SPEC,
				      WIFI_EVENT_REASON_SUCCESS,
				      NULL);
		break;
	case HostCmd_CMD_802_11_SCAN:
		if (resp->result != 0) {
			wifi_w("Scan temporary failure");
			/*
			 * Abort the split scan. The firmware has returned
			 * scan failure.
			*/
			wlan_abort_split_scan();
			wifi_event_completion(WIFI_EVENT_SCAN_RESULT,
					      WIFI_EVENT_REASON_FAILURE,
					      (void *)-1);
			break;
		}

		rv = wlan_ops_sta_process_cmdresp(pmpriv, command, resp, NULL);
		if (rv != MLAN_STATUS_SUCCESS)
			return -WM_FAIL;

		if (is_split_scan_complete()) {
			wifi_d("Split scan complete");
			wifi_event_completion(WIFI_EVENT_SCAN_RESULT,
				      WIFI_EVENT_REASON_SUCCESS,
				      NULL);
		}
		break;
#ifdef EXT_SCAN_SUPPORT
	case HostCmd_CMD_802_11_SCAN_EXT:
		if (resp->result != 0) {
			wifi_w("Scan temporary failure");
			/*
			 * Abort the split scan. The firmware has returned
			 * scan failure.
			*/
			wlan_abort_split_scan();
			wifi_event_completion(WIFI_EVENT_SCAN_RESULT,
					      WIFI_EVENT_REASON_FAILURE,
					      (void *)-1);
		}
		break;
#endif
	case HostCmd_CMD_802_11_DEAUTHENTICATE:
		wlan_ret_802_11_deauthenticate(pmpriv, resp, NULL);
		wifi_event_completion(WIFI_EVENT_DEAUTHENTICATION,
				      WIFI_EVENT_REASON_SUCCESS, NULL);
		break;
	case HostCmd_CMD_802_11_HS_CFG_ENH:
		wifi_process_hs_cfg_resp((t_u8 *)resp);
		break;
	case HostCmd_CMD_802_11_PS_MODE_ENH: {
		uint16_t ps_action = 0;
		result = wifi_process_ps_enh_response((t_u8 *)resp, &ps_event,
						      &ps_action);
		arg = (void *) ((t_u32)ps_action);
		wifi_event_completion(ps_event, result, arg);
	}
		break;
#if 0
	case HostCmd_CMD_SUPPLICANT_PMK:
		wifi_event_completion(WIFI_EVENT_SUPPLICANT_PMK,
				      WIFI_EVENT_REASON_SUCCESS,
				      resp);
		break;
#endif
	case HostCmd_CMD_802_11_RF_CHANNEL: {
		const HostCmd_DS_802_11_RF_CHANNEL *ch =
				&resp->params.rf_channel;
		if (ch->action == HostCmd_ACT_GEN_GET) {
			if (wm_wifi.cmd_resp_priv) {
				wifi_rf_channel_t *rf_c = (wifi_rf_channel_t *)
					wm_wifi.cmd_resp_priv;
				rf_c->current_channel = ch->current_channel;
				rf_c->rf_type = ch->rf_type;
			}
		}
		wm_wifi.cmd_resp_status = WM_SUCCESS;
		/* Is this required */
		rv = wlan_ops_sta_process_cmdresp(pmpriv, command,
				resp, NULL);
	}
		break;
#ifdef CONFIG_11N
	case HostCmd_CMD_11N_CFG:
		rv = wlan_ret_11n_cfg(pmpriv, resp, NULL);
		break;
	case HostCmd_CMD_11N_ADDBA_REQ:
		do_wlan_ret_11n_addba_req(pmpriv,resp);
		break;
	case HostCmd_CMD_11N_DELBA:
		do_wlan_ret_11n_delba(pmpriv,resp);
		break;
	case HostCmd_CMD_11N_ADDBA_RSP:
		do_wlan_ret_11n_addba_resp(resp);
		break;
	case HostCmd_CMD_AMSDU_AGGR_CTRL:
		rv = wlan_ret_amsdu_aggr_ctrl(pmpriv, resp, NULL);
		break;
#endif /* CONFIG_11N */
	case HostCmd_CMD_MAC_MULTICAST_ADR:
		break;
	case HostCmd_CMD_802_11_ASSOCIATE: {
		rv = wlan_ops_sta_process_cmdresp(pmpriv, command, resp, NULL);
		if (rv != MLAN_STATUS_SUCCESS)
			return -WM_FAIL;

		IEEEtypes_AssocRsp_t *passoc_rsp =
			(IEEEtypes_AssocRsp_t *) &resp->params;

		if (!passoc_rsp->status_code)
			result = WIFI_EVENT_REASON_SUCCESS;
		else {
			wifi_w("Assoc failed: Status: %d\n\r", passoc_rsp->status_code);
			wifi_show_assoc_fail_reason(passoc_rsp->status_code);

			/* Since we have failed assoc attempt clear this */
			pmpriv->media_connected = MFALSE;
		}

		wifi_event_completion(WIFI_EVENT_ASSOCIATION, result, NULL);
	}
		break;
	case HostCmd_CMD_802_11_MAC_ADDRESS: {
		HostCmd_DS_802_11_MAC_ADDRESS *pmac_addr =
			&resp->params.mac_addr;

		sta_addr = os_mem_alloc(MLAN_MAC_ADDR_LENGTH);
		if (!sta_addr) {
			wifi_w("No mem. Cannot process MAC address command");
			break;
		}

		memcpy(sta_addr, (uint8_t *)&pmac_addr->mac_addr, MLAN_MAC_ADDR_LENGTH);

		wifi_event_completion(WIFI_EVENT_MAC_ADDR_CONFIG,
				      WIFI_EVENT_REASON_SUCCESS, sta_addr);
	}
		break;
	case HostCmd_CMD_802_11_KEY_MATERIAL:
		if (resp->result == 0)
			result = WIFI_EVENT_REASON_SUCCESS;
		else
			result = WIFI_EVENT_REASON_FAILURE;
		break;
	case HostCmd_CMD_MAC_REG_ACCESS: {
		HostCmd_DS_MAC_REG_ACCESS *reg;
		reg = (HostCmd_DS_MAC_REG_ACCESS *) &resp->params.mac_reg;
		if (reg->action == HostCmd_ACT_GEN_GET) {
			if (wm_wifi.cmd_resp_priv) {
				uint32_t *mac_reg = (uint32_t *)
					wm_wifi.cmd_resp_priv;
				*mac_reg = reg->value;
			}
		}
		wm_wifi.cmd_resp_status = WM_SUCCESS;
	}
		break;
	case HostCmd_CMD_BBP_REG_ACCESS: {
		HostCmd_DS_BBP_REG_ACCESS *reg;
		reg = (HostCmd_DS_BBP_REG_ACCESS *) &resp->params.bbp_reg;
		if (reg->action == HostCmd_ACT_GEN_GET) {
			if (wm_wifi.cmd_resp_priv) {
				uint32_t *bbp_reg = (uint32_t *)
					wm_wifi.cmd_resp_priv;
				*bbp_reg = reg->value;
			}
		}
		wm_wifi.cmd_resp_status = WM_SUCCESS;
	}
		break;
	case HostCmd_CMD_RF_REG_ACCESS: {
		HostCmd_DS_RF_REG_ACCESS *reg;
		reg = (HostCmd_DS_RF_REG_ACCESS *) &resp->params.rf_reg;
		if (reg->action == HostCmd_ACT_GEN_GET) {
			if (wm_wifi.cmd_resp_priv) {
				uint32_t *rf_reg = (uint32_t *)
					wm_wifi.cmd_resp_priv;
				*rf_reg = reg->value;
			}
		}
		wm_wifi.cmd_resp_status = WM_SUCCESS;
	}
		break;
	case HostCmd_CMD_802_11_EEPROM_ACCESS: {
		HostCmd_DS_802_11_EEPROM_ACCESS *eeprom;
		eeprom = (HostCmd_DS_802_11_EEPROM_ACCESS *) &resp->params.eeprom;
		if (wm_wifi.cmd_resp_priv) {
			uint8_t *buf = (uint8_t *)
				wm_wifi.cmd_resp_priv;
			memcpy(buf, &eeprom->value, eeprom->byte_count);
		}
		wm_wifi.cmd_resp_status = WM_SUCCESS;
	}
		break;
	case HostCmd_CMD_MEM_ACCESS: {
		HostCmd_DS_MEM_ACCESS *mem;
		mem = (HostCmd_DS_MEM_ACCESS *) &resp->params.mem;
		if (mem->action == HostCmd_ACT_GEN_GET) {
			if (wm_wifi.cmd_resp_priv) {
				uint32_t *mem_value = (uint32_t *)
					wm_wifi.cmd_resp_priv;
				*mem_value = mem->value;
			}
		}
		wm_wifi.cmd_resp_status = WM_SUCCESS;
	}
		break;
	case HostCmd_CMD_MGMT_IE_LIST: {
		HostCmd_DS_MGMT_IE_LIST_CFG *ie_list_cfg;
		ie_list_cfg = (HostCmd_DS_MGMT_IE_LIST_CFG *) &resp->params.mgmt_ie_list;
                if (wm_wifi.cmd_resp_priv) {
		uint8_t *buf = (uint8_t *)
				wm_wifi.cmd_resp_priv;
			memcpy(buf, (void *) &ie_list_cfg->ds_mgmt_ie.ie_data_list[0],
				ie_list_cfg->ds_mgmt_ie.len);
		}
		wm_wifi.cmd_resp_status = WM_SUCCESS;
	}
		break;
	case HostCmd_CMD_VERSION_EXT:
		load_ver_ext(resp);
		break;
	case HostCmd_CMD_802_11_RF_ANTENNA:
		load_rf_antenna_mode(resp);
		break;
	case HostCmd_CMD_RSSI_INFO: {
		HostCmd_DS_802_11_RSSI_INFO_RSP *prssi_info_rsp =
			(HostCmd_DS_802_11_RSSI_INFO_RSP *)&resp->params;
		if (wm_wifi.cmd_resp_priv) {
			wifi_rssi_info_t *rssi_info = (wifi_rssi_info_t *)
				wm_wifi.cmd_resp_priv;
			compute_rssi_values(prssi_info_rsp, rssi_info);
		}
		wm_wifi.cmd_resp_status = WM_SUCCESS;
	}
		break;
	case HostCmd_CMD_802_11_NET_MONITOR:
		wm_wifi.cmd_resp_status = WM_SUCCESS;
		break;
	case HOST_CMD_APCMD_SYS_CONFIGURE:
		wifi_uap_handle_cmd_resp(resp);
		break;
	case HostCmd_CMD_TXPWR_CFG:
	case HostCmd_CMD_TX_RATE_CFG:
		rv = wlan_ops_sta_process_cmdresp(pmpriv, command,
						  resp, NULL);
		if (rv != MLAN_STATUS_SUCCESS)
			return -WM_FAIL;
		break;
	case HostCmd_CMD_802_11_SNMP_MIB:
		rv = wlan_ops_sta_process_cmdresp(pmpriv, command,
                                                  resp, NULL);
                if (rv != MLAN_STATUS_SUCCESS)
                        return -WM_FAIL;
                break;
	case HostCmd_CMD_OTP_READ_USER_DATA: {
		const HostCmd_DS_OTP_USER_DATA *user_data =
					&resp->params.otp_user_data;
		if (resp->result == HostCmd_RESULT_OK) {
			if (user_data->action == HostCmd_ACT_GEN_GET) {
				if (wm_wifi.cmd_resp_priv) {
					uint8_t *buf = (uint8_t *)
						wm_wifi.cmd_resp_priv;
					memcpy(buf, user_data->user_data,
						user_data->user_data_length);
				}
			}
			wm_wifi.cmd_resp_status = WM_SUCCESS;
		} else {
			wm_wifi.cmd_resp_status = -WM_FAIL;
		}

	}
		break;
	case HostCmd_CMD_CFG_DATA: {
		const HostCmd_DS_802_11_CFG_DATA *cfg_data =
					&resp->params.cfg_data;
		if (resp->result == HostCmd_RESULT_OK) {
			if (cfg_data->action == HostCmd_ACT_GEN_GET) {
				if (wm_wifi.cmd_resp_priv) {
					wifi_cal_data_t *cal_data =
						(wifi_cal_data_t *)
						wm_wifi.cmd_resp_priv;
					cal_data->data = (uint8_t *)
						os_mem_alloc(cfg_data->
						data_len);
					if (!cal_data->data) {
						wifi_w("No mem. Cannot"
						"process CAL DATA command");
						break;
					}
					cal_data->data_len = cfg_data->data_len;

					memcpy(cal_data->data, cfg_data->data,
						cfg_data->data_len);
				}
			}
			wm_wifi.cmd_resp_status = WM_SUCCESS;
		} else {
			wm_wifi.cmd_resp_status = -WM_FAIL;
		}

	}
		break;
	case HostCmd_CMD_AUTO_RECONNECT: {
		const HostCmd_DS_AUTO_RECONNECT *auto_reconn =
					&resp->params.auto_reconnect;
		if (resp->result == HostCmd_RESULT_OK) {
			if (auto_reconn->action == HostCmd_ACT_GEN_GET) {
				if (wm_wifi.cmd_resp_priv) {
					wifi_auto_reconnect_config_t
					*auto_reconnect_config =
					(wifi_auto_reconnect_config_t *)
						wm_wifi.cmd_resp_priv;

					auto_reconnect_config->
					reconnect_counter = auto_reconn->
					reconnect_counter;
					auto_reconnect_config->
					reconnect_interval = auto_reconn->
					reconnect_interval;
					auto_reconnect_config->
					flags = auto_reconn->
					flags;
				}
			}
			wm_wifi.cmd_resp_status = WM_SUCCESS;
		} else {
			wm_wifi.cmd_resp_status = -WM_FAIL;
		}

	}
		break;
	case HostCmd_CMD_RX_MGMT_IND: {
		if (resp->result == HostCmd_RESULT_OK)
			wm_wifi.cmd_resp_status = WM_SUCCESS;
		else
			wm_wifi.cmd_resp_status = -WM_FAIL;
	}
		break;
	case HostCmd_CMD_CHANNEL_TRPC_CONFIG: {
		const HostCmd_DS_CHAN_TRPC_CONFIG *chan_trpc_cfg =
					&resp->params.chan_trpc_cfg;
		if (resp->result == HostCmd_RESULT_OK) {
			if (chan_trpc_cfg->action == HostCmd_ACT_GEN_GET) {
				if (wm_wifi.cmd_resp_priv) {
					int i;
					chan_trpc_t
					*trpc_config =
					(chan_trpc_t *)
						wm_wifi.cmd_resp_priv;

					trpc_config->num_chans = 14;
					for (i = 0; i < 14; i++) {
						trpc_config->
							chan_trpc_config[i].
							num_mod_grps = 7;
						memcpy(&trpc_config->
						chan_trpc_config[i].chan_desc,
						((uint8_t *)&chan_trpc_cfg->
						ChanTrpcG.ChanTrpc[i].chanDesc
						+ (i * 6)),
						sizeof(MrvlChannelDesc_t));
						memcpy(trpc_config->
						chan_trpc_config[i].
						chan_trpc_entry,
						((uint8_t *)chan_trpc_cfg->
						ChanTrpcG.ChanTrpc[i].
						chanTrpcEntry +
						(i * 6)), 7 *
						sizeof(MrvlChanTrpcEntry_t));
					}
				}
			}
			wm_wifi.cmd_resp_status = WM_SUCCESS;
		} else {
			wm_wifi.cmd_resp_status = -WM_FAIL;
		}

	}
		break;
	case HostCmd_CMD_802_11_GET_LOG: {
		const HostCmd_DS_802_11_GET_LOG *get_log =
					&resp->params.get_log;
		if (resp->result == HostCmd_RESULT_OK) {
			if (wm_wifi.cmd_resp_priv) {
				uint8_t *buf = (uint8_t *)
						wm_wifi.cmd_resp_priv;
				memcpy(buf, get_log,
						sizeof(wlan_pkt_stats_t));
			}
			wm_wifi.cmd_resp_status = WM_SUCCESS;
		} else {
			wm_wifi.cmd_resp_status = -WM_FAIL;
		}
	}
	break;
	case HostCmd_CMD_BRIDGE_MODE:{
		const HostCmd_BRIDGE_MODE *bridge_mode =
					&resp->params.bridge_mode;
		if (resp->result == HostCmd_RESULT_OK) {
			if (bridge_mode->action == HostCmd_ACT_GEN_GET) {
				if (wm_wifi.cmd_resp_priv) {
					wifi_bridge_cfg_t *bridgecfg =
						(wifi_bridge_cfg_t *)
						wm_wifi.cmd_resp_priv;
					t_u8 *tlv = NULL;
					MrvlIEtypes_SsIdParamSet_t *ssid = NULL;
					MrvlIEtypes_Passphrase_t *pass = NULL;
					MrvlIEtypes_AutoLinkParamSet_t *autolink = NULL;
					HostCmd_DS_GEN *hostcmd = NULL;

					memset(bridgecfg, 0x00,
						sizeof(wifi_bridge_cfg_t));

					tlv = (t_u8 *) resp +
						sizeof(HostCmd_BRIDGE_MODE) +
						S_DS_GEN;
					hostcmd=(HostCmd_DS_GEN *)resp;
					bridgecfg->enable = bridge_mode->enable;

					ssid = (MrvlIEtypes_SsIdParamSet_t *)
						(tlv + sizeof(MrvlIEtypesHeader_t));

					bridgecfg->ex_ap_ssid_len =
						wlan_cpu_to_le16(ssid->header.len);
					memcpy(bridgecfg->ex_ap_ssid, ssid->ssid,
						bridgecfg->ex_ap_ssid_len);

					tlv = (t_u8 *) ssid;

					pass = (MrvlIEtypes_Passphrase_t *)
						(tlv + sizeof(MrvlIEtypesHeader_t)
						+ ssid->header.len);

					bridgecfg->ex_ap_pass_len =
						wlan_cpu_to_le16(pass->header.len);
                                        memcpy(bridgecfg->ex_ap_pass, pass->passphrase,
                                                bridgecfg->ex_ap_pass_len);

					tlv = (t_u8 *) pass;

					ssid = (MrvlIEtypes_SsIdParamSet_t *)
						(tlv + sizeof(MrvlIEtypesHeader_t)
						+ pass->header.len);

					bridgecfg->bridge_ssid_len =
						wlan_cpu_to_le16(ssid->header.len);
					memcpy(bridgecfg->bridge_ssid, ssid->ssid,
						bridgecfg->bridge_ssid_len);

					tlv = (t_u8 *) ssid;

					pass = (MrvlIEtypes_Passphrase_t *)
						(tlv + sizeof(MrvlIEtypesHeader_t)
						+ ssid->header.len);
					bridgecfg->bridge_pass_len =
						wlan_cpu_to_le16(pass->header.len);
					memcpy(bridgecfg->bridge_pass, pass->passphrase,
							bridgecfg->bridge_pass_len);

					tlv = (t_u8 *) pass;

					autolink = (MrvlIEtypes_AutoLinkParamSet_t *)
						(tlv + sizeof(MrvlIEtypesHeader_t)
						+ pass->header.len);
					if(hostcmd->size>((char *)autolink-(char *) resp)){
						memcpy(&bridgecfg->autolink, ((char*) autolink)
								+sizeof(MrvlIEtypesHeader_t),autolink->header.len);
					}

				}
			}
			wm_wifi.cmd_resp_status = WM_SUCCESS;
		} else {
			wm_wifi.cmd_resp_status = -WM_FAIL;
		}

	}
		break;
	case HostCmd_CMD_RECONFIGURE_TX_BUFF:
		if (resp->result == HostCmd_RESULT_OK)
			wm_wifi.cmd_resp_status = WM_SUCCESS;
		else
			wm_wifi.cmd_resp_status = -WM_FAIL;
		break;
	case HostCmd_CMD_PMF_PARAMS: {
		const HostCmd_DS_PMF_PARAMS *get_pmf_params =
					&resp->params.pmf_params;
		if ((resp->result == HostCmd_RESULT_OK) &&
			(get_pmf_params->action == HostCmd_ACT_GEN_GET)) {
			if (wm_wifi.cmd_resp_priv) {
				wifi_pmf_params_t *wifi_pmf_params =
					(wifi_pmf_params_t *) wm_wifi.cmd_resp_priv;

				wifi_pmf_params->mfpc = get_pmf_params->params.mfpc;
				wifi_pmf_params->mfpr = get_pmf_params->params.mfpr;
			}
			wm_wifi.cmd_resp_status = WM_SUCCESS;
		} else
			wm_wifi.cmd_resp_status = -WM_FAIL;
	}
		break;
	default:
		/* fixme: Currently handled by the legacy code. Change this
		   handling later. Also check the default return value then*/
		break;
	}

	if (rv != MLAN_STATUS_SUCCESS)
		return -WM_FAIL;

	return WM_SUCCESS;
}

#if defined(CONFIG_UAP_AMPDU_TX) || defined(CONFIG_UAP_AMPDU_RX)
/**
 *  @brief This function will search for the specific ie
 *
 *
 *  @param priv    A pointer to mlan_private
 *  @param ie_buf  A pointer to ie_buf
 *  @param ie_len  total ie length
 *  @param id      ie's id
 *
 *  @return	       ie's poiner or MNULL
 */
static t_u8 * wlan_get_specific_ie(pmlan_private priv, t_u8 * ie_buf, t_u8 ie_len,
		IEEEtypes_ElementId_e id)
{
	t_u32 bytes_left = ie_len;
	t_u8 *pcurrent_ptr = ie_buf;
	t_u16 total_ie_len;
	t_u8 *ie_ptr = MNULL;
	IEEEtypes_ElementId_e element_id;
	t_u8 element_len;

	ENTER();

	DBG_HEXDUMP(MCMD_D, "ie", ie_buf, ie_len);
	while (bytes_left >= 2) {
		element_id = (IEEEtypes_ElementId_e) (*((t_u8 *) pcurrent_ptr));
		element_len = *((t_u8 *) pcurrent_ptr + 1);
		total_ie_len = element_len + sizeof(IEEEtypes_Header_t);
		if (bytes_left < total_ie_len) {
			PRINTM(MERROR, "InterpretIE: Error in processing IE, "
					"bytes left < IE length\n");
			break;
		}
		if (element_id == id) {
			PRINTM(MCMND, "Find IE: id=%d\n", id);
			DBG_HEXDUMP(MCMND, "IE", pcurrent_ptr, total_ie_len);
			ie_ptr = pcurrent_ptr;
			break;
		}
		pcurrent_ptr += element_len + 2;
		/* Need to account for IE ID and IE Len */
		bytes_left -= (element_len + 2);
	}

	LEAVE();

	return ie_ptr;
}

/**
 *  @brief This function will search for the specific ie
 *
 *
 *  @param priv    A pointer to mlan_private
 *  @param pevent  A pointer to event buf
 *  @param sta_ptr A pointer to sta_node
 *
 *  @return	       N/A
 */
static void wrapper_wlan_check_sta_capability(pmlan_private priv,
		Event_Ext_t * pevent, sta_node * sta_ptr)
{
	t_u16 tlv_type, tlv_len;
	t_u16 frame_control, frame_sub_type = 0;
	t_u8 *assoc_req_ie = MNULL;
	t_u8 ie_len = 0, assoc_ie_len = 0;
	IEEEtypes_HTCap_t *pht_cap = MNULL;
	int tlv_buf_left = pevent->length - INTF_HEADER_LEN - ASSOC_EVENT_FIX_SIZE;
	MrvlIEtypesHeader_t *tlv = (MrvlIEtypesHeader_t *)
		((char*)pevent + INTF_HEADER_LEN + ASSOC_EVENT_FIX_SIZE);
	MrvlIETypes_MgmtFrameSet_t *mgmt_tlv = MNULL;

	ENTER();
	while (tlv_buf_left >= (int) sizeof(MrvlIEtypesHeader_t)) {
		tlv_type = wlan_le16_to_cpu(tlv->type);
		tlv_len = wlan_le16_to_cpu(tlv->len);
		if ((sizeof(MrvlIEtypesHeader_t) + tlv_len) >
				(unsigned int) tlv_buf_left) {
			PRINTM(MERROR, "wrong tlv: tlvLen=%d, tlvBufLeft=%d\n", tlv_len,
					tlv_buf_left);
			break;
		}
		if (tlv_type == TLV_TYPE_UAP_MGMT_FRAME) {
			mgmt_tlv = (MrvlIETypes_MgmtFrameSet_t *) tlv;
			memcpy(&frame_control, (t_u8 *) & (mgmt_tlv->frame_control),
					sizeof(frame_control));
			frame_sub_type =
				IEEE80211_GET_FC_MGMT_FRAME_SUBTYPE(frame_control);
			if ((mgmt_tlv->frame_control.type == 0) &&
				((frame_sub_type == SUBTYPE_ASSOC_REQUEST) ||
				 (frame_sub_type == SUBTYPE_REASSOC_REQUEST))) {

				if (frame_sub_type == SUBTYPE_ASSOC_REQUEST)
					assoc_ie_len =
						sizeof(IEEEtypes_AssocRqst_t);
				else if (frame_sub_type ==
						SUBTYPE_REASSOC_REQUEST)
					assoc_ie_len =
						sizeof(IEEEtypes_ReAssocRqst_t);

				ie_len = tlv_len - sizeof(IEEEtypes_FrameCtl_t)
						- assoc_ie_len;
				assoc_req_ie =
					(t_u8 *) tlv +
					sizeof(MrvlIETypes_MgmtFrameSet_t) +
					assoc_ie_len;
				pht_cap =
					(IEEEtypes_HTCap_t *)
						wlan_get_specific_ie(priv,
						assoc_req_ie,
						ie_len,
						HT_CAPABILITY);
				if (pht_cap) {
					PRINTM(MCMND, "STA supports 11n\n");
					sta_ptr->is_11n_enabled = MTRUE;
					if (GETHT_MAXAMSDU(pht_cap->
							ht_cap.ht_cap_info))
						sta_ptr->max_amsdu =
							MLAN_TX_DATA_BUF_SIZE_8K;
					else
						sta_ptr->max_amsdu =
							MLAN_TX_DATA_BUF_SIZE_4K;
				} else {
					PRINTM(MCMND, "STA doesn't "
							"support 11n\n");
				}
				break;
			}
		}
		tlv_buf_left -= (sizeof(MrvlIEtypesHeader_t) + tlv_len);
		tlv =
			(MrvlIEtypesHeader_t *) ((t_u8 *) tlv + tlv_len +
					sizeof(MrvlIEtypesHeader_t));
	}
	LEAVE();

	return;
}

/** Fixed size of bss start event */
#define BSS_START_EVENT_FIX_SIZE    12

/**
 *  @brief This function will search for the specific ie
 *
 *
 *  @param priv    A pointer to mlan_private
 *  @param pevent  A pointer to event buf
 *
 *  @return           N/A
 */
static void wrapper_wlan_check_uap_capability(pmlan_private priv,
		Event_Ext_t * pevent)
{
	t_u16 tlv_type, tlv_len;
	int tlv_buf_left = pevent->length - INTF_HEADER_LEN -
				BSS_START_EVENT_FIX_SIZE;
	MrvlIEtypesHeader_t *tlv = (MrvlIEtypesHeader_t *)
		((char*)pevent + INTF_HEADER_LEN + BSS_START_EVENT_FIX_SIZE);
	priv->wmm_enabled = MFALSE;
	priv->pkt_fwd = MFALSE;
	priv->is_11n_enabled = MFALSE;

	ENTER();

	while (tlv_buf_left >= (int) sizeof(MrvlIEtypesHeader_t)) {
		tlv_type = wlan_le16_to_cpu(tlv->type);
		tlv_len = wlan_le16_to_cpu(tlv->len);
		if ((sizeof(MrvlIEtypesHeader_t) + tlv_len) >
				(unsigned int) tlv_buf_left) {
			PRINTM(MERROR, "wrong tlv: tlvLen=%d, tlvBufLeft=%d\n",
					tlv_len, tlv_buf_left);
			break;
		}
		if (tlv_type == HT_CAPABILITY) {
			DBG_HEXDUMP(MCMD_D, "HT_CAP tlv", tlv,
					tlv_len + sizeof(MrvlIEtypesHeader_t));
			priv->is_11n_enabled = MTRUE;
		}
		if (tlv_type == TLV_TYPE_UAP_PKT_FWD_CTL) {
			DBG_HEXDUMP(MCMD_D, "pkt_fwd tlv", tlv,
					tlv_len + sizeof(MrvlIEtypesHeader_t));
			priv->pkt_fwd = *((t_u8 *) tlv +
					sizeof(MrvlIEtypesHeader_t));
			PRINTM(MCMND, "pkt_fwd FW: 0x%x\n", priv->pkt_fwd);
			if (priv->pkt_fwd & PKT_FWD_FW_BIT)
				priv->pkt_fwd = MFALSE;
			else
				priv->pkt_fwd |= PKT_FWD_ENABLE_BIT;
			PRINTM(MCMND, "pkt_fwd DRV: 0x%x\n", priv->pkt_fwd);
		}
		tlv_buf_left -= (sizeof(MrvlIEtypesHeader_t) + tlv_len);
		tlv =
			(MrvlIEtypesHeader_t *) ((t_u8 *) tlv + tlv_len +
					sizeof(MrvlIEtypesHeader_t));
	}
	LEAVE();
}
#endif /* CONFIG_UAP_AMPDU_TX || CONFIG_UAP_AMPDU_RX */

/* fixme: duplicated from legacy. needs to be cleaned up later */
#define IEEEtypes_REASON_UNSPEC                   1
#define IEEEtypes_REASON_PRIOR_AUTH_INVALID       2
#define IEEEtypes_REASON_DEAUTH_LEAVING           3

int wifi_handle_fw_event(struct bus_message *msg)
{
	mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];
#ifdef EXT_SCAN_SUPPORT
	mlan_status rv = MLAN_STATUS_SUCCESS;
#endif

	Event_Ext_t *evt = ((Event_Ext_t *) msg->data);
	t_u8 *sta_addr, *event_sta_addr;
	Event_AutoLink_SW_Node_t *pnewNode = NULL;
	char *pinfo = NULL;
#if defined(CONFIG_UAP_AMPDU_TX) || defined(CONFIG_UAP_AMPDU_RX)
	sta_node *sta_node_ptr;
#endif /* CONFIG_UAP_AMPDU_TX || CONFIG_UAP_AMPDU_RX */
#ifdef EXT_SCAN_SUPPORT
	mlan_event_scan_result *pext_scan_result;
#endif
	wevt_d(" [%-8u] EVENT - : 0x%x Len : %-4d Reason: %-4d",
	       os_get_timestamp(),
	       evt->event_id, evt->length, evt->reason_code);

	switch (evt->event_id) {
	case EVENT_LINK_LOST:
		wifi_event_completion(WIFI_EVENT_LINK_LOSS,
				      WIFI_EVENT_REASON_FAILURE,
				      (void *)IEEEtypes_REASON_DEAUTH_LEAVING);
#ifdef CONFIG_11N
		/* fixme: Should this be outside CONFIG_11N ? */
		wlan_handle_disconnect_event(pmpriv);
#endif
		break;
	case EVENT_DEAUTHENTICATED:
		if (!evt->reason_code)
			wifi_event_completion(WIFI_EVENT_LINK_LOSS,
					WIFI_EVENT_REASON_FAILURE,
					(void *)
					IEEEtypes_REASON_DEAUTH_LEAVING);
		else
			wifi_event_completion(WIFI_EVENT_AUTHENTICATION,
					WIFI_EVENT_REASON_FAILURE,
					(void *)&evt->reason_code);
#ifdef CONFIG_11N
		/* fixme: Should this be outside CONFIG_11N ? */
		wlan_handle_disconnect_event(pmpriv);
#endif
		break;
	case EVENT_DISASSOCIATED:
		wifi_event_completion(WIFI_EVENT_DISASSOCIATION,
				WIFI_EVENT_REASON_FAILURE,
				(void *)
				IEEEtypes_REASON_DEAUTH_LEAVING);
#ifdef CONFIG_11N
		/* fixme: Should this be outside CONFIG_11N ? */
		wlan_handle_disconnect_event(pmpriv);
#endif
		break;
	case EVENT_PORT_RELEASE:
		wifi_event_completion(WIFI_EVENT_AUTHENTICATION,
				      WIFI_EVENT_REASON_SUCCESS, NULL);
		break;
	case EVENT_PS_SLEEP:
		wifi_event_completion(WIFI_EVENT_SLEEP,
				      WIFI_EVENT_REASON_SUCCESS, NULL);
		break;
	case EVENT_PS_AWAKE:
		wifi_event_completion(WIFI_EVENT_AWAKE,
				      WIFI_EVENT_REASON_SUCCESS, NULL);
		break;
	case EVENT_MIC_ERR_MULTICAST:
		wifi_event_completion(WIFI_EVENT_ERR_MULTICAST,
				WIFI_EVENT_REASON_SUCCESS, NULL);
		break;
	case EVENT_MIC_ERR_UNICAST:
		wifi_event_completion(WIFI_EVENT_ERR_UNICAST,
				WIFI_EVENT_REASON_SUCCESS, NULL);
		break;
#ifdef CONFIG_P2P
	case EVENT_WIFIDIRECT_GENERIC_EVENT:
	case EVENT_WIFIDIRECT_SERVICE_DISCOVERY:
		wifi_d("EVENT: WIFIDIRECT event 0x%x\r\n", evt->event_id);
		wifi_wfd_event(true, false, &evt->event_id);
		break;
#endif
	case EVENT_HS_ACT_REQ:
		wifi_event_completion(WIFI_EVENT_HS_CONFIG,
				      WIFI_EVENT_REASON_SUCCESS, NULL);
		break;
#ifdef CONFIG_11N
	case EVENT_ADDBA: {
		void *saved_event_buff = wifi_11n_save_request(evt);
		if (saved_event_buff)
			wifi_event_completion(WIFI_EVENT_11N_ADDBA,
					      WIFI_EVENT_REASON_SUCCESS,
					      saved_event_buff);

		/* If allocation failed ignore this event quietly ! */
	}
		break;
	case EVENT_DELBA: {
		if(evt->bss_type == BSS_TYPE_STA) {
			void *saved_event_buff = wifi_11n_save_request(evt);
			if (saved_event_buff)
				wifi_event_completion(WIFI_EVENT_11N_DELBA,
						      WIFI_EVENT_REASON_SUCCESS,
						      saved_event_buff);

			/* If allocation failed ignore this event quietly ! */
		}
	}
		break;
	case EVENT_BA_STREAM_TIMEOUT: {
	      void *saved_event_buff = wifi_11n_save_request(evt);
	      if (saved_event_buff)
			wifi_event_completion(WIFI_EVENT_11N_BA_STREAM_TIMEOUT,
					WIFI_EVENT_REASON_SUCCESS,
					saved_event_buff);
		/* If allocation failed ignore this event quietly ! */
	}
	      break;
	case EVENT_AMSDU_AGGR_CTRL:
		/*
		 * Currently this event buffer isn't being used. Enable
		 * this when required.
		 */
		/* wifi_11n_save_request(evt); */
		wifi_event_completion(WIFI_EVENT_11N_AGGR_CTRL,
				      WIFI_EVENT_REASON_SUCCESS, NULL);
		break;
#endif /* CONFIG_11N */
#ifdef CONFIG_5GHz_SUPPORT
	case EVENT_CHANNEL_SWITCH_ANN:
		wifi_event_completion(WIFI_EVENT_CHAN_SWITCH,
				      WIFI_EVENT_REASON_SUCCESS, NULL);
		break;
#endif
	case EVENT_MICRO_AP_STA_ASSOC: {
		/*
		 * Alloc memory to store the STA mac id. This will be
		 * passed to event receiver thread. Freeing this is
		 * responsibility of the receiving thread.
		 */
		sta_addr = os_mem_alloc(MLAN_MAC_ADDR_LENGTH);
		if (!sta_addr) {
			wifi_w("No mem. Cannot process MAC address from assoc");
			break;
		}

		event_sta_addr = (t_u8 *)&evt->src_mac_addr;
		memcpy(sta_addr, event_sta_addr, MLAN_MAC_ADDR_LENGTH);

#if defined(CONFIG_UAP_AMPDU_TX) || defined(CONFIG_UAP_AMPDU_RX)
		wlan_update_uap_ampdu_info(sta_addr,1);

		sta_node_ptr = os_mem_alloc(sizeof(sta_node));
		if (!sta_node_ptr) {
			wifi_w("No mem. Cannot check station type");
			break;
		}

		wrapper_wlan_check_sta_capability((mlan_private *)
				mlan_adap->priv[1], msg->data, sta_node_ptr);

		if(!sta_node_ptr->is_11n_enabled)
			wlan_update_uap_ampdu_supported(sta_addr, MFALSE);

		os_mem_free(sta_node_ptr);
#endif /* CONFIG_UAP_AMPDU_TX || CONFIG_UAP_AMPDU_RX */

		wifi_event_completion(WIFI_EVENT_UAP_CLIENT_ASSOC,
				      WIFI_EVENT_REASON_SUCCESS, sta_addr);
		}
		break;
	case EVENT_MICRO_AP_STA_DEAUTH:
		if ((evt->reason_code ==
			IEEEtypes_REASON_PRIOR_AUTH_INVALID) ||
		    (evt->reason_code ==
			IEEEtypes_REASON_UNSPEC)) {
		/*
		 * Alloc memory to store the STA mac id. This will be
		 * passed to event receiver thread. Freeing this is
		 * responsibility of the receiving thread.
		 */
		sta_addr = os_mem_alloc(MLAN_MAC_ADDR_LENGTH);
		if (!sta_addr) {
			wifi_w("No mem. Cannot process MAC address from assoc");
			break;
		}
		event_sta_addr = (t_u8 *)&evt->src_mac_addr;
		memcpy(sta_addr, event_sta_addr, MLAN_MAC_ADDR_LENGTH);
		wifi_event_completion(WIFI_EVENT_UAP_CLIENT_DEAUTH,
				      WIFI_EVENT_REASON_SUCCESS, sta_addr);
		}
#if defined(CONFIG_UAP_AMPDU_TX) || defined(CONFIG_UAP_AMPDU_RX)
		wlan_update_uap_ampdu_info(evt->src_mac_addr, 0);
#endif /* CONFIG_UAP_AMPDU_TX || CONFIG_UAP_AMPDU_RX */
		break;
	case EVENT_MICRO_AP_BSS_START:
		wifi_d("uAP start event received");
		/*
		 * wmsdk: statement copied from
		 * mlan_uap_cmdevent.c. Necessary for other uAP functions.
		 */
#if defined(CONFIG_UAP_AMPDU_TX) || defined(CONFIG_UAP_AMPDU_RX)
		wrapper_wlan_check_uap_capability((mlan_private *)
				mlan_adap->priv[1], msg->data);
#endif /* CONFIG_UAP_AMPDU_TX || CONFIG_UAP_AMPDU_RX */
		pmpriv->uap_bss_started = MTRUE;
		break;
	case EVENT_AUTO_LINK_SWITCH_NEW_NODE:
		pnewNode = (Event_AutoLink_SW_Node_t*) msg->data;
		pinfo = os_mem_calloc(pnewNode->length);
		if (!pinfo) {
			wifi_w("No mem. Cannot process auto link switch network info");
			break;
		}
		memcpy(pinfo, pnewNode, pnewNode->length);
		wifi_event_completion(WIFI_EVENT_AUTOLINK_NETWORK_SWITCHED,
				      WIFI_EVENT_REASON_SUCCESS, pinfo);
		break;
#ifdef EXT_SCAN_SUPPORT
	case EVENT_EXT_SCAN_REPORT:
		pext_scan_result = (mlan_event_scan_result *) ((t_u8 *)msg->data + 4);

		rv = wlan_handle_event_ext_scan_report(pmpriv, (t_u8 *) pext_scan_result);

		if (rv != MLAN_STATUS_SUCCESS)
			return -WM_FAIL;

		if (is_split_scan_complete()) {
			wifi_d("Split scan complete");
			wifi_event_completion(WIFI_EVENT_SCAN_RESULT,
				      WIFI_EVENT_REASON_SUCCESS,
				      NULL);
		}
		break;
#endif
	default:
		wifi_d("Event %x not implemented."
		       " Please report the event hex value.\n\r",
		       evt->event_id);
		break;
	}

	return WM_SUCCESS;
}

const uint8_t wpa_oui_none[4] = {0x00, 0x50, 0xf2, 0x00};
const uint8_t wpa_oui01[4]    = {0x00, 0x50, 0xf2, 0x01};
const uint8_t wpa_oui02[4]    = {0x00, 0x50, 0xf2, 0x02};
const uint8_t wpa_oui03[4]    = {0x00, 0x50, 0xf2, 0x03};
const uint8_t wpa_oui04[4]    = {0x00, 0x50, 0xf2, 0x04};
const uint8_t wpa_oui05[4]    = {0x00, 0x50, 0xf2, 0x05};
const uint8_t wpa_oui06[4]    = {0x00, 0x50, 0xf2, 0x06};

const uint8_t wpa2_oui01[4]   = {0x00, 0x0f, 0xac, 0x01};
const uint8_t wpa2_oui02[4]   = {0x00, 0x0f, 0xac, 0x02};
const uint8_t wpa2_oui03[4]   = {0x00, 0x0f, 0xac, 0x03};
const uint8_t wpa2_oui04[4]   = {0x00, 0x0f, 0xac, 0x04};
const uint8_t wpa2_oui05[4]   = {0x00, 0x0f, 0xac, 0x05};
const uint8_t wpa2_oui06[4]   = {0x00, 0x0f, 0xac, 0x06};

const uint8_t wpa3_oui08[4]   = {0x00, 0x0f, 0xac, 0x08};
const uint8_t wpa3_oui12[4]   = {0x00, 0x0f, 0xac, 0x12};

static unsigned char process_rsn_ie(uint8_t *rsn_ie, _Cipher_t *mcstCipher,
			      _Cipher_t *ucstCipher, bool *is_pmf_required,
				_SecurityMode_t *WPA_WPA2_WEP)
{
	IEEEtypes_Rsn_t *prsn_ie = (IEEEtypes_Rsn_t *) rsn_ie;
	uint16_t akmp_count;
	uint8_t akmp, i;

	if (prsn_ie->pairwise_cipher.count == 2) {
		prsn_ie->len -= sizeof(wpa_suite);
		prsn_ie->pairwise_cipher.count = 1;

		if (!memcmp((const void *)
			(((uint8_t *) &prsn_ie->pairwise_cipher.list) +
                        sizeof(wpa_suite)),
			wpa2_oui04, sizeof(wpa_suite))) {

			memcpy((uint8_t *) &prsn_ie->pairwise_cipher.list,
			(((uint8_t *) &prsn_ie->pairwise_cipher.list) +
			sizeof(wpa_suite)),
			sizeof(wpa_suite));
		}
		memcpy((((uint8_t *) &prsn_ie->pairwise_cipher.list) +
			sizeof(wpa_suite)),
			(uint8_t *) &prsn_ie->auth_key_mgmt,
			2 * sizeof(wpa_suite));
	}

	memcpy(&akmp_count, (((uint8_t *) &prsn_ie->pairwise_cipher.list) +
                        sizeof(wpa_suite)), sizeof(uint16_t));

	for (i = 0; i < akmp_count; i++) {
		if (!memcmp((const void *) (((uint8_t *)
			&prsn_ie->pairwise_cipher.list) +
                        sizeof(wpa_suite) + sizeof(uint16_t) +
			sizeof(wpa_suite) * i),
			wpa3_oui08, sizeof(wpa_suite)))
			WPA_WPA2_WEP->wpa3_sae = 1;
		if (!memcmp((const void *) (((uint8_t *)
			&prsn_ie->pairwise_cipher.list) +
                        sizeof(wpa_suite) + sizeof(uint16_t) +
			sizeof(wpa_suite) * i),
			wpa3_oui12, sizeof(wpa_suite)))
			WPA_WPA2_WEP->owe = 1;
	}

	if (akmp_count == 2) {
		prsn_ie->len = 20;
		akmp_count = 1;
		memcpy((((uint8_t *) &prsn_ie->pairwise_cipher.list) +
                        sizeof(wpa_suite)), &akmp_count, sizeof(uint16_t));

		if (WPA_WPA2_WEP->wpa3_sae) {
			memcpy(((uint8_t *) &prsn_ie->pairwise_cipher.list) +
				sizeof(wpa_suite) + sizeof(uint16_t),
				wpa3_oui08, sizeof(wpa_suite));
		}
		memcpy((((uint8_t *) &prsn_ie->pairwise_cipher.list) +
			2 * sizeof(wpa_suite) + sizeof(uint16_t)),
			(((uint8_t *) &prsn_ie->pairwise_cipher.list) +
			 3 * sizeof(wpa_suite) + sizeof(uint16_t)), sizeof(uint16_t));
	}

	if (!memcmp((const void *)
				(((uint8_t *) &prsn_ie->pairwise_cipher.list) +
				 sizeof(wpa_suite) + sizeof(uint16_t)),
				wpa2_oui06, sizeof(wpa_suite))) {
		memcpy(&akmp, (((uint8_t *) &prsn_ie->pairwise_cipher.list) +
					2 * sizeof(wpa_suite) + sizeof(uint16_t)),
					sizeof(uint8_t));
		if (akmp & 0xC0)
			*is_pmf_required = true;
	}

	if (!memcmp((const void *) &prsn_ie->pairwise_cipher.list,
			wpa2_oui04, sizeof(wpa_suite)))
		ucstCipher->ccmp = true;
	else if (!memcmp((const void *) &prsn_ie->pairwise_cipher.list,
			wpa2_oui02, sizeof(wpa_suite)))
		ucstCipher->tkip = true;

	if (!memcmp((const void *) &prsn_ie->group_cipher, wpa2_oui04,
		sizeof(wpa_suite)))
		mcstCipher->ccmp = true;
	else if (!memcmp((const void *) &prsn_ie->group_cipher, wpa2_oui02,
		sizeof(wpa_suite)))
		mcstCipher->tkip = true;
	else if (!memcmp((const void *) &prsn_ie->group_cipher, wpa2_oui05,
		sizeof(wpa_suite)))
		mcstCipher->wep104 = true;
	else if (!memcmp((const void *) &prsn_ie->group_cipher, wpa2_oui01,
		sizeof(wpa_suite)))
		mcstCipher->wep40 = true;

	return prsn_ie->len + 2;
}

static unsigned char process_wpa_ie(uint8_t *wpa_ie, _Cipher_t *mcstCipher,
			      _Cipher_t *ucstCipher)
{
	IEEEtypes_Wpa_t *pwpa_ie = (IEEEtypes_Wpa_t *) wpa_ie;

	if (pwpa_ie->pairwise_cipher.count == 2) {
		pwpa_ie->len = 22;
		pwpa_ie->pairwise_cipher.count = 1;

		if (!memcmp((const void *)
			(((uint8_t *) &pwpa_ie->pairwise_cipher.list) +
                        sizeof(wpa_suite)),
			wpa_oui04, sizeof(wpa_suite))) {

			memcpy((uint8_t *) &pwpa_ie->pairwise_cipher.list,
			(((uint8_t *) &pwpa_ie->pairwise_cipher.list) +
			sizeof(wpa_suite)),
			sizeof(wpa_suite));
		}

		memcpy((((uint8_t *) &pwpa_ie->pairwise_cipher.list) +
			sizeof(wpa_suite)),
			(uint8_t *) &pwpa_ie->auth_key_mgmt,
			2 * sizeof(wpa_suite));
	}

	if (!memcmp((const void *) &pwpa_ie->pairwise_cipher.list,
			wpa_oui04, sizeof(wpa_suite)))
		ucstCipher->ccmp = true;
	else if (!memcmp((const void *) &pwpa_ie->pairwise_cipher.list,
			wpa_oui02, sizeof(wpa_suite)))
		ucstCipher->tkip = true;

	if (!memcmp((const void *) &pwpa_ie->group_cipher, wpa_oui04,
		sizeof(wpa_suite)))
		mcstCipher->ccmp = true;
	else if (!memcmp((const void *) &pwpa_ie->group_cipher, wpa_oui02,
		sizeof(wpa_suite)))
		mcstCipher->tkip = true;
	else if (!memcmp((const void *) &pwpa_ie->group_cipher, wpa_oui05,
		sizeof(wpa_suite)))
		mcstCipher->wep104 = true;
	else if (!memcmp((const void *) &pwpa_ie->group_cipher, wpa_oui01,
		sizeof(wpa_suite)))
		mcstCipher->wep40 = true;

	return pwpa_ie->len + 2;
}

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
			      bool *is_pmf_required)
{
	if (bss_index >= mlan_adap->num_in_scan_table) {
		wifi_w("Unable to find given entry %d in BSS table",
		       bss_index);
		return -WM_FAIL;
	}

	/* TODO
	 * Removed : const BSSDescriptor_t *d = &mlan_adap->pscan_table[bss_index]; */
	BSSDescriptor_t *d = &mlan_adap->pscan_table[bss_index];

	memcpy(BssId, d->mac_address, MLAN_MAC_ADDR_LENGTH);

	if (d->cap_info.ibss)
		*is_ibss_bit_set = true;
	else
		*is_ibss_bit_set = false;

	if (d->ssid.ssid_len <= MLAN_MAX_SSID_LENGTH) {
		*ssid_len = (int)d->ssid.ssid_len;
		memcpy(ssid, d->ssid.ssid, d->ssid.ssid_len);
	}

	*Channel = d->channel;		/*!< Channel associated to the BSSID */

	*RSSI = (uint8_t)d->rssi;
	/* uint8_t RSSI; */		/*!< Received signal strength */

	if (d->pwpa_ie || d->prsn_ie) {
		/* Check if WPA or WPA2 */
		if (d->pwpa_ie)
			WPA_WPA2_WEP->wpa = 1;
		if (d->prsn_ie)
			WPA_WPA2_WEP->wpa2 = 1;
	} else {
		/* Check if WEP */
		if (d->cap_info.privacy)
			WPA_WPA2_WEP->wepStatic = 1;
	}

	if (d->pwpa_ie)
		d->wpa_ie_buff_len = process_wpa_ie(d->wpa_ie_buff,
					wpa_mcstCipher, wpa_ucstCipher);

	if (d->prsn_ie)
		d->rsn_ie_buff_len = process_rsn_ie(d->rsn_ie_buff,
					rsn_mcstCipher, rsn_ucstCipher, is_pmf_required,
					WPA_WPA2_WEP);

	return WM_SUCCESS;
}

int wrapper_bssdesc_second_set(int bss_index,
			       bool *phtcap_ie_present,
			       bool *phtinfo_ie_present,
			       bool *wmm_ie_present,
			       uint8_t *band,
			       bool *wps_IE_exist,
			       uint16_t *wps_session, bool *wpa2_entp_IE_exist,
			       uint8_t *trans_mode, uint8_t *trans_bssid,
			       int *trans_ssid_len, uint8_t *trans_ssid)
{
	if (bss_index >= mlan_adap->num_in_scan_table) {
		wifi_w("Unable to find given entry %d in BSS table",
			 bss_index);
		return -WM_FAIL;
	}
	const BSSDescriptor_t *d = &mlan_adap->pscan_table[bss_index];

	if (d->pht_cap)
		*phtcap_ie_present = true;
	else
		*phtcap_ie_present = false;

	if (d->pht_info)
		*phtinfo_ie_present = true;
	else
		*phtinfo_ie_present = false;

	if (d->wmm_ie.vend_hdr.element_id == WMM_IE)
		*wmm_ie_present = true;
	else
		*wmm_ie_present = false;

	*band = d->bss_band;

	if (wps_IE_exist)
		*wps_IE_exist = d->wps_IE_exist;
	if (wps_session)
		*wps_session = d->wps_session;

	if (wpa2_entp_IE_exist)
		*wpa2_entp_IE_exist = d->wpa2_entp_IE_exist;

	*trans_mode = d->owe_transition_mode;

	memcpy(trans_bssid, d->trans_mac_address, MLAN_MAC_ADDR_LENGTH);

	if (d->trans_ssid.ssid_len <= MLAN_MAX_SSID_LENGTH) {
		*trans_ssid_len = (int)d->trans_ssid.ssid_len;
		memcpy(trans_ssid, d->trans_ssid.ssid,
			d->trans_ssid.ssid_len);
	}

	return WM_SUCCESS;
}


int wifi_get_scan_result_count(unsigned *count)
{
	if (!count)
		return -WM_E_INVAL;

	*count = mlan_adap->num_in_scan_table;
	return WM_SUCCESS;
}


int wrapper_wlan_set_regiontable(t_u8 region, t_u8 band)
{
	mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];
	int rv = wlan_set_regiontable(pmpriv, region, band);
	if (rv != MLAN_STATUS_SUCCESS)
		return -WM_FAIL;

	return WM_SUCCESS;
}

/*
 * This function is supposed to be called after scan is complete.
 */
void wifi_scan_process_results()
{
	mlan_private *pmpriv = (mlan_private *) mlan_adap->priv[0];
	/*
	 * This function is normally supposed to be called from inside
	 * mlan_scan.c during parsing of scanning results. But we do not
	 * have async. cmd send functionality in the current design. So, we
	 * have moved this outside.
	 */
	wlan_scan_process_results(pmpriv);
}

#ifdef CONFIG_WPA2_ENTP
void wifi_scan_enable_wpa2_enterprise_ap_only()
{
	scan_enable_wpa2_enterprise_ap_only = true;
}

bool wifi_get_scan_enable_wpa2_enterprise_ap_only()
{
	return scan_enable_wpa2_enterprise_ap_only;
}
#endif

bool check_for_wpa2_entp_ie(bool *wpa2_entp_IE_exist, const void *element_data, unsigned element_len)
{
	uint8_t wpa2_ent_IE[4];
	uint16_t len;

	memcpy(&len, element_data, sizeof(len));

	if (len * 4 >= element_len)
		return false;

	memcpy(&wpa2_ent_IE,
			(char *) element_data + len * 4 + 2 * sizeof(len), 4);

	if (!memcmp(wpa2_ent_IE, wpa2_akmp_oui, sizeof(wpa2_akmp_oui))) {

		/* WPA2 Enterprise IE is present in probe response. */
		wifi_d("WPA2 Enterprise AKMP IE :: %x:%x:%x:%x\r\n", wpa2_ent_IE[0], wpa2_ent_IE[1],
		       wpa2_ent_IE[2], wpa2_ent_IE[3]);

		*wpa2_entp_IE_exist = true;

		return true;
	}

	return false;
}

/*
 * The below wifi authenticate code, data structures and macros seems to be
 * completely absent from mlan. Our current code, including the wlc manager
 * currently depends on this command and its response (though it may be a
 * no-op).
 *
 * fixme: remove this later when appropriate.
 */
#define host_CMD_802_11_AUTHENTICATE 0x0011
#define AUTH_OPEN_SYSTEM 0

typedef PACK_START struct host_802_11Authenticate_t
{
	uint8_t MacAddr[MLAN_MAC_ADDR_LENGTH];
	uint8_t AuthType;
	uint16_t TimeOut;
	uint8_t Reserved[2];
} PACK_END host_802_11Authenticate_t;


void wifi_get_value1_from_cmdresp(const HostCmd_DS_COMMAND *resp, uint32_t *dev_value1)
{
	if (resp->params.mac_reg.offset == wlan_cpu_to_le16((t_u16)WLAN_VALUE1))
		*dev_value1 = resp->params.mac_reg.value;
}

/*
 * fixme: This function will be present till mlan integration is complete
 */
void wifi_get_mac_address_from_cmdresp(const HostCmd_DS_COMMAND *resp,
				       uint8_t *mac_addr)
{
	memcpy(mac_addr, &resp->params.mac_addr.mac_addr,
	       MLAN_MAC_ADDR_LENGTH);
}

void wifi_get_firmware_ver_ext_from_cmdresp(const HostCmd_DS_COMMAND *resp,
				       uint8_t *fw_ver_ext)
{
	uint8_t comma = 0x2C, space = 0x20;

	if (!resp->params.verext.version_str_sel) {
		memcpy(fw_ver_ext, &resp->params.verext.version_str,
		       MLAN_MAX_VER_STR_LEN);
	} else if (resp->params.verext.version_str_sel == 3 &&
		strlen((const char *)(&resp->params.verext.version_str))) {
		memcpy(fw_ver_ext + strlen((const char *)fw_ver_ext), &comma, 1);
		memcpy(fw_ver_ext + strlen((const char *)fw_ver_ext), &space, 1);
		memcpy(fw_ver_ext + strlen((const char *)fw_ver_ext),
			&resp->params.verext.version_str,
			strlen((const char *)(&resp->params.verext.version_str)));
	} else if (resp->params.verext.version_str_sel == 4 &&
		strlen((const char *)(&resp->params.verext.version_str))) {
		memcpy(fw_ver_ext + strlen((const char *)fw_ver_ext), &comma, 1);
		memcpy(fw_ver_ext + strlen((const char *)fw_ver_ext), &space, 1);
		memcpy(fw_ver_ext + strlen((const char *)fw_ver_ext),
			&resp->params.verext.version_str,
			strlen((const char *)(&resp->params.verext.version_str)));
	}
}

void wifi_prepare_set_cal_data_cmd(HostCmd_DS_COMMAND *cmd,
				   int seq_number)
{
	cmd->command = HostCmd_CMD_CFG_DATA;
	cmd->size = sizeof(HostCmd_DS_802_11_CFG_DATA) + S_DS_GEN +
				cal_data_len - 1;
	cmd->seq_num = seq_number;
	cmd->result = 0;
	cmd->params.cfg_data.action = HostCmd_ACT_GEN_SET;
	cmd->params.cfg_data.type = 0x02;
	cmd->params.cfg_data.data_len = cal_data_len;
	memcpy(cmd->params.cfg_data.data, cal_data, cal_data_len);
}
void wifi_prepare_get_hw_spec_cmd(HostCmd_DS_COMMAND *cmd,
				   int seq_number)
{
	cmd->command = HostCmd_CMD_GET_HW_SPEC;
	cmd->size = sizeof(HostCmd_DS_GET_HW_SPEC) + S_DS_GEN;
	cmd->seq_num = seq_number;
	cmd->result = 0;
}
/*
 * fixme: This function will be present till mlan integration is complete
 */
void wifi_prepare_get_mac_addr_cmd(HostCmd_DS_COMMAND *cmd,
				   int seq_number)
{
	cmd->command = HostCmd_CMD_802_11_MAC_ADDRESS;
	cmd->size = sizeof(HostCmd_DS_802_11_MAC_ADDRESS) + S_DS_GEN;
	cmd->seq_num = seq_number;
	cmd->result = 0;
	cmd->params.mac_addr.action = HostCmd_ACT_GEN_GET;
}

void wifi_prepare_get_fw_ver_ext_cmd(HostCmd_DS_COMMAND *cmd,
				   int seq_number, int version_str_sel)
{
	cmd->command = HostCmd_CMD_VERSION_EXT;
	cmd->size = sizeof(HostCmd_DS_VERSION_EXT) + S_DS_GEN;
	cmd->seq_num = seq_number;
	cmd->result = 0;
	cmd->params.verext.version_str_sel = version_str_sel;
}

void wifi_prepare_get_value1(HostCmd_DS_COMMAND *cmd,
				   int seq_number)
{
	cmd->command = HostCmd_CMD_MAC_REG_ACCESS;
	cmd->command = wlan_cpu_to_le16(cmd->command);
	cmd->size = wlan_cpu_to_le16(sizeof(HostCmd_DS_MAC_REG_ACCESS) + S_DS_GEN);
	cmd->seq_num = 0x0;
	cmd->result = 0x0;
	cmd->params.mac_reg.action = wlan_cpu_to_le16(HostCmd_ACT_GEN_GET);
	cmd->params.mac_reg.offset = wlan_cpu_to_le16((t_u16)WLAN_VALUE1);
	cmd->params.mac_reg.value = 0x0;
}

void wifi_prepare_set_mac_addr_cmd(HostCmd_DS_COMMAND *cmd,
				   int seq_number)
{
	cmd->command = HostCmd_CMD_802_11_MAC_ADDRESS;
	cmd->size = sizeof(HostCmd_DS_802_11_MAC_ADDRESS) + S_DS_GEN;
	cmd->seq_num = seq_number;
	cmd->result = 0;
	cmd->params.mac_addr.action = HostCmd_ACT_GEN_SET;
	memcpy(cmd->params.mac_addr.mac_addr, mac_addr, MLAN_MAC_ADDR_LENGTH);
}

#ifdef CONFIG_EXTERNAL_BLE_COEX
void wifi_prepare_set_coex_cmd(HostCmd_DS_COMMAND *cmd,
				   int seq_number)
{
	cmd->command = HostCmd_CMD_ROBUST_COEX;
	cmd->size = sizeof(MrvlIETypes_ExtBLECoex_Config_t) + S_DS_GEN;
	cmd->seq_num = seq_number;
	cmd->result = 0;
	cmd->params.ext_ble_coex_cfg.action = HostCmd_ACT_GEN_SET;
	cmd->params.ext_ble_coex_cfg.reserved = 0;
	cmd->params.ext_ble_coex_cfg.IEParam.type = TLV_TYPE_EXT_BLE_COEX_CFG;
	cmd->params.ext_ble_coex_cfg.IEParam.len = sizeof(MrvlIETypes_ExtBLECoex_Config_t) - 8;
	cmd->params.ext_ble_coex_cfg.Enabled = 1;
	cmd->params.ext_ble_coex_cfg.IgnorePriority = 0;
	cmd->params.ext_ble_coex_cfg.DefaultPriority = 0;
	cmd->params.ext_ble_coex_cfg.BLE_EIP_Input_GPIO_num = 0;
	cmd->params.ext_ble_coex_cfg.BLE_EIP_Input_GPIO_polarity = 1;
	cmd->params.ext_ble_coex_cfg.BLE_Pri_Input_GPIO_num = 1;
	cmd->params.ext_ble_coex_cfg.BLE_Pri_Input_GPIO_polarity = 1;
	cmd->params.ext_ble_coex_cfg.WLAN_EIP_Output_GPIO_num = 10;
	cmd->params.ext_ble_coex_cfg.WLAN_EIP_Output_GPIO_polarity = 1;
	cmd->params.ext_ble_coex_cfg.WLAN_Time = 200;
	cmd->params.ext_ble_coex_cfg.BT_Time = 255;
}
#endif

void wifi_prepare_txpwrlimit_data_cmd(HostCmd_DS_COMMAND *cmd,
				   int seq_number)
{
	int i, j;

	cmd->command = HostCmd_CMD_CHANNEL_TRPC_CONFIG;
	cmd->size = sizeof(HostCmd_DS_CHAN_TRPC_CONFIG) + S_DS_GEN
			- ((14 - txpwrlimit_no_of_channels) *
			sizeof(MrvlIEtypes_ChanTrpcCfg_t));
	cmd->seq_num = seq_number;
	cmd->result = 0;
	cmd->params.chan_trpc_cfg.action = HostCmd_ACT_GEN_SET;
	for (i = 0; i < txpwrlimit_no_of_channels; i++) {
		cmd->params.chan_trpc_cfg.ChanTrpcG.ChanTrpc[i].header.type =
				TLV_TYPE_CHANNEL_TRPC_CONFIG;
		cmd->params.chan_trpc_cfg.ChanTrpcG.ChanTrpc[i].header.len =
			sizeof(MrvlChannelDesc_t) +
			(MOD_GROUPS * sizeof(MrvlChanTrpcEntry_t));
		cmd->params.chan_trpc_cfg.ChanTrpcG.ChanTrpc[i].
			chanDesc.startFreq = 2407;
		cmd->params.chan_trpc_cfg.ChanTrpcG.ChanTrpc[i].
			chanDesc.chanWidth = 20;
		cmd->params.chan_trpc_cfg.ChanTrpcG.ChanTrpc[i].
			chanDesc.chanNum =
			*((uint8_t *)txpwrlimit_data +
				i * (MOD_GROUPS + 1) + 0);
		for (j = 0; j < MOD_GROUPS; j++) {
			cmd->params.chan_trpc_cfg.ChanTrpcG.ChanTrpc[i].
				chanTrpcEntry[j].modGroup = j;
			cmd->params.chan_trpc_cfg.ChanTrpcG.ChanTrpc[i].
				chanTrpcEntry[j].txPower =
				*((uint8_t *)txpwrlimit_data +
				i * (MOD_GROUPS + 1) + j + 1);
		}
	}
}

void wifi_prepare_enable_amsdu_cmd(HostCmd_DS_COMMAND *cmd,
				   int seq_number)
{
	cmd->command = HostCmd_CMD_AMSDU_AGGR_CTRL;
	cmd->size = sizeof(HostCmd_CMD_AMSDU_AGGR_CTRL) + 8;
	cmd->seq_num = seq_number;
	cmd->result = 0;
	cmd->params.amsdu_aggr_ctrl.action = HostCmd_ACT_GEN_SET;
	cmd->params.amsdu_aggr_ctrl.enable = 0x1;
	cmd->params.amsdu_aggr_ctrl.curr_buf_size = 0x0;
}

#ifdef CONFIG_WiFi_8801
void wifi_prepare_low_power_mode_cmd(HostCmd_DS_COMMAND *cmd, int seq_number)
{
	cmd->command = HostCmd_CMD_LOW_POWER_MODE;
	cmd->size = sizeof(HostCmd_CONFIG_LOW_PWR_MODE) + S_DS_GEN;
	cmd->seq_num = seq_number;
	cmd->result = 0;

	cmd->params.low_pwr_mode_cfg.enable = 1;

	return;
}
#endif

void wifi_prepare_ed_mac_mode_cmd(HostCmd_DS_COMMAND *cmd, int seq_number)
{
	cmd->command = HostCmd_CMD_ED_MAC_MODE;
	cmd->size = sizeof(HostCmd_CONFIG_ED_MAC_MODE) + S_DS_GEN;
	cmd->seq_num = seq_number;
	cmd->result = 0;

	cmd->params.ed_mac_mode_cfg.enable = 1;
	cmd->params.ed_mac_mode_cfg.offset = ed_mac_offset;

	return;
}

void wlan_prepare_mac_control_cmd(HostCmd_DS_COMMAND *cmd,
			  int seq_number)
{
	cmd->command = HostCmd_CMD_MAC_CONTROL;
	cmd->size = sizeof(HostCmd_DS_MAC_CONTROL) + S_DS_GEN;
	cmd->seq_num = seq_number;
	cmd->result = 0;

	cmd->params.mac_ctrl.action =
	    HostCmd_ACT_MAC_RX_ON | HostCmd_ACT_MAC_TX_ON |
	    HostCmd_ACT_MAC_ETHERNETII_ENABLE | HostCmd_ACT_MAC_WEP_ENABLE;

	return;
}

void wifi_set_cal_data(uint8_t *cdata, unsigned int clen)
{
	cal_data = cdata;
	cal_data_len = clen;
	cal_data_valid = true;
}

void wifi_set_mac_addr(uint8_t *mac)
{
	mac_addr = mac;
	mac_addr_valid = true;
}

void wifi_set_txpwrlimit_data(const uint8_t country, const uint8_t channels,
		const uint8_t **data)
{
	trpc_country = country;
	txpwrlimit_no_of_channels = channels;
	txpwrlimit_data = (uint8_t **) data;
	txpwrlimit_data_valid = true;
}

void _wifi_set_mac_addr(uint8_t *mac)
{
	wifi_get_command_lock();
	HostCmd_DS_COMMAND *cmd = wifi_get_command_buffer();
	memset(cmd, 0x00, sizeof(HostCmd_DS_COMMAND));
	cmd->seq_num = HostCmd_SET_SEQ_NO_BSS_INFO(
						   0 /* seq_num */,
						   0 /* bss_num */,
						   MLAN_BSS_TYPE_UAP);

	cmd->result = 0x0;
	wlan_ops_sta_prepare_cmd((mlan_private *) mlan_adap->priv[0],
					 HostCmd_CMD_802_11_MAC_ADDRESS,
					 HostCmd_ACT_GEN_SET,
					 0,
					 NULL, mac, cmd);
	wifi_wait_for_cmdresp(NULL);
}

#ifdef CONFIG_WiFi_8801
void wifi_enable_low_pwr_mode()
{
	low_power_mode = true;
}
#endif

void wifi_enable_ed_mac_mode(short offset)
{
	ed_mac_mode = true;
	ed_mac_offset = offset;
}

/**
 *  @brief This function get pxpd info for radiotap info
 *
 *  @param prx_pd   A pointer to RxPD
 *  @param prt_info   A pointer to radiotap_info
 *
 *  @return        N/A
 */
void wrapper_wlan_rxpdinfo_to_radiotapinfo(RxPD *prx_pd, radiotap_info *prt_info)
{
	mlan_private *priv = (mlan_private *) mlan_adap->priv[0];
	radiotap_info rt_info_tmp;
	t_u8 rx_rate_info = 0;
	t_u8 mcs_index = 0;
	t_u8 format = 0;
	t_u8 bw = 0;
	t_u8 gi = 0;
	t_u8 ldpc = 0;

	memset(&rt_info_tmp, 0x00, sizeof(rt_info_tmp));

	if (prx_pd->flags & RXPD_FLAG_EXTRA_HEADER)
		memcpy(&rt_info_tmp.extra_info, (t_u8 *)prx_pd + sizeof(*prx_pd),
			sizeof(rt_info_tmp.extra_info));

	rt_info_tmp.snr = prx_pd->snr;
	rt_info_tmp.nf = prx_pd->nf;
	rt_info_tmp.band_config = prx_pd->band_config;
	rt_info_tmp.chan_num = prx_pd->chan_num;

	rt_info_tmp.antenna = prx_pd->antenna;
	rx_rate_info = wlan_convert_v14_rate_ht_info(prx_pd->ht_info);

	if((rx_rate_info & 0x3) == MLAN_RATE_FORMAT_HT) {
		/* HT rate */
		format = MLAN_RATE_FORMAT_HT;
		mcs_index = prx_pd->rx_rate;
		/* 20M: bw=0, 40M: bw=1 */
		bw = (rx_rate_info & 0xC) >> 2;
		/* LGI: gi =0, SGI: gi = 1 */
		gi = (rx_rate_info & 0x10) >> 4;
	} else {
		/* LG rate */
		format = MLAN_RATE_FORMAT_LG;
		mcs_index = (prx_pd->rx_rate > MLAN_RATE_INDEX_OFDM0) ?
			prx_pd->rx_rate - 1 : prx_pd->rx_rate;
	}
	ldpc = rx_rate_info & 0x40;

	rt_info_tmp.rate_info.mcs_index = mcs_index;
	rt_info_tmp.rate_info.rate_info = (ldpc<<5) | (format<<3) | (bw<<1) | gi;
	rt_info_tmp.rate_info.bitrate =
	wlan_index_to_data_rate(priv->adapter, prx_pd->rx_rate, prx_pd->ht_info);

	memset(prt_info, 0x00, sizeof(*prt_info));
	memcpy(prt_info, &rt_info_tmp, sizeof(*prt_info));

	return;
}
/*
void wrapper_wlan_radiotapinfo_to_radiotap_hdr(const t_u8 *payload,
			const radiotap_info *prt_info,
			radiotap_header_t *prth)
{
	struct ieee80211_hdr *dot11_hdr = NULL;
	t_u8 format = 0;
	t_u8 bw = 0;
	t_u8 gi = 0;
	t_u8 ldpc = 0;
	t_u8 chan_num;
	t_u8 band = 0;

	memset(prth, 0x00, sizeof(*prth));

	dot11_hdr = (struct ieee80211_hdr *)(payload);

	ldpc = (prt_info->rate_info.rate_info & 0x20) >> 5;
	format = (prt_info->rate_info.rate_info & 0x18) >> 3;
	bw = (prt_info->rate_info.rate_info & 0x06) >> 1;
	gi = prt_info->rate_info.rate_info & 0x01;

	prth->hdr.it_version = PKTHDR_RADIOTAP_VERSION;
	prth->hdr.it_pad = 0;
	prth->hdr.it_len = wlan_cpu_to_le16(sizeof(*prth));
	prth->hdr.it_present =
		wlan_cpu_to_le32((1 << IEEE80211_RADIOTAP_TSFT) |
				(1 << IEEE80211_RADIOTAP_FLAGS) |
				(1 << IEEE80211_RADIOTAP_CHANNEL) |
				(1 << IEEE80211_RADIOTAP_DBM_ANTSIGNAL) |
				(1 << IEEE80211_RADIOTAP_DBM_ANTNOISE) |
				(1 << IEEE80211_RADIOTAP_ANTENNA));
	//Timstamp
	prth->body.timestamp = wlan_cpu_to_le64(os_get_timestamp());
	//Flags
	prth->body.flags = (prt_info->extra_info.flags &
			~(RADIOTAP_FLAGS_USE_SGI_HT |
				RADIOTAP_FLAGS_WITH_FRAGMENT |
				RADIOTAP_FLAGS_WEP_ENCRYPTION |
				RADIOTAP_FLAGS_FAILED_FCS_CHECK));
	//reverse fail fcs, 1 means pass FCS in FW, but means fail FCS in radiotap
	prth->body.flags |= (~prt_info->extra_info.flags) & RADIOTAP_FLAGS_FAILED_FCS_CHECK;
	if ((format == MLAN_RATE_FORMAT_HT) && (gi == 1))
		prth->body.flags |= RADIOTAP_FLAGS_USE_SGI_HT;
	if (ieee80211_is_mgmt(dot11_hdr->frame_control) ||
			ieee80211_is_data(dot11_hdr->frame_control)) {
		if ((ieee80211_has_morefrags(dot11_hdr->frame_control)) ||
				(!ieee80211_is_first_frag(dot11_hdr->seq_ctrl))) {
			prth->body.flags |= RADIOTAP_FLAGS_WITH_FRAGMENT;
		}
	}
	if (ieee80211_is_data(dot11_hdr->frame_control) &&
			ieee80211_has_protected(dot11_hdr->frame_control)) {
		payload = (t_u8 *)dot11_hdr + ieee80211_hdrlen(dot11_hdr->frame_control);
		if (!(*(payload + 3) & 0x20))//ExtIV bit shall be 0 for WEP frame
			prth->body.flags |= RADIOTAP_FLAGS_WEP_ENCRYPTION;
	}
	//Rate, t_u8 only apply for LG mode
	if (format == MLAN_RATE_FORMAT_LG) {
		prth->hdr.it_present |= wlan_cpu_to_le32(1 << IEEE80211_RADIOTAP_RATE);
		prth->body.rate = prt_info->rate_info.bitrate;
	}
	//Channel
	prth->body.channel.flags = 0;
	if (prt_info->chan_num)
		chan_num = prt_info->chan_num;
	else
		chan_num = wm_wifi.chan_num;
	band = (chan_num <= 14) ? IEEE80211_BAND_2GHZ : IEEE80211_BAND_5GHZ;
	prth->body.channel.frequency =
		wlan_cpu_to_le16(ieee80211_channel_to_frequency(chan_num, band));
	prth->body.channel.flags |=
		wlan_cpu_to_le16((band == IEEE80211_BAND_2GHZ) ? CHANNEL_FLAGS_2GHZ : CHANNEL_FLAGS_5GHZ);
	if (prth->body.channel.flags & wlan_cpu_to_le16(CHANNEL_FLAGS_2GHZ))
		prth->body.channel.flags |= wlan_cpu_to_le16(CHANNEL_FLAGS_DYNAMIC_CCK_OFDM);
	else
		prth->body.channel.flags |= wlan_cpu_to_le16(CHANNEL_FLAGS_OFDM);
	if (wm_wifi.chan_num &&
		prt_info->extra_info.channel_flags & MBIT(9))
		prth->body.channel.flags |= wlan_cpu_to_le16(CHANNEL_FLAGS_ONLY_PASSIVSCAN_ALLOW);
	//Antenna
	prth->body.antenna_signal = -(prt_info->nf - prt_info->snr);
	prth->body.antenna_noise = -prt_info->nf;
	prth->body.antenna = prt_info->antenna;
	//MCS
	if (format == MLAN_RATE_FORMAT_HT) {
		prth->hdr.it_present |= wlan_cpu_to_le32(1 << IEEE80211_RADIOTAP_MCS);
		prth->body.mcs.known = prt_info->extra_info.mcs_known;
		prth->body.mcs.flags = prt_info->extra_info.mcs_flags;
		//MCS mcs
		prth->body.mcs.known |= MCS_KNOWN_MCS_INDEX_KNOWN;
		prth->body.mcs.mcs = prt_info->rate_info.mcs_index;
		//MCS bw
		prth->body.mcs.known |= MCS_KNOWN_BANDWIDTH;
		prth->body.mcs.flags &= ~(0x03);//Clear, 20MHz as default
		if (bw == 1)
			prth->body.mcs.flags |= RX_BW_40;
		//MCS gi
		prth->body.mcs.known |= MCS_KNOWN_GUARD_INTERVAL;
		prth->body.mcs.flags &= ~(1<<2);
		if (gi)
			prth->body.mcs.flags |= gi<<2;
		//MCS FEC
		prth->body.mcs.known |= MCS_KNOWN_FEC_TYPE;
		prth->body.mcs.flags &= ~(1<<4);
		if (ldpc)
			prth->body.mcs.flags |= ldpc<<4;
	}
}*/
