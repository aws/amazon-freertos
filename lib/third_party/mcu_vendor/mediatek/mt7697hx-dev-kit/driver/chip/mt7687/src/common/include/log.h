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

/** @file log.h
 *
 *  debug log interface file
 *
 */

#ifndef __LOG_H__
#define __LOG_H__


#include <stdio.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#define LOG_PRINT_LEVEL_INFO  (0)
#define LOG_PRINT_LEVEL_ERROR (1)
#define LOG_PRINT_LEVEL_FATAL (2)
#define LOG_PRINT_LEVEL_OFF   (3)


#define log_info(_module, _message, ...) \
    do { \
        printf("[M: %s C: info F: %s L: %d]", (_module), __FUNCTION__, __LINE__); \
        printf((_message), ##__VA_ARGS__); \
        printf("\r\n"); \
    } while (0)


#define log_error(_module, _message, ...) \
    do { \
        printf("[M: %s C: error F: %s L: %d]", (_module), __FUNCTION__, __LINE__); \
        printf((_message), ##__VA_ARGS__); \
        printf("\r\n"); \
    } while (0)


#define log_fatal(_module, _message, ...) \
    do { \
        printf("[M: %s C: fatal F: %s L: %d]", (_module), __FUNCTION__, __LINE__); \
        printf((_message), ##__VA_ARGS__); \
        printf("\r\n"); \
    } while (0)


#define log_dump(_module, _message, _data, _length, ...) \
    do { \
        printf("[M: %s F: %s L: %d]", (_module), __FUNCTION__, __LINE__); \
        printf((_message), ##__VA_ARGS__); \
        printf("\r\n"); \
        uint32_t index = 0; \
        for (index = 0; index < ((uint32_t)(_length)); index++) { \
            printf("%02X", (((uint8_t *)(_data))[index])); \
            if ((index+1)%16 == 0) { \
                printf("\r\n"); \
                continue; \
            } \
            if (index+1 != ((uint32_t)(_length))) { \
                printf(" "); \
            } \
        } \
        if (0 != index && 0 != index%16) { \
            printf("\r\n"); \
        } \
        printf("\r\n"); \
    } while (0)



#define COMMON_LOG_PRINT_LEVEL LOG_PRINT_LEVEL_ERROR


#if COMMON_LOG_PRINT_LEVEL <= LOG_PRINT_LEVEL_INFO
#define dbg_info(_message, ...) \
    log_info("common", (_message), ##__VA_ARGS__)
#else
#define dbg_info(_message, ...)
#endif


#if COMMON_LOG_PRINT_LEVEL <= LOG_PRINT_LEVEL_ERROR
#define dbg_error(_message, ...) \
    log_error("common", (_message), ##__VA_ARGS__)
#else
#define dbg_error(_message, ...)
#endif


#if COMMON_LOG_PRINT_LEVEL <= LOG_PRINT_LEVEL_FATAL
#define dbg_fatal(_message, ...) \
    log_fatal("common", (_message), ##__VA_ARGS__)
#else
#define dbg_fatal(_message, ...)
#endif


#if COMMON_LOG_PRINT_LEVEL <= LOG_PRINT_LEVEL_INFO
#define dbg_dump(_message, _data, _length, ...) \
    log_dump("common", (_message), (_data), (_length), ##__VA_ARGS__)
#else
#define dbg_dump(_message, _data, _length, ...)
#endif





#ifdef __cplusplus
}
#endif


#endif//__LOG_H__

