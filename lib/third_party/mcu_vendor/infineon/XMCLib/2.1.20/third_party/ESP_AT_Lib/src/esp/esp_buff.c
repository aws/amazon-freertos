/**	
 * \file            esp_buff.c
 * \brief           buff manager
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
#include "esp/esp_buff.h"
#include "esp/esp_mem.h"

#ifndef ESP_CFG_DBG_BUFF
#define ESP_CFG_DBG_BUFF        ESP_CFG_DBG_OFF
#endif

/**
 * \brief           Initialize buffer
 * \param[in]       buff: Pointer to buffer structure
 * \param[in]       size: Size of buffer. This parameter must match length of memory used on memory param
 * \return          `1` on success, `0` otherwise
 */
uint8_t
esp_buff_init(esp_buff_t* buff, size_t size) {
    if (buff == NULL) {                         /* Check buffer structure */
        return 0;
    }
    ESP_MEMSET(buff, 0, sizeof(*buff));         /* Set buffer values to all zeros */

    buff->size = size;                          /* Set default values */
    buff->buff = esp_mem_alloc(size);           /* Allocate memory for buffer */
    if (buff->buff == NULL) {                   /* Check allocation */
        return 0;
    }

    return 1;                                   /* Initialized OK */
}

/**
 * \brief           Free dynamic allocation if used on memory
 * \param[in]       buff: Pointer to buffer structure
 */
void
esp_buff_free(esp_buff_t* buff) {
    if (buff != NULL) {
        esp_mem_free(buff->buff);               /* Free memory */
        buff->buff = NULL;
    }
}

/**
 * \brief           Write data to buffer
 * \param[in]       buff: Pointer to buffer structure
 * \param[in]       data: Pointer to data to copy memory from
 * \param[in]       count: Number of bytes we want to write
 * \return          Number of bytes actually written to buffer
 */
size_t
esp_buff_write(esp_buff_t* buff, const void* data, size_t count) {
	size_t i = 0;
	size_t free;
    const uint8_t* d = data;
    size_t tocopy;

    if (buff == NULL || count == 0) {           /* Check buffer structure */
        return 0;
    }
    if (buff->in >= buff->size) {               /* Check input pointer */
        buff->in = 0;
    }
    free = esp_buff_get_free(buff);             /* Get free memory */
    if (free < count) {                         /* Check available memory */	
        if (free == 0) {                        /* If no memory, stop execution */
            return 0;
        }
        count = free;                           /* Set values for write */
    }

    /* We have calculated memory for write */
    tocopy = buff->size - buff->in;             /* Calculate number of elements we can put at the end of buffer */
    if (tocopy > count) {                       /* Check for copy count */
        tocopy = count;
    }
    ESP_MEMCPY(&buff->buff[buff->in], d, tocopy);   /* Copy content to buffer */
    i += tocopy;                                /* Increase number of bytes we copied already */
    buff->in += tocopy;	
    count -= tocopy;
    if (count > 0) {                            /* Check if anything to write */	
        ESP_MEMCPY(buff->buff, (void *)&d[i], count);   /* Copy content */
        buff->in = count;                       /* Set input pointer */
    }
    if (buff->in >= buff->size) {               /* Check input overflow */
        buff->in = 0;
    }
    return (i + count);                         /* Return number of elements stored in memory */
}

/**
 * \brief           Read data from buffer
 * \param[in]       buff: Pointer to buffer structure
 * \param[out]      data: Pointer to data to copy memory to
 * \param[in]       count: Number of bytes we want to read
 * \return          Number of bytes actually read and saved to data variable
 */
size_t
esp_buff_read(esp_buff_t* buff, void* data, size_t count) {
    uint8_t *d = data;
    size_t i = 0, full;
    size_t tocopy;

    if (buff == NULL || count == 0) {           /* Check buffer structure */
        return 0;
    }
    if (buff->out >= buff->size) {              /* Check output pointer */
        buff->out = 0;
    }
    full = esp_buff_get_full(buff);             /* Get free memory */
    if (full < count) {                         /* Check available memory */
        if (full == 0) {                        /* If no memory, stop execution */
            return 0;
        }
        count = full;                           /* Set values for write */
    }

    tocopy = buff->size - buff->out;            /* Calculate number of elements we can read from end of buffer */
    if (tocopy > count) {                       /* Check for copy count */
        tocopy = count;
    }
    ESP_MEMCPY(d, &buff->buff[buff->out], tocopy);  /* Copy content from buffer */
    i += tocopy;                                /* Increase number of bytes we copied already */
    buff->out += tocopy;
    count -= tocopy;
    if (count > 0) {                            /* Check if anything to read */
        ESP_MEMCPY(&d[i], buff->buff, count);   /* Copy content */
        buff->out = count;                      /* Set input pointer */
    }
    if (buff->out >= buff->size) {              /* Check output overflow */
        buff->out = 0;
    }
    return i + count;                           /* Return number of elements stored in memory */
}

/**
 * \brief           Read from buffer but do not change read and write pointers
 * \param[in]       buff: Pointer to buffer structure
 * \param[in]       skip_count: Number of bytes to skip before reading peek data
 * \param[out]      data: Pointer to data to save read memory
 * \param[in]       count: Number of bytes to peek
 * \return          Number of bytes written to data array
 */
size_t
esp_buff_peek(esp_buff_t* buff, size_t skip_count, void* data, size_t count) {
    uint8_t *d = data;
    size_t i = 0, full, tocopy, out;

    if (buff == NULL || count == 0) {           /* Check buffer structure */
        return 0;
    }
    out = buff->out;
    if (buff->out >= buff->size) {              /* Check output pointer */
        buff->out = 0;
    }
    full = esp_buff_get_full(buff);             /* Get free memory */
    if (skip_count >= full) {                   /* We cannot skip for more than we have in buffer */
        return 0;
    }
    out += skip_count;                          /* Skip buffer data */
    full -= skip_count;                         /* Effective full is less than before */
    if (out >= buff->size) {                    /* Check overflow */
        out -= buff->size;                      /* Go to beginning */
    }
    if (full < count) {                         /* Check available memory */
        if (full == 0) {                        /* If no memory, stop execution */
            return 0;
        }
        count = full;                           /* Set values for write */
    }

    tocopy = buff->size - out;                  /* Calculate number of elements we can read from end of buffer */
    if (tocopy > count) {                       /* Check for copy count */
        tocopy = count;
    }
    ESP_MEMCPY(d, &buff->buff[out], tocopy);    /* Copy content from buffer */
    i += tocopy;                                /* Increase number of bytes we copied already */
    count -= tocopy;
    if (count > 0) {                            /* Check if anything to read */
        ESP_MEMCPY(&d[i], buff->buff, count);   /* Copy content */
    }
    return i + count;                           /* Return number of elements stored in memory */
}

/**
 * \brief           Get length of free space
 * \param[in]       buff: Pointer to buffer structure
 * \return          Number of free bytes in memory
 */
size_t
esp_buff_get_free(esp_buff_t* buff) {
    size_t size, in, out;

    if (buff == NULL) {                         /* Check buffer structure */
        return 0;
    }
    in = buff->in;                              /* Save values */
    out = buff->out;
    if (in == out) {                            /* Check if the same */
        size = buff->size;
    } else if (out > in) {                      /* Check normal mode */
        size = out - in;
    } else {                                    /* Check if overflow mode */
        size = buff->size - (in - out);
    }
    return size - 1;                            /* Return free memory */
}

/**
 * \brief           Get length of buffer currently being used
 * \param[in]       buff: Pointer to buffer structure
 * \return          Number of bytes ready to be read
 */
size_t
esp_buff_get_full(esp_buff_t* buff) {
    size_t in, out, size;

    if (buff == NULL) {                         /* Check buffer structure */
        return 0;
    }
    in = buff->in;                              /* Save values */
    out = buff->out;
    if (in == out) {                            /* Pointer are same? */
        size = 0;
    } else if (in > out) {                      /* buff is not in overflow mode */
        size = in - out;
    } else {                                    /* buff is in overflow mode */
        size = buff->size - (out - in);
    }
    return size;                                /* Return number of elements in buffer */
}

/**
 * \brief           Resets and clears buffer
 * \param[in]       buff: Pointer to buffer structure
 */
void
esp_buff_reset(esp_buff_t* buff) {
	if (buff == NULL) {                         /* Check buffer structure */
		return;
	}
	buff->in = 0;                               /* Reset values */
	buff->out = 0;
}

/**
 * \brief           Get linear address for buffer for fast read
 * \param[in]       buff: Pointer to buffer
 * \return          Pointer to start of linear address
 */
void *
esp_buff_get_linear_block_address(esp_buff_t* buff) {
    return &buff->buff[buff->out];              /* Return read address */
}

/**
 * \brief           Get length of linear block address before it overflows
 * \param[in]       buff: Pointer to buffer
 * \return          Length of linear address
 */
size_t
esp_buff_get_linear_block_length(esp_buff_t* buff) {
    size_t len;
    if (buff->in > buff->out) {
        len = buff->in - buff->out;
    } else if (buff->out > buff->in) {
        len = buff->size - buff->out;
    } else {
        len = 0;
    }
    return len;
}

/**
 * \brief           Skip (ignore) buffer data.
 * \note            Useful at the end of streaming transfer such as DMA
 * \param[in]       buff: Pointer to buffer structure
 * \param[in]       len: Length of bytes we want to skip
 * \return          Number of bytes skipped
 */
size_t
esp_buff_skip(esp_buff_t* buff, size_t len) {
    size_t full;
    full = esp_buff_get_full(buff);             /* Get buffer used length */
    if (len > full) {
        len = full;
    }
    buff->out += len;                           /* Advance buffer */
    if (buff->out >= buff->size) {              /* Subtract possible overflow */
        buff->out -= buff->size;                /* Do subtract */
    }
    return len;
}
