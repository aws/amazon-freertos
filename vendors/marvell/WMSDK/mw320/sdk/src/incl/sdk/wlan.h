/*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
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

/*! \file wlan.h
 * \brief WLAN Connection Manager
 *
 * The WLAN Connection Manager (WLCMGR) is one of the core components that
 * provides WiFi-level functionality like scanning for networks,
 * starting a network (Access Point) and associating / disassociating with
 * other wireless networks. The WLCMGR manages two logical interfaces,
 * the station interface and the micro-AP interface.
 * Both these interfaces can be active at the same time.
 *
 * \section wlan_usage Usage
 *
 * The WLCMGR is initialized by calling \ref wlan_init() and started by
 * calling \ref wlan_start(), one of the arguments of this function is a
 * callback handler. Many of the WLCMGR tasks are asynchronous in nature,
 * and the events are provided by invoking the callback handler.
 * The various usage scenarios of the WLCMGR are outlined below:
 *
 * - <b>Scanning:</b> A call to wlan_scan() initiates an asynchronous scan of
 *      the nearby wireless networks. The results are reported via the callback
 *      handler.
 * - <b>Network Profiles:</b> Starting / stopping wireless interfaces or
 *      associating / disassociating with other wireless networks is managed
 *      through network profiles. The network profiles record details about the
 *      wireless network like the SSID, type of security, security passphrase
 *      among other things. The network profiles can be managed by means of the
 *      \ref wlan_add_network() and \ref wlan_remove_network() calls.
 * - <b>Association:</b> The \ref wlan_connect() and \ref wlan_disconnect()
 *      calls can be used to manage connectivity with other wireless networks
 *      (Access Points). These calls manage the station interface of the system.
 * - <b>Starting a Wireless Network:</b> The \ref wlan_start_network()
 *       and \ref wlan_stop_network() calls can be used to start/stop
 *       our own (micro-AP) network. These calls manage
 *       the micro-AP interface of the system.
 *
 * @cond uml_diag
 *
 * \section WLCMGR_station_sm Station State Machine
 *
 * The WLAN Connection Manager station state diagram is as shown below. The Yellow boxes
 * indicate the various states. The top half is the name of the state, and the
 * bottom half consists of any code that is executed on entering that
 * state. Labels on the transitions indicate when that particular transition is
 * taken.
 *
 * @startuml{station.jpg}
 *
 * [*] --> Initializing
 * Initializing: net_wlan_init()
 * Initializing -down-> Idle : Success
 *
 * Idle -right-> Scanning : wlan_connect(WPA/WPA2/Mixed/Open)
 * Idle -left-> Configuring : wlan_connect(WEP)
 * Idle -down-> Scanning_User: wlan_scan()
 *
 * Scanning_User: report_scan_results()
 * Scanning_User -up-> Idle: scan_success
 *
 * Scanning: handle_scan_results()
 * Scanning -left-> Scanning : scan_success
 * Scanning -down-> Associating: Success
 * Configuring: wifi_send_wep_key_material_cmd()
 * Configuring -down-> Scanning: Success
 *
 * Associating: configure_security(), wifi_assoc()
 * Associating -down->Associated: assoc_success 
 * Associating -up->Scanning: assoc_failure
 *
 * Associated -down-> Requesting_Address: authentication_success
 * Associated -up-> Idle: authentication_failure, disassociation, deauthentication
 *
 * Requesting_Address: net_configure_address()
 * Requesting_Address -down-> Connected: static_ip
 * Requesting_Address -left-> Obtaining_Address: dhcp_ip
 *
 * Obtaining_Address: dhcp_start()
 * Obtaining_Address -down-> Connected: dhcp_success
 * Obtaining_Address -up-> Idle: dhcp_failure
 *
 * Connected: net_configure_dns()
 * Connected -up-> Idle: lins_loss, channel_switch, wlan_disconnect()
 *
 * @enduml
 *
 * \section WLCMGR_uap_sm Micro-AP State Machine
 *
 * The WLAN Connection Manager micro-AP state diagram is as shown below. The Yellow boxes
 * indicate the various states. The top half is the name of the state, and the
 * bottom half consists of any code that is executed on entering that
 * state. Labels on the transitions indicate when that particular transition is
 * taken.
 *
 * @startuml{uap.jpg}
 *
 * [*] --> Initializing
 * Initializing: net_wlan_init()
 * Initializing -down-> Configured: wlan_start_network()
 *
 * Configured: do_start()
 * Configured -down-> Started: up_started
 *
 * Started: net_configure_address()
 * Started -down-> Up: uap_addr_config
 *
 * Up -up-> Initializing: wlan_stop_network()
 * Started -up-> Initializing: wlan_stop_network()
 *
 * @enduml
 *
 * @endcond
 */

#ifndef __WLAN_H__
#define __WLAN_H__

#include <wmtypes.h>
#include <pwrmgr.h>
#include <wmerrno.h>
#include <flash.h>
#include <stdint.h>
#include <wifi_events.h>
#include <wifi.h>
#include <wlan_11d.h>
#ifdef CONFIG_WPA2_ENTP
  #include <wm_mbedtls_helper_api.h>
#endif

/* Configuration */

#include <wmlog.h>
#define wlcm_e(...)				\
	wmlog_e("wlcm", ##__VA_ARGS__)
#define wlcm_w(...)				\
	wmlog_w("wlcm", ##__VA_ARGS__)

#ifdef CONFIG_WLCMGR_DEBUG
#define wlcm_d(...)				\
	wmlog("wlcm", ##__VA_ARGS__)
#else
#define wlcm_d(...)
#endif /* ! CONFIG_WLCMGR_DEBUG */


#ifndef IEEEtypes_SSID_SIZE
#define IEEEtypes_SSID_SIZE            32
#endif /* IEEEtypes_SSID_SIZE */

#ifndef IEEEtypes_ADDRESS_SIZE
#define IEEEtypes_ADDRESS_SIZE         6
#endif /* IEEEtypes_ADDRESS_SIZE */

typedef enum {
	BSS_INFRASTRUCTURE = 1,
	BSS_INDEPENDENT,
	BSS_ANY
} IEEEtypes_Bss_t;

typedef enum {
	MGMT_RSN_IE = 48,
	MGMT_VENDOR_SPECIFIC_221 = 221,
	MGMT_WPA_IE = MGMT_VENDOR_SPECIFIC_221,
	MGMT_WPS_IE = MGMT_VENDOR_SPECIFIC_221,
} IEEEtypes_ElementId_t;

#define EVENT_SUBTYPE_FIRST_PACKET	0x01
#define EVENT_SUBTYPE_LAST_PACKET	0x02

/* The possible types of Basic Service Sets */

/** The number of times that the WLAN Connection Manager will look for a
 *  network before giving up. */
#ifdef CONFIG_P2P
#define WLAN_RESCAN_LIMIT				10
#else
#define WLAN_RESCAN_LIMIT				5
#endif
#define WLAN_11D_SCAN_LIMIT                            3
/** The minimum length for network names, see \ref wlan_network.  This must
 *  be between 1 and \ref WLAN_NETWORK_NAME_MAX_LENGTH */
#define WLAN_NETWORK_NAME_MIN_LENGTH	1
/** The space reserved for storing network names, \ref wlan_network */
#define WLAN_NETWORK_NAME_MAX_LENGTH	32
/** The space reserved for storing PSK (password) phrases. */
/* Max WPA2 passphrase can be upto 64 ASCII chars */
#define WLAN_PSK_MAX_LENGTH		65
/* Max WPA2 Enterprise identity can be upto 256 characters */
#define IDENTITY_MAX_LENGTH		256
/* Max WPA2 Enterprise password can be upto 256 unicode characters */
#define PASSWORD_MAX_LENGTH		256

#ifdef CONFIG_WLAN_KNOWN_NETWORKS
/** The size of the list of known networks maintained by the WLAN
   Connection Manager */
#define WLAN_MAX_KNOWN_NETWORKS CONFIG_WLAN_KNOWN_NETWORKS
#else
#error "CONFIG_WLAN_KNOWN_NETWORKS is not defined"
#endif /* CONFIG_WLAN_KNOWN_NETWORKS */
/** Length of a pairwise master key (PMK).  It's always 256 bits (32 Bytes) */
#define WLAN_PMK_LENGTH			32

/** The maximum number of IPV6 addresses that will be stored */
#define MAX_IPV6_ADDRESSES	3

/* Error Codes */

/** The operation was successful. */
#define WLAN_ERROR_NONE		0
/** The operation failed due to an error with one or more parameters. */
#define WLAN_ERROR_PARAM	1
/** The operation could not be performed because there is not enough memory. */
#define WLAN_ERROR_NOMEM	2
/** The operation could not be performed in the current system state. */
#define WLAN_ERROR_STATE	3
/** The operation failed due to an internal error. */
#define WLAN_ERROR_ACTION	4
/** The operation to change power state could not be performed*/
#define WLAN_ERROR_PS_ACTION	5
/** The requested feature is not supported*/
#define WLAN_ERROR_NOT_SUPPORTED 6

/** BIT value */
#define MBIT(x)		(((t_u32)1) << (x))

/** BITMAP for Association request */
#define WLAN_MGMT_ASSOC_REQ			MBIT(0)
/** BITMAP for Association response */
#define WLAN_MGMT_ASSOC_RESP			MBIT(1)
/** BITMAP for Reassociation request */
#define WLAN_MGMT_REASSOC_REQ			MBIT(2)
/** BITMAP for Reassociation response*/
#define WLAN_MGMT_REASSOC_RESP			MBIT(3)
/** BITMAP for Probe request */
#define WLAN_MGMT_PROBE_REQ			MBIT(4)
/** BITMAP for Probe response */
#define WLAN_MGMT_PROBE_RESP			MBIT(5)
/** BITMAP for Beacon */
#define WLAN_MGMT_BEACON			MBIT(8)
/** BITMAP for Action frame */
#define WLAN_MGMT_ACTION			MBIT(13)

/** enum for wlan errors*/
enum wm_wlan_errno {
	WM_E_WLAN_ERRNO_BASE = MOD_ERROR_START(MOD_WLAN),
	/** The Firmware download operation failed. */
	WLAN_ERROR_FW_DNLD_FAILED,
	/** The Firmware ready register not set. */
	WLAN_ERROR_FW_NOT_READY,
	/** The WiFi card not found. */
	WLAN_ERROR_CARD_NOT_DETECTED,
	/** The WiFi Firmware not found. */
	WLAN_ERROR_FW_NOT_DETECTED,
	/** BSSID not found in scan list */
	WLAN_BSSID_NOT_FOUND_IN_SCAN_LIST,
};
/* Events and States */

/** WLAN Connection Manager event reason */
enum wlan_event_reason {
	/** The WLAN Connection Manager has successfully connected to a network and
	 *  is now in the \ref WLAN_CONNECTED state. */
	WLAN_REASON_SUCCESS,
	/** The WLAN Connection Manager failed to connect before actual
	 * connection attempt with AP due to incorrect wlan network profile. */
	WLAN_REASON_CONNECT_FAILED,
	/** The WLAN Connection Manager could not find the network that it was
	 *  connecting to (or it has tried all known networks and failed to connect
	 *  to any of them) and it is now in the \ref WLAN_DISCONNECTED state. */
	WLAN_REASON_NETWORK_NOT_FOUND,
	/** The WLAN Connection Manager failed to authenticate with the network
	 *  and is now in the \ref WLAN_DISCONNECTED state. */
	WLAN_REASON_NETWORK_AUTH_FAILED,
	/** DHCP lease has been renewed.*/
	WLAN_REASON_ADDRESS_SUCCESS,
	/** The WLAN Connection Manager failed to obtain an IP address
	 *  or TCP stack configuration has failed or the IP address
	 *  configuration was lost due to a DHCP error.  The system is
	 *  now in the \ref WLAN_DISCONNECTED state. */
	WLAN_REASON_ADDRESS_FAILED,
	/** The WLAN Connection Manager has lost the link to the current network. */
	WLAN_REASON_LINK_LOST,
	/** The WLAN Connection Manager has received the channel switch
	 * announcement from the current network. */
	WLAN_REASON_CHAN_SWITCH,
	/** The WLAN Connection Manager has disconnected from the WPS network
	 *  (or has canceled a connection attempt) by request and is now in the
	 *  WLAN_DISCONNECTED state. */
	WLAN_REASON_WPS_DISCONNECT,
	/** The WLAN Connection Manager has disconnected from the current network
	 *  (or has canceled a connection attempt) by request and is now in the
	 *  WLAN_DISCONNECTED state. */
	WLAN_REASON_USER_DISCONNECT,
	/** The WLAN Connection Manager is initialized and is ready for use.
	 *  That is, it's now possible to scan or to connect to a network. */
	WLAN_REASON_INITIALIZED,
	/** The WLAN Connection Manager has failed to initialize and is therefore
	 *  not running. It is not possible to scan or to connect to a network.  The
	 *  WLAN Connection Manager should be stopped and started again via
	 *  wlan_stop() and wlan_start() respectively. */
	WLAN_REASON_INITIALIZATION_FAILED,
	/** The WLAN Connection Manager has entered power save mode. */
	WLAN_REASON_PS_ENTER,
	/** The WLAN Connection Manager has exited from power save mode. */
	WLAN_REASON_PS_EXIT,
	/** The WLAN Connection Manager has started uAP */
	WLAN_REASON_UAP_SUCCESS,
	/** A wireless client has joined uAP's BSS network */
	WLAN_REASON_UAP_CLIENT_ASSOC,
	/** A wireless client has left uAP's BSS network */
	WLAN_REASON_UAP_CLIENT_DISSOC,
	/** The WLAN Connection Manager has failed to start uAP */
	WLAN_REASON_UAP_START_FAILED,
	/** The WLAN Connection Manager has failed to stop uAP */
	WLAN_REASON_UAP_STOP_FAILED,
	/** The WLAN Connection Manager has stopped uAP */
	WLAN_REASON_UAP_STOPPED,
};

/** Wakeup events for which wakeup will occur */
enum wlan_wakeup_event_t {
	/** Wakeup on broadcast  */
	WAKE_ON_ALL_BROADCAST = 1,
	/** Wakeup on unicast  */
	WAKE_ON_UNICAST = 1<<1,
	/** Wakeup on MAC event  */
	WAKE_ON_MAC_EVENT = 1<<2,
	/** Wakeup on multicast  */
	WAKE_ON_MULTICAST = 1<<3,
	/** Wakeup on ARP broadcast  */
	WAKE_ON_ARP_BROADCAST = 1 << 4,
	/** Wakeup on receiving a management frame  */
	WAKE_ON_MGMT_FRAME = 1<<6,
};

/** WLAN station/micro-AP/Wi-Fi Direct Connection/Status state */
enum wlan_connection_state {
	/** The WLAN Connection Manager is not connected and no connection attempt
	 *  is in progress.  It is possible to connect to a network or scan. */
	WLAN_DISCONNECTED,
	/** The WLAN Connection Manager is not connected but it is currently 
	 *  attempting to connect to a network.  It is not possible to scan at this
	 *  time.  It is possible to connect to a different network. */
	WLAN_CONNECTING,
	/** The WLAN Connection Manager is not connected but associated. */
	WLAN_ASSOCIATED,
	/** The WLAN Connection Manager is connected.  It is possible to scan and
	 *  connect to another network at this time.  Information about the current
	 *  network configuration is available. */
	WLAN_CONNECTED,
	/** The WLAN Connection Manager has started uAP */
	WLAN_UAP_STARTED,
	/** The WLAN Connection Manager has stopped uAP */
	WLAN_UAP_STOPPED,
	/** The WLAN Connection Manager has started P2P uAP */
	WLAN_P2P_STARTED,
	/** The WLAN Connection Manager has stopped P2P uAP */
	WLAN_P2P_STOPPED,
	/** The WLAN Connection Manager is not connected and network scan
	 * is in progress. */
	WLAN_SCANNING,
	/** The WLAN Connection Manager is not connected and network association
	 * is in progress. */
	WLAN_ASSOCIATING,
};

/* Data Structures */

/** Station Power save mode */
enum wlan_ps_mode {
	/** Active mode */
	WLAN_ACTIVE = 0,
	/** IEEE power save mode */
	WLAN_IEEE,
	/** Deep sleep power save mode */
	WLAN_DEEP_SLEEP,
	/** Power down power save mode */
	WLAN_PDN,
};

/** micro-AP power save mode */
enum wlan_uap_ps_mode  {
	/**  Active mode */
	WLAN_UAP_ACTIVE = WLAN_PDN + 1,
	/** DTIM Sleep mode */	
	WLAN_UAP_DTIM_SLEEP,
	/** Inactivity Sleep mode */
	WLAN_UAP_INACTIVITY_SLEEP,
};

/** Wi-Fi Direct(GO) power save mode */
enum wlan_wfd_ps_mode  {
	/**  Active mode */
	WLAN_P2P_ACTIVE = 0,
	/** DTIM Sleep mode */	
	WLAN_P2P_DTIM_SLEEP,
	/** Inactivity Sleep mode */
	WLAN_P2P_INACTIVITY_SLEEP,
};

/* fixme: remove this check when mlan integration complete */
#ifndef INCLUDE_FROM_MLAN
enum wlan_ps_state {
	PS_STATE_AWAKE = 0,
	PS_STATE_PRE_SLEEP,
	PS_STATE_SLEEP_CFM,
	PS_STATE_SLEEP
};

typedef enum _ENH_PS_MODES
{
    GET_PS = 0,
    SLEEP_CONFIRM = 5,
    DIS_AUTO_PS = 0xfe,
    EN_AUTO_PS = 0xff,
} ENH_PS_MODES;

typedef enum _Host_Sleep_Action
{
    HS_CONFIGURE = 0x0001,
    HS_ACTIVATE = 0x0002,
} Host_Sleep_Action;
#endif /* INCLUDE_FROM_MLAN */

/** Scan Result */
struct wlan_scan_result {
	/** The network SSID, represented as a NULL-terminated C string of 0 to 32
	 *  characters.  If the network has a hidden SSID, this will be the empty
	 *  string.
	 */
	char ssid[33];
	/** SSID length */
	unsigned int ssid_len;
	/** The network BSSID, represented as a 6-byte array. */
	char bssid[6];
	/** The network channel. */
	unsigned int channel;
	/** The network wireless type. */
	enum wlan_bss_type type;
	/** The network wireless mode. */
	enum wlan_bss_role role;

	/* network features */

	/** The network supports WMM.  This is set to 0 if the network does not
	 *  support WMM or if the system does not have WMM support enabled. */
	unsigned wmm:1;
#ifdef CONFIG_WPS2
	/** The network supports WPS.  This is set to 0 if the network does not
	 *  support WPS or if the system does not have WPS support enabled. */
	unsigned wps:1;
	/** WPS Type PBC/PIN */
	unsigned int wps_session;
#endif
	/** WPA2 Enterprise security */
	unsigned wpa2_entp:1;
	/** The network uses WEP security. */
	unsigned wep:1;
	/** The network uses WPA security. */
	unsigned wpa:1;
	/** The network uses WPA2 security */
	unsigned wpa2:1;
	/** The network uses WPA3 SAE security */
	unsigned wpa3_sae:1;

	/** The signal strength of the beacon */
	unsigned char rssi;
	/** The network SSID, represented as a NULL-terminated C string of 0 to 32
	 *  characters.  If the network has a hidden SSID, this will be the empty
	 *  string.
	 */
	char trans_ssid[33];
	/** SSID length */
	unsigned int trans_ssid_len;
	/** The network BSSID, represented as a 6-byte array. */
	char trans_bssid[6];
};

typedef enum
{
	Band_2_4_GHz = 0,
	Band_5_GHz   = 1,
	Band_4_GHz   = 2,

} ChanBand_e;

#define NUM_CHAN_BAND_ENUMS  3

typedef enum
{
	ChanWidth_20_MHz = 0,
	ChanWidth_10_MHz = 1,
	ChanWidth_40_MHz = 2,
	ChanWidth_80_MHz = 3,
} ChanWidth_e;

typedef enum
{
	SECONDARY_CHAN_NONE  = 0,
	SECONDARY_CHAN_ABOVE = 1,
	SECONDARY_CHAN_BELOW = 3,
	//reserved 2, 4~255
} Chan2Offset_e;

typedef enum
{
	MANUAL_MODE = 0,
	ACS_MODE    = 1,
} ScanMode_e;

typedef PACK_START struct
{
	ChanBand_e       chanBand     : 2;
	ChanWidth_e      chanWidth    : 2;
	Chan2Offset_e    chan2Offset  : 2;
	ScanMode_e       scanMode     : 2;
} PACK_END BandConfig_t;

typedef PACK_START struct
{
	BandConfig_t bandConfig;
	uint8_t        chanNum;

} PACK_END ChanBandInfo_t;

/*auto link switch network info*/
typedef PACK_START struct _Event_AutoLink_SW_Node_t
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
	/*peer mac address*/
	uint8_t peer_mac_addr[MLAN_MAC_ADDR_LENGTH];
	/*associated channel band info*/
	ChanBandInfo_t chanBand;
	/*security type*/
	uint8_t secutype;
	/*multicast cipher*/
	uint8_t mcstcipher;
	/*unicast cipher*/
	uint8_t ucstcipher;
	/*peer ssid info*/
	/* tlv type*/
	uint16_t type_ssid;
	/** Header length */
	uint16_t len_ssid;
	/*ssid info*/
	uint8_t ssid[1];
} PACK_END Event_AutoLink_SW_Node_t;

/** Network security types*/
enum wlan_security_type {
	/** The network does not use security. */
	WLAN_SECURITY_NONE,
	/** The network uses WEP security with open key. */
	WLAN_SECURITY_WEP_OPEN,
	/** The network uses WEP security with shared key. */
	WLAN_SECURITY_WEP_SHARED,
	/** The network uses WPA security with PSK. */
	WLAN_SECURITY_WPA,
	/** The network uses WPA2 security with PSK. */
	WLAN_SECURITY_WPA2,
	/** The network uses WPA/WPA2 mixed security with PSK */
	WLAN_SECURITY_WPA_WPA2_MIXED,
	/** The network uses WPA2 Enterprise EAP-TLS security
	 * The identity field in \ref wlan_network structure is used */
	WLAN_SECURITY_EAP_TLS,
	/** The network can use any security method. This is often used when
	 * the user only knows the name and passphrase but not the security
	 * type.  */
	WLAN_SECURITY_WILDCARD,
	/** The network uses WPA2 Enterprise PEAP-MSCHAPV2 security
	 * The anonymous identity, identity and password fields in
	 * \ref wlan_network structure are used */
	WLAN_SECURITY_PEAP_MSCHAPV2,
	/** The network uses WPA3 security with SAE. */
	WLAN_SECURITY_WPA3_SAE,
	/** The network uses OWE only security without Transition mode support. */
	WLAN_SECURITY_OWE_ONLY,
};

struct wlan_cipher {
	uint8_t wep40:1;
	uint8_t wep104:1;
	uint8_t tkip:1;
	uint8_t ccmp:1;
	uint8_t rsvd:4;
} ;

static inline int is_valid_security(int security)
{
	/*Currently only these modes are supported */
	if ((security == WLAN_SECURITY_NONE) ||
	    (security == WLAN_SECURITY_WEP_OPEN) ||
	    (security == WLAN_SECURITY_WPA) ||
	    (security == WLAN_SECURITY_WPA2) ||
	    (security == WLAN_SECURITY_WPA_WPA2_MIXED) ||
	    (security == WLAN_SECURITY_EAP_TLS) ||
	    (security == WLAN_SECURITY_PEAP_MSCHAPV2) ||
	    (security == WLAN_SECURITY_OWE_ONLY) ||
	    (security == WLAN_SECURITY_WPA3_SAE) ||
	    (security == WLAN_SECURITY_WILDCARD))
		return 0;
	return -1;
}

/** Network security configuration */
struct wlan_network_security {
	/** Type of network security to use specified by enum
	 * wlan_security_type. */
	enum wlan_security_type type;
	/** Type of network security Group Cipher suite used internally*/
	struct wlan_cipher mcstCipher;
	/** Type of network security Pairwise Cipher suite used internally*/
	struct wlan_cipher ucstCipher;
	/** Is PMF required */
	bool is_pmf_required;
	/** Pre-shared key (network password).  For WEP networks this is a hex byte
	 * sequence of length psk_len, for WPA and WPA2 networks this is an ASCII
	 * pass-phrase of length psk_len.  This field is ignored for networks with no
	 * security. */
	char psk[WLAN_PSK_MAX_LENGTH];
	/** Length of the WEP key or WPA/WPA2 pass phrase, 0 to \ref
	 * WLAN_PSK_MAX_LENGTH.  Ignored for networks with no security. */
	char psk_len;
	/** Pairwise Master Key.  When pmk_valid is set, this is the PMK calculated
	 * from the PSK for WPA/PSK networks.  If pmk_valid is not set, this field
	 * is not valid.  When adding networks with \ref wlan_add_network, users
	 * can initialize pmk and set pmk_valid in lieu of setting the psk.  After
	 * successfully connecting to a WPA/PSK network, users can call \ref
	 * wlan_get_current_network to inspect pmk_valid and pmk.  Thus, the pmk
	 * value can be populated in subsequent calls to \ref wlan_add_network.
	 * This saves the CPU time required to otherwise calculate the PMK.
	 */
	char pmk[WLAN_PMK_LENGTH];

	/** Flag reporting whether pmk is valid or not. */
	bool pmk_valid;
#ifdef CONFIG_WLAN_BRIDGE
	/** Pre-shared key (network password) for bridge uap.*/
	char bridge_psk[WLAN_PSK_MAX_LENGTH];
	/** Length of the WEP key or WPA/WPA2 pass phrase for bridge uap, 0 to \ref
	 * WLAN_PSK_MAX_LENGTH.  Ignored for networks with no security. */
	char bridge_psk_len;
	/** Pairwise Master Key for bridge network */
	char bridge_pmk[WLAN_PMK_LENGTH];
	/** Flag reporting whether bridge pmk is valid or not. */
	bool bridge_pmk_valid;
#endif
#ifdef CONFIG_WPA2_ENTP
	/** TLS client cert configuration */
	wm_mbedtls_cert_t tls_cert;
	/* mbedtls_ssl_config handle */
	mbedtls_ssl_config *wlan_ctx;
	/* mbedtls_ssl_context handle */
	mbedtls_ssl_context *wlan_ssl;
#endif
};

/* Configuration for wireless scanning */
#define MAX_CHANNEL_LIST 5
struct wifi_scan_params_t {
	uint8_t *bssid;
	char *ssid;
	int channel[MAX_CHANNEL_LIST];
	IEEEtypes_Bss_t bss_type;
	int scan_duration;
	int split_scan_delay;
};

struct wlan_uapps_params_t {
	/** control bitmap
	 * 0 - disable protection frame Tx before PS(default)
	 * 1 - enable protection frame Tx before PS
	 */
	unsigned int ctrl_bitmap;
	/** minimum sleep period (micro second)
	 * Minimum sleep period between two beacons during
	 * inactivity.
	 */
	unsigned int min_sleep;
	/** maximum sleep period (micro second)
	 * Maximum sleep period between two beacons during
	 * inactivity.
	 */
	unsigned int max_sleep;
	/** minimum awake period (micro second)
	 * Minimum awake period between two beacons during
	 * inactivity.
	 */
	unsigned int min_awake;
	/** maximum awake period (micro second)
	 * Maximum awake period between two beacons during
	 * inactivity.
	 */
	unsigned int max_awake;
};

typedef wifi_scan_channel_list_t wlan_scan_channel_list_t;

/** Configuration for wireless scanning v2 */
typedef wifi_scan_params_v2_t wlan_scan_params_v2_t;

int verify_scan_duration_value(int scan_duration);
int verify_scan_channel_value(int channel);
int verify_split_scan_delay(int delay);
int set_scan_params(struct wifi_scan_params_t *wifi_scan_params);
int get_scan_params(struct wifi_scan_params_t *wifi_scan_params);
int wlan_get_current_rssi(short *rssi);
int wlan_get_current_nf();
void get_wep_key(const char *src, uint8_t * dest);

/** Address types to be used by the element wlan_ip_config.addr_type below
 */
enum {
	/** static IP address */
	ADDR_TYPE_STATIC = 0,
	/** Dynamic  IP address*/
	ADDR_TYPE_DHCP = 1,
	/** Link level address */
	ADDR_TYPE_LLA = 2,
};


/** This data structure represents an IPv4 address */
struct ipv4_config {
	/** Set to \ref ADDR_TYPE_DHCP to use DHCP to obtain the IP address or
	 *  \ref ADDR_TYPE_STATIC to use a static IP. In case of static IP
	 *  Address ip, gw, netmask and dns members must be specified.  When
	 *  using DHCP, the ip, gw, netmask and dns are overwritten by the
	 *  values obtained from the DHCP Server. They should be zeroed out if
	 *  not used. */
	unsigned addr_type:2;
	/** The system's IP address in network order. */
	unsigned address;
	/** The system's default gateway in network order. */
	unsigned gw;
	/** The system's subnet mask in network order. */
	unsigned netmask;
	/** The system's primary dns server in network order. */
	unsigned dns1;
	/** The system's secondary dns server in network order. */
	unsigned dns2;
};

#ifdef CONFIG_IPV6
/** This data structure represents an IPv6 address */
struct ipv6_config {
	/** The system's IPv6 address in network order. */
	unsigned address[4];
	 /** The address type: linklocal, site-local or global. */
	unsigned char addr_type;
	/** The state of IPv6 address (Tentative, Preferred, etc). */
	unsigned char addr_state;
};
#endif

/** Network IP configuration.
 *
 *  This data structure represents the network IP configuration
 *  for IPv4 as well as IPv6 addresses
 */
struct wlan_ip_config {
#ifdef CONFIG_IPV6
	/** The network IPv6 address configuration that should be
	 * associated with this interface. */
	struct ipv6_config ipv6[MAX_IPV6_ADDRESSES];
#endif
	/** The network IPv4 address configuration that should be
	 * associated with this interface. */
	struct ipv4_config ipv4;
};

/** WLAN Network Profile
 *
 *  This data structure represents a WLAN network profile. It consists of an
 *  arbitrary name, WiFi configuration, and IP address configuration.
 *
 *  Every network profile is associated with one of the two interfaces. The
 *  network profile can be used for the station interface (i.e. to connect to an
 *  Access Point) by setting the role field to \ref
 *  WLAN_BSS_ROLE_STA. The network profile can be used for the micro-AP
 *  interface (i.e. to start a network of our own.) by setting the mode field to
 *  \ref WLAN_BSS_ROLE_UAP.
 *
 *  If the mode field is \ref WLAN_BSS_ROLE_STA, either of the SSID or
 *  BSSID fields are used to identify the network, while the other members like
 *  channel and security settings characterize the network.
 *
 *  If the mode field is \ref WLAN_BSS_ROLE_UAP, the SSID, channel and security
 *  fields are used to define the network to be started.
 *
 *  In both the above cases, the address field is used to determine the type of
 *  address assignment to be used for this interface.
 */
struct wlan_network {
	/** The name of this network profile.  Each network profile that is
	 *  added to the WLAN Connection Manager must have a unique name. */
	char name[WLAN_NETWORK_NAME_MAX_LENGTH];
	/** The network SSID, represented as a C string of up to 32 characters
	 *  in length.
	 *  If this profile is used in the micro-AP mode, this field is
	 *  used as the SSID of the network.
	 *  If this profile is used in the station mode, this field is
	 *  used to identify the network. Set the first byte of the SSID to NULL
	 *  (a 0-length string) to use only the BSSID to find the network.
	 */
	char ssid[IEEEtypes_SSID_SIZE + 1];
	/*The network SSID for bridge uap*/
	char bridge_ssid[IEEEtypes_SSID_SIZE + 1];
	/** The network BSSID, represented as a 6-byte array.
	 *  If this profile is used in the micro-AP mode, this field is
	 *  ignored.
	 *  If this profile is used in the station mode, this field is
	 *  used to identify the network. Set all 6 bytes to 0 to use any BSSID,
	 *  in which case only the SSID will be used to find the network.
	 */
	char bssid[IEEEtypes_ADDRESS_SIZE];
	/** The channel for this network.
	 *
	 *  If this profile is used in micro-AP mode, this field
	 *  specifies the channel to start the micro-AP interface on. Set this
	 *  to 0 for auto channel selection.
	 *
	 *  If this profile is used in the station mode, this constrains the
	 *  channel on which the network to connect should be present. Set this
	 *  to 0 to allow the network to be found on any channel. */
	unsigned int channel;
	/** BSS type */
	enum wlan_bss_type type;
	/** The network wireless mode enum wlan_bss_role. Set this
	 *  to specify what type of wireless network mode to use.
	 *  This can either be \ref WLAN_BSS_ROLE_STA for use in
	 *  the station mode, or it can be \ref WLAN_BSS_ROLE_UAP
	 *  for use in the micro-AP mode. */
	enum wlan_bss_role role;
	/** The network security configuration specified by struct
	 * wlan_network_security for the network. */
	struct wlan_network_security security;
	/** The network IP address configuration specified by struct
	 * wlan_ip_config that should be associated with this interface. */
	struct wlan_ip_config ip;
#ifdef CONFIG_WPA2_ENTP
	char identity[IDENTITY_MAX_LENGTH];
#ifdef CONFIG_PEAP_MSCHAPV2
	char anonymous_identity[IDENTITY_MAX_LENGTH];
	char password[PASSWORD_MAX_LENGTH];
#endif
#endif

	/* Private Fields */

	/**
	 * If set to 1, the ssid field contains the specific SSID for this
	 * network.
	 * The WLAN Connection Manager will only connect to networks whose SSID
	 * matches.  If set to 0, the ssid field contents are not used when
	 * deciding whether to connect to a network, the BSSID field is used
	 * instead and any network whose BSSID matches is accepted. 
	 *
	 * This field will be set to 1 if the network is added with the SSID
	 * specified (not an empty string), otherwise it is set to 0.
	 */
	unsigned ssid_specific:1;
	unsigned trans_ssid_specific:1;

	/** If set to 1, the bssid field contains the specific BSSID for this 
	 *  network.  The WLAN Connection Manager will not connect to any other 
	 *  network with the same SSID unless the BSSID matches.  If set to 0, the 
	 *  WLAN Connection Manager will connect to any network whose SSID matches.
	 *
	 *  This field will be set to 1 if the network is added with the BSSID
	 *  specified (not set to all zeroes), otherwise it is set to 0. */
	unsigned bssid_specific:1;

	/**
	 * If set to 1, the channel field contains the specific channel for this
	 * network.  The WLAN Connection Manager will not look for this network on
	 * any other channel.  If set to 0, the WLAN Connection Manager will look
	 * for this network on any available channel. 
	 *
	 * This field will be set to 1 if the network is added with the channel
	 * specified (not set to 0), otherwise it is set to 0. */
	unsigned channel_specific:1;
	/**
	 * If set to 0, any security that matches is used. This field is
	 * internally set when the security type parameter above is set to
	 * WLAN_SECURITY_WILDCARD.
	 */
	unsigned security_specific:1;
#ifdef CONFIG_WPS2
	/** This indicates this network is used as an internal network for
	 * WPS */
	unsigned wps_specific:1;
#endif
	/** OWE Transition mode */
	unsigned int owe_trans_mode;
	char trans_ssid[IEEEtypes_SSID_SIZE + 1];
	unsigned int trans_ssid_len;
};

/** WLAN Beacon info
 *
 *  This data structure represents a WLAN Beacon info.
 */
typedef PACK_START struct {
	/** Frame Control flags */
	uint8_t frame_ctrl_flags;
	/** Duration */
	uint16_t duration;
	/** Destination MAC Address */
	char dest[MLAN_MAC_ADDR_LENGTH];
	/** Source MAC Address */
	char src[MLAN_MAC_ADDR_LENGTH];
	/** BSSID MAC Address */
	char bssid[MLAN_MAC_ADDR_LENGTH];
	/** Sequence and Fragmentation number */
	uint16_t seq_frag_num;
	/** Time stamp */
	uint8_t timestamp[8];
	/** Beacon Interval */
	uint16_t beacon_interval;
	/** Capability Information */
	uint16_t cap_info;
	/** SSID Element ID = 00 */
	uint8_t ssid_element_id;
	/** SSID Length */
	uint8_t ssid_len;
	/** SSID */
	char ssid[MLAN_MAX_SSID_LENGTH];
}  PACK_END wlan_beacon_info_t;

/** WLAN Probe Request info
 *
 *  This data structure represents a WLAN Probe Request info.
 */
typedef PACK_START struct {
	/** Frame Control flags */
	uint8_t frame_ctrl_flags;
	/** Duration */
	uint16_t duration;
	/** Destination MAC Address */
	char dest[MLAN_MAC_ADDR_LENGTH];
	/** Source MAC Address */
	char src[MLAN_MAC_ADDR_LENGTH];
	/** BSSID MAC Address */
	char bssid[MLAN_MAC_ADDR_LENGTH];
	/** Sequence and Fragmentation number */
	uint16_t seq_frag_num;
	/** SSID Element ID = 00 */
	uint8_t ssid_element_id;
	/** SSID Length */
	uint8_t ssid_len;
	/** SSID */
	char ssid[MLAN_MAX_SSID_LENGTH];
}  PACK_END wlan_probe_req_info_t;

/** WLAN Probe Request info
 *
 *  This data structure represents a WLAN Probe Request info.
 */
typedef PACK_START struct {
	/** Frame Control flags */
	uint8_t frame_ctrl_flags;
	/** Duration */
	uint16_t duration;
	/** Destination MAC Address */
	char dest[MLAN_MAC_ADDR_LENGTH];
	/** Source MAC Address */
	char src[MLAN_MAC_ADDR_LENGTH];
	/** BSSID MAC Address */
	char bssid[MLAN_MAC_ADDR_LENGTH];
	/** Sequence and Fragmentation number */
	uint16_t seq_frag_num;
}  PACK_END wlan_auth_info_t;

/** WLAN Data info
 *
 *  This data structure represents a WLAN Data info.
 */
typedef PACK_START struct {
	/** Frame Control flags */
	uint8_t frame_ctrl_flags;
	/** Duration */
	uint16_t duration;
	/** Destination MAC Address */
	char dest[MLAN_MAC_ADDR_LENGTH];
	/** BSSID MAC Address */
	char bssid[MLAN_MAC_ADDR_LENGTH];
	/** Source MAC Address */
	char src[MLAN_MAC_ADDR_LENGTH];
	/** Sequence and Fragmentation number */
	uint16_t seq_frag_num;
	/** QoS Control bits */
	uint16_t qos_ctrl;
} PACK_END wlan_data_info_t;


/** WLAN Frame type */
typedef enum {
	/* Association Request */
	ASSOC_REQ = 0x00,
	/* Association Response */
	ASSOC_RESP = 0x10,
	/* Re-association Request */
	REASSOC_REQ = 0x20,
	/* Re-association Response */
	REASSOC_RESP = 0x30,
	/** Probe Request */
	PROBE_REQ = 0x40,
	/** Probe Response */
	PROBE_RESP = 0x50,
	/** Beacon */
	BEACON = 0x80,
	/** Disassociation */
	DISASSOC = 0xA0,
	/** Auth Request */
	AUTH = 0xB0,
	/** De-auth */
	DEAUTH = 0xC0,
	/** Action */
	ACTION = 0xD0,
	/** Data */
	DATA = 0x08,
	/** QOS Data */
	QOS_DATA = 0x88,
} wlan_frame_type_t;

/** WLAN Frame info
 *
 *  This data structure represents a WLAN Frame info.
 */
typedef PACK_START struct {
	/** WLAN Frame type */
	wlan_frame_type_t frame_type;
	union {
		/** WLAN Beacon info */
		wlan_beacon_info_t beacon_info;
		/** WLAN Probe Request info */
		wlan_probe_req_info_t probe_req_info;
		/** WLAN Auth info */
		wlan_auth_info_t auth_info;
		/** WLAN Data info */
		wlan_data_info_t data_info;
	} frame_data;
} PACK_END wlan_frame_t;

typedef wifi_mgmt_frame_t wlan_mgmt_frame_t;

typedef wifi_cal_data_t wlan_cal_data_t;

typedef wifi_auto_reconnect_config_t wlan_auto_reconnect_config_t;

/* WLAN Connection Manager API */

/** Initialize the SDIO driver and create the wifi driver thread.
 *
 * \param[in]  fl Flash descriptor of the WLAN firmware on flash.
 * 	       \ref wm_wlan_init function describes how to derive
 * 	       flash descriptor.
 *
 * \return WM_SUCCESS if the WLAN Connection Manager service has
 *         initialized successfully.
 * \return Negative value if Initialization failed.
 */
int wlan_init(flash_desc_t *fl);

/** Start the WLAN Connection Manager service.
 *
 * This function starts the WLAN Connection Manager.
 *
 * \note The status of the WLAN Connection Manager is notified asynchronously
 * through the callback, \a cb, with a WLAN_REASON_INITIALIZED event
 * (if initialization succeeded) or WLAN_REASON_INITIALIZATION_FAILED
 * (if initialization failed).
 *
 * \note If the WLAN Connection Manager fails to initialize, the caller should
 * stop WLAN Connection Manager via wlan_stop() and try wlan_start() again.
 *
 * \param[in] cb A pointer to a callback function that handles WLAN events. All
 * further WLCMGR events will be notified in this callback. Refer to enum
 * \ref wlan_event_reason for the various events for which this callback is called.
 *
 * \return WM_SUCCESS if the WLAN Connection Manager service has started
 *         successfully.  
 * \return -WM_E_INVAL if the \a cb pointer is NULL.
 * \return -WM_FAIL if an internal error occurred.
 * \return WLAN_ERROR_STATE if the WLAN Connection Manager is already running.
 */
int wlan_start(int (*cb) (enum wlan_event_reason reason, void *data));

/** Stop the WLAN Connection Manager service.
 *
 *  This function stops the WLAN Connection Manager, causing station interface
 *  to disconnect from the currently connected network and stop the
 *  micro-AP interface.
 *
 *  \return WM_SUCCESS if the WLAN Connection Manager service has been
 *          stopped successfully.  
 *  \return WLAN_ERROR_STATE if the WLAN Connection Manager was not
 *          running.
 */
int wlan_stop(void);

/** Deinitialize SDIO driver, send shutdown command to WLAN firmware
 *  and delete the wifi driver thread.
 *  \param action Additional Action to be taken with deinit
 *			WLAN_PDN: for WLAN power down
 *			WLAN_ACTIVE: no action to be taken
 */
void wlan_deinit(int action);

/** WLAN initialize micro-AP network information
 *
 * This API intializes a default micro-AP network. The network ssid, passphrase
 * is initialized to NULL. Channel is set to auto. The IP Address of the
 * micro-AP interface is 192.168.10.1/255.255.255.0. Network name is set to
 * 'uap-network'.
 *
 * \param[out] net Pointer to the initialized micro-AP network
 */
void wlan_initialize_uap_network(struct wlan_network *net);

/** Add a network profile to the list of known networks.
 *
 *  This function copies the contents of \a network to the list of known
 *  networks in the WLAN Connection Manager.  The network's 'name' field must be
 *  unique and between \ref WLAN_NETWORK_NAME_MIN_LENGTH and \ref
 *  WLAN_NETWORK_NAME_MAX_LENGTH characters.  The network must specify at least
 *  an SSID or BSSID.  The WLAN Connection Manager may store up to
 *  WLAN_MAX_KNOWN_NETWORKS networks.
 *
 *  \note Profiles for the station interface may be added only when the station
 *  interface is in the \ref WLAN_DISCONNECTED or \ref WLAN_CONNECTED state.
 *
 *  \note This API can be used to add profiles for station or 
 *  micro-AP interfaces.
 *  
 *  \param[in] network A pointer to the \ref wlan_network that will be copied 
 *             to the list of known networks in the WLAN Connection Manager 
 *             successfully.
 *
 *  \return WM_SUCCESS if the contents pointed to by \a network have been
 *          added to the WLAN Connection Manager.  
 *  \return -WM_E_INVAL if \a network is NULL or the network name
 *          is not unique or the network name length is not valid
 *          or if station network profile is not a WPA2 Enterprise
 *          network if API \ref wlan_sta_connect_wpa2_enterprise_ap_only
 *          is used in WPA2 enterprise case.
 *  \return -WM_E_NOMEM if there was no room to add the network.
 *  \return WLAN_ERROR_STATE if the WLAN Connection Manager
 *          was running and not in the \ref WLAN_DISCONNECTED,
 *          \ref WLAN_ASSOCIATED or \ref WLAN_CONNECTED state.
 */
int wlan_add_network(struct wlan_network *network);

/** Remove a network profile from the list of known networks.
 *
 *  This function removes a network (identified by its name) from the WLAN
 *  Connection Manager, disconnecting from that network if connected.
 *
 *  \note This function is asynchronous if it is called while the WLAN
 *  Connection Manager is running and connected to the network to be removed.
 *  In that case, the WLAN Connection Manager will disconnect from the network
 *  and generate an event with reason \ref WLAN_REASON_USER_DISCONNECT. This
 *  function is synchronous otherwise.
 *
 *  \note This API can be used to remove profiles for station or
 *  micro-AP interfaces. Station network will not be removed if it is
 *  in \ref WLAN_CONNECTED state and uAP network will not be removed
 *  if it is in \ref WLAN_UAP_STARTED state.
 *
 *  \param[in] name A pointer to the string representing the name of the
 *             network to remove.
 *
 *  \return WM_SUCCESS if the network named \a name was removed from the
 *          WLAN Connection Manager successfully. Otherwise,
 *          the network is not removed.
 *  \return WLAN_ERROR_STATE if the WLAN Connection Manager was 
 *          running and the station interface was not in the \ref
 *          WLAN_DISCONNECTED state.
 *  \return -WM_E_INVAL if \a name is NULL or the network was not found in
 *          the list of known networks.  
 *  \return -WM_FAIL if an internal error occurred
 *          while trying to disconnect from the network specified for
 *          removal. 
 */
int wlan_remove_network(const char *name);

/** Connect to a wireless network (Access Point).
 *
 *  When this function is called, WLAN Connection Manager starts connection attempts
 *  to the network specified by \a name. The connection result will be notified
 *  asynchronously to the WLCMGR callback when the connection process has
 *  completed.
 *
 *  When connecting to a network, the event refers to the connection
 *  attempt to that network.
 *
 *  Calling this function when the station interface is in the \ref
 *  WLAN_DISCONNECTED state will, if successful, cause the interface to
 *  transition into the \ref WLAN_CONNECTING state.  If the connection attempt
 *  succeeds, the station interface will transition to the \ref WLAN_CONNECTED state,
 *  otherwise it will return to the \ref WLAN_DISCONNECTED state.  If this
 *  function is called while the station interface is in the \ref
 *  WLAN_CONNECTING or \ref WLAN_CONNECTED state, the WLAN Connection Manager
 *  will first cancel its connection attempt or disconnect from the network,
 *  respectively, and generate an event with reason \ref
 *  WLAN_REASON_USER_DISCONNECT. This will be followed by a second event that
 *  reports the result of the new connection attempt.
 *
 *  If the connection attempt was successful the WLCMGR callback is notified
 *  with the event \ref WLAN_REASON_SUCCESS, while if the connection attempt
 *  fails then either of the events, \ref WLAN_REASON_NETWORK_NOT_FOUND, \ref
 *  WLAN_REASON_NETWORK_AUTH_FAILED, \ref WLAN_REASON_CONNECT_FAILED
 *  or \ref WLAN_REASON_ADDRESS_FAILED are reported as appropriate.
 *
 *  \param[in] name A pointer to a string representing the name of the network
 *              to connect to.
 *
 *  \return WM_SUCCESS if a connection attempt was started successfully
 *  \return WLAN_ERROR_STATE if the WLAN Connection Manager was not running.  
 *  \return -WM_E_INVAL if there are no known networks to connect to
 *          or the network specified by \a name is not in the list
 *          of known networks or network \a name is NULL.
 *  \return -WM_FAIL if an internal error has occurred.
 */
int wlan_connect(char *name);

/** Disconnect from the current wireless network (Access Point).
 *
 *  When this function is called, the WLAN Connection Manager attempts to disconnect 
 *  the station interface from its currently connected network (or cancel an in-progress
 *  connection attempt) and return to the \ref WLAN_DISCONNECTED state. Calling
 *  this function has no effect if the station interface is already
 *  disconnected.
 *
 *  \note This is an asynchronous function and successful disconnection will be
 *  notified using the \ref WLAN_REASON_USER_DISCONNECT. 
 *
 * \return  WM_SUCCESS if successful
 * \return  WLAN_ERROR_STATE otherwise
 */
int wlan_disconnect(void);

/** Start a wireless network (Access Point).
 *
 *  When this function is called, the WLAN Connection Manager starts the network
 *  specified by \a name. The network with the specified \a name must be 
 *  first added using \ref wlan_add_network and must be a micro-AP network with
 *  a valid SSID. 
 *
 *  \note The WLCMGR callback is asynchronously notified of the status. On
 *  success, the event \ref WLAN_REASON_UAP_SUCCESS is reported, while on
 *  failure, the event \ref WLAN_REASON_UAP_START_FAILED is reported.
 *
 *  \param[in] name A pointer to string representing the name of the network
 *             to connect to.
 *
 *  \return WM_SUCCESS if successful.
 *  \return WLAN_ERROR_STATE if in power save state or uAP already running.
 *  \return -WM_E_INVAL if \a name was NULL or the network \a
 *          name was not found or it not have a specified SSID.
 */
int wlan_start_network(const char *name);

/** Stop a wireless network (Access Point).
 *
 *  When this function is called, the WLAN Connection Manager stops the network
 *  specified by \a name. The specified network must be a valid micro-AP
 *  network that has already been started.
 *
 *  \note The WLCMGR callback is asynchronously notified of the status. On
 *  success, the event \ref WLAN_REASON_UAP_STOPPED is reported, while on
 *  failure, the event \ref WLAN_REASON_UAP_STOP_FAILED is reported.
 *
 *  \param[in] name A pointer to a string representing the name of the network
 *             to stop.
 *
 *  \return WM_SUCCESS if successful.
 *  \return WLAN_ERROR_STATE if uAP is in power save state.
 *  \return -WM_E_INVAL if \a name was NULL or the network \a
 *          name was not found or that the network \a name is not a micro-AP
 *          network or it is a micro-AP network but does not have a specified
 *          SSID.
 */
int wlan_stop_network(const char *name);

/** Retrieve the wireless MAC address of station/micro-AP interface.
 *
 *  This function copies the MAC address of the wireless interface to
 *  the 6-byte array pointed to by \a dest.  In the event of an error, nothing 
 *  is copied to \a dest.
 *
 *  \param[out] dest A pointer to a 6-byte array where the MAC address will be
 *              copied.
 *
 *  \return WM_SUCCESS if the MAC address was copied.
 *  \return -WM_E_INVAL if \a dest is NULL.
 */
int wlan_get_mac_address(uint8_t *dest);

/** Retrieve the wireless MAC address of Wi-Fi Direct interface.
 *
 *  This function copies the MAC address of the wireless interface to
 *  the 6-byte array pointed to by \a dest.  In the event of an error, nothing 
 *  is copied to \a dest.
 *
 *  \param[out] dest A pointer to a 6-byte array where the MAC address will be
 *              copied.
 *
 *  \return WM_SUCCESS if the MAC address was copied.
 *  \return -WM_E_INVAL if \a dest is NULL.
 */
int wlan_get_wfd_mac_address(uint8_t *dest);

/** Retrieve the IP address configuration of the station interface.
 *
 *  This function retrieves the IP address configuration 
 *  of the station interface and copies it to the memory 
 *  location pointed to by \a addr.
 * 
 *  \note This function may only be called when the station interface is in the
 *  \ref WLAN_CONNECTED state.
 *
 *  \param[out] addr A pointer to the \ref wlan_ip_config.
 *
 *  \return WM_SUCCESS if successful.
 *  \return -WM_E_INVAL if \a addr is NULL.
 *  \return WLAN_ERROR_STATE if the WLAN Connection Manager was not running or was 
 *          not in the \ref WLAN_CONNECTED state. 
 *  \return -WM_FAIL if an internal error
 *          occurred when retrieving IP address information from the
 *          TCP stack. 
 */
int wlan_get_address(struct wlan_ip_config *addr);

/** Retrieve the IP address of micro-AP interface.
 *
 *  This function retrieves the current IP address configuration of micro-AP
 *  and copies it to the memory location pointed to by \a addr.
 *
 *  \note This function may only be called when the micro-AP interface is in the
 *  \ref WLAN_UAP_STARTED state.
 *
 *  \param[out] addr A pointer to the \ref wlan_ip_config.
 *
 *  \return WM_SUCCESS if successful.
 *  \return -WM_E_INVAL if \a addr is NULL.
 *  \return WLAN_ERROR_STATE if the WLAN Connection Manager was not running or
 *          the micro-AP interface was not in the \ref WLAN_UAP_STARTED state.
 *  \return -WM_FAIL if an internal error
 *          occurred when retrieving IP address information from the
 *          TCP stack.
 */
int wlan_get_uap_address(struct wlan_ip_config *addr);

/** Retrieve the IP address configuration of the Wi-Fi Direct interface.
 *
 *  This function retrieves the IP address configuration 
 *  of the Wi-Fi Direct interface and copies it to the memory 
 *  location pointed to by \a addr.
 *
 *  \note This function may only be called when the Wi-Fi Direct interface is in the
 *  \ref WLAN_CONNECTED or WLAN_P2P_STARTED state.
 * 
 *  \param[out] addr A pointer to the \ref wlan_ip_config.
 *
 *  \return WM_SUCCESS if successful.
 *  \return -WM_E_INVAL if \a addr is NULL.
 *  \return WLAN_ERROR_STATE if the WLAN Connection Manager was not running
 *  	    or was client and not in the \ref WLAN_CONNECTED or was GO and not in
 *  	    \ref WLAN_P2P_STARTED state.
 *  \return -WM_FAIL if an internal error
 *          occurred when retrieving IP address information from the
 *          TCP stack.
 */
int wlan_get_wfd_address(struct wlan_ip_config *addr);

/** Retrieve the current network configuration of station interface.
 *
 *  This function retrieves the current network configuration of station
 *  interface when the station interface is in the \ref WLAN_CONNECTED
 *  state.
 *
 *  \param[out] network A pointer to the \ref wlan_network. 
 *
 *  \return WM_SUCCESS if successful.
 *  \return -WM_E_INVAL if \a network is NULL.
 *  \return WLAN_ERROR_STATE if the WLAN Connection Manager was
 *          not running or not in the \ref WLAN_CONNECTED state.
 */
int wlan_get_current_network(struct wlan_network *network);

/** Retrieve the current network configuration of micro-AP interface.
 *
 *  This function retrieves the current network configuration of micro-AP
 *  interface when the micro-AP interface is in the \ref WLAN_UAP_STARTED state.
 *
 *  \param[out] network A pointer to the \ref wlan_network.
 *
 *  \return WM_SUCCESS if successful.
 *  \return -WM_E_INVAL if \a network is NULL.
 *  \return WLAN_ERROR_STATE if the WLAN Connection Manager was
 *           not running or not in the \ref WLAN_UAP_STARTED state.
 */
int wlan_get_current_uap_network(struct wlan_network *network);

/** Retrieve the current network configuration of Wi-Fi Direct interface.
 *
 *  This function retrieves the current network configuration of Wi-Fi Direct
 *  interface when the the as client interface is in the \ref WLAN_CONNECTED
 *  state or as a GO interface is in \ref WLAN_P2P_STARTED state.
 *
 *  \param[out] network A pointer to the \ref wlan_network. 
 *
 *  \return WM_SUCCESS if successful.
 *  \return -WM_E_INVAL if \a network is NULL.
 *  \return WLAN_ERROR_STATE if the WLAN Connection Manager was
 *          not running or when as a client not in the \ref 
 *          WLAN_CONNECTED state or as a GO not in \ref WLAN_P2P_STARTED state.
 */
int wlan_get_current_wfd_network(struct wlan_network *network);

/** Retrieve the status information of the micro-AP interface.
 *
 *  \return TRUE if micro-AP interface is in \ref WLAN_UAP_STARTED state.
 *  \return FALSE otherwise. 
 */
int is_uap_started(void);

/** Retrieve the status information of the station interface.
 *
 *  \return TRUE if station interface is in \ref WLAN_CONNECTED state.
 *  \return FALSE otherwise.
 */
bool is_sta_connected(void);

/** Retrieve the status information of the ipv4 network of station interface.
 *
 *  \return TRUE if ipv4 network of station interface is in \ref WLAN_CONNECTED
 *  state.
 *  \return FALSE otherwise.
 */
bool is_sta_ipv4_connected(void);

#ifdef CONFIG_IPV6
/** Retrieve the status information of the ipv6 network of station interface.
 *
 *  \return TRUE if ipv6 network of station interface is in \ref WLAN_CONNECTED
 *  state.
 *  \return FALSE otherwise.
 */
bool is_sta_ipv6_connected(void);
#endif

/** Retrieve the status information of the Wi-Fi Direct interface.
 *
 *  \return TRUE if Wi-Fi Direct interface is in \ref WLAN_P2P_STARTED state.
 *  \return FALSE otherwise. 
 */
int is_wfd_started(void);

/** Retrieve the information about a known network using \a index.
 *
 *  This function retrieves the contents of a network at \a index in the
 *  list of known networks maintained by the WLAN Connection Manager and
 *  copies it to the location pointed to by \a network.  
 *
 *  \note \ref wlan_get_network_count() may be used to retrieve the number 
 *  of known networks. \ref wlan_get_network() may be used to retrieve 
 *  information about networks at \a index 0 to one minus the number of networks.
 *
 *  \note This function may be called regardless of whether the WLAN Connection
 *  Manager is running. Calls to this function are synchronous.
 *
 *  \param[in] index The index of the network to retrieve.
 *  \param[out] network A pointer to the \ref wlan_network where the network
 *              configuration for the network at \a index will be copied.
 *
 *  \returns WM_SUCCESS if successful.
 *  \return -WM_E_INVAL if \a network is NULL or \a index is out of range.
 */
int wlan_get_network(unsigned int index, struct wlan_network *network);

/** Retrieve information about a known network using \a name.
 *
 *  This function retrieves the contents of a named network in the
 *  list of known networks maintained by the WLAN Connection Manager and
 *  copies it to the location pointed to by \a network.  
 *
 *  \note This function may be called regardless of whether the WLAN Connection
 *  Manager is running. Calls to this function are synchronous.
 *
 *  \param[in] name The name of the network to retrieve.
 *  \param[out] network A pointer to the \ref wlan_network where the network
 *              configuration for the network having name as \a name will be copied.
 *
 *  \return WM_SUCCESS if successful.
 *  \return -WM_E_INVAL if \a network is NULL or \a name is NULL.
 */
int wlan_get_network_byname(char *name, struct wlan_network *network);

/** Retrieve the number of networks known to the WLAN Connection Manager.
 *
 *  This function retrieves the number of known networks in the list maintained
 *  by the WLAN Connection Manager and copies it to \a count.
 *
 *  \note This function may be called regardless of whether the WLAN Connection
 *  Manager is running. Calls to this function are synchronous.
 *
 *  \param[out] count A pointer to the memory location where the number of
 *              networks will be copied.
 *
 *  \return WM_SUCCESS if successful.
 *  \return -WM_E_INVAL if \a count is NULL.
 */
int wlan_get_network_count(unsigned int *count);

/** Retrieve the connection state of station interface.
 *
 *  This function retrieves the connection state of station interface, which is
 *  one of \ref WLAN_DISCONNECTED, \ref WLAN_CONNECTING, \ref WLAN_ASSOCIATED
 *  or \ref WLAN_CONNECTED.
 *
 *  \param[out] state A pointer to the \ref wlan_connection_state where the
 *         current connection state will be copied.
 *
 *  \return WM_SUCCESS if successful.
 *  \return -WM_E_INVAL if \a state is NULL
 *  \return WLAN_ERROR_STATE if the WLAN Connection Manager was not running.
 */
int wlan_get_connection_state(enum wlan_connection_state *state);

/** Retrieve the connection state of micro-AP interface.
 *
 *  This function retrieves the connection state of micro-AP interface, which is
 *  one of \ref WLAN_UAP_STARTED, or \ref WLAN_UAP_STOPPED.
 *
 *  \param[out] state A pointer to the \ref wlan_connection_state where the
 *         current connection state will be copied.
 *
 *  \return WM_SUCCESS if successful.
 *  \return -WM_E_INVAL if \a state is NULL
 *  \return WLAN_ERROR_STATE if the WLAN Connection Manager was not running.
 */
int wlan_get_uap_connection_state(enum wlan_connection_state *state);

/** Retrieve the connection state of Wi-Fi Direct interface.
 *
 *  This function retrieves the connection state of Wi-Fi Direct interface,
 *  which is one of \ref WLAN_P2P_STARTED, or \ref WLAN_P2P_STOPPED.
 *
 *  \param[out] state A pointer to the \ref wlan_connection_state where the
 *         current connection state will be copied.
 *
 *  \return WM_SUCCESS if successful.
 *  \return -WM_E_INVAL if \a state is NULL
 *  \return WLAN_ERROR_STATE if the WLAN Connection Manager was not running.
 */
int wlan_get_wfd_connection_state(enum wlan_connection_state *state);

/** Scan for wireless networks.
 *
 *  When this function is called, the WLAN Connection Manager starts scan 
 *  for wireless networks. On completion of the scan the WLAN Connection Manager 
 *  will call the specified callback function \a cb. The callback function can then
 *  retrieve the scan results by using the \ref wlan_get_scan_result() function.
 *
 *  \note This function may only be called when the station interface is in the
 *  \ref WLAN_DISCONNECTED or \ref WLAN_CONNECTED state. Scanning is disabled
 *  in the \ref WLAN_CONNECTING state.
 *
 *  \note This function will block until it can issue a scan request if called
 *  while another scan is in progress.
 *
 *  \param[in] cb A pointer to the function that will be called to handle scan
 *         results when they are available.
 *
 *  \return WM_SUCCESS if successful.
 *  \return -WM_E_NOMEM if failed to allocated memory for \ref
 *	     wlan_scan_params_v2_t structure.
 *  \return -WM_E_INVAL if \a cb scan result callack functio pointer is NULL.
 *  \return WLAN_ERROR_STATE if the WLAN Connection Manager was
 *           not running or not in the \ref WLAN_DISCONNECTED or \ref
 *           WLAN_CONNECTED states.
 *  \return -WM_FAIL if an internal error has occurred and
 *           the system is unable to scan.
 */
int wlan_scan(int (*cb) (unsigned int count));

/** Scan for wireless networks using options provided.
 *
 *  When this function is called, the WLAN Connection Manager starts scan
 *  for wireless networks. On completion of the scan the WLAN Connection Manager
 *  will call the specified callback function \a cb. The callback function
 *  can then retrieve the scan results by using the \ref wlan_get_scan_result()
 *  function.
 *
 *  \note This function may only be called when the station interface is in the
 *  \ref WLAN_DISCONNECTED or \ref WLAN_CONNECTED state. Scanning is disabled
 *  in the \ref WLAN_CONNECTING state.
 *
 *  \note This function will block until it can issue a scan request if called
 *  while another scan is in progress.
 *
 *  \param[in] wlan_scan_param  A \ref wlan_scan_params_v2_t structure holding
 *	       a pointer to function that will be called
 *	       to handle scan results when they are available,
 *	       SSID of a wireless network, BSSID of a wireless network,
 *	       no of channels with scan type information and number of
 *	       probes.
 *
 *  \return WM_SUCCESS if successful.
 *  \return -WM_E_NOMEM if failed to allocated memory for \ref
 *	     wlan_scan_params_v2_t structure.
 *  \return -WM_E_INVAL if \a cb scan result callack functio pointer is NULL.
 *  \return WLAN_ERROR_STATE if the WLAN Connection Manager was
 *           not running or not in the \ref WLAN_DISCONNECTED or \ref
 *           WLAN_CONNECTED states.
 *  \return -WM_FAIL if an internal error has occurred and
 *           the system is unable to scan.
 */
int wlan_scan_with_opt(wlan_scan_params_v2_t wlan_scan_param);

/** Retrieve a scan result.
 *
 *  This function may be called to retrieve scan results when the WLAN
 *  Connection Manager has finished scanning. It must be called from within the
 *  scan result callback (see \ref wlan_scan()) as scan results are valid 
 *  only in that context. The callback argument 'count' provides the number 
 *  of scan results that may be retrieved and \ref wlan_get_scan_result() may 
 *  be used to retrieve scan results at \a index 0 through that number.
 *
 *  \note This function may only be called in the context of the scan results
 *  callback.
 *
 *  \note Calls to this function are synchronous.
 *
 *  \param[in] index The scan result to retrieve.
 *  \param[out] res A pointer to the \ref wlan_scan_result where the scan
 *              result information will be copied.
 *
 *  \return WM_SUCCESS if successful.
 *  \return -WM_E_INVAL if \a res is NULL
 *  \return WLAN_ERROR_STATE if the WLAN Connection Manager
 *          was not running
 *  \return -WM_FAIL if the scan result at \a
 *          index could not be retrieved (that is, \a index 
 *          is out of range).
 */
int wlan_get_scan_result(unsigned int index, struct wlan_scan_result *res);

#ifdef CONFIG_WiFi_8801
/**
 * Enable Low Power Mode in Wireless Firmware.
 *
 * \note When low power mode is enabled, the output power will be clipped at
 * ~+10dBm and the expected PA current is expected to be in the 80-90 mA
 * range for b/g/n modes.
 *
 * This function may be called to enable low power mode in firmware.
 * This should be call before \ref wlan_init() function.
 *
 * \return WM_SUCCESS if the call was successful.
 * \return -WM_FAIL if failed.
 *
 */
int wlan_enable_low_pwr_mode();
#endif

/**
 * Enable ED MAC mode in Wireless Firmware.
 *
 * \note When ed mac mode is enabled,
 * Wireless Firmware will behave following way:
 *
 * when background noise had reached -70dB or above,
 * WiFi chipset/module should hold data transmitting
 * until condition is removed.
 * It is applicable for both 5GHz and 2.4GHz bands.
 *
 * This function may be called to enable ed mac mode in firmware.
 * This should be call before \ref wlan_init() function.
 *
 * \param[in] offset Offset value can be 0 no offset, 0xffff offset
 *		     set to -1 or 1 offset set to 1.
 *
 * \return WM_SUCCESS if the call was successful.
 * \return -WM_FAIL if failed.
 *
 */
int wlan_enable_ed_mac_mode(short offset);

/** Set wireless calibration data in WLAN firmware.
 *
 * This function may be called to set wireless calibration data in firmware.
 * This should be call before \ref wlan_init() function.
 *
 * \param[in] cal_data The calibration data buffer
 * \param[in] cal_data_size Size of calibration data buffer.
 *
 */
void wlan_set_cal_data(uint8_t *cal_data, unsigned int cal_data_size);

/** Set wireless MAC Address in WLAN firmware.
 *
 * This function may be called to set wireless MAC Address in firmware.
 * This should be call before \ref wlan_init() function.
 *
 * \param[in] mac The MAC Address in 6 byte array format like
 *                uint8_t mac[] = { 0x00, 0x50, 0x43, 0x21, 0x19, 0x6E};
 *
 */
void wlan_set_mac_addr(uint8_t *mac);

/** Configure Listen interval of IEEE power save mode.
 *
 * \note <b>Delivery Traffic Indication Message (DTIM)</b>:
 * It is a concept in 802.11
 * It is a time duration after which AP will send out buffered
 * BROADCAST / MULTICAST data and stations connected to the AP
 * should wakeup to take this broadcast / multicast data.

 * \note <b>Traffic Indication Map (TIM)</b>:
 * It is a bitmap which the AP sends with each beacon.
 * The bitmap has one bit each for a station connected to AP.
 *
 * \note Each station is recognized by an Association Id (AID).
 * If AID is say 1 bit number 1 is set in the bitmap if
 * unicast data is present with AP in its buffer for station with AID = 1
 * Ideally AP does not buffer any unicast data it just sends
 * unicast data to the station on every beacon when station
 * is not sleeping.
 * When broadcast data / multicast data is to be send AP sets bit 0
 * of TIM indicating broadcast / multicast.
 * The occurrence of DTIM is defined by AP.
 * Each beacon has a number indicating period at which DTIM occurs.
 * The number is expressed in terms of number of beacons.
 * This period is called DTIM Period / DTIM interval.
 * For example:
 *     If AP has DTIM period = 3 the stations connected to AP
 *     have to wake up (if they are sleeping) to receive
 *     broadcast /multicast data on every third beacon.
 * Generic:
 *     When DTIM period is X
 *     AP buffers broadcast data / multicast data for X beacons.
 *     Then it transmits the data no matter whether station is awake or not.
 * Listen interval:
 * This is time interval on station side which indicates when station
 * will be awake to listen i.e. accept data.
 * Long listen interval:
 * It comes into picture when station sleeps (IEEEPS) and it does
 * not want to wake up on every DTIM
 * So station is not worried about broadcast data/multicast data
 * in this case.
 * This should be a design decision what should be chosen
 * Firmware suggests values which are about 3 times DTIM
 * at the max to gain optimal usage and reliability.
 * In the IEEEPS power save mode, the WiFi firmware goes to sleep and
 * periodically wakes up to check if the AP has any pending packets for it. A
 * longer listen interval implies that the WiFi card stays in power save for a
 * longer duration at the cost of additional delays while receiving data.
 * Please note that choosing incorrect value for listen interval will
 * causes poor response from device during data transfer.
 * Actual listen interval selected by firmware is equal to closest DTIM.
 * For e.g.:-
 *            AP beacon period : 100 ms
 *            AP DTIM period : 2
 *            Application request value: 500ms
 *            Actual listen interval = 400ms (This is the closest DTIM).
 * Actual listen interval set will be a multiple of DTIM closest to but
 * lower than the value provided by the application.
 *
 *  \note This API can be called before/after association.
 *  The configured listen interval will be used in subsequent association
 *  attempt.
 *  
 *  \param [in]  listen_interval Listen interval as below
 *               0 : Unchanged,
 *              -1 : Disable,
 *             1-49: Value in beacon intervals,
 *            >= 50: Value in TUs
 */
void wlan_configure_listen_interval(int listen_interval);

/** Configure Null packet interval of IEEE power save mode.
 *
 *  \note In IEEEPS station sends a NULL packet to AP to indicate that
 *  the station is alive and AP should not kick it off.
 *  If null packet is not send some APs may disconnect station
 *  which might lead to a loss of connectivity.
 *  The time is specified in seconds.
 *  Default value is 30 seconds.
 *
 *  \note This API should be called before configuring IEEEPS
 *
 *  \param [in] time_in_secs : -1 Disables null packet transmission,
 *                              0  Null packet interval is unchanged,
 *                              n  Null packet interval in seconds.
 */
void wlan_configure_null_pkt_interval(int time_in_secs);

/** Turn on Deep Sleep Power Save mode.
 *
 * \note This call is asynchronous. The system will enter the power-save mode
 * only when all requisite conditions are met. For example, wlan should be
 * disconnected for this to work.
 *
 * \return WM_SUCCESS if the call was successful.
 * \return WLAN_ERROR_STATE if the call was made in a state where such an
 *         operation is illegal.
 */
int wlan_deepsleepps_on();

/** Turn off Deep Sleep Power Save mode.
 *
 * \note This call is asynchronous. The system will exit the power-save mode
 *       only when all requisite conditions are met.
 *
 * \return WM_SUCCESS if the call was successful.
 * \return WLAN_ERROR_STATE if the call was made in a state where such an
 *         operation is illegal.
 */
int wlan_deepsleepps_off();

/** Turn on Power Down mode.
 *
 * \note This call is asynchronous. The system will enter the power-save mode
 * only when all requisite conditions are met.  For example, wlan should be
 * disconnected for this to work.
 *
 * \return WM_SUCCESS if the call was successful.
 * \return WLAN_ERROR_STATE if the call was made in a state where such an
 *         operation is illegal.
 */
int wlan_pdnps_on();

/** Turn off Power Down mode.
 *
 * \note This call is asynchronous. The system will exit the power-save mode
 *       only when all requisite conditions are met.
 *
 * \return WM_SUCCESS if the call was successful.
 * \return WLAN_ERROR_STATE if the call was made in a state where such an
 *         operation is illegal.
 */
int wlan_pdnps_off();

/** This function gets current antenna.
 *
 * \param[out] ant Pointer to antenna variable.
 *
 * \return WM_SUCCESS if successful.
 * \return WLAN_ERROR_STATE if unsuccessful.
 */
int wlan_get_current_ant(unsigned int *ant);

/** This function sets current antenna.
 *
 * \param[in] ant Antenna
 *            Valid values are 1, 2 and 65535.
 *            1: Set Antenna 1,
 *            2: Set Antenna 2,
 *        65535: Set Antenna diversity.
 *
 * \return WM_SUCCESS if successful.
 * \return WLAN_ERROR_STATE if unsuccessful.
 */
int wlan_set_current_ant(unsigned int ant);

/** Get the wifi firmware version extension string.
 *
 * \note This API does not allocate memory for pointer,
 *       It just returns pointer of WLCMGR internal static
 *       buffer, So no need to free the pointer by caller.
 *
 * \return wifi firmware version extension string pointer stored in
 *         WLCMGR
 */
char *wlan_get_firmware_version_ext();

/** Get station interface power save mode.
 *
 * \param[out] ps_mode A pointer to \ref wlan_ps_mode where station interface
 * 	      power save mode will be stored.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_E_INVAL if \a ps_mode was NULL.
 */
int wlan_get_ps_mode(enum wlan_ps_mode *ps_mode);

/** Get micro-AP interface power save mode.
 *
 * \param[out] ps_mode A pointer to \ref wlan_uap_ps_mode where micro-AP 
 * 	       power save mode will be stored.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_E_INVAL if \a ps_mode was NULL.
 */
int wlan_get_uap_ps_mode(enum wlan_uap_ps_mode *ps_mode);

/** Get Wi-Fi Direct interface power save mode.
 *
 * \param[out] ps_mode A pointer to \ref wlan_wfd_ps_mode where Wi-Fi Direct 
 * 	       interface power save mode will be stored.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_E_INVAL if \a ps_mode was NULL.
 */
int wlan_get_wfd_ps_mode(enum wlan_wfd_ps_mode *ps_mode);

/** Send message to WLAN Connection Manager thread.
 *
 * \param[in] event An event from \ref wifi_event.
 * \param[in] reason A reason code.
 * \param[in] data A pointer to data buffer associated with event. 
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if failed.
 */
int wlan_wlcmgr_send_msg(enum wifi_event event, int reason,
			 void *data);

/** Register basic WLAN CLI commands
 *
 * This function registers basic WLAN CLI commands like showing
 * version information, MAC address
 *
 * \note This function can only be called by the application after
 *  \ref wlan_init() called.
 *
 * \note This function gets called by \ref wlan_cli_init(), hence
 * only one function out of these two functions should be called in
 * the application.
 *
 *  \return WLAN_ERROR_NONE if the CLI commands were registered or
 *  \return WLAN_ERROR_ACTION if they were not registered (for example
 *   if this function was called while the CLI commands were already
 *   registered).
 */
int wlan_basic_cli_init(void);

/** Register basic WLAN CLI commands
 *
 * This function registers WLAN bridge CLI commands like bridge
 * enable/disable, get configuration
 *
 * \note This function can only be called by the application after
 *  \ref wlan_init() called.
 *
 *  \return WM_SUCCESS if the CLI commands were registered or
 *  \return -WM_FAIL if they were not registerd (for example
 *   if this function was called while the CLI commands were already
 *   registered).
 */
int wlan_bridge_cli_init(void);

/** Register WLAN CLI commands.
 *
 *  Try to register the WLAN CLI commands with the CLI subsystem. This
 *  function is available for the application for use.
 *
 *  \note This function can only be called by the application after \ref wlan_init()
 *  called.
 *
 *  \note This function internally calls \ref wlan_basic_cli_init(), hence
 *  only one function out of these two functions should be called in
 *  the application.
 *
 *  \return WM_SUCCESS if the CLI commands were registered or
 *  \return -WM_FAIL if they were not (for example if this function
 *          was called while the CLI commands were already registered).
*/
int wlan_cli_init(void);

/** Register WLAN iw* CLI commands.
 *
 *  Register the WLAN iw* CLI commands like ifconfig, iwlist, iwconfig etc
 *  with the CLI subsystem.
 *
 *  \note This function can only be called by the application after \ref wlan_init()
 *  called.
 *
 *  \return WM_SUCCESS if the CLI commands were registered or
 *  \return -WM_FAIL if they were not (for example if this function
 *          was called while the CLI commands were already registered).
 */
int wlan_iw_cli_init(void);

/** Register WLAN uaputl CLI commands.
 *
 *  Register the WLAN uaputl CLI commands with the CLI subsystem. 
 *
 *  \note This function can only be called by the application after \ref wlan_init()
 *  called.
 *
 *  \return WM_SUCCESS if the CLI commands were registered or
 *  \return -WM_FAIL if they were not (for example if this function
 *           was called while the CLI commands were already registered).
 */
int wlan_uaputl_cli_init(void);

/** Register WLAN enhanced CLI commands.
 *
 *  Register the WLAN enhanced CLI commands like set or get tx-power,
 *  tx-datarate, tx-modulation etc with the CLI subsystem.
 *
 *  \note This function can only be called by the application after \ref wlan_init()
 *  called.
 *
 *  \return WM_SUCCESS if the CLI commands were registered or
 *  \return -WM_FAIL if they were not (for example if this function
 *           was called while the CLI commands were already registered).
 */
int wlan_enhanced_cli_init(void);

/** Register WLAN Power Mgmt CLI commands.
 *
 *  Register the WLAN Power Mgmt CLI commands like pm-ieeeps-hs-cfg,
 *  pm-wifi-deepsleep-enter with the CLI subsystem.
 *
 *  \note This function can only be called by the application after \ref
 *  wlan_init() called.
 *
 *  \return WLAN_ERROR_NONE if the CLI commands were registered or
 *  \return WLAN_ERROR_ACTION if they were not (for example if this function
 *           was called while the CLI commands were already registered).
 */
int wlan_pm_cli_init(void);

/** Configure Power Mgr Framework's IEEEPS/Host Sleep Configuration
 *
 * Enables/Disables the power manager framework's power savings for the wireless
 * interface. When enabled, the power manager framework opportunistically puts
 * the wireless card into IEEEPS mode. Before putting the Wireless card in power
 * save this also sets the hostsleep configuration on the card as
 * specified. This makes the card generate a wakeup for the MC200 processor if
 * any of the wakeup conditions are met.
 *
 * \param[in] enabled 1 to enable, 0 to disable IEEE PS support
 * \param[in] wakeup_conditions conditions to wake the MC200 up on. This should
 *            be a logical OR of the conditions in \ref wlan_wakeup_event_t.
 *            Typically devices would want to wake up on
 *            \ref WAKE_ON_ALL_BROADCAST,
 *            \ref WAKE_ON_UNICAST,
 *            \ref WAKE_ON_MAC_EVENT.
 *            \ref WAKE_ON_MULTICAST,
 *            \ref WAKE_ON_ARP_BROADCAST,
 *            \ref WAKE_ON_MGMT_FRAME
 *
 * @pre      pm_init() should be called before this function is used .
 *           More details are found at \link mdev_pm.h \endlink and
 *           \link pwrmgr.h \endlink
 *
 * @note     This function should be used after Station gets connected
 *           to a network.
 *
 */
void pm_ieeeps_hs_cfg(char enabled, unsigned int wakeup_conditions);

/** Configure Power Mgr Framework's UAPPS(Inactivity Timeout)/Host Sleep
 * Configuration.
 *
 * Enables/Disables the power manager framework's power savings for the wireless
 * micro-AP interface. When enabled, the power manager framework puts
 * the wireless card into UAPPS mode after the inactivity timeout.
 * Before putting the Wireless card in power save this also sets the hostsleep
 * configuration on the card as specified. This makes the card generate a wakeup
 * for the Micro-Controller core if any of the wakeup conditions are met.
 *
 * \note uAP power save needs to be stopped before stopping the uAP.
 *
 * Please refer to the wireless chips documentation for more details
 * about the parameters.
 *
 * \param[in] enabled 1 to enable, 0 to disable UAP PS support.
 * \param[in] inactivity_timeout inactivity timeout in microseconds
 *	      (default: 200000).
 * \param[in] wlan_uapps_params A pointer to structure holding
 *	      following parameters:
 *	      ctrl bitmap:
 *	      0 - disable protection frame Tx before PS(default)
 *	      1 - enable protection frame Tx before PS,
 *	      min_sleep: Minimum sleep duration in microseconds(default: 17000).
 *	      max_sleep: Maximum sleep duration in microseconds(default: 17000).
 *	      min_awake: Minimum awake duration in microseconds(default: 2000).
 *	      max_awake: Maximum awake duration in microseconds(default: 2000).
 *	      If this is null then default values gets used by the power save
 *	      framework.
 *	      If your are not aware of these settings then pass NULL for safe
 *	      configuration.
 * \param[in] wakeup_conditions conditions to wake the MC200 up on. This should
 *            be a logical OR of the conditions in \ref wlan_wakeup_event_t.
 *            Typically devices would want to wake up on
 *            \ref WAKE_ON_ALL_BROADCAST,
 *            \ref WAKE_ON_UNICAST,
 *            \ref WAKE_ON_MAC_EVENT.
 *            \ref WAKE_ON_MULTICAST,
 *            \ref WAKE_ON_ARP_BROADCAST,
 *            \ref WAKE_ON_MGMT_FRAME
 *
 * @note     \ref WAKE_ON_ALL_BROADCAST is recommended over
 *	     \ref WAKE_ON_ARP_BROADCAST to have proper behaviour of micro-AP
 *	     interface when wireless client initiates the connection.
 *
 * @pre      pm_init() should be called before this function is used .
 *           More details are found at \link mdev_pm.h \endlink and
 *           \link pwrmgr.h \endlink
 *
 * @note     This function should be used after micro-AP gets started.
 *	     Also before stopping micro-AP, power save needs to be disabled.
 *
 */
void pm_uapps_hs_cfg(char enabled, unsigned int inactivity_to,
		     struct wlan_uapps_params_t *wlan_uapps_params,
		     unsigned int wakeup_conditions);

/**
 * Validate input string as a WEP key and convert it to binary if it is in
 * hexadecimal form.
 *
 * \param[in] input A pointer to WEP key string.
 * \param[out] output A pointer to output WEP key in binary format.
 * \param[in] max_output_len Maximum output len
 * \param[out] output_len A pointer to output len.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 */
int load_wep_key(const uint8_t *input,
		uint8_t *output, uint8_t *output_len,
		const unsigned max_output_len);

/**
 * Starts sniffing with station interface on given channel and filters.
 * The captured frames are delivered to application using registered
 * sniffer callback.
 *
 * \param[in] filter_flags A filter for control, management and data frames.
 * \param[in] radio_type Radio type 2.4 GHz
 * \param[in] channel Channel number to sniff as per radio type.
 * \param[in] sniffer_callback An application callback to process
 * 	      captured frames.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 *
 * \note  Filter flags:
 *			Bit[0]: Enable/disable management frame receive,
 *			Bit[1]: Enable/disable control frame receive,
 *			Bit[2]: Enable/disable data frame receive.
 *			Bit[3:15]: Reserved
 *
 *	  Radio_type:
 *		Band configuration type
 *		Bits[7:6]: Reserved (set to 0)
 *		Bits[5:4]: Secondary channel offset
 *		     00 = no secondary channel
 *		     01 = secondary channel is above primary channel
 *		     10 = reserved
 *		     11 = secondary channel is below primary channel
 *		Bits[3:2]: Channel width
 *		     00 = 20 MHz channel width
 *		     01, 10 and 11 = reserved
 *		Bits[1:0]: Band information
 *		     00 = 2.4 GHz band
 *		     01 = 5 GHz band
 *		     10 and 11 = reserved
 *
 * wlan_sniffer_start(4, 0, 1, sniffer_callback);
 *
 * \note The above call with start sniffer for data frames on channel
 * number 1 in 2.4 GHz band.
 *
 */
int wlan_sniffer_start(const uint16_t filter_flags, const uint8_t radio_type,
			const uint8_t channel,
			void (*sniffer_callback)(const wlan_frame_t *frame,
						 const uint16_t len));

/**
 * Get sniffer status of station interface.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 */
int wlan_sniffer_status();

/**
 * Stops sniffing on station interface.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 */
int wlan_sniffer_stop();

/**
 * Stops sniffing on station interface.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 */
int wlan_sniffer_with_radiotap_hdr_stop();

/**
 * Get maximum number of WLAN firmware supported stations that
 * will be allowed to connect to the uAP.
 *
 * \return Maximum number of WLAN firmware supported stations.
 *
 * \note Get operation is allowed in any uAP state.
 */
unsigned int wlan_get_uap_supported_max_clients();

/**
 * Get current maximum number of stations that
 * will be allowed to connect to the uAP.
 *
 * \param[out] max_sta_num A pointer to variable where current maximum
 *             number of stations of uAP interface will be stored.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 *
 * \note Get operation is allowed in any uAP state.
 */
int wlan_get_uap_max_clients(unsigned int *max_sta_num);

/**
 * Set maximum number of stations that will be allowed to connect to the uAP.
 *
 * \param[in] max_sta_num Number of maximum stations for uAP.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 *
 * \note Set operation in not allowed in \ref WLAN_UAP_STARTED state.
 */
int wlan_set_uap_max_clients(unsigned int max_sta_num);

/**
 * Get Station interface transmit power
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 *
 */
int wlan_get_sta_tx_power();

/**
 * Set Station interface transmit power
 *
 * \param[in] power_level Transmit power level.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 *
 */
int wlan_set_sta_tx_power(int power_level);

/**
 * Get Station interface transmit rate index
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 *
 */
int wlan_get_sta_tx_rate_index();

/**
 * Set Station interface transmit rate index
 *
 * \param [in] rate_index Transmit rate index.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 *
 */
int wlan_set_sta_tx_rate_index(int rate_index);

/**
 * Get Station interface current transmit rate index
 *
 * \note Please use this API after \ref wlan_get_sta_tx_rate_index()
 *       API to get current transmit rate index.
 *
 * \param [out] rate_index Current transmit rate index if Data
 *              rate is not auto.
 *
 * \return true if Data rate is auto.
 * \return false if Data rate is not auto.
 *
 */
bool wlan_get_current_sta_tx_rate_index(int *rate_index);

/**
 * Get Management IE for given BSS type (interface) and index.
 *
 * \param[in] bss_type  BSS Type of interface.
 * \param[in] index IE index.
 *
 * \param[out] buf Buffer to store requested IE data.
 * \param[out] buf_len To store length of IE data.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 *
 */
int wlan_get_mgmt_ie(enum wlan_bss_type bss_type, unsigned int index,
			void *buf, unsigned int *buf_len);

/**
 * Set Management IE for given BSS type (interface) and index.
 *
 * \param[in] bss_type  BSS Type of interface.
 * \param[in] id Type/ID of Management IE.
 * \param[in] buf Buffer containing IE data.
 * \param[in] buf_len Length of IE data.
 *
 * \return IE index if successful.
 * \return -WM_FAIL if unsuccessful.
 *
 */
int wlan_set_mgmt_ie(enum wlan_bss_type bss_type, IEEEtypes_ElementId_t id,
			void *buf, unsigned int buf_len);

/**
 * Clear Management IE for given BSS type (interface) and index.
 *
 * \param[in] bss_type  BSS Type of interface.
 * \param[in] index IE index.
 *
 * \return WM_SUCCESS if successful.
 * \return -WM_FAIL if unsuccessful.
 *
 */
int wlan_clear_mgmt_ie(enum wlan_bss_type bss_type, unsigned int index);

/**
 * Get current status of 11D support.
 *
 * \return true if 11d support is enabled by application.
 * \return false if not enabled.
 *
 */
bool wlan_get_11d_enable_status();

/**
 * Get current RSSI and Signal to Noise ratio from WLAN firmware.
 *
 * \param[in] rssi A pointer to variable to store current RSSI
 * \param[in] snr A pointer to variable to store current SNR.
 *
 * \return WM_SUCCESS if successful.
 */
int wlan_get_current_signal_strength(short *rssi, int *snr);

/**
 * Get average RSSI and Signal to Noise ratio from WLAN firmware.
 *
 * \param[in] rssi A pointer to variable to store current RSSI
 * \param[in] snr A pointer to variable to store current SNR.
 *
 * \return WM_SUCCESS if successful.
 */
int wlan_get_average_signal_strength(short *rssi, int *snr);

/**
 * Frame Tx - Injecting Wireless frames from Host
 *
 * This function is used to Inject Wireless frames from application
 * directly.
 *
 * \note All injected frames will be sent on station interface. Application
 * needs minimum of 2 KBytes stack for successful operation.
 * Also application have to take care of allocating buffer for 802.11 Wireless
 * frame (Header + Data) and freeing allocated buffer. Also this
 * API may not work when Power Save is enabled on station interface.
 *
 * \param[in] bss_type The interface on which management frame needs to be send.
 * \param[in] buff Buffer holding 802.11 Wireless frame (Header + Data).
 * \param[in] len Length of the 802.11 Wireless frame.
 *
 * \return WM_SUCCESS on success or error code.
 *
 */
#if 0 //FRAME_INJECTOR we dont need this
int wlan_inject_frame(const enum wlan_bss_type bss_type,
		const uint8_t *buff, const size_t len);
#endif
/**
 * This API is is used to set/cancel the remain on channel configuration.
 *
 * \note When status is false, channel and duration parameters are
 * ignored.
 *
 * \param[in] bss_type The interface to set channel.
 * \param[in] status false : Cancel the remain on channel configuration
 *                   true : Set the remain on channel configuration
 * \param[in] channel The channel to configure
 * \param[in] duration The duration for which to
 *	      remain on channel in milliseconds.
 *
 * \return WM_SUCCESS on success or error code.
 *
 */
int wlan_remain_on_channel(const enum wlan_bss_type bss_type,
			const bool status, const uint8_t channel,
			const uint32_t duration);

uint16_t wlan_get_seq_num(uint16_t seq_frag_num);
uint16_t wlan_get_frag_num(uint16_t seq_frag_num);

/**
 * Get User Data from OTP Memory
 *
 * \param[in] buf Pointer to buffer where data will be stored
 * \param[in] len Number of bytes to read
 *
 * \return WM_SUCCESS if user data read operation is successful.
 * \return -WM_E_INVAL if buf is not valid or of insufficient size.
 * \return -WM_FAIL if user data field is not present or command fails.
 */
int wlan_get_otp_user_data(uint8_t *buf, uint16_t len);

/**
 * Get Calibration data from WLAN firmware
 *
 * \param[out] cal_data Pointer to calibration data structure where
 *	      calibration data and it's length will be stored.
 *
 * \return WM_SUCCESS if cal data read operation is successful.
 * \return -WM_E_INVAL if cal_data is not valid.
 * \return -WM_FAIL if command fails.
 *
 * \note The user of this API should free the allocated buffer for
 *	 calibration data.
 */
int wlan_get_cal_data(wlan_cal_data_t *cal_data);

/**
 * Get the TRPC power table from board file for the specified
 * country code if available and populate the \ref chan_trpc
 * structure.
 *
 * \param[in] country Country to set the TRPC configuration.
 * \param[out] chan_trpc A channel TRPC configuration structure
 *	       where the board file power table content will get
 *	       populated.
 *
 * \return WM_SUCCESS on success, error otherwise.
 */
int wlan_country_trpc_cfg_data(board_country_code_t country,
			chan_trpc_t *chan_trpc);
/**
 * Set the TRPC channel configuration.
 *
 * \param[in] chan_trpc Channel TRPC configuration.
 *
 * \return WM_SUCCESS on success, error otherwise.
 *
 */
int wlan_set_trpc(chan_trpc_t *chan_trpc);

/**
 * Get the TRPC channel configuration.
 *
 * \param[out] chan_trpc Channel TRPC configuration structure where
 *	       Wi-Fi firmware configuration will get copied.
 *
 * \return WM_SUCCESS on success, error otherwise.
 *
 * \note application can use \ref print_trpc API to print the
 *	 content of the chan_trpc structure.
 */
int wlan_get_trpc(chan_trpc_t *chan_trpc);

/**
 * Enable Auto Reconnect feature in WLAN firmware.
 *
 * \param[in] auto_reconnect_config Auto Reconnect configuration
 *	      structure holding following parameters:
 *	      1. reconnect counter(0x1-0xff) - The number of times the WLAN
 *		 firmware retries connection attempt with AP.
 *				The value 0xff means retry forever.
 *				(default 0xff).
 *	      2. reconnect interval(0x0-0xff) - Time gap in seconds between
 *				each connection attempt (default 10).
 *	      3. flags - Bit 0:
 *			 Set to 1: Firmware should report link-loss to host
 *				if AP rejects authentication/association
 *				while reconnecting.
 *			 Set to 0: Default behaviour: Firmware does not report
 *				link-loss to host on AP rejection and
 *				continues internally.
 *			 Bit 1-15: Reserved.
 *
 * \return WM_SUCCESS if operation is successful.
 * \return -WM_FAIL if command fails.
 *
 */
int wlan_auto_reconnect_enable(wlan_auto_reconnect_config_t
		auto_reconnect_config);

/**
 * Disable Auto Reconnect feature in WLAN firmware.
 *
 * \return WM_SUCCESS if operation is successful.
 * \return -WM_FAIL if command fails.
 *
 */
int wlan_auto_reconnect_disable();

/**
 * Get Auto Reconnect configuration from WLAN firmware.
 *
 * \param[out] auto_reconnect_config Auto Reconnect configuration
 *	       structure where response from WLAN firmware will
 *	       get stored.
 *
 * \return WM_SUCCESS if operation is successful.
 * \return -WM_E_INVAL if auto_reconnect_config is not valid.
 * \return -WM_FAIL if command fails.
 *
 */
int wlan_get_auto_reconnect_config(wlan_auto_reconnect_config_t
		*auto_reconnect_config);

/**
 * This API can be used to start/stop the management frame forwards
 * to host through datapath.
 *
 * \param[in] bss_type The interface from which management frame needs to be
 *	      collected.
 * \param[in] mgmt_subtype_mask     Management Subtype Mask
 *	      If Bit X is set in mask, it means that IEEE Management Frame
 *	      SubTyoe X is to be filtered and passed through to host.
 *            Bit                   Description
 *	      [31:14]               Reserved
 *	      [13]                  Action frame
 *	      [12:9]                Reserved
 *	      [8]                   Beacon
 *	      [7:6]                 Reserved
 *	      [5]                   Probe response
 *	      [4]                   Probe request
 *	      [3]                   Reassociation response
 *	      [2]                   Reassociation request
 *	      [1]                   Association response
 *	      [0]                   Association request
 *	      Support multiple bits set.
 *	      0 = stop forward frame
 *	      1 = start forward frame
 *\param[in] rx_mgmt_callback The receive callback where the received management
 *	     frames are passed.
 *
 * \return WM_SUCCESS if operation is successful.
 * \return -WM_FAIL if command fails.
 *
 * \note Pass Management Subtype Mask all zero to disable all the management
 *	 frame forward to host.
 */
int wlan_rx_mgmt_indication(const enum wlan_bss_type bss_type,
			const uint32_t mgmt_subtype_mask,
			void (*rx_mgmt_callback)(const enum wlan_bss_type
				bss_type, const wlan_mgmt_frame_t *frame,
				const size_t len));

/** API to set the beacon period of uAP
 *
 *\param[in] beacon_period Beacon period in TU (1 TU = 1024 micro seconds)
 *
 *\note Please call this API before calling uAP start API.
 *
 */
void wlan_uap_set_beacon_period(const uint16_t beacon_period);

/** API to control SSID broadcast capability of uAP
 *
 * This API enables/disables the SSID broadcast feature
 * (also known as the hidden SSID feature). When broadcast SSID
 * is enabled, the AP responds to probe requests from client stations
 * that contain null SSID. When broadcast SSID is disabled, the AP
 * does not respond to probe requests that contain null SSID and
 * generates beacons that contain null SSID.
 *
 *\param[in] bcast_ssid_ctl Broadcast SSID control if true SSID will be
 *	     hidden otherwise it will be visible.
 *
 *\note Please call this API before calling uAP start API.
 *
 */
void wlan_uap_set_hidden_ssid(const bool bcast_ssid_ctl);

/** API to control the deauth during uAP channel switch
 *
 *\param[in] enable 0 -- Wi-Fi firmware will use default behaviour.
 *		    1 -- Wi-Fi firmware will not send deauth packet
 *		         when uap move to another channel.
 *
 *\note Please call this API before calling uAP start API.
 *
 */
void wlan_uap_ctrl_deauth(const bool enable);

/** API to enable channel switch announcement functionality on uAP. *
 *
 *\param[in] chan_sw_count A channel switch count to send
 *			   channel switch announcement in count no of
 *			   beacons before channel switch, during station
 *			   connection attempt on different channel with
 *			   Ex-AP.
 *
 *\note Please call this API before calling uAP start API. Also
 *	note that 11N should be enabled on uAP.
 *
 */
void wlan_uap_set_ecsa(const uint8_t chan_sw_count);

/**
 * This API can be used to enable AMPDU support on the go
 * when station is a transmitter.
 *
 * \note By default the station AMPDU TX support is on if
 * configuration option is enabled in defconfig.
 */
void wlan_sta_ampdu_tx_enable(void);

/**
 * This API can be used to disable AMPDU support on the go
 * when station is a transmitter.
 *
 *\note By default the station AMPDU RX support is on if
 * configuration option is enabled in defconfig.
 *
 */
void wlan_sta_ampdu_tx_disable(void);

/**
 * This API can be used to enable AMPDU support on the go
 * when station is a receiver.
 */
void wlan_sta_ampdu_rx_enable(void);

/**
 * This API can be used to disable AMPDU support on the go
 * when station is a receiver.
 */
void wlan_sta_ampdu_rx_disable(void);

/**
 * Set number of channels and channel number used during automatic
 * channel selection of uAP.
 *
 *\param[in] scan_chan_list A structure holding the number of channels and
 *	     channel numbers.
 *
 *\note Please call this API before uAP start API in order to set the user
 *      defined channels, otherwise it will have no effect. There is no need
 *      to call this API every time before uAP start, if once set same channel
 *      configuration will get used in all upcoming uAP start call. If user
 *      wish to change the channels at run time then it make sense to call
 *      this API before every uAP start API.
 */
void wlan_uap_set_scan_chan_list(wifi_scan_chan_list_t scan_chan_list);

#ifdef CONFIG_WPA2_ENTP

/**
 * Use this API if application want to allow station
 * connection to WPA2 Enterprise ap profiles only.
 *
 * If called the in scan result only the WPA2 Enterprise AP
 * will be listed and station network profile only with WPA2
 * Enterprise security will be allowed to add to network profile
 * list.
 */
void wlan_enable_wpa2_enterprise_ap_only();
#endif

#endif /* __WLAN_H__ */
