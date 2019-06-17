/**
 * Copyright (c) 2015 - 2018, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef NRF_TWI_MNGR_H__
#define NRF_TWI_MNGR_H__

#include <stdint.h>
#include "nrf_drv_twi.h"
#include "sdk_errors.h"
#include "nrf_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_twi_mngr TWI transaction manager
 * @{
 * @ingroup app_common
 *
 * @brief Module for scheduling TWI transactions.
 */

//If TWIM is present buffers can only be in RAM
/*lint -save -e491*/

/**
 * @brief Macro checking if buffers should be stored in RAM.
 */
#ifndef NRF_TWI_MNGR_BUFFERS_IN_RAM
  #define NRF_TWI_MNGR_BUFFERS_IN_RAM defined(TWIM_PRESENT)
#endif

/**
 * @brief Modifier used in array declaration for TWI Manager.
 *
 * @note  For TWI peripheral array can be const, for TWIM array has to be located in RAM.
 */
#if NRF_TWI_MNGR_BUFFERS_IN_RAM
  #define NRF_TWI_MNGR_BUFFER_LOC_IND
#else
  #define NRF_TWI_MNGR_BUFFER_LOC_IND const
#endif
/*lint -restore*/

/**
 * @brief Flag indicating that a given transfer should not be ended
 *        with a stop condition.
 *
 * Use this flag when a stop condition is undesirable between two transfers,
 * for example, when the first transfer is a write that sets an address in the slave
 * device and the second one is a read that fetches certain data using this
 * address. In this case, the second transfer should follow directly after the
 * first transfer, with a repeated start condition instead of a stop and then
 * a new start condition.
 */
#define NRF_TWI_MNGR_NO_STOP     0x01

/**
 * @brief Macro for creating a write transfer.
 *
 * @param[in] address Slave address.
 * @param[in] p_data  Pointer to the data to be sent.
 * @param[in] length  Number of bytes to transfer.
 * @param[in] flags   Transfer flags (see @ref NRF_TWI_MNGR_NO_STOP).
 */
#define NRF_TWI_MNGR_WRITE(address, p_data, length, flags) \
    NRF_TWI_MNGR_TRANSFER(NRF_TWI_MNGR_WRITE_OP(address), p_data, length, flags)

/**
 * @brief Macro for creating a read transfer.
 *
 * @param     address Slave address.
 * @param[in] p_data  Pointer to the buffer where received data should be placed.
 * @param     length  Number of bytes to transfer.
 * @param     flags   Transfer flags (see @ref NRF_TWI_MNGR_NO_STOP).
 */
#define NRF_TWI_MNGR_READ(address, p_data, length, flags) \
    NRF_TWI_MNGR_TRANSFER(NRF_TWI_MNGR_READ_OP(address), p_data, length, flags)

/**
 * @brief Helper macro, should not be used directly.
 */
#define NRF_TWI_MNGR_TRANSFER(_operation, _p_data, _length, _flags) \
{                                      \
    .p_data    = (uint8_t *)(_p_data), \
    .length    = _length,              \
    .operation = _operation,           \
    .flags     = _flags                \
}
/**
 * @brief Helper macro, should not be used directly.
 */
#define NRF_TWI_MNGR_WRITE_OP(address)      (((address) << 1) | 0)
/**
 * @brief Helper macro, should not be used directly.
 */
#define NRF_TWI_MNGR_READ_OP(address)       (((address) << 1) | 1)
/**
 * @brief Helper macro, should not be used directly.
 */
#define NRF_TWI_MNGR_IS_READ_OP(operation)  ((operation) & 1)
/**
 * @brief Helper macro, should not be used directly.
 */
#define NRF_TWI_MNGR_OP_ADDRESS(operation)  ((operation) >> 1)

/**
 * @brief TWI transaction callback prototype.
 *
 * @param     result      Result of operation (NRF_SUCCESS on success,
 *                        otherwise a relevant error code).
 * @param[in] p_user_data Pointer to user data defined in transaction
 *                        descriptor.
 */
typedef void (* nrf_twi_mngr_callback_t)(ret_code_t result, void * p_user_data);

/**
 * @brief TWI transfer descriptor.
 */
typedef struct {
    uint8_t * p_data;     ///< Pointer to the buffer holding the data.
    uint8_t   length;     ///< Number of bytes to transfer.
    uint8_t   operation;  ///< Device address combined with transfer direction.
    uint8_t   flags;      ///< Transfer flags (see @ref NRF_TWI_MNGR_NO_STOP).
} nrf_twi_mngr_transfer_t;

/**
 * @brief TWI transaction descriptor.
 */
typedef struct {
    nrf_twi_mngr_callback_t         callback;
    ///< User-specified function to be called after the transaction is finished.

    void *                          p_user_data;
    ///< Pointer to user data to be passed to the callback.

    nrf_twi_mngr_transfer_t const * p_transfers;
    ///< Pointer to the array of transfers that make up the transaction.

    uint8_t                         number_of_transfers;
    ///< Number of transfers that make up the transaction.

    nrf_drv_twi_config_t const *    p_required_twi_cfg;
    ///< Pointer to instance hardware configuration.
} nrf_twi_mngr_transaction_t;

/**
 * @brief TWI instance control block.
 */
typedef struct {
    nrf_twi_mngr_transaction_t const * volatile p_current_transaction;
    ///< Currently realized transaction.

    nrf_drv_twi_config_t default_configuration;
    ///< Default hardware configuration.

    nrf_drv_twi_config_t const * p_current_configuration;
    ///< Pointer to current hardware configuration.

    uint8_t volatile current_transfer_idx;
    ///< Index of currently performed transfer (within current transaction).
} nrf_twi_mngr_cb_t;

/**
 * @brief TWI transaction manager instance.
 */
typedef struct {
    nrf_twi_mngr_cb_t * p_nrf_twi_mngr_cb;
    ///< Control block of instance.

    nrf_queue_t const * p_queue;
    ///< Transaction queue.

    nrf_drv_twi_t twi;
    ///< Pointer to TWI master driver instance.
} nrf_twi_mngr_t;

/**
 * @brief Macro that simplifies defining a TWI transaction manager
 *        instance.
 *
 * This macro allocates a static buffer for the transaction queue.
 * Therefore, it should be used in only one place in the code for a given
 * instance.
 *
 * @note The queue size is the maximum number of pending transactions
 *       not counting the one that is currently realized. This means that
 *       for an empty queue with size of, for example, 4 elements, it is
 *       possible to schedule up to 5 transactions.
 *
 * @param[in]  _nrf_twi_mngr_name    Name of instance to be created.
 * @param[in]  _queue_size       Size of the transaction queue (maximum number
 *                              of pending transactions).
 * @param[in]  _twi_idx          Index of hardware TWI instance to be used.
 */
#define NRF_TWI_MNGR_DEF(_nrf_twi_mngr_name, _queue_size, _twi_idx) \
    NRF_QUEUE_DEF(nrf_twi_mngr_transaction_t const *,                                           \
                  _nrf_twi_mngr_name##_queue,                                                   \
                  (_queue_size),                                                                \
                  NRF_QUEUE_MODE_NO_OVERFLOW);                                                  \
    static nrf_twi_mngr_cb_t CONCAT_2(_nrf_twi_mngr_name, _cb);                                 \
    static const nrf_twi_mngr_t _nrf_twi_mngr_name =                                            \
    {                                                                                           \
        .p_nrf_twi_mngr_cb          = &CONCAT_2(_nrf_twi_mngr_name, _cb),                       \
        .p_queue                    = &_nrf_twi_mngr_name##_queue,                              \
        .twi                        = NRF_DRV_TWI_INSTANCE(_twi_idx)                            \
    }

/**
 * @brief Function for initializing a TWI transaction manager instance.
 *
 * @param[in] p_nrf_twi_mngr                 Pointer to the instance to be initialized.
 * @param[in] p_default_twi_config     Pointer to the TWI master driver configuration. This configuration
 *                                      will be used whenever the scheduled transaction will have
 *                                      p_twi_configuration set to NULL value.
 *
 * @return Values returned by the @ref nrf_drv_twi_init function.
 */
ret_code_t nrf_twi_mngr_init(nrf_twi_mngr_t const *          p_nrf_twi_mngr,
                             nrf_drv_twi_config_t const *    p_default_twi_config);

/**
 * @brief Function for uninitializing a TWI transaction manager instance.
 *
 * @param[in] p_nrf_twi_mngr Pointer to the instance to be uninitialized.
 */
void nrf_twi_mngr_uninit(nrf_twi_mngr_t const * p_nrf_twi_mngr);

/**
 * @brief Function for scheduling a TWI transaction.
 *
 * The transaction is enqueued and started as soon as the TWI bus is
 * available, thus when all previously scheduled transactions have been
 * finished (possibly immediately).
 *
 * @note If @ref nrf_twi_mngr_transaction_t::p_required_twi_cfg
 *       is set to a non-NULL value the module will compare it with
 *       @ref nrf_twi_mngr_cb_t::p_current_configuration and reinitialize hardware
 *       TWI instance with new parameters if any differences are found.
 *       If @ref nrf_twi_mngr_transaction_t::p_required_twi_cfg is set to NULL then
 *       it will treat it as it would be set to @ref nrf_twi_mngr_cb_t::default_configuration.
 *
 * @param[in] p_nrf_twi_mngr     Pointer to the TWI transaction manager instance.
 * @param[in] p_transaction Pointer to the descriptor of the transaction to be
 *                          scheduled.
 *
 * @retval NRF_SUCCESS      If the transaction has been successfully scheduled.
 * @retval NRF_ERROR_NO_MEM If the queue is full (Only if queue in
 *                          @ref NRF_QUEUE_MODE_NO_OVERFLOW).
 */
ret_code_t nrf_twi_mngr_schedule(nrf_twi_mngr_t const *             p_nrf_twi_mngr,
                                 nrf_twi_mngr_transaction_t const * p_transaction);

/**
 * @brief Function for scheduling a transaction and waiting until it is finished.
 *
 * This function schedules a transaction that consists of one or more transfers
 * and waits until it is finished.
 *
 * @param[in] p_nrf_twi_mngr      Pointer to the TWI transaction manager instance.
 * @param[in] p_config            Required TWI configuration.
 * @param[in] p_transfers         Pointer to an array of transfers to be performed.
 * @param     number_of_transfers Number of transfers to be performed.
 * @param     user_function       User-specified function to be called while
 *                                waiting. NULL if such functionality
 *                                is not needed.
 *
 * @retval NRF_SUCCESS    If the transfers have been successfully realized.
 * @retval NRF_ERROR_BUSY If some transfers are already being performed.
 * @retval -              Other error codes mean that the transaction has ended
 *                        with the error that is specified in the error code.
 */
ret_code_t nrf_twi_mngr_perform(nrf_twi_mngr_t const *          p_nrf_twi_mngr,
                                nrf_drv_twi_config_t const *    p_config,
                                nrf_twi_mngr_transfer_t const * p_transfers,
                                uint8_t                         number_of_transfers,
                                void                            (* user_function)(void));

/**
 * @brief Function for getting the current state of a TWI transaction manager
 *        instance.
 *
 * @param[in] p_nrf_twi_mngr Pointer to the TWI transaction manager instance.
 *
 * @retval true  If all scheduled transactions have been finished.
 * @retval false Otherwise.
 */
__STATIC_INLINE bool nrf_twi_mngr_is_idle(nrf_twi_mngr_t const * p_nrf_twi_mngr);

#ifndef SUPPRESS_INLINE_IMPLEMENTATION
__STATIC_INLINE bool nrf_twi_mngr_is_idle(nrf_twi_mngr_t const * p_nrf_twi_mngr)
{
    return (p_nrf_twi_mngr->p_nrf_twi_mngr_cb->p_current_transaction == NULL);
}
#endif //SUPPRESS_INLINE_IMPLEMENTATION
/**
 *@}
 **/


#ifdef __cplusplus
}
#endif

#endif // NRF_TWI_MNGR_H__
