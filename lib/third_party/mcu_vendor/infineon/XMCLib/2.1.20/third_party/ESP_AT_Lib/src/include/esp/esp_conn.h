/**	
 * \file            esp_conn.h
 * \brief           Connection API
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
#ifndef __ESP_CONN_H
#define __ESP_CONN_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "esp/esp.h"
    
/**
 * \ingroup         ESP
 * \defgroup        ESP_CONN Connection API
 * \brief           Connection API functions
 * \{
 */
 
espr_t      esp_conn_start(esp_conn_p* conn, esp_conn_type_t type, const char* host, esp_port_t port, void* arg, esp_cb_fn cb_func, uint32_t blocking);
espr_t      esp_conn_close(esp_conn_p conn, uint32_t blocking);
espr_t      esp_conn_send(esp_conn_p conn, const void* data, size_t btw, size_t* bw, uint32_t blocking);
espr_t      esp_conn_sendto(esp_conn_p conn, const esp_ip_t* ip, esp_port_t port, const void* data, size_t btw, size_t* bw, uint32_t blocking);
espr_t      esp_conn_set_arg(esp_conn_p conn, void* arg);
void *      esp_conn_get_arg(esp_conn_p conn);
uint8_t     esp_conn_is_client(esp_conn_p conn);
uint8_t     esp_conn_is_server(esp_conn_p conn);
uint8_t     esp_conn_is_active(esp_conn_p conn);
uint8_t     esp_conn_is_closed(esp_conn_p conn);
int8_t      esp_conn_getnum(esp_conn_p conn);
espr_t      esp_conn_set_ssl_buffersize(size_t size, uint32_t blocking);
espr_t      esp_get_conns_status(uint32_t blocking);
esp_conn_p  esp_conn_get_from_evt(esp_cb_t* evt);
espr_t      esp_conn_write(esp_conn_p conn, const void* data, size_t btw, uint8_t flush, size_t* mem_available);
espr_t      esp_conn_recved(esp_conn_p conn, esp_pbuf_p pbuf);
 
/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* __ESP_CONN_H */
