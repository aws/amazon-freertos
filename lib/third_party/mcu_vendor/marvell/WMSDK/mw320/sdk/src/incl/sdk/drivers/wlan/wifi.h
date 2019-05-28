/* @file wifi.h
 *
 *  @brief This file contains interface to wifi driver
 *
 *  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
 *
 *  MARVELL CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Material") are owned by Marvell International Ltd or its
 *  suppliers or licensors. Title to the Material remains with Marvell International Ltd
 *  or its suppliers and licensors. The Material contains trade secrets and
 *  proprietary and confidential information of Marvell or its suppliers and
 *  licensors. The Material is protected by worldwide copyright and trade secret
 *  laws and treaty provisions. No part of the Material may be used, copied,
 *  reproduced, modified, published, uploaded, posted, transmitted, distributed,
 *  or disclosed in any way without Marvell's prior express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by Marvell in writing.
 *
 */

#ifndef __WIFI_H__
#define __WIFI_H__

#include <wifi-decl.h>
#include <wifi_events.h>
#include <pwrmgr.h>
#include <flash.h>
#include <wm_os.h>
#include <wmerrno.h>

extern int16_t g_bcn_nf_last;
extern uint8_t g_rssi;
extern uint16_t g_data_nf_last;
extern uint16_t g_data_snr_last;

enum {
	WM_E_WIFI_ERRNO_START = MOD_ERROR_START(MOD_WIFI),
	/** The Firmware download operation failed. */
	WIFI_ERROR_FW_DNLD_FAILED,
	/** The Firmware ready register not set. */
	WIFI_ERROR_FW_NOT_READY,
	/** The WiFi card not found. */
	WIFI_ERROR_CARD_NOT_DETECTED,
	/** The WiFi Firmware not found. */
	WIFI_ERROR_FW_NOT_DETECTED,
	/** The WiFi Firmware XZ decompression failed. */
	WIFI_ERROR_FW_XZ_FAILED,
};

/**
 * Initialize Wi-Fi driver module.
 *
 * Performs SDIO init, downloads Wi-Fi Firmware, creates Wi-Fi Driver
 * and command response processor thread.
 *
 * Also creates mutex, and semaphores used in command and data synchronizations.
 *
 * \param[in] fl Pointer to flash descriptor where Wi-Fi Firmware is
 * stored on flash.
 *
 * \return WM_SUCCESS on success or -WM_FAIL on error.
 *
 */
int wifi_init(const flash_desc_t *fl);

/**
 * Deinitialize Wi-Fi driver module.
 *
 * Performs SDIO deinit, send shutdown command to Wi-Fi Firmware, deletes
 * Wi-Fi Driver and command processor thread.
 *
 * Also deletes mutex and semaphores used in command and data synchronizations.
 *
 */
void wifi_deinit(void);

/**
 * Register Data callback function with Wi-Fi Driver to receive
 * DATA from SDIO.
 *
 * This callback function is used to send data received from Wi-Fi
 * firmware to the networking stack.
 */
int wifi_register_data_input_callback(void (*data_intput_callback)
				(const uint8_t interface, const uint8_t *buffer,
				const uint16_t len));

/** Deregister Data callback function from Wi-Fi Driver */
void wifi_deregister_data_input_callback();

/**
 * Wi-Fi Driver low level output function.
 *
 * Data received from upper layer is passed to Wi-Fi Driver for transmission.
 *
 * \param[in] interface Interface on which DATA frame will be transmitted.
 *  0 for Station interface, 1 for uAP interface and 2 for Wi-Fi
 *  Direct interface.
 * \param[in] buffer A pointer pointing to DATA frame.
 * \param[in] len Length of DATA frame.
 *
 * \return WM_SUCCESS on success or -WM_E_NOMEM if memory is not available
 *  or -WM_E_BUSY if SDIO is busy.
 *
 */
int wifi_low_level_output(const uint8_t interface, const uint8_t *buffer,
			const uint16_t len);

/**
 * API to enable packet retries at wifi driver level.
 *
 * This API sets retry count which will be used by wifi driver to retry packet
 * transmission in case there was failure in earlier attempt. Failure may
 * happen due to SDIO write port un-availability or other failures in SDIO
 * write operation.
 *
 * \note Default value of retry count is zero.
 *
 * \param[in] count No of retry attempts.
 *
 */
void wifi_set_packet_retry_count(const int count);

/**
 * This API can be used to enable AMPDU support on the go
 * when station is a transmitter.
 */
void wifi_sta_ampdu_tx_enable(void);

/**
 * This API can be used to disable AMPDU support on the go
 * when station is a transmitter.
 */
void wifi_sta_ampdu_tx_disable(void);

/**
 * This API can be used to enable AMPDU support on the go
 * when station is a receiver.
 */
void wifi_sta_ampdu_rx_enable(void);

/**
 * This API can be used to disable AMPDU support on the go
 * when station is a receiver.
 */
void wifi_sta_ampdu_rx_disable(void);

/**
 * Get the device MAC address
 */
int wifi_get_device_mac_addr(mac_addr_t *mac_addr);

/**
 * Get the string representation of the wlan firmware version.
 */
int wifi_get_firmware_version(wifi_fw_version_t *ver);

/**
 * Get the string representation of the wlan firmware extended version.
 */
int wifi_get_firmware_version_ext(wifi_fw_version_ext_t *version_ext);

/**
 * Get the cached string representation of the wlan firmware extended version.
 */
int wifi_get_device_firmware_version_ext(wifi_fw_version_ext_t *fw_ver_ext);

/**
 * Get the timestamp of the last command sent to the firmware
 *
 * @return Timestamp in millisec of the last command sent
 */
unsigned wifi_get_last_cmd_sent_ms(void);

uint32_t wifi_get_value1();

/**
 * This will update the last command sent variable value to current
 * time. This is used for power management.
 */
void wifi_update_last_cmd_sent_ms();

/**
 * Register an event queue with the wifi driver to receive events
 *
 * The list of events which can be received from the wifi driver are
 * enumerated in the file wifi_events.h
 *
 * @param[in] event_queue The queue to which wifi driver will post events.
 *
 * @note Only one queue can be registered. If the registered queue needs to
 * be changed unregister the earlier queue first.
 *
 * @return Standard SDK return codes
 */
int wifi_register_event_queue(os_queue_t *event_queue);

/**
 * Unregister an event queue from the wifi driver.
 *
 * @param[in] event_queue The queue to which was registered earlier with
 * the wifi driver.
 *
 * @return Standard SDK return codes
 */
int wifi_unregister_event_queue(os_queue_t *event_queue);

/**
 * Get the count of elements in the scan list
 *
 * @param[in,out] count Pointer to a variable which will hold the count after
 * this call returns
 *
 * @warn The count returned by this function is the current count of the
 * elements. A scan command given to the driver or some other background
 * event may change this count in the wifi driver. Thus when the API
 * \ref wifi_get_scan_result is used to get individual elements of the scan
 * list, do not assume that it will return exactly 'count' number of
 * elements. Your application should not consider such situations as a
 * major event.
 *
 * @return Standard SDK return codes.
 */
int wifi_get_scan_result_count(unsigned *count);

int wifi_get_scan_result(unsigned int index, struct wifi_scan_result **desc);
int wifi_deauthenticate(uint8_t *bssid);

int wifi_uap_start(int type, char *ssid, uint8_t *mac_addr,
		int security, char *passphrase,
		int channel, wifi_scan_chan_list_t scan_chan_list);

int wifi_uap_stop(int type);

void wifi_uap_set_beacon_period(const t_u16 beacon_period);
void wifi_uap_set_hidden_ssid(const t_u8 bcast_ssid_ctl);
int wifi_uap_ctrl_deauth(bool enable);
void wifi_uap_set_ecsa(const t_u8 chan_sw_count);

int wifi_get_uap_max_clients(unsigned int *max_sta_num);
int wifi_set_uap_max_clients(unsigned int *max_sta_num);

int wifi_get_mgmt_ie(unsigned int bss_type, unsigned int index,
			void *buf, unsigned int *buf_len);
int wifi_set_mgmt_ie(unsigned int bss_type, unsigned int index,
			void *buf, unsigned int buf_len);
int wifi_clear_mgmt_ie(unsigned int bss_type, unsigned int index);

int wifi_send_enable_supplicant(int mode, const char *ssid);
int wifi_send_clear_wpa_psk(int mode, const char *ssid);
int wifi_send_add_wpa_psk(int mode, char *ssid,
			  char *passphrase, unsigned int len);
int wifi_send_get_wpa_pmk(int mode, char *ssid);
int wifi_send_add_wpa_pmk(int mode, char *bssid, char *ssid,
			  char *pmk, unsigned int len);

/**
 * Returns the current STA list connected to our uAP
 *
 * This function gets its information after querying the firmware. It will
 * block till the response is received from firmware or a timeout.
 *
 * @param[in, out] list After this call returns this points to the
 * structure \ref sta_list_t allocated by the callee. This is variable
 * length structure and depends on count variable inside it. <b> The caller
 * needs to free this buffer after use.</b>. If this function is unable to
 * get the sta list, the value of list parameter will be NULL
 *
 * \note The caller needs to explicitly free the buffer returned by this
 * function.
 *
 * @return void
 */
int wifi_uap_bss_sta_list(sta_list_t **list);

void wifi_enable_low_pwr_mode();

/**
 * Enable ED MAC mode in Wireless Firmware.
 *
 */
void wifi_enable_ed_mac_mode(short offset);

/** Set wifi calibration data in firmware.
 *
 * This function may be used to set wifi calibration data in firmware.
 *
 * @param[in] cdata The calibration data
 * @param[in] clen Length of calibration data
 *
 */
void wifi_set_cal_data(uint8_t *cdata, unsigned int clen);

/** Set wifi MAC address in firmware at load time.
 *
 * This function may be used to set wifi MAC address in firmware.
 *
 * @param[in] mac The new MAC Address
 *
 */
void wifi_set_mac_addr(uint8_t *mac);

/** Set wifi MAC address in firmware at run time.
 *
 * This function may be used to set wifi MAC address in firmware.
 *
 * @param[in] mac The new MAC Address
 *
 */
void _wifi_set_mac_addr(uint8_t *mac);

int wifi_sniffer_start(const t_u16 filter_flags, const t_u8 radio_type,
			const t_u8 channel);
int wifi_sniffer_status();
int wifi_sniffer_stop();

int wifi_set_key(int bss_index, bool is_pairwise, const uint8_t key_index,
		const uint8_t *key, unsigned key_len, const uint8_t *mac_addr);
int wifi_set_igtk_key(int bss_index, const uint8_t *pn,
		const uint16_t key_index,
		const uint8_t *key, unsigned key_len);
int wifi_remove_key(int bss_index, bool is_pairwise,
		const uint8_t key_index, const uint8_t *mac_addr);
#ifdef CONFIG_P2P
int wifi_register_wfd_event_queue(os_queue_t *event_queue);
int wifi_unregister_wfd_event_queue(os_queue_t *event_queue);
void wifi_wfd_event(bool peer_event, bool action_frame, void *data);
int wifi_wfd_start(char *ssid, int security, char *passphrase, int channel);
int wifi_wfd_stop(void);

/**
 * Returns the current STA list connected to our WFD
 *
 * This function gets its information after querying the firmware. It will
 * block till the response is received from firmware or a timeout.
 *
 * @param[in, out] list After this call returns this points to the
 * structure \ref sta_list_t allocated by the callee. This is variable
 * length structure and depends on count variable inside it. <b> The caller
 * needs to free this buffer after use.</b>. If this function is unable to
 * get the sta list, the value of list parameter will be NULL
 *
 * \note The caller needs to explicitly free the buffer returned by this
 * function.
 *
 * @return void
 */
int wifi_wfd_bss_sta_list(sta_list_t **list);


int wifi_get_wfd_mac_address(void);
int  wifi_wfd_ps_inactivity_sleep_enter(unsigned int ctrl_bitmap,
			       unsigned int inactivity_to,
			       unsigned int min_sleep,
			       unsigned int max_sleep,
				unsigned int min_awake, unsigned int max_awake);

int  wifi_wfd_ps_inactivity_sleep_exit();
int wifidirectapcmd_sys_config();
void wifidirectcmd_config();
#endif

int wifi_get_wpa_ie_in_assoc(uint8_t *wpa_ie);

/** Add Multicast Filter by MAC Address
 *
 * Multicast filters should be registered with the WiFi driver for IP-level
 * multicast addresses to work. This API allows for registration of such filters
 * with the WiFi driver.
 *
 * If multicast-mapped MAC address is 00:12:23:34:45:56 then pass mac_addr as
 * below:
 * mac_add[0] = 0x00
 * mac_add[1] = 0x12
 * mac_add[2] = 0x23
 * mac_add[3] = 0x34
 * mac_add[4] = 0x45
 * mac_add[5] = 0x56
 *
 * \param[in] mac_addr multicast mapped MAC address
 *
 * \return 0 on Success or else Error
 */
int wifi_add_mcast_filter(uint8_t *mac_addr);

/** Remove Multicast Filter by MAC Address
 *
 * This function removes multicast filters for the given multicast-mapped
 * MAC address. If multicast-mapped MAC address is 00:12:23:34:45:56
 * then pass mac_addr as below:
 * mac_add[0] = 0x00
 * mac_add[1] = 0x12
 * mac_add[2] = 0x23
 * mac_add[3] = 0x34
 * mac_add[4] = 0x45
 * mac_add[5] = 0x56
 *
 * \param[in] mac_addr multicast mapped MAC address
 *
 * \return  0 on Success or else Error
 */
int wifi_remove_mcast_filter(uint8_t *mac_addr);

/** Get Multicast Mapped Mac address from IPv4
 *
 * This function will generate Multicast Mapped MAC address from IPv4
 * Multicast Mapped MAC address will be in following format:
 * 1) Higher 24-bits filled with IANA Multicast OUI (01-00-5E)
 * 2) 24th bit set as Zero
 * 3) Lower 23-bits filled with IP address (ignoring higher 9bits).
 *
 * \param[in] mac_addr ipaddress(input)
 * \param[in] mac_addr multicast mapped MAC address(output)
 *
 * \return  void
 */
void wifi_get_ipv4_multicast_mac(uint32_t ipaddr, uint8_t *mac_addr);


#ifdef CONFIG_IPV6
/** Get Multicast Mapped Mac address from IPv6 address
 *
 * This function will generate Multicast Mapped MAC address from IPv6 address.
 * Multicast Mapped MAC address will be in following format:
 * 1) Higher 16-bits filled with IANA Multicast OUI (33-33)
 * 2) Lower 32-bits filled with last 4 bytes of IPv6 address
 *
 * \param[in] ipaddr last 4 bytes of IPv6 address
 * \param[in] mac_addr multicast mapped MAC address
 *
 * \return void
 */
void wifi_get_ipv6_multicast_mac(uint32_t ipaddr, uint8_t *mac_addr);
#endif /* CONFIG_IPV6 */

int wifi_get_antenna(uint16_t *ant_mode);
int wifi_set_antenna(uint16_t ant_mode);

void wifi_process_hs_cfg_resp(t_u8 *cmd_res_buffer);
int wifi_process_ps_enh_response(t_u8 *cmd_res_buffer, t_u16 *ps_event,
				 t_u16 *action);

int wifi_uap_tx_power_getset(uint8_t action, uint8_t *tx_power_dbm);
int wifi_uap_rates_getset(uint8_t action, char *rates, uint8_t num_rates);
int wifi_uap_mcbc_rate_getset(uint8_t action, uint16_t *mcbc_rate);
int wifi_uap_sta_ageout_timer_getset(uint8_t action,
				uint32_t *sta_ageout_timer);
int wifi_uap_ps_sta_ageout_timer_getset(uint8_t action,
				uint32_t *sta_ageout_timer);
int wifi_uap_group_rekey_timer_getset(uint8_t action,
			uint32_t *group_rekey_timer);
typedef enum {
	REG_MAC = 1,
	REG_BBP,
	REG_RF
} wifi_reg_t;

int wifi_reg_access(wifi_reg_t reg_type, uint16_t action,
			uint32_t offset, uint32_t *value);

int wifi_mem_access(uint16_t action, uint32_t addr, uint32_t *value);
int wifi_get_eeprom_data(uint32_t offset, uint32_t byte_count, uint8_t *buf);
/*
 * This function is supposed to be called after scan is complete from wlc
 * manager.
 */
void wifi_scan_process_results(void);

/**
 * Get the wifi region code
 *
 * This function will return one of the following values in the region_code
 * variable.\n
 * 0x10 : US FCC\n
 * 0x20 : CANADA\n
 * 0x30 : EU\n
 * 0x32 : FRANCE\n
 * 0x40 : JAPAN\n
 * 0x41 : JAPAN\n
 * 0x50 : China\n
 * 0xfe : JAPAN\n
 * 0xff : Special\n
 *
 * @return Standard WMSDK return codes.
 */
int wifi_get_region_code(t_u32 *region_code);

/**
 * Set the wifi region code.
 *
 * This function takes one of the values from the following array.\n
 * 0x10 : US FCC\n
 * 0x20 : CANADA\n
 * 0x30 : EU\n
 * 0x32 : FRANCE\n
 * 0x40 : JAPAN\n
 * 0x41 : JAPAN\n
 * 0x50 : China\n
 * 0xfe : JAPAN\n
 * 0xff : Special\n
 *
 * @return Standard WMSDK return codes.
 */
int wifi_set_region_code(t_u32 region_code);

/**
 * Get the uAP channel number
 *
 *
 * @param[in] channel Pointer to channel number. Will be initialized by
 * callee
 * @return Standard WMSDK return code
 */
int wifi_get_uap_channel(int *channel);

/**
 * Get/Set uAP's PMF parameters
 *
 * @param[in] action Get or Set
 * @param[in/out] Management frame protection capable
 * @param[in/out] Management frame protection required
 *
 * @return Standard WMSDK return code
 */
int wifi_uap_pmf_getset(uint8_t action, uint8_t *mfpc, uint8_t *mfpr);

/**
 * Sets the domain parameters for the uAP.
 *
 * @note This API only saves the domain params inside the driver internal
 * structures. The actual application of the params will happen only during
 * starting phase of uAP. So, if the uAP is already started then the
 * configuration will not apply till uAP re-start.
 *
 * @ To use this API you will need to fill up the structure
 * \ref wifi_domain_param_t with correct parameters.
 *
 * E.g. Programming for US country code
 * ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 *	wifi_sub_band_set_t sb = {
 *		.first_chan = 1,
 *		.no_of_chan= 11,
 *		.max_tx_pwr = 30,
 *	};
 *
 *	wifi_domain_param_t *dp = os_mem_alloc(sizeof(wifi_domain_param_t) +
 *					       sizeof(wifi_sub_band_set_t));
 *
 *	memcpy(dp->country_code, "US\0", COUNTRY_CODE_LEN);
 *	dp->no_of_sub_band = 1;
 *	memcpy(dp->sub_band, &sb, sizeof(wifi_sub_band_set_t));
 *
 *	wmprintf("wifi uap set domain params\n\r");
 *	wifi_uap_set_domain_params(dp);
 *	os_mem_free(dp);
 *
 */
int wifi_uap_enable_11d();
int wifi_uap_enable_11d_support();
int wifi_uap_set_domain_params(wifi_domain_param_t *dp);
int wifi_uap_set_params();
bool wifi_11d_is_channel_allowed(int channel);
wifi_sub_band_set_t *get_sub_band_from_country(int country, int *nr_sb);
int wifi_enable_11d_support();
int wifi_enable_11d_support_APIs();
int wifi_set_domain_params(wifi_domain_param_t *dp);
int wifi_set_country(int country);
int wifi_get_country();
int wifi_get_tx_power();
int wifi_set_tx_power(int power_level);
int wifi_set_smart_mode_cfg(char *ssid, int beacon_period,
			    wifi_chan_list_param_set_t *chan_list,
			    uint8_t *smc_start_addr, uint8_t *smc_end_addr,
			    uint16_t filter_type, int smc_frame_filter_len,
			    uint8_t *smc_frame_filter, int custom_ie_len,
			    uint8_t *custom_ie);
int wifi_get_smart_mode_cfg();
int wifi_start_smart_mode();
int wifi_stop_smart_mode();
int wifi_set_tx_rate_index(int rate_index);
int wifi_get_tx_rate_index();
bool _wifi_get_tx_rate_index(int *rate_index);

int wifi_send_scan_cmd(t_u8 bss_mode, const t_u8 *specific_bssid,
			const char *ssid, const char *ssid2,
			const t_u8 num_channels,
			const wifi_scan_channel_list_t *chan_list,
			const t_u8 num_probes,
			const bool keep_previous_scan);
void wifi_scan_enable_wpa2_enterprise_ap_only();

int wifi_send_remain_on_channel_cmd(unsigned int bss_type,
			wifi_remain_on_channel_t *remain_on_channel);
int wifi_set_trpc_params(chan_trpc_t *chan_trpc);

int wifi_get_trpc_params(chan_trpc_t *chan_trpc);

void wifi_set_txpwrlimit_data(const uint8_t country,
		const uint8_t channels, const uint8_t **data);

/**
 * Get User Data from OTP Memory
 *
 * \param[in] buf Pointer to buffer where data will be stored
 * \param[in] len Number of bytes to read
 *
 * \return WM_SUCCESS if user data read operation is successful.
 * \return -WM_FAIL if user data field is not present or command fails.
 */
int wifi_get_otp_user_data(uint8_t *buf, uint16_t len);

/**
 * Get Calibration data from WLAN firmware
 *
 * \param[out] cal_data Pointer to calibration data structure where
 *	      calibration data and it's length will be stored.
 *
 * \return WM_SUCCESS if cal data read operation is successful.
 * \return -WM_FAIL if cal data field is not present or command fails.
 *
 * \note The user of this API should free the allocated buffer for
 *	 calibration data.
 *
 */
int wifi_get_cal_data(wifi_cal_data_t *cal_data);

int wifi_auto_reconnect_enable(wifi_auto_reconnect_config_t
		auto_reconnect_config);

int wifi_auto_reconnect_disable();

int wifi_get_auto_reconnect_config(wifi_auto_reconnect_config_t
		*auto_reconnect_config);

int wrapper_wlan_11d_enable();

int wifi_set_rx_mgmt_indication(unsigned int bss_type,
			unsigned int mgmt_subtype_mask);

int wrapper_wlan_cmd_11n_addba_rspgen(void *saved_event_buff);

int wrapper_wlan_cmd_11n_delba_rspgen(void *saved_event_buff);

char *wifi_get_country_str(int country);

int wifi_country_trpc_cfg_data(struct pwr_table *tp, uint8_t country,
		chan_trpc_t *chan_trpc);

int wrapper_wlan_sta_ampdu_enable();

int wrapper_wlan_upa_ampdu_enable(uint8_t *addr);

/** Enable Bridge mode in WLAN firmware.
 *
 * \param[in] auto_link, Whether enable auto link for in-sta of bridge mode.
 * \param[in] hidden_ssid, Whether enable hidden_ssid for in-AP of bridge mode.
 * \param[in] cfg, Bridge configuration structure holding enable, auto_link,
 *	      hidden_ssid, EX-AP SSID, Passphrase, Bridge SSID and Passphrase.
 *
 * \return WM_SUCCESS if operation is successful.
 * \return -WM_FAIL if operation is failed.
 */
int wifi_enable_bridge_mode(wifi_bridge_cfg_t *cfg);

/** Disable Bridge mode in WLAN firmware.
 *
 * \return WM_SUCCESS if operation is successful.
 * \return -WM_FAIL if operation is failed.
 */
int wifi_disable_bridge_mode();

/** Get Bridge configuration from WLAN firmware.
 *
 * \param[out] cfg Bridge configuration structure where EX-AP SSID,
 *             Passphrase, Bridge SSID and Passphrase will get copied.
 *
 * \return WM_SUCCESS if operation is successful.
 * \return -WM_FAIL if operation is failed.
 */
int wifi_get_bridge_mode_config(wifi_bridge_cfg_t *cfg);

/**
 * Reconfigure TX buffer size during bridge mode operation.
 *
 * \param[in] buf_size Buffer size to configure.
 *
 * \return WM_SUCCESS if operation is successful.
 * \return -WM_FAIL is operation is failed.
 */
int wifi_config_bridge_tx_buf(uint16_t buf_size);

typedef PACK_START struct {
	/** Statistics counter */
	/** Multicast transmitted frame count */
	t_u32 mcast_tx_frame;
	/** Failure count */
	t_u32 failed;
	/** Retry count */
	t_u32 retry;
	/** Multi entry count */
	t_u32 multi_retry;
	/** Duplicate frame count */
	t_u32 frame_dup;
	/** RTS success count */
	t_u32 rts_success;
	/** RTS failure count */
	t_u32 rts_failure;
	/** Ack failure count */
	t_u32 ack_failure;
	/** Rx fragmentation count */
	t_u32 rx_frag;
	/** Multicast Tx frame count */
	t_u32 mcast_rx_frame;
	/** FCS error count */
	t_u32 fcs_error;
	/** Tx frame count */
	t_u32 tx_frame;
	/** WEP ICV error count */
	t_u32 wep_icv_error[4];
} PACK_END wlan_pkt_stats_t;

int wifi_get_log(wlan_pkt_stats_t *stats);
#endif
