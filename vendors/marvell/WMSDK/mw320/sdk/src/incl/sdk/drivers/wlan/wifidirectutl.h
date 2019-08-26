/** @file  wifidirectutl.h
 *
 *  @brief Header file for wifidirectutl application
 *
 * (C) Copyright 2008-2017 Marvell International Ltd. All Rights Reserved
 *
 * MARVELL CONFIDENTIAL
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Marvell International Ltd or its
 * suppliers or licensors. Title to the Material remains with
 * Marvell International Ltd
 * or its suppliers and licensors. The Material contains trade secrets and
 * proprietary and confidential information of Marvell or its suppliers and
 * licensors. The Material is protected by worldwide copyright and trade secret
 * laws and treaty provisions. No part of the Material may be used, copied,
 * reproduced, modified, published, uploaded, posted, transmitted, distributed,
 * or disclosed in any way without Marvell's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Marvell in writing.
 *
 */
/************************************************************************
Change log:
    07/10/09: Initial creation
************************************************************************/
#ifndef _WIFIDIRECT_H
#define _WIFIDIRECT_H

typedef unsigned char t_u8;
typedef unsigned short t_u16;
typedef unsigned int t_u32;
typedef unsigned long long t_u64;
typedef char t_s8;
typedef unsigned short t_s16;
typedef unsigned int t_s32;
typedef unsigned long long t_s64;
typedef void t_void;

#if 0
/** Character, 1 byte */
typedef char t_s8;
/** Unsigned character, 1 byte */
typedef unsigned char t_u8;

/** Short integer */
typedef signed short t_s16;
/** Unsigned short integer */
typedef unsigned short t_u16;

/** Integer */
typedef signed int t_s32;
/** Unsigned integer */
typedef unsigned int t_u32;

/** Long long integer */
typedef signed long long t_s64;
/** Unsigned long integer */
typedef unsigned long long t_u64;

/** Device name */
extern char dev_name[IFNAMSIZ + 1];
/** Option for cmd */
extern struct option cmd_options[2];
#endif
#if (BYTE_ORDER == LITTLE_ENDIAN)
#undef BIG_ENDIAN_SUPPORT
#endif

/** 16 bits byte swap */
#define swap_byte_16(x) \
	((t_u16)((((t_u16)(x) & 0x00ffU) << 8) | \
		(((t_u16)(x) & 0xff00U) >> 8)))

/** 32 bits byte swap */
#define swap_byte_32(x) \
	((t_u32)((((t_u32)(x) & 0x000000ffUL) << 24) | \
		(((t_u32)(x) & 0x0000ff00UL) <<  8) | \
		(((t_u32)(x) & 0x00ff0000UL) >>  8) | \
		(((t_u32)(x) & 0xff000000UL) >> 24)))

/** 64 bits byte swap */
#define swap_byte_64(x) \
	((t_u64)((t_u64)(((t_u64)(x) & 0x00000000000000ffULL) << 56) | \
		(t_u64)(((t_u64)(x) & 0x000000000000ff00ULL) << 40) | \
		(t_u64)(((t_u64)(x) & 0x0000000000ff0000ULL) << 24) | \
		(t_u64)(((t_u64)(x) & 0x00000000ff000000ULL) <<  8) | \
		(t_u64)(((t_u64)(x) & 0x000000ff00000000ULL) >>  8) | \
		(t_u64)(((t_u64)(x) & 0x0000ff0000000000ULL) >> 24) | \
		(t_u64)(((t_u64)(x) & 0x00ff000000000000ULL) >> 40) | \
		(t_u64)(((t_u64)(x) & 0xff00000000000000ULL) >> 56)))

/** Set opp_ps by shifting 7 bits left */
#define SET_OPP_PS(x) ((x) << 7)

/** Get opp_ps by masking and shifting 7 bits right */
#define GET_OPP_PS(x) ((x) >> 7)

/** CT window mask from opp_ps_ct_window combination */
#define CT_WINDOW_MASK      0x7F
/** Invitation flag mask */
#define INVITATION_FLAG_MASK 0x01

#ifdef BIG_ENDIAN_SUPPORT
/** Convert from 16 bit little endian format to CPU format */
#define le16_to_cpu(x) swap_byte_16(x)
/** Convert from 32 bit little endian format to CPU format */
#define le32_to_cpu(x) swap_byte_32(x)
/** Convert from 64 bit little endian format to CPU format */
#define le64_to_cpu(x) swap_byte_64(x)
/** Convert to 16 bit little endian format from CPU format */
#define cpu_to_le16(x) swap_byte_16(x)
/** Convert to 32 bit little endian format from CPU format */
#define cpu_to_le32(x) swap_byte_32(x)
/** Convert to 64 bit little endian format from CPU format */
#define cpu_to_le64(x) swap_byte_64(x)

/** Convert WIFIDIRECTCMD header to little endian format from CPU format */
#define endian_convert_request_header(x)                \
{                                                   \
	(x)->cmd_code = cpu_to_le16((x)->cmd_code);   \
	(x)->size = cpu_to_le16((x)->size);         \
	(x)->seq_num = cpu_to_le16((x)->seq_num);     \
	(x)->result = cpu_to_le16((x)->result);     \
}

/** Convert WIFIDIRECTCMD header from little endian format to CPU format */
#define endian_convert_response_header(x)               \
{                                                   \
	(x)->cmd_code = le16_to_cpu((x)->cmd_code);   \
	(x)->size = le16_to_cpu((x)->size);         \
	(x)->seq_num = le16_to_cpu((x)->seq_num);     \
	(x)->result = le16_to_cpu((x)->result);     \
}

/** Convert WIFIDIRECT header to little endian format from CPU format */
#define endian_convert_tlv_wifidirect_header_out(x)            \
{                                               \
	(x)->length = cpu_to_le16((x)->length); \
}

/** Convert WIFIDIRECT header from little endian format to CPU format */
#define endian_convert_tlv_wifidirect_header_in(x)             \
{                                               \
	(x)->length = le16_to_cpu((x)->length); \
}

/** Convert TLV header to little endian format from CPU format */
#define endian_convert_tlv_header_out(x)            \
{                                               \
	(x)->tag = cpu_to_le16((x)->tag);       \
	(x)->length = cpu_to_le16((x)->length); \
}

/** Convert TLV header from little endian format to CPU format */
#define endian_convert_tlv_header_in(x)             \
{                                               \
	(x)->tag = le16_to_cpu((x)->tag);       \
	(x)->length = le16_to_cpu((x)->length); \
}

#else /* BIG_ENDIAN_SUPPORT */
/** Do nothing */
#define le16_to_cpu(x) x
/** Do nothing */
#define le32_to_cpu(x) x
/** Do nothing */
#define le64_to_cpu(x) x
/** Do nothing */
#define cpu_to_le16(x) x
/** Do nothing */
#define cpu_to_le32(x) x
/** Do nothing */
#define cpu_to_le64(x) x

/** Do nothing */
#define endian_convert_request_header(x)
/** Do nothing */
#define endian_convert_response_header(x)
/** Do nothing */
#define endian_convert_tlv_wifidirect_header_out(x)
/** Do nothing */
#define endian_convert_tlv_wifidirect_header_in(x)
/** Do nothing */
#define endian_convert_tlv_header_out(x)
/** Do nothing */
#define endian_convert_tlv_header_in(x)
#endif /* BIG_ENDIAN_SUPPORT */

/** Convert WPS TLV header to network order */
#define endian_convert_tlv_wps_header_out(x)            \
	{                                               \
		(x)->tag = htons((x)->tag);       \
		(x)->length = htons((x)->length); \
	}

/** Convert WPS TLV header from network to host order */
#define endian_convert_tlv_wps_header_in(t, l)             \
	{                                               \
		(t) = ntohs(t);       \
		(l) = ntohs(l);       \
	}

/** Private command ID to set/get custom IE buffer */
#define CUSTOM_IE               (SIOCDEVPRIVATE + 13)

/** TLV type ID definition */
#define PROPRIETARY_TLV_BASE_ID              0x0100
/** TLV: Management IE list */
/*#define MRVL_MGMT_IE_LIST_TLV_ID             (PROPRIETARY_TLV_BASE_ID + 0x69)   */
/** TLV: WifiDirect Discovery Period */
#define MRVL_WIFIDIRECT_DISC_PERIOD_TLV_ID          (PROPRIETARY_TLV_BASE_ID + 0x7c)
/** TLV: WifiDirect Scan Enable */
#define MRVL_WIFIDIRECT_SCAN_ENABLE_TLV_ID          (PROPRIETARY_TLV_BASE_ID + 0x7d)
/** TLV: WifiDirect Peer Device  */
#define MRVL_WIFIDIRECT_PEER_DEVICE_TLV_ID          (PROPRIETARY_TLV_BASE_ID + 0x7e)
/** TLV: WifiDirect Scan Request Peer Device  */
#define MRVL_WIFIDIRECT_SCAN_REQ_DEVICE_TLV_ID      (PROPRIETARY_TLV_BASE_ID + 0x7f)
/** TLV: WifiDirect Device State */
#define MRVL_WIFIDIRECT_DEVICE_STATE_TLV_ID         (PROPRIETARY_TLV_BASE_ID + 0x80)
/** TLV: WifiDirect Intent */
#define MRVL_WIFIDIRECT_INTENT_TLV_ID               (PROPRIETARY_TLV_BASE_ID + 0x81)
/** TLV: WifiDirect Capability */
#define MRVL_WIFIDIRECT_CAPABILITY_TLV_ID           (PROPRIETARY_TLV_BASE_ID + 0x82)
/** TLV: WifiDirect Notice of Absence */
#define MRVL_WIFIDIRECT_NOA_TLV_ID                  (PROPRIETARY_TLV_BASE_ID + 0x83)
/** TLV: WifiDirect Opportunistic Power Save */
#define MRVL_WIFIDIRECT_OPP_PS_TLV_ID               (PROPRIETARY_TLV_BASE_ID + 0x84)
/** TLV: WifiDirect Invitation list */
#define MRVL_WIFIDIRECT_INVITATION_LIST_TLV_ID      (PROPRIETARY_TLV_BASE_ID + 0x85)
/** TLV: WifiDirect Listen channel */
#define MRVL_WIFIDIRECT_LISTEN_CHANNEL_TLV_ID       (PROPRIETARY_TLV_BASE_ID + 0x86)
/** TLV: WifiDirect Operating Channel */
#define MRVL_WIFIDIRECT_OPERATING_CHANNEL_TLV_ID    (PROPRIETARY_TLV_BASE_ID + 0x87)
/** TLV: WifiDirect Persistent Group */
#define MRVL_WIFIDIRECT_PERSISTENT_GROUP_TLV_ID     (PROPRIETARY_TLV_BASE_ID + 0x88)
/** TLV: WifiDirect Presence request parameters */
#define MRVL_WIFIDIRECT_PRESENCE_REQ_PARAMS_TLV_ID  (PROPRIETARY_TLV_BASE_ID + 0x8d)
/** TLV: WifiDirect Extended Listen Time */
#define MRVL_WIFIDIRECT_EXTENDED_LISTEN_TIME_TLV_ID (PROPRIETARY_TLV_BASE_ID + 0x8e)
/** TLV: WifiDirect Provisioning parameters */
#define MRVL_WIFIDIRECT_PROVISIONING_PARAMS_TLV_ID  (PROPRIETARY_TLV_BASE_ID + 0x8f)
/** TLV: WifiDirect WPS parameters */
#define MRVL_WIFIDIRECT_WPS_PARAMS_TLV_ID           (PROPRIETARY_TLV_BASE_ID + 0x90)

/** Max Device capability */
#define MAX_DEV_CAPABILITY                 255
/** Max group capability */
#define MAX_GRP_CAPABILITY                 255
/** Max Intent */
#define MAX_INTENT                        15
/** Max length of Primary device type OUI */
#define MAX_PRIMARY_OUI_LEN               4
/** Min value of Regulatory class */
#define MIN_REG_CLASS                      1
/** Max value of Regulatory class */
#define MAX_REG_CLASS                    255
/** Min value of NoA index */
#define MIN_NOA_INDEX                   0
/** Max value of NoA index */
#define MAX_NOA_INDEX                   255
/** Min value of CTwindow */
#define MIN_CTWINDOW                    0
/** Max value of CTwindow */
#define MAX_CTWINDOW                    63
/** Min value of Count/Type */
#define MIN_COUNT_TYPE                  1
/** Max value of Count/Type */
#define MAX_COUNT_TYPE                  255
/** Min Primary Device type category */
#define MIN_PRIDEV_TYPE_CAT                1
/** Max Primary Device type category */
#define MAX_PRIDEV_TYPE_CAT               11
/** Min Primary Device type subcategory */
#define MIN_PRIDEV_TYPE_SUBCATEGORY        1
/** Max Primary Device type subcategory */
#define MAX_PRIDEV_TYPE_SUBCATEGORY        9
/** Min value of WPS config method */
#define MIN_WPS_CONF_METHODS            0x01
/** Max value of WPS config method */
#define MAX_WPS_CONF_METHODS            0xffff
/** Max length of Advertisement Protocol IE  */
#define MAX_ADPROTOIE_LEN               4
/** Max length of Discovery Information ID  */
#define MAX_INFOID_LEN                  2
/** Max length of OUI  */
#define MAX_OUI_LEN                     3
/** Max count of interface list */
#define MAX_INTERFACE_ADDR_COUNT        41
/** Max count of secondary device types */
#define MAX_SECONDARY_DEVICE_COUNT      15
/** Max count of group secondary device types*/
#define MAX_GROUP_SECONDARY_DEVICE_COUNT 2
/** Maximum length of lines in configuration file */
#define MAX_CONFIG_LINE                 1024
/** Maximum number of arguments in configuration file */
#define MAX_ARGS_NUM                    256
/** Maximum channels */
#define MAX_CHANNELS    165
/** Maximum number of NoA descriptors */
#define MAX_NOA_DESCRIPTORS            8
/** Maximum number of channel list entries */
#define MAX_CHAN_LIST    8
/** Maximum buffer size for channel entries */
#define MAX_BUFFER_SIZE         64
/** WPS Minimum version number */
#define WPS_MIN_VERSION			0x10
/** WPS Maximum version number */
#define WPS_MAX_VERSION                 0x20
/** WPS Minimum request type */
#define WPS_MIN_REQUESTTYPE		0x00
/** WPS Maximum request type */
#define WPS_MAX_REQUESTTYPE		0x04
/** WPS Minimum config methods */
#define WPS_MIN_SPECCONFMETHODS		0x0001
/** WPS Maximum config methods */
#define WPS_MAX_SPECCONFMETHODS		0xFFFF
/** WPS UUID maximum length */
#define WPS_UUID_MAX_LEN                16
/** WPS Device Type maximum length */
#define WPS_DEVICE_TYPE_MAX_LEN         8
/** WPS Minimum association state */
#define WPS_MIN_ASSOCIATIONSTATE	0x0000
/** WPS Maximum association state */
#define WPS_MAX_ASSOCIATIONSTATE	0x0004
/** WPS Minimum configuration error */
#define WPS_MIN_CONFIGURATIONERROR	0x0000
/** WPS Maximum configuration error */
#define WPS_MAX_CONFIGURATIONERROR	0x0012
/** WPS Minimum Device password ID */
#define WPS_MIN_DEVICEPASSWORD		0x0000
/** WPS Maximum Device password ID */
#define WPS_MAX_DEVICEPASSWORD		0x000f
/** WPS Device Name maximum length */
#define WPS_DEVICE_NAME_MAX_LEN         32
/** WPS Model maximum length */
#define WPS_MODEL_MAX_LEN               32
/** WPS Serial maximum length */
#define WPS_SERIAL_MAX_LEN              32
/** WPS Manufacturer maximum length */
#define WPS_MANUFACT_MAX_LEN            64
/** WPS Device Info OUI+Type+SubType Length */
#define WPS_DEVICE_TYPE_LEN             8

/** Maximum value of invitation list index */
#define WIFIDIRECT_INVITATION_LIST_MAX         5
/** Maximum value of persistent group index */
#define WIFIDIRECT_PERSISTENT_GROUP_MAX        4
/** Minimum length of Passphrase   */
#define WIFIDIRECT_PASSPHRASE_LEN_MIN          8
/** Maximum length of PSK  */
#define WIFIDIRECT_PSK_LEN_MAX                 64
/** Persistent group cancel command   */
#define WIFIDIRECT_PERSISTENT_RECORD_CANCEL    0xFF
/** Maximum value of noA descriptors */
#define WIFIDIRECT_NOA_DESC_MAX                2
/** Country string last byte 0x04 */
#define WIFIDIRECT_COUNTRY_LAST_BYTE           0x04

/** Host Command ID : mgmt IE list */
#define  HostCmd_CMD_MGMT_IE_LIST             0x00f2

#ifndef ETH_ALEN
/** MAC address length */
#define ETH_ALEN    6
#endif

/** Action field value : get */
#define ACTION_GET  0
/** Action field value : set */
#define ACTION_SET  1

/** Success */
#define SUCCESS     1
/** Failure */
#define FAILURE     0
/** MAC BROADCAST */
#define WIFIDIRECT_RET_MAC_BROADCAST   0x1FF
/** MAC MULTICAST */
#define WIFIDIRECT_RET_MAC_MULTICAST   0x1FE

/** Command is successful */
#define CMD_SUCCESS     0
/** Command fails */
#define CMD_FAILURE     -1

/**
 * Hex or Decimal to Integer
 * @param   num string to convert into decimal or hex
 */
#define A2HEXDECIMAL(num)  \
	(strncasecmp("0x", (num), 2) ? \
	(unsigned int) strtoll((num), NULL, 0) : a2hex((num)))\

/**
 * Check of decimal or hex string
 * @param   num string
 */
#define IS_HEX_OR_DIGIT(num) \
	(strncasecmp("0x", (num), 2) ? ISDIGIT((num)) : ishexstring((num)))\

/** Find minimum value */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif /* MIN */

/** Level of wifidirect parameters in the wifidirect.conf file */
typedef enum {
	WIFIDIRECT_PARAMS_CONFIG = 1,
	WIFIDIRECT_CAPABILITY_CONFIG,
	WIFIDIRECT_GROUP_OWNER_INTENT_CONFIG,
	WIFIDIRECT_CHANNEL_CONFIG,
	WIFIDIRECT_MANAGEABILITY_CONFIG,
	WIFIDIRECT_CHANNEL_LIST_CONFIG,
	WIFIDIRECT_NOTICE_OF_ABSENCE,
	WIFIDIRECT_NOA_DESCRIPTOR,
	WIFIDIRECT_DEVICE_INFO_CONFIG,
	WIFIDIRECT_GROUP_INFO_CONFIG,
	WIFIDIRECT_GROUP_SEC_INFO_CONFIG,
	WIFIDIRECT_GROUP_CLIENT_INFO_CONFIG,
	WIFIDIRECT_DEVICE_SEC_INFO_CONFIG,
	WIFIDIRECT_GROUP_ID_CONFIG,
	WIFIDIRECT_GROUP_BSS_ID_CONFIG,
	WIFIDIRECT_DEVICE_ID_CONFIG,
	WIFIDIRECT_INTERFACE_CONFIG,
	WIFIDIRECT_TIMEOUT_CONFIG,
	WIFIDIRECT_EXTENDED_TIME_CONFIG,
	WIFIDIRECT_INTENDED_ADDR_CONFIG,
	WIFIDIRECT_OPCHANNEL_CONFIG,
	WIFIDIRECT_INVITATION_FLAG_CONFIG,
	WIFIDIRECT_WPSIE,
	WIFIDIRECT_DISCOVERY_REQUEST_RESPONSE = 0x20,
	WIFIDIRECT_DISCOVERY_QUERY,
	WIFIDIRECT_DISCOVERY_SERVICE,
	WIFIDIRECT_DISCOVERY_VENDOR,
	WIFIDIRECT_DISCOVERY_QUERY_RESPONSE_PER_PROTOCOL,
	WIFIDIRECT_EXTRA,
} wifidirect_param_level;

/** Valid Input Commands */
typedef enum {
	SCANCHANNELS,
	CHANNEL,
	WIFIDIRECT_DEVICECAPABILITY,
	WIFIDIRECT_GROUPCAPABILITY,
	WIFIDIRECT_INTENT,
	WIFIDIRECT_REGULATORYCLASS,
	WIFIDIRECT_MANAGEABILITY,
	WIFIDIRECT_COUNTRY,
	WIFIDIRECT_NO_OF_CHANNELS,
	WIFIDIRECT_NOA_INDEX,
	WIFIDIRECT_OPP_PS,
	WIFIDIRECT_CTWINDOW,
	WIFIDIRECT_COUNT_TYPE,
	WIFIDIRECT_DURATION,
	WIFIDIRECT_INTERVAL,
	WIFIDIRECT_START_TIME,
	WIFIDIRECT_PRIDEVTYPECATEGORY,
	WIFIDIRECT_PRIDEVTYPEOUI,
	WIFIDIRECT_PRIDEVTYPESUBCATEGORY,
	WIFIDIRECT_SECONDARYDEVCOUNT,
	WIFIDIRECT_GROUP_SECONDARYDEVCOUNT,
	WIFIDIRECT_GROUP_WIFIDIRECT_DEVICE_NAME,
	WIFIDIRECT_INTERFACECOUNT,
	WIFIDIRECT_ATTR_CONFIG_TIMEOUT,
	WIFIDIRECT_ATTR_EXTENDED_TIME,
	WIFIDIRECT_WPSCONFMETHODS,
	WIFIDIRECT_WPSVERSION,
	WIFIDIRECT_WPSSETUPSTATE,
	WIFIDIRECT_WPSREQRESPTYPE,
	WIFIDIRECT_WPSSPECCONFMETHODS,
	WIFIDIRECT_WPSUUID,
	WIFIDIRECT_WPSPRIMARYDEVICETYPE,
	WIFIDIRECT_WPSRFBAND,
	WIFIDIRECT_WPSASSOCIATIONSTATE,
	WIFIDIRECT_WPSCONFIGURATIONERROR,
	WIFIDIRECT_WPSDEVICENAME,
	WIFIDIRECT_WPSDEVICEPASSWORD,
	WIFIDIRECT_WPSMANUFACTURER,
	WIFIDIRECT_WPSMODELNAME,
	WIFIDIRECT_WPSMODELNUMBER,
	WIFIDIRECT_WPSSERIALNUMBER,
	WIFIDIRECT_CATEGORY,
	WIFIDIRECT_ACTION,
	WIFIDIRECT_DIALOGTOKEN,
	WIFIDIRECT_DISC_ADPROTOIE,
	WIFIDIRECT_GAS_COMEBACK_DELAY,
	WIFIDIRECT_DISC_INFOID,
	WIFIDIRECT_OUI,
	WIFIDIRECT_OUITYPE,
	WIFIDIRECT_OUISUBTYPE,
	WIFIDIRECT_SERVICEUPDATE_INDICATOR,
	WIFIDIRECT_DISC_SERVICEPROTO,
	WIFIDIRECT_DISC_SERVICETRANSACID,
	WIFIDIRECT_DISC_SERVICE_STATUS,
	WIFIDIRECT_MINDISCOVERYINT,
	WIFIDIRECT_MAXDISCOVERYINT,
	WIFIDIRECT_ENABLE_SCAN,
	WIFIDIRECT_DEVICE_STATE,
	WIFIDIRECT_INVITATIONFLAG,
	WIFIDIRECT_DISC_DNSTYPE,
	WIFIDIRECT_DISC_BONJOUR_VERSION,
	WIFIDIRECT_DISC_UPNP_VERSION,
	WIFIDIRECT_PRESENCE_REQ_TYPE,
} valid_inputs;

/** WIFIDIRECT IE header len */
#define WIFIDIRECT_IE_HEADER_LEN                   3

/** AP CMD header */
#define WIFIDIRECT_CMD_HEADER	/** Buf Size */         \
			t_u32 buf_size;         \
			/** Command Code */     \
			t_u16 cmd_code;         \
			/** Size */             \
			t_u16 size;             \
			/** Sequence Number */  \
			t_u16 seq_num;          \
			/** Result */           \
			t_s16 result

/**  TLV header size */
#define MRVL_TLV_HEADER_SIZE   4

/** WFDCMD buffer */
typedef PACK_START struct _WFDCMDBUF {
    /** Command header */
	WIFIDIRECT_CMD_HEADER;
} PACK_END WFDCMDBUF;

/** WIFIDIRECTCMD buffer */
typedef PACK_START struct _wifidirectcmdbuf {
    /** Command header */
	WIFIDIRECT_CMD_HEADER;
    /** Action : GET or SET */
	t_u16 action;
} PACK_END wifidirectcmdbuf;

/** TLV buffer : WifiDirect IE device Id */
typedef PACK_START struct _tlvbuf_wifidirect_device_id {
    /** TLV Header tag */
	t_u8 tag;
    /** TLV Header length */
	t_u16 length;
    /** WIFIDIRECT device MAC address */
	t_u8 dev_mac_address[ETH_ALEN];
} PACK_END tlvbuf_wifidirect_device_id;

/** TLV buffer : WifiDirect IE capability */
typedef PACK_START struct _tlvbuf_wifidirect_capability {
    /** TLV Header tag */
	t_u8 tag;
    /** TLV Header length */
	t_u16 length;
    /** WIFIDIRECT device capability */
	t_u8 dev_capability;
    /** WIFIDIRECT group capability */
	t_u8 group_capability;
} PACK_END tlvbuf_wifidirect_capability;

/** TLV buffer : WifiDirect IE Group owner intent */
typedef PACK_START struct _tlvbuf_wifidirect_group_owner_intent {
    /** TLV Header tag */
	t_u8 tag;
    /** TLV Header length */
	t_u16 length;
    /** WIFIDIRECT device group owner intent */
	t_u8 dev_intent;
} PACK_END tlvbuf_wifidirect_group_owner_intent;

/** TLV buffer : WifiDirect IE channel */
typedef PACK_START struct _tlvbuf_wifidirect_channel {
    /** TLV Header tag */
	t_u8 tag;
    /** TLV Header length */
	t_u16 length;
    /** WIFIDIRECT country string */
	t_u8 country_string[3];
    /** WIFIDIRECT regulatory class */
	t_u8 regulatory_class;
    /** WIFIDIRECT channel number */
	t_u8 channel_number;
} PACK_END tlvbuf_wifidirect_channel;

/** TLV buffer : WifiDirect IE invitation flag */
typedef PACK_START struct _tlvbuf_wifidirect_invitation_flag {
    /** TLV Header tag */
	t_u8 tag;
    /** TLV Header length */
	t_u16 length;
    /** WIFIDIRECT invitation flag */
	t_u8 invitation_flag;
} PACK_END tlvbuf_wifidirect_invitation_flag;

/** Channel Entry */
typedef PACK_START struct _chan_entry {
    /** WIFIDIRECT regulatory class */
	t_u8 regulatory_class;
    /** WIFIDIRECT no of channels */
	t_u8 num_of_channels;
    /** WIFIDIRECT channel number */
	t_u8 chan_list[0];
} PACK_END chan_entry;

/** NoA Descriptor */
typedef PACK_START struct _noa_descriptor {
    /** WIFIDIRECT count OR type */
	t_u8 count_type;
    /** WIFIDIRECT duration */
	t_u32 duration;
    /** WIFIDIRECT interval */
	t_u32 interval;
    /** WIFIDIRECT start time */
	t_u32 start_time;
} PACK_END noa_descriptor;

/** TLV buffer : WifiDirect IE channel list */
typedef PACK_START struct _tlvbuf_wifidirect_channel_list {
    /** TLV Header tag */
	t_u8 tag;
    /** TLV Header length */
	t_u16 length;
    /** WIFIDIRECT country string */
	t_u8 country_string[3];
    /** WIFIDIRECT channel entry list */
	chan_entry wifidirect_chan_entry_list[0];
} PACK_END tlvbuf_wifidirect_channel_list;

/** TLV buffer : WifiDirect IE Manageability */
typedef PACK_START struct _tlvbuf_wifidirect_manageability {
    /** TLV Header tag */
	t_u8 tag;
    /** TLV Header length */
	t_u16 length;
    /** WIFIDIRECT Manageability */
	t_u8 manageability;
} PACK_END tlvbuf_wifidirect_manageability;

/** TLV buffer : WifiDirect IE Notice of Absence */
typedef PACK_START struct _tlvbuf_wifidirect_notice_of_absence {
    /** TLV Header tag */
	t_u8 tag;
    /** TLV Header length */
	t_u16 length;
    /** WIFIDIRECT NoA Index */
	t_u8 noa_index;
    /** WIFIDIRECT CTWindow and OppPS parameters */
	t_u8 ctwindow_opp_ps;
    /** WIFIDIRECT NoA Descriptor list */
	noa_descriptor wifidirect_noa_descriptor_list[0];
} PACK_END tlvbuf_wifidirect_notice_of_absence;

/** TLV buffer : WifiDirect IE device Info */
typedef PACK_START struct _tlvbuf_wifidirect_device_info {
    /** TLV Header tag */
	t_u8 tag;
    /** TLV Header length */
	t_u16 length;
    /** WIFIDIRECT device address */
	t_u8 dev_address[ETH_ALEN];
    /** WPS config methods */
	t_u16 config_methods;
    /** Primary device type : category */
	t_u16 primary_category;
    /** Primary device type : OUI */
	t_u8 primary_oui[4];
    /** Primary device type : sub-category */
	t_u16 primary_subcategory;
    /** Secondary Device Count */
	t_u8 secondary_dev_count;
    /** Secondary Device Info */
	t_u8 secondary_dev_info[0];
    /** WPS Device Name Tag */
	t_u16 device_name_type;
    /** WPS Device Name Length */
	t_u16 device_name_len;
    /** Device name */
	t_u8 device_name[0];
} PACK_END tlvbuf_wifidirect_device_info;

/** TLV buffer : wifidirect IE WIFIDIRECT Group Info- Client Dev Info */
typedef PACK_START struct _wifidirect_client_dev_info {
    /** Length of each device */
	t_u8 dev_length;
    /** WIFIDIRECT device address */
	t_u8 wifidirect_dev_address[ETH_ALEN];
    /** WIFIDIRECT Interface  address */
	t_u8 wifidirect_intf_address[ETH_ALEN];
    /** WIFIDIRECT Device capability*/
	t_u8 wifidirect_dev_capability;
    /** WPS config methods */
	t_u16 config_methods;
    /** Primary device type : category */
	t_u16 primary_category;
    /** Primary device type : OUI */
	t_u8 primary_oui[4];
    /** Primary device type : sub-category */
	t_u16 primary_subcategory;
    /** Secondary Device Count */
	t_u8 wifidirect_secondary_dev_count;
    /** Secondary Device Info */
	t_u8 wifidirect_secondary_dev_info[0];
    /** WPS WIFIDIRECT Device Name Tag */
	t_u16 wifidirect_device_name_type;
    /** WPS WIFIDIRECT Device Name Length */
	t_u16 wifidirect_device_name_len;
    /** WIFIDIRECT Device name */
	t_u8 wifidirect_device_name[0];
} PACK_END wifidirect_client_dev_info;

/** TLV buffer : wifidirect IE WIFIDIRECT Group Info */
typedef PACK_START struct _tlvbuf_wifidirect_group_info {
    /** TLV Header tag */
	t_u8 tag;
    /** TLV Header length */
	t_u16 length;
    /** Secondary Device Info */
	t_u8 wifidirect_client_dev_list[0];
} PACK_END tlvbuf_wifidirect_group_info;

/** TLV buffer : WifiDirect IE group Id */
typedef PACK_START struct _tlvbuf_wifidirect_group_id {
    /** TLV Header tag */
	t_u8 tag;
    /** TLV Header length */
	t_u16 length;
    /** WIFIDIRECT group MAC address */
	t_u8 group_address[ETH_ALEN];
    /** WIFIDIRECT group SSID */
	t_u8 group_ssid[0];
} PACK_END tlvbuf_wifidirect_group_id;

/** TLV buffer : WifiDirect IE group BSS Id */
typedef PACK_START struct _tlvbuf_wifidirect_group_bss_id {
    /** TLV Header tag */
	t_u8 tag;
    /** TLV Header length */
	t_u16 length;
    /** WIFIDIRECT group Bss Id */
	t_u8 group_bssid[ETH_ALEN];
} PACK_END tlvbuf_wifidirect_group_bss_id;

/** TLV buffer : WifiDirect IE Interface */
typedef PACK_START struct _tlvbuf_wifidirect_interface {
    /** TLV Header tag */
	t_u8 tag;
    /** TLV Header length */
	t_u16 length;
    /** WIFIDIRECT interface Id */
	t_u8 interface_id[ETH_ALEN];
    /** WIFIDIRECT interface count */
	t_u8 interface_count;
    /** WIFIDIRECT interface addresslist */
	t_u8 interface_idlist[0];
} PACK_END tlvbuf_wifidirect_interface;

/** TLV buffer : WifiDirect configuration timeout */
typedef PACK_START struct _tlvbuf_wifidirect_config_timeout {
    /** TLV Header tag */
	t_u8 tag;
    /** TLV Header length */
	t_u16 length;
    /** Group configuration timeout */
	t_u8 group_config_timeout;
    /** Device configuration timeout */
	t_u8 device_config_timeout;
} PACK_END tlvbuf_wifidirect_config_timeout;

/** TLV buffer : WifiDirect extended listen time */
typedef PACK_START struct _tlvbuf_wifidirect_ext_listen_time {
    /** TLV Header tag */
	t_u8 tag;
    /** TLV Header length */
	t_u16 length;
    /** Availability period */
	t_u16 availability_period;
    /** Availability interval */
	t_u16 availability_interval;
} PACK_END tlvbuf_wifidirect_ext_listen_time;

/** TLV buffer : WifiDirect Intended Interface Address */
typedef PACK_START struct _tlvbuf_wifidirect_intended_addr {
    /** TLV Header tag */
	t_u8 tag;
    /** TLV Header length */
	t_u16 length;
    /** WIFIDIRECT Group interface address */
	t_u8 group_address[ETH_ALEN];
} PACK_END tlvbuf_wifidirect_intended_addr;

/** TLV buffer : WifiDirect WPS IE */
typedef PACK_START struct _tlvbuf_wifidirect_wps_ie {
    /** TLV Header tag */
	t_u16 tag;
    /** TLV Header length */
	t_u16 length;
    /** WIFIDIRECT WPS IE data */
	t_u8 data[0];
} PACK_END tlvbuf_wps_ie;

/** HostCmd_CMD_WIFIDIRECT_MODE_CONFIG */
typedef PACK_START struct _wifidirect_mode_config {
    /** Header */
	WIFIDIRECT_CMD_HEADER;
    /** Action */
	t_u16 action;		/* 0 = ACT_GET; 1 = ACT_SET; */
    /** wifidirect mode data */
	t_u16 mode;
} PACK_END wifidirect_mode_config;

/** HostCmd_CMD_WIFIDIRECT_PARAMS_CONFIG */
typedef PACK_START struct _wifidirect_params_config {
    /** Header */
	WIFIDIRECT_CMD_HEADER;
    /** Action */
	t_u16 action;		/* 0 = ACT_GET; 1 = ACT_SET; */
    /** TLV data */
	t_u8 wifidirect_tlv[0];
} PACK_END wifidirect_params_config;

/** Internal WIFIDIRECT structure for Query Data */
typedef PACK_START struct wifidirect_query_data {
	union {
		PACK_START struct upnp_specific_query {
	    /** version field */
			t_u8 version;
	    /** value */
			t_u8 value[0];
		} PACK_END upnp;

		PACK_START struct bonjour_specific_query {
	    /** DNS name */
			t_u8 dns[0];
	    /** DNS type */
			t_u8 dns_type;
	    /** version field */
			t_u8 version;
		} PACK_END bonjour;
	} u;
} PACK_END wifidirect_query_data;

/** Internal WIFIDIRECT structure for Response Data */
typedef PACK_START struct wifidirect_Response_data {
	union {
		PACK_START struct upnp_specific_response {
	    /** version field */
			t_u8 version;
	    /** value */
			t_u8 value[0];
		} PACK_END upnp;

		PACK_START struct bonjour_specific_response {
	    /** DNS name */
			t_u8 dns[0];
	    /** DNS type */
			t_u8 dns_type;
	    /** version field */
			t_u8 version;
	    /** DNS name */
			t_u8 record[0];
		} PACK_END bonjour;
	} u;
} PACK_END wifidirect_response_data;

/** HostCmd_CMD_WIFIDIRECT_SERVICE_DISCOVERY request */
typedef PACK_START struct _wifidirect_discovery_request {
    /** Header */
	WIFIDIRECT_CMD_HEADER;
    /** Peer mac address */
	t_u8 peer_mac_addr[ETH_ALEN];
    /** Category */
	t_u8 category;
    /** Action */
	t_u8 action;
    /** Dialog taken */
	t_u8 dialog_taken;
    /** Advertize protocol IE */
	t_u8 advertize_protocol_ie[4];
    /** Query request Length */
	t_u16 query_len;
    /** Information identifier */
	t_u8 info_id[2];
    /** Request Length */
	t_u16 request_len;
    /** OUI */
	t_u8 oui[3];
    /** OUI sub type */
	t_u8 oui_sub_type;
    /** Service update indicator */
	t_u16 service_update_indicator;
    /** Vendor Length */
	t_u16 vendor_len;
    /** Service protocol */
	t_u8 service_protocol;
    /** Service transaction Id */
	t_u8 service_transaction_id;
    /** Query Data */
	wifidirect_query_data disc_query;
} PACK_END wifidirect_discovery_request;

/** HostCmd_CMD_WIFIDIRECT_SERVICE_DISCOVERY response */
typedef PACK_START struct _wifidirect_discovery_response {
    /** Header */
	WIFIDIRECT_CMD_HEADER;
    /** Peer mac address */
	t_u8 peer_mac_addr[ETH_ALEN];
    /** Category */
	t_u8 category;
    /** Action */
	t_u8 action;
    /** Dialog taken */
	t_u8 dialog_taken;
    /** Status code */
	t_u16 status_code;
    /** GAS comback reply */
	t_u16 gas_reply;
    /** Advertize protocol IE */
	t_u8 advertize_protocol_ie[4];
    /** Query response Length */
	t_u16 query_len;
    /** Information identifier */
	t_u8 info_id[2];
    /** Response Length */
	t_u16 response_len;
    /** OUI */
	t_u8 oui[3];
    /** OUI sub type */
	t_u8 oui_sub_type;
    /** Service update indicator */
	t_u16 service_update_indicator;
    /** Vendor Length */
	t_u16 vendor_len;
    /** Service protocol */
	t_u8 service_protocol;
    /** Service transaction Id */
	t_u8 service_transaction_id;
    /** Discovery status code */
	t_u8 disc_status_code;
    /** Response Data */
	wifidirect_response_data disc_resp;
} PACK_END wifidirect_discovery_response;

/** HostCmd_CMD_WIFIDIRECT_GAS_COMEBACK_SERVICE request */
typedef PACK_START struct _wifidirect_gas_comeback_request {
    /** Header */
	WIFIDIRECT_CMD_HEADER;
    /** Peer mac address */
	t_u8 peer_mac_addr[ETH_ALEN];
    /** Category */
	t_u8 category;
    /** Action */
	t_u8 action;
    /** Dialog taken */
	t_u8 dialog_taken;
} PACK_END wifidirect_gas_comeback_request;

/** HostCmd_CMD_WIFIDIRECT_GAS_COMEBACK_SERVICE response */
typedef PACK_START struct _wifidirect_gas_comeback_response {
    /** Header */
	WIFIDIRECT_CMD_HEADER;
    /** Peer mac address */
	t_u8 peer_mac_addr[ETH_ALEN];
    /** Category */
	t_u8 category;
    /** Action */
	t_u8 action;
    /** Dialog taken */
	t_u8 dialog_taken;
    /** Status code */
	t_u16 status_code;
    /** Gas response fragment ID */
	t_u8 gas_fragment_id;
    /** GAS comback reply */
	t_u16 gas_reply;
    /** Advertize protocol IE */
	t_u8 advertize_protocol_ie[4];
    /** Query response Length */
	t_u16 query_len;
    /** Information identifier */
	t_u8 info_id[2];
    /** Response Length */
	t_u16 response_len;
    /** Response status code */
	t_u8 resp_status_code;
    /** OUI */
	t_u8 oui[3];
    /** OUI sub type */
	t_u8 oui_sub_type;
    /** Service update indicator */
	t_u16 service_update_indicator;
    /** Vendor Length */
	t_u16 vendor_len;
    /** Service protocol */
	t_u8 service_protocol;
    /** Service transaction Id */
	t_u8 service_transaction_id;
    /** Discovery status code */
	t_u8 disc_status_code;
    /** Response Data */
	wifidirect_response_data disc_resp;
} PACK_END wifidirect_gas_comeback_response;

/** HostCmd_CMD_WIFIDIRECT_ACTION_FRAME request */
typedef PACK_START struct _wifidirect_action_frame {
    /** Header */
	WIFIDIRECT_CMD_HEADER;
    /** Peer mac address */
	t_u8 peer_mac_addr[ETH_ALEN];
    /** Category */
	t_u8 category;
    /** Action */
	t_u8 action;
    /** OUI */
	t_u8 oui[3];
    /** OUI type */
	t_u8 oui_type;
    /** OUI sub type */
	t_u8 oui_sub_type;
    /** Dialog taken */
	t_u8 dialog_taken;
    /** IE List of TLVs */
	t_u8 ie_list[0];
} PACK_END wifidirect_action_frame;

/** custom IE */
typedef PACK_START struct _custom_ie {
    /** IE Index */
	t_u16 ie_index;
    /** Mgmt Subtype Mask */
	t_u16 mgmt_subtype_mask;
    /** IE Length */
	t_u16 ie_length;
    /** IE buffer */
	t_u8 ie_buffer[0];
} PACK_END custom_ie;

/** TLV buffer : custom IE */
typedef PACK_START struct _tlvbuf_custom_ie {
    /** Tag */
	t_u16 tag;
    /** Length */
	t_u16 length;
    /** custom IE data */
	custom_ie ie_data[0];
} PACK_END tlvbuf_custom_ie;

/** TLV buffer : persistent group */
typedef PACK_START struct _tlvbuf_wifidirect_persistent_group {
    /** Tag */
	t_u16 tag;
    /** Length */
	t_u16 length;
    /** Record Index */
	t_u8 rec_index;
    /** Device Role */
	t_u8 dev_role;
    /** wifidirect group Bss Id */
	t_u8 group_bssid[ETH_ALEN];
    /** wifidirect device MAC address */
	t_u8 dev_mac_address[ETH_ALEN];
    /** wifidirect group SSID length */
	t_u8 group_ssid_len;
    /** wifidirect group SSID */
	t_u8 group_ssid[0];
    /** wifidirect PSK length */
	t_u8 psk_len;
    /** wifidirect PSK */
	t_u8 psk[0];
    /** Num of PEER MAC Addresses */
	t_u8 num_peers;
    /** PEER MAC Addresses List */
	t_u8 peer_mac_addrs[0][ETH_ALEN];
} PACK_END tlvbuf_wifidirect_persistent_group;

/** TLV buffer : WifiDirect discovery period */
typedef PACK_START struct _tlvbuf_wifidirect_discovery_period {
    /** Tag */
	t_u16 tag;
    /** Length */
	t_u16 length;
    /** Min discovery interval */
	t_u16 min_disc_interval;
    /** Max discovery interval */
	t_u16 max_disc_interval;
} PACK_END tlvbuf_wifidirect_discovery_period;

/** TLV buffer : WifiDirect Intent */
typedef PACK_START struct _tlvbuf_wifidirect_intent {
    /** Tag */
	t_u16 tag;
    /** Length */
	t_u16 length;
    /** Intent value */
	t_u8 intent;
} PACK_END tlvbuf_wifidirect_intent;

/** TLV buffer : WifiDirect Invitation List */
typedef PACK_START struct _tlvbuf_wifidirect_invitation_list {
    /** Tag */
	t_u16 tag;
    /** Length */
	t_u16 length;
    /** Invitation peer address*/
	t_u8 inv_peer_addr[ETH_ALEN];
} PACK_END tlvbuf_wifidirect_invitation_list;

/** TLV buffer : WifiDirect Listen Channel */
typedef PACK_START struct _tlvbuf_wifidirect_listen_channel {
    /** Tag */
	t_u16 tag;
    /** Length */
	t_u16 length;
    /** Listen Channel */
	t_u8 listen_channel;
} PACK_END tlvbuf_wifidirect_listen_channel;

/** TLV buffer : WifiDirect Operating Channel */
typedef PACK_START struct _tlvbuf_wifidirect_operating_channel {
    /** Tag */
	t_u16 tag;
    /** Length */
	t_u16 length;
    /** Operating Channel */
	t_u8 operating_channel;
} PACK_END tlvbuf_wifidirect_operating_channel;

/** TLV buffer : WifiDirect NoA config */
typedef PACK_START struct _tlvbuf_wifidirect_noa_config {
    /** Tag */
	t_u16 tag;
    /** Length */
	t_u16 length;
    /** Enable/Disable NoA */
	t_u16 enable_noa;
    /** Index */
	t_u8 noa_index;
    /** CountType */
	t_u8 count_type;
    /** Duration */
	t_u32 duration;
    /** Interval */
	t_u32 interval;
} PACK_END tlvbuf_wifidirect_noa_config;

/** TLV buffer : wifidirect OppPS config */
typedef PACK_START struct _tlvbuf_opp_ps_config {
    /** Tag */
	t_u16 tag;
    /** Length */
	t_u16 length;
    /** CTWindow and OppPS*/
	t_u8 ctwindow_opp_ps;
} PACK_END tlvbuf_wifidirect_opp_ps_config;

/** TLV buffer : wifidirect capability config */
typedef PACK_START struct _tlvbuf_capability_config {
    /** Tag */
	t_u16 tag;
    /** Length */
	t_u16 length;
    /** Device capability */
	t_u8 dev_capability;
    /** Group capability */
	t_u8 group_capability;
} PACK_END tlvbuf_wifidirect_capability_config;

/** TLV buffer : WifiDirect Presence Request Parameters */
typedef PACK_START struct _tlvbuf_wifidirect_presence_req_params {
    /** Tag */
	t_u16 tag;
    /** Length */
	t_u16 length;
    /** Presence Request Type */
	t_u8 presence_req_type;
    /** Duration */
	t_u32 duration;
    /** Interval */
	t_u32 interval;
} PACK_END tlvbuf_wifidirect_presence_req_params;

/** TLV buffer : WifiDirect Extended Listen Timing parameters*/
typedef PACK_START struct _tlvbuf_mrvl_wifidirect_ext_listen_time {
    /** Tag */
	t_u16 tag;
    /** Length */
	t_u16 length;
    /** Duration */
	t_u16 duration;
    /** Interval */
	t_u16 interval;
} PACK_END tlvbuf_wifidirect_mrvl_ext_listen_time;

/** TLV buffer : WifiDirect Provisioning parameters*/
typedef PACK_START struct _tlvbuf_wifidirect_provisioning_params {
    /** Tag */
	t_u16 tag;
    /** Length */
	t_u16 length;
    /** action */
	t_u16 action;
    /** config methods */
	t_u16 config_methods;
    /** config methods */
	t_u16 dev_password;
} PACK_END tlvbuf_wifidirect_provisioning_params;

/** TLV buffer : WifiDirect WPS parameters*/
typedef PACK_START struct _tlvbuf_wifidirect_wps_params {
    /** Tag */
	t_u16 tag;
    /** Length */
	t_u16 length;
    /** action */
	t_u16 action;
} PACK_END tlvbuf_wifidirect_wps_params;

/** Max size of custom IE buffer */
#define MAX_SIZE_IE_BUFFER              (256)
/** Size of command buffer */
#define MRVDRV_SIZE_OF_CMD_BUFFER       (2048)
/** 4 byte header to store buf len*/
#define BUF_HEADER_SIZE     4
/** OUI Type WFA WIFIDIRECT */
#define OUI_TYPE_WFA_WIFIDIRECT    9

/** Host Command ioctl number */
#define WIFIDIRECT_HOSTCMD          (SIOCDEVPRIVATE + 1)
/** Host Command ID bit mask (bit 11:0) */
#define HostCmd_CMD_ID_MASK             0x0fff
/** WIFIDIRECTCMD response check */
#define WIFIDIRECTCMD_RESP_CHECK                0x8000

#ifdef WIFI_DISPLAY
/** Host Command ID : wfd display mode config */
#define HostCmd_CMD_WFD_DISPLAY_MODE_CONFIG         0x0106
#endif

/** Host Command ID : wifidirect mode config */
#define HostCmd_CMD_WIFIDIRECT_MODE_CONFIG         0x00eb
/** Host Command ID:  WIFIDIRECT_SET_PARAMS */
#define HostCmd_CMD_WIFIDIRECT_PARAMS_CONFIG       0x00ea
/** Host Command ID:  WIFIDIRECT_SERVICE_DISCOVERY */
#define HostCmd_CMD_WIFIDIRECT_SERVICE_DISCOVERY   0x00ec
/** Host Command ID:  WIFIDIRECT_ACTION_FRAME */
#define HostCmd_CMD_802_11_ACTION_FRAME             0x00f4

/** TLV : WifiDirect param capability */
#define TLV_TYPE_WIFIDIRECT_CAPABILITY           0x0002
/** TLV : WifiDirect param device Id */
#define TLV_TYPE_WIFIDIRECT_DEVICE_ID            0x0003
/** TLV : WifiDirect param group owner intent */
#define TLV_TYPE_WIFIDIRECT_GROUPOWNER_INTENT    0x0004
/** TLV : WifiDirect param config timeout */
#define TLV_TYPE_WIFIDIRECT_CONFIG_TIMEOUT       0x0005
/** TLV : WifiDirect param channel */
#define TLV_TYPE_WIFIDIRECT_CHANNEL              0x0006
/** TLV : WifiDirect param group bssId */
#define TLV_TYPE_WIFIDIRECT_GROUP_BSS_ID         0x0007
/** TLV : WifiDirect param extended listen time */
#define TLV_TYPE_WIFIDIRECT_EXTENDED_LISTEN_TIME 0x0008
/** TLV : WifiDirect param intended address */
#define TLV_TYPE_WIFIDIRECT_INTENDED_ADDRESS     0x0009
/** TLV : WifiDirect param manageability */
#define TLV_TYPE_WIFIDIRECT_MANAGEABILITY        0x000a
/** TLV : WifiDirect param channel list */
#define TLV_TYPE_WIFIDIRECT_CHANNEL_LIST         0x000b
/** TLV : WifiDirect Notice of Absence */
#define TLV_TYPE_WIFIDIRECT_NOTICE_OF_ABSENCE    0x000c
/** TLV : WifiDirect param device Info */
#define TLV_TYPE_WIFIDIRECT_DEVICE_INFO          0x000d
/** TLV : WifiDirect param Group Info */
#define TLV_TYPE_WIFIDIRECT_GROUP_INFO           0x000e
/** TLV : WifiDirect param group Id */
#define TLV_TYPE_WIFIDIRECT_GROUP_ID             0x000f
/** TLV : WifiDirect param interface */
#define TLV_TYPE_WIFIDIRECT_INTERFACE            0x0010
/** TLV : WifiDirect param operating channel */
#define TLV_TYPE_WIFIDIRECT_OPCHANNEL            0x0011
/** TLV : WifiDirect param invitation flag */
#define TLV_TYPE_WIFIDIRECT_INVITATION_FLAG      0x0012

/** enum : WPS attribute type */
typedef enum {
	SC_AP_Channel = 0x1001,
	SC_Association_State = 0x1002,
	SC_Authentication_Type = 0x1003,
	SC_Authentication_Type_Flags = 0x1004,
	SC_Authenticator = 0x1005,
	SC_Config_Methods = 0x1008,
	SC_Configuration_Error = 0x1009,
	SC_Confirmation_URL4 = 0x100A,
	SC_Confirmation_URL6 = 0x100B,
	SC_Connection_Type = 0x100C,
	SC_Connection_Type_Flags = 0x100D,
	SC_Credential = 0x100E,
	SC_Device_Name = 0x1011,
	SC_Device_Password_ID = 0x1012,
	SC_E_Hash1 = 0x1014,
	SC_E_Hash2 = 0x1015,
	SC_E_SNonce1 = 0x1016,
	SC_E_SNonce2 = 0x1017,
	SC_Encrypted_Settings = 0x1018,
	SC_Encryption_Type = 0X100F,
	SC_Encryption_Type_Flags = 0x1010,
	SC_Enrollee_Nonce = 0x101A,
	SC_Feature_ID = 0x101B,
	SC_Identity = 0X101C,
	SC_Identity_Proof = 0X101D,
	SC_Key_Wrap_Authenticator = 0X101E,
	SC_Key_Identifier = 0X101F,
	SC_MAC_Address = 0x1020,
	SC_Manufacturer = 0x1021,
	SC_Message_Type = 0x1022,
	SC_Model_Name = 0x1023,
	SC_Model_Number = 0x1024,
	SC_Network_Index = 0x1026,
	SC_Network_Key = 0x1027,
	SC_Network_Key_Index = 0x1028,
	SC_New_Device_Name = 0x1029,
	SC_New_Password = 0x102A,
	SC_OOB_Device_Password = 0X102C,
	SC_OS_Version = 0X102D,
	SC_Power_Level = 0X102F,
	SC_PSK_Current = 0x1030,
	SC_PSK_Max = 0x1031,
	SC_Public_Key = 0x1032,
	SC_Radio_Enabled = 0x1033,
	SC_Reboot = 0x1034,
	SC_Registrar_Current = 0x1035,
	SC_Registrar_Established = 0x1036,
	SC_Registrar_List = 0x1037,
	SC_Registrar_Max = 0x1038,
	SC_Registrar_Nonce = 0x1039,
	SC_Request_Type = 0x103A,
	SC_Response_Type = 0x103B,
	SC_RF_Band = 0X103C,
	SC_R_Hash1 = 0X103D,
	SC_R_Hash2 = 0X103E,
	SC_R_SNonce1 = 0X103F,
	SC_R_SNonce2 = 0x1040,
	SC_Selected_Registrar = 0x1041,
	SC_Serial_Number = 0x1042,
	SC_Simple_Config_State = 0x1044,
	SC_SSID = 0x1045,
	SC_Total_Networks = 0x1046,
	SC_UUID_E = 0x1047,
	SC_UUID_R = 0x1048,
	SC_Vendor_Extension = 0x1049,
	SC_Version = 0x104A,
	SC_X_509_Certificate_Request = 0x104B,
	SC_X_509_Certificate = 0x104C,
	SC_EAP_Identity = 0x104D,
	SC_Message_Counter = 0x104E,
	SC_Public_Key_Hash = 0x104F,
	SC_Rekey_Key = 0x1050,
	SC_Key_Lifetime = 0x1051,
	SC_Permitted_Config_Methods = 0x1052,
	SC_SelectedRegistrarConfigMethods = 0x1053,
	SC_Primary_Device_Type = 0x1054,
	SC_Secondary_Device_Type_List = 0x1055,
	SC_Portable_Device = 0x1056,
	SC_AP_Setup_Locked = 0x1057,
	SC_Application_List = 0x1058,
	SC_EAP_Type = 0x1059,
	SC_Initialization_Vector = 0x1060,
	SC_Key_Provided_Auto = 0x1061,
	SC_8021x_Enabled = 0x1062,
	SC_App_Session_key = 0x1063,
	SC_WEP_Transmit_Key = 0x1064,
} wps_simple_config_attribute;

#endif /* _WIFIDIRECT_H */
