/*
 * Amazon FreeRTOS
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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


/**
 * @file aws_json_utils.h
 * @brief Utility functions for JSON parsing.
 */
#ifndef AWS_JSON_UTILS_H_
#define AWS_JSON_UTILS_H_

#include <stdint.h>
#include "jsmn.h"
#include "FreeRTOS.h"

/**
 *
 *@brief Parse the json document string into an array of Json tokens.
 *
 * @param[in] pcDoc The input JSON string to be parsed.
 * @param[in] ulDocLength The length of the input json string to be parsed.
 * @param[out] pxJSMNTokens Array of Json tokens. The array is allocated by the user. The functions fills the array with parsed tokens.
 * @param[in] ulMaxTokens Maximum number of tokens that can be parsed.
 * @return Number of tokens parsed or < 0 if there is an error.
 */
int16_t JsonUtils_Parse( const char * const pcDoc, uint32_t ulDocLength, jsmntok_t * pxJSMNTokens, uint32_t ulMaxTokens );

/**
 *
 * @brief Gets the string value for a key within the JSON struct
 *
 * @param[in] pcDoc The input JSON string
 * @param[in] pxJSMNTokens Array of parsed JSON tokens
 * @param[in] sNumTokens Number of parsed JSON tokens
 * @param[in] pcKey The key for which the value needs to be found
 * @param[in] ulKeyLen Length of the key
 * @param[out] ppcValue Pointer to the value for the key
 * @param[out] pulValLen Length of the value
 */
void JsonUtils_GetStrValue( const char * const pcDoc, jsmntok_t * pxJSMNTokens,
		int16_t sNumTokens, const char* pcKey, uint32_t ulKeyLen, const char **ppcValue, uint32_t *pulValLen );

/**
 *
 * @brief Gets the integer (16 bit) value for a key within the JSON struct.
 *
 * @param[in] pcDoc The input JSON string.
 * @param[in] pxJSMNTokens Array of parsed JSON tokens
 * @param[in] sNumTokens Number of parsed JSON tokens
 * @param[in] pcKey The key for which the value needs to be found
 * @param[in] ulKeyLen Length of the key
 * @param[out] psValue Integer value for the key
 * @return
 */
BaseType_t JsonUtils_GetInt16Value( const char * const pcDoc, jsmntok_t * pxJSMNTokens,
		int16_t sNumTokens, const char* pcKey, uint32_t ulKeyLen, int16_t *psValue );

#endif /* AWS_JSON_UTILS_H_ */
