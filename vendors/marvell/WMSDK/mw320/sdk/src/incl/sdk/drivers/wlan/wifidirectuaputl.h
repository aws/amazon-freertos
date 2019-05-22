/** @file  uaputl.h
 *
 *  @brief Header file for uaputl application
 *
 * (C) Copyright 2008-2017 Marvell International Ltd. All Rights Reserved
 *
 * MARVELL CONFIDENTIAL
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Marvell International Ltd or its
 * suppliers or licensors. Title to the Material remains with Marvell
 * International Ltd or its suppliers and licensors. The Material contains
 * trade secrets and proprietary and confidential information of Marvell or
 * its suppliers and licensors. The Material is protected by worldwide
 * copyright and trade secret laws and treaty provisions. No part of the
 * Material may be used, copied, reproduced, modified, published, uploaded,
 * posted, transmitted, distributed, or disclosed in any way without Marvell's
 * prior express written permission.
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Marvell in writing.
 *
 */
/************************************************************************
Change log:
    03/01/08: Initial creation
************************************************************************/

#ifndef _UAP_H
#define _UAP_H

#include <cli_utils.h>

/** uAP application version string */
#define UAP_VERSION         "4.10"

/** 4 byte header to store buf len*/
#define BUF_HEADER_SIZE     4

/** TLV header length */
#define TLVHEADER_LEN       4

/** AP CMD header */
#define APCMDHEADER     /** Buf Size */         \
                        t_u32 buf_size;         \
                        /** Command Code */     \
                        t_u16 cmd_code;         \
                        /** Size */             \
                        t_u16 size;             \
                        /** Sequence Number */  \
                        t_u16 seq_num;          \
                        /** Result */           \
                        t_s16 result

/** TLV header */
#define TLVHEADER       /** Tag */      \
                        t_u16 tag;      \
                        /** Length */   \
                        t_u16 length

/** Maximum number of rates allowed at a time */
#define MAX_RATES               12

#if 0
/** APCMD buffer : sys_configure */
typedef PACK_START struct _apcmdbuf_sys_configure {
    /** Header */
	APCMDHEADER;
    /** Action : GET or SET */
	t_u16 action;
} PACK_END apcmdbuf_sys_configure;

/** TLV buffer : STA MAC address filtering control */
typedef PACK_START struct _tlvbuf_sta_mac_addr_filter {
    /** Header */
	TLVHEADER;
    /** Filter mode */
	t_u8 filter_mode;
    /** Number of STA MACs */
	t_u8 count;
    /** STA MAC addresses buffer */
	t_u8 mac_address[0];
} PACK_END tlvbuf_sta_mac_addr_filter;

/** TLV buffer : Channel Config */
typedef PACK_START struct _tlvbuf_channel_config {
    /** Header */
	TLVHEADER;
    /** Band Configuration
     *
     * [7-6] Channel Selection Mode; 00 manual, 01 ACS
     * [5-4] 00 - no secondary channel,
     *       01 -- secondary channel is above
     *       03 -- secondary channel is below
     * [3-2] Channel Width; 00 20 MHz
     * [1-0] Band Info; 00 2.4 GHz
     */
	t_u8 band_config_type;
    /** Channel number */
	t_u8 chan_number;
} PACK_END tlvbuf_channel_config;
#endif

#if 1
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
         (t_u64)(((t_u64)(x) & 0xff00000000000000ULL) >> 56) ))

#ifdef BIG_ENDIAN_SUPPORT
/** Convert from 16 bit little endian format to CPU format */
#define uap_le16_to_cpu(x) swap_byte_16(x)
/** Convert from 32 bit little endian format to CPU format */
#define uap_le32_to_cpu(x) swap_byte_32(x)
/** Convert from 64 bit little endian format to CPU format */
#define uap_le64_to_cpu(x) swap_byte_64(x)
/** Convert to 16 bit little endian format from CPU format */
#define uap_cpu_to_le16(x) swap_byte_16(x)
/** Convert to 32 bit little endian format from CPU format */
#define uap_cpu_to_le32(x) swap_byte_32(x)
/** Convert to 64 bit little endian format from CPU format */
#define uap_cpu_to_le64(x) swap_byte_64(x)

/** Convert APCMD header to little endian format from CPU format */
#define endian_convert_request_header(x)                \
    {                                                   \
        (x)->cmd_code = uap_cpu_to_le16((x)->cmd_code);   \
        (x)->size = uap_cpu_to_le16((x)->size);         \
        (x)->seq_num = uap_cpu_to_le16((x)->seq_num);     \
        (x)->result = uap_cpu_to_le16((x)->result);     \
    }

/** Convert APCMD header from little endian format to CPU format */
#define endian_convert_response_header(x)               \
    {                                                   \
        (x)->cmd_code = uap_le16_to_cpu((x)->cmd_code);   \
        (x)->size = uap_le16_to_cpu((x)->size);         \
        (x)->seq_num = uap_le16_to_cpu((x)->seq_num);     \
        (x)->result = uap_le16_to_cpu((x)->result);     \
    }

/** Convert TLV header to little endian format from CPU format */
#define endian_convert_tlv_header_out(x)            \
    {                                               \
        (x)->tag = uap_cpu_to_le16((x)->tag);       \
        (x)->length = uap_cpu_to_le16((x)->length); \
    }

/** Convert TLV header from little endian format to CPU format */
#define endian_convert_tlv_header_in(x)             \
    {                                               \
        (x)->tag = uap_le16_to_cpu((x)->tag);       \
        (x)->length = uap_le16_to_cpu((x)->length); \
    }

#else /* BIG_ENDIAN_SUPPORT */
/** Do nothing */
#define uap_le16_to_cpu(x) x
/** Do nothing */
#define uap_le32_to_cpu(x) x
/** Do nothing */
#define uap_le64_to_cpu(x) x
/** Do nothing */
#define uap_cpu_to_le16(x) x
/** Do nothing */
#define uap_cpu_to_le32(x) x
/** Do nothing */
#define uap_cpu_to_le64(x) x

/** Do nothing */
#define endian_convert_request_header(x)
/** Do nothing */
#define endian_convert_response_header(x)
/** Do nothing */
#define endian_convert_tlv_header_out(x)
/** Do nothing */
#define endian_convert_tlv_header_in(x)
#endif /* BIG_ENDIAN_SUPPORT */

/** Host Command ioctl number */
#define UAPHOSTCMD          (SIOCDEVPRIVATE + 1)
/** Private command ID to send ioctl */
#define UAP_IOCTL_CMD		(SIOCDEVPRIVATE + 2)
/** Updating ADDBA variables */
#define UAP_ADDBA_PARA          0
/** Updating priority table for AMPDU/AMSDU */
#define UAP_AGGR_PRIOTBL    1
/** Updating addbareject table */
#define UAP_ADDBA_REJECT    2
/** Get FW INFO */
#define UAP_FW_INFO         4
/** deep sleep subcommand */
#define UAP_DEEP_SLEEP      3
/** Tx data pause subcommand */
#define UAP_TX_DATA_PAUSE    5
/** sdcmd52 read write subcommand */
#define UAP_SDCMD52_RW      6
/** snmp mib subcommand */
#define UAP_SNMP_MIB        7
/** domain info subcommand */
#define UAP_DOMAIN_INFO     8
#if defined(DFS_TESTING_SUPPORT)
/** dfs testing subcommand */
#define UAP_DFS_TESTING     10
#endif
/** sub command ID to set/get Host Sleep configuration */
#define UAP_HS_CFG          11
/** sub command ID to set/get Host Sleep Parameters */
#define UAP_HS_SET_PARA     12

/** Management Frame Control Mask */
#define UAP_MGMT_FRAME_CONTROL  13
/** Tx rate configuration */
#define UAP_TX_RATE_CFG      14

/** Private command ID to Power Mode */
#define UAP_POWER_MODE      (SIOCDEVPRIVATE + 3)

/** Private command id to start/stop/reset bss */
#define UAP_BSS_CTRL        (SIOCDEVPRIVATE + 4)
/** BSS START */
#define UAP_BSS_START               0
/** BSS STOP */
#define UAP_BSS_STOP                1
/** BSS RESET */
#define UAP_BSS_RESET               2

/** deauth station */
#define	UAP_RADIO_CTL       (SIOCDEVPRIVATE + 5)

/** Private command ID to BSS config */
#define UAP_BSS_CONFIG      (SIOCDEVPRIVATE + 6)

/** deauth station */
#define	UAP_STA_DEAUTH	    (SIOCDEVPRIVATE + 7)

/** uap get station list */
#define UAP_GET_STA_LIST    (SIOCDEVPRIVATE + 11)

/** Private command ID to set/get custom IE buffer */
#define UAP_CUSTOM_IE       (SIOCDEVPRIVATE + 13)

/** Max IE index per BSS */
#define MAX_MGMT_IE_INDEX       12

/** HS_CFG: Get flag */
#define HS_CFG_FLAG_GET         0
/** HS_CFG: Set flag */
#define HS_CFG_FLAG_SET         1
/** HS_CFG: condition flag */
#define HS_CFG_FLAG_CONDITION   2
/** HS_CFG: gpio flag */
#define HS_CFG_FLAG_GPIO        4
/** HS_CFG: gap flag */
#define HS_CFG_FLAG_GAP         8
/** HS_CFG: all flags */
#define HS_CFG_FLAG_ALL         0x0f
/** HS_CFG: condition mask */
#define HS_CFG_CONDITION_MASK   0x4f

/** Host sleep config conditions : Cancel */
#define HS_CFG_CANCEL   0xffffffff

/** ds_hs_cfg */
typedef struct _ds_hs_cfg {
    /** subcmd */
	t_u32 subcmd;
    /** Bit0: 0 - Get, 1 Set
     *  Bit1: 1 - conditions is valid
     *  Bit2: 2 - gpio is valid
     *  Bit3: 3 - gap is valid
     */
	t_u32 flags;
    /** Host sleep config condition */
    /** Bit0: non-unicast data
     *  Bit1: unicast data
     *  Bit2: mac events
     *  Bit3: magic packet 
     */
	t_u32 conditions;
    /** GPIO */
	t_u32 gpio;
    /** Gap in milliseconds */
	t_u32 gap;
} ds_hs_cfg;

/** sleep_param */
typedef struct _ps_sleep_param {
    /** control bitmap */
	t_u32 ctrl_bitmap;
    /** minimum sleep period (micro second) */
	t_u32 min_sleep;
    /** maximum sleep period (micro second) */
	t_u32 max_sleep;
} ps_sleep_param;

/** inactivity sleep_param */
typedef struct _inact_sleep_param {
    /** inactivity timeout (micro second) */
	t_u32 inactivity_to;
    /** miniumu awake period (micro second) */
	t_u32 min_awake;
    /** maximum awake period (micro second) */
	t_u32 max_awake;
} inact_sleep_param;

/** flag for ps mode */
#define PS_FLAG_PS_MODE                 1
/** flag for sleep param */
#define PS_FLAG_SLEEP_PARAM             2
/** flag for inactivity sleep param */
#define PS_FLAG_INACT_SLEEP_PARAM       4

/** Disable power mode */
#define PS_MODE_DISABLE                      0
/** Enable periodic dtim ps */
#define PS_MODE_PERIODIC_DTIM                1
/** Enable inactivity ps */
#define PS_MODE_INACTIVITY                   2

/** sleep parameter */
#define SLEEP_PARAMETER                     1
/** inactivity sleep parameter */
#define INACTIVITY_SLEEP_PARAMETER          2

#if 0
/** sleep parameter : lower limit in micro-sec */
#define PS_SLEEP_PARAM_MIN                  5000
/** sleep parameter : upper limit in micro-sec */
#define PS_SLEEP_PARAM_MAX                  32000
/** power save awake period minimum value in micro-sec */
#define PS_AWAKE_PERIOD_MIN                 2000
#endif
/** ps_mgmt */
typedef struct _ps_mgmt {
    /** flags for valid field */
	t_u16 flags;
    /** power mode */
	t_u16 ps_mode;
    /** sleep param */
	ps_sleep_param sleep_param;
    /** inactivity sleep param */
	inact_sleep_param inact_param;
} ps_mgmt;

/** addba_param */
typedef struct _addba_param {
    /** subcmd */
	t_u32 subcmd;
    /** Set/Get */
	t_u32 action;
    /** block ack timeout for ADDBA request */
	t_u32 timeout;
    /** Buffer size for ADDBA request */
	t_u32 txwinsize;
    /** Buffer size for ADDBA response */
	t_u32 rxwinsize;
    /** amsdu for ADDBA request */
	t_u8 txamsdu;
    /** amsdu for ADDBA response */
	t_u8 rxamsdu;
} addba_param;

/** Default block ACK timeout */
#define DEFAULT_BLOCK_ACK_TIMEOUT   0xFFFF

/** Default block ACK timeout */
#define MAX_TXRX_WINDOW_SIZE        0x3FF

/** MAXIMUM number of TID */
#define MAX_NUM_TID     8

/** aggr_prio_tbl */
typedef struct _aggr_prio_tbl {
    /** subcmd */
	t_u32 subcmd;
    /** Set/Get */
	t_u32 action;
    /** ampdu priority table */
	t_u8 ampdu[MAX_NUM_TID];
    /** amsdu priority table */
	t_u8 amsdu[MAX_NUM_TID];
} aggr_prio_tbl;

/** addba_reject parameters */
typedef struct _addba_reject_para {
    /** subcmd */
	t_u32 subcmd;
    /** Set/Get */
	t_u32 action;
    /** BA Reject paramters */
	t_u8 addba_reject[MAX_NUM_TID];
} addba_reject_para;

/** fw_info */
typedef struct _fw_info {
    /** subcmd */
	t_u32 subcmd;
    /** Get */
	t_u32 action;
    /** Firmware release number */
	t_u32 fw_release_number;
    /** Device support for MIMO abstraction of MCSs */
	t_u8 hw_dev_mcs_support;
} fw_info;

#ifndef ETH_ALEN
/** MAC address length */
#define ETH_ALEN    6
#endif

/** deep_sleep parameters */
typedef struct _deep_sleep_para {
    /** subcmd */
	t_u32 subcmd;
    /** Set/Get */
	t_u32 action;
    /** enable/disable deepsleep*/
	t_u16 deep_sleep;
    /** idle_time */
	t_u16 idle_time;
} deep_sleep_para;
/** Default idle time for auto deep sleep */
#define DEEP_SLEEP_IDLE_TIME	100

/** tx_data_pause parameters */
typedef struct _tx_data_pause_para {
    /** subcmd */
	t_u32 subcmd;
    /** Set/Get */
	t_u32 action;
    /** enable/disable Tx data pause*/
	t_u16 txpause;
    /** Max number of TX buffer allowed for all PS client*/
	t_u16 txbufcnt;
} tx_data_pause_para;

/** Tx data pause disable */
#define TX_DATA_PAUSE_DISABLE   0
/** Tx data pause enable */
#define TX_DATA_PAUSE_ENABLE    1
/** Default maximum Tx buffer for all PS clients */
#define MAX_TX_BUF_CNT	        2

/** snmp_mib parameters */
typedef struct _snmp_mib_param {
    /** subcmd */
	t_u32 subcmd;
    /** Set/Get */
	t_u32 action;
    /** oid to set/get */
	t_u16 oid;
    /** length of oid value */
	t_u16 oid_val_len;
    /** oid value to set/get */
	t_u8 oid_value[0];
} snmp_mib_param;

/** domain_info parameters */
typedef struct _domain_info_param {
    /** subcmd */
	t_u32 subcmd;
    /** Set/Get */
	t_u32 action;
    /** domain_param TLV (incl. header) */
	t_u8 tlv[0];
} domain_info_param;

#if defined(DFS_TESTING_SUPPORT)
/** dfs_testing parameters */
typedef struct _dfs_testing_param {
    /** subcmd */
	t_u32 subcmd;
    /** Set/Get */
	t_u32 action;
    /** user CAC period (msec) */
	t_u16 usr_cac_period;
    /** user NOP period (sec) */
	t_u16 usr_nop_period;
    /** don't change channel on radar */
	t_u8 no_chan_change;
    /** fixed channel to change to on radar */
	t_u8 fixed_new_chan;
} dfs_testing_para;
#endif

/** mgmt_frame_ctrl */
typedef struct _mgmt_frame_ctrl {
    /** subcmd */
	t_u32 subcmd;
    /** Set/Get */
	t_u32 action;
    /** mask */
	t_u32 mask;
} mgmt_frame_ctrl;

/** Default device name */
#define DEFAULT_DEV_NAME    "uap0"

/** Success */
#define UAP_SUCCESS     1
/** Failure */
#define UAP_FAILURE     0
/** MAC BROADCAST */
#define UAP_RET_MAC_BROADCAST   0x1FF
/** MAC MULTICAST */
#define UAP_RET_MAC_MULTICAST   0x1FE

/** Command is successful */
#define CMD_SUCCESS     0
/** Command fails */
#define CMD_FAILURE     -1

/** BSS start error : Invalid parameters */
#define BSS_FAILURE_START_INVAL     2

/** Maximum line length for config file */
#define MAX_LINE_LENGTH         240
/** Maximum command length */
#define MAX_CMD_LENGTH          100
/** Size of command buffer */
#define MRVDRV_SIZE_OF_CMD_BUFFER       (2048)

/** Maximum number of clients supported by AP */
#define MAX_NUM_CLIENTS         16
/** Maximum number of MAC addresses for one-shot filter modifications */
#define MAX_MAC_ONESHOT_FILTER  16
/** Maximum SSID length */
#define MAX_SSID_LENGTH         32
/** Maximum SSID length */
#define MIN_SSID_LENGTH         1
/** Maximum WPA passphrase length */
#define MAX_WPA_PASSPHRASE_LENGTH   64
/** Minimum WPA passphrase length */
#define MIN_WPA_PASSPHRASE_LENGTH   8
/** Maximum data rates */
#define MAX_DATA_RATES          14
/** Maximum length of lines in configuration file */
#define MAX_CONFIG_LINE         240
/** MSB bit is set if its a basic rate */
#define BASIC_RATE_SET_BIT      0x80
/** Maximum group key timer */
#define MAX_GRP_TIMER           86400
/** Maximum Retry Limit */
#define MAX_RETRY_LIMIT         14
/** Maximum preamble type value */
#define MAX_PREAMBLE_TYPE       2

/** Maximum TX Power Limit */
#define MAX_TX_POWER    30
/** Minimum TX Power Limit */
#define MIN_TX_POWER    0

/** Maximum channel number in bg mode */
#define MAX_CHANNELS_BG 14
/** Maximum channels */
#define MAX_CHANNELS    165
#define DEFAULT_MAX_VALID_CHANNEL_BG 11

/** Max number of stations */
#define MAX_STA_COUNT       10

/** Maximum RTS threshold */
#define MAX_RTS_THRESHOLD   2347

/** Maximum fragmentation threshold */
#define MAX_FRAG_THRESHOLD 2346
/** Minimum fragmentation threshold */
#define MIN_FRAG_THRESHOLD 256

/** Maximum stage out time */
#define MAX_STAGE_OUT_TIME  864000
/** Minimum stage out time */
#define MIN_STAGE_OUT_TIME  300

/** Maximum DTIM period */
#define MAX_DTIM_PERIOD 100

/** Maximum BEACON period */
#define MAX_BEACON_PERIOD 4000

/** Minimum BEACON period */
#define MIN_BEACON_PERIOD 50

/** Maximum IE buffer length */
#define MAX_IE_BUFFER_LEN 256

/** Maximum custom IE count */
#define MAX_CUSTOM_IE_COUNT 4

/** Maximum number of rates allowed at a time */
#define MAX_RATES               12

/** Default wait period in seconds */
#define DEFAULT_WAIT_TIME       3

/** Maximum valid value of Deauth reason code */
#define MAX_DEAUTH_REASON_CODE     0xFFFF
#if 0
#ifdef __GNUC__
/** Structure packing begins */
#define PACK_START
/** Structure packeing end */
#define PACK_END  __attribute__ ((packed))
#else
/** Structure packing begins */
#define PACK_START   __packed
/** Structure packeing end */
#define PACK_END
#endif
#endif
/** Action field value : get */
#define ACTION_GET  0
/** Action field value : set */
#define ACTION_SET  1
/**
 * Hex or Decimal to Integer
 * @param   num string to convert into decimal or hex
 */
#define A2HEXDECIMAL(num)  \
    (strncasecmp("0x", (num), 2)?(unsigned int) strtoll((num),NULL,0):a2hex((num)))\

/** Find minimum value */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif /* MIN */

/** Valid Input Commands */
typedef enum {
	RDEEPROM,
	SCANCHANNELS,
	TXPOWER,
	PROTOCOL,
	CHANNEL,
	CHANNEL_EXT,
	BAND,
	RATE,
	BROADCASTSSID,
	RTSTHRESH,
	FRAGTHRESH,
	DTIMPERIOD,
	RADIOCONTROL,
	MCBCDATARATE,
	PKTFWD,
	STAAGEOUTTIMER,
	PSSTAAGEOUTTIMER,
	AUTHMODE,
	GROUPREKEYTIMER,
	MAXSTANUM,
	BEACONPERIOD,
	RETRYLIMIT,
	STICKYTIMCONFIG,
	STICKYTIMSTAMACADDR,
	COEX2040CONFIG,
	TXRATECFG,
	RSNREPLAYPROT,
	PREAMBLETYPE,
	EAPOL_PWK_HSK,
	EAPOL_GWK_HSK,
	COEX_COMM_BITMAP,
	COEX_COMM_AP_COEX,
	COEX_PROTECTION,
	COEX_SCO_ACL_FREQ,
	COEX_ACL_ENABLED,
	COEX_ACL_BT_TIME,
	COEX_ACL_WLAN_TIME,
	PWK_CIPHER,
	GWK_CIPHER,
} valid_inputs;

/** Message verbosity level */
enum { MSG_NONE, MSG_DEBUG, MSG_ALL };

/** oids_table */
typedef struct {
    /** oid type */
	t_u16 type;
    /** oid len */
	t_u16 len;
    /** oid name */
	char *name;
} oids_table;

/** 4 byte header to store buf len*/
#define BUF_HEADER_SIZE     4

/** TLV header length */
#define TLVHEADER_LEN       4

/** AP CMD header */
#define APCMDHEADER     /** Buf Size */         \
			t_u32 buf_size;         \
			/** Command Code */     \
			t_u16 cmd_code;         \
			/** Size */             \
			t_u16 size;             \
			/** Sequence Number */  \
			t_u16 seq_num;          \
			/** Result */           \
			t_s16 result

/** TLV header */
#define TLVHEADER       /** Tag */      \
			t_u16 tag;      \
			/** Length */   \
			t_u16 length

/* TLV Definitions */

/** TLV buffer header*/
typedef PACK_START struct _tlvbuf_header {
    /** Header type */
	t_u16 type;
    /** Header length */
	t_u16 len;
    /** Data */
	t_u8 data[0];
} PACK_END tlvbuf_header;

/** Band config ACS mode */
#define BAND_CONFIG_ACS_MODE    0x40
/** BITMAP for ACS mode */
#define BITMAP_ACS_MODE         0x01
/** Band Config mask */
#define BAND_CONFIG_MASK        0x03
/** Band Config 2.4GHz */
#define BAND_CONFIG_2_4GHZ      0x00
/** Band config 5GHz */
#define BAND_CONFIG_5GHZ        0x01

/** BITMAP for secondary channel above */
#define BITMAP_CHANNEL_ABOVE    0x02
/** BITMAP for secondary channel below */
#define BITMAP_CHANNEL_BELOW    0x04
/** Channle mode mask */
#define CHANNEL_MODE_MASK       0x07
/** secondary channel is below */
#define SECOND_CHANNEL_BELOW    0x30
/** secondary channel is above */
#define SECOND_CHANNEL_ABOVE    0x10
/** max primary channel support secondary channle above */
#define MAX_CHANNEL_ABOVE       7
/** min primary channel support secondary channle below */
#define MIN_CHANNEL_BELOW       5

/** Band B */
#define BAND_B          (0x01)
/** Band G */
#define BAND_G          (0x02)
/** Band A */
#define BAND_A          (0x04)

/** TLV buffer : Channel Config */
typedef PACK_START struct _tlvbuf_channel_config {
    /** Header */
	TLVHEADER;
    /** Band Configuration
     *
     * [7-6] Channel Selection Mode; 00 manual, 01 ACS
     * [5-4] 00 - no secondary channel,
     *       01 -- secondary channel is above
     *       03 -- secondary channel is below
     * [3-2] Channel Width; 00 20 MHz
     * [1-0] Band Info; 00 2.4 GHz
     */
	t_u8 band_config_type;
    /** Channel number */
	t_u8 chan_number;
} PACK_END tlvbuf_channel_config;

/** Channel List Entry */
typedef PACK_START struct _channel_list {
    /** Band Config */
	t_u8 band_config_type;
    /** Channel Number */
	t_u8 chan_number;
    /** Reserved */
	t_u8 reserved1;
    /** Reserved */
	t_u16 reserved2;
    /** Reserved */
	t_u16 reserved3;
} PACK_END channel_list;

/** TLV buffer : Channel List */
typedef PACK_START struct _tlvbuf_channel_list {
    /** Header */
	TLVHEADER;
    /** Channel List */
	channel_list chan_list[0];
} PACK_END tlvbuf_channel_list;

/** TLV buffer : AP MAC address */
typedef PACK_START struct _tlvbuf_ap_mac_address {
    /** Header */
	TLVHEADER;
    /** AP MAC address */
	t_u8 ap_mac_addr[ETH_ALEN];
} PACK_END tlvbuf_ap_mac_address;

/** TLV buffer : SSID */
typedef PACK_START struct _tlvbuf_ssid {
    /** Header */
	TLVHEADER;
    /** SSID */
	t_u8 ssid[0];
} PACK_END tlvbuf_ssid;

/** TLV buffer : Beacon period */
typedef PACK_START struct _tlvbuf_beacon_period {
    /** Header */
	TLVHEADER;
    /** Beacon period */
	t_u16 beacon_period_ms;
} PACK_END tlvbuf_beacon_period;

/** TLV buffer : DTIM period */
typedef PACK_START struct _tlvbuf_dtim_period {
    /** Header */
	TLVHEADER;
    /** DTIM period */
	t_u8 dtim_period;
} PACK_END tlvbuf_dtim_period;

/** TLV buffer : BSS status */
typedef PACK_START struct _tlvbuf_bss_status {
    /** Header */
	TLVHEADER;
    /** BSS status */
	t_u16 bss_status;
} PACK_END tlvbuf_bss_status;

/** TLV buffer : Channel */
typedef PACK_START struct _tlvbuf_phyparamdsset {
    /** Header */
	TLVHEADER;
    /** Channel */
	t_u8 channel;
} PACK_END tlvbuf_phyparamdsset;

/** TLV buffer : Operational rates */
typedef PACK_START struct _tlvbuf_rates {
    /** Header */
	TLVHEADER;
    /** Operational rates */
	t_u8 operational_rates[0];
} PACK_END tlvbuf_rates;

/** TLV buffer : Tx power */
typedef PACK_START struct _tlvbuf_tx_power {
    /** Header */
	TLVHEADER;
    /** Tx power in dBm */
	t_u8 tx_power_dbm;
} PACK_END tlvbuf_tx_power;

/** TLV buffer : SSID broadcast control */
typedef PACK_START struct _tlvbuf_bcast_ssid_ctl {
    /** Header */
	TLVHEADER;
    /** SSID broadcast control flag */
	t_u8 bcast_ssid_ctl;
} PACK_END tlvbuf_bcast_ssid_ctl;

/** TLV buffer : RSN replay protection */
typedef PACK_START struct _tlvbuf_rsn_replay_prot {
    /** Header */
	TLVHEADER;
    /** RSN replay protection control flag */
	t_u8 rsn_replay_prot;
} PACK_END tlvbuf_rsn_replay_prot;

/** TLV buffer : Preamble control */
typedef PACK_START struct _tlvbuf_preamble_ctl {
    /** Header */
	TLVHEADER;
    /** Preamble type */
	t_u8 preamble_type;
} PACK_END tlvbuf_preamble_ctl;

/** TLV buffer : Antenna control */
typedef PACK_START struct _tlvbuf_antenna_ctl {
    /** Header */
	TLVHEADER;
    /** Antenna type */
	t_u8 which_antenna;
    /** Antenna mode */
	t_u8 antenna_mode;
} PACK_END tlvbuf_antenna_ctl;

/** TLV buffer : RTS threshold */
typedef PACK_START struct _tlvbuf_rts_threshold {
    /** Header */
	TLVHEADER;
    /** RTS threshold */
	t_u16 rts_threshold;
} PACK_END tlvbuf_rts_threshold;

/** TLV buffer : MCBC Data Rate */
typedef PACK_START struct _tlvbuf_mcbc_data_rate {
    /** Header */
	TLVHEADER;
    /** MCBC data rate */
	t_u16 mcbc_datarate;
} PACK_END tlvbuf_mcbc_data_rate;

/** Packet forwarding to be done by FW or host */
#define PKT_FWD_FW_BIT  0x01
/** Intra-BSS broadcast packet forwarding allow bit */
#define PKT_FWD_INTRA_BCAST 0x02
/** Intra-BSS unicast packet forwarding allow bit */
#define PKT_FWD_INTRA_UCAST 0x04
/** Inter-BSS unicast packet forwarding allow bit */
#define PKT_FWD_INTER_UCAST 0x08
/** TLV buffer : Packet forward control */
typedef PACK_START struct _tlvbuf_pkt_fwd_ctl {
    /** Header */
	TLVHEADER;
    /** Packet forwarding control flag */
	t_u8 pkt_fwd_ctl;
} PACK_END tlvbuf_pkt_fwd_ctl;

/** sdcmd52rw parameters */
typedef struct _sdcmd52_para {
    /** subcmd */
	t_u32 subcmd;
    /** Write /Read */
	t_u32 action;
    /** Command 52 paramters */
	t_u8 cmd52_params[3];
} sdcmd52_para;

/** TLV buffer : STA MAC address filtering control */
typedef PACK_START struct _tlvbuf_sta_mac_addr_filter {
    /** Header */
	TLVHEADER;
    /** Filter mode */
	t_u8 filter_mode;
    /** Number of STA MACs */
	t_u8 count;
    /** STA MAC addresses buffer */
	t_u8 mac_address[0];
} PACK_END tlvbuf_sta_mac_addr_filter;

/** TLV buffer : STA ageout timer */
typedef PACK_START struct _tlvbuf_sta_ageout_timer {
    /** Header */
	TLVHEADER;
    /** STA ageout timer in ms */
	t_u32 sta_ageout_timer_ms;
} PACK_END tlvbuf_sta_ageout_timer;

/** TLV buffer : PS STA ageout timer */
typedef PACK_START struct _tlvbuf_ps_sta_ageout_timer {
    /** Header */
	TLVHEADER;
    /** PS STA ageout timer in ms */
	t_u32 ps_sta_ageout_timer_ms;
} PACK_END tlvbuf_ps_sta_ageout_timer;

/** TLV buffer : max station number */
typedef PACK_START struct _tlvbuf_max_sta_num {
    /** Header */
	TLVHEADER;
    /** max station number configured*/
	t_u16 max_sta_num_configured;
    /** max station number supported*/
	t_u16 max_sta_num_supported;
} PACK_END tlvbuf_max_sta_num;

/** TLV buffer : retry limit */
typedef PACK_START struct _tlvbuf_retry_limit {
    /** Header */
	TLVHEADER;
    /** Retry limit */
	t_u8 retry_limit;
} PACK_END tlvbuf_retry_limit;

/** TLV buffer : 2040 coex config */
typedef PACK_START struct _tlvbuf_2040_coex {
    /** Header */
	TLVHEADER;
    /** Enable */
	t_u8 enable;
} PACK_END tlvbuf_2040_coex;

/** TLV buffer : pairwise key handshake timeout */
typedef PACK_START struct _tlvbuf_eapol_pwk_hsk_timeout {
    /** Header */
	TLVHEADER;
    /** pairwise update timeout in milliseconds */
	t_u32 pairwise_update_timeout;
} PACK_END tlvbuf_eapol_pwk_hsk_timeout;

/** TLV buffer : pairwise key handshake number of retries */
typedef PACK_START struct _tlvbuf_eapol_pwk_hsk_retries {
    /** Header */
	TLVHEADER;
    /** pairwise key retries */
	t_u32 pwk_retries;
} PACK_END tlvbuf_eapol_pwk_hsk_retries;

/** TLV buffer : groupwise key handshake timeout */
typedef PACK_START struct _tlvbuf_eapol_gwk_hsk_timeout {
    /** Header */
	TLVHEADER;
    /** groupwise update timeout in milliseconds */
	t_u32 groupwise_update_timeout;
} PACK_END tlvbuf_eapol_gwk_hsk_timeout;

/** TLV buffer : groupwise key handshake number of retries */
typedef PACK_START struct _tlvbuf_eapol_gwk_hsk_retries {
    /** Header */
	TLVHEADER;
    /** groupwise key retries */
	t_u32 gwk_retries;
} PACK_END tlvbuf_eapol_gwk_hsk_retries;

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
    /** Header */
	TLVHEADER;
    /** custom IE data */
	custom_ie ie_data[0];
} PACK_END tlvbuf_custom_ie;

/** custom IE info */
typedef PACK_START struct _custom_ie_info {
    /** size of buffer */
	t_u16 buf_size;
    /** no of buffers of buf_size */
	t_u16 buf_count;
} PACK_END custom_ie_info;

/** TLV buffer : custom IE */
typedef PACK_START struct _tlvbuf_max_mgmt_ie {
    /** Header */
	TLVHEADER;
    /** No of tuples */
	t_u16 count;
    /** custom IE info tuples */
	custom_ie_info info[0];
} PACK_END tlvbuf_max_mgmt_ie;

/* Bitmap for protocol to use */
/** No security */
#define PROTOCOL_NO_SECURITY        1
/** Static WEP */
#define PROTOCOL_STATIC_WEP         2
/** WPA */
#define PROTOCOL_WPA                8
/** WPA2 */
#define PROTOCOL_WPA2               32
/** WP2 Mixed */
#define PROTOCOL_WPA2_MIXED         40

/* Bitmap for unicast/bcast cipher type */
/** None */
#define CIPHER_NONE                 0
/** WEP 40 */
#define CIPHER_WEP_40               1
/** WEP 104 */
#define CIPHER_WEP_104              2
/** TKIP */
#define CIPHER_TKIP                 4
/** AES CCMP */
#define CIPHER_AES_CCMP             8
/** Valid cipher bitmap */
#define CIPHER_BITMAP               0x0c
/** Valid protocol bitmap */
#define PROTOCOL_BITMAP             0x28
/** AES CCMP + TKIP cipher */
#define AES_CCMP_TKIP               12

/** TLV buffer : Authentication Mode */
typedef PACK_START struct _tlvbuf_auth_mode {
    /** Header */
	TLVHEADER;
    /** Authentication Mode */
	t_u8 auth_mode;
} PACK_END tlvbuf_auth_mode;

/** TLV buffer : Security Protocol */
typedef PACK_START struct _tlvbuf_protocol {
    /** Header */
	TLVHEADER;
    /** Security protocol */
	t_u16 protocol;
} PACK_END tlvbuf_protocol;

/** TLV buffer : cipher */
typedef PACK_START struct _tlvbuf_cipher {
    /** Header */
	TLVHEADER;
    /** Pairwise cipher */
	t_u8 pairwise_cipher;
    /** Group cipher */
	t_u8 group_cipher;
} PACK_END tlvbuf_cipher;

/** TLV buffer : Pairwise cipher */
typedef PACK_START struct _tlvbuf_pwk_cipher {
    /** Header */
	TLVHEADER;
    /** Protocol */
	t_u16 protocol;
    /** Pairwise cipher */
	t_u8 pairwise_cipher;
    /** Reserved */
	t_u8 reserved;
} PACK_END tlvbuf_pwk_cipher;

/** TLV buffer : Group cipher */
typedef PACK_START struct _tlvbuf_gwk_cipher {
    /** Header */
	TLVHEADER;
    /** Group cipher */
	t_u8 group_cipher;
    /** Reserved*/
	t_u8 reserved;
} PACK_END tlvbuf_gwk_cipher;

/** TLV buffer : Group re-key time */
typedef PACK_START struct _tlvbuf_group_rekey_timer {
    /** Header */
	TLVHEADER;
    /** Group rekey time in seconds */
	t_u32 group_rekey_time_sec;
} PACK_END tlvbuf_group_rekey_timer;

/** Key_mgmt_psk */
#define KEY_MGMT_NONE   0x04
/** Key_mgmt_none */
#define KEY_MGMT_PSK    0x02
/** Wmm Max AC queues */
#define MAX_AC_QUEUES   4

/** TLV buffer : KeyMgmt */
typedef PACK_START struct _tlvbuf_akmp {
    /** Header */
	TLVHEADER;
    /** KeyMgmt */
	t_u16 key_mgmt;
    /** key management operation */
	t_u16 key_mgmt_operation;
} PACK_END tlvbuf_akmp;

/** TLV buffer : Single WEP key */
typedef PACK_START struct _tlvbuf_wep_key {
    /** Header */
	TLVHEADER;
    /** Key index */
	t_u8 key_index;
    /** Default key flag */
	t_u8 is_default;
    /** Key */
	t_u8 key[0];
} PACK_END tlvbuf_wep_key;

/** TLV buffer : WPA passphrase */
typedef PACK_START struct _tlvbuf_wpa_passphrase {
    /** Header */
	TLVHEADER;
    /** WPA passphrase */
	t_u8 passphrase[0];
} PACK_END tlvbuf_wpa_passphrase;

/** TLV buffer : Fragmentation threshold */
typedef PACK_START struct _tlvbuf_frag_threshold {
    /** Header */
	TLVHEADER;
    /** Fragmentation threshold */
	t_u16 frag_threshold;
} PACK_END tlvbuf_frag_threshold;

/* APCMD definitions */
/** APCMD buffer */
typedef PACK_START struct _apcmdbuf {
    /** Header */
	APCMDHEADER;
} PACK_END apcmdbuf;

/** APCMD header length */
#define APCMDHEADERLEN  (sizeof(apcmdbuf))

/** APCMD buffer : sys_info request */
typedef PACK_START struct _apcmdbuf_sys_info_request {
    /** Header */
	APCMDHEADER;
} PACK_END apcmdbuf_sys_info_request;

/** APCMD buffer : sys_info response */
typedef PACK_START struct _apcmdbuf_sys_info_response {
    /** Header */
	APCMDHEADER;
    /** System information buffer */
	t_u8 sys_info[64];
} PACK_END apcmdbuf_sys_info_response;

/** APCMD buffer : sys_reset */
typedef PACK_START struct _apcmdbuf_sys_reset {
    /** Header */
	APCMDHEADER;
} PACK_END apcmdbuf_sys_reset;

/** APCMD buffer : sys_configure */
typedef PACK_START struct _apcmdbuf_sys_configure {
    /** Header */
	APCMDHEADER;
    /** Action : GET or SET */
	t_u16 action;
} PACK_END apcmdbuf_sys_configure;

/* Max transmit power for indoor operation */
#define MAX_TX_PWR_INDOOR    17

/** APCMD buffer : SNMP MIB */
typedef PACK_START struct _apcmdbuf_snmp_mib {
    /** Header */
	APCMDHEADER;
    /** Action : GET or SET */
	t_u16 action;
} PACK_END apcmdbuf_snmp_mib;
/** APCMD buffer : bss_start */
typedef PACK_START struct _apcmdbuf_bss_start {
    /** Header */
	APCMDHEADER;
} PACK_END apcmdbuf_bss_start;

/** APCMD buffer : bss_stop */
typedef PACK_START struct _apcmdbuf_bss_stop {
    /** Header */
	APCMDHEADER;
} PACK_END apcmdbuf_bss_stop;

/** APCMD buffer : sta_deauth */
typedef PACK_START struct _APCMDBUF_STA_DEAUTH {
    /** Header */
	APCMDHEADER;
    /** STA MAC address to deauthenticate */
	t_u8 sta_mac_address[ETH_ALEN];
} PACK_END APCMDBUF_STA_DEAUTH;

/** Reg TYPE*/
enum reg_commands {
	CMD_MAC = 1,
	CMD_BBP,
	CMD_RF
};

/** APCMD buffer: Regrdwr */
typedef PACK_START struct _apcmdbuf_reg_rdwr {
   /** Header */
	APCMDHEADER;
   /** Read or Write */
	t_u16 action;
   /** Register offset */
	t_u16 offset;
   /** Value */
	t_u32 value;
} PACK_END apcmdbuf_reg_rdwr;

/** sub-band type */
typedef PACK_START struct _ieeetypes_subband_set {
	t_u8 first_chan;    /**< First channel */
	t_u8 no_of_chan;    /**< Number of channels */
	t_u8 max_tx_pwr;    /**< Maximum Tx power */
} PACK_END ieeetypes_subband_set_t;

/** country code length  used for 802.11D */
#define COUNTRY_CODE_LEN    3

/** MAX domain SUB-BAND*/
#define MAX_SUB_BANDS 40

/** Max Multi Domain Entries for G */
#define MAX_MULTI_DOMAIN_CAPABILITY_ENTRY_G 1

/** Max Multi Domain Entries for A */
#define MAX_MULTI_DOMAIN_CAPABILITY_ENTRY_A 31

/** Country code and Sub-band */
typedef PACK_START struct domain_param {
    /** Header */
	TLVHEADER;
	t_u8 country_code[COUNTRY_CODE_LEN];	 /**< Country code */
	ieeetypes_subband_set_t subband[0];	 /**< Set of subbands */
} PACK_END domain_param_t;

/** HostCmd_CFG_80211D */
typedef PACK_START struct _apcmdbuf_cfg_80211d {
    /** Header */
	APCMDHEADER;
    /** Action */
	t_u16 action;		/* 0 = ACT_GET; 1 = ACT_SET; */
    /** Domain parameters */
	domain_param_t domain;
} PACK_END apcmdbuf_cfg_80211d;

/** HostCmd_MEM_ACCESS */
typedef PACK_START struct _apcmdbuf_mem_access {
    /** Header */
	APCMDHEADER;
    /** Action */
	t_u16 action;		/* 0 = ACT_GET; 1 = ACT_SET; */
    /** Reserved field */
	t_u16 reserved;
    /** Address */
	t_u32 address;
    /** Value */
	t_u32 value;
} PACK_END apcmdbuf_mem_access;

/** HostCmd_EEPROM_ACCESS */
typedef PACK_START struct _apcmdbuf_eeprom_access {
    /** Header */
	APCMDHEADER;
    /** Action */
	t_u16 action;		/* 0 = ACT_GET; */
    /** Offset field */
	t_u16 offset;		/* Multiples of 4 */
    /** Byte count */
	t_u16 byte_count;	/* Multiples of 4 */
    /** Value */
	t_u8 value[1];
} PACK_END apcmdbuf_eeprom_access;

/** TLV : BT Coex common configuration */
typedef PACK_START struct _tlvbuf_coex_common_cfg {
    /** Header */
	TLVHEADER;
    /** Configuration bitmap */
	t_u32 config_bitmap;
    /** AP Bt Coex Enabled or not */
	t_u32 ap_bt_coex;
    /** Reserved */
	t_u32 reserved[3];
} PACK_END tlvbuf_coex_common_cfg;

/** TLV : BT Coex SCO configuration */
typedef PACK_START struct _tlvbuf_coex_sco_cfg {
    /** Header */
	TLVHEADER;
    /** Qtime protection */
	t_u16 protection_qtime[4];
    /** Rate protection */
	t_u16 protection_rate;
    /** ACL frequency */
	t_u16 acl_frequency;
    /** Reserved */
	t_u32 reserved[4];
} PACK_END tlvbuf_coex_sco_cfg;

/** TLV : BT Coex ACL configuration */
typedef PACK_START struct _tlvbuf_coex_acl_cfg {
    /** Header */
	TLVHEADER;
    /** Enabled or not */
	t_u16 enabled;
    /** BT time */
	t_u16 bt_time;
    /** Wlan time */
	t_u16 wlan_time;
    /** Rate protection */
	t_u16 protection_rate;
    /** Reserved */
	t_u32 reserved[4];
} PACK_END tlvbuf_coex_acl_cfg;

/** TLV : BT Coex statistics */
typedef PACK_START struct _tlvbuf_coex_stats {
    /** Header */
	TLVHEADER;
    /** Null not sent */
	t_u32 null_not_sent;
    /** Null queued */
	t_u32 null_queued;
    /** Null not queued */
	t_u32 null_not_queued;
    /** CF end queued */
	t_u32 cf_end_queued;
    /** CF end not queued */
	t_u32 cf_end_not_queued;
    /** Null allocation failures */
	t_u32 null_alloc_fail;
    /** CF end allocation failures */
	t_u32 cf_end_alloc_fail;
    /** Reserved */
	t_u32 reserved[8];
} PACK_END tlvbuf_coex_stats;

/** APCMD buffer : BT Coex API extension */
typedef PACK_START struct _apcmdbuf_coex_config {
    /** Header */
	APCMDHEADER;
    /** Action : GET or SET */
	t_u16 action;
    /** Reserved for alignment */
	t_u16 coex_reserved;
    /** TLV buffer */
	t_u8 tlv_buffer[0];
} PACK_END apcmdbuf_coex_config;

/** BIT value */
#define MBIT(x)    (((t_u32)1) << (x))
/** RadioType : Support for 40Mhz channel BW */
#define IS_CHANNEL_WIDTH_40(Field2) (Field2 & MBIT(2))
/** RadioType : Get secondary channel */
#define GET_SECONDARY_CHAN(Field2) (Field2 & (MBIT(0) | MBIT(1)))
/** RadioType : Is RIFS allowed */
#define IS_RIFS_ALLOWED(Field2) (Field2 & MBIT(3))
/** RadioType : Get HT Protection */
#define GET_HT_PROTECTION(Field3) (Field3 & (MBIT(0) | MBIT(1)))
/** RadioType : Are Non-GreenField STAs present */
#define NONGF_STA_PRESENT(Field3) (Field3 & MBIT(2))
/** RadioType : Are OBSS Non-HT STAs present */
#define OBSS_NONHT_STA_PRESENT(Field3) (Field3 & MBIT(4))
/** HT Capabilities Data */
typedef PACK_START struct _HTCap_t {
    /** HT Capabilities Info field */
	t_u16 ht_cap_info;
    /** A-MPDU Parameters field */
	t_u8 ampdu_param;
    /** Supported MCS Set field */
	t_u8 supported_mcs_set[16];
    /** HT Extended Capabilities field */
	t_u16 ht_ext_cap;
    /** Transmit Beamforming Capabilities field */
	t_u32 tx_bf_cap;
    /** Antenna Selection Capability field */
	t_u8 asel;
} PACK_END HTCap_t, *pHTCap_t;
/** HT Information Data */
typedef PACK_START struct _HTInfo_t {
    /** Primary channel */
	t_u8 pri_chan;
    /** Field 2 */
	t_u8 field2;
    /** Field 3 */
	t_u16 field3;
    /** Field 4 */
	t_u16 field4;
    /** Bitmap indicating MCSs supported by all HT STAs in the BSS */
	t_u8 basic_mcs_set[16];
} PACK_END HTInfo_t, *pHTInfo_t;

/** MLAN 802.11 MAC Address */
typedef t_u8 mlan_802_11_mac_addr[ETH_ALEN];

/** mlan_802_11_ssid data structure */
typedef struct _mlan_802_11_ssid {
    /** SSID Length */
	t_u32 ssid_len;
    /** SSID information field */
	t_u8 ssid[MAX_SSID_LENGTH];
} mlan_802_11_ssid;

/** scan_chan_list data structure */
typedef struct _scan_chan_list {
    /** Channel number*/
	t_u8 chan_number;
    /** Band config type */
	t_u8 band_config_type;
} scan_chan_list;

/** mac_filter data structure */
typedef struct _mac_filter {
    /** Mac filter mode */
	t_u16 filter_mode;
    /** Mac adress count */
	t_u16 mac_count;
    /** Mac address list */
	mlan_802_11_mac_addr mac_list[MAX_MAC_ONESHOT_FILTER];
} mac_filter;

/** Data structure of WMM Aci/Aifsn */
typedef PACK_START struct _IEEEtypes_WmmAciAifsn_t {
#ifdef BIG_ENDIAN_SUPPORT
    /** Reserved */
	t_u8 reserved:1;
    /** Aci */
	t_u8 aci:2;
    /** Acm */
	t_u8 acm:1;
    /** Aifsn */
	t_u8 aifsn:4;
#else
    /** Aifsn */
	t_u8 aifsn:4;
    /** Acm */
	t_u8 acm:1;
    /** Aci */
	t_u8 aci:2;
    /** Reserved */
	t_u8 reserved:1;
#endif				/* BIG_ENDIAN_SUPPORT */
} PACK_END IEEEtypes_WmmAciAifsn_t, *pIEEEtypes_WmmAciAifsn_t;

/** Data structure of WMM ECW */
typedef PACK_START struct _IEEEtypes_WmmEcw_t {
#ifdef BIG_ENDIAN_SUPPORT
    /** Maximum Ecw */
	t_u8 ecw_max:4;
    /** Minimum Ecw */
	t_u8 ecw_min:4;
#else
    /** Minimum Ecw */
	t_u8 ecw_min:4;
    /** Maximum Ecw */
	t_u8 ecw_max:4;
#endif				/* BIG_ENDIAN_SUPPORT */
} PACK_END IEEEtypes_WmmEcw_t, *pIEEEtypes_WmmEcw_t;

/** Data structure of WMM AC parameters  */
typedef PACK_START struct _IEEEtypes_WmmAcParameters_t {
	IEEEtypes_WmmAciAifsn_t aci_aifsn;   /**< AciAifSn */
	IEEEtypes_WmmEcw_t ecw;		    /**< Ecw */
	t_u16 tx_op_limit;		      /**< Tx op limit */
} PACK_END IEEEtypes_WmmAcParameters_t, *pIEEEtypes_WmmAcParameters_t;

/** Data structure of WMM parameter IE  */
typedef PACK_START struct _WmmParameter_t {
    /** OuiType:  00:50:f2:02 */
	t_u8 ouitype[4];
    /** Oui subtype: 01 */
	t_u8 ouisubtype;
    /** version: 01 */
	t_u8 version;
    /** QoS information */
	t_u8 qos_info;
    /** Reserved */
	t_u8 reserved;
    /** AC Parameters Record WMM_AC_BE, WMM_AC_BK, WMM_AC_VI, WMM_AC_VO */
	IEEEtypes_WmmAcParameters_t ac_params[MAX_AC_QUEUES];
} PACK_END WmmParameter_t, *pWmmParameter_t;

/** wpa parameter */
typedef struct _wpa_param {
    /** Pairwise cipher WPA */
	t_u8 pairwise_cipher_wpa;
    /** Pairwise cipher WPA2 */
	t_u8 pairwise_cipher_wpa2;
    /** Group cipher */
	t_u8 group_cipher;
    /** RSN replay protection */
	t_u8 rsn_protection;
    /** Passphrase length */
	t_u32 length;
    /** Passphrase */
	t_u8 passphrase[64];
    /** Group key rekey time */
	t_u32 gk_rekey_time;
} wpa_param;

/** wep key */
typedef struct _wep_key {
    /** Key index 0-3 */
	t_u8 key_index;
    /** Is default */
	t_u8 is_default;
    /** Length */
	t_u16 length;
    /** Key data */
	t_u8 key[26];
} wep_key;

/** wep param */
typedef struct _wep_param {
    /** Key 0 */
	wep_key key0;
    /** Key 1 */
	wep_key key1;
    /** Key 2 */
	wep_key key2;
    /** Key 3 */
	wep_key key3;
} wep_param;

/** BSS config structure */
typedef struct _bss_config_t {
    /** AP mac addr */
	mlan_802_11_mac_addr mac_addr;
    /** SSID */
	mlan_802_11_ssid ssid;
    /** Broadcast ssid control */
	t_u8 bcast_ssid_ctl;
    /** Radio control: on/off */
	t_u8 radio_ctl;
    /** dtim period */
	t_u8 dtim_period;
    /** beacon period */
	t_u16 beacon_period;
    /** rates */
	t_u8 rates[MAX_DATA_RATES];
    /** Tx data rate */
	t_u16 tx_data_rate;
    /** multicast/broadcast data rate */
	t_u16 mcbc_data_rate;
    /** Tx power level */
	t_u8 tx_power_level;
    /** Tx antenna */
	t_u8 tx_antenna;
    /** Rx anteena */
	t_u8 rx_antenna;
    /** packet forward control */
	t_u8 pkt_forward_ctl;
    /** max station count */
	t_u16 max_sta_count;
    /** mac filter */
	mac_filter filter;
    /** station ageout timer in the unit of 100ms  */
	t_u32 sta_ageout_timer;
    /** PS station ageout timer in the unit of 100ms  */
	t_u32 ps_sta_ageout_timer;
    /** RTS threshold */
	t_u16 rts_threshold;
    /** fragmentation threshold */
	t_u16 frag_threshold;
    /**  retry_limit */
	t_u16 retry_limit;
    /**  pairwise update timeout in milliseconds */
	t_u32 pairwise_update_timeout;
    /**  pairwise handshake retries */
	t_u32 pwk_retries;
    /**  groupwise update timeout in milliseconds */
	t_u32 groupwise_update_timeout;
    /**  groupwise handshake retries */
	t_u32 gwk_retries;
    /** preamble type */
	t_u8 preamble_type;
    /** band cfg */
	t_u8 band_cfg;
    /** channel */
	t_u8 channel;
    /** auth mode */
	t_u16 auth_mode;
    /** encryption protocol */
	t_u16 protocol;
    /** key managment type */
	t_u16 key_mgmt;
    /** wep param */
	wep_param wep_cfg;
    /** wpa param */
	wpa_param wpa_cfg;
    /** Mgmt IE passthru mask */
	t_u32 mgmt_ie_passthru_mask;
	/* 
	 *  11n HT Cap 
	 */
    /** HT Capabilities Info field */
	t_u16 ht_cap_info;
    /** A-MPDU Parameters field */
	t_u8 ampdu_param;
    /** Supported MCS Set field */
	t_u8 supported_mcs_set[16];
    /** HT Extended Capabilities field */
	t_u16 ht_ext_cap;
    /** Transmit Beamforming Capabilities field */
	t_u32 tx_bf_cap;
    /** Antenna Selection Capability field */
	t_u8 asel;
    /** Enable 20/40 coex */
	t_u8 enable_2040coex;
    /** key managment operation */
	t_u16 key_mgmt_operation;
    /** BSS status */
	t_u16 bss_status;
#ifdef WIFI_DIRECT_SUPPORT
	/* pre shared key */
	t_u8 psk[32];
#endif				/* WIFI_DIRECT_SUPPORT */
    /** Number of channels in scan_channel_list */
	t_u32 num_of_chan;
    /** scan channel list in ACS mode */
	scan_chan_list chan_list[MAX_CHANNELS];
    /** Wmm parameters */
	WmmParameter_t wmm_para;
} bss_config_t;

/** station info */
typedef struct _sta_info {
    /** STA MAC address */
	t_u8 mac_address[ETH_ALEN];
    /** Power mfg status */
	t_u8 power_mfg_status;
    /** RSSI */
	t_s8 rssi;
} sta_info;

/** sta_list structure */
typedef struct _sta_list {
    /** station count */
	t_u16 sta_count;
    /** station list */
	sta_info info[MAX_NUM_CLIENTS];
} sta_list;

/** mlan_deauth_param */
typedef struct _deauth_param {
    /** STA mac addr */
	t_u8 mac_addr[ETH_ALEN];
    /** deauth reason */
	t_u16 reason_code;
} deauth_param;

/** APCMD buffer : bss_configure */
typedef PACK_START struct _apcmdbuf_bss_configure {
    /** Action : GET or SET */
	t_u32 action;
} PACK_END apcmdbuf_bss_configure;

/** Max EEPROM length */
#define MAX_EEPROM_LEN         20

/** Subcmd id for global flag */
#define DEBUG_SUBCOMMAND_GMODE          1
/** Subcmd id for Majorid mask */
#define DEBUG_SUBCOMMAND_MAJOREVTMASK   2
/** Subcmd id to trigger a scan */
#define DEBUG_SUBCOMMAND_CHANNEL_SCAN   3

/** Channel scan entry for each channel */
typedef PACK_START struct _channel_scan_entry_t {
    /** Channel Number */
	t_u8 chan_num;
    /** Number of APs */
	t_u8 num_of_aps;
    /** CCA count */
	t_u32 cca_count;
    /** Duration */
	t_u32 duration;
    /** Channel weight */
	t_u32 channel_weight;
} PACK_END channel_scan_entry_t;

/** Channel scan entry */
typedef PACK_START struct _channel_scan_entry {
    /** Number of channels */
	t_u8 num_channels;
    /** Channel scan entry */
	channel_scan_entry_t cst[0];
} PACK_END channel_scan_entry;

/** HostCmd_CFG_DATA */
typedef PACK_START struct _apcmdbuf_cfg_data {
    /** Header */
	APCMDHEADER;
    /** Action */
	t_u16 action;
    /** Type */
	t_u16 type;
    /** Data length */
	t_u16 data_len;
    /** Data */
	t_u8 data[0];
} PACK_END apcmdbuf_cfg_data;

/** Host Command ID bit mask (bit 11:0) */
#define HostCmd_CMD_ID_MASK             0x0fff
/** APCMD response check */
#define APCMD_RESP_CHECK            0x8000

/* AP CMD IDs */
/** APCMD : sys_info */
#define APCMD_SYS_INFO              0x00ae
/** APCMD : sys_reset */
#define APCMD_SYS_RESET             0x00af
/** APCMD : sys_configure */
#define APCMD_SYS_CONFIGURE         0x00b0
/** APCMD : bss_start */
#define APCMD_BSS_START             0x00b1
/** APCMD : bss_stop */
#define APCMD_BSS_STOP              0x00b2
/** APCMD : sta_list */
#define APCMD_STA_LIST              0x00b3
/** APCMD : sta_deauth */
#define APCMD_STA_DEAUTH            0x00b5
/** SNMP MIB SET/GET */
#define HostCmd_SNMP_MIB            0x0016
/** Read/Write Mac register */
#define HostCmd_CMD_MAC_REG_ACCESS  0x0019
/** Read/Write BBP register */
#define HostCmd_CMD_BBP_REG_ACCESS  0x001a
/** Read/Write RF register */
#define HostCmd_CMD_RF_REG_ACCESS   0x001b
/** Host Command ID : EEPROM access */
#define HostCmd_EEPROM_ACCESS       0x0059
/** Host Command ID : Memory access */
#define HostCmd_CMD_MEM_ACCESS      0x0086
/** Host Command ID : 802.11D configuration */
#define HostCmd_CMD_802_11D_DOMAIN_INFO      0x005b
/** Host Command ID : Configuration data */
#define HostCmd_CMD_CFG_DATA        0x008f

/** Oid for 802.11D enable/disable */
#define OID_80211D_ENABLE           0x0009
/** Oid for 802.11H enable/disable */
#define OID_80211H_ENABLE           0x000a

/** Host Command ID:  ROBUST_COEX */
#define HostCmd_ROBUST_COEX         0x00e0

/* TLV IDs */
/** TLV : Base */
#define PROPRIETARY_TLV_BASE_ID         0x0100

/** TLV : SSID */
#define MRVL_SSID_TLV_ID                0x0000
/** TLV : Operational rates */
#define MRVL_RATES_TLV_ID               0x0001
/** TLV : Channel */
#define MRVL_PHYPARAMDSSET_TLV_ID       0x0003
/**TLV: Domain type */
#define TLV_TYPE_DOMAIN                 0x0007
/** TLV type : Scan Channels list */
#define MRVL_CHANNELLIST_TLV_ID         (PROPRIETARY_TLV_BASE_ID + 0x01)
/** TLV type : Authentication type */
#define MRVL_AUTH_TLV_ID                (PROPRIETARY_TLV_BASE_ID + 0x1f)
/** TLV Id : Channel Config */
#define MRVL_CHANNELCONFIG_TLV_ID       (PROPRIETARY_TLV_BASE_ID + 0x2a)
/** TLV : AP MAC address */
#define MRVL_AP_MAC_ADDRESS_TLV_ID      (PROPRIETARY_TLV_BASE_ID + 0x2b)
/** TLV : Beacon period */
#define MRVL_BEACON_PERIOD_TLV_ID       (PROPRIETARY_TLV_BASE_ID + 0x2c)
/** TLV : DTIM period */
#define MRVL_DTIM_PERIOD_TLV_ID         (PROPRIETARY_TLV_BASE_ID + 0x2d)
/** TLV : Tx power */
#define MRVL_TX_POWER_TLV_ID            (PROPRIETARY_TLV_BASE_ID + 0x2f)
/** TLV : SSID broadcast control */
#define MRVL_BCAST_SSID_CTL_TLV_ID      (PROPRIETARY_TLV_BASE_ID + 0x30)
/** TLV : Preamble control */
#define MRVL_PREAMBLE_CTL_TLV_ID        (PROPRIETARY_TLV_BASE_ID + 0x31)
/** TLV : Antenna control */
#define MRVL_ANTENNA_CTL_TLV_ID         (PROPRIETARY_TLV_BASE_ID + 0x32)
/** TLV : RTS threshold */
#define MRVL_RTS_THRESHOLD_TLV_ID       (PROPRIETARY_TLV_BASE_ID + 0x33)
/** TLV : Packet forwarding control */
#define MRVL_PKT_FWD_CTL_TLV_ID         (PROPRIETARY_TLV_BASE_ID + 0x36)
/** TLV : STA information */
#define MRVL_STA_INFO_TLV_ID            (PROPRIETARY_TLV_BASE_ID + 0x37)
/** TLV : STA MAC address filter */
#define MRVL_STA_MAC_ADDR_FILTER_TLV_ID (PROPRIETARY_TLV_BASE_ID + 0x38)
/** TLV : STA ageout timer */
#define MRVL_STA_AGEOUT_TIMER_TLV_ID    (PROPRIETARY_TLV_BASE_ID + 0x39)
/** TLV : WEP keys */
#define MRVL_WEP_KEY_TLV_ID             (PROPRIETARY_TLV_BASE_ID + 0x3b)
/** TLV : WPA passphrase */
#define MRVL_WPA_PASSPHRASE_TLV_ID      (PROPRIETARY_TLV_BASE_ID + 0x3c)
/** TLV type : protocol TLV */
#define MRVL_PROTOCOL_TLV_ID            (PROPRIETARY_TLV_BASE_ID + 0x40)
/** TLV type : AKMP TLV */
#define MRVL_AKMP_TLV_ID                (PROPRIETARY_TLV_BASE_ID + 0x41)
/** TLV type : Cipher TLV */
#define MRVL_CIPHER_TLV_ID              (PROPRIETARY_TLV_BASE_ID + 0x42)
/** TLV : Fragment threshold */
#define MRVL_FRAG_THRESHOLD_TLV_ID      (PROPRIETARY_TLV_BASE_ID + 0x46)
/** TLV : Group rekey timer */
#define MRVL_GRP_REKEY_TIME_TLV_ID      (PROPRIETARY_TLV_BASE_ID + 0x47)
/**TLV: Max Station number */
#define MRVL_MAX_STA_CNT_TLV_ID         (PROPRIETARY_TLV_BASE_ID + 0x55)
/**TLV: Retry limit */
#define MRVL_RETRY_LIMIT_TLV_ID         (PROPRIETARY_TLV_BASE_ID + 0x5d)
/**TLV: MCBC data rate */
#define MRVL_MCBC_DATA_RATE_TLV_ID      (PROPRIETARY_TLV_BASE_ID + 0x62)
/**TLV: RSN replay protection */
#define MRVL_RSN_REPLAY_PROT_TLV_ID     (PROPRIETARY_TLV_BASE_ID + 0x64)
/** TLV: Management IE list */
#define MRVL_MGMT_IE_LIST_TLV_ID        (PROPRIETARY_TLV_BASE_ID + 0x69)
/** TLV : Coex common configuration */
#define MRVL_BT_COEX_COMMON_CFG_TLV_ID  (PROPRIETARY_TLV_BASE_ID + 0x6c)
/** TLV : Coex SCO configuration */
#define MRVL_BT_COEX_SCO_CFG_TLV_ID     (PROPRIETARY_TLV_BASE_ID + 0x6d)
/** TLV : Coex ACL configuration */
#define MRVL_BT_COEX_ACL_CFG_TLV_ID     (PROPRIETARY_TLV_BASE_ID + 0x6e)
/** TLV : Coex stats configuration */
#define MRVL_BT_COEX_STATS_TLV_ID       (PROPRIETARY_TLV_BASE_ID + 0x6f)
/** TLV :Pairwise Handshake Timeout */
#define MRVL_EAPOL_PWK_HSK_TIMEOUT_TLV_ID   (PROPRIETARY_TLV_BASE_ID + 0x75)
/** TLV :Pairwise Handshake Retries */
#define MRVL_EAPOL_PWK_HSK_RETRIES_TLV_ID   (PROPRIETARY_TLV_BASE_ID + 0x76)
/** TLV :Groupwise Handshake Timeout */
#define MRVL_EAPOL_GWK_HSK_TIMEOUT_TLV_ID   (PROPRIETARY_TLV_BASE_ID + 0x77)
/** TLV :Groupwise Handshake Retries */
#define MRVL_EAPOL_GWK_HSK_RETRIES_TLV_ID   (PROPRIETARY_TLV_BASE_ID + 0x78)
/** TLV : PS STA ageout timer */
#define MRVL_PS_STA_AGEOUT_TIMER_TLV_ID     (PROPRIETARY_TLV_BASE_ID + 0x7b)
/** TLV : Pairwise Cipher */
#define MRVL_CIPHER_PWK_TLV_ID              (PROPRIETARY_TLV_BASE_ID + 0x91)
/** TLV : Group Cipher */
#define MRVL_CIPHER_GWK_TLV_ID              (PROPRIETARY_TLV_BASE_ID + 0x92)
#if 0
/** TLV : BSS Status */
#define MRVL_BSS_STATUS_TLV_ID              (PROPRIETARY_TLV_BASE_ID + 0x93)
/** TLV : Sticky TIM config */
#define MRVL_STICKY_TIM_CONFIG_TLV_ID       (PROPRIETARY_TLV_BASE_ID + 0x96)
/** TLV : Sticky TIM MAC address */
#define MRVL_STICKY_TIM_STA_MAC_ADDR_TLV_ID (PROPRIETARY_TLV_BASE_ID + 0x97)
#endif
/** TLV : 20/40 coex config */
#define MRVL_2040_BSS_COEX_CONTROL_TLV_ID  (PROPRIETARY_TLV_BASE_ID + 0x98)
#if 0
/** TLV : Max Management IE */
#define MRVL_MAX_MGMT_IE_TLV_ID             (PROPRIETARY_TLV_BASE_ID + 0xAA)
#endif
/** TLV: HT_CAPABILITY */
#define HT_CAPABILITY_TLV_ID            0x2d
/** TLV: HT_INFO */
#define HT_INFO_TLV_ID                  0x3d
/** config mask for HT_CAP */
#define HT_CAP_CONFIG_MASK              0x1072
/** default htcap value */
#define DEFAULT_HT_CAP_VALUE            0x117e
/** HT_CAP validity check */
#define HT_CAP_CHECK_MASK		0x10c
/** config mask for ampdu parameter */
#define AMPDU_CONFIG_MASK               0x1f

/** Macro to check if 11n 40 Mhz is enabled */
#define IS_11N_40MHZ_ENABLED(cap)  ((cap) & 0x002) >> 1
/** Macro to check if 11n 20MHz with short GI is enabled */
#define IS_11N_20MHZ_SHORTGI_ENABLED(cap)  ((cap) & 0x0020) >> 5
/** Macro to check if 11n Green Field is enabled */
#define IS_11N_GF_ENABLED(cap)  ((cap) & 0x0010) >> 4
/** MCS set length */
#define MCS_SET_LEN     16

/** HT Capabilities tlv */
typedef PACK_START struct _tlvbuf_htcap_t {
    /** Header */
	TLVHEADER;
    /** HTCap struct */
	HTCap_t ht_cap;
} PACK_END tlvbuf_htcap_t;

/** HT Information tlv */
typedef PACK_START struct _tlvbuf_htinfo_t {
    /** Header */
	TLVHEADER;
    /** HTCap struct */
	HTInfo_t ht_info;
} PACK_END tlvbuf_htinfo_t;

/** Max bitmap rates size */
#define MAX_BITMAP_RATES_SIZE   10

/** tx_rate_cfg structure */
typedef PACK_START struct _tx_rate_cfg_t {
   /** Action */
	int subcmd;
   /** Action */
	int action;
   /** Rate configured */
	int rate;
   /** Rate bitmap */
	t_u16 bitmap_rates[MAX_BITMAP_RATES_SIZE];
} PACK_END tx_rate_cfg_t;

/** MCS0-7 supported */
#define DEFAULT_MCS_SET_0   0xff
/** MCS32 supported */
#define DEFAULT_MCS_SET_4   0x01
/** Rate bitmap for MCS 0 */
#define UAP_RATE_BITMAP_MCS0  32
/** Rate bitmap for MCS 127 */
#define UAP_RATE_BITMAP_MCS127 159
/** Rate index for MCS 0 */
#define UAP_RATE_INDEX_MCS0   12
/** Rate index for MCS 7 */
#define UAP_RATE_INDEX_MCS7   19
/** Rate index for MCS 32 */
#define UAP_RATE_INDEX_MCS32  44
#if 0
/** Type enumeration of WMM AC_QUEUES */
typedef enum _wmm_ac {
	AC_BE,
	AC_BK,
	AC_VI,
	AC_VO,
} wmm_ac;
#endif
/** ID for VENDOR_SPECIFIC_IE */
#define VENDOR_SPECIFIC_IE_TLV_ID   0xdd

/** WMM_PS Mask */
#define WMM_PS_MASK                 0x7f
/** Enable WMM PS */
#define ENABLE_WMM_PS               128
/** Disable WMM PS */
#define DISABLE_WMM_PS              0
/** Maximum number of AC QOS queues available in the driver/firmware */
#define MAX_AC_QUEUES               4

/** wmm parameter tlv */
typedef PACK_START struct _tlvbuf_wmm_para_t {
    /** Header */
	TLVHEADER;
    /** Wmm parameter */
	WmmParameter_t wmm_para;
} PACK_END tlvbuf_wmm_para_t;
#if 0
/** Function Prototype Declaration */
int mac2raw(char *mac, t_u8 *raw);
void print_mac(t_u8 *raw);
int uap_ioctl(t_u8 *cmd, t_u16 *size, t_u16 buf_size);
void print_auth(tlvbuf_auth_mode *tlv);
void print_tlv(t_u8 *buf, t_u16 len);
void print_cipher(tlvbuf_cipher *tlv);
void print_pwk_cipher(tlvbuf_pwk_cipher *tlv);
void print_gwk_cipher(tlvbuf_gwk_cipher *tlv);
void print_rate(tlvbuf_rates *tlv);
int string2raw(char *str, unsigned char *raw);
void print_mac_filter(tlvbuf_sta_mac_addr_filter *tlv);
int ishexstring(void *hex);
inline int ISDIGIT(char *x);
unsigned int a2hex(char *s);
int fparse_for_hex(FILE *fp, t_u8 *dst);
int is_input_valid(valid_inputs cmd, int argc, char *argv[]);
int is_cipher_valid(int pairwisecipher, int groupcipher);
int is_cipher_valid_with_proto(int pairwisecipher, int groupcipher,
			       int protocol);
int get_sys_cfg_rates(t_u8 *rates);
int is_tx_rate_valid(t_u8 rate);
int is_mcbc_rate_valid(t_u8 rate);
void hexdump_data(char *prompt, void *p, int len, char delim);
unsigned char hexc2bin(char chr);
int check_sys_config(t_u8 *buf, t_u16 len);
int get_max_sta_num_supported(t_u16 *max_sta_num_supported);
int get_sys_cfg_protocol(t_u16 *proto);
int is_cipher_valid_with_11n(int pairwisecipher, int groupcipher,
			     int protocol, int enable_11n);
int get_sys_cfg_11n(HTCap_t *pHtCap);
int get_fw_info(fw_info *pfw_info);
t_u8 parse_domain_file(char *country, int band,
		       ieeetypes_subband_set_t *sub_bands);
int sg_snmp_mib(t_u16 action, t_u16 oid, t_u16 size, t_u8 *oid_buf);
int check_channel_validity_11d(int channel, int band, int set_domain);
int check_tx_pwr_validity_11d(t_u8 tx_pwr);
int prepare_host_cmd_buffer(char *fname, char *cmd_name, t_u8 *buf);
char *mlan_config_get_line(FILE *fp, t_s8 *s, t_s32 size, int *line);
#endif
#endif
#endif /* _UAP_H */
