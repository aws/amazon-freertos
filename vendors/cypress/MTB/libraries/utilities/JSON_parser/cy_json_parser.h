/*
 * Copyright 2019-2020 Cypress Semiconductor Corporation
 * SPDX-License-Identifier: Apache-2.0
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file
 * The JSON parser utility library provides helper functions to parse JSON objects
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "cy_result_mw.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
/** Results returned by JSON library */
#define CY_RSLT_MODULE_JSON_ERR_CODE_START          (0) /** JSON parser result code base */
#define CY_RSLT_JSON_ERROR_BASE                     CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_JSON_BASE, CY_RSLT_MODULE_JSON_ERR_CODE_START)

#define CY_RSLT_JSON_GENERIC_ERROR                  ((cy_rslt_t)(CY_RSLT_JSON_ERROR_BASE + 1)) /** JSON parser generic error result */

#define OBJECT_START_TOKEN        '{'
#define OBJECT_END_TOKEN          '}'

#define ARRAY_START_TOKEN         '['
#define ARRAY_END_TOKEN           ']'

#define STRING_TOKEN              '"'

#define START_OF_VALUE            ':'

#define COMMA_SEPARATOR           ','

#define ESCAPE_TOKEN              '\\'

#define TRUE_TOKEN                't'

#define FALSE_TOKEN               'f'

#define NULL_TOKEN                'n'

/******************************************************
 *                   Enumerations
 ******************************************************/
/******************************************************************************/
/** \addtogroup group_json_enums *//** \{ */
/******************************************************************************/

/** JSON data types */
typedef enum
{
    JSON_STRING_TYPE,  /**< JSON string datatype */
    JSON_NUMBER_TYPE,  /**< JSON number datatype */
    JSON_VALUE_TYPE,   /**< JSON value datatype */
    JSON_ARRAY_TYPE,   /**< JSON array datatype */
    JSON_OBJECT_TYPE,  /**< JSON object */
    JSON_BOOLEAN_TYPE, /**< JSON boolean datatype */
    JSON_NULL_TYPE,    /**< JSON null object */
    UNKNOWN_JSON_TYPE  /**< JSON unknown type */
} cy_JSON_type_t;

/** \} */

/******************************************************
 *                 Type Definitions
 ******************************************************/
/******************************************************************************/
/** \addtogroup group_json_structures *//** \{ */
/******************************************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
/** JSON parser object */
typedef struct cy_JSON_object {

    char*               object_string;        /**< JSON object as a string */
    uint8_t             object_string_length; /**< Length of the JSON string */
    cy_JSON_type_t      value_type;           /**< JSON data type of value parsed */
    char*               value;                /**< JSON value parsed */
    uint16_t            value_length;         /**< JSON length of value parsed */
    struct cy_JSON_object* parent_object;     /**< Pointer to parent JSON object */
} cy_JSON_object_t;

/** Callback function used for registering with JSON parse
 *
 * @param[in] json_object : JSON object which contains the key=value pair parsed by the JSON parser
 * @param[in] arg         : User argument passed in when registering the callback
 *
 * @return cy_rslt_t
 */
typedef cy_rslt_t (*cy_JSON_callback_t)( cy_JSON_object_t* json_object, void *arg );

/** \} */

#define JSON_IS_NOT_ESC_CHAR( ch ) ( ( ch != '\b' )  &&  \
                                     ( ch != '\f' ) &&  \
                                     ( ch != '\n' ) &&  \
                                     ( ch != '\r' ) &&  \
                                     ( ch != '\t' ) &&  \
                                     ( ch != '\"' ) &&  \
                                     ( ch != '\\' ) )

/******************************************************
 *                 Global Variables
 ******************************************************/

/*****************************************************************************/
/**
 *
 *  @addtogroup group_json_func
 *
 * The JSON parser utility library provides helper functions to parse JSON objects
 *
 *
 *  @{
 */
/*****************************************************************************/


/** Register callback to be invoked by JSON parser while parsing the JSON data
 *
 * @param[in] json_callback : Pointer to the callback function to be invoked while parsing the JSON data
 * @param[in] arg           : User argument passed in when registering the callback
 *
 * @return cy_rslt_t
 */
cy_rslt_t cy_JSON_parser_register_callback( cy_JSON_callback_t json_callback, void *arg );

/** Returns the current callback function registered with the JSON parser
 *
 * @return @ref cy_JSON_callback_t
 */
cy_JSON_callback_t cy_JSON_parser_get_callback( void );

/** Parse the JSON data.
 *
 *  This function will parse the JSON input string through a single parse, calling a callback whenever it encounters milestones
 *  an object, passing in object name, JSON value type, and a value (if value is string or number )
 *
 * @param[in] json_input   : Pointer to the JSON data
 * @param[in] input_length : Length of the JSON data pointed by `json_input1`
 *
 * @return cy_rslt_t
 *
 * @note: Currently escape values are not supported.
 */
cy_rslt_t cy_JSON_parser( const char* json_input, uint32_t input_length );

/** @} */

#ifdef __cplusplus
} /*extern "C" */
#endif
