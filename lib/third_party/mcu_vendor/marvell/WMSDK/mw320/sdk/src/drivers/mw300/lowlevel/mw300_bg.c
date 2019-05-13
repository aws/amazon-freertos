/****************************************************************************//**
 * @file     mw300_bg.c
 * @brief    This file provides BG functions.
 * @version  V1.3.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 *
 * @note
 * Copyright (C) 2012 Marvell Technology Group Ltd. All rights reserved.
 *
 * @par
 * Marvell is supplying this software which provides customers with programming
 * information regarding the products. Marvell has no responsibility or 
 * liability for the use of the software. Marvell not guarantee the correctness 
 * of this software. Marvell reserves the right to make changes in the software 
 * without notification. 
 * 
 *******************************************************************************/

#include "mw300.h"
#include "mw300_bg.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @defgroup BG BG
 *  @brief BG driver modules
 *  @{
 */

/** @defgroup BG_Private_Type
 *  @{
 */

/*@} end of group BG_Private_Type*/

/** @defgroup BG_Private_Defines
 *  @{
 */

/*@} end of group BG_Private_Defines */

/** @defgroup BG_Private_Variables
 *  @{
 */

/*@} end of group BG_Private_Variables */

/** @defgroup BG_Global_Variables
 *  @{
 */

/*@} end of group BG_Global_Variables */

/** @defgroup BG_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group BG_Private_FunctionDeclaration */

/** @defgroup BG_Private_Functions
 *  @{
 */

/*@} end of group BG_Private_Functions */

/** @defgroup BG_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      Power Up Bandgap
 *
 * @param      none
 *
 * @return none
 *
 *******************************************************************************/
void BG_PowerUp(void)
{
  BG->CTRL.BF.PD = 0;
}

/****************************************************************************//**
 * @brief      Power Down Bandgap
 *
 * @param      none
 *
 * @return none
 *
 *******************************************************************************/
void BG_PowerDown(void)
{
  BG->CTRL.BF.PD = 1;
}

/*@} end of group BG_Public_Functions */

/*@} end of group BG */

/*@} end of group MW300_Periph_Driver */

