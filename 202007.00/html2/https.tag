<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.8.20">
  <compound kind="file">
    <name>iot_https_client.c</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/https/src/</path>
    <filename>iot__https__client_8c.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_PARTIAL_REQUEST_LINE</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>ade983b6cfc14bbfda5e3b54ad12dc74f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_USER_AGENT_HEADER_LINE</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a08ee6a51834ef0621be68e137e11d57d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_PARTIAL_HOST_HEADER_LINE</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a22f2939baf9c573f8c79321f6b9c20d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_CONNECTION_KEEP_ALIVE_HEADER_LINE</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a5ceb27a9768e58b79bfddefba50e6b4c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_CONNECTION_CLOSE_HEADER_LINE</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a9447c562ce8ea09faaea4cc2a8c3b29f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_CONNECTION_KEEP_ALIVE_HEADER_LINE_LENGTH</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a73020a002c581f88916d4ab2023692ab</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>KEEP_PARSING</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a83c154d537b7cac33b3eb8643a19eda7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>STOP_PARSING</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>abab39a9ab55c7f2e6c2c50b59b8fcbc3</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>_httpParserOnMessageBeginCallback</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a51de458b0a395edb69ece96692c96869</anchor>
      <arglist>(http_parser *pHttpParser)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>_httpParserOnStatusCallback</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>acfec8c6c67b0dcb25a16b6f8ddd10c28</anchor>
      <arglist>(http_parser *pHttpParser, const char *pLoc, size_t length)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>_httpParserOnHeaderFieldCallback</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a3a34e33282246046f5b8d94dc2acfdc6</anchor>
      <arglist>(http_parser *pHttpParser, const char *pLoc, size_t length)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>_httpParserOnHeaderValueCallback</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a1dced480d1221da787773dd7ac9deb91</anchor>
      <arglist>(http_parser *pHttpParser, const char *pLoc, size_t length)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>_httpParserOnHeadersCompleteCallback</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>ad867e2358e02f386706610900de4751f</anchor>
      <arglist>(http_parser *pHttpParser)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>_httpParserOnBodyCallback</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a0c6e82cbe65dcb38433c91d90c58798e</anchor>
      <arglist>(http_parser *pHttpParser, const char *pLoc, size_t length)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>_httpParserOnMessageCompleteCallback</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a3d2575eeee02908774ed30bd6589c627</anchor>
      <arglist>(http_parser *pHttpParser)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>_httpParserOnChunkHeaderCallback</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>ad90ca428ea09a2fde13af768f9090696</anchor>
      <arglist>(http_parser *pHttpParser)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>_httpParserOnChunkCompleteCallback</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a1716a45d4d5760b83f376ce99cc507a6</anchor>
      <arglist>(http_parser *pHttpParser)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_networkReceiveCallback</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>acaee05a709126be6d9bb34fbd62223ab</anchor>
      <arglist>(void *pNetworkConnection, void *pReceiveContext)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsReturnCode_t</type>
      <name>_createHttpsConnection</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a743bfef6a91d2f8a9442922fbf991669</anchor>
      <arglist>(IotHttpsConnectionHandle_t *pConnHandle, IotHttpsConnectionInfo_t *pConnInfo)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_networkDisconnect</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a3dd05a3d418740e819361cfe5b20e010</anchor>
      <arglist>(_httpsConnection_t *pHttpsConnection)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_networkDestroy</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a2ed90d0bed958adf186a9c98ee6bea4f</anchor>
      <arglist>(_httpsConnection_t *pHttpsConnection)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsReturnCode_t</type>
      <name>_addHeader</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a00b01bc758b3c46e07146a4936a7dd68</anchor>
      <arglist>(_httpsRequest_t *pHttpsRequest, const char *pName, uint32_t nameLen, const char *pValue, uint32_t valueLen)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsReturnCode_t</type>
      <name>_networkSend</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a5ec8afb1bd760ec9a4c4b5e77439c722</anchor>
      <arglist>(_httpsConnection_t *pHttpsConnection, uint8_t *pBuf, size_t len)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsReturnCode_t</type>
      <name>_networkRecv</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a21e13e133941676d9b96826181305dff</anchor>
      <arglist>(_httpsConnection_t *pHttpsConnection, uint8_t *pBuf, size_t bufLen, size_t *numBytesRecv)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsReturnCode_t</type>
      <name>_sendHttpsHeaders</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a4d6ca2d9b6cdd1346b31df34bbb537e1</anchor>
      <arglist>(_httpsConnection_t *pHttpsConnection, uint8_t *pHeadersBuf, uint32_t headersLength, bool isNonPersistent, uint32_t contentLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsReturnCode_t</type>
      <name>_sendHttpsBody</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a8dca278a4b243ca23603088618439e73</anchor>
      <arglist>(_httpsConnection_t *pHttpsConnection, uint8_t *pBodyBuf, uint32_t bodyLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsReturnCode_t</type>
      <name>_parseHttpsMessage</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>ae619c0ccaa43158ba9b5b9a7820a753b</anchor>
      <arglist>(_httpParserInfo_t *pHttpParserInfo, char *pBuf, size_t len)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsReturnCode_t</type>
      <name>_receiveHttpsMessage</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a2b5fd6c4247ca0b60587b53162b3ee4d</anchor>
      <arglist>(_httpsConnection_t *pHttpsConnection, _httpParserInfo_t *pParser, IotHttpsResponseParserState_t *pCurrentParserState, IotHttpsResponseParserState_t finalParserState, IotHttpsResponseBufferState_t currentBufferProcessingState, uint8_t **pBufCur, uint8_t **pBufEnd)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsReturnCode_t</type>
      <name>_receiveHttpsHeaders</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>ae44eb58f47d897aad0f59043ebf70e28</anchor>
      <arglist>(_httpsConnection_t *pHttpsConnection, _httpsResponse_t *pHttpsResponse)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsReturnCode_t</type>
      <name>_receiveHttpsBody</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a4cc1b6ab9042cbe72a02568a433f837b</anchor>
      <arglist>(_httpsConnection_t *pHttpsConnection, _httpsResponse_t *pHttpsResponse)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsReturnCode_t</type>
      <name>_flushHttpsNetworkData</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>abf6039a07c2646d8a6328da777e25931</anchor>
      <arglist>(_httpsConnection_t *pHttpsConnection, _httpsResponse_t *pHttpsResponse)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_sendHttpsRequest</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a27ec9ebb631e771aad4af1f11960530c</anchor>
      <arglist>(IotTaskPool_t pTaskPool, IotTaskPoolJob_t pJob, void *pUserContext)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsReturnCode_t</type>
      <name>_receiveHttpsBodyAsync</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a0ed02d1eace136cc379821d0b307afd7</anchor>
      <arglist>(_httpsResponse_t *pHttpsResponse)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsReturnCode_t</type>
      <name>_receiveHttpsBodySync</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>aaa1358b64f9790b05519b898fa0b245b</anchor>
      <arglist>(_httpsResponse_t *pHttpsResponse)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>_scheduleHttpsRequestSend</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>ab64d059816198c24f9def14212681ec4</anchor>
      <arglist>(_httpsRequest_t *pHttpsRequest)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>_addRequestToConnectionReqQ</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>adf038c9de9b06d68a06aa42af5992ed3</anchor>
      <arglist>(_httpsRequest_t *pHttpsRequest)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_cancelRequest</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a41fecba33d952a5e8b374fedc4be1ed2</anchor>
      <arglist>(_httpsRequest_t *pHttpsRequest)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_cancelResponse</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a1fafc4dfb9de1cf65c9abf3575b82dc7</anchor>
      <arglist>(_httpsResponse_t *pHttpsResponse)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsReturnCode_t</type>
      <name>_initializeResponse</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a61f61671d909aec47f00aec454553163</anchor>
      <arglist>(IotHttpsResponseHandle_t *pRespHandle, IotHttpsResponseInfo_t *pRespInfo, _httpsRequest_t *pHttpsRequest)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_incrementNextLocationToWriteBeyondParsed</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>aeed11a53af6a912b8d594d94a8d56256</anchor>
      <arglist>(uint8_t **pBufCur, uint8_t **pBufEnd)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsReturnCode_t</type>
      <name>_sendHttpsHeadersAndBody</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a0584427cc108cc0563badf52f0d1d55f</anchor>
      <arglist>(_httpsConnection_t *pHttpsConnection, _httpsRequest_t *pHttpsRequest)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_Init</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a532a958a03d130f0306c311c2a750bd9</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotHttpsClient_Cleanup</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a4a2b507259f9b99f8c6a658caaf7921f</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_Connect</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a0b35628f93edee5efb4056433d12b837</anchor>
      <arglist>(IotHttpsConnectionHandle_t *pConnHandle, IotHttpsConnectionInfo_t *pConnInfo)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_Disconnect</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a8d1c4fc3d122e22e6bd5db5ee3c0ae02</anchor>
      <arglist>(IotHttpsConnectionHandle_t connHandle)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_InitializeRequest</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>ae979e234f7ee8c4238de07f35fce971e</anchor>
      <arglist>(IotHttpsRequestHandle_t *pReqHandle, IotHttpsRequestInfo_t *pReqInfo)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_AddHeader</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>aa041b3db35e5016c8fa7691f385b173b</anchor>
      <arglist>(IotHttpsRequestHandle_t reqHandle, char *pName, uint32_t nameLen, char *pValue, uint32_t valueLen)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_SendSync</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a4ff89ea1405f61208446306744ac0814</anchor>
      <arglist>(IotHttpsConnectionHandle_t connHandle, IotHttpsRequestHandle_t reqHandle, IotHttpsResponseHandle_t *pRespHandle, IotHttpsResponseInfo_t *pRespInfo, uint32_t timeoutMs)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_WriteRequestBody</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a176331d088cc9b0047e601a0325cd908</anchor>
      <arglist>(IotHttpsRequestHandle_t reqHandle, uint8_t *pBuf, uint32_t len, int isComplete)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_ReadResponseBody</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a9e368690c0848986ea92e1456248392b</anchor>
      <arglist>(IotHttpsResponseHandle_t respHandle, uint8_t *pBuf, uint32_t *pLen)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_CancelRequestAsync</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a745295c8d66d158d05d8e045c21e2b03</anchor>
      <arglist>(IotHttpsRequestHandle_t reqHandle)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_CancelResponseAsync</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a66d4f56e136696918e6091d404ca36f2</anchor>
      <arglist>(IotHttpsResponseHandle_t respHandle)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_SendAsync</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>aa6af0fae04d9f86f2fe49fd42d517534</anchor>
      <arglist>(IotHttpsConnectionHandle_t connHandle, IotHttpsRequestHandle_t reqHandle, IotHttpsResponseHandle_t *pRespHandle, IotHttpsResponseInfo_t *pRespInfo)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_ReadResponseStatus</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a4a77019d97ddc4d543b7c29b01e7b8ca</anchor>
      <arglist>(IotHttpsResponseHandle_t respHandle, uint16_t *pStatus)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_ReadHeader</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>ae88c2ae8a0e8b8a4bd421a3d56c8343d</anchor>
      <arglist>(IotHttpsResponseHandle_t respHandle, char *pName, uint32_t nameLen, char *pValue, uint32_t valueLen)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_ReadContentLength</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>ae5f8450cc059e00d215bf7caa3edf59a</anchor>
      <arglist>(IotHttpsResponseHandle_t respHandle, uint32_t *pContentLength)</arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>_pHttpsMethodStrings</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a6b866c3f2fd7482cbaf14b4d52e7071d</anchor>
      <arglist>[]</arglist>
    </member>
    <member kind="variable">
      <type>const uint32_t</type>
      <name>requestUserBufferMinimumSize</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>a898557b7c306d04d9a1d6478603cc551</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint32_t</type>
      <name>responseUserBufferMinimumSize</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>ab3340fd4e0f710ea36bff43f7165e97d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint32_t</type>
      <name>connectionUserBufferMinimumSize</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>aafb5f7684f85f9775eb489c12ce4ce77</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static http_parser_settings</type>
      <name>_httpParserSettings</name>
      <anchorfile>iot__https__client_8c.html</anchorfile>
      <anchor>adff61b6e46a033a0c6d027cfa09290da</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_https_client.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/https/include/</path>
    <filename>iot__https__client_8h.html</filename>
    <includes id="iot__https__types_8h" name="iot_https_types.h" local="yes" imported="no">types/iot_https_types.h</includes>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_Init</name>
      <anchorfile>iot__https__client_8h.html</anchorfile>
      <anchor>a532a958a03d130f0306c311c2a750bd9</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotHttpsClient_Cleanup</name>
      <anchorfile>iot__https__client_8h.html</anchorfile>
      <anchor>a4a2b507259f9b99f8c6a658caaf7921f</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_Connect</name>
      <anchorfile>iot__https__client_8h.html</anchorfile>
      <anchor>a0b35628f93edee5efb4056433d12b837</anchor>
      <arglist>(IotHttpsConnectionHandle_t *pConnHandle, IotHttpsConnectionInfo_t *pConnInfo)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_Disconnect</name>
      <anchorfile>iot__https__client_8h.html</anchorfile>
      <anchor>a8d1c4fc3d122e22e6bd5db5ee3c0ae02</anchor>
      <arglist>(IotHttpsConnectionHandle_t connHandle)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_InitializeRequest</name>
      <anchorfile>iot__https__client_8h.html</anchorfile>
      <anchor>ae979e234f7ee8c4238de07f35fce971e</anchor>
      <arglist>(IotHttpsRequestHandle_t *pReqHandle, IotHttpsRequestInfo_t *pReqInfo)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_AddHeader</name>
      <anchorfile>iot__https__client_8h.html</anchorfile>
      <anchor>aa041b3db35e5016c8fa7691f385b173b</anchor>
      <arglist>(IotHttpsRequestHandle_t reqHandle, char *pName, uint32_t nameLen, char *pValue, uint32_t valueLen)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_WriteRequestBody</name>
      <anchorfile>iot__https__client_8h.html</anchorfile>
      <anchor>a176331d088cc9b0047e601a0325cd908</anchor>
      <arglist>(IotHttpsRequestHandle_t reqHandle, uint8_t *pBuf, uint32_t len, int isComplete)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_SendSync</name>
      <anchorfile>iot__https__client_8h.html</anchorfile>
      <anchor>a4ff89ea1405f61208446306744ac0814</anchor>
      <arglist>(IotHttpsConnectionHandle_t connHandle, IotHttpsRequestHandle_t reqHandle, IotHttpsResponseHandle_t *pRespHandle, IotHttpsResponseInfo_t *pRespInfo, uint32_t timeoutMs)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_SendAsync</name>
      <anchorfile>iot__https__client_8h.html</anchorfile>
      <anchor>aa6af0fae04d9f86f2fe49fd42d517534</anchor>
      <arglist>(IotHttpsConnectionHandle_t connHandle, IotHttpsRequestHandle_t reqHandle, IotHttpsResponseHandle_t *pRespHandle, IotHttpsResponseInfo_t *pRespInfo)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_CancelRequestAsync</name>
      <anchorfile>iot__https__client_8h.html</anchorfile>
      <anchor>a745295c8d66d158d05d8e045c21e2b03</anchor>
      <arglist>(IotHttpsRequestHandle_t reqHandle)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_CancelResponseAsync</name>
      <anchorfile>iot__https__client_8h.html</anchorfile>
      <anchor>a66d4f56e136696918e6091d404ca36f2</anchor>
      <arglist>(IotHttpsResponseHandle_t respHandle)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_ReadResponseStatus</name>
      <anchorfile>iot__https__client_8h.html</anchorfile>
      <anchor>a4a77019d97ddc4d543b7c29b01e7b8ca</anchor>
      <arglist>(IotHttpsResponseHandle_t respHandle, uint16_t *pStatus)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_ReadContentLength</name>
      <anchorfile>iot__https__client_8h.html</anchorfile>
      <anchor>ae5f8450cc059e00d215bf7caa3edf59a</anchor>
      <arglist>(IotHttpsResponseHandle_t respHandle, uint32_t *pContentLength)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_ReadHeader</name>
      <anchorfile>iot__https__client_8h.html</anchorfile>
      <anchor>ae88c2ae8a0e8b8a4bd421a3d56c8343d</anchor>
      <arglist>(IotHttpsResponseHandle_t respHandle, char *pName, uint32_t nameLen, char *pValue, uint32_t valueLen)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_ReadResponseBody</name>
      <anchorfile>iot__https__client_8h.html</anchorfile>
      <anchor>a9e368690c0848986ea92e1456248392b</anchor>
      <arglist>(IotHttpsResponseHandle_t respHandle, uint8_t *pBuf, uint32_t *pLen)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_https_types.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/https/include/types/</path>
    <filename>iot__https__types_8h.html</filename>
    <class kind="struct">IotHttpsClientCallbacks_t</class>
    <class kind="struct">IotHttpsUserBuffer_t</class>
    <class kind="struct">IotHttpsSyncInfo_t</class>
    <class kind="struct">IotHttpsAsyncInfo_t</class>
    <class kind="struct">IotHttpsConnectionInfo_t</class>
    <class kind="struct">IotHttpsRequestInfo_t</class>
    <class kind="struct">IotHttpsResponseInfo_t</class>
    <member kind="define">
      <type>#define</type>
      <name>IOT_HTTPS_IS_NON_TLS_FLAG</name>
      <anchorfile>iot__https__types_8h.html</anchorfile>
      <anchor>a22415f9a996bbdc689e658a618ebf58b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_HTTPS_DISABLE_SNI</name>
      <anchorfile>iot__https__types_8h.html</anchorfile>
      <anchor>a332dd6bdaa48440aee657f7ad1735dd6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_HTTPS_CONNECTION_HANDLE_INITIALIZER</name>
      <anchorfile>iot__https__types_8h.html</anchorfile>
      <anchor>a9c148f665bf0201640762823acf0874f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_HTTPS_REQUEST_HANDLE_INITIALIZER</name>
      <anchorfile>iot__https__types_8h.html</anchorfile>
      <anchor>a59eb8efbb864d78c4812f86f61a3c01a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_HTTPS_RESPONSE_HANDLE_INITIALIZER</name>
      <anchorfile>iot__https__types_8h.html</anchorfile>
      <anchor>a7433ad198ab324ac5e07cbea02582806</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_HTTPS_USER_BUFFER_INITIALIZER</name>
      <anchorfile>iot__https__types_8h.html</anchorfile>
      <anchor>a72ec8a7c9e8de40a6f80bebac146a1d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_HTTPS_SYNC_INFO_INITIALIZER</name>
      <anchorfile>iot__https__types_8h.html</anchorfile>
      <anchor>a6419c6e206a9956e7efe952dd735ff47</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_HTTPS_ASYNC_INFO_INITIALIZER</name>
      <anchorfile>iot__https__types_8h.html</anchorfile>
      <anchor>ad86154be6200190c350a43006a00d10e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_HTTPS_CONNECTION_INFO_INITIALIZER</name>
      <anchorfile>iot__https__types_8h.html</anchorfile>
      <anchor>a983b12006eec3f005b7559cd556b535b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_HTTPS_REQUEST_INFO_INITIALIZER</name>
      <anchorfile>iot__https__types_8h.html</anchorfile>
      <anchor>a50b17ee86b5959aec9279ceb10263ab5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_HTTPS_RESPONSE_INFO_INITIALIZER</name>
      <anchorfile>iot__https__types_8h.html</anchorfile>
      <anchor>a4dfc53ee0dd29036d4c5a9235de35c1f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_HTTPS_NETWORK_INTERFACE_TYPE</name>
      <anchorfile>iot__https__types_8h.html</anchorfile>
      <anchor>ad91cc9ac31d1201d586f31d22ddcc739</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct _httpsConnection *</type>
      <name>IotHttpsConnectionHandle_t</name>
      <anchorfile>group__https__client__datatypes__handles.html</anchorfile>
      <anchor>ga6f57a7b8c51f49588625b0dd4dd6dedb</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct _httpsRequest *</type>
      <name>IotHttpsRequestHandle_t</name>
      <anchorfile>group__https__client__datatypes__handles.html</anchorfile>
      <anchor>ga23265e6eec7d9fa98da915d8e6a44b01</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct _httpsResponse *</type>
      <name>IotHttpsResponseHandle_t</name>
      <anchorfile>group__https__client__datatypes__handles.html</anchorfile>
      <anchor>gad59eec14931d695aad203d087cb961ea</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>IotHttpsReturnCode_t</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>gaeddc61f93281b31a5125ff2e57de9825</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_OK</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825af5531caaf2f532241af4f548840bfa9a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_INVALID_PARAMETER</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a9da4d637fd9c7ffb678f6c38dd2b0d8e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_INVALID_PAYLOAD</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a05056960780c215288e8e3b2198e5003</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_MESSAGE_TOO_LARGE</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825af843f35a50769e5a51803f216b8348a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_OVERFLOW</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825ae4027f63df204a6805e8eafa64f6a038</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_INSUFFICIENT_MEMORY</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a1335a4ceb7b35eb133a85c7937357041</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_QUEUE_FULL</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825ad1f2e262121f7b9ded89b794bba588d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_RETRY</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a6a2b50a5f0bc88311c4915b27c5edf13</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_NOT_FOUND</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a06e353388aa9f8c603d511c97336ea88</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_MESSAGE_FINISHED</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a8d869ebdf6648e4db9f6f7d57c6ded5e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_INTERNAL_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a2b7b810207afd90656ec30de44c9f8b2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_NETWORK_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a5eb66c716fa9269eec62f302acd0dea2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_CONNECTION_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a6c569739cc62b7ee7c7e07a03ea1da5a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_STREAM_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825af1dfa7a4c2b6bd87a15389757ae66512</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_AUTHENTICATION_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825aeac36da5f7728e7f0f923c50b8143528</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_TLS_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a393abc7d3309cf057eceb91f783c092a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_USER_CALLBACK_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a98d0b93f2c3947f5816d6335b57f5a31</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_TIMEOUT_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a5087d30c1e6649f597b36d80dc876dec</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_PROTOCOL_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a9378fd542bff27e650645811a0e98a06</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_SEND_ABORT</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825af2e021c46eb64cc4bf2eab18c1d99aab</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_RECEIVE_ABORT</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a3b695e40d399ef8500494770ae5cd92a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_ASYNC_SCHEDULING_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825aba7cdfc90f4d8e4e7e9d058c95fabd85</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_PARSING_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a580063ea7d435812c1d256e0b5412774</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_FATAL</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825ad50da9a382731704fb01587a34dca548</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_BUSY</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a3d8759fdabaf6a5a9f524bb2b4d6d281</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_TRY_AGAIN</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825af845f74276676280240c4e25b93c660c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_DATA_EXIST</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a5e099e83668add49b23db7d88a05df02</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_NOT_SUPPORTED</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a08165c17c8e574fa185e1b455d78bf39</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>IotHttpsMethod_t</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>gaaa195d049a76d14bd34b4ab5d610e843</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>IotHttpsResponseStatus</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ga6cf83531f2a6d2f964621761825aac9d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint32_t</type>
      <name>requestUserBufferMinimumSize</name>
      <anchorfile>iot__https__types_8h.html</anchorfile>
      <anchor>a898557b7c306d04d9a1d6478603cc551</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint32_t</type>
      <name>responseUserBufferMinimumSize</name>
      <anchorfile>iot__https__types_8h.html</anchorfile>
      <anchor>ab3340fd4e0f710ea36bff43f7165e97d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint32_t</type>
      <name>connectionUserBufferMinimumSize</name>
      <anchorfile>iot__https__types_8h.html</anchorfile>
      <anchor>aafb5f7684f85f9775eb489c12ce4ce77</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_https_utils.c</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/https/src/</path>
    <filename>iot__https__utils_8c.html</filename>
    <includes id="iot__https__utils_8h" name="iot_https_utils.h" local="yes" imported="no">iot_https_utils.h</includes>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_GetUrlPath</name>
      <anchorfile>iot__https__utils_8c.html</anchorfile>
      <anchor>a22aefb5afa878193f061d72db78c5c15</anchor>
      <arglist>(const char *pUrl, size_t urlLen, const char **pPath, size_t *pPathLen)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_GetUrlAddress</name>
      <anchorfile>iot__https__utils_8c.html</anchorfile>
      <anchor>a2e9bddd8a0c7f10a076b31aabf7a7cad</anchor>
      <arglist>(const char *pUrl, size_t urlLen, const char **pAddress, size_t *pAddressLen)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_https_utils.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/https/include/</path>
    <filename>iot__https__utils_8h.html</filename>
    <includes id="iot__https__types_8h" name="iot_https_types.h" local="yes" imported="no">types/iot_https_types.h</includes>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_GetUrlPath</name>
      <anchorfile>iot__https__utils_8h.html</anchorfile>
      <anchor>a22aefb5afa878193f061d72db78c5c15</anchor>
      <arglist>(const char *pUrl, size_t urlLen, const char **pPath, size_t *pPathLen)</arglist>
    </member>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotHttpsClient_GetUrlAddress</name>
      <anchorfile>iot__https__utils_8h.html</anchorfile>
      <anchor>a2e9bddd8a0c7f10a076b31aabf7a7cad</anchor>
      <arglist>(const char *pUrl, size_t urlLen, const char **pAddress, size_t *pAddressLen)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_test_access_https.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/https/test/access/</path>
    <filename>iot__test__access__https_8h.html</filename>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotTestHttps_initializeResponse</name>
      <anchorfile>iot__test__access__https_8h.html</anchorfile>
      <anchor>a0837dba986cf5b12f365ea3aef165413</anchor>
      <arglist>(IotHttpsResponseHandle_t *pRespHandle, IotHttpsResponseInfo_t *pRespInfo, IotHttpsRequestHandle_t reqHandle)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotTestHttps_networkReceiveCallback</name>
      <anchorfile>iot__test__access__https_8h.html</anchorfile>
      <anchor>a551966527aca8c83a917e36b53e44022</anchor>
      <arglist>(void *pNetworkConnection, void *pReceiveContext)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_test_access_https_client.c</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/https/test/access/</path>
    <filename>iot__test__access__https__client_8c.html</filename>
    <member kind="function">
      <type>IotHttpsReturnCode_t</type>
      <name>IotTestHttps_initializeResponse</name>
      <anchorfile>iot__test__access__https__client_8c.html</anchorfile>
      <anchor>a0837dba986cf5b12f365ea3aef165413</anchor>
      <arglist>(IotHttpsResponseHandle_t *pRespHandle, IotHttpsResponseInfo_t *pRespInfo, IotHttpsRequestHandle_t reqHandle)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>IotTestHttps_networkReceiveCallback</name>
      <anchorfile>iot__test__access__https__client_8c.html</anchorfile>
      <anchor>a551966527aca8c83a917e36b53e44022</anchor>
      <arglist>(void *pNetworkConnection, void *pReceiveContext)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_tests_https_async.c</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/https/test/unit/</path>
    <filename>iot__tests__https__async_8c.html</filename>
    <includes id="iot__tests__https__common_8h" name="iot_tests_https_common.h" local="yes" imported="no">iot_tests_https_common.h</includes>
    <class kind="struct">_asyncVerificationParams_t</class>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_ASYNC_TIMEOUT_MS</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a673c7e3784a66ad17e843970dbff82f5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_MAX_ASYNC_REQUESTS</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>aed173ef8a5ad8d668839862a8bedb275</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_NETWORK_RECEIVE_CALLBACK_WAIT_MS</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a13d6b04d1816a59cb4e71f50e6fdace0</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_invokeNetworkReceiveCallback</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a58deecc923b8b254d40be9851220f8b3</anchor>
      <arglist>(void *pArgument)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkSendSuccess</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>ad5e9de0bbbcf93bc1753c9462d872864</anchor>
      <arglist>(void *pConnection, const uint8_t *pMessage, size_t messageLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkSendFailHeaders</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>aca123f9dabe9cc202b86aee13efd1638</anchor>
      <arglist>(void *pConnection, const uint8_t *pMessage, size_t messageLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkSendFailBody</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>aab6659adb9fd218ea47cfa6c765c6675</anchor>
      <arglist>(void *pConnection, const uint8_t *pMessage, size_t messageLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkReceiveFailHeaders</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a1666a51b042eb7e866880bffb66d158c</anchor>
      <arglist>(void *pConnection, uint8_t *pBuffer, size_t bytesRequested)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkReceiveFailBody</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a491336a85eed0d4f6cb91a24a1d76fc1</anchor>
      <arglist>(void *pConnection, uint8_t *pBuffer, size_t bytesRequested)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_httpParserExecuteFailHeaders</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>acc82f47a045c5e11bbf74952f94ee8f5</anchor>
      <arglist>(http_parser *parser, const http_parser_settings *settings, const char *data, size_t len)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_httpParserExecuteFailBody</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a1e241765a990f76ea4abf6786f4c6864</anchor>
      <arglist>(http_parser *parser, const http_parser_settings *settings, const char *data, size_t len)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>_networkSendFailsOnSecondHeaderSend</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a0047b4bde9b89d955bb4b7ac1fb5e4aa</anchor>
      <arglist>(void *pConnection, const uint8_t *pMessage, size_t messageLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkSendSuccessWithSettingParseFailForHeaders</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a2263a85e0f592a47e039f07aa42a5a72</anchor>
      <arglist>(void *pConnection, const uint8_t *pMessage, size_t messageLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkSendSuccessWithSettingParseFailForBody</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a61d50c134affd1fc498cc9568b79889b</anchor>
      <arglist>(void *pConnection, const uint8_t *pMessage, size_t messageLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_appendHeaderCallback</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a9c5f501ed12e168263d987682b8358eb</anchor>
      <arglist>(void *pPrivData, IotHttpsRequestHandle_t reqHandle)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_writeCallback</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a27783adbdf4e771cdc3295490a589f06</anchor>
      <arglist>(void *pPrivData, IotHttpsRequestHandle_t reqHandle)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_readReadyCallback</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a46c0ce65c07eebbf566e22aa3e648d3d</anchor>
      <arglist>(void *pPrivData, IotHttpsResponseHandle_t respHandle, IotHttpsReturnCode_t rc, uint16_t status)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_responseCompleteCallback</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a6bf99df60d0ff91bdc5b887602525221</anchor>
      <arglist>(void *pPrivData, IotHttpsResponseHandle_t respHandle, IotHttpsReturnCode_t rc, uint16_t status)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_connectionClosedCallback</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>af5406b0457848c1b4fb7d56e731bec4b</anchor>
      <arglist>(void *pPrivData, IotHttpsConnectionHandle_t connHandle, IotHttpsReturnCode_t rc)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_errorCallback</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a798ab42b05662e4cc60706defd131f2a</anchor>
      <arglist>(void *pPrivData, IotHttpsRequestHandle_t reqHandle, IotHttpsResponseHandle_t respHandle, IotHttpsReturnCode_t rc)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_appendHeaderCallbackThatCancels</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a55b0342118c96e29b76cef47e4bcf94a</anchor>
      <arglist>(void *pPrivData, IotHttpsRequestHandle_t reqHandle)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_writeCallbackThatCancels</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a5e29a47e2f5f378e96bd9a3ccd55bdc4</anchor>
      <arglist>(void *pPrivData, IotHttpsRequestHandle_t reqHandle)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_readReadyCallbackThatCancels</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a37e6aba4262a3011fddd7636ee3d8ff7</anchor>
      <arglist>(void *pPrivData, IotHttpsResponseHandle_t respHandle, IotHttpsReturnCode_t rc, uint16_t status)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_GROUP</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>aa044897fb496ac6113ce924ce921043b</anchor>
      <arglist>(HTTPS_Client_Unit_Async)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_SETUP</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a828d6d2e59af7e40fbc23491c84c036b</anchor>
      <arglist>(HTTPS_Client_Unit_Async)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_TEAR_DOWN</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a7b2f6d48d4aab49b23025fe1dadae2c4</anchor>
      <arglist>(HTTPS_Client_Unit_Async)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_GROUP_RUNNER</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a7fff32edf317a292e145cff528f4d76a</anchor>
      <arglist>(HTTPS_Client_Unit_Async)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a50452495a77b457f3d0dc57fa37e0a06</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncInvalidParameters)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>ab70b052618b04d884f9ef42ba821f2a0</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncFailureSendingHeaders)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>aa5f3b35ec5bc5028f068f7e0abcbc3d6</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncFailureSendingBody)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a140c352ed13d22aca7124ac6e3f3de5c</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncFailureReceivingHeaders)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a2cec51dfc804e9b57fc7e1612cbb7f5b</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncFailureReceivingBody)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a00768664e71c2687972ed32e80f34c89</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncFailureParsingHeaders)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a4803ad8bd31caff14b8c09d301d1a22c</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncFailureParsingBody)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a4ed3bb324cc54861969963ad0d603079</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncSomeBodyInHeaderBuffer)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a8ec712f67c2fba1394ee47dffbeef4b2</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncSomeHeaderInBodyBuffer)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a38603533386af0259e47042184f52c3f</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncEntireResponseInHeaderBuffer)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>ac6398ac7853d97b4d37c846d31981a43</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncBodyTooLarge)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a34d81484cd4594c5195e87d5885b9fac</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncIgnoreResponseBody)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>abb3669b891069f4b25a82104b98d1568</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncCancelBeforeScheduled)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>aaf84cb0215fcfdf4efb1d04eb0ed57ae</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncCancelDuringAppendHeaderCallback)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>aa2affbde6cbfb189fa8e3d1b9ea72643</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncCancelDuringWriteCallback)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a2821b7bda9c7ef1393961d7b2cd22bc7</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncCancelDuringReadReadyCallback)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a890f796fff32fae9a2c500549904d5ee</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsSuccess)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a170279420fb122c44e7db95612b24724</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsSecondHasNetworkSendFailure)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a80529c4f4bab77d949d11fc1dbbdff48</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsFirstHasNetworkReceiveFailure)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>af5114fa33f90a7167e111651ef59063c</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsFirstHasParsingFailure)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>afb49e064942f4f6a462bba29acd89e06</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsFirstIsNonPersistent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a524690f910aa3df437d1db703cf6e462</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsFirstIgnoresPresentResponseBody)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>afe4581c5dd9773f12e695e780badbf25</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncMultipleRequestsOneGetsCancelled)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a57ba369de04eb85a4b4e147c1942fcc6</anchor>
      <arglist>(HTTPS_Client_Unit_Async, SendAsyncChunkedResponse)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static _asyncVerificationParams_t</type>
      <name>_verifParams</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>ab3f8534bbdc1ab6b28699db444519fdf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static uint8_t</type>
      <name>_pAsyncRespUserBuffers</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a5883086df3f68432bd0fb90876e75ed7</anchor>
      <arglist>[HTTPS_TEST_MAX_ASYNC_REQUESTS][HTTPS_TEST_RESP_USER_BUFFER_SIZE]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static uint8_t</type>
      <name>_pAsyncReqUserBuffers</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a308004c31c7b7b6d813cac83ac3fa744</anchor>
      <arglist>[HTTPS_TEST_MAX_ASYNC_REQUESTS][HTTPS_TEST_REQ_USER_BUFFER_SIZE]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsRequestInfo_t</type>
      <name>_pAsyncReqInfos</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a99b4e3a81b15734007fedb4e24e0c895</anchor>
      <arglist>[HTTPS_TEST_MAX_ASYNC_REQUESTS]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsResponseInfo_t</type>
      <name>_pAsyncRespInfos</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a34fa7a6df361be0c1d154d3fe8ba5686</anchor>
      <arglist>[HTTPS_TEST_MAX_ASYNC_REQUESTS]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsRequestHandle_t</type>
      <name>_pAsyncRequestHandles</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>ad0edaabbf3540decc079d8cc2c80ee82</anchor>
      <arglist>[HTTPS_TEST_MAX_ASYNC_REQUESTS]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsResponseHandle_t</type>
      <name>_pAsyncResponseHandles</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>a013dd3420b1fae336eecc5ac4ec2da90</anchor>
      <arglist>[HTTPS_TEST_MAX_ASYNC_REQUESTS]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsAsyncInfo_t</type>
      <name>_asyncInfoBase</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>ad5e4587070d5a9f322dd9871839355ac</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsRequestInfo_t</type>
      <name>_reqInfoBase</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>abb2395fce5626873afaf41405a4e84ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsResponseInfo_t</type>
      <name>_respInfoBase</name>
      <anchorfile>iot__tests__https__async_8c.html</anchorfile>
      <anchor>aad606c386fd499a87fd5b429b971056f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_tests_https_client.c</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/https/test/unit/</path>
    <filename>iot__tests__https__client_8c.html</filename>
    <includes id="iot__tests__https__common_8h" name="iot_tests_https_common.h" local="yes" imported="no">iot_tests_https_common.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_REQUEST_LINE_WITHOUT_METHOD</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a77fefadb0ca6fcea451a6c7c052f5491</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_REQUEST_LINE_WITHOUT_PATH_WITHOUT_METHOD</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a09734adfb6d3e758cc5aedd12424eed7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_USER_AGENT_HEADER_LINE</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a49316f12bcdbf94d57bb759abe9175e7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_HOST_HEADER_LINE</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a692ac7602e1b0c480f595de48ab7d329</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_RESPONSE_HEADER_LINES</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>adced9b7d706dbdd6c3776ed6b39e91f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_RESPONSE_HEADER_LINES_LENGTH</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>ad941e226e253b935e1f11bb9e1a814f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_RESPONSE_HEADER_LINES_NO_CONTENT_LENGTH</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>ab726afbe4567b92ebc4f9807e3f24e88</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_RESPONSE_HEADER_LINES_NO_CONTENT_LENGTH_LENGTH</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>afa3e5c492754ef4362c54fef229fe477</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_DATE_HEADER</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>af77b18fe66ed331f78c5e713240fc193</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_ETAG_HEADER</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>aabb5851494c99d3f3bdb642da48463a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_NONEXISTENT_HEADER</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a866407820e1e38b790376a3d71970f03</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_DATE_HEADER_VALUE</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a1be0fef5c50476ddb222af0470d4a5f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_ETAG_HEADER_VALUE</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>aeb36d29d6da3ebdc39b02b08025bc721</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_CONTENT_LENGTH_VALUE</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a38d11b693f7854637bd240ed86dfa1e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_VALUE_BUFFER_LENGTH_LARGE_ENOUGH</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a0b2fa25aab976ce48b044159c83698de</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_VALUE_BUFFER_LENGTH_TOO_SMALL</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a9a171125f1c2ea9f4e53bfb36afc4bc6</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkReceiveFail</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a5791dbca330600bf387bb4e61801123f</anchor>
      <arglist>(void *pConnection, uint8_t *pBuffer, size_t bytesRequested)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkSendSuccess</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>ad5e9de0bbbcf93bc1753c9462d872864</anchor>
      <arglist>(void *pConnection, const uint8_t *pMessage, size_t messageLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkSendFail</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a8b683b934ff6f3c85f4b6ea3167ebebe</anchor>
      <arglist>(void *pConnection, const uint8_t *pMessage, size_t messageLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotNetworkError_t</type>
      <name>_networkCloseFail</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a607756709646417ecb289045848c6179</anchor>
      <arglist>(void *pConnection)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotNetworkError_t</type>
      <name>_networkDestroyFail</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>aca8d4e5d99ed3da87c68b7bda2ea1fec</anchor>
      <arglist>(void *pConnection)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotNetworkError_t</type>
      <name>_networkCreateFail</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a6c70b6046dec16d54f73028fbd7cc81e</anchor>
      <arglist>(void *pConnectionInfo, void *pCredentialInfo, void **pConnection)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotNetworkError_t</type>
      <name>_setReceiveCallbackFail</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a827535c782594dbb5a042b792a4de96e</anchor>
      <arglist>(void *pConnection, IotNetworkReceiveCallback_t receiveCallback, void *pContext)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_httpParserExecuteFail</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a864ddf732f6fb5e127cdf43bbb5cc8d0</anchor>
      <arglist>(http_parser *parser, const http_parser_settings *settings, const char *data, size_t len)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_GROUP</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>adc3a09661a507ccbcea1e602f3b49bf0</anchor>
      <arglist>(HTTPS_Client_Unit_API)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_SETUP</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a3e5af6c3c3ff05cad681cec6ddfecfba</anchor>
      <arglist>(HTTPS_Client_Unit_API)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_TEAR_DOWN</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a0df31f60ffe0cd167aa49cb53115848a</anchor>
      <arglist>(HTTPS_Client_Unit_API)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_GROUP_RUNNER</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a84fe8af28e183eef79ea8ecd8564e62f</anchor>
      <arglist>(HTTPS_Client_Unit_API)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a7e52d4e8a8fdbebcf9b0156e1fed39df</anchor>
      <arglist>(HTTPS_Client_Unit_API, ConnectInvalidParameters)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a85d6cc3ce6cd7831bd90e08f02541981</anchor>
      <arglist>(HTTPS_Client_Unit_API, ConnectFailure)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a53626f11657047080e878d341e060e8c</anchor>
      <arglist>(HTTPS_Client_Unit_API, ConnectSuccess)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a384b74624d1cde80a279909d6ca803b9</anchor>
      <arglist>(HTTPS_Client_Unit_API, DisconnectInvalidParameters)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a05caefa789e14f256cb76fa7ae131f93</anchor>
      <arglist>(HTTPS_Client_Unit_API, DisconnectFailure)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a38769f2c56e07c5fdee7fb50560befbe</anchor>
      <arglist>(HTTPS_Client_Unit_API, DisconnectSuccess)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>aa9f3054934c7d99b678787cba04ebae5</anchor>
      <arglist>(HTTPS_Client_Unit_API, InitializeRequestInvalidParameters)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>af93ebf9ed838fb6b9912d10a08f41a63</anchor>
      <arglist>(HTTPS_Client_Unit_API, InitializeRequestFormatCheck)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a2127425448a481b8fb6a2358e4e92d75</anchor>
      <arglist>(HTTPS_Client_Unit_API, AddHeaderInvalidParameters)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>ad14f897b86e96d76bd2e492f1e1e8181</anchor>
      <arglist>(HTTPS_Client_Unit_API, AddHeaderFormatCheck)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>aa6d75cd3b57547a3edf6daf1d1f73dcb</anchor>
      <arglist>(HTTPS_Client_Unit_API, AddHeaderMultipleHeaders)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a27f6f83dfd46965162f1e8621080cd1c</anchor>
      <arglist>(HTTPS_Client_Unit_API, ReadHeaderInvalidParameters)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a2c7ff53c287b269b96aec92c46a0d6d8</anchor>
      <arglist>(HTTPS_Client_Unit_API, ReadHeaderVaryingValues)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a89365b53d646e27efd6a90765ff07ad9</anchor>
      <arglist>(HTTPS_Client_Unit_API, ReadContentLengthInvalidParameters)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a5349e29e3b4fc3b42397fa2ed50d7251</anchor>
      <arglist>(HTTPS_Client_Unit_API, ReadContentLengthSuccess)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a42cf176b2763d0b88ac90a88245af460</anchor>
      <arglist>(HTTPS_Client_Unit_API, ReadContentLengthNotFound)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a762f64c3c16ffcad3a78a4ba0a6a1c5d</anchor>
      <arglist>(HTTPS_Client_Unit_API, ReadResponseStatusInvalidParameters)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a2b56fb8b21267adb933806290cbf0339</anchor>
      <arglist>(HTTPS_Client_Unit_API, ReadResponseStatusSuccess)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>acb3595a34725d967304ffae6d54ebbca</anchor>
      <arglist>(HTTPS_Client_Unit_API, WriteRequestBodyInvalidParameters)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a69c702efa0df1b585d49445a46cfc9fb</anchor>
      <arglist>(HTTPS_Client_Unit_API, WriteRequestBodySuccess)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a4c6e37c19951edf715223d8f2c96f4c5</anchor>
      <arglist>(HTTPS_Client_Unit_API, WriteRequestBodyNetworkSendFailure)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>ad6fb52d70e37a8acb7c3d79ad95dbcfb</anchor>
      <arglist>(HTTPS_Client_Unit_API, ReadResponseBodyInvalidParameters)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a63ea76e8806a5df30345f78d0bb40e86</anchor>
      <arglist>(HTTPS_Client_Unit_API, ReadResponseBodyNetworkReceiveFailure)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>af3b9f0b24646238546a788d60c57f8d0</anchor>
      <arglist>(HTTPS_Client_Unit_API, ReadResponseBodyParsingFailure)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a65893a88a3b7582751d38bf82a28bd1d</anchor>
      <arglist>(HTTPS_Client_Unit_API, ReadResponseBodySuccess)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsAsyncInfo_t</type>
      <name>_asyncInfo</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>aee0b0d65068c39a3c7cfc07a2628725b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsRequestInfo_t</type>
      <name>_reqInfo</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>a3fbdd9d4372ce1d2df8e67320cf0e6ee</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsResponseInfo_t</type>
      <name>_respInfo</name>
      <anchorfile>iot__tests__https__client_8c.html</anchorfile>
      <anchor>abcff0bcbabbdf0c5eb26a4a79f366048</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_tests_https_common.c</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/https/test/unit/</path>
    <filename>iot__tests__https__common_8c.html</filename>
    <includes id="iot__tests__https__common_8h" name="iot_tests_https_common.h" local="yes" imported="no">iot_tests_https_common.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE</name>
      <anchorfile>iot__tests__https__common_8c.html</anchorfile>
      <anchor>a09512780ca7ceac95d29e97e24afbba3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_GENERIC_RESPONSE_STATUS_LINE_LENGTH</name>
      <anchorfile>iot__tests__https__common_8c.html</anchorfile>
      <anchor>aec8eaff61a359090476fda6f39e6ff34</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_GENERIC_HEADER</name>
      <anchorfile>iot__tests__https__common_8c.html</anchorfile>
      <anchor>a069d28b470856466e5d00585081fd4f9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_GENERIC_VALUE</name>
      <anchorfile>iot__tests__https__common_8c.html</anchorfile>
      <anchor>ac2c17bffccaf08f4c32a9396341ef515</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MAX_UINT32_DIGITS</name>
      <anchorfile>iot__tests__https__common_8c.html</anchorfile>
      <anchor>ad4e8a093db2bc14233c6ab44af44a527</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MIN_UINT32_DIGITS</name>
      <anchorfile>iot__tests__https__common_8c.html</anchorfile>
      <anchor>aac8761a16b884350ac0b0a4908c6a502</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MAX_GENERIC_HEADER_LINE_LENGTH</name>
      <anchorfile>iot__tests__https__common_8c.html</anchorfile>
      <anchor>ab96ec7984cffc691e87e98ded55333b5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MIN_GENERIC_HEADER_LINE_LENGTH</name>
      <anchorfile>iot__tests__https__common_8c.html</anchorfile>
      <anchor>ae0a43451a0c2059d6032a3199ad892db</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_GENERIC_BODY_STARTING_CHAR</name>
      <anchorfile>iot__tests__https__common_8c.html</anchorfile>
      <anchor>ae831659bb97063920fc35d6e34ba930d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NUM_LETTERS_IN_ALPHABET</name>
      <anchorfile>iot__tests__https__common_8c.html</anchorfile>
      <anchor>af3c1de1127032e424cb4f9c6ba0a9379</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>_generateHttpResponseMessage</name>
      <anchorfile>iot__tests__https__common_8c.html</anchorfile>
      <anchor>abe75c616d8663168445bfdf91816a86c</anchor>
      <arglist>(int headerLength, int bodyLength)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>_verifyHttpResponseBody</name>
      <anchorfile>iot__tests__https__common_8c.html</anchorfile>
      <anchor>a48a48d644929652c7cc185d1833663e0</anchor>
      <arglist>(int bodyLength, uint8_t *pBody, int startIndex)</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>_pConnUserBuffer</name>
      <anchorfile>iot__tests__https__common_8c.html</anchorfile>
      <anchor>a87f3e61d7b6d7f1de245544fb46d02d8</anchor>
      <arglist>[HTTPS_TEST_CONN_USER_BUFFER_SIZE]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>_pReqUserBuffer</name>
      <anchorfile>iot__tests__https__common_8c.html</anchorfile>
      <anchor>ad5c631705e4bb6c146345db459a8707b</anchor>
      <arglist>[HTTPS_TEST_REQ_USER_BUFFER_SIZE]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>_pRespUserBuffer</name>
      <anchorfile>iot__tests__https__common_8c.html</anchorfile>
      <anchor>a9bd5c5453697f8e0de0381ff0e2dcba2</anchor>
      <arglist>[HTTPS_TEST_RESP_USER_BUFFER_SIZE]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>_pRespBodyBuffer</name>
      <anchorfile>iot__tests__https__common_8c.html</anchorfile>
      <anchor>aa8ed9251ac1d66e76948aac1399acb5c</anchor>
      <arglist>[HTTPS_TEST_RESP_BODY_BUFFER_SIZE]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>_pRespMessageBuffer</name>
      <anchorfile>iot__tests__https__common_8c.html</anchorfile>
      <anchor>aa482ad886658e5374ab85764c12722cb</anchor>
      <arglist>[HTTPS_TEST_RESPONSE_MESSAGE_LENGTH]</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_tests_https_common.h</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/https/test/unit/</path>
    <filename>iot__tests__https__common_8h.html</filename>
    <includes id="iot__test__access__https_8h" name="iot_test_access_https.h" local="yes" imported="no">iot_test_access_https.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_PORT</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>ad9037b3f4c69f16e2306868f63f357be</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_ADDRESS</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a6606bf300c4804fc1686255e6d6c61fc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_PATH</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a96fcc34c3e29cd3c517e9cca21819358</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_ALPN_PROTOCOL</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>ace96e4283f2467fe0bd7b1264d12f48e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_ROOT_CA</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>aa815f57c2be9428d1f3ac757206cb9f2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_CONN_USER_BUFFER_SIZE</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a52a4c6b84ffa1863f450cc083f2ba617</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_REQ_USER_BUFFER_SIZE</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a93ad60f94f45da2fb7806f5793d424d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_RESP_USER_BUFFER_SIZE</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a2aa3e746eea8f875cfb3dcf2c1b6c83b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_RESP_BODY_BUFFER_SIZE</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a810b02cb11b6d604937130508b8bacc6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_RESPONSE_MESSAGE_LENGTH</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a053a3504d19d5081037c5a1879db09d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_RESP_HEADER_BUFFER_LENGTH</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a27e8d3fa595d856859b0ae7ad9aab42d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_REQUEST_BODY</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a379f3b1c54af1ac4ecf2756a3679fd14</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_REQUEST_BODY_LENGTH</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a8f19386ad0a20bdcdc4b3779c15dd637</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_SMALL_RESPONSE</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a5576f84b122d705d587a19ad1bfa1561</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_SMALL_RESPONSE_LENGTH</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a821d26e6b3958b701d502743109e8db2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_CHUNKED_RESPONSE</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a1f0b26e3aa85973cb74b437db5ed8b32</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_CHUNKED_RESPONSE_BODY_LENGTH</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a9bc7f9a7d3071c56de0244da2878896e</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotNetworkError_t</type>
      <name>_networkCreateSuccess</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>ad1f8d9cce160371f966d642a1b2703e0</anchor>
      <arglist>(void *pConnectionInfo, void *pCredentialInfo, void **pConnection)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotNetworkError_t</type>
      <name>_setReceiveCallbackSuccess</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a5752892d3afcb207852705c0b748740e</anchor>
      <arglist>(void *pConnection, IotNetworkReceiveCallback_t receiveCallback, void *pContext)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsConnectionHandle_t</type>
      <name>_getConnHandle</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>aea40693c8398011f74eb58f73eda4556</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsRequestHandle_t</type>
      <name>_getReqHandle</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a806b1ae06f4bb70deb4c4c33ec0d2d4f</anchor>
      <arglist>(IotHttpsRequestInfo_t *pReqInfo)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotHttpsResponseHandle_t</type>
      <name>_getRespHandle</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a21d0dac5b62b908a2e5a98b732e22e9b</anchor>
      <arglist>(IotHttpsResponseInfo_t *pRespInfo, IotHttpsRequestHandle_t reqHandle)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotNetworkError_t</type>
      <name>_networkCloseSuccess</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a04b9447804bbdf407b541bdb0d28b105</anchor>
      <arglist>(void *pConnection)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IotNetworkError_t</type>
      <name>_networkDestroySuccess</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>ac4d1150bca06aa068d30f38d13c52036</anchor>
      <arglist>(void *pConnection)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkReceiveSuccess</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a638651fcb838dca4fbb595ef0127d501</anchor>
      <arglist>(void *pConnection, uint8_t *pBuffer, size_t bytesRequested)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>_generateHttpResponseMessage</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>abe75c616d8663168445bfdf91816a86c</anchor>
      <arglist>(int headerLength, int bodyLength)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>_verifyHttpResponseBody</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a48a48d644929652c7cc185d1833663e0</anchor>
      <arglist>(int bodyLength, uint8_t *pBody, int startIndex)</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>_pConnUserBuffer</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a87f3e61d7b6d7f1de245544fb46d02d8</anchor>
      <arglist>[HTTPS_TEST_CONN_USER_BUFFER_SIZE]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>_pRespBodyBuffer</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>aa8ed9251ac1d66e76948aac1399acb5c</anchor>
      <arglist>[HTTPS_TEST_RESP_BODY_BUFFER_SIZE]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>_pRespMessageBuffer</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>aa482ad886658e5374ab85764c12722cb</anchor>
      <arglist>[HTTPS_TEST_RESPONSE_MESSAGE_LENGTH]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>_pReqUserBuffer</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>ad5c631705e4bb6c146345db459a8707b</anchor>
      <arglist>[HTTPS_TEST_REQ_USER_BUFFER_SIZE]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>_pRespUserBuffer</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a9bd5c5453697f8e0de0381ff0e2dcba2</anchor>
      <arglist>[HTTPS_TEST_RESP_USER_BUFFER_SIZE]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static uint32_t</type>
      <name>_nextRespMessageBufferByteToReceive</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>a6cc4fc32c0eb755fdbde0b73e75962a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotNetworkInterface_t</type>
      <name>_networkInterface</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>ada4f6af66c4aae54a7f578a183980ccc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsConnectionInfo_t</type>
      <name>_connInfo</name>
      <anchorfile>iot__tests__https__common_8h.html</anchorfile>
      <anchor>ad1fd1598bf79ff303f26c958e4e6a7c7</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_tests_https_sync.c</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/https/test/unit/</path>
    <filename>iot__tests__https__sync_8c.html</filename>
    <includes id="iot__tests__https__common_8h" name="iot_tests_https_common.h" local="yes" imported="no">iot_tests_https_common.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_SYNC_TIMEOUT_MS</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>ac6ba52ae7d38e53e9e5d42812824bf93</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_NETWORK_RECEIVE_CALLBACK_WAIT_MS</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a13d6b04d1816a59cb4e71f50e6fdace0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_SMALL_RESPONSE_UP_TO_CARRIAGE_RETURN</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a6add49e6ac2a3a80a7bc8cd2ab57b4eb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_SMALL_RESPONSE_UP_TO_NEWLINE</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a8ec6c145b63148926790e96b81136549</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_SMALL_RESPONSE_UP_TO_COLON</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a123624e4f208d10b0534585ebf880f32</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_SMALL_RESPONSE_UP_TO_SPACE</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a058977aa6b79feb48642455ea9fb0d63</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_SMALL_RESPONSE_UP_TO_SPACE_IN_BETWEEN_VALUE</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a18726c56518658f7c067bf9d7148ebad</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_HEADER1</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>aa4bc168bd35d85af1fc5b24ca835f5ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_HEADER1_PLUS_COLON</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>aa8aec64b1664bf928f96759eab5c7112</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_HEADER1_PLUS_SPACE</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a03d48c9cb9c3521b799c05ebd539c272</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_HEADER2</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a3677d08822b8d8b5bb74cae8b89d8aa7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_HEADER_VALUE1</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a1e831a115cc434244ee33fc7c4a0eb4d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_HEADER_VALUE2_PLUS_SPACE</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a33027e1189b089cde7bc45bd2a8e3c3d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_HEADER_VALUE2_VALUE2A</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>aebfa4b9b6341df841de6bf9b8314f341</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_HEADER_VALUE1_PLUS_CARRIAGE_RETURN</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>ac978debc08dc2ca94080f5acf06deb9f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_HEADER_VALUE1_PLUS_NEWLINE</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a1c7cd426cc08be0eb6fae60eef716ae4</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_invokeNetworkReceiveCallback</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a58deecc923b8b254d40be9851220f8b3</anchor>
      <arglist>(void *pArgument)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_httpParserExecuteFailHeaders</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>acc82f47a045c5e11bbf74952f94ee8f5</anchor>
      <arglist>(http_parser *parser, const http_parser_settings *settings, const char *data, size_t len)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_httpParserExecuteFailBody</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a1e241765a990f76ea4abf6786f4c6864</anchor>
      <arglist>(http_parser *parser, const http_parser_settings *settings, const char *data, size_t len)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkSendFailHeaders</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>aca123f9dabe9cc202b86aee13efd1638</anchor>
      <arglist>(void *pConnection, const uint8_t *pMessage, size_t messageLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkSendFailBody</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>aab6659adb9fd218ea47cfa6c765c6675</anchor>
      <arglist>(void *pConnection, const uint8_t *pMessage, size_t messageLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkSendSuccess</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>ad5e9de0bbbcf93bc1753c9462d872864</anchor>
      <arglist>(void *pConnection, const uint8_t *pMessage, size_t messageLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkReceiveFailHeaders</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a1666a51b042eb7e866880bffb66d158c</anchor>
      <arglist>(void *pConnection, uint8_t *pBuffer, size_t bytesRequested)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkReceiveFailBody</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a491336a85eed0d4f6cb91a24a1d76fc1</anchor>
      <arglist>(void *pConnection, uint8_t *pBuffer, size_t bytesRequested)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkSendSuccessWithSettingParseFailForHeaders</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a2263a85e0f592a47e039f07aa42a5a72</anchor>
      <arglist>(void *pConnection, const uint8_t *pMessage, size_t messageLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>_networkSendSuccessWithSettingParseFailForBody</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a61d50c134affd1fc498cc9568b79889b</anchor>
      <arglist>(void *pConnection, const uint8_t *pMessage, size_t messageLength)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_GROUP</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>ae75843d587a46559e8917d5d860f0055</anchor>
      <arglist>(HTTPS_Client_Unit_Sync)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_SETUP</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a42371c8132b161a170df49ebe878fb1b</anchor>
      <arglist>(HTTPS_Client_Unit_Sync)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_TEAR_DOWN</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a90bbd8e2414e200c71881df4679f82a1</anchor>
      <arglist>(HTTPS_Client_Unit_Sync)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_GROUP_RUNNER</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>ab9aba70f7855c9453611a5e13841b8c0</anchor>
      <arglist>(HTTPS_Client_Unit_Sync)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>abe534ef13ba66db68f05ee7fa140bbdd</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncInvalidParameters)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a358b6111dbbda0df11c88bb0c14e5e25</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncFailureSendingHeaders)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a358bf214936890eced7dfdae96f3e6c5</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncFailureSendingBody)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a7a22dfe47886b9a96b056a0154f727f8</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncFailureReceivingHeaders)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>afbc25d01d4a8e3a9bfa2588c078e0aaf</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncFailureReceivingBody)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>ae048b51e97a53ac97df5b466e6bcb71d</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncFailureParsingHeaders)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a6a4616b905b56178f65cc9d3f98b7a5a</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncFailureParsingBody)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>ac2aa26e3df7dd77e95cee1bfdf86f6ba</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncSomeBodyInHeaderBuffer)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>acd12056cadef25c0e0489a86afa190c0</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncSomeHeaderInBodyBuffer)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>aaaa89ad7888cc3b45344228e581eaaf0</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncEntireResponseInHeaderBuffer)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a68da84e37d6799089a7241241ef65ebd</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncBodyTooLarge)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>aaec42b77054b9a6b32bc5acf62bc183e</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncBodyBufferNull)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a5b50b8c0608d112a716b89d8a1ee6f58</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncPersistentRequest)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a287b0d2fd70134df39e8ef585a1809ee</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncNonPersistentRequest)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a272dc897e8ece8d24ed1336eb4fb186d</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncHeadersEndsWithCarriageReturnSeparator)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>ac7ad98499b1575094c1fc727f86bf34c</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncHeadersEndsWithNewlineSeparator)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>ac67fa4d10ff84f520ed47a4919ddc172</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncHeadersEndsWithColonSeparator)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>ac9b831a8b2a5f43b283fb78bb452da6a</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncHeadersEndsWithSpaceSeparator)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a0ed4011e0c8f905cbfa459cccc0025ff</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncHeadersEndsWithSpaceAfterHeaderValue)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a5dc8014478b9268704de101f34ff4163</anchor>
      <arglist>(HTTPS_Client_Unit_Sync, SendSyncChunkedResponse)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsConnectionHandle_t</type>
      <name>_receiveCallbackConnHandle</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a827bb6c739ad9b8fd0f1f8e7a0ad9cec</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static bool</type>
      <name>_alreadyCreatedReceiveCallbackThread</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a9980d6f15f510d77c57390914244e348</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsRequestHandle_t</type>
      <name>_currentlySendingRequestHandle</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a464f7677f911de658193c6a6b039aa4a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsSyncInfo_t</type>
      <name>_syncRequestInfo</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>abb374ef88d16464eb17c214302e62b23</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsSyncInfo_t</type>
      <name>_syncResponseInfo</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a1ff71526289dd67edcfb34c1697ffc94</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsRequestInfo_t</type>
      <name>_reqInfo</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>a3fbdd9d4372ce1d2df8e67320cf0e6ee</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsResponseInfo_t</type>
      <name>_respInfo</name>
      <anchorfile>iot__tests__https__sync_8c.html</anchorfile>
      <anchor>abcff0bcbabbdf0c5eb26a4a79f366048</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_tests_https_system.c</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/https/test/system/</path>
    <filename>iot__tests__https__system_8c.html</filename>
    <includes id="iot__https__utils_8h" name="iot_https_utils.h" local="yes" imported="no">iot_https_utils.h</includes>
    <class kind="struct">_asyncVerificationParams_t</class>
    <member kind="define">
      <type>#define</type>
      <name>IOT_TEST_HTTPS_SERVER_HOST_NAME</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a31a81bc47b1f98745dcabc93853e1674</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_TEST_HTTPS_PORT</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>aa081052c4a3b1c794bdc9d02c7d5db44</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_TEST_HTTPS_ALPN_PROTOCOLS</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>afd68a231b9cb0973059fed9d7a8335d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_TEST_HTTPS_ROOT_CA</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a97c7bade471f33ab4a92e76073da72ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_TEST_HTTPS_CLIENT_CERTIFICATE</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>afd41d7fe69e176bf13d2c71a0f4655a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_TEST_HTTPS_CLIENT_PRIVATE_KEY</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a85784ca3fe5a32b28838615c00d6729c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_TEST_HTTPS_SYNC_TIMEOUT_MS</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a3d94afa7b12bb4ed4dae9166cb3cc96f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_TEST_HTTPS_ASYNC_TIMEOUT_MS</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>aa1175956a9d6f67b7b422380744e4612</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_TEST_HTTPS_INITIAL_CONNECTION_RETRY_DELAY</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a646611323a629f1178cff19d0b3077b9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IOT_TEST_HTTPS_CONNECTION_NUM_RETRIES</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>aa788298a80548c2a8d5659fa3996f5d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_CONN_USER_BUFFER_SIZE</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a52a4c6b84ffa1863f450cc083f2ba617</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_REQ_USER_BUFFER_SIZE</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a93ad60f94f45da2fb7806f5793d424d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_RESP_USER_BUFFER_SIZE</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a2aa3e746eea8f875cfb3dcf2c1b6c83b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_RESP_BODY_BUFFER_SIZE</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a810b02cb11b6d604937130508b8bacc6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_GET_REQUEST_PATH</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>aa242df8535f6a84b46a604c1d5d9b026</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_PUT_REQUEST_PATH</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a7e6c7d7157dcdc984559a9cbefabad99</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_POST_REQUEST_PATH</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a8ca14acc9b494eca7c55a888f489f61a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_MESSAGE_BODY</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a86deb11b02b09316b4b8acb5b9ee0b99</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_MESSAGE_BODY_LENGTH</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a7abeb4f8616a00478ef41c66ea2c9396</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_readReadyCallback</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a46c0ce65c07eebbf566e22aa3e648d3d</anchor>
      <arglist>(void *pPrivData, IotHttpsResponseHandle_t respHandle, IotHttpsReturnCode_t rc, uint16_t status)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_responseCompleteCallback</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a6bf99df60d0ff91bdc5b887602525221</anchor>
      <arglist>(void *pPrivData, IotHttpsResponseHandle_t respHandle, IotHttpsReturnCode_t rc, uint16_t status)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_testRequestSynchronous</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>aea2587fd318e0bd4405e708903bb4a2e</anchor>
      <arglist>(bool isNonPersistent, IotHttpsMethod_t method)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>_testRequestAsynchronous</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a6da302932a52406ee80a552af57e7645</anchor>
      <arglist>(bool isNonPersistent, IotHttpsMethod_t method)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_GROUP</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a93608813de5086a66c81cffe2a30cbf6</anchor>
      <arglist>(HTTPS_Client_System)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_SETUP</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>ab604411f0a6b44444c508d6b212ea493</anchor>
      <arglist>(HTTPS_Client_System)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_TEAR_DOWN</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>ad902b3559f63e4be876f12ee5d76088c</anchor>
      <arglist>(HTTPS_Client_System)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_GROUP_RUNNER</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a07ed9d34b5af99b9533b84c0e3167eec</anchor>
      <arglist>(HTTPS_Client_System)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>ab0075fbd407fc4b869673992a37c4e52</anchor>
      <arglist>(HTTPS_Client_System, GetRequestSynchronousPersistent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>aba5b4532b14600b6a303bd0a1813609d</anchor>
      <arglist>(HTTPS_Client_System, GetRequestSynchronousNonPersistent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a4e3e1d4dff27f6b2d4baade48dbd2d89</anchor>
      <arglist>(HTTPS_Client_System, GetRequestAsynchronousPersistent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a9cba83d111609fbb361b6069e12be1be</anchor>
      <arglist>(HTTPS_Client_System, GetRequestAsynchronousNonPersistent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a2a01bc587f12d0db8a80b90aae973d03</anchor>
      <arglist>(HTTPS_Client_System, HeadRequestSynchronousPersistent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a220e834983b9963806e5e8d7901bc2ac</anchor>
      <arglist>(HTTPS_Client_System, HeadRequestSynchronousNonPersistent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>af88901af0da13ed326ae458b45faa3c7</anchor>
      <arglist>(HTTPS_Client_System, HeadRequestAsynchronousPersistent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a81fe08ae400d3a6c91bfec4557c6eb63</anchor>
      <arglist>(HTTPS_Client_System, HeadRequestAsynchronousNonPersistent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a7b5e898cf6fe69cb51d7aa641d4106de</anchor>
      <arglist>(HTTPS_Client_System, PutRequestSynchronousPersistent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>ac29cc01d435de13943237c1260787369</anchor>
      <arglist>(HTTPS_Client_System, PutRequestSynchronousNonPersistent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>adf5d9d214e65327c6e6fef453f35892f</anchor>
      <arglist>(HTTPS_Client_System, PutRequestAsynchronousPersistent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a091c0667dec01cba5fc217794ee12221</anchor>
      <arglist>(HTTPS_Client_System, PutRequestAsynchronousNonPersistent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a29b270187d65742262a443736949c1da</anchor>
      <arglist>(HTTPS_Client_System, PostRequestSynchronousPersistent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>aaa247f1d3b89e281de43b6f8e0c6f341</anchor>
      <arglist>(HTTPS_Client_System, PostRequestSynchronousNonPersistent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>aef9620755fb0ed39a56e7f03474839bd</anchor>
      <arglist>(HTTPS_Client_System, PostRequestAsynchronousPersistent)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>adbc78b690dadd81970f0c662adc7ae2d</anchor>
      <arglist>(HTTPS_Client_System, PostRequestAsynchronousNonPersistent)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static uint8_t</type>
      <name>_pConnUserBuffer</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a87f3e61d7b6d7f1de245544fb46d02d8</anchor>
      <arglist>[HTTPS_TEST_CONN_USER_BUFFER_SIZE]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static uint8_t</type>
      <name>_pReqUserBuffer</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>ad5c631705e4bb6c146345db459a8707b</anchor>
      <arglist>[HTTPS_TEST_REQ_USER_BUFFER_SIZE]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static uint8_t</type>
      <name>_pRespUserBuffer</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>a9bd5c5453697f8e0de0381ff0e2dcba2</anchor>
      <arglist>[HTTPS_TEST_RESP_USER_BUFFER_SIZE]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static uint8_t</type>
      <name>_pRespBodyBuffer</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>aa8ed9251ac1d66e76948aac1399acb5c</anchor>
      <arglist>[HTTPS_TEST_RESP_BODY_BUFFER_SIZE]</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static IotHttpsConnectionInfo_t</type>
      <name>_connInfo</name>
      <anchorfile>iot__tests__https__system_8c.html</anchorfile>
      <anchor>ad1fd1598bf79ff303f26c958e4e6a7c7</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>iot_tests_https_utils.c</name>
    <path>/Users/diveks/Development/doc-test/amazon-freertos/libraries/c_sdk/standard/https/test/unit/</path>
    <filename>iot__tests__https__utils_8c.html</filename>
    <includes id="iot__https__utils_8h" name="iot_https_utils.h" local="yes" imported="no">iot_https_utils.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_URL_HTTPS</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>a34847c123fbb4544eecd8d0fe2887e22</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_URL_HTTPS_EXPECTED_PATH</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>ad1c9c44c0e93f47accded1ced694e636</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TSET_URL_HTTPS_EXPECTED_ADDRESS</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>a71e1560553a23d2af582a4869955a9fe</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_URL_WWW</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>a4eb1a17d6e3dec7d5c173338ff863891</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_URL_WWW_EXPECTED_PATH</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>ac215c4d7b5c947160492ff145273ee50</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_URL_WWW_EXPECTED_ADDRESS</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>a2e4680ef6e282e5e801a0db37536980b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_URL_NO_PATH</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>ac02199c38df52cdbb4793633db869c09</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_URL_NO_PATH_EXPECTED_PATH</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>af1ec2877410f238aacce240f8b3ae7cb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_URL_NO_PATH_EXPECTED_ADDRESS</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>ac9c23ff7113ef3a1d0cb8f34d3e73093</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_URL_NO_ADDRESS</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>ad1bd07c0ae56d5569c705ae275a1a02f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_URL_NO_ADDRESS_EXPECTED_PATH</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>a78d342cdfccd520307bb5c166b4899b9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_URL_NO_ADDRESS_EXPECTED_ADDRESS</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>a48ea860994b9e160fb56eee8e6f3ad65</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTPS_TEST_INVALID_URL</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>ac6f89dd874e27e3599fc2d05bc2b19a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_GROUP</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>a8f370bcc185099f8fb4b61cc000f54dd</anchor>
      <arglist>(HTTPS_Utils_Unit_API)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_SETUP</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>a7f74c712941ae98cde5c0ccc6a8813dc</anchor>
      <arglist>(HTTPS_Utils_Unit_API)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_TEAR_DOWN</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>aaf9981cfe6952478226c5075bc8b65de</anchor>
      <arglist>(HTTPS_Utils_Unit_API)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST_GROUP_RUNNER</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>a928f11a23d0a5d39f07ed0f7338ff11c</anchor>
      <arglist>(HTTPS_Utils_Unit_API)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>a84089bc502bce74deb04b9877277f190</anchor>
      <arglist>(HTTPS_Utils_Unit_API, GetUrlPathInvalidParameters)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>a43faf6fd54659e694fb65b60022a0f9d</anchor>
      <arglist>(HTTPS_Utils_Unit_API, GetUrlPathVerifications)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>ad3bc5587eb8b1006ce16caad04ccb7d6</anchor>
      <arglist>(HTTPS_Utils_Unit_API, GetUrlAddressInvalidParameters)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TEST</name>
      <anchorfile>iot__tests__https__utils_8c.html</anchorfile>
      <anchor>a0f3d167b728aa10a2cdd39693cd2d800</anchor>
      <arglist>(HTTPS_Utils_Unit_API, GetUrlAddressVerifications)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>_asyncVerificationParams_t</name>
    <filename>struct__async_verification_params__t.html</filename>
    <member kind="variable">
      <type>IotSemaphore_t</type>
      <name>completeSem</name>
      <anchorfile>struct__async_verification_params__t.html</anchorfile>
      <anchor>ac2238c9df0ef2e75c695f1ed712bdad9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>numRequestsTotal</name>
      <anchorfile>struct__async_verification_params__t.html</anchorfile>
      <anchor>a3105d2cd3be7d9ce66536805d24b9755</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int8_t</type>
      <name>numRequestsLeft</name>
      <anchorfile>struct__async_verification_params__t.html</anchorfile>
      <anchor>a92f40585b8b4a9eef3d8c5d0ef55b3d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>appendHeaderCallbackCount</name>
      <anchorfile>struct__async_verification_params__t.html</anchorfile>
      <anchor>ae535dae7bcf84b7059bf579bdeb690e8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>writeCallbackCount</name>
      <anchorfile>struct__async_verification_params__t.html</anchorfile>
      <anchor>ad7277cdd7b9edabb125093e872558013</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>readReadyCallbackCount</name>
      <anchorfile>struct__async_verification_params__t.html</anchorfile>
      <anchor>aca2d5d6c734185dfb85948030691d979</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>responseCompleteCallbackCount</name>
      <anchorfile>struct__async_verification_params__t.html</anchorfile>
      <anchor>aa74b6c693b8a01e1ceba2e8ec984f2a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>connectionClosedCallbackCount</name>
      <anchorfile>struct__async_verification_params__t.html</anchorfile>
      <anchor>afcd17dc34c36a9c5422b50f5cc2ae84f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>errorCallbackCount</name>
      <anchorfile>struct__async_verification_params__t.html</anchorfile>
      <anchor>a7cddaeae5de52a01a7f149ac016933d7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsReturnCode_t</type>
      <name>returnCode</name>
      <anchorfile>struct__async_verification_params__t.html</anchorfile>
      <anchor>a2a22305cc8aed1bc36acc5cabc8bb52e</anchor>
      <arglist>[HTTPS_TEST_MAX_ASYNC_REQUESTS]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>readReadyCallbackCountPerResponse</name>
      <anchorfile>struct__async_verification_params__t.html</anchorfile>
      <anchor>ab8484cb4477b4b0db3c6f98911fa9d70</anchor>
      <arglist>[HTTPS_TEST_MAX_ASYNC_REQUESTS]</arglist>
    </member>
    <member kind="variable">
      <type>IotSemaphore_t</type>
      <name>finishedSem</name>
      <anchorfile>struct__async_verification_params__t.html</anchorfile>
      <anchor>a7f4ee52673fcbd341f1ef2a4dd77d76f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>responseStatus</name>
      <anchorfile>struct__async_verification_params__t.html</anchorfile>
      <anchor>ab124ba5e23efbc3d6379ed068416d91e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>contentLength</name>
      <anchorfile>struct__async_verification_params__t.html</anchorfile>
      <anchor>a648e8a1ec25227ea97374bb6f443846e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsReturnCode_t</type>
      <name>contentLengthReturnCode</name>
      <anchorfile>struct__async_verification_params__t.html</anchorfile>
      <anchor>a3d8c2911164acddc4a70ad157038cb15</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bodyReceivedLength</name>
      <anchorfile>struct__async_verification_params__t.html</anchorfile>
      <anchor>a192d28f9ecccdac9b299cc156c443478</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsReturnCode_t</type>
      <name>readResponseBodyReturnCode</name>
      <anchorfile>struct__async_verification_params__t.html</anchorfile>
      <anchor>a5b563831b4c9956b4f1e0d52efd93378</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>_httpParserInfo_t</name>
    <filename>struct__http_parser_info__t.html</filename>
    <member kind="variable">
      <type>http_parser</type>
      <name>responseParser</name>
      <anchorfile>struct__http_parser_info__t.html</anchorfile>
      <anchor>aae7b159627eadf33f6a354c61afa567a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t(*</type>
      <name>parseFunc</name>
      <anchorfile>struct__http_parser_info__t.html</anchorfile>
      <anchor>aa5041feff32683be50c97a417b519c46</anchor>
      <arglist>)(http_parser *parser, const http_parser_settings *settings, const char *data, size_t len)</arglist>
    </member>
    <member kind="variable">
      <type>http_parser</type>
      <name>readHeaderParser</name>
      <anchorfile>struct__http_parser_info__t.html</anchorfile>
      <anchor>abb0775c08465cce55f5740c187750820</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>_httpsConnection_t</name>
    <filename>struct__https_connection__t.html</filename>
    <member kind="variable">
      <type>const IotNetworkInterface_t *</type>
      <name>pNetworkInterface</name>
      <anchorfile>struct__https_connection__t.html</anchorfile>
      <anchor>aced9b77befe13202c0529aa1bc1ea6d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>pNetworkConnection</name>
      <anchorfile>struct__https_connection__t.html</anchorfile>
      <anchor>abbecb163785c0a1928cc011b190be2cc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>timeout</name>
      <anchorfile>struct__https_connection__t.html</anchorfile>
      <anchor>aa4ac0538086e1c822052b1b506dbf538</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>isConnected</name>
      <anchorfile>struct__https_connection__t.html</anchorfile>
      <anchor>abb4305812a5405aced9e294bd2fb2ae7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>isDestroyed</name>
      <anchorfile>struct__https_connection__t.html</anchorfile>
      <anchor>aa94dae2de4071ef0402ae6e96c302395</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotMutex_t</type>
      <name>connectionMutex</name>
      <anchorfile>struct__https_connection__t.html</anchorfile>
      <anchor>a4bf284c9f11de1dc5b670da3c7f5b0bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotDeQueue_t</type>
      <name>reqQ</name>
      <anchorfile>struct__https_connection__t.html</anchorfile>
      <anchor>a21f6a1964d1d046ac6b61ed0e3c90973</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotDeQueue_t</type>
      <name>respQ</name>
      <anchorfile>struct__https_connection__t.html</anchorfile>
      <anchor>aae7979ba42d3b3d662cf3b7e7947c675</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotTaskPoolJobStorage_t</type>
      <name>taskPoolJobStorage</name>
      <anchorfile>struct__https_connection__t.html</anchorfile>
      <anchor>acc4e2d9d19c2c634fc6a232f7a7fc356</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotTaskPoolJob_t</type>
      <name>taskPoolJob</name>
      <anchorfile>struct__https_connection__t.html</anchorfile>
      <anchor>a2e56fa778b8b59a1435c10a4f247100d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>_httpsRequest_t</name>
    <filename>struct__https_request__t.html</filename>
    <member kind="variable">
      <type>IotLink_t</type>
      <name>link</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>a5c0beb75da9975332d1abe9caffb2577</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pHeaders</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>a1966ae98f9697e599878c4f5420a3acd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pHeadersEnd</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>a4ac1d1cfba79baab8b7cf33ddcb95ecb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pHeadersCur</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>a8f8c90cd097116843dcf0ca39dece93b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pBody</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>a8569b4c6f8748a53bf799d72be5b93fd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bodyLength</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>a9401ef3f3fd8a8942d30fd2214caba77</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsMethod_t</type>
      <name>method</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>a0528a756a25bea5cf5b8829f0eda8dba</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsConnectionInfo_t *</type>
      <name>pConnInfo</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>ae2c0755dcc0835e45a22503908941b57</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct _httpsResponse *</type>
      <name>pHttpsResponse</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>a580f2f9bc49581f90de4c8194e93f65f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct _httpsConnection *</type>
      <name>pHttpsConnection</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>ae700482f45d6d2295c7f9a6664ddeacd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>isNonPersistent</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>a228cf694e8b2d13ff26694424170ec16</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>isAsync</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>a9a26deb298b5dd9b9948a1e76cffc9c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>pUserPrivData</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>a8dc218674ffc2f3e5deb1a6be3790d66</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsClientCallbacks_t *</type>
      <name>pCallbacks</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>a0dc22e6d8739fcf7039211877b3240e9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>cancelled</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>a187425d091348cb46a1f27844b585cd4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsReturnCode_t</type>
      <name>bodyTxStatus</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>ade8d1c1afd35208d1973ca9111950f2a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>scheduled</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>a1595423e58c4b45c83f27b729470ee84</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>_httpsResponse_t</name>
    <filename>struct__https_response__t.html</filename>
    <member kind="variable">
      <type>IotLink_t</type>
      <name>link</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>aebe9b720d4a9256a8e3b82386463010b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pHeaders</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>a8557f6e01a7b7c5b5db1a7aaa5d5dcde</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pHeadersEnd</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>af76674aa6e63fe3e3f7b3d8c7ba99a5c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pHeadersCur</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>aabdffa8fce401dd52cb9fd08af0c8eb1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pBody</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>af85454d820f29cd3308aed1832997263</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pBodyEnd</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>ad999211cb93c171025fa699fd75d2b4b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pBodyCur</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>ae324d5833c961fefb8507b2bda4493b2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>_httpParserInfo_t</type>
      <name>httpParserInfo</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>ac23bdac2ef22c992618656e779f0884f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>status</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>a87dc9050d6cd417cb6a52a834737d74e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsMethod_t</type>
      <name>method</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>a6d0a68b8c070cddb62f940f02609f62b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsResponseParserState_t</type>
      <name>parserState</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>afaa75922eb9710db7d392393f36dec86</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsResponseBufferState_t</type>
      <name>bufferProcessingState</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>a28cd856e993f35daf776225b693f9f03</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>pReadHeaderField</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>a3e257bf4ba6f44bf4c6017f48630ec8e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>readHeaderFieldLength</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>a66c15cc7d0b25291c1b60276fcd6ff02</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>pReadHeaderValue</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>adcbdcc92701ef52e48f57b0924d338a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>readHeaderValueLength</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>ad83ba85b35694fb9271d97223e4e386a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>foundHeaderField</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>a9e33b49eb82e50baf39c92cdd397a9aa</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct _httpsConnection *</type>
      <name>pHttpsConnection</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>aeeeb9976417ad9bc2382e7f257ef6ad6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>isAsync</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>aff9e2b9dc10b8dc8be88b8a1aef87fe8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pBodyInHeaderBuf</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>a6a3f99bd9a7abbc14e49c06a55a2ecce</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pBodyCurInHeaderBuf</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>ae844a5f334b3b1bb1b204617fc99a436</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsReturnCode_t</type>
      <name>bodyRxStatus</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>a598fdb39e234422b2056408f473f71a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>cancelled</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>ae4447c3d1f76996da40f64844080e22d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotSemaphore_t</type>
      <name>respFinishedSem</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>a262904139816c12929f2a0c2992cea28</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsReturnCode_t</type>
      <name>syncStatus</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>a8e09700b38c39ece348662d9645ba7ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>reqFinishedSending</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>a53cf0e5fa5e4bee240da6c46a3812710</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsClientCallbacks_t *</type>
      <name>pCallbacks</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>a153549a58876cc147dc7966b07243d3d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>pUserPrivData</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>a6e15f338ce32779f13e17f77c1336404</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>isNonPersistent</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>a442cb17480c41ab9577d4217c22d8bb0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotHttpsAsyncInfo_t</name>
    <filename>struct_iot_https_async_info__t.html</filename>
    <member kind="variable">
      <type>IotHttpsClientCallbacks_t</type>
      <name>callbacks</name>
      <anchorfile>struct_iot_https_async_info__t.html</anchorfile>
      <anchor>a35ce2cbf2c0d2e9b6fb01f33c665b388</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>pPrivData</name>
      <anchorfile>struct_iot_https_async_info__t.html</anchorfile>
      <anchor>a4cd942bc17e6dad03b77901bf7a709dc</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotHttpsClientCallbacks_t</name>
    <filename>struct_iot_https_client_callbacks__t.html</filename>
    <member kind="variable">
      <type>void(*</type>
      <name>appendHeaderCallback</name>
      <anchorfile>struct_iot_https_client_callbacks__t.html</anchorfile>
      <anchor>aa740a06066ee5058af299994b48b84c7</anchor>
      <arglist>)(void *pPrivData, IotHttpsRequestHandle_t reqHandle)</arglist>
    </member>
    <member kind="variable">
      <type>void(*</type>
      <name>writeCallback</name>
      <anchorfile>struct_iot_https_client_callbacks__t.html</anchorfile>
      <anchor>aa509416ac5f315ecaf89010ba95e4a1b</anchor>
      <arglist>)(void *pPrivData, IotHttpsRequestHandle_t reqHandle)</arglist>
    </member>
    <member kind="variable">
      <type>void(*</type>
      <name>readReadyCallback</name>
      <anchorfile>struct_iot_https_client_callbacks__t.html</anchorfile>
      <anchor>a3471688b6ca1b2582c84453ffd38a1b4</anchor>
      <arglist>)(void *pPrivData, IotHttpsResponseHandle_t respHandle, IotHttpsReturnCode_t rc, uint16_t status)</arglist>
    </member>
    <member kind="variable">
      <type>void(*</type>
      <name>responseCompleteCallback</name>
      <anchorfile>struct_iot_https_client_callbacks__t.html</anchorfile>
      <anchor>a9c89a3dc89a85ccd164124947368824a</anchor>
      <arglist>)(void *pPrivData, IotHttpsResponseHandle_t respHandle, IotHttpsReturnCode_t rc, uint16_t status)</arglist>
    </member>
    <member kind="variable">
      <type>void(*</type>
      <name>connectionClosedCallback</name>
      <anchorfile>struct_iot_https_client_callbacks__t.html</anchorfile>
      <anchor>a2927a57bdff467c1a4774e198aaac5b0</anchor>
      <arglist>)(void *pPrivData, IotHttpsConnectionHandle_t connHandle, IotHttpsReturnCode_t rc)</arglist>
    </member>
    <member kind="variable">
      <type>void(*</type>
      <name>errorCallback</name>
      <anchorfile>struct_iot_https_client_callbacks__t.html</anchorfile>
      <anchor>a7d07f73d45d33a1681afc1aa0872874c</anchor>
      <arglist>)(void *pPrivData, IotHttpsRequestHandle_t reqHandle, IotHttpsResponseHandle_t respHandle, IotHttpsReturnCode_t rc)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotHttpsConnectionInfo_t</name>
    <filename>struct_iot_https_connection_info__t.html</filename>
    <member kind="variable">
      <type>const char *</type>
      <name>pAddress</name>
      <anchorfile>struct_iot_https_connection_info__t.html</anchorfile>
      <anchor>aaaa1911897186d2405351191181b5f32</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>addressLen</name>
      <anchorfile>struct_iot_https_connection_info__t.html</anchorfile>
      <anchor>a6492288283b3678981763f9a99128d25</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>port</name>
      <anchorfile>struct_iot_https_connection_info__t.html</anchorfile>
      <anchor>a667022eae6af9b4d784613d5f13d9cce</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>flags</name>
      <anchorfile>struct_iot_https_connection_info__t.html</anchorfile>
      <anchor>a76b141ec84b35ee5ed79c20a4cfa24c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>timeout</name>
      <anchorfile>struct_iot_https_connection_info__t.html</anchorfile>
      <anchor>a8367f55ad73560039a7c3a8fc7d54bc5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pCaCert</name>
      <anchorfile>struct_iot_https_connection_info__t.html</anchorfile>
      <anchor>afcb8db31570bc084900eda12fb4a7ced</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>caCertLen</name>
      <anchorfile>struct_iot_https_connection_info__t.html</anchorfile>
      <anchor>a330d03b29b37c9742c061a374f8a1def</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pClientCert</name>
      <anchorfile>struct_iot_https_connection_info__t.html</anchorfile>
      <anchor>aa228e9c353db50544ed979a11138da1a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>clientCertLen</name>
      <anchorfile>struct_iot_https_connection_info__t.html</anchorfile>
      <anchor>a0f21228b09428ea842657ce93ec8791a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pPrivateKey</name>
      <anchorfile>struct_iot_https_connection_info__t.html</anchorfile>
      <anchor>a0c23234ee46a593299b6e6c3afe9f765</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>privateKeyLen</name>
      <anchorfile>struct_iot_https_connection_info__t.html</anchorfile>
      <anchor>a831ab9fad9ef773dcf061293ba09fe31</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>pAlpnProtocols</name>
      <anchorfile>struct_iot_https_connection_info__t.html</anchorfile>
      <anchor>a0d7ae9779e900ed79a0e8ee152c533e4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>alpnProtocolsLen</name>
      <anchorfile>struct_iot_https_connection_info__t.html</anchorfile>
      <anchor>a32d8bdbd15154cc5afe42285c8b6f034</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsUserBuffer_t</type>
      <name>userBuffer</name>
      <anchorfile>struct_iot_https_connection_info__t.html</anchorfile>
      <anchor>a3b998a5ba9ef3b5ab4c887ff8598633c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IOT_HTTPS_NETWORK_INTERFACE_TYPE</type>
      <name>pNetworkInterface</name>
      <anchorfile>struct_iot_https_connection_info__t.html</anchorfile>
      <anchor>a7925af087404997c804030de9a5c2230</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotHttpsRequestInfo_t</name>
    <filename>struct_iot_https_request_info__t.html</filename>
    <member kind="variable">
      <type>const char *</type>
      <name>pPath</name>
      <anchorfile>struct_iot_https_request_info__t.html</anchorfile>
      <anchor>ab03a896e957dfa4627633fc5902ad327</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>pathLen</name>
      <anchorfile>struct_iot_https_request_info__t.html</anchorfile>
      <anchor>af56fe3cc2b4d8eb34c585dca30997606</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsMethod_t</type>
      <name>method</name>
      <anchorfile>struct_iot_https_request_info__t.html</anchorfile>
      <anchor>ac80defadf129996a4847d8ef611911d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pHost</name>
      <anchorfile>struct_iot_https_request_info__t.html</anchorfile>
      <anchor>a151da299a963c55cea6089d810454621</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>hostLen</name>
      <anchorfile>struct_iot_https_request_info__t.html</anchorfile>
      <anchor>a35b91f903dbbd7ebe84e2430fd016545</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>isNonPersistent</name>
      <anchorfile>struct_iot_https_request_info__t.html</anchorfile>
      <anchor>ae4a91f589052c5c8410e20d4e9ad072f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsUserBuffer_t</type>
      <name>userBuffer</name>
      <anchorfile>struct_iot_https_request_info__t.html</anchorfile>
      <anchor>a6b920a6db879dd3332821bf8e3b62ea0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>isAsync</name>
      <anchorfile>struct_iot_https_request_info__t.html</anchorfile>
      <anchor>a284d9a4c65f45eea086550cd7af26cdf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsAsyncInfo_t *</type>
      <name>pAsyncInfo</name>
      <anchorfile>struct_iot_https_request_info__t.html</anchorfile>
      <anchor>ab913b433b6a6abdb31d367b44902248e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsSyncInfo_t *</type>
      <name>pSyncInfo</name>
      <anchorfile>struct_iot_https_request_info__t.html</anchorfile>
      <anchor>ae7db8c66e3aea83ac042c5350751ae45</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>union IotHttpsRequestInfo_t::@0</type>
      <name>u</name>
      <anchorfile>struct_iot_https_request_info__t.html</anchorfile>
      <anchor>a3c10eb9f9a57b5017d5b2b9dd4fb8ca4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotHttpsResponseInfo_t</name>
    <filename>struct_iot_https_response_info__t.html</filename>
    <member kind="variable">
      <type>IotHttpsUserBuffer_t</type>
      <name>userBuffer</name>
      <anchorfile>struct_iot_https_response_info__t.html</anchorfile>
      <anchor>a2effbd283351264b492cbb3b14704c86</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotHttpsSyncInfo_t *</type>
      <name>pSyncInfo</name>
      <anchorfile>struct_iot_https_response_info__t.html</anchorfile>
      <anchor>afa3decd344371f9257d1fde04c519a03</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotHttpsSyncInfo_t</name>
    <filename>struct_iot_https_sync_info__t.html</filename>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pBody</name>
      <anchorfile>struct_iot_https_sync_info__t.html</anchorfile>
      <anchor>add7230f049c1f80e1a87bee9e5750853</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bodyLen</name>
      <anchorfile>struct_iot_https_sync_info__t.html</anchorfile>
      <anchor>a4833f3a5cf2317a096e0879cf022a1e1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>IotHttpsUserBuffer_t</name>
    <filename>struct_iot_https_user_buffer__t.html</filename>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pBuffer</name>
      <anchorfile>struct_iot_https_user_buffer__t.html</anchorfile>
      <anchor>a3326d02c65169e70dccb31ac931da438</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>bufferLen</name>
      <anchorfile>struct_iot_https_user_buffer__t.html</anchorfile>
      <anchor>a1f434521e2be8ac2cbde821af243cac9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>https_client_datatypes_handles</name>
    <title>Handles</title>
    <filename>group__https__client__datatypes__handles.html</filename>
    <member kind="typedef">
      <type>struct _httpsConnection *</type>
      <name>IotHttpsConnectionHandle_t</name>
      <anchorfile>group__https__client__datatypes__handles.html</anchorfile>
      <anchor>ga6f57a7b8c51f49588625b0dd4dd6dedb</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct _httpsRequest *</type>
      <name>IotHttpsRequestHandle_t</name>
      <anchorfile>group__https__client__datatypes__handles.html</anchorfile>
      <anchor>ga23265e6eec7d9fa98da915d8e6a44b01</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct _httpsResponse *</type>
      <name>IotHttpsResponseHandle_t</name>
      <anchorfile>group__https__client__datatypes__handles.html</anchorfile>
      <anchor>gad59eec14931d695aad203d087cb961ea</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>https_client_datatypes_enums</name>
    <title>Enumerated types</title>
    <filename>group__https__client__datatypes__enums.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>IotHttpsReturnCode_t</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>gaeddc61f93281b31a5125ff2e57de9825</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_OK</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825af5531caaf2f532241af4f548840bfa9a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_INVALID_PARAMETER</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a9da4d637fd9c7ffb678f6c38dd2b0d8e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_INVALID_PAYLOAD</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a05056960780c215288e8e3b2198e5003</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_MESSAGE_TOO_LARGE</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825af843f35a50769e5a51803f216b8348a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_OVERFLOW</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825ae4027f63df204a6805e8eafa64f6a038</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_INSUFFICIENT_MEMORY</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a1335a4ceb7b35eb133a85c7937357041</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_QUEUE_FULL</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825ad1f2e262121f7b9ded89b794bba588d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_RETRY</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a6a2b50a5f0bc88311c4915b27c5edf13</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_NOT_FOUND</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a06e353388aa9f8c603d511c97336ea88</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_MESSAGE_FINISHED</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a8d869ebdf6648e4db9f6f7d57c6ded5e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_INTERNAL_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a2b7b810207afd90656ec30de44c9f8b2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_NETWORK_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a5eb66c716fa9269eec62f302acd0dea2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_CONNECTION_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a6c569739cc62b7ee7c7e07a03ea1da5a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_STREAM_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825af1dfa7a4c2b6bd87a15389757ae66512</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_AUTHENTICATION_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825aeac36da5f7728e7f0f923c50b8143528</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_TLS_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a393abc7d3309cf057eceb91f783c092a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_USER_CALLBACK_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a98d0b93f2c3947f5816d6335b57f5a31</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_TIMEOUT_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a5087d30c1e6649f597b36d80dc876dec</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_PROTOCOL_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a9378fd542bff27e650645811a0e98a06</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_SEND_ABORT</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825af2e021c46eb64cc4bf2eab18c1d99aab</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_RECEIVE_ABORT</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a3b695e40d399ef8500494770ae5cd92a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_ASYNC_SCHEDULING_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825aba7cdfc90f4d8e4e7e9d058c95fabd85</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_PARSING_ERROR</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a580063ea7d435812c1d256e0b5412774</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_FATAL</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825ad50da9a382731704fb01587a34dca548</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_BUSY</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a3d8759fdabaf6a5a9f524bb2b4d6d281</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_TRY_AGAIN</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825af845f74276676280240c4e25b93c660c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_DATA_EXIST</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a5e099e83668add49b23db7d88a05df02</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>IOT_HTTPS_NOT_SUPPORTED</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ggaeddc61f93281b31a5125ff2e57de9825a08165c17c8e574fa185e1b455d78bf39</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>IotHttpsMethod_t</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>gaaa195d049a76d14bd34b4ab5d610e843</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>IotHttpsResponseStatus</name>
      <anchorfile>group__https__client__datatypes__enums.html</anchorfile>
      <anchor>ga6cf83531f2a6d2f964621761825aac9d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>https_client_datatypes_paramstructs</name>
    <title>Parameter structures</title>
    <filename>group__https__client__datatypes__paramstructs.html</filename>
    <class kind="struct">IotHttpsClientCallbacks_t</class>
    <class kind="struct">IotHttpsUserBuffer_t</class>
    <class kind="struct">IotHttpsSyncInfo_t</class>
    <class kind="struct">IotHttpsAsyncInfo_t</class>
    <class kind="struct">IotHttpsConnectionInfo_t</class>
    <class kind="struct">IotHttpsRequestInfo_t</class>
    <class kind="struct">IotHttpsResponseInfo_t</class>
  </compound>
  <compound kind="page">
    <name>https_design</name>
    <title>Design</title>
    <filename>https_design.html</filename>
    <docanchor file="https_design.html" title="Synchronous Design">Synchronous_Design</docanchor>
    <docanchor file="https_design.html" title="Asynchronous Design">Asynchronous_Design</docanchor>
    <docanchor file="https_design.html" title="Asynchronous Callbacks Ordering">Asynchronous_Callback_Order</docanchor>
  </compound>
  <compound kind="page">
    <name>https_demo</name>
    <title>Demos</title>
    <filename>https_demo.html</filename>
  </compound>
  <compound kind="page">
    <name>https_download_demo_usage</name>
    <title>Download Demo Usage Instructions</title>
    <filename>https_download_demo_usage.html</filename>
  </compound>
  <compound kind="page">
    <name>https_upload_demo_usage</name>
    <title>Upload Demo Usage Instructions</title>
    <filename>https_upload_demo_usage.html</filename>
  </compound>
  <compound kind="page">
    <name>https_demo_config</name>
    <title>Demo Configuration</title>
    <filename>https_demo_config.html</filename>
    <docanchor file="https_demo_config.html">IOT_DEMO_HTTPS_PRESIGNED_GET_URL</docanchor>
    <docanchor file="https_demo_config.html">IOT_DEMO_HTTPS_PORT</docanchor>
    <docanchor file="https_demo_config.html">IOT_DEMO_HTTPS_TRUSTED_ROOT_CA</docanchor>
    <docanchor file="https_demo_config.html">IOT_DEMO_HTTPS_CONN_BUFFER_SIZE</docanchor>
    <docanchor file="https_demo_config.html">IOT_DEMO_HTTPS_REQ_USER_BUFFER_SIZE</docanchor>
    <docanchor file="https_demo_config.html">IOT_DEMO_HTTPS_RESP_USER_BUFFER_SIZE</docanchor>
    <docanchor file="https_demo_config.html">IOT_DEMO_HTTPS_RESP_BODY_BUFFER_SIZE</docanchor>
    <docanchor file="https_demo_config.html">IOT_DEMO_HTTPS_UPLOAD_DATA</docanchor>
    <docanchor file="https_demo_config.html">IOT_DEMO_HTTPS_UPLOAD_DATA_SIZE</docanchor>
    <docanchor file="https_demo_config.html">IOT_DEMO_HTTPS_CONNECTION_RETRY_WAIT_MS</docanchor>
    <docanchor file="https_demo_config.html">IOT_DEMO_HTTPS_CONNECTION_NUM_RETRY</docanchor>
    <docanchor file="https_demo_config.html">IOT_HTTPS_DEMO_SYNC_TIMEOUT_MS</docanchor>
    <docanchor file="https_demo_config.html">IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS</docanchor>
    <docanchor file="https_demo_config.html">IOT_HTTPS_DEMO_ASYNC_TIMEOUT_MS</docanchor>
  </compound>
  <compound kind="page">
    <name>https_tests</name>
    <title>Tests</title>
    <filename>https_tests.html</filename>
  </compound>
  <compound kind="page">
    <name>https_tests_config</name>
    <title>Test Configuration</title>
    <filename>https_tests_config.html</filename>
    <docanchor file="https_tests_config.html">IOT_TEST_HTTPS_SERVER_HOST_NAME</docanchor>
    <docanchor file="https_tests_config.html">IOT_TEST_HTTPS_PORT</docanchor>
    <docanchor file="https_tests_config.html">IOT_TEST_HTTPS_ALPN_PROTOCOLS</docanchor>
    <docanchor file="https_tests_config.html">IOT_TEST_HTTPS_ROOT_CA</docanchor>
    <docanchor file="https_tests_config.html">IOT_TEST_HTTPS_CLIENT_CERTIFICATE</docanchor>
    <docanchor file="https_tests_config.html">IOT_TEST_HTTPS_CLIENT_PRIVATE_KEY</docanchor>
    <docanchor file="https_tests_config.html">IOT_TEST_HTTPS_SYNC_TIMEOUT_MS</docanchor>
    <docanchor file="https_tests_config.html">IOT_TEST_HTTPS_ASYNC_TIMEOUT_MS</docanchor>
    <docanchor file="https_tests_config.html">IOT_TEST_HTTPS_INITIAL_CONNECTION_RETRY_DELAY</docanchor>
    <docanchor file="https_tests_config.html">IOT_TEST_HTTPS_CONNECTION_NUM_RETRIES</docanchor>
  </compound>
  <compound kind="page">
    <name>https_config</name>
    <title>Configuration</title>
    <filename>https_config.html</filename>
    <docanchor file="https_config.html">AWS_IOT_HTTPS_ENABLE_METRICS</docanchor>
    <docanchor file="https_config.html">IOT_HTTPS_USER_AGENT</docanchor>
    <docanchor file="https_config.html">IOT_LOG_LEVEL_HTTPS</docanchor>
    <docanchor file="https_config.html">IOT_HTTPS_MAX_FLUSH_BUFFER_SIZE</docanchor>
    <docanchor file="https_config.html">IOT_HTTPS_RESPONSE_WAIT_MS</docanchor>
    <docanchor file="https_config.html">IOT_HTTPS_MAX_HOST_NAME_LENGTH</docanchor>
    <docanchor file="https_config.html">IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH</docanchor>
  </compound>
  <compound kind="page">
    <name>https_client_functions</name>
    <title>Functions</title>
    <filename>https_client_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_init</name>
    <title>IotHttpsClient_Init</title>
    <filename>https_client_function_init.html</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_cleanup</name>
    <title>IotHttpsClient_Cleanup</title>
    <filename>https_client_function_cleanup.html</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_disconnect</name>
    <title>IotHttpsClient_Disconnect</title>
    <filename>https_client_function_disconnect.html</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_connect</name>
    <title>IotHttpsClient_Connect</title>
    <filename>https_client_function_connect.html</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_initializerequest</name>
    <title>IotHttpsClient_InitializeRequest</title>
    <filename>https_client_function_initializerequest.html</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_addheader</name>
    <title>IotHttpsClient_AddHeader</title>
    <filename>https_client_function_addheader.html</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_writerequestbody</name>
    <title>IotHttpsClient_WriteRequestBody</title>
    <filename>https_client_function_writerequestbody.html</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_sendsync</name>
    <title>IotHttpsClient_SendSync</title>
    <filename>https_client_function_sendsync.html</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_sendasync</name>
    <title>IotHttpsClient_SendAsync</title>
    <filename>https_client_function_sendasync.html</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_cancelrequestasync</name>
    <title>IotHttpsClient_CancelRequestAsync</title>
    <filename>https_client_function_cancelrequestasync.html</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_cancelresponseasync</name>
    <title>IotHttpsClient_CancelResponseAsync</title>
    <filename>https_client_function_cancelresponseasync.html</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_readresponsestatus</name>
    <title>IotHttpsClient_ReadResponseStatus</title>
    <filename>https_client_function_readresponsestatus.html</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_readcontentlength</name>
    <title>IotHttpsClient_ReadContentLength</title>
    <filename>https_client_function_readcontentlength.html</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_readheader</name>
    <title>IotHttpsClient_ReadHeader</title>
    <filename>https_client_function_readheader.html</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_readresponsebody</name>
    <title>IotHttpsClient_ReadResponseBody</title>
    <filename>https_client_function_readresponsebody.html</filename>
  </compound>
  <compound kind="page">
    <name>https_client_constants</name>
    <title>Constants</title>
    <filename>https_client_constants.html</filename>
    <docanchor file="https_client_constants.html" title="HTTPS Client Minimum User Buffer Sizes">https_minimum_user_buffer_sizes</docanchor>
    <docanchor file="https_client_constants.html" title="HTTPS Client Connection Flags">https_connection_flags</docanchor>
    <docanchor file="https_client_constants.html" title="HTTP Initializers">https_initializers</docanchor>
    <docanchor file="https_client_constants.html" title="HTTPS Client Connection Flags">http_constants_connection_flags</docanchor>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title></title>
    <filename>index.html</filename>
    <docanchor file="index.html">https</docanchor>
    <docanchor file="index.html" title="Dependencies">https_dependencies</docanchor>
  </compound>
</tagfile>
