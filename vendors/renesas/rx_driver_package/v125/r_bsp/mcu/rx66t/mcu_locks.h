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
* File Name    : mcu_locks.h
* Device(s)    : RX66T
* Description  : This source file has 1 lock per MCU resource.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 27.07.2018 1.00     First Release
*         : 28.02.2019 1.01     Deleted the following enumeration constant.
*                               - BSP_LOCK_SMCIx (x = 1, 5, 6, 8, 9, 11 and 12.)
*                               Fixed coding style.
*         : 08.10.2019 1.02     Added the following enumeration constant.
*                               - BSP_LOCK_SWINT
*                               - BSP_LOCK_SWINT2
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Gets MCU configuration information. */
#include "r_bsp_config.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Multiple inclusion prevention macro */
#ifndef MCU_LOCKS_H
#define MCU_LOCKS_H

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* This enum defines all of the available hardware locks for this MCU. If you delete an entry out of this list then you
   will decrease the size of the locks array but will not be able to use that lock. For example, if your design is not
   using CAN at all then you can safely remove the BSP_LOCK_CAN# entries below. */
typedef enum
{
    BSP_LOCK_BSC = 0,
    BSP_LOCK_CAC,
    BSP_LOCK_CAN0,
    BSP_LOCK_CMPC0,
    BSP_LOCK_CMPC1,
    BSP_LOCK_CMPC2,
    BSP_LOCK_CMPC3,
    BSP_LOCK_CMPC4,
    BSP_LOCK_CMPC5,
    BSP_LOCK_CMT,
    BSP_LOCK_CMT0,
    BSP_LOCK_CMT1,
    BSP_LOCK_CMT2,
    BSP_LOCK_CMT3,
    BSP_LOCK_CRC,
    BSP_LOCK_DA,
    BSP_LOCK_DMAC,
    BSP_LOCK_DMAC0,
    BSP_LOCK_DMAC1,
    BSP_LOCK_DMAC2,
    BSP_LOCK_DMAC3,
    BSP_LOCK_DMAC4,
    BSP_LOCK_DMAC5,
    BSP_LOCK_DMAC6,
    BSP_LOCK_DMAC7,
    BSP_LOCK_DOC,
    BSP_LOCK_DTC,
    BSP_LOCK_ELC,
    BSP_LOCK_FLASH,
    BSP_LOCK_GPTW0,
    BSP_LOCK_GPTW1,
    BSP_LOCK_GPTW2,
    BSP_LOCK_GPTW3,
    BSP_LOCK_GPTW4,
    BSP_LOCK_GPTW5,
    BSP_LOCK_GPTW6,
    BSP_LOCK_GPTW7,
    BSP_LOCK_GPTW8,
    BSP_LOCK_GPTW9,
    BSP_LOCK_HRPWM,
    BSP_LOCK_IRQ0,
    BSP_LOCK_IRQ1,
    BSP_LOCK_IRQ2,
    BSP_LOCK_IRQ3,
    BSP_LOCK_IRQ4,
    BSP_LOCK_IRQ5,
    BSP_LOCK_IRQ6,
    BSP_LOCK_IRQ7,
    BSP_LOCK_IRQ8,
    BSP_LOCK_IRQ9,
    BSP_LOCK_IRQ10,
    BSP_LOCK_IRQ11,
    BSP_LOCK_IRQ12,
    BSP_LOCK_IRQ13,
    BSP_LOCK_IRQ14,
    BSP_LOCK_IRQ15,
    BSP_LOCK_ICU,
    BSP_LOCK_IWDT,
    BSP_LOCK_MPC,
    BSP_LOCK_MPU,
    BSP_LOCK_MTU,
    BSP_LOCK_MTU0,
    BSP_LOCK_MTU1,
    BSP_LOCK_MTU2,
    BSP_LOCK_MTU3,
    BSP_LOCK_MTU4,
    BSP_LOCK_MTU5,
    BSP_LOCK_MTU6,
    BSP_LOCK_MTU7,
    BSP_LOCK_MTU9,
    BSP_LOCK_POE,
    BSP_LOCK_POEG,
    BSP_LOCK_RAM,
    BSP_LOCK_RIIC0,
    BSP_LOCK_RSPI0,
    BSP_LOCK_S12AD,
    BSP_LOCK_S12AD1,
    BSP_LOCK_S12AD2,
    BSP_LOCK_SCI1,
    BSP_LOCK_SCI5,
    BSP_LOCK_SCI6,
    BSP_LOCK_SCI8,
    BSP_LOCK_SCI9,
    BSP_LOCK_SCI11,
    BSP_LOCK_SCI12,
    BSP_LOCK_SYSTEM,
    BSP_LOCK_TEMPS,
    BSP_LOCK_TMR0,
    BSP_LOCK_TMR1,
    BSP_LOCK_TMR2,
    BSP_LOCK_TMR3,
    BSP_LOCK_TMR4,
    BSP_LOCK_TMR5,
    BSP_LOCK_TMR6,
    BSP_LOCK_TMR7,
    BSP_LOCK_TMR01,
    BSP_LOCK_TMR23,
    BSP_LOCK_TMR45,
    BSP_LOCK_TMR67,
    BSP_LOCK_USB0,
    BSP_LOCK_WDT,
    BSP_LOCK_SWINT,
    BSP_LOCK_SWINT2,
    BSP_NUM_LOCKS //This entry is not a valid lock. It is used for sizing g_bsp_Locks[] array below. Do not touch!
} mcu_lock_t;

typedef struct
{
    /* The actual lock. int32_t is used because this is what the xchg() instruction takes as parameters. */
    int32_t     lock;

    /* Could add a ID for locking and unlocking. In this could protect against any function being able to unlock. */
} bsp_lock_t;

/***********************************************************************************************************************
Error checking
***********************************************************************************************************************/
#if BSP_CFG_USER_LOCKING_ENABLED == 0
#undef  BSP_CFG_USER_LOCKING_TYPE
#define BSP_CFG_USER_LOCKING_TYPE       bsp_lock_t
#else
    #if !defined(BSP_CFG_USER_LOCKING_TYPE)
    #error "R_BSP ERROR - If you are using your own locking mechanism then you must define BSP_CFG_USER_LOCKING_TYPE in r_bsp_config.h."
    #endif
#endif

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/
extern BSP_CFG_USER_LOCKING_TYPE g_bsp_Locks[];

#endif /* MCU_LOCKS_H */

