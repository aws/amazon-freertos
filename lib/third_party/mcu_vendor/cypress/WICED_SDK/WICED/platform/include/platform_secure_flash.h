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

/** @file
 * Defines macros describe cache
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                    Constants
 ******************************************************/

#ifndef PLATFORM_SECURE_SFLASH_SECTOR_SIZE
#define PLATFORM_SECURE_SFLASH_SECTOR_SIZE ( 4096 )
#endif

#define SFLASH_SHA256_HASH_SIZE             ( 32 )
#define SECURE_SECTOR_SIZE                  ( PLATFORM_SECURE_SFLASH_SECTOR_SIZE ) /* SECURE_SECTOR_DATA_SIZE + SECURE_SECTOR_METADATA_SIZE */

/******************************************************
 *                      Macros
 ******************************************************/
/*
 * secure sflash uses SECURE_SECTOR_METADATA_SIZE bytes out of each SFLASH_SECTOR to
 * store metadata (for example the SHA265 Hash of the sector data), so effectively
 * each sector can store only  ( SECTOR_SIZE - SECURE_SECTOR_METADATA_SIZE )
 * bytes of data.
 *
 */

#define SECURE_SECTOR_METADATA_SIZE         ( SFLASH_SHA256_HASH_SIZE )
#define SECURE_SECTOR_DATA_SIZE             ( SECURE_SECTOR_SIZE - SECURE_SECTOR_METADATA_SIZE )

 /* input : sflash physical address
  * Convert input physical address to nearest smaller sector aligned address
  */
#define ALIGN_TO_SECTOR_ADDRESS( x )        ( ( ( x ) / SECURE_SECTOR_SIZE ) * SECURE_SECTOR_SIZE )

/* input : size in bytes
 * For n input bytes, calculate the total secure sflash metadata overhead bytes
 * needed to store n input bytes in secure sflash
 */
#define SECURE_SFLASH_METADATA_SIZE( x )    ( ( ( x ) / SECURE_SECTOR_SIZE ) * SECURE_SECTOR_METADATA_SIZE )

/* input : sflash physical address
 * Convert sflash physical address to secure sflash physical address
 * The mapping is :
 * ( 0 )                           to  ( SECURE_SECTOR_DATA_SIZE - 1 )         ----->  0th Sector
 * ( SECURE_SECTOR_DATA_SIZE )     to  ( ( SECURE_SECTOR_DATA_SIZE * 2 ) -1 )  ----->  1st Sector
 * ( SECURE_SECTOR_DATA_SIZE * 2 ) to  ( ( SECURE_SECTOR_DATA_SIZE * 3 ) -1 )  ----->  2nd Sector
 * ...
 */
#define SECURE_SECTOR_ADDRESS( x )           ( ( ( x ) / SECURE_SECTOR_DATA_SIZE ) * SECURE_SECTOR_SIZE )

/* input : sflash physical address
 * Derive the offset of a given address within a secure sector
 */
#define OFFSET_WITHIN_SECURE_SECTOR( x )    ( ( x ) % SECURE_SECTOR_DATA_SIZE )

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef struct securesflash_handle securesflash_handle_t;

typedef struct sflash_handle sflash_handle_t;

typedef int ( *sflash_write_t ) ( const sflash_handle_t* const handle, unsigned long device_address,
        /*@observer@*/ const void* const data_addr, unsigned int size );
typedef int ( *sflash_read_t ) ( const sflash_handle_t* const handle, unsigned long device_address,
        /*@out@*/ /*@dependent@*/ void* data_addr, unsigned int size );
/******************************************************
 *                    Structures
 ******************************************************/

struct securesflash_handle
{
    /* Buffer to Read from Sflash and encrypt/decrypt data */
    uint8_t         hwcrypto_buffer[ SECURE_SECTOR_SIZE * 2 ];
#ifdef __IAR_SYSTEMS_ICC__
    #pragma pack(push, 32)
    uint8_t         scratch_pad [ 64 ];
    #pragma pack(pop)
#else
    uint8_t         scratch_pad [ 64 ] ALIGNED(32);
#endif
    uint8_t         hmac_key[ 32 ]; /* HMAC Key size */
    uint8_t         aes128_key[ 16 ]; /* AES CBC 128 Key size */
    sflash_read_t   sflash_secure_read_function;
    sflash_write_t  sflash_secure_write_function;
};

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

#ifdef __cplusplus
} /*extern "C" */
#endif
