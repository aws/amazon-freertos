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

/**
 * @file exception_mt7687.h
 *
 *  Define the CM4 exception handlers which is used in startup_mt7687.s
 *
 *  Author terrence.mei
 */

#ifndef __CM4_EXCEPTION_H__
#define __CM4_EXCEPTION_H__
#include <stdint.h>
#include <stdio.h>


#define log_info(_module, _message, ...) \
    do { \
        printf("[M: %s C: info F: %s L: %d]", (_module), __FUNCTION__, __LINE__); \
        printf((_message), ##__VA_ARGS__); \
        printf("\r\n"); \
    } while (0)

#define dbg_info(_message, ...) \
    log_info("common", (_message), ##__VA_ARGS__)

#define log_fatal(_module, _message, ...) \
    do { \
        printf("[M: %s C: fatal F: %s L: %d]", (_module), __FUNCTION__, __LINE__); \
        printf((_message), ##__VA_ARGS__); \
        printf("\r\n"); \
    } while (0)

#define dbg_fatal(_message, ...) \
    log_fatal("common", (_message), ##__VA_ARGS__)

/**
 *  @note exception default priority.
 */
#define  FAULT_EXCEPTION_PRIORITY  0


/** @brief When fault exception happen,how do we finish it.
 *
 *  @note Mode of finish fault exception.
 */
typedef enum {
    FAULT_LOOP  = 0,           //just keep the state and dead loop
    FAULT_RESET = 1,           //reset whole system
    FAULT_TASK_KILL = 2,       //kill the related task
    FAULT_MAX

} fault_finish_mode_t;

/**
 *  @note CFSR status.
 */
#define MMFSR_SHIFT         0
#define MMFSR_MASK          0xFF
#define MMFSR_IACCVIOL      0x01
#define MMFSR_DACCVIOL      0x02
#define MMFSR_MUNSTKERR     0x08
#define MMFSR_MSTKERR       0x10
#define MMFSR_MLSPERR       0x20
#define MMFSR_MMARVALID     0x80


#define BFSR_SHIFT          8
#define BFSR_MASK           0xFF
#define BFSR_IBUSERR        0x01
#define BFSR_PRECISERR      0x02
#define BFSR_IMPRECISERR    0x04
#define BFSR_UNSTKERR       0x08
#define BFSR_STKERR         0x10
#define BFSR_LSPERR         0x20
#define BFSR_BFARVALID      0x80


#define UFSR_SHIFT          16
#define UFSR_MASK           0xFFFF
#define UFSR_UNDEFINSTR     0x0001
#define UFSR_INVSTATE       0x0002
#define UFSR_INVPC          0x0004
#define UFSR_NOCP           0x0008
#define UFSR_UNALIGNED      0x0100
#define UFSR_DIVBYZERO      0x0200




/** @brief Set Mode of finish fault exception.
 *
 *  @param  Fault_Finish_Mode mode
 *
 *  @return void.
 *
 */
void set_fault_finish_mode(fault_finish_mode_t mode);

/** @brief Get Mode of finish fault exception.
 *
 *  @param  void
 *
 *  @return Fault_Finish_Mode mode.
 *
 */

fault_finish_mode_t get_fault_finish_mode(void);


/** @brief  set exception interrupt priority,since exception priority is high,so defalt set priority as 0.
 *
 *  @param  void.
 *
 *  @return void.
 *
 *  @note set exception default priority.
 */
void set_fault_priority(void);


/** @brief  enable exception interrupt.
 *
 *  @param  void.
 *
 *  @return void.
 *
 *  @note enable exception interrupt.
 */
void enable_fault_exception(void);


/** @brief  disable exception interrupt.
 *
 *  @param  void.
 *
 *  @return void.
 *
 *  @note disale exception interrupt.
 */
void disable_fault_exception(void);



/** @brief  NMI exception handler for NMI Pin input,default is disable.
 *
 *  @param  void.
 *
 *  @return void.
 *
 *  @note when NMI exception happend,this function will be called.
 */
void nmi_handler(void);


/** @brief  HardFault exception handler for hart fault,such as divide 0.
 *
 *  @param  void.
 *
 *  @return void.
 *
 *  @note when HardFault exception happend,this function will be called.
 */
void HardFault_Handler(void);


/** @brief  MemManage exception handler for invalid memory access.
 *
 *  @param  void.
 *
 *  @return void.
 *
 *  @note when MemManage exception happend,this function will be called.
 */
void MemManage_Handler(void);


/** @brief  BusFault exception handler for BUS error resposne.
 *
 *  @param  void.
 *
 *  @return void.
 *
 *  @note when BusFault exception happend,this function will be called.
 */
void BusFault_Handler(void);


/** @brief  UsageFault exception handler for wrong usage of instructions.
 *
 *  @param  void.
 *
 *  @return void.
 *
 *  @note when UsageFault exception happend,this function will be called.
 */
void UsageFault_Handler(void);



#endif


