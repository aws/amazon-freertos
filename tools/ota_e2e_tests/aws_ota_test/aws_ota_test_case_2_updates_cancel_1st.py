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
import time
from pathlib import Path
import boto3
from botocore.exceptions import ClientError
import paho.mqtt.client as mqtt
from .aws_ota_test_case import OtaTestCase
from .aws_ota_test_result import OtaTestResult


class OtaTest2UpdatesCancel1st(OtaTestCase):
    """
    This test will create 2 OTA updates in a row. When the device reports it is in progress
    downloading the first update, the test will force-cancel the first update. Device is expected
    to immediately abort the current update and pick up the second update, then finish it.
    """

    is_positive = True

    def get_job_exec_status(self, update_id, thing_name):
        iot_client = boto3.client('iot')
        exec_status = 'QUEUED'
        try:
            response = iot_client.describe_job_execution(jobId=f'AFR_OTA-{update_id}', thingName=thing_name)
            exec_status = response['execution']['status']
        except ClientError:
            print('Could not obtain job status yet')
        return exec_status

    def run(self):
        # Increase the version of the OTA image.
        self._otaProject.setApplicationVersion(0, 9, 1)
        # Build the OTA image.
        self._otaProject.buildProject()
        # Create 1st OTA update.
        otaUpdateId_1 = self._otaAwsAgent.quickCreateOtaUpdate(self._otaConfig, [self._protocol])
        # Prepare another image to be updated.
        self._otaProject.setApplicationVersion(0, 9, 2)
        self._otaProject.buildProject()
        # Create 2nd OTA update.
        otaUpdateId_2 = self._otaAwsAgent.quickCreateOtaUpdate(self._otaConfig, [self._protocol])

        # Wait until the job is in progress.
        thing_name = self._otaAwsAgent._iotThing.thing_name
        exec_status = 'QUEUED'
        timeout = time.time() + self._otaConfig['ota_timeout_sec']
        while exec_status == 'QUEUED' and time.time() < timeout:
            exec_status = self.get_job_exec_status(otaUpdateId_1, thing_name)
            time.sleep(1)
        if exec_status == 'QUEUED':
            return OtaTestResult(testName=self._name, result=OtaTestResult.ERROR,
                                 summary='Timeout waiting for OTA job status.')

        # Force cancel the first job that's in progress, device should pick up the 2nd update and succeed.
        iot_client = boto3.client('iot')
        iot_client.cancel_job_execution(jobId=f'AFR_OTA-{otaUpdateId_1}', thingName=thing_name, force=True)

        return self.getTestResultAfterOtaUpdateCompletion(otaUpdateId_2)
