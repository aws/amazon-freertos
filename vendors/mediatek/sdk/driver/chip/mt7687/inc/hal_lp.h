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

/*
** $Log: low_hal_lp.h $
**
**
**
**
*/

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

#ifndef __LOW_HAL_LP_H__
#define __LOW_HAL_LP_H__


#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include <stdint.h>
#include "mt7687.h"
#include "memory_attribute.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/


/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

typedef enum {
    LHAL_LP_CM4_HCLK_XTAL = 0,
    LHAL_LP_CM4_HCLK_F32K,
    LHAL_LP_CM4_HCLK_PLL1_64M,
    LHAL_LP_CM4_HCLK_PLL_CK,
    LHAL_LP_CM4_HCLK_NUM
} lhal_lp_cm4_hclk_t;


#define SAVE_PRIORITY_GROUP (IRQ_NUMBER_MAX)
typedef struct {
    uint32_t nvic_iser;                    /**< eint hardware debounce time */
    uint32_t nvic_iser1;
    uint32_t nvic_iser2;
    uint32_t nvic_ip[SAVE_PRIORITY_GROUP];
} nvic_sleep_backup_register_t;


/* CMSYS_CFG backup register struct
*/
typedef struct {
    volatile uint32_t STCALIB;
    volatile uint32_t AHB_BUFFERALBE;
    volatile uint32_t AHB_FIFO_TH;
    volatile uint32_t INT_ACTIVE_HL0;
    volatile uint32_t INT_ACTIVE_HL1;
    volatile uint32_t DCM_CTRL_REG;
} CMSYS_CFG_BAKEUP_REG_T;


typedef struct {
    volatile uint32_t FPCCR;
    volatile uint32_t FPCAR;
} FPU_BAKEUP_REG_T;


typedef struct {
    volatile uint32_t ACTLR;    /* Auxiliary Control Register */
    volatile uint32_t VTOR;     /* Vector Table Offset Register */
    volatile uint32_t SCR;      /* System Control Register */
    volatile uint32_t CCR;      /* Configuration Control Register */
    volatile uint8_t SHP[12];   /* System Handlers Priority Registers (4-7, 8-11, 12-15) */
    volatile uint32_t SHCSR;    /* System Handler Control and State Register */
    volatile uint32_t CPACR;    /* Coprocessor Access Control Register */
    volatile uint32_t CTRL;     /* Systick Control Register */
} CM4_SYS_CTRL_BAKEUP_REG_T;


/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/


/*
========================================================================
Routine Description:

Note:
========================================================================
*/
#if (defined (__GNUC__) || defined (__ICCARM__))    /* GCC,IAR compiler */

#define __CPU_CORE_REG_BACKUP(ptr) __asm volatile(  \
    "PUSH   {r0-r12, lr}                \n"\
    "MOV    r2,         %0              \n"\
    "MOV    r0,         r2              \n"\
    "MRS    r1,         psr             \n"\
    "STR    r1,         [r0],#4         \n"\
    "MRS    r1,         primask         \n"\
    "STR    r1,         [r0],#4         \n"\
    "MRS    r1,         faultmask       \n"\
    "STR    r1,         [r0],#4         \n"\
    "MRS    r1,         basepri         \n"\
    "STR    r1,         [r0],#4         \n"\
    "MRS    r1,         control         \n"\
    "STR    r1,         [r0]            \n"\
    "POP    {r0-r12,lr}                   "\
    : :"r" (ptr):                          \
  );

#define __CPU_STACK_POINT_BACKUP(msp) {         \
    __asm volatile("MOV r0, #0");               \
    __asm volatile("MSR control, r0");          \
    __asm volatile("ISB");                      \
    __asm volatile("MOV %0, sp" : "=r"(msp));   \
    __asm volatile("MOV r0, #6");               \
    __asm volatile("MSR control, r0");          \
    __asm volatile("ISB");                      \
}

#define __MSP_BACKUP(prt) __asm volatile(  \
    "MOV    r2,         %0              \n"\
    "MOV    r0,         r2              \n"\
    "LDR    r0,         [r0]            \n"\


#define __MSP_RESTORE(ptr) __asm volatile( \
    "MOV    r2,         %0              \n"\
    "MOV    r0,         r2              \n"\
    "LDR    r0,         [r0]            \n"\
    "MSR    msp,        r0              \n"\
    "ISB                                  "\
    : :"r"(ptr)                            \
  );

#define __SWITCH_TO_PSP_STACK_POINTER() __asm volatile(  \
    "MOV    r0,         #6              \n"\
    "MSR    control,    r0              \n"\
    "ISB                                  "\
  );

#define __PSP_RESTORE(ptr) __asm volatile(  \
    "MOV    r2,         %0              \n"\
    "MOV    r0,         r2              \n"\
    "LDR    r0,         [r0]            \n"\
    "MSR    psp,        r0              \n"\
    "ISB                                  "\
    : :"r"(ptr)                            \
  );

#define __CPU_CORE_REG_RESTORE(ptr) __asm volatile(  \
    "PUSH   {r0-r12, lr}                \n"\
    "MOV    r2,         %0              \n"\
    "MOV    r0,         r2              \n"\
    "LDR    r1, [r0],   #4              \n"\
    "MSR    psr,        r1              \n"\
    "LDR    r1, [r0],   #4              \n"\
    "MSR    primask,    r1              \n"\
    "LDR    r1, [r0],   #4              \n"\
    "MSR    faultmask,  r1              \n"\
    "LDR    r1, [r0],   #4              \n"\
    "MSR    basepri,    r1              \n"\
    "LDR    r1,         [r0],#4         \n"\
    "MSR    control,    r1              \n"\
    "POP    {r0-r12,lr}                   "\
    : :"r" (ptr):                          \
  );

#define __ENTER_DEEP_SLEEP(ptr) __asm volatile(  \
    "MOV    r3,         %0              \n"\
    "MOV    r2,         pc              \n"\
    "ADD    r2,         r2,#80          \n"\
    "ORR    r2,         r2,#1           \n"\
    "STR    r2,         [r3]            \n"\
    "DSB                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "WFI                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "NOP                                \n"\
    "ISB                                \n"\
    : :"r"(ptr)                            \
  );

#elif defined (__CC_ARM)    /* MDK compiler */

#define __CPU_CORE_REG_BACKUP(ptr) __asm volatile( \
    "MOV        r2,         "#ptr"              \n"\
    "MOV        r0,         r2                  \n"\
    "MRS        r1,         psr                 \n"\
    "STR        r1,         [r0],#4             \n"\
    "MRS        r1,         primask             \n"\
    "STR        r1,         [r0],#4             \n"\
    "MRS        r1,         faultmask           \n"\
    "STR        r1,         [r0],#4             \n"\
    "MRS        r1,         basepri             \n"\
    "STR        r1,         [r0],#4             \n"\
    "MRS        r1,         control             \n"\
    "STR        r1,         [r0]                \n"\
  )

#define __CPU_CORE_REG_RESTORE(ptr) __asm volatile( \
    "MOV        r2,         "#ptr"               \n"\
    "MOV        r0,         r2                   \n"\
    "LDR        r1,         [r0],#4              \n"\
    "MSR        psr,        r1                   \n"\
    "LDR        r1,         [r0],#4              \n"\
    "MSR        primask,    r1                   \n"\
    "LDR        r1,[r0],    #4                   \n"\
    "MSR        faultmask,  r1                   \n"\
    "LDR        r1,[r0],    #4                   \n"\
    "MSR        basepri,    r1                   \n"\
    "LDR        r1,[r0],    #4                   \n"\
    "MSR        control,    r1                   \n"\
)

#define __CPU_STACK_POINT_BACKUP(msp) {             \
    __asm volatile("MOV r0, #0");                   \
    __asm volatile("MSR control, r0");              \
    __asm volatile("ISB");                          \
    __asm volatile("MOV "#msp", __current_sp()");   \
    __asm volatile("MOV r0, #6");                   \
    __asm volatile("MSR control, r0");              \
    __asm volatile("ISB");                          \
}

#define __MSP_RESTORE(ptr) __asm volatile( \
    "MOV    r2,         "#ptr"          \n"\
    "MOV    r0,         r2              \n"\
    "LDR    r0,         [r0]            \n"\
    "MSR    msp,        r0              \n"\
    "ISB                                \n"\
  )

#define __SWITCH_TO_PSP_STACK_POINTER() __asm volatile( \
    "MOV    r0,         #6                           \n"\
    "MSR    control,    r0                           \n"\
    "ISB                                               "\
  );

#define __PSP_RESTORE(ptr) __asm volatile( \
    "MOV    r2,         "#ptr"          \n"\
    "MOV    r0,         r2              \n"\
    "LDR    r0,         [r0]            \n"\
    "MSR    psp,        r0              \n"\
    "ISB                                \n"\
  );

#define __ENTER_DEEP_SLEEP(ptr) __asm volatile( \
    "MOV    r3,         "#ptr"               \n"\
    "MOV    r2,         __current_pc()       \n"\
    "ADD    r2,         r2,#80               \n"\
    "ORR    r2,         r2,#1                \n"\
    "STR    r2,         [r3]                 \n"\
    "DSB                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "WFI                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "NOP                                     \n"\
    "ISB                                     \n"\
)

#define __BACKUP_SP(ptr) __asm volatile("MOV "#ptr", __current_sp()\n")

ATTR_TEXT_IN_TCM static __inline __asm volatile void __PUSH_CPU_REG(void)
{
    PRESERVE8
    PUSH    {r0 - r12}
    BX      r14
}

ATTR_TEXT_IN_TCM static __inline __asm volatile void __POP_CPU_REG(void)
{
    PRESERVE8
    POP     {r0 - r12}
    BX      r14
}

ATTR_TEXT_IN_TCM static __inline __asm volatile void __RESTORE_LR(unsigned int return_address)
{
    PRESERVE8
    MOV     lr, r0
    BX      r14
}
#endif  /* MDK compiler */

extern int8_t hal_lp_connsys_give_n9_own(void);
extern int8_t hal_lp_connsys_get_own_enable_int(void);
extern int8_t hal_lp_connsys_get_ownership(void);
extern void hal_lp_deep_sleep_set_rgu(void);
extern void hal_lp_legacy_sleep_set_rgu(void);
extern void hal_lp_deep_sleep(void);
extern void hal_lp_deep_sleep_backup_restore(void);
extern void hal_lp_legacy_sleep(void);
extern void hal_lp_handle_intr(void);
extern uint8_t hal_lp_get_wic_wakeup(void);
extern uint32_t hal_lp_get_wic_status(void);
#if (defined (__GNUC__) || defined (__CC_ARM))
__attribute__(( weak )) void hal_lp_set_PALDO(void);
#else//__ICCARM__, IAR
__weak void hal_lp_set_PALDO(void);
#endif
#ifdef HAL_GPIO_LOW_POWER_ENABLED
extern void hal_lp_set_gpio_sleep_mode(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __LOW_HAL_LP_H__ */

