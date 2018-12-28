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
* File Name    : r_bsp_common.c
* Description  : Implements functions that apply to all r_bsp boards and MCUs.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 06.05.2013 1.00     First Release
*         : 26.03.2014 1.10     Added R_BSP_SoftwareDelay() function
*         : 03.09.2014 1.20     Corrected R_BSP_SoftwareDelay() timing when using an RX64M
*         : 30.09.2015 1.30     Added RX23T
*         : 01.02.2016 1.40     Added RX24T
*                               Changed the value of the following macro definition.
*                               - OVERHEAD_CYCLES
*                               - OVERHEAD_CYCLES_64
*         : 29.02.2016 1.50     Added RX230
*         : 01.10.2016 1.60     Added RX65N
*         : 22.08.2016 1.70     Added RX24U
*         : 15.05.2017 1.80     Changed method of selecting the number of CPU cycles required to execute 
*                               the delayWait() loop.
*         : xx.xx.xxxx 1.90     Added support for GNUC and ICCRX.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Get information about current board and MCU. */
#include "platform.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define OVERHEAD_CYCLES        (2)    // R_BSP_SoftwareDelay() overhead per call
#define OVERHEAD_CYCLES_64     (2)    // R_BSP_SoftwareDelay() overhead per call using 64-bit ints

#define CKSEL_LOCO             (0x0)  // SCKCR3 register setting for LOCO

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
extern uint32_t get_iclk_freq_hz(void);  // Get the board specific ICLK frequency

/***********************************************************************************************************************
* Function Name: R_BSP_GetVersion
* Description  : Returns the current version of this module. The version number is encoded where the top 2 bytes are the
*                major version number and the bottom 2 bytes are the minor version number. For example, Version 4.25
*                would be returned as 0x00040019.
* Arguments    : none
* Return Value : Version of this module.
***********************************************************************************************************************/
R_PRAGMA_INLINE(R_BSP_GetVersion) uint32_t R_BSP_GetVersion (void)
{
    /* These version macros are defined in platform.h. */
    return ((((uint32_t)R_BSP_VERSION_MAJOR) << 16) | (uint32_t)R_BSP_VERSION_MINOR);
}


/***********************************************************************************************************************
* Function Name: delayWait
* Description  : This asm loop executes a known number (5) of CPU cycles. If a value of '4' is passed
*                in as an argument, then this function would consume 20 CPU cycles before returning.
* Arguments    : loop_cnt - A single 32-bit value is provided as the number of loops to execute.
*              :
* Return Value : None
***********************************************************************************************************************/
R_PRAGMA_STATIC_INLINE_ASM(delayWait)
void delayWait (unsigned long loop_cnt)
{
    R_ASM_INTERNAL_USED(loop_cnt)
    R_ASM_BEGIN
    R_ASM(    BRA.B    R_LAB_NEXT(0)    )
    R_ASM(    NOP                       ) // FIXME: What is the purpose of this NOP?
    R_LAB(0:                            )
    R_ASM(    NOP                       )
    R_ASM(    SUB     #01H, R1          )
    R_ASM(    BNE.B   R_LAB_PREV(0)     )
    R_ASM_END
}


/***********************************************************************************************************************
* Function Name: R_BSP_GetIClkFreqHz
* Description  : Return the current ICLK frequency in Hz.
* Arguments    : None
* Return Value : uint32_t - the system ICLK frequency in Hz
***********************************************************************************************************************/
uint32_t R_BSP_GetIClkFreqHz(void)
{
    return get_iclk_freq_hz();  // Get the MCU specific ICLK frequency
}


/***********************************************************************************************************************
* Function Name: R_BSP_SoftwareDelay
* Description  : Delay the specified duration in units and return.
* Arguments    : uint32_t delay  - the number of 'units' to delay
*              : bsp_delay_units_t units - the 'base' for the units specified. Valid values are:
*                  BSP_DELAY_MICROSECS, BSP_DELAY_MILLISECS, BSP_DELAY_SECS.
*
*                Accuracy is good, however the minimum possible delay depends on the current ICLK frequency
*                and the overhead clock cycles required for this function to run.
*                For example, given a 16 MHz ICLK and 180 clock cycles for this function, the minimum
*                possible delay would be: 1/16000000 * 180 = 11.25 uS
*
* Return Value : true if delay executed.
*                false if delay/units combination resulted in overflow or the delay cannot be achieved
***********************************************************************************************************************/
bool R_BSP_SoftwareDelay(uint32_t delay, bsp_delay_units_t units)
{
    volatile uint32_t iclkRate;
    volatile uint32_t delay_cycles;
    volatile uint32_t loop_cnt;
    volatile uint64_t delay_cycles_64;
    volatile uint64_t loop_cnt_64;

#ifdef BSP_CFG_PARAM_CHECKING_ENABLE
    if ((units != BSP_DELAY_MICROSECS) && (units != BSP_DELAY_MILLISECS) && (units != BSP_DELAY_SECS))
    {
        return(false);
    }
#endif

    iclkRate = R_BSP_GetIClkFreqHz();  // Get the current ICLK frequency

    /*
     * In order to handle all possible combinations of delay/ICLK it is necessary to use 64-bit
     * integers (not all MCUs have floating point support). However, there is no native hw support
     * for 64 bit integers so it requires many more clock cycles. This is not an issue if the
     * requested delay is long enough and the ICLK is fast, but for delays in the low microseconds
     * and/or a slow ICLK we use 32 bit integers to reduce the overhead cycles of this function
     * by approximately a third and stand the best chance of achieving the requested delay.
     */
    if ( (units == BSP_DELAY_MICROSECS) &&
         (delay <= (0xFFFFFFFFUL / iclkRate)) )  // Ensure (iclkRate * delay) will not exceed 32 bits
    {
        delay_cycles = ((iclkRate * delay) / units);

        if (delay_cycles > OVERHEAD_CYCLES)
        {
            delay_cycles -= OVERHEAD_CYCLES;
        }
        else
        {
            delay_cycles = 0;
        }

        loop_cnt = delay_cycles / CPU_CYCLES_PER_LOOP;

        if (loop_cnt == 0)
        {
            /* The requested delay is too large/small for the current ICLK. Return false which
             * also results in the minimum possible delay. */
            return(false);
        }
    }
    else
    {
        delay_cycles_64 = (((uint64_t)iclkRate * (uint64_t)delay) / units);

        if (delay_cycles_64 > OVERHEAD_CYCLES_64)
        {
            delay_cycles_64 -= OVERHEAD_CYCLES_64;
        }
        else
        {
            delay_cycles = 0;
        }

        loop_cnt_64 = delay_cycles_64 / CPU_CYCLES_PER_LOOP;

        if ((loop_cnt_64 > 0xFFFFFFFF) || (loop_cnt_64 == 0))
        {
            /* The requested delay is too large/small for the current ICLK. Return false which
             * also results in the minimum possible delay. */
            return(false);
        }

        loop_cnt = (uint32_t)loop_cnt_64;
    }

    delayWait(loop_cnt);

    return(true);
}


/***********************************************************************************************************************
* Function name: R_BSP_Change_PSW_PM_to_UserMode
* Description  : Switches to user mode. The PSW will be changed as following.
*                Before Execution                                       After Execution
*                PSW.PM                 PSW.U                           PSW.PM              PSW.U
*                0 (supervisor mode)    0 (interrupt stack)     -->     1 (user mode)       1 (user stack)
*                0 (supervisor mode)    1 (user stack)          -->     1 (user mode)       1 (user stack)
*                1 (user mode)          1 (user stack)          -->     NO CHANGE
*                1 (user mode)          0 (interrupt stack))    <==     N/A
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
R_PRAGMA_INLINE_ASM(R_BSP_Change_PSW_PM_to_UserMode)
void R_BSP_Change_PSW_PM_to_UserMode(void)
{
    R_ASM_BEGIN
    R_ASM(;_R_BSP_Change_PSW_PM_to_UserMode:                                           )
    R_ASM(    mvfc    psw, r1     ; get the current PSW value                          )
    R_ASM(    btst    #20, r1     ; check PSW.PM                                       )
    R_ASM(    bne.b   R_LAB_NEXT(0);_psw_pm_is_user_mode                               )
    R_ASM(;_psw_pm_is_supervisor_mode:                                                 )
    R_ASM(    pop     r2          ; pop the return address value of caller             )
    R_ASM(    bset    #20, r1     ; change PM = 0(Supervisor Mode) --> 1(User Mode)    )
    R_ASM(    push.l  r1          ; push new PSW value which will be changed           )
    R_ASM(    push.l  r2          ; push return address value                          )
    R_ASM(    rte                                                                      )
    R_LAB(0:;_psw_pm_is_user_mode:                                                     )
    R_ASM(    ;rts                                                                     )
    R_ASM_END
}
