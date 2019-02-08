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
 * @file uart.h
 *
 *  This header file exposes the API for UART.
 *
 */

#ifndef __UART_H__
#define __UART_H__

#include "stdint.h"
#include "hal_pdma_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UART_DMA_MAX_SETTING_VALUE (2<<16)

typedef enum {
    UART_INTERRUPT_NONE,
    UART_INTERRUPT_RECEIVE_ERROR,
    UART_INTERRUPT_RECEIVE_TIMEOUT,
    UART_INTERRUPT_RECEIVE_BREAK,
    UART_INTERRUPT_RECEIVE_AVAILABLE,
    UART_INTERRUPT_SEND_AVAILABLE,
    UART_INTERRUPT_SOFTWARE_FLOWCONTROL,
    UART_INTERRUPT_HARDWARE_FLOWCONTROL
} uart_interrupt_type_t;

typedef struct {
    bool is_rx;
    hal_uart_port_t uart_port;
} uart_dma_callback_data_t;

int getc_(void);
int getc_nowait(void);
void uart_set_baudrate(hal_uart_port_t uart_port, uint32_t baudrate);
vdma_channel_t uart_port_to_dma_channel(hal_uart_port_t uart_port, int32_t is_rx);
void uart_dma_channel_to_callback_data(vdma_channel_t dma_channel, uart_dma_callback_data_t *user_data);
void uart_enable_dma(hal_uart_port_t u_port);
void uart_interrupt_handler(hal_nvic_irq_t irq_number);
void uart_put_char_block(hal_uart_port_t u_port, unsigned char c);
uint8_t uart_get_char_block(hal_uart_port_t u_port);
void uart_set_format(hal_uart_port_t u_port,
                     hal_uart_word_length_t word_length,
                     hal_uart_stop_bit_t stop_bit,
                     hal_uart_parity_t parity);
void uart_set_hardware_flowcontrol(hal_uart_port_t u_port);
void uart_set_software_flowcontrol(hal_uart_port_t u_port, uint8_t xon, uint8_t xoff, uint8_t escape_character);
void uart_disable_flowcontrol(hal_uart_port_t u_port);
void uart_set_fifo(hal_uart_port_t u_port);

#ifdef HAL_SLEEP_MANAGER_ENABLED
void uart_set_sleep_mode(hal_uart_port_t u_port);
void uart_unmask_send_interrupt(hal_uart_port_t u_port);
#endif

void uart_unmask_receive_interrupt(hal_uart_port_t u_port);
void uart_purge_fifo(hal_uart_port_t u_port, int32_t is_rx);
uart_interrupt_type_t uart_query_interrupt_type(hal_uart_port_t u_port);
int32_t uart_verify_error(hal_uart_port_t u_port);
void uart_reset_default_value(hal_uart_port_t u_port);
void uart_query_empty(hal_uart_port_t u_port);

void uart_receive_handler(hal_uart_port_t uart_port, bool is_timeout);
void uart_send_handler(hal_uart_port_t uart_port, bool is_send_complete_trigger);
void uart_error_handler(hal_uart_port_t uart_port);

#ifdef __cplusplus
}
#endif

#endif
