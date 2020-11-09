/*
 * Amazon FreeRTOS Cellular Preview Release
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

/**
 * @brief cellular HAL common AT command parsing functions.
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "iot_config.h"
#include "cellular_at_core.h"

/* Configure logs for AT Core functions. */
#ifdef IOT_LOG_LEVEL_AT_CORE
    #define LIBRARY_LOG_LEVEL        IOT_LOG_LEVEL_AT_CORE
#else
    #ifdef IOT_LOG_LEVEL_GLOBAL
        #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
    #else
        #define LIBRARY_LOG_LEVEL    IOT_LOG_NONE
    #endif
#endif

#define LIBRARY_LOG_NAME    ( "AT_CORE" )
#include "iot_logging_setup.h"

/*-----------------------------------------------------------*/

/**
 * @brief String validation results.
 */
typedef enum CellularATStringValidationResult
{
    CELLULAR_AT_STRING_VALID,     /**< String is valid. */
    CELLULAR_AT_STRING_EMPTY,     /**< String is empty. */
    CELLULAR_AT_STRING_TOO_LARGE, /**< String is too large. */
    CELLULAR_AT_STRING_UNKNOWN    /**< Unknown String validation state. */
} CellularATStringValidationResult_t;

/*-----------------------------------------------------------*/

static CellularATError_t validateString( const char * pString,
                                         CellularATStringValidationResult_t * pStringValidationResult );
static uint8_t _charToNibble( char c );

/*-----------------------------------------------------------*/

static CellularATError_t validateString( const char * pString,
                                         CellularATStringValidationResult_t * pStringValidationResult )
{
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;
    size_t stringLength = 0U;

    if( ( pString == NULL ) || ( pStringValidationResult == NULL ) )
    {
        atStatus = CELLULAR_AT_BAD_PARAMETER;
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        /* The strnlen() function returns strlen(s), if that is less than maxlen,
         * or maxlen if there is no null terminating ('\0') among the first
         * maxlen characters pointed to by s.
         *
         * stringLength == CELLULAR_AT_MAX_STRING_SIZE is valid because it means that
         * ( CELLULAR_AT_MAX_STRING_SIZE + 1 ) character is null terminating
         * character.*/
        stringLength = strnlen( pString, CELLULAR_AT_MAX_STRING_SIZE + 1U );

        if( stringLength == 0U )
        {
            *pStringValidationResult = CELLULAR_AT_STRING_EMPTY;
        }
        else if( stringLength > CELLULAR_AT_MAX_STRING_SIZE )
        {
            *pStringValidationResult = CELLULAR_AT_STRING_TOO_LARGE;
        }
        else
        {
            *pStringValidationResult = CELLULAR_AT_STRING_VALID;
        }
    }

    return atStatus;
}

/*-----------------------------------------------------------*/

CellularATError_t Cellular_ATIsPrefixPresent( const char * pString,
                                              bool * result )
{
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;
    CellularATStringValidationResult_t stringValidationResult = CELLULAR_AT_STRING_UNKNOWN;
    char * ptrPrefixChar = NULL;
    char * ptrChar = NULL;

    if( result == NULL )
    {
        atStatus = CELLULAR_AT_BAD_PARAMETER;
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        atStatus = validateString( pString, &stringValidationResult );

        if( atStatus == CELLULAR_AT_SUCCESS )
        {
            if( stringValidationResult != CELLULAR_AT_STRING_VALID )
            {
                atStatus = CELLULAR_AT_BAD_PARAMETER;
            }
        }
    }

    /* Find location of first ':'. */
    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        *result = true;

        ptrPrefixChar = strchr( pString, ( int32_t ) ':' );

        if( ptrPrefixChar == NULL )
        {
            *result = false;
        }
        else
        {
            /* There should be only '+', '_', characters or digit before seperator. */
            for( ptrChar = ( char * ) pString; ptrChar < ptrPrefixChar; ptrChar++ )
            {
                if( ( ( isalpha( ( ( int ) ( *ptrChar ) ) ) ) == 0 ) &&
                    ( ( isdigit( ( ( int ) ( *ptrChar ) ) ) ) == 0 ) &&
                    ( *ptrChar != '+' ) && ( *ptrChar != '_' ) )
                {
                    *result = false;
                    break;
                }
            }
        }
    }

    return atStatus;
}

/*-----------------------------------------------------------*/

CellularATError_t Cellular_ATStrStartWith( const char * pString,
                                           const char * pPrefix,
                                           bool * result )
{
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;
    CellularATStringValidationResult_t stringValidationResult = CELLULAR_AT_STRING_UNKNOWN;
    const char * pTempString = pString;
    const char * pTempPrefix = pPrefix;

    if( result == NULL )
    {
        atStatus = CELLULAR_AT_BAD_PARAMETER;
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        atStatus = validateString( pTempString, &stringValidationResult );

        if( atStatus == CELLULAR_AT_SUCCESS )
        {
            if( stringValidationResult != CELLULAR_AT_STRING_VALID )
            {
                atStatus = CELLULAR_AT_BAD_PARAMETER;
            }
        }
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        atStatus = validateString( pTempPrefix, &stringValidationResult );

        if( atStatus == CELLULAR_AT_SUCCESS )
        {
            if( stringValidationResult != CELLULAR_AT_STRING_VALID )
            {
                atStatus = CELLULAR_AT_BAD_PARAMETER;
            }
        }
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        *result = true;

        while( *pTempPrefix != '\0' )
        {
            if( *pTempPrefix != *pTempString )
            {
                *result = false;
                break;
            }

            pTempPrefix++;
            pTempString++;
        }
    }

    return atStatus;
}

/*-----------------------------------------------------------*/

CellularATError_t Cellular_ATRemovePrefix( char ** ppString )
{
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;
    CellularATStringValidationResult_t stringValidationResult = CELLULAR_AT_STRING_UNKNOWN;

    atStatus = validateString( *ppString, &stringValidationResult );

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        if( stringValidationResult != CELLULAR_AT_STRING_VALID )
        {
            atStatus = CELLULAR_AT_BAD_PARAMETER;
        }
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        /* The strchr() function returns a pointer to the first occurrence of
         * the character in the string or NULL if the character is not found.
         *
         * In case of AT response, prefix is always followed by a colon (':'). */
        *ppString = strchr( *ppString, ( int32_t ) ':' );

        if( *ppString == NULL )
        {
            atStatus = CELLULAR_AT_BAD_PARAMETER;
        }
        else
        {
            /* Note that we remove both the prefix and the colon. */
            ( *ppString )++;
        }
    }

    return atStatus;
}

/*-----------------------------------------------------------*/

CellularATError_t Cellular_ATRemoveLeadingWhiteSpaces( char ** ppString )
{
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;
    CellularATStringValidationResult_t stringValidationResult = CELLULAR_AT_STRING_UNKNOWN;

    atStatus = validateString( *ppString, &stringValidationResult );

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        if( stringValidationResult != CELLULAR_AT_STRING_VALID )
        {
            atStatus = CELLULAR_AT_BAD_PARAMETER;
        }
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        /* isspace is a standard library function and we cannot control it. */
        /* coverity[misra_c_2012_directive_4_6_violation] */
        /* coverity[misra_c_2012_rule_10_1_violation] */
        /* coverity[misra_c_2012_rule_10_4_violation] */
        /* coverity[misra_c_2012_rule_18_4_violation] */
        /* coverity[misra_c_2012_rule_21_13_violation] */
        while( ( **ppString != '\0' ) && ( isspace( ( ( int ) ( **ppString ) ) ) != 0 ) )
        {
            ( *ppString )++;
        }
    }

    return atStatus;
}

/*-----------------------------------------------------------*/

CellularATError_t Cellular_ATRemoveTrailingWhiteSpaces( char * pString )
{
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;
    char * p = NULL;
    CellularATStringValidationResult_t stringValidationResult = CELLULAR_AT_STRING_UNKNOWN;

    atStatus = validateString( pString, &stringValidationResult );

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        if( stringValidationResult != CELLULAR_AT_STRING_VALID )
        {
            atStatus = CELLULAR_AT_BAD_PARAMETER;
        }
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        p = &pString[ strlen( pString ) ];

        do
        {
            --p;
            /* isspace is a standard library function and we cannot control it. */
            /* coverity[misra_c_2012_directive_4_6_violation] */
            /* coverity[misra_c_2012_rule_10_1_violation] */
            /* coverity[misra_c_2012_rule_10_4_violation] */
            /* coverity[misra_c_2012_rule_18_4_violation] */
            /* coverity[misra_c_2012_rule_21_13_violation] */
        } while( ( p > pString ) && ( isspace( ( int ) ( *p ) ) != 0 ) );

        p[ 1 ] = '\0';
    }

    return atStatus;
}

/*-----------------------------------------------------------*/

CellularATError_t Cellular_ATRemoveAllWhiteSpaces( char * pString )
{
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;
    CellularATStringValidationResult_t stringValidationResult = CELLULAR_AT_STRING_UNKNOWN;
    char * p = NULL;
    uint16_t ind = 0;
    char * pTempString = pString;

    atStatus = validateString( pTempString, &stringValidationResult );

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        if( stringValidationResult != CELLULAR_AT_STRING_VALID )
        {
            atStatus = CELLULAR_AT_BAD_PARAMETER;
        }
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        p = pTempString;

        while( ( *pTempString ) != '\0' )
        {
            /* isspace is a standard library function and we cannot control it. */
            /* coverity[misra_c_2012_directive_4_6_violation] */
            /* coverity[misra_c_2012_rule_10_1_violation] */
            /* coverity[misra_c_2012_rule_10_4_violation] */
            /* coverity[misra_c_2012_rule_18_4_violation] */
            /* coverity[misra_c_2012_rule_21_13_violation] */
            if( isspace( ( ( int ) ( *pTempString ) ) ) == 0 )
            {
                p[ ind ] = *pTempString;
                ind++;
            }

            pTempString++;
        }

        p[ ind ] = '\0';
    }

    return atStatus;
}

/*-----------------------------------------------------------*/

CellularATError_t Cellular_ATRemoveOutermostDoubleQuote( char ** ppString )
{
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;
    CellularATStringValidationResult_t stringValidationResult = CELLULAR_AT_STRING_UNKNOWN;
    char * p = NULL;

    atStatus = validateString( *ppString, &stringValidationResult );

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        if( stringValidationResult != CELLULAR_AT_STRING_VALID )
        {
            atStatus = CELLULAR_AT_BAD_PARAMETER;
        }
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        if( **ppString == '\"' )
        {
            ( *ppString )++;
        }

        p = *ppString;

        while( *p != '\0' )
        {
            p++;
        }

        if( *--p == '\"' )
        {
            *p = '\0';
        }
    }

    return atStatus;
}

/*-----------------------------------------------------------*/

CellularATError_t Cellular_ATRemoveAllDoubleQuote( char * pString )
{
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;
    CellularATStringValidationResult_t stringValidationResult = CELLULAR_AT_STRING_UNKNOWN;
    char * p = NULL;
    uint16_t ind = 0;
    char * pTempString = pString;

    atStatus = validateString( pTempString, &stringValidationResult );

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        if( stringValidationResult != CELLULAR_AT_STRING_VALID )
        {
            atStatus = CELLULAR_AT_BAD_PARAMETER;
        }
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        p = pTempString;

        while( ( *pTempString ) != '\0' )
        {
            if( *pTempString != '\"' )
            {
                p[ ind ] = *pTempString;
                ind++;
            }

            pTempString++;
        }

        p[ ind ] = '\0';
    }

    return atStatus;
}

/*-----------------------------------------------------------*/

CellularATError_t Cellular_ATGetNextTok( char ** ppString,
                                         char ** ppTokOutput )
{
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;
    const char * pDelimiter = ",";

    atStatus = Cellular_ATGetSpecificNextTok( ppString, pDelimiter, ppTokOutput );

    return atStatus;
}

/*-----------------------------------------------------------*/

CellularATError_t Cellular_ATGetSpecificNextTok( char ** ppString,
                                                 const char * pDelimiter,
                                                 char ** ppTokOutput )
{
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;
    CellularATStringValidationResult_t stringValidationResult = CELLULAR_AT_STRING_UNKNOWN;
    uint16_t tokStrLen = 0, dataStrlen = 0;
    char * tok = NULL;

    atStatus = validateString( *ppString, &stringValidationResult );

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        if( stringValidationResult != CELLULAR_AT_STRING_VALID )
        {
            atStatus = CELLULAR_AT_BAD_PARAMETER;
        }
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        dataStrlen = ( uint16_t ) strlen( *ppString );

        if( ( **ppString ) == ( *pDelimiter ) )
        {
            **ppString = '\0';
            tok = *ppString;
        }
        else
        {
            tok = strtok( *ppString, pDelimiter );
        }

        if( tok == NULL )
        {
            atStatus = CELLULAR_AT_BAD_PARAMETER;
        }
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        tokStrLen = ( uint16_t ) strlen( tok );

        if( ( tokStrLen < dataStrlen ) && ( ( *ppString )[ tokStrLen + 1U ] != '\0' ) )
        {
            *ppString = &tok[ strlen( tok ) + 1U ];
        }
        else
        {
            *ppString = &tok[ strlen( tok ) ];
        }

        *ppTokOutput = tok;
    }

    return atStatus;
}

/*-----------------------------------------------------------*/

static uint8_t _charToNibble( char c )
{
    uint8_t ret = 0xFF;

    if( ( c >= '0' ) && ( c <= '9' ) )
    {
        ret = ( uint8_t ) ( ( uint32_t ) c - ( uint32_t ) '0' );
    }
    else if( ( c >= 'a' ) && ( c <= 'f' ) )
    {
        ret = ( uint8_t ) ( ( uint32_t ) c - ( uint32_t ) 'a' + ( uint32_t ) 10 );
    }
    else
    {
        if( ( c >= 'A' ) && ( c <= 'F' ) )
        {
            ret = ( uint8_t ) ( ( uint32_t ) c - ( uint32_t ) 'A' + ( uint32_t ) 10 );
        }
    }

    return ret;
}

/*-----------------------------------------------------------*/

/* AT core helper API. */
/* coverity[misra_c_2012_rule_8_7_violation] */
CellularATError_t Cellular_ATHexStrToHex( const char * pString,
                                          uint8_t * const * const ppHexData,
                                          uint16_t hexDataLen )
{
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;
    CellularATStringValidationResult_t stringValidationResult = CELLULAR_AT_STRING_UNKNOWN;
    uint16_t strHexLen = 0, i = 0;
    const char * p;
    uint8_t firstNibble = 0, secondNibble = 0;

    atStatus = validateString( pString, &stringValidationResult );

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        if( stringValidationResult != CELLULAR_AT_STRING_VALID )
        {
            atStatus = CELLULAR_AT_BAD_PARAMETER;
        }
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        strHexLen = ( uint16_t ) ( strlen( pString ) / 2U );

        if( strHexLen >= hexDataLen )
        {
            atStatus = CELLULAR_AT_NO_MEMORY;
        }
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        p = pString;

        for( i = 0; i < strHexLen; i++ )
        {
            firstNibble = _charToNibble( *p ) << 4;
            secondNibble = _charToNibble( p[ 1 ] );
            ( *ppHexData )[ i ] = firstNibble | secondNibble;
            p = &p[ 2 ];
        }
    }

    return atStatus;
}

/*-----------------------------------------------------------*/

/* AT core helper API. */
/* coverity[misra_c_2012_rule_8_7_violation] */
CellularATError_t Cellular_ATIsStrDigit( const char * pString,
                                         bool * pResult )
{
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;
    CellularATStringValidationResult_t stringValidationResult = CELLULAR_AT_STRING_UNKNOWN;
    const char * pTempString = pString;

    if( pResult == NULL )
    {
        atStatus = CELLULAR_AT_BAD_PARAMETER;
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        atStatus = validateString( pTempString, &stringValidationResult );

        if( atStatus == CELLULAR_AT_SUCCESS )
        {
            if( stringValidationResult != CELLULAR_AT_STRING_VALID )
            {
                atStatus = CELLULAR_AT_BAD_PARAMETER;
            }
        }
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        *pResult = true;

        while( ( pTempString != NULL ) && ( *pTempString != '\0' ) )
        {
            /* isdigit is a standard library function and we cannot control it. */
            /* coverity[misra_c_2012_directive_4_6_violation] */
            /* coverity[misra_c_2012_rule_10_1_violation] */
            /* coverity[misra_c_2012_rule_10_4_violation] */
            /* coverity[misra_c_2012_rule_18_4_violation] */
            /* coverity[misra_c_2012_rule_21_13_violation] */
            if( isdigit( ( ( int ) ( *pTempString ) ) ) == 0 )
            {
                *pResult = false;
            }

            pTempString++;
        }
    }

    return atStatus;
}

/*-----------------------------------------------------------*/

CellularATError_t Cellular_ATcheckErrorCode( const char * pInputBuf,
                                             const char * const * const ppKeyList,
                                             size_t keyListLen,
                                             bool * result )
{
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;
    uint8_t i = 0;
    CellularATStringValidationResult_t stringValidationResult = CELLULAR_AT_STRING_UNKNOWN;
    bool tmpResult;

    if( result == NULL )
    {
        atStatus = CELLULAR_AT_BAD_PARAMETER;
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        atStatus = validateString( pInputBuf, &stringValidationResult );

        if( atStatus == CELLULAR_AT_SUCCESS )
        {
            if( stringValidationResult != CELLULAR_AT_STRING_VALID )
            {
                atStatus = CELLULAR_AT_BAD_PARAMETER;
            }
        }
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        *result = false;

        for( i = 0; i < keyListLen; i++ )
        {
            if( ( Cellular_ATStrStartWith( pInputBuf, ppKeyList[ i ], &tmpResult ) == CELLULAR_AT_SUCCESS ) && tmpResult )
            {
                *result = true;
                break;
            }
        }
    }

    return atStatus;
}

/*-----------------------------------------------------------*/

CellularATError_t Cellular_ATStrDup( char ** ppDst,
                                     const char * pSrc )
{
    char * p = NULL;
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;
    const char * pTempSrc = pSrc;

    if( ( ppDst == NULL ) || ( pTempSrc == NULL ) || ( strnlen( pTempSrc, CELLULAR_AT_MAX_STRING_SIZE ) >= CELLULAR_AT_MAX_STRING_SIZE ) )
    {
        atStatus = CELLULAR_AT_BAD_PARAMETER;
    }

    if( atStatus == CELLULAR_AT_SUCCESS )
    {
        *ppDst = ( char * ) pvPortMalloc( sizeof( char ) * ( strlen( pTempSrc ) + 1U ) );

        if( *ppDst != NULL )
        {
            p = *ppDst;

            while( *pTempSrc != '\0' )
            {
                *p = *pTempSrc;
                p++;
                pTempSrc++;
            }

            *p = '\0';
        }
    }

    return atStatus;
}

/*-----------------------------------------------------------*/

CellularATError_t Cellular_ATStrtoi( const char * pStr,
                                     int32_t base,
                                     int32_t * pResult )
{
    CellularATError_t atStatus = CELLULAR_AT_SUCCESS;
    int32_t retStrtol = 0;
    char * pEndStr = NULL;

    if( ( pStr == NULL ) || ( pResult == NULL ) )
    {
        atStatus = CELLULAR_AT_BAD_PARAMETER;
    }
    else
    {
        /* MISRA C 2012 Directive 4.7 – If a function returns error information, then
         * that error information shall be tested.
         * MISRA C 2012 Rule 22.8 – The errno variable must be "zero" before calling
         * strtol function.
         * MISRA C 2012 Rule 22.9 – The errno must be tested after strtol function is
         * called.
         *
         * The following line violates MISRA rule 4.7 because return value of strtol()
         * is not checked for error. Also, it violates MISRA rule 22.8 because variable
         * "errno" is neither used nor initialized to zero before strtol() is called.
         * It violates MISRA Rule 22.9 because Variable "errno" is not checked for error
         * after strtol() is called.
         *
         * The above three violations are justified because error checking by "errno"
         * for any POSIX API is not thread safe in FreeRTOS unless "configUSE_POSIX_ERRNO"
         * is enabled. In order to avoid the dependency on this feature, errno variable
         * is not used. The function strtol returns LONG_MIN and LONG_MAX in case of
         * underflow and overflow respectively and sets the errno to ERANGE. It is not
         * possible to distinguish between valid LONG_MIN and LONG_MAX return values
         * and underflow and overflow scenarios without checking errno. Therefore, we
         * cannot check return value of strtol for errors. We ensure that strtol performed
         * a valid conversion by checking that *pEndPtr is '\0'. strtol stores the address
         * of the first invalid character in *pEndPtr and therefore, '\0' value of *pEndPtr
         * means that the complete pToken string passed for conversion was valid and a valid
         * conversion was performed. */
        /* coverity[misra_c_2012_directive_4_7_violation] */
        /* coverity[misra_c_2012_rule_22_8_violation] */
        /* coverity[misra_c_2012_rule_22_9_violation] */
        retStrtol = strtol( pStr, &pEndStr, base );

        if( *pEndStr == '\0' )
        {
            *pResult = retStrtol;
        }
        else
        {
            atStatus = CELLULAR_AT_ERROR;
        }
    }

    return atStatus;
}

/*-----------------------------------------------------------*/
