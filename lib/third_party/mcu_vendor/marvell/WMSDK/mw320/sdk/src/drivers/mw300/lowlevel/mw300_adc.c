/** @file mw300_adc.c
 *
 *  @brief This file provides ADC functions.
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
#include "mw300_adc.h"
#include "mw300_clock.h"
#include "mw300_bg.h"
/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @defgroup ADC ADC
 *  @brief ADC driver modules
 *  @{
 */

/** @defgroup ADC_Private_Type
 *  @{
 */
static const uint32_t adcAddr[2] = {ADC0_BASE};

/*@} end of group ADC_Private_Type*/

/** @defgroup ADC_Private_Defines
 *  @{
 */

/*@} end of group ADC_Private_Defines */

/** @defgroup ADC_Private_Variables
 *  @{
 */

/*@} end of group ADC_Private_Variables */

/** @defgroup ADC_Global_Variables
 *  @{
 */

/*@} end of group ADC_Global_Variables */

/** @defgroup ADC_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group ADC_Private_FunctionDeclaration */

/** @defgroup ADC_Private_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief  ADC interrupt function
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  intPeriph:  select the interrupt peripheral
 *
 * @return none
 *
 *******************************************************************************/
static void ADC_IRQHandler(ADC_ID_Type adcID, INT_Peripher_Type intPeriph)
{
  uint32_t intStatus;
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  /* store unmasked interrupt flags for later use */
  intStatus = (~ADCx->IMR.WORDVAL) & 0x7F;
  intStatus &= ADCx->ISR.WORDVAL;
  
  /* clear all unmasked interrupt flags */
  ADCx->ICR.WORDVAL = intStatus;
  
  /* Conversion data ready interrupt */
  if( intStatus & (1 << ADC_RDY) )
  {
    if(intCbfArra[intPeriph][ADC_RDY] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][ADC_RDY]();
    }
    else
    {
      ADCx->IMR.BF.RDY_MASK = 1;
    }
  }
  
  /* Gain correction saturation interrupt */
  if( intStatus & (1 << ADC_GAINSAT) )
  {
    if(intCbfArra[intPeriph][ADC_GAINSAT] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][ADC_GAINSAT]();
    }
    else
    {
      ADCx->IMR.BF.GAINSAT_MASK = 1;
    }
  }
  
  /* Offset correction saturation interrupt */
  if( intStatus & (1 << ADC_OFFSAT) )
  {
    if(intCbfArra[intPeriph][ADC_OFFSAT] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][ADC_OFFSAT]();
    }
    else
    {
      ADCx->IMR.BF.OFFSAT_MASK = 1;
    }
  }
  
  /* Data negative saturation interrupt */
  if( intStatus & (1 << ADC_DATASAT_NEG) )
  {
    if(intCbfArra[intPeriph][ADC_DATASAT_NEG] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][ADC_DATASAT_NEG]();
    }
    else
    {
      ADCx->IMR.BF.DATASAT_NEG_MASK = 1;
    }
  }
  
  /* Data positive saturation interrupt */
  if( intStatus & (1 << ADC_DATASAT_POS) )
  {
    if(intCbfArra[intPeriph][ADC_DATASAT_POS] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][ADC_DATASAT_POS]();
    }
    else
    {
      ADCx->IMR.BF.DATASAT_POS_MASK = 1;
    }
  }
  
  /* FIFO overrun interrupt */
  if( intStatus & (1 << ADC_FIFO_OVERRUN) )
  {
    if(intCbfArra[intPeriph][ADC_FIFO_OVERRUN] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][ADC_FIFO_OVERRUN]();
    }
    else
    {
      ADCx->IMR.BF.FIFO_OVERRUN_MASK = 1;
    } 
  }
  
  /* FIFO underrun interrupt */
  if( intStatus & (1 << ADC_FIFO_UNDERRUN) )
  {
    if(intCbfArra[intPeriph][ADC_FIFO_UNDERRUN] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][ADC_FIFO_UNDERRUN]();
    }
    else
    {
      ADCx->IMR.BF.FIFO_UNDERRUN_MASK = 1;
    } 
  }
}

/*@} end of group ADC_Private_Functions */

/** @defgroup ADC_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      Initialize the ADC with pre-defined ADC configuration structure
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  adcConfigSet:  the ADC configuration struct pointer
 *
 * @return none
 *
 *******************************************************************************/
void ADC_Init(ADC_ID_Type adcID, ADC_CFG_Type* adcConfigSet)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  CLK_RC32MEnable();

  while (!PMU->CLK_RDY.BF.RC32M_RDY)
	;
  /* CAU clock source = RC32M */
  /* FIXME: CLK_SRC may be configurable */
  PMU->CAU_CLK_SEL.BF.CAU_CLK_SEL = 1;

  /* Enable CAU_CLK */
  CLK_ModuleClkEnable(CLK_GAU);

  /* Bandgap power up for ADC */
  /* FIXME: Verify bandgap config */
  BG_PowerUp();
  /* Bandgap clk enable */
  PMU->CAU_CTRL.BF.CAU_BG_MCLK_EN = 0;

  /* Enable ADC clk */
  CLK_ModuleClkEnable(CLK_ADC);
  /* FIXME: Enable only required module */
  CLK_ModuleClkEnable(CLK_ADC0_MCLK);

  ADCx->ANA.BF.RES_SEL = adcConfigSet->adcResolution;
  ADCx->ANA.BF.VREF_SEL = adcConfigSet->adcVrefSource; 
  ADCx->ANA.BF.INBUF_GAIN = adcConfigSet->adcGainSel;
  ADCx->GENERAL.BF.CLK_DIV_RATIO = adcConfigSet->adcClockDivider;
}

/****************************************************************************//**
* @brief      Select the ADC operation mode --- ADC / Tsensor
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  adcMode: select the ADC operation mode
 *
 * @return     none
 *
 *******************************************************************************/
void ADC_ModeSelect(ADC_ID_Type adcID, ADC_Mode_Type adcMode)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->ANA.BF.TS_EN = adcMode;
}

/****************************************************************************//**
 * @brief      Select the ADC input mode --- Single-ended / Differential
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  adcInputMode:  select the ADC input mode
 *
 * @return none
 *
 *******************************************************************************/
void ADC_InputModeSelect(ADC_ID_Type adcID, ADC_InputMode_Type adcInputMode)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->ANA.BF.SINGLEDIFF = adcInputMode;
}

/****************************************************************************//**
 * @brief      Select the ADC conversion mode --- One shot / Continuous
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  adcConversionMode:  select the ADC conversion mode
 *
 * @return none
 *
 *******************************************************************************/
void ADC_ConversionModeSelect(ADC_ID_Type adcID, ADC_ConversionMode_Type adcConversionMode)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->CONFIG.BF.CONT_CONV_EN = adcConversionMode;
}

/****************************************************************************//**
 * @brief      Set the ADC scan length
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  adcScanLengthType:  select the ADC scan length
 *
 * @return none
 *
 *******************************************************************************/
void ADC_SetScanLength(ADC_ID_Type adcID, ADC_ScanLength_Type adcScanLength)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->CONFIG.BF.SCAN_LENGTH = adcScanLength;
}

/****************************************************************************//**
 * @brief      Set the ADC scan channel
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  adcScanChannel:  select the ADC scan channel
 * @param[in]  adcChannelSource:  select the ADC channel source
 *
 * @return none
 *
 * Set the ADC channel
 *******************************************************************************/
void ADC_SetScanChannel(ADC_ID_Type adcID, ADC_ScanChannel_Type adcScanChannel, ADC_ChannelSource_Type adcChannelSource)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  if(adcScanChannel < 8)
  {
    ADCx->SCN1.WORDVAL &= ~(0xF << adcScanChannel);
    ADCx->SCN1.WORDVAL |= (adcChannelSource << adcScanChannel);
  }
  else
  {
    ADCx->SCN2.WORDVAL &= ~(0xF << (adcScanChannel-8));
    ADCx->SCN2.WORDVAL |= (adcChannelSource << (adcScanChannel-8));
  }
}

/****************************************************************************//**
 * @brief      Set the ADC buffer mode
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  adcInputBuf:  enable or disable the ADC input gain buffer
 *
 * @return none
 *
 *******************************************************************************/
void ADC_SetBufferMode(ADC_ID_Type adcID, FunctionalState adcInputBuf)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->ANA.BF.INBUF_EN = adcInputBuf;
}

/****************************************************************************//**
 * @brief      Set the ADC average length
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  adcAverageLength:  select the ADC average length
 *
 * @return none
 *
 *******************************************************************************/
void ADC_SetAverageLength(ADC_ID_Type adcID, ADC_AverageLength_Type adcAverageLength)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->CONFIG.BF.AVG_SEL = adcAverageLength;
}

/****************************************************************************//**
 * @brief      Configure the ADC final result buffer width
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  adcResultWidth:  select the ADC result width
 *
 * @return none
 *
 *******************************************************************************/
void ADC_ResultWidthConfig(ADC_ID_Type adcID, ADC_ResultWidth_Type adcResultWidth)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->RESULT_BUF.BF.WIDTH_SEL = adcResultWidth;
}

/****************************************************************************//**
 * @brief      Configure the ADC temperature sensor
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  adcTSensorMode:  select the internal or external temperature sensor
 *
 * @return none
 *
 *******************************************************************************/
void ADC_TSensorConfig(ADC_ID_Type adcID, ADC_TSensorMode_Type adcTSensorMode)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->ANA.BF.TSEXT_SEL = adcTSensorMode;
}

/****************************************************************************//**
 * @brief      Reset the whole ADC block
 *
 * @param[in]  adcID:  select the ADC module
 *
 * @return none
 *
 *******************************************************************************/
void ADC_Reset(ADC_ID_Type adcID)
{
  volatile uint32_t i;
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->CMD.BF.SOFT_RST = 1;
  
  /* dummy delay */
  for(i=0; i<10; i++);
  
  ADCx->CMD.BF.SOFT_RST = 0;
}

/****************************************************************************//**
 * @brief      Enable the ADC module
 *
 * @param[in]  adcID:  select the ADC module
 *
 * @return none
 *
 *******************************************************************************/
void ADC_Enable(ADC_ID_Type adcID)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->GENERAL.BF.GLOBAL_EN = 1;
}

/****************************************************************************//**
 * @brief      Disable the ADC module
 *
 * @param[in]  adcID:  select the ADC module
 *
 * @return none
 *
 *******************************************************************************/
void ADC_Disable(ADC_ID_Type adcID)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->GENERAL.BF.GLOBAL_EN = 0;
}

/****************************************************************************//**
 * @brief      Start the ADC conversion
 *
 * @param[in]  adcID:  select the ADC module
 *
 * @return none
 *
 *******************************************************************************/
void ADC_ConversionStart(ADC_ID_Type adcID)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->CMD.BF.CONV_START = 1;
}

/****************************************************************************//**
 * @brief      Stop the ADC conversion
 *
 * @param[in]  adcID:  select the ADC module
 *
 * @return none
 *
 *******************************************************************************/
void ADC_ConversionStop(ADC_ID_Type adcID)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->CMD.BF.CONV_START = 0;
}

/****************************************************************************//**
 * @brief      Execute the ADC self calibration 
 *
 * @param[in]  adcID:  select the ADC module
 *
 * @return     ADC calibration status
 *
 *******************************************************************************/
Status ADC_SelfCalibration(ADC_ID_Type adcID, ADC_CalVref_Type adcCalVref)
{
  volatile uint32_t i;
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  /* Select calibration voltage reference */
  ADCx->CONFIG.BF.CAL_VREF_SEL = adcCalVref;
  
  /* Execute self calibration */
  ADCx->GENERAL.BF.ADC_CAL_EN = 1;
  
  ADCx->CMD.BF.CONV_START = 1;

  /* Wait for self calibration done */
  for(i=0; i<1000000; i++)
  {
    if(ADCx->GENERAL.BF.ADC_CAL_EN == 0)
    {
      break;
    }
  }
  
  if(ADCx->GENERAL.BF.ADC_CAL_EN)
  {
    return ERROR;
  }
  
  /* Use self calibration data */
  ADCx->CONFIG.BF.CAL_DATA_SEL = 0;
  
  return DSUCCESS;
}

/****************************************************************************//**
 * @brief      Execute the ADC user calibration 
 *
 * @param[in]  adcID:  select the ADC module
 *
 * @return     ADC calibration status
 *
 *******************************************************************************/
Status ADC_UserCalibration(ADC_ID_Type adcID, int16_t adcOffsetCal, int16_t adcGainCal)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  /* Load user defined offset and gain calibration data */
  ADCx->OFFSET_CAL.BF.OFFSET_CAL_USR = adcOffsetCal;
  ADCx->GAIN_CAL.BF.GAIN_CAL_USR = adcGainCal;
  
  /* Use user defined calibration data */
  ADCx->CONFIG.BF.CAL_DATA_SEL = 1;
  
  return DSUCCESS;
}

/****************************************************************************//**
 * @brief      Get the ADC self calibration data
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  adcOffsetCal:  self offset calibration data pointer, evaluate NULL if not required
 * @param[in]  adcGainCal:  self gain calibration data pointer, evaluate NULL if not required
 *
 * @return none
 *
 *******************************************************************************/
void ADC_GetSelfCalibrationData(ADC_ID_Type adcID, int16_t* adcOffsetCal, int16_t* adcGainCal)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  if(adcOffsetCal != NULL)
  {
    *adcOffsetCal = ADCx->OFFSET_CAL.BF.OFFSET_CAL;
  }
  
  if(adcGainCal != NULL)
  {
    *adcGainCal = ADCx->GAIN_CAL.BF.GAIN_CAL;
  }
}

/****************************************************************************//**
 * @brief      Reset the ADC self calibration data
 *
 * @param[in]  adcID:  select the ADC module
 *
 * @return none
 *
 *******************************************************************************/
void ADC_ResetSelfCalibratonData(ADC_ID_Type adcID)
{
  volatile uint32_t i;
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->CONFIG.BF.CAL_DATA_RST = 1;
  
  /* dummy delay */
  for(i=0; i<10; i++);
  
  ADCx->CONFIG.BF.CAL_DATA_RST = 0;
}

/****************************************************************************//**
 * @brief      Get the ADC FIFO data count
 *
 * @param[in]  adcID:  select the ADC module
 *
 * @return     ADC FIFO data count
 *
 *******************************************************************************/
uint8_t ADC_GetFifoDataCount(ADC_ID_Type adcID)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  return ADCx->STATUS.BF.FIFO_DATA_COUNT;
}

/****************************************************************************//**
 * @brief      Get the 32-bit width packed ADC conversion result
 *
 * @param[in]  adcID:  select the ADC module
 *
 * @return     32-bit width packed ADC conversion result
 *
 *******************************************************************************/
uint32_t ADC_GetConversionResult(ADC_ID_Type adcID)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  return ADCx->RESULT.WORDVAL;
}

/****************************************************************************//**
 * @brief      Select the ADC DMA threshold
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  dmaThreshold:  select the ADC DMA threshold
 *
 * @return none
 *
 *******************************************************************************/
void ADC_DmaThresholdSel(ADC_ID_Type adcID, ADC_DmaThreshold_Type dmaThreshold)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->DMAR.BF.FIFO_THL = dmaThreshold;
}

/****************************************************************************//**
 * @brief      Enable/Disable the ADC DMA function
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  cmd:  enable or disable the ADC DMA function
 *
 * @return none
 *
 *******************************************************************************/
void ADC_DmaCmd(ADC_ID_Type adcID, FunctionalState cmd)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->DMAR.BF.DMA_EN = cmd;
}

/****************************************************************************//**
 * @brief      Select the ADC trigger source
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  trigSource:  select the ADC trigger source
 *
 * @return     none
 *
 *******************************************************************************/
void ADC_TriggerSourceSel(ADC_ID_Type adcID, ADC_TrigSource_Type trigSource)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->CONFIG.BF.TRIGGER_SEL = trigSource;
}

/****************************************************************************//**
 * @brief      Enable/Disable the ADC trigger function
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  cmd:  enable or disable the ADC trigger function
 *
 * @return     none
 *
 *******************************************************************************/
void ADC_TriggerCmd(ADC_ID_Type adcID, FunctionalState cmd)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->CONFIG.BF.TRIGGER_EN = cmd;
}

/****************************************************************************//**
 * @brief      Select the ADC audio pga gain
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  trigSource:  select the ADC audio pga gain
 *
 * @return     none
 *
 *******************************************************************************/
void ADC_AudioGainSel(ADC_ID_Type adcID, ADC_AudioGain_Type adcAudioGain)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->AUDIO.BF.PGA_GAIN = adcAudioGain;
}

/****************************************************************************//**
 * @brief      Enable/Disable the ADC audio function
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  cmd:  enable or disable the ADC audio function
 *
 * @return     none
 *
 *******************************************************************************/
void ADC_AudioCmd(ADC_ID_Type adcID, FunctionalState cmd)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->AUDIO.BF.EN = cmd;
}

/****************************************************************************//**
 * @brief      Select the ADC input voice level threshold
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  trigSource:  select the ADC input voice level threshold
 *
 * @return     none
 *
 *******************************************************************************/
void ADC_VoiceLevelSel(ADC_ID_Type adcID, ADC_VoiceLevel_Type adcVoiceLevel)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->VOICE_DET.BF.LEVEL_SEL = adcVoiceLevel;
}

/****************************************************************************//**
 * @brief      Enable/Disable the ADC voice level detection function
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  cmd:  enable or disable the ADC voice level detection function
 *
 * @return     none
 *
 *******************************************************************************/
void ADC_VoiceLevelDetectCmd(ADC_ID_Type adcID, FunctionalState cmd)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  ADCx->VOICE_DET.BF.DET_EN = cmd;
}

/****************************************************************************//**
 * @brief      Get the ADC status
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  statusType:  specify the ADC status type
 *
 * @return     flag status of the specified ADC status type
 *
 *******************************************************************************/
FlagStatus ADC_GetStatus(ADC_ID_Type adcID, ADC_Status_Type statusType)
{
  uint8_t bitStatus = RESET;
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  CHECK_PARAM(IS_ADC_STATUS_TYPE(statusType));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  switch(statusType)
  {
  /* Conversion data ready status */
  case ADC_STATUS_RDY:
    bitStatus = ADCx->IRSR.BF.RDY_RAW;
    break;
    
  /* Gain correction saturation status */
  case ADC_STATUS_GAINSAT:
    bitStatus = ADCx->IRSR.BF.GAINSAT_RAW;
    break;
    
  /* Offset correction saturation status */
  case ADC_STATUS_OFFSAT:
    bitStatus = ADCx->IRSR.BF.OFFSAT_RAW;
    break;
    
  /* Data negative saturation status */
  case ADC_STATUS_DATASAT_NEG:
    bitStatus = ADCx->IRSR.BF.DATASAT_NEG_RAW;
    break;
    
  /* Data positive saturation status */
  case ADC_STATUS_DATASAT_POS:
    bitStatus = ADCx->IRSR.BF.DATASAT_POS_RAW;
    break;
    
  /* FIFO overrun status */
  case ADC_STATUS_FIFO_OVERRUN:
    bitStatus = ADCx->IRSR.BF.FIFO_OVERRUN_RAW;
    break;
    
  /* FIFO underrun status */
  case ADC_STATUS_FIFO_UNDERRUN:
    bitStatus = ADCx->IRSR.BF.FIFO_UNDERRUN_RAW;
    break;
    
  /* Conversion active status */
  case ADC_STATUS_ACTIVE:
    bitStatus = ADCx->STATUS.BF.ACT;
    break;
    
  /* FIFO not empty status */
  case ADC_STATUS_FIFO_NE:
    bitStatus = ADCx->STATUS.BF.FIFO_NE;
    break;
    
  /* FIFO full status */
  case ADC_STATUS_FIFO_FULL:
    bitStatus = ADCx->STATUS.BF.FIFO_FULL;
    break;
  }
  
  return (FlagStatus)bitStatus;
}

/****************************************************************************//**
 * @brief      Get the ADC interrupt status
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  intType:  specify the ADC interrupt type
 *
 * @return     interrupt status of the ADC specified interrupt type
 *
 *******************************************************************************/
IntStatus ADC_GetIntStatus(ADC_ID_Type adcID, ADC_INT_Type intType)
{
  uint8_t bitStatus = RESET;
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  CHECK_PARAM(IS_ADC_INT_TYPE(intType));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  switch(intType)
  {
  /* Conversion data ready interrupt */
  case ADC_RDY:
    bitStatus = ADCx->ISR.BF.RDY;
    break;
    
  /* Gain correction saturation interrupt */
  case ADC_GAINSAT:
    bitStatus = ADCx->ISR.BF.GAINSAT;
    break;
    
  /* Offset correction saturation interrupt */
  case ADC_OFFSAT:
    bitStatus = ADCx->ISR.BF.OFFSAT;
    break;
    
  /* Data negative saturation interrupt */
  case ADC_DATASAT_NEG:
    bitStatus = ADCx->ISR.BF.DATASAT_NEG;
    break;
    
  /* Data positive saturation interrupt */
  case ADC_DATASAT_POS:
    bitStatus = ADCx->ISR.BF.DATASAT_POS;
    break;
    
  /* FIFO overrun interrupt */
  case ADC_FIFO_OVERRUN:
    bitStatus = ADCx->ISR.BF.FIFO_OVERRUN;
    break;
    
  /* FIFO underrun interrupt */
  case ADC_FIFO_UNDERRUN:
    bitStatus = ADCx->ISR.BF.FIFO_UNDERRUN;
    break;
    
  /* Any ADC interrupt */
  case ADC_INT_ALL:
    bitStatus = (ADCx->ISR.WORDVAL & 0x7F) ? 1 : 0;
    break;
  }
  
  return (IntStatus)bitStatus;
}

/****************************************************************************//**
 * @brief      Mask/Unmask the specified ADC interrupt
 *
 * @param[in]  adcID:  select the ADC module
 * @param[in]  intType:  specify the interrupt type
 * @param[in]  intMask:  mask or unmask the specified interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void ADC_IntMask(ADC_ID_Type adcID, ADC_INT_Type intType, IntMask_Type intMask)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  CHECK_PARAM(IS_ADC_INT_TYPE(intType));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  switch(intType)
  {
  /* Conversion data ready interrupt */
  case ADC_RDY:
    ADCx->IMR.BF.RDY_MASK = intMask;
    break;
    
  /* Gain correction saturation interrupt */
  case ADC_GAINSAT:
    ADCx->IMR.BF.GAINSAT_MASK = intMask;
    break;
    
  /* Offset correction saturation interrupt */
  case ADC_OFFSAT:
    ADCx->IMR.BF.OFFSAT_MASK = intMask;
    break;
    
  /* Data negative saturation interrupt */
  case ADC_DATASAT_NEG:
    ADCx->IMR.BF.DATASAT_NEG_MASK = intMask;
    break;
    
  /* Data positive saturation interrupt */
  case ADC_DATASAT_POS:
    ADCx->IMR.BF.DATASAT_POS_MASK = intMask;
    break;
    
  /* FIFO overrun interrupt */
  case ADC_FIFO_OVERRUN:
    ADCx->IMR.BF.FIFO_OVERRUN_MASK = intMask;
    break;
    
  /* FIFO underrun interrupt */
  case ADC_FIFO_UNDERRUN:
    ADCx->IMR.BF.FIFO_UNDERRUN_MASK = intMask;
    break;
    
  /* All ADC interrupts */
  case ADC_INT_ALL:
    ADCx->IMR.WORDVAL = (intMask == MASK) ? 0x7F : 0;
    break;
  }
}

 /****************************************************************************//**
 * @brief      Clear the specified ADC interrupt flag 
 *
 * @param[in]  adcID:  select the ADC module 
 * @param[in]  intType:  specify the interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void ADC_IntClr(ADC_ID_Type adcID, ADC_INT_Type intType)
{
  adc_reg_t * ADCx;
  
  CHECK_PARAM(IS_ADC_PERIPH(adcID));
  CHECK_PARAM(IS_ADC_INT_TYPE(intType));
  
  ADCx = (adc_reg_t *)(adcAddr[adcID]);
  
  switch(intType)
  {
  /* Conversion data ready interrupt */
  case ADC_RDY:
    ADCx->ICR.BF.RDY_CLR = 1;
    break;
    
  /* Gain correction saturation interrupt */
  case ADC_GAINSAT:
    ADCx->ICR.BF.GAINSAT_CLR = 1;
    break;
    
  /* Offset correction saturation interrupt */
  case ADC_OFFSAT:
    ADCx->ICR.BF.OFFSAT_CLR = 1;
    break;
    
  /* Data negative saturation interrupt */
  case ADC_DATASAT_NEG:
    ADCx->ICR.BF.DATASAT_NEG_CLR = 1;
    break;
    
  /* Data positive saturation interrupt */
  case ADC_DATASAT_POS:
    ADCx->ICR.BF.DATASAT_POS_CLR = 1;
    break;
    
  /* FIFO overrun interrupt */
  case ADC_FIFO_OVERRUN:
    ADCx->ICR.BF.FIFO_OVERRUN_CLR = 1;
    break;
    
  /* FIFO underrun interrupt */
  case ADC_FIFO_UNDERRUN:
    ADCx->ICR.BF.FIFO_UNDERRUN_CLR = 1;
    break;
    
  /* All ADC interrupts */
  case ADC_INT_ALL:
    ADCx->ICR.WORDVAL = 0x7F;
    break;
  }
}

/****************************************************************************//**
 * @brief  ADC0 interrupt function
 *
 * @param[in] none
 *
 * @return none
 *
 *******************************************************************************/
void ADC0_IRQHandler(void)
{
  ADC_IRQHandler(ADC0_ID, INT_ADC0);
}

/*@} end of group UART_Public_Functions */

/*@} end of group UART_definitions */

/*@} end of group MW300_Periph_Driver */

