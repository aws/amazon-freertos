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
* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : mcu_interrupts.h
* Description  : This module is the control of the interrupt enable.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 01.02.2016 1.00     First Release
*         : 28.02.2019 2.00     Added the following prototype declaration.
*                                - bsp_interrupt_enable_disable
*                               Deleted the following prototype declarations.
*                               (The following prototype declarations moved to the common file (r_bsp_interrupts.h).)
*                                - bsp_interrupt_open
*                                - R_BSP_InterruptWrite
*                                - R_BSP_InterruptRead
*                                - R_BSP_InterruptControl
*                               Fixed coding style.
*         : 26.07.2019 2.10     Added the following command.
*                               - BSP_INT_CMD_FIT_INTERRUPT_ENABLE
*                               - BSP_INT_CMD_FIT_INTERRUPT_DISABLE
*                               Added the following error code.
*                               - BSP_INT_ERR_INVALID_IPL
*                               Added union of bsp_int_ctrl_t.
*                               Added the following enumeration constant.
*                               - BSP_INT_SRC_EMPTY
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Multiple inclusion prevention macro */
#ifndef MCU_INTERRUPTS_H
#define MCU_INTERRUPTS_H

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* Available return codes. */
typedef enum
{
    BSP_INT_SUCCESS = 0,
    BSP_INT_ERR_NO_REGISTERED_CALLBACK,     /* There is not a registered callback for this interrupt source */
    BSP_INT_ERR_INVALID_ARG,                /* Illegal argument input */
    BSP_INT_ERR_UNSUPPORTED,                /* Operation is not supported by this API */
    BSP_INT_ERR_INVALID_IPL                 /* Illegal IPL value input */
} bsp_int_err_t;

/* Available interrupts to register a callback for. */
typedef enum
{
    BSP_INT_SRC_EXC_SUPERVISOR_INSTR = 0, /* Occurs when privileged instruction is executed in User Mode */
    BSP_INT_SRC_EXC_UNDEFINED_INSTR,      /* Occurs when MCU encounters an unknown instruction */
    BSP_INT_SRC_EXC_NMI_PIN,              /* NMI Pin interrupt */
    BSP_INT_SRC_EXC_FPU,                  /* FPU exception */
    BSP_INT_SRC_EXC_ACCESS,               /* Access exception */
    BSP_INT_SRC_OSC_STOP_DETECT,          /* Oscillation stop is detected */
    BSP_INT_SRC_IWDT_ERROR,               /* IWDT underflow/refresh error has occurred */
    BSP_INT_SRC_LVD1,                     /* Voltage monitoring 1 interrupt */
    BSP_INT_SRC_LVD2,                     /* Voltage monitoring 2 interrupt */
    BSP_INT_SRC_UNDEFINED_INTERRUPT,      /* Interrupt has triggered for a vector that user did not write a handler. */
    BSP_INT_SRC_BUS_ERROR,                /* Bus error: illegal address access or timeout */
    BSP_INT_SRC_EMPTY,
    BSP_INT_SRC_TOTAL_ITEMS               /* DO NOT MODIFY! This is used for sizing the interrupt callback array. */
} bsp_int_src_t;

/* Available commands for R_BSP_InterruptControl() function. */
typedef enum
{
    BSP_INT_CMD_CALL_CALLBACK = 0,        /* Calls registered callback function if one exists */
    BSP_INT_CMD_INTERRUPT_ENABLE,         /* Enables a given interrupt (Available for NMI pin and Bus Error) */
    BSP_INT_CMD_INTERRUPT_DISABLE,        /* Disables a given interrupt (Available for Bus Error) */
    BSP_INT_CMD_FIT_INTERRUPT_ENABLE,     /* Enables interrupt by control of IPL. */
    BSP_INT_CMD_FIT_INTERRUPT_DISABLE     /* Disables interrupt by control of IPL. */
} bsp_int_cmd_t;

/* Type to be used for pdata argument in Control function. */
typedef union
{
    uint32_t ipl;                         /* Used at the following times.
                                             - When enabling an interrupt to set that interrupt's priority level 
                                               by BSP_INT_CMD_GROUP_INTERRUPT_ENABLE command.
                                             - When disabling an interrupt to save that interrupt's priority level 
                                               by BSP_INT_CMD_FIT_INTERRUPT_DISABLE command.
                                             - When enabling an interrupt to set that interrupt's priority level 
                                               by BSP_INT_CMD_FIT_INTERRUPT_ENABLE command. */
} bsp_int_ctrl_t;

/* Easy to use typedef for callback functions. */
typedef void (*bsp_int_cb_t)(void *);

/* This structure is the common one that is passed as the 'void *' argument to callback functions when an
 * exception occurs.
 */
typedef struct
{
    bsp_int_src_t vector;         /* Which vector caused this interrupt */
} bsp_int_cb_args_t;

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
bsp_int_err_t bsp_interrupt_enable_disable(bsp_int_src_t vector, bool enable);

#endif /* MCU_INTERRUPTS_H */

