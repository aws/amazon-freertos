/**	
 * \file            esp_evt.c
 * \brief           Event helper functions
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
#include "esp/esp_evt.h"
#include "esp/esp_mem.h"

/**
 * \brief           Check if reset was forced by user
 * \param[in]       cc: Event handle
 * \return          `1` if forced by user, `0` otherwise
 */
uint8_t
esp_evt_reset_is_forced(esp_cb_t* cc) {
    return ESP_U8(!!cc->cb.reset.forced);
}

#if ESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__

/**
 * \brief           Get MAC address from station
 * \param[in]       cc: Event handle
 * \return          MAC address
 */
esp_mac_t *
esp_evt_ap_ip_sta_get_mac(esp_cb_t* cc) {
    return cc->cb.ap_ip_sta.mac;
}

/**
 * \brief           Get IP address from station
 * \param[in]       cc: Event handle
 * \return          IP address
 */
esp_ip_t *
esp_evt_ap_ip_sta_get_ip(esp_cb_t* cc) {
    return cc->cb.ap_ip_sta.ip;
}

/**
 * \brief           Get MAC address from connected station
 * \param[in]       cc: Event handle
 * \return          MAC address
 */
esp_mac_t *
esp_evt_ap_connected_sta_get_mac(esp_cb_t* cc) {
    return cc->cb.ap_conn_disconn_sta.mac;
}

/**
 * \brief           Get MAC address from disconnected station
 * \param[in]       cc: Event handle
 * \return          MAC address
 */
esp_mac_t *
esp_evt_ap_disconnected_sta_get_mac(esp_cb_t* cc) {
    return cc->cb.ap_conn_disconn_sta.mac;
}

#endif /* ESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__ */

/**
 * \brief           Get buffer from received data
 * \param[in]       cc: Event handle
 * \return          Buffer handle
 */
esp_pbuf_p
esp_evt_conn_data_recv_get_buff(esp_cb_t* cc) {
    return cc->cb.conn_data_recv.buff;
}

/**
 * \brief           Get connection handle for receive
 * \param[in]       cc: Event handle
 * \return          Connection handle
 */
esp_conn_p
esp_evt_conn_data_recv_get_conn(esp_cb_t* cc) {
    return cc->cb.conn_data_recv.conn;
}

/**
 * \brief           Get connection handle for data sent event
 * \param[in]       cc: Event handle
 * \return          Connection handle
 */
esp_conn_p
esp_evt_conn_data_sent_get_conn(esp_cb_t* cc) {
    return cc->cb.conn_data_sent.conn;
}

/**
 * \brief           Get number of bytes sent on connection
 * \param[in]       cc: Event handle
 * \return          Number of bytes sent
 */
size_t
esp_evt_conn_data_sent_get_length(esp_cb_t* cc) {
    return cc->cb.conn_data_sent.sent;
}

/**
 * \brief           Get connection handle
 * \param[in]       cc: Event handle
 * \return          Connection handle
 */
esp_conn_p
esp_evt_conn_data_send_err_get_conn(esp_cb_t* cc) {
    return cc->cb.conn_data_send_err.conn;
}

/**
 * \brief           Get number of bytes successfully sent on failed send command
 * \param[in]       cc: Event handle
 * \return          Connection handle
 */
size_t
esp_evt_conn_data_send_err_get_length(esp_cb_t* cc) {
    return cc->cb.conn_data_send_err.sent;
}

/**
 * \brief           Get connection handle
 * \param[in]       cc: Event handle
 * \return          Connection handle
 */
esp_conn_p
esp_evt_conn_active_get_conn(esp_cb_t* cc) {
    return cc->cb.conn_active_closed.conn;
}

/**
 * \brief           Check if new connection is client
 * \param[in]       cc: Event handle
 * \return          `1` if client, `0` otherwise
 */
uint8_t
esp_evt_conn_active_is_client(esp_cb_t* cc) {
    return ESP_U8(!!cc->cb.conn_active_closed.client);
}

/**
 * \brief           Get connection handle
 * \param[in]       cc: Event handle
 * \return          Connection handle
 */
esp_conn_p
esp_evt_conn_closed_get_conn(esp_cb_t* cc) {
    return cc->cb.conn_active_closed.conn;
}

/**
 * \brief           Check if just closed connection was client
 * \param[in]       cc: Event handle
 * \return          `1` if client, `0` otherwise
 */
uint8_t
esp_evt_conn_closed_is_client(esp_cb_t* cc) {
    return cc->cb.conn_active_closed.client;
}

/**
 * \brief           Check if connection close even was forced by user
 * \param[in]       cc: Event handle
 * \return          `1` if forced, `0` otherwise
 */
uint8_t
esp_evt_conn_closed_is_forced(esp_cb_t* cc) {
    return cc->cb.conn_active_closed.forced;
}

/**
 * \brief           Get connection handle
 * \param[in]       cc: Event handle
 * \return          Connection handle
 */
esp_conn_p
esp_evt_conn_poll_get_conn(esp_cb_t* cc) {
    return cc->cb.conn_poll.conn;
}

/**
 * \brief           Get connection error type
 * \param[in]       cc: Event handle
 * \return          Member of \ref espr_t enumeration
 */
espr_t
esp_evt_conn_error_get_error(esp_cb_t* cc) {
    return cc->cb.conn_error.err;
}

/**
 * \brief           Get connection type
 * \param[in]       cc: Event handle
 * \return          Member of \ref espr_t enumeration
 */
esp_conn_type_t
esp_evt_conn_error_get_type(esp_cb_t* cc) {
    return cc->cb.conn_error.type;
}

/**
 * \brief           Get connection host
 * \param[in]       cc: Event handle
 * \return          Host name for connection
 */
const char *
esp_evt_conn_error_get_host(esp_cb_t* cc) {
    return cc->cb.conn_error.host;
}

/**
 * \brief           Get connection port
 * \param[in]       cc: Event handle
 * \return          Host port number
 */
esp_port_t
esp_evt_conn_error_get_port(esp_cb_t* cc) {
    return cc->cb.conn_error.port;
}

/**
 * \brief           Get user argument
 * \param[in]       cc: Event handle
 * \return          User argument
 */
void *
esp_evt_conn_error_get_arg(esp_cb_t* cc) {
    return cc->cb.conn_error.arg;
}

#if ESP_CFG_MODE_STATION || __DOXYGEN__

/**
 * \brief           Get command success status
 * \param[in]       cc: Event handle
 * \return          \ref espOK on success, member of \ref espr_t otherwise
 */
espr_t
esp_evt_sta_list_ap_get_status(esp_cb_t* cc) {
    return cc->cb.sta_list_ap.status;
}

/**
 * \brief           Get command success status
 * \param[in]       cc: Event handle
 * \return          Pointer to \ref esp_ap_t with first access point description
 */
esp_ap_t *
esp_evt_sta_list_ap_get_aps(esp_cb_t* cc) {
    return cc->cb.sta_list_ap.aps;
}

/**
 * \brief           Get number of access points found
 * \param[in]       cc: Event handle
 * \return          Number of access points found
 */
size_t
esp_evt_sta_list_ap_get_length(esp_cb_t* cc) {
    return cc->cb.sta_list_ap.len;
}

/**
 * \brief           Get command success status
 * \param[in]       cc: Event handle
 * \return          \ref espOK on success, member of \ref espr_t otherwise
 */
espr_t
esp_evt_sta_join_ap_get_status(esp_cb_t* cc) {
    return cc->cb.sta_join_ap.status;
}

#endif /* ESP_CFG_MODE_STATION || __DOXYGEN__ */

#if ESP_CFG_DNS || __DOXYGEN__

/**
 * \brief           Get resolve status
 * \param[in]       cc: Event handle
 * \return          \ref espOK on success, member of \ref espr_t otherwise
 */
espr_t
esp_evt_dns_hostbyname_get_status(esp_cb_t* cc) {
    return cc->cb.dns_hostbyname.status;
}

/**
 * \brief           Get hostname used to resolve IP address
 * \param[in]       cc: Event handle
 * \return          Hostname
 */
const char *
esp_evt_dns_hostbyname_get_host(esp_cb_t* cc) {
    return cc->cb.dns_hostbyname.host;
}

/**
 * \brief           Get IP address from DNS function
 * \param[in]       cc: Event handle
 * \return          IP address
 */
esp_ip_t *
esp_evt_dns_hostbyname_get_ip(esp_cb_t* cc) {
    return cc->cb.dns_hostbyname.ip;
}

#endif /* ESP_CFG_DNS || __DOXYGEN__ */

#if ESP_CFG_PING || __DOXYGEN__

/**
 * \brief           Get ping status
 * \param[in]       cc: Event handle
 * \return          \ref espOK on success, member of \ref espr_t otherwise
 */
espr_t
esp_evt_ping_get_status(esp_cb_t* cc) {
    return cc->cb.ping.status;
}

/**
 * \brief           Get hostname used to ping
 * \param[in]       cc: Event handle
 * \return          Hostname
 */
const char *
esp_evt_ping_get_host(esp_cb_t* cc) {
    return cc->cb.ping.host;
}

/**
 * \brief           Get time required for ping
 * \param[in]       cc: Event handle
 * \return          Ping time
 */
uint32_t
esp_evt_ping_get_time(esp_cb_t* cc) {
    return cc->cb.ping.time;
}

#endif /* ESP_CFG_PING || __DOXYGEN__ */

/**
 * \brief           Get server command result status
 * \param[in]       cc: Event handle
 * \return          \ref espOK on success, member of \ref espr_t otherwise
 */
espr_t
esp_evt_server_get_status(esp_cb_t* cc) {
    return cc->cb.server.status;
}

/**
 * \brief           Get port for server operation
 * \param[in]       cc: Event handle
 * \return          Server port
 */
esp_port_t
esp_evt_server_get_port(esp_cb_t* cc) {
    return cc->cb.server.port;
}

/**
* \brief           Check if operation was to enable or disable server
* \param[in]       cc: Event handle
* \return          `1` if enable, `0` otherwise
*/
uint8_t
esp_evt_server_is_enable(esp_cb_t* cc) {
    return cc->cb.server.en;
}
