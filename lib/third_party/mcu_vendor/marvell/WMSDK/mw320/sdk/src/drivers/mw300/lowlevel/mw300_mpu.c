/** @file     mw300_mpu.c
 *
 *  @brief    This file provides MPU functions.
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
#include "mw300_mpu.h"
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @defgroup MPU MPU
 *  @brief MPU driver modules
 *  @{
 */

/** @defgroup MPU_Private_Type
 *  @{
 */

/*@} end of group MPU_Private_Type*/

/** @defgroup MPU_Private_Defines
 *  @{
 */

/*@} end of group MPU_Private_Defines */


/** @defgroup MPU_Private_Variables
 *  @{
 */


/*@} end of group MPU_Private_Variables */

/** @defgroup MPU_Global_Variables
 *  @{
 */


/*@} end of group MPU_Global_Variables */


/** @defgroup MPU_Private_FunctionDeclaration
 *  @{
 */


/*@} end of group MPU_Private_FunctionDeclaration */

/** @defgroup MPU_Private_Functions
 *  @{
 */


/*@} end of group MPU_Private_Functions */

/** @defgroup MPU_Public_Functions
 *  @{
 */

 
/****************************************************************************//**
 * @brief      Enable the MPU 
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_Enable(void)
{
  MPU->CTRL |= 0x01;
}

/****************************************************************************//**
 * @brief      Disable MPU 
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_Disable(void)
{
  MPU->CTRL &= 0xfffffffe;
}
/****************************************************************************//**
 * @brief      Enable the MPU region
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_RegionEnable(MPU_RegionNo_Type regionNo)
{
  /* select region */
  MPU->RBAR &= (~(1<<4));
  MPU->RNR = regionNo; 
  
  MPU->RASR |= 0x01;
}

/****************************************************************************//**
 * @brief      Disable the MPU region
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_RegionDisable(MPU_RegionNo_Type regionNo)
{
  /* select region */
  MPU->RBAR &= (~(1<<4));
  MPU->RNR = regionNo; 
  
  MPU->RASR &= (~0x01);
}

/****************************************************************************//**
 * @brief      Disable the MPU subregion
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_SubregionDisable(MPU_RegionNo_Type regionNo, MPU_SubregionNo_Type subregionNo)
{
  /* select region */
  MPU->RBAR &= (~(1<<4));
  MPU->RNR = regionNo; 
  
  MPU->RASR |= (subregionNo << 8);
}


/****************************************************************************//**
 * @brief      Enable the MPU subregion
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_SubregionEnable(MPU_RegionNo_Type regionNo, MPU_SubregionNo_Type subregionNo)
{
  /* select region */
  MPU->RBAR &= (~(1<<4));
  MPU->RNR |= regionNo; 
  
  MPU->RASR &= (~(subregionNo << 8));
}
/****************************************************************************//**
 * @brief      Initialize the MPU 
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_Init(MPU_Config_Type* mpuConfig)
{
  if(ENABLE == mpuConfig->hardFaultNMIEnable)
  {
    MPU->CTRL |= 0x02;
  }
  if(DISABLE == mpuConfig->hardFaultNMIEnable)
  {
    MPU->CTRL &= 0xfffffffd;
  }
  
  if(ENABLE == mpuConfig->privDefMemEnable)
  {
    MPU->CTRL |= 0x04;
  }
  if(DISABLE == mpuConfig->privDefMemEnable)
  {
    MPU->CTRL &= 0xfffffffb;
  }
  
  
}



/****************************************************************************//**
 * @brief      Initialize the MPU 
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_RegionConfig(MPU_RegionNo_Type regionNo, MPU_RegionConfig_Type* regionConfig)
{
  /* select region */
  MPU->RBAR &= (~(1<<4));
  MPU->RNR = regionNo; 
  
  /* set base address */
  MPU->RBAR &= 0x1f;
  regionConfig->baseAddress &= (~0x1f);  
  MPU->RBAR |= regionConfig->baseAddress;
  
  /* set region size */
  MPU->RASR &= (~0x3e);
  MPU->RASR |= ((regionConfig->size) << 1);
  
  /* set data access permission */
  MPU->RASR &= (~(7 << 24));
  MPU->RASR |= ((regionConfig->accessPermission) << 24);
  
  /* instruction access */
  if(ENABLE == regionConfig->instructionAccess)
  {
    MPU->RASR &= (~(1 <<28));
  }
  
  if(DISABLE == regionConfig->instructionAccess)
  {
    MPU->RASR |= (1 <<28);
  }
}

/****************************************************************************//**
 * @brief      Enable the MPU interrupt
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_IntEnable(void)
{
  SCB->SHCSR |= 0x00010000;
}

/****************************************************************************//**
 * @brief      Disable the MPU interrupt
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MPU_IntDisable(void)
{
  SCB->SHCSR &= (~(0x00010000));
}

/****************************************************************************//**
 * @brief       MPU interrupt handler
 *
 * @param[in]  None
 *
 * @return None
 *  
 *******************************************************************************/
void MemManageException(void)
{
  uint32_t intStatus;  
  
  /* keep a copy of current interrupt status */
  intStatus = SCB->CFSR;
  intStatus &= 0xff;
  
  /* clear the generated interrupts */
  SCB->CFSR = intStatus;

  /* stacking error interrupt */
  if((intStatus & (0x00000010))!= 0x00)
  {
    if(intCbfArra[INT_MPU][MPU_INT_STACKING_ERR] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_MPU][MPU_INT_STACKING_ERR]();
    }
    else
    {
      SCB->SHCSR &= (~(1 << 16));
    }
  }
 
  /* unstacking interrupt */
  if((intStatus & (0x00000008))!= 0x00)
  {
    if(intCbfArra[INT_MPU][MPU_INT_UNSTACKING_ERR] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_MPU][MPU_INT_UNSTACKING_ERR]();
    }
    else
    {
      SCB->SHCSR &= (~(1 << 16));
    }    
  }  

  /* data access violation interrupt */
  if((intStatus & (0x00000002))!= 0x00)
  {
    if(intCbfArra[INT_MPU][MPU_INT_DATA_ACCESS_VIOL] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_MPU][MPU_INT_DATA_ACCESS_VIOL]();
    }
    else
    {
      SCB->SHCSR &= (~(1 << 16));
    }    
  }  
  
  /* instruction access violation interrupt */
  if((intStatus & (0x00000001))!= 0x00)
  {
    if(intCbfArra[INT_MPU][MPU_INT_INSTRUCION_ACCESS_VIOL] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_MPU][MPU_INT_INSTRUCION_ACCESS_VIOL]();
    }  
    else
    {
      SCB->SHCSR &= (~(1 << 16));
    }    
  } 
}
/*@} end of group MPU_Public_Functions */

/*@} end of group MPU  */

/*@} end of group MW300_Periph_Driver */

