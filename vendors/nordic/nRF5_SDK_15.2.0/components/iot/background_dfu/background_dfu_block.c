/**
 * Copyright (c) 2017 - 2018, Nordic Semiconductor ASA
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

/** @file
 *
 * @defgroup background_dfu_block background_dfu_block.c
 * @{
 * @ingroup background_dfu
 * @brief Background DFU block handling implementation.
 *
 */

#include "background_dfu_block.h"

#include <assert.h>

#include "sdk_config.h"
#include "app_scheduler.h"
#include "background_dfu_operation.h"
#include "compiler_abstraction.h"
#include "nrf_dfu_handling_error.h"

#define NRF_LOG_MODULE_NAME background_dfu

#define NRF_LOG_LEVEL       BACKGROUND_DFU_CONFIG_LOG_LEVEL
#define NRF_LOG_INFO_COLOR  BACKGROUND_DFU_CONFIG_INFO_COLOR
#define NRF_LOG_DEBUG_COLOR BACKGROUND_DFU_CONFIG_DEBUG_COLOR

#include "nrf_log.h"

#define BITMAP_BYTE_FROM_INDEX(index) ((index) / 8)
#define BITMAP_BIT_FROM_INDEX(index)  (7 - ((index) % 8))

static void block_buffer_store(background_dfu_block_manager_t * p_bm);

/**@brief Convert block number to bitmap index.
 *
 * @param[in] block_num Block number.
 *
 * @return Corresponding index.
 */
static __INLINE uint16_t block_num_to_index(uint32_t block_num)
{
    return block_num % BLOCKS_PER_BUFFER;
}

/**@brief Set a bit in a bitmap.
 *
 * @param[inout] p_bitmap A pointer to the bitmap.
 * @param[in]    index    Bit index to set.
 */
static __INLINE void set_bitmap_bit(uint8_t * p_bitmap, uint16_t index)
{
    p_bitmap[BITMAP_BYTE_FROM_INDEX(index)] |= (0x01 << BITMAP_BIT_FROM_INDEX(index));
}

/**@brief Clear a bit in a bitmap.
 *
 * @param[inout] p_bitmap A pointer to the bitmap.
 * @param[in]    index    Bit index to clear.
 */
static __INLINE void clear_bitmap_bit(uint8_t * p_bitmap, uint16_t index)
{
    p_bitmap[BITMAP_BYTE_FROM_INDEX(index)] &= ~((uint8_t)(0x01 << BITMAP_BIT_FROM_INDEX(index)));
}

/**@brief Check if a bit in a bitmap is set.
 *
 * @param[inout] p_bitmap A pointer to the bitmap.
 * @param[in]    index    Bit index to check.
 *
 * @return True if bit is set, false otherwise.
 */
static __INLINE bool is_block_present(const uint8_t * p_bitmap, uint16_t index)
{
    return (p_bitmap[BITMAP_BYTE_FROM_INDEX(index)] >> BITMAP_BIT_FROM_INDEX(index)) & 0x01;
}

/**
 * @brief A callback function for DFU operation.
 */
static void dfu_operation_callback(nrf_dfu_response_t * p_res, void * p_context)
{
    background_dfu_block_manager_t * p_bm = (background_dfu_block_manager_t *)p_context;
    ret_code_t res_code;

    if (p_res->result != NRF_DFU_RES_CODE_SUCCESS)
    {
        p_bm->result_handler(BACKGROUND_DFU_BLOCK_STORE_ERROR, p_bm->p_context);
    }
    else
    {
        switch (p_res->request)
        {
            case NRF_DFU_OP_OBJECT_CREATE:
            {
                // Object created, write respective block.
                uint32_t current_size = p_bm->currently_stored_block * DEFAULT_BLOCK_SIZE;
                uint16_t data_offset  = block_num_to_index(p_bm->currently_stored_block) * DEFAULT_BLOCK_SIZE;
                uint16_t store_size   = MIN(DEFAULT_BLOCK_SIZE, (p_bm->image_size - current_size));
                res_code = background_dfu_op_write(p_bm->data + data_offset,
                                                   store_size,
                                                   dfu_operation_callback,
                                                   p_bm);

                if (res_code != NRF_SUCCESS)
                {
                    NRF_LOG_ERROR("Failed to store block (b:%d c:%d).",
                                    p_bm->currently_stored_block,
                                    p_bm->current_block);
                    p_bm->result_handler(BACKGROUND_DFU_BLOCK_STORE_ERROR, p_bm->p_context);
                }

                break;
            }

            case NRF_DFU_OP_OBJECT_WRITE:
                if (!((p_bm->currently_stored_block + 1) % BLOCKS_PER_DFU_OBJECT) ||
                    ((p_bm->currently_stored_block + 1) == BLOCKS_PER_SIZE(p_bm->image_size)))
                {
                    res_code = background_dfu_op_crc(dfu_operation_callback, p_bm);

                    if (res_code != NRF_SUCCESS)
                    {
                        NRF_LOG_ERROR("Failed to store block (b:%d c:%d).",
                                        p_bm->currently_stored_block,
                                        p_bm->current_block);
                        p_bm->result_handler(BACKGROUND_DFU_BLOCK_STORE_ERROR, p_bm->p_context);
                    }
                }
                else
                {
                    p_bm->last_block_stored = p_bm->currently_stored_block;
                    clear_bitmap_bit(p_bm->bitmap, block_num_to_index(p_bm->currently_stored_block));
                    p_bm->currently_stored_block = INVALID_BLOCK_NUMBER;

                    p_bm->result_handler(BACKGROUND_DFU_BLOCK_SUCCESS, p_bm->p_context);

                    block_buffer_store(p_bm);
                }

                break;

            case NRF_DFU_OP_CRC_GET:
                res_code = background_dfu_op_execute(dfu_operation_callback, p_bm);

                if (res_code != NRF_SUCCESS)
                {
                    NRF_LOG_ERROR("Failed to store block (b:%d c:%d).",
                                    p_bm->currently_stored_block,
                                    p_bm->current_block);
                    p_bm->result_handler(BACKGROUND_DFU_BLOCK_STORE_ERROR, p_bm->p_context);
                }

                break;

            case NRF_DFU_OP_OBJECT_EXECUTE:
                p_bm->last_block_stored = p_bm->currently_stored_block;
                clear_bitmap_bit(p_bm->bitmap, block_num_to_index(p_bm->currently_stored_block));
                p_bm->currently_stored_block = INVALID_BLOCK_NUMBER;

                p_bm->result_handler(BACKGROUND_DFU_BLOCK_SUCCESS, p_bm->p_context);

                block_buffer_store(p_bm);

                break;

            default:
                ASSERT(false);
        }
    }
}

/**@brief Store a block from the buffer in a flash.
 *
 * @param[inout] p_bm    A pointer to the block manager.
 * @param[in]    p_block A block number to store.
 *
 * @return NRF_SUCCESS on success, an error code is returned otherwise.
 */
static ret_code_t block_store(background_dfu_block_manager_t * p_bm, uint32_t block_num)
{
    p_bm->currently_stored_block = block_num;

    ret_code_t res_code = NRF_SUCCESS;
    uint32_t current_size = block_num * DEFAULT_BLOCK_SIZE;

    do
    {
        // Initialize DFU object if needed.
        if (!(block_num % BLOCKS_PER_DFU_OBJECT))
        {
            uint32_t object_size = MIN(DEFAULT_DFU_OBJECT_SIZE, (p_bm->image_size - current_size));

            res_code = background_dfu_op_create(p_bm->image_type,
                                                object_size,
                                                dfu_operation_callback,
                                                p_bm);
            break;
        }

        // Store block.
        uint16_t data_offset = block_num_to_index(block_num) * DEFAULT_BLOCK_SIZE;
        uint16_t store_size = MIN(DEFAULT_BLOCK_SIZE, (p_bm->image_size - current_size));
        res_code = background_dfu_op_write(p_bm->data + data_offset,
                                           store_size,
                                           dfu_operation_callback,
                                           p_bm);

    } while (0);
    return res_code;
}

/**@brief Check if block manager is busy storing a block.
 *
 * @param[inout] p_bm A pointer to the block manager.
 *
 */
static bool is_block_manager_busy(background_dfu_block_manager_t * p_bm)
{
    return p_bm->currently_stored_block >= 0;
}

/**@brief Store any valid blocks from the buffer in a flash.
 *
 * @param[inout] p_bm A pointer to the block manager.
 *
 */
static void block_buffer_store(background_dfu_block_manager_t * p_bm)
{
    ret_code_t res_code = NRF_SUCCESS;

    if (!is_block_manager_busy(p_bm))
    {
        if (p_bm->last_block_stored < p_bm->current_block)
        {
            int32_t block = p_bm->last_block_stored + 1;

            if (is_block_present(p_bm->bitmap, block_num_to_index(block)))
            {
                NRF_LOG_INFO("Storing block (b:%d c:%d).", block, p_bm->current_block);

                // There is a block to store.
                res_code = block_store(p_bm, block);
                if (res_code != NRF_SUCCESS)
                {
                    NRF_LOG_ERROR("Failed to store block (b:%d c:%d).", block, p_bm->current_block);
                    p_bm->result_handler(BACKGROUND_DFU_BLOCK_STORE_ERROR, p_bm->p_context);
                }
            }
            else
            {
                NRF_LOG_WARNING("Gap encountered - quit (b:%d c:%d).", block, p_bm->current_block);
            }
        }
    }
}

/**
 * @brief A callback function for scheduling DFU block operations.
 */
static void block_store_scheduled(void * p_evt, uint16_t event_length)
{
    UNUSED_PARAMETER(event_length);

    background_dfu_block_manager_t * p_bm = *((background_dfu_block_manager_t **)p_evt);
    block_buffer_store(p_bm);
}

/**@brief Copy block data to the buffer.
 *
 * @param[inout] p_bm    A pointer to the block manager.
 * @param[in]    p_block A pointer to the block.
 */
static void block_buffer_add(background_dfu_block_manager_t * p_bm,
                             const background_dfu_block_t   * p_block)
{
    uint16_t index = block_num_to_index(p_block->number);

    memcpy(p_bm->data + index * DEFAULT_BLOCK_SIZE, p_block->p_payload, DEFAULT_BLOCK_SIZE);
    set_bitmap_bit(p_bm->bitmap, index);

    if (p_bm->current_block < (int32_t)p_block->number)
    {
        p_bm->current_block = (int32_t)p_block->number;
    }

    // Schedule block store.
    UNUSED_RETURN_VALUE(app_sched_event_put(&p_bm, sizeof(p_bm), block_store_scheduled));
}

/***************************************************************************************************
 * @section Public
 **************************************************************************************************/

void block_manager_init(background_dfu_block_manager_t * p_bm,
                        uint32_t                         object_type,
                        uint32_t                         object_size,
                        int32_t                          initial_block,
                        block_manager_result_notify_t    result_handler,
                        void                           * p_context)
{
    p_bm->image_type             = object_type;
    p_bm->image_size             = object_size;
    p_bm->last_block_stored      = p_bm->current_block = initial_block - 1;
    p_bm->result_handler         = result_handler;
    p_bm->p_context              = p_context;
    p_bm->currently_stored_block = INVALID_BLOCK_NUMBER;

    memset(p_bm->bitmap, 0, sizeof(p_bm->bitmap));
}

background_dfu_block_result_t block_manager_block_process(background_dfu_block_manager_t * p_bm,
                                                          const background_dfu_block_t   * p_block)
{
    /*
     * Possible scenarios:
     *  1) We receive a block older than our last stored block - simply ignore it.
     *  2) We receive a block that fits within current buffer range - process it.
     *  3) We receive a block that exceeds current buffer range - abort DFU as we won't be able to catch-up.
     */

    if (p_block->size != DEFAULT_BLOCK_SIZE)
    {
        NRF_LOG_WARNING("Block with incorrect size received (s:%d n:%d).",
                p_block->size, p_block->number);
        return BACKGROUND_DFU_BLOCK_IGNORE;
    }

    if ((int32_t)p_block->number <= p_bm->last_block_stored)
    {
        NRF_LOG_WARNING("Ignoring block that already was stored(o:%d n:%d).",
                p_bm->last_block_stored, p_block->number);
        return BACKGROUND_DFU_BLOCK_IGNORE;
    }

    if ((int32_t)p_block->number > p_bm->last_block_stored + BLOCKS_PER_BUFFER)
    {
        NRF_LOG_WARNING("Too many blocks missed - abort DFU (o:%d n:%d).",
                p_bm->last_block_stored, p_block->number);
        return BACKGROUND_DFU_BLOCK_INVALID;
    }

    // Block fits within current buffer - copy it into the buffer and update the current block if most
    // recent block was received.
    block_buffer_add(p_bm, p_block);

    return BACKGROUND_DFU_BLOCK_SUCCESS;
}

bool block_manager_is_image_complete(const background_dfu_block_manager_t * p_bm)
{
    uint32_t image_blocks = BLOCKS_PER_SIZE(p_bm->image_size);

    NRF_LOG_DEBUG("Is image complete (o:%d n:%d).", p_bm->last_block_stored, image_blocks);

    if (p_bm->last_block_stored + 1 == image_blocks)
    {
        return true;
    }

    return false;
}

bool block_manager_request_bitmap_get(const background_dfu_block_manager_t * p_bm,
                                      background_dfu_request_bitmap_t      * p_req_bmp)
{
    if (p_bm->current_block > p_bm->last_block_stored)
    {
        memset(p_req_bmp, 0, sizeof(*p_req_bmp));
        p_req_bmp->offset = p_bm->last_block_stored + 1;
        p_req_bmp->size = (p_bm->current_block - p_bm->last_block_stored + 7) / 8;

        for (uint16_t block = p_req_bmp->offset; block <= p_bm->current_block; block++)
        {
            if (!is_block_present(p_bm->bitmap, block_num_to_index(block)))
            {
                set_bitmap_bit(p_req_bmp->bitmap, block - p_req_bmp->offset);
            }
        }

        // Clip empty bytes at the end.
        while ((p_req_bmp->size > 0) && (p_req_bmp->bitmap[p_req_bmp->size - 1] == 0))
        {
            p_req_bmp->size--;
        }

        if (p_req_bmp->size == 0)
        {
            return false;
        }

        return true;
    }

    return false;
}

bool block_manager_increment_current_block(background_dfu_block_manager_t * p_bm)
{
    uint32_t image_blocks = BLOCKS_PER_SIZE(p_bm->image_size);

    if (p_bm->current_block + 1 == image_blocks)
    {
        // Already on last block.
        return false;
    }
    else
    {
        p_bm->current_block++;
    }

    return true;
}

int32_t block_manager_get_current_block(const background_dfu_block_manager_t * p_bm)
{
    return p_bm->current_block;
}
