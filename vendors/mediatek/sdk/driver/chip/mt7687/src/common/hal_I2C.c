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
** $Log: hal_I2C.c $
**
** 04 13 2015 leo.hung
** [DVT][I2C]
** 1. Fix build error.
**
** 04 13 2015 leo.hung
** [DVT][I2C]
** 1. Update I2C DVT cases.
**
** 03 24 2015 leo.hung
** [DVT]
** 1. Fix build error of r=1.
**
** 12 16 2014 leo.hung
** [DVT][I2C]
** 1. Fix halI2CClearFIFO.
**
** 11 24 2014 leo.hung
** [DVT][I2C][PWM]
** 1. Add Auto regression.
** 2. Add volatile to fix -Os error.
**
** 11 24 2014 leo.hung
** [DVT][IrTx]
** 1. Add IrTx DVT.
**
** 11 11 2014 leo.hung
** [DVT][I2C][PWM]
** 1. Update I2C DMA mode.
** 2. Replace PWM field access with bit offset access.
**
** 11 10 2014 leo.hung
** [DVT][I2C]
** 1. Use CPU_FREQUENCY instead of hard code.
**
** 11 03 2014 leo.hung
** [DVT][I2C]
** 1. Seperate I2C clock rate setting for FPGA and Chip.
** 2. Fix I2C DMA mode setting.
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


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "hal_i2c_master.h"
#if defined(HAL_I2C_MASTER_MODULE_ENABLED)
#include <stdio.h>
#include <string.h>
#include "type_def.h"
#include "top.h"
#include "hal_I2C.h"
#include "dma_sw.h"
#include "hal_log.h"
#include "hal_gpt.h"
#include "_mtk_hal_debug.h"
#include "hal_nvic_internal.h"
#include "hal_pdma_internal.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
#define I2C_RW_POLLING    1

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
#define I2C_ENABLE_POLLING_TIMEOUT

volatile uint8_t i2c_timeout[2] = {0, 0};

#ifdef I2C_ENABLE_POLLING_TIMEOUT
#define I2C_POLLING_TIMEOUT_VALUE (131072)  /* Unit:32k. (131072 * 1 / 32k) = 4s */

static uint8_t i2c_master_wait_transaction_finish(IOT_I2C_TypeDef *pI2CTypeDef, uint8_t ucIdx, uint8_t ucI2CIdx, uint8_t is_tx)
{
    uint32_t gpt_start_count, gpt_current_count, gpt_duration_count;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &gpt_start_count);
    /* polling till transaction finish */
    while (1) {
        uint32_t saved_mask;
        saved_mask = save_and_set_interrupt_mask();
        if (1 == is_tx) {
            if (!((!((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET))) && ucIdx < 0xF0)) {
                restore_interrupt_mask(saved_mask);
                break;
            }
        } else {
            if (!((!((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET))))) {
                restore_interrupt_mask(saved_mask);
                break;
            }
        }
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &gpt_current_count);
        hal_gpt_get_duration_count(gpt_start_count, gpt_current_count, &gpt_duration_count);
        if (I2C_POLLING_TIMEOUT_VALUE < gpt_duration_count) {
            log_hal_error("[I2C%d][is_tx = %d]:polling tiemout!\r\n", ucI2CIdx, is_tx);
            //ASSERT(0);
            i2c_timeout[ucI2CIdx] = 1;
            //reset i2c hardware
            restore_interrupt_mask(saved_mask);
            return 1;
        }
        restore_interrupt_mask(saved_mask);
    }
    return 0;
}
#endif
P_IOT_I2C_TypeDef halI2CGetBaseAddr(UINT8 ucI2CIdx)
{
    IOT_I2C_TypeDef *pI2CTypeDef = NULL;

    if (0 == ucI2CIdx) {
        pI2CTypeDef = (IOT_I2C_TypeDef *)IOT_I2C0_MM_PAD_CON0;
    } else if (1 == ucI2CIdx) {
        pI2CTypeDef = (IOT_I2C_TypeDef *)IOT_I2C1_MM_PAD_CON0;
    }

    return pI2CTypeDef;
}


ENUM_HAL_RET_T halI2CClearFIFO(UINT8 ucI2CIdx, UINT8 ucRxClr, UINT8 ucTxClr)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    IOT_I2C_TypeDef *pI2CTypeDef = NULL;
    UINT32 u4Val = 0;

    pI2CTypeDef = halI2CGetBaseAddr(ucI2CIdx);
    //if (NULL == pI2CTypeDef)
    //{
    //    return HAL_RET_FAIL;
    //}

    u4Val = (pI2CTypeDef->MM_FIFO_CON0);

    if (ucRxClr) {
        u4Val |= (0x1 << I2C_MM_RX_FIFO_CLR_OFFSET);
    }

    if (ucTxClr) {
        u4Val |= (0x1 << I2C_MM_TX_FIFO_CLR_OFFSET);
    }

    (pI2CTypeDef->MM_FIFO_CON0) |= u4Val;

    return ret;
}

/*
 * ucSCLDRVHEn:
 * ucSDADRVHEn:     MM_PAD_CON0[6:5] can decide driving or pulling high of SDA and SCL.
 *                  Setting to driving high can reduce some latency, but it is different with spec.
 *                  Should NOT enable for normal case.
 * ucIntClkSyncEn:  Set MM_PAD_CON[7] to 1 to enable the internal clock synchronization of SDA and SCL inputs.
 *                  The glitch is not so many when enabling, but the latency with 2 internal clock cycle will be induced.
 * ucDeglitchCnt:   Enable the deglitch circuit by setting MM_PAD_CON[4:0] to non-zero value.
 */
ENUM_HAL_RET_T halI2CInit(UINT8 ucI2CIdx, UINT8 ucSCLDRVHEn, UINT8 ucSDADRVHEn, UINT8 ucIntClkSyncEn, UINT8 ucDeglitchCnt)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    IOT_I2C_TypeDef *pI2CTypeDef = NULL;

    // Sanity check
    if (ucDeglitchCnt & (~I2C_DE_CNT_MASK)) {
        return HAL_RET_FAIL;
    }

    pI2CTypeDef = halI2CGetBaseAddr(ucI2CIdx);
    //if (NULL == pI2CTypeDef)
    //{
    //    return HAL_RET_FAIL;
    //}

    if (ucSCLDRVHEn) {
        (pI2CTypeDef->MM_PAD_CON0) |= BIT(I2C_SCL_DRVH_EN_OFFSET);
    } else {
        (pI2CTypeDef->MM_PAD_CON0) &= (~BIT(I2C_SCL_DRVH_EN_OFFSET));
    }

    if (ucSDADRVHEn) {
        (pI2CTypeDef->MM_PAD_CON0) |= BIT(I2C_SDA_DRVH_EN_OFFSET);
    } else {
        (pI2CTypeDef->MM_PAD_CON0) &= (~BIT(I2C_SDA_DRVH_EN_OFFSET));
    }

    if (ucIntClkSyncEn) {
        (pI2CTypeDef->MM_PAD_CON0) |= BIT(I2C_CLK_SYNC_EN_OFFSET);
    } else {
        (pI2CTypeDef->MM_PAD_CON0) &= (~BIT(I2C_CLK_SYNC_EN_OFFSET));
    }

    (pI2CTypeDef->MM_PAD_CON0) &= (~I2C_DE_CNT_MASK);
    (pI2CTypeDef->MM_PAD_CON0) |= (ucDeglitchCnt << I2C_DE_CNT_OFFSET);

    halI2CClearFIFO(ucI2CIdx, 1, 1);


    return ret;
}

/*
 * MM_CON0 should be set correctly according to the I2C protocol.
 * MM_CON0[15] is used to enable the master function.
 * MM_CON0[14] should be 0 in normal usage.
 * Then setting MM_CNT_VAL_PHL and MM_CNT_VAL_PHH decide the bit rate of I2C in standard mode or fast mode.
 *
 * Note: General Mode / Normal Mode is configured per transfer.
 */
ENUM_HAL_RET_T halI2CMasterCtrl(UINT8 ucI2CIdx, ENUM_I2C_CLK_T eFrequency)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    IOT_I2C_TypeDef *pI2CTypeDef = NULL;
    UINT32 u4Val = 0;
    UINT8 ucPhase1Adj = 0;
    UINT32 u4OperFreq = 0;
    UINT16 u2PhaseVal = 0;

    // sanity check
    if (eFrequency >= I2C_CLK_NUM) {
        return HAL_RET_FAIL;
    }

    pI2CTypeDef = halI2CGetBaseAddr(ucI2CIdx);
    //if (NULL == pI2CTypeDef)
    //{
    //    return HAL_RET_FAIL;
    //}

    // Enable I2C transfer W   I2C_BASE + 0x270    [15]    MASTER_EN   1'b1    Set 1 to enable master mode
    //                                             [14]    MM_GMODE    1'b0    Set 0 to disable general mode (normal mode)
    //                                             [13]    MM_HS_EN    1'b0    Set 0 to disable HS mode
    //                                             [12]    MM_TB_EN    1'b0    Set 0 to disable Ten-Bit mode
    //                                             [11]    MCU_SEL 1'b0    Set 0 to select DSP I/F
    u4Val = (pI2CTypeDef->MM_CON0);
    u4Val |= BIT(I2C_MASTER_EN_OFFSET);
    u4Val &= (~BIT(I2C_MM_GMODE_OFFSET));
    u4Val &= (~BIT(I2C_MM_HS_EN_OFFSET));
    u4Val &= (~BIT(I2C_MM_TB_EN_OFFSET));
    u4Val &= (~BIT(I2C_MCU_SEL_OFFSET));
    (pI2CTypeDef->MM_CON0) = u4Val;

    // Clock rate(Frequency) selection
    // Sum of phase0,1,2,3 = 'Bus clock' / 'I2C clock rate'
    // Count from 0x0
    // Phase1 adjust value
    //1. Output register
    ucPhase1Adj = 1;
    u4Val = (pI2CTypeDef->MM_PAD_CON0);
    if (u4Val & BIT(I2C_CLK_SYNC_EN_OFFSET)) {
        // 2. Sync enable
        ucPhase1Adj += 1;
    }
    // 3. de-glitch (n+4)
    ucPhase1Adj += (((u4Val & I2C_DE_CNT_MASK) >> I2C_DE_CNT_OFFSET) + 4);

    // FPGA use 30MHz as I2C operation frequence
    // Confirmed with DE, 30M is 30*1000*1000 instead of 30*1024*1024

    u4OperFreq = top_xtal_freq_get(); // CPU_FREQUENCY;
    switch (eFrequency) {
        case I2C_CLK_50K:
            // 50KHz
            // 800 = (40000 / 50)
            u2PhaseVal = ((u4OperFreq / (50 * 1000)) / 4) - 1; // Count from 0

            // Set the value of phase1/0
            // pI2CTypeDef->MM_CNT_VAL_PHL = 0x0000BFC7; // sync disable, de-glitch=3
            // Set the value of phase3/2
            // pI2CTypeDef->MM_CNT_VAL_PHH = 0x0000C7C7;
            break;
        case I2C_CLK_100K:
            // 100KHz
            // 400 = (40000 / 100)
            u2PhaseVal = ((u4OperFreq / (100 * 1000)) / 4) - 1; // Count from 0

            // Set the value of phase1/0
            // pI2CTypeDef->MM_CNT_VAL_PHL = 0x00005B63; // sync disable, de-glitch=3
            // Set the value of phase3/2
            // pI2CTypeDef->MM_CNT_VAL_PHH = 0x00006363;
            break;
        case I2C_CLK_200K:
            // 200KHz
            // 200 = (40000 / 200)
            u2PhaseVal = ((u4OperFreq / (200 * 1000)) / 4) - 1; // Count from 0

            // Set the value of phase1/0
            // pI2CTypeDef->MM_CNT_VAL_PHL = 0x00002931; // sync disable, de-glitch=3
            // Set the value of phase3/2
            // pI2CTypeDef->MM_CNT_VAL_PHH = 0x00003131;

            break;
        case I2C_CLK_300K:
            // 300KHz
            // 300 = (40000 / 300)
            u2PhaseVal = ((u4OperFreq / (300 * 1000)) / 4) - 1; // Count from 0

            // Set the value of phase1/0
            // pI2CTypeDef->MM_CNT_VAL_PHL = 0x00002931; // sync disable, de-glitch=3
            // Set the value of phase3/2
            // pI2CTypeDef->MM_CNT_VAL_PHH = 0x00003131;

            break;
        case I2C_CLK_400K:
            // 400KHz
            // 100 = (40000 / 400)
            u2PhaseVal = ((u4OperFreq / (400 * 1000)) / 4) - 1; // Count from 0

            // Set the value of phase1/0
            // pI2CTypeDef->MM_CNT_VAL_PHL = 0x00001018; // sync disable, de-glitch=3
            // Set the value of phase3/2
            // pI2CTypeDef->MM_CNT_VAL_PHH = 0x00001818;
            break;
        default:
            return HAL_RET_FAIL;
            //break;
    }
    // Set the value of phase1/0
    pI2CTypeDef->MM_CNT_VAL_PHL = (((u2PhaseVal - ucPhase1Adj) << I2C_MM_CNTPHASE_VAL1_OFFSET) | (u2PhaseVal << I2C_MM_CNTPHASE_VAL0_OFFSET));
    // Set the value of phase3/2
    pI2CTypeDef->MM_CNT_VAL_PHH = ((u2PhaseVal << I2C_MM_CNTPHASE_VAL3_OFFSET) | (u2PhaseVal << I2C_MM_CNTPHASE_VAL2_OFFSET));

    return ret;
}

/*
 *
 */
UINT8 halI2CGetFIFOSpace(UINT8 WPtr, UINT8 RPtr)
{

    if (WPtr == RPtr) {
        return I2C_FIFO_MAX_LEN;
    } else if ((WPtr & BITS(0, 2)) == (RPtr & BITS(0, 2))) {
        return 0;
    }

    WPtr &= BITS(0, 2);
    RPtr &= BITS(0, 2);

    if (WPtr > RPtr) {
        return (I2C_FIFO_MAX_LEN - (WPtr - RPtr));
    } else {
        return (RPtr - WPtr);
    }
}

/*
 * return remaining space length of Tx FIFO
 */
UINT8 halI2CTxFIFOSpace(IOT_I2C_TypeDef *pI2CTypeDef)
{
#if 0
    UINT8   ucLen = 0;
#endif
    UINT8   ucTxWPtr = 0;
    UINT8   ucTxRPtr = 0;
    UINT32  u4Val = 0;

    u4Val = (pI2CTypeDef->MM_FIFO_PTR);
    ucTxWPtr = (u4Val & I2C_MM_TX_FIFO_WPTR_MASK) >> I2C_MM_TX_FIFO_WPTR_OFFSET;
    ucTxRPtr = (u4Val & I2C_MM_TX_FIFO_RPTR_MASK) >> I2C_MM_TX_FIFO_RPTR_OFFSET;

    //printf("[%s]ucTxWPtr=%u, ucTxRPtr=%u\n", __FUNCTION__, ucTxWPtr, ucTxRPtr);
    return halI2CGetFIFOSpace(ucTxWPtr, ucTxRPtr);
#if 0
    if (ucTxWPtr == ucTxRPtr) {
        return I2C_FIFO_MAX_LEN;
    } else if ((ucTxWPtr & BITS(0, 2)) == (ucTxRPtr & BITS(0, 2))) {
        return 0;
    }

    ucTxWPtr &= BITS(0, 2);
    ucTxRPtr &= BITS(0, 2);

    if (ucTxWPtr > ucTxRPtr) {
        ucLen = I2C_FIFO_MAX_LEN - (ucTxWPtr - ucTxRPtr);
    } else {
        ucLen = ucTxRPtr - ucTxWPtr;
    }
    //printf("---len=%u\n", ucLen);

    return ucLen;
#endif
}

/*
 * return remaining space length of Rx FIFO
 */
UINT8 halI2CRxFIFOSpace(IOT_I2C_TypeDef *pI2CTypeDef)
{
#if 0
    UINT8   ucLen = 0;
#endif
    UINT8   ucRxWPtr = 0;
    UINT8   ucRxRPtr = 0;
    UINT32  u4Val = 0;

    u4Val = (pI2CTypeDef->MM_FIFO_PTR);
    ucRxWPtr = (u4Val & I2C_MM_RX_FIFO_WPTR_MASK) >> I2C_MM_RX_FIFO_WPTR_OFFSET;
    ucRxRPtr = (u4Val & I2C_MM_RX_FIFO_RPTR_MASK) >> I2C_MM_RX_FIFO_RPTR_OFFSET;
    return halI2CGetFIFOSpace(ucRxWPtr, ucRxRPtr);
#if 0
    if (ucRxWPtr == ucRxRPtr) {
        return I2C_FIFO_MAX_LEN;
    } else if ((ucRxWPtr & BITS(0, 2)) == (ucRxRPtr & BITS(0, 2))) {
        return 0;
    }

    ucRxWPtr &= BITS(0, 2);
    ucRxRPtr &= BITS(0, 2);

    if (ucRxWPtr > ucRxRPtr) {
        ucLen = I2C_FIFO_MAX_LEN - (ucRxWPtr - ucRxRPtr);
    } else {
        ucLen = ucRxRPtr - ucRxWPtr;
    }

    return ucLen;
#endif
}

UINT16 halI2CGetFIFOStatus(UINT8 ucI2CIdx)
{
    IOT_I2C_TypeDef *pI2CTypeDef = NULL;

    pI2CTypeDef = halI2CGetBaseAddr(ucI2CIdx);
    //if (NULL == pI2CTypeDef)
    //{
    //    return HAL_RET_FAIL;
    //}

    return (pI2CTypeDef->MM_FIFO_STATUS);
}

/*
 * a. Read MM_STATUS[2]. The I2C master circuit is ready for preparing the next trigger if the value is 1.
 * b. Write the TX data into the memory or MM_FIFO_DATA according the value of DMA_CON0.
 * c. Set a value of MM_ID_CON0 to decide which slave you want to write.
 * d. Set MM_PACK_CON0[0] to 0 for write.
 * e. Set MM_CON0[0] to 1 to trigger the master to write data to slave.
 * f. After setting MM_CON0[0] to 1, the value of MM_STATUS[2] will become 0.
 *    Waiting an interrupt or the value of MM_STATUS[2] to 1 to check the TX transfer is over.
 */
/*
 * ucSlaveAddress[6:0] indicate 7-bit address, bit7 is un-used
 */
ENUM_HAL_RET_T halI2CNormalWrite(UINT8 ucI2CIdx, UINT8 ucSlaveAddress, const UINT8 *pucTxData, UINT16 u2Length)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    UINT8 ucIdx = 0;
    UINT8 ucTxLen = 0;
    UINT16 u2Residual = u2Length;
    IOT_I2C_TypeDef *pI2CTypeDef = NULL;

    pI2CTypeDef = halI2CGetBaseAddr(ucI2CIdx);
    //if (NULL == pI2CTypeDef)
    //{
    //    return HAL_RET_FAIL;
    //}

    // Sanity check
    if (!pucTxData) {
        return HAL_RET_FAIL;
    }

    // General mode Disable
    (pI2CTypeDef->MM_CON0) &= (~BIT(I2C_MM_GMODE_OFFSET));


    // a. Read MM_STATUS[2]. The I2C master circuit is ready for preparing the next trigger if the value is 1.
    if (!((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET))) {
        printf("[%s](!((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET)))\n", __FUNCTION__);
        log_hal_error("[I2C][Normal_Write]  not ready.\r\n");
        return HAL_RET_FAIL;
    }

    // b. Write the TX data into the memory or MM_FIFO_DATA according the value of DMA_CON0.
    ucTxLen = halI2CTxFIFOSpace(pI2CTypeDef);
    if (ucTxLen > u2Length) {
        ucTxLen = u2Length;
    }
    for (ucIdx = 0; ucIdx < ucTxLen; ucIdx++) {
        (pI2CTypeDef->MM_FIFO_DATA) = (*pucTxData);
        pucTxData++;
    }
    u2Residual = u2Residual - ucTxLen;

    // c. Set a value of MM_ID_CON0 to decide which slave you want to write.
    (pI2CTypeDef->MM_ID_CON0) &= (~I2C_MM_SLAVE_ID_MASK);
    (pI2CTypeDef->MM_ID_CON0) |= ((ucSlaveAddress & (I2C_MM_SLAVE_ID_MASK)) << I2C_MM_SLAVE_ID_OFFSET);

    // d. Set MM_PACK_CON0[0] to 0 for write.
    (pI2CTypeDef->MM_PACK_CON0) &= (~BIT(I2C_MM_PACK_RW_OFFSET));

    // e. Set MM_CON0[0] to 1 to trigger the master to write data to slave.
    (pI2CTypeDef->MM_CON0) |= BIT(I2C_MM_START_EN_OFFSET);

//#if (1 != I2C_FIFO_DVT)
    while (u2Residual) {
        if (((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET))) {
            // Tx transfer already terminated while Tx data not complete send out.
            log_hal_error("[I2C][Normal_Write] return before complete;length=%d.\r\n", u2Length);
            return HAL_RET_FAIL;
        }
        ucTxLen = halI2CTxFIFOSpace(pI2CTypeDef);
        if (0 == ucTxLen) {
            continue;
        }

        if (ucTxLen > u2Residual) {
            ucTxLen = u2Residual;
        }

        for (ucIdx = 0; ucIdx < ucTxLen; ucIdx++) {
#if 0
            printf("WPTR=%u, RPTR=%u, ucTxLen=%u\n", (((pI2CTypeDef->MM_FIFO_PTR) & I2C_MM_TX_FIFO_WPTR_MASK) >> I2C_MM_TX_FIFO_WPTR_OFFSET),
                   (((pI2CTypeDef->MM_FIFO_PTR) & I2C_MM_TX_FIFO_RPTR_MASK) >> I2C_MM_TX_FIFO_RPTR_OFFSET),
                   ucTxLen);
#endif
            (pI2CTypeDef->MM_FIFO_DATA) = (*pucTxData);
            pucTxData++;
        }
        u2Residual = u2Residual - ucTxLen;
    }
//#endif //#if (1 != I2C_FIFO_DVT)

#if I2C_RW_POLLING
    // f. After setting MM_CON0[0] to 1, the value of MM_STATUS[2] will become 0.
    //    Waiting an interrupt or the value of MM_STATUS[2] to 1 to check the TX transfer is over.
#ifndef I2C_ENABLE_POLLING_TIMEOUT
    do {
        // busy waiting
    } while ((!((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET))) && ucIdx < 0xF0);
#else
    //i2c_master_wait_transaction_finish(pI2CTypeDef, ucIdx, ucI2CIdx, 1);
    i2c_timeout[ucI2CIdx] = 0;
    if(0 != i2c_master_wait_transaction_finish(pI2CTypeDef, ucIdx, ucI2CIdx, 1)){
        return HAL_RET_FAIL;
    }

#endif
#endif

    if (I2C_TRANS_STATUS_OK != halI2CChkStatus(ucI2CIdx, 1, (1 + u2Length))) {
        log_hal_error("[I2C][Normal_Write] error_status = %d.\r\n", halI2CChkStatus(ucI2CIdx, 1, (1 + u2Length)));
        return HAL_RET_FAIL;
    }

    return ret;
}


/*
 * a. Read MM_STATUS[2] as 1.
 * b. Set MM_ID_CON0 and MM_ID_CON1 for slave address.
 * c. Set MM_PACK_CON0[0] to 1 for read.
 * d. Set the MM_CNT_BYTE_VAL_PK0 to decide how many data to be read.
 * e. Set MM_CON0[0] to 1
 * f. wait RX transfer over by interrupt or polling the value of MM_STATUS[2].
 */
ENUM_HAL_RET_T halI2CNormalRead(UINT8 ucI2CIdx, UINT8 ucSlaveAddress, PUINT8 pucRxData, UINT16 u2Length)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    UINT16  u2RetriveLen = 0;
    UINT16  u2Residual = u2Length;
    UINT8   ucRxBufIdx = 0;
    UINT16  u2Idx = 0;
    IOT_I2C_TypeDef *pI2CTypeDef = NULL;

    pI2CTypeDef = halI2CGetBaseAddr(ucI2CIdx);
    //if (NULL == pI2CTypeDef)
    //{
    //    printf("[%s](NULL == pI2CTypeDef)\n", __FUNCTION__);
    //    return HAL_RET_FAIL;
    //}

    if (NULL == pucRxData) {
        return HAL_RET_FAIL;
    }

    // General mode Disable
    (pI2CTypeDef->MM_CON0) &= (~BIT(I2C_MM_GMODE_OFFSET));

    // a. Read MM_STATUS[2] as 1.
    if (!((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET))) {
        printf("[%s](!((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET)))\n", __FUNCTION__);
        log_hal_error("[I2C][Normal_Read]  not ready.\r\n");
        return HAL_RET_FAIL;
    }

    // b. Set MM_ID_CON0 and MM_ID_CON1 for slave address.
    (pI2CTypeDef->MM_ID_CON0) &= (~I2C_MM_SLAVE_ID_MASK);
    (pI2CTypeDef->MM_ID_CON0) |= ((ucSlaveAddress & (I2C_MM_SLAVE_ID_MASK)) << I2C_MM_SLAVE_ID_OFFSET);

    // c. Set MM_PACK_CON0[0] to 1 for read.
    (pI2CTypeDef->MM_PACK_CON0) |= BIT(I2C_MM_PACK_RW_OFFSET);

    // d. Set the MM_CNT_BYTE_VAL_PK0 to decide how many data to be read.
    (pI2CTypeDef->MM_CNT_BYTE_VAL_PK0) = u2Length;

    // e. Set MM_CON0[0] to 1
    (pI2CTypeDef->MM_CON0) |= BIT(I2C_MM_START_EN_OFFSET);

    if (pucRxData) {
        while (u2Residual) {
            if ((((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET))) && ((pI2CTypeDef->MM_ACK_VAL) & 0x1)) {
                log_hal_error("[I2C][Normal_Read] return before complete;length=%d\r\n", u2Length);
                return HAL_RET_FAIL;
                // Rx transfer already terminated while not get enough data.
                // for example, an ack error happened. so we need also check
                // if ack happened.
            }

            u2RetriveLen = (I2C_FIFO_MAX_LEN - halI2CRxFIFOSpace(pI2CTypeDef));
            if (0 == u2RetriveLen) {
                continue;
            }
            if (u2RetriveLen > u2Residual) {
                u2RetriveLen = u2Residual;
            }
            for (u2Idx = 0; u2Idx < u2RetriveLen; u2Idx++) {
#if 0
                printf("WPTR=%u, RPTR=%u, u2RetriveLen=%u\n", (((pI2CTypeDef->MM_FIFO_PTR) & I2C_MM_RX_FIFO_WPTR_MASK) >> I2C_MM_RX_FIFO_WPTR_OFFSET),
                       (((pI2CTypeDef->MM_FIFO_PTR) & I2C_MM_RX_FIFO_RPTR_MASK) >> I2C_MM_RX_FIFO_RPTR_OFFSET),
                       u2RetriveLen);
#endif

                pucRxData[ucRxBufIdx] = (pI2CTypeDef->MM_FIFO_DATA);
                ucRxBufIdx++;
            }
            u2Residual = u2Residual - u2RetriveLen;
        }
    }

#if I2C_RW_POLLING
    // f. wait RX transfer over by interrupt or polling the value of MM_STATUS[2].
#ifndef I2C_ENABLE_POLLING_TIMEOUT
    do {
        // busy waiting
    } while ((!((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET))));
#else
    //i2c_master_wait_transaction_finish(pI2CTypeDef, 0, ucI2CIdx, 0);
    i2c_timeout[ucI2CIdx] = 0;
    if(0 != i2c_master_wait_transaction_finish(pI2CTypeDef, 0, ucI2CIdx, 0)){
       return HAL_RET_FAIL;
    }

#endif
#endif // #if I2C_RW_POLLING

#if 0
    printf("[halI2CNormalRead]\n------------\n");
    if (pucRxData) {
        for (u2Idx = 0; u2Idx < u2Length; u2Idx++) {
            printf("0x%02X ", pucRxData[u2Idx]);
            if (0 == ((u2Idx + 1) & 0x7)) {
                printf("\n");
            }
        }
    }
    printf("\n------------\n");
#endif

    if (I2C_TRANS_STATUS_OK != halI2CChkStatus(ucI2CIdx, 1, 1)) {
        log_hal_error("[I2C][Normal_Read] error_status = %d.\r\n", halI2CChkStatus(ucI2CIdx, 1, 1));
        return HAL_RET_FAIL;
    }

    return ret;
}

#define I2C_GM_RX_LOG 0

/*
   ucPkt0/1/2RW:    0: write
                    1: read
 */
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
                               UINT16 u2Pkt2Len)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    UINT16  u2Idx = 0;
    UINT16  u2TxLen = 0;
    UINT16  u2TxResidual = 0;
    UINT16  u2RxLen = 0;
    UINT16  u2RxResidual = 0;
    UINT16  u2RxBufIdx = 0;
    UINT16  u2AckLen = 0;
    IOT_I2C_TypeDef *pI2CTypeDef = NULL;
#if (1 == I2C_GM_RX_LOG)
    UINT16  u2RxLogLen = 0;
#endif

    pI2CTypeDef = halI2CGetBaseAddr(ucI2CIdx);
    //if (NULL == pI2CTypeDef)
    //{
    //    return HAL_RET_FAIL;
    //}

    // Sanity check
    if (ucPktNum > I2C_PKT_MAX_NUM) {
        return HAL_RET_FAIL;
    }

    if ((!pucTxData) || (!pucRxData)) {
        return HAL_RET_FAIL;
    }

    // General mode enable
    (pI2CTypeDef->MM_CON0) |= BIT(I2C_MM_GMODE_OFFSET);

    // a. Read MM_STATUS[2]. The I2C master circuit is ready for preparing the next trigger if the value is 1.
    if (!((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET))) {
        printf("[%s](!((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET)))\n", __FUNCTION__);
        log_hal_error("[I2C][General_RW]  not ready.\r\n");
        return HAL_RET_FAIL;
    }

    // b. Set a value of MM_ID_CON0 to decide which slave you want to write.
    (pI2CTypeDef->MM_ID_CON0) &= (~I2C_MM_SLAVE_ID_MASK);
    (pI2CTypeDef->MM_ID_CON0) |= ((ucSlaveAddress & (I2C_MM_SLAVE_ID_MASK)) << I2C_MM_SLAVE_ID_OFFSET);

    // c. Set MM_PACK_CON0[5:4] packet number.
    (pI2CTypeDef->MM_PACK_CON0) &= ~(I2C_MM_PACK_VAL_MASK);
    (pI2CTypeDef->MM_PACK_CON0) |= ((ucPktNum - 1) << I2C_MM_PACK_VAL_OFFSET);

    // d. Set the MM_CNT_BYTE_VAL_PK0/1/2 to decide data length.
    (pI2CTypeDef->MM_CNT_BYTE_VAL_PK0) = u2Pkt0Len;
    (pI2CTypeDef->MM_CNT_BYTE_VAL_PK1) = u2Pkt1Len;
    (pI2CTypeDef->MM_CNT_BYTE_VAL_PK2) = u2Pkt2Len;

    // e. Set each PKT's R/W mode
    if (ucPktNum) {
        if (ucPkt0RW) {
            // read
            u2RxResidual += u2Pkt0Len;
            (pI2CTypeDef->MM_PACK_CON0) |= BIT(0);
        } else {
            // write
            u2TxResidual += u2Pkt0Len;
            (pI2CTypeDef->MM_PACK_CON0) &= (~BIT(0));
        }
        ucPktNum--;
    }
    if (ucPktNum) {
        if (ucPkt1RW) {
            // read
            u2RxResidual += u2Pkt1Len;
            (pI2CTypeDef->MM_PACK_CON0) |= BIT(1);
        } else {
            // write
            u2TxResidual += u2Pkt1Len;
            (pI2CTypeDef->MM_PACK_CON0) &= (~BIT(1));
        }
        ucPktNum--;
    }
    if (ucPktNum) {
        if (ucPkt2RW) {
            // read
            u2RxResidual += u2Pkt2Len;
            (pI2CTypeDef->MM_PACK_CON0) |= BIT(2);
        } else {
            // write
            u2TxResidual += u2Pkt2Len;
            (pI2CTypeDef->MM_PACK_CON0) &= (~BIT(2));
        }
        ucPktNum--;
    }
#if (1 == I2C_GM_RX_LOG)
    u2RxLogLen = u2RxResidual;
#endif
    u2AckLen = ucPktNum + u2TxResidual;


    // f. Write the TX data into the memory or MM_FIFO_DATA according the value of DMA_CON0.
    u2TxLen = halI2CTxFIFOSpace(pI2CTypeDef);
    if (u2TxLen > u2TxResidual) {
        u2TxLen = u2TxResidual;
    }
    for (u2Idx = 0; u2Idx < u2TxLen; u2Idx++) {
        (pI2CTypeDef->MM_FIFO_DATA) = (*pucTxData);
        pucTxData++;
    }
    u2TxResidual = u2TxResidual - u2TxLen;

    // g. Set MM_CON0[0] to 1 to trigger the master to write data to slave.
    (pI2CTypeDef->MM_CON0) |= BIT(I2C_MM_START_EN_OFFSET);

    while (u2TxResidual || u2RxResidual) {
        //if ((0 != u2TxResidual) && ((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET)))
        if (((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET))) {
            // Tx transfer already terminated while Tx data not complete send out.
            // Rx transfer already terminated while not get enough data.
            return HAL_RET_FAIL;
        }
        u2RxLen = (I2C_FIFO_MAX_LEN - halI2CRxFIFOSpace(pI2CTypeDef));
        u2TxLen = halI2CTxFIFOSpace(pI2CTypeDef);
        if ((0 == u2RxLen) && (0 == u2TxLen)) {
            continue;
        }
        // Tx
        if (u2TxLen > u2TxResidual) {
            u2TxLen = u2TxResidual;
        }

        for (u2Idx = 0; u2Idx < u2TxLen; u2Idx++) {
            (pI2CTypeDef->MM_FIFO_DATA) = (*pucTxData);
            pucTxData++;
        }
        u2TxResidual = u2TxResidual - u2TxLen;

        // Rx
        if (u2RxLen > u2RxResidual) {
            u2RxLen = u2RxResidual;
        }
        for (u2Idx = 0; u2Idx < u2RxLen; u2Idx++) {
            pucRxData[u2RxBufIdx] = (pI2CTypeDef->MM_FIFO_DATA);
            u2RxBufIdx++;
        }
        u2RxResidual = u2RxResidual - u2RxLen;

    }

#if I2C_RW_POLLING // only for debug 
    // h. After setting MM_CON0[0] to 1, the value of MM_STATUS[2] will become 0.
    //    Waiting an interrupt or the value of MM_STATUS[2] to 1 to check the TX transfer is over.
#ifndef I2C_ENABLE_POLLING_TIMEOUT
    do {
        // busy waiting

    } while ((!((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET))));
#else
    //i2c_master_wait_transaction_finish(pI2CTypeDef, 0, ucI2CIdx, 0);
    i2c_timeout[ucI2CIdx] = 0;
    if(0 != i2c_master_wait_transaction_finish(pI2CTypeDef, 0, ucI2CIdx, 0)){
        return HAL_RET_FAIL;
    }

#endif
#endif // only for debug 

#if (1 == I2C_GM_RX_LOG)
    printf("[%s] Rx FiFo\n------------\n", __FUNCTION__);
    if (pucRxData) {
        for (u2Idx = 0; u2Idx < u2RxLogLen; u2Idx++) {
            printf("0x%02X ", pucRxData[u2Idx]);
            if (0 == ((u2Idx + 1) & 0x7)) {
                printf("\n");
            }
        }
    }
    printf("\n------------\n");
#endif

    if (I2C_TRANS_STATUS_OK != halI2CChkStatus(ucI2CIdx, ucPktNum, u2AckLen)) {
        return HAL_RET_FAIL;
    }

    return ret;
}

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
ENUM_I2C_TRANS_STATUS_T halI2CChkStatus(UINT8 ucI2CIdx, UINT8 ucPktNum, UINT8 ucAckLen)
{
    ENUM_I2C_TRANS_STATUS_T ret = I2C_TRANS_STATUS_OK;
    IOT_I2C_TypeDef *pI2CTypeDef = NULL;
    UINT32 u4Val = 0;
    UINT8 ucAckCheckLen = 0;
    UINT8 ucPktAckVal = 0;
    UINT8 i = 0;

    pI2CTypeDef = halI2CGetBaseAddr(ucI2CIdx);
    //if (NULL == pI2CTypeDef)
    //{
    //    return I2C_TRANS_STATUS_FAIL;
    //}

    // [Arbitration]
    if ((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_ARB_HAD_LOSE_OFFSET)) {
        // Arbitration lose
        return I2C_TRANS_STATUS_ARBITRATE;
    }

    // [ACK bit map]
    u4Val = (pI2CTypeDef->MM_ACK_VAL);
    ucPktAckVal = (u4Val >> (I2C_ACK_PKT0_OFFSET));

    // Check slave address exist
    for (i = 0; i < ucPktNum; i++) {
        if (ucPktAckVal & (0x1 << i)) {
            return I2C_TRANS_STATUS_SLAVE_NOT_EXIST;
        }
    }

    // Check ACK for data
    // Only last 8 received ACK are recorded
    ucAckCheckLen = ucAckLen;
    if (ucAckLen > 8) {
        ucAckCheckLen = 8;
    }
    for (i = 0; i < ucAckCheckLen; i++) {
        if (u4Val & (0x1 << i)) {
            return I2C_TRANS_STATUS_NACK;
        }
    }

    // [FIFO status]
    // Success Tx/Rx transfer should complete with Tx/Rx FIFO EMPTY
    if (0x11 != halI2CGetFIFOStatus(ucI2CIdx)) {
        return I2C_TRANS_STATUS_FIFO_FLOW;
    }

    return ret;
}

ENUM_I2C_TRANS_STATUS_T halI2CGetBusyStatus(UINT8 ucI2CIdx)
{
    IOT_I2C_TypeDef *pI2CTypeDef = NULL;
    ENUM_I2C_TRANS_STATUS_T busy_status;

    pI2CTypeDef = halI2CGetBaseAddr(ucI2CIdx);
    //if (NULL == pI2CTypeDef)
    //{
    //    return I2C_TRANS_STATUS_FAIL;
    //}
    busy_status = (ENUM_I2C_TRANS_STATUS_T)((pI2CTypeDef->MM_STATUS) & BIT(I2C_BUS_BUSY_OFFSET));
    //If I2C is idle, clear FIFO for next transaction
    if (I2C_TRANS_STATUS_SLAVE_NOT_EXIST == busy_status) {
        halI2CClearFIFO(ucI2CIdx, 1, 1);
    }
    return busy_status;
}
/*
 * I2C module build-in DMA is NOT supported in MT7687.
 * Leverage GDMA to do I2C+DMA access.
 */
ENUM_HAL_RET_T halI2CGDMACtrl(UINT8 ucI2CIdx, UINT8 ucGDMAIdx, UINT8 ucTxMode, UINT16 u2Len, const UINT8 *pucMemAddr)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    // IOT_I2C_HALF_GDMA_TypeDef *pI2CHalfGDMATypeDef = NULL;
    IOT_I2C_TypeDef *pI2CTypeDef = NULL;
    // UINT32 u4Val = 0;
    pdma_config_t  i2c_dma_config;

    // Sanity Check
    pI2CTypeDef = halI2CGetBaseAddr(ucI2CIdx);
    if (NULL == pI2CTypeDef) {
//        return HAL_RET_FAIL;
    }
    // I2C MUST use half channel DMA
    if ((ucGDMAIdx < 2) || (ucGDMAIdx > 12)) {
        return HAL_RET_FAIL;
    }

#if 0
    /* set GDMA PDN */
    DMA_Clock_Enable(ucGDMAIdx);



    pI2CHalfGDMATypeDef = (IOT_I2C_HALF_GDMA_TypeDef *)(CM4_DMA_BASE + (ucGDMAIdx * 0x100) + I2C_GDMA_DMAN_WPPT_OFFSET);

    /* Set DMA limiter to slowdown DMA. Avoid DMA and I2C handshake error */
    (pI2CHalfGDMATypeDef->DMAn_LIMITER) = 0x80;

    // Set GDMA transfer count W   GDMA_BASE + 0x0N10  [15:0]  LEN USER_DEFINED    Unit is byte
    (pI2CHalfGDMATypeDef->DMAn_COUNT) = (((pI2CHalfGDMATypeDef->DMAn_COUNT) & (~BITS(0, 15))) | u2Len);

    // Set GDMA source address	W	GDMA_BASE + 0x0N2C	[31:0]	PGMADDR	USER_DEFINED	The address in memory
    (pI2CHalfGDMATypeDef->DMAn_PGMADDR) = (UINT32)(pucMemAddr);

    // Set GDMA configurations W   GDMA_BASE + 0x0N14
    u4Val = (pI2CHalfGDMATypeDef->DMAn_CON);
    if (0 == ucI2CIdx) {
        // [25:20] MAS 6'd2 or 6'd4    Set 6'd2/6'd4 for I2C-1/2 TX respectively
        // [25:20] MAS 6'd3 or 6'd5    Set 6'd3/6'd5 for I2C-1/2 RX respectively
        if (1 == ucTxMode) {
            // Tx
            u4Val = (u4Val & (~BITS(20, 25))) | (0x2 << 20);
            u4Val = (u4Val & (~BIT(18)));
        } else {
            // Rx
            u4Val = (u4Val & (~BITS(20, 25))) | (0x3 << 20);
            u4Val = (u4Val | BIT(18));
        }
    } else if (1 == ucI2CIdx) {
        // [25:20] MAS 6'd2 or 6'd4    Set 6'd2/6'd4 for I2C-1/2 TX respectively
        // [25:20] MAS 6'd3 or 6'd5    Set 6'd3/6'd5 for I2C-1/2 RX respectively
        if (1 == ucTxMode) {
            // Tx
            u4Val = (u4Val & (~BITS(20, 25))) | (0x4 << 20);
        } else {
            // Rx
            u4Val = (u4Val & (~BITS(20, 25))) | (0x5 << 20);
        }
    }

    if (1 == ucTxMode) {
        // Tx
        // [18]    DIR 1'b0    Set 0 for Read (RAM to I2C) I2C Tx
        u4Val = (u4Val & (~BIT(18)));

        //[3] DINC    1'b0    Set 0 to disable incremental address
        u4Val = (u4Val & (~BIT(3)));
        //[2] SINC    1'b1    Set 1 to enable incremental address
        u4Val = (u4Val | BIT(2));
    } else {
        // Rx
        // [18]    DIR 1'b1    Set 1 for Write (I2C to RAM) I2C Rx
        u4Val = (u4Val | BIT(18));

        //[3] DINC    1'b1    Set 1 to enable incremental address
        u4Val = (u4Val | BIT(3));
        //[2] SINC    1'b0    Set 0 to disable incremental address
        u4Val = (u4Val & (~BIT(2)));
    }

    // [17]    WPEN    1'b0    Set 0 to disable wrapping
    u4Val = (u4Val & (~BIT(17)));
    // [15]    ITEN    1'b0    Set 0 to disable interrupt
    u4Val = (u4Val & (~BIT(15)));
    // [10:8]  BURST   3'b000  Set 0 for single-byte burst
    u4Val = (u4Val & (~BITS(8, 10)));
    // [5] B2W 1'b0    Set 0 to disable
    u4Val = (u4Val & (~BIT(5)));
    // [4] DREQ    1'b1    Set 1 to enable HW handshake
    u4Val = (u4Val | BIT(4));
    // [1:0]   SIZE    2'b00   Set 0 for single-byte transfer
    u4Val = (u4Val & (~BITS(0, 1)));
    (pI2CHalfGDMATypeDef->DMAn_CON) = u4Val;


    // Enable GDMA W   GDMA_BASE + 0x0N18  [15]    STR 1'b0    Set 0 to reset DMA transfer
    //             W   GDMA_BASE + 0x0N18  [15]    STR 1'b1    Set 1 to start DMA transfer
    u4Val = (pI2CHalfGDMATypeDef->DMAn_START);
    (pI2CHalfGDMATypeDef->DMAn_START) = (u4Val & (~BIT(15)));
    (pI2CHalfGDMATypeDef->DMAn_START) = (u4Val | BIT(15));
#endif

    // /* Set DMA limiter to slowdown DMA. Avoid DMA and I2C handshake error */
    extern pdma_status_t pdma_set_limiter(pdma_channel_t channel, uint8_t limiter);
    pdma_set_limiter((pdma_channel_t)ucGDMAIdx, 0x80);

    /* I2C TX */
    if (1 == ucTxMode) {
        pdma_init((pdma_channel_t)ucGDMAIdx);
        /* total dma length = size * count */
        i2c_dma_config.burst_mode = false;
        i2c_dma_config.count = u2Len;
        i2c_dma_config.master_type = PDMA_TX;
        i2c_dma_config.size = PDMA_BYTE;
        pdma_configure((pdma_channel_t)ucGDMAIdx, &i2c_dma_config);
        pdma_start_without_polling((pdma_channel_t)ucGDMAIdx, (UINT32)(pucMemAddr));
    }
    /* I2C RX */
    else
    {
        pdma_init((pdma_channel_t)ucGDMAIdx);
        /* total dma length = size * count */
        i2c_dma_config.burst_mode = false;
        i2c_dma_config.count = u2Len;
        i2c_dma_config.master_type = PDMA_RX;
        i2c_dma_config.size = PDMA_BYTE;
        pdma_configure((pdma_channel_t)ucGDMAIdx, &i2c_dma_config);
        pdma_start_without_polling((pdma_channel_t)ucGDMAIdx, (UINT32)(pucMemAddr));
    }

    return ret;
}


ENUM_HAL_RET_T halI2CGDMANormalWrite(UINT8 ucI2CIdx, UINT8 ucGDMAIdx, UINT8 ucSlaveAddress, const UINT8 *pucTxData, UINT16 u2Length)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    IOT_I2C_TypeDef *pI2CTypeDef = NULL;

    pI2CTypeDef = halI2CGetBaseAddr(ucI2CIdx);
    //if (NULL == pI2CTypeDef)
    //{
    //    return HAL_RET_FAIL;
    // }

    // Sanity check
    if (!pucTxData) {
        return HAL_RET_FAIL;
    }

    // General mode Disable
    (pI2CTypeDef->MM_CON0) &= (~BIT(I2C_MM_GMODE_OFFSET));


    // a. Read MM_STATUS[2]. The I2C master circuit is ready for preparing the next trigger if the value is 1.
    if (!((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET))) {
        log_hal_error("[I2C][DMA_Write] busy;length=%d.\r\n", u2Length);
        return HAL_RET_FAIL;
    }
    // b. Configure GDMA for Tx data.
    halI2CGDMACtrl(ucI2CIdx, ucGDMAIdx, 1, u2Length, pucTxData);

    // c. Set a value of MM_ID_CON0 to decide which slave you want to write.
    (pI2CTypeDef->MM_ID_CON0) &= (~I2C_MM_SLAVE_ID_MASK);
    (pI2CTypeDef->MM_ID_CON0) |= ((ucSlaveAddress & (I2C_MM_SLAVE_ID_MASK)) << I2C_MM_SLAVE_ID_OFFSET);

    // d. Set MM_PACK_CON0[0] to 0 for write.
    (pI2CTypeDef->MM_PACK_CON0) &= (~BIT(I2C_MM_PACK_RW_OFFSET));

    // e. Set MM_CON0[0] to 1 to trigger the master to write data to slave.
    (pI2CTypeDef->MM_CON0) |= BIT(I2C_MM_START_EN_OFFSET);

    return ret;
}

ENUM_HAL_RET_T halI2CGDMANormalRead(UINT8 ucI2CIdx, UINT8 ucGDMAIdx, UINT8 ucSlaveAddress, PUINT8 pucRxData, UINT16 u2Length)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;
    IOT_I2C_TypeDef *pI2CTypeDef = NULL;

    pI2CTypeDef = halI2CGetBaseAddr(ucI2CIdx);
    //if (NULL == pI2CTypeDef)
    //{
    //    printf("[%s](NULL == pI2CTypeDef)\n", __FUNCTION__);
    //    return HAL_RET_FAIL;
    //}

    if (NULL == pucRxData) {
        return HAL_RET_FAIL;
    }

    // General mode Disable
    (pI2CTypeDef->MM_CON0) &= (~BIT(I2C_MM_GMODE_OFFSET));

    // a. Read MM_STATUS[2] as 1.
    if (!((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET))) {
        printf("[%s](!((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET)))\n", __FUNCTION__);
        log_hal_error("[I2C][DMA_Read] busy;length=%d.\r\n", u2Length);
        return HAL_RET_FAIL;
    }

    // b. Set MM_ID_CON0 and MM_ID_CON1 for slave address.
    (pI2CTypeDef->MM_ID_CON0) &= (~I2C_MM_SLAVE_ID_MASK);
    (pI2CTypeDef->MM_ID_CON0) |= ((ucSlaveAddress & (I2C_MM_SLAVE_ID_MASK)) << I2C_MM_SLAVE_ID_OFFSET);

    // c. Set MM_PACK_CON0[0] to 1 for read.
    (pI2CTypeDef->MM_PACK_CON0) |= BIT(I2C_MM_PACK_RW_OFFSET);

    // d. Set the MM_CNT_BYTE_VAL_PK0 to decide how many data to be read.
    (pI2CTypeDef->MM_CNT_BYTE_VAL_PK0) = u2Length;

    // Configure GDMA for Rx data.
    halI2CGDMACtrl(ucI2CIdx, ucGDMAIdx, 0, u2Length, pucRxData);

    // e. Set MM_CON0[0] to 1
    (pI2CTypeDef->MM_CON0) |= BIT(I2C_MM_START_EN_OFFSET);

    return ret;
}

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
                                   UINT16 u2Pkt2Len)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;

    UINT16  u2TxResidual = 0;
    UINT16  u2RxResidual = 0;

    IOT_I2C_TypeDef *pI2CTypeDef = NULL;

    pI2CTypeDef = halI2CGetBaseAddr(ucI2CIdx);
    //if (NULL == pI2CTypeDef)
    //{
    //    return HAL_RET_FAIL;
    //}

    // Sanity check
    if (ucPktNum > I2C_PKT_MAX_NUM) {
        return HAL_RET_FAIL;
    }

    if ((!pucTxData) || (!pucRxData)) {
        return HAL_RET_FAIL;
    }

    // General mode enable
    (pI2CTypeDef->MM_CON0) |= BIT(I2C_MM_GMODE_OFFSET);

    // a. Read MM_STATUS[2]. The I2C master circuit is ready for preparing the next trigger if the value is 1.
    if (!((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET))) {
        printf("[%s](!((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET)))\n", __FUNCTION__);
        log_hal_error("[I2C][DMA_RW]  not ready.\r\n");
        return HAL_RET_FAIL;
    }

    // b. Set a value of MM_ID_CON0 to decide which slave you want to write.
    (pI2CTypeDef->MM_ID_CON0) &= (~I2C_MM_SLAVE_ID_MASK);
    (pI2CTypeDef->MM_ID_CON0) |= ((ucSlaveAddress & (I2C_MM_SLAVE_ID_MASK)) << I2C_MM_SLAVE_ID_OFFSET);

    // c. Set MM_PACK_CON0[5:4] packet number.
    (pI2CTypeDef->MM_PACK_CON0) &= ~(I2C_MM_PACK_VAL_MASK);
    (pI2CTypeDef->MM_PACK_CON0) |= ((ucPktNum - 1) << I2C_MM_PACK_VAL_OFFSET);

    // d. Set the MM_CNT_BYTE_VAL_PK0/1/2 to decide data length.
    (pI2CTypeDef->MM_CNT_BYTE_VAL_PK0) = u2Pkt0Len;
    (pI2CTypeDef->MM_CNT_BYTE_VAL_PK1) = u2Pkt1Len;
    (pI2CTypeDef->MM_CNT_BYTE_VAL_PK2) = u2Pkt2Len;

    // e. Set each PKT's R/W mode
    if (ucPktNum) {
        if (ucPkt0RW) {
            // read
            u2RxResidual += u2Pkt0Len;
            (pI2CTypeDef->MM_PACK_CON0) |= BIT(0);
        } else {
            // write
            u2TxResidual += u2Pkt0Len;
            (pI2CTypeDef->MM_PACK_CON0) &= (~BIT(0));
        }
        ucPktNum--;
    }
    if (ucPktNum) {
        if (ucPkt1RW) {
            // read
            u2RxResidual += u2Pkt1Len;
            (pI2CTypeDef->MM_PACK_CON0) |= BIT(1);
        } else {
            // write
            u2TxResidual += u2Pkt1Len;
            (pI2CTypeDef->MM_PACK_CON0) &= (~BIT(1));
        }
        ucPktNum--;
    }
    if (ucPktNum) {
        if (ucPkt2RW) {
            // read
            u2RxResidual += u2Pkt2Len;
            (pI2CTypeDef->MM_PACK_CON0) |= BIT(2);
        } else {
            // write
            u2TxResidual += u2Pkt2Len;
            (pI2CTypeDef->MM_PACK_CON0) &= (~BIT(2));
        }
        ucPktNum--;
    }

    if (u2TxResidual > 0) {
        // Configure GDMA for Tx data.
        halI2CGDMACtrl(ucI2CIdx, ucTxGDMAIdx, 1, u2TxResidual, pucTxData);
    }

    if (u2RxResidual > 0) {
        // Configure GDMA for Rx data.
        halI2CGDMACtrl(ucI2CIdx, ucRxGDMAIdx, 0, u2RxResidual, pucRxData);
    }
    // g. Set MM_CON0[0] to 1 to trigger the master to write data to slave.
    (pI2CTypeDef->MM_CON0) |= BIT(I2C_MM_START_EN_OFFSET);

    return ret;
}


#if (WIFI_BUILD_OPTION == 1)
// Only for DVT, NOT for release
// GDMA length < I2C Tx length
ENUM_HAL_RET_T halI2CGDMAGeneralRW_DVT_DEBUG(UINT8 ucI2CIdx,
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
        UINT16 u2Pkt2Len)
{
    ENUM_HAL_RET_T ret = HAL_RET_SUCCESS;

    UINT16  u2TxResidual = 0;
    UINT16  u2RxResidual = 0;

    IOT_I2C_TypeDef *pI2CTypeDef = NULL;

    pI2CTypeDef = halI2CGetBaseAddr(ucI2CIdx);
    //if (NULL == pI2CTypeDef)
    //{
    //    return HAL_RET_FAIL;
    //}

    // Sanity check
    if (ucPktNum > I2C_PKT_MAX_NUM) {
        return HAL_RET_FAIL;
    }

    if ((!pucTxData) || (!pucRxData)) {
        return HAL_RET_FAIL;
    }

    // General mode enable
    (pI2CTypeDef->MM_CON0) |= BIT(I2C_MM_GMODE_OFFSET);

    // a. Read MM_STATUS[2]. The I2C master circuit is ready for preparing the next trigger if the value is 1.
    if (!((pI2CTypeDef->MM_STATUS) & BIT(I2C_MM_START_READY_OFFSET))) {
        return HAL_RET_FAIL;
    }

    // b. Set a value of MM_ID_CON0 to decide which slave you want to write.
    (pI2CTypeDef->MM_ID_CON0) &= (~I2C_MM_SLAVE_ID_MASK);
    (pI2CTypeDef->MM_ID_CON0) |= ((ucSlaveAddress & (I2C_MM_SLAVE_ID_MASK)) << I2C_MM_SLAVE_ID_OFFSET);

    // c. Set MM_PACK_CON0[5:4] packet number.
    (pI2CTypeDef->MM_PACK_CON0) &= ~(I2C_MM_PACK_VAL_MASK);
    (pI2CTypeDef->MM_PACK_CON0) |= ((ucPktNum - 1) << I2C_MM_PACK_VAL_OFFSET);

    // d. Set the MM_CNT_BYTE_VAL_PK0/1/2 to decide data length.
    (pI2CTypeDef->MM_CNT_BYTE_VAL_PK0) = u2Pkt0Len;
    (pI2CTypeDef->MM_CNT_BYTE_VAL_PK1) = u2Pkt1Len;
    (pI2CTypeDef->MM_CNT_BYTE_VAL_PK2) = u2Pkt2Len;

    // e. Set each PKT's R/W mode
    if (ucPktNum) {
        if (ucPkt0RW) {
            // read
            u2RxResidual += u2Pkt0Len;
            (pI2CTypeDef->MM_PACK_CON0) |= BIT(0);
        } else {
            // write
            u2TxResidual += u2Pkt0Len;
            (pI2CTypeDef->MM_PACK_CON0) &= (~BIT(0));
        }
        ucPktNum--;
    }
    if (ucPktNum) {
        if (ucPkt1RW) {
            // read
            u2RxResidual += u2Pkt1Len;
            (pI2CTypeDef->MM_PACK_CON0) |= BIT(1);
        } else {
            // write
            u2TxResidual += u2Pkt1Len;
            (pI2CTypeDef->MM_PACK_CON0) &= (~BIT(1));
        }
        ucPktNum--;
    }
    if (ucPktNum) {
        if (ucPkt2RW) {
            // read
            u2RxResidual += u2Pkt2Len;
            (pI2CTypeDef->MM_PACK_CON0) |= BIT(2);
        } else {
            // write
            u2TxResidual += u2Pkt2Len;
            (pI2CTypeDef->MM_PACK_CON0) &= (~BIT(2));
        }
        ucPktNum--;
    }

    if (u2TxResidual > 0) {
        // Configure GDMA for Tx data.
        halI2CGDMACtrl(ucI2CIdx, ucTxGDMAIdx, 1, (u2TxResidual >> 1), pucTxData);
    }
    if (u2RxResidual > 0) {
        // Configure GDMA for Rx data.
        halI2CGDMACtrl(ucI2CIdx, ucRxGDMAIdx, 0, (u2RxResidual >> 1), pucRxData);
    }
    // g. Set MM_CON0[0] to 1 to trigger the master to write data to slave.
    (pI2CTypeDef->MM_CON0) |= BIT(I2C_MM_START_EN_OFFSET);

    return ret;
}
#endif // #if (WIFI_BUILD_OPTION == 1)

#endif
