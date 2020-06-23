/*
 * FreeRTOS BLE HAL V5.0.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file bt_hal_manager.h
 * @addtogroup HAL_BLUETOOTH
 * @brief BT provides the interfaces to control the Bluetooth device.
 * ,local device control and device discovery functionalities
 *
 * The common generique access profile is the starting point of Bluetooth API.
 * The flow is the following:
 * 1. pxBtManagerInit
 * 2. pxEnable (if wished)
 * 3. pxGetClassicAdapter or/and pxGetLEAdapter to initialize them
 *
 * @{
 */
#ifndef _BT_HAL_MANAGER_H_
#define _BT_HAL_MANAGER_H_

#include <stdint.h>

#include "bt_hal_manager_types.h"

/**
 * @brief  Incompatible API changes without backward compatibility.
 */
#define btMAJOR_VERSION    5

/**
 * @brief Add new functionality with backward compatibility.
 */
#define btMINOR_VERSION    0

/**
 * @brief Make changes in the API with backward compatibility.
 */
#define btPATCH_VERSION    1

/**
 * @brief  Help functions to convert version to string.
 */
#define btSTR_HELPER( x, y, z )    # x "." # y "." # z
#define btSTR( x, y, z )           btSTR_HELPER( x, y, z )

/**
 * @brief  Stringified version number.
 */
#define btSTRING_VERSION                        btSTR( btMAJOR_VERSION, btMINOR_VERSION, btPATCH_VERSION )

/**
 * Stack feature support bit mask
 */
#define btBLE_ADD_BLOB_SERVICE_SUPPORTED_BIT    0x0001 /** Support GATT server database decleration as a blob. */
#define btBLE_ADD_BLE_DYNAMIC_PRIVACY           0x0002 /** Stack can dynamicall enable or disable privacy. */
#define btBLE_BLE_CENTRAL_WHITELISTING          0x0004 /** Provide a mechanism to manage whitelist for Gatt server. */

/**
 * @brief Security Level.
 */
enum
{
    eBTSecLevelNoSecurity = 0x01,             /**< Mode 1 level 1, No security (No authentication and no encryption). */
    eBTSecLevelUnauthenticatedPairing = 0x02, /**< Mode 1 level 2, Unauthenticated pairing with encryption. */
    eBTSecLevelAuthenticatedPairing = 0x03,   /**< Mode 1 level 3, Authenticated pairing with encryption. */
    eBTSecLevelSecureConnect = 0x04,          /**< Mode 1 level 4, Authenticated LE Secure Connections pairing with encryption using a 128-bit strength encryption key. */
};
typedef uint8_t BTSecurityLevel_t;

/**
 * @brief Authentication requirement.
 */
enum
{
    eBTAuthReqBonding = 0x01,       /**< Set bonding request. */
    eBTAuthReqMitm = 0x03,          /**< MITM protection. */
    eBTAuthReqSecureConnect = 0x04, /**< Authenticated encryption. */
    eBTAuthReqKeyPress = 0x05,
};
typedef uint8_t BTAuthReq_t;

/**
 * @brief Preferred physical Transport for GATT connection .
 */
typedef enum
{
    BTTransportAuto,  /**<  BTTransportAuto  */
    BTTransportBredr, /**<  BTTransportBredr */
    BTTransportLe,    /**<  BTTransportLe  */
} BTTransport_t;

/**
 * @brief Bluetooth power State.
 */
typedef enum
{
    eBTstateOff, /**< Off mode */
    eBTstateOn,  /**< On mode */
} BTState_t;

/**
 * @brief Bluetooth PinKey Code, Valid only for BT legacy.
 */
typedef struct
{
    uint8_t ucPin[ 16 ]; /**< PinKey Code. */
} BTPinCode_t;

/**
 * @brief Authentication failure reasons.
 */
typedef enum
{
    eBTauthSuccess,                /**< eBTauthSuccess. */
    eBTauthFailLmpRespTimeout,     /**< eBTauthFailLmpRespTimeout. */
    eBTauthFailKeyMissing,         /**< eBTauthFailKeyMissing. */
    eBTauthFailEncrypMode,         /**< eBTauthFailEncrypMode. */
    eBTauthFailUnitKey,            /**< eBTauthFailUnitKey. */
    eBTauthFailSmpCfrmValue,       /**< eBTauthFailSmpCfrmValue. */
    eBTauthFailSmpEnc,             /**< eBTauthFailSmpEnc. */
    eBTauthFailSmpTimeout,         /**< eBTauthFailSmpTimeout. */
    eBTauthFailPageTimeout,        /**< eBTauthFailPageTimeout. */
    eBTauthFailSmpPasskeyFail,     /**< eBTauthFailSmpPasskeyFail. */
    eBTauthFailSmpOobFail,         /**< eBTauthFailSmpOobFail. */
    eBTauthFailSmpFail,            /**< eBTauthFailSmpFail. */
    eBTauthFailConnTimeout,        /**< eBTauthFailConnTimeout. */
    eBTauthFailSmp,                /**< eBTauthFailSmp. */
    eBTauthFailSmpPairNotSupport,  /**< eBTauthFailSmpPairNotSupport. */
    eBTauthFailSmpUnknownIo,       /**< eBTauthFailSmpUnknownIo. */
    eBTauthFailSmpUnknown,         /**< eBTauthFailSmpUnknown. */
    eBTauthFailHostRejectSecurity, /**< eBTauthFailHostRejectSecurity. */
    eBTauthFailInsuffSecurity,     /**< eBTauthFailInsuffSecurity. */
    eBTauthFailPeerUser,           /**< eBTauthFailPeerUser. */
    eBTauthFailUnspecified,        /**< eBTauthFailUnspecified. */
} BTAuthFailureReason_t;


/**
 * @brief Bluetooth local device and Remote Device property types.
 * Properties common to both adapter and remote device.
 */
typedef enum
{
    /**
     * Description - Bluetooth Device Name
     * Access mode - Adapter name can be GET/SET. Remote device can be GET
     * Data type   - BTBdname_t
     */
    eBTpropertyBdname = 0x1, /**< eBTpropertyBdname */

    /**
     * Description - Bluetooth Device Address
     * Access mode - Only GET.
     * Data type   - BTBdaddr_t
     */
    eBTpropertyBdaddr, /**< eBTpropertyBdaddr */

    /**
     * Description - Device Type - BREDR, BLE or DUAL Mode
     * Access mode - Only GET.
     * Data type   - BTDeviceType_t
     */
    eBTpropertyTypeOfDevice, /**< eBTpropertyTypeOfDevice */

    /**
     * Description - List of bonded devices
     * Access mode - Only GET.
     * Data type   - Array of BTBdaddr_t of the bonded remote devices
     *               (Array size inferred from property length).
     */
    eBTpropertyAdapterBondedDevices, /**< eBTpropertyAdapterBondedDevices */
    /* Properties unique to remote device */

    /**
     * Description - User defined friendly name of the remote device
     * Access mode - GET and SET
     * Data type   - BTBdname_t.
     */
    eBTpropertyRemoteFriendlyName, /**< eBTpropertyRemoteFriendlyName */

    /**
     * Description - RSSI value of the inquired remote device
     * Access mode - Only GET.
     * Data type   - int32_t.
     */
    eBTpropertyRemoteRssi, /**< eBTpropertyRemoteRssi */

    /**
     * Description - Remote version info
     * Access mode - SET/GET.
     * Data type   - BTRemoteVersion_t.
     */
    eBTpropertyRemoteVersionInfo, /**< eBTpropertyRemoteVersionInfo */

    /**
     * Description - MTU size
     * Access mode - SET/GET.
     * Data type   - uint32_t
     */
    eBTpropertyLocalMTUSize, /**< eBTpropertyLocalMTUSize */

    /**
     * Description - Bondable flag
     * Access mode - SET/GET
     * Data type   - bool (true for bondable)
     */
    eBTpropertyBondable, /**< eBTpropertyBondable */

    /**
     * Description - I/O flag.
     *
     * @WARNING: eBTpropertyIO has a dependency on the property eBTpropertySecureConnectionOnly.
     * Only I/O yes/no is compatible with eBTpropertySecureConnectionOnly = 1
     *
     * Access mode - SET/GET
     * Data type   - BTIOtypes_t
     */
    eBTpropertyIO, /**< eBTpropertyIO */

    /**
     * Description - Connectable mode flag
     * Access mode - SET/GET
     * Data type   - bool
     */
    eBTpropertyConnectable, /**< eBTpropertyConnectable Sets connectable mode. */

    /**
     * Description - Secure only mode flag.
     *
     * @WARNING: Secure connections only has a dependency on the property eBTpropertyIO.
     *  Only I/O yes/no is compatible with eBTpropertySecureConnectionOnly = 1
     *
     * Access mode - SET/GET
     * Data type   - bool
     */
    eBTpropertySecureConnectionOnly, /**< eBTpropertySecureOnly Sets secure connection only mode. */
} BTPropertyType_t;

/**
 * @brief Bluetooth Adapter Property data structure.
 */
typedef struct
{
    BTPropertyType_t xType; /**< Property type. */
    size_t xLen;            /**< Property value length. */
    void * pvVal;           /**< Property value. */
} BTProperty_t;

/**
 * @brief Bluetooth Out Of Band data for bonding.
 */
typedef struct
{
    uint8_t ucBleDevAddr[ btADDRESS_LEN ]; /**< LE Bluetooth Device Address */
    uint8_t ucC192[ btKEY_MAX_LEN ];       /**< Simple Pairing Hash C-192 */
    uint8_t ucR192[ btKEY_MAX_LEN ];       /**< Simple Pairing Randomizer R-192 */
    uint8_t ucC256[ btKEY_MAX_LEN ];       /**< Simple Pairing Hash C-256 */
    uint8_t ucR256[ btKEY_MAX_LEN ];       /**< Simple Pairing Randomizer R-256 */
    uint8_t ucSm_tk[ btKEY_MAX_LEN ];      /**< Security Manager TK Value */
    uint8_t ucLe_sc_c[ btKEY_MAX_LEN ];    /**< LE Secure Connections Random Value */
    uint8_t ucLe_sc_r[ btKEY_MAX_LEN ];    /**< LE Secure Connections Random Value */
} BTOutOfBandData_t;

/**
 * @brief Bluetooth Device Type.
 */
typedef enum
{
    eBTdeviceDevtypeBredr = 0x1, /**< Bluetooth classic device only. */
    eBTdeviceDevtypeBle,         /**< BLE device only. */
    eBTdeviceDevtypeDual,        /**< Device with BLE and BT classic. */
} BTDeviceType_t;

/** Bluetooth Bond state */
typedef enum
{
    eBTbondStateNone,
    eBTbondStateBonding,
    eBTbondStateBonded,
} BTBondState_t;

/**
 * @brief Bluetooth SSP Bonding Variant.
 */
typedef enum
{
    eBTsspVariantPasskeyConfirmation, /**< eBTsspVariantPasskeyConfirmation */
    eBTsspVariantPasskeyEntry,        /**< eBTsspVariantPasskeyEntry */
    eBTsspVariantConsent,             /**< eBTsspVariantConsent */
    eBTsspVariantPasskeyNotification, /**< eBTsspVariantPasskeyNotification */
} BTSspVariant_t;

/**
 * @brief Energy info.
 */
typedef struct
{
    uint8_t ucStatus;
    uint8_t ucCtrlState;    /**< stack reported state. */
    uint64_t ullTxTime;     /**< in ms. */
    uint64_t ullRxTime;     /**< in ms. */
    uint64_t ullIdleTime;   /**< in ms. */
    uint64_t ullEnergyUsed; /**< a product of mA, V and ms. */
} BTActivityEnergyInfo;

/**
 * @brief UUID traffic info.
 */
typedef struct
{
    int32_t lAppUid;
    uint64_t ullTxBytes;
    uint64_t ullRxBytes;
} BTUidTraffic_t;

/**
 * @brief Bluetooth state change Callback. Invoked on pxEnable/pxDisable.
 *
 * @param[in] xState Device event, triggered on state change. (switched on or off).
 */
typedef void ( * BTDeviceStateChangedCallback_t )( BTState_t xState );

/**
 * @brief GET/SET local device Properties callback.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] ulNumProperties Number of properties returned.
 * @param[in] pxProperties Array of properties.
 */
typedef void ( * BTDevicePropertiesCallback_t )( BTStatus_t xStatus,
                                                 uint32_t ulNumProperties,
                                                 BTProperty_t * pxProperties );

/**
 * @brief GET/SET Remote Device Properties callback.
 * Invoked on pxGetRemoteDeviceProperty, pxSetRemoteDeviceProperty, pxGetAllRemoteDeviceProperties.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] pxBdAddr Address of the Remote device.
 * @param[in] ulNumProperties Number of properties returned.
 * @param[in] pxProperties Array of properties.
 */
typedef void ( * BTRemoteDevicePropertiesCallback_t )( BTStatus_t xStatus,
                                                       BTBdaddr_t * pxBdAddr,
                                                       uint32_t ulNumProperties,
                                                       BTProperty_t * pxProperties );

/**
 * @brief Callback Invoked on Pin Request.
 *
 * @param[in] pxRemoteBdAddr remote Device address
 * @param[in] pxBdName remote Device name
 * @param[in] ulCod cod
 * @param[in] ucMin16Digit pin
 */
/** Bluetooth Legacy PinKey Request callback */
typedef void (* BTPinRequestCallback_t)( BTBdaddr_t * pxRemoteBdAddr,
                                         BTBdname_t * pxBdName,
                                         uint32_t ulCod,
                                         uint8_t ucMin16Digit );


/**
 * @brief Callback Invoked on SSP event.
 *
 * Bluetooth SSP Request callback - Just Works & Numeric Comparison
 * pass_key - Shall be 0 for eBTSspPairingVariantConsent &
 * eBTSspPairingPasskeyEntry.
 *
 *
 * @param[in] pxRemoteBdAddr Remote device Address.
 * @param[in] pxRemoteBdName Remote device Name (NULL if not available.
 * @param[in] ulCod Class Of Device.
 * @param[in] xPairingVariant Paring variant.
 * @param[in] ulPassKey Pass Key.
 */
typedef void ( * BTSspRequestCallback_t )( BTBdaddr_t * pxRemoteBdAddr,
                                           BTBdname_t * pxRemoteBdName,
                                           uint32_t ulCod,
                                           BTSspVariant_t xPairingVariant,
                                           uint32_t ulPassKey );

/**
 * @brief Bluetooth pairing state changed event callback.
 * Invoked in response to create_bond, cancel_bond or remove_bond
 * reason field is valid only in case of xStatus == eBTStatusFail.
 * Invoked on pairing event.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] pxRemoteBdAddr Remote device Address.
 * @param[in] xState Bonded state value.
 * @param[in] xReason Authentication failure status.
 * @param[in] xSecurityLevel Security level (mode 1, level 1, 2 ,3 ,4).
 */
typedef void ( * BTPairingStateChangedCallback_t )( BTStatus_t xStatus,
                                                    BTBdaddr_t * pxRemoteBdAddr,
                                                    BTBondState_t xState,
                                                    BTSecurityLevel_t xSecurityLevel,
                                                    BTAuthFailureReason_t xReason );

/**
 * @brief Callback invoked on pxReadEnergyInfo. Invoked on pxReadEnergyInfo.
 *
 * Ctrl_state-Current controller state-Active-1,scan-2,or idle-3 state as defined by HCI spec.
 * If the ctrl_state value is 0, it means the API call failed
 * Time values-In milliseconds as returned by the controller
 * Energy used-Value as returned by the controller
 * Status-Provides the status of the read_energy_info API call
 * uid_data provides an array of BTUidTraffic_t, where the array is terminated by an
 * element withapp_uid set to -1.
 *
 * @param[in] pxEnergyInfo Energy information.
 * @param[in] pxUidData UID data traffic.
 */
typedef void ( * BTEnergyInfoCallback_t )( BTActivityEnergyInfo * pxEnergyInfo,
                                           BTUidTraffic_t * pxUidData );

/**
 * @brief Bluetooth Test Mode Callback. Invoked when remote device uses pxDutModeSend.
 *
 * @param[in] usOpcode
 * @param[in] pucBuf
 * @param[in] xLen
 */
/* Receive any HCI event from controller. Must be in DUT Mode for this callback to be received */
typedef void ( * BTDutModeRecvCallback_t )( uint16_t usOpcode,
                                            uint8_t * pucBuf,
                                            size_t xLen );

/*
 * This callback shall be invoked whenever the le_tx_test, le_rx_test or le_test_end is invoked
 * The num_packets is valid only for le_test_end command */

/**
 * @brief LE Test mode callbacks. Invoked on pxLeTestMode.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] usNumPackets
 */
typedef void ( * BTLeTestModeCallback_t )( BTStatus_t xStatus,
                                           uint16_t usNumPackets );

/**
 * @brief Read RSSI Callback. Invoked on pxReadRssi.
 *
 * @param[in] pxBda Remote device address.
 * @param[in] ulRssi
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 */
typedef void ( * BTReadRssiCallback_t )( BTBdaddr_t * pxBda,
                                         uint32_t ulRssi,
                                         BTStatus_t xStatus );

/**
 * @brief Security request callback. Invoked on master from remote slave security request event.
 *
 * @param[in] pxBda Remote device address.
 * @param[in] xSecurityLevel Request security
 * @param[in] bBonding True is bonding is requested
 */
typedef void ( * BTSlaveSecurityRequestCallBack_t )( BTBdaddr_t * pxBda,
                                                     BTSecurityLevel_t xSecurityLevel,
                                                     bool bBonding );

/**
 * @brief TX power  Callback. Invoked on pxGetTxpower.
 *
 * @param[in] pxBda Remote device address.
 * @param[in] xTransport Specify if BLE and BT classic is being used.
 * @param[in] ucTxPowerValue Tx Power value.
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 */
typedef void ( * BTTxPowerCallback_t )( BTBdaddr_t * pxBda,
                                        BTTransport_t xTransport,
                                        uint8_t ucTxPowerValue,
                                        BTStatus_t xStatus );

/**
 * @brief Bonded callback, called when a bonded is established or removed.
 * Invoked on bond event or on pxRemoveBond.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] pxRemoteBdAddr Remote device address.
 * @param[in] bIsBonded true if address is bonded, false otherwise
 */
typedef void ( * BTBondedCallback_t)( BTStatus_t xStatus,
                                      BTBdaddr_t * pxRemoteBdAddr,
                                      bool bIsBonded );

/**
 * @brief Bluetooth DM callback structure.
 */
typedef struct
{
    /** set to sizeof(BTCallbacks_t) */
    BTDeviceStateChangedCallback_t pxDeviceStateChangedCb;
    BTDevicePropertiesCallback_t pxAdapterPropertiesCb;
    BTRemoteDevicePropertiesCallback_t pxRemoteDevicePropertiesCb;
    BTPinRequestCallback_t pxPinRequestCb;
    BTSspRequestCallback_t pxSspRequestCb;
    BTPairingStateChangedCallback_t pxPairingStateChangedCb;
    BTBondedCallback_t pxBondedCb; /** This is deprecated */
    BTDutModeRecvCallback_t pxDutModeRecvCb;
    BTLeTestModeCallback_t pxleTestModeCb;
    BTEnergyInfoCallback_t pxEnergyInfoCb;
    BTReadRssiCallback_t pxReadRssiCb;
    BTTxPowerCallback_t pxTxPowerCb;
    BTSlaveSecurityRequestCallBack_t pxSlaveSecurityRequestCb;
} BTCallbacks_t;

/**
 * @brief Represents the standard SAL device management interface.
 */
typedef struct
{
    /**
     * @brief Initializes the interface and provides callback routines.
     *
     * @param[in] pxCallbacks Initializer for common GAP callbacks.
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxBtManagerInit )( const BTCallbacks_t * pxCallbacks );

    /**
     * @brief Free up the memory
     *
     * @Warning: Note that similarly to pxBtManagerInit. Memory of adapter of server/client needs to be freed first (by calling pxUnregisterBleApp for example)
     *
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxBtManagerCleanup )( void );

    /**
     * @brief Enables Bluetooth module.
     *
     * Trigger BTDeviceStateChangedCallback_t.
     *
     * @param[in] ucGuestMode True if Bluetooth needs to be enabled in guest mode else FALSE.
     * @return  eBTStatusSuccess if BT enable permitted. Else error code.
     */
    BTStatus_t ( * pxEnable )( uint8_t ucGuestMode );

    /**
     * @brief Disables Bluetooth module.
     *
     * Trigger BTDeviceStateChangedCallback_t.
     *
     * @return eBTStatusSuccess if BT disable permitted. Else error code.
     */
    BTStatus_t ( * pxDisable )();

    /**
     * @brief Retrieves all properties of local device.
     *
     * Triggers BTDevicePropertiesCallback_t.
     *
     * @return eBTStatusSuccess if the operation is successful, else error code.
     * When properties are read, they will be returned as part of
     * pxAdapterPropertiesCb()
     */
    BTStatus_t ( * pxGetAllDeviceProperties )();

    /**
     * @brief Get Bluetooth Adapter property of 'type'.
     *
     * Triggers BTDevicePropertiesCallback_t.
     *
     * @param[in] xType Property type.
     * @return eBTStatusSuccess if the operation is successful, else error code.
     * When property is read, it  will be returned as part of
     * pxAdapterPropertiesCb()
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxGetDeviceProperty )( BTPropertyType_t xType );

    /**
     * @brief Sets Bluetooth Adapter property of 'type'.Based on the type, val shall be one of
     * BTBdaddr_t or BTBdname_t or BTScanmode_t etc.
     *
     * Triggers BTDevicePropertiesCallback_t.
     *
     * @param[in] pxProperty Property containing BTProperty_t structure.
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxSetDeviceProperty )( const BTProperty_t * pxProperty );

    /**
     * @brief Retrieves all properties of a remote device.
     *
     * Triggers BTRemoteDevicePropertiesCallback_t.
     *
     * @param[in] pxRemoteAddr BT Address of remote device
     * @return eBTStatusSuccess if the operation is successful, else error code.
     * When properties are read, they will be returned as part of
     * pxAdapterPropertiesCb()
     */
    BTStatus_t ( * pxGetAllRemoteDeviceProperties )( BTBdaddr_t * pxRemoteAddr );

    /**
     * @brief Get Bluetooth  property of 'type' associated with a remote device.
     *
     * Triggers BTRemoteDevicePropertiesCallback_t.
     *
     * @param[in] pxRemoteAddr BT Address of remote device.
     * @param[in] xType Property type.
     * @return eBTStatusSuccess if the operation is successful, else error code.
     * When property is read, it  will be returned as part of
     * pxRemoteDevicePropertiesCb()
     */
    BTStatus_t ( * pxGetRemoteDeviceProperty )( BTBdaddr_t * pxRemoteAddr,
                                                BTPropertyType_t xType );

    /**
     * @brief Sets Bluetooth  property of 'type'.Based on the type, to a remote device.
     *
     * Triggers BTRemoteDevicePropertiesCallback_t.
     *
     * @param[in] pxRemoteAddr BT Address of remote device.
     * @param[in] pxProperty Property to be set on remote device.
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxSetRemoteDeviceProperty )( BTBdaddr_t * pxRemoteAddr,
                                                const BTProperty_t * pxProperty );

    /**
     * @brief Initiates Bluetooth pairing to a remote device.
     *
     * Triggers BTPairingStateChangedCallback_t on both central and peripheral.
     *
     * @param[in] pxBdAddr BT Address of remote device.
     * @param[in] xTransport Transport for bonding, one of the enumeration value from       BTTransport_t.
     * @param[in] bCreateBond set this flag to TRUE if device needs to be bonded.
     *                        (Remember link authentication information)
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxPair )( const BTBdaddr_t * pxBdAddr,
                             BTTransport_t xTransport,
                             bool bCreateBond );

    /**
     * @brief Sets out of band data for a device for pairing.
     *
     * Triggers BTPairingStateChangedCallback_t.
     *
     * @param[in] pxBdAddr BT Address of remote device.
     * @param[in] xTransport Specify if BLE and BT classic is being used.
     * @param[in] pxOobData Out of band data
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxCreateBondOutOfBand )( const BTBdaddr_t * pxBdAddr,
                                            BTTransport_t xTransport,
                                            const BTOutOfBandData_t * pxOobData );

    /**
     * @brief Request authentication from the master.
     *
     * Triggers BTSlaveSecurityRequestCallBack_t and BTPairingStateChangedCallback_t if request is accepted.
     *
     * @param[in] pxBda Remote device address.
     * @param[in] xSecurityLevel Request security
     * @param[in] bBonding True is bonding is requested
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxSendSlaveSecurityRequest )( const BTBdaddr_t * pxBdAddr,
                                                 BTSecurityLevel_t xSecurityLevel,
                                                 bool bBonding );

    /* TODO:Handle out of band pairing in better way */

    /**
     * @brief Cancels an on going bonding procedure for the given device.
     *
     * Triggers BTPairingStateChangedCallback_t.
     *
     * @param[in] pxBdAddr BT Address of device getting bonded.
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxCancelBond )( const BTBdaddr_t * pxBdAddr );

    /**
     * @brief Removes the bonding with a device. If there is an active connection with the device,
     * connection will be disconnected.
     *
     * Will invoke pxBondedCb.
     *
     * @param[in] pxBdAddr BT Address of device that needs to be unbonded.
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxRemoveBond )( const BTBdaddr_t * pxBdAddr );
    /*!!TODO need an enum for connection state */

    /**
     * @brief Retrieves connection status for a device.
     *
     * @param[in] pxBdAddr BT Address of device.
     * @param[in] bConnectionState true if connected false if not connected
     *
     * @return Fail if there is no connection.
     */
    BTStatus_t ( * pxGetConnectionState )( const BTBdaddr_t * pxBdAddr,
                                           bool * bConnectionState );

    /**
     * @brief BT Legacy PinKey Reply.
     *
     * @param[in] pxBdAddr BT Address of device.
     * @param[in] ucAccept TRUE is pairing request is accepted, else FALSE.
     * @param[in] ucPinLen length of the pin code.
     * @param[in] pxPinCode Pin code.
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxPinReply )( const BTBdaddr_t * pxBdAddr,
                                 uint8_t ucAccept,
                                 uint8_t ucPinLen,
                                 BTPinCode_t * pxPinCode );

    /**
     * @brief BT SSP Reply - Just Works, Numeric Comparison and Passkey
     *
     * @param[in] pxBdAddr BT Address of device.
     * @param[in] xVariant Pairing variant.
     * @param[in] ucAccept TRUE is pairing request is accepted, else FALSE.
     *                  When accept = FALSE,passkey shall be zero
     * @param[in] ulPasskey : passkey valid only when variant is BTSspVariantPasskeyEntry and
     *                   BTSspVariantPasskeyNotification
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxSspReply )( const BTBdaddr_t * pxBdAddr,
                                 BTSspVariant_t xVariant,
                                 uint8_t ucAccept,
                                 uint32_t ulPasskey );

    /**
     * @brief Read Energy info details.
     *
     * Triggers BTEnergyInfoCallback_t.
     *
     * @return  value indicates eBTStatusSuccess or eBTStatusNotReady, Success indicates
     *          that the VSC command was sent to controller
     */
    BTStatus_t ( * pxReadEnergyInfo )();

    /**
     * Enables/disables local device to DUT mode.
     *
     * @param[in] bEnable TRUE if DUT mode needs to be enabled, else FALSE.
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxDutModeConfigure )( bool bEnable );

    /**
     * @brief Send any test HCI (vendor-specific) command to the controller. Must be in DUT Mode.
     *
     * Triggers BTDutModeRecvCallback_t on remote device.
     *
     * @param[in] usOpcode HCI VSC opcode.
     * @param[in] pucBuf Parameters associated with the command represented in byte buffer.
     * @param[in] xLen Total length of parameter buffer.
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxDutModeSend )( uint16_t usOpcode,
                                    uint8_t * pucBuf,
                                    size_t xLen );

    /**
     * @brief Send LE test command (LE_Receiver_Test, LE_Transmitter_Test, LE_Test_End )
     *   command to the controller.
     *
     * Triggers BTLeTestModeCallback_t.
     *
     * @param[in] usOpcode, LE test VSC opcode.
     * @param[in] pucBuf Parameters associated with the command represented in byte buffer.
     * @param[in] xLen Total length of parameter buffer.
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxLeTestMode )( uint16_t usOpcode,
                                   uint8_t * pucBuf,
                                   size_t xLen );

    /**
     * @brief Enables/disables capturing of HCI (snoop) logs.
     *
     * @param[in] bEnable TRUE if snoop logs needs to be enabled, else FALSE
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxConfigHCISnoopLog )( bool bEnable );

    /**
     * @brief Clears the stack configuration cache. Equivalent to BT layer Factory reset.
     *
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxConfigClear )();

    /**
     * @brief Retrieves RSSI  of a remote device
     *
     * Triggers BTReadRssiCallback_t.
     *
     * @param[in] pxBdAddr BT Address of remote device
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxReadRssi )( const BTBdaddr_t * pxBdAddr );

    /**
     * @brief Retrieves TX power of a remote device.
     *
     * Triggers BTTxPowerCallback_t.
     *
     * @param[in] pxBdAddr BT Address of remote device.
     * @param[in] xTransport LE / Classic.
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxGetTxpower )( const BTBdaddr_t * pxBdAddr,
                                   BTTransport_t xTransport );

    /**
     * @brief Retrieves the Classic interface.
     *
     * @return void* pointer that points to the profile interface itself. This needs to be
     *               type-casted to the Classic interface by the caller
     */
    const void * ( *pxGetClassicAdapter )( );

    /**
     * @brief Retrieves the HAL LE interface.
     * @return void* pointer that points to the BLE adapter interface itself. This needs to be
     *               type-casted to the LE interface by the caller
     */
    const void * ( *pxGetLeAdapter )( );

    /**
     * @brief Retrieves last error number from the stack.
     * @return error no of the last operation.
     */
    uint32_t ( * pxGetLastError )();

    /**
     *
     * @brief get stack features supported.
     *
     * @param[out] pulFeatureMask feature mask
     * @return Returns eBTStatusSuccess on successful call.
     */
    BTStatus_t ( * pxGetStackFeaturesSupport )( uint32_t * pulFeatureMask );
} BTInterface_t;

const BTInterface_t * BTGetBluetoothInterface();

#endif /* _BT_HAL_MANAGER_H_ */
/** @} */
