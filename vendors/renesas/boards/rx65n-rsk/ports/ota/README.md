It is expected that the bootloader shall support self-test of a new OTA image. If test execution fails, the bootloader shall roll back to the previous valid image. If test execution succeeds, the image shall be marked valid and the previous version erased.

The current implementation of Renesas Bootloader does not support the self-test feature.  In case the new firmware file received has some functional issues, the device may stop working, even though the crypto signature of the firmware file is correct.

If you have any questions, then please reach out to Renesas: https://www.renesas.com/support/contact.html
