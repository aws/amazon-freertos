/*
 * The Clear BSD License
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __FSL_USART_FREERTOS_H__
#define __FSL_USART_FREERTOS_H__

#include "fsl_usart.h"
#include <FreeRTOS.h>
#include <event_groups.h>
#include <semphr.h>

/*!
 * @addtogroup usart_freertos_driver
 * @{
 */

/*! @file*/

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief USART freertos driver version 2.0.1. */
#define FSL_USART_FREERTOS_DRIVER_VERSION (MAKE_VERSION(2, 0, 1))
/*@}*/

/*! @brief FLEX USART configuration structure */
struct rtos_usart_config
{
    USART_Type *base;                /*!< USART base address */
    uint32_t srcclk;                 /*!< USART source clock in Hz*/
    uint32_t baudrate;               /*!< Desired communication speed */
    usart_parity_mode_t parity;      /*!< Parity setting */
    usart_stop_bit_count_t stopbits; /*!< Number of stop bits to use */
    uint8_t *buffer;                 /*!< Buffer for background reception */
    uint32_t buffer_size;            /*!< Size of buffer for background reception */
};

/*! @brief FLEX USART FreeRTOS handle */
typedef struct _usart_rtos_handle
{
    USART_Type *base;              /*!< USART base address */
    usart_transfer_t txTransfer;   /*!< TX transfer structure */
    usart_transfer_t rxTransfer;   /*!< RX transfer structure */
    SemaphoreHandle_t rxSemaphore; /*!< RX semaphore for resource sharing */
    SemaphoreHandle_t txSemaphore; /*!< TX semaphore for resource sharing */
#define RTOS_USART_COMPLETE 0x1
#define RTOS_USART_RING_BUFFER_OVERRUN 0x2
#define RTOS_USART_HARDWARE_BUFFER_OVERRUN 0x4
    EventGroupHandle_t rxEvent; /*!< RX completion event */
    EventGroupHandle_t txEvent; /*!< TX completion event */
    void *t_state;              /*!< Transactional state of the underlying driver */
} usart_rtos_handle_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name USART RTOS Operation
 * @{
 */

/*!
 * @brief Initializes a USART instance for operation in RTOS.
 *
 * @param handle The RTOS USART handle, the pointer to allocated space for RTOS context.
 * @param t_handle The pointer to allocated space where to store transactional layer internal state.
 * @param cfg The pointer to the parameters required to configure the USART after initialization.
 * @return 0 succeed, others fail.
 */
int USART_RTOS_Init(usart_rtos_handle_t *handle, usart_handle_t *t_handle, const struct rtos_usart_config *cfg);

/*!
 * @brief Deinitializes a USART instance for operation.
 *
 * This function deinitializes the USART module, sets all register values to reset value,
 * and releases the resources.
 *
 * @param handle The RTOS USART handle.
 */
int USART_RTOS_Deinit(usart_rtos_handle_t *handle);

/*!
 * @name USART transactional Operation
 * @{
 */

/*!
 * @brief Sends data in the background.
 *
 * This function sends data. It is a synchronous API.
 * If the hardware buffer is full, the task is in the blocked state.
 *
 * @param handle The RTOS USART handle.
 * @param buffer The pointer to buffer to send.
 * @param length The number of bytes to send.
 */
int USART_RTOS_Send(usart_rtos_handle_t *handle, const uint8_t *buffer, uint32_t length);

/*!
 * @brief Receives data.
 *
 * This function receives data from USART. It is a synchronous API. If data is immediately available,
 * it is returned immediately and the number of bytes received.
 *
 * @param handle The RTOS USART handle.
 * @param buffer The pointer to buffer where to write received data.
 * @param length The number of bytes to receive.
 * @param received The pointer to a variable of size_t where the number of received data is filled.
 */
int USART_RTOS_Receive(usart_rtos_handle_t *handle, uint8_t *buffer, uint32_t length, size_t *received);

/* @} */

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* __FSL_USART_FREERTOS_H__ */
