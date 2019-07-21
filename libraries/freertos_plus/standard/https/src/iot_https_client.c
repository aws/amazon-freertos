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

/* HTTPS Client library private inclues. */
#include "private/iot_https_internal.h"

/*-----------------------------------------------------------*/

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
 * This is used for writing headers automatically during the sending of the HTTP request.
 * "Connection: keep-alive\r\n" is written automatically for a persistent connection.
 * "Connection: close\r\n" is written automatically for a non-presistent connection.
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

/**
 * @brief Indicates for the http-parser parsing execution function to tell it to keep parsing or to stop parsing.
 * 
 * A value of 0 means the parser should keep parsing if there is more unparsed length.
 * A value greater than 0 tells the parser to stop parsing.
 */
#define KEEP_PARSING                            0
#define STOP_PARSING                            1

/*-----------------------------------------------------------*/

/**
 * @brief Minimum size of the request user buffer.
 * 
 * The request user buffer is configured in IotHttpsClientRequestInfo_t.userBuffer. This buffer stores the internal context
 * of the request and then the request headers right after. The minimum size for the buffer is the total size of the 
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
 * The response user buffer is configured in IotHttpsClientRequestInfo_t.userBuffer. This buffer stores the internal context
 * of the response and then the response headers right after. This minimum size is calculated for the case if no bytes
 * from the HTTP response headers are to be stored. 
 */
const uint32_t responseUserBufferMinimumSize = sizeof(_httpsResponse_t);

/**
 * @brief Minimum size of the connection user buffer.
 * 
 * The connection user buffer is configured in IotHttpsConnectionInfo_t.userBuffer. This buffer stores the internal context of the 
 * connection.
 */
const uint32_t connectionUserBufferMinimumSize = sizeof(_httpsConnection_t);

/*-----------------------------------------------------------*/

/**
 * @brief Callback from http-parser to indicate the start of the HTTP response message is reached.
 * 
 * See https://github.com/nodejs/http-parser for more information.
 * 
 * @param[in] pHttpParser - http-parser state structure.
 * 
 * @return 0 to tell http-parser to keep parsing.
 *         1 to tell http-parser that parsing should stop return from http_parser_execute with error HPE_CB_message_begin.
 */
static int _httpParserOnMessageBeginCallback(http_parser * pHttpParser);

/**
 * @brief Callback from http-parser to indicate it found the HTTP response status code.
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
 * @brief Callback from http-parser to indicate it found an HTTP response header field.
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
 * @brief Callback from http-parser to indicate it found an HTTP response header value.
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
 * @brief Callback from http-parser to indicate it reached the end of the headers in the HTTP response messsage.
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
 * @brief Callback from http-parser to indicate it found HTTP response body.
 * 
 * This callback will be invoked multiple times if the response body is of "Transfer-Encoding: chunked". 
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
 * @brief Callback from http-parser to indicate it reached the end of the HTTP response messsage.
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

/* This code prints debugging information and is, therefore, compiled only when
 * log level is set to IOT_LOG_DEBUG. */
#if (LIBRARY_LOG_LEVEL == IOT_LOG_DEBUG)
/**
 * @brief Callback from http-parser to indicate it found an HTTP Transfer-Encoding: chunked header.
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
 * @brief Callback from http-parser to indicate it reached the end of an HTTP response messsage "chunk".
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
 * This function is called by the network layer whenever data is available for the HTTP library.
 * 
 * @param[in] pNetworkConnection - The network connection with the HTTPS connection, passed by the network stack.
 * @param[in] pReceiveContext - A pointer to the HTTPS Client connection handle for which the packet was received.
 */
static void _networkReceiveCallback( void* pNetworkConnection, void* pReceiveContext );

/**
 * @brief Connects to HTTPS server and initializes the connection context.
 * 
 * @param[out] pConnHandle - The out parameter to return handle representing the open connection.
 * @param[in] pConnInfo - The connection configuration.
 * 
 * @return #IOT_HTTPS_OK if the connection context initialization was successful.
 *         #IOT_HTTPS_CONNECTION_ERROR if the connection failed.
 *         #IOT_HTTPS_INTERNAL_ERROR if the context initialization failed.
 */
static IotHttpsReturnCode_t _createHttpsConnection(IotHttpsConnectionHandle_t * pConnHandle, IotHttpsConnectionInfo_t *pConnInfo);

/**
 * @brief Disconnects from the network.
 * 
 * @param[in] pHttpsConnection - HTTPS connection handle.
 */
static void _networkDisconnect(_httpsConnection_t* pHttpsConnection);

/**
 * @brief Destroys the network connection.
 * 
 * @param[in] pHttpsConnection - HTTPS connection handle.
 */
static void _networkDestroy(_httpsConnection_t* pHttpsConnection);

/**
 * @brief Add a header to the current HTTP request.
 * 
 * The headers are stored in reqHandle->pHeaders.
 * 
 * @param[in] pHttpsRequest - HTTP request context.
 * @param[in] pName - The name of the header to add. This is a NULL terminated string.
 * @param[in] pValue - The buffer containing the value string.
 * @param[in] valueLen - The length of the header value string.
 * 
 * @return #IOT_HTTPS_OK if the header was added to the request successfully.
 *         #IOT_HTTPS_INSUFFICIENT_MEMORY if there was not enough room in the IotHttpsRequestHandle_t->pHeaders.
 */
static IotHttpsReturnCode_t _addHeader(_httpsRequest_t * pHttpsRequest, const char * pName, const char * pValue, uint32_t valueLen );

/**
 * @brief Send data on the network.
 * 
 * @param[in] pHttpsConnection - HTTP connection context.
 * @param[in] pBuf - The buffer containing the data to send.
 * @param[in] len - The length of the data to send.
 * 
 * @return #IOT_HTTPS_OK if the data sent successfully.
 *         #IOT_HTTPS_NETWORK_ERROR if there was an error sending the data on the network.
 */
static IotHttpsReturnCode_t _networkSend(_httpsConnection_t* pHttpsConnection, uint8_t * pBuf, size_t len);

/**
 * @brief Receive data on the network.
 * 
 * @param[in] pHttpsConnection - HTTP connection context.
 * @param[in] pBuf - The buffer to receive the data into.
 * @param[in] len - The length of the data to receive.
 * 
 * @return #IOT_HTTPS_OK if the data was received successfully.
 *         #IOT_HTTPS_NETWORK_ERROR if we timedout trying to receive data from the network.
 */
static IotHttpsReturnCode_t _networkRecv( _httpsConnection_t* pHttpsConnection, uint8_t * pBuf, size_t bufLen);

/**
 * @brief Send all of the HTTP request headers in the pHeadersBuf and the final Content-Length and Connection headers.
 * 
 * All of the headers in headerbuf are sent first followed by the computed content length and persistent connection 
 * indication.
 * 
 * @param[in] pHttpsConnection - HTTP connection context.
 * @param[in] pHeadersBuf - The buffer containing the request headers to send. This buffer must contain HTTP headers
 *            lines without the indicator for the the end of the HTTP headers.
 * @param[in] headersLength - The length of the request headers to send.
 * @param[in] isNonPersistent - Indicator of whether the connection is persistent or not.
 * @param[in] contentLength - The length of the request body used for automatically creating a "Content-Length" header.
 * 
 * @return #IOT_HTTPS_OK if the headers were fully sent successfully.
 *         #IOT_HTTPS_NETWORK_ERROR if there was an error receiving the data on the network.
 */
static IotHttpsReturnCode_t _sendHttpsHeaders( _httpsConnection_t* pHttpsConnection, uint8_t* pHeadersBuf, uint32_t headersLength, bool isNonPersistent, uint32_t contentLength);

/**
 * @brief Send all of the HTTP request body in pBodyBuf.
 * 
 * @param[in] pHttpsConnection - HTTP connection context.
 * @param[in] pBodyBuf - Buffer of the request body to send.
 * @param[in] bodyLength - The length of the body to send.
 * 
 * @return #IOT_HTTPS_OK if the body was fully sent successfully.
 *         #IOT_HTTPS_NETWORK_ERROR if there was an error receiving the data on the network.
 */
static IotHttpsReturnCode_t _sendHttpsBody( _httpsConnection_t* pHttpsConnection, uint8_t* pBodyBuf, uint32_t bodyLength);

/**
 * @brief Parse the HTTP response message in pBuf.
 * 
 * @param[in] pHttpParserInfo - Pointer to the information containing the instance of the http-parser and the execution function.
 * @param[in] pBuf - The buffer containing the data to parse.
 * @param[in] len - The length of data to parse.
 * 
 * @return #IOT_HTTPS_OK if the data was parsed successfully.
 *         #IOT_HTTPS_PARSING_ERROR if there was an error with parsing the data.
 */
static IotHttpsReturnCode_t _parseHttpsMessage(_httpParserInfo_t* pHttpParserInfo, char* pBuf, size_t len);

/**
 * @brief Receive any part of an HTTP response.
 * 
 * This function is used for both receiving the body into the body buffer and receiving the header into the header 
 * buffer.
 * 
 * @param[in] pHttpsConnection - HTTP Connection context.
 * @param[in] pParser - Pointer to the instance of the http-parser.
 * @param[in] pCurrentParserState - The current state of what has been parsed in the HTTP response.
 * @param[in] finalParserState - The final state of the parser expected after this function finishes.
 * @param[in] pBufCur - Pointer to the next location to write data into the buffer pBuf. This is double pointer to update the response context buffer pointers.
 * @param[in] pBufEnd - Pointer to the end of the buffer to receive the HTTP response into.
 *
 * @return #IOT_HTTPS_OK if we received the HTTP response message part successfully.
 *         #IOT_HTTPS_PARSING_ERROR if there was an error with parsing the data.
 *         #IOT_HTTPS_NETWORK_ERROR if there was an error receiving the data on the network.
 */
static IotHttpsReturnCode_t _receiveHttpsMessage( _httpsConnection_t* pHttpsConnection, 
                                                  _httpParserInfo_t* pParser,
                                                  IotHttpsResponseParserState_t *pCurrentParserState,
                                                  IotHttpsResponseParserState_t finalParserState, 
                                                  uint8_t** pBufCur,
                                                  uint8_t** pBufEnd);

/**
 * @brief Receive the HTTP response headers.
 *  
 * Receiving the response headers is always the first step in receiving the response, therefore the 
 * pHttpsResponse->httpParserInfo will be initialized to a starting state when this function is called.
 * 
 * This function also sets internal states to indicate that the header buffer is being processed now for a new response.
 * 
 * @param[in] pHttpsConnection - HTTP connection context.
 * @param[in] pHttpsResponse - HTTP response context.
 * 
 * @return #IOT_HTTPS_OK if we received the HTTP headers successfully.
 *         #IOT_HTTPS_PARSING_ERROR if there was an error with parsing the header buffer.
 *         #IOT_HTTPS_NETWORK_ERROR if there was an error receiving the data on the network.
 */
static IotHttpsReturnCode_t _receiveHttpsHeaders( _httpsConnection_t* pHttpsConnection, _httpsResponse_t* pHttpsResponse);

/**
 * @brief Receive the HTTP response body.
 * 
 * Sets internal states to indicate that the the body buffer is being processed now for a new response.
 * 
 * @param[in] pHttpsConnection - HTTP connection context.
 * @param[in] pHttpsResponse - HTTP response context.
 * 
 * @return #IOT_HTTPS_OK if we received the HTTP body successfully.
 *         #IOT_HTTPS_PARSING_ERROR if there was an error with parsing the body buffer.
 *         #IOT_HTTPS_NETWORK_ERROR if there was an error receiving the data on the network.
 */
static IotHttpsReturnCode_t _receiveHttpsBody( _httpsConnection_t* pHttpsConnection, _httpsResponse_t* pHttpsResponse);

/**
 * @brief Read the rest of any HTTP response that may be on the network.
 * 
 * This reads the rest of any left over response data that might still be on the network buffers. We do not want this
 * data left over because it will spill into the header and body buffers of next response that we try to receive. 
 * 
 * If we performed a request without a body and the headers received exceeds the size of the 
 * pHttpsResponse->pHeaders buffer, then we need to flush the network buffer.
 * 
 * If the application configured the body buffer as null in IotHttpsResponseInfo_t.syncInfo.respData and the server 
 * sends body in the response, but it exceeds the size of  pHttpsResponse->pHeaders buffer, then we need to flush the 
 * network buffer.
 * 
 * If the amount of body received on the network does not fit into a non-null IotHttpsResponseInfo_t.syncInfo.respData, 
 * then we need to flush the network buffer.
 * 
 * If an asynchronous request cancels in the middle of a response process, after already sending the request message, 
 * then we need to flush the network buffer.
 * 
 * @param[in] pHttpsConnection - HTTP connection context.
 * @param[in] pHttpsResponse - HTTP response context.
 * 
 * @return #IOT_HTTPS_OK if we successfully flushed the network data.
 *         #IOT_HTTPS_PARSING_ERROR if there was an error with parsing the data.
 *         #IOT_HTTPS_NETWORK_ERROR if there was an error receiving the data on the network.
 */
static IotHttpsReturnCode_t _flushHttpsNetworkData( _httpsConnection_t* pHttpsConnection, _httpsResponse_t* pHttpsResponse );

/**
 * @brief Task pool job routine to send the HTTP request within the pUserContext.
 * 
 * @param[in] pTaskPool Pointer to the system task pool.
 * @param[in] pJob Pointer the to the HTTP request sending job.
 * @param[in] pContext Pointer to an HTTP request, passed as an opaque context.
 */
static void _sendHttpsRequest( IotTaskPool_t pTaskPool, IotTaskPoolJob_t pJob, void * pUserContext );


/**
 * @brief Receive the HTTPS body specific to an asynchronous type of response.
 * 
 * @param[in] pHttpsResponse - HTTP response context.
 * 
 * @return  #IOT_HTTPS_OK - If the the response body was received with no issues. 
 *          #IOT_HTTPS_ASYNC_CANCELLED - If the request was cancelled by the Application
 *          #IOT_HTTPS_PARSING_ERROR - If there was an issue parsing the HTTP response body.
 *          #IOT_HTTPS_NETWORK_ERROR if there was an error receiving the data on the network.
 */
static IotHttpsReturnCode_t _receiveHttpsBodyAsync(_httpsResponse_t* pHttpsResponse);

/**
 * @brief Receive the HTTPS body specific to a synchronous type of response.
 * 
 * @param[in] pHttpsResponse - HTTP response context.
 * 
 * @return  #IOT_HTTPS_OK - If the the response body was received with no issues. 
 *          #IOT_HTTPS_MESSAGE_TOO_LARGE - If the body from the network is too large to fit into the configured body buffer.
 *          #IOT_HTTPS_PARSING_ERROR - If there was an issue parsing the HTTP response body.
 *          #IOT_HTTPS_NETWORK_ERROR if there was an error receiving the data on the network.
 */
static IotHttpsReturnCode_t _receiveHttpsBodySync(_httpsResponse_t* pHttpsResponse);

/**
 * @brief Schedule the task to send the the HTTP request.
 * 
 * @param[in] pHttpsRequest - HTTP request context.
 * 
 * @return  #IOT_HTTPS_OK - If the task to send the HTTP request was successfully scheduled.
 *          #IOT_HTTPS_INTERNAL_ERROR - If a taskpool job could not be created.
 *          #IOT_HTTPS_ASYNC_SCHEDULING_ERROR - If there was an error scheduling the job.
 */
IotHttpsReturnCode_t _scheduleHttpsRequestSend(_httpsRequest_t* pHttpsRequest);

/**
 * @brief Add the request to the connection's request queue.
 * 
 * This will schedule a task if the request is first and only request in the queue.
 * 
 * @param[in] pHttpsRequest - HTTP request context.
 * 
 * @return  #IOT_HTTPS_OK - If the request was successfully added to the connection's request queue.
 *          #IOT_HTTPS_INTERNAL_ERROR - If a taskpool job could not be created.
 *          #IOT_HTTPS_ASYNC_SCHEDULING_ERROR - If there was an error scheduling the job.
 */
IotHttpsReturnCode_t _addRequestToConnectionReqQ(_httpsRequest_t* pHttpsRequest);

/**
 * @brief Cancel the HTTP request's processing.
 * 
 * pHttpsRequest->cancelled will be checked and the request cancelled if specified so at the following intervals:
 *  - Before sending the HTTPS headers at the start of the scheduled sending of the HTTPS request.
 *  - After Sending the HTTPS headers.
 *  - After Sending the HTTPS body.
 * 
 * @param[in] pHttpsRequest - HTTP request context.
 */
static void _cancelRequest(_httpsRequest_t *pHttpsRequest);

/**
 * @brief Cancel the HTTP response's processing.
 * 
 * pHttpsResponse->cancelled will be checked and the response cancelled if specified so at the following intervals:
 *  - At the start of the network receive callback.
 *  - After receiving the HTTPS headers.
 *  - After Receiving the HTTPS body.
 * 
 * @param[in] pHttpsResponse - HTTP response context.
 */
static void _cancelResponse(_httpsResponse_t *pHttpsResponse);

/**
 * @brief Initialize the input pHttpsResponse with pRespInfo.
 * 
 * @param[in] pHttpsResponse - Non-null HTTP response context.
 * @param[in] pRespInfo - Response configuration information.
 * @param[in] isAsync - Set to true for an asynchronous response, false otherwise.
 * @param[in] method - The HTTPS method of the originating request. 
 */
static IotHttpsReturnCode_t _initializeResponse( IotHttpsResponseHandle_t* pRespHandle, 
                                                 IotHttpsResponseInfo_t* pRespInfo, 
                                                 bool isAsync, 
                                                 IotHttpsMethod_t method );

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
    int retVal = KEEP_PARSING;
    IotLogDebug("Parser: Start of HTTPS Response message.");
    
    _httpsResponse_t *pHttpsResponse = (_httpsResponse_t*)(pHttpParser->data);
    /* Set the state of the parser. The headers are at the start of the message always. */
    pHttpsResponse->parserState = PARSER_STATE_IN_HEADERS;
    return retVal;
}

/*-----------------------------------------------------------*/

static int _httpParserOnStatusCallback(http_parser * pHttpParser, const char * pLoc, size_t length)
{
    _httpsResponse_t *pHttpsResponse = (_httpsResponse_t*)(pHttpParser->data);
    IotLogDebug("Parser: Status %.*s retrieved from HTTPS response.", length, pLoc);

    /* Save the status code so it can be retrieved with IotHttpsClient_ReadResponseStatus(). */
    pHttpsResponse->status = (uint16_t)(pHttpParser->status_code);
    /* If we are parsing the network data received in the header buffer then we
       increment pHttpsResponse->pHeadersCur. The status line in the response is
       part of the data stored in header buffer _httpResponse->pHeaders. */
    if(pHttpsResponse->bufferProcessingState == PROCESSING_STATE_FILLING_HEADER_BUFFER)
    {
        /* pHeadersCur will never exceed the pHeadersEnd here because PROCESSING_STATE_FILLING_HEADER_BUFFER 
           indicates we are currently in the header buffer and the total size of the header buffer is passed
           into http_parser_execute() as the maximum length to parse. */
        pHttpsResponse->pHeadersCur = (uint8_t*)(pLoc += length);
    }
    return KEEP_PARSING;
}

/*-----------------------------------------------------------*/

static int _httpParserOnHeaderFieldCallback(http_parser * pHttpParser, const char * pLoc, size_t length)
{
    IotLogDebug("Parser: HTTPS header field parsed %.*s", length, pLoc);

    _httpsResponse_t * pHttpsResponse = (_httpsResponse_t *)(pHttpParser->data);
    /* If we are parsing the network data received in the header buffer then we can increment 
      pHttpsResponse->pHeadersCur. */
    if(pHttpsResponse->bufferProcessingState == PROCESSING_STATE_FILLING_HEADER_BUFFER)
    {
        pHttpsResponse->pHeadersCur = (uint8_t*)(pLoc += length);
    }
    /* If the IotHttpsClient_ReadHeader() was called, then we check for the header field of interest. */
    if(pHttpsResponse->bufferProcessingState == PROCESSING_STATE_SEARCHING_HEADER_BUFFER)
    {
        if(strncmp(pHttpsResponse->pReadHeaderField, pLoc, length) == 0)
        {
            pHttpsResponse->foundHeaderField = true;   
        }
    }
    return KEEP_PARSING;
}

/*-----------------------------------------------------------*/

static int _httpParserOnHeaderValueCallback(http_parser * pHttpParser, const char * pLoc, size_t length)
{
    int retVal = KEEP_PARSING;

    IotLogDebug("Parser: HTTPS header value parsed %.*s", length, pLoc);
    _httpsResponse_t * pHttpsResponse = (_httpsResponse_t *)(pHttpParser->data);
    /* If we are parsing the network data received in the header buffer then we can increment 
      pHttpsResponse->pHeadersCur. */
    if(pHttpsResponse->bufferProcessingState == PROCESSING_STATE_FILLING_HEADER_BUFFER)
    {
        pHttpsResponse->pHeadersCur = (uint8_t*)(pLoc += length);
    }

    /* If the IotHttpsClient_ReadHeader() was called, then we check if we found the header field of interest. */
    if(pHttpsResponse->bufferProcessingState == PROCESSING_STATE_SEARCHING_HEADER_BUFFER)
    {
        if(pHttpsResponse->foundHeaderField)
        {
            pHttpsResponse->pReadHeaderValue = ( char* )( pLoc );
            pHttpsResponse->readHeaderValueLength = length;
            /* We found a header field so we don't want to keep parsing.*/
            retVal = STOP_PARSING;
        }
    }
    return retVal;
}

/*-----------------------------------------------------------*/

static int _httpParserOnHeadersCompleteCallback(http_parser * pHttpParser)
{
    IotLogDebug("Parser: End of the headers reached.");

    int retVal = KEEP_PARSING;
    _httpsResponse_t * pHttpsResponse = (_httpsResponse_t *)(pHttpParser->data);
    pHttpsResponse->parserState = PARSER_STATE_HEADERS_COMPLETE;

    /* If the IotHttpsClient_ReadHeader() was called, we return after finishing looking through all of the headers. 
       Returning a non-zero value exits the http parsing. */
    if(pHttpsResponse->bufferProcessingState == PROCESSING_STATE_SEARCHING_HEADER_BUFFER)
    {
        retVal = STOP_PARSING;
    }
    
    /* When in this callback the pHeaderCur pointer is at the first "\r" in the last header line. HTTP/1.1
    headers end with another "\r\n" at the end of the last line. This means we must increment
    the headerCur pointer to the length of "\r\n\r\n". */
    if(pHttpsResponse->bufferProcessingState == PROCESSING_STATE_FILLING_HEADER_BUFFER)
    {
        pHttpsResponse->pHeadersCur += ( 2 * HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH );
    }

    if(pHttpsResponse->bufferProcessingState < PROCESSING_STATE_FINISHED)
    {
        /* For a HEAD method, there is no body expected in the response, so we return 1 to skip body parsing. */
        if( ( pHttpsResponse->method == IOT_HTTPS_METHOD_HEAD ) )
        {
            retVal = STOP_PARSING;
            /* Since the message is considered complete now for a HEAD response, then we set the parser state 
               to the completed state. */
            pHttpsResponse->parserState = PARSER_STATE_BODY_COMPLETE;
        }
        /* If this is NOT a HEAD method and there is body configured, but the server does not send a body in the
           response, then the body buffer will be filled with the zeros from rest of the header buffer. http-parser
           will invoke the on_body callback and consider the zeros following the headers as body. */

        /* Also if it was configured in a synchronous response to ignore the HTTPS response body then also stop the body
           parsing. There could be body in the message, despite the body configured to be ignore, so the parser state
           stays as is so that the network socket can be flushed of the body if needed. */
        if(( pHttpsResponse->isAsync == false ) && ( pHttpsResponse->pBody == NULL ))
        {
            retVal = STOP_PARSING;
        }
    }

    return retVal;
}

/*-----------------------------------------------------------*/

static int _httpParserOnBodyCallback(http_parser * pHttpParser, const char * pLoc, size_t length)
{
    IotLogDebug("Parser: Reached the HTTPS message body. It is of length: %d", length);

    _httpsResponse_t * pHttpsResponse = (_httpsResponse_t *)(pHttpParser->data);
    pHttpsResponse->parserState = PARSER_STATE_IN_BODY;

    if((pHttpsResponse->bufferProcessingState == PROCESSING_STATE_FILLING_HEADER_BUFFER) && (pHttpsResponse->isAsync))
    {
        /* For an asynchronous response, the buffer to store the body will be available after the headers 
         * are read first. We may receive part of the body in the header buffer. We will want to leave this here
         * and copy it over when the body buffer is available in the _readReadyCallback().
         */
        if( pHttpsResponse->pBodyInHeaderBuf == NULL )
        {
            pHttpsResponse->pBodyInHeaderBuf = ( uint8_t* )( pLoc );
            pHttpsResponse->pBodyCurInHeaderBuf = pHttpsResponse->pBodyInHeaderBuf;
        }

        /* If there is a chunk encoded body in the header buffer, we will want to overwrite the chunk headers with the 
           actual body. This is so that when the application calls IotHttpsClient_ReadResponseBody(), in the 
           readReadyCallback(), we can pass the body into the body buffer provided right away. */
        if(pHttpsResponse->pBodyCurInHeaderBuf != (uint8_t*)pLoc)
        {
            memcpy(pHttpsResponse->pBodyCurInHeaderBuf, pLoc, length);
        }
        pHttpsResponse->pBodyCurInHeaderBuf += length;

    }
    else if(pHttpsResponse->bufferProcessingState < PROCESSING_STATE_FINISHED)
    {
        /* Has the user provided a buffer and is it large enough to fit the body? The
        case of body buffer not being large enough can happen if the body was received
        in the header buffer and the body buffer can not fit in all the body. */
        if((pHttpsResponse->pBodyCur != NULL) && (pHttpsResponse->pBodyEnd - pHttpsResponse->pBodyCur > 0))
        {
            /* There are two scenarios when we need to copy data around:
            1. Some or all of the response body may have been received in the header
            buffer. If that is the case, we copy the response body received in the
            header buffer to the user provided body buffer.
            2. When we receive chunked header, the actual body is separated in
            multiple chunks which are preceeded by length. For example, a chunked
            body may look like:

            7\r\n
            Mozilla\r\n 
            9\r\n
            Developer\r\n
            7\r\n
            Network\r\n
            0\r\n 
            \r\n

            In this case, we want the parsed body buffer to contain actual body only
            (MozillaDeveloperNetwork in the above example).
            */
            /* If the response body found by the parser (pLoc) is not equal to the
            current writable location in the body buffer (_httpsResponse->pBodyCur),
            it indicates that:
            - Either the data is in the header buffer and needs to be copied into the
            body buffer.
            - Or it is a chunked response and the data needs to be moved up in the
            body buffer. */
            if( (pHttpsResponse->pBodyCur + length) <= pHttpsResponse->pBodyEnd )
            {
                if(pHttpsResponse->pBodyCur != (uint8_t*)pLoc)
                {
                    memcpy(pHttpsResponse->pBodyCur, pLoc, length);
                }
                pHttpsResponse->pBodyCur += length;
            }
        }
    }

    return KEEP_PARSING;
}

/*-----------------------------------------------------------*/

static int _httpParserOnMessageCompleteCallback(http_parser * pHttpParser)
{
    IotLogDebug("Parser: End of the HTTPS message reached.");

    _httpsResponse_t * pHttpsResponse = (_httpsResponse_t *)(pHttpParser->data);
    pHttpsResponse->parserState = PARSER_STATE_BODY_COMPLETE;

    /* This callback is invoked when the complete HTTP response has been received.
    We tell the parser to parse the whole body buffer as opposed to the size of
    the response body. For example, if the size of the body buffer is 1000 but
    the size of the actual body is 500, we tell the parser to parse the whole
    buffer of length 1000. We do zero out the buffer in the beginning so that all
    the buffer after the actual body contains zeros. We return greater than zero to stop parsing
    since the end of the HTTP message has been reached. Any data beyond the end of the message is
    ignored. */
    return STOP_PARSING;
}

/*-----------------------------------------------------------*/

/* This code prints debugging information and is, therefore, compiled only when
 * log level is set to IOT_LOG_DEBUG. */
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

static IotHttpsReturnCode_t _receiveHttpsBodyAsync(_httpsResponse_t* pHttpsResponse)
{
    HTTPS_FUNCTION_ENTRY(IOT_HTTPS_OK);
    /* Get the request reference from the response for the callback function pointers. */
    _httpsRequest_t* pHttpsRequest = pHttpsResponse->pHttpsRequest;

    if(pHttpsRequest->pCallbacks->readReadyCallback)
    {
        /* If there is still more body that has not been passed back to the user, then this callback is invoked again. */
        do {
            pHttpsRequest->pCallbacks->readReadyCallback(pHttpsRequest->pUserPrivData, 
                pHttpsResponse, 
                pHttpsResponse->bodyRxStatus, 
                pHttpsResponse->status);
            if(pHttpsResponse->cancelled == true)
            {
                IotLogDebug("Cancelled HTTP response %d.", pHttpsResponse);
                status = IOT_HTTPS_ASYNC_CANCELLED;
                /* We break out of the loop and do not goto clean up because we want to print debugging logs for 
                   the parser state and the networks status. */
                break;
            }
        } while((pHttpsResponse->parserState < PARSER_STATE_BODY_COMPLETE) && (pHttpsResponse->bodyRxStatus == IOT_HTTPS_OK));

        if(HTTPS_FAILED(pHttpsResponse->bodyRxStatus))
        {
            IotLogError("Error receiving the HTTP response body for response %d. Error code: %d",
                pHttpsResponse,
                pHttpsResponse->bodyRxStatus);
        }

        if(pHttpsResponse->parserState < PARSER_STATE_BODY_COMPLETE)
        {
            IotLogDebug("Did not receive all of the HTTP response body for response %d.", 
                pHttpsResponse);
        }
    }

    status = pHttpsResponse->bodyRxStatus;
    /* This GOTO cleanup is here for compiler warnings about using HTTPS_FUNCTION_EXIT_NO_CLEANUP() without a 
       corresponding goto. */
    HTTPS_GOTO_CLEANUP();
    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _receiveHttpsBodySync(_httpsResponse_t* pHttpsResponse)
{
    HTTPS_FUNCTION_ENTRY(IOT_HTTPS_OK);
    _httpsConnection_t* pHttpsConnection = pHttpsResponse->pHttpsConnection;

    /* The header buffer is now filled or the end of the headers has been reached already. If part of the response
        body was read from the network into the header buffer, then it was already copied to the body buffer in the 
        _httpParserOnBodyCallback(). */
    if(pHttpsResponse->pBody != NULL)
    {
        /* If there is room left in the body buffer and we have not received the whole response body, 
           then try to receive more. */
        if( ( (pHttpsResponse->pBodyEnd - pHttpsResponse->pBodyCur) > 0 ) &&
            ( pHttpsResponse->parserState < PARSER_STATE_BODY_COMPLETE ) )
        {
            status = _receiveHttpsBody( pHttpsConnection,
                pHttpsResponse );
            if( HTTPS_FAILED(status) )
            {
                IotLogError( "Error receiving the HTTPS response body for response %d. Error code: %d.", 
                    pHttpsResponse,
                    status );
                HTTPS_GOTO_CLEANUP();
            }
        }
        else
        {
            IotLogDebug("Received the maximum amount of HTTP body when filling the header buffer for response %d.",
                pHttpsResponse);
        }

        /* If we don't reach the end of the HTTPS body in the parser, then we only received part of the body.
            The rest of body will be on the network socket. */
        if( HTTPS_SUCCEEDED(status) && (pHttpsResponse->parserState < PARSER_STATE_BODY_COMPLETE ))
        {
            IotLogError( "HTTPS response body does not fit into application provided response buffer at location 0x%x "
                "with length: %d",
                pHttpsResponse->pBody,
                pHttpsResponse->pBodyEnd - pHttpsResponse->pBody );
            HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_MESSAGE_TOO_LARGE);
        }
    }
    else
    {
        IotLogDebug("No response body was configure for response %d.", pHttpsResponse);
    }

    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

static void _networkReceiveCallback( void* pNetworkConnection, void* pReceiveContext )
{
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

    IotHttpsReturnCode_t flushStatus = IOT_HTTPS_OK;
    IotHttpsReturnCode_t disconnectStatus = IOT_HTTPS_OK;
    _httpsConnection_t* pHttpsConnection = ( _httpsConnection_t* )pReceiveContext;
    _httpsResponse_t* pCurrentHttpsResponse = NULL;
    _httpsResponse_t* pNextHttpsResponse = NULL;
    _httpsRequest_t* pCurrentHttpsRequest = NULL;
    _httpsRequest_t* pNextHttpsRequest = NULL;
    IotLink_t * pQItem = NULL;
    bool fatalDisconnect = false; 

    /* The network connection is already in the connection context. */
    ( void )pNetworkConnection;

    /* Dequeue response from the response queue. */
    IotMutex_Lock( &( pHttpsConnection->respQMutex ) );
    pQItem = IotDeQueue_DequeueHead( &(pHttpsConnection->respQ));
    IotMutex_Unlock( &(pHttpsConnection->respQMutex) );

    /* If the receive callback is invoked and there is no response expected, then this a violation of the HTTP/1.1
       protocol. */
    if( pQItem == NULL )
    {
        IotLogError("Received data on the network, when no response was expected...");
        fatalDisconnect = true;
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_NETWORK_ERROR);
    }

    /* Set the current HTTP response context to use. */
    pCurrentHttpsResponse = IotLink_Container(_httpsResponse_t, pQItem, link);

    /* Set the current HTTP request associated with this response. */
    pCurrentHttpsRequest = pCurrentHttpsResponse->pHttpsRequest;

    /* If the receive callback has invoked, but the request associated with this response has not finished sending 
       to the server, then this is a violation of the HTTP/1.1 protocol.  */
    if(pCurrentHttpsRequest->reqFinishedSending == false)
    {
        IotLogError("Received response data on the network when the request was not finished sending. This is unexpected.");
        fatalDisconnect = true;
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_NETWORK_ERROR);
    }

    /* If the current response was cancelled, then don't bother receiving the headers and body. */
    if(pCurrentHttpsResponse->cancelled)
    {
        IotLogDebug("Response ID: %d was cancelled.", pCurrentHttpsResponse);
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_ASYNC_CANCELLED);
    }

    /* Reset the http-parser state to an initial state. This is done so that a new response can be parsed from the 
       beginning. */
    pCurrentHttpsResponse->httpParserInfo.responseParser.data = (void *)(pCurrentHttpsResponse);
    pCurrentHttpsResponse->parserState = PARSER_STATE_NONE;
    pCurrentHttpsResponse->bufferProcessingState = PROCESSING_STATE_FILLING_HEADER_BUFFER;

    /* Receive the response from the network. */
    /* Receive the headers first. */
    status = _receiveHttpsHeaders(pHttpsConnection, pCurrentHttpsResponse);
    if(HTTPS_FAILED(status))
    {
        if(status == IOT_HTTPS_PARSING_ERROR)
        {
            /* There was an error parsing the HTTPS response body. This may be an indication of a server that does
                not adhere to protocol correctly. We should disconnect. */
            IotLogError("Failed to parse the HTTPS headers for response %d, Error code: %d.", 
                pCurrentHttpsResponse,
                status);
            fatalDisconnect = true;
        }
        else if(status == IOT_HTTPS_NETWORK_ERROR)
        {
            /* Given the function signature of IotNetworkInterface_t.receive, we can only receive 0 to the number of bytes 
            requested. Receiving less than the number of bytes requests is OK since we do not how much data is expected, so 
            we ask for the full size of the receive buffer. Thereofore, the only error that can be returned from receiving 
            the headers or body is a timeout. We always disconnect from the network when there is a timeout because the 
            server may be slow to respond. If the server happens to send the response later at the same time another response 
            is waiting in the queue, then the workflow is corrupted. Pipelining is not current supported in this library. */
            IotLogError("Network error receiving the HTTPS headers for response %d. Error code: %d",
                pCurrentHttpsResponse, 
                status);
            fatalDisconnect = true;
        }
        else /* Any other error. */
        {
            IotLogError("Failed to retrive the HTTPS body for request %d. Error code: %d", status);
        }
        HTTPS_GOTO_CLEANUP();
    }

    /* Check if we received all of the headers into the header buffer. */
    if( pCurrentHttpsResponse->parserState < PARSER_STATE_HEADERS_COMPLETE )
    {
        IotLogDebug( "Headers received on the network did not all fit into the configured header buffer for request %d."
            " The length of the headers buffer is: %d",
            pCurrentHttpsRequest,
            pCurrentHttpsResponse->pHeadersEnd - pCurrentHttpsResponse->pHeaders );
        /* It is not error if the headers did not all fit into the buffer. */
    }

    /* Receive the body. */
    if(pCurrentHttpsResponse->isAsync)
    {
        status = _receiveHttpsBodyAsync(pCurrentHttpsResponse);
    }
    else
    {
        /* Otherwise receive synchronously. */
        status = _receiveHttpsBodySync(pCurrentHttpsResponse);
    }

    if(HTTPS_FAILED(status))
    {
        if(status == IOT_HTTPS_ASYNC_CANCELLED)
        {
            /* If the request was cancelled, this is logged, but does not close the connection. */
            IotLogDebug("User cancelled during the async readReadyCallback() for response %d.", 
                pCurrentHttpsResponse);
        }
        else if(status == IOT_HTTPS_PARSING_ERROR)
        {
            /* There was an error parsing the HTTPS response body. This may be an indication of a server that does
                not adhere to protocol correctly. We should disconnect. */
            IotLogError("Failed to parse the HTTPS body for response %d, Error code: %d.", 
                pCurrentHttpsResponse,
                status);
            fatalDisconnect = true;
        }
        else if(status == IOT_HTTPS_NETWORK_ERROR)
        {
            /* We walways disconnect for a network error because failure to receive the HTTPS body will result in a 
               corruption of the workflow. */
            IotLogError("Network error receiving the HTTPS body for response %d. Error code: %d",
                pCurrentHttpsResponse, 
                status);
            fatalDisconnect = true;
        }
        else /* Any other error. */
        {
            IotLogError("Failed to retrive the HTTPS body for request %d. Error code: %d", status);
        }
        HTTPS_GOTO_CLEANUP();
    }

    IOT_FUNCTION_CLEANUP_BEGIN();

    /* Report errors back to the application. */
    if(HTTPS_FAILED(status))
    {
        if(pCurrentHttpsResponse->isAsync && pCurrentHttpsRequest->pCallbacks->errorCallback)
        {
            pCurrentHttpsRequest->pCallbacks->errorCallback(pCurrentHttpsRequest->pUserPrivData, pCurrentHttpsRequest, status);
        }
        pCurrentHttpsResponse->syncStatus = status;
    }

    /* If this is not a persistent request, the server would have closed it after sending a response, but we 
       disconnect anyways. If we are disconnecting there is is no point in wasting time 
       flushing the network. If the network is being disconnected we also do not schedule any pending requests. */
    if( fatalDisconnect || pCurrentHttpsRequest->isNonPersistent )
    {
        IotLogDebug("Disconnecting response %d.", pCurrentHttpsResponse);
        disconnectStatus = IotHttpsClient_Disconnect(pHttpsConnection);
        if((pCurrentHttpsResponse != NULL) && pCurrentHttpsResponse->isAsync && pCurrentHttpsRequest->pCallbacks->connectionClosedCallback)
        {
            pCurrentHttpsRequest->pCallbacks->connectionClosedCallback(pCurrentHttpsRequest->pUserPrivData, pHttpsConnection, disconnectStatus);
        }

        if(HTTPS_FAILED(disconnectStatus))
        {
            IotLogWarn("Failed to disconnect response %d. Error code: %d.", pCurrentHttpsResponse, disconnectStatus);
        }

        /* If we disconnect, we do not process anymore requests. */
    }
    else
    {
        /* Set the processing state of the buffer to finished for completeness. This is also to prevent the parsing of the flush
           data from incrementing any pointer in the HTTP response context. */
        pCurrentHttpsResponse->bufferProcessingState = PROCESSING_STATE_FINISHED;
        
        /* Flush the socket of the rest of the data if there is data left from this response. We need to do this
           so that for the next request on this connection, there is not left over response from this request in
           the next response buffer.

           If a continuous stream of data is coming in from the connection, with an unknown end, we may not be able to
           flush the network data. It may sit here forever. A continuous stream should be ingested with the async workflow. 
        
           All network errors are ignore here because network read will have read the data from network buffer despite
           errors. */
        flushStatus = _flushHttpsNetworkData( pHttpsConnection, pCurrentHttpsResponse );
        if( flushStatus == IOT_HTTPS_PARSING_ERROR)
        {
            IotLogWarn("There an error parsing the network flush data. The network buffer might not be fully flushed.");
        }
        else if( flushStatus != IOT_HTTPS_OK )
        {
            IotLogDebug("Network error when flushing the https network data: %d", flushStatus);
        }
        
        IotMutex_Lock(&(pHttpsConnection->reqQMutex));
        /* Now that the current request/response pair is finished, we dequeue the current request from the queue. */
        pQItem = IotDeQueue_DequeueHead( &(pHttpsConnection->reqQ));
        /* Get the next request to process. */
        pQItem = IotDeQueue_PeekHead( &(pHttpsConnection->reqQ));
        IotMutex_Unlock(&(pHttpsConnection->reqQMutex));

        /* If there is a next request to process, then create a taskpool job to send the request. */
        if(pQItem != NULL)
        {
            /* Set this next request to send. */
            pNextHttpsRequest = IotLink_Container(_httpsRequest_t, pQItem, link);
            /* Set the next response to receive. */
            pNextHttpsResponse = pNextHttpsRequest->pHttpsResponse;

            IotLogDebug("Request %d is next in the queue. Now scheduling a task to send the request.", pNextHttpsRequest);
            status = _scheduleHttpsRequestSend(pNextHttpsRequest);
            /* If there was an error with scheduling the new task, then report it. */
            if(HTTPS_FAILED(status))
            {
                IotLogError("Error scheduling HTTPS request %d. Error code: %d", pNextHttpsRequest, status);
                if(pNextHttpsResponse->isAsync && pNextHttpsRequest->pCallbacks->errorCallback)
                {
                    pNextHttpsRequest->pCallbacks->errorCallback(pNextHttpsRequest->pUserPrivData, pNextHttpsRequest, status);
                }
                else
                {
                    pNextHttpsResponse->syncStatus = status;
                }
            }
        }
        else
        {
            IotLogDebug("Network receive callback found the request queue empty. A network send task was not scheduled.");
        }
    }

    /* Signal to a synchronous reponse that the response is complete. */
    if( pCurrentHttpsResponse->isAsync && pCurrentHttpsRequest->pCallbacks->responseCompleteCallback )
    {
        pCurrentHttpsRequest->pCallbacks->responseCompleteCallback( pCurrentHttpsRequest->pUserPrivData, pCurrentHttpsResponse, status, pCurrentHttpsResponse->status );
    }

    /* For a synchronous request release the semaphore. */
    if(pCurrentHttpsResponse->isAsync == false)
    {
        IotSemaphore_Post(&(pCurrentHttpsResponse->respFinishedSem));
    }
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _createHttpsConnection(IotHttpsConnectionHandle_t * pConnHandle, IotHttpsConnectionInfo_t *pConnInfo)
{
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

    IotNetworkError_t networkStatus = IOT_NETWORK_SUCCESS;
    /* The maximum string length of the ALPN protocols is configured in IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH. 
       The +1 is for the NULL terminator needed by IotNetworkCredentials_t.pAlpnProtos. */
    char pAlpnProtos[IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH + 1] = { 0 };
    /* The maximum string length of the Server host name is configured in IOT_HTTPS_MAX_HOST_NAME_LENGTH. 
       This +1 is for the NULL terminator needed by IotNetworkServerInfo_t.pHostName. */
    char pHostName[IOT_HTTPS_MAX_HOST_NAME_LENGTH + 1] = { 0 };
    bool reqQMutexCreated = false;
    bool respQMutexCreated = false;
    IotNetworkServerInfo_t networkServerInfo = { 0 };
    IotNetworkCredentials_t networkCredentials = { 0 };
    _httpsConnection_t *pHttpsConnection = NULL;

    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(pConnInfo->userBuffer.pBuffer);
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(pConnInfo->pNetworkInterface);
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(pConnInfo->pAddress);
    HTTPS_ON_ARG_ERROR_GOTO_CLEANUP(pConnInfo->addressLen > 0);

    /* Make sure the connection context can fit in the user buffer. */
    HTTPS_ON_ARG_ERROR_MSG_GOTO_CLEANUP(pConnInfo->userBuffer.bufferLen >= connectionUserBufferMinimumSize, 
        IOT_HTTPS_INSUFFICIENT_MEMORY,  
        "Buffer size is too small to initialize the connection context. User buffer size: %d, required minimum size; %d.",
        (*pConnInfo).userBuffer.bufferLen, 
        connectionUserBufferMinimumSize);

    /* Make sure that the server address does not exceed the maximum permitted length. */
    HTTPS_ON_ARG_ERROR_MSG_GOTO_CLEANUP(pConnInfo->addressLen <= IOT_HTTPS_MAX_HOST_NAME_LENGTH,
        IOT_HTTPS_INVALID_PARAMETER,
        "IotHttpsConnectionInfo_t.addressLen has a host name length %d that exceeds maximum length %d.",
        pConnInfo->addressLen,
        IOT_HTTPS_MAX_HOST_NAME_LENGTH);

    /* Make sure that the ALPN protocols does not exceed the maximum permitted length. */
    HTTPS_ON_ARG_ERROR_MSG_GOTO_CLEANUP(pConnInfo->alpnProtocolsLen <= IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH,
        IOT_HTTPS_INVALID_PARAMETER,
        "IotHttpsConnectionInfo_t.alpnProtocolsLen of %d exceeds the configured maximum protocol length %d. See IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH for more information.",
        pConnInfo->alpnProtocolsLen,
        IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH);

    pHttpsConnection = (_httpsConnection_t *)(pConnInfo->userBuffer.pBuffer);
    
    /* Start with the disconnected state. */
    pHttpsConnection->isConnected = false;

    /* Initialize the queue of responses and requests. */
    IotDeQueue_Create( &(pHttpsConnection->reqQ) );
    IotDeQueue_Create( &(pHttpsConnection->respQ) );

    /* This timeout is used to wait for a response on the connection as well as
    for the timeout for the connect operation. */
    if( pConnInfo->timeout == 0 )
    {
        pHttpsConnection->timeout = IOT_HTTPS_RESPONSE_WAIT_MS;
    }
    else
    {
        pHttpsConnection->timeout = pConnInfo->timeout;
    }

    /* pNetworkInterface contains the connect, disconnect, send, and receive functions. */
    pHttpsConnection->pNetworkInterface = pConnInfo->pNetworkInterface;
    
    /* Copy the host name configuration from the address in the connection information. */
    memcpy( pHostName, pConnInfo->pAddress, pConnInfo->addressLen );
    pHostName[pConnInfo->addressLen] = '\0';
    /* Set it in the IOT network abstractions server information parameter. */
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
        networkStatus = pHttpsConnection->pNetworkInterface->create( &networkServerInfo,
            &networkCredentials,
            &( pHttpsConnection->pNetworkConnection ) );
    }
    else
    {
        networkStatus = pHttpsConnection->pNetworkInterface->create( &networkServerInfo,
            NULL,
            &( pHttpsConnection->pNetworkConnection ) );
    }

    /* Check to see if the network connection succeeded. If it did not succeed,
       then the output parameter pConnHandle will be used to return NULL and the
       function returns an error. */
    if( networkStatus != IOT_NETWORK_SUCCESS )
    {
        IotLogError( "Failed to connect to the server at %.*s on port %d with error: %d",
            pConnInfo->addressLen,
            pConnInfo->pAddress,
            pConnInfo->port,
            networkStatus );
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_CONNECTION_ERROR);
    }
    else
    {
        /* The connection succeeded so set the state to connected. */
        pHttpsConnection->isConnected = true;
    }

    /* The receive callback is invoked by the network layer when data is ready
      to be read from the network. */
    networkStatus = pHttpsConnection->pNetworkInterface->setReceiveCallback( pHttpsConnection->pNetworkConnection,
        _networkReceiveCallback,
        pHttpsConnection );
    if( networkStatus != IOT_NETWORK_SUCCESS )
    {
        IotLogError( "Failed to connect to set the HTTPS receive callback. " );
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INTERNAL_ERROR);
    }

    /* Connection was successful, so create synchronization primitives. */

    /* Create the mutex protecting operations the queue of requests waiting to be serviced in this connection. */
    reqQMutexCreated = IotMutex_Create( &(pHttpsConnection->reqQMutex), false );
    if(!reqQMutexCreated)
    {
        IotLogError("Failed to create an internal mutex.");
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INTERNAL_ERROR);
    }

    respQMutexCreated = IotMutex_Create( &(pHttpsConnection->respQMutex), false );
    if(!respQMutexCreated)
    {
        IotLogError("Failed to create an internal mutex.");
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INTERNAL_ERROR);
    }

    /* Return the new connection information. */
    *pConnHandle = pHttpsConnection;

    HTTPS_FUNCTION_CLEANUP_BEGIN();

    /* If we failed anywhere in the connection process, then destroy the semaphores created. */
    if( HTTPS_FAILED( status ) )
    {
        /* If there was a connect was successful, disconnect from the network.  */
        if(( pHttpsConnection != NULL) && (pHttpsConnection->isConnected))
        {   
            _networkDisconnect( pHttpsConnection );
            _networkDestroy( pHttpsConnection );
        }

        if(reqQMutexCreated)
        {
            IotMutex_Destroy(&(pHttpsConnection->reqQMutex));
        }

        if(respQMutexCreated)
        {
            IotMutex_Destroy(&(pHttpsConnection->respQMutex));
        }

        /* Set the connection handle as NULL if everything failed. */
        *pConnHandle = NULL;
    }

    HTTPS_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

static void _networkDisconnect(_httpsConnection_t* pHttpsConnection)
{
    IotNetworkError_t networkStatus = IOT_NETWORK_SUCCESS;

    networkStatus = pHttpsConnection->pNetworkInterface->close( pHttpsConnection->pNetworkConnection );
    if ( networkStatus != IOT_NETWORK_SUCCESS )
    {
        IotLogWarn("Failed to shutdown the socket with error code: %d", networkStatus );
    }
}

/*-----------------------------------------------------------*/

static void _networkDestroy(_httpsConnection_t* pHttpsConnection)
{
    IotNetworkError_t networkStatus = IOT_NETWORK_SUCCESS;

    networkStatus = pHttpsConnection->pNetworkInterface->destroy( pHttpsConnection->pNetworkConnection );
    if ( networkStatus != IOT_NETWORK_SUCCESS )
    {
        IotLogWarn("Failed to shutdown the socket with error code: %d", networkStatus );
    }
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _addHeader(_httpsRequest_t * pHttpsRequest, const char * pName, const char * pValue, uint32_t valueLen )
{
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

    int nameLen = strlen( pName ) ;
    int headerFieldSeparatorLen = HTTPS_HEADER_FIELD_SEPARATOR_LENGTH;
    uint32_t additionalLength = nameLen + headerFieldSeparatorLen + valueLen + HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;
    uint32_t possibleLastHeaderAdditionalLength = HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;

    /* Check if there is enough space to add the header field and value
    (name:value\r\n). We need to add a "\r\n" at the end of headers. The use of
    possibleLastHeaderAdditionalLength is to make sure that there is always
    space for the last "\r\n". */
    if( (additionalLength + possibleLastHeaderAdditionalLength + pHttpsRequest->pHeadersCur) > (pHttpsRequest->pHeadersEnd) )
    {
        IotLogError( "There is %d space left in the header buffer, but we want to add %d more of header.",
            pHttpsRequest->pHeadersEnd - pHttpsRequest->pHeadersCur,
            additionalLength + possibleLastHeaderAdditionalLength );
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INSUFFICIENT_MEMORY);
    }

    memcpy(pHttpsRequest->pHeadersCur, pName, nameLen);
    pHttpsRequest->pHeadersCur += nameLen;
    memcpy(pHttpsRequest->pHeadersCur, HTTPS_HEADER_FIELD_SEPARATOR, headerFieldSeparatorLen);
    pHttpsRequest->pHeadersCur += headerFieldSeparatorLen;
    memcpy(pHttpsRequest->pHeadersCur, pValue, valueLen);
    pHttpsRequest->pHeadersCur += valueLen;
    memcpy(pHttpsRequest->pHeadersCur, HTTPS_END_OF_HEADER_LINES_INDICATOR, HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH);
    pHttpsRequest->pHeadersCur += HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;
    IotLogDebug("Wrote header: \"%s: %.*s\r\n\". Space left in request user buffer: %d", 
        pName,
        valueLen,
        pValue,
        pHttpsRequest->pHeadersEnd - pHttpsRequest->pHeadersCur);

    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _networkSend(_httpsConnection_t* pHttpsConnection, uint8_t * pBuf, size_t len)
{
    HTTPS_FUNCTION_ENTRY(IOT_HTTPS_OK);

    size_t numBytesSent = 0;
    size_t numBytesSentTotal = 0;
    size_t sendLength = len;

    while( numBytesSentTotal < sendLength )
    {
        numBytesSent = pHttpsConnection->pNetworkInterface->send( pHttpsConnection->pNetworkConnection, 
            &( pBuf[numBytesSentTotal] ), 
            sendLength - numBytesSentTotal );

        /* pNetworkInterface->send returns 0 on error. */
        if( numBytesSent == 0 )
        {
            IotLogError("Error in sending the HTTPS headers. Error code: %d", numBytesSent);
            break;
        }

        numBytesSentTotal += numBytesSent;
    }

    if( numBytesSentTotal != sendLength )
    {
        IotLogError("Error sending data on the network. We sent %d but there were total %d.", numBytesSentTotal, sendLength);
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_NETWORK_ERROR);
    }

    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _networkRecv( _httpsConnection_t* pHttpsConnection, uint8_t * pBuf, size_t bufLen)
{
    HTTPS_FUNCTION_ENTRY(IOT_HTTPS_OK);

    size_t numBytesRecv = 0;

    numBytesRecv = pHttpsConnection->pNetworkInterface->receive( pHttpsConnection->pNetworkConnection,
        pBuf,
        bufLen);

    /* We return IOT_HTTPS_NETWORK_ERROR only if we receive nothing. Receiving less
       data than requested is okay because it is not known in advance how much data
       we are going to receive and therefore we request for the available buffer
       size. */
    if( numBytesRecv == 0)
    {
        IotLogError("Error in receiving the HTTPS response message. Socket Error code %d", numBytesRecv);
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_NETWORK_ERROR);
    }

    HTTPS_FUNCTION_EXIT_NO_CLEANUP(); 
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _sendHttpsHeaders( _httpsConnection_t* pHttpsConnection, uint8_t* pHeadersBuf, uint32_t headersLength, bool isNonPersistent, uint32_t contentLength)
{
    HTTPS_FUNCTION_ENTRY(IOT_HTTPS_OK);

    const char* connectionHeader = NULL;
    int numWritten = 0;
    int connectionHeaderLen = 0;
    /* The Content-Length header of the form "Content-Length: N\r\n" with a NULL terminator for snprintf. */
    char contentLengthHeaderStr[HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH + 1];
    /* The HTTP headers to send after the headers in pHeadersBuf are the Content-Length and the Connection type and
       the final "\r\n" to indicate the end of the the header lines. Note that we are using
       HTTPS_CONNECTION_KEEP_ALIVE_HEADER_LINE_LENGTH because length of "Connection: keep-alive\r\n" is
       more than "Connection: close\r\n". Creating a buffer of bigger size ensures that
       both the connection type strings will fit in the buffer. */
    char finalHeaders[HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH + HTTPS_CONNECTION_KEEP_ALIVE_HEADER_LINE_LENGTH + HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH] = { 0 };
    
    /* Send the headers passed into this function first. These headers are not termined with a second set of "\r\n". */
    status = _networkSend( pHttpsConnection, pHeadersBuf, headersLength);
    if( HTTPS_FAILED( status ) )
    {
        IotLogError("Error sending the HTTPS headers in the request user buffer. Error code: %d", status);
        HTTPS_GOTO_CLEANUP();
    }

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
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INTERNAL_ERROR);
    }

    /* snprintf() succeeded so copy that to the finalHeaders. */
    memcpy(finalHeaders, contentLengthHeaderStr, numWritten);
    /* Write the connection persistence type to the final headers. */
    if (isNonPersistent)
    {
        connectionHeader = HTTPS_CONNECTION_CLOSE_HEADER_LINE;
        connectionHeaderLen = FAST_MACRO_STRLEN(HTTPS_CONNECTION_CLOSE_HEADER_LINE);
    }
    else
    {
        connectionHeader = HTTPS_CONNECTION_KEEP_ALIVE_HEADER_LINE;
        connectionHeaderLen = FAST_MACRO_STRLEN(HTTPS_CONNECTION_KEEP_ALIVE_HEADER_LINE);
    }
    memcpy(&finalHeaders[numWritten], connectionHeader, connectionHeaderLen);
    numWritten += connectionHeaderLen;
    memcpy( &finalHeaders[numWritten], HTTPS_END_OF_HEADER_LINES_INDICATOR, HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH );
    numWritten += HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;

    status = _networkSend( pHttpsConnection, (uint8_t*)finalHeaders, numWritten);
    if( HTTPS_FAILED( status ) )
    {
        IotLogError("Error sending final HTTPS Headers \r\n%s. Error code: %d", finalHeaders, status);
        HTTPS_GOTO_CLEANUP();
    }

    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _sendHttpsBody( _httpsConnection_t* pHttpsConnection, uint8_t* pBodyBuf, uint32_t bodyLength)
{
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

    status = _networkSend( pHttpsConnection, pBodyBuf, bodyLength);
    if( HTTPS_FAILED( status ) )
    {
        IotLogError("Error sending final HTTPS body at location 0x%x. Error code: %d", pBodyBuf, status);
        HTTPS_GOTO_CLEANUP();
    }

    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _parseHttpsMessage(_httpParserInfo_t* pHttpParserInfo, char* pBuf, size_t len)
{
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

    size_t parsedBytes = 0;
    const char * pHttpParserErrorDescription = NULL;
    http_parser *pHttpParser = &(pHttpParserInfo->responseParser);

    IotLogDebug("Now parsing HTTP message buffer to process a response.");
    parsedBytes = pHttpParserInfo->parseFunc( pHttpParser, &_httpParserSettings, pBuf, len );
    IotLogDebug( "http-parser parsed %d bytes out of %d specified.", parsedBytes, len );

    /* If the parser fails with HPE_CLOSED_CONNECTION or HPE_INVALID_CONSTANT that simply means there
       was data beyond the end of the message. We do not fail in this case because we give the whole
       header buffer or body buffer to the parser even if it is only partly filled with data.
       Errors <= HPE_CB_chunk_complete means that a non-zero number was returned from some callback.
       A nonzero number is returned from some callbacks when we want to stop the parser early
       for example - a HEAD request or the user explicitly asked to ignore the body by not
       providing the body buffer. */
    if( (pHttpParser->http_errno != 0) &&
        ( HTTP_PARSER_ERRNO( pHttpParser ) != HPE_CLOSED_CONNECTION ) &&
        ( HTTP_PARSER_ERRNO( pHttpParser ) != HPE_INVALID_CONSTANT ) &&
        ( HTTP_PARSER_ERRNO( pHttpParser ) > HPE_CB_chunk_complete) )
    {
        pHttpParserErrorDescription = http_errno_description( HTTP_PARSER_ERRNO( pHttpParser ) );
        IotLogError("http_parser failed on the http response with error: %s", pHttpParserErrorDescription);
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_PARSING_ERROR);
    }

    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _receiveHttpsMessage( _httpsConnection_t* pHttpsConnection, 
    _httpParserInfo_t *pHttpParserInfo,
    IotHttpsResponseParserState_t *pCurrentParserState,
    IotHttpsResponseParserState_t finalParserState, 
    uint8_t** pBufCur,
    uint8_t** pBufEnd)
{    
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

    /* The final parser state is either the end of the header lines or the end of the entity body. This state is set in
       the http-parser callsback. */
    while( (*pCurrentParserState < finalParserState) && (*pBufEnd - *pBufCur > 0) ) 
    {
        status = _networkRecv( pHttpsConnection,
            *pBufCur, 
            *pBufEnd - *pBufCur);

        /* A network error in _networkRecv is returned only when we received zero bytes. In that case, there is 
           no point in parsing we return immediately with the network error. */
        if( HTTPS_FAILED( status ) )
        {
            IotLogError( "Network error receiving the HTTPS response headers. Error code: %d", status );
            break;
        }

        status = _parseHttpsMessage(pHttpParserInfo, (char*)(*pBufCur), *pBufEnd - *pBufCur);
        if(HTTPS_FAILED(status))
        {
            IotLogError("Failed to parse the message buffer with error: %d", pHttpParserInfo->responseParser.http_errno);
            break;
        }

        /* The _httResponse->pHeadersCur pointer is updated in the http_parser callbacks. */
        IotLogDebug( "There is %d of space left in the buffer.", *pBufEnd - *pBufCur );

    }

    /* If we did not reach the end of the headers or body in the parser callbacks, then the buffer configured does not
       fit all of that part of the HTTP message. */
    if( *pCurrentParserState < finalParserState)
    {
        IotLogDebug("There are still more data on the network. It could not fit into the specified length %d.",
            *pBufEnd - *pBufCur);
    }

    HTTPS_GOTO_CLEANUP();
    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _receiveHttpsHeaders( _httpsConnection_t* pHttpsConnection, _httpsResponse_t* pHttpsResponse)
{
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

    status = _receiveHttpsMessage(pHttpsConnection,
        &(pHttpsResponse->httpParserInfo),
        &(pHttpsResponse->parserState),
        PARSER_STATE_HEADERS_COMPLETE,
        &(pHttpsResponse->pHeadersCur),
        &(pHttpsResponse->pHeadersEnd));

    if( HTTPS_FAILED(status))
    {
        IotLogError("Error receiving the HTTP headers. Error code %d", status);
        HTTPS_GOTO_CLEANUP();
    }

    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

/* _receiveHttpsHeaders() must be called first before this function is called. */
static IotHttpsReturnCode_t _receiveHttpsBody( _httpsConnection_t* pHttpsConnection, 
                                               _httpsResponse_t* pHttpsResponse)
{
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

    pHttpsResponse->bufferProcessingState = PROCESSING_STATE_FILLING_BODY_BUFFER;

    status = _receiveHttpsMessage(pHttpsConnection,
        &(pHttpsResponse->httpParserInfo),
        &(pHttpsResponse->parserState),
        PARSER_STATE_BODY_COMPLETE,
        &(pHttpsResponse->pBodyCur),
        &(pHttpsResponse->pBodyEnd));
    if( HTTPS_FAILED(status) )
    {
        IotLogError("Error receiving the HTTP body. Error code %d", status);
        HTTPS_GOTO_CLEANUP();
    }

    HTTPS_FUNCTION_CLEANUP_BEGIN();

    IotLogDebug("The remaining content length on the network is %d.",
        pHttpsResponse->httpParserInfo.responseParser.content_length);

    HTTPS_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _flushHttpsNetworkData( _httpsConnection_t* pHttpsConnection, _httpsResponse_t* pHttpsResponse )
{
    HTTPS_FUNCTION_ENTRY(IOT_HTTPS_OK);

    static uint8_t flushBuffer[IOT_HTTPS_MAX_FLUSH_BUFFER_SIZE] = { 0 };
    const char * pHttpParserErrorDescription = NULL;
    IotHttpsReturnCode_t parserStatus = IOT_HTTPS_OK;
    IotHttpsReturnCode_t networkStatus = IOT_HTTPS_OK;

    /* Even if there is not body, the parser state will become body complete after the headers finish. */
    while( pHttpsResponse->parserState < PARSER_STATE_BODY_COMPLETE )
    {
        IotLogDebug( "Now clearing the rest of the response data on the socket. " );
        networkStatus = _networkRecv( pHttpsConnection, flushBuffer, IOT_HTTPS_MAX_FLUSH_BUFFER_SIZE );

        /* Run this through the parser so that we can get the end of the HTTP message, instead of simply timing out the socket to stop.
           If we relied on the socket timeout to stop reading the network socket, then the server may close the connection. */
        parserStatus = _parseHttpsMessage(&(pHttpsResponse->httpParserInfo), (char*)flushBuffer, IOT_HTTPS_MAX_FLUSH_BUFFER_SIZE );
        if(HTTPS_FAILED(parserStatus))
        {
            pHttpParserErrorDescription = http_errno_description( HTTP_PARSER_ERRNO( &pHttpsResponse->httpParserInfo.responseParser ) );
            IotLogError("Network Flush: Failed to parse the response body buffer with error: %d", pHttpsResponse->httpParserInfo.responseParser.http_errno);
            break;
        }

        /* If there is a network error then we want to stop clearing out the buffer. */
        if( HTTPS_FAILED(networkStatus) )
        {
            IotLogWarn( "Network Flush: Error receiving the rest of the HTTP response. Error code: %d", 
                networkStatus );
            break;
        }
    }

    /* All network errors except timeouts are returned. */
    if(HTTPS_FAILED(networkStatus))
    {
        status = networkStatus;
    }
    else
    {
        status = parserStatus;
    }
    HTTPS_GOTO_CLEANUP();

    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

static void _sendHttpsRequest( IotTaskPool_t pTaskPool, IotTaskPoolJob_t pJob, void * pUserContext )
{
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

    _httpsRequest_t* pHttpsRequest = (_httpsRequest_t*)( pUserContext );
    _httpsConnection_t* pHttpsConnection = pHttpsRequest->pHttpsConnection;
    _httpsResponse_t* pHttpsResponse = pHttpsRequest->pHttpsResponse;

    IotLogDebug( "Task with request ID: %d started.", pHttpsRequest );

    if(pHttpsRequest->cancelled == true)
    {
        IotLogDebug("Request ID: %d was cancelled.", pHttpsRequest );
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_ASYNC_CANCELLED);
    }

    /* To protect against out of order network data from a rouge server signal that the request is
       not finished sending. */
    pHttpsRequest->reqFinishedSending = false;

    /* Set the request associated with this response so that the network receive task can check if the request
       has finished sending and invoke callbacks. */
    pHttpsResponse->pHttpsRequest = pHttpsRequest;

    /* Queue the response to expect from the network. */
    IotMutex_Lock(&(pHttpsConnection->respQMutex));
    IotDeQueue_EnqueueTail(&(pHttpsConnection->respQ), &(pHttpsResponse->link));
    IotMutex_Unlock(&(pHttpsConnection->respQMutex));

    /* Get the headers from the application. For a synchronous request we should have appended extra headers before
       we got to this point. */
    if(pHttpsResponse->isAsync && pHttpsRequest->pCallbacks->appendHeaderCallback)
    {
        pHttpsRequest->pCallbacks->appendHeaderCallback(pHttpsRequest->pUserPrivData, pHttpsRequest);
    }

    if(pHttpsRequest->cancelled == true)
    {
        IotLogDebug("Request ID: %d was cancelled.", pHttpsRequest );
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_ASYNC_CANCELLED);
    }

    /* Ask the user for data to write body to the network. We only ask the user once. This is so that
       we can calculate the Content-Length to send.*/
    if(pHttpsResponse->isAsync && pHttpsRequest->pCallbacks->writeCallback)
    {
        /* If there is data, then a Content-Length header value will be provided and we send the headers
           before that user data. */
        pHttpsRequest->pCallbacks->writeCallback(pHttpsRequest->pUserPrivData, pHttpsRequest);
    }

    /* Send the HTTP headers. */
    status = _sendHttpsHeaders(pHttpsConnection,
        pHttpsRequest->pHeaders,
        pHttpsRequest->pHeadersCur - pHttpsRequest->pHeaders,
        pHttpsRequest->isNonPersistent,
        pHttpsRequest->bodyLength);
    if( HTTPS_FAILED(status) )
    {
        IotLogError("Error sending the HTTPS headers with error code: %d", status);
        IOT_SET_AND_GOTO_CLEANUP(status);
    }

    if((pHttpsRequest->pBody != NULL) && (pHttpsRequest->bodyLength > 0))
    {
        status = _sendHttpsBody( pHttpsConnection, pHttpsRequest->pBody, pHttpsRequest->bodyLength);
        if( HTTPS_FAILED(status) )
        {
            IotLogError("Error sending final HTTPS body. Return code: %d", status);
            HTTPS_GOTO_CLEANUP(); 
        }
    }

    if(pHttpsRequest->cancelled == true)
    {
        IotLogDebug("Request ID: %d was cancelled.", pHttpsRequest );
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_ASYNC_CANCELLED);
    }

    HTTPS_FUNCTION_CLEANUP_BEGIN();

    if( HTTPS_FAILED(status) )
    {
        /* If the headers or body failed to send, then there should be no response expected from the server. */
        _cancelResponse(pHttpsResponse);
        IotMutex_Lock(&(pHttpsConnection->respQMutex));
        IotDeQueue_Remove(&(pHttpsResponse->link));
        IotMutex_Unlock(&(pHttpsConnection->respQMutex));

        /* Set the error status in the sync workflow. */
        pHttpsResponse->syncStatus = status;

        /* Post to the response finished semaphore to unlock the application waiting on a synchronous request. */
        if(pHttpsRequest->isAsync == false)
        {
            IotSemaphore_Post( &(pHttpsResponse->respFinishedSem));
        }

        /* Return the error status or cancel status to the application for an asynchronous workflow. */
        if(pHttpsRequest->isAsync && pHttpsRequest->pCallbacks->errorCallback)
        {
            pHttpsRequest->pCallbacks->errorCallback( pHttpsRequest->pUserPrivData, pHttpsRequest, status );
        }
        
    }
    else
    {
        /* The request has finished sending. */
        pHttpsRequest->reqFinishedSending = true;
    }
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t _scheduleHttpsRequestSend(_httpsRequest_t* pHttpsRequest)
{
    HTTPS_FUNCTION_ENTRY(IOT_HTTPS_OK);

    IotTaskPoolError_t taskPoolStatus = IOT_TASKPOOL_SUCCESS;
    _httpsConnection_t* pHttpsConnection = pHttpsRequest->pHttpsConnection;

    taskPoolStatus = IotTaskPool_CreateJob( _sendHttpsRequest,
        ( void* )(pHttpsRequest),
        &( pHttpsConnection->taskPoolJobStorage ),
        &( pHttpsConnection->taskPoolJob ) );

    /* Creating a task pool job should never fail when parameters are valid. */
    if( taskPoolStatus != IOT_TASKPOOL_SUCCESS )
    {
        IotLogError( "Error creating a taskpool job for request servicing. Error code: %d", taskPoolStatus );
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INTERNAL_ERROR);
    }

    taskPoolStatus = IotTaskPool_Schedule( IOT_SYSTEM_TASKPOOL, pHttpsConnection->taskPoolJob, 0);
    if( taskPoolStatus != IOT_TASKPOOL_SUCCESS )
    {
        IotLogError( "Failed to schedule taskpool job. Error code: %d", taskPoolStatus );
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_ASYNC_SCHEDULING_ERROR);
    }

    HTTPS_FUNCTION_EXIT_NO_CLEANUP();  
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t _addRequestToConnectionReqQ(_httpsRequest_t* pHttpsRequest)
{
    HTTPS_FUNCTION_ENTRY(IOT_HTTPS_OK);

    _httpsConnection_t* pHttpsConnection = pHttpsRequest->pHttpsConnection;
    bool isOnlyRequest = false;

    /* Place the request into the queue. */
    IotMutex_Lock( &( pHttpsConnection->reqQMutex ) );

    /* If this is the first and only item in the list, then we will want to schedule a new task to service this 
        request. If this is the first and only item in the list, then there is no task currently sending a request 
        and there is no response currently being received. */
    if(IotDeQueue_IsEmpty(&( pHttpsConnection->reqQ )))
    {
        isOnlyRequest = true;
    }

    IotDeQueue_EnqueueTail(&( pHttpsConnection->reqQ ), &(pHttpsRequest->link));

    IotMutex_Unlock( &(pHttpsConnection->reqQMutex) );

    if(isOnlyRequest)
    {
        status = _scheduleHttpsRequestSend(pHttpsRequest);
        if( HTTPS_FAILED(status) )
        {
            IotLogError( "Failed to schedule the only request in the queue for request %d. Error code: %d", pHttpsRequest, status );

            /* If we fail to schedule the only request in the queue we should remove it. */
            IotMutex_Lock( &( pHttpsConnection->reqQMutex ) );
            IotDeQueue_Remove( &(pHttpsRequest->link) );
            IotMutex_Unlock( &(pHttpsConnection->reqQMutex) );

            HTTPS_GOTO_CLEANUP();
        }
    }

    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

static void _cancelRequest(_httpsRequest_t *pHttpsRequest)
{
    pHttpsRequest->cancelled = true;
    if(pHttpsRequest->pHttpsResponse != NULL)
    {
        pHttpsRequest->pHttpsResponse->cancelled = true;
    }
}

/*-----------------------------------------------------------*/

static void _cancelResponse(_httpsResponse_t *pHttpsResponse)
{
    pHttpsResponse->cancelled = true;
    if(pHttpsResponse->pHttpsRequest != NULL)
    {
        pHttpsResponse->pHttpsRequest->cancelled = true;
    }
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_Init( void )
{
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

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
/* This code prints debugging information and is, therefore, compiled only when
 * log level is set to IOT_LOG_DEBUG. */
#if (LIBRARY_LOG_LEVEL == IOT_LOG_DEBUG)
    _httpParserSettings.on_chunk_header = _httpParserOnChunkHeaderCallback;
    _httpParserSettings.on_chunk_complete = _httpParserOnChunkCompleteCallback;
#endif
    HTTPS_GOTO_CLEANUP();
    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

static IotHttpsReturnCode_t _initializeResponse( IotHttpsResponseHandle_t* pRespHandle, IotHttpsResponseInfo_t* pRespInfo, bool isAsync, IotHttpsMethod_t method )
{
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

    _httpsResponse_t* pHttpsResponse = NULL;

    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(pRespInfo->userBuffer.pBuffer);

    /* Check of the user buffer is large enough for the response context + default headers. */
    HTTPS_ON_ARG_ERROR_MSG_GOTO_CLEANUP( pRespInfo->userBuffer.bufferLen >= responseUserBufferMinimumSize,
        IOT_HTTPS_INSUFFICIENT_MEMORY,
        "Buffer size is too small to initialize the response context. User buffer size: %d, required minimum size; %d.",
        pRespInfo->userBuffer.bufferLen, 
        responseUserBufferMinimumSize);

    /* Initialize the corresponding response to this request. */
    pHttpsResponse = (_httpsResponse_t *)(pRespInfo->userBuffer.pBuffer);

    /* Clear out the response user buffer. This is important because we
    give the whole buffer to the parser as opposed to the actual content
    length and rely on the parser to stop when a complete HTTP response
    is found. To make sure that any data in the buffer which is not part
    of the received HTTP response, does not get interpreted as part of
    the HTTP repose, we zero out the buffer here. */
    memset(pRespInfo->userBuffer.pBuffer, 0, pRespInfo->userBuffer.bufferLen);
        
    pHttpsResponse->pHeaders = (uint8_t*)(pHttpsResponse) + sizeof(_httpsResponse_t);
    pHttpsResponse->pHeadersEnd = (uint8_t*)(pHttpsResponse) + pRespInfo->userBuffer.bufferLen;
    pHttpsResponse->pHeadersCur = pHttpsResponse->pHeaders;

    if( isAsync )
    {
        pHttpsResponse->isAsync = true;
        /* For an asynchronous request the response body is provided by the application in the 
           IotHttpsCallbacks_t.readReadyCallback(). These pointers will be updated when IotHttpsClient_ReadResponseBody()
           is invoked. */
        pHttpsResponse->pBody = NULL;
        pHttpsResponse->pBodyCur = NULL;
        pHttpsResponse->pBodyEnd = NULL;
    }
    else
    {
        pHttpsResponse->isAsync = false;
        /* The request body pointer is allowed to be NULL. pSyncInfo was checked for NULL earlier in this function. */
        pHttpsResponse->pBody = pRespInfo->pSyncInfo->pBody;
        pHttpsResponse->pBodyCur = pHttpsResponse->pBody;
        pHttpsResponse->pBodyEnd = pHttpsResponse->pBody + pRespInfo->pSyncInfo->bodyLen;
        /* Clear out the body bufffer. This is important because we give the
        whole buffer to the parser as opposed to the actual content length and
        rely on the parser to stop when a complete HTTP response is found. To
        make sure that any data in the buffer which is not part of the received
        HTTP response, does not get interpreted as part of the HTTP repose, we
        zero out the buffer here. */
        memset(pRespInfo->pSyncInfo->pBody, 0, pRespInfo->pSyncInfo->bodyLen);
    }
    
    /* Reinitialize the parser and set the fill buffer state to empty. This does not return any errors. */
    http_parser_init(&(pHttpsResponse->httpParserInfo.responseParser), HTTP_RESPONSE);
    http_parser_init(&(pHttpsResponse->httpParserInfo.readHeaderParser), HTTP_RESPONSE);
    /* Set the third party http parser function. */
    pHttpsResponse->httpParserInfo.parseFunc = http_parser_execute;

    pHttpsResponse->status = 0;
    pHttpsResponse->method = method;
    pHttpsResponse->parserState = PARSER_STATE_NONE;
    pHttpsResponse->bufferProcessingState = PROCESSING_STATE_NONE;
    pHttpsResponse->pReadHeaderField = NULL;
    pHttpsResponse->pReadHeaderValue = NULL;
    pHttpsResponse->readHeaderValueLength = 0;
    pHttpsResponse->foundHeaderField = 0;
    pHttpsResponse->pHttpsConnection = NULL;
    
    pHttpsResponse->pBodyInHeaderBuf = NULL;
    pHttpsResponse->pBodyCurInHeaderBuf = NULL;
    pHttpsResponse->bodyRxStatus = IOT_HTTPS_OK;
    pHttpsResponse->cancelled = false;
    pHttpsResponse->syncStatus = IOT_HTTPS_OK;

    /* Set the response handle to return. */
    *pRespHandle  = pHttpsResponse;

    HTTPS_FUNCTION_CLEANUP_BEGIN();
    if(HTTPS_FAILED(status))
    {
        pRespHandle = NULL;
    }
    HTTPS_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

void IotHttpsClient_Deinit( void )
{
    /* There is nothing to de-initialize here as of now. */
}

/* --------------------------------------------------------- */

IotHttpsReturnCode_t IotHttpsClient_Connect(IotHttpsConnectionHandle_t * pConnHandle, IotHttpsConnectionInfo_t *pConnInfo)
{
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

    /* Check for NULL parameters in a public API. */
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(pConnHandle);
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(pConnInfo);

    /* If a valid connection handle is passed in. */
    if(*pConnHandle != NULL)
    {
        /* If the handle in a connected state, then we want to disconnect before reconnecting. The ONLY way to put the 
           handle is a disconnect state is to call IotHttpsClient_Disconnect(). */
        if((*pConnHandle)->isConnected)
        {
            status = IotHttpsClient_Disconnect(*pConnHandle);
            if(HTTPS_FAILED(status))
            {
                IotLogError("Error disconnecting a connected *pConnHandle passed to IotHttpsClient_Connect().Error code %d", status);
                *pConnHandle = NULL;
                HTTPS_GOTO_CLEANUP();
            }
        }
    }

    /* Connect to the server now. Initialize all resources needed for the connection context as well here. */
    status = _createHttpsConnection(pConnHandle, pConnInfo);
    if(HTTPS_FAILED(status))
    {
        IotLogError("Error in IotHttpsClient_Connect(). Error code %d.", status);
    }

    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_Disconnect(IotHttpsConnectionHandle_t connHandle)
{
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

    _httpsRequest_t* pHttpsRequest = NULL;
    IotLink_t* pItem = NULL;

    HTTPS_ON_NULL_ARG_GOTO_CLEANUP( connHandle );

    /* Do not attempt to disconnect an already disconnected connection.
    It can happen when a user calls this functions and we return
    IOT_HTTPS_BUSY. */
    if( connHandle->isConnected )
    {
        /* Mark the network as disconnected whether the disconnect passes or not. */
        connHandle->isConnected = false;
        
        /* Disconnect from the network. */
        _networkDisconnect(connHandle);
    }

    /* If there is a request in the connection's request queue and it has not finished sending, then we cannot
       destroy the connection until it finishes. */
    IotMutex_Lock(&(connHandle->reqQMutex));
    pItem = IotDeQueue_DequeueHead(&(connHandle->reqQ));
    if(pItem != NULL)
    {
        pHttpsRequest = IotLink_Container(_httpsRequest_t, pItem, link);
        if(pHttpsRequest->reqFinishedSending == false)
        {
            IotLogError("Connection is in use. Disconnected, but cannot destroy the connection.");
            status = IOT_HTTPS_BUSY;
            /* Attempt to cancel the busy request. */
            _cancelRequest(pHttpsRequest);
            /* We set the status as busy, but we do not goto the cleanup right away because we still want to remove 
               all pending requests. */
        }

        /* Delete all of the other possible pending requests on the connection. */
        IotDeQueue_RemoveAll(&(connHandle->reqQ), NULL, 0);

        /* Put the request that was dequeued back so that the application can call this function again to check later. 
          If during the last check and this check reqFinishedSending gets set to true, that is OK because then the next
          time this function is called, there will be no requests in the reqQ. */
        if(pHttpsRequest->reqFinishedSending == false)
        {
            IotDeQueue_EnqueueHead(&(connHandle->reqQ), pItem);
        }
    }
    IotMutex_Unlock(&(connHandle->reqQMutex));

    /* Remove all pending responses. The network receive callback dequeues a response then proceeds to use the network 
       interface to receive the response. If this routine is called from the application context and there is a 
       network receive callback in process, this routine will wait in _networkDestroy until that routine returns. 
       If this is routine is called from the network receive callback context, then the destroy happens after the 
       network receive callback context returns. */
    IotMutex_Lock(&(connHandle->respQMutex));
    IotDeQueue_RemoveAll(&(connHandle->respQ), NULL, 0);
    IotMutex_Unlock(&(connHandle->reqQMutex));

    /* Destroy the network connection (cleaning up network socket resources). */
    if( HTTPS_SUCCEEDED(status))
    {
        _networkDestroy(connHandle);
    }

    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_InitializeRequest(IotHttpsRequestHandle_t * pReqHandle, IotHttpsRequestInfo_t *pReqInfo)
{
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

    _httpsRequest_t * pHttpsRequest = NULL;
    size_t additionalLength = 0;
    size_t spaceLen = 1;
    char* pSpace = " ";
    size_t httpsMethodLen = 0;
    size_t httpsProtocolVersionLen = FAST_MACRO_STRLEN(HTTPS_PROTOCOL_VERSION);

    /* Check for NULL parameters in the public API. */
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP( pReqHandle );
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP( pReqInfo );
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP( pReqInfo->userBuffer.pBuffer );
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP( pReqInfo->pHost );
    if(pReqInfo->isAsync)
    {
        HTTPS_ON_NULL_ARG_GOTO_CLEANUP( pReqInfo->pAsyncInfo );
    }
    else
    {
        HTTPS_ON_NULL_ARG_GOTO_CLEANUP( pReqInfo->pSyncInfo );
    }
    

    /* Check of the user buffer is large enough for the request context + default headers. */
    HTTPS_ON_ARG_ERROR_MSG_GOTO_CLEANUP( pReqInfo->userBuffer.bufferLen >= requestUserBufferMinimumSize,
        IOT_HTTPS_INSUFFICIENT_MEMORY,
        "Buffer size is too small to initialize the request context. User buffer size: %d, required minimum size; %d.",
        pReqInfo->userBuffer.bufferLen,
        requestUserBufferMinimumSize);

    /* Set the request contet to the start of the userbuffer. */
    pHttpsRequest = ( _httpsRequest_t *)(pReqInfo->userBuffer.pBuffer);
    /* Clear out the user buffer. */
    memset(pReqInfo->userBuffer.pBuffer, 0, pReqInfo->userBuffer.bufferLen);

    /* Set the start of the headers to the end of the request context in the user buffer. */
    pHttpsRequest->pHeaders = (uint8_t*)pHttpsRequest + sizeof( _httpsRequest_t );
    pHttpsRequest->pHeadersEnd = (uint8_t*)pHttpsRequest + pReqInfo->userBuffer.bufferLen;
    pHttpsRequest->pHeadersCur = pHttpsRequest->pHeaders;

    /* Get the length of the HTTP method. */
    httpsMethodLen = strlen( _pHttpsMethodStrings[pReqInfo->method] );

    /* Add the request line to the header buffer. */
    additionalLength = httpsMethodLen + \
        spaceLen + \
        pReqInfo->pathLen + \
        spaceLen + \
        httpsProtocolVersionLen + \
        HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;
    if( (additionalLength + pHttpsRequest->pHeadersCur) > (pHttpsRequest->pHeadersEnd ))
    {
        IotLogError("Request line does not fit into the request user buffer: \"%s %.*s HTTP/1.1\\r\\n\" . ",
            _pHttpsMethodStrings[IOT_HTTPS_METHOD_GET],
            pReqInfo->pathLen,
            pReqInfo->pPath);
        IotLogError( "The length needed is %d and the space available is %d.", additionalLength, pHttpsRequest->pHeadersEnd - pHttpsRequest->pHeadersCur );
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INSUFFICIENT_MEMORY);
    }

    /* Write "<METHOD> <PATH> HTTP/1.1\r\n" to the start of the header space. */
    memcpy(pHttpsRequest->pHeadersCur, _pHttpsMethodStrings[pReqInfo->method], httpsMethodLen);
    pHttpsRequest->pHeadersCur += httpsMethodLen;
    memcpy(pHttpsRequest->pHeadersCur, pSpace, spaceLen);
    pHttpsRequest->pHeadersCur += spaceLen;
    if(pReqInfo->pPath == NULL)
    {
        pReqInfo->pPath = HTTPS_EMPTY_PATH;
        pReqInfo->pathLen = FAST_MACRO_STRLEN(HTTPS_EMPTY_PATH);
    }
    memcpy(pHttpsRequest->pHeadersCur, pReqInfo->pPath, pReqInfo->pathLen);
    pHttpsRequest->pHeadersCur += pReqInfo->pathLen;
    memcpy( pHttpsRequest->pHeadersCur, pSpace, spaceLen );
    pHttpsRequest->pHeadersCur += spaceLen;
    memcpy(pHttpsRequest->pHeadersCur, HTTPS_PROTOCOL_VERSION, httpsProtocolVersionLen);
    pHttpsRequest->pHeadersCur += httpsProtocolVersionLen;
    memcpy(pHttpsRequest->pHeadersCur, HTTPS_END_OF_HEADER_LINES_INDICATOR, HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH);
    pHttpsRequest->pHeadersCur += HTTPS_END_OF_HEADER_LINES_INDICATOR_LENGTH;

    /* Add the User-Agent header. */
    status = _addHeader(pHttpsRequest, "User-Agent", IOT_HTTPS_USER_AGENT, FAST_MACRO_STRLEN( IOT_HTTPS_USER_AGENT ));
    if( HTTPS_FAILED(status) )
    {
        IotLogError("Failed to write header to the request user buffer: \"User-Agent: %s\\r\\n\" . Error code: %d", 
            IOT_HTTPS_USER_AGENT, 
            status);
        HTTPS_GOTO_CLEANUP();
    }

    status = _addHeader(pHttpsRequest, "Host", pReqInfo->pHost, pReqInfo->hostLen);
    if( HTTPS_FAILED(status) )
    {
        IotLogError("Failed to write \"Host: %.*s\\r\\n\" to the request user buffer. Error code: %d", 
            pReqInfo->hostLen, 
            pReqInfo->pHost, 
            status);
        HTTPS_GOTO_CLEANUP();
    }

    if(pReqInfo->isAsync)
    {
        pHttpsRequest->isAsync = true;
        /* If this is an asynchronous request then save the callbacks to use. */
        pHttpsRequest->pCallbacks = &(pReqInfo->pAsyncInfo->callbacks);
        pHttpsRequest->pUserPrivData = pReqInfo->pAsyncInfo->pPrivData;
        /* The body pointer and body length will be filled in when the application sends data in the writeCallback. */
        pHttpsRequest->pBody = NULL;
        pHttpsRequest->bodyLength = 0;
    }
    else
    {
        pHttpsRequest->isAsync = false;
        /* Set the HTTP request entity body. This is allowed to be NULL for no body like for a GET request. */
        pHttpsRequest->pBody = pReqInfo->pSyncInfo->pBody;
        pHttpsRequest->bodyLength = pReqInfo->pSyncInfo->bodyLen;
    }

    /* Save the method of this request. */
    pHttpsRequest->method = pReqInfo->method;
    /* Set the connection persistence flag for keeping the connection open after receiving a response. */
    pHttpsRequest->isNonPersistent = pReqInfo->isNonPersistent;
    /* Initialize the request to not finished sending. */
    pHttpsRequest->reqFinishedSending = false;
    /* Initialize the request cancellation. */
    pHttpsRequest->cancelled = false;

    /* Set the request handle to return. */
    *pReqHandle = pHttpsRequest;

    HTTPS_FUNCTION_CLEANUP_BEGIN();
    if(HTTPS_FAILED(status) && (pReqHandle != NULL))
    {
        /* Set the request handle to return to NULL, if we failed anywhere. */
        *pReqHandle = NULL;
    }
    HTTPS_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_AddHeader( IotHttpsRequestHandle_t reqHandle, char * pName, char * pValue, uint32_t len )
{
    HTTPS_FUNCTION_ENTRY(IOT_HTTPS_OK);

    /* Check for NULL pointer paramters. */
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP( pName );
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP( pValue );
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP( reqHandle );

    /* Check for auto-generated header "Content-Length". This header is created and send automatically when right before
       request body is sent on the network. */
    HTTPS_ON_ARG_ERROR_MSG_GOTO_CLEANUP(strncmp(pName, HTTPS_CONTENT_LENGTH_HEADER, FAST_MACRO_STRLEN(HTTPS_CONTENT_LENGTH_HEADER)) != 0,
        IOT_HTTPS_INVALID_PARAMETER,
        "Attempting to add auto-generated header %s. This is not allowed.",
        HTTPS_CONTENT_LENGTH_HEADER);

    /* Check for auto-generated header "Connection". This header is created and send automatically when right before
       request body is sent on the network. */
    HTTPS_ON_ARG_ERROR_MSG_GOTO_CLEANUP(strncmp(pName, HTTPS_CONNECTION_HEADER, FAST_MACRO_STRLEN(HTTPS_CONNECTION_HEADER)) != 0,
        IOT_HTTPS_INVALID_PARAMETER,
        "Attempting to add auto-generated header %s. This is not allowed.", 
        HTTPS_CONNECTION_HEADER);

        /* Check for auto-generated header "Host". This header is created and placed into the header buffer space
        in IotHttpClient_InitializeRequest(). */
    HTTPS_ON_ARG_ERROR_MSG_GOTO_CLEANUP(strncmp(pName, HTTPS_HOST_HEADER, FAST_MACRO_STRLEN(HTTPS_HOST_HEADER)) != 0,
        IOT_HTTPS_INVALID_PARAMETER,
        "Attempting to add auto-generated header %s. This is not allowed.", 
        HTTPS_HOST_HEADER);

        /* Check for auto-generated header "User-Agent". This header is created and placed into the header buffer space
        in IotHttpClient_InitializeRequest(). */
    HTTPS_ON_ARG_ERROR_MSG_GOTO_CLEANUP(strncmp(pName, HTTPS_USER_AGENT_HEADER, FAST_MACRO_STRLEN(HTTPS_USER_AGENT_HEADER)) != 0,
        IOT_HTTPS_INVALID_PARAMETER,
        "Attempting to add auto-generated header %s. This is not allowed.", 
        HTTPS_USER_AGENT_HEADER);


    status = _addHeader(reqHandle, pName, pValue, len );
    if(HTTPS_FAILED(status))
    {
        IotLogError("Error in IotHttpsClient_AddHeader(), error code %d.", status);
        HTTPS_GOTO_CLEANUP();
    }

    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_SendSync(IotHttpsConnectionHandle_t connHandle, 
                                             IotHttpsRequestHandle_t reqHandle, 
                                             IotHttpsResponseHandle_t * pRespHandle, 
                                             IotHttpsResponseInfo_t *pRespInfo,
                                             uint32_t timeoutMs)
{

    HTTPS_FUNCTION_ENTRY(IOT_HTTPS_OK);

    bool respFinishedSemCreated = false;
    _httpsResponse_t* pHttpsResponse = NULL;

    /* Parameter checks. */
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(connHandle);
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(reqHandle);
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(pRespHandle);
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(pRespInfo);

    /* If an asynchronous request/response is configured, that is invalid for this API. */
    if(reqHandle->isAsync)
    {
        IotLogError("Called IotHttpsClient_SendSync on an asynchronous configured request.");
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INVALID_PARAMETER);
    }

    /* Initialize the response handle to return. */
    status = _initializeResponse(pRespHandle, pRespInfo, false /* Not async */, reqHandle->method);
    if(HTTPS_FAILED(status))
    {
        IotLogError("Failed to initialize the response on the synchronous request %d.", reqHandle);
        HTTPS_GOTO_CLEANUP();
    }

    /* Set the internal response to use. */
    pHttpsResponse = *pRespHandle;
    
    /* The implicit connection passed and we need to the set the connection handle in the request and response. */
    reqHandle->pHttpsConnection = connHandle;
    pHttpsResponse->pHttpsConnection = connHandle;

    /* Create the semaphore used to wait on the response to finish being received. */
    respFinishedSemCreated = IotSemaphore_Create( &( pHttpsResponse->respFinishedSem ), 0 /* initialValue */, 1 /* maxValue */ );
    if( respFinishedSemCreated == false  )
    {
        IotLogError("Failed to create an internal semaphore.");
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_INTERNAL_ERROR);
    }

    /* Associate the response to the request so that we can schedule it to be received when the request gets scheduled to send. */
    reqHandle->pHttpsResponse = pHttpsResponse;

    /* Schedule this request to be sent by adding it to the connection's request queue. */
    status = _addRequestToConnectionReqQ(reqHandle);

    if(HTTPS_FAILED(status))
    {
        IotLogError("Failed to schedule the synchronous request. Error code: %d", status);
        HTTPS_GOTO_CLEANUP();
    }

    /* Wait for the request to finish. */
    if( timeoutMs == 0)
    {
        IotSemaphore_Wait(&(pHttpsResponse->respFinishedSem));
    }
    else
    {
        if( IotSemaphore_TimedWait( &(pHttpsResponse->respFinishedSem), timeoutMs ) == false )
        {
            IotLogError("Timed out waiting for the synchronous request to finish. Timeout ms: %d", timeoutMs);
            _cancelRequest(reqHandle);
            HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_BUSY);
        }
    }

    HTTPS_FUNCTION_CLEANUP_BEGIN();
    if(respFinishedSemCreated)
    {
        IotSemaphore_Destroy(&(pHttpsResponse->respFinishedSem));
    }

    /* If the syncStatus is anything other than IOT_HTTPS_OK, then the request was scheduled. */
    if( (pHttpsResponse != NULL) && HTTPS_FAILED( pHttpsResponse->syncStatus ) )
    {
        status = pHttpsResponse->syncStatus;
    }

    if(HTTPS_FAILED( status ))
    {
        if( pRespHandle != NULL )
        {
            *pRespHandle = NULL;
        }
        IotLogError("IotHttpsClient_SendSync() failed.");
    }

    HTTPS_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_WriteRequestBody(IotHttpsRequestHandle_t reqHandle, char *pBuf, uint32_t len, int isComplete)
{
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(reqHandle);
    /* This function is not valid for a synchronous response. Applications need to configure the request body in 
       IotHttpsRequestInfo_t.pSyncInfo_t.reqData before calling IotHttpsClient_SendSync(). */
    HTTPS_ON_ARG_ERROR_GOTO_CLEANUP(reqHandle->isAsync);
    HTTPS_ON_ARG_ERROR_MSG_GOTO_CLEANUP(isComplete == 1,
        IOT_HTTPS_NOT_SUPPORTED,
        "isComplete must be 1 in IotHttpsClient_WriteRequestBody() for the current version of the HTTPS Client library.");

    /* If the bodyLength is greater than 0, then we already called this function and we need to enforce that 
        function must only be called once. We only call this function once so that we can calculate the Content-Length
        easily. */
    if(reqHandle->bodyLength > 0)
    {
        IotLogError("Error this function must be called once with the data needed to send. Variable length HTTP "
            "request body is not supported in this library.");
        HTTPS_SET_AND_GOTO_CLEANUP(IOT_HTTPS_MESSAGE_FINISHED);
    }

    /* Set the pointer to the body and the length for the content-length calculation. */
    reqHandle->pBody = pBuf;
    reqHandle->bodyLength = len;

    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_ReadResponseBody(IotHttpsResponseHandle_t respHandle, uint8_t * pBuf, uint32_t *pLen)
{
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

    uint32_t bodyLengthInHeaderBuf = 0;

    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(respHandle);
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(pBuf);
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(pLen);
    HTTPS_ON_ARG_ERROR_GOTO_CLEANUP(respHandle->isAsync);

    /* Set the current body in the respHandle to use in _receiveHttpsBody(). _receiveHttpsBody is generic
        to both async and sync request/response handling. In the sync version the body is configured during
        initializing the request. In the async version the body is given in this function on the fly. */
    respHandle->pBody = pBuf;
    respHandle->pBodyCur = respHandle->pBody;
    respHandle->pBodyEnd = respHandle->pBodyCur + *pLen;
    /* When there is part of the body in the header pBuffer. We need to move that data to this body pBuffer 
        provided in this fuction. */
    bodyLengthInHeaderBuf = respHandle->pBodyCurInHeaderBuf - respHandle->pBodyInHeaderBuf;
    if( bodyLengthInHeaderBuf > 0)
    {
        uint32_t copyLength = bodyLengthInHeaderBuf > *pLen ? *pLen : bodyLengthInHeaderBuf;
        memcpy(respHandle->pBodyCur, respHandle->pBodyInHeaderBuf, copyLength);
        respHandle->pBodyCur += copyLength;
        /* This function may be called multiple times until all of the body that may be present in the header buffer is
           moved out. */
        respHandle->pBodyInHeaderBuf += copyLength;
    }

    /* If there is room in the body buffer just provided by the application and we have not completed the current 
       HTTP response message, then try to receive more body. */
    if(((respHandle->pBodyEnd - respHandle->pBodyCur) > 0) && (respHandle->parserState < PARSER_STATE_BODY_COMPLETE))
    {
        status = _receiveHttpsBody(respHandle->pHttpsConnection, respHandle );
    }

    *pLen = respHandle->pBodyCur - respHandle->pBody;

    HTTPS_FUNCTION_CLEANUP_BEGIN();
    respHandle->bodyRxStatus = status;
    HTTPS_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_CancelRequestAsync(IotHttpsRequestHandle_t reqHandle)
{
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

    /* Both reqHandle and respHandle cannot be NULL. */
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(reqHandle);

    _cancelRequest(reqHandle);

    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_CancelResponseAsync(IotHttpsResponseHandle_t respHandle)
{
    HTTPS_FUNCTION_ENTRY( IOT_HTTPS_OK );

    /* Both reqHandle and respHandle cannot be NULL. */
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP( respHandle );
    
    _cancelResponse(respHandle);

    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}


/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_SendAsync(IotHttpsConnectionHandle_t connHandle, IotHttpsRequestHandle_t reqHandle, IotHttpsResponseHandle_t * pRespHandle, IotHttpsResponseInfo_t* pRespInfo)
{
    HTTPS_FUNCTION_ENTRY(IOT_HTTPS_OK);

    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(connHandle);
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(reqHandle);
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(pRespHandle);
    HTTPS_ON_ARG_ERROR_GOTO_CLEANUP(reqHandle->isAsync);

    /* Initialize the response handle to return. */
    status = _initializeResponse(pRespHandle, pRespInfo, true /* Is async. */, reqHandle->method);
    if(HTTPS_FAILED(status))
    {
        IotLogError("Failed to initialize the response on the synchronous request %d.", reqHandle);
        HTTPS_GOTO_CLEANUP();
    }

    /* Set the connection handle in the request handle so that we can use it in the _writeResponseBody() callback. */
    reqHandle->pHttpsConnection = connHandle;

    /* Set the connection handle in the response handle sp that we can use it in the _readReadyCallback() callback. */
    ( *pRespHandle )->pHttpsConnection = connHandle;

    /* Associate the response to the request so that we can schedule it to be received when the request gets scheduled to send. */
    reqHandle->pHttpsResponse = *pRespHandle;

    /* Add the request to the connection's request queue. */
    status = _addRequestToConnectionReqQ(reqHandle);
    if(HTTPS_FAILED(status))
    {
        IotLogError("Failed to add request %d to the connection's request queue. Error code: %d.", reqHandle, status);
        HTTPS_GOTO_CLEANUP();
    }

    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_ReadResponseStatus(IotHttpsResponseHandle_t respHandle, uint16_t *pStatus)
{
    HTTPS_FUNCTION_ENTRY(IOT_HTTPS_OK);

    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(respHandle);
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(pStatus);

    HTTPS_ON_ARG_ERROR_MSG_GOTO_CLEANUP(respHandle->status != 0,
        IOT_HTTPS_NOT_FOUND,
        "The HTTP response status was not found in the HTTP response header buffer.");

    *pStatus = respHandle->status;
    
    HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_ReadHeader(IotHttpsResponseHandle_t respHandle, char *pName, char *pValue, uint32_t len)
{
    HTTPS_FUNCTION_ENTRY(IOT_HTTPS_OK);

    const char * pHttpParserErrorDescription = NULL;
    IotHttpsResponseBufferState_t savedBufferState = PROCESSING_STATE_NONE;
    IotHttpsResponseParserState_t savedParserState = PARSER_STATE_NONE;
    size_t numParsed = 0;

    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(respHandle);
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(pName);
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(pValue);

    /* The buffer processing state is changed to searching the header buffer in this function. The parser state is 
    changed in the response to wherever the parser is currently located in the response. If this function is called
    in the middle of processing a response (for example in readReadyCallback() routine of an asynchronous response), 
    then parsing the response need to be able to start at the same place it was before calling this function. */
    savedBufferState = respHandle->bufferProcessingState;
    savedParserState = respHandle->parserState;

    /* The header search parameters in the response handle are used as context in the http-parser callbacks. During
    the callback, pReadHeaderField is checked against the currently parsed header name. foundHeaderFiled is set to 
    true when the pReadHeaderField is found in a header field callback. The bufferProcessingState tells the callback
    to skip the logic pertaining to when the response is being parsed for the first time. pReadHeaderValue will store
    the header value found. readHeaderValueLength will store the the header value found's length. */
    respHandle->pReadHeaderField = pName;
    respHandle->foundHeaderField = false;
    respHandle->bufferProcessingState = PROCESSING_STATE_SEARCHING_HEADER_BUFFER;
    respHandle->pReadHeaderValue = NULL;
    respHandle->readHeaderValueLength = 0;

    /* Start over the HTTP parser so that it will parser from the beginning of the message. */
    http_parser_init( &( respHandle->httpParserInfo.readHeaderParser ), HTTP_RESPONSE );
    respHandle->httpParserInfo.readHeaderParser.data = (void *)(respHandle);

    IotLogDebug( "Now parsing HTTP Message buffer to read a header." );
    numParsed = respHandle->httpParserInfo.parseFunc(&(respHandle->httpParserInfo.readHeaderParser), &_httpParserSettings, (char*)(respHandle->pHeaders), respHandle->pHeadersCur - respHandle->pHeaders);
    IotLogDebug("Parsed %d characters in IotHttpsClient_ReadHeader().", numParsed);
    /* There shouldn't be any errors parsing the parsing the response body given that the handle is from a validly
       received response, so this check is defensive. If there were errors parsing the original response headers, then
       the response handle would have been invalidated and the connection closed. */
    if( (respHandle->httpParserInfo.readHeaderParser.http_errno != 0) && 
        ( HTTP_PARSER_ERRNO( &(respHandle->httpParserInfo.readHeaderParser) ) > HPE_CB_chunk_complete) )
    {
        pHttpParserErrorDescription = http_errno_description( HTTP_PARSER_ERRNO( &(respHandle->httpParserInfo.readHeaderParser) ) );
        IotLogError("http_parser failed on the http response with error: %s", pHttpParserErrorDescription);
        HTTPS_SET_AND_GOTO_CLEANUP( IOT_HTTPS_PARSING_ERROR);
    }

    /* Not only do we need an indication that the header field was found, but also that the value was found as well.
       The value is found when it is non-NULL. The case where the header field is found, but the value is not found 
       occurs when there are incomplete headers stored in the header buffer. The header buffer could end with a header
       field name. */
    if(respHandle->foundHeaderField && ( respHandle->pReadHeaderValue != NULL))
    {
        /* The len of the pValue buffer must account for the NULL terminator. */
        if(respHandle->readHeaderValueLength > (len - 1))
        {
            IotLogError("IotHttpsClient_ReadHeader(): The length of the pValue buffer specified is less than the actual length of the pValue. ");
            HTTPS_SET_AND_GOTO_CLEANUP( IOT_HTTPS_INSUFFICIENT_MEMORY );
        }
        else
        {
            /* strncpy adds a NULL terminator. */
            strncpy(pValue, respHandle->pReadHeaderValue, respHandle->readHeaderValueLength);
        }
    }
    else
    {
        IotLogError("IotHttpsClient_ReadHeader(): The header field %s was not found.", pName);
        HTTPS_SET_AND_GOTO_CLEANUP( IOT_HTTPS_NOT_FOUND );
    }

    HTTPS_FUNCTION_CLEANUP_BEGIN();
    /* Always restore the state back to what it was before entering this function. */
    if( respHandle != NULL )
    {
        respHandle->bufferProcessingState = savedBufferState;
        respHandle->parserState = savedParserState;
    }
    HTTPS_FUNCTION_CLEANUP_END();
}

/*-----------------------------------------------------------*/

IotHttpsReturnCode_t IotHttpsClient_ReadContentLength(IotHttpsResponseHandle_t respHandle, uint32_t *pContentLength)
{   
    HTTPS_FUNCTION_ENTRY(IOT_HTTPS_OK);

    const int CONTENT_LENGTH_NUMBERIC_BASE = 10;
    char pContentLengthStr[HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH] = { 0 };

    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(respHandle);
    HTTPS_ON_NULL_ARG_GOTO_CLEANUP(pContentLength);

    /* If there is no content-length header or if we were not able to store it in the header buffer this will be
       invalid. We do not use the content-length member of the http-parser state structure to get the content
       length as this is a PRIVATE member. Because it is a PRIVATE member it can be any value. */
    status = IotHttpsClient_ReadHeader(respHandle, HTTPS_CONTENT_LENGTH_HEADER, pContentLengthStr, HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH);
    if(HTTPS_FAILED(status))
    {
        *pContentLength = 0;
        IotLogError("Could not read the Content-Length for the response.");
        HTTPS_GOTO_CLEANUP();
    }

    *pContentLength = strtoul(pContentLengthStr, NULL, CONTENT_LENGTH_NUMBERIC_BASE);

   HTTPS_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/
