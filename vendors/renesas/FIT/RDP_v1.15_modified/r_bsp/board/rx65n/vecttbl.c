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
* Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : vecttbl.c
* Device(s)    : RX65N
* Description  : Definition of the exception vector table, reset vector, and user boot options.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version   Description
*         : 01.10.2016 1.00      First Release
*         : 15.05.2017 2.00      Deleted unnecessary comments.
*                                Added the following setting.
*                                - Setting of Expansion RAM.
*                                - Setting of Bank Mode Select.
*                                - Setting of Startup Bank Switch.
*                                Added the bsp startup module disable function.
*         : 27.07.2018 2.01      Modified the comment of START_BANK_VALUE.
*                                Changed the setting of ID code protection.
*         : xx.xx.xxxx 2.02      Added support for GNUC and ICCRX.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* BSP configuration. */
#include "platform.h"

/* When using the user startup program, disable the following code. */
#if (BSP_CFG_STARTUP_DISABLE == 0)

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Defines CV, CO, CZ, CU, CX, and CE bits. */
#define FPU_CAUSE_FLAGS     (0x000000FC)

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
extern void PowerON_Reset_PC(void);

#if defined(__CCRX__) || defined(__GNUC__)
R_PRAGMA_INTERRUPT_FUNCTION(excep_supervisor_inst_isr)
R_PRAGMA_INTERRUPT_FUNCTION(excep_access_isr)
R_PRAGMA_INTERRUPT_FUNCTION(excep_undefined_inst_isr)
R_PRAGMA_INTERRUPT_FUNCTION(excep_floating_point_isr)
R_PRAGMA_INTERRUPT_FUNCTION(non_maskable_isr)
R_PRAGMA_INTERRUPT_DEFAULT(undefined_interrupt_source_isr)
#elif defined(__ICCRX__)
R_PRAGMA_INTERRUPT_FUNCTION(__privileged_handler)
R_PRAGMA_INTERRUPT_FUNCTION(__excep_access_inst)
R_PRAGMA_INTERRUPT_FUNCTION(__undefined_handler)
R_PRAGMA_INTERRUPT_FUNCTION(_float_placeholder)
R_PRAGMA_INTERRUPT_FUNCTION(__NMI_handler)
R_PRAGMA_INTERRUPT_DEFAULT(__undefined_interrupt_source_handler)
#endif /* defined(__CCRX__), defined(__GNUC__), defined(__ICCRX__) */

R_PRAGMA_INTERRUPT(bus_error_isr, VECT(BSC,BUSERR))

/***********************************************************************************************************************
* Function name: excep_supervisor_inst_isr
* Description  : Supervisor Instruction Violation ISR
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#if defined(__CCRX__) || defined(__GNUC__)
R_ATTRIB_INTERRUPT void excep_supervisor_inst_isr(void)
#elif defined(__ICCRX__)
R_ATTRIB_INTERRUPT void __privileged_handler(void)
#endif /* defined(__CCRX__), defined(__GNUC__), defined(__ICCRX__) */
{
    /* If user has registered a callback for this exception then call it. */
    R_BSP_InterruptControl(BSP_INT_SRC_EXC_SUPERVISOR_INSTR, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
}

/***********************************************************************************************************************
* Function name: excep_access_isr
* Description  : Access exception ISR
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#if defined(__CCRX__) || defined(__GNUC__)
R_ATTRIB_INTERRUPT void excep_access_isr(void)
#elif defined(__ICCRX__)
R_ATTRIB_INTERRUPT void __excep_access_inst(void)
#endif /* defined(__CCRX__), defined(__GNUC__), defined(__ICCRX__) */
{
    /* If user has registered a callback for this exception then call it. */
    R_BSP_InterruptControl(BSP_INT_SRC_EXC_ACCESS, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
}

/***********************************************************************************************************************
* Function name: excep_undefined_inst_isr
* Description  : Undefined instruction exception ISR
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#if defined(__CCRX__) || defined(__GNUC__)
R_ATTRIB_INTERRUPT void excep_undefined_inst_isr(void)
#elif defined(__ICCRX__)
R_ATTRIB_INTERRUPT void __undefined_handler(void)
#endif /* defined(__CCRX__), defined(__GNUC__), defined(__ICCRX__) */
{
    /* If user has registered a callback for this exception then call it. */
    R_BSP_InterruptControl(BSP_INT_SRC_EXC_UNDEFINED_INSTR, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
}

/***********************************************************************************************************************
* Function name: excep_floating_point_isr
* Description  : Floating point exception ISR
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#if defined(__CCRX__) || defined(__GNUC__)
R_ATTRIB_INTERRUPT void excep_floating_point_isr(void)
#elif defined(__ICCRX__)
R_ATTRIB_INTERRUPT void _float_placeholder(void)
#endif /* defined(__CCRX__), defined(__GNUC__), defined(__ICCRX__) */
{
    /* Used for reading FPSW register. */
    uint32_t temp_fpsw;

    /* If user has registered a callback for this exception then call it. */
    R_BSP_InterruptControl(BSP_INT_SRC_EXC_FPU, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

    /* Get current FPSW. */
    temp_fpsw = (uint32_t)R_GET_FPSW();
    /* Clear only the FPU exception flags. */
    R_SET_FPSW(temp_fpsw & ((uint32_t)~FPU_CAUSE_FLAGS));
}

/***********************************************************************************************************************
* Function name: non_maskable_isr
* Description  : Non-maskable interrupt ISR
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#if defined(__CCRX__) || defined(__GNUC__)
R_ATTRIB_INTERRUPT void non_maskable_isr(void)
#elif defined(__ICCRX__)
R_ATTRIB_INTERRUPT void __NMI_handler(void)
#endif /* defined(__CCRX__), defined(__GNUC__), defined(__ICCRX__) */
{
    /* Determine what is the cause of this interrupt. */
    if (1 == ICU.NMISR.BIT.NMIST)
    {
        /* NMI pin interrupt is requested. */
        R_BSP_InterruptControl(BSP_INT_SRC_EXC_NMI_PIN, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

        /* Clear NMI pin interrupt flag. */
        ICU.NMICLR.BIT.NMICLR = 1;
    }

    if (1 == ICU.NMISR.BIT.OSTST)
    {
        /* Oscillation stop detection interrupt is requested. */
        R_BSP_InterruptControl(BSP_INT_SRC_OSC_STOP_DETECT, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

        /* Clear oscillation stop detect flag. */
        ICU.NMICLR.BIT.OSTCLR = 1;
    }

    if (1 == ICU.NMISR.BIT.WDTST)
    {
        /* WDT underflow/refresh error interrupt is requested. */
        R_BSP_InterruptControl(BSP_INT_SRC_WDT_ERROR, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

        /* Clear WDT flag. */
        ICU.NMICLR.BIT.WDTCLR = 1;
    }

    if (1 == ICU.NMISR.BIT.IWDTST)
    {
        /* IWDT underflow/refresh error interrupt is requested. */
        R_BSP_InterruptControl(BSP_INT_SRC_IWDT_ERROR, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

        /* Clear IWDT flag. */
        ICU.NMICLR.BIT.IWDTCLR = 1;
    }

    if (1 == ICU.NMISR.BIT.LVD1ST)
    {
        /* Voltage monitoring 1 interrupt is requested. */
        R_BSP_InterruptControl(BSP_INT_SRC_LVD1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

        /* Clear LVD1 flag. */
        ICU.NMICLR.BIT.LVD1CLR = 1;
    }

    if (1 == ICU.NMISR.BIT.LVD2ST)
    {
        /* Voltage monitoring 1 interrupt is requested. */
        R_BSP_InterruptControl(BSP_INT_SRC_LVD2, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

        /* Clear LVD2 flag. */
        ICU.NMICLR.BIT.LVD2CLR = 1;
    }

    if (1 == ICU.NMISR.BIT.RAMST)
    {
        if(1 == RAM.RAMSTS.BIT.RAMERR)
        {
            /* RAM Error interrupt is requested. */
            R_BSP_InterruptControl(BSP_INT_SRC_RAM, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

            /* Clear RAM flags. */
            RAM.RAMSTS.BIT.RAMERR = 0;
        }

        if(1 == RAM.EXRAMSTS.BIT.EXRAMERR)
        {
            /* Expansion RAM Error interrupt is requested. */
            R_BSP_InterruptControl(BSP_INT_SRC_EXRAM, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);

            /* Clear Expansion RAM flags. */
            RAM.EXRAMSTS.BIT.EXRAMERR = 0;
        }
    }
}

/***********************************************************************************************************************
* Function name: undefined_interrupt_source_isr
* Description  : All undefined interrupt vectors point to this function.
*                Set a breakpoint in this function to determine which source is creating unwanted interrupts.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#if defined(__CCRX__) || defined(__GNUC__)
R_ATTRIB_INTERRUPT void undefined_interrupt_source_isr(void)
#elif defined(__ICCRX__)
R_ATTRIB_INTERRUPT void __undefined_interrupt_source_handler(void)
#endif /* defined(__CCRX__), defined(__GNUC__), defined(__ICCRX__) */
{
    /* If user has registered a callback for this exception then call it. */
    R_BSP_InterruptControl(BSP_INT_SRC_UNDEFINED_INTERRUPT, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
}

/***********************************************************************************************************************
* Function name: bus_error_isr
* Description  : By default, this demo code enables the Bus Error Interrupt. This interrupt will fire if the user tries 
*                to access code or data from one of the reserved areas in the memory map, including the areas covered 
*                by disabled chip selects. A nop() statement is included here as a convenient place to set a breakpoint 
*                during debugging and development, and further handling should be added by the user for their 
*                application.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
R_ATTRIB_INTERRUPT void bus_error_isr (void)
{
    /* Clear the bus error */
    BSC.BERCLR.BIT.STSCLR = 1;

    /* 
        To find the address that was accessed when the bus error occurred, read the register BSC.BERSR2.WORD.  The upper
        13 bits of this register contain the upper 13-bits of the offending address (in 512K byte units)
    */

    /* If user has registered a callback for this exception then call it. */
    R_BSP_InterruptControl(BSP_INT_SRC_BUS_ERROR, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
}

/***********************************************************************************************************************
* The following array fills in the UB codes to get into User Boot Mode, the MDEB register, and the User Boot reset
* vector.
***********************************************************************************************************************/
#ifdef __BIG
    #define MDE_VALUE (0xfffffff8)    /* big */
#else
    #define MDE_VALUE (0xffffffff)    /* little */
#endif

#if defined(BSP_MCU_RX65N_2MB) /* In the case of 1.5 or 2Mbyte ROM capacity. */
    #if (BSP_CFG_CODE_FLASH_BANK_MODE == 0)
        #define BANK_MODE_VALUE (0xffffff8f)    /* dual */
    #else
        #define BANK_MODE_VALUE (0xffffffff)    /* linear */
    #endif
#else
    #define BANK_MODE_VALUE (0xffffffff)    /* linear */
#endif

#if defined(BSP_MCU_RX65N_2MB) /* In the case of 1.5 or 2Mbyte ROM capacity. */
    #if (BSP_CFG_CODE_FLASH_START_BANK == 0)
        /* The address range of bank 1 from FFE00000h to FFEFFFFFh and bank 0 from FFF00000h to FFFFFFFFh. */
        #define START_BANK_VALUE (0xffffffff)
    #else
        /* The address range of bank 1 from FFF00000h to FFFFFFFFh and bank 0 from FFE00000h to FFEFFFFFh. */
        #define START_BANK_VALUE (0xfffffff8)
    #endif
#endif

#if defined(__CCRX__)

#pragma address __MDEreg     = 0xFE7F5D00
#pragma address __OFS0reg    = 0xFE7F5D04
#pragma address __OFS1reg    = 0xFE7F5D08
#pragma address __TMINFreg   = 0xFE7F5D10
#if defined(BSP_MCU_RX65N_2MB)
#pragma address __BANKSELreg = 0xFE7F5D20
#endif
#pragma address __SPCCreg    = 0xFE7F5D40
#pragma address __TMEFreg    = 0xFE7F5D48
#pragma address __OSIS1reg   = 0xFE7F5D50
#pragma address __OSIS2reg   = 0xFE7F5D54
#pragma address __OSIS3reg   = 0xFE7F5D58
#pragma address __OSIS4reg   = 0xFE7F5D5C
#pragma address __FAWreg     = 0xFE7F5D64
#pragma address __ROMCODEreg = 0xFE7F5D70

const unsigned long __MDEreg     = (MDE_VALUE & BANK_MODE_VALUE);
const unsigned long __OFS0reg    = BSP_CFG_OFS0_REG_VALUE;
const unsigned long __OFS1reg    = BSP_CFG_OFS1_REG_VALUE;
const unsigned long __TMINFreg   = 0xffffffff;
#if defined(BSP_MCU_RX65N_2MB)
const unsigned long __BANKSELreg = START_BANK_VALUE;
#endif
const unsigned long __SPCCreg    = 0xffffffff;
const unsigned long __TMEFreg    = BSP_CFG_TRUSTED_MODE_FUNCTION;
const unsigned long __OSIS1reg   = BSP_CFG_ID_CODE_LONG_1;
const unsigned long __OSIS2reg   = BSP_CFG_ID_CODE_LONG_2;
const unsigned long __OSIS3reg   = BSP_CFG_ID_CODE_LONG_3;
const unsigned long __OSIS4reg   = BSP_CFG_ID_CODE_LONG_4;
const unsigned long __FAWreg     = BSP_CFG_FAW_REG_VALUE;
const unsigned long __ROMCODEreg = BSP_CFG_ROMCODE_REG_VALUE;

#elif defined(__GNUC__)

const unsigned long __MDEreg     __attribute__ ((section(".ofs1"))) = (MDE_VALUE & BANK_MODE_VALUE);
const unsigned long __OFS0reg    __attribute__ ((section(".ofs1"))) = BSP_CFG_OFS0_REG_VALUE;
const unsigned long __OFS1reg    __attribute__ ((section(".ofs1"))) = BSP_CFG_OFS1_REG_VALUE;
const unsigned long __TMINFreg   __attribute__ ((section(".ofs2"))) = 0xffffffff;
#if defined(BSP_MCU_RX65N_2MB)
//const unsigned long __BANKSELreg __attribute__ ((section(".ofs2"))) = START_BANK_VALUE;
#endif
const unsigned long __SPCCreg    __attribute__ ((section(".ofs3"))) = 0xffffffff;
const unsigned long __TMEFreg    __attribute__ ((section(".ofs4"))) = BSP_CFG_TRUSTED_MODE_FUNCTION;
const unsigned long __OSIS1reg   __attribute__ ((section(".ofs5"))) = BSP_CFG_ID_CODE_LONG_1;
const unsigned long __OSIS2reg   __attribute__ ((section(".ofs5"))) = BSP_CFG_ID_CODE_LONG_2;
const unsigned long __OSIS3reg   __attribute__ ((section(".ofs5"))) = BSP_CFG_ID_CODE_LONG_3;
const unsigned long __OSIS4reg   __attribute__ ((section(".ofs5"))) = BSP_CFG_ID_CODE_LONG_4;
const unsigned long __FAWreg     __attribute__ ((section(".ofs6"))) = BSP_CFG_FAW_REG_VALUE;
const unsigned long __ROMCODEreg __attribute__ ((section(".ofs7"))) = BSP_CFG_ROMCODE_REG_VALUE;

#elif defined(__ICCRX__)

#pragma public_equ = "__MDE", (MDE_VALUE & BANK_MODE_VALUE)
#pragma public_equ = "__OFS0", BSP_CFG_OFS0_REG_VALUE
#pragma public_equ = "__OFS1", BSP_CFG_OFS1_REG_VALUE
#pragma public_equ = "__TMINF", 0xffffffff
#pragma public_equ = "__BANKSEL", START_BANK_VALUE
#pragma public_equ = "__SPCC", 0xffffffff
#pragma public_equ = "__TMEF", BSP_CFG_TRUSTED_MODE_FUNCTION
#pragma public_equ = "__OSIS_1", BSP_CFG_ID_CODE_LONG_1
#pragma public_equ = "__OSIS_2", BSP_CFG_ID_CODE_LONG_2
#pragma public_equ = "__OSIS_3", BSP_CFG_ID_CODE_LONG_3
#pragma public_equ = "__OSIS_4", BSP_CFG_ID_CODE_LONG_4
#pragma public_equ = "__FAW", BSP_CFG_FAW_REG_VALUE
#pragma public_equ = "__ROMCODE", BSP_CFG_ROMCODE_REG_VALUE

#endif /* defined(__CCRX__), defined(__GNUC__), defined(__ICCRX__) */

/***********************************************************************************************************************
* The following array fills in the exception vector table.
***********************************************************************************************************************/
#if defined(__CCRX__) || defined(__GNUC__)
R_ATTRIB_SECTION_CHANGE_EXCEPTVECT void * const Except_Vectors[] =
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
R_ATTRIB_SECTION_CHANGE_END
#endif /* defined(__CCRX__) || defined(__GNUC__) */

/***********************************************************************************************************************
* The following array fills in the reset vector.
***********************************************************************************************************************/
#if defined(__CCRX__) || defined(__GNUC__)
R_ATTRIB_SECTION_CHANGE_RESETVECT void (* const Reset_Vector[])(void) =
{
    PowerON_Reset_PC                   /* 0xfffffffc  RESET */
};
R_ATTRIB_SECTION_CHANGE_END
#endif /* defined(__CCRX__) || defined(__GNUC__) */

#endif /* BSP_CFG_STARTUP_DISABLE == 0 */

