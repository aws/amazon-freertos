var group__https__client__datatypes__paramstructs =
[
    [ "IotHttpsClientCallbacks_t", "struct_iot_https_client_callbacks__t.html", [
      [ "appendHeaderCallback", "struct_iot_https_client_callbacks__t.html#aa740a06066ee5058af299994b48b84c7", null ],
      [ "writeCallback", "struct_iot_https_client_callbacks__t.html#aa509416ac5f315ecaf89010ba95e4a1b", null ],
      [ "readReadyCallback", "struct_iot_https_client_callbacks__t.html#a3471688b6ca1b2582c84453ffd38a1b4", null ],
      [ "responseCompleteCallback", "struct_iot_https_client_callbacks__t.html#a9c89a3dc89a85ccd164124947368824a", null ],
      [ "connectionClosedCallback", "struct_iot_https_client_callbacks__t.html#a2927a57bdff467c1a4774e198aaac5b0", null ],
      [ "errorCallback", "struct_iot_https_client_callbacks__t.html#a7d07f73d45d33a1681afc1aa0872874c", null ]
    ] ],
    [ "IotHttpsUserBuffer_t", "struct_iot_https_user_buffer__t.html", [
      [ "pBuffer", "struct_iot_https_user_buffer__t.html#a3326d02c65169e70dccb31ac931da438", null ],
      [ "bufferLen", "struct_iot_https_user_buffer__t.html#a1f434521e2be8ac2cbde821af243cac9", null ]
    ] ],
    [ "IotHttpsSyncInfo_t", "struct_iot_https_sync_info__t.html", [
      [ "pBody", "struct_iot_https_sync_info__t.html#add7230f049c1f80e1a87bee9e5750853", null ],
      [ "bodyLen", "struct_iot_https_sync_info__t.html#a4833f3a5cf2317a096e0879cf022a1e1", null ]
    ] ],
    [ "IotHttpsAsyncInfo_t", "struct_iot_https_async_info__t.html", [
      [ "callbacks", "struct_iot_https_async_info__t.html#a35ce2cbf2c0d2e9b6fb01f33c665b388", null ],
      [ "pPrivData", "struct_iot_https_async_info__t.html#a4cd942bc17e6dad03b77901bf7a709dc", null ]
    ] ],
    [ "IotHttpsConnectionInfo_t", "struct_iot_https_connection_info__t.html", [
      [ "pAddress", "struct_iot_https_connection_info__t.html#aaaa1911897186d2405351191181b5f32", null ],
      [ "addressLen", "struct_iot_https_connection_info__t.html#a6492288283b3678981763f9a99128d25", null ],
      [ "port", "struct_iot_https_connection_info__t.html#a667022eae6af9b4d784613d5f13d9cce", null ],
      [ "flags", "struct_iot_https_connection_info__t.html#a76b141ec84b35ee5ed79c20a4cfa24c5", null ],
      [ "timeout", "struct_iot_https_connection_info__t.html#a8367f55ad73560039a7c3a8fc7d54bc5", null ],
      [ "pCaCert", "struct_iot_https_connection_info__t.html#afcb8db31570bc084900eda12fb4a7ced", null ],
      [ "caCertLen", "struct_iot_https_connection_info__t.html#a330d03b29b37c9742c061a374f8a1def", null ],
      [ "pClientCert", "struct_iot_https_connection_info__t.html#aa228e9c353db50544ed979a11138da1a", null ],
      [ "clientCertLen", "struct_iot_https_connection_info__t.html#a0f21228b09428ea842657ce93ec8791a", null ],
      [ "pPrivateKey", "struct_iot_https_connection_info__t.html#a0c23234ee46a593299b6e6c3afe9f765", null ],
      [ "privateKeyLen", "struct_iot_https_connection_info__t.html#a831ab9fad9ef773dcf061293ba09fe31", null ],
      [ "pAlpnProtocols", "struct_iot_https_connection_info__t.html#a0d7ae9779e900ed79a0e8ee152c533e4", null ],
      [ "alpnProtocolsLen", "struct_iot_https_connection_info__t.html#a32d8bdbd15154cc5afe42285c8b6f034", null ],
      [ "userBuffer", "struct_iot_https_connection_info__t.html#a3b998a5ba9ef3b5ab4c887ff8598633c", null ],
      [ "pNetworkInterface", "struct_iot_https_connection_info__t.html#a7925af087404997c804030de9a5c2230", null ]
    ] ],
    [ "IotHttpsRequestInfo_t", "struct_iot_https_request_info__t.html", [
      [ "pPath", "struct_iot_https_request_info__t.html#ab03a896e957dfa4627633fc5902ad327", null ],
      [ "pathLen", "struct_iot_https_request_info__t.html#af56fe3cc2b4d8eb34c585dca30997606", null ],
      [ "method", "struct_iot_https_request_info__t.html#ac80defadf129996a4847d8ef611911d8", null ],
      [ "pHost", "struct_iot_https_request_info__t.html#a151da299a963c55cea6089d810454621", null ],
      [ "hostLen", "struct_iot_https_request_info__t.html#a35b91f903dbbd7ebe84e2430fd016545", null ],
      [ "isNonPersistent", "struct_iot_https_request_info__t.html#ae4a91f589052c5c8410e20d4e9ad072f", null ],
      [ "userBuffer", "struct_iot_https_request_info__t.html#a6b920a6db879dd3332821bf8e3b62ea0", null ],
      [ "isAsync", "struct_iot_https_request_info__t.html#a284d9a4c65f45eea086550cd7af26cdf", null ],
      [ "pAsyncInfo", "struct_iot_https_request_info__t.html#ab913b433b6a6abdb31d367b44902248e", null ],
      [ "pSyncInfo", "struct_iot_https_request_info__t.html#ae7db8c66e3aea83ac042c5350751ae45", null ],
      [ "u", "struct_iot_https_request_info__t.html#a3c10eb9f9a57b5017d5b2b9dd4fb8ca4", null ]
    ] ],
    [ "IotHttpsResponseInfo_t", "struct_iot_https_response_info__t.html", [
      [ "userBuffer", "struct_iot_https_response_info__t.html#a2effbd283351264b492cbb3b14704c86", null ],
      [ "pSyncInfo", "struct_iot_https_response_info__t.html#afa3decd344371f9257d1fde04c519a03", null ]
    ] ]
];