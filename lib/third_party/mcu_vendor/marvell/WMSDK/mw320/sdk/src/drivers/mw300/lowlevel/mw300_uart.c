/** @file     mw300_uart.c
 *
 *  @brief    This file provides UART functions.
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
#include "mw300_uart.h"
#include "mw300_clock.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @defgroup UART UART
 *  @brief UART driver modules
 *  @{
 */

/** @defgroup UART_Private_Type
 *  @{
 */

/*@} end of group UART_Private_Type*/

/** @defgroup UART_Private_Defines
 *  @{
 */

/*@} end of group UART_Private_Defines */

/** @defgroup UART_Private_Variables
 *  @{
 */

/**
 *  @brief UART0, UART1 and UART2 address array
 */
static const uint32_t uartAddr[3] = {UART0_BASE,UART1_BASE,UART2_BASE};

/*@} end of group UART_Private_Variables */

/** @defgroup UART_Global_Variables
 *  @{
 */

/*@} end of group UART_Global_Variables */

/** @defgroup UART_Private_FunctionDeclaration
 *  @{
 */
void UART_IntHandler(INT_Peripher_Type intPeriph, UART_ID_Type uartNo);

/*@} end of group UART_Private_FunctionDeclaration */

/** @defgroup UART_Private_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      UART interrupt handle
 *
 * @param[in]  intPeriph: Select the peripheral, such as INT_UART1,INT_AES
 * @param[in]  uartNo:  Select the UART port
 *
 * @return none
 *
 *******************************************************************************/
void UART_IntHandler(INT_Peripher_Type intPeriph, UART_ID_Type uartNo)
{
  uint32_t intId;

  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  intId = UARTx->IIR_FCR.WORDVAL;

  /* Auto Baudrate Lock interrupt */
  /* when ABL interrupt happens, Interrupt Pending bit is Not set */
  if( intId & (0x01<<4) )
  {
    if(intCbfArra[intPeriph][UART_INT_ABL] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][UART_INT_ABL]();
    }
  }

  if( !(intId & 0x01) )
  {
    /* Receive Time-Out Interrupt */
    if( intId & (0x01<<3) )
    {
      if(intCbfArra[intPeriph][UART_INT_RTO] != NULL)
      {
        /* call the callback function */
        intCbfArra[intPeriph][UART_INT_RTO]();
      }
    }

    /* Modem Interrupt */
    if( ((intId >>1)&0x3) == 0 )
    {
      if(intCbfArra[intPeriph][UART_INT_MODEM] != NULL)
      {
        /* call the callback function */
        intCbfArra[intPeriph][UART_INT_MODEM]();
      }
    }

    /* Transmit Data Request Interrupt */
    if( ((intId >>1)&0x3) == 1 )
    {
      if(intCbfArra[intPeriph][UART_INT_TDR] != NULL)
      {
        /* call the callback function */
        intCbfArra[intPeriph][UART_INT_TDR]();
      }
    }

    /* Receive Data Available Interrupt */
    if( ((intId >>1)&0x3) == 2 )
    {
      if(intCbfArra[intPeriph][UART_INT_RDA] != NULL)
      {
        /* call the callback function */
        intCbfArra[intPeriph][UART_INT_RDA]();
      }
    }

    /* Receive Line Status Error Interrupt */
    if( ((intId >>1)&0x3) == 3 )
    {
      if(intCbfArra[intPeriph][UART_INT_RLSE] != NULL)
      {
        /* call the callback function */
        intCbfArra[intPeriph][UART_INT_RLSE]();
      }
    }
  }
}

static void UART_Calculate_Dividers(CLK_Fraction_Type *uartClkSrcDiv,
        uint32_t refsclk)
{
    uint32_t sysclk, sclk;
    uint32_t nx, ny;

    sclk = refsclk;
    sysclk = CLK_GetSystemClk();
    /* sysclk/sclk should greater than 1.8 */
    while (sysclk*10 / sclk < 18)
        sclk /= 2;

    nx = sysclk;
    ny = sclk;
    while (nx >= 8192 || ny >= 2048) {
        if (nx % 10 == 0 && ny % 10 == 0) {
            nx /= 10;
            ny /= 10;
        } else {
            nx >>= 2;
            ny >>= 2;
        }
    }

    if (nx != 0 && ny != 0) {
        uartClkSrcDiv->clkDividend = ny;
        uartClkSrcDiv->clkDivisor = nx;
    }
}

static void UART_Configure_Dividers(UART_ID_Type uartNo, uint32_t baudRate)
{
    /**
     * Refer to http://www.wormfood.net/avrbaudcalc.php
     * when choose different UART clock frequcncy, which can meet
     * different baud rate requirements.
     * sclk equal to 14.7456MHz can meet the baud rate less than 230400
     * sclk equal to 32.768MHz can meet the baud rate 12800, 25600
     * sclk equal to 32MHz can meet the baud rate 250000, 500000,
     *  1000000, 2000000
     * sclk equal to 48MHz can meet the baud rate 1500000
     */
     const uint32_t refsclk[] = {14745600, 32768000, 32000000, 48000000};

    CLK_Fraction_Type uartClkSrcDiv = {0, 0};
    switch (baudRate) {
        case 300:
        case 600:
        case 1200:
        case 2400:
        case 4800:
        case 9600:
        case 14400:
        case 19200:
        case 28800:
        case 38400:
        case 57600:
        case 76800:
        case 115200:
        case 230400:
            UART_Calculate_Dividers(&uartClkSrcDiv, refsclk[0]);
            break;
        case 128000:
        case 256000:
            UART_Calculate_Dividers(&uartClkSrcDiv, refsclk[1]);
            break;
        case 250000:
        case 500000:
        case 1000000:
        case 2000000:
            UART_Calculate_Dividers(&uartClkSrcDiv, refsclk[2]);
            break;
        case 1500000:
            UART_Calculate_Dividers(&uartClkSrcDiv, refsclk[3]);
            break;
        default:
            uartClkSrcDiv.clkDividend = 0xE2;
            uartClkSrcDiv.clkDivisor = 0x3E8;
            break;
    }

    CLK_UARTDividerSet(CLK_GetUARTClkSrc((CLK_UartID_Type)uartNo), uartClkSrcDiv);
}
/*@} end of group UART_Private_Functions */

/** @defgroup UART_Public_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief      Initializes the UART
 *
 * @param[in]  uartNo:  Select the UART port,should be UART1_ID, UART2_ID.
 * @param[in]  uartCfgStruct:  Pointer to a UART configuration structure
 *
 * @return none
 *
 *******************************************************************************/
Status UART_Init(UART_ID_Type uartNo, UART_CFG_Type* uartCfgStruct)
{
  uint32_t baudRateDivisor = 0;
  uint32_t tempVal = 0;
  uint32_t fraction = 0;
  CLK_Fraction_Type uartClkSrcDiv = {0, 0};
   
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));
  CHECK_PARAM(IS_UART_DATA_BIT(uartCfgStruct->dataBits));
  CHECK_PARAM(IS_UART_PARITY(uartCfgStruct->parity));

  /************Configure the Baud Rate*********************************/
  UART_Configure_Dividers(uartNo, uartCfgStruct->baudRate);

  /* Get uart function clock parameters */
  CLK_GetUARTDivider(CLK_GetUARTClkSrc((CLK_UartID_Type)uartNo), &uartClkSrcDiv);

  /* Get uart function clock value */
  tempVal = (CLK_GetSystemClk() / uartClkSrcDiv.clkDivisor) *  uartClkSrcDiv.clkDividend;

  fraction = tempVal * 10 / (16 * uartCfgStruct->baudRate) % 10;
  baudRateDivisor = tempVal / (16 * uartCfgStruct->baudRate);

  if (fraction >= 5)
    ++baudRateDivisor;

  /* Configure the Baud Rate */
  UARTx->LCR.BF.DLAB = 0x01;
  UARTx->DLH_IER.BF_DLH.DLH = (baudRateDivisor >> 0x08)& 0xFF;
  UARTx->RBR_THR_DLL.BF_DLL.DLL = baudRateDivisor & 0xFF;
  UARTx->LCR.BF.DLAB = 0x0;

  /* Configure data bit */
  if(UART_DATABITS_8 == uartCfgStruct->dataBits)
  {
    UARTx->LCR.BF.WLS10 = 0x3;
  }
  else
  {
    UARTx->LCR.BF.WLS10 = 0x2;
  }

  /* Configure parity type */
  switch(uartCfgStruct->parity)
  {
    case UART_PARITY_NONE:
      UARTx->LCR.BF.PEN = 0x0;
      break;

    case UART_PARITY_ODD:
      UARTx->LCR.BF.PEN = 0x1;
      UARTx->LCR.BF.EPS = 0x0;
      break;

    case UART_PARITY_EVEN:
      UARTx->LCR.BF.PEN = 0x1;
      UARTx->LCR.BF.EPS = 0x1;
      break;
  }

  /* Enable or Disbale Sticky Parity */
  if(ENABLE == uartCfgStruct->stickyParity)
  {
    UARTx->LCR.BF.STKYP = 1;
  }
  else
  {
    UARTx->LCR.BF.STKYP = 0;
  }

  /* Enable or disable high speed uart */
  if(ENABLE == uartCfgStruct->highSpeedUart)
  {
    UARTx->DLH_IER.BF_IER.HSE = 1;
  }
  else
  {
    UARTx->DLH_IER.BF_IER.HSE = 0;
  }

  /* Enable or disable NRZ coding */
  if(ENABLE == uartCfgStruct->nrzCodeing)
  {
    UARTx->DLH_IER.BF_IER.NRZE = 1;
  }
  else
  {
    UARTx->DLH_IER.BF_IER.NRZE = 0;
  }
  return DSUCCESS;
}

/****************************************************************************//**
  * @brief      Send data to the UART peripheral
  *
  * @param[in]  uartNo:  Select the UART port,should be UART1_ID, UART2_ID.
  * @param[in]  buf:  Pointer to data to be send
  *
  * @return none
  *
  *****************************************************************************/
void UART_WriteLine(UART_ID_Type uartNo, uint8_t *buf)
{
	while (*buf) {
		while (UART_GetLineStatus(uartNo, UART_LINESTATUS_TEMT)
		       != SET)
			;
		UART_SendData(uartNo, *buf++);
       }
}

/****************************************************************************//**
 * @brief      Config UART FIFO function
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 * @param[in]  fifoCfg:  Pointer to a UART_FIFO_CFG_Type Structure
 *
 * @return none
 *
 *******************************************************************************/
void UART_FifoConfig(UART_ID_Type uartNo, UART_FifoCfg_Type * fifoCfg)
{
  uint32_t  uartFCR = 0x0;
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));
  CHECK_PARAM(IS_UART_PERIPHERAL_BUS(fifoCfg->peripheralBusType));
  CHECK_PARAM(IS_UART_RX_FIFO_LEVEL(fifoCfg->rxFifoLevel));
  CHECK_PARAM(IS_UART_TX_FIFO_LEVEL(fifoCfg->txFifoLevel));

  /* configure FIFO function */
  if(ENABLE == fifoCfg->fifoEnable)
  {
    uartFCR = 0x0;
    
    /* enable the tx and rx fifo*/
    uartFCR |= 0x01;
    
    /* configure the pheripheral bus type */
    if(UART_PERIPHERAL_BITS_32 == fifoCfg->peripheralBusType)
    {
      uartFCR |= (0x01 << 5);
    }

    /* reset tx fifo  */
    if(ENABLE == fifoCfg->rxFifoReset)
    {
      uartFCR |= (0x01 << 1);
    }

    /* reset rx fifo */
    if(ENABLE == fifoCfg->txFifoReset)
    {
      uartFCR |= (0x01 << 2);
    }

    /* configure tx fifo threshold */
    if(UART_TXFIFO_EMPTY == fifoCfg->txFifoLevel)
    {
      uartFCR |= (0x01 << 3);
    }
    
    /* config rx fifo threshold */
    switch(fifoCfg->rxFifoLevel)
    {
       case UART_RXFIFO_BYTE_1:
        uartFCR |= 0x00; 
        break;

      case UART_RXFIFO_BYTES_8:
        uartFCR |= (0x01 << 6);     
        break;

      case UART_RXFIFO_BYTES_16:
        uartFCR |= (0x02 << 6);
        break;

      case UART_RXFIFO_BYTES_32:
        uartFCR |= (0x03 << 6);
        break;

      default:
        break;
    }
  }
  else
  {
    /* disable the tx and rx fifo*/
    uartFCR &= 0xFE;
  }
   
  UARTx->IIR_FCR.WORDVAL = uartFCR;


  /* configure uart auto flow control function */
  if(ENABLE == fifoCfg->autoFlowControl)
  {

    UARTx->MCR.BF.AFE = 1;
  }
  else
  {
    UARTx->MCR.BF.AFE = 0;
  }

  /* enable or disable fifo dma function */
  if(ENABLE == fifoCfg->fifoDmaEnable)
  {
    UARTx->DLH_IER.BF_IER.DMAE = 1;
  }
  else
  {
    UARTx->DLH_IER.BF_IER.DMAE = 0;
  }
}

/****************************************************************************//**
 * @brief      Config UART IRDA function
 *
 * @param[in]  uartNo:  Select the UART port, should be UART0_ID, UART1_ID, UART2_ID.
 * @param[in]  irCfg:  Pointer to a UART_IrCfg_Type Structure
 *
 * @return none
 *
 *******************************************************************************/
void UART_IrConfig(UART_ID_Type uartNo, UART_IrCfg_Type* irCfg)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));
  CHECK_PARAM(IS_UART_IR_POLARITY(irCfg->txIrPolarity));
  CHECK_PARAM(IS_UART_IR_POLARITY(irCfg->rxIrPolarity));
  CHECK_PARAM(IS_UART_IR_PULSE_WIDTH(irCfg->irPulseWidth));

  /* config ir pulse width & polarity */
  UARTx->ISR.BF.XMODE = irCfg->irPulseWidth;
  UARTx->ISR.BF.TXPL = irCfg->txIrPolarity;
  UARTx->ISR.BF.RXPL = irCfg->rxIrPolarity;

  /* enable or disable ir function */
  UARTx->ISR.BF.XMITIR = irCfg->txIrEnable;
  UARTx->ISR.BF.RCVEIR = irCfg->rxIrEnable;
}

/****************************************************************************//**
 * @brief      Config UART Auto Baudrate Detection function
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 * @param[in]  baudCfg:  Pointer to a UART_AutoBaudCfg_Type Structure
 *
 * @return none
 *
 *******************************************************************************/
void UART_BaudConfig(UART_ID_Type uartNo, UART_AutoBaudCfg_Type* baudCfg)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));
  CHECK_PARAM(IS_UART_BAUD_CAL(baudCfg->baudCalType));
  CHECK_PARAM(IS_UART_BAUD_PROGRAM(baudCfg->baudProgramType));

  /* config auto baudrate calculation type */
  UARTx->ABR.BF.ABT = baudCfg->baudCalType;

  /* config auto baudrate program type */
  UARTx->ABR.BF.ABUP = baudCfg->baudProgramType;

  /* enable or disable auto baudrate function */
  UARTx->ABR.BF.ABE = baudCfg->autoBaudEnable;
}

/****************************************************************************//**
 * @brief      Enable UART function
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 *
 * @return none
 *
 *******************************************************************************/
void UART_Enable(UART_ID_Type uartNo)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  /* enable uart unit */
  UARTx->DLH_IER.BF_IER.UUE = 1;
}

/****************************************************************************//**
 * @brief      Disable UART function
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 *
 * @return none
 *
 *******************************************************************************/
void UART_Disable(UART_ID_Type uartNo)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  /* disenable uart unit */
  UARTx->DLH_IER.BF_IER.UUE = 0;
}

/****************************************************************************//**
 * @brief      Set a UART Break Condition to Receiving UART
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 *
 * @param[in]  newState: ENABLE or DISABLE Break Condition.
 *              -- ENABLE: force Txd to '0'.
 *              -- DISABLE: no effect on Txd output.
 *
 * @return none
 *
 *******************************************************************************/
void UART_SetBreakCmd(UART_ID_Type uartNo, FunctionalState newState)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  /* enable uart unit */
  if(ENABLE == newState)
  {
    UARTx->LCR.BF.SB = 1;
  }
  else
  {
    UARTx->LCR.BF.SB = 0;
  }
}

/****************************************************************************//**
 * @brief      Set a UART Loop Back Mode
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 *
 * @param[in]  newState: ENABLE or DISABLE Break Condition.
 *              -- ENABLE: enable uart loop back mode.
 *              -- DISABLE: disable uart loop back mode.
 *
 * @return none
 *
 *******************************************************************************/
void UART_SetLoopBackMode(UART_ID_Type uartNo, FunctionalState newState)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  /* enable uart unit */
  if(ENABLE == newState)
  {
    UARTx->MCR.BF.LOOP = 1;
  }
  else
  {
    UARTx->MCR.BF.LOOP = 0;
  }
}

/****************************************************************************//**
 * @brief      Set UART OUT2 Signal
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 *
 * @param[in]  status: RESET or SET OUT1 Signal.
 *               -- When Loopback Mode Enabled.
 *                    -- RESET: force MSR[DCD] to '0'
 *                    -- SET: force MSR[DCD] to '1'
 *               -- When Loopback Mode Disabled.
 *                    -- RESET: All UART Interrupt is Disabled
 *                    -- SET: All UART Interrupt is Enabled
 *
 * @return none
 *
 *******************************************************************************/
void UART_SetOut2Signal(UART_ID_Type uartNo, FlagStatus status)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  if(SET == status)
  {
    UARTx->MCR.BF.OUT2 = 1;
  }
  else
  {
    UARTx->MCR.BF.OUT2 = 0;
  }
}

/****************************************************************************//**
 * @brief      Set UART OUT1 Signal
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 *
 * @param[in]  status: RESET or SET OUT1 Signal.
 *              -- RESET: force MSR[RI] to '0'.
 *              -- SET: force MSR[RI] to '1'.
 *
 * @return none
 *
 *******************************************************************************/
void UART_SetOut1Signal(UART_ID_Type uartNo, FlagStatus status)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  if(SET == status)
  {
    UARTx->MCR.BF.OUT1 = 1;
  }
  else
  {
    UARTx->MCR.BF.OUT1 = 0;
  }
}

/****************************************************************************//**
 * @brief      Set UART DTR Signal
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 *
 * @param[in]  status: RESET or SET DTR Signal.
 *              -- RESET: force nDTR to '0'.
 *              -- SET: force nDTR to '1'.
 *
 * @return none
 *
 *******************************************************************************/
void UART_SetDTRSignal(UART_ID_Type uartNo, FlagStatus status)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  if(SET == status)
  {
    UARTx->MCR.BF.DTR = 1;
  }
  else
  {
    UARTx->MCR.BF.DTR = 0;
  }
}

/****************************************************************************//**
 * @brief      Assert UART RTS bit in MCR if Auto Flow Control Disabled
 *
 * @param[in]  uartNo:  Select the UART port,should be UART0_ID, UART1_ID,
 *
 * @return     None
 *
 *******************************************************************************/
void UART_RTS_Assert(UART_ID_Type uartNo)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  UARTx->MCR.BF.RTS = 1;
}

/****************************************************************************//**
 * @brief      Deassert UART RTS bit in MCR if Auto Flow Control Disabled
 *
 * @param[in]  uartNo:  Select the UART port,should be UART0_ID, UART1_ID,
 *
 * @return     None
 *
 *******************************************************************************/
void UART_RTS_Deassert(UART_ID_Type uartNo)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  UARTx->MCR.BF.RTS = 0;
}

/****************************************************************************//**
 * @brief      Set DL register according to Auto Baud Detect Count value
 *
 * @param[in]  uartNo:  Select the UART port,should be UART0_ID, UART1_ID,
 * @param[in]  autoBaudCount: Count value read from Auto-Baud Count register
 * @return     None
 *
 *******************************************************************************/
void UART_SetAutoBaudDL(UART_ID_Type uartNo, uint16_t autoBaudCount)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  /* enable DL register access */
  UARTx->LCR.BF.DLAB = 0x01;
  UARTx->DLH_IER.BF_DLH.DLH = (autoBaudCount >> 12)& 0xFF;
  UARTx->RBR_THR_DLL.BF_DLL.DLL = (autoBaudCount >> 4) & 0xFF;
  UARTx->LCR.BF.DLAB = 0x0;
}

/****************************************************************************//**
 * @brief      Receive a 32-bit data from UART rx fifo
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 *
 * @return     The received data
 *
 *******************************************************************************/
uint32_t UART_ReceiveData(UART_ID_Type uartNo)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* check the parameter */
  CHECK_PARAM(IS_UART_ID(uartNo));

  /* Receive data */
  return (uint32_t)(UARTx->RBR_THR_DLL.WORDVAL);
}

/****************************************************************************//**
 * @brief      Send a 32-bit data to UART tx fifo
 *
 * @param[in]  uartNo:  Select the UART port,should be UART1_ID, UART2_ID.
 * @param[in]  data:  The data to be send
 *
 * @return none
 *
 *******************************************************************************/
void UART_SendData(UART_ID_Type uartNo, uint32_t data)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* check the parameter */
  CHECK_PARAM(IS_UART_ID(uartNo));

  /* Send data */
  UARTx->RBR_THR_DLL.WORDVAL = ((uint32_t)data);
}

/****************************************************************************//**
 * @brief      Receive 9 bit data from the UART peripheral, odd parity default.
 *
 * @param[in]  uartNo:  Select the UART port,should be UART0_ID, UART1_ID,
 *                                                     UART2_ID
 *
 * @return     The received data
 *
 *******************************************************************************/
uint16_t UART_Receive9bits(UART_ID_Type uartNo)
{
	uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);
	uint16_t data;
	uint32_t lsr = 0;
	uint8_t parity = 0;
	int i;

	/* check the parameter */
	CHECK_PARAM(IS_UART_PERIPH(uartNo));

	/* Wait untill data is available */
	do {
		lsr = UARTx->LSR.WORDVAL;
	} while (!(lsr & 0x01));

	/* Receive data */
	data = (uint16_t)UARTx->RBR_THR_DLL.WORDVAL;

	for (i = 0; i < 8; i++)
		parity ^= ((data >> i) & 0x1);

	/* get bit9 according parity error status */
	data |= (parity ^ ((lsr >> 2) & 0x1)) << 8;

  return (data & 0x1FF);
}

/****************************************************************************//**
 * @brief Send 9 bit data to the UART peripheral
 *
 * @param[in]  uartNo:  Select the UART port,should be UART0_ID, UART1_ID,
 *                                                     UART2_ID
 * @param[in]  data:  The data to be send
 *
 * @return none
 *
 *******************************************************************************/
void UART_Send9bits(UART_ID_Type uartNo, uint16_t data)
{
	uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);
	uint8_t parity = 0;
	int i;

	/* check the parameter */
	CHECK_PARAM(IS_UART_PERIPH(uartNo));

	for (i = 0; i < 8; i++)
		parity ^= ((data >> i) & 0x1);

	/* Select Parity according to MSB bit */
        /* TODO: Find correct bust equivalent for mwm300 */
	/*while (UARTx->USR.BF.BUSY)
	  ;*/
	UARTx->LCR.BF.EPS = !(((data >> 8) & 0x1) ^ parity);

	/* Send data */
	UARTx->RBR_THR_DLL.WORDVAL = (uint32_t)data & 0xFF;

	/* Set parity back to EVEN */

	/* while (UARTx->USR.BF.BUSY)
	   ; */
	UARTx->LCR.BF.EPS = 1;
}

/****************************************************************************//**
 * @brief Set Stop Bits
 *
 * @param[in]  uartNo:  Select the UART port,should be UART0_ID, UART1_ID,
 *                                                     UART2_ID
 * @param[in]  stopBits: select UART_STOPBITS_1 or UART_STOPBITS_2
 *
 * @return none
 *
 *******************************************************************************/
void UART_SetStopBits(UART_ID_Type uartNo, UART_StopBits_Type stopBits)
{
	uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

	/* check the parameter */
	CHECK_PARAM(IS_UART_PERIPH(uartNo));

	UARTx->LCR.BF.STB = stopBits;
}

/****************************************************************************//**
 * @brief      Get UART auto baud count number
 *
 * @param[in]  uartNo:  Select the UART port,should be UART1_ID, UART2_ID.
 *
 * @return none
 *
 *******************************************************************************/
uint16_t UART_GetAutoBaudCount(UART_ID_Type uartNo)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* check the parameter */
  CHECK_PARAM(IS_UART_ID(uartNo));

  return(UARTx->ACR.BF.COUNT_VALUE);
}

/****************************************************************************//**
 * @brief      Get Line Status register value
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 * @param[in]  lineStatus: "UART_LineStatus_Type" type parameter
 *
 * @return The state value of UART Line Status register
 *
 *******************************************************************************/
FlagStatus UART_GetLineStatus(UART_ID_Type uartNo,UART_LineStatus_Type lineStatus)
{
  FlagStatus bitStatus = RESET;
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));
  CHECK_PARAM(IS_UART_LINE_STATUS(lineStatus));

  switch(lineStatus)
  {
    case UART_LINESTATUS_FIFOE:
      bitStatus = ((UARTx->LSR.BF.FIFOE == 1) ? SET : RESET);
      break;

    case UART_LINESTATUS_TEMT:
      bitStatus = ((UARTx->LSR.BF.TEMT == 1) ? SET : RESET);
      break;

    case UART_LINESTATUS_TDRQ:
      bitStatus = ((UARTx->LSR.BF.TDRQ == 1) ? SET : RESET);
      break;

    case UART_LINESTATUS_BI:
      bitStatus = ((UARTx->LSR.BF.BI == 1) ? SET : RESET);
      break;

    case UART_LINESTATUS_FE:
      bitStatus = ((UARTx->LSR.BF.FE == 1) ? SET : RESET);
      break;

    case UART_LINESTATUS_PE:
      bitStatus = ((UARTx->LSR.BF.PE == 1) ? SET : RESET);
      break;

    case UART_LINESTATUS_OE:
      bitStatus = ((UARTx->LSR.BF.OE == 1) ? SET : RESET);
      break;

    case UART_LINESTATUS_DR:
      bitStatus = ((UARTx->LSR.BF.DR == 1) ? SET : RESET);
      break;

    default:
      break;
  }

  return bitStatus;
}

/****************************************************************************//**
 * @brief      Get UART Modem Status register Value
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_Port, UART2_Port.
 * @param[in]  modemStatus: "UART_ModemStatus_Type" type parameter
 *
 * @return The state value of UART Modem Status register
 *
 *******************************************************************************/
FlagStatus UART_GetModemStatus(UART_ID_Type uartNo,UART_ModemStatus_Type modemStatus)
{
  FlagStatus bitStatus = RESET;
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));
  CHECK_PARAM(IS_UART_MODEM_STATUS(modemStatus));

  switch(modemStatus)
  {
    case UART_MODEMSTATUS_DCTS:
      bitStatus = ((UARTx->MSR.BF.DCTS == 1) ? SET : RESET);
      break;

    case UART_MODEMSTATUS_DDSR:
      bitStatus = ((UARTx->MSR.BF.DDSR == 1) ? SET : RESET);
      break;

    case UART_MODEMSTATUS_TERI:
      bitStatus = ((UARTx->MSR.BF.TERI == 1) ? SET : RESET);
      break;

    case UART_MODEMSTATUS_DDCD:
      bitStatus = ((UARTx->MSR.BF.DDCD == 1) ? SET : RESET);
      break;

    case UART_MODEMSTATUS_CTS:
      bitStatus = ((UARTx->MSR.BF.CTS == 1) ? SET : RESET);
      break;

    case UART_MODEMSTATUS_DSR:
      bitStatus = ((UARTx->MSR.BF.DSR == 1) ? SET : RESET);
      break;

    case UART_MODEMSTATUS_RI:
      bitStatus = ((UARTx->MSR.BF.RI == 1) ? SET : RESET);
      break;

    case UART_MODEMSTATUS_DCD:
      bitStatus = ((UARTx->MSR.BF.DCD == 1) ? SET : RESET);
      break;

    default:
      break;
  }

  return bitStatus;
}

/****************************************************************************//**
 * @brief      Get current UART receive FIFO level
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 *
 * @return The number of data entries in the receive FIFO
 *
 *******************************************************************************/
uint32_t UART_GetRxFIFOLevel(UART_ID_Type uartNo)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  return(UARTx->RFOR.BF.BYTE_COUNT);
}

/****************************************************************************//**
 * @brief      Mask/Unmask the UART interrupt
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 * @param[in]  intType:  Specifies the interrupt type
 * @param[in]  newState:  Enable/Disable Specified interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void UART_IntMask(UART_ID_Type uartNo,UART_INT_Type intType, IntMask_Type intMask)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));
  CHECK_PARAM(IS_UART_INT(intType));
  CHECK_PARAM(IS_INTMASK(intMask));

  if(intMask == UNMASK)
  {
    UARTx->MCR.BF.OUT2 = 1;
  }

  switch(intType)
  {
    case UART_INT_ABL:
      if(intMask == UNMASK)
      {
        /* Enable this interrupt */
        UARTx->ABR.BF.ABLIE = 1;
      }
      else
      {
        /* Disable this interrupt */
        UARTx->ABR.BF.ABLIE = 0;
      }
      break;

    case UART_INT_RTO:
      if(intMask == UNMASK)
      {
        /* Enable this interrupt */
        UARTx->DLH_IER.BF_IER.RTOIE = 1;
      }
      else
      {
        /* Disable this interrupt */
        UARTx->DLH_IER.BF_IER.RTOIE = 0;
      }
      break;

    case UART_INT_MODEM:
      if(intMask == UNMASK)
      {
        /* Enable this interrupt */
        UARTx->DLH_IER.BF_IER.MIE = 1;
      }
      else
      {
        /* Disable this interrupt */
        UARTx->DLH_IER.BF_IER.MIE = 0;
      }
      break;

    case UART_INT_TDR:
      if(intMask == UNMASK)
      {
        /* Enable this interrupt */
        UARTx->DLH_IER.BF_IER.TIE = 1;
      }
      else
      {
        /* Disable this interrupt */
        UARTx->DLH_IER.BF_IER.TIE = 0;
      }
      break;

    case UART_INT_RDA:
      if(intMask == UNMASK)
      {
        /* Enable this interrupt */
        UARTx->DLH_IER.BF_IER.RAVIE = 1;
      }
      else
      {
        /* Disable this interrupt */
        UARTx->DLH_IER.BF_IER.RAVIE = 0;
      }
      break;

    case UART_INT_RLSE:
      if(intMask == UNMASK)
      {
        /* Enable this interrupt */
        UARTx->DLH_IER.BF_IER.RLSE = 1;
      }
      else
      {
        /* Disable this interrupt */
        UARTx->DLH_IER.BF_IER.RLSE = 0;
      }
      break;

    case UART_INT_ALL:
      if(intMask == UNMASK)
      {
        /* Enable this interrupt */
        UARTx->ABR.BF.ABLIE = 1;
        UARTx->DLH_IER.BF_IER.RTOIE = 1;
        UARTx->DLH_IER.BF_IER.MIE = 1;
        UARTx->DLH_IER.BF_IER.TIE = 1;
        UARTx->DLH_IER.BF_IER.RAVIE = 1;
        UARTx->DLH_IER.BF_IER.RLSE = 1;
      }
      else
      {
        /* Disable this interrupt */
        UARTx->ABR.BF.ABLIE = 0;
        UARTx->DLH_IER.BF_IER.RTOIE = 0;
        UARTx->DLH_IER.BF_IER.MIE = 0;
        UARTx->DLH_IER.BF_IER.TIE = 0;
        UARTx->DLH_IER.BF_IER.RAVIE = 0;
        UARTx->DLH_IER.BF_IER.RLSE = 0;
      }
      break;

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief  UART0 interrupt function
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void UART0_IRQHandler(void)
{
   UART_IntHandler(INT_UART0,UART0_ID);
}
/****************************************************************************//**
 * @brief  UART1 interrupt function
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void UART1_IRQHandler(void)
{
   UART_IntHandler(INT_UART1,UART1_ID);
}
/****************************************************************************//**
 * @brief  UART2 interrupt function
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void UART2_IRQHandler(void)
{
   UART_IntHandler(INT_UART2,UART2_ID);
}

/****************************************************************************//**
 * @brief      Enable/Disable the UART DMA function
 *
 * @param[in]  uartNo:  select the uart port
 * @param[in]  cmd:  enable or disable the UART DMA function
 *
 * @return none
 *
 *******************************************************************************/
void UART_DmaCmd(UART_ID_Type uartNo, FunctionalState cmd)
{
  CHECK_PARAM(IS_UART_ID(uartNo));

  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  UARTx->DLH_IER.BF_IER.DMAE = cmd;
}

/*@} end of group UART_Public_Functions */

/*@} end of group UART_definitions */

/*@} end of group MW300_Periph_Driver */

