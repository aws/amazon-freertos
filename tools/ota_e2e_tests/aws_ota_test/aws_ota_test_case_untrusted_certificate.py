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


class OtaTestUntrustedCertificate(OtaTestCase):
    """
    This test verifies that device will reject an update if the firmware is signed with an untrusted
    certificate.
    """

    is_positive = False

    def __init__(self, positive, boardConfig, otaProject, otaAwsAgent, flashComm, protocol):
        # Define invalid/valid signing certificates for later use.
        self._validSignerArn = boardConfig['ota_config']['aws_signer_certificate_arn']
        self._bogusSignerArn = boardConfig['ota_config']['aws_untrusted_signer_certificate_arn']
        # Set a semi-unique 'signing type' name based on the invalid certificate's ARN to avoid conflicts.
        self._signingProfileName = f'{self._bogusSignerArn[-10:]}{otaAwsAgent._boardName[:10]}'

        # Call base constructor.
        super().__init__(positive, boardConfig, otaProject, otaAwsAgent, flashComm, protocol)

    def run(self):
        # Increase the version of the OTA image.
        self._otaProject.setApplicationVersion(0, 9, 1)
        # Use an invalid signing certificate.
        self._otaConfig['aws_signer_certificate_arn'] = self._bogusSignerArn
        # Build the OTA image.
        self._otaProject.buildProject()
        # Run the test.

        # Upload the OTA image to s3.
        self._otaAwsAgent.uploadFirmwareToS3Bucket(
            self._otaConfig['ota_firmware_file_path'],
            os.path.basename(self._otaConfig['ota_firmware_file_path'])
        )
        # Create a job.
        if self._otaAwsAgent._stageParams:
            otaUpdateId = self._otaAwsAgent.createOtaUpdate(
                protocols=[self._protocol],
                deploymentFiles=[
                    {
                        'fileName': os.path.basename(self._otaConfig['ota_firmware_file_path']),
                        'fileVersion': '1',
                        'fileLocation': {
                            's3Location': {
                                'bucket': self._otaConfig['aws_s3_bucket_name'],
                                'key': os.path.basename(self._otaConfig['ota_firmware_file_path']),
                                'version': self._otaAwsAgent.getS3ObjectVersion(os.path.basename(self._otaConfig['ota_firmware_file_path']))
                            }
                        },
                        'codeSigning': {
                            "startSigningJobParameter": {
                                'signingProfileName': self._signingProfileName,
                                'signingProfileParameter': {
                                    'platform': self._otaConfig['aws_signer_platform'],
                                    'certificateArn': self._otaConfig['aws_signer_certificate_arn'],
                                    'certificatePathOnDevice': self._otaConfig['aws_signer_certificate_file_name']
                                }
                            }
                        }
                    },
                ]
            )
        else:
            # Sign the corrupted image in s3 from the unsigned bucket to the signed bucket.
            signerJobId = self._otaAwsAgent.signFirmwareInS3Bucket(
                os.path.basename(self._otaConfig['ota_firmware_file_path']),
                self._otaConfig['aws_signer_certificate_arn'],
                self._otaConfig['aws_signer_certificate_file_name'],
                self._otaConfig['aws_signer_platform'],
                self._signingProfileName
            )
            # Create a stream from the image in the signed image in the signed bucket.
            streamId = self._otaAwsAgent.createIotStream(
                self._otaAwsAgent.getS3BucketName(),
                signerJobId
            )
            # Create the OTA update job.
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
                            'awsSignerJobId': signerJobId
                        }
                    },
                ]
            )

        # Reset to a valid signing key.
        self._otaConfig['aws_signer_certificate_arn'] = self._validSignerArn
        # Wait for the job to complete.
        return self.getTestResultAfterOtaUpdateCompletion(otaUpdateId)
