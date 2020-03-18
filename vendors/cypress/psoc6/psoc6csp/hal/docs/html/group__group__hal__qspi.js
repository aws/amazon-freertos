var group__group__hal__qspi =
[
    [ "cyhal_qspi_command_t", "group__group__hal__qspi.html#structcyhal__qspi__command__t", [
      [ "instruction", "group__group__hal__qspi.html#a08599350f3641e17c920fdc050cb7018", null ],
      [ "address", "group__group__hal__qspi.html#a8ba1a851d619001c2c00955afa4c268b", null ],
      [ "mode_bits", "group__group__hal__qspi.html#a15223b1038366a5e54e45190e9f7b063", null ],
      [ "dummy_count", "group__group__hal__qspi.html#a972b6ca026d8e28490ab029d431b7a20", null ],
      [ "data", "group__group__hal__qspi.html#aca4af49eaf3ffcc98d6fcaa1b49fb601", null ]
    ] ],
    [ "cyhal_qspi_command_t.instruction", "group__group__hal__qspi.html#structcyhal__qspi__command__t_8instruction", [
      [ "bus_width", "group__group__hal__qspi.html#aaea78d2eed5d55bf6a19b3a4933520d2", null ],
      [ "value", "group__group__hal__qspi.html#a2063c1608d6e0baf80249c42e2be5804", null ],
      [ "disabled", "group__group__hal__qspi.html#a075ae3d2fc31640504f814f60e5ef713", null ]
    ] ],
    [ "cyhal_qspi_command_t.address", "group__group__hal__qspi.html#structcyhal__qspi__command__t_8address", [
      [ "bus_width", "group__group__hal__qspi.html#aaea78d2eed5d55bf6a19b3a4933520d2", null ],
      [ "size", "group__group__hal__qspi.html#af7bd60b75b29d79b660a2859395c1a24", null ],
      [ "value", "group__group__hal__qspi.html#a2063c1608d6e0baf80249c42e2be5804", null ],
      [ "disabled", "group__group__hal__qspi.html#a075ae3d2fc31640504f814f60e5ef713", null ]
    ] ],
    [ "cyhal_qspi_command_t.mode_bits", "group__group__hal__qspi.html#structcyhal__qspi__command__t_8mode__bits", [
      [ "bus_width", "group__group__hal__qspi.html#aaea78d2eed5d55bf6a19b3a4933520d2", null ],
      [ "size", "group__group__hal__qspi.html#af7bd60b75b29d79b660a2859395c1a24", null ],
      [ "value", "group__group__hal__qspi.html#a2063c1608d6e0baf80249c42e2be5804", null ],
      [ "disabled", "group__group__hal__qspi.html#a075ae3d2fc31640504f814f60e5ef713", null ]
    ] ],
    [ "cyhal_qspi_command_t.data", "group__group__hal__qspi.html#structcyhal__qspi__command__t_8data", [
      [ "bus_width", "group__group__hal__qspi.html#aaea78d2eed5d55bf6a19b3a4933520d2", null ]
    ] ],
    [ "CYHAL_QSPI_RSLT_ERR_BUS_WIDTH", "group__group__hal__qspi.html#gacbb2bfd332dbbd3589ce6677bc068066", null ],
    [ "CYHAL_QSPI_RSLT_ERR_PIN", "group__group__hal__qspi.html#gaaff3affb5a155c8341d8081c04ba5fc2", null ],
    [ "CYHAL_QSPI_RSLT_ERR_DATA_SEL", "group__group__hal__qspi.html#ga71bf90ec56d0ff067b535f1ea667fb01", null ],
    [ "CYHAL_QSPI_RSLT_ERR_INSTANCE", "group__group__hal__qspi.html#ga98afbaa8e54635a5033feffdc5ee3a8d", null ],
    [ "CYHAL_QSPI_RSLT_ERR_FREQUENCY", "group__group__hal__qspi.html#ga01bf613bc3c9e26af1008d79e1e9cbeb", null ],
    [ "cyhal_qspi_event_callback_t", "group__group__hal__qspi.html#ga42887dff6257345711b353990edb1ab1", null ],
    [ "cyhal_qspi_bus_width_t", "group__group__hal__qspi.html#gaddc93a79f0708fd53897d0c37bc7544b", null ],
    [ "cyhal_qspi_size_t", "group__group__hal__qspi.html#ga0633be58b4427d173d1f94c4e6e4e058", null ],
    [ "cyhal_qspi_event_t", "group__group__hal__qspi.html#gadae12b69a94f743b345655ec751d8aad", [
      [ "CYHAL_QSPI_EVENT_NONE", "group__group__hal__qspi.html#ggadae12b69a94f743b345655ec751d8aadac4bda5668bc258f4508d4a1d5cdec9df", null ],
      [ "CYHAL_QSPI_IRQ_TRANSMIT_DONE", "group__group__hal__qspi.html#ggadae12b69a94f743b345655ec751d8aadab1b4499185f082601c68a36c6be0d034", null ],
      [ "CYHAL_QSPI_IRQ_RECEIVE_DONE", "group__group__hal__qspi.html#ggadae12b69a94f743b345655ec751d8aada3c0c0f4d57cf0d2a00ab20ec587fe409", null ]
    ] ],
    [ "cyhal_qspi_init", "group__group__hal__qspi.html#gafeb79a3262be28e59c267b5146273588", null ],
    [ "cyhal_qspi_free", "group__group__hal__qspi.html#ga14906073e688d345f01f880f9400bba3", null ],
    [ "cyhal_qspi_set_frequency", "group__group__hal__qspi.html#ga67150e1497b4547854d48b23deb8fd47", null ],
    [ "cyhal_qspi_read", "group__group__hal__qspi.html#ga4e8f85b99de75604ef38becb14ae073d", null ],
    [ "cyhal_qspi_read_async", "group__group__hal__qspi.html#gae29816daa8fec2c03d00c927aa9e1558", null ],
    [ "cyhal_qspi_write", "group__group__hal__qspi.html#ga7f67f2108cdf6312557fca9be9a3cd0e", null ],
    [ "cyhal_qspi_write_async", "group__group__hal__qspi.html#gab4e9be936651ce19e56c16c0b3ae07af", null ],
    [ "cyhal_qspi_transfer", "group__group__hal__qspi.html#gad2ec06f4a49520a89d6b9e0299d0c1ce", null ],
    [ "cyhal_qspi_register_callback", "group__group__hal__qspi.html#ga3650436f640ed3e7640dafaa90268700", null ],
    [ "cyhal_qspi_enable_event", "group__group__hal__qspi.html#gaed074770bfe073453581fef037cc006d", null ]
];