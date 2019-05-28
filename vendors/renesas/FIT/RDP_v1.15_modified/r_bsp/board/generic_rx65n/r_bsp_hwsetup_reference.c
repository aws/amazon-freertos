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
* Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : hwsetup.c
* Device(s)    : RX
* H/W Platform : GENERIC_RX65N
* Description  : Defines the initialization routines used each time the MCU is restarted.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version   Description
*         : 15.05.2017 1.00      First Release
*         : 01.12.2017 1.01      Added EBMAPCR register setting.
*         : 01.07.2018 1.02      Added the initialization function for Smart Configurator.
*         : 27.07.2018 1.03      Modified the comment of bsp_adc_initial_configure.
*         : xx.xx.xxxx 1.04      Added support for GNUC and ICCRX.
***********************************************************************************************************************/


/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* I/O Register and board definitions */
#include "platform.h"
#if BSP_CFG_CONFIGURATOR_SELECT == 1
#include "r_cg_macrodriver.h"
#endif

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
R_PRAGMA_UNPACK
typedef struct bsp_bsc {
    union {
        uint32_t u_long;
        R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_13(
            uint32_t prerr:1,
            uint32_t :1,
            uint32_t rpstop:1,
            uint32_t :10,
            uint32_t pr5sel:3,
            uint32_t :1,
            uint32_t pr4sel:3,
            uint32_t :1,
            uint32_t pr3sel:3,
            uint32_t :1,
            uint32_t pr2sel:3,
            uint32_t :1,
            uint32_t pr1sel:3
        ) bit;
    } ebmapcr;
} st_bsp_bsc_t;
R_PRAGMA_PACKOPTION

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* MCU I/O port configuration function declaration */
static void output_ports_configure(void);

/* Interrupt configuration function declaration */
static void interrupts_configure(void);

/* MCU peripheral module configuration function declaration */
static void peripheral_modules_enable(void);

/* ADC initial configuration function declaration */
static void bsp_adc_initial_configure(void);

#if defined(BSP_MCU_RX65N_2MB)
/* BUS initial configuration function declaration */
static void bsp_bsc_initial_configure(void);
#endif/* BSP_MCU_RX65N_2MB */

/***********************************************************************************************************************
* Function name: hardware_setup
* Description  : Contains setup functions called at device restart
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
void hardware_setup(void)
{
    output_ports_configure();
    interrupts_configure();
    peripheral_modules_enable();
    bsp_non_existent_port_init();
    bsp_adc_initial_configure();
#if defined(BSP_MCU_RX65N_2MB)
    bsp_bsc_initial_configure();
#endif/* BSP_MCU_RX65N_2MB */
}

/***********************************************************************************************************************
* Function name: output_ports_configure
* Description  : Configures the port and pin direction settings, and sets the pin outputs to a safe level.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void output_ports_configure(void)
{
    /* Add code here to setup additional output ports */
}

/***********************************************************************************************************************
* Function name: interrupts_configure
* Description  : Configures interrupts used
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void interrupts_configure(void)
{
    /* Add code here to setup additional interrupts */
}

/***********************************************************************************************************************
* Function name: peripheral_modules_enable
* Description  : Enables and configures peripheral devices on the MCU
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void peripheral_modules_enable(void)
{
    /* Add code here to enable peripherals used by the application */
#if BSP_CFG_CONFIGURATOR_SELECT == 1
    /* The initialization function for Smart Configurator */
    R_Systeminit();
#endif
}

/***********************************************************************************************************************
* Function name: bsp_adc_initial_configure
* Description  : Configures the ADC initial settings
* Arguments    : none
* Return value : none
* Note         : This function should be called when the PSW.I bit is 0 (interrupt disabled).
*                This function should be called when the ADCSR.ADST bit is 0.
***********************************************************************************************************************/
static void bsp_adc_initial_configure(void)
{
    unsigned long tmp_mstp; /* The unsigned long is for GNURX's -Wconversion because S12AD1 is as unsigned long:1. */

    /* Protect off. */
    SYSTEM.PRCR.WORD = 0xA502;

    /* Store the value of the MSTPCR. */
    tmp_mstp = MSTP(S12AD1);

    /* Release from the module-stop state */
    MSTP(S12AD1) = 0;

    /* Writing to the A/D conversion time setting register is enabled. */
    S12AD1.ADSAMPR.BYTE = 0x03;

    /* Sets conversion time for middle-speed for S12AD unit 1. */
    S12AD1.ADSAM.WORD = 0x0020;

    /* Writing to the A/D conversion time setting register is disabled. */
    S12AD1.ADSAMPR.BYTE = 0x02;

    /* Restore the value of the MSTPCR. */
    MSTP(S12AD1) = tmp_mstp;

    /* Protect on. */
    SYSTEM.PRCR.WORD = 0xA500;
}

#if defined(BSP_MCU_RX65N_2MB)
/***********************************************************************************************************************
* Function name: bsp_bsc_initial_configure
* Description  : Configures the BUS initial settings
* Arguments    : none
* Return value : none
* Note         : The EBMAPCR register is only available for products with at least 1.5 Mbytes of code flash memory.
***********************************************************************************************************************/
static void bsp_bsc_initial_configure(void)
{
    st_bsp_bsc_t bsp_bsc;

    /* Setting priority when bus right request contention occurs. */
    bsp_bsc.ebmapcr.u_long     = BSC.EBMAPCR.LONG;
    bsp_bsc.ebmapcr.bit.pr1sel = BSP_CFG_EBMAPCR_1ST_PRIORITY;
    bsp_bsc.ebmapcr.bit.pr2sel = BSP_CFG_EBMAPCR_2ND_PRIORITY;
    bsp_bsc.ebmapcr.bit.pr3sel = BSP_CFG_EBMAPCR_3RD_PRIORITY;
    bsp_bsc.ebmapcr.bit.pr4sel = BSP_CFG_EBMAPCR_4TH_PRIORITY;
    bsp_bsc.ebmapcr.bit.pr5sel = BSP_CFG_EBMAPCR_5TH_PRIORITY;

    /* Set to EBMAPCR register */
    BSC.EBMAPCR.LONG = bsp_bsc.ebmapcr.u_long;
}
#endif/* BSP_MCU_RX65N_2MB */
