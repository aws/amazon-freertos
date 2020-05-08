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
import time


class OtaTestPresignedUrlExpired(OtaTestCase):
    """
    When OTA update is created, user can configure how long the S3 presigned url would expire. This
    test verifies that device is able to perform OTA even if it cannot finish the download when the
    url expires. The device is expected to request a new job document which contains a new url to
    resume the download.
    """

    is_positive = True

    @staticmethod
    def supported_protocols():
        return ['HTTP']

    def setup(self):
        # Change block size to 1 KB, this is to slow down the OTA update.
        # TODO, this varies from board to board, find a better way to slow down the OTA update.
        self._otaProject.setOtaBlockSize(1024)
        return super().setup()

    def teardown(self):
        # Change block size back to 4 KB.
        self._otaProject.setOtaBlockSize(4096)
        super().teardown()

    def run(self):
        # TODO, Create an OTA update with presigned URL expire time set to 1 min.
        self._otaProject.setApplicationVersion(0, 9, 1)
        self._otaProject.buildProject()
        otaUpdateId = self._otaAwsAgent.quickCreateOtaUpdate(self._otaConfig, ['HTTP'], urlExpired=60)

        # Monitor the OTA update with a timer.
        elapsed = time.time()
        jobStatus, summary = self._otaAwsAgent.pollOtaUpdateCompletion(otaUpdateId, self._otaConfig['ota_timeout_sec'])
        elapsed = time.time() - elapsed

        # Fail the test case if the elapsed is less than 1 min.
        if elapsed < 60:
            jobStatus.status = 'FAILED'
            summary = 'Test takes less than 1 min to finished'

        return OtaTestResult.testResultFromJobStatus(self.getName(), jobStatus, True, summary)
