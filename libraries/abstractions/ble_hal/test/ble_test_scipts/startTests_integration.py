#
# FreeRTOS BLE HAL V2.0.0
# Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# http://aws.amazon.com/freertos
# http://www.FreeRTOS.org
#

import Queue
import sys
import os
import threading
import securityAgent
import testutils
import time
from testClass import runTest
from bleAdapter import bleAdapter


def main():
    scan_filter = dict()

    bleAdapter.init()
    agent = securityAgent.createSecurityAgent()

    isTestSuccessFull = runTest.Advertise_Without_Properties(
        scan_filter=scan_filter, bleAdapter=bleAdapter)
    runTest.submitTestResult(isTestSuccessFull,
                             runTest.Advertise_Without_Properties)

    isTestSuccessFull = runTest.Advertise_With_16bit_ServiceUUID(
        scan_filter=scan_filter, bleAdapter=bleAdapter)
    runTest.submitTestResult(isTestSuccessFull,
                             runTest.Advertise_With_16bit_ServiceUUID)

    isTestSuccessFull = runTest.Advertise_With_Manufacture_Data(
        scan_filter=scan_filter, bleAdapter=bleAdapter)
    runTest.submitTestResult(
        isTestSuccessFull,
        runTest.Advertise_With_Manufacture_Data)

    isTestSuccessFull = runTest.Advertise_Interval_Consistent_After_BT_Reset(
        scan_filter=scan_filter, bleAdapter=bleAdapter)
    runTest.submitTestResult(
        isTestSuccessFull,
        runTest.Advertise_Interval_Consistent_After_BT_Reset)

    isTestSuccessFull = runTest.Write_Notification_Size_Greater_Than_MTU_3(
        scan_filter=scan_filter, bleAdapter=bleAdapter)
    runTest.submitTestResult(
        isTestSuccessFull,
        runTest.Write_Notification_Size_Greater_Than_MTU_3)

    isTestSuccessFull = runTest.Send_Data_After_Disconnected(
        scan_filter=scan_filter, bleAdapter=bleAdapter)
    runTest.submitTestResult(
        isTestSuccessFull,
        runTest.Send_Data_After_Disconnected)

    isTestSuccessFull = runTest.Check_Bond_State(
        scan_filter=scan_filter, bleAdapter=bleAdapter)
    runTest.submitTestResult(
        isTestSuccessFull,
        runTest.Check_Bond_State)

    time.sleep(2)  # wait for connection parameters update
    runTest.submitTestResult(isTestSuccessFull, runTest.disconnect)
    runTest.printTestsSummary()
