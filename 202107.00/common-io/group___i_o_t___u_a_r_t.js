var group___i_o_t___u_a_r_t =
[
    [ "IotUARTConfig_t", "struct_iot_u_a_r_t_config__t.html", [
      [ "ulBaudrate", "struct_iot_u_a_r_t_config__t.html#a322fff59e4b347ddfda22e68884516f9", null ],
      [ "xParity", "struct_iot_u_a_r_t_config__t.html#abc590dcec8c07ca6ee851c57fe6f0e6f", null ],
      [ "xStopbits", "struct_iot_u_a_r_t_config__t.html#a0086a8255eda45f1910a0b15d775ea36", null ],
      [ "ucWordlength", "struct_iot_u_a_r_t_config__t.html#ab28bb961f2983bd7d36cb073489a4116", null ],
      [ "ucFlowControl", "struct_iot_u_a_r_t_config__t.html#ab8652644cfb682a5210655c1707f029a", null ]
    ] ],
    [ "IOT_UART_BAUD_RATE_DEFAULT", "group___i_o_t___u_a_r_t.html#ga118d2e35b63def19ed9a3978c3129dd2", null ],
    [ "IOT_UART_SUCCESS", "group___i_o_t___u_a_r_t.html#ga53ea0768c3a6aa784490aaf117118471", null ],
    [ "IOT_UART_INVALID_VALUE", "group___i_o_t___u_a_r_t.html#gaeed523cc0e9e837c0dd9fb40832a9b7b", null ],
    [ "IOT_UART_WRITE_FAILED", "group___i_o_t___u_a_r_t.html#ga7aa4e1f241385530d9474d3ab17f62e3", null ],
    [ "IOT_UART_READ_FAILED", "group___i_o_t___u_a_r_t.html#ga949a6ddebbdcb11fa000b754a029fe93", null ],
    [ "IOT_UART_BUSY", "group___i_o_t___u_a_r_t.html#gae49703ad4014576ce59505560fb1c836", null ],
    [ "IOT_UART_NOTHING_TO_CANCEL", "group___i_o_t___u_a_r_t.html#ga7d7c215f920ce006c49e2140d953eb37", null ],
    [ "IOT_UART_FUNCTION_NOT_SUPPORTED", "group___i_o_t___u_a_r_t.html#gaaf0155ab88aad5a4c1fb5668b02e8dc2", null ],
    [ "IotUARTCallback_t", "group___i_o_t___u_a_r_t.html#gab4f45c3cd119723cc23fc621deacfa26", null ],
    [ "IotUARTHandle_t", "group___i_o_t___u_a_r_t.html#ga9ccfe155fcf5c7ae44b6956d1c6963b4", null ],
    [ "IotUARTOperationStatus_t", "group___i_o_t___u_a_r_t.html#ga2e6b64e194639b86e2ccb06faf53054c", [
      [ "eUartCompleted", "group___i_o_t___u_a_r_t.html#gga2e6b64e194639b86e2ccb06faf53054ca6846be1e11c014e73befe8205e4e40bb", null ],
      [ "eUartLastWriteFailed", "group___i_o_t___u_a_r_t.html#gga2e6b64e194639b86e2ccb06faf53054ca312f862e4527c32e66e972529bafdae8", null ],
      [ "eUartLastReadFailed", "group___i_o_t___u_a_r_t.html#gga2e6b64e194639b86e2ccb06faf53054ca1f994469aec2e0d325b61b6705033b2b", null ]
    ] ],
    [ "IotUARTParity_t", "group___i_o_t___u_a_r_t.html#gacd2527ef08ac87f7b4beccce9d76bf36", [
      [ "eUartParityNone", "group___i_o_t___u_a_r_t.html#ggacd2527ef08ac87f7b4beccce9d76bf36ab775b5a669654de41fb3a0d29262f109", null ],
      [ "eUartParityOdd", "group___i_o_t___u_a_r_t.html#ggacd2527ef08ac87f7b4beccce9d76bf36ac311c14849777d24950fa3e4227d460b", null ],
      [ "eUartParityEven", "group___i_o_t___u_a_r_t.html#ggacd2527ef08ac87f7b4beccce9d76bf36a6217d48351fc7c0340c77a7e362d990a", null ]
    ] ],
    [ "IotUARTStopBits_t", "group___i_o_t___u_a_r_t.html#ga6219589764722b2414fdc326b2303748", [
      [ "eUartStopBitsOne", "group___i_o_t___u_a_r_t.html#gga6219589764722b2414fdc326b2303748ac9b870e5897bed379b900e6da9033701", null ],
      [ "eUartStopBitsTwo", "group___i_o_t___u_a_r_t.html#gga6219589764722b2414fdc326b2303748a167d08edaf63925c1d10096dd7ece6c0", null ]
    ] ],
    [ "IotUARTIoctlRequest_t", "group___i_o_t___u_a_r_t.html#ga5b317dec5ae5243cae470e5937cca8d4", [
      [ "eUartGetConfig", "group___i_o_t___u_a_r_t.html#gga5b317dec5ae5243cae470e5937cca8d4a235ccec5622b37c722f3b0986f7b3361", null ],
      [ "eGetTxNoOfbytes", "group___i_o_t___u_a_r_t.html#gga5b317dec5ae5243cae470e5937cca8d4a9b31bf166ade81ce5e1b8e9c13e82380", null ],
      [ "eGetRxNoOfbytes", "group___i_o_t___u_a_r_t.html#gga5b317dec5ae5243cae470e5937cca8d4a1ec6346ff4cfdd0022a474323c36c0ff", null ]
    ] ],
    [ "iot_uart_open", "group___i_o_t___u_a_r_t.html#ga32ac04310af91b401f583e1440bea904", null ],
    [ "iot_uart_set_callback", "group___i_o_t___u_a_r_t.html#gad38a1e10c4f2fdd516eca43ebd8e1a12", null ],
    [ "iot_uart_read_sync", "group___i_o_t___u_a_r_t.html#ga07377025aa5e05cea4872863a254ea6f", null ],
    [ "iot_uart_write_sync", "group___i_o_t___u_a_r_t.html#gab0d16e738164fbc77527ef6545ebf163", null ],
    [ "iot_uart_read_async", "group___i_o_t___u_a_r_t.html#ga41d3f03f5474aa51d284dc4aefc2da96", null ],
    [ "iot_uart_write_async", "group___i_o_t___u_a_r_t.html#gaaa8b837be623972c4fbd67abd6d203d4", null ],
    [ "iot_uart_ioctl", "group___i_o_t___u_a_r_t.html#gaeea1568e527ac8db1b9ce6a175131228", null ],
    [ "iot_uart_cancel", "group___i_o_t___u_a_r_t.html#ga878adb5a8035d55a51d350f7f8ea30bc", null ],
    [ "iot_uart_close", "group___i_o_t___u_a_r_t.html#gaf5ce6c614325fa1bae6004bfcdafeafd", null ]
];