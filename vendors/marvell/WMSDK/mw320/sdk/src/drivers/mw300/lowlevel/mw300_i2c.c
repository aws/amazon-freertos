/** @file     mw300_i2c.c
 *
 *  @brief    This file provides I2C functions.
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
#include "mw300_driver.h"
#include "mw300_i2c.h"
#include <compat_attribute.h>

/** @addtogroup MW300_Periph_Driver
 *  @{
 */

/** @defgroup I2C I2C
 *  @brief I2C driver modules
 *  @{
 */

/** @defgroup I2C_Private_Type
 *  @{
 */

/*@} end of group I2C_Private_Type */

/** @defgroup I2C_Private_Defines
 *  @{
 */

/*@} end of group I2C_Private_Defines */

/** @defgroup I2C_Private_Variables
 *  @{
 */

/**
 *  @brief I2C0, I2C1 address array
 */
static const uint32_t i2cAddr[2] = {I2C0_BASE, I2C1_BASE};

/*@} end of group I2C_Private_Variables */

/** @defgroup I2C_Global_Variables
 *  @{
 */

/*@} end of group I2C_Global_Variables */

/** @defgroup I2C_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group I2C_Private_FunctionDeclaration */

/** @defgroup I2C_Private_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief      I2C interrupt handler
 *
 * @param[in]  intPeriph: Select the peripheral, should be INT_I2C0, INT_I2C1
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 *
 * @return none
 *
 *******************************************************************************/
void I2C_IntHandler(INT_Peripher_Type intPeriph, I2C_ID_Type i2cNo)
{
  uint32_t intStatus;
	__UNUSED__ volatile uint32_t dummyData;

  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));

  /* Read the interrupt status */
  intStatus = I2Cx->INTR_STAT.WORDVAL;

  /* Dummy read to clear all interrupts */
  dummyData = I2Cx->CLR_INTR.BF.CLR_INTR;

  if(intStatus & (0x01 << I2C_INT_RX_UNDER))
  {
    if(intCbfArra[intPeriph][I2C_INT_RX_UNDER]!= NULL)
    {
      intCbfArra[intPeriph][I2C_INT_RX_UNDER]();
    }
    /* Mask the interrupt */
    else
    {
      I2Cx->INTR_MASK.BF.M_RX_UNDER = 0;
    }
  }

  if(intStatus & (0x01 << I2C_INT_RX_OVER))
  {
    if(intCbfArra[intPeriph][I2C_INT_RX_OVER]!= NULL)
    {
      intCbfArra[intPeriph][I2C_INT_RX_OVER]();
    }
    /* Mask the interrupt */
    else
    {
      I2Cx->INTR_MASK.BF.M_RX_OVER = 0;
    }
  }

  if(intStatus & (0x01 << I2C_INT_RX_FULL))
  {
    if(intCbfArra[intPeriph][I2C_INT_RX_FULL]!= NULL)
    {
      intCbfArra[intPeriph][I2C_INT_RX_FULL]();
    }
    /* Mask the interrupt */
    else
    {
      I2Cx->INTR_MASK.BF.M_RX_FULL = 0;
    }
  }

  if(intStatus & (0x01 << I2C_INT_TX_OVER))
  {
    if(intCbfArra[intPeriph][I2C_INT_TX_OVER]!= NULL)
    {
      intCbfArra[intPeriph][I2C_INT_TX_OVER]();
    }
    /* Mask the interrupt */
    else
    {
      I2Cx->INTR_MASK.BF.M_TX_OVER = 0;
    }
  }

  if(intStatus & (0x01 << I2C_INT_TX_EMPTY))
  {
    if(intCbfArra[intPeriph][I2C_INT_TX_EMPTY]!= NULL)
    {
      intCbfArra[intPeriph][I2C_INT_TX_EMPTY]();
    }
    /* Mask the interrupt */
    else
    {
      I2Cx->INTR_MASK.BF.M_TX_EMPTY = 0;
    }
  }

  if(intStatus & (0x01 << I2C_INT_RD_REQ))
  {
    if(intCbfArra[intPeriph][I2C_INT_RD_REQ]!= NULL)
    {
      intCbfArra[intPeriph][I2C_INT_RD_REQ]();
    }
    /* Mask the interrupt */
    else
    {
      I2Cx->INTR_MASK.BF.M_RD_REQ = 0;
    }
  }

  if(intStatus & (0x01 << I2C_INT_TX_ABORT))
  {
    if(intCbfArra[intPeriph][I2C_INT_TX_ABORT]!= NULL)
    {
      intCbfArra[intPeriph][I2C_INT_TX_ABORT]();
    }
    /* Mask the interrupt */
    else
    {
      I2Cx->INTR_MASK.BF.M_TX_ABRT = 0;
    }
  }

  if(intStatus & (0x01 << I2C_INT_RX_DONE))
  {
    if(intCbfArra[intPeriph][I2C_INT_RX_DONE]!= NULL)
    {
      intCbfArra[intPeriph][I2C_INT_RX_DONE]();
    }
    /* Mask the interrupt */
    else
    {
      I2Cx->INTR_MASK.BF.M_RX_DONE = 0;
    }
  }

  if(intStatus & (0x01 << I2C_INT_ACTIVITY))
  {
    if(intCbfArra[intPeriph][I2C_INT_ACTIVITY]!= NULL)
    {
      intCbfArra[intPeriph][I2C_INT_ACTIVITY]();
    }
    /* Mask the interrupt */
    else
    {
      I2Cx->INTR_MASK.BF.M_ACTIVITY = 0;
    }
  }

  if(intStatus & (0x01 << I2C_INT_STOP_DET))
  {
    if(intCbfArra[intPeriph][I2C_INT_STOP_DET]!= NULL)
    {
      intCbfArra[intPeriph][I2C_INT_STOP_DET]();
    }
    /* Mask the interrupt */
    else
    {
      I2Cx->INTR_MASK.BF.M_STOP_DET = 0;
    }
  }

  if(intStatus & (0x01 << I2C_INT_START_DET))
  {
    if(intCbfArra[intPeriph][I2C_INT_START_DET]!= NULL)
    {
      intCbfArra[intPeriph][I2C_INT_START_DET]();
    }
    /* Mask the interrupt */
    else
    {
      I2Cx->INTR_MASK.BF.M_START_DET = 0;
    }
  }

  if(intStatus & (0x01 << I2C_INT_GEN_CALL))
  {
    if(intCbfArra[intPeriph][I2C_INT_GEN_CALL]!= NULL)
    {
      intCbfArra[intPeriph][I2C_INT_GEN_CALL]();
    }
    /* Mask the interrupt */
    else
    {
      I2Cx->INTR_MASK.BF.M_GEN_CALL = 0;
    }
  }
}
/*@} end of group I2C_Private_Functions */

/** @defgroup I2C_Public_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief      Initializes the I2C
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 * @param[in]  i2cCfgStruct:  Pointer to a I2C configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void I2C_Init(I2C_ID_Type i2cNo, I2C_CFG_Type* i2cCfgStruct)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));

  /* Set the I2C speed mode */
  I2Cx->CON.BF.SPEED = i2cCfgStruct->speedMode;

  /* Set the I2C master or slave mode */
  if(I2C_MASTER == i2cCfgStruct->masterSlaveMode)
  {
    /* Disable slave */
    I2Cx->CON.BF.SLAVE_DISABLE = 1;

    /* Enable master */
    I2Cx->CON.BF.MASTER_MODE = 1;

    /* Set I2C start its transfer in 7 or 10 bits address mode */
    I2Cx->TAR.BF.BITADDR10_MASTER = i2cCfgStruct->masterAddrBitMode;

    /* Set whether RESTART conditions may be sent */
    I2Cx->CON.BF.RESTART_EN = ((i2cCfgStruct->restartEnable == ENABLE)? 1 : 0);
  }
  else
  {
    /* Enable slave */
    I2Cx->CON.BF.SLAVE_DISABLE = 0;

    /* Disable master */
    I2Cx->CON.BF.MASTER_MODE = 0;

    /* Set I2C slave address mode */
    I2Cx->CON.BF.BITADDR10_SLAVE = i2cCfgStruct->slaveAddrBitMode;

    /* Set I2C slave address */
    I2Cx->SAR.BF.SAR = i2cCfgStruct->ownSlaveAddr;
  }
}

/****************************************************************************//**
 * @brief      Initializes the I2C DMA
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 * @param[in]  i2cDmaCfg:  Pointer to a I2C DMA configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void I2C_DmaConfig(I2C_ID_Type i2cNo, I2C_DmaReqLevel_Type* i2cDmaCfg)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));

  /* Set Transmit data level */
  I2Cx->DMA_TDLR.BF.DMATDL = i2cDmaCfg->I2C_DMA_TransmitReqLevel;

  /* Set Receive data level */
  I2Cx->DMA_RDLR.BF.DMARDL = i2cDmaCfg->I2C_DMA_RecvReqLevel;
}

/****************************************************************************//**
 * @brief      Set I2c high speed master code address
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 * @param[in]  hsMAddr:  I2c high speed master code address
 *
 * @return none
 *
 *******************************************************************************/
void I2C_SetHsMAddr(I2C_ID_Type i2cNo, uint32_t hsMAddr)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameter */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));

  /* Set i2c high speed master code address */
  I2Cx->HS_MADDR.BF.HS_MAR = hsMAddr;
}

/****************************************************************************//**
 * @brief      Set the slave address when I2C is acted as a master.
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 * @param[in]  slaveAddr:  Slave address. Address format is depended by I2C
 *             initial configuration.
 *
 * @return none
 *
 *******************************************************************************/
void I2C_SetSlaveAddr(I2C_ID_Type i2cNo, uint32_t slaveAddr)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));

  /* Set the slave address */
  I2Cx->TAR.BF.TAR = slaveAddr;
}

/****************************************************************************//**
 * @brief      Enable I2C .
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 *
 * @return none
 *
 *******************************************************************************/
void I2C_Enable(I2C_ID_Type i2cNo)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));

  /* Enable the I2C */
  I2Cx->ENABLE.BF.ENABLE = 1;
}

/****************************************************************************//**
 * @brief      Disable I2C .
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 *
 * @return none
 *
 *******************************************************************************/
void I2C_Disable(I2C_ID_Type i2cNo)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));

  /* Disable the I2C */
  I2Cx->ENABLE.BF.ENABLE = 0;
}

/****************************************************************************//**
 * @brief      Cnfigurate the FIFO threshold.
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 * @param[in]  fifoCfg:  Pointer to FIFO type structure.
 *
 * @return none
 *
 *******************************************************************************/
void I2C_FIFOConfig(I2C_ID_Type i2cNo, I2C_FIFO_Type* fifoCfg)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));

  /* Set Receive FIFO threshold level */
  I2Cx->RX_TL.BF.RX_TL = fifoCfg->recvFifoThr;

  /* Set Transmit FIFO threshold level */
  I2Cx->TX_TL.BF.TX_TL = fifoCfg->transmitFifoThr;
}

/****************************************************************************//**
 * @brief      Enable/disable I2C DMA function.
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 * @param[in]  transmitDmaCtrl: Transmit DMA Enable/Disable function state
 * @param[in]  rcvDmaCtrl: Receive DMA Enable/Disable function state
 *
 * @return none
 *
 *******************************************************************************/
void I2C_DmaCmd(I2C_ID_Type i2cNo, FunctionalState transmitDmaCtrl, FunctionalState rcvDmaCtrl)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));
  CHECK_PARAM(PARAM_FUNCTIONALSTATE(transmitDmaCtrl));
  CHECK_PARAM(PARAM_FUNCTIONALSTATE(rcvDmaCtrl));

  /* Enable/Disable the I2C DMA transmit function */
  I2Cx->DMA_CR.BF.TDMAE = ((transmitDmaCtrl == ENABLE)? 1 : 0);

  /* Enable/Disable the I2C DMA receive function */
  I2Cx->DMA_CR.BF.RDMAE = ((rcvDmaCtrl == ENABLE)? 1 : 0);
}

/****************************************************************************//**
 * @brief      Enable/disable I2C general call function.
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 * @param[in]  clkCnt: Pointer to speed clock count type structure.
 *
 * @return none
 *
 *******************************************************************************/
void I2C_SetClkCount(I2C_ID_Type i2cNo, I2C_SpeedClkCnt_Type* clkCnt)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));

  /* Set standard speed clock count */
  I2Cx->SS_SCL_HCNT.BF.SS_SCL_HCNT = clkCnt->stdSpeedClkHighCnt;
  I2Cx->SS_SCL_LCNT.BF.SS_SCL_LCNT = clkCnt->stdSpeedClkLowCnt;

  /* Set fast speed clock count */
  I2Cx->FS_SCL_HCNT.BF.FS_SCL_HCNT = clkCnt->fastSpeedClkHighCnt;
  I2Cx->FS_SCL_LCNT.BF.FS_SCL_LCNT = clkCnt->fastSpeedClkLowCnt;

  /* Set high speed clock count */
  I2Cx->HS_SCL_HCNT.BF.HS_SCL_HCNT = clkCnt->highSpeedClkHighCnt;
  I2Cx->HS_SCL_LCNT.BF.HS_SCL_LCNT = clkCnt->highSpeedClkLowCnt;
}

/****************************************************************************//**
 * @brief      Enable/disable I2C general call function.
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 * @param[in]  newState: Enable/Disable function state
 *
 * @return none
 *
 *******************************************************************************/
void I2C_GeneralCall(I2C_ID_Type i2cNo, FunctionalState newState)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));
  CHECK_PARAM(PARAM_FUNCTIONALSTATE(newState));

  /* Set general call function */
  I2Cx->TAR.BF.SPECIAL = ((newState  == ENABLE)? 1 : 0);

  if(newState == ENABLE)
  {
    I2Cx->TAR.BF.GC_OR_START = 0;
  }
}

/****************************************************************************//**
 * @brief      Enable/disable I2C START BYTE function.
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 * @param[in]  newState: Enable/Disable function state
 *
 * @return none
 *
 *******************************************************************************/
void I2C_StartByteCmd(I2C_ID_Type i2cNo, FunctionalState newState)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));
  CHECK_PARAM(PARAM_FUNCTIONALSTATE(newState));

  /* Set general call function */
  I2Cx->TAR.BF.SPECIAL = ((newState  == ENABLE)? 1 : 0);

  if(newState == ENABLE)
  {
    I2Cx->TAR.BF.GC_OR_START = 1;
  }
}

/****************************************************************************//**
 * @brief      Enable/disable I2C general call ACK function.
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 * @param[in]  newState: Enable/Disable function state
 *             - ENABLE: I2C responses with a ACK when it receives a general call.
 *             - Disable: not ACK the general call and doesn't generate general call
 *             interrupts
 *
 * @return none
 *
 *******************************************************************************/
void I2C_GeneralCallAckCmd(I2C_ID_Type i2cNo, FunctionalState newState)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));
  CHECK_PARAM(PARAM_FUNCTIONALSTATE(newState));

  /* Set general call ACK function */
  I2Cx->ACK_GENERAL_CALL.BF.ACK_GEN_CALL = ((newState  == ENABLE)? 1 : 0);
}

/****************************************************************************//**
 * @brief      Send one byte.
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 * @param[in]  data: The data to be send
  *
 * @return none
 *
 *******************************************************************************/
void I2C_SendByte(I2C_ID_Type i2cNo, uint8_t data)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));

  /* Send one byte */
  I2Cx->DATA_CMD.WORDVAL = (((uint32_t)data) & 0xFF);
}

/****************************************************************************//**
 * @brief      Master read request .
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 *
 * @return none
 *
 *******************************************************************************/
void I2C_MasterReadCmd(I2C_ID_Type i2cNo)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));

  /* Set read command */
  I2Cx->DATA_CMD.WORDVAL = 0x100;
}

/****************************************************************************//**
 * @brief      I2c read one byte .
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 *
 * @return The received data
 *
 *******************************************************************************/
uint8_t I2C_ReceiveByte(I2C_ID_Type i2cNo)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));

  /* Read one byte */
  return((uint8_t)(I2Cx->DATA_CMD.WORDVAL & 0xFF));
}


/****************************************************************************//**
 * @brief      Mask/Unmask specified interrupt type
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 * @param[in]  intType:  Specified interrupt type
 *             - I2C_INT_RX_UNDER
 *             - I2C_INT_RX_OVER
 *             - I2C_INT_RX_FULL
 *             - I2C_INT_TX_OVER
 *             - I2C_INT_TX_EMPTY
 *             - I2C_INT_RD_REQ
 *             - I2C_INT_TX_ABORT
 *             - I2C_INT_RX_DONE
 *             - I2C_INT_ACTIVITY
 *             - I2C_INT_STOP_DET
 *             - I2C_INT_START_DET
 *             - I2C_INT_GEN_CALL
 *
 * @param[in]  intMask:  Interrupt mask/unmask type
 *             - I2C_INT_MASK
 *             - I2C_INT_UNMASK
 *
 * @return     The state flag of raw interrupt Status register.
 *             - SET
 *             - RESET
 *
 *******************************************************************************/
void I2C_IntMask(I2C_ID_Type i2cNo, I2C_INT_Type intType, IntMask_Type intMask)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));
  CHECK_PARAM(IS_I2C_INT_TYPE(intType));
  CHECK_PARAM(IS_INTMASK(intMask));

  switch(intType)
  {
    case I2C_INT_RX_UNDER:
      I2Cx->INTR_MASK.BF.M_RX_UNDER = ((intMask == MASK)? 0 : 1);
      break;

    case I2C_INT_RX_OVER:
      I2Cx->INTR_MASK.BF.M_RX_OVER = ((intMask == MASK)? 0 : 1);
      break;

    case I2C_INT_RX_FULL:
      I2Cx->INTR_MASK.BF.M_RX_FULL = ((intMask == MASK)? 0 : 1);
      break;

    case I2C_INT_TX_OVER:
      I2Cx->INTR_MASK.BF.M_TX_OVER = ((intMask == MASK)? 0 : 1);
      break;

    case I2C_INT_TX_EMPTY:
      I2Cx->INTR_MASK.BF.M_TX_EMPTY = ((intMask == MASK)? 0 : 1);
      break;

    case I2C_INT_RD_REQ:
      I2Cx->INTR_MASK.BF.M_RD_REQ = ((intMask == MASK)? 0 : 1);
      break;

    case I2C_INT_TX_ABORT:
      I2Cx->INTR_MASK.BF.M_TX_ABRT = ((intMask == MASK)? 0 : 1);
      break;

    case I2C_INT_RX_DONE:
      I2Cx->INTR_MASK.BF.M_RX_DONE = ((intMask == MASK)? 0 : 1);
      break;

    case I2C_INT_ACTIVITY:
      I2Cx->INTR_MASK.BF.M_ACTIVITY = ((intMask == MASK)? 0 : 1);
      break;

    case I2C_INT_STOP_DET:
      I2Cx->INTR_MASK.BF.M_STOP_DET = ((intMask == MASK)? 0 : 1);
      break;

    case I2C_INT_START_DET:
      I2Cx->INTR_MASK.BF.M_START_DET = ((intMask == MASK)? 0 : 1);
      break;

    case I2C_INT_GEN_CALL:
      I2Cx->INTR_MASK.BF.M_GEN_CALL = ((intMask == MASK)? 0 : 1);
      break;

    case I2C_INT_ALL:
      I2Cx->INTR_MASK.WORDVAL &= 0xf000;

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      Check whether interrupt status flag is set or not for given
 *             interrupt type
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 * @param[in]  intType:  Specified interrupt type
 *             - I2C_INT_RX_UNDER
 *             - I2C_INT_RX_OVER
 *             - I2C_INT_RX_FULL
 *             - I2C_INT_TX_OVER
 *             - I2C_INT_TX_EMPTY
 *             - I2C_INT_RD_REQ
 *             - I2C_INT_TX_ABORT
 *             - I2C_INT_RX_DONE
 *             - I2C_INT_ACTIVITY
 *             - I2C_INT_STOP_DET
 *             - I2C_INT_START_DET
 *             - I2C_INT_GEN_CALL
 *
 * @return     The state flag of raw interrupt Status register.
 *             - SET
 *             - RESET
 *
 *******************************************************************************/
FlagStatus I2C_GetIntStatus(I2C_ID_Type i2cNo, I2C_INT_Type intType)
{
  FlagStatus intBitStatus = RESET;

  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));
  CHECK_PARAM(IS_I2C_INT_TYPE(intType));

  switch(intType)
  {
    case I2C_INT_RX_UNDER:
      intBitStatus = (I2Cx->INTR_STAT.BF.R_RX_UNDER? SET : RESET);
      break;

    case I2C_INT_RX_OVER:
      intBitStatus = (I2Cx->INTR_STAT.BF.R_RX_OVER? SET : RESET);
      break;

    case I2C_INT_RX_FULL:
      intBitStatus = (I2Cx->INTR_STAT.BF.R_RX_FULL? SET : RESET);
      break;

    case I2C_INT_TX_OVER:
      intBitStatus = (I2Cx->INTR_STAT.BF.R_TX_OVER? SET : RESET);
      break;

    case I2C_INT_TX_EMPTY:
      intBitStatus = (I2Cx->INTR_STAT.BF.R_TX_EMPTY? SET : RESET);
      break;

    case I2C_INT_RD_REQ:
      intBitStatus = (I2Cx->INTR_STAT.BF.R_RD_REQ? SET : RESET);
      break;

    case I2C_INT_TX_ABORT:
      intBitStatus = (I2Cx->INTR_STAT.BF.R_TX_ABRT? SET : RESET);
      break;

    case I2C_INT_RX_DONE:
      intBitStatus = (I2Cx->INTR_STAT.BF.R_RX_DONE? SET : RESET);
      break;

    case I2C_INT_ACTIVITY:
      intBitStatus = (I2Cx->INTR_STAT.BF.R_ACTIVITY? SET : RESET);
      break;

    case I2C_INT_STOP_DET:
      intBitStatus = (I2Cx->INTR_STAT.BF.R_STOP_DET? SET : RESET);
      break;

    case I2C_INT_START_DET:
      intBitStatus = (I2Cx->INTR_STAT.BF.R_START_DET? SET : RESET);
      break;

    case I2C_INT_GEN_CALL:
      intBitStatus = (I2Cx->INTR_STAT.BF.R_GEN_CALL? SET : RESET);
      break;

    case I2C_INT_ALL:
      intBitStatus = ((I2Cx->INTR_STAT.WORDVAL & 0xFFF)? SET : RESET);
      break;

    default:
      break;
  }

  return intBitStatus;
}

/****************************************************************************//**
 * @brief      Check whether abort status flag is set or not for given
 *             abort status type
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 * @param[in]  abortType:  Specified abort type
 *             - ABORT_7B_ADDR_NOACK
 *             - ABORT_10ADDR1_NOACK
 *             - ABORT_10ADDR2_NOACK
 *             - ABORT_TXDATA_NOACK
 *             - ABORT_GCALL_NOACK
 *             - ABORT_GCALL_READ
 *             - ABORT_SBYTE_ACKDET
 *             - ABORT_HS_NORSTRT
 *             - ABORT_SBYTE_NORSTRT
 *             - ABORT_10B_RD_NORSTRT
 *             - ABORT_MASTER_DIS
 *             - ABORT_ARB_LOST
 *             - ABORT_SLVFLUSH_TXFIFO
 *             - ABORT_SLV_ARBLOST
 *             - ABORT_SLVRD_INTX
 *
 * @return     The state flag of raw interrupt Status register.
 *             - SET
 *             - RESET
 *
 *******************************************************************************/
FlagStatus I2C_GetTxAbortStatus(I2C_ID_Type i2cNo, I2C_TxAbortINT_Type abortType)
{
  FlagStatus bitStatus = RESET;

  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));
  CHECK_PARAM(IS_I2C_ABORT_TYPE(abortType) );

  switch(abortType)
  {
    case ABORT_7B_ADDR_NOACK:
      bitStatus = (I2Cx->TX_ABRT_SOURCE.BF.ABRT_7B_ADDR_NOACK? SET : RESET);
      break;

    case ABORT_10ADDR1_NOACK:
      bitStatus = (I2Cx->TX_ABRT_SOURCE.BF.ABRT_10ADDR1_NOACK? SET : RESET);
      break;

    case ABORT_10ADDR2_NOACK:
      bitStatus = (I2Cx->TX_ABRT_SOURCE.BF.ABRT_10ADDR2_NOACK? SET : RESET);
      break;

    case ABORT_TXDATA_NOACK:
      bitStatus = (I2Cx->TX_ABRT_SOURCE.BF.ABRT_TXDATA_NOACK? SET : RESET);
      break;

    case ABORT_GCALL_NOACK:
      bitStatus = (I2Cx->TX_ABRT_SOURCE.BF.ABRT_GCALL_NOACK? SET : RESET);
      break;

    case ABORT_GCALL_READ:
      bitStatus = (I2Cx->TX_ABRT_SOURCE.BF.ABRT_GCALL_READ? SET : RESET);
      break;

    case ABORT_SBYTE_ACKDET:
      bitStatus = (I2Cx->TX_ABRT_SOURCE.BF.ABRT_SBYTE_ACKDET? SET : RESET);
      break;

    case ABORT_HS_NORSTRT:
      bitStatus = (I2Cx->TX_ABRT_SOURCE.BF.ABRT_HS_NORSTRT? SET : RESET);
      break;

    case ABORT_SBYTE_NORSTRT:
      bitStatus = (I2Cx->TX_ABRT_SOURCE.BF.ABRT_SBYTE_NORSTRT? SET : RESET);
      break;

    case ABORT_10B_RD_NORSTRT:
      bitStatus = (I2Cx->TX_ABRT_SOURCE.BF.ABRT_10B_RD_NORSTRT? SET : RESET);
      break;

    case ABORT_MASTER_DIS:
      bitStatus = (I2Cx->TX_ABRT_SOURCE.BF.ABRT_MASTER_DIS? SET : RESET);
      break;

    case ABORT_ARB_LOST:
      bitStatus = (I2Cx->TX_ABRT_SOURCE.BF.ARB_LOST? SET : RESET);
      break;

    case ABORT_SLVFLUSH_TXFIFO:
      bitStatus = (I2Cx->TX_ABRT_SOURCE.BF.ABRT_SLVFLUSH_TXFIFO? SET : RESET);
      break;

    case ABORT_SLV_ARBLOST:
      bitStatus = (I2Cx->TX_ABRT_SOURCE.BF.ABRT_SLV_ARBLOST? SET : RESET);
      break;

    case ABORT_SLVRD_INTX:
      bitStatus = (I2Cx->TX_ABRT_SOURCE.BF.ABRT_SLVRD_INTX? SET : RESET);
      break;

    default:
      break;
  }

  return bitStatus;
}

/****************************************************************************//**
 * @brief      Clear specified interrupt type
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 * @param[in]  intType:  Specified interrupt type
 *             - I2C_INT_RX_UNDER
 *             - I2C_INT_RX_OVER
 *             - I2C_INT_TX_OVER
 *             - I2C_INT_RD_REQ
 *             - I2C_INT_TX_ABORT
 *             - I2C_INT_RX_DONE
 *             - I2C_INT_ACTIVITY
 *             - I2C_INT_STOP_DET
 *             - I2C_INT_START_DET
 *             - I2C_INT_GEN_CALL
 *             - I2C_INT_ALL
 *
 * @return     none
 *
 *******************************************************************************/
void I2C_IntClr(I2C_ID_Type i2cNo, I2C_INT_Type intType)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);
	__UNUSED__ volatile uint32_t dummyData;

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));
  CHECK_PARAM(IS_I2C_INT_TYPE(intType));

  switch(intType)
  {
    case I2C_INT_RX_UNDER:
      /* Dummy read to clear */
      dummyData = I2Cx->CLR_RX_UNDER.BF.CLR_RX_UNDER;
      break;

    case I2C_INT_RX_OVER:
      /* Dummy read to clear */
      dummyData = I2Cx->CLR_RX_OVER.BF.CLR_RX_OVER;
      break;

    case I2C_INT_TX_OVER:
      /* Dummy read to clear */
      dummyData = I2Cx->CLR_TX_OVER.BF.CLR_TX_OVER;
      break;

    case I2C_INT_RD_REQ:
      /* Dummy read to clear */
      dummyData = I2Cx->CLR_RD_REQ.BF.CLR_RD_REQ;
      break;

    case I2C_INT_TX_ABORT:
      /* Dummy read to clear */
      dummyData = I2Cx->CLR_TX_ABRT.BF.CLR_TX_ABRT;
      break;

    case I2C_INT_RX_DONE:
      /* Dummy read to clear */
      dummyData = I2Cx->CLR_RX_DONE.BF.CLR_RX_DONE;
      break;

    case I2C_INT_ACTIVITY:
      /* Dummy read to clear */
      dummyData = I2Cx->CLR_ACTIVITY.BF.CLR_ACTIVITY;
      break;

    case I2C_INT_STOP_DET:
      /* Dummy read to clear */
      dummyData = I2Cx->CLR_STOP_DET.BF.CLR_STOP_DET;
      break;

    case I2C_INT_START_DET:
      /* Dummy read to clear */
      dummyData = I2Cx->CLR_START_DET.BF.CLR_START_DET;
      break;

    case I2C_INT_GEN_CALL:
      /* Dummy read to clear */
      dummyData = I2Cx->CLR_GEN_CALL.BF.CLR_GEN_CALL;
      break;

    case I2C_INT_ALL:
      /* Dummy read to clear */
      dummyData = I2Cx->CLR_INTR.BF.CLR_INTR;
      break;

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      Check whether status flag is set or not for given status type
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 * @param[in]  statusType:  Specified status type
 *             - I2C_STATUS_ACTIVITY
 *             - I2C_STATUS_TFNF
 *             - I2C_STATUS_TFE
 *             - I2C_STATUS_RFNE
 *             - I2C_STATUS_RFF
 *             - I2C_STATUS_MST_ACTIVITY
 *             - I2C_STATUS_SLV_ACTIVITY
 *
 * @return     The state flag of Status register.
 *             - SET
 *             - RESET
 *
 *******************************************************************************/
FlagStatus I2C_GetStatus(I2C_ID_Type i2cNo, I2C_STATUS_Type statusType)
{
  FlagStatus bitStatus = RESET;

  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));
  CHECK_PARAM(IS_I2C_STATUS_BIT(statusType));

  switch(statusType)
  {
    case I2C_STATUS_ACTIVITY:
      bitStatus = (I2Cx->STATUS.BF.ACTIVITY? SET : RESET);
      break;

    case I2C_STATUS_TFNF:
      bitStatus = (I2Cx->STATUS.BF.TFNF? SET : RESET);
      break;

    case I2C_STATUS_TFE:
      bitStatus = (I2Cx->STATUS.BF.TFE? SET : RESET);
      break;

    case I2C_STATUS_RFNE:
      bitStatus = (I2Cx->STATUS.BF.RFNE? SET : RESET);
      break;

    case I2C_STATUS_RFF:
      bitStatus = (I2Cx->STATUS.BF.RFF? SET : RESET);
      break;

    case I2C_STATUS_MST_ACTIVITY:
      bitStatus = (I2Cx->STATUS.BF.MST_ACTIVITY? SET : RESET);
      break;

    case I2C_STATUS_SLV_ACTIVITY:
      bitStatus = (I2Cx->STATUS.BF.SLV_ACTIVITY? SET : RESET);
      break;

    default:
      break;
  }

  return bitStatus;
}

/* Commenting out ISR function here to handle it in mdev driver directly */
#if 0
/****************************************************************************//**
 * @brief  I2C0 interrupt function
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void I2C0_IRQHandler(void)
{
  I2C_IntHandler(INT_I2C0, I2C0_PORT);
}

/****************************************************************************//**
 * @brief  I2C1 interrupt function
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void I2C1_IRQHandler(void)
{
  I2C_IntHandler(INT_I2C1, I2C1_PORT);
}
#endif

/****************************************************************************//**
 * @brief     Check the exact cause for TX_ABORT
 *
 * /param[in] uint32_t* tx_abrt_reg reads the tx abort cause
 *
 *
 *
 * @return    none
 *
 *******************************************************************************/
void I2C_GetTxAbortCause(I2C_ID_Type i2cNo, uint32_t *tx_abrt_reg)
{
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);
  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));

  *tx_abrt_reg = I2Cx->TX_ABRT_SOURCE.WORDVAL;
}

/****************************************************************************//**
 * @brief      Check if specified interrupt type has occurred in RAW Interrupt
 * 	       status register
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 * @param[in]  intType:  Specified interrupt type
 *             - I2C_INT_RX_UNDER
 *             - I2C_INT_RX_OVER
 *             - I2C_INT_TX_OVER
 *             - I2C_INT_RD_REQ
 *             - I2C_INT_TX_ABORT
 *             - I2C_INT_RX_DONE
 *             - I2C_INT_ACTIVITY
 *             - I2C_INT_STOP_DET
 *             - I2C_INT_START_DET
 *             - I2C_INT_GEN_CALL
 *             - I2C_INT_ALL
 *
 * @return     The state flag of Status register.
 *             - SET
 *             - RESET

 *******************************************************************************/
FlagStatus I2C_GetRawIntStatus(I2C_ID_Type i2cNo, I2C_INT_Type intType)
{
  FlagStatus intBitStatus = RESET;
  i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_I2C_PERIPH(i2cNo));

  CHECK_PARAM(IS_I2C_INT_TYPE(intType));

  switch(intType)
  {
    case I2C_INT_RX_UNDER:
      intBitStatus = (I2Cx->RAW_INTR_STAT.BF.RX_UNDER ? SET : RESET);
	break;

    case I2C_INT_RX_OVER:
      intBitStatus = (I2Cx->RAW_INTR_STAT.BF.RX_OVER ? SET : RESET);
      break;

    case I2C_INT_RX_FULL:
      intBitStatus = (I2Cx->RAW_INTR_STAT.BF.RX_FULL ? SET : RESET);
      break;

    case I2C_INT_TX_OVER:
      intBitStatus = (I2Cx->RAW_INTR_STAT.BF.TX_OVER ? SET : RESET);
      break;

    case I2C_INT_TX_EMPTY:
      intBitStatus = (I2Cx->RAW_INTR_STAT.BF.TX_EMPTY ? SET : RESET);
      break;

    case I2C_INT_RD_REQ:
      intBitStatus = (I2Cx->RAW_INTR_STAT.BF.RD_REQ ? SET : RESET);
      break;

    case I2C_INT_TX_ABORT:
      intBitStatus = (I2Cx->RAW_INTR_STAT.BF.TX_ABRT ? SET : RESET);
      break;

    case I2C_INT_RX_DONE:
      intBitStatus = (I2Cx->RAW_INTR_STAT.BF.RX_DONE ? SET : RESET);
      break;

    case I2C_INT_ACTIVITY:
      intBitStatus = (I2Cx->RAW_INTR_STAT.BF.ACTIVITY ? SET : RESET);
      break;

    case I2C_INT_STOP_DET:
      intBitStatus = (I2Cx->RAW_INTR_STAT.BF.STOP_DET ? SET : RESET);
      break;

    case I2C_INT_START_DET:
      intBitStatus = (I2Cx->RAW_INTR_STAT.BF.START_DET ? SET : RESET);
      break;

    case I2C_INT_GEN_CALL:
      intBitStatus = (I2Cx->RAW_INTR_STAT.BF.GEN_CALL ? SET : RESET);
      break;

    case I2C_INT_ALL:
      intBitStatus = ((I2Cx->RAW_INTR_STAT.WORDVAL & 0xFFF) ? SET : RESET);
      break;

    default:
      break;
  }

	return intBitStatus;
}

/******************************************************************************
 * @brief      Return I2C interrupt status
 *
 * @param[in]  i2cNo:  Select the I2C port, should be I2C0_PORT, I2C1_PORT.
 *
 * @return none
 *
 ******************************************************************************/
uint32_t I2C_GetFullIntStatus(I2C_ID_Type i2cNo)
{
	i2c_reg_t * I2Cx = (i2c_reg_t *)(i2cAddr[i2cNo]);

	/* Check the parameters */
	CHECK_PARAM(IS_I2C_PERIPH(i2cNo));

	/* Read and return the interrupt status */
	return I2Cx->INTR_STAT.WORDVAL;
}

/*@} end of group I2C_Public_Functions */

/*@} end of group I2C_definitions */

/*@} end of group MW300_Periph_Driver */
