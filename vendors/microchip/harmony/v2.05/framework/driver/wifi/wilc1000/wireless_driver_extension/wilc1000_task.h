/*******************************************************************************
  WILC1000 Wireless Driver Main Task

  File Name:
    wilc1000_task.h

  Summary:
    Entry point of WILC1000 core driver.

  Description:
    Entry point of WILC1000 core driver.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/

#ifndef WILC1000_TASK_H_INCLUDED
#define WILC1000_TASK_H_INCLUDED

#include <stdbool.h>
#include "driver/wifi/wilc1000/wireless_driver_extension/common/include/nm_common.h"
#include "driver/wifi/wilc1000/wireless_driver_extension/driver/include/m2m_types.h"
#include "driver/wifi/wilc1000/wireless_driver_extension/driver/source/m2m_hif.h"
#include "driver/wifi/wilc1000/wireless_driver_extension/driver/include/m2m_wifi.h"

#ifdef __cplusplus
    extern "C" {
#endif

typedef void *(*GetRxBufFunc)(void);

typedef struct {
#if (OSAL_USE_RTOS == 1 || OSAL_USE_RTOS == 9)
    TaskHandle_t handle;
#endif    
	OSAL_SEM_HANDLE_TYPE scanDoneWait;
    OSAL_SEM_HANDLE_TYPE scanResultWait;
    OSAL_SEM_HANDLE_TYPE connInfoWait;
    OSAL_SEM_HANDLE_TYPE eventWait;
    volatile bool deinit_in_progress;
} WILC1000_PRIV;

typedef struct {
	WDRV_INDICATIONS	wdrvIndications;
    void (*FrameReceived_CB)(uint32_t len, uint8_t const *const frame);
    void (*ConnectEvent_CB)(bool connected, bool isServer, uint8_t const *const client);
    void (*RFReady_CB)(uint8_t const *const addr);
    void (*ScanDone_CB)(uint32_t status);
    void (*InitDone_CB)(void);
    void (*WPSDone_CB)(void);
    uint8_t address[6];
    GetRxBufFunc get_rx_buf;
    bool ethMode;
    uint8_t *fwOtaServerUrl;
} WILC1000_INTF;

typedef struct {
#define CONN_DATA_UNSPECIFIED -1
    uint8_t ssid[32 + 1];
    int16_t ssidLen;
    int8_t secType;
    uint8_t key[64 + 1];
    uint8_t keyLen;
    uint8_t keyIdx;
    uint8_t channel;
    int8_t isSoftAP;
} WILC1000_CONNECT_DATA;

typedef struct {
    bool scanListGetOnce;
    bool scanInProgress;
} WILC1000_SCAN_CONTROL;

extern void isr(void);

extern WILC1000_INTF *g_wilc1000_intf;

inline void wilc1000_init_completed(void) 
{
    do { 
        if (g_wilc1000_intf->InitDone_CB) 
            (*g_wilc1000_intf->InitDone_CB)(); 

        if (g_wilc1000_intf->wdrvIndications.initCompleted) 
            (*g_wilc1000_intf->wdrvIndications.initCompleted)(); 
    } while (0);
}

inline void wilc1000_rf_ready(uint8_t *mac) 
{
    do { 
        if (g_wilc1000_intf->RFReady_CB) 
            (*g_wilc1000_intf->RFReady_CB)(mac); 

        if (g_wilc1000_intf->wdrvIndications.rfReady) 
            (*g_wilc1000_intf->wdrvIndications.rfReady)(mac); 
    } while (0);
}

inline void wilc1000_wps_completed(void) 
{
    do { 
        if (g_wilc1000_intf->WPSDone_CB) 
            (*g_wilc1000_intf->WPSDone_CB)(); 

        if (g_wilc1000_intf->wdrvIndications.wpsCompleted) 
            (*g_wilc1000_intf->wdrvIndications.wpsCompleted)(); 
    } while (0);
}

inline void wilc1000_scan_completed(uint32_t status) 
{
    do { 
        if (g_wilc1000_intf->ScanDone_CB) 
            (*g_wilc1000_intf->ScanDone_CB)(status); 

        if (g_wilc1000_intf->wdrvIndications.scanCompleted) 
            (*g_wilc1000_intf->wdrvIndications.scanCompleted)(status); 
    } while (0);
}

inline void wilc1000_eth_data_received(uint32_t len, uint8_t *frame) 
{
    do { 
        if (g_wilc1000_intf->FrameReceived_CB) 
            (*g_wilc1000_intf->FrameReceived_CB)(len, frame); 

        if (g_wilc1000_intf->wdrvIndications.frameReceived) 
            (*g_wilc1000_intf->wdrvIndications.frameReceived)(len, frame); 
    } while (0);
}

inline void wilc1000_connect_event_received(bool connected, bool isServer, uint8_t *client) 
{
    do { 
        if (g_wilc1000_intf->ConnectEvent_CB) 
            (*g_wilc1000_intf->ConnectEvent_CB)(connected, isServer, client); 

        if (g_wilc1000_intf->wdrvIndications.connectEvent) 
            (*g_wilc1000_intf->wdrvIndications.connectEvent)(connected, isServer, client); 
    } while (0);
}

#define wilc1000_get_rx_bufer() do { if(g_wilc1000_intf->get_rx_buf) (*g_wilc1000_intf->get_rx_buf)(); } while (0);
#define wilc1000_eth_data_send(frame, len, ifcid) m2m_wifi_send_ethernet_pkt(frame, len, ifcid)
#define wilc1000_isr(sem) isr()

void wilc1000_task(void *arg);
void wilc1000_task_deinit(void);
void wilc1000_task_init(WILC1000_INTF *intf);
void wilc1000_task_init2(WILC1000_INTF *intf);
int8_t wilc1000_connect(WILC1000_CONNECT_DATA *data);
void wilc1000_scan_result_read(tstrM2mWifiscanResult *result);
void wilc1000_wps_info_read(tstrM2MWPSInfo *info);
void wilc1000_conn_info_read(tstrM2MConnInfo *info);
sint8 wilc1000_init(void);
sint8 wilc1000_deinit(void);
void wilc1000_fw_update(uint32_t pContext);

extern WILC1000_PRIV g_wdrvext_priv;
bool WDRV_WILC1000_IsScanComplete(void);

#ifdef __cplusplus
}
#endif

#endif /* WILC1000_TASK_H_INCLUDED*/
