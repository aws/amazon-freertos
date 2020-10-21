/*
 * Amazon FreeRTOS CELLULAR Preview Release
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#ifndef __CELLULAR_AT_CORE_H__
#define __CELLULAR_AT_CORE_H__

/* Standard includes */
#include <stdbool.h>
#include <string.h>

/* Standard includes. */
#include <stdint.h>

/*-----------------------------------------------------------*/

/**
 * @brief Maximum size of an AT string.
 */
#define CELLULAR_AT_MAX_STRING_SIZE    ( 256U )

/**
 * @brief Maximum size of an AT prefix.
 */
#define CELLULAR_AT_MAX_PREFIX_SIZE    ( 32 )


#define ARRY_SIZE( x )    ( sizeof( x ) / sizeof( x[ 0 ] ) )

/*-----------------------------------------------------------*/

/**
 * @brief Represents error codes returned from AT Core APIs.
 */
typedef enum CellularATError
{
    CELLULAR_AT_SUCCESS = 0,   /**< The operation was successful. */
    CELLULAR_AT_BAD_PARAMETER, /**< One or more of the input parameters is not valid. */
    CELLULAR_AT_NO_MEMORY,     /**< Memory allocation failure. */
    CELLULAR_AT_UNSUPPORTED,   /**< The operation is not supported. */
    CELLULAR_AT_MODEM_ERROR,   /**< Error in modem response. */
    CELLULAR_AT_ERROR,         /**< Generic Error or boolean false. */
    CELLULAR_AT_UNKNOWN        /**< Any other error other than the above mentioned ones. */
} CellularATError_t;

/*-----------------------------------------------------------*/

/**
 * @brief Remove prefix from a string
 *
 * Many AT responses contain prefix of the form +XXXX. This function removes the
 * prefix by moving the pointer after the prefix. For example:
 *
 * Input:
 * +---+---+---+---+---+---+---+---+---+---+---+
 * | + | C | P | I | N | : | R | E | A | D | Y |
 * +---+---+---+---+---+---+---+---+---+---+---+
 *   ^
 *   |
 *  *ppString
 *
 * Output:
 * +---+---+---+---+---+---+---+---+---+---+---+
 * | + | C | P | I | N | : | R | E | A | D | Y |
 * +---+---+---+---+---+---+---+---+---+---+---+
 *                           ^
 *                           |
 *                          *ppString
 *
 * Note that a prefix is always followed by colon (':') and this function
 * removes colon as well.
 *
 * @param[in/out] ppString The AT response to remove the prefix from. In
 * case of success, the pointer is updated to move past the prefix.
 *
 * @return CELLULAR_AT_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularATError_t Cellular_ATRemovePrefix( char ** ppString );

/**
 * @brief Remove all leading white spaces from an AT response.
 *
 * Leading white spaces are removed by updating the pointer to the first
 * non-white space character. For example:
 *
 * Input:
 * +--+--+--+---+---+---+---+---+---+---+---+------+
 * |  |  |  | r | e | s | p | o | n | s | e | '\0' |
 * +--+--+--+---+---+---+---+---+---+---+---+------+
 *  ^
 *  |
 * *ppString
 *
 * Output:
 * +--+--+--+---+---+---+---+---+---+---+---+------+
 * |  |  |  | r | e | s | p | o | n | s | e | '\0' |
 * +--+--+--+---+---+---+---+---+---+---+---+------+
 *            ^
 *            |
 *           *ppString
 *
 * @param[in/out] ppATResponse The AT response to remove the leading white spaces
 * from. In case of success, the pointer is updated to the first non white space
 * character.
 *
 * @return CELLULAR_AT_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularATError_t Cellular_ATRemoveLeadingWhiteSpaces( char ** ppString );

/**
 * @brief Remove all trailing white spaces from an AT response.
 *
 * Trailing spaces are removed by making the character next to the last
 * non white space character NULL ('\0'). For example:
 *
 * Input:
 * +---+---+---+---+---+---+---+---+--+--+--+------+
 * | r | e | s | p | o | n | s | e |  |  |  | '\0' |
 * +---+---+---+---+---+---+---+---+--+--+--+------+
 *
 * Output:
 * +---+---+---+---+---+---+---+---+------+--+--+------+
 * | r | e | s | p | o | n | s | e | '\0' |  |  | '\0' |
 * +---+---+---+---+---+---+---+---+------+--+--+------+
 *
 * @param[in/out] pString The AT response to remove the trailing white
 * spaces from.
 *
 * @return CELLULAR_AT_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularATError_t Cellular_ATRemoveTrailingWhiteSpaces( char * pString );

/**
 * @brief Remove all white spaces from an AT response.
 *
 * White spaces are removed by copying the non-white space characters to the
 * beginning. The character next to the last non-white space character is set
 * to the null character ('\0'). For example:
 *
 * Input:
 * +--+--+--+---+---+---+---+---+---+---+-- +---+------+
 * |  |  |  | r | e | s | p |   | o | n | s | e | '\0' |
 * +--+--+--+---+---+---+---+---+---+---+---+---+------+
 *
 * Output:
 * +---+---+---+---+---+---+---+---+------+------+
 * | r | e | s | p | o | n | s | e | '\0' | '\0' |
 * +---+---+---+---+---+---+---+---+------+------+
 *
 * @param[in/out] pString The AT response to remove the white spaces from.
 *
 * @return CELLULAR_AT_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularATError_t Cellular_ATRemoveAllWhiteSpaces( char * pString );

/**
 * @brief Remove outermost double quotes from an AT response.
 *
 * If the first character is double quote, it is removed by updating the pointer
 * to point to the next character. If the last character is double quote, it
 * is removed by making it the null character. Nothing else is done in any other
 * case. For example:
 *
 * Input:
 * +---+---+---+---+---+---+---+---+---+---+------+
 * | " | r | e | s | p | o | n | s | e | " | '\0' |
 * +---+---+---+---+---+---+---+---+---+---+------+
 *   ^
 *   |
 *  *ppString
 *
 * Output:
 * +---+---+---+---+---+---+---+---+---+------+------+
 * | " | r | e | s | p | o | n | s | e | '\0' | '\0' |
 * +---+---+---+---+---+---+---+---+---+------+------+
 *       ^
 *       |
 *      *ppString
 *
 * @param[in/out] ppString The AT response to remove the double quotes
 * from.
 *
 * @return CELLULAR_AT_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularATError_t Cellular_ATRemoveOutermostDoubleQuote( char ** ppString );

/**
 * @brief Remove all double quotes from an AT response.
 *
 * Double quotes are removed by copying all the other characters to the
 * beginning. The character next to the last character is set to the null
 * character ('\0'). For example:
 *
 * Input:
 * +---+---+---+---+---+---+---+---+---+---+---+---+------+
 * | " | r | e | s | " | p | " | o | n | s | e | " | '\0' |
 * +---+---+---+---+---+---+---+---+---+---+---+---+------+
 *
 * Output:
 * +---+---+---+---+---+---+---+---+------+---+---+---+------+
 * | r | e | s | p | o | n | s | e | '\0' | s | e | " | '\0' |
 * +---+---+---+---+---+---+---+---+------+---+---+---+------+
 *
 * @param[in/out] ppString The AT response to remove the double quotes
 * from.
 *
 * @return CELLULAR_AT_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularATError_t Cellular_ATRemoveAllDoubleQuote( char * pString );

/**
 * @brief Extract the next token based on comma (',') as delimiter.
 *
 * @param[in/out] ppString The AT response to extract the token from.
 * @param[in/out] ppTokOutput The output parameter to return the location of the
 * token.
 *
 * @return CELLULAR_AT_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularATError_t Cellular_ATGetNextTok( char ** ppString,
                                         char ** ppTokOutput );

/**
 * @brief Extract the next token based on the provided delimiter.
 *
 * This function uses *ppATResponse as the starting location to scan for tokens
 * which are sequences of contiguous characters separated by delimiters. When it
 * finds a token, *ppOutToken is updated to point to starting location of the
 * token and *ppATResponse is updated to point to the next character after the
 * token. This ensures that the next call to this function will extract the next
 * occurrence of the token.
 *
 * @param[in/out] ppString The AT response to extract the token from.
 * @param[in] pDelimiter The delimiter string.
 * @param[in/out] ppOutToken The output parameter to return the location of the
 * token.
 *
 * @return CELLULAR_AT_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularATError_t Cellular_ATGetSpecificNextTok( char ** ppString,
                                                 const char * pDelimiter,
                                                 char ** ppTokOutput );

/**
 * @brief Convert HEX string to HEX.
 *
 * This function requires the provided string to be of even length and returns
 * CELLULAR_AT_BAD_PARAMETER if it is not. It also requires the output buffer to be
 * of exactly half the length of the given hex string to ensure that it exactly
 * fits the converted data.
 *
 * It reads two characters and constructs a HEX byte by treating them upper and
 * lower nibble of the byte. For example:
 *
 * Input:
 * +---+---+---+---+------+
 * | 1 | 0 | A | B | '\0' |
 * +---+---+---+---+------+
 *
 * Output:
 * +----+-----+------+
 * | 16 | 171 | '\0' |
 * +----+-----+------+
 *
 * Decimal 16 is 0x10 and decimal 171 is 0xAB.
 *
 * @param[in] pStrData The hex string to convert to HEX.
 * @param[in] ppHexData The buffer to return the converted HEX data into.
 * @param[in] hexDataLen The length of the buffer.
 *
 * @return CELLULAR_AT_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularATError_t Cellular_ATHexStrToHex( const char * pString,
                                          uint8_t * const * const ppHexData,
                                          uint16_t hexDataLen );

/**
 * @brief Check if a string is numeric.
 *
 * A string is numeric if all the characters in it are digits. For example,
 * "1234" is numeric but "123YD" is not because 'Y' and 'D' are not digits.
 *
 * @param[in] pString The input string to check.
 * @param[out] result = true; The bool output parameter to return whether or not the
 * string is numeric.
 *
 * @return CELLULAR_AT_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularATError_t Cellular_ATIsStrDigit( const char * pString,
                                         bool * pResult );

/**
 * @brief check if a string as prefix present by determine present of ':'
 *
 * @param[in] pString: input string
 * @param[out] result = true/false
 *
 * @return CELLULAR_AT_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularATError_t Cellular_ATIsPrefixPresent( const char * pString,
                                              bool * result );

/**
 * @brief duplicate string from pSrc to ppDst, malloc is use to allocate mem space for ppDst
 *
 * @param[in] pSrc: input string to be copied
 * @param[out] ppDst: destination pointer
 *
 * @return CELLULAR_AT_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularATError_t Cellular_ATStrDup( char ** ppDst,
                                     const char * pSrc );

/**
 * @brief check if a string starts with certain prefix
 *
 * @param[in] pString: input string
 * @param[in] pString: input prefix
 * @param[out] result: return true if prefix is at start of pString, else false
 *
 * @return CELLULAR_AT_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularATError_t Cellular_ATStrStartWith( const char * pString,
                                           const char * pPrefix,
                                           bool * result );

/**
 * @brief check if certain success code/error code present in the input buffer
 *
 * @param[in] pInputBuf: the haystack buffer
 * @param[in] ppKeyList: list of keys
 * @param[in] keyListLen: size of the keyList array
 * @param[out] result: return true if any of Keys in ppKeyList is found in  is at start of pString, else false
 *  *
 * @return CELLULAR_AT_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularATError_t Cellular_ATcheckErrorCode( const char * pInputBuf,
                                             const char * const * const ppKeyList,
                                             size_t keyListLen,
                                             bool * result );

/**
 * @brief Convert string to int32_t.
 *
 * @param[in] pStr: the input string buffer.
 * @param[in] base: Numerical base (radix) of pStr.
 * Input string should not contain leading space or zero.
 * @param[out] pResult: converted int32_t result.
 *  *
 * @return CELLULAR_AT_SUCCESS if the operation is successful, otherwise an
 * error code indicating the cause of the error.
 */
CellularATError_t Cellular_ATStrtoi( const char * pStr,
                                     int32_t base,
                                     int32_t * pResult );

#endif /* __CELLULAR_AT_CORE_H__ */
