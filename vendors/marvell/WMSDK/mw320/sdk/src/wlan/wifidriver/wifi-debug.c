/** @file wifi-debug.c
 *
 *  @brief This file provides WIFI debug APIs.
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
#include <wmstdio.h>
#include <wm_os.h>

#include <wifi-debug.h>

#ifdef CONFIG_WIFI_EXTRA_DEBUG
static const char * const status_table[] = {
[0] = "Successful",
	[1] = "Unspecified failure",
	[10] = "Cannot support all requested capabilities in the capability"
	"information field",
	[11] = "Reassociation denied due to inability to confirm that "
	"association exists",
	[12] = "Association denied due to reason outside the scope of "
	"this standard",
	[13] = "Responding station does not support the specified "
	"authentication algorithm",
	[14] = "Received an authentication frame with authentication "
	"transaction sequence number out of expected sequence",
	[15] = "Authentication rejected because of challenge failure",
	[16] = "Authentication rejected due to timeout waiting for next "
	"frame in sequence",
	[17] = "Association denied because AP is unable to handle additional "
	"associated STA",
	[18] = "Association denied due to requesting station not supporting "
	"all of the data rates in the BSSBasicRateSet parameter",
	[19] = "Association denied due to requesting station not supporting "
	"the short preamble option",
	[20] = "Association denied due to requesting station not supporting "
	"the PBCC modulation option",
	[21] = "Association denied due to requesting station not supporting "
	"the channel agility option",
	[22] = "Association request rejected because Spectrum Management "
	"capability is required",
	[23] = "Association request rejected because the information in the "
	"Power Capability element is unacceptable",
	[24] = "Association request rejected because the information in the "
	"Supported Channels element is unacceptable",
	[25] = "Association request rejected due to requesting station not "
	"supporting the short slot time option",
	[26] = "Association request rejected due to requesting station not "
	"supporting the ER-PBCC modulation option",
	[27] = "Association denied due to requesting STA not supporting "
	"HT features",
	[28] = "R0KH Unreachable",
	[29] = "Association denied because the requesting STA does not "
	"support the PCO transition required by the AP",
	[30] = "Association request rejected temporarily; try again later",
	[31] = "Robust Management frame policy violation",
	[32] = "Unspecified, QoS related failure",
	[33] = "Association denied due to QAP having insufficient bandwidth "
	"to handle another QSTA",
	[34] = "Association denied due to poor channel conditions",
	[35] = "Association (with QBSS) denied due to requesting station "
	"not supporting the QoS facility",
	[37] = "The request has been declined",
	[38] = "The request has not been successful as one or more "
	"parameters have invalid values",
	[39] = "The TS has not been created because the request cannot be "
	"honored. However, a suggested Tspec is provided so that the "
	"initiating QSTA may attempt to send another TS with the suggested "
	"changes to the TSpec",
	[40] = "Invalid Information Element",
	[41] = "Group Cipher is not valid",
	[42] = "Pairwise Cipher is not valid",
	[43] = "AKMP is not valid",
	[44] = "Unsupported RSN IE version",
	[45] = "Invalid RSN IE Capabilities",
	[46] = "Cipher suite is rejected per security policy",
	[47] = "The TS has not been created. However, the HC may be capable "
	"of creating a TS, in response to a request, after the time "
	"indicated in the TS Delay element",
	[48] = "Direct link is not allowed in the BSS by policy",
	[49] = "Destination STA is not present within this QBSS",
	[50] = "The destination STA is not a QSTA",
	[51] = "Association denied because Listen Interval is too large",
	[52] = "Invalid Fast BSS Transition Action Frame Count",
	[53] = "Invalid PMKID",
	[54] = "Invalid MDIE",
	[55] = "Invalid FTIE",
	};

const char *get_status_str(uint16_t status)
{
	if (status < sizeof(status_table)/sizeof(status_table[0])
	    && status_table[status])
		return status_table[status];
	return "<unknown>";
}
#endif /* CONFIG_WIFI_EXTRA_DEBUG */

void wifi_show_assoc_fail_reason(int status)
{
#ifdef CONFIG_WIFI_EXTRA_DEBUG
	wmprintf("[wifi] Assoc Status: %s\n\r", get_status_str(status));

	switch (status) {
	case 13:
		wmprintf("Assoc failed: Network not found.\n\r");
		break;
	case 18:
		wmprintf("(11n disabled ?)\n\r");
		break;
	}
#endif /* CONFIG_WIFI_EXTRA_DEBUG */
}

#ifdef DUMP_PACKET_MAC
void dump_mac_addr(const char *msg, unsigned char *addr)
{
	if (msg)
		wmprintf("%s: ", msg);
	else
		wmprintf("mac: ", msg);

	int i;
	for (i = 0; i < MLAN_MAC_ADDR_LENGTH; i++) {
		wmprintf("%x", addr[i]);
		if (i != (MLAN_MAC_ADDR_LENGTH - 1))
			wmprintf(":");
	}

	wmprintf("\n\r");
}
#endif /* DUMP_PACKET_MAC */

#ifdef DEBUG_11N_AGGR
void dump_packet_header(const HostCmd_DS_COMMAND *cmd)
{
	wmprintf("Command ----- : %x\n\r", cmd->command);
	wmprintf("Size -------- : %d\n\r", cmd->size);
	wmprintf("Sequence ---- : %d\n\r", cmd->seq_num);
	wmprintf("Result ------ : %x\n\r", cmd->result);
}
#endif /* DEBUG_11N_AGGR */

#ifdef DEBUG_11N_AGGR
void dump_addba_req_rsp_packet(const HostCmd_DS_COMMAND *cmd)
{
	dump_packet_header(cmd);

	HostCmd_DS_11N_ADDBA_RSP *padd_ba_rsp =
	    (HostCmd_DS_11N_ADDBA_RSP *) &cmd->params.add_ba_rsp;
	wmprintf("add_rsp_result: %x\n\r", padd_ba_rsp->add_rsp_result);
	wmprintf("peer mac address: ");
	int i;
	for (i = 0; i < MLAN_MAC_ADDR_LENGTH; i++)
		wmprintf("%x ", padd_ba_rsp->peer_mac_addr[i]);
	wmprintf("\n\r");
	wmprintf("Dialog token: %x\n\r", padd_ba_rsp->dialog_token);
	wmprintf("Status code: %x\n\r", padd_ba_rsp->status_code);
	wmprintf("block_ack_param_set: %x\n\r",
		 padd_ba_rsp->block_ack_param_set);
	if (padd_ba_rsp->block_ack_param_set & 0x1)
		wmprintf("         AMSDU supported\n\r");
	else
		wmprintf("         AMSDU _NOT_ supported\n\r");
	if (padd_ba_rsp->block_ack_param_set & 0x2)
		wmprintf("         Immediate block ack\n\r");
	else
		wmprintf("         Delayed block ack\n\r");
	wmprintf("         TID: %d\n\r",
		 (padd_ba_rsp->block_ack_param_set & 0x3C) >> 2);
	wmprintf("         Buffer size: %d\n\r",
		 (padd_ba_rsp->block_ack_param_set & 0xFFC) >> 6);
	wmprintf("block_ack_tmo: %x\n\r", padd_ba_rsp->block_ack_tmo);
	wmprintf("ssn: %x\n\r", padd_ba_rsp->ssn);
	wmprintf("*************************\n\r");
}
#endif /* DEBUG_11N_AGGR */

#ifdef DEBUG_11N_ASSOC
void dump_htcap_info(const MrvlIETypes_HTCap_t *htcap)
{
	wmprintf("******* HT Cap ******\n\r");
	wmprintf("Type: %d\n\r", htcap->header.type);
	wmprintf("Len: %d\n\r", htcap->header.len);
	wmprintf("ht cap info: 0%x\n\r", htcap->ht_cap.ht_cap_info);
	wmprintf("ampdu_param: 0%x\n\r", htcap->ht_cap.ampdu_param);
	wmprintf("supported_mcs_set: ");
	int i;
	for (i = 0; i < 16; i++)
		wmprintf("0x%x ", htcap->ht_cap.supported_mcs_set[i]);
	wmprintf("\n\r");
	wmprintf("ht_ext_cap: 0%x\n\r", htcap->ht_cap.ht_ext_cap);
	wmprintf("tx_bf_cap: 0x%x\n\r", htcap->ht_cap.tx_bf_cap);
	wmprintf("asel: 0x%x\n\r", htcap->ht_cap.asel);
}

void dump_ht_info(const MrvlIETypes_HTInfo_t *htinfo)
{
	wmprintf("******* HT Info ******\n\r");
	wmprintf("Type: %d\n\r", htinfo->header.type);
	wmprintf("Len: %d\n\r", htinfo->header.len);
	wmprintf("pri_chan: %d\n\r", htinfo->ht_info.pri_chan);
	wmprintf("field2: 0%x\n\r", htinfo->ht_info.field2);
	wmprintf("field3: 0%x\n\r", htinfo->ht_info.field3);
	wmprintf("field4: 0%x\n\r", htinfo->ht_info.field4);
	wmprintf("basic_mcs_set: ");
	int i;
	for (i = 0; i < 16; i++)
		wmprintf("0x%x ", htinfo->ht_info.basic_mcs_set[i]);
	wmprintf("\n\r");
}
#endif /* DEBUG_11N_ASSOC */
