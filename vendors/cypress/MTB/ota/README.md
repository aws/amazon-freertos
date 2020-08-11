# PSoC 64 OTA Updates

PSoC® 64 devices have passed all of the required FreeRTOS qualification tests. However, the optional OTA feature implemented in the PSoC 64 Standard Secure AWS firmware library is still pending evaluation. The OTA feature as-implemented currently passes all of the OTA qualification tests except [“aws_ota_test_case_rollback_if_unable_to_connect_after_update.py”](https://github.com/aws/amazon-freertos/tree/202007.00/tools/ota_e2e_tests/aws_ota_test/aws_ota_test_case_rollback_if_unable_to_connect_after_update.py). 

When a successfully validated OTA image is applied to a device using the PSoC64 Standard Secure AWS MCU, and the device is unable to communicate with AWS IoT Core, the device will not be able to automatically rollback to the original known good image. This may result in the device being unreachable from AWS IoT Core for any further updates. This functionality is still under development by the Cypress team. 

For more information, please refer to [OTA Updates with AWS and the CY8CKIT-064S0S2-4343W Kit](https://community.cypress.com/docs/DOC-20063).  If you have further questions or need technical support, please contact the [Cypress Developer Community](https://community.cypress.com/community/software-forums/modustoolbox-amazon-freertos-sdk).
