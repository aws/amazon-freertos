/*
 * $ Copyright Cypress Semiconductor Apache2 $
 */

/*
 * Parse incoming TAR archive
 *
 * NOTE: Since the data we expect is coming over WiFi we possiblky
 *         won't be getting anything on a reasonable TAR_BLOCK_SIZE boundary.
 *
 *         We will need to store the last chunk we received that we didn't consume
 *         or have the caller do that for us in order to find the ustar headers
 *         nicely.
 *
 */
#include <ctype.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "untar.h"
#include "cy_result.h"
#include "JSON.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

/*************************************************************
 * Defines and enums
 ************************************************************/
#define CY_FILENAME_COMPONENT_JSON  "components.json"
#define CY_UNTAR_COMPONENTS_JSON_INDEX  (0)

#define CY_ASSERT(a, b)

/* components.json key strings */
#define CY_KEY_NUM_COMPONENTS       "numberOfComponents"
#define CY_KEY_VERSION              "version"
#define CY_KEY_FILES                "files"
#define CY_KEY_FILE_NAME            "fileName"
#define CY_KEY_FILE_TYPE            "fileType"
#define CY_KEY_FILE_SIZE            "fileSize"
//#define CY_KEY_FILE_COMPRESSION     "compression"     # future

/*************************************************************
 * Structures
 ************************************************************/

/*************************************************************
 * Data
 ************************************************************/

/*************************************************************
 * Function declarations
 ************************************************************/

/*************************************************************
 * Static Functions
 ************************************************************/
static int cy_untar_block_of_zeros( uint8_t *buffer, uint32_t size)
{
    while(size > 0)
    {
        if (*buffer != 0x00)
        {
            return 1;
        }
        buffer++;
        size--;
    }

    return 0;
}

static uint32_t cy_octal_string_to_u32(const char *octal_string)
{
    uint32_t    value = 0;

    if (octal_string == NULL)
    {
        return value;
    }

    while( isdigit(*octal_string) )
    {
        value <<= 3;
        value |= (uint32_t)(*octal_string - '0');
        octal_string++;
    }

    return value;
}

static cy_rslt_t ota_untar_json_callback( cy_json_object_t *obj, void*cb_arg )
{
    cy_untar_context_t *ctxt = (cy_untar_context_t *)cb_arg;
    if (ctxt == NULL)
    {
        return CY_RSLT_TYPE_ERROR;
    }

    if (strncmp(CY_KEY_NUM_COMPONENTS, obj->object_string, obj->object_string_length) == 0)
    {
        ctxt->num_files_in_json = (uint16_t)atoi(obj->value);
    }
    if (strncmp(CY_KEY_VERSION, obj->object_string, obj->object_string_length) == 0)
    {
        memcpy(ctxt->version, obj->value, obj->value_length);
    }
    if (strncmp(CY_KEY_FILES, obj->object_string, obj->object_string_length) == 0)
    {
        ctxt->curr_file_in_json = 0;
    }
    else if (strncmp(CY_KEY_FILE_NAME, obj->object_string, obj->object_string_length) == 0)
    {
        /* if we already have an entry for curr file, increment curr file */
        if (strncmp(CY_FILENAME_COMPONENT_JSON, obj->value, obj->value_length) == 0 )
        {
            /* components.json here - don't increment current file */
        }
        else if ( (ctxt->files[ctxt->curr_file_in_json].name[0] != 0) ||
                (ctxt->files[ctxt->curr_file_in_json].size != 0) )
        {
            ctxt->curr_file_in_json++;
        }
        memcpy(ctxt->files[ctxt->curr_file_in_json].name, obj->value, obj->value_length);
    }
    else if (strncmp(CY_KEY_FILE_SIZE, obj->object_string, obj->object_string_length) == 0)
    {
        ctxt->files[ctxt->curr_file_in_json].size = (uint32_t)atoi(obj->value);
    }
    else if (strncmp(CY_KEY_FILE_TYPE, obj->object_string, obj->object_string_length) == 0)
    {
        memcpy(ctxt->files[ctxt->curr_file_in_json].type, obj->value, obj->value_length);
    }

    return CY_RSLT_SUCCESS;
}

static cy_untar_result_t cy_parse_component_json_data(cy_untar_context_t *ctxt, uint8_t *buffer, uint32_t size)
{
    cy_rslt_t       result;
    const char      *file_start;

    if ( (ctxt == NULL) || (buffer == NULL) || (size == 0))
    {
        return CY_UNTAR_ERROR;
    }

    if (size < ctxt->files[0].size )
    {
        configPRINTF( ("%s() Not enough data for components.json parse need: %ld\n", __func__, ctxt->files[0].size) );
        /* still waiting for all of the json, still good */
        return CY_UNTAR_NOT_ENOUGH_DATA;
    }

    /* initialize info for start of parsing components.json */
    file_start = (char *)buffer;
    ctxt->curr_file_in_json = 0;
    ctxt->num_files_in_json = 0;
    cy_JSON_parser_register_callback( ota_untar_json_callback, ctxt );
    result = cy_JSON_parser( file_start, ctxt->files[0].size );
    if (result != CY_RSLT_SUCCESS)
    {
        return CY_UNTAR_ERROR;
    }
    return CY_UNTAR_SUCCESS;
}

/**
 * @brief process a ustar header
 *
 * NOTE: always consumes TAR_BLOCK_SIZE bytes
 *
 * @param ctxt[in,out]      ptr to context structure, gets updated
 * @param stream_offset[in] offset into entire stream
 * @param buffer[in]        ptr to the next buffer of input
 * @param size[in]          bytes in tar_buffer
 *
 * @return  CY_UNTAR_SUCCESS
 *          CY_UNTAR_ERROR
 */
static cy_untar_result_t cy_untar_parse_process_header(cy_untar_context_t *ctxt, uint32_t stream_offset, uint8_t *buffer, uint32_t size)
{
    uint16_t        i;
    uint32_t        component_size;
    ustar_header_t  *hdr = (ustar_header_t *)buffer;

    /* make sure this is a valid header */
    if (cy_is_tar_header( buffer, size ) != CY_UNTAR_SUCCESS)
    {

        if (cy_untar_block_of_zeros(buffer, size) == 0)
        {
            configPRINTF( ("%s() is_tar_header() ZEROs\n" , __func__) );
            /* full TAR_BLOCK_SIZE of 0x00 - ignore */
            return CY_UNTAR_SUCCESS;
        }

        /* expected a header but NOT A HEADER. */
        configPRINTF( ("%s() is_tar_header() failed\n" , __func__) );
        return CY_UNTAR_ERROR;
    }

    /* special case for our components.json file */
    if ( strcmp((char*)hdr->name, CY_FILENAME_COMPONENT_JSON) == 0)
    {
        if (ctxt->num_files > 0 )
        {
            /* error - components.json must be first! */
            configPRINTF( ("%s() missing components.json!\n" , __func__) );
            return CY_UNTAR_ERROR;
        }
        ctxt->current_file = CY_UNTAR_COMPONENTS_JSON_INDEX;
        ctxt->files[ctxt->current_file].size = cy_octal_string_to_u32((char*)hdr->size);
    }
    else
    {
        /* not components.json - find file in the list parsed from components.json */
        for (i = 0; i < ctxt->num_files_in_json ; i++)
        {
            if (strcmp((char*)hdr->name, ctxt->files[i].name) == 0)
            {
                /* is the file size the same as in components.json? */
                component_size = cy_octal_string_to_u32((char *)hdr->size);
                if (component_size != ctxt->files[i].size)
                {
                    /* data in tar and components.json do not match */
                    configPRINTF( ("%s() components.json faulty file size %ld != %ld %s!\n" , __func__, component_size, ctxt->files[i].size) );
                    return CY_UNTAR_ERROR;
                }
                /* we found a file */
                ctxt->current_file = i;
                break;
            }
        }
        if (i >= ctxt->num_files_in_json)
        {
            /* File not found in components.json - data in tar and components.json do not match */
            configPRINTF( ("%s() components.json faulty could not find %s!\n" , __func__, (char*)hdr->name) );
            return CY_UNTAR_ERROR;
        }
    }

    /* we found a valid file header */
    strcpy(ctxt->files[ctxt->current_file].name, (char *)hdr->name);     /* NUL terminated string */
    ctxt->files[ctxt->current_file].header_offset = stream_offset;
    ctxt->files[ctxt->current_file].found_in_tar = 1;
    ctxt->files[ctxt->current_file].processed = 0;

    /* increment # file headers found, change state to DATA */
    ctxt->num_files++;
    ctxt->state = CY_TAR_PARSE_DATA;
    return CY_UNTAR_SUCCESS;
}

/**
 * @brief process tar file data
 *
 * NOTE: always consumes size bytes
 *
 * @param ctxt[in,out]      ptr to context structure, gets updated
* @param stream_offset[in] offset into entire stream
 * @param buffer[in]        ptr to the next buffer of input
 * @param size[in]          bytes in tar_buffer
 * @param consumed[out]     ptr to save bytes used in callbacks (or skipped at end of file)
 *
 * @return  CY_UNTAR_SUCCESS
 *          CY_UNTAR_ERROR
 */
static cy_untar_result_t cy_untar_parse_process_data(cy_untar_context_t *ctxt, uint32_t stream_offset, uint8_t *buffer, uint32_t size, uint32_t *consumed)
{
    cy_untar_result_t result = CY_UNTAR_SUCCESS;

    /* data chunk here */
    /* check for special case of components.json */
    *consumed = 0;
    if (ctxt->already_parsed_components_json == 0)
    {
        result = cy_parse_component_json_data(ctxt, buffer, size);
        if (result == CY_UNTAR_SUCCESS)
        {
            *consumed = ctxt->files[CY_UNTAR_COMPONENTS_JSON_INDEX].size;
            ctxt->already_parsed_components_json = 1;
            ctxt->state = CY_TAR_PARSE_FIND_HEADER;
#if 1  //DEBUG
            {
                configPRINTF( ("%d:%s() parsed components.json done \n", __LINE__, __func__) );
                configPRINTF( ("       version : %s\n", ctxt->version) );
                configPRINTF( ("     num files : %d\n", ctxt->num_files_in_json) );
                int i;
                for (i = 0; i < ctxt->num_files_in_json; i++)
                {
                    configPRINTF( ("      file  %d : %s\n", i, ctxt->files[i].name) );
                    configPRINTF( ("               : %s\n", ctxt->files[i].type) );
                    configPRINTF( ("               : %ld\n", ctxt->files[i].size) );
                }
            }
#endif
            return CY_UNTAR_SUCCESS;
        }
        else if (CY_UNTAR_NOT_ENOUGH_DATA)
        {
            /* Data is not enough for components.json to be parsed
             * Signal for buffer to coalesce data until we have enough to parse.
             */
            ctxt->coalesce_needs = ( (ctxt->files[ctxt->current_file].size + (TAR_BLOCK_SIZE - 1)) / TAR_BLOCK_SIZE) * TAR_BLOCK_SIZE;
            CY_ASSERT((ctxt->coalesce_needs < sizeof(ctxt->coalesce_buffer)), "Needs > Buffer size");
            if (ctxt->coalesce_needs > sizeof(ctxt->coalesce_buffer))
            {
                configPRINTF( ("%s() Buffer needs > coalesce buffer. Reduce size of components.json or increase buffer size.\n",__func__) );
                return CY_UNTAR_ERROR;
            }
            /* this is normal when coalescing data */
            return CY_UNTAR_NOT_ENOUGH_DATA;
        }
        /* components.json parse failure */
        configPRINTF( ("%d:%s PARSE FAILURE\n", __LINE__, __func__) );
        return CY_UNTAR_COMPONENTS_JSON_PARSE_FAIL;
    }
    else
    {
        /* check to see if this is data for the current file */
        uint32_t stream_file_start = ctxt->files[ctxt->current_file].header_offset + TAR_BLOCK_SIZE;
        uint32_t file_offset = stream_offset  - stream_file_start;
        uint32_t data_for_file = ctxt->files[ctxt->current_file].size - file_offset;

        if ( data_for_file > size )
        {
            data_for_file = size;
        }

        if (data_for_file != 0)
        {
            if (ctxt->cb_func != NULL)
            {
                cy_untar_result_t result;
                file_offset = stream_offset - ctxt->files[ctxt->current_file].header_offset - TAR_BLOCK_SIZE;
                /* pass along data to be written */
                result = ctxt->cb_func(ctxt, ctxt->current_file, buffer, file_offset, data_for_file, ctxt->cb_arg);
                if (result != CY_UNTAR_SUCCESS)
                {
                    configPRINTF( ("%d:%s    CALBACK returned FAILURE\n", __LINE__, __func__, result) );
                    return result;
                }
            }
            ctxt->files[ctxt->current_file].processed += data_for_file;
            *consumed = data_for_file;
//            configPRINTF( ("      file  %d : %s\n", ctxt->current_file, ctxt->files[ctxt->current_file].name) );
//            configPRINTF( ("               : %ld of %ld\n", ctxt->files[ctxt->current_file].processed, ctxt->files[ctxt->current_file].size) );
        }

        if (ctxt->files[ctxt->current_file].processed >= ctxt->files[ctxt->current_file].size)
        {
            /* change state to look for next header */
            ctxt->state = CY_TAR_PARSE_FIND_HEADER;
        }
    }
    return CY_UNTAR_SUCCESS;
}

/*************************************************************
 * Functions
 ************************************************************/
/**
 * @brief Determine if this is a tar header
 *
 * @param buffer[in]        ptr to data buffer
 * @param size[in]          size of buffer
 *
 * @return  CY_UNTAR_INVALID
 *          CY_UNTAR_SUCCESS
 *          CY_UNTAR_NOT_ENOUGH_DATA
 */
cy_untar_result_t cy_is_tar_header( uint8_t *buffer, uint32_t size )
{
    ustar_header_t *hdr = (ustar_header_t *)buffer;
    if ( (hdr == NULL) || (size < sizeof(ustar_header_t)))
    {
        return CY_UNTAR_NOT_ENOUGH_DATA;
    }
    if (strncmp((char*)hdr->magic, TMAGIC, TMAGLEN) != 0)
    {
//        configPRINTF( ("is_tar_header() invalid ustar magic:%s\n", hdr->magic) );
        return CY_UNTAR_INVALID;
    }
    configPRINTF( ("is_tar_header(), hdr->magic: %s, TMAGIC: %s, TMAGLEN: %d\n", hdr->magic, TMAGIC, TMAGLEN) );
    return CY_UNTAR_SUCCESS;
}

/**
 * @brief Initialize our tar context
 *
 * @param ctxt[in,out]      ptr to context structure
 * @param cb_func[in]       callback function
 *
 * @return  CY_UNTAR_SUCCESS
 *          CY_UNTAR_ERROR
 */
cy_untar_result_t cy_untar_init( cy_untar_context_t *ctxt, untar_write_callback_t cb_func, void *cb_arg )
{
    if ( (ctxt == NULL) || (cb_func == NULL) )
    {
        return CY_UNTAR_ERROR;
    }
    memset (ctxt, 0x00, sizeof(cy_untar_context_t));
    ctxt->state = CY_TAR_PARSE_FIND_HEADER;
    ctxt->cb_func = cb_func;
    ctxt->cb_arg  = cb_arg;
    ctxt->magic = CY_UNTAR_CONTEXT_MAGIC;

    return CY_UNTAR_SUCCESS;
}

/**
 * @brief De-Initialize our tar context
 *
 * @param ctxt[in,out]      ptr to context structure
 *
 * @return  CY_UNTAR_SUCCESS
 *          CY_UNTAR_ERROR
 */
cy_untar_result_t cy_untar_deinit( cy_untar_context_t *ctxt )
{
    if (ctxt == NULL)
    {
        configPRINTF( ("%d:%s BAD ARGS\n", __LINE__, __func__) );
        return CY_UNTAR_ERROR;
    }
    memset (ctxt, 0x00, sizeof(cy_untar_context_t));
    return CY_UNTAR_SUCCESS;
}

/**
 * @brief Parse the tar data
 *
 * NOTE: This is meant to be called for each chunk of data received
 *       callback will be invoked when there is data to write
 *
 * @param ctxt[in,out]      ptr to context structure, gets updated
 * @param stream_offset[in] offset into entire stream
 * @param tar_buffer[in]    ptr to the next buffer of input
 * @param size[in]          bytes in tar_buffer
 * @param consumed[out]     ptr to save bytes used in callbacks (or skipped at end of file)
 *
 * @return  CY_UNTAR_SUCCESS
 *          CY_UNTAR_ERROR
 */
cy_untar_result_t cy_untar_parse( cy_untar_context_t *ctxt, uint32_t in_stream_offset, uint8_t *in_buffer, uint32_t in_size, uint32_t *consumed)
{
    cy_untar_result_t   result;

    uint8_t         *curr_buffer = in_buffer;
    uint32_t        curr_stream_offset = in_stream_offset;
    uint32_t        curr_size = in_size;

    if ( (ctxt == NULL) || (in_buffer == NULL) || (in_size == 0) || (consumed == NULL))
    {
        configPRINTF( ("%d:%s BAD ARGUMENTS\n", __LINE__, __func__) );
        return CY_UNTAR_ERROR;
    }

    /* we haven't consumed any data yet */
    *consumed = 0;
    if (ctxt->magic != CY_UNTAR_CONTEXT_MAGIC)
    {
        /* our untar context not initialized! */
        configPRINTF( ("%d:%s BAD UNTAR context MAGIC\n", __LINE__, __func__) );
        return CY_UNTAR_ERROR;
    }

    /* TAR is organized in TAR_BLOCK_SIZE chunks
     * a ustar_header is < TAR_BLOCK_SIZE
     * "components.json" *may* be larger than TAR_BLOCK_SIZE
     * but we constrain the sie to less than (TAR_BLOCK_SIZE * 2)
     * */

    while (curr_size > 0)
    {
        uint8_t used_coalesce;
        uint8_t *buff_to_use;
        uint32_t size_to_use;
        uint32_t stream_offset_to_use;
        uint32_t bytes_consumed;

        /* Have we indicated that we need to coalesce data ?
         * We will try to use this sparingly as it takes an extra copy of data.
         *
         * At start, ctxt->coalesce_needs = 0x00 so we do not trigger this.
         */
        if (ctxt->coalesce_bytes < ctxt->coalesce_needs)
        {
            /* copy data to fill the coalesce needs
             * coalesce_needs shall never be larger than the coalesce_buffer!
             */
            uint32_t copy_size = (ctxt->coalesce_needs - ctxt->coalesce_bytes);
            if (copy_size > curr_size)
            {
                copy_size = curr_size;
            }
            memcpy(&ctxt->coalesce_buffer[ctxt->coalesce_bytes], curr_buffer, copy_size);

            if (ctxt->coalesce_bytes == 0 )
            {
                ctxt->coalesce_stream_offset = curr_stream_offset;
            }
            ctxt->coalesce_bytes += copy_size;
            curr_size -= copy_size;
            curr_buffer += copy_size;
            *consumed += copy_size;
            curr_stream_offset += copy_size;

            if (*consumed == in_size)
            {
                /* we consumed all the data for the coalesce buffer, return */
                return CY_UNTAR_SUCCESS;
            }
            /* we can fall through to the next tests as we have set the pertinent variables */

        }

        /* assume we will use the direct buffer rather than the coalesce buffer
         * We set these here after the coalesce check in case we copied into the buffer
         */
        bytes_consumed = 0;
        used_coalesce = 0;
        buff_to_use = curr_buffer;
        size_to_use = curr_size;
        stream_offset_to_use = curr_stream_offset;

//        configPRINTF( ("%d:%s() ctxt->state: %d  consumed: %ld\n", __LINE__, __func__, ctxt->state, *consumed) );
        if (ctxt->state == CY_TAR_PARSE_FIND_HEADER)
        {
            /*
             * NOTES:
             *    ustar header always fits in one TAR_BLOCK_SIZE
             *    ustar header always stars on TAR_BLOCK_SIZE boundary
             */

            if ( (ctxt->coalesce_needs > 0) &&
                 (ctxt->coalesce_bytes >= ctxt->coalesce_needs) )
            {
                /* we have data in the coalesce buffer - use it? */
                if (ctxt->coalesce_bytes < TAR_BLOCK_SIZE)
                {
                    /* we need to fill the coalesce buffer before we can parse */
                    ctxt->coalesce_needs = TAR_BLOCK_SIZE;
                    continue;
                }
                buff_to_use = ctxt->coalesce_buffer;
                size_to_use = ctxt->coalesce_bytes;
                stream_offset_to_use = ctxt->coalesce_stream_offset;
                used_coalesce = 1;
            }

            if (size_to_use < TAR_BLOCK_SIZE)
            {
                /* we need to fill the coalesce buffer before we can parse */
                ctxt->coalesce_needs = TAR_BLOCK_SIZE;
                continue;
            }

            if ( (stream_offset_to_use % TAR_BLOCK_SIZE) != 0)
            {
                bytes_consumed = TAR_BLOCK_SIZE - (stream_offset_to_use % TAR_BLOCK_SIZE);
                if ( (used_coalesce == 1)  && (bytes_consumed > ctxt->coalesce_bytes))
                {
                    /* just skip the bytes in coalesce buffer */
                    bytes_consumed = ctxt->coalesce_bytes;
                }
            }
            else
            {
                /* A ustar header is always TAR_BLOCK_SIZE  */
                result = cy_untar_parse_process_header(ctxt, stream_offset_to_use, buff_to_use, size_to_use);
                if (result != CY_UNTAR_SUCCESS)
                {
                    configPRINTF( ("%d:%s() cy_untar_parse_process_header() fail  return %d\n", __LINE__, __func__, result) );
                    return result;
                }
                bytes_consumed = TAR_BLOCK_SIZE;    /* always 1 TAR_BLOCK_SIZE for ustar header */
            }
        }
        else if(ctxt->state == CY_TAR_PARSE_DATA)
        {
            /*
             * NOTES:
             *    "components.json" data may require more than one TAR_BLOCK_SIZE of data
             */

            if (ctxt->coalesce_needs > 0)
            {
                /* we have data in the coalesce buffer - use it */
                if (ctxt->coalesce_bytes < ctxt->coalesce_needs)
                {
                    /* we need to fill the coalesce buffer more before we can parse */
                    continue;
                }
                buff_to_use = ctxt->coalesce_buffer;
                size_to_use = ctxt->coalesce_bytes;
                stream_offset_to_use = ctxt->coalesce_stream_offset;
                used_coalesce = 1;
            }

//            configPRINTF( ("%d:%s() offset: %ld sz: %ld\n", __LINE__, __func__, stream_offset_to_use, size_to_use) );
            result = cy_untar_parse_process_data(ctxt, stream_offset_to_use, buff_to_use, size_to_use, &bytes_consumed);
            if (result == CY_UNTAR_NOT_ENOUGH_DATA)
            {
                /* continue will put data into coalesce buffer and consume the data
                 */
                continue;
            }
            if (result != CY_UNTAR_SUCCESS)
            {
                /* data parse fail */
                configPRINTF( ("%d:%s() cy_untar_parse_process_data() fail  return %d\n", __LINE__, __func__, result) );
                return result;
            }
        }

        /* handle use of coalesce buffer here */
        if (used_coalesce == 1)
        {
            if ( (bytes_consumed != 0 ) && (bytes_consumed < ctxt->coalesce_bytes) )
            {
                /* remove skipped data */
                uint32_t copy_size = ctxt->coalesce_bytes - bytes_consumed;
                memcpy(ctxt->coalesce_buffer, &ctxt->coalesce_buffer[bytes_consumed], copy_size);
                ctxt->coalesce_bytes -= bytes_consumed;
                /* If there are no more bytes in the coalesce buffer, clear out the starting stream offset.
                 * We will see it is zero when we add bytes to the buffer and set the stream offset correctly.
                 */
                if (ctxt->coalesce_bytes == 0)
                {
                    ctxt->coalesce_stream_offset = 0;
                }
                else
                {
                    ctxt->coalesce_stream_offset += bytes_consumed;
                }
            }
            else
            {
                /* we used it all, clear it */
                ctxt->coalesce_bytes = 0;
                ctxt->coalesce_needs = 0;
                ctxt->coalesce_stream_offset = 0;
                memset(ctxt->coalesce_buffer, 0xFF, sizeof(ctxt->coalesce_buffer));
            }
        }
        else
        {
            /* keep track of consumed input data */
            *consumed += bytes_consumed;
            curr_buffer += bytes_consumed;
            curr_size -= bytes_consumed;
            curr_stream_offset += bytes_consumed;
        }
    }   /* while we haven't consumed all the data */

    ctxt->bytes_processed += *consumed;
    return CY_UNTAR_SUCCESS;
}
