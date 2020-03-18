/****************************************************************************
**
**  Name:       bt_target.h
**
**  Function    this file contains definitions that will probably
**              change for each Bluetooth target system. This includes
**              such things as buffer pool sizes, number of tasks,
**              little endian/big endian conversions, etc...
**
**  NOTE        This file should always be included first.
**
**
**  Copyright (c) 1999-2015, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/

#ifndef BT_TARGET_H
#define BT_TARGET_H

#include "data_types.h"

#include "buildcfg.h"

/* Include common GKI definitions used by this platform */
#include "gki_target.h"

#include "bt_types.h"   /* This must be defined AFTER buildcfg.h */
#include "dyn_mem.h"    /* defines static and/or dynamic memory for components */

/* #define BYPASS_AVDATATRACE */


#ifdef WICED_BTE_LIB
/* Translate BTE function names into wiced_bt function names */
#include "wiced_bte_translate.h"
#endif

/******************************************************************************
**
** Platform-Specific
**
******************************************************************************/

#ifndef BT_BRCM_VS_INCLUDED
#define BT_BRCM_VS_INCLUDED             TRUE
#endif

#ifndef BRCM_ADV_PCF_LEGACY
#define BRCM_ADV_PCF_LEGACY             TRUE
#endif

/* set to FALSE unless using Zeevo */
#ifndef ZEEVO_CTRL_DEFINED
#define ZEEVO_CTRL_DEFINED FALSE
#endif

/* Supporting GPS shared transport */
#ifndef GPS_INCLUDED
#define GPS_INCLUDED               TRUE
#endif

/* API macros for simulator */

#define BTAPI

#ifndef BTE_BSE_WRAPPER
#ifdef  BTE_SIM_APP
#undef  BTAPI
#define BTAPI         __declspec(dllexport)
#endif
#endif

#define BT_API          BTAPI
#define BTU_API         BTAPI
#define A2D_API         BTAPI
#define VDP_API         BTAPI
#define AVDT_API        BTAPI
#define AVCT_API        BTAPI
#define AVRC_API        BTAPI
#define BIP_API         BTAPI
#define BNEP_API        BTAPI
#define BPP_API         BTAPI
#define BTM_API         BTAPI
#define DUN_API         BTAPI
#define FTP_API         BTAPI
#define GAP_API         BTAPI
#define GOEP_API        BTAPI
#define HCI_API         BTAPI
#define HID_API         BTAPI
#define HFP_API         BTAPI
#define HSP2_API        BTAPI
#define L2C_API         BTAPI
#define OBX_API         BTAPI
#define OPP_API         BTAPI
#define PAN_API         BTAPI
#define RFC_API         BTAPI
#define RPC_API         BTAPI
#define SDP_API         BTAPI
#define SPP_API         BTAPI
#define XML_API         BTAPI
#define BTA_API         BTAPI
#define SBC_API         BTAPI
#define LPM_API         BTAPI
#define MCE_API         BTAPI
#define MCA_API         BTAPI
#define GCE_API         BTAPI
#define GSE_API         BTAPI
#define GATT_API        BTAPI
#define SMP_API         BTAPI

/* Allow removal of 'static' qualifier for firmware builds (allows functions to be patched easier) */
#ifndef MAY_BE_STATIC
#define MAY_BE_STATIC   static
#endif

/******************************************************************************
**
** GKI Buffer Pools
**
******************************************************************************/

/* Receives HCI events from the lower-layer. */
#ifndef HCI_CMD_POOL_ID
#define HCI_CMD_POOL_ID             GKI_POOL_ID_2
#endif

#ifndef HCI_CMD_POOL_BUF_SIZE
#define HCI_CMD_POOL_BUF_SIZE       GKI_BUF2_SIZE
#endif

/* Receives ACL data packets from thelower-layer. */
#ifndef HCI_ACL_POOL_ID
#define HCI_ACL_POOL_ID             GKI_POOL_ID_3
#endif

#ifndef HCI_ACL_POOL_BUF_SIZE
#define HCI_ACL_POOL_BUF_SIZE       GKI_BUF3_SIZE
#endif

/* Maximum number of buffers available for ACL receive data. */
#ifndef HCI_ACL_BUF_MAX
#define HCI_ACL_BUF_MAX             GKI_BUF3_MAX
#endif

/* Receives SCO data packets from the lower-layer. */
#ifndef HCI_SCO_POOL_ID
#define HCI_SCO_POOL_ID             GKI_POOL_ID_6
#endif

/* Not used. */
#ifndef HCI_DATA_DESCR_POOL_ID
#define HCI_DATA_DESCR_POOL_ID      GKI_POOL_ID_0
#endif

/* Sends SDP data packets. */
#ifndef SDP_POOL_ID
#define SDP_POOL_ID                 GKI_POOL_ID_2
#endif

/* Sends RFCOMM command packets. */
#ifndef RFCOMM_CMD_POOL_ID
#define RFCOMM_CMD_POOL_ID          GKI_POOL_ID_2
#endif

#ifndef RFCOMM_CMD_POOL_BUF_SIZE
#define RFCOMM_CMD_POOL_BUF_SIZE    GKI_BUF2_SIZE
#endif

/* Sends RFCOMM data packets. */
#ifndef RFCOMM_DATA_POOL_ID
#define RFCOMM_DATA_POOL_ID         GKI_POOL_ID_3
#endif

#ifndef RFCOMM_DATA_POOL_BUF_SIZE
#define RFCOMM_DATA_POOL_BUF_SIZE   GKI_BUF3_SIZE
#endif

/* Sends L2CAP packets to the peer and HCI messages to the controller. */
#ifndef L2CAP_CMD_POOL_ID
#define L2CAP_CMD_POOL_ID           GKI_POOL_ID_2
#endif

#ifndef L2CAP_FCR_INCLUDED
#define L2CAP_FCR_INCLUDED    FALSE
#endif

/* Sends L2CAP segmented packets in ERTM mode */
#ifndef L2CAP_FCR_TX_POOL_ID
#define L2CAP_FCR_TX_POOL_ID        HCI_ACL_POOL_ID
#endif

/* Receives L2CAP segmented packets in ERTM mode */
#ifndef L2CAP_FCR_RX_POOL_ID
#define L2CAP_FCR_RX_POOL_ID        HCI_ACL_POOL_ID
#endif

/* number of LE buffer to assign if shared HCI buffer pool with BR/EDR,
   it can not exceed the number of ACL buffer available in controller */
#ifndef L2C_DEF_NUM_BLE_BUF_SHARED
#define L2C_DEF_NUM_BLE_BUF_SHARED      1
#endif

/* Used by BTM when it sends HCI commands to the controller. */
#ifndef BTM_CMD_POOL_ID
#define BTM_CMD_POOL_ID             GKI_POOL_ID_2
#endif

#ifndef OBX_CMD_POOL_SIZE
#define OBX_CMD_POOL_SIZE           GKI_BUF2_SIZE
#endif

#ifndef OBX_LRG_DATA_POOL_SIZE
#define OBX_LRG_DATA_POOL_SIZE      GKI_BUF4_SIZE
#endif

#ifndef OBX_LRG_DATA_POOL_ID
#define OBX_LRG_DATA_POOL_ID        GKI_POOL_ID_4
#endif

/* Used to send data to L2CAP. */
#ifndef GAP_DATA_POOL_ID
#define GAP_DATA_POOL_ID            GKI_POOL_ID_3
#endif

/* Used for SPP inquiry and discovery databases. */
#ifndef SPP_DB_POOL_ID
#define SPP_DB_POOL_ID              GKI_POOL_ID_3
#endif

#ifndef SPP_DB_SIZE
#define SPP_DB_SIZE                 GKI_BUF3_SIZE
#endif

#ifndef BIP_EVT_POOL_SIZE
#define BIP_EVT_POOL_SIZE           GKI_BUF3_SIZE
#endif

#ifndef BIP_DB_SIZE
#define BIP_DB_SIZE                 GKI_BUF3_SIZE
#endif


/* BNEP data and protocol messages. */
#ifndef BNEP_POOL_ID
#define BNEP_POOL_ID                GKI_POOL_ID_3
#endif

/* RPC pool for temporary trace message buffers. */
#ifndef RPC_SCRATCH_POOL_ID
#define RPC_SCRATCH_POOL_ID         GKI_POOL_ID_2
#endif

/* RPC scratch buffer size (not related to RPC_SCRATCH_POOL_ID) */
#ifndef RPC_SCRATCH_BUF_SIZE
#define RPC_SCRATCH_BUF_SIZE        GKI_BUF3_SIZE
#endif

/* RPC pool for protocol messages */
#ifndef RPC_MSG_POOL_ID
#define RPC_MSG_POOL_ID             GKI_POOL_ID_3
#endif

#ifndef RPC_MSG_POOL_SIZE
#define RPC_MSG_POOL_SIZE           GKI_BUF3_SIZE
#endif

/* AVDTP pool for protocol messages */
#ifndef AVDT_CMD_POOL_ID
#define AVDT_CMD_POOL_ID            GKI_POOL_ID_2
#endif

/* AVDTP pool size for media packets in case of fragmentation */
#ifndef AVDT_DATA_POOL_SIZE
#define AVDT_DATA_POOL_SIZE         GKI_BUF3_SIZE
#endif

#ifndef PAN_POOL_ID
#define PAN_POOL_ID                 GKI_POOL_ID_3
#endif

/* UNV pool for read/write serialization */
#ifndef UNV_MSG_POOL_ID
#define UNV_MSG_POOL_ID             GKI_POOL_ID_2
#endif

#ifndef UNV_MSG_POOL_SIZE
#define UNV_MSG_POOL_SIZE           GKI_BUF2_SIZE
#endif

/* AVCTP pool for protocol messages */
#ifndef AVCT_CMD_POOL_ID
#define AVCT_CMD_POOL_ID            GKI_POOL_ID_1
#endif

/* AVRCP pool for protocol messages */
#ifndef AVRC_CMD_POOL_ID
#define AVRC_CMD_POOL_ID            GKI_POOL_ID_1
#endif

/* AVRCP pool size for protocol messages */
#ifndef AVRC_CMD_POOL_SIZE
#define AVRC_CMD_POOL_SIZE          GKI_BUF1_SIZE
#endif

/* AVRCP Metadata pool for protocol messages */
#ifndef AVRC_META_CMD_POOL_ID
#define AVRC_META_CMD_POOL_ID       GKI_POOL_ID_2
#endif

/* AVRCP Metadata pool size for protocol messages */
#ifndef AVRC_META_CMD_POOL_SIZE
#define AVRC_META_CMD_POOL_SIZE     GKI_BUF2_SIZE
#endif


/* AVRCP buffer size for browsing channel messages */
#ifndef AVRC_BROWSE_POOL_SIZE
#define AVRC_BROWSE_POOL_SIZE     GKI_MAX_BUF_SIZE
#endif

/*  HDP buffer size for the Pulse Oximeter  */
#ifndef BTA_HL_LRG_DATA_POOL_SIZE
#define BTA_HL_LRG_DATA_POOL_SIZE      GKI_BUF7_SIZE
#endif

#ifndef BTA_HL_LRG_DATA_POOL_ID
#define BTA_HL_LRG_DATA_POOL_ID        GKI_POOL_ID_7
#endif

/* GATT Server Database pool ID */
#ifndef GATT_DB_POOL_ID
#define GATT_DB_POOL_ID                 GKI_POOL_ID_8
#endif

/* GATT Data sending buffer pool ID, use default ACL pool for fix channel data */
#ifndef GATT_BUF_POOL_ID
#define GATT_BUF_POOL_ID        HCI_ACL_POOL_ID
#endif
/******************************************************************************
**
** Lower Layer Interface
**
******************************************************************************/

/* Sends ACL data received over HCI to the upper stack. */
#ifndef HCI_ACL_DATA_TO_UPPER
#define HCI_ACL_DATA_TO_UPPER(p)    {((BT_HDR *)p)->event = BT_EVT_TO_BTU_HCI_ACL; GKI_send_msg (BTU_TASK, BTU_HCI_RCV_MBOX, p);}
#endif

/* Sends SCO data received over HCI to the upper stack. */
#ifndef HCI_SCO_DATA_TO_UPPER
#define HCI_SCO_DATA_TO_UPPER(p)    {((BT_HDR *)p)->event = BT_EVT_TO_BTU_HCI_SCO; GKI_send_msg (BTU_TASK, BTU_HCI_RCV_MBOX, p);}
#endif

/* Sends an HCI event received over HCI to theupper stack. */
#ifndef HCI_EVT_TO_UPPER
#define HCI_EVT_TO_UPPER(p)         {((BT_HDR *)p)->event = BT_EVT_TO_BTU_HCI_EVT; GKI_send_msg (BTU_TASK, BTU_HCI_RCV_MBOX, p);}
#endif

/* HCI 4 wire power management protocol. */
#ifndef HCILL_INCLUDED
#define HCILL_INCLUDED              FALSE
#endif

/* Macro for allocating buffer for HCI commands */
#ifndef HCI_GET_CMD_BUF
#if (!defined(HCI_USE_VARIABLE_SIZE_CMD_BUF) || (HCI_USE_VARIABLE_SIZE_CMD_BUF == FALSE))
/* Allocate fixed-size buffer from HCI_CMD_POOL (default case) */
#define HCI_GET_CMD_BUF(paramlen)    ((BT_HDR *)GKI_getpoolbuf (HCI_CMD_POOL_ID))
#else
/* Allocate smallest possible buffer (for platforms with limited RAM) */
#define HCI_GET_CMD_BUF(paramlen)    ((BT_HDR *)GKI_getbuf ((UINT16)(BT_HDR_SIZE + HCIC_PREAMBLE_SIZE + (paramlen))))
#endif
#endif  /* HCI_GET_CMD_BUF */

/******************************************************************************
**
** HCI Services (H4)
**
******************************************************************************/
#ifndef HCISU_H4_INCLUDED
#define HCISU_H4_INCLUDED               FALSE
#endif

#ifdef __cplusplus
extern "C" {
#endif
BT_API extern void bte_ncisu_send (BT_HDR *p_pkt, UINT16 event);
BT_API extern void bte_hcisu_send (BT_HDR *p_msg, UINT16 event);
#if (HCISU_H4_INCLUDED == TRUE)
BT_API extern void bte_hcisu_lp_allow_bt_device_sleep (void);
BT_API extern void bte_hcisu_lp_wakeup_host (void);
BT_API extern void bte_hcisu_lp_h4ibss_evt(UINT8 *p, UINT8 evt_len);
#endif

/* HCILL API for the applications */
typedef void (tHCILL_SLEEP_ACK)(void);
BT_API extern void HCILL_GoToSleep( tHCILL_SLEEP_ACK *sl_ack_fn);
typedef void (tHCILL_STATE_CBACK)(BOOLEAN is_sleep);
BT_API extern void HCILL_RegState( tHCILL_STATE_CBACK *p_cback);
#ifdef __cplusplus
}
#endif

/* Sends ACL data received from the upper stack to the BD/EDR HCI transport. */
#ifndef HCI_ACL_DATA_TO_LOWER
#define HCI_ACL_DATA_TO_LOWER(p)    bte_hcisu_send((BT_HDR *)(p), BT_EVT_TO_LM_HCI_ACL);
#endif

#ifndef HCI_BLE_ACL_DATA_TO_LOWER
#define HCI_BLE_ACL_DATA_TO_LOWER(p)    bte_hcisu_send((BT_HDR *)(p), (UINT16)(BT_EVT_TO_LM_HCI_ACL|LOCAL_BLE_CONTROLLER_ID));
#endif

/* Sends SCO data received from the upper stack to the HCI transport. */
#ifndef HCI_SCO_DATA_TO_LOWER
#define HCI_SCO_DATA_TO_LOWER(p)    bte_hcisu_send((BT_HDR *)(p), BT_EVT_TO_LM_HCI_SCO);
#endif

/* Sends an HCI command received from the upper stack to the BD/EDR HCI transport. */
#ifndef HCI_CMD_TO_LOWER
#define HCI_CMD_TO_LOWER(p)         bte_hcisu_send((BT_HDR *)(p), BT_EVT_TO_LM_HCI_CMD);
#endif

/* Sends an LM Diagnosic command received from the upper stack to the HCI transport. */
#ifndef HCI_LM_DIAG_TO_LOWER
#define HCI_LM_DIAG_TO_LOWER(p)     bte_hcisu_send((BT_HDR *)(p), BT_EVT_TO_LM_DIAG);
#endif

/* Send HCISU a message to allow BT sleep */
#ifndef HCI_LP_ALLOW_BT_DEVICE_SLEEP
#if (HCISU_H4_INCLUDED == TRUE)
#define HCI_LP_ALLOW_BT_DEVICE_SLEEP()       bte_hcisu_lp_allow_bt_device_sleep()
#else
#define HCI_LP_ALLOW_BT_DEVICE_SLEEP()       HCILP_AllowBTDeviceSleep()
#endif
#endif

/* Send HCISU a message to wakeup host */
#ifndef HCI_LP_WAKEUP_HOST
#if (HCISU_H4_INCLUDED == TRUE)
#define HCI_LP_WAKEUP_HOST()        bte_hcisu_lp_wakeup_host()
#else
#define HCI_LP_WAKEUP_HOST()        HCILP_WakeupHost()
#endif
#endif

/* Send HCISU the received H4IBSS event from controller */
#ifndef HCI_LP_RCV_H4IBSS_EVT
#if (HCISU_H4_INCLUDED == TRUE)
#define HCI_LP_RCV_H4IBSS_EVT(p1, p2)  bte_hcisu_lp_h4ibss_evt((UINT8*)(p1), (UINT8)(p2))
#else
#define HCI_LP_RCV_H4IBSS_EVT(p1, p2)  h4ibss_sleep_mode_evt((UINT8*)(p1), (UINT8)(p2))
#endif
#endif

/* If nonzero, the upper-layer sends at most this number of HCI commands to the lower-layer. */
#ifndef HCI_MAX_SIMUL_CMDS
#define HCI_MAX_SIMUL_CMDS          0
#endif

/* Timeout for receiving response to HCI command */
#ifndef BTU_CMD_CMPL_TIMEOUT
#define BTU_CMD_CMPL_TIMEOUT        8
#endif

/* If TRUE, BTU task will check HCISU again when HCI command timer expires */
#ifndef BTU_CMD_CMPL_TOUT_DOUBLE_CHECK
#define BTU_CMD_CMPL_TOUT_DOUBLE_CHECK      FALSE
#endif

/* If TRUE, stack is compiled to include MM dual stack functionality */
#ifndef BTU_DUAL_STACK_MM_INCLUDED
#define BTU_DUAL_STACK_MM_INCLUDED     FALSE
#endif

/* If TRUE, stack is compiled to support Embedded Lite Stack in BT chip */
#ifndef BTU_DUAL_STACK_BTC_INCLUDED
#define BTU_DUAL_STACK_BTC_INCLUDED      FALSE
#endif

/* If TRUE, stack is compiled to support content protection in BT chip */
#ifndef BTU_DUAL_STACK_BTC_CP_INCLUDED
#define BTU_DUAL_STACK_BTC_CP_INCLUDED  FALSE
#endif

/* If TRUE, stack is compiled to support Embedded Lite Stack for AV SNK in BT chip */
#ifndef BTU_BTC_SNK_INCLUDED
#define BTU_BTC_SNK_INCLUDED        FALSE
#endif

/* If TRUE, stack is compiled as Lite Stack in Multimedia chip */
/* If FALSE, stack is compiled as Full Stack in Baseband chip */
#ifndef BTU_STACK_LITE_ENABLED
#define BTU_STACK_LITE_ENABLED      FALSE
#endif

/* Transport pause time (BT slot(0.625ms) unit) when switching between BB and MM */
/* FW is using a tick which is 20 slot unit so if timeout is between 0 to 20 slot */
/* then actual timeout would be 0 to 12.5ms because it could be beteen ticks. */
/* if timeout is between 20 to 40 slot then actual timeout would be 12.5 to 25ms */
#ifndef BTU_DUAL_TRANSPORT_PAUSE_TIME
#define BTU_DUAL_TRANSPORT_PAUSE_TIME     40
#endif

/* if UART baudrate is different between BB and MM, it will be updated during switching */
#ifndef BTU_DUAL_TRANSPORT_BB_BAUDRATE
#define BTU_DUAL_TRANSPORT_BB_BAUDRATE      115200
#endif

#ifndef BTU_DUAL_TRANSPORT_MM_BAUDRATE
#define BTU_DUAL_TRANSPORT_MM_BAUDRATE      921600
#endif

/* If TRUE, stack is compiled to include the multi-av feature (A2DP packets are duplicated inside controller) */
#ifndef BTU_MULTI_AV_INCLUDED
#define BTU_MULTI_AV_INCLUDED       FALSE
#endif

/* Use 2 second for low-resolution systems, override to 1 for high-resolution systems */
#ifndef BT_1SEC_TIMEOUT
#define BT_1SEC_TIMEOUT             (2)
#endif

/* Quick Timer */
/* minimum should have 100 millisecond resolution for eL2CAP */
/* if HCILP_INCLUDED is TRUE     then it should have 100 millisecond resolution */
/* if SLIP_INCLUDED is TRUE      then it should have 10 millisecond resolution  */
/* if BRCM_USE_DELAY is FALSE then it should have 10 millisecond resolution  */
/* if none of them is included then QUICK_TIMER_TICKS_PER_SEC is set to 0 to exclude quick timer */
#ifndef QUICK_TIMER_TICKS_PER_SEC
#define QUICK_TIMER_TICKS_PER_SEC   100       /* 10ms timer */
#endif

/******************************************************************************
**
** BTM
**
******************************************************************************/
/* if set to TRUE, stack will automatically send an HCI reset at start-up. To be
set to FALSE for advanced start-up / shut-down procedures using USER_HW_ENABLE_API
and USER_HW_DISABLE_API macros */
#ifndef BTM_AUTOMATIC_HCI_RESET
#define BTM_AUTOMATIC_HCI_RESET      TRUE
#endif

/* Cancel Inquiry on incoming SSP - Work around code for a FW issue (CQ#167446). */
#ifndef BTM_NO_SSP_ON_INQUIRY
#define BTM_NO_SSP_ON_INQUIRY       FALSE
#endif

/* Include the implemenation needed by Pre-Lisbon controller (2.0_EDR or older) */
#ifndef BTM_PRE_LISBON_INCLUDED
#define BTM_PRE_LISBON_INCLUDED     TRUE
#endif

/* Includes SCO if TRUE */
#ifndef BTM_SCO_INCLUDED
#define BTM_SCO_INCLUDED            TRUE       /* TRUE includes SCO code */
#endif

/* Includes SCO if TRUE */
#ifndef BTM_SCO_HCI_INCLUDED
#define BTM_SCO_HCI_INCLUDED            FALSE       /* TRUE includes SCO over HCI code */
#endif

/* Includes WBS if TRUE */
#ifndef BTM_WBS_INCLUDED
#define BTM_WBS_INCLUDED            FALSE       /* TRUE includes WBS code */
#endif

/* Includes PCM2 support if TRUE */
#ifndef BTM_PCM2_INCLUDED
#define BTM_PCM2_INCLUDED           FALSE
#endif

/* TRUE to enable internal storage of linkkeys by stack (must implement wiced_bt_nvram_access_* functions) */
#ifndef BTM_INTERNAL_LINKKEY_STORAGE_INCLUDED
#define BTM_INTERNAL_LINKKEY_STORAGE_INCLUDED FALSE
#endif

#ifndef BTM_COEX_INCLUDED
#define BTM_COEX_INCLUDED FALSE
#endif

/**************************
** Initial SCO TX credit
*************************/
/* max TX SCO data packet size */
#ifndef BTM_SCO_DATA_SIZE_MAX
#define BTM_SCO_DATA_SIZE_MAX       240
#endif

/* maximum BTM buffering capacity */
#ifndef BTM_SCO_MAX_BUF_CAP
#define BTM_SCO_MAX_BUF_CAP     (BTM_SCO_INIT_XMIT_CREDIT * 4)
#endif

/* The size in bytes of the BTM inquiry database. */
#ifndef BTM_INQ_DB_SIZE
#define BTM_INQ_DB_SIZE             12
#endif

/* This is set to always try to acquire the remote device name. */
#ifndef BTM_INQ_GET_REMOTE_NAME
#define BTM_INQ_GET_REMOTE_NAME     FALSE
#endif

/* The inquiry duration in 1.28 second units when auto inquiry is enabled. */
#ifndef BTM_DEFAULT_INQ_DUR
#define BTM_DEFAULT_INQ_DUR         5
#endif

/* The inquiry mode when auto inquiry is enabled. */
#ifndef BTM_DEFAULT_INQ_MODE
#define BTM_DEFAULT_INQ_MODE        BTM_GENERAL_INQUIRY
#endif

/* The default periodic inquiry maximum delay when auto inquiry is enabled, in 1.28 second units. */
#ifndef BTM_DEFAULT_INQ_MAX_DELAY
#define BTM_DEFAULT_INQ_MAX_DELAY   30
#endif

/* The default periodic inquiry minimum delay when auto inquiry is enabled, in 1.28 second units. */
#ifndef BTM_DEFAULT_INQ_MIN_DELAY
#define BTM_DEFAULT_INQ_MIN_DELAY   20
#endif

/* TRUE if controller does not support inquiry event filtering. */
#ifndef BTM_BYPASS_EVENT_FILTERING
#define BTM_BYPASS_EVENT_FILTERING  FALSE
#endif

/* TRUE if inquiry filtering is desired from BTM. */
#ifndef BTM_USE_INQ_RESULTS_FILTER
#define BTM_USE_INQ_RESULTS_FILTER  TRUE
#endif

/* The default scan mode */
#ifndef BTM_DEFAULT_SCAN_TYPE
#define BTM_DEFAULT_SCAN_TYPE       BTM_SCAN_TYPE_INTERLACED
#endif

/* Should connections to unknown devices be allowed when not discoverable? */
#ifndef BTM_ALLOW_CONN_IF_NONDISCOVER
#define BTM_ALLOW_CONN_IF_NONDISCOVER   FALSE
#endif

/* When connectable mode is set to TRUE, the device will respond to paging. */
#ifndef BTM_IS_CONNECTABLE
#define BTM_IS_CONNECTABLE          FALSE
#endif

/* Sets the Page_Scan_Window:  the length of time that the device is performing a page scan. */
#ifndef BTM_DEFAULT_CONN_WINDOW
#define BTM_DEFAULT_CONN_WINDOW     0x0012
#endif

/* Sets the Page_Scan_Activity:  the interval between the start of two consecutive page scans. */
#ifndef BTM_DEFAULT_CONN_INTERVAL
#define BTM_DEFAULT_CONN_INTERVAL   0x0800
#endif

/* This is set to automatically perform inquiry scan on startup. */
#ifndef BTM_IS_DISCOVERABLE
#define BTM_IS_DISCOVERABLE         FALSE
#endif

/* When automatic inquiry scan is enabled, this sets the discovery mode. */
#ifndef BTM_DEFAULT_DISC_MODE
#define BTM_DEFAULT_DISC_MODE       BTM_GENERAL_DISCOVERABLE
#endif

/* When automatic inquiry scan is enabled, this sets the inquiry scan window. */
#ifndef BTM_DEFAULT_DISC_WINDOW
#define BTM_DEFAULT_DISC_WINDOW     0x0012
#endif

/* When automatic inquiry scan is enabled, this sets the inquiry scan interval. */
#ifndef BTM_DEFAULT_DISC_INTERVAL
#define BTM_DEFAULT_DISC_INTERVAL   0x0800
#endif

/* Number of milliseconds to delay BTU task startup upon device initialization. */
#ifndef BTU_STARTUP_DELAY
#define BTU_STARTUP_DELAY           0
#endif

/* Whether BTA is included in BTU task. */
#ifndef BTU_BTA_INCLUDED
#define BTU_BTA_INCLUDED            FALSE
#endif

/* Number of seconds to wait to send an HCI Reset command upon device initialization. */
#ifndef BTM_FIRST_RESET_DELAY
#define BTM_FIRST_RESET_DELAY       0
#endif

/* The number of seconds to wait for controller module to reset after issuing an HCI Reset command. */
#ifndef BTM_AFTER_RESET_TIMEOUT
#define BTM_AFTER_RESET_TIMEOUT     0
#endif

/* The default class of device. */
#ifndef BTM_INIT_CLASS_OF_DEVICE
#define BTM_INIT_CLASS_OF_DEVICE    "\x00\x1F\x00"
#endif

/* The number of SCO links. */
#ifndef BTM_MAX_SCO_LINKS
#define BTM_MAX_SCO_LINKS           3
#endif

/* The preferred type of SCO links (2-eSCO, 0-SCO). */
#ifndef BTM_DEFAULT_SCO_MODE
#define BTM_DEFAULT_SCO_MODE        2
#endif

/* The number of security records for peer devices. */
#ifndef BTM_SEC_MAX_DEVICE_RECORDS
#define BTM_SEC_MAX_DEVICE_RECORDS  8
#endif

/* Size of host-based private address resolution table */
#ifndef BTM_SEC_HOST_PRIVACY_ADDR_RESOLUTION_TABLE_SIZE
#define BTM_SEC_HOST_PRIVACY_ADDR_RESOLUTION_TABLE_SIZE   50
#endif

/* The number of security records for services. */
#ifndef BTM_SEC_MAX_SERVICE_RECORDS
#define BTM_SEC_MAX_SERVICE_RECORDS 24
#endif

/* If True, force a retrieval of remote device name for each bond in case it's changed */
#ifndef BTM_SEC_FORCE_RNR_FOR_DBOND
#define BTM_SEC_FORCE_RNR_FOR_DBOND  TRUE
#endif

/* Maximum device name length used in btm database. */
#ifndef BTM_MAX_REM_BD_NAME_LEN
#define BTM_MAX_REM_BD_NAME_LEN     20
#endif

/* Maximum local device name length stored btm database.
  '0' disables storage of the local name in BTM */
#ifndef BTM_MAX_LOC_BD_NAME_LEN
#define BTM_MAX_LOC_BD_NAME_LEN     31
#endif

/* TRUE if default string is used, FALSE if device name is set in the application */
#ifndef BTM_USE_DEF_LOCAL_NAME
#define BTM_USE_DEF_LOCAL_NAME      FALSE
#endif

/* Fixed Default String (Ignored if BTM_USE_DEF_LOCAL_NAME is FALSE) */
#ifndef BTM_DEF_LOCAL_NAME
#define BTM_DEF_LOCAL_NAME      ""
#endif

/* Maximum service name stored with security authorization (0 if not needed) */
#ifndef BTM_SEC_SERVICE_NAME_LEN
#define BTM_SEC_SERVICE_NAME_LEN    BT_MAX_SERVICE_NAME_LEN
#endif

/* Maximum number of pending security callback */
#ifndef BTM_SEC_MAX_CALLBACKS
#define BTM_SEC_MAX_CALLBACKS       7
#endif

/* Maximum length of the service name. */
#ifndef BT_MAX_SERVICE_NAME_LEN
#define BT_MAX_SERVICE_NAME_LEN     21
#endif

/* ACL buffer size in HCI Host Buffer Size command. */
#ifndef BTM_ACL_BUF_SIZE
#define BTM_ACL_BUF_SIZE            0
#endif

/* This is set to use the BTM power manager. */
#ifndef BTM_PWR_MGR_INCLUDED
#define BTM_PWR_MGR_INCLUDED        TRUE
#endif

/* The maximum number of clients that can register with the power manager. */
#ifndef BTM_MAX_PM_RECORDS
#define BTM_MAX_PM_RECORDS          2
#endif

/* This is set to show debug trace messages for the power manager. */
#ifndef BTM_PM_DEBUG
#define BTM_PM_DEBUG                TRUE
#endif

/* This is set to TRUE if link is to be unparked due to BTM_CreateSCO API. */
#ifndef BTM_SCO_WAKE_PARKED_LINK
#define BTM_SCO_WAKE_PARKED_LINK    TRUE
#endif

/* May be set to the the name of a function used for vendor specific chip initialization */
#ifndef BTM_APP_DEV_INIT
/* #define BTM_APP_DEV_INIT         myInitFunction() */
#endif

/* This is set to TRUE if the busy level change event is desired. (replace ACL change event) */
#ifndef BTM_BUSY_LEVEL_CHANGE_INCLUDED
#define BTM_BUSY_LEVEL_CHANGE_INCLUDED  TRUE
#endif

/* If the user does not respond to security process requests within this many seconds,
 * a negative response would be sent automatically.
 * It's recommended to use a value between 30 and OBX_TIMEOUT_VALUE
 * 30 is LMP response timeout value */
#ifndef BTM_SEC_TIMEOUT_VALUE
#define BTM_SEC_TIMEOUT_VALUE           35
#endif

/* Maximum number of callbacks that can be registered using BTM_RegisterForVSEvents */
#ifndef BTM_MAX_VSE_CALLBACKS
#define BTM_MAX_VSE_CALLBACKS           6
#endif

/* Number of streams for dual stack */
#ifndef BTM_SYNC_INFO_NUM_STR
#define BTM_SYNC_INFO_NUM_STR           2
#endif

/* Number of streams for dual stack in BT Controller */
#ifndef BTM_SYNC_INFO_NUM_STR_BTC
#define BTM_SYNC_INFO_NUM_STR_BTC       2
#endif

/******************************************
**    Lisbon Features
*******************************************/
/* This is set to TRUE if the server Extended Inquiry Response feature is desired. */
/* server sends EIR to client */
#ifndef BTM_EIR_SERVER_INCLUDED
#define BTM_EIR_SERVER_INCLUDED         TRUE
#endif

/* This is set to TRUE if the client Extended Inquiry Response feature is desired. */
/* client inquiry to server */
#ifndef BTM_EIR_CLIENT_INCLUDED
#define BTM_EIR_CLIENT_INCLUDED         TRUE
#endif

/* This is set to TRUE if the FEC is required for EIR packet. */
#ifndef BTM_EIR_DEFAULT_FEC_REQUIRED
#define BTM_EIR_DEFAULT_FEC_REQUIRED    TRUE
#endif

/* User defined UUID look up table */
#ifndef BTM_EIR_UUID_LKUP_TBL
#endif

/* Default security mode - SSP */
#ifndef BTM_DEFAULT_SECURITY_MODE
#define BTM_DEFAULT_SECURITY_MODE       BTM_SEC_MODE_SP
#endif

/* The IO capability of the local device (for Simple Pairing) */
#ifndef BTM_LOCAL_IO_CAPS
#define BTM_LOCAL_IO_CAPS               BTM_IO_CAP_IO
#endif

/* The default MITM Protection Requirement (for Simple Pairing)
 * Possible values are BTM_AUTH_SP_YES or BTM_AUTH_SP_NO */
#ifndef BTM_DEFAULT_AUTH_REQ
#define BTM_DEFAULT_AUTH_REQ            BTM_AUTH_SP_NO
#endif

/* The default MITM Protection Requirement for dedicated bonding using Simple Pairing
 * Possible values are BTM_AUTH_AP_YES or BTM_AUTH_AP_NO */
#ifndef BTM_DEFAULT_DD_AUTH_REQ
#define BTM_DEFAULT_DD_AUTH_REQ         BTM_AUTH_AP_YES
#endif

/* Include Out-of-Band implementation for Simple Pairing */
#ifndef BTM_OOB_INCLUDED
#define BTM_OOB_INCLUDED                TRUE
#endif

/* TRUE to include Sniff Subrating */
#ifndef BTM_SSR_INCLUDED
#define BTM_SSR_INCLUDED                TRUE
#endif

/* Support for BR/EDR Secure Connections */
#ifndef BTM_BR_SC_INCLUDED
#define BTM_BR_SC_INCLUDED              FALSE
#endif

/* Support for tester in conformance tests */
#ifndef BTM_SECURE_CONNECTIONS_CONFORMANCE_TEST
#define BTM_SECURE_CONNECTIONS_CONFORMANCE_TEST FALSE
#endif

/* support for cross transport key derivation (LE LTK <-> BR/EDR LK) */
#ifndef BTM_CROSS_TRANSP_KEY_DERIVATION
#define BTM_CROSS_TRANSP_KEY_DERIVATION     FALSE
#endif

/*************************
** End of Lisbon Features
**************************/

/* This is set to use the BTM TBFC Supported. */
#ifndef BTM_TBFC_INCLUDED
#define BTM_TBFC_INCLUDED               FALSE
#endif

/* TRUE to include bi-directionnal TBFC */
#ifndef BTM_BI_DIR_TBFC_INCLUDED
#define BTM_BI_DIR_TBFC_INCLUDED        FALSE
#endif

#ifndef BTM_TBFC_LE_TRIGGER_INCLUDED
#define BTM_TBFC_LE_TRIGGER_INCLUDED               FALSE
#endif


/* TRUE to include CLB (ConnectionLess Slave Broadcast) */
#ifndef BTM_CLB_INCLUDED
#define BTM_CLB_INCLUDED                FALSE
#endif

/* TRUE to include CLB Rx (ConnectionLess Broadcast Reception) */
#ifndef BTM_CLB_RX_INCLUDED
#define BTM_CLB_RX_INCLUDED             FALSE
#endif

/* Used for conformance testing ONLY */
#ifndef BTM_BLE_CONFORMANCE_TESTING
#define BTM_BLE_CONFORMANCE_TESTING     FALSE
#endif

#ifndef BLE_BRCM_MULTI_ADV_INCLUDED
#define BLE_BRCM_MULTI_ADV_INCLUDED     FALSE
#endif


#ifndef BTM_BLE_MAX_BG_CONN_DEV_NUM
#define BTM_BLE_MAX_BG_CONN_DEV_NUM    10
#endif



/******************************************************************************
**
** L2CAP
**
******************************************************************************/

/* The maximum number of simultaneous links that L2CAP can support. */
#ifndef MAX_L2CAP_LINKS
#define MAX_L2CAP_LINKS             4
#endif

/* The maximum number of simultaneous channels that L2CAP can support. */
#ifndef MAX_L2CAP_CHANNELS
#define MAX_L2CAP_CHANNELS          10
#endif

/* The maximum number of simultaneous channels that L2CAP BLE can support. */
#ifndef MAX_L2CAP_BLE_CHANNELS
#define MAX_L2CAP_BLE_CHANNELS      10
#endif

/* The maximum number of simultaneous applications that can register with L2CAP. */
#ifndef MAX_L2CAP_CLIENTS
#define MAX_L2CAP_CLIENTS           8
#endif

/* The maximum number of simultaneous applications that can register with L2CAP BLE. */
#ifndef MAX_L2CAP_BLE_CLIENTS
#define MAX_L2CAP_BLE_CLIENTS       8
#endif

/* The number of seconds of link inactivity before a link is disconnected. */
#ifndef L2CAP_LINK_INACTIVITY_TOUT
#define L2CAP_LINK_INACTIVITY_TOUT  3
#endif

/* The number of seconds of link inactivity after bonding before a link is disconnected. */
#ifndef L2CAP_BONDING_TIMEOUT
#define L2CAP_BONDING_TIMEOUT       3
#endif

/* The time from the HCI connection complete to disconnect if no channel is established. */
#ifndef L2CAP_LINK_STARTUP_TOUT
#define L2CAP_LINK_STARTUP_TOUT     60
#endif

/* The L2CAP MTU; must be in accord with the HCI ACL pool size. */
#ifndef L2CAP_MTU_SIZE
#define L2CAP_MTU_SIZE              1691
#endif

/* The L2CAP MPS over Bluetooth; must be in accord with the FCR tx pool size and ACL down buffer size. */
#ifndef L2CAP_MPS_OVER_BR_EDR
#define L2CAP_MPS_OVER_BR_EDR       1010
#endif

/* This is set to enable host flow control. */
#ifndef L2CAP_HOST_FLOW_CTRL
#define L2CAP_HOST_FLOW_CTRL        FALSE
#endif

/* If host flow control enabled, this is the number of buffers the controller can have unacknowledged. */
#ifndef L2CAP_HOST_FC_ACL_BUFS
#define L2CAP_HOST_FC_ACL_BUFS      20
#endif

/* The percentage of the queue size allowed before a congestion event is sent to the L2CAP client (typically 120%). */
#ifndef L2CAP_FWD_CONG_THRESH
#define L2CAP_FWD_CONG_THRESH       120
#endif

/* This is set to enable L2CAP to  take the ACL link out of park mode when ACL data is to be sent. */
#ifndef L2CAP_WAKE_PARKED_LINK
#define L2CAP_WAKE_PARKED_LINK      TRUE
#endif

/* Whether link wants to be the master or the slave. */
#ifndef L2CAP_DESIRED_LINK_ROLE
#define L2CAP_DESIRED_LINK_ROLE     HCI_ROLE_SLAVE
#endif

/* Include Non-Flushable Packet Boundary Flag feature of Lisbon */
#ifndef L2CAP_NON_FLUSHABLE_PB_INCLUDED
#define L2CAP_NON_FLUSHABLE_PB_INCLUDED     TRUE
#endif

/* max queued Multi-AV packets per link including controller */
#ifndef L2CAP_MULTI_AV_TOTAL_QUEUED_BUF
#define L2CAP_MULTI_AV_TOTAL_QUEUED_BUF     6
#endif

/* max links supported by Multi-AV feature */
#ifndef L2CAP_MAX_MULTI_AV_CID
#define L2CAP_MAX_MULTI_AV_CID              5
#endif

/* Minimum number of ACL credit for high priority link */
#ifndef L2CAP_HIGH_PRI_MIN_XMIT_QUOTA
#define L2CAP_HIGH_PRI_MIN_XMIT_QUOTA       4
#endif

/* used for monitoring HCI ACL credit management */
#ifndef L2CAP_HCI_FLOW_CONTROL_DEBUG
#define L2CAP_HCI_FLOW_CONTROL_DEBUG        TRUE
#endif

/* Used for calculating transmit buffers off of */
#ifndef L2CAP_NUM_XMIT_BUFFS
#define L2CAP_NUM_XMIT_BUFFS                HCI_ACL_BUF_MAX
#endif

/* Unicast Connectionless Data */
#ifndef L2CAP_UCD_INCLUDED
#define L2CAP_UCD_INCLUDED                  FALSE
#endif

/* Unicast Connectionless Data MTU */
#ifndef L2CAP_UCD_MTU
#define L2CAP_UCD_MTU                       L2CAP_MTU_SIZE
#endif

/* Unicast Connectionless Data Idle Timeout */
#ifndef L2CAP_UCD_IDLE_TIMEOUT
#define L2CAP_UCD_IDLE_TIMEOUT              2
#endif

/* Unicast Connectionless Data Idle Timeout */
#ifndef L2CAP_UCD_CH_PRIORITY
#define L2CAP_UCD_CH_PRIORITY               L2CAP_CHNL_PRIORITY_MEDIUM
#endif

/* Max clients on Unicast Connectionless Data */
#ifndef L2CAP_MAX_UCD_CLIENTS
#define L2CAP_MAX_UCD_CLIENTS               5
#endif

/* Used for features using fixed channels; set to false if no fixed channels supported (AMP, BLE, etc.) */
#ifndef L2CAP_FIXED_CHANNELS_SUPPORTED
#define L2CAP_FIXED_CHANNELS_SUPPORTED TRUE
#endif

#if defined(L2CAP_FIXED_CHANNELS_SUPPORTED) && ( L2CAP_FIXED_CHANNELS_SUPPORTED == TRUE )
#define L2CAP_MIN_NUM_OF_FIXED_CHANNELS_SUPPORTED 3
#endif

/* Excluding L2CAP signaling channel and UCD */
#ifndef L2CAP_NUM_FIXED_CHNLS
#define L2CAP_NUM_FIXED_CHNLS              (p_btm_cfg_settings->l2cap_application.max_le_l2cap_fixed_channels + L2CAP_MIN_NUM_OF_FIXED_CHANNELS_SUPPORTED)
#endif


/* Excluding L2CAP signaling channel and UCD */
#ifndef L2CAP_NUM_FIXED_CHNLS
#define L2CAP_NUM_FIXED_CHNLS               4
#endif

/* First fixed channel supported; 3 if AMP supported */
#ifndef L2CAP_FIRST_FIXED_CHNL
#define L2CAP_FIRST_FIXED_CHNL              4
#endif

#ifndef L2CAP_LAST_FIXED_CHNL
/* According to spec. we are expected to support cid 4, 5 and 6 for at minimum for LE.
cid's (0x7 - 0x1f) are reseved and (0x20-0x3e) are assigned numbers. Hence the max.
valid fixed channel id is 0x3e*/
#define L2CAP_LAST_FIXED_CHNL           (0x3E)
#endif

/* Round Robin service channels in link */
#ifndef L2CAP_ROUND_ROBIN_CHANNEL_SERVICE
#define L2CAP_ROUND_ROBIN_CHANNEL_SERVICE   TRUE
#endif

/* Used for calculating transmit buffers off of */
#ifndef L2CAP_NUM_XMIT_BUFFS
#define L2CAP_NUM_XMIT_BUFFS                HCI_ACL_BUF_MAX
#endif

/* used for monitoring eL2CAP data flow */
#ifndef L2CAP_ERTM_STATS
#define L2CAP_ERTM_STATS                    FALSE
#endif

/* Set to TRUE to support L2CAP BLE Connection-Oriented Channels (COC).
** Can be set to TRUE only if BLE_INCLUDED is set to TRUE. */
#ifndef L2CAP_LE_COC_INCLUDED
#define L2CAP_LE_COC_INCLUDED            FALSE
#endif

/* USED FOR FCR TEST ONLY:  When TRUE generates bad tx and rx packets */
#ifndef L2CAP_CORRUPT_ERTM_PKTS
#define L2CAP_CORRUPT_ERTM_PKTS             FALSE
#endif

/* Used for conformance testing ONLY:  When TRUE lets scriptwrapper overwrite info response */
#ifndef L2CAP_CONFORMANCE_TESTING
#define L2CAP_CONFORMANCE_TESTING           FALSE
#endif

/* Used for conformance testing ONLY */
#ifndef L2CAP_LE_COC_CONFORMANCE_TESTING
#define L2CAP_LE_COC_CONFORMANCE_TESTING    FALSE
#endif

/******************************************************************************
**
** BLE
**
******************************************************************************/

/* Bluetooth low energy(BLE) or Bluetooth smart */
#ifndef BLE_INCLUDED
#define BLE_INCLUDED            FALSE
#endif

/* BLE connection parameter request procedure support */
#ifndef BLE_LLT_INCLUDED
#define BLE_LLT_INCLUDED        TRUE
#endif

/* BLE data packet length extension support */
#ifndef BLE_DATA_LEN_EXT_INCLUDED
#define BLE_DATA_LEN_EXT_INCLUDED        TRUE
#endif

/* Local BLE controller identifier */
#ifndef LOCAL_BLE_CONTROLLER_ID
#define LOCAL_BLE_CONTROLLER_ID         (1)
#endif

/* BLE link layer privacy support */
#ifndef BTM_BLE_PRIVACY_SPT
#define BTM_BLE_PRIVACY_SPT         FALSE
#endif

/* BLE host address resolution support */
#ifndef BTM_BLE_HOST_ADDR_RESOLUTION
#define BTM_BLE_HOST_ADDR_RESOLUTION FALSE
#endif

/* Host BLE support */
#ifndef BTM_BLE_HOST_SUPPORT
#define BTM_BLE_HOST_SUPPORT        0x01
#endif

/* Host simultaneous BLE and BR/EDR support */
#ifndef BTM_BLE_SIMULTANEOUS_HOST
#define BTM_BLE_SIMULTANEOUS_HOST    0x01
#endif

/* BLE private address timeout(time the controller uses the address before a new address is generated) */
#ifndef BTM_BLE_PRIVATE_ADDR_INT
#define BTM_BLE_PRIVATE_ADDR_INT    900           /* 15 minutes minimum for random address refreshing */
#endif

/* BLE default connection parameters if not configured, use GAP recommend value for auto/selective connection */
/* default scan interval */
#ifndef BTM_BLE_SCAN_FAST_INT
#define BTM_BLE_SCAN_FAST_INT    96    /* 30 ~ 60 ms (use 60)  = 96 * 0.625 */
#endif
/* default scan window for background connection, applicable for auto connection or selective connection */
#ifndef BTM_BLE_SCAN_FAST_WIN
#define BTM_BLE_SCAN_FAST_WIN   48      /* 30 ms = 48 * 0.625 */
#endif

/* BLE default scan paramter 1 used in reduced power cycle (background scanning) */
/* default scan interval */
#ifndef BTM_BLE_SCAN_SLOW_INT_1
#define BTM_BLE_SCAN_SLOW_INT_1    2048    /* 1.28 s   = 2048 * 0.625 */
#endif
/* default scan window */
#ifndef BTM_BLE_SCAN_SLOW_WIN_1
#define BTM_BLE_SCAN_SLOW_WIN_1   18      /* 11.25 ms = 18 * 0.625 */
#endif

/* BLE default scan paramter 2 used in reduced power cycle (background scanning) */
/* default scan interval */
#ifndef BTM_BLE_SCAN_SLOW_INT_2
#define BTM_BLE_SCAN_SLOW_INT_2    4096    /* 2.56 s   = 4096 * 0.625 */
#endif
/* default scan window */
#ifndef BTM_BLE_SCAN_SLOW_WIN_2
#define BTM_BLE_SCAN_SLOW_WIN_2   36      /* 22.5 ms = 36 * 0.625 */
#endif

/* BLE default connection interval min */
#ifndef BTM_BLE_CONN_INT_MIN_DEF
#define BTM_BLE_CONN_INT_MIN_DEF     24      /* recommended min: 30ms  = 24 * 1.25 */
#endif

/* BLR default connectino interval max */
#ifndef BTM_BLE_CONN_INT_MAX_DEF
#define BTM_BLE_CONN_INT_MAX_DEF     40      /* recommended max: 50 ms = 40 * 1.25 */
#endif

/* BLE default slave latency */
#ifndef BTM_BLE_CONN_SLAVE_LATENCY_DEF
#define BTM_BLE_CONN_SLAVE_LATENCY_DEF  0      /* 0 */
#endif

/* BLE default supervision timeout */
#ifndef BTM_BLE_CONN_TIMEOUT_DEF
#define BTM_BLE_CONN_TIMEOUT_DEF    2000
#endif

/* HID over BLE support */
#ifndef HID_LE_INCLUDED
#define HID_LE_INCLUDED         FALSE
#endif

/* Broadcom BLE support */
#ifndef BLE_BRCM_INCLUDED
#define BLE_BRCM_INCLUDED         FALSE
#endif


/******************************************************************************
**
** ATT/GATT Protocol/Profile Settings
**
******************************************************************************/
#ifndef ATT_DEBUG
#define ATT_DEBUG           FALSE
#endif

/* GATT maximun server profiles */
#ifndef GATT_MAX_SR_PROFILES
#define GATT_MAX_SR_PROFILES        32 /* max is 32 */
#endif

/* GTAA fixed database support */
#ifndef GATT_FIXED_DB
#define GATT_FIXED_DB                   TRUE
#endif

#ifndef GATTS_APPU_USE_GATT_TRACE
#define GATTS_APPU_USE_GATT_TRACE       ATT_DEBUG
#endif

/* GATT maximun applications */
#ifndef GATT_MAX_APPS
#define GATT_MAX_APPS            10 /* note: 2 apps used internally GATT and GAP */
#endif

/* GATT maximun physical channels */
#ifndef GATT_MAX_PHY_CHANNEL
#define GATT_MAX_PHY_CHANNEL        4 /* limited by the BLE controller upto 15 */
#endif

/* Used for conformance testing ONLY */
#ifndef GATT_CONFORMANCE_TESTING
#define GATT_CONFORMANCE_TESTING           FALSE
#endif

/* number of background connection device allowence, ideally to be the same as WL size
*/
#ifndef GATT_MAX_BG_CONN_DEV
#define GATT_MAX_BG_CONN_DEV        32
#endif

#ifndef GATT_OVER_BREDR_INCLUDED
#define GATT_OVER_BREDR_INCLUDED TRUE
#endif

/******************************************************************************
**
** SMP
**
******************************************************************************/
#ifndef SMP_INCLUDED
#define SMP_INCLUDED         FALSE
#endif

#ifndef SMP_DEBUG
#define SMP_DEBUG            FALSE
#endif

#ifndef SMP_DEBUG1
#define SMP_DEBUG1            FALSE
#endif

#ifndef SMP_DEFAULT_AUTH_REQ
#define SMP_DEFAULT_AUTH_REQ    SMP_AUTH_NB_ENC_ONLY
#endif

#ifndef SMP_MAX_ENC_KEY_SIZE
#define SMP_MAX_ENC_KEY_SIZE    16
#endif

#ifndef SMP_MIN_ENC_KEY_SIZE
#define SMP_MIN_ENC_KEY_SIZE    7
#endif

/* Used for conformance testing ONLY */
#ifndef SMP_CONFORMANCE_TESTING
#define SMP_CONFORMANCE_TESTING           FALSE
#endif

/* Support for LE Secure Connections */
#ifndef SMP_LE_SC_INCLUDED
#define SMP_LE_SC_INCLUDED              FALSE
#endif

/* Support for LE Secure Connections OOB */
#ifndef SMP_LE_SC_OOB_INCLUDED
#define SMP_LE_SC_OOB_INCLUDED          FALSE
#endif

/* minimum link timeout after SMP pairing is done, leave room for key exchange
   and racing condition for the following service connection.
   Prefer greater than 0 second, and no less than default inactivity link idle
   timer(L2CAP_LINK_INACTIVITY_TOUT) in l2cap) */
#ifndef SMP_LINK_TOUT_MIN
#if (L2CAP_LINK_INACTIVITY_TOUT > 0)
#define SMP_LINK_TOUT_MIN               L2CAP_LINK_INACTIVITY_TOUT
#else
#define SMP_LINK_TOUT_MIN               2
#endif
#endif

/******************************************************************************
**
** SDP
**
******************************************************************************/

/* This is set to enable SDP server functionality. */
#ifndef SDP_SERVER_ENABLED
#define SDP_SERVER_ENABLED          TRUE
#endif

/* The maximum number of SDP records the server can support. */
#ifndef SDP_MAX_RECORDS
#define SDP_MAX_RECORDS             20
#endif

/* The maximum number of attributes in each record. */
#ifndef SDP_MAX_REC_ATTR
#define SDP_MAX_REC_ATTR            25
#endif

#ifndef SDP_MAX_PAD_LEN
#define SDP_MAX_PAD_LEN             350
#endif

/* The maximum length, in bytes, of an attribute. */
#ifndef SDP_MAX_ATTR_LEN
#define SDP_MAX_ATTR_LEN            256
#endif

/* The maximum number of attribute filters supported by SDP databases. */
#ifndef SDP_MAX_ATTR_FILTERS
#define SDP_MAX_ATTR_FILTERS        12
#endif

/* The maximum number of UUID filters supported by SDP databases. */
#ifndef SDP_MAX_UUID_FILTERS
#define SDP_MAX_UUID_FILTERS        3
#endif

/* This is set to enable SDP client functionality. */
#ifndef SDP_CLIENT_ENABLED
#define SDP_CLIENT_ENABLED          TRUE
#endif

/* The maximum number of record handles retrieved in a search. */
#ifndef SDP_MAX_DISC_SERVER_RECS
#define SDP_MAX_DISC_SERVER_RECS    21
#endif

/* The size of a scratchpad buffer, in bytes, for storing the response to an attribute request. */
#ifndef SDP_MAX_LIST_BYTE_COUNT
#define SDP_MAX_LIST_BYTE_COUNT     1000
#endif

/* The maximum number of parameters in an SDP protocol element. */
#ifndef SDP_MAX_PROTOCOL_PARAMS
#define SDP_MAX_PROTOCOL_PARAMS     2
#endif

/* The maximum number of simultaneous client and server connections. */
#ifndef SDP_MAX_CONNECTIONS
#define SDP_MAX_CONNECTIONS         4
#endif

/* The MTU size for the L2CAP configuration. */
#ifndef SDP_MTU_SIZE
#define SDP_MTU_SIZE                256
#endif

/* The flush timeout for the L2CAP configuration. */
#ifndef SDP_FLUSH_TO
#define SDP_FLUSH_TO                0xFFFF
#endif

/* The name for security authorization. */
#ifndef SDP_SERVICE_NAME
#define SDP_SERVICE_NAME            "Service Discovery"
#endif

/* The security level for BTM. */
#ifndef SDP_SECURITY_LEVEL
#define SDP_SECURITY_LEVEL          BTM_SEC_NONE
#endif

/******************************************************************************
**
** RFCOMM
**
******************************************************************************/

#ifndef RFCOMM_INCLUDED
#define RFCOMM_INCLUDED             FALSE
#endif

/* The maximum number of ports supported. */
#ifndef MAX_RFC_PORTS
#define MAX_RFC_PORTS               5
#endif

/* The maximum simultaneous links to different devices. */
#ifndef MAX_BD_CONNECTIONS
#define MAX_BD_CONNECTIONS          1
#endif

/* The port receive queue low watermark level, in bytes. */
#ifndef PORT_RX_LOW_WM
#define PORT_RX_LOW_WM              5000
#endif

/* The port receive queue high watermark level, in bytes. */
#ifndef PORT_RX_HIGH_WM
#define PORT_RX_HIGH_WM             8000
#endif

/* The port receive queue critical watermark level, in bytes. */
#ifndef PORT_RX_CRITICAL_WM
#define PORT_RX_CRITICAL_WM         12000
#endif

/* The port receive queue low watermark level, in number of buffers. */
#ifndef PORT_RX_BUF_LOW_WM
#define PORT_RX_BUF_LOW_WM          8
#endif

/* The port receive queue high watermark level, in number of buffers. */
#ifndef PORT_RX_BUF_HIGH_WM
#define PORT_RX_BUF_HIGH_WM         16
#endif

/* The port receive queue critical watermark level, in number of buffers. */
#ifndef PORT_RX_BUF_CRITICAL_WM
#define PORT_RX_BUF_CRITICAL_WM     22
#endif

/* The port transmit queue high watermark level, in bytes. */
#ifndef PORT_TX_HIGH_WM
#define PORT_TX_HIGH_WM             8000
#endif

/* The port transmit queue critical watermark level, in bytes. */
#ifndef PORT_TX_CRITICAL_WM
#define PORT_TX_CRITICAL_WM         10000
#endif

/* The port transmit queue high watermark level, in number of buffers. */
#ifndef PORT_TX_BUF_HIGH_WM
#define PORT_TX_BUF_HIGH_WM         16
#endif

/* The port transmit queue high watermark level, in number of buffers. */
#ifndef PORT_TX_BUF_CRITICAL_WM
#define PORT_TX_BUF_CRITICAL_WM     22
#endif

/* The RFCOMM multiplexer preferred flow control mechanism. */
#ifndef PORT_FC_DEFAULT
#define PORT_FC_DEFAULT             PORT_FC_CREDIT
#endif

/* The maximum number of credits receiver sends to peer when using credit-based flow control. */
#ifndef PORT_CREDIT_RX_MAX
#define PORT_CREDIT_RX_MAX          16
#endif

/* The credit low watermark level. */
#ifndef PORT_CREDIT_RX_LOW
#define PORT_CREDIT_RX_LOW          8
#endif

/* if application like BTA, Java or script test engine is running on other than BTU thread, */
/* PORT_SCHEDULE_LOCK shall be defined as GKI_sched_lock() or GKI_disable() */
#ifndef PORT_SCHEDULE_LOCK
#define PORT_SCHEDULE_LOCK          GKI_disable()
#endif

/* if application like BTA, Java or script test engine is running on other than BTU thread, */
/* PORT_SCHEDULE_LOCK shall be defined as GKI_sched_unlock() or GKI_enable() */
#ifndef PORT_SCHEDULE_UNLOCK
#define PORT_SCHEDULE_UNLOCK        GKI_enable()
#endif


/******************************************************************************
**
** OBX
**
******************************************************************************/
#ifndef OBX_INCLUDED
#define OBX_INCLUDED               FALSE
#endif

#ifndef OBX_CLIENT_INCLUDED
#define OBX_CLIENT_INCLUDED        TRUE
#endif

#ifndef OBX_SERVER_INCLUDED
#define OBX_SERVER_INCLUDED        TRUE
#endif

/* TRUE to include OBEX authentication/MD5 test code */
#ifndef OBX_MD5_TEST_INCLUDED
#define OBX_MD5_TEST_INCLUDED       FALSE
#endif

/* The timeout value (in seconds) for reliable sessions to remain in suspend. 0xFFFFFFFF for no timeout event. */
#ifndef OBX_SESS_TIMEOUT_VALUE
#define OBX_SESS_TIMEOUT_VALUE      600
#endif

/* The idle timeout value. 0 for no timeout event. */
#ifndef OBX_TIMEOUT_VALUE
#define OBX_TIMEOUT_VALUE           60
#endif

/* Timeout value used for disconnect */
#ifndef OBX_DISC_TOUT_VALUE
#define OBX_DISC_TOUT_VALUE         5
#endif

/* The maximum number of registered servers. */
#ifndef OBX_NUM_SERVERS
#define OBX_NUM_SERVERS             25
#endif

/* The maximum number of sessions for all registered servers.
 * (must be equal or bigger than OBX_NUM_SERVERS) */
#ifndef OBX_NUM_SR_SESSIONS
#define OBX_NUM_SR_SESSIONS         30
#endif

/* The maximum number of sessions per registered server.
 * must be less than MAX_BD_CONNECTIONS */
#ifndef OBX_MAX_SR_SESSION
#define OBX_MAX_SR_SESSION          4
#endif

/* The maximum number of suspended sessions per registered servers. */
#ifndef OBX_MAX_SUSPEND_SESSIONS
#define OBX_MAX_SUSPEND_SESSIONS    4
#endif

/* The maximum number of active clients. */
#ifndef OBX_NUM_CLIENTS
#define OBX_NUM_CLIENTS             8
#endif

/* The maximum length of OBEX target header.*/
#ifndef OBX_MAX_TARGET_LEN
#define OBX_MAX_TARGET_LEN          16
#endif

/* The maximum length of authentication challenge realm.*/
#ifndef OBX_MAX_REALM_LEN
#define OBX_MAX_REALM_LEN           30
#endif

/* The maximum of GKI buffer queued at OBX before flow control L2CAP */
#ifndef OBX_MAX_RX_QUEUE_COUNT
#define OBX_MAX_RX_QUEUE_COUNT      3
#endif

/* This option is application when OBEX over L2CAP is in use
   Pool ID where to reassemble the SDU.
   This Pool will allow buffers to be used that are larger than
   the L2CAP_MAX_MTU. */
#ifndef OBX_USER_RX_POOL_ID
#define OBX_USER_RX_POOL_ID     OBX_LRG_DATA_POOL_ID
#endif

/* This option is application when OBEX over L2CAP is in use
   Pool ID where to hold the SDU.
   This Pool will allow buffers to be used that are larger than
   the L2CAP_MAX_MTU. */
#ifndef OBX_USER_TX_POOL_ID
#define OBX_USER_TX_POOL_ID     OBX_LRG_DATA_POOL_ID
#endif

/* This option is application when OBEX over L2CAP is in use
GKI Buffer Pool ID used to hold MPS segments during SDU reassembly
*/
#ifndef OBX_FCR_RX_POOL_ID
#define OBX_FCR_RX_POOL_ID      HCI_ACL_POOL_ID
#endif

/* This option is application when OBEX over L2CAP is in use
GKI Buffer Pool ID used to hold MPS segments used in (re)transmissions.
L2CAP_DEFAULT_ERM_POOL_ID is specified to use the HCI ACL data pool.
Note:  This pool needs to have enough buffers to hold two times the window size negotiated
 in the L2CA_SetFCROptions (2 * tx_win_size)  to allow for retransmissions.
 The size of each buffer must be able to hold the maximum MPS segment size passed in
 L2CA_SetFCROptions plus BT_HDR (8) + HCI preamble (4) + L2CAP_MIN_OFFSET (11 - as of BT 2.1 + EDR Spec).
*/
#ifndef OBX_FCR_TX_POOL_ID
#define OBX_FCR_TX_POOL_ID      HCI_ACL_POOL_ID
#endif

/* This option is application when OBEX over L2CAP is in use
Size of the transmission window when using enhanced retransmission mode. Not used
in basic and streaming modes. Range: 1 - 63
*/
#ifndef OBX_FCR_OPT_TX_WINDOW_SIZE_BR_EDR
#define OBX_FCR_OPT_TX_WINDOW_SIZE_BR_EDR       20
#endif

/* This option is application when OBEX over L2CAP is in use
Number of transmission attempts for a single I-Frame before taking
Down the connection. Used In ERTM mode only. Value is Ignored in basic and
Streaming modes.
Range: 0, 1-0xFF
0 - infinite retransmissions
1 - single transmission
*/
#ifndef OBX_FCR_OPT_MAX_TX_B4_DISCNT
#define OBX_FCR_OPT_MAX_TX_B4_DISCNT    20
#endif

/* This option is application when OBEX over L2CAP is in use
Retransmission Timeout
Range: Minimum 2000 (2 secs) on BR/EDR when supporting PBF.
 */
#ifndef OBX_FCR_OPT_RETX_TOUT
#define OBX_FCR_OPT_RETX_TOUT           2000
#endif

/* This option is application when OBEX over L2CAP is in use
Monitor Timeout
Range: Minimum 12000 (12 secs) on BR/EDR when supporting PBF.
*/
#ifndef OBX_FCR_OPT_MONITOR_TOUT
#define OBX_FCR_OPT_MONITOR_TOUT        12000
#endif

/******************************************************************************
**
** BNEP
**
******************************************************************************/

#ifndef BNEP_INCLUDED
#define BNEP_INCLUDED               FALSE
#endif

/* Protocol filtering is an optional feature. Bydefault it will be turned on */
#ifndef BNEP_SUPPORTS_PROT_FILTERS
#define BNEP_SUPPORTS_PROT_FILTERS          TRUE
#endif

/* Multicast filtering is an optional feature. Bydefault it will be turned on */
#ifndef BNEP_SUPPORTS_MULTI_FILTERS
#define BNEP_SUPPORTS_MULTI_FILTERS         TRUE
#endif

/* BNEP status API call is used mainly to get the L2CAP handle */
#ifndef BNEP_SUPPORTS_STATUS_API
#define BNEP_SUPPORTS_STATUS_API            TRUE
#endif

/* This is just a debug function */
#ifndef BNEP_SUPPORTS_DEBUG_DUMP
#define BNEP_SUPPORTS_DEBUG_DUMP            TRUE
#endif

#ifndef BNEP_SUPPORTS_ALL_UUID_LENGTHS
#define BNEP_SUPPORTS_ALL_UUID_LENGTHS      TRUE    /* Otherwise it will support only 16bit UUIDs */
#endif

/*
** When BNEP connection changes roles after the connection is established
** we will do an authentication check again on the new role
*/
#ifndef BNEP_DO_AUTH_FOR_ROLE_SWITCH
#define BNEP_DO_AUTH_FOR_ROLE_SWITCH        TRUE
#endif


/* Maximum number of protocol filters supported. */
#ifndef BNEP_MAX_PROT_FILTERS
#define BNEP_MAX_PROT_FILTERS       5
#endif

/* Maximum number of multicast filters supported. */
#ifndef BNEP_MAX_MULTI_FILTERS
#define BNEP_MAX_MULTI_FILTERS      5
#endif

/* Minimum MTU size. */
#ifndef BNEP_MIN_MTU_SIZE
#define BNEP_MIN_MTU_SIZE           L2CAP_MTU_SIZE
#endif

/* Preferred MTU size. */
#ifndef BNEP_MTU_SIZE
#define BNEP_MTU_SIZE               BNEP_MIN_MTU_SIZE
#endif

/* Maximum size of user data, in bytes.  */
#ifndef BNEP_MAX_USER_DATA_SIZE
#define BNEP_MAX_USER_DATA_SIZE     1500
#endif

/* Maximum number of buffers allowed in transmit data queue. */
#ifndef BNEP_MAX_XMITQ_DEPTH
#define BNEP_MAX_XMITQ_DEPTH        20
#endif

/* Maximum number BNEP of connections supported. */
#ifndef BNEP_MAX_CONNECTIONS
#define BNEP_MAX_CONNECTIONS        7
#endif


/******************************************************************************
**
** AVDTP
**
******************************************************************************/

#ifndef AVDT_INCLUDED
#define AVDT_INCLUDED               FALSE
#endif

/* Include reporting capability in AVDTP */
#ifndef AVDT_REPORTING
#define AVDT_REPORTING              TRUE
#endif

/* Include multiplexing capability in AVDTP */
#ifndef AVDT_MULTIPLEXING
#define AVDT_MULTIPLEXING           TRUE
#endif

/* Number of simultaneous links to different peer devices. */
#ifndef AVDT_NUM_LINKS
#define AVDT_NUM_LINKS              2
#endif

/* Number of simultaneous stream endpoints. */
#ifndef AVDT_NUM_SEPS
#define AVDT_NUM_SEPS               4
#endif

/* Number of simultaneously received stream endpoints */
#ifndef AVDT_MAX_NUM_SEPS
#define AVDT_MAX_NUM_SEPS          16
#endif

/* Number of transport channels setup per media stream(audio or video) */
#ifndef AVDT_NUM_CHANNELS

#if AVDT_REPORTING == TRUE
/* signaling, media and reporting channels */
#define AVDT_NUM_CHANNELS   3
#else
/* signaling and media channels */
#define AVDT_NUM_CHANNELS   2
#endif

#endif

/* Number of transport channels setup by AVDT for all media streams
 * AVDT_NUM_CHANNELS * Number of simultaneous streams.
 */
#ifndef AVDT_NUM_TC_TBL
#define AVDT_NUM_TC_TBL             6
#endif


/* Maximum size in bytes of the codec capabilities information element. */
#ifndef AVDT_CODEC_SIZE
#define AVDT_CODEC_SIZE             10
#endif

/* Maximum size in bytes of the content protection information element. */
#ifndef AVDT_PROTECT_SIZE
#define AVDT_PROTECT_SIZE           90
#endif

/* Maximum number of GKI buffers in the fragment queue (for video frames).
 * Must be less than the number of buffers in the buffer pool of size AVDT_DATA_POOL_SIZE */
#ifndef AVDT_MAX_FRAG_COUNT
#define AVDT_MAX_FRAG_COUNT         15
#endif

#ifndef AVDT_CONFORMANCE_TESTING_INCLUDED
#define AVDT_CONFORMANCE_TESTING_INCLUDED FALSE
#endif

/******************************************************************************
**
** PAN
**
******************************************************************************/

#ifndef PAN_INCLUDED
#define PAN_INCLUDED                     FALSE
#endif

/* This will enable the PANU role */
#ifndef PAN_SUPPORTS_ROLE_PANU
#define PAN_SUPPORTS_ROLE_PANU              TRUE
#endif

/* This will enable the GN role */
#ifndef PAN_SUPPORTS_ROLE_GN
#define PAN_SUPPORTS_ROLE_GN                TRUE
#endif

/* This will enable the NAP role */
#ifndef PAN_SUPPORTS_ROLE_NAP
#define PAN_SUPPORTS_ROLE_NAP               TRUE
#endif

/* This is just for debugging purposes */
#ifndef PAN_SUPPORTS_DEBUG_DUMP
#define PAN_SUPPORTS_DEBUG_DUMP             TRUE
#endif


/* Maximum number of PAN connections allowed */
#ifndef MAX_PAN_CONNS
#define MAX_PAN_CONNS                    7
#endif

/* Default service name for NAP role */
#ifndef PAN_NAP_DEFAULT_SERVICE_NAME
#define PAN_NAP_DEFAULT_SERVICE_NAME    "Network Access Point Service"
#endif

/* Default service name for GN role */
#ifndef PAN_GN_DEFAULT_SERVICE_NAME
#define PAN_GN_DEFAULT_SERVICE_NAME     "Group Network Service"
#endif

/* Default service name for PANU role */
#ifndef PAN_PANU_DEFAULT_SERVICE_NAME
#define PAN_PANU_DEFAULT_SERVICE_NAME   "PAN User Service"
#endif

/* Default description for NAP role service */
#ifndef PAN_NAP_DEFAULT_DESCRIPTION
#define PAN_NAP_DEFAULT_DESCRIPTION     "Public NAP"
#endif

/* Default description for GN role service */
#ifndef PAN_GN_DEFAULT_DESCRIPTION
#define PAN_GN_DEFAULT_DESCRIPTION      "Widcomm GN"
#endif

/* Default description for PANU role service */
#ifndef PAN_PANU_DEFAULT_DESCRIPTION
#define PAN_PANU_DEFAULT_DESCRIPTION    "Laptop's PANU"
#endif

/* Default Security level for PANU role. */
#ifndef PAN_PANU_SECURITY_LEVEL
#define PAN_PANU_SECURITY_LEVEL          0
#endif

/* Default Security level for GN role. */
#ifndef PAN_GN_SECURITY_LEVEL
#define PAN_GN_SECURITY_LEVEL            0
#endif

/* Default Security level for NAP role. */
#ifndef PAN_NAP_SECURITY_LEVEL
#define PAN_NAP_SECURITY_LEVEL           0
#endif




/******************************************************************************
**
** GAP
**
******************************************************************************/

#ifndef GAP_INCLUDED
#define GAP_INCLUDED                FALSE
#endif

/* This is set to enable use of GAP L2CAP connections. */
#ifndef GAP_CONN_INCLUDED
#define GAP_CONN_INCLUDED           TRUE
#endif

/* This is set to enable posting event for data write */
#ifndef GAP_CONN_POST_EVT_INCLUDED
#define GAP_CONN_POST_EVT_INCLUDED  FALSE
#endif

/* The maximum number of simultaneous GAP L2CAP connections. */
#ifndef GAP_MAX_CONNECTIONS
#define GAP_MAX_CONNECTIONS         8
#endif

/******************************************************************************
**
** SPP
**
******************************************************************************/

#ifndef SPP_INCLUDED
#define SPP_INCLUDED                FALSE
#endif

/* The SPP default MTU. */
#ifndef SPP_DEFAULT_MTU
#define SPP_DEFAULT_MTU             127
#endif

/* The interval, in seconds, that a client tries to reconnect to a service. */
#ifndef SPP_RETRY_CONN_INTERVAL
#define SPP_RETRY_CONN_INTERVAL     1
#endif

/* The SPP discoverable mode: limited or general. */
#ifndef SPP_DISCOVERABLE_MODE
#define SPP_DISCOVERABLE_MODE       BTM_GENERAL_DISCOVERABLE
#endif

/* The maximum number of inquiry results returned in by inquiry procedure. */
#ifndef SPP_DEF_INQ_MAX_RESP
#define SPP_DEF_INQ_MAX_RESP        10
#endif

/* The SPP discovery record size. */
#ifndef SPP_DISC_REC_SIZE
#define SPP_DISC_REC_SIZE           60
#endif

#ifndef SPP_MAX_RECS_PER_DEVICE
#define SPP_MAX_RECS_PER_DEVICE     (SPP_DB_SIZE / SPP_DISC_REC_SIZE)
#endif

/* Inquiry duration in 1.28 second units. */
#ifndef SPP_DEF_INQ_DURATION
#define SPP_DEF_INQ_DURATION        9
#endif

/* keep the raw data received from SDP server in database. */
#ifndef SDP_RAW_DATA_INCLUDED
#define SDP_RAW_DATA_INCLUDED       FALSE
#endif

/* TRUE, to allow JV to create L2CAP connection on SDP PSM. */
#ifndef SDP_FOR_JV_INCLUDED
#define SDP_FOR_JV_INCLUDED         FALSE
#endif

/* Inquiry duration in 1.28 second units. */
#ifndef SDP_DEBUG
#define SDP_DEBUG                   TRUE
#endif

/******************************************************************************
**
** HSP2, HFP
**
******************************************************************************/

#ifndef HSP2_INCLUDED
#define HSP2_INCLUDED               FALSE
#endif

/* Include the ability to perform inquiry for peer devices. */
#ifndef HSP2_INQUIRY_INCLUDED
#define HSP2_INQUIRY_INCLUDED       TRUE
#endif

/* Include Audio Gateway specific code. */
#ifndef HSP2_AG_INCLUDED
#define HSP2_AG_INCLUDED            TRUE
#endif

/* Include Headset Specific Code. */
#ifndef HSP2_HS_INCLUDED
#define HSP2_HS_INCLUDED            TRUE
#endif

/* Include the ability to open an SCO connection for In-Band Ringing. */
#ifndef HSP2_IB_RING_INCLUDED
#define HSP2_IB_RING_INCLUDED       TRUE
#endif

/* Include the ability to repeat a ring. */
#ifndef HSP2_AG_REPEAT_RING
#define HSP2_AG_REPEAT_RING         TRUE
#endif

#ifndef HSP2_APP_CLOSES_ON_CKPD
#define HSP2_APP_CLOSES_ON_CKPD     FALSE
#endif


/* Include the ability to park a connection. */
#ifndef HSP2_PARK_INCLUDED
#define HSP2_PARK_INCLUDED          TRUE
#endif

/* Include HSP State Machine debug trace messages. */
#ifndef HSP2_FSM_DEBUG
#define HSP2_FSM_DEBUG              TRUE
#endif

/* The Module's Inquiry Scan Window. */
#ifndef HSP2_INQ_SCAN_WINDOW
#define HSP2_INQ_SCAN_WINDOW        0
#endif

/* The Module's Inquiry Scan Interval. */
#ifndef HSP2_INQ_SCAN_INTERVAL
#define HSP2_INQ_SCAN_INTERVAL      0
#endif

/* The Module's Page Scan Interval. */
#ifndef HSP2_PAGE_SCAN_INTERVAL
#define HSP2_PAGE_SCAN_INTERVAL     0
#endif

/* The Module's Page Scan Window. */
#ifndef HSP2_PAGE_SCAN_WINDOW
#define HSP2_PAGE_SCAN_WINDOW       0
#endif

/* The Park Mode's Minimum Beacon Period. */
#ifndef HSP2_BEACON_MIN_PERIOD
#define HSP2_BEACON_MIN_PERIOD      450
#endif

/* The Park Mode's Maximum Beacon Period. */
#ifndef HSP2_BEACON_MAX_PERIOD
#define HSP2_BEACON_MAX_PERIOD      500
#endif

/* The duration of the inquiry in seconds. */
#ifndef HSP2_INQ_DURATION
#define HSP2_INQ_DURATION           4
#endif

/* Maximum number of peer responses during an inquiry. */
#ifndef HSP2_INQ_MAX_NUM_RESPS
#define HSP2_INQ_MAX_NUM_RESPS      3
#endif

/* Maximum number of times to retry an inquiry prior to failure. */
#ifndef HSP2_MAX_INQ_RETRY
#define HSP2_MAX_INQ_RETRY          6
#endif

/* Maximum number of times to retry an RFCOMM connection prior to failure. */
#ifndef HSP2_MAX_CONN_RETRY
#define HSP2_MAX_CONN_RETRY         3
#endif

/* If the connect request failed for authentication reasons, do not retry */
#ifndef HSP2_NO_RETRY_ON_AUTH_FAIL
#define HSP2_NO_RETRY_ON_AUTH_FAIL  TRUE
#endif

/* Maximum number of characters in an HSP2 device name. */
#ifndef HSP2_MAX_NAME_LEN
#define HSP2_MAX_NAME_LEN           32
#endif

/* The minimum speaker and/or microphone gain setting. */
#ifndef HSP2_MIN_GAIN
#define HSP2_MIN_GAIN               0
#endif

/* The maximum speaker and/or microphone setting. */
#ifndef HSP2_MAX_GAIN
#define HSP2_MAX_GAIN               15
#endif

/* The default value to send on an AT+CKPD. */
#ifndef HSP2_KEYPRESS_DEFAULT
#define HSP2_KEYPRESS_DEFAULT       200
#endif

/* Maximum amount a data that can be received per RFCOMM frame. */
#ifndef HSP2_MAX_RFC_READ_LEN
#define HSP2_MAX_RFC_READ_LEN       128
#endif

/* The time in seconds to wait for completion of a partial AT command or response from the peer. */
#ifndef HSP2_AT_TO_INTERVAL
#define HSP2_AT_TO_INTERVAL         30
#endif

/* The time to wait before repeating a ring to a peer Headset. */
#ifndef HSP2_REPEAT_RING_TO
#define HSP2_REPEAT_RING_TO         4
#endif

/* Time to wait for a response for an AT command */
#ifndef HSP2_AT_RSP_TO
#define HSP2_AT_RSP_TO              20
#endif

/* SCO packet type(s) to use (bitmask: see spec), 0 - device default (recommended) */
#ifndef HSP2_SCO_PKT_TYPES
#define HSP2_SCO_PKT_TYPES          ((UINT16)0x0000)
#endif

/* The default settings of the SCO voice link. */
#ifndef HSP2_DEFAULT_VOICE_SETTINGS
#define HSP2_DEFAULT_VOICE_SETTINGS (HCI_INP_CODING_LINEAR | HCI_INP_DATA_FMT_2S_COMPLEMENT | HCI_INP_SAMPLE_SIZE_16BIT | HCI_AIR_CODING_FORMAT_CVSD)
#endif

#ifndef HSP2_MAX_AT_CMD_LENGTH
#define HSP2_MAX_AT_CMD_LENGTH       16
#endif

#ifndef HSP2_MAX_AT_VAL_LENGTH
#if (defined(HFP_INCLUDED) && HFP_INCLUDED == TRUE)
#define HSP2_MAX_AT_VAL_LENGTH       310
#else
#define HSP2_MAX_AT_VAL_LENGTH       5
#endif
#endif


#ifndef HSP2_SDP_DB_SIZE
#define HSP2_SDP_DB_SIZE             300
#endif


/******************************************************************************
**
** HFP
**
******************************************************************************/

#ifndef HFP_INCLUDED
#define HFP_INCLUDED                FALSE
#endif

/* Include Audio Gateway specific code. */
#ifndef HFP_AG_INCLUDED
#define HFP_AG_INCLUDED             TRUE
#endif

/* Include Hand Free Specific Code. */
#ifndef HFP_HF_INCLUDED
#define HFP_HF_INCLUDED             TRUE
#endif

/* Use AT interface instead of full blown API */
#ifndef AT_INTERFACE
#define AT_INTERFACE            FALSE
#endif

/* HFP Manages SCO establishement for various procedures */
#ifndef HFP_SCO_MGMT_INCLUDED
#define HFP_SCO_MGMT_INCLUDED             TRUE
#endif

/* CCAP compliant features and behavior desired */
#ifndef CCAP_COMPLIANCE
#define CCAP_COMPLIANCE             TRUE
#endif

/* Caller ID string, part of +CLIP result code */
#ifndef HFP_MAX_CLIP_INFO
#define HFP_MAX_CLIP_INFO             45
#endif

#ifndef HFP_RPT_PEER_INFO_INCLUDED
#define HFP_RPT_PEER_INFO_INCLUDED  TRUE  /* Reporting of peer features enabled */
#endif

/******************************************************************************
**
** HID
**
******************************************************************************/

/* HID Device Role Included */
#ifndef HID_DEV_INCLUDED
#define HID_DEV_INCLUDED             FALSE
#endif

#ifndef HID_DEV_PM_INCLUDED
#define HID_DEV_PM_INCLUDED         TRUE
#endif

/* The HID Device is a virtual cable */
#ifndef HID_DEV_VIRTUAL_CABLE
#define HID_DEV_VIRTUAL_CABLE       TRUE
#endif

/* The HID device initiates the reconnections */
#ifndef HID_DEV_RECONN_INITIATE
#define HID_DEV_RECONN_INITIATE     TRUE
#endif

/* THe HID device is normally connectable */
#ifndef HID_DEV_NORMALLY_CONN
#define HID_DEV_NORMALLY_CONN       FALSE
#endif

/* The device is battery powered */
#ifndef HID_DEV_BATTERY_POW
#define HID_DEV_BATTERY_POW         TRUE
#endif

/* Device is capable of waking up the host */
#ifndef HID_DEV_REMOTE_WAKE
#define HID_DEV_REMOTE_WAKE         TRUE
#endif

/* Device needs host to close SDP channel after SDP is over */
#ifndef HID_DEV_SDP_DISABLE
#define HID_DEV_SDP_DISABLE         TRUE
#endif

#ifndef HID_DEV_MTU_SIZE
#define HID_DEV_MTU_SIZE                 64
#endif

#ifndef HID_DEV_FLUSH_TO
#define HID_DEV_FLUSH_TO                 0xffff
#endif

#ifndef HID_DEV_PAGE_SCAN_WIN
#define HID_DEV_PAGE_SCAN_WIN       (0)
#endif

#ifndef HID_DEV_PAGE_SCAN_INT
#define HID_DEV_PAGE_SCAN_INT       (0)
#endif

#ifndef HID_DEV_MAX_CONN_RETRY
#define HID_DEV_MAX_CONN_RETRY      (15)
#endif

#ifndef HID_DEV_REPAGE_WIN
#define HID_DEV_REPAGE_WIN          (1)
#endif

#ifndef HID_DEV_SVC_NAME
#define HID_DEV_SVC_NAME            "HID"
#endif

#ifndef HID_DEV_SVC_DESCR
#define HID_DEV_SVC_DESCR           "3-button mouse and keyboard"
#endif

#ifndef HID_DEV_PROVIDER_NAME
#define HID_DEV_PROVIDER_NAME       "Widcomm"
#endif

#ifndef HID_DEV_REL_NUM
#define HID_DEV_REL_NUM             0x0100
#endif

#ifndef HID_DEV_PARSER_VER
#define HID_DEV_PARSER_VER          0x0111
#endif

#ifndef HID_DEV_SUBCLASS
#define HID_DEV_SUBCLASS            COD_MINOR_POINTING
#endif

#ifndef HID_DEV_COUNTRY_CODE
#define HID_DEV_COUNTRY_CODE        0x33
#endif

#ifndef HID_DEV_SUP_TOUT
#define HID_DEV_SUP_TOUT            0x8000
#endif

#ifndef HID_DEV_NUM_LANGS
#define HID_DEV_NUM_LANGS           1
#endif

#ifndef HID_DEV_INACT_TIMEOUT
#define HID_DEV_INACT_TIMEOUT       60
#endif

#ifndef HID_DEV_BUSY_MODE_PARAMS
#define HID_DEV_BUSY_MODE_PARAMS    { 320, 160, 10, 20, HCI_MODE_ACTIVE }
#endif

#ifndef HID_DEV_IDLE_MODE_PARAMS
#define HID_DEV_IDLE_MODE_PARAMS    { 320, 160, 10, 20, HCI_MODE_SNIFF }
#endif

#ifndef HID_DEV_SUSP_MODE_PARAMS
#define HID_DEV_SUSP_MODE_PARAMS    { 640, 320,  0,    0, HCI_MODE_PARK }
#endif

#ifndef HID_DEV_MAX_DESCRIPTOR_SIZE
#define HID_DEV_MAX_DESCRIPTOR_SIZE      128     /* Max descriptor size          */
#endif

#ifndef HID_DEV_LANGUAGELIST
#define HID_DEV_LANGUAGELIST             {0x35, 0x06, 0x09, 0x04, 0x09, 0x09, 0x01, 0x00}
#endif

#ifndef HID_DEV_LINK_SUPERVISION_TO
#define HID_DEV_LINK_SUPERVISION_TO      0x8000
#endif

#ifndef HID_CONTROL_POOL_ID
#define HID_CONTROL_POOL_ID             2
#endif

#ifndef HID_INTERRUPT_POOL_ID
#define HID_INTERRUPT_POOL_ID           2
#endif

#ifndef UCD_HID_INCLUDED
#define UCD_HID_INCLUDED    FALSE
#endif

#ifndef BRR_HID_INCLUDED
#define BRR_HID_INCLUDED    FALSE
#endif

#ifndef HID_DEV_SET_CONN_MODE
#define HID_DEV_SET_CONN_MODE    FALSE
#endif

/*************************************************************************
** Definitions for Both HID-Host & Device
*/
#ifndef HID_MAX_SVC_NAME_LEN
#define HID_MAX_SVC_NAME_LEN  32
#endif

#ifndef HID_MAX_SVC_DESCR_LEN
#define HID_MAX_SVC_DESCR_LEN 32
#endif

#ifndef HID_MAX_PROV_NAME_LEN
#define HID_MAX_PROV_NAME_LEN 32
#endif

/*************************************************************************
** Definitions for HID-Host
*/
#ifndef  HID_HOST_INCLUDED
#define HID_HOST_INCLUDED           FALSE
#endif

#ifndef HID_HOST_MAX_DEVICES
#define HID_HOST_MAX_DEVICES        7
#endif

#ifndef HID_HOST_MTU
#define HID_HOST_MTU                640
#endif

#ifndef HID_HOST_FLUSH_TO
#define HID_HOST_FLUSH_TO                 0xffff
#endif

#ifndef HID_HOST_MAX_CONN_RETRY
#define HID_HOST_MAX_CONN_RETRY     (15)
#endif

#ifndef HID_HOST_REPAGE_WIN
#define HID_HOST_REPAGE_WIN          (2)
#endif


/******************************************************************************
**
** DUN and FAX
**
******************************************************************************/

#ifndef DUN_INCLUDED
#define DUN_INCLUDED                FALSE
#endif


/******************************************************************************
**
** GOEP
**
******************************************************************************/

#ifndef GOEP_INCLUDED
#define GOEP_INCLUDED               FALSE
#endif

/* This is set to enable GOEP non-blocking file system access functions. */
#ifndef GOEP_FS_INCLUDED
#define GOEP_FS_INCLUDED        TRUE
#endif

/* GOEP authentication key size. */
#ifndef GOEP_MAX_AUTH_KEY_SIZE
#define GOEP_MAX_AUTH_KEY_SIZE      16
#endif

/* Maximum size of the realm authentication string. */
#ifndef GOEP_MAX_AUTH_REALM_SIZE
#define GOEP_MAX_AUTH_REALM_SIZE    16
#endif

/* Realm Character Set */
#ifndef GOEP_REALM_CHARSET
#define GOEP_REALM_CHARSET          0       /* ASCII */
#endif

/* This is set to the maximum length of path name allowed in the system (_MAX_PATH). */
#ifndef GOEP_MAX_PATH_SIZE
#define GOEP_MAX_PATH_SIZE          255
#endif

/* Specifies whether or not client's user id is required during obex authentication */
#ifndef GOEP_SERVER_USERID_REQUIRED
#define GOEP_SERVER_USERID_REQUIRED FALSE
#endif

/* This is set to the maximum length of file name allowed in the system (_MAX_FNAME). */
#ifndef GOEP_MAX_FILE_SIZE
#define GOEP_MAX_FILE_SIZE          128
#endif

/* Character used as path separator */
#ifndef GOEP_PATH_SEPARATOR
#define GOEP_PATH_SEPARATOR         ((char) 0x5c)   /* 0x2f ('/'), or 0x5c ('\') */
#endif

/******************************************************************************
**
** OPP
**
******************************************************************************/

#ifndef OPP_INCLUDED
#define OPP_INCLUDED                FALSE
#endif

/* This is set to enable OPP client capabilities. */
#ifndef OPP_CLIENT_INCLUDED
#define OPP_CLIENT_INCLUDED         FALSE
#endif

/* This is set to enable OPP server capabilities. */
#ifndef OPP_SERVER_INCLUDED
#define OPP_SERVER_INCLUDED         FALSE
#endif

/* if the optional formating functions are to be included or not */
#ifndef OPP_FORMAT_INCLUDED
#define OPP_FORMAT_INCLUDED         FALSE
#endif

/* Maximum number of client sessions allowed by server */
#ifndef OPP_MAX_SRVR_SESS
#define OPP_MAX_SRVR_SESS           3
#endif

/******************************************************************************
**
** FTP
**
******************************************************************************/

#ifndef FTP_INCLUDED
#define FTP_INCLUDED                FALSE
#endif

/* This is set to enable FTP client capabilities. */
#ifndef FTP_CLIENT_INCLUDED
#define FTP_CLIENT_INCLUDED         TRUE
#endif

/* This is set to enable FTP server capabilities. */
#ifndef FTP_SERVER_INCLUDED
#define FTP_SERVER_INCLUDED         TRUE
#endif

/******************************************************************************
**
** XML Parser
**
******************************************************************************/

#ifndef XML_STACK_SIZE
#define XML_STACK_SIZE             7
#endif

/******************************************************************************
**
** BPP Printer
**
******************************************************************************/
#ifndef BPP_DEBUG
#define BPP_DEBUG            FALSE
#endif

#ifndef BPP_INCLUDED
#define BPP_INCLUDED                FALSE
#endif

#ifndef BPP_SND_INCLUDED
#define BPP_SND_INCLUDED            FALSE
#endif

/* Maximum number of senders allowed to connect simultaneously
** The maximum is 6 or (OBX_NUM_SERVERS / 2), whichever is smaller
*/
#ifndef BPP_PR_MAX_CON
#define BPP_PR_MAX_CON         3
#endif

/* Service Name. maximum length: 248
#ifndef BPP_SERVICE_NAME
#define BPP_SERVICE_NAME            "Basic Printing"
#endif
 */
/* Document Format Supported. ASCII comma-delimited list of MIME type:version string
#ifndef BPP_DOC_FORMAT_SUPPORTED
#define BPP_DOC_FORMAT_SUPPORTED    "application/vnd.pwg-xhtml-print:1.0,application/vnd.hp-PCL:5E,application/PDF"
#endif

#ifndef BPP_DOC_FORMAT_SUPPORTED_LEN
#define BPP_DOC_FORMAT_SUPPORTED_LEN    77
#endif
 */
/* Character repertoires.
#ifndef BPP_CHARACTER_REPERTOIRES
#define BPP_CHARACTER_REPERTOIRES {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01}
#endif
 */
/* XHTML formats.
#ifndef BPP_XHTML_PRINT_FORMATS
#define BPP_XHTML_PRINT_FORMATS     "image/gif:89A,image/jpeg"
#endif

#ifndef BPP_XHTML_PRINT_FORMATS_LEN
#define BPP_XHTML_PRINT_FORMATS_LEN 24
#endif
 */
/* Color supported.
#ifndef BPP_COLOR_SUPORTED
#define BPP_COLOR_SUPORTED          FALSE
#endif
 */
/* 1284 ID string. First 2 bytes are the length.
#ifndef BPP_1284ID
#define BPP_1284ID                  "\x00\x48MANUFACTURER:ACME Manufacturing;COMMAND SET:PCL,MPL;MODEL:LaserBeam \?;"
#endif

#ifndef BPP_1284ID_LEN
#define BPP_1284ID_LEN              72
#endif
 */
/* Printer name.
#ifndef BPP_PRINTER_NAME
#define BPP_PRINTER_NAME            "My Printer"
#endif

#ifndef BPP_PRINTER_NAME_LEN
#define BPP_PRINTER_NAME_LEN        10
#endif
 */

/* Printer location.
#ifndef BPP_PRINTER_LOCATION
#define BPP_PRINTER_LOCATION        "Hotel Lobby"
#endif

#ifndef BPP_PRINTER_LOCATION_LEN
#define BPP_PRINTER_LOCATION_LEN    11
#endif
 */
/* Duplex printing supported.
#ifndef BPP_DUPLEX_SUPPORTED
#define BPP_DUPLEX_SUPPORTED        TRUE
#endif
 */

/* Media types supported.
#ifndef BPP_MEDIA_TYPES_SUPPORTED
#define BPP_MEDIA_TYPES_SUPPORTED   "stationary,continuous-long,photographic-high-gloss,cardstock"
#endif

#ifndef BPP_MEDIA_TYPES_SUPPORTED_LEN
#define BPP_MEDIA_TYPES_SUPPORTED_LEN   60
#endif
 */
/* Maximum media with supported.
#ifndef BPP_MAX_MEDIA_WIDTH
#define BPP_MAX_MEDIA_WIDTH         205
#endif
 */
/* Maximum media length supported.
#ifndef BPP_MAX_MEDIA_LENGTH
#define BPP_MAX_MEDIA_LENGTH        285
#endif
 */
/* the maximum string len for the media size of medium loaded */
#ifndef BPP_MEDIA_SIZE_LEN
#define BPP_MEDIA_SIZE_LEN          33
#endif

/* Debug Trace the SOAP object, if TRUE */
#ifndef BPP_TRACE_XML
#define BPP_TRACE_XML               TRUE
#endif

/* in case that the SOAP object does not all come in one OBEX packet,
 * this size of data may be kept in the BPP control block for continuing parsing.
 * The maximum is the size of the biggest GKI buffer (GKI_MAX_BUF_SIZE) */
#ifndef BPP_SOAP_KEEP_SIZE
#define BPP_SOAP_KEEP_SIZE          200
#endif


/******************************************************************************
**
** BIP
**
******************************************************************************/
#ifndef BIP_INCLUDED
#define BIP_INCLUDED                FALSE
#endif

/* TRUE to include imaging initiator */
#ifndef BIP_INITR_INCLUDED
#define BIP_INITR_INCLUDED          FALSE
#endif

/* TRUE to include imaging responder */
#ifndef BIP_RSPDR_INCLUDED
#define BIP_RSPDR_INCLUDED          FALSE
#endif

/* TRUE to include image push feature */
#ifndef BIP_PUSH_INCLUDED
#define BIP_PUSH_INCLUDED           TRUE
#endif

/* TRUE to include image pull feature */
#ifndef BIP_PULL_INCLUDED
#define BIP_PULL_INCLUDED           TRUE
#endif

/* TRUE to include advanced image printing feature */
#ifndef BIP_PRINTING_INCLUDED
#define BIP_PRINTING_INCLUDED       TRUE
#endif

/* TRUE to include automatic archive feature */
#ifndef BIP_ARCHIVE_INCLUDED
#define BIP_ARCHIVE_INCLUDED        TRUE
#endif

/* TRUE to include remote camera feature */
#ifndef BIP_CAMERA_INCLUDED
#define BIP_CAMERA_INCLUDED         TRUE
#endif

/* TRUE to include remote display feature */
#ifndef BIP_DISPLAY_INCLUDED
#define BIP_DISPLAY_INCLUDED        TRUE
#endif

/* TRUE to include sanity check code for API functions */
#ifndef BIP_SANITY_CHECKS
#define BIP_SANITY_CHECKS           TRUE
#endif

/* TRUE to show the received XML object in trace for conformance tests */
#ifndef BIP_TRACE_XML
#define BIP_TRACE_XML               TRUE
#endif

/* in case that the received XML object is not complete, the XML parser state machine needs
 * to keep a copy of the data from the last '<'
 * This macro specifies the maximun amount of data for this purpose */
#ifndef BIP_XML_CARRY_OVER_LEN
#define BIP_XML_CARRY_OVER_LEN      100
#endif

/* minimum 4, maximum is 255. The value should be set to the maximum size of encoding string + 1. JPEG2000.
 * If vendor specific format is supported, it might be bigger than 9 */
#ifndef BIP_IMG_ENCODE_SIZE
#define BIP_IMG_ENCODE_SIZE         9
#endif

/* MIME type: text/plain */
#ifndef BIP_TYPE_SIZE
#define BIP_TYPE_SIZE               20
#endif

/* example: iso-8895-1 */
#ifndef BIP_CHARSET_SIZE
#define BIP_CHARSET_SIZE            10
#endif

/* friendly name */
#ifndef BIP_FNAME_SIZE
#define BIP_FNAME_SIZE              20
#endif

/* service name */
#ifndef BIP_SNAME_SIZE
#define BIP_SNAME_SIZE              60
#endif

/* temporary storage file name(for file system access, may include path) */
#ifndef BIP_TEMP_NAME_SIZE
#define BIP_TEMP_NAME_SIZE          200
#endif

/* image file name */
#ifndef BIP_IMG_NAME_SIZE
#define BIP_IMG_NAME_SIZE           200
#endif

/* attachment file name */
#ifndef BIP_ATT_NAME_SIZE
#define BIP_ATT_NAME_SIZE           200
#endif

/* object (image, attachment, thumbnail) file name (may be used for file system) */
#ifndef BIP_OBJ_NAME_SIZE
#define BIP_OBJ_NAME_SIZE           200
#endif


/******************************************************************************
**
** PAN
**
******************************************************************************/

#ifndef PAN_INCLUDED
#define PAN_INCLUDED                FALSE
#endif


/******************************************************************************
**
** SAP
**
******************************************************************************/

#ifndef SIM_ACCESS_INCLUDED
#define SIM_ACCESS_INCLUDED         FALSE
#endif

#ifndef SAP_SERVER_INCLUDED
#define SAP_SERVER_INCLUDED         TRUE
#endif

#ifndef SAP_CLIENT_INCLUDED
#define SAP_CLIENT_INCLUDED         TRUE
#endif


/*************************************************************************
 * A2DP Definitions
 */
#ifndef A2D_INCLUDED
#define A2D_INCLUDED            FALSE
#endif

/* TRUE to include SBC utility functions */
#ifndef A2D_SBC_INCLUDED
#define A2D_SBC_INCLUDED        A2D_INCLUDED
#endif

/* TRUE to include MPEG-1,2 (mp3) utility functions */
#ifndef A2D_M12_INCLUDED
#define A2D_M12_INCLUDED        A2D_INCLUDED
#endif

/* TRUE to include MPEG-2,4 (aac) utility functions */
#ifndef A2D_M24_INCLUDED
#define A2D_M24_INCLUDED        A2D_INCLUDED
#endif

/*************************************************************************
 * VDP Definitions
 */
#ifndef VDP_INCLUDED
#define VDP_INCLUDED            FALSE
#endif

/******************************************************************************
**
** AVCTP
**
******************************************************************************/

#ifndef AVCT_INCLUDED
#define AVCT_INCLUDED               FALSE
#endif

/* Number of simultaneous ACL links to different peer devices. */
#ifndef AVCT_NUM_LINKS
#define AVCT_NUM_LINKS              2
#endif

/* Number of simultaneous AVCTP connections. */
#ifndef AVCT_NUM_CONN
#define AVCT_NUM_CONN               3
#endif

/* TRUE to support the browsing channel. */
#ifndef AVCT_BROWSE_INCLUDED
#define AVCT_BROWSE_INCLUDED        TRUE
#endif

/* TRUE to support the AVRCP 1.6 (GetTotalNumOfItems). */
#ifndef AVRC_1_6_INCLUDED
#define AVRC_1_6_INCLUDED           FALSE
#endif

/* Pool ID where to reassemble the SDU.
   This Pool allows buffers to be used that are larger than
   the L2CAP_MAX_MTU. */
#ifndef AVCT_BR_USER_RX_POOL_ID
#define AVCT_BR_USER_RX_POOL_ID     HCI_ACL_POOL_ID
#endif

/* Pool ID where to hold the SDU.
   This Pool allows buffers to be used that are larger than
   the L2CAP_MAX_MTU. */
#ifndef AVCT_BR_USER_TX_POOL_ID
#define AVCT_BR_USER_TX_POOL_ID     HCI_ACL_POOL_ID
#endif

/*
GKI Buffer Pool ID used to hold MPS segments during SDU reassembly
*/
#ifndef AVCT_BR_FCR_RX_POOL_ID
#define AVCT_BR_FCR_RX_POOL_ID      HCI_ACL_POOL_ID
#endif

/*
GKI Buffer Pool ID used to hold MPS segments used in (re)transmissions.
L2CAP_DEFAULT_ERM_POOL_ID is specified to use the HCI ACL data pool.
Note:  This pool needs to have enough buffers to hold two times the window size negotiated
 in the tL2CAP_FCR_OPTIONS (2 * tx_win_size)  to allow for retransmissions.
 The size of each buffer must be able to hold the maximum MPS segment size passed in
 tL2CAP_FCR_OPTIONS plus BT_HDR (8) + HCI preamble (4) + L2CAP_MIN_OFFSET (11 - as of BT 2.1 + EDR Spec).
*/
#ifndef AVCT_BR_FCR_TX_POOL_ID
#define AVCT_BR_FCR_TX_POOL_ID      HCI_ACL_POOL_ID
#endif

/* AVCTP Browsing channel FCR Option:
Size of the transmission window when using enhanced retransmission mode. Not used
in basic and streaming modes. Range: 1 - 63
*/
#ifndef AVCT_BR_FCR_OPT_TX_WINDOW_SIZE
#define AVCT_BR_FCR_OPT_TX_WINDOW_SIZE      10
#endif

/* AVCTP Browsing channel FCR Option:
Number of transmission attempts for a single I-Frame before taking
Down the connection. Used In ERTM mode only. Value is Ignored in basic and
Streaming modes.
Range: 0, 1-0xFF
0 - infinite retransmissions
1 - single transmission
*/
#ifndef AVCT_BR_FCR_OPT_MAX_TX_B4_DISCNT
#define AVCT_BR_FCR_OPT_MAX_TX_B4_DISCNT    20
#endif

/* AVCTP Browsing channel FCR Option: Retransmission Timeout
The AVRCP specification set a value in the range of 300 - 2000 ms
Timeout (in msecs) to detect Lost I-Frames. Only used in Enhanced retransmission mode.
Range: Minimum 2000 (2 secs) when supporting PBF.
 */
#ifndef AVCT_BR_FCR_OPT_RETX_TOUT
#define AVCT_BR_FCR_OPT_RETX_TOUT           2000
#endif

/* AVCTP Browsing channel FCR Option: Monitor Timeout
The AVRCP specification set a value in the range of 300 - 2000 ms
Timeout (in msecs) to detect Lost S-Frames. Only used in Enhanced retransmission mode.
Range: Minimum 12000 (12 secs) when supporting PBF.
*/
#ifndef AVCT_BR_FCR_OPT_MONITOR_TOUT
#define AVCT_BR_FCR_OPT_MONITOR_TOUT        12000
#endif

/******************************************************************************
**
** AVRCP
**
******************************************************************************/

#ifndef AVRC_INCLUDED
#define AVRC_INCLUDED               FALSE
#endif

/* TRUE to support AVRCP 1.3 - Metadata. */
#ifndef AVRC_METADATA_INCLUDED
#define AVRC_METADATA_INCLUDED      TRUE
#endif

/* TRUE to support AVRCP 1.4 - Advanced Control. */
#ifndef AVRC_ADV_CTRL_INCLUDED
#define AVRC_ADV_CTRL_INCLUDED      TRUE
#endif

/******************************************************************************
**
** MCAP
**
******************************************************************************/
#ifndef MCA_INCLUDED
#define MCA_INCLUDED                FALSE
#endif

/* TRUE to support Clock Synchronization OpCodes */
#ifndef MCA_SYNC_INCLUDED
#define MCA_SYNC_INCLUDED           FALSE
#endif

/* The MTU size for the L2CAP configuration on control channel. 48 is the minimal */
#ifndef MCA_CTRL_MTU
#define MCA_CTRL_MTU    60
#endif

/* The maximum number of registered MCAP instances. */
#ifndef MCA_NUM_REGS
#define MCA_NUM_REGS    12
#endif

/* The maximum number of control channels (to difference devices) per registered MCAP instances. */
#ifndef MCA_NUM_LINKS
#define MCA_NUM_LINKS   3
#endif

/* The maximum number of MDEP (including HDP echo) per registered MCAP instances. */
#ifndef MCA_NUM_DEPS
#define MCA_NUM_DEPS    13
#endif

/* The maximum number of MDL link per control channel. */
#ifndef MCA_NUM_MDLS
#define MCA_NUM_MDLS    4
#endif

/* Pool ID where to reassemble the SDU. */
#ifndef MCA_USER_RX_POOL_ID
#define MCA_USER_RX_POOL_ID     HCI_ACL_POOL_ID
#endif

/* Pool ID where to hold the SDU. */
#ifndef MCA_USER_TX_POOL_ID
#define MCA_USER_TX_POOL_ID     HCI_ACL_POOL_ID
#endif

/*
GKI Buffer Pool ID used to hold MPS segments during SDU reassembly
*/
#ifndef MCA_FCR_RX_POOL_ID
#define MCA_FCR_RX_POOL_ID      HCI_ACL_POOL_ID
#endif

/*
GKI Buffer Pool ID used to hold MPS segments used in (re)transmissions.
L2CAP_DEFAULT_ERM_POOL_ID is specified to use the HCI ACL data pool.
Note:  This pool needs to have enough buffers to hold two times the window size negotiated
 in the tL2CAP_FCR_OPTIONS (2 * tx_win_size)  to allow for retransmissions.
 The size of each buffer must be able to hold the maximum MPS segment size passed in
 tL2CAP_FCR_OPTIONS plus BT_HDR (8) + HCI preamble (4) + L2CAP_MIN_OFFSET (11 - as of BT 2.1 + EDR Spec).
*/
#ifndef MCA_FCR_TX_POOL_ID
#define MCA_FCR_TX_POOL_ID      HCI_ACL_POOL_ID
#endif

/* MCAP control channel FCR Option:
Size of the transmission window when using enhanced retransmission mode.
1 is defined by HDP specification for control channel.
*/
#ifndef MCA_FCR_OPT_TX_WINDOW_SIZE
#define MCA_FCR_OPT_TX_WINDOW_SIZE      1
#endif

/* MCAP control channel FCR Option:
Number of transmission attempts for a single I-Frame before taking
Down the connection. Used In ERTM mode only. Value is Ignored in basic and
Streaming modes.
Range: 0, 1-0xFF
0 - infinite retransmissions
1 - single transmission
*/
#ifndef MCA_FCR_OPT_MAX_TX_B4_DISCNT
#define MCA_FCR_OPT_MAX_TX_B4_DISCNT    20
#endif

/* MCAP control channel FCR Option: Retransmission Timeout
The AVRCP specification set a value in the range of 300 - 2000 ms
Timeout (in msecs) to detect Lost I-Frames. Only used in Enhanced retransmission mode.
Range: Minimum 2000 (2 secs) when supporting PBF.
 */
#ifndef MCA_FCR_OPT_RETX_TOUT
#define MCA_FCR_OPT_RETX_TOUT           2000
#endif

/* MCAP control channel FCR Option: Monitor Timeout
The AVRCP specification set a value in the range of 300 - 2000 ms
Timeout (in msecs) to detect Lost S-Frames. Only used in Enhanced retransmission mode.
Range: Minimum 12000 (12 secs) when supporting PBF.
*/
#ifndef MCA_FCR_OPT_MONITOR_TOUT
#define MCA_FCR_OPT_MONITOR_TOUT        12000
#endif

/* MCAP control channel FCR Option: Maximum PDU payload size.
The maximum number of payload octets that the local device can receive in a single PDU.
*/
#ifndef MCA_FCR_OPT_MPS_SIZE
#define MCA_FCR_OPT_MPS_SIZE            1000
#endif

/* Shared transport */
#ifndef NFC_SHARED_TRANSPORT_ENABLED
#define NFC_SHARED_TRANSPORT_ENABLED    FALSE
#endif

/******************************************************************************
**
** SER
**
******************************************************************************/

#ifndef SER_INCLUDED
#define SER_INCLUDED                FALSE
#endif

/* Task which runs the serial application. */
#ifndef SER_TASK
#define SER_TASK                    BTE_APPL_TASK
#endif

/* Mailbox used by serial application. */
#ifndef SER_MBOX
#define SER_MBOX                    TASK_MBOX_1
#endif

/* Mailbox mask. */
#ifndef SER_MBOX_MASK
#define SER_MBOX_MASK               TASK_MBOX_1_EVT_MASK
#endif

/* TX path application event. */
#ifndef SER_TX_PATH_APPL_EVT
#define SER_TX_PATH_APPL_EVT        EVENT_MASK(APPL_EVT_3)
#endif

/* RX path application event. */
#ifndef SER_RX_PATH_APPL_EVT
#define SER_RX_PATH_APPL_EVT        EVENT_MASK(APPL_EVT_4)
#endif

/******************************************************************************
**
** HCI Services
**
******************************************************************************/
/* Event mask reserved for handling HCIS events HCISU_TASK */
#ifndef HCISU_EVT_MASK
#define HCISU_EVT_MASK              EVENT_MASK(APPL_EVT_0)
#endif

/* MBox reserved for handling HCIS events HCISU_TASK */
#ifndef HCISU_MBOX
#define HCISU_MBOX                  TASK_MBOX_2
#endif

/* MBox event mask reserved for handling HCIS events HCISU_TASK */
#ifndef HCISU_MBOX_EVT_MASK
#define HCISU_MBOX_EVT_MASK         TASK_MBOX_2_EVT_MASK
#endif

/* Timer reserved for handling HCIS events HCISU_TASK */
#ifndef HCISU_TIMER
#define HCISU_TIMER                 TIMER_2
#endif

/* Timer event mask reserved for handling HCIS events HCISU_TASK */
#ifndef HCISU_TIMER_EVT_MASK
#define HCISU_TIMER_EVT_MASK        TIMER_2_EVT_MASK
#endif

/******************************************************************************
**
** HCI UART
**
******************************************************************************/
#ifndef BAUDRATE_UPDATE_ENCODED_INCLUDED
#define BAUDRATE_UPDATE_ENCODED_INCLUDED    FALSE
#endif

/******************************************************************************
**
** HCI Services (H5 3 wired uart), H4 plus SLIP enabled
**
******************************************************************************/
#ifndef SLIP_INCLUDED
#define SLIP_INCLUDED                   TRUE
#endif

#ifndef SLIP_STATIS_INCLUDED
#define SLIP_STATIS_INCLUDED            TRUE
#endif

#ifndef SLIP_SW_FLOW_CTRL
#define SLIP_SW_FLOW_CTRL               TRUE
#endif

#ifndef BT_TRACE_SLIP
#define BT_TRACE_SLIP                   FALSE
#endif

#ifndef SLIP_HOST_SLIDING_WINDOW_SIZE
#define SLIP_HOST_SLIDING_WINDOW_SIZE   7
#endif

#ifndef SLIP_MAX_RETRANSMIT
#define SLIP_MAX_RETRANSMIT             10
#endif

/* time (in ms) interval between WAKEUP messages */
#ifndef SLIP_WAKEUP_INTERVAL
#define SLIP_WAKEUP_INTERVAL            10
#endif

/* max trial to send WAKEUP messages up to 255 */
#ifndef SLIP_MAX_WAKEUP_TRIAL
#define SLIP_MAX_WAKEUP_TRIAL           10
#endif

/*
The H5 work around sequence will be:

1. controller sends CONFIG with configuration field. (This is not spec compliance.
   Controller shall not have configuration field. This initiates work around.)
2. host ignore the configuration field.
3. Host sends CONFIG with configuration field.
4. controller reponse CONFIG_RESPONSE with configuration field. This shall be the
   final configuration both FW and STACK use. (spec compliance).
5. Host copied the configuration field over and sent CONFIG_RESPONSE with this
   configuration field (workaround)
*/

#ifndef SLIP_CONFIG_FIELD_WORK_AROUND_INCLUDED
#define SLIP_CONFIG_FIELD_WORK_AROUND_INCLUDED       TRUE
#endif

/******************************************************************************
**
** Sleep Mode (Low Power Mode)
**
******************************************************************************/
#ifndef HCILP_INCLUDED
#define HCILP_INCLUDED                  TRUE
#endif

/* sleep mode

    0: disable
    1: UART with Host wake/BT wake out of band signals
    4: H4IBSS, UART with in band signal without Host/BT wake
    9: H5 with in band signal of SLIP without Host/BT wake
*/
#ifndef HCILP_SLEEP_MODE
#define HCILP_SLEEP_MODE                (1)
#endif

/* Host Stack Idle Threshold in 300ms or 25ms, it depends on controller

  In sleep mode 1, this is the number of firmware loops executed with no activity
    before the Host wake line is deasserted. Activity includes HCI traffic excluding
    certain sleep mode commands and the presence of SCO connections if the
    "Allow Host Sleep During SCO" flag is not set to 1. Each count of this
    parameter is roughly equivalent to 300ms or 25ms.

  Not applicable for sleep mode 4(H4IBSS) and 9(H5)
*/
#ifndef HCILP_IDLE_THRESHOLD
#define HCILP_IDLE_THRESHOLD             (0)
#endif

/* Host Controller Idle Threshold in 300ms or 25ms, it depends on controller

    This is the number of firmware loops executed with no activity before the HC is
    considered idle. Depending on the mode, HC may then attempt to sleep.
    Activity includes HCI traffic excluding certain sleep mode commands and
    the presence of ACL/SCO connections.

  Not applicable for sleep mode 4(H4IBSS)
*/
#ifndef HCILP_HC_IDLE_THRESHOLD
#define HCILP_HC_IDLE_THRESHOLD          (0)
#endif

/* GPIO for BT_WAKE signal */
/* Not applicable for sleep mode 4(H4IBSS) and 9(H5) */
#ifndef HCILP_BT_WAKE_GPIO
#define HCILP_BT_WAKE_GPIO              WICED_BT_PIN_DEVICE_WAKE
#endif

/* GPIO for HOST_WAKE signal */
/* Not applicable for sleep mode 4(H4IBSS) and 9(H5) */
#ifndef HCILP_HOST_WAKE_GPIO
#define HCILP_HOST_WAKE_GPIO            WICED_BT_PIN_HOST_WAKE
#endif

/* BT_WAKE Polarity - 0=Active Low, 1= Active High */
/* Not applicable for sleep mode 4(H4IBSS) and 9(H5) */
#ifndef HCILP_BT_WAKE_POLARITY
#define HCILP_BT_WAKE_POLARITY          0
#endif

/* HOST_WAKE Polarity - 0=Active Low, 1= Active High */
/* Not applicable for sleep mode 4(H4IBSS) and 9(H5) */
#ifndef HCILP_HOST_WAKE_POLARITY
#define HCILP_HOST_WAKE_POLARITY        0
#endif

/* HCILP_ALLOW_HOST_SLEEP_DURING_SCO

    When this flag is set to 0, the host is not allowed to sleep while
    an SCO is active. In sleep mode 1, the device will keep the host
    wake line asserted while an SCO is active.
    When this flag is set to 1, the host can sleep while an SCO is active.
    This flag should only be set to 1 if SCO traffic is directed to the PCM interface.

    Not applicable for sleep mode 4(H4IBSS) and 9(H5)
*/
#ifndef HCILP_ALLOW_HOST_SLEEP_DURING_SCO
#define HCILP_ALLOW_HOST_SLEEP_DURING_SCO       1
#endif

/* HCILP_COMBINE_SLEEP_MODE_AND_LPM

    In Mode 0, always set byte 7 to 0. In sleep mode 1, device always
    requires permission to sleep between scans / periodic inquiries regardless
    of the setting of this byte. In sleep mode 1, if byte is set, device must
    have "permission" to sleep during the low power modes of sniff, hold, and park.
    If byte is not set, device can sleep without permission during these modes.
    Permission to sleep in Mode 1 is obtained if the BT_WAKE signal is not asserted.

    Not applicable for sleep mode 4(H4IBSS) and 9(H5)
*/
#ifndef HCILP_COMBINE_SLEEP_MODE_AND_LPM
#define HCILP_COMBINE_SLEEP_MODE_AND_LPM        1
#endif

/* HCILP_ENABLE_UART_TXD_TRI_STATE

    When set to 0, the device will not tristate its UART TX line before going to sleep.
    When set to 1, the device will tristate its UART TX line before going to sleep.

    Not applicable for sleep mode 4(H4IBSS) and 9(H5)
*/
#ifndef HCILP_ENABLE_UART_TXD_TRI_STATE
#define HCILP_ENABLE_UART_TXD_TRI_STATE        0
#endif

/* HCILP_PULSED_HOST_WAKE

    Not applicable for sleep mode 4(H4IBSS) and 9(H5)
*/
#ifndef HCILP_PULSED_HOST_WAKE
#define HCILP_PULSED_HOST_WAKE        0
#endif

/* HCILP_SLEEP_GUARD_TIME

    Only for sleep mode 4(H4IBSS)
    Time in 12.5ms between starting to monitor controller's CTS and raising its RTS
*/
#ifndef HCILP_SLEEP_GUARD_TIME
#define HCILP_SLEEP_GUARD_TIME                 5
#endif

/* HCILP_WAKEUP_GUARD_TIME

    Only for sleep mode 4(H4IBSS)
    Time in 12.5ms between starting to monitor controller's CTS and lowering its RTS
*/
#ifndef HCILP_WAKEUP_GUARD_TIME
#define HCILP_WAKEUP_GUARD_TIME                 5
#endif

/* HCILP_TXD_CONFIG

    Only for sleep mode 4(H4IBSS)
    0: controller's TXD stays low in sleep mode
    1: controller's TXD stays high in sleep mode (default)
*/
#ifndef HCILP_TXD_CONFIG
#define HCILP_TXD_CONFIG                        1
#endif

/* HCILP_BT_WAKE_IDLE_TIMEOUT

    host's idle time in ms before initiating sleep procedure
*/
#ifndef HCILP_BT_WAKE_IDLE_TIMEOUT
#define HCILP_BT_WAKE_IDLE_TIMEOUT              50
#endif

#ifndef H4IBSS_INCLUDED
#define H4IBSS_INCLUDED                 TRUE
#endif

/* display H4IBSS state and event in text */
#ifndef H4IBSS_DEBUG
#define H4IBSS_DEBUG                    TRUE
#endif

/* time interval before going into sleep after having sent or received SLEEP_REQ_ACK */
/* Valid range is 20 - 50 ms */
#ifndef H4IBSS_SLEEP_GUARD_TIME
#define H4IBSS_SLEEP_GUARD_TIME         (40)
#endif

/* timeout(msec) to wait for response of sleep request */
#ifndef H4IBSS_SLEEP_REQ_RESP_TIME
#define H4IBSS_SLEEP_REQ_RESP_TIME      (50)
#endif

/******************************************************************************
**
** RPC
**
******************************************************************************/

#ifndef RPC_INCLUDED
#define RPC_INCLUDED                FALSE
#endif

/* RPCT task mailbox ID for messages coming from rpcgen code. */
#ifndef RPCT_MBOX
#define RPCT_MBOX                   TASK_MBOX_0
#endif

/* RPCT task event for mailbox. */
#ifndef RPCT_RPC_MBOX_EVT
#define RPCT_RPC_MBOX_EVT           TASK_MBOX_0_EVT_MASK
#endif

/* RPCT task event from driver indicating RX data is ready. */
#ifndef RPCT_RX_READY_EVT
#define RPCT_RX_READY_EVT           APPL_EVT_0
#endif

/* RPCT task event from driver indicating data TX is done. */
#ifndef RPCT_TX_DONE_EVT
#define RPCT_TX_DONE_EVT            APPL_EVT_1
#endif

/* RPCT task event indicating data is in the circular buffer. */
#ifndef RPCT_UCBUF_EVT
#define RPCT_UCBUF_EVT              APPL_EVT_2
#endif

/* Task ID of RPCGEN task. */
#ifndef RPCGEN_TASK
#define RPCGEN_TASK                 BTU_TASK
#endif

/* RPCGEN task event for messages coming from RPCT. */
#ifndef RPCGEN_MSG_EVT
#define RPCGEN_MSG_EVT              TASK_MBOX_1_EVT_MASK
#endif

#ifndef RPCGEN_MSG_MBOX
#define RPCGEN_MSG_MBOX             TASK_MBOX_1
#endif

/* Size of circular buffer used to store diagnostic messages. */
#ifndef RPCT_UCBUF_SIZE
#define RPCT_UCBUF_SIZE             2000
#endif

/******************************************************************************
**
** SAP - Sample HSP applications
**
******************************************************************************/

#ifndef SAP_INCLUDED
#define SAP_INCLUDED                FALSE
#endif

#ifndef HSA_HS_INCLUDED
#define HSA_HS_INCLUDED             FALSE
#endif

#ifndef HSA_AG_INCLUDED
#define HSA_AG_INCLUDED             FALSE
#endif

#ifndef MMI_INCLUDED
#define MMI_INCLUDED                FALSE
#endif

/* MMI task event from driver indicating RX data is ready. */
#ifndef MMI_RX_READY_EVT
#define MMI_RX_READY_EVT           APPL_EVT_0
#endif

/******************************************************************************
**
** APPL - Application Task
**
******************************************************************************/
/* When TRUE indicates that an application task is to be run */
#ifndef APPL_INCLUDED
#define APPL_INCLUDED                FALSE
#endif

/* When TRUE remote terminal code included (RPC MUST be included) */
#ifndef RSI_INCLUDED
#define RSI_INCLUDED                FALSE
#endif



#define L2CAP_FEATURE_REQ_ID      73
#define L2CAP_FEATURE_RSP_ID     173


#define L2CAP_ENHANCED_FEATURES   0



/* Use gki_delay for patch ram */
#ifndef BRCM_USE_DELAY
#if ( SLIP_INCLUDED == TRUE )
/* H5 need to be initialized after sending download mini driver HCI command */
#define BRCM_USE_DELAY           FALSE
#else
#define BRCM_USE_DELAY           TRUE
#endif
#endif

/******************************************************************************
**
** BTA
**
******************************************************************************/
/* BTA EIR canned UUID list (default is dynamic) */
#ifndef BTA_EIR_CANNED_UUID_LIST
#define BTA_EIR_CANNED_UUID_LIST FALSE
#endif

/* Number of supported customer UUID in EIR */
#ifndef BTA_EIR_SERVER_NUM_CUSTOM_UUID
#define BTA_EIR_SERVER_NUM_CUSTOM_UUID     8
#endif

/* Place EIR Additional data to the first in EIR (default is the end) */
#ifndef BTA_EIR_ADDITIONAL_DATA_FIRST
#define BTA_EIR_ADDITIONAL_DATA_FIRST    FALSE
#endif

/* MIP A2DP Feature enabled */
#ifndef BTA_MIP_INCLUDED
#define BTA_MIP_INCLUDED   FALSE
#endif

/* No 3D sync profile debug by default */
#ifndef BTA_3DS_DEBUG
#define BTA_3DS_DEBUG FALSE
#endif

/* Avoid sending AT+CKPD=200 */
#ifndef BTA_HS_CKPD_ON_CONNECT
#define BTA_HS_CKPD_ON_CONNECT FALSE
#endif

/* BTA AV CO SCMS-T content protection */
/* TRUE to use SCMS-T content protection */
#ifndef BTA_AV_CO_CP_SCMS_T
#define BTA_AV_CO_CP_SCMS_T FALSE
#endif

/* BTA AVK CO SCMS-T content protection */
/* TRUE to use SCMS-T content protection */
#ifndef BTA_AVK_CO_CP_SCMS_T
#define BTA_AVK_CO_CP_SCMS_T FALSE
#endif

/* BTA Generic PIM Profile Server */
#ifndef BTA_GSE_INCLUDED
#define BTA_GSE_INCLUDED FALSE
#endif

/* BTA Generic PIM Profile Client */
#ifndef BTA_GCE_INCLUDED
#define BTA_GCE_INCLUDED FALSE
#endif

/* BTA SIM Access Client */
#ifndef BTA_SAC_INCLUDED
#define BTA_SAC_INCLUDED FALSE
#endif

/* BTA AG Test Interface (SVT) */
#ifndef BTA_AG_TEST
#define BTA_AG_TEST TRUE
#endif


/******************************************************************************
**
** BTE
**
******************************************************************************/
#ifndef BTE_PLATFORM_IDLE
#define BTE_PLATFORM_IDLE
#endif

#ifndef BTE_IDLE_TASK_INCLUDED
#define BTE_IDLE_TASK_INCLUDED TRUE
#endif

#ifndef BTE_PLATFORM_INITHW
#define BTE_PLATFORM_INITHW
#endif

#ifndef BTE_BTA_CODE_INCLUDED
#define BTE_BTA_CODE_INCLUDED FALSE
#endif


/******************************************************************************
**
** BTTRC
**
******************************************************************************/
/* Whether to parse and display traces-> Platform specific implementation */
#ifndef BTTRC_DISP
#define BTTRC_DISP        BTTRC_DispOnInsight
#endif

/******************************************************************************
**
** Tracing:  Include trace header file here.
**
******************************************************************************/

#include "bt_trace.h"
#endif /* BT_TARGET_H */
