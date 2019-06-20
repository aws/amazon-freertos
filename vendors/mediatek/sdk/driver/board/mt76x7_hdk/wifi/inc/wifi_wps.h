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

#ifndef __WIFI_WPS_H__
#define __WIFI_WPS_H__

#if defined(MTK_WIFI_WPS_ENABLE)

#include <stdint.h>
#include <stdio.h>

/**
* @brief This function is the WPS credential process.
*
* @param[in] event indicates the Wi-Fi Credential Event.
*
* Value                                                            |Definition                                                |
* ------------------------------|------------------------------------------------------------------------|
* \b  #WIFI_EVENT_IOT_WPS_COMPLETE           |Get credential event, Enrollee get credential from Registrar.|
*
* @param[in]  payload is the credential message.
*
* @param[in]  length is the credential length,
*
* @return  >=0 get the total credential, <0 can't get the total credential.
*/
int32_t wifi_wps_credential_event_handler(wifi_event_t event, uint8_t *payload, uint32_t length);

/**
* @brief This function is register WPS credential handler example.
*
* @return  >=0 means success, <0 means fail.
*/
int32_t wifi_wps_register_credential_handler(void);

/**
* @brief This function is unregister WPS credential handler example.
*
* @return  >=0 means success, <0 means fail.
*/
int32_t wifi_wps_unregister_credential_handler(void);

/**
* @brief This function gets the device information for WPS from NVRAM
*
* @param[out] device_info holds the device information after this function being called.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note This function is used by Supplicant for WPS initialization, and this function might be removed next version.
*/
int32_t wifi_wps_get_device_info(wifi_wps_device_info_t *device_info);

#endif /* MTK_WIFI_WPS_ENABLE */

#endif /* __WIFI_SCAN_H */

