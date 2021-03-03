/*
 * Amazon FreeRTOS PKCS #11 PAL for Cypress development kit V1.0.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 *
 * This software, associated documentation and materials ("Software")
 * is owned by Cypress Semiconductor Corporation,
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/* The random number generation solution presented in this file is
 * for demonstration purposes only. It is not recommended to go into production with
 * the logic presented here. The current solution takes entropy from WLAN Firmware
 * generated random number and the CPU ticks.
 * For production development, it is recommended to use a source which will be
 * truly random in nature.
 */

/* C runtime includes. */
#include <stdlib.h>
#include <stdbool.h>

#ifdef CY_TFM_PSA_SUPPORTED
#include <psa/crypto.h>
#else
#include "cyhal_trng.h"
#endif

int mbedtls_hardware_poll( void * data,
                           unsigned char * output,
                           size_t len,
                           size_t * olen )
{
#ifndef CY_TFM_PSA_SUPPORTED
    static bool trng_initialized = false;
    static cyhal_trng_t obj;
    if (!trng_initialized)
    {
        // trng init does reference counting. We do not need to call free on it.
        cyhal_trng_init(&obj);
        trng_initialized = true;
    }

    *olen = sizeof(uint32_t);
    if ( (*olen) > len )
    {
        *olen = len;
    }
    uint32_t rand_num = cyhal_trng_generate(&obj);
    memcpy( ( void* ) output, (void*) &rand_num, *olen);

#else /* CY_TFM_PSA_SUPPORTED */
    ((void)(data));

    if (output == NULL || olen == NULL){
        return -1;
    }

    psa_status_t status = psa_crypto_init();
    if(status != PSA_SUCCESS) {
        return -1;
    }

    status = psa_generate_random(output, len);
    if (status != PSA_SUCCESS) {
        return -1;
    }

    *olen = len;
#endif /* CY_TFM_PSA_SUPPORTED */
    return 0;
}


