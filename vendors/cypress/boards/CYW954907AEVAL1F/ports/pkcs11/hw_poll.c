/*
 * Amazon FreeRTOS PKCS #11 PAL for Cypress CYW954907AEVAL1F development kit V1.0.1
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

/* Amazon FreeRTOS Includes. */
#include "FreeRTOS.h"

/* C runtime includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WLAN_FIRMWARE_PRNG_SEED
/* WICED includes */
#include "wiced_crypto.h"
#endif

#ifndef WLAN_FIRMWARE_PRNG_SEED
extern uint32_t ulSeedValid;
extern uint32_t ulSeed;
static uint32_t ulPrgnSeedDone = 0;
#endif

int mbedtls_hardware_poll( void * data,
                           unsigned char * output,
                           size_t len,
                           size_t * olen )
{

#ifdef WLAN_FIRMWARE_PRNG_SEED
    /* Enabling WLAN_FIRMWARE_PRNG_SEED in app's make file will
     * generate random number using WELL512 RNG. The initial seed
     * is the combination of  random number generated from WLAN firmware
     * and cpu tick count. WLAN FW generates random number using its high
     * frequency clock. And on system reboot the FW's clock starts from  zero.
     * Same random number sequence can be generated only if both WLAN FW's clock count
     * and CPU tick count matches with the different boots.
	 */
    wiced_crypto_get_random( output, len);
    *olen = len;
#else
    /* random number is generated using rand() function. Initial seed
     * is network latency.
     */
    if( ( ulPrgnSeedDone == 0 ) && ( ulSeedValid != 0 ) )
    {
        srand(ulSeed);
        ulPrgnSeedDone = 1;
    }
    *olen = sizeof(int);
    if ( (*olen) > len )
    {
        *olen = len;
    }
    int rand_num = rand();
    unsigned char *pucTemp = ( unsigned char * )&rand_num;
    memcpy( ( void* ) output, (void*) pucTemp, *olen);
#endif
    return 0;
}



