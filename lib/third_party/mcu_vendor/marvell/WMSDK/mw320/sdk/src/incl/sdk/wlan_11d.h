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

/*! \file wlan_11d.h
 * \brief WLAN module 11d API
 */

#ifndef __WLAN_11D_H__
#define __WLAN_11D_H__

#include <wifi.h>

/** 802.11d country codes */
typedef PACK_START enum
{
	/** US FCC */
	COUNTRY_US = 1,
	/** IC Canada */
	COUNTRY_CA,
	/** Singapore */
	COUNTRY_SG,
	/** ETSI */
	COUNTRY_EU,
	/** Australia */
	COUNTRY_AU,
	/** Republic Of Korea */
	COUNTRY_KR,
	/** France */
	COUNTRY_FR,
	/** Japan */
	COUNTRY_JP,
	/** China */
	COUNTRY_CN,
} PACK_END country_code_t;

/** wlan_11d Wi-Fi Region Configuration
 * By default, the SDK builds applications that are compliant with the
 * US region configuration. This implies that the module obeys the
 * US regulations for Wi-Fi transmissions on certified frequency bands. The
 * SDK provides mechanism for configuring various region codes in the
 * applications.
 * This can be performed in one of the following two ways:
 *
 * I) Specifying Country Code
 *
 * In this method of configuration, the application defines up-front
 * what is the country code that the MC200/MW300 is going to be deployed in.
 * Once configured the Wi-Fi firmware obeys the configured
 * countries regulations. This configuration can be set by making a call to the
 * wlan_set_country() API. This API should be called after WLAN is initialized
 * but before starting uAP or making any connection attempts on station
 * interface.
 *
 * For example:
 * wlan_set_country(COUNTRY_CN);
 *
 * II) Using 802.11D
 *
 * \note The FCC does not allow the use of 802.11D in the US starting Jan 1,
 * 2015.
 * In this method of configuration, the Wi-Fi driver of the SDK will
 * scan for Access Points in the vicinity and  accordingly configure
 * itself to operate in the available frequency bands. This configuration can
 * be set by making a call to the wlan_enable_11d() API. This API should be
 * called after WLAN is initialized but before starting uAP or making any
 * connection attempts on station interface.
 *
 * For example:
 * wlan_enable_11d();
 *
 * @{
 */

/** Enable 11D support in WLAN Driver.
 *
 * \note This API should be called after WLAN is initialized
 * but before starting uAP or making any connection attempts on station
 * interface.
 *
 * \note Either this function or wlan_set_country() should be used
 * at a time. If both functions are called in the application, then WLAN
 * Driver properties will be set as per the wlan_set_country() function.
 *
 * \return -WM_FAIL if operation was failed.
 * \return WM_SUCCESS if operation was successful.
 */
static inline int wlan_enable_11d()
{
	wifi_uap_enable_11d_support();
	return wifi_enable_11d_support();
}

/** Get country code from WLAN Driver.
 *
 * \note This API should be called after WLAN is initialized
 * but before starting uAP or making any connection attempts on station
 * interface.
 *
 * \return Country code. Refer to \ref country_code_t.
 *
 */
static inline int wlan_get_country()
{
	return wifi_get_country();
}

/** Set country code in WLAN Driver.
 *
 * \note This API should be called after WLAN is initialized
 * but before starting uAP or making any connection attempts on station
 * interface.
 *
 * \note Either this function or wlan_enable_11d() should be used
 * at a time. If both functions are called in the application, then WLAN
 * Driver properties will be set as per the wlan_set_country() function.
 *
 * \param[in] country Country code. Refer to \ref country_code_t.
 *
 * \return -WM_FAIL if operation was failed.
 * \return WM_SUCCESS if operation was successful.
 */
static inline int wlan_set_country(country_code_t country)
{
	int ret;
	chan_trpc_t chan_trpc;
	struct pwr_table *tp;

	tp = board_region_pwr_tbl((board_country_code_t) country);

	if (tp) {
		ret = wifi_country_trpc_cfg_data(tp, country, &chan_trpc);

		if (ret == WM_SUCCESS)
			wifi_set_trpc_params(&chan_trpc);
	}

	return wifi_set_country(country);
}

/**  wlan_11d_custom Custom Wi-Fi Region Configuration

   Ideally applications should use either wlan_enable_11d() or
   wlan_set_country() APIs to have standard 802.11d functionality as
   per regulations of Wi-Fi transmissions on certified frequency bands.

   But If application wants to configure custom 802.11d configurations
   then wlan_uap_set_domain_params() and wlan_set_domain_params APIs
   can be used for that.

   If applications just want to set a particular region then
   wlan_set_region_code() API can be used for the purpose.

   Supported region code values are given in mlan_11d.c file.

   Sets the domain parameters for the uAP.

	@note This API should be called after WLAN is initialized
	but before starting uAP

   To use this API you will need to fill up the structure
   \ref wifi_domain_param_t with correct parameters.

   The below section lists all the arrays that can be passed individually
   or in combination to the API wlan_uap_set_domain_params(). These are
   the sub band sets to be part of the Country Info IE in the uAP beacon.
   One of them is to be selected according to your region. Please have a look
   at the example given in the documentation below for reference.

   Supported Country Codes:
   "US" : USA,
   "CA" : Canada,
   "SG" : Singapore,
   "EU" : Europe,
   "AU" : Australia,
   "KR" : Republic of Korea,
   "CN" : China,
   "FR" : France,
   "JP" : Japan

\code

Region : US(US) or Canada(CA) or Singapore(SG) 2.4 GHz
wifi_sub_band_set_t subband_US_CA_SG_2_4_GHz[] = {
	{1, 11, 20}
};

Region: Europe(EU), Australia(AU), Republic of Korea(KR),
China(CN) 2.4 GHz
wifi_sub_band_set_t subband_EU_AU_KR_CN_2_4GHz[] = {
	{1, 13, 20}
};

Region: France(FR) 2.4 GHz
wifi_sub_band_set_t subband_FR_2_4GHz[] = {
	{1, 9, 20},
	{10, 4, 10}
};

Region: Japan(JP) 2.4 GHz
wifi_sub_band_set_t subband_JP_2_4GHz[] = {
	{1, 14, 20},
};

Region: Constrained 2.4 Ghz
wifi_sub_band_set_t subband_CS_2_4GHz[] = {
	{1, 9, 20},
	{10, 2, 10}
};

Region: US(US) or Singapore(SG) 5 GHz
wifi_sub_band_set_t subband_US_SG_5GHz[] = {
	{36, 1, 20},
	{40, 1, 20},
	{44, 1, 20},
	{48, 1, 20},
	{52, 1, 20},
	{56, 1, 20},
	{60, 1, 20},
	{64, 1, 20},
	{100, 1, 20},
	{104, 1, 20},
	{108, 1, 20},
	{112, 1, 20},
	{116, 1, 20},
	{120, 1, 20},
	{124, 1, 20},
	{128, 1, 20},
	{132, 1, 20},
	{136, 1, 20},
	{140, 1, 20},
	{149, 1, 20},
	{153, 1, 20},
	{157, 1, 20},
	{161, 1, 20},
	{165, 1, 20}
};

Region: Canada(CA) 5 GHz
wifi_sub_band_set_t subband_CA_5GHz[] = {
	{36, 1, 20},
	{40, 1, 20},
	{44, 1, 20},
	{48, 1, 20},
	{52, 1, 20},
	{56, 1, 20},
	{60, 1, 20},
	{64, 1, 20},
	{100, 1, 20},
	{104, 1, 20},
	{108, 1, 20},
	{112, 1, 20},
	{116, 1, 20},
	{132, 1, 20},
	{136, 1, 20},
	{140, 1, 20},
	{149, 1, 20},
	{153, 1, 20},
	{157, 1, 20},
	{161, 1, 20},
	{165, 1, 20}
};

Region: Europe/ETSI(EU), Australia(AU), Republic of Korea(KR) 5 GHz
wifi_sub_band_set_t subband_EU_AU_KR_5GHz[] = {
	{36, 1, 20},
	{40, 1, 20},
	{44, 1, 20},
	{48, 1, 20},
	{52, 1, 20},
	{56, 1, 20},
	{60, 1, 20},
	{64, 1, 20},
	{100, 1, 20},
	{104, 1, 20},
	{108, 1, 20},
	{112, 1, 20},
	{116, 1, 20},
	{120, 1, 20},
	{124, 1, 20},
	{128, 1, 20},
	{132, 1, 20},
	{136, 1, 20},
	{140, 1, 20}
};

Region: China(CN) 5 GHz
wifi_sub_band_set_t subband_CN_5GHz[] = {
	{149, 1, 33},
	{153, 1, 33},
	{157, 1, 33},
	{161, 1, 33},
	{165, 1, 33},
};

Region: France(FR) 5 GHz
wifi_sub_band_set_t subband_FR_5GHz[] = {
	{36, 1, 20},
	{40, 1, 20},
	{44, 1, 20},
	{48, 1, 20},
	{52, 1, 20},
	{56, 1, 20},
	{60, 1, 20},
	{64, 1, 20},
	{100, 1, 20},
	{104, 1, 20},
	{108, 1, 20},
	{112, 1, 20},
	{116, 1, 20},
	{120, 1, 20},
	{124, 1, 20},
	{128, 1, 20},
	{132, 1, 20},
	{136, 1, 20},
	{140, 1, 20},
	{149, 1, 20},
	{153, 1, 20},
	{157, 1, 20},
	{161, 1, 20},
	{165, 1, 20}
};

Region: Japan(JP) 5 GHz
wifi_sub_band_set_t subband_JP_5_GHz[] = {
	{8, 1, 23},
	{12, 1, 23},
	{16, 1, 23},
	{36, 1, 23},
	{40, 1, 23},
	{44, 1, 23},
	{48, 1, 23},
	{52, 1, 23},
	{56, 1, 23},
	{60, 1, 23},
	{64, 1, 23},
	{100, 1, 23},
	{104, 1, 23},
	{108, 1, 23},
	{112, 1, 23},
	{116, 1, 23},
	{120, 1, 23},
	{124, 1, 23},
	{128, 1, 23},
	{132, 1, 23},
	{136, 1, 23},
	{140, 1, 23}
};

	// We will be using the KR 2.4 and 5 GHz bands for this example

	int nr_sb = (sizeof(subband_EU_AU_KR_CN_2_4GHz)
		     + sizeof(subband_EU_AU_KR_5GHz))
		     / sizeof(wifi_sub_band_set_t);

	// We already have space for first sub band info entry in
	// wifi_domain_param_t
	wifi_domain_param_t *dp =
		os_mem_alloc(sizeof(wifi_domain_param_t) +
			     (sizeof(wifi_sub_band_set_t) * (nr_sb - 1)));

	// COUNTRY_CODE_LEN is 3. Add extra ' ' as country code is 2 characters
	memcpy(dp->country_code, "KR ", COUNTRY_CODE_LEN);

	dp->no_of_sub_band = nr_sb;
	memcpy(&dp->sub_band[0], &subband_EU_AU_KR_CN_2_4GHz[0],
	       1 * sizeof(wifi_sub_band_set_t));
	memcpy(&dp->sub_band[1], &subband_EU_AU_KR_5GHz,
	       (nr_sb - 1) * sizeof(wifi_sub_band_set_t));

	wlan_uap_set_domain_params(dp);
	os_mem_free(dp);
\endcode

\param[in] dp The wifi domain parameters

\return -WM_E_INVAL if invalid parameters were passed.
\return -WM_E_NOMEM if memory allocation failed.
\return WM_SUCCESS if operation was successful.
 */
static inline int wlan_uap_set_domain_params(wifi_domain_param_t *dp)
{
	return wifi_uap_set_domain_params(dp);
}

/**
   Sets the domain parameters for the Station interface.

   @note This API should be called after WLAN is initialized
   but before making any connection attempts on station interface.

   @note Refer wlan_uap_set_domain_params API documentation for
   \ref wifi_sub_band_set_t structures for different regions.

\code
	// We will be using the KR 2.4 and 5 GHz bands for this example

	int nr_sb = (sizeof(subband_EU_AU_KR_CN_2_4GHz)
			+ sizeof(subband_EU_AU_KR_5GHz))
			/ sizeof(wifi_sub_band_set_t);

	// We already have space for first sub band info entry in
	// wifi_domain_param_t
	wifi_domain_param_t *dp = os_mem_alloc(sizeof(wifi_domain_param_t) +
			(sizeof(wifi_sub_band_set_t) * (nr_sb - 1)));

	// COUNTRY_CODE_LEN is 3. Add extra ' ' as country code is 2 characters
	memcpy(dp->country_code, "KR ", COUNTRY_CODE_LEN);

	dp->no_of_sub_band = nr_sb;
	memcpy(&dp->sub_band[0], &subband_EU_AU_KR_CN_2_4GHz[0],
			1 * sizeof(wifi_sub_band_set_t));
	memcpy(&dp->sub_band[1], &subband_EU_AU_KR_5GHz,
			(nr_sb - 1) * sizeof(wifi_sub_band_set_t));

	wlan_set_domain_params(dp);
	os_mem_free(dp);
\endcode

\param[in] dp The wifi domain parameters

\return -WM_E_INVAL if invalid parameters were passed.
\return WM_SUCCESS if operation was successful.
*/
static inline int wlan_set_domain_params(wifi_domain_param_t *dp)
{
	return wifi_set_domain_params(dp);
}

/**
 Set 11D region code.

\param[in] region_code 11D region code to set.

\return -WM_FAIL if operation was failed.
\return WM_SUCCESS if operation was successful.
 */
static inline int wlan_set_region_code(uint32_t region_code)
{
	return wifi_set_region_code(region_code);
}

/**
 Get country string from country code

 This function converts country index to country string

\param[in] country Country index

\return Country string
 */
uint8_t *wlan_11d_country_index_2_string(int country);

/** @} */

#endif /* __WLAN_11D_H__ */
