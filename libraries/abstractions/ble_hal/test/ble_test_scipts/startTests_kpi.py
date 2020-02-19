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
    agent = None
    scan_filter = dict()

    bleAdapter.init()
    agent = securityAgent.createSecurityAgent(agent=agent)

    scan_filter.update({"UUIDs": [runTest.DUT_UUID_128]})
    bleAdapter.setDiscoveryFilter(scan_filter)

    # KPI test
    # Evaluate KPI from scanning start to advertisement received.
    # Evaluate KPI from scanning start to connection created.
    isTestSuccessFull = True
    startToReceivedSum = 0
    startToConnectedSum = 0
    numberOfReconnect = 3
    numberOfSuccess = 0

    for i in range(numberOfReconnect):
        if i == 0:
            bleAdapter.startDiscovery(runTest.discoveryEventCb)
        else:
            # wait for DUT to start advertising
            bleAdapter.startDiscovery(runTest.discoveryStartedCb)
        tStartScan = time.time()
        runTest.mainloop.run()
        startToReceived = time.time() - tStartScan
        bleAdapter.stopDiscovery()

        testDevice = runTest.getTestDevice()
        isConnectSuccessFull = bleAdapter.connect(testDevice)
        startToConnected = time.time() - tStartScan

        isTestSuccessFull &= isConnectSuccessFull
        startToReceivedSum += startToReceived
        if isConnectSuccessFull:
            startToConnectedSum += startToConnected
            numberOfSuccess += 1
        isTestSuccessFull &= bleAdapter.disconnect()

    if numberOfReconnect:
        startToReceivedAvg = startToReceivedSum / numberOfReconnect
        print "start scanning to received:" + str(startToReceivedAvg)
    if numberOfSuccess:
        startToConnectedAvg = startToConnectedSum / numberOfSuccess
        print "start scanning to connected:" + str(startToConnectedAvg)
    runTest.submitTestResult(isTestSuccessFull, runTest.reConnection)

    time.sleep(2)
    runTest.printTestsSummary()
