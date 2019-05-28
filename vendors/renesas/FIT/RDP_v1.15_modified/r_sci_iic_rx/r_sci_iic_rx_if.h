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
 * File Name    : r_sci_iic_rx_if.h
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
 *         : 04.03.2016 1.90     RX24T support added.Changed about the pin definisions.
 *         : 01.10.2016 2.00     Updated version to 2.00 for RX65N release
 *         : 31.08.2017 2.20     Changed minor version to '20' for RX24U, RX130-512KB, and RX65N-2MB support.
 **********************************************************************************************************************/
/* Guards against multiple inclusion */
#ifndef SCI_IIC_IF_H
    #define SCI_IIC_IF_H

#ifdef __cplusplus
extern "C" {
#endif

    R_PRAGMA_UNPACK

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
/* Version Number of API. */
    #define SCI_IIC_VERSION_MAJOR  (2)
    #define SCI_IIC_VERSION_MINOR  (20)

/*----------------------------------------------------------------------------*/
/*   Define return values and values of channel state flag.                   */
/*----------------------------------------------------------------------------*/
    #define SCI_IIC_NO_INIT         ((sci_iic_ch_dev_status_t)(0))
    #define SCI_IIC_IDLE            ((sci_iic_ch_dev_status_t)(1))
    #define SCI_IIC_FINISH          ((sci_iic_ch_dev_status_t)(2))
    #define SCI_IIC_NACK            ((sci_iic_ch_dev_status_t)(3))
    #define SCI_IIC_COMMUNICATION   ((sci_iic_ch_dev_status_t)(4))
    #define SCI_IIC_ERROR           ((sci_iic_ch_dev_status_t)(5))

/*----------------------------------------------------------------------------*/
/*   Defines the argument of the R_SCI_IIC_Control function.                  */
/*----------------------------------------------------------------------------*/
    #define SCI_IIC_GEN_ERR_CON        ((sci_iic_ctrl_ptn_t)(0x00))   /* Err output */
    #define SCI_IIC_GEN_START_CON      ((sci_iic_ctrl_ptn_t)(0x01))   /* Generate Start Condition */
    #define SCI_IIC_GEN_STOP_CON       ((sci_iic_ctrl_ptn_t)(0x02))   /* Generate Stop Condition  */
    #define SCI_IIC_GEN_RESTART_CON    ((sci_iic_ctrl_ptn_t)(0x04))   /* Generate Restart Condition*/
    #define SCI_IIC_GEN_SSDA_HI_Z      ((sci_iic_ctrl_ptn_t)(0x08))   /* Nack reception */
    #define SCI_IIC_GEN_SSCL_ONESHOT   ((sci_iic_ctrl_ptn_t)(0x10))   /* Output one shot of SSCL clock */
    #define SCI_IIC_GEN_RESET          ((sci_iic_ctrl_ptn_t)(0x20))   /* Reset SCI_IIC module */

/***********************************************************************************************************************
 Typedef definitions
 **********************************************************************************************************************/
typedef uint8_t sci_iic_ch_dev_status_t;
typedef uint8_t sci_iic_ctrl_ptn_t;

typedef enum /* SCI_IIC API error codes */
{
    SCI_IIC_SUCCESS = 0U,
    SCI_IIC_ERR_LOCK_FUNC,                  /* chan still running in another mode */
    SCI_IIC_ERR_INVALID_CHAN,               /* non-existent channel number */
    SCI_IIC_ERR_INVALID_ARG,
    SCI_IIC_ERR_NO_INIT,
    SCI_IIC_ERR_BUS_BUSY,
    SCI_IIC_ERR_OTHER
} sci_iic_return_t;

/*----------------------------------------------------------------------------*/
/*   Define sci_iic information structure type.                               */
/*----------------------------------------------------------------------------*/
/*----- Callback function type. -----*/
typedef void (*sci_iic_callback) (void); /* Callback function type */

/*----- Structure type. -----*/
/* SCI_IIC Information structure. */
typedef R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_10
(
    uint8_t                 rsv2, /* reserved */
    uint8_t                 rsv1, /* reserved */
    sci_iic_ch_dev_status_t dev_sts, /* Device status flag */
    uint8_t                 ch_no, /* Channel No. */
    sci_iic_callback        callbackfunc, /* Callback function */
    uint32_t                cnt2nd, /* 2nd Data Counter */
    uint32_t                cnt1st, /* 1st Data Counter */
    uint8_t               * p_data2nd, /* Pointer for 2nd Data buffer */
    uint8_t               * p_data1st, /* Pointer for 1st Data buffer */
    uint8_t               * p_slv_adr  /* Pointer for Slave address buffer */
) sci_iic_info_t;

/*----------------------------------------------------------------------------*/
/*   Define sci_iic status structure type.                                    */
/*----------------------------------------------------------------------------*/
typedef union
{
    uint32_t                LONG;
    R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_6
    (
        uint32_t rsv :27, /* reserve */
        uint32_t SCLI :1, /* SSCL Pin Level */
        uint32_t SDAI :1, /* SSDA Pin Level */
        uint32_t NACK :1, /* NACK detection flag */
        uint32_t TRS :1, /* Send mode / Receive mode flag */
        uint32_t BSY :1  /* Bus status flag */
    ) BIT;
} sci_iic_mcu_status_t;

/***********************************************************************************************************************
 Exported global variables
 **********************************************************************************************************************/
extern volatile sci_iic_ch_dev_status_t g_sci_iic_ChStatus[]; /* Channel state flag */

/***********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
/* IIC Driver API functions */
sci_iic_return_t R_SCI_IIC_Open (sci_iic_info_t * p_sci_iic_info);
sci_iic_return_t R_SCI_IIC_MasterSend (sci_iic_info_t * p_sci_iic_info);
sci_iic_return_t R_SCI_IIC_MasterReceive (sci_iic_info_t * p_sci_iic_info);
sci_iic_return_t R_SCI_IIC_GetStatus (sci_iic_info_t *p_sci_iic_info, sci_iic_mcu_status_t *p_sci_iic_status);
sci_iic_return_t R_SCI_IIC_Control (sci_iic_info_t * p_sci_iic_info, sci_iic_ctrl_ptn_t ctrl_ptn);
sci_iic_return_t R_SCI_IIC_Close (sci_iic_info_t * p_sci_iic_info);
uint32_t R_SCI_IIC_GetVersion (void);

    R_PRAGMA_PACKOPTION

#ifdef __cplusplus
}
#endif

#endif /* SCI_IIC_IF_H */

