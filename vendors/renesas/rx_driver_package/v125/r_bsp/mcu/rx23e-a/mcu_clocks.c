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
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : mcu_clocks.c
* Description  : Contains clock specific routines
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.07.2019 1.00     First Release
*         : 17.12.2019 1.01     Deleted the unused variables of clock_source_select function and 
*                               lpc_clock_source_select function.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "platform.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define BSP_PRV_CKSEL_LOCO            (0x0)
#define BSP_PRV_CKSEL_HOCO            (0x1)
#define BSP_PRV_CKSEL_MAIN_OSC        (0x2)
#define BSP_PRV_CKSEL_PLL             (0x4)

#define BSP_PRV_NORMALIZE_X10  (10)   /* used to avoid floating point arithmetic */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* When using the user startup program, disable the following code. */
#if BSP_CFG_STARTUP_DISABLE == 0
static void operating_frequency_set(void);
static void clock_source_select(void);
static void lpt_clock_source_select(void);
#endif /* BSP_CFG_STARTUP_DISABLE == 0 */

/***********************************************************************************************************************
* Function Name: get_iclk_freq_hz
* Description  : Return the current ICLK frequency in Hz.  Called by R_BSP_GetIClkFreqHz().
*                The system clock source can be changed at any time via SYSTEM.SCKCR3.BIT.CKSEL, so in order to
*                determine the ICLK frequency we need to first find the current system clock source and then,
*                in some cases where the clock source can be configured for multiple frequencies, calculate the
*                frequency at which it is currently running.
* Arguments    : None
* Return Value : uint32_t - the iclk frequency in Hz
***********************************************************************************************************************/
uint32_t get_iclk_freq_hz(void)
{
    uint32_t sys_clock_src_freq;
    uint32_t pll_multiplier;

    /* Casting is valid because it matches the type to the retern value. */
    uint8_t  cksel = (uint8_t)SYSTEM.SCKCR3.BIT.CKSEL;

    switch (cksel)
    {
        case BSP_PRV_CKSEL_LOCO:
            sys_clock_src_freq = BSP_LOCO_HZ;
            break;

        case BSP_PRV_CKSEL_HOCO:

            /* 32 MHz */
            sys_clock_src_freq = 32000000;
            break;

        case BSP_PRV_CKSEL_MAIN_OSC:
            sys_clock_src_freq = BSP_CFG_XTAL_HZ;
            break;

        case BSP_PRV_CKSEL_PLL:

            /* Casting is valid because it matches the type to the retern value. */
            pll_multiplier = ((((uint32_t)(SYSTEM.PLLCR.BIT.STC + 1)) * BSP_PRV_NORMALIZE_X10) / 2);

            /* Casting is valid because it matches the type to the retern value. */
            sys_clock_src_freq = ((BSP_CFG_XTAL_HZ / (((uint32_t)(1 << SYSTEM.PLLCR.BIT.PLIDIV)) * BSP_PRV_NORMALIZE_X10)) * pll_multiplier);
            break;

        default:

            /* Should never arrive here. Use the Main OSC freq as a default... */
            sys_clock_src_freq = BSP_CFG_XTAL_HZ;
            break;
    }

    /* Finally, divide the system clock source frequency by the currently set ICLK divider to get the ICLK frequency */
    return (sys_clock_src_freq / (uint32_t)(1 << SYSTEM.SCKCR.BIT.ICK));
} /* End of function get_iclk_freq_hz() */

/* When using the user startup program, disable the following code. */
#if BSP_CFG_STARTUP_DISABLE == 0

/***********************************************************************************************************************
* Function name: mcu_clock_setup
* Description  : Contains clock functions called at device restart.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
void mcu_clock_setup(void)
{
    /* Switch to high-speed operation */
    operating_frequency_set();
    lpt_clock_source_select();
} /* End of function mcu_clock_setup() */

/***********************************************************************************************************************
* Function name: operating_frequency_set
* Description  : Configures the clock settings for each of the device clocks
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void operating_frequency_set (void)
{
    /* Used for constructing value to write to SCKCR, and SCKCR3 registers. */
    uint32_t tmp_clock = 0;

    /* Protect off. */
    SYSTEM.PRCR.WORD = 0xA50B;

    /* Select the clock based upon user's choice. */
    clock_source_select();

    /* Figure out setting for FCK bits. */
#if   BSP_CFG_FCK_DIV == 1
    /* Do nothing since FCK bits should be 0. */
#elif BSP_CFG_FCK_DIV == 2
    tmp_clock |= 0x10000000;
#elif BSP_CFG_FCK_DIV == 4
    tmp_clock |= 0x20000000;
#elif BSP_CFG_FCK_DIV == 8
    tmp_clock |= 0x30000000;
#elif BSP_CFG_FCK_DIV == 16
    tmp_clock |= 0x40000000;
#elif BSP_CFG_FCK_DIV == 32
    tmp_clock |= 0x50000000;
#elif BSP_CFG_FCK_DIV == 64
    tmp_clock |= 0x60000000;
#else
    #error "Error! Invalid setting for BSP_CFG_FCK_DIV in r_bsp_config.h"
#endif

    /* Figure out setting for ICK bits. */
#if   BSP_CFG_ICK_DIV == 1
    /* Do nothing since ICK bits should be 0. */
#elif BSP_CFG_ICK_DIV == 2
    tmp_clock |= 0x01000000;
#elif BSP_CFG_ICK_DIV == 4
    tmp_clock |= 0x02000000;
#elif BSP_CFG_ICK_DIV == 8
    tmp_clock |= 0x03000000;
#elif BSP_CFG_ICK_DIV == 16
    tmp_clock |= 0x04000000;
#elif BSP_CFG_ICK_DIV == 32
    tmp_clock |= 0x05000000;
#elif BSP_CFG_ICK_DIV == 64
    tmp_clock |= 0x06000000;
#else
    #error "Error! Invalid setting for BSP_CFG_ICK_DIV in r_bsp_config.h"
#endif

    /* Figure out setting for PCKA bits. */
#if   BSP_CFG_PCKA_DIV == 1
    /* Do nothing since PCKA bits should be 0. */
#elif BSP_CFG_PCKA_DIV == 2
    tmp_clock |= 0x00001000;
#elif BSP_CFG_PCKA_DIV == 4
    tmp_clock |= 0x00002000;
#elif BSP_CFG_PCKA_DIV == 8
    tmp_clock |= 0x00003000;
#elif BSP_CFG_PCKA_DIV == 16
    tmp_clock |= 0x00004000;
#elif BSP_CFG_PCKA_DIV == 32
    tmp_clock |= 0x00005000;
#elif BSP_CFG_PCKA_DIV == 64
    tmp_clock |= 0x00006000;
#else
    #error "Error! Invalid setting for BSP_CFG_PCKA_DIV in r_bsp_config.h"
#endif

    /* Figure out setting for PCKB bits. */
#if   BSP_CFG_PCKB_DIV == 1
    /* Do nothing since PCKB bits should be 0. */
#elif BSP_CFG_PCKB_DIV == 2
    tmp_clock |= 0x00010100;
#elif BSP_CFG_PCKB_DIV == 4
    tmp_clock |= 0x00020200;
#elif BSP_CFG_PCKB_DIV == 8
    tmp_clock |= 0x00030300;
#elif BSP_CFG_PCKB_DIV == 16
    tmp_clock |= 0x00040400;
#elif BSP_CFG_PCKB_DIV == 32
    tmp_clock |= 0x00050500;
#elif BSP_CFG_PCKB_DIV == 64
    tmp_clock |= 0x00060600;
#else
    #error "Error! Invalid setting for BSP_CFG_PCKB_DIV in r_bsp_config.h"
#endif

    /* Figure out setting for PCKD bits. */
#if   BSP_CFG_PCKD_DIV == 1
    /* Do nothing since PCKD bits should be 0. */
#elif BSP_CFG_PCKD_DIV == 2
    tmp_clock |= 0x00000001;
#elif BSP_CFG_PCKD_DIV == 4
    tmp_clock |= 0x00000002;
#elif BSP_CFG_PCKD_DIV == 8
    tmp_clock |= 0x00000003;
#elif BSP_CFG_PCKD_DIV == 16
    tmp_clock |= 0x00000004;
#elif BSP_CFG_PCKD_DIV == 32
    tmp_clock |= 0x00000005;
#elif BSP_CFG_PCKD_DIV == 64
    tmp_clock |= 0x00000006;
#else
    #error "Error! Invalid setting for BSP_CFG_PCKD_DIV in r_bsp_config.h"
#endif

    /* b7 to b4 should be 0x0.
       b22 to b20 should be 0x0. */

    /* Set SCKCR register. */
    SYSTEM.SCKCR.LONG = tmp_clock;

    /* Dummy read and compare. cf."5. I/O Registers", "(2) Notes on writing to I/O registers" in User's manual.
       This is done to ensure that the register has been written before the next register access. The RX has a 
       pipeline architecture so the next instruction could be executed before the previous write had finished.
    */
    if(tmp_clock ==  SYSTEM.SCKCR.LONG)
    {
        R_BSP_NOP();
    }

    /* Choose clock source. Default for r_bsp_config.h is PLL. */
    tmp_clock = ((uint16_t)BSP_CFG_CLOCK_SOURCE) << 8;

    /* Casting is valid because it matches the type to the retern value. */
    SYSTEM.SCKCR3.WORD = (uint16_t)tmp_clock;

    /* Dummy read and compare. cf."5. I/O Registers", "(2) Notes on writing to I/O registers" in User's manual.
       This is done to ensure that the register has been written before the next register access. The RX has a 
       pipeline architecture so the next instruction could be executed before the previous write had finished.
    */
    if((uint16_t)tmp_clock ==  SYSTEM.SCKCR3.WORD)
    {
        R_BSP_NOP();
    }

#if BSP_CFG_CLOCK_SOURCE != 0
    /* We can now turn LOCO off since it is not going to be used. */
    SYSTEM.LOCOCR.BYTE = 0x01;

    /* Wait for five the LOCO cycles */
    /* 5 count of LOCO : (1000000/3440000)*5 = 1.453488us
       1.45 + 2 = 3.45us ("+2" is overhead cycle) */
    R_BSP_SoftwareDelay((uint32_t)4, BSP_DELAY_MICROSECS);
#endif

    /* Protect on. */
    SYSTEM.PRCR.WORD = 0xA500;
} /* End of function operating_frequency_set() */

/***********************************************************************************************************************
* Function name: clock_source_select
* Description  : Enables and disables clocks as chosen by the user.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void clock_source_select (void)
{
    /* Set to High-speed operating mode if clock is > 12MHz. */
    /* Because ICLK may not be maximum frequency, it is necessary to check all clock frequency. */
    if( (BSP_ICLK_HZ  > BSP_MIDDLE_SPEED_MAX_FREQUENCY)||
        (BSP_PCLKA_HZ > BSP_MIDDLE_SPEED_MAX_FREQUENCY)||
        (BSP_PCLKB_HZ > BSP_MIDDLE_SPEED_MAX_FREQUENCY)||
        (BSP_PCLKD_HZ > BSP_MIDDLE_SPEED_MAX_FREQUENCY)||
        (BSP_FCLK_HZ  > BSP_MIDDLE_SPEED_MAX_FREQUENCY) )
    {
        /* set to high-speed mode */
        SYSTEM.OPCCR.BYTE = 0x00;

        /* WAIT_LOOP */
        while(1 == SYSTEM.OPCCR.BIT.OPCMTSF)
        {
            /* Wait for transition to finish. */
            R_BSP_NOP();
        }
    }

    /* At this time the MCU is still running on the 4MHz LOCO. */

#if BSP_CFG_CLOCK_SOURCE == 1
    /* HOCO is chosen. Start it operating if it is not already operating. */
    if (1 == SYSTEM.HOCOCR.BIT.HCSTP)
    {
        /* WAIT_LOOP */
        while(1 == SYSTEM.OSCOVFSR.BIT.HCOVF)
        {
            /* The delay period needed is to make sure that the HOCO has stopped. */
            R_BSP_NOP();
        }

        /* HOCO is chosen. Start it operating. */
        SYSTEM.HOCOCR.BYTE = 0x00;

        /* Dummy read and compare. cf."5. I/O Registers", "(2) Notes on writing to I/O registers" in User's manual.
           This is done to ensure that the register has been written before the next register access. The RX has a 
           pipeline architecture so the next instruction could be executed before the previous write had finished.
         */
        if(0x00 ==  SYSTEM.HOCOCR.BYTE)
        {
            R_BSP_NOP();
        }

        /* WAIT_LOOP */
        while(0 == SYSTEM.OSCOVFSR.BIT.HCOVF)
        {
            /* The delay period needed is to make sure that the HOCO has stabilized. */
            R_BSP_NOP();
        }
    }
#endif /* BSP_CFG_CLOCK_SOURCE == 1 */

#if (BSP_CFG_CLOCK_SOURCE == 2) || (BSP_CFG_CLOCK_SOURCE == 4)

    /* Set the oscillation source of the main clock oscillator. */
    SYSTEM.MOFCR.BIT.MOSEL = BSP_CFG_MAIN_CLOCK_SOURCE;

    /* Configure the main clock oscillator drive capability. */
    if ((BSP_CFG_MCU_VCC_MV >= 2400) && (BSP_CFG_XTAL_HZ >= 10000000))
    {
        /* VCC >= 2.4v and BSP_CFG_XTAL_HZ >= 10 MHz */
        SYSTEM.MOFCR.BIT.MODRV21 = 1;
    }
    else
    {
        /* VCC < 2.4v  -or-  BSP_CFG_XTAL_HZ < 10 MHz */
        /* RSSKRX23E-A is 3.3V and uses 8MHz external oscillator */
        SYSTEM.MOFCR.BIT.MODRV21 = 0;
    }

    /* Set the oscillation stabilization wait time of the main clock oscillator. */
#if BSP_CFG_MAIN_CLOCK_SOURCE == 0 /* Resonator */
    SYSTEM.MOSCWTCR.BYTE = BSP_CFG_MOSC_WAIT_TIME;
#elif BSP_CFG_MAIN_CLOCK_SOURCE == 1 /* External oscillator input */
    SYSTEM.MOSCWTCR.BYTE = 0x00;
#else
    #error "Error! Invalid setting for BSP_CFG_MAIN_CLOCK_SOURCE in r_bsp_config.h"
#endif

    /* Set the main clock to operating. */
    SYSTEM.MOSCCR.BYTE = 0x00;

    /* WAIT_LOOP */
    while(0 == SYSTEM.OSCOVFSR.BIT.MOOVF)
    {
        /* The delay period needed is to make sure that the Main clock has stabilized. */
        R_BSP_NOP();
    }

#endif /* (BSP_CFG_CLOCK_SOURCE == 2) || (BSP_CFG_CLOCK_SOURCE == 4) */

#if BSP_CFG_CLOCK_SOURCE == 4

    /* Set PLL Input Divisor. */
    SYSTEM.PLLCR.BIT.PLIDIV = BSP_CFG_PLL_DIV >> 1;

    /* Set PLL Multiplier. */
    SYSTEM.PLLCR.BIT.STC = ((uint8_t)((float)BSP_CFG_PLL_MUL * 2.0)) - 1;

    /* Set the PLL to operating. */
    SYSTEM.PLLCR2.BYTE = 0x00;

    /* WAIT_LOOP */
    while (1 != SYSTEM.OSCOVFSR.BIT.PLOVF)
    {
        /* wait for stabilization */
        R_BSP_NOP();
    }
#endif

    /* LOCO is saved for last since it is what is running by default out of reset. This means you do not want to turn
       it off until another clock has been enabled and is ready to use. */
#if BSP_CFG_CLOCK_SOURCE == 0
    /* LOCO is chosen. This is the default out of reset. */
#else
    /* LOCO is not chosen but it cannot be turned off yet since it is still being used. */
#endif

} /* End of function clock_source_select() */

/***********************************************************************************************************************
* Function name: lpt_clock_source_select
* Description  : Enables clock sources for the lpt (if not already done) as chosen by the user.
*                This function also implements the software delays needed for the clocks to stabilize.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void lpt_clock_source_select (void)
{
    /* Protect off. DO NOT USE R_BSP_RegisterProtectDisable()! (not initialized yet) */
    SYSTEM.PRCR.WORD = 0xA501;

    /* INITIALIZE AND SELECT LPT CLOCK SOURCE */

#if (BSP_CFG_LPT_CLOCK_SOURCE == 1)

    /* IWDTCLK is chosen. Start it operating. */
    SYSTEM.ILOCOCR.BYTE = 0x00;

    /* Wait processing for the IWDT clock oscillation stabilization (50us) */
    R_BSP_SoftwareDelay((uint32_t)50, BSP_DELAY_MICROSECS);

    /* Controls whether to stop the IWDT counter in a low power consumption state.
    IWDTCSTPR - IWDT Count Stop Control Register
    b7     SLCSTP   - Sleep Mode Count Stop Control - Count stop is disabled.
    b6:b1  Reserved - These bits are read as 0. Writing to these bits has no effect. */
    IWDT.IWDTCSTPR.BIT.SLCSTP = 0;
#else
    /* LPT is not used. */
#endif

    /* Enable protect bit */
    SYSTEM.PRCR.WORD = 0xA500;
    return;
} /* End of function usb_lpc_clock_source_select() */

#endif /* BSP_CFG_STARTUP_DISABLE == 0 */

