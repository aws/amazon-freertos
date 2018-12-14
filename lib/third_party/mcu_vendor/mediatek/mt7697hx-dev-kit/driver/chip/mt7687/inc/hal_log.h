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

#ifndef __HAL_LOG_H__
#define __HAL_LOG_H__

/**
 * @filel_log.h.
 *
 * The debug printing macros that were used by HAL module is defined in this
 * header file.
 *
 * Feature option: MTK_HAL_PLAIN_LOG_ENABLE
 *
 * This feature option turns the LOG system integration off in HAL module.
 * It was added to allow bootloader, which has no operating system in it, to
 * be able to use HAL layer code.
 */

#include "hal_feature_config.h"

#if !defined(MTK_HAL_PLAIN_LOG_ENABLE) && !defined(MTK_HAL_NO_LOG_ENABLE)
#include "syslog.h"
#else
#include "stdio.h"
#endif /* !defined(MTK_HAL_PLAIN_LOG_ENABLE) */

#if !defined(__GNUC__) && defined(MTK_BL_LOAD_ENABLE)
#include "hw_uart.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
extern void log_hal_fatal_internal(const char *func, int line, const char *message, ...);
extern void log_hal_error_internal(const char *func, int line, const char *message, ...);
extern void log_hal_warning_internal(const char *func, int line, const char *message, ...);
extern void log_hal_info_internal(const char *func, int line, const char *message, ...);
extern void log_hal_dump_internal(const char *func, int line, const char *message, const void *data, int length, ...);


// For KEIL build will hang if bootloader use printf
#if !defined(__GNUC__) && defined(MTK_BL_LOAD_ENABLE)
#define log_hal_fatal(_message,...)                hw_uart_printf((_message),##__VA_ARGS__)
#define log_hal_error(_message,...)                hw_uart_printf((_message),##__VA_ARGS__)
#define log_hal_warning(_message,...)              hw_uart_printf((_message),##__VA_ARGS__)
#define log_hal_info(_message,...)                 hw_uart_printf((_message),##__VA_ARGS__)
#define log_hal_dump(_message, _data, _len, ...) 

#else
#define log_hal_fatal(_message,...)                log_hal_fatal_internal(__FUNCTION__, __LINE__, (_message),##__VA_ARGS__)
#define log_hal_error(_message,...)                log_hal_error_internal(__FUNCTION__, __LINE__, (_message),##__VA_ARGS__)
#define log_hal_warning(_message,...)              log_hal_warning_internal(__FUNCTION__, __LINE__, (_message),##__VA_ARGS__)
#define log_hal_info(_message,...)                 log_hal_info_internal(__FUNCTION__, __LINE__, (_message),##__VA_ARGS__)
#define log_hal_dump(_message, _data, _len, ...)   log_hal_dump_internal(__FUNCTION__, __LINE__, (_message), (_data), (_len),##__VA_ARGS__)

#endif



#if !defined(MTK_HAL_PLAIN_LOG_ENABLE) && !defined(MTK_HAL_NO_LOG_ENABLE)
#define log_hal_create(_level)                     log_create_module(hal, (_level))

/*
#define log_hal_config_print_switch(_switch)        log_config_print_switch(hal, (_switch))
#define log_hal_config_print_level(_level)          log_config_print_level(hal, (_level))
#define log_hal_config_print_func(_print_func)      log_config_print_func(hal, (_print_func))
#define log_hal_config_dump_func(_dump_func)        log_config_dump_func(hal, (_dump_func))
*/

#endif /* defined(MTK_HAL_PLAIN_LOG_ENABLE) */

#ifdef __cplusplus
}
#endif

#endif /* __HAL_LOG_H__ */

