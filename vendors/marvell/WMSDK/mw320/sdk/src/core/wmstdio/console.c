/** @file console.c
*
*  @brief Functions for mapping wmstdio to the serial console
*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
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
/** console.c: Functions for mapping wmstdio to the serial console.
 */
#include <string.h>

#include <wmstdio.h>
#include <mdev_uart.h>

static mdev_t * mdev_uart_ = NULL;

stdio_funcs_t serial_console_funcs;

mdev_t *serial_console_init(UART_ID_Type uart_port, int baud)
{
	mdev_uart_ = uart_drv_open(uart_port, baud);
	c_stdio_funcs = &serial_console_funcs;
	return mdev_uart_;
}

int serial_console_get_portid(int *port_id)
{
	*port_id = uart_drv_get_portid(mdev_uart_);
	return 0;
}

int serial_console_printf(char *str)
{
	int len;

	if (! mdev_uart_)
		return 0;

	len = strlen(str);
	uart_drv_write(mdev_uart_, (uint8_t *)str, len);
	return len;
}

int serial_console_flush()
{
	uart_drv_tx_flush(mdev_uart_);
	return 0;
}

int serial_console_getchar(uint8_t *inbyte_p)
{
	return uart_drv_read(mdev_uart_, inbyte_p, 1);
}

int serial_console_putchar(char *inbyte_p)
{
	if (!mdev_uart_)
		return 0;
	return uart_drv_write(mdev_uart_, (uint8_t *)inbyte_p, 1);
}

stdio_funcs_t serial_console_funcs = { 
	serial_console_printf, 
	serial_console_flush,
	serial_console_getchar,
	serial_console_putchar,
};
