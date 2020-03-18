var group__group__hal__i2c =
[
    [ "cyhal_i2c_cfg_t", "group__group__hal__i2c.html#structcyhal__i2c__cfg__t", [
      [ "is_slave", "group__group__hal__i2c.html#a734b40ed68843e48a0b2586e3fb58fcb", null ],
      [ "address", "group__group__hal__i2c.html#a7789fe25fd94e1ba74c7312f85fdb4cd", null ],
      [ "frequencyhal_hz", "group__group__hal__i2c.html#a4bc56f48688bf67f70d721fd7d94c16a", null ]
    ] ],
    [ "CYHAL_I2C_RSLT_ERR_INVALID_PIN", "group__group__hal__i2c.html#ga5b11d85404cfb697a5c9134069046b15", null ],
    [ "CYHAL_I2C_RSLT_ERR_CAN_NOT_REACH_DR", "group__group__hal__i2c.html#gaf787a4f4b1c9d9608a356384d456823c", null ],
    [ "CYHAL_I2C_RSLT_ERR_INVALID_ADDRESS_SIZE", "group__group__hal__i2c.html#ga765d128c16f9e753abf7c755aa336688", null ],
    [ "cyhal_i2c_event_callback_t", "group__group__hal__i2c.html#ga67df812388aae9fcc3b3b1986dab7a09", null ],
    [ "cyhal_i2c_event_t", "group__group__hal__i2c.html#gaaba7daccaa3861c32a4b0284e8194a79", [
      [ "CYHAL_I2C_EVENT_NONE", "group__group__hal__i2c.html#ggaaba7daccaa3861c32a4b0284e8194a79a80d8eb3997f062277769a958ff0b74bd", null ],
      [ "CYHAL_I2C_SLAVE_READ_EVENT", "group__group__hal__i2c.html#ggaaba7daccaa3861c32a4b0284e8194a79a08f85e87b6ee6690714adc58a8b4a426", null ],
      [ "CYHAL_I2C_SLAVE_WRITE_EVENT", "group__group__hal__i2c.html#ggaaba7daccaa3861c32a4b0284e8194a79af9d18fd8a6768ca60dbc475d3a08f13a", null ],
      [ "CYHAL_I2C_SLAVE_RD_IN_FIFO_EVENT", "group__group__hal__i2c.html#ggaaba7daccaa3861c32a4b0284e8194a79ad4a472c6da6121e533e871960a57d8b3", null ],
      [ "CYHAL_I2C_SLAVE_RD_BUF_EMPTY_EVENT", "group__group__hal__i2c.html#ggaaba7daccaa3861c32a4b0284e8194a79af1d3cec9a44b25d6d1434b9d21457cf2", null ],
      [ "CYHAL_I2C_SLAVE_RD_CMPLT_EVENT", "group__group__hal__i2c.html#ggaaba7daccaa3861c32a4b0284e8194a79af3743e0bae6fc016ae15741d69b765e8", null ],
      [ "CYHAL_I2C_SLAVE_WR_CMPLT_EVENT", "group__group__hal__i2c.html#ggaaba7daccaa3861c32a4b0284e8194a79aed43708fe51a3270699f80f21dde4bac", null ],
      [ "CYHAL_I2C_SLAVE_ERR_EVENT", "group__group__hal__i2c.html#ggaaba7daccaa3861c32a4b0284e8194a79aba7cb8e07c8f47bd41a74af1373c7d04", null ],
      [ "CYHAL_I2C_MASTER_WR_IN_FIFO_EVENT", "group__group__hal__i2c.html#ggaaba7daccaa3861c32a4b0284e8194a79a9fd6214feece0581282414f326fc3ef4", null ],
      [ "CYHAL_I2C_MASTER_WR_CMPLT_EVENT", "group__group__hal__i2c.html#ggaaba7daccaa3861c32a4b0284e8194a79a0365270204869bfe80a2015258c979e1", null ],
      [ "CYHAL_I2C_MASTER_RD_CMPLT_EVENT", "group__group__hal__i2c.html#ggaaba7daccaa3861c32a4b0284e8194a79ac5b8050af1c08b0d00a21539c5286435", null ],
      [ "CYHAL_I2C_MASTER_ERR_EVENT", "group__group__hal__i2c.html#ggaaba7daccaa3861c32a4b0284e8194a79a4bda40d48c2d7ca1c7556367b532920f", null ]
    ] ],
    [ "cyhal_i2c_init", "group__group__hal__i2c.html#ga6b7025575d2c859279007a81531b65c0", null ],
    [ "cyhal_i2c_free", "group__group__hal__i2c.html#ga3335fac7601fb3a6e4210d24e55cca61", null ],
    [ "cyhal_i2c_configure", "group__group__hal__i2c.html#ga37a35abcb9bf2d0ecf2a52b2fa5bce64", null ],
    [ "cyhal_i2c_master_write", "group__group__hal__i2c.html#gaf100c21cd95409c70e1ba0463c23e3f8", null ],
    [ "cyhal_i2c_master_read", "group__group__hal__i2c.html#ga0652a883293c83ebd0612fddbd8ff64d", null ],
    [ "cyhal_i2c_slave_config_write_buff", "group__group__hal__i2c.html#ga025cc832dd830a0dcef21da8781fea38", null ],
    [ "cyhal_i2c_slave_config_read_buff", "group__group__hal__i2c.html#ga6b354ba75f6d66aa5926e52d8e23db1a", null ],
    [ "cyhal_i2c_master_mem_write", "group__group__hal__i2c.html#gac9d0f4633fe8e121b93540d2415e4dd2", null ],
    [ "cyhal_i2c_master_mem_read", "group__group__hal__i2c.html#ga3b3c9ae9cd2a99bfb7fcd513c00a50bd", null ],
    [ "cyhal_i2c_master_transfer_async", "group__group__hal__i2c.html#gad6cd59cead5f7de0cf87571ceb656054", null ],
    [ "cyhal_i2c_abort_async", "group__group__hal__i2c.html#ga5ee41d9e5c6dc83376291ff5275c942a", null ],
    [ "cyhal_i2c_register_callback", "group__group__hal__i2c.html#gacf738716af52c484873e35331b9a30b7", null ],
    [ "cyhal_i2c_enable_event", "group__group__hal__i2c.html#ga92c7ef424d2e6fc7795c6f29a8ab3e76", null ]
];