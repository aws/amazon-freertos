/****************************************************************************
**
**  Name:       gki_target.h
**
**  Function    this file contains platform-specific GKI defintions
**              to be included by bt_target.h, nfc_target.h, and other
**              target header files.
**
**  NOTE        This file should always be included first.
**
**
**  Copyright (c) 1999-2014, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef GKI_TARGET_H
#define GKI_TARGET_H

#include "buildcfg.h"

#include "data_types.h"

/* Operating System Selection */
#ifndef BTE_SIM_APP
#define _GKI_ARM
#define _GKI_STANDALONE
#else
#define _BT_WIN32
#endif

/* define prefix for exporting APIs from libraries */
#define EXPORT_API

#ifndef BTE_BSE_WRAPPER
#ifdef  BTE_SIM_APP
#undef  EXPORT_API
#define EXPORT_API  __declspec(dllexport)
#endif
#endif

#define GKI_API EXPORT_API
#define UDRV_API EXPORT_API


/******************************************************************************
**
** Task configuration
**
******************************************************************************/

/* Definitions of task IDs for inter-task messaging */
#ifndef MMI_TASK
#define MMI_TASK                0
#endif

#ifndef HCISU_TASK
#define HCISU_TASK              1
#endif

#ifndef NFC_HAL_TASK
#define NFC_HAL_TASK            2
#endif

#ifndef NFC_TASK
#define NFC_TASK                3
#endif

#ifndef BTU_TASK
#define BTU_TASK                4
#endif

/* BTE Application, Sample Apps, or Serial port Demo based on JP3 and JP4 setting) */
#ifndef BTE_APPL_TASK
#define BTE_APPL_TASK           5
#endif

#ifndef DEV_MGR_TASK
#define DEV_MGR_TASK            6
#endif

#ifndef ISE_SCR_TASK
#define ISE_SCR_TASK            7
#endif

#ifndef UCODEC_TASK
#define UCODEC_TASK             8
#endif

#ifndef RPCT_TASK
#define RPCT_TASK               9
#endif

#ifndef UNV_TASK
#define UNV_TASK                10
#endif

#ifndef BTE_IDLE_TASK
#define BTE_IDLE_TASK           11
#endif

#ifndef UIPC_TASK
#define UIPC_TASK               12
#endif

#ifndef HCISU_AMP_TASK
#define HCISU_AMP_TASK          13
#endif


/* The number of GKI tasks in the software system. */
#ifndef GKI_MAX_TASKS
#define GKI_MAX_TASKS               14
#endif

/******************************************************************************
**
** Timer configuration
**
******************************************************************************/

/* The number of GKI timers in the software system. */
#ifndef GKI_NUM_TIMERS
#define GKI_NUM_TIMERS              3
#endif

/* A conversion value for translating ticks to calculate GKI timer.  */
#ifndef TICKS_PER_SEC
#define TICKS_PER_SEC               100
#endif

/* The number of ticks to increase on BT_TASK_EVT_GKI_TIMER_UPDATE  */
#ifndef GKI_TICKS_PER_TIMER_UPDATE_EVT
#define GKI_TICKS_PER_TIMER_UPDATE_EVT  100
#endif

/************************************************************************
**  Utility macros converting ticks to time with user define OS ticks per sec
**/
#ifndef GKI_MS_TO_TICKS
#define GKI_MS_TO_TICKS(x)   ((x) / (1000 / TICKS_PER_SEC))
#endif

#ifndef GKI_SECS_TO_TICKS
#define GKI_SECS_TO_TICKS(x)   ((x) * (TICKS_PER_SEC))
#endif

#ifndef GKI_TICKS_TO_MS
#define GKI_TICKS_TO_MS(x)   ((x) * 1000 / TICKS_PER_SEC)
#endif

#ifndef GKI_TICKS_TO_SECS
#define GKI_TICKS_TO_SECS(x)   ((x) / TICKS_PER_SEC)
#endif



/* TICK per second from OS (OS dependent change this macro accordingly to various OS) */
#ifndef OS_TICKS_PER_SEC
#define OS_TICKS_PER_SEC               1000
#endif

/************************************************************************
**  Utility macros converting ticks to time with user define OS ticks per sec
**/

#ifndef GKI_OS_TICKS_TO_MS
#define GKI_OS_TICKS_TO_MS(x)   ((x) * 1000 / OS_TICKS_PER_SEC)
#endif


#ifndef GKI_OS_TICKS_TO_SECS
#define GKI_OS_TICKS_TO_SECS(x)   ((x) / OS_TICKS_PER_SEC))
#endif


/* delay in ticks before stopping system tick. */
#ifndef GKI_DELAY_STOP_SYS_TICK
#define GKI_DELAY_STOP_SYS_TICK     10
#endif

/* Option to guarantee no preemption during timer expiration (most system don't need this) */
#ifndef GKI_TIMER_LIST_NOPREEMPT
#define GKI_TIMER_LIST_NOPREEMPT    FALSE
#endif

/******************************************************************************
**
** Buffer configuration
**
******************************************************************************/

/* TRUE if GKI uses dynamic buffers. */
#ifndef GKI_USE_DYNAMIC_BUFFERS
#define GKI_USE_DYNAMIC_BUFFERS     FALSE
#endif

/* The size of the buffers in pool 0. */
#ifndef GKI_BUF0_SIZE
#define GKI_BUF0_SIZE               64
#endif

/* The number of buffers in buffer pool 0. */
#ifndef GKI_BUF0_MAX
#define GKI_BUF0_MAX                48
#endif

/* The size of the buffers in pool 1. */
#ifndef GKI_BUF1_SIZE
#define GKI_BUF1_SIZE               128
#endif

/* The number of buffers in buffer pool 1. */
#ifndef GKI_BUF1_MAX
#define GKI_BUF1_MAX                26
#endif

/* The size of the buffers in pool 2. */
#ifndef GKI_BUF2_SIZE
#define GKI_BUF2_SIZE               660
#endif

/* The number of buffers in buffer pool 2. */
#ifndef GKI_BUF2_MAX
#define GKI_BUF2_MAX                45
#endif

/* The size of the buffers in pool 3. */
#ifndef GKI_BUF3_SIZE
#define GKI_BUF3_SIZE               2200
#endif

/* The number of buffers in buffer pool 3. */
#ifndef GKI_BUF3_MAX
#define GKI_BUF3_MAX                30
#endif

/* The size of the largest PUBLIC fixed buffer in system. */
#ifndef GKI_MAX_BUF_SIZE
#define GKI_MAX_BUF_SIZE            GKI_BUF3_SIZE
#endif

/* The pool ID of the largest PUBLIC fixed buffer in system. */
#ifndef GKI_MAX_BUF_SIZE_POOL_ID
#define GKI_MAX_BUF_SIZE_POOL_ID    GKI_POOL_ID_3
#endif

/* buffer size for USERIAL, it must large enough to hold BT_HDR and max packet size */
#ifndef USERIAL_POOL_BUF_SIZE
#define USERIAL_POOL_BUF_SIZE       GKI_BUF2_SIZE
#endif

/* buffer pool ID for USERIAL */
#ifndef USERIAL_POOL_ID
#define USERIAL_POOL_ID             GKI_POOL_ID_2
#endif

/* RESERVED buffer pool for OBX */
/* Ideally there should be 1 buffer for each instance for RX data, and some number
of TX buffers based on active instances. OBX will only use these if packet size
requires it. In most cases the large packets are used in only one direction so
the other direction will use smaller buffers.
Devices with small amount of RAM should limit the number of active obex objects.
*/
/* The size of the buffers in pool 4. */
#ifndef GKI_BUF4_SIZE
#define GKI_BUF4_SIZE               0x2000
#endif

/* The number of buffers in buffer pool 4. */
#ifndef GKI_BUF4_MAX
#define GKI_BUF4_MAX                (OBX_NUM_SERVERS + OBX_NUM_CLIENTS)
#endif

/* The number of fixed GKI buffer pools.
eL2CAP requires Pool ID 5
If BTM_SCO_HCI_INCLUDED is FALSE, Pool ID 6 is unnecessary, otherwise set to 7
If BTA_HL_INCLUDED is FALSE then Pool ID 7 is uncessary and set the following to 7, otherwise set to 8
If BLE_INCLUDED is FALSE then Pool ID 8 is uncessary and set the following to 8, otherwise set to 9
*/
#ifndef GKI_NUM_FIXED_BUF_POOLS
#define GKI_NUM_FIXED_BUF_POOLS     9
#endif

/* The buffer pool usage mask. */
#ifndef GKI_DEF_BUFPOOL_PERM_MASK
#define GKI_DEF_BUFPOOL_PERM_MASK   0xfff0
#endif

/* The number of fixed and dynamic buffer pools */
#ifndef GKI_NUM_TOTAL_BUF_POOLS
#define GKI_NUM_TOTAL_BUF_POOLS     10
#endif

/* The following is intended to be a reserved pool for L2CAP
Flow control and retransmissions and intentionally kept out
of order */

/* The number of buffers in buffer pool 5. */
#ifndef GKI_BUF5_MAX
#define GKI_BUF5_MAX                64
#endif

/* The size of the buffers in pool 5
** Special pool used by l2cap retransmissions only.  This size based on segment
** that will fit into both DH5 and 2-DH3 packet types after accounting for GKI
** header.  13 bytes of max headers allows us a 339 payload max. (in btui_app.txt)
** Note: 748 used for insight scriptwrapper with CAT-2 scripts.
*/
#ifndef GKI_BUF5_SIZE
#define GKI_BUF5_SIZE               748
#endif

/* The buffer corruption check flag. */
#ifndef GKI_ENABLE_BUF_CORRUPTION_CHECK
#define GKI_ENABLE_BUF_CORRUPTION_CHECK TRUE
#endif

/* The GKI severe error macro. */
#ifndef GKI_SEVERE
#define GKI_SEVERE(code)
#endif

/* TRUE if GKI includes debug functionality. */
#ifndef GKI_DEBUG
#define GKI_DEBUG                   FALSE
#endif

/* Maximum number of exceptions logged. */
#ifndef GKI_MAX_EXCEPTION
#define GKI_MAX_EXCEPTION           8
#endif

/* Maximum number of chars stored for each exception message. */
#ifndef GKI_MAX_EXCEPTION_MSGLEN
#define GKI_MAX_EXCEPTION_MSGLEN    64
#endif

#ifndef GKI_SEND_MSG_FROM_ISR
#define GKI_SEND_MSG_FROM_ISR    FALSE
#endif


/* The following is intended to be a reserved pool for SCO
over HCI data and intentionally kept out of order */

/* The size of the buffers in pool 6,
  BUF_SIZE = max SCO data 255 + sizeof(BT_HDR) = 8 + SCO packet header 3 + padding 2 = 268 */
#ifndef GKI_BUF6_SIZE
#define GKI_BUF6_SIZE               268
#endif

/* The number of buffers in buffer pool 6. */
#ifndef GKI_BUF6_MAX
#define GKI_BUF6_MAX                75
#endif


/* The following pool is a dedicated pool for HDP
   If a shared pool is more desirable then
   1. set BTA_HL_LRG_DATA_POOL_ID to the desired Gki Pool ID
   2. make sure that the shared pool size is larger than 9472
   3. adjust GKI_NUM_FIXED_BUF_POOLS accordingly since
      POOL ID 7 is not needed
*/

/* The size of the buffers in pool 7 */
#ifndef GKI_BUF7_SIZE
#define GKI_BUF7_SIZE               9472
#endif

/* The number of buffers in buffer pool 7. */
#ifndef GKI_BUF7_MAX
#define GKI_BUF7_MAX                2
#endif

/* The following pool is a dedicated pool for GATT
   If a shared pool is more desirable then
   1. set GATT_DB_POOL_ID to the desired Gki Pool ID
   2. make sure that the shared pool size fit a common GATT database needs
   3. adjust GKI_NUM_FIXED_BUF_POOLS accordingly since
      POOL ID 8 is not needed
*/

/* The size of the buffers in pool 8 */
#ifndef GKI_BUF8_SIZE
#define GKI_BUF8_SIZE               128
#endif

/* The number of buffers in buffer pool 8. */
#ifndef GKI_BUF8_MAX
#define GKI_BUF8_MAX                30
#endif


/* GKI Trace Macros */
#define GKI_TRACE_0(m)                             LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_GENERIC,m)
#define GKI_TRACE_1(m,p1)                          LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_GENERIC,m,p1)
#define GKI_TRACE_2(m,p1,p2)                       LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_GENERIC,m,p1,p2)
#define GKI_TRACE_3(m,p1,p2,p3)                    LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_GENERIC,m,p1,p2,p3)
#define GKI_TRACE_4(m,p1,p2,p3,p4)                 LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_GENERIC,m,p1,p2,p3,p4)
#define GKI_TRACE_5(m,p1,p2,p3,p4,p5)              LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_GENERIC,m,p1,p2,p3,p4,p5)
#define GKI_TRACE_6(m,p1,p2,p3,p4,p5,p6)           LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_GENERIC,m,p1,p2,p3,p4,p5,p6)
#define GKI_TRACE_7(m,p1,p2,p3,p4,p5,p6,p7)        LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_GENERIC,m,p1,p2,p3,p4,p5,p6,p7)

#define GKI_TRACE_ERROR_0(m)                       LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_ERROR,m)
#define GKI_TRACE_ERROR_1(m,p1)                    LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_ERROR,m,p1)
#define GKI_TRACE_ERROR_2(m,p1,p2)                 LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_ERROR,m,p1,p2)
#define GKI_TRACE_ERROR_3(m,p1,p2,p3)              LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_ERROR,m,p1,p2,p3)
#define GKI_TRACE_ERROR_4(m,p1,p2,p3,p4)           LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_ERROR,m,p1,p2,p3,p4)
#define GKI_TRACE_ERROR_5(m,p1,p2,p3,p4,p5)        LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_ERROR,m,p1,p2,p3,p4,p5)
#define GKI_TRACE_ERROR_6(m,p1,p2,p3,p4,p5,p6)     LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_ERROR,m,p1,p2,p3,p4,p5,p6)
#define GKI_TRACE_ERROR_7(m,p1,p2,p3,p4,p5,p6,p7)  LogMsg(TRACE_CTRL_GENERAL | TRACE_LAYER_GKI | TRACE_ORG_GKI | TRACE_TYPE_ERROR,m,p1,p2,p3,p4,p5,p6,p7)

#ifdef __cplusplus
extern "C"
{
#endif

EXPORT_API extern void LogMsg (UINT32 trace_set_mask, const char *fmt_str, ...);

#ifdef __cplusplus
}
#endif

#endif  /* GKI_TARGET_H */
