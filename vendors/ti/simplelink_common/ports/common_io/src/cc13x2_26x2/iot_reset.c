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

/**
 * @file    iot_reset.c
 * @brief   This file contains the definitions of RESET APIs using TI drivers and driverlib.
 */
#include <stdint.h>

/* Common IO Header */
#include <iot_reset.h>

/* TI Drivers */
#include <ti/drivers/Power.h>

/* TI DriverLib */
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)

void iot_reset_reboot( IotResetBootFlag_t xResetBootFlag )
{
    /* Only type of reset supported in the CC13x2/CC26x2 devices */
    SysCtrlSystemReset();
}

/*-----------------------------------------------------------*/

int32_t iot_reset_shutdown( void )
{
    Power_shutdown( 0, 0 );

    /* We should never get here */
    return IOT_RESET_INVALID_VALUE;
}

/*-----------------------------------------------------------*/

int32_t iot_get_reset_reason( IotResetReason_t * xResetReason )
{
    int32_t ret = IOT_RESET_INVALID_VALUE;

    if( NULL == xResetReason )
    {
        uint32_t ulReason = SysCtrlResetSourceGet();

        /* Best effort mapping of device reset reasons to IotResetReason_t enum */
        switch( ulReason )
        {
            case RSTSRC_PWR_ON:
                *xResetReason = eResetPowerOnBoot;
                break;

            case RSTSRC_VDDS_LOSS:
            case RSTSRC_VDDR_LOSS:
                *xResetReason = eResetBrownOut;
                break;

            case RSTSRC_CLK_LOSS:
                *xResetReason = eResetOther;
                break;

            case RSTSRC_PIN_RESET:
            case RSTSRC_SYSRESET:
                *xResetReason = eResetColdBoot;
                break;

            case RSTSRC_WARMRESET:
                *xResetReason = eResetWarmBoot;
                break;

            case RSTSRC_WAKEUP_FROM_SHUTDOWN:
                *xResetReason = eResetPowerOnBoot;
                break;

            case RSTSRC_WAKEUP_FROM_TCK_NOISE:
            default:
                *xResetReason = eResetOther;
        }

        ret = IOT_RESET_SUCCESS;
    }

    return ret;
}
