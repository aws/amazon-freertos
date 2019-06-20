/**
 * \file            esp_includes.h
 * \brief           All main includes
 */

/*
 * Copyright (c) 2018 Tilen Majerle
 *  
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of ESP-AT.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#ifndef __ESP_INCLUDES_H
#define __ESP_INCLUDES_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "esp_config.h"
#include "esp/esp_typedefs.h"
#include "esp/esp_buff.h"
#include "esp/esp_input.h"
#include "system/esp_sys.h"
#include "esp/esp_evt.h"
#include "esp/esp_debug.h"
#include "esp/esp_utilities.h"
#include "esp/esp_pbuf.h"
#include "esp/esp_conn.h"

#if ESP_CFG_MODE_STATION
#include "esp/esp_sta.h"
#endif /* ESP_CFG_MODE_STATION */
#if ESP_CFG_MODE_ACCESS_POINT
#include "esp/esp_ap.h"
#endif /* ESP_CFG_MODE_ACCESS_POINT */
#if ESP_CFG_OS
#include "esp/esp_netconn.h"
#endif /* ESP_CFG_OS */
#if ESP_CFG_PING
#include "esp/esp_ping.h"
#endif /* ESP_CFG_PING */
#if ESP_CFG_WPS
#include "esp/esp_wps.h"
#endif /* ESP_CFG_WPS */
#if ESP_CFG_SNTP
#include "esp/esp_sntp.h"
#endif /* ESP_CFG_SNTP */
#if ESP_CFG_HOSTNAME
#include "esp/esp_hostname.h"
#endif /* ESP_CFG_HOSTNAME */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ESP_INCLUDES_H */
