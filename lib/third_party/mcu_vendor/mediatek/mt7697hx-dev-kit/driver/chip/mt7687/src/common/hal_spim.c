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

#include "hal_spi_master.h"
#ifdef HAL_SPI_MASTER_MODULE_ENABLED

#include <stdio.h>
#include "type_def.h"
//#include "mem_util.h"
#include "timer.h"
#include "top.h"
#include "hal_spim.h"
#include "nvic.h"
//#include "dma_hw.h"
#include "pinmux.h"
#include "hal_gpio_7687.h"
#include "mt7687.h"
#include "hal_nvic.h"

HALSPIM_ISR halspim_isr;
void spim_irqhandler(hal_nvic_irq_t irq_number)
{
    volatile UINT32 reg;
    //printf("SPIM IRQ!!\n");
    halspim_isr();
    reg = spi_reg_inl(SPI_REG_STATUS);  // read to clear interrupt
    reg = reg;
    return;
}

void spim_isr_Register(HALSPIM_ISR spim_isr)
{
    halspim_isr = spim_isr;
}

/*
In real chip, spi module clock is 120Mhz
Use below define can generate 4, 6, 8, 10, 12Mhz
SPI_4M_DIV_VALUE, SPI_6M_DIV_VALUE
SPI_8M_DIV_VALUE, SPI_10M_DIV_VALUE
SPI_12M_DIV_VALUE
*/

INT32 halSpim_init(ULONG setting, ULONG clock)
{
    /* TODO: reset SPI */
    volatile UINT32 reg;
    volatile UINT32 *pTopCfgCM4PWRCtl = (volatile UINT32 *)TOP_CFG_CM4_PWR_CTL_CR;
    reg = cmnReadRegister32(pTopCfgCM4PWRCtl);
    reg = (reg >>  CM4_MPLL_EN_SHIFT) & CM4_MPLL_EN_MASK;

    if (reg != CM4_MPLL_EN_PLL1_ON_PLL2_ON) {
        cmnPLL1ON_PLL2ON(KAL_TRUE);
    }

    if ((DRV_Reg32(TOP_CFG_HCLK_2M_CKGEN) & (1 << 17)) != 0) {
        DRV_Reg32(TOP_CFG_HCLK_2M_CKGEN) &= ~(1 << 17);
        delay_ms(500);
    }

    hal_nvic_register_isr_handler(CM4_SPIM_IRQ, spim_irqhandler);
    NVIC_SetPriority(CM4_SPIM_IRQ, CM4_SPIM_PRI);

    NVIC_EnableIRQ(CM4_SPIM_IRQ); /* TODO: use irq save */

    /* Currently use default value: prefetch, spi_start_sel, cs_dsel_cnt, clock, clk_mode */
    spi_reg_and(SPI_REG_MASTER, ~0xEFFF067B);
    spi_reg_or(SPI_REG_MASTER, setting | (clock << SPI_MASTER_CLOCK_DIV_SHIFT));
    return 0;
}



INT32 spim_busy_wait(void)
{
    int n = 60000;
    do {
        if ((spi_reg_inl(SPI_REG_CTL) & SPI_CTL_BUSY) == 0) {
            return 0;
        }
    } while (--n > 0);

    //printf("%s: fail \n", __func__);
    return -1;
}

INT32 spim_more_buf_trans_gpio(const UINT32 op_addr, const size_t n_cmd_byte, UINT8 *buf, const size_t buf_cnt, const ULONG flag)
{
    UINT32 data;
    int i, q, r;
    int rc = -1;
    UINT8 *p_buf;

    /* step 0. enable more byte mode */
    spi_reg_or(SPI_REG_MASTER, SPI_MASTER_MB_MODE_ENABLE);

    spi_reg_outl(SPI_REG_OPCODE, op_addr);
    spi_reg_outl(SPI_REG_MOREBUF, ((n_cmd_byte * 8) << 24));

    /* step 2. write DI/DO data #0 ~ #7 */
    if (flag & SPI_WRITE_FLAG) {
        if (buf == NULL) {
            goto RET_MB_TRANS;
        }
        q = buf_cnt / 4;
        r = buf_cnt % 4;
        p_buf = buf;
        for (i = 0; i < q; i++) {
            spi_reg_outl(SPI_REG_DATA(i), *(uint32_t *)p_buf);
            p_buf += 4;
        }
        if (r > 0) {
            data = 0;
            for (i = 0; i < r; i++) {
                data |=  (*p_buf) << (8 * i);
                p_buf++;
            }
            spi_reg_outl(SPI_REG_DATA(q), data);
        } else {
            if (q < 8) {
                spi_reg_outl(SPI_REG_DATA(q), 0);
            }
        }
    } else {
        spi_reg_outl(SPI_REG_DATA(0), 0);
    }

    /* step 3. set rx (miso_bit_cnt) and tx (mosi_bit_cnt) bit count */
    spi_reg_and(SPI_REG_MOREBUF, ~SPI_MBCTL_TX_RX_CNT_MASK);
    if (flag & SPI_WRITE_FLAG) {
        spi_reg_or(SPI_REG_MOREBUF, buf_cnt << 3);
    } else {
        spi_reg_or(SPI_REG_MOREBUF, (buf_cnt << 3 << 12));
    }

    /* step 4. kick */
    //DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_DOUT2_RESET, (1 << 0));  //cs pull low
    spi_reg_or(SPI_REG_CTL, SPI_CTL_START);
    /* step 5. wait spi_master_busy */
    spim_busy_wait();
    //DRV_WriteReg32(IOT_GPIO_AON_BASE + IOT_GPIO_DOUT2_SET, (1 << 0));

    if (flag & SPI_WRITE_FLAG) {
        rc = 0;
        goto RET_MB_TRANS;
    }

    /* step 6. read DI/DO data #0 */
    if (flag & SPI_READ_FLAG) {
        if (buf == NULL) {
            //printf("%s: read null buf\n", __func__);
            return -1;
        }
        q = buf_cnt / 4;
        r = buf_cnt % 4;
        p_buf = buf;
        for (i = 0; i < q; i++) {
            *(uint32_t *)p_buf = spi_reg_inl(SPI_REG_DATA(i));
            p_buf += 4;
        }
        if (r > 0) {
            data = spi_reg_inl(SPI_REG_DATA(q));
            for (i = 0; i < r; i++) {
                *(p_buf + i) = (UINT8)(data >> (i * 8));
            }
        }
    }

    rc = 0;

RET_MB_TRANS:
    /* disable more-buf mode */
    spi_reg_and(SPI_REG_MASTER, ~(SPI_MASTER_MB_MODE_ENABLE));
    return rc;
}

INT32 spim_more_buf_trans_gpio_full_duplex(UINT32 op_addr, const size_t op_byte, const UINT8 *tx_buf, const size_t tx_byte, UINT8 *rx_buf, const size_t rx_byte)
{
    UINT32 data;
    int i, q, r;
    UINT8 *p_buf;

    spi_reg_or(SPI_REG_MASTER, SPI_MASTER_MB_MODE_ENABLE); /* enable more byte mode */
    //spi_reg_outl(SPI_REG_OPCODE, op_addr);
    spi_reg_or(SPI_REG_MASTER, (1 << 10)); /* enable full duplex mode */

    /* write data to CMD register */
    spi_reg_outl(SPI_REG_MOREBUF, ((op_byte * 8) << 24)); /* set cmd_bit_cnt to 32 */
    spi_reg_outl(SPI_REG_OPCODE, op_addr);

    /* step 2. write DI/DO data #0 ~ #3 */
    if (tx_byte > 0) {
        q = tx_byte / 4;
        r = tx_byte % 4;
        p_buf = (UINT8 *)tx_buf;
        for (i = 0; i < q; i++) {
            spi_reg_outl(SPI_REG_DATA(i), *(uint32_t *)p_buf);
            p_buf += 4;
        }
        if (r > 0) {
            data = 0;
            for (i = 0; i < r; i++) {
                data |=  (*p_buf) << (8 * i);
                p_buf++;
            }
            spi_reg_outl(SPI_REG_DATA(q), data);
            q++;
        }
        for (i = q; i < 4; i++) {
            spi_reg_outl(SPI_REG_DATA(i), 0);
        }
    } else {
        for (i = 0; i < 4; i++) {
            spi_reg_outl(SPI_REG_DATA(i), 0);
        }
    }

    /* step 3. set rx (miso_bit_cnt) and tx (mosi_bit_cnt) bit count */
    spi_reg_and(SPI_REG_MOREBUF, ~SPI_MBCTL_TX_RX_CNT_MASK);
    if (tx_byte >= rx_byte) {
        spi_reg_or(SPI_REG_MOREBUF, (tx_byte << 3) | (tx_byte << 3 << 12));
    } else {
        spi_reg_or(SPI_REG_MOREBUF, (rx_byte << 3) | (rx_byte << 3 << 12));
    }

    /* step 4. kick */
    spi_reg_or(SPI_REG_CTL, SPI_CTL_START);

    /* step 5. wait spi_master_busy */
    spim_busy_wait();

    /* step 6. read DI/DO data #4 ~ #7 */
    q = rx_byte / 4;
    r = rx_byte % 4;
    p_buf = rx_buf;
    for (i = 0; i < q; i++) {
        *(uint32_t *)p_buf = spi_reg_inl(SPI_REG_DATA(i + 4));
        p_buf += 4;
    }
    if (r > 0) {
        data = spi_reg_inl(SPI_REG_DATA(q + 4));
        for (i = 0; i < r; i++) {
            *(p_buf + i) = (UINT8)(data >> (i * 8));
        }
    }

    /* disable more-buf mode and full duplex mode */
    spi_reg_and(SPI_REG_MASTER, ~(SPI_MASTER_MB_MODE_ENABLE | DUPLEX_MASK));

    return 0;
}

#endif /* HAL_SPI_MASTER_MODULE_ENABLED */
