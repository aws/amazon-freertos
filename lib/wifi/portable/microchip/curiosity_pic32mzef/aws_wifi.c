/*******************************************************************************
  Network Interface file

  Summary:
    Network Interface file for FreeRTOS-Plus-TCP stack
    
  Description:
    - Interfaces PIC32 to the FreeRTOS TCP/IP stack
*******************************************************************************/

/*******************************************************************************
File Name:  pic32_NetworkInterface.c
Copyright 2017 Microchip Technology Incorporated and its subsidiaries.

Amazon FreeRTOS Wi-Fi for Curiosity_PIC32MZEF V1.0.0
Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE
*******************************************************************************/
#include <sys/kmem.h>

#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "Port_Sockets.h"
#include "NetworkInterface.h"
#include "NetworkBufferManagement.h"


#include "pic32_NetworkConfig.h"

#include "peripheral/eth/plib_eth.h"

#include "system_config.h"
#include "system/console/sys_console.h"
#include "system/debug/sys_debug.h"
#include "system/command/sys_command.h"

#include "driver/ethmac/drv_ethmac.h"
#include "driver/miim/drv_miim.h"

#include "tcpip/tcpip.h"
#include "tcpip/src/tcpip_private.h"
#include "tcpip/src/link_list.h"
#include "Port_WiFi.h"
#include "Wilc1000_task.h"
#include "Wdrv_wilc1000_main.h"
#include "Wdrv_wilc1000_common.h"

// initialization done timeout 
#define WIFI_MAC_INIT_TIMEOUT        (pdMS_TO_TICKS(5000))
#define WIFI_MAC_CONNECT_TIMEOUT        (pdMS_TO_TICKS(1000))


#define WIFI_MAX_PSK_LEN           								65
/*!< Maximum size for the WPA PSK including the NULL termination.
 */


bool isLinkUp();

WDRV_HOOKS WILC1000_hooks;
void xNetworkFrameReceived(uint32_t len, uint8_t const *const frame);

//************************************ Section: worker code **************************************************
//

 void WIFI_SecuritySet(WIFISecurity_t securityMode, const char * pcKey, uint8_t u8KeyLength)
{
	uint8_t	au8PSK[WIFI_MAX_PSK_LEN];

	memcpy(au8PSK,pcKey, u8KeyLength);

    switch (securityMode) {
    	case eWiFiSecurityOpen:
        	WDRV_EXT_CmdSecNoneSet();
        	break;
    	case eWiFiSecurityWEP:
        	if (( u8KeyLength== 5) || (u8KeyLength  == 13)) {
	            	int i;
	            	uint8_t buf[26] = {0};
	            	uint8_t c_value;
	            	for (i = 0; i < u8KeyLength * 2; i++) {
	                c_value = (i % 2 == 0 ) ? (au8PSK[i / 2] >> 4) : (au8PSK[i / 2] & 0x0F);
	                buf[i] = (c_value > 9) ? ('A' + c_value - 0x0A) : ('0' + c_value - 0x00);
	            	}
	            	u8KeyLength *= 2;
	            	memcpy(au8PSK, buf, u8KeyLength);
	            	au8PSK[u8KeyLength] = 0x00;
        	}
        	WDRV_EXT_CmdSecWEPSet(au8PSK, u8KeyLength);
       	break;
    	case eWiFiSecurityWPA:
	case eWiFiSecurityWPA2:
        	WDRV_EXT_CmdSecWPASet(au8PSK, u8KeyLength);
        	break;

    	default:
        	WDRV_ASSERT(false, "Undefined security mode");
        	break;
    }
}


/**
 * @brief Sets wifi mode.
 *
 * @param[in] xDeviceMode - Mode of the device Station / Access Point /P2P.
 *
 * @return eWiFiSuccess if disconnected, failure code otherwise.
 */
WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
{
	WIFIReturnCode_t ret = eWiFiFailure;
    	switch (xDeviceMode) 
	{
    	case eWiFiModeStation:
        	WDRV_EXT_CmdNetModeBSSSet();
		gp_wdrv_cfg->networkType = WDRV_NETWORK_TYPE_INFRASTRUCTURE;
		ret = eWiFiSuccess;
        	break;
    	case eWiFiModeAP:
        	WDRV_EXT_CmdNetModeAPSet();
		gp_wdrv_cfg->networkType = WDRV_NETWORK_TYPE_SOFT_AP;
		ret = eWiFiSuccess;
        	break;
    	case eWiFiModeP2P:
        	WDRV_ASSERT(false, "P2P mode will be supported in a future drop\n");
		ret = eWiFiFailure;
        	break;
    	default:
        	WDRV_ASSERT(false, "Undefined network type");
		ret = eWiFiFailure;
        	break;
    }

	return ret;	
}


/**
 * @brief Gets wifi mode.
 *
 * @param[in] pxDeviceMode - return mode Station / Access Point /P2P
 *
 * @return eWiFiSuccess if disconnected, failure code otherwise.
 */
WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
{

	* pxDeviceMode = WDRV_EXT_CmdNetModeGet();
	
	return eWiFiSuccess;	
}


/**
 * @brief Initializes the WiFi module.
 *
 * This function must be called exactly once before any other
 * WiFi functions (including socket functions) can be used.
 *
 * @return eWiFiSuccess if everything succeeds, eWiFiFailure otherwise.
 */
WIFIReturnCode_t WIFI_On( void )
{
	 EventBits_t  evBits;
	 
	WILC1000_hooks.frame_received = xNetworkFrameReceived;
	
	//Initialize WiFi Driver Radio and Firmware
	if (WDRV_EXT_Initialize_bare(&WILC1000_hooks) != WDRV_SUCCESS)
		return eWiFiFailure;
	

	// Wait for WiFi initlization to complete
	xTaskNotifyWait( WDRV_MAC_EVENT_INIT_DONE, WDRV_MAC_EVENT_INIT_DONE, &evBits, WIFI_MAC_INIT_TIMEOUT);

       if((evBits & WDRV_MAC_EVENT_INIT_DONE) == 0)
        {   
        	// Timed out 
		return eWiFiTimeout;
         }

	//Check if init was successful
        if(isWdrvExtReady() != true)
        {   
        	// Init failed!! 
		return eWiFiFailure;
        }
	
	return eWiFiSuccess;	
}


/**
 * @brief Turns off the WiFi module.
 *
 * This function turns off wifi module.
 *
 * @return eWiFiSuccess if everything succeeds, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Off( void )
{

	WDRV_EXT_Deinitialize();

	return eWiFiSuccess;	
}


/**
 * @brief Connects to Access Point.
 *
 * @param[in] pxNetworkParams Configuration to join AP.
 *
 * @return eWiFiSuccess if connection is successful, failure code otherwise.
 */
WIFIReturnCode_t WIFI_ConnectAP(const WIFINetworkParams_t * const pxNetworkParams)
{
	 EventBits_t  evBits;

	// Set mode to station mode
	WIFI_SetMode(eWiFiModeStation);

	// Set the AP channel
       WDRV_EXT_CmdChannelSet(pxNetworkParams->cChannel);

	//Set the AP SSID
    	WDRV_EXT_CmdSSIDSet((uint8_t *)pxNetworkParams->pcSSID, pxNetworkParams->ucSSIDLength);

	//Set the AP security
    	WIFI_SecuritySet(pxNetworkParams->xSecurity, pxNetworkParams->pcPassword,pxNetworkParams->ucPasswordLength);

	//Set driver state
	WDRV_IsDisconnectRequestedSet(false);
	WDRV_ConnectionStateSet(WDRV_CONNECTION_STATE_IN_PROGRESS);

	
	WDRV_DBG_INFORM_MESSAGE(("\r\nStart Wi-Fi Connection...\r\n"));

	if(WDRV_EXT_CmdConnect()!=WDRV_SUCCESS)
	{   
        	// Connection failed Miserably!! 
		return eWiFiFailure;
        }

	// Wait for WiFi connection to complete
	xTaskNotifyWait( WDRV_MAC_EVENT_CONNECT_DONE, WDRV_MAC_EVENT_CONNECT_DONE, &evBits, WIFI_MAC_CONNECT_TIMEOUT);

	 if((evBits & WDRV_MAC_EVENT_CONNECT_DONE) == 0)
        {   
        	// Timed out 
		return eWiFiTimeout;
         }	
	  
	if(isLinkUp())
		return eWiFiSuccess;
	else
		return eWiFiFailure;	
}

/**
 * @brief Disconnects from Access Point.
 *
 * @param[in] None.
 *
 * @return eWiFiSuccess if disconnected, failure code otherwise.
 */
WIFIReturnCode_t WIFI_Disconnect( void )
{
	if(WDRV_EXT_CmdDisconnect()!=WDRV_SUCCESS)
	{   
        	// failed Miserably!! 
		return eWiFiFailure;
        }

	return eWiFiSuccess;
}

/**
 * @brief Retrieves host IP address from URL using DNS
 *
 * @param[in] pcHost - Host URL.
 * @param[in] pucIPAddr - IP Address buffer.
 *
 * @return eWiFiSuccess if disconnected, failure code otherwise.
 * 
 */
WIFIReturnCode_t WIFI_GetHostIP( char * pcHost,
                                 uint8_t * pucIPAddr )
{
	*((uint32_t*)pucIPAddr)  = SOCKETS_GetHostByName(pcHost);

	return eWiFiSuccess;
}

/**
 * @brief Retrieves the WiFi interface's IP address.
 *
 * @param[in] pucIPAddr - IP Address buffer.
 * unsigned long ulIPAddress = 0;
 * WIFI_GetIP(&ulIPAddress);
 *
 * @return eWiFiSuccess if disconnected, eWiFiFailure otherwise.
 */
WIFIReturnCode_t WIFI_GetIP( uint8_t * pucIPAddr )
{
	*((uint32_t*)pucIPAddr) = FreeRTOS_GetIPAddress();

	return eWiFiSuccess;
}


/**
 * @brief Configure SoftAP
 *
 * @param[in] pxNetworkParams - Network params to configure and start soft AP.
 *
 * @return eWiFiSuccess if disconnected, failure code otherwise.
 */
WIFIReturnCode_t WIFI_ConfigureAP( WIFINetworkParams_t * pxNetworkParams )
{
	// Set mode to station mode
	WIFI_SetMode(eWiFiModeAP);

	// Set the AP channel
       WDRV_EXT_CmdChannelSet(pxNetworkParams->cChannel);

	//Set the AP SSID
    	WDRV_EXT_CmdSSIDSet((uint8_t*)pxNetworkParams->pcSSID, pxNetworkParams->ucSSIDLength);

	//Set the AP security
    	WIFI_SecuritySet(pxNetworkParams->xSecurity, pxNetworkParams->pcPassword,pxNetworkParams->ucPasswordLength);

	//Set driver state
	WDRV_IsDisconnectRequestedSet(false);
	WDRV_ConnectionStateSet(WDRV_CONNECTION_STATE_IN_PROGRESS);

	return eWiFiSuccess;
}

/**
 * @brief Stop SoftAP operation.
 *
 * @param[in] None
 *
 * @return eWiFiSuccess if disconnected, failure code otherwise.
 */
WIFIReturnCode_t WIFI_StartAP( void )
{
	 EventBits_t  evBits;

	if(WDRV_EXT_CmdConnect()!=WDRV_SUCCESS)
	{   
        	// Connection failed Miserably!! 
		return eWiFiFailure;
        }


	// Wait for WiFi connection to complete
	xTaskNotifyWait( WDRV_MAC_EVENT_CONNECT_DONE, WDRV_MAC_EVENT_CONNECT_DONE, &evBits, WIFI_MAC_CONNECT_TIMEOUT);

	 if((evBits & WDRV_MAC_EVENT_CONNECT_DONE) == 0)
        {   
        	// Timed out 
		return eWiFiTimeout;
         }	
	  
	if(isLinkUp())
		return eWiFiSuccess;
	else
		return eWiFiFailure;	
}

/**
 * @brief Stop SoftAP operation.
 *
 * @param[in] None
 *
 * @return eWiFiSuccess if disconnected, failure code otherwise.
 */
WIFIReturnCode_t WIFI_StopAP( void )
{
	return WIFI_Disconnect();
}

/**
 * @brief Set power management mode
 *
 * @param[in] usPMModeType - Low power mode type.
 *
 * @param[in] pvOptionValue - A buffer containing the value of the option to set
 *                            depends on the mode type 
 *                            ex - beacon interval in sec
 *
 * @return eWiFiSuccess if disconnected, failure code otherwise.
 */
WIFIReturnCode_t WIFI_SetPMMode( uint16_t usPMModeType,  
                                 const void * pvOptionValue )
{
	// TODO: correctly implement this function
	WDRV_EXT_CmdPowerSavePut(false,usPMModeType, 1);
    return eWiFiSuccess;
}

