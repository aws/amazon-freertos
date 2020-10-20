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

#ifndef __WIFI_LWIP_H__
#define __WIFI_LWIP_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  network init function. initial wifi and lwip config
  * @param None
  * @retval None
  */

void lwip_network_init(uint8_t opmode);


void lwip_net_start(uint8_t opmode);


void lwip_net_stop(uint8_t opmode);


/**
  * @brief  when wifi and ip ready will return.
  * @param None
  * @retval None
  */
void lwip_net_ready(void);

/**
* @note This api is only for internal use
*/
uint8_t wifi_set_opmode(uint8_t target_mode);


typedef enum lwip_helper_ip_addr_type {
    LWIP_HELPER_IPADDR_TYPE_V4 = 0,
    LWIP_HELPER_IPADDR_TYPE_V6,
    LWIP_HELPER_IPADDR_TYPE_UNSPEC
} lwip_helper_ip_addr_type_t;

/**
* @note IP address
*/
typedef struct wifi_lwip_helper_ip_addr {
    uint8_t type;         /**< e.g. IPV4/IPV6 */
    uint32_t ip_addr[4];  /**< IP address in binary form i.e. use inet_ntop/inet_pton for conversion */
} wifi_lwip_helper_ip_addr_t;


/**
* @note ip ready event callback func type.
*/
typedef void (*wifi_lwip_helper_ip_ready_callback_t)(const wifi_lwip_helper_ip_addr_t* ret_ip_addr);

/**
* @note register ip ready event callback.
*/
int8_t wifi_lwip_helper_register_ip_ready_callback(wifi_lwip_helper_ip_ready_callback_t ip_callback);

/**
* @note deregister ip ready event callback.
*/
int8_t wifi_lwip_helper_deregister_ip_ready_callback();


#ifdef __cplusplus
}
#endif

#endif /* __WIFI_LWIP_H__ */
