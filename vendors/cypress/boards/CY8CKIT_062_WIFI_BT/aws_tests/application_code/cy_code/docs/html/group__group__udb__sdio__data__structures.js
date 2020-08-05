var group__group__udb__sdio__data__structures =
[
    [ "stc_sdio_event_flag_t", "group__group__udb__sdio__data__structures.html#structstc__sdio__event__flag__t", [
      [ "u8CmdComplete", "group__group__udb__sdio__data__structures.html#a061029c2fa96edef8ab2fe29a9053bca", null ],
      [ "u8TransComplete", "group__group__udb__sdio__data__structures.html#a07343d30d3fdcbe8bfd953bd900a76a0", null ],
      [ "u8CRCError", "group__group__udb__sdio__data__structures.html#af1b91233badae2c85908bce3a7e83fb1", null ]
    ] ],
    [ "stc_sdio_irq_cb_t", "group__group__udb__sdio__data__structures.html#structstc__sdio__irq__cb__t", [
      [ "pfnCardIntCb", "group__group__udb__sdio__data__structures.html#a36cc7aaabc59ff9d6cf313fcd464af69", null ]
    ] ],
    [ "stc_sdio_gInternalData_t", "group__group__udb__sdio__data__structures.html#structstc__sdio__g_internal_data__t", [
      [ "pstcCallBacks", "group__group__udb__sdio__data__structures.html#af430c75eed0b25b3f7c3b6080fd35f9f", null ],
      [ "stcEvents", "group__group__udb__sdio__data__structures.html#a8d9a9a97d4df992243d29d04fc3f5280", null ]
    ] ],
    [ "stc_sdio_cmd_config_t", "group__group__udb__sdio__data__structures.html#structstc__sdio__cmd__config__t", [
      [ "u8CmdIndex", "group__group__udb__sdio__data__structures.html#a5ef64c6f9a0f04e27ad77e23e7bb332d", null ],
      [ "u32Argument", "group__group__udb__sdio__data__structures.html#a679a95c2e03f0a7192081305c736ca38", null ],
      [ "bResponseRequired", "group__group__udb__sdio__data__structures.html#a0d6a9500f8a82c38d74074aada3addbf", null ],
      [ "pu8ResponseBuf", "group__group__udb__sdio__data__structures.html#ab1d901e4a8f6b9d45415d316f16c5456", null ]
    ] ],
    [ "stc_sdio_data_config_t", "group__group__udb__sdio__data__structures.html#structstc__sdio__data__config__t", [
      [ "bRead", "group__group__udb__sdio__data__structures.html#a40ab9466e56167e364f453ae748ef202", null ],
      [ "u16BlockSize", "group__group__udb__sdio__data__structures.html#ae26656b470e843f62d03a4e1a63bfce7", null ],
      [ "u16BlockCount", "group__group__udb__sdio__data__structures.html#a510ad8c84a5a8ffcbd56d76727edff15", null ],
      [ "pu8Data", "group__group__udb__sdio__data__structures.html#a5544f2cb8dcf1a91759971b9e9d87881", null ]
    ] ],
    [ "stc_sdio_cmd_t", "group__group__udb__sdio__data__structures.html#structstc__sdio__cmd__t", [
      [ "u32CmdIdx", "group__group__udb__sdio__data__structures.html#adeee0ccc21d74fe1cb55a51f74a502ec", null ],
      [ "u32Arg", "group__group__udb__sdio__data__structures.html#a320c4d4286f518e2542ab9f922410609", null ],
      [ "pu32Response", "group__group__udb__sdio__data__structures.html#ae43fe2b0889d501b59722dba47d11f94", null ],
      [ "pu8Data", "group__group__udb__sdio__data__structures.html#a93c9e4f7e83bfb2032f420c36fa6f607", null ],
      [ "bRead", "group__group__udb__sdio__data__structures.html#a10f614526d366fe1c7400f878518bed3", null ],
      [ "u16BlockCnt", "group__group__udb__sdio__data__structures.html#a1d757369ae3f4ae514a9d2ac52fffbc7", null ],
      [ "u16BlockSize", "group__group__udb__sdio__data__structures.html#afc7fa3dc7ce42550ece41521ce18f370", null ]
    ] ],
    [ "sdio_card_int_cb_t", "group__group__udb__sdio__data__structures.html#gafc11f581c2e7d7e7a472d8534221fa87", null ],
    [ "en_sdio_event_t", "group__group__udb__sdio__data__structures.html#ga319933645e97c021e8cbd43ba8b57180", [
      [ "SdCmdEventCmdDone", "group__group__udb__sdio__data__structures.html#gga319933645e97c021e8cbd43ba8b57180a5379b3ea8106972a47f08b1501f84102", null ],
      [ "SdCmdEventTransferDone", "group__group__udb__sdio__data__structures.html#gga319933645e97c021e8cbd43ba8b57180ab513258ceed11e1a6d893b21f4cff179", null ]
    ] ],
    [ "en_sdio_result_t", "group__group__udb__sdio__data__structures.html#ga19f4a94e6ce5cb94b5dca629741c2b38", [
      [ "Ok", "group__group__udb__sdio__data__structures.html#gga19f4a94e6ce5cb94b5dca629741c2b38a06d32f047358de4e6a30c28046f4688e", null ],
      [ "Error", "group__group__udb__sdio__data__structures.html#gga19f4a94e6ce5cb94b5dca629741c2b38a4dfd42ec49d09d8c6555c218301cc30f", null ],
      [ "CommandCrcError", "group__group__udb__sdio__data__structures.html#gga19f4a94e6ce5cb94b5dca629741c2b38aaa770a934bfefc60d322abab65bfe8a6", null ],
      [ "CommandIdxError", "group__group__udb__sdio__data__structures.html#gga19f4a94e6ce5cb94b5dca629741c2b38aa52a72ff13b657535c822f36d5ad0c4c", null ],
      [ "CommandEndError", "group__group__udb__sdio__data__structures.html#gga19f4a94e6ce5cb94b5dca629741c2b38a0197ae65a422cf329c567e100c25eccb", null ],
      [ "DataCrcError", "group__group__udb__sdio__data__structures.html#gga19f4a94e6ce5cb94b5dca629741c2b38ad40a99f8e6d96768983934c18197c4fd", null ],
      [ "CMDTimeout", "group__group__udb__sdio__data__structures.html#gga19f4a94e6ce5cb94b5dca629741c2b38afd18daa3d0b40e98d6d41eaa54921c50", null ],
      [ "DataTimeout", "group__group__udb__sdio__data__structures.html#gga19f4a94e6ce5cb94b5dca629741c2b38a33e2ea268376cda791e6714a11c85aa1", null ],
      [ "ResponseFlagError", "group__group__udb__sdio__data__structures.html#gga19f4a94e6ce5cb94b5dca629741c2b38a90116a08ef08360d5b6c46c27afc3ef9", null ]
    ] ]
];