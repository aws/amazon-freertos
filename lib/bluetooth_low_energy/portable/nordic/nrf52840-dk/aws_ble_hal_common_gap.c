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
 * @file aws_ble_hal_common_gap.c
 * @brief Hardware Abstraction Layer for GAP ble stack.
 */

#include <string.h>
#include "FreeRTOS.h"
#include "aws_ble_event_manager.h"
#include "bt_hal_manager_adapter_ble.h"
#include "bt_hal_manager.h"
#include "bt_hal_gatt_server.h"
#include "aws_ble_hal_internals.h"

BTProperties_t xProperties;
uint32_t ulGAPEvtMngHandle;
static BTCallbacks_t xBTCallbacks;

BTStatus_t prvBTManagerInit( const BTCallbacks_t * pxCallbacks );
BTStatus_t prvBtManagerCleanup( void );
BTStatus_t prvBTEnable( uint8_t ucGuestMode );
BTStatus_t prvBTDisable();
BTStatus_t prvBTGetAllDeviceProperties();
BTStatus_t prvBTGetDeviceProperty( BTPropertyType_t xType );
BTStatus_t prvBTSetDeviceProperty( const BTProperty_t * pxProperty );
BTStatus_t prvBTGetAllRemoteDeviceProperties( BTBdaddr_t * pxRemoteAddr );
BTStatus_t prvBTGetRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                         BTPropertyType_t type );
BTStatus_t prvBTSetRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                         const BTProperty_t * property );
BTStatus_t prvBTPair( const BTBdaddr_t * pxBdAddr,
                      BTTransport_t xTransport,
                      bool bCreateBond );
BTStatus_t prvBTCreateBondOutOfBand( const BTBdaddr_t * pxBdAddr,
                                     BTTransport_t xTransport,
                                     const BTOutOfBandData_t * pxOobData );
BTStatus_t prvBTCancelBond( const BTBdaddr_t * pxBdAddr );
BTStatus_t prvBTRemoveBond( const BTBdaddr_t * pxBdAddr );
BTStatus_t prvBTGetConnectionState( const BTBdaddr_t * pxBdAddr );
BTStatus_t prvBTPinReply( const BTBdaddr_t * pxBdAddr,
                          uint8_t ucAccept,
                          uint8_t ucPinLen,
                          BTPinCode_t * pxPinCode );
BTStatus_t prvBTSspReply( const BTBdaddr_t * pxBdAddr,
                          BTSspVariant_t xVariant,
                          uint8_t ucAccept,
                          uint32_t ulPasskey );
BTStatus_t prvBTReadEnergyInfo();
BTStatus_t prvBTDutModeConfigure( bool bEnable );
BTStatus_t prvBTDutModeSend( uint16_t usOpcode,
                             uint8_t * pucBuf,
                             size_t xLen );
BTStatus_t prvBTLeTestMode( uint16_t usOpcode,
                            uint8_t * pucBuf,
                            size_t xLen );
BTStatus_t prvBTConfigHCISnoopLog( bool bEnable );
BTStatus_t prvBTConfigClear();
BTStatus_t prvBTReadRssi( const BTBdaddr_t * pxBdAddr );
BTStatus_t prvBTGetTxpower( const BTBdaddr_t * pxBdAddr,
                            BTTransport_t xTransport );
const void * prvGetClassicAdapter();
const void * prvGetLeAdapter();


static BTInterface_t xBTinterface =
{
    .pxBtManagerInit                = prvBTManagerInit,
    .pxBtManagerCleanup             = prvBtManagerCleanup,
    .pxEnable                       = prvBTEnable,
    .pxDisable                      = prvBTDisable,
    .pxGetAllDeviceProperties       = prvBTGetAllDeviceProperties,
    .pxGetDeviceProperty            = prvBTGetDeviceProperty,
    .pxSetDeviceProperty            = prvBTSetDeviceProperty,
    .pxGetAllRemoteDeviceProperties = prvBTGetAllRemoteDeviceProperties,
    .pxGetRemoteDeviceProperty      = prvBTGetRemoteDeviceProperty,
    .pxSetRemoteDeviceProperty      = prvBTSetRemoteDeviceProperty,
    .pxPair                         = prvBTPair,
    .pxCreateBondOutOfBand          = prvBTCreateBondOutOfBand,
    .pxCancelBond                   = prvBTCancelBond,
    .pxRemoveBond                   = prvBTRemoveBond,
    .pxGetConnectionState           = prvBTGetConnectionState,
    .pxPinReply                     = prvBTPinReply,
    .pxSspReply                     = prvBTSspReply,
    .pxReadEnergyInfo               = prvBTReadEnergyInfo,
    .pxDutModeConfigure             = prvBTDutModeConfigure,
    .pxDutModeSend                  = prvBTDutModeSend,
    .pxLeTestMode                   = prvBTLeTestMode,
    .pxConfigHCISnoopLog            = prvBTConfigHCISnoopLog,
    .pxConfigClear                  = prvBTConfigClear,
    .pxReadRssi                     = prvBTReadRssi,
    .pxGetTxpower                   = prvBTGetTxpower,
    .pxGetClassicAdapter            = prvGetClassicAdapter,
    .pxGetLeAdapter                 = prvGetLeAdapter,
};

/*-----------------------------------------------------------*/


void prvGAPeventHandler(  )
{
     
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTManagerInit( const BTCallbacks_t * pxCallbacks )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    if( pxCallbacks != NULL )
    {
        xBTCallbacks = *pxCallbacks;
    }
    else
    {
        xStatus = eBTStatusFail;
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBtManagerCleanup()
{
    BTStatus_t xStatus = eBTStatusSuccess;

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTEnable( uint8_t ucGuestMode )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTDisable()
{
    BTStatus_t xStatus = eBTStatusSuccess;

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGetAllDeviceProperties()
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGetDeviceProperty( BTPropertyType_t xType )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSetDeviceProperty( const BTProperty_t * pxProperty )
{
    BTStatus_t xStatus = eBTStatusSuccess;


    xBTCallbacks.pxAdapterPropertiesCb( xStatus, 1, ( BTProperty_t * ) pxProperty );

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGetAllRemoteDeviceProperties( BTBdaddr_t * pxRemoteAddr )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGetRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                         BTPropertyType_t type )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSetRemoteDeviceProperty( BTBdaddr_t * pxRemoteAddr,
                                         const BTProperty_t * property )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTPair( const BTBdaddr_t * pxBdAddr,
                      BTTransport_t xTransport,
                      bool bCreateBond )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTCreateBondOutOfBand( const BTBdaddr_t * pxBdAddr,
                                     BTTransport_t xTransport,
                                     const BTOutOfBandData_t * pxOobData )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTCancelBond( const BTBdaddr_t * pxBdAddr )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTRemoveBond( const BTBdaddr_t * pxBdAddr )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGetConnectionState( const BTBdaddr_t * pxBdAddr )
{
    return 0;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTPinReply( const BTBdaddr_t * pxBdAddr,
                          uint8_t ucAccept,
                          uint8_t ucPinLen,
                          BTPinCode_t * pxPinCode )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTSspReply( const BTBdaddr_t * pxBdAddr,
                          BTSspVariant_t xVariant,
                          uint8_t ucAccept,
                          uint32_t ulPasskey )
{
    BTStatus_t xStatus = eBTStatusSuccess;

    return xStatus;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTReadEnergyInfo()
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTDutModeConfigure( bool bEnable )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTDutModeSend( uint16_t usOpcode,
                             uint8_t * pucBuf,
                             size_t xLen )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTLeTestMode( uint16_t usOpcode,
                            uint8_t * pucBuf,
                            size_t xLen )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTConfigHCISnoopLog( bool bEnable )
{
    return eBTStatusUnsupported;
}


/*-----------------------------------------------------------*/

BTStatus_t prvBTConfigClear()
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTReadRssi( const BTBdaddr_t * pxBdAddr )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

BTStatus_t prvBTGetTxpower( const BTBdaddr_t * pxBdAddr,
                            BTTransport_t xTransport )
{
    return eBTStatusUnsupported;
}

/*-----------------------------------------------------------*/

const void * prvGetClassicAdapter()
{
    return NULL;
}

/*-----------------------------------------------------------*/

const BTInterface_t * BTGetBluetoothInterface()
{
    return &xBTinterface;
}

