/** @file     mw300_gpt.c
 *
 *  @brief    This file provides GPT functions.
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
#include "mw300_gpt.h"
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @defgroup GPT GPT
 *  @brief GPT driver modules
 *  @{
 */

/** @defgroup GPT_Private_Type
 *  @{
 */


/*@} end of group GPT_Private_Type*/

/** @defgroup GPT_Private_Defines
 *  @{
 */


/*@} end of group GPT_Private_Defines */


/** @defgroup GPT_Private_Variables
 *  @{
 */
 
/**  
 *  @brief GPT1 and GPT2 address array 
 */
static const uint32_t gptAddr[4] = {GPT0_BASE, GPT1_BASE, GPT2_BASE, GPT3_BASE};

/*@} end of group GPT_Private_Variables */

/** @defgroup GPT_Global_Variables
 *  @{
 */


/*@} end of group GPT_Global_Variables */


/** @defgroup GPT_Private_FunctionDeclaration
 *  @{
 */


/*@} end of group GPT_Private_FunctionDeclaration */

/** @defgroup GPT_Private_Functions
 *  @{
 */


/*@} end of group GPT_Private_Functions */

/** @defgroup GPT_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      Initialize the GPT 
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  gptConfig:  Pointer to a GPT configuration structure
 *
 * @return none
 *
 * Initialize the GPT 
 *******************************************************************************/
void GPT_Init(GPT_ID_Type gptID, GPT_Config_Type* gptConfig)
{  
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID));

  /**************************Configure the clock*******************************/
  
  /* select the clock source */
  GPTx->CLK_CNTL_REG.BF.CLK_SRC = gptConfig->clockSrc;
   
  /* set clock prescaler */
  GPTx->CLK_CNTL_REG.BF.CLK_PRE = gptConfig->clockPrescaler;

  /* set clock divider */
  GPTx->CLK_CNTL_REG.BF.CLK_DIV = gptConfig->clockDivider;

  /************************Configure the counter *****************************/

  /* set upp value */
  GPTx->CNT_UPP_VAL_REG.BF.UPP_VAL = gptConfig->uppVal;
  
  /* set counter value update mode */
  GPTx->CNT_CNTL_REG.BF.CNT_UPDT_MOD = gptConfig->cntUpdate;
}

/****************************************************************************//**
 * @brief      Reset GPT counter 
 *
 * @param[in]  gptID:  Select the GPT module
 *
 * @return reset status
 *  
 * Reset the GPT counter
 *******************************************************************************/
Status GPT_CounterReset(GPT_ID_Type gptID)
{  
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  volatile uint32_t cnt = 0;
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID));
    
  /* Reset the GPT counter */
  GPTx->CNT_EN_REG.BF.CNT_RESET = 0x1;
  
  /* Wating until the counter reset is done */
  while(cnt < 0x300000)
  {
    /* Read the counter reset status */
    if(GPTx->CNT_EN_REG.BF.CNT_RST_DONE)
    {
      return DSUCCESS;
    }
    
    cnt++;
  }
  
  return ERROR;
}


/****************************************************************************//**
 * @brief      Start the GPT counter
 *
 * @param[in]  gptID:  Select the GPT module
 *
 * @return none
 *
 * Start the GPT 
 *******************************************************************************/
void GPT_Start(GPT_ID_Type gptID)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));  
  
  /* start GPT counter */
  GPTx->CNT_EN_REG.BF.CNT_START = 1;
}

/****************************************************************************//**
 * @brief      Stop the GPT counter
 *
 * @param[in]  gptID:  Select the GPT module
 *
 * @return none
 *
 * Stop the GPT 
 *******************************************************************************/
void GPT_Stop(GPT_ID_Type gptID)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  
  /* stop GPT counter */
  GPTx->CNT_EN_REG.BF.CNT_STOP = 1;
}

/****************************************************************************//**
 * @brief      GPT channel reset
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  resetChannelNo:  Select the GPT channels that need to be reset. 
 *                              Its value should be one of GPT_RST_CH0(0x01), 
 *                              GPT_RST_CH1(0x02), GPT_RST_CH3(0x04), GPT_RST_CH4(0x08),
 *                              GPT_RST_CH5(0x10) or bitwise OR operation on any 
 *                              number of them.
 *
 * @return none
 *
 * Reset the GPT channel
 *******************************************************************************/
void GPT_ChannelReset(GPT_ID_Type gptID, uint32_t resetChannelNo)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  
  /* reset the GPT channels */
  GPTx->USER_REQ_REG.WORDVAL |= (((resetChannelNo) & 0x3F)<<8);    
}

/****************************************************************************//**
 * @brief      Initialize GPT input function 
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  gptInputConfig : Pointer to a input function configuration
 *
 * @return none
 *
 *******************************************************************************/
void GPT_InputConfig(GPT_ID_Type gptID, GPT_InputConfig_Type* gptInputConfig)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));  

  /* select trigger sampling clock divider */
  GPTx->IC_CNTL_REG.BF.IC_DIV = gptInputConfig->sampleClkDivider;

  /* select input filtering */
  GPTx->IC_CNTL_REG.BF.IC_WIDTH = gptInputConfig->trigFilter;
}

/****************************************************************************//**
 * @brief      Selet the channel function
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  channelID: Channel ID
 * @param[in]  gptChOutConfig:  Pointer to a channel output configuration
 *
 * @return none
 *
 *******************************************************************************/
void GPT_ChannelFuncSelect(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID,
                           GPT_ChannelFunc_Type channelFunc)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_CHANNEL_ID(channelID));
  
  /* select the channel function */
  GPTx->CH[channelID].CHX_CNTL_REG.BF.CH_IO = channelFunc;
}

/****************************************************************************//**
 * @brief      Select the channel input capture edge 
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  channelID:Channel ID
 * @param[in]  edgeSel : Select the input capture edge
 *
 * @return none
 *
 *******************************************************************************/
void GPT_ChannelInputConfig(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, GPT_ICEdge_Type edgeSel)
{  
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_CHANNEL_ID(channelID));
  
  /* select the input capture edge */
  GPTx->CH[channelID].CHX_CNTL_REG.BF.IC_EDGE = edgeSel;
}


/****************************************************************************//**
 * @brief      Initialize GPT channel config for PWM and One-shot modes
 *
 * @param[in]  gptID:  Select te GPT module
 * @param[in]  channelID:  Channel ID
 * @param[in]  gptChOutConfig:  Pointer to a channel output configuration
 *
 * @return none
 *
 *******************************************************************************/
void GPT_ChannelOutputConfig(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, 
                          GPT_ChannelOutputConfig_Type* gptChOutConfig)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_CHANNEL_ID(channelID));  

  /* set the polarity of the waveform */
  GPTx->CH[channelID].CHX_CNTL_REG.BF.POL = gptChOutConfig->polarity;
  
  /************************set channel match values*****************************/

  /* set CMR0 */
  GPTx->CH[channelID].CHX_CMR0_REG.BF.CMR0 = gptChOutConfig->cntMatchVal0;
  
  /* set CMR1 */
  GPTx->CH[channelID].CHX_CMR1_REG.BF.CMR1 = gptChOutConfig->cntMatchVal1;
  
  /* update them to internal shadow registers */
  GPTx->USER_REQ_REG.WORDVAL |= (0x00010000 << channelID);
}


/****************************************************************************//**
 * @brief      Set GPT counter overflow value 
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  uppVal: Counter overflow value
 *
 * @return none
 *
 *******************************************************************************/
void GPT_SetCounterUppVal(GPT_ID_Type gptID, uint32_t uppVal)
{    
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));

  /* set counter overflow value */
  GPTx->CNT_UPP_VAL_REG.BF.UPP_VAL = uppVal;
}
 
/****************************************************************************//**
 * @brief      Set channel counter match value 0 
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  channelID: Channel ID
 * @param[in]  cmr0:  Channel counter match value 0
 *
 * @return none
 *
 *******************************************************************************/
void GPT_SetChannelMatchVal0(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, uint32_t cmr0)
{  
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_CHANNEL_ID(channelID));
  
  /* set channel CMR0 */
  GPTx->CH[channelID].CHX_CMR0_REG.BF.CMR0 = cmr0;
  
  /* update it to internal shadow register */
  GPTx->USER_REQ_REG.WORDVAL |= (0x00010000 << channelID);
}
 

/****************************************************************************//**
 * @brief      Set channel counter match value 1
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  channelID:  Channel ID
 * @param[in]  cmr1:  Channel counter match value 1
 *
 * @return none
 *
 *******************************************************************************/
void GPT_SetChannelMatchVal1(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, uint32_t cmr1)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_CHANNEL_ID(channelID));
  
  /* set channel CMR1 */
  GPTx->CH[channelID].CHX_CMR1_REG.BF.CMR1 = cmr1;
  
  /* update it to internal shadow register */
  GPTx->USER_REQ_REG.WORDVAL |= (0x00010000 << channelID);
}

/****************************************************************************//**
 * @brief      Set channel counter match value 0 and counter match value 0
 *
 * @param[in]  gptID:  Select he GPT module
 * @param[in]  channelID:  Channel ID
 * @param[in]  cmr0:  Channel counter match value 0
 * @param[in]  cmr1:  Channel counter match value 1
 *
 * @return none
 *
 *******************************************************************************/
void GPT_SetChannelBothMatchVal(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, uint32_t cmr0, uint32_t cmr1)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_CHANNEL_ID(channelID));
  
  /* set CMR0 */
  GPTx->CH[channelID].CHX_CMR0_REG.BF.CMR0 = cmr0;
  
  /* set channel CMR1 */
  GPTx->CH[channelID].CHX_CMR1_REG.BF.CMR1 = cmr1;
  
  /* update them to internal shadow registers */
  GPTx->USER_REQ_REG.WORDVAL |= (0x00010000 << channelID);
}

/****************************************************************************//**
 * @brief      Get GPT capture value 
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  channelID:  Channel ID
 *
 * @return capture value
 *
 *******************************************************************************/
uint32_t GPT_GetCaptureVal(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID)
{
  uint32_t capValue;
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_CHANNEL_ID(channelID));

  /* read the value */
  capValue = GPTx->CH[channelID].CHX_CMR0_REG.BF.CMR0;

  return capValue;
}
 
 /****************************************************************************//**
 * @brief      Get counter value 
 *
 * @param[in]  gptID:  Select the GPT module
 *
 * @return counter value
 *
 *******************************************************************************/
uint32_t GPT_GetCounterVal(GPT_ID_Type gptID)
{  
  uint32_t cntValue;
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);    
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  
  /* read the value */
  cntValue = GPTx->CNT_VAL_REG.BF.CNT_VAL;

  return cntValue;
}    

/****************************************************************************//**
 * @brief  GPT DMA channel enable / disable function 
 *
 * @param[in]  gptID: Select the GPT module
 * @param[in]  dmaChannel: Select the DMA channel
 * @param[in]  state: Enable / Disable  
 *
 * @return none
 *
 *******************************************************************************/
void GPT_DMACmd(GPT_ID_Type gptID, GPT_DMAChannel_Type dmaChannel, FunctionalState state)
{  
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_DMA_CHANNEL_ID(dmaChannel));
  
  /* Enable / Disable DMA channel  */
  if(ENABLE == state)
  {
    GPTx->DMA_CNTL_EN_REG.WORDVAL |= (1<<dmaChannel);
  }
  if(DISABLE == state)
  {
    GPTx->DMA_CNTL_EN_REG.WORDVAL &= (~(1<<dmaChannel));
  }
}

/****************************************************************************//**
 * @brief  GPT channel selection for DMA channels function 
 * 
 * @param[in]  gptID: Select the GPT module
 * @param[in]  dmaChannel: Select the DMA channel 
 * @param[in]  channelID: GPT channel ID
 *
 * @return none
 *
 *******************************************************************************/
void GPT_DMAChannelSelect(GPT_ID_Type gptID, GPT_DMAChannel_Type dmaChannel,
                          GPT_ChannelNumber_Type channelID)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_CHANNEL_ID(channelID));
  CHECK_PARAM(IS_GPT_DMA_CHANNEL_ID(dmaChannel));
  
  /* select a gpt channel for DMA channel 0 */
  if(GPT_DMA0 == dmaChannel)
  {
    GPTx->DMA_CNTL_CH_REG.BF.DMA0_CH = channelID;
  }
  
  /* select a GPT channel for DMA channel 1 */
  if(GPT_DMA1 == dmaChannel)
  {
    GPTx->DMA_CNTL_CH_REG.BF.DMA1_CH = channelID;
  }
}

/****************************************************************************//**
 * @brief      GPT ADC/DAC trigger enable
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  state:  Enable / Disable 
 *
 * @return none
 *
 *******************************************************************************/
void GPT_TrigCmd(GPT_ID_Type gptID, FunctionalState state)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID));

  /* enable ADC trigger */
  GPTx->TCR_REG.BF.TRIG_EN = state;
}


/****************************************************************************//**
 * @brief      GPT ADC/DAC trigger config
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  channelID:  Channel ID.It indicates which channel can trigger ADC.
 * @param[in]  triggerDelay: the delay after which a signal is generated to trigger ADC.
 *
 * @return none
 *
 *******************************************************************************/
void GPT_TrigConfig(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, uint32_t trigDelay)
{    
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID));

  /* select a GPT channel as the trigger source */
  GPTx->TCR_REG.BF.TRIG_CHSEL = channelID;
  
  /* set the delay time */
  GPTx->TDR_REG.BF.TRIG_DLY = trigDelay;
}


/****************************************************************************//**
 * @brief     MASK / UNMASK  GPT interrupt 
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  intType:  Interrupt type
 * @param[in]  intMsk:   MASK / UNMASK control
 *
 * @return none
 *
 *******************************************************************************/
void GPT_IntMask(GPT_ID_Type gptID, GPT_INT_Type intType, IntMask_Type intMsk)
{   
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
 
  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_INT_TYPE(intType));
  CHECK_PARAM(IS_INTMASK(intMsk));

  /* set channel interrupt mask/unmask */
  if(intType < GPT_CHANNEL_NUMBER)
  {
    if(intMsk == UNMASK )
    {
      GPTx->INT_MSK_REG.WORDVAL &= (~(1<<(intType)));
    }
    else
    {
      GPTx->INT_MSK_REG.WORDVAL |= (1<<(intType));
    }   
  }
  
  /* set channel error interrupt mask/unmask */
  if((intType >=  GPT_INT_CH0_ERR) && (intType <= GPT_INT_CH5_ERR))
  {
    if(intMsk == UNMASK )
    {
      GPTx->INT_MSK_REG.WORDVAL &= (~(0x00000100<<(intType-6)));
    }
    else
    {
      GPTx->INT_MSK_REG.WORDVAL |= (0x00000100<<(intType-6));
    }   
  }
  
  switch(intType)
  {
    /* set counter up interrupt mask/unmask*/
    case GPT_INT_CNT_UPP:
      GPTx->INT_MSK_REG.BF.CNT_UPP_MSK = intMsk;
      break;

    /* set DMA Channel 0 interrupt mask/unmask*/
    case GPT_INT_DMA0_OF:
      GPTx->INT_MSK_REG.BF.DMA0_OF_MSK = intMsk;
      break;

    /* set DMA Channel 1 interrupt mask/unmask*/
    case GPT_INT_DMA1_OF:
      GPTx->INT_MSK_REG.BF.DMA1_OF_MSK = intMsk;
      break;
      
    /* set all interrupt mask/unmask*/
    case GPT_INT_ALL:
      if(intMsk == UNMASK)
      {
        GPTx->INT_MSK_REG.WORDVAL &= (~GPT_INT_ALL_MSK);
      }
      else
      {
        GPTx->INT_MSK_REG.WORDVAL |= GPT_INT_ALL_MSK;
      }
      break;
      
    default:
      break;
  }  
}

/****************************************************************************//**
 * @brief      Clear GPT status flag
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  status:  GPT status type
 *
 * @return none
 *
 *******************************************************************************/
void GPT_StatusClr(GPT_ID_Type gptID, GPT_Status_Type status)
{  
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_STATUS_TYPE(status));

  /* clear channel status flag */
  if(status < GPT_CHANNEL_NUMBER)
  {
    GPTx->STS_REG.WORDVAL = 1<<(status);
  }
  
  /* clear channel error status flag */   
  if((status >=  GPT_STATUS_CH0_ERR) && (status <= GPT_STATUS_CH5_ERR))
  {
    GPTx->STS_REG.WORDVAL = 0x00000100<<(status-6);
  }
  
  switch(status)
  {
    /* clear counter up status flag */
    case GPT_STATUS_CNT_UPP:
      GPTx->STS_REG.WORDVAL = GPT_INT_CNT_UPP_MSK;
      break;

    /* clear DMA CH0 status flag */
    case GPT_STATUS_DMA0_OF:
      GPTx->STS_REG.WORDVAL = GPT_INT_DMA0_OVF_MSK;
      break;

    /* clear DMA CH1 status flag */
    case GPT_STATUS_DMA1_OF:
      GPTx->STS_REG.WORDVAL = GPT_INT_DMA1_OVF_MSK;
      break;
      
     /* clear all the GPT status flags */
    case GPT_STATUS_ALL:
      GPTx->STS_REG.WORDVAL = GPT_INT_ALL_MSK;
      break;
      
    default:
      break;
  }  
}

/****************************************************************************//**
 * @brief      Get GPT status flag
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  status:  Status type
 *
 * @return status
 *
 *******************************************************************************/
FlagStatus GPT_GetStatus(GPT_ID_Type gptID, GPT_Status_Type status)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  uint32_t retStatus = 0;

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_STATUS_TYPE(status));

  /* get channel status */
  if(status < GPT_CHANNEL_NUMBER)
  {
    retStatus = ((GPTx->STS_REG.WORDVAL) & (1<<(status)));
  }
    
  /* get channel error status */
  if((status >= 6) && (status < GPT_CHANNEL_NUMBER + 6))
  {
    retStatus = ((GPTx->STS_REG.WORDVAL) & (1<<(status + 2)));
  }
    
  switch(status)
  {
    /* get counter up status */
    case GPT_STATUS_CNT_UPP:
      retStatus = GPTx->STS_REG.BF.CNT_UPP_STS;
      break;

    /* get DMA Channel0 status */
    case GPT_STATUS_DMA0_OF:
      retStatus = GPTx->STS_REG.BF.DMA0_OF_STS;
      break;

    /* get DMA Channel1 status */
    case GPT_STATUS_DMA1_OF:
      retStatus = GPTx->STS_REG.BF.DMA1_OF_STS;
      break;

    /* get all status. If there is any interrupt, return SET, otherwise return RESET. */
    case GPT_INT_ALL:
      retStatus = (GPTx->STS_REG.WORDVAL)&(0x03013F3F);
      break;
      
    default:
      break;
  }

  if(retStatus)
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}

/****************************************************************************//**
 * @brief  GPT channel interrupt function 
 *
 * @param[in]  intPeriph: Select the peripheral
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  chIntStatus: Channel interrupt status 
 *
 * @return none
 *
 *******************************************************************************/
static void GPT_ChIRQHandler(INT_Peripher_Type intPeriph, GPT_ID_Type gptID, uint32_t chIntStatus)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  uint8_t cnt = 0;
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID)); 
  
  /* scan the channels */
  for(cnt = 0; cnt < GPT_CHANNEL_NUMBER; cnt++)
  {
    /* check if interrupt available for the specific channel */
    if((chIntStatus & 0x01) == 0x01)
    {
      if(intCbfArra[intPeriph][cnt] != NULL)
      {      
        /* call the callback function */
        intCbfArra[intPeriph][cnt]();
      }      
      else
      {
        /* Disable the interrupt if callback function is not setup */
        GPTx->INT_MSK_REG.WORDVAL |= (1<<cnt) ;
      }
      
    }
    
    chIntStatus = chIntStatus >> 1;
  }
  
}

/****************************************************************************//**
 * @brief  GPT channel error interrupt function 
 *
 * @param[in]  intPeriph: Select the peripheral
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  chErrIntStatus: Channel error interrupt status 
 *
 * @return none
 *
 *******************************************************************************/
static void GPT_ChErrIRQHandler(INT_Peripher_Type intPeriph, GPT_ID_Type gptID, uint32_t chErrIntStatus)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  uint8_t cnt = 0;
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID)); 

  /* scan the channels */
  for(cnt = 0; cnt < GPT_CHANNEL_NUMBER; cnt++)
  {
    /* check if interrupt available for the specific channel */
    if((chErrIntStatus & 0x0100) != 0)
    {
      if(intCbfArra[intPeriph][cnt+6] != NULL)
      {      
        /* call the callback function */
        intCbfArra[intPeriph][cnt+6]();
      }      
      else
      {
        /* Disable the interrupt if callback function is not setup */
        GPTx->INT_MSK_REG.WORDVAL |= (0x00000100<<cnt) ;
      }
      
    }
    
    chErrIntStatus = chErrIntStatus >> 1;
  }
  
}

/****************************************************************************//**
 * @brief  GPT interrupt function 
 *
 * @param[in]  intPeriph: Select the peripheral
 * @param[in]  gptID:  Select the GPT module
 *
 * @return none
 *
 *******************************************************************************/
static void GPT_IRQHandler(INT_Peripher_Type intPeriph, GPT_ID_Type gptID)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  uint32_t intStatus, chIntStatus, chErrIntStatus;  
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID));

  /* keep a copy of current interrupt status */
  intStatus = GPTx->INT_REG.WORDVAL;
  chIntStatus = intStatus & (GPT_INT_CH_ALL_MSK);
  chErrIntStatus = intStatus & (GPT_INT_CH_ALL_ERR_MSK);   
  
  /* clear the generated interrupts */
  GPTx->STS_REG.WORDVAL = intStatus;
  
  /* channel interrupt */
  if(chIntStatus != 0x00)
  {
    GPT_ChIRQHandler(intPeriph, gptID, chIntStatus);
  }
  
  /* channel error interrupt */
  if(chErrIntStatus != 0x00)
  {
    GPT_ChErrIRQHandler(intPeriph, gptID, chErrIntStatus);
  }
  
  /* counter upp interrupt */
  if((intStatus & (GPT_INT_CNT_UPP_MSK))!= 0x00)
  {
    if(intCbfArra[intPeriph][GPT_INT_CNT_UPP] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][GPT_INT_CNT_UPP]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      GPTx->INT_MSK_REG.BF.CNT_UPP_MSK = 1;
    }
  }
 
  /* DMA channel 0 overflow interrupt */
  if((intStatus & (GPT_INT_DMA0_OVF_MSK))!= 0x00)
  {
    if(intCbfArra[intPeriph][GPT_INT_DMA0_OF] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][GPT_INT_DMA0_OF]();
    }
    else
    {
      /* Disable the interrupt if callback function is not setup */
      GPTx->INT_MSK_REG.BF.DMA0_OF_MSK = 1;
    }
  }  

  /* DMA channel 1 overflow interrupt */
  if((intStatus & (GPT_INT_DMA1_OVF_MSK))!= 0x00)
  {
    if(intCbfArra[intPeriph][GPT_INT_DMA1_OF] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][GPT_INT_DMA1_OF]();
    }    
    else
    {
      /* Disable the interrupt if callback function is not setup */
      GPTx->INT_MSK_REG.BF.DMA1_OF_MSK = 1;
    }
  }  

}


/****************************************************************************//**
 * @brief  GPT0 interrupt handle function 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void GPT0_IRQHandler(void)
{
  GPT_IRQHandler(INT_GPT0, GPT0_ID);
}

/****************************************************************************//**
 * @brief  GPT1 interrupt handle function 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void GPT1_IRQHandler(void)
{
  GPT_IRQHandler(INT_GPT1, GPT1_ID);
}

/****************************************************************************//**
 * @brief  GPT2 interrupt handle function 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void GPT2_IRQHandler(void)
{
  GPT_IRQHandler(INT_GPT2, GPT2_ID);
}

/****************************************************************************//**
 * @brief  GPT3 interrupt handle function 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void GPT3_IRQHandler(void)
{
  GPT_IRQHandler(INT_GPT3, GPT3_ID);
}
/*@} end of group GPT_Public_Functions */

/*@} end of group GPT  */

/*@} end of group MW300_Periph_Driver */

