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
* Copyright (C) 2013-2017 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_cmt_rx.c
* Description  : This module creates timer ticks using CMT channels or one-shot events based on period in uS. 
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 06.11.2013 2.10    First GSCE Release.
*         : 11.03.2014 2.20    Added support for RX110.
*                              Fixes bug in R_CMT_Control CMT_RX_CMD_GET_NUM_CHANNELS command.
*         : 22.04.2014 2.30    Added support for RX64M.
*         : 10.11.2014 2.40    Added support for RX113.
*         : 20.02.2015 2.50    Added support for RX71M.
*         : 01.04.2015 2.60    Added support for RX231.
*         : 30.09.2015 2.70    Added support for RX23T.
*         : 01.10.2015 2.80    Added support for RX130.
*         : 01.12.2015 2.90    Added support for RX230, RX24T.
*         : 15.03.2016 3.00    Added support for RX65N.
*         : 19.12.2016 3.10    Added support for RX24U, RX24T(512KB).
*                              Fix to stop timer before callback function is executed.
*         : 21.07.2017 3.20    Added support for RX65N-2M, RX130-512KB
*         : xx.xx.xxxx x.xx    Added support for GNUC and ICCRX.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "platform.h"
#include "r_cmt_rx_config.h" /* Configuration header file */
#include "r_cmt_rx_if.h"     /*  Interface header file */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Define the number of CMT channels based on MCU type. */
#if defined(BSP_MCU_RX62_ALL) || defined(BSP_MCU_RX63_ALL) || defined(BSP_MCU_RX21_ALL) || \
    defined(BSP_MCU_RX61_ALL) || defined(BSP_MCU_RX64_ALL) || defined(BSP_MCU_RX113)    || \
    defined(BSP_MCU_RX71_ALL) || defined(BSP_MCU_RX231)    || defined(BSP_MCU_RX23_ALL) || \
    defined(BSP_MCU_RX24_ALL) || defined(BSP_MCU_RX65_ALL)
    #define CMT_RX_NUM_CHANNELS        (4)
#elif defined(BSP_MCU_RX111)  || defined(BSP_MCU_RX110)    || defined(BSP_MCU_RX130)
    #define CMT_RX_NUM_CHANNELS        (2)
#else
    #error "Error! Number of channels for this MCU is not defined in r_cmt_rx.c"
#endif

/* Max number of ticks for counter without overflowing/underflowing. */
#define CMT_RX_MAX_TIMER_TICKS         (0x10000)

/* Starting with RX63x MCUs, there are 2 peripheral clocks: PCLKA and PCLKB. PCLKA is only used by the Ethernet block.
   This means that PCLKB would match functionality of PCLK in RX62x devices as far as the CMT is concerned. PCLKB is 
   used for the CMT on RX210 MCUs too. */
#if defined(BSP_MCU_RX63_ALL) || defined(BSP_MCU_RX21_ALL) || defined(BSP_MCU_RX11_ALL) || defined(BSP_MCU_RX64_ALL) || \
    defined(BSP_MCU_RX71_ALL) || defined(BSP_MCU_RX113)    || defined(BSP_MCU_RX231)    || defined(BSP_MCU_RX23_ALL) || \
    defined(BSP_MCU_RX13_ALL) || defined(BSP_MCU_RX24_ALL) || defined(BSP_MCU_RX65_ALL)
    #define CMT_PCLK_HZ                 (BSP_PCLKB_HZ)
#else
    #define CMT_PCLK_HZ                 (BSP_PCLK_HZ)
#endif

/* Which MCUs have register protection. */
#if defined(BSP_MCU_RX21_ALL) || defined(BSP_MCU_RX63_ALL) || defined(BSP_MCU_RX11_ALL) || defined(BSP_MCU_RX64_ALL) || \
    defined(BSP_MCU_RX71_ALL) || defined(BSP_MCU_RX231)    || defined(BSP_MCU_RX23_ALL) || defined(BSP_MCU_RX13_ALL) || \
    defined(BSP_MCU_RX24_ALL) || defined(BSP_MCU_RX65_ALL)
    #define CMT_REG_PROTECT             (1)
#else
    #define CMT_REG_PROTECT             (0)
#endif


/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* Enum for current mode of CMT channel. */
typedef enum
{
    CMT_RX_MODE_DISABLED = 0,
    CMT_RX_MODE_PERIODIC,
    CMT_RX_MODE_ONE_SHOT
} cmt_modes_t;

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* Used to prevent having duplicate code for each channel. This only works if the channels are identical (just at 
   different locations in memory). This is easy to tell by looking in iodefine.h and seeing if the same structure
   was used for all channels. */
static volatile struct st_cmt0 __evenaccess * const g_cmt_channels[CMT_RX_NUM_CHANNELS] =
{
/* Currently all RX MCUs have either 2 or 4 CMT channels. */
#if   CMT_RX_NUM_CHANNELS == 4
    &CMT0, &CMT1, &CMT2, &CMT3
#elif CMT_RX_NUM_CHANNELS == 2
    &CMT0, &CMT1
#else
    #error "Error! r_cmt_rx is not setup for this many CMT channels. Please refer to g_cmt_channels[] in r_cmt_rx.c"
#endif
};

/* Holds current mode of each CMT channel. */
static cmt_modes_t g_cmt_modes[CMT_RX_NUM_CHANNELS];

/* This array holds the available clock dividers. For example, if PCLK/8 is available, then '8' would be an entry.
   Note that the index of the divider in the array should correspond to the bit setting used for the CKS[1:0] bitfield.
   For example, 128 is at index 2 in the array and to choose PCLK/128 in the CMCR register you would set the CKS[1:0] 
   bitfield to 2. */
static const uint32_t g_cmt_clock_dividers[] = { 8, 32, 128, 512 };

/* Array that holds user's callback functions. */
static void  (* g_cmt_callbacks[CMT_RX_NUM_CHANNELS])(void * pdata);

/* Internal functions. */
static bool cmt_lock_state(void);
static void cmt_unlock_state(void);
static bool cmt_find_channel(uint32_t * channel);
static void power_on(uint32_t channel);
static void power_off(uint32_t channel);
static void cmt_counter_start(uint32_t channel);
static void cmt_counter_stop(uint32_t channel);
static bool cmt_setup_channel(uint32_t channel, uint32_t frequency_hz);
static void cmt_isr_common(uint32_t channel);
static bool cmt_create(uint32_t frequency_hz, void (* callback)(void * pdata), cmt_modes_t mode, uint32_t * channel);

/***********************************************************************************************************************
* Function Name: R_CMT_CreatePeriodic
* Description  : Sets up a CMT channel and calls a callback function at a set frequency.
* Arguments    : frequency_hz -
*                    Frequency in Hz of how often to call the callback function.
*                callback -
*                    Which function to call when timer expires. If you wish to use R_CMT_Control() to check the status 
*                    of a channel instead of using a callback then use FIT_NO_FUNC for this parameter.
*                channel -
*                    Pointer of where to store which channel was used.
* Return Value : true - 
*                    Channel initialized successfully.
*                false -
*                    Invalid channel or frequency could not be used.
***********************************************************************************************************************/
bool R_CMT_CreatePeriodic (uint32_t frequency_hz, void (* callback)(void * pdata), uint32_t * channel)
{
    return cmt_create(frequency_hz, callback, CMT_RX_MODE_PERIODIC, channel);
} 

/***********************************************************************************************************************
* Function Name: R_CMT_CreateOneShot
* Description  : Sets up a CMT channel and calls a callback function once after a user-defined amount of time.
* Arguments    : period_us -
*                    How long until compare match occurs. Unit is microseconds.
*                callback -
*                    Which function to call when timer expires. If you wish to use R_CMT_Control() to check the status 
*                    of a channel instead of using a callback then use FIT_NO_FUNC for this parameter.
*                channel -
*                    Pointer of where to store which channel was used.
* Return Value : true - 
*                    Channel initialized successfully.
*                false -
*                    Invalid channel or period could not be used.
***********************************************************************************************************************/
bool R_CMT_CreateOneShot (uint32_t period_us, void (* callback)(void * pdata), uint32_t * channel)
{    
    return cmt_create((1000000/period_us), callback, CMT_RX_MODE_ONE_SHOT, channel);
}

/***********************************************************************************************************************
* Function Name: R_CMT_Stop
* Description  : Stop a counter and puts it in module stop state to conserve power.
* Arguments    : channel - 
*                    Which channel to use.
* Return Value : true - 
*                    Counter stopped.
*                false -
*                    Invalid channel number.
***********************************************************************************************************************/
bool R_CMT_Stop (uint32_t channel)
{
    /* Make sure valid channel number was input. */
#if BSP_CFG_RTOS_USED == 0      //Non-OS
    if (channel >= CMT_RX_NUM_CHANNELS)
#elif BSP_CFG_RTOS_USED == 1        // FreeRTOS
    if ((channel >= CMT_RX_NUM_CHANNELS) || (BSP_CFG_RTOS_SYSTEM_TIMER == channel))
#elif BSP_CFG_RTOS_USED == 2    // SEGGER embOS
#elif BSP_CFG_RTOS_USED == 3    // Micrium MicroC/OS
#elif BSP_CFG_RTOS_USED == 4    // Renesas RI600V4 & RI600PX
#endif
    {
        /* Invalid channel number was used. */
        return false;
    }

    /* Stop counter. */
    cmt_counter_stop(channel);

    /* Turn off power to channel. */
    power_off(channel);

    /* Set mode to disabled. */
    g_cmt_modes[channel] = CMT_RX_MODE_DISABLED;

    /* Set channel as available. */
    R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_CMT0 + channel));

    return true;
} 

/***********************************************************************************************************************
* Function Name: R_CMT_Control
* Description  : Handles minor functions of this module.
* Arguments    : channel - 
*                    Which channel is being referenced. If not channel is needed input CMT_RX_NO_CHANNEL.
*                command -
*                    What command is being input.
*                pdata - 
*                    Pointer to data to be input or filled in if needed.
* Return Value : true - 
*                    Command completed successfully.
*                false -
*                    Invalid command. 
***********************************************************************************************************************/
bool R_CMT_Control (uint32_t channel, cmt_commands_t command, void * pdata)
{
    bool ret = true;

#if BSP_CFG_RTOS_USED == 0      // Non-OS
#elif BSP_CFG_RTOS_USED == 1    // FreeRTOS
    if (CMT_RX_CMD_GET_NUM_CHANNELS == command)
    {
        /* Return the number of CMT channels on this MCU. */
        *(uint32_t *)pdata = CMT_RX_NUM_CHANNELS;
        ret = true;
        return ret;
    }
    else if ((channel == BSP_CFG_RTOS_SYSTEM_TIMER) || (channel >= CMT_RX_NUM_CHANNELS))
    {
        /* Specified CMT channel is being used by RTOS or Invalid channel */
        /* Modifying is NOT allowed for the other commands. */
        ret = false;
        return ret;
    }
#elif BSP_CFG_RTOS_USED == 2    // SEGGER embOS
#elif BSP_CFG_RTOS_USED == 3    // Micrium MicroC/OS
#elif BSP_CFG_RTOS_USED == 4    // Renesas RI600V4 & RI600PX
#endif

    /* Process command that was input. */
    switch (command)
    {
        case CMT_RX_CMD_IS_CHANNEL_COUNTING:

            /* Check input channel. */
            if (channel < CMT_RX_NUM_CHANNELS)
            {
                /* Valid channel input. Is this channel currently counting? */
                if (CMT_RX_MODE_DISABLED == g_cmt_modes[channel])
                {
                    *(bool *)pdata = false;
                }
                else
                {
                    *(bool *)pdata = true;
                }
            }
            else
            {
                ret = false;
            }
        break;
#if BSP_CFG_RTOS_USED == 0   /* Unreachable code because of checks above */
        case CMT_RX_CMD_GET_NUM_CHANNELS:
            *(uint32_t *)pdata = CMT_RX_NUM_CHANNELS;
        break;
#endif
        case CMT_RX_CMD_PAUSE:

            if (channel < CMT_RX_NUM_CHANNELS)
            {
                /* Valid channel input. Is this channel currently counting? */
                if (CMT_RX_MODE_DISABLED != g_cmt_modes[channel])
                {

                    switch (channel)
                    {
                        case 0:
                            CMT.CMSTR0.BIT.STR0 = 0;
                        break;
                        case 1:
                            CMT.CMSTR0.BIT.STR1 = 0;
                        break;

                        #if   CMT_RX_NUM_CHANNELS == 4
                        case 2:
                            CMT.CMSTR1.BIT.STR2 = 0;
                        break;
                        case 3:
                            CMT.CMSTR1.BIT.STR3 = 0;
                        break;
                        #endif

                        default:
                        break;
                    }
                }
            }
            break;


        /* Both restart and resume do the same thing, except restart zeros the counter first */ 
        case CMT_RX_CMD_RESTART:
        case CMT_RX_CMD_RESUME:
            if (channel < CMT_RX_NUM_CHANNELS)
            {
                /* Stop the timer once to prevent an interrupt from occurring */
                switch (channel)
                {
                    case 0:
                        CMT.CMSTR0.BIT.STR0 = 0;
                    break;
                    case 1:
                        CMT.CMSTR0.BIT.STR1 = 0;
                    break;
#if   CMT_RX_NUM_CHANNELS == 4
                    case 2:
                        CMT.CMSTR1.BIT.STR2 = 0;
                    break;
                    case 3:
                        CMT.CMSTR1.BIT.STR3 = 0;
                    break;
#endif
                    default:
                    break;
                }

                /* Valid channel input. Is this channel currently counting? */
                if (CMT_RX_MODE_DISABLED != g_cmt_modes[channel])
                {

                    if (CMT_RX_CMD_RESTART == command)
                    {
                      /* For restarting, clear counter. */
                      (*g_cmt_channels[channel]).CMCNT = 0;
                    }

                    switch (channel)
                    {
                        case 0:
                            CMT.CMSTR0.BIT.STR0 = 1;
                        break;
                        case 1:
                            CMT.CMSTR0.BIT.STR1 = 1;
                        break;
#if   CMT_RX_NUM_CHANNELS == 4
                        case 2:
                            CMT.CMSTR1.BIT.STR2 = 1;
                        break;
                        case 3:
                            CMT.CMSTR1.BIT.STR3 = 1;
                        break;
#endif
                        default:
                        break;
                    }
                }
            }
            break;

        default:
            ret = false;
        break;
    }

    return ret;
} 

/***********************************************************************************************************************
* Function Name: cmt_create
* Description  : Sets up a CMT channel based on user input options.
* Arguments    : frequency_hz -
*                    Frequency in Hz of how often to call the callback function.
*                callback -
*                    Which function to call when timer expires.
*                mode -
*                    Current mode of CMT channel
*                channel -
*                    Pointer of where to store which channel was used.
* Return Value : true - 
*                    Channel initialized successfully.
*                false -
*                    Invalid channel or period could not be used.
***********************************************************************************************************************/
static bool cmt_create (uint32_t frequency_hz, void (* callback)(void * pdata), cmt_modes_t mode, uint32_t * channel)
{
    /* Return value. */
    bool     ret = false;    

    /* Grab state to make sure we do not interfere with another operation. */
    if (cmt_lock_state() != true)
    {
        /* Another operation is already in progress. */
        return false;
    }

    /* Was a channel found? */
    if (false == cmt_find_channel(channel))
    {
        /* Channel not found or not available. */
        ret = false;
    }
    else
    {
        /* Look to see if we can support this frequency. */
        ret = cmt_setup_channel(*channel, frequency_hz);
    
        /* Set up ICU registers, save callback function, and start counter if valid divider was found. */
        if (true == ret)
        {        
            /* Set mode of operation. */
            g_cmt_modes[*channel] = mode;
    
            /* Save callback function to be used. */
            g_cmt_callbacks[*channel] = callback;
    
            /* Start channel counting. */
            cmt_counter_start(*channel);
        }
        else
        {
            /* Could not support input frequency. Release lock on channel. */
            R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_CMT0 + (*channel)));
        }
    }

    /* Release state so other operations can be performed. */
    cmt_unlock_state();

    return ret;
}

/***********************************************************************************************************************
* Function Name: cmt_lock_state
* Description  : Attempt to lock the state so that we right to perform an operation.
* Arguments    : none
* Return Value : true - 
*                    State was obtained
*                false - 
*                    State was not obtained because code is busy with another on-going operation.
***********************************************************************************************************************/
static bool cmt_lock_state (void)
{
    bool ret;

    /* Attempt to acquire lock. */
    ret = R_BSP_HardwareLock(BSP_LOCK_CMT);

    return ret;
}

/***********************************************************************************************************************
* Function Name: cmt_unlock_state
* Description  : Release state so that other operations can be performed.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
static void cmt_unlock_state (void)
{
    /* Release lock. */
    R_BSP_HardwareUnlock(BSP_LOCK_CMT);
}

/***********************************************************************************************************************
* Function Name: cmt_find_channel
* Description  : Attempt to find open CMT channel to use.
* Arguments    : channel - 
*                    Channel number if channel was found.
* Return Value : true -
*                    Channel was found and locked.
*                false -
*                    No channels available.
***********************************************************************************************************************/
static bool cmt_find_channel (uint32_t * channel)
{
    bool     channel_found = false;
    uint32_t i;

    /* Look for an available channel. */
    for (i = 0; i < CMT_RX_NUM_CHANNELS; i++)
    {
#if BSP_CFG_RTOS_USED == 0      // Non-OS
#elif BSP_CFG_RTOS_USED == 1    // FreeRTOS
        if (i == BSP_CFG_RTOS_SYSTEM_TIMER)
        {
            /* Found CMT channel is being used for RTOS. */
            continue;
        }
#elif BSP_CFG_RTOS_USED == 2    // SEGGER embOS
#elif BSP_CFG_RTOS_USED == 3    // Micrium MicroC/OS
#elif BSP_CFG_RTOS_USED == 4    // Renesas RI600V4 & RI600PX
#endif
        if (true == R_BSP_HardwareLock((mcu_lock_t)(BSP_LOCK_CMT0 + i)))
        {
            /* Channel found. */
            *channel = i;
            channel_found = true;
            
            break;            
        }
    }        

    return channel_found;
}

/***********************************************************************************************************************
* Function Name: power_on
* Description  : Enables CMT channel.
* Arguments    : channel - 
*                    Channel number to enable.
* Return Value : none
***********************************************************************************************************************/
static void power_on (uint32_t channel)
{
#if CMT_REG_PROTECT == 1
    /* Enable writing to MSTP registers. */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
#endif

    /* Enable selected CMT Channel. Go ahead and make sure counter is stopped too. */
    switch (channel)
    {
        case 0:

            /* Bring module out of stop state. */
            MSTP(CMT0) = 0;

            /* Stop counter. */
            CMT.CMSTR0.BIT.STR0 = 0;
        break;   
        case 1:

            /* Bring module out of stop state. */
            MSTP(CMT1) = 0;

            /* Stop counter. */
            CMT.CMSTR0.BIT.STR1 = 0;        
        break;
#if   CMT_RX_NUM_CHANNELS == 4
        case 2:

            /* Bring module out of stop state. */
            MSTP(CMT2) = 0;

            /* Stop counter. */
            CMT.CMSTR1.BIT.STR2 = 0;        
        break;
        case 3:

            /* Bring module out of stop state. */
            MSTP(CMT3) = 0;

            /* Stop counter. */
            CMT.CMSTR1.BIT.STR3 = 0;       
        break;
#endif
        default:

            /* Should never get here. Valid channel number is checked above. */
        break;
    }    

#if CMT_REG_PROTECT == 1

    /* Disable writing to MSTP registers. */
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);
#endif
}

/***********************************************************************************************************************
* Function Name: power_off
* Description  : Disables CMT channel.
* Arguments    : channel - 
*                    Channel number to disable.
* Return Value : none
***********************************************************************************************************************/
static void power_off (uint32_t channel)
{
#if CMT_REG_PROTECT == 1

    /* Enable writing to MSTP registers. */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
#endif

    /* Enable selected CMT Channel. Go ahead and make sure counter is stopped too. Since each MSTP bit controls
       2 channels, make sure to only disable CMT unit when both channels are off. */
    switch (channel)
    {
        case 0:

            /* Stop counter. */
            CMT.CMSTR0.BIT.STR0 = 0;

            /* Only turn off CMT unit if both channels are off. */
            if (g_cmt_modes[1] == CMT_RX_MODE_DISABLED)
            {
                /* Put module in stop state. */
                MSTP(CMT0) = 1;
            }
        break;   
        case 1:

            /* Stop counter. */
            CMT.CMSTR0.BIT.STR1 = 0;        

            /* Only turn off CMT unit if both channels are off. */
            if (g_cmt_modes[0] == CMT_RX_MODE_DISABLED)
            {
                /* Put module in stop state. */
                MSTP(CMT1) = 1;
            }
        break;
#if   CMT_RX_NUM_CHANNELS == 4
        case 2:
        
            /* Stop counter. */
            CMT.CMSTR1.BIT.STR2 = 0;        

            /* Only turn off CMT unit if both channels are off. */
            if (g_cmt_modes[3] == CMT_RX_MODE_DISABLED)
            {
                /* Put module in stop state. */
                MSTP(CMT2) = 1;
            }
        break;
        case 3:

            /* Stop counter. */
            CMT.CMSTR1.BIT.STR3 = 0;       

            /* Only turn off CMT unit if both channels are off. */
            if (g_cmt_modes[2] == CMT_RX_MODE_DISABLED)
            {

                /* Put module in stop state. */
                MSTP(CMT3) = 1;
            }
        break;
#endif
        default:

            /* Should never get here. Valid channel number is checked above. */
        break;
    }    

#if CMT_REG_PROTECT == 1
    /* Disable writing to MSTP registers. */
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);
#endif
}

/***********************************************************************************************************************
* Function Name: cmt_counter_start
* Description  : Starts a CMT channel counting.
* Arguments    : channel - 
*                    Channel number to use.
* Return Value : none
***********************************************************************************************************************/
static void cmt_counter_start (uint32_t channel)
{
    /* Enable compare match interurpt. */
    (*g_cmt_channels[channel]).CMCR.BIT.CMIE = 1;

    /* Clear counter. */
    (*g_cmt_channels[channel]).CMCNT = 0;

    /* Start counter channel. */
    switch (channel)
    {
        case 0:

            /* Setup ICU registers. */
            IR(CMT0, CMI0)  = 0;                /* Clear any previously pending interrupts */
            IPR(CMT0, CMI0) = CMT_RX_CFG_IPR;   /* Set interrupt priority */
            IEN(CMT0, CMI0) = 1;                /* Enable compare match interrupt */

            /* Start counter. */
            CMT.CMSTR0.BIT.STR0 = 1;
        break;   
        case 1:

            /* Setup ICU registers. */
            IR(CMT1, CMI1)  = 0;                /* Clear any previously pending interrupts */
            IPR(CMT1, CMI1) = CMT_RX_CFG_IPR;   /* Set interrupt priority */
            IEN(CMT1, CMI1) = 1;                /* Enable compare match interrupt */

            /* Start counter. */
            CMT.CMSTR0.BIT.STR1 = 1;        
        break;
#if   CMT_RX_NUM_CHANNELS == 4
        case 2:

            /* Setup ICU registers. */
            IR(CMT2, CMI2)  = 0;                /* Clear any previously pending interrupts */
            IPR(CMT2, CMI2) = CMT_RX_CFG_IPR;   /* Set interrupt priority */
            IEN(CMT2, CMI2) = 1;                /* Enable compare match interrupt */

            /* Start counter. */
            CMT.CMSTR1.BIT.STR2 = 1;        
        break;
        case 3:

            /* Setup ICU registers. */
            IR(CMT3, CMI3)  = 0;                /* Clear any previously pending interrupts */
            IPR(CMT3, CMI3) = CMT_RX_CFG_IPR;   /* Set interrupt priority */
            IEN(CMT3, CMI3) = 1;                /* Enable compare match interrupt */

            /* Start counter. */
            CMT.CMSTR1.BIT.STR3 = 1;       
        break;
#endif
        default:

            /* Should never get here. Valid channel number is checked above. */
        break;
    }    
}

/***********************************************************************************************************************
* Function Name: cmt_counter_stop
* Description  : Stops a CMT channel counting.
* Arguments    : channel - 
*                    Channel number to use.
* Return Value : none
***********************************************************************************************************************/
static void cmt_counter_stop (uint32_t channel)
{
    /* Stop counter channel. */
    switch (channel)
    {
        case 0:

            /* Stop counter. */
            CMT.CMSTR0.BIT.STR0 = 0;

            /* Set ICU registers. */
            IEN(CMT0, CMI0) = 0;            /* Disable compare match interrupt */
            IPR(CMT0, CMI0) = 0;            /* Set interrupt priority */
            IR(CMT0, CMI0)  = 0;            /* Clear any previously pending interrupts */
        break;   
        case 1:

            /* Stop counter. */
            CMT.CMSTR0.BIT.STR1 = 0;        

            /* Set ICU registers. */
            IEN(CMT1, CMI1) = 0;            /* Disable compare match interrupt */
            IPR(CMT1, CMI1) = 0;            /* Set interrupt priority */
            IR(CMT1, CMI1)  = 0;            /* Clear any previously pending interrupts */
        break;
#if   CMT_RX_NUM_CHANNELS == 4
        case 2:

            /* Stop counter. */
            CMT.CMSTR1.BIT.STR2 = 0;        

            /* Set ICU registers. */
            IEN(CMT2, CMI2) = 0;            /* Disable compare match interrupt */
            IPR(CMT2, CMI2) = 0;            /* Set interrupt priority */
            IR(CMT2, CMI2)  = 0;            /* Clear any previously pending interrupts */
        break;
        case 3:

            /* Stop counter. */
            CMT.CMSTR1.BIT.STR3 = 0;       

            /* Set ICU registers. */
            IEN(CMT3, CMI3) = 0;            /* Disable compare match interrupt */
            IPR(CMT3, CMI3) = 0;            /* Set interrupt priority */
            IR(CMT3, CMI3)  = 0;            /* Clear any previously pending interrupts */

        break;
#endif
        default:

            /* Should never get here. Valid channel number is checked above. */
        break;
    }    
}

/***********************************************************************************************************************
* Function Name: cmt_setup_channel
* Description  : Sets dividers and counter so compare match occurs at input frequency.
* Arguments    : channel - 
*                    Channel number to use.
*                frequency_hz -
*                    Frequency at which to set CMT channel
* Return Value : true -
*                    Channel setup completed.
*                false -
*                    Channel setup failed.
***********************************************************************************************************************/
static bool cmt_setup_channel (uint32_t channel, uint32_t frequency_hz)
{
    uint32_t i;
    bool     ret = false;


    if (((uint32_t)CMT_PCLK_HZ / 8) >= frequency_hz) /* Requested frequency must not be higher than PCLK. */
    {
        /* Choose clock. This is done by looking through the available dividers to see if we can match the frequency
           input by the user. */
        for (i = 0; i < ((sizeof(g_cmt_clock_dividers))/(sizeof(g_cmt_clock_dividers[0]))); i++)
        {
            /* Determine minimum frequency this divider can hit. For example, if a PCLK/8 is used and PCLK is 48MHz, then
               the minimum frequency we can support is around 91.5Hz. This obtained by doing the following calculation:
               (PCLK / divider) / max_counter_value
               Example:
               (48,000,000 / 8) / 65,536 = 91.5 */
            if (frequency_hz > (((uint32_t)CMT_PCLK_HZ / g_cmt_clock_dividers[i]) / (uint32_t)CMT_RX_MAX_TIMER_TICKS))
            {
                /* Enable peripheral channel. */
                power_on(channel);

                /* We can use this divider. Figure out counter ticks needed for this frequency. */
                (*g_cmt_channels[channel]).CMCOR = (uint16_t)((((uint32_t)CMT_PCLK_HZ /g_cmt_clock_dividers[i])/frequency_hz) -1 );


                /* Set clock divider to be used. */
                (*g_cmt_channels[channel]).CMCR.BIT.CKS = i;

                /* Save that we did find a valid divider. */
                ret = true;

                /* No need to check other dividers. */
                break;
            }
        }
    }

    return ret;
}

/***********************************************************************************************************************
* Function Name: R_CMT_GetVersion
* Description  : Returns the current version of this module. The version number is encoded where the top 2 bytes are the
*                major version number and the bottom 2 bytes are the minor version number. For example, Version 4.25 
*                would be returned as 0x00040019.
* Arguments    : none
* Return Value : Version of this module.
***********************************************************************************************************************/
R_PRAGMA_INLINE(R_CMT_GetVersion)
uint32_t R_CMT_GetVersion (void)
{
    /* These version macros are defined in r_cmt_rx_if.h. */
    return ((((uint32_t)CMT_RX_VERSION_MAJOR) << 16) | (uint32_t)CMT_RX_VERSION_MINOR);
} 

/***********************************************************************************************************************
* Function Name: cmt_isr_common
* Description  : Common part of ISR handling. Put here and made inline so changes will always apply to all channels.
* Arguments    : channel -
*                    Which channel this is for.
* Return Value : none
***********************************************************************************************************************/
R_PRAGMA_STATIC_INLINE(cmt_isr_common)
void cmt_isr_common (uint32_t channel)
{
    /* If this is one-shot mode then stop timer. */
    if (CMT_RX_MODE_ONE_SHOT == g_cmt_modes[channel])
    {
        R_CMT_Stop(channel);
    }

    /* Check for valid callback pointer before calling it. */
    if ((NULL != g_cmt_callbacks[channel]) && ((uint32_t)FIT_NO_FUNC != (uint32_t)g_cmt_callbacks[channel]))
    {
        /* Valid callback found, jump to it. */
        g_cmt_callbacks[channel]((void *)&channel);
    }
}

/***********************************************************************************************************************
* Function Name: cmt0_isr
* Description  : Interrupt when compare match for this channel occurs.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#if BSP_CFG_RTOS_USED == 0      // Non-OS
R_PRAGMA_STATIC_INTERRUPT(cmt0_isr, VECT(CMT0, CMI0))
R_ATTRIB_STATIC_INTERRUPT void cmt0_isr (void)
{
    cmt_isr_common(0);
}
#elif BSP_CFG_RTOS_USED == 1    // FreeRTOS
    #if (BSP_CFG_RTOS_SYSTEM_TIMER != 0)
        R_PRAGMA_STATIC_INTERRUPT(cmt0_isr, VECT(CMT0, CMI0))
        static void cmt0_isr (void)
        {
            cmt_isr_common(0);
        }
    #endif /* (BSP_CFG_RTOS_SYSTEM_TIMER != 0) */
#elif BSP_CFG_RTOS_USED == 2    // SEGGER embOS
#elif BSP_CFG_RTOS_USED == 3    // Micrium MicroC/OS
#elif BSP_CFG_RTOS_USED == 4    // Renesas RI600V4 & RI600PX
#endif

/***********************************************************************************************************************
* Function Name: cmt1_isr
* Description  : Interrupt when compare match for this channel occurs.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#if BSP_CFG_RTOS_USED == 0      // Non-OS
R_PRAGMA_STATIC_INTERRUPT(cmt1_isr, VECT(CMT1, CMI1))
R_ATTRIB_STATIC_INTERRUPT void cmt1_isr (void)
{
    cmt_isr_common(1);
}
#elif BSP_CFG_RTOS_USED == 1    // FreeRTOS
    #if (BSP_CFG_RTOS_SYSTEM_TIMER != 1)
        R_PRAGMA_STATIC_INTERRUPT(cmt1_isr, VECT(CMT1, CMI1))
        static void cmt1_isr (void)
        {
            cmt_isr_common(1);
        }
    #endif /* (BSP_CFG_RTOS_SYSTEM_TIMER != 1) */
#elif BSP_CFG_RTOS_USED == 2    // SEGGER embOS
#elif BSP_CFG_RTOS_USED == 3    // Micrium MicroC/OS
#elif BSP_CFG_RTOS_USED == 4    // Renesas RI600V4 & RI600PX
#endif

#if   CMT_RX_NUM_CHANNELS == 4
/***********************************************************************************************************************
* Function Name: cmt2_isr
* Description  : Interrupt when compare match for this channel occurs.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#if BSP_CFG_RTOS_USED == 0      // Non-OS
R_PRAGMA_STATIC_INTERRUPT(cmt2_isr, VECT(CMT2, CMI2))
R_ATTRIB_STATIC_INTERRUPT void cmt2_isr (void)
{
    cmt_isr_common(2);
}
#elif BSP_CFG_RTOS_USED == 1    // FreeRTOS
    #if (BSP_CFG_RTOS_SYSTEM_TIMER != 2)
        R_PRAGMA_STATIC_INTERRUPT(cmt2_isr, VECT(CMT2, CMI2))
        static void cmt2_isr (void)
        {
            cmt_isr_common(2);
        }
    #endif /* (BSP_CFG_RTOS_SYSTEM_TIMER != 2) */
#elif BSP_CFG_RTOS_USED == 2    // SEGGER embOS
#elif BSP_CFG_RTOS_USED == 3    // Micrium MicroC/OS
#elif BSP_CFG_RTOS_USED == 4    // Renesas RI600V4 & RI600PX
#endif

/***********************************************************************************************************************
* Function Name: cmt3_isr
* Description  : Interrupt when compare match for this channel occurs.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#if BSP_CFG_RTOS_USED == 0      // Non-OS
R_PRAGMA_STATIC_INTERRUPT(cmt3_isr, VECT(CMT3, CMI3))
R_ATTRIB_STATIC_INTERRUPT void cmt3_isr (void)
{
    cmt_isr_common(3);
}
#elif BSP_CFG_RTOS_USED == 1    // FreeRTOS
    #if (BSP_CFG_RTOS_SYSTEM_TIMER != 3)
        R_PRAGMA_STATIC_INTERRUPT(cmt3_isr, VECT(CMT3, CMI3))
        static void cmt3_isr (void)
        {
            cmt_isr_common(3);
        }
    #endif /* (BSP_CFG_RTOS_SYSTEM_TIMER != 3) */
#elif BSP_CFG_RTOS_USED == 2    // SEGGER embOS
#elif BSP_CFG_RTOS_USED == 3    // Micrium MicroC/OS
#elif BSP_CFG_RTOS_USED == 4    // Renesas RI600V4 & RI600PX
#endif

#endif

