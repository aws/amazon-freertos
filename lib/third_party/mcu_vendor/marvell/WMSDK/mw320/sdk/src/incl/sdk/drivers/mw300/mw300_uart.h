/** @file mw300_uart.h
*
*  @brief This file contains UART driver module header
*
*  (C) Copyright 2012-2018 Marvell International Ltd. All Rights Reserved
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

/***************************************************************************//**
 * @version  V1.0.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 ******************************************************************************/

#ifndef __MW300_UART_H__
#define __MW300_UART_H__

#include "mw300.h"
#include "mw300_driver.h"
#include "mw300_clock.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  UART
 *  @{
 */

/** @defgroup UART_Public_Types UART_Public_Types
 *  @{
 */

/**
 *  @brief UART No. type definition
 */
typedef enum
{
  UART0_ID = 0,                                     /*!< UART0 port define */
  UART1_ID,                                      /*!< UART1 port define */
  UART2_ID,                                      /*!< UART2 port define */
}UART_ID_Type;

/**
 *  @brief UART parity type definition
 */
typedef enum
{
  UART_PARITY_NONE = 0,                             /*!< UART none define */
  UART_PARITY_ODD,                               /*!< UART odd define */
  UART_PARITY_EVEN,                              /*!< UART even define */
}UART_Parity_Type;

/**
 *  @brief UART data bits type definition
 */
typedef enum
{
  UART_DATABITS_5,                               /*!< UART data bits length: 5 bits */
  UART_DATABITS_6,                               /*!< UART data bits length: 6 bits */
  UART_DATABITS_7,                               /*!< UART data bits length: 7 bits */
  UART_DATABITS_8,                               /*!< UART data bits length: 8 bits */
}UART_DataBits_Type;

/**
 *  @brief UART stop bits type definition
 */
typedef enum
{
  UART_STOPBITS_1,                               /*!< UART stop bits length: 1 bits */
  UART_STOPBITS_2,                           /*!< UART stop bits length: 2 bits */
}UART_StopBits_Type;

/**
 *  @brief UART peripheral bus type definition
 */
typedef enum
{
  UART_PERIPHERAL_BITS_8 = 0,                    /*!< UART Peripheral Bus: 8 bits */
  UART_PERIPHERAL_BITS_32,                       /*!< UART Peripheral Bus: 32 bits */
}UART_PeripheralBus_Type;


/**
 *  @brief UART TX FIFO level type definition
 */
typedef enum
{
  UART_TXFIFO_HALF_EMPTY = 0,                    /*!< UART Transmit InterruptLevel: half empty */
  UART_TXFIFO_EMPTY,                             /*!< UART Transmit InterruptLevel: full empty */
}UART_TxFIFOLevel_Type;

/**
 *  @brief UART RX FIFO level type definition
 */
typedef enum
{
  UART_RXFIFO_BYTE_1 = 0,                        /*!< 1 byte or more in rx fifo causes interrupt(Not valid in dma mode)  */
  UART_RXFIFO_BYTES_8,                           /*!< 8 byte or more in rx fifo causes interrupt */
  UART_RXFIFO_BYTES_16,                          /*!< 16 byte or more in rx fifo causes interrupt */
  UART_RXFIFO_BYTES_32,                          /*!< 32 byte or more in rx fifo causes interrupt */
}UART_RxFIFOLevel_Type;

/**
 *  @brief UART Infrared Encoder/Decoder Polarity type definition
 */
typedef enum
{
  UART_IR_POSITIVE_PULSE = 0,                    /*!< UART IR Encoder/Decoder takes positive pulse as zero */
  UART_IR_NEGATIVE_PULSE,                        /*!< UART IR Encoder/Decoder takes negative pulse as zero */
}UART_IR_Polarity_Type;

/**
 *  @brief UART Infrared Transmit Pulse Width type definition
 */
typedef enum
{
  UART_IR_PULSEWIDTH_3DIV16 = 0,                 /*!< UART IR Transmit pulse width is 3/16 of a bit time width */
  UART_IR_PULSEWIDTH_1P6MS,                      /*!< UART IR Transmit pulse width is 1.6ms width */
}UART_IR_PulseWidth_Type;

/**
 *  @brief UART Baudrate Calculation type definition
 */
typedef enum
{
  UART_BAUDRATE_CAL_FORMULA = 0,                 /*!< Formula used to calculate baudrate */
  UART_BAUDRATE_CAL_TABLE,                       /*!< Table used to calculate baudrate */
}UART_BaudRateCal_Type;

/**
 *  @brief UART baudrate divider programme type definition
 */
typedef enum
{
  UART_BAUDRATE_PROGRAM_PROCESSOR = 0,            /*!< Processor Program UART Divisor Latch Register */
  UART_BAUDRATE_PROGRAM_UART,                     /*!< UART Program UART Divisor Latch Register */
}UART_BaudRateProgram_Type;

/**
 *  @brief UART interrupt type definition
 */
typedef enum
{
  UART_INT_ABL,                                 /*!< Auto Baudrate Lock interrupt type */
  UART_INT_RTO,                                 /*!< Receive Time-Out Interrupt type, cleared by hardware. Only happens rx fifo level < threshold */
  UART_INT_MODEM,                         /*!< Modem Interrupt type */
  UART_INT_TDR,                                 /*!< Transmit Data Request Interrupt type, cleared by writing data to fifo or read IIR */
  UART_INT_RDA,                                 /*!< Receive Data Available Interrupt type */
  UART_INT_RLSE,                         /*!< Receive Line Status Error Interrupt type */
  UART_INT_ALL,                                  /*!< All the interrupt */
}UART_INT_Type;

/**
 *  @brief UART Line status type definition
 */
typedef enum
{
  UART_LINESTATUS_FIFOE = 0,                     /*!< Indicate at least Parity Error, Framing Error or Break Indication Happens */
  UART_LINESTATUS_TEMT,                          /*!< Set when Transmit Holding register and Transmit Shift register are both empty */
  UART_LINESTATUS_TDRQ,                          /*!< Indicate UART is Ready to receive a new character for transmission */
  UART_LINESTATUS_BI,                            /*!< Break Inerrupt is set when reeived data is held low for longer than a full-word time */
  UART_LINESTATUS_FE,                             /*!< Framing Error bit indicates character without a stop bit */
  UART_LINESTATUS_PE,                             /*!< Parity error bit */
  UART_LINESTATUS_OE,                             /*!< Overrun error bit */
  UART_LINESTATUS_DR,                             /*!< Data ready bit */
}UART_LineStatus_Type;

/**
 *  @brief UART Modem status type definition
 */
typedef enum
{
  UART_MODEMSTATUS_DCTS = 0,                     /*!< Delta clear to send bit */
  UART_MODEMSTATUS_DDSR,                     /*!< Delta data set ready bit */
  UART_MODEMSTATUS_TERI,                     /*!< Trailing edge of ring indicator bit */
  UART_MODEMSTATUS_DDCD,                     /*!< Delta data carrier detect bit */
  UART_MODEMSTATUS_CTS,                             /*!< Clear to send bit */
  UART_MODEMSTATUS_DSR,                             /*!< Data set ready bit */
  UART_MODEMSTATUS_RI,                             /*!< Ring indicator bit */
  UART_MODEMSTATUS_DCD,                             /*!< Data carrier ditect bit */
}UART_ModemStatus_Type;

/**
 *  @brief UART Configuration Structure type definition: fixed with 1 stop bit
 */
typedef struct
{
  uint32_t baudRate;                             /*!< Uart Baudrate */
  UART_DataBits_Type dataBits;                   /*!< Uart Frame Length of Data Bit  */
  FunctionalState stickyParity;                  /*!< Force parity bit to be opposite of EPS bit value, when parity enabled. */
  UART_Parity_Type parity;                       /*!< Uart Parity check type */
  FunctionalState highSpeedUart;                 /*!< Enable or Disable High Speed Uart */
  FunctionalState nrzCodeing;                    /*!< Enable or Disable NRZ coding */
}UART_CFG_Type;

/**
 *  @brief UART FIFO Configuration Structure type definition
 */
typedef struct
{
  FunctionalState fifoEnable;                    /*!< Enable or Disable Uart Fifo Function*/
  FunctionalState autoFlowControl;               /*!< Auto flow control enable or disable */
  FunctionalState rxFifoReset;                   /*!< Reset uart receive fifo */
  FunctionalState txFifoReset;                   /*!< Reset uart transmit fifo */  
  UART_PeripheralBus_Type peripheralBusType;     /*!< Uart Peripheral Bus Type: 8-bits or 32-bits. Only valid when Fifo Mode Enabled. */
  FunctionalState fifoDmaEnable;                 /*!< Enable or Disable Uart Fifo DMA Function.*/
  UART_RxFIFOLevel_Type rxFifoLevel;             /*!< Uart Receive Fifo Interrupt Trigger Level. Only valid when Fifo Mode Enabled.*/
  UART_TxFIFOLevel_Type txFifoLevel;             /*!< Uart Transmit Fifo Interrupt Trigger Level. Only valid when Fifo Mode Enabled.*/
}UART_FifoCfg_Type;

/**
 *  @brief UART Infrared Configuration Structure type definition
 */
typedef struct
{
  FunctionalState txIrEnable;                    /*!< Enable or Disable Uart Transmit IR Function*/
  FunctionalState rxIrEnable;                    /*!< Enable or Disable Uart Receive IR Function*/
  UART_IR_Polarity_Type txIrPolarity;            /*!< Transmit IR Polarity type */
  UART_IR_Polarity_Type rxIrPolarity;            /*!< Receive IR Polarity type */
  UART_IR_PulseWidth_Type irPulseWidth;          /*!< IR Pulse Width type */
}UART_IrCfg_Type;

/**
 *  @brief UART Infrared Configuration Structure type definition
 */
typedef struct
{
  FunctionalState autoBaudEnable;                /*!< Enable or Disable Uart Auto Baud Function*/
  UART_BaudRateCal_Type baudCalType;             /*!< Uart Auto Baud calculation type */
  UART_BaudRateProgram_Type baudProgramType;     /*!< Uart Auto Baud Program type */
}UART_AutoBaudCfg_Type;

/*@} end of group UART_Public_Types */

/** @defgroup UART_Public_Constants
 *  @{
 */

/** @defgroup UART_ID
 *  @{
 */
#define IS_UART_ID(UART_ID)              ((UART_ID >= 0) && ((UART_ID) <= 2))
/*@} end of group UART_ID */

/** @defgroup UART_PARITY
 *  @{
 */
#define IS_UART_PARITY(UART_PARITY)      ((UART_PARITY >= 0) && ((UART_PARITY) <= 3))
/*@} end of group UART_PARITY */

/** @defgroup UART_DATA_BIT
 *  @{
 */
#define IS_UART_DATA_BIT(UART_DATA_BIT)      ((UART_DATA_BIT >= 0) && ((UART_DATA_BIT) <= 1))
/*@} end of group UART_DATA_BIT */

/** @defgroup UART_PERIPHERAL_BUS
 *  @{
 */
#define IS_UART_PERIPHERAL_BUS(UART_PERIPHERAL_BUS)      ((UART_PERIPHERAL_BUS >= 0) && ((UART_PERIPHERAL_BUS) <= 1))
/*@} end of group UART_PERIPHERAL_BUS */

/** @defgroup UART_TRAIL_BYTE
 *  @{
 */
#define IS_UART_TRAIL_BYTE(UART_TRAIL_BYTE)      ((UART_TRAIL_BYTE >= 0) && ((UART_TRAIL_BYTE) <= 1))
/*@} end of group UART_TRAIL_BYTE */

/** @defgroup UART_TX_FIFO_LEVEL
 *  @{
 */
#define IS_UART_TX_FIFO_LEVEL(UART_TX_FIFO_LEVEL)      ((UART_TX_FIFO_LEVEL >= 0) && ((UART_TX_FIFO_LEVEL) <= 1))
/*@} end of group UART_TX_FIFO_LEVEL */

/** @defgroup UART_RX_FIFO_LEVEL
 *  @{
 */
#define IS_UART_RX_FIFO_LEVEL(UART_RX_FIFO_LEVEL)      ((UART_RX_FIFO_LEVEL >= 0) && ((UART_RX_FIFO_LEVEL) <= 3))
/*@} end of group UART_RX_FIFO_LEVEL */

/** @defgroup UART_IR_POLARITY
 *  @{
 */
#define IS_UART_IR_POLARITY(UART_IR_POLARITY)      ((UART_IR_POLARITY >= 0) && ((UART_IR_POLARITY) <= 1))
/*@} end of group UART_IR_POLARITY */

/** @defgroup UART_IR_PULSE_WIDTH
 *  @{
 */
#define IS_UART_IR_PULSE_WIDTH(UART_IR_PULSE_WIDTH)      ((UART_IR_PULSE_WIDTH >= 0) && ((UART_IR_PULSE_WIDTH) <= 1))
/*@} end of group UART_IR_PULSE_WIDTH */

/** @defgroup UART_BAUD_CAL
 *  @{
 */
#define IS_UART_BAUD_CAL(UART_BAUD_CAL)      ((UART_BAUD_CAL >= 0) && ((UART_BAUD_CAL) <= 1))
/*@} end of group UART_BAUD_CAL */

/** @defgroup UART_BAUD_PROGRAM
 *  @{
 */
#define IS_UART_BAUD_PROGRAM(UART_BAUD_PROGRAM)      ((UART_BAUD_PROGRAM >= 0) && ((UART_BAUD_PROGRAM) <= 1))
/*@} end of group UART_BAUD_PROGRAM */

/** @defgroup UART_INT
 *  @{
 */
#define IS_UART_INT(UART_INT)      ((UART_INT >= 0) && ((UART_INT) <= UART_INT_ALL))
/*@} end of group UART_INT */

/** @defgroup UART_Stop_Bit
 *  @{
 */
#define IS_UART_STOPBITS(STOPBITS)        (((STOPBITS) == UART_STOPBITS_1) || \
                                           ((STOPBITS) == UART_STOPBITS_2))
/*@} end of group UART_Stop_Bit */

/** @defgroup UART_LINE_STATUS
 *  @{
 */
#define IS_UART_LINE_STATUS(UART_LINE_STATUS)      ((UART_LINE_STATUS >= 0) && ((UART_LINE_STATUS) <= 7))
/*@} end of group UART_LINE_STATUS */

/** @defgroup UART_MODEM_STATUS
 *  @{
 */
#define IS_UART_MODEM_STATUS(UART_MODEM_STATUS)      ((UART_MODEM_STATUS >= 0) && ((UART_MODEM_STATUS) <= 7))
/*@} end of group UART_LINE_STATUS */

/*@} end of group UART_Public_Constants */

/** @defgroup UART_Public_Macro
 *  @{
 */

/*@} end of group UART_Public_Macro */

/** @defgroup UART_Public_FunctionDeclaration
 *  @brief UART functions declaration
 *  @{
 */
Status UART_Init(UART_ID_Type uartNo, UART_CFG_Type* uartCfgStruct);
void UART_FifoConfig(UART_ID_Type uartNo, UART_FifoCfg_Type* fifoCfg);
void UART_IrConfig(UART_ID_Type uartNo, UART_IrCfg_Type* irCfg);
void UART_BaudConfig(UART_ID_Type uartNo, UART_AutoBaudCfg_Type* baudCfg);
void UART_Enable(UART_ID_Type uartNo);
void UART_Disable(UART_ID_Type uartNo);

void UART_SetBreakCmd(UART_ID_Type uartNo, FunctionalState newState);
void UART_SetStopBits(UART_ID_Type uartNo, UART_StopBits_Type stopBits);

void UART_SetLoopBackMode(UART_ID_Type uartNo, FunctionalState newState);
void UART_SetOut2Signal(UART_ID_Type uartNo, FlagStatus status);
void UART_SetOut1Signal(UART_ID_Type uartNo, FlagStatus status);
void UART_SetDTRSignal(UART_ID_Type uartNo, FlagStatus status);
void UART_RTS_Assert(UART_ID_Type uartNo);
void UART_RTS_Deassert(UART_ID_Type uartNo);
void UART_SetAutoBaudDL(UART_ID_Type uartNo, uint16_t autoBaudCount);

uint32_t UART_ReceiveData(UART_ID_Type uartNo);
void UART_SendData(UART_ID_Type uartNo, uint32_t data);
void UART_WriteLine(UART_ID_Type uartNo, uint8_t *buf);
uint16_t UART_Receive9bits(UART_ID_Type uartNo);
void UART_Send9bits(UART_ID_Type uartNo, uint16_t data);

uint16_t UART_GetAutoBaudCount(UART_ID_Type uartNo);
uint32_t UART_GetRxFIFOLevel(UART_ID_Type uartNo);
FlagStatus UART_GetLineStatus(UART_ID_Type uartNo,UART_LineStatus_Type lineStatus);
FlagStatus UART_GetModemStatus(UART_ID_Type uartNo,UART_ModemStatus_Type modemStatus);

void UART_IntMask(UART_ID_Type uartNo,UART_INT_Type intType, IntMask_Type intMask);

void UART0_IRQHandler(void);
void UART1_IRQHandler(void);
void UART2_IRQHandler(void);

void UART_DmaCmd(UART_ID_Type uartNo, FunctionalState cmd);

/*@} end of group UART_Public_FunctionDeclaration */

/*@} end of group UART  */

/*@} end of group MW300_Periph_Driver */
#endif /* __MW300_UART_H__ */
