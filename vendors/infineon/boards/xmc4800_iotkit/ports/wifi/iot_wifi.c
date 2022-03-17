/*
 * FreeRTOS Wi-Fi for Infineon XMC4800 IoT Connectivity Kit V1.0.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Copyright (c) 2018, Infineon Technologies AG
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 * products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes with
 * Infineon Technologies AG dave@infineon.com).
 */

/**
 * @file iot_wifi.c
 * @brief Wi-Fi Interface.
 */

/* Socket and Wi-Fi interface includes. */
#include "FreeRTOS.h"
#include "iot_wifi.h"

/* Wi-Fi configuration includes. */
#include "aws_wifi_config.h"
#include "limits.h"
#include "esp/esp.h"

#define CONNECT_BIT    (1 << 0)
#define DISCONNECT_BIT (1 << 1)

SemaphoreHandle_t xWiFiSemaphoreHandle; /**< Wi-Fi module semaphore. */
const TickType_t xSemaphoreWaitTicks = pdMS_TO_TICKS( wificonfigMAX_SEMAPHORE_WAIT_TIME_MS );

static TaskHandle_t xTaskToNotify = NULL;
static BaseType_t xIsWiFiInitialized = pdFALSE;

static char pcLastAttemptedSSID[ wificonfigMAX_SSID_LEN + 1 ] = { 0 };

/*-----------------------------------------------------------*/

/**
 * @brief Function to set a memory block to zero.
 * The function sets memory to zero using a volatile pointer so that compiler
 * wont optimize out the function if the buffer to be set to zero is not used further.
 * 
 * @param pBuf Pointer to buffer to be set to zero
 * @param size Length of the buffer to be set zero
 */
static void prvMemzero( void * pBuf, size_t size )
{
    volatile uint8_t * pMem = pBuf;
    uint32_t i;

    for( i = 0U; i < size; i++ )
    {
        pMem[ i ] = 0U;
    }
}

/*-----------------------------------------------------------*/

espr_t esp_callback_func(esp_cb_t* cb)
{
  switch (cb->type) {
    case ESP_CB_INIT_FINISH:
    {
      //configPRINT("Library initialized!\r\n");
      break;
    }

    case ESP_CB_RESET_FINISH:
    {
      //configPRINT("Device reset sequence finished!\r\n");
      break;
    }

    case ESP_CB_RESET:
    {
      //configPRINT("Device reset detected!\r\n");
      break;
    }

    case ESP_CB_WIFI_CONNECTED:
    {
      xTaskNotify(xTaskToNotify, CONNECT_BIT, eSetValueWithOverwrite);
      break;
    }

    case ESP_CB_WIFI_DISCONNECTED:
    {
      //configPRINT("Wifi Disconnected\r\n");
      //xSemaphoreGive(xWiFiConnectSemaphoreHandle);
      xTaskNotify(xTaskToNotify, DISCONNECT_BIT, eSetValueWithOverwrite);
      break;
    }
	default: break;
  }
  return espOK;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_On( void )
{
  if (xIsWiFiInitialized == pdFALSE)
  {
    if (esp_init(esp_callback_func, 1) != espOK)
    {
      return eWiFiFailure;
    }

    xWiFiSemaphoreHandle = xSemaphoreCreateBinary();
    if (xWiFiSemaphoreHandle == NULL)
    {
      return eWiFiFailure;
    }
    xSemaphoreGive(xWiFiSemaphoreHandle);

    xIsWiFiInitialized = pdTRUE;
  }

  return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Off( void )
{
  return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConnectAP( const WIFINetworkParams_t * const pxNetworkParams )
{
  BaseType_t xResult;
  uint32_t ulNotifiedValue;
  WIFIReturnCode_t status = eWiFiFailure;

  if ((pxNetworkParams == NULL) || (pxNetworkParams->ucSSIDLength == 0) 
      || ( pxNetworkParams->ucSSIDLength > wificonfigMAX_SSID_LEN ) )
  {
	return eWiFiFailure;
  }

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
    /* Store the handle of the calling task. */
    xTaskToNotify = xTaskGetCurrentTaskHandle();

    if (esp_sta_has_ip())
	{
	  if (esp_sta_quit(1) == espOK)
	  {
	      xResult = xTaskNotifyWait( pdFALSE,    /* Don't clear bits on entry. */
	                                 ULONG_MAX,        /* Clear all bits on exit. */
	                                 &ulNotifiedValue, /* Stores the notified value. */
									 xSemaphoreWaitTicks );

	      if (xResult == pdTRUE)
	      {
	    	if (ulNotifiedValue == DISCONNECT_BIT)
	    	{
	          status = eWiFiSuccess;
	        }
	      }
	  }
	}

    char pcSSID[pxNetworkParams->ucSSIDLength + 1];
    memcpy(pcSSID, pxNetworkParams->ucSSID, pxNetworkParams->ucSSIDLength);
    pcSSID[pxNetworkParams->ucSSIDLength] = '\0';

    char pcPassword[ wificonfigMAX_PASSPHRASE_LEN + 1];
    espr_t xReturn = espOK;

    memcpy(pcPassword, pxNetworkParams->xPassword.xWPA.cPassphrase, pxNetworkParams->xPassword.xWPA.ucLength);
    pcPassword[pxNetworkParams->xPassword.xWPA.ucLength] = '\0';

    strncpy(pcLastAttemptedSSID, (char *)pxNetworkParams->ucSSID, pxNetworkParams->ucSSIDLength + 1);

    xReturn = esp_sta_join(pcSSID, pcPassword, NULL, 0, 1);
    /* Use a private function to reset the memory block instead of memset, so that compiler wont optimize away the function call. */
    prvMemzero( pcPassword, sizeof( pcPassword ) );

    if (xReturn == espOK)
    {
      xResult = xTaskNotifyWait( pdFALSE,    /* Don't clear bits on entry. */
                                 ULONG_MAX,        /* Clear all bits on exit. */
                                 &ulNotifiedValue, /* Stores the notified value. */
								 xSemaphoreWaitTicks );

      if (xResult == pdTRUE)
      {
    	if (ulNotifiedValue == CONNECT_BIT)
    	{
          status = eWiFiSuccess;
        }
      }
    }

	/* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }

  return status;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Disconnect( void )
{
  BaseType_t xResult;
  uint32_t ulNotifiedValue;
  WIFIReturnCode_t status = eWiFiFailure;

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
	/* Store the handle of the calling task. */
	xTaskToNotify = xTaskGetCurrentTaskHandle();

	if (esp_sta_has_ip())
	{
      if (esp_sta_quit(1) == espOK)
      {
	      xResult = xTaskNotifyWait( pdFALSE,    /* Don't clear bits on entry. */
	                                 ULONG_MAX,        /* Clear all bits on exit. */
	                                 &ulNotifiedValue, /* Stores the notified value. */
									 xSemaphoreWaitTicks );

	      if (xResult == pdTRUE)
	      {
	    	if (ulNotifiedValue == DISCONNECT_BIT)
	    	{
	          status = eWiFiSuccess;
	        }
	      }
      }
	}
	else
	{
	  status = eWiFiSuccess;
	}
    /* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }

  return status;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Reset( void )
{
  WIFIReturnCode_t status = eWiFiFailure;

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
    if (esp_reset_with_delay(ESP_CFG_RESET_DELAY_DEFAULT, 1) == espOK)
    {
      status = eWiFiSuccess;
    }

    /* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }

  return status;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Scan( WIFIScanResult_t * pxBuffer,
                            uint8_t ucNumNetworks )
{
  WIFIReturnCode_t status = eWiFiFailure;

  if ((pxBuffer == NULL) || (ucNumNetworks == 0))
  {
	return eWiFiFailure;
  }

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
    esp_ap_t APs[ESP_CFG_MAX_DETECTED_AP];
	size_t found_aps = 0;

	if (ucNumNetworks > ESP_CFG_MAX_DETECTED_AP)
	{
	  ucNumNetworks = ESP_CFG_MAX_DETECTED_AP;
	}

	if (esp_sta_list_ap(NULL, APs, ucNumNetworks, &found_aps, 1) == espOK)
    {
   	  for (int32_t i = 0; i < ucNumNetworks; ++i)
   	  {
   		memcpy(pxBuffer[i].ucSSID, APs[i].ssid, wificonfigMAX_SSID_LEN);
   	    memcpy(pxBuffer[i].ucBSSID, APs[i].mac.mac, wificonfigMAX_BSSID_LEN);
   	    pxBuffer[i].cRSSI = APs[i].rssi;
   	    pxBuffer[i].xSecurity = APs[i].ecn;
   	    pxBuffer[i].ucChannel = APs[i].ch;
   	  }

      status = eWiFiSuccess;
    }

    /* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }

  return status;
}
/*-----------------------------------------------------------*/

__STATIC_INLINE esp_mode_t aws_mode_conversion(WIFIDeviceMode_t xDeviceMode)
{
  switch (xDeviceMode)
  {
#if ESP_CFG_MODE_STATION
    case eWiFiModeStation:
      return ESP_MODE_STA;
      break;
#endif

#if ESP_CFG_MODE_ACCESS_POINT
    case eWiFiModeAP:
      return ESP_MODE_AP;
      break;
#endif

#if ESP_CFG_MODE_STATION_ACCESS_POINT
    case eWiFiModeP2P:
      return ESP_MODE_STA_AP;
      break;
#endif

    default:
      return ESP_MODE_NOT_SUPPORTED;
      break;
  }
}

WIFIReturnCode_t WIFI_SetMode( WIFIDeviceMode_t xDeviceMode )
{
  WIFIReturnCode_t status = eWiFiFailure;

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
    esp_mode_t mode = aws_mode_conversion(xDeviceMode);
    if (mode == ESP_MODE_NOT_SUPPORTED)
    {
	  status = eWiFiNotSupported;
    }

    if (esp_set_wifi_mode(mode, 1) == espOK)
    {
      status = eWiFiSuccess;
    }

    /* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }

  return status;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMode( WIFIDeviceMode_t * pxDeviceMode )
{
  if (pxDeviceMode == NULL)
  {
	return eWiFiFailure;
  }

  *pxDeviceMode = eWiFiModeStation;
  return eWiFiSuccess;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkAdd( const WIFINetworkProfile_t * const pxNetworkProfile,
                                  uint16_t * pusIndex )
{
  return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkGet( WIFINetworkProfile_t * pxNetworkProfile,
                                  uint16_t usIndex )
{
  return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_NetworkDelete( uint16_t usIndex )
{
  return eWiFiNotSupported;
}

/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_Ping( uint8_t * pucIPAddr,
                            uint16_t usCount,
                            uint32_t ulIntervalMS )
{
  char host_name[20];
  WIFIReturnCode_t status = eWiFiSuccess;
  int ret;

  /* Check params */
  if ((pucIPAddr == NULL) || (usCount == 0))
  {
    return eWiFiFailure;
  }

  sprintf(host_name, "%d.%d.%d.%d", pucIPAddr[0], pucIPAddr[1], pucIPAddr[2], pucIPAddr[3]);

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
    for (int32_t i = 0; i < usCount; ++i)
    {
      uint32_t time;
      ret = esp_ping(host_name, &time, 1);
      if ( ret != espOK )
      {
  	    status = eWiFiFailure;
            ESP_CFG_DBG_OUT( "WIFI_Ping returning %d \r\n", ret );
  	    break;
      }

      vTaskDelay(pdMS_TO_TICKS(ulIntervalMS));
    }

    /* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }


  return status;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetIPInfo( WIFIIPConfiguration_t * xIPConfig )
{
  esp_ip_t gw;
  esp_ip_t nm;

  WIFIReturnCode_t status = eWiFiFailure;

  if (xIPConfig == NULL)
  {
	return eWiFiFailure;
  }

  memset(xIPConfig, 0, sizeof(WIFIIPConfiguration_t));

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
    if (esp_sta_getip((esp_ip_t *)&xIPConfig->xIPAddress.ulAddress[0], &gw, &nm, 0, 1) == espOK)
    {
      status = eWiFiSuccess;
    }

    /* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }

  return status;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetMAC( uint8_t * pucMac )
{
  WIFIReturnCode_t status = eWiFiFailure;

  if (pucMac == NULL)
  {
	return eWiFiFailure;
  }

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
    if (esp_sta_getmac((esp_mac_t *)pucMac, 0, 1) == espOK)
    {
      status = eWiFiSuccess;
    }

    /* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }

  return status;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetHostIP( char * pcHost,
                                 uint8_t * pucIPAddr )
{
  WIFIReturnCode_t status = eWiFiFailure;

  if ((pcHost == NULL) || (pucIPAddr == NULL))
  {
	return eWiFiFailure;
  }

  /* Acquire semaphore */
  if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
  {
    if (esp_dns_getbyhostname(pcHost, (esp_ip_t *)pucIPAddr, 1) == espOK)
    {
      status = eWiFiSuccess;
    }

    /* Release semaphore */
    xSemaphoreGive(xWiFiSemaphoreHandle);
  }
  else
  {
    status = eWiFiTimeout;
  }

  return status;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StartAP( void )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_StopAP( void )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_ConfigureAP( const WIFINetworkParams_t * const pxNetworkParams )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_SetPMMode( WIFIPMMode_t xPMModeType,
                                 const void * pvOptionValue )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

WIFIReturnCode_t WIFI_GetPMMode( WIFIPMMode_t * pxPMModeType,
                                 void * pvOptionValue )
{
    return eWiFiNotSupported;
}
/*-----------------------------------------------------------*/

BaseType_t WIFI_IsConnected(const WIFINetworkParams_t * const pxNetworkParams)
{
	BaseType_t status = pdFALSE;

	/* Acquire semaphore */
	if (xSemaphoreTake(xWiFiSemaphoreHandle, xSemaphoreWaitTicks) == pdTRUE)
	{

		if (esp_sta_has_ip())
		{
			if (pxNetworkParams && pxNetworkParams->ucSSIDLength > 0)
			{
				if (pxNetworkParams->ucSSIDLength == strnlen(pcLastAttemptedSSID, wificonfigMAX_SSID_LEN)
					&& (0 == memcmp(pxNetworkParams->ucSSID, pcLastAttemptedSSID, pxNetworkParams->ucSSIDLength)))
				{
					status = pdTRUE;
				}
			}
			else
			{
				status = pdTRUE;
			}
		}

		/* Release semaphore */
		xSemaphoreGive(xWiFiSemaphoreHandle);
	}

	return status;
}

WIFIReturnCode_t WIFI_RegisterEvent( WIFIEventType_t xEventType, WIFIEventHandler_t xHandler )
{
    /** Needs to implement dispatching network state change events **/
    return eWiFiNotSupported;
}
