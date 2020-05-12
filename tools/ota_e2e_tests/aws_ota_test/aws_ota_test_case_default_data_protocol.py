"""
FreeRTOS
Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

http://aws.amazon.com/freertos
http://www.FreeRTOS.org

"""
from .aws_ota_test_case import OtaTestCase
from .aws_ota_test_result import OtaTestResult


class OtaTestDefaultDataProtocol(OtaTestCase):
    """
    When device supports multiple data transfer protocols, it has a default one to use when the
    update is created with multiple data transfer protocols enabled. This test builds the initial
    image with MQTT as the default protocol, and flash to the device. Then it builds another image
    with HTTP as the default protocol, and uses it to create an OTA update with both MQTT and HTTP
    enabled. The device is expected to finish the update with MQTT. Then another update is created,
    the device is expected to finish this update with HTTP because the second image sets HTTP as
    default protocol.
    """

    is_positive = True

    @classmethod
    def generate_test_cases(cls, boardConfig, otaProject, otaAwsAgent, flashComm):
        return [cls(cls.is_positive, boardConfig, otaProject, otaAwsAgent, flashComm, 'MIXED')]

    # Override the setup to always turn on MQTT and HTTP. And set the primary data protocol to MQTT.
    def setup(self):
        self._otaProject.setHTTPConfig()
        self._otaProject.setOTAPrimaryDataProtocol('MQTT')
        return super().setup()

    def run(self):
        # Set default protocol to HTTP in the new image.
        self._otaProject.setApplicationVersion(0, 9, 1)
        self._otaProject.setOTAPrimaryDataProtocol('HTTP')
        self._otaProject.buildProject()

        # Create an OTA update with both protocols. Device should perform a successful OTA update
        # with MQTT.
        otaUpdateId = self._otaAwsAgent.quickCreateOtaUpdate(self._otaConfig, ['MQTT', 'HTTP'])
        jobStatus, summary = self._otaAwsAgent.pollOtaUpdateCompletion(otaUpdateId, self._otaConfig['ota_timeout_sec'])
        if jobStatus.status != 'SUCCEEDED':
            return OtaTestResult.testResultFromJobStatus(self.getName(), jobStatus, True, summary)

        # Create another OTA update with both protocols. Device should perform a successful OTA update
        # with HTTP.
        self._otaProject.setApplicationVersion(0, 9, 2)
        self._otaProject.buildProject()
        otaUpdateId = self._otaAwsAgent.quickCreateOtaUpdate(self._otaConfig, ['MQTT', 'HTTP'])
        jobStatus, summary = self._otaAwsAgent.pollOtaUpdateCompletion(otaUpdateId, self._otaConfig['ota_timeout_sec'])

        return OtaTestResult.testResultFromJobStatus(self.getName(), jobStatus, self._positive, summary)
