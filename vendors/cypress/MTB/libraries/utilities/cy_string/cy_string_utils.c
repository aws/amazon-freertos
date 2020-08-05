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

/** @file
 *  String utilities
 */

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "cy_string_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

/**
 ******************************************************************************
 * Convert an ASCII hex character into a nibble
 *
 * @param[in]  char     : The single hex character to convert to a nibble
 * @param[out] nibble   : Pointer to store The value of the nibble in the lower 4 bits
 *
 * @return    0 = SUCCESS
 *           -1 = not a hex character
 */
static inline char cy_hexchar_to_nibble( char hexchar, uint8_t* nibble )
{
    if ( ( hexchar >= '0' ) && ( hexchar <= '9' ) )
    {
        *nibble = (uint8_t)( hexchar - '0' );
        return 0;
    }
    else if ( ( hexchar >= 'A' ) && ( hexchar <= 'F' ) )
    {
        *nibble = (uint8_t) ( hexchar - 'A' + 10 );
        return 0;
    }
    else if ( ( hexchar >= 'a' ) && ( hexchar <= 'f' ) )
    {
        *nibble = (uint8_t) ( hexchar - 'a' + 10 );
        return 0;
    }
    return -1;
}

static uint8_t cy_string_to_generic( const char* string, uint16_t str_length,  uint32_t* value_out, uint8_t is_unsigned, uint8_t is_hex )
{
    uint8_t nibble;
    uint8_t characters_processed = 0;

    if ( string == NULL )
    {
        return 0;
    }

    *value_out = 0;

    while ( ( characters_processed != str_length ) &&
            ( 0 == cy_hexchar_to_nibble( *string, &nibble ) ) &&
            ( ( is_hex != 0 ) || ( nibble < 10 ) )
          )
    {
        if ( is_hex != 0 )
        {
            if ( ( ( *value_out > ( 0x7fffffff >> 4 ) ) && ( is_unsigned == 0 ) ) ||
                 ( *value_out > ( 0xffffffff >> 4 ) )
               )
            {
                break;
            }
            *value_out = ( *value_out << 4 ) + nibble;
        }
        else
        {
            if ( ( ( *value_out > ( 0x7fffffff / 10 ) ) && ( is_unsigned == 0 ) ) ||
                 ( *value_out > ( 0xffffffff / 10 ) )
               )
            {
                break;
            }
            *value_out = ( *value_out * 10 ) + nibble;
        }
        string++;
        characters_processed++;
    }

    return characters_processed;
}

/*!
 ******************************************************************************
 * Convert a decimal or hexidecimal string to an integer.
 *
 * @param[in] str  The string containing the value.
 *
 * @return    The value represented by the string.
 */
uint32_t cy_generic_string_to_unsigned( const char* str )
{
    uint32_t val = 0;
    uint8_t is_hex = 0;

    if ( (strncmp( str, "0x", 2) == 0) || (strncmp(str, "0X", 2) == 0))
    {
        is_hex = 1;
        str += 2;
    }

    cy_string_to_generic( str, strlen(str), &val, 1, is_hex );

    return val;
}

/**
 * Converts a decimal/hexidecimal string (with optional sign) to a signed long int
 * Better than strtol or atol or atoi because the return value indicates if an error occurred
 *
 * @param string[in]     : The string buffer to be converted
 * @param str_length[in] : The maximum number of characters to process in the string buffer
 * @param value_out[out] : The unsigned in that will receive value of the the decimal string
 * @param is_hex[in]     : 0 = Decimal string, 1 = Hexidecimal string
 *
 * @return the number of characters successfully converted (including sign).  i.e. 0 = error
 *
 */
uint8_t cy_string_to_signed( const char* string, uint16_t str_length, int32_t* value_out, uint8_t is_hex )
{
    uint8_t characters_processed = 0;
    uint8_t retval;
    char    first_char;

    if ( string == NULL )
    {
        return 0;
    }

    first_char = *string;

    if ( ( first_char == '-' ) || ( *string == '+' ) )
    {
        characters_processed++;
        string++;
        str_length--;
    }

    retval = cy_string_to_generic( string, str_length, (uint32_t*)value_out, 0, is_hex );
    if ( retval == 0 )
    {
        return 0;
    }

    if ( first_char == '-' )
    {
        *value_out = -(*value_out);
    }
    return (uint8_t) ( characters_processed + retval );
}

/**
 * Converts a decimal/hexidecimal string to an unsigned long int
 * Better than strtol or atol or atoi because the return value indicates if an error occurred
 *
 * @param string[in]     : The string buffer to be converted
 * @param str_length[in] : The maximum number of characters to process in the string buffer
 * @param value_out[out] : The unsigned in that will receive value of the the decimal string
 * @param is_hex[in]     : 0 = Decimal string, 1 = Hexidecimal string
 *
 * @return the number of characters successfully converted.  i.e. 0 = error
 *
 */
uint8_t cy_string_to_unsigned( const char* string, uint8_t str_length, uint32_t* value_out, uint8_t is_hex )
{
    return cy_string_to_generic( string, str_length,  value_out, 1, is_hex );
}

#ifdef __cplusplus
}
#endif
