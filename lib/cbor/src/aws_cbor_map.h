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
#ifndef AWS_CBOR_MAP_H
#define AWS_CBOR_MAP_H

/**
 * @file
 * @brief Map type functions
 */

#include "aws_cbor.h"
#include <stdint.h>

/**
 * @brief Writes the opening sequence for a CBOR map
 * @param cbor_handle_t Handle for the CBOR data struct.
 */
void CBOR_OpenMap( cbor_handle_t /*cbor_data*/ );

/**
 * @brief Writes the closing sequence for a CBOR map
 * @param cbor_handle_t Handle for the CBOR data struct.
 */
void CBOR_CloseMap( cbor_handle_t /*cbor_data*/ );

/**
 * @brief Checks if key matches the provided string
 * @param cbor_handle_t Handle for the CBOR data struct with the cursor pointing
 *     at a key string
 * @param "const char *" String to check against the cbor data struct
 */
bool CBOR_KeyIsMatch( cbor_handle_t /*cbor_data*/, const char * /*key*/ );

/**
 * @brief Searches for a key in a CBOR map
 * @param cbor_handle_t Handle for the CBOR data struct with the cursor pointing
 *     at a key string
 * @param "const char *" String to check against the cbor data struct
 */
void CBOR_SearchForKey( cbor_handle_t /*cbor_data*/, const char * /*key*/ );

/**
 * @brief Appends a key-value pair to a map
 * @warning CBOR_AppendKey() does not check for duplication
 */
void CBOR_AppendKey( cbor_handle_t /*cbor_data*/, const char * /*key*/,
                     write_function_t /*write_function*/, const void * /*value*/ );

/**
 * @brief Assigns a value to a key in a CBOR map
 */
void CBOR_AssignKey( cbor_handle_t /*cbor_data*/, const char * /*key*/,
                     write_function_t /*write_function*/, const void * /*value*/ );

/**
 * @brief  Gets the size (in bytes) of a map data item
 * @param  "const cbor_byte_t *" pointer to the head of the data item
 * @return cbor_ssize_t            size (in bytes) of the map
 */
cbor_ssize_t CBOR_MapSize( const cbor_byte_t * /*ptr*/ );

/**
 * @brief Writes a map into the CBOR buffer
 *
 * Writes the contents of a CBOR buffer (map) into another CBOR buffer.
 *
 * @param cbor_handle_t   Handle for the CBOR destination data struct.
 * @param  "const void *" Handle for the CBOR source data struct.
 *
 */
void CBOR_WriteMap( cbor_handle_t /*cbor_data*/, const void * /*input*/ );

/**
 * @brief Reads a map (as a CBOR buffer) from another CBOR buffer
 * @param cbor_handle_t  Handle for the CBOR data struct.
 * @return               Handle to a new CBOR data struct.
 */
cbor_handle_t CBOR_ReadMap( cbor_handle_t /*cbor_data*/ );

#endif /* end of include guard: AWS_CBOR_MAP_H */
