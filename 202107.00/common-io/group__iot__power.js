var group__iot__power =
[
    [ "IotPowerWakeupSources_t", "struct_iot_power_wakeup_sources__t.html", [
      [ "pucWakeupSources", "struct_iot_power_wakeup_sources__t.html#ac994d98c45eb5883a04a3ae2854a1f60", null ],
      [ "usLength", "struct_iot_power_wakeup_sources__t.html#a6b8ffa8fcb7ef1bc2dafd15f1a725af5", null ]
    ] ],
    [ "IOT_POWER_SUCCESS", "group__iot__power.html#ga073f0548fec06dc28c8e99dd6925a04c", null ],
    [ "IOT_POWER_INVALID_VALUE", "group__iot__power.html#ga86da222302ac7bcf241472567957d3f9", null ],
    [ "IOT_POWER_NOT_INITIALIZED", "group__iot__power.html#gaef49328ef17d00712c71c1defdd5d772", null ],
    [ "IOT_POWER_GET_FAILED", "group__iot__power.html#ga164dd2088e8b69ac4a8818ffb10e0ac9", null ],
    [ "IOT_POWER_SET_FAILED", "group__iot__power.html#ga9dea30a052cdfed0f5de592f443865cd", null ],
    [ "IOT_POWER_FUNCTION_NOT_SUPPORTED", "group__iot__power.html#gaa87df195ca018031012c066086ed8100", null ],
    [ "IotPowerHandle_t", "group__iot__power.html#ga9c962b7bc78d9761e6eea4b6bc16a5a8", null ],
    [ "IotPowerCallback_t", "group__iot__power.html#ga37fbfd9722dadaf11437948892e52d71", null ],
    [ "IotPowerMode_t", "group__iot__power.html#ga56e09b75c6dff00aa546ddbcf36d3e24", [
      [ "eHighPowerPerfMode", "group__iot__power.html#gga56e09b75c6dff00aa546ddbcf36d3e24a8dbf38598a0c8833271f09bc48f9327f", null ],
      [ "eNormalPowerPerfMode", "group__iot__power.html#gga56e09b75c6dff00aa546ddbcf36d3e24a1bedc47c4109202eee9b81c91431bf1f", null ],
      [ "eLowPowerPerfMode", "group__iot__power.html#gga56e09b75c6dff00aa546ddbcf36d3e24aff9c9fe47803a46bdadd0bcb5fffd590", null ],
      [ "eUnknownPowerMode", "group__iot__power.html#gga56e09b75c6dff00aa546ddbcf36d3e24affd195044b991ce246ebababde913d31", null ]
    ] ],
    [ "IotPowerIdleMode_t", "group__iot__power.html#ga224aad48f367b30004b2cfd25fdb3d72", [
      [ "eIdleNoneMode", "group__iot__power.html#gga224aad48f367b30004b2cfd25fdb3d72affa0c878ae2438f4e5b9e94fac6d3e53", null ],
      [ "eIdlePCMode", "group__iot__power.html#gga224aad48f367b30004b2cfd25fdb3d72a83cadf4581d95162f8b4098f98211ffc", null ],
      [ "eIdleClkSrcOffModeMode", "group__iot__power.html#gga224aad48f367b30004b2cfd25fdb3d72a0e6b1199539e6573fb80b836a2c1f3c1", null ],
      [ "eIdleVddOffMode", "group__iot__power.html#gga224aad48f367b30004b2cfd25fdb3d72af6700a6c59ec996af51240fbdb3c09dc", null ]
    ] ],
    [ "IotPowerIoctlRequest_t", "group__iot__power.html#gaf75d3829597be038216e288e74e8c95d", [
      [ "eSetPCWakeThreshold", "group__iot__power.html#ggaf75d3829597be038216e288e74e8c95da058a052f5249abc1cfd23bf4a15a6b1a", null ],
      [ "eGetPCWakeThreshold", "group__iot__power.html#ggaf75d3829597be038216e288e74e8c95da9e7084edb6efade0c4770e27c10d1640", null ],
      [ "eSetClkSrcOffWakeThreshold", "group__iot__power.html#ggaf75d3829597be038216e288e74e8c95dab79e0de4e54a3082a7faa73a8655f43c", null ],
      [ "eGetClkSrcOffWakeThreshold", "group__iot__power.html#ggaf75d3829597be038216e288e74e8c95da6197a93b974bcf94c7d468ef9c399bc8", null ],
      [ "eSetVddOfffWakeThreshold", "group__iot__power.html#ggaf75d3829597be038216e288e74e8c95da55bbb237d2403f57f9a131641f714b5d", null ],
      [ "eGetVddOfffWakeThreshold", "group__iot__power.html#ggaf75d3829597be038216e288e74e8c95dab3acb9cbd894ca3b9ac32bdc9750aeb9", null ],
      [ "eSetWakeupSources", "group__iot__power.html#ggaf75d3829597be038216e288e74e8c95dacaf867cb0697a90ff7f304d4499abff6", null ],
      [ "eGetWakeupSources", "group__iot__power.html#ggaf75d3829597be038216e288e74e8c95dac4998f1eaab5a35a7bddd278f467e51f", null ],
      [ "eGetLastIdleMode", "group__iot__power.html#ggaf75d3829597be038216e288e74e8c95da0e66439501c2bf3eeb40337a6bd245ea", null ]
    ] ],
    [ "iot_power_open", "group__iot__power.html#gae029ebaf20f96e32e5e38c41d6b6f097", null ],
    [ "iot_power_set_mode", "group__iot__power.html#ga42630cef3fe685829f7e9b7415ba6bbc", null ],
    [ "iot_power_reset_mode", "group__iot__power.html#ga92624c6af6a1e48fa9f0bafb03fa052b", null ],
    [ "iot_power_set_callback", "group__iot__power.html#gaec9403194b6354385ba42e9cab67de6e", null ],
    [ "iot_power_ioctl", "group__iot__power.html#ga206f95fdbc8a6bb71017d9c699e0aefc", null ],
    [ "iot_power_get_mode", "group__iot__power.html#gabc06f80aba7d01ca8b844afa5c1ffd18", null ],
    [ "iot_power_close", "group__iot__power.html#gaa9f9a4c9ea5c99b1d5074f4e9cefdea9", null ]
];