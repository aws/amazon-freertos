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


class OtaTestBackToBackSwitchProtocol(OtaTestCase):
    """
    This test verifies that device is able to switch data transfer protocol between MQTT and HTTP.
    It first creates an OTA update over MQTT but with invalid signature to ensure that update is
    done via MQTT but device won't reboot. Then it creates another OTA update over HTTP with valid
    signature, device should be able to switch to HTTP to finish the update. Then the same process
    is repeated but with HTTP first, then MQTT.
    """

    is_positive = True

    @classmethod
    def generate_test_cases(cls, boardConfig, otaProject, otaAwsAgent, flashComm):
        return [cls(cls.is_positive, boardConfig, otaProject, otaAwsAgent, flashComm, 'MIXED')]

    # Override the setup to always turn on MQTT and HTTP.
    def setup(self):
        self._otaProject.setHTTPConfig()
        return super().setup()

    def _doOTAUpdate(self, protocol):
        otaUpdateId = self._otaAwsAgent.quickCreateOtaUpdate(self._otaConfig, [protocol])
        jobStatus, summary = self._otaAwsAgent.pollOtaUpdateCompletion(otaUpdateId, self._otaConfig['ota_timeout_sec'])
        return jobStatus, summary

    def _doInvalidOTAUpdate(self, protocol):
        self._otaAwsAgent.uploadFirmwareToS3Bucket(
            self._otaConfig['ota_firmware_file_path'],
            self._otaConfig['device_firmware_file_name']
        )

        # Create an OTA job with custom code signing using an invalid signature.
        hashingAlgorithm = 'SHA1' if 'cc3220' in self._boardConfig['name'] else 'SHA256'
        signingAlgorithm = 'RSA' if 'cc3220' in self._boardConfig['name'] else 'ECDSA'
        otaUpdateId = self._otaAwsAgent.createOtaUpdate(
            protocols=[protocol],
            deploymentFiles=[
                {
                    'fileName': self._otaConfig['device_firmware_file_name'],
                    'fileVersion': '1',
                    'fileLocation': {
                        's3Location': {
                            'bucket': self._otaAwsAgent.getS3BucketName(),
                            'key': self._otaConfig['device_firmware_file_name']
                        },
                    },
                    'codeSigning': {
                        "customCodeSigning": {
                            "signature": {
                                "inlineDocument": "DummySignature"
                            },
                            "certificateChain": {
                                "certificateName": self._otaConfig["aws_signer_certificate_file_name"],
                                "inlineDocument": "TrustUs"
                            },
                            "hashAlgorithm": hashingAlgorithm,
                            "signatureAlgorithm": signingAlgorithm
                        }
                    }
                },
            ]
        )
        jobStatus, summary = self._otaAwsAgent.pollOtaUpdateCompletion(otaUpdateId, self._otaConfig['ota_timeout_sec'])
        return jobStatus, summary

    def run(self):
        # Build a newer version.
        self._otaProject.setApplicationVersion(0, 9, 1)
        self._otaProject.buildProject()

        # Do OTA with an invalid signature with MQTT, expect to fail.
        jobStatus, summary = self._doInvalidOTAUpdate("MQTT")
        if jobStatus.status == 'SUCCEEDED':
            return OtaTestResult.testResultFromJobStatus(self.getName(), jobStatus, False, summary)

        # Do OTA with a new version with HTTP, expect to succeed.
        jobStatus, summary = self._doOTAUpdate("HTTP")
        if jobStatus.status != 'SUCCEEDED':
            return OtaTestResult.testResultFromJobStatus(self.getName(), jobStatus, True, summary)

        # Build newer version.
        self._otaProject.setApplicationVersion(0, 9, 2)
        self._otaProject.buildProject()

        # Do OTA with an invalid signature with HTTP, expect to fail.
        jobStatus, summary = self._doInvalidOTAUpdate("HTTP")
        if jobStatus.status == 'SUCCEEDED':
            return OtaTestResult.testResultFromJobStatus(self.getName(), jobStatus, False, summary)

        # Do OTA with a new version with MQTT, expect to succeed.
        jobStatus, summary = self._doOTAUpdate("MQTT")

        return OtaTestResult.testResultFromJobStatus(self.getName(), jobStatus, True, summary)
