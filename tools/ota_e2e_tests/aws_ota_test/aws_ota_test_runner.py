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
import os
import traceback
from .aws_flash_serial_comm import FlashSerialComm
from .aws_ota_project import OtaAfrProject
from .aws_ota_aws_agent import OtaAwsAgent
from .aws_ota_test_case_factory import OtaTestCaseFactory
from .aws_ota_test_result import OtaTestResult

class OtaTestRunner:
    """Run all of the OTA tests.
    Before we run the tests we need to initialize all of the resource needed by the tests.
    There is one test runner per board.
    Args:
        boardConfig(dict): The full board.json configuration.
        stage(dict): What development environment stage the AWS service stack is in to run OTA tests.
    Attributes:
        _flashComm(obj:FlashSerialComm): MCU flash and serial communication resource.
        _otaProject(obj:OtaAfrProject): FreeRTOS code resource.
        _otaAwsAgent(obj:OtaAwsAgent): Interface to AWS CLI resource.
        _otaTestCases(list(obj:OtaTestCase)): All of the OTA test cases in this run.
    Methods:
        runTests(): Run all of the tests found in the boardConfig during initialization.
        __initializeOtaProject(): Initialize _otaProject resource for testing.
        __getOtaTestCases(_otaConfig): Get all of the tests supported by the input board.
        __runTest(otaTestCase): Run a single test case.
        __cleanup(): Clean up all resource created during initialization.
    Example:
        testRunner = OtaTestRunner(boardConfig)
        testRunner.runTests()
    """
    def __init__(self, boardConfig, stageParams):
        # Override AWS credentials if provided in config.
        if boardConfig.get('aws_access_id'):
            os.environ['AWS_ACCESS_KEY_ID'] = boardConfig['aws_access_id']
        if boardConfig.get('aws_secret_key'):
            os.environ['AWS_SECRET_ACCESS_KEY'] = boardConfig['aws_secret_key']
        if boardConfig.get('aws_region_name'):
            os.environ['AWS_DEFAULT_REGION'] = boardConfig['aws_region_name']

        self._boardConfig = boardConfig
        self._stageParams = stageParams
        self._otaConfig = boardConfig['ota_config']
        self._otaProject = OtaAfrProject(boardConfig)
        # OTA jobs only accept underscore and no dots. We replace dots with underscores for all IoT Core related names.
        self._otaAwsAgent = OtaAwsAgent(self._boardConfig['name'].replace('.', '_'), self._otaConfig, stageParams, True)
        # FlashSerialComm opens a thread. If there is an exception in OtaAwsAgent we want to exit the program, so this is initialized last.
        self._flashComm = FlashSerialComm(boardConfig['flash_config'], boardConfig['flash_config']['output'], self._otaConfig['device_firmware_file_name'])

        # Get the test cases from the board's ota config.
        self._otaTestCases = self.__getOtaTestCases(self._otaConfig)

    def __getOtaTestCases(self, _otaConfig):
        """Get all of the tests supported by the input board.
        Args:
            _otaConfig(dict): 'ota_config' in board.json containing the 'supported_tests' key.
        Returns a list of OtaTestCase objects.
        """
        otaTestCases = []
        try:
            for test in _otaConfig['supported_tests']:
                otaTestCases.extend(OtaTestCaseFactory.createTestCases(
                    test,
                    self._boardConfig,
                    self._otaProject,
                    self._otaAwsAgent,
                    self._flashComm
                ))
        except Exception as e:
            print(e)
            self.__cleanup()
            raise

        if not otaTestCases:
            raise Exception("Test Case Not Found")

        return otaTestCases

    def __runTest(self, otaTestCase):
        """
        - Build the code with the version desired.
        - Flash the code built.
        """
        testResult = OtaTestResult(testName=otaTestCase.getName(), result=OtaTestResult.ERROR, summary='Can not collect test result. Please check logs.')
        testResult = otaTestCase.runTest()

        testResult.board = self._boardConfig['name']
        return testResult

    def runTests(self):
        """Run all tests this Test Runner object holds.
        Returns the results of the tests.
        """
        try:
            testResults = []
            for otaTestCase in self._otaTestCases:
                testResults.append(self.__runTest(otaTestCase))
        finally:
            # All tests are over so clean up.
            print("cleanning up aws resources")
            self.__cleanup()

        return testResults

    def __cleanup(self):
        """Free resources used by the OTA agent and the flash serial object.
        """
        # We will want to close the serial flash thread first so that there are no outstanding threads
        # in case cleaning AWS Resources causes exceptions.
        try:
            self._flashComm.cleanup()
        except:
            # We still want to clean up AWS resources if the serial thread fails to cleanup. So just
            # print the exception here.
            print(e)

        try:
            self._otaAwsAgent.cleanup()
        except Exception as e:
            print(e)
            raise
