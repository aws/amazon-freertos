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
 * @file aws_json_utils.c
 * @brief
 */
#include <string.h>
#include <stdlib.h>
#include "aws_json_utils.h"

#define INT16_MAX_WIDTH         ( 6 )

int16_t JsonUtils_Parse( const char * const pcDoc, uint32_t ulDocLength, jsmntok_t * pxJSMNTokens, uint32_t ulMaxTokens )
{
	jsmn_parser xJSMNParser;
	int16_t sReturn;

	jsmn_init( &xJSMNParser );
	sReturn = ( int16_t ) jsmn_parse( &xJSMNParser,
			pcDoc,
			ulDocLength,
			pxJSMNTokens,
			ulMaxTokens );

	return sReturn;

}

void JsonUtils_GetStrValue( const char * const pcDoc, jsmntok_t * pxJSMNTokens,
		int16_t sNumTokens, const char* pcKey, uint32_t ulKeyLen, const char **ppcValue, uint32_t *pulValLen )
{
	uint16_t usIdx;
	jsmntok_t *pxTk;
	BaseType_t xKeyFound = pdFALSE;

	for( usIdx = 0; usIdx < sNumTokens; usIdx++ )
	{
		pxTk = pxJSMNTokens + usIdx;
		if( pxTk->type == JSMN_STRING &&
				ulKeyLen == ( pxTk->end - pxTk->start ) &&
				strncmp( (pcDoc + pxTk->start), pcKey, ( pxTk->end - pxTk->start ) ) == 0 )
		{
			xKeyFound = pdTRUE;
			break;
		}
	}

	if( xKeyFound && usIdx < sNumTokens-1 )
	{
		pxTk++;
		*ppcValue = (pcDoc + pxTk->start);
		*pulValLen = pxTk->end - pxTk->start;
	}
	else
	{
		*ppcValue  = NULL;
		*pulValLen = 0;
	}

}

BaseType_t JsonUtils_GetInt16Value( const char * const pcDoc, jsmntok_t * pxJSMNTokens,
		int16_t sNumTokens, const char* pcKey, uint32_t ulKeyLen, int16_t *psValue )

{
	const char* pcVal = NULL;
	uint32_t ulValLen;
	char cInt16Val[ INT16_MAX_WIDTH + 1 ];
	BaseType_t xValFound = pdFALSE;

	memset( cInt16Val, 0x00, sizeof(cInt16Val) );

	JsonUtils_GetStrValue( pcDoc, pxJSMNTokens, sNumTokens,
			pcKey,
			ulKeyLen,
			&pcVal,
			&ulValLen );

	if( pcVal != NULL )
	{
		strncpy( cInt16Val, pcVal, ulValLen );
		*psValue = atoi( cInt16Val );
		xValFound = pdTRUE;
	}

	return xValFound;
}
