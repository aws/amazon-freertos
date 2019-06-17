/** @file     mw300_sdio.c
 *
 *  @brief    This file provides SDIO functions.
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

#include "mw300_sdio.h"
#include "mw300.h"
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @defgroup SDIO SDIO
 *  @brief SDIO driver module
 *  @{
 */

/** @defgroup SDIO_Private_Type
 *  @{
 */

/*@} end of group SDIO_Private_Type*/


/** @defgroup SDIO_Private_Defines
 *  @{
 */

/**
  * @brief  Separate command and transfer mode control for host
  */
#define SDIOC_TRANS_MODE                 (*(volatile uint16_t *)(SDIO_BASE+ 0x0C))
#define SDIOC_CMD                        (*(volatile uint16_t *)(SDIO_BASE+ 0x0E))

/*@} end of group SDIO_Private_Defines */


/** @defgroup SDIO_Private_Variables
 *  @{
 */

/*@} end of group SDIO_Private_Variables */


/** @defgroup SDIO_Global_Variables
 *  @{
 */

/*@} end of group SDIO_Global_Variables */


/** @defgroup SDIO_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group SDIO_Private_FunctionDeclaration */

/** @defgroup SDIO_Private_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      Initializes SDIO module
 *
 * @param[in]  sdioCfgStruct:  Pointer to a SDIO configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_Init(SDIOC_CFG_Type* sdioCfgStruct)
{
  /* Enable/ Disable Clock */
  SDIO->CNTL2.BF.CLKEN = sdioCfgStruct->clkEnable;
  
  /* Enable internal clock */
  SDIO->CNTL2.BF.INTCLKEN = 0x1;
  
  /* Set the data width */
  SDIO->CNTL1.BF._4BITMD = sdioCfgStruct->busWidth;

  /* Set the speed mode */
  SDIO->CNTL1.BF.HISPEED = sdioCfgStruct->speedMode;

  /* Set the bus voltage */
  SDIO->CNTL1.BF.VLTGSEL = sdioCfgStruct->busVoltSel;

  /* Enable/ Disable read wait control */
  SDIO->CNTL1.BF.RDWTCNTL = sdioCfgStruct->readWaitCtrl;

  /* Enable/ Disable interrupt at blosk gap */
  SDIO->CNTL1.BF.BGIRQEN = sdioCfgStruct->blkGapInterrupt;
}

/****************************************************************************//**
 * @brief      Send SDIO command
 *
 * @param[in]  cmd  : Pointer to a SDIO CMD configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_SendCmd(SDIOC_CmdCfg_Type* cmd)
{ 
  uint16_t regVal = 0;
 
  regVal |= cmd->resp;
  regVal |= (cmd->cmdCrcCheck << 3);
  regVal |= (cmd->cmdIndexCheck << 4);
  regVal |= (cmd->dataPreSel << 5);
  regVal |= (cmd->cmdType << 6);
  regVal |= (cmd->cmdIndex << 8);
  
  /* Set the cmd argument */
  SDIO->ARG.BF.ARG = cmd->arg;

  /* Fill upper two bytes of CMD_XFRMD register */    
  SDIOC_CMD = regVal;
}

/****************************************************************************//**
 * @brief      Set SDIO transfer mode
 *
 * @param[in]  transfMode:  Pointer to a SDIO transfer type structure
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_SetTransferMode(SDIOC_Transfer_Type* transfMode)
{ 
  uint16_t regVal = 0;
 
  regVal |= transfMode->dmaEnabe;
  regVal |= (transfMode->blkCntEnable << 1);
  regVal |= (transfMode->transferDir << 4);
  regVal |= (transfMode->blkSel << 5);

  /* Fill lower two bytes of CMD_XFRMD register */ 
  SDIOC_TRANS_MODE = regVal;
  
  /* Set the block count */
  SDIO->BLK_CNTL.BF.BLK_CNT = transfMode->blkCnt;

  /* Set the block size */
  SDIO->BLK_CNTL.BF.XFR_BLKSZ = transfMode->blkSize;

  /* Set the data timeout counter value */
  SDIO->CNTL2.BF.DTOCNTR = transfMode->dataTimeoutCnt;
}

/****************************************************************************//**
 * @brief      Get SDIO command response
 *
 * @param[in]  rpx:  SDIO response number
 *
 * @return SDIO response register
 *
 *******************************************************************************/
uint32_t SDIOC_GetResponse(SDIOC_ResponseNum_Type rpx)
{
  /* Get SDIO response */
  return SDIO->RESP[rpx].WORDVAL;
}

/****************************************************************************//**
 * @brief      Config SDIO DMA
 *
 * @param[in]  dmaCfg:  Pointer to a SDIO DMA configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_DmaConfig(SDIOC_DmaCfg_Type* dmaCfg)
{
  /* Set host DMA buffer size */
  SDIO->BLK_CNTL.BF.DMA_BUFSZ = dmaCfg->dmaBufSize;

  /* Set DMA system address */
  SDIO->SYSADDR.WORDVAL = dmaCfg->dmaSystemAddr;
}

/****************************************************************************//**
 * @brief      Read data from SDIO buffer
 *
 * @param  none
 *
 * @return SDIO buffer data
 *
 *******************************************************************************/
uint32_t SDIOC_ReadData(void)
{
  /* Read SDIO buffer data */
  return SDIO->DP.WORDVAL;
}

/****************************************************************************//**
 * @brief      Write data to SDIO buffer
 *
 * @param[in]  data: transfer data
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_WriteData(uint32_t data)
{
  /* Write data to SDIO buffer */
  SDIO->DP.WORDVAL = data;
}

/****************************************************************************//**
 * @brief      Update the DMA system address
 *
 * @param[in]  addr: DMA system address
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_UpdateSysAddr(uint32_t addr)
{
  /* Update the DMA system address */
  SDIO->SYSADDR.WORDVAL = addr;
}

/****************************************************************************//**
 * @brief      Get the host DMA buffer size
 *
 * @param  none
 *
 * @return DMA buffer size
 *
 *******************************************************************************/
uint32_t SDIOC_GetDmaBufsz(void)
{
  /* Get the host DMA buffer size */
  return (uint32_t)(1 << (SDIO->BLK_CNTL.BF.DMA_BUFSZ + 12));
}

/****************************************************************************//**
 * @brief      Bus power on
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_PowerOn(void)
{
  /* Bus power on */
  SDIO->CNTL1.BF.BUSPWR = 1;
}

/****************************************************************************//**
 * @brief      Bus power off
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_PowerOff(void)
{
  /* Bus power off */
  SDIO->CNTL1.BF.BUSPWR = 0;
}

/****************************************************************************//**
 * @brief      Reset SDIO module
 *
 * @param  none
 *
 * @return Staus
 *
 *******************************************************************************/
Status SDIOC_Reset(void)
{
  volatile uint32_t timeout = 100;
    
  /* Software reset for all */
  SDIO->CNTL2.BF.MSWRST = 1;

  /* Assert the software reset complete */
  while(timeout--)
  {
    if(SDIO->CNTL2.BF.MSWRST == 0)
      return DSUCCESS;
  }
  
  return ERROR;
}

/****************************************************************************//**
 * @brief      Reset SDIO data lines
 *
 * @param  none
 *
 * @return Status
 *
 *******************************************************************************/
Status SDIOC_DataxLineReset(void)
{  
  volatile uint32_t timeout = 100;
  
  /* Softwate reset data lines */
  SDIO->CNTL2.BF.DATSWRST = 1;

  /* Assert the software reset complete */
  while(timeout--)
  {
    if(SDIO->CNTL2.BF.DATSWRST == 0)
      return DSUCCESS;
  }
  
  return ERROR;
}

/****************************************************************************//**
 * @brief      Reset SDIO command line
 *
 * @param  none
 *
 * @return Status
 *
 *******************************************************************************/
Status SDIOC_CmdLineReset(void)
{    
  volatile uint32_t timeout = 100;
    
  /* Software reset command line */
  SDIO->CNTL2.BF.CMDSWRST = 1;

  /* Assert the software reset complete */
  while(timeout--)
  {
    if(SDIO->CNTL2.BF.CMDSWRST == 0)
      return DSUCCESS;
  }
  
  return ERROR;
}

/****************************************************************************//**
 * @brief      Control LED on or off
 *
 * @param[in]  ledCtrl: led control
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_LedCtrl(SDIOC_LED_Type ledCtrl)
{
  /* Led control */
  SDIO->CNTL1.WORDVAL = ledCtrl;
}

/****************************************************************************//**
 * @brief      Get SDIO status
 *
 * @param[in]  statusType:  Status type
 *
 * @return SDIO status
 *
 *******************************************************************************/
FlagStatus SDIOC_GetStatus(SDIOC_STATUS_Type statusType)
{
  FlagStatus status = RESET;

  /* Get the SDIO status */
  switch(statusType)
  {
    /* Command inhibit(CMD) status */
    case SDIOC_STATUS_CCMDINHBT:
      status = ((SDIO->STATE.BF.CCMDINHBT) ? SET : RESET);
      break;

    /* Command inhibit(DAT) status */
    case SDIOC_STATUS_DCMDINHBT:
      status = ((SDIO->STATE.BF.DCMDINHBT) ? SET : RESET);
      break;

    /* DAT line active */
    case SDIOC_STATUS_DATACTV:
      status = ((SDIO->STATE.BF.DATACTV) ? SET : RESET);
      break;

    /* Write transfer active */
    case SDIOC_STATUS_WRACTV:
      status = ((SDIO->STATE.BF.WRACTV) ? SET : RESET);
      break;

    /* Read transfer active */
    case SDIOC_STATUS_RDACTV:
      status = ((SDIO->STATE.BF.RDACTV) ? SET : RESET);
      break;

    /* Buffer write enable */
    case SDIOC_STATUS_BUFWREN:
      status = ((SDIO->STATE.BF.BUFWREN) ? SET : RESET);
      break;

    /* Buffer read enable */
    case SDIOC_STATUS_BUFRDEN:
      status = ((SDIO->STATE.BF.BUFRDEN) ? SET : RESET);
      break;

    /* Card inserted */
    case SDIOC_STATUS_CINDSTD:
      status = ((SDIO->STATE.BF.CDINSTD) ? SET : RESET);
      break;

    /* Card state stable */
    case SDIOC_STATUS_CDSTBL:
      status = ((SDIO->STATE.BF.CDSTBL) ? SET : RESET);
      break;

    /* Card detect pin level */
    case SDIOC_STATUS_CDDETLVL:
      status = ((SDIO->STATE.BF.CDDETLVL) ? SET : RESET);
      break;

    /* Write protect switch pin level */
    case SDIOC_STATUS_WPSWLVL:
      status = ((SDIO->STATE.BF.WPSWLVL) ? SET : RESET);
      break;

    /* Data[0] line signal level */
    case SDIOC_STATUS_LWRDATLVL_DAT0:
      status = ((SDIO->STATE.BF.LWRDATLVL & 0x1) ? SET : RESET);
      break;

    /* Data[1] line signal level */
    case SDIOC_STATUS_LWRDATLVL_DAT1:
      status = ((SDIO->STATE.BF.LWRDATLVL & 0x2) ? SET : RESET);
      break;

    /* Data[2] line signal level */
    case SDIOC_STATUS_LWRDATLVL_DAT2:
      status = ((SDIO->STATE.BF.LWRDATLVL & 0x4) ? SET : RESET);
      break;

    /* Data[3] line signal level */
    case SDIOC_STATUS_LWRDATLVL_DAT3:
      status = ((SDIO->STATE.BF.LWRDATLVL & 0x8) ? SET : RESET);
      break;

    /* CMD line signal level */
    case SDIOC_STATUS_CMDLVL:
      status = ((SDIO->STATE.BF.CMDLVL) ? SET : RESET);
      break;

    /* Data[4] line signal level */
    case SDIOC_STATUS_UPRDATLVL_DAT4:
      status = ((SDIO->STATE.BF.UPRDATLVL & 0x1) ? SET : RESET);
      break;

    /* Data[5] line signal level */
    case SDIOC_STATUS_UPRDATLVL_DAT5:
      status = ((SDIO->STATE.BF.UPRDATLVL & 0x2) ? SET : RESET);
      break;

    /* Data[6] line signal level */
    case SDIOC_STATUS_UPRDATLVL_DAT6:
      status = ((SDIO->STATE.BF.UPRDATLVL & 0x4) ? SET : RESET);
      break;

    /* Data[7] line signal level */
    case SDIOC_STATUS_UPRDATLVL_DAT7:
      status = ((SDIO->STATE.BF.UPRDATLVL & 0x8) ? SET : RESET);
      break;

    default:
      break;
  }

  return status;
}

/****************************************************************************//**
 * @brief      Get SDIO capabilities register value
 *
 * @param  none
 *
 * @return SDIO capabilities register value
 *
 *******************************************************************************/
uint32_t SDIOC_GetCapabilities(void)
{
  /* Get capabilities register value */
  return SDIO->CAP0.WORDVAL;
}

/****************************************************************************//**
 * @brief      Get SDIO interrupt status
 *
 * @param[in]  intType:  Interrupt type
 *
 * @return SDIO interrupt status
 *
 *******************************************************************************/
FlagStatus SDIOC_GetIntStatus(SDIOC_INT_Type intType)
{
  FlagStatus status = RESET;

  /* Get the SDIO interrupt status */
  switch(intType)
  {
    /* Command complete interrupt */
    case SDIOC_INT_CMDCOMP:
      status = ((SDIO->I_STAT.BF.CMDCOMP) ? SET : RESET);
      break;

    /* Transfer complete interrupt */
    case SDIOC_INT_XFRCOMP:
      status = ((SDIO->I_STAT.BF.XFRCOMP) ? SET : RESET);
      break;

    /* Block gap event */
    case SDIOC_INT_BGEVNT:
      status = ((SDIO->I_STAT.BF.BGEVNT) ? SET : RESET);
      break;

    /* DMA interrupt */
    case SDIOC_INT_DMAINT:
      status = ((SDIO->I_STAT.BF.DMAINT) ? SET : RESET);
      break;

    /* Buffer write ready interrupt */
    case SDIOC_INT_BUFWRRDY:
      status = ((SDIO->I_STAT.BF.BUFWRRDY) ? SET : RESET);
      break;

    /* Buffer read ready interrupt */
    case SDIOC_INT_BUFRDRDY:
      status = ((SDIO->I_STAT.BF.BUFRDRDY) ? SET : RESET);
      break;

    /* Card insertion interrupt */
    case SDIOC_INT_CDINS:
      status = ((SDIO->I_STAT.BF.CDINS) ? SET : RESET);
      break;

    /* Card remove interrupt */
    case SDIOC_INT_CDREM:
      status = ((SDIO->I_STAT.BF.CDREM) ? SET : RESET);
      break;

    /* Card interrupt */
    case SDIOC_INT_CDINT:
      status = ((SDIO->I_STAT.BF.CDINT) ? SET : RESET);
      break;

    /* Error interrupt */
    case SDIOC_INT_ERRINT:
      status = ((SDIO->I_STAT.BF.ERRINT) ? SET : RESET);
      break;

    /* Command timeout error interrupt */
    case SDIOC_INT_CTOERR:
      status = ((SDIO->I_STAT.BF.CTOERR) ? SET : RESET);
      break;

    /* Command crc error interrupt */
    case SDIOC_INT_CCRCERR:
      status = ((SDIO->I_STAT.BF.CCRCERR) ? SET : RESET);
      break;

    /* Command end bit error interrupt */
    case SDIOC_INT_CENDERR:
      status = ((SDIO->I_STAT.BF.CENDERR) ? SET : RESET);
      break;

    /* Command index error interrupt */
    case SDIOC_INT_CIDXERR:
      status = ((SDIO->I_STAT.BF.CIDXERR) ? SET : RESET);
      break;

    /* Data timeout error interrupt */
    case SDIOC_INT_DTOERR:
      status = ((SDIO->I_STAT.BF.DTOERR) ? SET : RESET);
      break;

    /* Data crc error interrupt */
    case SDIOC_INT_DCRCERR:
      status = ((SDIO->I_STAT.BF.DCRCERR) ? SET : RESET);
      break;

    /* Data end bit error interrupt */
    case SDIOC_INT_DENDERR:
      status = ((SDIO->I_STAT.BF.DENDERR) ? SET : RESET);
      break;

    /* Current limit error interrupt */
    case SDIOC_INT_ILMTERR:
      status = ((SDIO->I_STAT.BF.ILMTERR) ? SET : RESET);
      break;

    /* AHB Target error interrupt */
    case SDIOC_INT_AHBTERR:
      status = ((SDIO->I_STAT.BF.AHBTERR) ? SET : RESET);
      break;

    /* All interrupt */
    case SDIOC_INT_ALL:
      status = ((SDIO->I_STAT.WORDVAL & 0x10FF81FF) ? SET : RESET);
      break;

  default:
    break;
  }

  return status;
}

/****************************************************************************//**
 * @brief      Mask/ Unmask SDIO interrupt status
 *
 * @param[in]  intType  : Interrupt type
 * @param[in]  maskState: Mask / Unmask control
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_IntMask(SDIOC_INT_Type intType, IntMask_Type maskState)
{
  /* Mask/ unmask the SDIO interrupt status */
  switch(intType)
  {
    /* Mask/ unmask command complete interrupt status */
    case SDIOC_INT_CMDCOMP:
      SDIO->I_STAT_EN.BF.CMDCOMPSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask transfer complete interrupt status */
    case SDIOC_INT_XFRCOMP:
      SDIO->I_STAT_EN.BF.XFRCOMPSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask block gap event */
    case SDIOC_INT_BGEVNT:
      SDIO->I_STAT_EN.BF.BGEVNTSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask DMA interrupt status */
    case SDIOC_INT_DMAINT:
      SDIO->I_STAT_EN.BF.DMAINTSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask buffer write ready interrupt status */
    case SDIOC_INT_BUFWRRDY:
      SDIO->I_STAT_EN.BF.BUFWRRDYSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask buffer read ready interrupt status */
    case SDIOC_INT_BUFRDRDY:
      SDIO->I_STAT_EN.BF.BUFRDRDYSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask card insertion interrupt status */
    case SDIOC_INT_CDINS:
      SDIO->I_STAT_EN.BF.CDINSSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask card remove interrupt status */
    case SDIOC_INT_CDREM:
      SDIO->I_STAT_EN.BF.CDREMSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask card interrupt status */
    case SDIOC_INT_CDINT:
      SDIO->I_STAT_EN.BF.CDINTSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask command timeout error interrupt status */
    case SDIOC_INT_CTOERR:
      SDIO->I_STAT_EN.BF.CTOSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask command crc error interrupt status */
    case SDIOC_INT_CCRCERR:
      SDIO->I_STAT_EN.BF.CCRCSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask command end bit error interrupt status */
    case SDIOC_INT_CENDERR:
      SDIO->I_STAT_EN.BF.CENDSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask command index error interrupt status */
    case SDIOC_INT_CIDXERR:
       SDIO->I_STAT_EN.BF.CIDXSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask data timeout error interrupt status */
    case SDIOC_INT_DTOERR:
       SDIO->I_STAT_EN.BF.DTOSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask data crc error interrupt status */
    case SDIOC_INT_DCRCERR:
       SDIO->I_STAT_EN.BF.DCRCSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask data end bit error interrupt status */
    case SDIOC_INT_DENDERR:
      SDIO->I_STAT_EN.BF.DENDSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask current limit error interrupt status */
    case SDIOC_INT_ILMTERR:
      SDIO->I_STAT_EN.BF.ILMTSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask AHB target error interrupt status */
    case SDIOC_INT_AHBTERR:
      SDIO->I_STAT_EN.BF.ATERRSTEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask all interrupt status */
    case SDIOC_INT_ALL:
      if (maskState == UNMASK)
      {
        SDIO->I_STAT_EN.WORDVAL |= 0x10FF01FF;
      } 
      else
      {
        SDIO->I_STAT_EN.WORDVAL = 0x00000000;
      }
      break;

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      Mask/ Unmask SDIO interrupt signal
 *
 * @param[in]  intType  : Interrupt type
 * @param[in]  maskState: Mask / Unmask control
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_IntSigMask(SDIOC_INT_Type intType, IntMask_Type maskState)
{
  /* Mask/ unmask the SDIO interrupt signal */
  switch(intType)
  {
    /* Mask/ unmask command complete interrupt signal */
    case SDIOC_INT_CMDCOMP:
      SDIO->I_SIG_EN.BF.CMDCOMPSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask transfer complete interrupt signal */
    case SDIOC_INT_XFRCOMP:
      SDIO->I_SIG_EN.BF.XFRCOMPSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask block gap event */
    case SDIOC_INT_BGEVNT:
      SDIO->I_SIG_EN.BF.BGEVNTSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask DMA interrupt signal */
    case SDIOC_INT_DMAINT:
      SDIO->I_SIG_EN.BF.DMAINTSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask buffer write ready interrupt signal */
    case SDIOC_INT_BUFWRRDY:
      SDIO->I_SIG_EN.BF.BUFWRRDYSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask buffer read ready interrupt signal */
    case SDIOC_INT_BUFRDRDY:
      SDIO->I_SIG_EN.BF.BUFRDRDYSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask card insertion interrupt signal */
    case SDIOC_INT_CDINS:
      SDIO->I_SIG_EN.BF.CDINSSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask card remove interrupt signal */
    case SDIOC_INT_CDREM:
      SDIO->I_SIG_EN.BF.CDREMSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask card interrupt signal */
    case SDIOC_INT_CDINT:
      SDIO->I_SIG_EN.BF.CDINTSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask command timeout error interrupt signal */
    case SDIOC_INT_CTOERR:
      SDIO->I_SIG_EN.BF.CTOSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask command crc error interrupt signal */
    case SDIOC_INT_CCRCERR:
      SDIO->I_SIG_EN.BF.CCRCSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask command end bit error interrupt signal */
    case SDIOC_INT_CENDERR:
      SDIO->I_SIG_EN.BF.CENDSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask command index error interrupt signal */
    case SDIOC_INT_CIDXERR:
       SDIO->I_SIG_EN.BF.CIDXSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask data timeout error interrupt signal */
    case SDIOC_INT_DTOERR:
       SDIO->I_SIG_EN.BF.DTOSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask data crc error interrupt signal */
    case SDIOC_INT_DCRCERR:
       SDIO->I_SIG_EN.BF.DCRCSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask data end bit error interrupt signal */
    case SDIOC_INT_DENDERR:
      SDIO->I_SIG_EN.BF.DENDSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask current limit error interrupt signal */
    case SDIOC_INT_ILMTERR:
      SDIO->I_SIG_EN.BF.ILMTSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask AHB target error interrupt signal */
    case SDIOC_INT_AHBTERR:
      SDIO->I_SIG_EN.BF.ATERRSGEN = ((maskState == UNMASK) ? 1 : 0);
      break;

    /* Mask/ unmask all interrupt signal */
    case SDIOC_INT_ALL:
      if (maskState == UNMASK)
      {
        SDIO->I_SIG_EN.WORDVAL |= 0x10FF01FF;
      } 
      else
      {
        SDIO->I_SIG_EN.WORDVAL = 0x00000000;
      }
      break;

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      Clear SDIO interrupt status
 *
 * @param[in]  intType  : Interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void SDIOC_IntClr(SDIOC_INT_Type intType)
{
  /* clear the SDIO interrupt status */
  switch(intType)
  {
    /* clear command complete interrupt status */
    case SDIOC_INT_CMDCOMP:
      SDIO->I_STAT.BF.CMDCOMP = 1;
      break;

    /* clear transfer complete interrupt status */
    case SDIOC_INT_XFRCOMP:
      SDIO->I_STAT.BF.XFRCOMP = 1;
      break;

    /* clear block gap event */
    case SDIOC_INT_BGEVNT:
      SDIO->I_STAT.BF.BGEVNT = 1;
      break;

    /* clear DMA interrupt status */
    case SDIOC_INT_DMAINT:
      SDIO->I_STAT.BF.DMAINT = 1;
      break;

    /* clear buffer write ready interrupt status */
    case SDIOC_INT_BUFWRRDY:
      SDIO->I_STAT.BF.BUFWRRDY = 1;
      break;

    /* clear buffer read ready interrupt status */
    case SDIOC_INT_BUFRDRDY:
      SDIO->I_STAT.BF.BUFRDRDY = 1;
      break;

    /* clear card insertion interrupt status */
    case SDIOC_INT_CDINS:
      SDIO->I_STAT.BF.CDINS = 1;
      break;

    /* clear card remove interrupt status */
    case SDIOC_INT_CDREM:
      SDIO->I_STAT.BF.CDREM = 1;
      break;

    /* clear command timeout error interrupt status */
    case SDIOC_INT_CTOERR:
      SDIO->I_STAT.BF.CTOERR = 1;
      break;

    /* clear command crc error interrupt status */
    case SDIOC_INT_CCRCERR:
      SDIO->I_STAT.BF.CCRCERR = 1;
      break;

    /* clear command end bit error interrupt status */
    case SDIOC_INT_CENDERR:
      SDIO->I_STAT.BF.CENDERR = 1;
      break;

    /* clear command index error interrupt status */
    case SDIOC_INT_CIDXERR:
      SDIO->I_STAT.BF.CIDXERR = 1;
      break;

    /* clear data timeout error interrupt status */
    case SDIOC_INT_DTOERR:
      SDIO->I_STAT.BF.DTOERR = 1;
      break;

    /* clear data crc error interrupt status */
    case SDIOC_INT_DCRCERR:
      SDIO->I_STAT.BF.DCRCERR = 1;
      break;

    /* clear data end bit error interrupt status */
    case SDIOC_INT_DENDERR:
      SDIO->I_STAT.BF.DENDERR = 1;
      break;

    /* clear current limit error interrupt status */
    case SDIOC_INT_ILMTERR:
      SDIO->I_STAT.BF.ILMTERR = 1;
      break;

    /* clear AHB target error interrupt status */
    case SDIOC_INT_AHBTERR:
      SDIO->I_STAT.BF.AHBTERR = 1;
      break;

    /* clear all interrupt status */
    case SDIOC_INT_ALL:
      SDIO->I_STAT.WORDVAL |= 0x10FF00FF;
      break;

  default:
    break;
  }
}
/*@} end of group SDIO_Public_Functions */

/*@} end of group SDIO_definitions */

/*@} end of group MW300_Periph_Driver */

