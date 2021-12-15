<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.8.20" doxygen_gitid="f246dd2f1c58eea39ea3f50c108019e4d4137bd5">
  <compound kind="file">
    <name>core_http_client.c</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/coreHTTP/source/</path>
    <filename>core__http__client_8c.html</filename>
    <includes id="core__http__client_8h" name="core_http_client.h" local="yes" imported="no">core_http_client.h</includes>
    <includes id="core__http__client__private_8h" name="core_http_client_private.h" local="yes" imported="no">core_http_client_private.h</includes>
    <member kind="function" static="yes">
      <type>static uint32_t</type>
      <name>getZeroTimestampMs</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a11d42680d4c46335520d4cf2c7b4ab2d</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static HTTPStatus_t</type>
      <name>sendHttpData</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>ade50d682517f4aba726e50bd8f71aa56</anchor>
      <arglist>(const TransportInterface_t *pTransport, HTTPClient_GetCurrentTimeFunc_t getTimestampMs, const uint8_t *pData, size_t dataLen)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static HTTPStatus_t</type>
      <name>sendHttpHeaders</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>acf96c993a4a2dc6440d8a0be4aab901f</anchor>
      <arglist>(const TransportInterface_t *pTransport, HTTPClient_GetCurrentTimeFunc_t getTimestampMs, HTTPRequestHeaders_t *pRequestHeaders, size_t reqBodyLen, uint32_t sendFlags)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static HTTPStatus_t</type>
      <name>addContentLengthHeader</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a07cf85ef7e5a4b93a96671fd4d069849</anchor>
      <arglist>(HTTPRequestHeaders_t *pRequestHeaders, size_t contentLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static HTTPStatus_t</type>
      <name>sendHttpBody</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a22f94989802d0f8bd4d08eb79c913d82</anchor>
      <arglist>(const TransportInterface_t *pTransport, HTTPClient_GetCurrentTimeFunc_t getTimestampMs, const uint8_t *pRequestBodyBuf, size_t reqBodyBufLen)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static char *</type>
      <name>httpHeaderStrncpy</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a1794ae3e1305deec6b66de0031f06f96</anchor>
      <arglist>(char *pDest, const char *pSrc, size_t len, uint8_t isField)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static HTTPStatus_t</type>
      <name>addHeader</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a9e89adcbc07c646d53c7c803fa2d9fa9</anchor>
      <arglist>(HTTPRequestHeaders_t *pRequestHeaders, const char *pField, size_t fieldLen, const char *pValue, size_t valueLen)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static HTTPStatus_t</type>
      <name>addRangeHeader</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>adf23c3eea0b1edf671694c8869ca3b17</anchor>
      <arglist>(HTTPRequestHeaders_t *pRequestHeaders, int32_t rangeStartOrlastNbytes, int32_t rangeEnd)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static HTTPStatus_t</type>
      <name>getFinalResponseStatus</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a7cc749a54dc2f77951341f2aa3ba8988</anchor>
      <arglist>(HTTPParsingState_t parsingState, size_t totalReceived, size_t responseBufferLen)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static HTTPStatus_t</type>
      <name>receiveAndParseHttpResponse</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a23717e1f6fb18170e5c5b087b4d32ca7</anchor>
      <arglist>(const TransportInterface_t *pTransport, HTTPResponse_t *pResponse, const HTTPRequestHeaders_t *pRequestHeaders)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static HTTPStatus_t</type>
      <name>sendHttpRequest</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a4eadb2537a7ed36c7561cb9432d89ff9</anchor>
      <arglist>(const TransportInterface_t *pTransport, HTTPClient_GetCurrentTimeFunc_t getTimestampMs, HTTPRequestHeaders_t *pRequestHeaders, const uint8_t *pRequestBodyBuf, size_t reqBodyBufLen, uint32_t sendFlags)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static uint8_t</type>
      <name>convertInt32ToAscii</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>ae2cb3e15c03ae979f7966f2fcec53bc6</anchor>
      <arglist>(int32_t value, char *pBuffer, size_t bufferLength)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static HTTPStatus_t</type>
      <name>writeRequestLine</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a31a6c7eba272a1d070b03d8fdba62391</anchor>
      <arglist>(HTTPRequestHeaders_t *pRequestHeaders, const char *pMethod, size_t methodLen, const char *pPath, size_t pathLen)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static HTTPStatus_t</type>
      <name>findHeaderInResponse</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a9318ad3c65f775d5fb3467de19320a58</anchor>
      <arglist>(const uint8_t *pBuffer, size_t bufferLen, const char *pField, size_t fieldLen, const char **pValueLoc, size_t *pValueLen)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>findHeaderFieldParserCallback</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a73f3186eba2edadf19cf264566f3bb18</anchor>
      <arglist>(http_parser *pHttpParser, const char *pFieldLoc, size_t fieldLen)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>findHeaderValueParserCallback</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a9f8da60e8a7694171f1962f106a8b318</anchor>
      <arglist>(http_parser *pHttpParser, const char *pValueLoc, size_t valueLen)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>findHeaderOnHeaderCompleteCallback</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a2faae933523718600b6ae64f6ae022bd</anchor>
      <arglist>(http_parser *pHttpParser)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>initializeParsingContextForFirstResponse</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a859ffd50bb4dbc6728bf0acb8d4da962</anchor>
      <arglist>(HTTPParsingContext_t *pParsingContext, const HTTPRequestHeaders_t *pRequestHeaders)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static HTTPStatus_t</type>
      <name>parseHttpResponse</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a5e8340bff0f3311f5cad2009039f1dcf</anchor>
      <arglist>(HTTPParsingContext_t *pParsingContext, HTTPResponse_t *pResponse, size_t parseLen)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>httpParserOnMessageBeginCallback</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a9967a0058c6a281ba712e53b006c794b</anchor>
      <arglist>(http_parser *pHttpParser)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>httpParserOnStatusCallback</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a65e3484868f697a7ddbaea66f60208c4</anchor>
      <arglist>(http_parser *pHttpParser, const char *pLoc, size_t length)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>httpParserOnHeaderFieldCallback</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>accc77efd739f6f83734a735e2a994aa0</anchor>
      <arglist>(http_parser *pHttpParser, const char *pLoc, size_t length)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>httpParserOnHeaderValueCallback</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a248fecda36ee7b7ff5c958aa47014e8d</anchor>
      <arglist>(http_parser *pHttpParser, const char *pLoc, size_t length)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>httpParserOnHeadersCompleteCallback</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a893094c266a87942923409b4f8a918bb</anchor>
      <arglist>(http_parser *pHttpParser)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>httpParserOnBodyCallback</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a05d8d29ff2474a611bedc010c95220e8</anchor>
      <arglist>(http_parser *pHttpParser, const char *pLoc, size_t length)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static int</type>
      <name>httpParserOnMessageCompleteCallback</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a53c5a2693d5006b4becd10e039ff4795</anchor>
      <arglist>(http_parser *pHttpParser)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>processCompleteHeader</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a5b52dd3324e97b23af678200d7aa0489</anchor>
      <arglist>(HTTPParsingContext_t *pParsingContext)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static HTTPStatus_t</type>
      <name>processHttpParserError</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a038d58442b6626dc81b596e538d49e44</anchor>
      <arglist>(const http_parser *pHttpParser)</arglist>
    </member>
    <member kind="function">
      <type>HTTPStatus_t</type>
      <name>HTTPClient_InitializeRequestHeaders</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>ab523c4d4e3b756928d463bd864361a9c</anchor>
      <arglist>(HTTPRequestHeaders_t *pRequestHeaders, const HTTPRequestInfo_t *pRequestInfo)</arglist>
    </member>
    <member kind="function">
      <type>HTTPStatus_t</type>
      <name>HTTPClient_AddHeader</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>a451c30912fcdf48c71f9144788250916</anchor>
      <arglist>(HTTPRequestHeaders_t *pRequestHeaders, const char *pField, size_t fieldLen, const char *pValue, size_t valueLen)</arglist>
    </member>
    <member kind="function">
      <type>HTTPStatus_t</type>
      <name>HTTPClient_AddRangeHeader</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>af91c4a57452556db2be926743aa02b81</anchor>
      <arglist>(HTTPRequestHeaders_t *pRequestHeaders, int32_t rangeStartOrlastNbytes, int32_t rangeEnd)</arglist>
    </member>
    <member kind="function">
      <type>HTTPStatus_t</type>
      <name>HTTPClient_Send</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>aa33b3334128496487d820f0da7ae5601</anchor>
      <arglist>(const TransportInterface_t *pTransport, HTTPRequestHeaders_t *pRequestHeaders, const uint8_t *pRequestBodyBuf, size_t reqBodyBufLen, HTTPResponse_t *pResponse, uint32_t sendFlags)</arglist>
    </member>
    <member kind="function">
      <type>HTTPStatus_t</type>
      <name>HTTPClient_ReadHeader</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>ac621a0756f290f1437e2c72a24e5b0f9</anchor>
      <arglist>(const HTTPResponse_t *pResponse, const char *pField, size_t fieldLen, const char **pValueLoc, size_t *pValueLen)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>HTTPClient_strerror</name>
      <anchorfile>core__http__client_8c.html</anchorfile>
      <anchor>af9a8f411ff5783dd2f0ca2ad43f37e18</anchor>
      <arglist>(HTTPStatus_t status)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>core_http_client.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/coreHTTP/source/include/</path>
    <filename>core__http__client_8h.html</filename>
    <includes id="core__http__config__defaults_8h" name="core_http_config_defaults.h" local="yes" imported="no">core_http_config_defaults.h</includes>
    <includes id="transport__interface_8h" name="transport_interface.h" local="yes" imported="no">transport_interface.h</includes>
    <class kind="struct">HTTPRequestHeaders_t</class>
    <class kind="struct">HTTPRequestInfo_t</class>
    <class kind="struct">HTTPClient_ResponseHeaderParsingCallback_t</class>
    <class kind="struct">HTTPResponse_t</class>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_METHOD_GET</name>
      <anchorfile>group__http__constants.html</anchorfile>
      <anchor>ga91faefe43f8d3577719ba174c8a5ebcd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_METHOD_PUT</name>
      <anchorfile>group__http__constants.html</anchorfile>
      <anchor>gac1999442fe79f9c13dc51086c61e8774</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_METHOD_POST</name>
      <anchorfile>group__http__constants.html</anchorfile>
      <anchor>ga6c5872fb505bc0d37a9de2e2192dc7c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_METHOD_HEAD</name>
      <anchorfile>group__http__constants.html</anchorfile>
      <anchor>ga7fe13cca8f55d3159b4bbda7c656804d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_MAX_CONTENT_LENGTH_HEADER_LENGTH</name>
      <anchorfile>group__http__constants.html</anchorfile>
      <anchor>ga7a6adfe8f1a63441e5838a004040588f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_SEND_DISABLE_CONTENT_LENGTH_FLAG</name>
      <anchorfile>group__http__send__flags.html</anchorfile>
      <anchor>ga66db30f9be610b9f1c851d9cd9cb3cff</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_REQUEST_KEEP_ALIVE_FLAG</name>
      <anchorfile>group__http__request__flags.html</anchorfile>
      <anchor>gadec54e27d2d24006b4c22a7f945282de</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_RESPONSE_CONNECTION_CLOSE_FLAG</name>
      <anchorfile>group__http__response__flags.html</anchorfile>
      <anchor>ga362e461ad360ffb2ae6b986c688993c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_RESPONSE_CONNECTION_KEEP_ALIVE_FLAG</name>
      <anchorfile>group__http__response__flags.html</anchorfile>
      <anchor>ga259404ed98011458fb1ad2e90fba1752</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_RANGE_REQUEST_END_OF_FILE</name>
      <anchorfile>group__http__constants.html</anchorfile>
      <anchor>ga4719dc6fd6e1b92739e1c3a88efa7046</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint32_t(*</type>
      <name>HTTPClient_GetCurrentTimeFunc_t</name>
      <anchorfile>group__http__callback__types.html</anchorfile>
      <anchor>ga32d6894c1c9d62568136203b57c6d796</anchor>
      <arglist>)(void)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>HTTPStatus_t</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>ga39fad9050063a355ffd95e95381e743f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPSuccess</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fa68f5addd5bf4367e177957a4c43dd4bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPInvalidParameter</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743faed61c0d831bc64e24db373b803837138</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPNetworkError</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fa59fa581a112b1994db1c45d8a7f84160</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPPartialResponse</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fa2b345207e843dc4e3d2ef9ea34c7f024</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPNoResponse</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fa8aa7c86301f91fd4b3c0b698ab8c177a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPInsufficientMemory</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fa33aae6d01bae2736a4a977ef04399ad4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPSecurityAlertResponseHeadersSizeLimitExceeded</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fac9de51dc48124bbbc331a9161afe3862</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPSecurityAlertExtraneousResponseData</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743faf6441d45e373b7686e45c2d9a0d12dd3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPSecurityAlertInvalidChunkHeader</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743faef569c934115aa105cc2cff1f72ef78b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPSecurityAlertInvalidProtocolVersion</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fab6ed7f7e24e250896a7b35825006d460</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPSecurityAlertInvalidStatusCode</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743faf53f0e027fd6d66ecd796bce19711562</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPSecurityAlertInvalidCharacter</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fa075511c32463db38713944d4e43cc01f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPSecurityAlertInvalidContentLength</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fac1b1794cf29fd99be06e0b0ed5f9213d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPParserInternalError</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fa37855790aef77f76074771d000b3e76b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPHeaderNotFound</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fad2fc47ae0e9bbf4a459f38a36020824a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPInvalidResponse</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fa8674a7c17d6878db3a6cf7e2e6e6a398</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>HTTPStatus_t</type>
      <name>HTTPClient_InitializeRequestHeaders</name>
      <anchorfile>core__http__client_8h.html</anchorfile>
      <anchor>ab523c4d4e3b756928d463bd864361a9c</anchor>
      <arglist>(HTTPRequestHeaders_t *pRequestHeaders, const HTTPRequestInfo_t *pRequestInfo)</arglist>
    </member>
    <member kind="function">
      <type>HTTPStatus_t</type>
      <name>HTTPClient_AddHeader</name>
      <anchorfile>core__http__client_8h.html</anchorfile>
      <anchor>a451c30912fcdf48c71f9144788250916</anchor>
      <arglist>(HTTPRequestHeaders_t *pRequestHeaders, const char *pField, size_t fieldLen, const char *pValue, size_t valueLen)</arglist>
    </member>
    <member kind="function">
      <type>HTTPStatus_t</type>
      <name>HTTPClient_AddRangeHeader</name>
      <anchorfile>core__http__client_8h.html</anchorfile>
      <anchor>af91c4a57452556db2be926743aa02b81</anchor>
      <arglist>(HTTPRequestHeaders_t *pRequestHeaders, int32_t rangeStartOrlastNbytes, int32_t rangeEnd)</arglist>
    </member>
    <member kind="function">
      <type>HTTPStatus_t</type>
      <name>HTTPClient_Send</name>
      <anchorfile>core__http__client_8h.html</anchorfile>
      <anchor>aa33b3334128496487d820f0da7ae5601</anchor>
      <arglist>(const TransportInterface_t *pTransport, HTTPRequestHeaders_t *pRequestHeaders, const uint8_t *pRequestBodyBuf, size_t reqBodyBufLen, HTTPResponse_t *pResponse, uint32_t sendFlags)</arglist>
    </member>
    <member kind="function">
      <type>HTTPStatus_t</type>
      <name>HTTPClient_ReadHeader</name>
      <anchorfile>core__http__client_8h.html</anchorfile>
      <anchor>ac621a0756f290f1437e2c72a24e5b0f9</anchor>
      <arglist>(const HTTPResponse_t *pResponse, const char *pField, size_t fieldLen, const char **pValueLoc, size_t *pValueLen)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>HTTPClient_strerror</name>
      <anchorfile>core__http__client_8h.html</anchorfile>
      <anchor>af9a8f411ff5783dd2f0ca2ad43f37e18</anchor>
      <arglist>(HTTPStatus_t status)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>core_http_client_private.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/coreHTTP/source/include/</path>
    <filename>core__http__client__private_8h.html</filename>
    <class kind="struct">findHeaderContext_t</class>
    <class kind="struct">HTTPParsingContext_t</class>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_PROTOCOL_VERSION</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a80eefd366038dc5a3bae6cd8b27fe1ae</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_PROTOCOL_VERSION_LEN</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>aa7961d9b9288123b1cc96d153299ad31</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_EMPTY_PATH</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>af2bb2317c7f90b7db70b8f0150bb28fe</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_EMPTY_PATH_LEN</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a4842e1e66dec35cb8d5ad140db33be9e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_HEADER_LINE_SEPARATOR</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>ae0358d41cf70311874e8125ee571b5ac</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_HEADER_LINE_SEPARATOR_LEN</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a35fd3ea020f0b658b3eaf2e246be45b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_HEADER_END_INDICATOR</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a0b8c0284cb90738dcd3d441f0f228dd9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_HEADER_END_INDICATOR_LEN</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a550d668792aa8f8e68d202f33dcd9a53</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_HEADER_FIELD_SEPARATOR</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>af43a649370df6b9b161ec1c2b480219a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_HEADER_FIELD_SEPARATOR_LEN</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a92404c36d8ab6f23b0e4ca0540397bc7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SPACE_CHARACTER</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a5819263c85fa1e3662680e9595aa1a8b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SPACE_CHARACTER_LEN</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>aca62be07b50b505b6db3950aafb962c1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DASH_CHARACTER</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a7d564bb943cd1ab2bd4544931dc0670c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DASH_CHARACTER_LEN</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>ac128dc6511d1d1396cb648c8361979af</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>CARRIAGE_RETURN_CHARACTER</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a6f61e104cb04eb2760fa761cc08a7703</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LINEFEED_CHARACTER</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a85816ff6da596bbf255514cab8a53953</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>COLON_CHARACTER</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a059a7633515b43b905e459772e2e216b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_HEADER_STRNCPY_IS_VALUE</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a1306d0c002cf7d4cc9a2bcff96068f44</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_HEADER_STRNCPY_IS_FIELD</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a1b8b03fbe50ac6a383a055338dba3803</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_USER_AGENT_FIELD</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>ad211616c9587553ec1bae2f8c9b080a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_USER_AGENT_FIELD_LEN</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a6842bfa4af01fe2d3e5d45a8c9e90429</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_HOST_FIELD</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a5b810c11cf395733a3074912853e6802</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_HOST_FIELD_LEN</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a24eb8b9d1349f291965ccf65a8c3bfd5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_USER_AGENT_VALUE_LEN</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a3fbb359bb37d4b892e659750ec74709c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_CONNECTION_FIELD</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a00c9f765a62413668b8b3fd1fab4761d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_CONNECTION_FIELD_LEN</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>aa51b7e1d8d2d5d6fc89e553f7e37c4e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_CONTENT_LENGTH_FIELD</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a4df7bee38c3d7f17eda96873e0e2e5e7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_CONTENT_LENGTH_FIELD_LEN</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a2c757946bca1472c0540b7d08f9eec3e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_CONNECTION_KEEP_ALIVE_VALUE</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>abcb1d6f41e8bb5a870b217e5b331938e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_CONNECTION_KEEP_ALIVE_VALUE_LEN</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a35dc780b50f908b482a8fc29ece41f22</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_RANGE_REQUEST_HEADER_FIELD</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>af24e842ea2bcdf146721c7bfd5107c73</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_RANGE_REQUEST_HEADER_FIELD_LEN</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>ad09b0f824a360f9410980b010de43d08</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_RANGE_REQUEST_HEADER_VALUE_PREFIX</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a5ba5e33803ae5734b5093479041a0875</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_RANGE_REQUEST_HEADER_VALUE_PREFIX_LEN</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a7f2644dba5716326d6bb991c5ffa2a72</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>MAX_INT32_NO_OF_DECIMAL_DIGITS</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a4f52500045ff77d717544fdb6811d556</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_MAX_RANGE_REQUEST_VALUE_LEN</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a713caa09b72a5f9343d91ec678a7d0e8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_PARSER_STOP_PARSING</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a4235a9eef858e49d3be23e92b4398aee</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_PARSER_CONTINUE_PARSING</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a76d44c456271458717b05c30a54ac6d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_MINIMUM_REQUEST_LINE_LENGTH</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>aa33a015440859bb10f946a2837972072</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>HTTPParsingState_t</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a9b42138b048d030fe583c0dd0b487b4c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTP_PARSING_NONE</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a9b42138b048d030fe583c0dd0b487b4caf6b11469258d9bca75e36a7a99287217</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTP_PARSING_INCOMPLETE</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a9b42138b048d030fe583c0dd0b487b4ca18581a8425b3cd0fe28fdcaceb357e39</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTP_PARSING_COMPLETE</name>
      <anchorfile>core__http__client__private_8h.html</anchorfile>
      <anchor>a9b42138b048d030fe583c0dd0b487b4caa01f579f46229b464b106d044cd50f20</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>core_http_config_defaults.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/coreHTTP/source/include/</path>
    <filename>core__http__config__defaults_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_MAX_RESPONSE_HEADERS_SIZE_BYTES</name>
      <anchorfile>core__http__config__defaults_8h.html</anchorfile>
      <anchor>ae75b97ea0b2c54acc59a148274a882e8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_USER_AGENT_VALUE</name>
      <anchorfile>core__http__config__defaults_8h.html</anchorfile>
      <anchor>ad588638704f5bfbc30bc56f971dba343</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_RECV_RETRY_TIMEOUT_MS</name>
      <anchorfile>core__http__config__defaults_8h.html</anchorfile>
      <anchor>a909c2c6f89016b92bd848527f671e8f2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_SEND_RETRY_TIMEOUT_MS</name>
      <anchorfile>core__http__config__defaults_8h.html</anchorfile>
      <anchor>ae498a6815f6925b8283ce0babc101946</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogError</name>
      <anchorfile>core__http__config__defaults_8h.html</anchorfile>
      <anchor>a8d9dbaaa88129137a4c68ba0456a18b1</anchor>
      <arglist>(message)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogWarn</name>
      <anchorfile>core__http__config__defaults_8h.html</anchorfile>
      <anchor>a7da92048aaf0cbfcacde9539c98a0e05</anchor>
      <arglist>(message)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogInfo</name>
      <anchorfile>core__http__config__defaults_8h.html</anchorfile>
      <anchor>a00810b1cb9d2f25d25ce2d4d93815fba</anchor>
      <arglist>(message)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LogDebug</name>
      <anchorfile>core__http__config__defaults_8h.html</anchorfile>
      <anchor>af60e8ffc327d136e5d0d8441ed98c98d</anchor>
      <arglist>(message)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>transport_interface.h</name>
    <path>/home/runner/work/aws-iot-device-sdk-embedded-C/aws-iot-device-sdk-embedded-C/libraries/standard/coreHTTP/source/interface/</path>
    <filename>transport__interface_8h.html</filename>
    <class kind="struct">TransportInterface_t</class>
    <member kind="typedef">
      <type>struct NetworkContext</type>
      <name>NetworkContext_t</name>
      <anchorfile>group__http__struct__types.html</anchorfile>
      <anchor>ga7769e434e7811caed8cd6fd7f9ec26ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>int32_t(*</type>
      <name>TransportRecv_t</name>
      <anchorfile>group__http__callback__types.html</anchorfile>
      <anchor>gaff3e08cfa7368b526ec1f8d87d10d7c2</anchor>
      <arglist>)(NetworkContext_t *pNetworkContext, void *pBuffer, size_t bytesToRecv)</arglist>
    </member>
    <member kind="typedef">
      <type>int32_t(*</type>
      <name>TransportSend_t</name>
      <anchorfile>group__http__callback__types.html</anchorfile>
      <anchor>ga90d540c6a334b9b73f94d83f08a0edd6</anchor>
      <arglist>)(NetworkContext_t *pNetworkContext, const void *pBuffer, size_t bytesToSend)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>findHeaderContext_t</name>
    <filename>structfind_header_context__t.html</filename>
    <member kind="variable">
      <type>const char *</type>
      <name>pField</name>
      <anchorfile>structfind_header_context__t.html</anchorfile>
      <anchor>aaa1682195d478087f4f441d4a718d070</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>fieldLen</name>
      <anchorfile>structfind_header_context__t.html</anchorfile>
      <anchor>a6f9016d3a374fe15bcff964ecffe10dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char **</type>
      <name>pValueLoc</name>
      <anchorfile>structfind_header_context__t.html</anchorfile>
      <anchor>a1426be39bf89d99c7b1ae54589968e20</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t *</type>
      <name>pValueLen</name>
      <anchorfile>structfind_header_context__t.html</anchorfile>
      <anchor>ab570a632683ff451d7dd5fa03ae4fe60</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>fieldFound</name>
      <anchorfile>structfind_header_context__t.html</anchorfile>
      <anchor>af106b9bc8d60b203ba86ded6bf3c81c8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>valueFound</name>
      <anchorfile>structfind_header_context__t.html</anchorfile>
      <anchor>a99ebe7e0a7edfb209c4411ed81eaea69</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>HTTPClient_ResponseHeaderParsingCallback_t</name>
    <filename>struct_h_t_t_p_client___response_header_parsing_callback__t.html</filename>
    <member kind="variable">
      <type>void(*</type>
      <name>onHeaderCallback</name>
      <anchorfile>struct_h_t_t_p_client___response_header_parsing_callback__t.html</anchorfile>
      <anchor>abed82645f6b2a8026b27dd6a6c0bcf30</anchor>
      <arglist>)(void *pContext, const char *fieldLoc, size_t fieldLen, const char *valueLoc, size_t valueLen, uint16_t statusCode)</arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>pContext</name>
      <anchorfile>struct_h_t_t_p_client___response_header_parsing_callback__t.html</anchorfile>
      <anchor>af7f741b5b476b653d1fca5b526404b55</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>HTTPParsingContext_t</name>
    <filename>struct_h_t_t_p_parsing_context__t.html</filename>
    <member kind="variable">
      <type>http_parser</type>
      <name>httpParser</name>
      <anchorfile>struct_h_t_t_p_parsing_context__t.html</anchorfile>
      <anchor>a2eec93f35846f77ae76346b9cfd2f929</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>HTTPParsingState_t</type>
      <name>state</name>
      <anchorfile>struct_h_t_t_p_parsing_context__t.html</anchorfile>
      <anchor>acecc99ec16396a6a5f2fae9aeb77c11f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>HTTPResponse_t *</type>
      <name>pResponse</name>
      <anchorfile>struct_h_t_t_p_parsing_context__t.html</anchorfile>
      <anchor>a82a9ef99d459edda5b28bd970e431756</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>isHeadResponse</name>
      <anchorfile>struct_h_t_t_p_parsing_context__t.html</anchorfile>
      <anchor>ad76fd5c0284963414074fad62eb0cd02</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pBufferCur</name>
      <anchorfile>struct_h_t_t_p_parsing_context__t.html</anchorfile>
      <anchor>ad51bd758cbd948379501c4dc9c8ac883</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pLastHeaderField</name>
      <anchorfile>struct_h_t_t_p_parsing_context__t.html</anchorfile>
      <anchor>a6b30a78efcdd943912a0597b06d9faca</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>lastHeaderFieldLen</name>
      <anchorfile>struct_h_t_t_p_parsing_context__t.html</anchorfile>
      <anchor>ab754524f58a9e270fcef98c250175f26</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pLastHeaderValue</name>
      <anchorfile>struct_h_t_t_p_parsing_context__t.html</anchorfile>
      <anchor>ab33bd5fcf92399e9c094141e43c0ae6a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>lastHeaderValueLen</name>
      <anchorfile>struct_h_t_t_p_parsing_context__t.html</anchorfile>
      <anchor>a757b82f506c9fdf41e1935d56a4296ad</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>HTTPRequestHeaders_t</name>
    <filename>struct_h_t_t_p_request_headers__t.html</filename>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pBuffer</name>
      <anchorfile>struct_h_t_t_p_request_headers__t.html</anchorfile>
      <anchor>acaffc0e3ae5413bf7c673f83388bc7c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>bufferLen</name>
      <anchorfile>struct_h_t_t_p_request_headers__t.html</anchorfile>
      <anchor>abe5f1cb0cd15619a6f8716872d7f2617</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>headersLen</name>
      <anchorfile>struct_h_t_t_p_request_headers__t.html</anchorfile>
      <anchor>acc36d2ff2421ace537c37b12660ec49b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>HTTPRequestInfo_t</name>
    <filename>struct_h_t_t_p_request_info__t.html</filename>
    <member kind="variable">
      <type>const char *</type>
      <name>pMethod</name>
      <anchorfile>struct_h_t_t_p_request_info__t.html</anchorfile>
      <anchor>a620cfbaf3f3857a99b7068be7e892585</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>methodLen</name>
      <anchorfile>struct_h_t_t_p_request_info__t.html</anchorfile>
      <anchor>a50162d5b598dfdf109f3b4af16ba41ce</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pPath</name>
      <anchorfile>struct_h_t_t_p_request_info__t.html</anchorfile>
      <anchor>a9bd015fcfe7ed1f5f1781d83417468a8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>pathLen</name>
      <anchorfile>struct_h_t_t_p_request_info__t.html</anchorfile>
      <anchor>ac19e3e9531dfb9a601d6a9cf6bd757fc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pHost</name>
      <anchorfile>struct_h_t_t_p_request_info__t.html</anchorfile>
      <anchor>ab056fe58943dd9402b319684cd86d3e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>hostLen</name>
      <anchorfile>struct_h_t_t_p_request_info__t.html</anchorfile>
      <anchor>adcfa3373309d5706788727cab5e92895</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>reqFlags</name>
      <anchorfile>struct_h_t_t_p_request_info__t.html</anchorfile>
      <anchor>a157ebe1394fe870d342b25a5521f2505</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>HTTPResponse_t</name>
    <filename>struct_h_t_t_p_response__t.html</filename>
    <member kind="variable">
      <type>uint8_t *</type>
      <name>pBuffer</name>
      <anchorfile>struct_h_t_t_p_response__t.html</anchorfile>
      <anchor>a8198002b2a936b784cd791560c1a3ca8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>bufferLen</name>
      <anchorfile>struct_h_t_t_p_response__t.html</anchorfile>
      <anchor>a5b224d37c0a03f44326a86c27067b18a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>HTTPClient_ResponseHeaderParsingCallback_t *</type>
      <name>pHeaderParsingCallback</name>
      <anchorfile>struct_h_t_t_p_response__t.html</anchorfile>
      <anchor>a0516505352424390934e765b2fed50ab</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>HTTPClient_GetCurrentTimeFunc_t</type>
      <name>getTime</name>
      <anchorfile>struct_h_t_t_p_response__t.html</anchorfile>
      <anchor>ac6001c06fdaae311c3ddf859b9b3a3ce</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>pHeaders</name>
      <anchorfile>struct_h_t_t_p_response__t.html</anchorfile>
      <anchor>a88006d526bae430301bd668e6348e6ac</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>headersLen</name>
      <anchorfile>struct_h_t_t_p_response__t.html</anchorfile>
      <anchor>a7259691ae7623c45e73f7baa28b1268c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const uint8_t *</type>
      <name>pBody</name>
      <anchorfile>struct_h_t_t_p_response__t.html</anchorfile>
      <anchor>aa712a2fd7fb57ef514d383e47c96ae26</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>bodyLen</name>
      <anchorfile>struct_h_t_t_p_response__t.html</anchorfile>
      <anchor>a055ce88aa4b185d6d5d9e73048a933fe</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>statusCode</name>
      <anchorfile>struct_h_t_t_p_response__t.html</anchorfile>
      <anchor>a2eaee249cd6a2bb4c86c4ea62362df1d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>contentLength</name>
      <anchorfile>struct_h_t_t_p_response__t.html</anchorfile>
      <anchor>ab3787660398f9bb866ee39a976106748</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>headerCount</name>
      <anchorfile>struct_h_t_t_p_response__t.html</anchorfile>
      <anchor>a2cc9259ce7b23b3d11c126bde0b700da</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint32_t</type>
      <name>respFlags</name>
      <anchorfile>struct_h_t_t_p_response__t.html</anchorfile>
      <anchor>a43d3cd31feebcd50ab2bd3078d1a77bc</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>TransportInterface_t</name>
    <filename>struct_transport_interface__t.html</filename>
    <member kind="variable">
      <type>TransportRecv_t</type>
      <name>recv</name>
      <anchorfile>struct_transport_interface__t.html</anchorfile>
      <anchor>a7c34e9b865e2a509306f09c7dfa3699e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TransportSend_t</type>
      <name>send</name>
      <anchorfile>struct_transport_interface__t.html</anchorfile>
      <anchor>a01cd9935e9a5266ca196243a0054d489</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>NetworkContext_t *</type>
      <name>pNetworkContext</name>
      <anchorfile>struct_transport_interface__t.html</anchorfile>
      <anchor>aaf4702050bef8d62714a4d3900e95087</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>http_enum_types</name>
    <title>Enumerated Types</title>
    <filename>group__http__enum__types.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>HTTPStatus_t</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>ga39fad9050063a355ffd95e95381e743f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPSuccess</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fa68f5addd5bf4367e177957a4c43dd4bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPInvalidParameter</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743faed61c0d831bc64e24db373b803837138</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPNetworkError</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fa59fa581a112b1994db1c45d8a7f84160</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPPartialResponse</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fa2b345207e843dc4e3d2ef9ea34c7f024</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPNoResponse</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fa8aa7c86301f91fd4b3c0b698ab8c177a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPInsufficientMemory</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fa33aae6d01bae2736a4a977ef04399ad4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPSecurityAlertResponseHeadersSizeLimitExceeded</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fac9de51dc48124bbbc331a9161afe3862</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPSecurityAlertExtraneousResponseData</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743faf6441d45e373b7686e45c2d9a0d12dd3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPSecurityAlertInvalidChunkHeader</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743faef569c934115aa105cc2cff1f72ef78b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPSecurityAlertInvalidProtocolVersion</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fab6ed7f7e24e250896a7b35825006d460</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPSecurityAlertInvalidStatusCode</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743faf53f0e027fd6d66ecd796bce19711562</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPSecurityAlertInvalidCharacter</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fa075511c32463db38713944d4e43cc01f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPSecurityAlertInvalidContentLength</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fac1b1794cf29fd99be06e0b0ed5f9213d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPParserInternalError</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fa37855790aef77f76074771d000b3e76b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPHeaderNotFound</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fad2fc47ae0e9bbf4a459f38a36020824a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HTTPInvalidResponse</name>
      <anchorfile>group__http__enum__types.html</anchorfile>
      <anchor>gga39fad9050063a355ffd95e95381e743fa8674a7c17d6878db3a6cf7e2e6e6a398</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>http_callback_types</name>
    <title>Callback Types</title>
    <filename>group__http__callback__types.html</filename>
    <member kind="typedef">
      <type>uint32_t(*</type>
      <name>HTTPClient_GetCurrentTimeFunc_t</name>
      <anchorfile>group__http__callback__types.html</anchorfile>
      <anchor>ga32d6894c1c9d62568136203b57c6d796</anchor>
      <arglist>)(void)</arglist>
    </member>
    <member kind="typedef">
      <type>int32_t(*</type>
      <name>TransportRecv_t</name>
      <anchorfile>group__http__callback__types.html</anchorfile>
      <anchor>gaff3e08cfa7368b526ec1f8d87d10d7c2</anchor>
      <arglist>)(NetworkContext_t *pNetworkContext, void *pBuffer, size_t bytesToRecv)</arglist>
    </member>
    <member kind="typedef">
      <type>int32_t(*</type>
      <name>TransportSend_t</name>
      <anchorfile>group__http__callback__types.html</anchorfile>
      <anchor>ga90d540c6a334b9b73f94d83f08a0edd6</anchor>
      <arglist>)(NetworkContext_t *pNetworkContext, const void *pBuffer, size_t bytesToSend)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>http_struct_types</name>
    <title>Parameter Structures</title>
    <filename>group__http__struct__types.html</filename>
    <class kind="struct">HTTPRequestHeaders_t</class>
    <class kind="struct">HTTPRequestInfo_t</class>
    <class kind="struct">HTTPClient_ResponseHeaderParsingCallback_t</class>
    <class kind="struct">HTTPResponse_t</class>
    <class kind="struct">TransportInterface_t</class>
    <member kind="typedef">
      <type>struct NetworkContext</type>
      <name>NetworkContext_t</name>
      <anchorfile>group__http__struct__types.html</anchorfile>
      <anchor>ga7769e434e7811caed8cd6fd7f9ec26ec</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>http_basic_types</name>
    <title>Basic Types</title>
    <filename>group__http__basic__types.html</filename>
  </compound>
  <compound kind="group">
    <name>http_constants</name>
    <title>Constants</title>
    <filename>group__http__constants.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_METHOD_GET</name>
      <anchorfile>group__http__constants.html</anchorfile>
      <anchor>ga91faefe43f8d3577719ba174c8a5ebcd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_METHOD_PUT</name>
      <anchorfile>group__http__constants.html</anchorfile>
      <anchor>gac1999442fe79f9c13dc51086c61e8774</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_METHOD_POST</name>
      <anchorfile>group__http__constants.html</anchorfile>
      <anchor>ga6c5872fb505bc0d37a9de2e2192dc7c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_METHOD_HEAD</name>
      <anchorfile>group__http__constants.html</anchorfile>
      <anchor>ga7fe13cca8f55d3159b4bbda7c656804d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_MAX_CONTENT_LENGTH_HEADER_LENGTH</name>
      <anchorfile>group__http__constants.html</anchorfile>
      <anchor>ga7a6adfe8f1a63441e5838a004040588f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_RANGE_REQUEST_END_OF_FILE</name>
      <anchorfile>group__http__constants.html</anchorfile>
      <anchor>ga4719dc6fd6e1b92739e1c3a88efa7046</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>http_send_flags</name>
    <title>HTTPClient_Send Flags</title>
    <filename>group__http__send__flags.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_SEND_DISABLE_CONTENT_LENGTH_FLAG</name>
      <anchorfile>group__http__send__flags.html</anchorfile>
      <anchor>ga66db30f9be610b9f1c851d9cd9cb3cff</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>http_request_flags</name>
    <title>HTTPRequestInfo_t Flags</title>
    <filename>group__http__request__flags.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_REQUEST_KEEP_ALIVE_FLAG</name>
      <anchorfile>group__http__request__flags.html</anchorfile>
      <anchor>gadec54e27d2d24006b4c22a7f945282de</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>http_response_flags</name>
    <title>HTTPResponse_t Flags</title>
    <filename>group__http__response__flags.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_RESPONSE_CONNECTION_CLOSE_FLAG</name>
      <anchorfile>group__http__response__flags.html</anchorfile>
      <anchor>ga362e461ad360ffb2ae6b986c688993c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HTTP_RESPONSE_CONNECTION_KEEP_ALIVE_FLAG</name>
      <anchorfile>group__http__response__flags.html</anchorfile>
      <anchor>ga259404ed98011458fb1ad2e90fba1752</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>http_design</name>
    <title>Design</title>
    <filename>http_design.html</filename>
    <docanchor file="http_design.html" title="Transport Interface">http_transport_interface_blurb</docanchor>
    <docanchor file="http_design.html" title="Building an HTTP Request">http_request_serialization</docanchor>
    <docanchor file="http_design.html" title="HTTP Range Requests and Partial Content Responses">http_range_support</docanchor>
    <docanchor file="http_design.html" title="Receiving and Parsing an HTTP Response">http_response_deserialization</docanchor>
    <docanchor file="http_design.html" title="Reading the HTTP Response Headers">http_response_headers</docanchor>
  </compound>
  <compound kind="page">
    <name>http_porting</name>
    <title>Porting Guide</title>
    <filename>http_porting.html</filename>
    <docanchor file="http_porting.html" title="Configuration Macros">http_porting_config</docanchor>
    <docanchor file="http_porting.html" title="Transport Interface">http_porting_transport</docanchor>
    <docanchor file="http_porting.html" title="Time Function">http_porting_time</docanchor>
  </compound>
  <compound kind="page">
    <name>http_config</name>
    <title>Configurations</title>
    <filename>http_config.html</filename>
    <docanchor file="http_config.html">HTTP_DO_NOT_USE_CUSTOM_CONFIG</docanchor>
    <docanchor file="http_config.html">HTTP_MAX_RESPONSE_HEADERS_SIZE_BYTES</docanchor>
    <docanchor file="http_config.html">HTTP_USER_AGENT_VALUE</docanchor>
    <docanchor file="http_config.html">HTTP_SEND_RETRY_TIMEOUT_MS</docanchor>
    <docanchor file="http_config.html">HTTP_RECV_RETRY_TIMEOUT_MS</docanchor>
    <docanchor file="http_config.html" title="LogError">http_logerror</docanchor>
    <docanchor file="http_config.html" title="LogWarn">http_logwarn</docanchor>
    <docanchor file="http_config.html" title="LogInfo">http_loginfo</docanchor>
    <docanchor file="http_config.html" title="LogDebug">http_logdebug</docanchor>
  </compound>
  <compound kind="page">
    <name>http_functions</name>
    <title>Functions</title>
    <filename>http_functions.html</filename>
  </compound>
  <compound kind="page">
    <name>httpclient_initializerequestheaders_function</name>
    <title>HTTPClient_InitializeRequestHeaders</title>
    <filename>httpclient_initializerequestheaders_function.html</filename>
  </compound>
  <compound kind="page">
    <name>httpclient_addheader_function</name>
    <title>HTTPClient_AddHeader</title>
    <filename>httpclient_addheader_function.html</filename>
  </compound>
  <compound kind="page">
    <name>httpclient_addrangeheader_function</name>
    <title>HTTPClient_AddRangeHeader</title>
    <filename>httpclient_addrangeheader_function.html</filename>
  </compound>
  <compound kind="page">
    <name>httpclient_send_function</name>
    <title>HTTPClient_Send</title>
    <filename>httpclient_send_function.html</filename>
  </compound>
  <compound kind="page">
    <name>httpclient_readheader_function</name>
    <title>HTTPClient_ReadHeader</title>
    <filename>httpclient_readheader_function.html</filename>
  </compound>
  <compound kind="page">
    <name>httpclient_strerror_function</name>
    <title>HTTPClient_strerror</title>
    <filename>httpclient_strerror_function.html</filename>
  </compound>
  <compound kind="page">
    <name>http_transport_interface</name>
    <title>Transport Interface</title>
    <filename>http_transport_interface.html</filename>
    <docanchor file="http_transport_interface.html" title="Transport Interface Overview">http_transport_interface_overview</docanchor>
    <docanchor file="http_transport_interface.html" title="Implementing the Transport Interface">http_transport_interface_implementation</docanchor>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Overview</title>
    <filename>index.html</filename>
    <docanchor file="index.html">http</docanchor>
    <docanchor file="index.html" title="Memory Requirements">http_memory_requirements</docanchor>
  </compound>
</tagfile>
