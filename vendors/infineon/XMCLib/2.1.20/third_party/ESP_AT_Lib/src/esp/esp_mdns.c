/**	
 * \file            esp_mdns.c
 * \brief           mDNS API
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
#include "esp/esp_private.h"
#include "esp/esp_mdns.h"
#include "esp/esp_mem.h"

#if ESP_CFG_MDNS || __DOXYGEN__

/**
 * \brief           Configure mDNS parameters with hostname and server
 * \param[in]       en: Status to enable (`1`) or disable (`0`) mDNS function
 * \param[in]       host: mDNS host name
 * \param[in]       server: mDNS server name
 * \param[in]       port: mDNS server port number
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_mdns_configure(uint8_t en, const char* host, const char* server, esp_port_t port, uint32_t blocking) {
    ESP_MSG_VAR_DEFINE(msg);                    /* Define variable for message */
    
    if (en) {
        ESP_ASSERT("host != NULL", host != NULL);   /* Assert input parameters */
        ESP_ASSERT("server != NULL", server != NULL);   /* Assert input parameters */
        ESP_ASSERT("port", port);               /* Assert input parameters */
    }
		
    ESP_MSG_VAR_ALLOC(msg);                     /* Allocate memory for variable */
    ESP_MSG_VAR_REF(msg).cmd_def = ESP_CMD_WIFI_MDNS;
    ESP_MSG_VAR_REF(msg).msg.mdns.en = en;
    ESP_MSG_VAR_REF(msg).msg.mdns.host = host;
    ESP_MSG_VAR_REF(msg).msg.mdns.server = server;
    ESP_MSG_VAR_REF(msg).msg.mdns.port = port;

    return espi_send_msg_to_producer_mbox(&ESP_MSG_VAR_REF(msg), espi_initiate_cmd, blocking, 1000);/* Send message to producer queue */
}

#endif /* ESP_CFG_PING || __DOXYGEN__ */
