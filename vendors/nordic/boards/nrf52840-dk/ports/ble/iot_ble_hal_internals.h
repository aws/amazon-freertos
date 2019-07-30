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
 * @file iot_ble_hal_internals.h
 * @brief Internally shared functions and variable for HAL ble stack.
 */

#ifndef _AWS_BLE_INTERNALS_H_
#define _AWS_BLE_INTERNALS_H_

#include "ble_gap.h"
#include "bt_hal_manager.h"
#include "nrf_ble_gatt.h"
#include "peer_manager.h"
#include "ble_advertising.h"
#include "iot_ble_config.h"
#include "nrf_strerror.h"


#define _IOT_BLE_TOGGLE_BLOB_CREATE    0

typedef enum
{
    GATT_SERVICE, GATT_CHAR, GATT_DESCR
} BTGattAttributeType_t;
typedef struct
{
    ble_gap_addr_t xDeviceAddress;
    uint32_t ulMtu;
    uint8_t * puDeviceName;
    uint16_t usDeviceNameLength;
    BTDeviceType_t xDeviceType;
    BTIOtypes_t xIOTypes;
    bool bBondable;
    bool bOnlySecure;
} BTProperties_t;

typedef struct
{
    const BTBdaddr_t * pxBdAddr;
    uint32_t ulMinInterval;
    uint32_t ulMaxInterval;
    uint32_t ulLatency;
    uint32_t ulTimeout;
} BTConnectionParams_t;

extern BTConnectionParams_t xLatestDesiredConnectionParams;
extern uint8_t pucBondedAddresses[ 6 * IOT_BLE_MAX_BONDED_DEVICES ];
extern uint32_t ulGAPEvtMngHandle;
extern BTBleAdapterCallbacks_t xBTBleAdapterCallbacks;
extern uint32_t ulGattServerIFhandle;
extern bool bGattInitialized;
extern uint16_t usGattConnHandle;
extern BTBdaddr_t xConnectionRemoteAddress;
extern bool bGattServerInitialized;
extern BTProperties_t xProperties;
extern uint8_t ucBaseUUIDindex;
extern BTUuid_t xAppUuid;
extern bool bIsAdvertising;
extern BTGattServerCallbacks_t xGattServerCb;
extern ble_advertising_t xAdvertisingHandle;
extern ret_code_t prvResumeAdvertisement( uint8_t ucAdvMode );
const void * prvBTGetGattServerInterface();
void prvGAPeventHandler( ble_evt_t const * p_ble_evt,
                         void * p_context );
void prvPmEventHandler( const pm_evt_t * event );

nrf_ble_gatt_t * prvGetGattHandle();

const void * prvGetLeAdapter();

ret_code_t xBTGattUpdateMtu( nrf_ble_gatt_t * xGattHandler,
                             uint16_t usConnHandle );

extern ble_gap_sec_params_t xSecurityParameters;

ret_code_t prvAFRUUIDtoNordic( BTUuid_t * pxAFRUuid,
                               ble_uuid_t * pxNordicUUID );

ret_code_t prvStopAdv();

/**
 * @brief Returns the softdevice handle for a given inner GATT handle
 * @param handle inner GATT handle
 * @return the softdevice handle
 */
uint16_t prvGattToSDHandle( uint16_t handle );

/**
 * @brief Returns the given inner GATT handle for a softdevice handle
 * @param handle softdevice handle
 * @return inner GATT handle
 */
uint16_t prvGattFromSDHandle( uint16_t handle );

BTGattAttributeType_t prvGattAttributeType( uint16_t handle );
static inline const BTStatus_t BTNRFError( const ret_code_t xErrCode )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    switch( xErrCode )
    {
        case NRF_SUCCESS:
            xStatus = eBTStatusSuccess;
            break;

        case NRF_ERROR_API_NOT_IMPLEMENTED:
            xStatus = eBTStatusUnsupported;
            break;

        case NRF_ERROR_BUSY:
            xStatus = eBTStatusBusy;
            break;

        case NRF_ERROR_FEATURE_NOT_ENABLED:
            xStatus = eBTStatusUnsupported;
            break;

        case NRF_ERROR_NO_MEM:
            xStatus = eBTStatusNoMem;
            break;

        case NRF_ERROR_INVALID_PARAM:
            xStatus = eBTStatusParamInvalid;
            break;

        default:
            xStatus = eBTStatusFail;
    }

    return xStatus;
}

#define BT_NRF_PRINT_ERROR( function, errorcode )                                                                                  \
    if( ( errorcode ) != NRF_SUCCESS )                                                                                             \
    {                                                                                                                              \
        NRF_LOG_ERROR( "Error, cannot execute " # function ", err_code: %d, %s\n", ( errorcode ), nrf_strerror_get( errorcode ) ); \
    }                                                                                                                              \




#endif /* ifndef _AWS_BLE_INTERNALS_H_ */
