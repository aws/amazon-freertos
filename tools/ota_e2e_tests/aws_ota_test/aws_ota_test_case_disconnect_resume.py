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


class OtaTestDisconnectResume(OtaTestCase):
    """
    Happy path test for suspend and resume feature. The test will create a OTA update first, then
    connect to AWS IoT core with same client ID (thing name) and credentials after the OTA update
    started on device. AWS IoT core should then disconnect the device. Device is expected to detect
    it is disconnected from IoT core after a period of time, then it should put itself in suspend
    state and try to reconnect to the IoT core and resume the download.
    """

    is_positive = True
    connected_to_iot = False

    def get_job_exec_status(self, update_id, thing_name):
        iot_client = boto3.client('iot')
        exec_status = 'QUEUED'
        try:
            response = iot_client.describe_job_execution(jobId=f'AFR_OTA-{update_id}', thingName=thing_name)
            exec_status = response['execution']['status']
        except ClientError:
            print('Could not obtain job status yet')
        return exec_status

    def connect_to_iot(self, thing_name, certfile, keyfile):
        def on_connect(client, userdata, flags, rc):
            print('----------------------------------------\n'
                  'Connected to IoT core. Device should now disconnect then reconnect.\n'
                  '----------------------------------------')
            self.connected_to_iot = True
            mqtt_client.disconnect()
            mqtt_client.loop_stop()

        mqtt_client = mqtt.Client(client_id=thing_name)
        mqtt_client.tls_set(certfile=certfile, keyfile=keyfile)
        mqtt_client.on_connect = on_connect
        mqtt_client.connect_async(self._otaAwsAgent.getAwsIotEndpoint(), 8883, 60)
        mqtt_client.loop_start()

    def run(self):
        # Increase the version of the OTA image.
        self._otaProject.setApplicationVersion(0, 9, 1)
        # Build the OTA image.
        self._otaProject.buildProject()
        # Start an OTA Update.
        otaUpdateId = self._otaAwsAgent.quickCreateOtaUpdate(self._otaConfig, [self._protocol])

        # Wait until the job is in progress.
        thing_name = self._otaAwsAgent._iotThing.thing_name
        exec_status = 'QUEUED'
        timeout = time.time() + self._otaConfig['ota_timeout_sec']
        while exec_status == 'QUEUED' and time.time() < timeout:
            exec_status = self.get_job_exec_status(otaUpdateId, thing_name)
            time.sleep(1)
        if exec_status == 'QUEUED':
            return OtaTestResult(testName=self._name, result=OtaTestResult.ERROR,
                                 summary='Timeout waiting for OTA job status.')

        # Connect to IoT core with same thing name, this should disconnect the device from IoT core.
        # Note: currently there's no way to load the cert/key from memory, this is a limitation from
        # the python ssl module. We have to write them to temporary files.
        cert = Path('tmp_cert.pem')
        cert.write_text(self._otaAwsAgent._iotThing.cert)
        key = Path('tmp_key.pem')
        key.write_text(self._otaAwsAgent._iotThing.prv_key)
        self.connect_to_iot(thing_name, str(cert), str(key))

        # Wait up to 10 second until we connect to the IoT core
        timeout = time.time() + 10
        while not self.connected_to_iot and time.time() < timeout:
            time.sleep(1)

        # Delete the temporary credential files.
        cert.unlink()
        key.unlink()

        # Device should reconnect and OTA should succeed.
        if self.connected_to_iot:
            return self.getTestResultAfterOtaUpdateCompletion(otaUpdateId)
        else:
            return OtaTestResult(testName=self._name, result=OtaTestResult.ERROR,
                                 summary='Could not disconnect the device from IoT core.')
