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

#ifndef _MET_TYPES_H
#define _MET_TYPES_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#ifndef MET_MAX_EVENTS
    #define MET_MAX_EVENTS 2048
#endif
#ifndef MET_MAX_TAKS_OBJECTS
    #define MET_MAX_TAKS_OBJECTS 16
#endif
#ifndef MET_TAG_BUFFER_SIZE
    #define MET_TAG_BUFFER_SIZE 128
#endif

#define OBJECT_NTYPES       4
#define OBJTYPE_TASK        0
#define OBJTYPE_TAG         1


//#define MET_DEBUG
#define MET_ERROR
#include "syslog.h"
#ifdef MET_DEBUG
#define MET_STR "[MET]:"
#define MET_DBGF(args...)    LOG_I(met, MET_STR args);
#else
#define MET_DBGF(args...)
#endif

#ifdef MET_ERROR
#define MET_ERR_STR "[MET ERR]:"
#define MET_ERRF(args...)    LOG_E(met, MET_ERR_STR args);
#else
#define MET_ERRF(args...)
#endif


#define MET_MAGIC           (0x2054454DUL)


//Definition of MET Mode
//bit24~31: OBJECT_NTYPES
//bit16~24: bit16: Task trace enabled
//          bit17: Queue trace enabled
#define MODE_TASK           (1 << 16)
#define MODE_TAG            (1 << 17)
#define MODE_QUEUE          (1 << 18)
//bit0~2: recording mode
#define MODE_OVERWRITE      0
#define MODE_STOP_WHEN_FULL 1
#define MODE_STREAMING      3


//Definition of MET Event Type
#define EV_REC_START        0x81     //start recording
#define EV_REC_STOP         0x82     //stop recording
#define EV_DTS              0x83    //Just DTS
#define EV_DTS_H            0x84    //High part of DTS to the next event
#define EV_TASK_SWTCH_1     0x01     //Switch from a running task
#define EV_TASK_SWTCH_2     0x02     //Switch from a blocked task
#define EV_TASK_SWTCH_3     0x03     //Switch from a suspended task
#define EV_TASK_SWTCH_4     0x04     //Switch from a deleted task
#define EV_TASK_CREATE      0x05
#define EV_TASK_DELETE      0x06
#define EV_TAG_BEGIN        0x07
#define EV_TAG_END          0x08
#define EV_TAG_ONESHOT      0x09



//Definition of MET Status
#define STS_INITED          (1 << 0)
#define STS_STARTED         (1 << 1)
#define STS_STOPED          (1 << 2)
#define STS_NOTIMER         (1 << 3)
#define STS_EVB_FULL        (1 << 4)
#define STS_EVB_OVERRUN     (1 << 5)
#define STS_OBJ_OVERRUN     (1 << 6)


typedef struct __attribute__((packed))
{
    uint8_t type;
    uint8_t object;
    uint16_t dts;
} event1_t;

typedef struct __attribute__((packed))
{
    uint8_t type;
    uint8_t object;
    uint16_t data;
} event2_t;

typedef struct __attribute__((packed))
{
    uint8_t object;
    uint8_t value;
    char name[configMAX_TASK_NAME_LEN];
} obj_task_t;

typedef struct __attribute__((packed))
{
    uint32_t magic;
    uint32_t length;
    uint32_t cycle_freq;
    uint32_t mode;
    uint64_t start_time;
    uint64_t stop_time;
    uint32_t status;
    uint8_t  obj_size[OBJECT_NTYPES];
    uint8_t  obj_len[OBJECT_NTYPES];
    uint8_t  obj_max[OBJECT_NTYPES];
    uint32_t obj_off[OBJECT_NTYPES];

} met_header_t;


typedef struct __attribute__((packed))
{
    met_header_t hd;
    uint32_t evb_len;
    uint32_t evb_rp;
    uint32_t evb_wp;
    uint32_t evb[MET_MAX_EVENTS];
    obj_task_t obj_task[MET_MAX_TAKS_OBJECTS];
    char obj_tag[MET_TAG_BUFFER_SIZE];
} met_t;

int met_init_timer(met_t* ptr);
uint64_t met_get_ts(void);
uint16_t met_get_dts(void);
void met_add_event(void *pev);
void met_add_event1(uint8_t event, uint8_t object);
void met_add_task_object(uint8_t object, uint8_t value, char *name);
//Pubilic API
void met_init(int mode);
int met_tag_create(char *tname);
void met_tag_start(int tid);
void met_tag_end(int tid);
void met_tag_oneshot(int tid);
void met_start(void);
void met_stop(void);

#endif //_MET_TYPES_H
