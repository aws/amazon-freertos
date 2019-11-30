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
* Copyright (C) 2017-2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_flash_targets.h
* Description  : This is a private header file used internally by the FLASH module. It should not be modified or
*                included by the user in their application.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           02.08.2017 1.00    First Release
*           27.02.2018 1.10    Added RX66T
*           23.10.2018 1.20    Added RX72T
*         : 19.04.2019 4.00    Removed support for flash type 2.
***********************************************************************************************************************/

#ifndef FLASH_TARGETS_HEADER_FILE
#define FLASH_TARGETS_HEADER_FILE

#include <stdbool.h>
#include "platform.h"       // provides variable types and BSP equates

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/* WHEN BSP IS USED, MAP FLASH MCU_xxx EQUATES TO BSP EQUATES HERE */

#define MCU_CFG_PART_MEMORY_SIZE    BSP_CFG_MCU_PART_MEMORY_SIZE
#define MCU_ROM_SIZE_BYTES          BSP_ROM_SIZE_BYTES
#define MCU_RAM_SIZE_BYTES          BSP_RAM_SIZE_BYTES
#define MCU_DATA_FLASH_SIZE_BYTES   BSP_DATA_FLASH_SIZE_BYTES

#if   defined(BSP_MCU_RX111)
    #define MCU_RX111
    #define MCU_RX11_ALL
#elif defined(BSP_MCU_RX110)
    #define MCU_RX110
    #define MCU_RX11_ALL
#elif defined(BSP_MCU_RX113)
    #define MCU_RX113
    #define MCU_RX11_ALL
#elif defined(BSP_MCU_RX130)
    #define MCU_RX130
    #define MCU_RX13_ALL
#elif defined(BSP_MCU_RX231)
    #define MCU_RX231
    #define MCU_RX23_ALL
#elif defined(BSP_MCU_RX230)
    #define MCU_RX230
    #define MCU_RX23_ALL
#elif defined(BSP_MCU_RX23T)
    #define MCU_RX23T
    #define MCU_RX23_ALL
#elif defined(BSP_MCU_RX24T)
    #define MCU_RX24T
    #define MCU_RX24_ALL
#elif defined(BSP_MCU_RX24U)
    #define MCU_RX24U
    #define MCU_RX24_ALL
#elif defined(BSP_MCU_RX64M)
    #define MCU_RX64M
    #define MCU_RX64_ALL
#elif defined(BSP_MCU_RX651) || defined(BSP_MCU_RX65N)
    #define MCU_RX651
    #define MCU_RX65N
    #define MCU_RX65_ALL
#elif defined(BSP_MCU_RX66T)
    #define MCU_RX66T
    #define MCU_RX66_ALL
#elif defined(BSP_MCU_RX71M)
    #define MCU_RX71M
    #define MCU_RX71_ALL
#elif defined(BSP_MCU_RX72T)
    #define MCU_RX72T
    #define MCU_RX72_ALL
#else
    #error "!!! No 'targets' folder for this MCU Group !!!"
#endif

#define MCU_CFG_ICLK_HZ             BSP_ICLK_HZ
#define MCU_CFG_FCLK_HZ             BSP_FCLK_HZ



/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/


/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/


/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/

#endif /* FLASH_TARGETS_HEADER_FILE */

/* end of file */
