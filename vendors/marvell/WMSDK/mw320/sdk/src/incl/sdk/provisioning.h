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

/** @file provisioning.h
*
*  @brief Provisioning Support
*
*  \section intro Introduction
*  Provisioning process lets user configure the WMSDK enabled device to connect
*  to his home network first time. Provisioning library in WMSDK provides
*  implementation of three provisioning methods to the application developer.
*  These methods are:
*  \li micro-AP network based provisioning
*  \li WPS based provisioning
*  \li EZConnect (Sniffer-based) provisioning
*
*  Of the above methods, the provisioning library allows co-existence of
*  micro-AP and WPS provisioning methods.
*
*
*  \subsection wlannw_prov WLAN network (micro-AP) based provisioning
*
*  In this method, a network is hosted by WMSDK enabled device. Clients can
*  associate with this network and use the provided HTTP APIs to configure the
*  home network properties on to the device. These HTTP APIs let users scan the
*  environment to find available access points, choose the access point of the
*  interest, provide security passphrase for the access point and finally push
*  the settings to the device to complete provisioning. These HTTP APIs are
*  listed in the Marvell System API guide.
*
*  For microAP based provisioning, a secure network can be hosted on the device
*  thus ensuring that the data transferred over the air during the provisioning
*  process is encrypted.
*
*
*  \subsection wps_prov WPS based provisioning
*  WiFi Protected Setup is a standard that attempts to allow easy yet secure
*  association of WiFi clients to the access point. Provisioning library allows
*  developers to extend WPS association methods to the end-user. It supports
*  both a) PIN and b) Push-button methods. These are described below.
*  \li PIN Method: In this method a 8-digit numerical PIN should be used on both
*  device and access point. This PIN can be static or dynamically generated on
*  the device. Depending on the capabilities of the device the PIN can be
*  printed or displayed on the device screen. When the same PIN is entered
*  within interval of 2 minutes on the access point and WPS session is started,
*  association succeeds.
*  \li Push-button Method: In this method a physical or virtual pushbutton on
*  the access point as well as on the device is pressed within interval of 2
*  minutes, the association succeeds.
*
*  \subsection ezconnect_prov EZConnect provisioning
*
*  In this method, the device acts as a sniffer and captures all the packets on
*  the air looking for a certain signature. A smart phone application (Android /
*  iOS) is then used to transmit the target network credentials in a certain
*  format. The target network credentials can be encrypted using a pre-shared
*  key. The device sniffs these packets off the air, decrypts the data and
*  establishes connection with the target network.
*
*  @cond uml_diag
*
*  @startuml{secure_provisioning.png}
*  actor Customer
*  participant WM
*  participant "Service Client" as SP
*  note left of SP: iPOD App or Laptop
*  Customer->WM: First Power on
*  WM->WM: Start micro-AP network
*  note left: IP: 192.168.10.1\nSSID: "wmdemo-AABB"
*  Customer->SP: Start Client
*  SP->SP: Scan Wireless Networks
*  SP->WM: Connect to micro-AP network
*  SP->WM: Get list of available networks
*  activate WM
*  WM->WM: Scan Wireless Networks in vicinity
*  WM->SP: Returns list of networks
*  deactivate WM
*
*  SP->SP: Display the list of networks
*  Customer->SP: Select the home wireless network
*  SP->WM: HTTP POST /sys/network
*
*
*  WM->WM: Connect to the configured network
*  @enduml
*  @endcond
*
*/

#ifndef __PROVISIONING_H__
#define __PROVISIONING_H__

#include <wlan.h>

/** Provisioning Scan Policies
 *
 * The micro-AP provisioning process performs a periodic scan. The scan results
 * are available through the /sys/scan HTTP API. The following scan policies can
 * be used for this scanning.
 */
enum prov_scan_policy {
	 /** Periodic Scan. This is the default scan policy. The periodicity can
	  * be configured using the prov_set_scan_config() function or through
	  * the /sys/scan-config HTTP API.
	  */
	 PROV_PERIODIC_SCAN,
	 /** On-Demand Scan. In this policy a network scan is performed on
	  * provisioning start-up. Further calls are only triggered on
	  * subsequent requests to the /sys/scan HTTP API.
	  */
	 PROV_ON_DEMAND_SCAN,
};
/** @cond */
enum wm_prov_errno {
	WM_E_PROV_ERRNO_BASE = MOD_ERROR_START(MOD_PROV),
	WM_E_PROV_INVALID_CONF,
	WM_E_PROV_INVALID_STATE,
	WM_E_PROV_SESSION_ATTEMPT,
	WM_E_PROV_PIN_CHECKSUM_ERR,
};
/** @endcond */

/** This enum enlists the events that are delivered to the application by
  provisioning module through the provisioning_event_handler callback
  registered in struct provisioning_config. Note that all the events may not
  be delivered in the context of provisioning thread. */
enum prov_event {
	/** This event is delivered immediately after when provisioning thread
	  begins its operation i.e. when prov_start is called */
	PROV_START,
	/** This event is delivered immediately after smart mode is started
	    i.e. when wlan_start_smart_mode is called */
	PROV_START_SMART_MODE,
	/** This event is delivered immediately after smart mode is stopped
	    i.e. when wlan_stop_smart_mode is called */
	PROV_STOP_SMART_MODE,
	/** This event is delivered when device switches from micro AP mode to
	    smart mode */
	PROV_RESET_TO_SMC,
	/** This event is received along with struct wlan_network data. This
	  event is delivered to application in case of successful POST to
	  /sys/network or in case of successful WPS session attempt.
	  Application can use received wlan_network structure to connect as
	  well as to store in PSM. */
	PROV_NETWORK_SET_CB,
	/** This event is received when device receives micro-AP connection
	 * request in smc mode. Generated only when EZConnect provisioning is
	 * enabled */
	PROV_MICRO_AP_UP_REQUESTED,
	/** This event is currently unused
	*/
	PROV_ABORTED, /* unused */
	/** This event is generated by provisioning library when
	  \li PIN is registered with provisioning thread and found scan results
	  contain atleast one SSID with on-going WPS session with PIN mode
	  active
	  \li Pushbutton is registered with provisioning thread and found scan
	  results contain atleast one SSID with on-going WPS session with
	  Pushbutton mode active.

	  Along with this event an array of struct wlan_scan_results is sent.
	  The number of array elements can be found by dividing "len" by sizeof
	  struct wlan_scan_results. The application is expected to return index
	  of chosen SSID with which WPS session should be attempted after going
	  through the provided scan list. The application should return -1, if
	  it doesn't intend to attempt session with any SSID found in the list.
	*/
	PROV_WPS_SSID_SELECT_REQ,
	/** This event is generated when WPS session attempt is started by the
	  provisioning thread. Application can use this to provide visual
	  indicator to end-user.
	*/
	PROV_WPS_SESSION_STARTED,
	/** This event is generated when the PIN times out. The timeout interval
	  is 2 minutes. Application may choose to re-send the PIN on timeout.
	*/
	PROV_WPS_PIN_TIMEOUT,
	/** This event is generated when the Pushbutton event times out. The
	  timeout interval is 2 minutes.
	*/
	PROV_WPS_PBC_TIMEOUT,
	/** This event is generated when WPS session succeeds. This event is
	  preceded by PROV_SET_NETWORK_CB event that delivers the home network
	  security parameters to the application.
	*/
	PROV_WPS_SESSION_SUCCESSFUL,
	/** This event is generated when the WPS session can not succeed due
	  to some condition like PIN values at both ends did not match.
	*/
	PROV_WPS_SESSION_UNSUCCESSFUL,
};


/** This enum enlists the states in which provisioning thread can be.
  Application may want to do some validation depending on the state before
  sending any request to provisioning thread. */
enum provisioning_state {
	/** This state indicates the provisioning thread is still initializing
	  provisioning network and/or WPS thread. Application should not pass
	  events like PIN or Pushbutton registration in this state as they will
	  be ignored.
	*/
	PROVISIONING_STATE_INITIALIZING = 0,
	/** This state indicates that the device is in provisioning state ready
	  to get network settings applied.
	*/
	PROVISIONING_STATE_ACTIVE,
	/** This state indicates either
	 * \li POST on /sys/network has arrived
	 * 	or
	 * \li WPS session attempt is in progress
	 *
	 * This state protects from simultaneous attempts to configure the
	 * device
	 */
	PROVISIONING_STATE_SET_CONFIG,
	/** This state indicates that provisioning is successful
	*/
	PROVISIONING_STATE_DONE,
	/** This state indicates that provisioning state machine
	    is about to be stopped */
	PROVISIONING_STATE_STOP_REQUESTED,
	/** This state indicates that provisioning state machine
	    is stopped */
	PROVISIONING_STATE_STOPPED,
};

/** This structure is used to represent scan configuration */
struct prov_scan_config {
	/** Wireless scan parameters as defined at struct wifi_scan_params_t */
	struct wifi_scan_params_t wifi_scan_params;
	/** Interval between consecutive scans represented in seconds. This
	  value should not be less than 2 seconds and should not exceed 60
	  seconds. */
	int scan_interval;
};

/** WLAN network (uAP) based provisioning mode */
#define PROVISIONING_WLANNW		(unsigned int)(1<<0)
/** WPS provisioning mode */
#define PROVISIONING_WPS                (unsigned int)(1<<1)
/** Marvell EZConnect Provisioning mode */
#define PROVISIONING_EZCONNECT		(unsigned int)(1<<2)

/** Provisioning device key is used in sniffer based provisioning and
 *  application based secure provisioning for encryption
 */
/* Minimum length of provisioning device key */
#define PROV_DEVICE_KEY_MIN_LENGTH     8
/* Maximum length of provisioning device key */
#define PROV_DEVICE_KEY_MAX_LENGTH    32

/** This structure is used to configure provisioning module with application
  specific settings. */
struct provisioning_config {
	/** Provisioning mode; could be \ref PROVISIONING_WLANNW and/or \ref
	 * PROVISIONING_WPS or \ref PROVISIONING_EZCONNECT. Note that while \ref
	 * PROVISIONING_WLANNW and \ref PROVISIONING_WPS can be simultaneously
	 * used, \ref PROVISIONING_EZCONNECT can only be used by itself. */
	unsigned int prov_mode;
	/** Callback function pointer that will be invoked for any events to be
	 * given to the application. These events are listed in enum
	 * prov_event. arg is a generic pointer that may be valid when data is
	 * to be received along with event. In such case len contains length of
	 * data passed along.  */
	int (*provisioning_event_handler) (enum prov_event event, void *arg,
					   int len);
};

/* Currently we only support 30 bytes of binary custom data */
#define MAX_CUSTOM_DATA_LEN 30
/** This structure is used to store provisioned network and custom data */
struct provisioning_data {
	/* Configured network */
	struct wlan_network net;
	/* CUstom data length */
	int custom_data_len;
	/* 16 byte custom data */
	uint8_t custom_data[MAX_CUSTOM_DATA_LEN];
};

/* Provisioning APIs */

/** Starts provisioning with provided configuration
 * \param pc Pointer to provisioning_config structure that contains application
 *        specific provisioning configuration.
 *
 * \return WM_SUCCESS if the call is successful
 *	   -WM_FAIL in case of error
 *
 * \note The application should not create this structure instance through local
 * variable. Provisioning thread needs to access this throughout its lifetime.
 *
 */
int prov_start(struct provisioning_config *pc);

/** Stops provisioning thread */
void prov_finish(void);

/* EZconnect Provisioning APIs */

/** Start EZConnect Provisioning
 *
 * This function starts the EZConnect provisioning process.
 *
 * \param pc Pointer to provisioning_config structure that contains application
 *        specific provisioning configuration.
 *
 * \return WM_SUCCESS if the call is successful
 *	   -WM_FAIL in case of error
 *
 * \note The application should not create this structure instance through local
 * variable. Provisioning thread needs to access this throughout its lifetime.
 *
 */
int prov_ezconnect_start(struct provisioning_config *pc);

/** Stops ezconnect provisioning thread */
void prov_ezconnect_finish(void);

#ifdef CONFIG_WPS2
/** Give WPS pushbutton press event to provisioning module
 * \return WM_SUCCESS if call is successful
 *	   -WM_E_PROV_SESSION_ATTEMPT if WPS session is already in progress
 * 	   -WM_E_PROV_INVALID_STATE if provisioning module is not in active
 *        state
 */
int prov_wps_pushbutton_press(void);

/** Register WPS PIN with provisioning module
 * \param pin PIN to be registered
 * \return WM_SUCCESS if call is successful
 * 	   -WM_E_PROV_PIN_CHECKSUM_ERR in case of PIN checksum invalid
 *	   -WM_E_PROV_SESSION_ATTEMPT if WPS session is already in progress
 * 	   -WM_E_PROV_INVALID_STATE if provisioning module is not in active
 *        state
 */
int prov_wps_provide_pin(unsigned int pin);
#endif

/** Verify scan interval value
 * \param scan_interval Scan interval in seconds
 * \return WM_SUCCESS if interval is valid
 *	   -WM_FAIL if interval is invalid
 */
int verify_scan_interval_value(int scan_interval);

/* Provisioning web handlers APIs */
/** Register provisioning HTTP API handlers
 * \return WM_SUCCESS if handlers registered successfully
 *	   -WM_FAIL if handlers registration fails
 */
int register_provisioning_web_handlers(void);

/** Unregister provisioning HTTP API handlers */
void unregister_provisioning_web_handlers(void);

typedef int (*nw_save_cb_t)(struct provisioning_data *);
/* Secure provisioning web handlers APIs */
/** Register secure provisioning HTTP API handlers
 *
 * \param key Pointer to provisioning key string for encryption. This
 *            must be 8 to 32-bytes in length.
 * \param len Length of the provisioning key.
 * \param cb  Pointer to the function that will be called when network is set.
 * \return WM_SUCCESS if handlers registered successfully
 *	   -WM_FAIL if handlers registration fails
 */
int register_secure_provisioning_web_handlers(uint8_t *key, int len,
					      nw_save_cb_t nw_save_cb);

/** Unregister secure provisioning HTTP API handlers */
void unregister_secure_provisioning_web_handlers(void);

/** Set ezconnect provisioning micro-AP ssid
 *
 * In multi modal provisioning, device enters in SMC mode in which it sniffs
 * packets and also sends beacons and probe responses. SSID information is
 * required in populating beacon information. Hence, this API should be called
 * before configuring SMC mode.
 *
 * \param ssid       The micro-AP SSID
 * \param passphrase The WPA2 passphrase of the micro-AP.
 *                   Pass NULL for an Open network.
 */
void register_ezconnect_provisioning_ssid(char *ssid, char *passphrase);

/** Enqueue periodic wlan_scan job in system work queue
 *
 * In multi modal provisioning, when device receives micro AP auth request, it
 * starts micro AP. To make scan results available for /prov/scan secure web
 * handler, wlan_scan is enqueued in system work queue which updates scan list
 * periodically.
 *
 * \return WM_SUCCESS on success
 * \return -WM_FAIL otherwise
 */
int wscan_periodic_start();

/** Dequeue wlan_scan job from system work queue
 *
 * \return WM_SUCCESS on success
 * \return -WM_FAIL otherwise
 */
int wscan_periodic_stop();

/** This function lets user use /sys/network handler in "provisioned" mode
 * by letting application register a callback handler that is called with
 * received wlan_network structure that gives control directly to the
 * application bypassing provisioning module.
 * \param cb_fn Function pointer that should be called in case of POST on
 *              /sys/network
 * \return WM_SUCCESS if handler registered successfully
 * 	   -WM_FAIL if handler registration fails
 */
int set_network_cb_register(int (*cb_fn) (struct provisioning_data *));

/* This function is internal to the provisioning module */
int prov_ezconn_set_device_key(uint8_t *key, int len);

/* This function is internal to the provisioning module */
void prov_ezconn_unset_device_key(void);

/** Set Provisioning Scan Policy
 *
 * The micro-AP provisioning process performs a periodic scan. The scan results
 * are available through the /sys/scan HTTP API. This API can be used to modify
 * the provisioning scan policy.
 *
 * The default policy is to scan periodically. The periodicity can be configured
 * using the prov_set_scan_config() function or through the /sys/scan-config
 * HTTP API.
 *
 * \param scan_policy can be either of \ref PROV_PERIODIC_SCAN
 * or \ref PROV_ON_DEMAND_SCAN
 * \return -WM_FAIL in case of the scan policy cannot be set.
 * \return WM_SUCCESS if scan policy is set successfully.
 */
int prov_set_scan_policy(enum prov_scan_policy scan_policy);

/** Lock the scanned entries
 *
 * Get the lock on scanned entries structure. This is required since
 * the provisioning module periodically updates the scanned entries.
 * This update should not take place in between access to those scanned
 * network entries
 *
 * \return -WM_FAIL if error in locking the mutex
 * WM_SUCCESS if success in locking the mutex
 */
int prov_get_lock_scan_results();

/** Revoke the lock on scanned entries
 *
 * Revoke the lock from scanned entries structure
 *
 */
void prov_put_lock_scan_results();

/** Get the number of the network scanned entries
 *
 * This API returns the count of scanned network entries by the firmware
 *
 * \return number of valid scanned entries
 */
int prov_get_scan_count();

/** Get the scanned entry present
 *
 * This API is used to retrieve the network scanned entry from the specified
 * index
 *
 * \param[out] sptr address of the pointer of type wlan_scan_result pointing
 * to the scan result at given index
 * \param[in] index specifies the index of the result to be copied
 *
 * \return WM_SUCCESS if valid index
 * -WM_FAIL if index is invalid
 */
int prov_get_scan_result(struct wlan_scan_result **sptr, unsigned int index);

/** Sets the current station interface network
 *
 * This API is used to command the station interface to connect to the
 * specified network
 *
 * \param[in] prov_method An integer to specify the type of WLAN
 * provisioning mode. This can be one of the following PROVISIONING_WLANNW,
 * PROVISIONING_WPS or PROVISIONING_EZCONNECT
 * \param[in] net pointer to the \ref wlan_network containing
 * the network credentials to be associated with
 *
 * \return WM_SUCCESS on success
 * \return error otherwise
 */
int prov_set_network(int prov_method, struct wlan_network *net);
#endif
