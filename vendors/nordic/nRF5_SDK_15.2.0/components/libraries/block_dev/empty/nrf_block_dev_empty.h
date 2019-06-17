/**
 * Copyright (c) 2016 - 2018, Nordic Semiconductor ASA
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
#ifndef NRF_BLOCK_DEV_EMPTY_H__
#define NRF_BLOCK_DEV_EMPTY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_block_dev.h"
#include "nrf_log_instance.h"

/**@file
 *
 * @defgroup nrf_block_dev_empty Empty implementation
 * @ingroup nrf_block_dev
 *
 * This module implements block device API. It works like:
 *           - /dev/empty for write operations
 *           - /dev/zero for read operations
 * @{
 *
 */

/**
 * @brief EMPTY block device operations
 * */
extern const nrf_block_dev_ops_t nrf_block_device_empty_ops;

/**
 * @brief Work structure of EMPTY block device.
 */
typedef struct {
    nrf_block_dev_geometry_t geometry;    //!< Block device geometry
    nrf_block_dev_ev_handler ev_handler;  //!< Block device event handler
    void const *             p_context;   //!< Context handle passed to event handler
} nrf_block_dev_empty_work_t;


/**
 * @brief EMPTY block device config initializer (@ref nrf_block_dev_empty_config_t)
 *
 * @param blk_size   Block size
 * @param blk_count  Block count
 * */
#define NRF_BLOCK_DEV_EMPTY_CONFIG(blk_size, blk_count)  {  \
        .block_size = (blk_size),                           \
        .block_count = (blk_count)                          \
}

/**
 * @brief EMPTY block device config
 */
typedef struct {
    uint32_t block_size;        //!< Desired block size
    uint32_t block_count;       //!< Desired block count
} nrf_block_dev_empty_config_t;

/**
 * @brief EMPTY block device
 * */
typedef struct {
    nrf_block_dev_t              block_dev;     //!< Block device
    nrf_block_dev_info_strings_t info_strings;  //!< Block device information strings
    nrf_block_dev_empty_config_t empty_config;  //!< EMPTY block device config
    nrf_block_dev_empty_work_t * p_work;        //!< EMPTY block device work structure
    NRF_LOG_INSTANCE_PTR_DECLARE(p_log)         //!< Pointer to instance of the logger object (Conditionally compiled).
} nrf_block_dev_empty_t;

/** @brief Name of the module used for logger messaging.
 */
#define NRF_BLOCK_DEV_EMPTY_LOG_NAME block_dev_empty

/**
 * @brief Defines a EMPTY block device.
 *
 * @param name          Instance name
 * @param config        Configuration @ref nrf_block_dev_empty_config_t
 * @param info          Info strings @ref NFR_BLOCK_DEV_INFO_CONFIG
 * */
#define NRF_BLOCK_DEV_EMPTY_DEFINE(name, config, info)                                            \
    static nrf_block_dev_empty_work_t CONCAT_2(name, _work);                                      \
    NRF_LOG_INSTANCE_REGISTER(NRF_BLOCK_DEV_EMPTY_LOG_NAME, name,                                 \
                              NRF_BLOCK_DEV_EMPTY_CONFIG_INFO_COLOR,                              \
                              NRF_BLOCK_DEV_EMPTY_CONFIG_DEBUG_COLOR,                             \
                              NRF_BLOCK_DEV_EMPTY_CONFIG_LOG_INIT_FILTER_LEVEL,                   \
                              NRF_BLOCK_DEV_EMPTY_CONFIG_LOG_ENABLED ?                            \
                                   NRF_BLOCK_DEV_EMPTY_CONFIG_LOG_LEVEL : NRF_LOG_SEVERITY_NONE); \
    static const nrf_block_dev_empty_t name = {                                                   \
        .block_dev = { .p_ops = &nrf_block_device_empty_ops },                                    \
        .info_strings = BRACKET_EXTRACT(info),                                                    \
        .empty_config = config,                                                                   \
        .p_work = &CONCAT_2(name, _work),                                                         \
        NRF_LOG_INSTANCE_PTR_INIT(p_log, NRF_BLOCK_DEV_EMPTY_LOG_NAME, name)                      \
    }

/**
 * @brief Returns block device API handle from EMPTY block device.
 *
 * @param[in] p_blk_empty EMPTY block device
 * @return Block device handle
 */
static inline nrf_block_dev_t const *
nrf_block_dev_empty_ops_get(nrf_block_dev_empty_t const * p_blk_empty)
{
    return &p_blk_empty->block_dev;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRF_BLOCK_DEV_EMPTY_H__ */
