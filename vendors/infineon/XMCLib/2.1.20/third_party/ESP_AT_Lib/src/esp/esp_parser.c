/**
 * \file            esp_parser.c
 * \brief           Parse incoming data from AT port
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
#include "esp/esp_parser.h"
#include "esp/esp_mem.h"

/**
 * \brief           Parse number from string
 * \note            Input string pointer is changed and number is skipped
 * \param[in]       Pointer to pointer to string to parse
 * \return          Parsed number
 */
int32_t
espi_parse_number(const char** str) {
    int32_t val = 0;
    uint8_t minus = 0;
    const char* p = *str;                       /*  */

    if (*p == '"') {                            /* Skip leading quotes */
        p++;
    }
    if (*p == ',') {                            /* Skip leading comma */
        p++;
    }
    if (*p == '"') {                            /* Skip leading quotes */
        p++;
    }
    if (*p == '-') {                            /* Check negative number */
        minus = 1;
        p++;
    }
    while (ESP_CHARISNUM(*p)) {                 /* Parse until character is valid number */
        val = val * 10 + ESP_CHARTONUM(*p);
        p++;
    }
    if (*p == ',') {                            /* Go to next entry if possible */
        p++;
    }
    *str = p;                                   /* Save new pointer with new offset */

    return minus ? -val : val;
}

/**
 * \brief           Parse port from string
 * \note            Input string pointer is changed and number is skipped
 * \param[in]       Pointer to pointer to string to parse
 * \return          Parsed port number
 */
esp_port_t
espi_parse_port(const char** str) {
    esp_port_t p;

    p = (esp_port_t)espi_parse_number(str);     /* Parse port */
    return p;
}

/**
 * \brief           Parse number from string as hex
 * \note            Input string pointer is changed and number is skipped
 * \param[in]       Pointer to pointer to string to parse
 * \return          Parsed number
 */
uint32_t
espi_parse_hexnumber(const char** str) {
    int32_t val = 0;
    const char* p = *str;                       /*  */
    
    if (*p == '"') {                            /* Skip leading quotes */
        p++;
    }
    if (*p == ',') {                            /* Skip leading comma */
        p++;
    }
    if (*p == '"') {                            /* Skip leading quotes */
        p++;
    }
    while (ESP_CHARISHEXNUM(*p)) {              /* Parse until character is valid number */
        val = val * 16 + ESP_CHARHEXTONUM(*p);
        p++;
    }
    if (*p == ',') {                            /* Go to next entry if possible */
        p++;
    }
    *str = p;                                   /* Save new pointer with new offset */
    return val;
}

/**
 * \brief           Parse input string as string part of AT command
 * \param[in,out]   src: Pointer to pointer to string to parse from
 * \param[in]       dst: Destination pointer. Use NULL in case you want only skip string in source
 * \param[in]       dst_len: Length of distance buffer, including memory for `NULL` termination
 * \param[in]       trim: Set to 1 to process entire string, even if no memory anymore
 * \return          `1` on success, `0` otherwise
 */
uint8_t
espi_parse_string(const char** src, char* dst, size_t dst_len, uint8_t trim) {
    const char* p = *src;
    size_t i;
    
    if (*p == ',') {
        p++;
    }
    if (*p == '"') {
        p++;
    }
    i = 0;
    if (dst_len) {
        dst_len--;
    }
    while (*p) {
        if (*p == '"' && (p[1] == ',' || p[1] == '\r' || p[1] == '\n')) {
            p++;
            break;
        }
        if (dst != NULL) {
            if (i < dst_len) {
                *dst++ = *p;
                i++;
            } else if (!trim) {
                break;
            }
        }
        p++;
    }
    if (dst != NULL) {
        *dst = 0;
    }
    *src = p;
    return 1;
}

/**
 * \brief           Parse string as IP address
 * \param[in,out]   src: Pointer to pointer to string to parse from
 * \param[in]       dst: Destination pointer
 * \return          `1` on success, `0` otherwise
 */
uint8_t
espi_parse_ip(const char** src, esp_ip_t* ip) {
    const char* p = *src;
    
    if (*p == '"') {
        p++;
    }
    ip->ip[0] = espi_parse_number(&p); p++;
    ip->ip[1] = espi_parse_number(&p); p++;
    ip->ip[2] = espi_parse_number(&p); p++;
    ip->ip[3] = espi_parse_number(&p);
    if (*p == '"') {
        p++;
    }
    
    *src = p;                                   /* Set new pointer */
    return 1;
}

/**
 * \brief           Parse string as MAC address
 * \param[in,out]   src: Pointer to pointer to string to parse from
 * \param[in]       dst: Destination pointer
 * \return          `1` on success, `0` otherwise
 */
uint8_t
espi_parse_mac(const char** src, esp_mac_t* mac) {
    const char* p = *src;
    
    if (*p == '"') {                            /* Go to next entry if possible */
        p++;
    }
    mac->mac[0] = espi_parse_hexnumber(&p); p++;
    mac->mac[1] = espi_parse_hexnumber(&p); p++;
    mac->mac[2] = espi_parse_hexnumber(&p); p++;
    mac->mac[3] = espi_parse_hexnumber(&p); p++;
    mac->mac[4] = espi_parse_hexnumber(&p); p++;
    mac->mac[5] = espi_parse_hexnumber(&p);
    if (*p == '"') {                            /* Skip quotes if possible */
        p++;
    }
    if (*p == ',') {                            /* Go to next entry if possible */
        p++;
    }
    *src = p;                                   /* Set new pointer */
    return 1;
}

/**
 * \brief           Parse +CIPSTATUS response from ESP device
 * \param[in]       str: Input string to parse
 * \return          Member of \ref espr_t enumeration
 */
espr_t
espi_parse_cipstatus(const char* str) {
    uint8_t cn_num = 0;
    
    cn_num = espi_parse_number(&str);           /* Parse connection number */
    esp.active_conns |= 1 << cn_num;            /* Set flag as active */
    
    espi_parse_string(&str, NULL, 0, 1);        /* Parse string and ignore result */
    
    espi_parse_ip(&str, &esp.conns[cn_num].remote_ip);
    esp.conns[cn_num].remote_port = espi_parse_number(&str);
    esp.conns[cn_num].local_port = espi_parse_number(&str);
    esp.conns[cn_num].status.f.client = !espi_parse_number(&str);
    
    return espOK;
}

/**
 * \brief           Parse +IPD statement
 * \param[in]       str: Input string to parse
 * \return          Member of \ref espr_t enumeration
 */
espr_t
espi_parse_ipd(const char* str) {
    uint8_t conn;
    size_t len;
    
    conn = espi_parse_number(&str);             /* Parse number for connection number */
    len = espi_parse_number(&str);              /* Parse number for number of bytes to read */
    espi_parse_ip(&str, &esp.ipd.ip);           /* Parse incoming packet IP */
    esp.ipd.port = espi_parse_port(&str);       /* Get port on IPD data */
    
    ESP_MEMCPY(&esp.conns[conn].remote_ip, &esp.ipd.ip, sizeof(esp.ipd.ip));
    ESP_MEMCPY(&esp.conns[conn].remote_port, &esp.ipd.port, sizeof(esp.ipd.port));
    
    esp.ipd.read = 1;                           /* Start reading network data */
    esp.ipd.tot_len = len;                      /* Total number of bytes in this received packet */
    esp.ipd.rem_len = len;                      /* Number of remaining bytes to read */
    esp.ipd.conn = &esp.conns[conn];            /* Pointer to connection we have data for */
    
    return espOK;
}

/**
 * \brief           Parse AT and SDK versions from AT+GMR response
 * \param[in]       str: String starting with version numbers
 * \param[out]      version_out: Output variable to save version
 * \return          `1` on success, `0` otherwise
 */
uint8_t
espi_parse_at_sdk_version(const char* str, uint32_t* version_out) {
    *version_out = 0;
    
    *version_out |= ((uint8_t)espi_parse_number(&str)) << 24;   str++;
    *version_out |= ((uint8_t)espi_parse_number(&str)) << 16;   str++;
    *version_out |= ((uint8_t)espi_parse_number(&str)) << 8;    str++;
    *version_out |= ((uint8_t)espi_parse_number(&str));
    
    return 1;
}

/**
 * \brief           Parse +LINK_CONN received string for new connection active
 */
uint8_t
espi_parse_link_conn(const char* str) {
    if (str == NULL) {
        return 0;
    }
    if (*str == '+') {
        str += 11;
    }
    esp.link_conn.failed = espi_parse_number(&str);
    esp.link_conn.num = espi_parse_number(&str);
    if (!strncmp(str, "\"TCP\"", 5)) {
        esp.link_conn.type = ESP_CONN_TYPE_TCP;
    } else if (!strncmp(str, "\"UDP\"", 5)) {
        esp.link_conn.type = ESP_CONN_TYPE_UDP;
    } else if (!strncmp(str, "\"SSL\"", 5)) {
        esp.link_conn.type = ESP_CONN_TYPE_SSL;
    } else {
        return 0;
    }
    str += 6;
    esp.link_conn.is_server = espi_parse_number(&str);
    espi_parse_ip(&str, &esp.link_conn.remote_ip);
    esp.link_conn.remote_port = espi_parse_number(&str);
    esp.link_conn.local_port = espi_parse_number(&str);
    return 1;
}

#if ESP_CFG_MODE_STATION || __DOXYGEN__
/**
 * \brief           Parse received message for list access points
 * \param[in]       str: Pointer to input string starting with +CWLAP
 * \param[in]       msg: Pointer to message
 * \return          `1` on success, `0` otherwise
 */
uint8_t
espi_parse_cwlap(const char* str, esp_msg_t* msg) {
    if (!CMD_IS_DEF(ESP_CMD_WIFI_CWLAP) ||      /* Do we have valid message here and enough memory to save everything? */
        msg->msg.ap_list.aps == NULL || msg->msg.ap_list.apsi >= msg->msg.ap_list.apsl) {   
        return 0;
    }
    if (*str == '+') {                          /* Does string contain '+' as first character */
        str += 7;                               /* Skip this part */
    }
    if (*str++ != '(') {                        /* We must start with opening bracket */
        return 0;
    }
    
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].ecn = (esp_ecn_t)espi_parse_number(&str);
    espi_parse_string(&str, msg->msg.ap_list.aps[msg->msg.ap_list.apsi].ssid, sizeof(msg->msg.ap_list.aps[msg->msg.ap_list.apsi].ssid), 1);
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].rssi = espi_parse_number(&str);
    espi_parse_mac(&str, &msg->msg.ap_list.aps[msg->msg.ap_list.apsi].mac);
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].ch = espi_parse_number(&str);
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].offset = espi_parse_number(&str);
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].cal = espi_parse_number(&str);

    espi_parse_number(&str);                    /* Parse pwc */
    espi_parse_number(&str);                    /* Parse gc */
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].bgn = espi_parse_number(&str);
    msg->msg.ap_list.aps[msg->msg.ap_list.apsi].wps = espi_parse_number(&str);

    msg->msg.ap_list.apsi++;                    /* Increase number of found elements */
    if (msg->msg.ap_list.apf) {                 /* Set pointer if necessary */
        *msg->msg.ap_list.apf = msg->msg.ap_list.apsi;
    }
    return 1;
}
#endif /* ESP_CFG_MODE_STATION || __DOXYGEN__ */

#if ESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__
/**
 * \brief           Parse received message for list stations
 * \param[in]       str: Pointer to input string starting with +CWLAP
 * \param[in]       msg: Pointer to message
 * \return          `1` on success, `0` otherwise
 */
uint8_t
espi_parse_cwlif(const char* str, esp_msg_t* msg) {
    if (!CMD_IS_DEF(ESP_CMD_WIFI_CWLIF) ||      /* Do we have valid message here and enough memory to save everything? */
        msg->msg.sta_list.stas == NULL || msg->msg.sta_list.stai >= msg->msg.sta_list.stal) {   
        return 0;
    }
    
    espi_parse_ip(&str, &msg->msg.sta_list.stas[msg->msg.sta_list.stai].ip);
    espi_parse_mac(&str, &msg->msg.sta_list.stas[msg->msg.sta_list.stai].mac);

    msg->msg.sta_list.stai++;                   /* Increase number of found elements */
    if (msg->msg.sta_list.staf) {               /* Set pointer if necessary */
        *msg->msg.sta_list.staf = msg->msg.sta_list.stai;
    }
    return 1;
}

/**
 * \brief           Parse MAC address and send to user layer
 * \param[in]       str: Input string excluding `+DIST_STA_IP:` part
 * \param[in]       is_conn: Set to `1` if station connected or `0` if station disconnected
 * \return          `1` on success, `0` otherwise
 */
uint8_t
espi_parse_ap_conn_disconn_sta(const char* str, uint8_t is_conn) {
    esp_mac_t mac;
    espi_parse_mac(&str, &mac);                 /* Parse MAC address */

    esp.cb.cb.ap_conn_disconn_sta.mac = &mac;
    espi_send_cb(is_conn ? ESP_CB_AP_CONNECTED_STA : ESP_CB_AP_DISCONNECTED_STA);   /* Send event function */
    return 1;
}

/**
 * \brief           Parse received string "+DIST_STA_IP" and send notification to user layer
 * \param[in]       str: Input string excluding "+DIST_STA_IP:" part
 * \return          `1` on success, `0` otherwise
 */
uint8_t
espi_parse_ap_ip_sta(const char* str) {
    esp_mac_t mac;
    esp_ip_t ip;

    espi_parse_mac(&str, &mac);                 /* Parse MAC address */
    espi_parse_ip(&str, &ip);                   /* Parse IP address */

    esp.cb.cb.ap_ip_sta.mac = &mac;
    esp.cb.cb.ap_ip_sta.ip = &ip;
    espi_send_cb(ESP_CB_AP_IP_STA);             /* Send event function */
    return 1;
}
#endif /* ESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__ */

#if ESP_CFG_DNS || __DOXYGEN__

/**
 * \brief           Parse received message domain DNS name
 * \param[in]       str: Pointer to input string starting with +CWLAP
 * \param[in]       msg: Pointer to message
 * \return          `1` on success, `0` otherwise
 */
uint8_t
espi_parse_cipdomain(const char* str, esp_msg_t* msg) {
    if (!CMD_IS_DEF(ESP_CMD_TCPIP_CIPDOMAIN)) {   
        return 0;
    }
    if (*str == '+') {
        str += 11;
    }
    espi_parse_ip(&str, msg->msg.dns_getbyhostname.ip); /* Parse IP address */
    return 1;
}

#endif /* ESP_CFG_DNS || __DOXYGEN__ */

#if ESP_CFG_PING || __DOXYGEN__

/**
 * \brief           Parse received time for ping
 * \param[in]       str: Pointer to input string starting with +time
 * \param[in]       msg: Pointer to message
 * \return          `1` on success, `0` otherwise
 */
uint8_t
espi_parse_ping_time(const char* str, esp_msg_t* msg) {
    uint32_t time;

    if (!CMD_IS_DEF(ESP_CMD_TCPIP_PING)) {
        return 0;
    }
    if (*str == '+') {
        str++;
    }
    time = espi_parse_number(&str);
    if (msg->msg.tcpip_ping.time != NULL) {
        *msg->msg.tcpip_ping.time = time;
    }
    return 1;
}

#endif /* ESP_CFG_PING || __DOXYGEN__ */

#if ESP_CFG_SNTP || __DOXYGEN__

/**
 * \brief           Parse received message for SNTP time
 * \param[in]       str: Pointer to input string starting with +CWLAP
 * \param[in]       msg: Pointer to message
 * \return          `1` on success, `0` otherwise
 */
uint8_t
espi_parse_cipsntptime(const char* str, esp_msg_t* msg) {
    if (!CMD_IS_DEF(ESP_CMD_TCPIP_CIPSNTPTIME)) {
        return 0;
    }
    if (*str == '+') {                              /* Check input string */
        str += 13;
    }
    
    /*
     * Scan for day in a week
     */
    if (!strncmp(str, "Mon", 3)) {
        msg->msg.tcpip_sntp_time.dt->day = 1;
    } else if (!strncmp(str, "Tue", 3)) {
        msg->msg.tcpip_sntp_time.dt->day = 2;
    } else if (!strncmp(str, "Wed", 3)) {
        msg->msg.tcpip_sntp_time.dt->day = 3;
    } else if (!strncmp(str, "Thu", 3)) {
        msg->msg.tcpip_sntp_time.dt->day = 4;
    } else if (!strncmp(str, "Fri", 3)) {
        msg->msg.tcpip_sntp_time.dt->day = 5;
    } else if (!strncmp(str, "Sat", 3)) {
        msg->msg.tcpip_sntp_time.dt->day = 6;
    } else if (!strncmp(str, "Sun", 3)) {
        msg->msg.tcpip_sntp_time.dt->day = 7;
    }
    str += 4;
    
    /*
     * Scan for month in a year
     */
    if (!strncmp(str, "Jan", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 1;
    } else if (!strncmp(str, "Feb", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 2;
    } else if (!strncmp(str, "Mar", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 3;
    } else if (!strncmp(str, "Apr", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 4;
    } else if (!strncmp(str, "May", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 5;
    } else if (!strncmp(str, "Jun", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 6;
    } else if (!strncmp(str, "Jul", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 7;
    } else if (!strncmp(str, "Aug", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 8;
    } else if (!strncmp(str, "Sep", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 9;
    } else if (!strncmp(str, "Oct", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 10;
    } else if (!strncmp(str, "Nov", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 11;
    } else if (!strncmp(str, "Dec", 3)) {
        msg->msg.tcpip_sntp_time.dt->month = 12;
    }
    str += 4;
    
    msg->msg.tcpip_sntp_time.dt->date = espi_parse_number(&str);
    str++;
    msg->msg.tcpip_sntp_time.dt->hours = espi_parse_number(&str);
    str++;
    msg->msg.tcpip_sntp_time.dt->minutes = espi_parse_number(&str);
    str++;
    msg->msg.tcpip_sntp_time.dt->seconds = espi_parse_number(&str);
    str++;
    msg->msg.tcpip_sntp_time.dt->year = espi_parse_number(&str);
    return 1;
}

#endif /* ESP_CFG_SNTP || __DOXYGEN__ */

#if ESP_CFG_HOSTNAME || __DOXYGEN__

/**
 * \brief           Parse received message for HOSTNAME
 * \param[in]       str: Pointer to input string starting with +CWHOSTNAME
 * \param[in]       msg: Pointer to message
 * \return          `1` on success, `0` otherwise
 */
uint8_t
espi_parse_hostname(const char* str, esp_msg_t* msg) {
    size_t i;
    if (!CMD_IS_DEF(ESP_CMD_WIFI_CWHOSTNAME_GET)) {
        return 0;
    }
    if (*str == '+') {                              /* Check input string */
        str += 12;
    }
    msg->msg.wifi_hostname.hostname[0] = 0;
    if (*str != '\r') {
        i = 0;
        while (i < (msg->msg.wifi_hostname.length - 1) && *str && *str != '\r') {
            msg->msg.wifi_hostname.hostname[i] = *str;
            i++;
            str++;
        }
        msg->msg.wifi_hostname.hostname[i] = 0;
    }
    return 1;
}

#endif /* ESP_CFG_HOSTNAME || __DOXYGEN__ */
