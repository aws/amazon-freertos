/*
 * Copyright 2019, Cypress Semiconductor Corporation or a subsidiary of
 * Cypress Semiconductor Corporation. All Rights Reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software"), is owned by Cypress Semiconductor Corporation
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


#if defined(AFR) && !defined(HAVE_GETTIMEOFDAY)

#if defined(HAVE_CONFIG_H) && !defined(INCLUDED_CONFIG_H_)
    /* NOTE: config.h doesn't have guard includes! */
    #define INCLUDED_CONFIG_H_
    #include "config.h"
#endif /* defined(HAVE_CONFIG_H) && !defined(INCLUDED_CONFIG_H_) */

#include "headers.h"
#include "gettimeofday.h"
#include <FreeRTOS.h>
#include <task.h>
#include "cy_iperf_rtc.h"

static bool rtc_enabled = false;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int afr_gettimeofday(struct timeval* tv, void* timezone)
{
    if(!rtc_enabled)
    {
        rtc_init();
        rtc_write(0);
        rtc_enabled = true;
    }

    time_t t = (time_t)-1;

    t = rtc_read();

    tv->tv_sec = (long) t;
    tv->tv_usec = (long) ((t - tv->tv_sec) * 1e6);

    return 0;
}

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* defined(AFR) && !defined(HAVE_GETTIMEOFDAY) */