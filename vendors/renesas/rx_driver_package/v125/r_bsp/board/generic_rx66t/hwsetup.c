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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : hwsetup.c
* Device(s)    : RX
* H/W Platform : GENERIC_RX66T
* Description  : Defines the initialization routines used each time the MCU is restarted.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version   Description
*         : 27.07.2018 1.00      First Release
*         : 31.08.2018 1.01      Corrected execution order for bsp_volsr_initial_configure function.
*         : 28.02.2019 1.02      Fixed coding style.
*                                Added the following function.
*                                - rom_cache_function_set
*                                - rom_cache_noncacheable_area0_set
*                                - rom_cache_noncacheable_area1_set
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
Private global variables and functions
***********************************************************************************************************************/
/* When using the user startup program, disable the following code. */
#if BSP_CFG_STARTUP_DISABLE == 0
/* ROM cache configuration function declaration */
#if BSP_CFG_ROM_CACHE_ENABLE == 1
static void rom_cache_function_set(void);
#if BSP_CFG_NONCACHEABLE_AREA0_ENABLE == 1
static void rom_cache_noncacheable_area0_set(void);
#endif /* BSP_CFG_NONCACHEABLE_AREA0_ENABLE == 1 */
#if BSP_CFG_NONCACHEABLE_AREA1_ENABLE == 1
static void rom_cache_noncacheable_area1_set(void);
#endif /* BSP_CFG_NONCACHEABLE_AREA1_ENABLE == 1 */
#endif /* BSP_CFG_ROM_CACHE_ENABLE == 1 */
#endif /* BSP_CFG_STARTUP_DISABLE == 0 */

/* MCU I/O port configuration function declaration */
static void output_ports_configure(void);

/* Interrupt configuration function declaration */
static void interrupts_configure(void);

/* MCU peripheral module configuration function declaration */
static void peripheral_modules_enable(void);

/* VOLSR register initial configuration function declaration */
static void bsp_volsr_initial_configure(void);


/***********************************************************************************************************************
* Function name: hardware_setup
* Description  : Contains setup functions called at device restart
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
void hardware_setup(void)
{
/* When using the user startup program, disable the following code. */
#if BSP_CFG_STARTUP_DISABLE == 0
#if BSP_CFG_ROM_CACHE_ENABLE == 1
    /* Initialize ROM cache function */
    rom_cache_function_set();
#endif /* BSP_CFG_ROM_CACHE_ENABLE == 1 */
#endif /* BSP_CFG_STARTUP_DISABLE == 0 */

    output_ports_configure();
    interrupts_configure();
    bsp_volsr_initial_configure();
    peripheral_modules_enable();
    bsp_non_existent_port_init();
} /* End of function hardware_setup() */

/* When using the user startup program, disable the following code. */
#if BSP_CFG_STARTUP_DISABLE == 0
#if BSP_CFG_ROM_CACHE_ENABLE == 1
/***********************************************************************************************************************
* Function name: rom_cache_function_set
* Description  : Configures the rom cache function.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void rom_cache_function_set (void)
{
#if BSP_CFG_NONCACHEABLE_AREA0_ENABLE == 1
    rom_cache_noncacheable_area0_set();
#endif /* BSP_CFG_NONCACHEABLE_AREA0_ENABLE == 1 */

#if BSP_CFG_NONCACHEABLE_AREA1_ENABLE == 1
    rom_cache_noncacheable_area1_set();
#endif /* BSP_CFG_NONCACHEABLE_AREA1_ENABLE == 1 */

    /* Invalidates the contents of the ROM cache. */
    FLASH.ROMCIV.WORD = 0x0001;

    /* Enables the ROM cache. */
    FLASH.ROMCE.WORD = 0x0001;
} /* End of function rom_cache_function_set() */

#if BSP_CFG_NONCACHEABLE_AREA0_ENABLE == 1
/***********************************************************************************************************************
* Function name: rom_cache_noncacheable_area0_set
* Description  : Configures non-cacheable area 0 of the ROM cache function.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void rom_cache_noncacheable_area0_set (void)
{
    /* Used to specify the value written to the NCRC0 register. */
    uint32_t tmp_ncrc = 0;

    /* Disables the ROM cache. */
    FLASH.ROMCE.WORD = 0x0000;

    /* Makes settings to the NCRG0 register. */
    #if (BSP_CFG_NONCACHEABLE_AREA0_ADDR >= 0xFFC00000) \
      && ((BSP_CFG_NONCACHEABLE_AREA0_ADDR & 0x0000000F) == 0x00000000)
    FLASH.NCRG0 = BSP_CFG_NONCACHEABLE_AREA0_ADDR;
    #else
        #error "Error! Invalid setting for BSP_CFG_NONCACHEABLE_AREA0_ADDR in r_bsp_config.h"
    #endif

    /* Sets the value of the NCSZ bits. */
#if   BSP_CFG_NONCACHEABLE_AREA0_SIZE == 0x0
    /* Do nothing since NCRC0 bits should be 0. */
#elif BSP_CFG_NONCACHEABLE_AREA0_SIZE == 0x1
    tmp_ncrc |= 0x00000010;
#elif BSP_CFG_NONCACHEABLE_AREA0_SIZE == 0x2
    tmp_ncrc |= 0x00000030;
#elif BSP_CFG_NONCACHEABLE_AREA0_SIZE == 0x3
    tmp_ncrc |= 0x00000070;
#elif BSP_CFG_NONCACHEABLE_AREA0_SIZE == 0x4
    tmp_ncrc |= 0x000000F0;
#elif BSP_CFG_NONCACHEABLE_AREA0_SIZE == 0x5
    tmp_ncrc |= 0x000001F0;
#elif BSP_CFG_NONCACHEABLE_AREA0_SIZE == 0x6
    tmp_ncrc |= 0x000003F0;
#elif BSP_CFG_NONCACHEABLE_AREA0_SIZE == 0x7
    tmp_ncrc |= 0x000007F0;
#elif BSP_CFG_NONCACHEABLE_AREA0_SIZE == 0x8
    tmp_ncrc |= 0x00000FF0;
#elif BSP_CFG_NONCACHEABLE_AREA0_SIZE == 0x9
    tmp_ncrc |= 0x00001FF0;
#elif BSP_CFG_NONCACHEABLE_AREA0_SIZE == 0xA
    tmp_ncrc |= 0x00003FF0;
#elif BSP_CFG_NONCACHEABLE_AREA0_SIZE == 0xB
    tmp_ncrc |= 0x00007FF0;
#elif BSP_CFG_NONCACHEABLE_AREA0_SIZE == 0xC
    tmp_ncrc |= 0x0000FFF0;
#elif BSP_CFG_NONCACHEABLE_AREA0_SIZE == 0xD
    tmp_ncrc |= 0x0001FFF0;
#elif BSP_CFG_NONCACHEABLE_AREA0_SIZE == 0xE
    tmp_ncrc |= 0x0003FFF0;
#elif BSP_CFG_NONCACHEABLE_AREA0_SIZE == 0xF
    tmp_ncrc |= 0x0007FFF0;
#elif BSP_CFG_NONCACHEABLE_AREA0_SIZE == 0x10
    tmp_ncrc |= 0x000FFFF0;
#else
    #error "Error! Invalid setting for BSP_CFG_NONCACHEABLE_AREA0_SIZE in r_bsp_config.h"
#endif

    /* Sets the value of the NC1E bits. */
#if BSP_CFG_NONCACHEABLE_AREA0_IF_ENABLE == 1
    tmp_ncrc |= 0x00000002;
#endif

    /* Sets the value of the NC2E bits. */
#if BSP_CFG_NONCACHEABLE_AREA0_OA_ENABLE == 1
    tmp_ncrc |= 0x00000004;
#endif

    /* Sets the value of the NC3E bits. */
#if BSP_CFG_NONCACHEABLE_AREA0_DM_ENABLE == 1
    tmp_ncrc |= 0x00000008;
#endif

    /* Makes settings to the NCRC0 register. */
    FLASH.NCRC0.LONG = tmp_ncrc;
} /* End of function rom_cache_noncacheable_area0_set() */
#endif /* BSP_CFG_NONCACHEABLE_AREA0_ENABLE == 1 */

#if BSP_CFG_NONCACHEABLE_AREA1_ENABLE == 1
/***********************************************************************************************************************
* Function name: rom_cache_noncacheable_area1_set
* Description  : Configures non-cacheable area 1 of the ROM cache function.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void rom_cache_noncacheable_area1_set (void)
{
    /* Used to specify the value written to the NCRC1 register. */
    uint32_t tmp_ncrc = 0;

    /* Disables the ROM cache. */
    FLASH.ROMCE.WORD = 0x0000;

    /* Makes settings to the NCRG1 register. */
    #if (BSP_CFG_NONCACHEABLE_AREA1_ADDR >= 0xFFC00000) \
      && ((BSP_CFG_NONCACHEABLE_AREA1_ADDR & 0x0000000F) == 0x00000000)
    FLASH.NCRG1 = BSP_CFG_NONCACHEABLE_AREA1_ADDR;
    #else
        #error "Error! Invalid setting for BSP_CFG_NONCACHEABLE_AREA1_ADDR in r_bsp_config.h"
    #endif

    /* Sets the value of the NCSZ bits. */
#if   BSP_CFG_NONCACHEABLE_AREA1_SIZE == 0x0
    /* Do nothing since NCRC1 bits should be 0. */
#elif BSP_CFG_NONCACHEABLE_AREA1_SIZE == 0x1
    tmp_ncrc |= 0x00000010;
#elif BSP_CFG_NONCACHEABLE_AREA1_SIZE == 0x2
    tmp_ncrc |= 0x00000030;
#elif BSP_CFG_NONCACHEABLE_AREA1_SIZE == 0x3
    tmp_ncrc |= 0x00000070;
#elif BSP_CFG_NONCACHEABLE_AREA1_SIZE == 0x4
    tmp_ncrc |= 0x000000F0;
#elif BSP_CFG_NONCACHEABLE_AREA1_SIZE == 0x5
    tmp_ncrc |= 0x000001F0;
#elif BSP_CFG_NONCACHEABLE_AREA1_SIZE == 0x6
    tmp_ncrc |= 0x000003F0;
#elif BSP_CFG_NONCACHEABLE_AREA1_SIZE == 0x7
    tmp_ncrc |= 0x000007F0;
#elif BSP_CFG_NONCACHEABLE_AREA1_SIZE == 0x8
    tmp_ncrc |= 0x00000FF0;
#elif BSP_CFG_NONCACHEABLE_AREA1_SIZE == 0x9
    tmp_ncrc |= 0x00001FF0;
#elif BSP_CFG_NONCACHEABLE_AREA1_SIZE == 0xA
    tmp_ncrc |= 0x00003FF0;
#elif BSP_CFG_NONCACHEABLE_AREA1_SIZE == 0xB
    tmp_ncrc |= 0x00007FF0;
#elif BSP_CFG_NONCACHEABLE_AREA1_SIZE == 0xC
    tmp_ncrc |= 0x0000FFF0;
#elif BSP_CFG_NONCACHEABLE_AREA1_SIZE == 0xD
    tmp_ncrc |= 0x0001FFF0;
#elif BSP_CFG_NONCACHEABLE_AREA1_SIZE == 0xE
    tmp_ncrc |= 0x0003FFF0;
#elif BSP_CFG_NONCACHEABLE_AREA1_SIZE == 0xF
    tmp_ncrc |= 0x0007FFF0;
#elif BSP_CFG_NONCACHEABLE_AREA1_SIZE == 0x10
    tmp_ncrc |= 0x000FFFF0;
#else
    #error "Error! Invalid setting for BSP_CFG_NONCACHEABLE_AREA1_SIZE in r_bsp_config.h"
#endif

    /* Sets the value of the NC1E bits. */
#if BSP_CFG_NONCACHEABLE_AREA1_IF_ENABLE == 1
    tmp_ncrc |= 0x00000002;
#endif

    /* Sets the value of the NC2E bits. */
#if BSP_CFG_NONCACHEABLE_AREA1_OA_ENABLE == 1
    tmp_ncrc |= 0x00000004;
#endif

    /* Sets the value of the NC3E bits. */
#if BSP_CFG_NONCACHEABLE_AREA1_DM_ENABLE == 1
    tmp_ncrc |= 0x00000008;
#endif

    /* Makes settings to the NCRC1 register. */
    FLASH.NCRC1.LONG = tmp_ncrc;
} /* End of function rom_cache_noncacheable_area1_set() */
#endif /* BSP_CFG_NONCACHEABLE_AREA1_ENABLE == 1 */
#endif /* BSP_CFG_ROM_CACHE_ENABLE == 1 */
#endif /* BSP_CFG_STARTUP_DISABLE == 0 */

/***********************************************************************************************************************
* Function name: output_ports_configure
* Description  : Configures the port and pin direction settings, and sets the pin outputs to a safe level.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void output_ports_configure(void)
{
    /* Add code here to setup additional output ports */
    R_BSP_NOP();
} /* End of function output_ports_configure() */

/***********************************************************************************************************************
* Function name: interrupts_configure
* Description  : Configures interrupts used
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void interrupts_configure(void)
{
    /* Add code here to setup additional interrupts */
    R_BSP_NOP();
} /* End of function interrupts_configure() */

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
    /* Smart Configurator initialization function */
    R_Systeminit();
#endif
} /* End of function peripheral_modules_enable() */

/***********************************************************************************************************************
* Function name: bsp_volsr_initial_configure
* Description  : Initializes the VOLSR register.
* Arguments    : none
* Return value : none
* Note         : none
***********************************************************************************************************************/
static void bsp_volsr_initial_configure(void)
{
    /* Used for argument of R_BSP_VoltageLevelSetting function. */
    uint8_t tmp_arg = 0;

    /* Set the pattern of the VOLSR(PGAVLS). */
#if (0xB == BSP_CFG_MCU_PART_FUNCTION) || (0xF == BSP_CFG_MCU_PART_FUNCTION)
    /* Configure for package without PGA diffrential input. */
    tmp_arg = BSP_VOL_AD_NEGATIVE_VOLTAGE_NOINPUT;
#else
    /* Configure for package with PGA diffrential input. */
    #if (3000 > BSP_CFG_MCU_AVCC_MV) || (5500 < BSP_CFG_MCU_AVCC_MV)
        #error "Error! Invalid setting for BSP_CFG_MCU_AVCC_MV in r_bsp_config.h"
    #elif 4000 > BSP_CFG_MCU_AVCC_MV
    tmp_arg = BSP_VOL_AD_NEGATIVE_VOLTAGE_NOINPUT;
    #else
        #if (0 == BSP_CFG_AD_NEGATIVE_VOLTAGE_INPUT_AN000) && (0 == BSP_CFG_AD_NEGATIVE_VOLTAGE_INPUT_AN001) \
             && (0 == BSP_CFG_AD_NEGATIVE_VOLTAGE_INPUT_AN002) && (0 == BSP_CFG_AD_NEGATIVE_VOLTAGE_INPUT_PGAVSS0) \
             && (0 == BSP_CFG_AD_NEGATIVE_VOLTAGE_INPUT_AN100) && (0 == BSP_CFG_AD_NEGATIVE_VOLTAGE_INPUT_AN101) \
             && (0 == BSP_CFG_AD_NEGATIVE_VOLTAGE_INPUT_AN102) && (0 == BSP_CFG_AD_NEGATIVE_VOLTAGE_INPUT_PGAVSS1)
    tmp_arg = BSP_VOL_AD_NEGATIVE_VOLTAGE_NOINPUT;
        #else
    tmp_arg = BSP_VOL_AD_NEGATIVE_VOLTAGE_INPUT;
        #endif
    #endif
#endif /* (0xB == BSP_CFG_MCU_PART_FUNCTION) || (0xF == BSP_CFG_MCU_PART_FUNCTION) */

    /* Set the pattern of the VOLSR(RICVLS). */
#if (4500 <= BSP_CFG_MCU_VCC_MV) && (5500 >= BSP_CFG_MCU_VCC_MV)
    tmp_arg |= BSP_VOL_RIIC_4_5V_OROVER;
#elif (2700 <= BSP_CFG_MCU_VCC_MV) && (4500 > BSP_CFG_MCU_VCC_MV)
    tmp_arg |= BSP_VOL_RIIC_UNDER_4_5V;
#else
    #error "Error! Invalid setting for BSP_CFG_MCU_VCC_MV in r_bsp_config.h"
#endif

    R_BSP_VoltageLevelSetting(tmp_arg);
} /* End of function bsp_volsr_initial_configure() */

