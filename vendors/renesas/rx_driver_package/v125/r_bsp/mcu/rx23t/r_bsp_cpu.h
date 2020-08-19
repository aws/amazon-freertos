/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_bsp_cpu.h
* Description  : This module implements CPU specific functions. An example is enabling/disabling interrupts.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 30.09.2015 1.00     First Release
*         : 13.04.2016 1.01     Added the setting of PRC2 (HOCOWTCR).
*         : 28.02.2019 2.00     Added bsp_ram_initialize function.
*                               Fixed coding style.
*         : 26.07.2019 2.01     Added R_BSP_SoftwareReset function.
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Multiple inclusion prevention macro */
#ifndef CPU_H
#define CPU_H

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* The different types of registers that can be protected. */
typedef enum
{
    /* PRC0
       Enables writing to the Flash MEMWAIT register and the clock generation registers: SCKCR, SCKCR3, PLLCR, PLLCR2,
       MOSCCR, LOCOCR, ILOCOCR, OSTDCR, OSTDSR, MEMWAIT */
    BSP_REG_PROTECT_CGC = 0,

    /* PRC1
       Enables writing to the registers related to operating modes, low power consumption, the clock generation circuit,
       and software reset: SYSCR1, SBYCR, MSTPCRA, MSTPCRB, MSTPCRC, MSTPCRD, OPCCR, MOFCR, MOSCWTCR, SWRR */
    BSP_REG_PROTECT_LPC_CGC_SWR,

    /* PRC2
       Enables writing to the registers related to the clock generation circuit: HOCOWTCR. */
    BSP_REG_PROTECT_HOCOWTCR,

    /* PRC3
       Enables writing to the registers related to the LVD: LVCMPCR, LVDLVLR, LVD1CR0, LVD1CR1, LVD1SR, LVD2CR0,
       LVD2CR1, LVD2SR */
    BSP_REG_PROTECT_LVD,

    /* MPC.PWPR
       Enables writing to MPC's PFS registers. */
    BSP_REG_PROTECT_MPC,

    /* This entry is used for getting the number of enum items. This must be the last entry. DO NOT REMOVE THIS ENTRY!*/
    BSP_REG_PROTECT_TOTAL_ITEMS
} bsp_reg_protect_t;

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
void     R_BSP_InterruptsDisable(void);
void     R_BSP_InterruptsEnable(void);
uint32_t R_BSP_CpuInterruptLevelRead(void);
bool     R_BSP_CpuInterruptLevelWrite(uint32_t level);
void     R_BSP_RegisterProtectEnable(bsp_reg_protect_t regs_to_protect);
void     R_BSP_RegisterProtectDisable(bsp_reg_protect_t regs_to_unprotect);
void     R_BSP_SoftwareReset(void);

void     bsp_register_protect_open(void); //r_bsp internal function. DO NOT CALL.
void     bsp_ram_initialize(void);

#endif /* CPU_H */

