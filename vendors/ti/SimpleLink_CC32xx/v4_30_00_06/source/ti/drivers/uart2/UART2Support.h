/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file       UART2Support.h
 *
 *  @brief      Holder of common helper functions for the UART driver
 *
 *  ============================================================================
 */

#ifndef ti_drivers_UART2Support__include
#define ti_drivers_UART2Support__include

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/UART2.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @brief  Function to disable the receive and receive interrupts
 *
 *  @param[in]  hwAttrs    A pointer to a UART2_HWAttrs structure
 */
extern void UART2Support_disableRx(UART2_HWAttrs const *hwAttrs);

/*!
 *  @brief  Function to disable the transmit interrupt
 *
 *  @param[in]  hwAttrs    A pointer to a UART2_HWAttrs structure
 */
extern void UART2Support_disableTx(UART2_HWAttrs const *hwAttrs);

/*!
 *  @brief  Function to update RX ring buffer state
 *
 *  @param[in]  handle    A UART2_Handle returned from UART2_open()
 */
extern void UART2Support_dmaRefreshRx(UART2_Handle handle);

/*!
 *  @brief  Function to configure the receive DMA
 *
 *  @param[in]  handle    A UART2_Handle returned from UART2_open()
 */
extern void UART2Support_dmaStartRx(UART2_Handle handle);

/*!
 *  @brief  Function to configure the tramsit DMA
 *
 *  @param[in]  handle    A UART2_Handle returned from UART2_open()
 */
extern void UART2Support_dmaStartTx(UART2_Handle handle);

/*!
 *  @brief  Function to disable the RX DMA
 *
 *  @param[in]  handle    A UART2_Handle returned from UART2_open()
 */
extern void UART2Support_dmaStopRx(UART2_Handle handle);

/*!
 *  @brief  Function to disable the TX DMA
 *
 *  @param[in]  handle    A UART2_Handle returned from UART2_open()
 */
extern void UART2Support_dmaStopTx(UART2_Handle handle);

/*!
 *  @brief  Function to enable receive, receive timeout, and error interrupts
 *
 *  @param[in]  handle    A UART2_Handle returned from UART2_open()
 */
extern void UART2Support_enableInts(UART2_Handle handle);

/*!
 *  @brief  Function to enable the receive
 *
 *  @param[in]  hwAttrs    A pointer to a UART2_HWAttrs structure
 */
extern void UART2Support_enableRx(UART2_HWAttrs const *hwAttrs);

/*!
 *  @brief  Function to enable the transmit interrupt
 *
 *  @param[in]  hwAttrs    A pointer to a UART2_HWAttrs structure
 */
extern void UART2Support_enableTx(UART2_HWAttrs const *hwAttrs);

/*!
 *  @brief  Function to set power constraint
 */
extern void UART2Support_powerSetConstraint(void);

/*!
 *  @brief  Function to release power constraint
 */
extern void UART2Support_powerRelConstraint(void);

/*!
 *  @brief  Function to convert RX error status to a UART2 error code
 *
 *  @param[in]  errorData    Data indicating the UART RX error status
 *
 *  @return Returns a status indicating success or failure of the read.
 *
 *  @retval UART2_STATUS_SUCCESS  The call was successful.
 *  @retval UART2_STATUS_EOVERRUN A fifo overrun occurred.
 *  @retval UART2_STATUS_EFRAMING A framinig error occurred.
 *  @retval UART2_STATUS_EBREAK   A break error occurred.
 *  @retval UART2_STATUS_EPARITY  A parity error occurred.
 */
extern int_fast16_t UART2Support_rxStatus2ErrorCode(uint32_t errorData);

/*!
 *  @brief  Function to send data
 *
 *  @param[in]  hwAttrs    A pointer to a UART2_HWAttrs structure
 *  @param[in]  size       The number of bytes in the buffer that should be
 *                         written to the UART
 *
 *  @param[in]  buf        A pointer to buffer containing data to be written
 *                         to the UART
 *
 *  @return Returns the number of bytes written
 */
extern uint32_t UART2Support_sendData(UART2_HWAttrs const *hwAttrs,
        size_t size, uint8_t *buf);

/*!
 *  @brief  Function to determine if TX is in progress
 *
 *  @param[in]  hwAttrs    A pointer to a UART2_HWAttrs structure
 *
 *  @return Returns true if there is no TX in progress, otherwise false
 */
extern bool UART2Support_txDone(UART2_HWAttrs const *hwAttrs);

/*!
 *  @brief  Function to clear receive errors
 *
 *  @param[in]  hwAttrs    A pointer to a UART2_HWAttrs structure
 *
 *  @return Returns a status indicating success or failure of the read.
 *
 *  @retval UART2_STATUS_SUCCESS  The call was successful.
 *  @retval UART2_STATUS_EOVERRUN A fifo overrun occurred.
 *  @retval UART2_STATUS_EFRAMING A framinig error occurred.
 *  @retval UART2_STATUS_EBREAK   A break error occurred.
 *  @retval UART2_STATUS_EPARITY  A parity error occurred.
 */
extern int UART2Support_uartRxError(UART2_HWAttrs const *hwAttrs);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_UART2__include */
