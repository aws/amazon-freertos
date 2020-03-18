var group__group__hal__dma =
[
    [ "cyhal_dma_cfg_t", "group__group__hal__dma.html#structcyhal__dma__cfg__t", [
      [ "src_addr", "group__group__hal__dma.html#addcfd22935f15ed7fd86990afb2d8fe5", null ],
      [ "src_increment", "group__group__hal__dma.html#a82383d2bf599937a55503e744b70a0bf", null ],
      [ "dst_addr", "group__group__hal__dma.html#a4652fecb04594b84b09b07bee308e32a", null ],
      [ "dst_increment", "group__group__hal__dma.html#ac294f82b4a288c8d9ad084b3a6e9a26e", null ],
      [ "transfer_width", "group__group__hal__dma.html#ae5b6698dfc754dc0ca497842681ce7ef", null ],
      [ "length", "group__group__hal__dma.html#aa7fbd97e22cee8fd0079fe27479f00c1", null ],
      [ "burst_size", "group__group__hal__dma.html#ab9a976a59ce1acea9c281337c290d229", null ],
      [ "action", "group__group__hal__dma.html#a17a6575e0901cce49fdf8b7da8f2130d", null ]
    ] ],
    [ "CYHAL_DMA_RSLT_ERR_INVALID_TRANSFER_WIDTH", "group__group__hal__dma.html#gac542b630eb631806c288c72f223dc0c0", null ],
    [ "CYHAL_DMA_RSLT_ERR_INVALID_PARAMETER", "group__group__hal__dma.html#gac576d5f06fd63b8dc70e05acd558fe24", null ],
    [ "CYHAL_DMA_RSLT_ERR_INVALID_PRIORITY", "group__group__hal__dma.html#gac2510f51f4594fe57036b3159fd09b93", null ],
    [ "CYHAL_DMA_RSLT_ERR_INVALID_ALIGNMENT", "group__group__hal__dma.html#ga4ae033f30838fec2f52990da40138cb3", null ],
    [ "CYHAL_DMA_RSLT_ERR_INVALID_BURST_SIZE", "group__group__hal__dma.html#gaa6faad501be814ecf834fb164de05a02", null ],
    [ "CYHAL_DMA_RSLT_ERR_CHANNEL_BUSY", "group__group__hal__dma.html#ga4a5db2232d3cdda8b1680c1d24891a17", null ],
    [ "CYHAL_DMA_RSLT_WARN_TRANSFER_ALREADY_STARTED", "group__group__hal__dma.html#ga2eb08384825380516ae141ec3e719d8f", null ],
    [ "CYHAL_DMA_RSLT_FATAL_UNSUPPORTED_HARDWARE", "group__group__hal__dma.html#gae9b5dac93c1ca8dfea78883f4063a169", null ],
    [ "cyhal_dma_event_callback_t", "group__group__hal__dma.html#ga8d2fdb7ab4e9ba5367ef596ad42bbe3f", null ],
    [ "cyhal_dma_direction_t", "group__group__hal__dma.html#ga430c1e3ddedade3b6f7ef1f8532e665b", [
      [ "CYHAL_DMA_DIRECTION_MEM2MEM", "group__group__hal__dma.html#gga430c1e3ddedade3b6f7ef1f8532e665ba86dedc0ee3d9bf091e9f936040e2066a", null ],
      [ "CYHAL_DMA_DIRECTION_MEM2PERIPH", "group__group__hal__dma.html#gga430c1e3ddedade3b6f7ef1f8532e665baef4c3c201a805bc17ea57d8dcfe49f49", null ],
      [ "CYHAL_DMA_DIRECTION_PERIPH2MEM", "group__group__hal__dma.html#gga430c1e3ddedade3b6f7ef1f8532e665ba1226b2af396731bd8b959c14924bb612", null ],
      [ "CYHAL_DMA_DIRECTION_PERIPH2PERIPH", "group__group__hal__dma.html#gga430c1e3ddedade3b6f7ef1f8532e665ba6bb2146283e064542427ddffc2fbe0d7", null ]
    ] ],
    [ "cyhal_dma_event_t", "group__group__hal__dma.html#gad4507e9f3660b19e1ddc3085a96279e2", [
      [ "CYHAL_DMA_NO_INTR", "group__group__hal__dma.html#ggad4507e9f3660b19e1ddc3085a96279e2a31943df9f42ee4e594f2701f9daf16af", null ],
      [ "CYHAL_DMA_TRANSFER_COMPLETE", "group__group__hal__dma.html#ggad4507e9f3660b19e1ddc3085a96279e2ae5ed3faaed4b314d6b7115016e4c2e73", null ],
      [ "CYHAL_DMA_SRC_BUS_ERROR", "group__group__hal__dma.html#ggad4507e9f3660b19e1ddc3085a96279e2aed1388898b5c44164b69c29226461c4f", null ],
      [ "CYHAL_DMA_DST_BUS_ERROR", "group__group__hal__dma.html#ggad4507e9f3660b19e1ddc3085a96279e2a28e6e742cdc81f90dc1a0af854edcfa2", null ],
      [ "CYHAL_DMA_SRC_MISAL", "group__group__hal__dma.html#ggad4507e9f3660b19e1ddc3085a96279e2ac02b14f1884befe2fc3c736ca09b5063", null ],
      [ "CYHAL_DMA_DST_MISAL", "group__group__hal__dma.html#ggad4507e9f3660b19e1ddc3085a96279e2aa975cfc15572bcde29d3b79771f3f69d", null ],
      [ "CYHAL_DMA_CURR_PTR_NULL", "group__group__hal__dma.html#ggad4507e9f3660b19e1ddc3085a96279e2a5d44bed82a7ab60351ccdc8be3ccac85", null ],
      [ "CYHAL_DMA_ACTIVE_CH_DISABLED", "group__group__hal__dma.html#ggad4507e9f3660b19e1ddc3085a96279e2ac6197710bf44a105b69558913826465e", null ],
      [ "CYHAL_DMA_DESCR_BUS_ERROR", "group__group__hal__dma.html#ggad4507e9f3660b19e1ddc3085a96279e2ad48347e72e87bd61c17fdab21128a0ef", null ]
    ] ],
    [ "cyhal_dma_transfer_action_t", "group__group__hal__dma.html#ga05291825b0a7c7b9a92373ec53342b7a", [
      [ "CYHAL_DMA_TRANSFER_BURST", "group__group__hal__dma.html#gga05291825b0a7c7b9a92373ec53342b7aa1d892969519c81dd0728bc5ea99c835e", null ],
      [ "CYHAL_DMA_TRANSFER_FULL", "group__group__hal__dma.html#gga05291825b0a7c7b9a92373ec53342b7aac422419b0c70b168a56a7bedc2bc2dad", null ]
    ] ],
    [ "cyhal_dma_init", "group__group__hal__dma.html#ga77d62adf012b570ff1a157b43d134318", null ],
    [ "cyhal_dma_free", "group__group__hal__dma.html#ga8c1bf16caa5d53cb129e5544da1c8b21", null ],
    [ "cyhal_dma_configure", "group__group__hal__dma.html#gad39f32ac4fada8bc9e757df0a22fcce4", null ],
    [ "cyhal_dma_start_transfer", "group__group__hal__dma.html#gaf57a2b19848478b6604e09a3ecc3fbfc", null ],
    [ "cyhal_dma_is_busy", "group__group__hal__dma.html#ga6f962c164fedb088dbd2a48e21550fd0", null ],
    [ "cyhal_dma_register_callback", "group__group__hal__dma.html#gacbccbcab79700c9e17bc47c525967876", null ],
    [ "cyhal_dma_enable_event", "group__group__hal__dma.html#gad41941e505b20be164c389925e7a25dc", null ]
];