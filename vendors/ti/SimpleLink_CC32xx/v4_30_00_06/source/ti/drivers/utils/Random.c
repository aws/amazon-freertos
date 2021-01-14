/*
 * Copyright (c) 2018-2020 Texas Instruments Incorporated
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
/*
 *  ======== Random.c ========
 */

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/utils/Random.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <ti/devices/DeviceFamily.h>

#if (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X2_CC26X2 || \
     DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X0_CC26X0)

    #include <ti/drivers/TRNG.h>
    #include <ti/drivers/trng/TRNGCC26XX.h>
    #include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>

#elif (DeviceFamily_ID == DeviceFamily_ID_MSP432E401Y || \
       DeviceFamily_ID == DeviceFamily_ID_MSP432E411Y)

    #include DeviceFamily_constructPath(driverlib/inc/hw_sysctl.h)
    #include DeviceFamily_constructPath(driverlib/types.h)

#endif

#define STATE_SIZE_IN_WORDS 5

static uint32_t state[STATE_SIZE_IN_WORDS];

/*
 *  ======== Random_seedAutomatic ========
 */
int_fast16_t Random_seedAutomatic(void) {

#if (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X2_CC26X2 || \
     DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X0_CC26X0)

    TRNGCC26XX_Object object = {0};
    TRNG_Params params;
    CryptoKey seedKey;
    int_fast16_t status;

    /* Use minimum TRNGCC26XX_SAMPLES_PER_CYCLE_MIN since
     * we do not need the full amount of entropy and only need
     * to kickstart the PRNG.
     */
    const TRNGCC26XX_HWAttrs hwAttrs = {
        .samplesPerCycle = TRNGCC26XX_SAMPLES_PER_CYCLE_MIN,
        .intPriority = ~0,
    };

    /* Allocate TRNG instance on the stack since we will not need it
     * hereafter. This also helps avoid problems with hardcoded indexes.
     */
    TRNG_Config config = {
        .object = &object,
        .hwAttrs = &hwAttrs
    };

    params.returnBehavior = TRNG_RETURN_BEHAVIOR_POLLING;

    TRNG_init();

    TRNG_Handle handle = TRNG_construct(&config, &params);

    if (!handle) {
        return Random_STATUS_ERROR;
    }

    CryptoKeyPlaintext_initBlankKey(&seedKey,
                                    (uint8_t *)state,
                                    sizeof(state));

    status = TRNG_generateEntropy(handle, &seedKey);

    TRNG_close(handle);

    if (status != TRNG_STATUS_SUCCESS) {
        return Random_STATUS_ERROR;
    }

    return Random_STATUS_SUCCESS;
#elif (DeviceFamily_ID == DeviceFamily_ID_MSP432E401Y || \
       DeviceFamily_ID == DeviceFamily_ID_MSP432E411Y)

    /* MSP432E4 has a 128-bit unique device ID that we can use */
    state[0] = HWREG(SYSCTL_UNIQUEID0);
    state[1] = HWREG(SYSCTL_UNIQUEID1);
    state[2] = HWREG(SYSCTL_UNIQUEID2);
    state[3] = HWREG(SYSCTL_UNIQUEID3);
    state[4] = 0x00000001;

    return Random_STATUS_SUCCESS;
#else

    /* If neither a TRNG nor a unique ID are available, use a constant */
    state[0] = 0x00000001;
    state[1] = 0x00000002;
    state[2] = 0x00000003;
    state[3] = 0x00000004;
    state[4] = 0x00000005;

    return Random_STATUS_SUCCESS;
#endif
}

/*
 *  ======== Random_seedManual ========
 */
void Random_seedManual(uint8_t seed[Random_SEED_LENGTH]) {
    uintptr_t key;

    key = HwiP_disable();

    memcpy(state, seed, sizeof(state));

    HwiP_restore(key);
}

/*
 *  ======== Random_getNumber ========
 */
uint32_t Random_getNumber(void) {
    uintptr_t key;
    uint32_t s;
    uint32_t v;
    uint32_t result;

    key = HwiP_disable();

    /* "xorwow" XOR shift PRNG from section 3.1 of Marsaglia's "Xorshift RNGs" paper */
    v = state[3];

    v ^= v >> 2;
    v ^= v << 1;

    state[3] = state[2];
    state[2] = state[1];
    state[1] = state[0];
    s = state[0];

    v ^= s;
    v ^= s << 4;

    state[0] = v;

    state[4] += 362437;

    result = v + state[4];

    HwiP_restore(key);

    return result;
}

/*
 *  ======== Random_getBytes ========
 */
void Random_getBytes(void *buffer, size_t bufferSize) {
    uint32_t i;
    uint32_t randomNumber;

    for (i = 0; i < bufferSize / sizeof(uint32_t); i++){
        ((uint32_t *)buffer)[i] = Random_getNumber();
    }

    randomNumber = Random_getNumber();

    memcpy((uint32_t *)buffer + bufferSize / sizeof(uint32_t),
           &randomNumber,
           bufferSize % sizeof(uint32_t));
}
