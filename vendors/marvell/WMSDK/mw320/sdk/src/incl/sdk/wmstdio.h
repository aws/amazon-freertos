/*
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

/** @file wmstdio.h
*
*  @brief This file contains declarations for Standard Input Output functions
*/

#ifndef _WMSTDIO_H_
#define _WMSTDIO_H_

#include <wmtypes.h>
#ifndef __linux__
#include <mdev_uart.h>
#else
#include <stdio.h>
#endif  /* ! __linux__ */

/** Maximum number of characters that can be printed using wmprintf */
#define MAX_MSG_LEN 127

typedef struct stdio_funcs {
	int (*sf_printf)(char *);
	int (*sf_flush)();
	int (*sf_getchar)(uint8_t *);
	int (*sf_putchar)(char *);
} stdio_funcs_t;

extern stdio_funcs_t *c_stdio_funcs;

#ifndef __linux__
/** The printf function
 *
 * This function prints data to the output port. The function prototype is analogous
 * to the printf() function of the C language.
 *
 * \param[in] format The format string
 *
 * \return Number of bytes written to the console.
 */
extern
int wmprintf(const char *format, ...);
#else /* __linux__ */
#define wmprintf printf
#endif /* ! __linux__ */

/** The printf function
 *
 * This function prints character to the output port. This function should be
 * used for printing data character by character.
 *
 * \param[in] ch Pointer to the character to written to the console
 *
 * \return Number of bytes written to the console.
 */
extern
int wmstdio_putchar(char *ch);

/** The low level printf function
 *
 * This function prints data to the output port. This function should be
 * used while printing from interrupts.
 *
 * \param[in] format Pointer to the character to written to the console
 *
 */
extern
void ll_printf(const char *format, ...);

/** Flush Standard I/O
 *
 * This function flushes the send buffer to the output port.
 *
 * @return WM_SUCCESS
 */
extern
uint32_t wmstdio_flush(void);

/**
 * Read one byte
 *
 * \param pu8c Pointer to the location to store the byte read.
 *
 * \return Number of bytes read from console port.
 */
extern
uint32_t wmstdio_getchar(uint8_t *pu8c);
/**
 * Returns port id used for console
 *
 * \param[out] port_id pointer to port_id
 *
 * \return WM_SUCCESS
 */
extern
int wmstdio_getconsole_port(int *port_id);

#ifndef __linux__
/**
 * Initialize the standard input output facility
 *
 * \param[in] uart_port UART Port to be used as the console (e.g. UART0_ID,
 * UART1_ID etc.
 * \param[in] baud Baud rate to be used on the port. 0 implies the default baud
 * rate which is 115200 bps.
 * \return WM_SUCCESS on success, error code otherwise.
 */
extern
int wmstdio_init(UART_ID_Type uart_port, int baud);
#endif /* ! __linux__ */

/* To be used in situations where you HAVE to send data to the console */
uint32_t __console_wmprintf(const char *format, ...);

/* Helper functions to print a float value. Some compilers have a problem
 * interpreting %f
 */

#define wm_int_part_of(x)     ((int)(x))
static inline int wm_frac_part_of(float x, short precision)
{
	int scale = 1;

	while (precision--)
		scale *= 10;

	return (x < 0 ? (int)(((int)x - x) * scale) : (int)((x - (int)x) * scale));
}

#endif
