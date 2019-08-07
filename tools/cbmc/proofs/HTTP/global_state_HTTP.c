#ifndef USER_DATA_SIZE
#define USER_DATA_SIZE 1000
#endif

/****************************************************************/

/* Implementation of safe malloc which returns NULL if the requested
 * size is 0.  Warning: The behavior of malloc(0) is platform
 * dependent.  It is possible for malloc(0) to return an address
 * without allocating memory.
 */
void *safeMalloc(size_t xWantedSize) {
  if(xWantedSize == 0) {
    return NULL;
  }
  uint8_t byte;
  return byte ? malloc(xWantedSize) : NULL;
}

/****************************************************************/

/* It is common for a buffer to contain a header struct followed by
 * user data.  We optimize CBMC performance by allocating space for
 * the buffer using a struct with two members: the first member is the
 * header struct and the second member is the user data. This is
 * faster than just allocating a sequence of bytes large enough to
 * hold the header struct and the user data.  We modeled
 * responseHandle, requestHandle and connectionHandle similarly.
 */

typedef struct _responseHandle
{
  struct _httpsResponse RespHandle;
  char data[USER_DATA_SIZE];
} _resHandle_t;

typedef struct _requestHandle
{
  struct _httpsRequest ReqHandle;
  char data[USER_DATA_SIZE];
} _reqHandle_t;

typedef struct _connectionHandle
{
  struct _httpsConnection pConnHandle;
  char data[USER_DATA_SIZE];
} _connHandle_t;

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
  /* nondet_bool is required to get coverage. */
  _httpsResponse->foundHeaderField = nondet_bool();
  _httpsResponse->parserState = PARSER_STATE_BODY_COMPLETE;

  size_t ret;
  return ret;
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

  /* Fill the entire memory object pointed to by pBuffer with
   * unconstrained data.  This use of __CPROVER_array_copy with a
   * single byte is a common CBMC idiom. */
  uint8_t byte;
  __CPROVER_array_copy(pBuffer,&byte);

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
IotNetworkInterface_t *newNetworkInterface() {
  return nondet_bool() ? &IOTNI : NULL;
}

int is_valid_NetworkInterface(IotNetworkInterface_t *netif) {
  return
    netif &&
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
IotHttpsConnectionInfo_t * newConnectionInfo() {
  IotHttpsConnectionInfo_t * pConnInfo =
    safeMalloc(sizeof(IotHttpsConnectionInfo_t));
  if(pConnInfo) {
    pConnInfo->pNetworkInterface = newNetworkInterface();
    pConnInfo->pAddress = safeMalloc(pConnInfo->addressLen);
    pConnInfo->pAlpnProtocols = safeMalloc(pConnInfo->alpnProtocolsLen);
    pConnInfo->pCaCert = safeMalloc(sizeof(uint32_t));
    pConnInfo->pClientCert = safeMalloc(sizeof(uint32_t));
    pConnInfo->pPrivateKey = safeMalloc(sizeof(uint32_t));
    pConnInfo->userBuffer.pBuffer = safeMalloc(sizeof(_connHandle_t));
  }
  return pConnInfo;
}

int is_valid_ConnectionInfo(IotHttpsConnectionInfo_t *pConnInfo) {
  return
    pConnInfo &&
    pConnInfo->pCaCert &&
    pConnInfo->pClientCert &&
    pConnInfo->pPrivateKey &&
    pConnInfo->userBuffer.pBuffer &&
    is_valid_NetworkInterface(pConnInfo->pNetworkInterface);
}

/****************************************************************
 * IotHttpsConnectionHandle constructor
 ****************************************************************/

/* Creates a Connection Handle and assigns memory accordingly. */
IotHttpsConnectionHandle_t newIotConnectionHandle () {
  IotHttpsConnectionHandle_t pConnectionHandle =
    safeMalloc(sizeof(_connHandle_t));
  if(pConnectionHandle) {
    /* network connection just points to an allocated memory object */
    pConnectionHandle->pNetworkConnection = safeMalloc(1);
    pConnectionHandle->pNetworkInterface = newNetworkInterface();
    pConnectionHandle->reqQ.pPrevious = &(pConnectionHandle->reqQ);
    pConnectionHandle->reqQ.pNext = &(pConnectionHandle->reqQ);
    pConnectionHandle->respQ.pPrevious = &(pConnectionHandle->respQ);
    pConnectionHandle->respQ.pNext = &(pConnectionHandle->respQ);
  }
  return pConnectionHandle;
}

int is_valid_IotConnectionHandle(IotHttpsConnectionHandle_t handle) {
  return
    handle &&
    handle->pNetworkConnection &&
    is_valid_NetworkInterface(handle->pNetworkInterface);
}

/****************************************************************
 * IotHttpsResponseHandle constructor
 ****************************************************************/

/* Creates a Response Handle and assigns memory accordingly. */
IotHttpsResponseHandle_t newIotResponseHandle() {
  IotHttpsResponseHandle_t pResponseHandle = safeMalloc(sizeof(_resHandle_t));
  if(pResponseHandle) {
    uint32_t len;

    pResponseHandle->httpParserInfo.parseFunc = http_parser_execute;

    pResponseHandle->pBody = saveMalloc(len);
    pResponseHandle->pHttpsConnection = newIotConnectionHandle();
    pResponseHandle->pReadHeaderValue =
      safeMalloc(pResponseHandle->readHeaderValueLength);
  }
  return pResponseHandle;
}

int is_valid_newIotResponseHandle(IotHttpsResponseHandle_t pResponseHandle) {
  int required =
    pResponseHandle &&
    __CPROVER_same_object(pResponseHandle->pHeaders,
			  pResponseHandle->pHeadersCur) &&
    // Does this overconstrain End?!?
    __CPROVER_same_object(pResponseHandle->pHeaders,
			  pResponseHandle->pHeadersEnd);
  if (!required) return 0;

  int valid_headers =
    pResponseHandle->pHeaders == ((_resHandle_t*)pResponseHandle)->data;
  int valid_order =
    pResponseHandle->pHeaders <= pResponseHandle->pHeadersCur &&
    pResponseHandle->pHeadersCur <=  pResponseHandle->pHeadersEnd;
  int valid_parserdata =
    pResponseHandle->httpParserInfo.readHeaderParser.data == pResponseHandle;
  return
    valid_headers &&
    valid_order &&
    valid_parserdata;
}


/****************************************************************
 * IotHttpsRequestHandle constructor
 ****************************************************************/

/* Creates a Request Handle and assigns memory accordingly. */
IotHttpsRequestHandle_t newIotRequestHandle() {
  IotHttpsRequestHandle_t pRequestHandle = safeMalloc(sizeof(_reqHandle_t));
  if (pRequestHandle) {
    uint32_t len;
    pRequestHandle->pHttpsResponse = newIotResponseHandle();
    pRequestHandle->pHttpsConnection = newIotConnectionHandle();
    pRequestHandle->pBody = malloc(len);
    pRequestHandle->pHeaders = ((_reqHandle_t*)pRequestHandle)->data;
    pRequestHandle->pHeadersCur = pRequestHandle->pHeaders;
    pRequestHandle->pHeadersEnd = pRequestHandle->pHeaders + sizeof(((_reqHandle_t*)pRequestHandle)->data);
    pRequestHandle->pConnInfo = newConnectionInfo();
  }
  return pRequestHandle;
}

/****************************************************************
 * IotHttpsRequestInfo constructor
 ****************************************************************/

/* Creates a Request Info and assigns memory accordingly. */
IotHttpsRequestInfo_t * newIotRequestInfo() {
  IotHttpsRequestInfo_t * pReqInfo = safeMalloc(sizeof(IotHttpsRequestInfo_t));
  if(pReqInfo) {
    uint32_t bufferSize;
    uint32_t hostNameLen;
    __CPROVER_assume(bufferSize >=0 && bufferSize <= requestUserBufferMinimumSize);
    pReqInfo->userBuffer.bufferLen = bufferSize;
    pReqInfo->userBuffer.pBuffer = safeMalloc(bufferSize);
    __CPROVER_assume(hostNameLen >= 0 && hostNameLen <= IOT_HTTPS_MAX_HOST_NAME_LENGTH + 1);
    pReqInfo->pHost = safeMalloc(hostNameLen);
  }
  return pReqInfo;
}

/****************************************************************
 * IotHttpsResponseInfo constructor
 ****************************************************************/

/* Creates a Response Info and assigns memory accordingly. */
IotHttpsResponseInfo_t * newIotResponseInfo() {
  IotHttpsResponseInfo_t * pRespInfo = safeMalloc(sizeof(IotHttpsResponseInfo_t));
  if(pRespInfo) {
    uint32_t bufferSize;
    uint32_t bodySize;
    pRespInfo->userBuffer.bufferLen = bufferSize;
    pRespInfo->userBuffer.pBuffer = safeMalloc(bufferSize);
    /* We assume that these two pointers can not be NULL.*/
    pRespInfo->pSyncInfo = malloc(sizeof(IotHttpsSyncInfo_t));
    pRespInfo->pSyncInfo->pBody = malloc(bodySize);
    pRespInfo->pSyncInfo->bodyLen = bodySize;
  }
  return pRespInfo;
}
