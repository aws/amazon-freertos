var group__ble__datatypes__structs =
[
    [ "IotBleAdvertisementParams_t", "struct_iot_ble_advertisement_params__t.html", [
      [ "appearance", "struct_iot_ble_advertisement_params__t.html#af2c2b304dfaa85ee4178a6d9f296b98d", null ],
      [ "minInterval", "struct_iot_ble_advertisement_params__t.html#a52c31a5243a0d16599e1972ea0bd9bfe", null ],
      [ "maxInterval", "struct_iot_ble_advertisement_params__t.html#a418538a4d0f4716b887e2e4bc4949b0d", null ],
      [ "pManufacturerData", "struct_iot_ble_advertisement_params__t.html#a9f43ca0255fd1e0ca92c91b3e7cd18db", null ],
      [ "pServiceData", "struct_iot_ble_advertisement_params__t.html#a2e207496dc0ed5fd3fc10012db17b84c", null ],
      [ "pUUID1", "struct_iot_ble_advertisement_params__t.html#a576f267fcacad38de0ca49d2efb88e12", null ],
      [ "pUUID2", "struct_iot_ble_advertisement_params__t.html#ad8026cb77eceb8696900e94068b46930", null ],
      [ "manufacturerLen", "struct_iot_ble_advertisement_params__t.html#a63700e2aba9ad874f66141920e638695", null ],
      [ "serviceDataLen", "struct_iot_ble_advertisement_params__t.html#aee966567b6e769c7c5227644d8c0c161", null ],
      [ "includeTxPower", "struct_iot_ble_advertisement_params__t.html#aa002e1df4d792962c497f51b8ba09fcc", null ],
      [ "name", "struct_iot_ble_advertisement_params__t.html#a61420f4960f7f95ca5583242b19fcb40", null ],
      [ "setScanRsp", "struct_iot_ble_advertisement_params__t.html#ac87e0e1172432dae910bba35a971cc86", null ]
    ] ],
    [ "IotBleConnectionParam_t", "struct_iot_ble_connection_param__t.html", [
      [ "minInterval", "struct_iot_ble_connection_param__t.html#acfaf698d7274f5a9868a859cc85c339a", null ],
      [ "maxInterval", "struct_iot_ble_connection_param__t.html#a34aa00be9f23aeddaee2df4452cbf47f", null ],
      [ "latency", "struct_iot_ble_connection_param__t.html#a65fdeb5d8f8936254af7de21270d0da2", null ],
      [ "timeout", "struct_iot_ble_connection_param__t.html#aa7fc274041d12a8da764f31df5fe2312", null ]
    ] ],
    [ "IotBleConnectionInfoListElement_t", "struct_iot_ble_connection_info_list_element__t.html", [
      [ "connectionList", "struct_iot_ble_connection_info_list_element__t.html#a2aa514fc93a437b163eb3de3aed51f7d", null ],
      [ "connectionParams", "struct_iot_ble_connection_info_list_element__t.html#a1696e126dfd03261814bac9da937597b", null ],
      [ "remoteBdAddr", "struct_iot_ble_connection_info_list_element__t.html#a84d19e76d732e3ab3bcdba21d9dbd34e", null ],
      [ "securityLevel", "struct_iot_ble_connection_info_list_element__t.html#a9bc19107a7927f8d3c5b0d2ff9bca083", null ],
      [ "clientCharDescrListHead", "struct_iot_ble_connection_info_list_element__t.html#a06544c5d16270eb20edaa6c82df56ec6", null ],
      [ "connId", "struct_iot_ble_connection_info_list_element__t.html#aa5f7b70790a024a5fe50986dd4da5167", null ],
      [ "isBonded", "struct_iot_ble_connection_info_list_element__t.html#a83efe4b777b0f0335cea8d3260a4d48b", null ]
    ] ],
    [ "IotBleReadEventParams_t", "struct_iot_ble_read_event_params__t.html", [
      [ "connId", "struct_iot_ble_read_event_params__t.html#a676480b3d0714a6523ef2f6b462ad100", null ],
      [ "transId", "struct_iot_ble_read_event_params__t.html#a757b1a25aaac8450b18d8a38025f5c55", null ],
      [ "pRemoteBdAddr", "struct_iot_ble_read_event_params__t.html#aa43998fb9ad5b52de6999b81566c1c1c", null ],
      [ "attrHandle", "struct_iot_ble_read_event_params__t.html#a0ad867195c80665f6044f6ccbd97f3b6", null ],
      [ "offset", "struct_iot_ble_read_event_params__t.html#a5ec6dc40b7f36b1157bc4495f621c836", null ]
    ] ],
    [ "IotBleWriteEventParams_t", "struct_iot_ble_write_event_params__t.html", [
      [ "transId", "struct_iot_ble_write_event_params__t.html#a85af199e536d8d3d146646b34a0b44d1", null ],
      [ "pRemoteBdAddr", "struct_iot_ble_write_event_params__t.html#ae87b754704ca4d8d3ed36754c59e6ccd", null ],
      [ "pValue", "struct_iot_ble_write_event_params__t.html#a10a31f9bce025cafb8757e37bbe3ee18", null ],
      [ "length", "struct_iot_ble_write_event_params__t.html#ab6aea1925055f9ec6851823481839ebd", null ],
      [ "connId", "struct_iot_ble_write_event_params__t.html#a24b14e2adf64d5b19a1c673d16133071", null ],
      [ "attrHandle", "struct_iot_ble_write_event_params__t.html#a21464a99a4e888983a53ccd70ef92ae1", null ],
      [ "offset", "struct_iot_ble_write_event_params__t.html#a8cd2a17851b785272f0a4f79ade75c4a", null ],
      [ "needRsp", "struct_iot_ble_write_event_params__t.html#a1956f3f8301cb64a0b3dc07102a194b2", null ],
      [ "isPrep", "struct_iot_ble_write_event_params__t.html#a9564b04190b11a43c0e3e6e4762312dc", null ]
    ] ],
    [ "IotBleExecWriteEventParams_t", "struct_iot_ble_exec_write_event_params__t.html", [
      [ "transId", "struct_iot_ble_exec_write_event_params__t.html#a4a5481463cd72677a29bdb10257dbbf8", null ],
      [ "pRemoteBdAddr", "struct_iot_ble_exec_write_event_params__t.html#a16d9e31a657d8af854a17e30ca0eab94", null ],
      [ "connId", "struct_iot_ble_exec_write_event_params__t.html#a02216a1366225590c5d053c313cc0887", null ],
      [ "execWrite", "struct_iot_ble_exec_write_event_params__t.html#aa827e03b3ba7ed318fbe4efc56f4c46c", null ]
    ] ],
    [ "IotBleRespConfirmEventParams_t", "struct_iot_ble_resp_confirm_event_params__t.html", [
      [ "status", "struct_iot_ble_resp_confirm_event_params__t.html#a851536e1dacc9fb72bdee6b44b7279a2", null ],
      [ "handle", "struct_iot_ble_resp_confirm_event_params__t.html#a549e3a9ae98e7c2d3ca3b703b4bcf77f", null ]
    ] ],
    [ "IotBleIndicationSentEventParams_t", "struct_iot_ble_indication_sent_event_params__t.html", [
      [ "pAttribute", "struct_iot_ble_indication_sent_event_params__t.html#ac812d1e73ea99c389a8b1138f3b626b3", null ],
      [ "connId", "struct_iot_ble_indication_sent_event_params__t.html#a90b85ff050b2d272c5ee00d353bd4f68", null ],
      [ "status", "struct_iot_ble_indication_sent_event_params__t.html#aa8c003ab7617649167f9d510bf00f055", null ]
    ] ],
    [ "IotBleAttributeEvent_t", "struct_iot_ble_attribute_event.html", [
      [ "pParamRead", "struct_iot_ble_attribute_event.html#af000cffda3324dff9fb003e3ea5069e2", null ],
      [ "pParamWrite", "struct_iot_ble_attribute_event.html#a85a0549443858500f6ee232817499b76", null ],
      [ "pParamExecWrite", "struct_iot_ble_attribute_event.html#a89a133e4dd68ab388b7047813950f50d", null ],
      [ "pParamRespConfirm", "struct_iot_ble_attribute_event.html#a51476d61347df02cf125b0a8271e6632", null ],
      [ "pParamIndicationSent", "struct_iot_ble_attribute_event.html#a8a3277d571f30f3ab4005c0239ae33b5", null ],
      [ "xEventType", "struct_iot_ble_attribute_event.html#a6a6ba8826b0a11003de39d74fdceb124", null ]
    ] ],
    [ "IotBleAttributeData_t", "struct_iot_ble_attribute_data__t.html", [
      [ "size", "struct_iot_ble_attribute_data__t.html#afd86c0eed1e62534c14bc76c5ead332e", null ],
      [ "pData", "struct_iot_ble_attribute_data__t.html#aedfed227d6f8416925e5164e27ff4e1c", null ],
      [ "uuid", "struct_iot_ble_attribute_data__t.html#a66926c57bfc3ff0fe9ee8d25d0671eaa", null ],
      [ "handle", "struct_iot_ble_attribute_data__t.html#ac0823b7a61d73b561d8369654eb9b149", null ]
    ] ],
    [ "IotBleEventResponse_t", "struct_iot_ble_event_response__t.html", [
      [ "pAttrData", "struct_iot_ble_event_response__t.html#a9e8dd37339ec95aa8958a01a251e55b2", null ],
      [ "attrDataOffset", "struct_iot_ble_event_response__t.html#a64326656938aa31287fc2533d6aee973", null ],
      [ "eventStatus", "struct_iot_ble_event_response__t.html#a97a079474634f6fc46eb6e954eb3ef00", null ],
      [ "rspErrorStatus", "struct_iot_ble_event_response__t.html#a59be899db102159383553990f0b9b847", null ]
    ] ],
    [ "IotBleEventsCallbacks_t", "union_iot_ble_events_callbacks__t.html", [
      [ "pMtuChangedCb", "union_iot_ble_events_callbacks__t.html#a082a8b2b7b7ba0de0ae6970bcdee76fc", null ],
      [ "pConnectionCb", "union_iot_ble_events_callbacks__t.html#a4d7eb5b979658273711128fb9dd01f8f", null ],
      [ "pGAPPairingStateChangedCb", "union_iot_ble_events_callbacks__t.html#a1408546ac060ac04be4bc57e2e767fc7", null ],
      [ "pConnParameterUpdateRequestCb", "union_iot_ble_events_callbacks__t.html#aeee7e91eaf75ab2954920e393d1d6357", null ],
      [ "pNumericComparisonCb", "union_iot_ble_events_callbacks__t.html#a69ae6c4b48f24cec4407f8a96f3c9a6b", null ],
      [ "pvPtr", "union_iot_ble_events_callbacks__t.html#ac3481c0bb7d4a59eab511618323aa638", null ]
    ] ],
    [ "IotBleListNetworkRequest_t", "struct_iot_ble_list_network_request__t.html", [
      [ "maxNetworks", "struct_iot_ble_list_network_request__t.html#ab569bac0526f308805f5063d8f056b40", null ],
      [ "timeoutMs", "struct_iot_ble_list_network_request__t.html#acc14419712e27e459d3a7a265fed4935", null ]
    ] ],
    [ "IotBleAddNetworkRequest_t", "struct_iot_ble_add_network_request__t.html", [
      [ "network", "struct_iot_ble_add_network_request__t.html#a1120eef64bd422cd92ab1bbad30caeea", null ],
      [ "savedIdx", "struct_iot_ble_add_network_request__t.html#af01e4dc626debcea29b9ffec606bc999", null ],
      [ "connect", "struct_iot_ble_add_network_request__t.html#ad10840286b0f3256886e98fae2f0e241", null ]
    ] ],
    [ "IotBleEditNetworkRequest_t", "struct_iot_ble_edit_network_request__t.html", [
      [ "curIdx", "struct_iot_ble_edit_network_request__t.html#af1de7515c63bd561974227b58f05c059", null ],
      [ "newIdx", "struct_iot_ble_edit_network_request__t.html#a4f67708ea6c3a968c07f7d6d28af994d", null ]
    ] ],
    [ "IotBleDeleteNetworkRequest_t", "struct_iot_ble_delete_network_request__t.html", [
      [ "idx", "struct_iot_ble_delete_network_request__t.html#ab4d23a6ccd1b72d318730c8bc128b05d", null ]
    ] ],
    [ "IotBleWifiNetworkInfo_t", "struct_iot_ble_wifi_network_info__t.html", [
      [ "pSSID", "struct_iot_ble_wifi_network_info__t.html#a8c986732bac3a1498f17fad334b3c47d", null ],
      [ "SSIDLength", "struct_iot_ble_wifi_network_info__t.html#a62e4ee1bb5eea98c91c45a00aae8e349", null ],
      [ "pBSSID", "struct_iot_ble_wifi_network_info__t.html#a5f9b008c8a5e3fe9a43d0fd06579c2eb", null ],
      [ "BSSIDLength", "struct_iot_ble_wifi_network_info__t.html#acfc3878ef57374c79149436676a7b5cc", null ],
      [ "savedIdx", "struct_iot_ble_wifi_network_info__t.html#a472c86dbfc4d0b404a015dd1a5f0e0df", null ],
      [ "RSSI", "struct_iot_ble_wifi_network_info__t.html#a60e21aa748fdf01533b2987f25cfa134", null ],
      [ "security", "struct_iot_ble_wifi_network_info__t.html#a09b57be99fb597c5bc8e038abc5b43d1", null ],
      [ "status", "struct_iot_ble_wifi_network_info__t.html#a6a754cf698e4880ef9205fe2a698d6b8", null ],
      [ "hidden", "struct_iot_ble_wifi_network_info__t.html#a2f75cfe908003e18af17f7d4c03cf179", null ],
      [ "connected", "struct_iot_ble_wifi_network_info__t.html#a1d8ddab827463f6fa3046b8da898ae0e", null ]
    ] ],
    [ "IotBleWifiProvService_t", "struct_iot_ble_wifi_prov_service__t.html", [
      [ "pChannel", "struct_iot_ble_wifi_prov_service__t.html#a7976c6b9ee6cd99914c61ccee51400bf", null ],
      [ "lock", "struct_iot_ble_wifi_prov_service__t.html#aa54ad1944f413e1d10659e01bfe2d9fa", null ],
      [ "numNetworks", "struct_iot_ble_wifi_prov_service__t.html#a85db601e762484e693bdccfd8b956a26", null ],
      [ "connectedIdx", "struct_iot_ble_wifi_prov_service__t.html#a6e6d30e48724227212c7bf0bd0f71b30", null ],
      [ "listNetworkRequest", "struct_iot_ble_wifi_prov_service__t.html#aaa445ca9d41d93adbd853413028c4e2d", null ],
      [ "addNetworkRequest", "struct_iot_ble_wifi_prov_service__t.html#a2d6e6b0dd04d48fc81d3530fb6c591c2", null ],
      [ "editNetworkRequest", "struct_iot_ble_wifi_prov_service__t.html#aaeab936fd7bfa8d160d8d52e506931d6", null ],
      [ "deleteNetworkRequest", "struct_iot_ble_wifi_prov_service__t.html#a7da0bee75cf7b5d49c0f9d93f5eeef64", null ]
    ] ]
];