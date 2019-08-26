/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 *
 * Fundamental types and constants relating to WFA NAN
 * (Neighbor Awareness Networking)
 *
 * $Id$
 */
#ifndef _NAN_H_
#define _NAN_H_

#include <typedefs.h>
#include <proto/802.11.h>

/* XXX: Do we want to include p2p.h for constants like P2P_WFDS_HASH_LEN and
 * maybe P2P_WFDS_MAX_SVC_NAME_LEN etc.?
 */

/* This marks the start of a packed structure section. */
#include <packed_section_start.h>

/* WiFi NAN OUI values */
#define NAN_OUI            WFA_OUI     /* WiFi OUI */
/* For oui_type field identifying the type and version of the NAN IE. */
#define NAN_OUI_TYPE       0x13        /* Type/Version */
/* IEEE 802.11 vendor specific information element. (Same as P2P_IE_ID.) */
#define NAN_IE_ID          0xdd

/* Same as P2P_PUB_AF_CATEGORY and DOT11_ACTION_CAT_PUBLIC */
#define NAN_PUB_AF_CATEGORY     0x04
/* IEEE 802.11 Public Action Frame Vendor Specific. (Same as P2P_PUB_AF_ACTION.) */
#define NAN_PUB_AF_ACTION       0x09
/* Number of octents in hash of service name. (Same as P2P_WFDS_HASH_LEN.) */
#define NAN_SVC_HASH_LEN    6
/* Size of fixed length part of nan_pub_act_frame_t before attributes. */
#define NAN_PUB_ACT_FRAME_FIXED_LEN 6
/* Number of octents in master rank value. */
#define NAN_MASTER_RANK_LEN     8
/* NAN public action frame header size */
#define NAN_PUB_ACT_FRAME_HDR_SIZE (OFFSETOF(nan_pub_act_frame_t, data))
/* NAN network ID */
#define NAN_NETWORK_ID		"\x51\x6F\x9A\x01\x00\x00"
/* Service Control Type length */
#define NAN_SVC_CONTROL_TYPE_LEN	2

/* Attribute TLV header size */
#define NAN_ATTR_ID_OFF		0
#define NAN_ATTR_LEN_OFF	1
#define NAN_ATTR_DATA_OFF	3

#define NAN_ATTR_ID_LEN		1	/* ID field length */
#define NAN_ATTR_LEN_LEN	2	/* Length field length */
#define NAN_ATTR_HDR_LEN	3	/* ID + 2-byte length field */

/* Vendor-specific public action frame for NAN */
typedef BWL_PRE_PACKED_STRUCT struct nan_pub_act_frame_s {
	/* NAN_PUB_AF_CATEGORY 0x04 */
	uint8 category_id;
	/* NAN_PUB_AF_ACTION 0x09 */
	uint8 action_field;
	/* NAN_OUI 0x50-6F-9A */
	uint8 oui[DOT11_OUI_LEN];
	/* NAN_OUI_TYPE 0x13 */
	uint8 oui_type;
	/* One or more NAN Attributes follow */
	uint8 data[1];
} BWL_POST_PACKED_STRUCT nan_pub_act_frame_t;

/* NAN attributes as defined in the nan spec */
enum {
	NAN_ATTR_MASTER_IND = 0,
	NAN_ATTR_CLUSTER = 1,
	NAN_ATTR_SVC_ID_LIST = 2,
	NAN_ATTR_SVC_DESCRIPTOR = 3,
	NAN_ATTR_CONN_CAP = 4,
	NAN_ATTR_INFRA = 5,
	NAN_ATTR_P2P = 6,
	NAN_ATTR_IBSS = 7,
	NAN_ATTR_MESH = 8,
	NAN_ATTR_FURTHER_NAN_SD = 9,
	NAN_ATTR_FURTHER_AVAIL = 10,
	NAN_ATTR_COUNTRY_CODE = 11,
	NAN_ATTR_RANGING = 12,
	NAN_ATTR_VENDOR_SPECIFIC = 221
};

typedef BWL_PRE_PACKED_STRUCT struct wifi_nan_ie_s {
	uint8	id;		/* IE ID: NAN_IE_ID 0xDD */
	uint8	len;		/* IE length */
	uint8	oui[DOT11_OUI_LEN]; /* NAN_OUI 50:6F:9A */
	uint8	oui_type;	/* NAN_OUI_TYPE 0x13 */
	uint8	attr[1];	/* var len attributes */
} BWL_POST_PACKED_STRUCT wifi_nan_ie_t;

#define NAN_IE_HDR_SIZE	(OFFSETOF(wifi_nan_ie_t, attr))

/* master indication record  */
typedef BWL_PRE_PACKED_STRUCT struct wifi_nan_master_ind_attr_s {
	uint8	id;
	uint16	len;
	uint8	master_preference;
	uint8	random_factor;
} BWL_POST_PACKED_STRUCT wifi_nan_master_ind_attr_t;

/* cluster attr record  */
typedef BWL_PRE_PACKED_STRUCT struct wifi_nan_cluster_attr_s {
	uint8	id;
	uint16	len;
	uint8   amr[NAN_MASTER_RANK_LEN];
	uint8   hop_count;
	/* Anchor Master Beacon Transmission Time */
	uint32  ambtt;
} BWL_POST_PACKED_STRUCT wifi_nan_cluster_attr_t;

/*  container for service ID records  */
typedef BWL_PRE_PACKED_STRUCT struct wifi_nan_svc_id_attr_s {
	uint8	id;
	uint16	len;
	uint8	svcid[NAN_SVC_HASH_LEN]; /* 6*len of srvc IDs */
} BWL_POST_PACKED_STRUCT wifi_nan_svc_id_attr_t;

/* service_control bitmap for wifi_nan_svc_descriptor_attr_t below */
#define NAN_SC_PUBLISH 0x0
#define NAN_SC_SUBSCRIBE 0x1
#define NAN_SC_FOLLOWUP 0x2
/* Set to 1 if a Matching Filter field is included in descriptors. */
#define NAN_SC_MATCHING_FILTER_PRESENT 0x8
/* Set to 1 if a Service Response Filter field is included in descriptors. */
#define NAN_SC_SR_FILTER_PRESENT 0x10
/* Set to 1 if a Service Info field is included in descriptors. */
#define NAN_SC_SVC_INFO_PRESENT 0x20
/* range is close proximity only */
#define NAN_SC_RANGE_LIMITED 0x40

/* Service descriptor */
typedef BWL_PRE_PACKED_STRUCT struct wifi_nan_svc_descriptor_attr_s {
	/* Attribute ID - 0x03. */
	uint8 id;
	/* Length of the following fields in the attribute */
	uint16 len;
	/* Hash of the Service Name */
	uint8 svc_hash[NAN_SVC_HASH_LEN];
	/* Publish or subscribe instance id */
	uint8 instance_id;
	/* Requestor Instance ID */
	uint8 requestor_id;
	/* Service Control Bitmask. Also determines what data follows. */
	uint8 svc_control;
	/* Optional fields follow */
} BWL_POST_PACKED_STRUCT wifi_nan_svc_descriptor_attr_t;

/* IBSS attribute */
typedef BWL_PRE_PACKED_STRUCT struct wifi_nan_ibss_attr_s {
	/* Attribute ID - 0x07. */
	uint8 id;
	/* Length of the following fields in the attribute */
	uint16 len;
	/* BSSID of the ibss */
	struct ether_addr bssid;
	/*
	 map control:, bits:
	[0-3]: Id for associated further avail map attribute
	[4-5]: avail interval duration: 0:16ms; 1:32ms; 2:64ms; 3:reserved
	[6] : repeat : 0 - applies to next DW, 1: 16 intervals max? wtf?
	[7] : reserved
	*/
	uint8 map_ctrl;
	/* avail. intervals bitmap, var len  */
	uint8 avail_bmp[1];
} BWL_POST_PACKED_STRUCT wifi_nan_ibss_attr_t;

/* Further Availability MAP attr  */
typedef BWL_PRE_PACKED_STRUCT struct wifi_nan_favail_attr_s {
	/* Attribute ID - 0x0A. */
	uint8 id;
	/* Length of the following fields in the attribute */
	uint16 len;
	/* MAP id: val [0..15], values[16-255] reserved */
	uint8 map_id;
	/*  availibility entry, var len */
	uint8 avil_entry[1];
} BWL_POST_PACKED_STRUCT wifi_nan_favail_attr_t;

/* Further Availability MAP attr  */
typedef BWL_PRE_PACKED_STRUCT struct wifi_nan_avail_entry_s {
	/*
	 entry control
	 [0-1]: avail interval duration: 0:16ms; 1:32ms; 2:64ms;
	 [2:7] reserved
	*/
	uint8 entry_ctrl;
	/* operating class: freq band etc IEEE 802.11 */
	uint8 opclass;
	/* channel number */
	uint8 chan;
	uint8 map_id;
	/*  avail bmp, var len */
	uint8 avail_bmp[1];
} BWL_POST_PACKED_STRUCT wifi_nan_avail_entry_t;

/* Map control Field */
#define NAN_MAPCTRL_IDMASK	0x7
#define NAN_MAPCTRL_DURSHIFT	3
#define NAN_MAPCTRL_REPEAT	0x40

#define NAN_VENDOR_TYPE_RTT	0

/* Vendor Specific Attribute */
typedef BWL_PRE_PACKED_STRUCT struct wifi_nan_vendor_attr_s {
	uint8	id;		/* 0xDD */
	uint16	len;		/* IE length */
	uint8	oui[DOT11_OUI_LEN]; /* 00-90-4C */
	uint8	type;		/* attribute type */
	uint8	attr[1];	/* var len attributes */
} BWL_POST_PACKED_STRUCT wifi_nan_vendor_attr_t;

#define NAN_VENDOR_HDR_SIZE	(OFFSETOF(wifi_nan_vendor_attr_t, attr))

/* This marks the end of a packed structure section. */
#include <packed_section_end.h>

#endif /* _NAN_H_ */
