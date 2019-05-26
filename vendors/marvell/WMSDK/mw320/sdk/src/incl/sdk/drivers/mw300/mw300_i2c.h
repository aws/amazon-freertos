/** @file mw300_i2c.h
*
*  @brief This file contains I2C driver module header
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

/****************************************************************************//*
 * @version  V1.3.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 ******************************************************************************/

#ifndef __MW300_I2C_H__
#define __MW300_I2C_H__

#include "mw300.h"
#include "mw300_driver.h"

/** @addtogroup MW300_Periph_Driver
 *  @{
 */

/** @addtogroup I2C
 *  @{
 */

/** @defgroup I2C_Public_Types
 *  @{
 */

/**
 *  @brief I2C master and slave type definition
 */
typedef enum
{
  I2C_MASTER = 0,                     /*!< I2C master mode define */
  I2C_SLAVE,                          /*!< I2C slave mode define */
}I2C_Mode_Type;

/**
 *  @brief I2C speed mode type definition
 */
typedef enum
{
  I2C_SPEED_STANDARD = 1,             /*!< I2C standard speed define */
  I2C_SPEED_FAST,                     /*!< I2C fast speed define */
  I2C_SPEED_HIGH,
}I2C_SpeedMode_Type;

/**
 *  @brief I2C address mode type definition
 */
typedef enum
{
  I2C_ADDR_BITS_7,                    /*!< I2C 7 bits address mode define */
  I2C_ADDR_BITS_10,                   /*!< I2C 10 bits address mode define */
}I2C_AddrMode_Type;

/**
 *  @brief I2C Configuration Structure type definition
 */
typedef struct
{
  I2C_Mode_Type masterSlaveMode;      /*!< Specifies the I2C mode.
                                           - I2C_MASTER: I2C master enable
                                           - I2C_SLAVE: I2C slave enable */

  I2C_SpeedMode_Type speedMode;       /*!< Specifies the I2C speed mode.
                                           - I2C_SPEED_STANDARD: standard speed (100kbits/s)
                                           - I2C_SPEED_FAST: fast speed (400kbits/s) */

  I2C_AddrMode_Type masterAddrBitMode;/*!< Specifies the I2C start its transfer in 7 or 10 bits address mode
                                           when acting as master.
                                           - I2C_ADDR_BITS_7: 7 bits address mode
                                           - I2C_ADDR_BIT_10: 10 bits address mode */

  I2C_AddrMode_Type slaveAddrBitMode; /*!< Specifies the I2C response to 7 or 10 bits address mode
                                           when acting as slave.
                                           - I2C_ADDR_BITS_7: 7 bits address mode
                                           - I2C_ADDR_BIT_10: 10 bits address mode */

  uint32_t ownSlaveAddr;              /*!< Slave address when the I2C is operating as a slave.
                                           This parameter can be a 7-bit or 10-bit address */

  FunctionalState restartEnable;      /*!< Determines whether RESTART conditions may be sent when acting
                                           as a master.
                                           - ENABLE: enable RESTART condition
                                           - DISABLE: disable RESTART condition */

}I2C_CFG_Type;

/**
 *  @brief I2C speed clock count type definition
 */
typedef struct
{
  uint32_t stdSpeedClkHighCnt;        /*!< Standard speed I2C clock SCL high count. The minimum valid value is 6 */

  uint32_t stdSpeedClkLowCnt;         /*!< Standard speed I2C clock SCL low count. The minimum valid value is 8 */

  uint32_t fastSpeedClkHighCnt;       /*!< Fast speed I2C clock SCL high count. The minimum valid value is 6 */

  uint32_t fastSpeedClkLowCnt;        /*!< Fast speed I2C clock SCL low count. The minimum valid value is 8 */

  uint32_t highSpeedClkHighCnt;       /*!< High speed I2C clock SCL high count. The minimum valid value is 6 */

  uint32_t highSpeedClkLowCnt;        /*!< High speed I2C clock SCL low count. The minimum valid value is 8 */

}I2C_SpeedClkCnt_Type;

/**
 *  @brief I2C FIFO threshold level type definition
 */
typedef struct
{
  uint32_t recvFifoThr;               /*!< Receive FIFO threshold level, the valid range is 0 - 15 */

  uint32_t transmitFifoThr;           /*!< Transmit FIFO threshold level, the valid range is 0 - 15 */

}I2C_FIFO_Type;

/**
 *  @brief I2C DMA request FIFO level type definition
 */
typedef struct
{
  uint32_t I2C_DMA_TransmitReqLevel;  /*!< Transmit data level, the valid range is 0 - 15 */

  uint32_t I2C_DMA_RecvReqLevel;      /*!< Receive data level, the valid range is 0 - 15 */

}I2C_DmaReqLevel_Type;

/**
 *  @brief I2C interrupt type definition
 */
typedef enum
{
  I2C_INT_RX_UNDER,                   /*!< Receive FIFO underflow interrupt */

  I2C_INT_RX_OVER,                    /*!< Receive FIFO overflow interrupt */

  I2C_INT_RX_FULL,                    /*!< Receive FIFO full interrupt */

  I2C_INT_TX_OVER,                    /*!< Transmit FIFO overflow interrupt */

  I2C_INT_TX_EMPTY,                   /*!< Transmit FIFO empty interrupt */

  I2C_INT_RD_REQ,                     /*!< Read request interrupt. It is set when I2C as a slave has been addressed
                                           by a remote master that is asking for data to be transferred */

  I2C_INT_TX_ABORT,                   /*!< Transmit abort interrupt */

  I2C_INT_RX_DONE,                    /*!< Transmit done interrupt. When I2C is acting as a slave-transmitter, this bit
                                           is set to 1 if the master does not acknowledge a transmitted byte. This
                                           occurs on the last byte of the transmission, indicating that the transmission
                                           is done */

  I2C_INT_ACTIVITY,                   /*!< Bus activity interrupt */

  I2C_INT_STOP_DET,                   /*!< Stop condition occur interrupt.Indicates whether a STOP condition has occurred
                                           on the I2C interface */

  I2C_INT_START_DET,                  /*!< Stop condition occur interrupt. Indicates whether a START or RESTART condition
                                           has occurred on the I2C interface */

  I2C_INT_GEN_CALL,                   /*!< General call interrupt. Set only when a General Call address is received and
                                           it is acknowledged. */

  I2C_INT_ALL,                        /*!< All the interrupts */

}I2C_INT_Type;

/**
 *  @brief I2C tx abort interrupt type definition
 */
typedef enum
{
  ABORT_7B_ADDR_NOACK,                /*!< Master is in 7-bit addressing mode and the address sent was not acknowledged
                                           by any slave */

  ABORT_10ADDR1_NOACK,                /*!< Master is in 10-bit address mode and the first 10-bit address byte was not
                                           acknowledged by any slave. */

  ABORT_10ADDR2_NOACK,                /*!< Master is in 10-bit address mode and the second address byte of the 10-bit address
                                           was not acknowledged by any slave. */

  ABORT_TXDATA_NOACK,                 /*!< This is a master-mode only bit. Master has received an acknowledgement for the
                                           address, but when it sent data byte(s) following the address, it did not receive an
                                           acknowledge from the remote slave(s). */

  ABORT_GCALL_NOACK,                  /*!< I2C in master mode sent a General Call and no slave on the bus
                                           acknowledged the General Call. */

  ABORT_GCALL_READ,                   /*!< I2C in master mode sent a General Call but the user programmed the
                                           byte following the General Call to be a read from the bus */

  ABORT_SBYTE_ACKDET,                 /*!< Master has sent a START Byte and the START Byte was acknowledged (wrong behavior) */

  ABORT_HS_NORSTRT,                   /*!< The restart is disabled and the user is trying to send a START Byte */

  ABORT_SBYTE_NORSTRT,                /*!< The start is disabled and the user is trying to send a START Byte */

  ABORT_10B_RD_NORSTRT,               /*!< The restart is disabled and the master sends a read command in 10-bit addressing mode. */

  ABORT_MASTER_DIS,                   /*!< User tries to initiate a Master operation with the Master mode disabled */

  ABORT_ARB_LOST,                     /*!< Master has lost arbitration, or if IC_TX_ABRT_SOURCE[14] is also set, then
                                           the slave transmitter has lost arbitration. */

  ABORT_SLVFLUSH_TXFIFO,              /*!< Slave has received a read command and some data exists in the TX FIFO so the slave
                                           issues a TX_ABRT interrupt to flush old data in TX FIFO. */

  ABORT_SLV_ARBLOST,                  /*!< Slave lost the bus while transmitting data to a remote master. IC_TX_ABRT_SOURCE[12]
                                           is set at the same time. */

  ABORT_SLVRD_INTX,                   /*!< When the processor side responds to a slave mode request for data to be transmitted
                                           to a remote master and user writes a 1 in Read/Write bit of DATA_CMD register. */
}I2C_TxAbortINT_Type;

/**
 *  @brief I2C status type definition
 */
typedef enum
{
  I2C_STATUS_ACTIVITY,                /*!< I2C activity status */

  I2C_STATUS_TFNF,                    /*!< Transmit FIFO not full status */

  I2C_STATUS_TFE,                     /*!< Transmit FIFO completely empty status */

  I2C_STATUS_RFNE,                    /*!< Receive FIFO not empty status */

  I2C_STATUS_RFF,                     /*!< Receive FIFO completely full status */

  I2C_STATUS_MST_ACTIVITY,            /*!< Master FSM activity status */

  I2C_STATUS_SLV_ACTIVITY,            /*!< Master FSM activity status */
}I2C_STATUS_Type;

/**
 *  @brief I2C No. type definition
 */
typedef enum
{
  I2C0_PORT = 0,                      /*!< I2C0 port define */
  I2C1_PORT,                          /*!< I2C1 port define */
}I2C_ID_Type;

/*@} end of group I2C_Public_Types definitions */

/** @defgroup I2C_Public_Constants
 *  @{
 */

/** @defgroup I2C_Port
 *  @{
 */
#define IS_I2C_PERIPH(PERIPH)         (((PERIPH) == I2C0_PORT) || \
                                       ((PERIPH) == I2C1_PORT))

#define IS_I2C_INTSTAT_SET(__VAR32__, __BIT__)	\
	(__VAR32__ & (0x01 << __BIT__))

/*@} end of group I2C_Port definitions*/

/** @defgroup I2C_mode
 *  @{
 */
#define IS_I2C_MODE(MODE)             (((MODE) == I2C_SLAVE) || \
                                       ((MODE) == I2C_MASTER))

/*@} end of group I2C_mode definitions*/

/** @defgroup I2C_address_mode
 *  @{
 */
#define IS_I2C_ADDRESSMODE(ADDMODE)   (((ADDMODE) == I2C_ADDR_BITS_7) || \
                                       ((ADDMODE) == I2C_ADDR_BITS_10))

/*@} end of group I2C_address_mode definitions*/

/** @defgroup I2C_speed_mode
 *  @{
 */
#define IS_I2C_SPEED(SPEED)           (((SPEED) == I2C_SPEED_STANDARD) || \
                                       ((SPEED) == I2C_SPEED_FAST))

/*@} end of group I2C_speed_mode definitions*/

/** @defgroup I2C_Status_mode
 *  @{
 */
#define IS_I2C_STATUS_BIT(BIT)        ((BIT) < I2C_STATUS_SLV_ACTIVITY)

/*@} end of group I2C_Status_mode */

/** @defgroup I2C_INT_TYPE
 *  @{
 */
#define IS_I2C_INT_TYPE(INT_TYPE)     ((INT_TYPE) < I2C_INT_ALL)

/*@} end of group I2C_INT_TYPE */

/** @defgroup I2C_ABORT_TYPE
 *  @{
 */
#define IS_I2C_ABORT_TYPE(ABORT)      ((ABORT) < ABORT_SLVRD_INTX)

/*@} end of group I2C_ABORT_TYPE */

/** @defgroup I2C_INT_MASK
 *  @{
 */
#define IS_I2C_INTMASK(INTMASK)       (((INTMASK) == MASK) || \
                                       ((INTMASK) == UNMASK))

/*@} end of group I2C_INT_MASK */

/*@} end of group I2C_Public_Constants definitions*/

/** @defgroup I2C_Public_Macro
 *  @{
 */

/*@} end of group I2C_Public_Macro */

/** @defgroup I2C_Public_FunctionDeclaration
 *  @brief I2C function statement
 *  @{
 */
void I2C_Init(I2C_ID_Type i2cNo, I2C_CFG_Type* i2cCfgStruct);
void I2C_DmaConfig(I2C_ID_Type i2cNo, I2C_DmaReqLevel_Type* i2cDmaCfg);

void I2C_SetHsMAddr(I2C_ID_Type i2cNo, uint32_t hsMAddr);

void I2C_SetSlaveAddr(I2C_ID_Type i2cNo, uint32_t slaveAddr);
void I2C_Enable(I2C_ID_Type i2cNo);
void I2C_Disable(I2C_ID_Type i2cNo);
void I2C_FIFOConfig(I2C_ID_Type i2cNo, I2C_FIFO_Type* fifoCfg);
void I2C_DmaCmd(I2C_ID_Type i2cNo, FunctionalState transmitDmaCtrl, FunctionalState rcvDmaCtrl);
void I2C_SetClkCount(I2C_ID_Type i2cNo, I2C_SpeedClkCnt_Type* clkCnt);
void I2C_GeneralCall(I2C_ID_Type i2cNo, FunctionalState newState);
void I2C_StartByteCmd(I2C_ID_Type i2cNo, FunctionalState newState);
void I2C_GeneralCallAckCmd(I2C_ID_Type i2cNo, FunctionalState newState);
void I2C_SendByte(I2C_ID_Type i2cNo, uint8_t data);
uint8_t I2C_ReceiveByte(I2C_ID_Type i2cNo);
void I2C_MasterReadCmd(I2C_ID_Type i2cNo);

void I2C_IntMask(I2C_ID_Type i2cNo, I2C_INT_Type intType, IntMask_Type intMask);
FlagStatus I2C_GetIntStatus(I2C_ID_Type i2cNo, I2C_INT_Type intType);
FlagStatus I2C_GetTxAbortStatus(I2C_ID_Type i2cNo, I2C_TxAbortINT_Type abortType);
FlagStatus I2C_GetStatus(I2C_ID_Type i2cNo, I2C_STATUS_Type statusType);
FlagStatus I2C_GetRawIntStatus(I2C_ID_Type i2cNo, I2C_INT_Type statusType);
void I2C_IntClr(I2C_ID_Type i2cNo, I2C_INT_Type intType);
void I2C_GetTxAbortCause(I2C_ID_Type i2cNo, uint32_t *tx_abrt_reg);

void I2C0_IRQHandler(void);
void I2C1_IRQHandler(void);

uint32_t I2C_GetFullIntStatus(I2C_ID_Type i2cNo);

/*@} end of group I2C_Public_FunctionDeclaration */

/*@} end of group I2C */

/*@} end of group MW300_Periph_Driver */

#endif /* End of __MW300_I2C_H__ */
