var iot__gpio_8h =
[
    [ "IOT_GPIO_SUCCESS", "group__iot__gpio.html#ga1835bd94dbd1bb48c038122f28281845", null ],
    [ "IOT_GPIO_INVALID_VALUE", "group__iot__gpio.html#gade75c8ba369322d17c45bab8d0ee5054", null ],
    [ "IOT_GPIO_READ_FAILED", "group__iot__gpio.html#gafd43771b6ddb810266483ae6bd87e49b", null ],
    [ "IOT_GPIO_WRITE_FAILED", "group__iot__gpio.html#ga60ad4a899e0bd06bf12cb9a22703a160", null ],
    [ "IOT_GPIO_FUNCTION_NOT_SUPPORTED", "group__iot__gpio.html#gab6f99c0e56b0b0fb18425c419b1c8de2", null ],
    [ "IotGpioHandle_t", "group__iot__gpio.html#ga0c08d5cfd7fb6efd5fd938eec68740ee", null ],
    [ "IotGpioCallback_t", "group__iot__gpio.html#ga22bc82b84aa0804a9b9e67eb51da983f", null ],
    [ "IotGpioDirection_t", "group__iot__gpio.html#gafd7b3b48e6e4236fd122e5198be22280", [
      [ "eGpioDirectionInput", "group__iot__gpio.html#ggafd7b3b48e6e4236fd122e5198be22280a3e885d41bb4b836ef13903f1605b0503", null ],
      [ "eGpioDirectionOutput", "group__iot__gpio.html#ggafd7b3b48e6e4236fd122e5198be22280a4108f41e83891b5fcf35843a82fea036", null ]
    ] ],
    [ "IotGpioOutputMode_t", "group__iot__gpio.html#ga3bd66b5efe467d36d9c1e2d196f633e3", [
      [ "eGpioOpenDrain", "group__iot__gpio.html#gga3bd66b5efe467d36d9c1e2d196f633e3a6ce9c687476fbd75c21b3b37eb608a9a", null ],
      [ "eGpioPushPull", "group__iot__gpio.html#gga3bd66b5efe467d36d9c1e2d196f633e3a4c8b59fd4dcc2bdcdc334cff4227589e", null ]
    ] ],
    [ "IotGpioPull_t", "group__iot__gpio.html#ga8d04456c6805020fdf3783ce90cf70a7", [
      [ "eGpioPullNone", "group__iot__gpio.html#gga8d04456c6805020fdf3783ce90cf70a7a852e7e472c6c9ab65f3a555a11f29ab4", null ],
      [ "eGpioPullUp", "group__iot__gpio.html#gga8d04456c6805020fdf3783ce90cf70a7a45e08b5c704439abd5769e0fca2b9ffc", null ],
      [ "eGpioPullDown", "group__iot__gpio.html#gga8d04456c6805020fdf3783ce90cf70a7a6bebabe3d271872d3c7f19b9f6959a03", null ]
    ] ],
    [ "IotGpioInterrupt_t", "group__iot__gpio.html#gae5571853fb61c2f0946d9c5e4c041f78", [
      [ "eGpioInterruptNone", "group__iot__gpio.html#ggae5571853fb61c2f0946d9c5e4c041f78a3a5bbe113cac022bd0d3231c29aa104b", null ],
      [ "eGpioInterruptRising", "group__iot__gpio.html#ggae5571853fb61c2f0946d9c5e4c041f78ab187f0879a4aeed54af635c572d50d75", null ],
      [ "eGpioInterruptFalling", "group__iot__gpio.html#ggae5571853fb61c2f0946d9c5e4c041f78af2bf1bfca83ab1fda8c32fae00257e0d", null ],
      [ "eGpioInterruptEdge", "group__iot__gpio.html#ggae5571853fb61c2f0946d9c5e4c041f78ad9d4aa02469b87eaee125e376a08a724", null ],
      [ "eGpioInterruptLow", "group__iot__gpio.html#ggae5571853fb61c2f0946d9c5e4c041f78a07b1a06dcc9b0e9ff28d2f35504a6e2f", null ],
      [ "eGpioInterruptHigh", "group__iot__gpio.html#ggae5571853fb61c2f0946d9c5e4c041f78a5a4f65c815b41e5351b65249df494bab", null ]
    ] ],
    [ "IotGpioIoctlRequest_t", "group__iot__gpio.html#gad0cdaf905321e5fda18fc5802b7afafe", [
      [ "eSetGpioFunction", "group__iot__gpio.html#ggad0cdaf905321e5fda18fc5802b7afafea8b9018ecd3830281b449ed7b27bd23fb", null ],
      [ "eSetGpioDirection", "group__iot__gpio.html#ggad0cdaf905321e5fda18fc5802b7afafea8d510d41627dc009da0d308d5573b187", null ],
      [ "eSetGpioPull", "group__iot__gpio.html#ggad0cdaf905321e5fda18fc5802b7afafeafce415dc2bc2bfd5c16b55ff5d7c2eee", null ],
      [ "eSetGpioOutputMode", "group__iot__gpio.html#ggad0cdaf905321e5fda18fc5802b7afafea3e68c0c06fdbdd5bedbc5de738458b57", null ],
      [ "eSetGpioInterrupt", "group__iot__gpio.html#ggad0cdaf905321e5fda18fc5802b7afafea2f3029fa1bfae1272b755877ebdbb83a", null ],
      [ "eSetGpioSpeed", "group__iot__gpio.html#ggad0cdaf905321e5fda18fc5802b7afafeabc03fb7341139b9726a25382300c8473", null ],
      [ "eSetGpioDriveStrength", "group__iot__gpio.html#ggad0cdaf905321e5fda18fc5802b7afafea4b10ed52b1342bae3b5cf50b73139f92", null ],
      [ "eGetGpioFunction", "group__iot__gpio.html#ggad0cdaf905321e5fda18fc5802b7afafea1125eef413fc0e27610729bfb5844491", null ],
      [ "eGetGpioDirection", "group__iot__gpio.html#ggad0cdaf905321e5fda18fc5802b7afafeab5740ebc53702233dfb1325ab51bda77", null ],
      [ "eGetGpioPull", "group__iot__gpio.html#ggad0cdaf905321e5fda18fc5802b7afafeaee8f0e00b2c302070ce05ee17144c355", null ],
      [ "eGetGpioOutputType", "group__iot__gpio.html#ggad0cdaf905321e5fda18fc5802b7afafeaacf59908bb9a081aeafee0f77814daaa", null ],
      [ "eGetGpioInterrupt", "group__iot__gpio.html#ggad0cdaf905321e5fda18fc5802b7afafea6654a4bc6be4bdce2a93e1279c7ce7e9", null ],
      [ "eGetGpioSpeed", "group__iot__gpio.html#ggad0cdaf905321e5fda18fc5802b7afafeaaaeafc7dd60ba224eb69e5b76c9e6f3d", null ],
      [ "eGetGpioDriveStrength", "group__iot__gpio.html#ggad0cdaf905321e5fda18fc5802b7afafea1327ede758ca43017c4cbc1047703d46", null ]
    ] ],
    [ "iot_gpio_open", "group__iot__gpio.html#ga0298cd792cd9ed8de8c8fbf79711cbc8", null ],
    [ "iot_gpio_set_callback", "group__iot__gpio.html#ga36b379d49e4ffda207f9abb3ebe4ae81", null ],
    [ "iot_gpio_read_sync", "group__iot__gpio.html#gaef2b3a7997b274a0584ce963e7752677", null ],
    [ "iot_gpio_write_sync", "group__iot__gpio.html#gaddb584785ec4c861d33f87a7fb8b1780", null ],
    [ "iot_gpio_close", "group__iot__gpio.html#ga7e72b7e985409a0801a65c6c1152f71b", null ],
    [ "iot_gpio_ioctl", "group__iot__gpio.html#gaf12ac9d33f23b6920bc46ffdb3628e26", null ]
];