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
 *  WICED Resource API's
 *  The Resource Management functions reads resource from a resource location
 *  and returns the number of bytes from an offset in an caller filled buffer.
 *
 *  Functions to get the resource size and resource data
 *
 *  The Resource could be one of the three locations
 *
 *  - Wiced Filesystem (File System)
 *  - Internal Memory  (Embedded Flash memory)
 *  - External Storage ( External Flash connected via SPI interface)
 *
 */

#ifndef INCLUDED_RESOURCE_H_
#define INCLUDED_RESOURCE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************
 *                     Macros
 ******************************************************/
#ifndef MIN
#define MIN( x, y ) ((x) < (y) ? (x) : (y))
#endif /* ifndef MIN */

/* Suppress unused parameter warning */
#ifndef UNUSED_PARAMETER
#define UNUSED_PARAMETER( x ) ( (void)(x) )
#endif

#ifndef RESULT_ENUM
#define RESULT_ENUM( prefix, name, value )  prefix ## name = (value)
#endif /* ifndef RESULT_ENUM */


/* These Enum result values are for Resource errors
 * Values: 4000 - 4999
 */
#define RESOURCE_RESULT_LIST( prefix )  \
    RESULT_ENUM(  prefix, SUCCESS,                         0 ),   /**< Success */                           \
    RESULT_ENUM(  prefix, UNSUPPORTED,                     7 ),   /**< Unsupported function */              \
    RESULT_ENUM(  prefix, OFFSET_TOO_BIG,               4001 ),   /**< Offset past end of resource */       \
    RESULT_ENUM(  prefix, FILE_OPEN_FAIL,               4002 ),   /**< Failed to open resource file */      \
    RESULT_ENUM(  prefix, FILE_SEEK_FAIL,               4003 ),   /**< Failed to seek to requested offset in resource file */ \
    RESULT_ENUM(  prefix, FILE_READ_FAIL,               4004 ),   /**< Failed to read resource file */

#define resource_get_size( resource ) ((resource)->size)

/******************************************************
 *                    Constants
 ******************************************************/

#define RESOURCE_ENUM_OFFSET  ( 1300 )

/******************************************************
 *                Enumerations
 ******************************************************/

/**
 * Result type for WICED Resource function
 */
typedef enum
{
    RESOURCE_RESULT_LIST( RESOURCE_ )
} resource_result_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef const void*   resource_data_t;
typedef unsigned long resource_size_t;

/******************************************************
 *                   Structures
 ******************************************************/

/**
 * Memory handle
 */
typedef struct
{
   const char* data;           /**< resource data */
} memory_resource_handle_t;

/**
 * Filesystem handle
 */
typedef struct
{
    unsigned long offset;      /**< Offset to the start of the resource */
    const char* filename;      /**< name of the resource                */
} filesystem_resource_handle_t;


typedef enum
{
    RESOURCE_IN_MEMORY,           /**< resource location in memory           */
    RESOURCE_IN_FILESYSTEM,       /**< resource location in filesystem       */
    RESOURCE_IN_EXTERNAL_STORAGE  /**< resource location in external storage */
} resource_location_t;

/**
 * Resource handle structure
 */
typedef struct
{
   resource_location_t location;        /**< resource location */
   unsigned long size;                  /**< resource size     */
    union
    {
       filesystem_resource_handle_t fs;                       /** < filesystem resource handle */
       memory_resource_handle_t     mem;                      /** < memory resource handle     */
       void*                        external_storage_context; /** < external storage context   */
    } val;
} resource_hnd_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/*****************************************************************************/
/** @addtogroup  resourceapi  Wiced Resource Management API's
 *  @ingroup framework
 *
 *  WCIED Resource Management API's has functions to get the
 *  resource size and reads resource data from a resource
 *  location and returns the number of bytes in an caller
 *  filled buffer
 *
 *  The Resource could be one of the three locations
 *
 *  - Wiced Filesystem ( File System)
 *  - Internal Memory   (Embedded Flash memory)
 *  - External Storage  ( External Flash connected via SPI interface )
 *
 *  @{
 */
/*****************************************************************************/

/** Read resource using the handle specified
 *
 * @param[in]  resource : handle of the resource to read
 * @param[in]  offset   : offset from the beginning of the resource block
 * @param[in]  maxsize  : size of the buffer
 * @param[out] size     : size of the data successfully read
 * @param[in]  buffer   : pointer to a buffer to contain the read data
 *
 * @return @ref resource_result_t
 */
extern resource_result_t resource_read( const resource_hnd_t* resource, uint32_t offset, uint32_t maxsize, uint32_t* size, void* buffer );

/** Retrieve a read only resource buffer using the handle specified
 *
 * @param[in]  resource : handle of the resource to read
 * @param[in]  offset   : offset from the beginning of the resource block
 * @param[in]  maxsize  : size of the buffer
 * @param[out] size     : size of the data successfully read
 * @param[out] buffer   : pointer to a buffer pointer to point to the resource data
 *
 * @return @ref resource_result_t
 */
extern resource_result_t resource_get_readonly_buffer ( const resource_hnd_t* resource, uint32_t offset, uint32_t maxsize, uint32_t* size_out, const void** buffer );

/** Free a read only resource buffer using the handle specified
 *
 * @param[in]  resource : handle of the resource to read
 * @param[in]  buffer   : pointer to a buffer set using resource_get_readonly_buffer
 *
 * @return @ref resource_result_t
 */
extern resource_result_t resource_free_readonly_buffer( const resource_hnd_t* handle, const void* buffer );
/* @} */
#ifdef __cplusplus
} /*extern "C" */
#endif

#endif /* ifndef INCLUDED_RESOURCE_H_ */
