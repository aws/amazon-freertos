var group__iot__i2c =
[
    [ "IotI2CConfig_t", "struct_iot_i2_c_config__t.html", [
      [ "ulMasterTimeout", "struct_iot_i2_c_config__t.html#a9aa01c46973c5e82d6bd7ae88e5c9a70", null ],
      [ "ulBusFreq", "struct_iot_i2_c_config__t.html#adfff942ebb9ecbb75cac57ef2c2c710d", null ]
    ] ],
    [ "IOT_I2C_STANDARD_MODE_BPS", "group__iot__i2c.html#ga39dedd82c859dd488c36bc5e3a08bfa3", null ],
    [ "IOT_I2C_FAST_MODE_BPS", "group__iot__i2c.html#ga66720ecc3ff5a93ac9d7cfa7f13de54c", null ],
    [ "IOT_I2C_FAST_MODE_PLUS_BPS", "group__iot__i2c.html#ga6197dc227a3c6df7122de50a8ee8550e", null ],
    [ "IOT_I2C_HIGH_SPEED_BPS", "group__iot__i2c.html#ga04adf3426f58755529fd9346b3d7f2ec", null ],
    [ "IOT_I2C_SUCCESS", "group__iot__i2c.html#ga9f948585154dcb4c56ee3fe2de98b65f", null ],
    [ "IOT_I2C_INVALID_VALUE", "group__iot__i2c.html#ga7c3d32e22885563ee966d31d22a7a533", null ],
    [ "IOT_I2C_BUSY", "group__iot__i2c.html#ga8248dfa6b9f354c200b4306105113f99", null ],
    [ "IOT_I2C_WRITE_FAILED", "group__iot__i2c.html#ga6fcd783d68a244d1f48937e6d8c0bed7", null ],
    [ "IOT_I2C_READ_FAILED", "group__iot__i2c.html#gae26399840bffad58422bc26c1718e293", null ],
    [ "IOT_I2C_NACK", "group__iot__i2c.html#gaa0dd2eb48b5a66eb69d3537f358134f6", null ],
    [ "IOT_I2C_BUS_TIMEOUT", "group__iot__i2c.html#ga4d147622319b047effeefebbe0b86cb3", null ],
    [ "IOT_I2C_NOTHING_TO_CANCEL", "group__iot__i2c.html#gadc577b4e35153b903f04d7b0b6cf225e", null ],
    [ "IOT_I2C_FUNCTION_NOT_SUPPORTED", "group__iot__i2c.html#gaa4372dada4f0dd84f288a52df0b3a49e", null ],
    [ "IOT_I2C_SLAVE_ADDRESS_NOT_SET", "group__iot__i2c.html#ga23288108c96fad5d4f047c9a59d969d8", null ],
    [ "IotI2CHandle_t", "group__iot__i2c.html#ga6b10b8bf106ac971adacb0ba2d55ce79", null ],
    [ "IotI2CCallback_t", "group__iot__i2c.html#ga9cfbdc6f340ac300bd82f5bbe37c1930", null ],
    [ "IotI2CBusStatus_t", "group__iot__i2c.html#gacc842976e064b31f5525816ce492df4b", [
      [ "eI2CBusIdle", "group__iot__i2c.html#ggacc842976e064b31f5525816ce492df4ba675f57e9e0a861ab91b25bf6edc1cf12", null ],
      [ "eI2cBusBusy", "group__iot__i2c.html#ggacc842976e064b31f5525816ce492df4ba952c730267c3789ef5e53163dde290da", null ]
    ] ],
    [ "IotI2COperationStatus_t", "group__iot__i2c.html#gaaa0d0c7fc5d58748f18f06ac6c206188", [
      [ "eI2CCompleted", "group__iot__i2c.html#ggaaa0d0c7fc5d58748f18f06ac6c206188a89e6508639a38834cb5f865aefe13d7d", null ],
      [ "eI2CDriverFailed", "group__iot__i2c.html#ggaaa0d0c7fc5d58748f18f06ac6c206188a85f05ef36457d23a97434611891dfa5d", null ],
      [ "eI2CNackFromSlave", "group__iot__i2c.html#ggaaa0d0c7fc5d58748f18f06ac6c206188ab454d36ee9565b25abfdd853d04a59d3", null ],
      [ "eI2CMasterTimeout", "group__iot__i2c.html#ggaaa0d0c7fc5d58748f18f06ac6c206188ab0002506f3d88f5f775118ed798d9372", null ]
    ] ],
    [ "IotI2CIoctlRequest_t", "group__iot__i2c.html#gaf6c106295bcbda3469e1e4d792a6075d", [
      [ "eI2CSendNoStopFlag", "group__iot__i2c.html#ggaf6c106295bcbda3469e1e4d792a6075da8f87883392ee2b403a16b5390e89b4c6", null ],
      [ "eI2CSetSlaveAddr", "group__iot__i2c.html#ggaf6c106295bcbda3469e1e4d792a6075da1e1afe232ebb3c786387f6fb4fcb9f23", null ],
      [ "eI2CSetMasterConfig", "group__iot__i2c.html#ggaf6c106295bcbda3469e1e4d792a6075da296485174fb38af72af642364c366d82", null ],
      [ "eI2CGetMasterConfig", "group__iot__i2c.html#ggaf6c106295bcbda3469e1e4d792a6075daa38fd06bf7ad043a8130d43ae1178d60", null ],
      [ "eI2CGetBusState", "group__iot__i2c.html#ggaf6c106295bcbda3469e1e4d792a6075da6a83e28f0cdf3e0146b1157ebf978133", null ],
      [ "eI2CBusReset", "group__iot__i2c.html#ggaf6c106295bcbda3469e1e4d792a6075da7337865bb3b2c53874446f372da02312", null ],
      [ "eI2CGetTxNoOfbytes", "group__iot__i2c.html#ggaf6c106295bcbda3469e1e4d792a6075da12b87e84b9702f09651e63ef77c8bf94", null ],
      [ "eI2CGetRxNoOfbytes", "group__iot__i2c.html#ggaf6c106295bcbda3469e1e4d792a6075daf8542407b24b649e862359ad11ca9abb", null ]
    ] ],
    [ "iot_i2c_open", "group__iot__i2c.html#gaf8f5f0ebea988cc69c1a475bd9de495c", null ],
    [ "iot_i2c_set_callback", "group__iot__i2c.html#ga5e3b3385a789286583cdb6b3e403f43e", null ],
    [ "iot_i2c_read_sync", "group__iot__i2c.html#gad80a773727ade2369c1c0f1d529ba280", null ],
    [ "iot_i2c_write_sync", "group__iot__i2c.html#ga3b7f8bf2ea30d68fac0d082ec21265f5", null ],
    [ "iot_i2c_read_async", "group__iot__i2c.html#ga4a2759a113c8be82ed14bc8059e09ff8", null ],
    [ "iot_i2c_write_async", "group__iot__i2c.html#gab86ebe47dc9c0cb0337f24cf4505f570", null ],
    [ "iot_i2c_ioctl", "group__iot__i2c.html#gad1f7684f8555cf807339aa717733cca7", null ],
    [ "iot_i2c_close", "group__iot__i2c.html#ga4c79a2c4038796eca1333ce1d964a139", null ],
    [ "iot_i2c_cancel", "group__iot__i2c.html#ga3139a61ef95d7d7c4ee9137b5f78d9f7", null ]
];