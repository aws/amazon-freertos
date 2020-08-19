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
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_bsp_vbatt.c
* Description  : Contains battery backup function routines.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 26.07.2019 1.00     First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "platform.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_BSP_VbattInitialSetting
 ******************************************************************************************************************//**
* @brief Initialize the battery backup function(VBATT).
 * @details Initialize the battery backup function(VBATT). This processing is needed for using the BSP and peripheral 
 * FIT modules. Call this function only if all the following conditions are satisfied.
 * - The BSP startup is disabled.
 * - Do not use the battery backup function.
 * - Access to the RTC related registers.
 * 
 * Call this function before accessing RTC related registers.
 * @note The R_BSP_VbattInitialSetting function performs a part of processing in the startup function. This function 
 * is enabled if BSP startup is disabled and the battery backup function is not used.
 * See Section 5.24 in the application note for more information.
 */
#if (BSP_CFG_STARTUP_DISABLE != 0) && (BSP_CFG_VBATT_ENABLE == 0)
void R_BSP_VbattInitialSetting(void)
{
    /* Wait for power voltage stabilization of VBATT function. */
    vbatt_voltage_stability_wait();
}
#endif

#if (BSP_CFG_VBATT_ENABLE == 0)
/***********************************************************************************************************************
* Function Name: vbatt_voltage_stability_wait
* Description  : Wait for power voltage stabilization of VBATT function.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void vbatt_voltage_stability_wait (void)
{
    /* Protect off. DO NOT USE R_BSP_RegisterProtectDisable()! (not initialized yet) */
    SYSTEM.PRCR.WORD = 0xA508;

    /* Disable vbatt function. */
    SYSTEM.VBATTCR.BIT.VBATTDIS = 1;

    /* WAIT_LOOP */
    while (0 != SYSTEM.VBATTSR.BIT.VBATRLVDETF)
    {
        /* Reset Battery Backup Power Voltage Drop Detection Flag. */
        SYSTEM.VBATTSR.BIT.VBATRLVDETF = 0;
    }

    /* Protect on. */
    SYSTEM.PRCR.WORD = 0xA500;

} /* End of function vbatt_voltage_stability_wait() */
#endif /* (defined(BSP_CFG_VBATT_ENABLE) && (BSP_CFG_VBATT_ENABLE == 0)) */

