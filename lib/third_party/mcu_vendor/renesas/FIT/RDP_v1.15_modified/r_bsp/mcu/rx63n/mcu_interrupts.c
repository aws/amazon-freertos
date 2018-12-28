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
* File Name	   : mcu_interrupts.c
* Description  : This module allows for callbacks to be registered for certain interrupts.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.05.2013 1.00     First Release
*         : xx.xx.xxxx x.xx     Added support for GNUC and ICCRX.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Access to r_bsp. */
#include "platform.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* This macro is used to suppress compiler messages about a parameter not being used in a function. The nice thing
 * about using this implementation is that it does not take any extra RAM or ROM.
 */
#define INTERNAL_NOT_USED(p)        ((void)(p))
/* Let FPSW EV, EO, EZ, EU, EX=1 (FPU exceptions enabled.) */
#define FPU_EXCEPTIONS_ENABLE       (0x00007C00)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/
 
/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* This array holds callback functions. */
static void (* g_bsp_vectors[BSP_INT_SRC_TOTAL_ITEMS])(void * pdata);

bsp_int_err_t bsp_interrupt_enable_disable(bsp_int_src_t vector, bool enable);

/***********************************************************************************************************************
* Function Name: bsp_interrupt_open
* Description  : Initialize callback function array.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void bsp_interrupt_open (void)
{
    uint32_t i;

    for (i = 0; i < BSP_INT_SRC_TOTAL_ITEMS; i++)
    {
        g_bsp_vectors[i] = FIT_NO_FUNC;
    }
}

/***********************************************************************************************************************
* Function Name: R_BSP_InterruptWrite
* Description  : Registers a callback function for supported interrupts. If FIT_NO_FUNC, NULL, or
*                any other invalid function address is passed for the callback argument then any previously registered
*                callbacks are unregistered. Use of FIT_NO_FUNC is preferred over NULL since access to the address
*                defined by FIT_NO_FUNC will cause a bus error which is easy for the user to catch. NULL typically
*                resolves to 0 which is a valid address on RX MCUs.
* Arguments    : vector -
*                    Which interrupt to register a callback for.
*                callback -
*                    Pointer to function to call when interrupt occurs.
* Return Value : BSP_INT_SUCCESS -
*                    Callback registered
*                BSP_INT_ERR_INVALID_ARG -
*                    Invalid function address input, any previous function has been unregistered
***********************************************************************************************************************/
bsp_int_err_t R_BSP_InterruptWrite (bsp_int_src_t vector,  bsp_int_cb_t callback)
{
    bsp_int_err_t err;

    err = BSP_INT_SUCCESS;

    /* Check for valid address. */
    if (((uint32_t)callback == (uint32_t)NULL) || ((uint32_t)callback == (uint32_t)FIT_NO_FUNC))
    {
        g_bsp_vectors[vector] = FIT_NO_FUNC;
    }
    else
    {
        g_bsp_vectors[vector] = callback;
    }

    return err;
}

/***********************************************************************************************************************
* Function Name: R_BSP_InterruptRead
* Description  : Returns the callback function address for an interrupt.
* Arguments    : vector -
*                    Which interrupt to read the callback for.
*                callback -
*                    Pointer of where to store callback address.
* Return Value : BSP_INT_SUCCESS -
*                    Callback was registered and address has been stored in 'callback' parameter.
*                BSP_INT_ERR_NO_REGISTERED_CALLBACK -
*                    No valid callback has been registered for this interrupt source.
***********************************************************************************************************************/
bsp_int_err_t R_BSP_InterruptRead (bsp_int_src_t vector, bsp_int_cb_t * callback)
{
    bsp_int_err_t err;

    err = BSP_INT_SUCCESS;

    /* Check for valid address. */
    if (((uint32_t)g_bsp_vectors[vector] == (uint32_t)NULL) || ((uint32_t)g_bsp_vectors[vector] == (uint32_t)FIT_NO_FUNC))
    {
        err = BSP_INT_ERR_NO_REGISTERED_CALLBACK;
    }
    else
    {
        *callback = g_bsp_vectors[vector];
    }

    return err;
}

/***********************************************************************************************************************
* Function Name: R_BSP_InterruptControl
* Description  : Executes specified command.
* Arguments    : vector -
*                    Which vector is being used.
*                cmd -
*                    Which command to execute.
*                pdata -
*                    Pointer to data to use with command. Not currently used. Use FIT_NO_PTR.
* Return Value : BSP_INT_SUCCESS -
*                    Command executed successfully.
*                BSP_INT_ERR_NO_REGISTERED_CALLBACK -
*                    No valid callback has been registered for this interrupt source.
*                BSP_INT_ERR_INVALID_ARG -
*                    Invalid command sent in.
***********************************************************************************************************************/
bsp_int_err_t R_BSP_InterruptControl (bsp_int_src_t vector, bsp_int_cmd_t cmd, void * pdata)
{
    bsp_int_err_t       err;
    bsp_int_cb_args_t   cb_args;

    err = BSP_INT_SUCCESS;

    /* This code is only used to remove compiler info messages about these parameters not being used. */
    INTERNAL_NOT_USED(pdata);

    switch (cmd)
    {
        case (BSP_INT_CMD_CALL_CALLBACK):
            if (((uint32_t)g_bsp_vectors[vector] != (uint32_t)NULL) &&
                ((uint32_t)g_bsp_vectors[vector] != (uint32_t)FIT_NO_FUNC))
            {
                /* Fill in callback info. */
                cb_args.vector = vector;

                g_bsp_vectors[vector](&cb_args);
            }
            else
            {
                err = BSP_INT_ERR_NO_REGISTERED_CALLBACK;
            }
        break;

        case (BSP_INT_CMD_INTERRUPT_ENABLE):
            err = bsp_interrupt_enable_disable(vector, true);
        break;

        case (BSP_INT_CMD_INTERRUPT_DISABLE):
            err = bsp_interrupt_enable_disable(vector, false);
        break;

        default:
            err = BSP_INT_ERR_INVALID_ARG;
        break;
    }

    return err;
}

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
    uint32_t      temp_fpsw;
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
                IEN(BSC,BUSERR) = 1;
                /* Enable illegal address interrupt in the BSC */
                BSC.BEREN.BIT.IGAEN = 1;
                /* Enable timeout detection enable. */
                BSC.BEREN.BIT.TOEN = 1;
            }
            else
            {
                /* Disable the bus error interrupt. */
                /* Disable the interrupt in the ICU*/
                IEN(BSC,BUSERR) = 0;
                /* Disable illegal address interrupt in the BSC */
                BSC.BEREN.BIT.IGAEN = 0;
                /* Disable timeout detection enable. */
                BSC.BEREN.BIT.TOEN = 0;
            }

        break;

        case (BSP_INT_SRC_EXC_FPU):

            /* Get current FPSW. */
            temp_fpsw = (uint32_t)R_GET_FPSW();

            if (true == enable)
            {
                /* Set the FPU exception flags. */
                R_SET_FPSW(temp_fpsw | (uint32_t)FPU_EXCEPTIONS_ENABLE);
            }
            else
            {
                /* Clear only the FPU exception flags. */
                R_SET_FPSW(temp_fpsw & (uint32_t)~FPU_EXCEPTIONS_ENABLE);
            }

        break;

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
}

