/**
 * \file            esp.h
 * \brief           ESP AT commands parser
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
#ifndef __ESP_H
#define __ESP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Get most important include files */
#include "esp/esp_includes.h"

/**
 * \defgroup        ESP ESP-AT Lib
 * \brief           ESP stack
 * \{
 */

espr_t      esp_init(esp_cb_fn cb_func, uint32_t blocking);
espr_t      esp_reset(uint32_t blocking);
espr_t      esp_reset_with_delay(uint32_t delay, uint32_t blocking);

espr_t      esp_restore(uint32_t blocking);
espr_t      esp_set_at_baudrate(uint32_t baud, uint32_t blocking);
espr_t      esp_set_wifi_mode(esp_mode_t mode, uint32_t blocking);

espr_t      esp_set_server(uint8_t en, esp_port_t port, uint16_t max_conn, uint16_t timeout, esp_cb_fn cb, uint32_t blocking);

espr_t      esp_dns_getbyhostname(const char* host, esp_ip_t* ip, uint32_t blocking);

espr_t      esp_update_sw(uint32_t blocking);

espr_t      esp_core_lock(void);
espr_t      esp_core_unlock(void);

espr_t      esp_cb_register(esp_cb_fn cb_fn);
espr_t      esp_cb_unregister(esp_cb_fn cb_fn);

espr_t      esp_device_set_present(uint8_t present, uint32_t blocking);
uint8_t     esp_device_is_present(void);

void        esp_delay(uint32_t ms);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ESP_H */
