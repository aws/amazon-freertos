/*
 * Amazon FreeRTOS
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */
#ifndef AWS_CBOR_H /* Guards against multiple inclusion */
#define AWS_CBOR_H

/**
 * @file
 * @brief Public interface for AWS CBOR
 * @see @glos{cbor}
 * @see @glos{key}
 * @see @glos{value}
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief CBOR object errors
 */
typedef enum
{
    /** No error occurred in the last function call */
    eCBOR_ERR_NO_ERROR = 0,

    /**
     * Unspecified error occurred.  A code path failed to specify a more
     * precise error code.
     */
    eCBOR_ERR_DEFAULT_ERROR,

    /**
     * The data type in the CBOR buffer does not match the data type of the
     * read function.  E.g. Calling ReadInt while on a string @glos{value}.
     */
    eCBOR_ERR_READ_TYPE_MISMATCH,

    /** Performed an unsupported but legal CBOR read operation */
    eCBOR_ERR_UNSUPPORTED_READ_OPERATION,

    /** Performed an unsupported but legal CBOR write operation */
    eCBOR_ERR_UNSUPPORTED_WRITE_OPERATION,

    /** Requested @glos{key} was not found in map */
    eCBOR_ERR_KEY_NOT_FOUND,

    /** Insufficient space for write operation */
    eCBOR_ERR_INSUFFICENT_SPACE,

    /** CBOR data handle is NULL */
    eCBOR_ERR_NULL_HANDLE,

    /** Pointer to @glos{key} string is NULL */
    eCBOR_ERR_NULL_KEY,

    /** pointer to @glos{value} is NULL */
    eCBOR_ERR_NULL_VALUE,
} cbor_err_t;

/** @brief Pointer to a CborData_s struct */
typedef struct CborData_s * cbor_handle_t;

/** @brief Pointer to write function for use with assigning values to keys */
typedef void (* write_function_t)( cbor_handle_t,
                                   void const * );

typedef char       * cbor_key_t;
typedef char const * cbor_const_key_t;
typedef char       * cbor_string_t;
typedef char const * cbor_const_string_t;
typedef int        cbor_int_t;
typedef int        cbor_ssize_t;
typedef uint8_t    cbor_byte_t;

/**
 * @brief Checks the error state of the CBOR object.
 * @param   cbor_handle_t  Handle for the CBOR data struct.
 * @return  cbor_err_t     The current error state of the CBOR object.
 */
cbor_err_t CBOR_CheckError( cbor_handle_t /*cbor_data*/ );

/**
 * @brief Clears the error state from the CBOR object
 * @param cbor_handle_t   Handle for the CBOR data struct.
 */
void CBOR_ClearError( cbor_handle_t /*cbor_data*/ );

/**
 * @brief Initializes user provided data buffer with AWS CBOR data structure.
 *
 * Creates a CBOR buffer object and allocates the amount of space the user
 * specifies, or the minimum buffer size, whichever is greater.  The buffer is
 * initialized with an empty map.
 *
 * @param  size size of the new buffer
 * @return A pointer to the AWS CBOR data structure
 * @return Returns NULL if unable to allocate space
 *
 * @see cbor_handle_t
 * @see CBOR_Delete
 */
cbor_handle_t CBOR_New( cbor_ssize_t /*size*/ );

/**
 * @brief Frees memory allocated by CBOR_New(0)
 * Frees memory previosuly allocated and nulls out the cbor_handle_t pointer
 * @param "cbor_handle_t *" handle for cbor_handle_t
 * @see CBOR_New
 */
void CBOR_Delete( cbor_handle_t * /*handle*/ );

/**
 * @brief Returns a pointer to the raw buffer
 *
 * @note The memory pointed to belongs to the CBOR handle.  Do not modify.
 * @note If the CBOR handle is deleted, the pointer should no longer be
 *     dereferenced.
 * @note If the buffer is needed after the buffer handle is destroyed, the
 *     buffer should be copied to memory owned by the caller.
 *
 * @code
 * cbor_byte_t * my_cbor_data = CBOR_GetRawBuffer(my_cbor_handle);
 * cbor_ssize_t  my_data_size = CBOR_GerBufferSize(my_cbor_handle);
 * UART_PrintBuffer(my_cbor_data, my_data_size);
 * @endcode
 *
 * @param  cbor_handle_t Handle for the CBOR data struct
 * @return               Pointer to the raw CBOR buffer
 * @see CBOR_GerBufferSize
 */
cbor_byte_t const * const CBOR_GetRawBuffer( cbor_handle_t /*handle*/ );

/**
 * @brief Returns the size of the CBOR buffer in bytes
 * @param  cbor_handle_t Handle for the CBOR data struct
 * @return               Size of the CBOR buffer in bytes
 * @see CBOR_GetRawBuffer
 */
cbor_ssize_t const CBOR_GetBufferSize( cbor_handle_t /*handle*/ );

/**
 * @brief Points read/write cursor to @glos{value} at given @glos{key}
 * @return True: Key was found
 * @return False: Key was NOT found
 */
bool CBOR_FindKey( cbor_handle_t /*cbor_data*/, cbor_const_key_t /*key*/ );

/**
 * @brief Assigns a @glos{key} with a string @glos{value}
 * @param cbor_handle_t   Handle for the CBOR data struct.
 * @param cbor_const_key_t  @glos{key}   - zero terminated string
 * @param cbor_const_string_t  @glos{value} - zero terminated string
 */
void CBOR_AssignKeyWithString( cbor_handle_t /*cbor_data*/,
                               cbor_const_key_t /*key*/, cbor_const_string_t /*value*/ );

/**
 * @brief Appends a @glos{key} with a string @glos{value}
 * @warning AppendKey... does not check for duplicate keys.
 * @param cbor_handle_t   Handle for the CBOR data struct.
 * @param cbor_const_key_t  @glos{key}   - zero terminated string
 * @param cbor_const_string_t  @glos{value} - zero terminated string
 */
void CBOR_AppendKeyWithString( cbor_handle_t /*cbor_data*/,
                               cbor_const_key_t /*key*/, cbor_const_string_t /*value*/ );

/**
 * @brief Reads string from CBOR buffer at @glos{key}
 * @note Returns pointer to newly allocated memory
 * @param cbor_handle_t   Handle for the CBOR data struct.
 * @param cbor_const_key_t  @glos{key}   - zero terminated string
 * @return "char *" @glos{value} - pointer to new zero terminated string
 */
char * CBOR_FromKeyReadString(
    cbor_handle_t /*cbor_data*/, cbor_const_key_t /*key*/ );

/**
 * @brief Assigns a @glos{key} with an integer @glos{value}
 * @param cbor_handle_t   Handle for the CBOR data struct.
 * @param cbor_const_key_t  @glos{key}   - zero terminated string
 * @param "cbor_int_t"           @glos{value} - integer
 */
void CBOR_AssignKeyWithInt( cbor_handle_t /*cbor_data*/,
                            cbor_const_key_t /*key*/, cbor_int_t /*value*/ );

/**
 * @brief Appends a @glos{key} with an integer @glos{value}
 * @warning AppendKey... does not check for duplicate keys.
 * @param cbor_handle_t   Handle for the CBOR data struct.
 * @param cbor_const_key_t  @glos{key}   - zero terminated string
 * @param "cbor_int_t"           @glos{value} - integer
 */
void CBOR_AppendKeyWithInt( cbor_handle_t /*cbor_data*/,
                            cbor_const_key_t /*key*/, cbor_int_t /*value*/ );

/**
 * @brief Reads integer from CBOR buffer at @glos{key}
 * @param cbor_handle_t   Handle for the CBOR data struct.
 * @param cbor_const_key_t  @glos{key}   - zero terminated string
 * @return "cbor_int_t" @glos{value}
 */
cbor_int_t CBOR_FromKeyReadInt(
    cbor_handle_t /*cbor_data*/, cbor_const_key_t /*key*/ );

/**
 * @brief Assigns a @glos{key} with a map.
 *
 * @param cbor_handle_t   Handle for the destination CBOR data struct
 * @param cbor_const_key_t  @glos{key}   - zero terminated string
 * @param cbor_handle_t   Handle for the source CBOR data struct
 */
void CBOR_AssignKeyWithMap( cbor_handle_t /*cbor_data*/,
                            cbor_const_key_t /*key*/, cbor_handle_t /*value*/ );

/**
 * @brief Appends a @glos{key} with a map.
 * @warning AppendKey... does not check for duplicate keys.
 * @param cbor_handle_t   Handle for the destination CBOR data struct
 * @param cbor_const_key_t  @glos{key}   - zero terminated string
 * @param cbor_handle_t   Handle for the source CBOR data struct
 */
void CBOR_AppendKeyWithMap( cbor_handle_t /*cbor_data*/,
                            cbor_const_key_t /*key*/, cbor_handle_t /*value*/ );

/**
 * @brief Reads map from CBOR buffer at @glos{key}
 * @note Returns pointer to newly allocated memory
 * @param cbor_handle_t    Handle for the CBOR data struct.
 * @param "const char *"   @glos{key}   - zero terminated string
 * @return "cbor_handle_t" @glos{value} - pointer to new copy of CBOR map
 */
cbor_handle_t CBOR_FromKeyReadMap(
    cbor_handle_t /*cbor_data*/, cbor_const_key_t /*key*/ );

/**
 * @brief Concatenates the source map to the destination map
 *
 * @warning AppendMap does not check for duplicate keys.
 *
 * CBOR_AppendMap copies the key value pairs from the source map to the
 * destination map.
 *
 * @param dest cbor_handle_t for destination map
 * @param src cbor_handle_t for source map
 */
void CBOR_AppendMap( cbor_handle_t /*dest*/, cbor_handle_t /*src*/ );

#endif /* ifndef AWS_CBOR_H */

/*
 * End of File
 */
