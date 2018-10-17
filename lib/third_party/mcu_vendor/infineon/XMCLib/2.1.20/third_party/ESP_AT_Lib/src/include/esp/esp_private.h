/**
 * \file            esp_private.h
 * \brief           Private structures and enumerations
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
#ifndef __ESP_PRIV_H
#define __ESP_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "esp/esp.h"
#include "esp/esp_typedefs.h"
#include "esp/esp_debug.h"

/**
 * \addtogroup      ESP_TYPEDEFS
 * \{
 */
 
/**
 * \brief           List of possible messages
 */
typedef enum {
    ESP_CMD_IDLE = 0,                           /*!< IDLE mode */
    
    /*
     * Basic AT commands
     */
    ESP_CMD_RESET,                              /*!< Reset device */
    ESP_CMD_ATE0,                               /*!< Disable ECHO mode on AT commands */
    ESP_CMD_ATE1,                               /*!< Enable ECHO mode on AT commands */
    ESP_CMD_GMR,                                /*!< Get AT commands version */
    ESP_CMD_GSLP,                               /*!< Set ESP to sleep mode */
    ESP_CMD_RESTORE,                            /*!< Restore ESP internal settings to default values */
    ESP_CMD_UART,
    ESP_CMD_SLEEP,
    ESP_CMD_WAKEUPGPIO,
    ESP_CMD_RFPOWER,
    ESP_CMD_RFVDD,
    ESP_CMD_RFAUTOTRACE,
    ESP_CMD_SYSRAM,
    ESP_CMD_SYSADC,
    ESP_CMD_SYSIOSETCFG,
    ESP_CMD_SYSIOGETCFG,
    ESP_CMD_SYSGPIODIR,
    ESP_CMD_SYSGPIOWRITE,
    ESP_CMD_SYSGPIOREAD,
    ESP_CMD_SYSMSG,                             /*!< Configure system messages */
    ESP_CMD_SYSMSG_CUR,
    
    /*
     * WiFi based commands
     */
    ESP_CMD_WIFI_CWMODE,                        /*!< Set/Get wifi mode */
    ESP_CMD_WIFI_CWLAPOPT,                      /*!< Configure what is visible on CWLAP response */
#if ESP_CFG_MODE_STATION || __DOXYGEN__
    ESP_CMD_WIFI_CWJAP,                         /*!< Connect to access point */
    ESP_CMD_WIFI_CWQAP,                         /*!< Disconnect from access point */
    ESP_CMD_WIFI_CWLAP,                         /*!< List available access points */
    ESP_CMD_WIFI_CIPSTAMAC_GET,                 /*!< Get MAC address of ESP station */
    ESP_CMD_WIFI_CIPSTAMAC_SET,                 /*!< Set MAC address of ESP station */
    ESP_CMD_WIFI_CIPSTA_GET,                    /*!< Get IP address of ESP station */
    ESP_CMD_WIFI_CIPSTA_SET,                    /*!< Set IP address of ESP station */
    ESP_CMD_WIFI_CWAUTOCONN,                    /*!< Configure auto connection to access point */
#endif /* ESP_CFG_MODE_STATION || __DOXYGEN__ */
#if ESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__
    ESP_CMD_WIFI_CWSAP_GET,                     /*!< Get software access point configuration */
    ESP_CMD_WIFI_CWSAP_SET,                     /*!< Set software access point configuration */
    ESP_CMD_WIFI_CIPAPMAC_GET,                  /*!< Get MAC address of ESP access point */
    ESP_CMD_WIFI_CIPAPMAC_SET,                  /*!< Set MAC address of ESP access point */
    ESP_CMD_WIFI_CIPAP_GET,                     /*!< Get IP address of ESP access point */
    ESP_CMD_WIFI_CIPAP_SET,                     /*!< Set IP address of ESP access point */
    ESP_CMD_WIFI_CWLIF,                         /*!< Get connected stations on access point */
#endif /* ESP_CFG_MODE_STATION || __DOXYGEN__ */
#if ESP_CFG_WPS || __DOXYGEN__
    ESP_CMD_WIFI_WPS,                           /*!< Set WPS option */
#endif /* ESP_CFG_WPS || __DOXYGEN__ */
#if ESP_CFG_MDNS || __DOXYGEN__
    ESP_CMD_WIFI_MDNS,                          /*!< Configure MDNS function */
#endif /* ESP_CFG_MDNS || __DOXYGEN__ */
#if ESP_CFG_HOSTNAME || __DOXYGEN__
    ESP_CMD_WIFI_CWHOSTNAME_SET,                /*!< Set device hostname */
    ESP_CMD_WIFI_CWHOSTNAME_GET,                /*!< Get device hostname */
#endif /* ESP_CFG_HOSTNAME || __DOXYGEN__ */
    
    /*
     * TCP/IP related commands
     */
    ESP_CMD_TCPIP_CIPSTATUS,                    /*!< Get status of connections */
#if ESP_CFG_DNS || __DOXYGEN__
    ESP_CMD_TCPIP_CIPDOMAIN,                    /*!< Get IP address from domain name = DNS function */
#endif /* ESP_CFG_DNS || __DOXYGEN__ */
    ESP_CMD_TCPIP_CIPSTART,                     /*!< Start client connection */
    ESP_CMD_TCPIP_CIPSSLSIZE,                   /*!< Set SSL buffer size for SSL connection */
    ESP_CMD_TCPIP_CIPSEND,                      /*!< Send network data */
    ESP_CMD_TCPIP_CIPCLOSE,                     /*!< Close active connection */
    ESP_CMD_TCPIP_CIFSR,                        /*!< Get local IP */
    ESP_CMD_TCPIP_CIPMUX,                       /*!< Set single or multiple connections */
    ESP_CMD_TCPIP_CIPSERVER,                    /*!< Enables/Disables server mode */
    ESP_CMD_TCPIP_CIPSERVERMAXCONN,             /*!< Sets maximal number of connections allowed for server population */
    ESP_CMD_TCPIP_CIPMODE,                      /*!< Transmission mode, either transparent or normal one */
    ESP_CMD_TCPIP_CIPSTO,                       /*!< Sets connection timeout */
#if ESP_CFG_PING || __DOXYGEN__
    ESP_CMD_TCPIP_PING,                         /*!< Ping domain */
#endif /* ESP_CFG_PING || __DOXYGEN__ */
    ESP_CMD_TCPIP_CIUPDATE,                     /*!< Perform self-update */
#if ESP_CFG_SNTP || __DOXYGEN__
    ESP_CMD_TCPIP_CIPSNTPCFG,                   /*!< Configure SNTP servers */
    ESP_CMD_TCPIP_CIPSNTPTIME,                  /*!< Get current time using SNTP */
#endif /* ESP_SNT || __DOXYGEN__ */
    ESP_CMD_TCPIP_CIPDNS,                       /*!< Configure user specific DNS servers */
    ESP_CMD_TCPIP_CIPDINFO,                     /*!< Configure what data are received on +IPD statement */
} esp_cmd_t;

/**
 * \brief           Connection structure
 */
typedef struct esp_conn_t {
    esp_conn_type_t type;                       /*!< Connection type */
    uint8_t         num;                        /*!< Connection number */
    esp_ip_t        remote_ip;                  /*!< Remote IP address */
    esp_port_t      remote_port;                /*!< Remote port number */
    esp_port_t      local_port;                 /*!< Local IP address */
    esp_cb_fn       cb_func;                    /*!< Callback function for connection */
    void*           arg;                        /*!< User custom argument */
    
    uint8_t         val_id;                     /*!< Validation ID number. It is increased each time a new connection is established.
                                                     It protects sending data to wrong connection in case we have data in send queue,
                                                     and connection was closed and active again in between. */
    
    uint8_t*        buff;                       /*!< Pointer to buffer when using \ref esp_conn_write function */
    size_t          buff_len;                   /*!< Total length of buffer */
    size_t          buff_ptr;                   /*!< Current write pointer of buffer */
    
    union {
        struct {
            uint8_t active:1;                   /*!< Status whether connection is active */
            uint8_t client:1;                   /*!< Status whether connection is in client mode */
            uint8_t data_received:1;            /*!< Status whether first data were received on connection */
            uint8_t in_closing:1;               /*!< Status if connection is in closing mode.
                                                    When in closing mode, ignore any possible received data from function */
        } f;
    } status;                                   /*!< Connection status union with flag bits */
} esp_conn_t;

/**
 * \ingroup         ESP_PBUF
 * \brief           Packet buffer structure
 */
typedef struct esp_pbuf_t {
    struct esp_pbuf_t* next;                    /*!< Next pbuf in chain list */
    size_t tot_len;                             /*!< Total length of pbuf chain */
    size_t len;                                 /*!< Length of payload */
    size_t ref;                                 /*!< Number of references to this structure */
    uint8_t* payload;                           /*!< Pointer to payload memory */
    esp_ip_t ip;                                /*!< Remote address for received IPD data */
    esp_port_t port;                            /*!< Remote port for received IPD data */
} esp_pbuf_t;

/**
 * \brief           Incoming network data read structure
 */
typedef struct {
    uint8_t             read;                   /*!< Set to 1 when we should process input data as connection data */
    size_t              tot_len;                /*!< Total length of packet */
    size_t              rem_len;                /*!< Remaining bytes to read in current +IPD statement */
    esp_conn_p          conn;                   /*!< Pointer to connection for network data */
    esp_ip_t            ip;                     /*!< Remote IP address on from IPD data */
    esp_port_t          port;                   /*!< Remote port on IPD data */
    
    size_t              buff_ptr;               /*!< Buffer pointer to save data to */
    esp_pbuf_p          buff;                   /*!< Pointer to data buffer used for receiving data */
} esp_ipd_t;

/**
 * \brief           Message queue structure to share between threads
 */
typedef struct esp_msg {
    esp_cmd_t       cmd_def;                    /*!< Default message type received from queue */
    esp_cmd_t       cmd;                        /*!< Since some commands can have different subcommands, sub command is used here */
    uint8_t         i;                          /*!< Variable to indicate order number of subcommands */
    esp_sys_sem_t   sem;                        /*!< Semaphore for the message */
    uint8_t         is_blocking;                /*!< Status if command is blocking */
    uint32_t        block_time;                 /*!< Maximal blocking time in units of milliseconds. Use 0 to for non-blocking call */
    espr_t          res;                        /*!< Result of message operation */
    espr_t          (*fn)(struct esp_msg *);    /*!< Processing callback function to process packet */
    union {
        struct {
            uint32_t delay;                     /*!< Delay in units of milliseconds before executing first RESET command */
        } reset;                                /*!< Reset message */
        struct {
            uint32_t baudrate;                  /*!< Baudrate for AT port */
        } uart;
        struct {
            esp_mode_t mode;                    /*!< Mode of operation */                    
        } wifi_mode;                            /*!< When message type \ref ESP_CMD_WIFI_CWMODE is used */
#if ESP_CFG_MODE_STATION || __DOXYGEN__
        struct {
            const char* name;                   /*!< AP name */
            const char* pass;                   /*!< AP password */
            const esp_mac_t* mac;               /*!< Specific MAC address to use when connecting to AP */
            uint8_t def;                        /*!< Value indicates to connect as current only or as default */
            uint8_t error_num;                  /*!< Error number on connecting */
        } sta_join;                             /*!< Message for joining to access point */
        struct {
            uint8_t en;                         /*!< Status to enable/disable auto join feature */
        } sta_autojoin;                         /*!< Message for auto join procedure */
        struct {
            const char* ssid;                   /*!< Pointer to optional filter SSID name to search */
            esp_ap_t* aps;                      /*!< Pointer to array to save access points */
            size_t apsl;                        /*!< Length of input array of access points */
            size_t apsi;                        /*!< Current access point array */
            size_t* apf;                        /*!< Pointer to output variable holding number of access points found */
        } ap_list;                              /*!< List for access points */
#endif /* ESP_CFG_MODE_STATION || __DOXYGEN__ */
#if ESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__
        struct {
            const char* ssid;                   /*!< Name of access point */
            const char* pwd;                    /*!< Password of access point */
            esp_ecn_t ecn;                      /*!< Ecryption used */
            uint8_t ch;                         /*!< RF Channel used */
            uint8_t max_sta;                    /*!< Max allowed connected stations */
            uint8_t hid;                        /*!< Configuration if network is hidden or visible */
            uint8_t def;                        /*!< Save as default configuration */
        } ap_conf;                              /*!< Parameters to configure access point */
        struct {
            esp_sta_t* stas;                    /*!< Pointer to array to save access points */
            size_t stal;                        /*!< Length of input array of access points */
            size_t stai;                        /*!< Current access point array */
            size_t* staf;                       /*!< Pointer to output variable holding number of access points found */
        } sta_list;                             /*!< List for stations */
#endif /* ESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__ */
        struct {
            esp_ip_t* ip;                       /*!< Pointer to IP variable */
            esp_ip_t* gw;                       /*!< Pointer to gateway variable */
            esp_ip_t* nm;                       /*!< Pointer to netmask variable */
            uint8_t def;                        /*!< Value for receiving default or current settings */
        } sta_ap_getip;                         /*!< Message for reading station or access point IP */
        struct {
            esp_mac_t* mac;                     /*!< Pointer to MAC variable */
            uint8_t def;                        /*!< Value for receiving default or current settings */
        } sta_ap_getmac;                        /*!< Message for reading station or access point MAC address */
        struct {
            const esp_ip_t* ip;                 /*!< Pointer to IP variable */
            const esp_ip_t* gw;                 /*!< Pointer to gateway variable */
            const esp_ip_t* nm;                 /*!< Pointer to netmask variable */
            uint8_t def;                        /*!< Value for receiving default or current settings */
        } sta_ap_setip;                         /*!< Message for setting station or access point IP */
        struct {
            const esp_mac_t* mac;               /*!< Pointer to MAC variable */
            uint8_t def;                        /*!< Value for receiving default or current settings */
        } sta_ap_setmac;                        /*!< Message for setting station or access point MAC address */
        struct {
            char* hostname;                     /*!< Hostname set/get value */
            size_t length;                      /*!< Length of buffer when reading hostname */
        } wifi_hostname;                        /*!< Set or get hostname structure */
        
        /*
         * Connection based commands
         */
        struct {
            esp_conn_t** conn;                  /*!< Pointer to pointer to save connection used */
            const char* host;                   /*!< Host to use for connection */
            esp_port_t port;                    /*!< Remote port used for connection */
            esp_conn_type_t type;               /*!< Connection type */
            void* arg;                          /*!< Connection custom argument */
            esp_cb_fn cb_func;                  /*!< Callback function to use on connection */
            uint8_t num;                        /*!< Connection number used for start */
        } conn_start;                           /*!< Structure for starting new connection */
        struct {
            esp_conn_t* conn;                   /*!< Pointer to connection to close */
            uint8_t val_id;                     /*!< Connection current validation ID when command was sent to queue */
        } conn_close;
        struct {
            esp_conn_t* conn;                   /*!< Pointer to connection to send data */
            size_t btw;                         /*!< Number of remaining bytes to write */
            size_t ptr;                         /*!< Current write pointer for data */
            const uint8_t* data;                /*!< Data to send */
            size_t sent;                        /*!< Number of bytes sent in last packet */
            size_t sent_all;                    /*!< Number of bytes sent all together */
            uint8_t tries;                      /*!< Number of tries used for last packet */
            uint8_t wait_send_ok_err;           /*!< Set to 1 when we wait for SEND OK or SEND ERROR */
            const esp_ip_t* remote_ip;           /*!< Remote IP address for UDP connection */
            esp_port_t remote_port;               /*!< Remote port address for UDP connection */
            uint8_t fau;                        /*!< Free after use flag to free memory after data are sent (or not) */
            size_t* bw;                         /*!< Number of bytes written so far */
            uint8_t val_id;                     /*!< Connection current validation ID when command was sent to queue */
        } conn_send;                            /*!< Structure to send data on connection */
        
        /*
         * TCP/IP based commands
         */
        struct {
            uint8_t mux;                        /*!< Mux status, either enabled or disabled */
        } tcpip_mux;                            /*!< Used for setting up multiple connections */
        struct {
            uint8_t en;                         /*!< Enable/Disable server status */
            esp_port_t port;                    /*!< Server port number */
            uint16_t max_conn;                  /*!< Maximal number of connections available for server */
            uint16_t timeout;                   /*!< Connection timeout */
            esp_cb_fn cb;                       /*!< Server default callback function */
        } tcpip_server;
        struct {
            size_t size;                        /*!< Size for SSL in uints of bytes */
        } tcpip_sslsize;                        /*!< TCP SSL size for SSL connections */
        struct {
            const char* host;                   /*!< Hostname to resolve IP address for */
            esp_ip_t* ip;                       /*!< Pointer to IP address to save result */
        } dns_getbyhostname;                    /*!< DNS function */
#if ESP_CFG_PING || __DOXYGEN__
        struct {
            const char* host;                   /*!< Hostname to ping */
            uint32_t* time;                     /*!< Pointer to time variable */
        } tcpip_ping;                           /*!< Pinging structure */
#endif /* ESP_CFG_PING || __DOXYGEN__ */
#if ESP_CFG_SNTP || __DOXYGEN__
        struct {
            uint8_t en;                         /*!< Status if SNTP is enabled or not */
            int8_t tz;                          /*!< Timezone setup */
            const char* h1;                     /*!< Optional server 1 */
            const char* h2;                     /*!< Optional server 2 */
            const char* h3;                     /*!< Optional server 3 */
        } tcpip_sntp_cfg;                       /*!< SNTP configuration */
        struct {
            esp_datetime_t* dt;                 /*!< Pointer to datetime structure */
        } tcpip_sntp_time;                      /*!< SNTP get time */
#endif /* ESP_CFG_SNTP || __DOXYGEN__ */
#if ESP_CFG_WPS || __DOXYGEN__
        struct {
            uint8_t en;                         /*!< Status if WPS is enabled or not */
        } wps_cfg;                              /*!< WPS configuration */
#endif /* ESP_CFG_WPS || __DOXYGEN__ */
#if ESP_CFG_MDNS || __DOXYGEN__
        struct {
            uint8_t en;                         /*!< Set to 1 to enable or 0 to disable */
            const char* host;                   /*!< mDNS host name */
            const char* server;                 /*!< mDNS server */
            esp_port_t port;                    /*!< mDNS server port */
        } mdns;                                 /*!< mDNS configuration */
#endif /* ESP_CFG_MDNS || __DOXYGEN__ */
    } msg;                                      /*!< Group of different message contents */
} esp_msg_t;

/**
 * \brief           IP and MAC structure with netmask and gateway addresses
 */
typedef struct {
    esp_ip_t ip;                                /*!< IP address */
    esp_ip_t gw;                                /*!< Gateway address */
    esp_ip_t nm;                                /*!< Netmask address */
    esp_mac_t mac;                              /*!< MAC address */
} esp_ip_mac_t;

/**
 * \brief           Link connection active info
 */
typedef struct {
    uint8_t failed;                             /*!< Status if connection successful */
    uint8_t num;                                /*!< Connection number */
    uint8_t is_server;                          /*!< Status if connection is client or server */
    esp_conn_type_t type;                       /*!< Connection type */
    esp_ip_t remote_ip;                         /*!< Remote IP address */
    esp_port_t remote_port;                     /*!< Remote port */
    esp_port_t local_port;                      /*!< Local port number */
} esp_link_conn_t;

/**
 * \brief           Callback function linked list prototype
 */
typedef struct esp_cb_func {
    struct esp_cb_func* next;                   /*!< Next function in the list */
    esp_cb_fn fn;                               /*!< Function pointer itself */
} esp_cb_func_t;

/**
 * \brief           ESP global structure
 */
typedef struct {    
    uint32_t version_at;                        /*!< Version of AT command software on ESP device */
    uint32_t version_sdk;                       /*!< Version of SDK used to build AT software */

    esp_sys_sem_t       sem_sync;               /*!< Synchronization semaphore between threads */
    esp_sys_mbox_t      mbox_producer;          /*!< Producer message queue handle */
    esp_sys_mbox_t      mbox_process;           /*!< Consumer message queue handle */
    esp_sys_thread_t    thread_producer;        /*!< Producer thread handle */
    esp_sys_thread_t    thread_process;         /*!< Processing thread handle */
#if !ESP_CFG_INPUT_USE_PROCESS || __DOXYGEN__
    esp_buff_t          buff;                   /*!< Input processing buffer */
#endif /* !ESP_CFG_INPUT_USE_PROCESS || __DOXYGEN__ */
    esp_ll_t            ll;                     /*!< Low level functions */
    
    esp_msg_t*          msg;                    /*!< Pointer to current user message being executed */
    
    uint32_t            active_conns;           /*!< Bit field of currently active connections, @todo: In case user has more than 32 connections, single variable is not enough */
    uint32_t            active_conns_last;      /*!< The same as previous but status before last check */
    
    esp_conn_t          conns[ESP_CFG_MAX_CONNS];   /*!< Array of all connection structures */
    
    esp_link_conn_t     link_conn;              /*!< Link connection handle */
    esp_ipd_t           ipd;                    /*!< Incoming data structure */
    esp_cb_t            cb;                     /*!< Callback processing structure */
    
    esp_cb_func_t*      cb_func;                /*!< Callback function linked list */
    esp_cb_fn           cb_server;              /*!< Default callback function for server connections */
    
#if ESP_CFG_MODE_STATION || __DOXYGEN__
    esp_ip_mac_t        sta;                    /*!< Station IP and MAC addressed */
#endif /* ESP_CFG_MODE_STATION || __DOXYGEN__ */
#if ESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__
    esp_ip_mac_t        ap;                     /*!< Access point IP and MAC addressed */
#endif /* ESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__ */
    
    union {
        struct {
            uint8_t     initialized:1;          /*!< Flag indicating ESP library is initialized */
            uint8_t     dev_present:1;          /*!< Flag indicating if physical device is connected to host device */
            uint8_t     r_got_ip:1;             /*!< Flag indicating ESP has IP */
            uint8_t     r_w_conn:1;             /*!< Flag indicating ESP is connected to wifi */
        } f;                                    /*!< Flags structure */
    } status;                                   /*!< Status structure */
    
    uint8_t conn_val_id;                        /*!< Validation ID increased each time device connects to wifi network or on reset.
                                                    It is used for connections */
} esp_t;

/**
 * \brief           Unicode support structure
 */
typedef struct esp_unicode_t {
    uint8_t ch[4];                              /*!< UTF-8 max characters */
    uint8_t t;                                  /*!< Total expected length in UTF-8 sequence */
    uint8_t r;                                  /*!< Remaining bytes in UTF-8 sequence */
    espr_t res;                                 /*!< Current result of processing */
} esp_unicode_t;

/**
 * \}
 */

#if !__DOXYGEN__

/**
 * \ingroup         ESP
 * \defgroup        ESP_PRIVATE Internal functions
 * \brief           Functions, structures and enumerations
 * \{
 */

extern esp_t esp;

#define ESP_MSG_VAR_DEFINE(name)                esp_msg_t* name
#define ESP_MSG_VAR_ALLOC(name)                 do {\
    (name) = esp_mem_alloc(sizeof(*(name)));        \
    ESP_DEBUGW(ESP_CFG_DBG_VAR | ESP_DBG_TYPE_TRACE, (name) != NULL, "MSG VAR: Allocated %d bytes at %p\r\n", sizeof(*(name)), (name)); \
    ESP_DEBUGW(ESP_CFG_DBG_VAR | ESP_DBG_TYPE_TRACE, (name) == NULL, "MSG VAR: Error allocating %d bytes\r\n", sizeof(*(name))); \
    if ((name) == NULL) {                           \
        return espERRMEM;                           \
    }                                               \
    ESP_MEMSET(name, 0x00, sizeof(*(name)));        \
} while (0)
#define ESP_MSG_VAR_REF(name)                   (*(name))
#define ESP_MSG_VAR_FREE(name)                  do {\
    ESP_DEBUGF(ESP_CFG_DBG_VAR | ESP_DBG_TYPE_TRACE, "MSG VAR: Free memory: %p\r\n", (name)); \
    esp_mem_free(name);                             \
    (name) = NULL;                                  \
} while (0)

#define ESP_CHARISNUM(x)                    ((x) >= '0' && (x) <= '9')
#define ESP_CHARTONUM(x)                    ((x) - '0')
#define ESP_CHARISHEXNUM(x)                 (((x) >= '0' && (x) <= '9') || ((x) >= 'a' && (x) <= 'f') || ((x) >= 'A' && (x) <= 'F'))
#define ESP_CHARHEXTONUM(x)                 (((x) >= '0' && (x) <= '9') ? ((x) - '0') : (((x) >= 'a' && (x) <= 'f') ? ((x) - 'a' + 10) : (((x) >= 'A' && (x) <= 'F') ? ((x) - 'A' + 10) : 0)))
#define ESP_ISVALIDASCII(x)                 (((x) >= 32 && (x) <= 126) || (x) == '\r' || (x) == '\n')

#define CMD_IS_CUR(c)                   (esp.msg != NULL && esp.msg->cmd == (c))
#define CMD_IS_DEF(c)                   (esp.msg != NULL && esp.msg->cmd_def == (c))
#define CMD_GET_CUR()                   ((esp_cmd_t)(((esp.msg != NULL) ? esp.msg->cmd : ESP_CMD_IDLE)))
#define CMD_GET_DEF()                   ((esp_cmd_t)(((esp.msg != NULL) ? esp.msg->cmd_def : ESP_CMD_IDLE)))

#if !__DOXYGEN__
typedef struct {
    char data[128];
    uint8_t len;
} esp_recv_t;
#endif /* !__DOXYGEN__ */

#define CRLF                            "\r\n"

#define RECV_ADD(ch)                    do { recv_buff.data[recv_buff.len++] = ch; recv_buff.data[recv_buff.len] = 0; } while (0)
#define RECV_RESET()                    do { recv_buff.len = 0; recv_buff.data[0] = 0; } while (0)
#define RECV_LEN()                      recv_buff.len
#define RECV_IDX(index)                 recv_buff.data[index]

#define ESP_AT_PORT_SEND_BEGIN()        do { ESP_AT_PORT_SEND_STR("AT"); } while (0)
#define ESP_AT_PORT_SEND_END()          do { ESP_AT_PORT_SEND_STR(CRLF); } while (0)

#define ESP_AT_PORT_SEND_STR(str)       esp.ll.send_fn((const uint8_t *)(str), (uint16_t)strlen(str))
#define ESP_AT_PORT_SEND_CHR(str)       esp.ll.send_fn((const uint8_t *)(str), (uint16_t)1)
#define ESP_AT_PORT_SEND(d, l)          esp.ll.send_fn((const uint8_t *)(d), (uint16_t)l)

#define ESP_AT_PORT_SEND_QUOTE_COND(q)  do { if ((q)) { ESP_AT_PORT_SEND_STR("\""); } } while (0)
#define ESP_AT_PORT_SEND_COMMA_COND(c)  do { if ((c)) { ESP_AT_PORT_SEND_STR(","); } } while (0)
#define ESP_AT_PORT_SEND_EQUAL_COND(e)  do { if ((e)) { ESP_AT_PORT_SEND_STR("="); } } while (0)

#define ESP_AT_PORT_SEND_CUR_DEF(is_d, e)  do { ESP_AT_PORT_SEND_STR((is_d) ? "_DEF" : "_CUR"); ESP_AT_PORT_SEND_EQUAL_COND(e); } while (0)

#define ESP_PORT2NUM(port)              ((uint32_t)(port))

/**
 * \brief           Protect (count up) OS protection (mutex)
 */
#define ESP_CORE_PROTECT()                  esp_sys_protect()

/**
 * \brief           Unprotect (count down) OS protection (mutex)
 */
#define ESP_CORE_UNPROTECT()                esp_sys_unprotect()

const char * espi_dbg_msg_to_string(esp_cmd_t cmd);
espr_t      espi_process(const void* data, size_t len);
espr_t      espi_process_buffer(void);
espr_t      espi_initiate_cmd(esp_msg_t* msg);
uint8_t     espi_is_valid_conn_ptr(esp_conn_p conn);
espr_t      espi_send_cb(esp_cb_type_t type);
espr_t      espi_send_conn_cb(esp_conn_t* conn, esp_cb_fn cb);
void        espi_conn_init(void);
void        espi_conn_start_timeout(esp_conn_p conn);
espr_t      espi_send_msg_to_producer_mbox(esp_msg_t* msg, espr_t (*process_fn)(esp_msg_t *), uint32_t block, uint32_t max_block_time);
uint32_t    espi_get_from_mbox_with_timeout_checks(esp_sys_mbox_t* b, void** m, uint32_t timeout);

void        espi_reset_everything(uint8_t forced);

/**
 * \}
 */

#endif /* !__DOXYGEN__ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ESP_PRIV_H */
