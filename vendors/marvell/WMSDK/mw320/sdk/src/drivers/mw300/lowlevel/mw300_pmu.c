/** @file     mw300_pmu.c
 *
 *  @brief    This file provides PMU functions.
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

#include "mw300_pmu.h"
#include <compat_attribute.h>

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @defgroup PMU
 *  @brief PMU driver modules
 *  @{
 */

/** @defgroup PMU_Private_Type
 *  @{
 */

/**
 *  @brief PMU interrupt status type definition
 */


/**
 * @brief PMU interrupt call-back function type definitions
 */



/*@} end of group PMU_Private_Type*/

/** @defgroup PMU_Private_Defines
 *  @{
 */


/* PMU interrupt status bit mask */


/*@} end of group PMU_Private_Defines */


/** @defgroup PMU_Private_Variables
 *  @{
 */


        
/*@} end of group PMU_Private_Variables */

/** @defgroup PMU_Global_Variables
 *  @{
 */


/*@} end of group PMU_Global_Variables */


/** @defgroup PMU_Private_FunctionDeclaration
 *  @{
 */


/*@} end of group PMU_Private_FunctionDeclaration */

/** @defgroup PMU_Private_Functions
 *  @{
 */


/*@} end of group PMU_Private_Functions */


/** @defgroup PMU_Public_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief     Set system will be in sleep mode
 *
 * @param[in]  pmuMode:  power mode option
 *
 * @return none
 *
 *******************************************************************************/
void PMU_SetSleepMode(PMU_SleepMode_Type pmuMode)
{
  CHECK_PARAM(IS_PMU_POWER_MODE(pmuMode));
  
  /* set PMU basic mode */
  PMU->PWR_MODE.BF.PWR_MODE = pmuMode;

  /* select deepsleep or not */
  if(pmuMode == PMU_PM1)
  {
    SCB->SCR &= !SCB_SCR_SLEEPDEEP_Msk;
  }
  else
  {
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  }
}

/****************************************************************************//**
 * @brief     Get system last sleep mode
 *
 * @param[in]  none
 *
 * @return  rerurn power mode
 *
 *******************************************************************************/
uint32_t PMU_GetSleepMode(void)
{
  return (uint32_t) PMU->PWR_MODE_STATUS.BF.PWR_MODE_STATUS;
}

/****************************************************************************//**
 * @brief      wakeup pin trigger type set
 *
 * @param[in]  wakeuppin: wakeup pin source
 * @param[in]  trigmode: trigger mode selector
 *
 * @return     Return none
 *
 *******************************************************************************/
void PMU_ConfigWakeupPin(PMU_WakeupPin_Type wakeupPin, PMU_WakeupTriggerMode_Type trigmode)
{
  CHECK_PARAM(IS_PMU_WAKEUP_PIN_TYPE(wakeupPin));
  CHECK_PARAM(IS_PMU_WAKEUP_TRIGGER_MODE(trigmode));
  
  switch(wakeupPin)
  {
  /* case GPIO22 interrupt */
  case PMU_GPIO22_INT:
    PMU->WAKEUP_EDGE_DETECT.BF.WAKEUP0 = (trigmode & 0x1);
    break;
    
  /* case GPIO23 interrupt */
  case PMU_GPIO23_INT:
    PMU->WAKEUP_EDGE_DETECT.BF.WAKEUP1 = (trigmode & 0x1);
    break;
 
  /* default */
  default:
    break;
  }
}

/****************************************************************************//**
 * @brief      Get last reset cause
 *
 * @param  none
 *
 * @return     Return last reset cause
 *
 *******************************************************************************/
uint32_t PMU_GetLastResetCause(void)
{
  return (uint32_t) (PMU->LAST_RST_CAUSE.WORDVAL & 0x3F);
}


/****************************************************************************//**
 * @brief      Clear last reset cause
 *
 * @param[in]  resetCause: reset cause to be cleared
 *
 * @return none
 *
 *******************************************************************************/
void PMU_ClrLastResetCause(PMU_LastResetCause_Type resetCause)
{
  switch(resetCause)
  {
  case PMU_BROWNOUT_VBAT:
    PMU->LAST_RST_CLR.BF.BROWNOUT_VBAT_CLR = 1;
    break;
    
  case PMU_CM3_SYSRESETREQ:
    PMU->LAST_RST_CLR.BF.CM3_SYSRESETREQ_CLR = 1;
    break;
    
  case PMU_CM3_LOCKUP:
    PMU->LAST_RST_CLR.BF.CM3_LOCKUP_CLR = 1;
    break;
    
  case PMU_WDTOUT:
    PMU->LAST_RST_CLR.BF.WDT_RST_CLR = 1;
    break;
    
  case PMU_RESETSRC_ALL:
    PMU->LAST_RST_CLR.WORDVAL |= PMU_LAST_RST_CAUSE_ALL;
    break;
    
  default:
    break;    
  }
}

/****************************************************************************//**
 * @brief      wakeup source interrupt requeset clear
 *
 * @param[in]  wakeupSrc: wakeup source to be cleared
 *
 * @return     Return none
 *
 *******************************************************************************/
void PMU_ClearWakeupSrcInt(PMU_WakeupSource_Type wakeupSrc)
{
  CHECK_PARAM(IS_PMU_WAKEUP_SOURCE_TYPE(wakeupSrc));
  
  switch(wakeupSrc)
  {
    /* case pin0: GPIO22 interrupt */
  case PMU_WAKEUP_PIN0:
    PMU->WAKE_SRC_CLR.BF.CLR_PIN0_INT = 1;
    break;
  
    /* case pin1: GPIO23 interrupt */
  case PMU_WAKEUP_PIN1:
    PMU->WAKE_SRC_CLR.BF.CLR_PIN1_INT = 1;
    break;
    
    /* case WLAN interrupt */
  case PMU_WAKEUP_WLAN:
    PMU->WAKE_SRC_CLR.BF.CLR_WL_INT = 1;
    break;
    
    /* case RTC interrupt */
  case PMU_WAKEUP_RTC:
    PMU->WAKE_SRC_CLR.BF.CLR_RTC_INT = 1;
    break;
    
    /* case ULPCOMP interrupt */
  case PMU_WAKEUP_ULPCOMP:
    PMU->WAKE_SRC_CLR.BF.CLR_COMP_INT = 1;
    break;
  
  /* default */
  default:
    break;  
  }
}

/****************************************************************************//**
 * @brief      Get last wakeup status: which source caused the last wakeup
 *
 * @param[in] wakeupSrc: wakeup source to be select
 *
 * @return    Return wakeup status
 *
 *******************************************************************************/
FlagStatus PMU_GetLastWakeupStatus(PMU_WakeupSource_Type wakeupSrc)
{
  FlagStatus bitStatus = RESET;
  
  CHECK_PARAM(IS_PMU_WAKEUP_SOURCE_TYPE(wakeupSrc));
  
  switch(wakeupSrc)
  {
  case PMU_WAKEUP_PIN0:
    bitStatus = (PMU->WAKEUP_STATUS.BF.PIN0_WAKEUP_STATUS? SET : RESET);
    break;
    
  case PMU_WAKEUP_PIN1:
    bitStatus = (PMU->WAKEUP_STATUS.BF.PIN1_WAKEUP_STATUS? SET : RESET);
    break;
    
  case PMU_WAKEUP_WLAN:
    bitStatus = (PMU->WAKEUP_STATUS.BF.WLINT_WAKEUP_STATUS? SET : RESET);
    break;
    
  case PMU_WAKEUP_RTC:
    bitStatus = (PMU->WAKEUP_STATUS.BF.RTC_WAKEUP_STATUS? SET : RESET);
    break;
    
  case PMU_WAKEUP_ULPCOMP:
    bitStatus = (PMU->WAKEUP_STATUS.BF.PMIP_COMP_WAKEUP_STATUS? SET : RESET);
    break;
    
  default:
    break;
  }
  
  return bitStatus;
  
}

/****************************************************************************//**
 * @brief      MASK/UNMASK specified interrupt
 *
 * @param[in] wakeupSrc: wakeup source to be select
 *
 * @return    Return wakeup status
 *
 *******************************************************************************/
void PMU_WakeupSrcIntMask(PMU_WakeupSource_Type wakeupSrc, IntMask_Type intMask)
{
  CHECK_PARAM(IS_PMU_WAKEUP_SOURCE_TYPE(wakeupSrc));
  CHECK_PARAM(IS_INTMASK(intMask));
  
  switch(wakeupSrc)
  {
  case PMU_WAKEUP_PIN0:
    PMU->WAKEUP_MASK.BF.PIN0_WAKEUP_MASK = ((intMask == MASK)? 0 : 1);
    break;
    
  case PMU_WAKEUP_PIN1:
    PMU->WAKEUP_MASK.BF.PIN1_WAKEUP_MASK = ((intMask == MASK)? 0 : 1);
    break;
    
  case PMU_WAKEUP_RTC:
    PMU->WAKEUP_MASK.BF.RTC_WAKEUP_MASK = ((intMask == MASK)? 0 : 1);
    break;
    
  case PMU_WAKEUP_ULPCOMP:
    PMU->WAKEUP_MASK.BF.PMIP_COMP_WAKEUP_MASK = ((intMask == MASK)? 0 : 1);
    break;
    
  case PMU_WAKEUP_WLAN:
    PMU->WAKEUP_MASK.BF.WL_WAKEUP_MASK = ((intMask == MASK)? 0 : 1);
    break;
    
  default:
    break;
  }
}

/****************************************************************************//**
 * @brief      Power on specified VDDIO domain
 *
 * @param[in]  domain: VDDIO domain to be awitched on
 *
 * @return none
 *
 *******************************************************************************/
void PMU_PowerOnVDDIO(PMU_VDDIODOMAIN_Type domain)
{
  CHECK_PARAM(IS_PMU_VDDIO_DOMAIN_TYPE(domain));
  
  switch(domain)
  {
  case PMU_VDDIO_0:
    PMU->IO_PAD_PWR_CFG.BF.GPIO0_PDB = 1;
    PMU->IO_PAD_PWR_CFG.BF.GPIO0_LOW_VDDB = 1;
    break;
    
  case PMU_VDDIO_1:
    PMU->IO_PAD_PWR_CFG.BF.GPIO1_PDB = 1;
    PMU->IO_PAD_PWR_CFG.BF.GPIO1_LOW_VDDB = 1;
    break;
    
  case PMU_VDDIO_AON:
    PMU->IO_PAD_PWR_CFG.BF.GPIO_AON_PDB = 1;
    PMU->IO_PAD_PWR_CFG.BF.GPIO_AON_LOW_VDDB = 1;
    break;
    
  case PMU_VDDIO_2:
    PMU->IO_PAD_PWR_CFG.BF.GPIO2_PDB = 1;
    PMU->IO_PAD_PWR_CFG.BF.GPIO2_LOW_VDDB = 1;
    break;    
    
  case PMU_VDDIO_3:
    PMU->IO_PAD_PWR_CFG.BF.GPIO3_PDB = 1;
    PMU->IO_PAD_PWR_CFG.BF.GPIO3_LOW_VDDB = 1;
    break;    
    
  default:
    break;
  }
}

/****************************************************************************//**
 * @brief      Power off specified VDDIO domain
 *
 * @param[in]  domain: VDDIO domain to be awitched on
 *
 * @return none
 *
 *******************************************************************************/
void PMU_PowerOffVDDIO(PMU_VDDIODOMAIN_Type domain)
{
  CHECK_PARAM(IS_PMU_VDDIO_DOMAIN_TYPE(domain));
  
  switch(domain)
  {
  case PMU_VDDIO_0:
    PMU->IO_PAD_PWR_CFG.BF.GPIO0_LOW_VDDB = 0;
    break;
    
  case PMU_VDDIO_1:
    PMU->IO_PAD_PWR_CFG.BF.GPIO1_LOW_VDDB = 0;
    break;
    
  case PMU_VDDIO_AON:
    PMU->IO_PAD_PWR_CFG.BF.GPIO_AON_LOW_VDDB = 0;
    break;
    
  case PMU_VDDIO_2:
    PMU->IO_PAD_PWR_CFG.BF.GPIO2_LOW_VDDB = 0;
    break;    
    
  case PMU_VDDIO_3:
    PMU->IO_PAD_PWR_CFG.BF.GPIO3_LOW_VDDB = 0;
    break;   
    
  default:
    break;
  }
}

/****************************************************************************//**
* @brief      Set the pad of specified VDDIO domain to power down mode
 *
 * @param[in]  domain: VDDIO domain to be powered down
 *
 * @return none
 *
 *******************************************************************************/
void PMU_PowerDownVDDIO(PMU_VDDIODOMAIN_Type domain)
{
  CHECK_PARAM(IS_PMU_VDDIO_DOMAIN_TYPE(domain));
  
  switch(domain)
  {
  case PMU_VDDIO_0:
    PMU->IO_PAD_PWR_CFG.BF.GPIO0_PDB = 0;
    break;
    
  case PMU_VDDIO_1:
    PMU->IO_PAD_PWR_CFG.BF.GPIO1_PDB = 0;
    break;
    
  case PMU_VDDIO_AON:
    PMU->IO_PAD_PWR_CFG.BF.GPIO_AON_PDB = 0;
    break;
    
  case PMU_VDDIO_2:
    PMU->IO_PAD_PWR_CFG.BF.GPIO2_PDB = 0;
    break;
    
  case PMU_VDDIO_3:
    PMU->IO_PAD_PWR_CFG.BF.GPIO3_PDB = 0;
    break;    
    
  default:
    break;
  }
}

/****************************************************************************//**
 * @brief      Set the pad of specified VDDIO domain to normal mode
 *
 * @param[in]  domain: VDDIO domain to be set to normal mode
 *
 * @return none
 *
 *******************************************************************************/
void PMU_PowerNormalVDDIO(PMU_VDDIODOMAIN_Type domain)
{
  CHECK_PARAM(IS_PMU_VDDIO_DOMAIN_TYPE(domain));
  
  switch(domain)
  {
  case PMU_VDDIO_0:
    PMU->IO_PAD_PWR_CFG.BF.GPIO0_PDB = 1;
    break;
    
  case PMU_VDDIO_1:
    PMU->IO_PAD_PWR_CFG.BF.GPIO1_PDB = 1;
    break;
    
  case PMU_VDDIO_AON:
    PMU->IO_PAD_PWR_CFG.BF.GPIO_AON_PDB = 1;
    break;
    
  case PMU_VDDIO_2:
    PMU->IO_PAD_PWR_CFG.BF.GPIO2_PDB = 1;
    break;
    
  case PMU_VDDIO_3:
    PMU->IO_PAD_PWR_CFG.BF.GPIO3_PDB = 1;
    break;    
    
  default:
    break;
  }
}

/****************************************************************************//**
 * @brief      Configure power domain level
 *
 * @param[in]  domain: VDDIO domain to be configured
 * @param[in]  level: VDDIO level
 *
 * @return none
 *
 *******************************************************************************/
void PMU_ConfigVDDIOLevel(PMU_VDDIODOMAIN_Type domain, PMU_VDDIOLEVEL_Type level)
{
  CHECK_PARAM(IS_PMU_VDDIO_DOMAIN_TYPE(domain));
  CHECK_PARAM(IS_PMU_VDDIO_LEVEL_TYPE(level));
  
  switch(domain)
  {
  case PMU_VDDIO_0:
    PMU->IO_PAD_PWR_CFG.BF.GPIO0_V18 = (level & 0x1);
    PMU->IO_PAD_PWR_CFG.BF.GPIO0_V25 = ((level >> 1) & 0x1);
    break;
    
  case PMU_VDDIO_1:
    PMU->IO_PAD_PWR_CFG.BF.GPIO1_V18 = (level & 0x1);
    PMU->IO_PAD_PWR_CFG.BF.GPIO1_V25 = ((level >> 1) & 0x1);
    break;
    
  case PMU_VDDIO_AON:
    PMU->IO_PAD_PWR_CFG.BF.GPIO_AON_V18 = (level & 0x1);
    PMU->IO_PAD_PWR_CFG.BF.GPIO_AON_V25 = ((level >> 1) & 0x1);
    break;
    
  case PMU_VDDIO_2:
    PMU->IO_PAD_PWR_CFG.BF.GPIO2_V18 = (level & 0x1);
    PMU->IO_PAD_PWR_CFG.BF.GPIO2_V25 = ((level >> 1) & 0x1);
    break;
    
  case PMU_VDDIO_3:
    PMU->IO_PAD_PWR_CFG.BF.GPIO3_V18 = (level & 0x1);
    PMU->IO_PAD_PWR_CFG.BF.GPIO3_V25 = ((level >> 1) & 0x1);
    break;   
    
  default:
    break;
  }
}

/****************************************************************************//**
 * @brief      Configure extra interrupt
 *
 * @param[in]  gpioPin: gpio pin to be configured
 *
 * @return none
 *
 *******************************************************************************/
void PMU_ConfigExtraInterrupt(PMU_EXTRAINT_Type gpioPin)
{
  switch(gpioPin)
  {
  case PMU_INT34_GPIO_0:
    PMU->EXT_SEL_REG0.BF.SEL_34 = 1;
    break;
    
  case PMU_INT34_GPIO_1:
    PMU->EXT_SEL_REG0.BF.SEL_34 = 0;
    break;
    
  case PMU_INT35_GPIO_2:
    PMU->EXT_SEL_REG0.BF.SEL_35 = 1;
    break;
    
  case PMU_INT35_GPIO_3:
    PMU->EXT_SEL_REG0.BF.SEL_35 = 0;
    break;
    
  case PMU_INT36_GPIO_4:
    PMU->EXT_SEL_REG0.BF.SEL_36 = 1;
    break;
    
  case PMU_INT36_GPIO_5:
    PMU->EXT_SEL_REG0.BF.SEL_36 = 0;
    break;
    
  case PMU_INT37_GPIO_6:
    PMU->EXT_SEL_REG0.BF.SEL_37 = 1;
    break;
    
  case PMU_INT37_GPIO_7:
    PMU->EXT_SEL_REG0.BF.SEL_37 = 0;
    break; 
    
  case PMU_INT38_GPIO_8:
    PMU->EXT_SEL_REG0.BF.SEL_38 = 1;
    break;
    
  case PMU_INT38_GPIO_9:
    PMU->EXT_SEL_REG0.BF.SEL_38 = 0;
    break; 
    
  case PMU_INT39_GPIO_10:
    PMU->EXT_SEL_REG0.BF.SEL_39 = 1;
    break;
    
  case PMU_INT39_GPIO_11:
    PMU->EXT_SEL_REG0.BF.SEL_39 = 0;
    break;    
   
  case PMU_INT40_GPIO_12:
    PMU->EXT_SEL_REG0.BF.SEL_40 = 1;
    break;
    
  case PMU_INT40_GPIO_13:
    PMU->EXT_SEL_REG0.BF.SEL_40 = 0;
    break;     

  case PMU_INT41_GPIO_14:
    PMU->EXT_SEL_REG0.BF.SEL_41 = 1;
    break;
    
  case PMU_INT41_GPIO_15:
    PMU->EXT_SEL_REG0.BF.SEL_41 = 0;
    break;

  case PMU_INT42_GPIO_16:
    PMU->EXT_SEL_REG0.BF.SEL_42 = 1;
    break;
    
  case PMU_INT42_GPIO_17:
    PMU->EXT_SEL_REG0.BF.SEL_42 = 0;
    break;

  case PMU_INT43_GPIO_18:
    PMU->EXT_SEL_REG0.BF.SEL_43 = 1;
    break;
    
  case PMU_INT43_GPIO_19:
    PMU->EXT_SEL_REG0.BF.SEL_43 = 0;
    break;       
    
  case PMU_INT44_GPIO_20:
    PMU->EXT_SEL_REG0.BF.SEL_44 = 1;
    break;
    
  case PMU_INT44_GPIO_21:
    PMU->EXT_SEL_REG0.BF.SEL_44 = 0;
    break;    
    
  case PMU_INT45_GPIO_22:
    PMU->EXT_SEL_REG0.BF.SEL_45 = 1;
    break;
    
  case PMU_INT45_GPIO_23:
    PMU->EXT_SEL_REG0.BF.SEL_45 = 0;
    break;
    
  case PMU_INT46_GPIO_24:
    PMU->EXT_SEL_REG0.BF.SEL_46 = 1;
    break;
    
  case PMU_INT46_GPIO_25:
    PMU->EXT_SEL_REG0.BF.SEL_46 = 0;
    break;    

  case PMU_INT47_GPIO_26:
    PMU->EXT_SEL_REG0.BF.SEL_47 = 1;
    break;
    
  case PMU_INT47_GPIO_27:
    PMU->EXT_SEL_REG0.BF.SEL_47 = 0;
    break;

  case PMU_INT48_GPIO_28:
    PMU->EXT_SEL_REG0.BF.SEL_48 = 1;
    break;
    
  case PMU_INT48_GPIO_29:
    PMU->EXT_SEL_REG0.BF.SEL_48 = 0;
    break;
    
  case PMU_INT49_GPIO_30:
    PMU->EXT_SEL_REG0.BF.SEL_49 = 1;
    break; 
    
  case PMU_INT49_GPIO_31:
    PMU->EXT_SEL_REG0.BF.SEL_49 = 0;
    break;
 
  case PMU_INT50_GPIO_32:
    PMU->EXT_SEL_REG0.BF.SEL_50 = 1;
    break;
    
  case PMU_INT50_GPIO_33:
    PMU->EXT_SEL_REG0.BF.SEL_50 = 0;
    break; 
    
  case PMU_INT51_GPIO_34:
    PMU->EXT_SEL_REG0.BF.SEL_51 = 1;
    break;
    
  case PMU_INT51_GPIO_35:
    PMU->EXT_SEL_REG0.BF.SEL_51 = 0;
    break;
    
  case PMU_INT52_GPIO_36:
    PMU->EXT_SEL_REG0.BF.SEL_52 = 1;
    break;
    
  case PMU_INT52_GPIO_37:
    PMU->EXT_SEL_REG0.BF.SEL_52 = 0;
    break;
    
  case PMU_INT53_GPIO_38:
    PMU->EXT_SEL_REG0.BF.SEL_53 = 1;
    break;
    
  case PMU_INT53_GPIO_39:
    PMU->EXT_SEL_REG0.BF.SEL_53 = 0;
    break;
    
  case PMU_INT54_GPIO_40:
    PMU->EXT_SEL_REG0.BF.SEL_54 = 1;
    break;
    
  case PMU_INT54_GPIO_41:
    PMU->EXT_SEL_REG0.BF.SEL_54 = 0;
    break;
    
  case PMU_INT55_GPIO_42:
    PMU->EXT_SEL_REG0.BF.SEL_55 = 1;
    break;
    
  case PMU_INT55_GPIO_43:
    PMU->EXT_SEL_REG0.BF.SEL_55 = 0;
    break; 
    
  case PMU_INT56_GPIO_44:
    PMU->EXT_SEL_REG0.BF.SEL_56 = 1;
    break;
    
  case PMU_INT56_GPIO_45:
    PMU->EXT_SEL_REG0.BF.SEL_56 = 0;
    break; 
    
  case PMU_INT57_GPIO_46:
    PMU->EXT_SEL_REG0.BF.SEL_57 = 1;
    break;
    
  case PMU_INT57_GPIO_47:
    PMU->EXT_SEL_REG0.BF.SEL_57 = 0;
    break;
    
  case PMU_INT58_GPIO_48:
    PMU->EXT_SEL_REG0.BF.SEL_58 = 1;
    break;
    
  case PMU_INT58_GPIO_49:
    PMU->EXT_SEL_REG0.BF.SEL_58 = 0;
    break;

  default:
    break;
  }
}

/****************************************************************************//**
 * @brief      Select ulpcomp operation mode --- single-ended / differential
 *
* @param[in]  mode: select the mode
 *
 * @return none
 *
 *******************************************************************************/
void PMU_UlpcompModeSelect(PMU_UlpcompMode_Type ulpcompMode)
{
  CHECK_PARAM(IS_PMU_ULPCOMP_MODE(ulpcompMode));
  
  PMU->PMIP_CMP_CTRL.BF.COMP_DIFF_EN = ulpcompMode;
}

/****************************************************************************//**
 * @brief      Select ulpcomp reference voltage for single-ended mode
 *
* @param[in]  refVolType: select the reference voltage level
 *
 * @return none
 *
 *******************************************************************************/
void PMU_UlpcompRefVoltageSel(PMU_UlpcompRef_Type refVolType)
{
  CHECK_PARAM(IS_PMU_ULPCOMP_REFVOLT_TYPE(refVolType));
  
  PMU->PMIP_CMP_CTRL.BF.COMP_REF_SEL = (refVolType & 0x7);
}

/****************************************************************************//**
 * @brief      Select ulpcomp hysteresis level
 *
* @param[in]  hystType: select the hysteresis level
 *
 * @return none
 *
 *******************************************************************************/
void PMU_UlpcompHysteresisSel(PMU_UlpcompHyst_Type hystType)
{
  CHECK_PARAM(IS_PMU_ULPCOMP_HYST_TYPE(hystType));
  
  PMU->PMIP_CMP_CTRL.BF.COMP_HYST = (hystType & 0x3);
}

/****************************************************************************//**
 * @brief      Enable/Disable ulpcomp
 *
 * @param[in]  state:  enable/disable
 *
 * @return none
 *
 *******************************************************************************/
void PMU_UlpcompCmd(FunctionalState state)
{
  CHECK_PARAM(PARAM_FUNCTIONALSTATE(state));
  
  PMU->PMIP_CMP_CTRL.BF.COMP_EN = ((state  == ENABLE)? 1 : 0);
}

/****************************************************************************//**
 * @brief      Get ulpcomp ready status
 *
 * @param[in]  none
 *
 * @return status
 *
 *******************************************************************************/
FlagStatus PMU_GetUlpcompStatus(void)
{
  uint32_t rdyStatus = 0;
  
  rdyStatus = PMU->PMIP_CMP_CTRL.BF.COMP_RDY;
  
  if(rdyStatus)
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}

/****************************************************************************//**
 * @brief      Get ulpcomp output value
 *
 * @param[in]  none
 *
 * @return ulpcomp output value
 *
 *******************************************************************************/
uint32_t PMU_GetUlpcompOutValue(void)
{
  return (uint32_t) PMU->PMIP_CMP_CTRL.BF.COMP_OUT;
}

/****************************************************************************//**
 * @brief      Initialize vbat brown out detection
 *
* @param[in]  brndetConfig: Pointer to a vbat brndet configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void PMU_ConfigVbatBrndet(PMU_VbatBrndetConfig_Type* brndetConfig)
{
  CHECK_PARAM(IS_PMU_VBAT_BRNDET_TRIG_TYPE(brndetConfig->brnTrigVolt));
  CHECK_PARAM(IS_PMU_VBAT_BRNDET_HYST_TYPE(brndetConfig->brnHyst));
  CHECK_PARAM(IS_PMU_VBAT_BRNDET_FILT_TYPE(brndetConfig->brnFilter));
  
  PMU->PMIP_BRNDET_VBAT.BF.BRNTRIG_VBAT_CNTL = brndetConfig->brnTrigVolt;
  
  PMU->PMIP_BRNDET_VBAT.BF.BRNHYST_VBAT_CNTL = brndetConfig->brnHyst;
  
  PMU->PMIP_BRNDET_VBAT.BF.BRNDET_VBAT_FILT = brndetConfig->brnFilter;
}

/****************************************************************************//**
 * @brief      Enable/Disable vbat brndet
 *
 * @param[in]  state:  enable/disable
 *
 * @return none
 *
 *******************************************************************************/
void PMU_VbatBrndetCmd(FunctionalState state)
{
  CHECK_PARAM(PARAM_FUNCTIONALSTATE(state));
  
  PMU->PMIP_BRNDET_VBAT.BF.BRNDET_VBAT_EN = ((state  == ENABLE)? 1 : 0);
}

/****************************************************************************//**
 * @brief      Enable/Disable vbat brndet reset
 *
 * @param[in]  state:  enable/disable
 *
 * @return none
 *
 *******************************************************************************/
void PMU_VbatBrndetRstCmd(FunctionalState state)
{
  CHECK_PARAM(PARAM_FUNCTIONALSTATE(state));
  
  PMU->PMIP_BRN_CFG.BF.BRNDET_VBAT_RST_EN = ((state  == ENABLE)? 1 : 0);
}

/****************************************************************************//**
 * @brief      Get vbat brndet ready status
 *
 * @param[in]  none
 *
 * @return status
 *
 *******************************************************************************/
FlagStatus PMU_GetVbatBrndetStatus(void)
{
  uint32_t rdyStatus = 0;
  
  rdyStatus = PMU->PMIP_BRNDET_VBAT.BF.BRNDET_VBAT_RDY;
  
  if(rdyStatus)
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}

/****************************************************************************//**
 * @brief      Get vbat brndet output value
 *
 * @param[in]  none
 *
 * @return ulpcomp output value
 *
 *******************************************************************************/
uint32_t PMU_GetVbatBrndetOutValue(void)
{
  return (uint32_t) PMU->PMIP_BRNDET_VBAT.BF.BRNDET_VBAT_OUT;
}

/****************************************************************************//**
 * @brief      Enable/Disable vbat brndet interrupt
 *
 * @param[in]  state:  enable/disable
 *
 * @return none
 *
 *******************************************************************************/
void PMU_VbatBrndetIntCmd(FunctionalState state)
{
  CHECK_PARAM(PARAM_FUNCTIONALSTATE(state));
  
  PMU->PMIP_BRN_INT_SEL.BF.PMIP_BRN_INT_SEL = ((state  == ENABLE)? 1 : 0);
}

/****************************************************************************//**
 * @brief      Power on WLAN
 *
 * @param[in]  none
 *
 * @return none
 *
 *******************************************************************************/
void PMU_PowerOnWLAN(void)
{
  PMU->WLAN_CTRL.BF.PD = 0x1;
}

/****************************************************************************//**
 * @brief      Power down WLAN
 *
 * @param[in]  none
 *
 * @return none
 *
 *******************************************************************************/
void PMU_PowerDownWLAN(void)
{
  PMU->WLAN_CTRL.BF.PD = 0x0;
}

/****************************************************************************//**
 * @brief      Enable MCI wakeup WLAN
 *
 * @param[in]  none
 *
 * @return none
 *
 *******************************************************************************/
void PMU_MCIWakeupWLANEnable(void)
{
  PMU->WLAN_CTRL1.BF.MCI_WL_WAKEUP = 0x1;
}

/****************************************************************************//**
 * @brief      Disable MCI wakeup WLAN
 *
 * @param[in]  none
 *
 * @return none
 *
 *******************************************************************************/
void PMU_MCIWakeupWLANDisable(void)
{
  PMU->WLAN_CTRL1.BF.MCI_WL_WAKEUP = 0x0;
}

/*@} end of group PMU_Public_Functions */
/*@} end of group PMU_definitions */
/*@} end of group MW300_Periph_Driver */

__INTERRUPT__ void ExtPin0_IRQHandler()
{
        /* counter upp interrupt */
        if (intCbfArra[INT_EXTPIN0][0] != NULL) {
                /* call the callback function */
                intCbfArra[INT_EXTPIN0][0]();
        }
        PMU_ClearWakeupSrcInt(PMU_WAKEUP_PIN0);
        NVIC_ClearPendingIRQ(ExtPin0_IRQn);
}

__INTERRUPT__ void ExtPin1_IRQHandler()
{
        /* counter upp interrupt */
        if (intCbfArra[INT_EXTPIN1][0] != NULL) {
                /* call the callback function */
                intCbfArra[INT_EXTPIN1][0]();
        }
        PMU_ClearWakeupSrcInt(PMU_WAKEUP_PIN1);
        NVIC_ClearPendingIRQ(ExtPin1_IRQn);
}

__INTERRUPT__ void WIFIWKUP_IRQHandler()
{
        /* counter upp interrupt */
        if (intCbfArra[INT_WIFIWAKEUP][0] != NULL) {
                /* call the callback function */
                intCbfArra[INT_WIFIWAKEUP][0]();
        }
        PMU_ClearWakeupSrcInt(PMU_WAKEUP_WLAN);
        NVIC_ClearPendingIRQ(WIFIWKUP_IRQn);
}


