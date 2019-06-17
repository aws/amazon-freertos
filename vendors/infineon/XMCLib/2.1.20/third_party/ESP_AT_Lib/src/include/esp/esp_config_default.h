/**
 * \file            esp_config_default.h
 * \brief           Default configuration for ESP
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
#ifndef __ESP_DEFAULT_CONFIG_H
#define __ESP_DEFAULT_CONFIG_H

/**
 * \defgroup        ESP_CONF Configuration
 * \brief           Configuration parameters
 * \{
 *
 */

/**
 * \brief           Enables (`1`) or disables (`0`) operating system support for ESP library
 *
 * \note            Value must be set to `1` in the current revision
 *
 * \note            Check \ref ESP_CONF_OS group for more configuration related to operating system
 *
 */
#ifndef ESP_CFG_OS
#define ESP_CFG_OS                          1
#endif

/**
 * \brief           Default system port implementation
 *
 *                  According to selected port, stack will automatically include apropriate file
 *
 *                  Parameter can be a value of \ref ESP_SYS_PORTS choices
 */
#ifndef ESP_CFG_SYS_PORT
#define ESP_CFG_SYS_PORT                    ESP_SYS_PORT_CMSIS_OS
#endif

/**
 * \brief           Memory alignment for dynamic memory allocations
 *
 * \note            Some CPUs can work faster if memory is aligned, usually to `4` or `8` bytes.
 *                  To speed up this possibilities, you can set memory alignment and library
 *                  will try to allocate memory on aligned boundaries.
 *
 * \note            Some CPUs such ARM Cortex-M0 dont't support unaligned memory access.
 *                  This CPUs must have set correct memory alignment value. 
 *
 * \note            This value must be power of `2`
 */
#ifndef ESP_CFG_MEM_ALIGNMENT
#define ESP_CFG_MEM_ALIGNMENT               4
#endif

/**
 * \brief           Maximal number of connections AT software can support on ESP device
 * \note            In case of official AT software, leave this on default value (`5`)
 */
#ifndef ESP_CFG_MAX_CONNS
#define ESP_CFG_MAX_CONNS                   5
#endif

/**
 * \brief           Maximal number of bytes we can send at single command to ESP
 * \note            Value can not exceed `2048` bytes or no data won't be send at all (ESP8266 AT SW limitation)
 *
 * \note            This is limitation of ESP AT commands and on systems where RAM
 *                  is not an issue, it should be set to maximal value (`2048`)
 *                  to optimize speed performance of sending data
 */
#ifndef ESP_CFG_CONN_MAX_DATA_LEN
#define ESP_CFG_CONN_MAX_DATA_LEN           2048
#endif

/**
 * \brief           Set number of retries for send data command.
 *
 *                  Sometimes it can happen that `AT+SEND` command fails due to different problems.
 *                  Trying to send the same data multiple times can raise chances for success.
 */
#ifndef ESP_CFG_MAX_SEND_RETRIES
#define ESP_CFG_MAX_SEND_RETRIES            3
#endif

/**
 * \brief           Maximal buffer size for entries in +IPD statement from ESP
 *
 * \note            If `+IPD` length is larger that this value, 
 *                  multiple pbuf entries will be created to hold entire `+IPD`
 */
#ifndef ESP_CFG_IPD_MAX_BUFF_SIZE
#define ESP_CFG_IPD_MAX_BUFF_SIZE           1460
#endif

/**
 * \brief           Default baudrate used for AT port
 *
 * \note            User may call API function to change to desired baudrate if necessary
 */
#ifndef ESP_CFG_AT_PORT_BAUDRATE
#define ESP_CFG_AT_PORT_BAUDRATE            115200
#endif

/**
 * \brief           Enables (`1`) or disables (`0`) ESP acting as station
 * 
 * \note            When device is in station mode, it can connect to other access points
 */
#ifndef ESP_CFG_MODE_STATION
#define ESP_CFG_MODE_STATION                1
#endif

/**
 * \brief           Enables (`1`) or disables (`0`) ESP acting as access point
 * 
 * \note            When device is in access point mode, it can accept connections from other stations
 */
#ifndef ESP_CFG_MODE_ACCESS_POINT
#define ESP_CFG_MODE_ACCESS_POINT           1
#endif

/**
 * \brief           Buffer size for received data waiting to be processed
 * \note            When server mode is active and a lot of connections are in queue
 *                  this should be set high otherwise your buffer may overflow
 * 
 * \note            Buffer size also depends on TX user driver if it uses DMA or blocking mode.
 *                  In case of DMA (CPU can work other tasks), buffer may be smaller as CPU
 *                  will have more time to process all the incoming bytes
 *
 * \note            This parameter has no meaning when \ref ESP_CFG_INPUT_USE_PROCESS is enabled
 */
#ifndef ESP_CFG_RCV_BUFF_SIZE
#define ESP_CFG_RCV_BUFF_SIZE               0x400
#endif

/**
 * \brief           Enables (`1`) or disables (`0`) reset sequence after \ref esp_init call
 *
 * \note            When this functionality is disabled, user must manually call \ref esp_reset to send
 *                  reset sequence to ESP device.
 */
#ifndef ESP_CFG_RESET_ON_INIT
#define ESP_CFG_RESET_ON_INIT               1
#endif

/**
 * \brief           Default delay (milliseconds unit) before sending first AT command on reset sequence
 */
#ifndef ESP_CFG_RESET_DELAY_DEFAULT
#define ESP_CFG_RESET_DELAY_DEFAULT         1000
#endif

/**
 * \defgroup        ESP_CONF_DBG Debugging
 * \brief           Debugging configurations
 * \{
 */

/**
 * \brief           Set global debug support.
 *                  Possible values are \ref ESP_DBG_ON or \ref ESP_DBG_OFF
 *
 * \note            Set to \ref ESP_DBG_OFF to globally disable all debugs
 */
#ifndef ESP_CFG_DBG
#define ESP_CFG_DBG                         ESP_DBG_OFF
#endif

/**
 * \brief           Debugging output function
 * 
 *                  Called with format and optional parameters for printf style debug
 */
#ifndef ESP_CFG_DBG_OUT
#define ESP_CFG_DBG_OUT(fmt, ...)           do { extern int printf( const char * format, ... ); printf(fmt, ## __VA_ARGS__); } while (0)
#endif

/**
 * \brief           Minimal debug level
 *
 */
#ifndef ESP_CFG_DBG_LVL_MIN
#define ESP_CFG_DBG_LVL_MIN                 ESP_DBG_LVL_ALL
#endif

/**
 * \brief           Enabled debug types
 *                  
 *                  When debug is globally enabled with \ref ESP_CFG_DBG parameter,
 *                  user must enable debug types such as TRACE or STATE messages.
 */
#ifndef ESP_CFG_DBG_TYPES_ON
#define ESP_CFG_DBG_TYPES_ON                0
#endif

/**
 * \brief           Set debug level for memory manager
 *
 *                  Possible values are \ref ESP_DBG_ON or \ref ESP_DBG_OFF
 */
#ifndef ESP_CFG_DBG_MEM
#define ESP_CFG_DBG_MEM                     ESP_DBG_OFF
#endif

/**
 * \brief           Set debug level for input module
 *
 *                  Possible values are \ref ESP_DBG_ON or \ref ESP_DBG_OFF
 */
#ifndef ESP_CFG_DBG_INPUT
#define ESP_CFG_DBG_INPUT                   ESP_DBG_OFF
#endif

/**
 * \brief           Set debug level for ESP threads
 *
 *                  Possible values are \ref ESP_DBG_ON or \ref ESP_DBG_OFF
 */
#ifndef ESP_CFG_DBG_THREAD
#define ESP_CFG_DBG_THREAD                  ESP_DBG_OFF
#endif

/**
 * \brief           Set debug level for asserting of input variables
 *
 *                  Possible values are \ref ESP_DBG_ON or \ref ESP_DBG_OFF
 */
#ifndef ESP_CFG_DBG_ASSERT
#define ESP_CFG_DBG_ASSERT                  ESP_DBG_OFF
#endif

/**
 * \brief           Set debug level for incoming data received from device
 *
 *                  Possible values are \ref ESP_DBG_ON or \ref ESP_DBG_OFF
 */
#ifndef ESP_CFG_DBG_IPD
#define ESP_CFG_DBG_IPD                     ESP_DBG_OFF
#endif

/**
 * \brief           Set debug level for netconn sequential API
 *
 *                  Possible values are \ref ESP_DBG_ON or \ref ESP_DBG_OFF
 */
#ifndef ESP_CFG_DBG_NETCONN
#define ESP_CFG_DBG_NETCONN                 ESP_DBG_OFF
#endif

/**
 * \brief           Set debug level for packet buffer manager
 *
 *                  Possible values are \ref ESP_DBG_ON or \ref ESP_DBG_OFF
 */
#ifndef ESP_CFG_DBG_PBUF
#define ESP_CFG_DBG_PBUF                    ESP_DBG_OFF
#endif

/**
 * \brief           Set debug level for connections
 *
 *                  Possible values are \ref ESP_DBG_ON or \ref ESP_DBG_OFF
 */
#ifndef ESP_CFG_DBG_CONN
#define ESP_CFG_DBG_CONN                    ESP_DBG_OFF
#endif

/**
 * \brief           Set debug level for dynamic variable allocations
 *
 *                  Possible values are \ref ESP_DBG_ON or \ref ESP_DBG_OFF
 */
#ifndef ESP_CFG_DBG_VAR
#define ESP_CFG_DBG_VAR                     ESP_DBG_OFF
#endif

/**
 * \brief           Enables (`1`) or disables (`0`) echo mode on AT commands
 *                  sent to ESP device.
 *
 * \note            This mode is useful when debugging ESP communication
 */
#ifndef ESP_CFG_AT_ECHO
#define ESP_CFG_AT_ECHO                     0
#endif
 
/**
 * \}
 */

/**
 * \defgroup        ESP_CONF_OS OS configuration
 * \brief           Operating system dependant configuration
 * \{
 */
 
/**
 * \brief           Set number of message queue entries for procuder thread
 *
 *                  Message queue is used for storing memory address to command data
 */
#ifndef ESP_CFG_THREAD_PRODUCER_MBOX_SIZE
#define ESP_CFG_THREAD_PRODUCER_MBOX_SIZE   16
#endif

/**
 * \brief           Set number of message queue entries for processing thread
 *
 *                  Message queue is used to notify processing thread about new received data on AT port
 */
#ifndef ESP_CFG_THREAD_PROCESS_MBOX_SIZE
#define ESP_CFG_THREAD_PROCESS_MBOX_SIZE    16
#endif

/**
 * \brief           Enables (`1`) or disables (`0`) direct support for processing input data
 *
 *                  When this mode is enabled, no overhead is included for copying data
 *                  to receive buffer because bytes are processed directly by \ref esp_input_process function
 *
 *                  If this mode is not enabled, then user have to send every received byte via \ref esp_input
 *                  function to the internal buffer for future processing. This may introduce additional overhead
 *                  with data copy and may decrease library performance
 *
 * \note            This mode can only be used when \ref ESP_CFG_OS is enabled
 *
 * \note            When using this mode, separate thread must be dedicated only 
 *                  for reading data on AT port. It is usually implemented in LL driver
 *
 * \note            Best case for using this mode is if DMA receive is supported by host device
 */
#ifndef ESP_CFG_INPUT_USE_PROCESS
#define ESP_CFG_INPUT_USE_PROCESS           0
#endif

/**
 * \brief           Producer thread hook, called each time thread wakes-up and does the processing.
 *
 *                  It can be used to check if thread is alive.
 */
#ifndef ESP_THREAD_PRODUCER_HOOK
#define ESP_THREAD_PRODUCER_HOOK()
#endif

/**
 * \brief           Process thread hook, called each time thread wakes-up and does the processing.
 *
 *                  It can be used to check if thread is alive.
 */
#ifndef ESP_THREAD_PROCESS_HOOK
#define ESP_THREAD_PROCESS_HOOK()
#endif

/**
 * \}
 */

/**
 * \defgroup        ESP_CONF_MODULES Modules
 * \brief           Configuration of specific modules
 * \{
 */

/**
 * \defgroup        ESP_CONF_MODULES_NETCONN Netconn module
 * \brief           Configuration of netconn API module
 * \{
 */
 
/**
 * \brief           Enables (`1`) or disables (`0`) NETCONN sequential API support for OS systems
 *
 * \note            To use this feature, OS support is mandatory. 
 * \sa              ESP_CFG_OS
 */
#ifndef ESP_CFG_NETCONN
#define ESP_CFG_NETCONN                     0
#endif

/**
 * \brief           Accept and put a new connection to receive message queue immediately when connections starts
 *
 *                  If this parameter is set to 0, a new connection info will be written 
 *                  to accept mbox only when first data packet arrives to device,
 *                  otherwise it will be immediately written to receive mbox.
 */
#ifndef ESP_CFG_NETCONN_ACCEPT_ON_CONNECT
#define ESP_CFG_NETCONN_ACCEPT_ON_CONNECT   1
#endif

/**
 * \brief           Enables (`1`) or disables (`0`) receive timeout feature
 *
 *                  When this option is enabled, user will get an option
 *                  to set timeout value for receive data on netconn,
 *                  before function returns timeout error.
 *
 * \note            Even if this option is enabled, user must still manually set timeout, 
 *                  by default time will be set to 0 which means no timeout.
 */
#ifndef ESP_CFG_NETCONN_RECEIVE_TIMEOUT
#define ESP_CFG_NETCONN_RECEIVE_TIMEOUT     0
#endif

/**
 * \brief           Accept queue length for new client when netconn server is used
 *
 *                  Defines number of maximal clients waiting in accept queue of server connection
 */
#ifndef ESP_CFG_NETCONN_ACCEPT_QUEUE_LEN
#define ESP_CFG_NETCONN_ACCEPT_QUEUE_LEN    5
#endif

/**
 * \brief           Receive queue length for pbuf entries
 *
 *                  Defines maximal number of pbuf data packet references for receive
 */
#ifndef ESP_CFG_NETCONN_RECEIVE_QUEUE_LEN
#define ESP_CFG_NETCONN_RECEIVE_QUEUE_LEN   8
#endif

/**
 * \}
 */

/**
 * \defgroup        ESP_CONF_MODULES_REST_CLIENT HTTP client module
 * \brief           Configuration of HTTP REST client API module
 * \{
 */
 
/**
 * \brief           Enables (`1`) or disables (`0`) HTTP REST client sequential API support for OS systems
 *
 * \note            To use this feature, OS support is mandatory. 
 * \sa              ESP_CFG_OS
 */
#ifndef ESP_CFG_REST_CLIENT
#define ESP_CFG_REST_CLIENT                 0
#endif

/**
 * \}
 */

/**
 * \brief           Enables (`1`) or disables (`0`) support for DNS functions
 *
 */
#ifndef ESP_CFG_DNS
#define ESP_CFG_DNS                         0
#endif
 
/**
 * \brief           Enables (`1`) or disables (`0`) support for ping functions
 *
 */
#ifndef ESP_CFG_PING
#define ESP_CFG_PING                        0
#endif
 
/**
 * \brief           Enables (`1`) or disables (`0`) support for WPS functions
 *
 */
#ifndef ESP_CFG_WPS
#define ESP_CFG_WPS                         0
#endif

/**
 * \brief           Enables (`1`) or disables (`0`) support for SNTP protocol with AT commands
 *
 */
#ifndef ESP_CFG_SNTP
#define ESP_CFG_SNTP                        0
#endif

/**
 * \brief           Enables (`1`) or disables (`0`) support for mDNS
 *
 */
#ifndef ESP_CFG_MDNS
#define ESP_CFG_MDNS                        0
#endif

/**
 * \brief           Enables (`1`) or disables (`0`) support for SNTP protocol with AT commands
 *
 */
#ifndef ESP_CFG_HOSTNAME
#define ESP_CFG_HOSTNAME                    0
#endif

/**
 * \}
 */

/**
 * \brief           Poll interval for connections in units of milliseconds
 *
 *                  Value indicates interval time to call poll event on active connections.
 *
 * \note            Single poll interval applies for all connections
 */
#ifndef ESP_CFG_CONN_POLL_INTERVAL
#define ESP_CFG_CONN_POLL_INTERVAL          500
#endif

/**
 * \defgroup        ESP_CONF_STD_LIB Standard library
 * \brief           Standard C library configuration
 * \{
 *
 * Configuration allows you to overwrite default C language function
 * in case of better implementation with hardware (for example DMA for data copy).
 */

/**
 * \brief           Memory copy function declaration
 * 
 *                  User is able to change the memory function and in case
 *                  hardware supports copy operation, it may implement its own implementation
 *
 *                  Function prototype must be similar to:
 * \code{c}
void *  my_memcpy(void* dst, const void* src, size_t len);
\endcode
 *
 * \param[in]       dst: Destination memory start address
 * \param[in]       src: Source memory start address
 * \param[in]       len: Number of bytes to copy
 * \return          Destination memory start address
 */
#ifndef ESP_MEMCPY
#define ESP_MEMCPY(dst, src, len)           memcpy(dst, src, len)
#endif

/**
 * \brief           Memory set function declaration
 * 
 *                  User is able to change the memory function and in case
 *                  hardware supports copy operation, it may implement its own implementation
 *
 *                  Function prototype must be similar to:
 * \code{c}
void *  my_memset(void* dst, int b, size_t len);
\endcode
 *
 * \param[in]       dst: Destination memory start address
 * \param[in]       b: Value (byte) to set in memory
 * \param[in]       len: Number of bytes to set
 * \return          Destination memory start address
 */
#ifndef ESP_MEMSET
#define ESP_MEMSET(dst, b, len)             memset(dst, b, len)
#endif
 
/**
 * \}
 */

/**
 * \}
 */
 
#if !__DOXYGEN__

/* Define group mode value */
#define ESP_CFG_MODE_STATION_ACCESS_POINT   (ESP_CFG_MODE_STATION && ESP_CFG_MODE_ACCESS_POINT)

/* At least one of them must be enabled */
#if !ESP_CFG_MODE_STATION && !ESP_CFG_MODE_ACCESS_POINT
#error "Invalid ESP configuration. ESP_CFG_MODE_STATION and ESP_CFG_MODE_STATION cannot be disabled at the same time!"
#endif

#if !ESP_CFG_OS
    #if ESP_CFG_INPUT_USE_PROCESS
    #error "ESP_CFG_INPUT_USE_PROCESS may only be enabled when OS is used!"
    #endif /* ESP_CFG_INPUT_USE_PROCESS */
#endif /* !ESP_CFG_OS */

/* Check for WPS functionality */
#if ESP_CFG_WPS && !ESP_CFG_MODE_STATION
#error "WPS function may only be used when station mode is enabled!"
#endif /* ESP_CFG_WPS && !ESP_CFG_MODE_STATION */

#endif /* !__DOXYGEN__ */

#endif /* __ESP_DEFAULT_CONFIG_H */
