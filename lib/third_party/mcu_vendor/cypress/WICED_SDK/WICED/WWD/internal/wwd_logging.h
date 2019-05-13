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
#ifndef INCLUDED_WWD_LOGGING_H_
#define INCLUDED_WWD_LOGGING_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*
#define WWD_LOGGING_STDOUT_ENABLE
*/
/*
#define WWD_LOGGING_BUFFER_ENABLE
*/

#if defined( WWD_LOGGING_STDOUT_ENABLE )

#include <stdio.h>
extern int wwd_logging_enabled;
#define WWD_LOG( x ) if (wwd_logging_enabled) {printf x; }

#elif defined( WWD_LOGGING_BUFFER_ENABLE )
extern int wwd_logging_enabled;
#define LOGGING_BUFFER_SIZE 1024
extern int wwd_logging_printf( const char *format, ...);
extern void wwd_get_logbuffer( uint8_t **bufptr );
#define WWD_LOG( x ) if ( wwd_logging_enabled ) {wwd_logging_printf x; }

/* Enable WICED_LOGBUF for sigma dut debugging, as WWD_LOG in code may
 * be too much of a logging
 */
#define WICED_LOGBUF( x ) //if (wwd_logging_enabled) {wwd_logging_printf x; }

#else /* if defined( WWD_LOGGING_BUFFER_ENABLE ) */

#define WWD_LOG(x)
#define WICED_LOGBUF( x )

#endif /* if defined( WWD_LOGGING_BUFFER_ENABLE ) */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ifndef INCLUDED_WWD_LOGGING_H_ */
