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
* Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_bsp.h
* H/W Platform : GENERIC_RX65N
* Description  : Has the header files that should be included for this platform.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 15.05.2017 1.00     First Release
*         : 01.07.2018 1.01     Additional RTOS header file.
*         : 28.02.2019 2.00     Added the following include path.
*                                - r_rx_compiler.h
*                                - r_rtos.h
*                                - r_bsp_interrupts.h
*                                - mcu_clocks.h
*                                - r_rx_intrinsic_functions.h
*                               Modified the following include path.
*                                - lowsrc.h
*                                - r_bsp_mcu_startup.h
*                                - vecttbl.h
*                               Added support for GNUC and ICCRX.
*                               Fixed coding style.
*         : 08.10.2019 2.01     Added the following include path.
*                                - r_bsp_software_interrupt.h
*                               Changed include of iodefine.h for CS+ and RX651.
*                               Changed include of r_bsp_config.h for added support of Renesas RTOS(RI600V4 or RI600PX).
***********************************************************************************************************************/

/* Make sure that no other platforms have already been defined. Do not touch this! */
#ifdef  PLATFORM_DEFINED
#error  "Error - Multiple platforms defined in platform.h!"
#else
#define PLATFORM_DEFINED
#endif

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************************************************
INCLUDE APPROPRIATE MCU AND BOARD FILES
***********************************************************************************************************************/
#include    "mcu/all/r_bsp_common.h"
#include    "r_bsp_config.h"
#include    "mcu/all/r_rx_compiler.h"

#include    "r_bsp_interrupt_config.h"

#include    "mcu/all/lowlvl.h"
#include    "mcu/all/lowsrc.h"
#include    "mcu/all/r_bsp_mcu_startup.h"

#ifndef __RX651IODEFINE_HEADER__
#define __RX651IODEFINE_HEADER__
#if defined(__CCRX__)
#include    "mcu/rx65n/register_access/ccrx/iodefine.h"
#elif defined(__GNUC__)
#include    "mcu/rx65n/register_access/gnuc/iodefine.h"
#elif defined(__ICCRX__)
#include    "mcu/rx65n/register_access/iccrx/iodefine.h"
#endif /* defined(__CCRX__), defined(__GNUC__), defined(__ICCRX__) */
#endif /* __RX651IODEFINE_HEADER__ */
#include    "mcu/rx65n/r_bsp_cpu.h"
#include    "mcu/rx65n/r_bsp_locking.h"
#include    "mcu/rx65n/mcu_clocks.h"
#include    "mcu/rx65n/mcu_info.h"
#include    "mcu/rx65n/mcu_init.h"
#include    "mcu/rx65n/mcu_interrupts.h"
#include    "mcu/rx65n/mcu_locks.h"
#include    "mcu/rx65n/mcu_mapped_interrupts_private.h"
#include    "mcu/rx65n/mcu_mapped_interrupts.h"
#include    "mcu/rx65n/vecttbl.h"

#include    "board/generic_rx65n/hwsetup.h"

#include    "mcu/all/r_bsp_interrupts.h"
#include    "mcu/all/r_bsp_software_interrupt.h"
#include    "mcu/all/r_rx_intrinsic_functions.h"
#include    "mcu/all/r_rtos.h"

#ifdef __cplusplus
}
#endif

#ifndef BSP_BOARD_GENERIC_RX65N
#define BSP_BOARD_GENERIC_RX65N

#endif /* BSP_BOARD_GENERIC_RX65N */

