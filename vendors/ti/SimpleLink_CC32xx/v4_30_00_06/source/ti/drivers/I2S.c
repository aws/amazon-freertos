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
/*
 *  ======== I2S.c ========
 */

#include <ti/drivers/I2S.h>

extern const I2S_Config I2S_config[];
extern const uint_least8_t I2S_count;

/* Default I2S parameters structure */
const I2S_Params I2S_defaultParams = {
  .samplingFrequency     = 8000,                              /* Sampling Freq */
  .memorySlotLength      = I2S_MEMORY_LENGTH_16BITS,          /* Memory slot length */
  .moduleRole            = I2S_MASTER,                        /* Master / Slave selection */
  .trueI2sFormat         = (bool)true,                        /* Activate true I2S format */
  .invertWS              = (bool)true,                        /* WS inversion */
  .isMSBFirst            = (bool)true,                        /* Endianness selection */
  .isDMAUnused           = (bool)false,                       /* Selection between DMA and CPU transmissions */
  .samplingEdge          = I2S_SAMPLING_EDGE_RISING,          /* Sampling edge */
  .beforeWordPadding     = 0,                                 /* Before sample padding */
  .bitsPerWord           = 16,                                /* Bits/Sample */
  .afterWordPadding      = 0,                                 /* After sample padding */
  .fixedBufferLength     = 1,                                 /* Fixed Buffer Length */
  .SD0Use                = I2S_SD0_OUTPUT,                    /* SD0Use */
  .SD1Use                = I2S_SD1_INPUT,                     /* SD1Use */
  .SD0Channels           = I2S_CHANNELS_STEREO,               /* Channels activated on SD0 */
  .SD1Channels           = I2S_CHANNELS_STEREO,               /* Channels activated on SD1 */
  .phaseType             = I2S_PHASE_TYPE_DUAL,               /* Phase type */
  .startUpDelay          = 2,                                 /* Start up delay in number of WS periods */
  .MCLKDivider           = 40,                                /* MCLK divider */
  .readCallback          = NULL,                              /* Read callback */
  .writeCallback         = NULL,                              /* Write callback */
  .errorCallback         = NULL,                              /* Error callback */
  .custom                = NULL,                              /* customParams */
};

/*
 *  ======== I2S_Params_init ========
 */
void I2S_Params_init(I2S_Params *params)
{
    *params = I2S_defaultParams;
}

/*
 *  ======== I2S_Transaction_init ========
 */
void I2S_Transaction_init(I2S_Transaction *transaction)
{
    transaction->bufPtr               = NULL;
    transaction->bufSize              = 0;
    transaction->bytesTransferred     = 0;
    transaction->untransferredBytes   = 0;
    transaction->numberOfCompletions  = 0;
    transaction->arg                  = (uintptr_t)NULL;
}
