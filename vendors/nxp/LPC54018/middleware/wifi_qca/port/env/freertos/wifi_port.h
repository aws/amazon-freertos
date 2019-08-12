/*
 * Copyright (c) 2016, NXP Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used tom  endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __WIFI_PORT_H__
#define __WIFI_PORT_H__

/* "wifi_env_port.h" macros can be overloaded here.
 * This file can contain only porting macros, nothing else */

#include "wifi_env_port.h"

extern A_STATUS WIFISHIELD_PowerUp(uint32_t enable);
#define CUSTOM_HW_POWER_UP_DOWN(pCxt, powerup) \
    WIFISHIELD_PowerUp(powerup)

extern A_STATUS WIFIDRVS_SPI_InOutBuffer(uint8_t *pBuffer, uint16_t length, uint8_t doRead, boolean sync);
#define CUSTOM_BUS_INOUT_BUFFER(pCxt, pBuffer, length, doRead, sync) \
    WIFIDRVS_SPI_InOutBuffer((pBuffer), (length), (doRead), (sync))

extern A_STATUS WIFIDRVS_SPI_InOutToken(uint32_t OutToken, uint8_t DataSize, uint32_t *pInToken);
#define CUSTOM_BUS_INOUT_TOKEN(pCxt, oT, dS, pInT) \
    WIFIDRVS_SPI_InOutToken((oT), (dS), (pInT))

#define CUSTOM_BUS_START_TRANSFER(pCxt, sync) (A_OK)

#define CUSTOM_BUS_COMPLETE_TRANSFER(pCxt, sync) (A_OK)

extern A_STATUS WIFISHIELD_InitDrivers(void *param);
#define CUSTOM_HW_INIT(pCxt) WIFISHIELD_InitDrivers((pCxt))

extern A_STATUS WIFISHIELD_DeinitDrivers(void *param);
#define CUSTOM_HW_DEINIT(pCxt) WIFISHIELD_DeinitDrivers((pCxt))

#endif
