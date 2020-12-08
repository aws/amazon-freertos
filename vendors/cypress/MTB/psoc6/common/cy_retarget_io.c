/***************************************************************************//**
* \file cy_retarget_io.c
*
* \brief
* Provides APIs for retargeting stdio to UART hardware contained on the Cypress
* kits.
*
********************************************************************************
* \copyright
* Copyright 2018-2019 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include "cy_retarget_io.h"
#include "cyhal_hw_types.h"
#include "cyhal_uart.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* UART HAL object used by BSP for Debug UART port */
cyhal_uart_t cy_retarget_io_uart_obj;

/* Tracks the previous character sent to output stream */
#ifdef CY_RETARGET_IO_CONVERT_LF_TO_CRLF
static char cy_retarget_io_stdout_prev_char = 0;
#endif /* CY_RETARGET_IO_CONVERT_LF_TO_CRLF */

static uint8_t cy_retarget_io_getchar(void)
{
    uint8 c = 0;
    cyhal_uart_getc(&cy_retarget_io_uart_obj, &c, 0);
    return c;
}

static void cy_retarget_io_putchar(char c)
{
    cyhal_uart_putc(&cy_retarget_io_uart_obj, c);
}

#if defined(__ARMCC_VERSION) /* ARM-MDK */
    /***************************************************************************
    * Function Name: fputc
    ***************************************************************************/
    __attribute__((weak)) int fputc(int ch, FILE *f)
    {
        (void)f;
    #ifdef CY_RETARGET_IO_CONVERT_LF_TO_CRLF
        if ((char)ch == '\n' && cy_retarget_io_stdout_prev_char != '\r')
        {
            cy_retarget_io_putchar('\r');
        }

        cy_retarget_io_stdout_prev_char = (char)ch;
    #endif /* CY_RETARGET_IO_CONVERT_LF_TO_CRLF */
        cy_retarget_io_putchar(ch);
        return (ch);
    }
#elif defined (__ICCARM__) /* IAR */
    #include <yfuns.h>

    /***************************************************************************
    * Function Name: __write
    ***************************************************************************/
    __weak size_t __write(int handle, const unsigned char * buffer, size_t size)
    {
        size_t nChars = 0;
        /* This template only writes to "standard out", for all other file
        * handles it returns failure. */
        if (handle != _LLIO_STDOUT)
        {
            return (_LLIO_ERROR);
        }
        if (buffer != NULL)
        {
            for (/* Empty */; nChars < size; ++nChars)
            {
            #ifdef CY_RETARGET_IO_CONVERT_LF_TO_CRLF
                if (*buffer == '\n' && cy_retarget_io_stdout_prev_char != '\r')
                {
                    cy_retarget_io_putchar('\r');
                }

                cy_retarget_io_stdout_prev_char = *buffer;
            #endif /* CY_RETARGET_IO_CONVERT_LF_TO_CRLF */
                cy_retarget_io_putchar(*buffer);
                ++buffer;
            }
        }
        return (nChars);
    }
#else /* (__GNUC__)  GCC */
    /* Add an explicit reference to the floating point printf library to allow
    the usage of floating point conversion specifier. */
    __asm (".global _printf_float");
    /***************************************************************************
    * Function Name: _write
    ***************************************************************************/
    __attribute__((weak)) int _write (int fd, const char *ptr, int len)
    {
        int nChars = 0;
        (void)fd;
        if (ptr != NULL)
        {
            for (/* Empty */; nChars < len; ++nChars)
            {
            #ifdef CY_RETARGET_IO_CONVERT_LF_TO_CRLF
                if (*ptr == '\n' && cy_retarget_io_stdout_prev_char != '\r')
                {
                    cy_retarget_io_putchar('\r');
                }

                cy_retarget_io_stdout_prev_char = *ptr;
            #endif /* CY_RETARGET_IO_CONVERT_LF_TO_CRLF */
                cy_retarget_io_putchar((uint32_t)*ptr);
                ++ptr;
            }
        }
        return (nChars);
    }
#endif


#if defined(__ARMCC_VERSION) /* ARM-MDK */
    /***************************************************************************
    * Function Name: fgetc
    ***************************************************************************/
    __attribute__((weak)) int fgetc(FILE *f)
    {
        (void)f;
        return (cy_retarget_io_getchar());
    }
#elif defined (__ICCARM__) /* IAR */
    __weak size_t __read(int handle, unsigned char * buffer, size_t size)
    {
        /* This template only reads from "standard in", for all other file
        handles it returns failure. */
        if ((handle != _LLIO_STDIN) || (buffer == NULL))
        {
            return (_LLIO_ERROR);
        }
        else
        {
            *buffer = cy_retarget_io_getchar();
            return (1);
        }
    }
#else /* (__GNUC__)  GCC */
    /* Add an explicit reference to the floating point scanf library to allow
    the usage of floating point conversion specifier. */
    __asm (".global _scanf_float");
    __attribute__((weak)) int _read (int fd, char *ptr, int len)
    {
        int nChars = 0;
        (void)fd;
        if (ptr != NULL)
        {
            for(/* Empty */;nChars < len;++ptr)
            {
                *ptr = (char)cy_retarget_io_getchar();
                ++nChars;
                if((*ptr == '\n') || (*ptr == '\r'))
                {
                    break;
                }
            }
        }
        return (nChars);
    }
#endif

cy_rslt_t cy_retarget_io_init(cyhal_gpio_t tx, cyhal_gpio_t rx, uint32_t baudrate)
{
    const cyhal_uart_cfg_t uart_config =
    {
        .data_bits = 8,
        .stop_bits = 1,
        .parity = CYHAL_UART_PARITY_NONE,
        .rx_buffer = NULL,
        .rx_buffer_size = 0,
    };

    cy_rslt_t result = cyhal_uart_init(&cy_retarget_io_uart_obj, tx, rx, NULL, &uart_config);

    if (result == CY_RSLT_SUCCESS)
    {
        result = cyhal_uart_set_baud(&cy_retarget_io_uart_obj, baudrate, NULL);
    }

    return result;
}

void cy_retarget_io_deinit()
{
    cyhal_uart_free(&cy_retarget_io_uart_obj);
}

#if defined(__cplusplus)
}
#endif
