/*
 * Copyright (c) 2018, Texas Instruments Incorporated
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

#include <ti/drivers/utils/StructRingBuf.h>

#include <ti/drivers/dpl/HwiP.h>

/*
 *  ======== StructRingBuf_construct ========
 */
void StructRingBuf_construct(StructRingBuf_Handle object, void *bufPtr,
    size_t bufSize, size_t structSize)
{
    object->buffer = bufPtr;
    object->length = bufSize;
    object->count = 0;
    object->head = bufSize - 1;
    object->tail = 0;
    object->maxCount = 0;
    object->structSize = structSize;
}

/*
 *  ======== StructRingBuf_get ========
 */
int StructRingBuf_get(StructRingBuf_Handle object, void *data)
{
    unsigned int key;

    key = HwiP_disable();

    if (!object->count) {
        HwiP_restore(key);
        return -1;
    }

    memcpy(data, &object->buffer[object->structSize * object->tail], object->structSize);
    object->tail = (object->tail + 1) % object->length;
    object->count--;

    HwiP_restore(key);

    return (object->count);
}

/*
 *  ======== StructRingBuf_getCount ========
 */
int StructRingBuf_getCount(StructRingBuf_Handle object)
{
    return (object->count);
}

/*
 *  ======== StructRingBuf_isFull ========
 */
bool StructRingBuf_isFull(StructRingBuf_Handle object)
{
    return (object->count == object->length);
}

/*
 *  ======== StructRingBuf_getMaxCount ========
 */
int StructRingBuf_getMaxCount(StructRingBuf_Handle object)
{
    return (object->maxCount);
}

/*
 *  ======== StructRingBuf_peek ========
 */
int StructRingBuf_peek(StructRingBuf_Handle object, void **data)
{
    *data = &object->buffer[object->structSize * object->tail];

    return (object->count);
}

/*
 *  ======== StructRingBuf_put ========
 */
int StructRingBuf_put(StructRingBuf_Handle object, const void *data)
{
    unsigned int key;
    unsigned int next;

    key = HwiP_disable();

    if (object->count != object->length) {
        next = (object->head + 1) % object->length;
        memcpy(&object->buffer[object->structSize * next], data, object->structSize);
        object->head = next;
        object->count++;
        object->maxCount = (object->count > object->maxCount) ?
                            object->count :
                            object->maxCount;
    }
    else {

        HwiP_restore(key);
        return (-1);
    }

    HwiP_restore(key);

    return (object->count);
}
