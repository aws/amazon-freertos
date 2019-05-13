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
 *  Private structures and definitions for the WicedFS Read-Only file system.
 */

#ifndef INCLUDED_WICEDFS_INTERNAL_H_
#define INCLUDED_WICEDFS_INTERNAL_H_

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef INSIDE_WICEDFS_C_
#error wicedfs_internal.h should not be used externally
#endif /* ifndef INSIDE_WICEDFS_C_ */

/******************************************************
 *                     Macros
 ******************************************************/
/* Suppress unused parameter warning */
#ifndef UNUSED_PARAMETER
#define UNUSED_PARAMETER(x) /*@-noeffect@*/ ( (void)(x) ) /*@+noeffect@*/
#endif

/******************************************************
 *                    Constants
 ******************************************************/
/* File table Entry Type definitions
 * To be used for the type_flags_permissions field
 */

#define WICEDFS_TYPE_DIR       (0)
#define WICEDFS_TYPE_FILE      (1)
#define WICEDFS_TYPE_LINK      (2)
#define WICEDFS_TYPE_MASK      (0xf)

/* Magic Number for the filesystem header (wicedfs_filesystem_header_t) */
#define WICEDFS_MAGIC     ((uint32_t)(((uint8_t)'B'<<0)|((uint8_t)'R'<<8)|((uint8_t)'C'<<16)|((uint8_t)'M'<<24)))

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
/*****************************************************************
 * The following structures define the filesystem data structures
 * on the hardware device.
 *
 * They are consequently byte packed.
 */

#pragma pack(1)

/* Overall File-system Header structure */
typedef struct
{
    uint32_t        magic_number;        /* Should be  'B', 'R', 'C', 'M' for valid filesystem - see WICEDFS_MAGIC below */
    uint32_t        version;             /* Version 1 only so far */
    wicedfs_usize_t root_dir_offset;     /* Offset to wicedfs_dir_header_t for root directory from start of this  wicedfs_filesystem_header_t */
} wicedfs_filesystem_header_t;


/* Directory Header structure*/
typedef struct
{
    wicedfs_ssize_t file_table_offset;   /* Offset from the start of this directory header to the File table */
    wicedfs_usize_t num_files;           /* Number of entries in the file table */
    uint32_t        file_header_size;    /* The size in bytes of each entry in the file table */
    uint32_t        filename_size;       /* The size in bytes of the filename part of each file table entry */
} wicedfs_dir_header_t;

/* File Table entry structure */
typedef struct
{
    uint32_t        type_flags_permissions;  /* A bit field containing the Type, Flags and Permissions for the entry - See definitions below */
    wicedfs_ssize_t offset;                  /* Offset from the start of this file table entry structure to the content data for this entry */
    wicedfs_usize_t size;                    /* Size in bytes of the content data for this entry */
    /* filename follows this - variable size as per wicedfs_dir_header_t.filename_size */
} wicedfs_file_header_t;

#pragma pack()

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

#ifdef __cplusplus
} /*extern "C" */
#endif

#endif /* ifndef INCLUDED_`_INTERNAL_H_ */
