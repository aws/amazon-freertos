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
 * File Name    : r_riic_rx.c
 * Description  : Functions for using RIIC on RX devices. 
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * History      : DD.MM.YYYY Version  Description
 *              : 01.07.2013 1.00     First Release
 *              : 30.09.2013 1.10     Change symbol of return value and status
 *              : 08.10.2013 1.20     Modified processing for the I/O register initialization and mode transition 
 *                                     when a stop condition is detected while the slave is communicating.
 *                                    Modified processing for mode transition when an API function is called
 *                                     while the bus is busy.
 *                                    Modified processing for mode transition
 *                                     when an arbitration lost occurred and the addresses do not match.
 *                                    Modified incorrect slave transmission after the master reception.
 *                                    Modified processing for the I/O register initialization
 *                                     when generating a start condition and receiving the slave address.
 *              : 17.07.2014 1.30     Added the processes for the timeout detection.
 *                                    Added the call back function for the timeout detection.
 *                                    The number of channels varies depending on the MCU.
 *                                    Therefore, process for each channel is set 
 *                                    in the source file(r_riic_rx###.c) for each MCU folder
 *                                    instead of this file.
 *                                    Similarly, PORT register, ICU register, SYSTEM register also varies
 *                                    depending on the MCU.
 *                                    Therefore, process for port function assignment and interrupt settings
 *                                    and module stop enable/disble is set in the source file(r_riic_rx###.c) 
 *                                    for each MCU folder instead of this file.
 *                                    (### is 111 or 64m)
 *              : 22.09.2014 1.40     The module is updated to measure the issue that slave communication 
 *                                    is not available after an arbitration-lost occurs and the bus is locked. 
 *                                      The issue occurs when the following four conditions are all met.
 *                                         - RIIC FIT module rev. 1.30 or earlier is used.
 *                                         - RX device operates as both the master and the slave 
 *                                           in multi-master communication.
 *                                         - An arbitration-lost is detected when communicating as the master.
 *                                         - Communication other than master reception or slave reception is performed.
 *              : 14.11.2014 1.50     Added RX113 support.
 *              : 09.10.2014 1.60     Added RX71M support.
 *              : 20.10.2014 1.70     Added RX231 support.
 *              : 31.10.2015 1.80     Added RX130, RX230, RX23T support.
 *              : 04.03.2016 1.90     Added RX24T support.
 *                                    Modified calculation processing for ICBRH and ICBRL registers value
 *              : 01.10.2016 2.00     Fixed a program according to the Renesas coding rules.
 *              : 02.06.2017 2.10     Deleted interrupt functions of RIIC3.
 *                                    Fixed processing of the riic_bps_calc function.
 *              : xx.xx.xxxx x.xx     Added support for GNUC and ICCRX.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
/* Defines for RIIC support */
#include "r_riic_rx_if.h"
#include "r_riic_rx_private.h"

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/
/*----------------------------------------------------------------------------*/
/*   riic infomation                                                          */
/*----------------------------------------------------------------------------*/
riic_ch_dev_status_t g_riic_ChStatus[MAX_RIIC_CH_NUM]; /* Channel status */

/*----------------------------------------------------------------------------*/
/*   Callback function                                                        */
/*----------------------------------------------------------------------------*/
volatile riic_callback g_riic_callbackfunc_m[MAX_RIIC_CH_NUM];
volatile riic_callback g_riic_callbackfunc_s[MAX_RIIC_CH_NUM];

/***********************************************************************************************************************
 Private global variables and functions
 **********************************************************************************************************************/
/*----------------------------------------------------------------------------*/
/*   riic infomation                                                          */
/*----------------------------------------------------------------------------*/
static riic_info_t * priic_info_m[MAX_RIIC_CH_NUM]; /* IIC driver information */
static riic_info_t * priic_info_s[MAX_RIIC_CH_NUM]; /* IIC driver information */

static riic_api_event_t riic_api_event[MAX_RIIC_CH_NUM]; /* Event flag */
static riic_api_info_t riic_api_info[MAX_RIIC_CH_NUM]; /* Internal status management */

/*----------------------------------------------------------------------------*/
/*   Main processing of RIIC Driver API functions                             */
/*----------------------------------------------------------------------------*/
static riic_return_t riic_open (riic_info_t *);
static riic_return_t riic_master_send (riic_info_t *);
static riic_return_t riic_master_receive (riic_info_t *);
static riic_return_t riic_master_send_receive (riic_info_t *);
static riic_return_t riic_slave_transfer (riic_info_t *);
static void riic_getstatus (riic_info_t *, riic_mcu_status_t *);
static riic_return_t riic_control (riic_info_t *, uint8_t ctrl_ptn);
static void riic_close (riic_info_t *);
static riic_return_t r_riic_advance (riic_info_t *);
static riic_return_t riic_advance (riic_info_t *);

/*----------------------------------------------------------------------------*/
/*   Called from function table                                               */
/*----------------------------------------------------------------------------*/
static riic_return_t riic_func_table (riic_api_event_t, riic_info_t *);
static riic_return_t riic_init_driver (riic_info_t *);
static riic_return_t riic_generate_start_cond (riic_info_t *);
static riic_return_t riic_after_gen_start_cond (riic_info_t *);
static riic_return_t riic_after_send_slvadr (riic_info_t *);
static riic_return_t riic_after_receive_slvadr (riic_info_t *);
static riic_return_t riic_write_data_sending (riic_info_t *);
static riic_return_t riic_read_data_receiving (riic_info_t *);
static riic_return_t riic_after_dtct_stop_cond (riic_info_t *);
static riic_return_t riic_arbitration_lost (riic_info_t *);
static riic_return_t riic_nack (riic_info_t *);
static riic_return_t riic_enable_slave_transfer (riic_info_t *);
static riic_return_t riic_time_out (riic_info_t *);

/*----------------------------------------------------------------------------*/
/*   Other function                                                           */
/*----------------------------------------------------------------------------*/
static void riic_api_mode_event_init (riic_info_t *, riic_api_mode_t);
static void riic_api_mode_set (riic_info_t *, riic_api_mode_t);
static void riic_api_status_set (riic_info_t *, riic_api_status_t);
static void riic_set_ch_status (riic_info_t *, riic_ch_dev_status_t);
static riic_return_t riic_check_chstatus_start (riic_info_t *);
static riic_return_t riic_check_chstatus_advance (riic_info_t *);
static void riic_enable (riic_info_t *);
static void riic_disable (riic_info_t *);
static void riic_init_io_register (riic_info_t *);
static void riic_slv_addr_match_int_enable (riic_info_t *);
static void riic_int_enable (riic_info_t *);
static void riic_int_disable (riic_info_t *);
static void riic_int_icier_setting (riic_info_t *, uint8_t New_icier);
static void riic_set_frequency (riic_info_t *);
static bool riic_check_bus_busy (riic_info_t *);
static void riic_start_cond_generate (riic_info_t *);
static void riic_re_start_cond_generate (riic_info_t *);
static void riic_stop_cond_generate (riic_info_t *);
static void riic_set_sending_data (riic_info_t *, uint8_t *p_data);
static uint8_t riic_get_receiving_data (riic_info_t *);
static void riic_receive_wait_setting (riic_info_t *);
static void riic_receive_pre_end_setting (riic_info_t *);
static void riic_receive_end_setting (riic_info_t *);
static void riic_reset_clear (riic_info_t *);
static void riic_reset_set (riic_info_t *);
static void riic_all_reset (riic_info_t *);
static void riic_clear_ir_flag (riic_info_t *);

static riic_return_t riic_bps_calc (riic_info_t *, uint16_t kbps);

/* static double riic_check_freq(void); */

#ifdef TN_RXA012A
static void riic_timeout_counter_clear (uint8_t channel);
#endif

/*----------------------------------------------------------------------------*/
/*   function table                                                           */
/*----------------------------------------------------------------------------*/
static const riic_mtx_t gc_riic_mtx_tbl[RIIC_STS_MAX][RIIC_EV_MAX] =
{
    /* Uninitialized state */
    {
        { RIIC_EV_INIT, riic_init_driver }, /* IIC Driver Initialization */
        { RIIC_EV_EN_SLV_TRANSFER, NULL }, /* Enable Slave Transfer */
        { RIIC_EV_GEN_START_COND, NULL }, /* Start Condition Generation */
        { RIIC_EV_INT_START, NULL }, /* Start Condition Generation Interrupt */
        { RIIC_EV_INT_ADD, NULL }, /* Address Sending Interrupt */
        { RIIC_EV_INT_SEND, NULL }, /* Data Sending Interrupt */
        { RIIC_EV_INT_RECEIVE, NULL }, /* Data Receiving Interrupt */
        { RIIC_EV_INT_STOP, NULL }, /* Stop Condition Generation Interrupt */
        { RIIC_EV_INT_AL, NULL }, /* Arbitration-Lost Interrupt */
        { RIIC_EV_INT_NACK, NULL }, /* No Acknowledge Interrupt */
        { RIIC_EV_INT_TMO, NULL }, /* Time out Interrupt */
    },

    /* Idle state */
    {
        { RIIC_EV_INIT, NULL }, /* IIC Driver Initialization */
        { RIIC_EV_EN_SLV_TRANSFER, riic_enable_slave_transfer }, /* Enable Slave Transfer */
        { RIIC_EV_GEN_START_COND, riic_generate_start_cond }, /* Start Condition Generation */
        { RIIC_EV_INT_START, NULL }, /* Start Condition Generation Interrupt */
        { RIIC_EV_INT_ADD, NULL }, /* Address Sending Interrupt */
        { RIIC_EV_INT_SEND, NULL }, /* Data Sending Interrupt */
        { RIIC_EV_INT_RECEIVE, NULL }, /* Data Receiving Interrupt */
        { RIIC_EV_INT_STOP, NULL }, /* Stop Condition Generation Interrupt */
        { RIIC_EV_INT_AL, NULL }, /* Arbitration-Lost Interrupt */
        { RIIC_EV_INT_NACK, NULL }, /* No Acknowledge Interrupt */
        { RIIC_EV_INT_TMO, NULL }, /* Time out Interrupt */
    },

    /* Idle state on enable slave transfer */
    {
        { RIIC_EV_INIT, NULL }, /* IIC Driver Initialization */
        { RIIC_EV_EN_SLV_TRANSFER, NULL }, /* Enable Slave Transfer */
        { RIIC_EV_GEN_START_COND, riic_generate_start_cond }, /* Start Condition Generation */
        { RIIC_EV_INT_START, NULL }, /* Start Condition Generation Interrupt */
        { RIIC_EV_INT_ADD, riic_after_receive_slvadr }, /* Address Sending Interrupt */
        { RIIC_EV_INT_SEND, NULL }, /* Data Sending Interrupt */
        { RIIC_EV_INT_RECEIVE, riic_after_receive_slvadr }, /* Data Receiving Interrupt */
        { RIIC_EV_INT_STOP, NULL }, /* Stop Condition Generation Interrupt */
        { RIIC_EV_INT_AL, NULL }, /* Arbitration-Lost Interrupt */
        { RIIC_EV_INT_NACK, NULL }, /* No Acknowledge Interrupt */
        { RIIC_EV_INT_TMO, NULL }, /* Time out Interrupt */
    },

    /* Start condition generation completion wait state */
    {
        { RIIC_EV_INIT, NULL }, /* IIC Driver Initialization */
        { RIIC_EV_EN_SLV_TRANSFER, NULL }, /* Enable Slave Transfer */
        { RIIC_EV_GEN_START_COND, NULL }, /* Start Condition Generation */
        { RIIC_EV_INT_START, riic_after_gen_start_cond }, /* Start Condition Generation Interrupt */
        { RIIC_EV_INT_ADD, NULL }, /* Address Sending Interrupt */
        { RIIC_EV_INT_SEND, NULL }, /* Data Sending Interrupt */
        { RIIC_EV_INT_RECEIVE, NULL }, /* Data Receiving Interrupt */
        { RIIC_EV_INT_STOP, NULL }, /* Stop Condition Generation Interrupt */
        { RIIC_EV_INT_AL, riic_arbitration_lost }, /* Arbitration-Lost Interrupt */
        { RIIC_EV_INT_NACK, riic_nack }, /* No Acknowledge Interrupt */
        { RIIC_EV_INT_TMO, riic_time_out }, /* Time out Interrupt */
    },

    /* Slave address [Write] transmission completion wait state */
    {
        { RIIC_EV_INIT, NULL }, /* IIC Driver Initialization */
        { RIIC_EV_EN_SLV_TRANSFER, NULL }, /* Enable Slave Transfer */
        { RIIC_EV_GEN_START_COND, NULL }, /* Start Condition Generation */
        { RIIC_EV_INT_START, NULL }, /* Start Condition Generation Interrupt */
        { RIIC_EV_INT_ADD, riic_after_send_slvadr }, /* Address Sending Interrupt */
        { RIIC_EV_INT_SEND, NULL }, /* Data Sending Interrupt */
        { RIIC_EV_INT_RECEIVE, NULL }, /* Data Receiving Interrupt */
        { RIIC_EV_INT_STOP, NULL }, /* Stop Condition Generation Interrupt */
        { RIIC_EV_INT_AL, riic_arbitration_lost }, /* Arbitration-Lost Interrupt */
        { RIIC_EV_INT_NACK, riic_nack }, /* No Acknowledge Interrupt */
        { RIIC_EV_INT_TMO, riic_time_out }, /* Time out Interrupt */
    },

    /* Slave address [Read] transmission completion wait state */
    {
        { RIIC_EV_INIT, NULL }, /* IIC Driver Initialization */
        { RIIC_EV_EN_SLV_TRANSFER, NULL }, /* Enable Slave Transfer */
        { RIIC_EV_GEN_START_COND, NULL }, /* Start Condition Generation */
        { RIIC_EV_INT_START, NULL }, /* Start Condition Generation Interrupt */
        { RIIC_EV_INT_ADD, NULL }, /* Address Sending Interrupt */
        { RIIC_EV_INT_SEND, NULL }, /* Data Sending Interrupt */
        { RIIC_EV_INT_RECEIVE, riic_after_send_slvadr }, /* Data Receiving Interrupt */
        { RIIC_EV_INT_STOP, NULL }, /* Stop Condition Generation Interrupt  */
        { RIIC_EV_INT_AL, riic_arbitration_lost }, /* Arbitration-Lost Interrupt */
        { RIIC_EV_INT_NACK, riic_nack }, /* No Acknowledge Interrupt */
        { RIIC_EV_INT_TMO, riic_time_out }, /* Time out Interrupt */
    },

    /* Data transmission completion wait state */
    {
        { RIIC_EV_INIT, NULL }, /* IIC Driver Initialization */
        { RIIC_EV_EN_SLV_TRANSFER, NULL }, /* Enable Slave Transfer */
        { RIIC_EV_GEN_START_COND, NULL }, /* Start Condition Generation */
        { RIIC_EV_INT_START, NULL }, /* Start Condition Generation Interrupt */
        { RIIC_EV_INT_ADD, NULL }, /* Address Sending Interrupt */
        { RIIC_EV_INT_SEND, riic_write_data_sending }, /* Data Sending Interrupt */
        { RIIC_EV_INT_RECEIVE, NULL }, /* Data Receiving Interrupt */
        { RIIC_EV_INT_STOP, NULL }, /* Stop Condition Generation Interrupt */
        { RIIC_EV_INT_AL, riic_arbitration_lost }, /* Arbitration-Lost Interrupt */
        { RIIC_EV_INT_NACK, riic_nack }, /* No Acknowledge Interrupt */
        { RIIC_EV_INT_TMO, riic_time_out }, /* Time out Interrupt */
    },

    /* Data reception completion wait state */
    {
        { RIIC_EV_INIT, NULL }, /* IIC Driver Initialization */
        { RIIC_EV_EN_SLV_TRANSFER, NULL }, /* Enable Slave Transfer */
        { RIIC_EV_GEN_START_COND, NULL }, /* Start Condition Generation */
        { RIIC_EV_INT_START, NULL }, /* Start Condition Generation Interrupt */
        { RIIC_EV_INT_ADD, NULL }, /* Address Sending Interrupt */
        { RIIC_EV_INT_SEND, NULL }, /* Data Sending Interrupt */
        { RIIC_EV_INT_RECEIVE, riic_read_data_receiving }, /* Data Receiving Interrupt */
        { RIIC_EV_INT_STOP, NULL }, /* Stop Condition Generation Interrupt */
        { RIIC_EV_INT_AL, riic_arbitration_lost }, /* Arbitration-Lost Interrupt */
        { RIIC_EV_INT_NACK, riic_nack }, /* No Acknowledge Interrupt */
        { RIIC_EV_INT_TMO, riic_time_out }, /* Time out Interrupt */
    },

    /* Stop condition generation completion wait state */
    {
        { RIIC_EV_INIT, NULL }, /* IIC Driver Initialization */
        { RIIC_EV_EN_SLV_TRANSFER, NULL }, /* Enable Slave Transfer */
        { RIIC_EV_GEN_START_COND, NULL }, /* Start Condition Generation */
        { RIIC_EV_INT_START, NULL }, /* Start Condition Generation Interrupt */
        { RIIC_EV_INT_ADD, NULL }, /* Address Sending Interrupt */
        { RIIC_EV_INT_SEND, NULL }, /* Data Sending Interrupt */
        { RIIC_EV_INT_RECEIVE, NULL }, /* Data Receiving Interrupt */
        { RIIC_EV_INT_STOP, riic_after_dtct_stop_cond }, /* Stop Condition Generation Interrupt */
        { RIIC_EV_INT_AL, NULL }, /* Arbitration-Lost Interrupt */
        { RIIC_EV_INT_NACK, riic_nack }, /* No Acknowledge Interrupt */
        { RIIC_EV_INT_TMO, riic_time_out }, /* Time out Interrupt */
    },
};

/***********************************************************************************************************************
 * Function Name: R_RIIC_Open
 * Description  : Initializes the RIIC driver. Initializes the I/O register for RIIC control.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 *              : RIIC_ERR_LOCK_FUNC             ; Another task is handling API function
 *              : RIIC_ERR_INVALID_CHAN          ; channel number invalid for part
 *              : RIIC_ERR_INVALID_ARG           ; Parameter error
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
riic_return_t R_RIIC_Open (riic_info_t * p_riic_info)
{
    bool chk = RIIC_FALSE;
    riic_return_t ret;

    /* ---- CHECK ARGUMENTS ---- */
#if (1U == RIIC_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == p_riic_info)
    {
        return RIIC_ERR_INVALID_ARG;
    }

    /* ---- CHECK CHANNEL ---- */
    if (false == riic_mcu_check_channel(p_riic_info->ch_no))
    {
        return RIIC_ERR_INVALID_CHAN;
    }
#endif

    /*  ---- RIIC HARDWARE LOCK ---- */
    chk = riic_mcu_hardware_lock(p_riic_info->ch_no);
    if (RIIC_FALSE == chk)
    {
        /* Lock has already been acquired by another task. Needs to try again later. */
        return RIIC_ERR_LOCK_FUNC;
    }

    /* ---- INITIALIZE RIIC INTERNAL STATUS INFORMATION ---- */
    g_riic_ChStatus[p_riic_info->ch_no] = RIIC_NO_INIT;
    p_riic_info->dev_sts = RIIC_NO_INIT;

    /* ---- INITIALIZE CHANNEL ---- */
    /* Calls the API function. */
    ret = riic_open(p_riic_info);

    return ret;
} /* End of function R_RIIC_Open() */

/***********************************************************************************************************************
 * Function Name: riic_open
 * Description  : sub function of R_RIIC_Open(). Initializes the I/O register for RIIC control.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
static riic_return_t riic_open (riic_info_t * p_riic_info)
{
    riic_return_t ret;

    /* Updates the channel status. */
    riic_set_ch_status(p_riic_info, RIIC_IDLE);

    /* Initialize API status on RAM */
    riic_api_mode_event_init(p_riic_info, RIIC_MODE_NONE);

    /* Sets the internal status. (Internal information initialization) */
    riic_api_status_set(p_riic_info, RIIC_STS_NO_INIT);

    /* Enables the IIC peripheral registers. */
    /* Includes I/O register read operation at the end of the following function. */
    /* RIIC HARDWARE ENABLE */
    riic_mcu_power_on(p_riic_info->ch_no);

    /* Initializes the IIC driver. */
    ret = riic_func_table(RIIC_EV_INIT, p_riic_info);

    if (RIIC_SUCCESS != ret)
    {
        ret = RIIC_ERR_OTHER;

        /* Updates the channel status. */
        riic_set_ch_status(p_riic_info, RIIC_NO_INIT);
    }
    else
    {
        /* Registers a callback function for supported interrupts. */
        riic_mcu_int_init(p_riic_info->ch_no);

#if (1U == RIIC_CFG_PORT_SET_PROCESSING)
        /* Init Port Processing */
        /* Ports open setting */
        /* Includes I/O register read operation at the end of the following function. */
        riic_mcu_io_open(p_riic_info->ch_no);

        /* Enables RIIC multi-function pin controller. */
        /* Includes I/O register read operation at the end of the following function. */
        riic_mcu_mpc_enable(p_riic_info->ch_no);
#endif
    }

    return ret;
} /* End of function riic_open() */

/***********************************************************************************************************************
 * Outline      : 
 * Function Name: R_RIIC_MasterSend
 * Description  : Master transmission start processing.
 *                Generates the start condition. Starts the master transmission.
 * Arguments    : riic_info_t * p_riic_info  ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 *              : RIIC_ERR_INVALID_CHAN          ; None existent channel number
 *              : RIIC_ERR_INVALID_ARG           ; Parameter error
 *              : RIIC_ERR_NO_INIT               ; Uninitialized state
 *              : RIIC_ERR_BUS_BUSY              ; Bus busy
 *              : RIIC_ERR_AL                    ; Arbitration lost error
 *              : RIIC_ERR_TMO                   ; Time out error
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
riic_return_t R_RIIC_MasterSend (riic_info_t * p_riic_info)
{
    riic_return_t ret;

    /* ---- CHECK ARGUMENTS ---- */
#if (1U == RIIC_CFG_PARAM_CHECKING_ENABLE)
    if ((((NULL == p_riic_info) || ((0 == p_riic_info->cnt1st) && ((uint8_t *) FIT_NO_PTR != p_riic_info->p_data1st)))
            || ((0 == p_riic_info->cnt2nd) && ((uint8_t *) FIT_NO_PTR != p_riic_info->p_data2nd)))
            || (NULL == p_riic_info->callbackfunc))
    {
        return RIIC_ERR_INVALID_ARG;
    }

    /* ---- CHECK CHANNEL ---- */
    if (false == riic_mcu_check_channel(p_riic_info->ch_no))
    {
        return RIIC_ERR_INVALID_CHAN;
    }
#endif

    /* Calls the API function. */
    ret = riic_master_send(p_riic_info);

    return ret;
} /* End of function R_RIIC_MasterSend() */

/***********************************************************************************************************************
 * Function Name: riic_master_send
 * Description  : sub function of R_RIIC_MasterSend().
 *              : Generates the start condition. Starts the master transmission.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 *              : RIIC_ERR_NO_INIT               ; Uninitialized state
 *              : RIIC_ERR_BUS_BUSY              ; Bus busy
 *              : RIIC_ERR_AL                    ; Arbitration lost error
 *              : RIIC_ERR_TMO                   ; Time out error
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
static riic_return_t riic_master_send (riic_info_t * p_riic_info)
{
    riic_return_t ret;

    /* Checks the channel status. */
    ret = riic_check_chstatus_start(p_riic_info);

    if (RIIC_SUCCESS != ret)
    {
        return ret;
    }

    /* Updates the channel status. */
    riic_set_ch_status(p_riic_info, RIIC_COMMUNICATION);

    /* Initialize API status on RAM */
    riic_api_mode_event_init(p_riic_info, RIIC_MODE_M_SEND);

    /* Copy p_riic_info address */
    priic_info_m[p_riic_info->ch_no] = p_riic_info;

    /* Sets the callback function. */
    g_riic_callbackfunc_m[p_riic_info->ch_no] = p_riic_info->callbackfunc;

    /* Generates the start condition.  */
    ret = riic_func_table(RIIC_EV_GEN_START_COND, p_riic_info);

    if (RIIC_SUCCESS != ret)
    {
        /* Initialize API status on RAM */
        riic_set_ch_status(p_riic_info, RIIC_IDLE);
        if (RIIC_MODE_S_READY == riic_api_info[p_riic_info->ch_no].B_Mode)
        {
            riic_api_mode_set(p_riic_info, RIIC_MODE_NONE);
            riic_api_mode_set(p_riic_info, RIIC_MODE_S_READY);
        }
        else
        {
            riic_api_mode_set(p_riic_info, RIIC_MODE_NONE);
        }
    }

    return ret;
} /* End of function riic_master_send() */

/***********************************************************************************************************************
 * Outline      : 
 * Function Name: R_RIIC_MasterReceive
 * Description  : Master reception start processing.
 *              : Generates the start condition. Starts the master reception.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 *              : RIIC_ERR_INVALID_CHAN          ; None existent channel number
 *              : RIIC_ERR_INVALID_ARG           ; Parameter error
 *              : RIIC_ERR_NO_INIT               ; Uninitialized state
 *              : RIIC_ERR_BUS_BUSY              ; Bus busy
 *              : RIIC_ERR_AL                    ; Arbitration lost error
 *              : RIIC_ERR_TMO                   ; Time out error
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
riic_return_t R_RIIC_MasterReceive (riic_info_t * p_riic_info)
{
    riic_return_t ret;

    /* ---- CHECK ARGUMENTS ---- */
#if (1U == RIIC_CFG_PARAM_CHECKING_ENABLE)
    /* Parameter check */
    if ((((((NULL == p_riic_info) || (NULL == p_riic_info->p_slv_adr))
            || ((0 != p_riic_info->cnt1st) && (NULL == p_riic_info->p_data1st))) || (NULL == p_riic_info->p_data2nd))
            || (0 == p_riic_info->cnt2nd)) || (NULL == p_riic_info->callbackfunc))
    {
        return RIIC_ERR_INVALID_ARG;
    }

    /* ---- CHECK CHANNEL ---- */
    if (false == riic_mcu_check_channel(p_riic_info->ch_no))
    {
        return RIIC_ERR_INVALID_CHAN;
    }

#endif

    if (0 == p_riic_info->cnt1st)
    {
        /* Calls the API function. */
        ret = riic_master_receive(p_riic_info);
    }
    else
    {
        /* Calls the API function. */
        ret = riic_master_send_receive(p_riic_info);
    }

    return ret;
} /* End of function R_RIIC_MasterReceive() */

/***********************************************************************************************************************
 * Outline      : 
 * Function Name: riic_master_receive
 * Description  : sub function of R_RIIC_MasterReceive(). 
 *              : Generates the start condition. Starts the master reception.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 *              : RIIC_ERR_NO_INIT               ; Uninitialized state
 *              : RIIC_ERR_BUS_BUSY              ; Bus busy
 *              : RIIC_ERR_AL                    ; Arbitration lost error
 *              : RIIC_ERR_TMO                   ; Time out error
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
static riic_return_t riic_master_receive (riic_info_t * p_riic_info)
{
    riic_return_t ret;

    /* Checks the channel status. */
    ret = riic_check_chstatus_start(p_riic_info);

    if (RIIC_SUCCESS != ret)
    {
        return ret;
    }

    /* Updates the channel status. */
    riic_set_ch_status(p_riic_info, RIIC_COMMUNICATION);

    /* Initialize API status on RAM */
    riic_api_mode_event_init(p_riic_info, RIIC_MODE_M_RECEIVE);

    /* Copy p_riic_info address */
    priic_info_m[p_riic_info->ch_no] = p_riic_info;

    /* Sets the callback function. */
    g_riic_callbackfunc_m[p_riic_info->ch_no] = p_riic_info->callbackfunc;

    /* Generates the start condition. */
    ret = riic_func_table(RIIC_EV_GEN_START_COND, p_riic_info);

    if (RIIC_SUCCESS != ret)
    {
        /* Initialize API status on RAM */
        riic_set_ch_status(p_riic_info, RIIC_IDLE);
        if (RIIC_MODE_S_READY == riic_api_info[p_riic_info->ch_no].B_Mode)
        {
            riic_api_mode_set(p_riic_info, RIIC_MODE_NONE);
            riic_api_mode_set(p_riic_info, RIIC_MODE_S_READY);
        }
        else
        {
            riic_api_mode_set(p_riic_info, RIIC_MODE_NONE);
        }
    }

    return ret;
} /* End of function riic_master_receive() */

/***********************************************************************************************************************
 * Outline      : 
 * Function Name: riic_master_send_receive
 * Description  : sub function of R_RIIC_MasterReceive().
 *              : Generates the start condition. Starts the master reception.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 *              : RIIC_ERR_NO_INIT               ; Uninitialized state
 *              : RIIC_ERR_BUS_BUSY              ; Bus busy
 *              : RIIC_ERR_AL                    ; Arbitration lost error
 *              : RIIC_ERR_TMO                   ; Time out error
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
static riic_return_t riic_master_send_receive (riic_info_t * p_riic_info)
{
    riic_return_t ret;

    /* Checks the channel status. */
    ret = riic_check_chstatus_start(p_riic_info);

    if (RIIC_SUCCESS != ret)
    {
        return ret;
    }

    /* Updates the channel status. */
    riic_set_ch_status(p_riic_info, RIIC_COMMUNICATION);

    /* Initialize API status on RAM */
    riic_api_mode_event_init(p_riic_info, RIIC_MODE_M_SEND_RECEIVE);

    /* Copy p_riic_info address */
    priic_info_m[p_riic_info->ch_no] = p_riic_info;

    /* Sets the callback function. */
    g_riic_callbackfunc_m[p_riic_info->ch_no] = p_riic_info->callbackfunc;

    /* Generates the start condition. */
    ret = riic_func_table(RIIC_EV_GEN_START_COND, p_riic_info);

    if (RIIC_SUCCESS != ret)
    {
        /* Initialize API status on RAM */
        riic_set_ch_status(p_riic_info, RIIC_IDLE);
        if (RIIC_MODE_S_READY == riic_api_info[p_riic_info->ch_no].B_Mode)
        {
            riic_api_mode_set(p_riic_info, RIIC_MODE_NONE);
            riic_api_mode_set(p_riic_info, RIIC_MODE_S_READY);
        }
        else
        {
            riic_api_mode_set(p_riic_info, RIIC_MODE_NONE);
        }
    }

    return ret;
} /* End of function riic_master_send_receive() */

/***********************************************************************************************************************
 * Function Name: R_RIIC_SlaveTransfer
 * Description  : Slave transfer start processing.
 *              : Generates the start condition. Starts the slave reception and the slave transmission.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 *              : RIIC_ERR_INVALID_CHAN          ; None existent channel number
 *              : RIIC_ERR_INVALID_ARG           ; Parameter error
 *              : RIIC_ERR_NO_INIT               ; Uninitialized state
 *              : RIIC_ERR_BUS_BUSY              ; Bus busy
 *              : RIIC_ERR_AL                    ; Arbitration lost error
 *              : RIIC_ERR_TMO                   ; Time out error
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
riic_return_t R_RIIC_SlaveTransfer (riic_info_t * p_riic_info)
{
    riic_return_t ret;

    /* ---- CHECK ARGUMENTS ---- */
#if (1U == RIIC_CFG_PARAM_CHECKING_ENABLE)
    /* Parameter check */
    if (((NULL == p_riic_info)
            || (((uint8_t *) FIT_NO_PTR == p_riic_info->p_data1st) && ((uint8_t *) FIT_NO_PTR == p_riic_info->p_data2nd)))
            || (NULL == p_riic_info->callbackfunc))
    {
        return RIIC_ERR_INVALID_ARG;
    }

    /* ---- CHECK CHANNEL ---- */
    if (false == riic_mcu_check_channel(p_riic_info->ch_no))
    {
        return RIIC_ERR_INVALID_CHAN;
    }

#endif

    /* Calls the API function. */
    ret = riic_slave_transfer(p_riic_info);

    return ret;
} /* End of function R_RIIC_SlaveTransfer() */

/***********************************************************************************************************************
 * Outline      : 
 * Function Name: riic_slave_transfer
 * Description  : sub function of R_RIIC_SlaveTransfer.
 *              : Generates the start condition. Starts the slave reception and the slave transmission.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 *              : RIIC_ERR_NO_INIT               ; Uninitialized state
 *              : RIIC_ERR_BUS_BUSY              ; Bus busy
 *              : RIIC_ERR_AL                    ; Arbitration lost error
 *              : RIIC_ERR_TMO                   ; Time out error
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
static riic_return_t riic_slave_transfer (riic_info_t * p_riic_info)
{
    riic_return_t ret;

    /* Checks the channel status. */
    ret = riic_check_chstatus_start(p_riic_info);

    if (RIIC_SUCCESS != ret)
    {
        return ret;
    }

    if (RIIC_MODE_S_READY != riic_api_info[p_riic_info->ch_no].N_Mode)
    {
        /* Updates the channel status. */
        riic_set_ch_status(p_riic_info, RIIC_IDLE);

        /* Initialize API status on RAM */
        riic_api_mode_event_init(p_riic_info, RIIC_MODE_S_READY);

        /* Generates the start condition. */
        ret = riic_func_table(RIIC_EV_EN_SLV_TRANSFER, p_riic_info);
    }

    /* Copy p_riic_info address */
    priic_info_s[p_riic_info->ch_no] = p_riic_info;

    /* Sets the callback function. */
    g_riic_callbackfunc_s[p_riic_info->ch_no] = p_riic_info->callbackfunc;

    /* ---- Enables IIC bus interrupt enable register. ---- */
    if (((uint8_t *) FIT_NO_PTR != p_riic_info->p_data1st) && ((uint8_t *) FIT_NO_PTR != p_riic_info->p_data2nd))
    {
        /* Enables slave send and slave receive */
        riic_int_icier_setting(p_riic_info, RIIC_ICIER_TX_RX);
    }
    else if (((uint8_t *) FIT_NO_PTR != p_riic_info->p_data1st) && ((uint8_t *) FIT_NO_PTR == p_riic_info->p_data2nd))
    {
        /* Enable slave send */
        riic_int_icier_setting(p_riic_info, RIIC_ICIER_TX);
    }
    else if (((uint8_t *) FIT_NO_PTR == p_riic_info->p_data1st) && ((uint8_t *) FIT_NO_PTR != p_riic_info->p_data2nd))
    {
        /* Enable slave receive */
        riic_int_icier_setting(p_riic_info, RIIC_ICIER_RX);
    }
    else
    {
        /* Do Nothing */
    }

    return ret;
} /* End of function riic_slave_transfer() */

/***********************************************************************************************************************
 * Function Name: R_RIIC_GetStatus
 * Description  : get status of this RIIC module. 
 *                Returns the state of this module.
 *                Obtains the state of the RIIC channel, which specified by the parameter, by reading the register,
 *                variable, or others, and returns the obtained state as 32-bit structure.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 *                riic_mcu_status_t * p_riic_status ; The address to store the I2C state flag.
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 *              : RIIC_ERR_INVALID_ARG           ; Parameter error
 *              : RIIC_ERR_INVALID_CHAN          ; channel number invalid for part
 **********************************************************************************************************************/
riic_return_t R_RIIC_GetStatus (riic_info_t *p_riic_info, riic_mcu_status_t *p_riic_status)
{
    /* ---- CHECK ARGUMENTS ---- */
#if (1U == RIIC_CFG_PARAM_CHECKING_ENABLE)

    if ((NULL == p_riic_info) || (NULL == p_riic_status))
    {
        return RIIC_ERR_INVALID_ARG;
    }

    /* ---- CHECK CHANNEL ---- */
    if (false == riic_mcu_check_channel(p_riic_info->ch_no))
    {
        return RIIC_ERR_INVALID_CHAN;
    }

#endif

    /* Calls the API function. */
    riic_getstatus(p_riic_info, p_riic_status);

    return RIIC_SUCCESS;
} /* End of function R_RIIC_GetStatus() */

/***********************************************************************************************************************
 * Function Name: riic_getstatus
 * Description  : sub function of R_RIIC_GetStatus.
 *                Returns the state of this module.
 *                Obtains the state of the RIIC channel, which specified by the parameter, by reading the register,
 *                variable, or others, and returns the obtained state as 32-bit structure.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 *                riic_mcu_status_t * p_riic_status ; The address to store the I2C state flag.
 * Return Value : none
 **********************************************************************************************************************/
static void riic_getstatus (riic_info_t *p_riic_info, riic_mcu_status_t *p_riic_status)
{
    volatile uint8_t uctmp = 0x00;
    volatile riic_mcu_status_t sts_flag;
    volatile uint8_t * const piccr1_reg = RIIC_ICCR1_ADR(p_riic_info->ch_no);
    volatile uint8_t * const piccr2_reg = RIIC_ICCR2_ADR(p_riic_info->ch_no);
    volatile uint8_t * const picsr1_reg = RIIC_ICSR1_ADR(p_riic_info->ch_no);
    volatile uint8_t * const picsr2_reg = RIIC_ICSR2_ADR(p_riic_info->ch_no);

    sts_flag.LONG = 0x00000000;

    /* ---- Check Bus state ---- */
    if (RIIC_ICCR2_BBSY_SET == ((*piccr2_reg) & RIIC_ICCR2_BBSY))
    {
        sts_flag.BIT.BSY = 1U;
    }
    else
    {
        sts_flag.BIT.BSY = 0U;
    }

    /* ---- Check Mode ---- */
    if (RIIC_ICCR2_TRS_SET == ((*piccr2_reg) & RIIC_ICCR2_TRS))
    {
        /* Send */
        sts_flag.BIT.TRS = 1U;
    }
    else
    {
        /* Receive */
        sts_flag.BIT.TRS = 0U;
    }

    /* ---- Check Event detection ---- */
    if (RIIC_ICSR2_NACKF_SET == ((*picsr2_reg) & RIIC_ICSR2_NACKF))
    {
        /* NACK translation detected*/
        sts_flag.BIT.NACK = 1U;
    }
    else
    {
        /* Not detected */
        sts_flag.BIT.NACK = 0U;
    }

    /* ---- Check Send buffer status ---- */
    if (RIIC_ICSR2_TDRE_SET == ((*picsr2_reg) & RIIC_ICSR2_TDRE))
    {
        /* Send buffer empty */
        sts_flag.BIT.SBUF = 1U;
    }
    else
    {
        /* Send buffer not empty */
        sts_flag.BIT.SBUF = 0U;
    }

    /* ---- Check Receive buffer status ---- */
    if (RIIC_ICSR2_RDRF_SET == ((*picsr2_reg) & RIIC_ICSR2_RDRF))
    {
        /* Receive buffer not empty */
        sts_flag.BIT.RBUF = 1U;
    }
    else
    {
        /* Receive buffer empty */
        sts_flag.BIT.RBUF = 0U;
    }

    /* ---- Check SDA pin level ---- */
    if (RIIC_ICCR1_SDAI_SET == ((*piccr1_reg) & RIIC_ICCR1_SDAI))
    {
        /* SDA pin is High level */
        sts_flag.BIT.SDAI = 1U;
    }
    else
    {
        /* SDA pin is Low level */
        sts_flag.BIT.SDAI = 0U;
    }

    /* ---- Check SCL pin level ---- */
    if (RIIC_ICCR1_SCLI_SET == ((*piccr1_reg) & RIIC_ICCR1_SCLI))
    {
        /* SCL pin is High level */
        sts_flag.BIT.SCLI = 1U;
    }
    else
    {
        /* SCL pin is Low level */
        sts_flag.BIT.SCLI = 0U;
    }

    /* ---- Check SDA output control status ---- */
    if (RIIC_ICCR1_SDAO_SET == ((*piccr1_reg) & RIIC_ICCR1_SDAO))
    {
        /* SDA pin is Open */
        sts_flag.BIT.SDAO = 1U;
    }
    else
    {
        /* SDA pin is Low hold */
        sts_flag.BIT.SDAO = 0U;
    }

    /* ---- Check SCL output control status ---- */
    if (RIIC_ICCR1_SCLO_SET == ((*piccr1_reg) & RIIC_ICCR1_SCLO))
    {
        /* SCL pin is Open */
        sts_flag.BIT.SCLO = 1U;
    }
    else
    {
        /* SCL pin is Low hold */
        sts_flag.BIT.SCLO = 0U;
    }

    /* ---- Check Event detection ---- */
    if (RIIC_ICSR2_START_SET == ((*picsr2_reg) & RIIC_ICSR2_START))
    {
        /* Start condition detected*/
        sts_flag.BIT.ST = 1U;
    }
    else
    {
        /* Not detected */
        sts_flag.BIT.ST = 0U;
    }

    /* ---- Check Event detection ---- */
    if (RIIC_ICSR2_STOP_SET == ((*picsr2_reg) & RIIC_ICSR2_STOP))
    {
        /* Stop condition detected*/
        sts_flag.BIT.SP = 1U;
    }
    else
    {
        /* Not detected */
        sts_flag.BIT.SP = 0U;
    }

    /* ---- Check Event detection ---- */
    if (RIIC_ICSR2_AL_SET == ((*picsr2_reg) & RIIC_ICSR2_AL))
    {
        /* Arbitration lost detected*/
        sts_flag.BIT.AL = 1U;
    }
    else
    {
        /* Not detected */
        sts_flag.BIT.AL = 0U;
    }

    /* ---- Check Event detection ---- */
    if (RIIC_ICSR2_TMOF_SET == ((*picsr2_reg) & RIIC_ICSR2_TMOF))
    {
        /* Time out detected*/
        sts_flag.BIT.TMO = 1U;
    }
    else
    {
        /* Not detected */
        sts_flag.BIT.TMO = 0U;
    }

    /* ---- Check Master/Slave Mode ---- */
    if (RIIC_ICCR2_MST_SET == ((*piccr2_reg) & RIIC_ICCR2_MST))
    {
        /* Master Mode */
        sts_flag.BIT.MST = 1U;
    }
    else
    {
        /* Slave Mode */
        sts_flag.BIT.MST = 0U;
    }

    /* ---- Check Host Address detection ---- */
    if (RIIC_ICSR1_HOA_SET == ((*picsr1_reg) & RIIC_ICSR1_HOA))
    {
        /* Host Address detected */
        sts_flag.BIT.HOA = 1U;
    }
    else
    {
        /* Not detected */
        sts_flag.BIT.HOA = 0U;
    }

    /* ---- Check Device ID Address detection ---- */
    if (RIIC_ICSR1_DID_SET == ((*picsr1_reg) & RIIC_ICSR1_DID))
    {
        /* Device ID Address detected */
        sts_flag.BIT.DID = 1U;
    }
    else
    {
        /* Not detected */
        sts_flag.BIT.DID = 0U;
    }

    /* ---- Check Generalcall Address detection ---- */
    if (RIIC_ICSR1_GCA_SET == ((*picsr1_reg) & RIIC_ICSR1_GCA))
    {
        /* Generalcall Address detected */
        sts_flag.BIT.GCA = 1U;
    }
    else
    {
        /* Not detected */
        sts_flag.BIT.GCA = 0U;
    }

    /* ---- Check Slave0 Address detection ---- */
    if (RIIC_ICSR1_AAS0_SET == ((*picsr1_reg) & RIIC_ICSR1_AAS0))
    {
        /* Slave0 Address detected */
        sts_flag.BIT.AAS0 = 1U;
    }
    else
    {
        /* Not detected */
        sts_flag.BIT.AAS0 = 0U;
    }

    /* ---- Check Slave1 Address detection ---- */
    if (RIIC_ICSR1_AAS1_SET == ((*picsr1_reg) & RIIC_ICSR1_AAS1))
    {
        /* Slave1 Address detected */
        sts_flag.BIT.AAS1 = 1U;
    }
    else
    {
        /* Not detected */
        sts_flag.BIT.AAS1 = 0U;
    }

    /* ---- Check Slave2 Address detection ---- */
    if (RIIC_ICSR1_AAS2_SET == ((*picsr1_reg) & RIIC_ICSR1_AAS2))
    {
        /* Slave2 Address detected */
        sts_flag.BIT.AAS2 = 1U;
    }
    else
    {
        /* Not detected */
        sts_flag.BIT.AAS2 = 0U;
    }

    /* ---- Set the state ---- */
    *p_riic_status = sts_flag;

    /* ---- clear flag ---- */
    (*picsr2_reg) &= RIIC_ICSR2_AL_CLR;
    (*picsr2_reg) &= RIIC_ICSR2_NACKF_CLR;
    uctmp = *picsr2_reg;

    if (RIIC_AL == g_riic_ChStatus[p_riic_info->ch_no])
    {
        /* Initialize the channel status. */
        riic_set_ch_status(p_riic_info, RIIC_FINISH);
    }
} /* End of function riic_getstatus() */

/***********************************************************************************************************************
 * Function Name: R_RIIC_Control
 * Description  : RIIC scl sda control processing.
 *                This function is mainly used when a communication error occurs.
 *                Outputs control signals of the simple I2C mode.
 *                Outputs conditions specified by the argument, NACK,
 *                and one-shot of the SSCL clock. Also resets the simple I2C mode settings.
 * Arguments    : r_iic_info_t * p_r_iic_info    ; IIC Information
 *                uint8_t cntl_ptn               ; Output Pattern
 *                 [Options]
 *                  RIIC_GEN_START_CON
 *                  RIIC_GEN_STOP_CON
 *                  RIIC_GEN_RESTART_CON 
 *                  RIIC_GEN_SDA_HI_Z
 *                  RIIC_GEN_SCL_ONESHOT
 *                  RIIC_GEN_RESET
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 *              : RIIC_ERR_INVALID_CHAN          ; None existent channel number
 *              : RIIC_ERR_INVALID_ARG           ; Parameter error
 *              : RIIC_ERR_BUS_BUSY              ; Bus busy
 *              : RIIC_ERR_AL                    ; Arbitration lost error
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
riic_return_t R_RIIC_Control (riic_info_t * p_riic_info, uint8_t ctrl_ptn)
{
    riic_return_t ret;

    /* ---- CHECK ARGUMENTS ---- */
#if (1U == RIIC_CFG_PARAM_CHECKING_ENABLE)

    if ((NULL == p_riic_info) || (0 == ctrl_ptn))
    {
        return RIIC_ERR_INVALID_ARG;
    }

    /* ---- CHECK CHANNEL ---- */
    if (false == riic_mcu_check_channel(p_riic_info->ch_no))
    {
        return RIIC_ERR_INVALID_CHAN;
    }

#endif

    /* Calls the API function. */
    ret = riic_control(p_riic_info, ctrl_ptn);

    return ret;
} /* End of function R_RIIC_Control() */

/***********************************************************************************************************************
 * Function Name: riic_control
 * Description  : sub function of R_RIIC_Control.
 *                This function is mainly used when a communication error occurs.
 *                Outputs control signals of the simple I2C mode.
 *                Outputs conditions specified by the argument, NACK,
 *                and one-shot of the SSCL clock. Also resets the simple I2C mode settings.
 * Arguments    : r_iic_info_t * p_riic_info     ; IIC Information
 *                uint8_t cntl_ptn               ; Output Pattern
 *                 [Options]
 *                  RIIC_GEN_START_CON
 *                  RIIC_GEN_STOP_CON
 *                  RIIC_GEN_RESTART_CON 
 *                  RIIC_GEN_SDA_HI_Z
 *                  RIIC_GEN_SCL_ONESHOT
 *                  RIIC_GEN_RESET
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 *              : RIIC_ERR_INVALID_ARG           ; Parameter error
 *              : RIIC_ERR_BUS_BUSY              ; Bus busy
 *              : RIIC_ERR_AL                    ; Arbitration lost error
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
static riic_return_t riic_control (riic_info_t * p_riic_info, uint8_t ctrl_ptn)
{
    volatile riic_return_t ret = RIIC_SUCCESS;
    volatile uint8_t uctmp = 0x00;
    volatile uint32_t cnt = 0x00000000;

    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const picsr2_reg = RIIC_ICSR2_ADR(p_riic_info->ch_no);
    volatile uint8_t * const piccr1_reg = RIIC_ICCR1_ADR(p_riic_info->ch_no);
    volatile uint8_t * const piccr2_reg = RIIC_ICCR2_ADR(p_riic_info->ch_no);

    /* Disable all RIIC interrupt */
    riic_int_disable(p_riic_info);

    /* ==== Release reset(IICRST) of RIIC ==== */
    riic_reset_clear(p_riic_info);

    if (RIIC_ICCR1_ICE_SET != ((*piccr1_reg) & RIIC_ICCR1_ICE)) /* Check ICCR1.ICE = 0 */
    {
        /* Set ICE bit to enable RIIC modeule */
        (*piccr1_reg) |= RIIC_ICCR1_ICE_SET;
    }

    if ((0 != (uint8_t) (((RIIC_GEN_START_CON | RIIC_GEN_RESTART_CON) | RIIC_GEN_STOP_CON) & ctrl_ptn))
            && (0 == (uint8_t) (((RIIC_GEN_SDA_HI_Z | RIIC_GEN_SCL_ONESHOT) | RIIC_GEN_RESET) & ctrl_ptn)))
    {
        /* ==== Check request output pattern ==== */
        /* ---- Generate the start condition ---- */
        if (RIIC_GEN_START_CON == (ctrl_ptn & RIIC_GEN_START_CON))
        {
            /* Checks bus busy. */
            if (RIIC_ICCR2_BBSY_SET == ((*piccr2_reg) & RIIC_ICCR2_BBSY))
            {
                /* When BBSY bit is "1"(Bus busy) */
                ret = RIIC_ERR_BUS_BUSY;
                return ret;
            }

            /* Generates the start condition. */
            riic_start_cond_generate(p_riic_info);

            /* Wait the request generation has been completed */
            cnt = RIIC_CFG_BUS_CHECK_COUNTER;
            while (RIIC_ICSR2_START_SET != ((*picsr2_reg) & RIIC_ICSR2_START))
            {
                /* Check Arbitration */
                if (RIIC_ICSR2_AL_SET == ((*picsr2_reg) & RIIC_ICSR2_AL))
                {
                    /* When AL bit is "1" */
                    ret = RIIC_ERR_AL;
                    return ret;
                }

                /* Check Reply */
                if (0x00000000 < cnt)
                {
                    cnt--;
                }
                else
                {
                    /* When Non Reply */
                    ret = RIIC_ERR_BUS_BUSY;
                    return ret;
                }
            }

            /* Clear START detect bit */
            (*picsr2_reg) &= RIIC_ICSR2_START_CLR;
        }

        /* ---- Generate the restart condition ---- */
        if (RIIC_GEN_RESTART_CON == (ctrl_ptn & RIIC_GEN_RESTART_CON))
        {
            /* Checks bus busy. */
            if (RIIC_ICCR2_BBSY_SET != ((*piccr2_reg) & RIIC_ICCR2_BBSY))
            {
                /* When BBSY bit is "0"(Not Bus busy) */
                ret = RIIC_ERR_OTHER;
                return ret;
            }

            /* Generates the restart condition. */
            riic_re_start_cond_generate(p_riic_info);

            /* Wait the request generation has been completed*/
            cnt = RIIC_CFG_BUS_CHECK_COUNTER;
            while (RIIC_ICSR2_START_SET != ((*picsr2_reg) & RIIC_ICSR2_START))
            {
                /* Check Arbitration */
                if (RIIC_ICSR2_AL_SET == ((*picsr2_reg) & RIIC_ICSR2_AL))
                {
                    /* When AL bit is "1" */
                    ret = RIIC_ERR_AL;
                    return ret;
                }

                /* Check Reply */
                if (0x00000000 < cnt)
                {
                    cnt--;
                }
                else
                {
                    /* When Non Reply */
                    ret = RIIC_ERR_BUS_BUSY;
                    return ret;
                }
            }

            /* Clear START detect bit */
            (*picsr2_reg) &= RIIC_ICSR2_START_CLR;
        }

        /* ---- Generate the stop condition ---- */
        if (RIIC_GEN_STOP_CON == (ctrl_ptn & RIIC_GEN_STOP_CON))
        {
            /* Checks bus busy. */
            if (RIIC_ICCR2_BBSY_SET != ((*piccr2_reg) & RIIC_ICCR2_BBSY))
            {
                /* When BBSY bit is "0"(Not Bus busy) */
                ret = RIIC_ERR_OTHER;
                return ret;
            }

            /* Generates the stop condition. */
            riic_stop_cond_generate(p_riic_info);

            /* Wait the request generation has been completed*/
            cnt = RIIC_CFG_BUS_CHECK_COUNTER;
            while (RIIC_ICSR2_STOP_SET != ((*picsr2_reg) & RIIC_ICSR2_STOP))
            {
                /* Check Arbitration */
                if (RIIC_ICSR2_AL_SET == ((*picsr2_reg) & RIIC_ICSR2_AL))
                {
                    /* When AL bit is "1" */
                    ret = RIIC_ERR_AL;
                    return ret;
                }

                /* Check Reply */
                if (0x00000000 < cnt)
                {
                    cnt--;
                }
                else
                {
                    /* When Non Reply */
                    ret = RIIC_ERR_BUS_BUSY;
                    return ret;
                }
            }

            /* Clear STOP detect bit */
            (*picsr2_reg) &= RIIC_ICSR2_STOP_CLR;
        }
    }
    else if ((0x00 != (uint8_t) ((RIIC_GEN_SDA_HI_Z | RIIC_GEN_SCL_ONESHOT) & ctrl_ptn))
            && (0x00 == (uint8_t) ((((RIIC_GEN_START_CON | RIIC_GEN_RESTART_CON) | RIIC_GEN_STOP_CON) |
            RIIC_GEN_RESET) & ctrl_ptn)))
    {
        /* ---- Select SDA pin in a high-impedance state ---- */
        if (RIIC_GEN_SDA_HI_Z == (ctrl_ptn & RIIC_GEN_SDA_HI_Z))
        {
            /* Changes the SDA pin in a high-impedance state. */
            /* Sets for ICCR1.SDAO bit. */
            *piccr1_reg = (uint8_t) (((*piccr1_reg) | RIIC_ICCR1_SDAO_SET) & RIIC_ICCR1_SOWP_CLR);
        }

        /* ---- Generate OneShot of SSCL clock ---- */
        if (RIIC_GEN_SCL_ONESHOT == (ctrl_ptn & RIIC_GEN_SCL_ONESHOT))
        {
            /* Output SCL oneshot */
            (*piccr1_reg) |= RIIC_ICCR1_CLO_SET;

            /* Wait output scl oneshot has been completed */
            cnt = RIIC_CFG_BUS_CHECK_COUNTER;
            while (RIIC_ICCR1_CLO_SET == ((*piccr1_reg) & RIIC_ICCR1_CLO_SET))
            {
                /* Check Reply */
                if (0x00000000 < cnt)
                {
                    cnt--;
                }
                else
                {
                    /* Set SCLO bit is "1"(Hi-z). */
                    *piccr1_reg = (uint8_t) (((*piccr1_reg) | RIIC_ICCR1_SCLO_SET) & RIIC_ICCR1_SOWP_CLR);

                    /* When Non Reply */
                    ret = RIIC_ERR_BUS_BUSY;
                    return ret;
                }
            }
        }
    }
    else if ((0x00 != (uint8_t) ((RIIC_GEN_RESET) & ctrl_ptn))
            && (0x00 == (uint8_t) (((((RIIC_GEN_START_CON | RIIC_GEN_RESTART_CON) | RIIC_GEN_STOP_CON) |
            RIIC_GEN_SDA_HI_Z) | RIIC_GEN_SCL_ONESHOT) & ctrl_ptn)))
    {
        /* ---- Generate Reset ---- */
        /* Updates the channel status. */
        riic_set_ch_status(p_riic_info, RIIC_IDLE);

        /* Initialize API status on RAM */
        riic_api_mode_event_init(p_riic_info, RIIC_MODE_NONE);

        /* Initializes the IIC registers. */
        /* Sets the internal status. */
        /* Enables the interrupt. */
        /* Clears the RIIC reset or internal reset. */
        riic_enable(p_riic_info);
    }
    else
    {
        /* parameter error */
        ret = RIIC_ERR_INVALID_ARG;
    }
    uctmp = *piccr1_reg;

    riic_clear_ir_flag(p_riic_info); /* Clear IR flag for RXI and TXI */

    return ret;
} /* End of function riic_control() */

/***********************************************************************************************************************
 * Function Name: R_RIIC_Close
 * Description  : Resets the RIIC driver.
 *              : The processing performs internal reset by setting to ICCR1.IICRST bit.
 *              : Forcibly, stops the IIC communication.
 *              : After called the processing, channel status becomes "RIIC_NO_INIT".
 *              : When starts the communication again, please call an initialization processing.
 * Arguments    : riic_info_t * p_riic_info      ;   IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 *              : RIIC_ERR_INVALID_ARG           ; Parameter error
 *              : RIIC_ERR_INVALID_CHAN          ; channel number invalid for part
 **********************************************************************************************************************/
riic_return_t R_RIIC_Close (riic_info_t * p_riic_info)
{
    /* ---- CHECK ARGUMENTS ---- */
#if (1U == RIIC_CFG_PARAM_CHECKING_ENABLE)
    if (NULL == p_riic_info)
    {
        return RIIC_ERR_INVALID_ARG;
    }

    /* ---- CHECK CHANNEL ---- */
    if (false == riic_mcu_check_channel(p_riic_info->ch_no))
    {
        return RIIC_ERR_INVALID_CHAN;
    }

#endif

    /* ---- Hardware Unlock ---- */
    riic_mcu_hardware_unlock(p_riic_info->ch_no);

    /* Calls the API function. */
    riic_close(p_riic_info);

    return RIIC_SUCCESS;
} /* End of function R_RIIC_Close() */

/***********************************************************************************************************************
 * Function Name: riic_close
 * Description  : sub function of R_RIIC_Close. 
 *                Resets the RIIC driver.
 *              : The processing performs internal reset by setting to ICCR1.IICRST bit.
 *              : Forcibly, stops the IIC communication.
 *              : After called the processing, channel status becomes "RIIC_NO_INIT".
 *              : When starts the communication again, please call an initialization processing.
 * Arguments    : riic_info_t * p_riic_info      ;   IIC Information
 * Return Value : none
 **********************************************************************************************************************/
static void riic_close (riic_info_t * p_riic_info)
{
    /* Updates the channel status. */
    riic_set_ch_status(p_riic_info, RIIC_NO_INIT);

    /* Disables IIC. */
    riic_disable(p_riic_info);

#if (1U == RIIC_CFG_PORT_SET_PROCESSING)
    /* Disables RIIC multi-function pin controller after setting SCL and SDA to Hi-z by Reset. */
    /* Includes I/O register read operation at the end of the following function. */
    riic_mcu_mpc_disable(p_riic_info->ch_no);
#endif

    riic_mcu_power_off(p_riic_info->ch_no);
} /* End of function riic_close() */

/***********************************************************************************************************************
 * Function Name: R_RIIC_GetVersion
 * Description  : Returns the version of this module. The version number is 
 *                encoded such that the top two bytes are the major version
 *                number and the bottom two bytes are the minor version number.
 * Arguments    : none
 * Return Value : version number
 **********************************************************************************************************************/
R_PRAGMA_INLINE(R_RIIC_GetVersion)
uint32_t R_RIIC_GetVersion (void)
{
    uint32_t const version = (RIIC_VERSION_MAJOR << 16) | RIIC_VERSION_MINOR;

    return version;
} /* End of function R_RIIC_GetVersion() */

/***********************************************************************************************************************
 * Function Name: r_riic_advance
 * Description  : Advances the IIC communication.
 *              : The return value shows the communication result. Refer to the return value.
 * Arguments    : riic_info_t * p_riic_info     ;   IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 *              : RIIC_ERR_INVALID_ARG           ; Parameter error
 *              : RIIC_ERR_BUS_BUSY              ; Bus busy
 *              : RIIC_ERR_AL                    ; Arbitration lost error
 *              : RIIC_ERR_TMO                   ; Time out error
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
static riic_return_t r_riic_advance (riic_info_t * p_riic_info)
{
    riic_return_t ret;

    /* Parameter check */
    if (NULL == p_riic_info)
    {
        ret = RIIC_ERR_INVALID_ARG;
    }
    else
    {
        /* Calls the API function. */
        ret = riic_advance(p_riic_info);
    }

    return ret;
} /* End of function r_riic_advance() */

/***********************************************************************************************************************
 * Function Name: riic_advance
 * Description  : sub function of r_riic_advance.
 *                Advances the IIC communication.
 *              : The return value shows the communication result. Refer to the return value.
 * Arguments    : riic_info_t * p_riic_info     ;   IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 *              : RIIC_ERR_BUS_BUSY              ; Bus busy
 *              : RIIC_ERR_NO_INIT               ; Uninitialized state
 *              : RIIC_ERR_AL                    ; Arbitration lost error
 *              : RIIC_ERR_TMO                   ; Time out error
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
static riic_return_t riic_advance (riic_info_t * p_riic_info)
{
    volatile uint8_t * const picsr2_reg = RIIC_ICSR2_ADR(p_riic_info->ch_no);
    volatile uint8_t * const picier_reg = RIIC_ICIER_ADR(p_riic_info->ch_no);
    riic_return_t ret;

    /* Checks the channel status. */
    ret = riic_check_chstatus_advance(p_riic_info);
    if (RIIC_SUCCESS != ret)
    {
        return ret;
    }

    /* Event happened? */
    if (RIIC_EV_INIT != riic_api_event[p_riic_info->ch_no])
    {
        /* IIC communication processing */
        ret = riic_func_table(riic_api_event[p_riic_info->ch_no], p_riic_info);

        /* return value? */
        switch (ret)
        {
            case RIIC_SUCCESS :
                if (RIIC_STS_TMO == riic_api_info[p_riic_info->ch_no].N_status)
                {
                    /* Checks the callback function. */
                    if ((((((RIIC_MODE_M_SEND == riic_api_info[p_riic_info->ch_no].N_Mode)
                            || (RIIC_MODE_M_SEND == riic_api_info[p_riic_info->ch_no].B_Mode))
                            || (RIIC_MODE_M_RECEIVE == riic_api_info[p_riic_info->ch_no].N_Mode))
                            || (RIIC_MODE_M_RECEIVE == riic_api_info[p_riic_info->ch_no].B_Mode))
                            || (RIIC_MODE_M_SEND_RECEIVE == riic_api_info[p_riic_info->ch_no].N_Mode))
                            || (RIIC_MODE_M_SEND_RECEIVE == riic_api_info[p_riic_info->ch_no].B_Mode))
                    {
                        riic_set_ch_status(priic_info_m[p_riic_info->ch_no], RIIC_TMO);
                        if (NULL != g_riic_callbackfunc_m[p_riic_info->ch_no])
                        {
                            g_riic_callbackfunc_m[p_riic_info->ch_no]();
                        }
                    }

                    if ((RIIC_MODE_S_SEND == riic_api_info[p_riic_info->ch_no].N_Mode)
                            || (RIIC_MODE_S_RECEIVE == riic_api_info[p_riic_info->ch_no].N_Mode))
                    {
                        riic_set_ch_status(priic_info_s[p_riic_info->ch_no], RIIC_TMO);
                        if (NULL != g_riic_callbackfunc_s[p_riic_info->ch_no])
                        {
                            g_riic_callbackfunc_s[p_riic_info->ch_no]();
                        }
                    }
                }

                else if (((RIIC_STS_SP_COND_WAIT != riic_api_info[p_riic_info->ch_no].N_status)
                        || (0x00 != ((*picsr2_reg) & RIIC_ICSR2_STOP))) || (0x00 != ((*picier_reg) & RIIC_ICIER_SP)))
                {
                    /* Advances communication. (Not finished) */
                    /* Do Nothing */
                }
                else
                {
                    /* Finished communication.(after detect stop condition) */
                    /* Updates the channel status. */
                    if (RIIC_ICSR2_AL == ((*picsr2_reg) & RIIC_ICSR2_AL))
                    {
                        if (((RIIC_MODE_S_READY == riic_api_info[p_riic_info->ch_no].N_Mode)
                                || (RIIC_MODE_S_SEND == riic_api_info[p_riic_info->ch_no].N_Mode))
                                || (RIIC_MODE_S_RECEIVE == riic_api_info[p_riic_info->ch_no].N_Mode))
                        {
                            riic_set_ch_status(priic_info_s[p_riic_info->ch_no], RIIC_FINISH);
                        }

                        riic_set_ch_status(priic_info_m[p_riic_info->ch_no], RIIC_AL);
                    }
                    else if (RIIC_ICSR2_NACKF == ((*picsr2_reg) & RIIC_ICSR2_NACKF))
                    {
                        riic_set_ch_status(p_riic_info, RIIC_NACK);
                    }
                    else
                    {
                        riic_set_ch_status(p_riic_info, RIIC_FINISH);
                    }

                    /* Checks the callback function. */
                    if ((((((RIIC_MODE_M_SEND == riic_api_info[p_riic_info->ch_no].N_Mode)
                            || (RIIC_MODE_M_SEND == riic_api_info[p_riic_info->ch_no].B_Mode))
                            || (RIIC_MODE_M_RECEIVE == riic_api_info[p_riic_info->ch_no].N_Mode))
                            || (RIIC_MODE_M_RECEIVE == riic_api_info[p_riic_info->ch_no].B_Mode))
                            || (RIIC_MODE_M_SEND_RECEIVE == riic_api_info[p_riic_info->ch_no].N_Mode))
                            || (RIIC_MODE_M_SEND_RECEIVE == riic_api_info[p_riic_info->ch_no].B_Mode))
                    {
                        if (NULL != g_riic_callbackfunc_m[p_riic_info->ch_no])
                        {
                            g_riic_callbackfunc_m[p_riic_info->ch_no]();
                        }
                    }

                    if ((RIIC_MODE_S_SEND == riic_api_info[p_riic_info->ch_no].N_Mode)
                            || (RIIC_MODE_S_RECEIVE == riic_api_info[p_riic_info->ch_no].N_Mode))
                    {
                        if (NULL != g_riic_callbackfunc_s[p_riic_info->ch_no])
                        {
                            g_riic_callbackfunc_s[p_riic_info->ch_no]();
                        }
                    }

                    /* Return to the last mode */
                    if ((RIIC_MODE_S_READY == riic_api_info[p_riic_info->ch_no].N_Mode)
                            || (RIIC_MODE_S_READY == riic_api_info[p_riic_info->ch_no].B_Mode))
                    {
                        /* Initialize API status on RAM */
                        riic_api_mode_set(p_riic_info, RIIC_MODE_NONE);
                        riic_api_mode_set(p_riic_info, RIIC_MODE_S_READY);
                        riic_api_status_set(p_riic_info, RIIC_STS_IDLE_EN_SLV);

                        /* ---- Enables IIC bus interrupt enable register. ---- */
                        if (((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data1st)
                                && ((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data2nd))
                        {
                            /* Enables slave send and slave receive */
                            riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_TX_RX);
                        }
                        else if (((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data1st)
                                && ((uint8_t *) FIT_NO_PTR == priic_info_s[p_riic_info->ch_no]->p_data2nd))
                        {
                            /* Enable slave send */
                            riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_TX);
                        }
                        else if (((uint8_t *) FIT_NO_PTR == priic_info_s[p_riic_info->ch_no]->p_data1st)
                                && ((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data2nd))
                        {
                            /* Enable slave receive */
                            riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_RX);
                        }
                        else
                        {
                            /* Do Nothing */
                        }
                    }
                    else
                    {
                        /* Initialize API status on RAM */
                        riic_api_mode_set(p_riic_info, RIIC_MODE_NONE);
                        riic_api_status_set(p_riic_info, RIIC_STS_IDLE);

                        /* Initializes ICIER register. */
                        *picier_reg = RIIC_ICIER_INIT;
                        while (RIIC_ICIER_INIT != (*picier_reg))
                        {
                            /* Do Nothing */
                        }
                    }
                }
            break;

            default :

                /* Updates the channel status. */
                riic_set_ch_status(p_riic_info, RIIC_ERROR);
            break;
        }
    }

    /* Event nothing. */
    else
    {
        ret = RIIC_ERR_OTHER;
    }

    return ret;
} /* End of function riic_advance() */

/***********************************************************************************************************************
 * Function Name: riic_func_table
 * Description  : IIC Protocol Execution Processing.
 *                Executes the function which set in "pfunc".
 * Arguments    : riic_api_event_t event    ; Event
 *              : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : Refer to the each calling function.
 **********************************************************************************************************************/
static riic_return_t riic_func_table (riic_api_event_t event, riic_info_t * p_riic_info)
{
    riic_return_t ret;
    riic_return_t (*pFunc) (riic_info_t *);
    riic_api_status_t n_status;

    /* Acquires a now state. */
    n_status = riic_api_info[p_riic_info->ch_no].N_status;

    /* Checks the parameter. */
    if ((RIIC_STS_MAX > n_status) && (RIIC_EV_MAX > event))
    {
        /* Checks the appointed function. */
        if (NULL != gc_riic_mtx_tbl[n_status][event].proc)
        {
            /* Sets the function. */
            pFunc = gc_riic_mtx_tbl[n_status][event].proc;

            /* Event flag initialization. */
            riic_api_event[p_riic_info->ch_no] = RIIC_EV_INIT;

            /* Calls the status transition function. */
            ret = (*pFunc)(p_riic_info);
        }
        else
        {
            /* Appointed function error */
            ret = RIIC_ERR_OTHER;
        }
    }
    else
    {
        /* Appointed function error */
        ret = RIIC_ERR_OTHER;
    }

    return ret;
} /* End of function riic_func_table() */

/***********************************************************************************************************************
 * Function Name: riic_init_driver
 * Description  : Initialize IIC Driver Processing.
 *                Initializes the IIC registers.
 *              : Sets the internal status.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation, idle state
 **********************************************************************************************************************/
static riic_return_t riic_init_driver (riic_info_t * p_riic_info)
{
    /* Initializes the IIC registers. */
    /* Sets the internal status. */
    /* Enables the interrupt. */
    /* Clears the RIIC reset or internal reset. */
    riic_enable(p_riic_info);

    return RIIC_SUCCESS;
} /* End of function riic_init_driver() */

/***********************************************************************************************************************
 * Function Name: riic_generate_start_cond
 * Description  : Generate Start Condition Processing.
 *                Checks bus busy. Sets the internal status. Generates the start condition.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation, communication state
 *              : RIIC_ERR_BUS_BUSY              ; Bus busy error
 **********************************************************************************************************************/
static riic_return_t riic_generate_start_cond (riic_info_t * p_riic_info)
{
    riic_return_t ret;
    bool boolret;
    volatile uint8_t uctmp;

    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const picier_reg = RIIC_ICIER_ADR(p_riic_info->ch_no);
    volatile uint8_t * const picsr2_reg = RIIC_ICSR2_ADR(p_riic_info->ch_no);

    /* Checks bus busy. */
    boolret = riic_check_bus_busy(p_riic_info);
    if (RIIC_FALSE == boolret)
    {
        ret = RIIC_ERR_BUS_BUSY; /* Bus busy */
    }
    else
    {
        /* Clears each status flag. */
        (*picsr2_reg) &= RIIC_ICSR2_STOP_CLR;
        while (0x00 != (((*picsr2_reg) & RIIC_ICSR2_START) || ((*picsr2_reg) & RIIC_ICSR2_STOP)))
        {
            /* Do Nothing */
        }

        /* Sets the internal status. */
        riic_api_status_set(p_riic_info, RIIC_STS_ST_COND_WAIT);

        /* Clears ALIE bit. */
        (*picier_reg) &= (~RIIC_ICIER_AL);
        uctmp = *picier_reg; /* Reads ICIER. */

        /* Enables IIC bus interrupt enable register. */
        if (RIIC_MODE_S_READY == riic_api_info[p_riic_info->ch_no].B_Mode)
        {
            if (((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data1st)
                    && ((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data2nd))
            {
                /* Enables slave send and slave receive */
                riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_ST_NAK_AL | RIIC_ICIER_TX_RX);
            }
            else if (((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data1st)
                    && ((uint8_t *) FIT_NO_PTR == priic_info_s[p_riic_info->ch_no]->p_data2nd))
            {
                /* Enable slave send */
                riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_ST_NAK_AL | RIIC_ICIER_TX);
            }
            else if (((uint8_t *) FIT_NO_PTR == priic_info_s[p_riic_info->ch_no]->p_data1st)
                    && ((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data2nd))
            {
                /* Enable slave receive */
                riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_ST_NAK_AL | RIIC_ICIER_RX);
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            riic_int_icier_setting(p_riic_info, RIIC_ICIER_ST_NAK_AL);
        }

        /* Generates the start condition. */
        riic_start_cond_generate(p_riic_info);

        ret = RIIC_SUCCESS;
    }

    return ret;
} /* End of function riic_generate_start_cond() */

/***********************************************************************************************************************
 * Function Name: riic_after_gen_start_cond
 * Description  : After Start Condition Generation Processing.
 *                Performs one of the following processing according to the state.
 *                Transmits the slave address for writing.
 *                Transmits the slave address for reading.
 *                Generates the stop condition.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation, communication state
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
static riic_return_t riic_after_gen_start_cond (riic_info_t * p_riic_info)
{
    riic_return_t ret = RIIC_SUCCESS;
    uint8_t buf_send_data;
    volatile uint32_t cnt;
    bool scl_low_chk;

    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const piccr1_reg = RIIC_ICCR1_ADR(p_riic_info->ch_no);
    volatile uint8_t * const picsr2_reg = RIIC_ICSR2_ADR(p_riic_info->ch_no);

    /* IIC mode? */
    switch (riic_api_info[p_riic_info->ch_no].N_Mode)
    {
        case RIIC_MODE_M_SEND :
        case RIIC_MODE_M_SEND_RECEIVE :

            /* Checks the previous status. */
            switch (riic_api_info[p_riic_info->ch_no].B_status)
            {
                /* The previous status is idle. */
                case RIIC_STS_IDLE :
                case RIIC_STS_IDLE_EN_SLV :

                    /* Is the slave address pointer set? */
                    if ((uint8_t *) FIT_NO_PTR == p_riic_info->p_slv_adr) /* Pattern 4 of Master Write */
                    {
                        /* Sets the internal status. */
                        riic_api_status_set(p_riic_info, RIIC_STS_SP_COND_WAIT);

                        /* Enables the IIC bus interrupt. */
                        if (RIIC_MODE_S_READY == riic_api_info[p_riic_info->ch_no].B_Mode)
                        {
                            if (((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data1st)
                                    && ((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data2nd))
                            {
                                /* Enables slave send and slave receive */
                                riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_SP_NAK_AL |
                                RIIC_ICIER_TX_RX);
                            }
                            else if (((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data1st)
                                    && ((uint8_t *) FIT_NO_PTR == priic_info_s[p_riic_info->ch_no]->p_data2nd))
                            {
                                /* Enable slave send */
                                riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_SP_NAK_AL |
                                RIIC_ICIER_TX);
                            }
                            else if (((uint8_t *) FIT_NO_PTR == priic_info_s[p_riic_info->ch_no]->p_data1st)
                                    && ((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data2nd))
                            {
                                /* Enable slave receive */
                                riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_SP_NAK_AL |
                                RIIC_ICIER_RX);
                            }
                            else
                            {
                                /* Do Nothing */
                            }
                        }
                        else
                        {
                            riic_int_icier_setting(p_riic_info, RIIC_ICIER_SP_NAK_AL);
                        }

                        /* check SCL line */
                        scl_low_chk = false;
                        for (cnt = RIIC_CFG_SCL_CHECK_COUNTER; (false == scl_low_chk) && (cnt > 0x00000000); cnt--)
                        {
                            if (RIIC_ICCR1_SCLI_SET != ((*piccr1_reg) & RIIC_ICCR1_SCLI)) /* SCL low ? */
                            {
                                scl_low_chk = true;
                            }
                        }

                        if (false == scl_low_chk) /* time out ? */
                        {
                            return RIIC_ERR_OTHER;
                        }

                        /* Generates the stop condition. */
                        riic_stop_cond_generate(p_riic_info);
                    }
                    else
                    {
                        /* Sets a write code. */
                        buf_send_data = (uint8_t) ((*(p_riic_info->p_slv_adr)) << 1U);
                        buf_send_data &= W_CODE;

                        /* Sets the internal status. */
                        riic_api_status_set(p_riic_info, RIIC_STS_SEND_SLVADR_W_WAIT);

                        /* Enables the IIC bus interrupt. */
                        /* Transmit data empty interrupt request is enabled. */
                        if (RIIC_MODE_S_READY == riic_api_info[p_riic_info->ch_no].B_Mode)
                        {
                            if (((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data1st)
                                    && ((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data2nd))
                            {
                                /* Enables slave send and slave receive */
                                riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_TEND_NAK_AL |
                                RIIC_ICIER_TX_RX);
                            }
                            else if (((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data1st)
                                    && ((uint8_t *) FIT_NO_PTR == priic_info_s[p_riic_info->ch_no]->p_data2nd))
                            {
                                /* Enable slave send */
                                riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_TEND_NAK_AL |
                                RIIC_ICIER_TX);
                            }
                            else if (((uint8_t *) FIT_NO_PTR == priic_info_s[p_riic_info->ch_no]->p_data1st)
                                    && ((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data2nd))
                            {
                                /* Enable slave receive */
                                riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_TEND_NAK_AL |
                                RIIC_ICIER_RX);
                            }
                            else
                            {
                                /* Do Nothing */
                            }
                        }
                        else
                        {
                            riic_int_icier_setting(p_riic_info, RIIC_ICIER_TEND_NAK_AL);
                        }

                        /* Transmits the slave address. */
                        riic_set_sending_data(p_riic_info, &buf_send_data);
                    }
                break;

                    /* Previous status is data transfer completion waiting status. */
                case RIIC_STS_SEND_DATA_WAIT :

                    /* Sets a read code. */
                    buf_send_data = (uint8_t) ((*(p_riic_info->p_slv_adr)) << 1U);
                    buf_send_data |= R_CODE;

                    /* Sets the internal status. */
                    riic_api_status_set(p_riic_info, RIIC_STS_SEND_SLVADR_R_WAIT);

                    /* Enables the IIC bus interrupt. */
                    riic_int_icier_setting(p_riic_info, RIIC_ICIER_RX_NAK_AL);

                    /* Transmits the slave address. */
                    riic_set_sending_data(p_riic_info, &buf_send_data);

                break;

                default :

                    /* None status. */
                    ret = RIIC_ERR_OTHER;
                break;
            }
        break;

        case RIIC_MODE_M_RECEIVE :

            /* Sets a read code. */
            buf_send_data = (uint8_t) ((*(p_riic_info->p_slv_adr)) << 1U);
            buf_send_data |= R_CODE;

            /* Sets the internal status. */
            riic_api_status_set(p_riic_info, RIIC_STS_SEND_SLVADR_R_WAIT);

            /* Enables the IIC bus interrupt. */
            if (RIIC_MODE_S_READY == riic_api_info[p_riic_info->ch_no].B_Mode)
            {
                if (((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data1st)
                        && ((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data2nd))
                {
                    /* Enables slave send and slave receive */
                    riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_RX_NAK_AL |
                    RIIC_ICIER_TX_RX);
                }
                else if (((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data1st)
                        && ((uint8_t *) FIT_NO_PTR == priic_info_s[p_riic_info->ch_no]->p_data2nd))
                {
                    /* Enable slave send */
                    riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_RX_NAK_AL | RIIC_ICIER_TX);
                }
                else if (((uint8_t *) FIT_NO_PTR == priic_info_s[p_riic_info->ch_no]->p_data1st)
                        && ((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data2nd))
                {
                    /* Enable slave receive */
                    riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_RX_NAK_AL | RIIC_ICIER_RX);
                }
                else
                {
                    /* Do Nothing */
                }
            }
            else
            {
                riic_int_icier_setting(p_riic_info, RIIC_ICIER_RX_NAK_AL);
            }

            /* Transmits the slave address. */
            riic_set_sending_data(p_riic_info, &buf_send_data);

        break;

        default :
            ret = RIIC_ERR_OTHER;
        break;
    }
    return ret;
} /* End of function riic_after_gen_start_cond() */

/***********************************************************************************************************************
 * Function Name: riic_after_send_slvadr
 * Description  : After Slave Address Transmission Processing.
 *                Performs one of the following processing according to the state.
 *                Transmits a data. Generates the stop condition. Receives a data.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation, communication state
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
static riic_return_t riic_after_send_slvadr (riic_info_t * p_riic_info)
{
    riic_return_t ret = RIIC_SUCCESS;
    volatile uint8_t uctmp = 0x00;

    /* IIC mode? */
    switch (riic_api_info[p_riic_info->ch_no].N_Mode)
    {
        case RIIC_MODE_M_SEND :

            /* --- Pattern 1 of Master Write --- */
            if (((uint8_t *) FIT_NO_PTR != (uint8_t *) p_riic_info->p_data1st)
                    && ((uint8_t *) FIT_NO_PTR != (uint8_t *) p_riic_info->p_data2nd))
            {
                /* Sets the internal status. */
                riic_api_status_set(p_riic_info, RIIC_STS_SEND_DATA_WAIT);

                /* Enables the IIC bus interrupt. */
                riic_int_icier_setting(p_riic_info, RIIC_ICIER_TEND_NAK_AL);

                /* 1st data counter = 0?  */
                if (0x00000000 != p_riic_info->cnt1st)
                {
                    /* Transmits a 1st data. */
                    riic_set_sending_data(p_riic_info, p_riic_info->p_data1st);

                    /* Decreases the 1st data counter. */
                    p_riic_info->cnt1st--;

                    /* Increases the 1st data buffer pointer. */
                    p_riic_info->p_data1st++;
                }
                else
                {
                    ret = RIIC_ERR_OTHER;
                }
            }

            /* --- Pattern 2 of Master Write --- */
            else if (((uint8_t *) FIT_NO_PTR == p_riic_info->p_data1st)
                    && ((uint8_t *) FIT_NO_PTR != p_riic_info->p_data2nd))
            {
                /* Sets the internal status. */
                riic_api_status_set(p_riic_info, RIIC_STS_SEND_DATA_WAIT);

                /* Enables the IIC bus interrupt. */
                riic_int_icier_setting(p_riic_info, RIIC_ICIER_TEND_NAK_AL);

                /* 2nd data counter = 0?  */
                if (0x00000000 != p_riic_info->cnt2nd)
                {
                    /* Transmits a 2nd data. */
                    riic_set_sending_data(p_riic_info, p_riic_info->p_data2nd);

                    /* Decreases the 2nd data counter. */
                    p_riic_info->cnt2nd--;

                    /* Increases the 2nd data buffer pointer. */
                    p_riic_info->p_data2nd++;
                }
                else
                {
                    ret = RIIC_ERR_OTHER;
                }
            }

            /* --- Pattern 3 of Master Write --- */
            else if (((uint8_t *) FIT_NO_PTR == p_riic_info->p_data1st)
                    && ((uint8_t *) FIT_NO_PTR == p_riic_info->p_data2nd))
            {
                /* Sets the internal status. */
                riic_api_status_set(p_riic_info, RIIC_STS_SP_COND_WAIT);

                /* Enables the IIC bus interrupt. */
                riic_int_icier_setting(p_riic_info, RIIC_ICIER_SP_NAK_AL);

                /* Generates the stop condition. */
                riic_stop_cond_generate(p_riic_info);
            }
            else
            {
                ret = RIIC_ERR_OTHER;
            }

        break;

        case RIIC_MODE_M_RECEIVE :

            /* Sets the internal status. */
            riic_api_status_set(p_riic_info, RIIC_STS_RECEIVE_DATA_WAIT);

            /* Enables the IIC bus interrupt. */
            riic_int_icier_setting(p_riic_info, RIIC_ICIER_RX_NAK_AL);

            if (2U >= p_riic_info->cnt2nd)
            {
                /* The period between the ninth clock cycle and the first clock cycle is held low. */
                riic_receive_wait_setting(p_riic_info);
            }

            if (0x00000001 >= p_riic_info->cnt2nd)
            {
                /* Processing before receiving the "last byte - 1byte" in RX. */
                riic_receive_pre_end_setting(p_riic_info);
            }

            /* Performs dummy read. */
            uctmp = riic_get_receiving_data(p_riic_info);

        break;

        case RIIC_MODE_M_SEND_RECEIVE :

            /* Now status? */
            switch (riic_api_info[p_riic_info->ch_no].N_status)
            {
                case RIIC_STS_SEND_SLVADR_W_WAIT :

                    /* Sets the internal status. */
                    riic_api_status_set(p_riic_info, RIIC_STS_SEND_DATA_WAIT);

                    /* Enables the IIC bus interrupt. */
                    riic_int_icier_setting(p_riic_info, RIIC_ICIER_TEND_NAK_AL);

                    /* Transmits a 1st data. */
                    riic_set_sending_data(p_riic_info, p_riic_info->p_data1st);

                    /* Decreases the 1st data counter. */
                    p_riic_info->cnt1st--;

                    /* Increases the 1st Data buffer pointer. */
                    p_riic_info->p_data1st++;

                break;

                case RIIC_STS_SEND_SLVADR_R_WAIT :

                    /* Sets the internal status. */
                    riic_api_status_set(p_riic_info, RIIC_STS_RECEIVE_DATA_WAIT);

                    /* Enables the IIC bus interrupt. */
                    riic_int_icier_setting(p_riic_info, RIIC_ICIER_RX_NAK_AL);

                    if (2U >= p_riic_info->cnt2nd)
                    {
                        /* The period between the ninth clock cycle and the first clock cycle is held low. */
                        riic_receive_wait_setting(p_riic_info);
                    }

                    if (0x00000001 >= p_riic_info->cnt2nd)
                    {
                        /* Processing before receiving the "last byte - 1byte" in RX. */
                        riic_receive_pre_end_setting(p_riic_info);
                    }

                    /* Performs dummy read. */
                    uctmp = riic_get_receiving_data(p_riic_info);

                break;

                default :
                    ret = RIIC_ERR_OTHER;
                break;
            }
        break;

        default :
            ret = RIIC_ERR_OTHER;
        break;

    }

    return ret;
} /* End of function riic_after_send_slvadr() */

/***********************************************************************************************************************
 * Outline      : 
 * Function Name: riic_after_receive_slvadr
 * Description  : After Slave Address Receive Processing
 *                Performs one of the following processing according to the state.
 *                Transmits a data. Generates the stop condition. Receives a data.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation, communication state
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
static riic_return_t riic_after_receive_slvadr (riic_info_t * p_riic_info)
{
    volatile uint8_t * const picsr2_reg = RIIC_ICSR2_ADR(p_riic_info->ch_no);
    volatile uint8_t * const piccr2_reg = RIIC_ICCR2_ADR(p_riic_info->ch_no);
    volatile uint8_t uctmp = 0x00;
    uint8_t blank_data[1] =
    { BLANK };

    /* Clears each status flag. */
    (*picsr2_reg) &= RIIC_ICSR2_STOP_CLR;
    while (0x00 != ((*picsr2_reg) & RIIC_ICSR2_STOP))
    {
        /* Do Nothing */
    }

    /* ---- Enables IIC bus interrupt enable register. ---- */
    if (((uint8_t *) FIT_NO_PTR != p_riic_info->p_data1st) && ((uint8_t *) FIT_NO_PTR != p_riic_info->p_data2nd))
    {
        /* Enables slave send and slave receive */
        riic_int_icier_setting(p_riic_info, RIIC_ICIER_TX_RX_SP_NAK);
    }
    else if (((uint8_t *) FIT_NO_PTR != p_riic_info->p_data1st) && ((uint8_t *) FIT_NO_PTR == p_riic_info->p_data2nd))
    {
        /* Enable slave send */
        riic_int_icier_setting(p_riic_info, RIIC_ICIER_TX_SP_NAK);
    }
    else if (((uint8_t *) FIT_NO_PTR == p_riic_info->p_data1st) && ((uint8_t *) FIT_NO_PTR != p_riic_info->p_data2nd))
    {
        /* Enable slave receive */
        riic_int_icier_setting(p_riic_info, RIIC_ICIER_RX_SP);
    }
    else
    {
        /* Do Nothing */
    }

    /* Slave receive mode? */
    if (0x00 == ((*piccr2_reg) & RIIC_ICCR2_TRS))
    {
        /* Sets the internal status. */
        riic_api_status_set(p_riic_info, RIIC_STS_RECEIVE_DATA_WAIT);

        if (0x00000002 >= p_riic_info->cnt2nd)
        {
            /* The period between the ninth clock cycle and the first clock cycle is held low. */
            riic_receive_wait_setting(p_riic_info);
        }

        if (0x00000001 >= p_riic_info->cnt2nd)
        {
            /* Processing before receiving the "last byte - 1byte" in RX. */
            riic_receive_pre_end_setting(p_riic_info);
        }

        /* Performs dummy read. */
        uctmp = riic_get_receiving_data(p_riic_info);
    }

    /* Slave send mode */
    else
    {
        /* Sets the internal status. */
        riic_api_status_set(p_riic_info, RIIC_STS_SEND_DATA_WAIT);

        /* 1st data counter = 0?  */
        if (0x00000000 != p_riic_info->cnt1st)
        {
            if ((uint8_t *) FIT_NO_PTR == p_riic_info->p_data1st)
            {
                /* Internal error */
                return RIIC_ERR_OTHER;
            }

            /* Transmits a 1st data. */
            riic_set_sending_data(p_riic_info, p_riic_info->p_data1st);

            /* Decreases the 1st data counter. */
            p_riic_info->cnt1st--;

            /* Increases the 1st data buffer pointer. */
            p_riic_info->p_data1st++;
        }
        else
        {
            /* Transmits a "FFh" data. */
            riic_set_sending_data(p_riic_info, blank_data);
        }

    }

    return RIIC_SUCCESS;

} /* End of function riic_after_receive_slvadr() */

/***********************************************************************************************************************
 * Function Name: riic_write_data_sending
 * Description  : After Transmitting Data Processing.
 *                Performs one of the following processing  according to the state.
 *                Transmits a data. Generates stop condition. Generates restart condition.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation, communication state
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
static riic_return_t riic_write_data_sending (riic_info_t * p_riic_info)
{
    riic_return_t ret = RIIC_SUCCESS;
    uint8_t blank_data[1] =
    { BLANK };

    /* IIC mode? */
    switch (riic_api_info[p_riic_info->ch_no].N_Mode)
    {
        case RIIC_MODE_M_SEND :

            /* Is 1st data pointer set? */
            if ((uint8_t *) FIT_NO_PTR != p_riic_info->p_data1st)
            {
                /* 1st data counter = 0?  */
                if (0x00000000 != p_riic_info->cnt1st) /* Pattern 1 of Master Write */
                {
                    /* Transmits a 1st data. */
                    riic_set_sending_data(p_riic_info, p_riic_info->p_data1st);

                    /* Decreases the 1st data counter. */
                    p_riic_info->cnt1st--;

                    /* Increases the 1st data buffer pointer. */
                    p_riic_info->p_data1st++;

                    break;
                }
            }

            /* Is 2nd data pointer set? */
            if ((uint8_t *) FIT_NO_PTR != p_riic_info->p_data2nd)
            {
                /* 2nd data counter = 0? */
                if (0x00000000 != p_riic_info->cnt2nd) /* Pattern 2 of Master Write */
                {
                    /* Transmits a 2nd data. */
                    riic_set_sending_data(p_riic_info, p_riic_info->p_data2nd);

                    /* Decreases the 2nd data counter. */
                    p_riic_info->cnt2nd--;

                    /* Increases the 2nd data buffer pointer. */
                    p_riic_info->p_data2nd++;

                    break;
                }
            }

            /* Sets the internal status. */
            riic_api_status_set(p_riic_info, RIIC_STS_SP_COND_WAIT);

            /* Enables the IIC bus interrupt. */
            riic_int_icier_setting(p_riic_info, RIIC_ICIER_SP_NAK_AL);

            /* Generates the stop condition. */
            riic_stop_cond_generate(p_riic_info);

        break;

        case RIIC_MODE_M_SEND_RECEIVE :

            /* Is 1st data pointer set? */
            if (0x00000000 != p_riic_info->cnt1st)
            {
                /* Transmits a 1st data. */
                riic_set_sending_data(p_riic_info, p_riic_info->p_data1st);

                /* Decreases the 1st data counter. */
                p_riic_info->cnt1st--;

                /* Increases the 1st data buffer pointer. */
                p_riic_info->p_data1st++;

                break;
            }

            /* Sets the internal status. */
            riic_api_status_set(p_riic_info, RIIC_STS_ST_COND_WAIT);

            /* Enables the IIC bus interrupt. */
            riic_int_icier_setting(p_riic_info, RIIC_ICIER_ST_NAK_AL);

            /* Restarts the condition generation */
            riic_re_start_cond_generate(p_riic_info);

        break;

        case RIIC_MODE_S_SEND :

            /* Is 1st data pointer set? */
            if ((uint8_t *) FIT_NO_PTR != p_riic_info->p_data1st)
            {
                /* 1st data counter = 0?  */
                if (0x00000000 != p_riic_info->cnt1st)
                {
                    /* Transmits a 1st data. */
                    riic_set_sending_data(p_riic_info, p_riic_info->p_data1st);

                    /* Decreases the 1st data counter. */
                    p_riic_info->cnt1st--;

                    /* Increases the 1st data buffer pointer. */
                    p_riic_info->p_data1st++;
                }
                else
                {
                    /* Transmits a "FFh" data. */
                    riic_set_sending_data(p_riic_info, blank_data);
                }
            }
            else
            {
                /* Transmits a "FFh" data. */
                riic_set_sending_data(p_riic_info, blank_data);
            }

        break;

        default :
            ret = RIIC_ERR_OTHER;
        break;
    }

    return ret;
} /* End of function riic_write_data_sending() */

/***********************************************************************************************************************
 * Function Name: riic_read_data_receiving
 * Description  : After Receiving Data Processing.
 *                Performs one of the following processing  according to the state.
 *                Receives a data. Generates stop condition.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation, communication state
 **********************************************************************************************************************/
static riic_return_t riic_read_data_receiving (riic_info_t * p_riic_info)
{
    volatile uint8_t uctmp = 0x00;

    if (0x00000003 >= p_riic_info->cnt2nd)
    {
        /* The period between the ninth clock cycle and the first clock cycle is held low. */
        riic_receive_wait_setting(p_riic_info);
    }

    if (0x00000002 >= p_riic_info->cnt2nd)
    {
        /* Processing before receiving the "last byte - 1byte" in RX. */
        riic_receive_pre_end_setting(p_riic_info);
    }

    if (0x00000001 >= p_riic_info->cnt2nd)
    {
        if (RIIC_MODE_S_RECEIVE != riic_api_info[p_riic_info->ch_no].N_Mode)
        {
            /* Sets the internal status. */
            riic_api_status_set(p_riic_info, RIIC_STS_SP_COND_WAIT);

            /* Enables the IIC bus interrupt. */
            riic_int_icier_setting(p_riic_info, RIIC_ICIER_SP_NAK_AL);

            /* Generates the stop condition. */
            riic_stop_cond_generate(p_riic_info);
        }

        if (0x00000000 != p_riic_info->cnt2nd)
        {
            /* Stores the received data. */
            *p_riic_info->p_data2nd = riic_get_receiving_data(p_riic_info);
            p_riic_info->cnt2nd--; /* Decreases the receiving data counter. */
            p_riic_info->p_data2nd++; /* Increases the receiving data buffer pointer. */
        }
        else
        {
            /* Performs dummy read. */
            uctmp = riic_get_receiving_data(p_riic_info);
        }

        /* Reception end setting */
        riic_receive_end_setting(p_riic_info);
    }
    else
    {
        /* Stores the received data. */
        *p_riic_info->p_data2nd = riic_get_receiving_data(p_riic_info);
        p_riic_info->cnt2nd--; /* Decreases the receiving data counter. */
        p_riic_info->p_data2nd++; /* Increases the receiving data buffer pointer. */
    }

    return RIIC_SUCCESS;
} /* End of function riic_read_data_receiving() */

/***********************************************************************************************************************
 * Function Name: riic_after_dtct_stop_cond
 * Description  : After Generating Stop Condition Processing.
 *                Checks bus busy.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation, finished communication and idle state
 *              : RIIC_ERR_BUS_BUSY              ; Bus busy error
 *              : RIIC_ERR_AL                    ; Arbitration lost error
 *              : RIIC_ERR_TMO                  ; Time out error
 **********************************************************************************************************************/
static riic_return_t riic_after_dtct_stop_cond (riic_info_t * p_riic_info)
{
    riic_return_t ret = RIIC_SUCCESS;
    bool boolret = RIIC_FALSE;
    volatile uint8_t * const picser_reg = RIIC_ICSER_ADR(p_riic_info->ch_no);

    /* Waits from "bus busy" to "bus ready". */
    boolret = riic_check_bus_busy(p_riic_info);
    if (RIIC_FALSE == boolret)
    {
        ret = RIIC_ERR_BUS_BUSY;
    }
    else
    {
        if (RIIC_TMO == g_riic_ChStatus[p_riic_info->ch_no])
        {
            ret = RIIC_ERR_TMO;
        }

        if (RIIC_AL == g_riic_ChStatus[p_riic_info->ch_no])
        {
            ret = RIIC_ERR_AL;
        }
    }

    if ((RIIC_MODE_S_SEND == riic_api_info[p_riic_info->ch_no].N_Mode)
            || (RIIC_MODE_S_RECEIVE == riic_api_info[p_riic_info->ch_no].N_Mode))
    {
        /* Disable slave address1 */
        (*picser_reg) &= RIIC_ICSER_SAR_DASABLE;
    }

    return ret;
} /* End of function riic_after_dtct_stop_cond() */

/***********************************************************************************************************************
 * Function Name: riic_arbitration_lost
 * Description  : Arbitration Lost Error Processing. 
 *                Returns RIIC_ERR_AL.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                    ; Arbitration lost error
 **********************************************************************************************************************/
static riic_return_t riic_arbitration_lost (riic_info_t * p_riic_info)
{
    /* Checks the internal mode. */
    /* slave transfer not enable in now */
    if (((RIIC_MODE_M_SEND == riic_api_info[p_riic_info->ch_no].N_Mode)
            || (RIIC_MODE_M_RECEIVE == riic_api_info[p_riic_info->ch_no].N_Mode))
            || (RIIC_MODE_M_SEND_RECEIVE == riic_api_info[p_riic_info->ch_no].N_Mode))
    {
        /* slave transfer not enable in now and before */
        if ((((RIIC_MODE_M_SEND == riic_api_info[p_riic_info->ch_no].B_Mode)
                || (RIIC_MODE_M_RECEIVE == riic_api_info[p_riic_info->ch_no].B_Mode))
                || (RIIC_MODE_M_SEND_RECEIVE == riic_api_info[p_riic_info->ch_no].B_Mode))
                || (RIIC_MODE_NONE == riic_api_info[p_riic_info->ch_no].B_Mode))
        {
            /* Sets the internal status. */
            riic_api_status_set(p_riic_info, RIIC_STS_SP_COND_WAIT);

            /* Enables the IIC bus interrupt. */
            riic_int_icier_setting(p_riic_info, RIIC_ICIER_SP);

        }
        else if (RIIC_MODE_S_READY == riic_api_info[p_riic_info->ch_no].B_Mode)
        {
            /* Initialize API status on RAM */
            riic_api_mode_event_init(priic_info_s[p_riic_info->ch_no], RIIC_MODE_S_READY);

            /* Sets the internal status. */
            riic_api_status_set(priic_info_s[p_riic_info->ch_no], RIIC_STS_AL);

            /* ---- Enables IIC bus interrupt enable register. ---- */
            if (((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data1st)
                    && ((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data2nd))
            {
                /* Enables slave send and slave receive */
                riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_TX_RX_SP_NAK);
            }
            else if (((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data1st)
                    && ((uint8_t *) FIT_NO_PTR == priic_info_s[p_riic_info->ch_no]->p_data2nd))
            {
                /* Enable slave send */
                riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_TX_SP_NAK);
            }
            else if (((uint8_t *) FIT_NO_PTR == priic_info_s[p_riic_info->ch_no]->p_data1st)
                    && ((uint8_t *) FIT_NO_PTR != priic_info_s[p_riic_info->ch_no]->p_data2nd))
            {
                /* Enable slave receive */
                riic_int_icier_setting(priic_info_s[p_riic_info->ch_no], RIIC_ICIER_RX_SP);
            }
            else
            {
                /* Do Nothing */
            }
        }
        else
        {
            /* Do Nothing */
        }
    }

    /* slave transfer enable in now */
    else if ((RIIC_MODE_S_SEND == riic_api_info[p_riic_info->ch_no].N_Mode)
            || (RIIC_MODE_S_RECEIVE == riic_api_info[p_riic_info->ch_no].N_Mode))
    {
        /* Do Nothing */
    }
    else
    {
        /* Do Nothing */
    }

    return RIIC_SUCCESS;
} /* End of function riic_arbitration_lost() */

/***********************************************************************************************************************
 * Function Name: riic_time_out
 * Description  : Time Out Error Processing.
 *                Generates the time out error and returns RIIC_TMO.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Arbitration lost error
 **********************************************************************************************************************/
static riic_return_t riic_time_out (riic_info_t * p_riic_info)
{
    /* Sets the internal status. */
    riic_api_status_set(p_riic_info, RIIC_STS_TMO);

    return RIIC_SUCCESS;
} /* End of function riic_time_out() */

/***********************************************************************************************************************
 * Function Name: riic_nack
 * Description  : Nack Error Processing.
 *                Generates the stop condition and returns RIIC_NACK.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 **********************************************************************************************************************/
static riic_return_t riic_nack (riic_info_t * p_riic_info)
{
    volatile uint8_t * const piccr2_reg = RIIC_ICCR2_ADR(p_riic_info->ch_no);
    volatile uint8_t uctmp = 0x00;

    /* Sets the internal status. */
    riic_api_status_set(p_riic_info, RIIC_STS_SP_COND_WAIT);

    if (RIIC_ICCR2_MST_SET == ((*piccr2_reg) & RIIC_ICCR2_MST))
    {
        /* Enables the IIC bus interrupt. */
        riic_int_icier_setting(p_riic_info, RIIC_ICIER_SP_AL);

        /* Generates the stop condition. */
        riic_stop_cond_generate(p_riic_info);
    }
    else
    {
        /* Enables the IIC bus interrupt. */
        riic_int_icier_setting(p_riic_info, RIIC_ICIER_SP);
    }

    /* Checks the internal mode. */
    if ((((RIIC_MODE_M_RECEIVE == riic_api_info[p_riic_info->ch_no].N_Mode)
            || (RIIC_MODE_M_SEND_RECEIVE == riic_api_info[p_riic_info->ch_no].N_Mode))
            || (RIIC_MODE_S_SEND == riic_api_info[p_riic_info->ch_no].N_Mode))
            || (RIIC_MODE_S_RECEIVE == riic_api_info[p_riic_info->ch_no].N_Mode))
    {
        /* Dummy Read */
        /* Refer to the RX User's Manual. */
        uctmp = riic_get_receiving_data(p_riic_info);
    }

    return RIIC_SUCCESS;
} /* End of function riic_nack() */

/***********************************************************************************************************************
 * Function Name: riic_enable_slave_transfer
 * Description  : Enable IIC Slave Transfer Processing.
 *                Enable the slave address match interrupt. Sets the internal status.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation, idle state on enable slave transfer
 **********************************************************************************************************************/
static riic_return_t riic_enable_slave_transfer (riic_info_t * p_riic_info)
{
    /* Setting the IIC registers. */
    /* Includes I/O register read operation at the end of the following function. */
    riic_slv_addr_match_int_enable(p_riic_info);

    /* Sets the internal status. */
    riic_api_status_set(p_riic_info, RIIC_STS_IDLE_EN_SLV);

    return RIIC_SUCCESS;
} /* End of function riic_enable_slave_transfer() */

/***********************************************************************************************************************
 * Function Name: riic_api_mode_event_init
 * Description  : Initializes RAM.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 *              : riic_api_mode_t new_mode  ; New mode
 * Return Value : None
 **********************************************************************************************************************/
static void riic_api_mode_event_init (riic_info_t * p_riic_info, riic_api_mode_t new_mode)
{
    /* Clears the event flag. */
    riic_api_event[p_riic_info->ch_no] = RIIC_EV_INIT;

    /* Sets the previous mode. */
    riic_api_info[p_riic_info->ch_no].B_Mode = riic_api_info[p_riic_info->ch_no].N_Mode;

    /* Sets the mode to RIIC_MODE_NONE. */
    riic_api_info[p_riic_info->ch_no].N_Mode = new_mode;
} /* End of function riic_api_mode_event_init() */

/***********************************************************************************************************************
 * Function Name: riic_api_mode_set
 * Description  : Update Internal Status Processing.
 *                Updates the now status and the previous status.
 * Arguments    : riic_info_t * p_riic_info          ; IIC Information
 *              : riic_api_mode_t new_status    ; New status
 * Return Value : None
 **********************************************************************************************************************/
static void riic_api_mode_set (riic_info_t * p_riic_info, riic_api_mode_t new_mode)
{
    /* Sets the previous status. */
    riic_api_info[p_riic_info->ch_no].B_Mode = riic_api_info[p_riic_info->ch_no].N_Mode;

    /* Sets the now status. */
    riic_api_info[p_riic_info->ch_no].N_Mode = new_mode;
} /* End of function riic_api_mode_set() */

/***********************************************************************************************************************
 * Function Name: riic_api_status_set
 * Description  : Update Internal Status Processing.
 *                Updates the now status and the previous status.
 * Arguments    : riic_info_t * p_riic_info          ; IIC Information
 *              : riic_api_status_t new_status  ; New status
 * Return Value : None
 **********************************************************************************************************************/
static void riic_api_status_set (riic_info_t * p_riic_info, riic_api_status_t new_status)
{
    /* Sets the previous status. */
    riic_api_info[p_riic_info->ch_no].B_status = riic_api_info[p_riic_info->ch_no].N_status;

    /* Sets the now status. */
    riic_api_info[p_riic_info->ch_no].N_status = new_status;
} /* End of function riic_api_status_set() */

/***********************************************************************************************************************
 * Function Name: riic_set_ch_status
 * Description  : Updates the channel status.
 * Arguments    : riic_info_t * p_riic_info          ; IIC Information
 *              : riic_ch_dev_status_t status               ; channel status, device status
 * Return Value : None
 **********************************************************************************************************************/
static void riic_set_ch_status (riic_info_t * p_riic_info, riic_ch_dev_status_t status)
{
    /* Sets the channel status. */
    g_riic_ChStatus[p_riic_info->ch_no] = status;

    /* Sets the device status. */
    p_riic_info->dev_sts = status;
} /* End of function riic_set_ch_status() */

/***********************************************************************************************************************
 * Function Name: riic_check_chstatus_start
 * Description  : Check Channel Status Processing (at Start Function).
 *                When calls the starting function, checks the channel status.
 * Arguments    : riic_info_t * p_riic_info          ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 *              : RIIC_ERR_NO_INIT               ; Uninitialized state
 *              : RIIC_ERR_BUS_BUSY              ; Bus busy
 *              : RIIC_ERR_AL                    ; Arbitration lost error
 *              : RIIC_ERR_TMO                   ; Time out error
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
static riic_return_t riic_check_chstatus_start (riic_info_t * p_riic_info)
{
    volatile uint8_t * const piccr2_reg = RIIC_ICCR2_ADR(p_riic_info->ch_no);
    riic_return_t ret;

    /* Checks the channel status. */
    switch (g_riic_ChStatus[p_riic_info->ch_no])
    {
        case RIIC_NO_INIT :

            /* It is necessary to reinitialize. */
            /* Sets the return value to uninitialized state. */
            ret = RIIC_ERR_NO_INIT;
        break;

        case RIIC_IDLE :
        case RIIC_FINISH :
        case RIIC_NACK :

            /* Checks the device status. */
            if ((((RIIC_NO_INIT == p_riic_info->dev_sts) || (RIIC_IDLE == p_riic_info->dev_sts))
                    || (RIIC_FINISH == p_riic_info->dev_sts)) || (RIIC_NACK == p_riic_info->dev_sts))
            {
                /* Sets the return value to success state. */
                ret = RIIC_SUCCESS;
            }
            else if (RIIC_COMMUNICATION == p_riic_info->dev_sts)
            {
                /* Another device performed a driver reset processing
                 when this device is in idle state or communicating state. */
                /* It is necessary to reinitialize. */
                /* Sets the return value to bus busy state. */
                ret = RIIC_ERR_BUS_BUSY;
            }
            else if (RIIC_TMO == p_riic_info->dev_sts)
            {
                /* The device is in error state. */
                /* Sets the return value to time out error state. */
                ret = RIIC_ERR_TMO;
            }
            else if (RIIC_AL == p_riic_info->dev_sts)
            {
                /* The device is in error state. */
                /* Sets the return value to error state. */
                ret = RIIC_ERR_AL;
            }
            else
            {
                ret = RIIC_ERR_OTHER;
            }
        break;

        case RIIC_COMMUNICATION :

            /* The device or another device is on communication. */
            /* Sets the return value to communication state. */
            ret = RIIC_ERR_BUS_BUSY;
        break;

        case RIIC_TMO :

            /* The channel is in error state. */
            /* Sets the return value to time out error state. */
            ret = RIIC_ERR_TMO;
        break;

        case RIIC_AL :

            /* The channel is in error state. */
            /* Sets the return value to error state. */
            ret = RIIC_ERR_AL;
        break;

        default :
            ret = RIIC_ERR_OTHER;
        break;
    }

    if (RIIC_MSK_BBSY == ((*piccr2_reg) & RIIC_MSK_BBSY))
    {
        ret = RIIC_ERR_BUS_BUSY;
    }

    return ret;
} /* End of function riic_check_chstatus_start() */

/***********************************************************************************************************************
 * Function Name: riic_check_chstatus_advance
 * Description  : Check Channel Status Processing (at Advance Function).
 *                When calls the advance function, checks channel status.
 * Arguments    : riic_info_t * p_riic_info          ; IIC Information
 * Return Value : RIIC_SUCCESS                   ; Successful operation
 *              : RIIC_ERR_NO_INIT               ; Uninitialized state
 *              : RIIC_ERR_AL                    ; Arbitration lost error
 *              : RIIC_ERR_TMO                   ; Time out error
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
static riic_return_t riic_check_chstatus_advance (riic_info_t * p_riic_info)
{
    riic_return_t ret;

    /* Checks the channel status. */
    switch (g_riic_ChStatus[p_riic_info->ch_no])
    {
        case RIIC_NO_INIT :

            /* It is necessary to reinitialize. */
            /* Sets the return value to uninitialized state. */
            ret = RIIC_ERR_NO_INIT;
        break;

        case RIIC_IDLE :
        case RIIC_FINISH :
        case RIIC_NACK :

            /* Sets the return value to error state. */
            ret = RIIC_ERR_OTHER;
        break;

        case RIIC_COMMUNICATION :

            /* Checks the device status. */
            if ((((RIIC_NO_INIT == p_riic_info->dev_sts) || (RIIC_IDLE == p_riic_info->dev_sts))
                    || (RIIC_FINISH == p_riic_info->dev_sts)) || (RIIC_NACK == p_riic_info->dev_sts))
            {
                /* Sets the return value to idle state. */
                ret = RIIC_ERR_OTHER;
            }
            else if (RIIC_COMMUNICATION == p_riic_info->dev_sts)
            {
                /* The device or another device is on communication. */
                /* Sets the return value to communication state. */
                ret = RIIC_SUCCESS;
            }
            else if (RIIC_TMO == p_riic_info->dev_sts)
            {
                /* The device is in error state. */
                /* Sets the return value to time out error state. */
                ret = RIIC_ERR_TMO;
            }
            else if (RIIC_AL == p_riic_info->dev_sts)
            {
                /* The device is in error state. */
                /* Sets the return value to error state. */
                ret = RIIC_ERR_AL;
            }
            else
            {
                ret = RIIC_ERR_OTHER;
            }
        break;

        case RIIC_TMO :

            /* The channel is in time out error state. */
            /* Sets the return value to time out error state. */
            ret = RIIC_ERR_TMO;
        break;

        case RIIC_AL :

            /* The channel is in error state. */
            /* Sets the return value to error state. */
            ret = RIIC_ERR_AL;
        break;

        default :
            ret = RIIC_ERR_OTHER;
        break;
    }

    return ret;

} /* End of function riic_check_chstatus_advance() */

/***********************************************************************************************************************
 * Function Name: riic_enable
 * Description  : Enables IIC bus interrupt enable register.
 *              : Enables interrupt.
 *              : Enables IIC.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void riic_enable (riic_info_t * p_riic_info)
{
    /* Initializes the IIC registers. */
    /* Includes I/O register read operation at the end of the following function. */
    riic_init_io_register(p_riic_info);

    /* Sets the internal status. */
    riic_api_status_set(p_riic_info, RIIC_STS_IDLE);

    /* Enables the interrupt. */
    riic_int_enable(p_riic_info);

    /* Clears the RIIC reset or internal reset. */
    riic_reset_clear(p_riic_info);
} /* End of function riic_enable() */

/***********************************************************************************************************************
 * Function Name: riic_disable
 * Description  : Disables IIC.
 *              : Disables interrupt.
 *              : Initializes IIC bus interrupt enable register.
 *              : Sets internal status.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void riic_disable (riic_info_t * p_riic_info)
{
    /* Disables IIC. */
    /* Sets SCLn and SDAn to non-driven state. */
    /* Resets all RIIC registers and the internal status. (RIIC reset) */
    riic_all_reset(p_riic_info);

    /* Sets the internal mode. */
    riic_api_mode_event_init(p_riic_info, RIIC_MODE_NONE);

    /* Sets the internal status. */
    riic_api_status_set(p_riic_info, RIIC_STS_NO_INIT);

    /* Disables the interrupt. */
    riic_int_disable(p_riic_info);

    /* Clears the RIIC reset or internal reset. */
    riic_reset_clear(p_riic_info);
} /* End of function riic_disable() */

/***********************************************************************************************************************
 * Outline      : Initialize IIC Driver Processing
 * Function Name: riic_init_io_register
 * Description  : Initializes RIIC register.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void riic_init_io_register (riic_info_t * p_riic_info)
{
    volatile uint8_t uctmp = 0x00;

    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const piccr1_reg = RIIC_ICCR1_ADR(p_riic_info->ch_no);
    volatile uint8_t * const psarl0_reg = RIIC_SARL0_ADR(p_riic_info->ch_no);
    volatile uint8_t * const psaru0_reg = RIIC_SARU0_ADR(p_riic_info->ch_no);
    volatile uint8_t * const psarl1_reg = RIIC_SARL1_ADR(p_riic_info->ch_no);
    volatile uint8_t * const psaru1_reg = RIIC_SARU1_ADR(p_riic_info->ch_no);
    volatile uint8_t * const psarl2_reg = RIIC_SARL2_ADR(p_riic_info->ch_no);
    volatile uint8_t * const psaru2_reg = RIIC_SARU2_ADR(p_riic_info->ch_no);
    volatile uint8_t * const picser_reg = RIIC_ICSER_ADR(p_riic_info->ch_no);
    volatile uint8_t * const picmr2_reg = RIIC_ICMR2_ADR(p_riic_info->ch_no);
    volatile uint8_t * const picmr3_reg = RIIC_ICMR3_ADR(p_riic_info->ch_no);
    volatile uint8_t * const picfer_reg = RIIC_ICFER_ADR(p_riic_info->ch_no);

    /* Sets SCLn and SDAn to non-driven state. */
    /* Resets all RIIC registers and the internal status. (RIIC reset) */
    riic_all_reset(p_riic_info);

    /* Resets ICMR.BC, ICSR1, ICSR2, ICDRS registers and the internal status. (Internal reset) */
    (*piccr1_reg) |= RIIC_ICCR1_ICE_SET; /* Sets ICCR1.ICE bit to 1. */

    /* Sets slave address format and slave address. */
    *psarl0_reg = RIIC_SARL0_INIT; /* Sets SARLy and SARUy.(y=0,1,2) */
    *psaru0_reg = RIIC_SARU0_INIT; /* Sets SARLy and SARUy.(y=0,1,2) */
    *psarl1_reg = RIIC_SARL1_INIT; /* Sets SARLy and SARUy.(y=0,1,2) */
    *psaru1_reg = RIIC_SARU1_INIT; /* Sets SARLy and SARUy.(y=0,1,2) */
    *psarl2_reg = RIIC_SARL2_INIT; /* Sets SARLy and SARUy.(y=0,1,2) */
    *psaru2_reg = RIIC_SARU2_INIT; /* Sets SARLy and SARUy.(y=0,1,2) */
    *picser_reg = RIIC_ICSER_INIT; /* Sets ICSER register. */

    uctmp = *picser_reg; /* Reads ICSER. */

    /* Sets a transfer clock. */
    /* Includes I/O register read operation at the end of the following function. */
    riic_set_frequency(p_riic_info); /* Sets ICMR1.CKS[2:0] bit, ICBRL and ICBRH registers. */

    /* Initializes ICMR2 and ICMR3 registers. */
    *picmr2_reg = g_riic_icmr2_init[p_riic_info->ch_no];
    *picmr3_reg = g_riic_icmr3_init[p_riic_info->ch_no];

    /* Disables IIC interrupt. */
    riic_int_disable(p_riic_info);

#ifdef TN_RXA012A
    riic_timeout_counter_clear(p_riic_info->ch_no);
#endif

    /* Initializes ICFER register. */
    *picfer_reg = g_riic_icfer_init[p_riic_info->ch_no];
    uctmp = *picfer_reg; /* Reads ICFER. */
} /* End of function riic_init_io_register() */

/***********************************************************************************************************************
 * Function Name: riic_slv_addr_match_int_enable
 * Description  : Enable Slave Address Match Interrupt Processing.
 *                Enables slave address match interrupt.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void riic_slv_addr_match_int_enable (riic_info_t * p_riic_info)
{
    volatile uint8_t uctmp = 0x00;

    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const psarl0_reg = RIIC_SARL0_ADR(p_riic_info->ch_no);
    volatile uint8_t * const psaru0_reg = RIIC_SARU0_ADR(p_riic_info->ch_no);
    volatile uint8_t * const psarl1_reg = RIIC_SARL1_ADR(p_riic_info->ch_no);
    volatile uint8_t * const psaru1_reg = RIIC_SARU1_ADR(p_riic_info->ch_no);
    volatile uint8_t * const psarl2_reg = RIIC_SARL2_ADR(p_riic_info->ch_no);
    volatile uint8_t * const psaru2_reg = RIIC_SARU2_ADR(p_riic_info->ch_no);
    volatile uint8_t * const picser_reg = RIIC_ICSER_ADR(p_riic_info->ch_no);

    /* internal reset */
    riic_reset_set(p_riic_info);

    /* Enables slave address match interrupt. */
    if (1U == g_riic_slv_ad0_format[p_riic_info->ch_no])
    {
        /* 7bit address format setting */
        *psarl0_reg = g_riic_slv_ad0_val[p_riic_info->ch_no] << 1;
        *psaru0_reg = 0x00;
        (*picser_reg) |= RIIC_ICSER_SAR0E_SET;
    }
    else if (2U == g_riic_slv_ad0_format[p_riic_info->ch_no])
    {
        /* 10bit address format setting */
        *psarl0_reg = (uint8_t) (g_riic_slv_ad0_val[p_riic_info->ch_no] & RIIC_SARL_MASK);
        *psaru0_reg = (uint8_t) (((g_riic_slv_ad0_val[p_riic_info->ch_no] & RIIC_SARU_MASK) >> 7) |
        RIIC_SARU_10BIT_FORMAT_SET);
        (*picser_reg) |= RIIC_ICSER_SAR0E_SET;
    }
    else
    {
        /* Disable slave address0 */
        (*picser_reg) &= RIIC_ICSER_SAR0E_CLR;
    }

    if (1U == g_riic_slv_ad1_format[p_riic_info->ch_no])
    {
        /* 7bit address format setting */
        *psarl1_reg = (uint8_t) (g_riic_slv_ad1_val[p_riic_info->ch_no] << 1);
        *psaru1_reg = 0x00;
        (*picser_reg) |= RIIC_ICSER_SAR1E_SET;
    }
    else if (2U == g_riic_slv_ad1_format[p_riic_info->ch_no])
    {
        /* 10bit address format setting */
        *psarl1_reg = (uint8_t) (g_riic_slv_ad1_val[p_riic_info->ch_no] & RIIC_SARL_MASK);
        *psaru1_reg = (uint8_t) (((g_riic_slv_ad1_val[p_riic_info->ch_no] & RIIC_SARU_MASK) >> 7) |
        RIIC_SARU_10BIT_FORMAT_SET);
        (*picser_reg) |= RIIC_ICSER_SAR1E_SET;
    }
    else
    {
        /* Disable slave address1 */
        (*picser_reg) &= RIIC_ICSER_SAR1E_CLR;
    }

    if (1U == g_riic_slv_ad2_format[p_riic_info->ch_no])
    {
        /* 7bit address format setting */
        *psarl2_reg = (uint8_t) (g_riic_slv_ad2_val[p_riic_info->ch_no] << 1);
        *psaru2_reg = 0x00;
        (*picser_reg) |= RIIC_ICSER_SAR2E_SET;
    }
    else if (2 == g_riic_slv_ad2_format[p_riic_info->ch_no])
    {
        /* 10bit address format setting */
        *psarl2_reg = (uint8_t) (g_riic_slv_ad2_val[p_riic_info->ch_no] & RIIC_SARL_MASK);
        *psaru2_reg = (uint8_t) (((g_riic_slv_ad2_val[p_riic_info->ch_no] & RIIC_SARU_MASK) >> 7) |
        RIIC_SARU_10BIT_FORMAT_SET);
        (*picser_reg) |= RIIC_ICSER_SAR2E_SET;
    }
    else
    {
        /* Disable slave address2 */
        (*picser_reg) &= RIIC_ICSER_SAR2E_CLR;
    }

    if (1U == g_riic_gca_enable[p_riic_info->ch_no])
    {
        /* General call address setting */
        (*picser_reg) |= RIIC_ICSER_GCAE_SET;
    }
    else
    {
        /* Disable General call address */
        (*picser_reg) &= RIIC_ICSER_GCAE_CLR;
    }
    uctmp = *picser_reg; /* Reads ICSER. */

    /* Clears internal reset. */
    riic_reset_clear(p_riic_info);
} /* End of function riic_slv_addr_match_int_enable() */

/***********************************************************************************************************************
 * Function Name: riic_int_enable
 * Description  : Enable Interrupt Processing.
 *                Clears interrupt request register. Enables interrupt.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void riic_int_enable (riic_info_t * p_riic_info)
{
    /* Clears the interrupt request register. */
    riic_clear_ir_flag(p_riic_info);

    /* Enable interrupts each target MCU.  */
    riic_mcu_int_enable(p_riic_info->ch_no);
} /* End of function riic_int_enable() */

/***********************************************************************************************************************
 * Function Name: riic_int_disable
 * Description  : Disable Interrupt Processing.
 *                Disables interrupt. Sets interrupt source priority. Clears interrupt request register.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void riic_int_disable (riic_info_t * p_riic_info)
{
    /* Disable interrupts each target MCU.  */
    riic_mcu_int_disable(p_riic_info->ch_no);

    /* Clears the interrupt request register. */
    /* Includes I/O register read operation at the end of the following function. */
    riic_clear_ir_flag(p_riic_info);
} /* End of function riic_int_disable() */

/***********************************************************************************************************************
 * Function Name: riic_int_icier_setting
 * Description  : Set ICIER Register Processing.
 *                Sets an interrupt condition of RIIC.
 * Arguments    : riic_info_t *  p_riic_info     ; IIC Information
 : uint8_t        New_icier       ; New ICIER value
 * Return Value : None
 **********************************************************************************************************************/
static void riic_int_icier_setting (riic_info_t * p_riic_info, uint8_t New_icier)
{
    /* Setting TMOIE bit of interrupt enable register. */
    riic_mcu_int_icier_setting(p_riic_info->ch_no, New_icier);

} /* End of function riic_int_icier_setting() */

/***********************************************************************************************************************
 * Function Name: riic_set_frequency
 * Description  : Set IIC Frequency Processing. Sets ICMR1, ICBRL and ICBRH registers.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void riic_set_frequency (riic_info_t * p_riic_info)
{
    riic_bps_calc(p_riic_info, g_riic_bps[p_riic_info->ch_no]); /* Set BPS */
} /* End of function riic_set_frequency() */

/***********************************************************************************************************************
 * Function Name: riic_check_bus_busy
 * Description  : Check Bus Busy Processing.
 *                When the bits of the RIIC registers are the following set values, the bus is released.
 *              :   ICCR2.BBSY = 0 ; The I2C bus is released (bus free state).
 *              :   ICCR1.SCLI = 1 ; SCLn pin input is at a high level.
 *              :   ICCR1.SDAI = 1 ; SDAn pin input is at a high level.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : RIIC_TRUE                      ; Bus ready
 *              : RIIC_FALSE                     ; Bus busy
 **********************************************************************************************************************/
static bool riic_check_bus_busy (riic_info_t * p_riic_info)
{
    volatile uint32_t cnt = 0x00000000;

    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const piccr1_reg = RIIC_ICCR1_ADR(p_riic_info->ch_no);
    volatile uint8_t * const piccr2_reg = RIIC_ICCR2_ADR(p_riic_info->ch_no);

    /* Checks bus busy. */
    for (cnt = RIIC_CFG_BUS_CHECK_COUNTER; cnt > 0x00000000; cnt--)
    {
        /* Bus busy? */
        if ((RIIC_MSK_BBSY != ((*piccr2_reg) & RIIC_MSK_BBSY)) && /* ICCR2.BBSY = 0 */
        (RIIC_MSK_SCLSDA == ((*piccr1_reg) & RIIC_MSK_SCLSDA))) /* ICCR1.SCLI = 1, ICCR1.SDAI = 1 */
        {
            return RIIC_TRUE; /* Bus ready */
        }
    }
    return RIIC_FALSE; /* Bus busy */
} /* End of function riic_check_bus_busy() */

/***********************************************************************************************************************
 * Function Name: riic_start_cond_generate
 * Description  : Generate Start Condition Processing.
 *                Sets ICCR2.ST bit.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void riic_start_cond_generate (riic_info_t * p_riic_info)
{
    volatile uint8_t uctmp = 0x00;

    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const picsr2_reg = RIIC_ICSR2_ADR(p_riic_info->ch_no);
    volatile uint8_t * const piccr2_reg = RIIC_ICCR2_ADR(p_riic_info->ch_no);

    /* Clears start condition detection flag. */
    if (0x00 != (((*picsr2_reg) & RIIC_ICSR2_START_SET) >> 2U))
    {
        (*picsr2_reg) &= RIIC_ICSR2_START_CLR;
    }

    /* Generates a start condition. */
    (*piccr2_reg) |= RIIC_ICCR2_ST; /* Sets ICCR2.ST bit. */
    uctmp = *piccr2_reg; /* Reads ICCR2. */
} /* End of function riic_start_cond_generate() */

/***********************************************************************************************************************
 * Function Name: riic_re_start_cond_generate
 * Description  : Generate Restart Condition Processing.
 *                Sets ICCR2.RS bit.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void riic_re_start_cond_generate (riic_info_t * p_riic_info)
{
    volatile uint8_t uctmp = 0x00;

    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const picsr2_reg = RIIC_ICSR2_ADR(p_riic_info->ch_no);
    volatile uint8_t * const piccr2_reg = RIIC_ICCR2_ADR(p_riic_info->ch_no);

    /* Clears start condition detection flag. */
    if (0x00 != (((*picsr2_reg) & RIIC_ICSR2_START_SET) >> 2U))
    {
        (*picsr2_reg) &= RIIC_ICSR2_START_CLR;
    }

    /* Generates a restart condition. */
    (*piccr2_reg) |= RIIC_ICCR2_RS; /* Sets ICCR2.RS bit. */
    uctmp = *piccr2_reg; /* Reads ICCR2. */
} /* End of function riic_re_start_cond_generate() */

/***********************************************************************************************************************
 * Function Name: riic_stop_cond_generate
 * Description  : Generate Stop Condition Processing.
 *                Clears ICSR2.STOP bit and sets ICCR2.SP bit.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void riic_stop_cond_generate (riic_info_t * p_riic_info)
{
    volatile uint8_t uctmp = 0x00;

    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const picsr2_reg = RIIC_ICSR2_ADR(p_riic_info->ch_no);
    volatile uint8_t * const piccr2_reg = RIIC_ICCR2_ADR(p_riic_info->ch_no);

    /* Clears stop condition detection flag. */
    if (0x00 != (((*picsr2_reg) & RIIC_ICSR2_STOP_SET) >> 3U))
    {
        (*picsr2_reg) &= RIIC_ICSR2_STOP_CLR;
    }

    /* Generates a stop condition. */
    (*piccr2_reg) |= RIIC_ICCR2_SP; /* Sets ICCR2.SP bit. */
    uctmp = *piccr2_reg; /* Reads ICCR2. */
} /* End of function riic_stop_cond_generate() */

/***********************************************************************************************************************
 * Function Name: riic_set_sending_data
 * Description  : Transmit Data Processing.
 *                Sets transmission data to ICDRT register.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 *              : uint8_t *p_data                ; Transmitted data buffer pointer
 * Return Value : None
 **********************************************************************************************************************/
static void riic_set_sending_data (riic_info_t * p_riic_info, uint8_t *p_data)
{
    volatile uint8_t uctmp = 0x00;

    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const picdrt_reg = RIIC_ICDRT_ADR(p_riic_info->ch_no);

    /* Clears TXI interrupt request register. */
    /* For the measure to keep the IR flag for the TXI interrupt from stocking twice,               */
    /* when the IR flag is 1 before writing data to the data buffer in the TXI interrupt handling,  */
    /* the IR flag is cleared before transmission.                                                  */
    /* This issue happens, for example, when the following three all occur.                         */
    /*  - An AL occurred during MasterSend.                                                         */
    /*  - In the communication with an AL occurred, the RX device is specified with SlaveSend       */
    /*    by the communication target Master.                                                       */
    /*  - The TXI interrupt handling is in wait state after receiving the address due to            */
    /*    another interrupt execution, etc.                                                         */
    riic_mcu_clear_ir_txi(p_riic_info->ch_no);

    /* Sets the transmitting data. */
    *picdrt_reg = *p_data; /* Writes data to RIIC in order to transmit. */
    uctmp = *picdrt_reg; /* Reads ICDRT. */
} /* End of function riic_set_sending_data() */

/***********************************************************************************************************************
 * Function Name: riic_get_receiving_data
 * Description  : Receive Data Prpcessing.
 *                Stores received data from ICDRR register.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : Returns received data.
 **********************************************************************************************************************/
static uint8_t riic_get_receiving_data (riic_info_t * p_riic_info)
{
    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const picdrr_reg = RIIC_ICDRR_ADR(p_riic_info->ch_no);

    /* Stores the received data. */
    return *picdrr_reg;
} /* End of function riic_get_receiving_data() */

/***********************************************************************************************************************
 * Function Name: riic_receive_wait_setting
 * Description  : Receive "last byte - 2bytes" Setting Proccesing.
 *                Sets ICMR3.WAIT bit.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void riic_receive_wait_setting (riic_info_t * p_riic_info)
{
    volatile uint8_t uctmp = 0x00;

    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const picmr3_reg = RIIC_ICMR3_ADR(p_riic_info->ch_no);

    /* Sets ICMR3.WAIT bit. */
    (*picmr3_reg) |= RIIC_ICMR3_WAIT_SET;
    uctmp = *picmr3_reg; /* Reads ICMR3. */
} /* End of function riic_receive_wait_setting() */

/***********************************************************************************************************************
 * Function Name: riic_receive_pre_end_setting
 * Description  : Receive "last byte - 1byte" Setting Proccesing.
 *                Sets ICMR3.RDRFS bit and ACKBT bit.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void riic_receive_pre_end_setting (riic_info_t * p_riic_info)
{
    volatile uint8_t uctmp = 0x00;

    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const picmr3_reg = RIIC_ICMR3_ADR(p_riic_info->ch_no);

    /* Sets ICMR3.RDRFS bit.*/
    (*picmr3_reg) |= RIIC_ICMR3_RDRFS_SET;

    /* Sets ICMR3.ACKBT bit. */
    (*picmr3_reg) |= RIIC_ICMR3_ACKWP_SET; /* Modification of the ACKBT bit is enabled.                */
    (*picmr3_reg) |= RIIC_ICMR3_ACKBT_SET; /* A 1 is sent as the acknowledge bit (NACK transmission).  */
    (*picmr3_reg) &= RIIC_ICMR3_ACKWP_CLR; /* Modification of the ACKBT bit is disabled.               */
    uctmp = *picmr3_reg; /* Reads ICMR3.                                             */
} /* End of function riic_receive_pre_end_setting() */

/***********************************************************************************************************************
 * Function Name: riic_receive_end_setting
 * Description  : Receive End Setting Processing.
 *                Sets ICMR3.ACKBT bit and clears ICMR3.WAIT bit.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void riic_receive_end_setting (riic_info_t * p_riic_info)
{
    volatile uint8_t uctmp = 0x00;

    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const picmr3_reg = RIIC_ICMR3_ADR(p_riic_info->ch_no);

    /* Sets ICMR3.ACKBT bit. */
    (*picmr3_reg) |= RIIC_ICMR3_ACKWP_SET; /* Modification of the ACKBT bit is enabled. */
    (*picmr3_reg) |= RIIC_ICMR3_ACKBT_SET;
    (*picmr3_reg) &= RIIC_ICMR3_ACKWP_CLR; /* Modification of the ACKBT bit is disabled. */

    /* Clears ICMR3.WAIT bit. */
    (*picmr3_reg) &= RIIC_ICMR3_WAIT_CLR;
    uctmp = *picmr3_reg; /* Reads ICMR3. */
} /* End of function riic_receive_end_setting() */

/***********************************************************************************************************************
 * Function Name: riic_reset_clear
 * Description  : Enable IIC Function Processing.
 *                Enables RIIC communication and enables RIIC multi-function pin controller.
 * Arguments    : riic_info_t * p_riic_info     ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void riic_reset_clear (riic_info_t * p_riic_info)
{
    volatile uint8_t uctmp = 0x00;

    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const piccr1_reg = RIIC_ICCR1_ADR(p_riic_info->ch_no);

    /* Enables RIIC. */
    (*piccr1_reg) &= RIIC_ICCR1_ENABLE; /* Clears ICCR1.IICRST bit. */
    uctmp = *piccr1_reg; /* Reads ICCR1. */
} /* End of function riic_reset_clear() */

/***********************************************************************************************************************
 * Function Name: riic_reset_set
 * Description  : Disable IIC Function Processing.
 *                Disables RIIC communication and disables RIIC multi-function pin controller.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void riic_reset_set (riic_info_t * p_riic_info)
{
    volatile uint8_t uctmp = 0x00;

    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const piccr1_reg = RIIC_ICCR1_ADR(p_riic_info->ch_no);

    /* Resets RIIC registers. */
    (*piccr1_reg) |= RIIC_ICCR1_RIIC_RESET; /* Sets ICCR1.IICRST bit. */
    uctmp = *piccr1_reg; /* Reads ICCR1. */
} /* End of function riic_reset_set() */

/***********************************************************************************************************************
 * Function Name: riic_all_reset
 * Description  : All reset IIC Function Processing.
 *                Resets all registers and internal states of the RIIC.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void riic_all_reset (riic_info_t * p_riic_info)
{
    volatile uint8_t uctmp = 0x00;

    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const piccr1_reg = RIIC_ICCR1_ADR(p_riic_info->ch_no);

    /* Resets RIIC registers. */
    (*piccr1_reg) &= RIIC_ICCR1_ICE_CLR; /* Clears ICCR1.ICE bit to 0. */
    (*piccr1_reg) |= RIIC_ICCR1_RIIC_RESET; /* Sets ICCR1.IICRST bit to 1. */

    uctmp = *piccr1_reg; /* Reads ICCR1. */
} /* End of function riic_all_reset() */

/***********************************************************************************************************************
 * Function Name: riic_clear_ir_flag
 * Description  : Clears Interrupt Request Flag Processing.
 *                Clears interrupt request register.
 * Arguments    : riic_info_t * p_riic_info      ; IIC Information
 * Return Value : None
 **********************************************************************************************************************/
static void riic_clear_ir_flag (riic_info_t * p_riic_info)
{
    uint8_t internal_flag = 0x00; /* Determines whether reinitialization is necessary. */
    volatile uint8_t uctmp = 0x00;

    /* Creates the register pointer for the specified RIIC channel. */
    volatile uint8_t * const piccr1_reg = RIIC_ICCR1_ADR(p_riic_info->ch_no);
    volatile uint8_t * const picier_reg = RIIC_ICIER_ADR(p_riic_info->ch_no);

    /* Checks IR flag. */
    /* If IR flag is set, clears IR flag. */
    if ((RIIC_IR_SET == riic_mcu_check_ir_txi(p_riic_info->ch_no))
            || (RIIC_IR_SET == riic_mcu_check_ir_rxi(p_riic_info->ch_no)))
    {
        /* Checks ICCR1.ICE bit. */
        if (RIIC_ICCR1_ICE_SET == ((*piccr1_reg) & RIIC_ICCR1_ICE)) /* ICCR1.ICE = 1 */
        {
            /* Sets return value. */
            internal_flag = 0x01;

            /* Clears ICCR1.ICE bit and sets ICCR1.IICRST bit. */
            (*piccr1_reg) |= RIIC_ICCR1_RIIC_RESET;
            while (RIIC_ICCR1_RIIC_RESET != ((*piccr1_reg) & RIIC_ICCR1_RIIC_RESET))
            {
                /* Do Nothing */
            }
            (*piccr1_reg) &= RIIC_ICCR1_ICE_CLR;
            while (RIIC_ICCR1_NOT_DRIVEN != ((*piccr1_reg) & RIIC_ICCR1_ICE))
            {
                /* Do Nothing */
            }
        }

        /* Initializes ICIER register. */
        *picier_reg = RIIC_ICIER_INIT;
        while (RIIC_ICIER_INIT != (*picier_reg))
        {
            /* Do Nothing */
        }

        riic_mcu_clear_ir_rxi(p_riic_info->ch_no); /* Clears RXI interrupt request register. */
        riic_mcu_clear_ir_txi(p_riic_info->ch_no); /* Clears TXI interrupt request register. */

        /* Checks the need of the reinitialization. */
        if (0x01 == internal_flag)
        {
            internal_flag = 0x00;

            /* Reinitializes RIIC register because cleared ICCR1.ICE bit. */
            /* Includes I/O register read operation at the end of the following function. */
            riic_init_io_register(p_riic_info);
        }
    }
} /* End of function riic_clear_ir_flag() */

/***********************************************************************************************************************
 * Function Name: riic_bps_calc
 * Description  : Calculation for ICBRH and ICBRL registers value
 *              :  and CKS bits value from setting bps of the argument.
 *              :
 * Arguments    : riic_info_t * p_riic_info      ; IIC Informationu
 *              : uint16_t kbps                  ; bit rate(kHz)
 * Return Value : RIIC_SUCCESS                   ; Successful operation, communication state
 *              : RIIC_ERR_OTHER                 ; Other error
 **********************************************************************************************************************/
static riic_return_t riic_bps_calc (riic_info_t * p_riic_info, uint16_t kbps)
{
    volatile uint8_t uctmp;
    volatile uint8_t * const picmr1_reg = RIIC_ICMR1_ADR(p_riic_info->ch_no);
    volatile uint8_t * const picbrl_reg = RIIC_ICBRL_ADR(p_riic_info->ch_no);
    volatile uint8_t * const picbrh_reg = RIIC_ICBRH_ADR(p_riic_info->ch_no);

    const double scl100k_up_time = 1000E-9;
    const double scl100k_down_time = 300E-9;
    const double scl400k_up_time = 300E-9;
    const double scl400k_down_time = 300E-9;
    const double scl1m_up_time = 120E-9;
    const double scl1m_down_time = 120E-9;

    const uint8_t d_cks[RIIC_MAX_DIV] =
    { 1, 2, 4, 8, 16, 32, 64, 128 }; /* divider array of RIIC clock  */
    double pclk_val;

    volatile double bps = (double) (kbps * 1000);
    volatile double L_time; /* H Width period */
    volatile double H_time; /* L Width period */

    volatile double calc_val; /* Using for calculation buffer */

    double scl_up_time;
    double scl_down_time;
    volatile uint8_t cnt;

    pclk_val = riic_mcu_check_freq(); /* Store pclk frequency */

    /* Set Rise up time and down time */
    if (kbps > RIIC_FAST_SPPED_MAX)
    {
        /* When bps more than 400Kbps[Fast mode plus] */
        scl_up_time = scl1m_up_time;
        scl_down_time = scl1m_down_time;
    }
    else if (kbps > RIIC_STAD_SPPED_MAX)
    {
        /* When bps more than 100Kbps[Fast mode] */
        scl_up_time = scl400k_up_time;
        scl_down_time = scl400k_down_time;
    }
    else
    {
        /* When bps less than 100Kbps[Standard mode] */
        scl_up_time = scl100k_up_time;
        scl_down_time = scl100k_down_time;
    }

    /* Calculation for ICBRH and ICBRL registers value */

    /* Calculation L width time */
    L_time = (1 / (2 * bps)); /* Harf period of frequency */
    H_time = L_time;

    /* Check I2C mode of Speed */
    if (kbps > RIIC_FAST_SPPED_MAX)
    {
        /* 400kbps over */
        /* Check L width */
        if (L_time < 0.5E-6)
        {
            /* Wnen L width less than 0.5us */
            /* Subtract Rise up and down time for SCL from H/L width */
            L_time = 0.5E-6;
            H_time = (((1 / bps) - L_time) - scl_up_time) - scl_down_time;
        }
        else
        {
            /* Subtract Rise up and down time for SCL from H/L width */
            L_time -= scl_down_time;
            H_time -= scl_up_time;
        }
    }
    else if (kbps > RIIC_STAD_SPPED_MAX)
    {
        /* 100kbps over */
        /* Check L width */
        if (L_time < 1.3E-6)
        {
            /* Wnen L width less than 1.3us */
            /* Subtract Rise up and down time for SCL from H/L width */
            L_time = 1.3E-6;
            H_time = (((1 / bps) - L_time) - scl_up_time) - scl_down_time;
        }
        else
        {
            /* Subtract Rise up and down time for SCL from H/L width */
            L_time -= scl_down_time;
            H_time -= scl_up_time;
        }
    }
    else
    {
        /* less than 100kbps */
/* Since the L width is calculated based on the half period of frequency above, it is always larger than 4.7 us
   at 100 kbps or less. Comment out the following branch processing.
         * Check L width *
        if (L_time < 4.7E-6)
        {
            * Wnen L width less than 4.7us *
            * Subtract Rise up and down time for SCL from H/L width *
            L_time = 4.7E-6;
            H_time = (((1 / bps) - L_time) - scl_up_time) - scl_down_time;
        }
        else
        {
 */
        /* Subtract Rise up and down time for SCL from H/L width */
        L_time -= scl_down_time;
        H_time -= scl_up_time;

/*      }*/
    }

    /*************** Calculation ICBRL value ***********************/
    /* Calculating until calc_val is less than 32 */
    for (calc_val = 0xFF, cnt = 0; RIIC_ICBR_MAX < calc_val; cnt++)
    {
        calc_val = L_time; /* Set L width time */

        /* Check the range of divider of CKS */
        if (cnt >= RIIC_MAX_DIV)
        {
            /* Cannot set bps */
            return RIIC_ERR_OTHER;
        }

        calc_val = (calc_val / (d_cks[cnt] / pclk_val));/* Calculattion ICBRL value */
        calc_val = calc_val + 0.5; /* round off */
    }

     /* store ICMR1 value to avoid CKS bit. */
    uctmp = (uint8_t) ((uint8_t) (*picmr1_reg) & (((~BIT4) & (~BIT5)) & (~BIT6)));

    *picmr1_reg = (uint8_t) ((uctmp) | ((cnt - 1) << 4)); /* Set ICMR1.CKS bits.*/
    *picbrl_reg = (uint8_t) ((((uint8_t) (calc_val - 1) | BIT7) | BIT6) | BIT5); /* Set value to ICBRL register */

    /*************** Calculation ICBRH value ***********************/
    calc_val = H_time; /* Set H width */
    calc_val = (calc_val / (d_cks[cnt - 1] / pclk_val)); /* Calculattion ICBRH value */
    calc_val = (uint8_t) (calc_val + 0.5); /* round off */

    /* If the calculated value is less than 1, it rounded up to 1. */
    if (1 > calc_val)
    {
        calc_val = 1;
    }

    *picbrh_reg = (uint8_t) ((((uint8_t) (calc_val - 1) | BIT7) | BIT6) | BIT5); /* Set value to ICBRH register */

    uctmp = *picbrh_reg; /* dummy read */

    return RIIC_SUCCESS;

} /* End of function riic_bps_calc() */

#if (RIIC_CFG_CH0_INCLUDED == 1U)
/***********************************************************************************************************************
 * Function Name: riic0_eei_sub
 * Description  : Interrupt EEI handler for channel 0.
 *                Types of interrupt requests transfer error or event generation.
 *                The event generations are arbitration-lost, NACK detection, timeout detection, 
 *                start condition detection, and stop condition detection.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void riic0_eei_sub (void)
{
    riic_info_t * p_riic_info;

    #ifdef TN_RXA012A
    riic_timeout_counter_clear(0);
    #endif

    /* Checks Time Out. */
    if ((0U != RIIC0.ICSR2.BIT.TMOF) && (0U != RIIC0.ICIER.BIT.TMOIE))
    {
        /* all interrupt disable */
        RIIC0.ICIER.BIT.TMOIE = 0U;
        while (0U != RIIC0.ICIER.BIT.TMOIE)
        {
            /* Do Nothing */
        }
        riic_api_event[0] = RIIC_EV_INT_TMO;
    }

    /* Checks Arbitration Lost. */
    if ((0U != RIIC0.ICSR2.BIT.AL) && (0U != RIIC0.ICIER.BIT.ALIE))
    {
        RIIC0.ICIER.BIT.ALIE = 0U;
        while (0U != RIIC0.ICIER.BIT.ALIE)
        {
            /* Do Nothing */
        }
        riic_api_event[0] = RIIC_EV_INT_AL;
        if (RIIC_MODE_S_READY == riic_api_info[0].B_Mode)
        {
            riic_set_ch_status(priic_info_s[0], RIIC_COMMUNICATION);
        }
    }

    /* Checks stop condition detection. */
    if ((0U != RIIC0.ICSR2.BIT.STOP) && (0U != RIIC0.ICIER.BIT.SPIE))
    {
        if (((RIIC_MODE_S_READY == riic_api_info[0].N_Mode) || (RIIC_MODE_S_SEND == riic_api_info[0].N_Mode))
                || (RIIC_MODE_S_RECEIVE == riic_api_info[0].N_Mode))
        {
            /* Sets the internal status. */
            riic_api_status_set(priic_info_s[0], RIIC_STS_SP_COND_WAIT);
        }

        /* Prohibits STOP interrupt. */
        RIIC0.ICIER.BIT.SPIE = 0U;

        RIIC0.ICMR3.BIT.RDRFS = 0U; /* Refer to the technical update. */
        RIIC0.ICMR3.BIT.ACKWP = 1U; /* Refer to the technical update. */
        RIIC0.ICMR3.BIT.ACKBT = 0U; /* Refer to the technical update. */
        RIIC0.ICMR3.BIT.ACKWP = 0U; /* Refer to the technical update. */
        while ((0U != RIIC0.ICMR3.BIT.RDRFS) || (0U != RIIC0.ICMR3.BIT.ACKBT))
        {
            /* Do Nothing */
        }

        /* Clears each status flag. */
        RIIC0.ICSR2.BIT.STOP = 0U;
        while (0U != RIIC0.ICSR2.BIT.STOP)
        {
            /* Do Nothing */
        }

        /* Sets event flag. */
        riic_api_event[0] = RIIC_EV_INT_STOP;
    }

    /* Checks NACK reception. */
    if ((0U != RIIC0.ICSR2.BIT.NACKF) && (0U != RIIC0.ICIER.BIT.NAKIE))
    {
        /* Prohibits NACK interrupt to generate stop condition. */
        RIIC0.ICIER.BIT.NAKIE = 0U;

        /* Prohibits these interrupt. */
        /* After NACK interrupt, these interrupts will occur when they do not stop the following interrupts. */
        RIIC0.ICIER.BIT.TEIE = 0U;
        RIIC0.ICIER.BIT.TIE = 0U;
        RIIC0.ICIER.BIT.RIE = 0U;
        while (((0U != RIIC0.ICIER.BIT.TEIE) || (0U != RIIC0.ICIER.BIT.TIE)) || (0U != RIIC0.ICIER.BIT.RIE))
        {
            /* Do Nothing */
        }

        if (0U == RIIC0.ICCR2.BIT.TRS)
        {
            riic_mcu_clear_ir_rxi(0);
        }

        /* Sets event flag. */
        riic_api_event[0] = RIIC_EV_INT_NACK;
    }

    /* Checks start condition detection. */
    if ((0U != RIIC0.ICSR2.BIT.START) && (0U != RIIC0.ICIER.BIT.STIE))
    {
        /* Disable start condition detection interrupt. */
        /* Clears status flag. */
        RIIC0.ICIER.BIT.STIE = 0U;
        RIIC0.ICSR2.BIT.START = 0U;
        while ((0U != RIIC0.ICSR2.BIT.START) || (0U != RIIC0.ICIER.BIT.STIE))
        {
            /* Do Nothing */
        }

        /* Sets event flag. */
        riic_api_event[0] = RIIC_EV_INT_START;
    }

    switch (riic_api_info[0].N_Mode)
    {
        case RIIC_MODE_M_SEND :
        case RIIC_MODE_M_RECEIVE :
        case RIIC_MODE_M_SEND_RECEIVE :

            /* Master mode data */
            p_riic_info = priic_info_m[0];
        break;

        case RIIC_MODE_S_READY :
        case RIIC_MODE_S_SEND :
        case RIIC_MODE_S_RECEIVE :

            /* Slave mode data */
            p_riic_info = priic_info_s[0];
        break;

        default :

            /* Internal error */
            return;
        break;
    }

    r_riic_advance(p_riic_info); /* Calls advance function */
} /* End of function riic0_eei_sub() */

/***********************************************************************************************************************
 * Function Name: riic0_txi_sub
 * Description  : Interrupt TXI handler for channel 0.
 *                Types of interrupt requests transmission data empty.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void riic0_txi_sub (void)
{
    uint8_t tmp;

    #ifdef TN_RXA012A
    riic_timeout_counter_clear(0);
    #endif

    /* TXI processing is performed only when the following conditions are all met.                 */
    /*  - The address part of data has been received and the received address matches the RX's     */
    /*    own address.                                                                             */
    /*  - The RX device is in slave mode.                                                          */
    /*  - The RX device is in transmission mode (the R/W bit received is 'R' (slave transmission)) */
    tmp = RIIC0.ICCR2.BYTE;
    if ((RIIC_ICCR2_MST_SET != (tmp & RIIC_ICCR2_MST)) && (RIIC_ICCR2_TRS_SET == (tmp & RIIC_ICCR2_TRS)))
    {
        /* Processing when the addresses match (slave send specified) */

        /* Determines whether the state is arbitration-lost detected.                           */
        /* When an arbitration-lost is detected, the riic_after_receive_slvadr function is not  */
        /* executed. Thus the internal status needs to be changed to "RIIC_STS_SEND_DATA_WAIT". */
        if (RIIC_STS_AL == riic_api_info[0].N_status)
        {
            /* Processing when an arbitration-lost is detected. */
            /* Sets the internal status. */
            riic_api_status_set(priic_info_s[0], RIIC_STS_SEND_DATA_WAIT);
        }

        /* Determines if the slave transmission is the first slave transmission.             */
        /* N_mode is updated from "RIIC_MODE_S_READY" to "RIIC_MODE_S_SEND" on the first TXI */
        /* interrupt after the slave transmission is determined.                             */
        if (RIIC_MODE_S_READY == riic_api_info[0].N_Mode)
        {
            /* Processing for the first slave transmission */
            /* Sets the internal mode. */
            riic_api_info[0].N_Mode = RIIC_MODE_S_SEND; /* Set slave transmission mode. */
        }

        /* Sets event. */
        switch (riic_api_info[0].N_status)
        {
            case RIIC_STS_IDLE_EN_SLV :

                /* Updates the channel status. */
                riic_set_ch_status(priic_info_s[0], RIIC_COMMUNICATION);

                /* Sets interrupted address sending. */
                riic_api_event[0] = RIIC_EV_INT_ADD;

            break;

            case RIIC_STS_SEND_DATA_WAIT :

                /* Sets interrupted data sending. */
                riic_api_event[0] = RIIC_EV_INT_SEND;

            break;

            default :

                /* Does nothing. */
            break;

        }

        r_riic_advance(priic_info_s[0]); /* Calls advance function */

    }
} /* End of function riic0_txi_sub() */

/***********************************************************************************************************************
 * Function Name: riic0_rxi_sub
 * Description  : Interrupt RXI handler for channel 0.
 *                Types of interrupt requests reception end.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void riic0_rxi_sub (void)
{
    riic_info_t * p_riic_info;

    #ifdef TN_RXA012A
    riic_timeout_counter_clear(0);
    #endif

    if (RIIC_STS_AL == riic_api_info[0].N_status)
    {
        /* Sets the internal status. */
        riic_api_status_set(priic_info_s[0], RIIC_STS_IDLE_EN_SLV);
    }

    if (RIIC_MODE_S_READY == riic_api_info[0].N_Mode)
    {
        /* Sets the internal mode. */
        riic_api_info[0].N_Mode = RIIC_MODE_S_RECEIVE;
    }

    if ((RIIC_STS_IDLE_EN_SLV == riic_api_info[0].N_status))
    {
        /* Updates the channel status. */
        riic_set_ch_status(priic_info_s[0], RIIC_COMMUNICATION);
    }

    switch (riic_api_info[0].N_Mode)
    {
        case RIIC_MODE_M_RECEIVE :
        case RIIC_MODE_M_SEND_RECEIVE :

            /* Master mode data */
            p_riic_info = priic_info_m[0];
        break;

        case RIIC_MODE_S_RECEIVE :

            /* Slave mode data */
            p_riic_info = priic_info_s[0];
        break;

        default :

            /* Internal error */
            return;
        break;
    }

    /* Sets interrupted data receiving. */
    riic_api_event[0] = RIIC_EV_INT_RECEIVE;

    r_riic_advance(p_riic_info); /* Calls advance function */
} /* End of function riic0_rxi_sub() */

/***********************************************************************************************************************
 * Function Name: riic0_tei_sub
 * Description  : Interrupt TEI handler for channel 0.
 *                Types of interrupt requests transmission end.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void riic0_tei_sub (void)
{
    #ifdef TN_RXA012A
    riic_timeout_counter_clear(0);
    #endif

    /* Clears ICSR2.TEND. */
    RIIC0.ICSR2.BIT.TEND = 0U;
    while (0U != RIIC0.ICSR2.BIT.TEND)
    {
        /* Do Nothing */
    }

    /* Sets event. */
    switch (riic_api_info[0].N_status)
    {
        case RIIC_STS_SEND_SLVADR_W_WAIT :
        case RIIC_STS_SEND_SLVADR_R_WAIT :

            /* Sets interrupted address sending. */
            riic_api_event[0] = RIIC_EV_INT_ADD;

        break;

        case RIIC_STS_SEND_DATA_WAIT :

            /* Sets interrupted data sending. */
            riic_api_event[0] = RIIC_EV_INT_SEND;

        break;

        default :

            /* Does nothing. */
        break;

    }

    r_riic_advance(priic_info_m[0]); /* Calls advance function */
} /* End of function riic0_tei_sub() */
#endif

#if (RIIC_CFG_CH1_INCLUDED == 1U)
/***********************************************************************************************************************
 * Function Name: riic1_eei_sub
 * Description  : Interrupt EEI handler for channel 1.
 *                Types of interrupt requests transfer error or event generation.
 *                The event generations are arbitration-lost, NACK detection, timeout detection, 
 *                start condition detection, and stop condition detection.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void riic1_eei_sub (void)
{
    riic_info_t * p_riic_info;

    #ifdef TN_RXA012A
    riic_timeout_counter_clear(1);
    #endif

    /* Checks Time Out. */
    if ((0U != RIIC1.ICSR2.BIT.TMOF) && (0U != RIIC1.ICIER.BIT.TMOIE))
    {
        /* all interrupt disable */
        RIIC1.ICIER.BIT.TMOIE = 0U;
        while (0U != RIIC1.ICIER.BIT.TMOIE)
        {
            /* Do Nothing */
        }
        riic_api_event[1] = RIIC_EV_INT_TMO;
    }

    /* Checks Arbitration Lost. */
    if ((0U != RIIC1.ICSR2.BIT.AL) && (0U != RIIC1.ICIER.BIT.ALIE))
    {
        RIIC1.ICIER.BIT.ALIE = 0U;
        while (0U != RIIC1.ICIER.BIT.ALIE)
        {
            /* Do Nothing */
        }
        riic_api_event[1] = RIIC_EV_INT_AL;
        if (RIIC_MODE_S_READY == riic_api_info[1].B_Mode)
        {
            riic_set_ch_status(priic_info_s[1], RIIC_COMMUNICATION);
        }
    }

    /* Checks stop condition detection. */
    if ((0U != RIIC1.ICSR2.BIT.STOP) && (0U != RIIC1.ICIER.BIT.SPIE))
    {
        if (((RIIC_MODE_S_READY == riic_api_info[1].N_Mode) || (RIIC_MODE_S_SEND == riic_api_info[1].N_Mode))
                || (RIIC_MODE_S_RECEIVE == riic_api_info[1].N_Mode))
        {
            /* Sets the internal status. */
            riic_api_status_set(priic_info_s[1], RIIC_STS_SP_COND_WAIT);
        }

        /* Prohibits STOP interrupt. */
        RIIC1.ICIER.BIT.SPIE = 0U;

        RIIC1.ICMR3.BIT.RDRFS = 0U; /* Refer to the technical update. */
        RIIC1.ICMR3.BIT.ACKWP = 1U; /* Refer to the technical update. */
        RIIC1.ICMR3.BIT.ACKBT = 0U; /* Refer to the technical update. */
        RIIC1.ICMR3.BIT.ACKWP = 0U; /* Refer to the technical update. */
        while ((0U != RIIC1.ICMR3.BIT.RDRFS) || (0U != RIIC1.ICMR3.BIT.ACKBT))
        {
            /* Do Nothing */
        }

        /* Clears each status flag. */
        RIIC1.ICSR2.BIT.STOP = 0U;
        while (0U != RIIC1.ICSR2.BIT.STOP)
        {
            /* Do Nothing */
        }

        /* Sets event flag. */
        riic_api_event[1] = RIIC_EV_INT_STOP;
    }

    /* Checks NACK reception. */
    if ((0U != RIIC1.ICSR2.BIT.NACKF) && (0U != RIIC1.ICIER.BIT.NAKIE))
    {
        /* Prohibits NACK interrupt to generate stop condition. */
        RIIC1.ICIER.BIT.NAKIE = 0U;

        /* Prohibits these interrupt. */
        /* After NACK interrupt, these interrupts will occur when they do not stop the following interrupts. */
        RIIC1.ICIER.BIT.TEIE = 0U;
        RIIC1.ICIER.BIT.TIE = 0U;
        RIIC1.ICIER.BIT.RIE = 0U;
        while (((0U != RIIC1.ICIER.BIT.TEIE) || (0U != RIIC1.ICIER.BIT.TIE)) || (0U != RIIC1.ICIER.BIT.RIE) )
        {
            /* Do Nothing */
        }

        if (0U == RIIC1.ICCR2.BIT.TRS)
        {
            riic_mcu_clear_ir_rxi(1);
        }

        /* Sets event flag. */
        riic_api_event[1] = RIIC_EV_INT_NACK;
    }

    /* Checks start condition detection. */
    if ((0U != RIIC1.ICSR2.BIT.START) && (0U != RIIC1.ICIER.BIT.STIE))
    {
        /* Disable start condition detection interrupt. */
        /* Clears status flag. */
        RIIC1.ICIER.BIT.STIE = 0U;
        RIIC1.ICSR2.BIT.START = 0U;
        while ((0U != RIIC1.ICSR2.BIT.START) || (0U != RIIC1.ICIER.BIT.STIE))
        {
            /* Do Nothing */
        }

        /* Sets event flag. */
        riic_api_event[1] = RIIC_EV_INT_START;
    }

    switch (riic_api_info[1].N_Mode)
    {
        case RIIC_MODE_M_SEND :
        case RIIC_MODE_M_RECEIVE :
        case RIIC_MODE_M_SEND_RECEIVE :

            /* Master mode data */
            p_riic_info = priic_info_m[1];
        break;

        case RIIC_MODE_S_READY :
        case RIIC_MODE_S_SEND :
        case RIIC_MODE_S_RECEIVE :

            /* Slave mode data */
            p_riic_info = priic_info_s[1];
        break;

        default :

            /* Internal error */
            return;
        break;
    }

    r_riic_advance(p_riic_info); /* Calls advance function */
} /* End of function riic1_eei_sub() */

/***********************************************************************************************************************
 * Function Name: riic1_txi_sub
 * Description  : Interrupt TXI handler for channel 1.
 *                Types of interrupt requests transmission data empty. Does not use.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void riic1_txi_sub (void)
{
    uint8_t tmp;

    #ifdef TN_RXA012A
    riic_timeout_counter_clear(1);
    #endif

    /* TXI processing is performed only when the following conditions are all met.                 */
    /*  - The address part of data has been received and the received address matches the RX's     */
    /*    own address.                                                                             */
    /*  - The RX device is in slave mode.                                                          */
    /*  - The RX device is in transmission mode (the R/W bit received is 'R' (slave transmission)) */
    tmp = RIIC1.ICCR2.BYTE;
    if ((RIIC_ICCR2_MST_SET != (tmp & RIIC_ICCR2_MST)) && (RIIC_ICCR2_TRS_SET == (tmp & RIIC_ICCR2_TRS)))
    {
        /* Processing when the addresses match (slave send specified) */

        /* Determines whether the state is arbitration-lost detected.                           */
        /* When an arbitration-lost is detected, the riic_after_receive_slvadr function is not  */
        /* executed. Thus the internal status needs to be changed to "RIIC_STS_SEND_DATA_WAIT". */
        if (RIIC_STS_AL == riic_api_info[1].N_status)
        {
            /* Processing when an arbitration-lost is detected. */
            /* Sets the internal status. */
            riic_api_status_set(priic_info_s[1], RIIC_STS_SEND_DATA_WAIT);
        }

        /* Determines if the slave transmission is the first slave transmission.             */
        /* N_mode is updated from "RIIC_MODE_S_READY" to "RIIC_MODE_S_SEND" on the first TXI */
        /* interrupt after the slave transmission is determined.                             */
        if (RIIC_MODE_S_READY == riic_api_info[1].N_Mode)
        {
            /* Processing for the first slave transmission */
            /* Sets the internal mode. */
            riic_api_info[1].N_Mode = RIIC_MODE_S_SEND; /* Set slave transmission mode. */
        }

        /* Sets event. */
        switch (riic_api_info[1].N_status)
        {
            case RIIC_STS_IDLE_EN_SLV :

                /* Updates the channel status. */
                riic_set_ch_status(priic_info_s[1], RIIC_COMMUNICATION);

                /* Sets interrupted address sending. */
                riic_api_event[1] = RIIC_EV_INT_ADD;

            break;

            case RIIC_STS_SEND_DATA_WAIT :

                /* Sets interrupted data sending. */
                riic_api_event[1] = RIIC_EV_INT_SEND;

            break;

            default :

                /* Does nothing. */
            break;

        }

        r_riic_advance(priic_info_s[1]); /* Calls advance function */

    }
} /* End of function riic1_txi_sub() */

/***********************************************************************************************************************
 * Outline      : 
 * Function Name: riic1_rxi_sub
 * Description  : Interrupt RXI handler for channel 1.
 *                Types of interrupt requests reception end.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void riic1_rxi_sub (void)
{
    riic_info_t * p_riic_info;

    #ifdef TN_RXA012A
    riic_timeout_counter_clear(1);
    #endif

    if (RIIC_STS_AL == riic_api_info[1].N_status)
    {
        /* Sets the internal status. */
        riic_api_status_set(priic_info_s[1], RIIC_STS_IDLE_EN_SLV);
    }

    if (RIIC_MODE_S_READY == riic_api_info[1].N_Mode)
    {
        /* Sets the internal mode. */
        riic_api_info[1].N_Mode = RIIC_MODE_S_RECEIVE;
    }

    if ((RIIC_STS_IDLE_EN_SLV == riic_api_info[1].N_status))
    {
        /* Updates the channel status. */
        riic_set_ch_status(priic_info_s[1], RIIC_COMMUNICATION);
    }

    switch (riic_api_info[1].N_Mode)
    {
        case RIIC_MODE_M_RECEIVE :
        case RIIC_MODE_M_SEND_RECEIVE :

            /* Master mode data */
            p_riic_info = priic_info_m[1];
        break;

        case RIIC_MODE_S_RECEIVE :

            /* Slave mode data */
            p_riic_info = priic_info_s[1];
        break;

        default :

            /* Internal error */
            return;
        break;
    }

    /* Sets interrupted data receiving. */
    riic_api_event[1] = RIIC_EV_INT_RECEIVE;

    r_riic_advance(p_riic_info); /* Calls advance function */
} /* End of function riic1_rxi_sub() */

/***********************************************************************************************************************
 * Outline      : 
 * Function Name: riic1_tei_sub
 * Description  : Interrupt TEI handler for channel 1.
 *                Types of interrupt requests transmission end.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void riic1_tei_sub (void)
{
    #ifdef TN_RXA012A
    riic_timeout_counter_clear(1);
    #endif

    /* Clears ICSR2.TEND. */
    RIIC1.ICSR2.BIT.TEND = 0U;
    while (0U != RIIC1.ICSR2.BIT.TEND)
    {
        /* Do Nothing */
    }

    /* Sets event. */
    switch (riic_api_info[1].N_status)
    {
        case RIIC_STS_SEND_SLVADR_W_WAIT :
        case RIIC_STS_SEND_SLVADR_R_WAIT :

            /* Sets interrupted address sending. */
            riic_api_event[1] = RIIC_EV_INT_ADD;

        break;

        case RIIC_STS_SEND_DATA_WAIT :

            /* Sets interrupted data sending. */
            riic_api_event[1] = RIIC_EV_INT_SEND;

        break;

        default :

            /* Does nothing. */
        break;

    }

    r_riic_advance(priic_info_m[1]); /* Calls advance function */
} /* End of function riic1_tei_sub() */
#endif

#if (RIIC_CFG_CH2_INCLUDED == 1U)
/***********************************************************************************************************************
 * Function Name: riic2_eei_sub
 * Description  : Interrupt EEI handler for channel 2.
 *                Types of interrupt requests transfer error or event generation.
 *                The event generations are arbitration-lost, NACK detection, timeout detection, 
 *                start condition detection, and stop condition detection.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void riic2_eei_sub (void)
{
    riic_info_t * p_riic_info;

    #ifdef TN_RXA012A
    riic_timeout_counter_clear(2);
    #endif

    /* Checks Time Out. */
    if ((0U != RIIC2.ICSR2.BIT.TMOF) && (0U != RIIC2.ICIER.BIT.TMOIE))
    {
        /* all interrupt disable */
        RIIC2.ICIER.BIT.TMOIE = 0U;
        while (0U != RIIC2.ICIER.BIT.TMOIE)
        {
            /* Do Nothing */
        }
        riic_api_event[2] = RIIC_EV_INT_TMO;
    }

    /* Checks Arbitration Lost. */
    if ((0U != RIIC2.ICSR2.BIT.AL) && (0U != RIIC2.ICIER.BIT.ALIE))
    {
        RIIC2.ICIER.BIT.ALIE = 0U;
        while (0U != RIIC2.ICIER.BIT.ALIE)
        {
            /* Do Nothing */
        }
        riic_api_event[2] = RIIC_EV_INT_AL;
        if(RIIC_MODE_S_READY == riic_api_info[2].B_Mode)
        {
            riic_set_ch_status(priic_info_s[2], RIIC_COMMUNICATION);
        }
    }

    /* Checks stop condition detection. */
    if ((0U != RIIC2.ICSR2.BIT.STOP) && (0U != RIIC2.ICIER.BIT.SPIE))
    {
        if (((RIIC_MODE_S_READY == riic_api_info[2].N_Mode) || (RIIC_MODE_S_SEND == riic_api_info[2].N_Mode))
                || (RIIC_MODE_S_RECEIVE == riic_api_info[2].N_Mode))
        {
            /* Sets the internal status. */
            riic_api_status_set(priic_info_s[2], RIIC_STS_SP_COND_WAIT);
        }

        /* Prohibits STOP interrupt. */
        RIIC2.ICIER.BIT.SPIE = 0U;

        RIIC2.ICMR3.BIT.RDRFS = 0U; /* Refer to the technical update. */
        RIIC2.ICMR3.BIT.ACKWP = 1U; /* Refer to the technical update. */
        RIIC2.ICMR3.BIT.ACKBT = 0U; /* Refer to the technical update. */
        RIIC2.ICMR3.BIT.ACKWP = 0U; /* Refer to the technical update. */
        while ((0U != RIIC2.ICMR3.BIT.RDRFS) || (0U != RIIC2.ICMR3.BIT.ACKBT))
        {
            /* Do Nothing */
        }

        /* Clears each status flag. */
        RIIC2.ICSR2.BIT.STOP = 0U;
        while (0U != RIIC2.ICSR2.BIT.STOP)
        {
            /* Do Nothing */
        }

        /* Sets event flag. */
        riic_api_event[2] = RIIC_EV_INT_STOP;
    }

    /* Checks NACK reception. */
    if ((0U != RIIC2.ICSR2.BIT.NACKF) && (0U != RIIC2.ICIER.BIT.NAKIE))
    {
        /* Prohibits NACK interrupt to generate stop condition. */
        RIIC2.ICIER.BIT.NAKIE = 0U;

        /* Prohibits these interrupt. */
        /* After NACK interrupt, these interrupts will occur when they do not stop the following interrupts. */
        RIIC2.ICIER.BIT.TEIE = 0U;
        RIIC2.ICIER.BIT.TIE = 0U;
        RIIC2.ICIER.BIT.RIE = 0U;
        while (((0U != RIIC2.ICIER.BIT.TEIE) || (0U != RIIC2.ICIER.BIT.TIE)) || (0U != RIIC2.ICIER.BIT.RIE) )
        {
            /* Do Nothing */
        }

        if (0U == RIIC2.ICCR2.BIT.TRS)
        {
            riic_mcu_clear_ir_rxi(2);
        }

        /* Sets event flag. */
        riic_api_event[2] = RIIC_EV_INT_NACK;
    }

    /* Checks start condition detection. */
    if ((0U != RIIC2.ICSR2.BIT.START) && (0U != RIIC2.ICIER.BIT.STIE))
    {
        /* Disable start condition detection interrupt. */
        /* Clears status flag. */
        RIIC2.ICIER.BIT.STIE = 0U;
        RIIC2.ICSR2.BIT.START = 0U;
        while ((0U != RIIC2.ICSR2.BIT.START) || (0U != RIIC2.ICIER.BIT.STIE))
        {
            /* Do Nothing */
        }

        /* Sets event flag. */
        riic_api_event[2] = RIIC_EV_INT_START;
    }

    switch(riic_api_info[2].N_Mode)
    {
        case RIIC_MODE_M_SEND :
        case RIIC_MODE_M_RECEIVE :
        case RIIC_MODE_M_SEND_RECEIVE :

            /* Master mode data */
            p_riic_info = priic_info_m[2];
        break;

        case RIIC_MODE_S_READY :
        case RIIC_MODE_S_SEND :
        case RIIC_MODE_S_RECEIVE :

            /* Slave mode data */
            p_riic_info = priic_info_s[2];
        break;

        default :

            /* Internal error */
            return;
        break;
    }

    r_riic_advance(p_riic_info); /* Calls advance function */
} /* End of function riic2_eei_sub() */

/***********************************************************************************************************************
 * Function Name: riic2_txi_sub
 * Description  : Interrupt TXI handler for channel 2.
 *                Types of interrupt requests transmission data empty.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void riic2_txi_sub (void)
{
    uint8_t tmp;

    #ifdef TN_RXA012A
    riic_timeout_counter_clear(2);
    #endif

    /* TXI processing is performed only when the following conditions are all met.                 */
    /*  - The address part of data has been received and the received address matches the RX's     */
    /*    own address.                                                                             */
    /*  - The RX device is in slave mode.                                                          */
    /*  - The RX device is in transmission mode (the R/W bit received is 'R' (slave transmission)) */
    tmp = RIIC2.ICCR2.BYTE;
    if ((RIIC_ICCR2_MST_SET != (tmp & RIIC_ICCR2_MST)) && (RIIC_ICCR2_TRS_SET == (tmp & RIIC_ICCR2_TRS)))
    {
        /* Processing when the addresses match (slave send specified) */

        /* Determines whether the state is arbitration-lost detected.                           */
        /* When an arbitration-lost is detected, the riic_after_receive_slvadr function is not  */
        /* executed. Thus the internal status needs to be changed to "RIIC_STS_SEND_DATA_WAIT". */
        if (RIIC_STS_AL == riic_api_info[2].N_status)
        {
            /* Processing when an arbitration-lost is detected. */
            /* Sets the internal status. */
            riic_api_status_set(priic_info_s[2], RIIC_STS_SEND_DATA_WAIT);
        }

        /* Determines if the slave transmission is the first slave transmission.             */
        /* N_mode is updated from "RIIC_MODE_S_READY" to "RIIC_MODE_S_SEND" on the first TXI */
        /* interrupt after the slave transmission is determined.                             */
        if (RIIC_MODE_S_READY == riic_api_info[2].N_Mode)
        {
            /* Processing for the first slave transmission */
            /* Sets the internal mode. */
            riic_api_info[2].N_Mode = RIIC_MODE_S_SEND; /* Set slave transmission mode. */
        }

        /* Sets event. */
        switch (riic_api_info[2].N_status)
        {
            case RIIC_STS_IDLE_EN_SLV :

                /* Updates the channel status. */
                riic_set_ch_status(priic_info_s[2], RIIC_COMMUNICATION);

                /* Sets interrupted address sending. */
                riic_api_event[2] = RIIC_EV_INT_ADD;

            break;

            case RIIC_STS_SEND_DATA_WAIT :

                /* Sets interrupted data sending. */
                riic_api_event[2] = RIIC_EV_INT_SEND;

            break;

            default :

                /* Does nothing. */
            break;

        }

        r_riic_advance(priic_info_s[2]); /* Calls advance function */

    }
} /* End of function riic2_txi_sub() */

/***********************************************************************************************************************
 * Outline      : 
 * Function Name: riic2_rxi_sub
 * Description  : Interrupt RXI handler for channel 2.
 *                Types of interrupt requests reception end.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void riic2_rxi_sub (void)
{
    riic_info_t * p_riic_info;

    #ifdef TN_RXA012A
    riic_timeout_counter_clear(2);
    #endif

    if (RIIC_STS_AL == riic_api_info[2].N_status)
    {
        /* Sets the internal status. */
        riic_api_status_set(priic_info_s[2], RIIC_STS_IDLE_EN_SLV);
    }

    if (RIIC_MODE_S_READY == riic_api_info[2].N_Mode)
    {
        /* Sets the internal mode. */
        riic_api_info[2].N_Mode = RIIC_MODE_S_RECEIVE;
    }

    if ((RIIC_STS_IDLE_EN_SLV == riic_api_info[2].N_status))
    {
        /* Updates the channel status. */
        riic_set_ch_status(priic_info_s[2], RIIC_COMMUNICATION);
    }

    switch (riic_api_info[2].N_Mode)
    {
        case RIIC_MODE_M_RECEIVE :
        case RIIC_MODE_M_SEND_RECEIVE :

            /* Master mode data */
            p_riic_info = priic_info_m[2];
        break;

        case RIIC_MODE_S_RECEIVE :

            /* Slave mode data */
            p_riic_info = priic_info_s[2];
        break;

        default :

            /* Internal error */
            return;
        break;
    }

    /* Sets interrupted data receiving. */
    riic_api_event[2] = RIIC_EV_INT_RECEIVE;

    r_riic_advance(p_riic_info); /* Calls advance function */
} /* End of function riic2_rxi_sub() */

/***********************************************************************************************************************
 * Outline      : 
 * Function Name: riic2_tei_sub
 * Description  : Interrupt TEI handler for channel 2.
 *                Types of interrupt requests transmission end.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
void riic2_tei_sub (void)
{
    #ifdef TN_RXA012A
    riic_timeout_counter_clear(2);
    #endif

    /* Clears ICSR2.TEND. */
    RIIC2.ICSR2.BIT.TEND = 0U;
    while (0U != RIIC2.ICSR2.BIT.TEND)
    {
        /* Do Nothing */
    }

    /* Sets event. */
    switch (riic_api_info[2].N_status)
    {
        case RIIC_STS_SEND_SLVADR_W_WAIT :
        case RIIC_STS_SEND_SLVADR_R_WAIT :

            /* Sets interrupted address sending. */
            riic_api_event[2] = RIIC_EV_INT_ADD;

        break;

        case RIIC_STS_SEND_DATA_WAIT :

            /* Sets interrupted data sending. */
            riic_api_event[2] = RIIC_EV_INT_SEND;

        break;

        default :

            /* Does nothing. */
        break;

    }

    r_riic_advance(priic_info_m[2]); /* Calls advance function */
} /* End of function riic2_tei_sub() */
#endif

/***********************************************************************************************************************
 * Function Name: riic_timeout_counter_clear
 * Description  : Technical update No.TN-RX*-A012A process.
 *                Time out internal counter clear.
 * Arguments    : None
 * Return Value : None
 **********************************************************************************************************************/
#ifdef TN_RXA012A
static void riic_timeout_counter_clear (uint8_t channel)
{
    volatile uint8_t * const picmr2_reg = RIIC_ICMR2_ADR(channel);
    volatile uint8_t * const ptmocntl_reg = RIIC_TMOCNTL_ADR(channel);
    volatile uint8_t * const ptmocnth_reg = RIIC_TMOCNTH_ADR(channel);

    /* time out internal counter write enable */
    (*picmr2_reg) |= RIIC_ICMR2_TMWE_SET;
    if (RIIC_ICMR2_TMWE == ((*picmr2_reg) & RIIC_ICMR2_TMWE))
    {
        /* Do Nothing */
    }

    /* time out iternal counter clear */
    (*ptmocntl_reg) = 0x00;
    (*ptmocnth_reg) = 0x00;
    if (0xFF == (*ptmocnth_reg))
    {
        /* Do Nothing */
    }

    /* time out internal counter write disable */
    (*picmr2_reg) &= RIIC_ICMR2_TMWE_CLR;
    if (0x00 == ((*picmr2_reg) & RIIC_ICMR2_TMWE))
    {
        /* Do Nothing */
    }
} /* End of function riic_timeout_counter_clear() */
#endif

