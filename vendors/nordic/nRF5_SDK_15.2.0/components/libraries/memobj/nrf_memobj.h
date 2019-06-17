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

#ifndef NRF_MEMOBJ_H
#define NRF_MEMOBJ_H

/**
* @defgroup nrf_memobj Memory Object module
* @{
* @ingroup app_common
* @brief Functions for controlling a memory object.
*/
#include <stdint.h>
#include <stdlib.h>
#include "sdk_errors.h"
#include "nrf_balloc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A memory object can consist of multiple chunks with the same size. Each object has a header part
 * and a data part. The first element in a memory object is memory object head which has a special header.
 * The remaining objects have a header of the same size.
 * 
@verbatim
 _____________________        _____________________          _____________________
|                     |      |                     |        |                     |
|4 B  head header     |  --> |4 B    p_next        |------->|4 B  p_memobj_pool   |
|_____________________|  |   |_____________________|        |_____________________|
|                     |  |   |                     |        |                     |
|4 B    p_next        |--|   |                     |        |                     |
|_____________________|      |                     |  ....  |                     |
|                     |      |        data         |        |        data         |
|                     |      |                     |        |                     |
|       data          |      |                     |        |                     |
|                     |      |                     |        |                     |
|_____________________|      |_____________________|        |_____________________|
         head                      mid_element                  last_element
@endverbatim
 *
 */
#define NRF_MEMOBJ_STD_HEADER_SIZE sizeof(uint32_t)

/**
 * @brief Macro for creating an nrf_memobj pool.
 *
 * This macro declares an nrf_balloc object. The element in the pool contains a user-defined data part and
 * a memory object header.
 *
 * @param _name       Name of the instance.
 * @param _chunk_size Size of a single chunk.
 * @param _pool_size  Number of chunks in the pool.
 */
#define NRF_MEMOBJ_POOL_DEF(_name, _chunk_size, _pool_size)                           \
    STATIC_ASSERT((_chunk_size) > sizeof(uint32_t));                                  \
    NRF_BALLOC_DEF(_name, ((_chunk_size)+NRF_MEMOBJ_STD_HEADER_SIZE), (_pool_size))

/**
 * @brief Pool of memory objects.
 */
typedef nrf_balloc_t nrf_memobj_pool_t;

/**
 * @brief Memory object handle.
 */
typedef void * nrf_memobj_t;

/**
 * @brief Function for initializing the memobj pool instance.
 *
 * This function initializes the pool.
 *
 * @param[in] p_pool     Pointer to the memobj pool instance structure.
 *
 * @return  NRF_SUCCESS on success, otherwise an error code.
 */
ret_code_t nrf_memobj_pool_init(nrf_memobj_pool_t const * p_pool);

/**
 * @brief Function for allocating a memobj with a requested size.
 *
 * Fixed length elements in the pool are linked together to provide the amount of memory requested by
 * the user. If a memory object is successfully allocated, then the users can use the memory.
 * However, it is fragmented into multiple objects so it must be accessed through the API:
 * @ref nrf_memobj_write and @ref nrf_memobj_read.
 * 
 * @param[in] p_pool     Pointer to the memobj pool instance structure.
 * @param[in] size       Data size of requested object.
 *
 * @return  Pointer to a memory object or NULL if the requested size cannot be allocated.
 */
nrf_memobj_t * nrf_memobj_alloc(nrf_memobj_pool_t const * p_pool,
                                size_t size);

/**
 * @brief Function for indicating that a memory object is used and cannot be freed.
 *
 * Memory object can be shared and reused between multiple modules and this mechanism ensures that
 * object is freed when no longer used by any module. Memory object has a counter which is incremented
 * whenever this function is called. @ref nrf_memobj_put function decrements the counter.
 *
 * @param[in] p_obj  Pointer to memory object.
 */
void nrf_memobj_get(nrf_memobj_t const * p_obj);


/**
 * @brief Function for indicated that memory object is no longer used by the module and can be freed
 * if no other module is using it.
 *
 * Memory object is returned to the pool if internal counter reaches 0 after decrementing. It means
 * that no other module is needing it anymore.
 *
 * @note Memory object holds pointer to the pool which was used to allocate it so it does not have
 * to be provided explicitly to this function.
 *
 * @param[in] p_obj  Pointer to memory object.
 */
void nrf_memobj_put(nrf_memobj_t * p_obj);


/**
 * @brief Function for forcing freeing of the memory object.
 *
 * @note This function should be use with caution because it can lead to undefined behavior of the
 * modules since modules using the memory object are not aware that it has been freed.
 *
 * @param[in] p_obj  Pointer to memory object.
 */
void nrf_memobj_free(nrf_memobj_t * p_obj);

/**
 * @brief Function for writing data to the memory object.
 *
 * @param[in]      p_obj  Pointer to memory object.
 * @param[in]      p_data Pointer to data to be written to the memory object.
 * @param[in]      len    Amount of data to be written to the memory object.
 * @param[in]      offset Offset.
 */
void nrf_memobj_write(nrf_memobj_t * p_obj,
                      void *         p_data,
                      size_t         len,
                      size_t         offset);

/**
 * @brief Function for reading data from the memory object.
 *
 * @param[in]     p_obj  Pointer to memory object.
 * @param[in]     p_data Pointer to the destination buffer.
 * @param[in]     len    Amount of data to be read from the memory object.
 * @param[in]     offset Offset.
 */
void nrf_memobj_read(nrf_memobj_t * p_obj,
                     void *         p_data,
                     size_t         len,
                     size_t         offset);

#ifdef __cplusplus
}
#endif

#endif //NRF_MEMOBJ_H

/** @} */
