/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
#include <ti/drivers/ITM.h>
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ocp_shared.h)

#define SPARE_REG4_SWV_ON_TDO_S 6
#define SWV_ON_TDO_MASK (~(1<<SPARE_REG4_SWV_ON_TDO_S))

/*
 *  ======== ITM_applyPinMux ========
 */
bool ITM_applyPinMux(void)
{
    /* Mux the SWO functionality out on the TDO pin */
    /* NB! This is not configurable, it must use TDO */
    HWREG(OCP_SHARED_BASE + OCP_SHARED_O_SPARE_REG_4) = ((HWREG(OCP_SHARED_BASE + OCP_SHARED_O_SPARE_REG_4) & SWV_ON_TDO_MASK) | (1<<SPARE_REG4_SWV_ON_TDO_S));

    return (bool)true;
}

/*
 *  ======== ITM_clearPinMux ========
 */
void ITM_clearPinMux(void)
{
    /* Remove Mux of SWO to TDO pin */
    HWREG(OCP_SHARED_BASE + OCP_SHARED_O_SPARE_REG_4) &= (1UL << SPARE_REG4_SWV_ON_TDO_S);
}
