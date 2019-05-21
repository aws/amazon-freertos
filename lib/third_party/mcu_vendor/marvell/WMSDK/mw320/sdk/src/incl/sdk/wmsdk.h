/** @file wmsdk.h
*
*  @brief This file provides APIs for the various features offered by the WMSDK
*
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

#ifndef _WMSDK_H_
#define _WMSDK_H_

#include <wmstdio.h>
#include <wm_utils.h>
#ifdef CONFIG_ENABLE_TLS
#include <mbedtls/ssl.h>
#endif /* CONFIG_ENABLE_TLS */

/** WLAN Connect failure reason codes
 */
typedef enum {
	/** In case AP-is down or network unavailable */
	WM_WLAN_NETWORK_NOT_FOUND,
	/** In case of authentication failure */
	WM_WLAN_AUTH_FAILED,
	/** In case of failure in assigning IP address */
	WM_WLAN_DHCP_FAILED,
} wlan_failure_reason_t;

/** Connect to an Access Point using the ssid and passphrase
 *
 * This function initializes the Wi-Fi subsystem and connects to an
 * Access Point using the ssid and psk.
 *
 * \param[in] ssid SSID of the Access Point
 * \param[in] psk passphrase of the Access Point. Pass NULL for open networks.
 *
 * \return WM_SUCCESS on success
 * \return negative error code otherwise
 */
int wm_wlan_connect(char *ssid, char *psk);

/** Start configuration app, and connect to Access Point if configured
 *
 * - If the network configuration settings are done, this function initializes
 * the Wi-Fi subsystem to connect to the Access Point as configured.
 * - If the network configuration settings are not done, this function starts a
 * Wi-Fi network (micro-AP) of its own, with the my_ssid/my_wpa2_passphrase. An
 * end-user can connect to this micro-AP network and enter home network
 * configuration. Once the configuration is received, it then connects to the
 * Access Point that is configured.
 *
 * \param[in] my_ssid The SSID of the micro-AP network that should be hosted
 * \param[in] my_wpa2_passphrase The passphrase of the micro-AP network that
 * should be hosted
 *
 * \return WM_SUCCESS on success
 * \return negative error code otherwise
 */
int wm_wlan_start(char *my_ssid, char *my_wpa2_passphrase);

/** Callback function when Wi-Fi is initialized.
 *
 * This function gets invoked when Wi-Fi interface is initialized. Data holds
 * the device state whether it is provisioned or not. If device state is 1, that
 * indicates device is provisioned and if it is 0, indicates device is not
 * provisioned.
 */
WEAK void wlan_event_wifi_initialized(void *data);

/** Callback function when Wi-Fi station is connected
 *
 * This function gets called when the station interface of the Wi-Fi network
 * successfully associates with an Access Point.
 */
WEAK void wlan_event_normal_connected(void *data);

/** Callback function when Wi-Fi station connection fails
 *
 * This function gets called when the station interface of the Wi-Fi network
 * fails to associate with an Access Point. The station will continue to make
 * connection attempts infinitely. This function will be called for connection
 * failure.
 */
WEAK void wlan_event_normal_connect_failed(wlan_failure_reason_t fail_code);

/** Callback function when Wi-Fi station link is lost
 *
 * This function gets called when the station interface of the Wi-Fi network
 * loses the connection that was previously established with the Access
 * Point. The station will continue to make connection attempts to the Access
 * Point, after this callback returns.
 */
WEAK void wlan_event_normal_link_lost(void *data);

/** Reset the device to factory settings
 *
 * Erase all the configured information in this device, including network
 * credentials and any other configuration data. Once all the settings are erase
 * reboot the device to start fresh.
 */
int invoke_reset_to_factory();

/** These APIs are left undocumented for now */
void wm_wlan_set_httpd_handlers(void *hdlrs, int no);

#ifdef CONFIG_ENABLE_TLS
typedef struct wm_tls_handle {
	void *ptr;
} wm_tls_handle_t;

/** Representation of a TLS Certificate */
typedef struct wm_tls_cert {
	/** The buffer that holds the certificate */
	const unsigned char *cert;
	/** The size of the data in the buffer pointed to above */
	unsigned int         cert_size;
} wm_tls_cert_t;

/** Representation of a TLS Key */
typedef struct wm_tls_key {
	/** The buffer that holds the certificate */
	const unsigned char *key;
	/** The size of the data in the buffer pointed to above */
	unsigned int         key_size;
} wm_tls_key_t;

/** Open a simple TLS client for communication
 *
 * This can be used for performing read/write operations over a TLS connection.
 *
 * \param[in] handle Pointer to a wm_tls_handle_t data type
 * \param[in] ca_cert Pointer to a buffer containing the CA certificate
 * \param[in] client_cert Pointer to a buffer containing the Client certificate
 * \param[in] client_key Pointer to a buffer containing the Client key
 * \param[in] sock_fd The connected TCP socket to be associated with this TLS
 * session
 *
 * \return WM_SUCCESS on success
 * \return negative error code otherwise
 */
int wm_tls_client_open(wm_tls_handle_t *handle,
		       wm_tls_cert_t *ca_cert,
		       wm_tls_cert_t *client_cert,
		       wm_tls_key_t *client_key,
		       int sock_fd);
/** Read from a TLS session
 *
 * \param[in] handle The handle returned from wm_tls_client_open
 * \param[in] data Pointer to the buffer to read data
 * \param[in] sz The maximum size of the buffer
 *
 * \return number of bytes read from the session
 * \return negative error code otherwise
 */
int wm_tls_client_read(wm_tls_handle_t *handle, void* data, int sz);

/** Write to a TLS session
 *
 * \param[in] handle The handle returned from wm_tls_client_open
 * \param[in] data Pointer to the buffer to write data
 * \param[in] sz The size of the data to be written
 *
 * \return number of bytes written to the session
 * \return negative error code otherwise
 */
int wm_tls_client_write(wm_tls_handle_t *handle, const void* data, int sz);

/** Close a TLS session
 *
 * \param[in] handle The handle to be closed
 *
 */
void wm_tls_client_close(wm_tls_handle_t *handle);

/** Reset SSL internal timer
 *
 * \param[in] handle The handle returned from wm_tls_client_open
 */
void wm_tls_reset_read_timer(wm_tls_handle_t *handle);

/** Set SSL read timeout
 *
 * \param[in] handle The handle returned from wm_tls_client_open
 * \param[in] timeout Desired timeout value to be set
 */
void wm_tls_set_read_timeout(wm_tls_handle_t *handle, uint32_t timeout);

/** Setup a simple TLS client context
 *
 * \param[in] handle Pointer to a wm_tls_handle_t data type
 * \param[in] ca_cert Pointer to a buffer containing the CA certificate
 * \param[in] client_cert Pointer to a buffer containing the Client certificate
 * \param[in] client_key Pointer to a buffer containing the Client key
 * \param[in] sock_fd The connected TCP socket to be associated with this TLS
 * session
 *
 * \return SSL context from handle on success
 * \return NULL otherwise
 */
mbedtls_ssl_context *wm_tls_client_setup(wm_tls_handle_t *handle,
		wm_tls_cert_t *ca_cert,
		wm_tls_cert_t *client_cert,
		wm_tls_key_t *client_key,
		int sock_fd);

/** Perform SSL connection based on parameters set in handle
 *
 * \param[in] handle Pointer to a wm_tls_handle_t data type
 *
 * \return WM_SUCCESS on success
 * \return negative error code otherwise
 */
int wm_tls_connect(wm_tls_handle_t *handle);
#endif /* CONFIG_ENABLE_TLS */

#endif /* ! _WMSDK_H_ */
