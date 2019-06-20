/** @file     mw300_wdt.c
 *
 *  @brief    This file provides WDT functions.
 *
 *  (C) Copyright 2012-2018 Marvell International Ltd. All Rights Reserved.
 *
 *  MARVELL CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Material") are owned by Marvell International Ltd or its
 *  suppliers or licensors. Title to the Material remains with Marvell
 *  International Ltd or its suppliers and licensors. The Material contains
 *  trade secrets and proprietary and confidential information of Marvell or its
 *  suppliers and licensors. The Material is protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Material may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without Marvell's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by Marvell in writing.
 *
 */

#include "mw300.h"
#include "mw300_wdt.h"
#include "mw300_driver.h"
#include <compat_attribute.h>

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @defgroup WDT WDT
 *  @brief WDT driver modules
 *  @{
 */

/** @defgroup WDT_Private_Type
 *  @{
 */

/*@} end of group WDT_Private_Type*/

/** @defgroup WDT_Private_Defines
 *  @{
 */

/**
 *  @brief WDT feed pattern 
 */
#define WDT_FEED_PATTERN            0x76 


/*@} end of group WDT_Private_Defines */


/** @defgroup WDT_Private_Variables
 *  @{
 */


/*@} end of group WDT_Private_Variables */

/** @defgroup WDT_Global_Variables
 *  @{
 */


/*@} end of group WDT_Global_Variables */


/** @defgroup WDT_Private_FunctionDeclaration
 *  @{
 */


/*@} end of group WDT_Private_FunctionDeclaration */

/** @defgroup WDT_Private_Functions
 *  @{
 */


/*@} end of group WDT_Private_Functions */

/** @defgroup WDT_Public_Functions
 *  @{
 */


/****************************************************************************//**
 * @brief      Initialize the WDT 
 *
 * @param[in]  WDTConfig:  Pointer to a WDT configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void WDT_Init(WDT_Config_Type* wdtConfig)
{
  /* set watchdog initial tomeout value */
  WDT->TORR.BF.TOP_INIT = wdtConfig->timeoutVal;

  /* set watchdog tomeout value */
  WDT->TORR.BF.TOP = wdtConfig->timeoutVal;

  /* set watchdog mode */
  WDT->CR.BF.RMOD = wdtConfig->mode;

  /* set watchdog reset pulse width */
  WDT->CR.BF.RPL = wdtConfig->resetPulseLen;
}

/****************************************************************************//**
 * @brief      Enable WDT counter 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void WDT_Enable(void)
{
  /* enable WDT counter */
  WDT->CR.BF.EN = 1;
}

/****************************************************************************//**
 * @brief      Disable WDT counter 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void WDT_Disable(void)
{
  /* disable WDT counter */
  WDT->CR.BF.EN = 0;
}

/****************************************************************************//**
 * @brief      Set WDT response mode 
 *
 * @param[in]  mode:  WDT response mode
 *
 * @return none
 *
 *******************************************************************************/
void WDT_SetMode(WDT_Mode_Type mode)
{
  /* set WDT response mode */
  WDT->CR.BF.RMOD = mode;
}

/****************************************************************************//**
 * @brief      Set WDT timeout value 
 *
 * @param[in]  timeoutVal:  counter timeout value
 *
 * @return none
 *
 *******************************************************************************/
void WDT_SetTimeoutVal(uint32_t timeoutVal)
{
  /* set WDT timeout value */
  WDT->TORR.BF.TOP = timeoutVal;
}
 
/****************************************************************************//**
 * @brief      Set WDT reset pulse lenghth value 
 *
 * @param[in]  resetPulseLen:  WDT reset pulse lenghth
 *
 * @return none
 *
 *******************************************************************************/
void WDT_SetResetPulseLen(WDT_ResetPulseLen_Type resetPulseLen)
{
  /* set WDT reset pulse length value */
  WDT->CR.BF.RPL = resetPulseLen;
}

/****************************************************************************//**
 * @brief      Get WDT current counter value 
 *
 * @param  none
 *
 * @return WDT current counter value
 *
 *******************************************************************************/
uint32_t WDT_GetCounterVal(void)
{
  return WDT->CCVR.WORDVAL;
}

/****************************************************************************//**
 * @brief      restart WDT counter value
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void WDT_RestartCounter(void)
{
  /* disable irq interrupt */
  __disable_irq();
  /* feed the dog with fixed pattern */
  WDT->CRR.BF.CRR = WDT_FEED_PATTERN;
  /* then enable irq interrupt */ 
  __enable_irq();
}
    
/****************************************************************************//**
 * @brief      Clear WDT interrupt flag
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void WDT_IntClr(void)
{
	__UNUSED__ volatile uint32_t tmpData;
  
  /* clear interrupt flag */
  tmpData = WDT->EOI.WORDVAL;
}

/****************************************************************************//**
 * @brief      Get WDT interrupt status
 *
 * @param  none
 *
 * @return interrupt status
 *
 *******************************************************************************/
IntStatus WDT_GetIntStatus(void)
{
  /* get interrupt status */
  return (IntStatus)WDT->STAT.BF.STAT;
}


/****************************************************************************//**
 * @brief  WDT interrupt function 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void WDT_IRQHandler(void)
{
  /* check call back function availability */
  if(intCbfArra[INT_WDT][0] != NULL)
  {
    /* call the call back function */
    intCbfArra[INT_WDT][0]();
  }
}


/*@} end of group WDT_Public_Functions */

/*@} end of group WDT  */

/*@} end of group MW300_Periph_Driver */

