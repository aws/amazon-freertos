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
* File Name    : vecttbl.c
* Device(s)    : RX13T
* Description  : Definition of the fixed vector table and option setting memory.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 26.07.2019 1.00     First Release
*         : 08.10.2019 1.01     Changed for added support of Renesas RTOS (RI600V4 or RI600PX).
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

/***********************************************************************************************************************
* The following array fills in the endian and option function select registers, and the fixed vector table
* bytes.
***********************************************************************************************************************/
#ifdef __BIG
    #define BSP_PRV_MDE_VALUE (0xfffffff8)    /* big */
#else
    #define BSP_PRV_MDE_VALUE (0xffffffff)    /* little */
#endif

#if defined(__ICCRX__)

#pragma public_equ = "__MDES", BSP_PRV_MDE_VALUE
#pragma public_equ = "__OFS1", BSP_CFG_OFS1_REG_VALUE
#pragma public_equ = "__OFS0", BSP_CFG_OFS0_REG_VALUE
#pragma public_equ = "__ID_BYTES_1_4", BSP_CFG_ID_CODE_LONG_1
#pragma public_equ = "__ID_BYTES_5_8", BSP_CFG_ID_CODE_LONG_2
#pragma public_equ = "__ID_BYTES_9_12", BSP_CFG_ID_CODE_LONG_3
#pragma public_equ = "__ID_BYTES_13_16", BSP_CFG_ID_CODE_LONG_4

#endif /* defined(__ICCRX__) */

#if BSP_CFG_RTOS_USED == 4  /* Renesas RI600V4 & RI600PX */
     /* System configurator generates the ritble.src as interrupt & exception vector tables. */
#else /* BSP_CFG_RTOS_USED!=4 */

#if defined(__CCRX__) || defined(__GNUC__)
R_BSP_ATTRIB_SECTION_CHANGE_FIXEDVECT void * const Fixed_Vectors[] =
{
    /* The Endian select register (MDE), Option function select register 1 (OFS1), and Option function select
       register 0 (OFS0) are located in User ROM. */
    (void *)BSP_PRV_MDE_VALUE,        /* 0xffffff80 - MDE */
    (void *)0xFFFFFFFF,               /* 0xffffff84 - Reserved */
    (void *)BSP_CFG_OFS1_REG_VALUE,   /* 0xffffff88 - OFS1 */
    (void *)BSP_CFG_OFS0_REG_VALUE,   /* 0xffffff8c - OFS0 */

    /* 0xffffff90 through 0xffffffaf: Reserved area */
    (void *)0xFFFFFFFF,   /* 0xffffff90 - Reserved */
    (void *)0xFFFFFFFF,   /* 0xffffff94 - Reserved */
    (void *)0xFFFFFFFF,   /* 0xffffff98 - Reserved */
    (void *)0xFFFFFFFF,   /* 0xffffff9c - Reserved */

    /* 0xffffffa0 through 0xffffffaf: ID Code Protection. The ID code is specified using macros in r_bsp_config.h. */
    (void *) BSP_CFG_ID_CODE_LONG_1,  /* 0xffffffa0 - Control code and ID code */
    (void *) BSP_CFG_ID_CODE_LONG_2,  /* 0xffffffa4 - ID code (cont.) */
    (void *) BSP_CFG_ID_CODE_LONG_3,  /* 0xffffffa8 - ID code (cont.) */
    (void *) BSP_CFG_ID_CODE_LONG_4,  /* 0xffffffac - ID code (cont.) */

    /* 0xffffffb0 through 0xffffffcf: Reserved area */
    (void *) 0xFFFFFFFF,  /* 0xffffffb0 - Reserved */
    (void *) 0xFFFFFFFF,  /* 0xffffffb4 - Reserved */
    (void *) 0xFFFFFFFF,  /* 0xffffffb8 - Reserved */
    (void *) 0xFFFFFFFF,  /* 0xffffffbc - Reserved */
    (void *) 0xFFFFFFFF,  /* 0xffffffc0 - Reserved */
    (void *) 0xFFFFFFFF,  /* 0xffffffc4 - Reserved */
    (void *) 0xFFFFFFFF,  /* 0xffffffc8 - Reserved */
    (void *) 0xFFFFFFFF,  /* 0xffffffcc - Reserved */

    /* Fixed vector table */
    (void *) excep_supervisor_inst_isr,         /* 0xffffffd0  Exception(Supervisor Instruction) */
    (void *) undefined_interrupt_source_isr,    /* 0xffffffd4  Reserved */
    (void *) undefined_interrupt_source_isr,    /* 0xffffffd8  Reserved */
    (void *) excep_undefined_inst_isr,          /* 0xffffffdc  Exception(Undefined Instruction) */
    (void *) undefined_interrupt_source_isr,    /* 0xffffffe0  Reserved */
    (void *) excep_floating_point_isr,          /* 0xffffffe4  Exception(Floating Point) */
    (void *) undefined_interrupt_source_isr,    /* 0xffffffe8  Reserved */
    (void *) undefined_interrupt_source_isr,    /* 0xffffffec  Reserved */
    (void *) undefined_interrupt_source_isr,    /* 0xfffffff0  Reserved */
    (void *) undefined_interrupt_source_isr,    /* 0xfffffff4  Reserved */
    (void *) non_maskable_isr,                  /* 0xfffffff8  NMI */
    (void *) R_BSP_POWER_ON_RESET_FUNCTION      /* 0xfffffffc  RESET */
};
R_BSP_ATTRIB_SECTION_CHANGE_END
#endif /* defined(__CCRX__), defined(__GNUC__) */

#endif/* BSP_CFG_RTOS_USED */

#endif /* BSP_CFG_STARTUP_DISABLE == 0 */

