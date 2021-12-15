var iot__flash_8h =
[
    [ "IOT_FLASH_SUCCESS", "group__iot__flash.html#ga1f9d15bbe1a4f9230f87cc970365fe99", null ],
    [ "IOT_FLASH_INVALID_VALUE", "group__iot__flash.html#ga81aaa62fed3d88f46ba94fe71e452dcd", null ],
    [ "IOT_FLASH_WRITE_FAILED", "group__iot__flash.html#gab9c1332da8284618be132176cfadbaef", null ],
    [ "IOT_FLASH_READ_FAILED", "group__iot__flash.html#ga77f892f737ea29f1c2920be3fef8a878", null ],
    [ "IOT_FLASH_ERASE_FAILED", "group__iot__flash.html#ga0a3cdc25a766663c38c0a08a82027e42", null ],
    [ "IOT_FLASH_DEVICE_BUSY", "group__iot__flash.html#ga6c132308926c9acf1d1ebea1fa3e3c44", null ],
    [ "IOT_FLASH_CTRL_OP_FAILED", "group__iot__flash.html#ga0240ee98196932a9cbc0e9d13278ac07", null ],
    [ "IOT_FLASH_FUNCTION_NOT_SUPPORTED", "group__iot__flash.html#gade2dc6e2d0eccf30a12a103286d67ad0", null ],
    [ "IotFlashHandle_t", "group__iot__flash.html#gac4e989320abbbc6b48481f5b9cabe970", null ],
    [ "IotFlashCallback_t", "group__iot__flash.html#gaf1bffee5e74b79fd9446616c4987f0bd", null ],
    [ "IotFlashStatus_t", "group__iot__flash.html#gacd20e169bc34b6f5aae45d498573e4d1", [
      [ "eFlashIdle", "group__iot__flash.html#ggacd20e169bc34b6f5aae45d498573e4d1a62b7d78d68e265f071a6c5c95226561a", null ],
      [ "eFlashCmdInProgress", "group__iot__flash.html#ggacd20e169bc34b6f5aae45d498573e4d1a4c9bcc17019b0841278e038a43587452", null ],
      [ "eFlashEraseFailed", "group__iot__flash.html#ggacd20e169bc34b6f5aae45d498573e4d1afa12b0e28258c6b7289301cac8bab4dc", null ],
      [ "eFlashProgramSuspended", "group__iot__flash.html#ggacd20e169bc34b6f5aae45d498573e4d1ad4ad35c9694b132084f7cdfceb44dce5", null ],
      [ "eFlashEraseSuspended", "group__iot__flash.html#ggacd20e169bc34b6f5aae45d498573e4d1a24a9aec9e54f4ba266b72aa8ceb36a6f", null ]
    ] ],
    [ "IotFlashOperationStatus_t", "group__iot__flash.html#gafa04f783e4395d3c43472372188964b1", [
      [ "eFlashCompleted", "group__iot__flash.html#ggafa04f783e4395d3c43472372188964b1a6fb157a2b448c1811fecf6e8eb5139a3", null ],
      [ "eFlashLastEraseFailed", "group__iot__flash.html#ggafa04f783e4395d3c43472372188964b1ad52da42f95bc036169ea507e30e0218f", null ],
      [ "eFlashLastReadFailed", "group__iot__flash.html#ggafa04f783e4395d3c43472372188964b1a8780a317ca838c0b684baf74b31b2cce", null ],
      [ "eFlashLastWriteFailed", "group__iot__flash.html#ggafa04f783e4395d3c43472372188964b1a173f89738a7648bb7b34fc854b099555", null ]
    ] ],
    [ "IotFlashWriteProtect_t", "group__iot__flash.html#gafb762b29a09b39cfc2ac6e4e46779f10", [
      [ "eFlashReadWrite", "group__iot__flash.html#ggafb762b29a09b39cfc2ac6e4e46779f10a302b7cc744974e3d3a2590042a7d7302", null ],
      [ "eFlashReadOnly", "group__iot__flash.html#ggafb762b29a09b39cfc2ac6e4e46779f10a2599eb8a70c2210af2b7e04b9cd0c303", null ]
    ] ],
    [ "IotFlashIoctlRequest_t", "group__iot__flash.html#ga28f92d41621231a065fb29794517d936", [
      [ "eSetFlashBlockProtection", "group__iot__flash.html#gga28f92d41621231a065fb29794517d936a2177aa2ad1e9c21acbc90d869e128809", null ],
      [ "eGetFlashBlockProtection", "group__iot__flash.html#gga28f92d41621231a065fb29794517d936a31d2d4bfb00991ecdefa291fee44002a", null ],
      [ "eSuspendFlashProgramErase", "group__iot__flash.html#gga28f92d41621231a065fb29794517d936ae73b30fab7f134c109b25dbdabf64a5c", null ],
      [ "eResumeFlashProgramErase", "group__iot__flash.html#gga28f92d41621231a065fb29794517d936a3324563885658a786878554a83407d00", null ],
      [ "eGetFlashStatus", "group__iot__flash.html#gga28f92d41621231a065fb29794517d936a5771fb727bd1b645091e13d56dae30f2", null ],
      [ "eGetFlashTxNoOfbytes", "group__iot__flash.html#gga28f92d41621231a065fb29794517d936aab8c3c22b5faac2aab78858e45f51ae2", null ],
      [ "eGetFlashRxNoOfbytes", "group__iot__flash.html#gga28f92d41621231a065fb29794517d936a818aa05dce831a7fdb086f23aa564ac6", null ]
    ] ],
    [ "iot_flash_open", "group__iot__flash.html#ga50f778eca5447705287967eaab6e8ea2", null ],
    [ "iot_flash_getinfo", "group__iot__flash.html#ga7159088510ab0dff69f490ce9a0c7757", null ],
    [ "iot_flash_set_callback", "group__iot__flash.html#gaea612a702ba4dbd5b8fbe5b3d97eedfb", null ],
    [ "iot_flash_ioctl", "group__iot__flash.html#ga76bf68f5f107e5b5f1b56793b102fd27", null ],
    [ "iot_flash_erase_sectors", "group__iot__flash.html#ga9617c1dc402696b6e4cc632eb32bf3a2", null ],
    [ "iot_flash_erase_chip", "group__iot__flash.html#gaa933226a1ebceab85117dec49923cbc0", null ],
    [ "iot_flash_write_sync", "group__iot__flash.html#ga4202fbefdad0d9b0d327a88b9c4bd34a", null ],
    [ "iot_flash_read_sync", "group__iot__flash.html#ga28dd7397dc892977fb66de03c6bbc4ac", null ],
    [ "iot_flash_write_async", "group__iot__flash.html#gaeac741ec315ac0acdbd52324e87641da", null ],
    [ "iot_flash_read_async", "group__iot__flash.html#gaf9c0af22e3f736bc995d48f534d4c347", null ],
    [ "iot_flash_close", "group__iot__flash.html#ga4fb373990d8ab485ec2e395ce00c843c", null ]
];