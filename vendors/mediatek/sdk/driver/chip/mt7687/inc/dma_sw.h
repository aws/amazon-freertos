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

/**
 * @file dma_sw.h
 *
 *  This header file exposes the API for DMA.
 *
 */

#ifndef _DMA_SW_H
#define _DMA_SW_H

#include "hal_platform.h"
#ifdef HAL_GDMA_MODULE_ENABLED
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

#define DMA_CON_MASTER_I2C1_TX  0x02
#define DMA_CON_MASTER_I2C1_RX  0x03
#define DMA_CON_MASTER_I2C2_TX  0x04
#define DMA_CON_MASTER_I2C2_RX  0x05
#define DMA_CON_MASTER_I2S_TX   0x06
#define DMA_CON_MASTER_I2S_RX   0x07
#define DMA_CON_MASTER_UART1TX  0x08
#define DMA_CON_MASTER_UART1RX  0x09
#define DMA_CON_MASTER_UART2TX  0x0A
#define DMA_CON_MASTER_UART2RX  0x0B
#define DMA_CON_MASTER_BTIF_TX  0x0C
#define DMA_CON_MASTER_BTIF_RX  0x0D
#define DMA_CON_MASTER_EP2I_TX  0x0E
#define DMA_CON_MASTER_EP2O_RX  0x0F
#define DMA_CON_MASTER_EP3I_TX  0x10
#define DMA_CON_MASTER_EP3O_RX  0x11
#define DMA_CON_MASTER_EP4I_TX  0x12
#define DMA_CON_MASTER_EP4O_RX  0x13
#define DMA_CON_MASTER_ADC_RX   0x14
#define DMA_CON_MASTER_HIF_TRX  0x15
#define DMA_CON_MASTER_SPIM_TX  0x16
#define DMA_CON_MASTER_SPIM_RX  0x17

#define DMA_VFIFO_CH_S          12   /* VFIFO start channel */
#define DMA_VFIFO_CH_E          25   /* VFIFO end channel */

#define DMA_MAX_CHANNEL         11 // not include VFF DMA
#define DMA_MAX_FULL_CHANNEL    2
typedef enum {
    DMA_I2C1_TX = DMA_CON_MASTER_I2C1_TX,
    DMA_I2C1_RX,
    DMA_I2C2_TX,
    DMA_I2C2_RX,
    DMA_I2S_TX,
    DMA_I2S_RX,
    DMA_UART1TX,
    DMA_UART1RX,
    DMA_UART2TX,
    DMA_UART2RX,
    DMA_BTIF_TX,
    DMA_BTIF_RX,
    DMA_EP2I_TX,
    DMA_EP2O_RX,
    DMA_EP3I_TX,
    DMA_EP3O_RX,
    DMA_EP4IH_TX,
    DMA_EP4IV_TX,
    DMA_EP4O_RX,
    DMA_ADC_RX,
    DMA_HIF_TRX,
    DMA_SPIM_TX,
    DMA_SPIM_RX,
    DMA_SW,
    DMA_IDLE
} DMA_Master;

typedef enum {
    DMA_BYTE = 0,
    DMA_SHORT,
    DMA_LONG
} DMA_TranSize;

typedef enum {
    DMA_HWTX,           /* use DMA_HWMENU, from RAM to register */
    DMA_HWRX,           /* use DMA_HWMENU, from register to RAM */
    DMA_SWCOPY,         /* use DMA_SWCOPYMENU, from RAM to RAM */
    DMA_HWTX_RINGBUFF,  /* use DMA_HWRINGBUFF_MENU, from RAM to register */
    DMA_HWRX_RINGBUFF   /* use DMA_HWRINGBUFF_MENU, from register to RAM */
} DMA_Type;

typedef struct {
    bool                        burst_mode;    /* burst mode = 0 ==> single mode */
    uint8_t                     cycle;        /* active only when (burst_mode == TRUE) */
} DMA_TMODE;

typedef enum {
    VDMA_I2S_TX_CH              = DMA_VFIFO_CH_S,
    VDMA_I2S_RX_CH,
    VDMA_UART1TX_CH,
    VDMA_UART1RX_CH,
    VDMA_UART2TX_CH,
    VDMA_UART2RX_CH,
    VDMA_BTIF_TX_CH,
    VDMA_BTIF_RX_CH,
    VDMA_USB_EP2O_CH,
    VDMA_USB_EP3I_CH,
    VDMA_USB_EP3O_CH,
    VDMA_USB_EP4I_CH,
    VDMA_USB_EP4O_CH,
    VDMA_ADC_RX_CH,
} DMA_VFIFO_CHANNEL;

typedef struct {
    DMA_TMODE                   TMOD;
    DMA_Master                  master;
    uint32_t                    addr;
    void                        *WPPT;
    void                        *WPTO;
} DMA_HWRINGBUFF_MENU;

typedef struct {
    DMA_TMODE                   TMOD;
    DMA_Master                  master;
    uint32_t                    addr;
} DMA_HWMENU;

typedef struct {
    DMA_TMODE                   TMOD;
    DMA_Master                  master;
    uint32_t                    source;
    uint32_t                    destination;
    void                        *WPPT;
    void                        *WPTO;
} DMA_FULLSIZE_HWRINGBUFF_MENU;

typedef struct {
    DMA_TMODE                   TMOD;
    DMA_Master                  master;
    uint32_t                    source;
    uint32_t                    destination;
} DMA_FULLSIZE_HWMENU;

typedef struct {
    uint32_t                    srcaddr;
    uint32_t                    dstaddr;
} DMA_SWCOPYMENU;

typedef union {
    DMA_HWMENU                  menu_hw;
    DMA_HWRINGBUFF_MENU         menu_hw_ringbuff;
    DMA_SWCOPYMENU              menu_swcopy;
    DMA_FULLSIZE_HWMENU         menu_fullsize_hw;
    DMA_FULLSIZE_HWRINGBUFF_MENU menu_fullsize_hw_ringbuff;
} DMA_MENU;

typedef struct {
    DMA_Type                    type;
    DMA_TranSize                size;
    uint32_t                    count;
    void                        *menu;
    void (*callback)(void);
} DMA_INPUT;

typedef void (* VOID_FUNC)(void);

#define DMA_Start(_n)           DRV_WriteReg32(DMA_START(_n), DMA_START_BIT)
#define DMA_Stop_Now(_n)        DRV_WriteReg32(DMA_START(_n), DMA_STOP_BIT)
#define DMA_GetCount(_n)        DRV_Reg32(DMA_COUNT(_n))
#define DMA_ACKI(_n)            DRV_WriteReg32(DMA_ACKINT(_n), DMA_ACKINT_BIT)
#define DMA_ACKTOI(_n)          DRV_WriteReg32(DMA_ACKINT(_n), DMA_ACKTOINT_BIT)
#define DMA_CheckRunStat(_n)    ( _n > 16 ? ((DRV_Reg32(DMA_GLBSTA1) & DMA_GLBSTA_RUN(_n)) != 0) : ((DRV_Reg32(DMA_GLBSTA0) & DMA_GLBSTA_RUN(_n)) != 0))
#define DMA_CheckITStat(_n)     ( _n > 16 ? ((DRV_Reg32(DMA_GLBSTA1) & DMA_GLBSTA_IT(_n)) != 0) : ((DRV_Reg32(DMA_GLBSTA0) & DMA_GLBSTA_IT(_n)) != 0))
#define DMA_WaitUntilRdy(_n)    while(DMA_CheckRunStat(_n))

#define DMA_Config(dma_no, dma_menu, start)             DMA_Config_Internal(dma_no, dma_menu, false, false, 0, start)
#define DMA_FullSize_Config(dma_no, dma_menu, start)    DMA_Config_Internal(dma_no, dma_menu, true, false, 0, start)
#define DMA_Config_B2W(dma_no, dma_menu, start, b2w)    DMA_Config_Internal(dma_no, dma_menu, false, b2w, 0, start)
#define DMA_Vfifo_Get_Read_Pointer(uart_port)           DRV_Reg32(DMA_RDPTR(UARTPort[uart_port].Rx_DMA_Ch))
#define DMA_Vfifo_Get_Write_Pointer(uart_port)          DRV_Reg32(DMA_WRPTR(UARTPort[uart_port].Rx_DMA_Ch))

/**
 * @brief  Initialize DMA hardware
 *
 * @param  void
 *
 * @return void
 *
 * @note   Initialize the DMA before operations.
 */
void DMA_Init(void);

/**
 * @brief  Stop the DMA channel
 *
 * @param  channel [IN] the number of operating channel
 *
 * @return void
 *
 * @note   Stop the specified DMA channel
 */
void DMA_Stop(uint8_t channel);

/**
 * @brief  Start the DMA channel
 *
 * @param  channel [IN] the number of operating channel
 *
 * @return void
 *
 * @note   Start the specified DMA channel
 */
void DMA_Run(uint8_t channel);

/**
 * @brief  Get the half-size DMA channel
 *
 * @param  DMA_CODE [IN] the peripheral on which for DMA operating
 *
 * @return the channel number of DMA to work with specified peripheral
 *
 * @note   Get a channel of half-sized DMA to perform data movement
 */
uint8_t DMA_GetChannel(DMA_Master DMA_CODE);

/**
 * @brief  Free the half-size DMA channel
 *
 * @param  handle [IN] the number of operating channel
 *
 * @return void
 *
 * @note   Free the resource of the specified DMA channel
 */
void DMA_FreeChannel(uint8_t handle);

/**
 * @brief   full-size DMA channel get valid channel
 *
 * @param  DMA_CODE [IN] the peripheral owns the DMA resource
 *
 * @param  channel [IN] full channel dma number
 *
 * @return channel number
 *
 * @note    full-sized DMA init for performing data movement
 */
uint8_t DMA_FullSize_CheckIdleChannel(DMA_Master DMA_CODE, uint8_t channel);

/**
 * @brief  Free the full-size DMA channel
 *
 * @param  handle [IN] the number of operating channel
 *
 * @return void
 *
 * @note   Free the resource of the specified DMA channel
 */
void DMA_FullSize_FreeChannel(uint8_t handle);

/**
 * @brief  Config the DMA channel
 *
 * @param  dma_no [IN] the number of operating channel
 * @param  dma_menu [IN] the parameters for setting the DMA
 * @param  fullsize [IN] specify if full-size DMA
 * @param  b2w [IN] specify if enable the byte to word function
 * @param  limit [IN] specify if the limitation function is used
 * @param  start [IN] specify if the DMA starts after configured
 *
 * @return void
 *
 * @note   Config the DMA after got a DMA resource
 */
void DMA_Config_Internal(uint8_t    dma_no,
                         DMA_INPUT  *dma_menu,
                         bool   fullsize,
                         bool   b2w, uint8_t limit,
                         bool   start);

/**
 * @brief  Configure the DMA throttle and handshake control
 *
 * @param  handle [IN] the number of operating channel
 * @param  enable [IN] specify if the DRQ function is enabled
 *
 * @return void
 *
 * @note   Set the DMA DRQ function before use
 */
void DMA_DRQ(uint8_t	dma_no,
             uint8_t    enable);

/**
 * @brief  Initialize the DMA VFIFO
 *
 * @param  void
 *
 * @return void
 *
 * @note   Initialize the DMA VFIFO hardware
 */
void DMA_Vfifo_init(void);

/**
 * @brief  Configure the DMA VFIFO
 *
 * @param  adrs [IN] the start address of VFIFO buffer
 * @param  len [IN] the length of VFIFO buffer
 * @param  ch [IN] the VFIFO channel to configure
 * @param  alt_len [IN] the alert length of VFIFO
 * @param  dma_count [IN] the threshold for triggering the interrupt to MCU
 * @param  timeout_counter [IN] the timeout for triggering the interrupt to MCU
 *
 * @return void
 *
 * @note   Configure the DMA VFIFO channel
 */
void DMA_Vfifo_SetAdrs(uint32_t adrs,
                       uint32_t                      len, DMA_VFIFO_CHANNEL ch,
                       uint32_t                      alt_len,
                       uint32_t                      dma_count,
                       uint32_t                      timeout_counter);



void DMA_Vfifo_Set_timeout(DMA_VFIFO_CHANNEL ch, uint32_t timeout_counter);



void DMA_Vfifo_enable_interrupt(DMA_VFIFO_CHANNEL ch);


void DMA_Vfifo_disable_interrupt(DMA_VFIFO_CHANNEL ch);



/**
 * @brief  Flush the DMA VFIFO
 *
 * @param  ch [IN] the VFIFO channel to flush
 *
 * @return void
 *
 * @note   Flush the DMA VFIFO channel
 */
void DMA_Vfifo_Flush(DMA_VFIFO_CHANNEL ch);

/**
 * @brief  Register the callback function for threshold interrupt
 *
 * @param  ch [IN] the interrupt source VFIFO channel
 * @param  callback_func [IN] callback function to handle the interrupt
 *
 * @return void
 *
 * @note   Register the callback function for threshold interrupt
 */
void DMA_Vfifo_Register_Callback(DMA_VFIFO_CHANNEL  ch,
                                 VOID_FUNC          callback_func);

/**
 * @brief  Register the callback function for timeout interrupt
 *
 * @param  ch [IN] the interrupt source VFIFO channel
 * @param  callback_func [IN] callback function to handle the interrupt
 *
 * @return void
 *
 * @note   Register the callback function for timeout interrupt
 */
void DMA_Vfifo_Register_TO_Callback(DMA_VFIFO_CHANNEL ch,
                                    VOID_FUNC         callback_func);

/**
 * @brief  Enable the power and clock for the DMA channel
 *
 * @param  channel [IN] the operating DMA channel
 *
 * @return void
 *
 * @note   Enable the clock of DMA before use
 */
void DMA_Clock_Enable(uint8_t channel);

/**
 * @brief  Disable the power and clock for the DMA channel
 *
 * @param  channel [IN] the operating DMA channel
 *
 * @return void
 *
 * @note   Disable the clock of DMA after use
 */
void DMA_Clock_Disable(uint8_t channel);

/**
 * @brief  Register the callback function for receiving DMA done event
 *
 * @param  dma_ch [IN] the interrupt source DMA channel
 * @param  handler [IN] callback function to handle the interrupt
 *
 * @return void
 *
 * @note   Register the callback function for DMA done event
 */
int DMA_Register(uint8_t dma_ch, VOID_FUNC handler);

/**
 * @brief  De-register the callback function for receiving DMA done event
 *
 * @param  dma_ch [IN] the interrupt source DMA channel
 *
 * @return void
 *
 * @note   De-register the callback function for DMA done event
 */
int DMA_UnRegister(uint8_t dma_ch);

bool DMA_memcpy(const void *src, const void *dst, unsigned int length);

uint8_t dma_set_channel_idle(uint8_t channel);

uint8_t  dma_set_channel_busy(uint8_t channel);



#ifdef __cplusplus
}
#endif

#endif  //#ifdef HAL_GDMA_MODULE_ENABLED

#endif   /*_DMA_SW_H*/

