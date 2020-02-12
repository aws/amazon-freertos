/*
 * Copyright (c) 2020, Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "uart_stdout.h"

/* TF-M stdio_output_string implementation uses CMSIS UART driver
 * to write string directly to UART. Hook it to putchar instead.*/
int stdio_output_string(const unsigned char *str, uint32_t len)
{
    for (int i=0; i<len; i++)
    {
        putchar(str[i]);
    }
    return len;
}
