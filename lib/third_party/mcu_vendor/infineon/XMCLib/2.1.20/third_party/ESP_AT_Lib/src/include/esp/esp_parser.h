/**
 * \file            esp_parser.h
 * \brief           Parser of AT responses
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
#ifndef __ESP_PARSER_H
#define __ESP_PARSER_H

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#include "esp/esp.h"

espr_t      espi_parse_cipstatus(const char* str);
espr_t      espi_parse_ipd(const char* str);
    
int32_t     espi_parse_number(const char** str);
uint8_t     espi_parse_string(const char** src, char* dst, size_t dst_len, uint8_t trim);
uint8_t     espi_parse_ip(const char** src, esp_ip_t* ip);
uint8_t     espi_parse_mac(const char** src, esp_mac_t* mac);
    
uint8_t     espi_parse_cwlap(const char* src, esp_msg_t* msg);
uint8_t     espi_parse_cwlif(const char* str, esp_msg_t* msg);
uint8_t     espi_parse_cipdomain(const char* src, esp_msg_t* msg);
uint8_t     espi_parse_ping_time(const char* str, esp_msg_t* msg);
uint8_t     espi_parse_cipsntptime(const char* str, esp_msg_t* msg);
uint8_t     espi_parse_hostname(const char* str, esp_msg_t* msg);
uint8_t     espi_parse_link_conn(const char* str);

uint8_t     espi_parse_at_sdk_version(const char* str, uint32_t* version_out);

uint8_t     espi_parse_ap_conn_disconn_sta(const char* str, uint8_t is_conn);
uint8_t     espi_parse_ap_ip_sta(const char* str);

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */

#endif /* __ESP_PARSER_H */
