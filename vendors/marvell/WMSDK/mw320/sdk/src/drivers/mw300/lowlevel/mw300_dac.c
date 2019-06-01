/** @file     mw300_dac.c
 *
 *  @brief    This file provides DAC functions.
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
#include "mw300_dac.h"
#include "mw300_clock.h"
#include "mw300_bg.h"
/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @defgroup DAC DAC
 *  @brief DAC driver modules
 *  @{
 */

/** @defgroup DAC_Private_Type
 *  @{
 */

/*@} end of group DAC_Private_Type*/


/** @defgroup DAC_Private_Defines
 *  @{
 */



/*@} end of group DAC_Private_Defines */


/** @defgroup DAC_Private_Variables
 *  @{
 */

/*@} end of group DAC_Private_Variables */

/** @defgroup DAC_Global_Variables
 *  @{
 */

/*@} end of group DAC_Global_Variables */


/** @defgroup DAC_Private_FunctionDeclaration
 *  @{
 */


/*@} end of group DAC_Private_FunctionDeclaration */

/** @defgroup DAC_Private_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief  DAC interrupt function 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void DAC_IRQHandler(void)
{
  uint32_t intStatus;
  
  /* Store unmasked interrupt flags for later use */
  intStatus = (~DAC->IMR.WORDVAL) & 0x1F;
  intStatus &= DAC->ISR.WORDVAL;
  
  /* Clear all unmasked interrupt flags */
  DAC->ICR.WORDVAL = intStatus;
  
  /* DAC channel A data ready inerrupt */
  if( intStatus & (1 << DAC_CH_A_RDY) )
  {
    if(intCbfArra[INT_DAC][DAC_CH_A_RDY] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_DAC][DAC_CH_A_RDY]();
    }
    else
    {
      DAC->IMR.BF.A_RDY_INT_MSK = 1;
    }    
  }

  /* DAC channel B data ready inerrupt */
  if( intStatus & (1 << DAC_CH_B_RDY) )
  {
    if(intCbfArra[INT_DAC][DAC_CH_B_RDY] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_DAC][DAC_CH_B_RDY]();
    }
    else
    {
      DAC->IMR.BF.B_RDY_INT_MSK = 1;
    }
  }
  
  /* DAC channel A time out inerrupt */
  if( intStatus & (1 << DAC_CH_A_TO) )
  {
    if(intCbfArra[INT_DAC][DAC_CH_A_TO] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_DAC][DAC_CH_A_TO]();
    }
    else
    {
      DAC->IMR.BF.A_TO_INT_MSK = 1;
    }
  }  
 
  /* DAC channel B time out inerrupt */
  if( intStatus & (1 << DAC_CH_B_TO) )
  {
    if(intCbfArra[INT_DAC][DAC_CH_B_TO] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_DAC][DAC_CH_B_TO]();
    }
    else
    {
      DAC->IMR.BF.B_TO_INT_MSK = 1;
    }
  }  

  /* DAC triangle wave overflow inerrupt */
  if( intStatus & (1 << DAC_TRIA_OVERFLOW) )
  {
    if(intCbfArra[INT_DAC][DAC_TRIA_OVERFLOW] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_DAC][DAC_TRIA_OVERFLOW]();
    }
    else
    {
      DAC->IMR.BF.TRIA_OVFL_INT_MSK = 1;
    }
  }    
}

/*@} end of group DAC_Private_Functions */


/** @defgroup DAC_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      Configure the DAC channels
 *
 * @param[in]  dacChId:  DAC channel ID
 * @param[in]  channelConfig:  channel configuration struct
 *
 * @return none
 *
 *******************************************************************************/
void DAC_ChannelConfig(DAC_ChannelID_Type dacChId, DAC_ChannelConfig_Type* channelConfig)
{
  CHECK_PARAM(IS_DAC_CHANNEL_ID(dacChId));
  
  /* DAC channel A */
  if(dacChId == DAC_CH_A)
  {
    CHECK_PARAM(IS_DAC_CHANNEL_A_WAVE(channelConfig->waveMode));
    
    DAC->ACTRL.BF.A_WAVE = channelConfig->waveMode;
    DAC->ACTRL.BF.A_IO_EN = channelConfig->outMode;
    DAC->ACTRL.BF.A_MODE = channelConfig->timingMode;
  }
  
  /* DAC channel B */
  if(dacChId == DAC_CH_B)
  {
    CHECK_PARAM(IS_DAC_CHANNEL_B_WAVE(channelConfig->waveMode));
        
    DAC->BCTRL.BF.B_WAVE = channelConfig->waveMode;
    DAC->BCTRL.BF.B_IO_EN = channelConfig->outMode;
    DAC->BCTRL.BF.B_MODE = channelConfig->timingMode;
  }  
}


/****************************************************************************//**
 * @brief      Enable the selected DAC channel
 *
 * @param[in]  dacChId:  DAC channel ID
 *
 * @return none
 *
 *******************************************************************************/
void DAC_ChannelEnable(DAC_ChannelID_Type dacChId)
{
  CHECK_PARAM(IS_DAC_CHANNEL_ID(dacChId));
  
  /* DAC channel A */
  if(dacChId == DAC_CH_A)
  {
    DAC->ACTRL.BF.A_EN = 1;
  }
  
  /* DAC channel B */
  if(dacChId == DAC_CH_B)
  {
    DAC->BCTRL.BF.B_EN = 1;
  }

}

/****************************************************************************//**
 * @brief      Disable the selected DAC channel
 *
 * @param[in]  dacChId:  DAC channel ID
 *
 * @return none
 *
 *******************************************************************************/
void DAC_ChannelDisable(DAC_ChannelID_Type dacChId)
{
  CHECK_PARAM(IS_DAC_CHANNEL_ID(dacChId));
  
  /* DAC channel A */
  if(dacChId == DAC_CH_A)
  {
    DAC->ACTRL.BF.A_EN = 0;
  }
  
  /* DAC channel B */
  if(dacChId == DAC_CH_B)
  {
    DAC->BCTRL.BF.B_EN = 0;
  } 
}

/****************************************************************************//**
 * @brief      Reset DAC channel
 *
 * @param[in]  dacChId:  DAC channel ID
 *
 * @return none
 *
 *******************************************************************************/
void DAC_ChannelReset(DAC_ChannelID_Type dacChId)
{
  volatile uint32_t i;

  CHECK_PARAM(IS_DAC_CHANNEL_ID(dacChId));
  
  /* DAC channel A */
  if(dacChId == DAC_CH_A)
  {
    DAC->RST.BF.A_SOFT_RST = 1;
    
    /* Delay */
    for(i=0; i<10; i++);
    
    DAC->RST.BF.A_SOFT_RST = 0;
  }
  
  /* DAC channel B */
  if(dacChId == DAC_CH_B)
  {
    DAC->RST.BF.B_SOFT_RST = 1;
    
    /* Delay */
    for(i=0; i<10; i++);
    
    DAC->RST.BF.B_SOFT_RST = 0;
  } 
}

/****************************************************************************//**
 * @brief      Set DAC DMA mode 
 *
 * @param[in]  dacChId:  DAC channel ID
 * @param[in]  newCmd:  Enable/disable DMA for specified channel
 *
 * @return none
 *
 *******************************************************************************/
void DAC_DmaCmd(DAC_ChannelID_Type dacChId, FunctionalState newCmd)
{
  CHECK_PARAM(IS_DAC_CHANNEL_ID(dacChId));
  
  switch(dacChId)
  {
    /* DAC channel A */
    case DAC_CH_A:
      DAC->ACTRL.BF.A_DEN = ((newCmd == ENABLE) ? 1:0);
      break;
      
    /* DAC channel B */
    case DAC_CH_B:
      DAC->BCTRL.BF.B_DEN = ((newCmd == ENABLE) ? 1:0);
      break;
      
    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      Set DAC channel data
 *
 * @param[in]  dacChId:  DAC channel ID
 * @param[in]  channelData:  channel data
 *
 * @return none
 *
 *******************************************************************************/
void DAC_SetChannelData(DAC_ChannelID_Type dacChId, uint16_t channelData)
{
  CHECK_PARAM(IS_DAC_CHANNEL_ID(dacChId));
  
  /* DAC channel A */
  if(dacChId == DAC_CH_A)
  {
    DAC->ADATA.BF.A_DATA = channelData;
  }
  
  /* DAC channel B */
  if(dacChId == DAC_CH_B)
  {
    DAC->BDATA.BF.B_DATA = channelData;
  } 
}


/****************************************************************************//**
 * @brief      Initializes the DAC with pre-defined DAC configuration structure
 *
 * @param[in]  dacConfigSet:  Pointer to a DAC configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void DAC_Init(DAC_CFG_Type* dacConfigSet)
{
  volatile uint32_t i;
  /* TODO: Test DAC operation after SOFT_CLK_RESET */
  DAC->CLK.BF.SOFT_CLK_RST = 1;
 
  /* Delay */
  for(i=0; i<10; i++);
 
  DAC->CLK.BF.SOFT_CLK_RST = 0;
  CLK_RC32MEnable();

  while (!PMU->CLK_RDY.BF.RC32M_RDY)
	;
  /* CAU clock source = RC32M */
  PMU->CAU_CLK_SEL.BF.CAU_CLK_SEL = 1;

  /* Enable CAU_CLK */
  CLK_ModuleClkEnable(CLK_GAU);

  /* Bandgap power up for DAC */
  /* FIXME: Verify bandgap config */
  BG_PowerUp();

  /* Bandgap clk enable */
  PMU->CAU_CTRL.BF.CAU_BG_MCLK_EN = 0;

  /* Enable DAC clk */
  CLK_ModuleClkEnable(CLK_DAC_MCLK);

  DAC->CLK.BF.CLK_CTRL = dacConfigSet->conversionRate;

  /* Set DAC reference source */
  DAC->CTRL.BF.REF_SEL = dacConfigSet->refSource;
  
  /* Set DAC channel output Range*/
  DAC->ACTRL.BF.A_RANGE = dacConfigSet->rangeSelect;
}

/****************************************************************************//**
 * @brief      Set DAC channel data
 *
 * @param[in]  triangleConfig:  Pointer to a DAC triangle waveform configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void DAC_TriangleConfig(DAC_TriangleConfig_Type* triangleConfig)
{
  /* Set triangle maximum output in lsb */
  DAC->ACTRL.BF.A_TRIA_MAMP_SEL = triangleConfig->triangleMamp;

  /* Set triangle step size in lsb */
  DAC->ACTRL.BF.A_TRIA_STEP_SEL = triangleConfig->triangleStepSize;

  /* Set triangle waveform type is full or half mode */
  DAC->ACTRL.BF.A_TRIA_HALF = triangleConfig->triangleWaveform;
  
  /* Set triangle minimum output in lsb */
  DAC->ADATA.BF.A_DATA = triangleConfig->triangleBase;
}

/****************************************************************************//**
 * @brief      Set DAC trigger source
 *
 * @param[in]  dacChId:  DAC channel ID
 * @param[in]  triggerSource:  DAC trigger source
 *
 * @return none
 *
 *******************************************************************************/
void DAC_TriggerSourceConfig(DAC_ChannelID_Type dacChId, DAC_TriggerSource_Type triggerSource)
{
  CHECK_PARAM(IS_DAC_CHANNEL_ID(dacChId));
  CHECK_PARAM(IS_DAC_TRIGGER_SOURCE(triggerSource));
  
  if(dacChId == DAC_CH_A)
  {
    DAC->ACTRL.BF.A_TRIG_SEL = triggerSource;
  }
  
  if(dacChId == DAC_CH_B)
  {
    DAC->BCTRL.BF.B_TRIG_SEL = triggerSource;
  }
  
}

/****************************************************************************//**
 * @brief      Set DAC GPIO trigger type
 *
 * @param[in]  dacChId:  DAC channel ID
 * @param[in]  triggerType:  DAC GPIO trigger type
 *
 * @return none
 *
 *******************************************************************************/
void DAC_GPIOTriggerTypeConfig(DAC_ChannelID_Type dacChId, DAC_GPIOTrigger_Type triggerType)
{
  CHECK_PARAM(IS_DAC_CHANNEL_ID(dacChId));
  CHECK_PARAM(IS_DAC_GPIO_TRIGGER(triggerType));
  
  if(dacChId == DAC_CH_A)
  {
    DAC->ACTRL.BF.A_TRIG_TYP = triggerType;
  }
  
  if(dacChId == DAC_CH_B)
  {
    DAC->BCTRL.BF.B_TRIG_TYP = triggerType;
  }
}

/****************************************************************************//**
 * @brief      Set DAC trigger cmd
 *
 * @param[in]  dacChId:  DAC channel ID
 * @param[in]  state:  ENABLE/DISABLE DAC trigger
 *
 * @return none
 *
 *******************************************************************************/
void DAC_TriggerCmd(DAC_ChannelID_Type dacChId, FunctionalState newCmd)
{
  CHECK_PARAM(IS_DAC_CHANNEL_ID(dacChId));
  
  if(dacChId == DAC_CH_A)
  {
    DAC->ACTRL.BF.A_TRIG_EN = newCmd;
  }
  
  if(dacChId == DAC_CH_B)
  {
    DAC->BCTRL.BF.B_TRIG_EN = newCmd;
  }
}

/****************************************************************************//**
 * @brief      Get DAC status 
 *
 * @param[in]  statusType:  Specifies the status type
 *
 * @return     State value of the specified DAC status type
 *
 *******************************************************************************/
FlagStatus DAC_GetStatus(DAC_Status_Type status)
{
  FlagStatus bitStatus = RESET;
 
  CHECK_PARAM(IS_DAC_STATUS_TYPE(status));
  
  switch(status)
  {
    /* DAC A ready status*/
    case DAC_STATUS_CH_A_RDY:    
      if (DAC->IRSR.BF.A_RDY_INT_RAW == 1)
      {
        bitStatus = SET;
      }   
      break;

    /* DAC A ready status*/
    case DAC_STATUS_CH_B_RDY:    
      if (DAC->IRSR.BF.B_RDY_INT_RAW == 1)
      {
        bitStatus = SET;
      }   
      break;
      
    /* DAC A time-out status */
    case DAC_STATUS_CH_A_TO:    
      if (DAC->IRSR.BF.A_TO_INT_RAW == 1)
      {
        bitStatus = SET;
      }   
      break;      

    /* DAC B time-out status */
    case DAC_STATUS_CH_B_TO:    
      if (DAC->IRSR.BF.B_TO_INT_RAW == 1)
      {
        bitStatus = SET;
      }   
      break; 
      
    /* DAC triangle overflow status */  
    case DAC_STATUS_TRIA_OVERFLOW:    
      if (DAC->IRSR.BF.TRIA_OVFL_INT_RAW == 1)
      {
        bitStatus = SET;
      }   
      break;       
  
    /* DAC A data valid status */  
    case DAC_STATUS_CH_A_DV:    
      if (DAC->STATUS.BF.A_DV == 1)
      {
        bitStatus = SET;
      }   
      break;      
      
    /* DAC B data valid status */ 
    case DAC_STATUS_CH_B_DV:    
      if (DAC->STATUS.BF.B_DV == 1)
      {
        bitStatus = SET;
      }   
      break;       
      
    default:
      break;
  }

  return bitStatus;
}

/****************************************************************************//**
 * @brief      Get DAC interrupt status 
 *
 * @param[in]  intType:  Specifies the interrupt type
 *
 * @return     The state value of DAC interrupt status register
 *
 *******************************************************************************/
IntStatus DAC_GetIntStatus(DAC_INT_Type intType)
{
  IntStatus bitStatus = RESET;
  
  CHECK_PARAM(IS_DAC_INT_TYPE(intType));
  
  switch(intType)
  {
    /* DAC A ready interrupt */
    case DAC_CH_A_RDY:    
      if (DAC->ISR.BF.A_RDY_INT == 1)
      {
        bitStatus = SET;
      }   
      break;

    /* DAC B ready interrupt */
    case DAC_CH_B_RDY:    
      if (DAC->ISR.BF.B_RDY_INT == 1)
      {
        bitStatus = SET;
      }   
      break;
      
    /* DAC A time-out interrupt */
    case DAC_CH_A_TO:    
      if (DAC->ISR.BF.A_TO_INT == 1)
      {
        bitStatus = SET;
      }   
      break;      
 
    /* DAC B time-out interrupt */
    case DAC_CH_B_TO:    
      if (DAC->ISR.BF.B_TO_INT == 1)
      {
        bitStatus = SET;
      }   
      break;  
     
    /* DAC triangle overflow interrupt */
    case DAC_TRIA_OVERFLOW:    
      if (DAC->ISR.BF.TRIA_OVFL_INT == 1)
      {
        bitStatus = SET;
      }   
      break;     
      
    /* Any DAC interrupt */  
    case DAC_INT_ALL:    
      if (DAC->ISR.WORDVAL & 0x1F)
      {
        bitStatus = SET;
      }   
      break;         
      
    default:
      break;
  }

  return bitStatus;
}

/****************************************************************************//**
 * @brief      Mask/Unmask specified DAC interrupt type 
 *
 * @param[in]  intType:  Specifies the interrupt type
 * @param[in]  intMask:  Mask/Unmask specified interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void DAC_IntMask(DAC_INT_Type intType, IntMask_Type intMask)
{
  uint32_t cmdAll;
  
  CHECK_PARAM(IS_DAC_INT_TYPE(intType));
  CHECK_PARAM(IS_INTMASK(intMask));
              
  if(intMask == UNMASK)
  {
    cmdAll = 0;
  }
  else
  {
    cmdAll = 0x1F;
  }
  
  switch(intType)
  {
    /* DAC A ready interrupt */
    case DAC_CH_A_RDY:
      DAC->IMR.BF.A_RDY_INT_MSK = intMask;
      break;
      
    /* DAC B ready interrupt */
    case DAC_CH_B_RDY:
      DAC->IMR.BF.B_RDY_INT_MSK = intMask;
      break;   
      
    /* DAC A time-out interrupt */
    case DAC_CH_A_TO:
      DAC->IMR.BF.A_TO_INT_MSK = intMask;
      break;      
      
    /* DAC B time-out interrupt */
    case DAC_CH_B_TO:
      DAC->IMR.BF.B_TO_INT_MSK = intMask;
      break;  
      
    /* DAC triangle overflow interrupt */
    case DAC_TRIA_OVERFLOW:
      DAC->IMR.BF.TRIA_OVFL_INT_MSK = intMask;
      break;       
      
    /* All DAC interrupts */  
    case DAC_INT_ALL:
      DAC->IMR.WORDVAL = cmdAll;
      break;        

    default:
      break;
  }
}

 /****************************************************************************//**
 * @brief      Clear DAC interrupt flag 
 *
 * @param[in]  intType:  Specifies the interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void DAC_IntClr(DAC_INT_Type intType)
{
  CHECK_PARAM(IS_DAC_INT_TYPE(intType));
  
  switch(intType)
  {
    /* DAC A ready interrupt */
    case DAC_CH_A_RDY:    
      DAC->ICR.BF.A_RDY_INT_CLR = 1;
      break;
    
    /* DAC B ready interrupt */
    case DAC_CH_B_RDY:    
      DAC->ICR.BF.B_RDY_INT_CLR = 1;
      break;   
      
    /* DAC A time-out interrupt */
    case DAC_CH_A_TO:    
      DAC->ICR.BF.A_TO_INT_CLR = 1;
      break;      

    /* DAC B time-out interrupt */
    case DAC_CH_B_TO:    
      DAC->ICR.BF.B_TO_INT_CLR = 1;
      break;   
      
    /* DAC triangle overflow interrupt */
    case DAC_TRIA_OVERFLOW:    
      DAC->ICR.BF.TRIA_OVFL_INT_CLR = 1;
      break;         

    /* All DAC interrupts */  
    case DAC_INT_ALL:    
      DAC->ICR.WORDVAL = 0x1F;
      break;     
      
    default:
      break;
  }
}

/*@} end of group DAC_Public_Functions */

/*@} end of group DAC_definitions */

/*@} end of group MW300_Periph_Driver */

