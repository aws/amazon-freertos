"""
Amazon FreeRTOS
Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

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
from abc import abstractmethod
from collections import namedtuple

class OtaTestCase( object ):
    """OTA Test Case abstract class.
    Attributes:
        _name(str): Name of the testcase.
        _otaConfig(dict): 'ota_config' in board.json.
        _otaProject(obj:OtaAfrProject): Amazon FreeRTOS source code resource.
        _otaAwsAgent(obj:OtaAwsAgent): AWS CLI for OTA resource.
        _flashComm(obj:FlashSerialComm): MCU Flash and Serial read resource.
    Methods:
        getTestResult() : abstract method
        run() : abstract method
        getName()
        setup() : Sets up _otaProject as 0.9.0 should be overwritten if that is not desired.
        teardown()
        runTest()
        getTestResultAfterJobCompletion()
    """
    TestCaseResult = namedtuple('TestCaseResult', 'result reason')
    def __init__(self, name, boardConfig, otaProject, otaAwsAgent, flashComm):
        self._name = name
        self._boardConfig = boardConfig
        self._otaConfig = boardConfig['ota_config']
        self._otaProject = otaProject
        self._otaAwsAgent = otaAwsAgent
        self._flashComm = flashComm

    def getName(self):
        """Return the name of this test case.
        """
        return self._name
        
    def setup(self):
        """Setup the OTA test.
        This method should be overwritten if flashing version 0.9.0 is not desired.
        """
        self._otaProject.setApplicationVersion(0, 9, 0)
        self._otaProject.buildProject()
        self._otaProject.generateFactoryImage()
        self._flashComm.flashAndRead()

    def teardown(self):
        """ Tear down the OTA test.
        All the necessary cleanup. Optional method.
        """
        None
        
    @abstractmethod
    def getTestResult(self, otaStatus, log):
        """Get the results of the test given the otaStatus or the output in the log.
        Returns OtaTestCase.PASS or OtaTestCase.FAIL.
        Args:
            otaStatus(namedtuple(status, reason)): The status from AWS IoT OTA Service.
            log(str): The output log from the MCU device.
        """
        print("OtaTestCase::getTestResult is not implemented.")
        return OtaTestResult(OtaTestCase.FAIL, 'OtaTestCase::getTestResult is not implemented')

    def runTest(self):
        """Run this OTA test case.
        """
        print('---------- Running '+ self._name + ' ----------')
        # Run the implemented setup.
        jobId =self.setup()
        # Run the implemented runTest function
        testResult = self.run()
        print('OTA E2E TEST RESULT: ' + testResult.result + '; because ' + testResult.reason)
        # Clean up
        self.teardown()
        print('---------- Finished '+ self._name + ' ----------')
        return testResult


    @abstractmethod
    def run(self):
        """ Run the OTA test case setup by the jobID. 
        Should create the desired OTA job here and wait on completion.
        """
        print('OtaTestCase::run is not implemented.')

    def getTestResultAfterJobCompletion(self, jobId):
        """Utility helper to poll for completion of the input job then stop reading
        from the serial port.
        Args:
            jobId(str): AWS IoT Job ID to poll for completion status.
        """
        jobStatus = self._otaAwsAgent.pollJobCompletion(jobId, self._otaConfig['ota_timeout_sec'])
        self._flashComm.stopSerialRead()
        return self.getTestResult(jobStatus, self._flashComm.getSerialLog())