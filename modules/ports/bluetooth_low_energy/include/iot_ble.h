/*
 * Amazon FreeRTOS
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file iot_ble.h
 * @brief BLE GAP and GATT API.
 */

#ifndef _AWS_BLE_H_
#define _AWS_BLE_H_

#include <stddef.h>
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "bt_hal_gatt_types.h"
#include "iot_ble_config.h"
#include "iot_linear_containers.h"

/**
 * @ingroup ble_datatypes_structs
 * @brief Structure containing all advertisement parameters.
 *
 * @note: The size of the advertisement message is limited, it is up to the application to make sure everything fits in the advertisement message.
 * For example, with BT 5.0 spec, Advertising 2 UUID of 128 bits, will consume all the advertisement message space (32 bytes).
 * @warning: Blocking API should not be called from a callback.
 */
typedef struct
{
    uint32_t appearance;      /**< Appearance. */
    uint32_t minInterval;     /**< Minimum connection interval. */
    uint32_t maxInterval;     /**< Maximum connection interval. */
    char * pManufacturerData; /**< Manufacturer data */
    char * pServiceData;      /**< Service data */
    BTUuid_t * pUUID1;        /**< First UUID to advertise. */
    BTUuid_t * pUUID2;        /**< Second UUID to advertise. */
    uint16_t manufacturerLen; /**< Length of manufacturer data. */
    uint16_t serviceDataLen;  /**< Service data length */
    bool includeTxPower;      /**< Include Tx Power in advertisement message. */
    bool includeName;         /**< Include device name in advertisement message. */
    bool setScanRsp;          /**< Set to true if the user wishes to set up a scan response instead of an advertisement message. */
} IotBleAdvertisementParams_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief Connection parameters.
 */
typedef struct
{
    uint32_t minInterval; /**< Minimum connection interval. */
    uint32_t maxInterval; /**< Maximum connection interval. */
    uint32_t latency;     /**< Slave latency. */
    uint32_t timeout;     /**< Connection timeout. */
} IotBleConnectionParam_t;


/**
 * @ingroup ble_datatypes_structs
 * @brief Contains the connection info. Return when requested by IotBleGetConnectionInfoList.
 */
typedef struct
{
    IotLink_t connectionList;                 /**< Link to the list of connections. */
    IotBleConnectionParam_t connectionParams; /**< Connection parameters. */
    BTBdaddr_t remoteBdAddr;                  /**< Remote device address. */
    BTSecurityLevel_t securityLevel;          /**< Security level of the connection. */
    IotLink_t clientCharDescrListHead;        /**< Head of CCD list. */
    uint16_t connId;                          /**< Connection Id. */
    bool isBonded;                            /**< True if device is bonded. */
} IotBleConnectionInfoListElement_t;

/**
 * @ingroup ble_datatypes_enums
 * @brief Attribute event type.
 */
typedef enum
{
    eBLERead,                      /**< Read event. */
    eBLEWrite,                     /**< Write event. */
    eBLEWriteNoResponse,           /**< Write event, no response required. */
    eBLEWritePrepare,              /**< Prepare Write event. */
    eBLEExecWrite,                 /**< Execute Write event. */
    eBLEResponseConfirmation,      /**< Confirmation from remote device. */
    eBLEIndicationConfirmReceived, /**< Received confirm to indication from remote device. */
} IotBleAttributeEventType_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief Attribute event type.
 */
typedef struct IotBleAttributeEvent IotBleAttributeEvent_t;


/**
 * @ingroup ble_datatypes_functionpointers
 * @brief Callback called when a request on a attribute is made by the remote device.
 *
 * @param[in] pAttribute Pointer to the attribute being accessed.
 * @param[in] pEventParam Pointer to the event data.
 */
typedef void (* IotBleAttributeEventCallback_t)( IotBleAttributeEvent_t * pEventParam );

/************************************************************/

/**
 * @ingroup ble_datatypes_structs
 * @brief Parameters for read event.
 */
typedef struct
{
    uint16_t connId;            /**< Connection ID. */
    uint32_t transId;           /**< Transaction ID. */
    BTBdaddr_t * pRemoteBdAddr; /**< Remote device address. */
    uint16_t attrHandle;        /**< Param handle. */
    uint16_t offset;            /**< Read offset. */
} IotBleReadEventParams_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief Parameters for write event.
 */
typedef struct
{
    uint32_t transId;           /**< Transaction ID. */
    BTBdaddr_t * pRemoteBdAddr; /**< Remote device address. */
    uint8_t * pValue;           /**< Data to write. */
    uint32_t length;            /**< Data length. */
    uint16_t connId;            /**< Connection ID. */
    uint16_t attrHandle;        /**< Param handle. */
    uint16_t offset;            /**< Write offset. */
    bool needRsp;               /**< Need to respond. */
    bool isPrep;                /**< Set to true if it is a prepare write. */
} IotBleWriteEventParams_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief Parameters for write executed event.
 */
typedef struct
{
    uint32_t transId;           /**< Transaction ID. */
    BTBdaddr_t * pRemoteBdAddr; /**< Remote device address. */
    uint16_t connId;            /**< Connection ID. */
    bool execWrite;             /**< Execute Write command. */
} IotBleExecWriteEventParams_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief Parameters for confirmation response event.
 */
typedef struct
{
    BTStatus_t status; /**< Reported status. */
    uint16_t handle;   /**< Param handle. */
} IotBleRespConfirmEventParams_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief Parameters for indication sent event.
 */
typedef struct
{
    BTAttribute_t * pAttribute; /**< Pointer to attribute being accessed. */
    uint16_t connId;            /**< Connection ID. */
    BTStatus_t status;          /**< Reported status. */
} IotBleIndicationSentEventParams_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief BLE Attribute event.
 */
struct IotBleAttributeEvent
{
    union
    {
        IotBleReadEventParams_t * pParamRead;                     /**< Read event. */
        IotBleWriteEventParams_t * pParamWrite;                   /**< Write event. */
        IotBleExecWriteEventParams_t * pParamExecWrite;           /**< Execute write event. */
        IotBleRespConfirmEventParams_t * pParamRespConfim;        /**<Response confirm event. */
        IotBleIndicationSentEventParams_t * pParamIndicationSent; /**< Indication event. */
    };
    IotBleAttributeEventType_t xEventType;                        /**< Event type (read/write/...). */
};

/**
 * @ingroup ble_datatypes_structs
 * @brief Basic info contained in an attribute.
 * This is common to all attributes.
 */
typedef struct
{
    size_t size;     /**< Size of data field. */
    uint8_t * pData; /**< Attribute data field. */
    BTUuid_t uuid;   /**< Attribute UUID*/
    size_t handle;   /**< Attribute handle. Note, this field is Auto filled(field during BLE_AddService call). */
} IotBleAttributeData_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief BLE Event response.
 */
typedef struct
{
    IotBleAttributeData_t * pAttrData; /**< Pointer to common attribute date. */
    size_t attrDataOffset;             /**< Read/Write index. */
    BTStatus_t eventStatus;            /**< Event status. */
    BTRspErrorStatus_t rspErrorStatus; /**< Response error status. */
} IotBleEventResponse_t;

/**
 * @ingroup ble_datatypes_functionpointers
 * @brief Callback invoked when the MTU for a given connection changes.
 *
 * @param[in] connId Connection ID
 * @param[in] mtu Established MTU size.
 */
typedef void (* IotBle_MtuChangedCallback_t)( uint16_t connId,
                                              uint16_t mtu );

/**
 * @ingroup ble_datatypes_functionpointers
 * @brief Callback indicating that a remote device has connected or been disconnected.
 * @param[in] status Returns eBTStatusSuccess if operation succeeded.
 * @param[in] connId Connection ID.
 * @param[in] connected Flag set to true if device is connected.
 * @param[in] pRemoteBdAddr Remote device address.
 */
typedef void (* IotBle_ConnectionCallback_t)( BTStatus_t status,
                                              uint16_t connId,
                                              bool connected,
                                              BTBdaddr_t * pRemoteBdAddr );

/**
 * @ingroup ble_datatypes_functionpointers
 * @brief  Callback indicating the status of a listen() operation. Invoked on BLE_StartAdv.
 *
 * @param[in] status Returns eBTStatusSuccess if operation succeeded.
 */
typedef void (* IotBle_StartAdvCallback_t)( BTStatus_t status );

/**
 * @ingroup ble_datatypes_functionpointers
 * @brief  Callback invoked on BLE_ConnParameterUpdateRequest from remote device.
 *
 * @param[in] status Returns eBTStatusSuccess if operation succeeded.
 * @param[in] pRemoteBdAddr Address of the Remote device
 * @param[in] pConnectionParam Connection parameters.
 * @param[in] connInterval Established connection interval.
 */
typedef void ( * IotBle_ConnParameterUpdateRequestCallback_t )( BTStatus_t status,
                                                                const BTBdaddr_t * pRemoteBdAddr,
                                                                IotBleConnectionParam_t * pConnectionParam,
                                                                uint32_t connInterval );

/**
 * @ingroup ble_datatypes_functionpointers
 * @brief Callback invoked on BLE_RemoveBond.
 *
 * @param[in] status Returns eBTStatusSuccess if operation succeeded.
 * @param[in] pRemoteBdAddr Address of the bonded device.
 */
typedef void (* IotBleRemoveBondCallback_t)( BTStatus_t status,
                                             const BTBdaddr_t * pRemoteBdAddr );

/**
 * @ingroup ble_datatypes_functionpointers
 * @brief Callback invoked when pairing state is changed.
 *
 * @param[in] status Returns eBTStatusSuccess if operation succeeded.
 * @param[in] pRemoteBdAddr Address of the remote device.
 * @param[in] securityLevel Security level (mode 1, level 1, 2 ,3 ,4).
 * @param[in] reason Reason for failing to authenticate.
 */
typedef void (* IotBle_PairingStateChanged_t)( BTStatus_t status,
                                               BTBdaddr_t * pRemoteBdAddr,
                                               BTSecurityLevel_t securityLevel,
                                               BTAuthFailureReason_t reason );

/**
 * @ingroup ble_datatypes_functionpointers
 * @brief Callback invoked on pairing request from remote device.
 *
 * Numeric comparison event.
 *
 * @param[in] pRemoteBdAddr Address of the remote device.
 */
typedef void ( * IotBle_NumericComparisonCallback_t )( BTBdaddr_t * pRemoteBdAddr,
                                                       uint32_t passKey );

/**
 * @ingroup ble_datatypes_functionpointers
 * @brief Bonded callback, called when a bonded is established or removed.
 * Invoked on bond event or on BLE_RemoveBond.
 *
 * @param[in] status Returns eBTStatusSuccess if operation succeeded.
 * @param[in] pRemoteBdAddr Remote device address.
 * @param[in] isBonded true if address is bonded, false otherwise
 */
typedef void ( * IotBle_BondedCallback_t)( BTStatus_t status,
                                           BTBdaddr_t * pRemoteBdAddr,
                                           bool isBonded );

/**
 * @ingroup ble_datatypes_enums
 * @brief enum listing all the BLE events (not directly triggered by a function call)
 */
typedef enum
{
    eBLEBonded,                             /**< Bonded toggled event */
    eBLEMtuChanged,                         /**< eBLEMtuChanged Event triggering BLEMtuChangedCallback_t. */
    eBLEConnection,                         /**< eBLEConnection Event  triggering BLEConnectionCallback_t. */
    eBLEPairingStateChanged,                /**< eBLEPairingStateChanged Event triggering BLEPairingStateChanged_t. */
    eBLEConnParameterUpdateRequestCallback, /**< eBLEConnParameterUpdateRequestCallback Event triggering BLEConnParameterUpdateRequestCallback_t.  */
    eBLENumericComparisonCallback,          /**< Numeric comparison event. Need callback from the user. */
    eNbEvents,                              /**< eNbEvents  Number of events  */
} IotBleEvents_t;

/**
 * @ingroup ble_datatypes_structs
 * @brief BLE events not directly triggered by a function call.
 * Most of them are triggered by a remote device message.
 */
typedef union
{
    IotBle_BondedCallback_t pBondedCb;                                         /**< Device bonded event. */
    IotBle_MtuChangedCallback_t pMtuChangedCb;                                 /**<  MTU changed event */
    IotBle_ConnectionCallback_t pConnectionCb;                                 /**<  Connection event. */
    IotBle_PairingStateChanged_t pGAPPairingStateChangedCb;                    /**<  Pairing state changed event. */
    IotBle_ConnParameterUpdateRequestCallback_t pConnParameterUpdateRequestCb; /**<  Connection parameter update event. */
    IotBle_NumericComparisonCallback_t pNumericComparisonCb;                   /**<  Pairing request. */
    void * pvPtr;                                                              /**< Used for generic operations. */
} IotBleEventsCallbacks_t;


/**
 * @functionspage{iotble,BLE library,BLE Control and Services Management}
 * - @functionname{iotble_function_init}
 * - @functionname{iotble_function_on}
 * - @functionname{iotble_function_off}
 * - @functionname{iotble_function_startadv}
 * - @functionname{iotble_function_stopadv}
 * - @functionname{iotble_function_connparameterupdaterequest}
 * - @functionname{iotble_function_registereventcb}
 * - @functionname{iotble_function_unregistereventcb}
 * - @functionname{iotble_function_removebond}
 * - @functionname{iotble_function_createservice}
 * - @functionname{iotble_function_deleteservice}
 * - @functionname{iotble_function_sendindication}
 * - @functionname{iotble_function_sendresponse}
 * - @functionname{iotble_function_getconnectioninfolist}
 * - @functionname{iotble_function_getconnectioninfo}
 * - @functionname{iotble_function_confirmnumericcomparisonkeys}
 */

/**
 * @functionpage{IotBle_Init,iotble,init}
 * @functionpage{IotBle_On,iotble,on}
 * @functionpage{IotBle_Off,iotble,off}
 * @functionpage{IotBle_StartAdv,iotble,startadv}
 * @functionpage{IotBle_StopAdv,iotble,stopadv}
 * @functionpage{IotBle_ConnParameterUpdateRequest,iotble,connparameterupdaterequest}
 * @functionpage{IotBle_RegisterEventCb,iotble,registereventcb}
 * @functionpage{IotBle_UnRegisterEventCb,iotble,unregistereventcb}
 * @functionpage{IotBle_RemoveBond,iotble,removebond}
 * @functionpage{IotBle_CreateService,iotble,createservice}
 * @functionpage{IotBle_DeleteService,iotble,deleteservice}
 * @functionpage{IotBle_SendIndication,iotble,sendindication}
 * @functionpage{IotBle_SendResponse,iotble,sendresponse}
 * @functionpage{IotBle_GetConnectionInfoList,iotble,getconnectioninfolist}
 * @functionpage{IotBle_GetConnectionInfo,iotble,getconnectioninfo}
 * @functionpage{IotBle_ConfirmNumericComparisonKeys,iotble,confirmnumericcomparisonkeys}
 */

#if ( IOT_BLE_ADD_CUSTOM_SERVICES == 1 )

/**
 * @cond DOXYGEN_IGNORE
 * @brief that implementation of this function need to be given when IOT_BLE_ADD_CUSTOM_SERVICES is true
 * and when the user needs to add his own services
 */
    void IotBle_AddCustomServicesCb( void );
/** @endcond */
#endif

/**
 * @brief Starting point. Initialize the BLE stack and its services. It is up to the application to decide when BLE should be started.
 *
 * @return Returns eBTStatusSuccess on successful call.
 */
/* @[declare_iotble_init] */
BTStatus_t IotBle_Init( void );
/* @[declare_iotble_init] */

/**
 * @brief Turns on the BLE radio.
 *
 * @return Returns eBTStatusSuccess on successful call.
 */
/* @[declare_iotble_on] */
BTStatus_t IotBle_On( void );
/* @[declare_iotble_on] */

/**
 * @brief Turns off the BLE radio.
 *
 * @return Returns eBTStatusSuccess on successful call.
 */
/* @[declare_iotble_off] */
BTStatus_t IotBle_Off( void );
/* @[declare_iotble_off] */

/**
 * @brief Start advertisements to listen for incoming connections.
 *
 * @return Returns eBTStatusSuccess on successful call.
 */
/* @[declare_iotble_startadv] */
BTStatus_t IotBle_StartAdv( IotBle_StartAdvCallback_t pStartAdvCb );
/* @[declare_iotble_startadv] */

/**
 * @brief Stop advertisements to listen for incoming connections.
 *
 * @return Returns eBTStatusSuccess on successful call.
 */
/* @[declare_iotble_stopadv] */
BTStatus_t IotBle_StopAdv( void );
/* @[declare_iotble_stopadv] */

/**
 * @brief Request an update of the connection parameters.
 *
 * @param[in] pRemoteBdAddr Address of the remote device.
 * @param[in] pConnectionParam New connection parameters.
 * @return Returns eBTStatusSuccess on successful call.
 */
/* @[declare_iotble_connparameterupdaterequest] */
BTStatus_t IotBle_ConnParameterUpdateRequest( const BTBdaddr_t * pRemoteBdAddr,
                                              IotBleConnectionParam_t * pConnectionParam );
/* @[declare_iotble_connparameterupdaterequest] */


/**
 * @brief Used to route event to whatever service requests it.
 *
 * For example, one service could require knowledge of connection status,
 * the it would subscribe to connectionCallback event. That API is giving the
 * flexibility of having more than one service listening to the same event.
 *
 * @param[in] event The event.
 * @param[in] bleEventsCallbacks Callback returning status of the operation.
 * @return Returns eBTStatusSuccess on successful call.
 */
/* @[declare_iotble_registereventcb] */
BTStatus_t IotBle_RegisterEventCb( IotBleEvents_t event,
                                   IotBleEventsCallbacks_t bleEventsCallbacks );
/* @[declare_iotble_registereventcb] */

/**
 * @brief Remove a subscription to an event.
 *
 * @param[in] event The event.
 * @param[in] bleEventsCallbacks The subscription to remove.
 * @return Returns eBTStatusSuccess on successful call.
 */
/* @[declare_iotble_unregistereventcb] */
BTStatus_t IotBle_UnRegisterEventCb( IotBleEvents_t event,
                                     IotBleEventsCallbacks_t bleEventsCallbacks );
/* @[declare_iotble_unregistereventcb] */

/**
 * @brief Remove a bonded device. Will trigger pBondedCb.
 *
 * @param[in] pRemoteBdAddr Address of the bonded device.
 * @return Returns eBTStatusSuccess on successful call
 */
/* @[declare_iotble_removebond] */
BTStatus_t IotBle_RemoveBond( const BTBdaddr_t * pRemoteBdAddr );
/* @[declare_iotble_removebond] */

/**
 * @brief Create a new service.
 *
 * Allocate memory for a new service.
 *
 * @param[in] pService :        Service to create
 * @param[in] pEventsCallbacks:  Call backs for events on each attributes in the service.
 * @return Returns eBTStatusSuccess on successful call.
 */
/* @[declare_iotble_createservice] */
BTStatus_t IotBle_CreateService( BTService_t * pService,
                                 IotBleAttributeEventCallback_t pEventsCallbacks[] );
/* @[declare_iotble_createservice] */

/**
 * @brief Delete a local service.
 *
 * @param[in] pService Pointer to the service to delete.
 * @return Returns eBTStatusSuccess on successful call.
 */
/* @[declare_iotble_deleteservice] */
BTStatus_t IotBle_DeleteService( BTService_t * pService );
/* @[declare_iotble_deleteservice] */

/**
 * @brief Send value indication to a remote device.
 *
 * @param[in] pResp Pointer to the indication/Notification data.
 * @param[in] connId Connection ID.
 * @param[in] confirm Set to true for an indication, false is a notification.
 * @return Returns eBTStatusSuccess on successful call.
 */
/* @[declare_iotble_sendindication] */
BTStatus_t IotBle_SendIndication( IotBleEventResponse_t * pResp,
                                  uint16_t connId,
                                  bool confirm );
/* @[declare_iotble_sendindication] */

/**
 * @brief Send response to the remote device.
 *
 * @param[in] pResp Pointer to the response data.
 * @param[in] connId Connection ID.
 * @param[in] transId Transaction ID.
 * @return Returns eBTStatusSuccess on successful call.
 */
/* @[declare_iotble_sendresponse] */
BTStatus_t IotBle_SendResponse( IotBleEventResponse_t * pResp,
                                uint16_t connId,
                                uint32_t transId );
/* @[declare_iotble_sendresponse] */

/**
 * @brief Send response to the remote device.
 *
 * This function return a pointer to the connection info list.
 * The elements of this list have type:IotBleConnectionInfoListElement_t.
 * Looked into aws_doubly_linked_list.h for information on how to use the linked list.
 *
 * @param[out] pConnectionInfoList Returns the head of the connection list.
 * @return Returns eBTStatusSuccess on successful call.
 */
/* @[declare_iotble_getconnectioninfolist] */
BTStatus_t IotBle_GetConnectionInfoList( IotLink_t ** pConnectionInfoList );
/* @[declare_iotble_getconnectioninfolist] */

/**
 * @brief Get connection info for a specific connection ID.
 *
 * This function return a pointer to the connection info that matches the connection ID.
 *
 * @param[in] connId Connection ID.
 * @param[out] pConnectionInfo Returns a pointer to the connection info that matches the connection ID.
 * @return Returns eBTStatusSuccess on successful call.
 */
/* @[declare_iotble_getconnectioninfo] */
BTStatus_t IotBle_GetConnectionInfo( uint16_t connId,
                                     IotBleConnectionInfoListElement_t ** pConnectionInfo );
/* @[declare_iotble_getconnectioninfo] */

/**
 * @brief Confirm key for numeric comparison.
 *
 * @param[in] pBdAddr address.
 * @param[in] keyAccepted Set to true when numeric comparison is confirmed by the user.
 * @return Returns eBTStatusSuccess on successful call.
 */
/* @[declare_iotble_confirmnumericcomparisonkeys] */
BTStatus_t IotBle_ConfirmNumericComparisonKeys( BTBdaddr_t * pBdAddr,
                                                bool keyAccepted );
/* @[declare_iotble_confirmnumericcomparisonkeys] */

#endif /* _AWS_BLE_H_*/
