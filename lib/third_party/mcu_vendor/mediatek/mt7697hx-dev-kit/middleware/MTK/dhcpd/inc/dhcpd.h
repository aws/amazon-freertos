/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __DHCPD_H__
#define __DHCPD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ip4_addr.h"
#include "syslog.h"

/* Save client config when AP is keeping power on.
  * Client config will not be stored when AP is power off. */
#define DHCPD_SAVE_CLIENT_CONFIG_ON_LINE


/**
 * @addtogroup DHCPD
 * @{
 * This section introduces the DHCP daemon (DHCPD) APIs including details on how to use this module, enums, structures and functions.
 * It's a simple DHCP daemon to operate in soft AP mode for assigning IP addresses to connected Wi-Fi station nodes.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b DHCP                       | Dynamic Host Configuration Protocol. |
 * |\b Wi-Fi                      | Wi-Fi is a local area wireless computer networking technology, https://en.wikipedia.org/wiki/Wi-Fi |
 *
 * @section dhcpd_api_usage How to use this module
 *
 * - Step1: Set DHCPD parameters. 
 *              If you want to use all default DHCPD parameters(DHPCD_DEFAULT_X, such as DHPCD_DEFAULT_SERVER_IP),
 *              skip this step.
 *
 * - Step2: Call #dhcpd_start() to start the service.
 *              If you want to use all default DHCPD parameters(DHPCD_DEFAULT_X, such as DHPCD_DEFAULT_SERVER_IP),
 *              just call #dhcpd_start(NULL).
 *
 *  - Example code:
 *     @code
 *        xTaskHandle dhcpd_task;
 *        int32_t process_softap(void)
 *        {
 *            LOG_I(common, "Now start dhcpd:");
 *             dhcpd_settings_t dhcpd_settings;
 *             
 *		  memset(&dhcpd_settings, 0, sizeof(dhcpd_settings_t));
 *		  strncpy(dhcpd_settings.dhcpd_server_address, "10.10.10.2" , IP4ADDR_STRLEN_MAX - 1);
 *		  strncpy(dhcpd_settings.dhcpd_gateway, "10.10.10.1" , IP4ADDR_STRLEN_MAX - 1);
 *		  strncpy(dhcpd_settings.dhcpd_netmask, "255.255.255.0" , IP4ADDR_STRLEN_MAX - 1);
 *		  strncpy(dhcpd_settings.dhcpd_primary_dns, "8.8.8.8" , IP4ADDR_STRLEN_MAX - 1);
 *		  strncpy(dhcpd_settings.dhcpd_secondary_dns, "8.8.4.4" , IP4ADDR_STRLEN_MAX - 1);
 *		  strncpy(dhcpd_settings.dhcpd_ip_pool_start, "10.10.10.3" , IP4ADDR_STRLEN_MAX - 1);
 *		  strncpy(dhcpd_settings.dhcpd_ip_pool_end, "10.10.10.11" , IP4ADDR_STRLEN_MAX - 1);
 *
 *		  dhcpd_start(&dhcpd_settings);
 *
 *            return 0;
 *        }
 *     @endcode
 */

/**
 * @addtogroup dhcpd_define Define
 * @{
 */

/**
 * @brief DHCPD IP default lease time is in seconds
 */
#define DHCPD_DEFAULT_LEASE_TIME 64800     // 18 hours

/**
 * @brief default pool size of the DHCPD.
 */
#define DHCPD_DEFAULT_MAX_LEASE_NUM 10


/* Default Settings for DHCPD */

#define DHPCD_DEFAULT_SERVER_IP			("10.10.10.1")
#define DHPCD_DEFAULT_NETMASK			("255.255.255.0")
#define DHPCD_DEFAULT_GATEWAY			("10.10.10.1")
#define DHPCD_DEFAULT_PRIMARY_DNS		("8.8.8.8")
#define DHPCD_DEFAULT_SECONDARY_DNS		("8.8.4.4")
#define DHPCD_DEFAULT_IP_POOL_START		("10.10.10.2")
#define DHPCD_DEFAULT_IP_POOL_END		("10.10.10.10")

typedef struct
{
	char dhcpd_server_address[IP4ADDR_STRLEN_MAX];
	char dhcpd_gateway[IP4ADDR_STRLEN_MAX];
	char dhcpd_netmask[IP4ADDR_STRLEN_MAX];
	char dhcpd_primary_dns[IP4ADDR_STRLEN_MAX];
	char dhcpd_secondary_dns[IP4ADDR_STRLEN_MAX];
	char dhcpd_ip_pool_start[IP4ADDR_STRLEN_MAX];
	char dhcpd_ip_pool_end[IP4ADDR_STRLEN_MAX];
} dhcpd_settings_t;


/**
 * @}
 */

/**
 * @brief This function has been phased out.
 *           This function sets the range of the IP addresses to lease by specifying the last number of the
 *           first and the last IP addresses. This function can only be called before #dhcp_start().
 * @param[in] start is the last number of the first IP address allocated for the DHCP server.
 * @param[in] end is the last number of the last IP address allocated for the DHCP server.
 * @sa #dhcpd_start();
 * @return None
 */
void dhcpd_set_lease(uint8_t start, uint8_t end);


/**
 * @brief This function starts the DHCPD service. It creates a new task for the DHCPD, then returns. The DHCPD task runs until dhcpd_stop() is called.
 * @dhcpd_settings[in] . Settings for DHCPD, such as dhcp server ip, gateway, netmask, dns, and ip pool. These settings can be set individually.
 *                                If the setting is not set, default value will be used. Be ware that when both default values and customer configurated
 *                                value are used, they should match to each other.
 * @sa #dhcpd_stop();
 * @return None
 */
int dhcpd_start(dhcpd_settings_t *dhcpd_settings);


/**
 * @brief This function stops the DHCPD service. It kills the DHCPD task, and stops the DHCPD service immediately.
 * @sa #dhcpd_start();
 * @return None
 */
void dhcpd_stop(void);

/**
 * @}
 */

#undef MTK_DEBUG_LEVEL_NONE

/* Define 0 to disable logging, define 1 to enable logging. */
#ifndef MTK_DEBUG_LEVEL_NONE
#define DHCPD_DEBUG 1
#else
#define DHCPD_DEBUG 0
#endif

/* The following content is used inside the DHCPD module. */
#if DHCPD_DEBUG
#define DHCPD_printf(x, ...) LOG_I(dhcpd, x, ##__VA_ARGS__)
#define DHCPD_WARN(x, ...) LOG_W(dhcpd, x, ##__VA_ARGS__)
#define DHCPD_ERR(x, ...) LOG_E(dhcpd, x, ##__VA_ARGS__)
#else
#define DHCPD_printf(x, ...)
#define DHCPD_WARN(x, ...)
#define DHCPD_ERR(x, ...)
#endif


#ifdef __cplusplus
}
#endif

#endif /* __DHCPD_H__ */
