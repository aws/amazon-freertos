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

#ifndef __MT7687_CONF_H__
#define __MT7687_CONF_H__


#define OSC_FREQUENCY		40000000
#define CPU_FREQUENCY		40000000	/* HCLK : CPU = 1: 1*/

#define __NVIC_PRIO_BITS	3	/*!< uses 3 Bits for the Priority Levels    */
#define __FPU_PRESENT             1       /*!< FPU present */

typedef enum IRQn {
    /****  CM4 internal exceptions  **********/
    Reset_IRQn                  = -15,	/* Reset */
    NonMaskableInt_IRQn         = -14,	/* NMI */
    HardFault_IRQn              = -13,	/* HarFault */
    MemoryManagement_IRQn       = -12,	/* Memory Management */
    BusFault_IRQn               = -11,	/* Bus Fault  */
    UsageFault_IRQn             = -10,	/* Usage Fault */
    SVC_IRQn                    = -5,	/* SV Call*/
    DebugMonitor_IRQn           = -4,	/* Debug Monitor */
    PendSV_IRQn                 = -2,	/* Pend SV */
    SysTick_IRQn                = -1,	/* System Tick */

    /****  MT7687 specific external/peripheral interrupt ****/
//	PERI0_IRQn                  = 0,
//	PERI1_IRQn                  = 1,
    CM4_UART1_IRQ = 0 ,
    CM4_DMA_IRQ   = 1 ,
    CM4_HIF_IRQ   = 2 ,
    CM4_I2C1_IRQ  = 3 ,
    CM4_I2C2_IRQ  = 4 ,
    CM4_UART2_IRQ = 5 ,
    CM4_MTK_CRYPTO_IRQ = 6,
    CM4_SF_IRQ      = 7,
    CM4_EINT_IRQ    = 8,
    CM4_BTIF_IRQ    = 9,
    CM4_WDT_IRQ      = 10,
    CM4_AHB_MON_IRQ   = 11,
    CM4_SPI_SLAVE_IRQ = 12,
    CM4_N9_WDT_IRQ    = 13,
    CM4_ADC_IRQ       = 14,
    CM4_IRDA_TX_IRQ   = 15,
    CM4_IRDA_RX_IRQ   = 16,
    CM4_USB_VBUS_ON_IRQ   = 17,
    CM4_USB_VBUS_OFF_IRQ  = 18,
    CM4_SPIS_CS_WAKEUP_IRQ = 19, /* for CM4 Wakeup */
    CM4_GPT3_IRQ       = 20,
    CM4_IRQ21_IRQ      = 21, /* reserved */
    CM4_AUDIO_IRQ      = 22,
    CM4_HIF_DMA_IRQ    = 23,
    CM4_GPT_IRQ        = 24,
    CM4_ADC_COMP_IRQ   = 25,
    CM4_AUDIO_WAKE_IRQ = 26,
    CM4_SPIM_IRQ       = 27,
    CM4_USB_IRQ        = 28,
    CM4_UDMA_IRQ       = 29,
    CM4_TRNG_IRQ       = 30,
    IRQ_NUMBER_MAX     = 96
} IRQn_Type;

typedef IRQn_Type hal_nvic_irq_t;

#include "core_cm4.h"                  /* Core Peripheral Access Layer */
#include "system_mt7687.h"

#define NVIC_RAM_VECTOR_ADDRESS   (0x20000000)  // Location of vectors in RAM
#define NVIC_FLASH_VECTOR_ADDRESS (0x10000000)       // Initial vector position in flash


/******************************************************************************/
/*                          CM4 peripheral IRQ number                         */
/******************************************************************************/


#define CM4_WIC_INT_WAKE_UP_IRQ 31
#define CM4_WIC_0_IRQ           32 //PAD_ANTSEL2
#define CM4_WIC_1_IRQ           33 //PAD_WAKE_N
#define CM4_WIC_2_IRQ           34 //PAD_SDIO_CLK
#define CM4_WIC_3_IRQ           35 //PAD_SDIO_DAT3
#define CM4_WIC_4_IRQ           36 //PAD_SDIO_DAT0
#define CM4_WIC_5_IRQ           37 //PAD_GPIO0
#define CM4_WIC_6_IRQ           38 //PAD_GPIO1
#define CM4_WIC_7_IRQ           39 //PAD_UART_RX
#define CM4_WIC_8_IRQ           40 //PAD_WF_RF_DIS_B
#define CM4_WIC_9_IRQ           41 //PAD_BT_RF_DIS_B
#define CM4_WIC_10_IRQ          42 //PAD_WF_LED_B
#define CM4_WIC_11_IRQ          43 //PAD_BT_LED_B

#define CM4_EINT_0_IRQ          56 //PAD_ANTSEL0
#define CM4_EINT_1_IRQ          57 //PAD_ANTSEL1
#define CM4_EINT_2_IRQ          58 //PAD_ANTSEL3
#define CM4_EINT_3_IRQ          59 //PAD_ANTSEL4
#define CM4_EINT_4_IRQ          60 //PAD_ANTSEL5
#define CM4_EINT_5_IRQ          61 //PAD_ANTSEL6
#define CM4_EINT_6_IRQ          62 //PAD_ANTSEL7

#define CM4_EINT_19_IRQ         75 //PAD_UART_DBG
#define CM4_EINT_20_IRQ         76 //PAD_UART_TX
#define CM4_EINT_21_IRQ         77 //PAD_UART_RTS
#define CM4_EINT_22_IRQ         78 //PAD_UART_CTS

/* IRQ Priority */
#define DEFAULT_PRI             5
#define HIGHEST_PRI             9
#define CM4_UART1_PRI			DEFAULT_PRI
#define CM4_DMA_PRI			    DEFAULT_PRI
#define CM4_HIF_PRI			    DEFAULT_PRI
#define CM4_I2C1_PRI			DEFAULT_PRI
#define CM4_I2C2_PRI			DEFAULT_PRI
#define CM4_UART2_PRI			DEFAULT_PRI
#define CM4_MTK_CRYPTO_PRI		DEFAULT_PRI
#define CM4_SF_PRI			    DEFAULT_PRI
#define CM4_EINT_PRI			DEFAULT_PRI
#define CM4_BTIF_PRI			DEFAULT_PRI
#define CM4_WDT_PRI			    DEFAULT_PRI
#define CM4_AHB_MON_PRI			DEFAULT_PRI
#define CM4_SPI_SLAVE_PRI		DEFAULT_PRI
#define CM4_N9_WDT_PRI			DEFAULT_PRI
#define CM4_ADC_PRI			    DEFAULT_PRI
#define CM4_IRDA_TX_PRI			DEFAULT_PRI
#define CM4_IRDA_RX_PRI			DEFAULT_PRI
#define CM4_USB_VBUS_ON_PRI		DEFAULT_PRI
#define CM4_USB_VBUS_OFF_PRI	DEFAULT_PRI
#define CM4_SPIS_CS_WAKEUP_PRI	DEFAULT_PRI
#define CM4_GPT3_PRI			DEFAULT_PRI
#define CM4_PRI21_PRI			DEFAULT_PRI
#define CM4_AUDIO_PRI			DEFAULT_PRI
#define CM4_HIF_DMA_PRI			DEFAULT_PRI
#define CM4_GPT_PRI			    DEFAULT_PRI
#define CM4_ADC_COMP_PRI		DEFAULT_PRI
#define CM4_AUDIO_WAKE_PRI	    DEFAULT_PRI
#define CM4_SPIM_PRI			DEFAULT_PRI
#define CM4_USB_PRI			    DEFAULT_PRI
#define CM4_UDMA_PRI			DEFAULT_PRI
#define CM4_TRNG_PRI            DEFAULT_PRI

#define CM4_WIC_INT_WAKE_UP_PRI  HIGHEST_PRI
#define CM4_WIC_0_PRI            DEFAULT_PRI
#define CM4_WIC_1_PRI            DEFAULT_PRI
#define CM4_WIC_2_PRI            DEFAULT_PRI
#define CM4_WIC_3_PRI            DEFAULT_PRI
#define CM4_WIC_4_PRI            DEFAULT_PRI
#define CM4_WIC_5_PRI            DEFAULT_PRI
#define CM4_WIC_6_PRI            DEFAULT_PRI
#define CM4_WIC_7_PRI            DEFAULT_PRI
#define CM4_WIC_8_PRI            DEFAULT_PRI
#define CM4_WIC_9_PRI            DEFAULT_PRI
#define CM4_WIC_10_PRI           DEFAULT_PRI
#define CM4_WIC_11_PRI           DEFAULT_PRI

#define CM4_EINT_0_PRI           DEFAULT_PRI
#define CM4_EINT_1_PRI           DEFAULT_PRI
#define CM4_EINT_2_PRI           DEFAULT_PRI
#define CM4_EINT_3_PRI           DEFAULT_PRI
#define CM4_EINT_4_PRI           DEFAULT_PRI
#define CM4_EINT_5_PRI           DEFAULT_PRI
#define CM4_EINT_6_PRI           DEFAULT_PRI

#define CM4_EINT_19_PRI          DEFAULT_PRI
#define CM4_EINT_20_PRI          DEFAULT_PRI
#define CM4_EINT_21_PRI          DEFAULT_PRI
#define CM4_EINT_22_PRI          DEFAULT_PRI

#define CM4_RTC_ALARM_PRI        DEFAULT_PRI


#ifdef __cplusplus
extern "C" {
#endif


typedef void (*NVIC_IRQ_Handler)(void);

//extern int NVIC_Register(int irqn, NVIC_IRQ_Handler handler);
//extern int NVIC_UnRegister(int irqn);

#ifndef _STDLIB_H_
void abort(void);
#endif



#ifdef __cplusplus
}
#endif

#endif
