/**	
 * \file            esp_sta.c
 * \brief           Station API
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
#include "esp/esp_sta.h"
#include "esp/esp_mem.h"

#if ESP_CFG_MODE_STATION || __DOXYGEN__

/**
 * \brief           Quit (disconnect) from access point
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_sta_quit(uint32_t blocking) {
    ESP_MSG_VAR_DEFINE(msg);                    /* Define variable for message */
    
    ESP_MSG_VAR_ALLOC(msg);                     /* Allocate memory for variable */
    ESP_MSG_VAR_REF(msg).cmd_def = ESP_CMD_WIFI_CWQAP;
    
    return espi_send_msg_to_producer_mbox(&ESP_MSG_VAR_REF(msg), espi_initiate_cmd, blocking, 1000);    /* Send message to producer queue */
}

/**
 * \brief           Join as station to access point
 * \param[in]       name: SSID of access point to connect to
 * \param[in]       pass: Password of access point. Use `NULL` if AP does not have password
 * \param[in]       mac: Pointer to MAC address of AP. If you have APs with same name, you can use MAC to select proper one.
 *                      Use `NULL` if not needed
 * \param[in]       def: Status whether this is default SSID or only current one
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_sta_join(const char* name, const char* pass, const esp_mac_t* mac, uint8_t def, uint32_t blocking) {
    ESP_MSG_VAR_DEFINE(msg);                    /* Define variable for message */
    
    ESP_ASSERT("name != NULL", name != NULL);   /* Assert input parameters */
    
    ESP_MSG_VAR_ALLOC(msg);                     /* Allocate memory for variable */
    ESP_MSG_VAR_REF(msg).cmd_def = ESP_CMD_WIFI_CWJAP;
    ESP_MSG_VAR_REF(msg).msg.sta_join.def = def;
    ESP_MSG_VAR_REF(msg).msg.sta_join.name = name;
    ESP_MSG_VAR_REF(msg).msg.sta_join.pass = pass;
    ESP_MSG_VAR_REF(msg).msg.sta_join.mac = mac;
    
    return espi_send_msg_to_producer_mbox(&ESP_MSG_VAR_REF(msg), espi_initiate_cmd, blocking, 30000);   /* Send message to producer queue */
}

/**
 * \brief           Configure auto join to access point on startup
 * \note            For auto join feature, you need to do a join to access point with default mode.
 *                  Check \ref esp_sta_join for more information
 * \param[in]       en: Set to `1` to enable or `0` to disable
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_sta_autojoin(uint8_t en, uint32_t blocking) {
    ESP_MSG_VAR_DEFINE(msg);                    /* Define variable for message */

    ESP_MSG_VAR_ALLOC(msg);                     /* Allocate memory for variable */
    ESP_MSG_VAR_REF(msg).cmd_def = ESP_CMD_WIFI_CWAUTOCONN;
    ESP_MSG_VAR_REF(msg).msg.sta_autojoin.en = en;

    return espi_send_msg_to_producer_mbox(&ESP_MSG_VAR_REF(msg), espi_initiate_cmd, blocking, 30000);   /* Send message to producer queue */
}

/**
 * \brief           Get station IP address
 * \param[out]      ip: Pointer to variable to save IP address
 * \param[out]      gw: Pointer to output variable to save gateway address
 * \param[out]      nm: Pointer to output variable to save netmask address
 * \param[in]       def: Status whether default (`1`) or current (`0`) IP to read
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_sta_getip(esp_ip_t* ip, esp_ip_t* gw, esp_ip_t* nm, uint8_t def, uint32_t blocking) {
    ESP_MSG_VAR_DEFINE(msg);                    /* Define variable for message */
    
    ESP_MSG_VAR_ALLOC(msg);                     /* Allocate memory for variable */
    ESP_MSG_VAR_REF(msg).cmd_def = ESP_CMD_WIFI_CIPSTA_GET;
    ESP_MSG_VAR_REF(msg).msg.sta_ap_getip.ip = ip;
    ESP_MSG_VAR_REF(msg).msg.sta_ap_getip.gw = gw;
    ESP_MSG_VAR_REF(msg).msg.sta_ap_getip.nm = nm;
    ESP_MSG_VAR_REF(msg).msg.sta_ap_getip.def = def;
    
    return espi_send_msg_to_producer_mbox(&ESP_MSG_VAR_REF(msg), espi_initiate_cmd, blocking, 1000);    /* Send message to producer queue */
}

/**
 * \brief           Set station IP address
 * \param[in]       ip: Pointer to IP address
 * \param[in]       gw: Pointer to gateway address. Set to `NULL` to use default gateway
 * \param[in]       nm: Pointer to netmask address. Set to `NULL` to use default netmask
 * \param[in]       def: Status whether default (`1`) or current (`0`) IP to set
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_sta_setip(const esp_ip_t* ip, const esp_ip_t* gw, const esp_ip_t* nm, uint8_t def, uint32_t blocking) {
    ESP_MSG_VAR_DEFINE(msg);                    /* Define variable for message */
    
    ESP_ASSERT("ip != NULL", ip != NULL);       /* Assert input parameters */
    
    ESP_MSG_VAR_ALLOC(msg);                     /* Allocate memory for variable */
    ESP_MSG_VAR_REF(msg).cmd_def = ESP_CMD_WIFI_CIPSTA_SET;
    ESP_MSG_VAR_REF(msg).msg.sta_ap_setip.ip = ip;
    ESP_MSG_VAR_REF(msg).msg.sta_ap_setip.gw = gw;
    ESP_MSG_VAR_REF(msg).msg.sta_ap_setip.nm = nm;
    ESP_MSG_VAR_REF(msg).msg.sta_ap_setip.def = def;
    
    return espi_send_msg_to_producer_mbox(&ESP_MSG_VAR_REF(msg), espi_initiate_cmd, blocking, 1000);    /* Send message to producer queue */
}

/**
 * \brief           Get station MAC address
 * \param[out]      mac: Pointer to output variable to save MAC address
 * \param[in]       def: Status whether default (`1`) or current (`0`) IP to read
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_sta_getmac(esp_mac_t* mac, uint8_t def, uint32_t blocking) {
    ESP_MSG_VAR_DEFINE(msg);                    /* Define variable for message */
    
    ESP_MSG_VAR_ALLOC(msg);                     /* Allocate memory for variable */
    ESP_MSG_VAR_REF(msg).cmd_def = ESP_CMD_WIFI_CIPSTAMAC_GET;
    ESP_MSG_VAR_REF(msg).msg.sta_ap_getmac.mac = mac;
    ESP_MSG_VAR_REF(msg).msg.sta_ap_getmac.def = def;
    
    return espi_send_msg_to_producer_mbox(&ESP_MSG_VAR_REF(msg), espi_initiate_cmd, blocking, 1000);    /* Send message to producer queue */
}

/**
 * \brief           Set station MAC address
 * \param[in]       mac: Pointer to variable with MAC address
 * \param[in]       def: Status whether default (`1`) or current (`0`) MAC to write
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_sta_setmac(const esp_mac_t* mac, uint8_t def, uint32_t blocking) {
    ESP_MSG_VAR_DEFINE(msg);                    /* Define variable for message */
    
    ESP_ASSERT("mac != NULL", mac != NULL);     /* Assert input parameters */
    
    ESP_MSG_VAR_ALLOC(msg);                     /* Allocate memory for variable */
    ESP_MSG_VAR_REF(msg).cmd_def = ESP_CMD_WIFI_CIPSTAMAC_SET;
    ESP_MSG_VAR_REF(msg).msg.sta_ap_setmac.mac = mac;
    ESP_MSG_VAR_REF(msg).msg.sta_ap_setmac.def = def;
    
    return espi_send_msg_to_producer_mbox(&ESP_MSG_VAR_REF(msg), espi_initiate_cmd, blocking, 1000);    /* Send message to producer queue */
}

/**
 * \brief           Check if ESP got IP from access point
 * \return          `1` on success, `0` otherwise
 */
uint8_t
esp_sta_has_ip(void) {
    uint8_t res;
    ESP_CORE_PROTECT();
    res = ESP_U8(esp.status.f.r_got_ip);
    ESP_CORE_UNPROTECT();
    return res;
}

/**
 * \brief           Check if station is connected to WiFi network
 * \return          `1` on success, `0` otherwise
 */
uint8_t
esp_sta_is_joined(void) {
    return esp_sta_has_ip();
}

/**
 * \brief           Copy IP address from internal value to user variable
 * \note            Use \ref esp_sta_getip to refresh actual IP value from device
 * \param[out]      ip: Pointer to output IP variable. Set to `NULL` if not interested in IP address
 * \param[out]      gw: Pointer to output gateway variable. Set to `NULL` if not interested in gateway address
 * \param[out]      nm: Pointer to output netmask variable. Set to `NULL` if not interested in netmask address
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_sta_copy_ip(esp_ip_t* ip, esp_ip_t* gw, esp_ip_t* nm) {
    espr_t res = espERR;
    if ((ip != NULL || gw != NULL || nm != NULL) && esp_sta_has_ip()) { /* Do we have a valid IP address? */
        ESP_CORE_PROTECT();                     /* Protect ESP core */
        if (ip != NULL) {
            ESP_MEMCPY(ip, &esp.sta.ip, sizeof(esp.sta.ip));/* Copy IP address */
        }
        if (gw != NULL) {
            ESP_MEMCPY(gw, &esp.sta.gw, sizeof(esp.sta.gw));/* Copy gateway address */
        }
        if (nm != NULL) {
            ESP_MEMCPY(nm, &esp.sta.nm, sizeof(esp.sta.nm));/* Copy netmask address */
        }
        res = espOK;
        ESP_CORE_UNPROTECT();                   /* Unprotect ESP core */
    }
    return res;
}

/**
 * \brief           List for available access points ESP can connect to
 * \param[in]       ssid: Optional SSID name to search for. Set to `NULL` to disable filter
 * \param[in]       aps: Pointer to array of available access point parameters
 * \param[in]       apsl: Length of aps array
 * \param[out]      apf: Pointer to output variable to save number of access points found
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref espOK on success, member of \ref espr_t enumeration otherwise
 */
espr_t
esp_sta_list_ap(const char* ssid, esp_ap_t* aps, size_t apsl, size_t* apf, uint32_t blocking) {
    ESP_MSG_VAR_DEFINE(msg);                    /* Define variable for message */
    
    if (apf != NULL) {
        *apf = 0;
    }
    
    ESP_MSG_VAR_ALLOC(msg);                     /* Allocate memory for variable */
    ESP_MSG_VAR_REF(msg).cmd_def = ESP_CMD_WIFI_CWLAP;
    ESP_MSG_VAR_REF(msg).msg.ap_list.ssid = ssid;
    ESP_MSG_VAR_REF(msg).msg.ap_list.aps = aps;
    ESP_MSG_VAR_REF(msg).msg.ap_list.apsl = apsl;
    ESP_MSG_VAR_REF(msg).msg.ap_list.apf = apf;
    
    return espi_send_msg_to_producer_mbox(&ESP_MSG_VAR_REF(msg), espi_initiate_cmd, blocking, 30000);   /* Send message to producer queue */
}

/**
 * \brief           Check if access point is `802.11b` compatible
 * \param[in]       ap: Access point detailes acquired by \ref esp_sta_list_ap
 * \return          `1` on success, `0` otherwise
 */
uint8_t
esp_sta_is_ap_802_11b(esp_ap_t* ap) {
    return ESP_U8(!!(ap->bgn & 0x01));          /* Bit 0 is for b check */
}

/**
 * \brief           Check if access point is `802.11g` compatible
 * \param[in]       ap: Access point detailes acquired by \ref esp_sta_list_ap
 * \return          `1` on success, `0` otherwise
 */
uint8_t
esp_sta_is_ap_802_11g(esp_ap_t* ap) {
    return ESP_U8(!!(ap->bgn & 0x02));          /* Bit 1 is for g check */
}

/**
 * \brief           Check if access point is `802.11n` compatible
 * \param[in]       ap: Access point detailes acquired by \ref esp_sta_list_ap
 * \return          `1` on success, `0` otherwise
 */
uint8_t
esp_sta_is_ap_802_11n(esp_ap_t* ap) {
    return ESP_U8(!!(ap->bgn & 0x04));          /* Bit 2 is for n check */
}

#endif /* ESP_CFG_MODE_STATION || __DOXYGEN__ */
