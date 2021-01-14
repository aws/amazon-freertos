/*
 * Copyright (c) 2019-2020, Texas Instruments Incorporated
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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <ti/drivers/cryptoutils/utils/CryptoUtils.h>

/*
 *  ======== CryptoUtils_buffersMatch ========
 */
bool CryptoUtils_buffersMatch(const void *buffer0,
                              const void *buffer1,
                              size_t bufferByteLength) {
    uint8_t tempResult = 0;
    size_t i;

    /* XOR each byte of the buffer together and OR the results.
     * If the OR'd result is non-zero, the buffers do not match.
     * There is no branch based on the content of the buffers here to avoid
     * timing attacks.
     */
    for (i = 0; i < bufferByteLength; i++) {
        tempResult |= ((uint8_t *)buffer0)[i] ^ ((uint8_t *)buffer1)[i];
    }

    return tempResult == 0 ? true : false;
}

/*
 *  ======== CryptoUtils_buffersMatchWordAligned ========
 */
bool CryptoUtils_buffersMatchWordAligned(const uint32_t *buffer0,
                                         const uint32_t *buffer1,
                                         size_t bufferByteLength) {
    uint32_t tempResult = 0;
    size_t i;

    /* We could skip the branch and just set tempResult equal to the
     * statement below for the same effect but this is more explicit.
     */
    if (bufferByteLength % sizeof(uint32_t) != 0) {
        return false;
    }

    /* XOR each 32-bit word of the buffer together and OR the results.
     * If the OR'd result is non-zero, the buffers do not match.
     * There is no branch based on the content of the buffers here to avoid
     * timing attacks.
     */
    for (i = 0; i < bufferByteLength / sizeof(uint32_t); i++) {
        tempResult |= buffer0[i] ^ buffer1[i];
    }

    return tempResult == 0 ? true : false;
}

/*
 *  ======== CryptoUtils_reverseBufferBytewise ========
 */
void CryptoUtils_reverseBufferBytewise(void * buffer, size_t bufferByteLength) {
    uint8_t *bufferLow = buffer;
    uint8_t *bufferHigh = bufferLow + bufferByteLength - 1;
    uint8_t tmp;

    while (bufferLow < bufferHigh) {
        tmp = *bufferLow;
        *bufferLow = *bufferHigh;
        *bufferHigh = tmp;
        bufferLow++;
        bufferHigh--;
    }
}

/*
 *  ======== CryptoUtils_isBufferAllZeros ========
 */
bool CryptoUtils_isBufferAllZeros(const void *buffer, size_t bufferByteLength) {
    uint32_t i;
    uint8_t bufferBits = 0;

    for (i = 0; i < bufferByteLength; i++) {
        bufferBits |= ((uint8_t *)buffer)[i];
    }

    return bufferBits == 0;
}

/*
 *  ======== CryptoUtils_copyPad ========
 */
void CryptoUtils_copyPad(const void *source,
                         void *destination,
                         size_t sourceLength) {
    uint32_t i;
    uint8_t remainder;
    uint32_t temp;
    uint8_t *tempBytePointer;
    const uint8_t *sourceBytePointer;

    remainder = sourceLength % sizeof(uint32_t);
    temp = 0;
    tempBytePointer = (uint8_t *)&temp;
    sourceBytePointer = (uint8_t *)source;

    /* Copy source to destination starting at the end of source and the
     * beginning of destination.
     * We assemble each word in normal order and write one word at a
     * time since the PKA_RAM requires word-aligned reads and writes.
     */

    for (i = 0; i < sourceLength / sizeof(uint32_t); i++) {
            uint32_t sourceOffset = sizeof(uint32_t) * i;

            tempBytePointer[0] = sourceBytePointer[sourceOffset + 0];
            tempBytePointer[1] = sourceBytePointer[sourceOffset + 1];
            tempBytePointer[2] = sourceBytePointer[sourceOffset + 2];
            tempBytePointer[3] = sourceBytePointer[sourceOffset + 3];

            *((uint32_t *)destination + i) = temp;
    }

    /* Reset to 0 so we do not have to zero-out individual bytes */
    temp = 0;

    /* If sourceLength is not a word-multiple, we need to copy over the
     * remaining bytes and zero pad the word we are writing to PKA_RAM.
     */
    if (remainder == 1) {

        tempBytePointer[0] = sourceBytePointer[0];

        /* i is reused from the loop above. This write  zero-pads the
         * destination buffer to word-length.
         */
        *((uint32_t *)destination + i) = temp;
    }
    else if (remainder == 2) {

        tempBytePointer[0] = sourceBytePointer[0];
        tempBytePointer[1] = sourceBytePointer[1];

       *((uint32_t *)destination + i) = temp;
    }
    else if (remainder == 3) {

        tempBytePointer[0] = sourceBytePointer[0];
        tempBytePointer[1] = sourceBytePointer[1];
        tempBytePointer[2] = sourceBytePointer[2];

        *((uint32_t *)destination + i) = temp;
    }

}

/*
 *  ======== CryptoUtils_reverseCopyPad ========
 */
void CryptoUtils_reverseCopyPad(const void *source,
                                void *destination,
                                size_t sourceLength) {
    uint32_t i;
    uint8_t remainder;
    uint32_t temp;
    uint8_t *tempBytePointer;
    const uint8_t *sourceBytePointer;

    remainder = sourceLength % sizeof(uint32_t);
    temp = 0;
    tempBytePointer = (uint8_t *)&temp;
    sourceBytePointer = (uint8_t *)source;

    /* Copy source to destination starting at the end of source and the
     * beginning of destination.
     * We assemble each word in byte-reversed order and write one word at a
     * time since the PKA_RAM requires word-aligned reads and writes.
     */

    for (i = 0; i < sourceLength / sizeof(uint32_t); i++) {
            uint32_t sourceOffset = sourceLength - 1 - sizeof(uint32_t) * i;

            tempBytePointer[3] = sourceBytePointer[sourceOffset - 3];
            tempBytePointer[2] = sourceBytePointer[sourceOffset - 2];
            tempBytePointer[1] = sourceBytePointer[sourceOffset - 1];
            tempBytePointer[0] = sourceBytePointer[sourceOffset - 0];

            *((uint32_t *)destination + i) = temp;
    }

    /* Reset to 0 so we do not have to zero-out individual bytes */
    temp = 0;

    /* If sourceLength is not a word-multiple, we need to copy over the
     * remaining bytes and zero pad the word we are writing to PKA_RAM.
     */
    if (remainder == 1) {

        tempBytePointer[0] = sourceBytePointer[0];

        /* i is reused from the loop above. This write  zero-pads the
         * destination buffer to word-length.
         */
        *((uint32_t *)destination + i) = temp;
    }
    else if (remainder == 2) {

        tempBytePointer[0] = sourceBytePointer[1];
        tempBytePointer[1] = sourceBytePointer[0];

       *((uint32_t *)destination + i) = temp;
    }
    else if (remainder == 3) {

        tempBytePointer[0] = sourceBytePointer[2];
        tempBytePointer[1] = sourceBytePointer[1];
        tempBytePointer[2] = sourceBytePointer[0];

        *((uint32_t *)destination + i) = temp;
    }

}
