/*
 * Copyright (c) 2019-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

#ifndef __SL_NET_CONN_H__
#define __SL_NET_CONN_H__

#include <stdint.h>
#include <stdbool.h>

#include <ti/net/slnetsock.h>
#include <ti/net/slnetutils.h>
#include <ti/net/slneterr.h>


#ifdef    __cplusplus
extern "C" {
#endif


/*!
    \defgroup SlNetConn SlNetConn group

    \short Managing and monitoring network connections

*/
/*!

    \addtogroup SlNetConn
    @{

*/

/*****************************************************************************/
/* Macro declarations                                                        */
/*****************************************************************************/

/*! @cond INTERNAL */

/* Maximum number of NetIf supported */
#define SLNETCONN_NETWORK_INTERFACES_NUM    2

/* Maximum number of Users supported */
#define SLNETCONN_USERS_NUM                 8

/* Enable/Disable Module's Logs
 * Registration of a debug print callback will be required when enable
 *
 * \sa - SlNetConn_registerDebugCallback */
#define SLNETCONN_DEBUG_ENABLE              (0)

/*! @endcond */

/*****************************************************************************/
/* Structure/Enum declarations                                               */
/*****************************************************************************/

typedef enum
{
    SLNETCONN_SERVICE_LVL_MAC                = 0,
    SLNETCONN_SERVICE_LVL_IP                 = 1,
    SLNETCONN_SERVICE_LVL_INTERNET           = 2
} SlNetConnServiceLevel_e;

/* Note: SLNETCONN_STATUS_CONNECTED serves as a baseline for the CONNECTED statuses.
 * It is equal to SLNETCONN_STATUS_CONNECTED_MAC (as SLNETCONN_SERVICE_LVL_MAC = 0).
 * Note: Internet Connection is not maintained - it will be checked only upon the first
 *       connection, but the system will not identify if the access-point loses the Internet access.
 *       Users may check the connection periodically using SlNetConn_getStatus() (with
 *       bCheckInternetConnection set to true).
 *       User can also use socket error indication as the trigger for Internet access check.
 */
typedef enum
{
    SLNETCONN_STATUS_DISCONNECTED            = 1,
    SLNETCONN_STATUS_WAITING_FOR_CONNECTION  = 2,
    SLNETCONN_STATUS_CONNECTED               = 3,
    SLNETCONN_STATUS_CONNECTED_MAC           = SLNETCONN_STATUS_CONNECTED + SLNETCONN_SERVICE_LVL_MAC,
    SLNETCONN_STATUS_CONNECTED_IP            = SLNETCONN_STATUS_CONNECTED + SLNETCONN_SERVICE_LVL_IP,
    SLNETCONN_STATUS_CONNECTED_INTERNET      = SLNETCONN_STATUS_CONNECTED + SLNETCONN_SERVICE_LVL_INTERNET,
} SlNetConnStatus_e;



/*!
    Application callback function prototype.

    This function is per application and is called by the connection manager
    when an event occurs after the connection is established.

    \sa SlNetConn_start(), SlNetConn_stop()
 */
typedef void (*SlNetConn_AppEvent_f)(uint32_t ifID, SlNetConnStatus_e netStatus, void* data);

/*!
    \brief Global parameters and attributes used to establish connections.

    This structure is set using the SlNetConn_setConfiguration(). If the
    function is not called the default values will apply.

    \sa     SlNetConn_setConfiguration()
*/

typedef struct SlNetConn_config_t
{
    uint32_t ifBitmap;          /**< bitmask of SlNet interfaces that
                                 * will be used to establish the
                                 * connection. The bit corresponds to
                                 * the I/F ID as defined when using
                                 * SlNetIf_add(). Default is 0 (which
                                 * means all available interfaces).
                                 */
    char *pServerAddress;       /**< IP address of a global server
                                 * used to verify the internet
                                 * connection. Default is "8.8.8.8"
                                 * (google DNS server).
                                 */
    uint16_t serverAddrFamily;  /**< global server address family:
                                 *    - #SLNETSOCK_AF_INET
                                 *    - #SLNETSOCK_AF_INET6
                                 */
} SlNetConn_config_t;


/*! @cond INTERNAL */
/*!
    Debug print callback function prototype.
    This function can be used by the application to register
    a printing method. This will enable the module's log.

    \sa SlNetConn_registerDebugCallback()
 */
typedef void (*SlNetConn_Debug_f)(const char *  _format, ...);

/*! @endcond */

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/

/*!

    \brief Initialize the SlNetConn module

    SlNetConn_init() initializes the SlNetConn module.  Internally, this includes:
    - Allocating a connection manager CB
    - Registering a SlNetIf events handler
    - Creating a message queue to handle requests between the application
      and connection manager threads

    This function is called by the application layer and can be called only once

    \param[in] flags            Reserved

    \return                     Zero on success, negative error code on failure
*/
int32_t SlNetConn_init(int32_t flags);

/*!

    \brief Sets the SlNetConn global configuration parameters.

    The use of this function is optional. If not called, SlNetConn will use
    the default configuration.

    SlNetConn_setConfiguration() can only be called after SlNetConn_init() and
    before SlNetConn_start().

    \param[in] pConf            SlNetConn configuration (see SlNetConn_config_t)

    \return                     Zero on success, negative error code on failure
*/
int32_t SlNetConn_setConfiguration(SlNetConn_config_t *pConf);


/*!

    \brief Request a connection (of specific target level) to be establish.

    SlNetConn_start() passes the connection manager a request to start a connection.

    The connection request is valid until SlNetConn_stop() is called.

    The fAppEventHdl identifies the request (same "fAppEventHdl" should be used in the SlNetConn_stop()).

    The function is blocking (with timeout) until the requested connection is established or until
    the timeout expires (if timeout is 0, the function will return immediately). Timeout expiration doesn't cancel the request.
    The Start Connection handles temporary disconnections ("link down"). The user will get a "Disconnection" indication
    through the fAppEventHdl, but the connection is expected to re-established automatically (when possible).
    Following the return of the function, the user will be notified through the event handler on the
    "Connection" and "Disconnection" events. Upon disconnection, the user may use SlNetConn_waitForConnection() to
    block the execution until the link is up again.

    This function can be called from several threads.

    This function must be called after SlNetConn_init() and SlNetConn_setConfiguration().

    \param[in] targetServiceLevel            Specifies the requested connection level
    \param[in] fAppEventHdl                  Pointer to user callback function. A unique callback should be used per SlNetConn_start()
                                             request.
                                             The callback identifies the user request and should be used upon SlNetConn_stop().
    \param[in] timeout                       Is an upper bound (in seconds) for the time the SlNetConn_start() can be blocked.
                                             0 means non-blocking command - The command will return immediately.
                                             Value above 0xffff seconds mean infinity timeout.
    \param[in] flags                         reserved

    \return                                  0 on success (i.e. connected with the requested service level),
                                             1 when connection is still in process, or negative error code on failure.
                                             return code of '1' will occur when the timeout (even when timeout == 0) expires. It
                                             is not considered a failure. The connection indication (and any other status change) will
                                             be notified through fAppEventHdl. Since the request is still active, user may call
                                             SlNetConn_stop() in this case.
                                             Any (negative) error code means the Start request has failed.

    \sa                                      SlNetConn_stop()
*/
int32_t SlNetConn_start(SlNetConnServiceLevel_e targetServiceLevel, SlNetConn_AppEvent_f fAppEventHdl,
                        uint32_t timeout, uint32_t flags);


/*!

    \brief Remove user's connection request.

    SlNetConn_stop() cancels a connection request  (identified by user event handler).
    When the last user request is being removed, a disconnection will be triggered.
    This function is non-blocking and called by the application layer.
    This function must be called after SlNetConn_init() and SlNetConn_setConfiguration().

    \param[in] fAppEventHdl     Pointer to user callback function. This should be a unique
                                Callback pointer that corresponds to specific SlNetConn_start()
                                request.

    \return                     Zero on success, or negative error code on failure

    \sa                         SlNetConn_start()
*/
int32_t SlNetConn_stop(SlNetConn_AppEvent_f fAppEventHdl);


/*!

    \brief Wait for connection (of specific target level) to be establish.

    SlNetConn_waitForConnection() waits until the requested connection is established.
    The function doesn't change user requests status and doesn't trigger a connection. It must
    follow a SlNetConn_start() with similar (or higher) target service level.
    This function is blocking.
    This function can be called from several threads.
    This function must be called after SlNetConn_init() and SlNetConn_setConfiguration().

    \param[in] targetServiceLevel            Specifies the requested connection level
    \param[in] timeout                       Is an upper bound (in seconds) for the time the SlNetConn_waitForConnection() can be blocked.
                                             0 means non-blocking command. The command will return immediately. The
                                             return code can still be '1' (i.e. Timeout), if the connection setup
                                             is in progress.
                                             Value above 0xffff seconds mean infinity timeout.

    \return                                  0 on success (i.e. connected with the requested service level),
                                             1 when connection is still in process, or negative error code on failure.
                                             This will occur when the timeout (even when timeout == 0) expires.
                                             Any (negative) error code means the Start request has failed.
*/
int32_t SlNetConn_waitForConnection(SlNetConnServiceLevel_e targetServiceLevel, uint32_t timeout);

/*!

    \brief Retrieve the current connection status.

    This will be used by the user poll the current status of the connection manager. If an IP
    level of connection is already established, the bCheckInternetConnection can be set to
    request the SlNetConn to check whether an internet access is available.

    \param[in]  bCheckInternetConnection     Perform internet connection (if IP Address is Acquired)
                                             before reporting the active service level.
    \param[out] pStatus                      The current active service level

    \return                                  Zero on success, or negative error code on failure
*/
int32_t SlNetConn_getStatus(bool bCheckInternetConnection, SlNetConnStatus_e *pStatus);

/*!
    \brief SlNetConn_process callback - Connection manager thread, handles all
    connection requests.

    This is the main thread context of the SlNetConn, it will be used to process SlNetIf
    events and to invoke application's callbacks.

    \param[in] pvParameters      Task parameters (should be kept null).

    The thread (that uses this function) needs to be created by the
    application before SlNetConn_start() is called.
*/
void *SlNetConn_process(void* pvParameters);

/*! @cond INTERNAL */
#if SLNETCONN_DEBUG_ENABLE
/*!
    \brief register debug print handler

    This function can be used by the application to register
    a debug printing method. This will enable the module's log.

    \param[in] fDebugPrint      Application level debug method.
*/
void SlNetConn_registerDebugCallback(SlNetConn_Debug_f fDebugPrint);
#endif
/*! @endcond */

/*!

 Close the Doxygen group.
 @}

*/


#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* __NET_CONN_H__ */
