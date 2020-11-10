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
* File Name    : vecttbl.c
* Device(s)    : RX64M
* Description  : Definition of the exception vector table, reset vector, and user boot options.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 07.08.2013 1.00     First Release
*         : 30.05.2014 1.01     Added Option_Setting_Memory section for OFS and MDE register access
*         : 14.10.2014 1.02     Added Trusted mode enable/disable based on r_bsp_config.h setting.
*         : 15.05.2017 1.03     Deleted unnecessary comments.
*         : 01.11.2017 2.00     Added the bsp startup module disable function.
*         : 27.07.2018 2.01     Changed the setting of ID code protection.
*         : 28.02.2019 3.00     Deleted exception functions.
*                               (Exception functions moved to the common file (r_bsp_interrupts.c).)
*                               Added support for GNUC and ICCRX.
*                               Fixed coding style.
*         : 31.07.2019 3.01     Fixed initialization for option-setting memory.
*         : 08.10.2019 3.01     Changed for added support of Renesas RTOS (RI600V4 or RI600PX).
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* BSP configuration. */
#include "platform.h"

/* When using the user startup program, disable the following code. */
#if BSP_CFG_STARTUP_DISABLE == 0

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
R_BSP_POR_FUNCTION(R_BSP_POWER_ON_RESET_FUNCTION);
R_BSP_UB_POR_FUNCTION(R_BSP_UB_POWER_ON_RESET_FUNCTION);

/***********************************************************************************************************************
* The following array fills in the UB codes to get into User Boot Mode, the MDEB register, and the User Boot reset
* vector.
***********************************************************************************************************************/
#ifdef __BIG
    #define BSP_PRV_MDE_VALUE (0xfffffff8)    /* big */
#else
    #define BSP_PRV_MDE_VALUE (0xffffffff)    /* little */
#endif

/* The UB Code A, UB Code B, and Endian select register B (MDEB) are located in the User Boot space. Immediately
   following the MDEB register is the User Boot Reset Vector so it is defined below as well. These settings will only
   be used when the MCU is reset in User Boot Mode. In order for the MCU to start up in User Boot Mode the following
   conditions must be met:
   1) UB code A is 55736572h and 426F6F74h.
   2) UB code B is FFFF FF07h and 0008 C04Ch.
   3) The low level is being input on the MD pin.
   4) The high level is being input on the PC7 pin.
   Please see the Option-Setting Memory section of your MCU's HW manual for more information. */

/* 0xFF7FFFE8 - 0xFF7FFFEF : UB Code A register
   0xFF7FFFF0 - 0xFF7FFFF7 : UB Code B register
   0xFF7FFFF8 - 0xFF7FFFFB : Reserved
   0xFF7FFFFC - 0xFF7FFFFF : User Boot Reset Vector */

#if BSP_CFG_USER_BOOT_ENABLE == 1

/* Use this array if you are using User Boot. Make sure to fill in valid address for UB Reset Vector. */
/* Allocate this space in the user boot sector. */
R_BSP_ATTRIB_SECTION_CHANGE_UBSETTINGS const uint32_t user_boot_settings[6] =
{
    0x55736572,                 /* Required setting for UB Code A to get into User Boot */
    0x426f6f74,                 /* Required setting for UB Code A to get into User Boot */
    0xffffff07,                 /* Required setting for UB Code B to get into User Boot */
    0x0008c04c,                 /* Required setting for UB Code B to get into User Boot */
    0xFFFFFFFF,                 /* Reserved */
    (uint32_t) R_BSP_UB_POWER_ON_RESET_FUNCTION /* This is the User Boot Reset Vector.
                                                   When using User Boot put in the reset address here */
};
R_BSP_ATTRIB_SECTION_CHANGE_END

#endif /* BSP_CFG_USER_BOOT_ENABLE == 1 */

#if defined(__CCRX__)

#pragma address __SPCCreg  = 0x00120040         /* SPCC register */
#pragma address __TMEFreg  = 0x00120048         /* TMEF register */
#pragma address __OSIS1reg = 0x00120050         /* OSIC register (ID codes) */
#pragma address __OSIS2reg = 0x00120054         /* OSIC register (ID codes) */
#pragma address __OSIS3reg = 0x00120058         /* OSIC register (ID codes) */
#pragma address __OSIS4reg = 0x0012005C         /* OSIC register (ID codes) */
#pragma address __TMINFreg = 0x00120060         /* TMINF register */
#pragma address __MDEreg   = 0x00120064         /* MDE register (Single Chip Mode) */
#pragma address __OFS0reg  = 0x00120068         /* OFS0 register */
#pragma address __OFS1reg  = 0x0012006c         /* OFS1 register */

const uint32_t __SPCCreg  = 0xffffffff;
const uint32_t __TMEFreg  = BSP_CFG_TRUSTED_MODE_FUNCTION;
const uint32_t __OSIS1reg = BSP_CFG_ID_CODE_LONG_1;
const uint32_t __OSIS2reg = BSP_CFG_ID_CODE_LONG_2;
const uint32_t __OSIS3reg = BSP_CFG_ID_CODE_LONG_3;
const uint32_t __OSIS4reg = BSP_CFG_ID_CODE_LONG_4;
const uint32_t __TMINFreg = 0xffffffff;
const uint32_t __MDEreg   = BSP_PRV_MDE_VALUE;
const uint32_t __OFS0reg  = BSP_CFG_OFS0_REG_VALUE;
const uint32_t __OFS1reg  = BSP_CFG_OFS1_REG_VALUE;

#elif defined(__GNUC__)

const uint32_t __SPCCreg  __attribute__ ((section(".ofs1"))) = 0xffffffff;
const uint32_t __TMEFreg  __attribute__ ((section(".ofs2"))) = BSP_CFG_TRUSTED_MODE_FUNCTION;
const st_ofsm_sec_ofs3_t __ofsm_sec_ofs3   __attribute__ ((section(".ofs3"))) = {
    BSP_CFG_ID_CODE_LONG_1, /* __OSIS1reg */
    BSP_CFG_ID_CODE_LONG_2, /* __OSIS2reg */
    BSP_CFG_ID_CODE_LONG_3, /* __OSIS3reg */
    BSP_CFG_ID_CODE_LONG_4  /* __OSIS4reg */
};
const st_ofsm_sec_ofs4_t __ofsm_sec_ofs4   __attribute__ ((section(".ofs4"))) = {
    0xffffffff, /* __TMINFreg */
    BSP_PRV_MDE_VALUE, /* __MDEreg */
    BSP_CFG_OFS0_REG_VALUE, /* __OFS0reg */
    BSP_CFG_OFS1_REG_VALUE  /* __OFS1reg */
};

#elif defined(__ICCRX__)

#pragma public_equ = "__SPCC", 0xffffffff
#pragma public_equ = "__TMEF", BSP_CFG_TRUSTED_MODE_FUNCTION
#pragma public_equ = "__OSIS_1", BSP_CFG_ID_CODE_LONG_1
#pragma public_equ = "__OSIS_2", BSP_CFG_ID_CODE_LONG_2
#pragma public_equ = "__OSIS_3", BSP_CFG_ID_CODE_LONG_3
#pragma public_equ = "__OSIS_4", BSP_CFG_ID_CODE_LONG_4
#pragma public_equ = "__TMINF", 0xffffffff
#pragma public_equ = "__MDE", BSP_PRV_MDE_VALUE
#pragma public_equ = "__OFS0", BSP_CFG_OFS0_REG_VALUE
#pragma public_equ = "__OFS1", BSP_CFG_OFS1_REG_VALUE

#endif /* defined(__CCRX__), defined(__GNUC__), defined(__ICCRX__) */

/***********************************************************************************************************************
* The following array fills in the exception vector table.
***********************************************************************************************************************/
#if BSP_CFG_RTOS_USED == 4  /* Renesas RI600V4 & RI600PX */
     /* System configurator generates the ritble.src as interrupt & exception vector tables. */
#else /* BSP_CFG_RTOS_USED!=4 */

#if defined(__CCRX__) || defined(__GNUC__)
R_BSP_ATTRIB_SECTION_CHANGE_EXCEPTVECT void (* const Except_Vectors[])(void) =
{
    /* Offset from EXTB: Reserved area - must be all 0xFF */
    (void (*)(void))0xFFFFFFFF,  /* 0x00 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x04 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x08 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x0c - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x10 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x14 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x18 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x1c - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x20 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x24 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x28 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x2c - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x30 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x34 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x38 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x3c - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x40 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x44 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x48 - Reserved */
    (void (*)(void))0xFFFFFFFF,  /* 0x4c - Reserved */

    /* Exception vector table */
    excep_supervisor_inst_isr,         /* 0x50  Exception(Supervisor Instruction) */
    excep_access_isr,                  /* 0x54  Exception(Access exception) */
    undefined_interrupt_source_isr,    /* 0x58  Reserved */
    excep_undefined_inst_isr,          /* 0x5c  Exception(Undefined Instruction) */
    undefined_interrupt_source_isr,    /* 0x60  Reserved */
    excep_floating_point_isr,          /* 0x64  Exception(Floating Point) */
    undefined_interrupt_source_isr,    /* 0x68  Reserved */
    undefined_interrupt_source_isr,    /* 0x6c  Reserved */
    undefined_interrupt_source_isr,    /* 0x70  Reserved */
    undefined_interrupt_source_isr,    /* 0x74  Reserved */
    non_maskable_isr,                  /* 0x78  NMI */
};
R_BSP_ATTRIB_SECTION_CHANGE_END
#endif /* defined(__CCRX__), defined(__GNUC__) */

/***********************************************************************************************************************
* The following array fills in the reset vector.
***********************************************************************************************************************/
#if defined(__CCRX__) || defined(__GNUC__)
R_BSP_ATTRIB_SECTION_CHANGE_RESETVECT void (* const Reset_Vector[])(void) =
{
    R_BSP_POWER_ON_RESET_FUNCTION                   /* 0xfffffffc  RESET */
};
R_BSP_ATTRIB_SECTION_CHANGE_END
#endif /* defined(__CCRX__), defined(__GNUC__) */

#endif/* BSP_CFG_RTOS_USED */

#endif /* BSP_CFG_STARTUP_DISABLE == 0 */

