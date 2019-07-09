/*
 * Amazon FreeRTOS HTTPS Client V1.0.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file iot_https_client.h
 * @brief Implementation of the user-facing functions of the Amazon FreeRTOS HTTPS Client library. 
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Error handling include. */
#include "private/iot_error.h"

/* HTTPS Client library private inclues. */
#include "private/iot_https_internal.h"

/*-----------------------------------------------------------*/

/**
 * @brief The length of the end of the header line.
 * 
 * This is the string length of "\r\n". This defined here for use in initializing local string arrays.
 */
#define HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH  ( 2 )

/**
 * @brief Partial HTTPS request first line.
 * 
 * This is used for the calculation of the requestUserBufferMinimumSize. 
 * The minimum path is "/" because we cannot know how long the application requested path is is going to be. 
 * CONNECT is the longest string length HTTP method according to RFC 2616.
 */
#define HTTPS_PARTIAL_REQUEST_LINE              HTTPS_CONNECT_METHOD " " HTTPS_EMPTY_PATH " " HTTPS_PROTOCOL_VERSION

/**
 * @brief The User-Agent header line string.
 * 
 * This is of the form:
 * "User-Agent: <configured-user-agent>\r\n"
 * This is used for the calculation of the requestUserBufferMinimumSize.
 */
#define HTTPS_USER_AGENT_HEADER_LINE            HTTPS_USER_AGENT_HEADER HTTPS_HEADER_FIELD_SEPARATOR IOT_HTTPS_USER_AGENT HTTPS_END_OF_HEADER_LINES_INDICATOR

/**
 * @brief The Host header line with the field only and not the value.
 * 
 * This is of the form:
 * "Host: \r\n"
 * This is used for the calculation of the requestUserBufferMinimumSize. The Host value is not specified because we
 * cannot anticipate what server the client is making requests to.
 */
#define HTTPS_PARTIAL_HOST_HEADER_LINE          HTTPS_HOST_HEADER HTTPS_HEADER_FIELD_SEPARATOR HTTPS_END_OF_HEADER_LINES_INDICATOR   

/**
 * @brief The maximum Content-Length header line size.
 * 
 * This is the length of header line string: "Content-Length: 4294967296\r\n". 4294967296 is 2^32. This number is chosen
 * because it is the maximum file size that can be represented in a 32 bit system.
 * 
 * This is used to initialize a local array for the final headers to send.
 */
#define HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH    ( 26 )

/*
 * String constants for the Connection header and possible values.
 * 
 * This is used for writing headers automatically in during the sending of the HTTP request.
 * "Connection: keep-alive\r\n" is written automatically for a persistent connection.
 * "Connection: close\r\n" is written automatically for a closed connection.
 */
#define HTTPS_CONNECTION_KEEP_ALIVE_HEADER_LINE HTTPS_CONNECTION_HEADER HTTPS_HEADER_FIELD_SEPARATOR HTTPS_CONNECTION_KEEP_ALIVE_HEADER_VALUE HTTPS_END_OF_HEADER_LINES_INDICATOR
#define HTTPS_CONNECTION_CLOSE_HEADER_LINE      HTTPS_CONNECTION_HEADER HTTPS_HEADER_FIELD_SEPARATOR HTTPS_CONNECTION_CLOSE_HEADER_VALUE HTTPS_END_OF_HEADER_LINES_INDICATOR

/**
 * @brief The length of the "Connection: keep-alive\r\n" header. 
 * 
 * This is used for sizing a local buffer for the final headers to send that include the "Connection: keep-alive\r\n" 
 * header line. 
 * 
 * This is used to initialize a local array for the final headers to send.
 */
#define HTTPS_CONNECTION_KEEP_ALIVE_HEADER_LINE_LENGTH      ( 24 )

/*-----------------------------------------------------------*/

/**
 * @brief Minimum size of the request user buffer.
 * 
 * The user buffer is configured in IotHttpsClientRequestInfo_t.reqUserBuffer. This buffer stores the internal context
 * of the request then the request headers right after. The minimum size for the buffer is the total size of the 
 * internal request context, the HTTP formatted request line, the User-Agent header line, and the part of the Host 
 * header line.
 */
const uint32_t requestUserBufferMinimumSize = sizeof(_httpsRequest_t) + 
                sizeof(HTTPS_PARTIAL_REQUEST_LINE) +
                sizeof(HTTPS_USER_AGENT_HEADER_LINE) +
                sizeof(HTTPS_PARTIAL_HOST_HEADER_LINE);

/**
 * @brief Minimum size of the response user buffer.
 * 
 * The user buffer is configured in IotHttpsClientRequestInfo_t.respUserBuffer. This buffer stores the internal context
 * of the response and then the response headers right after. This minimum size is calculated for if there are no bytes
 * from the HTTP response headers stored. 
 */
const uint32_t responseUserBufferMinimumSize = sizeof(_httpsResponse_t);

/**
 * @brief Minimum size of the connection user buffer.
 * 
 * The user buffer is configured in IotHttpsConnectionInfo_t.userBuffer. This buffer stores the internal context of the 
 * connection.
 */
const uint32_t connectionUserBufferMinimumSize = sizeof(_httpsConnection_t);

/*-----------------------------------------------------------*/

/**
 * @brief Callback for http-parser to indicate the start of the HTTP response message is reached.
 * 
 * @param[in] pHttpParser - http-parser state structure.
 * 
 * @return 0 to tell http-parser to keep parsing.
 *         1 to tell http-parser that parsing should stop return from http_parser_execute with error HPE_CB_message_begin.
 */
static int _httpParserOnMessageBeginCallback(http_parser * pHttpParser);

/**
 * @brief Callback for http-parser to indicate it found the HTTP response status code.
 * 
 * See https://github.com/nodejs/http-parser for more information.
 * 
 * @param[in] pHttpParser - http-parser state structure.
 * @param[in] pLoc - Pointer to the HTTP response status code string in the response message buffer.
 * @param[in] length - The length of the HTTP response status code string.
 * 
 * @return 0 to tell http-parser to keep parsing.
 *         1 to tell http-parser that parsing should stop return from http_parser_execute with error HPE_CB_status.
 */
static int _httpParserOnStatusCallback(http_parser * pHttpParser, const char * pLoc, size_t length);

/**
 * @brief Callback for http-parser to indicate it found an HTTP response header field.
 * 
 * If only part of the header field was returned here in this callback, then this callback will be invoked again the
 * next time the parser executes on the next part of the header field.
 * 
 * See https://github.com/nodejs/http-parser for more information.
 * 
 * @param[in] pHttpParser - http-parser state structure.
 * @param[in] pLoc - Pointer to the header field string in the response message buffer.
 * @param[in] length - The length of the header field.
 * 
 * @return 0 to tell http-parser to keep parsing.
 *         1 to tell http-parser that parsing should stop return from http_parser_execute with error HPE_CB_header_field.
 */
static int _httpParserOnHeaderFieldCallback(http_parser * pHttpParser, const char * pLoc, size_t length);

/**
 * @brief Callback for http-parser to indicate it found an HTTP response header value.
 * 
 * This value corresponds to the field that was found in the _httpParserOnHeaderFieldCallback() called immediately 
 * before this callback was called.
 * 
 * If only part of the header value was returned here in this callback, then this callback will be invoked again the
 * next time the parser executes on the next part of the header value.
 * 
 * See https://github.com/nodejs/http-parser for more information.
 * 
 * @param[in] pHttpParser - http-parser state structure.
 * @param[in] pLoc - Pointer to the header value string in the response message buffer.
 * @param[in] length - The length of the header value.
 * 
 * @return 0 to tell http-parser to keep parsing.
 *         1 to tell http-parser that parsing should stop return from http_parser_execute with error HPE_CB_header_value.
 */
static int _httpParserOnHeaderValueCallback(http_parser * pHttpParser, const char * pLoc, size_t length);

/**
 * @brief Callback for http-parser to indicate it reached the end of the headers in the HTTP response messsage.
 * 
 * The end of the headers is signalled in a HTTP response message by another "\r\n" after the final header line.
 * 
 * See https://github.com/nodejs/http-parser for more information.
 * 
 * @param[in] pHttpParser - http-parser state structure.
 * 
 * @return 0 to tell http-parser to keep parsing.
 *         1 to tell http-parser that parsing should stop return from http_parser_execute with error HPE_CB_headers_complete.
 */
static int _httpParserOnHeadersCompleteCallback(http_parser * pHttpParser);

/**
 * @brief Callback for http-parser to indicate it found HTTP response body.
 * 
 * This callback will be invoked multiple times if there response body is of "Transfer-Encoding: chunked". 
 * _httpParserOnChunkHeaderCallback() will be invoked first, then _httpParserOnBodyCallback(), then 
 * _httpParserOnChunkCompleteCallback(), then repeated back to _httpParserOnChunkHeaderCallback() if there are more
 * "chunks".
 * 
 * See https://github.com/nodejs/http-parser for more information.
 * 
 * @param[in] pHttpParser - http-parser state structure.
 * @param[in] pLoc - Pointer to the body string in the response message buffer.
 * @param[in] length - The length of the body found.
 * 
 * @return 0 to tell http-parser to keep parsing.
 *         1 to tell http-parser that parsing should stop return from http_parser_execute with error HPE_CB_body.
 */
static int _httpParserOnBodyCallback(http_parser * pHttpParser, const char * pLoc, size_t length);

/**
 * @brief Callback for http-parser to indicate it reached the end of the HTTP response messsage.
 * 
 * The end of the message is signalled in a HTTP response message by another "\r\n" after the final header line, with no
 * entity body; or it is singalled by "\r\n" at the end of the entity body.
 * 
 * For a Transfer-Encoding: chunked type of response message, the end of the message is signalled by a terminating 
 * chunk header with length zero.
 * 
 * See https://github.com/nodejs/http-parser for more information.
 * 
 * @param[in] pHttpParser - http-parser state structure.
 * 
 * @return 0 to tell http-parser to keep parsing.
 *         1 to tell http-parser that parsing should stop return from http_parser_execute with error HPE_CB_message_complete.
 */
static int _httpParserOnMessageCompleteCallback(http_parser * pHttpParser);

/* To save code space we do not compile in this code that just prints debugging. */
#if (LIBRARY_LOG_LEVEL == IOT_LOG_DEBUG)
/**
 * @brief Callback for http-parser to indicate it found an HTTP Transfer-Encoding: chunked header.
 * 
 * Transfer-Encoding: chunked headers are embedded in the HTTP response entity body by a "\r\n" followed by the size of
 * the chunk followed by another "\r\n".
 * 
 * If only part of the header field was returned here in this callback, then this callback will be invoked again the
 * next time the parser executes on the next part of the header field.
 * 
 * See https://github.com/nodejs/http-parser for more information.
 * 
 * @param[in] pHttpParser - http-parser state structure.
 * 
 * @return 0 to tell http-parser to keep parsing.
 *         1 to tell http-parser that parsing should stop return from http_parser_execute with error HPE_CB_chunk_header.
 */
static int _httpParserOnChunkHeaderCallback(http_parser * pHttpParser);

/**
 * @brief Callback for http-parser to indicate it reached the end of an HTTP response messsage "chunk".
 * 
 * A chunk is complete when the chunk header size is read fully in the body.
 * 
 * See https://github.com/nodejs/http-parser for more information.
 * 
 * @param[in] pHttpParser - http-parser state structure.
 * 
 * @return 0 to tell http-parser to keep parsing.
 *         1 to tell http-parser that parsing should stop return from http_parser_execute with error HPE_CB_chunk_complete.
 */
static int _httpParserOnChunkCompleteCallback(http_parser * pHttpParser);
#endif

/**
 * @brief Network receive callback for the HTTPS Client library.
 * 
 * This function is called by the network abstraction whenever data is available for the HTTP library.
 * 
 * @param[in] pNetworkConnection - The network connection with the HTTPS connection, pass by the network stack.
 * @param[in] pReceiveContext - A pointer to the HTTPS Client connection handle for which the packet was received.
 */
static void _networkReceiveCallback( void* pNetworkConnection, void* pReceiveContext );

/**
 * @brief Connects to HTTPS server and initializes the connection context.
 * 
 * @param[out] pConnHandle - Handle returned representing the open connection.
 * @param[in] pConnInfo - The connection configuration.
 * 
 * @return #IOT_HTTPS_OK if the connection was successful and so was initializing the context.
 *         #IOT_HTTPS_CONNECTION_ERROR if the connection failed.
 *         #IOT_HTTPS_INTERNAL_ERROR if the context failed to initialize.
 */
static IotHttpsReturnCode_t _connectHttpsServer(IotHttpsConnectionHandle_t * pConnHandle, IotHttpsConnectionInfo_t *pConnInfo);

/**
 * @brief Disconnects from the network.
 * 
 * @param[in] _httpsConnection - HTTPS connection handle.
 */
static void _networkDisconnect(_httpsConnection_t* _httpsConnection);

/**
 * @brief Destroys the network connection.
 * 
 * @param[in] _httpsConnection - HTTPS connection handle.
 */
static void _networkDestroy(_httpsConnection_t* _httpsConnection);

/**
 * @brief Add a header to the current HTTP request.
 * 
 * The headers are stored in reqHandle->pHeaders.
 * 
 * @param[in] reqHandle - HTTP request context.
 * @param[in] pName - The name of the header to add. This is a NULL terminated string.
 * @param[in] pValue - The buffer to value string.
 * @param[in] valueLen - The length of the header value string.
 * 
 * @return #IOT_HTTPS_OK if the header was added to the request successfully.
 *         #IOT_HTTPS_INSUFFICIENT_MEMORY if there was not room in the IotHttpsRequestHandle_t->pHeaders.
 */
static IotHttpsReturnCode_t _addHeader(IotHttpsRequestHandle_t reqHandle, const char * pName, const char * pValue, uint32_t valueLen );

/**
 * @brief Send data on the network.
 * 
 * @param[in] _httpsConnection - HTTP connection context.
 * @param[in] pBuf - Buffer of data to send.
 * @param[in] len - The length of the data to send.
 * 
 * @return #IOT_HTTPS_OK if the data sent successfully.
 *         #IOT_HTTPS_NETWORK_ERROR if there was an error sending the data on the network.
 */
static IotHttpsReturnCode_t _networkSend(_httpsConnection_t* _httpsConnection, uint8_t * pBuf, size_t len);

/**
 * @brief Receive data on the network.
 * 
 * @param[in] _httpsConnection - HTTP connection context.
 * @param[in] pBuf - Buffer of data to receive into.
 * @param[in] len - The length of the data to receive.
 * 
 * @return #IOT_HTTPS_OK if the data was received successfully.
 *         #IOT_HTTPS_NETWORK_ERROR if there was an error receiving the data on the network.
 *         #IOT_HTTPS_TIMEOUT_ERROR if we timedout trying to receive data from the network.
 */
static IotHttpsReturnCode_t _networkRecv( _httpsConnection_t* _httpsConnection, uint8_t * pBuf, size_t bufLen);

/**
 * @brief Send all of the HTTP request headers in the pHeadersBuf and the final Content-Length and Connection headers.
 * 
 * All of the headers in headerbuf are sent first followed by the computed content length and persistent connection 
 * indication.
 * 
 * @param[in] _httpsConnection - HTTP connection context.
 * @param[in] pHeadersBuf - Buffer of the request headers to send. This buffer must contain HTTP headers lines without the 
 *      indicator for the the end of the HTTP headers.
 * @param[in] headersLength - The length of the request headers to send.
 * @param[in] isNonPersistent - Indicator of whether the connection is persistent or not.
 * @param[in] contentLength - The length of the request body used for automatically creating a "Content-Length" header.
 * 
 * @return #IOT_HTTPS_OK if the headers were fully sent successfully.
 *         #IOT_HTTPS_NETWORK_ERROR if there was an error receiving the data on the network.
 *         #IOT_HTTPS_TIMEOUT_ERROR if we timedout trying to receive data from the network.
 */
static IotHttpsReturnCode_t _sendHttpsHeaders( _httpsConnection_t* _httpsConnection, uint8_t* pHeadersBuf, uint32_t headersLength, bool isNonPersistent, uint32_t contentLength);

/**
 * @brief Send all of the HTTP request body in pBodyBuf.
 * 
 * @param[in] _httpsConnection - HTTP connection context.
 * @param[in] pBodyBuf - Buffer of the request body to send.
 * @param[in] bodyLength - The length of the body to send.
 * 
 * @return #IOT_HTTPS_OK if the body was fully sent successfully.
 *         #IOT_HTTPS_NETWORK_ERROR if there was an error receiving the data on the network.
 *         #IOT_HTTPS_TIMEOUT_ERROR if we timedout trying to receive data from the network.
 */
static IotHttpsReturnCode_t _sendHttpsBody( _httpsConnection_t* _httpsConnection, uint8_t* pBodyBuf, uint32_t bodyLength);

/**
 * @brief Parse the HTTP response message in pBuf.
 * 
 * @param[in] pHttpParser - Pointer to the instance of the http-parser.
 * @param[in] pBuf - The buffer of data to parse.
 * @param[in] len - The length of data to parse.
 * 
 * @return #IOT_HTTPS_OK if the data was parsed successfully.
 *         #IOT_HTTPS_PARSING_ERROR if there was an error with parsing the data.
 */
static IotHttpsReturnCode_t _parseHttpsMessage(http_parser* pHttpParser, char* pBuf, size_t len);

/**
 * @brief Receive any part of an HTTP response.
 * 
 * This function is used for both receiving the body into the body buffer and receiving the header into the header 
 * buffer.
 * 
 * @param[in] _httpsConnection - HTTP Connection context.
 * @param[in] pParser - Pointer to the instance of the http-parser.
 * @param[in] pCurrentParserState - The current state of what has been parsed in the HTTP response.
 * @param[in] finalParserState - The final state of the parser expected after this function finishes.
 * @param[in] pBuf - Pointer to the buffer to receive the HTTP response into.
 * @param[in] pBufCur - Pointer to the next location to write data into the buffer pBuf. This is double pointer to update the response context buffer pointers.
 * @param[in] pBufEnd - Pointer to the end of the buffer to receive the HTTP response into.
 * @param[out] pNetworkStatus - The network status will be returned here. The network status can be any of the return values from _networkRecv().
 * 
 * @return #IOT_HTTPS_OK if we received the HTTP response message part successfully.
 *         #IOT_HTTPS_PARSING_ERROR if there was an error with parsing the data.
 */
static IotHttpsReturnCode_t _receiveHttpsMessage( _httpsConnection_t* _httpsConnection, 
                                                  http_parser* pParser,
                                                  IotHttpsResponseParserState_t *pCurrentParserState,
                                                  IotHttpsResponseParserState_t finalParserState, 
                                                  uint8_t** pBuf,
                                                  uint8_t** pBufCur,
                                                  uint8_t** pBufEnd,
                                                  IotHttpsReturnCode_t *pNetworkStatus);

/**
 * @brief Receive the HTTP response headers.
 * 
 * If the Content-Length header field is found in the received headers, then #IotHttpsResponseInternal_t.contentLength 
 * will be set and available. 
 *  
 * Receiving the response headers is always the first step in receiving the response, therefore the 
 * _httpsResponse->httpParser will be initialized to a starting state when this function is called.
 * 
 * This function also sets internal states to indicate that the header buffer is being processed now for a new response.
 * 
 * @param[in] _httpsConnection - HTTP connection context.
 * @param[in] _httpsResponse - HTTP response context.
 * @param[out] pNetworkStatus - The network status will be returned here. The network status can be any of the return values from _networkRecv().
 * 
 * @return #IOT_HTTPS_OK if we received the HTTP headers successfully.
 *         #IOT_HTTPS_PARSING_ERROR if there was an error with parsing the header buffer.
 */
static IotHttpsReturnCode_t _receiveHttpsHeaders( _httpsConnection_t* _httpsConnection, _httpsResponse_t* _httpsResponse, IotHttpsReturnCode_t *pNetworkStatus);

/**
 * @brief Receive the HTTP response body.
 * 
 * Sets internal states to indicate that the the body buffer is being processed now for a new response.
 * 
 * @param[in] _httpsConnection - HTTP connection context.
 * @param[in] _httpsResponse - HTTP response context.
 * @param[out] pNetworkStatus - The network status will be returned here. The network status can be any of the return values from _networkRecv().
 * 
 * @return #IOT_HTTPS_OK if we received the HTTP body successfully.
 *         #IOT_HTTPS_PARSING_ERROR if there was an error with parsing the body buffer.
 */
static IotHttpsReturnCode_t _receiveHttpsBody( _httpsConnection_t* _httpsConnection, _httpsResponse_t* _httpsResponse, IotHttpsReturnCode_t *pNetworkStatus);

/**
 * @brief Read the rest of any HTTP response that may be on the network.
 * 
 * This reads the rest of any left over response data that might still be on the network buffers. We do not want this
 * data left over because it will spill into the header and body buffers of next response that we try to receive. 
 * 
 * If we performed a request without a body and the headers received exceeds the size of the 
 * _httpResponse->pHeaders buffer, then we need to flush the network buffer.
 * 
 * If the application configured the body buffer as null in IotHttpsResponseInfo_t.syncInfo.respData and the server 
 * sends body in the response, but it exceeds the size of  _httpResponse->pHeaders buffer, then we need to flush the 
 * network buffer.
 * 
 * If the amount of body received on the network does not fit into a non-null IotHttpsResponseInfo_t.syncInfo.respData, 
 * then we need to flush the network buffer.
 * 
 * If an asynchronous request cancels in the middle of a response process, after already sending the request message, 
 * then we need to flush the network buffer.
 * 
 * @param[in] _httpsConnection - HTTP connection context.
 * @param[in] _httpsResponse - HTTP response context.
 * 
 * @return #IOT_HTTPS_OK if we received successfully flushed the network data.
 *         #IOT_HTTPS_PARSING_ERROR if there was an error with parsing the data.
 *         #IOT_HTTPS_NETWORK_ERROR if there was an error receiving the data on the network.
 *         #IOT_HTTPS_TIMEOUT_ERROR if we timedout trying to receive data from the network.
 */
static IotHttpsReturnCode_t _flushHttpsNetworkData( _httpsConnection_t* _httpsConnection, _httpsResponse_t* _httpsResponse );

/*-----------------------------------------------------------*/

/**
 * @brief Definition of the http-parser settings.
 * 
 * The http_parser_settings holds all of the callbacks invoked by the http-parser.
 */
static http_parser_settings _httpParserSettings = { 0 };

/*-----------------------------------------------------------*/

static int _httpParserOnMessageBeginCallback(http_parser * pHttpParser)
{
    IotLogDebug("Parser: Start of HTTPS Response message.");
    
    _httpsResponse_t *_httpsResponse = (_httpsResponse_t*)(pHttpParser->data);
    /* Set the state of the parser. The headers are at the start of the message always. */
    _httpsResponse->parserState = PARSER_STATE_IN_HEADERS;
    return 0;
}

/*-----------------------------------------------------------*/

static int _httpParserOnStatusCallback(http_parser * pHttpParser, const char * pLoc, size_t length)
{
    _httpsResponse_t *_httpsResponse = (_httpsResponse_t*)(pHttpParser->data);
    IotLogDebug("Parser: Status %.*s retrieved from HTTPS response.", length, pLoc);

    /* Save the status code so it can be retrieved with IotHttpsClient_ReadResponseStatus(). */
    _httpsResponse->status = (uint16_t)(pHttpParser->status_code);
    /* If we are parsing the network data received in the header buffer then we can increment 
      _httpsResponse->pHeadersCur. The status line in the response is part of the data stored in
      _httpResponse->pHeaders. */
    if(_httpsResponse->bufferProcessingState == PROCESSING_STATE_FILLING_HEADER_BUFFER)
    {
        /* pHeadersCur will never exceed the pHeadersEnd here because PROCESSING_STATE_FILLING_HEADER_BUFFER 
           indicates we are currently in the header buffer and the total size of the header buffer is passed
           into http_parser_execute() as the maximum length to parse. */
        _httpsResponse->pHeadersCur = (uint8_t*)(pLoc += length);
    }
    return 0;
}

/*-----------------------------------------------------------*/

static int _httpParserOnHeaderFieldCallback(http_parser * pHttpParser, const char * pLoc, size_t length)
{
    IotLogDebug("Parser: HTTPS header field parsed %.*s", length, pLoc);

    _httpsResponse_t * _httpsResponse = (_httpsResponse_t *)(pHttpParser->data);
    /* If we are parsing the network data received in the header buffer then we can increment 
      _httpsResponse->pHeadersCur. */
    if(_httpsResponse->bufferProcessingState == PROCESSING_STATE_FILLING_HEADER_BUFFER)
    {
        _httpsResponse->pHeadersCur = (uint8_t*)(pLoc += length);
    }
    /* If the IotHttpsClient_ReadHeader() was called, then we check for the header field of interest. */
    if(_httpsResponse->bufferProcessingState == PROCESSING_STATE_SEARCHING_HEADER_BUFFER)
    {
        if(strncmp(_httpsResponse->pReadHeaderField, pLoc, length) == 0)
        {
            _httpsResponse->foundHeaderField = true;   
        }
    }
    return 0;
}

/*-----------------------------------------------------------*/

static int _httpParserOnHeaderValueCallback(http_parser * pHttpParser, const char * pLoc, size_t length)
{
    int retVal = 0;

    IotLogDebug("Parser: HTTPS header value parsed %.*s", length, pLoc);
    _httpsResponse_t * _httpsResponse = (_httpsResponse_t *)(pHttpParser->data);
    /* If we are parsing the network data received in the header buffer then we can increment 
      _httpsResponse->pHeadersCur. */
    if(_httpsResponse->bufferProcessingState == PROCESSING_STATE_FILLING_HEADER_BUFFER)
    {
        _httpsResponse->pHeadersCur = (uint8_t*)(pLoc += length);
    }

    /* If the IotHttpsClient_ReadHeader() was called, then we check if we found the header field of interest. */
    if(_httpsResponse->bufferProcessingState == PROCESSING_STATE_SEARCHING_HEADER_BUFFER)
    {
        if(_httpsResponse->foundHeaderField)
        {
            _httpsResponse->pReadHeaderValue = ( char* )( pLoc );
            _httpsResponse->readHeaderValueLength = length;
            /* We found a header field so we don't want to keep parsing.*/
            retVal = 1;
        }
    }
    return retVal;
}

/*-----------------------------------------------------------*/

static int _httpParserOnHeadersCompleteCallback(http_parser * pHttpParser)
{
    IotLogDebug("Parser: End of the headers reached in the pHttpParser.");

    int retVal = 0;
    _httpsResponse_t * _httpsResponse = (_httpsResponse_t *)(pHttpParser->data);
    _httpsResponse->parserState = PARSER_STATE_HEADERS_COMPLETE;

    /* If the IotHttpsClient_ReadHeader() was called, we return after finishing looking through all of the headers. 
       Returning a non-zero value exits the http parsing. */
    if(_httpsResponse->bufferProcessingState == PROCESSING_STATE_SEARCHING_HEADER_BUFFER)
    {
        retVal = 1;
    }
    else
    {
        /* When in this callback the pheaderCur pointer is at the first "\r" in the last header line. HTTP/1.1
        headers end with another "\r\n" at the end of the last line. This means we must increment
        the headerCur pointer to the length of "\r\n\r\n". */
        if(_httpsResponse->bufferProcessingState == PROCESSING_STATE_FILLING_HEADER_BUFFER)
        {
            _httpsResponse->pHeadersCur += strlen("\r\n\r\n");
        }

        /* content_length will be zero if no Content-Length header found by the parser. */
        _httpsResponse->contentLength = (uint32_t)(pHttpParser->content_length);
        IotLogDebug("Parser: Content-Length found is %d.", _httpsResponse->contentLength);

        /* For a HEAD method, there is no body expected in the response, so we return 1 to skip body parsing. 
        Also if it was configured in a synchronous response to ignore the HTTPS response body then also stop the body 
        parsing. */
        if((_httpsResponse->method == IOT_HTTPS_METHOD_HEAD) || (_httpsResponse->pBody == NULL))
        {
            retVal = 1;
        }
    }
    return retVal;
}

/*-----------------------------------------------------------*/

static int _httpParserOnBodyCallback(http_parser * pHttpParser, const char * pLoc, size_t length)
{
    IotLogDebug("Parser: Reached the HTTPS message body. It is of length: %d", length);

    _httpsResponse_t * _httpsResponse = (_httpsResponse_t *)(pHttpParser->data);
    _httpsResponse->parserState = PARSER_STATE_IN_BODY;

    /* No matter what buffer is being processed (header or body) we want to copy the data to the body
       buffer if it exists. */
    if((_httpsResponse->pBodyCur != NULL) && (_httpsResponse->pBodyEnd - _httpsResponse->pBodyCur > 0))
    {
        /* Only copy the data if the current location is not the bodyCur. Also only copy if the length does not
           exceed the body buffer. This might happen, only in the synchronous workflow, if the header buffer is larger 
           than the body buffer and receives entity body larger than the body bufffer. */
        if( (_httpsResponse->pBodyCur != (uint8_t*)pLoc) && ( (_httpsResponse->pBodyCur + length) <= _httpsResponse->pBodyEnd ) )
        {
            memcpy(_httpsResponse->pBodyCur, pLoc, length);
        }
        _httpsResponse->pBodyCur += length;
    }
    return 0;
}

/*-----------------------------------------------------------*/

static int _httpParserOnMessageCompleteCallback(http_parser * pHttpParser)
{
    IotLogDebug("Parser: End of the HTTPS message reached.");
    _httpsResponse_t * _httpsResponse = (_httpsResponse_t *)(pHttpParser->data);
    _httpsResponse->parserState = PARSER_STATE_BODY_COMPLETE;

    /* When this callback is reached the end of the HTTP message is indicated. If the length of buffer passed in is 
       larger than the message, then http_parser_execute will return either HPE_CLOSED_CONNECTION or 
       HPE_INVALID_CONSTANT. */
    return 0;
}

/*-----------------------------------------------------------*/

/* To save code space we do not compile in this code that just prints debugging. */
#if (LIBRARY_LOG_LEVEL == IOT_LOG_DEBUG)
static int _httpParserOnChunkHeaderCallback(http_parser * pHttpParser)
{
    ( void )pHttpParser;
    IotLogDebug("Parser: HTTPS message Chunked encoding header callback.");
    IotLogDebug( "Parser: HTTPS message Chunk size: %d", pHttpParser->content_length );
    return 0;
}

/*-----------------------------------------------------------*/

static int _httpParserOnChunkCompleteCallback(http_parser * pHttpParser)
{
    ( void )pHttpParser;
    IotLogDebug("End of a HTTPS message Chunk complete callback.");
    return 0;
}
#endif

/*-----------------------------------------------------------*/

static void _networkReceiveCallback( void* pNetworkConnection, void* pReceiveContext )
{
    _httpsConnection_t* _httpsConnection = ( _httpsConnection_t* )pReceiveContext;
    ( void )pNetworkConnection;

    /* Post that any thread in the connection can read on the socket. */
    IotSemaphore_Post( &( _httpsConnection->rxStartSem ) );
    /* Block while that thread reads from the socket. */
    IotSemaphore_Wait( &( _httpsConnection->rxFinishSem ) );
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_Init( void )
{
    /* This sets all member in the _httpParserSettings to zero. It does not return any errors. */
    http_parser_settings_init( &_httpParserSettings );

    /* Set the http-parser callbacks. */
    _httpParserSettings.on_message_begin = _httpParserOnMessageBeginCallback;
    _httpParserSettings.on_status = _httpParserOnStatusCallback;
    _httpParserSettings.on_header_field = _httpParserOnHeaderFieldCallback;
    _httpParserSettings.on_header_value = _httpParserOnHeaderValueCallback;
    _httpParserSettings.on_headers_complete = _httpParserOnHeadersCompleteCallback;
    _httpParserSettings.on_body = _httpParserOnBodyCallback;
    _httpParserSettings.on_message_complete = _httpParserOnMessageCompleteCallback;
/* To save code space we do not compile in code that just prints debugging. */
#if (LIBRARY_LOG_LEVEL == IOT_LOG_DEBUG)
    _httpParserSettings.on_chunk_header = _httpParserOnChunkHeaderCallback;
    _httpParserSettings.on_chunk_complete = _httpParserOnChunkCompleteCallback;
#endif

    return IOT_HTTPS_OK;
}

/*-----------------------------------------------------------*/

void IotHttpsClient_Deinit( void )
{
    /* The library has not taken any resources that need freeing. This implementation is here for formality.*/
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _connectHttpsServer(IotHttpsConnectionHandle_t * pConnHandle, IotHttpsConnectionInfo_t *pConnInfo)
{
    IOT_FUNCTION_ENTRY(IotHttpsReturnCode_t, IOT_HTTPS_OK);

    IotNetworkError_t networkStatus = IOT_NETWORK_SUCCESS;
    /* The maxmimum string length of the ALPN protocols is configured in IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH. 
       This is +1 for the unfortunate NULL terminator needed by IotNetworkCredentials_t.pAlpnProtos. */
    char pAlpnProtos[IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH + 1] = { 0 };
    /* The maximum string length of the Server host name is configured in IOT_HTTPS_MAX_HOST_NAME_LENGTH. 
       This is +1 for the unfortunate NULL terminator needed by IotNetworkServerInfo_t.pHostName. */
    char pHostName[IOT_HTTPS_MAX_HOST_NAME_LENGTH + 1] = { 0 };
    bool connSemCreated = false;
    bool rxStartSemCreated = false;
    bool rxFinishedSemCreated = false;
    IotNetworkServerInfo_t networkServerInfo = { 0 };
    IotNetworkCredentials_t networkCredentials = { 0 };
    _httpsConnection_t *_httpsConnection = NULL;

    /* Make sure the connection context can fit in the user buffer. */
    if(pConnInfo->userBuffer.bufferLen < connectionUserBufferMinimumSize)
    {
        IotLogError("Buffer size is too small to initialize the connection context. User buffer size: %d, required minimum size; %d.", 
            (*pConnInfo).userBuffer.bufferLen, 
            connectionUserBufferMinimumSize);
        IOT_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INSUFFICIENT_MEMORY);
    }

    /* Set the internal connection context to the start of the user buffer. */
    if(pConnInfo->userBuffer.pBuffer == NULL)
    {
        IotLogError("IotHttpsConnectionInfo_t.userBuffer.pBuffer was NULL.");
        IOT_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INVALID_PARAMETER);
        
    }
    _httpsConnection = (_httpsConnection_t *)(pConnInfo->userBuffer.pBuffer);
    
    /* Set to disconnected initially. */
    _httpsConnection->isConnected = false;

    /* This timeout is used to wait for a response on the connection. */
    if( pConnInfo->timeout == 0 )
    {
        _httpsConnection->timeout = IOT_HTTPS_RESPONSE_WAIT_MS;
    }
    else
    {
        _httpsConnection->timeout = pConnInfo->timeout;
    }

    if( pConnInfo->pNetworkInterface == NULL)
    {
        IotLogError("pNetworkInterface in pConnInfo is NULL.");
        IOT_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INVALID_PARAMETER);
    }
    /* pNetworkInterface contains the connect, disconnect, send, and receive over the network functions. */
    _httpsConnection->pNetworkInterface = pConnInfo->pNetworkInterface;
    
    /* IotNetworkServerInfo_t should take in the length of the host name instead of requiring a NULL terminator. */
    if((pConnInfo->pAddress == NULL) || (pConnInfo->addressLen == 0))
    {
        IotLogError("IotHttpsConnectionInfo_t.pAddress is NULL or not specified.");
        IOT_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INVALID_PARAMETER);
    }
    if(pConnInfo->addressLen > IOT_HTTPS_MAX_HOST_NAME_LENGTH)
    {
        IotLogError("IotHttpsConnectionInfo_t.addressLen has a host name length %d that exceeds maximum length %d.",
            pConnInfo->addressLen,
            IOT_HTTPS_MAX_HOST_NAME_LENGTH);
        IOT_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INVALID_PARAMETER);
    }
    memcpy( pHostName, pConnInfo->pAddress, pConnInfo->addressLen );
    pHostName[pConnInfo->addressLen] = '\0';

    networkServerInfo.pHostName = pHostName; /* This requires a NULL terminated string. */
    networkServerInfo.port = pConnInfo->port;

    /* If this is TLS connection, then set the network credentials. */
    if( ( pConnInfo->flags & IOT_HTTPS_IS_NON_TLS_FLAG ) == 0 )
    {
        if( pConnInfo->flags & IOT_HTTPS_DISABLE_SNI )
        {
            networkCredentials.disableSni = true;
        }
        else
        {
            networkCredentials.disableSni = false;
        }

        
        if( pConnInfo->pAlpnProtocols != NULL )
        {
            /* IotNetworkCredentials_t should take in a length for the alpn protocols string instead of requiring a 
            NULL terminator. */
            if( pConnInfo->alpnProtocolsLen > IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH )
            {
                IotLogError( "IotHttpsConnectionInfo_t.alpnProtocolsLen of %d exceeds the configured maximum protocol length %d. See IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH for more information.",
                    pConnInfo->alpnProtocolsLen,
                    IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH );
                IOT_SET_AND_GOTO_CLEANUP( IOT_HTTPS_INVALID_PARAMETER );
            }
            memcpy( pAlpnProtos, pConnInfo->pAlpnProtocols, pConnInfo->alpnProtocolsLen );
            pAlpnProtos[pConnInfo->alpnProtocolsLen] = '\0';
            networkCredentials.pAlpnProtos = pAlpnProtos; /* This requires a NULL termination. It is inconsistent with other members in the struct. */
        }
        else
        {
            networkCredentials.pAlpnProtos = NULL;
        }

        /* If any of these are NULL a network error will result depending on the connection. */
        networkCredentials.pRootCa = pConnInfo->pCaCert;
        networkCredentials.rootCaSize = pConnInfo->caCertLen;
        networkCredentials.pClientCert = pConnInfo->pClientCert;
        networkCredentials.clientCertSize = pConnInfo->clientCertLen;
        networkCredentials.pPrivateKey = pConnInfo->pPrivateKey;
        networkCredentials.privateKeySize = pConnInfo->privateKeyLen;
    }

    /* If this is a TLS connection connect with networkCredentials. Otherwise pass NULL. */
    if( ( pConnInfo->flags & IOT_HTTPS_IS_NON_TLS_FLAG ) == 0 )
    {
        /* create() will connect to the server specified. */
        networkStatus = _httpsConnection->pNetworkInterface->create( &networkServerInfo,
            &networkCredentials,
            &( _httpsConnection->pNetworkConnection ) );
    }
    else
    {
        networkStatus = _httpsConnection->pNetworkInterface->create( &networkServerInfo,
            NULL,
            &( _httpsConnection->pNetworkConnection ) );
    }

    /* Check to see if the network connection succeeded. If we did not succeed then the connHandle
       will be NULL and we return an error. */
    if( networkStatus != IOT_NETWORK_SUCCESS )
    {
        IotLogError( "Failed to connect to the server at %.*s on port %d with error: %d",
            pConnInfo->addressLen,
            pConnInfo->pAddress,
            pConnInfo->port,
            networkStatus );
        IOT_SET_AND_GOTO_CLEANUP(IOT_HTTPS_CONNECTION_ERROR);
    }
    else
    {
        /* The connection succeeded so this a connected context. */
        _httpsConnection->isConnected = true;
    }

    /* The receive callback tells the task context handling the request/response that the network
       is ready to read from. */
    networkStatus = _httpsConnection->pNetworkInterface->setReceiveCallback( _httpsConnection->pNetworkConnection,
        _networkReceiveCallback,
        _httpsConnection );
    if( networkStatus != IOT_NETWORK_SUCCESS )
    {
        IotLogError( "Failed to connect to set the HTTPS receive callback. " );
        IOT_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INTERNAL_ERROR);
    }

    /* Connection was successful, so create semaphores. */

    /* The overall lock on the connection is set to 1 to start so that the first
       thread to need to use it can use it. */
    connSemCreated = IotSemaphore_Create( &( _httpsConnection->connSem ), 1, 1 );
    if(!connSemCreated)
    {
        IotLogError("Failed to create an internal connection semaphore.");
        IOT_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INTERNAL_ERROR);
    }

    /* The reading start semaphore is initialized to 0 so that the network context can
       set it to 1 for a thread to read from the socket. */
    rxStartSemCreated = IotSemaphore_Create( &( _httpsConnection->rxStartSem ), 0, 1 );
    if(!rxStartSemCreated)
    {
        IotLogError( "Failed to create an internal connection semaphore." );
        IOT_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INTERNAL_ERROR);
    }

    /* The reading finished semaphore is initialized to 0 that the application context
       can set it to 1 for the network context to return from the read ready callback. */
    rxFinishedSemCreated = IotSemaphore_Create( &( _httpsConnection->rxFinishSem ), 0, 1 );
    if(!rxFinishedSemCreated)
    {
        IotLogError( "Failed to create an internal connection semaphore." );
        IOT_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INTERNAL_ERROR);
    }

    /* Return the new connection information. */
    *pConnHandle = _httpsConnection;

    IOT_FUNCTION_CLEANUP_BEGIN();

    /* If we failed anywhere in the connection process, then destroy the semaphores created. */
    if(status != IOT_HTTPS_OK)
    {
        /* If there was a connect was successful, disconnect from the network.  */
        if(( _httpsConnection != NULL) && (_httpsConnection->isConnected))
        {   
            _networkDisconnect( _httpsConnection );
            _networkDestroy( _httpsConnection );
        }

        if(connSemCreated)
        {
            IotSemaphore_Destroy( &(_httpsConnection->connSem) );
        }

        if(rxStartSemCreated)
        {
            IotSemaphore_Destroy( &(_httpsConnection->rxStartSem) );
        }

        if(rxFinishedSemCreated)
        {
            IotSemaphore_Destroy( &(_httpsConnection->rxFinishSem) );
        }

        

        /* Set the connection handle as NULL if everything failed. */
        *pConnHandle = NULL;
    }

    IOT_FUNCTION_CLEANUP_END();
}

/* --------------------------------------------------------- */

IotHttpsReturnCode_t IotHttpsClient_Connect(IotHttpsConnectionHandle_t * pConnHandle, IotHttpsConnectionInfo_t *pConnInfo)
{
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;

    /* Check for NULL parameters in a public API. */
    if((pConnHandle == NULL) || (pConnInfo == NULL))
    {
        IotLogError("Null parameters passed into IotHttpsClient_Connect");
        status = IOT_HTTPS_INVALID_PARAMETER;
    }

    /* If a valid connection handle is passed in. */
    if((status == IOT_HTTPS_OK) && (*pConnHandle != NULL))
    {
        /* If the handle in a connected state, then we want to disconnect before reconnecting. The ONLY way to put the 
           handle is a disconnect state is to call IotHttpsClient_Disconnect(). */
        if((*pConnHandle)->isConnected)
        {
            status = IotHttpsClient_Disconnect(*pConnHandle);
            if(status != IOT_HTTPS_OK)
            {
                IotLogError("Error disconnecting a connected *pConnHandle passed to IotHttpsClient_Connect().Error code %d", status);
            }
        }
    }

    /* Connect to the server now. Initialize all resources needed for the connection context as well here. */
    if(status == IOT_HTTPS_OK)
    {
        status = _connectHttpsServer(pConnHandle, pConnInfo);
        if(status != IOT_HTTPS_OK)
        {
            IotLogError("Error in IotHttpsClient_Connect(). Error code %d.", status);
        }
    }

    return status;
}

/*-----------------------------------------------------------*/

static void _networkDisconnect(_httpsConnection_t* _httpsConnection)
{
    IotNetworkError_t networkStatus = IOT_NETWORK_SUCCESS;
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;

    networkStatus = _httpsConnection->pNetworkInterface->close( _httpsConnection->pNetworkConnection );
    if ( networkStatus != IOT_NETWORK_SUCCESS )
    {
        IotLogWarn("Failed to shutdown the socket with error code: %d", networkStatus );
        status = IOT_HTTPS_NETWORK_ERROR;
    }

    return status;
}

/*-----------------------------------------------------------*/

static void _networkDestroy(_httpsConnection_t* _httpsConnection)
{
    IotNetworkError_t networkStatus = IOT_NETWORK_SUCCESS;
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;

    networkStatus = _httpsConnection->pNetworkInterface->destroy( _httpsConnection->pNetworkConnection );
    if ( networkStatus != IOT_NETWORK_SUCCESS )
    {
        IotLogWarn("Failed to shutdown the socket with error code: %d", networkStatus );
        status = IOT_HTTPS_NETWORK_ERROR;
    }

    return status;
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_Disconnect(IotHttpsConnectionHandle_t connHandle)
{
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;
    IotHttpsReturnCode_t networkStatus = IOT_HTTPS_OK;

    if( connHandle == NULL )
    {
        IotLogError("NULL parameter passed into IotHttpsClient_Disconnect().");
        status = IOT_HTTPS_INVALID_PARAMETER;
    }

    if( status == IOT_HTTPS_OK)
    {
        /* Mark the network as disconnected whether the disconnect passes or not. */
        connHandle->isConnected = false;
        
        /* Disconnect from the network. */
        _networkDisconnect(connHandle);

        if( IotSemaphore_TryWait(&(connHandle->connSem) ) == false)
        {
            IotLogError("Connection is in use, cannot destroy yet. Please wait for the outstanding request to finish.");
            status = IOT_HTTPS_BUSY;
        } 
    }

    if(status == IOT_HTTPS_OK)
    {
        /* Clean connection context resources. */
        IotSemaphore_Destroy( &(connHandle->connSem) );
        IotSemaphore_Destroy( &(connHandle->rxStartSem) );
        IotSemaphore_Destroy( &(connHandle->rxFinishSem) );

        /* Destroy the network connection (cleaning up network socket resources). */
        _networkDestroy(connHandle);
    }

    return status;
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _addHeader(IotHttpsRequestHandle_t reqHandle, const char * pName, const char * pValue, uint32_t valueLen )
{
    int nameLen = strlen( pName ) ;
    int headerFieldSeparatorLen = strlen(HTTPS_HEADER_FIELD_SEPARATOR);
    uint32_t additionalLength = nameLen + headerFieldSeparatorLen + valueLen + HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;
    uint32_t possibleLastHeaderAdditionalLength = HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;

    /* Check if the additional length needed for another header does not exceed the header buffer. */
    if( (additionalLength + possibleLastHeaderAdditionalLength + reqHandle->pHeadersCur) > (reqHandle->pHeadersEnd) )
    {
        IotLogError( "There is %d space left in the header buffer, but we want to add %d more of header.",
            reqHandle->pHeadersEnd - reqHandle->pHeadersCur,
            additionalLength + possibleLastHeaderAdditionalLength );
        status = IOT_HTTPS_INSUFFICIENT_MEMORY;
    }

    if(status == IOT_HTTPS_OK)
    {
        memcpy(reqHandle->pHeadersCur, pName, nameLen);
        reqHandle->pHeadersCur += nameLen;
        memcpy(reqHandle->pHeadersCur, HTTPS_HEADER_FIELD_SEPARATOR, headerFieldSeparatorLen);
        reqHandle->pHeadersCur += headerFieldSeparatorLen;
        memcpy(reqHandle->pHeadersCur, pValue, valueLen);
        reqHandle->pHeadersCur += valueLen;
        memcpy(reqHandle->pHeadersCur, HTTPS_END_OF_HEADER_LINES_INDICATOR, HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH);
        reqHandle->pHeadersCur += HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;
        IotLogDebug("Wrote header: \"%s: %.*s\r\n\". Space left in request user buffer: %d", 
            pName,
            valueLen,
            pValue,
            reqHandle->pHeadersEnd - reqHandle->pHeadersCur);
    }

    return status;
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_InitializeRequest(IotHttpsRequestHandle_t * pReqHandle, IotHttpsRequestInfo_t *pReqInfo)
{
    _httpsRequest_t * _httpsRequest = NULL;
    size_t additionalLength = 0;
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;
    int spaceLen = 1;
    char* pSpace = " ";
    int httpsMethodLen = 0;
    int httpsProtocolVersionLen = strlen(HTTPS_PROTOCOL_VERSION);

    /* Check for NULL parameters in the public API. */
    if( ( pReqHandle == NULL) || ( pReqInfo == NULL) )
    {
        IotLogError("NULL parameter passed into IotHttpsClient_InitializeRequest().");
        status = IOT_HTTPS_INVALID_PARAMETER;
    }

    if( status == IOT_HTTPS_OK)
    {
        /* Check of the user buffer is large enough for the request context + default headers. */
        if(pReqInfo->reqUserBuffer.bufferLen < requestUserBufferMinimumSize)
        {
            IotLogError("Buffer size is too small to initialize the request context. User buffer size: %d, required minimum size; %d.", 
                pReqInfo->reqUserBuffer.bufferLen,
                requestUserBufferMinimumSize);
            status = IOT_HTTPS_INSUFFICIENT_MEMORY;
        }
    }

    if( status == IOT_HTTPS_OK )
    {
        /* Set the request contet to the start of the userbuffer. */
        if(pReqInfo->reqUserBuffer.pBuffer != NULL)
        {
            _httpsRequest = ( _httpsRequest_t *)(pReqInfo->reqUserBuffer.pBuffer);
            /* Clear out the user buffer. */
            memset(pReqInfo->reqUserBuffer.pBuffer, 0, pReqInfo->reqUserBuffer.bufferLen);
        }
        else
        {
            IotLogError("The user buffer pointer IotHttpsRequestInfo_t.reqUserBuffer.pBuffer is NULL.");
            status = IOT_HTTPS_INVALID_PARAMETER;
        }
    }

    if( status == IOT_HTTPS_OK )
    {
        /* Set the start of the headers to the end of the request context in the user buffer. */
        _httpsRequest->pHeaders = (uint8_t*)_httpsRequest + sizeof( _httpsRequest_t );
        _httpsRequest->pHeadersEnd = (uint8_t*)_httpsRequest + pReqInfo->reqUserBuffer.bufferLen;
        _httpsRequest->pHeadersCur = _httpsRequest->pHeaders;

        /* Get the length of the HTTP method. */
        httpsMethodLen = strlen( _pHttpsMethodStrings[pReqInfo->method] );

        /* Add the request line to the header buffer. */
        additionalLength = httpsMethodLen + \
            spaceLen + \
            pReqInfo->pathLen + \
            spaceLen + \
            httpsProtocolVersionLen + \
            HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;
        if( (additionalLength + _httpsRequest->pHeadersCur) > (_httpsRequest->pHeadersEnd ))
        {
            IotLogError("Request line does not fit into the request user buffer: \"%s %.*s HTTP/1.1\\r\\n\" . ",
                _pHttpsMethodStrings[IOT_HTTPS_METHOD_GET],
                pReqInfo->pathLen,
                pReqInfo->pPath);
            IotLogError( "The length needed is %d and the space available is %d.", additionalLength, _httpsRequest->pHeadersEnd - _httpsRequest->pHeadersCur );
            status = IOT_HTTPS_INSUFFICIENT_MEMORY;
        }
    }

    if(status == IOT_HTTPS_OK)
    {
        /* Write "<METHOD> <PATH> HTTP/1.1\r\n" to the start of the header space. */
        memcpy(_httpsRequest->pHeadersCur, _pHttpsMethodStrings[pReqInfo->method], httpsMethodLen);
        _httpsRequest->pHeadersCur += httpsMethodLen;
        memcpy(_httpsRequest->pHeadersCur, pSpace, spaceLen);
        _httpsRequest->pHeadersCur += spaceLen;
        if(pReqInfo->pPath == NULL)
        {
            pReqInfo->pPath = HTTPS_EMPTY_PATH;
            pReqInfo->pathLen = strlen(HTTPS_EMPTY_PATH);
        }
        memcpy(_httpsRequest->pHeadersCur, pReqInfo->pPath, pReqInfo->pathLen);
        _httpsRequest->pHeadersCur += pReqInfo->pathLen;
        memcpy( _httpsRequest->pHeadersCur, pSpace, spaceLen );
        _httpsRequest->pHeadersCur += spaceLen;
        memcpy(_httpsRequest->pHeadersCur, HTTPS_PROTOCOL_VERSION, httpsProtocolVersionLen);
        _httpsRequest->pHeadersCur += httpsProtocolVersionLen;
        memcpy(_httpsRequest->pHeadersCur, HTTPS_END_OF_HEADER_LINES_INDICATOR, HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH);
        _httpsRequest->pHeadersCur += HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;

        /* Add the User-Agent header. */
        status = _addHeader(_httpsRequest, "User-Agent", IOT_HTTPS_USER_AGENT, strlen( IOT_HTTPS_USER_AGENT ));
        if( status != IOT_HTTPS_OK )
        {
            IotLogError("Failed to write header to the request user buffer: \"User-Agent: %s\\r\\n\" . Error code: %d", 
                IOT_HTTPS_USER_AGENT, 
                status);
        }   
    }

    if( status == IOT_HTTPS_OK )
    {
        /* Check for a NULL IotHttpsRequestInfo_t.pHost. */
        if( pReqInfo->pHost == NULL )
        {
            IotLogError( "NULL IotHttpsRequestInfo_t.pHost was passed into IotHttpsClient_InitializeRequest()." );
            status = IOT_HTTPS_INVALID_PARAMETER;
        }
    }

    if( status == IOT_HTTPS_OK)
    {
        status = _addHeader(_httpsRequest, "Host", pReqInfo->pHost, pReqInfo->hostLen);
        if( status != IOT_HTTPS_OK )
        {
            IotLogError("Failed to write \"Host: %.*s\\r\\n\" to the request user buffer. Error code: %d", 
                pReqInfo->hostLen, 
                pReqInfo->pHost, 
                status);
        }
    }

    if( status == IOT_HTTPS_OK )
    {
        if(pReqInfo->pSyncInfo != NULL)
        {
            /* Set the HTTP request entity body. This is allowed to be NULL for no body like for a GET request. */
            _httpsRequest->pBody = pReqInfo->pSyncInfo->pReqData;
            _httpsRequest->bodyLength = pReqInfo->pSyncInfo->reqDataLen;
        }
        else
        {
            IotLogError("IotHttpsRequestInfo_t.pSyncInfo is NULL.");
            status = IOT_HTTPS_INVALID_PARAMETER;
        }
    }

    if( status == IOT_HTTPS_OK )
    {
        /* Save the connection info if the connection is to be made at the time of the request. */
        _httpsRequest->pConnInfo = pReqInfo->pConnInfo;
        /* Set the connection persistence flag for keeping the connection open after receiving a response. */
        _httpsRequest->isNonPersistent = pReqInfo->isNonPersistent;

        /* Initialize the corresponding response to this request. */
        if(pReqInfo->respUserBuffer.bufferLen < responseUserBufferMinimumSize)
        {
            IotLogError("Buffer size is too small to initialize the response context associated with this request. User buffer size: %d, required minimum size; %d.", 
                pReqInfo->respUserBuffer.bufferLen, 
                responseUserBufferMinimumSize);
            status = IOT_HTTPS_INSUFFICIENT_MEMORY;
        }
    }

    if( status == IOT_HTTPS_OK)
    {
        if(pReqInfo->respUserBuffer.pBuffer != NULL)
        {
            _httpsRequest->pRespHandle = (_httpsResponse_t *)(pReqInfo->respUserBuffer.pBuffer);
            /* Clear out the response user buffer. */
            memset(pReqInfo->respUserBuffer.pBuffer, 0, pReqInfo->respUserBuffer.bufferLen);
        }
        else
        {
            IotLogError("IotHttpsRequestInfo_t.respUserBuffer.pBuffer is NULL.");
            status = IOT_HTTPS_INVALID_PARAMETER;
        }
    }
        
    if( status == IOT_HTTPS_OK)
    {
        _httpsRequest->pRespHandle->pHeaders = (uint8_t*)(_httpsRequest->pRespHandle) + sizeof(_httpsResponse_t);
        _httpsRequest->pRespHandle->pHeadersEnd = (uint8_t*)(_httpsRequest->pRespHandle) + pReqInfo->respUserBuffer.bufferLen;
        _httpsRequest->pRespHandle->pHeadersCur = _httpsRequest->pRespHandle->pHeaders;

        /* The request body pointer is allowed to be NULL. pSyncInfo was checked for NULL earlier in this function. */
        _httpsRequest->pRespHandle->pBody = pReqInfo->pSyncInfo->pRespData;
        _httpsRequest->pRespHandle->pBodyCur = _httpsRequest->pRespHandle->pBody;
        _httpsRequest->pRespHandle->pBodyEnd = _httpsRequest->pRespHandle->pBody + pReqInfo->pSyncInfo->respDataLen;

        /* Reinitialize the parser and set the fill buffer state to empty. This does not return any errors. */
        http_parser_init(&(_httpsRequest->pRespHandle->httpParser), HTTP_RESPONSE);

        _httpsRequest->pRespHandle->status = 0;
        _httpsRequest->pRespHandle->method = pReqInfo->method;
        _httpsRequest->pRespHandle->contentLength = 0;
        _httpsRequest->pRespHandle->parserState = PARSER_STATE_NONE;
        _httpsRequest->pRespHandle->bufferProcessingState = PROCESSING_STATE_NONE;
        _httpsRequest->pRespHandle->pReadHeaderField = NULL;
        _httpsRequest->pRespHandle->pReadHeaderValue = NULL;
        _httpsRequest->pRespHandle->readHeaderValueLength = 0;
        _httpsRequest->pRespHandle->foundHeaderField = 0;
        _httpsRequest->pRespHandle->pConnHandle = NULL;
        *pReqHandle = _httpsRequest;
    }

    if((status != IOT_HTTPS_OK) && (pReqHandle != NULL))
    {
        /* Set the request handle to return to NULL, if we failed anywhere. */
        *pReqHandle = NULL;
    }

    return status;
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_AddHeader( IotHttpsRequestHandle_t reqHandle, char * pName, char * pValue, uint32_t len )
{
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;

    /* Check for NULL pointer paramters. */
    if( (pName == NULL) || (pValue == NULL) || (reqHandle == NULL) )
    {
        IotLogError("Null parameter passed into IotHttpsClient_AddHeader().");
        status = IOT_HTTPS_INVALID_PARAMETER;
    }

    if (status == IOT_HTTPS_OK)
    {
        /* Check for auto-generated header "Content-Length". This header is created and send automatically when right before
        request body is sent on the network. */
        if( strncmp(pName, HTTPS_CONTENT_LENGTH_HEADER, strlen(HTTPS_CONTENT_LENGTH_HEADER)) == 0)
        {
            IotLogError("Attempting to add auto-generated header %s. This is not allowed.", HTTPS_CONTENT_LENGTH_HEADER);
            status = IOT_HTTPS_INVALID_PARAMETER;
        }
    }

    if (status == IOT_HTTPS_OK)
    {
        /* Check for auto-generated header "Content-Length". This header is created and send automatically when right before
        request body is sent on the network. */
        if( strncmp(pName, HTTPS_CONNECTION_HEADER, strlen(HTTPS_CONNECTION_HEADER)) == 0)
        {
            IotLogError("Attempting to add auto-generated header %s. This is not allowed.", HTTPS_CONNECTION_HEADER);
            status = IOT_HTTPS_INVALID_PARAMETER;
        }
    }

    if(status == IOT_HTTPS_OK)
    {
        /* Check for auto-generated header "Host". This header is created and placed into the header buffer space
        in IotHttpClient_InitializeRequest(). */
        if( strncmp(pName, HTTPS_HOST_HEADER, strlen(HTTPS_HOST_HEADER)) == 0)
        {
            IotLogError("Attempting to add auto-generated header %s. This is not allowed.", HTTPS_HOST_HEADER);
            status = IOT_HTTPS_INVALID_PARAMETER;
        }
    }

    if(status == IOT_HTTPS_OK)
    {
        /* Check for auto-generated header "User-Agent". This header is created and placed into the header buffer space
        in IotHttpClient_InitializeRequest(). */
        if(strncmp(pName, HTTPS_USER_AGENT_HEADER, strlen(HTTPS_USER_AGENT_HEADER)) == 0)
        {
            IotLogError("Attempting to add auto-generated header %s. This is not allowed.", HTTPS_USER_AGENT_HEADER);
            status = IOT_HTTPS_INVALID_PARAMETER;
        }
    }

    if(status == IOT_HTTPS_OK)
    {
        status = _addHeader(reqHandle, pName, pValue, len );
        if(status != IOT_HTTPS_OK)
        {
            IotLogError("Error in IotHttpsClient_AddHeader(), error code %d.", status);
        }
    }

    return status;
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _networkSend(_httpsConnection_t* _httpsConnection, uint8_t * pBuf, size_t len)
{
    size_t numBytesSent = 0;
    size_t numBytesSentTotal = 0;
    size_t sendLength = len;
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;

    /* Send the headers first because the HTTPS body is in a separate pointer these are not contiguous. */
    while( numBytesSentTotal < sendLength )
    {
        numBytesSent = _httpsConnection->pNetworkInterface->send( _httpsConnection->pNetworkConnection, 
            &( pBuf[numBytesSentTotal] ), 
            sendLength - numBytesSentTotal );

        if( numBytesSent <= 0 )
        {
            IotLogError("Error in sending the HTTPS headers. Error code: %d", numBytesSent);
            break;
        }

        numBytesSentTotal += numBytesSent;
    }

    if( numBytesSentTotal != sendLength )
    {
        IotLogError("Error sending data on the network. We sent %d but there is %d left to send.", numBytesSentTotal, sendLength);
        status = IOT_HTTPS_NETWORK_ERROR;
    }

    return status;
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _networkRecv( _httpsConnection_t* _httpsConnection, uint8_t * pBuf, size_t bufLen)
{
    size_t numBytesRecv = 0;
    size_t numBytesRecvTotal = 0;
    size_t lengthToReceive = bufLen;
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;

    do {
        numBytesRecv = _httpsConnection->pNetworkInterface->receive( _httpsConnection->pNetworkConnection,
            &( pBuf[ numBytesRecvTotal ] ),
            lengthToReceive - numBytesRecvTotal );

        if( numBytesRecv > 0 )
        {
            numBytesRecvTotal += numBytesRecv;
        }
        if( numBytesRecv == 0)
        {
            IotLogError("Timedout waiting for the HTTPS response message.");
            status = IOT_HTTPS_TIMEOUT_ERROR;
            break;
        }
        if( numBytesRecv < 0)
        {
            IotLogError("Error in receiving the HTTPS response message. Socket Error code %d", numBytesRecv);
            status = IOT_HTTPS_NETWORK_ERROR;
            break;
        }
    } while((numBytesRecv > 0)  && ( lengthToReceive - numBytesRecvTotal > 0));

    return status;   
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _sendHttpsHeaders( _httpsConnection_t* _httpsConnection, uint8_t* pHeadersBuf, uint32_t headersLength, bool isNonPersistent, uint32_t contentLength)
{
    const char* connectionHeader = NULL;
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;
    int numWritten = 0;
    int connectionHeaderLen = 0;
    /* The Content-Length header of the form "Content-Length: N\r\n" with a NULL terminator for snprintf. */
    char contentLengthHeaderStr[HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH + 1];
    /* The HTTP headers to send after the headers in pHeadersBuf are the Content-Length and the Connection type and
    the final "\r\n" to indicate the end of the the header lines. */
    char finalHeaders[HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH + HTTPS_CONNECTION_KEEP_ALIVE_HEADER_LINE_LENGTH + HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH] = { 0 };
    
    /* Send the headers passed into this function first. These headers are not termined with a second set of "\r\n". */
    status = _networkSend( _httpsConnection, pHeadersBuf, headersLength);
    if( status != IOT_HTTPS_OK )
    {
        IotLogError("Error sending the HTTPS headers in the request user buffer. Error code: %d", status);
    }

    if(status == IOT_HTTPS_OK)
    {
        /* If there is a Content-Length, then write that to the finalHeaders to send. */
        if (contentLength > 0)
        {
            numWritten = snprintf(contentLengthHeaderStr, 
                sizeof(contentLengthHeaderStr), 
                "%s: %d\r\n",
                HTTPS_CONTENT_LENGTH_HEADER,
                contentLength);
        }
        if( numWritten < 0 )
        {
            IotLogError("Internal error in snprintf() in _sendHttpsHeaders(). Error code %d.", numWritten);
            status = IOT_HTTPS_INTERNAL_ERROR;
        }
    }

    if(status == IOT_HTTPS_OK)
    {
        /* snprintf() succeeded so copy that to the finalHeaders. */
        memcpy(finalHeaders, contentLengthHeaderStr, numWritten);
        /* Write the connection persistence type to the final headers. */
        if (isNonPersistent)
        {
            connectionHeader = HTTPS_CONNECTION_CLOSE_HEADER_LINE;
            connectionHeaderLen = strlen(HTTPS_CONNECTION_CLOSE_HEADER_LINE);
        }
        else
        {
            connectionHeader = HTTPS_CONNECTION_KEEP_ALIVE_HEADER_LINE;
            connectionHeaderLen = strlen(HTTPS_CONNECTION_KEEP_ALIVE_HEADER_LINE);
        }
        memcpy(&finalHeaders[numWritten], connectionHeader, connectionHeaderLen);
        numWritten += connectionHeaderLen;
        memcpy( &finalHeaders[numWritten], HTTPS_END_OF_HEADER_LINES_INDICATOR, HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH );
        numWritten += HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;

        status = _networkSend( _httpsConnection, (uint8_t*)finalHeaders, numWritten);
        if( status != IOT_HTTPS_OK )
        {
            IotLogError("Error sending final HTTPS Headers \r\n%s. Error code: %d", finalHeaders, status);
        }
    }

    return status;

}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _sendHttpsBody( _httpsConnection_t* _httpsConnection, uint8_t* pBodyBuf, uint32_t bodyLength)
{
    IotHttpsReturnCode_t status = _networkSend( _httpsConnection, pBodyBuf, bodyLength);
    if( status != IOT_HTTPS_OK )
    {
        IotLogError("Error sending final HTTPS body at location 0x%x. Error code: %d", pBodyBuf, status);
    }
    return status;
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _parseHttpsMessage(http_parser* pHttpParser, char* pBuf, size_t len)
{
    size_t parsedBytes = 0;
    const char * pHttpParserErrorDescription = NULL;
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;

    parsedBytes = http_parser_execute( pHttpParser, &_httpParserSettings, pBuf, len );
    IotLogDebug( "http-parser parsed %d bytes out of %d specified.", parsedBytes, len );

    /* If the parser fails with HPE_CLOSED_CONNECTION or HPE_INVALID_CONSTANT that simply means there
       was data beyond the end of the message. We do not fail in this case because we often give the whole
       header buffer or body buffer to the parser even if it is only partly filled with data. 
       The error must also not be because if exiting the parser early. Errors <= CB_chunk_complete mean a 
       non-zero number was returned from some callback. I return a nonzero from the callback when I want to stop the 
       parser early like for a HEAD request. */
    if( (pHttpParser->http_errno != 0) &&
        ( HTTP_PARSER_ERRNO( pHttpParser ) != HPE_CLOSED_CONNECTION ) &&
        ( HTTP_PARSER_ERRNO( pHttpParser ) != HPE_INVALID_CONSTANT ) &&
        ( HTTP_PARSER_ERRNO( pHttpParser ) > HPE_CB_chunk_complete) )
    {
        pHttpParserErrorDescription = http_errno_description( HTTP_PARSER_ERRNO( pHttpParser ) );
        IotLogError("http_parser failed on the http response with error: %s", pHttpParserErrorDescription);
        status = IOT_HTTPS_PARSING_ERROR;
    }

    return status;
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _receiveHttpsMessage( _httpsConnection_t* _httpsConnection, 
    http_parser* pParser,
    IotHttpsResponseParserState_t *pCurrentParserState,
    IotHttpsResponseParserState_t finalParserState, 
    uint8_t** pBuf,
    uint8_t** pBufCur,
    uint8_t** pBufEnd,
    IotHttpsReturnCode_t *pNetworkStatus)
{    
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;

    while( (*pCurrentParserState < finalParserState) && (*pBufEnd - *pBufCur > 0) ) 
    {
        *pNetworkStatus = _networkRecv( _httpsConnection,
            *pBufCur, 
            *pBufEnd - *pBufCur);

        status = _parseHttpsMessage(pParser, (char*)(*pBufCur), *pBufEnd - *pBufCur);
        if(status != IOT_HTTPS_OK)
        {
            IotLogError("Failed to parse the message buffer with error: %d", pParser->http_errno);
            break;
        }

        /* The _httResponse->pHeadersCur pointer is updated in the http_parser callbacks. */
        IotLogDebug( "There is %d of space left in the buffer.", *pBufEnd - *pBufCur );

        /* We cannot anticipate the unique network error received when the server closes the connection. 
           We simply exit the loop if there nothing else to receive. We do not return the network status because 
           the error may just be a server closed the connection, but there may still have been HTTP data in the buffer
           that we parsed. */
        if( *pNetworkStatus != IOT_HTTPS_OK )
        {
            IotLogError( "Error receiving the HTTPS response headers. Error code: %d", status );
            break;
        }

    }

    /* If we did not reach the end of the headers or body in the parser callbacks, then the buffer configured does not
       fit all of that part of the HTTP message. */
    if( *pCurrentParserState < finalParserState)
    {
        IotLogDebug("There are still more data on the network. It could not fit into buffer at location 0x%x with length %d.",
            *pBuf,
            *pBufEnd - *pBuf);
    }

    return status;
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _receiveHttpsHeaders( _httpsConnection_t* _httpsConnection, _httpsResponse_t* _httpsResponse, IotHttpsReturnCode_t *pNetworkStatus)
{
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;

    status = _receiveHttpsMessage(_httpsConnection,
        &(_httpsResponse->httpParser),
        &(_httpsResponse->parserState),
        PARSER_STATE_HEADERS_COMPLETE,
        &(_httpsResponse->pHeaders),
        &(_httpsResponse->pHeadersCur),
        &(_httpsResponse->pHeadersEnd),
        pNetworkStatus);
    if( status != IOT_HTTPS_OK)
    {
        IotLogError("Error receiving the HTTP headers. Error code %d", status);
    }

    return status;
}

/*-----------------------------------------------------------*/

/* _receiveHttpsHeaders() must be called first before this function is called. */
static IotHttpsReturnCode_t _receiveHttpsBody( _httpsConnection_t* _httpsConnection, _httpsResponse_t* _httpsResponse, IotHttpsReturnCode_t *pNetworkStatus)
{
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;
    _httpsResponse->bufferProcessingState = PROCESSING_STATE_FILLING_BODY_BUFFER;

    status = _receiveHttpsMessage(_httpsConnection,
        &(_httpsResponse->httpParser),
        &(_httpsResponse->parserState),
        PARSER_STATE_BODY_COMPLETE,
        &(_httpsResponse->pBody),
        &(_httpsResponse->pBodyCur),
        &(_httpsResponse->pBodyEnd),
        pNetworkStatus);
    if( status != IOT_HTTPS_OK)
    {
        IotLogError("Error receiving the HTTP body. Error code %d", status);
    }

    IotLogDebug("The message Content-Length is %d (Will be > 0 for a Content-Length header existing). The remaining content length on the network is %d.",
        _httpsResponse->contentLength,
        _httpsResponse->httpParser.content_length);

    return status;
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _flushHttpsNetworkData( _httpsConnection_t* _httpsConnection, _httpsResponse_t* _httpsResponse )
{
    static uint8_t flushBuffer[IOT_HTTPS_MAX_FLUSH_BUFFER_SIZE] = { 0 };
    const char * pHttpParserErrorDescription = NULL;
    IotHttpsReturnCode_t parserStatus = IOT_HTTPS_OK;
    IotHttpsReturnCode_t networkStatus = IOT_HTTPS_OK;
    IotHttpsReturnCode_t returnStatus = IOT_HTTPS_OK;

    /* Even if there is not body, the parser state will become body complete after the headers finish. */
    while( _httpsResponse->parserState < PARSER_STATE_BODY_COMPLETE )
    {
        IotLogDebug( "Now clearing the rest of the response data on the socket. " );
        networkStatus = _networkRecv( _httpsConnection, flushBuffer, IOT_HTTPS_MAX_FLUSH_BUFFER_SIZE );

        /* Run this through the parser so that we can get the end of the HTTP message, instead of simply timing out the socket to stop.
           If we relied on the socket timeout to stop reading the network socket, then the server may close the connection. */
        parserStatus = _parseHttpsMessage(&(_httpsResponse->httpParser), (char*)flushBuffer, IOT_HTTPS_MAX_FLUSH_BUFFER_SIZE );
        if(parserStatus != IOT_HTTPS_OK)
        {
            pHttpParserErrorDescription = http_errno_description( HTTP_PARSER_ERRNO( &_httpsResponse->httpParser ) );
            IotLogError("Failed to parse the response body buffer with error: %d", _httpsResponse->httpParser.http_errno);
            break;
        }

        /* If there is a network error then we want to stop clearing out the buffer. */
        if( networkStatus != IOT_HTTPS_OK )
        {
            IotLogError( "Error receiving the HTTPS response headers. Error code: %d", networkStatus );
            break;
        }
    }

    /* All network errors except timeout are returned. */
    if( networkStatus != IOT_HTTPS_TIMEOUT_ERROR)
    {
        returnStatus = networkStatus;
    }
    else
    {
        returnStatus = parserStatus;
    }

    /* If there is a timeout error just return the parser status. */
    return returnStatus;
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_SendSync(IotHttpsConnectionHandle_t *pConnHandle, IotHttpsRequestHandle_t reqHandle, IotHttpsResponseHandle_t * pRespHandle)
{

    IOT_FUNCTION_ENTRY(IotHttpsReturnCode_t, IOT_HTTPS_OK);

    _httpsResponse_t * _httpsResponse = NULL;
    _httpsConnection_t * _httpsConnection = NULL;
    IotHttpsReturnCode_t flushStatus = IOT_HTTPS_OK;
    IotHttpsReturnCode_t networkStatus = IOT_HTTPS_OK;

    /* Check for NULL parameters in a public API. */
    if(( pConnHandle == NULL) || (reqHandle == NULL) || (pRespHandle == NULL))
    {
        IotLogError("NULL parameter passed into IotHttpsClient_SendSync()");
        IOT_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INVALID_PARAMETER);
    }

    if(reqHandle->pRespHandle == NULL)
    {
        IotLogError("Null response handle associated with the input reqHandle to IotHttpsClient_SendSync().");
        IOT_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INVALID_PARAMETER);
    }
    /* Set the internal response context to use and the response handle to return. */
    _httpsResponse = reqHandle->pRespHandle;

    /* Set the response handle to return. */
    *pRespHandle = _httpsResponse;
    ( *pRespHandle )->pReqHandle = reqHandle;

    /* If the pConnHandle points to a NULL handle or the pConnHandle is false, then make the connection now. */
    if( (* pConnHandle == NULL) || ((* pConnHandle)->isConnected == false ))
    {
        /* In order to make the connection now the pConnInfo member of IotHttpsRequestHandle_t must not be NULL. */
        if(reqHandle->pConnInfo == NULL)
        {
            IotLogError("IotHttpsRequestInfo_t should have been confogired with pConnInfo not NULL in IotHttpsClient_InitializeRequest() in order to connect implicitly.");
            IOT_GOTO_CLEANUP();
        }
        /* This routine will set the pConnHandle to return if successful. */
        status = _connectHttpsServer(pConnHandle, reqHandle->pConnInfo);
        if(status != IOT_HTTPS_OK)
        {
            IotLogError("An error occurred in connecting with th server with error code: %d", status);
            IOT_GOTO_CLEANUP();
        }
    }
    /* Else pConnHandle is not null and it is connected. */

    /* Reset the http-parser state to an initial state. This is done so that a new response can be parsed from the 
       beginning. */
    _httpsResponse->httpParser.data = (void *)(_httpsResponse);
    _httpsResponse->parserState = PARSER_STATE_NONE;
    _httpsResponse->bufferProcessingState = PROCESSING_STATE_FILLING_HEADER_BUFFER;

    /* Set the internal connection context since we are connected now. */
    _httpsConnection = *pConnHandle;

    /* Lock the entire connection for sending the request and receiving the response. */
    if( IotSemaphore_TimedWait( &(_httpsConnection->connSem), IOT_HTTPS_MAX_CONN_USAGE_WAIT_MS ) == false )
    {
        IotLogError("Timed out in sync send waiting on the connection handle to be free. The current timeout is %d.", IOT_HTTPS_MAX_CONN_USAGE_WAIT_MS);
        status = IOT_HTTPS_BUSY;
        IOT_GOTO_CLEANUP();
    }

    /* Send the headers first. Because the HTTPS body is in a separate pointer. */
    status = _sendHttpsHeaders( _httpsConnection,
        reqHandle->pHeaders,
        reqHandle->pHeadersCur - reqHandle->pHeaders,
        reqHandle->isNonPersistent,
        reqHandle->bodyLength);

    if( status != IOT_HTTPS_OK )
    {
        IotLogError("Error sending the HTTPS headers in the request user buffer. Error code: %d", status);
        IOT_GOTO_CLEANUP();
    }

    /* Send the body now if it exists. */
    if(reqHandle->pBody != NULL)
    {
        status = _sendHttpsBody( _httpsConnection, reqHandle->pBody, reqHandle->bodyLength);
        if( status != IOT_HTTPS_OK )
        {
            IotLogError("Error sending final HTTPS body. Return code: %d", status);
            IOT_GOTO_CLEANUP(); 
        }
    }

    /* Wait for the network to have data to read.*/
    if( IotSemaphore_TimedWait( &( _httpsConnection->rxStartSem ), _httpsConnection->timeout ) == false )
    {
        IotLogError( "Timed out waiting for a response from the network." );
        status = IOT_HTTPS_TIMEOUT_ERROR;
        IOT_GOTO_CLEANUP();
    }

    status = _receiveHttpsHeaders( _httpsConnection, _httpsResponse, &networkStatus);
    if(status != IOT_HTTPS_OK)
    {
        IotLogError("Error in receiving the HTTPS headers into user header buffer at 0x%x. Error code: %d. Network error %d.", _httpsResponse->pHeaders, status, networkStatus);
        IOT_GOTO_CLEANUP(); 
    }

    if( _httpsResponse->parserState < PARSER_STATE_HEADERS_COMPLETE )
    {
        IotLogDebug( "Headers received on the network did not all fit into the configured header buffer. The length of the headers buffer is: %d",
            _httpsResponse->pHeadersEnd - _httpsResponse->pHeaders );
        /* It is not error if the headers did not all fit into the buffer. */
    }

    /* The header buffer is now filled or the end of the headers has been reached already. If part of the response
        body was read from the network into the header buffer, then it was already copied to the body buffer in the 
        _httpParserOnBodyCallback(). */
    if(_httpsResponse->pBody != NULL)
    {
        /* If there is room left in the body buffer, then try to receive more. */
        if( (_httpsResponse->pBodyEnd - _httpsResponse->pBodyCur) > 0 )
        {
            status = _receiveHttpsBody( _httpsConnection,
                _httpsResponse,
                &networkStatus );
            if( status != IOT_HTTPS_OK )
            {
                IotLogError( "Error receiving the HTTPS response body. Error code: %d. Network status %d.", status, networkStatus );
                IOT_GOTO_CLEANUP();
            }
        }

        /* If we don't reach the end of the HTTPS body in the parser, then we only received part of the body.
            The rest of body will be on the socket. */
        if( _httpsResponse->parserState < PARSER_STATE_BODY_COMPLETE )
        {
            IotLogError( "HTTPS response body does not fit into application provided response buffer at location 0x%x with length: %d",
                _httpsResponse->pBody,
                _httpsResponse->pBodyEnd - _httpsResponse->pBody );
            status = IOT_HTTPS_MESSAGE_TOO_LARGE;
            IOT_GOTO_CLEANUP();
        }
    }

    IOT_FUNCTION_CLEANUP_BEGIN();

    /* Flush the socket of the rest of the data if there is data left from this response. We need to do this
        so that for the next request on this connection, there is not left over response from this request in
        the next response buffer.

        If a continuous stream of data is coming in from the connection, with an unknown end, we may not be able to
        flush the network data. It may sit here forever. A continuous stream should be ingested with the async workflow. 
        
        All network errors are ignore here because network read will have read the data from network buffer despite
        errors. */
    if( _httpsConnection != NULL )
    {
        flushStatus = _flushHttpsNetworkData( _httpsConnection, _httpsResponse );
    }
    
    if( flushStatus == IOT_HTTPS_PARSING_ERROR)
    {
        IotLogWarn("There an error parsing the network flush data. The network buffer might not be fully flushed. Error code %d.", flushStatus);
    }
    else
    {
        IotLogDebug("Received network error when flushing the socket. Error code: %d", flushStatus);
    }

    /* If this is not a persistent connection, the server would have closed it after
        sending a response, but we disconnect anyways. */
    if( (_httpsConnection != NULL) && ( reqHandle->isNonPersistent ))
    {
        status = IotHttpsClient_Disconnect( _httpsConnection );
        if( status != IOT_HTTPS_OK )
        {
            IotLogError( "Failed to disconnected from the server with error: %d", status );
            /* The disconnect status is not returned as the server may already be disconnected. */
        }
    }

    /* Set the buffer state to finished for both success or error. This is so that the application can still
        read headers copied even if there was an error. PROCESSING_STATE_FINISHED state is checked in the https parsing 
        callacks to know if we are currently parsing a new response or we are simply parsing to read header values.*/
    _httpsResponse->bufferProcessingState = PROCESSING_STATE_FINISHED;

    if(_httpsConnection != NULL)
    {
        /* Let the network thread know that reading from the socket is finished. Whatever context called
            the network receive callback will return from that callback now. */
        IotSemaphore_Post( &( _httpsConnection->rxFinishSem ) );
        /* Let other threads waiting know the connection is ready to be used. */
        IotSemaphore_Post( &(_httpsConnection->connSem) );
    }
 
    IOT_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_ReadResponseStatus(IotHttpsResponseHandle_t respHandle, uint16_t *pStatus)
{
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;

    if((respHandle == NULL) || (pStatus == NULL))
    {
        IotLogError("NULL parameter passed into IotHttpsClient_ReadResponseStatus().");
        status = IOT_HTTPS_INVALID_PARAMETER;
    }

    if( status == IOT_HTTPS_OK)
    {
        if( respHandle->status == 0)
        {
            IotLogError("The HTTP response status was not found in the HTTP response header buffer.");
            return IOT_HTTPS_NOT_FOUND;
        }
        *pStatus = respHandle->status;
    }

    
    return status;
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_ReadHeader(IotHttpsResponseHandle_t respHandle, char *pName, char *pValue, uint32_t len)
{
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;
    const char * pHttpParserErrorDescription = NULL;
    IotHttpsResponseBufferState_t savedState = respHandle->bufferProcessingState;
    size_t numParsed = 0;

    if((respHandle == NULL) || (pName == NULL) || (pValue == NULL))
    {
        IotLogError("NULL parameter passed into IotHttpsClient_ReadResponseStatus().");
        status = IOT_HTTPS_INVALID_PARAMETER;
    }

    if(status == IOT_HTTPS_OK)
    {
        respHandle->pReadHeaderField = pName;
        respHandle->foundHeaderField = false;
        respHandle->bufferProcessingState = PROCESSING_STATE_SEARCHING_HEADER_BUFFER;

        http_parser_init( &( respHandle->httpParser ), HTTP_RESPONSE );
        numParsed = http_parser_execute(&(respHandle->httpParser), &_httpParserSettings, (char*)(respHandle->pHeaders), respHandle->pHeadersCur - respHandle->pHeaders);
        IotLogDebug("Parsed %d characters in IotHttpsClient_ReadHeader().", numParsed);
        if( (respHandle->httpParser.http_errno != 0) && 
            ( HTTP_PARSER_ERRNO( &(respHandle->httpParser) ) > HPE_CB_chunk_complete) )
        {
            pHttpParserErrorDescription = http_errno_description( HTTP_PARSER_ERRNO( &(respHandle->httpParser) ) );
            IotLogError("http_parser failed on the http response with error: %s", pHttpParserErrorDescription);
            status = IOT_HTTPS_PARSING_ERROR;
        }
    }

    if( status == IOT_HTTPS_OK)
    {
        if(respHandle->foundHeaderField)
        {
            if(respHandle->readHeaderValueLength > len)
            {
                IotLogError("IotHttpsClient_ReadHeader(): The length of the pValue buffer specified is less than the actual length of the pValue. ");
                status = IOT_HTTPS_INSUFFICIENT_MEMORY;
            }
            else
            {
                /* stncpy adds a NULL terminator. */
                strncpy(pValue, respHandle->pReadHeaderValue, respHandle->readHeaderValueLength);
            }
        }
        else
        {
            IotLogError("IotHttpsClient_ReadHeader(): The header field %s was not found.", pName);
            status = IOT_HTTPS_NOT_FOUND;
        }
    }


    /* Always restore the state back to what it was before entering this function. */
    respHandle->bufferProcessingState = savedState;
    return status;
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_ReadContentLength(IotHttpsResponseHandle_t respHandle, uint32_t *pContentLength)
{   
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;

    /* Check for NULL parameters in this public API. */
    if((respHandle == NULL) || (pContentLength == NULL))
    {
        IotLogError("NULL parameter passed into IotHttpsClient_ReadContentLength().");
        status = IOT_HTTPS_INVALID_PARAMETER;
    }

    if( status == IOT_HTTPS_OK)
    {
        /* If there is no content-length header or if we were not able to store it in the header buffer this will be
        invalid. */
        if(respHandle->contentLength <= 0)
        {
            IotLogError("The content length not found in the HTTP response header buffer.");
            *pContentLength = 0;
            status = IOT_HTTPS_NOT_FOUND;
        }
        else
        {
            *pContentLength = respHandle->contentLength;
        }
    }

    return status;
}

/*-----------------------------------------------------------*/
