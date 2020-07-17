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
import os


class OtaTestIncorrectPlatform(OtaTestCase):
    """
    Different hardware supports different signing and hashing algorithm. This test verifies that
    device will fail the OTA update is it's created with non-supported algorithm.
    """

    is_positive = False

    def __init__(self, positive, boardConfig, otaProject, otaAwsAgent, flashComm, protocol):
        if boardConfig['ota_config']['aws_signer_platform'] == 'AmazonFreeRTOS-TI-CC3220SF':
            self._signingAlgorithm = 'ECDSA'
            self._hashingAlgorithm = 'SHA256'
        else:
            self._signingAlgorithm = 'RSA'
            self._hashingAlgorithm = 'SHA1'

        # Call base constructor.
        super().__init__(positive, boardConfig, otaProject, otaAwsAgent, flashComm, protocol)

    def run(self):
        # Increase the version of the OTA image.
        self._otaProject.setApplicationVersion(0, 9, 1)
        # Build the OTA image.
        self._otaProject.buildProject()

        # Run the test.
        # Upload the OTA image to s3.
        self._otaAwsAgent.uploadFirmwareToS3Bucket(
            self._otaConfig['ota_firmware_file_path'],
            os.path.basename(self._otaConfig['ota_firmware_file_path'])
        )
        # Create a stream.
        streamId = self._otaAwsAgent.createIotStream(
            self._otaAwsAgent.getS3BucketName(),
            os.path.basename(self._otaConfig['ota_firmware_file_path'])
        )
        # Create a job.
        otaUpdateId = self._otaAwsAgent.createOtaUpdate(
            protocols=[self._protocol],
            deploymentFiles=[
                {
                    'fileName': self._otaConfig['device_firmware_file_name'],
                    'fileVersion': '1',
                    'fileLocation': {
                        'stream': {
                            'streamId': streamId,
                            'fileId': 0
                        },
                    },
                    'codeSigning': {
                        "customCodeSigning": {
                            "signature": {
                                "inlineDocument": "IAmVeryTrusted"
                            },
                            "certificateChain": {
                                "certificateName": "VeryTrustworthyOrganization",
                                "inlineDocument": "TrustUs"
                            },
                            "hashAlgorithm": self._hashingAlgorithm,
                            "signatureAlgorithm": self._signingAlgorithm
                        }
                    }
                },
            ]
        )

        # Wait for the job to complete.
        return self.getTestResultAfterOtaUpdateCompletion(otaUpdateId)
