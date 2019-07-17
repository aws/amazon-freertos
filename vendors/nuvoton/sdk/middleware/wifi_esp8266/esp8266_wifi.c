/**************************************************************************//**
 * @file     esp8266_wifi.c
 * @version  V1.10
 * @brief    M480 series ESP8266 WiFi driver source file
 *
 * @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include "stdlib.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Socket and Wi-Fi interface includes. */
#include "iot_wifi.h"

/* Wi-Fi driver includes. */
#include "esp8266_wifi.h"

ESP_WIFI_IPD_t xWifiIpd[wificonfigMAX_SOCKETS];

/* Uart rx buffer control */
#define RX_BUF_SIZE         4096
#define RX_BUF_RESET()      (rx_buf_ridx = rx_buf_widx = 0)

static uint8_t rx_buf[RX_BUF_SIZE];
static uint16_t rx_buf_ridx, rx_buf_widx;

void RX_BUF_PUSH(uint8_t d)
{
    rx_buf[rx_buf_widx++] = d;
    if (rx_buf_widx == RX_BUF_SIZE) {
        rx_buf_widx = 0;
    }
}

uint8_t RX_BUF_POP(void)
{
    uint8_t d = rx_buf[(rx_buf_ridx++)];

    if (rx_buf_ridx == RX_BUF_SIZE) {
        rx_buf_ridx = 0;
    }

    return d;
}

uint8_t RX_BUF_FULL(void)
{
    if ((rx_buf_widx == (RX_BUF_SIZE - 1)) && (rx_buf_ridx == 0)) {
        return 1;
    }

    if (rx_buf_ridx == (rx_buf_widx + 1)) {
        return 1;
    } else {
        return 0;
    }
}

uint16_t RX_BUF_COUNT(void)
{
    if (rx_buf_widx >= rx_buf_ridx) {
        return (rx_buf_widx - rx_buf_ridx);
    } else {
        return (RX_BUF_SIZE + rx_buf_widx - rx_buf_ridx);
    }
}


/**
 * @brief  Parse an IP address
 */
static void ParseIpAddr( char * string, uint8_t * addr )
{
    char *pcDelim = "\",.";
    char *pcPtr;
    uint8_t ucCount;

    pcPtr = strtok(string, pcDelim);

    for (ucCount = 0; ucCount < 4; ucCount++) {
        addr[ucCount] = atoi(pcPtr);
        pcPtr = strtok(NULL, pcDelim);
    }
}

/**
 * @brief  Parse an MAC address
 */
static void ParseMacAddr( char * string, uint8_t * addr )
{
    char *pcDelim = "\",:";
    char *pcPtr;
    uint8_t ucCount;

    pcPtr = strtok(string, pcDelim);

    for (ucCount = 0; ucCount < 6; ucCount++) {
        addr[ucCount] = strtol(pcPtr, NULL, 16);
        pcPtr = strtok(NULL, pcDelim);
    }
}

/**
 * @brief  Parse an Access Point information
 */
static void ParseApDetail( char * string, WIFIScanResult_t *xBuffer )
{
    char *pcDelim = ",";
    char *pcPtr;
    char *pcSave;
    char *pcSSID;
    uint8_t ucNum = 1;
    uint8_t ucMode;

    pcPtr = strtok_r(string, pcDelim, &pcSave);

    while (pcPtr != NULL) {
        switch (ucNum++) {
        case 1:
            ucMode = (uint8_t)atoi(pcPtr);
            if (ucMode == 4) {
                /* Does not support WPA_WPA2, set it as WPA2 mode */
                ucMode = 3;
            } else if (ucMode > 4) {
                /* Set as eWiFiSecurityNotSupported */
                ucMode = 4;
            }
            xBuffer->xSecurity = (WIFISecurity_t)ucMode;
            break;

        case 2:
            pcSSID = strtok(pcPtr, "\"");
            if (pcSSID) {
                strlcpy(&xBuffer->cSSID[0], pcSSID, wificonfigMAX_SSID_LEN + 1);
            }
            break;

        case 3:
            xBuffer->cRSSI = (int8_t)atoi(pcPtr);
            break;

        case 4:
            ParseMacAddr(pcPtr, &xBuffer->ucBSSID[0]);
            break;

        case 5:
            xBuffer->cChannel = (int8_t)atoi(pcPtr);
            break;

        default:
            break;
        }
        pcPtr = strtok_r(NULL, pcDelim, &pcSave);
    }
}

/**
 * @brief  Get device IP and MAC address
 * @param  Obj: pointer to WiF module
 * @retval None
 */
static void AT_ParseAddress( ESP_WIFI_Object_t * pxObj )
{
    char *pcDelim = ",\r\n";
    char *pcPtr;
    char *pcSave;

    pcPtr = strtok_r((char *)pxObj->CmdData, pcDelim, &pcSave);

    while (pcPtr != NULL){
        if (strcmp(pcPtr, "+CIFSR:STAIP") == 0) {
            pcPtr = strtok_r(NULL, pcDelim, &pcSave);
            ParseIpAddr(pcPtr, pxObj->StaIpAddr);
        } else if (strcmp(pcPtr, "+CIFSR:STAMAC") == 0) {
            pcPtr = strtok_r(NULL, pcDelim, &pcSave);
            ParseMacAddr(pcPtr, pxObj->StaMacAddr);
        } else if (strcmp(pcPtr, "+CIFSR:APIP") == 0) {
            pcPtr = strtok_r(NULL, pcDelim, &pcSave);
            ParseIpAddr(pcPtr, pxObj->ApIpAddr);
        } else if (strcmp(pcPtr, "+CIFSR:APMAC") == 0) {
            pcPtr = strtok_r(NULL, pcDelim, &pcSave);
            ParseMacAddr(pcPtr, pxObj->ApMacAddr);
        }
        pcPtr = strtok_r(NULL, pcDelim, &pcSave);
    }
}

/**
 * @brief  Get scanned AP information
 */
static void AT_ParseAccessPoint( ESP_WIFI_Object_t * pxObj, WIFIScanResult_t * pxBuffer, uint8_t ucNumNetworks )
{
    char *pcDelim = "()\r\n";
    char *pcPtr;
    char *pcSave;
    uint8_t ucCount;

    pcPtr = strtok_r((char *)pxObj->CmdData, pcDelim, &pcSave);

    for (ucCount = 0; ucCount < ucNumNetworks; ) {
        if (pcPtr != NULL){
            if (strcmp(pcPtr, "+CWLAP:") == 0) {
                pcPtr = strtok_r(NULL, pcDelim, &pcSave);
                ParseApDetail(pcPtr, (WIFIScanResult_t *)(&pxBuffer[ucCount]));
                ucCount++;
            }
            pcPtr = strtok_r(NULL, pcDelim, &pcSave);
        } else
            break;
    }
}

/**
 * @brief  Initialize platform
 */
BaseType_t ESP_Platform_Init( ESP_WIFI_Object_t * pxObj )
{
    if (pxObj->Uart == (UART_T *)UART1) {
        CLK_EnableModuleClock(UART1_MODULE);
        /* Select UART1 clock source is HXT */
        CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART1SEL_HXT, CLK_CLKDIV0_UART1(1));
#if 0
        /* Set PB multi-function pins for UART1 RXD, TXD */
        SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB3MFP_Msk | SYS_GPB_MFPL_PB2MFP_Msk);
        SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB3MFP_UART1_TXD | SYS_GPB_MFPL_PB2MFP_UART1_RXD);
#else
        /* Set PH multi-function pins for UART1 RXD, TXD */
        SYS->GPH_MFPH &= ~(SYS_GPH_MFPH_PH8MFP_Msk | SYS_GPH_MFPH_PH9MFP_Msk);
        SYS->GPH_MFPH |= (SYS_GPH_MFPH_PH8MFP_UART1_TXD | SYS_GPH_MFPH_PH9MFP_UART1_RXD);
#endif
    } else {
        configPRINTF(("Do not support MFP setting of UART_BASE 0x%p !\n", pxObj->Uart));
    }

    /* Configure UART */
    UART_Open(pxObj->Uart, pxObj->UartBaudRate);
    /* Set RX Trigger Level = 8 */
    pxObj->Uart->FIFO = (pxObj->Uart->FIFO &~ UART_FIFO_RFITL_Msk) | UART_FIFO_RFITL_8BYTES;
    /* Set Timeout time 0x3E bit-time */
    UART_SetTimeoutCnt(pxObj->Uart, 0x3E);
    /* enable uart */
    UART_EnableInt(pxObj->Uart, UART_INTEN_RDAIEN_Msk | UART_INTEN_RXTOIEN_Msk | UART_INTEN_TOCNTEN_Msk);
    NVIC_EnableIRQ((IRQn_Type)pxObj->UartIrq);

    return pdTRUE;
}

/**
 * @brief  Write data to UART interface
 */
uint16_t ESP_IO_Send( ESP_WIFI_Object_t * pxObj, uint8_t pucTxBuf[], uint16_t usWriteBytes )
{
    if (usWriteBytes < 100)
        Nuvoton_debug_printf(("[%s] \"%s\"\n", __func__, pucTxBuf));
    else
        Nuvoton_debug_printf(("[%s]\n", __func__));

    return UART_Write(pxObj->Uart, pucTxBuf, usWriteBytes);
}

/**
 * @brief  UART interrupt handler for RX
 */
void UART1_IRQHandler(void)
{
    uint8_t ucDat;

    while (!UART_GET_RX_EMPTY(UART1)) {
        if (RX_BUF_FULL()) {
            configPRINTF(("[%s] RX buffer full !!\n", __func__));
        }
        ucDat = UART_READ(UART1);
        RX_BUF_PUSH(ucDat);
    }
}

/**
 * @brief  Read data from UART interface
 */
ESP_WIFI_Status_t ESP_IO_Recv( ESP_WIFI_Object_t * pxObj, uint8_t pucRxBuf[], uint16_t usReadBytes, 
                               uint8_t ucLinkID, TickType_t xTimeout )
{
    ESP_WIFI_Status_t xRet = ESP_WIFI_STATUS_TIMEOUT;
    TickType_t xTickTimeout;
    uint8_t ucRecvHeader[64];
    char *pcDelim = ",:";
    char *pcPtr;
    char *pcRecv;
    uint8_t ucExit = 0;
    uint8_t ucIpdLinkID = 0;
    uint16_t usCount;
    uint16_t usIpdLength;

    /* Set timeout ticks */
    xTickTimeout = xTaskGetTickCount() + xTimeout;

    /* Clear the receive buffer */
    pcPtr = (char *)(pucRxBuf);
    memset(pcPtr, 0x0, usReadBytes);

    for (usCount = 0; usCount < usReadBytes; usCount++) {
        /* Check RX empty => failed */
        while (RX_BUF_COUNT() == 0) {
            if (xTaskGetTickCount() > xTickTimeout) {
                /* If ESP8266 already receices data, need to wait for data send out complete */
                if ((strstr(pucRxBuf, "\r\nRecv") > 0) && (strstr(pucRxBuf, "bytes\r\n") > 0)) {
                    xTickTimeout += pdMS_TO_TICKS(1000);
                } else {
                    if (xTimeout > pdMS_TO_TICKS(ESP_WIFI_NONBLOCK_RECV_TO)) {
                        configPRINTF(("ERROR: [%s] usReadBytes %d reach the timeout %d !!\n", __func__, usReadBytes, xTimeout));
                    }
                    ucExit = 1;
                    break;
                }
            }
        }
        if (ucExit == 1) {
            break;
        }

        /* Get data from RX buffer */
        pucRxBuf[usCount] = RX_BUF_POP();
        if (xTickTimeout - xTaskGetTickCount() < pdMS_TO_TICKS(5)) {
            xTickTimeout = xTaskGetTickCount() + pdMS_TO_TICKS(10);
        }

        /* Check the end of the message to reduce the response time */
        switch (pucRxBuf[usCount]) {
        case '\n':
            if (strstr(pcPtr, AT_OK_STRING)) {
                ucExit = 1;
                xRet = ESP_WIFI_STATUS_OK;
            } else if ((strstr(pcPtr, AT_FAIL_STRING)) || (strstr(pcPtr, AT_ERROR_STRING))) {
                ucExit = 1;
                xRet = ESP_WIFI_STATUS_ERROR;
            } else if (strstr(pcPtr, AT_CLOSE_STRING)) {
                ucExit = 1;
                xRet = ESP_WIFI_STATUS_CLOSE;
            }
            break;

        case '>':
            if (strstr(pcPtr, AT_SEND_STRING) == pcPtr) {
                ucExit = 1;
                xRet = ESP_WIFI_STATUS_SEND;
            }
            break;

        case ':':
            /* Check if meet the receive data format */
            pcRecv = strstr(pcPtr, AT_RECV_STRING);
            if (pcRecv) {
                /* Clear the receive buffer */
                memcpy(ucRecvHeader, pcRecv, sizeof(ucRecvHeader));
                pcRecv = (char *)(ucRecvHeader);

                pcPtr = strstr(pcRecv, AT_RECV_STRING);
                pcPtr = strtok(pcPtr, pcDelim);
                /* Check multiple connection */
                if (pxObj->IsMultiConn == pdTRUE) {
                    /* Get the Link ID */
                    pcPtr = strtok(NULL, pcDelim);
                    ucIpdLinkID = (uint8_t)atoi(pcPtr);
                }
                pcPtr = strtok(NULL, pcDelim);
                usIpdLength = (uint16_t)atoi(pcPtr);
                if (xWifiIpd[ucIpdLinkID].DataLength + usIpdLength > sizeof(xWifiIpd[ucIpdLinkID].Data)) {
                    if (xWifiIpd[ucIpdLinkID].DataLength < sizeof(xWifiIpd[ucIpdLinkID].Data)) {
                        configPRINTF(("ERROR: [%s] Get ipd %d bytes reach the maximum size !!\n", __func__, usIpdLength));
                    }
                }
                if (pxObj->ActiveCmd != CMD_RECV) {
                    /* Current active command is not the receive, we need to store the IPD data */
                    for (; usIpdLength > 0; usIpdLength--) {
                        while (RX_BUF_COUNT() == 0) {
                            if (xTaskGetTickCount() > xTickTimeout) {
                                if (xTimeout > pdMS_TO_TICKS(ESP_WIFI_NONBLOCK_RECV_TO)) {
                                    configPRINTF(("ERROR: [%s] Get ipd reach the timeout %d !!\n", __func__, xTimeout));
                                }
                                ucExit = 1;
                                break;
                            }
                        }
                        if (ucExit == 0) {
                            if (xWifiIpd[ucIpdLinkID].DataLength < sizeof(xWifiIpd[ucIpdLinkID].Data)) {
                                xWifiIpd[ucIpdLinkID].Data[xWifiIpd[ucIpdLinkID].DataLength++] = RX_BUF_POP();
                            } else {
                                RX_BUF_POP();
                            }
                            if (xTickTimeout - xTaskGetTickCount() < 5) {
                                xTickTimeout = xTaskGetTickCount() + 10;
                            }
                        } else {
                            break;
                        }
                    }
                    ucExit = 1;
                } else {
                    if (usReadBytes >= (usCount + usIpdLength + 1)) {
                        usReadBytes = usCount + usIpdLength + 1;
                    } else {
                        configPRINTF(("ERROR: [%s] Get ipd %d bytes cause a buffer overflow !!\n", __func__, usIpdLength));
                    }
                }
                xRet = ESP_WIFI_STATUS_RECV;
            }
            break;

        default:
            break;
        }
        if (ucExit == 1) {
            break;
        }
    }
    if (usCount < 100) {
        if (usCount > 0)
            Nuvoton_debug_printf(("[%s] \"%s\", usCount = %d, xRet = %d\n", __func__, pucRxBuf, usCount, xRet));
        else {
            Nuvoton_debug_printf(("."));
			 /* Release the CPU resource */
            vTaskDelay(1);
		}
    } else
        Nuvoton_debug_printf(("[%s], usCount = %d, xRet = %d\n", __func__, usCount, xRet));

    return xRet;
}

/**
 * @brief  Execute AT command
 * @param  pxObj: pointer to WiF module
 * @param  pucCmd: pointer to AT command string
 * @param  ulTimeWaitMs: command wait time in ms
 * @retval Operation status
 */
static ESP_WIFI_Status_t ESP_AT_Command( ESP_WIFI_Object_t * pxObj, uint8_t * pucCmd, uint32_t ulTimeWaitMs )
{
    ESP_WIFI_Status_t xRet = ESP_WIFI_STATUS_TIMEOUT;
    uint16_t usCount;

    /* Wait for last AT command is done */
    while (xTaskGetTickCount() < pxObj->AvailableTick) {
        taskYIELD();
    }

    if (ESP_IO_Send(pxObj, pucCmd, strlen((char *)pucCmd)) > 0) {
        do {
            xRet = ESP_IO_Recv(pxObj, pxObj->CmdData, sizeof(pxObj->CmdData), 0, pxObj->Timeout);

            if (ulTimeWaitMs > 0) {
                /* Set the time that next AT command is workable */
                pxObj->AvailableTick = xTaskGetTickCount() + pdMS_TO_TICKS(ulTimeWaitMs);
            }
        } while (xRet > ESP_WIFI_STATUS_TIMEOUT);
    }
    Nuvoton_debug_printf(("[%s] pucCmd %s, xRet = %d\n", __func__, pucCmd, xRet));

    return xRet;
}

/**
 * @brief  Check the DNS server setting
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_CheckDnsServer( ESP_WIFI_Object_t * pxObj )
{
    ESP_WIFI_Status_t xRet;
    char *pcDelim = ":\r\n";
    char *pcPtr;

    sprintf((char *)pxObj->CmdData, "AT+CIPDNS_DEF?\r\n");
    xRet = ESP_AT_Command(pxObj, pxObj->CmdData, 0);

    if (xRet == ESP_WIFI_STATUS_OK) {
        xRet = ESP_WIFI_STATUS_ERROR;

        pcPtr = strtok((char *)pxObj->CmdData, pcDelim);
        while (pcPtr != NULL){
            if (strcmp(pcPtr, "+CIPDNS_DEF") == 0) {
                pcPtr = strtok(NULL, pcDelim);

                /* To check is there a DNS server */
                if (strcmp(pcPtr, "255.255.255.255") == 0) {
                    configPRINTF(("There is no DNS server, set it to be resolver1.opendns.com.\n"));
                    /* Specify the DNS server to resolver1.opendns.com */
                    sprintf((char *)pxObj->CmdData, "AT+CIPDNS_DEF=1,\"208.67.222.222\"\r\n");
                    xRet = ESP_AT_Command(pxObj, pxObj->CmdData, 0);
                }
                break;
            }
            pcPtr = strtok(NULL, pcDelim);
        }
    }

    return xRet;
}

/**
 * @brief  Initialize WIFI module
 * @param  Obj: pointer to WiF module
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_Init( ESP_WIFI_Object_t * pxObj )
{
    ESP_WIFI_Status_t xRet = ESP_WIFI_STATUS_ERROR;

    if (ESP_Platform_Init(pxObj) == pdTRUE) {
        /* Reset the WiFi module */
        xRet = ESP_WIFI_Reset(pxObj);

        ESP_WIFI_CheckDnsServer(pxObj);
    }

    return xRet;
}

/**
 * @brief  Connects to an AP
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_Connect( ESP_WIFI_Object_t * pxObj, const char * cSSID, const char * cPassword )
{
    ESP_WIFI_Status_t xRet;

    /* Reset the WiFi module */
    xRet = ESP_WIFI_Reset(pxObj);

    /* Set WiFi to STA mode */
    xRet = ESP_AT_Command(pxObj, (uint8_t *)"AT+CWMODE=1\r\n", 0);

    if (xRet == ESP_WIFI_STATUS_OK) {
        configPRINTF(("Try to connect to AP \"%s\" ...\n", cSSID));
        sprintf((char *)pxObj->CmdData, "AT+CWJAP=\"%s\",\"%s\"\r\n", cSSID, cPassword);
        xRet = ESP_AT_Command(pxObj, pxObj->CmdData, 0);
        if (xRet == ESP_WIFI_STATUS_OK) {
            pxObj->IsConnected = pdTRUE;

            /* Enable multiple connections */
            xRet = ESP_WIFI_SetMultiConn(pxObj, (uint8_t)1);
        }
    }

    return xRet;
}

/**
 * @brief  Disconnects from the AP
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_Disconnect( ESP_WIFI_Object_t * pxObj )
{
    ESP_WIFI_Status_t xRet;

    xRet = ESP_AT_Command(pxObj, (uint8_t *)"AT+CWQAP\r\n", 0);
    if (xRet == ESP_WIFI_STATUS_OK) {
        pxObj->IsConnected = pdFALSE;
        /* Prevent scan AP soon and returns an error */
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    return xRet;
}

/**
 * @brief  Restarts the module
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_Reset( ESP_WIFI_Object_t * pxObj )
{
    ESP_WIFI_Status_t xRet;
    uint8_t ucCount;

    xRet = ESP_AT_Command(pxObj, (uint8_t *)"AT+RST\r\n", 1000);
    vTaskDelay(pdMS_TO_TICKS(500));

    /* Reset the flags, RX and Ipd buffers */
    pxObj->IsMultiConn = pdFALSE;
    pxObj->ActiveCmd = CMD_NONE;
    RX_BUF_RESET();
    for (ucCount = 0; ucCount < wificonfigMAX_SOCKETS; ucCount++) {
        xWifiIpd[ucCount].DataLength = 0;
    }

    return xRet;
}

/**
 * @brief  Lists Available APs
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_Scan( ESP_WIFI_Object_t * pxObj, WIFIScanResult_t * pxBuffer, uint8_t ucNumNetworks )
{
    ESP_WIFI_Status_t xRet;

    xRet = ESP_AT_Command(pxObj, "AT+CWLAP\r\n", 0);

    if (xRet == ESP_WIFI_STATUS_OK) {
        AT_ParseAccessPoint(pxObj, pxBuffer, ucNumNetworks);
    }

    return xRet;
}

/**
 * @brief  Check the module is connected to an AP
 */
BaseType_t ESP_WIFI_IsConnected( ESP_WIFI_Object_t * pxObj )
{
    return pxObj->IsConnected;
}

/**
 * @brief  Get network status
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_GetNetStatus( ESP_WIFI_Object_t * pxObj )
{
    ESP_WIFI_Status_t xRet;

    xRet = ESP_AT_Command(pxObj, (uint8_t *)"AT+CIFSR\r\n", 0);

    if (xRet == ESP_WIFI_STATUS_OK) {
        AT_ParseAddress(pxObj);
    }

    return xRet;
}

/**
 * @brief  Reset the Ipd buffer
 */
void ESP_WIFI_Reset_Ipd( ESP_WIFI_Conn_t * pxConn )
{
    xWifiIpd[pxConn->LinkID].DataLength = 0;
}

/**
 * @brief  Get the Ipd buffer size of this socket
 */
uint16_t ESP_WIFI_Get_Ipd_Size( ESP_WIFI_Conn_t * pxConn )
{
    return xWifiIpd[pxConn->LinkID].DataLength;
}

/**
 * @brief  Get the connection status
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_GetConnStatus( ESP_WIFI_Object_t * pxObj, ESP_WIFI_Conn_t * pxConn )
{
    ESP_WIFI_Status_t xRet;
    char *pcDelim = ":,\r\n";
    char *pcPtr;
    uint8_t ucCount;

    xRet = ESP_AT_Command(pxObj, (uint8_t *)"AT+CIPSTATUS\r\n", 0);

    if (xRet == ESP_WIFI_STATUS_OK) {
        pxConn->IsConnected = pdFAIL;
        pcPtr = strtok((char *)pxObj->CmdData, pcDelim);
        /* Skip the first line of response */
        pcPtr = strtok(NULL, pcDelim);

        while (pcPtr != NULL){
            if (strcmp(pcPtr, "+CIPSTATUS") == 0) {
                pcPtr = strtok(NULL, pcDelim);
                if (pxConn->LinkID == (uint8_t)atoi(pcPtr)) {
                    pxConn->IsConnected = pdTRUE;
                    break;
                }
            } else if (strcmp(pcPtr, "STATUS") == 0) {
                pcPtr = strtok(NULL, pcDelim);
                pxConn->Status = (uint8_t)atoi(pcPtr);
            }
            pcPtr = strtok(NULL, pcDelim);
        }
    }

    return xRet;
}

/**
 * @brief  Ping an IP address
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_Ping( ESP_WIFI_Object_t * pxObj, uint8_t * pucIPAddr )
{
    sprintf((char *)pxObj->CmdData, "AT+PING=\"%d.%d.%d.%d\"\r\n", pucIPAddr[0], pucIPAddr[1], pucIPAddr[2], pucIPAddr[3]);

    return ESP_AT_Command(pxObj, pxObj->CmdData, 0);
}

/**
 * @brief  Get the IP address from a host name
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_GetHostIP( ESP_WIFI_Object_t * pxObj, char * pcHost, uint8_t * pucIPAddr )
{
    ESP_WIFI_Status_t xRet;
    char *pcDelim = ":\r\n";
    char *pcPtr;

    sprintf((char *)pxObj->CmdData, "AT+CIPDOMAIN=\"%s\"\r\n", pcHost);
    xRet = ESP_AT_Command(pxObj, pxObj->CmdData, 0);

    if (xRet == ESP_WIFI_STATUS_OK) {
        xRet = ESP_WIFI_STATUS_ERROR;

        pcPtr = strtok((char *)pxObj->CmdData, pcDelim);
        while (pcPtr != NULL){
            if (strcmp(pcPtr, "+CIPDOMAIN") == 0) {
                pcPtr = strtok(NULL, pcDelim);
                ParseIpAddr(pcPtr, pucIPAddr);
                xRet = ESP_WIFI_STATUS_OK;
                break;
            }
            pcPtr = strtok(NULL, pcDelim);
        }
    }

    return xRet;
}

/**
 * @brief  Enable or Disable Multiple Connections
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_SetMultiConn( ESP_WIFI_Object_t * pxObj, uint8_t mode )
{
    ESP_WIFI_Status_t xRet;

    sprintf((char *)pxObj->CmdData, "AT+CIPMUX=%d\r\n", mode);
    xRet = ESP_AT_Command(pxObj, pxObj->CmdData, 0);

    if (xRet == ESP_WIFI_STATUS_OK) {
        pxObj->IsMultiConn = (mode) ? pdTRUE : pdFALSE;
    }

    return xRet;
}

/**
 * @brief  Establishes TCP Connection, UDP Transmission or SSL Connection
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_StartClient( ESP_WIFI_Object_t * pxObj, ESP_WIFI_Conn_t * pxConn )
{
    ESP_WIFI_Status_t xRet;
    ESP_WIFI_Status_t xClose;
    uint8_t ucCount;

    /* Set multiple connection */
    if (pxObj->IsMultiConn == pdTRUE) {
        sprintf((char *)pxObj->CmdData, "AT+CIPSTART=%d,", pxConn->LinkID);
    } else {
        sprintf((char *)pxObj->CmdData, "AT+CIPSTART=");
    }

    /* Set connection type */
    switch (pxConn->Type) {
    case 0:
        sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\"TCP\",");
        break;
    case 1:
        sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\"UDP\",");
        break;
    case 2:
        sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\"SSL\",");
        break;
    }

    /* Set remote IP address and port */
    sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\"%d", pxConn->RemoteIP[0]);
    for (ucCount = 1; ucCount < 4; ucCount++) {
        sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), ".%d", pxConn->RemoteIP[ucCount]);
    }
    sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\",%d", pxConn->RemotePort);

    /* Set remaining data */
    if (pxConn->Type == ESP_WIFI_UDP) {
        if (pxConn->UdpLocalPort > 0) {
            sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), ",%d,%d", pxConn->UdpLocalPort, pxConn->UdpMode);
        }
    } else {
        if (pxConn->TcpKeepAlive > 0) {
            sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), ",%d", pxConn->TcpKeepAlive);
        }
    }

    sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\r\n");
    xRet = ESP_AT_Command(pxObj, pxObj->CmdData, 0);

    if (xRet != ESP_WIFI_STATUS_OK) {
        configPRINTF(("%s returns %d failed !!\n", __func__, xRet));
        /* Wait for socket closed if returns error */
        if ((xRet == ESP_WIFI_STATUS_ERROR) && (!strstr((char *)pxObj->CmdData, "ALREADY CONNECTED"))) {
            do {
                Nuvoton_debug_printf(("Socket connect error, Wait for socket close.\n"));
                xClose = ESP_IO_Recv(pxObj, pxObj->CmdData, sizeof(pxObj->CmdData), 0, pxObj->Timeout);
            } while (xClose != ESP_WIFI_STATUS_CLOSE);
            pxObj->AvailableTick = xTaskGetTickCount() + pdMS_TO_TICKS(200);
        }
    }

    return xRet;
}

/**
 * @brief  Closes the TCP/UDP/SSL Connection
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_StopClient( ESP_WIFI_Object_t * pxObj, ESP_WIFI_Conn_t * pxConn )
{
    if (pxObj->IsMultiConn == pdTRUE) {
        sprintf((char *)pxObj->CmdData, "AT+CIPCLOSE=%d\r\n", pxConn->LinkID);
    } else {
        sprintf((char *)pxObj->CmdData, "AT+CIPCLOSE\r\n");
    }

    return ESP_AT_Command(pxObj, pxObj->CmdData, 0);
}

/**
 * @brief  Sends Data
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_Send( ESP_WIFI_Object_t * pxObj, ESP_WIFI_Conn_t * pxConn, uint8_t * pcBuf, 
                                 uint16_t usReqLen, uint16_t * usSendLen, TickType_t xTimeout )
{
    ESP_WIFI_Status_t xRet = ESP_WIFI_STATUS_ERROR;
    TickType_t xTickCurrent, xTickEnd, xTickTimeout;
    uint16_t usAskSend, usRealSend;
    uint8_t ucExit = 0;
    uint8_t ucCount;

    xTickEnd = xTaskGetTickCount() + xTimeout;

    if (pcBuf == NULL) {
        return xRet;
    }

    (*usSendLen) = 0;
    while ((*usSendLen) < usReqLen) {
        usAskSend = usReqLen - (*usSendLen);
        if (usAskSend > ESP_WIFI_SEND_SIZE)
            usAskSend = ESP_WIFI_SEND_SIZE;

        /* Set multiple connection */
        if (pxObj->IsMultiConn == pdTRUE) {
            sprintf((char *)pxObj->CmdData, "AT+CIPSEND=%d,%d", pxConn->LinkID, usAskSend);
        } else {
            sprintf((char *)pxObj->CmdData, "AT+CIPSEND=%d", usAskSend);
        }

        /* Set remote IP and port for UDP connection */
        if ((pxConn->Type == ESP_WIFI_UDP) && (pxConn->UdpLocalPort > 0)) {
            sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\"%d", pxConn->RemoteIP[0]);
            for (ucCount = 1; ucCount < 4; ucCount++) {
                sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), ".%d", pxConn->RemoteIP[ucCount]);
            }
            sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\",%d", pxConn->RemotePort);
        }

        sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\r\n");
        /* Must wait a period of time after the CIPSEND AT command */
        xRet = ESP_AT_Command(pxObj, pxObj->CmdData, 10);

        if (xRet == ESP_WIFI_STATUS_OK) {
            pxObj->ActiveCmd = CMD_SEND;
            xRet = ESP_WIFI_STATUS_ERROR;

            xTickCurrent = xTaskGetTickCount();
            if (xTickCurrent < xTickEnd) {
                xTickTimeout = xTickEnd - xTickCurrent;
                if (ESP_IO_Recv(pxObj, pxObj->CmdData, sizeof(pxObj->CmdData), 0, xTickTimeout) == ESP_WIFI_STATUS_SEND) {
                    /* WiFi module allows to send data */
                    memcpy(pxObj->CmdData, pcBuf + (*usSendLen), usAskSend);
                    *(pxObj->CmdData + usAskSend) = 0;
                    usRealSend = ESP_IO_Send(pxObj, pxObj->CmdData, usAskSend);
                    if (usRealSend > 0) {
                        do {
                            xTickCurrent = xTaskGetTickCount();
                            if (xTickCurrent >= xTickEnd) {
                                xRet = ESP_WIFI_STATUS_TIMEOUT;
                            } else {
                                xTickTimeout = xTickEnd - xTickCurrent;
                                /* Ensure the module has enough time to send data */
                                if (xTickTimeout < pdMS_TO_TICKS(200)) {
                                    xTickTimeout = pdMS_TO_TICKS(200);
                                }
                                xRet = ESP_IO_Recv(pxObj, pxObj->CmdData, sizeof(pxObj->CmdData), 0, xTickTimeout);
                            }
                        } while (xRet > ESP_WIFI_STATUS_TIMEOUT);
                        (*usSendLen) += usRealSend;
                        pxObj->ActiveCmd = CMD_NONE;
                    }
                }
            } else {
                xRet = ESP_WIFI_STATUS_TIMEOUT;
                break;
            }
        } else {
            break;
        }
    }

    return xRet;
}

/**
 * @brief  Receive Data
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_Recv( ESP_WIFI_Object_t * pxObj, ESP_WIFI_Conn_t * pxConn, uint8_t * pcBuf, 
                                 uint16_t usReqLen, uint16_t * usRecvLen, TickType_t xTimeout )
{
    ESP_WIFI_Status_t xRet = ESP_WIFI_STATUS_ERROR;
    TickType_t xTickCurrent, xTickEnd, xTickTimeout;
    char *pcDelim = ",:";
    char *pcPtr;
    uint8_t ucLinkID;
    uint16_t usCount, usRemain;

    xTickEnd = xTaskGetTickCount() + xTimeout;

    if (pcBuf == NULL) {
        return xRet;
    }

    pxObj->ActiveCmd = CMD_RECV;
    (*usRecvLen) = 0;
    if (xWifiIpd[pxConn->LinkID].DataLength) {
        if (xWifiIpd[pxConn->LinkID].DataLength >= usReqLen) {
            memcpy(pcBuf, xWifiIpd[pxConn->LinkID].Data, usReqLen);
            (*usRecvLen) = usReqLen;

            /* Remain data of the IPD content */
            xWifiIpd[pxConn->LinkID].DataLength = xWifiIpd[pxConn->LinkID].DataLength - usReqLen;
            for (usCount = 0; usCount < xWifiIpd[pxConn->LinkID].DataLength; usCount++) {
                xWifiIpd[pxConn->LinkID].Data[usCount] = xWifiIpd[pxConn->LinkID].Data[usReqLen + usCount];
            }
            memset(xWifiIpd[pxConn->LinkID].Data + usCount, 0x0, usReqLen);
            xRet = ESP_WIFI_STATUS_OK;
        } else {
            memcpy(pcBuf, xWifiIpd[pxConn->LinkID].Data, xWifiIpd[pxConn->LinkID].DataLength);
            (*usRecvLen) = xWifiIpd[pxConn->LinkID].DataLength;

            /* Reset the IPD content */
            xWifiIpd[pxConn->LinkID].DataLength = 0;
            memset(xWifiIpd[pxConn->LinkID].Data, 0x0, sizeof(xWifiIpd[pxConn->LinkID].Data));
        }
    }

    while ((*usRecvLen) < usReqLen) {
        xTickCurrent = xTaskGetTickCount();
        if (xTickCurrent < xTickEnd) {
            xTickTimeout = xTickEnd - xTickCurrent;
        } else {
            break;
        }

        xRet = ESP_IO_Recv(pxObj, pxObj->CmdData, sizeof(pxObj->CmdData), pxConn->LinkID, xTickTimeout);
        if (xRet == ESP_WIFI_STATUS_RECV) {
            xRet = ESP_WIFI_STATUS_TIMEOUT;
            pcPtr = strstr((char *)(pxObj->CmdData), AT_RECV_STRING);
            if (pcPtr) {
                /* Meet the receive data format */
                pcPtr = strtok(pcPtr, pcDelim);

                if (pxObj->IsMultiConn == pdTRUE) {
                    /* Get the Link ID */
                    pcPtr = strtok(NULL, pcDelim);
                    ucLinkID = (int8_t)atoi(pcPtr);

                    /* Get the receive length */
                    pcPtr = strtok(NULL, pcDelim);
                    usCount = (uint16_t)atoi(pcPtr);

                    /* point to the start address of receive data */
                    pcPtr += strlen(pcPtr) + 1;

                    if (ucLinkID == pxConn->LinkID) {
                        /* Copy the receive data */
                        if (usCount > (usReqLen - (*usRecvLen))) {
                            usRemain = (*usRecvLen) + usCount - usReqLen;
                            usCount = usReqLen - (*usRecvLen);
                            xWifiIpd[pxConn->LinkID].DataLength = usRemain;
                            memcpy(xWifiIpd[pxConn->LinkID].Data, pcPtr+usCount, usRemain);
                        }
                        memcpy(pcBuf + (*usRecvLen), pcPtr, usCount);
                        (*usRecvLen) += usCount;
                        xRet = ESP_WIFI_STATUS_OK;
                    } else {
                        /* Store the receive data for another socket */
                        memcpy(xWifiIpd[ucLinkID].Data + xWifiIpd[ucLinkID].DataLength, pcPtr, usCount);
                        xWifiIpd[ucLinkID].DataLength += usCount;
                    }
                }
            }
        }
    }
    pxObj->ActiveCmd = CMD_NONE;

    return xRet;
}
