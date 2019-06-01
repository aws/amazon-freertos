/** @file     mw300_dma.c
 *
 *  @brief    This file provides DMA functions.
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
#include <compat_attribute.h>
#include "mw300.h"
#include "mw300_driver.h"
#include "mw300_dma.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @defgroup DMA DMA
 *  @brief DMA driver modules
 *  @{
 */

/** @defgroup DMA_Private_Type
 *  @{
 */

/*@} end of group DMA_Private_Type*/

/** @defgroup DMA_Private_Defines
 *  @{
 */

/*@} end of group DMA_Private_Defines */

/** @defgroup DMA_Private_Variables
 *  @{
 */

/**
 * @brief interrupt channel array
 */
static const  __UNUSED__  uint32_t intChannelArra[] = {INT_DMA_CH0, INT_DMA_CH1,
						       INT_DMA_CH2, INT_DMA_CH3,
						       INT_DMA_CH4, INT_DMA_CH5,
						       INT_DMA_CH6, INT_DMA_CH7,
						       INT_DMA_CH8, INT_DMA_CH9,
					       INT_DMA_CH10, INT_DMA_CH11,
					       INT_DMA_CH12, INT_DMA_CH13,
					       INT_DMA_CH14, INT_DMA_CH15,
					       INT_DMA_CH16, INT_DMA_CH17,
					       INT_DMA_CH18, INT_DMA_CH19,
					       INT_DMA_CH20, INT_DMA_CH21,
					       INT_DMA_CH22, INT_DMA_CH23,
					       INT_DMA_CH24, INT_DMA_CH25,
					       INT_DMA_CH26, INT_DMA_CH27,
					       INT_DMA_CH28, INT_DMA_CH29,
					       INT_DMA_CH30, INT_DMA_CH31};

/*@} end of group DMA_Private_Variables */

/** @defgroup DMA_Global_Variables
 *  @{
 */

/*@} end of group DMA_Global_Variables */

/** @defgroup DMA_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group DMA_Private_FunctionDeclaration */

/** @defgroup DMA_Private_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      DMA interrupt handler 
 *
 * @param[in]  none
 *
 * @return none
 *
 *******************************************************************************/
/* Note: Since mdev_dma.c implements DMA_IRQHandler() call in the mdev layer,
 * DMA_IRQHandler() from mw300_dma.c is disabled */
#if 0
void DMA_IRQHandler()
{
  uint32_t intChannel;
  uint64_t intStatusTfr, intStatusBlock, intStatusBusErr, intStatusAddrErr;
      
  /* Get current unmasked interrupt status */
  intStatusTfr = DMA->STATUS_TFRINT.WORDVAL;
  intStatusBlock = DMA->STATUS_BLOCKINT.WORDVAL;
  intStatusBusErr = DMA->STATUS_BUSERRINT.WORDVAL;
  intStatusAddrErr = DMA->STATUS_ADDRERRINT.WORDVAL;
  
  /* Clear the unmasked generated interrupts */
  DMA->STATUS_TFRINT.WORDVAL = 0xffffffff;
  DMA->STATUS_BLOCKINT.WORDVAL = 0xffffffff;
  DMA->STATUS_BUSERRINT.WORDVAL = 0xffffffff;
  DMA->STATUS_ADDRERRINT.WORDVAL = 0xffffffff;

  if(intStatusTfr)
  {
    /* Check interrupt channel */
    for(intChannel = 0; intChannel < 32; intChannel++)
    {
      if(intStatusTfr & (0x01 << intChannel))
      {
        if(intCbfArra[intChannelArra[intChannel]][INT_DMA_TRANS_COMPLETE] != NULL)
        {
          intCbfArra[intChannelArra[intChannel]][INT_DMA_TRANS_COMPLETE]();
        }
        /* Mask interrupt */
        else
        {
          DMA->MASK_TFRINT.WORDVAL = (1 << intChannel);
        }
      }
    }
  }

  if(intStatusBlock)
  {
    /* Check interrupt channel */
    for(intChannel = 0; intChannel < 32; intChannel++)
    {
      if(intStatusBlock & (0x01 << intChannel))
      {
        if(intCbfArra[intChannelArra[intChannel]][INT_BLK_TRANS_COMPLETE] != NULL)
        {
          intCbfArra[intChannelArra[intChannel]][INT_BLK_TRANS_COMPLETE]();
        }
        /* Mask interrupt */
        else
        {
          DMA->MASK_BLOCKINT.WORDVAL = (1 << intChannel);
        }
      }
    }
  }

  if(intStatusBusErr)
  {
    /* Check interrupt channel */
    for(intChannel = 0; intChannel < 32; intChannel++)
    {
      if(intStatusBusErr & (0x01 << intChannel))
      {
        if(intCbfArra[intChannelArra[intChannel]][INT_BUS_ERROR] != NULL)
       {
         intCbfArra[intChannelArra[intChannel]][INT_BUS_ERROR]();
       }
       /* Mask interrupt */
       else
       {
         DMA->MASK_BUSERRINT.WORDVAL = (1 << intChannel);
       }
      }
    }
  }

  if(intStatusAddrErr)
  {
    /* Check interrupt channel */
    for(intChannel = 0; intChannel < 32; intChannel++)
    {
      if(intStatusAddrErr & (0x01 << intChannel))
      {
        if(intCbfArra[intChannelArra[intChannel]][INT_ADDRESS_ERROR] != NULL)
        {
          intCbfArra[intChannelArra[intChannel]][INT_ADDRESS_ERROR]();
        }
        /* Mask interrupt */
        else
        {
          DMA->MASK_ADDRERRINT.WORDVAL = (1 << intChannel);
        }
      }
    }
  }
  
}
#endif
/*@} end of group DMA_Private_Functions */

/** @defgroup DMA_Public_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief      Initializes the DMA channel
 *
 * @param[in]  channelx:  Select the DMA channel, should be CHANNEL_0 ... CHANNEL_31 
 * @param[in]  dmaCfgStruct:  Pointer to a DMA configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void DMA_ChannelInit(DMA_Channel_Type channelx, DMA_CFG_Type * dmaCfgStruct)
{
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));

  /* Config the source address of DMA transfer */
  DMA->CHANNEL[channelx].SADR.WORDVAL = dmaCfgStruct->srcDmaAddr;

  /* Config the destination address of DMA transfer */
  DMA->CHANNEL[channelx].TADR.WORDVAL = dmaCfgStruct->destDmaAddr;
  
  /* Transfer type and flow control */
  DMA->CHANNEL[channelx].CTRLA.BF.TRAN_TYPE = dmaCfgStruct->transfType;

  /* Set burst transaction length */
  DMA->CHANNEL[channelx].CTRLA.BF.TRAN_SIZE = dmaCfgStruct->burstLength;

  /* Set Source address increment mode */
  DMA->CHANNEL[channelx].CTRLA.BF.INCSRCADDR = dmaCfgStruct->srcAddrInc;

  /* Set Target address increment mode */
  DMA->CHANNEL[channelx].CTRLA.BF.INCTRGADDR = dmaCfgStruct->destAddrInc;

  /* Transfer width */
  DMA->CHANNEL[channelx].CTRLA.BF.WIDTH = dmaCfgStruct->transfWidth;

  /* Set Dma transfer length in byte */  
  DMA->CHANNEL[channelx].CTRLA.BF.LEN = dmaCfgStruct->transfLength;
}

/****************************************************************************//**
 * @brief      Enable the DMA 
 *
 * @param[in]  channelx:  Select the DMA channel, should be CHANNEL_0 ... CHANNEL_31 
 *
 * @return none
 *
 *******************************************************************************/
void DMA_Enable(DMA_Channel_Type channelx)
{
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));
  
  /* Enable/Disable DMA */
  DMA->CHANNEL[channelx].CHL_EN.BF.CHL_EN = 1;
}

/****************************************************************************//**
 * @brief      Disable the DMA 
 *
 * @param[in]  channelx:  Select the DMA channel, should be CHANNEL_0 ... CHANNEL_31 
 *
 * @return none
 *
 *******************************************************************************/
void DMA_Disable(DMA_Channel_Type channelx)
{
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));
  
  /* Enable/Disable DMA */
  DMA->CHANNEL[channelx].CHL_EN.BF.CHL_EN = 0;
}

/****************************************************************************//**
 * @brief      Stop the DMA
 *
 * @param[in]  channelx:  Select the DMA channel, should be CHANNEL_0 ... CHANNEL_31 
 *
 * @return none
 *
 *******************************************************************************/
void DMA_Stop(DMA_Channel_Type channelx)
{
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));
  
  /* Enable/Disable DMA */
  DMA->CHANNEL[channelx].CHL_STOP.BF.CHL_STOP = 0x1;
}

/****************************************************************************//**
 * @brief      Set Peripheral type of DMA interface 
 *
 * @param[in]  channelx:  Select the DMA channel, should be CHANNEL_0 ... CHANNEL_31
 * @param[in]  perDmaInter:  Select Peripheral type of DMA interface
 *
 * @return none
 *
 *******************************************************************************/
void DMA_SetPeripheralType(DMA_Channel_Type channelx, DMA_PerMapping_Type perDmaInter)
{
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));
  
  /* Config the peripheral type of DMA interface */
  DMA->CHANNEL[channelx].CTRLB.BF.PERNUM = perDmaInter;
}

/****************************************************************************//**
 * @brief      Set DMA ACK Delay Cycle for single transfer in Memory to Peripheral transfer type
 *
 * @param[in]  delaycycle:  For single write transaction to peripheral. The value should be
                            less than 1023, default value 12
 *
 * @return none
 *
 *******************************************************************************/
void DMA_SetAckDelayCycle(uint32_t delaycycle)
{
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_DELAY_CYCLE(delaycycle));  
  
  /* Config DMA ACK Delay Cycle for single transfer */
  DMA->ACK_DELAY.BF.ACK_DELAY_NUM = delaycycle;
}

/****************************************************************************//**
 * @brief      Get current DMA channel enable Status 
 *
 * @param[in]  channelx: Select the DMA channel, should be CHANNEL_0 ... CHANNEL_31 
 *
 * @return The anable state value: RESET or SET
 *
 *******************************************************************************/
FlagStatus DMA_GetChannelEnableStatus(DMA_Channel_Type channelx)
{
  FlagStatus BitStatus = RESET; 
  
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));
  
  if(DMA->CHANNEL[channelx].CHL_EN.BF.CHL_EN)
  {
    BitStatus = SET;
  }
  else
  {
    BitStatus = RESET;
  }
  
  return BitStatus;
}

/****************************************************************************//**
 * @brief      Get current DMA channel interrupt Status for given interrupt type 
 *
 * @param[in]  channelx: Select the DMA channel, should be CHANNEL_0 ... CHANNEL_31 
 * @param[in]  intType: specified interrupt, should be 
 *             - INT_DMA_TRANS_COMPLETE
 *             - INT_BLK_TRANS_COMPLETE
 *             - INT_BUS_ERROR
 *             - INT_ADDRESS_ERROR
 *
 * @return The interrupt state value: RESET or SET
 *
 *******************************************************************************/
FlagStatus DMA_GetChannelIntStatus(DMA_Channel_Type channelx, DMA_Int_Type intType)
{
  FlagStatus intBitStatus = RESET;

  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));
  CHECK_PARAM(IS_DMA_INT(intType));
  
  switch(intType)
  {
    case INT_DMA_TRANS_COMPLETE:
      if(DMA->STATUS_TFRINT.WORDVAL & (1 << channelx))
      {
        intBitStatus = SET;
      }
      else
      {
        intBitStatus = RESET;
      }
      
      break;

    case INT_BLK_TRANS_COMPLETE:
      if(DMA->STATUS_BLOCKINT.WORDVAL & (1 << channelx))
      {
        intBitStatus = SET;
      }
      else
      {
        intBitStatus = RESET;
      }

      break;
      
    case INT_BUS_ERROR:
      if(DMA->STATUS_BUSERRINT.WORDVAL & (1 << channelx))
      {
        intBitStatus = SET;
      }
      else
      {
        intBitStatus = RESET;
      }

      break;

    case INT_ADDRESS_ERROR:
      if(DMA->STATUS_ADDRERRINT.WORDVAL & (1 << channelx))
      {
        intBitStatus = SET;
      }
      else
      {
        intBitStatus = RESET;
      }

      break;

    default:
      break;
  }

  return intBitStatus;
}

/****************************************************************************//**
 * @brief      Set DMA interrupt mask for given interrupt type 
 *
 * @param[in]  channelx: Select the DMA channel, should be CHANNEL_0 ... CHANNEL_31
 * @param[in]  intMask: Mask/Unmask specified interrupt type 
 * @param[in]  intType: specified interrupt, should be 
 *             - INT_DMA_TRANS_COMPLETE
 *             - INT_BLK_TRANS_COMPLETE
 *             - INT_BUS_ERROR
 *             - INT_ADDRESS_ERROR
 *
 * @return none
 *
 *******************************************************************************/
void DMA_IntMask(DMA_Channel_Type channelx, DMA_Int_Type intType, IntMask_Type intMask)
{
  uint64_t unmaskInt, maskInt;
  
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));
  CHECK_PARAM(IS_DMA_INT(intType));
  CHECK_PARAM(IS_INTMASK(intMask));;


  unmaskInt = (1 << channelx);
  
  maskInt = ~unmaskInt;

  switch(intType)
  {
    case INT_DMA_TRANS_COMPLETE:
      if (MASK == intMask)
         DMA->MASK_TFRINT.WORDVAL &= maskInt;
      else
	 DMA->MASK_TFRINT.WORDVAL |= unmaskInt;
      break;

    case INT_BLK_TRANS_COMPLETE:
      if (MASK == intMask)
         DMA->MASK_BLOCKINT.WORDVAL &= maskInt;
      else
	 DMA->MASK_BLOCKINT.WORDVAL |= unmaskInt;
      break;

    case INT_BUS_ERROR:
       if (MASK == intMask)
         DMA->MASK_BUSERRINT.WORDVAL &= maskInt;
      else
	 DMA->MASK_BUSERRINT.WORDVAL |= unmaskInt;
      break;

    case INT_ADDRESS_ERROR:
      if (MASK == intMask)
         DMA->MASK_ADDRERRINT.WORDVAL &= maskInt;
      else
	 DMA->MASK_ADDRERRINT.WORDVAL |= unmaskInt;
      break;

     case INT_CH_ALL:
      if (MASK == intMask) {
	DMA->MASK_TFRINT.WORDVAL &= maskInt;
	DMA->MASK_BLOCKINT.WORDVAL &= maskInt;
	DMA->MASK_BUSERRINT.WORDVAL &= maskInt;
	DMA->MASK_ADDRERRINT.WORDVAL &= maskInt;
      } else {
	DMA->MASK_TFRINT.WORDVAL |= unmaskInt;
	DMA->MASK_BLOCKINT.WORDVAL |= unmaskInt;
	DMA->MASK_BUSERRINT.WORDVAL |= unmaskInt;
	DMA->MASK_ADDRERRINT.WORDVAL |= unmaskInt;
      }
    default:
      break;
  }

}

/****************************************************************************//**
 * @brief      Get current DMA interrupt Status for given interrupt type 
 *
 * @param[in]  channelx: Select the DMA channel, should be CHANNEL_0 ... CHANNEL_31 
 * @param[in]  intType: specified interrupt, should be 
 *             - INT_DMA_TRANS_COMPLETE
 *             - INT_BLK_TRANS_COMPLETE
 *             - INT_BUS_ERROR
 *             - INT_ADDRESS_ERROR
 *             - INT_CH_ALL
 *
 * @return none
 *
 *******************************************************************************/
void DMA_IntClr(DMA_Channel_Type channelx, DMA_Int_Type intType)
{
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHANNEL(channelx));
  CHECK_PARAM(IS_DMA_INT(intType));

  uint32_t clrInt = (1 << channelx);
  switch(intType)
  {
    case INT_DMA_TRANS_COMPLETE:    
      DMA->STATUS_TFRINT.WORDVAL = clrInt;
      break;

    case INT_BLK_TRANS_COMPLETE:
      DMA->STATUS_BLOCKINT.WORDVAL = clrInt;
      break;
      
    case INT_BUS_ERROR:      
      DMA->STATUS_BUSERRINT.WORDVAL = clrInt;
      break;

    case INT_ADDRESS_ERROR:      
      DMA->STATUS_ADDRERRINT.WORDVAL = clrInt;
      break;
      
    case INT_CH_ALL:      
      DMA->STATUS_TFRINT.WORDVAL = clrInt;
      DMA->STATUS_BLOCKINT.WORDVAL = clrInt;
      DMA->STATUS_BUSERRINT.WORDVAL = clrInt;
      DMA->STATUS_ADDRERRINT.WORDVAL = clrInt;
      break;
      
    default:
      break;
  }
}

/*@} end of group DMA_Public_Functions */

/*@} end of group DMA_definitions */

/*@} end of group MW300_Periph_Driver */

