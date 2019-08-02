<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile>
  <compound kind="file">
    <name>iot_https_client.h</name>
    <path>C:/b/amazon-freertos/libraries/c_sdk/standard/https/include/</path>
    <filename>iot__https__client_8h</filename>
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
      <name>IotHttpsClient_Deinit</name>
      <anchorfile>iot__https__client_8h.html</anchorfile>
      <anchor>ac2e6cbf1f1b52ed76304a6772429b1c7</anchor>
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
    <path>C:/b/amazon-freertos/libraries/c_sdk/standard/https/include/types/</path>
    <filename>iot__https__types_8h</filename>
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
    <name>iot_https_utils.h</name>
    <path>C:/b/amazon-freertos/libraries/c_sdk/standard/https/include/</path>
    <filename>iot__https__utils_8h</filename>
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
      <anchor>a5d06266eb9cf1f305684bc84af1dfcd9</anchor>
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
      <type>bool</type>
      <name>isDisconnecting</name>
      <anchorfile>struct__https_connection__t.html</anchorfile>
      <anchor>aeef81d2fe692defee9450ca3d7f44ccd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotMutex_t</type>
      <name>reqQMutex</name>
      <anchorfile>struct__https_connection__t.html</anchorfile>
      <anchor>a7d92da57941ca099211ba6e5f6ca34ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IotMutex_t</type>
      <name>respQMutex</name>
      <anchorfile>struct__https_connection__t.html</anchorfile>
      <anchor>a649ff81f74a5099424e055e5ec45916c</anchor>
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
      <type>bool</type>
      <name>reqFinishedSending</name>
      <anchorfile>struct__https_request__t.html</anchorfile>
      <anchor>a0c24729a5140db4a20609d7b675f5000</anchor>
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
      <type>struct _httpsRequest *</type>
      <name>pHttpsRequest</name>
      <anchorfile>struct__https_response__t.html</anchorfile>
      <anchor>a3a8282903a3312faa0529b04095b5c69</anchor>
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
      <anchor>a91ca3e5fa61b639b7ba98bd9024ec68a</anchor>
      <arglist>)(void *pPrivData, IotHttpsRequestHandle_t reqHandle)</arglist>
    </member>
    <member kind="variable">
      <type>void(*</type>
      <name>writeCallback</name>
      <anchorfile>struct_iot_https_client_callbacks__t.html</anchorfile>
      <anchor>ae0c6fa6e49309744532beb4cf728e9fb</anchor>
      <arglist>)(void *pPrivData, IotHttpsRequestHandle_t reqHandle)</arglist>
    </member>
    <member kind="variable">
      <type>void(*</type>
      <name>readReadyCallback</name>
      <anchorfile>struct_iot_https_client_callbacks__t.html</anchorfile>
      <anchor>a5edf91d794af4a1d5ed127cdc9f2e9c7</anchor>
      <arglist>)(void *pPrivData, IotHttpsResponseHandle_t respHandle, IotHttpsReturnCode_t rc, uint16_t status)</arglist>
    </member>
    <member kind="variable">
      <type>void(*</type>
      <name>responseCompleteCallback</name>
      <anchorfile>struct_iot_https_client_callbacks__t.html</anchorfile>
      <anchor>a1a0224414b40c432c72456475e699c7b</anchor>
      <arglist>)(void *pPrivData, IotHttpsResponseHandle_t respHandle, IotHttpsReturnCode_t rc, uint16_t status)</arglist>
    </member>
    <member kind="variable">
      <type>void(*</type>
      <name>connectionClosedCallback</name>
      <anchorfile>struct_iot_https_client_callbacks__t.html</anchorfile>
      <anchor>a12d44b3f1851146d1ad3908564c4856c</anchor>
      <arglist>)(void *pPrivData, IotHttpsConnectionHandle_t connHandle, IotHttpsReturnCode_t rc)</arglist>
    </member>
    <member kind="variable">
      <type>void(*</type>
      <name>errorCallback</name>
      <anchorfile>struct_iot_https_client_callbacks__t.html</anchorfile>
      <anchor>a54376993b3dd827ffec0fd8519657a99</anchor>
      <arglist>)(void *pPrivData, IotHttpsRequestHandle_t reqHandle, IotHttpsReturnCode_t rc)</arglist>
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
      <anchor>aeaec785562659580c5433e0dedff3b46</anchor>
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
    <filename>https_design</filename>
  </compound>
  <compound kind="page">
    <name>https_demo</name>
    <title>Demos</title>
    <filename>https_demo</filename>
  </compound>
  <compound kind="page">
    <name>https_demo_config</name>
    <title>Demo Configuration</title>
    <filename>https_demo_config</filename>
    <docanchor file="https_demo_config">IOT_DEMO_HTTPS_PRESIGNED_GET_URL</docanchor>
    <docanchor file="https_demo_config">IOT_DEMO_HTTPS_TRUSTED_ROOT_CA</docanchor>
    <docanchor file="https_demo_config">IOT_DEMO_HTTPS_CONN_BUFFER_SIZE</docanchor>
    <docanchor file="https_demo_config">IOT_DEMO_HTTPS_REQ_USER_BUFFER_SIZE</docanchor>
    <docanchor file="https_demo_config">IOT_DEMO_HTTPS_RESP_USER_BUFFER_SIZE</docanchor>
    <docanchor file="https_demo_config">IOT_DEMO_HTTPS_RESP_BODY_BUFFER_SIZE</docanchor>
    <docanchor file="https_demo_config">IOT_HTTPS_DEMO_MAX_ASYNC_REQUESTS</docanchor>
    <docanchor file="https_demo_config">IOT_HTTPS_DEMO_ASYNC_TIMEOUT_MS</docanchor>
  </compound>
  <compound kind="page">
    <name>https_tests</name>
    <title>Tests</title>
    <filename>https_tests</filename>
  </compound>
  <compound kind="page">
    <name>https_config</name>
    <title>Configuration</title>
    <filename>https_config</filename>
    <docanchor file="https_config">AWS_IOT_HTTPS_ENABLE_METRICS</docanchor>
    <docanchor file="https_config">IOT_HTTPS_USER_AGENT</docanchor>
    <docanchor file="https_config">IOT_LOG_LEVEL_HTTPS</docanchor>
    <docanchor file="https_config">IOT_HTTPS_MAX_FLUSH_BUFFER_SIZE</docanchor>
    <docanchor file="https_config">IOT_HTTPS_RESPONSE_WAIT_MS</docanchor>
    <docanchor file="https_config">IOT_HTTPS_MAX_HOST_NAME_LENGTH</docanchor>
    <docanchor file="https_config">IOT_HTTPS_MAX_ALPN_PROTOCOLS_LENGTH</docanchor>
  </compound>
  <compound kind="page">
    <name>https_client_functions</name>
    <title>Functions</title>
    <filename>https_client_functions</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_init</name>
    <title>IotHttpsClient_Init</title>
    <filename>https_client_function_init</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_deinit</name>
    <title>IotHttpsClient_Deinit</title>
    <filename>https_client_function_deinit</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_disconnect</name>
    <title>IotHttpsClient_Disconnect</title>
    <filename>https_client_function_disconnect</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_connect</name>
    <title>IotHttpsClient_Connect</title>
    <filename>https_client_function_connect</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_initializerequest</name>
    <title>IotHttpsClient_InitializeRequest</title>
    <filename>https_client_function_initializerequest</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_addheader</name>
    <title>IotHttpsClient_AddHeader</title>
    <filename>https_client_function_addheader</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_writerequestbody</name>
    <title>IotHttpsClient_WriteRequestBody</title>
    <filename>https_client_function_writerequestbody</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_sendsync</name>
    <title>IotHttpsClient_SendSync</title>
    <filename>https_client_function_sendsync</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_sendasync</name>
    <title>IotHttpsClient_SendAsync</title>
    <filename>https_client_function_sendasync</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_cancelrequestasync</name>
    <title>IotHttpsClient_CancelRequestAsync</title>
    <filename>https_client_function_cancelrequestasync</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_cancelresponseasync</name>
    <title>IotHttpsClient_CancelResponseAsync</title>
    <filename>https_client_function_cancelresponseasync</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_readresponsestatus</name>
    <title>IotHttpsClient_ReadResponseStatus</title>
    <filename>https_client_function_readresponsestatus</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_readcontentlength</name>
    <title>IotHttpsClient_ReadContentLength</title>
    <filename>https_client_function_readcontentlength</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_readheader</name>
    <title>IotHttpsClient_ReadHeader</title>
    <filename>https_client_function_readheader</filename>
  </compound>
  <compound kind="page">
    <name>https_client_function_readresponsebody</name>
    <title>IotHttpsClient_ReadResponseBody</title>
    <filename>https_client_function_readresponsebody</filename>
  </compound>
  <compound kind="page">
    <name>https_client_constants</name>
    <title>Constants</title>
    <filename>https_client_constants</filename>
    <docanchor file="https_client_constants" title="HTTPS Client Minimum User Buffer Sizes">https_minimum_user_buffer_sizes</docanchor>
    <docanchor file="https_client_constants" title="HTTPS Client Connection Flags">https_connection_flags</docanchor>
    <docanchor file="https_client_constants" title="HTTP Initializers">https_initializers</docanchor>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title></title>
    <filename>index</filename>
    <docanchor file="index">https</docanchor>
    <docanchor file="index" title="Dependencies">https_dependencies</docanchor>
  </compound>
</tagfile>
