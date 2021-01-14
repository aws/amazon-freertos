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
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/utils/RingBuf.h>

/*
 *  ======== RingBuf_construct ========
 */
void RingBuf_construct(RingBuf_Handle object, unsigned char *bufPtr,
    size_t bufSize)
{
    object->buffer = bufPtr;
    object->length = bufSize;
    object->count = 0;
    object->head = bufSize - 1;
    object->tail = 0;
    object->maxCount = 0;
}

/*
 *  ======== RingBuf_get ========
 */
int RingBuf_get(RingBuf_Handle object, unsigned char *data)
{
    unsigned int key;

    key = HwiP_disable();

    if (!object->count) {
        HwiP_restore(key);
        return -1;
    }

    *data = object->buffer[object->tail];
    object->tail = (object->tail + 1) % object->length;
    object->count--;

    HwiP_restore(key);

    return (object->count);
}

/*
 *  ======== RingBuf_getConsume ========
 */
int RingBuf_getConsume(RingBuf_Handle object, size_t size)
{
    unsigned int key;

    key = HwiP_disable();

    if (object->count < size) {
        size = object->count;
    }
    object->count -= size;
    object->tail += size;
    object->tail %= object->length;

    HwiP_restore(key);

    return (size);
}

/*
 *  ======== RingBuf_getCount ========
 */
int RingBuf_getCount(RingBuf_Handle object)
{
    return (object->count);
}

/*
 *  ======== RingBuf_getMaxCount ========
 */
int RingBuf_getMaxCount(RingBuf_Handle object)
{
    return (object->maxCount);
}

/*
 *  ======== RingBuf_getn ========
 */
int RingBuf_getn(RingBuf_Handle object, unsigned char *data, size_t n)
{
    unsigned int key;
    size_t       removed = 0;

    key = HwiP_disable();
    if (n > object->count) {
        n = object->count;
    }
    while (n) {
        *data++ = object->buffer[object->tail++];
        object->tail %= object->length;
        --object->count;
        --n;
        ++removed;
    }
    HwiP_restore(key);

    return (removed);
}

/*
 *  ======== RingBuf_getPointer ========
 */
int RingBuf_getPointer(RingBuf_Handle object, unsigned char **data)
{
    size_t       result;
    unsigned int key;

    key = HwiP_disable();

    result = object->length - object->tail;
    if (object->count < result) {
        result = object->count;
    }
    *data = object->buffer + object->tail;

    HwiP_restore(key);

    return (result);
}

/*
 *  ======== RingBuf_flush ========
 */
void RingBuf_flush(RingBuf_Handle object)
{
    uintptr_t key;
    size_t    maxCount;

    key = HwiP_disable();
    maxCount = object->maxCount;
    RingBuf_construct(object, object->buffer, object->length);
    object->maxCount = maxCount;
    HwiP_restore(key);
}

/*
 *  ======== RingBuf_isFull ========
 */
bool RingBuf_isFull(RingBuf_Handle object)
{
    return (object->count == object->length);
}

/*
 *  ======== RingBuf_peek ========
 */
int RingBuf_peek(RingBuf_Handle object, unsigned char *data)
{
    unsigned int key;
    int          retCount;

    key = HwiP_disable();

    *data = object->buffer[object->tail];
    retCount = object->count;

    HwiP_restore(key);

    return (retCount);
}

/*
 *  ======== RingBuf_put ========
 */
int RingBuf_put(RingBuf_Handle object, unsigned char data)
{
    unsigned int key;
    unsigned int next;

    key = HwiP_disable();

    if (object->count != object->length) {
        next = (object->head + 1) % object->length;
        object->buffer[next] = data;
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

/*
 *  ======== RingBuf_putAdvance ========
 */
int RingBuf_putAdvance(RingBuf_Handle object, size_t size)
{
    unsigned int key;

    key = HwiP_disable();

    if (RingBuf_space(object) < size) {
        size = RingBuf_space(object);
    }
    object->count += size;
    object->head += size;
    object->head %= object->length;

    object->maxCount = (object->count > object->maxCount) ?
            object->count : object->maxCount;

    HwiP_restore(key);

    return (size);
}

/*
 *  ======== RingBuf_putn ========
 */
int RingBuf_putn(RingBuf_Handle object, unsigned char *data, size_t n)
{
    unsigned int key;
    size_t       next;
    size_t       added = 0;

    key = HwiP_disable();
    if (n > RingBuf_space(object)) {
        n = RingBuf_space(object);
    }
    while (n) {
        next = (object->head + 1) % object->length;
        object->buffer[next] = *data++;
        object->head = next;
        ++object->count;
        --n;
        ++added;
    }
    if (object->maxCount < object->count) {
        object->maxCount = object->count;
    }
    HwiP_restore(key);

    return (added);
}

/*
 *  ======== RingBuf_putPointer ========
 */
int RingBuf_putPointer(RingBuf_Handle object, unsigned char **data)
{
    size_t       result;
    size_t       next;
    unsigned int key;

    key = HwiP_disable();

    next = (object->head + 1) % object->length;
    result = object->length - next;
    if (RingBuf_space(object) < result) {
        result = RingBuf_space(object);
    }
    *data = object->buffer + next;

    HwiP_restore(key);

    return (result);
}
