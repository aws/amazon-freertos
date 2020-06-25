/*
 * $ Copyright Cypress Semiconductor Apache2 $
 */

#ifndef UNTAR_H__
#define UNTAR_H__   1


#include <stdint.h>
#include "cy_result.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @ Unix Standard Tar header format
 *
 * The Open Group Base Specifications Issue 7, 2018 edition
 * IEEE Std 1003.1-2017 (Revision of IEEE Std 1003.1-2008)
 * Copyright © 2001-2018 IEEE and The Open Group
 *
 * ustar Interchange Format
 * A ustar archive tape or file shall contain a series of logical records.
 * Each logical record shall be a fixed-size logical record of 512 octets (see below).
 * Although this format may be thought of as being stored on 9-track industry-standard
 * 12.7 mm (0.5 in) magnetic tape, other types of transportable media are not excluded.
 * Each file archived shall be represented by a header logical record that describes
 * the file, followed by zero or more logical records that give the contents of the file.
 * At the end of the archive file there shall be two 512-octet logical records filled with
 * binary zeros, interpreted as an end-of-archive indicator.
 * The logical records may be grouped for physical I/O operations, as described under the
 * -b blocksize and -x ustar options. Each group of logical records may be written with a
 * single operation equivalent to the write() function. On magnetic tape, the result of
 * this write shall be a single tape physical block. The last physical block shall always
 * be the full size, so logical records after the two zero logical records may contain undefined data.
 *
 */

#define TAR_BLOCK_SIZE  512

/* General definitions */
#define TNAMELEN    100     /* name field length */
#define TMAGIC      "ustar " /* ustar plus null byte. */
#define TMAGLEN     6       /* Length of the above. */
#define TVERSION    "00"    /* 00 without a null byte. */
#define TVERSLEN    2       /* Length of the above. */

/* Typeflag field definitions */
#define REGTYPE     '0'     /* Regular file. */
#define AREGTYPE    '\0'    /* Regular file. */
#define LNKTYPE     '1'     /* Link. */
#define SYMTYPE     '2'     /* Symbolic link. */
#define CHRTYPE     '3'     /* Character special. */
#define BLKTYPE     '4'     /* Block special. */
#define DIRTYPE     '5'     /* Directory. */
#define FIFOTYPE    '6'     /* FIFO special. */
#define CONTTYPE    '7'     /* Reserved. */

/* Mode field bit definitions (octal) */
#define TSUID       04000   /* Set UID on execution. */
#define TSGID       02000   /* Set GID on execution. */
#define TSVTX       01000   /* On directories, restricted deletion flag. */
#define TUREAD      00400   /* Read by owner. */
#define TUWRITE     00200   /* Write by owner. */
#define TUEXEC      00100   /* Execute/search by owner. */
#define TGREAD      00040   /* Read by group. */
#define TGWRITE     00020   /* Write by group. */
#define TGEXEC      00010   /* Execute/search by group. */
#define TOREAD      00004   /* Read by other. */
#define TOWRITE     00002   /* Write by other. */
#define TOEXEC      00001   /* Execute/search by other. */

/**
 * Cypress definitions
 */

#define CY_UNTAR_CONTEXT_MAGIC      (0x981345A0)
#define CY_MAX_TAR_FILES            (8)
#define CY_TAR_COALESCE_BUFFER_SIZE (TAR_BLOCK_SIZE * 2)
#define CY_FILE_TYPE_LEN            (16)
#define CY_VERSION_STRING_MAX       (16)

typedef enum {
    CY_UNTAR_SUCCESS          = 0,
    CY_UNTAR_ERROR,
    CY_UNTAR_INVALID,
    CY_UNTAR_NOT_ENOUGH_DATA,
    CY_UNTAR_COMPONENTS_JSON_PARSE_FAIL
} cy_untar_result_t;

typedef enum {
    CY_TAR_PARSE_UNINITIALIZED = 0,
    CY_TAR_PARSE_FIND_HEADER,       /* tar file header */
    CY_TAR_PARSE_DATA,
} cy_tar_parse_state_t;


typedef struct ustar_header_s {
                                    /* offset       size                                */
    uint8_t name[TNAMELEN];         /*    0  0x00   100   file name (NUL terminated)    */
    uint8_t mode[8];                /*  100  0x64     8   See mode bits above           */
    uint8_t uid[8];                 /*  108  0x6c     8   User ID                       */
    uint8_t gid[8];                 /*  116  0x74     8   Group ID                      */
    uint8_t size[12];               /*  124  0x7c    12   file size                     */
                                                     /* 12 digit octal in ASCII         */
                                                     /* The number of logical records   */
                                                     /* written following the header    */
                                                     /* shall be (size+511)/512,        */
                                                     /* ignoring any fraction in the    */
                                                     /* result of the division.         */
    uint8_t mtime[12];              /*  136  0x88    12   modified time ?               */
    uint8_t chksum[8];              /*  148  0x94     8   checksum                      */
    uint8_t typeflag;               /*  156  0x9c     1   type (see above)              */
    uint8_t linkname[TNAMELEN];     /*  157  0x9d   100   linked file name              */
    uint8_t magic[TMAGLEN];         /*  257 0x101     6   TMAGIC (NUL termiinated)      */
    uint8_t version[TVERSLEN];      /*  263 0x107     2   TVERSION                      */
    uint8_t uname[32];              /*  265 0x109    32   user name                     */
    uint8_t gname[32];              /*  297 0x129    32   group name                    */
    uint8_t devmajor[8];            /*  329 0x149     8   major                         */
    uint8_t devminor[8];            /*  337 0x151     8   minor                         */
    uint8_t prefix[155];            /*  345 0x159   155   prefix                        */
                                    /*  500 0x1f4    12   padding implied to end of 512 byte block   */
} ustar_header_t;


typedef struct cy_ota_file_info_s {
    char                name[TNAMELEN];     /* copied from the components.json file         */
    char                type[CY_FILE_TYPE_LEN]; /* from components.json                         */
    int                 found_in_tar;       /* encountered the header in the tar file       */
    uint32_t            header_offset;      /* offset of header in tar file                 */
    uint32_t            size;               /* from components.json, verified from header   */
    uint32_t            processed;          /* bytes processed from tar file                */
} cy_ota_file_info_t;

/**
 * @brief callback to handle tar data
 *
 * @param ctxt          untar context
 * @param file_index    index into ctxt->files for the data
 * @param buffer        data to use
 * @param file_offset   offset into the file to store data
 * @param chunk_size    amount of data in buffer to use
 * @param cb_arg        argument passed into initialization
 *
 * return   CY_UNTAR_SUCCESS
 *          CY_UNTAR_ERROR
 */
typedef struct cy_untar_context_s * cy_untar_context_ptr;
typedef cy_untar_result_t (*untar_write_callback_t)(cy_untar_context_ptr ctxt, uint16_t file_index, uint8_t *buffer, uint32_t file_offset, uint32_t chunk_size, void *cb_arg);

typedef struct cy_untar_context_s {
    uint32_t                magic;                          /* CY_UNTAR_CONTEXT_MAGIC               */
    cy_tar_parse_state_t    state;                          /* curent parsing state                 */
    untar_write_callback_t  cb_func;                        /* callback function to deal with data  */
    void                    *cb_arg;                        /* opaque arg passed to callback        */

    int                     already_parsed_components_json; /* True if we've parsed components.json */
    uint32_t                bytes_processed;                /* bytes processed from the archive     */

    /* for JSON parsing */
    char                    version[CY_VERSION_STRING_MAX]; /* string of major.minor.build          */
    uint16_t                num_files_in_json;              /* number of files in components.json   */
    uint16_t                curr_file_in_json;              /* parsing file info in components.json */

    uint16_t                current_file;                   /* currently processing this file from the tar archive  */
    uint16_t                num_files;                      /* number of files encountered                          */
    cy_ota_file_info_t      files[CY_MAX_TAR_FILES];        /* file info                                            */

    uint32_t                coalesce_stream_offset;         /* offset into stream where coalesce buffer starts  */
    uint32_t                coalesce_bytes;                 /* number of bytes in coalesce buffer               */
    uint32_t                coalesce_needs;                 /* number of bytes needed in coalesce buffer        */
    uint8_t                 coalesce_buffer[CY_TAR_COALESCE_BUFFER_SIZE];
} cy_untar_context_t;

/**
 * @brief Determine if this is a tar header
 *
 * @param buffer[in]        ptr to data buffer
 * @param size[in]          size of buffer
 *
 * @return  CY_UNTAR_NOT_VALID
 *          CY_UNTAR_VALID
 *          CY_UNTAR_NOT_ENOUGH_DATA
 */
cy_untar_result_t cy_is_tar_header( uint8_t *buffer, uint32_t size );

/**
 * @brief Initialize our tar context
 *
 * @param ctxt[in,out]      ptr to context structure
 * @param cb_func[in]       callback function
 *
 * @return  CY_UNTAR_SUCCESS
 *          CY_UNTAR_ERROR
 */
cy_untar_result_t cy_untar_init( cy_untar_context_t *ctxt, untar_write_callback_t cb_func, void *cb_arg );

/**
 * @brief De-Initialize our tar context
 *
 * @param ctxt[in,out]      ptr to context structure
 *
 * @return  CY_UNTAR_SUCCESS
 *          CY_UNTAR_ERROR
 */
cy_untar_result_t cy_untar_deinit( cy_untar_context_t *ctxt );

/**
 * @brief Parse the tar data
 *
 * NOTE: This is meant to be called for each chunk of data received
 *       callback will be invoked when there is data to write
 *
 * @param ctxt[in,out]      ptr to context structure, gets updated
 * @param stream_offset     byte offset in current stream of start of tar_buffer
 * @param tar_buffer[in]    ptr to the next buffer of input
 * @param size[in]          bytes in tar_buffer
 * @param consumed[out]     bytes used in callbacks (or skipped at end of file)
 *
 * @return  CY_UNTAR_SUCCESS
 *          CY_UNTAR_ERROR
 */
cy_untar_result_t cy_untar_parse( cy_untar_context_t *ctxt, uint32_t stream_offset, uint8_t *tar_buffer, uint32_t size, uint32_t *consumed);


#ifdef __cplusplus
} /*extern "C" */
#endif


#endif  /* UNTAR_H__ */
