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
 * @file aws_BLE.h
 * @brief BLE GAP API.
 */

#ifndef _AWS_BLE_H_
#define _AWS_BLE_H_

#include <stddef.h>
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "bt_hal_gatt_types.h"
#include "aws_doubly_linked_list.h"

/**
 * @brief Structure containing all advertisement parameters.
 *
 * Note: The size of the advertisement message is limited, it is up to the application to make sure everything fits in the advertisement message.
 * For example, with BT 5.0 spec, Advertising 2 UUID of 128 bits, will consume all the advertisement message space (32 bytes).
 *
 */
typedef struct
{
    bool bIncludeTxPower;       /**< Include Tx Power in advertisement message. */
    bool bIncludeName;          /**< Include device name in advertisement message. */
    bool bSetScanRsp;           /**< Set to true if the user wishes to set up a scan response instead of an advertisement message. */
    uint32_t ulAppearance;      /**< Appearance. */
    uint32_t ulMinInterval;     /**< Minimum connection interval. */
    uint32_t ulMaxInterval;     /**< Maximum connection interval. */
    uint16_t usManufacturerLen; /**< Length of manufacturer data. */
    char * pcManufacturerData;  /**< Manufacturer data */
    uint16_t usServiceDataLen;  /**< Service data length */
    char * pcServiceData;	    /**< Service data */
    BTUuid_t * pxUUID1;         /**< First UUID to advertise. */
    BTUuid_t * pxUUID2;         /**< Second UUID to advertise. */
} BLEAdvertismentParams_t;

/**
 * @brief Connection parameters.
 */
typedef struct
{
    uint32_t ulMinInterval; /**< Minimum connection interval. */
    uint32_t ulMaxInterval; /**< Maximum connection interval. */
    uint32_t ulLatency;     /**< Slave latency. */
    uint32_t ulTimeout;     /**< Connection timeout. */
} BLEConnectionParam_t;


/**
 * @brief Contains the connection info. Return when requested by BLE_GetConnectionInfoList.
 */
typedef struct
{
    Link_t xConnectionList;
    BLEConnectionParam_t xConnectionParams; /**< Connection parameters. */
    uint16_t usConnId;                      /**< Connection Id. */
    BTBdaddr_t pxRemoteBdAddr;              /**< Remote device address. */
    bool bIsBonded;                         /**< True if device is bonded. */
    BTSecurityLevel_t xSecurityLevel;       /**< Security level of the connection. */
    Link_t xClientCharDescrListHead;        /**< Head of CCD list. */
} BLEConnectionInfoListElement_t;

/**
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
} BLEAttributeEventType_t;

typedef struct BLEService               BLEService_t;
typedef struct BLECharacteristic        BLECharacteristic_t;
typedef struct BLECharacteristicDescr   BLECharacteristicDescr_t;
typedef struct BLEIncludedService       BLEIncludedService_t;
typedef struct BLEAttribute             BLEAttribute_t;
typedef struct BLEAttributeEvent        BLEAttributeEvent_t;
typedef void                            * BLEAttributeHandle_t;

/**
 *
 * @brief Callback called when a request on a attribute is made by the remote device.
 *
 * @param[in] pxAttribute Pointer to the attribute being accessed.
 * @param[in] pxEventParam Pointer to the event data.
 */
typedef void (* BLEAttributeEventCallback_t)( BLEAttribute_t * pxAttribute,
                                              BLEAttributeEvent_t * pxEventParam );
/************************************************************/

/**
 * @brief Basic info contained in an attribute.
 * This is common to all attributes.
 */
typedef struct
{
    size_t xSize;      /**< Size of data field. */
    uint8_t * pucData; /**< Attribute data field. */
    BTUuid_t xUuid;    /**< Attribute UUID*/
    size_t xHandle;    /**< Attribute handle. Note, this field is Auto filled(field during BLE_AddService call). */
} BLEAttributeData_t;

/**
 * @brief Structure describing a service.
 */
struct BLEService
{
    BLEAttributeData_t xAttributeData;         /**< Service attribute. */
    BLECharacteristic_t * pxCharacteristics;   /**< Pointer to characteristics contained in the service. */
    size_t xNbCharacteristics;                 /**< Number of characteristics. Since the middleware has the knowledge, this is auto filled when BLE_CreateService is called. */
    BLECharacteristicDescr_t * pxDescriptors;  /**< Pointer to descriptors contained in the service. */
    size_t xNbDescriptors;                     /**< Number of descriptors. Since the middleware has the knowledge, this is auto filled when BLE_CreateService is called. */
    BLEIncludedService_t * pxIncludedServices; /**< Pointer to included services contained in the service. */
    size_t xNbIncludedServices;                /**< Number of included services. Since the middleware has the knowledge, this is auto filled when BLE_CreateService is called. */
    BTGattServiceTypes_t xServiceType;         /**< Type of service (primary, secondary). */
    uint8_t ucInstId;                          /**< Instance ID. */
};

/**
 * @brief Structure describing a characteristic.
 */
struct BLECharacteristic
{
    BLEAttributeData_t xAttributeData;                    /**< Characteristic attribute. */
    BLEService_t * pxParentService;                       /**< Pointer to service containing the characteristic.Since the middleware has the knowledge, this is auto filled when BLE_AddService is called. */
    BLECharacteristicDescr_t ** pxDescriptors;            /**< Array of Descriptors contained in that characteristics. */
    size_t xNbDescriptors;                                /**< Number of descriptors contained in that characteristic. */
    BTCharProperties_t xProperties;                       /**< Characteristic properties. */
    BTCharPermissions_t xPermissions;                     /**< Characteristic permissions. */
    BLEAttributeEventCallback_t pxAttributeEventCallback; /**< Callback triggered when attribute is being accessed. */
};

/**
 * @brief Structure describing a characteristic descriptor.
 */
struct BLECharacteristicDescr
{
    BLEAttributeData_t xAttributeData;                    /**< Descriptor attribute. */
    BLEService_t * pxParentService;                       /**< Pointer to service containing the descriptor. Since the middleware has the knowledge, this is auto filled when BLE_AddService is called. */
    BLECharacteristic_t * pxParentCharacteristic;         /**< Pointer to characteristic containing the descriptor. Since the middleware has the knowledge, this is auto filled when BLE_AddService is called. */
    BTCharPermissions_t xPermissions;                     /**< Descriptor permissions. */
    BLEAttributeEventCallback_t pxAttributeEventCallback; /**< Callback triggered when attribute is being accessed. */
};

/**
 * @brief  Structure describing an included service.
 */
struct BLEIncludedService
{
    BLEAttributeData_t xAttributeData;                    /**< Included service attribute. */
    BLEService_t * pxParentService;                       /**< Pointer to service containing the included service. Since the middleware has the knowledge, this is auto filled when BLE_AddService is called. */
    BLEService_t * pxPtrToService;                        /**< Pointer to the service being included. */
    BLEAttributeEventCallback_t pxAttributeEventCallback; /**< Callback triggered when attribute is being accessed. */
};

/**
 * @brief Generic BLE attribute.
 */
struct BLEAttribute
{
    BTDbAttributeType_t xAttributeType; /**< Type of attribute. */
    union
    {
        BLEService_t * pxService;                         /**< Pointer to the relevant attribute structure. */
        BLECharacteristic_t * pxCharacteristic;           /**< Pointer to the relevant attribute structure. */
        BLECharacteristicDescr_t * pxCharacteristicDescr; /**< Pointer to the relevant attribute structure. */
        BLEIncludedService_t * pxIncludedService;         /**< Pointer to the relevant attribute structure. */
        void * pxCharDeclaration;                         /**< Generic pointer for operations. */
    };
};
/************************************************************/

/**
 * @brief Parameters for read event.
 */
typedef struct
{
    uint16_t usConnId;            /**< Connection ID. */
    uint32_t ulTransId;           /**< Transaction ID. */
    BTBdaddr_t * pxRemoteBdAddr;  /**< Remote device address. */
    BLEAttribute_t * pxAttribute; /**< Pointer to attribute being accessed. */
    uint16_t usOffset;            /**< Read offset. */
} BLEReadEventParams_t;

/**
 * @brief .
 */
typedef struct
{
    uint16_t usConnId;            /**< Connection ID. */
    uint32_t ulTransId;           /**< Transaction ID. */
    BTBdaddr_t * pxRemoteBdAddr;  /**< Remote device address. */
    BLEAttribute_t * pxAttribute; /**< Pointer to attribute being accessed. */
    uint16_t usOffset;            /**< Write offset. */
    bool bIsPrep;                 /**< Set to true if it is a prepare write. */
    uint8_t * pucValue;           /**< Data to write. */
    size_t xLength;               /**< Data length. */
} BLEWriteEventParams_t;

/**
 * @brief .
 */
typedef struct
{
    uint16_t usConnId;            /**< Connection ID. */
    uint32_t ulTransId;           /**< Transaction ID. */
    BTBdaddr_t * pxRemoteBdAddr;  /**< Remote device address. */
    BLEAttribute_t * pxAttribute; /**< Pointer to attribute being accessed. */
    bool bExecWrite;              /**< Execute Write command. */
} BLEExecWriteEventParams_t;

/**
 * @brief .
 */
typedef struct
{
    BTStatus_t xStatus;           /**< Reported status. */
    BLEAttribute_t * pxAttribute; /**< Pointer to attribute being accessed. */
} BLERespConfirmEventParams_t;

/**
 * @brief .
 */
typedef struct
{
    uint16_t usConnId;            /**< Connection ID. */
    BTStatus_t xStatus;           /**< Reported status. */
    BLEAttribute_t * pxAttribute; /**< Pointer to attribute being accessed. */
} BLEIndicationSentEventParams_t;

/**
 * @brief .
 */
struct BLEAttributeEvent
{
    BLEAttributeEventType_t xEventType; /**< Event type (read/write/...). */
    union
    {
        BLEReadEventParams_t * pxParamRead;                     /**< Read event. */
        BLEWriteEventParams_t * pxParamWrite;                   /**< Write event. */
        BLEExecWriteEventParams_t * pxParamExecWrite;           /**< Execute write event. */
        BLERespConfirmEventParams_t * pxParamRespConfim;        /**<Response confirm event. */
        BLEIndicationSentEventParams_t * pxParamIndicationSent; /**< Indication event. */
    };
};
/************************************************************/

/**
 * @brief .
 */
typedef struct
{
    BLEAttributeData_t * pxAttrData;    /**< Pointer to common attribute date. */
    size_t xAttrDataOffset;             /**< Read/Write index. */
    BTStatus_t xEventStatus;            /**< Event status. */
    BTRspErrorStatus_t xRspErrorStatus; /**< Response error status. */
} BLEEventResponse_t;
/************************************************************/

/**
 * @brief Callback triggered when a service has been started. Invoked on BLE_StartService.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] pxService Pointer to the service structure.
 */
typedef void (* BLEServiceStartedCallback_t)( BTStatus_t xStatus,
                                              BLEService_t * pxService );

/**
 * @brief Callback triggered when a service has been deleted. Invoked on BLE_DeleteService.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] usAttributeDataHandle Handle of the deleted service.
 */
typedef void (* BLEServiceDeletedCallback_t)( BTStatus_t xStatus,
                                              uint16_t usAttributeDataHandle );

/**
 * @brief Callback triggered when a service has been stopped. Invoked on BLE_StopService.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] pxService Pointer to service structure.
 */
typedef void (* BLEServiceStoppedCallback_t)( BTStatus_t xStatus,
                                              BLEService_t * pxService );


/**
 * @brief Callback invoked when the MTU for a given connection changes.
 *
 * @param[in] usConnId Connection ID
 * @param[in] usMtu Established MTU size.
 */
typedef void (* BLEMtuChangedCallback_t)( uint16_t usConnId,
                                          uint16_t usMtu );

/**
 * @brief Callback indicating that a remote device has connected or been disconnected.
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] usConnId Connection ID.
 * @param[in] bConnected Flag set to true if device is connected.
 * @param[in] pxRemoteBdAddr Remote device address.
 */
typedef void (* BLEConnectionCallback_t)( BTStatus_t xStatus,
                                          uint16_t usConnId,
                                          bool bConnected,
                                          BTBdaddr_t * pxRemoteBdAddr );

/**
 * @brief  Callback indicating the status of a listen() operation. Invoked on BLE_StartAdv.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 */
typedef void (* BLEStartAdvCallback_t)( BTStatus_t xStatus );

/**
 * @brief Callback for set adv callback. Invoked on BLE_SetAdvData.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 */
typedef void (* BLESetAdvDataCallback_t) ( BTStatus_t xStatus );

/**
 * @brief  Callback invoked on BLE_ConnParameterUpdateRequest from remote device.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] pxRemoteBdAddr Address of the Remote device
 * @param[in] pxConnectionParam Connection parameters.
 * @param[in] usConnInterval Established connection interval.
 */
typedef void ( * BLEConnParameterUpdateRequestCallback_t )( BTStatus_t xStatus,
                                                            const BTBdaddr_t * pxRemoteBdAddr,
                                                            BLEConnectionParam_t * pxConnectionParam,
                                                            uint32_t usConnInterval );

/**
 * @brief Callback invoked on BLE_RemoveBond.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] pxRemoteBdAddr Address of the bonded device.
 */
typedef void (* BLERemoveBondCallback_t)( BTStatus_t xStatus,
                                          const BTBdaddr_t * pxRemoteBdAddr );

/**
 * @brief Callback invoked when pairing state is changed.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] pxRemoteBdAddr Address of the remote device.
 * @param[in] xSecurityLevel Security level (mode 1, level 1, 2 ,3 ,4).
 * @param[in] bIsBonded Set to true if the device is also bonded.
 * @param[in] xReason Reason for failing to authenticate.
 */
typedef void (* BLEPairingStateChanged_t)( BTStatus_t xStatus,
                                           BTBdaddr_t * pxRemoteBdAddr,
                                           BTSecurityLevel_t xSecurityLevel,
										   BTAuthFailureReason_t xReason );

/**
 * @brief Callback invoked on pairing request from remote device.
 *
 * Numeric comparison event.
 *
 * @param[in] pxRemoteBdAddr Address of the remote device.
 */
typedef void ( * BLENumericComparisonCallback_t )( BTBdaddr_t * pxRemoteBdAddr, uint32_t ulPassKey );

/**
 * @brief Bonded callback, called when a bonded is established or removed.
 * Invoked on bond event or on BLE_RemoveBond.
 *
 * @param[in] xStatus Returns eBTStatusSuccess if operation succeeded.
 * @param[in] pxRemoteBdAddr Remote device address.
 * @param[in] bIsBonded true if address is bonded, false otherwise
 */
typedef void ( * BLEBondedCallback_t)( BTStatus_t xStatus,
                                       BTBdaddr_t * pxRemoteBdAddr,
                                       bool bIsBonded );

/**
 * @brief enum listing all the BLE events (not directly triggered by a function call)
 */
typedef enum
{
    eBLEBonded,                             /**< Bonded toggled event */
    eBLEMtuChanged,                         /**< eBLEMtuChanged Event triggering BLEMtuChangedCallback_t. */
    eBLEConnection,                         /**< eBLEConnection Event  triggering BLEConnectionCallback_t. */
    eBLEPairingStateChanged,                /**< eBLEPairingStateChanged Event triggering BLEPairingStateChanged_t. */
    eBLEConnParameterUpdateRequestCallback, /**< eBLEConnParameterUpdateRequestCallback Event triggering BLEConnParameterUpdateRequestCallback_t.  */
	eBLENumericComparisonCallback,			/**< Numeric comparison event. Need callback from the user. */
	eNbEvents,                              /**< eNbEvents  Number of events  */
} BLEEvents_t;

/**
 * @ brief BLE events not directly triggered by a function call.
 * Most of them are triggered by a remote device message.
 */
typedef union
{
    BLEBondedCallback_t pxBondedCb;
    BLEMtuChangedCallback_t pxMtuChangedCb;                                 /**<  MTU changed event */
    BLEConnectionCallback_t pxConnectionCb;                                 /**<  Connection event. */
    BLEPairingStateChanged_t pxGAPPairingStateChangedCb;                    /**<  Pairing state changed event. */
    BLEConnParameterUpdateRequestCallback_t pxConnParameterUpdateRequestCb; /**<  Connection parameter update event. */
    BLENumericComparisonCallback_t pxNumericComparisonCb;    				/**<  Pairing request. */
    void * pvPtr;                                                           /**< Used for generic operations. */
} BLEEventsCallbacks_t;


/**
 * @brief Stop advertisements to listen for incoming connections.
 *
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_StopAdv( void );

/**
 * @brief Start advertisements to listen for incoming connections.
 *
 * @param[in] pxStartAdvCb Callback returning status of the operation.
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_StartAdv( BLEStartAdvCallback_t pxStartAdvCb );

/**
 * @brief Set the advertising data or scan response data.
 *
 * @param[in] usAdvertisingEventProperties Type of advertisement.
 * @param[in] pxAdvParams Advertisement parameters.
 * @param[in] pxSetAdvDataCb Callback returning status of the operation.
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_SetAdvData( BTAdvProperties_t usAdvertisingEventProperties,
                           BLEAdvertismentParams_t * pxAdvParams,
                           BLESetAdvDataCallback_t pxSetAdvDataCb );

/**
 * @brief Request an update of the connection parameters.
 *
 * @param[in] pxRemoteBdAddr Address of the remote device.
 * @param[in] pxConnectionParam New connection parameters.
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_ConnParameterUpdateRequest( const BTBdaddr_t * pxRemoteBdAddr,
                                           BLEConnectionParam_t * pxConnectionParam );

/**
 * @brief Starting point. Initialize the BLE stack.
 * Low level function BLE pxEnable is not called. It is up to the application to decide when BLE should be started.
 *
 * @param[in] pxAppUuid APP UUID
 * @param[in] pxProperty Pointer to a property array that the user wishes to set.
 * @param[in] xNbProperties Number of property in the array.
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_Init( BTUuid_t * pxAppUuid,
                     const BTProperty_t * pxProperty,
                     size_t xNbProperties );

/**
 * @brief Used to route event to whatever service requests it.
 *
 * For example, one service could require knowledge of connection status,
 * the it would subscribe to connectionCallback event. That API is giving the
 * flexibility of having more than one service listening to the same event.
 *
 * @param[in] xEvent The event.
 * @param[in] xBLEEventsCallbacks Callback returning status of the operation.
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_RegisterEventCb( BLEEvents_t xEvent,
                                BLEEventsCallbacks_t xBLEEventsCallbacks );

/**
 * @brief Remove a subscription to an event.
 *
 * @param[in] xEvent The event.
 * @param[in] xBLEEventsCallbacks The subscription to remove.
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_UnRegisterEventCb( BLEEvents_t xEvent,
                                  BLEEventsCallbacks_t xBLEEventsCallbacks );

/**
 * @brief Remove a bonded device. Will trigger pxBondedCb.
 *
 * @param[in] pxRemoteBdAddr Address of the bonded device.
 * @param[in] pxBLERemoveBondCb Callback returning status of the operation.
 * @return Returns eBTStatusSuccess on successful call
 */
BTStatus_t BLE_RemoveBond( const BTBdaddr_t * pxRemoteBdAddr );

/**
 * @brief Create a new service.
 *
 * Allocate memory for a new service.
 *
 * @param[out] ppxService Pointer to the memory allocated.
 * @param[in] xNbCharacteristic Number of characteristics in the service.
 * @param[in] xNbDescriptors Number of descriptors in the service.
 * @param[in] xNumDescrsPerChar Array the specify sequentially the number of descriptors per characteristic.
 * @param[in] xNbIncludedServices Number of included services in the service.
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_CreateService( BLEService_t ** ppxService,
                              size_t xNbCharacteristic,
                              size_t xNbDescriptors,
                              size_t xNumDescrsPerChar[],
                              size_t xNbIncludedServices );

/**
 * @brief Add a new service.
 *
 * @param[in] pxService Pointer to the service to add.
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_AddService( BLEService_t * pxService );

/**
 * @brief Start a local service.
 *
 * @param[in] pxService Pointer to the service to start.
 * @param[in] pxCallback Callback returning status of the operation.
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_StartService( BLEService_t * pxService,
                             BLEServiceStartedCallback_t pxCallback );

/**
 * @brief  Stop a local service.
 *
 * @param[in] pxService Pointer to the service to stop.
 * @param[in] pxCallback Callback returning status of the operation.
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_StopService( BLEService_t * pxService,
                            BLEServiceStoppedCallback_t pxCallback );

/**
 * @brief Delete a local service.
 *
 * @param[in] pxService Pointer to the service to delete.
 * @param[in] pxCallback Callback returning status of the operation.
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_DeleteService( BLEService_t * pxService,
                              BLEServiceDeletedCallback_t pxCallback );

/**
 * @brief Send value indication to a remote device.
 *
 * @param[in] pxResp Pointer to the indication/Notification data.
 * @param[in] usConnId Connection ID.
 * @param[in] bConfirm Set to true for an indication, false is a notification.
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_SendIndication( BLEEventResponse_t * pxResp,
                               uint16_t usConnId,
                               bool bConfirm );

/**
 * @brief Send response to the remote device.
 *
 * @param[in] pxResp Pointer to the response data.
 * @param[in] usConnId Connection ID.
 * @param[in] ulTransId Transaction ID.
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_SendResponse( BLEEventResponse_t * pxResp,
                             uint16_t usConnId,
                             uint32_t ulTransId );

/**
 * @brief Send response to the remote device.
 *
 * This function return a pointer to the connection info list.
 * The elements of this list have type:BLEConnectionInfoListElement_t.
 * Looked into aws_doubly_linked_list.h for information on how to use the linked list.
 *
 * @param[out] pxConnectionList Returns the head of the connection list.
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_GetConnectionInfoList( Link_t ** ppxConnectionInfoList );

/**
 * @brief Get connection info for a specific connection ID.
 *
 * This function return a pointer to the connection info that matches the connection ID.
 *
 * @param[in] usConnId Connection ID.
 * @param[out] pxConnectionInfo Returns a pointer to the connection info that matches the connection ID.
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_GetConnectionInfo( uint16_t usConnId,
                                  BLEConnectionInfoListElement_t ** ppxConnectionInfo );

/**
 * @brief Confirm key for numeric comparison.
 *
 * @param[in] pxBdAddr address.
 * @param[in] bKeyAccepted Set to true when numeric comparison is confirmed by the user.
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_ConfirmNumericComparisonKeys( BTBdaddr_t * pxBdAddr, bool bKeyAccepted );

/**
 * Turns on the BLE radio
 *
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_ON( void );

/**
 * Turns of the BLE radio
 *
 * @return Returns eBTStatusSuccess on successful call.
 */
BTStatus_t BLE_OFF( void );

#endif /* _AWS_BLE_H_*/
