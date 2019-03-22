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
 */

/** @file
 *  Defines common constants used with WICED
 *
 */

#ifndef INCLUDED_WWD_CONSTANTS_H_
#define INCLUDED_WWD_CONSTANTS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef MODUSTOOLBOX
#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
#define WICED_VERSION STRINGIZE(WICED_SDK_MAJOR_VER) "." STRINGIZE(WICED_SDK_MINOR_VER) "." STRINGIZE(WICED_SDK_REV_NUMBER) "." STRINGIZE(WICED_SDK_BUILD_NUMBER)
#endif

#ifndef MIN
extern int MIN (/*@sef@*/ int x, /*@sef@*/ int y); /* LINT : This tells lint that  the parameter must be side-effect free. i.e. evaluation does not change any values (since it is being evaulated more than once */
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif /* ifndef MIN */

#ifndef MAX
extern int MAX (/*@sef@*/ int x, /*@sef@*/ int y); /* LINT : This tells lint that  the parameter must be side-effect free. i.e. evaluation does not change any values (since it is being evaulated more than once */
#define MAX(x,y)  ((x) > (y) ? (x) : (y))
#endif /* ifndef MAX */

#ifndef ROUND_UP
extern int ROUND_UP (/*@sef@*/ int x, /*@sef@*/ int y); /* LINT : This tells lint that  the parameter must be side-effect free. i.e. evaluation does not change any values (since it is being evaulated more than once */
#define ROUND_UP(x,y)    ((x) % (y) ? (x) + (y)-((x)%(y)) : (x))
#endif /* ifndef ROUND_UP */

#ifndef DIV_ROUND_UP
extern int DIV_ROUND_UP (int m, /*@sef@*/ int n); /* LINT : This tells lint that  the parameter must be side-effect free. i.e. evaluation does not change any values (since it is being evaulated more than once */
#define DIV_ROUND_UP(m, n)    (((m) + (n) - 1) / (n))
#endif /* ifndef DIV_ROUND_UP */

#ifndef PLATFORM
#define PLATFORM "Unknown"
#endif /* ifndef PLATFORM */

#ifndef FreeRTOS_VERSION
#define FreeRTOS_VERSION "Unknown"
#endif /* ifndef FreeRTOS_VERSION */

#ifndef LwIP_VERSION
#define LwIP_VERSION "Unknown"
#endif /* ifndef LwIP_VERSION */

#ifndef WICED_VERSION
#define WICED_VERSION "Unknown"
#endif /* ifndef WICED_VERSION */

#define WIFI_IE_OUI_LENGTH    (3)

/** @cond !ADDTHIS*/
#define WEP_ENABLED            0x0001
#define TKIP_ENABLED           0x0002
#define AES_ENABLED            0x0004
#define SHARED_ENABLED     0x00008000
#define WPA_SECURITY       0x00200000
#define WPA2_SECURITY      0x00400000
#define WPA3_SECURITY      0x01000000

#define ENTERPRISE_ENABLED 0x02000000
#define WPS_ENABLED        0x10000000
#define IBSS_ENABLED       0x20000000
#define FBT_ENABLED        0x40000000
/** @endcond */

#define DSSS_PARAMETER_SET_LENGTH (1)

#define HT_CAPABILITIES_IE_LENGTH (26)
#define HT_OPERATION_IE_LENGTH    (22)

#define WWD_VHT_FEATURES_PROPRATES_ENAB   (2)

#define WWD_CNTRY_BUF_SZ           (4)

#define RRM_CAPABILITIES_LEN (5)
#define WL_RRM_RPT_VER      0
#define WL_RRM_RPT_MAX_PAYLOAD  64
#define WL_RRM_RPT_MIN_PAYLOAD  7
#define WL_RRM_RPT_FALG_ERR 0
#define WL_RRM_RPT_FALG_OK  1

/* TLV defines */
#define TLV_TAG_OFF        0    /* tag offset */
#define TLV_LEN_OFF        1    /* length offset */
#define TLV_HDR_LEN        2    /* header length */
#define TLV_BODY_OFF        2    /* body offset */

#define DOT11_NEIGHBOR_REP_IE_FIXED_LEN 13
#define DOT11_MNG_NEIGHBOR_REP_ID       52  /* 11k & 11v Neighbor report id */

#define    WICED_MAX_CHANNEL_NUM        224 /* max # supported channels. */

/* Bitmap definitions for cap ie */
#define DOT11_RRM_CAP_LINK      0
#define DOT11_RRM_CAP_NEIGHBOR_REPORT   1
#define DOT11_RRM_CAP_PARALLEL      2
#define DOT11_RRM_CAP_REPEATED      3
#define DOT11_RRM_CAP_BCN_PASSIVE   4
#define DOT11_RRM_CAP_BCN_ACTIVE    5
#define DOT11_RRM_CAP_BCN_TABLE     6
#define DOT11_RRM_CAP_BCN_REP_COND  7
#define DOT11_RRM_CAP_FM        8
#define DOT11_RRM_CAP_CLM       9
#define DOT11_RRM_CAP_NHM       10
#define DOT11_RRM_CAP_SM        11
#define DOT11_RRM_CAP_LCIM      12
#define DOT11_RRM_CAP_LCIA      13
#define DOT11_RRM_CAP_TSCM      14
#define DOT11_RRM_CAP_TTSCM     15
#define DOT11_RRM_CAP_AP_CHANREP    16
#define DOT11_RRM_CAP_RMMIB     17
/* bit18-bit26, not used for RRM_IOVAR */
#define DOT11_RRM_CAP_MPTI      27
#define DOT11_RRM_CAP_NBRTSFO       28
#define DOT11_RRM_CAP_RCPI      29
#define DOT11_RRM_CAP_RSNI      30
#define DOT11_RRM_CAP_BSSAAD        31
#define DOT11_RRM_CAP_BSSAAC        32
#define DOT11_RRM_CAP_AI        33
#define DOT11_RRM_CAP_LAST 34

#define DOT11_OUI_LEN 3 /** Length in bytes of 802.11 OUI*/

/* WNM/NPS subfeatures mask */
#define WL_WNM_BSSTRANS         0x00000001
#define WL_WNM_PROXYARP         0x00000002
#define WL_WNM_MAXIDLE          0x00000004
#define WL_WNM_TIMBC            0x00000008
#define WL_WNM_TFS              0x00000010
#define WL_WNM_SLEEP            0x00000020
#define WL_WNM_DMS              0x00000040
#define WL_WNM_FMS              0x00000080
#define WL_WNM_NOTIF            0x00000100
#define WL_WNM_WBTEXT           0x00000200
#ifndef  WL_WNM_MAX
# define WL_WNM_MAX              0x00000400
#endif

/* DHCP Lease renewal Offload Sub-command ID's */

#define WWD_IPV4_ADDR_LEN            4      /* IPV4 Address Length   */
#define WWD_DLTRO_SUBCMD_CONNECT     0      /* DLTRO connection info */
#define WWD_DLTRO_SUBCMD_PARAM       1      /* DLTRO parameter info  */
#define WWD_DLTRO_SUBCMD_MAX_DLTRO   2      /* Max DLTRO supported   */

#define ETHER_TYPE_IP              0x0800   /* IP                    */
#define IPV4_ADDR_LEN                4      /* IPV4 address length   */

/* IPV4 field decodes */
#define IPV4_VER_MASK              0xf0     /* IPV4 version mask */
#define IPV4_VER_SHIFT               4      /* IPV4 version shift */
#define IP_VER_4                     4      /* version number for IPV4 */
#define IPV4_MIN_HEADER_LEN          20     /* Minimum size for an IP header (no options) */
#define IP_PROT_UDP                 0x11    /* UDP protocol type */
#define IPV4_VER_HL_OFFSET           0      /* version and ihl byte offset */
#define IPV4_HLEN_MASK               0x0f    /* IPV4 header length mask */
#ifndef IPV4_HLEN
#define IPV4_HLEN(ipv4_body)        (4 * (((uint8_t *)(ipv4_body))[IPV4_VER_HL_OFFSET] & IPV4_HLEN_MASK))
#endif
#define UDP_HDR_LEN                  8      /* UDP header length */
#define UDP_PORT_LEN                 2      /* UDP port length */



#ifndef _BSS_TRANS_POLICY_ROAM_ALWAYS
typedef enum
{
#define _BSS_TRANS_POLICY_ROAM_ALWAYS
    WL_BSSTRANS_POLICY_ROAM_ALWAYS = 0,     /* Roam (or disassociate) in all cases */
    WL_BSSTRANS_POLICY_ROAM_IF_MODE = 1,    /* Roam only if requested by Request Mode field */
    WL_BSSTRANS_POLICY_ROAM_IF_PREF = 2,    /* Roam only if Preferred BSS provided */
    WL_BSSTRANS_POLICY_WAIT = 3,            /* Wait for deauth and send Accepted status */
    WL_BSSTRANS_POLICY_PRODUCT = 4,         /* Policy for real product use cases (non-pf) */
} wwd_bsstrans_policy_t;
#endif /* _BSS_TRANS_POLICY_ROAM_ALWAYS */

/* MAX NAN Slots outside Discovery Windows that are contiguous in time domain
 * and are associated with the same channel or channel set
 */
#define NAN_MAX_TIMESLOT 32

#define NAN_ABITMAP_SHIFT      4 /* 16 ms for each bit */

/* Length of the Service Discovery Hash */
#define WL_NAN_SVC_HASH_LEN 6

#define WL_NAN_SVC_HASH "NAN123"

#define NAN_SUBSCRIBE_PERIOD 1
#define NAN_PUBLISH_PERIOD 1

/** Special values for time to live (ttl) parameter */
#define WL_NAN_TTL_UNTIL_CANCEL                 0xFFFFFFFF

#define WL_NAN_PUB_BOTH          0x3000
#define WL_NAN_RANGE_LIMITED     0x0040

#define BCM_XTLV_OPTION_ALIGN32    0x0001 /* 32bit alignment of type.len.data */
#define BCM_IOV_BATCH_MASK 0x8000

enum wl_nan_sub_cmd_xtlv_id {
         /* nan cfg sub-commands */
        WL_NAN_CMD_CFG_ENABLE = 0x0101,
        WL_NAN_CMD_CFG_STATE = 0x0102,
        WL_NAN_CMD_CFG_HOP_CNT = 0x0103,
        WL_NAN_CMD_CFG_HOP_LIMIT = 0x0104,
        WL_NAN_CMD_CFG_WARMUP_TIME = 0x0105,
        WL_NAN_CMD_CFG_RSSI_THRESHOLD = 0x0106,
        WL_NAN_CMD_CFG_STATUS = 0x0107,
        WL_NAN_CMD_CFG_OUI = 0x0108,
        WL_NAN_CMD_CFG_COUNT = 0x0109,
        WL_NAN_CMD_CFG_CLEARCOUNT = 0x010a,
        WL_NAN_CMD_CFG_CHANNEL = 0x010b,
        WL_NAN_CMD_CFG_BAND = 0x010c,
        WL_NAN_CMD_CFG_CID = 0x010d,
        WL_NAN_CMD_CFG_IF_ADDR = 0x010e,
        WL_NAN_CMD_CFG_BCN_INTERVAL = 0x010f,
        WL_NAN_CMD_CFG_SDF_TXTIME = 0x0110,
        WL_NAN_CMD_CFG_STOP_BCN_TX = 0x0111,
        WL_NAN_CMD_CFG_SID_BEACON = 0x0112,
        WL_NAN_CMD_CFG_DW_LEN = 0x0113,
        WL_NAN_CMD_CFG_MAX = WL_NAN_CMD_CFG_DW_LEN, /* Add new commands before and update */

        /* nan election sub-commands */
        WL_NAN_CMD_ELECTION_HOST_ENABLE = 0x0201,
        WL_NAN_CMD_ELECTION_METRICS_CONFIG = 0x0202,
        WL_NAN_CMD_ELECTION_METRICS_STATE = 0x0203,
        WL_NAN_CMD_ELECTION_JOIN = 0x0204,
        WL_NAN_CMD_ELECTION_LEAVE = 0x0205,
        WL_NAN_CMD_ELECTION_MERGE = 0x0206,
        WL_NAN_CMD_ELECTION_STOP = 0x0207,      /* stop nan for a given cluster ID  */
        WL_NAN_CMD_SCAN          = 0x0208,      /* specific to WICED */
        WL_NAN_CMD_ELECTION_MAX = WL_NAN_CMD_SCAN, /* New commands go before and update */

        /* nan SD sub-commands */
        WL_NAN_CMD_SD_PARAMS = 0x0301,
        WL_NAN_CMD_SD_PUBLISH = 0x0302,
        WL_NAN_CMD_SD_PUBLISH_LIST = 0x0303,
        WL_NAN_CMD_SD_CANCEL_PUBLISH = 0x0304,
        WL_NAN_CMD_SD_SUBSCRIBE = 0x0305,
        WL_NAN_CMD_SD_SUBSCRIBE_LIST = 0x0306,
        WL_NAN_CMD_SD_CANCEL_SUBSCRIBE = 0x0307,
        WL_NAN_CMD_SD_VND_INFO = 0x0308,
        WL_NAN_CMD_SD_STATS = 0x0309,
        WL_NAN_CMD_SD_TRANSMIT = 0x030A,
        WL_NAN_CMD_SD_FUP_TRANSMIT = 0x030B,
        WL_NAN_CMD_SD_CONNECTION = 0x030C,
        WL_NAN_CMD_SD_SHOW = 0x030D,
        WL_NAN_CMD_SD_MAX = WL_NAN_CMD_SD_SHOW, /* New commands go before and update */

        /* nan time sync sub-commands */
        WL_NAN_CMD_SYNC_TSRESERVE = 0x0401,
        WL_NAN_CMD_SYNC_TSSCHEDULE = 0x0402,
        WL_NAN_CMD_SYNC_TSRELEASE = 0x0403,
        WL_NAN_CMD_SYNC_MAX = WL_NAN_CMD_SYNC_TSRELEASE, /* New ones before and update */

        /* nan2 commands */
        WL_NAN_CMD_DATA_CONFIG = 0x501,
        WL_NAN_CMD_DATA_AUTOCONN = 0x502,
        WL_NAN_CMD_DATA_NDP_CREATE = 0x503,
        WL_NAN_CMD_DATA_DATAREQ = 0x504,
        WL_NAN_CMD_DATA_DATARESP = 0x505,
        WL_NAN_CMD_DATA_DATAEND = 0x506,
        WL_NAN_CMD_DATA_SCHEDUPD = 0x507,
        WL_NAN_CMD_DATA_CONNECT = 0x508,
        WL_NAN_CMD_DATA_CAP = 0x509,
        WL_NAN_CMD_DATA_STATUS = 0x50A,
        WL_NAN_CMD_DATA_STATS = 0x50B,
        WL_NAN_CMD_DATA_NDP_DEL = 0x50C,
        WL_NAN_CMD_DATA_NDP_SHOW = 0x50D,
        WL_NAN_CMD_DATA_PATH_MAX = WL_NAN_CMD_DATA_NDP_SHOW, /* New ones before and update */

        /*  nan debug sub-commands  */
        WL_NAN_CMD_DBG_SCAN_PARAMS = 0x0f01,
        WL_NAN_CMD_DBG_SCAN = 0x0f02,
        WL_NAN_CMD_DBG_SCAN_RESULTS = 0x0f03,
        WL_NAN_CMD_DBG_EVENT_MASK = 0x0f04,
        WL_NAN_CMD_DBG_EVENT_CHECK = 0x0f05,
        WL_NAN_CMD_DBG_DUMP = 0x0f06,
        WL_NAN_CMD_DBG_CLEAR = 0x0f07,
        WL_NAN_CMD_DBG_RSSI = 0x0f08,
        WL_NAN_CMD_DBG_DEBUG = 0x0f09,
        WL_NAN_CMD_DBG_TEST1 = 0x0f0a,
        WL_NAN_CMD_DBG_TEST2 = 0x0f0b,
        WL_NAN_CMD_DBG_TEST3 = 0x0f0c,
        WL_NAN_CMD_DBG_DISC_RESULTS = 0x0f0d,
        WL_NAN_CMD_DBG_STATS = 0x0f0e,
        WL_NAN_CMD_DBG_MAX = WL_NAN_CMD_DBG_STATS /* New ones before and update */
};

/*
 * WL_NAN_CMD_CFG_STATE
 */
enum wl_nan_role {
        WL_NAN_ROLE_AUTO = 0,
        WL_NAN_ROLE_NON_MASTER_NON_SYNC = 1,
        WL_NAN_ROLE_NON_MASTER_SYNC = 2,
        WL_NAN_ROLE_MASTER = 3,
        WL_NAN_ROLE_ANCHOR_MASTER = 4
};


/** Definitions of different NAN Bands */
/* do not change the order */
typedef enum
{
       NAN_BAND_B = 0,
       NAN_BAND_A,
       NAN_BAND_AUTO,
       NAN_BAND_INVALID = 0xFF
} wwd_nan_band_t;

/* This XTLV definitions for iovar "wl counters" have been taken for 43012 chip from
 * IGUANA_TWIG_13_10_271 branch under ./include/wlioctl.h
 */

typedef enum  {
        WL_CNT_XTLV_WLC = 0x100,                /**< WLC layer counters */
        WL_CNT_XTLV_WLC_RINIT_RSN = 0x101,      /**< WLC layer reinitreason extension */
        WL_CNT_XTLV_CNTV_LE10_UCODE = 0x200,    /**< wl counter ver < 11 UCODE MACSTAT */
        WL_CNT_XTLV_LT40_UCODE_V1 = 0x300,      /**< corerev < 40 UCODE MACSTAT */
        WL_CNT_XTLV_GE40_UCODE_V1 = 0x400,      /**< corerev >= 40 UCODE MACSTAT */
        WL_CNT_XTLV_GE64_UCODEX_V1 = 0x800      /* corerev >= 64 UCODEX MACSTAT */
}wwd_cnt_xtlv_id;

/** Enumeration of WICED interfaces. \n
 * @note The config interface is a virtual interface that shares the softAP interface
 */
typedef enum
{
    WWD_STA_INTERFACE          = 0,         /**< STA or Client Interface                                                 */
    WWD_AP_INTERFACE           = 1,         /**< softAP Interface                                                        */
    WWD_P2P_INTERFACE          = 2,         /**< P2P Interface                                                           */
    WWD_ETHERNET_INTERFACE     = 3,         /**< Ethernet Interface                                                      */

    WWD_INTERFACE_MAX,                      /** DO NOT USE - MUST BE LAST INTERFACE VALUE - used for counting interfaces */
    WWD_INTERFACE_FORCE_32_BIT = 0x7fffffff /**< Exists only to force wwd_interface_t type to 32 bits                    */
} wwd_interface_t;

/**
 * Enumeration of Wi-Fi security modes
 */
typedef enum
{
    WICED_SECURITY_OPEN           = 0,                                                                   /**< Open security                                         */
    WICED_SECURITY_WEP_PSK        = WEP_ENABLED,                                                         /**< WEP PSK Security with open authentication             */
    WICED_SECURITY_WEP_SHARED     = ( WEP_ENABLED   | SHARED_ENABLED ),                                  /**< WEP PSK Security with shared authentication           */
    WICED_SECURITY_WPA_TKIP_PSK   = ( WPA_SECURITY  | TKIP_ENABLED ),                                    /**< WPA PSK Security with TKIP                            */
    WICED_SECURITY_WPA_AES_PSK    = ( WPA_SECURITY  | AES_ENABLED ),                                     /**< WPA PSK Security with AES                             */
    WICED_SECURITY_WPA_MIXED_PSK  = ( WPA_SECURITY  | AES_ENABLED | TKIP_ENABLED ),                      /**< WPA PSK Security with AES & TKIP                      */
    WICED_SECURITY_WPA2_AES_PSK   = ( WPA2_SECURITY | AES_ENABLED ),                                     /**< WPA2 PSK Security with AES                            */
    WICED_SECURITY_WPA2_TKIP_PSK  = ( WPA2_SECURITY | TKIP_ENABLED ),                                    /**< WPA2 PSK Security with TKIP                           */
    WICED_SECURITY_WPA2_MIXED_PSK = ( WPA2_SECURITY | AES_ENABLED | TKIP_ENABLED ),                      /**< WPA2 PSK Security with AES & TKIP                     */
    WICED_SECURITY_WPA2_FBT_PSK   = ( WPA2_SECURITY | AES_ENABLED | FBT_ENABLED),                        /**< WPA2 FBT PSK Security with AES & TKIP */
    WICED_SECURITY_WPA3_SAE       = ( WPA3_SECURITY | AES_ENABLED ),                                     /**< WPA3 Security with AES */
    WICED_SECURITY_WPA3_WPA2_PSK  = ( WPA3_SECURITY | WPA2_SECURITY | AES_ENABLED ),                     /**< WPA3 WPA2 PSK Security with AES */

    WICED_SECURITY_WPA_TKIP_ENT   = ( ENTERPRISE_ENABLED | WPA_SECURITY  | TKIP_ENABLED ),               /**< WPA Enterprise Security with TKIP                     */
    WICED_SECURITY_WPA_AES_ENT    = ( ENTERPRISE_ENABLED | WPA_SECURITY  | AES_ENABLED ),                /**< WPA Enterprise Security with AES                      */
    WICED_SECURITY_WPA_MIXED_ENT  = ( ENTERPRISE_ENABLED | WPA_SECURITY  | AES_ENABLED | TKIP_ENABLED ), /**< WPA Enterprise Security with AES & TKIP               */
    WICED_SECURITY_WPA2_TKIP_ENT  = ( ENTERPRISE_ENABLED | WPA2_SECURITY | TKIP_ENABLED ),               /**< WPA2 Enterprise Security with TKIP                    */
    WICED_SECURITY_WPA2_AES_ENT   = ( ENTERPRISE_ENABLED | WPA2_SECURITY | AES_ENABLED ),                /**< WPA2 Enterprise Security with AES                     */
    WICED_SECURITY_WPA2_MIXED_ENT = ( ENTERPRISE_ENABLED | WPA2_SECURITY | AES_ENABLED | TKIP_ENABLED ), /**< WPA2 Enterprise Security with AES & TKIP              */
    WICED_SECURITY_WPA2_FBT_ENT =   ( ENTERPRISE_ENABLED | WPA2_SECURITY | AES_ENABLED | FBT_ENABLED),

    WICED_SECURITY_IBSS_OPEN      = ( IBSS_ENABLED ),                                  /**< Open security on IBSS ad-hoc network         */
    WICED_SECURITY_WPS_OPEN       = ( WPS_ENABLED ),                                                     /**< WPS with open security                                */
    WICED_SECURITY_WPS_SECURE     = ( WPS_ENABLED | AES_ENABLED),                                        /**< WPS with AES security                                 */

    WICED_SECURITY_UNKNOWN        = -1,                                                                  /**< May be returned by scan function if security is unknown. Do not pass this to the join function! */

    WICED_SECURITY_FORCE_32_BIT   = 0x7fffffff                                                           /**< Exists only to force wiced_security_t type to 32 bits */
} wiced_security_t;

/**
 * Enumeration of methods of scanning
 */
typedef enum
{
    WICED_SCAN_TYPE_ACTIVE              = 0x00,  /**< Actively scan a network by sending 802.11 probe(s)                              */
    WICED_SCAN_TYPE_PASSIVE             = 0x01,  /**< Passively scan a network by listening for beacons from APs                      */
    WICED_SCAN_TYPE_PNO                 = 0x02,  /**< Use preferred network offload to detect an AP                                   */
    WICED_SCAN_TYPE_PROHIBITED_CHANNELS = 0x04,  /**< Permit (passively) scanning a channel that isn't valid for the current country  */
    WICED_SCAN_TYPE_NO_BSSID_FILTER     = 0x08   /**< Return a scan record for each beacon or probe response RX'ed                    */
} wiced_scan_type_t;

/**
 ** Enumeration of scan result flags
 */
typedef enum
{
    WICED_SCAN_RESULT_FLAG_RSSI_OFF_CHANNEL   = 0x01,  /**< RSSI came from an off channel DSSS (1 or 1 Mb) Rx */
    WICED_SCAN_RESULT_FLAG_BEACON             = 0x02   /**< Beacon (vs probe response)                        */
} wiced_scan_result_flag_t;

/**
 * Enumeration of network types
 */
typedef enum
{
    WICED_BSS_TYPE_INFRASTRUCTURE = 0, /**< Denotes infrastructure network                  */
    WICED_BSS_TYPE_ADHOC          = 1, /**< Denotes an 802.11 ad-hoc IBSS network           */
    WICED_BSS_TYPE_ANY            = 2, /**< Denotes either infrastructure or ad-hoc network */
    WICED_BSS_TYPE_MESH           = 3, /**< Denotes 802.11 mesh network */

    WICED_BSS_TYPE_UNKNOWN        = -1 /**< May be returned by scan function if BSS type is unknown. Do not pass this to the Join function */
} wiced_bss_type_t;

/**
 * Enumeration of 802.11 radio bands
 */
typedef enum
{
    WICED_802_11_BAND_5GHZ   = 0, /**< Denotes 5GHz radio band   */
    WICED_802_11_BAND_2_4GHZ = 1  /**< Denotes 2.4GHz radio band */
} wiced_802_11_band_t;

/**
 * Enumeration of antenna selection options
 */
typedef enum
{
    WICED_ANTENNA_1    = 0,  /**< Denotes antenna 1 */
    WICED_ANTENNA_2    = 1,  /**< Denotes antenna 2 */
    WICED_ANTENNA_AUTO = 3   /**< Denotes auto diversity, the best antenna is automatically selected */
} wiced_antenna_t;

/**
 * Enumeration of applicable packet mask bits for custom Information Elements (IEs)
 */
typedef enum
{
    VENDOR_IE_BEACON         = 0x1,  /**< Denotes beacon packet                  */
    VENDOR_IE_PROBE_RESPONSE = 0x2,  /**< Denotes probe response packet          */
    VENDOR_IE_ASSOC_RESPONSE = 0x4,  /**< Denotes association response packet    */
    VENDOR_IE_AUTH_RESPONSE  = 0x8,  /**< Denotes authentication response packet */
    VENDOR_IE_PROBE_REQUEST  = 0x10, /**< Denotes probe request packet           */
    VENDOR_IE_ASSOC_REQUEST  = 0x20, /**< Denotes association request packet     */
    VENDOR_IE_CUSTOM         = 0x100 /**< Denotes a custom IE identifier         */
} wiced_ie_packet_flag_t;

/**
 * Enumeration of custom IE management actions
 */
typedef enum
{
    WICED_ADD_CUSTOM_IE,     /**< Add a custom IE    */
    WICED_REMOVE_CUSTOM_IE   /**< Remove a custom IE */
} wiced_custom_ie_action_t;

/**
 * Enumeration of 802.11 QoS, i.e. WMM, traffic classes
 */
typedef enum
{
    WMM_AC_BE =         0,      /**< Best Effort */
    WMM_AC_BK =         1,      /**< Background  */
    WMM_AC_VI =         2,      /**< Video       */
    WMM_AC_VO =         3,      /**< Voice       */
} wiced_qos_access_category_t;

/**
 * Enumeration of IP header Type of Service (TOS) values, which map to 802.11 QoS traffic classes
 */
typedef enum
{
    TOS_VO7 = 7, /**< 0xE0, 111 0  0000 (7)  AC_VO tos/dscp values */
    TOS_VO  = 6, /**< 0xD0, 110 0  0000 (6)  AC_VO                 */
    TOS_VI  = 5, /**< 0xA0, 101 0  0000 (5)  AC_VI                 */
    TOS_VI4 = 4, /**< 0x80, 100 0  0000 (4)  AC_VI                 */
    TOS_BE  = 0, /**< 0x00, 000 0  0000 (0)  AC_BE                 */
    TOS_EE  = 3, /**< 0x60, 011 0  0000 (3)  AC_BE                 */
    TOS_BK  = 1, /**< 0x20, 001 0  0000 (1)  AC_BK                 */
    TOS_LE  = 2, /**< 0x40, 010 0  0000 (2)  AC_BK                 */
} wiced_ip_header_tos_t;

/**
 * Enumeration of listen interval time unit types
 */
typedef enum
{
    WICED_LISTEN_INTERVAL_TIME_UNIT_BEACON, /**< Time units specified in beacon periods */
    WICED_LISTEN_INTERVAL_TIME_UNIT_DTIM    /**< Time units specified in DTIM periods   */
} wiced_listen_interval_time_unit_t;

/**
 * Enumeration of packet filter modes
 */
typedef enum
{
    WICED_PACKET_FILTER_MODE_FORWARD = 1, /**< Packet filter engine forwards matching packets, discards non-matching packets */
    WICED_PACKET_FILTER_MODE_DISCARD = 0, /**< Packet filter engine discards matching packets, forwards non-matching packets */
} wiced_packet_filter_mode_t;

/**
 * Enumeration of packet filter rules
 */
typedef enum
{
    WICED_PACKET_FILTER_RULE_POSITIVE_MATCHING  = 0, /**< Specifies that a filter should match a given pattern     */
    WICED_PACKET_FILTER_RULE_NEGATIVE_MATCHING  = 1  /**< Specifies that a filter should NOT match a given pattern */
} wiced_packet_filter_rule_t;

typedef enum
{
    WICED_SCAN_INCOMPLETE,
    WICED_SCAN_COMPLETED_SUCCESSFULLY,
    WICED_SCAN_ABORTED,
} wiced_scan_status_t;

/** List of HT modes supported */
typedef enum
{
    WICED_HT_MODE_HT20      = 0,        /**< HT20 mode is set on the band */
    WICED_HT_MODE_HT40      = 1,        /**< HT40 mode is set on the band */
    WICED_HT_MODE_HT_MIX    = 2         /**< HT20 mode is set for 2.4 band and HT40 is set for 5 GHz band */
} wiced_ht_mode_t;

typedef enum
{
    WICED_11N_SUPPORT_DISABLED = 0,
    WICED_11N_SUPPORT_ENABLED  = 1,
} wiced_11n_support_t;


/* 802.11 Information Element Identification Numbers (as per section 8.4.2.1 of 802.11-2012) */
typedef enum
{
    DOT11_IE_ID_SSID                                 = 0,
    DOT11_IE_ID_SUPPORTED_RATES                      = 1,
    DOT11_IE_ID_FH_PARAMETER_SET                     = 2,
    DOT11_IE_ID_DSSS_PARAMETER_SET                   = 3,
    DOT11_IE_ID_CF_PARAMETER_SET                     = 4,
    DOT11_IE_ID_TIM                                  = 5,
    DOT11_IE_ID_IBSS_PARAMETER_SET                   = 6,
    DOT11_IE_ID_COUNTRY                              = 7,
    DOT11_IE_ID_HOPPING_PATTERN_PARAMETERS           = 8,
    DOT11_IE_ID_HOPPING_PATTERN_TABLE                = 9,
    DOT11_IE_ID_REQUEST                              = 10,
    DOT11_IE_ID_BSS_LOAD                             = 11,
    DOT11_IE_ID_EDCA_PARAMETER_SET                   = 12,
    DOT11_IE_ID_TSPEC                                = 13,
    DOT11_IE_ID_TCLAS                                = 14,
    DOT11_IE_ID_SCHEDULE                             = 15,
    DOT11_IE_ID_CHALLENGE_TEXT                       = 16,
    /* 17-31 Reserved */
    DOT11_IE_ID_POWER_CONSTRAINT                     = 32,
    DOT11_IE_ID_POWER_CAPABILITY                     = 33,
    DOT11_IE_ID_TPC_REQUEST                          = 34,
    DOT11_IE_ID_TPC_REPORT                           = 35,
    DOT11_IE_ID_SUPPORTED_CHANNELS                   = 36,
    DOT11_IE_ID_CHANNEL_SWITCH_ANNOUNCEMENT          = 37,
    DOT11_IE_ID_MEASUREMENT_REQUEST                  = 38,
    DOT11_IE_ID_MEASUREMENT_REPORT                   = 39,
    DOT11_IE_ID_QUIET                                = 40,
    DOT11_IE_ID_IBSS_DFS                             = 41,
    DOT11_IE_ID_ERP                                  = 42,
    DOT11_IE_ID_TS_DELAY                             = 43,
    DOT11_IE_ID_TCLAS_PROCESSING                     = 44,
    DOT11_IE_ID_HT_CAPABILITIES                      = 45,
    DOT11_IE_ID_QOS_CAPABILITY                       = 46,
    /* 47 Reserved */
    DOT11_IE_ID_RSN                                  = 48,
    /* 49 Reserved */
    DOT11_IE_ID_EXTENDED_SUPPORTED_RATES             = 50,
    DOT11_IE_ID_AP_CHANNEL_REPORT                    = 51,
    DOT11_IE_ID_NEIGHBOR_REPORT                      = 52,
    DOT11_IE_ID_RCPI                                 = 53,
    DOT11_IE_ID_MOBILITY_DOMAIN                      = 54,
    DOT11_IE_ID_FAST_BSS_TRANSITION                  = 55,
    DOT11_IE_ID_TIMEOUT_INTERVAL                     = 56,
    DOT11_IE_ID_RIC_DATA                             = 57,
    DOT11_IE_ID_DSE_REGISTERED_LOCATION              = 58,
    DOT11_IE_ID_SUPPORTED_OPERATING_CLASSES          = 59,
    DOT11_IE_ID_EXTENDED_CHANNEL_SWITCH_ANNOUNCEMENT = 60,
    DOT11_IE_ID_HT_OPERATION                         = 61,
    DOT11_IE_ID_SECONDARY_CHANNEL_OFFSET             = 62,
    DOT11_IE_ID_BSS_AVERAGE_ACCESS_DELAY             = 63,
    DOT11_IE_ID_ANTENNA                              = 64,
    DOT11_IE_ID_RSNI                                 = 65,
    DOT11_IE_ID_MEASUREMENT_PILOT_TRANSMISSION       = 66,
    DOT11_IE_ID_BSS_AVAILABLE_ADMISSION_CAPACITY     = 67,
    DOT11_IE_ID_BSS_AC_ACCESS_DELAY                  = 68,
    DOT11_IE_ID_TIME_ADVERTISEMENT                   = 69,
    DOT11_IE_ID_RM_ENABLED_CAPABILITIES              = 70,
    DOT11_IE_ID_MULTIPLE_BSSID                       = 71,
    DOT11_IE_ID_20_40_BSS_COEXISTENCE                = 72,
    DOT11_IE_ID_20_40_BSS_INTOLERANT_CHANNEL_REPORT  = 73,
    DOT11_IE_ID_OVERLAPPING_BSS_SCAN_PARAMETERS      = 74,
    DOT11_IE_ID_RIC_DESCRIPTOR                       = 75,
    DOT11_IE_ID_MANAGEMENT_MIC                       = 76,
    DOT11_IE_ID_EVENT_REQUEST                        = 78,
    DOT11_IE_ID_EVENT_REPORT                         = 79,
    DOT11_IE_ID_DIAGNOSTIC_REQUEST                   = 80,
    DOT11_IE_ID_DIAGNOSTIC_REPORT                    = 81,
    DOT11_IE_ID_LOCATION_PARAMETERS                  = 82,
    DOT11_IE_ID_NONTRANSMITTED_BSSID_CAPABILITY      = 83,
    DOT11_IE_ID_SSID_LIST                            = 84,
    DOT11_IE_ID_MULTIPLE_BSSID_INDEX                 = 85,
    DOT11_IE_ID_FMS_DESCRIPTOR                       = 86,
    DOT11_IE_ID_FMS_REQUEST                          = 87,
    DOT11_IE_ID_FMS_RESPONSE                         = 88,
    DOT11_IE_ID_QOS_TRAFFIC_CAPABILITY               = 89,
    DOT11_IE_ID_BSS_MAX_IDLE_PERIOD                  = 90,
    DOT11_IE_ID_TFS_REQUEST                          = 91,
    DOT11_IE_ID_TFS_RESPONSE                         = 92,
    DOT11_IE_ID_WNM_SLEEP_MODE                       = 93,
    DOT11_IE_ID_TIM_BROADCAST_REQUEST                = 94,
    DOT11_IE_ID_TIM_BROADCAST_RESPONSE               = 95,
    DOT11_IE_ID_COLLOCATED_INTERFERENCE_REPORT       = 96,
    DOT11_IE_ID_CHANNEL_USAGE                        = 97,
    DOT11_IE_ID_TIME_ZONE                            = 98,
    DOT11_IE_ID_DMS_REQUEST                          = 99,
    DOT11_IE_ID_DMS_RESPONSE                         = 100,
    DOT11_IE_ID_LINK_IDENTIFIER                      = 101,
    DOT11_IE_ID_WAKEUP_SCHEDULE                      = 102,
    /* 103 Reserved */
    DOT11_IE_ID_CHANNEL_SWITCH_TIMING                = 104,
    DOT11_IE_ID_PTI_CONTROL                          = 105,
    DOT11_IE_ID_TPU_BUFFER_STATUS                    = 106,
    DOT11_IE_ID_INTERWORKING                         = 107,
    DOT11_IE_ID_ADVERTISMENT_PROTOCOL                = 108,
    DOT11_IE_ID_EXPEDITED_BANDWIDTH_REQUEST          = 109,
    DOT11_IE_ID_QOS_MAP_SET                          = 110,
    DOT11_IE_ID_ROAMING_CONSORTIUM                   = 111,
    DOT11_IE_ID_EMERGENCY_ALERT_IDENTIFIER           = 112,
    DOT11_IE_ID_MESH_CONFIGURATION                   = 113,
    DOT11_IE_ID_MESH_ID                              = 114,
    DOT11_IE_ID_MESH_LINK_METRIC_REPORT              = 115,
    DOT11_IE_ID_CONGESTION_NOTIFICATION              = 116,
    DOT11_IE_ID_MESH_PEERING_MANAGEMENT              = 117,
    DOT11_IE_ID_MESH_CHANNEL_SWITCH_PARAMETERS       = 118,
    DOT11_IE_ID_MESH_AWAKE_WINDOW                    = 119,
    DOT11_IE_ID_BEACON_TIMING                        = 120,
    DOT11_IE_ID_MCCAOP_SETUP_REQUEST                 = 121,
    DOT11_IE_ID_MCCAOP_SETUP_REPLY                   = 122,
    DOT11_IE_ID_MCCAOP_ADVERTISMENT                  = 123,
    DOT11_IE_ID_MCCAOP_TEARDOWN                      = 124,
    DOT11_IE_ID_GANN                                 = 125,
    DOT11_IE_ID_RANN                                 = 126,
    DOT11_IE_ID_EXTENDED_CAPABILITIES                = 127,
    /* 128-129 Reserved */
    DOT11_IE_ID_PREQ                                 = 130,
    DOT11_IE_ID_PREP                                 = 131,
    DOT11_IE_ID_PERR                                 = 132,
    /* 133-136 Reserved */
    DOT11_IE_ID_PXU                                  = 137,
    DOT11_IE_ID_PXUC                                 = 138,
    DOT11_IE_ID_AUTHENTICATED_MESH_PEERING_EXCHANGE  = 139,
    DOT11_IE_ID_MIC                                  = 140,
    DOT11_IE_ID_DESTINATION_URI                      = 141,
    DOT11_IE_ID_U_APSD_COEXISTENCE                   = 142,
    /* 143-173 Reserved */
    DOT11_IE_ID_MCCAOP_ADVERTISMENT_OVERVIEW         = 174,
    /* 175-220 Reserved */
    DOT11_IE_ID_VENDOR_SPECIFIC                      = 221,
    /* 222-255 Reserved */
} dot11_ie_id_t;

/* Protected Management Frame Capability */
enum {
    WL_MFP_NONE = 0,
    WL_MFP_CAPABLE,
    WL_MFP_REQUIRED
};

#ifndef RESULT_ENUM
#define RESULT_ENUM( prefix, name, value )  prefix ## name = (value)
#endif /* ifndef RESULT_ENUM */


/* These Enum result values are for WWD errors
 * Values: 1000 - 1999
 */

#define WWD_RESULT_LIST( prefix )  \
    RESULT_ENUM( prefix, SUCCESS,                         0 ),   /**< Success */                           \
    RESULT_ENUM( prefix, PENDING,                         1 ),   /**< Pending */                           \
    RESULT_ENUM( prefix, TIMEOUT,                         2 ),   /**< Timeout */                           \
    RESULT_ENUM( prefix, BADARG,                          5 ),   /**< Bad Arguments */                     \
    RESULT_ENUM( prefix, UNFINISHED,                     10 ),   /**< Operation not finished yet (maybe aborted) */ \
    RESULT_ENUM( prefix, PARTIAL_RESULTS,              1003 ),   /**< Partial results */                   \
    RESULT_ENUM( prefix, INVALID_KEY,                  1004 ),   /**< Invalid key */                       \
    RESULT_ENUM( prefix, DOES_NOT_EXIST,               1005 ),   /**< Does not exist */                    \
    RESULT_ENUM( prefix, NOT_AUTHENTICATED,            1006 ),   /**< Not authenticated */                 \
    RESULT_ENUM( prefix, NOT_KEYED,                    1007 ),   /**< Not keyed */                         \
    RESULT_ENUM( prefix, IOCTL_FAIL,                   1008 ),   /**< IOCTL fail */                        \
    RESULT_ENUM( prefix, BUFFER_UNAVAILABLE_TEMPORARY, 1009 ),   /**< Buffer unavailable temporarily */    \
    RESULT_ENUM( prefix, BUFFER_UNAVAILABLE_PERMANENT, 1010 ),   /**< Buffer unavailable permanently */    \
    RESULT_ENUM( prefix, CONNECTION_LOST,              1012 ),   /**< Connection lost */                   \
    RESULT_ENUM( prefix, OUT_OF_EVENT_HANDLER_SPACE,   1013 ),   /**< Cannot add extra event handler */    \
    RESULT_ENUM( prefix, SEMAPHORE_ERROR,              1014 ),   /**< Error manipulating a semaphore */    \
    RESULT_ENUM( prefix, FLOW_CONTROLLED,              1015 ),   /**< Packet retrieval cancelled due to flow control */ \
    RESULT_ENUM( prefix, NO_CREDITS,                   1016 ),   /**< Packet retrieval cancelled due to lack of bus credits */ \
    RESULT_ENUM( prefix, NO_PACKET_TO_SEND,            1017 ),   /**< Packet retrieval cancelled due to no pending packets */ \
    RESULT_ENUM( prefix, CORE_CLOCK_NOT_ENABLED,       1018 ),   /**< Core disabled due to no clock */    \
    RESULT_ENUM( prefix, CORE_IN_RESET,                1019 ),   /**< Core disabled - in reset */         \
    RESULT_ENUM( prefix, UNSUPPORTED,                  1020 ),   /**< Unsupported function */             \
    RESULT_ENUM( prefix, BUS_WRITE_REGISTER_ERROR,     1021 ),   /**< Error writing to WLAN register */   \
    RESULT_ENUM( prefix, SDIO_BUS_UP_FAIL,             1022 ),   /**< SDIO bus failed to come up */       \
    RESULT_ENUM( prefix, JOIN_IN_PROGRESS,             1023 ),   /**< Join not finished yet */   \
    RESULT_ENUM( prefix, NETWORK_NOT_FOUND,            1024 ),   /**< Specified network was not found */   \
    RESULT_ENUM( prefix, INVALID_JOIN_STATUS,          1025 ),   /**< Join status error */   \
    RESULT_ENUM( prefix, UNKNOWN_INTERFACE,            1026 ),   /**< Unknown interface specified */ \
    RESULT_ENUM( prefix, SDIO_RX_FAIL,                 1027 ),   /**< Error during SDIO receive */   \
    RESULT_ENUM( prefix, HWTAG_MISMATCH,               1028 ),   /**< Hardware tag header corrupt */   \
    RESULT_ENUM( prefix, RX_BUFFER_ALLOC_FAIL,         1029 ),   /**< Failed to allocate a buffer to receive into */   \
    RESULT_ENUM( prefix, BUS_READ_REGISTER_ERROR,      1030 ),   /**< Error reading a bus hardware register */   \
    RESULT_ENUM( prefix, THREAD_CREATE_FAILED,         1031 ),   /**< Failed to create a new thread */   \
    RESULT_ENUM( prefix, QUEUE_ERROR,                  1032 ),   /**< Error manipulating a queue */   \
    RESULT_ENUM( prefix, BUFFER_POINTER_MOVE_ERROR,    1033 ),   /**< Error moving the current pointer of a packet buffer  */   \
    RESULT_ENUM( prefix, BUFFER_SIZE_SET_ERROR,        1034 ),   /**< Error setting size of packet buffer */   \
    RESULT_ENUM( prefix, THREAD_STACK_NULL,            1035 ),   /**< Null stack pointer passed when non null was reqired */   \
    RESULT_ENUM( prefix, THREAD_DELETE_FAIL,           1036 ),   /**< Error deleting a thread */   \
    RESULT_ENUM( prefix, SLEEP_ERROR,                  1037 ),   /**< Error sleeping a thread */ \
    RESULT_ENUM( prefix, BUFFER_ALLOC_FAIL,            1038 ),   /**< Failed to allocate a packet buffer */ \
    RESULT_ENUM( prefix, NO_PACKET_TO_RECEIVE,         1039 ),   /**< No Packets waiting to be received */ \
    RESULT_ENUM( prefix, INTERFACE_NOT_UP,             1040 ),   /**< Requested interface is not active */ \
    RESULT_ENUM( prefix, DELAY_TOO_LONG,               1041 ),   /**< Requested delay is too long */ \
    RESULT_ENUM( prefix, INVALID_DUTY_CYCLE,           1042 ),   /**< Duty cycle is outside limit 0 to 100 */ \
    RESULT_ENUM( prefix, PMK_WRONG_LENGTH,             1043 ),   /**< Returned pmk was the wrong length */ \
    RESULT_ENUM( prefix, UNKNOWN_SECURITY_TYPE,        1044 ),   /**< AP security type was unknown */ \
    RESULT_ENUM( prefix, WEP_NOT_ALLOWED,              1045 ),   /**< AP not allowed to use WEP - it is not secure - use Open instead */ \
    RESULT_ENUM( prefix, WPA_KEYLEN_BAD,               1046 ),   /**< WPA / WPA2 key length must be between 8 & 64 bytes */ \
    RESULT_ENUM( prefix, FILTER_NOT_FOUND,             1047 ),   /**< Specified filter id not found */ \
    RESULT_ENUM( prefix, SPI_ID_READ_FAIL,             1048 ),   /**< Failed to read 0xfeedbead SPI id from chip */ \
    RESULT_ENUM( prefix, SPI_SIZE_MISMATCH,            1049 ),   /**< Mismatch in sizes between SPI header and SDPCM header */ \
    RESULT_ENUM( prefix, ADDRESS_ALREADY_REGISTERED,   1050 ),   /**< Attempt to register a multicast address twice */ \
    RESULT_ENUM( prefix, SDIO_RETRIES_EXCEEDED,        1051 ),   /**< SDIO transfer failed too many times. */ \
    RESULT_ENUM( prefix, NULL_PTR_ARG,                 1052 ),   /**< Null Pointer argument passed to function. */ \
    RESULT_ENUM( prefix, THREAD_FINISH_FAIL,           1053 ),   /**< Error deleting a thread */ \
    RESULT_ENUM( prefix, WAIT_ABORTED,                 1054 ),   /**< Semaphore/mutex wait has been aborted */ \
    RESULT_ENUM( prefix, SET_BLOCK_ACK_WINDOW_FAIL,    1055 ),   /**< Failed to set block ack window */ \
    RESULT_ENUM( prefix, DELAY_TOO_SHORT,              1056 ),   /**< Requested delay is too short */ \
    RESULT_ENUM( prefix, INVALID_INTERFACE,            1057 ),   /**< Invalid interface provided */ \
    RESULT_ENUM( prefix, WEP_KEYLEN_BAD,               1058 ),   /**< WEP / WEP_SHARED key length must be 5 or 13 bytes */ \
    RESULT_ENUM( prefix, HANDLER_ALREADY_REGISTERED,   1059 ),   /**< EAPOL handler already registered */ \
    RESULT_ENUM( prefix, AP_ALREADY_UP,                1060 ),   /**< Soft AP or P2P group owner already up */ \
    RESULT_ENUM( prefix, EAPOL_KEY_PACKET_M1_TIMEOUT,  1061 ),   /**< Timeout occurred while waiting for EAPOL packet M1 from AP */ \
    RESULT_ENUM( prefix, EAPOL_KEY_PACKET_M3_TIMEOUT,  1062 ),   /**< Timeout occurred while waiting for EAPOL packet M3 from AP, which may indicate incorrect WPA2/WPA passphrase */ \
    RESULT_ENUM( prefix, EAPOL_KEY_PACKET_G1_TIMEOUT,  1063 ),   /**< Timeout occurred while waiting for EAPOL packet G1 from AP */ \
    RESULT_ENUM( prefix, EAPOL_KEY_FAILURE,            1064 ),   /**< Unknown failure occurred during the EAPOL key handshake */ \
    RESULT_ENUM( prefix, MALLOC_FAILURE,               1065 ),   /**< Memory allocation failure */ \
    RESULT_ENUM( prefix, ACCESS_POINT_NOT_FOUND,       1066 ),   /**< Access point not found */ \
    RESULT_ENUM( prefix, RTOS_ERROR,                   1067 ),   /**< RTOS operation failed */ \
    RESULT_ENUM( prefix, CLM_BLOB_DLOAD_ERROR,         1068 ),   /**< CLM blob download failed */


/* These Enum result values are returned directly from the WLAN during an ioctl or iovar call.
 * Values: 1100 - 1200
 */
#define WLAN_ENUM_OFFSET  (2000)

#define WLAN_RESULT_LIST( prefix ) \
    RESULT_ENUM( prefix, ERROR,                       2001 ),  /**< Generic Error */                     \
    RESULT_ENUM( prefix, BADARG,                      2002 ),  /**< Bad Argument */                      \
    RESULT_ENUM( prefix, BADOPTION,                   2003 ),  /**< Bad option */                        \
    RESULT_ENUM( prefix, NOTUP,                       2004 ),  /**< Not up */                            \
    RESULT_ENUM( prefix, NOTDOWN,                     2005 ),  /**< Not down */                          \
    RESULT_ENUM( prefix, NOTAP,                       2006 ),  /**< Not AP */                            \
    RESULT_ENUM( prefix, NOTSTA,                      2007 ),  /**< Not STA  */                          \
    RESULT_ENUM( prefix, BADKEYIDX,                   2008 ),  /**< BAD Key Index */                     \
    RESULT_ENUM( prefix, RADIOOFF,                    2009 ),  /**< Radio Off */                         \
    RESULT_ENUM( prefix, NOTBANDLOCKED,               2010 ),  /**< Not  band locked */                  \
    RESULT_ENUM( prefix, NOCLK,                       2011 ),  /**< No Clock */                          \
    RESULT_ENUM( prefix, BADRATESET,                  2012 ),  /**< BAD Rate valueset */                 \
    RESULT_ENUM( prefix, BADBAND,                     2013 ),  /**< BAD Band */                          \
    RESULT_ENUM( prefix, BUFTOOSHORT,                 2014 ),  /**< Buffer too short */                  \
    RESULT_ENUM( prefix, BUFTOOLONG,                  2015 ),  /**< Buffer too long */                   \
    RESULT_ENUM( prefix, BUSY,                        2016 ),  /**< Busy */                              \
    RESULT_ENUM( prefix, NOTASSOCIATED,               2017 ),  /**< Not Associated */                    \
    RESULT_ENUM( prefix, BADSSIDLEN,                  2018 ),  /**< Bad SSID len */                      \
    RESULT_ENUM( prefix, OUTOFRANGECHAN,              2019 ),  /**< Out of Range Channel */              \
    RESULT_ENUM( prefix, BADCHAN,                     2020 ),  /**< Bad Channel */                       \
    RESULT_ENUM( prefix, BADADDR,                     2021 ),  /**< Bad Address */                       \
    RESULT_ENUM( prefix, NORESOURCE,                  2022 ),  /**< Not Enough Resources */              \
    RESULT_ENUM( prefix, UNSUPPORTED,                 2023 ),  /**< Unsupported */                       \
    RESULT_ENUM( prefix, BADLEN,                      2024 ),  /**< Bad length */                        \
    RESULT_ENUM( prefix, NOTREADY,                    2025 ),  /**< Not Ready */                         \
    RESULT_ENUM( prefix, EPERM,                       2026 ),  /**< Not Permitted */                     \
    RESULT_ENUM( prefix, NOMEM,                       2027 ),  /**< No Memory */                         \
    RESULT_ENUM( prefix, ASSOCIATED,                  2028 ),  /**< Associated */                        \
    RESULT_ENUM( prefix, RANGE,                       2029 ),  /**< Not In Range */                      \
    RESULT_ENUM( prefix, NOTFOUND,                    2030 ),  /**< Not Found */                         \
    RESULT_ENUM( prefix, WME_NOT_ENABLED,             2031 ),  /**< WME Not Enabled */                   \
    RESULT_ENUM( prefix, TSPEC_NOTFOUND,              2032 ),  /**< TSPEC Not Found */                   \
    RESULT_ENUM( prefix, ACM_NOTSUPPORTED,            2033 ),  /**< ACM Not Supported */                 \
    RESULT_ENUM( prefix, NOT_WME_ASSOCIATION,         2034 ),  /**< Not WME Association */               \
    RESULT_ENUM( prefix, SDIO_ERROR,                  2035 ),  /**< SDIO Bus Error */                    \
    RESULT_ENUM( prefix, WLAN_DOWN,                   2036 ),  /**< WLAN Not Accessible */               \
    RESULT_ENUM( prefix, BAD_VERSION,                 2037 ),  /**< Incorrect version */                 \
    RESULT_ENUM( prefix, TXFAIL,                      2038 ),  /**< TX failure */                        \
    RESULT_ENUM( prefix, RXFAIL,                      2039 ),  /**< RX failure */                        \
    RESULT_ENUM( prefix, NODEVICE,                    2040 ),  /**< Device not present */                \
    RESULT_ENUM( prefix, UNFINISHED,                  2041 ),  /**< To be finished */                    \
    RESULT_ENUM( prefix, NONRESIDENT,                 2042 ),  /**< access to nonresident overlay */     \
    RESULT_ENUM( prefix, DISABLED,                    2043 ),  /**< Disabled in this build */

/**
 * Common result type for WICED functions
 */
typedef enum
{
    WWD_RESULT_LIST( WWD_ )
    WLAN_RESULT_LIST( WWD_WLAN_ )
} wwd_result_t;

#ifndef USE_BOOL_UINT32
/**
 * Boolean values
 */
typedef enum
{
    WICED_FALSE = 0,
    WICED_TRUE  = 1
} wiced_bool_t;
#else
/**
 * Boolean values
 * wiced_bool_t typedef to enum allows compiler to potentially optimize the size,
 * This can create issues when other pre-build code like BT patch ROM typedef wiced_bool_t
 * to other types.
 */
enum wiced_bool
{
    WICED_FALSE = 0,
    WICED_TRUE  = 1
};
typedef unsigned int wiced_bool_t;
#endif

/**
 * I/O State Values
 */
typedef enum
{
    WWD_ACTIVE_LOW = 0,
    WWD_ACTIVE_HIGH = 1
} wwd_io_state_t;

/**
 * Enumeration of Dot11 Reason Codes
 */
typedef enum
{
    WWD_DOT11_RC_RESERVED  = 0,    /**< Reserved     */
    WWD_DOT11_RC_UNSPECIFIED  = 1  /**< Unspecified  */
} wwd_dot11_reason_code_t;


/******************************************************
 * @cond      Constants
 ******************************************************/

/**
 * Transfer direction for the WICED platform bus interface
 */
typedef enum
{
    /* If updating this enum, the bus_direction_mapping variable will also need to be updated */
    BUS_READ,
    BUS_WRITE
} wwd_bus_transfer_direction_t;

/**
 * Macro for creating country codes according to endianness
 * @cond !ADDTHIS
 */

#ifdef IL_BIGENDIAN
#define MK_CNTRY( a, b, rev )  (((unsigned char)(b)) + (((unsigned char)(a))<<8) + (((unsigned short)(rev))<<16))
#else /* ifdef IL_BIGENDIAN */
#define MK_CNTRY( a, b, rev )  (((unsigned char)(a)) + (((unsigned char)(b))<<8) + (((unsigned short)(rev))<<16))
#endif /* ifdef IL_BIGENDIAN */

/* Suppress unused parameter warning */
#ifndef UNUSED_PARAMETER
#define UNUSED_PARAMETER(x) /*@-noeffect@*/ ( (void)(x) ) /*@+noeffect@*/
#endif

/* Suppress unused variable warning */
#ifndef UNUSED_VARIABLE
#define UNUSED_VARIABLE(x) /*@-noeffect@*/ ( (void)(x) ) /*@+noeffect@*/
#endif


/* Suppress unused variable warning occurring due to an assert which is disabled in release mode */
#ifndef REFERENCE_DEBUG_ONLY_VARIABLE
#define REFERENCE_DEBUG_ONLY_VARIABLE(x) /*@-noeffect@*/ ( (void)(x) ) /*@+noeffect@*/
#endif

/* Allow functions to be deprecated */
#ifdef __GNUC__
#define DEPRECATE( name )  name __attribute__ ((deprecated))
#else
#define DEPRECATE( name )  deprecated_ ## name
#endif

/*
 * @endcond
 */

/**
 * Enumerated list of country codes
 */
typedef enum
{
    WICED_COUNTRY_AFGHANISTAN                                     = MK_CNTRY( 'A', 'F', 0 ),             /* AF Afghanistan */
    WICED_COUNTRY_ALBANIA                                         = MK_CNTRY( 'A', 'L', 0 ),             /* AL Albania */
    WICED_COUNTRY_ALGERIA                                         = MK_CNTRY( 'D', 'Z', 0 ),             /* DZ Algeria */
    WICED_COUNTRY_AMERICAN_SAMOA                                  = MK_CNTRY( 'A', 'S', 0 ),             /* AS American_Samoa */
    WICED_COUNTRY_ANGOLA                                          = MK_CNTRY( 'A', 'O', 0 ),             /* AO Angola */
    WICED_COUNTRY_ANGUILLA                                        = MK_CNTRY( 'A', 'I', 0 ),             /* AI Anguilla */
    WICED_COUNTRY_ANTIGUA_AND_BARBUDA                             = MK_CNTRY( 'A', 'G', 0 ),             /* AG Antigua_and_Barbuda */
    WICED_COUNTRY_ARGENTINA                                       = MK_CNTRY( 'A', 'R', 0 ),             /* AR Argentina */
    WICED_COUNTRY_ARMENIA                                         = MK_CNTRY( 'A', 'M', 0 ),             /* AM Armenia */
    WICED_COUNTRY_ARUBA                                           = MK_CNTRY( 'A', 'W', 0 ),             /* AW Aruba */
    WICED_COUNTRY_AUSTRALIA                                       = MK_CNTRY( 'A', 'U', 0 ),             /* AU Australia */
    WICED_COUNTRY_AUSTRIA                                         = MK_CNTRY( 'A', 'T', 0 ),             /* AT Austria */
    WICED_COUNTRY_AZERBAIJAN                                      = MK_CNTRY( 'A', 'Z', 0 ),             /* AZ Azerbaijan */
    WICED_COUNTRY_BAHAMAS                                         = MK_CNTRY( 'B', 'S', 0 ),             /* BS Bahamas */
    WICED_COUNTRY_BAHRAIN                                         = MK_CNTRY( 'B', 'H', 0 ),             /* BH Bahrain */
    WICED_COUNTRY_BAKER_ISLAND                                    = MK_CNTRY( '0', 'B', 0 ),             /* 0B Baker_Island */
    WICED_COUNTRY_BANGLADESH                                      = MK_CNTRY( 'B', 'D', 0 ),             /* BD Bangladesh */
    WICED_COUNTRY_BARBADOS                                        = MK_CNTRY( 'B', 'B', 0 ),             /* BB Barbados */
    WICED_COUNTRY_BELARUS                                         = MK_CNTRY( 'B', 'Y', 0 ),             /* BY Belarus */
    WICED_COUNTRY_BELGIUM                                         = MK_CNTRY( 'B', 'E', 0 ),             /* BE Belgium */
    WICED_COUNTRY_BELIZE                                          = MK_CNTRY( 'B', 'Z', 0 ),             /* BZ Belize */
    WICED_COUNTRY_BENIN                                           = MK_CNTRY( 'B', 'J', 0 ),             /* BJ Benin */
    WICED_COUNTRY_BERMUDA                                         = MK_CNTRY( 'B', 'M', 0 ),             /* BM Bermuda */
    WICED_COUNTRY_BHUTAN                                          = MK_CNTRY( 'B', 'T', 0 ),             /* BT Bhutan */
    WICED_COUNTRY_BOLIVIA                                         = MK_CNTRY( 'B', 'O', 0 ),             /* BO Bolivia */
    WICED_COUNTRY_BOSNIA_AND_HERZEGOVINA                          = MK_CNTRY( 'B', 'A', 0 ),             /* BA Bosnia_and_Herzegovina */
    WICED_COUNTRY_BOTSWANA                                        = MK_CNTRY( 'B', 'W', 0 ),             /* BW Botswana */
    WICED_COUNTRY_BRAZIL                                          = MK_CNTRY( 'B', 'R', 0 ),             /* BR Brazil */
    WICED_COUNTRY_BRITISH_INDIAN_OCEAN_TERRITORY                  = MK_CNTRY( 'I', 'O', 0 ),             /* IO British_Indian_Ocean_Territory */
    WICED_COUNTRY_BRUNEI_DARUSSALAM                               = MK_CNTRY( 'B', 'N', 0 ),             /* BN Brunei_Darussalam */
    WICED_COUNTRY_BULGARIA                                        = MK_CNTRY( 'B', 'G', 0 ),             /* BG Bulgaria */
    WICED_COUNTRY_BURKINA_FASO                                    = MK_CNTRY( 'B', 'F', 0 ),             /* BF Burkina_Faso */
    WICED_COUNTRY_BURUNDI                                         = MK_CNTRY( 'B', 'I', 0 ),             /* BI Burundi */
    WICED_COUNTRY_CAMBODIA                                        = MK_CNTRY( 'K', 'H', 0 ),             /* KH Cambodia */
    WICED_COUNTRY_CAMEROON                                        = MK_CNTRY( 'C', 'M', 0 ),             /* CM Cameroon */
    WICED_COUNTRY_CANADA                                          = MK_CNTRY( 'C', 'A', 0 ),             /* CA Canada */
    WICED_COUNTRY_CANADA_REV950                                   = MK_CNTRY( 'C', 'A', 950 ),           /* CA Canada Revision 950 */
    WICED_COUNTRY_CAPE_VERDE                                      = MK_CNTRY( 'C', 'V', 0 ),             /* CV Cape_Verde */
    WICED_COUNTRY_CAYMAN_ISLANDS                                  = MK_CNTRY( 'K', 'Y', 0 ),             /* KY Cayman_Islands */
    WICED_COUNTRY_CENTRAL_AFRICAN_REPUBLIC                        = MK_CNTRY( 'C', 'F', 0 ),             /* CF Central_African_Republic */
    WICED_COUNTRY_CHAD                                            = MK_CNTRY( 'T', 'D', 0 ),             /* TD Chad */
    WICED_COUNTRY_CHILE                                           = MK_CNTRY( 'C', 'L', 0 ),             /* CL Chile */
    WICED_COUNTRY_CHINA                                           = MK_CNTRY( 'C', 'N', 0 ),             /* CN China */
    WICED_COUNTRY_CHRISTMAS_ISLAND                                = MK_CNTRY( 'C', 'X', 0 ),             /* CX Christmas_Island */
    WICED_COUNTRY_COLOMBIA                                        = MK_CNTRY( 'C', 'O', 0 ),             /* CO Colombia */
    WICED_COUNTRY_COMOROS                                         = MK_CNTRY( 'K', 'M', 0 ),             /* KM Comoros */
    WICED_COUNTRY_CONGO                                           = MK_CNTRY( 'C', 'G', 0 ),             /* CG Congo */
    WICED_COUNTRY_CONGO_THE_DEMOCRATIC_REPUBLIC_OF_THE            = MK_CNTRY( 'C', 'D', 0 ),             /* CD Congo,_The_Democratic_Republic_Of_The */
    WICED_COUNTRY_COSTA_RICA                                      = MK_CNTRY( 'C', 'R', 0 ),             /* CR Costa_Rica */
    WICED_COUNTRY_COTE_DIVOIRE                                    = MK_CNTRY( 'C', 'I', 0 ),             /* CI Cote_D'ivoire */
    WICED_COUNTRY_CROATIA                                         = MK_CNTRY( 'H', 'R', 0 ),             /* HR Croatia */
    WICED_COUNTRY_CUBA                                            = MK_CNTRY( 'C', 'U', 0 ),             /* CU Cuba */
    WICED_COUNTRY_CYPRUS                                          = MK_CNTRY( 'C', 'Y', 0 ),             /* CY Cyprus */
    WICED_COUNTRY_CZECH_REPUBLIC                                  = MK_CNTRY( 'C', 'Z', 0 ),             /* CZ Czech_Republic */
    WICED_COUNTRY_DENMARK                                         = MK_CNTRY( 'D', 'K', 0 ),             /* DK Denmark */
    WICED_COUNTRY_DJIBOUTI                                        = MK_CNTRY( 'D', 'J', 0 ),             /* DJ Djibouti */
    WICED_COUNTRY_DOMINICA                                        = MK_CNTRY( 'D', 'M', 0 ),             /* DM Dominica */
    WICED_COUNTRY_DOMINICAN_REPUBLIC                              = MK_CNTRY( 'D', 'O', 0 ),             /* DO Dominican_Republic */
    WICED_COUNTRY_DOWN_UNDER                                      = MK_CNTRY( 'A', 'U', 0 ),             /* AU G'Day mate! */
    WICED_COUNTRY_ECUADOR                                         = MK_CNTRY( 'E', 'C', 0 ),             /* EC Ecuador */
    WICED_COUNTRY_EGYPT                                           = MK_CNTRY( 'E', 'G', 0 ),             /* EG Egypt */
    WICED_COUNTRY_EL_SALVADOR                                     = MK_CNTRY( 'S', 'V', 0 ),             /* SV El_Salvador */
    WICED_COUNTRY_EQUATORIAL_GUINEA                               = MK_CNTRY( 'G', 'Q', 0 ),             /* GQ Equatorial_Guinea */
    WICED_COUNTRY_ERITREA                                         = MK_CNTRY( 'E', 'R', 0 ),             /* ER Eritrea */
    WICED_COUNTRY_ESTONIA                                         = MK_CNTRY( 'E', 'E', 0 ),             /* EE Estonia */
    WICED_COUNTRY_ETHIOPIA                                        = MK_CNTRY( 'E', 'T', 0 ),             /* ET Ethiopia */
    WICED_COUNTRY_FALKLAND_ISLANDS_MALVINAS                       = MK_CNTRY( 'F', 'K', 0 ),             /* FK Falkland_Islands_(Malvinas) */
    WICED_COUNTRY_FAROE_ISLANDS                                   = MK_CNTRY( 'F', 'O', 0 ),             /* FO Faroe_Islands */
    WICED_COUNTRY_FIJI                                            = MK_CNTRY( 'F', 'J', 0 ),             /* FJ Fiji */
    WICED_COUNTRY_FINLAND                                         = MK_CNTRY( 'F', 'I', 0 ),             /* FI Finland */
    WICED_COUNTRY_FRANCE                                          = MK_CNTRY( 'F', 'R', 0 ),             /* FR France */
    WICED_COUNTRY_FRENCH_GUINA                                    = MK_CNTRY( 'G', 'F', 0 ),             /* GF French_Guina */
    WICED_COUNTRY_FRENCH_POLYNESIA                                = MK_CNTRY( 'P', 'F', 0 ),             /* PF French_Polynesia */
    WICED_COUNTRY_FRENCH_SOUTHERN_TERRITORIES                     = MK_CNTRY( 'T', 'F', 0 ),             /* TF French_Southern_Territories */
    WICED_COUNTRY_GABON                                           = MK_CNTRY( 'G', 'A', 0 ),             /* GA Gabon */
    WICED_COUNTRY_GAMBIA                                          = MK_CNTRY( 'G', 'M', 0 ),             /* GM Gambia */
    WICED_COUNTRY_GEORGIA                                         = MK_CNTRY( 'G', 'E', 0 ),             /* GE Georgia */
    WICED_COUNTRY_GERMANY                                         = MK_CNTRY( 'D', 'E', 0 ),             /* DE Germany */
    WICED_COUNTRY_EUROPEAN_WIDE_REV895                            = MK_CNTRY( 'E', '0', 895 ),           /* E0 European_Wide Revision 895 */
    WICED_COUNTRY_GHANA                                           = MK_CNTRY( 'G', 'H', 0 ),             /* GH Ghana */
    WICED_COUNTRY_GIBRALTAR                                       = MK_CNTRY( 'G', 'I', 0 ),             /* GI Gibraltar */
    WICED_COUNTRY_GREECE                                          = MK_CNTRY( 'G', 'R', 0 ),             /* GR Greece */
    WICED_COUNTRY_GRENADA                                         = MK_CNTRY( 'G', 'D', 0 ),             /* GD Grenada */
    WICED_COUNTRY_GUADELOUPE                                      = MK_CNTRY( 'G', 'P', 0 ),             /* GP Guadeloupe */
    WICED_COUNTRY_GUAM                                            = MK_CNTRY( 'G', 'U', 0 ),             /* GU Guam */
    WICED_COUNTRY_GUATEMALA                                       = MK_CNTRY( 'G', 'T', 0 ),             /* GT Guatemala */
    WICED_COUNTRY_GUERNSEY                                        = MK_CNTRY( 'G', 'G', 0 ),             /* GG Guernsey */
    WICED_COUNTRY_GUINEA                                          = MK_CNTRY( 'G', 'N', 0 ),             /* GN Guinea */
    WICED_COUNTRY_GUINEA_BISSAU                                   = MK_CNTRY( 'G', 'W', 0 ),             /* GW Guinea-bissau */
    WICED_COUNTRY_GUYANA                                          = MK_CNTRY( 'G', 'Y', 0 ),             /* GY Guyana */
    WICED_COUNTRY_HAITI                                           = MK_CNTRY( 'H', 'T', 0 ),             /* HT Haiti */
    WICED_COUNTRY_HOLY_SEE_VATICAN_CITY_STATE                     = MK_CNTRY( 'V', 'A', 0 ),             /* VA Holy_See_(Vatican_City_State) */
    WICED_COUNTRY_HONDURAS                                        = MK_CNTRY( 'H', 'N', 0 ),             /* HN Honduras */
    WICED_COUNTRY_HONG_KONG                                       = MK_CNTRY( 'H', 'K', 0 ),             /* HK Hong_Kong */
    WICED_COUNTRY_HUNGARY                                         = MK_CNTRY( 'H', 'U', 0 ),             /* HU Hungary */
    WICED_COUNTRY_ICELAND                                         = MK_CNTRY( 'I', 'S', 0 ),             /* IS Iceland */
    WICED_COUNTRY_INDIA                                           = MK_CNTRY( 'I', 'N', 0 ),             /* IN India */
    WICED_COUNTRY_INDONESIA                                       = MK_CNTRY( 'I', 'D', 0 ),             /* ID Indonesia */
    WICED_COUNTRY_IRAN_ISLAMIC_REPUBLIC_OF                        = MK_CNTRY( 'I', 'R', 0 ),             /* IR Iran,_Islamic_Republic_Of */
    WICED_COUNTRY_IRAQ                                            = MK_CNTRY( 'I', 'Q', 0 ),             /* IQ Iraq */
    WICED_COUNTRY_IRELAND                                         = MK_CNTRY( 'I', 'E', 0 ),             /* IE Ireland */
    WICED_COUNTRY_ISRAEL                                          = MK_CNTRY( 'I', 'L', 0 ),             /* IL Israel */
    WICED_COUNTRY_ITALY                                           = MK_CNTRY( 'I', 'T', 0 ),             /* IT Italy */
    WICED_COUNTRY_JAMAICA                                         = MK_CNTRY( 'J', 'M', 0 ),             /* JM Jamaica */
    WICED_COUNTRY_JAPAN                                           = MK_CNTRY( 'J', 'P', 0 ),             /* JP Japan */
    WICED_COUNTRY_JERSEY                                          = MK_CNTRY( 'J', 'E', 0 ),             /* JE Jersey */
    WICED_COUNTRY_JORDAN                                          = MK_CNTRY( 'J', 'O', 0 ),             /* JO Jordan */
    WICED_COUNTRY_KAZAKHSTAN                                      = MK_CNTRY( 'K', 'Z', 0 ),             /* KZ Kazakhstan */
    WICED_COUNTRY_KENYA                                           = MK_CNTRY( 'K', 'E', 0 ),             /* KE Kenya */
    WICED_COUNTRY_KIRIBATI                                        = MK_CNTRY( 'K', 'I', 0 ),             /* KI Kiribati */
    WICED_COUNTRY_KOREA_REPUBLIC_OF                               = MK_CNTRY( 'K', 'R', 1 ),             /* KR Korea,_Republic_Of */
    WICED_COUNTRY_KOSOVO                                          = MK_CNTRY( '0', 'A', 0 ),             /* 0A Kosovo */
    WICED_COUNTRY_KUWAIT                                          = MK_CNTRY( 'K', 'W', 0 ),             /* KW Kuwait */
    WICED_COUNTRY_KYRGYZSTAN                                      = MK_CNTRY( 'K', 'G', 0 ),             /* KG Kyrgyzstan */
    WICED_COUNTRY_LAO_PEOPLES_DEMOCRATIC_REPUBIC                  = MK_CNTRY( 'L', 'A', 0 ),             /* LA Lao_People's_Democratic_Repubic */
    WICED_COUNTRY_LATVIA                                          = MK_CNTRY( 'L', 'V', 0 ),             /* LV Latvia */
    WICED_COUNTRY_LEBANON                                         = MK_CNTRY( 'L', 'B', 0 ),             /* LB Lebanon */
    WICED_COUNTRY_LESOTHO                                         = MK_CNTRY( 'L', 'S', 0 ),             /* LS Lesotho */
    WICED_COUNTRY_LIBERIA                                         = MK_CNTRY( 'L', 'R', 0 ),             /* LR Liberia */
    WICED_COUNTRY_LIBYAN_ARAB_JAMAHIRIYA                          = MK_CNTRY( 'L', 'Y', 0 ),             /* LY Libyan_Arab_Jamahiriya */
    WICED_COUNTRY_LIECHTENSTEIN                                   = MK_CNTRY( 'L', 'I', 0 ),             /* LI Liechtenstein */
    WICED_COUNTRY_LITHUANIA                                       = MK_CNTRY( 'L', 'T', 0 ),             /* LT Lithuania */
    WICED_COUNTRY_LUXEMBOURG                                      = MK_CNTRY( 'L', 'U', 0 ),             /* LU Luxembourg */
    WICED_COUNTRY_MACAO                                           = MK_CNTRY( 'M', 'O', 0 ),             /* MO Macao */
    WICED_COUNTRY_MACEDONIA_FORMER_YUGOSLAV_REPUBLIC_OF           = MK_CNTRY( 'M', 'K', 0 ),             /* MK Macedonia,_Former_Yugoslav_Republic_Of */
    WICED_COUNTRY_MADAGASCAR                                      = MK_CNTRY( 'M', 'G', 0 ),             /* MG Madagascar */
    WICED_COUNTRY_MALAWI                                          = MK_CNTRY( 'M', 'W', 0 ),             /* MW Malawi */
    WICED_COUNTRY_MALAYSIA                                        = MK_CNTRY( 'M', 'Y', 0 ),             /* MY Malaysia */
    WICED_COUNTRY_MALDIVES                                        = MK_CNTRY( 'M', 'V', 0 ),             /* MV Maldives */
    WICED_COUNTRY_MALI                                            = MK_CNTRY( 'M', 'L', 0 ),             /* ML Mali */
    WICED_COUNTRY_MALTA                                           = MK_CNTRY( 'M', 'T', 0 ),             /* MT Malta */
    WICED_COUNTRY_MAN_ISLE_OF                                     = MK_CNTRY( 'I', 'M', 0 ),             /* IM Man,_Isle_Of */
    WICED_COUNTRY_MARTINIQUE                                      = MK_CNTRY( 'M', 'Q', 0 ),             /* MQ Martinique */
    WICED_COUNTRY_MAURITANIA                                      = MK_CNTRY( 'M', 'R', 0 ),             /* MR Mauritania */
    WICED_COUNTRY_MAURITIUS                                       = MK_CNTRY( 'M', 'U', 0 ),             /* MU Mauritius */
    WICED_COUNTRY_MAYOTTE                                         = MK_CNTRY( 'Y', 'T', 0 ),             /* YT Mayotte */
    WICED_COUNTRY_MEXICO                                          = MK_CNTRY( 'M', 'X', 0 ),             /* MX Mexico */
    WICED_COUNTRY_MICRONESIA_FEDERATED_STATES_OF                  = MK_CNTRY( 'F', 'M', 0 ),             /* FM Micronesia,_Federated_States_Of */
    WICED_COUNTRY_MOLDOVA_REPUBLIC_OF                             = MK_CNTRY( 'M', 'D', 0 ),             /* MD Moldova,_Republic_Of */
    WICED_COUNTRY_MONACO                                          = MK_CNTRY( 'M', 'C', 0 ),             /* MC Monaco */
    WICED_COUNTRY_MONGOLIA                                        = MK_CNTRY( 'M', 'N', 0 ),             /* MN Mongolia */
    WICED_COUNTRY_MONTENEGRO                                      = MK_CNTRY( 'M', 'E', 0 ),             /* ME Montenegro */
    WICED_COUNTRY_MONTSERRAT                                      = MK_CNTRY( 'M', 'S', 0 ),             /* MS Montserrat */
    WICED_COUNTRY_MOROCCO                                         = MK_CNTRY( 'M', 'A', 0 ),             /* MA Morocco */
    WICED_COUNTRY_MOZAMBIQUE                                      = MK_CNTRY( 'M', 'Z', 0 ),             /* MZ Mozambique */
    WICED_COUNTRY_MYANMAR                                         = MK_CNTRY( 'M', 'M', 0 ),             /* MM Myanmar */
    WICED_COUNTRY_NAMIBIA                                         = MK_CNTRY( 'N', 'A', 0 ),             /* NA Namibia */
    WICED_COUNTRY_NAURU                                           = MK_CNTRY( 'N', 'R', 0 ),             /* NR Nauru */
    WICED_COUNTRY_NEPAL                                           = MK_CNTRY( 'N', 'P', 0 ),             /* NP Nepal */
    WICED_COUNTRY_NETHERLANDS                                     = MK_CNTRY( 'N', 'L', 0 ),             /* NL Netherlands */
    WICED_COUNTRY_NETHERLANDS_ANTILLES                            = MK_CNTRY( 'A', 'N', 0 ),             /* AN Netherlands_Antilles */
    WICED_COUNTRY_NEW_CALEDONIA                                   = MK_CNTRY( 'N', 'C', 0 ),             /* NC New_Caledonia */
    WICED_COUNTRY_NEW_ZEALAND                                     = MK_CNTRY( 'N', 'Z', 0 ),             /* NZ New_Zealand */
    WICED_COUNTRY_NICARAGUA                                       = MK_CNTRY( 'N', 'I', 0 ),             /* NI Nicaragua */
    WICED_COUNTRY_NIGER                                           = MK_CNTRY( 'N', 'E', 0 ),             /* NE Niger */
    WICED_COUNTRY_NIGERIA                                         = MK_CNTRY( 'N', 'G', 0 ),             /* NG Nigeria */
    WICED_COUNTRY_NORFOLK_ISLAND                                  = MK_CNTRY( 'N', 'F', 0 ),             /* NF Norfolk_Island */
    WICED_COUNTRY_NORTHERN_MARIANA_ISLANDS                        = MK_CNTRY( 'M', 'P', 0 ),             /* MP Northern_Mariana_Islands */
    WICED_COUNTRY_NORWAY                                          = MK_CNTRY( 'N', 'O', 0 ),             /* NO Norway */
    WICED_COUNTRY_OMAN                                            = MK_CNTRY( 'O', 'M', 0 ),             /* OM Oman */
    WICED_COUNTRY_PAKISTAN                                        = MK_CNTRY( 'P', 'K', 0 ),             /* PK Pakistan */
    WICED_COUNTRY_PALAU                                           = MK_CNTRY( 'P', 'W', 0 ),             /* PW Palau */
    WICED_COUNTRY_PANAMA                                          = MK_CNTRY( 'P', 'A', 0 ),             /* PA Panama */
    WICED_COUNTRY_PAPUA_NEW_GUINEA                                = MK_CNTRY( 'P', 'G', 0 ),             /* PG Papua_New_Guinea */
    WICED_COUNTRY_PARAGUAY                                        = MK_CNTRY( 'P', 'Y', 0 ),             /* PY Paraguay */
    WICED_COUNTRY_PERU                                            = MK_CNTRY( 'P', 'E', 0 ),             /* PE Peru */
    WICED_COUNTRY_PHILIPPINES                                     = MK_CNTRY( 'P', 'H', 0 ),             /* PH Philippines */
    WICED_COUNTRY_POLAND                                          = MK_CNTRY( 'P', 'L', 0 ),             /* PL Poland */
    WICED_COUNTRY_PORTUGAL                                        = MK_CNTRY( 'P', 'T', 0 ),             /* PT Portugal */
    WICED_COUNTRY_PUETO_RICO                                      = MK_CNTRY( 'P', 'R', 0 ),             /* PR Pueto_Rico */
    WICED_COUNTRY_QATAR                                           = MK_CNTRY( 'Q', 'A', 0 ),             /* QA Qatar */
    WICED_COUNTRY_REUNION                                         = MK_CNTRY( 'R', 'E', 0 ),             /* RE Reunion */
    WICED_COUNTRY_ROMANIA                                         = MK_CNTRY( 'R', 'O', 0 ),             /* RO Romania */
    WICED_COUNTRY_RUSSIAN_FEDERATION                              = MK_CNTRY( 'R', 'U', 0 ),             /* RU Russian_Federation */
    WICED_COUNTRY_RWANDA                                          = MK_CNTRY( 'R', 'W', 0 ),             /* RW Rwanda */
    WICED_COUNTRY_SAINT_KITTS_AND_NEVIS                           = MK_CNTRY( 'K', 'N', 0 ),             /* KN Saint_Kitts_and_Nevis */
    WICED_COUNTRY_SAINT_LUCIA                                     = MK_CNTRY( 'L', 'C', 0 ),             /* LC Saint_Lucia */
    WICED_COUNTRY_SAINT_PIERRE_AND_MIQUELON                       = MK_CNTRY( 'P', 'M', 0 ),             /* PM Saint_Pierre_and_Miquelon */
    WICED_COUNTRY_SAINT_VINCENT_AND_THE_GRENADINES                = MK_CNTRY( 'V', 'C', 0 ),             /* VC Saint_Vincent_and_The_Grenadines */
    WICED_COUNTRY_SAMOA                                           = MK_CNTRY( 'W', 'S', 0 ),             /* WS Samoa */
    WICED_COUNTRY_SANIT_MARTIN_SINT_MARTEEN                       = MK_CNTRY( 'M', 'F', 0 ),             /* MF Sanit_Martin_/_Sint_Marteen */
    WICED_COUNTRY_SAO_TOME_AND_PRINCIPE                           = MK_CNTRY( 'S', 'T', 0 ),             /* ST Sao_Tome_and_Principe */
    WICED_COUNTRY_SAUDI_ARABIA                                    = MK_CNTRY( 'S', 'A', 0 ),             /* SA Saudi_Arabia */
    WICED_COUNTRY_SENEGAL                                         = MK_CNTRY( 'S', 'N', 0 ),             /* SN Senegal */
    WICED_COUNTRY_SERBIA                                          = MK_CNTRY( 'R', 'S', 0 ),             /* RS Serbia */
    WICED_COUNTRY_SEYCHELLES                                      = MK_CNTRY( 'S', 'C', 0 ),             /* SC Seychelles */
    WICED_COUNTRY_SIERRA_LEONE                                    = MK_CNTRY( 'S', 'L', 0 ),             /* SL Sierra_Leone */
    WICED_COUNTRY_SINGAPORE                                       = MK_CNTRY( 'S', 'G', 0 ),             /* SG Singapore */
    WICED_COUNTRY_SLOVAKIA                                        = MK_CNTRY( 'S', 'K', 0 ),             /* SK Slovakia */
    WICED_COUNTRY_SLOVENIA                                        = MK_CNTRY( 'S', 'I', 0 ),             /* SI Slovenia */
    WICED_COUNTRY_SOLOMON_ISLANDS                                 = MK_CNTRY( 'S', 'B', 0 ),             /* SB Solomon_Islands */
    WICED_COUNTRY_SOMALIA                                         = MK_CNTRY( 'S', 'O', 0 ),             /* SO Somalia */
    WICED_COUNTRY_SOUTH_AFRICA                                    = MK_CNTRY( 'Z', 'A', 0 ),             /* ZA South_Africa */
    WICED_COUNTRY_SPAIN                                           = MK_CNTRY( 'E', 'S', 0 ),             /* ES Spain */
    WICED_COUNTRY_SRI_LANKA                                       = MK_CNTRY( 'L', 'K', 0 ),             /* LK Sri_Lanka */
    WICED_COUNTRY_SURINAME                                        = MK_CNTRY( 'S', 'R', 0 ),             /* SR Suriname */
    WICED_COUNTRY_SWAZILAND                                       = MK_CNTRY( 'S', 'Z', 0 ),             /* SZ Swaziland */
    WICED_COUNTRY_SWEDEN                                          = MK_CNTRY( 'S', 'E', 0 ),             /* SE Sweden */
    WICED_COUNTRY_SWITZERLAND                                     = MK_CNTRY( 'C', 'H', 0 ),             /* CH Switzerland */
    WICED_COUNTRY_SYRIAN_ARAB_REPUBLIC                            = MK_CNTRY( 'S', 'Y', 0 ),             /* SY Syrian_Arab_Republic */
    WICED_COUNTRY_TAIWAN_PROVINCE_OF_CHINA                        = MK_CNTRY( 'T', 'W', 0 ),             /* TW Taiwan,_Province_Of_China */
    WICED_COUNTRY_TAJIKISTAN                                      = MK_CNTRY( 'T', 'J', 0 ),             /* TJ Tajikistan */
    WICED_COUNTRY_TANZANIA_UNITED_REPUBLIC_OF                     = MK_CNTRY( 'T', 'Z', 0 ),             /* TZ Tanzania,_United_Republic_Of */
    WICED_COUNTRY_THAILAND                                        = MK_CNTRY( 'T', 'H', 0 ),             /* TH Thailand */
    WICED_COUNTRY_TOGO                                            = MK_CNTRY( 'T', 'G', 0 ),             /* TG Togo */
    WICED_COUNTRY_TONGA                                           = MK_CNTRY( 'T', 'O', 0 ),             /* TO Tonga */
    WICED_COUNTRY_TRINIDAD_AND_TOBAGO                             = MK_CNTRY( 'T', 'T', 0 ),             /* TT Trinidad_and_Tobago */
    WICED_COUNTRY_TUNISIA                                         = MK_CNTRY( 'T', 'N', 0 ),             /* TN Tunisia */
    WICED_COUNTRY_TURKEY                                          = MK_CNTRY( 'T', 'R', 0 ),             /* TR Turkey */
    WICED_COUNTRY_TURKMENISTAN                                    = MK_CNTRY( 'T', 'M', 0 ),             /* TM Turkmenistan */
    WICED_COUNTRY_TURKS_AND_CAICOS_ISLANDS                        = MK_CNTRY( 'T', 'C', 0 ),             /* TC Turks_and_Caicos_Islands */
    WICED_COUNTRY_TUVALU                                          = MK_CNTRY( 'T', 'V', 0 ),             /* TV Tuvalu */
    WICED_COUNTRY_UGANDA                                          = MK_CNTRY( 'U', 'G', 0 ),             /* UG Uganda */
    WICED_COUNTRY_UKRAINE                                         = MK_CNTRY( 'U', 'A', 0 ),             /* UA Ukraine */
    WICED_COUNTRY_UNITED_ARAB_EMIRATES                            = MK_CNTRY( 'A', 'E', 0 ),             /* AE United_Arab_Emirates */
    WICED_COUNTRY_UNITED_KINGDOM                                  = MK_CNTRY( 'G', 'B', 0 ),             /* GB United_Kingdom */
    WICED_COUNTRY_UNITED_STATES                                   = MK_CNTRY( 'U', 'S', 0 ),             /* US United_States */
    WICED_COUNTRY_UNITED_STATES_REV4                              = MK_CNTRY( 'U', 'S', 4 ),             /* US United_States Revision 4 */
    WICED_COUNTRY_UNITED_STATES_REV931                            = MK_CNTRY( 'Q', '1', 931 ),           /* Q1 United_States Revision 931 */
    WICED_COUNTRY_UNITED_STATES_NO_DFS                            = MK_CNTRY( 'Q', '2', 0 ),             /* Q2 United_States_(No_DFS) */
    WICED_COUNTRY_UNITED_STATES_MINOR_OUTLYING_ISLANDS            = MK_CNTRY( 'U', 'M', 0 ),             /* UM United_States_Minor_Outlying_Islands */
    WICED_COUNTRY_URUGUAY                                         = MK_CNTRY( 'U', 'Y', 0 ),             /* UY Uruguay */
    WICED_COUNTRY_UZBEKISTAN                                      = MK_CNTRY( 'U', 'Z', 0 ),             /* UZ Uzbekistan */
    WICED_COUNTRY_VANUATU                                         = MK_CNTRY( 'V', 'U', 0 ),             /* VU Vanuatu */
    WICED_COUNTRY_VENEZUELA                                       = MK_CNTRY( 'V', 'E', 0 ),             /* VE Venezuela */
    WICED_COUNTRY_VIET_NAM                                        = MK_CNTRY( 'V', 'N', 0 ),             /* VN Viet_Nam */
    WICED_COUNTRY_VIRGIN_ISLANDS_BRITISH                          = MK_CNTRY( 'V', 'G', 0 ),             /* VG Virgin_Islands,_British */
    WICED_COUNTRY_VIRGIN_ISLANDS_US                               = MK_CNTRY( 'V', 'I', 0 ),             /* VI Virgin_Islands,_U.S. */
    WICED_COUNTRY_WALLIS_AND_FUTUNA                               = MK_CNTRY( 'W', 'F', 0 ),             /* WF Wallis_and_Futuna */
    WICED_COUNTRY_WEST_BANK                                       = MK_CNTRY( '0', 'C', 0 ),             /* 0C West_Bank */
    WICED_COUNTRY_WESTERN_SAHARA                                  = MK_CNTRY( 'E', 'H', 0 ),             /* EH Western_Sahara */
    WICED_COUNTRY_WORLD_WIDE_XV_REV983                            = MK_CNTRY( 'X', 'V', 983 ),           /* Worldwide Locale Revision 983 */
    WICED_COUNTRY_WORLD_WIDE_XX                                   = MK_CNTRY( 'X', 'X', 0 ),             /* Worldwide Locale (passive Ch12-14) */
    WICED_COUNTRY_WORLD_WIDE_XX_REV17                             = MK_CNTRY( 'X', 'X', 17 ),            /* Worldwide Locale (passive Ch12-14) Revision 17 */
    WICED_COUNTRY_YEMEN                                           = MK_CNTRY( 'Y', 'E', 0 ),             /* YE Yemen */
    WICED_COUNTRY_ZAMBIA                                          = MK_CNTRY( 'Z', 'M', 0 ),             /* ZM Zambia */
    WICED_COUNTRY_ZIMBABWE                                        = MK_CNTRY( 'Z', 'W', 0 ),             /* ZW Zimbabwe */
} wiced_country_code_t;

/**
 * Enumerated list of aggregate codes and edit WICED_COUNTRY_AGGREGATE_CUSTOMER for supporting new aggregate
 * as per customer like XZ/278
 */
typedef enum
{
    WICED_COUNTRY_AGGREGATE_XA_0                                   = MK_CNTRY( 'X', 'A', 0 ),
    WICED_COUNTRY_AGGREGATE_XT_0                                   = MK_CNTRY( 'X', 'T', 0 ),
    WICED_COUNTRY_AGGREGATE_XV_0                                   = MK_CNTRY( 'X', 'V', 0 ),
    WICED_COUNTRY_AGGREGATE_CUSTOMER                               = MK_CNTRY( 'X', 'Z', 278 ),
} wiced_aggregate_code_t;

/* WICED Radio Resource Management Report Types */
typedef enum
{
   WICED_RRM_CHLOAD_REPORT = 0,
   WICED_RRM_NBR_LIST_REPORT,
   WICED_RRM_BCN_REPORT,
   WICED_LM_REPORT
} wwd_rrm_report_type_t;

/** @endcond */

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* ifndef INCLUDED_WWD_CONSTANTS_H_ */
