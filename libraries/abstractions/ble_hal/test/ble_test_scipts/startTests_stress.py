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
import time
from testClass import runTest
from bleAdapter import bleAdapter
import testutils


def main():
    agent = None
    STRESS_TIER = 1

    if (STRESS_TIER == 0):
        STRESS_TEST_INIT_REPETITION = 1
        STRESS_TEST_ENABLE_REPETITION = 1
        STRESS_TEST_MODULE_REPETITION = 1
    elif (STRESS_TIER == 1):
        STRESS_TEST_INIT_REPETITION = 1
        STRESS_TEST_ENABLE_REPETITION = 1
        STRESS_TEST_MODULE_REPETITION = 10
    elif (STRESS_TIER == 2):
        STRESS_TEST_INIT_REPETITION = 2
        STRESS_TEST_ENABLE_REPETITION = 10
        STRESS_TEST_MODULE_REPETITION = 100
    elif (STRESS_TIER == 3):
        STRESS_TEST_INIT_REPETITION = 3
        STRESS_TEST_ENABLE_REPETITION = 100
        STRESS_TEST_MODULE_REPETITION = 1000

    isTestSuccessFull = True

    scan_filter = dict()

    bleAdapter.init()
    agent = securityAgent.createSecurityAgent(agent=agent)

    scan_filter.update({"UUIDs": [runTest.DUT_UUID_128]})

    for i in range(
            STRESS_TEST_INIT_REPETITION *
            STRESS_TEST_ENABLE_REPETITION):
        bleAdapter.setDiscoveryFilter(scan_filter)
        for j in range(STRESS_TEST_MODULE_REPETITION):
            if i == 0 and j == 0:
                bleAdapter.startDiscovery(runTest.discoveryEventCb)
            else:
                bleAdapter.startDiscovery(runTest.discoveryStartedCb)
            runTest.mainloop.run()
            bleAdapter.stopDiscovery()

            testDevice = runTest.getTestDevice()
            isConnectSuccessFull = bleAdapter.connect(testDevice)

            isTestSuccessFull &= isConnectSuccessFull
            runTest.stopAdvertisement(scan_filter)
            isTestSuccessFull &= bleAdapter.disconnect()

        bleAdapter.startDiscovery(runTest.discoveryStartedCb)
        runTest.mainloop.run()
        bleAdapter.stopDiscovery()

        runTest._simple_connect()
        runTest.stopAdvertisement(scan_filter)
        # Discover all primary services
        isTestSuccessFull = runTest.discoverPrimaryServices()
        runTest.submitTestResult(
            isTestSuccessFull,
            runTest.discoverPrimaryServices)
        bleAdapter.gatt.updateLocalAttributeTable()
        isTestSuccessFull &= runTest.checkUUIDs(bleAdapter.gatt)
        # Check attribute table properties
        isTestSuccessFull &= runTest.checkProperties(bleAdapter.gatt)

        # Check read/write, simple connection
        for i in range(STRESS_TEST_MODULE_REPETITION):
            isTestSuccessFull &= runTest.readWriteSimpleConnection()
            runTest.submitTestResult(
                isTestSuccessFull,
                runTest.readWriteSimpleConnection)
            time.sleep(2)

        bleAdapter.subscribeForNotification(
            runTest.DUT_NOTIFY_CHAR_UUID)  # subscribe for next test
        bleAdapter.subscribeForNotification(
            runTest.DUT_INDICATE_CHAR_UUID)  # subscribe for next test

        # Check read/write, simple connection
        for i in range(STRESS_TEST_MODULE_REPETITION):
            # Enable and receive notification and indication then disable.
            bleAdapter.setNotificationCallBack(runTest.notificationCb)
            isTestSuccessFull = True
            runTest.mainloop.run()
            runTest.submitTestResult(isTestSuccessFull, runTest.notification)

            bleAdapter.setNotificationCallBack(runTest.indicationCb)
            isTestSuccessFull = True
            runTest.mainloop.run()
            runTest.submitTestResult(isTestSuccessFull, runTest.indication)
        time.sleep(5)

        isTestSuccessFull = bleAdapter.subscribeForNotification(
            runTest.DUT_NOTIFY_CHAR_UUID, subscribe=False)  # unsubscribe
        isTestSuccessFull = True
        runTest.submitTestResult(isTestSuccessFull, runTest.removeNotification)

        isTestSuccessFull = bleAdapter.subscribeForNotification(
            runTest.DUT_INDICATE_CHAR_UUID, subscribe=False)  # unsubscribe
        isTestSuccessFull = True
        runTest.submitTestResult(isTestSuccessFull, runTest.removeIndication)

        if STRESS_TEST_MODULE_REPETITION:
            runTest.submitTestResult(isTestSuccessFull, runTest.reConnection)

        time.sleep(2)

    runTest.printTestsSummary()
