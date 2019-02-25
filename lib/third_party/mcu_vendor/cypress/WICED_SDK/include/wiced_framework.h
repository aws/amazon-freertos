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
 *  Defines functions that allow access to the Device Configuration Table (DCT)
 *
 */

#pragma once

#include <stdint.h>
#include "platform_dct.h"
#include "wiced_result.h"
#include "wiced_utilities.h"
#include "wiced_dct_common.h"
#include "wiced_waf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 * @cond                Macros
 ******************************************************/

#define DCT_OFFSET( type, member )  ( (uint32_t)&((type *)0)->member )

#if defined(__IAR_SYSTEMS_ICC__)
#pragma section="initial_dct_section"
#define DEFINE_APP_DCT( type ) \
    const type _app_dct @ "initial_dct_section"; \
    const type _app_dct =
#else  /* #if defined(__IAR_SYSTEMS_ICC__) */
#define DEFINE_APP_DCT( type ) const type _app_dct =
#endif /* #if defined(__IAR_SYSTEMS_ICC__) */

/******************************************************
 *                    Constants
 ******************************************************/

#define WICED_FACTORY_RESET_MAGIC_VALUE  ( 0xA6C5A54E )
#define WICED_FRAMEWORK_LOAD_ALWAYS      ( 0x0 )
#define WICED_FRAMEWORK_LOAD_ONCE        ( 0x1 )


/******************************************************
 *                   Enumerations
 ******************************************************/

/**
 * Application validity
 */
typedef enum
{
    APP_INVALID = 0,
    APP_VALID   = 1
} app_valid_t;

/**
 * Bootloader status
 */
typedef enum boot_status_enum
{
    BOOTLOADER_BOOT_STATUS_START                   = 0x00,
    BOOTLOADER_BOOT_STATUS_APP_OK                  = 0x01,
    BOOTLOADER_BOOT_STATUS_WLAN_BOOTED_OK          = 0x02,
    BOOTLOADER_BOOT_STATUS_WICED_STARTED_OK        = 0x03,
    BOOTLOADER_BOOT_STATUS_DATA_CONNECTIVITY_OK    = 0x04
} boot_status_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/**
 * Copy Function Source Structure
 */
typedef struct
{
    union
    {
        void*    file_handle;
        uint32_t fixed_address;
    } val;
    enum
    {
        COPY_SRC_FILE,
        COPY_SRC_FIXED,
    } type;
} platform_copy_src_t;


/** Structure to hold information about a system monitor item */
typedef struct
{
    uint32_t last_update;              /**< Time of the last system monitor update */
    uint32_t longest_permitted_delay;  /**< Longest permitted delay between checkins with the system monitor */
} wiced_system_monitor_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 * @endcond
 ******************************************************/

/*****************************************************************************/
/** @defgroup framework       WICED Application Framework
 *
 * WICED functions for managing applications and non-volatile data.
 * The WICED Application framework provides an interface for
 * application configuration data, settings and operational modes.
 *
 * Concept: WICED supports a fixed number of applications to be written on it.
 * The word applications may be a bit confusing, it is binary files. Some of
 * These files are applications that can be loaded, while other are files used
 * for other purposes, these include file system image, wifi firmware, BT firm
 * ware, etc. Following is a description of the very tiny file system that can
 * handle these binaries and yet allow them to be updated.
 *
 * Operations supported:
 * Erase App: An app (or binary file in general) needs to be erased before it can
 * be rewritten.
 *
 * Write File: Once an app is erased it can be written, a new file can be uploaded
 * to the location of the file. The new file doesn't need to be the same size, it
 * can be bigger size (details on how to handle this will come later)
 *
 * Close File: Once a file is written, it must be closed. This will update the
 * entries in the app file system to use the new file.
 *
 * Read File: An app can always be read.
 *
 * Design:
 * App header locations: The DCT contains a list of the application locations that
 * WICED will support. At this point, it includes 8 blobs (Factory reset App, OTA
 * upgrade app, APP0, resources file system, Wifi firmware, APP1, APP2, APP3).
 * Each entry in the DCT points not the app location, but to the location of its
 * description within a log block (this is 1 block assigned for logging the apps).
 *
 * Log Block: The log block stores the apps locations. Apps are located with a
 * granulaity of a block. each app location has the following data:
 * - Number of entry for the app
 * - Entries : Each entry is a start block and count, indicated a consequetive list
 *             of blocks with for the app.
 * As long as the new file is covered by these entries, no update is needed for the
 * app header. Once a new block is needed to be written A new header will be added
 * at the ended of the log block and the DCT location for the app header will be
 * updated. This is done in the close operation.
 *
 * Erasing a file: Once an erase command is issued, the header address is obtained
 * from the DCT, the header is read and all entries are erase.
 *
 * Writing to a file: Before a file is written it has to be erased. As long as the
 * writing offset and size is within the entries of the app, the writing is done
 * with no change. Once the writing exceeds the already assigned sizes, the next
 * empty block is going to be written wiht the new data. Writing will continue to
 * the next blocks as required (the number of the first block and the count of blocks
 * will be stored in ram for update)
 *
 * Closing a file: When closing a file, if no new entries is added, then nothing to be
 * done. If however a new entry was needed, then the program header is read and updated
 * with a new entry and written as the end of the log block, DCT app locations is updated
 * to point to the new app header location.
 *
 * Reading of a file: When a file is read, the offset is mapped to the actual address in the
 * flash:
 *      Physical address = entry physical adddress + (offset - entry file offset)
 *      Entry file offset = Count of previous blocks * size of block
 *      Entry physical address = Entry * size of block
 */
/*****************************************************************************/

/*****************************************************************************/
/** @addtogroup dct       DCT
 *  @ingroup framework
 *
 * Device Configuration Table (Non-volatile flash storage space). The DCT is a structure
 * which stores persistent application and system settings and allows for management of
 * application settings.
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Reads the DCT and returns a pointer to the DCT data
 *
 * The operation of this function depends on whether the DCT is located in
 * external or internal flash memory.
 * If ptr_is_writable is set to false and the DCT is located in internal flash,
 * then a direct pointer to the flash memory will be returned.
 * Otherwise memory will be allocated and the DCT data will be copied into it.
 *
 * @note : this function MUST be used in pairs with wiced_dct_read_unlock to
 *         ensure that any allocated memory is freed.
 *
 * @note: See DCT definitions in WICED/platform/include/platform_dct.h
 *
 * @note: This function locks the DCT during the read. The DCT is not left in a locked state.
 *
 * @param info_ptr [out]       : A pointer to the pointer that will be filled on return
 * @param ptr_is_writable [in] : If true then then the returned pointer will be in RAM
 *                               allowing it to be modified. e.g. before being written
 * @param section [in]         : The section of the DCT which should be read
 * @param offset [in]          : The offset in bytes within the section
 * @param size [in]            : The length of data that should be read
 *
 * @return    Wiced Result
 */
extern wiced_result_t wiced_dct_read_lock( void** info_ptr, wiced_bool_t ptr_is_writable, dct_section_t section, uint32_t offset, uint32_t size );


/** Frees any space allocated in wiced_dct_read_lock()
 *
 * @note : this function must be used in pairs with wiced_dct_read_lock
 *
 * @note: See DCT definitions in WICED/platform/include/platform_dct.h
 *
 * @param info_ptr [in]       : A pointer that was created with wiced_dct_read_lock()
 * @param ptr_is_writable[in] : Indicates whether the pointer was retrevied as a writable pointer
 *
 * @return    Wiced Result
 */
extern wiced_result_t wiced_dct_read_unlock( void* info_ptr, wiced_bool_t ptr_is_writable );


/** Writes data to the DCT
 *
 * Writes a chunk of data to the DCT.
 *
 * @note : Ensure that this function is only called occasionally, otherwise
 *         the flash memory wear may result.
 *
 * @note: See DCT definitions in WICED/platform/include/platform_dct.h
 *
 * @param info_ptr [in] : A pointer to the pointer that will be filled on return
 * @param section [in]  : The section of the DCT which should be read
 * @param offset [in]   : The offset in bytes within the section
 * @param size [in]     : The length of data that should be written
 *
 * @return    Wiced Result
 */
wiced_result_t wiced_dct_write( const void* info_ptr, dct_section_t section, uint32_t offset, uint32_t size );

/** Write the boot_details structure to the DCT.
 *
 * @param new_boot_details [in] : A pointer to the new boot_detail structure to be written.
 *
 * @return    Wiced Result
 */
wiced_result_t wiced_dct_write_boot_details( const boot_detail_t* new_boot_details );

/** Write the app location to the DCT.
 *
 * @param new_app_location_info [in] : A pointer to the new image location structure to be written.
 * @param dct_app_index [in]         : Application index to write.
 *
 * @return    Wiced Result
 */
wiced_result_t wiced_dct_write_app_location( image_location_t* new_app_location_info, uint32_t dct_app_index );

/** @} */

/*****************************************************************************/
/** @addtogroup app       App management
 *  @ingroup framework
 *
 * Application management functions to set the boot target to either RAM or FLASH, functions to
 * reboot. The application management has functions to open the file-system via wiced_framework_app_open
 * and close via wiced_framework_app_close. The Application management provides utility functions
 * to erase, write application and reads the size of Application from SFLASH
 * and API's to get/set size.
 *
 * @note: these functions are implemented as function pointers to allow
 *        them to be shared between a boot-loader and application
 *
 * @note: For Definition of the Structure wiced_app_t please refer /WICED/Platform/MCU/wiced_waf_common.h
 *
 *  @{
 */
/*****************************************************************************/

/** Sets the next booting application after reset
 *  updates the boot details to point to the specified application ID. \p
 *
 * A valid application must exist in the specified application. If the applications
 * is not valid, the behavior is undetermined.
 *
 * @warning To run the new a reboot is required.
 *
 * @param[in] app_id   : Application ID.
 * @param[in] load_once: A flag to indicate to reload the application every time or just
 *                       once. A RAM application will need to be reloaded every time while
 *                       applications running from flash can be loaded just once.
 *
 * @return @ref wiced_result_t
 *
 */
static inline wiced_result_t wiced_framework_set_boot( uint8_t app_id, char load_once );

/** Reboots the system
 *
 * Causes a soft-reset of the processor, which will restart the program
 * from the boot vector. The function does not return.
 *
 *  @return Does not return!
 */
static inline void wiced_framework_reboot( void );

/** Initialize the application for modification to
 *  unlock the application for later modification.
 *
 * @warning Applications must be opened before any write or read operations.
 *
 * @param[in] app_id   : Application ID.
 * @param[inout] app   : Application handler.
 *
 *
 * @return @ref wiced_result_t
 */
static inline wiced_result_t wiced_framework_app_open( uint8_t app_id, wiced_app_t* app );

/** Finalize application modification.
 *
 * Lock the application (flush any cached operations).
 *
 * @warning Applications must be closed after all write and read operations.
 *
 * @param[in] app   : Application handler.
 *
 * @return @ref wiced_result_t
 */
static inline wiced_result_t wiced_framework_app_close( wiced_app_t* app );

/** Erase the full application content from external flash.
 *
 * @warning Applications must be erased before being rewritten.
 *
 * @param[inout] app   : Application handler.
 *
 * @return @ref wiced_result_t
 */
static inline wiced_result_t wiced_framework_app_erase( wiced_app_t* app );

/** Writes a chunk of the application to external flash with a given size
 *  into external flash.
 *
 * @warning Applications must be erased before being rewritten. To fully erase an
 *          application call wiced_framework_erase_app. Applications can also be
 *          erased on go by passing last_erased_sector pointer. However when erasing
 *          on the go, writing to the file must be sequential with no gaps.
 *
 * @param[inout] app : Application handler.
 * @param[in] data   : The data to be written
 * @param[in] size   : The number of bytes to be written
 *
 *  @return @ref wiced_result_t
 */
static inline wiced_result_t wiced_framework_app_write_chunk( wiced_app_t* app, const uint8_t* data, uint32_t size );

/** Reads a chunk of the application with a given offset and size
 *  from external flash.
 *
 * @param[inout] app : Application handler.
 * @param[in] offset : The offset from the start of the application in bytes
 * @param[in] data   : The buffer for the data to be read
 * @param[in] size   : The number of bytes to be read
 *
 *  @return @ref wiced_result_t
 */
static inline wiced_result_t wiced_framework_app_read_chunk( wiced_app_t* app, uint32_t offset, uint8_t* data, uint32_t size );

/** Returns the current size of the application.
 *
 * @warning The size of the application is always aligned to sector boundaries. Application
 *          size may be different from actual size on a PC.
 *
 * @param[inout] app : Application handler.
 * @param[out] size  : The size allocated to the application in bytes.
 *
 *  @return @ref wiced_result_t
 */
static inline wiced_result_t wiced_framework_app_get_size( wiced_app_t* app, uint32_t* size );

/** Sets the current size of the application.
 *
 * @warning Before updating the application, the size must be set to match the new size of
 *          the application. The size of the application is always aligned to sector
 *          boundaries. Application size may be different from actual size on a PC. If the
 *          provided size is smaller than the current size, the current size is maintained.
 *
 * @param[inout] app : Application handler.
 * @param[in] size   : The new size allocated to the application in bytes.
 *
 *  @return @ref wiced_result_t
 */
static inline wiced_result_t wiced_framework_app_set_size( wiced_app_t* app, uint32_t size );

/** @} */


/*****************************************************************************/
/** @addtogroup sysmon       System Monitor
 *  @ingroup mgmt
 *
 * Functions to communicate with the system monitor
 *
 *  @{
 */
/*****************************************************************************/

/** Registers a system monitor with the system monitor thread
 *
 * @param[out] system_monitor          : A pointer to a system monitor object that will be watched
 * @param[in]  initial_permitted_delay : The maximum time in milliseconds allowed between monitor updates
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_register_system_monitor(wiced_system_monitor_t* system_monitor, uint32_t initial_permitted_delay);

/** Updates a system monitor and resets the last update time
 *
 * @param[out] system_monitor  : A pointer to a system monitor object to be updated
 * @param[in]  permitted_delay : The maximum time in milliseconds allowed between monitor updates
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_update_system_monitor(wiced_system_monitor_t* system_monitor, uint32_t permitted_delay);

/** Wakeup system monitor thread
 *
 * @return @ref wiced_result_t
 */
extern wiced_result_t wiced_wakeup_system_monitor_thread(void);

/** @} */

/*****************************************************************************/
/**  Implementations of inline functions                                    **/
/*****************************************************************************/
//
//static inline ALWAYS_INLINE wiced_result_t wiced_dct_write( const void* info_ptr, dct_section_t section, uint32_t offset, uint32_t size )
//{
//    return wiced_dct_update( info_ptr, section, offset, size );
//}

static inline ALWAYS_INLINE wiced_result_t wiced_framework_set_boot( uint8_t app_id, char load_once)
{
    return wiced_waf_app_set_boot( app_id, load_once );
}

static inline ALWAYS_INLINE void wiced_framework_reboot( void )
{
    wiced_waf_reboot( );
}

static inline ALWAYS_INLINE wiced_result_t wiced_framework_app_open( uint8_t app_id, wiced_app_t* app )
{
    return wiced_waf_app_open( app_id, app );
}

static inline ALWAYS_INLINE wiced_result_t wiced_framework_app_erase( wiced_app_t* app )
{
    return wiced_waf_app_erase( app );
}

static inline ALWAYS_INLINE wiced_result_t  wiced_framework_app_write_chunk( wiced_app_t* app, const uint8_t* data, uint32_t size )
{
    return wiced_waf_app_write_chunk( app, data, size);
}

static inline ALWAYS_INLINE wiced_result_t  wiced_framework_app_read_chunk( wiced_app_t* app, uint32_t offset, uint8_t* data, uint32_t size )
{
    return wiced_waf_app_read_chunk( app, offset, data, size);
}

static inline ALWAYS_INLINE wiced_result_t  wiced_framework_app_get_size( wiced_app_t* app, uint32_t* size )
{
    return wiced_waf_app_get_size( app, size);
}

static inline ALWAYS_INLINE wiced_result_t  wiced_framework_app_set_size( wiced_app_t* app, uint32_t size )
{
    return wiced_waf_app_set_size( app, size);
}

static inline ALWAYS_INLINE wiced_result_t wiced_framework_app_close( wiced_app_t* app )
{
    return wiced_waf_app_close( app );
}

/*****************************************************************************/
/**  Deprecated functions                                                   **/
/*****************************************************************************/
#ifdef __cplusplus
} /*extern "C" */
#endif
