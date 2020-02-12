/*
 * FreeRTOS BLE HAL V4.0.1
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
 * @file bt_hal_manager_adapter_classic.h
 * @addtogroup HAL_BLUETOOTH
 * @brief BT Classic Adapter provides the interfaces to control Bluetooth classic (BR/EDR)
 * functionalities of local device control and device discovery functionalities
 * USAGE
 * -----
 *
 * @{
 */

#ifndef _BT_HAL_MANAGER_ADAPTER_CLASSIC_
#define _BT_HAL_MANAGER_ADAPTER_CLASSIC_

#include "bt_hal_manager.h"

/** Bluetooth Adapter Visibility Modes*/
typedef enum
{
    eBTScanModeNone,
    eBTScanModeConnectable,
    eBTScanModeConnectableDiscoverable
} BTScanMode_t;


/* Bluetooth scan params * / */
typedef enum
{
    eBTPageScanInterval220ms,
    eBTPageScanInterval1280ms
} BTScanParam_t;


/** Bluetooth Adapter Discovery state */
typedef enum
{
    eBTDiscoveryStopped,
    eBTDiscoveryStarted
} BTDiscoveryState_t;

/** Bluetooth ACL connection state */
typedef enum
{
    eBTaclStateConnected,
    eBTaclStateDisconnected
} BTaclState_t;


/** Bluetooth SDP service record */
typedef struct
{
    BTUuid_t xUuid;
    uint16_t usChannel;
    char * cName; /* what's the maximum length */
} BTServiceRecord_t;

/* manufacturer information EIR data */
typedef struct
{
    uint32_t ulVvendor;
    uint32_t ulProduct;
} BTEirManfInfo_t;

/* gadget manufacturer information EIR data */
typedef struct
{
    uint32_t ulVendor;
    uint32_t ulProduct;
    uint32_t ulUuid;
} BTGadgetEirManfInfo_t;

/** Bluetooth Scan types */
typedef enum
{
    eBTScanTypeStandard,
    eBTScanTypeInterlaced
} BTScanType_t;

/* Bluetooth local device and Remote Device property types */
typedef enum
{
    /**
     * Description - Bluetooth Device Name
     * Access mode - Adapter name can be GET/SET. Remote device can be GET
     * Data type   - BTBdname_t
     */
    eBTPropertyBdname = 0x1,

    /**
     * Description - Bluetooth Device Address
     * Access mode - Only GET.
     * Data type   - BTBdaddr_t
     */
    eBTPropertyBdaddr,

    /**
     * Description - Device Type - BREDR, BLE or DUAL Mode
     * Access mode - Only GET.
     * Data type   - BTDeviceType_t.
     */
    eBTPropertyTypeOfDevice,

    /**
     * Description - Bluetooth Service 128-bit UUIDs
     * Access mode - Only GET.
     * Data type   - Array of BTUuid_t (Array size inferred from property length).
     */
    eBTPropertyUUIDS,

    /**
     * Description - Bluetooth Class of Device as found in Assigned Numbers
     * Access mode - Only GET.
     * Data type   - uint32_t.
     */
    eBTPropertyClassOfDevice,

    /**
     * Description - Bluetooth Service Record
     * Access mode - Only GET.
     * Data type   - BTServiceRecord_t
     */
    eBTPropertyServiceRecord,

    /**
     * Description - Bluetooth Adapter Discovery timeout (in seconds)
     * Access mode - GET and SET
     * Data type   - uint32_t
     */
    eBTPropertyAdapterDiscoveryTimeout,
    /* Properties unique to adapter */

    /**
     * Description - Bluetooth Adapter scan mode
     * Access mode - GET and SET
     * Data type   - BTScanMode_t.
     */
    eBTPropertyAdapterScanMode,
    /* Properties unique to manufacturer and device */

    /**
     * Description - Manufacturer specific info on EIR
     * Access mode - GET
     * Data type   - BTEirManfInfo_t.
     */
    eBTPropertyEIRManfInfo = 0xFE,
    /* Properties unique to remove  */

    /**
     * Description - Bluetooth Adapter scan parameters
     * Access mode - SET
     * Data type   - BTScanParam_t.
     */
    eBTPropertyAdapterScanParam = 0x100,

    /**
     * Description - Bluetooth Adapter's connectability
     * Access mode - GET and SET
     * Data type   - 2 * uint32_t
     */
    eBTPropertyConnectability = 0x101,

    /**
     * Description - Bluetooth Adapter's scan type (Standard or Interlaced)
     * Access mode - GET and SET
     * Data type   - BTScanType_t
     */
    eBTPropertyScanType = 0x102,

    /**
     * Description - Manufacturer gadget specific info on EIR
     * Access mode - GET
     * Data type   - BTGadgetEirManfInfo_t.
     */
    eBTPropertyGadgetEIRManfInfo = 0x103,
} BTClassicPropertyType_t;

/** Bluetooth Adapter Property data structure */
typedef struct
{
    BTClassicPropertyType_t xType;
    size_t xLen;
    void * pvVal;
} BTClassicProperty_t;


/** GET/SET local device Properties callback */
typedef void (* BTClassicDevicePropertiesCallback_t)( BTStatus_t xStatus,
                                                      uint32_t ulNumProperties,
                                                      BTClassicProperty_t * pxProperties );

/** GET/SET Remote Device Properties callback */

/** TODO: For remote device properties, do not see a need to get/set
 * multiple properties - num_properties shall be 1
 */
typedef void ( * BTClassicRemoteDevicePropertiesCallback_t )( BTStatus_t xStatus,
                                                              BTBdaddr_t * pxBdAddr,
                                                              uint32_t ulNumProperties,
                                                              BTClassicProperty_t * pxProperties );

/** New device discovered callback */

/** If EIR data is not present, then BD_NAME and RSSI shall be NULL and -1
 * respectively */
typedef void (* BTDeviceFoundCallback_t)( uint32_t ulNumProperties,
                                          BTClassicProperty_t * pxProperties );

/** Discovery state changed callback */
typedef void (* BTDiscoveryStateChangedCallback_t)( BTDiscoveryState_t xState );

/** Bluetooth ACL connection state changed callback */
typedef void (* BTaclStateChangedCallback_t)( BTStatus_t xStatus,
                                              BTBdaddr_t * pxRemoteBdAddr,
                                              BTaclState_t xState );

/** Bluetooth DM callback structure. */
typedef struct
{
    BTClassicDevicePropertiesCallback_t pxDevProperties_cb;
    BTClassicRemoteDevicePropertiesCallback_t pxRemoteDevProperties_cb;
    BTDeviceFoundCallback_t pxDeviceFound_cb;
    BTDiscoveryStateChangedCallback_t pxDiscoveryStateChanged_cb;
    BTaclStateChangedCallback_t pxAclStateChanged_cb;
} BTClassicCallbacks_t;


/** Represents the standard SAL BT Classic device management interface. */
typedef struct
{
    /**
     * Initializes SAL classic BT adapter.
     *
     * @param callbacks, BTCallbacks_t callbacks.
     * @return  eBTStatusSuccess if init is successful. Or any other error code
     */
    BTStatus_t ( * pxInit )( BTClassicCallbacks_t * pxCallbacks );

    /**
     * Cleans to SAL classic Bluetooth adapter
     *
     * @param None
     * @return  eBTStatusSuccess if cleanup is successful
     */
    BTStatus_t ( * pxCleanup )();

    /** Get Bluetooth Classic Adapter property of 'type'
     *
     * @param property type
     * @return eBTStatusSuccess if the operation is successful, else error code.
     * When property is read, it  will be returned as part of
     * BTClassic_device_properties_callback()
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxGetClassicDeviceProperty )( BTClassicPropertyType_t xType );

    /** Sets Bluetooth classic Adapter property of 'type'.Based on the type, val shall be one of
     * BTScanmode_t, BTScanType_t etc
     *
     * @param property containing BTClassicProperty_t structure
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxSetClassicDeviceProperty )( const BTClassicProperty_t * pxProperty );

    /**
     * Retrieves all properties of a remote device
     *
     * @param : BTbdaddr_t :BT Address of remote device
     * @return eBTStatusSuccess if the operation is successful, else error code.
     * When properties are read, they will be returned as part of
     * BTClassicRemoteDevicePropertiesCallback()
     */
    BTStatus_t ( * pxGetAllRemoteClassicDeviceProperties )( BTBdaddr_t * pxRemote_addr );

    /** Get Bluetooth  property of 'type' associated with a remote device
     *
     * @param : BTbdaddr_t :BT Address of remote device
     * @param: type, Property type
     * @return eBTStatusSuccess if the operation is successful, else error code.
     * When property is read, it  will be returned as part of
     * BTClassicRemoteDevicePropertiesCallback()
     */
    BTStatus_t ( * pxGetRemoteClassicDeviceProperty )( BTBdaddr_t * pxRemote_addr,
                                                       BTClassicPropertyType_t xType );

    /** Sets Bluetooth  property of 'type'.Based on the type, to a remote device
     * @param : BTbdaddr_t :BT Address of remote device
     * @param: property :  Property to be set on remote device
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxSetRemoteClassicDeviceProperty )( BTBdaddr_t * pxRemote_addr,
                                                       const BTClassicProperty_t * pxProperty );

    /** Get Remote Device's service record  for the given UUID
     * @param : BTbdaddr_t :BT Address of remote device
     * @param: uuid : UUID of the service.
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxGetRemoteServiceRecord )( BTBdaddr_t * pxRemote_addr,
                                               BTUuid_t * pxUuid );

    /** Starts service search on a given Bluetooth device
     * @param : BTbdaddr_t :BT Address of remote device
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxGetRemoteService )( BTBdaddr_t * pxRemote_addr );

    /** Starts device discovery for nearby devices
     * @param : None
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxStartDiscovery )();

    /** Stops an ongoing device discovery
     * @param : None
     * @return eBTStatusSuccess if the operation is successful, else error code.
     */
    BTStatus_t ( * pxStopDiscovery )();

    /** Retrieves the HAL profile interface.
     * @param BTProfile_t profile needed
     * @return void* pointer that points to the profile interface itself. This needs to be
     *               type-casted to the right profile interface by the caller
     */
    const void * ( *ppvGetProfileInterface )( BTProfile_t xProfile );
} BTClassicInterface_t;

extern const BTClassicInterface_t * BT_GetClassicAdapter();

#endif /* _BT_HAL_MANAGER_ADAPTER_CLASSIC_ */
/** @} */
