/**
 * \file            esp_input.c
 * \brief           Wrapper for passing input data to stack
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
#include "esp/esp_private.h"
#include "esp/esp.h"
#include "esp/esp_input.h"
#include "esp/esp_buff.h"

static uint32_t esp_recv_total_len;
static uint32_t esp_recv_calls;

#if !ESP_CFG_INPUT_USE_PROCESS || __DOXYGEN__

/**
 * \brief           Write data to input buffer
 * \note            \ref ESP_CFG_INPUT_USE_PROCESS must be disabled to use this function
 * \param[in]       data: Pointer to data to write
 * \param[in]       len: Number of data elements in units of bytes
 * \return          Member of \ref espr_t enumeration
 */
espr_t
esp_input(const void* data, size_t len) {
    if (esp.buff.buff == NULL) {
        return espERR;
    }
    esp_buff_write(&esp.buff, data, len);       /* Write data to buffer */
    esp_sys_mbox_putnow(&esp.mbox_process, NULL);   /* Write empty box, don't care if write fails */
    esp_recv_total_len += len;                  /* Update total number of received bytes */
    esp_recv_calls++;                           /* Update number of calls */
    return espOK;
}

#endif /* !ESP_CFG_INPUT_USE_PROCESS || __DOXYGEN__ */

#if ESP_CFG_INPUT_USE_PROCESS || __DOXYGEN__

/**
 * \brief           Process input data directly without writing it to input buffer
 * \note            This function may only be used when in OS mode,
 *                  where single thread is dedicated for input read of AT receive
 * 
 * \note            \ref ESP_CFG_INPUT_USE_PROCESS must be enabled to use this function
 *
 * \param[in]       data: Pointer to received data to be processed
 * \param[in]       len: Length of data to process in units of bytes
 */
espr_t
esp_input_process(const void* data, size_t len) {
    espr_t res = espOK;
    esp_recv_total_len += len;                  /* Update total number of received bytes */
    esp_recv_calls++;                           /* Update number of calls */
    
    if (!esp.status.f.initialized) {
        return espERR;
    }
    if (len) {
        ESP_CORE_PROTECT();                     /* Protect core */
        res = espi_process(data, len);          /* Process input data */
        ESP_CORE_UNPROTECT();                   /* Release core */
    }
    return res;
}

#endif /* ESP_CFG_INPUT_USE_PROCESS || __DOXYGEN__ */
