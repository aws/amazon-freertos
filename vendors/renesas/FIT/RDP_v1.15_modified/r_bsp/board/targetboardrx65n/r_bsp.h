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
* H/W Platform : TARGETBOARDRX65N
* Description  : Has the header files that should be included for this platform.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 01.11.2017 1.00     First Release
*         : 01.07.2018 1.01     Additional RTOS header file.
*         : xx.xx.xxxx 1.02     Added support for GNUC and ICCRX.
*                               Added #include for board/all/cksetup.h.
*                               Relocated hwsetup.h, lowsrc.h and vecttbl.h to the 'board/all' folder.
***********************************************************************************************************************/

#ifndef BSP_BOARD_TARGETBOARDRX65N
#define BSP_BOARD_TARGETBOARDRX65N

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
#include    "mcu/all/r_compiler.h"
#include    "r_bsp_config.h"
#include    "r_bsp_interrupt_config.h"
#if defined(__CCRX__)
#include    "mcu/rx65n/register_access/ccrx/iodefine.h"
#elif defined(__GNUC__)
#include    "mcu/rx65n/register_access/gnuc/iodefine.h"
#elif defined(__ICCRX__)
#include    "mcu/rx65n/register_access/iccrx/iodefine.h"
#endif /* defined(__CCRX__), defined(__GNUC__), defined(__ICCRX__) */
#include    "mcu/rx65n/mcu_info.h"
#include    "mcu/rx65n/mcu_locks.h"
#include    "mcu/rx65n/locking.h"
#include    "mcu/rx65n/cpu.h"
#include    "mcu/rx65n/mcu_init.h"
#include    "mcu/rx65n/mcu_interrupts.h"
#include    "mcu/rx65n/mcu_mapped_interrupts_private.h"
#include    "mcu/rx65n/mcu_mapped_interrupts.h"
#include    "mcu/rx65n/mcu_startup.h"
#include    "board/targetboardrx65n/targetboardrx65n.h"
#include    "board/all/clkconf.h"
#include    "board/all/hwsetup.h"
#include    "board/all/lowsrc.h"
#include    "board/all/vecttbl.h"
#if BSP_CFG_RTOS_USED == 0      // Non-OS
#elif BSP_CFG_RTOS_USED == 1    // FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "croutine.h"
#include "timers.h"
#include "event_groups.h"
#include "message_buffer.h"
#include "freertos_start.h"
#elif BSP_CFG_RTOS_USED == 2    // SEGGER embOS
#elif BSP_CFG_RTOS_USED == 3    // Micrium MicroC/OS
#elif BSP_CFG_RTOS_USED == 4    // Renesas RI600V4 & RI600PX
#else
#endif

#ifdef __cplusplus
}
#endif

#endif /* BSP_BOARD_TARGETBOARDRX65N */

