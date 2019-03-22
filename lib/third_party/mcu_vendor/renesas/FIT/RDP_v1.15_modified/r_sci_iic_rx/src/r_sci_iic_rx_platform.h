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
 * Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_sci_iic_rx_platform.h
 * Description  : Functions for using SCI_IIC on RX devices. 
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 22.09.2014 1.00     First Release
 *         : 01.12.2014 1.40     RX113 support added.
 *         : 15.12.2014 1.50     RX71M support added.
 *         : 27.02.2015 1.60     RX63N support added.
 *         : 29.05.2015 1.70     RX231 support added.
 *         : 31.10.2015 1.80     RX130, RX230, RX23T support added.
 *         : 04.03.2016 1.90     RX24T support added.
 *         : 01.10.2016 2.00     RX65N support added.
 *         : 16.12.2016 2.20     Changed include path becuase changed file structure.
 *         :                     RX24U support added.
 **********************************************************************************************************************/
/* Guards against multiple inclusion */
#ifndef SCI_IIC_PLATFORM_H
    #define SCI_IIC_PLATFORM_H
/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
/* Include specifics for chosen MCU.  */
    #if defined(BSP_MCU_RX111)
        #include "./targets/rx111/r_sci_iic_rx111_private.h"
    #elif defined(BSP_MCU_RX110)
        #include "./targets/rx110/r_sci_iic_rx110_private.h"
    #elif defined(BSP_MCU_RX113)
        #include "./targets/rx113/r_sci_iic_rx113_private.h"
    #elif defined(BSP_MCU_RX130)
        #include "./targets/rx130/r_sci_iic_rx130_private.h"
    #elif defined(BSP_MCU_RX230)
        #include "./targets/rx230/r_sci_iic_rx230_private.h"
    #elif defined(BSP_MCU_RX231)
        #include "./targets/rx231/r_sci_iic_rx231_private.h"
    #elif defined(BSP_MCU_RX23T)
        #include "./targets/rx23t/r_sci_iic_rx23t_private.h"
    #elif defined(BSP_MCU_RX24T)
        #include "./targets/rx24t/r_sci_iic_rx24t_private.h"
    #elif defined(BSP_MCU_RX24U)
        #include "./targets/rx24u/r_sci_iic_rx24u_private.h"
    #elif defined(BSP_MCU_RX63N)
        #include "./targets/rx63n/r_sci_iic_rx63n_private.h"
    #elif defined(BSP_MCU_RX64M)
        #include "./targets/rx64m/r_sci_iic_rx64m_private.h"
    #elif defined(BSP_MCU_RX65N)
        #include "./targets/rx65n/r_sci_iic_rx65n_private.h"
    #elif defined(BSP_MCU_RX71M)
        #include "./targets/rx71m/r_sci_iic_rx71m_private.h"
    #else
        #error "This MCU is not supported by the current r_sci_iic_rx module."
    #endif

#endif /* SCI_IIC_PLATFORM_H */

