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
 *  Header for Event detection
 *
 *  Provides constants and prototypes for functions that allow both
 *  Wiced and user applications to receive event callbacks.
 */

#ifndef INCLUDED_WWD_EVENTS_H
#define INCLUDED_WWD_EVENTS_H

#include "wwd_structures.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************
 * @cond       Typedefs
 ******************************************************/

/**
 * Enumerated list of event types
 */
typedef enum
{
    WLC_E_NONE                           =  -1,
    WLC_E_SET_SSID                       =   0  /** indicates status of set SSID */,
    WLC_E_JOIN                           =   1, /** differentiates join IBSS from found (WLC_E_START) IBSS */
    WLC_E_START                          =   2, /** STA founded an IBSS or AP started a BSS */
    WLC_E_AUTH                           =   3, /** 802.11 AUTH request */
    WLC_E_AUTH_IND                       =   4, /** 802.11 AUTH indication */
    WLC_E_DEAUTH                         =   5, /** 802.11 DEAUTH request */
    WLC_E_DEAUTH_IND                     =   6, /** 802.11 DEAUTH indication */
    WLC_E_ASSOC                          =   7, /** 802.11 ASSOC request */
    WLC_E_ASSOC_IND                      =   8, /** 802.11 ASSOC indication */
    WLC_E_REASSOC                        =   9, /** 802.11 REASSOC request */
    WLC_E_REASSOC_IND                    =  10, /** 802.11 REASSOC indication */
    WLC_E_DISASSOC                       =  11, /** 802.11 DISASSOC request */
    WLC_E_DISASSOC_IND                   =  12, /** 802.11 DISASSOC indication */
    WLC_E_QUIET_START                    =  13, /** 802.11h Quiet period started */
    WLC_E_QUIET_END                      =  14, /** 802.11h Quiet period ended */
    WLC_E_BEACON_RX                      =  15, /** BEACONS received/lost indication */
    WLC_E_LINK                           =  16, /** generic link indication */
    WLC_E_MIC_ERROR                      =  17, /** TKIP MIC error occurred */
    WLC_E_NDIS_LINK                      =  18, /** NDIS style link indication */
    WLC_E_ROAM                           =  19, /** roam attempt occurred: indicate status & reason */
    WLC_E_TXFAIL                         =  20, /** change in dot11FailedCount (txfail) */
    WLC_E_PMKID_CACHE                    =  21, /** WPA2 pmkid cache indication */
    WLC_E_RETROGRADE_TSF                 =  22, /** current AP's TSF value went backward */
    WLC_E_PRUNE                          =  23, /** AP was pruned from join list for reason */
    WLC_E_AUTOAUTH                       =  24, /** report AutoAuth table entry match for join attempt */
    WLC_E_EAPOL_MSG                      =  25, /** Event encapsulating an EAPOL message */
    WLC_E_SCAN_COMPLETE                  =  26, /** Scan results are ready or scan was aborted */
    WLC_E_ADDTS_IND                      =  27, /** indicate to host addts fail/success */
    WLC_E_DELTS_IND                      =  28, /** indicate to host delts fail/success */
    WLC_E_BCNSENT_IND                    =  29, /** indicate to host of beacon transmit */
    WLC_E_BCNRX_MSG                      =  30, /** Send the received beacon up to the host */
    WLC_E_BCNLOST_MSG                    =  31, /** indicate to host loss of beacon */
    WLC_E_ROAM_PREP                      =  32, /** before attempting to roam */
    WLC_E_PFN_NET_FOUND                  =  33, /** PFN network found event */
    WLC_E_PFN_NET_LOST                   =  34, /** PFN network lost event */
    WLC_E_RESET_COMPLETE                 =  35,
    WLC_E_JOIN_START                     =  36,
    WLC_E_ROAM_START                     =  37,
    WLC_E_ASSOC_START                    =  38,
    WLC_E_IBSS_ASSOC                     =  39,
    WLC_E_RADIO                          =  40,
    WLC_E_PSM_WATCHDOG                   =  41, /** PSM microcode watchdog fired */
    WLC_E_CCX_ASSOC_START                =  42, /** CCX association start */
    WLC_E_CCX_ASSOC_ABORT                =  43, /** CCX association abort */
    WLC_E_PROBREQ_MSG                    =  44, /** probe request received */
    WLC_E_SCAN_CONFIRM_IND               =  45,
    WLC_E_PSK_SUP                        =  46, /** WPA Handshake */
    WLC_E_COUNTRY_CODE_CHANGED           =  47,
    WLC_E_EXCEEDED_MEDIUM_TIME           =  48, /** WMMAC excedded medium time */
    WLC_E_ICV_ERROR                      =  49, /** WEP ICV error occurred */
    WLC_E_UNICAST_DECODE_ERROR           =  50, /** Unsupported unicast encrypted frame */
    WLC_E_MULTICAST_DECODE_ERROR         =  51, /** Unsupported multicast encrypted frame */
    WLC_E_TRACE                          =  52,
    WLC_E_BTA_HCI_EVENT                  =  53, /** BT-AMP HCI event */
    WLC_E_IF                             =  54, /** I/F change (for wlan host notification) */
    WLC_E_P2P_DISC_LISTEN_COMPLETE       =  55, /** P2P Discovery listen state expires */
    WLC_E_RSSI                           =  56, /** indicate RSSI change based on configured levels */
    WLC_E_PFN_BEST_BATCHING              =  57, /** PFN best network batching event */
    WLC_E_EXTLOG_MSG                     =  58,
    WLC_E_ACTION_FRAME                   =  59, /** Action frame reception */
    WLC_E_ACTION_FRAME_COMPLETE          =  60, /** Action frame Tx complete */
    WLC_E_PRE_ASSOC_IND                  =  61, /** assoc request received */
    WLC_E_PRE_REASSOC_IND                =  62, /** re-assoc request received */
    WLC_E_CHANNEL_ADOPTED                =  63, /** channel adopted (xxx: obsoleted) */
    WLC_E_AP_STARTED                     =  64, /** AP started */
    WLC_E_DFS_AP_STOP                    =  65, /** AP stopped due to DFS */
    WLC_E_DFS_AP_RESUME                  =  66, /** AP resumed due to DFS */
    WLC_E_WAI_STA_EVENT                  =  67, /** WAI stations event */
    WLC_E_WAI_MSG                        =  68, /** event encapsulating an WAI message */
    WLC_E_ESCAN_RESULT                   =  69, /** escan result event */
    WLC_E_ACTION_FRAME_OFF_CHAN_COMPLETE =  70, /** action frame off channel complete */  /* NOTE - This used to be WLC_E_WAKE_EVENT */
    WLC_E_PROBRESP_MSG                   =  71, /** probe response received */
    WLC_E_P2P_PROBREQ_MSG                =  72, /** P2P Probe request received */
    WLC_E_DCS_REQUEST                    =  73,
    WLC_E_FIFO_CREDIT_MAP                =  74, /** credits for D11 FIFOs. [AC0,AC1,AC2,AC3,BC_MC,ATIM] */
    WLC_E_ACTION_FRAME_RX                =  75, /** Received action frame event WITH wl_event_rx_frame_data_t header */
    WLC_E_WAKE_EVENT                     =  76, /** Wake Event timer fired, used for wake WLAN test mode */
    WLC_E_RM_COMPLETE                    =  77, /** Radio measurement complete */
    WLC_E_HTSFSYNC                       =  78, /** Synchronize TSF with the host */
    WLC_E_OVERLAY_REQ                    =  79, /** request an overlay IOCTL/iovar from the host */
    WLC_E_CSA_COMPLETE_IND               =  80,
    WLC_E_EXCESS_PM_WAKE_EVENT           =  81, /** excess PM Wake Event to inform host  */
    WLC_E_PFN_SCAN_NONE                  =  82, /** no PFN networks around */
    WLC_E_PFN_SCAN_ALLGONE               =  83, /** last found PFN network gets lost */
    WLC_E_GTK_PLUMBED                    =  84,
    WLC_E_ASSOC_IND_NDIS                 =  85, /** 802.11 ASSOC indication for NDIS only */
    WLC_E_REASSOC_IND_NDIS               =  86, /** 802.11 REASSOC indication for NDIS only */
    WLC_E_ASSOC_REQ_IE                   =  87,
    WLC_E_ASSOC_RESP_IE                  =  88,
    WLC_E_ASSOC_RECREATED                =  89, /** association recreated on resume */
    WLC_E_ACTION_FRAME_RX_NDIS           =  90, /** rx action frame event for NDIS only */
    WLC_E_AUTH_REQ                       =  91, /** authentication request received */
    WLC_E_TDLS_PEER_EVENT                =  92, /** discovered peer, connected/disconnected peer */
    WLC_E_MESH_DHCP_SUCCESS              =  92, /** DHCP handshake successful for a mesh interface */
    WLC_E_SPEEDY_RECREATE_FAIL           =  93, /** fast assoc recreation failed */
    WLC_E_NATIVE                         =  94, /** port-specific event and payload (e.g. NDIS) */
    WLC_E_PKTDELAY_IND                   =  95, /** event for tx pkt delay suddently jump */
    WLC_E_AWDL_AW                        =  96, /** AWDL AW period starts */
    WLC_E_AWDL_ROLE                      =  97, /** AWDL Master/Slave/NE master role event */
    WLC_E_AWDL_EVENT                     =  98, /** Generic AWDL event */
    WLC_E_NIC_AF_TXS                     =  99, /** NIC AF txstatus */
    WLC_E_NAN                            = 100, /** NAN event */
    WLC_E_BEACON_FRAME_RX                = 101,
    WLC_E_SERVICE_FOUND                  = 102, /** desired service found */
    WLC_E_GAS_FRAGMENT_RX                = 103, /** GAS fragment received */
    WLC_E_GAS_COMPLETE                   = 104, /** GAS sessions all complete */
    WLC_E_P2PO_ADD_DEVICE                = 105, /** New device found by p2p offload */
    WLC_E_P2PO_DEL_DEVICE                = 106, /** device has been removed by p2p offload */
    WLC_E_WNM_STA_SLEEP                  = 107, /** WNM event to notify STA enter sleep mode */
    WLC_E_TXFAIL_THRESH                  = 108, /** Indication of MAC tx failures (exhaustion of 802.11 retries) exceeding threshold(s) */
    WLC_E_PROXD                          = 109, /** Proximity Detection event */
    WLC_E_IBSS_COALESCE                  = 110, /** IBSS Coalescing */
    WLC_E_MESH_PAIRED                    = 110, /** Mesh peer found and paired */
    WLC_E_AWDL_RX_PRB_RESP               = 111, /** AWDL RX Probe response */
    WLC_E_AWDL_RX_ACT_FRAME              = 112, /** AWDL RX Action Frames */
    WLC_E_AWDL_WOWL_NULLPKT              = 113, /** AWDL Wowl nulls */
    WLC_E_AWDL_PHYCAL_STATUS             = 114, /** AWDL Phycal status */
    WLC_E_AWDL_OOB_AF_STATUS             = 115, /** AWDL OOB AF status */
    WLC_E_AWDL_SCAN_STATUS               = 116, /** Interleaved Scan status */
    WLC_E_AWDL_AW_START                  = 117, /** AWDL AW Start */
    WLC_E_AWDL_AW_END                    = 118, /** AWDL AW End */
    WLC_E_AWDL_AW_EXT                    = 119, /** AWDL AW Extensions */
    WLC_E_AWDL_PEER_CACHE_CONTROL        = 120,
    WLC_E_CSA_START_IND                  = 121,
    WLC_E_CSA_DONE_IND                   = 122,
    WLC_E_CSA_FAILURE_IND                = 123,
    WLC_E_CCA_CHAN_QUAL                  = 124, /** CCA based channel quality report */
    WLC_E_BSSID                          = 125, /** to report change in BSSID while roaming */
    WLC_E_TX_STAT_ERROR                  = 126, /** tx error indication */
    WLC_E_BCMC_CREDIT_SUPPORT            = 127, /** credit check for BCMC supported */
    WLC_E_PSTA_PRIMARY_INTF_IND          = 128, /** psta primary interface indication */
    WLC_E_BT_WIFI_HANDOVER_REQ           = 130, /* Handover Request Initiated */
    WLC_E_SPW_TXINHIBIT                  = 131, /* Southpaw TxInhibit notification */
    WLC_E_FBT_AUTH_REQ_IND               = 132, /* FBT Authentication Request Indication */
    WLC_E_RSSI_LQM                       = 133, /* Enhancement addition for WLC_E_RSSI */
    WLC_E_PFN_GSCAN_FULL_RESULT          = 134, /* Full probe/beacon (IEs etc) results */
    WLC_E_PFN_SWC                        = 135, /* Significant change in rssi of bssids being tracked */
    WLC_E_AUTHORIZED                     = 136, /* a STA been authroized for traffic */
    WLC_E_PROBREQ_MSG_RX                 = 137, /* probe req with wl_event_rx_frame_data_t header */
    WLC_E_PFN_SCAN_COMPLETE              = 138, /* PFN completed scan of network list */
    WLC_E_RMC_EVENT                      = 139, /* RMC Event */
    WLC_E_DPSTA_INTF_IND                 = 140, /* DPSTA interface indication */
    WLC_E_RRM                            = 141, /* RRM Event */
    WLC_E_ULP                            = 146, /* ULP entry event */
    WLC_E_LAST                           = 147, /** highest val + 1 for range checking */
    WLC_E_FORCE_32_BIT                   = 0x7FFFFFFE  /** Force enum to be stored in 32 bit variable */
} wwd_event_num_t;


#define WLC_SUP_STATUS_OFFSET      (256)
#define WLC_DOT11_SC_STATUS_OFFSET (512)
/**
 * Enumerated list of event status codes
 * @note : WLC_SUP values overlap other values, so it is necessary
 *         to check the event type
 */
typedef enum
{
    WLC_E_STATUS_SUCCESS                  =  0,  /** operation was successful */
    WLC_E_STATUS_FAIL                     =  1,  /** operation failed */
    WLC_E_STATUS_TIMEOUT                  =  2,  /** operation timed out */
    WLC_E_STATUS_NO_NETWORKS              =  3,  /** failed due to no matching network found */
    WLC_E_STATUS_ABORT                    =  4,  /** operation was aborted */
    WLC_E_STATUS_NO_ACK                   =  5,  /** protocol failure: packet not ack'd */
    WLC_E_STATUS_UNSOLICITED              =  6,  /** AUTH or ASSOC packet was unsolicited */
    WLC_E_STATUS_ATTEMPT                  =  7,  /** attempt to assoc to an auto auth configuration */
    WLC_E_STATUS_PARTIAL                  =  8,  /** scan results are incomplete */
    WLC_E_STATUS_NEWSCAN                  =  9,  /** scan aborted by another scan */
    WLC_E_STATUS_NEWASSOC                 = 10,  /** scan aborted due to assoc in progress */
    WLC_E_STATUS_11HQUIET                 = 11,  /** 802.11h quiet period started */
    WLC_E_STATUS_SUPPRESS                 = 12,  /** user disabled scanning (WLC_SET_SCANSUPPRESS) */
    WLC_E_STATUS_NOCHANS                  = 13,  /** no allowable channels to scan */
    WLC_E_STATUS_CCXFASTRM                = 14,  /** scan aborted due to CCX fast roam */
    WLC_E_STATUS_CS_ABORT                 = 15,  /** abort channel select */

    /* for WLC_SUP messages */
    WLC_SUP_DISCONNECTED                  = 0  + WLC_SUP_STATUS_OFFSET,
    WLC_SUP_CONNECTING                    = 1  + WLC_SUP_STATUS_OFFSET,
    WLC_SUP_IDREQUIRED                    = 2  + WLC_SUP_STATUS_OFFSET,
    WLC_SUP_AUTHENTICATING                = 3  + WLC_SUP_STATUS_OFFSET,
    WLC_SUP_AUTHENTICATED                 = 4  + WLC_SUP_STATUS_OFFSET,
    WLC_SUP_KEYXCHANGE                    = 5  + WLC_SUP_STATUS_OFFSET,
    WLC_SUP_KEYED                         = 6  + WLC_SUP_STATUS_OFFSET,
    WLC_SUP_TIMEOUT                       = 7  + WLC_SUP_STATUS_OFFSET,
    WLC_SUP_LAST_BASIC_STATE              = 8  + WLC_SUP_STATUS_OFFSET,
    /* Extended supplicant authentication states */
    WLC_SUP_KEYXCHANGE_WAIT_M1            = WLC_SUP_AUTHENTICATED,      /** Waiting   to receive handshake msg M1 */
    WLC_SUP_KEYXCHANGE_PREP_M2            = WLC_SUP_KEYXCHANGE,         /** Preparing to send    handshake msg M2 */
    WLC_SUP_KEYXCHANGE_WAIT_M3            = WLC_SUP_LAST_BASIC_STATE,   /** Waiting   to receive handshake msg M3 */
    WLC_SUP_KEYXCHANGE_PREP_M4            = 9  + WLC_SUP_STATUS_OFFSET, /** Preparing to send    handshake msg M4 */
    WLC_SUP_KEYXCHANGE_WAIT_G1            = 10 + WLC_SUP_STATUS_OFFSET, /** Waiting   to receive handshake msg G1 */
    WLC_SUP_KEYXCHANGE_PREP_G2            = 11 + WLC_SUP_STATUS_OFFSET, /** Preparing to send    handshake msg G2 */

    WLC_DOT11_SC_SUCCESS                  =  0 + WLC_DOT11_SC_STATUS_OFFSET,  /* Successful */
    WLC_DOT11_SC_FAILURE                  =  1 + WLC_DOT11_SC_STATUS_OFFSET,  /* Unspecified failure */
    WLC_DOT11_SC_CAP_MISMATCH             = 10 + WLC_DOT11_SC_STATUS_OFFSET,  /* Cannot support all requested capabilities in the Capability Information field */
    WLC_DOT11_SC_REASSOC_FAIL             = 11 + WLC_DOT11_SC_STATUS_OFFSET,  /* Reassociation denied due to inability to confirm that association exists */
    WLC_DOT11_SC_ASSOC_FAIL               = 12 + WLC_DOT11_SC_STATUS_OFFSET,  /* Association denied due to reason outside the scope of this standard */
    WLC_DOT11_SC_AUTH_MISMATCH            = 13 + WLC_DOT11_SC_STATUS_OFFSET,  /* Responding station does not support the specified authentication algorithm */
    WLC_DOT11_SC_AUTH_SEQ                 = 14 + WLC_DOT11_SC_STATUS_OFFSET,  /* Received an Authentication frame with authentication transaction sequence number out of expected sequence */
    WLC_DOT11_SC_AUTH_CHALLENGE_FAIL      = 15 + WLC_DOT11_SC_STATUS_OFFSET,  /* Authentication rejected because of challenge failure */
    WLC_DOT11_SC_AUTH_TIMEOUT             = 16 + WLC_DOT11_SC_STATUS_OFFSET,  /* Authentication rejected due to timeout waiting for next frame in sequence */
    WLC_DOT11_SC_ASSOC_BUSY_FAIL          = 17 + WLC_DOT11_SC_STATUS_OFFSET,  /* Association denied because AP is unable to handle additional associated stations */
    WLC_DOT11_SC_ASSOC_RATE_MISMATCH      = 18 + WLC_DOT11_SC_STATUS_OFFSET,  /* Association denied due to requesting station not supporting all of the data rates in the BSSBasicRateSet parameter */
    WLC_DOT11_SC_ASSOC_SHORT_REQUIRED     = 19 + WLC_DOT11_SC_STATUS_OFFSET,  /* Association denied due to requesting station not supporting the Short Preamble option */
    WLC_DOT11_SC_ASSOC_PBCC_REQUIRED      = 20 + WLC_DOT11_SC_STATUS_OFFSET,  /* Association denied due to requesting  station not supporting the PBCC Modulation option */
    WLC_DOT11_SC_ASSOC_AGILITY_REQUIRED   = 21 + WLC_DOT11_SC_STATUS_OFFSET,  /* Association denied due to requesting station not supporting the Channel Agility option */
    WLC_DOT11_SC_ASSOC_SPECTRUM_REQUIRED  = 22 + WLC_DOT11_SC_STATUS_OFFSET,  /* Association denied because Spectrum Management capability is required. */
    WLC_DOT11_SC_ASSOC_BAD_POWER_CAP      = 23 + WLC_DOT11_SC_STATUS_OFFSET,  /* Association denied because the info in the Power Cap element is unacceptable. */
    WLC_DOT11_SC_ASSOC_BAD_SUP_CHANNELS   = 24 + WLC_DOT11_SC_STATUS_OFFSET,  /* Association denied because the info in the Supported Channel element is unacceptable */
    WLC_DOT11_SC_ASSOC_SHORTSLOT_REQUIRED = 25 + WLC_DOT11_SC_STATUS_OFFSET,  /* Association denied due to requesting station not supporting the Short Slot Time option */
    WLC_DOT11_SC_ASSOC_ERPBCC_REQUIRED    = 26 + WLC_DOT11_SC_STATUS_OFFSET,  /* Association denied due to requesting station not supporting the ER-PBCC Modulation option */
    WLC_DOT11_SC_ASSOC_DSSOFDM_REQUIRED   = 27 + WLC_DOT11_SC_STATUS_OFFSET,  /* Association denied due to requesting station not supporting the DSS-OFDM option */
    WLC_DOT11_SC_DECLINED                 = 37 + WLC_DOT11_SC_STATUS_OFFSET,  /* request declined */
    WLC_DOT11_SC_INVALID_PARAMS           = 38 + WLC_DOT11_SC_STATUS_OFFSET,  /* One or more params have invalid values */
    WLC_DOT11_SC_INVALID_AKMP             = 43 + WLC_DOT11_SC_STATUS_OFFSET,  /* Association denied due to invalid AKMP */
    WLC_DOT11_SC_INVALID_MDID             = 54 + WLC_DOT11_SC_STATUS_OFFSET,  /* Association denied due to invalid MDID */
    WLC_DOT11_SC_INVALID_FTIE             = 55 + WLC_DOT11_SC_STATUS_OFFSET,  /* Association denied due to invalid FTIE */

    WLC_E_STATUS_FORCE_32_BIT   = 0x7FFFFFFE                     /** Force enum to be stored in 32 bit variable */
} wwd_event_status_t;

#define WLC_E_PRUNE_REASON_OFFSET    (256)
#define WLC_E_SUP_REASON_OFFSET      (512)
#define WLC_E_DOT11_RC_REASON_OFFSET (768)


/**
 * Enumerated list of event reason codes
 * @note : Several values overlap other values, so it is necessary
 *         to check the event type
 */
typedef enum
{
    /* roam reason codes */
    WLC_E_REASON_INITIAL_ASSOC    = 0,   /** initial assoc */
    WLC_E_REASON_LOW_RSSI         = 1,   /** roamed due to low RSSI */
    WLC_E_REASON_DEAUTH           = 2,   /** roamed due to DEAUTH indication */
    WLC_E_REASON_DISASSOC         = 3,   /** roamed due to DISASSOC indication */
    WLC_E_REASON_BCNS_LOST        = 4,   /** roamed due to lost beacons */
    WLC_E_REASON_FAST_ROAM_FAILED = 5,   /** roamed due to fast roam failure */
    WLC_E_REASON_DIRECTED_ROAM    = 6,   /** roamed due to request by AP */
    WLC_E_REASON_TSPEC_REJECTED   = 7,   /** roamed due to TSPEC rejection */
    WLC_E_REASON_BETTER_AP        = 8,   /** roamed due to finding better AP */

    /* NAN sub-events comes as a reason code with event as WLC_E_NAN */
    WLC_E_NAN_EVENT_STATUS_CHG = 9,  /* generated on any change in nan_mac status */
    WLC_E_NAN_EVENT_MERGE = 10,      /* Merged to a NAN cluster */
    WLC_E_NAN_EVENT_STOP = 11,       /* NAN stopped */
    WLC_E_NAN_EVENT_P2P = 12,       /* NAN P2P EVENT */

    /* XXX: Dont use below four events: They will be cleanup, use WL_NAN_EVENT_POST_DISC */
    WLC_E_NAN_EVENT_WINDOW_BEGIN_P2P = 13, /* Event for begin of P2P further availability window */
    WLC_E_NAN_EVENT_WINDOW_BEGIN_MESH = 14,
    WLC_E_NAN_EVENT_WINDOW_BEGIN_IBSS = 15,
    WLC_E_NAN_EVENT_WINDOW_BEGIN_RANGING = 16,
    WLC_E_NAN_EVENT_POST_DISC = 17, /* Event for post discovery data */
    WLC_E_NAN_EVENT_DATA_IF_ADD = 18, /* Event for Data IF add */
    WLC_E_NAN_EVENT_DATA_PEER_ADD = 19, /* Event for peer add */

    /* nan 2.0 */
    WLC_E_NAN_EVENT_DATA_IND = 20, /* Data Indication to Host */
    WLC_E_NAN_EVENT_DATA_CONF = 21, /* Data Response to Host */
    WLC_E_NAN_EVENT_SDF_RX = 22,       /* entire service discovery frame */
    WLC_E_NAN_EVENT_DATA_END = 23,
    WLC_E_NAN_EVENT_BCN_RX = 24,       /* received beacon payload */


    /* prune reason codes */
    WLC_E_PRUNE_ENCR_MISMATCH     = 1  + WLC_E_PRUNE_REASON_OFFSET,  /** encryption mismatch */
    WLC_E_PRUNE_BCAST_BSSID       = 2  + WLC_E_PRUNE_REASON_OFFSET,  /** AP uses a broadcast BSSID */
    WLC_E_PRUNE_MAC_DENY          = 3  + WLC_E_PRUNE_REASON_OFFSET,  /** STA's MAC addr is in AP's MAC deny list */
    WLC_E_PRUNE_MAC_NA            = 4  + WLC_E_PRUNE_REASON_OFFSET,  /** STA's MAC addr is not in AP's MAC allow list */
    WLC_E_PRUNE_REG_PASSV         = 5  + WLC_E_PRUNE_REASON_OFFSET,  /** AP not allowed due to regulatory restriction */
    WLC_E_PRUNE_SPCT_MGMT         = 6  + WLC_E_PRUNE_REASON_OFFSET,  /** AP does not support STA locale spectrum mgmt */
    WLC_E_PRUNE_RADAR             = 7  + WLC_E_PRUNE_REASON_OFFSET,  /** AP is on a radar channel of STA locale */
    WLC_E_RSN_MISMATCH            = 8  + WLC_E_PRUNE_REASON_OFFSET,  /** STA does not support AP's RSN */
    WLC_E_PRUNE_NO_COMMON_RATES   = 9  + WLC_E_PRUNE_REASON_OFFSET,  /** No rates in common with AP */
    WLC_E_PRUNE_BASIC_RATES       = 10 + WLC_E_PRUNE_REASON_OFFSET,  /** STA does not support all basic rates of BSS */
    WLC_E_PRUNE_CCXFAST_PREVAP    = 11 + WLC_E_PRUNE_REASON_OFFSET,  /** CCX FAST ROAM: prune previous AP */
    WLC_E_PRUNE_CIPHER_NA         = 12 + WLC_E_PRUNE_REASON_OFFSET,  /** BSS's cipher not supported */
    WLC_E_PRUNE_KNOWN_STA         = 13 + WLC_E_PRUNE_REASON_OFFSET,  /** AP is already known to us as a STA */
    WLC_E_PRUNE_CCXFAST_DROAM     = 14 + WLC_E_PRUNE_REASON_OFFSET,  /** CCX FAST ROAM: prune unqualified AP */
    WLC_E_PRUNE_WDS_PEER          = 15 + WLC_E_PRUNE_REASON_OFFSET,  /** AP is already known to us as a WDS peer */
    WLC_E_PRUNE_QBSS_LOAD         = 16 + WLC_E_PRUNE_REASON_OFFSET,  /** QBSS LOAD - AAC is too low */
    WLC_E_PRUNE_HOME_AP           = 17 + WLC_E_PRUNE_REASON_OFFSET,  /** prune home AP */
    WLC_E_PRUNE_AP_BLOCKED        = 18 + WLC_E_PRUNE_REASON_OFFSET,  /** prune blocked AP */
    WLC_E_PRUNE_NO_DIAG_SUPPORT   = 19 + WLC_E_PRUNE_REASON_OFFSET,  /** prune due to diagnostic mode not supported */

    /* WPA failure reason codes carried in the WLC_E_PSK_SUP event */
    WLC_E_SUP_OTHER               = 0  + WLC_E_SUP_REASON_OFFSET,  /** Other reason */
    WLC_E_SUP_DECRYPT_KEY_DATA    = 1  + WLC_E_SUP_REASON_OFFSET,  /** Decryption of key data failed */
    WLC_E_SUP_BAD_UCAST_WEP128    = 2  + WLC_E_SUP_REASON_OFFSET,  /** Illegal use of ucast WEP128 */
    WLC_E_SUP_BAD_UCAST_WEP40     = 3  + WLC_E_SUP_REASON_OFFSET,  /** Illegal use of ucast WEP40 */
    WLC_E_SUP_UNSUP_KEY_LEN       = 4  + WLC_E_SUP_REASON_OFFSET,  /** Unsupported key length */
    WLC_E_SUP_PW_KEY_CIPHER       = 5  + WLC_E_SUP_REASON_OFFSET,  /** Unicast cipher mismatch in pairwise key */
    WLC_E_SUP_MSG3_TOO_MANY_IE    = 6  + WLC_E_SUP_REASON_OFFSET,  /** WPA IE contains > 1 RSN IE in key msg 3 */
    WLC_E_SUP_MSG3_IE_MISMATCH    = 7  + WLC_E_SUP_REASON_OFFSET,  /** WPA IE mismatch in key message 3 */
    WLC_E_SUP_NO_INSTALL_FLAG     = 8  + WLC_E_SUP_REASON_OFFSET,  /** INSTALL flag unset in 4-way msg */
    WLC_E_SUP_MSG3_NO_GTK         = 9  + WLC_E_SUP_REASON_OFFSET,  /** encapsulated GTK missing from msg 3 */
    WLC_E_SUP_GRP_KEY_CIPHER      = 10 + WLC_E_SUP_REASON_OFFSET,  /** Multicast cipher mismatch in group key */
    WLC_E_SUP_GRP_MSG1_NO_GTK     = 11 + WLC_E_SUP_REASON_OFFSET,  /** encapsulated GTK missing from group msg 1 */
    WLC_E_SUP_GTK_DECRYPT_FAIL    = 12 + WLC_E_SUP_REASON_OFFSET,  /** GTK decrypt failure */
    WLC_E_SUP_SEND_FAIL           = 13 + WLC_E_SUP_REASON_OFFSET,  /** message send failure */
    WLC_E_SUP_DEAUTH              = 14 + WLC_E_SUP_REASON_OFFSET,  /** received FC_DEAUTH */
    WLC_E_SUP_WPA_PSK_TMO         = 15 + WLC_E_SUP_REASON_OFFSET,  /** WPA PSK 4-way handshake timeout */

    DOT11_RC_RESERVED             =  0 + WLC_E_DOT11_RC_REASON_OFFSET, /* d11 RC reserved */
    DOT11_RC_UNSPECIFIED          =  1 + WLC_E_DOT11_RC_REASON_OFFSET, /* Unspecified reason */
    DOT11_RC_AUTH_INVAL           =  2 + WLC_E_DOT11_RC_REASON_OFFSET, /* Previous authentication no longer valid */
    DOT11_RC_DEAUTH_LEAVING       =  3 + WLC_E_DOT11_RC_REASON_OFFSET, /* Deauthenticated because sending station is leaving (or has left) IBSS or ESS */
    DOT11_RC_INACTIVITY           =  4 + WLC_E_DOT11_RC_REASON_OFFSET, /* Disassociated due to inactivity */
    DOT11_RC_BUSY                 =  5 + WLC_E_DOT11_RC_REASON_OFFSET, /* Disassociated because AP is unable to handle all currently associated stations */
    DOT11_RC_INVAL_CLASS_2        =  6 + WLC_E_DOT11_RC_REASON_OFFSET, /* Class 2 frame received from nonauthenticated station */
    DOT11_RC_INVAL_CLASS_3        =  7 + WLC_E_DOT11_RC_REASON_OFFSET, /* Class 3 frame received from nonassociated station */
    DOT11_RC_DISASSOC_LEAVING     =  8 + WLC_E_DOT11_RC_REASON_OFFSET, /* Disassociated because sending station is leaving (or has left) BSS */
    DOT11_RC_NOT_AUTH             =  9 + WLC_E_DOT11_RC_REASON_OFFSET, /* Station requesting (re)association is not * authenticated with responding station */
    DOT11_RC_BAD_PC               = 10 + WLC_E_DOT11_RC_REASON_OFFSET, /* Unacceptable power capability element */
    DOT11_RC_BAD_CHANNELS         = 11 + WLC_E_DOT11_RC_REASON_OFFSET, /* Unacceptable supported channels element */
                                                                       /* 12 is unused */
                                                                       /* XXX 13-23 are WPA/802.11i reason codes defined in proto/wpa.h */
                                                                       /* 32-39 are QSTA specific reasons added in 11e */
    DOT11_RC_UNSPECIFIED_QOS      = 32 + WLC_E_DOT11_RC_REASON_OFFSET, /* unspecified QoS-related reason */
    DOT11_RC_INSUFFCIENT_BW       = 33 + WLC_E_DOT11_RC_REASON_OFFSET, /* QAP lacks sufficient bandwidth */
    DOT11_RC_EXCESSIVE_FRAMES     = 34 + WLC_E_DOT11_RC_REASON_OFFSET, /* excessive number of frames need ack */
    DOT11_RC_TX_OUTSIDE_TXOP      = 35 + WLC_E_DOT11_RC_REASON_OFFSET, /* transmitting outside the limits of txop */
    DOT11_RC_LEAVING_QBSS         = 36 + WLC_E_DOT11_RC_REASON_OFFSET, /* QSTA is leaving the QBSS (or restting) */
    DOT11_RC_BAD_MECHANISM        = 37 + WLC_E_DOT11_RC_REASON_OFFSET, /* does not want to use the mechanism */
    DOT11_RC_SETUP_NEEDED         = 38 + WLC_E_DOT11_RC_REASON_OFFSET, /* mechanism needs a setup */
    DOT11_RC_TIMEOUT              = 39 + WLC_E_DOT11_RC_REASON_OFFSET, /* timeout */
    DOT11_RC_MAX                  = 23 + WLC_E_DOT11_RC_REASON_OFFSET, /* Reason codes > 23 are reserved */

    WLC_E_REASON_FORCE_32_BIT     = 0x7FFFFFFE                     /** Force enum to be stored in 32 bit variable */
} wwd_event_reason_t;


typedef enum
{
    WLC_E_NAN_EVENT_START = 1,     /* NAN cluster started */
    WLC_E_NAN_EVENT_JOIN = 2,      /* Joined to a NAN cluster */
    WLC_E_NAN_EVENT_ROLE = 3,      /* Role or State changed */
    WLC_E_NAN_EVENT_SCAN_COMPLETE = 4,
    WLC_E_NAN_EVENT_DISCOVERY_RESULT = 5,
    WLC_E_NAN_EVENT_REPLIED = 6,
    WLC_E_NAN_EVENT_TERMINATED = 7,    /* the instance ID will be present in the ev data */
    WLC_E_NAN_EVENT_RECEIVE = 8

} wwd_nan_events_t;

/**
 * Event handler prototype definition
 *
 * @param[out] wwd_event_header_t : wwd event header
 * @param[out] uint8_t*           : event data
 * @param[out] handler_user_data  : semaphore data
 */
typedef void* (*wwd_event_handler_t)( const wwd_event_header_t* event_header, const uint8_t* event_data, /*@null@*/ void* handler_user_data );



/*
 * @param[in] wwd_rrm_report_type : Radio resource management report type
 * @param[out] callback           : A callback when the RRM report is received
 * @param[in]  rrm_req            : pointer to the radio resource management request data
 * @param[out] report_ptr         : A pointer to the pointer that indicates where to put the next RRM report
 * @param[in]  void *             : user data ( semaphore)
 * @param[in]  interface          : WWD_STA_INTERFACE or WWD_AP_INTERFACE
*/
wwd_result_t wwd_wifi_rrm_request(   wwd_rrm_report_type_t                      report_type,
                                     wwd_event_handler_t                        callback,
                                     void*                                      rrm_req,
                                     wwd_rrm_report_t**                         report_ptr,
                                     void*                                      user_data,
                                     wwd_interface_t                            interface
                                  );
/** @endcond */


/** @addtogroup mgmt WICED Management
 *  @{
 */

/**
 * Registers locally a handler to receive event callbacks.
 * Does not notify Wi-Fi about event subscription change.
 * Can be used to refresh local callbacks (e.g. after deep-sleep)
 * if Wi-Fi is already notified about them.
 *
 * This function registers a callback handler to be notified when
 * a particular event is received.
 *
 * Alternately the function clears callbacks for given event type.
 *
 * @note : Currently each event may only be registered to one handler
 *         and there is a limit to the number of simultaneously registered
 *         events
 *
 * @param  event_nums     The event types that are to trigger the handler
 *                        See @ref wwd_event_num_t for available events.
 *                        Must be defined in a global constant.
 * @param handler_func   A function pointer to the new handler callback,
 *                        or NULL if callbacks are to be disabled for the given event type
 * @param handler_user_data  A pointer value which will be passed to the event handler function
 *                            at the time an event is triggered (NULL is allowed)
 *
 * @return WWD_SUCCESS or Error code
 */
extern wwd_result_t wwd_management_set_event_handler_locally( /*@keep@*/ const wwd_event_num_t* event_nums, /*@null@*/ wwd_event_handler_t handler_func, /*@null@*/ /*@keep@*/ void* handler_user_data, wwd_interface_t interface );


/**
 * Registers a handler to receive event callbacks.
 * Subscribe locally and notify Wi-Fi about subscription.
 *
 * This function registers a callback handler to be notified when
 * a particular event is received.
 *
 * Alternately the function clears callbacks for given event type.
 *
 * @note : Currently each event may only be registered to one handler
 *         and there is a limit to the number of simultaneously registered
 *         events
 *
 * @param  event_nums     The event types that are to trigger the handler
 *                        See @ref wwd_event_num_t for available events.
 *                        Must be defined in a global constant.
 * @param handler_func   A function pointer to the new handler callback,
 *                        or NULL if callbacks are to be disabled for the given event type
 * @param handler_user_data  A pointer value which will be passed to the event handler function
 *                            at the time an event is triggered (NULL is allowed)
 *
 * @return WWD_SUCCESS or Error code
 */
extern wwd_result_t wwd_management_set_event_handler( /*@keep@*/ const wwd_event_num_t* event_nums, /*@null@*/ wwd_event_handler_t handler_func, /*@null@*/ /*@keep@*/ void* handler_user_data, wwd_interface_t interface );

/** @cond */

extern void*  wiced_rrm_report_handler ( const wwd_event_header_t* event_header, const uint8_t* event_data, /*@returned@*/ void* handler_user_data );

extern void*  wiced_nan_scan_handler   ( const wwd_event_header_t* event_header, const uint8_t* event_data, /*@returned@*/ void* handler_user_data );

/*
* @param[out] result_ptr         : A pointer to the pointer that indicates where to put the next RRM report
* @param[out] callback           : A callback when the RRM report is received
* @param[out] wwd_rrm_report_t   : Pointer to RRM report data
* @param[in] interface           : WWD_STA_INTERFACE or WWD_AP_INTERFACE
*/
wwd_result_t wiced_wifi_rrm_request(   wwd_rrm_report_type_t                        report_type,
                                       wwd_event_handler_t                          callback,
                                       void*                                        rrm_req,
                                       wwd_rrm_report_t**                           report_ptr,
                                       wwd_interface_t                              interface
                                   );
#pragma pack(1)
struct wiced_event_header_struct
{
    uint16_t             reserved1;
    uint16_t             flags;
    wwd_event_num_t      event_type;
    wwd_event_status_t   status;
    wwd_event_reason_t   reason;
    uint32_t             auth_type;
    uint32_t             datalen;
    wiced_mac_t          addr;
    uint8_t              interface; /* Do not use wwd_interface_t here */
};
#pragma pack()

/** @endcond */

/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* ifndef INCLUDED_WWD_EVENTS_H */
