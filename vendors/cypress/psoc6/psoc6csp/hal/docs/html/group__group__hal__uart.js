var group__group__hal__uart =
[
    [ "cyhal_uart_cfg_t", "group__group__hal__uart.html#structcyhal__uart__cfg__t", [
      [ "data_bits", "group__group__hal__uart.html#a3360061aafad236eb9a9d347f9154643", null ],
      [ "stop_bits", "group__group__hal__uart.html#a6864a54db95005a564fd8617f9029e20", null ],
      [ "parity", "group__group__hal__uart.html#a70b51afc79e56af7360a9b35de3fdb0e", null ],
      [ "rx_buffer", "group__group__hal__uart.html#abf31b7706f129f2ea59471ba7fb6835a", null ],
      [ "rx_buffer_size", "group__group__hal__uart.html#a3ab4d4e2c33ec8ba1a0943e1f24d61f1", null ]
    ] ],
    [ "CYHAL_UART_RSLT_ERR_INVALID_PIN", "group__group__hal__uart.html#ga10dd6d5d7975757a4d900ce6da3b002e", null ],
    [ "CYHAL_UART_RSLT_ERR_PM_CALLBACK", "group__group__hal__uart.html#ga7f6b37c70d5a2e1f067c8248f631d4ab", null ],
    [ "CY_RSLT_ERR_CSP_UART_GETC_TIMEOUT", "group__group__hal__uart.html#ga264e4adc4d1c01e4c92aa93680e928ca", null ],
    [ "CY_RSLT_WRN_CSP_UART_BAUD_TOLERANCE", "group__group__hal__uart.html#gab9e6c49a6d0947c7dc1594bb424555be", null ],
    [ "CYHAL_UART_DEFAULT_BAUD", "group__group__hal__uart.html#ga4b2c1534032adfc825026b5882fa2cd6", null ],
    [ "CYHAL_UART_MAX_BAUD_PERCENT_DIFFERENCE", "group__group__hal__uart.html#ga9c1580a18947e5e976a997767b35b2c6", null ],
    [ "cyhal_uart_event_callback_t", "group__group__hal__uart.html#gac444f4c849b289d99a75e0132f04fb65", null ],
    [ "cyhal_uart_parity_t", "group__group__hal__uart.html#ga410f06ac3fe5576220caf146fda6b77b", [
      [ "CYHAL_UART_PARITY_NONE", "group__group__hal__uart.html#gga410f06ac3fe5576220caf146fda6b77ba327098a8b49fceb10e330d561019d8cb", null ],
      [ "CYHAL_UART_PARITY_EVEN", "group__group__hal__uart.html#gga410f06ac3fe5576220caf146fda6b77ba47c0ca4d5811d418e1bdaea54da62f7f", null ],
      [ "CYHAL_UART_PARITY_ODD", "group__group__hal__uart.html#gga410f06ac3fe5576220caf146fda6b77bad3844eeec5198449f882f5b3509282ea", null ]
    ] ],
    [ "cyhal_uart_event_t", "group__group__hal__uart.html#gaf307283f3d358e4b6048846034df19aa", [
      [ "CYHAL_UART_IRQ_NONE", "group__group__hal__uart.html#ggaf307283f3d358e4b6048846034df19aaadc15f333858441e62e2de2408a5ac767", null ],
      [ "CYHAL_UART_IRQ_TX_TRANSMIT_IN_FIFO", "group__group__hal__uart.html#ggaf307283f3d358e4b6048846034df19aaaafb0642f2b2672db3ab4446e4c075864", null ],
      [ "CYHAL_UART_IRQ_TX_DONE", "group__group__hal__uart.html#ggaf307283f3d358e4b6048846034df19aaa656718b2b8343a46a64478b1d415bfc4", null ],
      [ "CYHAL_UART_IRQ_TX_ERROR", "group__group__hal__uart.html#ggaf307283f3d358e4b6048846034df19aaa9984c9f5e7f3b57c9740d390c35388db", null ],
      [ "CYHAL_UART_IRQ_RX_FULL", "group__group__hal__uart.html#ggaf307283f3d358e4b6048846034df19aaa7d073741825cc32b1be09fa59d6c8123", null ],
      [ "CYHAL_UART_IRQ_RX_DONE", "group__group__hal__uart.html#ggaf307283f3d358e4b6048846034df19aaac2db86894106691360ad64c35d33b578", null ],
      [ "CYHAL_UART_IRQ_RX_ERROR", "group__group__hal__uart.html#ggaf307283f3d358e4b6048846034df19aaa7f5d1f47a602c8ac0df06c121bf4ce46", null ],
      [ "CYHAL_UART_IRQ_RX_NOT_EMPTY", "group__group__hal__uart.html#ggaf307283f3d358e4b6048846034df19aaa9b64ead890054ee5943b58bafc0f367e", null ],
      [ "CYHAL_UART_IRQ_TX_EMPTY", "group__group__hal__uart.html#ggaf307283f3d358e4b6048846034df19aaa0c7868cf2a2719db7fc2cb8c357e47a9", null ]
    ] ],
    [ "cyhal_uart_init", "group__group__hal__uart.html#gadfd25c5e408ee377148b492e64fbadaf", null ],
    [ "cyhal_uart_free", "group__group__hal__uart.html#ga51731996aca653fc76caee4cc7ad684f", null ],
    [ "cyhal_uart_set_baud", "group__group__hal__uart.html#ga14dcad3682e9b49e70b7f1e77cd862cc", null ],
    [ "cyhal_uart_configure", "group__group__hal__uart.html#ga4af8ba6ff0012214be498a001f1ac782", null ],
    [ "cyhal_uart_getc", "group__group__hal__uart.html#ga89108b2d339dc9863ec660588e3a4a12", null ],
    [ "cyhal_uart_putc", "group__group__hal__uart.html#ga2184202b0b3995fcf625caf9f8c3dc05", null ],
    [ "cyhal_uart_readable", "group__group__hal__uart.html#gad85e26dd47eecd9705ab1931253c20f1", null ],
    [ "cyhal_uart_writable", "group__group__hal__uart.html#ga760296fe084b3e0ab3cdd1fba745a8b3", null ],
    [ "cyhal_uart_clear", "group__group__hal__uart.html#gadc1932516baa1bd7285527a5a7456d75", null ],
    [ "cyhal_uart_set_flow_control", "group__group__hal__uart.html#ga318d61213f3485bbfbb723243f6a6b61", null ],
    [ "cyhal_uart_write", "group__group__hal__uart.html#ga2be8141bcc5fcc349b0dce1c09c4e98f", null ],
    [ "cyhal_uart_read", "group__group__hal__uart.html#gae436ac5108e3bc92f439c66c315e07ca", null ],
    [ "cyhal_uart_write_async", "group__group__hal__uart.html#gab569e52660c519b775e3a15f3e2dbb46", null ],
    [ "cyhal_uart_read_async", "group__group__hal__uart.html#ga00ef108f7ee7beba3d5090b2e506b54f", null ],
    [ "cyhal_uart_is_tx_active", "group__group__hal__uart.html#ga5ee15173efae0edfa0813e8b78a1a467", null ],
    [ "cyhal_uart_is_rx_active", "group__group__hal__uart.html#ga84bd3dc69297980c77697dcecbac4a57", null ],
    [ "cyhal_uart_write_abort", "group__group__hal__uart.html#gaa143498ce81a014c19bcd1e8ad43a3f6", null ],
    [ "cyhal_uart_read_abort", "group__group__hal__uart.html#ga94980c5a10527027be1da8e11bc34a98", null ],
    [ "cyhal_uart_register_callback", "group__group__hal__uart.html#gae26bab64811713b1d69170352fe32c20", null ],
    [ "cyhal_uart_enable_event", "group__group__hal__uart.html#ga60c4d315ea758449334ce17d29629651", null ]
];