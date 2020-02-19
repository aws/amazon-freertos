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
from abc import ABC, abstractmethod
import os
import time
import errno
import traceback
import subprocess

from .aws_ota_test_result import OtaTestResult


class OtaTestCase(ABC):
    """OTA Test Case abstract class.
    Attributes:
        _name(str): Name of the testcase.
        _positive(bool): Positive or negative of the testcase.
        _otaConfig(dict): 'ota_config' in board.json.
        _otaProject(obj:OtaAfrProject): FreeRTOS source code resource.
        _otaAwsAgent(obj:OtaAwsAgent): AWS CLI for OTA resource.
        _flashComm(obj:FlashSerialComm): MCU Flash and Serial read resource.
    Methods:
        run() : abstract method
        getName()
        setup() : Sets up _otaProject as 0.9.0 should be overwritten if that is not desired.
        teardown()
        runTest()
        getTestResultAfterOtaUpdateCompletion()
    """
    def __init__(self, positive, boardConfig, otaProject, otaAwsAgent, flashComm, protocol):
        self._name = self.__class__.__name__
        self._positive = positive
        self._boardConfig = boardConfig
        self._otaConfig = boardConfig['ota_config']
        self._otaProject = otaProject
        self._otaAwsAgent = otaAwsAgent
        self._flashComm = flashComm
        self._protocol = protocol

        self._logFilePath = os.path.join(
            'logs',
            self._otaAwsAgent._boardName,
            self._otaAwsAgent._boardName + '.' +
            self._name + '.' +
            self._protocol + '.txt'
        )

    @staticmethod
    def supported_protocols():
        """Which protocols are supported in this test case, default is both MQTT and HTTP, can be
        overridden by subclass.
        """
        return ['MQTT', 'HTTP']

    @classmethod
    def generate_test_cases(cls, boardConfig, otaProject, otaAwsAgent, flashComm):
        """Generate test cases based on configurations, can be overridden by subclass
        """
        enabled_protocols = boardConfig['ota_config']['data_protocols']
        test_cases = []
        for protocol in enabled_protocols:
            if protocol in cls.supported_protocols():
                test_cases.append(cls(cls.is_positive, boardConfig, otaProject, otaAwsAgent, flashComm, protocol))

        return test_cases

    def getName(self):
        """Return the name of this test case.
        """
        return f'{self._name}_{self._protocol}'

    def setup(self):
        """Setup the OTA test.
        All the necessary setup. Optional method, but do call super setup if implementation is provided in sub-class.
        """
        self._otaProject.initializeOtaProject()
        self._otaProject.setClientCredentialsForAwsIotEndpoint(self._otaAwsAgent.getAwsIotEndpoint())
        self._otaProject.setClientCredentialsForWifi(self._boardConfig['wifi_ssid'], self._boardConfig['wifi_password'], self._boardConfig['wifi_security'])
        self._otaProject.setClientCredentialForThingName(self._otaAwsAgent.getThingName())
        self._otaProject.setClientCredentialKeys(self._otaAwsAgent.getThingCertificate(), self._otaAwsAgent.getThingPrivateKey())
        self._otaProject.copyCodesignerCertificateToBootloader(self._otaAwsAgent.getCodeSignerCertificateFromArn(self._otaConfig['aws_signer_certificate_arn']))
        self._otaProject.setMqttLogsOn()
        self._otaProject.setFreeRtosConfigNetworkInterface(self._boardConfig.get('windows_network_interface', 0))
        if self._otaConfig.get('compile_codesigner_certificate', False):
            self._otaProject.setCodesignerCertificate(self._otaAwsAgent.getCodeSignerCertificateFromArn(self._otaConfig['aws_signer_certificate_arn']))
        transportation = self._otaConfig.get('transportation')
        if transportation == 'ble':
            self._otaProject.setBleConfig()
        if 'HTTP' in self._protocol:
            self._otaProject.setHTTPConfig()
        self._otaProject.setApplicationVersion(0, 9, 0)

        buildReturnCode = self._otaProject.buildProject()
        flashReturnCode = self._flashComm.flashAndRead()

        return buildReturnCode + flashReturnCode

    def teardown(self):
        """ Tear down the OTA test.
        All the necessary cleanup. Optional method, but do call super teardown if implementation is provided in sub-class.
        """
        self._flashComm.stopSerialRead()

        # Call git to reset the source code.
        try:
          subprocess.run(['git', 'checkout', f'{self._boardConfig["afr_root"]}'])
        except OSError as e:
          print(f'Error reseting the source code: {e}')

    def runTest(self):
        """Run this OTA test case.
        """
        start = time.time()
        print(f'---------- Running {self._boardConfig["name"]} : {self.getName()} ----------')

        # Run the implemented runTest function
        logAppendage = ''
        try:
            # Run the implemented setup.
            returnCodes = self.setup()

            # Run the actual test.
            if all(p == 0 for p in returnCodes):
                testResult = self.run()
            else:
                testResult = OtaTestResult(testName=self.getName(), result=OtaTestResult.ERROR, summary='Building or flashing failed. Please check logs.')
        except Exception:
            logAppendage = traceback.format_exc()
            print(logAppendage)
            testResult = OtaTestResult(testName=self.getName(), result=OtaTestResult.ERROR, summary='Exception found during test execution. Please check logs.')

        # The test is finished save the log to the board's folder
        self.createTestLog(logAppendage)

        # Clean up
        self.teardown()

        print(f'---------- Finished {self._boardConfig["name"]} : {self.getName()} ----------')
        end = time.time()

        time.sleep(3) # Wait for the device log flashes completely.

        testResult.print(int(end - start))

        return testResult

    def createTestLog(self, appendage=''):
        """Create a log in the file system of the device output for this test case.
        """
        if not os.path.exists(os.path.dirname(self._logFilePath)):
            try:
                os.makedirs(os.path.dirname(self._logFilePath))
            except OSError as exc: # Guard against race condition
                if exc.errno != errno.EEXIST:
                    raise
        with open(self._logFilePath, 'w') as logFile:
            logFile.write(self._flashComm.getSerialLog() + appendage)

    @abstractmethod
    def run(self):
        """ Run the OTA test case setup by the jobID.
        Should create the desired OTA job here and wait on completion.
        """
        raise Exception("OtaTestCase::run is not implemented. Please provide the implementation.")

    def getTestResultAfterOtaUpdateCompletion(self, otaUpdateId):
        """Utility helper to poll for completion of the input job then stop reading
        from the serial port.
        Args:
            otaUpdateId(str): AWS IoT OTA Update ID to poll for completion status.
        """
        jobStatus, summary = self._otaAwsAgent.pollOtaUpdateCompletion(otaUpdateId, self._otaConfig['ota_timeout_sec'])
        return OtaTestResult.testResultFromJobStatus(self.getName(), jobStatus, self._positive, summary)
