/*
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

#ifndef _et_wiced_h_
#define _et_wiced_h_

#include "platform_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* defaults */
#ifndef WICED_ETHERNET_DESCNUM_TX
#define WICED_ETHERNET_DESCNUM_TX 8
#endif
#ifndef WICED_ETHERNET_DESCNUM_RX
#define WICED_ETHERNET_DESCNUM_RX 4
#endif

/* tunables */
#define NTXD                  WICED_ETHERNET_DESCNUM_TX               /* # tx dma ring descriptors (must be ^2) */
#define NRXD                  WICED_ETHERNET_DESCNUM_RX               /* # rx dma ring descriptors (must be ^2) */
#define NRXBUFPOST            MAX((WICED_ETHERNET_DESCNUM_RX) / 2, 1) /* try to keep this # rbufs posted to the chip */
#define RXBUFSZ               WICED_ETHERNET_BUFFER_SIZE              /* receive buffer size */

/* dma tunables */
#ifndef TXMR
#define TXMR                  2 /* number of outstanding reads */
#endif

#ifndef TXPREFTHRESH
#define TXPREFTHRESH          8 /* prefetch threshold */
#endif

#ifndef TXPREFCTL
#define TXPREFCTL            16 /* max descr allowed in prefetch request */
#endif

#ifndef TXBURSTLEN
#define TXBURSTLEN          128 /* burst length for dma reads */
#endif

#ifndef RXPREFTHRESH
#define RXPREFTHRESH          1 /* prefetch threshold */
#endif

#ifndef RXPREFCTL
#define RXPREFCTL             8 /* max descr allowed in prefetch request */
#endif

#ifndef RXBURSTLEN
#define RXBURSTLEN          128 /* burst length for dma writes */
#endif


/****************************** internal **************************/

#define IS_POWER_OF_2(x)          (((x) != 0) && (((x) & (~(x) + 1)) == (x)))


static inline void wiced_osl_static_asserts(void)
{
    STATIC_ASSERT(IS_POWER_OF_2(NTXD));
    STATIC_ASSERT(IS_POWER_OF_2(NRXD));
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _et_wiced_h_ */
