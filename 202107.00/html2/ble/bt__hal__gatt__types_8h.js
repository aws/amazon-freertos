var bt__hal__gatt__types_8h =
[
    [ "btGATT_MAX_ATTR_LEN", "group__HAL__BLUETOOTH.html#ga1a7d653463b2a9fcf562fa114adaa5b6", null ],
    [ "BTCharProperties_t", "group__HAL__BLUETOOTH.html#gad080c49c32176711baa18602b6a12770", null ],
    [ "BTCharPermissions_t", "group__HAL__BLUETOOTH.html#gaf18b3f17c8aa58ef8473f49ac2fb7ddf", null ],
    [ "BTServiceUUID_t", "group__HAL__BLUETOOTH.html#ga8b891d4e44836fd951ad1f13c42a0b3b", null ],
    [ "BTConnectCallback_t", "group__HAL__BLUETOOTH.html#ga4f58f8dad2631d86837eaf216cf833fd", null ],
    [ "BTDisconnectCallback_t", "group__HAL__BLUETOOTH.html#ga427a82ffed8a66782d795cbe1f9a071c", null ],
    [ "BTReadRemoteRssiCallback_t", "group__HAL__BLUETOOTH.html#gaea1973e50f9855ce5dbdf526b73b1ac7", null ],
    [ "BTCongestionCallback_t", "group__HAL__BLUETOOTH.html#ga016f1b8e1953b8f3a98a4aff9425071a", null ],
    [ "BTGattStatus_t", "group__HAL__BLUETOOTH.html#gae3f7d1f4e7d4188a250e314475352374", [
      [ "eBTGattStatusSuccess", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a51060d4d256106d510420d10634566fc", null ],
      [ "eBTInvalidHandle", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a921c38dfe11e27b0e2e5899349741261", null ],
      [ "eBTGattStatusReadNotPermitted", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a83c88bde61b69ec7e03d64a122ed7b01", null ],
      [ "eBTGattStatusWriteNotPermitted", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a53fd128e06da4e14c525b9283a9f615c", null ],
      [ "eBTGattStatusInvalidPDU", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374adcd0c29a4f7322ec61aca670dafdffbe", null ],
      [ "eBTGattStatusInsufficientAuthentication", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a78782eeb9d080bab86409bdac2a108bd", null ],
      [ "eBTGattStatusRequestNotSupported", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a69162ea5a53e48acc2b785fad12c37da", null ],
      [ "eBTGattStatusInvalidOffset", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a05db7b9ff1ca372c0d97361e10c6cab1", null ],
      [ "eBTGattStatusInsufficientAuthorization", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a5bbd004757d6efa11f3dfcf144d97dd4", null ],
      [ "eBTGattStatusPrepareQueueFull", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a39a3aceaa8def06ba89d2d605128ad75", null ],
      [ "eBTGattStatusAttributeNotFound", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a4e0f2f9562afaa512853aa3a2eff9fa8", null ],
      [ "eBTGattStatusAttributeNotLong", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a577cffc6cc5f43e5ee6849fa80dedf01", null ],
      [ "eBTGattStatusInsufficientKeySize", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a7091c30daa7c60ce08c7423d9af14880", null ],
      [ "eBTGattStatusInvalidAttributeLength", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a14358310b49ef4c2a0488b79e0106109", null ],
      [ "eBTGattStatusUnlikelyError", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a7fe6ea959fbed7534518eb20799bd5b1", null ],
      [ "eBTGattStatusInsufficientEncryption", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a21487939009d47907b56580df40c96c7", null ],
      [ "eBTGattStatusUnsupportedGroupType", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374afc0cd7f169a499a61f979cc2674dcce5", null ],
      [ "eBTGattStatusInsufficientResources", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a0c4bb7143ce3bb311b768e8b86dd8c95", null ],
      [ "eBTGattStatusInternalError", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a1109b99f8c87820bf0026805505d723f", null ],
      [ "eBTGattStatusError", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374ac795b7646783fb11796a24bc25669995", null ],
      [ "eBTGattStatusConnectionCongested", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a068e4573dd3f1a26be06e5c4cb7318c3", null ],
      [ "eBTGattStatusErrorConnEstFail", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374af3a559d6b08c51b05e248f9c3e7a7eb9", null ],
      [ "eBTGattStatusErrorConnTimeout", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a3a828672a83ab5c19d25779136dd33a3", null ],
      [ "eBTGattStatusLocalHostTerminatedConnection", "group__HAL__BLUETOOTH.html#ggae3f7d1f4e7d4188a250e314475352374a9d51a7c30364f875aa103dab65e492a1", null ]
    ] ],
    [ "eBTPropNone", "group__HAL__BLUETOOTH.html#ggadc29c2ff13d900c2f185ee95427fb06ca0b941ee0ac3984df6492143801218cc3", null ],
    [ "eBTPropBroadcast", "group__HAL__BLUETOOTH.html#ggadc29c2ff13d900c2f185ee95427fb06ca29a0720371761dd2d67840af141f19ff", null ],
    [ "eBTPropRead", "group__HAL__BLUETOOTH.html#ggadc29c2ff13d900c2f185ee95427fb06ca72e8b811d484abad45ff09565eb069b7", null ],
    [ "eBTPropWriteNoResponse", "group__HAL__BLUETOOTH.html#ggadc29c2ff13d900c2f185ee95427fb06ca413ee993a2a181c47f72231a64c36c2f", null ],
    [ "eBTPropWrite", "group__HAL__BLUETOOTH.html#ggadc29c2ff13d900c2f185ee95427fb06ca64f3a5dd3d4ad3f1453f51ba643ccdf3", null ],
    [ "eBTPropNotify", "group__HAL__BLUETOOTH.html#ggadc29c2ff13d900c2f185ee95427fb06caa006c406b90f3874876d466c19e10d55", null ],
    [ "eBTPropIndicate", "group__HAL__BLUETOOTH.html#ggadc29c2ff13d900c2f185ee95427fb06cae11a365d3e31c99912d1d54674123447", null ],
    [ "eBTPropSignedWrite", "group__HAL__BLUETOOTH.html#ggadc29c2ff13d900c2f185ee95427fb06cad7e23b5598d02a672996fa25fc8c3841", null ],
    [ "eBTPropExtendedProps", "group__HAL__BLUETOOTH.html#ggadc29c2ff13d900c2f185ee95427fb06ca519137ff87f85f90c95e811f89800f7f", null ],
    [ "eBTPermNone", "group__HAL__BLUETOOTH.html#gga61dadd085c1777f559549e05962b2c9ea06d18f069b647d1ae80cc4bb3fb451c4", null ],
    [ "eBTPermRead", "group__HAL__BLUETOOTH.html#gga61dadd085c1777f559549e05962b2c9ea8325a48a4a20814e53afb40db5f4ffea", null ],
    [ "eBTPermReadEncrypted", "group__HAL__BLUETOOTH.html#gga61dadd085c1777f559549e05962b2c9ea5785a0ecec6d54319e016f5f0bea0f6e", null ],
    [ "eBTPermReadEncryptedMitm", "group__HAL__BLUETOOTH.html#gga61dadd085c1777f559549e05962b2c9ea4a2db8c46dcf6ce637a104ed992d73bf", null ],
    [ "eBTPermWrite", "group__HAL__BLUETOOTH.html#gga61dadd085c1777f559549e05962b2c9ea3cfa3940a8668127035c18c4ba7c613d", null ],
    [ "eBTPermWriteEncrypted", "group__HAL__BLUETOOTH.html#gga61dadd085c1777f559549e05962b2c9ea8c0ffeb6004b07ba2a51be5196286fe1", null ],
    [ "eBTPermWriteEncryptedMitm", "group__HAL__BLUETOOTH.html#gga61dadd085c1777f559549e05962b2c9ea2ea723131d60886fe6fe7d3d1a3b2ed9", null ],
    [ "eBTPermWriteSigned", "group__HAL__BLUETOOTH.html#gga61dadd085c1777f559549e05962b2c9eaa970663c467e491b324a4c7cb3079c64", null ],
    [ "eBTPermWriteSignedMitm", "group__HAL__BLUETOOTH.html#gga61dadd085c1777f559549e05962b2c9eacdf62b170fa40bf1bcfc6053d1fa7369", null ],
    [ "BTAttrWriteRequests_t", "group__HAL__BLUETOOTH.html#ga55c1e7ffff1cf5ea0164fa8e6795a0ce", [
      [ "eBTWriteTypeNoResponse", "group__HAL__BLUETOOTH.html#gga55c1e7ffff1cf5ea0164fa8e6795a0cea9681e4b581a7f800d3146cb7de0ea12a", null ],
      [ "eBTWriteTypeDefault", "group__HAL__BLUETOOTH.html#gga55c1e7ffff1cf5ea0164fa8e6795a0cea1540792ce245577295d72ca319baf92a", null ],
      [ "eBTWriteTypePrepare", "group__HAL__BLUETOOTH.html#gga55c1e7ffff1cf5ea0164fa8e6795a0ceacb8341dddbbf007ac7221d10dd112596", null ],
      [ "eBTWriteTypeSigned", "group__HAL__BLUETOOTH.html#gga55c1e7ffff1cf5ea0164fa8e6795a0ceaf4a0ec416dcea8d1591b85ee8df9e5b8", null ]
    ] ],
    [ "BTGattServiceTypes_t", "group__HAL__BLUETOOTH.html#ga42d9ff4543b70f554131656403ce0dde", [
      [ "eBTServiceTypePrimary", "group__HAL__BLUETOOTH.html#gga42d9ff4543b70f554131656403ce0ddeac4af7bd10fe3fe69564551de8bbe2804", null ],
      [ "eBTServiceTypeSecondary", "group__HAL__BLUETOOTH.html#gga42d9ff4543b70f554131656403ce0ddea2887a50ea7fdde925d35c3041f148fe3", null ]
    ] ],
    [ "BTDbAttributeType_t", "group__HAL__BLUETOOTH.html#gae3c29649b63768bc288904e3f4202744", [
      [ "eBTDbPrimaryService", "group__HAL__BLUETOOTH.html#ggae3c29649b63768bc288904e3f4202744a4d3ebaa6953a2f4b22abfb3b4688a525", null ],
      [ "eBTDbSecondaryService", "group__HAL__BLUETOOTH.html#ggae3c29649b63768bc288904e3f4202744aa6d9b379f74e9703140e3d9ffe4bfedf", null ],
      [ "eBTDbIncludedService", "group__HAL__BLUETOOTH.html#ggae3c29649b63768bc288904e3f4202744aa0126b319c857ea5f0f5811cb6787f49", null ],
      [ "eBTDbCharacteristicDecl", "group__HAL__BLUETOOTH.html#ggae3c29649b63768bc288904e3f4202744a6cb593924840487c6e1e0304adb2c07c", null ],
      [ "eBTDbCharacteristic", "group__HAL__BLUETOOTH.html#ggae3c29649b63768bc288904e3f4202744a13d4e7209ea43c7ad5d50410b393404d", null ],
      [ "eBTDbDescriptor", "group__HAL__BLUETOOTH.html#ggae3c29649b63768bc288904e3f4202744ace647fcd790e4d26626f62e4baf310ae", null ]
    ] ]
];