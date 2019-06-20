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

/* Request Line lengths */
#define HTTPS_REQUEST_LINE_LENGTH                   20 /* strlen("CONNECT / HTTP/1.1\r\n"). The minimum path is "/" and CONNECT is the longest HTTP method according to rfc 2616. */

#define HTTPS_PROTOCOL_VERSION                      "HTTP/1.1"

#define HTTPS_CONTENT_LENGTH_HEADER                 "Content-Length"
#define HTTPS_USER_AGENT_HEADER                     "User-Agent"
#define HTTPS_HOST_HEADER                           "Host"
#define HTTPS_CONNECTION_HEADER                     "Connection"
#define HTTPS_HEADER_FIELD_SEPARATOR                ": "
#define HTTPS_END_OF_HEADER_LINES_INDICATOR         "\r\n"
#define HTTPS_MAX_FILE_SIZE_LENGTH                  10      /* strlen(str(2^32)) */

#define HTTPS_CONNECTION_KEEP_ALIVE_HEADER_VALUE    "keep-alive"
#define HTTPS_CONNECTION_CLOSE_HEADER_VALUE         "closed"

/* Headers that are always there. */
#define HTTPS_USER_AGENT_HEADER_LINE            HTTPS_USER_AGENT_HEADER HTTPS_HEADER_FIELD_SEPARATOR IOT_HTTPS_USER_AGENT HTTPS_END_OF_HEADER_LINES_INDICATOR
#define HTTPS_PARTIAL_HOST_HEADER_LINE          HTTPS_HOST_HEADER HTTPS_HEADER_FIELD_SEPARATOR HTTPS_END_OF_HEADER_LINES_INDICATOR   

#define HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH    26 /* strlen("Content-Length: 4294967296\r\n")*/
#define HTTPS_CONNECTION_KEEP_ALIVE_HEADER_LINE HTTPS_CONNECTION_HEADER HTTPS_HEADER_FIELD_SEPARATOR HTTPS_CONNECTION_KEEP_ALIVE_HEADER_VALUE HTTPS_END_OF_HEADER_LINES_INDICATOR
#define HTTPS_CONNECTION_KEEP_ALIVE_HEADER_LINE_LENGTH  24 /* strlen("Connection: keep-alive\r\n") */
#define HTTPS_CONNECTION_CLOSE_HEADER_LINE      HTTPS_CONNECTION_HEADER HTTPS_HEADER_FIELD_SEPARATOR HTTPS_CONNECTION_CLOSE_HEADER_VALUE HTTPS_END_OF_HEADER_LINES_INDICATOR

const uint32_t requestUserBufferMinimumSize = sizeof(_httpsRequest_t) + 
                HTTPS_REQUEST_LINE_LENGTH +
                sizeof(HTTPS_USER_AGENT_HEADER_LINE) +
                sizeof( HTTPS_PARTIAL_HOST_HEADER_LINE) +
                sizeof(HTTPS_END_OF_HEADER_LINES_INDICATOR);
const uint32_t responseUserBufferMinimumSize = sizeof(_httpsRequest_t);
const uint32_t connectionUserBufferMinimumSize = sizeof(_httpsConnection_t);

const char* httpsMethodStrings[] = {
    "GET",
    "HEAD"
};

/* HTTPS Parser callback functions are pointed to by this structure. */
http_parser_settings _httpParserSettings;

static int _httpParserOnMessageBeginCallback(http_parser * httpParser)
{
    IotLogDebug("Parser: Start of HTTPS Response message.");
    _httpsResponse_t *_httpsResponse = (_httpsResponse_t*)(httpParser->data);
    _httpsResponse->parserState = eIotHttpsParserInHeaders;
    return 0;
}

static int _httpParserOnStatusCallback(http_parser * httpParser, const char * loc, size_t length)
{
    _httpsResponse_t *_httpsResponse = (_httpsResponse_t*)(httpParser->data);
    IotLogDebug("Parser: Status %.*s retrieved from HTTPS response.", length, loc);
    /* We need to save the status. */
    _httpsResponse->status = httpParser->status_code;
    if(_httpsResponse->bufferState == eIotHttpsHeaderBuffer)
    {
        _httpsResponse->headersCur = loc += length;
    }
    return 0;
}

static int _httpParserOnHeaderFieldCallback(http_parser * httpParser, const char * loc, size_t length)
{
    IotLogDebug("Parser: HTTPS header field parsed %.*s", length, loc);
    _httpsResponse_t * _httpsResponse = (_httpsResponse_t *)(httpParser->data);
    if(_httpsResponse->bufferState == eIotHttpsHeaderBuffer)
    {
        _httpsResponse->headersCur = loc += length;
    }
    if(_httpsResponse->bufferState == eIotHttpsHeaderSearch)
    {
        if(strncmp(_httpsResponse->readHeaderField, loc, length) == 0)
        {
            _httpsResponse->foundHeaderField = true;   
        }
    }
    return 0;
}

static int _httpParserOnHeaderValueCallback(http_parser * httpParser, const char * loc, size_t length)
{
    IotLogDebug("Parser: HTTPS header value parsed %.*s", length, loc);
    _httpsResponse_t * _httpsResponse = (_httpsResponse_t *)(httpParser->data);
    if(_httpsResponse->bufferState == eIotHttpsHeaderBuffer)
    {
        _httpsResponse->headersCur = loc += length;
    }
    if(_httpsResponse->bufferState == eIotHttpsHeaderSearch)
    {
        if(_httpsResponse->foundHeaderField)
        {
            _httpsResponse->readHeaderValue = loc;
            _httpsResponse->readHeaderValueLength = length;
            /* We found a header field so we don't want to keep parsing.*/
            return 1;
        }
    }
    return 0;
}

static int _httpParserOnHeadersCompleteCallback(http_parser * httpParser)
{
    IotLogDebug("Parser: End of the headers reached in the httpParser.");
    _httpsResponse_t * _httpsResponse = (_httpsResponse_t *)(httpParser->data);
    _httpsResponse->parserState = eIotHttpsParserHeadersComplete;

    /* In the header search state we return after finishing looking through all of the headers. */
    if(_httpsResponse->bufferState == eIotHttpsHeaderSearch)
    {
        return 1;
    }

    /* When here the headerCur pointer is at the "\r" following the last header value. HTTP/1.1
       headers end with another "\r\n" at the end of the last line. This means we must increment
       the headerCur pointer to the length of "\r\n\r\n". */
    if(_httpsResponse->bufferState == eIotHttpsHeaderBuffer)
    {
        _httpsResponse->headersCur += strlen("\r\n\r\n");
    }

    /* content_length will be zero if no Content-Length header. */
    _httpsResponse->contentLength = httpParser->content_length;

    /* For a HEAD method, there is not body expected in the response, so we return 1 to skip body parsing. 
       Also if it was configured to ignore the HTTPS response body then also stop the body parsing. */
    if(_httpsResponse->method == IOT_HTTPS_METHOD_HEAD || (_httpsResponse->isAsync == false && _httpsResponse->body == NULL))
    {
        return 1;
    }
    return 0;
}

static int _httpParserOnBodyCallback(http_parser * httpParser, const char * loc, size_t length)
{
    IotLogDebug("Parser: Reached the HTTPS message body. It is of length: %d", length);
    /* Even when there is a chunk, we want to write the body directly to the response data 
    location specified.
     */
    _httpsResponse_t * _httpsResponse = (_httpsResponse_t *)(httpParser->data);
    _httpsResponse->parserState = eIotHttpsParserInBody;

    if(_httpsResponse->bufferState == eIotHttpsHeaderBuffer)
    {
        if(_httpsResponse->isAsync)
        {
            /* For an asynchronous response, the buffer to store the body will be available after the headers 
             * are read first. We may receive part of the body in the header buffer. We will want to leave this
             * and copy it over when the body buffer is available.
             */
            _httpsResponse->bodyStartInHeaderBuf = loc;
            _httpsResponse->bodyLengthInHeaderBuf = (uint32_t)length;
            /* Do not process anymore body that is inside the header buffer. */
            return 1;
        }
        else
        {
            /* If this is synchronous reponse and there is a response body buffer configured by the application,
               then we want to check if the length of the body the parser is currently at will fit into that buffer. */
            if(_httpsResponse->body != NULL && _httpsResponse->bodyCur + length > _httpsResponse->bodyEnd)
            {
                /* Move as must of the body as we can into the body buffer. */
                length = _httpsResponse->bodyEnd - _httpsResponse->bodyCur;
            }
        }
    }

    /* If we are in the header buffer and we come across HTTPS body, then we want to copy it to the body
       buffer. */
    /* Only copy the data if the current location is not the bodyCur. */
    if(_httpsResponse->bodyCur != NULL)
    {
        if(_httpsResponse->bodyCur != loc)
        {
            memcpy(_httpsResponse->bodyCur, loc, length);
        }
        _httpsResponse->bodyCur += length;
    }
    return 0;
}

static int _httpParserOnMessageCompleteCallback(http_parser * httpParser)
{
    IotLogDebug("Parser: End of the HTTPS message reached.");
    _httpsResponse_t * _httpsResponse = (_httpsResponse_t *)(httpParser->data);
    _httpsResponse->parserState = eIotHttpsParserBodyComplete;

    /* When this callback is reached the parser does not continue even if the length of buffer passed in is 
       larger than the message. */
    return 0;
}

/* Callback used for https-parser library when the message transfer encoding type is "chunked".
   This callback is called when the parser comes across the start of a chunk in the message body.
   The start of a chunk is denoted by the chunk size followed by \r\n. */
static int _httpParserOnChunkHeaderCallback(http_parser * httpParser)
{
    IotLogDebug("Parser: HTTPS message Chunked encoding header callback.");
    IotLogDebug( "Parser: HTTPS message Chunk size: %d", httpParser->content_length );
    return 0;
}

/* Callback used when a chunk is complete. The end of a chunk is denoted by \r\n. */
static int _httpParserOnChunkCompleteCallback(http_parser * httpParser)
{
    IotLogDebug("End of a HTTPS message Chunk complete callback.");
    return 0;
}

void IotHttpsClient_ReceiveCallback( void* pNetworkConnection, void* pReceiveContext )
{
    _httpsConnection_t* _httpsConnection = ( _httpsConnection_t* )pReceiveContext;
    /* Post that any thread in the connection can read on the socket. */
    IotSemaphore_Post( &( _httpsConnection->rxStartSem ) );
    /* Block while that thread reads from the socket. */
    IotSemaphore_Wait( &( _httpsConnection->rxFinishSem ) );
}


IotHttpsReturnCode_t IotHttpsClient_Init( void )
{
    _httpParserSettings.on_message_begin = _httpParserOnMessageBeginCallback;
    _httpParserSettings.on_status = _httpParserOnStatusCallback;
    _httpParserSettings.on_header_field = _httpParserOnHeaderFieldCallback;
    _httpParserSettings.on_header_value = _httpParserOnHeaderValueCallback;
    _httpParserSettings.on_headers_complete = _httpParserOnHeadersCompleteCallback;
    _httpParserSettings.on_body = _httpParserOnBodyCallback;
    _httpParserSettings.on_message_complete = _httpParserOnMessageCompleteCallback;
    _httpParserSettings.on_chunk_header = _httpParserOnChunkHeaderCallback;
    _httpParserSettings.on_chunk_complete = _httpParserOnChunkCompleteCallback;

    return IOT_HTTPS_OK;
}

void IotHttpsClient_Deinit( void )
{
    /* The library has not taken any resources that need freeing. This implementation is here for formality.*/
}


IotHttpsReturnCode_t IotHttpsClient_Connect(IotHttpsConnectionHandle_t * pConnHandle, IotHttpsConnectionInfo_t *pConnConfig)
{
    IotNetworkError_t networkStatus = IOT_NETWORK_SUCCESS;

    /* Set the internal connection context to the start of the user buffer. */
    _httpsConnection_t *_httpsConnection = (_httpsConnection_t *)((*pConnConfig).userBuffer.pBuffer);
    
    /* Keep track of a non-persistent HTTP connection. If this flag is set to true, then we will call IotHttpsClient_Disconnect
       at the end of a synchronous or asychronous request. The end of a request is when the response has been fully recieved. */
    if( pConnConfig->flags & IOT_HTTPS_IS_NON_PERSISTENT_FLAG )
    {
        _httpsConnection->nonPersistent = true;
    }
    else
    {
        _httpsConnection->nonPersistent = false;
    }

    /* This timeout is used to wait for a response on the connection. */
    if( pConnConfig->timeout == 0 )
    {
        _httpsConnection->timeout = IOT_HTTPS_RESPONSE_WAIT_MS;
    }
    else
    {
        _httpsConnection->timeout = pConnConfig->timeout;
    }

    /* pNetworkInterface contains the connect, disconnect, send, and receive over the network functions. */
    _httpsConnection->pNetworkInterface = ( IotNetworkInterface_t* )( pConnConfig->pNetworkInterface );
    
    /* I am hoping this is a temporary malloc until the interface changes to include the length. All configuration 
       structures that ask for a string should also ask for a length. This saves memory allocation. This is useful 
       for parsing a URL and passing the pointer to the host name within the URL string. If the configuration is within 
       some JSON string or other buffer, we should be able to pointer directly to that (possible const) buffer and provide 
       a length in that buffer instead of allocating memory simply for a NULL terminated string.
      */
    char * pHostName = ( char* )malloc( (size_t)(pConnConfig->addressLen) + 1);
    if( pHostName != NULL )
    {
        memcpy( pHostName, pConnConfig->pAddress, pConnConfig->addressLen );
        pHostName[pConnConfig->addressLen] = '\0';
    }

    IotNetworkServerInfo_t networkServerInfo = {
        .pHostName = pHostName, /* This requires a NULL terminated string. */
        .port = pConnConfig->port
    };

    /* The TLS handshakes requires information about SNI, alpn protocols, trusted
       server certificate, the client certificate, and client private key.*/
    IotNetworkCredentials_t networkCredentials = { 0 };
    if( ( pConnConfig->flags & IOT_HTTPS_IS_NON_TLS_FLAG ) == 0 )
    {
        if( pConnConfig->flags & IOT_HTTPS_DISABLE_SNI )
        {
            networkCredentials.disableSni = true;
        }
        else
        {
            networkCredentials.disableSni = false;
        }

        char *pAlpnProtos = ( char* )malloc( ( size_t )( pConnConfig->alpnProtocolsLen ) + 1 );
        if( pConnConfig->pAlpnProtocols != NULL )
        {
            memcpy( pAlpnProtos, pConnConfig->pAlpnProtocols, pConnConfig->alpnProtocolsLen );
        }
        pAlpnProtos[pConnConfig->alpnProtocolsLen] = '\0';
        networkCredentials.pAlpnProtos = pAlpnProtos;
  
        networkCredentials.pRootCa = pConnConfig->pCaCert;
        networkCredentials.rootCaSize = pConnConfig->caCertLen;
        networkCredentials.pClientCert = pConnConfig->pClientCert;
        networkCredentials.clientCertSize = pConnConfig->clientCertLen;
        networkCredentials.pPrivateKey = pConnConfig->pPrivateKey;
        networkCredentials.privateKeySize = pConnConfig->privateKeyLen;

        /* create() will connect to the server specified. */
        networkStatus = _httpsConnection->pNetworkInterface->create( &networkServerInfo,
            &networkCredentials,
            &( _httpsConnection->pNetworkConnection ) );
        
        /* if memory allocation for the alpn protocols succeeded then free that memory. */
        if( pConnConfig->pAlpnProtocols != NULL )
        {
            free( pAlpnProtos );
        }
    }
    else
    {
        networkStatus = _httpsConnection->pNetworkInterface->create( &networkServerInfo,
            NULL,
            &( _httpsConnection->pNetworkConnection ) );
    }

    /* If memory allocation for the NULL terminated host name succeeded, then free that memory. */
    if( pHostName != NULL )
    {
        free( pHostName );
    }

    /* Check to see if the network connection succeeded. If we did not succeed then the connHandle
       will be NULL and we return an error. */
    if( networkStatus != IOT_NETWORK_SUCCESS )
    {
        IotLogError( "Failed to connect to the server at %.*s on port %d with error: %d",
            pConnConfig->addressLen,
            pConnConfig->pAddress,
            pConnConfig->port,
            networkStatus );
        return IOT_HTTPS_CONNECTION_ERROR;
    }

    /* The recieve callback tells the task context handling the request/response that the network
       is ready to read from. */
    networkStatus = _httpsConnection->pNetworkInterface->setReceiveCallback( _httpsConnection->pNetworkConnection,
        IotHttpsClient_ReceiveCallback,
        _httpsConnection );
    if( networkStatus != IOT_NETWORK_SUCCESS )
    {
        IotLogError( "Failed to connect to set the HTTPS receive callback. " );
        return IOT_HTTPS_INTERNAL_ERROR;
    }

    /* Connection was successful, so create semaphores. */
    /* The overall lock on the connection is set to 1 to start so that the first
       thread to need to use it can use it. */
    if( IotSemaphore_Create( &( _httpsConnection->connSem ), 1, 1 ) == false )
    {
        IotLogError("Failed to create an internal connection semaphore.");
        return IOT_HTTPS_INTERNAL_ERROR;
    }

    /* The reading start semaphore is initialized to 0 so that the network context can
       set it to 1 for a thread to read from the socket. */
    if( IotSemaphore_Create( &( _httpsConnection->rxStartSem ), 0, 1 ) == false )
    {
        IotLogError( "Failed to create an internal connection semaphore." );
        return IOT_HTTPS_INTERNAL_ERROR;
    }

    /* The reading finished semaphore is initialized to 0 that the application context
       can set it to 1 for the network context to return from the read ready callback. */
    if( IotSemaphore_Create( &( _httpsConnection->rxFinishSem ), 0, 1 ) == false )
    {
        IotLogError( "Failed to create an internal connection semaphore." );
        return IOT_HTTPS_INTERNAL_ERROR;
    }

    /* Return the new connection information. */
    *pConnHandle = _httpsConnection;

    return IOT_HTTPS_OK;
}

static IotHttpsReturnCode_t _networkDisconnect(_httpsConnection_t* _httpsConnection)
{
    IotNetworkError_t networkStatus = IOT_NETWORK_SUCCESS;
    networkStatus = _httpsConnection->pNetworkInterface->close( _httpsConnection->pNetworkConnection );

    if ( networkStatus != IOT_NETWORK_SUCCESS )
    {
        IotLogError("Failed to shutdown the socket with error code: %d", networkStatus );
        return IOT_HTTPS_CONNECTION_ERROR;
    }

    return IOT_HTTPS_OK;
}

IotHttpsReturnCode_t IotHttpsClient_Disconnect(IotHttpsConnectionHandle_t connHandle)
{
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;
    status = _networkDisconnect(connHandle);
    if( status != IOT_HTTPS_OK )
    {
        IotLogWarn("Failed to disconnect from the server with error code: %d", status);
    }
    IotSemaphore_Destroy( &(connHandle->connSem) );
    IotSemaphore_Destroy( &(connHandle->rxStartSem) );
    IotSemaphore_Destroy( &(connHandle->rxFinishSem) );
    return status;
}

IotHttpsReturnCode_t _addHeader(IotHttpsRequestHandle_t reqHandle, char * pName, char * pValue, uint32_t len )
{
    uint32_t additionalLength = strlen( pName ) + strlen( ": " ) + len + strlen( "\r\n" );
    uint32_t possibleLastHeaderAdditionalLength = strlen(HTTPS_END_OF_HEADER_LINES_INDICATOR);
    if( additionalLength + possibleLastHeaderAdditionalLength + reqHandle->headersCur > reqHandle->headersEnd )
    {
        IotLogError( "There is %d space left in the header buffer, but we want to add %d more of header.",
            reqHandle->headersEnd - reqHandle->headersCur,
            additionalLength + possibleLastHeaderAdditionalLength );
        return IOT_HTTPS_INSUFFICIENT_MEMORY;
    }

    /* numWritten does not include the NULL terminating character. */
    int numWritten = snprintf(reqHandle->headersCur,
                        additionalLength + 1, /* +1 for the the null terminating character written by snprintf(). */
                        "%s: %.*s\r\n",
                        pName,
                        len,
                        pValue);
    if( numWritten < 0 )
    {
        IotLogError("Failed to write \"%s: %.*s\r\n\" to the request user buffer. Error code: %d",
                        pName,
                        len,
                        pValue,
                        numWritten);
        return IOT_HTTPS_INTERNAL_ERROR;
    }
    reqHandle->headersCur += numWritten;
    IotLogDebug("Wrote header: \"%s: %.*s\r\n\". Space left in request user buffer: %d", 
            pName,
            len,
            pValue,
            reqHandle->headersEnd - reqHandle->headersCur);

    return IOT_HTTPS_OK;
}

IotHttpsReturnCode_t IotHttpsClient_InitializeRequest(IotHttpsRequestHandle_t * pReqHande, IotHttpsRequestInfo_t *pReq)
{
    if( ( pReqHande == NULL) || ( pReq == NULL) )
    {
        return IOT_HTTPS_INVALID_PARAMETER;
    }

    /* Set the request contet to the startof the userbuffer. */
    _httpsRequest_t * _httpsRequest = ( _httpsRequest_t *)((*pReq).reqUserBuffer.pBuffer);
    if( _httpsRequest == NULL )
    {
        /* TODO: Allocate memory for the request buffer. */
    }

    /* Set the start of the headers to the end of the request context in the user buffer. */
    _httpsRequest->headers = (uint8_t*)_httpsRequest + sizeof( _httpsRequest_t );
    _httpsRequest->headersEnd = (uint8_t*)_httpsRequest + ( *pReq ).reqUserBuffer.bufferLen;
    _httpsRequest->headersCur = _httpsRequest->headers;

    /* Add the request line to the header buffer. The path in the req is not NULL terminated so we write each
       part individually. */
    size_t additionalLength = strlen(httpsMethodStrings[(*pReq ).method]) + strlen(" ") + (*pReq).pathLen + strlen(" ") + strlen("HTTP/1.1\r\n");
    if( additionalLength + _httpsRequest->headersCur > _httpsRequest->headersEnd )
    {
        IotLogError("Request line does not fit into the request user buffer: \"%s %.*s HTTP/1.1\\r\\n\" . ",
            httpsMethodStrings[IOT_HTTPS_METHOD_GET],
            (*pReq).pathLen,
            (*pReq).pPath);
        IotLogError( "The length needed is %d and the space availible is %d.", additionalLength, _httpsRequest->headersEnd - _httpsRequest->headersCur );
        return IOT_HTTPS_INSUFFICIENT_MEMORY;
    }

    int numWritten = snprintf(_httpsRequest->headersCur,
                              additionalLength + 1,
                              "%s %.*s HTTP/1.1\r\n",
                              httpsMethodStrings[(*pReq ).method],
                              (*pReq).pathLen,
                              (*pReq).pPath);
    if(numWritten < 0)
    {
        IotLogError("Failed to write Request line into the request user buffer: \"%s %.*s HTTP/1.1\\r\\n\" . Error code: %d",
            httpsMethodStrings[( *pReq ).method],
            (*pReq).pathLen,
            (*pReq).pPath,
            numWritten);
        return IOT_HTTPS_INTERNAL_ERROR;
    }
    _httpsRequest->headersCur += numWritten;

    /* Add the required headers. */
    IotHttpsReturnCode_t addHeaderStatus = IOT_HTTPS_OK;
    addHeaderStatus = _addHeader(_httpsRequest, "User-Agent", IOT_HTTPS_USER_AGENT, strlen( IOT_HTTPS_USER_AGENT ));
    if( addHeaderStatus != IOT_HTTPS_OK )
    {
        IotLogError("Failed to write header to the request user buffer: \"User-Agent: %s\\r\\n\" . Error code: %d", 
            IOT_HTTPS_USER_AGENT, 
            addHeaderStatus);
        return addHeaderStatus;
    }
    addHeaderStatus = _addHeader(_httpsRequest, "Host", ( *pReq ).pHost, ( *pReq ).hostLen);
    
    if( addHeaderStatus != IOT_HTTPS_OK )
    {
        IotLogError("Failed to write \"Host: %.*s\\r\\n\" to the request user buffer. Error code: %d", 
            ( *pReq ).hostLen, 
            ( *pReq ).pHost, 
            addHeaderStatus);
        return addHeaderStatus;
    }

    /* If this is a synchronous request then save where the body is stored. */
    _httpsRequest->body = ( *pReq ).pSyncInfo->pReqData;
    _httpsRequest->bodyLength = ( *pReq ).pSyncInfo->reqDataLen;

    /* If this is an asynchronous request then save the callbacks to use. */
    _httpsRequest->callbacks = &(( *pReq ).pAsyncInfo->callbacks);
    _httpsRequest->userPrivData = ( *pReq ).pAsyncInfo->pPrivData;
    _httpsRequest->contentLength = 0;

    /* Save the connection info if the connection is to be made at the time of the request. */
    _httpsRequest->pConnInfo = ( *pReq ).pConnInfo;

    /* Initialize the corresponding response to this request. */
    _httpsRequest->pRespHandle = (_httpsResponse_t *)(( *pReq ).respUserBuffer.pBuffer);

    _httpsRequest->pRespHandle->body = ( *pReq ).pSyncInfo->pRespData;
    _httpsRequest->pRespHandle->bodyCur = _httpsRequest->pRespHandle->body;
    _httpsRequest->pRespHandle->bodyEnd = _httpsRequest->pRespHandle->body + ( *pReq ).pSyncInfo->respDataLen;

    _httpsRequest->pRespHandle->headers = (uint8_t*)(_httpsRequest->pRespHandle) + sizeof(_httpsResponse_t);
    _httpsRequest->pRespHandle->headersEnd = (uint8_t*)(_httpsRequest->pRespHandle) + ( *pReq ).respUserBuffer.bufferLen;
    _httpsRequest->pRespHandle->headersCur = _httpsRequest->pRespHandle->headers;

    _httpsRequest->pRespHandle->bodyRxStatus = IOT_HTTPS_OK;
    _httpsRequest->pRespHandle->cancelled = false;
    _httpsRequest->pRespHandle->bodyStartInHeaderBuf = NULL;
    _httpsRequest->pRespHandle->bodyLengthInHeaderBuf = 0;
    _httpsRequest->pRespHandle->status = 0;
    _httpsRequest->pRespHandle->contentLength = 0;
    
    /* Reinitialize the parser and set the fill buffer state to empty. */
    http_parser_init(&(_httpsRequest->pRespHandle->httpParser), HTTP_RESPONSE);

    *pReqHande = _httpsRequest;
    return IOT_HTTPS_OK;
}

IotHttpsReturnCode_t IotHttpsClient_AddHeader( IotHttpsRequestHandle_t reqHandle, char * pName, char * pValue, uint32_t len )
{
    /* Check for NULL pointer paramters. */
    if( (pName == NULL) || (pValue == NULL) || (reqHandle == NULL) )
    {
        IotLogError("Null parameter passed into IotHttpsClient_AddHeader.");
        return IOT_HTTPS_INVALID_PARAMETER;
    }

    /* Check for auto-generated header "Content-Length". This header is created and send automatically when right before
       request body is sent on the network. */
    if( strncmp(pName, HTTPS_CONTENT_LENGTH_HEADER, strlen(HTTPS_CONTENT_LENGTH_HEADER)) == 0)
    {
        IotLogError("Attempting to add auto-generated header %s. This is not allowed.", HTTPS_CONTENT_LENGTH_HEADER);
        return IOT_HTTPS_INVALID_PARAMETER;
    }

    /* Check for auto-generated header "Content-Length". This header is created and send automatically when right before
       request body is sent on the network. */
    if( strncmp(pName, HTTPS_CONNECTION_HEADER, strlen(HTTPS_CONNECTION_HEADER)) == 0)
    {
        IotLogError("Attempting to add auto-generated header %s. This is not allowed.", HTTPS_CONNECTION_HEADER);
        return IOT_HTTPS_INVALID_PARAMETER;
    }

    /* Check for auto-generated header "Host". This header is created and placed into the header buffer space
       in IotHttpClient_InitializeRequest(). */
    if( strncmp(pName, HTTPS_HOST_HEADER, strlen(HTTPS_HOST_HEADER)) == 0)
    {
        IotLogError("Attempting to add auto-generated header %s. This is not allowed.", HTTPS_HOST_HEADER);
        return IOT_HTTPS_INVALID_PARAMETER;
    }

    /* Check for auto-generated header "User-Agent". This header is created and placed into the header buffer space
       in IotHttpClient_InitializeRequest(). */
    if(strncmp(pName, HTTPS_USER_AGENT_HEADER, strlen(HTTPS_USER_AGENT_HEADER)) == 0)
    {
        IotLogError("Attempting to add auto-generated header %s. This is not allowed.", HTTPS_USER_AGENT_HEADER);
        return IOT_HTTPS_INVALID_PARAMETER;
    }

    return _addHeader(reqHandle, pName, pValue, len );
}

static IotHttpsReturnCode_t _networkSend(_httpsConnection_t* _httpsConnection, uint8_t * buf, size_t len)
{
    int32_t numBytesSent = 0;
    int32_t numBytesSentTotal = 0;
    int32_t sendLength = len;

    /* Send the headers first because the HTTPS body is in a separate pointer these are not contiguous. */
    while( numBytesSentTotal < sendLength )
    {
        numBytesSent = _httpsConnection->pNetworkInterface->send( _httpsConnection->pNetworkConnection, 
            &( buf[numBytesSentTotal] ), 
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
        return IOT_HTTPS_NETWORK_ERROR;
    }

    return IOT_HTTPS_OK;
}

static IotHttpsReturnCode_t _networkRecv( _httpsConnection_t* _httpsConnection, uint8_t * buf, size_t bufLen)
{
    int32_t numBytesRecv = 0;
    int32_t numBytesRecvTotal = 0;
    int32_t lengthToReceive = bufLen;

    do {
        numBytesRecv = _httpsConnection->pNetworkInterface->receive( _httpsConnection->pNetworkConnection,
            &( buf[ numBytesRecvTotal ] ),
            lengthToReceive - numBytesRecvTotal );

        if( numBytesRecv > 0 )
        {
            numBytesRecvTotal += numBytesRecv;
        }
        if( numBytesRecv == 0)
        {
            IotLogError("Timedout waiting for the HTTPS response message.");
            return IOT_HTTPS_TIMEOUT_ERROR;
        }
        if( numBytesRecv < 0)
        {
            IotLogError("Error in receiving the HTTPS response message. Socket Error code %d", numBytesRecv);
            return IOT_HTTPS_NETWORK_ERROR;
        }
    } while((numBytesRecv > 0)  && ( lengthToReceive - numBytesRecvTotal > 0));

    return IOT_HTTPS_OK;   
}

IotHttpsReturnCode_t _sendHttpsHeaders( _httpsConnection_t* _httpsConnection, uint8_t* headersBuf, uint32_t headersLength, bool isNonPersistent, uint32_t contentLength)
{
    IotHttpsReturnCode_t status = _networkSend( _httpsConnection, headersBuf, headersLength);
    if( status != IOT_HTTPS_OK )
    {
        IotLogError("Error sending the HTTPS headers in the request user buffer. Error code: %d", status);
        return status;
    }

    char finalHeadersKeepAlive[HTTPS_MAX_CONTENT_LENGTH_LINE_LENGTH + HTTPS_CONNECTION_KEEP_ALIVE_HEADER_LINE_LENGTH + 1] = { 0 };
    const char* connectionHeader;
    if (isNonPersistent)
    {
        connectionHeader = HTTPS_CONNECTION_CLOSE_HEADER_LINE;
    }
    else
    {
        connectionHeader = HTTPS_CONNECTION_KEEP_ALIVE_HEADER_LINE;
    }

    if (contentLength > 0)
    {
        snprintf(finalHeadersKeepAlive, 
            sizeof(finalHeadersKeepAlive), 
            "%sContent-Length: %d\r\n\r\n",
            connectionHeader,
            contentLength);
    }
    else 
    {
        snprintf(finalHeadersKeepAlive, 
            sizeof(finalHeadersKeepAlive), 
            "%s\r\n",
            connectionHeader);
    }
    status = _networkSend( _httpsConnection, finalHeadersKeepAlive, strlen(finalHeadersKeepAlive));
    if( status != IOT_HTTPS_OK )
    {
        IotLogError("Error sending final HTTPS Headers \r\n%s. Error code: %d", finalHeadersKeepAlive, status);
        return status;
    }

    return IOT_HTTPS_OK;

}

IotHttpsReturnCode_t _sendHttpsBody( _httpsConnection_t* _httpsConnection, uint8_t* bodyBuf, uint32_t bodyLength)
{
    IotHttpsReturnCode_t status = _networkSend( _httpsConnection, bodyBuf, bodyLength);
    if( status != IOT_HTTPS_OK )
    {
        IotLogError("Error sending final HTTPS body at location 0x%x. Error code: %d", bodyBuf, status);
    }
    return status;
}

IotHttpsReturnCode_t _parseHttpsMessage(http_parser* pHttpParser, char* buf, size_t len)
{
    size_t parsedBytes = http_parser_execute(pHttpParser, &_httpParserSettings, buf, len );
    /* If the parser fails with HPE_CLOSED_CONNECTION or HPE_INVALID_CONSTANT that simply means there
       was data beyond the end of the message. We do not fail in this case because we often give the whole
       header buffer or body buffer to the parser even if it is only partly filled with data. 
       HPE_CB_message_begin <= error <= HPE_CB_chunk_complete covers all errors for returning a non-zer value 
       from the parser callbacks. I return a nonzero from the callback when I want to stop the parser early like
       for a HEAD request or when we find part of the response body in the header buffer. */
    if(pHttpParser->http_errno != 0 &&
        ( HTTP_PARSER_ERRNO( pHttpParser ) != HPE_CLOSED_CONNECTION ) &&
        ( HTTP_PARSER_ERRNO( pHttpParser ) != HPE_INVALID_CONSTANT ) &&
        ( ( HTTP_PARSER_ERRNO( pHttpParser ) >= HPE_CB_message_begin) &&
          ( HTTP_PARSER_ERRNO( pHttpParser ) <= HPE_CB_chunk_complete) ))
    {
        const char * httpParserErrorDescription = http_errno_description( HTTP_PARSER_ERRNO( pHttpParser ) );
        IotLogError("http_parser failed on the http response with error: %s", httpParserErrorDescription);
        return IOT_HTTPS_PARSING_ERROR;
    }

    return IOT_HTTPS_OK;
}

IotHttpsReturnCode_t _receiveHttpsMessage( _httpsConnection_t* _httpsConnection, 
    http_parser* parser,
    IotHttpsResponseParserState_t *currentParserState,
    IotHttpsResponseParserState_t finalParserState, 
    uint8_t** buf,
    uint8_t** bufCur,
    uint8_t** bufEnd,
    IotHttpsReturnCode_t *networkStatus)
{    
    while( (*currentParserState < finalParserState) && (*bufEnd - *bufCur > 0) ) 
    {
        *networkStatus = _networkRecv( _httpsConnection,
            *bufCur, 
            *bufEnd - *bufCur);

        /* Case 1: Let's say that we are late in receiving the data on the network and the server closes the connection.
           SOCKETS_Recv() will return some connection closed negative return code, BUT data will still be filled
           inside the header buffer.
           We get a negative error code at the same time as data being filled in the buffer when the whole of the 
           HTTPS message fits in the header buffer. 
           
           Case 2: This is a response following one that did not fit in the last response's buffer. This means data from the last response
           may preceed the status line of the response we want. http-parser will return immediately with HPT_INVALID_CONSTANT error if some
           constant that is NOT the status line is reached. We want to search character by character until the start of the request line is 
           reached. Either this or we flush the socket after each response. */
        IotHttpsReturnCode_t status = _parseHttpsMessage(parser, *bufCur, *bufEnd - *bufCur);
        if(status != IOT_HTTPS_OK)
        {
            IotLogError("Failed to parse the message buffer with error: %d", parser->http_errno);
            return status;
        }

        /* The _httResponse->headersCur pointer is updated in the http_parser callbacks. */
        IotLogDebug( "There is %d of space left in the buffer.", *bufEnd - *bufCur );

        /* We cannot anticipate the unique network error received when the server closes the connection. 
           We simply exit the loop if there nothing else to receive. We do not return the network status because 
           the error may just be a server closed the connection, but there may still have been data in the buffer
           that we parsed. */
        if( *networkStatus != IOT_HTTPS_OK )
        {
            IotLogError( "Error receiving the HTTPS response headers. Error code: %d", status );
            break;
        }

    }

    /* If we did not reach the end of the headers with the current header buffer size then log that info. */
    if( *currentParserState < finalParserState)
    {
        IotLogDebug("There are still more data on the network. It could not fit into buffer at \
            location 0x%x with length %d.",
            *buf,
            *bufEnd - *buf);
    }

    return IOT_HTTPS_OK;
}

IotHttpsReturnCode_t _receiveHttpsHeaders( _httpsConnection_t* _httpsConnection, _httpsResponse_t* _httpsResponse, IotHttpsReturnCode_t *networkStatus)
{
    /* Initialize the HTTPS parser for a new execution. Incase we are reusing the
    the request without re-initialization. */
    http_parser_init(&(_httpsResponse->httpParser), HTTP_RESPONSE);
    _httpsResponse->httpParser.data = (void *)(_httpsResponse);
    _httpsResponse->parserState = eIotHttpsParserNone;
    _httpsResponse->bufferState = eIotHttpsHeaderBuffer;

    IotHttpsReturnCode_t status = _receiveHttpsMessage(_httpsConnection,
        &(_httpsResponse->httpParser),
        &(_httpsResponse->parserState),
        eIotHttpsParserHeadersComplete,
        &(_httpsResponse->headers),
        &(_httpsResponse->headersCur),
        &(_httpsResponse->headersEnd),
        networkStatus);

    /* Whether the parser complete the headers or not we want to see if we got the Content-Length. */
    _httpsResponse->contentLength = _httpsResponse->httpParser.content_length;

    return status;
}

/* _receiveHttpsHeaders() must be called first before this function is called. */
IotHttpsReturnCode_t _receiveHttpsBody( _httpsConnection_t* _httpsConnection, _httpsResponse_t* _httpsResponse, IotHttpsReturnCode_t *networkStatus)
{
    _httpsResponse->bufferState = eIotHttpsBodyBuffer;

    // TODO: When there is headers in te buffer check the buffering. 
    IotHttpsReturnCode_t status = _receiveHttpsMessage(_httpsConnection,
        &(_httpsResponse->httpParser),
        &(_httpsResponse->parserState),
        eIotHttpsParserBodyComplete,
        &(_httpsResponse->body),
        &(_httpsResponse->bodyCur),
        &(_httpsResponse->bodyEnd),
        networkStatus);

    IotLogDebug("The message Content-Length is %d (Will be > 0 for a Content-Length header existing). The remaining content length on the network is %d.",
        _httpsResponse->contentLength,
        _httpsResponse->httpParser.content_length);

    return IOT_HTTPS_OK;
}

static IotHttpsReturnCode_t _flushHttpsNetworkData( _httpsConnection_t* _httpsConnection, _httpsResponse_t* _httpsResponse )
{
    static uint8_t flushBuffer[IOT_HTTPS_MAX_FLUSH_BUFFER_SIZE];
    /* Even if there is not body, the parser state will become body complete after the headers finish. */
    while( _httpsResponse->parserState < eIotHttpsParserBodyComplete )
    {
        IotLogDebug( "Now clearing the rest of the response data on the socket. " );
        IotHttpsReturnCode_t networkStatus = _networkRecv( _httpsConnection, flushBuffer, IOT_HTTPS_MAX_FLUSH_BUFFER_SIZE );

        /* Run this through the parser so that we can get the end of the HTTP message, instead of simply timing out the socket to stop.
           If we relied on the socket timeout to stop reading the network socket, then the server may close the connection. */
        IotHttpsReturnCode_t parserStatus = _parseHttpsMessage(&(_httpsResponse->httpParser), flushBuffer, IOT_HTTPS_MAX_FLUSH_BUFFER_SIZE );
        if(parserStatus != IOT_HTTPS_OK)
        {
            const char * httpParserErrorDescription = http_errno_description( HTTP_PARSER_ERRNO( &_httpsResponse->httpParser ) );
            IotLogError("Failed to parse the response body buffer with error: %d", _httpsResponse->httpParser.http_errno);
            return parserStatus;
        }

        /* If there is a network error then we want to stop clearing out the buffer. */
        if( networkStatus != IOT_HTTPS_OK )
        {
            IotLogError( "Error receiving the HTTPS response headers. Error code: %d", networkStatus );
            break;
        }
    }
    return IOT_HTTPS_OK;
}

IotHttpsReturnCode_t IotHttpsClient_SendSync(IotHttpsConnectionHandle_t *pConnHandle, IotHttpsRequestHandle_t reqHandle, IotHttpsResponseHandle_t * pRespHandle)
{
    IOT_FUNCTION_ENTRY(IotHttpsReturnCode_t, IOT_HTTPS_OK);

    _httpsResponse_t * _httpsResponse = reqHandle->pRespHandle;
    *pRespHandle = _httpsResponse;

    /* If there is connection info in the request handle, then make the connection now. */
    if( reqHandle->pConnInfo != NULL )
    {
        IotHttpsClient_Connect(&(reqHandle->pConnHandle), reqHandle->pConnInfo);
    }
    else
    {
        reqHandle->pConnHandle = *pConnHandle;
    }

    /* Set the connection handles to return. */
    *pConnHandle = reqHandle->pConnHandle;
    *pRespHandle = reqHandle->pRespHandle;
    ( *pRespHandle )->pConnHandle = *pConnHandle;
    ( *pRespHandle )->pReqHandle = reqHandle;

    _httpsConnection_t * _httpsConnection = *pConnHandle;

    /* This response is being fetch synchronously. */
    _httpsResponse->isAsync = false;

    if( IotSemaphore_TimedWait( &(_httpsConnection->connSem), IOT_HTTPS_MAX_CONN_USAGE_WAIT_MS ) == true )
    {
        /* Send the headers first. Because the HTTPS body is in a separate pointer. */
        status = _sendHttpsHeaders( _httpsConnection,
            reqHandle->headers,
            reqHandle->headersCur - reqHandle->headers,
            _httpsConnection->nonPersistent,
            reqHandle->bodyLength);

        if( status != IOT_HTTPS_OK )
        {
            IotLogError("Error sending the HTTPS headers in the request user buffer. Error code: %d", status);
            IOT_GOTO_CLEANUP();
        }

        /* Send the body now if it exists. */
        if(reqHandle->body != NULL)
        {
            status = _sendHttpsBody( _httpsConnection, reqHandle->body, reqHandle->bodyLength);
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

        IotHttpsReturnCode_t networkStatus;
        status = _receiveHttpsHeaders( _httpsConnection, _httpsResponse, &networkStatus);
        if(status != IOT_HTTPS_OK)
        {
            IotLogError("Error in receiving the HTTPS headers into user header buffer at 0x%x. Error code: %d.", _httpsResponse->headers, status);
            IOT_GOTO_CLEANUP(); 
        }

        if( _httpsResponse->parserState < eIotHttpsParserHeadersComplete )
        {
            IotLogDebug( "Headers recieved on the network did not all fit into the configured header buffer. \
                The length of the headers buffer is: %d",
                _httpsResponse->headersEnd - _httpsResponse->headers );
            /* It is not error if the headers did not all fit into the buffer. */
            //status = IOT_HTTPS_MESSAGE_TOO_LARGE;

            /* If we are not expecting a body, then we want to flush the socket. */
            if( _httpsResponse->body == NULL )
            {
                IOT_GOTO_CLEANUP();
            }
        }

        /* The header buffer is now filled or the end of the headers has been reached already. If part of the response
           body was read from the network into the header buffer, then it was already copied to the body buffer in the 
           _httpParserOnBodyCallback(). */
        if(_httpsResponse->body != NULL)
        {
            /* If there is room left in the body buffer, then try to receive more. */
            if( _httpsResponse->bodyEnd - _httpsResponse->bodyCur > 0 )
            {
                IotHttpsReturnCode_t networkStatus;
                status = _receiveHttpsBody( _httpsConnection,
                    _httpsResponse,
                    &networkStatus );
                if( status != IOT_HTTPS_OK )
                {
                    IotLogError( "Error receiving the HTTPS response body. Error code: %d", status );
                    IOT_GOTO_CLEANUP();
                }
            }

            /* If we don't reach the end of the HTTPS body in the parser, then we only received part of the body.
               The rest of body will be on the socket. */
            if( _httpsResponse->parserState < eIotHttpsParserBodyComplete )
            {
                IotLogError( "HTTPS response body does not fit into application provided response buffer at location \
                0x%x with length: %d",
                    _httpsResponse->body,
                    _httpsResponse->bodyEnd - _httpsResponse->body );
                status = IOT_HTTPS_MESSAGE_TOO_LARGE;
                IOT_GOTO_CLEANUP();
            }
        }

        IOT_FUNCTION_CLEANUP_BEGIN();

        /* If this is not a persistent connection, the server would have closed it after
           sending a response, but we disconnect anyways. */
        if( _httpsConnection->nonPersistent )
        {
            status = IotHttpsClient_Disconnect( _httpsConnection );
            if( status != IOT_HTTPS_OK )
            {
                IotLogError( "Failed to disconnected from the server with error: %d", status );
                /* TODO: Not sure if we should return this status yet. */
            }
        }

        /* Set the buffer state to finished for both success or error. This is so that the application can still
           read headers copied even if there was an error. eIotHttpsFinished state is checked in the https parsing 
           callacks to know if we are currently parsing a new response or we are simply parsing to read header values.*/
        _httpsResponse->bufferState = eIotHttpsFinished;

        /* Flush the socket of the rest of the data if there is data left from this response. We need to do this
           so that for the next request on this connection, there is not left over response from this request in
           the next response buffer.

           If a continuous stream of data is coming in from the connection, with an unknown end, we may not be able to
           flush the network data. It may sit here forever. A continuous stream should be ingested with the async workflow. */
        _flushHttpsNetworkData( _httpsConnection, _httpsResponse );

        /* Let the network thread know that reading from the socket is finished. Whatever context called
           the network receive callback will return from that callback now. */
        IotSemaphore_Post( &( _httpsConnection->rxFinishSem ) );
        /* Let other threads waiting know the connection is ready to be used. */
        IotSemaphore_Post( &(_httpsConnection->connSem) );
        IOT_FUNCTION_CLEANUP_END();
    }
    else
    {
        IotLogError("Timed out in sync send waiting on the connection handle to be free. The current timeout is %d.", IOT_HTTPS_MAX_CONN_USAGE_WAIT_MS);
        return IOT_HTTPS_BUSY;
    }
    
    return IOT_HTTPS_OK;
}

IotHttpsReturnCode_t IotHttpsClient_WriteRequestBody(IotHttpsRequestHandle_t reqHandle, char *pBuf, uint32_t len, int isComplete)
{
    /* If the Content-Length is greater than 0, then we already send the headers. */
    if(reqHandle->contentLength > 0)
    {
        IotLogError("Error already sent a \"Content-Length\" header and the following body to the server. This \
            function cannot be called twice in the same request-response flow.");
        return IOT_HTTPS_MESSAGE_FINISHED;
    }
    else
    {
        reqHandle->contentLength = len;
    }
    
    /* Send the headers first then send the body. */
    IotHttpsReturnCode_t status =  _sendHttpsHeaders( reqHandle->pConnHandle,
        reqHandle->headers, 
        reqHandle->headersCur - reqHandle->headers,
        reqHandle->pConnHandle->nonPersistent,
        reqHandle->contentLength);
    
    if( status != IOT_HTTPS_OK )
    {
        IotLogError("Failed to send the HTTPS headers in the request user buffer. Error code %d", status);
        return status;
    }

    

    /* Next we send the body. */
    status = _sendHttpsBody(reqHandle->pConnHandle, pBuf, len);
    if( status != IOT_HTTPS_OK )
    {
        IotLogError("Failed to send the HTTPS body at buf location 0x%x. Error code %d", pBuf, status);
        return status;
    }
}

IotHttpsReturnCode_t IotHttpsClient_ReadResponseBody(IotHttpsResponseHandle_t respHandle, char * pBuf, uint32_t *pLen)
{
    IotHttpsReturnCode_t status = IOT_HTTPS_OK;
    if(respHandle->isAsync)
    {
        /* Set the current body in the respHandle to use in _receiveHttpsBody(). _receiveHttpsBody is generic
           to both async and sync request/response handling. In the sync version the body is configured during
           initializing the request. In the async version the body is given in this function on the fly. */
        respHandle->body = pBuf;
        respHandle->bodyCur = respHandle->body;
        respHandle->bodyEnd = respHandle->bodyCur + *pLen;
        /* When there is part of the body in the header pBuffer. We need to move that data to this body pBuffer 
           provided in this fuction. */
        if(respHandle->bodyStartInHeaderBuf != NULL)
        {
            uint32_t copyLength = respHandle->bodyLengthInHeaderBuf > *pLen ? *pLen : respHandle->bodyLengthInHeaderBuf;
            memcpy(respHandle->bodyCur, respHandle->bodyStartInHeaderBuf, copyLength);
            respHandle->bodyCur += copyLength;
        }
        if(respHandle->bodyEnd - respHandle->bodyCur > 0)
        {
            status = _receiveHttpsBody(respHandle->pConnHandle, respHandle, &(respHandle->bodyRxStatus) );
        }
        *pLen = respHandle->bodyCur - respHandle->body;
    }
    else
    {
        /* This is not valid for a synchronous response. Applications need reference the pBuffer in 
        IotHttpsRequestInfo_t.pSyncInfo_t.*/
        IotLogError("Called IotHttpsClient_ReadResponseBody() on a synchronous response. This is valid only for an asynchronous response.");
        return IOT_HTTPS_INVALID_PARAMETER;
    }
    return status;
}

IotHttpsReturnCode_t IotHttpsClient_CancelRequestAsync(IotHttpsRequestHandle_t reqHandle, IotHttpsResponseHandle_t respHandle)
{
    if( respHandle != NULL )
    {
        respHandle->cancelled = true;
    }
    else if( reqHandle != NULL )
    {
        reqHandle->cancelled = true;
    }
    else
    {
        IotLogError( "Both parameters to IotHttpsClient_CancelRequestAsync are NULL." );
        return IOT_HTTPS_INVALID_PARAMETER;
    }
    return IOT_HTTPS_OK;
}

void _IotHttps_ReceiveResponseAsync( IotTaskPool_t pTaskPool, IotTaskPoolJob_t pJob, void * pUserContext )
{
    /*
     * In MQTT the taskpool sends the MQTT payload for a publish over the network.
     * The taskpool is used to call the publish receive callback defined by the application.
     * When a publish is received on the network, the network abstraction task de-serializes and allocates 
     * memory for the packet and then assigns the passing the data to the application to a taskpool task.
     */

    IOT_FUNCTION_ENTRY( IotHttpsReturnCode_t, IOT_HTTPS_OK );

    /* The current implementation does not allow pipelining. */
    _httpsRequest_t* _httpsRequest = (_httpsRequest_t*)( pUserContext );
    _httpsConnection_t* _httpsConnection = _httpsRequest->pConnHandle;
    _httpsResponse_t* _httpsResponse = _httpsRequest->pRespHandle;

    IotLogDebug( "Task with request ID: %d started.", _httpsRequest );

    if(_httpsResponse->cancelled == true)
    {
        IotLogDebug("Request ID: %d was cancelled.", _httpsRequest );
        return;
    }

    if( IotSemaphore_TimedWait( &(_httpsConnection->connSem), IOT_HTTPS_MAX_CONN_USAGE_WAIT_MS ) == pdTRUE )
    {
        /* Get the headers from the application. */
        if(_httpsRequest->callbacks->appendHeaderCallback != NULL)
        {
            _httpsRequest->callbacks->appendHeaderCallback(_httpsRequest->userPrivData, _httpsRequest);
        }

        if(_httpsResponse->cancelled == true)
        {
            IotLogDebug("Request ID: %d was cancelled.", _httpsRequest );
            IOT_GOTO_CLEANUP();
            return;
        }

        /* Ask the user for data to write to the network. We only ask the user once. This is so that
           we can calculate the Content-Length to send.*/
        if(_httpsRequest->callbacks->writeCallback != NULL)
        {
            /* If there is data, then a Content-Length header value will be provided and we sent the headers
               before that user data. */
            _httpsRequest->callbacks->writeCallback(_httpsRequest->userPrivData, _httpsRequest);
            /* Now that the body has been sent we can reset this request's content length to 0. */
            _httpsRequest->contentLength = 0;
        }
        else
        {
            status = _sendHttpsHeaders(_httpsConnection,
                _httpsRequest->headers,
                _httpsRequest->headersCur - _httpsRequest->headers,
                _httpsConnection->nonPersistent,
                0);
            if( status != IOT_HTTPS_OK )
            {
                IotLogError("Error sending the HTTPS headers with error code: %d", status);
                IOT_GOTO_CLEANUP();
            }
        }

        if(_httpsResponse->cancelled == true)
        {
            IotLogDebug("Request ID: %d was cancelled.", _httpsRequest );
            IOT_GOTO_CLEANUP();
            return;
        }

        /* Wait for the network to have data to read. */
        if( IotSemaphore_TimedWait( &( _httpsConnection->rxStartSem ), _httpsConnection->timeout ) == false )
        {
            IotLogError( "Timed out waiting for a response from the network." );
            status = IOT_HTTPS_TIMEOUT_ERROR;
            IOT_GOTO_CLEANUP();
        }

        /* Receive the response from the network. */
        /* Receive the headers first. */
        IotHttpsReturnCode_t networkStatus;
        status = _receiveHttpsHeaders(_httpsConnection, _httpsResponse, &networkStatus);
        if( status != IOT_HTTPS_OK )
        {
            IotLogError("Error receiving the HTTPS headers with error code: %d", status);
            IOT_GOTO_CLEANUP();
        }

        if(_httpsResponse->cancelled == true)
        {
            IotLogDebug("Request ID: %d was cancelled.", _httpsRequest );
            IOT_GOTO_CLEANUP();
            return;
        }

        /* Receive the body. */
        if(_httpsRequest->callbacks->readReadyCallback)
        {
            /* If there is still more body that we have not passed back to the user, then we need to call the callback again. */
            do {
                _httpsRequest->callbacks->readReadyCallback(_httpsRequest->userPrivData, 
                    _httpsResponse, 
                    _httpsResponse->bodyRxStatus, 
                    _httpsResponse->status);
                if(_httpsResponse->cancelled == true)
                {
                    IotLogDebug("Cancelled HTTP request. Now going to flush the betwork buffer.");
                    IOT_GOTO_CLEANUP();;
                }
            } while(_httpsResponse->parserState < eIotHttpsParserBodyComplete && _httpsResponse->bodyRxStatus == IOT_HTTPS_OK);
        }
        else
        {
            
               _flushHttpsNetworkData(_httpsConnection, _httpsResponse);
        }

        IOT_FUNCTION_CLEANUP_BEGIN();
        
        /* If this is not a persistent connection, the server would have closed it after 
           sending a response, but we disconnect anyways. */
        if(_httpsConnection->nonPersistent)
        {
            status = IotHttpsClient_Disconnect(_httpsConnection);
            if( status != IOT_HTTPS_OK )
            {
                IotLogError("Failed to disconnected from the server with error: %d", status);
                /* Do not exit after an error here. The disconnect should always send a FIN or RST to the server
                   even if we are already disconnected. */
            }
            if(_httpsRequest->callbacks->connectionClosedCallback)
            {
                _httpsRequest->callbacks->connectionClosedCallback(_httpsRequest->userPrivData, _httpsConnection, status);
            }
        }

        /* There may still be headers on the network socket that we have not read. We will want to flush
           the network buffer before the next response. */
        _flushHttpsNetworkData(_httpsConnection, _httpsResponse);

        /* Let the network thread know that reading from the socket is finished. Whatever context called
           the network receive callback will return from that callback now. */
        IotSemaphore_Post( &( _httpsConnection->rxFinishSem ) );

        /* Let other threads waiting know the connection is ready to be used. */
        IotSemaphore_Post( &( _httpsConnection->connSem ) );

        /* Return the possible error to the application. */
        if( status != IOT_HTTPS_OK )
        {
            _httpsRequest->callbacks->errorCallback( _httpsRequest->userPrivData, _httpsRequest, status );
        }

        /* Response is complete. */
        if( _httpsRequest->callbacks->responseCompleteCallback )
        {
            _httpsRequest->callbacks->responseCompleteCallback( _httpsRequest->userPrivData, _httpsResponse, _httpsResponse->bodyRxStatus, _httpsResponse->status );
        }

        return;
        /* IOT_FUNCTION_CLEANUP_END(); We are not returning a status. */

    }  
    else
    {
        IotLogError("Timed out in async send waiting on the connection handle to be free. The current timeout is %d.", IOT_HTTPS_MAX_CONN_USAGE_WAIT_MS);
        _httpsRequest->callbacks->errorCallback(_httpsRequest->userPrivData, _httpsRequest, IOT_HTTPS_BUSY);
    }
}

IotHttpsReturnCode_t IotHttpsClient_SendAsync(IotHttpsConnectionHandle_t *pConnHandle, IotHttpsRequestHandle_t reqHandle, IotHttpsResponseHandle_t * pRespHandle)
{
    IotTaskPoolError_t taskPoolStatus = IOT_TASKPOOL_SUCCESS;

    /* If there is connection info in the request handle, then make the connection now. The application developer will want to 
       protect against making a connection twice if re-using a request.*/
    if( reqHandle->pConnInfo != NULL )
    {
        IotHttpsReturnCode_t status = IotHttpsClient_Connect( &(reqHandle->pConnHandle), reqHandle->pConnInfo );
        if(reqHandle->callbacks->connectionEstablishedCallback && status == IOT_HTTPS_OK)
        {
            reqHandle->callbacks->connectionEstablishedCallback(reqHandle->userPrivData, pConnHandle, status);
        }
        if(status != IOT_HTTPS_OK)
        {
            reqHandle->callbacks->errorCallback(reqHandle->userPrivData, reqHandle, status);
        }
    }
    else
    {
        reqHandle->pConnHandle = *pConnHandle;
    }

    /* Set the connection handles to return. */
    *pConnHandle = reqHandle->pConnHandle;
    *pRespHandle = reqHandle->pRespHandle;
    ( *pRespHandle )->pConnHandle = *pConnHandle;
    ( *pRespHandle )->pReqHandle = reqHandle;

    /* The response associated with this request is being fetched asynchronously. */
    (*pRespHandle)->isAsync = true;

    /* Schedule the request. */
    IotLogDebug( "Task with request ID: %d scheduled.", reqHandle );

    //TODO: DO NOT schedule every available task from the task pool. heh.

    taskPoolStatus = IotTaskPool_CreateJob( _IotHttps_ReceiveResponseAsync,
        ( void* )reqHandle,
        &( reqHandle->taskPoolJobStorage ),
        &( reqHandle->taskPoolJob ) );

    /* Creating a task pool job should never fail when parameters are valid. */
    if( taskPoolStatus != IOT_TASKPOOL_SUCCESS )
    {
        IotLogError( "Error creating a taskpool job for receiving the asynchronous HTTPS response. Error code: %d", taskPoolStatus );
        return IOT_HTTPS_INTERNAL_ERROR;
    }

    taskPoolStatus = IotTaskPool_Schedule( IOT_SYSTEM_TASKPOOL, reqHandle->taskPoolJob, 0);
    if( taskPoolStatus != IOT_TASKPOOL_SUCCESS )
    {
        IotLogWarn( "Failed to schedule taskpool job. Error code: %d", taskPoolStatus );
        return IOT_HTTPS_ASYNC_SCHEDULING_ERROR;
    }

    return IOT_HTTPS_OK;
}

IotHttpsReturnCode_t IotHttpsClient_ReadResponseStatus(IotHttpsResponseHandle_t respHandle, uint16_t *status)
{
    if( respHandle->status == 0)
    {
        IotLogError("The HTTP response status was not found inthe HTTP response header buffer.");
        return IOT_HTTPS_NOT_FOUND;
    }
    if( status != NULL)
    {
        *status = respHandle->status;
    }
    else
    {
        IotLogError("NULL status out parameter passed into IotHttpsClient_ReadResponseStatus().");
        return IOT_HTTPS_INVALID_PARAMETER;
    }
    
    return IOT_HTTPS_OK;
}

IotHttpsReturnCode_t IotHttpsClient_ReadHeader(IotHttpsResponseHandle_t respHandle, char *pName, char *pValue, uint32_t len)
{
    respHandle->readHeaderField = pName;
    respHandle->foundHeaderField = false;
    respHandle->bufferState = eIotHttpsHeaderSearch;

    http_parser_init( &( respHandle->httpParser ), HTTP_RESPONSE );
    http_parser_execute(&(respHandle->httpParser), &_httpParserSettings, respHandle->headers, respHandle->headersCur - respHandle->headers);
    if(respHandle->foundHeaderField)
    {
        if(respHandle->readHeaderValueLength > len)
        {
            IotLogError("IotHttpsClient_ReadHeader(): The length of the pValue buffer specified is less than the actual length of the pValue. ");
            return IOT_HTTPS_INSUFFICIENT_MEMORY;
        }
        strncpy(pValue, respHandle->readHeaderValue, respHandle->readHeaderValueLength);
    }
    else
    {
        IotLogError("IotHttpsClient_ReadHeader(): The header field %s was not found.", pName);
        return IOT_HTTPS_NOT_FOUND;
    }
    return IOT_HTTPS_OK;
}

IotHttpsReturnCode_t IotHttpsClient_ReadContentLength(IotHttpsResponseHandle_t respHandle, uint32_t *contentLength)
{   
    if((respHandle == NULL) || (contentLength == NULL))
    {
        IotLogError("NULL parameter passed into IotHttpsClient_ReadContentLength.");
        return IOT_HTTPS_INVALID_PARAMETER;
    }

    /* If there is no content-length header or if we were not able to store it in the header buffer this will be
       invalid. */
    if(respHandle->contentLength == 0)
    {
        IotLogError("The content length not found in the HTTP response header buffer.");
        return IOT_HTTPS_NOT_FOUND;
    }

    *contentLength = respHandle->contentLength;

    return IOT_HTTPS_OK;
}