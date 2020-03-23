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
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_bsp_cpu.c
* Description  : This module implements CPU specific functions. An example is enabling/disabling interrupts.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 28.02.2019 3.00     Merged processing of all devices.
*                               Added support for GNUC and ICCRX.
*                               Fixed coding style.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Platform support. */
#include "platform.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#ifdef BSP_MCU_REGISTER_WRITE_PROTECTION
/* Key code for writing PRCR register. */
#define BSP_PRV_PRCR_KEY        (0xA500)
#endif

#ifdef BSP_MCU_VOLTAGE_LEVEL_SETTING
/* The macro definition for combinations where settings of USBVON bit conflict. */
#define BSP_PRV_USBVON_CONFLICT (BSP_VOL_USB_POWEROFF | BSP_VOL_USB_POWERON)
/* The macro definition for combinations where settings of PGAVLS bit conflict. */
#define BSP_PRV_PGAVLS_CONFLICT (BSP_VOL_AD_NEGATIVE_VOLTAGE_INPUT | BSP_VOL_AD_NEGATIVE_VOLTAGE_NOINPUT)
/* The macro definition for combinations where settings of RICVLS bit conflict. */
#define BSP_PRV_RICVLS_CONFLICT (BSP_VOL_RIIC_4_5V_OROVER | BSP_VOL_RIIC_UNDER_4_5V)
/* Bit number of VOLSR register. */
#define BSP_PRV_VOLSR_RICVLS_BIT_NUM  (7)
#define BSP_PRV_VOLSR_PGAVLS_BIT_NUM  (6)
#define BSP_PRV_VOLSR_USBVON_BIT_NUM  (2)
#endif

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
#ifdef BSP_MCU_REGISTER_WRITE_PROTECTION
/* Used for holding reference counters for protection bits. */
static volatile uint16_t gs_protect_counters[BSP_REG_PROTECT_TOTAL_ITEMS];

/* Masks for setting or clearing the PRCR register. Use -1 for size because PWPR in MPC is used differently. */
static const    uint16_t gs_prcr_masks[BSP_REG_PROTECT_TOTAL_ITEMS-1] = 
{
#ifdef BSP_MCU_RCPC_PRC0
    0x0001,         /* PRC0. */
#endif
#ifdef BSP_MCU_RCPC_PRC1
    0x0002,         /* PRC1. */
#endif
#ifdef BSP_MCU_RCPC_PRC2
    0x0004,         /* PRC2. */
#endif
#ifdef BSP_MCU_RCPC_PRC3
    0x0008,         /* PRC3. */
#endif
};
#endif

/***********************************************************************************************************************
* Function Name: R_BSP_InterruptsDisable
* Description  : Globally disable interrupts.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void R_BSP_InterruptsDisable (void)
{
    /* Use the compiler intrinsic function to clear the I flag. */
    R_BSP_CLRPSW_I();
} /* End of function R_BSP_InterruptsDisable() */

/***********************************************************************************************************************
* Function Name: R_BSP_InterruptsEnable
* Description  : Globally enable interrupts.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void R_BSP_InterruptsEnable (void)
{
    /* Use the compiler intrinsic function to set the I flag. */
    R_BSP_SETPSW_I();
} /* End of function R_BSP_InterruptsEnable() */

/***********************************************************************************************************************
* Function Name: R_BSP_CpuInterruptLevelRead
* Description  : Reads the processor interrupt priority level.
* Arguments    : none
* Return Value : The current processor IPL
***********************************************************************************************************************/
uint32_t R_BSP_CpuInterruptLevelRead (void)
{
    /* Use the compiler intrinsic function to read the CPU IPL. */
    uint32_t psw_value;

    /* Casting is valid because it matches the type to the right side or argument. */
    psw_value = (uint32_t)R_BSP_GET_PSW();
    psw_value = psw_value & 0x0f000000;
    psw_value = psw_value >> 24;

    return psw_value;
} /* End of function R_BSP_CpuInterruptLevelRead() */

/***********************************************************************************************************************
* Function Name: R_BSP_CpuInterruptLevelWrite
* Description  : Writes the processor interrupt priority level.
* Arguments    : level -
*                    The level to set the processor's IPL to.
* Return Value : true -
*                    The level was set successfully.
*                false -
*                    Invalid level input. 
***********************************************************************************************************************/
bool R_BSP_CpuInterruptLevelWrite (uint32_t level)
{
    bool ret;

    /* The R_BSP_SET_IPL() function use the MVTIPL instruction.
       The MVTIPL instruction needs to set an immediate value to src. */

    ret = true;

    /* Use the compiler intrinsic function to set the CPU IPL. */
    switch (level)
    {
        case (0):

            /* IPL = 0 */
            R_BSP_SET_IPL(0);
            break;

        case (1):

            /* IPL = 1 */
            R_BSP_SET_IPL(1);
            break;

        case (2):

            /* IPL = 2 */
            R_BSP_SET_IPL(2);
            break;

        case (3):

            /* IPL = 3 */
            R_BSP_SET_IPL(3);
            break;

        case (4):

            /* IPL = 4 */
            R_BSP_SET_IPL(4);
            break;

        case (5):

            /* IPL = 5 */
            R_BSP_SET_IPL(5);
            break;

        case (6):

            /* IPL = 6 */
            R_BSP_SET_IPL(6);
            break;

        case (7):

            /* IPL = 7 */
            R_BSP_SET_IPL(7);
            break;

#if 7 < BSP_MCU_IPL_MAX
        case (8):

            /* IPL = 8 */
            R_BSP_SET_IPL(8);
            break;

        case (9):

            /* IPL = 9 */
            R_BSP_SET_IPL(9);
            break;

        case (10):

            /* IPL = 10 */
            R_BSP_SET_IPL(10);
             break;

        case (11):

            /* IPL = 11 */
            R_BSP_SET_IPL(11);
            break;

        case (12):

            /* IPL = 12 */
            R_BSP_SET_IPL(12);
            break;

        case (13):

            /* IPL = 13 */
            R_BSP_SET_IPL(13);
            break;

        case (14):

            /* IPL = 14 */
            R_BSP_SET_IPL(14);
            break;

        case (15):

            /* IPL = 15 */
            R_BSP_SET_IPL(15);
            break;
#endif /* BSP_MCU_IPL_MAX */

        default:
            ret = false;
            break;
    }

    return ret;
} /* End of function R_BSP_CpuInterruptLevelWrite() */

/***********************************************************************************************************************
* Function Name: R_BSP_RegisterProtectEnable
* Description  : Enables register protection. Registers that are protected cannot be written to. Register protection is 
*                enabled by using the Protect Register (PRCR) and the MPC's Write-Protect Register (PWPR).
* Arguments    : regs_to_unprotect -
*                    Which registers to disable write protection for. See typedef defines of bsp_reg_protect_t.
* Return Value : none
***********************************************************************************************************************/
void R_BSP_RegisterProtectEnable (bsp_reg_protect_t regs_to_protect)
{
#ifdef BSP_MCU_REGISTER_WRITE_PROTECTION
    volatile uint32_t    ipl_value;
    bool                 ret;

    /* Get the current Processor Interrupt Priority Level (IPL). */
    ipl_value = R_BSP_CpuInterruptLevelRead();

    /* Set IPL to the maximum value to disable all interrupts,
     * so the scheduler can not be scheduled in critical region.
     * Note: Please set this macro more than IPR for other FIT module interrupts. */
    if (ipl_value < BSP_CFG_FIT_IPL_MAX)
    {
        ret = R_BSP_CpuInterruptLevelWrite(BSP_CFG_FIT_IPL_MAX);
        if (false == ret)
        {
            /* check return value */
            R_BSP_NOP();
        }
    }

    /* Is it safe to disable write access? */
    if (0 != gs_protect_counters[regs_to_protect])
    {
        /* Decrement the protect counter */
        gs_protect_counters[regs_to_protect]--;
    }

    /* Is it safe to disable write access? */
    if (0 == gs_protect_counters[regs_to_protect])
    {
        if (BSP_REG_PROTECT_MPC != regs_to_protect)
        {
            /* Enable protection using PRCR register. */
            /* When writing to the PRCR register the upper 8-bits must be the correct key. Set lower bits to 0 to
               disable writes.
               b15:b8 PRKEY - Write 0xA5 to upper byte to enable writing to lower byte
               b7:b4  Reserved (set to 0)
               b3     PRC3  - Please check the user's manual.
               b2     PRC2  - Please check the user's manual.
               b1     PRC1  - Please check the user's manual.
               b0     PRC0  - Please check the user's manual.
            */
            SYSTEM.PRCR.WORD = (uint16_t)((SYSTEM.PRCR.WORD | BSP_PRV_PRCR_KEY) & (~gs_prcr_masks[regs_to_protect]));
        }
        else
        {
            /* Enable protection for MPC using PWPR register. */
            /* Enable writing of PFSWE bit. It could be assumed that the B0WI bit is still cleared from a call to
               protection disable function, but it is written here to make sure that the PFSWE bit always gets
               cleared. */
            MPC.PWPR.BIT.B0WI = 0;

            /* Disable writing to PFS registers. */
            MPC.PWPR.BIT.PFSWE = 0;

            /* Disable writing of PFSWE bit. */
            MPC.PWPR.BIT.B0WI = 1;
        }
    }

    /* Restore the IPL. */
    if (ipl_value < BSP_CFG_FIT_IPL_MAX)
    {
        ret = R_BSP_CpuInterruptLevelWrite(ipl_value);
        if (false == ret)
        {
            /* check return value */
            R_BSP_NOP();
        }
    }

#else /* BSP_MCU_REGISTER_WRITE_PROTECTION */
    /* No registers to protect. */
    /* This code is only used to remove compiler info messages about this parameter not being used. */
    INTERNAL_NOT_USED(regs_to_protect);
#endif /* BSP_MCU_REGISTER_WRITE_PROTECTION */
} /* End of function R_BSP_RegisterProtectEnable() */

/***********************************************************************************************************************
* Function Name: R_BSP_RegisterProtectDisable
* Description  : Disables register protection. Registers that are protected cannot be written to. Register protection is
*                disabled by using the Protect Register (PRCR) and the MPC's Write-Protect Register (PWPR).
* Arguments    : regs_to_unprotect -
*                    Which registers to disable write protection for. See typedef defines of bsp_reg_protect_t.
* Return Value : none
***********************************************************************************************************************/
void R_BSP_RegisterProtectDisable (bsp_reg_protect_t regs_to_unprotect)
{
#ifdef BSP_MCU_REGISTER_WRITE_PROTECTION
    volatile uint32_t    ipl_value;
    bool                 ret;

    /* Get the current Processor Interrupt Priority Level (IPL). */
    ipl_value = R_BSP_CpuInterruptLevelRead();

    /* Set IPL to the maximum value to disable all interrupts,
     * so the scheduler can not be scheduled in critical region.
     * Note: Please set this macro more than IPR for other FIT module interrupts. */
    if (ipl_value < BSP_CFG_FIT_IPL_MAX)
    {
        ret = R_BSP_CpuInterruptLevelWrite(BSP_CFG_FIT_IPL_MAX);
        if (false == ret)
        {
            /* check return value */
            R_BSP_NOP();
        }
    }

    /* If this is first entry then disable protection. */
    if (0 == gs_protect_counters[regs_to_unprotect])
    {
        if (BSP_REG_PROTECT_MPC != regs_to_unprotect)
        {
            /* Enable protection using PRCR register. */
            /* When writing to the PRCR register the upper 8-bits must be the correct key.
               Set lower bits to 1 to enable writes.
               b15:b8 PRKEY - Write 0xA5 to upper byte to enable writing to lower byte
               b7:b4  Reserved (set to 0)
               b3     PRC3  - Please check the user's manual.
               b2     PRC2  - Please check the user's manual.
               b1     PRC1  - Please check the user's manual.
               b0     PRC0  - Please check the user's manual.
            */
            SYSTEM.PRCR.WORD = (uint16_t)((SYSTEM.PRCR.WORD | BSP_PRV_PRCR_KEY) | gs_prcr_masks[regs_to_unprotect]);
        }
        else
        {
            /* Disable protection for MPC using PWPR register. */
            /* Enable writing of PFSWE bit. */
            MPC.PWPR.BIT.B0WI = 0;

            /* Enable writing to PFS registers. */
            MPC.PWPR.BIT.PFSWE = 1;
        }
    }

    /* Increment the protect counter */
    gs_protect_counters[regs_to_unprotect]++;

    /* Restore the IPL. */
    if (ipl_value < BSP_CFG_FIT_IPL_MAX)
    {
        ret = R_BSP_CpuInterruptLevelWrite(ipl_value);
        if (false == ret)
        {
            /* check return value */
            R_BSP_NOP();
        }
    }

#else /* BSP_MCU_REGISTER_WRITE_PROTECTION */
    /* No registers to protect. */
    /* This code is only used to remove compiler info messages about this parameter not being used. */
    INTERNAL_NOT_USED(regs_to_unprotect);
#endif /* BSP_MCU_REGISTER_WRITE_PROTECTION */
} /* End of function R_BSP_RegisterProtectDisable() */

#ifdef BSP_MCU_VOLTAGE_LEVEL_SETTING
/***********************************************************************************************************************
* Function Name: R_BSP_VoltageLevelSetting
* Description  : Writes the voltage level setting.
* Arguments    : ctrl_ptn -
*                    Register Setting Patterns
*                [Options]
*                    BSP_VOL_USB_POWEROFF
*                    BSP_VOL_USB_POWERON
*                    BSP_VOL_AD_NEGATIVE_VOLTAGE_INPUT
*                    BSP_VOL_AD_NEGATIVE_VOLTAGE_NOINPUT
*                    BSP_VOL_RIIC_4_5V_OROVER
*                    BSP_VOL_RIIC_UNDER_4_5V
* Return Value : true -
*                    The pattern was set successfully. The VOLSR register was updated.
*                false -
*                    The function was called under one of the following conditions, so the VOLSR register setting was 
*                    not updated.
*                    - Setting patterns that cannot be selected at the same time were specified.
*                    - A setting pattern related to the USB was selected when the USB was not in the module stop state.
*                    - A setting pattern related to the AD was selected when the AD was not in the module stop state.
*                    - A setting pattern related to the RIIC was selected when the RIIC was not in the module stop 
*                      state.
* Note         : When specifying a setting pattern related to the USB, call the function before the USB is released 
*                from the module stop state. When specifying a setting pattern related to the AD, call the function 
*                before the AD is released from the module stop state. Also when specifying a setting pattern related 
*                to the RIIC, call the function before the RIIC is released from the module stop state.
***********************************************************************************************************************/
bool R_BSP_VoltageLevelSetting (uint8_t ctrl_ptn)
{
    uint8_t  *p_volsr_addr;

#if BSP_CFG_PARAM_CHECKING_ENABLE == 1
    /* ---- CHECK ARGUMENTS ---- */
    if (BSP_PRV_USBVON_CONFLICT == (ctrl_ptn & BSP_PRV_USBVON_CONFLICT))
    {
        return false;
    }

    if (BSP_PRV_PGAVLS_CONFLICT == (ctrl_ptn & BSP_PRV_PGAVLS_CONFLICT))
    {
        return false;
    }

    if (BSP_PRV_RICVLS_CONFLICT == (ctrl_ptn & BSP_PRV_RICVLS_CONFLICT))
    {
        return false;
    }
#endif

    /* Check USB module stop state. */
    if(0 != (ctrl_ptn & BSP_PRV_USBVON_CONFLICT))
    {
        /* Casting is valid because it matches the type to the right side or argument. */
        if(0 == MSTP(USB0))
        {
            return false;
        }
    }

    /* Check AD module stop state. */
    if(0 != (ctrl_ptn & BSP_PRV_PGAVLS_CONFLICT))
    {
        /* Casting is valid because it matches the type to the right side or argument. */
        if((0 == MSTP(S12AD)) || (0 == MSTP(S12AD1)))
        {
            return false;
        }
    }

    /* Check RIIC module stop state. */
    if(0 != (ctrl_ptn & BSP_PRV_RICVLS_CONFLICT))
    {
        /* Casting is valid because it matches the type to the right side or argument. */
        if(0 == MSTP(RIIC0))
        {
            return false;
        }
    }

    /* Protect off. */
    SYSTEM.PRCR.WORD = 0xA502;

    /* Casting is valid because it matches the type to the right side or argument. */
    p_volsr_addr = (uint8_t *)&SYSTEM.VOLSR.BYTE;

    /* Updated the RICVLS bit. */
    if(0 != (ctrl_ptn & BSP_VOL_RIIC_UNDER_4_5V))
    {
        R_BSP_BIT_SET(p_volsr_addr, BSP_PRV_VOLSR_RICVLS_BIT_NUM);
    }

    if(0 != (ctrl_ptn & BSP_VOL_RIIC_4_5V_OROVER))
    {
        R_BSP_BIT_CLEAR(p_volsr_addr, BSP_PRV_VOLSR_RICVLS_BIT_NUM);
    }

    /* Updated the PGAVLS bit. */
    if(0 != (ctrl_ptn & BSP_VOL_AD_NEGATIVE_VOLTAGE_NOINPUT))
    {
        R_BSP_BIT_SET(p_volsr_addr, BSP_PRV_VOLSR_PGAVLS_BIT_NUM);
    }

    if(0 != (ctrl_ptn & BSP_VOL_AD_NEGATIVE_VOLTAGE_INPUT))
    {
        R_BSP_BIT_CLEAR(p_volsr_addr, BSP_PRV_VOLSR_PGAVLS_BIT_NUM);
    }

    /* Updated the USBVON bit. */
    if(0 != (ctrl_ptn & BSP_VOL_USB_POWERON))
    {
        R_BSP_BIT_SET(p_volsr_addr, BSP_PRV_VOLSR_USBVON_BIT_NUM);
    }

    if(0 != (ctrl_ptn & BSP_VOL_USB_POWEROFF))
    {
        R_BSP_BIT_CLEAR(p_volsr_addr, BSP_PRV_VOLSR_USBVON_BIT_NUM);
    }

    /* Protect on. */
    SYSTEM.PRCR.WORD = 0xA500;

    return true;
}  /* End of function R_BSP_VoltageLevelSetting() */ 
#endif /* BSP_MCU_VOLTAGE_LEVEL_SETTING */

/***********************************************************************************************************************
* Function Name: bsp_register_protect_open
* Description  : Initializes variables needed for register protection functionality.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void bsp_register_protect_open (void)
{
#ifdef BSP_MCU_REGISTER_WRITE_PROTECTION
    uint32_t i;

    /* Initialize reference counters to 0. */
    /* WAIT_LOOP */
    for (i = 0; i < BSP_REG_PROTECT_TOTAL_ITEMS; i++)
    {
        gs_protect_counters[i] = 0;
    }
#else
    /* No registers to protect. */
#endif
} /* End of function bsp_register_protect_open() */

/***********************************************************************************************************************
* Function Name: bsp_ram_initialize
* Description  : Initialize ram variable.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void bsp_ram_initialize (void)
{
    uint32_t i;

    /* Initialize g_bsp_Locks to 0. */
    /* WAIT_LOOP */
    for (i = 0; i < BSP_NUM_LOCKS; i++)
    {
        g_bsp_Locks[i].lock = 0;
    }
} /* End of function bsp_ram_initialize() */

