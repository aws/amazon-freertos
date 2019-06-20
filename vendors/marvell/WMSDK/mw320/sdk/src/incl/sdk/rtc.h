/** @file rtc.h
*
*  @brief RTC Header
*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
*
*  MARVELL CONFIDENTIAL
*  The source code contained or described herein and all documents related to
*  the source code ("Material") are owned by Marvell International Ltd or its
*  suppliers or licensors. Title to the Material remains with Marvell
*  International Ltd or its suppliers and licensors. The Material contains
*  trade secrets and proprietary and confidential information of Marvell or its
*  suppliers and licensors. The Material is protected by worldwide copyright
*  and trade secret laws and treaty provisions. No part of the Material may be
*  used, copied, reproduced, modified, published, uploaded, posted,
*  transmitted, distributed, or disclosed in any way without Marvell's prior
*  express written permission.
*
*  No license under any patent, copyright, trade secret or other intellectual
*  property right is granted to or conferred upon you by disclosure or delivery
*  of the Materials, either expressly, by implication, inducement, estoppel or
*  otherwise. Any license under such intellectual property rights must be
*  express and approved by Marvell in writing.
*
*/

#ifndef RTC_H
#define RTC_H

#include <wmtime.h>

#ifdef CONFIG_HW_RTC
#define rtc_init       hwrtc_init
#define rtc_time_set   hwrtc_time_set
#define rtc_time_get   hwrtc_time_get

extern void hwrtc_init(void);
extern int hwrtc_time_set(time_t time);
extern time_t hwrtc_time_get(void);
extern void hwrtc_time_update(void);
#else
/* Use Software RTC (Real Time Clock) if no hardware RTC is available */
#define rtc_init       swrtc_init
#define rtc_time_set   swrtc_time_set
#define rtc_time_get   swrtc_time_get

extern void swrtc_init(void);
extern int swrtc_time_set(time_t time);
extern time_t swrtc_time_get(void);

/** Time stored in POSIX format (Seconds since epoch) */
extern time_t cur_posix_time;

/** Absolute number of ticks */
extern unsigned int abs_tick_count;
#endif
#endif
