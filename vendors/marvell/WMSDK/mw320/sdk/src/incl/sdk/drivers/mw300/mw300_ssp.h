/** @file mw300_ssp.h
*
*  @brief This file contains SSP driver module header
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
 * @version  V1.0.0
 * @date     12-Aug-2013
 * @author   CE Application Team
 ******************************************************************************/

#ifndef __MW300_SSP_H__
#define __MW300_SSP_H__

#include "mw300.h"
#include "mw300_driver.h"

/** @addtogroup  MW300_Periph_Driver
 *  @{
 */

/** @addtogroup  SSP
 *  @{
 */

/** @defgroup SSP_Public_Types SSP_Public_Types
 *  @{
 */

/**
 *  @brief SSP No. type definition
 */
typedef enum
{
  SSP0_ID = 0,	                            /*!< SSP0 port define */
  SSP1_ID,                                  /*!< SSP1 port define */
  SSP2_ID,                                  /*!< SSP2 port define */
}SSP_ID_Type;

/**
 *  @brief SSP mode definition
 */
typedef enum
{
  SSP_NORMAL = 0,                           /*!< SSP0 port define */
  SSP_NETWORK,                              /*!< SSP1 port define */
}SSP_MODE_Type;

/**
 *  @brief SSP mode type definition
 */
typedef enum
{
  SSP_MASTER = 0,                           /*!< SSP Master mode define */
  SSP_SLAVE,                                /*!< SSP Slave mode define */
}SSP_MS_Type;

/**
 *  @brief SSP frame format type definition
 */
typedef enum
{
  SSP_FRAME_SPI = 0,                        /*!< SSP frame format: SPI mode */
  SSP_FRAME_SSP,                            /*!< SSP frame format: SSP mode */
  SSP_FRAME_PSP = 3,                        /*!< SSP frame format: PSP mode */
}SSP_FrameFormat_Type;

/**
 *  @brief SSP data size type definition
 */
typedef enum
{
  SSP_DATASIZE_8 = 0,                       /*!< SSP data size length: 8 bits */
  SSP_DATASIZE_16,                          /*!< SSP data size length: 16 bits */
  SSP_DATASIZE_18,                          /*!< SSP data size length: 18 bits */
  SSP_DATASIZE_32,                          /*!< SSP data size length: 32 bits */
}SSP_DataSize_Type;

/**
 *  @brief SSP Network Mode Busy Status definition
 */
typedef enum
{
  SSP_NWK_FREE = 0,                         /*!< SSP Network Mode: no frame is currently active */
  SSP_NWK_BUSY,                             /*!< SSP Network Mode: a frame is currently active */
}SSP_NWK_Stat;

/**
 *  @brief SSP slave clock free running type definition
 */
typedef enum
{
  SSP_SLAVECLK_CONTINUOUS = 0,              /*!< SSP slave clock free running */
  SSP_SLAVECLK_TRANSFER,                    /*!< SSP slave clock active when data transfers */
}SSP_SlaveClk_Type;

/**
 *  @brief SSP slave clock free running type definition
 */
typedef enum
{
  SSP_TXD3STATE_12SLSB = 0,                 /*!< SSP TXDx is 3 state 1/2 clock cycle after start LSB */
  SSP_TXD3STATE_ELSB,                       /*!< SSP TXDx is 3 state on clock edge ends LSB */
}SSP_Txd3State_Type;

/**
 *  @brief SSP serial frame polarity type
 */
typedef enum
{
  SSP_SAMEFRM_PSP = 0,                      /*!< SSP frame polarity is determined by psp polarity bit */
  SSP_INVFRM_PSP,                           /*!< SSP frame polarity is inverted from psp polarity bit */
}SSP_SeriFrmPola_Type;

/**
 *  @brief SSP transmit and receive mode type definition
 */
typedef enum
{
  SSP_TR_MODE = 0,                          /*!< SSP works at transmit/receive mode */
  SSP_RX_MODE,                              /*!< SSP works at receive without transmit mode */
}SSP_TR_Type;

/**
 *  @brief SPI serial clock phase type
 */
typedef enum
{
  SPI_SCPHA_1 = 0,                          /*!< SPI clock is inactive until 1 cycle after frame start */
  SPI_SCPHA_1_2,                            /*!< SPI clock is inactive until 1/2 cycle after frame start */
}SPI_ClkPhase_Type;

/**
 *  @brief SPI serial clock polarity type
 */
typedef enum
{
  SPI_SCPOL_LOW = 0,                        /*!< Inactive state of SSPSCLK is LOW */
  SPI_SCPOL_HIGH,                           /*!< Inactive state of SSPSCLK is HIGH */
}SPI_ClkPolarity_Type;

/**
 *  @brief PSP frame sync relative timing bit type
 */
typedef enum
{
  PSP_AFTER_DMYSTOP = 0,                    /*!< PSP next frame is asserted after the end of DMTSTOP timing */
  PSP_WITH_LSB,                             /*!< PSP next frame is asserted with the LSB of the previous frame */
}PSP_FSRT_Type;

/**
 *  @brief PSP end of transfer data state type
 */
typedef enum
{
  PSP_ENDTRANS_LOW = 0,                     /*!< PSP end of transfer data state: LOW */
  PSP_ENDTRANS_LASTVAL,                     /*!< PSP end of transfer data state: Last Value(Bit0) */
}PSP_EndOfTransfer_Type;

/**
 *  @brief serial frame polarity type
 */
typedef enum
{
  PSP_SFRMP_LOW = 0,                        /*!< PSP serial frame polarity: LOW */
  PSP_SFRMP_HIGH,                           /*!< PSP serial frame polarity: HIGN */
}PSP_SeriFrmPola_Type;

/**
 *  @brief serial frame polarity type
 */
typedef enum
{
  PSP_DRIVFALL_SAMPRISE_IDLELOW = 0,        /*!< Data driver(falling), data sample(rising), idle(low) */
  PSP_DRIVRISE_SAMPFALL_IDLELOW,            /*!< Data driver(rising), data sample(falling), idle(low) */
  PSP_DRIVRISE_SAMPFALL_IDLEHIGH,           /*!< Data driver(rising), data sample(falling), idle(high) */
  PSP_DRIVFALL_SAMPRISE_IDLEHIGH,           /*!< Data driver(falling), data sample(rising), idle(high) */
}PSP_SeriClkMode_Type;

/**
 *  @brief SSP interrupt type definition
 */
typedef enum
{
  SSP_INT_TFURI = 1,                        /*!< Transmit Fifo Underrun interrupt enable */
  SSP_INT_RFORI,                            /*!< Receive Fifo Overrun interrupt enable*/
  SSP_INT_BCEI,                             /*!< Bit Count Error interrupt enable */
  SSP_INT_RTOI,                             /*!< Receiver Time Out interrupt enable */
  SSP_INT_TFEI,                             /*!< Transmit Fifo Empty interrupt enable */
  SSP_INT_RFFI,                             /*!< Receive Fifo Full interrupt enable */
  SSP_INT_ALL,                              /*!< All the interrupt */
}SSP_INT_Type;

/**
 *  @brief SSP status type definition
 */
typedef enum
{
  SSP_STATUS_RXOSS = 1,                     /*!< Rx Fifo Odd sample status */
  SSP_STATUS_TXOSS,                         /*!< Tx Fifo Odd sample status */
  SSP_STATUS_BCE,                           /*!< Bit Count Error status */
  SSP_STATUS_CSS,                           /*!< Clock synchronnization status */
  SSP_STATUS_TFUR,                          /*!< Transmit Fifo underrun status */
  SSP_STATUS_RTOI,                          /*!< Receiver Time Out interrupt status */
  SSP_STATUS_RFOR,                          /*!< Receive Fifo Overrun status */
  SSP_STATUS_RFF,                           /*!< Receive Fifo full status */
  SSP_STATUS_TFE,                           /*!< Transmit Fifo Empty status */
  SSP_STATUS_BUSY,                          /*!< Busy status */
  SSP_STATUS_RFNE,                          /*!< Receive Fifo not empty status */
  SSP_STATUS_TFNF,                          /*!< Transmit Fifo not full status */
}SSP_Status_Type;

/**
 *  @brief SSP No. type definition
 */
typedef enum
{
  HIGHER_SPEED_1 = 0,                       /*!< Higher speed define */
  HIGHER_SPEED_2,                           /*!< Much higher speed define */
}SSP_HigherSpeed_Type;

/**
 *  @brief SSP configuration structure type definition
 */
typedef struct
{
  SSP_MODE_Type  mode;
  SSP_FrameFormat_Type frameFormat;         /*!< SSP frame format type */
  SSP_MS_Type masterOrSlave;                /*!< SSP function mode: Master or Slave */
  SSP_TR_Type trMode;                       /*!< SSP tx/rx mode: SSP_TR_MODE or SSP_RX_MODE */
  SSP_DataSize_Type dataSize;               /*!< SSP frame data size */
  SSP_SeriFrmPola_Type sfrmPola;            /*!< SSP serial frame polarity type */
  SSP_SlaveClk_Type slaveClkRunning;        /*!< SSP slave clock running type */
  SSP_Txd3State_Type txd3StateType;         /*!< SSP txd turns to three state type */
  FunctionalState txd3StateEnable;          /*!< Enable or Disable SSP turns txd three state mode */
  uint32_t timeOutVal;
}SSP_CFG_Type;

/**
 *  @brief SSP network mode configuration structure type definition
 */
typedef struct
{
  uint8_t frameRateDiv;                    /*!< SSP Network Time Slot: 0-7 indicate time slot number 1-8 */
  uint8_t txTimeSlotActive;                /*!< SSP tx time slot active bits
                                                 0: SSP does not transmit data in this time slot
                                                 1: SSP transmit data in this time slot */
  uint8_t rxTimeSlotActive;                /*!< SSP tx time slot active bits
                                                 0: SSP does not receive data in this time slot
                                                 1: SSP receive data in this time slot */
}SSP_NWK_Type;

/**
 *  @brief SSP Fifo configuration structure type definition
 */
typedef struct
{
  FunctionalState fifoPackMode;             /*!< Enable or Disale SSP Fifo packed mode */
  uint32_t rxFifoFullLevel;                 /*!< SSP receive fifo full threshold */
  uint32_t txFifoEmptyLevel;                /*!< SSP transmit fifo empty threshold */
  FunctionalState rxDmaService;             /*!< Enable or Diasble SSP receive fifo dma service request */
  FunctionalState txDmaService;             /*!< Enable or Diasble SSP transmit fifo dma service request */
}SSP_FIFO_Type;

/**
 *  @brief SPI phase and polarity configuration structure type definition
 */
typedef struct
{
  SPI_ClkPhase_Type spiClkPhase;            /*!< SPI clock phase */
  SPI_ClkPolarity_Type spiClkPolarity;      /*!< SPI clock polarity */
}SPI_Param_Type;

/**
 *  @brief PSP parameter configuration structure type definition
 */
typedef struct
{
  PSP_FSRT_Type pspFsrtType;                /*!< PSP frame sync relative time mode */
  PSP_SeriClkMode_Type pspClkMode;          /*!< PSP serial clock type */
  PSP_SeriFrmPola_Type pspFrmPola;          /*!< PSP serial frame polarity */
  PSP_EndOfTransfer_Type pspEndTransState;  /*!< PSP end of transfer type */
  uint32_t startDelay;                      /*!< PSP start delay: clock cycle */
  uint32_t dummyStart;                      /*!< PSP dummy start: clock cycle */
  uint32_t dummyStop;                       /*!< PSP dummy stop: clock cycle */
  uint32_t frmDelay;                        /*!< PSP frame delay: one-half clock cycle */
  uint32_t frmLength;                       /*!< PSP frame length: clock cycle */
}PSP_Param_Type;

/*@} end of group SSP_Public_Types */

/** @defgroup SSP_Public_Constants
 *  @{
 */

/** @defgroup SSP_ID
 *  @{
 */
#define IS_SSP_PERIPH(PERIPH)               (((PERIPH) == SSP0_ID) || \
                                             ((PERIPH) == SSP1_ID) || \
                                             ((PERIPH) == SSP2_ID))
                                             
/*@} end of group SSP_ID definitions */

/** @defgroup SSP_MODE_TYPE
 *  @{
 */
#define IS_SSP_MODE(MODE)                   (((MODE) == SSP_NORMAL) || \
                                             ((MODE) == SSP_NETWORK))

/*@} end of group SSP_MODE_TYPE definitions */
                                             
/** @defgroup SSP_MASTER_SLAVE
 *  @{
 */
#define IS_SSP_MS(MS)                       (((MS) == SSP_MASTER) || \
                                             ((MS) == SSP_SLAVE))

/*@} end of group SSP_MASTER_SLAVE definitions */

/** @defgroup SSP_FRAME_FORMAT
 *  @{
 */
#define IS_SSP_FRAMEFORMAT(FRAMEFORMAT)     (((FRAMEFORMAT) == SSP_FRAME_SPI) || \
                                             ((FRAMEFORMAT) == SSP_FRAME_SSP) || \
                                             ((FRAMEFORMAT) == SSP_FRAME_PSP))

/*@} end of group SSP_FRAME_FORMAT definitions */

/** @defgroup SSP_DATA_SIZE
 *  @{
 */
#define IS_SSP_DATASIZE(DATASIZE)           (((DATASIZE) == SSP_DATASIZE_8) || \
                                             ((DATASIZE) == SSP_DATASIZE_16) || \
                                             ((DATASIZE) == SSP_DATASIZE_18) || \
                                             ((DATASIZE) == SSP_DATASIZE_32))

/*@} end of group SSP_DATA_SIZE definitions */                                             

/** @defgroup SSP_SLAVE_CLK
 *  @{
 */
#define IS_SSP_SLAVECLK(SLAVECLK)           (((SLAVECLK) == SSP_SLAVECLK_CONTINUOUS) || \
                                             ((SLAVECLK) == SSP_SLAVECLK_TRANSFER))

/*@} end of group SSP_SLAVE_CLK definitions */                                              
                                             
/** @defgroup SSP_NWK_STAT
 *  @{
 */
#define IS_SSP_NETWORKSTAT(NETWORKSTAT)     (((NETWORKSTAT) == SSP_NETWORK_FREE) || \
                                             ((NETWORKSTAT) == SSP_NETWORK_BUSY))

/*@} end of group SSP_NWK_STAT definitions */                                               
                                            
/** @defgroup SSP_TXD_3STATE
 *  @{
 */
#define IS_SSP_TXD3STATE(TXD3STATE)         (((TXD3STATE) == SSP_TXD3STATE_12SLSB) || \
                                             ((TXD3STATE) == SSP_TXD3STATE_ELSB))

/*@} end of group SSP_TXD_3STATE definitions */                                             
                                             
/** @defgroup SSP_FRAME_POLARITY
 *  @{
 */
#define IS_SSP_FRMPOLARITY(FRMPOLARITY)     (((FRMPOLARITY) == SSP_SAMEFRM_PSP) || \
                                             ((FRMPOLARITY) == SSP_INVFRM_PSP))

/*@} end of group SSP_FRAME_POLARITY definitions */                                              
                                             
/** @defgroup SSP_TR_TYPE
 *  @{
 */
#define IS_SSP_TRTYPE(TRTYPE)               (((TRTYPE) == SSP_TR_MODE) || \
                                             ((TRTYPE) == SSP_RX_MODE))

/*@} end of group SSP_TR_TYPE definitions */                                               
                                             
/** @defgroup SSP_CLOCK_PHASE
 *  @{
 */
#define IS_SPI_CLKPHASE(CLKPHASE)           (((CLKPHASE) == SPI_SCPHA_1) || \
                                             ((CLKPHASE) == SPI_SCPHA_1_2))

/*@} end of group SSP_CLOCK_PHASE definitions */                                              
                                             
/** @defgroup SSP_CLOCK_POLARITY
 *  @{
 */
#define IS_SPI_CLKPOLARITY(CLKPOLARITY)     (((CLKPOLARITY) == SPI_SCPOL_LOW) || \
                                             ((CLKPOLARITY) == SPI_SCPOL_HIGH))

/*@} end of group SSP_CLOCK_POLARITY definitions */                                              
                                             
/** @defgroup PSP_FSRT_TYPE
 *  @{
 */
#define IS_PSP_FSRTTYPE(FSRTTYPE)           (((FSRTTYPE) == PSP_AFTER_DMYSTOP) || \
                                             ((FSRTTYPE) == PSP_WITH_LSB))

/*@} end of group PSP_FSRT_TYPE definitions */                                               
                                             
/** @defgroup PSP_ENDOFTRANS_TYPE
 *  @{
 */
#define IS_PSP_ENDTRANSTYPE(ENDTRANSTYPE)   (((ENDTRANSTYPE) == PSP_ENDTRANS_LOW) || \
                                             ((ENDTRANSTYPE) == PSP_ENDTRANS_LASTVAL))

/*@} end of group PSP_ENDOFTRANS_TYPE definitions */                                                                                            
                                             
/** @defgroup PSP_FRAME_POLARITY
 *  @{
 */
#define IS_PSP_FRMPOLARITY(FRMPOLARITY)     (((FRMPOLARITY) == PSP_SFRMP_LOW) || \
                                             ((FRMPOLARITY) == PSP_SFRMP_HIGH))

/*@} end of group PSP_FRAME_POLARITY definitions */                                               
                                             
/** @defgroup PSP_CLOCK_MODE
 *  @{
 */
#define IS_PSP_CLKMODE(CLKMODE)             (((CLKMODE) == PSP_DRIVFALL_SAMPRISE_IDLELOW) || \
                                             ((CLKMODE) == PSP_DRIVRISE_SAMPFALL_IDLELOW) || \
                                             ((CLKMODE) == PSP_DRIVRISE_SAMPFALL_IDLEHIGH) || \
                                             ((CLKMODE) == PSP_DRIVFALL_SAMPRISE_IDLEHIGH))

/*@} end of group PSP_CLOCK_MODE definitions */                                               
                                             
/** @defgroup SSP_INT_TYPE
 *  @{
 */
#define IS_SSP_INT_TYPE(INT_TYPE)           (((INT_TYPE) > 0) && (INT_TYPE < 8))

/*@} end of group SSP_INT_TYPE definitions */   

/** @defgroup SSP_STATUS_TYPE
 *  @{
 */
#define IS_SSP_STATUS_TYPE(STATUS_TYPE)     (((STATUS_TYPE) > 0) && (STATUS_TYPE < 13))

/*@} end of group SSP_STATUS_TYPE definitions */

/** @defgroup SSP_HIGHERSPEED_TYPE
 *  @{
 */
#define IS_SSP_HIGHERSPEED(HIGHERSPEED)     (((HIGHERSPEED) == HIGHER_SPEED_1) || \
                                             ((HIGHERSPEED) == HIGHER_SPEED_2))

/*@} end of group SSP_HIGHERSPEED_TYPE definitions */

/*@} end of group SSP_Public_Constants */

/** @defgroup SSP_Public_Macro
 *  @{
 */

/*@} end of group SSP_Public_Macro */

/** @defgroup SSP_Public_FunctionDeclaration
 *  @brief SSP functions declaration
 *  @{
 */
void SSP_Init(SSP_ID_Type sspNo, SSP_CFG_Type* sspCfgStruct);
void SSP_FifoConfig(SSP_ID_Type sspNo, SSP_FIFO_Type* sspFifoCfg);
void SSP_NwkConfig(SSP_ID_Type sspNo, SSP_NWK_Type* sspNwkCfg);
void SPI_Config(SSP_ID_Type sspNo, SPI_Param_Type* spiParaStruct);
void PSP_Config(SSP_ID_Type sspNo, PSP_Param_Type* pspParaStruct);
Status PSP_ExitFromNwk(SSP_ID_Type sspNo);
void SSP_HigherSpeedConfig(SSP_ID_Type sspNo, SSP_HigherSpeed_Type HigherSpd, FunctionalState newState);

void SSP_Enable(SSP_ID_Type sspNo);
void SSP_Disable(SSP_ID_Type sspNo);

void SSP_SendData(SSP_ID_Type sspNo, uint32_t data);
uint32_t SSP_RecvData(SSP_ID_Type sspNo);

void SSP_SetLoopBackMode(SSP_ID_Type sspNo, FunctionalState newState);
uint32_t SSP_GetTxFifoLevel(SSP_ID_Type sspNo);
uint32_t SSP_GetRxFifoLevel(SSP_ID_Type sspNo);

void SSP_IntMask(SSP_ID_Type sspNo, SSP_INT_Type intType, IntMask_Type intMask);
Status SSP_IntClr(SSP_ID_Type sspNo, SSP_INT_Type intType);
FlagStatus SSP_GetStatus(SSP_ID_Type sspNo, SSP_Status_Type sspStatus);
SSP_NWK_Stat SSP_GetNwkStatus(SSP_ID_Type sspNo);
uint8_t SSP_GetActiveTimeSlot(SSP_ID_Type sspNo);

void SSP0_IRQHandler(void);
void SSP1_IRQHandler(void);
void SSP2_IRQHandler(void);

/*@} end of group SSP_Public_FunctionDeclaration */

/*@} end of group SSP  */

/*@} end of group MW300_Periph_Driver */
#endif /* __MW300_SSP_H__ */
