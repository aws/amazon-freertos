/*
 * Copyright 2014, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file
 *
 * Bluetooth Audio Video Distribution Transport Protocol (AVDTP) Application
 * Programming Interface
 *
 */
#pragma once

#include "wiced_bt_types.h"
#include "bt_target.h"
#include "wwd_constants.h"

/*****************************************************************************
**  Constants
*****************************************************************************/
#define AVDT_VERSION_1_0            0x0100
#define AVDT_VERSION_1_2            0x0102
#define AVDT_VERSION_1_3            0x0103


#ifndef AVDT_VERSION
#define AVDT_VERSION                AVDT_VERSION_1_3
#endif

/* AVDTP version when feature was added */
#define AVDT_VERSION_DELAYREPORT    AVDT_VERSION_1_3        /* Delay Reporting */
#define AVDT_VERSION_CP             AVDT_VERSION_1_2        /* Content Protection */

/**
 * @anchor AVDT_RESULT
 * @name AVDT result codes.
 * @{
 *
 * API function return value result codes. */
#define AVDT_SUCCESS                0       /**< Function successful */
#define AVDT_BAD_PARAMS             1       /**< Invalid parameters */
#define AVDT_NO_RESOURCES           2       /**< Not enough resources */
#define AVDT_BAD_HANDLE             3       /**< Bad handle */
#define AVDT_BUSY                   4       /**< A procedure is already in progress */
#define AVDT_WRITE_FAIL             5       /**< Write failed */
/** @} AVDT_RESULT */

/* The index to access the codec type in codec_info[]. */
#define AVDT_CODEC_TYPE_INDEX       2

/* The size in bytes of a Adaptation Layer header. */
#define AVDT_AL_HDR_SIZE            3

/* The size in bytes of a media packet header. */
#define AVDT_MEDIA_HDR_SIZE         12

/* The size, in byte, of the SCMS Header */
#define AVDT_MEDIA_CP_HDR_SIZE      1

/* AVDTP 7.5.3 Adaptation Layer Fragmentation
 * original length of the un-fragmented transport packet should be specified by
 * two bytes length field of Adaptation Layer Header  */
#define AVDT_MAX_MEDIA_SIZE         (0xFFFF - AVDT_MEDIA_HDR_SIZE)

/* The handle is used when reporting MULTI_AV specific events */
#define AVDT_MULTI_AV_HANDLE        0xFF

/* The number of bytes needed by the protocol stack for the protocol headers
** of a media packet.  This is the size of the media packet header, the
** L2CAP packet header and HCI header.
*/
#define AVDT_MEDIA_OFFSET           23

/* The marker bit is used by the application to mark significant events such
** as frame boundaries in the data stream.  This constant is used to check or
** set the marker bit in the m_pt parameter of an wiced_bt_avdt_write_req()
** or AVDT_DATA_IND_EVT.
*/
#define AVDT_MARKER_SET             0x80

/**
 * @anchor AVDT_SEP_TYPE
 * @name AVDT SEP types.
 * @{
 *
 * Stream endpoint type. */
#define AVDT_TSEP_SRC               0       /**< Source */
#define AVDT_TSEP_SNK               1       /**< Sink */
/** @} AVDT_SEP_TYPE */

/* initiator/acceptor role for adaption */
#define AVDT_INT                    0       /* initiator */
#define AVDT_ACP                    1       /* acceptor */

/**
 * @anchor AVDT_MEDIA
 * @name Media types
 * @{
 *
 * Media types
 */
#define AVDT_MEDIA_AUDIO            0       /* Audio SEP */
#define AVDT_MEDIA_VIDEO            1       /* Video SEP */
#define AVDT_MEDIA_MULTI            2       /* Multimedia SEP */
/** @} AVDT_MEDIA */

/**
 * @anchor AVDT_REPORT_TYPE
 * @name AVDT report types.
 * @{
 *
 * for reporting packets */
#define AVDT_RTCP_PT_SR             200     /**< the packet type - SR (Sender Report) */
#define AVDT_RTCP_PT_RR             201     /**< the packet type - RR (Receiver Report) */
#define AVDT_RTCP_PT_SDES           202     /**< the packet type - SDES (Source Description) */
typedef uint8_t AVDT_REPORT_TYPE;
/** @} AVDT_REPORT_TYPE */


#define AVDT_RTCP_SDES_CNAME        1       /* SDES item CNAME */
#ifndef AVDT_MAX_CNAME_SIZE
#define AVDT_MAX_CNAME_SIZE         28
#endif


/**
 * @anchor AVDT_PSC
 * @name Protocol service capabilities
 * @{
 *
 * Protocol service capabilities
 */
#define AVDT_PSC_TRANS              (1<<1)  /**< Media transport */
#define AVDT_PSC_REPORT             (1<<2)  /**< Reporting */
#define AVDT_PSC_RECOV              (1<<3)  /**< Recovery */
#define AVDT_PSC_CP                 (1<<4)  /**< Content protection */
#define AVDT_PSC_HDRCMP             (1<<5)  /**< Header compression */
#define AVDT_PSC_MUX                (1<<6)  /**< Multiplexing */
#define AVDT_PSC_CODEC              (1<<7)  /**< Codec */
#define AVDT_PSC_DELAY_RPT          (1<<8)  /**< Delay Report */
/** @} AVDT_PSC */

/**
 * @anchor AVDT_RECOV
 * @name Recovery type
 * @{
 *
 * Recovery type
 */
#define AVDT_RECOV_RFC2733          1       /**< RFC2733 recovery */
/** @} AVDT_RECOV */

/**
 * @anchor AVDT_HDRCMP
 * @name Header compression capabilities mask
 * @{
 *
 * Header compression capabilities mask
 */
#define AVDT_HDRCMP_MEDIA           (1<<5)  /**< Available for media packets */
#define AVDT_HDRCMP_RECOV           (1<<6)  /**< Available for recovery packets */
#define AVDT_HDRCMP_BACKCH          (1<<7)  /**< Back channel supported */
/** @} AVDT_HDRCMP */

/**
 * @anchor AVDT_MUX
 * @name Multiplexing capabilities mask
 * @{
 *
 * Multiplexing capabilities mask
 */
#define AVDT_MUX_FRAG               (1<<7)  /**< Allow Adaptation Layer Fragmentation */
/** @} AVDT_MUX */

/**
 * @anchor AVDT_ASC
 * @name AVDT application service category
 * @{
 *
 * Application service category. This indicates the application
 * service category.
 *
 */
#define AVDT_ASC_PROTECT            4       /**< Content protection */
#define AVDT_ASC_CODEC              7       /**< Codec */
/** @} AVDT_ASC */

/**
 * @anchor AVDT_ERR
 * @name AVDT Error codes
 * @{
 *
 * The following are error codes defined in the AVDTP and GAVDP
 * specifications.  These error codes communicate protocol errors between
 * AVDTP and the application.  More detailed descriptions of the error codes
 * and their appropriate use can be found in the AVDTP and GAVDP specifications.
 * These error codes are unrelated to the result values returned by the
 * AVDTP API functions.
 *
 */
#define AVDT_ERR_HEADER             0x01    /**< Bad packet header format */
#define AVDT_ERR_LENGTH             0x11    /**< Bad packet length */
#define AVDT_ERR_SEID               0x12    /**< Invalid SEID */
#define AVDT_ERR_IN_USE             0x13    /**< The SEP is in use */
#define AVDT_ERR_NOT_IN_USE         0x14    /**< The SEP is not in use */
#define AVDT_ERR_CATEGORY           0x17    /**< Bad service category */
#define AVDT_ERR_PAYLOAD            0x18    /**< Bad payload format */
#define AVDT_ERR_NSC                0x19    /**< Requested command not supported */
#define AVDT_ERR_INVALID_CAP        0x1A    /**< Reconfigure attempted invalid capabilities */
#define AVDT_ERR_RECOV_TYPE         0x22    /**< Requested recovery type not defined */
#define AVDT_ERR_MEDIA_TRANS        0x23    /**< Media transport capability not correct */
#define AVDT_ERR_RECOV_FMT          0x25    /**< Recovery service capability not correct */
#define AVDT_ERR_ROHC_FMT           0x26    /**< Header compression service capability not correct */
#define AVDT_ERR_CP_FMT             0x27    /**< Content protection service capability not correct */
#define AVDT_ERR_MUX_FMT            0x28    /**< Multiplexing service capability not correct */
#define AVDT_ERR_UNSUP_CFG          0x29    /**< Configuration not supported */
#define AVDT_ERR_BAD_STATE          0x31    /**< Message cannot be processed in this state */
#define AVDT_ERR_REPORT_FMT         0x65    /**< Report service capability not correct */
#define AVDT_ERR_SERVICE            0x80    /**< Invalid service category */
#define AVDT_ERR_RESOURCE           0x81    /**< Insufficient resources */
#define AVDT_ERR_INVALID_MCT        0xC1    /**< Invalid Media Codec Type */
#define AVDT_ERR_UNSUP_MCT          0xC2    /**< Unsupported Media Codec Type */
#define AVDT_ERR_INVALID_LEVEL      0xC3    /**< Invalid Level */
#define AVDT_ERR_UNSUP_LEVEL        0xC4    /**< Unsupported Level */
#define AVDT_ERR_INVALID_CP         0xE0    /**< Invalid Content Protection Type */
#define AVDT_ERR_INVALID_FORMAT     0xE1    /**< Invalid Content Protection format */

/* Additional error codes.  This indicates error codes used by AVDTP
** in addition to the ones defined in the specifications.
*/
#define AVDT_ERR_CONNECT            0x07    /**< Connection failed. */
#define AVDT_ERR_TIMEOUT            0x08    /**< Response timeout. */
/** @} AVDT_ERR */

/**
 * @anchor AVDT_EVT
 * @name AVDT Events
 * @{
 */
#define AVDT_DISCOVER_CFM_EVT       0       /**< Discover confirm */
#define AVDT_GETCAP_CFM_EVT         1       /**< Get capabilities confirm */
#define AVDT_OPEN_CFM_EVT           2       /**< Open confirm */
#define AVDT_OPEN_IND_EVT           3       /**< Open indication */
#define AVDT_CONFIG_IND_EVT         4       /**< Configuration indication */
#define AVDT_START_CFM_EVT          5       /**< Start confirm */
#define AVDT_START_IND_EVT          6       /**< Start indication */
#define AVDT_SUSPEND_CFM_EVT        7       /**< Suspend confirm */
#define AVDT_SUSPEND_IND_EVT        8       /**< Suspend indication */
#define AVDT_CLOSE_CFM_EVT          9       /**< Close confirm */
#define AVDT_CLOSE_IND_EVT          10      /**< Close indication */
#define AVDT_RECONFIG_CFM_EVT       11      /**< Reconfiguration confirm */
#define AVDT_RECONFIG_IND_EVT       12      /**< Reconfiguration indication */
#define AVDT_SECURITY_CFM_EVT       13      /**< Security confirm */
#define AVDT_SECURITY_IND_EVT       14      /**< Security indication */
#define AVDT_WRITE_CFM_EVT          15      /**< Write confirm */
#define AVDT_CONNECT_IND_EVT        16      /**< Signaling channel connected */
#define AVDT_DISCONNECT_IND_EVT     17      /**< Signaling channel disconnected */
#define AVDT_REPORT_CONN_EVT        18      /**< Reporting channel connected */
#define AVDT_REPORT_DISCONN_EVT     19      /**< Reporting channel disconnected */
#define AVDT_DELAY_REPORT_EVT       20      /**< Delay report received */
#define AVDT_DELAY_REPORT_CFM_EVT   21      /**< Delay report response received */
/** @} AVDT_EVT */

#define AVDT_MAX_EVT                (AVDT_DELAY_REPORT_CFM_EVT)

/* PSM for AVDT */
#define AVDT_PSM                    0x0019

/**
 * @anchor AVDT_NSC
 * @name Non-supported commands
 * @{
 *
 * Non-supported protocol command messages (used in wiced_bt_avdt_cs_t)
 */
#define AVDT_NSC_SUSPEND            0x01    /* Suspend command not supported */
#define AVDT_NSC_RECONFIG           0x02    /* Reconfigure command not supported */
#define AVDT_NSC_SECURITY           0x04    /* Security command not supported */
/** @} AVDT_NSC */

/*****************************************************************************
**  Type Definitions
*****************************************************************************/

typedef struct
{
    uint32_t  ntp_sec;        /* NTP time: seconds relative to 0h UTC on 1 January 1900 */
    uint32_t  ntp_frac;       /* NTP time: the fractional part */
    uint32_t  rtp_time;       /* timestamp in RTP header */
    uint32_t  pkt_count;      /* sender's packet count: since starting transmission
                               * up until the time this SR packet was generated. */
    uint32_t  octet_count;    /* sender's octet count: same comment */
} wiced_bt_avdt_sender_info_t;

typedef struct
{
    uint8_t   frag_lost;      /* fraction lost since last RR */
    uint32_t  packet_lost;    /* cumulative number of packets lost since the beginning */
    uint32_t  seq_num_rcvd;   /* extended highest sequence number received */
    uint32_t  jitter;         /* interarrival jitter */
    uint32_t  lsr;            /* last SR timestamp */
    uint32_t  dlsr;           /* delay since last SR */
} wiced_bt_avdt_report_blk_t;

typedef union
{
    wiced_bt_avdt_sender_info_t     sr;
    wiced_bt_avdt_report_blk_t      rr;
    uint8_t                         cname[AVDT_MAX_CNAME_SIZE + 1];
} wiced_bt_avdt_report_data_t;

/** AVDT subsytem configuration */
typedef struct {
    uint16_t        ctrl_mtu;                       /**< L2CAP MTU of the AVDTP signaling channel */
    uint8_t         ret_tout;                       /**< AVDTP signaling retransmission timeout */
    uint8_t         sig_tout;                       /**< AVDTP signaling message timeout */
    uint8_t         idle_tout;                      /**< AVDTP idle signaling channel timeout */
    uint8_t         sec_mask;                       /**< Security mask (not used on WICED platforms: security is configured using wiced_bt_cfg) */
} wiced_bt_avdt_reg_t;

/** Stream endpoint information */
typedef struct {
    wiced_bool_t    in_use;                         /**< TRUE if stream is currently in use */
    uint8_t         seid;                           /**< Stream endpoint identifier */
    uint8_t         media_type;                     /**< Media type (see @ref AVDT_MEDIA "Media types") */
    uint8_t         tsep;                           /**< SEP type (see @ref AVDT_SEP_TYPE "Stream endpoint types") */
} wiced_bt_avdt_sep_info_t;

/** Stream endpoint configuration */
typedef struct {
    uint8_t     codec_info[AVDT_CODEC_SIZE];        /**< Codec capabilities array (dependent on coded type; for SBC,
                                                            use wiced_bt_a2d_bld_sbc_info or wiced_bt_a2d_pars_sbc_info to build/parse codec_info)  */
    uint8_t     protect_info[AVDT_PROTECT_SIZE];    /**< Content protection capabilities */
    uint8_t     num_codec;                          /**< Number of media codec information elements */
    uint8_t     num_protect;                        /**< Number of content protection information elements */
    uint16_t    psc_mask;                           /**< Protocol service capabilities mask (see @ref AVDT_PSC "Protocol sevice capabilities") */
    uint8_t     recov_type;                         /**< Recovery type (see @ref AVDT_RECOV "Recovery types") */
    uint8_t     recov_mrws;                         /**< Maximum recovery window size */
    uint8_t     recov_mnmp;                         /**< Recovery maximum number of media packets */
    uint8_t     hdrcmp_mask;                        /**< Header compression capabilities mask (see @ref AVDT_HDRCMP "Header compression capabilities") */
#if AVDT_MULTIPLEXING == TRUE
    uint8_t     mux_mask;                           /**< Multiplexing capabilities. AVDT_MUX_XXX bits can be combined with a bitwise OR */
    uint8_t     mux_tsid_media;                     /**< TSID for media transport session */
    uint8_t     mux_tcid_media;                     /**< TCID for media transport session */
    uint8_t     mux_tsid_report;                    /**< TSID for reporting transport session */
    uint8_t     mux_tcid_report;                    /**< TCID for reporting transport session */
    uint8_t     mux_tsid_recov;                     /**< TSID for recovery transport session */
    uint8_t     mux_tcid_recov;                     /**< TCID for recovery transport session */
#endif
} wiced_bt_avdt_cfg_t;

/** Header for AVDT event callback data */
typedef struct {
    uint8_t     err_code;                           /**< Zero if operation succeeded; nonzero if operation failed */
    uint8_t     err_param;                          /**< Error parameter included for some events */
    uint8_t     label;                              /**< Transaction label */
    uint8_t     seid;                               /**< For internal use only */
    uint8_t     sig_id;                             /**< For internal use only */
    uint8_t     ccb_idx;                            /**< For internal use only */
} wiced_bt_avdt_evt_hdr_t;

/** Data for AVDT_GETCAP_CFM_EVT, AVDT_RECONFIG_IND_EVT, and AVDT_RECONFIG_CFM_EVT */
typedef struct {
    wiced_bt_avdt_evt_hdr_t     hdr;                /**< Event header */
    wiced_bt_avdt_cfg_t         *p_cfg;             /**< Pointer to configuration for this SEP */
} wiced_bt_avdt_config_t;

/** Data for AVDT_CONFIG_IND_EVT */
typedef struct {
    wiced_bt_avdt_evt_hdr_t     hdr;                /**< Event header */
    wiced_bt_avdt_cfg_t         *p_cfg;             /**< Pointer to configuration for this SEP */
    uint8_t                     int_seid;           /**< Stream endpoint ID of stream initiating the operation */
} wiced_bt_avdt_setconfig_t;

/** This data structure is associated with the AVDT_OPEN_IND_EVT and AVDT_OPEN_CFM_EVT */
typedef struct {
    wiced_bt_avdt_evt_hdr_t     hdr;                /**< Event header */
    uint16_t                    peer_mtu;           /**< Transport channel L2CAP MTU of the peer */
    uint16_t                    lcid;               /**< L2CAP LCID for media channel */
} wiced_bt_avdt_open_t;

/** Data for AVDT_SECURITY_IND_EVT and AVDT_SECURITY_CFM_EVT */
typedef struct {
    wiced_bt_avdt_evt_hdr_t     hdr;                /**< Event header */
    uint8_t                     *p_data;            /**< Pointer to security data */
    uint16_t                    len;                /**< Length in bytes of the security data */
} wiced_bt_avdt_security_t;

/** Data for AVDT_DISCOVER_CFM_EVT */
typedef struct {
    wiced_bt_avdt_evt_hdr_t     hdr;                /**< Event header */
    wiced_bt_avdt_sep_info_t    *p_sep_info;        /**< Pointer to SEP information */
    uint8_t                     num_seps;           /**< Number of stream endpoints */
} wiced_bt_avdt_discover_t;

/** Data for AVDT_DELAY_REPORT_EVT */
typedef struct {
    wiced_bt_avdt_evt_hdr_t     hdr;                /**< Event header */
    uint16_t                    delay;              /**< Delay value */
} wiced_bt_avdt_delay_rpt_t;

/** Data for AVDT event notifications */
typedef union {
    wiced_bt_avdt_evt_hdr_t       hdr;              /**< Generic event data */
    wiced_bt_avdt_discover_t      discover_cfm;     /**< Discovery confirm (AVDT_DISCOVER_CFM_EVT) */
    wiced_bt_avdt_config_t        getcap_cfm;       /**< Get Capabilities result (AVDT_GETCAP_CFM_EVT) */
    wiced_bt_avdt_open_t          open_cfm;         /**< Open confirm (AVDT_OPEN_CFM_EVT) */
    wiced_bt_avdt_open_t          open_ind;         /**< Open indication (AVDT_OPEN_IND_EVT) */
    wiced_bt_avdt_setconfig_t     config_ind;       /**< Configuration indication (AVDT_CONFIG_IND_EVT) */
    wiced_bt_avdt_evt_hdr_t       start_cfm;        /**< Start confirm (AVDT_START_CFM_EVT) */
    wiced_bt_avdt_evt_hdr_t       suspend_cfm;      /**< Suspend confirm (AVDT_SUSPEND_CFM_EVT) */
    wiced_bt_avdt_evt_hdr_t       close_cfm;        /**< Close confirm (AVDT_CLOSE_CFM_EVT) */
    wiced_bt_avdt_config_t        reconfig_cfm;     /**< Reconfiguration confirm (AVDT_RECONFIG_CFM_EVT) */
    wiced_bt_avdt_config_t        reconfig_ind;     /**< Reconfiguration indication (AVDT_RECONFIG_IND_EVT) */
    wiced_bt_avdt_security_t      security_cfm;     /**< Security confirm (AVDT_SECURITY_CFM_EVT) */
    wiced_bt_avdt_security_t      security_ind;     /**< Security indication (AVDT_SECURITY_IND_EVT) */
    wiced_bt_avdt_evt_hdr_t       connect_ind;      /**< Connect Indication (AVDT_CONNECT_IND_EVT) */
    wiced_bt_avdt_evt_hdr_t       disconnect_ind;   /**< Disconnect Indication (AVDT_DISCONNECT_IND_EVT) */
    wiced_bt_avdt_evt_hdr_t       report_conn;      /**< Reporting channel connected (AVDT_REPORT_CONN_EVT) */
    wiced_bt_avdt_delay_rpt_t     delay_rpt_cmd;    /**< Delay report received (AVDT_DELAY_REPORT_EVT) */
} wiced_bt_avdt_ctrl_t;

/**
 * Function         wiced_bt_avdt_ctrl_cback_t
 *
 *                  AVDT control callback
 *
 * @note            This function passes control events
 *                  to the application.  This function is required for all registered stream
 *                  endpoints and for the AVDT_DiscoverReq() and AVDT_GetCapReq() functions.
 *
 * @param[in]       handle  : AVDT connection handle
 * @param[in]       bd_addr : Peer address
 * @param[in]       event   : AVDT event (see @ref AVDT_EVT "AVDT events")
 * @param[in]       p_data  : AVDT event data
 *
 * @return          Nothing
 */
typedef void (wiced_bt_avdt_ctrl_cback_t)(uint8_t handle, wiced_bt_device_address_t bd_addr, uint8_t event,
                                          wiced_bt_avdt_ctrl_t *p_data);

/**
 * Function         wiced_bt_avdt_data_cback_t
 *
 *                  AVDT data callback
 *
 * @note            It is executed when AVDTP has a media
 *                  packet ready for the application.  This function is required for SNK
 *                  endpoints and not applicable for SRC endpoints.
 *
 *
 * @param[in]       handle      : AVDT connection handle
 * @param[in]       p_pkt       : Pointer to the data packet
 * @param[in]       time_stamp  : Data packet time stamp
 * @param[in]       m_pt        : Marker and payload type byte
 *
 * @return          Nothing
 */
typedef void (wiced_bt_avdt_data_cback_t)(uint8_t handle, BT_HDR *p_pkt, uint32_t time_stamp,
                                          uint8_t m_pt);

#if AVDT_MULTIPLEXING == TRUE

/** Function         wiced_bt_avdt_media_cback_t
 *
 *                  AVDT media callback
 *
 * @note            This is the second version of the data callback function. This version uses
 *                  application buffer assigned by wiced_bt_avdt_set_media_buf. Caller can assign different
 *                  buffer during callback or can leave the current buffer for further using.
 *                  This callback is called when AVDTP has a media packet ready for the application.
 *                  This function is required for SNK endpoints and not applicable for SRC endpoints.
 *
 * @param[in]       handle      : AVDT connection handle
 * @param[in]       p_payload   : Pointer to the payload
 * @param[in]       payload_len : Payload length
 * @param[in]       time_stamp  : Time stamp
 * @param[in]       seq_num     : Sequence number
 * @param[in]       m_pt        : Marker and payload type byte
 * @param[in]       marker      : Marker byte
 *
 * @return          Nothing
 */
typedef void (wiced_bt_avdt_media_cback_t)(uint8_t handle, uint8_t *p_payload, uint32_t payload_len,
                                uint32_t time_stamp, uint16_t seq_num, uint8_t m_pt, uint8_t marker);
#endif

#if AVDT_REPORTING == TRUE
/* This is the report callback function.
**
**
*/
/** Function         wiced_bt_avdt_report_cback_t
 *
 *                  AVDT report callback
 *
 * @note            It is executed when AVDTP has a reporting
 *                  packet ready for the application.  This function is required for streams
 *                  created with AVDT_PSC_REPORT.
 *
 * @param[in]       handle : AVDT connection handle
 * @param[in]       type   : Report type (see @ref AVDT_REPORT_TYPE "AVDT report types")
 * @param[in]       p_data : Pointer to the report data
 *
 * @return          Nothing
 */
typedef void (wiced_bt_avdt_report_cback_t)(uint8_t handle, AVDT_REPORT_TYPE type,
                                wiced_bt_avdt_report_data_t *p_data);
#endif

typedef uint16_t (wiced_bt_avdt_getcap_req_t) (wiced_bt_device_address_t bd_addr, uint8_t seid, wiced_bt_avdt_cfg_t *p_cfg, wiced_bt_avdt_ctrl_cback_t *p_cback);

/** This structure contains information required when a stream is created.
** It is passed to the wiced_bt_avdt_create_stream() function.
*/
typedef struct {
    wiced_bt_avdt_cfg_t             cfg;            /**< SEP configuration */
    wiced_bt_avdt_ctrl_cback_t      *p_ctrl_cback;  /**< Control callback function */
    wiced_bt_avdt_data_cback_t      *p_data_cback;  /**< Data callback function */
#if AVDT_MULTIPLEXING == TRUE
    wiced_bt_avdt_media_cback_t     *p_media_cback; /**< Media callback function. It will be called only if p_data_cback is NULL */
#endif
#if AVDT_REPORTING == TRUE
    wiced_bt_avdt_report_cback_t    *p_report_cback;/**< Report callback function. */
#endif
    uint8_t                         tsep;           /**< SEP type (see @ref AVDT_SEP_TYPE "Stream endpoint types") */
    uint8_t                         media_type;     /**< Media type (see @ref AVDT_MEDIA "Media types") */
    uint16_t                        nsc_mask;       /**< Non-supported protocol command messages mask (see @ref AVDT_NSC "Non-supported commands mask") */
} wiced_bt_avdt_cs_t;

/**
 * @anchor AVDT_DATA_OPT
 * @name AVDT data option mask
 * @{
 *
 * AVDT data option mask is used in the write request */
#define AVDT_DATA_OPT_NONE      0x00            /**< No option still add RTP header */
#define AVDT_DATA_OPT_NO_RTP    (0x01 << 0)     /**< Skip adding RTP header */

typedef uint8_t wiced_bt_avdt_data_opt_mask_t;
/** @} AVDT_DATA_OPT */


/*****************************************************************************
**  External Function Declarations
*****************************************************************************/

/**
 *
 * @defgroup   wicedbt_av_protocols Audio/Video-related Transport Protocols
 * @ingroup    wicedbt_av
 *
 * @defgroup   wicedbt_avdtp A/V Distribution Transport Protocol
 * @ingroup    wicedbt_av_protocols
 *
 * @addtogroup  wicedbt_avdtp        Audio / Video Distribution Transport Protocol (AVDTP)
 *
 * This protocol specifies the transport for audio and/or video distribution connections and streaming
 * of audio or video media over the Bluetooth air interface.
 *
 * @{
*/

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Function         wiced_bt_avdt_register
 *
 *                  Initialize AVDTP subsystem and register callback for
 *                  event notification. This function must be called prior
 *                  to calling other AVDT APIs.
 *
 * @param[in]       p_reg     : AVDT registration parameters
 * @param[in]       p_cback   : Callback for AVDT event notification
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_register(wiced_bt_avdt_reg_t *p_reg, wiced_bt_avdt_ctrl_cback_t *p_cback);

/**
 *
 * Function         wiced_bt_avdt_deregister
 *
 *                  Called to deregister use AVDTP protocol.
 *                  Before this function can be called, all streams must be
 *                  removed with wiced_bt_avdt_remove_stream().
 *
 * @param[in]       None
 *
 * @return          None
 *
 */
void wiced_bt_avdt_deregister(void);

/**
 *
 * Function         wiced_bt_avdt_create_stream
 *
 *                  Create a stream endpoint.  After a stream endpoint is
 *                  created an application can initiate a connection between
 *                  this endpoint and an endpoint on a peer device.  In
 *                  addition, a peer device can discover, get the capabilities,
 *                  and connect to this endpoint.
 *
 * @param[out]      p_handle    : Connection handle (valid if AVRC_SUCCESS is returned)
 * @param[in]       p_cs        : Stream configuration
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
*/
uint16_t wiced_bt_avdt_create_stream(uint8_t *p_handle, wiced_bt_avdt_cs_t *p_cs);

/**
 *
 * Function         wiced_bt_avdt_remove_stream
 *
 *                  Remove a stream endpoint.  This function is called when
 *                  the application is no longer using a stream endpoint.
 *                  If this function is called when the endpoint is connected
 *                  the connection is closed and then the stream endpoint
 *                  is removed.
 *
 *
 * @param[in]       handle      : Connection handle
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_remove_stream(uint8_t handle);

/**
 *
 * Function         wiced_bt_avdt_update_stream
 *
 *                  Change all the sink SEPs to available or unavailable
 *
 * @note            This function can only be called if there is no active
 *                  stream connection to the stream type to be updated
 *
 * @param[in]       sep_type    : (see @ref AVDT_SEP_TYPE "AVDT sep type")
 * @param[in]       available   : If true set all SEPs to available,
 *                                false set all SEPs to unavailable
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
*******************************************************************************/
uint16_t wiced_bt_avdt_update_stream(uint8_t sep_type, wiced_bool_t available);


/**
 *
 * Function         wiced_bt_avdt_discover_req
 *
 *                  This function initiates a connection to the AVDTP service
 *                  on the peer device, if not already present, and discovers
 *                  the stream endpoints on the peer device.  (Please note
 *                  that AVDTP discovery is unrelated to SDP discovery).
 *                  This function can be called at any time regardless of whether
 *                  there is an AVDTP connection to the peer device.
 *
 *                  When discovery is complete, an AVDT_DISCOVER_CFM_EVT
 *                  is sent to the application via its callback function.
 *                  The application must not call wiced_bt_avdt_get_cap_req() or
 *                  wiced_bt_avdt_discover_req() again to the same device until
 *                  discovery is complete.
 *
 *                  The memory addressed by sep_info is allocated by the
 *                  application.  This memory is written to by AVDTP as part
 *                  of the discovery procedure.  This memory must remain
 *                  accessible until the application receives the
 *                  AVDT_DISCOVER_CFM_EVT.
 *
 * @param[in]       bd_addr     : Peer bd_addr
 * @param[in]       p_cback     : Callback for discovery results
 * @param[in]       max_seps    : Maximun number of stream end point to discover
 * @param[out]      p_sep_info  : Pointer to the stream end point
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_discover_req(wiced_bt_device_address_t bd_addr, wiced_bt_avdt_sep_info_t *p_sep_info,
                                    uint8_t max_seps, wiced_bt_avdt_ctrl_cback_t *p_cback);


/**
 *
 * Function         wiced_bt_avdt_get_cap_req
 *
 *                  This function initiates a connection to the AVDTP service
 *                  on the peer device, if not already present, and gets the
 *                  capabilities of a stream endpoint on the peer device.
 *                  This function can be called at any time regardless of
 *                  whether there is an AVDTP connection to the peer device.
 *
 *                  When the procedure is complete, an AVDT_GETCAP_CFM_EVT is
 *                  sent to the application via its callback function.  The
 *                  application must not call wiced_bt_avdt_get_cap_req() or
 *                  wiced_bt_avdt_discover_req() again until the procedure is complete.
 *
 *                  The memory pointed to by p_cfg is allocated by the
 *                  application.  This memory is written to by AVDTP as part
 *                  of the get capabilities procedure.  This memory must
 *                  remain accessible until the application receives
 *                  the AVDT_GETCAP_CFM_EVT.
 *
 * @param[in]       bd_addr     : Peer bd_addr
 * @param[in]       seid        : Stream end point ID (From wiced_bt_avdt_discover_req)
 * @param[in]       p_cback     : Callback for results
 * @param[out]      p_cfg       : Pointer to the stream end point configuration
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_get_cap_req(wiced_bt_device_address_t bd_addr, uint8_t seid, wiced_bt_avdt_cfg_t *p_cfg,
                                   wiced_bt_avdt_ctrl_cback_t *p_cback);

/**
 *
 * Function         wiced_bt_avdt_get_all_cap_req
 *
 *                  This function initiates a connection to the AVDTP service
 *                  on the peer device, if not already present, and gets the
 *                  capabilities of a stream endpoint on the peer device.
 *                  This function can be called at any time regardless of
 *                  whether there is an AVDTP connection to the peer device.
 *
 *                  When the procedure is complete, an AVDT_GETCAP_CFM_EVT is
 *                  sent to the application via its callback function.  The
 *                  application must not call wiced_bt_avdt_get_cap_req() or
 *                  wiced_bt_avdt_discover_req() again until the procedure is complete.
 *
 *                  The memory pointed to by p_cfg is allocated by the
 *                  application.  This memory is written to by AVDTP as part
 *                  of the get capabilities procedure.  This memory must
 *                  remain accessible until the application receives
 *                  the AVDT_GETCAP_CFM_EVT.
 *
 * @param[in]       bd_addr     : Peer bd_addr
 * @param[in]       seid        : Stream end point ID (From wiced_bt_avdt_discover_req)
 * @param[in]       p_cback     : Callback for results
 * @param[out]      p_cfg       : Pointer to the stream end point configuration
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_get_all_cap_req(wiced_bt_device_address_t bd_addr, uint8_t seid, wiced_bt_avdt_cfg_t *p_cfg,
                                       wiced_bt_avdt_ctrl_cback_t *p_cback);

/**
 *
 * Function         wiced_bt_avdt_delay_report
 *
 *                  This functions sends a Delay Report to the peer device
 *                  that is associated with a particular SEID.
 *                  This function is called by SNK device.
 *
 * @param[in]       handle      : AVDT connection handle
 * @param[in]       seid        : Stream end point ID
 * @param[in]       delay       : Amount of delay in ms
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_delay_report(uint8_t handle, uint8_t seid, uint16_t delay);

/**
 *
 * Function         wiced_bt_avdt_open_req
 *
 *                  This function initiates a connection to the AVDTP service
 *                  on the peer device, if not already present, and connects
 *                  to a stream endpoint on a peer device.  When the connection
 *                  is completed, an AVDT_OPEN_CFM_EVT is sent to the
 *                  application via the control callback function for this handle.
 *
 * @param[in]       handle      : AVDT connection handle
 * @param[in]       bd_addr     : Peer bd_addr
 * @param[in]       seid        : Stream end point ID
 * @param[in]       p_cfg       : Pointer to the stream end point configuration
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_open_req(uint8_t handle, wiced_bt_device_address_t bd_addr, uint8_t seid,
                                wiced_bt_avdt_cfg_t *p_cfg);


/**
 *
 * Function         wiced_bt_avdt_config_rsp
 *
 *                  Respond to a configure request from the peer device.  This
 *                  function must be called if the application receives an
 *                  AVDT_CONFIG_IND_EVT through its control callback.
 *
 *
 * @param[in]       handle      : AVDT connection handle
 * @param[in]       label       : Transaction label
 * @param[in]       error_code  : Error code (see @ref AVDT_ERR "AVDT error codes")
 * @param[in]       category    : Service category (see @ref AVDT_ASC "AVDT service categories")
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_config_rsp(uint8_t handle, uint8_t label, uint8_t error_code,
                                  uint8_t category);

/**
 *
 * Function         wiced_bt_avdt_security_set_scms
 *
 *                  Set the SCMS Content Protection. This function must be called
 *                  when the peer device is connected.
 *
 * @param[in]       handle      : AVDT connection handle
 * @param[in]       enable      : If true enable content protection, flase disable
 * @param[in]       scms_hdr    : SCMS content protection header
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_security_set_scms(uint8_t handle, wiced_bool_t enable,
                                         uint8_t scms_hdr);

/**
 *
 * Function         wiced_bt_avdt_start_req
 *
 *                  Start one or more stream endpoints.  This initiates the
 *                  transfer of media packets for the streams.  All stream
 *                  endpoints must previously be opened.  When the streams
 *                  are started, an AVDT_START_CFM_EVT is sent to the
 *                  application via the control callback function for each stream.
 *
 *
 * @param[in]       p_handles   : Pointer to the AVDT connection handles, each byte
 *                                is a connection handle
 * @param[in]       num_handles : Number of connections to start
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_start_req(uint8_t *p_handles, uint8_t num_handles);

/**
 *
 * Function         wiced_bt_avdt_start_resp
 *
 *                  Sends A2DP start response .  This initiates the transfer of media
 *                  packets for the stream. Stream endpoint must previously
 *                  be opened.This function must be called if the application
 *                  receives an AVDT_START_IND_EVT through its control callback.
 *
 *
 * @param[in]       handle   : AVDT connection handle
 * @param[in]       label    : Transaction label
  * @param[in]      status   : Indicates if start request is accepted(AVDT_SUCCESS) or rejected(AVDT Error codes)
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_start_resp( uint8_t handle, uint8_t label, uint8_t status );

/**
 *
 * Function         wiced_bt_avdt_suspend_req
 *
 *                  Suspend one or more stream endpoints. This suspends the
 *                  transfer of media packets for the streams.  All stream
 *                  endpoints must previously be open and started.  When the
 *                  streams are suspended, an AVDT_SUSPEND_CFM_EVT is sent to
 *                  the application via the control callback function for
 *                  each stream.
 *
 *
 * @param[in]       p_handles   : Pointer to the AVDT connection handles, each byte
 *                                is a connection handle
 * @param[in]       num_handles : Number of connections to suspend
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_suspend_req(uint8_t *p_handles, uint8_t num_handles);

/**
 *
 * Function         wiced_bt_avdt_close_req
 *
 * Description      Close a stream endpoint.  This stops the transfer of media
 *                  packets and closes the transport channel associated with
 *                  this stream endpoint.  When the stream is closed, an
 *                  AVDT_CLOSE_CFM_EVT is sent to the application via the
 *                  control callback function for this handle.
 *
 *
 * @param[in]       handle   : AVDT connection handle
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_close_req(uint8_t handle);

/**
 *
 * Function         wiced_bt_avdt_reconfig_req
 *
 *                  Reconfigure a stream endpoint.  This allows the application
 *                  to change the codec or content protection capabilities of
 *                  a stream endpoint after it has been opened.  This function
 *                  can only be called if the stream is opened but not started
 *                  or if the stream has been suspended.  When the procedure
 *                  is completed, an AVDT_RECONFIG_CFM_EVT is sent to the
 *                  application via the control callback function for this handle.
 *
 *
 * @param[in]       handle   : AVDT connection handle
 * @param[in]       p_cfg    : Pointer to the stream end point configuration
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_reconfig_req(uint8_t handle, wiced_bt_avdt_cfg_t *p_cfg);

/**
 *
 * Function         wiced_bt_avdt_reconfig_rsp
 *
 *                  Respond to a reconfigure request from the peer device.
 *                  This function must be called if the application receives
 *                  an AVDT_RECONFIG_IND_EVT through its control callback.
 *
 *
 * @param[in]       handle      : AVDT connection handle
 * @param[in]       label       : Transaction label
 * @param[in]       error_code  : Error code (see @ref AVDT_ERR "AVDT error codes")
 * @param[in]       category    : Service category (see @ref AVDT_ASC "AVDT service categories")
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_reconfig_rsp(uint8_t handle, uint8_t label, uint8_t error_code,
                                    uint8_t category);

/**
 *
 * Function         wiced_bt_avdt_security_req
 *
 *                  Send a security request to the peer device.  When the
 *                  security procedure is completed, an AVDT_SECURITY_CFM_EVT
 *                  is sent to the application via the control callback function
 *                  for this handle.  (Please note that AVDTP security procedures
 *                  are unrelated to Bluetooth link level security.)
 *
 *
 * @param[in]       handle      : AVDT connection handle
 * @param[in]       p_data      : Pointer to the content protection data
 * @param[in]       len         : Length of the data
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_security_req(uint8_t handle, uint8_t *p_data, uint16_t len);

/**
 *
 * Function         wiced_bt_avdt_security_rsp
 *
 *                  Respond to a security request from the peer device.
 *                  This function must be called if the application receives
 *                  an AVDT_SECURITY_IND_EVT through its control callback.
 *                  (Please note that AVDTP security procedures are unrelated
 *                  to Bluetooth link level security.)
 *
 *
 * @param[in]       handle      : AVDT connection handle
 * @param[in]       label       : Transaction label
 * @param[in]       error_code  : Error code (see @ref AVDT_ERR "AVDT error codes")
 * @param[in]       p_data      : Pointer to the content protection data
 * @param[in]       len         : Length of the data
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_security_rsp(uint8_t handle, uint8_t label, uint8_t error_code,
                                    uint8_t *p_data, uint16_t len);


/**
 *
 * Function         wiced_bt_avdt_write_req
 *
 *                  Send a media packet to the peer device.  The stream must
 *                  be started before this function is called.  Also, this
 *                  function can only be called if the stream is a SRC.
 *
 *                  When AVDTP has sent the media packet, an AVDT_WRITE_CFM_EVT is
 *                  sent to the application via the control callback.
 *                  The application can decide whether to wait for the AVDT_WRITE_CFM_EVT,
 *                  before making the next wiced_bt_avdt_write_req() call.
 *                  The application may make its first call to wiced_bt_avdt_write_req()
 *                  after it receives an AVDT_START_CFM_EVT or AVDT_START_IND_EVT.
 *
 *
 *                  The opt parameter allows passing specific options like:
 *                  - NO_RTP : do not add the RTP header to buffer
 *
 * @param[in]       handle      : AVDT connection handle
 * @param[in]       p_media_buf : Pointer to the media buffer to write
 * @param[in]       buf_len     : Size of the buffer
 * @param[in]       time_stamp  : Time stamp
 * @param[in]       m_pt        : Marker and payload byte
 * @param[in]       opt         : Date option mask (see @ref AVDT_DATA_OPT "AVDT data options")
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_write_req(uint8_t handle, uint8_t *p_media_buf, uint16_t buf_len,
                                 uint32_t time_stamp, uint8_t m_pt, wiced_bt_avdt_data_opt_mask_t opt);

/**
 *
 * Function         wiced_bt_avdt_connect_req
 *
 *                  This function initiates an AVDTP signaling connection
 *                  to the peer device.  When the connection is completed, an
 *                  AVDT_CONNECT_IND_EVT is sent to the application via its
 *                  control callback function.  If the connection attempt fails
 *                  an AVDT_DISCONNECT_IND_EVT is sent.  The security mask
 *                  parameter overrides the outgoing security mask set in
 *                  wiced_bt_avdt_register().
 *
 * @param[in]       bd_addr     : Peer bd_addr
 * @param[in]       sec_mask    : Security requirement
 * @param[in]       p_cback     : Callback for event notifications
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_connect_req(wiced_bt_device_address_t bd_addr, uint8_t sec_mask,
                                   wiced_bt_avdt_ctrl_cback_t *p_cback);

/**
 *
 * Function         wiced_bt_avdt_disconnect_req
 *
 *                  This function disconnect an AVDTP signaling connection
 *                  to the peer device.  When disconnected an
 *                  AVDT_DISCONNECT_IND_EVT is sent to the application via its
 *                  control callback function.
 *
 * @param[in]       bd_addr     : Peer bd_addr
 * @param[in]       p_cback     : Callback for event notifications
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_disconnect_req(wiced_bt_device_address_t bd_addr,
                                      wiced_bt_avdt_ctrl_cback_t *p_cback);

/**
 *
 * Function         wiced_bt_avdt_get_l2cap_channel
 *
 *                  Get the L2CAP CID used by the handle.
 *
 * @param[in]       handle  : AVDT connection handle
 *
 * @return          CID if successful, otherwise 0.
 */
uint16_t wiced_bt_avdt_get_l2cap_channel(uint8_t handle);

/**
 *
 * Function         wiced_bt_avdt_get_signal_channel
 *
 *                  Get the L2CAP CID used by the signal channel of the given handle.
 *
 * @param[in]       handle  : AVDT connection handle
 * @param[in]       bd_addr : Peer bd_addr
 *
 * @return          CID if successful, otherwise 0.
 *
 */
uint16_t wiced_bt_avdt_get_signal_channel(uint8_t handle, wiced_bt_device_address_t bd_addr);

/**
 * Function         wiced_bt_avdt_set_media_buf
 *
 * Description      Assigns buffer for media packets or forbids using of assigned
 *                  buffer if argument p_buf is NULL. This function can only
 *                  be called if the stream is a SNK.
 *
 *                  AVDTP uses this buffer to reassemble fragmented media packets.
 *                  When AVDTP receives a complete media packet, it calls the
 *                  p_media_cback assigned by wiced_bt_avdt_create_stream().
 *                  This function can be called during callback to assign a
 *                  different buffer for next media packet or can leave the current
 *                  buffer for next packet.
 *
 * @param[in]       handle      : AVDT connection handle
 * @param[in]       p_buf       : Pointer to the medie buffer
 * @param[in]       buf_len     : Size of the buffer
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 *
 */
uint16_t wiced_bt_avdt_set_media_buf(uint8_t handle, uint8_t *p_buf, uint32_t buf_len);

/**
 * Function         wiced_bt_avdt_send_report
 *
 *                  Sends report packet
 *
 * @param[in]       handle      : AVDT connection handle
 * @param[in]       type        : Report type (see @ref AVDT_REPORT_TYPE "AVDT report types")
 * @param[in]       p_data      : Pointer to the report data
 *
 * @return          Result code (see @ref AVDT_RESULT "AVDT result codes")
 *                  AVDT_SUCCESS if successful, otherwise error.
 */
uint16_t wiced_bt_avdt_send_report(uint8_t handle, AVDT_REPORT_TYPE type,
                                   wiced_bt_avdt_report_data_t *p_data);

/*
 *
 * Function         wiced_bt_avdt_set_trace_level
 *
 *                  Sets the trace level for AVDT. If 0xff is passed, the
 *                  current trace level is returned.
 *
 * @param[in]       new_level: new trace level
 *                             0xff-returns the current setting.
 *                             0-turns off tracing.
 *                             >= 1-Errors.
 *                             >= 2-Warnings.
 *                             >= 3-APIs.
 *                             >= 4-Events.
 *                             >= 5-Debug.
 *
 * @return          The new trace level or current trace level if
 *                  the input parameter is 0xff.
 */
uint8_t wiced_bt_avdt_set_trace_level (uint8_t new_level);

#ifdef __cplusplus
}
#endif

/** @} wicedbt_avdt */
