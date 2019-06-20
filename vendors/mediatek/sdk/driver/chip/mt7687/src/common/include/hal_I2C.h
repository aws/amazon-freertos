/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/*
** $Log: hal_I2C.h $
**
** 11 10 2014 leo.hung
** [DVT][I2C]
** 1. Use CPU_FREQUENCY instead of hard code.
**
** 10 29 2014 leo.hung
** [DVT][I2C]
** 1. Remove parameter ucGModeEn in halI2CMasterCtrl().
**    Config General/Normal mode per transfer instead of init time.
** 2. Fix halI2CChkStatus() for slave address exist check.
** 3. Add I2C GDMA mode for Normal and General R/W.
**
** 10 28 2014 leo.hung
** [DVT][I2C]
** 1. Add transfer status check.
** 2. Add I2C interrupt check.
**
** 10 27 2014 leo.hung
** [DVT]
** 1. Update I2C.
**
** 10 23 2014 leo.hung
** [DVT]
** 1. Update I2C.
**
**
*/

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

#ifndef _HAL_I2C_H
#define _HAL_I2C_H

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "mt7687_cm4_hw_memmap.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/


/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/


/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/


/*
========================================================================
Routine Description:

Note:
========================================================================
*/

ENUM_HAL_RET_T halI2CClearFIFO(UINT8 ucI2CIdx, UINT8 ucRxClr, UINT8 ucTxClr);


/*
 * ucSCLDRVHEn:
 * ucSDADRVHEn:     MM_PAD_CON0[6:5] can decide driving or pulling high of SDA and SCL.
 *                  Setting to driving high can reduce some latency, but it is different with spec.
 * ucIntClkSyncEn:  Set MM_PAD_CON[7] to 1 to enable the internal clock synchronization of SDA and SCL inputs.
 *                  The glitch is not so many when enabling, but the latency with 2 internal clock cycle will be induced.
 * ucDeglitchCnt:   Enable the deglitch circuit by setting MM_PAD_CON[4:0] to non-zero value.
 */
ENUM_HAL_RET_T halI2CInit(UINT8 ucI2CIdx, UINT8 ucSCLDRVHEn, UINT8 ucSDADRVHEn, UINT8 ucIntClkSyncEn, UINT8 ucDeglitchCnt);


/*
 * MM_CON0 should be set correctly according to the I2C protocol.
 * MM_CON0[15] is used to enable the master function.
 * MM_CON0[14] should be 0 in normal usage.
 * Then setting MM_CNT_VAL_PHL and MM_CNT_VAL_PHH decide the bit rate of I2C in standard mode or fast mode.
 */
ENUM_HAL_RET_T halI2CMasterCtrl(UINT8 ucI2CIdx, ENUM_I2C_CLK_T eFrequency);

/*
 * a. Read MM_STATUS[2]. The I2C master circuit is ready for preparing the next trigger if the value is 1.
 * b. Write the TX data into the memory or MM_FIFO_DATA according the value of DMA_CON0.
 * c. Set a value of MM_ID_CON0 to decide which slave you want to write.
 * d. Set MM_PACK_CON0[0] to 0 for write.
 * e. Set MM_CON0[0] to 1 to trigger the master to write data to slave.
 * f. After setting MM_CON0[0] to 1, the value of MM_STATUS[2] will become 0.
 *    Waiting an interrupt or the value of MM_STATUS[2] to 1 to check the TX transfer is over.
 */
ENUM_HAL_RET_T halI2CNormalWrite(UINT8 ucI2CIdx, UINT8 ucSlaveAddress, const UINT8 *pucTxData, UINT16 u2Length);


/*
 * a. Read MM_STATUS[2] as 1.
 * b. Set MM_ID_CON0 and MM_ID_CON1 for slave address.
 * c. Set MM_PACK_CON0[0] to 1 for read.
 * d. Set the MM_CNT_BYTE_VAL_PK0 to decide how many data to be read.
 * e. Set MM_CON0[0] to 1 and wait RX transfer over by interrupt or polling the value of MM_STATUS[2].
 */
ENUM_HAL_RET_T halI2CNormalRead(UINT8 ucI2CIdx, UINT8 ucSlaveAddress, PUINT8 pucRxData, UINT16 u2Length);


ENUM_HAL_RET_T halI2CGeneralRW(UINT8 ucI2CIdx,
                               UINT8 ucSlaveAddress,
                               PUINT8 pucTxData,
                               PUINT8 pucRxData,
                               UINT8 ucPktNum,
                               UINT8 ucPkt0RW,
                               UINT16 u2Pkt0Len,
                               UINT8 ucPkt1RW,
                               UINT16 u2Pkt1Len,
                               UINT8 ucPkt2RW,
                               UINT16 u2Pkt2Len);


/*
 * [Arbitration]
 * MM_STATUS[1] will become 1 to indicate that there is arbitration lose in the transfer before.
 * The data of TX may not be write out correctly or the data of RX may not be read correctly when arbitration lose.
 * Write MM_STATUS[1] as 1 to clear the value of this bit.
 *
 * [ACK bit map]
 * The MM_PACK_CON0[0] in standard/fast mode or MM_PACK_CON0[1] in high speed mode should be 0 to indicate the slave exist on the bus outside.
 * If the value is 1, the transfer will stop after the slave address has transmitted, because there is no slave to give an acknowledge bit.
 *
 * [FIFO status]
 * MM_FIFO_STATUS should also be checked after transfer when using FIFO mode.
 * There are overflow, underflow, full, and empty flags of each TX or RX FIFO.
 * Write MM_FIFO_CON0[1] or MM_FIFO_CON0[0] as 1 to clear the related FIFO status of TX FIFO or RX FIFO.
 *
 * ucPktNum:    Normal mode: alwasy 1
 *              General mode: according to pkt number 1 ~ 3
 * ucAckLen:    RECEIVED ack number including "slave address" and "Tx data".
 *              NOT including "Ack for Rx data" which is SENT by master.
 */
ENUM_I2C_TRANS_STATUS_T halI2CChkStatus(UINT8 ucI2CIdx, UINT8 ucPktNum, UINT8 ucAckLen);
ENUM_I2C_TRANS_STATUS_T halI2CGetBusyStatus(UINT8 ucI2CIdx);
P_IOT_I2C_TypeDef halI2CGetBaseAddr(UINT8 ucI2CIdx);



UINT16 halI2CGetFIFOStatus(UINT8 ucI2CIdx);


/*
 * I2C module build-in DMA is NOT supported in MT7687.
 * Leverage GDMA to do I2C+DMA access.
 */
ENUM_HAL_RET_T halI2CGDMACtrl(UINT8 ucI2CIdx, UINT8 ucGDMAIdx, UINT8 ucTxMode, UINT16 u2Len, const UINT8 *pucMemAddr);


ENUM_HAL_RET_T halI2CGDMANormalWrite(UINT8 ucI2CIdx, UINT8 ucGDMAIdx, UINT8 ucSlaveAddress, const UINT8 *pucTxData, UINT16 u2Length);
ENUM_HAL_RET_T halI2CGDMANormalRead(UINT8 ucI2CIdx, UINT8 ucGDMAIdx, UINT8 ucSlaveAddress, PUINT8 pucRxData, UINT16 u2Length);
ENUM_HAL_RET_T halI2CGDMAGeneralRW(UINT8 ucI2CIdx,
                                   UINT8 ucTxGDMAIdx,
                                   UINT8 ucRxGDMAIdx,
                                   UINT8 ucSlaveAddress,
                                   PUINT8 pucTxData,
                                   PUINT8 pucRxData,
                                   UINT8 ucPktNum,
                                   UINT8 ucPkt0RW,
                                   UINT16 u2Pkt0Len,
                                   UINT8 ucPkt1RW,
                                   UINT16 u2Pkt1Len,
                                   UINT8 ucPkt2RW,
                                   UINT16 u2Pkt2Len);
#endif //_HAL_I2C_H

