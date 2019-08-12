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
#ifndef _INCLUDED_CRC_H_
#define _INCLUDED_CRC_H_

#include <stdint.h>
#include "platform_toolchain.h"

#ifdef __cplusplus
extern "C" {
#endif

/* crc defines */
#define CRC8_INIT_VALUE  0xff       /* Initial CRC8 checksum value */
#define CRC16_INIT_VALUE 0xffff     /* Initial CRC16 checksum value */
#define CRC32_INIT_VALUE 0xffffffff /* Initial CRC32 checksum value */

#ifdef NO_CRC_FUNCTIONS
#define WICED_CRC_FUNCTION_NAME(function) wiced_##function
#else
#define WICED_CRC_FUNCTION_NAME(function) function
#endif

extern uint8_t WICED_CRC_FUNCTION_NAME( crc8 )
    (
        uint8_t* pdata,       /* pointer to array of data to process */
        unsigned int nbytes,  /* number of input data bytes to process */
        uint8_t crc           /* either CRC8_INIT_VALUE or previous return value */
    );

extern uint16_t WICED_CRC_FUNCTION_NAME( crc16 )
    (
        uint8_t* pdata,       /* pointer to array of data to process */
        unsigned int nbytes,  /* number of input data bytes to process */
        uint16_t crc          /* either CRC16_INIT_VALUE or previous return value */
    );

extern uint32_t WICED_CRC_FUNCTION_NAME( crc32 )
    (
        uint8_t* pdata,       /* pointer to array of data to process */
        unsigned int nbytes,  /* number of input data bytes to process */
        uint32_t crc          /* either CRC32_INIT_VALUE or previous return value */
    );

#ifdef NO_CRC_FUNCTIONS
static inline ALWAYS_INLINE uint8_t crc8( uint8_t* pdata, unsigned int nbytes, uint8_t crc )
{
    return wiced_crc8( pdata, nbytes, crc );
}

static inline ALWAYS_INLINE uint16_t crc16( uint8_t* pdata, unsigned int nbytes, uint16_t crc )
{
    return wiced_crc16( pdata, nbytes, crc );
}

static inline ALWAYS_INLINE uint32_t crc32( uint8_t* pdata, unsigned int nbytes, uint32_t crc )
{
    return wiced_crc32( pdata, nbytes, crc );
}
#endif

#ifdef __cplusplus
} /*extern "C" */
#endif

#endif /* ifndef _INCLUDED_CRC_H_ */
