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
* Device(s)    : RX63x
* Description  : Definition of the fixed vector table and user boot options.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 26.10.2011 1.00     First Release
*         : 17.02.2012 1.10     Made function names compliant with CS v4.0
*         : 13.03.2012 1.20     ID Code is now specified in r_bsp_config.h. It is still used here in Fixed_Vectors[].
*         : 26.06.2012 1.30     Brought in arrays from flash_options.c into here because they were in the same space.
*                               Also added optional callbacks to the ISRs. Now using sections instead of defining the
*                               address for a specific array. Added macros to define the OFS registers. Added option
*                               to choose endian for User App and User Boot. Add macro to set ROM Code Protection.
*         : 16.07.2012 1.40     Added code to handle exception interrupts better.
*         : 19.11.2012 1.50     Updated code to use 'BSP_' and 'BSP_CFG_' prefix for macros.
*         : 06.05.2013 1.60     Code auto detects endian from compiler macros now.
*         : 24.06.2013 1.70     Code now uses functions from mcu_interrupts.c for exception callbacks. All interrupts
*                               that map to NMI vector are now supported. Fixed bug in FPU flag clearing.
*         : xx.xx.xxxx x.xx     Added support for GNUC and ICCRX.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* BSP configuration. */
#include "platform.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Defines CV, CO, CZ, CU, CX, and CE bits. */
#define FPU_CAUSE_FLAGS     (0x000000FC)

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* BCH - 01/16/2013 */
/* 3447: External linkage is not needed for this special function as it is the function that is run out of reset. */
/* PRQA S 3447 ++ */
extern void PowerON_Reset_PC(void);

#if defined(__CCRX__) || defined(__GNUC__)
R_PRAGMA_INTERRUPT_FUNCTION(excep_supervisor_inst_isr)
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
   0xFF7FFFF8 - 0xFF7FFFFB : MDEB register
   0xFF7FFFFC - 0xFF7FFFFF : User Boot Reset Vector */

#if BSP_CFG_USER_BOOT_ENABLE == 1

/* Use this array if you are using User Boot. Make sure to fill in valid address for UB Reset Vector. */
/* Allocate this space in the user boot sector. */
R_ATTRIB_SECTION_CHANGE_UBSETTINGS const uint32_t user_boot_settings[6] = 
{
    0x55736572,                 //Required setting for UB Code A to get into User Boot
    0x426f6f74,                 //Required setting for UB Code A to get into User Boot
    0xffffff07,                 //Required setting for UB Code B to get into User Boot
    0x0008c04c,                 //Required setting for UB Code B to get into User Boot
    MDE_VALUE,
    (uint32_t) PowerON_Reset_PC //This is the User Boot Reset Vector. When using User Boot put in the reset address here
};
R_ATTRIB_SECTION_CHANGE_END

#endif /* BSP_CFG_USER_BOOT_ENABLE == 1 */

/***********************************************************************************************************************
* The following array fills in the option function select registers, fixed vector table, and the ID code protection 
* bytes.
***********************************************************************************************************************/
#if defined(__CCRX__) || defined(__GNUC__)
R_ATTRIB_SECTION_CHANGE_FIXEDVECT void * const Fixed_Vectors[] = 
{
    /* The Endian select register S (MDES), Option function select register 1 (OFS1), and Option function select 
       register 0 (OFS0) are located in User ROM. */
    
    /* 0xFFFFFF80 - 0xFFFFFF83 : MDES register
       0xFFFFFF84 - 0xFFFFFF87 : Reserved space (0xFF's)
       0xFFFFFF88 - 0xFFFFFF8B : OFS1 register
       0xFFFFFF8C - 0xFFFFFF8F : OFS0 register */

    (void *)MDE_VALUE,      //Endian
    (void *)0xFFFFFFFF,     //Reserved space
    /* Configure whether voltage detection 0 circuit and HOCO are enabled after reset. 
       OFS1 - Option Function Select Register 1 
       b31:b9 Reserved (set to 1)
       b8     HOCOEN - Enable/disable HOCO oscillation after a reset (0=enable, 1=disable)
       b7:b3  Reserved (set to 1)
       b2     LVDAS - Choose to enable/disable Voltage Detection 0 Circuit after a reset (0=enable, 1=disable)
       b1:b0  Reserved (set to 1) */
    (void *)BSP_CFG_OFS1_REG_VALUE,     //Defined in r_bsp_config.h
    /* Configure WDT and IWDT settings. 
       OFS0 - Option Function Select Register 0 
       b31:b29 Reserved (set to 1)
       b28     WDTRSTIRQS - WDT Reset Interrupt Request - What to do on underflow (0=take interrupt, 1=reset MCU)
       b27:b26 WDTRPSS - WDT Window Start Position Select - (0=25%, 1=50%, 2=75%, 3=100%,don't use)
       b25:b24 WDTRPES - WDT Window End Position Select - (0=75%, 1=50%, 2=25%, 3=0%,don't use)
       b23:b20 WDTCKS - WDT Clock Frequency Division Ratio - (1=/4, 4=/64, 0xF=/128, 6=/512, 7=/2048, 8=/8192)
       b19:b18 WDTTOPS - WDT Timeout Period Select - (0=1024 cycles, 1=4096, 2=8192, 3=16384)
       b17     WDTSTRT - WDT Start Mode Select - (0=auto-start after reset, halt after reset)
       b16:b15 Reserved (set to 1)
       b14     IWDTSLCSTP - IWDT Sleep Mode Count Stop Control - (0=can't stop count, 1=stop w/some low power modes)
       b13     Reserved (set to 1)
       b12     IWDTRSTIRQS - IWDT Reset Interrupt Request - What to do on underflow (0=take interrupt, 1=reset MCU)
       b11:b10 IWDTRPSS - IWDT Window Start Position Select - (0=25%, 1=50%, 2=75%, 3=100%,don't use)
       b9:b8   IWDTRPES - IWDT Window End Position Select - (0=75%, 1=50%, 2=25%, 3=0%,don't use)
       b7:b4   IWDTCKS - IWDT Clock Frequency Division Ratio - (0=none, 2=/16, 3 = /32, 4=/64, 0xF=/128, 5=/256)
       b3:b2   IWDTTOPS - IWDT Timeout Period Select - (0=1024 cycles, 1=4096, 2=8192, 3=16384)
       b1      IWDTSTRT - IWDT Start Mode Select - (0=auto-start after reset, halt after reset)
       b0      Reserved (set to 1) */
    (void *)BSP_CFG_OFS0_REG_VALUE,

    /* 0xffffff90 through 0xffffff9f: Reserved area - must be all 0xFF */
    (void *)0xFFFFFFFF,   /* 0xffffff90 - Reserved */
    (void *)0xFFFFFFFF,   /* 0xffffff94 - Reserved */
    (void *)0xFFFFFFFF,   /* 0xffffff98 - Reserved */

    /* The 32-bit area immediately below (0xffffff9c through 0xffffff9f) is a special area that allows the ROM to be 
       protected from reading or writing by a parallel programmer. Please refer to the HW manual for appropriate 
       settings. The default (all 0xff) places no restrictions and therefore allows reads and writes by a parallel 
       programmer. */
    (void *)BSP_CFG_ROM_CODE_PROTECT_VALUE,   /* 0xffffff9C - ROM Code Protection */

    /* The memory are immediately below (0xffffffa0 through 0xffffffaf) is a special area that allows the on-chip 
       firmware to be protected. See the section "ID Code Protection" in the HW manual for details on how to enable 
       protection. Setting the four long words below to non-0xFF values will enable protection.  Do this only after 
       carefully review the HW manual */
   
    /* 0xffffffA0 through 0xffffffaf: ID Code Protection. The ID code is specified using macros in r_bsp_config.h.  */
    (void *) BSP_CFG_ID_CODE_LONG_1,  /* 0xffffffA0 - Control code and ID code */
    (void *) BSP_CFG_ID_CODE_LONG_2,  /* 0xffffffA4 - ID code (cont.) */
    (void *) BSP_CFG_ID_CODE_LONG_3,  /* 0xffffffA8 - ID code (cont.) */
    (void *) BSP_CFG_ID_CODE_LONG_4,  /* 0xffffffAC - ID code (cont.) */
  
    /* 0xffffffB0 through 0xffffffcf: Reserved area */
    (void *) 0xFFFFFFFF,  /* 0xffffffB0 - Reserved */
    (void *) 0xFFFFFFFF,  /* 0xffffffB4 - Reserved */
    (void *) 0xFFFFFFFF,  /* 0xffffffB8 - Reserved */
    (void *) 0xFFFFFFFF,  /* 0xffffffBC - Reserved */
    (void *) 0xFFFFFFFF,  /* 0xffffffC0 - Reserved */
    (void *) 0xFFFFFFFF,  /* 0xffffffC4 - Reserved */
    (void *) 0xFFFFFFFF,  /* 0xffffffC8 - Reserved */
    (void *) 0xFFFFFFFF,  /* 0xffffffCC - Reserved */

    /* Fixed vector table */
    /* BCH - 01/16/2013 */
    /* The PRQA tool gives a defect here for casting between pointer-to-function to pointer-to-object since this is 
       undefined behavior in C90. This could be split into a separate array but it would not provide any real benefit.
       For now this is suppressed. */
    /* PRQA S 0307 ++ */
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
    (void *) PowerON_Reset_PC                   /* 0xfffffffc  RESET */
};
R_ATTRIB_SECTION_CHANGE_END
#endif /* defined(__CCRX__) || defined(__GNUC__) */


