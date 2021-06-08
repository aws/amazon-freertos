/*
 * FreeRTOS HTTPS Client V1.2.0
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
 * @file iot_tests_https_common.c
 * @brief Common definitions for the HTTPS Client unit tests.
 */
#include "iot_tests_https_common.h"

/**
 * The generic response status line and header line information for generating a test HTTP response message header.
 */
#define HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE           "HTTP/1.1 200 OK\r\n"                                     /**< @brief A response status line for generating a test response message. */
#define HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE_LENGTH    ( sizeof( HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE ) - 1 ) /**< @brief The length of the test response status line. */
#define HTTPS_TEST_GENERIC_HEADER                         "header"                                                  /**< @brief A generic header name for generating a test response message. */
#define HTTPS_TEST_GENERIC_VALUE                          "value"                                                   /**< @brief A generic header value for generating a test response message. */

/**
 * The maximum and minimum digits in a uint32_t.
 *
 * This is used to size an array holding some generic headers when generating a test HTTP response message.
 */
#define MAX_UINT32_DIGITS                                 ( 10 ) /**< @brief The maximum digits in a uin32_t; 2^32 = 4294967296 which is 10 digits. */
#define MIN_UINT32_DIGITS                                 ( 1 )  /**< @brief The minimum digits in a uin32_t. */

/**
 * The maximum and minimum line lengths of a generic HTTP response header of the form "headerN: valueN\r\n".
 *
 * These are needed for sizing arrays holding some intermediate string processing information when generating a test \
 * HTTP response message.
 */
#define MAX_GENERIC_HEADER_LINE_LENGTH       \
    sizeof( HTTPS_TEST_GENERIC_HEADER ) - 1  \
    + HTTPS_HEADER_FIELD_SEPARATOR_LENGTH    \
    + MAX_UINT32_DIGITS                      \
    + sizeof( HTTPS_TEST_GENERIC_VALUE ) - 1 \
    + MAX_UINT32_DIGITS                      \
    + HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH /**< @brief The maximum length of a header line used for generating a test HTTP response message. */
#define MIN_GENERIC_HEADER_LINE_LENGTH       \
    sizeof( HTTPS_TEST_GENERIC_HEADER ) - 1  \
    + HTTPS_HEADER_FIELD_SEPARATOR_LENGTH    \
    + MIN_UINT32_DIGITS                      \
    + sizeof( HTTPS_TEST_GENERIC_VALUE ) - 1 \
    + MIN_UINT32_DIGITS                      \
    + HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH /**< @brief The minimum length of a header line used for generating a test HTTP response message. */

/**
 * Starting character and maximum letters to increment to generate a test HTTP response message body.
 */
#define HTTPS_TEST_GENERIC_BODY_STARTING_CHAR    'a'            /**< @brief The starting character in the body for generating a test HTTP response. */
#define NUM_LETTERS_IN_ALPHABET                  ( ( int ) 26 ) /**< @brief The number of letters in the alphabet is used to restart the HTTP response body generation at 'a' again after 'z'. */

/*-----------------------------------------------------------*/

/**
 * @brief HTTPS Client connection user buffer to share among the tests.
 *
 * This variable is extern to save memory. This is acceptable as the HTTPS Client unit tests run sequentially.
 * The user buffers are always overwritten each utilizing test, so data left over affecting other tests is not a
 * concern.
 */
uint8_t _pConnUserBuffer[ HTTPS_TEST_CONN_USER_BUFFER_SIZE ] = { 0 };

/**
 * @brief HTTPS Request user buffer to share among the tests.
 *
 * This variable is extern to save memory. This is acceptable as the HTTPS Client unit tests run sequentially.
 * The user buffers are always overwritten each utilizing test, so data left over affecting other tests is not a
 * concern.
 */
uint8_t _pReqUserBuffer[ HTTPS_TEST_REQ_USER_BUFFER_SIZE ] = { 0 };

/**
 * @brief HTTPS Response user buffer to share among the tests.
 *
 * This variable is extern to save memory. This is acceptable as the HTTPS Client unit tests run sequentially.
 * The user buffers are always overwritten each utilizing test, so data left over affecting other tests is not a
 * concern.
 */
uint8_t _pRespUserBuffer[ HTTPS_TEST_RESP_USER_BUFFER_SIZE ] = { 0 };

/**
 * @brief HTTPS Response body buffer to share among the tests.
 *
 * This variable is extern to save memory. This is acceptable as the HTTPS Client unit tests run sequentially.
 * The user buffers are always overwritten each utilizing test, so data left over affecting other tests is not a
 * concern.
 */
uint8_t _pRespBodyBuffer[ HTTPS_TEST_RESP_BODY_BUFFER_SIZE ] = { 0 };

/**
 * @brief An HTTP response message to share among the tests.
 *
 * This variable is extern to save memory. This is acceptable as the HTTPS Client unit tests run sequentially.
 * The user buffers are always overwritten each utilizing test, so data left over affecting other tests is not a
 * concern.
 */
uint8_t _pRespMessageBuffer[ HTTPS_TEST_RESPONSE_MESSAGE_LENGTH ] = { 0 };

/*-----------------------------------------------------------*/

/* See iot_tests_https_common.c for a description of this function. */
void _generateHttpResponseMessage( int headerLength,
                                   int bodyLength )
{
    /* The content length header is needed so that http-parser will give the body length back to the application
     * correctly during it's callback. +1 for the NULL terminator generated in snprintf. the NULL terminator is not
     * included in the HTTP response message. */
    char contentLengthHeaderLine[ HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH + 1 ] = { 0 };
    int index = 0;
    int nextCopyIndex = 0;
    int currentHeaderLineLength = 0;
    int headerSpaceLeft = headerLength - HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH - HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE_LENGTH;
    /* +1 for the NULL terminator for snprintf. This is not included in the final set of headers. */
    char currentHeaderLine[ MAX_GENERIC_HEADER_LINE_LENGTH + 1 ] = { 0 };

    TEST_ASSERT_LESS_THAN_MESSAGE( HTTPS_TEST_RESPONSE_MESSAGE_LENGTH,
                                   headerLength + bodyLength,
                                   "Tried to generate a test HTTP response message into a buffer too small." );

    /* We want the headerLength desired to be at least the size of:
     * "HTTP/1.1 200 OK\r\nContent-Length: <bodyLengthStr>\r\nheader0: value0\r\n\r\n".
     * This is in order to generate meaningful test data. */
    TEST_ASSERT_GREATER_THAN_MESSAGE( HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE_LENGTH + \
                                      HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH +           \
                                      MIN_GENERIC_HEADER_LINE_LENGTH +                 \
                                      HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH,
                                      headerLength,
                                      "Tried to generate a test HTTP response message with specifying too small a desired headerLength." );

    /* Generate the HTTP response status line. This is a required HTTP response message line. */
    memcpy( &( _pRespMessageBuffer[ nextCopyIndex ] ), HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE, HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE_LENGTH );
    nextCopyIndex += HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE_LENGTH;

    /* Generate the Content-Length header line. This is needed so that during testing we reach the
     * onMessageCompleteCallback() during parsing. */
    currentHeaderLineLength = snprintf( contentLengthHeaderLine,
                                        sizeof( contentLengthHeaderLine ),
                                        "%s%s%d%s",
                                        HTTPS_CONTENT_LENGTH_HEADER,
                                        HTTPS_HEADER_FIELD_SEPARATOR,
                                        bodyLength,
                                        HTTPS_END_OF_HEADER_LINES_INDICATOR );
    memcpy( &( _pRespMessageBuffer[ nextCopyIndex ] ), contentLengthHeaderLine, currentHeaderLineLength );
    nextCopyIndex += currentHeaderLineLength;
    headerSpaceLeft -= currentHeaderLineLength;

    /* Generate the header lines */
    while( headerSpaceLeft > 0 )
    {
        /* currentHeaderLineLength will equal the amount of characters written without the NULL terminator. */
        currentHeaderLineLength = snprintf( currentHeaderLine,
                                            sizeof( currentHeaderLine ),
                                            "%s%d%s%s%d%s",
                                            HTTPS_TEST_GENERIC_HEADER,
                                            index,
                                            HTTPS_HEADER_FIELD_SEPARATOR,
                                            HTTPS_TEST_GENERIC_VALUE,
                                            index,
                                            HTTPS_END_OF_HEADER_LINES_INDICATOR );

        /* We need to check if the current header line will fit into the header space left.
         * Given that the input headerLength desired MUST be less than the length of
         * "HTTP/1.1 200 OK\r\nContent-Length: <bodyLengthStr>\r\nheader0: value0\r\n\r\n", we
         * are guaranteed to have written "HTTP/1.1 200 OK\r\nheader0: value0\r\n" before we reach this if case. */
        if( headerSpaceLeft < currentHeaderLineLength )
        {
            /* We want to overwite the last header value with filler data to reach our desired headerLength. We do this
             * by writing data at the end of the last header value. Which means we need to go back before the last
             * "\r\n" written. */
            nextCopyIndex -= HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;
            memset( &( _pRespMessageBuffer[ nextCopyIndex ] ), '0', headerSpaceLeft );
            nextCopyIndex += headerSpaceLeft;
            memcpy( &( _pRespMessageBuffer[ nextCopyIndex ] ),
                    HTTPS_END_OF_HEADER_LINES_INDICATOR,
                    HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH );
            nextCopyIndex += HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;
            headerSpaceLeft = 0;
        }
        else
        {
            memcpy( &( _pRespMessageBuffer[ nextCopyIndex ] ), currentHeaderLine, currentHeaderLineLength );
            index++;
            nextCopyIndex += currentHeaderLineLength;
            headerSpaceLeft -= currentHeaderLineLength;
        }
    }

    /* Add the last header indicator. */
    memcpy( &( _pRespMessageBuffer[ nextCopyIndex ] ),
            HTTPS_END_OF_HEADER_LINES_INDICATOR,
            HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH );
    nextCopyIndex += HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;

    /* Generate the body. Write 'a' through 'z' repeating so that proper copying can be verified. */
    for( index = 0; index < bodyLength; index++ )
    {
        _pRespMessageBuffer[ nextCopyIndex ] = HTTPS_TEST_GENERIC_BODY_STARTING_CHAR + ( index % NUM_LETTERS_IN_ALPHABET ); /* Is modulus too expensive for MCUs? */
        nextCopyIndex++;
    }

    /* Set this to mimic reading from the network. */
    _nextRespMessageBufferByteToReceive = 0;
}

/*-----------------------------------------------------------*/

/**
 * @brief Test verify the response body in pBody up to bodyLength.
 *
 * This will verify that 'a' through 'z' (then repeated) are written properly.
 */
void _verifyHttpResponseBody( int bodyLength,
                              uint8_t * pBody,
                              int startIndex )
{
    int curIndex = 0;
    int endIndex = bodyLength;

    for( curIndex = 0; curIndex < endIndex; curIndex++ )
    {
        TEST_ASSERT_EQUAL( HTTPS_TEST_GENERIC_BODY_STARTING_CHAR + ( ( startIndex + curIndex ) % NUM_LETTERS_IN_ALPHABET ), pBody[ curIndex ] );
    }
}
