#include "cbmc.h"

/****************************************************************/

// TODO: Generate a header file for function declarations
IotHttpsResponseHandle_t allocate_IotResponseHandle();
IotHttpsRequestHandle_t allocate_IotRequestHandle();

/****************************************************************/

/* Implementation of safe malloc which returns NULL if the requested
 * size is 0.  Warning: The behavior of malloc(0) is platform
 * dependent.  It is possible for malloc(0) to return an address
 * without allocating memory, but doing so generates a spurious
 * error with cbmc --pointer-primitive-check.
 */
void *safeMalloc(size_t xWantedSize) {
  return xWantedSize > 0 && nondet_bool() ? malloc(xWantedSize) : NULL;
}

/****************************************************************
 * HTTP parser stubs
 ****************************************************************/

/* Model the third party HTTP Parser. */
size_t http_parser_execute (http_parser *parser,
                            const http_parser_settings *settings,
                            const char *data,
                            size_t len) {
  __CPROVER_assert(parser, "http_parser_execute parser nonnull");
  __CPROVER_assert(settings, "http_parser_execute settings nonnull");
  __CPROVER_assert(data, "http_parser_execute data nonnull");

  _httpsResponse_t *_httpsResponse = (_httpsResponse_t *)(parser->data);
  // Choose whether the parser found the header
  _httpsResponse->foundHeaderField = nondet_bool();
  _httpsResponse->parserState = PARSER_STATE_BODY_COMPLETE;

  // Generate the header value found
  size_t valueLength;
  if (_httpsResponse->foundHeaderField) {
    __CPROVER_assume(valueLength <= len);
    _httpsResponse->pReadHeaderValue = malloc(valueLength+1);
    __CPROVER_assume(_httpsResponse->pReadHeaderValue != NULL);
    _httpsResponse->pReadHeaderValue[valueLength] = 0;
    _httpsResponse->readHeaderValueLength = valueLength;
  }

  // Return the number of characters in ReadHeaderValue
  return _httpsResponse->foundHeaderField ? valueLength : 0;
}

/****************************************************************
 * IotNetworkInterface constructor
 ****************************************************************/

IotNetworkError_t IotNetworkInterfaceCreate( void * pConnectionInfo,
                                             void * pCredentialInfo,
                                             void * pConnection ) {
  __CPROVER_assert(pConnectionInfo,
                   "IotNetworkInterfaceCreate pConnectionInfo");
  /* create accepts NULL credentials when there is no TLS configuration. */
  __CPROVER_assert(pConnection, "IotNetworkInterfaceCreate pConnection");

  /* The network connection created by this function is an opaque type
   * that is simply passed to the other network functions we are
   * stubbing out, so we just ensure that it points to a memory
   * object. */
  *(char **)pConnection = malloc(1); /* network connection is opaque.  */
  __CPROVER_assume(*(char **)pConnection != NULL);

  IotNetworkError_t error;
  return error;
}

size_t IotNetworkInterfaceSend( void * pConnection,
                                const uint8_t * pMessage,
                                size_t messageLength ) {
  __CPROVER_assert(pConnection, "IotNetworkInterfaceSend pConnection");
  __CPROVER_assert(pMessage, "IotNetworkInterfaceSend pMessage");

  size_t size;
  __CPROVER_assume(size <= messageLength);
  return size;
}

IotNetworkError_t IotNetworkInterfaceClose( void * pConnection ) {
  __CPROVER_assert(pConnection, "IotNetworkInterfaceClose pConnection");

  IotNetworkError_t error;
  return error;
}

size_t IotNetworkInterfaceReceive( void * pConnection,
                                   uint8_t * pBuffer,
                                   size_t bytesRequested ) {
  __CPROVER_assert(pConnection, "IotNetworkInterfaceReceive pConnection");
  __CPROVER_assert(pBuffer, "IotNetworkInterfaceReceive pBuffer");

  __CPROVER_havoc_object(pBuffer);

  size_t size;
  __CPROVER_assume(size <= bytesRequested);
  return size;
}

IotNetworkError_t IotNetworkInterfaceCallback( void * pConnection,
                                               IotNetworkReceiveCallback_t
                                                 receiveCallback,
                                               void * pContext ) {
  __CPROVER_assert(pConnection,
                   "IotNetworkInterfaceCallback pConnection");
  __CPROVER_assert(receiveCallback,
                   "IotNetworkInterfaceCallback receiveCallback");
  __CPROVER_assert(pContext,
                   "IotNetworkInterfaceCallback pContext");

  IotNetworkError_t error;
  return error;
}

IotNetworkError_t IotNetworkInterfaceDestroy( void * pConnection ) {
  __CPROVER_assert(pConnection, "IotNetworkInterfaceDestroy pConnection");

  IotNetworkError_t error;
  return error;
}

IotNetworkInterface_t IOTNI = {
  .create = IotNetworkInterfaceCreate,
  .close = IotNetworkInterfaceClose,
  .send = IotNetworkInterfaceSend,
  .receive = IotNetworkInterfaceReceive,
  .setReceiveCallback = IotNetworkInterfaceCallback,
  .destroy = IotNetworkInterfaceDestroy
};

/* Models the Network Interface. */
IotNetworkInterface_t *allocate_NetworkInterface() {
  return nondet_bool() ? &IOTNI : NULL;
}

int is_valid_NetworkInterface(IotNetworkInterface_t *netif) {
  return
    netif->create &&
    netif->close &&
    netif->send &&
    netif->receive &&
    netif->setReceiveCallback &&
    netif->destroy;
}

/* Use
 *   __CPROVER_assume(is_stubbed_NetworkInterface(netif));
 * to ensure the stubbed out functions are used.  The initializer for
 * IOTNI appears to be ignored when CBMC is run with
 * --nondet-static. */

int is_stubbed_NetworkInterface(IotNetworkInterface_t *netif) {
  return
    netif->create == IotNetworkInterfaceCreate &&
    netif->close == IotNetworkInterfaceClose &&
    netif->send == IotNetworkInterfaceSend &&
    netif->receive == IotNetworkInterfaceReceive &&
    netif->setReceiveCallback == IotNetworkInterfaceCallback &&
    netif->destroy == IotNetworkInterfaceDestroy;
}

/****************************************************************
 * IotHttpsConnectionInfo constructor
 ****************************************************************/

/* Creates a Connection Info and assigns memory accordingly. */
IotHttpsConnectionInfo_t * allocate_IotConnectionInfo() {
  IotHttpsConnectionInfo_t * pConnInfo =
    safeMalloc(sizeof(IotHttpsConnectionInfo_t));
  if(pConnInfo) {
    pConnInfo->pNetworkInterface = allocate_NetworkInterface();
    pConnInfo->pAddress = safeMalloc(pConnInfo->addressLen);
    pConnInfo->pAlpnProtocols = safeMalloc(pConnInfo->alpnProtocolsLen);
    pConnInfo->pCaCert = safeMalloc(sizeof(uint32_t));
    pConnInfo->pClientCert = safeMalloc(sizeof(uint32_t));
    pConnInfo->pPrivateKey = safeMalloc(sizeof(uint32_t));
    pConnInfo->userBuffer.pBuffer = safeMalloc(sizeof(struct _httpsConnection));
  }
  return pConnInfo;
}

int is_valid_IotConnectionInfo(IotHttpsConnectionInfo_t *pConnInfo) {
  return
    pConnInfo->pCaCert &&
    pConnInfo->pClientCert &&
    pConnInfo->pPrivateKey &&
    pConnInfo->userBuffer.pBuffer &&
    pConnInfo->pNetworkInterface &&
    is_valid_NetworkInterface(pConnInfo->pNetworkInterface);
}

/****************************************************************
 * IotHttpsConnectionHandle constructor
 ****************************************************************/

/* Creates a Connection Handle and assigns memory accordingly. */
IotHttpsConnectionHandle_t allocate_IotConnectionHandle () {
  IotHttpsConnectionHandle_t pConnectionHandle =
    safeMalloc(sizeof(struct _httpsConnection));
  if(pConnectionHandle) {
    // network connection just points to an allocated memory object
    pConnectionHandle->pNetworkConnection = safeMalloc(1);
    pConnectionHandle->pNetworkInterface = allocate_NetworkInterface();
  }
  return pConnectionHandle;
}

IotHttpsConnectionHandle_t
initialize_IotConnectionHandle (IotHttpsConnectionHandle_t
                                pConnectionHandle) {
  if(pConnectionHandle) {
    IotListDouble_Create(&pConnectionHandle->reqQ);
    IotListDouble_Create(&pConnectionHandle->respQ);
    // Add zero or one element to response queue
    if (nondet_bool()) {
      IotHttpsResponseHandle_t resp = allocate_IotResponseHandle();
      __CPROVER_assume(resp);
      IotListDouble_InsertHead(&pConnectionHandle->respQ, &resp->link);
    }
    // Add zero or one element to request queue
    if (nondet_bool()) {
      IotHttpsRequestHandle_t req = allocate_IotRequestHandle();
      __CPROVER_assume(req);
      IotListDouble_InsertHead(&pConnectionHandle->reqQ, &req->link);
    }
 }
  return pConnectionHandle;
}

int is_valid_IotConnectionHandle(IotHttpsConnectionHandle_t handle) {
  return
    handle->pNetworkConnection &&
    handle->pNetworkInterface &&
    is_valid_NetworkInterface(handle->pNetworkInterface);
}

/****************************************************************
 * IotHttpsResponseHandle constructor
 ****************************************************************/

/* Creates a Response Handle and assigns memory accordingly. */
IotHttpsResponseHandle_t allocate_IotResponseHandle() {
  IotHttpsResponseHandle_t pResponseHandle =
    safeMalloc(sizeof(struct _httpsResponse));
  if(pResponseHandle) {
    size_t headerLen;
    __CPROVER_assume(headerLen < CBMC_MAX_OBJECT_SIZE);
    size_t headerCurOffset;
    __CPROVER_assume(headerCurOffset < headerLen);
    size_t headerEndOffset;
    __CPROVER_assume(headerEndOffset < headerLen);
    pResponseHandle->pHeaders = safeMalloc(headerLen);
    pResponseHandle->pHeadersEnd = pResponseHandle->pHeadersCur = pResponseHandle->pHeaders;
    if (pResponseHandle->pHeaders) {
      pResponseHandle->pHeadersCur = pResponseHandle->pHeaders + headerCurOffset;
      pResponseHandle->pHeadersEnd = pResponseHandle->pHeaders + headerEndOffset;
    }

    size_t bodyLen;
    __CPROVER_assume(bodyLen < CBMC_MAX_OBJECT_SIZE);
    size_t bodyCurOffset;
    __CPROVER_assume(bodyCurOffset < bodyLen);
    size_t bodyEndOffset;
    __CPROVER_assume(bodyEndOffset < bodyLen);
    pResponseHandle->pBody = safeMalloc(bodyLen);
    pResponseHandle->pBodyEnd = pResponseHandle->pBodyCur = pResponseHandle->pBody;
    if (pResponseHandle->pBody) {
      pResponseHandle->pBodyCur = pResponseHandle->pBody + bodyCurOffset;
      pResponseHandle->pBodyEnd = pResponseHandle->pBody + bodyEndOffset;
    }

    pResponseHandle->pHttpsConnection = allocate_IotConnectionHandle();
    pResponseHandle->pReadHeaderField =
      safeMalloc(pResponseHandle->readHeaderFieldLength);
    pResponseHandle->pReadHeaderValue =
      safeMalloc(pResponseHandle->readHeaderValueLength);
  }
  return pResponseHandle;
}

// ???: Should be is_stubbed
IotHttpsResponseHandle_t
initialize_IotResponseHandle(IotHttpsResponseHandle_t pResponseHandle) {
  if(pResponseHandle) {
    pResponseHandle->httpParserInfo.parseFunc = http_parser_execute;
  }
  return pResponseHandle;
}

int is_valid_IotResponseHandle(IotHttpsResponseHandle_t pResponseHandle) {
  if (pResponseHandle->pHeaders == NULL) return false;
  if (pResponseHandle->pBody == NULL) return false;

  int valid_parserdata =
    pResponseHandle->httpParserInfo.readHeaderParser.data == pResponseHandle;

  size_t header_size = __CPROVER_OBJECT_SIZE(pResponseHandle->pHeaders);
  bool valid_header_pointers =
    header_size < CBMC_MAX_OBJECT_SIZE &&

    __CPROVER_POINTER_OFFSET(pResponseHandle->pHeaders) <= header_size &&
    __CPROVER_POINTER_OFFSET(pResponseHandle->pHeadersCur) <= header_size &&
    __CPROVER_POINTER_OFFSET(pResponseHandle->pHeadersEnd) <= header_size &&

    __CPROVER_same_object(pResponseHandle->pHeaders,
                          pResponseHandle->pHeadersCur) &&
    __CPROVER_same_object(pResponseHandle->pHeaders,
                          pResponseHandle->pHeadersEnd) &&

    pResponseHandle->pHeaders <= pResponseHandle->pHeadersCur &&
    pResponseHandle->pHeadersCur <= pResponseHandle->pHeadersEnd;

  size_t body_size = __CPROVER_OBJECT_SIZE(pResponseHandle->pBody);
  bool valid_body_pointers =
    body_size < CBMC_MAX_OBJECT_SIZE &&

    __CPROVER_POINTER_OFFSET(pResponseHandle->pBody) <= body_size &&
    __CPROVER_POINTER_OFFSET(pResponseHandle->pBodyCur) <= body_size &&
    __CPROVER_POINTER_OFFSET(pResponseHandle->pBodyEnd) <= body_size &&

    __CPROVER_same_object(pResponseHandle->pBody,
                          pResponseHandle->pBodyCur) &&
    __CPROVER_same_object(pResponseHandle->pBody,
                          pResponseHandle->pBodyEnd) &&

    pResponseHandle->pBody <= pResponseHandle->pBodyCur &&
    pResponseHandle->pBodyCur <= pResponseHandle->pBodyEnd;

  return
    valid_parserdata &&
    valid_header_pointers &&
    valid_body_pointers;
}

/****************************************************************
 * IotHttpsRequestHandle constructor
 ****************************************************************/

/* Creates a Request Handle and assigns memory accordingly. */
IotHttpsRequestHandle_t allocate_IotRequestHandle() {
  IotHttpsRequestHandle_t pRequestHandle =
    safeMalloc(sizeof(struct _httpsRequest));
  if (pRequestHandle) {
    pRequestHandle->pHttpsResponse = allocate_IotResponseHandle();
    pRequestHandle->pHttpsConnection = allocate_IotConnectionHandle();

    size_t headerLen;
    __CPROVER_assume(headerLen < CBMC_MAX_OBJECT_SIZE);
    size_t headerCurOffset;
    __CPROVER_assume(headerCurOffset < headerLen);
    size_t headerEndOffset;
    __CPROVER_assume(headerEndOffset < headerLen);
    pRequestHandle->pHeaders = safeMalloc(headerLen);
    pRequestHandle->pHeadersEnd = pRequestHandle->pHeadersCur = pRequestHandle->pHeaders;
    if (pRequestHandle->pHeaders) {
      pRequestHandle->pHeadersCur = pRequestHandle->pHeaders + headerCurOffset;
      pRequestHandle->pHeadersEnd = pRequestHandle->pHeaders + headerEndOffset;
    }

    pRequestHandle->pBody = safeMalloc(pRequestHandle->bodyLength);
    pRequestHandle->pConnInfo = allocate_IotConnectionInfo();
  }
  return pRequestHandle;
}

int is_valid_IotRequestHandle(IotHttpsRequestHandle_t pRequestHandle) {
  if (pRequestHandle->pHeaders == NULL) return false;
  if (pRequestHandle->pBody == NULL) return false;

  size_t header_size = __CPROVER_OBJECT_SIZE(pRequestHandle->pHeaders);

  bool valid_header_pointers =
    header_size < CBMC_MAX_OBJECT_SIZE &&

    __CPROVER_POINTER_OFFSET(pRequestHandle->pHeaders) <= header_size &&
    __CPROVER_POINTER_OFFSET(pRequestHandle->pHeadersCur) <= header_size &&
    __CPROVER_POINTER_OFFSET(pRequestHandle->pHeadersEnd) <= header_size &&

    __CPROVER_same_object(pRequestHandle->pHeaders,
                          pRequestHandle->pHeadersCur) &&
    __CPROVER_same_object(pRequestHandle->pHeaders,
                          pRequestHandle->pHeadersEnd) &&

    pRequestHandle->pHeaders <= pRequestHandle->pHeadersCur &&
    pRequestHandle->pHeadersCur <= pRequestHandle->pHeadersEnd;

  return
    valid_header_pointers;
}

/****************************************************************
 * IotHttpsRequestInfo constructor
 * This is currently unusued and untested.
 ****************************************************************/

/* Creates a Request Info and assigns memory accordingly. */
IotHttpsRequestInfo_t * allocate_IotRequestInfo() {
  IotHttpsRequestInfo_t * pReqInfo
    = safeMalloc(sizeof(IotHttpsRequestInfo_t));
  if(pReqInfo) {
    pReqInfo->userBuffer.pBuffer = safeMalloc(pReqInfo->userBuffer.bufferLen);
    pReqInfo->pHost = safeMalloc(pReqInfo->hostLen);
  }
  return pReqInfo;
}

int is_valid_IotRequestInfo(IotHttpsRequestInfo_t * pReqInfo) {
  return
    pReqInfo->hostLen <= IOT_HTTPS_MAX_HOST_NAME_LENGTH + 1;
}

/****************************************************************
 * IotHttpsResponseInfo constructor
 ****************************************************************/

/* Creates a Response Info and assigns memory accordingly. */
IotHttpsResponseInfo_t * allocate_IotResponseInfo() {
  IotHttpsResponseInfo_t * pRespInfo =
    safeMalloc(sizeof(IotHttpsResponseInfo_t));
  if(pRespInfo) {
    pRespInfo->userBuffer.pBuffer = safeMalloc(pRespInfo->userBuffer.bufferLen);
    pRespInfo->pSyncInfo = safeMalloc(sizeof(IotHttpsSyncInfo_t));
    if (pRespInfo->pSyncInfo)
      pRespInfo->pSyncInfo->pBody = safeMalloc(pRespInfo->pSyncInfo->bodyLen);
  }
  return pRespInfo;
}

int is_valid_IotResponseInfo(IotHttpsResponseInfo_t * pRespInfo){
  return
    pRespInfo->pSyncInfo &&
    pRespInfo->pSyncInfo->pBody &&
    pRespInfo->pSyncInfo->bodyLen <= CBMC_MAX_OBJECT_SIZE &&
    pRespInfo->userBuffer.bufferLen <= CBMC_MAX_OBJECT_SIZE;
}
