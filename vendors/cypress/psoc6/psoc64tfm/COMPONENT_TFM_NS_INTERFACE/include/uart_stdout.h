/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __UART_STDOUT_H__
#define __UART_STDOUT_H__

#include <stdint.h>

/**
 * \brief Output buffer by STDIO.
 */
int stdio_output_string(const unsigned char *str, uint32_t len);

#endif /* __UART_STDOUT_H__ */
