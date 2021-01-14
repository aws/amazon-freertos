/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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

#ifndef SLWIFICONN_H_
#define SLWIFICONN_H_

#include <stdint.h>
#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/net/slnetif.h>

#define SLWIFICONN_DEBUG_ENABLE     (1)

/* Additional command (on top Simplelink provisioning commands for
 * supporting provisioning with EXTERNAL CONFIGURATION without internal configuration
 * (e.g. for WAC only support)
 */
#define SL_WLAN_PROVISIONING_CMD_START_MODE_EXTERNAL_CONFIGURATION   10


/****************************************************************************
                      TYPE DEFINITION (structs and enum)
****************************************************************************/
/* Wifi APP Events -
 * IMPORTANT NOTE: Do no use blocking operations or long processing in the event callback context.
 *                 Trigger other tasks to process the events
 */
typedef enum
{
    /* Indication when WIFI RF is POWERED UP -
     * this should mainly informational (user display) purposes
     */
    WifiConnEvent_POWERED_UP,

    /* Indication when WIFI RF is POWERED DOWN (per user request or when going idle due to
     * scan/connections failure -
     * this should mainly informational (user display) purposes
     */
    WifiConnEvent_POWERED_DOWN,

    /* Indication when PROVISIONING process starts  -
     * this should mainly informational (user display) purposes
     * Provisioning is only enabled using SlWifiConn_enableProvisioning.
     */
    WifiConnEvent_PROVISIONING_STARTED,

    /* Indication when PROVISIONING process starts  -
     * this should mainly informational (user display) purposes.
     * Provisioning is only enabled using SlWifiConn_enableProvisioning.
     */
    WifiConnEvent_PROVISIONING_STOPPED,

    /* Indication when Provisioning with External Configuration (e.g. WAC) is ready to start -
     * this should trigger the external provisioning task.
     * External Provisioning is only enabled using SlWifiConn_enableProvisioning with
     * SL_WLAN_PROVISIONING_CMD_START_MODE_APSC_EXTERNAL_CONFIGURATION or
     * SL_WLAN_PROVISIONING_CMD_START_EXTERNAL_CONFIGURATION
     */
    WifiConnEvent_EXTERNAL_PROVISIONING_START_REQ,

    /* Indication when Provisioning with External Configuration (e.g. WAC) needs to be stopped -
     * this should stop the external provisioning task.
     * External Provisioning is only enabled using SlWifiConn_enableProvisioning with
     * SL_WLAN_PROVISIONING_CMD_START_MODE_APSC_EXTERNAL_CONFIGURATION or
     * SL_WLAN_PROVISIONING_CMD_START_EXTERNAL_CONFIGURATION
     */
    WifiConnEvent_EXTERNAL_PROVISIONING_STOP_REQ,

    /* Indication when external Cloud Confirmation is ready to be started -
     * this should trigger the external confirmation task.
     * Cloud Confirmation is only enabled using SlWifiConn_enableProvisioning with
     * flag == SL_WLAN_PROVISIONING_CMD_FLAG_EXTERNAL_CONFIRMATION
     */
    WifiConnEvent_CLOUD_CONFIRMATION_START_REQ,

    /* Indication when external Cloud Confirmation needs to be stopped -
     * this should stop the external confirmation task.
     * External Provisioning is only enabled using SlWifiConn_enableProvisioning with
     * Cloud Confirmation is only enabled using SlWifiConn_enableProvisioning with
     * flag == SL_WLAN_PROVISIONING_CMD_FLAG_EXTERNAL_CONFIRMATION
     */
    WifiConnEvent_CLOUD_CONFIRMATION_STOP_REQ,
} WifiConnEventId_e;

/* Wifi APP Events */
typedef enum
{
    WifiProvStatus_SUCCEED,
    WifiProvStatus_FAILED,
    WifiProvStatus_STOPPED,
} WifiConnProvStatus_e;

/* Wifi Provisioning Modes of operation */
typedef enum
{
    WifiProvMode_OFF,       // Disable Provisioning (default, update using SlWifiConn_enableProvisioning)
    WifiProvMode_ONE_SHOT,  // Enable one cycle of provisioning (disable it upon completion)
    WifiProvMode_ON,        // Enable Provisioning whenever connection can't be established
} WifiConnProvMode_e;

/* SlWifiConn_enableProvisioning flags */
#define SLWIFICONN_PROV_FLAG_EXTERNAL_CONFIRMATION          1
#define SLWIFICONN_PROV_FLAG_FORCE_PROVISIONING             2

/* SlWifiConn_addProfile flags */
#define SLWIFICONN_PROFILE_FLAG_NON_PERSISTENT      1

typedef union {
    uint8_t              provisioningCmd;   // upon WifiConnEvent_PROVISIONING_STARTED
    WifiConnProvStatus_e status;            // upon WifiConnEvent_PROVISIONING_STOPPED
} WifiConnEventData_u;

typedef void (*SlWifiConn_AppEventCB_f)(WifiConnEventId_e eventId , WifiConnEventData_u *pData);


/*
 * Wifi Conn User Settings - parameters can retrieved/updated using the
 * SlWifiConn_getConfiguration / SlWifiConn_setConfiguration API
 */
typedef struct
{
    /* Time to wait for AP connection (WAIT_FOR_CONN).
     * Upon expiration, the device will enter provisioning or go to IDLE.
     *  Default: 3 [seconds]
     *  Note: if static profile is set (i.e. NON_PERSISTENT profile), the connect timeout
     *        will be used first to try the static profile, and upon expiration - the timeout
     *        will be used again while trying to connect with stored profiles */
    uint16_t            connectTimeout;

    /* Time to wait for IP acquisition (WAIT_FOR_IP)
     *  Default: 6 [seconds] */
    uint16_t            ipTimeout;

    /* Time to wait for AP re-connection (WAIT_FOR_CONN)
     * This considers an AP reset and waits till the AP is back on
     * Upon expiration, the device will enter provisioning or go to IDLE.
     *  Default: 60 [seconds] */
    uint16_t            reconnectTimeout;

    /* Time to wait for provsioning complete (PROVISIONING)
     * It assumes slow user response
     *  Default: 300 [seconds] -> 5 minutes
     *  Limitation: must be >= 30 seconds */
    uint16_t            provisioningTimeout;

    /* The IDLE interval is applied when connection can't be established
     * (provisioned AP is not found). The device will stop scanning and go to
     * low power mode. When the configured duration expires, the device will be enabled
     * and go through the connection steps again and back to IDLE in case of failure.
     * The next two value defines the low and high boundaries for a telescopic series
     * with ratio of 2. For example using the defaults the idle intervals will be (in sec):
     *  20, 40, 80, 120, 120....
     *  Default: Min=20 [seconds] -> Max=120 [second] */
    uint16_t            idleMinInterval;
    uint16_t            idleMaxInterval;

    /* Provisioning AP settings (if needed). The following parameters, defines
     * The SSID and Security parameters of the soft AP used during the provisioning.
     * Both the SSID and Password should provided as strings.
     * NULL SSID should be used to keep the existing SSID name.
     * Factory default SSID is based on device MAC address (3 least significant
     * bytes of the MAC address): "mysimplelink-xx:yy:zz".
     * Note that the "Simplelink Starter Pro" example application only supports
     * the default SSID.
     * Only SL_WLAN_SEC_TYPE_OPEN or SL_WLAN_SEC_TYPE_WPA_WPA2 are allowed as
     * security methods.
     * If SL_WLAN_SEC_TYPE_OPEN is used for security type, the ApPassword is not
     * relevant.
     * Note: these setting will override any previous AP configurations.
     *  Default: ApSSID = NULL ("mysimplelink-<macaddr>")
     *           ApSecType = SL_WLAN_SEC_TYPE_OPEN
     *           ApPassword = <n/a> */
    char                *provisioningAP_ssid;
    uint8_t              provisioningAP_secType;
    char                *provisioningAP_password;

    /* Provisioning SC settings (if needed). If public Key is required for security,
     * the the ScKey should point to 16 chars string (e.g. "1234567890123456").
     * Note: these setting will override any previous AP configurations.
     *  Default: NULL [no key] */
    char                *provisioningSC_key;

} WifiConnSettings_t;



/*!
    Debug print callback function prototype.
    This function can be used by the application to register
    a printng method. This will enable the module's log.

    \sa SlNetConn_RegisterDebugCallback()
 */
typedef void (*SlWifiConn_Debug_f)(const char *  _format, ...);

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/

/*!

    \brief Initialize the SlWifiConn resource and trigger the NWP in \n
           station role.

    Called by the application (before or after the interface is added) - \n
    perform software (context and OS resources) initialization.

    \param[in] NetIfCB    This is the callback for the SlNetIf events

    \return               0 upon success or a negative error code

    \note                 To be called before any other SlWifiConn API

    \sa                   SlWifiConn_deinit()

*/
int    SlWifiConn_init(SlWifiConn_AppEventCB_f fWifiAppCB);


/*!

    \brief De-init module and Free all allocated resources (include the SM task)

    Called by the application when Wi-Fi is disabled (no active connection request).
    Once processed, no other SlWifiConn API (other than SlWifiConn_init()) can be called.

    \param[in] None

    \return               0 upon success or a negative error code

    \note                 To be called when Wi-Fi is disabled

    \sa                   SlWifiConn_init()

*/
int SlWifiConn_deinit();

/*!

    \brief An external request to set the WLAN Role

    \param[in] role         the requested role

    \return                 0 upon success or a negative error code

    \note                   can be called only when there is no active enable request
                            or during external provisioning
*/
int SlWifiConn_setRole(SlWlanMode_e role);

/*!

    \brief An external request to reset the Network Processor

    \param[in] none

    \return                 Active WLAN role upon success or a negative error code

    \note                   can be called only when there is no active enable request
                            (i.e. in OFF state)
*/
int SlWifiConn_reset();

/*!

    \brief Set Module Settings

    Called by netIfWifi when connection to AP is not needed.
    Note: Timeout parameters have huge impact on the system. Please do not update
          them unless it necessary.
          Provisioning timeout cannot be less then 30 seconds

    \param[in] pWifiConnSettings    pointer to SlWifiConn configuration structure

    \return                 0 upon success or a negative error code
*/
int    SlWifiConn_setConfiguration(WifiConnSettings_t *pWifiConnSettings);

/*!

    \brief Retrieve Module Settings

    \param[in] pWifiConnSettings    pointer to SlWifiConn configuration structure

    \return                 0 upon success or a negative error code
*/
int    SlWifiConn_getConfiguration(WifiConnSettings_t *pWifiConnSettings);

/*!

    \brief Add a User or External Provisioning Profile

    Called in case of a fixed profile or when external provisioning method detects\n
    a new profile.

    \param[in]      pSsid       SSID of the Access Point
    \param[in]      SSID        SSID Length
    \param[in]      pMacAddr    6 bytes for MAC address
    \param[in]      pSecParams  Security parameters (use NULL key for SL_WLAN_SEC_TYPE_OPEN)\n
                                Security types options:
                                - SL_WLAN_SEC_TYPE_OPEN
                                - SL_WLAN_SEC_TYPE_WEP
                                - SL_WLAN_SEC_TYPE_WEP_SHARED
                                - SL_WLAN_SEC_TYPE_WPA_WPA2
                                - SL_WLAN_SEC_TYPE_WPA2_PLUS
                                - SL_WLAN_SEC_TYPE_WPA3
                                - SL_WLAN_SEC_TYPE_WPA_ENT
                                - SL_WLAN_SEC_TYPE_WPS_PBC
                                - SL_WLAN_SEC_TYPE_WPS_PIN
                                - SL_WLAN_SEC_TYPE_WPA_PMK - insert preprocessed PMK as key

    \param[in]      pSecExtParams  Enterprise parameters - identity, identity length,
                                   Anonymous, Anonymous length, CertIndex (not supported,
                                   certificates need to be placed in a specific file ID),
                                   EapMethod.\n Use NULL in case Enterprise parameters is not in use

    \param[in]      Priority    Profile priority. Lowest priority: 0, Highest priority: 15.
    \param[in]      flags       SLWIFICONN_PROFILE_FLAG_NON_PERSISTENT - do not store the profile
                                                (use sl_WlanConnect ather than sl_WlanProfileAdd)

    \return                 0 upon success or a negative error code
*/
int SlWifiConn_addProfile(const char *pSsid, uint16_t ssidLen, const _u8 *pMacAddr,
                          SlWlanSecParams_t  *pSecParams, SlWlanSecParamsExt_t* pSecExtParams,
                          int prio, uint32_t flags);


/*!

    \brief An Indication of completion of an external cloud provisioning or
    external configuration processing (this will trigger a provisioning stop)

    \return                 0 upon success or a negative error code
*/
int SlWifiConn_externalProvsioningCompleted(void);

/*!

    \brief Add a User or External Provisioning Profile

    Called in case of a fixed profile or when external provisioning method detects\n
    a new profile.

    \param[in] mode             Provisioning mode: OFF, ON or single shot (see WifiConnProvMode_e)
    \param[in] command          The provisioning command to use, one of:
                                     SL_WLAN_PROVISIONING_CMD_START_MODE_APSC,
                                     SL_WLAN_PROVISIONING_CMD_START_MODE_AP,
                                     SL_WLAN_PROVISIONING_CMD_START_MODE_SC,
                                     SL_WLAN_PROVISIONING_CMD_START_MODE_APSC_EXTERNAL_CONFIGURATION
    \param[in] flags            Provisioning optional flags (bit mask of the following):
                                    SLWIFICONN_PROV_FLAG_EXTERNAL_CONFIRMATION - enable external confirmation
                                    SLWIFICONN_PROV_FLAG_FORCE_PROVISIONING - delete all profiles

    \return                     0 upon success or a negative error code
*/
int    SlWifiConn_enableProvisioning(WifiConnProvMode_e mode, uint8_t command,
                                     uint32_t flags);


/*!

    \brief Register an Application Event Callback

    Provide Wi-Fi specific notifications that are not received through SlNetIf,
    e.g. whenever provisioning is started or stopped

    \param[in] none

    \return                 0 upon success or a negative error code
*/
int    SlWifiConn_registerNetIFCallback(SlNetIf_Event_t fNetIfCB, uint16_t ifId);

/*!

    \brief Interface Enable Trigger

    Called by netIfWifi to trigger the connection state machine upon the \n
           first connection request.


    \param[in] none

    \return                 0 upon success or a negative error code
*/
int    SlWifiConn_enable(void);

/*!

    \brief Interface Disable Trigger

    Called by netIfWifi when connection to AP is not needed.

    \param[in] none

    \return                 0 upon success or a negative error code
*/
int    SlWifiConn_disable(void);

/*!
    \brief SlWifiConn_Task callback - Wifi Connection thread, handles all
    Wifi SM events.

    This is the main thread context of the SlWifiConn, it will be used to process
    events and to invoke application's callbacks.

    \param[in] pvParameters      Task parameters (should be kept null).

    The thread (that uses this function) needs to be created by the
    application before SlWifiConn_enable is called.
*/
void *SlWifiConn_process(void* pvParameters);


#if SLWIFICONN_DEBUG_ENABLE
/*!
    \brief register debug print handler

    This function can be used by the application to register
    a debug printing method. This will enable the module's log.

    \param[in] fDebugPrint      Application level debug method.
*/
void SlWifiConn_registerDebugCallback(SlWifiConn_Debug_f fDebugPrint);
#endif


#endif /* SLWIFICONN_H_ */
