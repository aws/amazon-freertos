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

#define ESP8266_ALL_SOCKET_IDS      0xFF

ESP_WIFI_IPD_t xWifiIpd[wificonfigMAX_SOCKETS];

struct ESP_WIFI_PACKET {
    struct ESP_WIFI_PACKET *next;
    uint8_t LinkID;
    uint16_t DataLength;
    uint16_t TotalLength;
} ESP_WIFI_PACKET;

struct ESP_WIFI_PACKET *gPackets, **gPacketsEnd;

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
								xBuffer->ucSSIDLength = strlen( pcSSID );
								if( xBuffer->ucSSIDLength > wificonfigMAX_SSID_LEN )
								{
								    xBuffer->ucSSIDLength = wificonfigMAX_SSID_LEN;			
								}
								memcpy( xBuffer->ucSSID, pcSSID, xBuffer->ucSSIDLength );
            }
            break;

        case 3:
            xBuffer->cRSSI = (int8_t)atoi(pcPtr);
            break;

        case 4:
            ParseMacAddr(pcPtr, &xBuffer->ucBSSID[0]);
            break;

        case 5:
            xBuffer->ucChannel = (int8_t)atoi(pcPtr);
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
      
        /* Set PH multi-function pins for UART1 RXD, TXD */
        SYS->GPH_MFPH &= ~(SYS_GPH_MFPH_PH8MFP_Msk | SYS_GPH_MFPH_PH9MFP_Msk);
        SYS->GPH_MFPH |= (SYS_GPH_MFPH_PH8MFP_UART1_TXD | SYS_GPH_MFPH_PH9MFP_UART1_RXD);
        SYS_ResetModule(UART1_RST);
        vTaskDelay(pdMS_TO_TICKS(10));
#if ESP_WIFI_SERIAL_FC
        /* Set PB multi-function pins for UART1 RTS and CTS */
        SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB8MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk);
        SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB8MFP_UART1_nRTS | SYS_GPB_MFPH_PB9MFP_UART1_nCTS);
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
    char *pcDelim = ",:\r\n";
    char *pcPtr;
    char *pcRecv;
    uint8_t ucExit = 0;
    uint8_t ucIpdLinkID = 0;
    uint16_t usCount;
    uint16_t usNum;
    uint16_t usIpdLength;
    uint16_t usAllocLength;

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
                if ((strstr((char *)pucRxBuf, "\r\nRecv") > 0) && (strstr((char *)pucRxBuf, "bytes\r\n") > 0)) {
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
            } else if (strstr(pcPtr, AT_CLOSE0_STRING)) {
                ucExit = 1;
                xRet = ESP_WIFI_STATUS_CLOSE;
                xWifiIpd[0].IsOpen = pdFALSE;
            } else if (strstr(pcPtr, AT_CLOSE1_STRING)) {
                ucExit = 1;
                xRet = ESP_WIFI_STATUS_CLOSE;
                xWifiIpd[1].IsOpen = pdFALSE;
            } else if (strstr(pcPtr, AT_CLOSE2_STRING)) {
                ucExit = 1;
                xRet = ESP_WIFI_STATUS_CLOSE;
                xWifiIpd[2].IsOpen = pdFALSE;
            } else if (strstr(pcPtr, AT_CLOSE3_STRING)) {
                ucExit = 1;
                xRet = ESP_WIFI_STATUS_CLOSE;
                xWifiIpd[3].IsOpen = pdFALSE;
            } else if (strstr(pcPtr, AT_CLOSE4_STRING)) {
                ucExit = 1;
                xRet = ESP_WIFI_STATUS_CLOSE;
                xWifiIpd[4].IsOpen = pdFALSE;
            } else if ((strstr(pcPtr, AT_RECV_STRING)) && (pxObj->IsPassiveMode == pdTRUE)) {
                ucExit = 1;
                xRet = ESP_WIFI_STATUS_RECV;

                pcRecv = strstr(pcPtr, AT_RECV_STRING);
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
                xWifiIpd[ucIpdLinkID].DataAvailable = usIpdLength;
            }
            break;

        case '>':
            if (strstr(pcPtr, AT_SEND_STRING) == pcPtr) {
                ucExit = 1;
                xRet = ESP_WIFI_STATUS_SEND;
            }
            break;

        case ':':
            if (pcRecv = strstr(pcPtr, AT_PASSIVE_STRING)) {
                /* For TCP passive mode to check +CIPRECVDATA */
                if (xWifiIpd[pxObj->ActiveSocket].IsPassiveMode == pdTRUE) {
                    /* Clear the receive buffer */
                    memcpy(ucRecvHeader, pcRecv, sizeof(ucRecvHeader));
                    pcRecv = (char *)(ucRecvHeader);

                    /* Get the IPD length */
                    pcPtr = strstr(pcRecv, AT_PASSIVE_STRING);
                    pcPtr = strtok(pcPtr, pcDelim);
                    pcPtr = strtok(NULL, pcDelim);
                    usIpdLength = (uint16_t)atoi(pcPtr);
                    xWifiIpd[pxObj->ActiveSocket].DataReceive = usIpdLength;

                    for (usNum = 0; usNum < usIpdLength; usNum++) {
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
                            *((char *)(xWifiIpd[pxObj->ActiveSocket].TcpData) + usNum) = RX_BUF_POP();
                            if (xTickTimeout - xTaskGetTickCount() < 5) {
                                xTickTimeout = xTaskGetTickCount() + 10;
                            }
                        } else {
                            break;
                        }
                    }

                    ucExit = 1;
                    xRet = ESP_WIFI_STATUS_RECV;
                }
            } else if (pcRecv = strstr(pcPtr, AT_RECV_STRING)) {
                /* For TCP/UDP active mode to check +IPD */

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
                /* Get the IPD length */
                pcPtr = strtok(NULL, pcDelim);
                usIpdLength = (uint16_t)atoi(pcPtr);

                if (ucIpdLinkID >= wificonfigMAX_SOCKETS || !xWifiIpd[ucIpdLinkID].IsOpen) {
                    configPRINTF(("ERROR: [%s] Invalid socket ID %d !!\n", __func__, ucIpdLinkID));
                    ucExit = 1;
                    break;
                }

                if (xWifiIpd[ucIpdLinkID].IsPassiveMode == pdFALSE) {
                    /* for RX packet dynamic malloc */
                    usAllocLength = sizeof(struct ESP_WIFI_PACKET) + usIpdLength;

                    if ((pxObj->HeapUsage + usAllocLength) > ESP_WIFI_IPD_SIZE) {
                        configPRINTF(("ERROR: [%s] ESP_WIFI_IPD_SIZE %d limit exceeded !!\n", __func__, ESP_WIFI_IPD_SIZE));
                        ucExit = 1;
                        break;
                    }

                    //struct ESP_WIFI_PACKET *packet = (struct ESP_WIFI_PACKET *)malloc(usAllocLength);
                    struct ESP_WIFI_PACKET *packet = (struct ESP_WIFI_PACKET *)pvPortMalloc(usAllocLength);
                    if (!packet) {
                        configPRINTF(("ERROR: [%s] Unable to allocate memory for packet !!\n", __func__));
                        ucExit = 1;
                        break;
                    }
                    pxObj->HeapUsage += usAllocLength;

                    packet->LinkID = ucIpdLinkID;
                    packet->DataLength = usIpdLength;
                    packet->TotalLength = usIpdLength;
                    packet->next = 0;

                    for (usNum = 0; usNum < usIpdLength; usNum++) {
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
                            *((char *)(packet + 1) + usNum) = RX_BUF_POP();
                            if (xTickTimeout - xTaskGetTickCount() < 5) {
                                xTickTimeout = xTaskGetTickCount() + 10;
                            }
                        } else {
                            break;
                        }
                    }

                    // Append to packet list
                    *gPacketsEnd = packet;
                    gPacketsEnd = &packet->next;

                    ucExit = 1;
                    xRet = ESP_WIFI_STATUS_RECV;
                }
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
        else
            Nuvoton_debug_printf(("."));
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
        memset( pxObj->CmdData, 0, ESP_WIFI_DATA_SIZE );
        if (xRet == ESP_WIFI_STATUS_OK) {
            pxObj->IsConnected = pdTRUE;

            /* Enable multiple connections */
            xRet = ESP_WIFI_SetMultiConn(pxObj, (uint8_t)1);

            if (xRet == ESP_WIFI_STATUS_OK) {
                /* Enable TCP passive mode */
                xRet = ESP_WIFI_SetTcpPassive(pxObj, (uint8_t)ESP_WIFI_TCP_PASSIVE);
#if ESP_WIFI_SERIAL_FC
                if (xRet == ESP_WIFI_STATUS_OK) {
                    /* Enable TCP passive mode */
                    xRet = ESP_WIFI_StartSerialFlowCtrl(pxObj);
                }
#endif
            }
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

#if ESP_WIFI_HW_RESET
    /* H/W reset */
    GPIO_SetMode(PH, BIT3, GPIO_MODE_OUTPUT);
    PH3 = 0;
    vTaskDelay(pdMS_TO_TICKS(2));
    PH3 = 1;
    xRet = ESP_WIFI_STATUS_OK;
    vTaskDelay(pdMS_TO_TICKS(200));
#else
    /* S/W reset */
    xRet = ESP_AT_Command(pxObj, (uint8_t *)"AT+RST\r\n", 1000);
    vTaskDelay(pdMS_TO_TICKS(500));
#endif

    /* Reset the flags and RX buffer */
    pxObj->IsMultiConn = pdFALSE;
    pxObj->IsPassiveMode = pdFALSE;
    pxObj->HeapUsage = 0;
    pxObj->ActiveSocket = -1;
    gPackets = 0;
    gPacketsEnd = &gPackets;
    RX_BUF_RESET();

    /* Clear the socket Ipd buffer */
    for (int id = 0; id < wificonfigMAX_SOCKETS; id++) {
        xWifiIpd[id].IsOpen = pdFALSE;
    }
    ESP_WIFI_Clear_Ipd(pxObj, ESP8266_ALL_SOCKET_IDS);

    // Might take a while to respond after HW reset
    for (int i = 0; i < 5; i++) {
        xRet = ESP_AT_Command(pxObj, (uint8_t *)"AT\r\n", 100);
        if (xRet == ESP_WIFI_STATUS_OK) {
            break;
        }
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

    xRet = ESP_AT_Command(pxObj, (uint8_t *)"AT+CIFSR\r\n", 30);

    if (xRet == ESP_WIFI_STATUS_OK) {
        AT_ParseAddress(pxObj);
    }

    return xRet;
}

/**
 * @brief  Clear the Ipd buffer
 */
void ESP_WIFI_Clear_Ipd( ESP_WIFI_Object_t * pxObj, uint8_t LinkID )
{
    struct ESP_WIFI_PACKET **p = &gPackets;

    while (*p) {
        if ((*p)->LinkID == LinkID || LinkID == ESP8266_ALL_SOCKET_IDS) {
            struct ESP_WIFI_PACKET *q = *p;

            if (gPacketsEnd == &(*p)->next) {
                gPacketsEnd = p; // Set last packet next field/gPacketsEnd
            }
            *p = (*p)->next;
            pxObj->HeapUsage -= (sizeof(struct ESP_WIFI_PACKET) + q->TotalLength);;
            vPortFree(q);
        } else {
            // Point to last packet next field
            p = &(*p)->next;
        }
    }
    if (LinkID == ESP8266_ALL_SOCKET_IDS) {
        for (int id = 0; id < wificonfigMAX_SOCKETS; id++) {
            xWifiIpd[LinkID].DataAvailable = 0;
        }
    } else {
        xWifiIpd[LinkID].DataAvailable = 0;
    }
}

/**
 * @brief  Get the Ipd buffer size of this socket
 */
uint16_t ESP_WIFI_Get_Ipd_Size( ESP_WIFI_Object_t * pxObj )
{
    return pxObj->HeapUsage;
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
 * @brief  Enable or Disable TCP Passive Mode
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_SetTcpPassive( ESP_WIFI_Object_t * pxObj, uint8_t mode )
{
    ESP_WIFI_Status_t xRet;

    sprintf((char *)pxObj->CmdData, "AT+CIPRECVMODE=%d\r\n", mode);
    xRet = ESP_AT_Command(pxObj, pxObj->CmdData, 0);

    if (xRet == ESP_WIFI_STATUS_OK) {
        pxObj->IsPassiveMode = (mode) ? pdTRUE : pdFALSE;
    }

    return xRet;
}

#if ESP_WIFI_SERIAL_FC
/**
 * @brief  Enable H/W UART Flow Control
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_StartSerialFlowCtrl( ESP_WIFI_Object_t * pxObj)
{
    ESP_WIFI_Status_t xRet;

    configPRINTF(("Enable WiFi H/W flow control\n"));
    sprintf((char *)pxObj->CmdData, "AT+UART_CUR=%d,8,1,0,3\r\n", wificonfigBAUD_RATE);
    xRet = ESP_AT_Command(pxObj, pxObj->CmdData, 0);

    if (xRet == ESP_WIFI_STATUS_OK) {
        if (wificonfigBAUD_RATE != 115200) {
            /* Setup the baud rate */
            configPRINTF(("Change baudrate to %d\n", wificonfigBAUD_RATE));
            UART_Open(pxObj->Uart, wificonfigBAUD_RATE);
        }
        // NOTE: Added in M480. Before configuring RTSACTLV, disable TX/RX.
        pxObj->Uart->FUNCSEL |= UART_FUNCSEL_TXRXDIS_Msk;
        while (pxObj->Uart->FIFOSTS & UART_FIFOSTS_TXRXACT_Msk);
        // nRTS pin output is low level active
        pxObj->Uart->MODEM |= UART_MODEM_RTSACTLV_Msk;
        // nCTS pin input is low level active
        pxObj->Uart->MODEMSTS |= UART_MODEMSTS_CTSACTLV_Msk;
        // NOTE: Added in M480. After configuring RTSACTLV, re-enable TX/RX.
        pxObj->Uart->FUNCSEL &= ~UART_FUNCSEL_TXRXDIS_Msk;
        // Configure RTS trigger level to 8 bytes
        pxObj->Uart->FIFO = (pxObj->Uart->FIFO & ~UART_FIFO_RTSTRGLV_Msk) | UART_FIFO_RTSTRGLV_8BYTES;
        // Enable RTS
        pxObj->Uart->INTEN |= UART_INTEN_ATORTSEN_Msk;
        // Enable CTS
        pxObj->Uart->INTEN |= UART_INTEN_ATOCTSEN_Msk;
    }

    return xRet;
}
#endif

/**
 * @brief  Establishes TCP Connection, UDP Transmission or SSL Connection
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_StartClient( ESP_WIFI_Object_t * pxObj, ESP_WIFI_Conn_t * pxConn )
{
    ESP_WIFI_Status_t xRet;
    ESP_WIFI_Status_t xClose;
    uint8_t ucCount;

    if (pxConn->LinkID >= wificonfigMAX_SOCKETS || xWifiIpd[pxConn->LinkID].IsOpen) {
        configPRINTF(("ERROR: [%s] Invalid socket ID %d !!\n", __func__, pxConn->LinkID));
        return ESP_WIFI_STATUS_ERROR;
    }

    /* Set multiple connection */
    if (pxObj->IsMultiConn == pdTRUE) {
        sprintf((char *)pxObj->CmdData, "AT+CIPSTART=%d,", pxConn->LinkID);
    } else {
        sprintf((char *)pxObj->CmdData, "AT+CIPSTART=");
    }

    xWifiIpd[pxConn->LinkID].IsPassiveMode = pdFALSE;
    /* Set connection type */
    switch (pxConn->Type) {
    case ESP_WIFI_TCP:
        sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\"TCP\",");
        xWifiIpd[pxConn->LinkID].IsPassiveMode = pxObj->IsPassiveMode;
        break;
    case ESP_WIFI_UDP:
        sprintf((char *)pxObj->CmdData + strlen((char *)pxObj->CmdData), "\"UDP\",");
        break;
    case ESP_WIFI_SSL:
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

    if (xRet == ESP_WIFI_STATUS_OK) {
        xWifiIpd[pxConn->LinkID].IsOpen = pdTRUE;
        ESP_WIFI_Clear_Ipd(pxObj, pxConn->LinkID);
    } else {
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
        xWifiIpd[pxConn->LinkID].IsOpen = pdFALSE;
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
    uint8_t ucCount;

    xTickEnd = xTaskGetTickCount() + xTimeout;
    (*usSendLen) = 0;

    if (pcBuf == NULL || pxConn->LinkID >= wificonfigMAX_SOCKETS || !xWifiIpd[pxConn->LinkID].IsOpen) {
        return xRet;
    }

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
 * @brief  Receive Data for TCP passive mode
 * @retval Operation status
 */
ESP_WIFI_Status_t ESP_WIFI_Recv_TCP_PASSIVE( ESP_WIFI_Object_t * pxObj, uint8_t ucLinkID, uint8_t * pcBuf, 
                                 uint16_t usReqLen, uint16_t * usRecvLen, TickType_t xTickEnd )
{
    ESP_WIFI_Status_t xRet = ESP_WIFI_STATUS_TIMEOUT;
   

    if (xWifiIpd[ucLinkID].DataAvailable != 0) {
        xWifiIpd[ucLinkID].TcpData = (char *)pcBuf;
        xWifiIpd[ucLinkID].DataReceive = 0;
        pxObj->ActiveSocket = ucLinkID;

        // +CIPRECVDATA supports up to 2048 bytes at a time
        usReqLen = usReqLen > 2048 ? 2048 : usReqLen;

        sprintf((char *)pxObj->CmdData, "AT+CIPRECVDATA=%d,%d\r\n", ucLinkID, usReqLen);
        xRet = ESP_AT_Command(pxObj, pxObj->CmdData, 0);

        if (xRet == ESP_WIFI_STATUS_OK) {
            if (xWifiIpd[ucLinkID].DataReceive > 0) {
                if (xWifiIpd[ucLinkID].DataAvailable > xWifiIpd[ucLinkID].DataReceive) {
                    xWifiIpd[ucLinkID].DataAvailable -= xWifiIpd[ucLinkID].DataReceive;
                } else {
                    xWifiIpd[ucLinkID].DataAvailable = 0;
                }
                (*usRecvLen) = xWifiIpd[ucLinkID].DataReceive;
            }
        }
        pxObj->ActiveSocket = -1;
    } else {
        // Try to get the +IPD message
        xRet = ESP_IO_Recv(pxObj, pxObj->CmdData, sizeof(pxObj->CmdData), ucLinkID, pdMS_TO_TICKS(ESP_WIFI_SHORT_RECV_TO));
        // To avoid upper layer receive unknown status
        if (xRet == ESP_WIFI_STATUS_RECV)
            xRet = ESP_WIFI_STATUS_TIMEOUT;
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
    uint8_t ucDoFlag = 1;

    if (pcBuf == NULL) {
        return xRet;
    }

    xTickEnd = xTaskGetTickCount() + xTimeout;
    (*usRecvLen) = 0;

    if (pxConn->LinkID >= wificonfigMAX_SOCKETS || !xWifiIpd[pxConn->LinkID].IsOpen) {
        configPRINTF(("ERROR: [%s] Invalid socket ID %d !!\n", __func__, pxConn->LinkID));
        return xRet;
    }

    /* Check TCP passive mode */
    if (xWifiIpd[pxConn->LinkID].IsPassiveMode == pdTRUE) {
        /* TCP passive mode */
        if (usReqLen > 0)
            xRet = ESP_WIFI_Recv_TCP_PASSIVE(pxObj, pxConn->LinkID, pcBuf, usReqLen, usRecvLen, xTickEnd);
        else
            xRet = ESP_WIFI_STATUS_OK;
    } else {
        /* TCP/UDP active mode */
        while (((*usRecvLen) < usReqLen) && (ucDoFlag == 1)) {
            // Check if any packets are ready for us
            for (struct ESP_WIFI_PACKET **p = &gPackets; *p; p = &(*p)->next) {
                if ((*p)->LinkID == pxConn->LinkID) {
                    struct ESP_WIFI_PACKET *q = *p;

                    if (q->DataLength <= usReqLen) {
                        // Return and remove full packet
                        memcpy(pcBuf, q + 1, q->DataLength);

                        if (gPacketsEnd == &(*p)->next) {
                            gPacketsEnd = p;
                        }
                        *p = (*p)->next;

                        vPortFree(q);
                        pxObj->HeapUsage -= (sizeof(struct ESP_WIFI_PACKET) + q->TotalLength);
                        (*usRecvLen) = q->DataLength;
                    } else {
                        // Return only partial packet
                        memcpy(pcBuf, q + 1, usReqLen);

                        q->DataLength -= usReqLen;
                        memmove(q + 1, (uint8_t *)(q + 1) + usReqLen, q->DataLength);

                        (*usRecvLen) = usReqLen;
                    }
                    xRet = ESP_WIFI_STATUS_OK;
                    break;
                }
            }

            if ((*usRecvLen) == 0) {
                while (1) {
                    xTickCurrent = xTaskGetTickCount();
                    if (xTickCurrent < xTickEnd) {
                        xTickTimeout = xTickEnd - xTickCurrent;
                    } else {
                        ucDoFlag = 0;
                        xRet = ESP_WIFI_STATUS_TIMEOUT;
                        break;
                    }

                    xRet = ESP_IO_Recv(pxObj, pxObj->CmdData, sizeof(pxObj->CmdData), pxConn->LinkID, xTickTimeout);
                    if (xRet == ESP_WIFI_STATUS_RECV) {
                        break;
                    } else {
                        /* Release the CPU resource */
                        vTaskDelay(1);
                    }
                }
            } else {
                ucDoFlag = 0;
            }
        }
    }

    return xRet;
}
