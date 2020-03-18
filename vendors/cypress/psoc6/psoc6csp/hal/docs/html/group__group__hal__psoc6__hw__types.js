var group__group__hal__psoc6__hw__types =
[
    [ "cyhal_resource_inst_t", "group__group__hal__psoc6__hw__types.html#structcyhal__resource__inst__t", [
      [ "type", "group__group__hal__psoc6__hw__types.html#ad111187a65d182386ea96858369b83d4", null ],
      [ "block_num", "group__group__hal__psoc6__hw__types.html#a91895766b5f5efe1dfffac2d7537e73a", null ],
      [ "channel_num", "group__group__hal__psoc6__hw__types.html#a4488a199114a86e5a72184c7a0a16c06", null ]
    ] ],
    [ "cyhal_clock_divider_t", "group__group__hal__psoc6__hw__types.html#structcyhal__clock__divider__t", [
      [ "div_type", "group__group__hal__psoc6__hw__types.html#a4733f829f74e68dc41b0ba756b170143", null ],
      [ "div_num", "group__group__hal__psoc6__hw__types.html#a0a5eb659204369f9b0add0400cb6a625", null ]
    ] ],
    [ "cyhal_event_callback_data_t", "group__group__hal__psoc6__hw__types.html#structcyhal__event__callback__data__t", [
      [ "callback", "group__group__hal__psoc6__hw__types.html#a3c3d10edf78afaf402eb6eb9004a17c0", null ],
      [ "callback_arg", "group__group__hal__psoc6__hw__types.html#aeabdbaef4f75c5b947fa9d1534d230f9", null ]
    ] ],
    [ "cyhal_adc_t", "group__group__hal__psoc6__hw__types.html#structcyhal__adc__t", [
      [ "base", "group__group__hal__psoc6__hw__types.html#ac5615cdba01efd5c110f0ca6adbef43e", null ],
      [ "resource", "group__group__hal__psoc6__hw__types.html#a9343981a568de936a5c729c9b94e3888", null ],
      [ "clock", "group__group__hal__psoc6__hw__types.html#a3be15ab591892f42f2e8047b113ac11d", null ],
      [ "dedicated_clock", "group__group__hal__psoc6__hw__types.html#a1446fcf0c070b58ba0a02a6d1ad0e167", null ],
      [ "channel_used", "group__group__hal__psoc6__hw__types.html#a99ad2633b7863087b7e7f15e6c5edfa8", null ]
    ] ],
    [ "cyhal_adc_channel_t", "group__group__hal__psoc6__hw__types.html#structcyhal__adc__channel__t", [
      [ "adc", "group__group__hal__psoc6__hw__types.html#ae3f5aaa0bc16116937f581313a9a88cf", null ],
      [ "pin", "group__group__hal__psoc6__hw__types.html#a2992d44c4da33b26a3e4a0bccb87c5f7", null ],
      [ "channel_idx", "group__group__hal__psoc6__hw__types.html#a6bf6a5d80c620d0c1ffd7e281f4ff5e1", null ]
    ] ],
    [ "cyhal_crc_t", "group__group__hal__psoc6__hw__types.html#structcyhal__crc__t", [
      [ "base", "group__group__hal__psoc6__hw__types.html#aa8172d5f14d9a24826e003df016ea62d", null ],
      [ "resource", "group__group__hal__psoc6__hw__types.html#aac1de671fc1cf74d6808fd126fe89238", null ],
      [ "crc_width", "group__group__hal__psoc6__hw__types.html#a6e9f1abc6fc3e175b4d6536a050d320f", null ]
    ] ],
    [ "cyhal_dac_t", "group__group__hal__psoc6__hw__types.html#structcyhal__dac__t", [
      [ "base", "group__group__hal__psoc6__hw__types.html#ae3f1adca8232cf5d6835ed5d21843c96", null ],
      [ "resource", "group__group__hal__psoc6__hw__types.html#a63d98a1c184591f8219d4f479f36bd75", null ],
      [ "pin", "group__group__hal__psoc6__hw__types.html#a7e9694b84de60b955ff0d8a6afbc55c2", null ]
    ] ],
    [ "cyhal_dma_t", "group__group__hal__psoc6__hw__types.html#structcyhal__dma__t", [
      [ "resource", "group__group__hal__psoc6__hw__types.html#a435949478e9279c9270c62843a69070e", null ],
      [ "channel_config", "group__group__hal__psoc6__hw__types.html#a247c4d7230d59372568288b37836ba22", null ],
      [ "descriptor_config", "group__group__hal__psoc6__hw__types.html#aaa6622eeae4867e2db07bbb6a1e9e41a", null ],
      [ "descriptor", "group__group__hal__psoc6__hw__types.html#ad3c6025f50891123f61f825fede0e2f2", null ],
      [ "irq_cause", "group__group__hal__psoc6__hw__types.html#ae05f049b8e40d8f1a111ec3328afc5a7", null ],
      [ "callback_data", "group__group__hal__psoc6__hw__types.html#a9fb6d3fe2403deb665ce86e958fb956a", null ]
    ] ],
    [ "cyhal_dma_t.channel_config", "group__group__hal__psoc6__hw__types.html#unioncyhal__dma__t_8channel__config", [
      [ "dw", "group__group__hal__psoc6__hw__types.html#a1f2121f36f817bd18540e5fa7de06f59", null ],
      [ "dmac", "group__group__hal__psoc6__hw__types.html#ae555fc29296b95adb45c0092d8f35338", null ]
    ] ],
    [ "cyhal_dma_t.descriptor_config", "group__group__hal__psoc6__hw__types.html#unioncyhal__dma__t_8descriptor__config", [
      [ "dw", "group__group__hal__psoc6__hw__types.html#a1f2121f36f817bd18540e5fa7de06f59", null ],
      [ "dmac", "group__group__hal__psoc6__hw__types.html#ae555fc29296b95adb45c0092d8f35338", null ]
    ] ],
    [ "cyhal_dma_t.descriptor", "group__group__hal__psoc6__hw__types.html#unioncyhal__dma__t_8descriptor", [
      [ "dw", "group__group__hal__psoc6__hw__types.html#a1f2121f36f817bd18540e5fa7de06f59", null ],
      [ "dmac", "group__group__hal__psoc6__hw__types.html#ae555fc29296b95adb45c0092d8f35338", null ]
    ] ],
    [ "cyhal_flash_t", "group__group__hal__psoc6__hw__types.html#structcyhal__flash__t", [
      [ "empty", "group__group__hal__psoc6__hw__types.html#aaa4efca804da27aa9934f5bc2a88b609", null ]
    ] ],
    [ "cyhal_i2c_t", "group__group__hal__psoc6__hw__types.html#structcyhal__i2c__t", [
      [ "base", "group__group__hal__psoc6__hw__types.html#a0929625ac8253a070d2edb3cc3c1bd4e", null ],
      [ "resource", "group__group__hal__psoc6__hw__types.html#a34241849f682fd5bee4f6b7be9c5e44d", null ],
      [ "pin_sda", "group__group__hal__psoc6__hw__types.html#ab6d7b3f033a28dd9589d765d3c12e758", null ],
      [ "pin_scl", "group__group__hal__psoc6__hw__types.html#ae48ab483971698a3695c2145e1eafe28", null ],
      [ "clock", "group__group__hal__psoc6__hw__types.html#a5b92064689aa877f10dbb8ddf74d0f35", null ],
      [ "is_shared_clock", "group__group__hal__psoc6__hw__types.html#ad927263ef78c2fca8f05a0a2bcfe4dc3", null ],
      [ "context", "group__group__hal__psoc6__hw__types.html#aa56649f560fd041a0b1303921e2ba3fa", null ],
      [ "rx_config", "group__group__hal__psoc6__hw__types.html#a43ab6f2f527440a244846ee58b1e903d", null ],
      [ "tx_config", "group__group__hal__psoc6__hw__types.html#aace0bcce15d6519d9d7edf4470719b68", null ],
      [ "is_slave", "group__group__hal__psoc6__hw__types.html#ab603bff9673ba097723feebcb95107d4", null ],
      [ "async", "group__group__hal__psoc6__hw__types.html#a7fb0b40848cf8486c0f10f8b5f2abfc5", null ],
      [ "address", "group__group__hal__psoc6__hw__types.html#a1f7b4087fcc65fadd219cc056620857b", null ],
      [ "irq_cause", "group__group__hal__psoc6__hw__types.html#a74a6c300ca7a9aab30886a343e1c6d19", null ],
      [ "pending", "group__group__hal__psoc6__hw__types.html#ade05e50ecdc505da34ca7a0cc190d7fd", null ],
      [ "events", "group__group__hal__psoc6__hw__types.html#afa9f049ebdc491d46985c81ff80cb4e8", null ],
      [ "callback_data", "group__group__hal__psoc6__hw__types.html#aaaab9b7e79c057162b0f000e0502295c", null ]
    ] ],
    [ "cyhal_ezi2c_t", "group__group__hal__psoc6__hw__types.html#structcyhal__ezi2c__t", [
      [ "base", "group__group__hal__psoc6__hw__types.html#a1acfb04d7f408bef718b8fc65d3e51ce", null ],
      [ "resource", "group__group__hal__psoc6__hw__types.html#a9c99de3b28996d598c0bfff24553fe78", null ],
      [ "pin_sda", "group__group__hal__psoc6__hw__types.html#a28cfb1d3efde5f9680cc1da02804cb34", null ],
      [ "pin_scl", "group__group__hal__psoc6__hw__types.html#a75b314887d1fcaaaba028873a3f1292d", null ],
      [ "clock", "group__group__hal__psoc6__hw__types.html#acbdbb4ebcadb36dd632d9eb53dcbc8d2", null ],
      [ "is_shared_clock", "group__group__hal__psoc6__hw__types.html#af155dccb8af1decc16cce52d4ceaf7cc", null ],
      [ "context", "group__group__hal__psoc6__hw__types.html#a3aa065ed0c7c1d4d24c559cc4f4a644b", null ],
      [ "irq_cause", "group__group__hal__psoc6__hw__types.html#a9cded0c71c054dea98ddf9f0c68d5ff7", null ],
      [ "callback_data", "group__group__hal__psoc6__hw__types.html#a78fc15a46183b145984ad509e430d8af", null ]
    ] ],
    [ "cyhal_lptimer_t", "group__group__hal__psoc6__hw__types.html#structcyhal__lptimer__t", [
      [ "base", "group__group__hal__psoc6__hw__types.html#ab6e909cedc7f2a404a5de186d7c731cb", null ],
      [ "resource", "group__group__hal__psoc6__hw__types.html#a06e532202124b44f3cbd3a88270f50ae", null ],
      [ "callback_data", "group__group__hal__psoc6__hw__types.html#aee76c50832553de4525a4270d5d06df5", null ]
    ] ],
    [ "cyhal_pwm_t", "group__group__hal__psoc6__hw__types.html#structcyhal__pwm__t", [
      [ "base", "group__group__hal__psoc6__hw__types.html#a41c9cf028da6911d16b3954c4a74ddf5", null ],
      [ "resource", "group__group__hal__psoc6__hw__types.html#a95ce1ac7eb33b0923c3d6ee60a1f94e8", null ],
      [ "pin", "group__group__hal__psoc6__hw__types.html#adbc3775af399b89b28f710ba299e633e", null ],
      [ "pin_compl", "group__group__hal__psoc6__hw__types.html#acd470392a044f5d9e89462be54e54516", null ],
      [ "clock", "group__group__hal__psoc6__hw__types.html#aa4cbd043e68a6683c6ba7ff259fe3628", null ],
      [ "clock_hz", "group__group__hal__psoc6__hw__types.html#ad501ff10983847c24cac342a38b8794f", null ],
      [ "dedicated_clock", "group__group__hal__psoc6__hw__types.html#af556afd3af8713db166800d38c74387c", null ],
      [ "callback_data", "group__group__hal__psoc6__hw__types.html#a374fc521b5fabf12c01e7fd4c164e134", null ]
    ] ],
    [ "cyhal_qspi_t", "group__group__hal__psoc6__hw__types.html#structcyhal__qspi__t", [
      [ "base", "group__group__hal__psoc6__hw__types.html#a7d4a257b40a90d8df5f2078c333be024", null ],
      [ "resource", "group__group__hal__psoc6__hw__types.html#ac59d6d6a74889f341406e5abafa07625", null ],
      [ "pin_ios", "group__group__hal__psoc6__hw__types.html#a1593d196807e7bbf202d9785006c66ba", null ],
      [ "pin_sclk", "group__group__hal__psoc6__hw__types.html#a69cf396fb94143caf3c17f21e77fe30a", null ],
      [ "pin_ssel", "group__group__hal__psoc6__hw__types.html#a49f51d923e0c4d0c08911b8853bf9b30", null ],
      [ "frequency", "group__group__hal__psoc6__hw__types.html#a90a548622c5d7bb4a67aaf4ffebf4f97", null ],
      [ "mode", "group__group__hal__psoc6__hw__types.html#a32c76b6134b5fcfc5ed7a5bed1ebe93d", null ],
      [ "context", "group__group__hal__psoc6__hw__types.html#a701c962e05235ae4d577fd2831df2f1a", null ],
      [ "slave_select", "group__group__hal__psoc6__hw__types.html#a03a0419ecb43ffc36c7d6f407f49fc9d", null ],
      [ "data_select", "group__group__hal__psoc6__hw__types.html#a9174d14499fb4f0af86ada40d9a6c750", null ],
      [ "irq_cause", "group__group__hal__psoc6__hw__types.html#a6e5636079826219fc50c6eca3d0b42ba", null ],
      [ "callback_data", "group__group__hal__psoc6__hw__types.html#a548bcda97b2206efadd024b69339973a", null ]
    ] ],
    [ "cyhal_trng_t", "group__group__hal__psoc6__hw__types.html#structcyhal__trng__t", [
      [ "base", "group__group__hal__psoc6__hw__types.html#acf8a5871571c60e989496cefe80df53f", null ],
      [ "resource", "group__group__hal__psoc6__hw__types.html#adff657185b0e7645d799377dedc5eaf6", null ]
    ] ],
    [ "cyhal_rtc_t", "group__group__hal__psoc6__hw__types.html#structcyhal__rtc__t", [
      [ "empty", "group__group__hal__psoc6__hw__types.html#aff21062458721c4f2e7cc6eb9700f815", null ]
    ] ],
    [ "cyhal_sdhc_t", "group__group__hal__psoc6__hw__types.html#structcyhal__sdhc__t", [
      [ "base", "group__group__hal__psoc6__hw__types.html#af8cbea03143779795a2e4972c635f4dd", null ],
      [ "resource", "group__group__hal__psoc6__hw__types.html#ad179252d21dd3cb37ae739cd4cb8926a", null ],
      [ "emmc", "group__group__hal__psoc6__hw__types.html#af4fc74f61d8086822834b928787ac79e", null ],
      [ "dmaType", "group__group__hal__psoc6__hw__types.html#a365b370b54b12e6754a3dce00a6df82d", null ],
      [ "enableLedControl", "group__group__hal__psoc6__hw__types.html#ad2c4ad3c7a68210914f6a180747674b9", null ],
      [ "context", "group__group__hal__psoc6__hw__types.html#ace871089c0db12bbea58e571514ab471", null ],
      [ "pin_clk", "group__group__hal__psoc6__hw__types.html#af7f97c5e37997a65df7799c5e66455fe", null ],
      [ "pin_cmd", "group__group__hal__psoc6__hw__types.html#a02e5d8916c5c1e15152a95744d6895be", null ],
      [ "pin_data", "group__group__hal__psoc6__hw__types.html#a072f05e510d39475bc879745f8f67346", null ],
      [ "pin_cardDetect", "group__group__hal__psoc6__hw__types.html#afe0c6a7e564b723137384810daaebea0", null ],
      [ "pin_ioVoltSel", "group__group__hal__psoc6__hw__types.html#a23e2d73cec426c5c71195c5e07209183", null ],
      [ "pin_cardIfPwrEn", "group__group__hal__psoc6__hw__types.html#ab0ef18346eebebc8b413ef63b9cdf2ac", null ],
      [ "pin_cardMechWriteProt", "group__group__hal__psoc6__hw__types.html#a914137cebd5692f79343f6327a959148", null ],
      [ "pin_ledCtrl", "group__group__hal__psoc6__hw__types.html#a123619860ae2fcaeee3f350a53836a99", null ],
      [ "pin_cardEmmcReset", "group__group__hal__psoc6__hw__types.html#a17154ca2e853521132aaffb9d6efa1ec", null ],
      [ "irq_cause", "group__group__hal__psoc6__hw__types.html#adadc83241a4d70cc0d2139218749ca72", null ],
      [ "callback_data", "group__group__hal__psoc6__hw__types.html#a2bd77b27b881fa7a20283d847ac0fd54", null ]
    ] ],
    [ "cyhal_sdio_t", "group__group__hal__psoc6__hw__types.html#structcyhal__sdio__t", [
      [ "base", "group__group__hal__psoc6__hw__types.html#ac4b575bfe95953180357fc511f1e5914", null ],
      [ "resource", "group__group__hal__psoc6__hw__types.html#a722e1255e00bdf81242a40eda39e2290", null ],
      [ "emmc", "group__group__hal__psoc6__hw__types.html#a6cfdc4e7436bee85524491bcbaa71f3d", null ],
      [ "dmaType", "group__group__hal__psoc6__hw__types.html#ad85bb812245a8b5e46d611db6872e3f3", null ],
      [ "context", "group__group__hal__psoc6__hw__types.html#abbfdd2921695a42d9bef2aa0bf9bbfed", null ],
      [ "pin_clk", "group__group__hal__psoc6__hw__types.html#a2ee725f184bcac8b12f8ea1a2eeb2635", null ],
      [ "pin_cmd", "group__group__hal__psoc6__hw__types.html#a6e03fcad3619f23a805c9f0f4cef84e3", null ],
      [ "pin_data0", "group__group__hal__psoc6__hw__types.html#ae6e6a56a702ec396296b3157a2461077", null ],
      [ "pin_data1", "group__group__hal__psoc6__hw__types.html#a0d6a82b63f847fd52e6f9834c257d568", null ],
      [ "pin_data2", "group__group__hal__psoc6__hw__types.html#a933ec9a436f9ddd1ee2321871bdcbf5f", null ],
      [ "pin_data3", "group__group__hal__psoc6__hw__types.html#a9268ca2e6abbe7873c914ce6cb85d18d", null ],
      [ "frequencyhal_hz", "group__group__hal__psoc6__hw__types.html#a1575bc5e8889960f5d34d3e7926a1931", null ],
      [ "block_size", "group__group__hal__psoc6__hw__types.html#aff42f0fd4ff5e0026f7532469d829f08", null ],
      [ "irq_cause", "group__group__hal__psoc6__hw__types.html#aa69c64759bc59b3836ce07317ca735bc", null ],
      [ "pm_params", "group__group__hal__psoc6__hw__types.html#aae4f29af7f36f95cd6349c43b9da5c03", null ],
      [ "pm_callback", "group__group__hal__psoc6__hw__types.html#a03cc99ecdc625bc088a180358ebde181", null ],
      [ "events", "group__group__hal__psoc6__hw__types.html#acb9a4e41a6c52418ccd92285f5eadde0", null ],
      [ "callback_data", "group__group__hal__psoc6__hw__types.html#a8eaf2c237cb6229a06e774981c39f081", null ]
    ] ],
    [ "cyhal_spi_t", "group__group__hal__psoc6__hw__types.html#structcyhal__spi__t", [
      [ "base", "group__group__hal__psoc6__hw__types.html#a9d0c88aefd98c3154063d62806bc7896", null ],
      [ "resource", "group__group__hal__psoc6__hw__types.html#a50ef30629c0ed6b9b88fd3edd1d6d917", null ],
      [ "pin_miso", "group__group__hal__psoc6__hw__types.html#a4abba543deddc161985cb5e2bce54a27", null ],
      [ "pin_mosi", "group__group__hal__psoc6__hw__types.html#aeb604dd90daa694e30c9401c4a35a261", null ],
      [ "pin_sclk", "group__group__hal__psoc6__hw__types.html#a0257721a8d77e714a2403d20c474097b", null ],
      [ "pin_ssel", "group__group__hal__psoc6__hw__types.html#a93109d08aba142cbfde856aed127c295", null ],
      [ "clock", "group__group__hal__psoc6__hw__types.html#a843f3879714133f0932b80ba2d600769", null ],
      [ "clk_mode", "group__group__hal__psoc6__hw__types.html#a53afd13ba64fe0511d4e6a18e1e0e5de", null ],
      [ "mode", "group__group__hal__psoc6__hw__types.html#a48e2879bfa6bea23645f5e2b8a684eb9", null ],
      [ "data_bits", "group__group__hal__psoc6__hw__types.html#a50a0b5cf864a709a12c53b57bf1f60e0", null ],
      [ "is_slave", "group__group__hal__psoc6__hw__types.html#adac2b2d71c8aa2918756ff15b7eed46f", null ],
      [ "alloc_clock", "group__group__hal__psoc6__hw__types.html#ae7b748656fdeb754c3d5e4fca97ce981", null ],
      [ "oversample_value", "group__group__hal__psoc6__hw__types.html#a1719642510d5c74a45fc5ed4e3af0b4c", null ],
      [ "msb_first", "group__group__hal__psoc6__hw__types.html#ad3b024dc55f4ba25ae9a002fdf6274ef", null ],
      [ "context", "group__group__hal__psoc6__hw__types.html#a9d3190844992b833c1985a0f70db0f47", null ],
      [ "irq_cause", "group__group__hal__psoc6__hw__types.html#aaad3adf0f7954bb7fee6e543220b2c38", null ],
      [ "pending", "group__group__hal__psoc6__hw__types.html#a98f79f2e0021c9dab3c5a727c1553a63", null ],
      [ "rx_buffer", "group__group__hal__psoc6__hw__types.html#ad61070aa55e039d0f02e7e4a9547bf49", null ],
      [ "rx_buffer_size", "group__group__hal__psoc6__hw__types.html#ae83a2c35e45a21c0bb078cfcf915b981", null ],
      [ "tx_buffer", "group__group__hal__psoc6__hw__types.html#a5b27c2bab5d173ffdc1187b153859f3c", null ],
      [ "tx_buffer_size", "group__group__hal__psoc6__hw__types.html#a68458b1ec4318a905b47e78874b9571d", null ],
      [ "is_async", "group__group__hal__psoc6__hw__types.html#ae332bc5a8e1965808c24eb36f7f4be01", null ],
      [ "callback_data", "group__group__hal__psoc6__hw__types.html#a81359f063c7f6603a9665c2af197595e", null ]
    ] ],
    [ "cyhal_timer_t", "group__group__hal__psoc6__hw__types.html#structcyhal__timer__t", [
      [ "base", "group__group__hal__psoc6__hw__types.html#a06c580b091f73b291fd52fff1b8f16eb", null ],
      [ "resource", "group__group__hal__psoc6__hw__types.html#a9352515090b84442156c98d65c0cac3f", null ],
      [ "pin", "group__group__hal__psoc6__hw__types.html#a6c796ef4668f891ea33fbd09138b305e", null ],
      [ "clock", "group__group__hal__psoc6__hw__types.html#a2ab7826d37d6e5b2ed22436640c38302", null ],
      [ "dedicated_clock", "group__group__hal__psoc6__hw__types.html#ae83e7455ede47cf0a73e40fc01627c52", null ],
      [ "clock_hz", "group__group__hal__psoc6__hw__types.html#a522c20468779112cd1ce3595a8a532ba", null ],
      [ "direction", "group__group__hal__psoc6__hw__types.html#ad8583a9cf35b822487188b3fabdf0bc3", null ],
      [ "is_continuous", "group__group__hal__psoc6__hw__types.html#a1d32e3b441be2be671ed2baa03875af4", null ],
      [ "is_compare", "group__group__hal__psoc6__hw__types.html#ad24ebfd94befdc37fed99fbbcc1fccb7", null ],
      [ "callback_data", "group__group__hal__psoc6__hw__types.html#aaeb99b486d72d0d37a3f61fcb2ba69cf", null ]
    ] ],
    [ "cyhal_uart_t", "group__group__hal__psoc6__hw__types.html#structcyhal__uart__t", [
      [ "base", "group__group__hal__psoc6__hw__types.html#adb1e03e9a708b3dd6aed68e6581a463a", null ],
      [ "resource", "group__group__hal__psoc6__hw__types.html#a8500bcc4df32b74911f7f2c7e620e609", null ],
      [ "pin_rx", "group__group__hal__psoc6__hw__types.html#aa211e6d348346ecb7391dfc07df98454", null ],
      [ "pin_tx", "group__group__hal__psoc6__hw__types.html#a934295a171024345bf34fb06d79572dc", null ],
      [ "pin_cts", "group__group__hal__psoc6__hw__types.html#a8b5b034b002f4e0f8070705fe4e79cc0", null ],
      [ "pin_rts", "group__group__hal__psoc6__hw__types.html#a7620d3f9a57ea1299c92d34229d126c3", null ],
      [ "is_user_clock", "group__group__hal__psoc6__hw__types.html#a4628cb953d5faf964d15c6f4cbb0257b", null ],
      [ "clock", "group__group__hal__psoc6__hw__types.html#a3c21ae841f36623755e3e15769177d4d", null ],
      [ "context", "group__group__hal__psoc6__hw__types.html#a697fcf1b077b74a133145af1c28ac2f5", null ],
      [ "config", "group__group__hal__psoc6__hw__types.html#ac3dcfd1fa7e11f70bab10f0576b7c290", null ],
      [ "irq_cause", "group__group__hal__psoc6__hw__types.html#a1f4bed812246890c7edf9827ab4a358d", null ],
      [ "pm_params", "group__group__hal__psoc6__hw__types.html#a92a8e7cb69ba644f4b54dac36ba38d68", null ],
      [ "pm_callback", "group__group__hal__psoc6__hw__types.html#ac075b7dc93ece7f4565db5f4bea23384", null ],
      [ "saved_tx_hsiom", "group__group__hal__psoc6__hw__types.html#a008b02a1e8dacd618e91a354bde6e2e8", null ],
      [ "saved_rts_hsiom", "group__group__hal__psoc6__hw__types.html#a5f630a9fe03003133e35606897f92c0e", null ],
      [ "callback_data", "group__group__hal__psoc6__hw__types.html#af4a8e8f055a31f3ed62aec9a50bc7fec", null ]
    ] ],
    [ "cyhal_usb_dev_t", "group__group__hal__psoc6__hw__types.html#structcyhal__usb__dev__t", [
      [ "base", "group__group__hal__psoc6__hw__types.html#a3c6b9b0f98859eff39ca7f376c2282e0", null ],
      [ "context", "group__group__hal__psoc6__hw__types.html#a71e9161fcac9326df1d83e026ede4ada", null ],
      [ "resource", "group__group__hal__psoc6__hw__types.html#ac15fc6729389743f48c21e8c3e68e52e", null ],
      [ "pll_resource", "group__group__hal__psoc6__hw__types.html#a3a276cf5df1c7b0cea53fe44e30f53a5", null ],
      [ "clock", "group__group__hal__psoc6__hw__types.html#ae5bdc10ae0628199a228cb541aa0c1cf", null ],
      [ "shared_clock", "group__group__hal__psoc6__hw__types.html#a4dd7bc3028d5e9d7bc4da5bb076d83ef", null ],
      [ "pin_dp", "group__group__hal__psoc6__hw__types.html#a3101a7d5d01e3075613ca3e6b0b911c0", null ],
      [ "pin_dm", "group__group__hal__psoc6__hw__types.html#adeffee8b006f521b524e47d58e99d643", null ],
      [ "rd_data", "group__group__hal__psoc6__hw__types.html#aa7973c6fcf336f6e2db8e0a123a7526d", null ],
      [ "rd_size", "group__group__hal__psoc6__hw__types.html#a2f579c1dfe2024e95f6a6d04a547796e", null ]
    ] ],
    [ "cyhal_wdt_t", "group__group__hal__psoc6__hw__types.html#structcyhal__wdt__t", [
      [ "placeholder", "group__group__hal__psoc6__hw__types.html#ab767afdb6a026741efa6e40457b3634e", null ]
    ] ],
    [ "CYHAL_ISR_PRIORITY_DEFAULT", "group__group__hal__psoc6__hw__types.html#gad822a466fc63847114add720ef84c83a", null ],
    [ "cyhal_system_callback_t", "group__group__hal__psoc6__hw__types.html#gacd10538852e7575732463b1f552b73d4", null ],
    [ "cyhal_source_t", "group__group__hal__psoc6__hw__types.html#gae6e4ea4100036d96d2efc006a3f69c9b", null ],
    [ "cyhal_clock_divider_types_t", "group__group__hal__psoc6__hw__types.html#gac29456963668507ecb7ecdf15e48b9d0", null ],
    [ "cyhal_system_divider_t", "group__group__hal__psoc6__hw__types.html#gae934ba770f2233f792a7de704bf1f59f", null ],
    [ "cyhal_resource_t", "group__group__hal__psoc6__hw__types.html#ga63b023ea7b5e73db59ddc0423c77975b", [
      [ "CYHAL_RSC_ADC", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975ba612bee18e656ba78a4ca633f8913249d", null ],
      [ "CYHAL_RSC_BLESS", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975bac10abc82f19b0a06a14a98e30a75b74d", null ],
      [ "CYHAL_RSC_CAN", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975ba1b775f74aa64eb24531a8669a6e25546", null ],
      [ "CYHAL_RSC_CLKPATH", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975ba59ccc8aae425c4249a6d44a211b9e13a", null ],
      [ "CYHAL_RSC_CLOCK", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975ba87c497b200dfbc0c5daf9617a89f4aa7", null ],
      [ "CYHAL_RSC_CRYPTO", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975ba25fa2cb684b519d6826067ce6d5d1afe", null ],
      [ "CYHAL_RSC_DAC", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975badad76bbe79d1dd504f44296427c38fe2", null ],
      [ "CYHAL_RSC_DMA", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975badd087e136a6661d42919ab8c145a395f", null ],
      [ "CYHAL_RSC_DW", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975ba1f6f00a8d9b758e0ea4a1ad8da3f5dd0", null ],
      [ "CYHAL_RSC_GPIO", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975ba5f4259788e0fc887772496df602d7549", null ],
      [ "CYHAL_RSC_I2S", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975ba6b93380724a9f1c695cf1f7a2e993a90", null ],
      [ "CYHAL_RSC_LCD", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975ba66e93a0a9834e5b16a3156d87ad993e4", null ],
      [ "CYHAL_RSC_LPCOMP", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975ba66b3400d94288b89d49def6194537bbe", null ],
      [ "CYHAL_RSC_LPTIMER", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975ba6408a108cf029e1e7b69c98ce86320e7", null ],
      [ "CYHAL_RSC_OPAMP", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975ba279192cbd81c5e71e5f4c3939d2e96a8", null ],
      [ "CYHAL_RSC_PDM", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975ba9123cdde61ea752aa7c362b3f9ca2814", null ],
      [ "CYHAL_RSC_SMIF", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975ba0b3f19bcd7698ff1293d1bcbccb31b77", null ],
      [ "CYHAL_RSC_RTC", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975ba65ea21d45fd38e900113b40eb3849236", null ],
      [ "CYHAL_RSC_SCB", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975bac1349fcb893be326bcd28ed254bd7a80", null ],
      [ "CYHAL_RSC_SDHC", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975baf35ab7112b4bcd054bdb0874a441155a", null ],
      [ "CYHAL_RSC_TCPWM", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975baa445456a2983f23d4be55613f2d09eb6", null ],
      [ "CYHAL_RSC_UDB", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975ba446a3390d41bd50e12d0b8f0c1715a53", null ],
      [ "CYHAL_RSC_USB", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975bae7778bb22af66e6dfb5c620d6c3c9108", null ],
      [ "CYHAL_RSC_INVALID", "group__group__hal__psoc6__hw__types.html#gga63b023ea7b5e73db59ddc0423c77975baf4c05e33bb994cc6a1d94bd301dcc988", null ]
    ] ],
    [ "cyhal_system_clock_t", "group__group__hal__psoc6__hw__types.html#ga996e3eaa0953a18334a13c2c47bc2abb", null ]
];