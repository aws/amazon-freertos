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
class OtaTestResult:
    """Object representing a OTA test result.
    Attributes
        result(str): The result (PASS | FAIL | ERROR) of this testcase.
        board(str): The board name of this testcase.
        testName(str): The name of this testcase.
        jobStatus(str): The job status from AWS IoT, containing both status and status reason provided by devices.
        summary(str): The summary for the test result.
    Methods:
        testResultFromJobStatus(passOrFail, jobStatus, isPositive)
    Example:
        passingResult = OtaTestResult(OtaTestResult.PASS, 'TI', 'OtaTestGreaterVersion', 'SUCCEEDED (accepted v0.9.1)', 'accepted v0.9.1')
        failingResult = OtaTestResult(OtaTestResult.FAIL, 'TI', 'OtaTestGreaterVersion')
        errorResult = OtaTestResult(OtaTestResult.ERROR, 'TI')
        errorResult = OtaTestResult(OtaTestResult.ERROR, 'TI', 'OtaTestGreaterVersion')
    """
    PASS    = 'PASS'   # The test executed and passed.
    FAIL    = 'FAIL'   # The test executed but failed.
    ERROR   = 'ERROR'  # The test may or may not execute due to test script error.

    __HEADER    = '\033[95m'
    __OKBLUE    = '\033[94m'
    __OKGREEN   = '\033[92m'
    __WARNING   = '\033[93m'
    __FAIL      = '\033[91m'
    __ENDC      = '\033[0m'
    __BOLD      = '\033[1m'
    __UNDERLINE = '\033[4m'

    __RESULT_COLOR = {
        PASS:   __OKGREEN,
        FAIL:   __FAIL,
        ERROR:  __WARNING,
    }

    def __init__(self, *, result, board='', testName='', jobStatus=None, summary=''):
        self.result = result
        self.board = board
        self.testName = testName
        self.jobStatus = jobStatus
        self.summary = summary

    def print(self, elapsed):
        print(self.__RESULT_COLOR[self.result] + 'OTA E2E TEST RESULT: ' + self.result)
        print(self.__OKBLUE + 'IOT JOB STATUS: ' + (self.jobStatus if self.jobStatus else 'No IoT Job Status'))
        print(self.__OKBLUE + 'OTA E2E TEST RESULT SUMMARY: ' + (self.summary if self.summary else 'No Test Summary') + self.__ENDC)
        print(self.__BOLD + 'Time Elapsed: ' + str(int(elapsed / 60)) + " Minutes and " + str(int(elapsed % 60)) + " Seconds"  + self.__ENDC)

    def testResultFromJobStatus(testName, jobStatus, isPositive, summary):
        """Quickly turn the Test result from OtaAwsAgent into a OtaTest Result.
        Args:
            testName(str): The name of the test case.
            jobStatus(nametuple(status reason)): This is typically from OtaAwsAgent.pollOtaUpdateCompletion() or OtaAwsAgent.__getJobStatus().
            isPositive(bool): The flag tells the test case is happy case or not.
        Returns an OtaTestResult.
        """
        if isPositive:
            passOrFail = OtaTestResult.PASS if jobStatus.status == 'SUCCEEDED' else OtaTestResult.FAIL
        else:
            passOrFail = OtaTestResult.FAIL if jobStatus.status == 'SUCCEEDED' else OtaTestResult.PASS

        return OtaTestResult(result=passOrFail, testName=testName, jobStatus=jobStatus.status + ' (' + jobStatus.reason + ')' if jobStatus else "", summary= summary)
    staticmethod = testResultFromJobStatus
