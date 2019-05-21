/** @file wmstdio.c
*
*  @brief The functions for developer input/output
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

/** wmstdio.c: The functions for developer input/output
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include <wm_os.h>
#include <wmstdio.h>
#include <mdev_uart.h>

static char ll_msg_buf_[MAX_MSG_LEN];
static char wmstdio_msg_buf_[MAX_MSG_LEN];

stdio_funcs_t empty_funcs = {NULL, NULL, NULL, NULL};
stdio_funcs_t *c_stdio_funcs = &empty_funcs;
static os_mutex_t wmstdio_mutex;
mdev_t *serial_console_init(UART_ID_Type uart_port, int baud);
int  serial_console_get_portid(int *port_id);

int vprintf(const char *format, va_list ap)
{
	if (!c_stdio_funcs->sf_printf) {
		/* The stdio facility is not available */
		return 0;
	}

	os_mutex_get(&wmstdio_mutex, OS_WAIT_FOREVER);
	vsnprintf(wmstdio_msg_buf_, MAX_MSG_LEN, &format[0], ap);
	int ret = c_stdio_funcs->sf_printf(wmstdio_msg_buf_);
	os_mutex_put(&wmstdio_mutex);
	return ret;
}

/**
 * Format the string printf style and send it to the output port
 * 
 * @param format 
 * 
 * @return int
 */
int wmprintf(const char *format, ...)
{
	va_list args;
	/* Format the string */
	va_start(args, format);
	uint32_t ret = vprintf(format, args);
	va_end(args);
	return ret;
}

int __wrap_printf(const char *format, ...)
{
	va_list args;
	/* Format the string */
	va_start(args, format);
	uint32_t ret = vprintf(format, args);
	va_end(args);
	return ret;
}

int __wrap_fprintf(void *stream, const char *format, ...)
{
	((void) stream);
	va_list args;
	/* Format the string */
	va_start(args, format);
	uint32_t ret = vprintf(format, args);
	va_end(args);
	return ret;
}

/**
 * char_printf should be used to print debug messages character by character
 *
 * @param ch
 *
 * @return int
 */
int wmstdio_putchar(char *ch_)
{
	uint32_t ret;
	os_mutex_get(&wmstdio_mutex, OS_WAIT_FOREVER);
	ret = c_stdio_funcs->sf_putchar(ch_);
	os_mutex_put(&wmstdio_mutex);
	return ret;
}

/**
 * ll_printf should be used to print debug messages in isr and os debug prints
 *
 * @param format
 *
 * @return uint32_t
 */
void ll_printf(const char *format, ...)
{
	int port_id;
	va_list args;

	/* Format the string */
	va_start(args, format);
	vsnprintf(ll_msg_buf_, MAX_MSG_LEN, &format[0], args);
	va_end(args);
	serial_console_get_portid(&port_id);
	UART_WriteLine(port_id, (uint8_t *)ll_msg_buf_);
}
/**
 * Flush the send buffer to the stdio port
 * 
 * @return uint32_t
 */
uint32_t wmstdio_flush(void)
{
    if (! c_stdio_funcs->sf_flush)
    {
        /* The stdio facility is not available */
        return 1;
    }

    
    return c_stdio_funcs->sf_flush();
}

/**
 * Read one byte
 * 
 * @param pu8c 
 * 
 * @return uint8_t
 */
uint32_t wmstdio_getchar(uint8_t *inbyte_p)
{
    if (! c_stdio_funcs->sf_getchar)
    {
        /* The stdio facility is not available */
        return 0;
    }

    return c_stdio_funcs->sf_getchar(inbyte_p);
}

int wmstdio_getconsole_port(int  *port_id)
{
	int ret = 0;
	ret = serial_console_get_portid(port_id);
	return ret;
}

/**
 * Initialize the standard input output facility
 */

int wmstdio_init(UART_ID_Type uart_port, int baud)
{
	static char init_done;
	int ret;

	if (init_done)
		return WM_SUCCESS;

	uart_drv_init(uart_port, UART_8BIT);

	ret = os_mutex_create(&wmstdio_mutex, "uart-rd",
			OS_MUTEX_INHERIT);

	if (ret != WM_SUCCESS)
		return -WM_FAIL;

	serial_console_init(uart_port, baud);
	init_done = 1;
	return WM_SUCCESS;
}

/* For sending data only to the SERIAL console and nowhere else 
 * Usually for debugging purposes.
 */
int serial_console_printf(char *str);
uint32_t __console_wmprintf(const char *format, ...)
{
	va_list args;
	/* Format the string */
	if (c_stdio_funcs != &empty_funcs) {
		va_start(args, format);
		vsnprintf(wmstdio_msg_buf_, MAX_MSG_LEN, &format[0], args);
		va_end(args);
		return serial_console_printf(wmstdio_msg_buf_);
	} else {
		return 0;
	}
}

