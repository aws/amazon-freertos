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

#ifndef IOT_HTTPS_INTERNAL_H_
#define IOT_HTTPS_INTERNAL_H_

/* The config header is always included first. */
#include "iot_config.h"

/* Standard Includes. */
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

/* HTTPS Client library includes. */
#include "iot_https_client.h"

/* Third party http-parser include. */
#include "http_parser.h"

/* Task pool include. */
#include "iot_taskpool.h"

/* Types include. */
#include "types/iot_taskpool_types.h"

/* Platform layer includes. */
#include "platform/iot_threads.h"
#include "platform/iot_network.h"

/*-----------------------------------------------------------*/ 

/* Configure logs for HTTPS Client functions. */
#ifdef IOT_LOG_LEVEL_HTTPS
    #define LIBRARY_LOG_LEVEL        IOT_LOG_LEVEL_HTTPS
#else
    #ifdef IOT_LOG_LEVEL_GLOBAL
        #define LIBRARY_LOG_LEVEL    IOT_LOG_LEVEL_GLOBAL
    #else
        #define LIBRARY_LOG_LEVEL    IOT_LOG_NONE
    #endif
#endif

#define LIBRARY_LOG_NAME    ( "HTTPS Client" )
#include "iot_logging_setup.h"

/*
 * Provide default values for undefined memory allocation functions based on
 * the usage of dynamic memory allocation.
 */
#if IOT_STATIC_MEMORY_ONLY == 1
    #include "private/iot_static_memory.h"
#endif

/**
 * @cond DOXYGEN_IGNORE
 * Doxygen should ignore this section.
 *
 * Provide default values for undefined configuration constants.
 */
#ifndef AWS_IOT_HTTPS_ENABLE_METRICS
    #define AWS_IOT_HTTPS_ENABLE_METRICS        ( 1 )
#endif
#ifndef IOT_HTTPS_USER_AGENT
    #define IOT_HTTPS_USER_AGENT                "amazon-freertos"
#endif
#ifndef IOT_HTTPS_MAX_CONN_USAGE_WAIT_MS
    #define IOT_HTTPS_MAX_CONN_USAGE_WAIT_MS    ( 60000 )
#endif 
#ifndef IOT_HTTPS_MAX_FLUSH_BUFFER_SIZE
    #define IOT_HTTPS_MAX_FLUSH_BUFFER_SIZE     ( 1024 )
#endif
#ifndef IOT_HTTPS_RESPONSE_WAIT_MS
    #define IOT_HTTPS_RESPONSE_WAIT_MS          ( 1000 )
#endif
#ifndef IOT_HTTPS_MAX_HOST_NAME_LENGTH
    #define IOT_HTTPS_MAX_HOST_NAME_LENGTH      ( 255 ) /* Per FQDN, the maximum host name length is 255 bytes. */
#endif
#ifndef IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH
    #define IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH ( 255 ) /* The maximum alpn protocols length is chosen arbitrarily. */
#endif

/** @endcond */

/*-----------------------------------------------------------*/ 

/**
 * @brief The state of the HTTP response parsing. 
 * 
 * This state notes what has been parsed in the HTTP response.
 * 
 * The states move as follows:
 * PARSER_STATE_NONE --> PARSER_STATE_IN_HEADERS --> PARSER_STATE_HEADERS_COMPLETE --> PARSER_STATE_BODY_COMPLETE
 * 
 * The parser callbacks are called in the following order:
 * 1. _httpParserOnMessageBeginCallback()
 * 2. _httpParserOnStatusCallback()
 * 3. _httpParserOnHeaderFieldCallback()
 * 4. _httpParserOnHeaderValueCallback()
 * 5. _httpParserOnHeadersCompleteCallback()
 * 6. _httpParserOnChunkHeaderCallback() (optional only if the response is chunked)
 * 7. _httpParserOnBodyCallback()
 * 8. _httpParserOnChunkCompleteCallback() (optional only if the response is chunked)
 * 9. _httpParserOnMessageCompleteCallback()
 * 
 * Theses states are set in the parser callbacks and used outside the callbacks to determine action.
 * 
 * PARSER_STATE_NONE is assigned to #_httpsResponse_t.parserState when the _httpsResponse_t.parserState is initialized 
 * in @ref IotHttpsClient_InitializeRequest and before parsing a new respone message from the server.
 * 
 * PARSER_STATE_IN_HEADERS is assigned the start of the HTTP Response message. This occurs in the 
 * _httpParserOnMessageBeginCallback(). HTTP headers are always first and there is always the response status line
 * and some headers in a response message acccording to RFC 2616.
 * 
 * PARSER_STATE_HEADERS_COMPLETE is assigned when all of the headers are finished being parsed in the HTTP response
 * message. This occurs in the _httpParserOnHeadersCompleteCallback(). The state can end here if the response has no
 * body, like for a response to a HEAD request. 
 * If this state is not reached, after receiving headers from the network into the user configured header buffer and 
 * running it through the parser, then we know that not all of the headers from the response could fit into the buffer.
 * 
 * PARSER_STATE_IN_BODY is assigned each time the parser reaches HTTP response body. This occurs in the 
 * _httpParserOnBodyCallback().
 * 
 * PARSER_STATE_BODY_COMPLETE is assigned when the parser has finished with the whole HTTP response message. This 
 * happens when _httpParserOnMessageCompleteCallback() is invoked.
 * If this state is not reached, after receiving body from the network into the user configured body buffer and 
 * running it through the parser, then we know that not all of the body from the response could fit into the buffer.
 */
typedef enum IotHttpsResponseParserState {
    PARSER_STATE_NONE = 0,          /**< @brief The parser has not started so we are neither in the headers or the body. */
    PARSER_STATE_IN_HEADERS,        /**< @brief The parser is currently parsing the HTTP respone headers. */
    PARSER_STATE_HEADERS_COMPLETE,  /**< @brief The parser has finished parsing the headers. */
    PARSER_STATE_IN_BODY,           /**< @brief The parser is currently parsing the HTTP response body. */
    PARSER_STATE_BODY_COMPLETE      /**< @brief The parser has completed parsing the HTTP response body. */
} IotHttpsResponseParserState_t;

/**
 * @brief The state denoting which buffer (the header buffer or the body buffer) that is currently being processed
 * and for what.
 * 
 * This state is set outside of the parser callbacks and used inside the of parser callbacks to determine actions.
 * 
 * The state moves as follows:
 * Receiving and parsing a response: PROCESSING_STATE_NONE --> PROCESSING_STATE_FILLING_HEADER_BUFFER --> PROCESSING_STATE_FILLING_BODY_BUFFER --> PROCESSING_STATE_FINISHED
 * Searching a response for headers: ((enter state)) --> PROCESSING_STATE_SEARCHING_HEADER_BUFFER --> ((enter state))
 * 
 * PROCESSING_STATE_NONE is assigned when #_httpsResponse_t.bufferProcessingState is being initialize in 
 * @ref IotHttpsClient_InitializeRequest.
 * 
 * PROCESSING_STATE_FILLING_HEADER_BUFFER is assigned at the start of receiving HTTP response headers from the network 
 * into the header buffer, before processing the received headers with the parser. 
 * This state is then used in the parser callbacks _httpParserOnStatusCallback(), _httpParserOnHeaderFieldCallback(), 
 * _httpParserOnHeaderValueCallback(), and _httpParserOnHeadersCompleteCallback() to move the 
 * #_httpsResponse_t.headersCur pointer along in the header buffer.
 * Since the server sends the HTTP response as a single continuous message, somtimes during receiving of the HTTP 
 * headers we may receive part or all of the HTTP response body:
 * ((example header buffer))[headers headers headers headers body body body]
 * When parsing this header buffer the parser will execute _httpParserOnBodyCallback() in the 
 * PROCESSING_STATE_FILLING_HEADER_BUFFER state. The state is used here, for an asynchronous response, to save where
 * and how much body is inside the of the header buffer. When a body buffer becomes available, the body in the header
 * buffer will be copied to the body buffer.
 * 
 * PROCESSING_STATE_FILLING_BODY_BUFFER is assigned at the start of receiving the HTTP response body form the network 
 * into the body buffer, before processing the received body with the parser.
 * 
 * PROCESSING_STATE_FINISHED is assigned at the end of IotHttpsClient_SendSync() or at the end of 
 * IotHttpsClient_SendAsync() when both the header and body buffer are finished being filled with network data and 
 * parsed.
 * 
 * PROCESSING_STATE_SEARCHING_HEADER_BUFFER is assigned in IotHttpsClient_ReadHeader() when searching for a header
 * in the header buffer.
 * This state is used in the parser callback _httpParserOnHeaderFieldCallback() to check if the current header field
 * parsed equals the header we are searching for. It is used in parser callback _httpParserOnHeaderValueCallback() to 
 * return the header value if the corresponding field we are searching for was found. It is used in parser callback 
 * _httpParserOnHeadersCompleteCallback() to stop parsing the header buffer if the header we are searching for was not
 * found.
 * 
 * The header buffer is separate from the body buffer.
 * The header buffer is configured in #IotHttpRequestInfo_t.respUserBuff. The body buffer is configured in 
 * #IotHttpRequestInfo_t.syncInfo->respData or as buffer provided asynchronously during the 
 * #IotHttpsClientCallbacks_t.readReadyCallback() to call to @ref IotHttpsClient_ReadResponseBody().
 */
typedef enum IotHttpsResponseBufferState {
    PROCESSING_STATE_NONE,                      /**< @brief There is no buffer processing currently. */
    PROCESSING_STATE_FILLING_HEADER_BUFFER,     /**< @brief The header buffer is being filled and parsed. */
    PROCESSING_STATE_FILLING_BODY_BUFFER,       /**< @brief The body buffer is being filled and parsed. */
    PROCESSING_STATE_FINISHED,                  /**< @brief Filling and parsing of both buffers is finished. */
    PROCESSING_STATE_SEARCHING_HEADER_BUFFER    /**< @brief The header buffer is being search. */
} IotHttpsResponseBufferState_t;

/*-----------------------------------------------------------*/ 

/**
 * @brief Represents an HTTP connection.
 */
typedef struct _httpsConnection
{
    IotNetworkInterface_t * pNetworkInterface;  /**< @brief Network interface with calls for connect, disconnect, send, and receive. */
    void *pNetworkConnection;                   /**< @brief Pointer to the network connection to use pNetworkInterface calls on. */
    IotSemaphore_t connSem;                     /**< @brief Binary semaphore to lock the connection if a request/response is currently in progress. */
    bool nonPersistent;                         /**< @brief Non persistent flag to indicate closing the connection immediately after receiving the request. */
    IotSemaphore_t rxStartSem;                  /**< @brief Semaphore indicating that data on the network is ready to read. */
    IotSemaphore_t rxFinishSem;                 /**< @brief Semaphore indicating that the data on the network is done being read. */
    uint32_t timeout;                           /**< @brief Timeout for a connection and waiting for a response from the network. */
    bool isConnected;                           /**< @brief true if a connection was successful most recently on this context. We have not way of knowing if the 
                                                            serving closed the connection on us because that error is unique to the underlying TLS layer. This is set
                                                            to false initially, set to true for a successful intentional call to connect, and then set to false only
                                                            after an explicit disconnect with a non-persistent request or a call to @ref https_client_function_disconnect. */
} _httpsConnection_t;

/**
 * @brief Represents an HTTP response.
 */
typedef struct _httpsResponse
{
    uint8_t * pHeaders;      /**< @brief Pointer to the start of the headers buffer. */
    uint8_t * pHeadersEnd;   /**< @brief Pointer to the end of the headers buffer. */
    uint8_t * pHeadersCur;   /**< @brief Pointer to the next location to fill in the headers buffer. */
    uint8_t * pBody;         /**< @brief Pointer to the start of the body buffer. */
    uint8_t * pBodyCur;      /**< @brief Pointer to the next location to fill in the body buffer. */
    uint8_t * pBodyEnd;      /**< @brief Pointer to the end of the body buffer. */
    http_parser httpParser; /**< @brief Instance of the http_parser for response processing. */
    uint16_t status;        /**< @brief The HTTP response status code of this response. */
    IotHttpsMethod_t method;    /**< @brief The method of the originating request. */
    uint32_t contentLength; /**< @brief The content length of the response body. This is zero when no Content-Length header was parsed the header buffer. */
    IotHttpsResponseParserState_t parserState;  /**< @brief The current state of the parser. See #IotHttpsResponseParserState_t documentation for more details. */
    IotHttpsResponseBufferState_t bufferProcessingState;    /**< @brief Which buffer is currently being processed and for what. See #IotHttpsResponseBufferState_t documentation. */
    char * pReadHeaderField; /**< @brief Header field that we want to read from the header buffer from calling IotHttpsClient_ReadHeader(). */
    char * pReadHeaderValue; /**< @brief Header value that we read from the headers buffer from calling IotHttpsClient_ReadHeader(). */
    size_t readHeaderValueLength;   /**< @brief Length of pReadHeaderValue. */
    bool foundHeaderField;  /**< @brief State to use during parsing to let us know when we found the header field in the https-parser callbacks. 
                                        This is set to true when the header field is found in [parser callback _httpParserOnHeaderFieldCallback().
                                        On the following parser callback _httpParserOnHeaderValueCallback() we will store the value in pReadHeaderValue then exit the parsing. */
    struct _httpsConnection *pConnHandle;    /**< @brief Connection associated with response. This is set during IotHttpsClient_SendAsync() and IotHttpsClient_SendSync(). */
    struct _httpsRequest *pReqHandle;        /**< @brief Request associated with response. This is set during IotHttpsClient_InitializeRequest(). */
} _httpsResponse_t;

/**
 * @brief Represents and HTTP request.
 */
typedef struct _httpsRequest
{
    uint8_t * pHeaders;      /**< @brief Pointer to the start of the headers buffer. */
    uint8_t * pHeadersEnd;   /**< @brief Pointer to the end of the headers buffer. */
    uint8_t * pHeadersCur;   /**< @brief Pointer to the next location to fill in the headers buffer. */
    uint8_t * pBody;         /**< @brief Pointer to the start of the body buffer. */
    uint32_t bodyLength;    /**< @brief Length of request body buffer. */
    IotHttpsConnectionInfo_t* pConnInfo;    /**< @brief Connection info associated with this request. */
    struct _httpsResponse *pRespHandle;     /**< @brief Response associated with request. This is set during IotHttpsClient_InitializeRequest(). */
    struct _httpsConnection *pConnHandle;   /**< @brief Connection associated with request. This is set during IotHttpsClient_SendAsync() and IotHttpsClient_SendSync(). */
    uint32_t contentLength; /**< @brief The content length of the request body. */
} _httpsRequest_t;

#endif /* IOT_HTTPS_INTERNAL_H_ */
