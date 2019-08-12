/**
 * \file            esp_netconn.h
 * \brief           API functions for sequential calls
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
#ifndef __ESP_NETCONN_H
#define __ESP_NETCONN_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "esp/esp.h"

/**
 * \ingroup         ESP_API
 * \defgroup        ESP_NETCONN Network connection
 * \brief           Network connection
 * \{
 */

struct esp_netconn_t;

/**
 * \brief           Netconn object structure
 */
typedef struct esp_netconn_t* esp_netconn_p;
 
/**
 * \brief           Netconn connection type
 */
typedef enum {
    ESP_NETCONN_TYPE_TCP = ESP_CONN_TYPE_TCP,   /*!< TCP connection */
    ESP_NETCONN_TYPE_SSL = ESP_CONN_TYPE_SSL,   /*!< SSL connection */
    ESP_NETCONN_TYPE_UDP = ESP_CONN_TYPE_UDP,   /*!< UDP connection */
} esp_netconn_type_t;

esp_netconn_p   esp_netconn_new(esp_netconn_type_t type);
espr_t          esp_netconn_delete(esp_netconn_p nc);
espr_t          esp_netconn_bind(esp_netconn_p nc, esp_port_t port);
espr_t          esp_netconn_connect(esp_netconn_p nc, const char* host, esp_port_t port);
espr_t          esp_netconn_receive(esp_netconn_p nc, esp_pbuf_p* pbuf);
espr_t          esp_netconn_close(esp_netconn_p nc);
int8_t          esp_netconn_getconnnum(esp_netconn_p nc);
void            esp_netconn_set_receive_timeout(esp_netconn_p nc, uint32_t timeout);
uint32_t        esp_netconn_get_receive_timeout(esp_netconn_p nc);

/* TCP only */
espr_t          esp_netconn_listen(esp_netconn_p nc);
espr_t          esp_netconn_accept(esp_netconn_p nc, esp_netconn_p* new_api);
espr_t          esp_netconn_write(esp_netconn_p nc, const void* data, size_t btw);
espr_t          esp_netconn_flush(esp_netconn_p nc);

/* UDP only */
espr_t          esp_netconn_send(esp_netconn_p nc, const void* data, size_t btw);
espr_t          esp_netconn_sendto(esp_netconn_p nc, const esp_ip_t* ip, esp_port_t port, const void* data, size_t btw);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ESP_NETCONN_H */
