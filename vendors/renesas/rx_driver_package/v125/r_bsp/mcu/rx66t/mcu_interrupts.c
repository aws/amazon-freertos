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
* File Name    : mcu_interrupts.c
* Description  : This module is the control of the interrupt enable.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version   Description
*         : 27.07.2018 1.00      First Release
*         : 28.02.2019 2.00      Deleted the following functions.
*                                (The following functions moved to the common file (r_bsp_interrupts.c).)
*                                - bsp_interrupt_open
*                                - bsp_interrupt_group_enable_disable
*                                  (And, changed the function name to bsp_gr_int_enable_disable.h.)
*                                - R_BSP_InterruptWrite
*                                - R_BSP_InterruptRead
*                                - R_BSP_InterruptControl
*                                Deleted the following definition.
*                                (The following definition moved to the common file (r_bsp_common.h).)
*                                - INTERNAL_NOT_USED(p)
*                                Replaced the setting of IEN bit.
*                                Added support for GNUC and ICCRX.
*                                Fixed coding style.
*                                Added support Group BE0 interrupts.
*                                Modified the bsp_interrupt_group_enable_disable function.
*                                Added the group_be0_handler_isr function.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Access to r_bsp. */
#include "platform.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Let FPSW EV, EO, EZ, EU, EX=1 (FPU exceptions enabled.) */
#define BSP_PRV_FPU_EXCEPTIONS_ENABLE       (0x00007C00)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
R_BSP_PRAGMA_STATIC_INTERRUPT(group_bl0_handler_isr, VECT(ICU,GROUPBL0))
R_BSP_PRAGMA_STATIC_INTERRUPT(group_bl1_handler_isr, VECT(ICU,GROUPBL1))
R_BSP_PRAGMA_STATIC_INTERRUPT(group_al0_handler_isr, VECT(ICU,GROUPAL0))
R_BSP_PRAGMA_STATIC_INTERRUPT(group_be0_handler_isr, VECT(ICU,GROUPBE0))

/***********************************************************************************************************************
* Function Name: bsp_interrupt_enable_disable
* Description  : Either enables or disables an interrupt.
* Arguments    : vector -
*                    Which vector to enable or disable.
*                enable -
*                    Whether to enable or disable the interrupt.
* Return Value : BSP_INT_SUCCESS -
*                    Interrupt enabled or disabled.
*                BSP_INT_ERR_UNSUPPORTED -
*                    API does not support enabling/disabling for this vector.
***********************************************************************************************************************/
bsp_int_err_t bsp_interrupt_enable_disable (bsp_int_src_t vector, bool enable)
{
#ifdef __FPU
    uint32_t      tmp_fpsw;
#endif
    bsp_int_err_t err = BSP_INT_SUCCESS;

    switch (vector)
    {
        case (BSP_INT_SRC_BUS_ERROR):
            if (true == enable)
            {
                /* Enable the bus error interrupt to catch accesses to illegal/reserved areas of memory */
                /* Clear any pending interrupts */
                IR(BSC,BUSERR) = 0;

                /* Make this the highest priority interrupt (adjust as necessary for your application */
                IPR(BSC,BUSERR) = 0x0F;

                /* Enable the interrupt in the ICU*/
                R_BSP_InterruptRequestEnable(VECT(BSC,BUSERR));

                /* Enable illegal address interrupt in the BSC */
                BSC.BEREN.BIT.IGAEN = 1;

                /* Enable timeout detection enable. */
                BSC.BEREN.BIT.TOEN = 1;
            }
            else
            {
                /* Disable the bus error interrupt. */
                /* Disable the interrupt in the ICU*/
                R_BSP_InterruptRequestDisable(VECT(BSC,BUSERR));

                /* Disable illegal address interrupt in the BSC */
                BSC.BEREN.BIT.IGAEN = 0;

                /* Disable timeout detection enable. */
                BSC.BEREN.BIT.TOEN = 0;
            }
            break;

#ifdef __FPU
        case (BSP_INT_SRC_EXC_FPU):

            /* Get current FPSW. */
            tmp_fpsw = (uint32_t)R_BSP_GET_FPSW();

            if (true == enable)
            {
                /* Set the FPU exception flags. */
                R_BSP_SET_FPSW((tmp_fpsw | (uint32_t)BSP_PRV_FPU_EXCEPTIONS_ENABLE));
            }
            else
            {
                /* Clear only the FPU exception flags. */
                R_BSP_SET_FPSW((tmp_fpsw & (uint32_t)~BSP_PRV_FPU_EXCEPTIONS_ENABLE));
            }
            break;
#endif

        case (BSP_INT_SRC_EXC_NMI_PIN):
            if (true == enable)
            {
                /* Enable NMI pin interrupt (cannot undo!) */
                ICU.NMIER.BIT.NMIEN = 1;
            }
            else
            {
                /* NMI pin interrupts cannot be disabled after being enabled. */
                err = BSP_INT_ERR_UNSUPPORTED;
            }
            break;

        default:
            err = BSP_INT_ERR_UNSUPPORTED;
            break;
    }

    return err;
} /* End of function bsp_interrupt_enable_disable() */

/***********************************************************************************************************************
* Function Name: group_bl0_handler_isr
* Description  : Interrupt handler for Group BL0 interrupts. The way this code works is that for each possible interrupt
*                in this group the following will be performed:
*                1) Test to see if an interrupt is requested for this source
*                2) If an interrupt is requested then the registered callback is called (if one is registered)
*                NOTE: The interrupt request flag must be cleared in the peripheral.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
R_BSP_ATTRIB_STATIC_INTERRUPT void group_bl0_handler_isr (void)
{
    /* BL0 IS3 */
    if (1 == ICU.GRPBL0.BIT.IS3)
    {
        /* BSP_INT_SRC_BL0_SCI1_ERI1 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI1_ERI1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL0 IS2 */
    if (1 == ICU.GRPBL0.BIT.IS2)
    {
        /* BSP_INT_SRC_BL0_SCI1_TEI1 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI1_TEI1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL0 IS11 */
    if (1 == ICU.GRPBL0.BIT.IS11)
    {
        /* BSP_INT_SRC_BL0_SCI5_ERI5 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI5_ERI5, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL0 IS10 */
    if (1 == ICU.GRPBL0.BIT.IS10)
    {
        /* BSP_INT_SRC_BL0_SCI5_TEI5 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI5_TEI5, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL0 IS13 */
    if (1 == ICU.GRPBL0.BIT.IS13)
    {
        /* BSP_INT_SRC_BL0_SCI6_ERI6 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI6_ERI6, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL0 IS12 */
    if (1 == ICU.GRPBL0.BIT.IS12)
    {
        /* BSP_INT_SRC_BL0_SCI6_TEI6 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI6_TEI6, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL0 IS17 */
    if (1 == ICU.GRPBL0.BIT.IS17)
    {
        /* BSP_INT_SRC_BL0_SCI12_ERI12 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI12_ERI12, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL0 IS16 */
    if (1 == ICU.GRPBL0.BIT.IS16)
    {
        /* BSP_INT_SRC_BL0_SCI12_TEI12 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI12_TEI12, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL0 IS18 */
    if (1 == ICU.GRPBL0.BIT.IS18)
    {
        /* BSP_INT_SRC_BL0_SCI12_SCIX0 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI12_SCIX0, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL0 IS19 */
    if (1 == ICU.GRPBL0.BIT.IS19)
    {
        /* BSP_INT_SRC_BL0_SCI12_SCIX1 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI12_SCIX1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL0 IS20 */
    if (1 == ICU.GRPBL0.BIT.IS20)
    {
        /* BSP_INT_SRC_BL0_SCI12_SCIX2 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI12_SCIX2, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL0 IS21 */
    if (1 == ICU.GRPBL0.BIT.IS21)
    {
        /* BSP_INT_SRC_BL0_SCI12_SCIX3 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_SCI12_SCIX3, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL0 IS26 */
    if (1 == ICU.GRPBL0.BIT.IS26)
    {
        /* BSP_INT_SRC_BL0_CAC_FERRI */
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_CAC_FERRI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL0 IS27 */
    if (1 == ICU.GRPBL0.BIT.IS27)
    {
        /* BSP_INT_SRC_BL0_CAC_MENDI */
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_CAC_MENDI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL0 IS28 */
    if (1 == ICU.GRPBL0.BIT.IS28)
    {
        /* BSP_INT_SRC_BL0_CAC_OVFI */
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_CAC_OVFI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL0 IS29 */
    if (1 == ICU.GRPBL0.BIT.IS29)
    {
        /* BSP_INT_SRC_BL0_DOC_DOPCI */
        R_BSP_InterruptControl(BSP_INT_SRC_BL0_DOC_DOPCI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }
} /* End of function group_bl0_handler_isr() */

/***********************************************************************************************************************
* Function Name: group_bl1_handler_isr
* Description  : Interrupt handler for Group BL1 interrupts. The way this code works is that for each possible interrupt
*                in this group the following will be performed:
*                1) Test to see if an interrupt is requested for this source
*                2) If an interrupt is requested then the registered callback is called (if one is registered)
*                NOTE: The interrupt request flag must be cleared in the peripheral.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
R_BSP_ATTRIB_STATIC_INTERRUPT void group_bl1_handler_isr (void)
{
    /* BL1 IS0 */
    if (1 == ICU.GRPBL1.BIT.IS0)
    {
        /* BSP_INT_SRC_BL1_POEG_POEGGAI */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_POEG_POEGGAI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS1 */
    if (1 == ICU.GRPBL1.BIT.IS1)
    {
        /* BSP_INT_SRC_BL1_POEG_POEGGBI */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_POEG_POEGGBI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS2 */
    if (1 == ICU.GRPBL1.BIT.IS2)
    {
        /* BSP_INT_SRC_BL1_POEG_POEGGCI */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_POEG_POEGGCI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS3 */
    if (1 == ICU.GRPBL1.BIT.IS3)
    {
        /* BSP_INT_SRC_BL1_POEG_POEGGDI */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_POEG_POEGGDI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS8 */
    if (1 == ICU.GRPBL1.BIT.IS8)
    {
        /* BSP_INT_SRC_BL1_POE3_OEI5 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_POE3_OEI5, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS9 */
    if (1 == ICU.GRPBL1.BIT.IS9)
    {
        /* BSP_INT_SRC_BL1_POE3_OEI1 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_POE3_OEI1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS10 */
    if (1 == ICU.GRPBL1.BIT.IS10)
    {
        /* BSP_INT_SRC_BL1_POE3_OEI2 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_POE3_OEI2, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS11 */
    if (1 == ICU.GRPBL1.BIT.IS11)
    {
        /* BSP_INT_SRC_BL1_POE3_OEI3 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_POE3_OEI3, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS12 */
    if (1 == ICU.GRPBL1.BIT.IS12)
    {
        /* BSP_INT_SRC_BL1_POE3_OEI4 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_POE3_OEI4, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS14 */
    if (1 == ICU.GRPBL1.BIT.IS14)
    {
        /* BSP_INT_SRC_BL1_RIIC0_EEI0 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_RIIC0_EEI0, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS13 */
    if (1 == ICU.GRPBL1.BIT.IS13)
    {
        /* BSP_INT_SRC_BL1_RIIC0_TEI0 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_RIIC0_TEI0, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS18 */
    if (1 == ICU.GRPBL1.BIT.IS18)
    {
        /* BSP_INT_SRC_BL1_S12AD2_S12CMPAI2 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_S12AD2_S12CMPAI2, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS19 */
    if (1 == ICU.GRPBL1.BIT.IS19)
    {
        /* BSP_INT_SRC_BL1_S12AD2_S12CMPBI2 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_S12AD2_S12CMPBI2, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS20 */
    if (1 == ICU.GRPBL1.BIT.IS20)
    {
        /* BSP_INT_SRC_BL1_S12AD0_S12CMPAI */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_S12AD0_S12CMPAI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS21 */
    if (1 == ICU.GRPBL1.BIT.IS21)
    {
        /* BSP_INT_SRC_BL1_S12AD0_S12CMPBI */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_S12AD0_S12CMPBI, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS22 */
    if (1 == ICU.GRPBL1.BIT.IS22)
    {
        /* BSP_INT_SRC_BL1_S12AD1_S12CMPAI1 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_S12AD1_S12CMPAI1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS23 */
    if (1 == ICU.GRPBL1.BIT.IS23)
    {
        /* BSP_INT_SRC_BL1_S12AD1_S12CMPBI1 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_S12AD1_S12CMPBI1, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS25 */
    if (1 == ICU.GRPBL1.BIT.IS25)
    {
        /* BSP_INT_SRC_BL1_SCI8_ERI8 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_SCI8_ERI8, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS24 */
    if (1 == ICU.GRPBL1.BIT.IS24)
    {
        /* BSP_INT_SRC_BL1_SCI8_TEI8 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_SCI8_TEI8, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS27 */
    if (1 == ICU.GRPBL1.BIT.IS27)
    {
        /* BSP_INT_SRC_BL1_SCI9_ERI9 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_SCI9_ERI9, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* BL1 IS26 */
    if (1 == ICU.GRPBL1.BIT.IS26)
    {
        /* BSP_INT_SRC_BL1_SCI9_TEI9 */
        R_BSP_InterruptControl(BSP_INT_SRC_BL1_SCI9_TEI9, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }
} /* End of function group_bl1_handler_isr() */

/***********************************************************************************************************************
* Function Name: group_al0_handler_isr
* Description  : Interrupt handler for Group AL0 interrupts. The way this code works is that for each possible interrupt
*                in this group the following will be performed:
*                1) Test to see if an interrupt is requested for this source
*                2) If an interrupt is requested then the registered callback is called (if one is registered)
*                NOTE: The interrupt request flag must be cleared in the peripheral.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
R_BSP_ATTRIB_STATIC_INTERRUPT void group_al0_handler_isr (void)
{
    /* AL0 IS13 */
    if (1 == ICU.GRPAL0.BIT.IS13)
    {
        /* BSP_INT_SRC_AL0_SCI11_ERI11 */
        R_BSP_InterruptControl(BSP_INT_SRC_AL0_SCI11_ERI11, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* AL0 IS12 */
    if (1 == ICU.GRPAL0.BIT.IS12)
    {
        /* BSP_INT_SRC_AL0_SCI11_TEI11 */
        R_BSP_InterruptControl(BSP_INT_SRC_AL0_SCI11_TEI11, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* AL0 IS17 */
   if (1 == ICU.GRPAL0.BIT.IS17)
    {
        /* BSP_INT_SRC_AL0_RSPI0_SPEI0 */
        R_BSP_InterruptControl(BSP_INT_SRC_AL0_RSPI0_SPEI0, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }

    /* AL0 IS16 */
    if (1 == ICU.GRPAL0.BIT.IS16)
    {
        /* BSP_INT_SRC_AL0_RSPI0_SPII0 */
        R_BSP_InterruptControl(BSP_INT_SRC_AL0_RSPI0_SPII0, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }
} /* End of function group_al0_handler_isr() */

/***********************************************************************************************************************
* Function Name: group_be0_handler_isr
* Description  : Interrupt handler for Group BE0 interrupts. The way this code works is that for each possible interrupt
*                in this group the following will be performed:
*                1) Test to see if an interrupt is requested for this source
*                2) If an interrupt is requested then the registered callback is called (if one is registered)
*                NOTE: The interrupt request flag must be cleared in the peripheral.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
R_BSP_ATTRIB_STATIC_INTERRUPT void group_be0_handler_isr (void)
{
    /* BE0 IS0 */
    if (1 == ICU.GRPBE0.BIT.IS0)
    {
        /* Clear the interrupt status flag. */
        ICU.GCRBE0.BIT.CLR0 = 1;

        /* BSP_INT_SRC_BE0_CAN0_ERS0 */
        R_BSP_InterruptControl(BSP_INT_SRC_BE0_CAN0_ERS0, BSP_INT_CMD_CALL_CALLBACK, FIT_NO_PTR);
    }
} /* End of function group_be0_handler_isr() */

