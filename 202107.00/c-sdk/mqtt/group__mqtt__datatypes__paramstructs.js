var group__mqtt__datatypes__paramstructs =
[
    [ "IotMqttPublishInfo_t", "structIotMqttPublishInfo__t.html", [
      [ "qos", "structIotMqttPublishInfo__t.html#a483e9224f18a5d0cca607ca5dee5bad4", null ],
      [ "retain", "structIotMqttPublishInfo__t.html#ae0bc925a73b0826a17a254c2168b0127", null ],
      [ "pTopicName", "structIotMqttPublishInfo__t.html#a7242bf67c6a74d6659e6ae78d3008c52", null ],
      [ "topicNameLength", "structIotMqttPublishInfo__t.html#a93b0c97558bd24ba6324c228ec84e684", null ],
      [ "pPayload", "structIotMqttPublishInfo__t.html#a5b4df4baeab5357db34df56d979ef044", null ],
      [ "payloadLength", "structIotMqttPublishInfo__t.html#a5c223cae824741c2c8cb3bc27102ec79", null ],
      [ "retryMs", "structIotMqttPublishInfo__t.html#aba435c0af41d6146bafa04d56cce3c80", null ],
      [ "retryLimit", "structIotMqttPublishInfo__t.html#a9da2582d17581ae022f8061cdaecf62b", null ]
    ] ],
    [ "IotMqttCallbackParam_t", "structIotMqttCallbackParam__t.html", [
      [ "mqttConnection", "structIotMqttCallbackParam__t.html#a0a0e08563884d2dbdfb233aae8314ecb", null ],
      [ "type", "structIotMqttCallbackParam__t.html#aedf5447c46b2cda8c19f8d5c69e0993c", null ],
      [ "reference", "structIotMqttCallbackParam__t.html#abe1c9f048fb7bf5cadcd8573eded1188", null ],
      [ "result", "structIotMqttCallbackParam__t.html#a6045c426bb7ec49444cb253327babab7", null ],
      [ "operation", "structIotMqttCallbackParam__t.html#ad94b73414e669c6e0d9c8ff2f271eef6", null ],
      [ "pTopicFilter", "structIotMqttCallbackParam__t.html#aa978834d27b4c1f4ed647f2f979ee6c2", null ],
      [ "topicFilterLength", "structIotMqttCallbackParam__t.html#ad5bb2105fe94c7b442d364849c632a88", null ],
      [ "info", "structIotMqttCallbackParam__t.html#a703eb0008838c2076cc83c562dbaf79e", null ],
      [ "message", "structIotMqttCallbackParam__t.html#aacb13590175dcbd6ec0f44911a64a1ab", null ],
      [ "disconnectReason", "structIotMqttCallbackParam__t.html#a1204ced13d78eb18bbf8daaff08f1601", null ],
      [ "u", "structIotMqttCallbackParam__t.html#a9deff64564a7b43d872fcb28f047e1bc", null ]
    ] ],
    [ "IotMqttCallbackInfo_t", "structIotMqttCallbackInfo__t.html", [
      [ "pCallbackContext", "structIotMqttCallbackInfo__t.html#a5aee2a21986f1f9f29040a3de47d67f5", null ],
      [ "function", "structIotMqttCallbackInfo__t.html#a8ad8cdffb834fd2282b787d3ba9adc98", null ]
    ] ],
    [ "IotMqttSubscription_t", "structIotMqttSubscription__t.html", [
      [ "qos", "structIotMqttSubscription__t.html#ac69481fe2236fecc936aa41ee47e50a5", null ],
      [ "pTopicFilter", "structIotMqttSubscription__t.html#af8eecdfcec488790e377b1025ba0e42a", null ],
      [ "topicFilterLength", "structIotMqttSubscription__t.html#aeeca6e02a2964f8d9921590a2b9562e4", null ],
      [ "callback", "structIotMqttSubscription__t.html#a8a45a7c71a222d5eeac809d6a8028448", null ]
    ] ],
    [ "IotMqttConnectInfo_t", "structIotMqttConnectInfo__t.html", [
      [ "awsIotMqttMode", "structIotMqttConnectInfo__t.html#a25de63bc6ac4d21ca2a53e7df823b713", null ],
      [ "cleanSession", "structIotMqttConnectInfo__t.html#a7c1ee639c15085caa74f16aacf5d6c8c", null ],
      [ "pPreviousSubscriptions", "structIotMqttConnectInfo__t.html#af8b19dd2558ca1dcda19b4e0e75e477e", null ],
      [ "previousSubscriptionCount", "structIotMqttConnectInfo__t.html#a295eddcf48256963cca1b13cf2abf2c7", null ],
      [ "pWillInfo", "structIotMqttConnectInfo__t.html#af6b8c985f015bb8d85a60e08dda36c11", null ],
      [ "keepAliveSeconds", "structIotMqttConnectInfo__t.html#accfc0aa36662cbe90c92eaf65ca67242", null ],
      [ "pClientIdentifier", "structIotMqttConnectInfo__t.html#a3f560e3658cc6aa0aaa3c3ddd1383137", null ],
      [ "clientIdentifierLength", "structIotMqttConnectInfo__t.html#a1e16cc20774bc1e17031c5a04340c9b0", null ],
      [ "pUserName", "structIotMqttConnectInfo__t.html#a56fddcdd3649fffcc79e82eaa946248a", null ],
      [ "userNameLength", "structIotMqttConnectInfo__t.html#af4717004fa29a0bd853e73f01de6c01b", null ],
      [ "pPassword", "structIotMqttConnectInfo__t.html#a3a31057446b64d8b8d98104a0cd4d6ef", null ],
      [ "passwordLength", "structIotMqttConnectInfo__t.html#adb3db722e4d5d51138bb13283e83b66b", null ]
    ] ],
    [ "IotMqttPacketInfo_t", "structIotMqttPacketInfo__t.html", [
      [ "pRemainingData", "structIotMqttPacketInfo__t.html#afb6dc53000887371217c9f8716bceae9", null ],
      [ "remainingLength", "structIotMqttPacketInfo__t.html#a9e49da9a2ccc8f9be9b288e3607a8c00", null ],
      [ "packetIdentifier", "structIotMqttPacketInfo__t.html#ac53b7fd73876c1371788c989d0911626", null ],
      [ "type", "structIotMqttPacketInfo__t.html#a0e05291d67be9b06f391eea3102a63bb", null ],
      [ "pubInfo", "structIotMqttPacketInfo__t.html#ae746fe77f27a05f297cb785525ed0489", null ]
    ] ],
    [ "IotMqttSerializer_t", "structIotMqttSerializer__t.html", [
      [ "getPacketType", "structIotMqttSerializer__t.html#a75687c9665cd327adb5cc00802f15cce", null ],
      [ "getRemainingLength", "structIotMqttSerializer__t.html#a30c131dfebc41234173804585990f3ea", null ],
      [ "freePacket", "structIotMqttSerializer__t.html#ad9c41e3fdd5749b04f5576d7d1f947cc", null ],
      [ "connect", "structIotMqttSerializer__t.html#ae922a777652736fdb9eaf87771edb3e5", null ],
      [ "publish", "structIotMqttSerializer__t.html#aec803c8b9bbe8a6b1d22d2de8a35c157", null ],
      [ "publishSetDup", "structIotMqttSerializer__t.html#ab9fff6b297dbcc0cb55b346e2e689791", null ],
      [ "puback", "structIotMqttSerializer__t.html#a379dabb87df5b63abf8dc4a215f18745", null ],
      [ "subscribe", "structIotMqttSerializer__t.html#a4d69cc3a9709b000ecfbdbf09d92cdd5", null ],
      [ "unsubscribe", "structIotMqttSerializer__t.html#a18e7ded2b4f9784ded6c43ee4b383e40", null ],
      [ "pingreq", "structIotMqttSerializer__t.html#a99fccec385d1411a7b32c24f29026197", null ],
      [ "disconnect", "structIotMqttSerializer__t.html#a647e627923eff01172d2a27e184af0cc", null ],
      [ "serialize", "structIotMqttSerializer__t.html#a9e427831b87602b4fc7e573a589f2af8", null ],
      [ "connack", "structIotMqttSerializer__t.html#a4ebeba34e6704f82581987021cc8cd30", null ],
      [ "publish", "structIotMqttSerializer__t.html#a7186818e84771fa0ca443174b86e9833", null ],
      [ "puback", "structIotMqttSerializer__t.html#a3cf789a87735ffe6a170921195a82214", null ],
      [ "suback", "structIotMqttSerializer__t.html#a5a8f696a985b93a7390bf94fdfbbfb67", null ],
      [ "unsuback", "structIotMqttSerializer__t.html#aeb3ba6e9d914172375cbabd25fac3c9a", null ],
      [ "pingresp", "structIotMqttSerializer__t.html#a36dcb58b245e7f5af79c9cd789c7475b", null ],
      [ "deserialize", "structIotMqttSerializer__t.html#a7d2b0de45ecb17fc7156454b4262423a", null ]
    ] ],
    [ "IotMqttNetworkInfo_t", "structIotMqttNetworkInfo__t.html", [
      [ "createNetworkConnection", "structIotMqttNetworkInfo__t.html#a93287a7d47f84bc160bf3dcddb338fde", null ],
      [ "pNetworkServerInfo", "structIotMqttNetworkInfo__t.html#acd2dece9543e35a5e0227aaec05e7df3", null ],
      [ "pNetworkCredentialInfo", "structIotMqttNetworkInfo__t.html#a6b0cd12b3cf580f44f1ef9ca9d6a0253", null ],
      [ "setup", "structIotMqttNetworkInfo__t.html#abe07fab68b70e24ec9ab4e412868b08b", null ],
      [ "pNetworkConnection", "structIotMqttNetworkInfo__t.html#ad23891069d681b9d117776d66bab0b93", null ],
      [ "u", "structIotMqttNetworkInfo__t.html#a5489253fc2f6024f13460187fc809910", null ],
      [ "pNetworkInterface", "structIotMqttNetworkInfo__t.html#ade7f3b72fa18293d1f2fbcfadfcbbf37", null ],
      [ "disconnectCallback", "structIotMqttNetworkInfo__t.html#a7d3b3e7895b3240f9e4904191cc98402", null ],
      [ "pMqttSerializer", "structIotMqttNetworkInfo__t.html#a2d2a568f60fa43c6290996e5951a4bda", null ]
    ] ]
];