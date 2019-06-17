/**
 * \file            esp_rest_client.h
 * \brief           HTTP REST client implementation based on NETCONN API
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
#ifndef __ESP_HTTP_CLIENT_H
#define __ESP_HTTP_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "esp/esp.h"

/**
 * \ingroup         ESP_API
 * \defgroup        ESP_REST_CLIENT HTTP REST client
 * \brief           HTTP REST client
 * \{
 */

/**
 * \brief           HTTP REST handle
 */
typedef struct {
    esp_netconn_p nc;                           /*!< Netconn sequential API handle */
} esp_rest_t;

espr_t  esp_rest_execute(esp_http_method_t m, const char* uri, const void* tx_data, size_t tx_len, uint16_t* http_code, esp_pbuf_p* p, size_t* p_off);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ESP_HTTP_CLIENT_H */
