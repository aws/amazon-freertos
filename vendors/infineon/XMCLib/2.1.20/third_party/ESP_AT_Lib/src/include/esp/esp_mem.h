/**	
 * \file            esp_mem.h
 * \brief           Memory manager
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
#ifndef __ESP_MEM_H
#define __ESP_MEM_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "esp/esp.h"

/**
 * \ingroup         ESP
 * \defgroup        ESP_MEM Memory manager
 * \brief           Dynamic memory manager
 * \{
 */

/**
 * \brief           Single memory region descriptor
 */
typedef struct esp_mem_region {
    void* start_addr;                           /*!< Start address of region */
    size_t size;                                /*!< Size in units of bytes of region */
} esp_mem_region_t;

void*   esp_mem_alloc(uint32_t size);
void*   esp_mem_realloc(void* ptr, size_t size);
void*   esp_mem_calloc(size_t num, size_t size);
void    esp_mem_free(void* ptr);
size_t  esp_mem_getfree(void);
size_t  esp_mem_getfull(void);
size_t  esp_mem_getminfree(void);

uint8_t esp_mem_assignmemory(const esp_mem_region_t* regions, size_t size);
    
/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ESP_MEM_H */
