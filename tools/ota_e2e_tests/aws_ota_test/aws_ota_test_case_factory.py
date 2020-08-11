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
from .aws_ota_test_case_greater_version import OtaTestGreaterVersion
from .aws_ota_test_case_back_to_back_switch_protocol import OtaTestBackToBackSwitchProtocol
from .aws_ota_test_case_default_data_protocol import OtaTestDefaultDataProtocol
from .aws_ota_test_case_disconnect_cancel_update import OtaTestDisconnectCancelUpdate
from .aws_ota_test_case_disconnect_resume import OtaTestDisconnectResume
from .aws_ota_test_case_presigned_url_expired import OtaTestPresignedUrlExpired
from .aws_ota_test_case_unsigned_image import OtaTestUnsignedImage
from .aws_ota_test_case_same_version import OtaTestSameVersion
from .aws_ota_test_case_untrusted_certificate import OtaTestUntrustedCertificate
from .aws_ota_test_case_previous_version import OtaTestPreviousVersion
from .aws_ota_test_case_single_byte_image import OtaTestSingleByteImage
from .aws_ota_test_case_missing_filename import OtaTestMissingFilename
from .aws_ota_test_case_incorrect_platform import OtaTestIncorrectPlatform
from .aws_ota_test_case_back_to_back_downloads import OtaTestBackToBackDownloads
from .aws_ota_test_case_rollback_if_unable_to_connect_after_update import OtaTestRollbackIfUnableToConnectAfterUpdate
from .aws_ota_test_case_greater_version_min_block_config import OtaTestGreaterVersionMinBlockConfig
from .aws_ota_test_case_greater_version_max_block_config import OtaTestGreaterVersionMaxBlockConfig
from .aws_ota_test_case_dummy_test import OtaTestDummyTest


class OtaTestCaseFactory:
    """Return OTA test case objects based on the test name.
    """
    @staticmethod
    def createTestCases(test_name, boardConfig, otaProject, otaAwsAgent, flashComm):
        return globals()[test_name].generate_test_cases(boardConfig, otaProject, otaAwsAgent, flashComm)
