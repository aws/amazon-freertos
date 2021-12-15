var group__ble__datatypes__structs =
[
    [ "IotBleAdvertisementParams_t", "structIotBleAdvertisementParams__t.html", [
      [ "appearance", "structIotBleAdvertisementParams__t.html#af2c2b304dfaa85ee4178a6d9f296b98d", null ],
      [ "minInterval", "structIotBleAdvertisementParams__t.html#a52c31a5243a0d16599e1972ea0bd9bfe", null ],
      [ "maxInterval", "structIotBleAdvertisementParams__t.html#a418538a4d0f4716b887e2e4bc4949b0d", null ],
      [ "pManufacturerData", "structIotBleAdvertisementParams__t.html#a9f43ca0255fd1e0ca92c91b3e7cd18db", null ],
      [ "pServiceData", "structIotBleAdvertisementParams__t.html#a2e207496dc0ed5fd3fc10012db17b84c", null ],
      [ "pUUID1", "structIotBleAdvertisementParams__t.html#a576f267fcacad38de0ca49d2efb88e12", null ],
      [ "pUUID2", "structIotBleAdvertisementParams__t.html#ad8026cb77eceb8696900e94068b46930", null ],
      [ "manufacturerLen", "structIotBleAdvertisementParams__t.html#a63700e2aba9ad874f66141920e638695", null ],
      [ "serviceDataLen", "structIotBleAdvertisementParams__t.html#aee966567b6e769c7c5227644d8c0c161", null ],
      [ "includeTxPower", "structIotBleAdvertisementParams__t.html#aa002e1df4d792962c497f51b8ba09fcc", null ],
      [ "name", "structIotBleAdvertisementParams__t.html#a61420f4960f7f95ca5583242b19fcb40", null ],
      [ "setScanRsp", "structIotBleAdvertisementParams__t.html#ac87e0e1172432dae910bba35a971cc86", null ]
    ] ],
    [ "IotBleConnectionParam_t", "structIotBleConnectionParam__t.html", [
      [ "minInterval", "structIotBleConnectionParam__t.html#acfaf698d7274f5a9868a859cc85c339a", null ],
      [ "maxInterval", "structIotBleConnectionParam__t.html#a34aa00be9f23aeddaee2df4452cbf47f", null ],
      [ "latency", "structIotBleConnectionParam__t.html#a65fdeb5d8f8936254af7de21270d0da2", null ],
      [ "timeout", "structIotBleConnectionParam__t.html#aa7fc274041d12a8da764f31df5fe2312", null ]
    ] ],
    [ "IotBleConnectionInfoListElement_t", "structIotBleConnectionInfoListElement__t.html", [
      [ "connectionList", "structIotBleConnectionInfoListElement__t.html#a2aa514fc93a437b163eb3de3aed51f7d", null ],
      [ "connectionParams", "structIotBleConnectionInfoListElement__t.html#a1696e126dfd03261814bac9da937597b", null ],
      [ "remoteBdAddr", "structIotBleConnectionInfoListElement__t.html#a84d19e76d732e3ab3bcdba21d9dbd34e", null ],
      [ "securityLevel", "structIotBleConnectionInfoListElement__t.html#a9bc19107a7927f8d3c5b0d2ff9bca083", null ],
      [ "clientCharDescrListHead", "structIotBleConnectionInfoListElement__t.html#a06544c5d16270eb20edaa6c82df56ec6", null ],
      [ "connId", "structIotBleConnectionInfoListElement__t.html#aa5f7b70790a024a5fe50986dd4da5167", null ],
      [ "isBonded", "structIotBleConnectionInfoListElement__t.html#a83efe4b777b0f0335cea8d3260a4d48b", null ]
    ] ],
    [ "IotBleReadEventParams_t", "structIotBleReadEventParams__t.html", [
      [ "connId", "structIotBleReadEventParams__t.html#a676480b3d0714a6523ef2f6b462ad100", null ],
      [ "transId", "structIotBleReadEventParams__t.html#a757b1a25aaac8450b18d8a38025f5c55", null ],
      [ "pRemoteBdAddr", "structIotBleReadEventParams__t.html#aa43998fb9ad5b52de6999b81566c1c1c", null ],
      [ "attrHandle", "structIotBleReadEventParams__t.html#a0ad867195c80665f6044f6ccbd97f3b6", null ],
      [ "offset", "structIotBleReadEventParams__t.html#a5ec6dc40b7f36b1157bc4495f621c836", null ]
    ] ],
    [ "IotBleWriteEventParams_t", "structIotBleWriteEventParams__t.html", [
      [ "transId", "structIotBleWriteEventParams__t.html#a85af199e536d8d3d146646b34a0b44d1", null ],
      [ "pRemoteBdAddr", "structIotBleWriteEventParams__t.html#ae87b754704ca4d8d3ed36754c59e6ccd", null ],
      [ "pValue", "structIotBleWriteEventParams__t.html#a10a31f9bce025cafb8757e37bbe3ee18", null ],
      [ "length", "structIotBleWriteEventParams__t.html#ab6aea1925055f9ec6851823481839ebd", null ],
      [ "connId", "structIotBleWriteEventParams__t.html#a24b14e2adf64d5b19a1c673d16133071", null ],
      [ "attrHandle", "structIotBleWriteEventParams__t.html#a21464a99a4e888983a53ccd70ef92ae1", null ],
      [ "offset", "structIotBleWriteEventParams__t.html#a8cd2a17851b785272f0a4f79ade75c4a", null ],
      [ "needRsp", "structIotBleWriteEventParams__t.html#a1956f3f8301cb64a0b3dc07102a194b2", null ],
      [ "isPrep", "structIotBleWriteEventParams__t.html#a9564b04190b11a43c0e3e6e4762312dc", null ]
    ] ],
    [ "IotBleExecWriteEventParams_t", "structIotBleExecWriteEventParams__t.html", [
      [ "transId", "structIotBleExecWriteEventParams__t.html#a4a5481463cd72677a29bdb10257dbbf8", null ],
      [ "pRemoteBdAddr", "structIotBleExecWriteEventParams__t.html#a16d9e31a657d8af854a17e30ca0eab94", null ],
      [ "connId", "structIotBleExecWriteEventParams__t.html#a02216a1366225590c5d053c313cc0887", null ],
      [ "execWrite", "structIotBleExecWriteEventParams__t.html#aa827e03b3ba7ed318fbe4efc56f4c46c", null ]
    ] ],
    [ "IotBleRespConfirmEventParams_t", "structIotBleRespConfirmEventParams__t.html", [
      [ "status", "structIotBleRespConfirmEventParams__t.html#a851536e1dacc9fb72bdee6b44b7279a2", null ],
      [ "handle", "structIotBleRespConfirmEventParams__t.html#a549e3a9ae98e7c2d3ca3b703b4bcf77f", null ]
    ] ],
    [ "IotBleIndicationSentEventParams_t", "structIotBleIndicationSentEventParams__t.html", [
      [ "pAttribute", "structIotBleIndicationSentEventParams__t.html#ac812d1e73ea99c389a8b1138f3b626b3", null ],
      [ "connId", "structIotBleIndicationSentEventParams__t.html#a90b85ff050b2d272c5ee00d353bd4f68", null ],
      [ "status", "structIotBleIndicationSentEventParams__t.html#aa8c003ab7617649167f9d510bf00f055", null ]
    ] ],
    [ "IotBleAttributeEvent_t", "structIotBleAttributeEvent.html", [
      [ "pParamRead", "structIotBleAttributeEvent.html#af000cffda3324dff9fb003e3ea5069e2", null ],
      [ "pParamWrite", "structIotBleAttributeEvent.html#a85a0549443858500f6ee232817499b76", null ],
      [ "pParamExecWrite", "structIotBleAttributeEvent.html#a89a133e4dd68ab388b7047813950f50d", null ],
      [ "pParamRespConfirm", "structIotBleAttributeEvent.html#a51476d61347df02cf125b0a8271e6632", null ],
      [ "pParamIndicationSent", "structIotBleAttributeEvent.html#a8a3277d571f30f3ab4005c0239ae33b5", null ],
      [ "xEventType", "structIotBleAttributeEvent.html#a6a6ba8826b0a11003de39d74fdceb124", null ]
    ] ],
    [ "IotBleAttributeData_t", "structIotBleAttributeData__t.html", [
      [ "size", "structIotBleAttributeData__t.html#afd86c0eed1e62534c14bc76c5ead332e", null ],
      [ "pData", "structIotBleAttributeData__t.html#aedfed227d6f8416925e5164e27ff4e1c", null ],
      [ "uuid", "structIotBleAttributeData__t.html#a66926c57bfc3ff0fe9ee8d25d0671eaa", null ],
      [ "handle", "structIotBleAttributeData__t.html#ac0823b7a61d73b561d8369654eb9b149", null ]
    ] ],
    [ "IotBleEventResponse_t", "structIotBleEventResponse__t.html", [
      [ "pAttrData", "structIotBleEventResponse__t.html#a9e8dd37339ec95aa8958a01a251e55b2", null ],
      [ "attrDataOffset", "structIotBleEventResponse__t.html#a64326656938aa31287fc2533d6aee973", null ],
      [ "eventStatus", "structIotBleEventResponse__t.html#a97a079474634f6fc46eb6e954eb3ef00", null ],
      [ "rspErrorStatus", "structIotBleEventResponse__t.html#a59be899db102159383553990f0b9b847", null ]
    ] ],
    [ "IotBleEventsCallbacks_t", "unionIotBleEventsCallbacks__t.html", [
      [ "pMtuChangedCb", "unionIotBleEventsCallbacks__t.html#a082a8b2b7b7ba0de0ae6970bcdee76fc", null ],
      [ "pConnectionCb", "unionIotBleEventsCallbacks__t.html#a4d7eb5b979658273711128fb9dd01f8f", null ],
      [ "pGAPPairingStateChangedCb", "unionIotBleEventsCallbacks__t.html#a1408546ac060ac04be4bc57e2e767fc7", null ],
      [ "pConnParameterUpdateRequestCb", "unionIotBleEventsCallbacks__t.html#aeee7e91eaf75ab2954920e393d1d6357", null ],
      [ "pNumericComparisonCb", "unionIotBleEventsCallbacks__t.html#a69ae6c4b48f24cec4407f8a96f3c9a6b", null ],
      [ "pvPtr", "unionIotBleEventsCallbacks__t.html#ac3481c0bb7d4a59eab511618323aa638", null ]
    ] ],
    [ "IotBleWifiProvListNetworksRequest_t", "structIotBleWifiProvListNetworksRequest__t.html", [
      [ "scanSize", "structIotBleWifiProvListNetworksRequest__t.html#afc5c1bd33dcd4c2488c20890ca2170cb", null ],
      [ "scanTimeoutMS", "structIotBleWifiProvListNetworksRequest__t.html#af53f46d03001516f0cd615c49c607656", null ]
    ] ],
    [ "IotBleWifiProvAddNetworkRequest_t", "structIotBleWifiProvAddNetworkRequest__t.html", [
      [ "network", "structIotBleWifiProvAddNetworkRequest__t.html#a968c2e6afd8d872e7275cbe56b8037b8", null ],
      [ "index", "structIotBleWifiProvAddNetworkRequest__t.html#abe29ab025ff6c8da9187f6fa3a8db61b", null ],
      [ "info", "structIotBleWifiProvAddNetworkRequest__t.html#ab90b9d1aa1038e54b595379c5d585b46", null ],
      [ "isProvisioned", "structIotBleWifiProvAddNetworkRequest__t.html#a1621eac626f49dc8c8bb4b217f5f9f9c", null ],
      [ "shouldConnect", "structIotBleWifiProvAddNetworkRequest__t.html#ac1f65f392e4fc873125865ef61e55a4e", null ]
    ] ],
    [ "IotBleWifiProvEditNetworkRequest_t", "structIotBleWifiProvEditNetworkRequest__t.html", [
      [ "currentPriorityIndex", "structIotBleWifiProvEditNetworkRequest__t.html#a97d66d9cc871a3e3bf1e62d9201d391d", null ],
      [ "newPriorityIndex", "structIotBleWifiProvEditNetworkRequest__t.html#a93179c614ffd194cc210646816eef684", null ]
    ] ],
    [ "IotBleWifiProvDeleteNetworkRequest_t", "structIotBleWifiProvDeleteNetworkRequest__t.html", [
      [ "priorityIndex", "structIotBleWifiProvDeleteNetworkRequest__t.html#a7706630697401eeb222f60bd1ae69bbe", null ]
    ] ],
    [ "IotBleWifiProvNetworkInfo_t", "structIotBleWifiProvNetworkInfo__t.html", [
      [ "pScannedNetwork", "structIotBleWifiProvNetworkInfo__t.html#a5e336e8528acffa0fa753c24ae7e852b", null ],
      [ "pSavedNetwork", "structIotBleWifiProvNetworkInfo__t.html#abdebbe7f74416a99a18ad8f73be4d065", null ],
      [ "info", "structIotBleWifiProvNetworkInfo__t.html#a5de08674707de9e450e8ef9e0345b1a2", null ],
      [ "index", "structIotBleWifiProvNetworkInfo__t.html#ae99d860470941b2397b65e86994f1534", null ],
      [ "isSavedNetwork", "structIotBleWifiProvNetworkInfo__t.html#a5715677ed37c162259d179ac42a7153d", null ],
      [ "isConnected", "structIotBleWifiProvNetworkInfo__t.html#ad1e069122684ec29a918f215d7e346c0", null ],
      [ "isHidden", "structIotBleWifiProvNetworkInfo__t.html#ab5c807ff5df4b0931eea54893581794b", null ]
    ] ],
    [ "IotBleWifiProvResponse_t", "structIotBleWifiProvResponse__t.html", [
      [ "requestType", "structIotBleWifiProvResponse__t.html#a1f8bbd0d5208481f30d5fd64d98c9cdd", null ],
      [ "status", "structIotBleWifiProvResponse__t.html#a6484494303c3b64685b440d58d8e8981", null ],
      [ "networkInfo", "structIotBleWifiProvResponse__t.html#addd11bf4a1eb642379ed2b1b6d87c278", null ],
      [ "statusOnly", "structIotBleWifiProvResponse__t.html#ae69b87a3a5667203eb29494b93fd2216", null ]
    ] ]
];