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

/*
 * @file
 * The string utilities module is a collection of string conversion helpers to convert between integer and strings
 *
 */

#ifndef __CY_STRING_H__
#define __CY_STRING_H__

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/**
 *
 *  @addtogroup group_string_func
 *
 * The string utilities module is a collection of string conversion helpers to convert between integer and strings
 *
 *  @{
 */
/*****************************************************************************/

/*!
 ******************************************************************************
 * Convert a decimal or hexidecimal string to an integer.
 *
 * @param[in] str  The string containing the value.
 *
 * @return    The value represented by the string.
 */
uint32_t cy_generic_string_to_unsigned( const char* str );

/**
 * Converts a decimal/hexidecimal string (with optional sign) to a signed long int
 * Better than strtol or atol or atoi because the return value indicates if an error occurred
 *
 * @param[in] string     : The string buffer to be converted
 * @param[in] str_length : The maximum number of characters to process in the string buffer
 * @param[out] value_out : The unsigned in that will receive value of the the decimal string
 * @param[in] is_hex     : 0 = Decimal string, 1 = Hexidecimal string
 *
 * @return the number of characters successfully converted (including sign).  i.e. 0 = error
 *
 */
uint8_t cy_string_to_signed( const char* string, uint16_t str_length, int32_t* value_out, uint8_t is_hex );


/**
 * Converts a decimal/hexidecimal string to an unsigned long int
 * Better than strtol or atol or atoi because the return value indicates if an error occurred
 *
 * @param[in] string     : The string buffer to be converted
 * @param[in] str_length : The maximum number of characters to process in the string buffer
 * @param[out] value_out : The unsigned in that will receive value of the the decimal string
 * @param[in] is_hex     : 0 = Decimal string, 1 = Hexidecimal string
 *
 * @return the number of characters successfully converted.  i.e. 0 = error
 *
 */
uint8_t cy_string_to_unsigned( const char* string, uint8_t str_length, uint32_t* value_out, uint8_t is_hex );

/** @} */

#ifdef __cplusplus
}
#endif


#endif /* __CY_STRING_H__ */
