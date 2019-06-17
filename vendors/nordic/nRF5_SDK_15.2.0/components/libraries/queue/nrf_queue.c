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
#include "sdk_common.h"
#if NRF_MODULE_ENABLED(NRF_QUEUE)
#include "nrf_queue.h"
#include "app_util_platform.h"

#if NRF_QUEUE_CONFIG_LOG_ENABLED
    #define NRF_LOG_LEVEL             NRF_QUEUE_CONFIG_LOG_LEVEL
    #define NRF_LOG_INIT_FILTER_LEVEL NRF_QUEUE_CONFIG_LOG_INIT_FILTER_LEVEL
    #define NRF_LOG_INFO_COLOR        NRF_QUEUE_CONFIG_INFO_COLOR
    #define NRF_LOG_DEBUG_COLOR       NRF_QUEUE_CONFIG_DEBUG_COLOR
#else
    #define NRF_LOG_LEVEL       0
#endif // NRF_QUEUE_CONFIG_LOG_ENABLED
#include "nrf_log.h"

NRF_SECTION_DEF(nrf_queue, nrf_queue_t);

#if NRF_QUEUE_CLI_CMDS
#include "nrf_cli.h"

static void nrf_queue_status(nrf_cli_t const * p_cli, size_t argc, char **argv)
{
    UNUSED_PARAMETER(argv);

    if (nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
        return;
    }

    if (argc > 1)
    {
        nrf_cli_fprintf(p_cli, NRF_CLI_ERROR, "Bad argument count");
        return;
    }

    uint32_t num_of_instances = NRF_SECTION_ITEM_COUNT(nrf_queue, nrf_queue_t);
    uint32_t i;

    for (i = 0; i < num_of_instances; i++)
    {
        const nrf_queue_t * p_instance = NRF_SECTION_ITEM_GET(nrf_queue, nrf_queue_t, i);

        uint32_t element_size = p_instance->element_size;
        uint32_t size         = p_instance->size;
        uint32_t max_util     = nrf_queue_max_utilization_get(p_instance);
        uint32_t util         = nrf_queue_utilization_get(p_instance);
        const char * p_name   = p_instance->p_name;
        nrf_cli_fprintf(p_cli, NRF_CLI_NORMAL,
                        "%s\r\n\t- Element size:\t%d\r\n"
                        "\t- Usage:\t%u%% (%u out of %u elements)\r\n"
                        "\t- Maximum:\t%u%% (%u out of %u elements)\r\n"
                        "\t- Mode:\t\t%s\r\n\r\n",
                        p_name, element_size,
                        100ul * util/size, util,size,
                        100ul * max_util/size, max_util,size,
                        (p_instance->mode == NRF_QUEUE_MODE_OVERFLOW) ? "Overflow" : "No overflow");

    }
}
// Register "queue" command and its subcommands in CLI.
NRF_CLI_CREATE_STATIC_SUBCMD_SET(nrf_queue_commands)
{
     NRF_CLI_CMD(status, NULL, "Print status of queue instances.", nrf_queue_status),
     NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CMD_REGISTER(queue, &nrf_queue_commands, "Commands for BALLOC management", nrf_queue_status);
#endif //NRF_QUEUE_CLI_CMDS

/**@brief Get next element index.
 *
 * @param[in]   p_queue     Pointer to the queue instance.
 * @param[in]   idx         Current index.
 *
 * @return      Next element index.
 */
__STATIC_INLINE size_t nrf_queue_next_idx(nrf_queue_t const * p_queue, size_t idx)
{
    ASSERT(p_queue != NULL);
    return (idx < p_queue->size) ? (idx + 1) : 0;
}

/**@brief Get current queue utilization. This function assumes that this process will not be interrupted.
 *
 * @param[in]   p_queue     Pointer to the queue instance.
 *
 * @return      Current queue utilization.
 */
__STATIC_INLINE size_t queue_utilization_get(nrf_queue_t const * p_queue)
{
    size_t front    = p_queue->p_cb->front;
    size_t back     = p_queue->p_cb->back;
    return (back >= front) ? (back - front) : (p_queue->size + 1 - front + back);
}

bool nrf_queue_is_full(nrf_queue_t const * p_queue)
{
    ASSERT(p_queue != NULL);
    size_t front    = p_queue->p_cb->front;
    size_t back     = p_queue->p_cb->back;

    return (nrf_queue_next_idx(p_queue, back) == front);
}

ret_code_t nrf_queue_push(nrf_queue_t const * p_queue, void const * p_element)
{
    ret_code_t status = NRF_SUCCESS;

    ASSERT(p_queue != NULL);
    ASSERT(p_element != NULL);

    CRITICAL_REGION_ENTER();
    bool is_full = nrf_queue_is_full(p_queue);

    if (!is_full || (p_queue->mode == NRF_QUEUE_MODE_OVERFLOW))
    {
        // Get write position.
        size_t write_pos = p_queue->p_cb->back;
        p_queue->p_cb->back = nrf_queue_next_idx(p_queue, p_queue->p_cb->back);
        if (is_full)
        {
            // Overwrite the oldest element.
            NRF_LOG_INST_WARNING(p_queue->p_log, "Queue full. Overwriting oldest element.");
            p_queue->p_cb->front = nrf_queue_next_idx(p_queue, p_queue->p_cb->front);
        }

        // Write a new element.
        switch (p_queue->element_size)
        {
            case sizeof(uint8_t):
                ((uint8_t *)p_queue->p_buffer)[write_pos] = *((uint8_t *)p_element);
                break;

            case sizeof(uint16_t):
                ((uint16_t *)p_queue->p_buffer)[write_pos] = *((uint16_t *)p_element);
                break;

            case sizeof(uint32_t):
                ((uint32_t *)p_queue->p_buffer)[write_pos] = *((uint32_t *)p_element);
                break;

            case sizeof(uint64_t):
                ((uint64_t *)p_queue->p_buffer)[write_pos] = *((uint64_t *)p_element);
                break;

            default:
                memcpy((void *)((size_t)p_queue->p_buffer + write_pos * p_queue->element_size),
                       p_element,
                       p_queue->element_size);
                break;
        }

        // Update utilization.
        size_t utilization = queue_utilization_get(p_queue);
        if (p_queue->p_cb->max_utilization < utilization)
        {
            p_queue->p_cb->max_utilization = utilization;
        }
    }
    else
    {
        status = NRF_ERROR_NO_MEM;
    }

    CRITICAL_REGION_EXIT();

    NRF_LOG_INST_DEBUG(p_queue->p_log, "pushed element 0x%08X, status:%d", p_element, status);
    return status;
}

ret_code_t nrf_queue_generic_pop(nrf_queue_t const * p_queue,
                                 void              * p_element,
                                 bool                just_peek)
{
    ret_code_t status = NRF_SUCCESS;

    ASSERT(p_queue      != NULL);
    ASSERT(p_element    != NULL);

    CRITICAL_REGION_ENTER();

    if (!nrf_queue_is_empty(p_queue))
    {
        // Get read position.
        size_t read_pos = p_queue->p_cb->front;

        // Update next read position.
        if (!just_peek)
        {
            p_queue->p_cb->front = nrf_queue_next_idx(p_queue, p_queue->p_cb->front);
        }

        // Read element.
        switch (p_queue->element_size)
        {
            case sizeof(uint8_t):
                *((uint8_t *)p_element) = ((uint8_t *)p_queue->p_buffer)[read_pos];
                break;

            case sizeof(uint16_t):
                *((uint16_t *)p_element) = ((uint16_t *)p_queue->p_buffer)[read_pos];
                break;

            case sizeof(uint32_t):
                *((uint32_t *)p_element) = ((uint32_t *)p_queue->p_buffer)[read_pos];
                break;

            case sizeof(uint64_t):
                *((uint64_t *)p_element) = ((uint64_t *)p_queue->p_buffer)[read_pos];
                break;

            default:
                memcpy(p_element,
                       (void const *)((size_t)p_queue->p_buffer + read_pos * p_queue->element_size),
                       p_queue->element_size);
                break;
        }
    }
    else
    {
        status = NRF_ERROR_NOT_FOUND;
    }

    CRITICAL_REGION_EXIT();
    NRF_LOG_INST_DEBUG(p_queue->p_log, "%s element 0x%08X, status:%d",
                                         just_peek ? "peeked" : "popped", p_element, status);
    return status;
}

/**@brief Write elements to the queue. This function assumes that there is enough room in the queue
 *        to write the requested number of elements and that this process will not be interrupted.
 *
 * @param[in]   p_queue             Pointer to the nrf_queue_t instance.
 * @param[in]   p_data              Pointer to the buffer with elements to write.
 * @param[in]   element_count       Number of elements to write.
 */
static void queue_write(nrf_queue_t const * p_queue, void const * p_data, uint32_t element_count)
{
    size_t prev_available = nrf_queue_available_get(p_queue);
    size_t continuous     = p_queue->size + 1 - p_queue->p_cb->back;
    void * p_write_ptr    = (void *)((size_t)p_queue->p_buffer
                          + p_queue->p_cb->back * p_queue->element_size);
    if (element_count <= continuous)
    {
        memcpy(p_write_ptr,
               p_data,
               element_count * p_queue->element_size);

        p_queue->p_cb->back = ((p_queue->p_cb->back + element_count) <= p_queue->size)
                            ? (p_queue->p_cb->back + element_count)
                            : 0;
    }
    else
    {
        size_t first_write_length = continuous * p_queue->element_size;
        memcpy(p_write_ptr,
               p_data,
               first_write_length);

        size_t elements_left = element_count - continuous;
        memcpy(p_queue->p_buffer,
               (void const *)((size_t)p_data + first_write_length),
               elements_left * p_queue->element_size);

        p_queue->p_cb->back = elements_left;
        if (prev_available < element_count)
        {
            // Overwrite the oldest elements.
            p_queue->p_cb->front = nrf_queue_next_idx(p_queue, p_queue->p_cb->back);
        }
    }

    // Update utilization.
    size_t utilization = queue_utilization_get(p_queue);
    if (p_queue->p_cb->max_utilization < utilization)
    {
        p_queue->p_cb->max_utilization = utilization;
    }
}

ret_code_t nrf_queue_write(nrf_queue_t const * p_queue,
                           void const        * p_data,
                           size_t              element_count)
{
    ret_code_t status = NRF_SUCCESS;

    ASSERT(p_queue != NULL);
    ASSERT(p_data != NULL);
    ASSERT(element_count <= p_queue->size);

    if (element_count == 0)
    {
        return NRF_SUCCESS;
    }

    CRITICAL_REGION_ENTER();

    if ((nrf_queue_available_get(p_queue) >= element_count)
     || (p_queue->mode == NRF_QUEUE_MODE_OVERFLOW))
    {
        queue_write(p_queue, p_data, element_count);
    }
    else
    {
        status = NRF_ERROR_NO_MEM;
    }

    CRITICAL_REGION_EXIT();

    NRF_LOG_INST_DEBUG(p_queue->p_log, "Write %d elements (start address: 0x%08X), status:%d",
                                       element_count, p_data, status);
    return status;
}


size_t nrf_queue_in(nrf_queue_t const * p_queue,
                    void const        * p_data,
                    size_t              element_count)
{
    ASSERT(p_queue != NULL);
    ASSERT(p_data != NULL);

    size_t req_element_count = element_count;

    if (element_count == 0)
    {
        return 0;
    }

    CRITICAL_REGION_ENTER();

    if (p_queue->mode == NRF_QUEUE_MODE_OVERFLOW)
    {
        element_count = MIN(element_count, p_queue->size);
    }
    else
    {
        size_t available = nrf_queue_available_get(p_queue);
        element_count    = MIN(element_count, available);
    }

    queue_write(p_queue, p_data, element_count);

    CRITICAL_REGION_EXIT();

    NRF_LOG_INST_DEBUG(p_queue->p_log, "Put in %d elements (start address: 0x%08X), requested :%d",
                                       element_count, p_data, req_element_count);

    return element_count;
}

/**@brief Read elements from the queue. This function assumes that there are enough elements
 *        in the queue to read and that this process will not be interrupted.
 *
 * @param[in]   p_queue             Pointer to the nrf_queue_t instance.
 * @param[out]  p_data              Pointer to the buffer where elements will be copied.
 * @param[in]   element_count       Number of elements to read.
 */
static void queue_read(nrf_queue_t const * p_queue, void * p_data, uint32_t element_count)
{
    size_t front        = p_queue->p_cb->front;
    size_t back         = p_queue->p_cb->back;
    size_t continuous   = (front <= back) ? (back - front) : (p_queue->size + 1 - front);
    void const * p_read_ptr = (void const *)((size_t)p_queue->p_buffer
                                           + front * p_queue->element_size);

    if (element_count <= continuous)
    {
        memcpy(p_data,
               p_read_ptr,
               element_count * p_queue->element_size);

        p_queue->p_cb->front = ((front + element_count) <= p_queue->size)
                             ? (front + element_count)
                             : 0;
    }
    else
    {
        size_t first_read_length = continuous * p_queue->element_size;
        memcpy(p_data,
               p_read_ptr,
               first_read_length);

        size_t elements_left = element_count - continuous;
        memcpy((void *)((size_t)p_data + first_read_length),
               p_queue->p_buffer,
               elements_left * p_queue->element_size);

        p_queue->p_cb->front = elements_left;
    }
}

ret_code_t nrf_queue_read(nrf_queue_t const * p_queue,
                          void              * p_data,
                          size_t              element_count)
{
    ret_code_t status = NRF_SUCCESS;

    ASSERT(p_queue != NULL);
    ASSERT(p_data != NULL);

    if (element_count == 0)
    {
        return NRF_SUCCESS;
    }

    CRITICAL_REGION_ENTER();

    if (element_count <= queue_utilization_get(p_queue))
    {
        queue_read(p_queue, p_data, element_count);
    }
    else
    {
        status = NRF_ERROR_NOT_FOUND;
    }

    CRITICAL_REGION_EXIT();

    NRF_LOG_INST_DEBUG(p_queue->p_log, "Read %d elements (start address: 0x%08X), status :%d",
                                       element_count, p_data, status);
    return status;
}

size_t nrf_queue_out(nrf_queue_t const * p_queue,
                     void              * p_data,
                     size_t              element_count)
{
    ASSERT(p_queue != NULL);
    ASSERT(p_data != NULL);

    size_t req_element_count = element_count;

    if (element_count == 0)
    {
        return 0;
    }

    CRITICAL_REGION_ENTER();

    size_t utilization = queue_utilization_get(p_queue);
    element_count      = MIN(element_count, utilization);

    queue_read(p_queue, p_data, element_count);

    CRITICAL_REGION_EXIT();

    NRF_LOG_INST_DEBUG(p_queue->p_log, "Out %d elements (start address: 0x%08X), requested :%d",
                                       element_count, p_data, req_element_count);
    return element_count;
}

void nrf_queue_reset(nrf_queue_t const * p_queue)
{
    ASSERT(p_queue != NULL);

    CRITICAL_REGION_ENTER();

    memset(p_queue->p_cb, 0, sizeof(nrf_queue_cb_t));

    CRITICAL_REGION_EXIT();

    NRF_LOG_INST_DEBUG(p_queue->p_log, "Reset");
}

size_t nrf_queue_utilization_get(nrf_queue_t const * p_queue)
{
    size_t utilization;
    ASSERT(p_queue != NULL);

    CRITICAL_REGION_ENTER();

    utilization = queue_utilization_get(p_queue);

    CRITICAL_REGION_EXIT();

    return utilization;
}

bool nrf_queue_is_empty(nrf_queue_t const * p_queue)
{
    ASSERT(p_queue != NULL);
    size_t front    = p_queue->p_cb->front;
    size_t back     = p_queue->p_cb->back;
    return (front == back);
}

size_t nrf_queue_available_get(nrf_queue_t const * p_queue)
{
    ASSERT(p_queue != NULL);
    return p_queue->size - nrf_queue_utilization_get(p_queue);
}

size_t nrf_queue_max_utilization_get(nrf_queue_t const * p_queue)
{
    ASSERT(p_queue != NULL);
    return p_queue->p_cb->max_utilization;
}

void nrf_queue_max_utilization_reset(nrf_queue_t const * p_queue)
{
    ASSERT(p_queue != NULL);
    p_queue->p_cb->max_utilization = 0;
}


#endif // NRF_MODULE_ENABLED(NRF_QUEUE)
