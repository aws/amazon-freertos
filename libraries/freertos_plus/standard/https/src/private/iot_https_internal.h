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

/* Types include. */
#include "types/iot_taskpool_types.h"

/* Platform layer includes. */
#include "platform/iot_threads.h"
#include "platform/iot_network.h"

/*-----------------------------------------------------------*/ 

/* Configure logs for HTTPS functions. */
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
/** @endcond */

/* The state of the httpParser when processig the HTTPS response. */
typedef enum IotHttpsResponseParserState {
    eIotHttpsParserNone = 0,
    eIotHttpsParserInHeaders,
    eIotHttpsParserHeadersComplete,
    eIotHttpsParserInBody,
    eIotHttpsParserBodyComplete /* The state will be set to this when the whole HTTP response message has been parsed. */
} IotHttpsResponseParserState_t;

/* Determine if the response is currently being filled in the header buffer
   or the body buffer. */
typedef enum IotHttpsResponseBufferState {
    eIotHttpsHeaderBuffer,
    eIotHttpsBodyBuffer,
    eIotHttpsFinished,
    eIotHttpsHeaderSearch
} IotHttpsResponseBufferState_t;

typedef struct _httpsConnection
{
    IotNetworkInterface_t * pNetworkInterface;   /* Network interface with calls for connect, disconnect, send, and receive. */
    void *pNetworkConnection;                    /* Pointer to the network connection to use pNetworkInterface calls on. */
    IotSemaphore_t connSem;             /* Binary semaphore to lock the connection if a request/response is currently in progress. */
    bool nonPersistent;                 /* Non persistent flag to indicate closing the connection immediately after receiving the request. */
    IotSemaphore_t rxStartSem;          /* Semaphore indicating that the socket is ready to read. */
    IotSemaphore_t rxFinishSem;         /* Semaphore indicating that the socket is done being read. */
    uint32_t timeout;
} _httpsConnection_t;

/**
 * With a separate response structure, we create a mapping from requestInternal->resposeInternal.
 * If we want to pipeline asynchronous requests, then the responses would come in the order of the
 * requests sent.
 * If callbacks and responses are associated with a request and not a connection, then we need to lock
 * the connection until a response has been fully received for a request. If we lock a connection, then
 * we cannot perform HTTPS pipelineing of requests.
 */
typedef struct _httpsResponse
{
    uint8_t * headers;      /* Begining of headers. */
    uint8_t * headersEnd;   /* End of headers buffer. */
    uint8_t * headersCur;   /* Last valid filled location in the headers buffer. */
    uint8_t * body;         /* Begin of body. This will be NULL to ignore body or for a HEAD request. */
    uint8_t * bodyCur;      /* The current pointer into the body for receiving partial bodies. */
    uint8_t * bodyEnd;      /* End of the body buffer. */
    uint8_t * bodyStartInHeaderBuf;     /* Pointer to the start of body inside the header buffer for copying to
                                        asyncrhonous response processing. */
    uint32_t bodyLengthInHeaderBuf;     /* Length of the body pointed to by bodyStartInHeaderbuf. */
    IotHttpsReturnCode_t bodyRxStatus;  /* The status of the receiving the HTTPS body in an async response. */
    http_parser httpParser; /* Instance of the http_parser for response processing. */
    uint16_t status;        /* The status of the response. */
    IotHttpsMethod_t method; /* The method of the originating requires. */
    uint32_t contentLength; /* The content length of the response body. This is zero for no Content-Length header sent by the server. */
    IotHttpsResponseParserState_t parserState;   /* Where the httpParser left off when executing. */
    IotHttpsResponseBufferState_t bufferState;   /* Which buffer is currently being filled when receiving the response from the network. */
    char * readHeaderField; /* Header field that we want to read from calling IotHttpsClient_ReadHeader(). */
    char * readHeaderValue; /* Header value that we read from the headers buffer. This is for storing the value to be returned
                               when calling IotHttpsClient_ReadHeader(). */
    size_t readHeaderValueLength;   /* Length of readHeaderValue. */
    bool foundHeaderField;  /* State to store to let us know when we found the header field in the https-parser callbacks. On the following
                               header value callback we will store the value in readHeaderValue then exit the parsing. */
    bool isAsync;           /* This is set to true if this response is currently being retrieved asynchronously. Set to
                               false otherwise. */
    struct _httpsConnection *pConnHandle;    /* Connection associated with response. This is set during IotHttpsClient_SendAsync(). */
    struct _httpsRequest *pReqHandle;        /* Request associated with response. This is set during request initialization. */
    bool cancelled;         /* Set this to true to cancel the request/response processing in the async workflow. */
} _httpsResponse_t;

typedef struct _httpsRequest
{
    uint8_t * headers;      /* Begining of headers. */
    uint8_t * headersEnd;   /* End of headers buffer. */
    uint8_t * headersCur;   /* Last valid filled location in the headers buffer. */
    uint8_t * body;         /* Begin of request body. */
    uint32_t bodyLength;    /* Length of request body. */
    IotHttpsClientCallbacks_t* callbacks;    /* asynchronous request pointers. */
    IotHttpsConnectionInfo_t* pConnInfo;          /* Possible connection info if connection is to be made at request send time. */
    struct _httpsResponse *pRespHandle; /* Internal response context pointer. */
    struct _httpsConnection *pConnHandle; /* Current connection associated with this request. This is set during IotHttpsClient_SendAsync().*/
    uint32_t contentLength; /* The content length of the request body. */
    IotTaskPoolJobStorage_t taskPoolJobStorage; /* An asynchronous opertion requires storage for the task pool job. */
    IotTaskPoolJob_t taskPoolJob;               /* The task pool job identifier for an asynchronous request. */
    void * userPrivData;                        /* user private data to hand back in the callbacks for an asynchronous request. */
    bool cancelled;         /* Set this to true to cancel the request/response processing in the async workflow. */
} _httpsRequest_t;

#endif /* IOT_HTTPS_INTERNAL_H_ */
