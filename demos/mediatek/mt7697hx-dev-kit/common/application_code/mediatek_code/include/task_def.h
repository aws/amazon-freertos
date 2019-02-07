/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __TASK_DEF_H__
#define __TASK_DEF_H__

#include "FreeRTOSConfig.h"

typedef enum {
    TASK_PRIORITY_IDLE = 0,                                 /* lowest, special for idle task */
    TASK_PRIORITY_SYSLOG,                                   /* special for syslog task */

    /* User task priority begin, please define your task priority at this interval */
    TASK_PRIORITY_LOW,                                      /* low */
    TASK_PRIORITY_BELOW_NORMAL,                             /* below normal */
    TASK_PRIORITY_NORMAL,                                   /* normal */
    TASK_PRIORITY_ABOVE_NORMAL,                             /* above normal */
    TASK_PRIORITY_HIGH,                                     /* high */
    TASK_PRIORITY_SOFT_REALTIME,                            /* soft real time */
    TASK_PRIORITY_HARD_REALTIME,                            /* hard real time */
    /* User task priority end */

    /*Be careful, the max-priority number can not be bigger than configMAX_PRIORITIES - 1, or kernel will crash!!! */
    TASK_PRIORITY_TIMER = configMAX_PRIORITIES - 1,         /* highest, special for timer task to keep time accuracy */
} task_priority_type_t;

/* part_1: SDK tasks configure infomation, please don't modify */

/*The following is an example to define the XXXX task.
#define XXXX_TASK_NAME "XXXX"
#define XXXX_TASK_STACKSIZE 512
#define XXXX_TASK_PRIO TASK_PRIORITY_LOW
#define XXXX_QUEUE_LENGTH  16
*/

/* for wifi net task */
#define UNIFY_NET_TASK_NAME                 "net"
#define UNIFY_NET_TASK_STACKSIZE            (1024*4) /*unit byte!*/
#define UNIFY_NET_TASK_PRIO                 TASK_PRIORITY_NORMAL
#define UNIFY_NET_QUEUE_LENGTH              16

/* for wifi inband task */
#define UNIFY_INBAND_TASK_NAME              "inband"
#define UNIFY_INBAND_TASK_STACKSIZE         (1024*4) /*unit byte!*/
#define UNIFY_INBAND_TASK_PRIO              TASK_PRIORITY_HIGH
#define UNIFY_INBAND_QUEUE_LENGTH           16

/* for wifi supplicant task */
#define UNIFY_WPA_SUPPLICANT_TASK_NAME        "wpa_supplicant"
#define UNIFY_WPA_SUPPLICANT_TASK_STACKSIZE   (2048*4) /*unit byte!*/
#define UNIFY_WPA_SUPPLICANT_TASK_PRIO        TASK_PRIORITY_ABOVE_NORMAL

/* for smart connection task */
#define UNIFY_SMTCN_TASK_NAME              "smtcn"
#define UNIFY_SMTCN_TASK_STACKSIZE         (1024*4) /*unit byte!*/
#define UNIFY_SMTCN_TASK_PRIO              TASK_PRIORITY_NORMAL

/* for lwIP task */

/* defined in lwipopts_freertos.h */
/*
#define TCPIP_THREAD_NAME              "lwIP"
#define TCPIP_THREAD_STACKSIZE         (512 * 4)
#define TCPIP_THREAD_PRIO              TASK_PRIORITY_HIGH
*/

/* for iperf task */
#define IPERF_TASK_NAME                "iperf"
#define IPERF_TASK_STACKSIZE           (1200 * 4)
#define IPERF_TASK_PRIO                TASK_PRIORITY_NORMAL

/* for ping task */
#define PING_TASK_NAME                 "ping"
#define PING_TASK_STACKSIZE            (512 * 4)
#define PING_TASK_PRIO                 TASK_PRIORITY_NORMAL

/* syslog task definition */
#define SYSLOG_TASK_NAME "SYSLOG"
/*This definition determines whether the port service feature is enabled. If it is not defined, then the port service feature is not supported.*/
#if defined(MTK_PORT_SERVICE_ENABLE)
#define SYSLOG_TASK_STACKSIZE 1024
#else
#define SYSLOG_TASK_STACKSIZE 192
#endif
#define SYSLOG_TASK_PRIO TASK_PRIORITY_SYSLOG
/*MTK_NO_PSRAM_ENABLE: the definition of MTK_NO_PSRAM_ENABLE determines whether the demo uses PSRAM. If it is defined, means no PSRAM in this project.*/
#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697) || defined(MTK_NO_PSRAM_ENABLE)
#define SYSLOG_QUEUE_LENGTH 8
#elif (PRODUCT_VERSION == 2523)
#define SYSLOG_QUEUE_LENGTH 512
#endif

/*This option is to enable homekit under middleware/MTK/homekit folder. if this option is defined, the sources and header files under middleware/MTK/homekit/inc be included by middleware/MTK/homekit/MakeFile.*/
#ifdef MTK_HOMEKIT_ENABLE
/* for homekit process after net work status changed */
#define HK_NET_CHANGE_TASK_NAME         "net_change_task"
#define HK_NET_CHANGE_TASK_STACKSIZE     (2 * 1024)
#define HK_NET_CHANGE_TASK_PRIO          TASK_PRIORITY_HIGH

/* start mDNS in homekit init */
#define HK_MDNS_TASK_NAME                   "mdnsd"
#define HK_MDNS_FOR_WAC_TASK_STACKSIZE      (12 * 1024)
#define HK_MDNS_FOR_HAP_TASK_STACKSIZE      (10 * 1024)
/* When switch WIFI mode from AP mode to STA in WAC, need restart mDNS*/
#define HK_MDNS_SWITCH_WIFI_TASK_STACKSIZE  (15 * 1024)
#define HK_MDNS_TASK_PRIO                   TASK_PRIORITY_HIGH

/* for WAC start task */
#define HK_WAC_START_TASK_NAME          "wac1"
#define HK_WAC_START_TASK_STACKSIZE     (1 * 1024)
#define HK_WAC_START_TASK_PRIO          TASK_PRIORITY_NORMAL

/* for WAC engine task */
#define HK_WAC_ENGINE_TASK_NAME         "wac"
#define HK_WAC_ENGINE_TASK_STACKSIZE    (6 * 1024)
#define HK_WAC_ENGINE_TASK_PRIO         TASK_PRIORITY_HIGH

/* for WAC http listener task */
#define HK_WAC_HTTP_TASK_NAME           "httpw"
#define HK_WAC_HTTP_TASK_STACKSIZE      (1024)
#define HK_WAC_HTTP_TASK_PRIO           TASK_PRIORITY_HIGH

/* for hap running task */
#define HK_HAP_TASK_NAME                "hap"
#define HK_HAP_TASK_STACKSIZE           (5 * 1024)
#define HK_HAP_TASK_PRIO                TASK_PRIORITY_NORMAL

/* for hap running task */
#define HK_INIT_TASK_NAME               "hmkit"
#define HK_INIT_TASK_STACKSIZE          (2 * 1024)
#define HK_INIT_TASK_PRIO               TASK_PRIORITY_NORMAL

/* for create http listener in hap */
#define HK_HAP_HTTP_TASK_NAME           "httphapmultiListener"
#define HK_HAP_HTTP_TASK_STACKSIZE      (5 * 1024)
#define HK_HAP_HTTP_TASK_PRIO           TASK_PRIORITY_HIGH
#endif

/* for dhcpd task */
#define DHCPD_TASK_NAME                 "dhcpd"
#define DHCPD_TASK_STACKSIZE            (1024)
#define DHCPD_TASK_PRIO                 TASK_PRIORITY_NORMAL

/* for os utilization task */
/*MTK_OS_CPU_UTILIZATION_ENABLE: This definition determines whether enable CPU utilization profiling feature, if enabled, can use AT command on MT25x3 or CLI command on MT76x7 to do CPU utilization profiling.*/
#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)
#define MTK_OS_CPU_UTILIZATION_TASK_NAME "CPU"
#define MTK_OS_CPU_UTILIZATION_STACKSIZE 512
#define MTK_OS_CPU_UTILIZATION_PRIO      TASK_PRIORITY_SOFT_REALTIME
#endif

/* for bluetooth task */
#define BLUETOOTH_TASK_NAME                 "bt_task"
#define BLUETOOTH_TASK_STACKSIZE            (1024 * 4)
#define BLUETOOTH_TASK_PRIO                 TASK_PRIORITY_HIGH



/* part_2: Application and customer tasks configure information */
/* currently, only UI task and tasks to show example project function which define in apps/project/src/main.c */

/* for create cli */
/*This option is to enable and disable CLI (command line interface) engine.*/
#if defined(MTK_MINICLI_ENABLE)
#define MINICLI_TASK_NAME               "cli"
#if defined(MTK_HTTPCLIENT_SSL_ENABLE)
#define MINICLI_TASK_STACKSIZE          (8192)
#else
#define MINICLI_TASK_STACKSIZE          (4096)
#endif
#define MINICLI_TASK_PRIO               TASK_PRIORITY_SOFT_REALTIME
#endif

/* for tcp test task */
#define TCP_TASK_NAME                   "tcp_server"
#define TCP_TASK_STACKSIZE              (512 * 4)
#define TCP_TASK_PRIO                   TASK_PRIORITY_NORMAL

/*This definition determines whether enable BLE throughput measurement or not. If it is defined, then BLE throughput can be measured from system log.*/
#ifdef BLE_THROUGHPUT
#define BLE_TP_TASK_NAME              "ble_tp_task"
#define BLE_TP_TASK_STACKSIZE         (1024*4) /*unit byte*/
#define BLE_TP_TASK_PRIORITY          TASK_PRIORITY_NORMAL
#endif

/* for set n9log cli task */
#define N9LOG_TASK_NAME                 "n9log"
#define N9LOG_TASK_STACKSIZE            (512)
#define MAX_N9LOG_SIZE                  (1024)
#define N9LOG_TASK_PRIO                 TASK_PRIORITY_LOW

/* dump n9 task definition */
#define DUMP_TASK_NAME             "dump_trigger_task"
#define DUMP_TASK_STACKSIZE        (512*4)
#define DUMP_TASK_PRIORITY         TASK_PRIORITY_NORMAL

#endif

