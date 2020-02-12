/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include "log/tfm_log_raw.h"
#include "uart_stdout.h"

#define PRINT_BUFF_SIZE 32
#define NUM_BUFF_SIZE 12

struct formatted_buffer_t {
    size_t pos;
    uint8_t buf[PRINT_BUFF_SIZE];
};

const char hex_digits_lo[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                              '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
const char hex_digits_up[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                              '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

static void _tfm_flush_formatted_buffer(struct formatted_buffer_t *pb,
                                        uint8_t data)
{
    pb->buf[pb->pos++] = data;
    if (pb->pos >= PRINT_BUFF_SIZE) {
        pb->pos = 0;
        /* uart flush and print here. */
        stdio_output_string(pb->buf, PRINT_BUFF_SIZE);
    }
}

static int _tfm_string_output(struct formatted_buffer_t *pb,
                              const char *str)
{
    int count = 0;

    while (*str) {
        _tfm_flush_formatted_buffer(pb, *str++);
        count++;
    }

    return count;
}

static int _tfm_dec_num_output(struct formatted_buffer_t *pb,
                               int32_t num, uint8_t sign)
{
    int count = 0;
    uint8_t num_buff[NUM_BUFF_SIZE] = {0};
    uint32_t number = (uint32_t)num;
    uint32_t k = 0;

    if (sign == 'd' && num < 0) {
        _tfm_flush_formatted_buffer(pb, '-');
        count++;
        number = -num;
    }

    do {
        num_buff[k++] = '0' + number % 10;
        number /= 10;
    } while (number);

    while (k) {
        _tfm_flush_formatted_buffer(pb, num_buff[--k]);
        count++;
    }

    return count;
}

static int _tfm_hex_num_output(struct formatted_buffer_t *pb, uint32_t num,
                               const char *hex_digits)
{
    int count = 0;
    uint8_t num_buff[NUM_BUFF_SIZE] = {0};
    uint32_t k = 0;

    do {
        num_buff[k++] = hex_digits[num & 0x0f];
        num >>= 4;
    } while (num);

    while (k) {
        _tfm_flush_formatted_buffer(pb, num_buff[--k]);
        count++;
    }

    return count;
}

static int _tfm_log_vprintf(const char *fmt, va_list ap)
{
    int count = 0;
    struct formatted_buffer_t outputbuf;

    outputbuf.pos = 0;

    while (*fmt) {
        if (*fmt == '%') {
            switch (*(++fmt)) {
            case 'd':
            case 'i':
                count += _tfm_dec_num_output(&outputbuf,
                                             va_arg(ap, int32_t), 'd');
                break;
            case 'u':
                count += _tfm_dec_num_output(&outputbuf,
                                             va_arg(ap, int32_t), 'u');
                break;
            case 'x':
                count += _tfm_hex_num_output(&outputbuf, va_arg(ap, uint32_t),
                                             hex_digits_lo);
                break;
            case 'X':
                count += _tfm_hex_num_output(&outputbuf, va_arg(ap, uint32_t),
                                             hex_digits_up);
                break;
            case 'p':
                count += _tfm_string_output(&outputbuf, "0x");
                count += _tfm_hex_num_output(&outputbuf, va_arg(ap, uint32_t),
                                             hex_digits_lo);
                break;
            case 's':
                count += _tfm_string_output(&outputbuf, va_arg(ap, char*));
                break;
            case 'c':
                _tfm_flush_formatted_buffer(&outputbuf,
                                            (uint8_t)va_arg(ap, int32_t));
                count++;
                break;
            case '%':
                _tfm_flush_formatted_buffer(&outputbuf, '%');
                count++;
                break;
            default:
                count += _tfm_string_output(&outputbuf, "[Unsupported Tag]");
                continue;
            }
            fmt++;
        } else {
            _tfm_flush_formatted_buffer(&outputbuf, *fmt++);
            count++;
        }
    }

    /* End of printf, flush buf */
    if (outputbuf.pos) {
        count += stdio_output_string(outputbuf.buf, outputbuf.pos);
    }

    return count;
}

int tfm_log_printf(const char *fmt, ...)
{
    int count = 0;
    va_list ap;

    va_start(ap, fmt);
    count = _tfm_log_vprintf(fmt, ap);
    va_end(ap);

    return count;
}
