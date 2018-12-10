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
** $Log: low_hal_lp.c $
**
**
**
**
*/

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "mt7687.h"
#include "hal_nvic.h"
#include "hal_lp.h"
#include "hal_cache_internal.h"
#include "mt7687_cm4_hw_memmap.h"
#include "connsys_driver.h"
#include "connsys_bus.h"
#ifdef HAL_GPIO_LOW_POWER_ENABLED
#include "gpio_sleep_config.h"
#endif
#include "low_hal_gpio.h"
#include <stdio.h>

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

static uint8_t hal_lp_wic_wakeup = 0;

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

extern void uart_backup_all_registers(void);
extern void uart_restore_all_registers(void);
extern void DMA_Vfifo_restore(void);

int8_t (*ptr_lp_connsys_get_own_enable_int)(void) = NULL;
int8_t (*ptr_lp_connsys_give_n9_own)(void) = NULL;
uint8_t (*ptr_connsys_get_ownership)(void) = NULL;

static void hal_lp_cm4_hclk_switch(lhal_lp_cm4_hclk_t cm4_hclk)
{
    UINT32  cr_value = 0;
    UINT8   set_value = 0;

    /*
        CM4_HCLK_SW
            XTAL (HW default)   1.set cr_cm4_hclk_sel[2:0]=0    CM4_TOPCFGAON + 0x1B0[2:0]  3'd0
            F32K                1.set cr_cm4_hclk_sel[2:0]=1    CM4_TOPCFGAON + 0x1B0[2:0]  3'd1
            PLL1_64M            1.set cr_cm4_hclk_sel[2:0]=2    CM4_TOPCFGAON + 0x1B0[2:0]  3'd2
            PLL_CK              1.set cr_cm4_hclk_sel[2:0]=4    CM4_TOPCFGAON + 0x1B0[2:0]  3'd4
    */
    switch (cm4_hclk) {
        case LHAL_LP_CM4_HCLK_XTAL:
            set_value = 0;
            break;
        case LHAL_LP_CM4_HCLK_F32K:
            set_value = 1;
            break;
        case LHAL_LP_CM4_HCLK_PLL1_64M:
            set_value = 2;
            break;
        case LHAL_LP_CM4_HCLK_PLL_CK:
            set_value = 4;
            break;
        default:
            break;
    }
    cr_value = HAL_REG_32(TOP_AON_CM4_CKGEN0) & (~CM4_CKGEN0_CM4_HCLK_SEL_MASK);
    cr_value |= (set_value << CM4_CKGEN0_CM4_HCLK_SEL_OFFSET);
    HAL_REG_32(TOP_AON_CM4_CKGEN0) = cr_value;
}


int8_t hal_lp_connsys_get_own_enable_int()
{
    if (ptr_lp_connsys_get_own_enable_int != NULL) {
        return ptr_lp_connsys_get_own_enable_int();
    }

    return 0;
}


int8_t hal_lp_connsys_give_n9_own()
{
    if (ptr_lp_connsys_give_n9_own != NULL) {
        return ptr_lp_connsys_give_n9_own();
    }

    return 0;
}


int8_t hal_lp_connsys_get_ownership()
{
    if (ptr_connsys_get_ownership != NULL) {
        return ptr_connsys_get_ownership();
    }

    return TRUE;
}

void hal_lp_deep_sleep_set_rgu()
{
    /* Deep Sleep RGU control for Memory */

    /* ROM_CODE        (arx) : ORIGIN = 0x0,            LENGTH = 0x00005000  20k  */
    /* TCMRAM          (arw) : ORIGIN = 0x00100000,     LENGTH = 0x00010000  64k  */
    /* XIP_LOADER_CODE (arx) : ORIGIN = 0x10000000,     LENGTH = 0x00008000  32k  */
    /* XIP_CODE        (arx) : ORIGIN = 0x1006C000,     LENGTH = 0x00040000  256k */
    /* SYSRAM          (arw) : ORIGIN = 0x20000000,     LENGTH = 0x00040000  256k */

    P_IOT_CM4_RGU_MEM_CTRL_TypeDef pIOTCM4RGUMemCtrlTypeDef = (P_IOT_CM4_RGU_MEM_CTRL_TypeDef)(CM4_TOPRGU_BASE + 0x17C);

    /* set IDLM ROM PD to HW control        CM4_TOPRGU + 0x1C0 */
    /* Power Down ROM */
    /* 1bit for 16KB memory, total 64K */
    (pIOTCM4RGUMemCtrlTypeDef->CM4_ROM_PD_EN) = 0xF;

    /* CM4_MEM0_PDN_EN and CM4_MEM0_SLP_EN corresponding bits can NOT both be 0 */
    /*
        0x83009190  CM4_MEM0_PDN_EN
            31  16  CM4_RAMD_HWCTL_PDN  "1bit for 16KB memory       (SYSRAM)"
            15  8   CM4_RAMC_HWCTL_PDN  "1bit for 8KB memory        (TCM)"
            7   4   CM4_RAMB_HWCTL_PDN  "1bit for 8KB memory        (CACHE)"
            3   0   CM4_RAMA_HWCTL_PDN  "1bit for 1 Memory Macro    (HSP)"
     */
    /*
        0x83009194  CM4_MEM1_PDN_EN
            7   4   CM4_RAM_AUDIO_HWCTL_PDN "1bit for 1 Memory Macro    (AUDIO)"
            3   0   CM4_RAME_HWCTL_PDN      "1bit for 1 Memory Macro    (L1CACHE)"
     */

    /* Power Down SYSRAM except last bank */
    /* 1bit for 16KB memory */
    /* set IDLM RAM PD to HW control        CM4_TOPRGU + 0x190, 0x194 */
    (pIOTCM4RGUMemCtrlTypeDef->CM4_MEM0_PDN_EN) = 0x000000FF;
    (pIOTCM4RGUMemCtrlTypeDef->CM4_MEM1_PDN_EN) = 0x00FF; /* [7:4] Audio RAM, [3:0] L1 Cache */

    /* set IDLM RAM SLEEP to HW control     CM4_TOPRGU + 0x198, 0x19C */
    (pIOTCM4RGUMemCtrlTypeDef->CM4_MEM0_SLP_EN) = 0xFFFFFF00;
    (pIOTCM4RGUMemCtrlTypeDef->CM4_MEM1_SLP_EN) = 0x0; /* [7:4] Audio RAM, [3:0] L1 Cache */

}

void hal_lp_legacy_sleep_set_rgu()
{
    /* Deep Sleep RGU control for Memory */

    /* ROM_CODE        (arx) : ORIGIN = 0x0,            LENGTH = 0x00005000  20k  */
    /* TCMRAM          (arw) : ORIGIN = 0x00100000,     LENGTH = 0x00010000  64k  */
    /* XIP_LOADER_CODE (arx) : ORIGIN = 0x10000000,     LENGTH = 0x00008000  32k  */
    /* XIP_CODE        (arx) : ORIGIN = 0x1006C000,     LENGTH = 0x00040000  256k */
    /* SYSRAM          (arw) : ORIGIN = 0x20000000,     LENGTH = 0x00040000  256k */

    P_IOT_CM4_RGU_MEM_CTRL_TypeDef pIOTCM4RGUMemCtrlTypeDef = (P_IOT_CM4_RGU_MEM_CTRL_TypeDef)(CM4_TOPRGU_BASE + 0x17C);


    /* set IDLM ROM PD to HW control        CM4_TOPRGU + 0x1C0 */
    /* Power Down ROM */
    /* 1bit for 16KB memory, total 64K */
    (pIOTCM4RGUMemCtrlTypeDef->CM4_ROM_PD_EN) = 0xF;


    /* CM4_MEM0_PDN_EN and CM4_MEM0_SLP_EN corresponding bits can NOT both be 0 */
    /*
        0x83009190  CM4_MEM0_PDN_EN
            31  16  CM4_RAMD_HWCTL_PDN  "1bit for 16KB memory       (SYSRAM)"
            15  8   CM4_RAMC_HWCTL_PDN  "1bit for 8KB memory        (TCM)"
            7   4   CM4_RAMB_HWCTL_PDN  "1bit for 8KB memory        (CACHE)"
            3   0   CM4_RAMA_HWCTL_PDN  "1bit for 1 Memory Macro    (HSP)"
     */
    /*
        0x83009194  CM4_MEM1_PDN_EN
            7   4   CM4_RAM_AUDIO_HWCTL_PDN "1bit for 1 Memory Macro    (AUDIO)"
            3   0   CM4_RAME_HWCTL_PDN      "1bit for 1 Memory Macro    (L1CACHE)"
     */

    /* Sleep all SYSRAM */
    /* 1bit for 16KB memory */
    /* set IDLM RAM PD to HW control        CM4_TOPRGU + 0x190, 0x194 */
    (pIOTCM4RGUMemCtrlTypeDef->CM4_MEM0_PDN_EN) = 0x0;
    (pIOTCM4RGUMemCtrlTypeDef->CM4_MEM1_PDN_EN) = 0x0; /* [7:4] Audio RAM, [3:0] L1 Cache */

    /* set IDLM RAM SLEEP to HW control     CM4_TOPRGU + 0x198, 0x19C */
    (pIOTCM4RGUMemCtrlTypeDef->CM4_MEM0_SLP_EN) = 0xFFFFFFFF;
    (pIOTCM4RGUMemCtrlTypeDef->CM4_MEM1_SLP_EN) = 0x00FF; /* [7:4] Audio RAM, [3:0] L1 Cache */

}

void hal_lp_deep_sleep()
{
    P_IOT_CM4_RGU_MEM_CTRL_TypeDef pIOTCM4RGUMemCtrlTypeDef = (P_IOT_CM4_RGU_MEM_CTRL_TypeDef)(CM4_TOPRGU_BASE + 0x17C);

    /* enable RGU top_hwctl    CM4_TOPRGU + 0x180[31]  1'b1    1: deep sleep, MTCMOS power off */
    (pIOTCM4RGUMemCtrlTypeDef->CM4_POS_S_EN) |= (BIT(31));

    hal_lp_cm4_hclk_switch(LHAL_LP_CM4_HCLK_XTAL);
    hal_lp_deep_sleep_set_rgu();

    /* set HW_CONTROL   CM4_TOPCFGAON + 0x1B8[31:30]    2'b11 */
    HAL_REG_32(CM4_TOPCFGAON_BASE + 0x1B8) |= (BITS(30, 31));

    /* Can not in XIP, due to flash need AHB which should be controlled by HW instead of SW force */
    /* set AHB_STOP=1, MCUSYS_STOP=1    CM4_CONFG + 0x104[2:0]  3'b111 */
    /* HAL_REG_32(CM4_CONFIG_BASE + 0x104) |= (BITS(0, 2)); */

    /* enable CM4 System Control Register SleepDeep bit    0xE000ED10[2]   1'b1 */
    HAL_REG_32(MCU_CFG_NVIC_BASE + 0xD10) |= BIT(2);

    /* set CM4 code "__wfi();" */
    /* Could be called outside to increase flexibility */

    __asm volatile("dsb");

    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");

    __asm volatile("wfi");

    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");

    __asm volatile("isb");

    HAL_REG_32(MCU_CFG_NVIC_BASE + 0xD10) &= ~(BIT(2));
    HAL_REG_32(TOP_CFG_CM4_PWR_CTL_CR)    &= ~(BIT(30));
}


volatile nvic_sleep_backup_register_t nvic_backup_register;
//volatile CMSYS_CFG_BAKEUP_REG_T  cmsys_cfg_reg;
volatile FPU_BAKEUP_REG_T  fpu_reg;
volatile CM4_SYS_CTRL_BAKEUP_REG_T  cm4_sys_ctrl_reg;


ATTR_TEXT_IN_TCM inline uint32_t nvic_sleep_backup_register(void)
{
    uint32_t i;
    nvic_backup_register.nvic_iser = NVIC->ISER[0];
    nvic_backup_register.nvic_iser1 = NVIC->ISER[1];
    nvic_backup_register.nvic_iser2 = NVIC->ISER[2];
    for (i = 0; i < SAVE_PRIORITY_GROUP; i++) {
        nvic_backup_register.nvic_ip[i] = NVIC->IP[i];
    }
    return 0;
}

ATTR_TEXT_IN_TCM inline uint32_t nvic_sleep_restore_register(void)
{
    uint32_t i;
    NVIC->ISER[0] = nvic_backup_register.nvic_iser;
    NVIC->ISER[1] = nvic_backup_register.nvic_iser1;
    NVIC->ISER[2] = nvic_backup_register.nvic_iser2;
    for (i = 0; i < SAVE_PRIORITY_GROUP; i++) {
        NVIC->IP[i] = nvic_backup_register.nvic_ip[i];
    }
    return 0;
}

ATTR_TEXT_IN_TCM inline void fpu_status_save(void)
{
    fpu_reg.FPCCR = FPU->FPCCR;
    fpu_reg.FPCAR = FPU->FPCAR;
}

ATTR_TEXT_IN_TCM inline void fpu_status_restore(void)
{
    FPU->FPCCR = fpu_reg.FPCCR;
    FPU->FPCAR = fpu_reg.FPCAR;
}

/*
* backup CM4 system control registers
*/
ATTR_TEXT_IN_TCM void deepsleep_backup_CM4_sys_ctrl_reg(void)
{
    cm4_sys_ctrl_reg.ACTLR = SCnSCB->ACTLR;
    cm4_sys_ctrl_reg.VTOR = SCB->VTOR;
    cm4_sys_ctrl_reg.SCR = SCB->SCR;
    cm4_sys_ctrl_reg.CCR = SCB->CCR;

    cm4_sys_ctrl_reg.SHP[0] = SCB->SHP[0]; /* MemMange */
    cm4_sys_ctrl_reg.SHP[1] = SCB->SHP[1]; /* BusFault */
    cm4_sys_ctrl_reg.SHP[2] = SCB->SHP[2]; /* UsageFault */
    cm4_sys_ctrl_reg.SHP[7] = SCB->SHP[7]; /* SVCall */
    cm4_sys_ctrl_reg.SHP[8] = SCB->SHP[8]; /* DebugMonitor */
    cm4_sys_ctrl_reg.SHP[10] = SCB->SHP[10]; /* PendSV */
    cm4_sys_ctrl_reg.SHP[11] = SCB->SHP[11]; /* SysTick */

    cm4_sys_ctrl_reg.SHCSR = SCB->SHCSR;
    cm4_sys_ctrl_reg.CPACR = SCB->CPACR;

    cm4_sys_ctrl_reg.CTRL = SysTick->CTRL;
}

/*
* restore CM4 system control registers
*/
ATTR_TEXT_IN_TCM void deepsleep_restore_CM4_sys_ctrl_reg(void)
{
    SCnSCB->ACTLR = cm4_sys_ctrl_reg.ACTLR;
    SCB->VTOR = cm4_sys_ctrl_reg.VTOR;
    SCB->SCR = cm4_sys_ctrl_reg.SCR;
    SCB->CCR = cm4_sys_ctrl_reg.CCR;

    SCB->SHP[0] = cm4_sys_ctrl_reg.SHP[0]; /* MemMange */
    SCB->SHP[1] = cm4_sys_ctrl_reg.SHP[1]; /* BusFault */
    SCB->SHP[2] = cm4_sys_ctrl_reg.SHP[2]; /* UsageFault */
    SCB->SHP[7] = cm4_sys_ctrl_reg.SHP[7]; /* SVCall */
    SCB->SHP[8] = cm4_sys_ctrl_reg.SHP[8]; /* DebugMonitor */
    SCB->SHP[10] = cm4_sys_ctrl_reg.SHP[10]; /* PendSV */
    SCB->SHP[11] = cm4_sys_ctrl_reg.SHP[11]; /* SysTick */

    SCB->SHCSR = cm4_sys_ctrl_reg.SHCSR;
    SCB->CPACR = cm4_sys_ctrl_reg.CPACR;

    SysTick->CTRL = cm4_sys_ctrl_reg.CTRL;
}




#if     defined (__GNUC__)      //GCC disable compiler optimize
#pragma GCC push_options
#pragma GCC optimize ("O0")
#elif   defined (__ICCARM__)    //IAR disable compiler optimize
#pragma optimize=none
#elif   defined (__CC_ARM)      //MDK disable compiler optimize
#pragma push
#pragma diag_suppress 1267
#pragma O0
#endif
volatile uint32_t deepsleep_wakeup_address;
volatile uint32_t temp_addr, temp_reg;
volatile uint32_t origin_msp_bak_reg, origin_psp_bak_reg, backup_return_address;
void hal_lp_deep_sleep_backup_restore()
{
    P_IOT_CM4_RGU_MEM_CTRL_TypeDef pIOTCM4RGUMemCtrlTypeDef = (P_IOT_CM4_RGU_MEM_CTRL_TypeDef)(CM4_TOPRGU_BASE + 0x17C);

    /* enable RGU top_hwctl    CM4_TOPRGU + 0x180[31]  1'b1    1: deep sleep, MTCMOS power off */
    (pIOTCM4RGUMemCtrlTypeDef->CM4_POS_S_EN) |= (BIT(31));

    hal_lp_cm4_hclk_switch(LHAL_LP_CM4_HCLK_XTAL);
    hal_lp_deep_sleep_set_rgu();

    /* set HW_CONTROL   CM4_TOPCFGAON + 0x1B8[31:30]    2'b11 */
    HAL_REG_32(CM4_TOPCFGAON_BASE + 0x1B8) |= (BITS(30, 31));

    /* Can not in XIP, due to flash need AHB which should be controlled by HW instead of SW force */
    /* set AHB_STOP=1, MCUSYS_STOP=1    CM4_CONFG + 0x104[2:0]  3'b111 */
    /* HAL_REG_32(CM4_CONFIG_BASE + 0x104) |= (BITS(0, 2)); */

    /* enable CM4 System Control Register SleepDeep bit    0xE000ED10[2]   1'b1 */
    HAL_REG_32(MCU_CFG_NVIC_BASE + 0xD10) |= BIT(2);


    /* UART backup */
    uart_backup_all_registers();

    /* GPIO backup */
    gpio_backup_all_registers();

    /* Start back up Cmsys */
    /* NVIC backup */
    nvic_sleep_backup_register();

#ifdef HAL_CACHE_MODULE_ENABLED
    /* cache backcp */
    cache_status_save();
#endif

#ifdef HAL_MPU_MODULE_ENABLED
    /* mpu backcp */
    mpu_status_save();
#endif

    /* start backup */
    fpu_status_save();

    /* CM4 system control registers backup*/
    deepsleep_backup_CM4_sys_ctrl_reg();

    __CPU_STACK_POINT_BACKUP(origin_msp_bak_reg);

    /* backup PSP Address */
#if (defined (__GNUC__) || defined (__ICCARM__))
    __asm volatile("push {r0-r12, lr}");
    __asm volatile("mov %0, sp" : "=r"(origin_psp_bak_reg));
#elif defined (__CC_ARM)
    __PUSH_CPU_REG();
    __BACKUP_SP(origin_psp_bak_reg);
#endif



    // enter sleep
    temp_addr = (uint32_t)&deepsleep_wakeup_address;
    __ENTER_DEEP_SLEEP(temp_addr);




    /* restore MSP */
    temp_reg = (unsigned int)&origin_msp_bak_reg;
    __MSP_RESTORE(temp_reg);

    /* swtich stack point to psp */
    __SWITCH_TO_PSP_STACK_POINTER();

    /* restore PSP */
    temp_reg = (unsigned int)&origin_psp_bak_reg;
    __PSP_RESTORE(temp_reg);

#if (defined (__GNUC__) || defined (__ICCARM__))
    __asm volatile("pop {r0-r12, lr}");
#elif defined (__CC_ARM)
    __POP_CPU_REG();
#endif

    /* Start restore Cmsys */
    /* CM4 system control registers restore*/
    deepsleep_restore_CM4_sys_ctrl_reg();

    /* fpu restore */
    fpu_status_restore();

    /* cmsys config restore */
    //cmsys_cfg_status_restore();

#ifdef HAL_MPU_MODULE_ENABLED
    /* mpu restore */
    mpu_status_restore();
#endif

#ifdef HAL_CACHE_MODULE_ENABLED
    /* cache restore */
    cache_status_restore();
#endif

    /* NVIC restore */
    nvic_sleep_restore_register();
    /* End restore Cmsys */

    /* GPIO restore */
    gpio_restore_all_registers();

    DMA_Vfifo_restore();

    /* UART restore */
    uart_restore_all_registers();

    HAL_REG_32(MCU_CFG_NVIC_BASE + 0xD10) &= ~(BIT(2));
    HAL_REG_32(TOP_CFG_CM4_PWR_CTL_CR)    &= ~(BIT(30));
}
#if     defined (__GNUC__)
#pragma GCC push_options
#elif   defined (__CC_ARM)
#pragma pop
#endif


void hal_lp_legacy_sleep()
{
    P_IOT_CM4_RGU_MEM_CTRL_TypeDef pIOTCM4RGUMemCtrlTypeDef = (P_IOT_CM4_RGU_MEM_CTRL_TypeDef)(CM4_TOPRGU_BASE + 0x17C);

    /* disable RGU top_hwctl   CM4_TOPRGU + 0x180[31]  1'b0    0: legacy sleep, keep MTCMOS power on */
    (pIOTCM4RGUMemCtrlTypeDef->CM4_POS_S_EN) &= (~BIT(31));

    hal_lp_cm4_hclk_switch(LHAL_LP_CM4_HCLK_XTAL);
    hal_lp_legacy_sleep_set_rgu();

    /* set HW_CONTROL   CM4_TOPCFGAON + 0x1B8[31:30]    2'b11 */
    HAL_REG_32(CM4_TOPCFGAON_BASE + 0x1B8) |= (BITS(30, 31));

    /* Can not in XIP, due to flash need AHB which should be controlled by HW instead of SW force */
    /* set AHB_STOP=1, MCUSYS_STOP=1    CM4_CONFG + 0x104[2:0]  3'b111 */
    /* HAL_REG_32(CM4_CONFIG_BASE + 0x104) |= (BITS(0, 2)); */

    /* enable CM4 System Control Register SleepDeep bit    0xE000ED10[2]   1'b1 */
    HAL_REG_32(MCU_CFG_NVIC_BASE + 0xD10) |= BIT(2);

    /* set CM4 code "__wfi();" */
    /* Could be called outside to increase flexibility */
    __asm volatile("dsb");

    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");

    __asm volatile("wfi");

    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");

    __asm volatile("isb");

    HAL_REG_32(MCU_CFG_NVIC_BASE + 0xD10) &= ~(BIT(2));
    HAL_REG_32(TOP_CFG_CM4_PWR_CTL_CR)    &= ~(BIT(30));
}

static void hal_lp_wic_isr(hal_nvic_irq_t irq_number)
{
    /* clear WIC IRQ status */
    HAL_REG_32(CM4_WIC_SW_CLR_ADDR) = 0x1;

    hal_lp_wic_wakeup = 1;
}

void hal_lp_handle_intr()
{
    hal_nvic_register_isr_handler((hal_nvic_irq_t)CM4_WIC_INT_WAKE_UP_IRQ, hal_lp_wic_isr);
    NVIC_SetPriority((hal_nvic_irq_t)CM4_WIC_INT_WAKE_UP_IRQ, CM4_WIC_INT_WAKE_UP_PRI);
    NVIC_EnableIRQ((hal_nvic_irq_t)CM4_WIC_INT_WAKE_UP_IRQ);
}

uint8_t hal_lp_get_wic_wakeup()
{
    return hal_lp_wic_wakeup;
}

uint32_t hal_lp_get_wic_status()
{
    return (HAL_REG_32(CM4_WIC_PEND_STA0_ADDR) & BIT(31));
}

#ifdef HAL_GPIO_LOW_POWER_ENABLED
void hal_lp_set_PALDO(void)
{
    uint32_t val = 0;
    val = *((volatile uint32_t*)0x81021430);
    val &= ~(0x40000000);
    val |=  (1 << 30);
    *((volatile uint32_t*)0x81021430) = val;

    val = *((volatile uint32_t*)0x81021430);
    val &= ~(0x80000000);
    val |=  (uint32_t)(1U << 31);
    *((volatile uint32_t*)0x81021430) = val;
}

void hal_lp_set_gpio_sleep_mode(void)
{
    uint32_t value = 0;

    // Default all pull-up
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x60) = 0x88888888;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x64) = 0x88888888;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x68) = 0x88888888;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x6C) = 0x88888888;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x70) = 0x88888888;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x74) = 0x88888888;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x78) = 0x88888888;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x7C) = 0x88888888;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x80) = 0x88888888;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x84) = 0x88888884;

    //GPIO 0 ~ 6
    value = HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x60);
    value &= ~(0xFFFFFFFF);
    value |= ( (PINMUX_AON_SLEEP_CONFIG_GPIO0)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO1 << 4)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO2 << 8)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO3 << 12)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO4 << 16)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO5 << 20)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO6 << 24)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO7 << 28));
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x60) = value;

    //GPIO 24 ~ 31
    value = HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x6C);
    value &= ~(0xFFFFFFFF);
    value |= ( (PINMUX_AON_SLEEP_CONFIG_GPIO24)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO25 << 4)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO26 << 8)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO27 << 12)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO28 << 16)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO29 << 20)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO30 << 24)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO31 << 28) );
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x6C) = value;

    //GPIO 32 ~ 39
    value = HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x70);
    value &= ~(0xFFFFFFFF);
    value |= ( (PINMUX_AON_SLEEP_CONFIG_GPIO32)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO33 << 4)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO34 << 8)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO35 << 12)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO36 << 16)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO37 << 20)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO38 << 24)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO39 << 28) );
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x70) = value;

    //GPIO 57 ~ 60
    value = HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x7C);
    value &= ~(0x000FFFF0);
    value |= ( (PINMUX_AON_SLEEP_CONFIG_GPIO57 << 4)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO58 << 8)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO59 << 12)
            | (PINMUX_AON_SLEEP_CONFIG_GPIO60 << 16) );
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x7C) = value;

    /* IO low power settings Section A */
/*
    // A-1. Pull Up PAD_IN_GPIO8 ~ PAD_IN_GPIO22, PAD_IN_UART0_TXD (0x80025000, Bit23 ~ Bit8)
    // Reg[0x80025000] |=  0x00FFFF00
    HAL_REG_32(IOT_GPIO_OFF_BASE) |= 0x00FFFF00;
    // A-2. Pull Up PAD_IN_GPIO44 ~ PAD_IN_PWM7 (0x80025010, Bit24 ~ Bit8)
    // Reg[0x80025010] |=  0x01FFFF00
    HAL_REG_32(IOT_GPIO_OFF_BASE + 0x10) |= 0x01FFFF00;
    // A-3. Pull Up PAD_IN_ADC6 ~ PAD_IN_ADC4 (0x80025010, Bit31 ~ Bit29)
    // Reg[0x80025010] |=  0xE0000000
    HAL_REG_32(IOT_GPIO_OFF_BASE + 0x10) |= 0xE0000000;
    // A-4. Pull Up PAD_IN_ADC15 ~ ADC7 (0x80025020, Bit8 ~ Bit0)
    // Reg[0x80025020] |=  0x000001FF
    HAL_REG_32(IOT_GPIO_OFF_BASE + 0x20) |= 0x000001FF;
*/
/*    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x48) &= ~0xFFFFFFFF;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x48) |=  0x00000902;

    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x4C) &= ~0xFFFFFFFF;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x4C) |=  0x00000902;

    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x50) &= ~0xFFFFFFFF;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x50) |=  0x00000902;

    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x54) &= ~0xFFFFFFFF;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x54) |=  0x00000902;

    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x58) &= ~0xFFFFFFFF;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x58) |=  0x00000902;

    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x5C) &= ~0xFFFFFFFF;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x5C) |=  0x00000902;
*/

    // A-7. Enable IO SLP function
    // Default all enable
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE) = 0xFFFFFFFF;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x04) = 0xFFFFFFFF;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x08) = 0x0000FFFF;

    value = HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE);
    value &= ~(0xFF0000FF);
    value |= ( (PINMUX_AON_SLEEP_ENABLE_GPIO0)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO1 << 1)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO2 << 2)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO3 << 3)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO4 << 4)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO5 << 5)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO6 << 6)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO7 << 7)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO24 << 24)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO25 << 25)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO26 << 26)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO27 << 27)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO28 << 28)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO29 << 29)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO30 << 30)
            | ((uint32_t)PINMUX_AON_SLEEP_ENABLE_GPIO31 << 31) );
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE) = value;

    value = HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x04);
    value &= ~(0x1E0000FF);
    value |= ( (PINMUX_AON_SLEEP_ENABLE_GPIO32)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO33 << 1)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO34 << 2)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO35 << 3)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO36 << 4)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO37 << 5)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO38 << 6)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO39 << 7)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO57 << 25)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO58 << 26)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO59 << 27)
            | (PINMUX_AON_SLEEP_ENABLE_GPIO60 << 28) );
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x04) = value;


    /* IO low power settings Section C */
/*
    // C-1. SIP_CK
    // IO SLP: input, pull-up
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x84) &= ~0x000000F0;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x84) |=  0x00000080;

    // C-2. SIP_CS
    // IO SLP: input, pull-up
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x84) &= ~0x00000F00;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x84) |=  0x00000800;

    // C-3. SIP_D0
    // IO SLP: input, pull-up
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x84) &= ~0x0000F000;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x84) |=  0x00008000;

    // C-4. SIP_D1
    // IO SLP: input, pull-up
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x84) &= ~0x000F0000;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x84) |=  0x00080000;

    // C-5. SIP_D2
    // IO SLP: input, pull-up
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x84) &= ~0x00F00000;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x84) |=  0x00800000;

    // C-6. SIP_D3
    // IO SLP: input, pull-up
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x84) &= ~0x0F000000;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x84) |=  0x08000000;

    // C-7. HSPIN
    // IO SLP: input, pull-up
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x84) &= ~0xF0000000;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x84) |=  0x80000000;

    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x84) &= ~0x0000000F;
    HAL_REG_32(IOT_GPIO_PINMUX_AON_BASE + 0x84) |=  0x00000004;
*/
}
#endif
