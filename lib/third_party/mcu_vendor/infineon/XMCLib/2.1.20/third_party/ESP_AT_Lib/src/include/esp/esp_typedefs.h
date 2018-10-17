/**
 * \file            esp_typedefs.h
 * \brief           List of structures and enumerations for public usage
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
#ifndef __ESP_DEFS_H
#define __ESP_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"


/**
 * \ingroup         ESP
 * \defgroup        ESP_TYPEDEFS Structures and enumerations
 * \brief           List of core structures and enumerations
 * \{
 */
    
/**
 * \}
 */

/**
 * \ingroup         ESP_TYPEDEFS
 * \brief           Result enumeration used across application functions
 */
typedef enum {
    espOK = 0,                                  /*!< Function succeeded */
    espOKIGNOREMORE,                            /*!< Function succedded, should continue as espOK but ignore sending more data. This result is possible on connection data receive callback */
    espERR,
    espPARERR,                                  /*!< Wrong parameters on function call */
    espERRMEM,                                  /*!< Memory error occurred */
    espTIMEOUT,                                 /*!< Timeout occurred on command */
    espCONT,                                    /*!< There is still some command to be processed in current command */
    espCLOSED,                                  /*!< Connection just closed */
    espINPROG,                                  /*!< Operation is in progress */

    espERRNOIP,                                 /*!< Station does not have IP address */
    espERRNOFREECONN,                           /*!< There is no free connection available to start */
    espERRCONNTIMEOUT,                          /*!< Timeout received when connection to access point */
    espERRPASS,                                 /*!< Invalid password for access point */
    espERRNOAP,                                 /*!< No access point found with specific SSID and MAC address */
    espERRCONNFAIL,                             /*!< Connection failed to access point */
    espERRWIFINOTCONNECTED,                     /*!< Wifi not connected to access point */
    espERRNODEVICE,                             /*!< Device is not present */
} espr_t;

/**
 * \ingroup         ESP_TYPEDEFS
 * \brief           List of encryptions of access point
 */
typedef enum {
    ESP_ECN_OPEN = 0x00,                        /*!< No encryption on access point */
    ESP_ECN_WEP,                                /*!< WEP (Wired Equivalent Privacy) encryption */
    ESP_ECN_WPA_PSK,                            /*!< WPA (Wifi Protected Access) encryption */
    ESP_ECN_WPA2_PSK,                           /*!< WPA2 (Wifi Protected Access 2) encryption */
    ESP_ECN_WPA_WPA2_PSK,                       /*!< WPA/2 (Wifi Protected Access 1/2) encryption */
    ESP_ECN_WPA2_Enterprise                     /*!< Enterprise encryption. \note ESP is currently not able to connect to access point of this encryption type */
} esp_ecn_t;

/**
 * \ingroup         ESP_TYPEDEFS
 * \brief           IP structure
 */
typedef struct {
    uint8_t ip[4];                              /*!< IPv4 address */
} esp_ip_t;

/**
 * \ingroup         ESP_UTILITIES
 * \brief           Set IP address to \ref esp_ip_t variable
 * \param[in]       ip_str: Pointer to IP structure
 * \param[in]       ip1,ip2,ip3,ip4: IPv4 parts
 */
#define ESP_SET_IP(ip_str, ip1, ip2, ip3, ip4)      do { (ip_str)->ip[0] = (ip1); (ip_str)->ip[1] = (ip2); (ip_str)->ip[2] = (ip3); (ip_str)->ip[3] = (ip4); } while (0)

/**
 * \ingroup         ESP_TYPEDEFS
 * \brief           Port variable
 */
typedef uint16_t    esp_port_t;

/**
 * \ingroup         ESP_TYPEDEFS
 * \brief           MAC address
 */
typedef struct {
    uint8_t mac[6];                             /*!< MAC address */
} esp_mac_t;

/**
 * \ingroup         ESP_AP
 * \brief           Access point data structure
 */
typedef struct {
    esp_ecn_t ecn;                              /*!< Encryption mode */
    char ssid[21];                              /*!< Access point name */
    int16_t rssi;                               /*!< Received signal strength indicator */
    esp_mac_t mac;                              /*!< MAC physical address */
    uint8_t ch;                                 /*!< WiFi channel used on access point */
    int8_t offset;                              /*!< Access point offset */
    uint8_t cal;                                /*!< Calibration value */
    uint8_t bgn;                                /*!< Information about 802.11[b|g|n] support */
    uint8_t wps;                                /*!< Status if WPS function is supported */
} esp_ap_t;

/**
 * \ingroup         ESP_STA
 * \brief           Station data structure
 */
typedef struct {
    esp_ip_t ip;                                /*!< IP address of connected station */
    esp_mac_t mac;                              /*!< MAC address of connected station */
} esp_sta_t;

/**
 * \ingroup         ESP_TYPEDEFS
 * \brief           Date and time structure
 */
typedef struct {
    uint8_t date;                               /*!< Day in a month, from 1 to up to 31 */
    uint8_t month;                              /*!< Month in a year, from 1 to 12 */
    uint16_t year;                              /*!< Year */
    uint8_t day;                                /*!< Day in a week, from 1 to 7 */
    uint8_t hours;                              /*!< Hours in a day, from 0 to 23 */
    uint8_t minutes;                            /*!< Minutes in a hour, from 0 to 59 */
    uint8_t seconds;                            /*!< Seconds in a minute, from 0 to 59 */
} esp_datetime_t;

/**
 * \ingroup         ESP_TYPEDEFS
 * \brief           List of possible WiFi modes
 */
typedef enum {
#if ESP_CFG_MODE_STATION || __DOXYGEN__
    ESP_MODE_STA = 1,                           /*!< Set WiFi mode to station only */
#endif /* ESP_CFG_MODE_STATION || __DOXYGEN__ */
#if ESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__
    ESP_MODE_AP = 2,                            /*!< Set WiFi mode to access point only */
#endif /* ESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__ */
#if (ESP_CFG_MODE_STATION_ACCESS_POINT) || __DOXYGEN__
    ESP_MODE_STA_AP = 3,                        /*!< Set WiFi mode to station and access point */
#endif /* (ESP_CFG_MODE_STATION_ACCESS_POINT) || __DOXYGEN__ */
    ESP_MODE_NOT_SUPPORTED = 4,                        /*!< Set WiFi mode to station and access point */
} esp_mode_t;

/**
 * \ingroup         ESP_TYPEDEFS
 * \brief           List of possible HTTP methods
 */
typedef enum {
    ESP_HTTP_METHOD_GET,                        /*!< HTTP method GET */
    ESP_HTTP_METHOD_HEAD,                       /*!< HTTP method HEAD */
    ESP_HTTP_METHOD_POST,                       /*!< HTTP method POST */
    ESP_HTTP_METHOD_PUT,                        /*!< HTTP method PUT */
    ESP_HTTP_METHOD_DELETE,                     /*!< HTTP method DELETE */
    ESP_HTTP_METHOD_CONNECT,                    /*!< HTTP method CONNECT */
    ESP_HTTP_METHOD_OPTIONS,                    /*!< HTTP method OPTIONS */
    ESP_HTTP_METHOD_TRACE,                      /*!< HTTP method TRACE */
    ESP_HTTP_METHOD_PATCH,                      /*!< HTTP method PATCH */
} esp_http_method_t;

/**
 * \ingroup         ESP_CONN
 * \brief           List of possible connection types
 */
typedef enum {
    ESP_CONN_TYPE_TCP,                          /*!< Connection type is TCP */
    ESP_CONN_TYPE_UDP,                          /*!< Connection type is UDP */
    ESP_CONN_TYPE_SSL,                          /*!< Connection type is SSL */
} esp_conn_type_t;

/* Forward declarations */
struct esp_cb_t;
struct esp_conn_t;
struct esp_pbuf_t;

/**
 * \ingroup         ESP_CONN
 * \brief           Pointer to \ref esp_conn_t structure
 */
typedef struct esp_conn_t* esp_conn_p;

/**
 * \ingroup         ESP_PBUF
 * \brief           Pointer to \ref esp_pbuf_t structure
 */
typedef struct esp_pbuf_t* esp_pbuf_p;

/**
 * \ingroup         ESP_EVT
 * \brief           Event function prototype
 * \param[in]       cb: Callback event data
 * \return          \ref espOK on success, member of \ref espr_t otherwise
 */
typedef espr_t  (*esp_cb_fn)(struct esp_cb_t* cb);

/**
 * \ingroup         ESP_EVT
 * \brief           List of possible callback types received to user
 */
typedef enum esp_cb_type_t {
    ESP_CB_RESET,                               /*!< Device reset detected */
    ESP_CB_RESET_FINISH,                        /*!< Reset operation finished */
    
    ESP_CB_INIT_FINISH,                         /*!< Initialization has been finished at this point */
    ESP_CB_RESTORE_FINISH,                      /*!< Device restore operation finished */
    
    ESP_CB_DEVICE_PRESENT,                      /*!< Notification when device present status changes */
    
    ESP_CB_CONN_DATA_RECV,                      /*!< Connection data received */
    ESP_CB_CONN_DATA_SENT,                      /*!< Data were successfully sent */
    ESP_CB_CONN_DATA_SEND_ERR,                  /*!< Error trying to send data */
    ESP_CB_CONN_ACTIVE,                         /*!< Connection just became active */
    ESP_CB_CONN_ERROR,                          /*!< Client connection start was not successful */
    ESP_CB_CONN_CLOSED,                         /*!< Connection was just closed */
    ESP_CB_CONN_POLL,                           /*!< Poll for connection if there are any changes */
    
    ESP_CB_SERVER,                              /*!< Server status changed */

#if ESP_CFG_MODE_STATION || __DOXYGEN__
    ESP_CB_WIFI_CONNECTED,                      /*!< Station just connected to AP */
    ESP_CB_WIFI_GOT_IP,                         /*!< Station has valid IP */
    ESP_CB_WIFI_DISCONNECTED,                   /*!< Station just disconnected from AP */
    ESP_CB_WIFI_IP_ACQUIRED,                    /*!< Station IP address acquired */
  
    ESP_CB_STA_LIST_AP,                         /*!< Station listed APs event */
    ESP_CB_STA_JOIN_AP,                         /*!< Join to access point */
#endif /* ESP_CFG_MODE_STATION || __DOXYGEN__ */
#if ESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__
    ESP_CB_AP_CONNECTED_STA,                    /*!< New station just connected to ESP's access point */
    ESP_CB_AP_DISCONNECTED_STA,                 /*!< New station just disconnected from ESP's access point */
    ESP_CB_AP_IP_STA,                           /*!< New station just received IP from ESP's access point */
#endif /* ESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__ */
#if ESP_CFG_DNS || __DOXYGEN__
    ESP_CB_DNS_HOSTBYNAME,                      /*!< DNS domain service finished */
#endif /* ESP_CFG_DNS || __DOXYGEN__ */
#if ESP_CFG_PING || __DOXYGEN__
    ESP_CB_PING,                                /*!< PING service finished */
#endif /* ESP_CFG_PING || __DOXYGEN__ */
} esp_cb_type_t;

/**
 * \ingroup         ESP_EVT
 * \brief           Global callback structure to pass as parameter to callback function
 */
typedef struct esp_cb_t {
    esp_cb_type_t type;                         /*!< Callback type */
    union {
        struct {
            uint8_t forced;                     /*!< Set to 1 if reset forced by user */
        } reset;                                /*!< Reset occurred. Use with \ref ESP_CB_RESET event */

        struct {
            esp_conn_p conn;                    /*!< Connection where data were received */
            esp_pbuf_p buff;                    /*!< Pointer to received data */
        } conn_data_recv;                       /*!< Network data received. Use with \ref ESP_CB_CONN_DATA_RECV event */
        struct {
            esp_conn_p conn;                    /*!< Connection where data were sent */
            size_t sent;                        /*!< Number of bytes sent on connection */
        } conn_data_sent;                       /*!< Data successfully sent. Use with \ref ESP_CB_CONN_DATA_SENT event */
        struct {
            esp_conn_p conn;                    /*!< Connection where data were sent */
            size_t sent;                        /*!< Number of bytes sent on connection before error occurred */
        } conn_data_send_err;                   /*!< Data were not sent. Use with \ref ESP_CB_CONN_DATA_SEND_ERR event */
        struct {
            const char* host;                   /*!< Host to use for connection */
            esp_port_t port;                    /*!< Remote port used for connection */
            esp_conn_type_t type;               /*!< Connection type */
            void* arg;                          /*!< Connection argument used on connection */
            /*
             * \todo: Implement error reason:
             *  - No free connection to start
             *  - Remote host is not responding
             *  - ...
             */
            espr_t err;                         /*!< Error value */
        } conn_error;                           /*!< Client connection start error. Use with \ref ESP_CB_CONN_ERROR event */
        struct {
            esp_conn_p conn;                    /*!< Pointer to connection */
            uint8_t client;                     /*!< Set to 1 if connection is/was client mode */
            uint8_t forced;                     /*!< Set to 1 if connection action was forced (when active: 1 = CLIENT, 0 = SERVER: when closed, 1 = CMD, 0 = REMOTE) */
        } conn_active_closed;                   /*!< Process active and closed statuses at the same time. Use with \ref ESP_CB_CONN_ACTIVE or \ref ESP_CB_CONN_CLOSED events */
        struct {
            esp_conn_p conn;                    /*!< Set connection pointer */
        } conn_poll;                            /*!< Polling active connection to check for timeouts. Use with \ref ESP_CB_CONN_POLL event */

        struct {
            espr_t status;                      /*!< Status of command */
            uint8_t en;                         /*!< Status to enable/disable server */
            esp_port_t port;                    /*!< Server port number */
        } server;                               /*!< Server change event. Use with \ref ESP_CB_SERVER event */
#if ESP_CFG_MODE_STATION || __DOXYGEN__
        struct {
            espr_t status;                      /*!< Status of command */
            esp_ap_t* aps;                      /*!< Pointer to access points */
            size_t len;                         /*!< Number of access points found */
        } sta_list_ap;                          /*!< Station list access points. Use with \ref ESP_CB_STA_LIST_AP event */
        struct {
            espr_t status;                      /*!< Connection status */
        } sta_join_ap;                          /*!< Join to access point. Use with \ref ESP_CB_STA_JOIN_AP event */
#endif /* ESP_CFG_MODE_STATION || __DOXYGEN__ */
#if ESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__
        struct {
            esp_mac_t* mac;                     /*!< Station MAC address */
        } ap_conn_disconn_sta;                  /*!< A new station connected or disconnected to ESP's access point. Use with \ref ESP_CB_AP_CONNECTED_STA or \ref ESP_CB_AP_DISCONNECTED_STA events */
        struct {
            esp_mac_t* mac;                     /*!< Station MAC address */
            esp_ip_t* ip;                       /*!< Station IP address */
        } ap_ip_sta;                            /*!< Station got IP address from ESP's access point. Use with \ref ESP_CB_AP_IP_STA event */
#endif /* ESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__ */
#if ESP_CFG_DNS || __DOXYGEN__
        struct {
            espr_t status;                      /*!< Operation status */
            const char* host;                   /*!< Host name for DNS lookup */
            esp_ip_t* ip;                       /*!< Pointer to IP result */
        } dns_hostbyname;                       /*!< DNS domain service finished. Use with \ref ESP_CB_DNS_HOSTBYNAME event */
#endif /* ESP_CFG_DNS || __DOXYGEN__ */        
#if ESP_CFG_PING || __DOXYGEN__
        struct {
            espr_t status;                      /*!< Operation status */
            const char* host;                   /*!< Host name for ping */
            uint32_t time;                      /*!< Time required for ping. Valid only if operation succedded */
        } ping;                                 /*!< Ping finished. Use with \ref ESP_CB_PING event */
#endif /* ESP_CFG_PING || __DOXYGEN__ */
    } cb;                                       /*!< Callback event union */
} esp_cb_t;

#define ESP_SIZET_MAX                           ((size_t)(-1))  /*!< Maximal value of size_t variable type */
 
/**
 * \ingroup         ESP_LL
 * \brief           Function prototype for AT output data
 * \param[in]       data: Pointer to data to send
 * \param[in]       len: Number of bytes to send
 * \return          Number of bytes sent
 */
typedef uint16_t (*esp_ll_send_fn)(const void* data, uint16_t len);

/**
 * \ingroup         ESP_LL
 * \brief           Low level user specific functions
 */
typedef struct {
    esp_ll_send_fn send_fn;                     /*!< Callback function to transmit data */
    struct {
        uint32_t baudrate;                      /*!< UART baudrate value */
    } uart;                                     /*!< UART communication parameters */
} esp_ll_t;

/**
 * \ingroup         ESP_TIMEOUT
 * \brief           Timeout callback function prototype
 */
typedef void (*esp_timeout_fn_t)(void *);

/**
 * \ingroup         ESP_TIMEOUT
 * \brief           Timeout structure
 */
typedef struct esp_timeout_t {
    struct esp_timeout_t* next;                 /*!< Pointer to next timeout entry */
    uint32_t time;                              /*!< Time difference from previous entry */
    void* arg;                                  /*!< Argument to pass to callback function */
    esp_timeout_fn_t fn;                        /*!< Callback function for timeout */
} esp_timeout_t;

/**
 * \ingroup         ESP_BUFF
 * \brief           Buffer structure
 */
typedef struct esp_buff {
	size_t size;                                /*!< Size of buffer in units of bytes */
	size_t in;                                  /*!< Input pointer to save next value */
	size_t out;                                 /*!< Output pointer to read next value */
	uint8_t* buff;                              /*!< Pointer to buffer data array */
	uint8_t flags;                              /*!< Flags for buffer */
} esp_buff_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ESP_DEFS_H */
