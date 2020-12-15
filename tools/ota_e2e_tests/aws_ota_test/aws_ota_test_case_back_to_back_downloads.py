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


class OtaTestBackToBackDownloads(OtaTestCase):
    """
    This test creates 3 consecutive OTA updates. The device is expected to update 3 times in a row.
    """

    is_positive = True

    def __buildAndOtaInputVersion(self, x, y, z):
        # Build x.y.z for download
        self._otaProject.setApplicationVersion(x, y, z)
        # Build the OTA image.
        self._otaProject.buildProject()
        # Start an OTA Update.
        otaUpdateId = self._otaAwsAgent.quickCreateOtaUpdate(self._otaConfig, [self._protocol])
        # Poll on completion
        jobStatus, summary = self._otaAwsAgent.pollOtaUpdateCompletion(otaUpdateId, self._otaConfig['ota_timeout_sec'])
        return jobStatus, summary

    def run(self):
        # Build 0.9.1 for download
        jobStatus, summary = self.__buildAndOtaInputVersion(0, 9, 1)
        if jobStatus.status != 'SUCCEEDED':
            return OtaTestResult.testResultFromJobStatus(self.getName(), jobStatus, self._positive, summary)

        # Build 0.9.2 for download
        jobStatus, summary = self.__buildAndOtaInputVersion(0, 9, 2)
        if jobStatus.status != 'SUCCEEDED':
            return OtaTestResult.testResultFromJobStatus(self.getName(), jobStatus, self._positive, summary)

        # Build 0.9.3 for download
        jobStatus, summary = self.__buildAndOtaInputVersion(0, 9, 3)
        if jobStatus.status != 'SUCCEEDED':
            return OtaTestResult.testResultFromJobStatus(self.getName(), jobStatus, self._positive, summary)

        return OtaTestResult.testResultFromJobStatus(self.getName(), jobStatus, self._positive, summary)
