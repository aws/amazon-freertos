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
 *  @file       I2CSupport.h
 *
 *  @brief      Holder of common helper functions for the I2C driver
 *
 *  ============================================================================
 */

#ifndef ti_drivers_I2CSupport__include
#define ti_drivers_I2CSupport__include

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/I2C.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @brief  Function to send a finish command to the master module
 *
 *  @param[in]  hwAttrs    A pointer to an I2C_HWAttrs structure
 */
extern void I2CSupport_masterFinish(I2C_HWAttrs const *hwAttrs);

/*!
 *  @brief  Function to set power constraint
 */
extern void I2CSupport_powerSetConstraint(void);

/*!
 *  @brief  Function to release power constraint
 */
extern void I2CSupport_powerRelConstraint(void);

/*!
 *  @brief  Function to initialize transfers
 *
 *  @param[in]  handle          An I2C_Handle returned from I2C_open()
 *  @param[in]  transaction     A pointer to an I2C_Transaction
 *
 *  @return Returns a status indicating success or failure of the transfer
 *
 *  @retval I2C_STATUS_SUCCESS    The transfer was successful
 *  @retval I2C_STATUS_INCOMPLETE I2C transaction is in progress or returned
 *                                without completing
 *  @retval I2C_STATUS_BUS_BUSY   I2C bus already in use by another I2C Master
 */
extern int_fast16_t I2CSupport_primeTransfer(I2C_Handle handle,
    I2C_Transaction *transaction);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_I2CSupport__include */
