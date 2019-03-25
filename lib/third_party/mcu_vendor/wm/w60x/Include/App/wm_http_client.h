/**
 * @file    wm_http_client.h
 *
 * @brief   Http client APIs
 *
 * @author  wanghf
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_HTTP_CLIENT_H
#define WM_HTTP_CLIENT_H

#include "wm_config.h"
#include "wm_type_def.h"
#ifdef BOOL
#undef BOOL
#endif
#ifdef UCHAR
#undef UCHAR
#endif
#ifdef CHAR
#undef CHAR
#endif
#ifdef UINT16
#undef UINT16
#endif
#ifdef INT16
#undef INT16
#endif
#ifdef UINT32
#undef UINT32
#endif
#ifdef INT32
#undef INT32
#endif
#ifdef UINT64
#undef UINT64
#endif
#ifdef INT64
#undef INT64
#endif
#ifdef ULONG
#undef ULONG
#endif
#ifdef LONG
#undef LONG
#endif
#define VOID                    void
typedef int BOOL;
typedef unsigned char           UCHAR;
//typedef signed char             CHAR;
typedef char                    CHAR;
typedef unsigned short          UINT16; 
typedef signed short            INT16;
typedef unsigned int            UINT32;
typedef signed int              INT32;
typedef unsigned long long      UINT64;
typedef long long               INT64;
typedef unsigned long           ULONG;
typedef signed long             LONG;


/* HTTP Status, API Return codes */
/** HTTP Success status */
#define HTTP_CLIENT_SUCCESS                 0
/** Unknown error */
#define HTTP_CLIENT_UNKNOWN_ERROR           1
/** an Invalid handle or possible bad pointer was passed to a function */
#define HTTP_CLIENT_ERROR_INVALID_HANDLE    2
/** Buffer too small or a failure while in memory allocation */
#define HTTP_CLIENT_ERROR_NO_MEMORY         3
/** an attempt to use an invalid socket handle was made */
#define HTTP_CLIENT_ERROR_SOCKET_INVALID    4
/** Can't send socket parameters */
#define HTTP_CLIENT_ERROR_SOCKET_CANT_SET   5
/** Error while resolving host name */
#define HTTP_CLIENT_ERROR_SOCKET_RESOLVE    6
/** Error while connecting to the remote server */
#define HTTP_CLIENT_ERROR_SOCKET_CONNECT    7
/** socket time out error */
#define HTTP_CLIENT_ERROR_SOCKET_TIME_OUT   8
/** Error while receiving data */
#define HTTP_CLIENT_ERROR_SOCKET_RECV       9
/** Error while sending data */
#define HTTP_CLIENT_ERROR_SOCKET_SEND       10
/** Error while receiving the remote HTTP headers */
#define HTTP_CLIENT_ERROR_HEADER_RECV       11
/** Could not find element within header */
#define HTTP_CLIENT_ERROR_HEADER_NOT_FOUND  12
/** The headers search clue was too large for the internal API buffer */
#define HTTP_CLIENT_ERROR_HEADER_BIG_CLUE   13
/** No content length was specified for the outgoing data. the caller should
    specify chunking mode in the session creation */
#define HTTP_CLIENT_ERROR_HEADER_NO_LENGTH  14
/** The HTTP chunk token that was received from the server was too big and possibly wrong */
#define HTTP_CLIENT_ERROR_CHUNK_TOO_BIG     15
/** Could not authenticate with the remote host */
#define HTTP_CLIENT_ERROR_AUTH_HOST         16
/** Could not authenticate with the remote proxy */
#define HTTP_CLIENT_ERROR_AUTH_PROXY        17
/** Bad or not supported HTTP verb was passed to a function */
#define HTTP_CLIENT_ERROR_BAD_VERB          18
/** a function received a parameter that was too large */
#define HTTP_CLIENT_ERROR_LONG_INPUT        19
/** The session state prevents the current function from proceeding */
#define HTTP_CLIENT_ERROR_BAD_STATE         20
/** Could not parse the chunk length while in chunked transfer */
#define HTTP_CLIENT_ERROR_CHUNK             21
/** Could not parse curtail elements from the URL (such as the host name, HTTP prefix act') */
#define HTTP_CLIENT_ERROR_BAD_URL           22
/** Could not detect key elements in the received headers */
#define HTTP_CLIENT_ERROR_BAD_HEADER        23
/** Error while attempting to resize a buffer */
#define HTTP_CLIENT_ERROR_BUFFER_RSIZE      24
/** Authentication schema is not supported */
#define HTTP_CLIENT_ERROR_BAD_AUTH          25
/** The selected authentication schema does not match the server response */
#define HTTP_CLIENT_ERROR_AUTH_MISMATCH     26
/** an element was missing while parsing the digest authentication challenge */
#define HTTP_CLIENT_ERROR_NO_DIGEST_TOKEN   27
/** Digest algorithem could be MD5 or MD5-sess other types are not supported */
#define HTTP_CLIENT_ERROR_NO_DIGEST_ALG     28
/** Binding error */
#define HTTP_CLIENT_ERROR_SOCKET_BIND		29
/** Tls negotiation error */
#define HTTP_CLIENT_ERROR_TLS_NEGO			30
/** Feature is not (yet) implemented */
#define HTTP_CLIENT_ERROR_NOT_IMPLEMENTED   64
/** HTTP end of stream message */
#define HTTP_CLIENT_EOS                     1000

// HTTP Session flags (Public flags)
#define HTTP_CLIENT_FLAG_KEEP_ALIVE         0x00000001 // Set the keep alive header
#define HTTP_CLIENT_FLAG_SEND_CHUNKED       0x00000002 // The outgoing should chunked
#define HTTP_CLIENT_FLAG_NO_CACHE           0x00000004 // Set the no cache header
#define HTTP_CLIENT_FLAG_ASYNC              0x00000008 // Currently not implemented 
#define HTTP_CLIENT_FLAG_MULTIPART_FORM       0x00000010 // The outgoing should multipart/form-data

// HTTP Type Definitions 
typedef UINT32          HTTP_SESSION_HANDLE;
typedef UINT32          HTTP_CLIENT_SESSION_FLAGS;
/******************************************************************************
*
*  Section      : HTTP API structures
*
******************************************************************************/

/* HTTP Type Definitions */
/** http seesion handle */
typedef u32 tls_http_session_handle_t;
/** http seesion flags */
typedef u32 tls_http_session_flags_t;

/** HTTP Supported authentication methods */
typedef enum _HTTP_AUTH_SCHEMA
{
    AuthSchemaNone      = 0,
    AuthSchemaBasic,
    AuthSchemaDigest,
    AuthSchemaKerberos,
    AuthNotSupported

} HTTP_AUTH_SCHEMA;
/** HTTP supported verbs */
typedef enum _HTTP_VERB
{
    VerbGet             = 0,
    VerbHead,
    VerbPost,
    VerbPut,
    VerbFwup,
    VerbNotSupported
    // Note: others verb such as connect and put are currently not supported

} HTTP_VERB;
/** Data structure that the caller can request at any time that will include
    some information regarding the session */
typedef struct _HTTP_CLIENT
{
    UINT32        HTTPStatusCode;                 // HTTP Status code (200 OK)
    UINT32		    RequestBodyLengthSent;          // Total bytes sent (body only)
    UINT32		    ResponseBodyLengthReceived;     // Total bytes received (body only)
    UINT32		    TotalResponseBodyLength;        // as extracted from the “content-length" header
    UINT32        HttpState;
} HTTP_CLIENT;

/** HTTP parameters */
typedef struct _HTTPParameters
{
    CHAR*                  Uri;        
    CHAR*                  ProxyHost;  
    UINT32                  UseProxy ;  
    UINT32                  ProxyPort;
    UINT32                  Verbose;
    CHAR*                  UserName;
    CHAR*                  Password;
    HTTP_AUTH_SCHEMA      AuthType;

} HTTPParameters;

#if TLS_CONFIG_HTTP_CLIENT_TASK
/** the callback function of http clent for received */
typedef void  (*http_client_recv_callback_fn)(HTTP_SESSION_HANDLE pSession, CHAR * data, UINT32 totallen, UINT32 datalen);
/** the callback function of http clent for err */
typedef void  (*http_client_err_callback_fn)(HTTP_SESSION_HANDLE pSession, int err);

/** message of the http client */
typedef struct _http_client_msg
{
    HTTP_SESSION_HANDLE pSession;
    HTTPParameters param;
    HTTP_VERB method;
    CHAR* sendData;
    UINT32      dataLen;
    http_client_recv_callback_fn recv_fn;
    http_client_err_callback_fn err_fn;
} http_client_msg;
#endif

/**
 * @defgroup APP_APIs APP APIs
 * @brief APP APIs
 */

/**
 * @addtogroup APP_APIs
 * @{
 */

/**
 * @defgroup HTTPC_APIs HTTPC APIs
 * @brief HTTP client APIs
 */

/**
 * @addtogroup HTTPC_APIs
 * @{
 */


/******************************************************************************
*
*  Section      : HTTP API public interface
*
******************************************************************************/

/**
 * @brief          Allocate memory for a new HTTP Session
 *
 * @param[in]      Flags    HTTP Session internal API flags, 0 should be passed here
 *
 * @retval         0        failed
 * @retval         other    HTTP Session handle
 *
 * @note           None
 */
HTTP_SESSION_HANDLE     HTTPClientOpenRequest         (HTTP_CLIENT_SESSION_FLAGS Flags);
/**
 * @brief          Closes the active connection and free the corresponding memory
 *
 * @param[in]      *pSession                HTTP Session handle
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
UINT32                  HTTPClientCloseRequest        (HTTP_SESSION_HANDLE *pSession);
/**
 * @brief          Sets the HTTP authentication schema
 *
 * @param[in]      pSession                 HTTP Session handle
 * @param[in]      AuthSchema             HTTP Supported authentication methods
 * @param[in]      *pReserved               Reserved parameter
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
UINT32                  HTTPClientSetAuth             (HTTP_SESSION_HANDLE pSession, HTTP_AUTH_SCHEMA AuthSchema, void *pReserved);
/**
 * @brief          Sets credentials for the target host
 *
 * @param[in]      pSession                 HTTP Session handle
 * @param[in]      *pUserName              User name
 * @param[in]      *pPassword               Password
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
UINT32                  HTTPClientSetCredentials      (HTTP_SESSION_HANDLE pSession, CHAR *pUserName, CHAR *pPassword);
/**
 * @brief          Sets all the proxy related parameters
 *
 * @param[in]      pSession        HTTP Session handle
 * @param[in]      *pProxyName    The host name
 * @param[in]      nPort           The proxy port number
 * @param[in]      *pUserName     User name for proxy authentication (can be null)
 * @param[in]      *pPassword      User password for proxy authentication (can be null)
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
UINT32                  HTTPClientSetProxy            (HTTP_SESSION_HANDLE pSession, CHAR *pProxyName, UINT16 nPort, CHAR *pUserName, CHAR *pPassword);
/**
 * @brief          Sets the HTTP verb for the outgoing request
 *
 * @param[in]      pSession                 HTTP Session handle
 * @param[in]      HttpVerb               HTTP supported verbs
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
UINT32                  HTTPClientSetVerb             (HTTP_SESSION_HANDLE pSession, HTTP_VERB HttpVerb);
/**
 * @brief          Add headers into the outgoing request
 *
 * @param[in]      pSession                 HTTP Session
 * @param[in]      *pHeaderName            The Header name
 * @param[in]      *pHeaderData            The header data
 * @param[in]      nInsert                  Reserved, could be any
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
UINT32                  HTTPClientAddRequestHeaders   (HTTP_SESSION_HANDLE pSession, CHAR *pHeaderName, CHAR *pHeaderData, BOOL nInsert);
/**
 * @brief          This function builds the request headers, performs a DNS resolution,
 *                 opens the connection (if it was not opened yet by a previous request
 *                 or if it has closed) and sends the request headers
 *
 * @param[in]      pSession      HTTP Session handle
 * @param[in]      *pUrl         The requested URL
 * @param[in]      *pData        Data to post to the server
 * @param[in]      nDataLength     Length of posted data
 * @param[in]      TotalLength    Valid only when http method is post
 *                              TRUE:  Post data to http server.
 *                              FALSE: In a post request without knowing the total
 *                                     length in advance so return error or use chunking.
 * @param[in]      nTimeout     Operation timeout
 * @param[in]      nClientPort  Client side port 0 for none
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
UINT32                  HTTPClientSendRequest         (HTTP_SESSION_HANDLE pSession, CHAR *pUrl, VOID *pData, UINT32 nDataLength, BOOL TotalLength, UINT32 nTimeout,UINT32 nClientPort);
/**
 * @brief          Write data to the remote server
 *
 * @param[in]      pSession                 HTTP Session handle
 * @param[in]      *pBuffer                 Data to write to the server
 * @param[in]      nBufferLength              Length of wtitten data
 * @param[in]      nTimeout                Timeout for the operation
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
UINT32                  HTTPClientWriteData           (HTTP_SESSION_HANDLE pSession, VOID *pBuffer, UINT32 nBufferLength, UINT32 nTimeout);
/**
 * @brief          Receives the response header on the connection and parses it.
 *                 Performs any required authentication.
 *
 * @param[in]      pSession                 HTTP Session handle
 * @param[in]      nTimeout                Timeout for the operation
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
UINT32                  HTTPClientRecvResponse        (HTTP_SESSION_HANDLE pSession, UINT32 nTimeout);
/**
 * @brief          Read data from the server. Parse out the chunks data
 *
 * @param[in]      pSession           HTTP Session handle
 * @param[out]     *pBuffer           A pointer to a buffer that will be filled with the servers response
 * @param[in]      nBytesToRead     The size of the buffer (numbers of bytes to read)
 * @param[in]      nTimeout          Operation timeout in seconds
 * @param[out]     *nBytesRecived    Count of the bytes that were received in this operation
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
UINT32                  HTTPClientReadData            (HTTP_SESSION_HANDLE pSession, VOID *pBuffer, UINT32 nBytesToRead, UINT32 nTimeout, UINT32 *nBytesRecived);
/**
 * @brief          Fill the users structure with the session information
 *
 * @param[in]      pSession                 HTTP Session handle
 * @param[out]     *HTTPClient            The session information
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
UINT32                  HTTPClientGetInfo             (HTTP_SESSION_HANDLE pSession, HTTP_CLIENT *HTTPClient);
/**
 * @brief          Initiate the headr searching functions and find the first header
 *
 * @param[in]      pSession           HTTP Session handle
 * @param[in]      *pSearchClue      Search clue
 * @param[out]     *pHeaderBuffer    A pointer to a buffer that will be filled with the header name and value
 * @param[out]     *nLength           Count of the bytes that were received in this operation
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
UINT32                  HTTPClientFindFirstHeader     (HTTP_SESSION_HANDLE pSession, CHAR *pSearchClue,CHAR *pHeaderBuffer, UINT32 *nLength);
/**
 * @brief          Find the next header.
 *
 * @param[in]      pSession           HTTP Session handle
 * @param[out]     *pHeaderBuffer    A pointer to a buffer that will be filled with the header name and value
 * @param[out]     *nLength           Count of the bytes that were received in this operation
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
UINT32                  HTTPClientGetNextHeader       (HTTP_SESSION_HANDLE pSession, CHAR *pHeaderBuffer, UINT32 *nLength);
/**
 * @brief          Terminate a headers search session
 *
 * @param[in]      pSession                 HTTP Session handle
 *
 * @retval         HTTP_CLIENT_SUCCESS     success
 * @retval         other                   failed
 *
 * @note           None
 */
UINT32                  HTTPClientFindCloseHeader     (HTTP_SESSION_HANDLE pSession);

#if TLS_CONFIG_HTTP_CLIENT_TASK
/**
 * @brief          initialize task of the http client
 *
 * @param          None
 *
 * @retval         WM_SUCCESS     success
 * @retval         WM_FAILED      failed
 *
 * @note           None
 */
int http_client_task_init(void);

/**
 * @brief          post message to the task of http client
 *
 * @param[in]      msg      pointer to the message
 *
 * @retval         ERR_OK     success
 * @retval         other      failed
 *
 * @note           None
 */
int http_client_post(http_client_msg * msg);
#endif /* TLS_CONFIG_HTTP_CLIENT_TASK */

/**
 * @}
 */

/**
 * @}
 */

#endif /* WM_HTTP_CLIENT_H */

