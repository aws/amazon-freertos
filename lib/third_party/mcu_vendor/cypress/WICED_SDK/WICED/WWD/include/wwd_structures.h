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
 *  Defines common structures used with WWD
 *
 */

#ifndef INCLUDED_WWD_STRUCTURES_H
#define INCLUDED_WWD_STRUCTURES_H

#include "wwd_constants.h"
#include "wwd_wlioctl.h"


#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#define SSID_NAME_SIZE        (32)
#define HOSTNAME_SIZE         (32)

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/
/*   wl_nan_sub_cmd may also be used in dhd  */
typedef struct wwd_nan_sub_cmd wwd_nan_sub_cmd_t;
typedef int (nan_cmd_handler_t)(void *wl, const wwd_nan_sub_cmd_t *cmd, int argc, char **argv,
        wiced_bool_t *is_set, uint8_t *iov_data, uint16_t *avail_len);

/** @cond !ADDTHIS*/

typedef volatile void*    host_semaphore_pointer_t;
typedef volatile void*    host_mutex_pointer_t;
typedef volatile void*    host_thread_pointer_t;

typedef uint32_t          wwd_time_t;          /* Time value in milliseconds */
typedef struct wl_bss_info_struct     wiced_bss_info_t;

typedef struct edcf_acparam    wiced_edcf_ac_param_t;
typedef struct wl_action_frame wiced_action_frame_t;

typedef struct wiced_event_header_struct wwd_event_header_t;
/** @endcond */

/******************************************************
 *                    Structures
 ******************************************************/


/*
    The received packet formats are different when EXT_STA is enabled. In case
    of EXT_STA the received packets are in 802.11 format, where as in other
    case the received packets have Ethernet II format

    1. 802.11 frames
    ----------------------------------------------------------------------------
    | FC (2) | DID (2) |A1 (6) |A2 (6)|A3 (6) |SID (2) |SNAP (6) |type (2) |data (46 - 1500) |
    ----------------------------------------------------------------------------

    2. Ethernet II frames
    -------------------------------------------------
    | DA (6) | SA (6) | type (2) | data (46 - 1500) |
    -------------------------------------------------
    */


/**
 * Structure describing a packet filter list item
*/
typedef struct
{
    uint32_t                       id;            /**< Unique identifier for a packet filter item                             */
    wiced_packet_filter_rule_t     rule;          /**< Filter matches are either POSITIVE or NEGATIVE matching */
    uint16_t                       offset;        /**< Offset in bytes to start filtering (referenced to the start of the ethernet packet) */
    uint16_t                       mask_size;     /**< Size of the mask in bytes */
    uint8_t*                       mask;          /**< Pattern mask bytes to be ANDed with the pattern eg. "\xff00" (must be in network byte order) */
    uint8_t*                       pattern;       /**< Pattern bytes used to filter eg. "\x0800"  (must be in network byte order) */
    wiced_bool_t                   enabled_status; /**< When returned from wwd_wifi_get_packet_filters, indicates if the filter is enabled */
} wiced_packet_filter_t;

/** @cond */
typedef struct wl_pkt_filter_stats wiced_packet_filter_stats_t;
/** @endcond */

/**
 * Structure describing a packet filter list item
*/
typedef struct
{
    uint8_t     keep_alive_id;  /**< Unique identifier for the keep alive packet */
    uint32_t    period_msec;    /**< Repeat interval in milliseconds             */
    uint16_t    packet_length;  /**< Length of the keep alive packet             */
    uint8_t*    packet;         /**< Pointer to the keep alive packet            */
} wiced_keep_alive_packet_t;

/**
 * Structure for storing a Service Set Identifier (i.e. Name of Access Point)
 */
typedef struct
{
    uint8_t length;                  /**< SSID length */
    uint8_t value[ SSID_NAME_SIZE ]; /**< SSID name (AP name)  */
} wiced_ssid_t;

/**
 * Structure for storing a MAC address (Wi-Fi Media Access Control address).
 */
typedef struct
{
    uint8_t octet[6]; /**< Unique 6-byte MAC address */
} wiced_mac_t;

/**
 * Structure for storing a null terminated network hostname
 */
typedef struct
{
    char value[ HOSTNAME_SIZE + 1 ];
} wiced_hostname_t;

typedef char wwd_country_t[WWD_CNTRY_BUF_SZ] ;

/**
 * Structure for storing extended scan parameters
 */
typedef struct
{
    int32_t number_of_probes_per_channel;                     /**< Number of probes to send on each channel                                               */
    int32_t scan_active_dwell_time_per_channel_ms;            /**< Period of time to wait on each channel when active scanning                            */
    int32_t scan_passive_dwell_time_per_channel_ms;           /**< Period of time to wait on each channel when passive scanning                           */
    int32_t scan_home_channel_dwell_time_between_channels_ms; /**< Period of time to wait on the home channel when scanning. Only relevant if associated. */
} wiced_scan_extended_params_t;

/**
 * Structure for storing radio band list information
 */
typedef struct
{
    int32_t number_of_bands; /**< Number of bands supported, currently 1 or 2      */
    int32_t current_band;    /**< Current band type : WLC_BAND_2G or WLC_BAND_5G   */
    int32_t other_band;      /**< If number of bands is 2 then the other band type */
} wiced_band_list_t;

/**
 * Structure for storing AP information
 */
#pragma pack(1)
typedef struct wiced_ap_info
{
    wiced_ssid_t              SSID;             /**< Service Set Identification (i.e. Name of Access Point)                    */
    wiced_mac_t               BSSID;            /**< Basic Service Set Identification (i.e. MAC address of Access Point)       */
    int16_t                   signal_strength;  /**< Receive Signal Strength Indication in dBm. <-90=Very poor, >-30=Excellent */
    uint32_t                  max_data_rate;    /**< Maximum data rate in kilobits/s                                           */
    wiced_bss_type_t          bss_type;         /**< Network type                                                              */
    wiced_security_t          security;         /**< Security type                                                             */
    uint8_t                   channel;          /**< Radio channel that the AP beacon was received on                          */
    wiced_802_11_band_t       band;             /**< Radio band                                                                */
    struct wiced_ap_info*     next;             /**< Pointer to the next scan result                                           */
} wiced_ap_info_t;
#pragma pack()

/**
 * Structure for storing scan results
 */
#pragma pack(1)
typedef struct wiced_scan_result
{
    wiced_ssid_t              SSID;             /**< Service Set Identification (i.e. Name of Access Point)                    */
    wiced_mac_t               BSSID;            /**< Basic Service Set Identification (i.e. MAC address of Access Point)       */
    int16_t                   signal_strength;  /**< Receive Signal Strength Indication in dBm. <-90=Very poor, >-30=Excellent */
    uint32_t                  max_data_rate;    /**< Maximum data rate in kilobits/s                                           */
    wiced_bss_type_t          bss_type;         /**< Network type                                                              */
    wiced_security_t          security;         /**< Security type                                                             */
    uint8_t                   channel;          /**< Radio channel that the AP beacon was received on                          */
    wiced_802_11_band_t       band;             /**< Radio band                                                                */
    uint8_t                   ccode[2];         /**< Two letter ISO country code from AP                                       */
    uint8_t                   flags;            /**< flags                                                                     */
    struct wiced_scan_result* next;             /**< Pointer to the next scan result                                           */
} wiced_scan_result_t;
#pragma pack()

/**
 * Structure for storing a WEP key
 */
typedef struct
{
    uint8_t index;    /**< WEP key index [0/1/2/3]                                             */
    uint8_t length;   /**< WEP key length. Either 5 bytes (40-bits) or 13-bytes (104-bits) */
    uint8_t data[32]; /**< WEP key as values NOT chars                                     */
} wiced_wep_key_t;


/** Structure for storing 802.11 powersave listen interval values \n
 * See @ref wiced_wifi_get_listen_interval for more information
 */
typedef struct
{
    uint8_t  beacon;  /**< Listen interval in beacon periods */
    uint8_t  dtim;    /**< Listen interval in DTIM periods   */
    uint16_t assoc;   /**< Listen interval as sent to APs    */
} wiced_listen_interval_t;


#pragma pack(1)

/**
 * Structure describing a list of associated softAP clients
 */
typedef struct
{
    uint32_t    count;         /**< Number of MAC addresses in the list    */
    wiced_mac_t   mac_list[1];   /**< Variable length array of MAC addresses */
} wiced_maclist_t;

typedef struct
{
    char     abbrev[3];
    uint8_t  rev;
    uint8_t  data[64];
} wiced_country_info_t;


/*
 * Structure of a 10Mb/s Ethernet header.
 */
typedef struct
{
    uint8_t  ether_dhost[ETHER_ADDR_LEN];
    uint8_t  ether_shost[ETHER_ADDR_LEN];
    uint16_t ether_type;
} wiced_ether_header_t;

typedef struct
{
    uint8_t   version_ihl;        /* Version and Internet Header Length */
    uint8_t   tos;            /* Type Of Service */
    uint16_t  tot_len;        /* Number of bytes in packet (max 65535) */
    uint16_t  id;
    uint16_t  frag;           /* 3 flag bits and fragment offset */
    uint8_t   ttl;            /* Time To Live */
    uint8_t   prot;           /* Protocol */
    uint16_t  hdr_chksum;     /* IP header checksum */
    uint8_t   src_ip[IPV4_ADDR_LEN];  /* Source IP Address */
    uint8_t   dst_ip[IPV4_ADDR_LEN];  /* Destination IP Address */
} ipv4_hdr_t;

/* These fields are stored in network order */
typedef struct
{
    uint16_t  src_port;   /* Source Port Address */
    uint16_t  dst_port;   /* Destination Port Address */
    uint16_t  len;        /* Number of bytes in datagram including header */
    uint16_t  chksum;     /* entire datagram checksum with pseudoheader */
} bcmudp_hdr_t;

#pragma pack()

/* DSS Parameter Set */
typedef struct
{
    uint8_t type;
    uint8_t length;
    uint8_t current_channel;
} dsss_parameter_set_ie_t;

/* In dongle firmware this is defined as ht_add_ie_t in 802.11.h. It has similar structure but different field names due to history.
 * Wireshark reports this element as the HT information element. */
typedef struct
{
    uint8_t       type;
    uint8_t       length;
    uint8_t       primary_channel;
    uint8_t       ht_operation_subset_1;
    uint16_t      ht_operation_subset_2;
    uint16_t      ht_operation_subset_3;
    uint8_t       rx_supported_mcs_set[16];
} ht_operation_ie_t;


/* 11k Radio resource capabilities bit information */
typedef struct radio_resource_management_capability_debug_msg
{
    uint32_t value;
    const char* string;
} radio_resource_management_capability_debug_msg_t;

/* 11k Radio resource capabilities */
typedef struct
{
    uint8_t radio_resource_management[RRM_CAPABILITIES_LEN];
} radio_resource_management_capability_ie_t;

/* 11k Radio resource beacon request */
typedef struct radio_resource_management_beacon_req
{
    uint8_t       bcn_mode;
    int           duration;
    int           channel;
    wiced_mac_t   da;
    uint16_t      random_int;
    wlc_ssid_t    ssid;
    uint16_t      repetitions;
} radio_resource_management_beacon_req_t;

/* 11k Radio resource management request */
typedef struct radio_resource_management_req
{
    wiced_mac_t   da;
    uint8_t       regulatory;
    uint8_t       channel;
    uint16_t      random_int;
    uint16_t      duration;
    uint16_t      repetitions;
} radio_resource_management_req_t;

/* 11k Radio resource management frame request */
typedef struct radio_resource_management_framereq
{
    wiced_mac_t   da;
    uint8_t       regulatory;
    uint8_t       channel;
    uint16_t      random_int;
    uint16_t      duration;
    wiced_mac_t   ta;
    uint16_t      repetitions;
} radio_resource_management_framereq_t;

/* 11k Radio resource management statistics request */
typedef struct radio_resource_management_statreq
{
    wiced_mac_t   da;
    wiced_mac_t   peer;
    uint16_t      random_int;
    uint16_t      duration;
    uint8_t       group_id;
    uint16_t      repetitions;
} radio_resource_management_statreq_t;

/* 11k Radio resource statistics report */
typedef struct
{
    uint16_t      version;
    wiced_mac_t   sta_address;
    uint32_t      timestamp;
    uint16_t      flag;
    uint16_t      length_of_payload;
    unsigned char data[WL_RRM_RPT_MAX_PAYLOAD];
} radio_resource_management_statrpt_t;

/* 11k Neighbor Report element */
typedef struct rrm_nbr_element
{
    uint8_t       id;
    uint8_t       length;
    wiced_mac_t   bssid;
    uint32_t      bssid_info;
    uint8_t       regulatory;
    uint8_t       channel;
    uint8_t       phytype;
    uint8_t       pad;
} radio_resource_management_nbr_element_t;

typedef struct
{
    uint8_t mode;       /* 1 == target must not tx between recieving CSA and actually switching */
                        /* 0 == target may tx between recieving CSA and actually switching */
    uint8_t count;      /* count number of beacons before switching */
    uint16_t chspec;    /* target chanspec */
} wiced_chan_switch_t;

typedef struct radio_resource_management_neight_report
{
  struct radio_resource_management_neight_report *link;
  radio_resource_management_nbr_element_t nbr_elm;
} radio_resource_management_neighbor_report_t;


typedef struct wwd_rrm_report
{
  wwd_rrm_report_type_t      type;
  uint32_t                   report_len;
  uint8_t                   *report;
} wwd_rrm_report_t;


/*
 * DHCP lease time renew off-load structure definitions
 */

typedef struct
{
    uint8_t addr[WWD_IPV4_ADDR_LEN];
} wwd_ipv4_addr_t;


/* Common iovar structure */
typedef struct
{
     uint8_t subcmd_id;    /* Sub-command id */
     uint8_t pad;
     uint16_t len;         /* Total length of data[] */
     uint8_t data[1];      /* Sub-command data */
} wwd_dltro_t;

/* WL_DLTRO_SUBCMD_CONNECT sub-command data
 * Invoke with unique 'index' for each DLTRO connection
 */
typedef struct
{
     uint8_t  index;                /* DLTRO connection index, 0 to max-1 */
     uint8_t  ip_addr_type;         /* 0 - IPv4, 1 - IPv6 */
     uint8_t  offload_type;         /* 0 - Client, 1 - Server */
     uint8_t  pad;
     uint32_t tid;                  /* Transaction id */
     uint32_t timer_val;            /* DHCP lease time remaining in seconds */
     uint32_t time_before_expiry;   /* Time before expiry for DHCP lease renewal in seconds */
     uint32_t len;                  /* Length of the variable data */
     uint8_t  data[1];              /* Variable length field containing DLTRO packet */
} wwd_dltro_connect_t;

/* WWD_DLTRO_SUBCMD_PARAM sub-command data
 * Invoke with unique 'index' for each DLTRO connection
 */
typedef struct
{
     uint8_t index;        /* DLTRO connection index, 0 to max-1 */
     uint8_t retry;        /* Number of retries */
} wwd_dltro_param_t;

/* WWD_DLTRO_SUBCMD_PARAM sub-command data to GET configured info for specific index */
typedef struct
{
    uint8_t index;        /* DLTRO connection index, 0 to max-1 */
} wwd_dltro_get_param_t;

/* WWD_DLTRO_SUBCMD_MAX_DLTRO sub-command data */
typedef struct
{
     uint8_t max;      /* Max DLTRO supported */
} wwd_dltro_max_dltro_t;

/*********************
 *********************
     XTLV Structures
 *********************
 *******************
 */

/* bcm type(id), length, value with w/16 bit id/len. The structure below
 * is nominal, and is used to support variable length id and type. See
 * xtlv options below.
 */
typedef struct wwd_xtlv {
        uint16_t  id;
        uint16_t  len;
        uint8_t   data[1];
} wwd_xtlv_t;


/*
 * Batched commands will have the following memory layout
 * +--------+---------+-----+-------+
 * |version |count    | pad |sub-cmd|
 * +--------+---------+-----+-------+
 * version >= 0x8000
 * count = number of sub-commands encoded in the iov buf
 * sub-cmd one or more sub-commands for processing
 * Where sub-cmd is padded byte buffer with memory layout as follows
 * +--------+---------+-----------------------+-------------+------
 * |cmd-id  |length   |IN(options) OUT(status)|command data |......
 * +--------+---------+-----------------------+-------------+------
 * cmd-id =sub-command ID
 * length = length of this sub-command
 * IN(options) = On input processing options/flags for this command
 * OUT(status) on output processing status for this command
 * command data = encapsulated IOVAR data as a single structure or packed TLVs for each
 * individual sub-command.
 */
typedef struct bcm_iov_batch_buf {
        uint16_t version;
        uint8_t count;
        uint8_t pad; /* Align sub-commands on 32 bit boundary */
        wwd_xtlv_t cmds[1];
} bcm_iov_batch_buf_t;


typedef struct bcm_iov_batch_subcmd {
        uint16_t id;
        uint16_t len;
        union {
                uint32_t options;
                uint32_t status;
        } u;
        uint8_t data[1];
} bcm_iov_batch_subcmd_t;



/*
 * NAN structures
 */

typedef struct wwd_nan_config_rssi_threshold
{
  uint8_t nan_band;
  int8_t rssi_close;
  int8_t rssi_mid;
  uint8_t pad;
} wwd_nan_config_rssi_threshold_t;

typedef struct wwd_nan_config_count
{
  uint32_t cnt_bcn_tx;              /**< TX discovery/sync beacon count */
  uint32_t cnt_bcn_rx;              /**< RX discovery/sync beacon count */
  uint32_t cnt_svc_disc_tx;         /**< TX service discovery frame count */
  uint32_t cnt_svc_disc_rx;         /**< RX service discovery frame count */
} wwd_nan_config_count_t;


typedef struct wwd_nan_config_params
{
  struct ether_addr cid;            /**< cluster id */
  uint8_t hop_count;                /**< hop count  */
} wwd_nan_config_params_t;

typedef struct wwd_nan_state
{
  uint8_t enabled;                                /**< NAN enabled or not */
  uint8_t inited;                                 /**< NAN status initialized or not */
  uint8_t joined;                                 /**< NAN status joined to cluster or not */
  uint8_t merged;                                 /**< NAN cluster merge */
  uint8_t role;                                   /**< NAN role */
  uint32_t chspec[2];                             /**< Channel Spec */
  uint8_t mr[8];                                  /**< Master Rank */
  uint8_t amr[8];                                 /**< Anchor Master Rank */
  uint32_t cnt_pend_txfrm;                        /**< pending TX frames */
  wwd_nan_config_count_t nan_config_status;       /**< NAN config status */
  uint32_t ambtt;                                 /**< Anchor master beacon target time */
  wwd_nan_config_params_t nan_config_params;      /**< NAN config parameters */
} wwd_nan_state_t;


typedef struct wwd_nan_config_oui_type
{
  uint8_t nan_oui[DOT11_OUI_LEN];
  uint8_t type;
} wwd_nan_config_oui_type_t;


typedef struct ether_addr wwd_nan_cluster_id_t;

typedef struct wwd_nan_sid_beacon_control
{
  uint8_t sid_enable;       /* Flag to indicate the inclusion of Service IDs in Beacons */
  uint8_t sid_count;        /* Limit for number of SIDs to be included in Beacons */
  uint8_t pad[2];
} wwd_nan_sid_beacon_control_t;

/* Set only */
typedef struct wwd_nan_election_metric_config
{
  uint8_t random_factor;    /* Configured random factor */
  uint8_t master_pref;      /* configured master preference */
  uint8_t pad[2];
} wwd_nan_election_metric_config_t;


typedef struct wwd_nan_join
{
  uint8_t start_cluster;    /* Start a cluster */
  uint8_t pad[3];
  wwd_nan_cluster_id_t cluster_id; /* Cluster ID to join */
} wwd_nan_join_t;

typedef struct wwd_nan_timeslot
{
  uint32_t  abitmap; /**< available bitmap */
  uint32_t chanlist[NAN_MAX_TIMESLOT];
} wwd_nan_timeslot_t;

/* type(aka id)/length/value buffer triple */
typedef struct wwd_tlv
{
  uint8_t   id;
  uint8_t   len;
  uint8_t   data[1];
} wwd_tlv_t;

typedef struct wwd_nan_sd_publish
{
  uint16_t  length;    /* length including options */
  uint16_t  flags;     /* bitmap representing aforesaid optional flags */
  uint8_t   svc_hash[WL_NAN_SVC_HASH_LEN]; /* Hash for the service name */
  uint8_t   instance_id; /* Instance of the current service */
  int8_t    proximity_rssi; /* RSSI limit to Rx subscribe or pub SDF 0 no effect */
  uint8_t   period; /* period of the unsolicited SDF xmission in DWs */
  int32_t   ttl; /* TTL for this instance id, -1 will run till cancelled */
  wwd_tlv_t optional[1]; /* optional fields in the SDF  as appropriate */
} wwd_nan_sd_publish_t, wwd_nan_sd_subscribe_t;

/*
 * WL_NAN_CMD_SD_PUBLISH_LIST
 * WL_NAN_CMD_SD_SUBSCRIBE_LIST
 */
typedef struct wwd_nan_service_info
{
  uint8_t instance_id;      /* Publish instance ID */
  uint8_t service_hash[WL_NAN_SVC_HASH_LEN]; /* Hash for service name */
} wwd_nan_service_info_t;

typedef struct wl_nan_service_list
{
  uint16_t id_count; /* Number of registered publish/subscribe services */
  wwd_nan_service_info_t list[1]; /* service info defined by nan_service instance */
} wwd_nan_service_list_t;

/*
 * WL_NAN_CMD_SD_TRANSMIT
 * WL_NAN_CMD_SD_FUP_TRANSMIT
 */
typedef struct wwd_nan_sd_transmit {
   uint8_t local_service_id;               /* Sender Service ID */
   uint8_t requestor_service_id;           /* Destination Service ID */
   struct ether_addr destination_addr;     /* Destination MAC */
   uint16_t token;                         /* follow_up_token when a follow-up message is queued successfully */
   uint8_t priority;                       /* requested relative priority */
   uint8_t service_info_len;               /* size in bytes of the service info payload */
   wwd_nan_service_info_t service_info[1]; /* Service Info payload */
} wwd_nan_sd_transmit_t;



/*
 * nan sub-commands list entry
 */
struct wwd_nan_sub_cmd {
        char *name;
        uint8_t version;
        uint16_t id;
        uint16_t type;
        nan_cmd_handler_t *handler;
};





/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ifndef INCLUDED_WWD_STRUCTURES_H */
