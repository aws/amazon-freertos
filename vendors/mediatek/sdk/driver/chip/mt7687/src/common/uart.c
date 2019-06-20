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

#include "hal_uart.h"

#ifdef HAL_UART_MODULE_ENABLED

#include "uart.h"
#include "uart_hw.h"
#include "mem_util.h"
#include "timer.h"
#include "hal_nvic.h"

#include "top.h"

#include "type_def.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t gUartClkFreq;
const uint32_t g_uart_regbase[] = {CM4_UART1_BASE, CM4_UART2_BASE};
const hal_nvic_irq_t g_uart_port_to_irq_num[] = {CM4_UART1_IRQ, CM4_UART2_IRQ};
#ifdef HAL_SLEEP_MANAGER_ENABLED
const char *const g_uart_sleep_handler_name[] = {"uart0", "uart1"};
#endif

/*****************************/
/* Internal used function */
/*****************************/
/*
 *     get character from SW FIFO (interrupt based)
 *     (ROM code shall call getc_nowait() instead of this function)
 */
int getc_(void)
{
    //int rc = rb_get_and_zap();
    int rc = getc_nowait();
    return rc;
}

/*****************************/

/*
 *    get character from HW directly
 */
int getc_nowait(void)
{
    char c;

    if (HAL_REG_32(CONSOLE + UART_LSR) & 0x1) { //  DataReady bit
        c = HAL_REG_32(CONSOLE + UART_RBR);
        return c;
    } else {
        return -1;
    }
}

void uart_set_baudrate(hal_uart_port_t uart_port, uint32_t baudrate)
{
    uint32_t UART_BASE = g_uart_regbase[uart_port];
    uint32_t data, uart_lcr, high_speed_div, sample_count, sample_point, fraction;
    uint16_t  fraction_L_mapping[] = {0x00, 0x10, 0x44, 0x92, 0x29, 0xaa, 0xb6, 0xdb, 0xad, 0xff, 0xff};
    uint16_t  fraction_M_mapping[] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03};
    gUartClkFreq = top_xtal_freq_get();

    // 100, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600
    HAL_REG_32(UART_BASE + UART_RATE_STEP) = 0x3; // based on sampe_count * baud_pulse, baud_rate = system clock frequency / sampe_count
    uart_lcr = HAL_REG_32(UART_BASE + UART_LCR);     /* DLAB start */
    HAL_REG_32(UART_BASE + UART_LCR) = (uart_lcr | UART_LCR_DLAB);
    data = gUartClkFreq / baudrate;
    high_speed_div = (data >> 8) + 1; // divided by 256
    {
        sample_count = data / high_speed_div - 1;
        sample_point = (sample_count + 1) / 2 - 2;
    }
    fraction = (gUartClkFreq * 10 / baudrate / data - (sample_count + 1) * 10) % 10;
    HAL_REG_32(UART_BASE + UART_DLL) = (high_speed_div & 0x00ff);
    HAL_REG_32(UART_BASE + UART_DLH) = ((high_speed_div >> 8) & 0x00ff);
    HAL_REG_32(UART_BASE + UART_STEP_COUNT) = sample_count;
    HAL_REG_32(UART_BASE + UART_SAMPLE_COUNT) = sample_point;
    if (baudrate == 5000000) {
        HAL_REG_32(UART_BASE + UART_FRACDIV_M) = 0x0;
        HAL_REG_32(UART_BASE + UART_FRACDIV_L) = 0x0;
    } else {
    HAL_REG_32(UART_BASE + UART_FRACDIV_M) = fraction_M_mapping[fraction];
    HAL_REG_32(UART_BASE + UART_FRACDIV_L) = fraction_L_mapping[fraction];
    }

    if (baudrate >= 3000000) {
        HAL_REG_32(UART_BASE + UART_GUARD) = 0x12;    /* delay 2 bits per byte. */
    }

    HAL_REG_32(UART_BASE + UART_LCR) = (uart_lcr);   /* DLAB end */
}

vdma_channel_t uart_port_to_dma_channel(hal_uart_port_t uart_port, int32_t is_rx)
{
    vdma_channel_t dma_channel = VDMA_UART0RX;

    switch (uart_port) {
        case HAL_UART_0:
            if (is_rx) {
                dma_channel = VDMA_UART0RX;
            } else {
                dma_channel = VDMA_UART0TX;
            }
            break;
        case HAL_UART_1:
            if (is_rx) {
                dma_channel = VDMA_UART1RX;
            } else {
                dma_channel = VDMA_UART1TX;
            }
            break;
        default:
            break;
    }

    return dma_channel;
}

void uart_dma_channel_to_callback_data(vdma_channel_t dma_channel, uart_dma_callback_data_t *user_data)
{
    switch (dma_channel) {
        case VDMA_UART0RX:
            user_data->is_rx = true;
            user_data->uart_port = HAL_UART_0;
            break;
        case VDMA_UART1RX:
            user_data->is_rx = true;
            user_data->uart_port = HAL_UART_1;
            break;
        case VDMA_UART0TX:
            user_data->is_rx = false;
            user_data->uart_port = HAL_UART_0;
            break;
        case VDMA_UART1TX:
            user_data->is_rx = false;
            user_data->uart_port = HAL_UART_1;
            break;
        default:
            break;
    }
}

void uart_enable_dma(hal_uart_port_t u_port)
{
    uint32_t base = g_uart_regbase[u_port];
    HAL_REG_32(base + UART_VFIFO_EN_REG) = 0x1;
}

void uart_interrupt_handler(hal_nvic_irq_t irq_number)
{
    hal_uart_port_t uart_port;
    uart_interrupt_type_t type;
    uint32_t value;
    uint32_t base;

    switch (irq_number) {
        case CM4_UART1_IRQ:
            uart_port = HAL_UART_0;
            break;
        case CM4_UART2_IRQ:
            uart_port = HAL_UART_1;
            break;
        default:
            uart_port = HAL_UART_0;
    }

    type = uart_query_interrupt_type(uart_port);

    switch (type) {
        /* received data and timeout happen */
    case UART_INTERRUPT_RECEIVE_TIMEOUT:
            /*clear Rx timeout*/
            base = g_uart_regbase[uart_port];
            value = HAL_REG_32(base + UART_VFIFO_EN_REG);
            value = value;
            uart_receive_handler(uart_port, true);
            break;
        /* receive line status changed Any of BI/FE/PE/OE becomes set */
        case UART_INTERRUPT_RECEIVE_ERROR:
            uart_error_handler(uart_port);
            break;
        /* received data or received Trigger level reached */
        case UART_INTERRUPT_RECEIVE_AVAILABLE:
            //uart_receive_handler(uart_port, false);
            break;
        /* false interrupt detect */
        case UART_INTERRUPT_NONE:
            break;
        /* received break signal */
        case UART_INTERRUPT_RECEIVE_BREAK:
            uart_purge_fifo(uart_port, 1);
            break;
        /* TX holding register is empty or the TX FIFO reduce to it's trigger level */
        case UART_INTERRUPT_SEND_AVAILABLE:
#ifdef HAL_SLEEP_MANAGER_ENABLED
            uart_send_handler(uart_port, true);
            break;
#endif
        /* detect hardware flow control request (CTS is high) */
        case UART_INTERRUPT_HARDWARE_FLOWCONTROL:
        /* an XOFF character has been received */
        case UART_INTERRUPT_SOFTWARE_FLOWCONTROL:
        default:
            break;
    }
}

/* MT7687 CM4 */
void uart_put_char_block(hal_uart_port_t u_port, unsigned char c)
{
    uint32_t base = g_uart_regbase[u_port];

    while (!(HAL_REG_32(base + UART_LSR) & 0x20 /* THRE bit */))
        ;
    HAL_REG_32(base + UART_RBR) = c;

    return;
}


/* MT7687 CM4 */
uint8_t uart_get_char_block(hal_uart_port_t u_port)
{
    uint32_t base = g_uart_regbase[u_port];
    char         c;

    while (!(HAL_REG_32(base + UART_LSR) & 0x1)) //  DataReady bit
        ;

    c = HAL_REG_32(base + UART_RBR);

    return c;
}

void uart_set_format(hal_uart_port_t u_port,
                     hal_uart_word_length_t word_length,
                     hal_uart_stop_bit_t stop_bit,
                     hal_uart_parity_t parity)
{
    uint32_t base = g_uart_regbase[u_port];
    uint16_t byte;

    /* DLAB start */
    byte = HAL_REG_32(base + UART_LCR);

    /* Setup wordlength */
    byte &= ~UART_DATA_MASK;
    byte |= (uint16_t)word_length;

    /* setup stop bit */
    byte &= ~UART_STOP_MASK;
    byte |= (uint16_t)(stop_bit << 2);

    /* setup parity bit */
    byte &= ~UART_PARITY_MASK;
    byte |= (uint16_t)(parity << 3);

    /* DLAB End */
    HAL_REG_32(base + UART_LCR) = byte;
}

void uart_set_hardware_flowcontrol(hal_uart_port_t u_port)
{
    uint32_t base = g_uart_regbase[u_port];
    uint16_t EFR, LCR;

    LCR = HAL_REG_32(base + UART_LCR);

    HAL_REG_32(base + UART_LCR) = 0xbf;
    EFR = HAL_REG_32(base + UART_EFR);
    EFR |= 0x00d0;
    HAL_REG_32(base + UART_EFR) = EFR;

    HAL_REG_32(base + UART_ESCAPE_EN) = 0;

    HAL_REG_32(base + UART_LCR) = 0x00;
    HAL_REG_32(base + UART_MCR) = 0x02;

    HAL_REG_32(base + UART_LCR) = LCR;
}

void uart_set_software_flowcontrol(hal_uart_port_t u_port, uint8_t xon, uint8_t xoff, uint8_t escape_character)
{
    uint32_t base = g_uart_regbase[u_port];
    uint16_t EFR, LCR;

    LCR = HAL_REG_32(base + UART_LCR);

    HAL_REG_32(base + UART_LCR) = 0xbf;
    HAL_REG_32(base + UART_XON1) = xon;
    HAL_REG_32(base + UART_XON2) = xon;
    HAL_REG_32(base + UART_XOFF1) = xoff;
    HAL_REG_32(base + UART_XOFF2) = xoff;

    EFR = HAL_REG_32(base + UART_EFR);
    EFR |= 0x000A;
    HAL_REG_32(base + UART_EFR) = EFR;

    HAL_REG_32(base + UART_LCR) = LCR;

    HAL_REG_32(base + UART_ESCAPE_DATA) = escape_character;
    HAL_REG_32(base + UART_ESCAPE_EN) = 1;
}

void uart_disable_flowcontrol(hal_uart_port_t u_port)
{
    uint32_t base = g_uart_regbase[u_port];
    uint16_t LCR;

    LCR = HAL_REG_32(base + UART_LCR);

    HAL_REG_32(base + UART_LCR) = 0xbf;

    HAL_REG_32(base + UART_EFR) = 0x0;

    HAL_REG_32(base + UART_LCR) = 0x00;

    HAL_REG_32(base + UART_LCR) = LCR;
}

void uart_set_fifo(hal_uart_port_t u_port)
{
    uint32_t base = g_uart_regbase[u_port];
    uint16_t LCR, EFR;

    LCR = HAL_REG_32(base + UART_LCR);

    HAL_REG_32(base + UART_LCR) = 0xBF;
    EFR = HAL_REG_32(base + UART_EFR);
    EFR |= 0x10;
    HAL_REG_32(base + UART_EFR) = EFR;

    HAL_REG_32(base + UART_LCR) = 0x00;
    HAL_REG_32(base + UART_FCR) = 0x87;

    HAL_REG_32(base + UART_LCR) = LCR;
}
#ifdef HAL_SLEEP_MANAGER_ENABLED
void uart_set_sleep_mode(hal_uart_port_t u_port)
{
    uint32_t base = g_uart_regbase[u_port];

    HAL_REG_32(base + UART_SLEEP_EN) = 0x01;
}

void uart_unmask_send_interrupt(hal_uart_port_t u_port)
{
    uint32_t base = g_uart_regbase[u_port];
    uint16_t IER, LCR;

    LCR = HAL_REG_32(base + UART_LCR);
    HAL_REG_32(base + UART_LCR) = 0x00;

    IER = HAL_REG_32(base + UART_IER);
    IER |= 0x02;
    HAL_REG_32(base + UART_IER) = IER;

    HAL_REG_32(base + UART_LCR) = LCR;
}
#endif

void uart_unmask_receive_interrupt(hal_uart_port_t u_port)
{
    uint32_t base = g_uart_regbase[u_port];
    uint16_t IER, LCR;

    LCR = HAL_REG_32(base + UART_LCR);
    HAL_REG_32(base + UART_LCR) = 0x00;

    IER = HAL_REG_32(base + UART_IER);
    IER |= 0x05;
    HAL_REG_32(base + UART_IER) = IER;

    HAL_REG_32(base + UART_LCR) = LCR;
}

void uart_purge_fifo(hal_uart_port_t u_port, int32_t is_rx)
{
    uint32_t base = g_uart_regbase[u_port];
    uint16_t FCR = 0x81;

    if (is_rx) {
        FCR |= 0x02;
    } else {
        FCR |= 0x04;
    }

    HAL_REG_32(base + UART_FCR) = FCR;
}

uart_interrupt_type_t uart_query_interrupt_type(hal_uart_port_t u_port)
{
    uint32_t base = g_uart_regbase[u_port];
    uint16_t IIR, LSR;
    uart_interrupt_type_t type = UART_INTERRUPT_NONE;

    IIR = HAL_REG_32(base + UART_IIR);
    if (IIR & 0x1) {
        return type;
    }

    switch (IIR & 0x3F) {
        /* received data and timeout happen */
        case 0x0c:
            type = UART_INTERRUPT_RECEIVE_TIMEOUT;
            break;
        /* receive line status changed Any of BI/FE/PE/OE becomes set */
        case 0x06:
            LSR = HAL_REG_32(base + UART_LSR);
            if (LSR & 0x10) {
                type = UART_INTERRUPT_RECEIVE_BREAK;
            } else {
                type = UART_INTERRUPT_RECEIVE_ERROR;
            }
            break;
        /* TX Holding Register Empty */
        case 0x02:
            type = UART_INTERRUPT_SEND_AVAILABLE;
            break;
        default:
            break;
    }

    return type;
}

int32_t uart_verify_error(hal_uart_port_t u_port)
{
    uint32_t base = g_uart_regbase[u_port];
    uint16_t LSR;
    int32_t ret = 0;

    LSR = HAL_REG_32(base + UART_LSR);
    if (!(LSR & 0x0e)) {
        ret = -1;
    }

    return ret;
}

void uart_reset_default_value(hal_uart_port_t u_port)
{
    unsigned int base = g_uart_regbase[u_port];

    HAL_REG_32(base + UART_LCR) = 0xbf;
    HAL_REG_32(base + UART_EFR) = 0x10;
    HAL_REG_32(base + UART_XON1) = 0x00;
    HAL_REG_32(base + UART_XOFF1) = 0x00;

    HAL_REG_32(base + UART_LCR) = 0x80;
    HAL_REG_32(base + UART_DLL) = 0x00;
    HAL_REG_32(base + UART_DLH) = 0x00;

    HAL_REG_32(base + UART_LCR) = 0x00;
    HAL_REG_32(base + UART_IER) = 0x00;
    HAL_REG_32(base + UART_FCR) = 0x00;

    HAL_REG_32(base + UART_LCR) = 0xbf;
    HAL_REG_32(base + UART_EFR) = 0x00;
    HAL_REG_32(base + UART_LCR) = 0x00;

    HAL_REG_32(base + UART_MCR) = 0x00;
    HAL_REG_32(base + UART_SCR) = 0x00;
    HAL_REG_32(base + UART_RATE_STEP) = 0x00;
    HAL_REG_32(base + UART_STEP_COUNT) = 0x00;
    HAL_REG_32(base + UART_SAMPLE_COUNT) = 0x00;
    HAL_REG_32(base + UART_RATE_FIX_REG) = 0x00;
    HAL_REG_32(base + UART_GUARD) = 0x00;
    HAL_REG_32(base + UART_ESCAPE_DATA) = 0x00;
    HAL_REG_32(base + UART_ESCAPE_EN) = 0x00;
    HAL_REG_32(base + UART_SLEEP_EN) = 0x00;
    HAL_REG_32(base + UART_VFIFO_EN_REG) = 0x00;
    HAL_REG_32(base + UART_RX_TRIGGER_ADDR) = 0x00;
    HAL_REG_32(base + UART_FRACDIV_L) = 0x00;
    HAL_REG_32(base + UART_FRACDIV_M) = 0x00;
    HAL_REG_32(base + UART_TX_ACTIVE_EN) = 0x00;
}

void uart_query_empty(hal_uart_port_t u_port)
{
    unsigned int base = g_uart_regbase[u_port];

    while (!(HAL_REG_32(base + UART_LSR) & 0x40));
}

#ifdef __cplusplus
}
#endif

#endif

