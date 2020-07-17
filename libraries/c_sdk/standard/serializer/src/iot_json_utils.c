/*
 * FreeRTOS Serializer V1.1.2
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
 * @file iot_json_utils.c
 * @brief Implements the functions in iot_json_utils.h
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* JSON utilities include. */
#include "iot_json_utils.h"

/*-----------------------------------------------------------*/

bool IotJsonUtils_FindJsonValue( const char * pJsonDocument,
                                 size_t jsonDocumentLength,
                                 const char * pJsonKey,
                                 size_t jsonKeyLength,
                                 const char ** pJsonValue,
                                 size_t * pJsonValueLength )
{
    size_t i = 0;
    size_t jsonValueLength = 0;
    char openCharacter = '\0', closeCharacter = '\0';
    int nestingLevel = 0;

    /* Ensure the JSON document is long enough to contain the key/value pair. At
     * the very least, a JSON key/value pair must contain the key and the 6
     * characters {":""} */
    if( jsonDocumentLength < jsonKeyLength + 6 )
    {
        return false;
    }

    /* Search the characters in the JSON document for the key. The end of the JSON
     * document does not have to be searched once too few characters remain to hold a
     * value. */
    while( i < jsonDocumentLength - jsonKeyLength - 3 )
    {
        /* If the first character in the key is found and there's an unescaped double
         * quote after the key length, do a string compare for the key. */
        if( ( pJsonDocument[ i ] == pJsonKey[ 0 ] ) &&
            ( pJsonDocument[ i + jsonKeyLength ] == '\"' ) &&
            ( pJsonDocument[ i + jsonKeyLength - 1 ] != '\\' ) &&
            ( strncmp( pJsonDocument + i,
                       pJsonKey,
                       jsonKeyLength ) == 0 ) )
        {
            /* Key found; this is a potential match. */

            /* Skip the characters in the JSON key and closing double quote. */
            i += jsonKeyLength + 1;

            /* Skip all whitespace characters between the closing " and the : */
            while( pJsonDocument[ i ] == ' ' ||
                   pJsonDocument[ i ] == '\n' ||
                   pJsonDocument[ i ] == '\r' ||
                   pJsonDocument[ i ] == '\t' )
            {
                i++;

                /* If the end of the document is reached, this isn't a match. */
                if( i >= jsonDocumentLength )
                {
                    return false;
                }
            }

            /* The character immediately following a key (and any whitespace) must be a :
             * If it's another character, then this string is a JSON value; skip it. */
            if( pJsonDocument[ i ] != ':' )
            {
                continue;
            }
            else
            {
                /* Skip the : */
                i++;
            }

            /* Skip all whitespace characters between : and the first character in the value. */
            while( pJsonDocument[ i ] == ' ' ||
                   pJsonDocument[ i ] == '\n' ||
                   pJsonDocument[ i ] == '\r' ||
                   pJsonDocument[ i ] == '\t' )
            {
                i++;

                /* If the end of the document is reached, this isn't a match. */
                if( i >= jsonDocumentLength )
                {
                    return false;
                }
            }

            /* Value found. Set the output parameter. */
            if( pJsonValue != NULL )
            {
                *pJsonValue = pJsonDocument + i;
            }

            /* Calculate the value's length. */
            switch( pJsonDocument[ i ] )
            {
                /* Calculate length of a JSON string. */
                case '\"':
                    /* Include the length of the opening and closing double quotes. */
                    jsonValueLength = 2;

                    /* Skip the opening double quote. */
                    i++;

                    /* Add the length of all characters in the JSON string. */
                    while( pJsonDocument[ i ] != '\"' )
                    {
                        /* Ignore escaped double quotes. */
                        if( ( pJsonDocument[ i ] == '\\' ) &&
                            ( i + 1 < jsonDocumentLength ) &&
                            ( pJsonDocument[ i + 1 ] == '\"' ) )
                        {
                            /* Skip the characters \" */
                            i += 2;
                            jsonValueLength += 2;
                        }
                        else
                        {
                            /* Add the length of a single character. */
                            i++;
                            jsonValueLength++;
                        }

                        /* If the end of the document is reached, this isn't a match. */
                        if( i >= jsonDocumentLength )
                        {
                            return false;
                        }
                    }

                    break;

                /* Set the matching opening and closing characters of a JSON object or array.
                 * The length calculation is performed below. */
                case '{':
                    openCharacter = '{';
                    closeCharacter = '}';
                    break;

                case '[':
                    openCharacter = '[';
                    closeCharacter = ']';
                    break;

                /* Calculate the length of a JSON primitive. */
                default:

                    /* Skip the characters in the JSON value. The JSON value ends with a , or } */
                    while( pJsonDocument[ i ] != ',' &&
                           pJsonDocument[ i ] != '}' )
                    {
                        /* Any whitespace before a , or } means the JSON document is invalid. */
                        if( ( pJsonDocument[ i ] == ' ' ) ||
                            ( pJsonDocument[ i ] == '\n' ) ||
                            ( pJsonDocument[ i ] == '\r' ) ||
                            ( pJsonDocument[ i ] == '\t' ) )
                        {
                            return false;
                        }

                        i++;
                        jsonValueLength++;

                        /* If the end of the document is reached, this isn't a match. */
                        if( i >= jsonDocumentLength )
                        {
                            return false;
                        }
                    }

                    break;
            }

            /* Calculate the length of a JSON object or array. */
            if( ( openCharacter != '\0' ) && ( closeCharacter != '\0' ) )
            {
                /* Include the length of the opening and closing characters. */
                jsonValueLength = 2;

                /* Skip the opening character. */
                i++;

                /* Add the length of all characters in the JSON object or array. This
                 * includes the length of nested objects. */
                while( pJsonDocument[ i ] != closeCharacter ||
                       ( pJsonDocument[ i ] == closeCharacter && nestingLevel != 0 ) )
                {
                    /* An opening character starts a nested object. */
                    if( pJsonDocument[ i ] == openCharacter )
                    {
                        nestingLevel++;
                    }
                    /* A closing character ends a nested object. */
                    else if( pJsonDocument[ i ] == closeCharacter )
                    {
                        nestingLevel--;
                    }

                    i++;
                    jsonValueLength++;

                    /* If the end of the document is reached, this isn't a match. */
                    if( i >= jsonDocumentLength )
                    {
                        return false;
                    }
                }
            }

            /* JSON value length calculated; set the output parameter. */
            if( pJsonValueLength != NULL )
            {
                *pJsonValueLength = jsonValueLength;
            }

            return true;
        }

        i++;
    }

    return false;
}

/*-----------------------------------------------------------*/
