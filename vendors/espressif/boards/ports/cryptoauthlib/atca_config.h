/**
 * \file
 * \brief Cryptoauthlib Configuration Defines
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#ifndef _ATCA_CONFIG_H
#define _ATCA_CONFIG_H

/** Use I2C */
#define ATCA_HAL_I2C

/* ATCA_ATECC608A_SUPPORT is not defined so that both
 *  ATECC608A and ATECC608B can be supported */
#define ATCA_ATECC608_SUPPORT

/** Use the following address for ECC devices */
#define ATCA_I2C_ECC_ADDRESS            0x6C
#define ATCA_DEVICE_PRIVATE_KEY_SLOT    0x00

#ifndef ATCA_POST_DELAY_MSEC
    #define ATCA_POST_DELAY_MSEC        25
#endif

#define PKCS11_LABEL_IS_SERNUM          1

/** Use RTOS timers (i.e. delays that yield) */
#define ATCA_USE_RTOS_TIMER             1

#define atca_delay_ms                   hal_rtos_delay_ms
#define atca_delay_us                   hal_delay_us

#include "FreeRTOS.h"

#define ATCA_PLATFORM_MALLOC            pvPortMalloc
#define ATCA_PLATFORM_FREE              vPortFree

#endif /* _ATCA_CONFIG_H */
