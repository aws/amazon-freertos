/*
 * Amazon FreeRTOS CELLULAR Preview Release
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
 
/* The config header is always included first. */
#include "aws_cellular_config.h"
#include "cellular_config_defaults.h"

#include "cellular_api.h"
#include "cellular_types.h"

CellularError_t Cellular_Init( CellularHandle_t * pCellularHandle,
                               const CellularCommInterface_t * pCommInterface )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_Cleanup( CellularHandle_t cellularHandle )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_SetRatPriority( CellularHandle_t cellularHandle,
                                         const CellularRat_t * pRatPriorities,
                                         uint8_t ratPrioritiesLength )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_RfOn( CellularHandle_t cellularHandle )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_RfOff( CellularHandle_t cellularHandle )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_GetSimCardStatus( CellularHandle_t cellularHandle,
                                           CellularSimCardStatus_t * pSimCardStatus )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_GetSimCardInfo( CellularHandle_t cellularHandle,
                                         CellularSimCardInfo_t * pSimCardInfo )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_GetModemInfo( CellularHandle_t cellularHandle,
                                       CellularModemInfo_t * pModemInfo )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_GetRegisteredNetwork( CellularHandle_t cellularHandle,
                                               CellularPlmnInfo_t * pNetworkInfo )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_GetNetworkTime( CellularHandle_t cellularHandle,
                                         CellularTime_t * pNetworkTime )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_GetSignalInfo( CellularHandle_t cellularHandle,
                                        CellularSignalInfo_t * pSignalInfo )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_GetServiceStatus( CellularHandle_t cellularHandle,
                                           CellularServiceStatus_t * pServiceStatus )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_SetPdnConfig( CellularHandle_t cellularHandle,
                                       uint8_t contextId,
                                       const CellularPdnConfig_t * pPdnConfig )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_GetPdnStatus( CellularHandle_t cellularHandle,
                                       CellularPdnStatus_t * pPdnStatusBuffers,
                                       uint8_t numStatusBuffers,
                                       uint8_t * pNumStatus )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_ActivatePdn( CellularHandle_t cellularHandle,
                                      uint8_t contextId )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_DeactivatePdn( CellularHandle_t cellularHandle,
                                        uint8_t contextId )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_GetIPAddress( CellularHandle_t cellularHandle,
                                       uint8_t contextId,
                                       char * pBuffer,
                                       uint32_t bufferLength )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_SetDns( CellularHandle_t cellularHandle,
                                 uint8_t contextId,
                                 const char * pDnsServerAddress )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_RegisterUrcNetworkRegistrationEventCallback( CellularHandle_t cellularHandle,
                                                                      CellularUrcNetworkRegistrationCallback_t networkRegistrationCallback )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_RegisterUrcPdnEventCallback( CellularHandle_t cellularHandle,
                                                      CellularUrcPdnEventCallback_t pdnEventCallback )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_RegisterUrcSignalStrengthChangedCallback( CellularHandle_t cellularHandle,
                                                                   CellularUrcSignalStrengthChangedCallback_t signalStrengthChangedCallback )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_RegisterModemEventCallback( CellularHandle_t cellularHandle,
                                                     CellularModemEventCallback_t modemEventCallback )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_RegisterUrcGenericCallback( CellularHandle_t cellularHandle,
                                                     CellularUrcGenericCallback_t genericCallback )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_GetPsmSettings( CellularHandle_t cellularHandle,
                                         CellularPsmSettings_t * pPsmSettings )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_SetPsmSettings( CellularHandle_t cellularHandle,
                                         const CellularPsmSettings_t * pPsmSettings )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_GetEidrxSettings( CellularHandle_t cellularHandle,
                                           CellularEidrxSettingsList_t * pEidrxSettingsList )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_SetEidrxSettings( CellularHandle_t cellularHandle,
                                           const CellularEidrxSettings_t * pEidrxSettings )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_ATCommandRaw( CellularHandle_t cellularHandle,
                                       const char * pATCommandPrefix,
                                       const char * pATCommandPayload,
                                       CellularATCommandType_t atCommandType,
                                       CellularATCommandResponseReceivedCallback_t responseReceivedCallback,
                                       void * pData,
                                       uint16_t dataLen )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_CreateSocket( CellularHandle_t cellularHandle,
                                       uint8_t pdnContextId,
                                       CellularSocketDomain_t socketDomain,
                                       CellularSocketType_t socketType,
                                       CellularSocketProtocol_t socketProtocol,
                                       CellularSocketHandle_t * pSocketHandle )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_SocketConnect( CellularHandle_t cellularHandle,
                                        CellularSocketHandle_t socketHandle,
                                        CellularSocketAccessMode_t dataAccessMode,
                                        const CellularSocketAddress_t * pRemoteSocketAddress )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_SocketSend( CellularHandle_t cellularHandle,
                                     CellularSocketHandle_t socketHandle,
                                     const uint8_t * pData,
                                     uint32_t dataLength,
                                     uint32_t * pSentDataLength )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_SocketRecv( CellularHandle_t cellularHandle,
                                     CellularSocketHandle_t socketHandle,
                                     uint8_t * pBuffer,
                                     uint32_t bufferLength,
                                     uint32_t * pReceivedDataLength )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_SocketClose( CellularHandle_t cellularHandle,
                                      CellularSocketHandle_t socketHandle )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_GetHostByName( CellularHandle_t cellularHandle,
                                        uint8_t contextId,
                                        const char * pcHostName,
                                        char * pResolvedAddress )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_SocketSetSockOpt( CellularHandle_t cellularHandle,
                                           CellularSocketHandle_t socketHandle,
                                           CellularSocketOptionLevel_t optionLevel,
                                           CellularSocketOption_t option,
                                           const uint8_t * pOptionValue,
                                           uint32_t optionValueLength )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_SocketRegisterSocketOpenCallback( CellularHandle_t cellularHandle,
                                                           CellularSocketHandle_t socketHandle,
                                                           CellularSocketOpenCallback_t socketOpenCallback,
                                                           void * pCallbackContext )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_SocketRegisterDataReadyCallback( CellularHandle_t cellularHandle,
                                                          CellularSocketHandle_t socketHandle,
                                                          CellularSocketDataReadyCallback_t dataReadyCallback,
                                                          void * pCallbackContext )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/

CellularError_t Cellular_SocketRegisterClosedCallback( CellularHandle_t cellularHandle,
                                                       CellularSocketHandle_t socketHandle,
                                                       CellularSocketClosedCallback_t closedCallback,
                                                       void * pCallbackContext )
{
    /* FIX ME. */
    return CELLULAR_UNSUPPORTED;
}
/*-----------------------------------------------------------*/
