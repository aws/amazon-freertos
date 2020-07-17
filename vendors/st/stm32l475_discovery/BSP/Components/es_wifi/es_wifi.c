/**
  ******************************************************************************
  * @file    es-wifi.c
  * @author  MCD Application Team
  * @date    2018-06-06
  * @brief   This file provides a set of functions to manage the es-wifi module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "es_wifi.h"

#define DEBUG  printf("%s:%d :",__FILE__,__LINE__);printf

#define AT_OK_STRING "\r\nOK\r\n> "
#define AT_OK_STRING_LEN (sizeof(AT_OK_STRING) - 1)

#define AT_ERROR_STRING "\r\nERROR"

#define AT_DELIMETER_STRING "\r\n> "
#define AT_DELIMETER_LEN        4

// This is version 3.5.2.5, with the byte order reversed for easy comparison
#define UPDATED_SCAN_PARAMETERS_FW_REV (0x05020503)

#define CHARISHEXNUM(x)                 (((x) >= '0' && (x) <= '9') || \
                                         ((x) >= 'a' && (x) <= 'f') || \
                                         ((x) >= 'A' && (x) <= 'F'))

#define CHARISNUM(x)                    ((x) >= '0' && (x) <= '9')
#define CHAR2NUM(x)                     ((x) - '0')
/* Private function prototypes -----------------------------------------------*/
static  uint8_t Hex2Num(char a);
static uint32_t ParseHexNumber(char* ptr, uint8_t* cnt);
static uint32_t ParseHexNumber(char* ptr, uint8_t* cnt);
static void ParseMAC(char* ptr, uint8_t* arr);
static void ParseIP(char* ptr, uint8_t* arr);
static ES_WIFI_SecurityType_t ParseSecurity(char* ptr);
#if (ES_WIFI_USE_UART == 1)
static void AT_ParseUARTConfig(char *pdata, ES_WIFI_UARTConfig_t *pConfig);
#endif
static void AT_ParseSystemConfig(char *pdata, ES_WIFI_SystemConfig_t *pConfig);
static void AT_ParseConnSettings(char *pdata, ES_WIFI_Network_t *NetSettings);
static void AT_ParseTransportSettings(char *pdata, ES_WIFI_Transport_t *TransportSettings);
static void AT_ParseIsConnected(char *pdata, uint8_t *isConnected);
static ES_WIFI_Status_t AT_ExecuteCommand(ES_WIFIObject_t *Obj, uint8_t* cmd, uint8_t *pdata);

uint32_t HAL_GetTick(void);
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Convert char in Hex format to integer.
  * @param  a: character to convert
  * @retval integer value.
  */

static  uint8_t Hex2Num(char a)
{
    if (a >= '0' && a <= '9') {                             /* Char is num */
        return a - '0';
    } else if (a >= 'a' && a <= 'f') {                      /* Char is lowercase character A - Z (hex) */
        return (a - 'a') + 10;
    } else if (a >= 'A' && a <= 'F') {                      /* Char is uppercase character A - Z (hex) */
        return (a - 'A') + 10;
    }

    return 0;
}

/**
  * @brief  Extract a hex number from a string.
  * @param  ptr: pointer to string
  * @param  cnt: pointer to the number of parsed digit
  * @retval Hex value.
  */
static uint32_t ParseHexNumber(char* ptr, uint8_t* cnt)
{
    uint32_t sum = 0;
    uint8_t i = 0;

    while (CHARISHEXNUM(*ptr)) {                    		/* Parse number */
        sum <<= 4;
        sum += Hex2Num(*ptr);
        ptr++;
        i++;
    }

    if (cnt != NULL) {                               		/* Save number of characters used for number */
        *cnt = i;
    }
    return sum;                                        		/* Return number */
}

/**
  * @brief  Parses and returns number from string.
  * @param  ptr: pointer to string
  * @param  cnt: pointer to the number of parsed digit
  * @retval integer value.
  */
static int32_t ParseNumber(char* ptr, uint8_t* cnt)
{
    uint8_t minus = 0, i = 0;
    int32_t sum = 0;

    if (*ptr == '-') {                                		/* Check for minus character */
        minus = 1;
        ptr++;
        i++;
    }
    while (CHARISNUM(*ptr)) {                        		/* Parse number */
        sum = 10 * sum + CHAR2NUM(*ptr);
        ptr++;
        i++;
    }
    if (cnt != NULL) {                                		/* Save number of characters used for number */
        *cnt = i;
    }
    if (minus) {                                    		/* Minus detected */
        return 0 - sum;
    }
    return sum;                                       		/* Return number */
}

/**
  * @brief  Parses and returns MAC address.
  * @param  ptr: pointer to string
  * @param  arr: pointer to MAC array
  * @retval None.
  */
static void ParseMAC(char* ptr, uint8_t* arr)
{
  uint8_t hexnum = 0, hexcnt;

  while(* ptr) {
    hexcnt = 1;
    if(*ptr != ':')
    {
      arr[hexnum++] = ParseHexNumber(ptr, &hexcnt);
    }
    ptr = ptr + hexcnt;
  }
}

/**
  * @brief  Parses and returns IP address.
  * @param  ptr: pointer to string
  * @param  arr: pointer to IP array
  * @retval None.
  */
static  void ParseIP(char* ptr, uint8_t* arr)
{
  uint8_t hexnum = 0, hexcnt;

  while(* ptr) {
    hexcnt = 1;
    if(*ptr != '.')
    {
      arr[hexnum++] = ParseNumber(ptr, &hexcnt);
    }
    ptr = ptr + hexcnt;
  }
}

/**
  * @brief  Parses Security type.
  * @param  ptr: pointer to string
  * @retval Encryption type.
  */
static ES_WIFI_SecurityType_t ParseSecurity(char* ptr)
{
  if(strstr(ptr,"Open")) return ES_WIFI_SEC_OPEN;
  else if(strstr(ptr,"WEP")) return ES_WIFI_SEC_WEP;
  else if(strstr(ptr,"WPA WPA2")) return ES_WIFI_SEC_WPA_WPA2;
  else if(strstr(ptr,"WPA2 TKIP")) return ES_WIFI_SEC_WPA2_TKIP;
  else if(strstr(ptr,"WPA2")) return ES_WIFI_SEC_WPA2;
  else if(strstr(ptr,"WPA")) return ES_WIFI_SEC_WPA;
  else return ES_WIFI_SEC_UNKNOWN;
}

/**
  * @brief  Parses ES module informations and save them in the handle.
  * @param  Obj: pointer to module handle
  * @param  ptr: pointer to string
  * @retval None.
  */
static void AT_ParseInfo(ES_WIFIObject_t *Obj,uint8_t *pdata)
{
  char *ptr;
  uint8_t num = 0;

  ptr = strtok((char *)pdata + 2, ",");

  while (ptr != NULL){
    switch (num++) {
    case 0:
      strncpy((char *)Obj->Product_ID,  ptr, ES_WIFI_PRODUCT_ID_SIZE);
      break;

    case 1:
      strncpy((char *)Obj->FW_Rev,  ptr, ES_WIFI_FW_REV_SIZE );
      break;

    case 2:
      strncpy((char *)Obj->API_Rev,  ptr, ES_WIFI_API_REV_SIZE);
      break;

    case 3:
      strncpy((char *)Obj->Stack_Rev,  ptr, ES_WIFI_STACK_REV_SIZE);
      break;

    case 4:
      strncpy((char *)Obj->RTOS_Rev,  ptr, ES_WIFI_RTOS_REV_SIZE);
      break;

    case 5:
      Obj->CPU_Clock = ParseNumber(ptr, NULL);
      break;

    case 6:
      ptr = strtok(ptr, "\r");
      strncpy((char *)Obj->Product_Name,  ptr, ES_WIFI_PRODUCT_NAME_SIZE);
      break;

    default: break;
    }
    ptr = strtok(NULL, ",");
  }
}

/**
  * @brief  Parses Access point configuration.
  * @param  APs: Access points structure
  * @param  ptr: pointer to string
  * @retval None.
  */
static void AT_ParseAP(char *pdata, ES_WIFI_APs_t *APs)
{
  uint8_t num = 0;
  char *ptr;
  APs->nbr = 0;

  ptr = strtok(pdata + 2, ",");

  while ((ptr != NULL) && (APs->nbr < ES_WIFI_MAX_DETECTED_AP)) {
    switch (num++) {
    case 0: /* Ignore index */
    case 4: /* Ignore Max Rate */
    case 5: /* Ignore Network Type */
    case 7: /* Ignore Radio Band */
      break;

    case 1:
      ptr[strlen(ptr) - 1] = 0;
      strncpy((char *)APs->AP[APs->nbr].SSID,  ptr+ 1, ES_WIFI_MAX_SSID_NAME_SIZE + 1);
      break;

    case 2:
      ParseMAC(ptr, APs->AP[APs->nbr].MAC);
      break;

    case 3:
      APs->AP[APs->nbr].RSSI = ParseNumber(ptr, NULL);
      break;

    case 6:
      APs->AP[APs->nbr].Security = ParseSecurity(ptr);
      break;

    case 8:
      APs->AP[APs->nbr].Channel = ParseNumber(ptr, NULL);
      APs->nbr++;
      num = 1;
      break;

    default:
      break;
    }
    ptr = strtok(NULL, ",");
  }
}

uint32_t ReverseByteOrder32bit(uint8_t * buf)
{
	return ((buf[0] << 0) | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24));
}

/**
  * @brief  Parses FW revision
  * @param  ver: version array (4 position 8-bit array)
  * @param  pdata: pointer to string
  * @retval None.
  */
static void AT_ParseFWRev(char *pdata, uint8_t *ver)
{
  uint8_t num = 0;
  char *ptr;

  ptr = strtok(pdata + 1, ".");

  while ((ptr != NULL) && (num < 4)) {
    switch (num) {
    case 0:
    case 1:
    case 2:
    case 3:
    {
      ver[num] = atoi(ptr);

      break;
    }
    default:
      break;
    }
    num++;
    ptr = strtok(NULL, ".");
  }
}

/**
  * @brief  Parses Access point configuration.
  * @param  APs: Access points structure
  * @param  ptr: pointer to string
  * @retval None.
  */
static void AT_ParseSingleAP(char *pdata, ES_WIFI_AP_t *AP)
{
  uint8_t num = 0;
  char *ptr;

  ptr = strtok(pdata + 2, ",");

  while (ptr != NULL) {
    switch (num++) {
    case 0: /* Ignore index */
    case 4: /* Ignore Max Rate */
    case 5: /* Ignore Network Type */
    case 7: /* Ignore Radio Band */
      break;

    case 1:
      ptr[strlen(ptr) - 1] = 0;
      strncpy((char *)AP->SSID,  ptr+ 1, ES_WIFI_MAX_SSID_NAME_SIZE + 1);
      break;

    case 2:
      ParseMAC(ptr, AP->MAC);
      break;

    case 3:
      AP->RSSI = ParseNumber(ptr, NULL);
      break;

    case 6:
      AP->Security = ParseSecurity(ptr);
      break;

    case 8:
      AP->Channel = ParseNumber(ptr, NULL);
      num = 1;
      break;

    default:
      break;
    }
    ptr = strtok(NULL, ",");
  }
}

#if (ES_WIFI_USE_UART == 1)
/**
  * @brief  Parses UART configuration.
  * @param  APs: UART Config structure
  * @param  ptr: pointer to string
  * @retval None.
  */
static void AT_ParseUARTConfig(char *pdata, ES_WIFI_UARTConfig_t *pConfig)
{
  uint8_t num = 0;
  char *ptr;

  ptr = strtok(pdata + 2, ",");

  while (ptr != NULL) {
    switch (num++) {
    case 0:
      pConfig->Port = ParseNumber(ptr, NULL);
      break;

    case 1:
      pConfig->BaudRate = ParseNumber(ptr, NULL);
      break;

    case 2:
      pConfig->DataWidth = ParseNumber(ptr, NULL);
      break;

    case 3:
      pConfig->Parity = ParseNumber(ptr, NULL);
      break;

    case 4:
      pConfig->StopBits = ParseNumber(ptr, NULL);
      break;

    case 5:
      pConfig->Mode = ParseNumber(ptr, NULL);
      break;

    default:
      break;
    }
    ptr = strtok(NULL, ",");
  }
}
#endif

/**
  * @brief  Parses System configuration.
  * @param  APs: System Config structure
  * @param  ptr: pointer to string
  * @retval None.
  */
static void AT_ParseSystemConfig(char *pdata, ES_WIFI_SystemConfig_t *pConfig)
{
  uint8_t num = 0;
  char *ptr;

  ptr = strtok(pdata + 2, ",");

  while (ptr != NULL) {
    switch (num++) {
    case 0:
      pConfig->Configuration = ParseNumber(ptr, NULL);
      break;

    case 1:
      pConfig->WPSPin = ParseNumber(ptr, NULL);
      break;

    case 2:
      pConfig->VID = ParseNumber(ptr, NULL);
      break;

    case 3:
      pConfig->PID = ParseNumber(ptr, NULL);
      break;

    case 4:
      ParseMAC(ptr, pConfig->MAC);
      break;

    case 5:
      ParseIP(ptr, pConfig->AP_IPAddress);
      break;

    case 6:
      pConfig->PS_Mode = ParseNumber(ptr, NULL);
      break;

    case 7:
      pConfig->RadioMode = ParseNumber(ptr, NULL);
      break;

    case 8:
      pConfig->CurrentBeacon = ParseNumber(ptr, NULL);
      break;

    case 9:
      pConfig->PrevBeacon = ParseNumber(ptr, NULL);
      break;

    case 10:
      pConfig->ProductName = ParseNumber(ptr, NULL);
      break;

    default:
      break;
    }
    ptr = strtok(NULL, ",");
  }
}


/**
  * @brief  Parses WIFI connection settings.
  * @param  NetSettings: settings
  * @param  pdata: pointer to data
  * @retval None.
  */
static void AT_ParseConnSettings(char *pdata, ES_WIFI_Network_t *NetSettings)
{
  uint8_t num = 0;
  char *ptr;

  ptr = strtok(pdata + 2, ",");

  while (ptr != NULL) {
    switch (num++) {
    case 0:
      strncpy((char *)NetSettings->SSID,  ptr, ES_WIFI_MAX_SSID_NAME_SIZE + 1);
      break;

    case 1:
      strncpy((char *)NetSettings->pswd,  ptr, ES_WIFI_MAX_PSWD_NAME_SIZE + 1);
      break;

    case 2:
        NetSettings->Security = (ES_WIFI_SecurityType_t)ParseNumber(ptr, NULL);
        break;

    case 3:
      NetSettings->DHCP_IsEnabled = ParseNumber(ptr, NULL);
      break;

    case 4:
      NetSettings->IP_Ver = (ES_WIFI_IPVer_t)ParseNumber(ptr, NULL);
      break;

    case 5:
      ParseIP(ptr, NetSettings->IP_Addr);
      break;

    case 6:
      ParseIP(ptr, NetSettings->IP_Mask);
      break;

    case 7:
      ParseIP(ptr, NetSettings->Gateway_Addr);
      break;

    case 8:
      ParseIP(ptr, NetSettings->DNS1);
      break;

    case 9:
      ParseIP(ptr, NetSettings->DNS2);
      break;

    case 10:
      NetSettings->JoinRetries = ParseNumber(ptr, NULL);
      break;

    case 11:
      NetSettings->AutoConnect = ParseNumber(ptr, NULL);
      break;

    default:
      break;
    }
    ptr = strtok(NULL, ",");
    if ((ptr != NULL) && (ptr[-1] == ','))
    { /* Ignore empty fields */
      num++;
    }
  }
}


/**
  * @brief  Parses WIFI transport settings.
  * @param  TransportSettings: settings
  * @param  pdata: pointer to data
  * @retval None.
  */
static void AT_ParseTransportSettings(char *pdata, ES_WIFI_Transport_t *TransportSettings)
{
  uint8_t num = 0;
  char *ptr;

  ptr = strtok(pdata + 2, ",");

  while (ptr != NULL) {
    switch (num++) {
    case 0:
      TransportSettings->Protocol = (ES_WIFI_ConnType_t) ParseNumber(ptr, NULL);
      break;

    case 1:
      ParseIP(ptr, TransportSettings->Local_IP_Addr);
      break;

    case 2:
      TransportSettings->Local_Port = ParseNumber(ptr, NULL);
      break;

    case 3:
      ParseIP(ptr, TransportSettings->Remote_IP_Addr);
      break;

    case 4:
      TransportSettings->Remote_Port = ParseNumber(ptr, NULL);
      break;

    case 5:
      TransportSettings->TCP_Server = ParseNumber(ptr, NULL);
      break;

    case 6:
      TransportSettings->UDP_Server = ParseNumber(ptr, NULL);
      break;

    case 7:
      TransportSettings->TCP_Backlogs = ParseNumber(ptr, NULL);
      break;

    case 8:
      TransportSettings->Accept_Loop = ParseNumber(ptr, NULL);
      break;

    case 9:
      TransportSettings->Read_Mode = ParseNumber(ptr, NULL);
      break;

    default:
      break;
    }
    ptr = strtok(NULL, ",");
    if ((ptr != NULL) && (ptr[-1] == ','))
    { /* Ignore empty fields */
      num++;
    }
  }
}


/**
  * @brief  Parses the connection status
  * @param  pdata: pointer to data
  * @param  isConnected: pointer to result
  * @retval None.
  */
static void AT_ParseIsConnected(char *pdata, uint8_t *isConnected)
{
  *isConnected = (pdata[2] == '1') ? 1 : 0;
}

/**
  * @brief  Execute AT command.
  * @param  Obj: pointer to module handle
  * @param  cmd: pointer to command string
  * @param  pdata: pointer to returned data
  * @retval Operation Status.
  */
static ES_WIFI_Status_t AT_ExecuteCommand(ES_WIFIObject_t *Obj, uint8_t* cmd, uint8_t *pdata)
{
  int ret = 0;
  int16_t recv_len = 0;
  LOCK_WIFI();  

  ret = Obj->fops.IO_Send(cmd, strlen((char*)cmd), Obj->Timeout);

  if( ret > 0)
  {
    recv_len = Obj->fops.IO_Receive(pdata, ES_WIFI_DATA_SIZE, Obj->Timeout);
    if((recv_len > 0) && (recv_len < ES_WIFI_DATA_SIZE))
    {
      *(pdata + recv_len) = 0;
      if(strstr((char *)pdata, AT_OK_STRING))
      {
        UNLOCK_WIFI();
        return ES_WIFI_STATUS_OK;
      }
      else if(strstr((char *)pdata, AT_ERROR_STRING))
      {
        UNLOCK_WIFI();
        return ES_WIFI_STATUS_UNEXPECTED_CLOSED_SOCKET;
      }
    }
    if (recv_len == ES_WIFI_ERROR_STUFFING_FOREVER ) 
    {
      UNLOCK_WIFI();
      return ES_WIFI_STATUS_MODULE_CRASH;
    }
  }
  UNLOCK_WIFI();  
  return ES_WIFI_STATUS_IO_ERROR;
}

/**
  * @brief  Execute AT command with data.
  * @param  Obj: pointer to module handle
  * @param  cmd: pointer to command string
  * @param  pcmd_data: pointer to binary data
  * @param  len: binary data length
  * @param  pdata: pointer to returned data
  * @retval Operation Status.
  */

static ES_WIFI_Status_t AT_RequestSendData(ES_WIFIObject_t *Obj, uint8_t* cmd, uint8_t *pcmd_data, uint16_t len, uint8_t *pdata)
{
  int16_t send_len = 0;
  int16_t recv_len = 0;
  uint16_t cmd_len = 0;
  uint16_t n ;

  LOCK_WIFI();  
  cmd_len = strlen((char*)cmd);

  /* can send only even number of byte on first send */
  if (cmd_len & 1) return ES_WIFI_STATUS_ERROR;
  n=Obj->fops.IO_Send(cmd, cmd_len, Obj->Timeout);
  if (n == cmd_len)
  {    
    send_len = Obj->fops.IO_Send(pcmd_data, len, Obj->Timeout);
    if (send_len == len)
    {
      recv_len = Obj->fops.IO_Receive(pdata, 0, Obj->Timeout);
      if (recv_len > 0)
      {
        *(pdata+recv_len) = 0;
        if(strstr((char *)pdata, AT_OK_STRING))
        {
          UNLOCK_WIFI();
          return ES_WIFI_STATUS_OK;
        }
        else if(strstr((char *)pdata, AT_ERROR_STRING))
        {
          UNLOCK_WIFI();
          return ES_WIFI_STATUS_UNEXPECTED_CLOSED_SOCKET;
        }
        else
        {
          UNLOCK_WIFI();
          return ES_WIFI_STATUS_ERROR;
        }
      }
      UNLOCK_WIFI();
      if (recv_len == ES_WIFI_ERROR_STUFFING_FOREVER ) 
      {
        return ES_WIFI_STATUS_MODULE_CRASH;
      }
      return ES_WIFI_STATUS_ERROR;
    }
    else
    {
      return ES_WIFI_STATUS_ERROR;
    }
  }
  return ES_WIFI_STATUS_IO_ERROR;
}


/**
  * @brief  Parses Received data.
  * @param  Obj: pointer to module handle
  * @param  cmd:command formatted string
  * @param  pdata: payload
  * @param  Reqlen : requested Data length.
  * @param  ReadData : pointer to received data length.
  * @retval Operation Status.
  */
static ES_WIFI_Status_t AT_RequestReceiveData(ES_WIFIObject_t *Obj, uint8_t* cmd, char *pdata, uint16_t Reqlen, uint16_t *ReadData)
{
  int len;
  uint8_t *p=Obj->CmdData;
  
  LOCK_WIFI();  
  if(Obj->fops.IO_Send(cmd, strlen((char*)cmd), Obj->Timeout) > 0)
  {
    len = Obj->fops.IO_Receive(p, 0 , Obj->Timeout);
    if ((p[0]!='\r') || (p[1]!='\n'))
    {
     return  ES_WIFI_STATUS_IO_ERROR;
    }
    len-=2;
    p+=2;
    if (len >= AT_OK_STRING_LEN)
    {
     while(len && (p[len-1]==0x15)) len--;
     p[len] = '\0';
     if(strstr( (char*) p + len - AT_OK_STRING_LEN, AT_OK_STRING))
     {
       *ReadData = len - AT_OK_STRING_LEN;
	   if (*ReadData > Reqlen)
       {
         *ReadData = Reqlen;
       }
       memcpy(pdata, p, *ReadData);
       UNLOCK_WIFI();
       return ES_WIFI_STATUS_OK;
     }
     else if(memcmp((char *)p + len - AT_DELIMETER_LEN , AT_DELIMETER_STRING, AT_DELIMETER_LEN) == 0)
     {
       *ReadData = 0;
       UNLOCK_WIFI();
       return ES_WIFI_STATUS_UNEXPECTED_CLOSED_SOCKET;
     }
     
     UNLOCK_WIFI();
     *ReadData = 0;
     return ES_WIFI_STATUS_UNEXPECTED_CLOSED_SOCKET;
   }
   if (len == ES_WIFI_ERROR_STUFFING_FOREVER ) 
   {
     UNLOCK_WIFI();
     return ES_WIFI_STATUS_MODULE_CRASH;
   }
  }
  UNLOCK_WIFI();
  return ES_WIFI_STATUS_IO_ERROR;
}


/**
  * @brief  Initialize WIFI module.
  * @param  Obj: pointer to module handle
  * @retval Operation Status.
  */
ES_WIFI_Status_t  ES_WIFI_Init(ES_WIFIObject_t *Obj)
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;

  Obj->Timeout = ES_WIFI_TIMEOUT;

  if (Obj->fops.IO_Init(ES_WIFI_INIT) == 0)
  {
    LOCK_WIFI();

    ret = AT_ExecuteCommand(Obj,(uint8_t*)"I?\r\n", Obj->CmdData);

    if(ret == ES_WIFI_STATUS_OK)
    {
      AT_ParseInfo (Obj, Obj->CmdData);
    }

    UNLOCK_WIFI();
  }
  return ret;
}

/**
  * @brief  Return ProductID.
  * @param  Obj: pointer to module handle
  * @param  productID: pointer productID
  * @retval Operation Status.
  */
ES_WIFI_Status_t  ES_WIFI_GetProductID(ES_WIFIObject_t *Obj, uint8_t *productID)
{
  strncpy((char *)productID, (char *)Obj->Product_ID, ES_WIFI_PRODUCT_ID_SIZE);
  return ES_WIFI_STATUS_OK;
}

/**
  * @brief  Return Firmware Revision.
  * @param  Obj: pointer to module handle
  * @param  productID: pointer Revision
  * @retval Operation Status.
  */
ES_WIFI_Status_t  ES_WIFI_GetFWRevID(ES_WIFIObject_t *Obj, uint8_t *FWRev)
{
  strncpy((char *)FWRev, (char *)Obj->FW_Rev, ES_WIFI_FW_REV_SIZE);
  return ES_WIFI_STATUS_OK;
}

/**
  * @brief  Return product Name.
  * @param  Obj: pointer to module handle
  * @param  productID: pointer product Name
  * @retval Operation Status.
  */
ES_WIFI_Status_t  ES_WIFI_GetProductName(ES_WIFIObject_t *Obj, uint8_t *productName)
{
  strncpy((char *)productName, (char *)Obj->Product_Name, ES_WIFI_PRODUCT_NAME_SIZE);
  return ES_WIFI_STATUS_OK;
}

/**
  * @brief  Return API revision.
  * @param  Obj: pointer to module handle
  * @param  productID: pointer API revision.
  * @retval Operation Status.
  */
ES_WIFI_Status_t  ES_WIFI_GetAPIRev(ES_WIFIObject_t *Obj, uint8_t *APIRev)
{
  strncpy((char *)APIRev, (char *)Obj->API_Rev, ES_WIFI_API_REV_SIZE);
  return ES_WIFI_STATUS_OK;
}

/**
  * @brief  Return Stack Revision.
  * @param  Obj: pointer to module handle
  * @param  productID: pointer Stack Revision
  * @retval Operation Status.
  */
ES_WIFI_Status_t  ES_WIFI_GetStackRev(ES_WIFIObject_t *Obj, uint8_t *StackRev)
{
  strncpy((char *)StackRev, (char *)Obj->Stack_Rev, ES_WIFI_STACK_REV_SIZE);
  return ES_WIFI_STATUS_OK;
}

/**
  * @brief  Return RTOS Revision
  * @param  Obj: pointer to module handle
  * @param  productID: pointer RTOS Revision
  * @retval Operation Status.
  */
ES_WIFI_Status_t  ES_WIFI_GetRTOSRev(ES_WIFIObject_t *Obj, uint8_t *RTOSRev)
{
  strncpy((char *)RTOSRev, (char *)Obj->RTOS_Rev, ES_WIFI_RTOS_REV_SIZE);
  return ES_WIFI_STATUS_OK;
}


/**
  * @brief  Initialize WIFI module.
  * @param  Obj: pointer to module handle
  * @retval Operation Status.
  */
ES_WIFI_Status_t  ES_WIFI_RegisterBusIO(ES_WIFIObject_t *Obj, IO_Init_Func IO_Init,
                                                              IO_DeInit_Func  IO_DeInit,
                                                              IO_Delay_Func   IO_Delay,
                                                              IO_Send_Func    IO_Send,
                                                              IO_Receive_Func  IO_Receive)
{
  if(!Obj || !IO_Init || !IO_DeInit || !IO_Send || !IO_Receive)
  {
    return ES_WIFI_STATUS_ERROR;
  }

  Obj->fops.IO_Init = IO_Init;
  Obj->fops.IO_DeInit = IO_DeInit;
  Obj->fops.IO_Send = IO_Send;
  Obj->fops.IO_Receive = IO_Receive;
  Obj->fops.IO_Delay = IO_Delay;

  return ES_WIFI_STATUS_OK;
}

/**
  * @brief  Change default Timeout.
  * @param  Obj: pointer to module handle
  * @param  Timeout: Timeout in mS
  * @retval Operation Status.
  */
ES_WIFI_Status_t  ES_WIFI_SetTimeout(ES_WIFIObject_t *Obj, uint32_t Timeout)
{
  Obj->Timeout = Timeout;
  return ES_WIFI_STATUS_OK;
}

/**
  * @brief  List all detected APs.
  * @param  Obj: pointer to module handle
  * @param  APs: pointer Access points structure
  * @retval Operation Status.
  */
ES_WIFI_Status_t  ES_WIFI_ListAccessPoints(ES_WIFIObject_t *Obj, ES_WIFI_APs_t *APs)
{
  ES_WIFI_Status_t ret;
  int send_len;
  int16_t recv_len = 0;
  uint8_t version[4] = { 0 };
  LOCK_WIFI();

  sprintf((char*)Obj->CmdData, Obj->FW_Rev);

  AT_ParseFWRev(Obj->CmdData, version);

  if (ReverseByteOrder32bit(version) >= UPDATED_SCAN_PARAMETERS_FW_REV)
  {
    APs->nbr = 0;

    sprintf((char*)Obj->CmdData,"F0=2\r");

    send_len = Obj->fops.IO_Send(Obj->CmdData, strlen((char*)Obj->CmdData), Obj->Timeout);

    if (send_len == 5)
    {
  	  do
	  {
	    recv_len = Obj->fops.IO_Receive(Obj->CmdData, ES_WIFI_DATA_SIZE, Obj->Timeout);

        if((recv_len > 0) && (recv_len < ES_WIFI_DATA_SIZE))
	    {
          *(Obj->CmdData + recv_len) = 0;

	      if(strstr((char *)Obj->CmdData, AT_OK_STRING))
          {
		    UNLOCK_WIFI();
		    return ES_WIFI_STATUS_OK;
	      }
	      else if(strstr((char *)Obj->CmdData, AT_ERROR_STRING))
	      {
            UNLOCK_WIFI();
            return ES_WIFI_STATUS_UNEXPECTED_CLOSED_SOCKET;
	      }
	    }
	    if (recv_len == ES_WIFI_ERROR_STUFFING_FOREVER )
	    {
          UNLOCK_WIFI();
	      return ES_WIFI_STATUS_MODULE_CRASH;
	    }

        if (APs->nbr < ES_WIFI_MAX_DETECTED_AP)
        {
          AT_ParseSingleAP((char *)Obj->CmdData, &(APs->AP[APs->nbr]));
          APs->nbr++;
        }

        sprintf((char*)Obj->CmdData,"MR\r");

        send_len = Obj->fops.IO_Send(Obj->CmdData, strlen((char*)Obj->CmdData), Obj->Timeout);
	  } while (send_len == 3);
    }

    UNLOCK_WIFI();
    return ES_WIFI_STATUS_IO_ERROR;
  }
  else
  {
    ret = AT_ExecuteCommand(Obj,(uint8_t*)"F0\r", Obj->CmdData);
    if(ret == ES_WIFI_STATUS_OK)
    {
      AT_ParseAP((char *)Obj->CmdData, APs);
    }
    UNLOCK_WIFI();
    return ret;
  }
}

/**
  * @brief  Join an Access point.
  * @param  Obj: pointer to module handle
  * @param  Ssid: the access point id.
  * @param  Password: the Access point password.
  * @param  SecType: Security type.
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_Connect(ES_WIFIObject_t *Obj, const char* SSID,
                                         const char* Password,
                                         ES_WIFI_SecurityType_t SecType)
{
  ES_WIFI_Status_t ret;
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"C1=%s\r", SSID);
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"C2=%s\r", Password);
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

    if(ret == ES_WIFI_STATUS_OK)
    {
      Obj->Security = SecType;
      sprintf((char*)Obj->CmdData,"C3=%d\r", (uint8_t)SecType);
      ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

      if(ret == ES_WIFI_STATUS_OK)
      {
        sprintf((char*)Obj->CmdData,"C0\r");
        ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
        if(ret == ES_WIFI_STATUS_OK)
        {
           Obj->NetSettings.IsConnected = 1;
        }
      }
    }
  }
  UNLOCK_WIFI();
  return ret;
}

/**
  * @brief  Check whether the module is connected to an access point.
  * @retval Operation Status.
  */
uint8_t ES_WIFI_IsConnected(ES_WIFIObject_t *Obj)
{
  ES_WIFI_Status_t ret ;
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"CS\r");
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
    AT_ParseIsConnected((char *)Obj->CmdData, &(Obj->NetSettings.IsConnected));
  }
  UNLOCK_WIFI();
  return Obj->NetSettings.IsConnected;
}
/**
  * @brief  Disconnect from a network.
  * @param  Obj: pointer to module handle
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_Disconnect(ES_WIFIObject_t *Obj)
{
   ES_WIFI_Status_t ret;
   LOCK_WIFI();  
   sprintf((char*)Obj->CmdData,"CD\r");
   ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
   UNLOCK_WIFI();
   return  ret;
}
/**
  * @brief  Return network settings.
  * @param  Obj: pointer to module handle
  * @param  Pointer to network setting structure.
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_GetNetworkSettings(ES_WIFIObject_t *Obj)
{
  ES_WIFI_Status_t ret;
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"C?\r");
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

  if(ret == ES_WIFI_STATUS_OK)
  {
     AT_ParseConnSettings((char *)Obj->CmdData, &Obj->NetSettings);
  }
  
  UNLOCK_WIFI();
  return ret;
}

/**
  * @brief  Configure and activate SoftAP.
  * @param  Obj: pointer to module handle
  * @param  ApConfig : Pointer to AP config structure.
  * @param  ip : AP IP address
  * @param  mac : AP MAC address
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_ActivateAP(ES_WIFIObject_t *Obj, ES_WIFI_APConfig_t *ApConfig)
{
  ES_WIFI_Status_t ret;
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"AS=0,%s\r", ApConfig->SSID);
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {

    sprintf((char*)Obj->CmdData,"A1=%c\r", (int)ApConfig->Security + '0');
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
    if(ret == ES_WIFI_STATUS_OK)
    {

      sprintf((char*)Obj->CmdData,"A2=%s\r", ApConfig->Pass);
      ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
      if(ret == ES_WIFI_STATUS_OK)
      {

        sprintf((char*)Obj->CmdData,"AC=%d\r", ApConfig->Channel);
        ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
        if(ret == ES_WIFI_STATUS_OK)
        {

          sprintf((char*)Obj->CmdData,"AT=%d\r", ApConfig->MaxConnections);
          ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
          if(ret == ES_WIFI_STATUS_OK)
          {
            sprintf((char*)Obj->CmdData,"A0\r");
            ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
            if(ret == ES_WIFI_STATUS_OK)
            {
              if(strstr((char *)Obj->CmdData, "[AP     ]"))
              {
                ret = ES_WIFI_STATUS_OK;
              }
            }
          }
        }
      }
    }
  }
  UNLOCK_WIFI();
  return ret;
}

/**
  * @brief  Get AP notification.
  * @param  Obj: pointer to module handle
  * @param  ip : Assigned ip address.
  * @param  ip : joind device mac address.
  * @retval AP State.
  */
ES_WIFI_APState_t ES_WIFI_WaitAPStateChange(ES_WIFIObject_t *Obj)
{
  ES_WIFI_APState_t ret = ES_WIFI_AP_NONE;
  char *ptr;
  LOCK_WIFI();  

 #if (ES_WIFI_USE_UART == 1)
  if(Obj->fops.IO_Receive(Obj->CmdData, 0, Obj->Timeout) > 0)
  {
    if(strstr((char *)Obj->CmdData, AT_ERROR_STRING))
    {
      UNLOCK_WIFI();
      return ES_WIFI_AP_ERROR;
    }
#else
    do
    {
      sprintf((char*)Obj->CmdData,"MR\r");
      if(AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData) != ES_WIFI_STATUS_OK)
      {
        UNLOCK_WIFI();
        return ES_WIFI_AP_ERROR;
      }
#endif
    else  if(strstr((char *)Obj->CmdData, "[AP DHCP]"))
    {
      ptr = strtok((char *)Obj->CmdData + 2, " ");
      ptr = strtok(NULL, " ");
      ptr = strtok(NULL, " ");
      ptr = strtok(NULL, " ");
      ParseMAC((char *)ptr, Obj->APSettings.MAC_Addr);
      ptr = strtok(NULL, " ");
      ptr = strtok(NULL, "\r");
      ParseIP((char *)ptr, Obj->APSettings.IP_Addr);
      ret = ES_WIFI_AP_ASSIGNED;
#if (ES_WIFI_USE_SPI == 1)
      break;
#endif
    }
    else  if(strstr((char *)Obj->CmdData, "[JOIN   ]"))
    {
      ptr = strtok((char *)Obj->CmdData + 12, ",");
      strncpy((char *)Obj->APSettings.SSID, ptr, ES_WIFI_MAX_SSID_NAME_SIZE  );
      ptr = strtok(NULL, ",");
      ParseIP((char *)ptr, Obj->APSettings.IP_Addr);
      ret =  ES_WIFI_AP_JOINED;
#if (ES_WIFI_USE_SPI == 1)
      break;
#endif
    }
#if (ES_WIFI_USE_UART == 1)
    }
#else
	UNLOCK_WIFI();
    Obj->fops.IO_Delay(100);
	LOCK_WIFI();
  } while (1);
#endif
  UNLOCK_WIFI();
  return ret;
}

/**
  * @brief  retrn the MAC address of the es module.
  * @param  Obj: pointer to module handle
  * @param  mac: pointer to the MAC address array.
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_GetMACAddress(ES_WIFIObject_t *Obj, uint8_t *mac)
{
  ES_WIFI_Status_t ret ;
  char *ptr;
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"Z5\r");
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
    ptr = strtok((char *)(Obj->CmdData + 2), "\r\n");
    ParseMAC(ptr, mac) ;
  }
  UNLOCK_WIFI();
  return ret;
}

/**
  * @brief  retrn the IP address of the es module.
  * @param  Obj: pointer to module handle
  * @param  mac: pointer to the IP address array.
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_GetIPAddress(ES_WIFIObject_t *Obj, uint8_t *ipaddr)
{
  memcpy(ipaddr, Obj->NetSettings.IP_Addr, 4);
  return ES_WIFI_STATUS_OK;
}

/**
  * @brief  Set the MAC address of the es module.
  * @param  Obj: pointer to module handle
  * @param  mac: pointer to the MAC address array.
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_SetMACAddress(ES_WIFIObject_t *Obj, uint8_t *mac)
{
  ES_WIFI_Status_t ret ;
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"Z4=%X:%X:%X:%X:%X:%X\r",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5] );
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"Z1\r");
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }
  UNLOCK_WIFI();
  return ret;
}

/**
  * @brief  Reset To factory defaults.
  * @param  Obj: pointer to module handle
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_ResetToFactoryDefault(ES_WIFIObject_t *Obj)
{
  ES_WIFI_Status_t ret ;
  LOCK_WIFI();
  sprintf((char*)Obj->CmdData,"Z0\r");
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  UNLOCK_WIFI();
  return ret;
}

/**
  * @brief  Reset the module.
  * @param  Obj: pointer to module handle
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_ResetModule(ES_WIFIObject_t *Obj)
{
  int ret;
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"ZR\r");
  ret = Obj->fops.IO_Send(Obj->CmdData, strlen((char*)Obj->CmdData), Obj->Timeout);
#if (ES_WIFI_USE_UART == 0)
 if (ret==3)
  {
    // Fake received needed in case of SPI to unlock SPI and deselect NSS 
     Obj->fops.IO_Receive(Obj->CmdData, 0, 1);
  }
#endif
  UNLOCK_WIFI();
  return (ret > 0) ? ES_WIFI_STATUS_OK : ES_WIFI_STATUS_ERROR;
}


/**
  * @brief  Hard Reset the module.
  * @param  Obj: pointer to module handle
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_HardResetModule(ES_WIFIObject_t *Obj)
{
  int ret;
  LOCK_WIFI();  
  ret = Obj->fops.IO_Init(ES_WIFI_RESET);
  UNLOCK_WIFI();
  return (ret > 0) ? ES_WIFI_STATUS_OK : ES_WIFI_STATUS_ERROR;
}

/**
  * @brief  Set Product Name.
  * @param  Obj: pointer to module handle
  * @param  ProductName : pointer to product name string
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_SetProductName(ES_WIFIObject_t *Obj, uint8_t *ProductName)
{
  ES_WIFI_Status_t ret ;
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"ZN=%s\r",ProductName);
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"Z1\r");
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }
  UNLOCK_WIFI();
  return ret;
}

#if (ES_WIFI_USE_FIRMWAREUPDATE == 1)
/**
  * @brief  OTA Firmware Upgrade.
  * @param  Obj: pointer to module handle
  * @param  Upgrade link path
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_OTA_Upgrade(ES_WIFIObject_t *Obj, uint8_t *link)
{
  ES_WIFI_Status_t ret ;
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"Z0=%d\r%s",strlen((char *)link), (char *)link);
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  UNLOCK_WIFI();
  return ret;
}
#endif
#if (ES_WIFI_USE_UART == 1)
/**
  * @brief  Set UART Baud Rate.
  * @param  Obj: pointer to module handle
  * @param  UART BAud rate
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_SetUARTBaudRate(ES_WIFIObject_t *Obj, uint16_t BaudRate)
{
  ES_WIFI_Status_t ret ;
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"U2=%d\r", BaudRate);
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"U0\r");
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }
  UNLOCK_WIFI();
  return ret;
}

/**
  * @brief  Get UART Configuration.
  * @param  Obj: pointer to module handle
  * @param  pointer to UART config structure
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_GetUARTConfig(ES_WIFIObject_t *Obj, ES_WIFI_UARTConfig_t *pconf)
{
  ES_WIFI_Status_t ret ;
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"U?\r");
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
    AT_ParseUARTConfig((char *)Obj->CmdData, pconf);
  }
  UNLOCK_WIFI();
  return ret;
}
#endif

/**
  * @brief  Get System Configuration.
  * @param  Obj: pointer to module handle
  * @param  pointer to System config structure
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_GetSystemConfig(ES_WIFIObject_t *Obj, ES_WIFI_SystemConfig_t *pconf)
{
  ES_WIFI_Status_t ret ;
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"Z?\r");
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
    AT_ParseSystemConfig((char *)Obj->CmdData, pconf);
  }
  UNLOCK_WIFI();
  return ret;
}

#if (ES_WIFI_USE_PING == 1)
/**
  * @brief  Ping an IP address.
  * @param  Obj: pointer to module handle
  * @param  Ping: ping structure.
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_Ping(ES_WIFIObject_t *Obj, uint8_t *address, uint16_t count, uint16_t interval_ms)
{
  ES_WIFI_Status_t ret;
  uint8_t responseIpAddress[4];
  int rtt, valuesRead = 0, responseStart = 0;
  uint16_t responsesParsed = 0;
  LOCK_WIFI();

  sprintf((char*)Obj->CmdData,"T1=%d.%d.%d.%d\r", address[0],address[1],
          address[2],address[3]);
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

  if(ret == ES_WIFI_STATUS_OK)
  {

    sprintf((char*)Obj->CmdData,"T2=%d\r", count);
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

    if(ret == ES_WIFI_STATUS_OK)
    {
      sprintf((char*)Obj->CmdData,"T3=%d\r", interval_ms);
      ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

      if(ret == ES_WIFI_STATUS_OK)
      {
        sprintf((char*)Obj->CmdData,"T0=\r");
        ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
        if(ret == ES_WIFI_STATUS_OK)
        {
          /* If the T0 command is successfully executed, the Inventek module
           * returns one of the following:
           *
           * Success - \r\nIP0.IP1.IP2.IP3,RTT\r\nOK\r\n>
           * Timeout - \r\nIP0.IP1.IP2.IP3,Timeout\r\nOK\r\n>
           *
           * Where IP0..IP3 are the octacts of the IP address pinged and RTT is
           * the round trip time.
           *
           * Example success response - \r\n8.8.8.8,48\r\nOK\r\n>
           * Example timeout response - \r\n192.168.1.1,Timeout\r\nOK\r\n>
           * 
           * The "\r\nIP0.IP1.IP2.IP3,RTT" part is repeated in the response
           * the number of ping count times (specified with the T2 command).
           * 
           * The following check tries to read the 4 IP octacts and the RTT
           * value from the response - if we are able to successfully read all
           * the 5 values (i.e. sscanf returns 5), it is a success response. We
           * will not be able to read the RTT value in case of timeout as sscanf
           * will not be able to parse "Timeout" as integer. As a result,
           * valuesRead will not be 5 and it will be detected as timeout.
           */
          /* Assume timeout unless we find a successful response. */
          ret = ES_WIFI_STATUS_TIMEOUT;
          do
          {
            /* Parse the first response. */
            valuesRead = sscanf((char*)&(Obj->CmdData[responseStart]),
                                   "\r\n%hhu.%hhu.%hhu.%hhu,%d",
                                   &responseIpAddress[0],
                                   &responseIpAddress[1],
                                   &responseIpAddress[2],
                                   &responseIpAddress[3],
                                   &rtt);

            if(valuesRead == 5 &&
              responseIpAddress[0] == address[0] &&
              responseIpAddress[1] == address[1] &&
              responseIpAddress[2] == address[2] &&
              responseIpAddress[3] == address[3])
            {
              /* We found a successful ping response and so we can stop. */
              ret = ES_WIFI_STATUS_OK;
              break;
            }
            else
            {
              /* Move past the \r at the beginning of the current response so
               * that the loop below gets to the next \r which marks the start
               * of the next response. */
              responseStart++;

              /* Current ping response was not a successful ping - move to the
               * beginning of the next response. */
              while(Obj->CmdData[responseStart] != '\0' && Obj->CmdData[responseStart] != '\r')
              {
                responseStart++;
              }
            }

            responsesParsed++;
          } while(responsesParsed < count);
        }
      }
    }
  }
  UNLOCK_WIFI();
  return ret;
}
#endif
/**
  * @brief  DNS Lookup to get IP address .
  * @param  Obj: pointer to module handle
  * @param  url: Domain Name.
  * @param  ipaddress: IP address.
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_DNS_LookUp(ES_WIFIObject_t *Obj, const char *url, uint8_t *ipaddress)
{
  ES_WIFI_Status_t ret;
  char *ptr;
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"D0=%s\r", url);
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

  if(ret == ES_WIFI_STATUS_OK)
  {
    ptr = strtok((char *)Obj->CmdData + 2, "\r");
    ParseIP(ptr, ipaddress);
  }
  UNLOCK_WIFI();
  return ret;
}


/**
  * @brief  Configure and Start a Client connection.
  * @param  Obj: pointer to module handle
  * @param  conn: pointer to the connection structure
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_StartClientConnection(ES_WIFIObject_t *Obj, ES_WIFI_Conn_t *conn)
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_OK;
  
  if (conn->RemotePort == 0) return ES_WIFI_STATUS_ERROR;
  
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"P0=%d\r", conn->Number);
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

  if (ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"P1=%d\r", conn->Type);
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }

  if (ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"P2=%d\r", conn->LocalPort);
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }

  if (ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"P4=%d\r", conn->RemotePort);
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }

  if ((ret == ES_WIFI_STATUS_OK) && ((conn->Type == ES_WIFI_TCP_CONNECTION) || (conn->Type == ES_WIFI_TCP_SSL_CONNECTION)))
  {
    sprintf((char*)Obj->CmdData,"P3=%d.%d.%d.%d\r", conn->RemoteIP[0],conn->RemoteIP[1],
            conn->RemoteIP[2],conn->RemoteIP[3]);
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }

  if ((ret == ES_WIFI_STATUS_OK) && (conn->Type == ES_WIFI_TCP_SSL_CONNECTION))
  {
    sprintf((char*)Obj->CmdData,"P9=2\r");
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }

  if (ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"P6=1\r");
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }
  
  UNLOCK_WIFI();
  return ret;
}


/**
  * @brief  Stop Client connection.
  * @param  Obj: pointer to module handle
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_StopClientConnection(ES_WIFIObject_t *Obj, ES_WIFI_Conn_t *conn)
{
  ES_WIFI_Status_t ret;
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"P0=%d\r", conn->Number);
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

  if (ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"P6=0\r");
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }
  UNLOCK_WIFI();
  return ret;
}

#if (ES_WIFI_USE_AWS == 1)
/**
  * @brief  Configure and Start a AWS Client connection.
  * @param  Obj: pointer to module handle
  * @param  conn: pointer to the connection structure
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_StartAWSClientConnection(ES_WIFIObject_t *Obj, ES_WIFI_AWS_Conn_t *conn)
{

  ES_WIFI_Status_t ret;
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"P0=%d\r", conn->Number);
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

  if(ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"P1=%d\r", conn->Type);
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
    if(ret == ES_WIFI_STATUS_OK)
    {
      sprintf((char*)Obj->CmdData,"P4=%d\r", conn->RemotePort);
      ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

      if(ret == ES_WIFI_STATUS_OK)
      {
        sprintf((char*)Obj->CmdData,"PM=0,%s\r", conn->PublishTopic);
        ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
        if(ret == ES_WIFI_STATUS_OK)
        {
          if(ret == ES_WIFI_STATUS_OK)
          {
            sprintf((char*)Obj->CmdData,"PM=1,%s\r", conn->SubscribeTopic);
            ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
            if(ret == ES_WIFI_STATUS_OK)
            {

              sprintf((char*)Obj->CmdData,"PM=2,%d\r", conn->MQTTMode);
              ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
              if(ret == ES_WIFI_STATUS_OK)
              {
                sprintf((char*)Obj->CmdData,"PM=5,%s\r", conn->ClientID);
                ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
                if(ret == ES_WIFI_STATUS_OK)
                {
                  sprintf((char*)Obj->CmdData,"PM\r");
                  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
                  if(ret == ES_WIFI_STATUS_OK)
                  {
                    sprintf((char*)Obj->CmdData,"P6=1\r");
                    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  UNLOCK_WIFI();
  return ret;
}
#endif
/**
  * @brief  Configure and Start a Server.
  * @param  Obj: pointer to module handle
  * @param  conn: pointer to the connection structure
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_StartServerSingleConn(ES_WIFIObject_t *Obj, ES_WIFI_Conn_t *conn)
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_OK;
  LOCK_WIFI();    
#if (ES_WIFI_USE_UART == 1)
  char *ptr;
#endif
    
  sprintf((char*)Obj->CmdData,"P0=%d\r", conn->Number);
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  if(ret != ES_WIFI_STATUS_OK)
  {
    UNLOCK_WIFI();
    return ret;
  }
  
  if ((conn->Type != ES_WIFI_UDP_CONNECTION) && (conn->Type != ES_WIFI_UDP_LITE_CONNECTION))
  {
    sprintf((char*)Obj->CmdData,"PK=1,3000\r");
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }
  
  if(ret == ES_WIFI_STATUS_OK)
  {  
      sprintf((char*)Obj->CmdData,"P1=%d\r", conn->Type);
      ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
      if(ret == ES_WIFI_STATUS_OK)
      {
        sprintf((char*)Obj->CmdData,"P8=%d\r", conn->Backlog);
        ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
        if (ret == ES_WIFI_STATUS_OK)    
		    {	
		      sprintf((char*)Obj->CmdData,"P2=%d\r", conn->LocalPort);
          ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
          if (ret == ES_WIFI_STATUS_OK)
          {
            // multi accept mode
            sprintf((char*)Obj->CmdData,"P5=11\r");
            ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

 #if (ES_WIFI_USE_UART == 1)
            // should normally send out of the band 
            if(Obj->fops.IO_Receive(Obj->CmdData, 0, Obj->Timeout) > 0)
            {
              if(strstr((char *)Obj->CmdData, "Accepted"))
              {
                ptr = strtok((char *)Obj->CmdData + 2, " ");
                ptr = strtok(NULL, " ");
                ptr = strtok(NULL, " ");
                ptr = strtok(NULL, ":");
                ParseIP((char *)ptr, conn->RemoteIP);
                ret = ES_WIFI_STATUS_OK;
              }
            }
#endif    
          }
        }
      }
	}
  UNLOCK_WIFI();
  return ret;
}

/**
  * @brief  Wait for a client connection to the Server.
  * @param  Obj: pointer to module handle
  * @param  conn: pointer to the connection structure
  * @retval Operation Status.
  */
ES_WIFI_Status_t  ES_WIFI_WaitServerConnection(ES_WIFIObject_t *Obj,uint32_t timeout,ES_WIFI_Conn_t *conn)
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_OK;
  uint32_t	t;
  uint32_t	tlast;
  uint32_t	tstart;
  char *ptr;
  
  tstart=HAL_GetTick();
  tlast=tstart+timeout;
  if (tlast < tstart )
  {
	   tstart=0;
  }
  
  
  do
  {
#if (ES_WIFI_USE_UART == 0)
    // mandatory to flush MR async messages 
    memset(Obj->CmdData,0,sizeof(Obj->CmdData));
    sprintf((char*)Obj->CmdData,"MR\r");
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
    if(ret == ES_WIFI_STATUS_OK)
    {
      if((strstr((char *)Obj->CmdData, "[SOMA]")) && (strstr((char *)Obj->CmdData, "[EOMA]")))
      {
        if(strstr((char *)Obj->CmdData, "Accepted"))
        {
         // flushing out the MR messages
        }
        else if(!strstr((char *)Obj->CmdData,"[SOMA][EOMA]"))
        {
          DEBUG("Bad MR stntax msg %s\n", Obj->CmdData);
          UNLOCK_WIFI();
          return ES_WIFI_STATUS_ERROR;
        }
      }
    }
    else
    {
      DEBUG("MR command failed %s\n", Obj->CmdData);
      UNLOCK_WIFI();
      return ES_WIFI_STATUS_ERROR;
    }        
#endif
    
    memset(Obj->CmdData,0,sizeof(Obj->CmdData));
    sprintf((char*)Obj->CmdData,"P?\r");
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
    if(ret == ES_WIFI_STATUS_OK)
    {
      if(strncmp((char *)Obj->CmdData, "\r\n0,0.0.0.0,",12)!=0)
      {
        ptr = strtok((char *)Obj->CmdData + 2, ",");
        ptr = strtok(0, ","); //port
        ParseIP((char *)ptr, conn->RemoteIP);
        ptr = strtok(0, ","); //port
        conn->LocalPort=ParseNumber(ptr,0);
        ptr = strtok(0, ","); //ip
        ptr = strtok(0, ","); //remote port
        conn->RemotePort=ParseNumber(ptr,0);
	UNLOCK_WIFI();
	return ES_WIFI_STATUS_OK;
      }
    }
    else
    {
      DEBUG("P? command failed %s\n", Obj->CmdData);
      UNLOCK_WIFI();
      return ES_WIFI_STATUS_ERROR;
    }
    
    UNLOCK_WIFI();
    Obj->fops.IO_Delay(100);
    LOCK_WIFI();
    t = HAL_GetTick();
  } 
  while ((timeout==0) ||((t < tlast) || (t < tstart)));
  return ES_WIFI_STATUS_TIMEOUT;
}

/**
  * @brief  Close current server connection.
  * @param  Obj: pointer to module handle
  * @param  socket:  server socket
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_CloseServerConnection(ES_WIFIObject_t *Obj, int socket)
{
  ES_WIFI_Status_t ret;
  LOCK_WIFI();  
  sprintf((char*)Obj->CmdData,"P0=%d\r", socket);
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  if(ret != ES_WIFI_STATUS_OK)
  {
    DEBUG(" Can not select socket %s\n", Obj->CmdData);
    UNLOCK_WIFI();
    return ret;
  }
  
  sprintf((char*)Obj->CmdData,"P5=10\r");
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  if(ret != ES_WIFI_STATUS_OK)
  {
    DEBUG(" Open next failed %s\n", Obj->CmdData);
  }  
  
  UNLOCK_WIFI();
  return ret;
}



/**
  * @brief  Stop a Server.
  * @param  Obj: pointer to module handle
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_StopServerSingleConn(ES_WIFIObject_t *Obj, int socket)
{
  ES_WIFI_Status_t ret;
  LOCK_WIFI();  
  sprintf((char*)Obj->CmdData,"P0=%d\r", socket);
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  if(ret != ES_WIFI_STATUS_OK)
  {
    DEBUG("Selecting socket failed: %s\n", Obj->CmdData);
    UNLOCK_WIFI();
    return ret;  
  }
  
  sprintf((char*)Obj->CmdData,"P5=0\r");
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  if(ret != ES_WIFI_STATUS_OK)
  {
    DEBUG("Stopping server failed %s\n", Obj->CmdData);
    UNLOCK_WIFI();
    return ret;
  }
  
  UNLOCK_WIFI();
  return ret;
}


/**
  * @brief  Configure and Start a Server.
  * @param  Obj: pointer to module handle
  * @param  conn: pointer to the connection structure
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_StartServerMultiConn(ES_WIFIObject_t *Obj, ES_WIFI_Conn_t *conn)
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"PK=1,3000\r");
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"P0=%d\r", conn->Number);
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
    if(ret == ES_WIFI_STATUS_OK)
    {
      sprintf((char*)Obj->CmdData,"P1=%d\r", conn->Type);
      ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
      if(ret == ES_WIFI_STATUS_OK)
      {
        sprintf((char*)Obj->CmdData,"P2=%d\r", conn->LocalPort);
        ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
        if(ret == ES_WIFI_STATUS_OK)
        {
          sprintf((char*)Obj->CmdData,"P8=6\r");
          ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

          if(ret == ES_WIFI_STATUS_OK)
          {
            sprintf((char*)Obj->CmdData,"P5=1\r");
            ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

            if(ret == ES_WIFI_STATUS_OK)
            {
#if (ES_WIFI_USE_UART == 1)
              if(Obj->fops.IO_Receive(Obj->CmdData, 0, Obj->Timeout) > 0)
              { 
                if(strstr((char *)Obj->CmdData, "Accepted"))
                {
                  ret = ES_WIFI_STATUS_OK;
                }
              }           
#endif
            }
            if(ret == ES_WIFI_STATUS_OK)
            {
              sprintf((char*)Obj->CmdData,"P7=1\r");
              ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

            }
          }
        }
      }
    }
  }
  UNLOCK_WIFI();
  return ret;
}

/**
  * @brief  Stop a Server.
  * @param  Obj: pointer to module handle
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_StopServerMultiConn(ES_WIFIObject_t *Obj)
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;
  LOCK_WIFI();  

  /* close the socket handle for the current request. */
  sprintf((char*)Obj->CmdData,"P7=2\r");
  ret =  AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

  if(ret == ES_WIFI_STATUS_OK)
  {
    /*Get the next request out of the queue */
    sprintf((char*)Obj->CmdData,"P7=3\r");
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
    if(ret == ES_WIFI_STATUS_OK)
    {
#if (ES_WIFI_USE_UART == 1)
        if(Obj->fops.IO_Receive(Obj->CmdData, 0, Obj->Timeout) > 0)
		{
			if(strstr((char *)Obj->CmdData, "Accepted"))
			{
			  ret = ES_WIFI_STATUS_OK;
			}
		}
#else
		do
		{
			sprintf((char*)Obj->CmdData,"MR\r");
			if(AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData) == ES_WIFI_STATUS_OK)
			{ 
				if(strstr((char *)Obj->CmdData, "Accepted"))
				{
				  ret = ES_WIFI_STATUS_OK;
				  break;
				}
			}
		    else
	        {
		      break;
			}
		
			UNLOCK_WIFI();
			Obj->fops.IO_Delay(100);
			LOCK_WIFI();
		}
		while(1);
#endif	  
    }
  }
  UNLOCK_WIFI();
  return ret;
}
/**
  * @brief  Send an amount data over WIFI.
  * @param  Obj: pointer to module handle
  * @param  Socket: number of the socket
  * @param  pdata: pointer to data
  * @param  len : length of the data to be sent
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_SendData(ES_WIFIObject_t *Obj, uint8_t Socket, uint8_t *pdata, uint16_t Reqlen , uint16_t *SentLen , uint32_t Timeout)
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;
  LOCK_WIFI();
  if(Reqlen >= ES_WIFI_PAYLOAD_SIZE ) Reqlen= ES_WIFI_PAYLOAD_SIZE;

  *SentLen = Reqlen;
  sprintf((char*)Obj->CmdData,"P0=%d\r", Socket);
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"S2=%lu\r",Timeout);
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

    if(ret == ES_WIFI_STATUS_OK)
    {
      sprintf((char *)Obj->CmdData,"S3=%04d\r",Reqlen);
      ret = AT_RequestSendData(Obj, Obj->CmdData, pdata, Reqlen, Obj->CmdData);

      if(ret == ES_WIFI_STATUS_OK)
      {
        if(strstr((char *)Obj->CmdData,"-1\r\n"))
        {
          DEBUG("Send Data detect error %s\n", (char *)Obj->CmdData);
          ret = ES_WIFI_STATUS_ERROR;
        }
      }
      else
      {
        DEBUG("Send Data command failed\n");  
      }
    }
    else
    {
      DEBUG("S2 command failed\n"); 
    }
  }
  else
  {
   DEBUG("P0 command failed\n"); 
  }

  if (ret == ES_WIFI_STATUS_ERROR)
  {
    *SentLen = 0;
  }
  UNLOCK_WIFI();
  return ret;
}

ES_WIFI_Status_t  ES_WIFI_SendDataTo(ES_WIFIObject_t *Obj, uint8_t Socket, uint8_t *pdata, uint16_t Reqlen , uint16_t *SentLen, uint32_t Timeout, uint8_t *IPaddr, uint16_t Port)
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;
  LOCK_WIFI();  

  sprintf((char*)Obj->CmdData,"P0=%d\r", Socket);
  ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

  if (ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"P2=%d\r", /*LocalPort*/ 56830 ); // WARN: Does not work!
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }

  // ? Are we sure that the Firmware can change the packet destination without stopping the socket?
  if (ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"P4=%d\r", Port);
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }

  if (ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"P3=%d.%d.%d.%d\r", IPaddr[0], IPaddr[1], IPaddr[2], IPaddr[3]);
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }

  if (ret == ES_WIFI_STATUS_OK)
  {
    if(Reqlen >= ES_WIFI_PAYLOAD_SIZE )
    {
      Reqlen= ES_WIFI_PAYLOAD_SIZE;
    }
  }

  if(ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData, "S2=%lu\r", Timeout);
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }

  if(ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char *)Obj->CmdData, "S3=%04d\r", Reqlen);
    ret = AT_RequestSendData(Obj, Obj->CmdData, pdata, Reqlen, Obj->CmdData);
  }

  if(ret == ES_WIFI_STATUS_OK)
  {
    char *ptr = strstr((char *)Obj->CmdData,"-1\r\n");
    if(ptr != NULL)
    {
      if (ptr < (char *) &Obj->CmdData[sizeof(Obj->CmdData)])
      {
        ret = ES_WIFI_STATUS_ERROR;
      }
      else
      {
        ret = ES_WIFI_STATUS_IO_ERROR;
      }
    }
  }

  if (ret == ES_WIFI_STATUS_OK)
  {
    *SentLen = Reqlen;
  }
  else
  {
    DEBUG("Send error:\n%s\n", Obj->CmdData);
    *SentLen = 0;
  }

  UNLOCK_WIFI();
  return ret;
}

int issue15=0;
/**
  * @brief  Receive an amount data over WIFI.
  * @param  Obj: pointer to module handle
  * @param  Socket: number of the socket
  * @param  pdata: pointer to data
  * @param  len : pointer to the length of the data to be received
  * @retval Operation Status.
  */
ES_WIFI_Status_t ES_WIFI_ReceiveData(ES_WIFIObject_t *Obj, uint8_t Socket, uint8_t *pdata, uint16_t Reqlen, uint16_t *Receivedlen, uint32_t Timeout)
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;
  LOCK_WIFI();  

  if(Reqlen <= ES_WIFI_PAYLOAD_SIZE )
  {
    sprintf((char*)Obj->CmdData,"P0=%d\r", Socket);
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

    if(ret == ES_WIFI_STATUS_OK)
    {
      sprintf((char*)Obj->CmdData,"R1=%d\r", Reqlen);
      ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
      if(ret == ES_WIFI_STATUS_OK)
      {
        sprintf((char*)Obj->CmdData,"R2=%lu\r", Timeout);
        ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
        if(ret == ES_WIFI_STATUS_OK)
        {
          sprintf((char*)Obj->CmdData,"R0\r");
          ret = AT_RequestReceiveData(Obj, Obj->CmdData, (char *)pdata, Reqlen, Receivedlen);
          if (ret != ES_WIFI_STATUS_OK)
          {
            DEBUG("AT_RequestReceiveData  failed\n"); 
          }
        }
        else
        {
         DEBUG("setting timeout failed\n"); 
        }
      }
      else
      {
        DEBUG("setting requested len failed\n"); 
        *Receivedlen = 0;
      }
    }
    else
    {
      DEBUG("setting socket for read failed\n"); 
      issue15++;
    }
  }
  UNLOCK_WIFI();
  return ret;
}


ES_WIFI_Status_t  ES_WIFI_ReceiveDataFrom(ES_WIFIObject_t *Obj, uint8_t Socket, uint8_t *pdata, uint16_t Reqlen, uint16_t *Receivedlen, uint32_t Timeout, uint8_t *IPaddr, uint16_t *pPort)
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;
  *Receivedlen = 0;
  LOCK_WIFI();  

  if (Reqlen <= ES_WIFI_PAYLOAD_SIZE )
  {
    sprintf((char*)Obj->CmdData, "P0=%d\r", Socket);
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }

  if(ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"R1=%d\r", Reqlen);
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }
  else
  {
    DEBUG("P0 failed.\n");
  }

  if(ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"R2=%lu\r", Timeout);
    ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);
  }
  else
  {
    DEBUG("R1 failed.\n");
  }

  if(ret == ES_WIFI_STATUS_OK)
  {
    sprintf((char*)Obj->CmdData,"R0\r");
    ret = AT_RequestReceiveData(Obj, Obj->CmdData, (char *)pdata, Reqlen, Receivedlen);
  }
  else
  {
    DEBUG("R2 failed.\n");
  }

  if (ret == ES_WIFI_STATUS_OK)
  {
    if (*Receivedlen > Reqlen)
    {
      DEBUG("AT_RequestReceiveData overflow\n.");
      ret = ES_WIFI_STATUS_ERROR;
    }
    else
    {
      if (*Receivedlen > 0)
      {
        /* Get the peer addr */
        sprintf((char*)Obj->CmdData,"P?\r");
        ret = AT_ExecuteCommand(Obj, Obj->CmdData, Obj->CmdData);

        if (ret == ES_WIFI_STATUS_OK)
        {
          ES_WIFI_Transport_t TransportSettings;
          memset(&TransportSettings, 0, sizeof(TransportSettings));
          AT_ParseTransportSettings((char *) Obj->CmdData, &TransportSettings);
          memcpy(IPaddr, TransportSettings.Remote_IP_Addr, 4);
          *pPort = TransportSettings.Remote_Port;
        }
      }
    }
  }

  if (ret != ES_WIFI_STATUS_OK)
  {
    DEBUG("Read error:\n%s\n", Obj->CmdData);
    *Receivedlen = 0;
  }
  UNLOCK_WIFI();
  return ret;
}

ES_WIFI_Status_t  ES_WIFI_StoreCreds( ES_WIFIObject_t *Obj,
                                      ES_WIFI_CredsFunction_t credsFunction, uint8_t credSet,
                                      uint8_t* ca, uint16_t caLength,
                                      uint8_t* certificate, uint16_t certificateLength,
                                      uint8_t* key, uint16_t keyLength )
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;
  LOCK_WIFI();

  /* Set the credential set to use. */
  sprintf( ( char* )( Obj->CmdData ),"PF=%d,%d\r", credsFunction, credSet );
  ret = AT_ExecuteCommand( Obj, Obj->CmdData, Obj->CmdData );

  if( ret == ES_WIFI_STATUS_OK )
  {
    /* Store rootCA. */
    sprintf( ( char* )( Obj->CmdData ),"PG=%d,0,%04d\r", credSet, caLength );
    ret = AT_RequestSendData( Obj, Obj->CmdData, ( uint8_t* ) ca, caLength, Obj->CmdData );

    if( ret == ES_WIFI_STATUS_OK )
    {
      /* Store device certificate. */
      sprintf( ( char* )( Obj->CmdData ),"PG=%d,1,%04d\r", credSet, certificateLength );
      ret = AT_RequestSendData( Obj, Obj->CmdData, certificate, certificateLength, Obj->CmdData );

      if( ret == ES_WIFI_STATUS_OK )
      {
        /* Store device key. */
        sprintf( ( char* )( Obj->CmdData ),"PG=%d,2,%04d\r", credSet, keyLength );
        ret = AT_RequestSendData( Obj, Obj->CmdData, key, keyLength, Obj->CmdData );
      }
    }
  }

  UNLOCK_WIFI();
  return ret;
}

ES_WIFI_Status_t  ES_WIFI_StoreCA( ES_WIFIObject_t *Obj,
                                   ES_WIFI_CredsFunction_t credsFunction,
                                   uint8_t credSet,
                                   uint8_t* ca,
                                   uint16_t caLength )
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;
  LOCK_WIFI();

  /* Set the credential set to use. */
  sprintf( ( char* )( Obj->CmdData ),"PF=%d,%d\r", credsFunction, credSet );
  ret = AT_ExecuteCommand( Obj, Obj->CmdData, Obj->CmdData );

  if( ret == ES_WIFI_STATUS_OK )
  {
    /* Store CA. */
    sprintf( ( char* )( Obj->CmdData ),"PG=%d,0,%04d\r", credSet, caLength );
    ret = AT_RequestSendData( Obj, Obj->CmdData, ca, caLength, Obj->CmdData );
  }

  UNLOCK_WIFI();
  return ret;
}

ES_WIFI_Status_t  ES_WIFI_StoreCertificate( ES_WIFIObject_t *Obj,
                                            ES_WIFI_CredsFunction_t credsFunction,
                                            uint8_t credSet,
                                            uint8_t* certificate,
                                            uint16_t certificateLength )
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;
  LOCK_WIFI();

  /* Set the credential set to use. */
  sprintf( ( char* )( Obj->CmdData ),"PF=%d,%d\r", credsFunction, credSet );
  ret = AT_ExecuteCommand( Obj, Obj->CmdData, Obj->CmdData );

  if( ret == ES_WIFI_STATUS_OK )
  {
    /* Store certificate. */
    sprintf( ( char* )( Obj->CmdData ),"PG=%d,1,%04d\r", credSet, certificateLength );
    ret = AT_RequestSendData( Obj, Obj->CmdData, certificate, certificateLength, Obj->CmdData );
  }

  UNLOCK_WIFI();
  return ret;
}

ES_WIFI_Status_t  ES_WIFI_StoreKey( ES_WIFIObject_t *Obj,
                                    ES_WIFI_CredsFunction_t credsFunction,
                                    uint8_t credSet,
                                    uint8_t* key,
                                    uint16_t keyLength )
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;
  LOCK_WIFI();

  /* Set the credential set to use. */
  sprintf( ( char* )( Obj->CmdData ),"PF=%d,%d\r", credsFunction, credSet );
  ret = AT_ExecuteCommand( Obj, Obj->CmdData, Obj->CmdData );

  if( ret == ES_WIFI_STATUS_OK )
  {
    /* Store device key. */
    sprintf( ( char* )( Obj->CmdData ),"PG=%d,2,%04d\r", credSet, keyLength );
    ret = AT_RequestSendData( Obj, Obj->CmdData, key, keyLength, Obj->CmdData );
  }

  UNLOCK_WIFI();
  return ret;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
