/** @file     mw300_qspi.h
 *
 *  @brief     This file provides QSPI driver module header file.
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
#include "mw300_qspi.h"

/** @addtogroup MW300_Periph_Driver
 *  @{
 */

/** @defgroup QSPI QSPI
 *  @brief QSPI driver modules
 *  @{
 */

/** @defgroup QSPI_Private_Type
 *  @{
 */

/*@} end of group QSPI_Private_Type*/

/** @defgroup QSPI_Private_Defines
 *  @{
 */

/*@} end of group QSPI_Private_Defines */

/** @defgroup QSPI_Private_Variables
 *  @{
 */

/*@} end of group QSPI_Private_Variables */

/** @defgroup QSPI_Global_Variables
 *  @{
 */

/*@} end of group QSPI_Global_Variables */


/** @defgroup QSPI_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group QSPI_Private_FunctionDeclaration */

/** @defgroup QSPI_Private_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief  QSPI interrupt function 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void QSPI_IRQHandler(void)
{
  uint32_t temp;
  
  /* Store interrupt flags for later use */
  temp = QSPI->ISR.WORDVAL;
  
  /* Clear all interrupt flags */
  QSPI->ISC.WORDVAL = 0x1;
  
  /* QSPI transfer done inerrupt */
  if( temp & (1 << QSPI_XFER_DONE) )
  {
    if(intCbfArra[INT_QSPI][QSPI_XFER_DONE] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI][QSPI_XFER_DONE]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI->IMR.BF.XFER_DONE_IM = 1;
    }    
  }

  /* QSPI transfer ready interrupt */
  if( temp & (1 << QSPI_XFER_RDY) )
  {
    if(intCbfArra[INT_QSPI][QSPI_XFER_RDY] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI][QSPI_XFER_RDY]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI->IMR.BF.XFER_RDY_IM = 1;
    }
  }
  
  /* QSPI read fifo dma burst interrupt */
  if( temp & (1 << QSPI_RFIFO_DMA_BURST) )
  {
    if(intCbfArra[INT_QSPI][QSPI_RFIFO_DMA_BURST] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI][QSPI_RFIFO_DMA_BURST]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI->IMR.BF.RFIFO_DMA_BURST_IM = 1;
    }
  }  
 
  /* QSPI write fifo dma burst interrupt */
  if( temp & (1 << QSPI_WFIFO_DMA_BURST) )
  {
    if(intCbfArra[INT_QSPI][QSPI_WFIFO_DMA_BURST] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI][QSPI_WFIFO_DMA_BURST]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI->IMR.BF.WFIFO_DMA_BURST_IM = 1;
    } 
  }   
  
  /* QSPI read fifo empty interrupt */
  if( temp & (1 << QSPI_RFIFO_EMPTY) )
  {
    if(intCbfArra[INT_QSPI][QSPI_RFIFO_EMPTY] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI][QSPI_RFIFO_EMPTY]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI->IMR.BF.RFIFO_EMPTY_IM = 1;
    }
  }   
  
  /* QSPI read fifo full inerrupt */
  if( temp & (1 << QSPI_RFIFO_FULL) )
  {
    if(intCbfArra[INT_QSPI][QSPI_RFIFO_FULL] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI][QSPI_RFIFO_FULL]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI->IMR.BF.RFIFO_FULL_IM = 1;
    } 
  }   

  /* QSPI write fifo empty interrupt */
  if( temp & (1 << QSPI_WFIFO_EMPTY) )
  {
    if(intCbfArra[INT_QSPI][QSPI_WFIFO_EMPTY] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI][QSPI_WFIFO_EMPTY]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI->IMR.BF.WFIFO_EMPTY_IM = 1;
    } 
  }   
  
  /* QSPI write fifo full interrupt */
  if( temp & (1 << QSPI_WFIFO_FULL) )
  {
    if(intCbfArra[INT_QSPI][QSPI_WFIFO_FULL] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI][QSPI_WFIFO_FULL]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI->IMR.BF.WFIFO_FULL_IM = 1;
    } 
  }   
  
  /* QSPI read fifo underflow interrupt */
  if( temp & (1 << QSPI_RFIFO_UNDRFLW) )
  {
    if(intCbfArra[INT_QSPI][QSPI_RFIFO_UNDRFLW] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI][QSPI_RFIFO_UNDRFLW]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI->IMR.BF.RFIFO_UNDRFLW_IM = 1;
    } 
  }   
  
  /* QSPI read fifo overflow interrupt */
  if( temp & (1 << QSPI_RFIFO_OVRFLW) )
  {
    if(intCbfArra[INT_QSPI][QSPI_RFIFO_OVRFLW] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI][QSPI_RFIFO_OVRFLW]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI->IMR.BF.RFIFO_OVRFLW_IM = 1;
    } 
  }  
  
  /* QSPI read fifo underflow interrupt */
  if( temp & (1 << QSPI_RFIFO_UNDRFLW) )
  {
    if(intCbfArra[INT_QSPI][QSPI_RFIFO_UNDRFLW] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI][QSPI_RFIFO_UNDRFLW]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI->IMR.BF.WFIFO_UNDRFLW_IM = 1;
    } 
  }   
  
  /* QSPI write fifo overflow interrupt */
  if( temp & (1 << QSPI_WFIFO_OVRFLW) )
  {
    if(intCbfArra[INT_QSPI][QSPI_WFIFO_OVRFLW] != NULL)
    {
      /* Call the callback function */
      intCbfArra[INT_QSPI][QSPI_WFIFO_OVRFLW]();
    }
    else
    {
      /* Mask this interrupt */
      QSPI->IMR.BF.WFIFO_OVRFLW_IM = 1;
    } 
  }  
}

/*@} end of group QSPI_Private_Functions */

/** @defgroup QSPI_Public_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief      Reset QSPI
 *
 * @param none
 *
 * @return none
 *
 * Reset QSPI
 *******************************************************************************/
void QSPI_Reset(void)
{
  uint32_t i;
  
  QSPI->CONF2.BF.SRST = 1;
  
  /* Delay */
  for(i=0; i<10; i++);
  
  QSPI->CONF2.BF.SRST = 0;
}

/****************************************************************************//**
 * @brief      Initializes the QSPI 
 *
 * @param[in]  qspiConfigSet:  Pointer to a QSPI configuration structure
 *
 * @return none
 *
 * Initializes the QSPI 
 *******************************************************************************/
void QSPI_Init(QSPI_CFG_Type* qspiConfigSet)
{
  /* Set data pin number */
  QSPI->CONF.BF.DATA_PIN = qspiConfigSet->dataPinMode;
  
  /* Set address pin number */
  QSPI->CONF.BF.ADDR_PIN = qspiConfigSet->addrPinMode;
  
  /* Set QSPI clock mode */
  QSPI->CONF.BF.CLK_PHA = (qspiConfigSet->clkMode) & 1;
  QSPI->CONF.BF.CLK_POL = ((qspiConfigSet->clkMode)>>1) & 1;
  
  /* Set QSPI capture clock edge */
  QSPI->TIMING.BF.CLK_CAPT_EDGE = qspiConfigSet->captEdge;
  
  /* Set data length mode */
  QSPI->CONF.BF.BYTE_LEN = qspiConfigSet->byteLen;
  
  /* Set QSPI clock prescaler */
  QSPI->CONF.BF.CLK_PRESCALE = qspiConfigSet->preScale;
}

/****************************************************************************//**
 * @brief      Mask/Unmask specified interrupt type
 *
 * @param[in]  intType:  Specified interrupt type
 *             - QSPI_XFER_DONE             - QSPI_XFER_RDY
 *             - QSPI_RFIFO_DMA_BURST       - QSPI_WFIFO_DMA_BURST
 *             - QSPI_RFIFO_EMPTY           - QSPI_RFIFO_FULL
 *             - QSPI_WFIFO_EMPTY           - QSPI_WFIFO_FULL
 *             - QSPI_RFIFO_UNDRFLW         - QSPI_RFIFO_OVRFLW
 *             - QSPI_WFIFO_UNDRFLW         - QSPI_WFIFO_OVRFLW
 *             - QSPI_INT_ALL
 * @param[in]  intMask:  Interrupt mask/unmask type
 *             - UNMASK
 *             - MASK
 *
 * @return none
 *
 *******************************************************************************/
void QSPI_IntMask( QSPI_INT_Type intType, IntMask_Type intMask)
{
  /* Check the parameters */
  CHECK_PARAM(IS_QSPI_INT_TYPE(intType));
  CHECK_PARAM(IS_INTMASK(intMask));

  switch(intType)
  {
    case QSPI_XFER_DONE:
      QSPI->IMR.BF.XFER_DONE_IM = ((intMask == MASK)? 1 : 0);      
      break;

    case QSPI_XFER_RDY:
      QSPI->IMR.BF.XFER_RDY_IM = ((intMask == MASK)? 1 : 0);     
      break;

    case QSPI_RFIFO_DMA_BURST:
      QSPI->IMR.BF.RFIFO_DMA_BURST_IM = ((intMask == MASK)? 1 : 0);     
      break;

    case QSPI_WFIFO_DMA_BURST:
      QSPI->IMR.BF.WFIFO_DMA_BURST_IM = ((intMask == MASK)? 1 : 0);      
      break;

    case QSPI_RFIFO_EMPTY:
      QSPI->IMR.BF.RFIFO_EMPTY_IM = ((intMask == MASK)? 1 : 0);      
      break;

    case QSPI_RFIFO_FULL:
      QSPI->IMR.BF.RFIFO_FULL_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_WFIFO_EMPTY:
      QSPI->IMR.BF.WFIFO_EMPTY_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_WFIFO_FULL:
      QSPI->IMR.BF.WFIFO_FULL_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_RFIFO_UNDRFLW:
      QSPI->IMR.BF.RFIFO_UNDRFLW_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_RFIFO_OVRFLW:
      QSPI->IMR.BF.RFIFO_OVRFLW_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_WFIFO_UNDRFLW:
      QSPI->IMR.BF.WFIFO_UNDRFLW_IM = ((intMask == MASK)? 1 : 0);      
      break;
      
    case QSPI_WFIFO_OVRFLW:
      QSPI->IMR.BF.WFIFO_OVRFLW_IM = ((intMask == MASK)? 1 : 0);      
      break;

    case QSPI_INT_ALL:
      QSPI->IMR.WORDVAL = ((intMask == MASK)? 0xFFF : 0);      
      break;

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      Clear transfer done interrupt
 *
 * @param none
 *
 * @return none
 *
 * Clear transfer done interrupt
 *******************************************************************************/
void QSPI_IntClr(void)
{
  QSPI->ISC.WORDVAL = 1;
}

/****************************************************************************//**
 * @brief      Flush Write and Read FIFOs
 *
 * @param none
 *
 * @return none
 *
 * Flush Write and Read FIFOs
 *******************************************************************************/
Status QSPI_FlushFIFO(void)
{
  volatile uint32_t localCnt = 0;

  QSPI->CONF.BF.FIFO_FLUSH = 1;
    
  /* Wait until Write and Read FIFOs are flushed. */ 
  while(localCnt++ < 0xFFFFFFF)
  {
    if( (QSPI->CONF.BF.FIFO_FLUSH) == RESET )
    {
      return DSUCCESS;
    }
  }
  
  return ERROR;    
}

/****************************************************************************//**
 * @brief      Set QSPI serial interface header count 
 *
 * @param[in]  instrcnt:  number of bytes in INSTR register to shift out
 *                  QSPI_INSTR_CNT_0BYTE/QSPI_INSTR_CNT_1BYTE/QSPI_INSTR_CNT_2BYTE
 *
 *             addrcnt:   number of bytes in ADDR register to shift out
 *                  QSPI_ADDR_CNT_0BYTE/QSPI_ADDR_CNT_1BYTE/QSPI_ADDR_CNT_2BYTE/QSPI_ADDR_CNT_3BYTE/QSPI_ADDR_CNT_4BYTE
 *
 *             rmcnt:     number of bytes in RMODE register to shift out
 *                  QSPI_RM_CNT_0BYTE/QSPI_RM_CNT_1BYTE/QSPI_RM_CNT_2BYTE
 *
 *             dummycnt:  number of bytes as dummy to shift out
 *                  QSPI_DUMMY_CNT_0BYTE/QSPI_DUMMY_CNT_1BYTE/QSPI_DUMMY_CNT_2BYTE/QSPI_DUMMY_CNT_3BYTE
 *
 * @return none
 *
 * Set QSPI serial interface header count 
 *******************************************************************************/
void QSPI_SetHdrcnt(QSPI_INSTR_CNT_TYPE instrcnt, QSPI_ADDR_CNT_TYPE addrcnt, QSPI_RM_CNT_TYPE rmcnt, QSPI_DUMMY_CNT_TYPE dummycnt)
{
  /* Check the parameters */
  CHECK_PARAM(IS_QSPI_INSTR_CNT_TYPE(instrcnt));
  CHECK_PARAM(IS_QSPI_ADDR_CNT_TYPE(addrcnt));
  CHECK_PARAM(IS_QSPI_RM_CNT_TYPE(rmcnt));
  CHECK_PARAM(IS_QSPI_DUMMY_CNT_TYPE(dummycnt));  

  QSPI->HDRCNT.BF.INSTR_CNT = instrcnt;
  QSPI->HDRCNT.BF.ADDR_CNT  = addrcnt;
  QSPI->HDRCNT.BF.RM_CNT    = rmcnt;
  QSPI->HDRCNT.BF.DUMMY_CNT = dummycnt;
}

/****************************************************************************//**
 * @brief      Set number of bytes in INSTR register to shift out to the serial interface 
 *
 * @param[in]  instrcnt:   number of bytes in INSTR register to shift out
 *
 * @return none
 *
 * Set number of bytes in INSTR register to shift out to the serial interface 
 *******************************************************************************/
void QSPI_SetInstrCnt(QSPI_INSTR_CNT_TYPE instrcnt)
{
  /* Check the parameter */
  CHECK_PARAM(IS_QSPI_INSTR_CNT_TYPE(instrcnt));

  QSPI->HDRCNT.BF.INSTR_CNT = instrcnt;
}

/****************************************************************************//**
 * @brief      Set number of bytes in ADDR register to shift out to the serial interface 
 *
 * @param[in]  addrcnt:   number of bytes in ADDR register to shift out
 *
 * @return none
 *
 * Set number of bytes in ADDR register to shift out to the serial interface 
 *******************************************************************************/
void QSPI_SetAddrCnt(QSPI_ADDR_CNT_TYPE addrcnt)
{
  /* Check the parameter */
  CHECK_PARAM(IS_QSPI_ADDR_CNT_TYPE(addrcnt));

  QSPI->HDRCNT.BF.ADDR_CNT = addrcnt;
}

/****************************************************************************//**
 * @brief      Set number of bytes in RMODE register to shift out to the serial interface 
 *
 * @param[in]  rmcnt:   number of bytes in RMODE register to shift out
 *
 * @return none
 *
 * Set number of bytes in RMODE register to shift out to the serial interface 
 *******************************************************************************/
void QSPI_SetRModeCnt(QSPI_RM_CNT_TYPE rmcnt)
{
  /* Check the parameter */
  CHECK_PARAM(IS_QSPI_RM_CNT_TYPE(rmcnt));

  QSPI->HDRCNT.BF.RM_CNT = rmcnt;
}

/****************************************************************************//**
 * @brief      Set number of bytes as dummy to shift out to the serial interface 
 *
 * @param[in]  dummycnt:   number of bytes as dummy to shift out
 *
 * @return none
 *
 * Set number of bytes as dummy to shift out to the serial interface
 *******************************************************************************/
void QSPI_SetDummyCnt(QSPI_DUMMY_CNT_TYPE dummycnt)
{
  /* Check the parameter */
  CHECK_PARAM(IS_QSPI_DUMMY_CNT_TYPE(dummycnt));  

  QSPI->HDRCNT.BF.DUMMY_CNT = dummycnt;
}

/****************************************************************************//**
 * @brief      Set QSPI serial interface instruction 
 *
 * @param[in]  instruct:  QSPI serial interface instruction
 *
 * @return none
 *
 * Set QSPI serial interface instruction 
 *******************************************************************************/
void QSPI_SetInstr(uint32_t instruct)
{
  QSPI->INSTR.BF.INSTR = instruct;
}

/****************************************************************************//**
 * @brief      Set QSPI serial interface address 
 *
 * @param[in]  address:  QSPI serial interface address
 *
 * @return none
 *
 * Set QSPI serial interface address  
 *******************************************************************************/
void QSPI_SetAddr(uint32_t address)
{
  QSPI->ADDR.WORDVAL = address;
}

/****************************************************************************//**
 * @brief      Set QSPI serial interface read mode 
 *
 * @param[in]  readMode:  QSPI serial interface read mode
 *
 * @return none
 *
 * Set QSPI serial interface read mode 
 *******************************************************************************/
void QSPI_SetRMode(uint32_t readMode)
{
  QSPI->RDMODE.BF.RMODE = readMode;
}

/****************************************************************************//**
 * @brief      Set number of bytes of data to shift in from the serial interface 
 *
 * @param[in]  count:   number of bytes of data to shift in
 *
 * @return none
 *
 * Set number of bytes of data to shift in from the serial interface
 *******************************************************************************/
void QSPI_SetDInCnt(uint32_t count)
{
  QSPI->DINCNT.BF.DATA_IN_CNT = count;
}

/****************************************************************************//**
 * @brief      Activate or de-activate serial select output 
 *
 * @param[in]  newCmd:   Activate or de-activate
 *
 * @return none
 *
 * Activate or de-activate serial select output
 *******************************************************************************/
 void QSPI_SetSSEnable(FunctionalState newCmd)
{
  QSPI->CNTL.BF.SS_EN = newCmd;
  while(0==QSPI->CNTL.BF.XFER_RDY);
}

/****************************************************************************//**
 * @brief      Start the specified QSPI transfer
 *
 * @param[in]  rw:  read/write transfer
 *
 * @return none
 *
 * Start the QSPI transfer 
 *******************************************************************************/
void QSPI_StartTransfer(QSPI_RW_Type rw)
{
  /* Assert QSPI SS */
  QSPI_SetSSEnable(ENABLE);
  
  /* Set read/write mode */
  QSPI->CONF.BF.RW_EN = rw;
  
  /* Start QSPI */
  QSPI->CONF.BF.XFER_START = 1;
}  

/****************************************************************************//**
 * @brief      Stop QSPI transfer
 *
 * @param none
 *
 * @return none
 *
 * Stop QSPI transfer
 *******************************************************************************/
void QSPI_StopTransfer(void) 
{
  /* Wait until QSPI ready */
  while(QSPI->CNTL.BF.XFER_RDY == 0);
  
  /* Wait until wfifo empty */
  while(QSPI->CNTL.BF.WFIFO_EMPTY == 0);
        
  /* Stop QSPI */
  QSPI->CONF.BF.XFER_STOP = 1;
  
  /* Wait until QSPI release start signal */
  while(QSPI->CONF.BF.XFER_START == 1);
  
  /* De-assert QSPI SS */
  QSPI_SetSSEnable(DISABLE);
}

/****************************************************************************//**
 * @brief      Write a byte to QSPI serial interface
 *
 * @param[in]  byte:  data to be written
 *
 * @return none
 *
 * Write a byte to QSPI serial interface 
 *******************************************************************************/
void QSPI_WriteByte(uint8_t byte)
{
  /* Wait unitl WFIFO is not full*/
  while(QSPI->CNTL.BF.WFIFO_FULL == 1);
  
  QSPI->DOUT.WORDVAL = (byte & 0xFF);
}

/****************************************************************************//**
 * @brief     Read a byte from QSPI serial interface
 *
 * @param none
 *
 * @return    byte from QSPI serial interface
 *
 * Read a byte from QSPI serial interface 
 *******************************************************************************/
uint8_t QSPI_ReadByte(void)
{
  uint8_t data;
  
  /* Wait if RFIFO is empty*/
  while(QSPI->CNTL.BF.RFIFO_EMPTY == 1);
  
  data = (QSPI->DIN.BF.DATA_IN & 0xFF);
  return data;
}

/****************************************************************************//**
 * @brief      Write a word to QSPI serial interface
 *
 * @param[in]  word:  data to be written
 *
 * @return none
 *
 * Write a word to QSPI serial interface 
 *******************************************************************************/
void QSPI_WriteWord(uint32_t word)
{
  /* Wait unitl WFIFO is not full*/
  while(QSPI->CNTL.BF.WFIFO_FULL == 1);
  
  QSPI->DOUT.WORDVAL = word;
}

/****************************************************************************//**
 * @brief     Read a word from QSPI serial interface
 *
 * @param none
 *
 * @return    word from QSPI serial interface
 *
 * Read a word from QSPI serial interface 
 *******************************************************************************/
uint32_t QSPI_ReadWord(void)
{
  /* Wait if RFIFO is empty*/
  while(QSPI->CNTL.BF.RFIFO_EMPTY == 1);
  
  return QSPI->DIN.WORDVAL;
}

/****************************************************************************//**
 * @brief      Check whether status flag is set or not for given status type
 *
 * @param[in]  qspiStatus:  Specified status type
 *             - QSPI_STATUS_XFER_RDY
 *             - QSPI_STATUS_RFIFO_EMPTY
 *             - QSPI_STATUS_RFIFO_FULL
 *             - QSPI_STATUS_WFIFO_EMPTY
 *             - QSPI_STATUS_WFIFO_FULL
 *             - QSPI_STATUS_RFIFO_UNDRFLW
 *             - QSPI_STATUS_RFIFO_OVRFLW
 *             - QSPI_STATUS_WFIFO_UNDRFLW
 *             - QSPI_STATUS_WFIFO_OVRFLW
 *
 * @return The state value of QSPI serial interface control register. 
 *             - SET 
 *             - RESET
 *
 *******************************************************************************/
FlagStatus QSPI_GetStatus(QSPI_Status_Type qspiStatus)
{
  FlagStatus intBitStatus = RESET;
    
  /* Check the parameters */
  CHECK_PARAM(IS_QSPI_STATUS_BIT(qspiStatus));

  switch(qspiStatus)
  {
    case QSPI_STATUS_XFER_RDY:
      intBitStatus = (QSPI->CNTL.BF.XFER_RDY? SET : RESET); 
      break;
      
    case QSPI_STATUS_RFIFO_EMPTY:
      intBitStatus = (QSPI->CNTL.BF.RFIFO_EMPTY? SET : RESET); 
      break;
      
    case QSPI_STATUS_RFIFO_FULL:
      intBitStatus = (QSPI->CNTL.BF.RFIFO_FULL? SET : RESET); 
      break;
      
    case QSPI_STATUS_WFIFO_EMPTY:
      intBitStatus = (QSPI->CNTL.BF.WFIFO_EMPTY? SET : RESET); 
      break;
      
    case QSPI_STATUS_WFIFO_FULL:
      intBitStatus = (QSPI->CNTL.BF.WFIFO_FULL? SET : RESET); 
      break;
      
    case QSPI_STATUS_RFIFO_UNDRFLW:
      intBitStatus = (QSPI->CNTL.BF.RFIFO_UNDRFLW? SET : RESET); 
      break;
      
    case QSPI_STATUS_RFIFO_OVRFLW:
      intBitStatus = (QSPI->CNTL.BF.RFIFO_OVRFLW? SET : RESET); 
      break;
      
    case QSPI_STATUS_WFIFO_UNDRFLW:
      intBitStatus = (QSPI->CNTL.BF.WFIFO_UNDRFLW? SET : RESET); 
      break;
      
    case QSPI_STATUS_WFIFO_OVRFLW:
      intBitStatus = (QSPI->CNTL.BF.WFIFO_OVRFLW? SET : RESET); 
      break;
      
    default:
      break;
  }

  return intBitStatus;
}

/****************************************************************************//**
 * @brief      Check whether int status flag is set or not for given status type
 *
 * @param[in]  qspiIntStatus:  Specified int status type
 *             - QSPI_XFER_DONE             - QSPI_XFER_RDY
 *             - QSPI_RFIFO_DMA_BURST       - QSPI_WFIFO_DMA_BURST
 *             - QSPI_RFIFO_EMPTY           - QSPI_RFIFO_FULL
 *             - QSPI_WFIFO_EMPTY           - QSPI_WFIFO_FULL
 *             - QSPI_RFIFO_UNDRFLW         - QSPI_RFIFO_OVRFLW
 *             - QSPI_WFIFO_UNDRFLW         - QSPI_WFIFO_OVRFLW
 *             - QSPI_INT_ALL
 *
 * @return The state value of QSPI serial interface control register. 
 *             - SET 
 *             - RESET
 *
 *******************************************************************************/
FlagStatus QSPI_GetIntStatus(QSPI_INT_Type qspiIntStatus)
{
  FlagStatus intBitStatus = RESET;
    
  /* Check the parameters */
  CHECK_PARAM(IS_QSPI_INT_TYPE(qspiIntStatus));

  switch(qspiIntStatus)
  {
    case QSPI_XFER_DONE:
      intBitStatus = (QSPI->ISR.BF.XFER_DONE_IS? SET : RESET); 
      break;
      
    case QSPI_XFER_RDY:
      intBitStatus = (QSPI->ISR.BF.XFER_RDY_IS? SET : RESET); 
      break;
      
    case QSPI_RFIFO_DMA_BURST:
      intBitStatus = (QSPI->ISR.BF.RFIFO_DMA_BURST_IS? SET : RESET); 
      break;
      
    case QSPI_WFIFO_DMA_BURST:
      intBitStatus = (QSPI->ISR.BF.WFIFO_DMA_BURST_IS? SET : RESET); 
      break;
      
    case QSPI_RFIFO_EMPTY:
      intBitStatus = (QSPI->ISR.BF.RFIFO_EMPTY_IS? SET : RESET); 
      break;
      
    case QSPI_RFIFO_FULL:
      intBitStatus = (QSPI->ISR.BF.RFIFO_FULL_IS? SET : RESET); 
      break;
      
    case QSPI_WFIFO_EMPTY:
      intBitStatus = (QSPI->ISR.BF.WFIFO_EMPTY_IS? SET : RESET); 
      break;
      
    case QSPI_WFIFO_FULL:
      intBitStatus = (QSPI->ISR.BF.WFIFO_FULL_IS? SET : RESET); 
      break;
      
    case QSPI_RFIFO_UNDRFLW:
      intBitStatus = (QSPI->ISR.BF.RFIFO_UNDRFLW_IS? SET : RESET); 
      break;
      
    case QSPI_RFIFO_OVRFLW:
      intBitStatus = (QSPI->ISR.BF.RFIFO_OVRFLW_IS? SET : RESET); 
      break;
      
    case QSPI_WFIFO_UNDRFLW:
      intBitStatus = (QSPI->ISR.BF.WFIFO_UNDRFLW_IS? SET : RESET); 
      break;
      
    case QSPI_WFIFO_OVRFLW:
      intBitStatus = (QSPI->ISR.BF.WFIFO_OVRFLW_IS? SET : RESET); 
      break;
      
    case QSPI_INT_ALL:
      intBitStatus = ((QSPI->ISR.WORDVAL & 0xFFF)? SET : RESET); 
      break;

  default:
      break;
  }

  return intBitStatus;
}


/****************************************************************************//**
 * @brief      Get serial interface transfer state
 *
 * @param none
 *
 * @return    serial interface transfer state
 *            - SET 
 *            - RESET
 * Get serial interface transfer state
 *******************************************************************************/
FlagStatus QSPI_IsTransferCompleted(void)
{
  return (QSPI->CONF.BF.XFER_START == 0? SET : RESET); 
}

/****************************************************************************//**
 * @brief      Enable or disable the QSPI DMA function and set the burst size
 *
 * @param[in]  dmaCtrl: DMA read or write, QSPI_DMA_READ or QSPI_DMA_WRITE
 * @param[in]  dmaDataCtrl: DMA Diable/Enable and set the data burst size
 *                          - QSPI_DMA_DISABLE
 *                          - QSPI_DMA_1DATA
 *                          - QSPI_DMA_4DATA
 *                          - QSPI_DMA_8DATA
 *
 * @return none
 *
 *******************************************************************************/
void QSPI_DmaCmd(QSPI_DMA_Type dmaCtrl, QSPI_DMA_Data_Type dmaDataCtrl)
{
  CHECK_PARAM(IS_QSPI_DMA_TYPE(dmaCtrl));
  CHECK_PARAM(IS_QSPI_DMA_DATA_TYPE(dmaDataCtrl));

  if(dmaCtrl == QSPI_DMA_READ)
  {
    if(dmaDataCtrl == QSPI_DMA_DISABLE)
    {
      QSPI->CONF2.BF.DMA_RD_EN = 0;
    }
    else
    {
      QSPI->CONF2.BF.DMA_RD_EN = 1;
      QSPI->CONF2.BF.DMA_RD_BURST = dmaDataCtrl;
    }
  }
  else
  {
    if(dmaDataCtrl == QSPI_DMA_DISABLE)
    {
      QSPI->CONF2.BF.DMA_WR_EN = 0;
    }
    else
    {
      QSPI->CONF2.BF.DMA_WR_EN = 1;
      QSPI->CONF2.BF.DMA_WR_BURST = dmaDataCtrl;
    }
  }
}

/*@} end of group QSPI_Public_Functions */

/*@} end of group QSPI_definitions */

/*@} end of group MW300_Periph_Driver */

