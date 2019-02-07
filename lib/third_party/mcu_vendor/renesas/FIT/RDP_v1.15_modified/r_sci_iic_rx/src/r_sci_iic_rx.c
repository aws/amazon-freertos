/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
 * No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
 * applicable laws, including copyright laws. 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO 
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the 
 * following link:
 * http://www.renesas.com/disclaimer 
 *
 * Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_sci_iic_rx.c
 * Description  : Functions for using SCI_IIC on RX devices. 
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.07.2013 1.00     First Release
 *         : 30.09.2013 1.10     Change symbol of return value and status
 *         : 01.07.2014 1.20     RX110 support added.
 *         : 22.09.2014 1.30     RX64M support added.
 *         : 01.12.2014 1.40     RX113 support added.
 *         : 15.12.2014 1.50     RX71M support added.
 *         : 27.02.2015 1.60     RX63N support added.
 *         : 29.05.2015 1.70     RX231 support added.
 *         : 31.10.2015 1.80     RX130, RX230, RX23T support added.
 *         : 04.03.2016 1.90     RX24T support added.
 *         : 01.10.2016 2.00     Fixed a program according to the Renesas coding rules.
 *         : 31.08.2017 2.20     Changed about the calculation processing for address of PIDR.
 *         : xx.xx.xxxx x.xx     Added support for GNUC and ICCRX.
 *                               Deleted stddef.h, stdio.h, and stdint.h includes because platform.h.includes them.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
/* Access to peripherals and board defines. */
#include "platform.h"

/* Defines for SCI_IIC support */
#include "r_sci_iic_rx_private.h"
#include "r_sci_iic_rx_if.h"
#include "r_sci_iic_rx_config.h"

/* Include specifics for chosen MCU.  */
#include "r_sci_iic_rx_platform.h"

/***********************************************************************************************************************
 Private global variables and functions
 **********************************************************************************************************************/
/*----------------------------------------------------------------------------*/
/*  Main processing of SCI IIC Driver API functions                           */
/*----------------------------------------------------------------------------*/
static sci_iic_return_t sci_iic_open (sci_iic_info_t * p_sci_iic_info);
static sci_iic_return_t sci_iic_master_send_receive (sci_iic_info_t * p_sci_iic_info, sci_iic_api_mode_t api_mode);
static sci_iic_return_t sci_iic_advance (sci_iic_info_t * p_sci_iic_info);
static void sci_iic_close (sci_iic_info_t * p_sci_iic_info);
static sci_iic_return_t sci_iic_control (sci_iic_info_t * p_sci_iic_info, sci_iic_ctrl_ptn_t ctrl_ptn);
static sci_iic_return_t sci_iic_getstatus (sci_iic_info_t * p_sci_iic_info, sci_iic_mcu_status_t * p_sci_iic_status);

/*----------------------------------------------------------------------------*/
/*  Called from function table                                                */
/*----------------------------------------------------------------------------*/
static sci_iic_return_t sci_iic_func_table (sci_iic_api_event_t event, sci_iic_info_t * p_sci_iic_info);
static sci_iic_return_t sci_iic_init_driver (sci_iic_info_t * p_sci_iic_info);
static sci_iic_return_t sci_iic_generate_start_cond (sci_iic_info_t * p_sci_iic_info);
static sci_iic_return_t sci_iic_after_gen_start_cond (sci_iic_info_t * p_sci_iic_info);
static sci_iic_return_t sci_iic_after_send_slvadr (sci_iic_info_t * p_sci_iic_info);
static sci_iic_return_t sci_iic_write_data_sending (sci_iic_info_t * p_sci_iic_info);
static sci_iic_return_t sci_iic_read_data_receiving (sci_iic_info_t * p_sci_iic_info);
static sci_iic_return_t sci_iic_release (sci_iic_info_t * p_sci_iic_info);
static sci_iic_return_t sci_iic_nack (sci_iic_info_t * p_sci_iic_info);

/*----------------------------------------------------------------------------*/
/*  Other function                                                            */
/*----------------------------------------------------------------------------*/
static void sci_iic_api_status_init (sci_iic_info_t * p_sci_iic_info);
static void sci_iic_set_internal_status (sci_iic_info_t * p_sci_iic_info, sci_iic_api_status_t new_status);
static void sci_iic_set_ch_status (sci_iic_info_t * p_sci_iic_info, sci_iic_ch_dev_status_t status);
static sci_iic_return_t sci_iic_check_chstatus_start (sci_iic_info_t * p_sci_iic_info);
static sci_iic_return_t sci_iic_check_chstatus_advance (sci_iic_info_t * p_sci_iic_info);
static void sci_iic_disable (sci_iic_info_t * p_sci_iic_info);
static void sci_iic_enable (sci_iic_info_t * p_sci_iic_info);
static bool sci_iic_stop_cond_wait (sci_iic_info_t * p_sci_iic_info);
static bool sci_iic_check_stop_event (sci_iic_info_t * p_sci_iic_info);

static void sci_iic_int_icier_setting (sci_iic_info_t * p_sci_iic_info, uint8_t New_icier);
static void sci_iic_start_cond_generate (sci_iic_info_t * p_sci_iic_info);
static void sci_iic_re_start_cond_generate (sci_iic_info_t * p_sci_iic_info);
static void sci_iic_stop_cond_generate (sci_iic_info_t * p_sci_iic_info);
static void sci_iic_set_sending_data (sci_iic_info_t * p_sci_iic_info, uint8_t * p_data);
static uint8_t sci_iic_get_receiving_data (sci_iic_info_t * p_sci_iic_info);
static void sci_iic_iic_disable (sci_iic_info_t * p_sci_iic_info);
static void sci_iic_iic_enable (sci_iic_info_t * p_sci_iic_info);
static void sci_iic_register_init_setting (sci_iic_info_t * p_sci_iic_info);

/*----------------------------------------------------------------------------*/
/*   Function table                                                           */
/*----------------------------------------------------------------------------*/
static const sci_iic_mtx_t gc_sci_iic_mtx_tbl[SCI_IIC_STS_MAX][SCI_IIC_EV_MAX] =
{
    /* Uninitialized state (sts0) */
    {
        { SCI_IIC_EV_INIT, sci_iic_init_driver }, /* IIC Driver Initialization */
        { SCI_IIC_EV_GEN_START_COND, NULL }, /* Start Condition Generation */
        { SCI_IIC_EV_INT_START, NULL }, /* Start Condition Generation Interrupt */
        { SCI_IIC_EV_INT_ADD, NULL }, /* Address Sending Interrupt */
        { SCI_IIC_EV_INT_SEND, NULL }, /* Data Sending Interrupt */
        { SCI_IIC_EV_INT_STOP, NULL }, /* Stop Condition Generation Interrupt */
        { SCI_IIC_EV_INT_NACK, NULL }, /* No Acknowledge Interrupt */
},

    /* Idle state (sts1)*/
    {
        { SCI_IIC_EV_INIT, NULL }, /* IIC Driver Initialization */
        { SCI_IIC_EV_GEN_START_COND, sci_iic_generate_start_cond }, /* Start Condition Generation */
        { SCI_IIC_EV_INT_START, NULL }, /* Start Condition Generation Interrupt */
        { SCI_IIC_EV_INT_ADD, NULL }, /* Address Sending Interrupt */
        { SCI_IIC_EV_INT_SEND, NULL }, /* Data Sending Interrupt */
        { SCI_IIC_EV_INT_STOP, NULL }, /* Stop Condition Generation Interrupt */
        { SCI_IIC_EV_INT_NACK, NULL }, /* No Acknowledge Interrupt */
    },

    /* Start condition generation completion wait state (sts2)*/
    {
        { SCI_IIC_EV_INIT, NULL }, /* IIC Driver Initialization */
        { SCI_IIC_EV_GEN_START_COND, NULL }, /* Start Condition Generation */
        { SCI_IIC_EV_INT_START, sci_iic_after_gen_start_cond }, /* Start Condition Generation Interrupt */
        { SCI_IIC_EV_INT_ADD, NULL }, /* Address Sending Interrupt */
        { SCI_IIC_EV_INT_SEND, NULL }, /* Data Sending Interrupt */
        { SCI_IIC_EV_INT_STOP, NULL }, /* Stop Condition Generation Interrupt */
        { SCI_IIC_EV_INT_NACK, sci_iic_nack }, /* No Acknowledge Interrupt */
    },

    /* Slave address [Write] transmission completion wait state (sts3)*/
    {
        { SCI_IIC_EV_INIT, NULL }, /* IIC Driver Initialization */
        { SCI_IIC_EV_GEN_START_COND, NULL }, /* Start Condition Generation */
        { SCI_IIC_EV_INT_START, NULL }, /* Start Condition Generation Interrupt */
        { SCI_IIC_EV_INT_ADD, sci_iic_after_send_slvadr }, /* Address Sending Interrupt */
        { SCI_IIC_EV_INT_SEND, NULL }, /* Data Sending Interrupt */
        { SCI_IIC_EV_INT_STOP, NULL }, /* Stop Condition Generation Interrupt */
        { SCI_IIC_EV_INT_NACK, sci_iic_nack }, /* No Acknowledge Interrupt */
    },

    /* Slave address [Read] transmission completion wait state (sts4) */
    {
        { SCI_IIC_EV_INIT, NULL }, /* IIC Driver Initialization */
        { SCI_IIC_EV_GEN_START_COND, NULL }, /* Start Condition Generation */
        { SCI_IIC_EV_INT_START, NULL }, /* Start Condition Generation Interrupt */
        { SCI_IIC_EV_INT_ADD, sci_iic_after_send_slvadr }, /* Address Sending Interrupt */
        { SCI_IIC_EV_INT_SEND, NULL }, /* Data Sending Interrupt */
        { SCI_IIC_EV_INT_STOP, NULL }, /* Stop Condition Generation Interrupt */
        { SCI_IIC_EV_INT_NACK, sci_iic_nack }, /* No Acknowledge Interrupt */
    },

    /* Data transmission completion wait state (sts5)*/
    {
        { SCI_IIC_EV_INIT, NULL }, /* IIC Driver Initialization */
        { SCI_IIC_EV_GEN_START_COND, NULL }, /* Start Condition Generation */
        { SCI_IIC_EV_INT_START, NULL }, /* Start Condition Generation Interrupt */
        { SCI_IIC_EV_INT_ADD, NULL }, /* Address Sending Interrupt */
        { SCI_IIC_EV_INT_SEND, sci_iic_write_data_sending }, /* Data Sending Interrupt */
        { SCI_IIC_EV_INT_STOP, NULL }, /* Stop Condition Generation Interrupt */
        { SCI_IIC_EV_INT_NACK, sci_iic_nack }, /* No Acknowledge Interrupt */
    },

    /* Data reception completion wait state (sts6)*/
    {
        { SCI_IIC_EV_INIT, NULL }, /* IIC Driver Initialization */
        { SCI_IIC_EV_GEN_START_COND, NULL }, /* Start Condition Generation */
        { SCI_IIC_EV_INT_START, NULL }, /* Start Condition Generation Interrupt */
        { SCI_IIC_EV_INT_ADD, sci_iic_read_data_receiving }, /* Address Sending Interrupt */
        { SCI_IIC_EV_INT_SEND, NULL }, /* Data Sending Interrupt */
        { SCI_IIC_EV_INT_STOP, NULL }, /* Stop Condition Generation Interrupt */
        { SCI_IIC_EV_INT_NACK, sci_iic_nack }, /* No Acknowledge Interrupt */
    },

    /* Stop condition generation completion wait state (sts7) */
    {
        { SCI_IIC_EV_INIT, NULL }, /* IIC Driver Initialization */
        { SCI_IIC_EV_GEN_START_COND, NULL }, /* Start Condition Generation */
        { SCI_IIC_EV_INT_START, NULL }, /* Start Condition Generation Interrupt */
        { SCI_IIC_EV_INT_ADD, NULL }, /* Address Sending Interrupt */
        { SCI_IIC_EV_INT_SEND, NULL }, /* Data Sending Interrupt */
        { SCI_IIC_EV_INT_STOP, sci_iic_release }, /* Stop Condition Generation Interrupt */
        { SCI_IIC_EV_INT_NACK, sci_iic_nack }, /* No Acknowledge Interrupt */
    },
};

/*----------------------------------------------------------------------------*/
/*   SCI IIC channel status                                                   */
/*----------------------------------------------------------------------------*/
volatile sci_iic_ch_dev_status_t g_sci_iic_ChStatus[SCI_IIC_NUM_CH_MAX]; /* Channel status */

/***********************************************************************************************************************
 * Function Name: R_SCI_IIC_Open
 * Description  : Initializes the SCI_IIC driver.
 *              : Initializes the I/O register for SCI_IIC control.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : SCI_IIC_SUCCESS                    ; Successful operation
 *              : SCI_IIC_ERR_ERR_LOCK_FUNC          ; Another task is handling API function
 *              ; SCI_IIC_ERR_INVALID_CHAN           ; channel number invalid for part
 *              : SCI_IIC_ERR_INVALID_ARG            ; Parameter error
 *              ; SCI_IIC_ERR_OTHER                  ; Other error
 **********************************************************************************************************************/
sci_iic_return_t R_SCI_IIC_Open (sci_iic_info_t * p_sci_iic_info)
{
    bool chk;
    sci_iic_return_t ret;

#if (1 == SCI_IIC_CFG_PARAM_CHECKING_ENABLE)
    /* ---- CHECK ARGUMENTS ---- */
    ret = r_sci_iic_check_arguments(p_sci_iic_info, SCI_IIC_MODE_OPEN);

    if (SCI_IIC_SUCCESS != ret)
    {
        return ret;
    }
#endif

    /*  ---- SCI_IIC HARDWARE LOCK ---- */
    chk = R_BSP_HardwareLock(g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->hd_lock);

    if (false == chk)
    {
        /* Lock has already been acquired by another task. Needs to try again later. */
        return SCI_IIC_ERR_LOCK_FUNC;
    }

    /* ---- INITIALIZE SCI_IIC INTERNAL STATUS INFORMATION ---- */
    g_sci_iic_ChStatus[p_sci_iic_info->ch_no] = SCI_IIC_NO_INIT;
    p_sci_iic_info->dev_sts = SCI_IIC_NO_INIT;

    /* ---- INITIALIZE CHANNEL ---- */
    /* Calls the API function. */
    ret = sci_iic_open(p_sci_iic_info);

    return ret;
} /* End of function R_SCI_IIC_Open() */

/***********************************************************************************************************************
 * Function Name: sci_iic_open
 * Description  : Initializes the SCI_IIC driver.
 *              : Initializes the I/O register for SCI_IIC control.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 **********************************************************************************************************************/
static sci_iic_return_t sci_iic_open (sci_iic_info_t * p_sci_iic_info)
{
    sci_iic_return_t ret;
#if SCI_IIC_CFG_PORT_SETTING_PROCESSING
    volatile __evenaccess    const sci_iic_ch_rom_t * prom = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom;
#endif

    /* Updates the channel status. */
    sci_iic_set_ch_status(p_sci_iic_info, SCI_IIC_IDLE);

#if SCI_IIC_CFG_PORT_SETTING_PROCESSING
    /* Ports open setting :Sets ports to input mode, Ports input pull-up becomes "Off".*/
    /* Includes I/O register read operation at the end of the following function. */
    /* Set N-channel open-drain output */
    /* Disables SCI_IIC multi-function pin controller after setting SCL and SDA to Hi-z. */
    /* Includes I/O register read operation at the end of the following function. */
    /* Set Hi-z */
    r_sci_iic_io_open(prom->sscl_port_gr, prom->sscl_port_pin);
    r_sci_iic_mpc_setting(prom->sscl_port_gr, prom->sscl_port_pin, SCI_IIC_MPC_SSCL_INIT);
    r_sci_iic_io_open(prom->ssda_port_gr, prom->ssda_port_pin);
    r_sci_iic_mpc_setting(prom->ssda_port_gr, prom->ssda_port_pin, SCI_IIC_MPC_SSDA_INIT);
#endif

    /* Enables the IIC peripheral registers. */
    /* Includes I/O register read operation at the end of the following function. */
    r_sci_iic_power_on(p_sci_iic_info->ch_no);

    sci_iic_register_init_setting(p_sci_iic_info);

    /* Initialize variables to check bus busy. */
    g_sci_iic_handles[p_sci_iic_info->ch_no]->mode = SCI_IIC_STPREQ;

    /* Initialize API status on RAM */
    sci_iic_api_status_init(p_sci_iic_info);

    /* Sets the internal status. (Internal information initialization) */
    sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_NO_INIT);

    /* Initializes the IIC driver. */
    ret = sci_iic_func_table(SCI_IIC_EV_INIT, p_sci_iic_info);

    if (SCI_IIC_SUCCESS != ret)
    {
        /* Updates the channel status. */
        sci_iic_set_ch_status(p_sci_iic_info, SCI_IIC_NO_INIT);
    }

    /* Disables IIC. */
    sci_iic_disable(p_sci_iic_info);

    return ret;
} /* End of function sci_iic_open() */

/***********************************************************************************************************************
 * Function Name: R_SCI_IIC_MasterSend
 * Description  : Master transmission start processing.
 *                Generates the start condition. Starts the master transmission.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : SCI_IIC_SUCCESS                    ; Successful operation
 *              ; SCI_IIC_ERR_INVALID_CHAN           ; channel number invalid for part
 *              : SCI_IIC_ERR_INVALID_ARG            ; Parameter error
 *              : SCI_IIC_ERR_NO_INIT                ; Uninitialized state
 *              : SCI_IIC_ERR_BUS_BUSY               ; Bus busy
 *              ; SCI_IIC_ERR_OTHER                  ; Other error
 **********************************************************************************************************************/
sci_iic_return_t R_SCI_IIC_MasterSend (sci_iic_info_t * p_sci_iic_info)
{
    sci_iic_return_t ret;

#if (1 == SCI_IIC_CFG_PARAM_CHECKING_ENABLE)
    /* ---- CHECK ARGUMENTS ---- */
    ret = r_sci_iic_check_arguments(p_sci_iic_info, SCI_IIC_MODE_SEND);

    if (SCI_IIC_SUCCESS != ret)
    {
        return ret;
    }
#endif

    /* Calls the API function. */
    ret = sci_iic_master_send_receive(p_sci_iic_info, SCI_IIC_MODE_SEND);

    return ret;
} /* End of function R_SCI_IIC_MasterSend() */

/***********************************************************************************************************************
 * Function Name: R_SCI_IIC_MasterReceive
 * Description  : Master reception start processing.
 *                Generates the start condition. Starts the master reception.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : SCI_IIC_SUCCESS                    ; Successful operation
 *              ; SCI_IIC_ERR_INVALID_CHAN           ; channel number invalid for part
 *              : SCI_IIC_ERR_INVALID_ARG            ; Parameter error
 *              : SCI_IIC_ERR_NO_INIT                ; Uninitialized state
 *              : SCI_IIC_ERR_BUS_BUSY               ; Bus busy
 *              ; SCI_IIC_ERR_OTHER                  ; Other error
 **********************************************************************************************************************/
sci_iic_return_t R_SCI_IIC_MasterReceive (sci_iic_info_t * p_sci_iic_info)
{
    sci_iic_return_t ret;

#if (1 == SCI_IIC_CFG_PARAM_CHECKING_ENABLE)
    /* ---- CHECK ARGUMENTS ---- */
    ret = r_sci_iic_check_arguments(p_sci_iic_info, SCI_IIC_MODE_RECEIVE);

    if (SCI_IIC_SUCCESS != ret)
    {
        return ret;
    }
#endif

    /* cnt1st is NULL -> Master Receive */
    if (0 == p_sci_iic_info->cnt1st)
    {
        /* Master Receive */
        /* Calls the API function. */
        ret = sci_iic_master_send_receive(p_sci_iic_info, SCI_IIC_MODE_RECEIVE);
    }
    else
    {
        /* Master Transmission Reception */
        /* Calls the API function. */
        ret = sci_iic_master_send_receive(p_sci_iic_info, SCI_IIC_MODE_SEND_RECEIVE);
    }

    return ret;
} /* End of function R_SCI_IIC_MasterReceive() */

/***********************************************************************************************************************
 * Function Name: sci_iic_master_send_receive
 * Description  : Master transmission start processing.
 *                Generates the start condition. Starts the master transmission.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 *              ; sci_iic_api_mode_t api_mode        ; Internal Mode
 **********************************************************************************************************************/
static sci_iic_return_t sci_iic_master_send_receive (sci_iic_info_t * p_sci_iic_info, sci_iic_api_mode_t api_mode)
{
    sci_iic_return_t ret;

    /* Checks the channel status. */
    ret = sci_iic_check_chstatus_start(p_sci_iic_info);

    if (SCI_IIC_SUCCESS != ret)
    {
        return ret;
    }

    g_sci_iic_handles[p_sci_iic_info->ch_no]->psci_iic_info_ch = p_sci_iic_info;

    /* Updates the channel status. */
    sci_iic_set_ch_status(p_sci_iic_info, SCI_IIC_COMMUNICATION);

    /* ---- Initialize API status on RAM ---- */
    sci_iic_api_status_init(p_sci_iic_info);

    /* Sets the internal mode. */
    g_sci_iic_handles[p_sci_iic_info->ch_no]->api_mode = api_mode;

    /* Enables IIC. */
    sci_iic_enable(p_sci_iic_info);

    /* Generates the start condition. */
    ret = sci_iic_func_table(SCI_IIC_EV_GEN_START_COND, p_sci_iic_info);

    if (SCI_IIC_SUCCESS != ret)
    {
        /* Disables IIC. */
        sci_iic_disable(p_sci_iic_info);

        /* Updates the channel status. */
        sci_iic_set_ch_status(p_sci_iic_info, SCI_IIC_IDLE);
    }

    return ret;
} /* End of function sci_iic_master_send_receive() */

/***********************************************************************************************************************
 * Function Name: R_SCI_IIC_GetStatus
 * Description  : Returns the state of this module.
 *                Obtains the state of the SCI channel, which specified by the parameter, 
 *                by reading the register, variable, or others, and returns 
 *                the obtained state as 32-bit structure.
 * Arguments    : sci_iic_info_t * p_sci_iic_info        ; IIC Information
 *                sci_iic_mcu_status_t * p_sci_iic_status; The address to store the I2C state flag.
 * Return Value : SCI_IIC_SUCCESS                        ; Successful operation
 *              ; SCI_IIC_ERR_INVALID_CHAN               ; channel number invalid for part
 *              : SCI_IIC_ERR_INVALID_ARG                ; Parameter error
 **********************************************************************************************************************/
sci_iic_return_t R_SCI_IIC_GetStatus (sci_iic_info_t *p_sci_iic_info, sci_iic_mcu_status_t *p_sci_iic_status)
{
    sci_iic_return_t ret;

#if (1 == SCI_IIC_CFG_PARAM_CHECKING_ENABLE)
    /* ---- CHECK ARGUMENTS ---- */
    ret = r_sci_iic_check_arguments(p_sci_iic_info, SCI_IIC_MODE_GETSTATUS);

    if (SCI_IIC_SUCCESS != ret)
    {
        return ret;
    }
#endif

    /* Calls the API function. */
    ret = sci_iic_getstatus(p_sci_iic_info, p_sci_iic_status);

    return ret;
} /* End of function R_SCI_IIC_GetStatus() */

/***********************************************************************************************************************
 * Function Name: sci_iic_getstatus
 * Description  : Returns the state of this module.
 *                Obtains the state of the SCI channel, which specified by the parameter,
 *                by reading the register, variable, or others, and returns
 *                the obtained state as 32-bit structure.
 * Arguments    : sci_iic_info_t * p_sci_iic_info        ; IIC Information
 *                sci_iic_mcu_status_t * p_sci_iic_status; The address to store the I2C state flag.
 **********************************************************************************************************************/
static sci_iic_return_t sci_iic_getstatus (sci_iic_info_t * p_sci_iic_info, sci_iic_mcu_status_t * p_sci_iic_status)
{
    sci_iic_mcu_status_t sts_flag;
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;
    volatile uint8_t __evenaccess * const ppidr = SCI_IIC_PRV_PIDR_BASE_REG;
    volatile __evenaccess    const sci_iic_ch_rom_t * prom = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom;

    /* ---- Check Bus state ---- */
    if (SCI_IIC_STPREQ != g_sci_iic_handles[p_sci_iic_info->ch_no]->mode)
    {
        sts_flag.BIT.BSY = 1;
    }
    else
    {
        sts_flag.BIT.BSY = 0;
    }

    /* ---- Check Mode ---- */
    if (SCI_IIC_MODE_SEND == g_sci_iic_handles[p_sci_iic_info->ch_no]->api_mode)
    {
        /* Transmit */
        sts_flag.BIT.TRS = 1;
    }
    else if ((SCI_IIC_MODE_RECEIVE == g_sci_iic_handles[p_sci_iic_info->ch_no]->api_mode)
            || (SCI_IIC_MODE_SEND_RECEIVE == g_sci_iic_handles[p_sci_iic_info->ch_no]->api_mode))
    {
        /* Receive */
        sts_flag.BIT.TRS = 0;
    }
    else
    {
        sts_flag.BIT.TRS = 0;
    }

    /* ---- Check Event detection ---- */
    /* SISR - I2C Status Register 
     b7:b6 - Reserved - These bits are read as 0.
     b5:b4 - Reserved - The read value is undefined.
     b3    - Reserved - These bits are read as 0.
     b2    - Reserved - The read value is undefined.
     b1    - Reserved - These bits are read as 0.
     b0    - IICACKR  - ACK Reception Data Flag  */
    if (1 == pregs->SISR.BIT.IICACKR)
    {
        /* NACK translation detected */
        sts_flag.BIT.NACK = 1;
    }
    else
    {
        /* Not detected */
        sts_flag.BIT.NACK = 0;
    }

    /* Check SSCL Pin level */
    if (SCI_IIC_LOW == ((*(ppidr + prom->sscl_port_gr)) & (1U << prom->sscl_port_pin)))
    {
        sts_flag.BIT.SCLI = 0;
    }
    else
    {
        sts_flag.BIT.SCLI = 1;
    }

    /* Check SSDA Pin level */
    if (SCI_IIC_LOW == ((*(ppidr + prom->ssda_port_gr)) & (1U << prom->ssda_port_pin)))
    {
        sts_flag.BIT.SDAI = 0;
    }
    else
    {
        sts_flag.BIT.SDAI = 1;
    }

    sts_flag.BIT.rsv = 0;

    /* ---- Set the state ---- */
    (*p_sci_iic_status) = sts_flag;

    return SCI_IIC_SUCCESS;
} /* End of function sci_iic_getstatus() */

/***********************************************************************************************************************
 * Function Name: R_SCI_IIC_Control
 * Description  : This function is mainly used when a communication error occurs.
 *                Outputs control signals of the simple I2C mode. 
 *                Outputs conditions specified by the argument, NACK,
 *                and one-shot of the SSCL clock. Also resets the simple I2C mode settings.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 *                sci_iic_ctrl_ptn_t ctrl_ptn        ; Output Pattern
 * Return Value : SCI_IIC_SUCCESS                    ; Successful operation
 *              ; SCI_IIC_ERR_INVALID_CHAN           ; channel number invalid for part
 *              : SCI_IIC_ERR_INVALID_ARG            ; Parameter error
 *              : SCI_IIC_ERR_BUS_BUSY               ; Bus busy
 *              ; SCI_IIC_ERR_OTHER                  ; Other error
 **********************************************************************************************************************/
sci_iic_return_t R_SCI_IIC_Control (sci_iic_info_t * p_sci_iic_info, sci_iic_ctrl_ptn_t ctrl_ptn)
{
    sci_iic_return_t ret;

#if (1 == SCI_IIC_CFG_PARAM_CHECKING_ENABLE)
    /* ---- CHECK ARGUMENTS ---- */
    ret = r_sci_iic_check_arguments(p_sci_iic_info, SCI_IIC_MODE_CONTROL);

    if (SCI_IIC_SUCCESS != ret)
    {
        return ret;
    }

    if (SCI_IIC_GEN_ERR_CON == ctrl_ptn)
    {
        return SCI_IIC_ERR_INVALID_ARG;
    }

#endif

    /* Calls the API function. */
    ret = sci_iic_control(p_sci_iic_info, ctrl_ptn);

    return ret;
} /* End of function R_SCI_IIC_Control() */

/***********************************************************************************************************************
 * Function Name: sci_iic_control
 * Description  : This function is mainly used when a communication error occurs.
 *                Outputs control signals of the simple I2C mode.
 *                Outputs conditions specified by the argument, NACK,
 *                and one-shot of the SSCL clock. Also resets the simple I2C mode settings.
 * Arguments    : sci_iic_info_t * p_sci_iic_info   ; IIC Information
 *                sci_iic_ctrl_ptn_t ctrl_ptn       ; Output Pattern
 **********************************************************************************************************************/
static sci_iic_return_t sci_iic_control (sci_iic_info_t * p_sci_iic_info, sci_iic_ctrl_ptn_t ctrl_ptn)
{
    volatile uint32_t cnt;
    volatile uint32_t i;
    sci_iic_return_t ret = SCI_IIC_SUCCESS;
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;
    volatile uint8_t __evenaccess * const ppidr = SCI_IIC_PRV_PIDR_BASE_REG;
    volatile __evenaccess    const sci_iic_ch_rom_t * prom = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom;

    /* Enable Simple IIC mode */
    /* Enables SCI_IIC multi-function pin controller.*/
    sci_iic_iic_enable(p_sci_iic_info);

    /* Disable interrupt */
    r_sci_iic_int_disable(p_sci_iic_info);

    if ((0 != (ctrl_ptn & ((SCI_IIC_GEN_START_CON | SCI_IIC_GEN_RESTART_CON) | SCI_IIC_GEN_STOP_CON)))
            && (0 == (ctrl_ptn & ((SCI_IIC_GEN_SSDA_HI_Z | SCI_IIC_GEN_SSCL_ONESHOT) | SCI_IIC_GEN_RESET))))
    {
        /* ==== Check request output pattern ==== */
        /* ---- Generate the start condition ---- */
        if ((ctrl_ptn & SCI_IIC_GEN_START_CON) == SCI_IIC_GEN_START_CON)
        {

            if ((SCI_IIC_LOW == ((*(ppidr + prom->sscl_port_gr)) & (1U << prom->sscl_port_pin)))
                    || (SCI_IIC_LOW == ((*(ppidr + prom->ssda_port_gr)) & (1U << prom->ssda_port_pin))))
            {
                /* When BBSY bit is "1"(Bus busy) */
                ret = SCI_IIC_ERR_BUS_BUSY;
                return ret;
            }

            /* Generates the start condition. */
            /* SCR - Serial Control Register
             b7     TIE - Transmit Interrupt Enable
             b6     RIE - Receive Interrupt Enable
             b5     TE - Transmit Enable - Serial transmission is enabled.
             b4     RE - Receive Enable - Serial reception is enabled.
             b3     MPIE - Multi-Processor Interrupt Enable
             b2     TEIE  - Transmit End Interrupt Enable
             b1:b0  CKE - Clock Enable*/
            pregs->SCR.BYTE |= SCI_IIC_SCR_TE_RE;

            /* SIMR3 - I2C Mode Register 3
             b7:b6  IICSCLS - SSCL Output Select - Generate a start, restart, or stop condition.
             b5:b4  IICSDAS - SSDA Output Select - Generate a start, restart, or stop condition.
             b3     IICSTIF - Issuing of Start, Restart, or Stop Condition Completed Flag
             - There are no requests for generating conditions
             b0     IICSTAREQ - Start Condition Generation - A start condition is generated. */
            pregs->SIMR3.BYTE = SCI_IIC_ST_CON_GENERATED;

            /* Wait the request generation has been completed */
            i = SCI_IIC_CFG_BUS_CHECK_COUNTER;

            while (1 != pregs->SIMR3.BIT.IICSTIF)
            {
                /* Check Reply */
                if (0U < i)
                {
                    i--;
                }
                else
                {
                    /* When None Reply */
                    ret = SCI_IIC_ERR_OTHER;
                    return ret;
                }

            }

            pregs->SIMR3.BIT.IICSTIF = SCI_IIC_IICSTIF_CLEAR;

            while (SCI_IIC_IICSTIF_CLEAR != pregs->SIMR3.BIT.IICSTIF)
            {
                /* nothing to do */
            }

        }

        /* ---- Generate the restart condition ---- */
        if ((ctrl_ptn & SCI_IIC_GEN_RESTART_CON) == SCI_IIC_GEN_RESTART_CON)
        {
            /* Generates the restart condition. */
            /* SCR - Serial Control Register
             b5     TE - Transmit Enable - Serial transmission is enabled.
             b4     RE - Receive Enable - Serial reception is enabled. */
            pregs->SCR.BYTE |= SCI_IIC_SCR_TE_RE;

            /* SIMR3 - I2C Mode Register 3
             b1     IIRSTAREQ - Restart Condition Generation - A start condition is generated. */
            pregs->SIMR3.BYTE = SCI_IIC_RST_CON_GENERATED;

            /* Wait the request generation has been completed */
            i = SCI_IIC_CFG_BUS_CHECK_COUNTER;

            while (1 != pregs->SIMR3.BIT.IICSTIF)
            {
                /* Check Reply */
                if (0U < i)
                {
                    i--;
                }
                else
                {
                    /* When None Reply */
                    ret = SCI_IIC_ERR_OTHER;
                    return ret;
                }

            }

            pregs->SIMR3.BIT.IICSTIF = SCI_IIC_IICSTIF_CLEAR;

            while (SCI_IIC_IICSTIF_CLEAR != pregs->SIMR3.BIT.IICSTIF)
            {
                /* nothing to do */
            }

        }

        /* ---- Generate the stop condition ---- */
        if ((ctrl_ptn & SCI_IIC_GEN_STOP_CON) == SCI_IIC_GEN_STOP_CON)
        {
            /* Generates the stop condition. */
            /* SCR - Serial Control Register
             b5     TE - Transmit Enable - Serial transmission is enabled.
             b4     RE - Receive Enable - Serial reception is enabled. */
            pregs->SCR.BYTE |= SCI_IIC_SCR_TE_RE;

            /* SIMR3 - I2C Mode Register 3
             b2     IISTPREQ - Stop Condition Generation - A start condition is generated. */
            pregs->SIMR3.BYTE = SCI_IIC_SP_CON_GENERATED;

            /* Wait the request generation has been completed */
            i = SCI_IIC_CFG_BUS_CHECK_COUNTER;

            while (1 != pregs->SIMR3.BIT.IICSTIF)
            {
                /* Check Reply */
                if (0U < i)
                {
                    i--;
                }
                else
                {
                    /* When None Reply */
                    ret = SCI_IIC_ERR_OTHER;
                    return ret;
                }

            }

            pregs->SIMR3.BIT.IICSTIF = SCI_IIC_IICSTIF_CLEAR;

            while (SCI_IIC_IICSTIF_CLEAR != pregs->SIMR3.BIT.IICSTIF)
            {
                /* nothing to do */
            }

        }
    }
    else if ((0 != (ctrl_ptn & (SCI_IIC_GEN_SSDA_HI_Z | SCI_IIC_GEN_SSCL_ONESHOT)))
            && (0 == (ctrl_ptn & (((SCI_IIC_GEN_START_CON | SCI_IIC_GEN_RESTART_CON) | SCI_IIC_GEN_STOP_CON) |
            SCI_IIC_GEN_RESET))))
    {

        /* ---- Generate NACK ---- */
        if ((ctrl_ptn & SCI_IIC_GEN_SSDA_HI_Z) == SCI_IIC_GEN_SSDA_HI_Z)
        {
            /* Output NACK */
            /* SIMR3 - I2C Mode Register 3
             b7:b6  IICSCLS - SSCL Output Select - Place the SSCL pin in the high-impedance state.
             b5:b4  IICSDAS - SSDA Output Select - Place the SSDA pin in the high-impedance state. */
            pregs->SIMR3.BIT.IICSDAS = SCI_IIC_HI_Z_OUTPUT; /* SSDAn set Hi-z */
        }

        if (SCI_IIC_GEN_SSCL_ONESHOT == (ctrl_ptn & SCI_IIC_GEN_SSCL_ONESHOT))
        {
            /* ---- Generate OneShot of SSCL clock ---- */
            /* Output from Low to Hi-z */
            /* SIMR3 - I2C Mode Register 3
             b7:b6  IICSCLS - SSCL Output Select - Output the low level on the SSDA pin.
             b5:b4  IICSDAS - SSDA Output Select - Place the SSDA pin in the high-impedance state. */
            pregs->SIMR3.BIT.IICSCLS = SCI_IIC_LOW_OUTPUT;

            for (cnt = SCI_IIC_ONESHOT_WAIT; cnt > 0; cnt--)
            {
                /* nothing to do */
            }

            /* SIMR3 - I2C Mode Register 3
             b7:b6  IICSCLS - SSCL Output Select - Place the SSCL pin in the high-impedance state.*/
            pregs->SIMR3.BIT.IICSCLS = SCI_IIC_HI_Z_OUTPUT; /* SSCLn output  Hi-z*/

            for (cnt = SCI_IIC_ONESHOT_WAIT; cnt > 0; cnt--)
            {
                /* nothing to do */
            }

        }

    }
    else if ((0 != (ctrl_ptn & SCI_IIC_GEN_RESET))
            && (0 == (ctrl_ptn & ((((SCI_IIC_GEN_START_CON | SCI_IIC_GEN_RESTART_CON) | SCI_IIC_GEN_STOP_CON) |
            SCI_IIC_GEN_SSDA_HI_Z) | SCI_IIC_GEN_SSCL_ONESHOT))))
    {
        /* ---- Generate Reset ---- */
        ret = sci_iic_open(p_sci_iic_info);
    }
    else
    {
        /* parameter error */
        ret = SCI_IIC_ERR_INVALID_ARG;
    }

    /* Reset cnrl_ptn */
    ctrl_ptn = 0U;

    return ret;
} /* End of function sci_iic_control() */

/***********************************************************************************************************************
 * Function Name: R_SCI_IIC_Close
 * Description  : Resets the SCI_IIC driver.
 *              : After called the processing, channel status becomes "SCI_IIC_NO_INIT".
 *              : When starts the communication again, please call an initialization processing.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : SCI_IIC_SUCCESS                    ; Successful operation
 *              ; SCI_IIC_ERR_INVALID_CHAN           ; channel number invalid for part
 *              : SCI_IIC_ERR_INVALID_ARG            ; Parameter error
 **********************************************************************************************************************/
sci_iic_return_t R_SCI_IIC_Close (sci_iic_info_t * p_sci_iic_info)
{
    bool chk;

#if (1 == SCI_IIC_CFG_PARAM_CHECKING_ENABLE)
    sci_iic_return_t ret;

    /* ---- CHECK ARGUMENTS ---- */
    ret = r_sci_iic_check_arguments(p_sci_iic_info, SCI_IIC_MODE_CLOSE);

    if (SCI_IIC_SUCCESS != ret)
    {
        return ret;
    }
#endif

    /* Calls the API function. */
    sci_iic_close(p_sci_iic_info);

    /* ---- Hardware Unlock ---- */
    chk = R_BSP_HardwareUnlock(g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->hd_lock);

    if (false == chk)
    {
        return SCI_IIC_ERR_LOCK_FUNC;
    }

    return SCI_IIC_SUCCESS;
} /* End of function R_SCI_IIC_Close() */

/***********************************************************************************************************************
 * Function Name: sci_iic_close
 * Description  : Resets the SCI_IIC driver.
 *              : After called the processing, channel status becomes "SCI_IIC_NO_INIT".
 *              : When starts the communication again, please call an initialization processing.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 **********************************************************************************************************************/
static void sci_iic_close (sci_iic_info_t * p_sci_iic_info)
{
    /* Disables IIC. */
    sci_iic_disable(p_sci_iic_info);

    /* Reset SCI I/O register. */
    r_sci_iic_clear_io_register(p_sci_iic_info);

    /* Updates the channel status. */
    sci_iic_set_ch_status(p_sci_iic_info, SCI_IIC_NO_INIT);

    r_sci_iic_power_off(p_sci_iic_info->ch_no);
} /* End of function sci_iic_close() */

/***********************************************************************************************************************
 * Function Name: R_SCI_IIC_GetVersion
 * Description  : Returns the version of this module. The version number is 
 *                encoded such that the top two bytes are the major version
 *                number and the bottom two bytes are the minor version number.
 * Arguments    : none
 * Return Value : version number
 **********************************************************************************************************************/
R_PRAGMA_INLINE(R_SCI_IIC_GetVersion)
uint32_t R_SCI_IIC_GetVersion (void)
{
    uint32_t const version = (SCI_IIC_VERSION_MAJOR << 16) | SCI_IIC_VERSION_MINOR;

    return version;
} /* End of function R_SCI_IIC_GetVersion() */

/***********************************************************************************************************************
 * Function Name: r_sci_iic_advance
 * Description  : Advances the IIC communication.
 *              : The return value shows the communication result. Refer to the return value.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : SCI_IIC_SUCCESS                    ; Successful operation
 *              ; SCI_IIC_ERR_INVALID_CHAN           ; channel number invalid for part
 *              : SCI_IIC_ERR_INVALID_ARG            ; Parameter error
 *              : SCI_IIC_ERR_BUS_BUSY               ; Bus busy
 *              ; SCI_IIC_ERR_OTHER                  ; Other error
 **********************************************************************************************************************/
sci_iic_return_t r_sci_iic_advance (sci_iic_info_t * p_sci_iic_info)
{
    sci_iic_return_t ret;

    /* Parameter check */
    if (NULL == p_sci_iic_info)
    {
        ret = SCI_IIC_ERR_INVALID_ARG;
    }
    else
    {
        /* Calls the API function. */
        ret = sci_iic_advance(p_sci_iic_info);
    }

    return ret;
} /* End of function r_sci_iic_advance() */

/***********************************************************************************************************************
 * Function Name: sci_iic_advance
 * Description  : Advances the IIC communication.
 *              : The return value shows the communication result. Refer to the return value.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 **********************************************************************************************************************/
static sci_iic_return_t sci_iic_advance (sci_iic_info_t * p_sci_iic_info)
{
    sci_iic_return_t ret;
    bool boolret;
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;

    /* Checks the channel status. */
    ret = sci_iic_check_chstatus_advance(p_sci_iic_info);

    if (SCI_IIC_SUCCESS != ret)
    {
        return ret;
    }

    /* Event happened? */
    if (SCI_IIC_EV_INIT != g_sci_iic_handles[p_sci_iic_info->ch_no]->api_event)
    {
        /* IIC communication processing */
        ret = sci_iic_func_table(g_sci_iic_handles[p_sci_iic_info->ch_no]->api_event, p_sci_iic_info);

        if (SCI_IIC_SUCCESS == ret)
        {
            /* status value? */
            switch (g_sci_iic_ChStatus[p_sci_iic_info->ch_no])
            {
                /* Advances communication. (Not finished) */
                case SCI_IIC_COMMUNICATION :

                    /* Do Nothing */

                break;

                    /* Finished communication. */
                case SCI_IIC_FINISH :

                    /* Disables IIC. */
                    sci_iic_disable(p_sci_iic_info);

                    /* Place the SSCL pin & SSDA pin in the high-impedance state */
                    /* SIMR3 - I2C Mode Register 3
                     b7:b6  IICSCLS - SSCL Output Select - Place the SSCL pin in the high-impedance state.
                     b5:b4  IICSDAS - SSDA Output Select - Place the SSDA pin in the high-impedance state.
                     b3     IICSTIF - Issuing of Start, Restart, or Stop Condition Completed Flag
                     - There are no requests for generating conditions */
                    pregs->SIMR3.BIT.IICSTIF = SCI_IIC_IICSTIF_CLEAR;
                    pregs->SIMR3.BIT.IICSCLS = SCI_IIC_HI_Z_OUTPUT;
                    pregs->SIMR3.BIT.IICSDAS = SCI_IIC_HI_Z_OUTPUT;

                    while (SCI_IIC_IICSTIF_CLEAR != pregs->SIMR3.BIT.IICSTIF)
                    {
                        /* nothing to do */
                    }

                    /* Checks the callback function. */
                    if (NULL != g_sci_iic_handles[p_sci_iic_info->ch_no]->psci_iic_info_ch->callbackfunc)
                    {
                        /* Calls the callback function. */
                        g_sci_iic_handles[p_sci_iic_info->ch_no]->psci_iic_info_ch->callbackfunc();
                    }
                    else
                    {
                        return SCI_IIC_ERR_INVALID_ARG;
                    }

                break;

                    /* NACK is occurred. */
                case SCI_IIC_NACK :

                    /* Waits the stop condition generation. */
                    boolret = sci_iic_stop_cond_wait(p_sci_iic_info);

                    /* Disables IIC. */
                    sci_iic_disable(p_sci_iic_info);

                    /* Place the SSCL pin & SSDA pin in the high-impedance state */
                    /* SIMR3 - I2C Mode Register 3
                     b7:b6  IICSCLS - SSCL Output Select - Place the SSCL pin in the high-impedance state.
                     b5:b4  IICSDAS - SSDA Output Select - Place the SSDA pin in the high-impedance state.
                     b3     IICSTIF - Issuing of Start, Restart, or Stop Condition Completed Flag
                     - There are no requests for generating conditions */
                    pregs->SIMR3.BIT.IICSTIF = SCI_IIC_IICSTIF_CLEAR;
                    pregs->SIMR3.BIT.IICSCLS = SCI_IIC_HI_Z_OUTPUT;
                    pregs->SIMR3.BIT.IICSDAS = SCI_IIC_HI_Z_OUTPUT;

                    while (SCI_IIC_IICSTIF_CLEAR != pregs->SIMR3.BIT.IICSTIF)
                    {
                        /* nothing to do */
                    }

                    /* Check the stop condition generation. */
                    if (SCI_IIC_TRUE == boolret)
                    {
                        ret = SCI_IIC_SUCCESS;
                    }
                    else
                    {
                        /* Updates the channel status. */
                        sci_iic_set_ch_status(p_sci_iic_info, SCI_IIC_ERROR);

                        ret = SCI_IIC_ERR_BUS_BUSY;
                    }

                    /* Checks callback function. */
                    if (NULL != g_sci_iic_handles[p_sci_iic_info->ch_no]->psci_iic_info_ch->callbackfunc)
                    {
                        /* Calls the callback function. */
                        g_sci_iic_handles[p_sci_iic_info->ch_no]->psci_iic_info_ch->callbackfunc();
                    }
                    else
                    {
                        return SCI_IIC_ERR_INVALID_ARG;
                    }

                break;

                default :

                    /* Checks the callback function. */
                    if (NULL != g_sci_iic_handles[p_sci_iic_info->ch_no]->psci_iic_info_ch->callbackfunc)
                    {
                        /* Calls the callback function. */
                        g_sci_iic_handles[p_sci_iic_info->ch_no]->psci_iic_info_ch->callbackfunc();
                    }
                    else
                    {
                        return SCI_IIC_ERR_INVALID_ARG;
                    }

                break;
            }
            return ret;
        }
        else
        {
            return SCI_IIC_ERR_OTHER;
        }
    }
    else
    {
        /* Event nothing. */
        /* During communication */
        return SCI_IIC_ERR_OTHER;
    }
} /* End of function sci_iic_advance() */

/***********************************************************************************************************************
 * Function Name: sci_iic_func_table
 * Description  : IIC Protocol Execution Processing. Executes the function which set in "pfunc".
 * Arguments    : sci_iic_api_event_t event          ; Event
 *              : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : Refer to the each calling function.
 **********************************************************************************************************************/
static sci_iic_return_t sci_iic_func_table (sci_iic_api_event_t event, sci_iic_info_t * p_sci_iic_info)
{
    sci_iic_return_t ret = SCI_IIC_ERR_OTHER;
    sci_iic_return_t (*pFunc) (sci_iic_info_t *);
    sci_iic_api_status_t n_status;

    /* Acquires a now state. */
    n_status = g_sci_iic_handles[p_sci_iic_info->ch_no]->api_n_status;

    /* Checks the parameter. */
    if ((SCI_IIC_STS_MAX > n_status) && (SCI_IIC_EV_MAX > event))
    {
        /* Checks the appointed function. */
        if (NULL != gc_sci_iic_mtx_tbl[n_status][event].proc)
        {
            /* Sets the function. */
            pFunc = gc_sci_iic_mtx_tbl[n_status][event].proc;

            /* Event flag initialization. */
            g_sci_iic_handles[p_sci_iic_info->ch_no]->api_event = SCI_IIC_EV_INIT;

            /* Calls the status transition function. */
            ret = (*pFunc)(p_sci_iic_info);
        }
        else
        {
            /* Appointed function error */
            ret = SCI_IIC_ERR_OTHER;
        }
    }
    else
    {
        /* Appointed function error */
        ret = SCI_IIC_ERR_OTHER;
    }

    return ret;
} /* End of function sci_iic_func_table() */

/***********************************************************************************************************************
 * Function Name: sci_iic_init_driver
 * Description  : Initialize IIC Driver Processing (Func0)
 *              : Initializes the IIC registers. Sets the internal status.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : SCI_IIC_SUCCESS                    ; Successful operation, idle state
 **********************************************************************************************************************/
static sci_iic_return_t sci_iic_init_driver (sci_iic_info_t * p_sci_iic_info)
{
    /* Initializes the IIC registers. */
    /* Includes I/O register read operation at the end of the following function. */
    r_sci_iic_init_io_register(p_sci_iic_info);

    /* Sets the internal status. */
    sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_IDLE);

    return SCI_IIC_SUCCESS;
} /* End of function sci_iic_init_driver() */

/***********************************************************************************************************************
 * Function Name: sci_iic_generate_start_cond
 * Description  : Generate Start Condition Processing (Func1).
 *                Checks bus busy. Sets the internal status. Generates the start condition.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : SCI_IIC_SUCCESS                    ; Successful operation, communication state
 *              : SCI_IIC_ERR_BUS_BUSY               ; None reply error
 **********************************************************************************************************************/
static sci_iic_return_t sci_iic_generate_start_cond (sci_iic_info_t * p_sci_iic_info)
{
    sci_iic_return_t ret = SCI_IIC_SUCCESS;
    volatile uint8_t __evenaccess * const ppidr = SCI_IIC_PRV_PIDR_BASE_REG;
    volatile __evenaccess    const sci_iic_ch_rom_t * prom = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom;

    /* Check Bus busy(SSDA,SSCL pin level) */
    if ((SCI_IIC_LOW == ((*(ppidr + prom->sscl_port_gr)) & (1U << prom->sscl_port_pin)))
            || (SCI_IIC_LOW == ((*(ppidr + prom->ssda_port_gr)) & (1U << prom->ssda_port_pin))))
    {
        return SCI_IIC_ERR_BUS_BUSY;
    }

    /* Sets the internal status. */
    sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_ST_COND_WAIT);

    /* Generates the start condition.*/
    sci_iic_start_cond_generate(p_sci_iic_info);

    return ret;
} /* End of function sci_iic_generate_start_cond() */

/***********************************************************************************************************************
 * Function Name: sci_iic_after_gen_start_cond
 * Description  : After Start Condition Generation Processing (Func2).
 *                Performs one of the following processing according to the state.
 *                Transmits the slave address for writing.
 *                Transmits the slave address for reading.
 *                Generates the stop condition.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : SCI_IIC_SUCCESS                    ; Successful operation, communication state
 *              : SCI_IIC_ERR_OTHER                  ; Other error
 **********************************************************************************************************************/
static sci_iic_return_t sci_iic_after_gen_start_cond (sci_iic_info_t * p_sci_iic_info)
{
    sci_iic_return_t ret = SCI_IIC_SUCCESS;
    uint8_t buf_send_data;
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;

    switch (g_sci_iic_handles[p_sci_iic_info->ch_no]->api_mode)
    {
        case SCI_IIC_MODE_SEND :
        case SCI_IIC_MODE_SEND_RECEIVE :

            /* Checks the previous status. */
            switch (g_sci_iic_handles[p_sci_iic_info->ch_no]->api_b_status)
            {
                /* The previous status is idle. */
                case SCI_IIC_STS_IDLE :

                    /* Is the slave address pointer set? */
                    if ((uint8_t *) FIT_NO_PTR == p_sci_iic_info->p_slv_adr) /* Pattern 4 of Master Write  */
                    {
                        /* Sets the internal status. */
                        sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_SP_COND_WAIT);

                        /* Generates the stop condition. */
                        sci_iic_stop_cond_generate(p_sci_iic_info);

                        return ret;
                    }

                    /* Sets a write code. */
                    buf_send_data = (uint8_t) ((*p_sci_iic_info->p_slv_adr) << 1);
                    buf_send_data &= SCI_IIC_W_CODE;

                    /* Sets the internal status. */
                    sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_SEND_SLVADR_W_WAIT);

                    /* Enables the IIC bus interrupt. */
                    /* SIMR3 - I2C Mode Register 3
                     b7:b6  IICSCLS - SSCL Output Select - Serial clock output.
                     b5:b4  IICSDAS - SSDA Output Select - Serial data output.
                     b3     IICSTIF - Issuing of Start, Restart, or Stop Condition Completed Flag
                     - There are no requests for generating conditions */
                    pregs->SIMR3.BIT.IICSTIF = SCI_IIC_IICSTIF_CLEAR;
                    pregs->SIMR3.BIT.IICSCLS = SCI_IIC_CLOCK_OUTPUT;
                    pregs->SIMR3.BIT.IICSDAS = SCI_IIC_CLOCK_OUTPUT;

                    while (SCI_IIC_IICSTIF_CLEAR != pregs->SIMR3.BIT.IICSTIF)
                    {
                        /* nothing to do */
                    }

                    sci_iic_set_sending_data(p_sci_iic_info, &buf_send_data);

                break;

                    /* Previous status is data transfer completion waiting status. */
                case SCI_IIC_STS_SEND_DATA_WAIT :

                    /* Sets a read code. */
                    buf_send_data = (uint8_t) ((*p_sci_iic_info->p_slv_adr) << 1);
                    buf_send_data |= SCI_IIC_R_CODE;

                    /* Sets the internal status. */
                    sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_SEND_SLVADR_R_WAIT);

                    /* Enables the IIC bus interrupt. */
                    /* SIMR3 - I2C Mode Register 3
                     b7:b6  IICSCLS - SSCL Output Select - Serial clock output.
                     b5:b4  IICSDAS - SSDA Output Select - Serial data output.
                     b3     IICSTIF - Issuing of Start, Restart, or Stop Condition Completed Flag
                     - There are no requests for generating conditions */
                    pregs->SIMR3.BIT.IICSTIF = SCI_IIC_IICSTIF_CLEAR;
                    pregs->SIMR3.BIT.IICSCLS = SCI_IIC_CLOCK_OUTPUT;
                    pregs->SIMR3.BIT.IICSDAS = SCI_IIC_CLOCK_OUTPUT;

                    while (SCI_IIC_IICSTIF_CLEAR != pregs->SIMR3.BIT.IICSTIF)
                    {
                        /* nothing to do */
                    }

                    /* Transmits the slave address. */
                    sci_iic_set_sending_data(p_sci_iic_info, &buf_send_data);
                break;

                default :

                    /* None status. */
                    ret = SCI_IIC_ERR_OTHER;

                break;
            }
            return ret;

        case SCI_IIC_MODE_RECEIVE :

            /* Sets a read code. */
            buf_send_data = (uint8_t) ((*p_sci_iic_info->p_slv_adr) << 1);
            buf_send_data |= SCI_IIC_R_CODE;

            /* Sets the internal status. */
            sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_SEND_SLVADR_R_WAIT);

            /* Enables the IIC bus interrupt. */
            /* SIMR3 - I2C Mode Register 3
             b7:b6  IICSCLS - SSCL Output Select - Serial clock output.
             b5:b4  IICSDAS - SSDA Output Select - Serial data output.
             b3     IICSTIF - Issuing of Start, Restart, or Stop Condition Completed Flag
             - There are no requests for generating conditions */
            pregs->SIMR3.BIT.IICSTIF = SCI_IIC_IICSTIF_CLEAR;
            pregs->SIMR3.BIT.IICSCLS = SCI_IIC_CLOCK_OUTPUT;
            pregs->SIMR3.BIT.IICSDAS = SCI_IIC_CLOCK_OUTPUT;

            while (SCI_IIC_IICSTIF_CLEAR != pregs->SIMR3.BIT.IICSTIF)
            {
                /* nothing to do */
            }

            /* Transmits the slave address. */
            sci_iic_set_sending_data(p_sci_iic_info, &buf_send_data);
        break;

        default :
            ret = SCI_IIC_ERR_OTHER;
        break;
    }
    return ret;
} /* End of function sci_iic_after_gen_start_cond() */

/***********************************************************************************************************************
 * Function Name: sci_iic_after_send_slvadr
 * Description  : After Slave Address Transmission Processing (Func3).
 *                Performs one of the following processing according to the state.
 *                Transmits a data. Generates the stop condition. Receives a data.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : SCI_IIC_SUCCESS                    ; Successful operation, communication state
 *              : SCI_IIC_ERR_OTHER                  ; Other error
 **********************************************************************************************************************/
static sci_iic_return_t sci_iic_after_send_slvadr (sci_iic_info_t * p_sci_iic_info)
{
    sci_iic_return_t ret = SCI_IIC_SUCCESS;
    volatile uint8_t uctmp = 0U;
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;

    switch (g_sci_iic_handles[p_sci_iic_info->ch_no]->api_mode)
    {
        case SCI_IIC_MODE_SEND : /* MasterSend  */

            /* Are 1st data and 2nd data pointer set? */
            /* Pattern 3 of Master Write    */
            if (((uint8_t *) FIT_NO_PTR == p_sci_iic_info->p_data1st)
                    && ((uint8_t *) FIT_NO_PTR == p_sci_iic_info->p_data2nd))
            {
                /* Sets the internal status. */
                sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_SP_COND_WAIT);

                /* Generates the stop condition. */
                sci_iic_stop_cond_generate(p_sci_iic_info);

                return ret;
            }

            /* Is 1st data pointer set? */
            if (((uint8_t *) FIT_NO_PTR != p_sci_iic_info->p_data1st)
                    && ((uint8_t *) FIT_NO_PTR != p_sci_iic_info->p_data2nd))
            {
                /* Pattern 1 of Master Write */
                /* Sets the internal status. */
                sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_SEND_DATA_WAIT);

                /* 1st data counter = 0?  */
                if (0U != p_sci_iic_info->cnt1st)
                {
                    /* Transmits a 1st data. */
                    sci_iic_set_sending_data(p_sci_iic_info, p_sci_iic_info->p_data1st);

                    /* Decreases the 1st data counter.*/
                    p_sci_iic_info->cnt1st--;

                    /* Increases the 1st data buffer pointer. */
                    p_sci_iic_info->p_data1st++;
                }
                else
                {
                    return SCI_IIC_ERR_OTHER;
                }
            }
            else if (((uint8_t *) FIT_NO_PTR == p_sci_iic_info->p_data1st)
                    && ((uint8_t *) FIT_NO_PTR != p_sci_iic_info->p_data2nd))
            {
                /* Pattern 2 of Master Write */
                /* Sets the internal status. */
                sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_SEND_DATA_WAIT);

                /* 2nd data counter = 0?  */
                if (0U != p_sci_iic_info->cnt2nd)
                {
                    /* Transmits a 2nd data. */
                    sci_iic_set_sending_data(p_sci_iic_info, p_sci_iic_info->p_data2nd);

                    /* Decreases the 2nd data counter. */
                    p_sci_iic_info->cnt2nd--;

                    /* Increases the 2nd data buffer pointer. */
                    p_sci_iic_info->p_data2nd++;

                }
                else
                {
                    return SCI_IIC_ERR_OTHER;
                }
            }
            else
            {
                return SCI_IIC_ERR_OTHER;
            }

        break;

        case SCI_IIC_MODE_RECEIVE :

            /* Sets the internal status. */
            sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_RECEIVE_DATA_WAIT);

            if (1U >= p_sci_iic_info->cnt2nd)
            {
                /* Processing before receiving the "last byte - 1byte" in RX. */
                /* SIMR2 - I2C Mode Register 2
                 b7:b6  Reserved - These bits are read as 0.
                 b5     IICACKT - ACK Transmission Data - ACK transmission.
                 b4:b2  Reserved - These bits are read as 0.
                 b1     IICCSC - Clock Synchronization
                 b0     IICINTM - I2C Interrupt Mode Select - NACK translation */
                pregs->SIMR2.BIT.IICACKT = SCI_IIC_NACK_TRANS;
            }
            else
            {
                /* ACK transmission  */
                /* SIMR2 - I2C Mode Register 2
                 b0     IICINTM - I2C Interrupt Mode Select - ACK translation */
                pregs->SIMR2.BIT.IICACKT = SCI_IIC_ACK_TRANS;
            }

            /* Performs dummy read. */
            uctmp = sci_iic_get_receiving_data(p_sci_iic_info);

            p_sci_iic_info->cnt2nd--;

            /* Write Dummy date */
            /* TDR - Transmit Data Register */
            pregs->TDR = 0xff;

        break;

        case SCI_IIC_MODE_SEND_RECEIVE :

            /* Now status? */
            switch (g_sci_iic_handles[p_sci_iic_info->ch_no]->api_n_status)
            {

                case SCI_IIC_STS_SEND_SLVADR_W_WAIT :

                    /* Sets the internal status. */
                    sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_SEND_DATA_WAIT);

                    /* Transmits a 1st data. */
                    sci_iic_set_sending_data(p_sci_iic_info, p_sci_iic_info->p_data1st);

                    /* Decreases the 1st data counter. */
                    p_sci_iic_info->cnt1st--;

                    /* Increases the 1st Data buffer pointer. */
                    p_sci_iic_info->p_data1st++;

                break;

                case SCI_IIC_STS_SEND_SLVADR_R_WAIT :

                    if (1U >= p_sci_iic_info->cnt2nd)
                    {
                        /* Processing before receiving the "last byte - 1byte" in RX. */
                        /* SIMR2 - I2C Mode Register 2
                         b7:b6  Reserved - These bits are read as 0.
                         b5     IICACKT - ACK Transmission Data - ACK transmission.
                         b4:b2  Reserved - These bits are read as 0.
                         b1     IICCSC - Clock Synchronization
                         b0     IICINTM - I2C Interrupt Mode Select - NACK translation */
                        pregs->SIMR2.BIT.IICACKT = SCI_IIC_NACK_TRANS;
                    }
                    else
                    {
                        /* ACK transmission  */
                        /* SIMR2 - I2C Mode Register 2
                         b0     IICINTM - I2C Interrupt Mode Select - ACK translation */
                        pregs->SIMR2.BIT.IICACKT = SCI_IIC_ACK_TRANS;
                    }

                    /* Sets the internal status. */
                    sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_RECEIVE_DATA_WAIT);

                    /* Performs dummy read. */
                    uctmp = sci_iic_get_receiving_data(p_sci_iic_info);

                    p_sci_iic_info->cnt2nd--;

                    /* Write Dummy date */
                    /* TDR - Transmit Data Register */
                    pregs->TDR = 0xff;

                break;

                default :

                    ret = SCI_IIC_ERR_OTHER;

                break;
            }

        break;

        default :

            ret = SCI_IIC_ERR_OTHER;

        break;

    }

    return ret;
} /* End of function sci_iic_after_send_slvadr() */

/***********************************************************************************************************************
 * Function Name: sci_iic_write_data_sending
 * Description  : After Transmitting Data Processing (Func4).
 *                Performs one of the following processing  according to the state.
 *                Transmits a data. Generates stop condition. Generates restart condition.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : SCI_IIC_SUCCESS                    ; Successful operation, communication state
 *              : SCI_IIC_ERR_OTHER                  ; Other error
 **********************************************************************************************************************/
static sci_iic_return_t sci_iic_write_data_sending (sci_iic_info_t * p_sci_iic_info)
{
    sci_iic_return_t ret = SCI_IIC_SUCCESS;

    /* IIC mode? */
    switch (g_sci_iic_handles[p_sci_iic_info->ch_no]->api_mode)
    {
        case SCI_IIC_MODE_SEND :

            /* Is 1st data pointer set? */
            if ((uint8_t *) FIT_NO_PTR != p_sci_iic_info->p_data1st)
            {
                /* 1st data counter = 0?  */
                if (0U != p_sci_iic_info->cnt1st) /* Pattern 1 of Master Write  */
                {
                    /* Transmits a 1st data. */
                    sci_iic_set_sending_data(p_sci_iic_info, p_sci_iic_info->p_data1st);

                    /* Decreases the 1st data counter. */
                    p_sci_iic_info->cnt1st--;

                    /* Increases the 1st data buffer pointer. */
                    p_sci_iic_info->p_data1st++;

                    return ret;
                }
            }

            /* Is 2nd data pointer set? */
            if ((uint8_t *) FIT_NO_PTR != p_sci_iic_info->p_data2nd)
            {
                /* 2nd data counter = 0? */
                if (0U != p_sci_iic_info->cnt2nd) /* Pattern 2 of Master Write */
                {
                    /* Transmits a 2nd data. */
                    sci_iic_set_sending_data(p_sci_iic_info, p_sci_iic_info->p_data2nd);

                    /* Decreases the 2nd data counter. */
                    p_sci_iic_info->cnt2nd--;

                    /* Increases the 2nd data buffer pointer. */
                    p_sci_iic_info->p_data2nd++;

                    return ret;
                }
            }

            /* Sets the internal status. */
            sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_SP_COND_WAIT);

            /* Enables the IIC bus interrupt.*/
            sci_iic_int_icier_setting(p_sci_iic_info, SCI_IIC_SCR_ENABLE);

            /* Generates the stop condition. */
            sci_iic_stop_cond_generate(p_sci_iic_info);
        break;

        case SCI_IIC_MODE_SEND_RECEIVE :

            /* Is 1st data pointer set? */
            if (0U != p_sci_iic_info->cnt1st)
            {
                /* Transmits a 1st data. */
                sci_iic_set_sending_data(p_sci_iic_info, p_sci_iic_info->p_data1st);

                /* Decreases the 1st data counter. */
                p_sci_iic_info->cnt1st--;

                /* Increases the 1st data buffer pointer. */
                p_sci_iic_info->p_data1st++;

                return ret;
            }

            /* Sets the internal status. */
            sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_ST_COND_WAIT);

            /* Restarts the condition generation */
            sci_iic_re_start_cond_generate(p_sci_iic_info);

        break;

        default :
            ret = SCI_IIC_ERR_OTHER;
        break;
    }

    return ret;
} /* End of function sci_iic_write_data_sending() */

/***********************************************************************************************************************
 * Function Name: sci_iic_read_data_receiving
 * Description  : After Receiving Data Processing (Func5).
 *                Performs one of the following processing  according to the state.
 *                Receives a data. Generates stop condition.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : SCI_IIC_SUCCESS                    ; Successful operation, communication state
 **********************************************************************************************************************/
static sci_iic_return_t sci_iic_read_data_receiving (sci_iic_info_t * p_sci_iic_info)
{
    sci_iic_return_t ret = SCI_IIC_SUCCESS;
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;

    if (1U >= p_sci_iic_info->cnt2nd)
    {
        /* Processing before receiving the "last byte - 1byte" in RX. */
        /* SIMR2 - I2C Mode Register 2
         b5     IICACKT - ACK Transmission Data - ACK transmission.*/
        pregs->SIMR2.BIT.IICACKT = SCI_IIC_NACK_TRANS;
    }

    if (0U >= p_sci_iic_info->cnt2nd)
    {
        /* Stores the received data. */
        (*p_sci_iic_info->p_data2nd) = sci_iic_get_receiving_data(p_sci_iic_info);

        /* Sets the internal status. */
        sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_SP_COND_WAIT);

        /* Generates the stop condition. */
        sci_iic_stop_cond_generate(p_sci_iic_info);
    }
    else
    {
        /* Stores the received data. */
        (*p_sci_iic_info->p_data2nd) = sci_iic_get_receiving_data(p_sci_iic_info);
        p_sci_iic_info->cnt2nd--; /* Decreases the receiving data counter.  */
        p_sci_iic_info->p_data2nd++; /* Increases the receiving data buffer pointer. */

        /* Write Dummy date */
        /* TDR - Transmit Data Register */
        pregs->TDR = 0xff;
    }

    return ret;
} /* End of function sci_iic_read_data_receiving() */

/***********************************************************************************************************************
 * Function Name: sci_iic_release
 * Description  : After Generating Stop Condition Processing (Func6).
 *                Checks bus busy.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : SCI_IIC_SUCCESS                    ; Successful operation, finished communication and idle state
 **********************************************************************************************************************/
static sci_iic_return_t sci_iic_release (sci_iic_info_t * p_sci_iic_info)
{
    sci_iic_return_t ret = SCI_IIC_SUCCESS;

    /* Updates the channel status. */
    sci_iic_set_ch_status(p_sci_iic_info, SCI_IIC_FINISH);

    return ret;
} /* End of function sci_iic_release() */

/***********************************************************************************************************************
 * Function Name: sci_iic_nack
 * Description  : Nack Error Processing (Func7).
 *                Generates the stop condition and returns SCI_IIC_NACK.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : SCI_IIC_SUCCESS                    ; Detected NACK, finished communication and idle state
 **********************************************************************************************************************/
static sci_iic_return_t sci_iic_nack (sci_iic_info_t * p_sci_iic_info)
{
    sci_iic_return_t ret = SCI_IIC_SUCCESS;
    volatile uint8_t uctmp = 0U;

    /* Sets the internal status. */
    sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_SP_COND_WAIT);

    /* Enables the IIC bus interrupt. */
    sci_iic_int_icier_setting(p_sci_iic_info, SCI_IIC_SCR_ENABLE);

    /* Generates the stop condition. */
    sci_iic_stop_cond_generate(p_sci_iic_info);

    /* Checks the internal mode. */
    if ((SCI_IIC_MODE_RECEIVE == g_sci_iic_handles[p_sci_iic_info->ch_no]->api_mode)
            || (SCI_IIC_MODE_SEND_RECEIVE == g_sci_iic_handles[p_sci_iic_info->ch_no]->api_mode))
    {
        /* Dummy Read */
        /* Refer to the RX User's Manual. */
        uctmp = sci_iic_get_receiving_data(p_sci_iic_info);
    }

    /* Updates the channel status. */
    sci_iic_set_ch_status(p_sci_iic_info, SCI_IIC_NACK);

    return ret;
} /* End of function sci_iic_nack() */

/***********************************************************************************************************************
 * Function Name: sci_iic_api_status_init
 * Description  : Initializes RAM.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void sci_iic_api_status_init (sci_iic_info_t * p_sci_iic_info)
{
    /* Clears the event flag. */
    g_sci_iic_handles[p_sci_iic_info->ch_no]->api_event = SCI_IIC_EV_INIT;

    /* Sets the mode to SCI_IIC_MODE_NONE. */
    g_sci_iic_handles[p_sci_iic_info->ch_no]->api_mode = SCI_IIC_MODE_NONE;
} /* End of function sci_iic_api_status_init() */

/***********************************************************************************************************************
 * Function Name: sci_iic_set_internal_status
 * Description  : Update Internal Status Processing.
 *                Updates the now status and the previous status.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 *              : sci_iic_api_status_t new_status    ; New status
 * Return Value : None
 **********************************************************************************************************************/
static void sci_iic_set_internal_status (sci_iic_info_t * p_sci_iic_info, sci_iic_api_status_t new_status)
{
    /* Sets the previous status. */
    g_sci_iic_handles[p_sci_iic_info->ch_no]->api_b_status = g_sci_iic_handles[p_sci_iic_info->ch_no]->api_n_status;

    /* Sets the now status. */
    g_sci_iic_handles[p_sci_iic_info->ch_no]->api_n_status = new_status;
} /* End of function sci_iic_set_internal_status() */

/***********************************************************************************************************************
 * Function Name: sci_iic_set_ch_status
 * Description  : Updates the channel status.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void sci_iic_set_ch_status (sci_iic_info_t * p_sci_iic_info, sci_iic_ch_dev_status_t status)
{
    /* Sets the channel status. */
    g_sci_iic_ChStatus[p_sci_iic_info->ch_no] = status;

    /* Sets the device status. */
    p_sci_iic_info->dev_sts = status;
} /* End of function sci_iic_set_ch_status() */

/***********************************************************************************************************************
 * Function Name: sci_iic_check_chstatus_start
 * Description  : Check Channel Status Processing (at Start Function)
 *                When calls the starting function, checks the channel status.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : SCI_IIC_SUCCESS                    ; Successful operation
 *              : SCI_IIC_ERR_NO_INIT                ; Uninitialized state
 *              : SCI_IIC_ERR_BUS_BUSY               ; Bus busy
 *              ; SCI_IIC_ERR_OTHER                  ; Other error
 **********************************************************************************************************************/
static sci_iic_return_t sci_iic_check_chstatus_start (sci_iic_info_t * p_sci_iic_info)
{
    sci_iic_return_t ret = SCI_IIC_ERR_OTHER;

    /* Checks the channel status. */
    switch (g_sci_iic_ChStatus[p_sci_iic_info->ch_no])
    {
        case SCI_IIC_NO_INIT :

            /* It is necessary to reinitialize. */
            /* Sets the return value to uninitialized state. */
            ret = SCI_IIC_ERR_NO_INIT;

        break;

        case SCI_IIC_IDLE :
        case SCI_IIC_FINISH :
        case SCI_IIC_NACK :

            /* Checks the device status. */
            if ((((SCI_IIC_NO_INIT == p_sci_iic_info->dev_sts) || (SCI_IIC_IDLE == p_sci_iic_info->dev_sts))
                    || (SCI_IIC_FINISH == p_sci_iic_info->dev_sts)) || (SCI_IIC_NACK == p_sci_iic_info->dev_sts))
            {
                /* Sets the return value to success state. */
                ret = SCI_IIC_SUCCESS;
            }
            else if (SCI_IIC_COMMUNICATION == p_sci_iic_info->dev_sts)
            {
                /* Another device performed a driver reset processing
                 when this device is in idle state or communicating state. */
                /* It is necessary to reinitialize. */
                /* Sets the return value to uninitialized state. */
                ret = SCI_IIC_ERR_BUS_BUSY;
            }
            else
            {
                /* The device is in uninitialized state, idle state or error state. */
                ret = SCI_IIC_ERR_OTHER;
            }

        break;

        case SCI_IIC_COMMUNICATION :

            /* The device or another device is on communication. */
            /* Sets the return value to bus busy state. */
            ret = SCI_IIC_ERR_BUS_BUSY;

        break;

        default :

            ret = SCI_IIC_ERR_OTHER;

        break;
    }

    return ret;
} /* End of function sci_iic_check_chstatus_start() */

/***********************************************************************************************************************
 * Function Name: sci_iic_check_chstatus_advance
 * Description  : Check Channel Status Processing (at Advance Function).
 *                When calls the advance function, checks channel status.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ; IIC Information
 * Return Value : SCI_IIC_SUCCESS                    ; Successful operation
 *              : SCI_IIC_ERR_NO_INIT                ; Uninitialized state
 *              : SCI_IIC_ERR_BUS_BUSY               ; Bus busy
 *              ; SCI_IIC_ERR_OTHER                  ; Other error
 **********************************************************************************************************************/
static sci_iic_return_t sci_iic_check_chstatus_advance (sci_iic_info_t * p_sci_iic_info)
{
    sci_iic_return_t ret = SCI_IIC_ERR_OTHER;

    /* Checks the channel status. */
    switch (g_sci_iic_ChStatus[p_sci_iic_info->ch_no])
    {
        case SCI_IIC_NO_INIT :

            /* It is necessary to reinitialize. */
            /* Sets the return value to uninitialized state. */
            ret = SCI_IIC_ERR_NO_INIT;

        break;

        case SCI_IIC_IDLE :
        case SCI_IIC_FINISH :
        case SCI_IIC_NACK :

            /* Sets the return value to error state. */
            ret = SCI_IIC_ERR_OTHER;

        break;

        case SCI_IIC_COMMUNICATION :

            /* Checks the device status. */
            if ((((SCI_IIC_NO_INIT == p_sci_iic_info->dev_sts) || (SCI_IIC_IDLE == p_sci_iic_info->dev_sts))
                    || (SCI_IIC_FINISH == p_sci_iic_info->dev_sts)) || (SCI_IIC_NACK == p_sci_iic_info->dev_sts))
            {
                /* Sets the return value to idle state. */
                ret = SCI_IIC_ERR_OTHER;
            }
            else if (SCI_IIC_COMMUNICATION == p_sci_iic_info->dev_sts)
            {
                /* The device or another device is on communication. */
                /* Sets the return value to communication state. */
                ret = SCI_IIC_SUCCESS;
            }
            else
            {
                /* The device is in uninitialized state, communicating state or error state. */
                ret = SCI_IIC_ERR_OTHER;
            }

        break;

        default :

            ret = SCI_IIC_ERR_OTHER;

        break;
    }

    return ret;
} /* End of function sci_iic_check_chstatus_advance() */

/***********************************************************************************************************************
 * Function Name: sci_iic_disable
 * Description  : Disables IIC. Disables interrupt. Initializes IIC bus interrupt enable register.
 *              : Sets internal status.
 * Arguments    : sci_iic_info_t * p_sci_iic_info     ;   IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void sci_iic_disable (sci_iic_info_t * p_sci_iic_info)
{
    /* Initializes the IIC bus interrupt enable register. */
    sci_iic_int_icier_setting(p_sci_iic_info, SCI_IIC_SCR_INIT);

    /* Disables IIC. */
    sci_iic_iic_disable(p_sci_iic_info);

    /* Disables the interrupt. */
    r_sci_iic_int_disable(p_sci_iic_info);

    /* Sets the internal status. */
    sci_iic_set_internal_status(p_sci_iic_info, SCI_IIC_STS_IDLE);
} /* End of function sci_iic_disable() */

/***********************************************************************************************************************
 * Function Name: sci_iic_enable
 * Description  : Enables IIC bus interrupt enable register.
 *              : Enables interrupt. Enables IIC.
 * Arguments    : sci_iic_info_t * p_sci_iic_info     ;   IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void sci_iic_enable (sci_iic_info_t * p_sci_iic_info)
{
    /* Enables IIC.*/
    sci_iic_iic_enable(p_sci_iic_info);

    /* Enables IIC bus interrupt enable register. */
    sci_iic_int_icier_setting(p_sci_iic_info, SCI_IIC_SCR_ENABLE);

    /* Enables the interrupt. */
    r_sci_iic_int_enable(p_sci_iic_info);
} /* End of function sci_iic_enable() */

/***********************************************************************************************************************
 * Function Name: sci_iic_stop_cond_wait
 * Description  : Wait for Stop Condition Generation Processing.
 *                Checks the stop condition generation and checks form "bus busy" to "bus ready".
 * Arguments    : sci_iic_info_t * p_sci_iic_info     ;   IIC Information
 * Return Value : SCI_IIC_TRUE                        ;   Successful operation ,stop condition generation
 *              : SCI_IIC_FALSE                       ;   Failed operation ,stop condition generation
 **********************************************************************************************************************/
static bool sci_iic_stop_cond_wait (sci_iic_info_t * p_sci_iic_info)
{
    volatile uint16_t cnt;
    bool boolret = SCI_IIC_TRUE;

    cnt = SCI_IIC_STOP_COND_WAIT;
    do
    {
        /* Checks the stop condition generation. */
        if (SCI_IIC_TRUE == sci_iic_check_stop_event(p_sci_iic_info))
        {
            return boolret;
        }
        cnt--;
    } while (0U != cnt);

    /* Stop Condition generation error? */
    if (0U == cnt)
    {
        boolret = SCI_IIC_FALSE;
    }

    return boolret;
} /* End of function sci_iic_stop_cond_wait() */

/***********************************************************************************************************************
 * Function Name: sci_iic_check_stop_event
 * Description  : Check Event Flag Processing (at Stop Condition Generation).
 *                Checks the event flag.
 * Arguments    : sci_iic_info_t * p_sci_iic_info     ;   IIC Information
 * Return Value : SCI_IIC_TRUE                        ;   Sets the event flag to SCI_IIC_EV_INT_STOP.
 *              : SCI_IIC_FALSE                       ;   Does not set the event flag to SCI_IIC_EV_INT_STOP.
 **********************************************************************************************************************/
static bool sci_iic_check_stop_event (sci_iic_info_t * p_sci_iic_info)
{
    bool boolret = SCI_IIC_TRUE;
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;

    if ((SCI_IIC_STPREQ == g_sci_iic_handles[p_sci_iic_info->ch_no]->mode) && (1 == pregs->SIMR3.BIT.IICSTIF))
    {
        boolret = SCI_IIC_TRUE;
    }
    else
    {
        boolret = SCI_IIC_FALSE;
    }

    return boolret;
} /* End of function sci_iic_check_stop_event() */

/***********************************************************************************************************************
 * Outline      : Set ICIER Register Processing -> Set SCR Register Processing
 * Function Name: sci_iic_int_icier_setting
 * Description  : Sets an interrupt condition of SCI_IIC.
 * Arguments    : sci_iic_info_t * p_sci_iic_info         ;   IIC Information
 *              : uint8_t new_status                      ;   New status
 * Return Value : None
 **********************************************************************************************************************/
static void sci_iic_int_icier_setting (sci_iic_info_t * p_sci_iic_info, uint8_t New_icier)
{
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;

    /* Sets ICIER register. */
    pregs->SCR.BYTE = New_icier;
} /* End of function sci_iic_int_icier_setting() */

/***********************************************************************************************************************
 * Function Name: sci_iic_start_cond_generate
 * Description  : Generate Start Condition Processing.
 *                Sets SCR register and SIMR3 register.
 * Arguments    : sci_iic_info_t * p_sci_iic_info     ;   IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void sci_iic_start_cond_generate (sci_iic_info_t * p_sci_iic_info)
{
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;

    g_sci_iic_handles[p_sci_iic_info->ch_no]->mode = SCI_IIC_STAREQ; /* mode set start mode.*/

    /* SCR - Serial Control Register
     b5     TE - Transmit Enable - Serial transmission is enabled
     b4     RE - Transmit Enable - Serial reception is enabled */
    pregs->SCR.BYTE |= SCI_IIC_SCR_TE_RE;

    /* SIMR3 - I2C Mode Register 3
     b7:b6  IICSCLS - SSCL Output Select - Generate a start, restart, or stop condition.
     b5:b4  IICSDAS - SSDA Output Select - Generate a start, restart, or stop condition.
     b3     IICSTIF - Issuing of Start, Restart, or Stop Condition Completed Flag
     - There are no requests for generating conditions
     b0     IICSTAREQ - Start Condition Generation   - A start condition is not generated. */
    pregs->SIMR3.BYTE = SCI_IIC_ST_CON_GENERATED;
} /* End of function sci_iic_start_cond_generate() */

/***********************************************************************************************************************
 * Function Name: sci_iic_re_start_cond_generate
 * Description  : Generate Restart Condition Processing.
 *                Sets SCR register and SIMR3 register.
 * Arguments    : sci_iic_info_t * p_sci_iic_info     ;   IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void sci_iic_re_start_cond_generate (sci_iic_info_t * p_sci_iic_info)
{
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;

    g_sci_iic_handles[p_sci_iic_info->ch_no]->mode = SCI_IIC_RSTAREQ; /* mode set restart mode. */

    /* SCR - Serial Control Register
     b5     TE - Transmit Enable - Serial transmission is enabled
     b4     RE - Transmit Enable - Serial reception is enabled */
    pregs->SCR.BYTE |= SCI_IIC_SCR_TE_RE;

    /* SIMR3 - I2C Mode Register 3
     b7:b6  IICSCLS - SSCL Output Select - Generate a start, restart, or stop condition.
     b5:b4  IICSDAS - SSDA Output Select - Generate a start, restart, or stop condition.
     b3     IICSTIF - Issuing of Start, Restart, or Stop Condition Completed Flag
     - There are no requests for generating conditions
     b1     IICRSTAREQ - Start Condition Generation   - A restart condition is not generated. */
    pregs->SIMR3.BYTE = SCI_IIC_RST_CON_GENERATED;
} /* End of function sci_iic_re_start_cond_generate() */

/***********************************************************************************************************************
 * Function Name: sci_iic_stop_cond_generate
 * Description  : Generate Stop Condition Processing.
 *                Sets SCR register and SIMR3 register.
 * Arguments    : sci_iic_info_t * p_sci_iic_info     ;   IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void sci_iic_stop_cond_generate (sci_iic_info_t * p_sci_iic_info)
{
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;

    /* Enables the IIC bus interrupt. */
    g_sci_iic_handles[p_sci_iic_info->ch_no]->mode = SCI_IIC_STPREQ; /* mode set stop mode. */

    /* SCR - Serial Control Register
     b5     TE - Transmit Enable - Serial transmission is enabled
     b4     RE - Transmit Enable - Serial reception is enabled */
    pregs->SCR.BYTE |= SCI_IIC_SCR_TE_RE;

    /* SIMR3 - I2C Mode Register 3
     b7:b6  IICSCLS - SSCL Output Select - Generate a start, restart, or stop condition.
     b5:b4  IICSDAS - SSDA Output Select - Generate a start, restart, or stop condition.
     b3     IICSTIF - Issuing of Start, Restart, or Stop Condition Completed Flag
     - There are no requests for generating conditions
     b2     IICSTPREQ - Start Condition Generation   - A stop condition is not generated. */
    pregs->SIMR3.BYTE = SCI_IIC_SP_CON_GENERATED;
} /* End of function sci_iic_stop_cond_generate() */

/***********************************************************************************************************************
 * Function Name: sci_iic_set_sending_data
 * Description  : Transmit Data Processing.
 *                Sets transmission data to TDR register.
 * Arguments    : sci_iic_info_t * p_sci_iic_info    ;   IIC Information
 *              : uint8_t * p_data                   ;   Transmitted data buffer pointer
 * Return Value : None
 **********************************************************************************************************************/
static void sci_iic_set_sending_data (sci_iic_info_t * p_sci_iic_info, uint8_t * p_data)
{
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;

    /* Sets the transmitting data. */
    pregs->TDR = (*p_data); /* Writes data to SCI_IIC in order to transmit.   */

    /* dummy read */
    if (pregs->TDR)
    {
        R_NOP();
    }
} /* End of function sci_iic_set_sending_data() */

/***********************************************************************************************************************
 * Function Name: sci_iic_get_receiving_data
 * Description  : Receive Data Processing.
 *                Stores received data from RDR register.
 * Arguments    : sci_iic_info_t * p_sci_iic_info     ;   IIC Information
 * Return Value : Returns received data.
 **********************************************************************************************************************/
static uint8_t sci_iic_get_receiving_data (sci_iic_info_t * p_sci_iic_info)
{
    uint8_t ret;
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;

    ret = pregs->RDR;

    return ret;
} /* End of function sci_iic_get_receiving_data() */

/***********************************************************************************************************************
 * Function Name: sci_iic_iic_disable
 * Description  : Disable IIC Function Processing.
 *                Disables SCI_IIC communication and disables SCI_IIC multi-function pin controller.
 * Arguments    : sci_iic_info_t * p_sci_iic_info     ;   IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void sci_iic_iic_disable (sci_iic_info_t * p_sci_iic_info)
{
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;
#if SCI_IIC_CFG_PORT_SETTING_PROCESSING
    volatile __evenaccess    const sci_iic_ch_rom_t * prom = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom;
#endif

#if SCI_IIC_CFG_PORT_SETTING_PROCESSING
    /* Disables SCI_IIC multi-function pin controller after setting SCL and SDA to Hi-z by Reset. */
    /* Includes I/O register read operation at the end of the following function. */
    r_sci_iic_mpc_setting(prom->sscl_port_gr, prom->sscl_port_pin, SCI_IIC_MPC_SSCL_INIT);
    r_sci_iic_mpc_setting(prom->ssda_port_gr, prom->ssda_port_pin, SCI_IIC_MPC_SSDA_INIT);
#endif

    /* Resets SCI_IIC registers. */
    pregs->SCMR.BIT.SMIF = 0;
    pregs->SIMR1.BIT.IICM = 0;

    /* dummy read */
    if (pregs->SCMR.BYTE)
    {
        R_NOP();
    }
} /* End of function sci_iic_iic_disable() */

/***********************************************************************************************************************
 * Function Name: sci_iic_iic_enable
 * Description  : Enables SCI_IIC communication and enables SCI_IIC multi-function pin controller.
 * Arguments    : sci_iic_info_t * p_sci_iic_info     ;   IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void sci_iic_iic_enable (sci_iic_info_t * p_sci_iic_info)
{
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;
#if SCI_IIC_CFG_PORT_SETTING_PROCESSING
    volatile __evenaccess    const sci_iic_ch_rom_t * prom = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom;
#endif

    /* Enables SCI_IIC.*/
    pregs->SCMR.BIT.SMIF = 0;
    pregs->SIMR1.BIT.IICM = 1;

#if SCI_IIC_CFG_PORT_SETTING_PROCESSING
    /* Enables SCI_IIC multi-function pin controller.*/
    /* Includes I/O register read operation at the end of the following function. */
    r_sci_iic_mpc_setting(prom->sscl_port_gr, prom->sscl_port_pin, prom->sscl_en_val);
    r_sci_iic_mpc_setting(prom->ssda_port_gr, prom->ssda_port_pin, prom->ssda_en_val);
#endif
} /* End of function sci_iic_iic_enable() */

/***********************************************************************************************************************
 * Function Name: sci_iic_register_init_setting
 * Description  : SCI register Initial setting.
 * Arguments    : sci_iic_info_t * p_sci_iic_info     ;   IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void sci_iic_register_init_setting (sci_iic_info_t * p_sci_iic_info)
{
    sci_regs_t pregs = g_sci_iic_handles[p_sci_iic_info->ch_no]->prom->regs;

    /* Place the SSCL pin & SSDA pin in the high-impedance state */
    /* SIMR3 - I2C Mode Register 3
     b7:b6  IICSCLS - SSCL Output Select - Place the SSCL pin in the high-impedance state.
     b5:b4  IICSDAS - SSDA Output Select - Place the SSDA pin in the high-impedance state.
     b3     IICSTIF - Issuing of Start, Restart, or Stop Condition Completed Flag
     - There are no requests for generating conditions
     b2     IICSTPREQ  - Stop Condition Generation    - A stop condition is not generated.
     b1     IICRSTAREQ - Restart Condition Generation - A restart condition is not generated.
     b0     IICRSTAREQ - Start Condition Generation   - A start condition is not generated. */
    pregs->SIMR3.BIT.IICSCLS = SCI_IIC_HI_Z_OUTPUT; /* Output Hi-z */
    pregs->SIMR3.BIT.IICSDAS = SCI_IIC_HI_Z_OUTPUT; /* Output Hi-z */

    /* Set the data transmission/reception format in SMR, and SCMR. */
    /* SMR - Serial Mode Register
     b7     CM - Communications Mode - Asynchronous mode.
     b6     CHR - Character Length - Selects 8 bits as the data length.
     b5     PE - Parity Enable - Parity bit addition is not performed.
     b4     PM - Parity Mode - Selects even parity.
     b3     STOP - Stop Bit Length - 1 stop bit
     b2     MP  - Multi-Processor Mode - Multi-processor communications function is disabled.
     b1:b0  CKS - Clock Select - PCLK clock (n = 0) */
    pregs->SMR.BYTE = SCI_IIC_SMR_INIT; /* PCLK clock */

    /* Set the data transmission/reception format in SMR, and SCMR. */
    /* SMCR - Smart Card Mode Register
     b7     BCP2 - Base Clock Pulse 2 - 128 clock cycles (S = 128).
     b6:b4  Reserve - These bits are read as 1.
     b3     SDIR - Transmitted/Received Data Transfer Direction -Transfer with MSB-first.
     b2     SINV - Transmitted/Received Data Invert - Receive data is stored asit is in RDR.
     b1     Reserve - These bits are read as 1.
     b0     SMIF  - Smart Card Interface Mode Select - Serial communications interface mode. */
    pregs->SCMR.BIT.SDIR = SCI_IIC_MSB_FIRST;
} /* End of function sci_iic_register_init_setting() */

/***********************************************************************************************************************
 * Function Name: r_sci_iic_txi_isr_processing
 * Description  : TXI Processing.
 * Arguments    : uint8_t ch_no    ; number of channel
 * Return Value : None
 **********************************************************************************************************************/
void r_sci_iic_txi_isr_processing (uint8_t ch_no)
{
    sci_regs_t pregs = g_sci_iic_handles[ch_no]->prom->regs;

    /* ---- Checks NACK reception. ---- */
    if ((((SCI_IIC_MODE_SEND == g_sci_iic_handles[ch_no]->api_mode)
            || (SCI_IIC_STS_SEND_SLVADR_R_WAIT == g_sci_iic_handles[ch_no]->api_n_status))
            || ((SCI_IIC_MODE_SEND_RECEIVE == g_sci_iic_handles[ch_no]->api_mode)
                    && (SCI_IIC_STS_SEND_SLVADR_W_WAIT == g_sci_iic_handles[ch_no]->api_n_status)))
            || ((SCI_IIC_MODE_SEND_RECEIVE == g_sci_iic_handles[ch_no]->api_mode)
                    && (SCI_IIC_STS_SEND_DATA_WAIT == g_sci_iic_handles[ch_no]->api_n_status)))
    {
        /* ---- Receive NACK reception ---- */
        if (0U != pregs->SISR.BIT.IICACKR)
        {
            g_sci_iic_handles[ch_no]->api_event = SCI_IIC_EV_INT_NACK;
        }
        else
        {
            /* Sets event. */
            switch (g_sci_iic_handles[ch_no]->api_n_status)
            {
                case SCI_IIC_STS_SEND_SLVADR_W_WAIT :
                case SCI_IIC_STS_SEND_SLVADR_R_WAIT :
                case SCI_IIC_STS_RECEIVE_DATA_WAIT :

                    /* Sets interrupted address sending. */
                    g_sci_iic_handles[ch_no]->api_event = SCI_IIC_EV_INT_ADD;

                break;

                case SCI_IIC_STS_SEND_DATA_WAIT :

                    /* Sets interrupted data sending. */
                    g_sci_iic_handles[ch_no]->api_event = SCI_IIC_EV_INT_SEND;

                break;

                default :

                    /* Does nothing. */

                break;
            }

        }

    }
    else
    {
        /* ---- Receive ACK reception ---- */
        /* Sets event. */
        switch (g_sci_iic_handles[ch_no]->api_n_status)
        {
            case SCI_IIC_STS_SEND_SLVADR_W_WAIT :
            case SCI_IIC_STS_SEND_SLVADR_R_WAIT :
            case SCI_IIC_STS_RECEIVE_DATA_WAIT :

                /* Sets interrupted address sending. */
                g_sci_iic_handles[ch_no]->api_event = SCI_IIC_EV_INT_ADD;

            break;

            case SCI_IIC_STS_SEND_DATA_WAIT :

                /* Sets interrupted data sending. */
                g_sci_iic_handles[ch_no]->api_event = SCI_IIC_EV_INT_SEND;

            break;

            default :

                /* Does nothing. */

            break;
        }

    }

} /* End of function r_sci_iic_txi_isr_processing() */

/***********************************************************************************************************************
 * Function Name: r_sci_iic_tei_isr_processing
 * Description  : TEI Processing
 * Arguments    : uint8_t ch_no    ; number of channel
 * Return Value : None
 **********************************************************************************************************************/
void r_sci_iic_tei_isr_processing (uint8_t ch_no)
{
    sci_regs_t pregs = g_sci_iic_handles[ch_no]->prom->regs;

    /* Clear IR flag */
    pregs->SIMR3.BIT.IICSTIF = SCI_IIC_IICSTIF_CLEAR;

    while (SCI_IIC_IICSTIF_CLEAR != pregs->SIMR3.BIT.IICSTIF)
    {
        /* nothing to do */
    }

    while (0 != ((*g_sci_iic_handles[ch_no]->prom->pir_tei) & (g_sci_iic_handles[ch_no]->prom->tei_ir_mask)))
    {
        /* nothing to do */
    }

    /* ---- Checks NACK reception. ---- */
    if (0U != pregs->SISR.BIT.IICACKR)
    {
        g_sci_iic_handles[ch_no]->api_event = SCI_IIC_EV_INT_NACK;
    }

    /* ---- Checks start condition detection. ---- */
    if ((SCI_IIC_STAREQ == g_sci_iic_handles[ch_no]->mode) || (SCI_IIC_RSTAREQ == g_sci_iic_handles[ch_no]->mode))
    {
        g_sci_iic_handles[ch_no]->mode = 0;

        /* Sets event flag. */
        g_sci_iic_handles[ch_no]->api_event = SCI_IIC_EV_INT_START;
    }

    /* ---- Checks stop condition detection. ---- */
    if (SCI_IIC_STPREQ == g_sci_iic_handles[ch_no]->mode)
    {
        /* Sets event flag. */
        g_sci_iic_handles[ch_no]->api_event = SCI_IIC_EV_INT_STOP;
    }
} /* End of function r_sci_iic_tei_isr_processing() */

