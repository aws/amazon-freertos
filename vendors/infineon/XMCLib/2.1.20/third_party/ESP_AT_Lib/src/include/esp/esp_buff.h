/**	
 * \file            esp_buff.h
 * \brief           Buffer manager
 */
 
/*
 * Copyright (c) 2018 Tilen Majerle
 *  
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of ESP-AT.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#ifndef __ESP_BUFF_H
#define __ESP_BUFF_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "esp/esp.h"

/**
 * \ingroup         ESP
 * \defgroup        ESP_BUFF Ring buffer
 * \brief           Generic ring buffer
 * \{
 */

uint8_t     esp_buff_init(esp_buff_t* buff, size_t len);
void        esp_buff_free(esp_buff_t* buff);
size_t      esp_buff_write(esp_buff_t* buff, const void* data, size_t count);
size_t      esp_buff_read(esp_buff_t* buff, void* data, size_t count);
size_t      esp_buff_get_free(esp_buff_t* buff);
size_t      esp_buff_get_full(esp_buff_t* buff);
void        esp_buff_reset(esp_buff_t* buff);
size_t      esp_buff_peek(esp_buff_t* buff, size_t skip_count, void* data, size_t count);
void *      esp_buff_get_linear_block_address(esp_buff_t* buff);
size_t      esp_buff_get_linear_block_length(esp_buff_t* buff);
size_t      esp_buff_skip(esp_buff_t* buff, size_t len);

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* __ESP_BUFF_H */
