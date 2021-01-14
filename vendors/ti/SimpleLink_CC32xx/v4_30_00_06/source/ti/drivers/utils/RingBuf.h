/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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

#ifndef ti_drivers_utils_RingBuf__include
#define ti_drivers_utils_RingBuf__include

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned char      *buffer;
    size_t              length;
    size_t              count;
    size_t              head;
    size_t              tail;
    size_t              maxCount;
} RingBuf_Object, *RingBuf_Handle;

/*!
 *  @brief  Initialize circular buffer
 *
 *  @param  object  Pointer to a RingBuf Object that contains the member
 *                  variables to operate a circular buffer.
 *
 *  @param  bufPtr  Pointer to data buffer to be used for the circular buffer.
 *                  The buffer is NOT stored in RingBuf_Object.
 *
 *  @param  bufSize The size of bufPtr in number of unsigned chars.
 */
void RingBuf_construct(RingBuf_Handle object, unsigned char *bufPtr,
    size_t bufSize);

/*!
 *  @brief  Flush all the data from the buffer.
 *
 *  @param  object  Pointer to a RingBuf Object that contains the member
 *                  variables to operate a circular buffer.
 */
void RingBuf_flush(RingBuf_Handle object);

/*!
 *  @brief  Get an unsigned char from the end of the circular buffer and remove
 *          it.
 *
 *  @param  object  Pointer to a RingBuf Object that contains the member
 *                  variables to operate a circular buffer.
 *
 *  @param  data    Pointer to an unsigned char to be filled with the data from
 *                  the front of the circular buffer.
 *
 *  @return         Number of unsigned chars on the buffer after taking it out
 *                  of the circular buffer. If it returns -1, the circular
 *                  buffer was already empty and data is invalid.
 */
int RingBuf_get(RingBuf_Handle object, unsigned char *data);

/*!
 *  @brief  Advance the get index and decrement the buffer count. This function
 *          should normally be called from a context where HWI is disabled. For
 *          efficiency, it is incumbent on the caller to ensure mutual exclusion
 *          with the proper HWI lock.
 *
 *  @param  object  Pointer to a RingBuf Object that contains the member
 *                  variables to operate a circular buffer.
 *
 *  @param size     Number of unsigned characters to advance the get index.
 *
 *  @return         Number of unsigned chars that we were able to be advanced.
 */
int RingBuf_getConsume(RingBuf_Handle object, size_t size);

/*!
 *  @brief  Get the number of unsigned chars currently stored on the circular
 *          buffer.
 *
 *  @param  object  Pointer to a RingBuf Object that contains the member
 *          variables to operate a circular buffer.
 *
 *  @return         Number of unsigned chars on the circular buffer.
 */
int RingBuf_getCount(RingBuf_Handle object);

/*!
 *  @brief  A high-water mark indicating the largest number of unsigned chars
 *          stored on the circular buffer since it was constructed.
 *
 *  @return         Get the largest number of unsigned chars that were at one
 *                  point in the circular buffer.
 */
int RingBuf_getMaxCount(RingBuf_Handle object);

/*!
 *  @brief  Get one or more unsigned chars from the end of the circular buffer and
 *          remove them.
 *
 *  @param  object  Pointer to a RingBuf Object that contains the member
 *                  variables to operate a circular buffer.
 *
 *  @param  data    Pointer to an unsigned char to be filled with the data from
 *                  the front of the circular buffer.
 *
 *  @param  n       number of unsigned chars to try and remove.
 *
 *  @return         Number of unsigned chars successfully removed from the buffer
 *                  and copied into \a data. May be 0 or less than \a n.
 */
int RingBuf_getn(RingBuf_Handle object, unsigned char *data, size_t n);

/*!
 *  @brief  Get a pointer reference to the next chunk of linear memory available for
 *          accessing data in the buffer. This function should
 *          normally be called from a context where HWI is disabled. For
 *          efficiency, it is incumbent on the caller to ensure mutual
 *          exclusion with the proper HWI lock.
 *
 *  @param  object  Pointer to a RingBuf Object that contains the member
 *                  variables to operate a circular buffer.
 *
 *  @param  data    Reference to a pointer to set for the memory location in the
 *                  buffer where data can accessed.
 *
 *  @return         Number of unsigned chars in linear memory where data
 *                  can be accessed, or 0 if it's empty.
 */
int RingBuf_getPointer(RingBuf_Handle object, unsigned char **data);

/*!
 *  @brief  Function to determine if the circular buffer is full or not.
 *
 *  @param  object  Pointer to a RingBuf Object that contains the member
 *                  variables to operate a circular buffer.
 *
 *  @return         true if circular buffer is full, else false.
 */
bool RingBuf_isFull(RingBuf_Handle object);

/*!
 *  @brief  Get an unsigned char from the end of the circular buffer without
 *          removing it.
 *
 *  @param  object  Pointer to a RingBuf Object that contains the member
 *                  variables to operate a circular buffer.
 *
 *  @param  data    Pointer to an unsigned char to be filled with the data from
 *                  the front of the circular buffer. This function does not
 *                  remove the data from the circular buffer. Do not evaluate
 *                  data if the count returned is equal to 0.
 *
 *  @return         Number of unsigned chars on the circular buffer. If the
 *                  number != 0, then data will contain the unsigned char at the
 *                  end of the circular buffer.
 */
int RingBuf_peek(RingBuf_Handle object, unsigned char *data);

/*!
 *  @brief  Put an unsigned char into the end of the circular buffer.
 *
 *  @param  object  Pointer to a RingBuf Object that contains the member
 *                  variables to operate a circular buffer.
 *
 *  @param  data    unsigned char to be placed at the end of the circular
 *                  buffer.
 *
 *  @return         Number of unsigned chars on the buffer after it was added,
 *                  or -1 if it's already full.
 */
int RingBuf_put(RingBuf_Handle object, unsigned char data);

/*!
 *  @brief  Advance the committed put index and increment the buffer count. This
 *          function should normally be called from a context where HWI is disabled.
 *          For efficiency, it is incumbent on the caller to ensure mutual
 *          exclusion with the proper HWI lock.
 *
 *  @param  object  Pointer to a RingBuf Object that contains the member
 *                  variables to operate a circular buffer.
 *
 *  @param size     Number of unsigned characters to commit to the put index.
 *
 *  @return         Number of unsigned chars that we were able to be committed.
 */
int RingBuf_putAdvance(RingBuf_Handle object, size_t size);

/*!
 *  @brief  Put one or more unsigned chars into the end of the circular buffer.
 *
 *  @param  object  Pointer to a RingBuf Object that contains the member
 *                  variables to operate a circular buffer.
 *
 *  @param  data    unsigned chars to be placed at the end of the circular
 *                  buffer.
 *
 *  @param  n       number of unsigned chars to try and remove.
 *
 *  @return         Number of unsigned chars placed into the buffer. May be 0 or
 *                  less than \a n.
 */
int RingBuf_putn(RingBuf_Handle object, unsigned char *data, size_t n);

/*!
 *  @brief  Get a pointer reference to the next chunk of linear memory available for
 *          adding data to the buffer. This function should normally be called from
 *          a context where HWI is disabled. For efficiency, it is incumbent on the
 *          caller to ensure mutual exclusion with the proper HWI lock.
 *
 *  @param  object  Pointer to a RingBuf Object that contains the member
 *                  variables to operate a circular buffer.
 *
 *  @param  data    Reference to a pointer to set for the memory location in the
 *                  buffer where more data can be added.
 *
 *  @return         Number of unsigned chars available in linear memory where data
 *                  can be added, or 0 if it's already full.
 */
int RingBuf_putPointer(RingBuf_Handle object, unsigned char **data);

/*!
 *  @brief  Return the number of unsigned characters that the buffer has space for.
 *          This function should normally be called from a context where HWI is
 *          disabled. For efficiency, it is incumbent on the caller to ensure mutual
 *          exclusion with the proper HWI lock.
 *
 *  @param  object  Pointer to a RingBuf Object that contains the member
 *                  variables to operate a circular buffer.
 *
 *  @return         Number of unsigned chars that the buffer has space for.
 */
static inline size_t RingBuf_space(RingBuf_Handle object)
{
    return (object->length - object->count);
}


#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_utils_RingBuf__include */
