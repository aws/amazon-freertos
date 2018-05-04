/*******************************************************************************
*  Network Interface file
*
*  Summary:
*   Network Interface file for FreeRTOS-Plus-TCP stack
*
*  Description:
*   - Interfaces PIC32 to the FreeRTOS TCP/IP stack
*******************************************************************************/

/*******************************************************************************
*  File Name:  pic32_NetworkInterface.c
*  Copyright 2017 Microchip Technology Incorporated and its subsidiaries.
*
*  Amazon FreeRTOS Wi-Fi for Curiosity PIC32MZEF V1.0.3
*  Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy of
*  this software and associated documentation files (the "Software"), to deal in
*  the Software without restriction, including without limitation the rights to
*  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
*  of the Software, and to permit persons to whom the Software is furnished to do
*  so, subject to the following conditions:
*  The above copyright notice and this permission notice shall be included in all
*  copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*  SOFTWARE
*******************************************************************************/
#ifndef PIC32_USE_ETHERNET
    #include <sys/kmem.h>

    #include "FreeRTOS.h"
    #include "semphr.h"
    #include "event_groups.h"
    #include "FreeRTOS_IP.h"
    #include "FreeRTOS_IP_Private.h"
    #include "NetworkInterface.h"
    #include "NetworkBufferManagement.h"

    #include "aws_wifi.h"
    #include "aws_secure_sockets.h"

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
    #include "wilc1000_task.h"
    #include "wdrv_wilc1000_main.h"
    #include "wdrv_wilc1000_common.h"

    /* initialization done timeout. */
    #define WIFI_MAC_INIT_TIMEOUT       ( pdMS_TO_TICKS( 5000 ) )
    #define WIFI_MAC_CONNECT_TIMEOUT    ( pdMS_TO_TICKS( 10000 ) )
    #define WIFI_MAC_DISCONNECT_TIMEOUT_SECONDS 10

    #define WIFI_MAX_PSK_LEN            65

    /*!< Maximum size for the WPA PSK including the NULL termination.
    */


    bool isLinkUp();

    WDRV_HOOKS WILC1000_hooks;
    void xNetworkFrameReceived( uint32_t len,
                                uint8_t const * const frame );

    /************************************* Section: worker code ************************************************** */
    /* */

    void WIFI_SecuritySet( WIFISecurity_t securityMode,
                        const char * pcKey,
                        uint8_t u8KeyLength )
    {
        uint8_t au8PSK[ WIFI_MAX_PSK_LEN ];

        if( securityMode != eWiFiSecurityOpen )
        {
             memcpy( au8PSK, pcKey, u8KeyLength );
        }

        switch( securityMode )
        {
            case eWiFiSecurityOpen:
                WDRV_EXT_CmdSecNoneSet();
                break;

            case eWiFiSecurityWEP:

                if( ( u8KeyLength == 5 ) || ( u8KeyLength == 13 ) )
                {
                    int i;
                    uint8_t buf[ 26 ] = { 0 };
                    uint8_t c_value;

                    for( i = 0; i < u8KeyLength * 2; i++ )
                    {
                        c_value = ( i % 2 == 0 ) ? ( au8PSK[ i / 2 ] >> 4 ) : ( au8PSK[ i / 2 ] & 0x0F );
                        buf[ i ] = ( c_value > 9 ) ? ( 'A' + c_value - 0x0A ) : ( '0' + c_value - 0x00 );
                    }

                    u8KeyLength *= 2;
                    memcpy( au8PSK, buf, u8KeyLength );
                    au8PSK[ u8KeyLength ] = 0x00;
                }

                WDRV_EXT_CmdSecWEPSet( au8PSK, u8KeyLength );
                break;

            case eWiFiSecurityWPA:
            case eWiFiSecurityWPA2:
                WDRV_EXT_CmdSecWPASet( au8PSK, u8KeyLength );
                break;

            default:
                WDRV_ASSERT( pdFALSE, "Undefined security mode" );
                break;
        }
    }


    /**
     * @brief Sets Wi-Fi mode.
     *
     * @param[in] xDeviceMode - Mode of the device Station / Access Point /P2P.
     *
     * @return eWiFiSuccess if everything succeeds, failure code otherwise.
     */
    WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
    {
        WIFIReturnCode_t ret = eWiFiNotSupported;

        switch( xDeviceMode )
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
                /* WDRV_ASSERT drops execution into a while(1) loop. It is better to exit this 
                * function. */
                /* WDRV_ASSERT( pdFALSE, "P2P mode will be supported in a future drop\n" ); */
                configPRINTF( ("P2P mode will be supported in a future drop\r\n") );
                ret = eWiFiNotSupported;
                break;

            default:
                /* WDRV_ASSERT drops execution into a while(1) loop. It is better to exit this 
                * function. */
                /* WDRV_ASSERT( pdFALSE, "Undefined network type" ); */
                configPRINTF( ("Undefined network type\r\n") );
                ret = eWiFiNotSupported;
                break;
        }

        return ret;
    }


    /**
     * @brief Gets Wi-Fi mode.
     *
     * @param[in] pxDeviceMode - return mode Station / Access Point /P2P
     *
     * @return eWiFiSuccess if everything succeeds, failure code otherwise..
     */
    WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
    {
        configASSERT( pxDeviceMode != NULL );
        
        *pxDeviceMode = WDRV_EXT_CmdNetModeGet();

        return eWiFiSuccess;
    }


    /**
     * @brief Initializes the Wi-Fi module.
     *
     * This function must be called exactly once before any other
     * Wi-Fi functions (including socket functions) can be used.
     *
     * @return eWiFiSuccess if everything succeeds, eWiFiFailure otherwise.
     */
    WIFIReturnCode_t WIFI_On( void )
    {
        EventBits_t evBits;

        WILC1000_hooks.frame_received = xNetworkFrameReceived;
        
        /* Make sure the Wi-Fi is off before turning on. */
        WDRV_EXT_Deinitialize();

        /*Initialize Wi-Fi Driver Radio and Firmware .*/
        if( WDRV_EXT_Initialize_bare( &WILC1000_hooks ) != WDRV_SUCCESS )
        {
            return eWiFiFailure;
        }

        /* Wait for Wi-Fi initialization to complete. */
        xTaskNotifyWait( WDRV_MAC_EVENT_INIT_DONE, WDRV_MAC_EVENT_INIT_DONE, &evBits, WIFI_MAC_INIT_TIMEOUT );

        if( ( evBits & WDRV_MAC_EVENT_INIT_DONE ) == 0 )
        {
            /* Timed out. */
            return eWiFiTimeout;
        }

        /*Check if init was successful. */
        if( isWdrvExtReady() != pdTRUE )
        {
            /* Init failed. */
            return eWiFiFailure;
        }

        return eWiFiSuccess;
    }


    /**
     * @brief Turns off the Wi-Fi module.
     *
     * This function turns off Wi-Fi module.
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
    WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
    {
        EventBits_t evBits;
        uint8_t ucMACAddress[ipMAC_ADDRESS_LENGTH_BYTES];
        configASSERT( pxNetworkParams != NULL );
        configASSERT( pxNetworkParams->pcSSID != NULL );

        if ( pxNetworkParams->xSecurity != eWiFiSecurityOpen )
        {
            configASSERT( pxNetworkParams->pcPassword != NULL );
        }

        /* Set mode to station mode. */
        WIFI_SetMode( eWiFiModeStation );

        /* Set the AP channel. */
        /* Setting the AP channel is optional to connect to an AP. The router automatically
         * sets a channel for the connection. */
        /* WDRV_EXT_CmdChannelSet( pxNetworkParams->cChannel ); */

        /*Set the AP SSID. */
        if( pxNetworkParams->ucSSIDLength > wificonfigMAX_SSID_LEN )
        {
            return eWiFiFailure;
        }
        WDRV_EXT_CmdSSIDSet( ( uint8_t * ) pxNetworkParams->pcSSID, pxNetworkParams->ucSSIDLength );

        /*Set the AP security. */
        if( pxNetworkParams->ucPasswordLength > wificonfigMAX_PASSPHRASE_LEN )
        {
            return eWiFiFailure;
        }
        WIFI_SecuritySet( pxNetworkParams->xSecurity, pxNetworkParams->pcPassword, pxNetworkParams->ucPasswordLength );

        /*Set driver state. */
        WDRV_IsDisconnectRequestedSet( pdFALSE );
        WDRV_ConnectionStateSet( WDRV_CONNECTION_STATE_IN_PROGRESS );

        WDRV_DBG_INFORM_MESSAGE( ( "Start Wi-Fi Connection...\r\n" ) );
        
        /* Read MAC address from the chip and set it in FreeRTOS network stack. */        
        WDRV_EXT_CmdMacAddressGet( ucMACAddress );       
        FreeRTOS_UpdateMACAddress( ucMACAddress );

        if( WDRV_EXT_CmdConnect() != WDRV_SUCCESS )
        {
            /* Connection failed miserably. */
            return eWiFiFailure;
        }

        /* Wait for Wi-Fi connection to complete. */
        xTaskNotifyWait( WDRV_MAC_EVENT_CONNECT_DONE, WDRV_MAC_EVENT_CONNECT_DONE, &evBits, WIFI_MAC_CONNECT_TIMEOUT );

        if( ( evBits & WDRV_MAC_EVENT_CONNECT_DONE ) == 0 )
        {
            /* Timed out. */
            return eWiFiTimeout;
        }

        if( isLinkUp() )
        {
            return eWiFiSuccess;
        }
        else
        {
            return eWiFiFailure;
        }
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
        uint32_t ulSecondsCount = 0;
        
        if( WDRV_EXT_CmdDisconnect() != WDRV_SUCCESS )
        {
            /* Failed miserably. */
            return eWiFiFailure;
        }
        
        /* Check if the connection is still up, then give the driver time
        * to disconnect. */
        while ( isLinkUp() ) {
            
            /* Timeout disconnection after 10 seconds. */
            if (ulSecondsCount == WIFI_MAC_DISCONNECT_TIMEOUT_SECONDS) {
                return eWiFiTimeout;
            }
            
            vTaskDelay( pdMS_TO_TICKS( 1000 ) );
            ulSecondsCount++;
        }

        return eWiFiSuccess;
    }

    /**
     * @brief Retrieves host IP address from URL using DNS
     *
     * @param[in] pcHost - Host URL.
     * @param[in] pucIPAddr - IP Address buffer.
     *
     * @return eWiFiSuccess if everything succeeds, failure code otherwise.
     *
     */
    WIFIReturnCode_t WIFI_GetHostIP( char * pcHost, uint8_t * pucIPAddr )
    {
        WIFIReturnCode_t xStatus = eWiFiFailure;
        
        configASSERT( pcHost != NULL );
        configASSERT( pucIPAddr != NULL );
        
        *( ( uint32_t * ) pucIPAddr ) = SOCKETS_GetHostByName( pcHost );

        if ( *( ( uint32_t * ) pucIPAddr ) != 0 )
        {
            xStatus = eWiFiSuccess;
        }

        return xStatus;
    }

    /**
     * @brief Retrieves the Wi-Fi interface's IP address.
     *
     * @param[in] pucIPAddr - IP Address buffer.
     * unsigned long ulIPAddress = 0;
     * WIFI_GetIP(&ulIPAddress);
     *
     * @return eWiFiSuccess if disconnected, eWiFiFailure otherwise.
     */
    WIFIReturnCode_t WIFI_GetIP( uint8_t * pucIPAddr )
    {
        configASSERT( pucIPAddr != NULL );
        
        *( ( uint32_t * ) pucIPAddr ) = FreeRTOS_GetIPAddress();

        return eWiFiSuccess;
    }


    /**
     * @brief Configure SoftAP
     *
     * @param[in] pxNetworkParams - Network params to configure and start soft AP.
     *
     * @return eWiFiSuccess if everything succeeds, failure code otherwise.
     */
    WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams )
    {
        configASSERT( pxNetworkParams != NULL );
        configASSERT( pxNetworkParams->pcSSID != NULL );
        
        if ( pxNetworkParams->xSecurity != eWiFiSecurityOpen )
        {
            configASSERT( pxNetworkParams->pcPassword != NULL );
        }
        
        /* Set mode to station mode. */
        WIFI_SetMode( eWiFiModeAP );

        /* Set the AP channel. */
        WDRV_EXT_CmdChannelSet( pxNetworkParams->cChannel );

        /*Set the AP SSID. */
        if( pxNetworkParams->ucSSIDLength > wificonfigMAX_SSID_LEN )
        {
            return eWiFiFailure;
        }
        WDRV_EXT_CmdSSIDSet( ( uint8_t * ) pxNetworkParams->pcSSID, pxNetworkParams->ucSSIDLength );

        /*Set the AP security. */
        if( pxNetworkParams->ucPasswordLength > wificonfigMAX_PASSPHRASE_LEN )
        {
            return eWiFiFailure;
        }
        WIFI_SecuritySet( pxNetworkParams->xSecurity, pxNetworkParams->pcPassword, pxNetworkParams->ucPasswordLength );

        /*Set driver state. */
        WDRV_IsDisconnectRequestedSet( pdFALSE );
        WDRV_ConnectionStateSet( WDRV_CONNECTION_STATE_IN_PROGRESS );

        return eWiFiSuccess;
    }

    /**
     * @brief Stop SoftAP operation.
     *
     * @param[in] None
     *
     * @return eWiFiSuccess if everything succeeds, failure code otherwise.
     */
    WIFIReturnCode_t WIFI_StartAP( void )
    {
        EventBits_t evBits;

        if( WDRV_EXT_CmdConnect() != WDRV_SUCCESS )
        {
            /* Connection failed miserably. */
            return eWiFiFailure;
        }

        /* Wait for Wi-Fi connection to complete. */
        xTaskNotifyWait( WDRV_MAC_EVENT_CONNECT_DONE, WDRV_MAC_EVENT_CONNECT_DONE, &evBits, WIFI_MAC_CONNECT_TIMEOUT );

        if( ( evBits & WDRV_MAC_EVENT_CONNECT_DONE ) == 0 )
        {
            /* Timed out. */
            return eWiFiTimeout;
        }

        if( isLinkUp() )
        {
            return eWiFiSuccess;
        }
        else
        {
            return eWiFiFailure;
        }
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
     * @return eWiFiSuccess if everything succeeds, failure code otherwise.
     */
    WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                    const void * pvOptionValue )
    {
        configASSERT( pvOptionValue != NULL );
        
        /* TODO: correctly implement this function .*/
        WDRV_EXT_CmdPowerSavePut( pdFALSE, xPMModeType, 1 );

        return eWiFiSuccess;
    }

    /**
     * @brief Get power management mode
     *
     * @param[out] xPMModeType - pointer to get current power mode set.
     *
     * @param[out] pvOptionValue - optional value
     *
     * @return eWiFiSuccess if everything succeeds, failure code otherwise.
     */
    WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                    void * pvOptionValue )
    {
        return eWiFiNotSupported;
    }

    /**
     * @brief Retrieves the Wi-Fi interface's MAC address.
     *
     * @param[out] MAC Address buffer.
     *
     * @return eWiFiSuccess if disconnected, eWiFiFailure otherwise.
     */
    WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac )
    {
        WIFIReturnCode_t xReturnStatus = eWiFiFailure;
        
        configASSERT( pucMac != NULL );

        if ( WDRV_EXT_CmdMacAddressGet( pucMac ) == WDRV_SUCCESS )
        {
            xReturnStatus = eWiFiSuccess;
        }
        
        return xReturnStatus;
    }


    /**
     * @brief Perform WiF Scan
     *
     * @param[in] pxBuffer - Buffer for scan results.
     * @param[in] uxNumNetworks - Number of networks in scan result.
     *
     * @return eWiFiSuccess if disconnected, eWiFiFailure otherwise.
     */
    WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                                uint8_t ucNumNetworks )
    {
        return eWiFiNotSupported;
    }

    /**
     * @brief Wi-Fi Add Network profile.
     *
     * Adds Wi-Fi network to network list in non-volatile memory
     *
     * @param[in] pxNetworkProfile - network profile parameters
     * @param[out] pusIndex - network profile index
     *
     * @return Profile stored index on success, or negative error code on failure.
     */
    WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile,
                                    uint16_t * pusIndex )
    {
        return eWiFiNotSupported;
    }

    /**
     * @brief Wi-Fi Get Network profile .
     *
     * Gets Wi-Fi network parameters at given index from network list in Non volatile memory
     *
     * @param[out] pxNetworkProfile - pointer to return network profile parameters
     * @param[in] usIndex - Index of the network profile,
     *                       must be between 0 to wificonfigMAX_NETWORK_PROFILES
     *
     * @return eWiFiSuccess if everything succeeds, failure code otherwise.
     */
    WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                    uint16_t usIndex )
    {
        return eWiFiNotSupported;
    }

    /**
     * @brief Wi-Fi Delete Network profile .
     *
     * Deletes Wi-Fi network from network list at given index in Non volatile memory
     *
     * @param[in] usIndex - Index of the network profile, must be between 0 to wificonfigMAX_NETWORK_PROFILES
     *                      wificonfigMAX_NETWORK_PROFILES as index will delete all network profiles
     *
     * @return eWiFiSuccess if everything succeeds, failure code otherwise.
     */
    WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex )
    {
        return eWiFiNotSupported;
    }

    /**
     * @brief Check if the Wi-Fi is connected.
     * 
     * @return pdTRUE if the link is up, pdFalse otherwise.
     */
    BaseType_t WIFI_IsConnected( void )
    {
        BaseType_t xIsConnected = pdFALSE;
        
        if ( isLinkUp() ) {
            xIsConnected = pdTRUE;
        }
        
        return xIsConnected;
    }
    
    /**
    * @brief Ping an IP address in the network.
    *
    * @param[in] IP Address array to ping.
    * @param[in] Number of times to ping
    * @param[in] Interval in mili-seconds for ping operation
    *
    * @return eWiFiSuccess if ping was successful, other failure code otherwise.
    */
    WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr,
                            uint16_t usCount,
                            uint32_t ulIntervalMS )
    {
        return eWiFiNotSupported;
    }
    
    /**
    * @brief Resets the Wi-Fi Module.
    *
    * @param[in] None.
    *
    * @return eWiFiSuccess if Wi-Fi module was successfully reset, failure code otherwise.
    */
   WIFIReturnCode_t WIFI_Reset( void )
   {
       return eWiFiNotSupported;
   }

#endif
