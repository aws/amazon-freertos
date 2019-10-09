#
# Amazon FreeRTOS BLE HAL V2.0.0
# Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

    # default DUT_name: nimble(without set_property)
    # TODO: check DUT with MAC address instead of name.

    scan_filter.update({ "UUIDs": [runTest.DUT_UUID_128]})

    # default DUT_name: nimble(without set_property)
    # TODO: check DUT with MAC address instead of name.
    DUT_ORIGINAL_NAME = runTest.DUT_NAME
    runTest.DUT_NAME = "nimb"
    bleAdapter.setDiscoveryFilter(scan_filter)

    # Advertisement interval consistent after reset test
    # The first time uses different callback to get/check test device information. Use the second time and third time KPI to compare.
    # First time connection
    isTestSuccessFull = True
    bleAdapter.startDiscovery(runTest.discoveryEventCb)
    # firstStartScan = time.time()
    runTest.mainloop.run()
    # firstKPI = time.time() - firstStartScan
    runTest.submitTestResult(isTestSuccessFull, runTest.advertisement)
    bleAdapter.stopDiscovery()

    testDevice = runTest.getTestDevice()
    isTestSuccessFull = bleAdapter.connect(testDevice)
    runTest.submitTestResult(isTestSuccessFull, runTest.simpleConnection)
    time.sleep(2) #wait for connection parameters update

    isTestSuccessFull &= bleAdapter.disconnect()

    # Second time connection
    bleAdapter.startDiscovery(runTest.discoveryStartedCb)#wait for DUT to start advertising
    secondStartScan = time.time()
    runTest.mainloop.run()
    secondKPI = time.time() - secondStartScan
    bleAdapter.stopDiscovery()
    isConnectSuccessFull = bleAdapter.connect(testDevice)
    isTestSuccessFull &= isConnectSuccessFull
    runTest.submitTestResult(isTestSuccessFull, runTest.reConnection)
    time.sleep(2) #wait for connection parameters update
    # Second time disconnect
    isTestSuccessFull &= bleAdapter.disconnect()

    #Third time connection
    bleAdapter.startDiscovery(runTest.discoveryStartedCb)#wait for DUT to start advertising
    thirdStartScan = time.time()
    runTest.mainloop.run()
    thirdKPI = time.time() - thirdStartScan
    bleAdapter.stopDiscovery()
    isConnectSuccessFull = bleAdapter.connect(testDevice)
    isTestSuccessFull &= isConnectSuccessFull
    runTest.submitTestResult(isTestSuccessFull, runTest.reConnection)

    if thirdKPI > secondKPI * 10:
        isTestSuccessFull &= false
    # write result back to server
    isTestSuccessFull = runTest.discoverPrimaryServices()
    bleAdapter.gatt.updateLocalAttributeTable()

    isTestSuccessFull &= runTest.writeResultWithoutResponse(chr(isTestSuccessFull + 48))
    runTest.submitTestResult(isTestSuccessFull, runTest.writeWithoutResponse)

    # Data size > MTU - 3 send notification test
    bleAdapter.setNotificationCallBack(runTest.notificationMTUCb)
    bleAdapter.subscribeForNotification(runTest.DUT_NOTIFY_CHAR_UUID) #subscribe for next test
    isTestSuccessFull = True
    runTest.mainloop.run()
    isTestSuccessFull = runTest.isNotificationDeclinedSuccessFull
    runTest.submitTestResult(isTestSuccessFull, runTest.notification)

    isTestSuccessFull = bleAdapter.subscribeForNotification(runTest.DUT_NOTIFY_CHAR_UUID, subscribe = False) #unsubscribe
    runTest.submitTestResult(isTestSuccessFull, runTest.removeNotification)

    isTestSuccessFull &= bleAdapter.disconnect()
    testutils.removeBondedDevices()

    runTest.DUT_NAME = DUT_ORIGINAL_NAME

    isTestSuccessFull = runTest.Advertise_With_16bit_ServiceUUID(scan_filter=scan_filter, bleAdapter=bleAdapter)
    runTest.submitTestResult(isTestSuccessFull, runTest.Advertise_With_16bit_ServiceUUID)

    isTestSuccessFull = runTest.Advertise_With_Manufacture_Data(scan_filter=scan_filter, bleAdapter=bleAdapter)
    runTest.submitTestResult(isTestSuccessFull, runTest.Advertise_With_Manufacture_Data)

    scan_filter.update({ "UUIDs": [runTest.DUT_UUID_128]})
    # default DUT_name: nimble(without set_property)
    bleAdapter.setDiscoveryFilter(scan_filter)

    # First time connection
    isTestSuccessFull = True
    bleAdapter.startDiscovery(runTest.discoveryEventCb)
    runTest.mainloop.run()
    runTest.submitTestResult(isTestSuccessFull, runTest.advertisement)
    bleAdapter.stopDiscovery()

    testDevice = runTest.getTestDevice()
    isTestSuccessFull = bleAdapter.connect(testDevice)
    runTest.submitTestResult(isTestSuccessFull, runTest.simpleConnection)

    isTestSuccessFull = runTest.discoverPrimaryServices()
    runTest.submitTestResult(isTestSuccessFull, runTest.discoverPrimaryServices)

    bleAdapter.gatt.updateLocalAttributeTable()

    # Check device not present. After discovery of services, advertisement should have stopped.
    runTest.stopAdvertisement(scan_filter)

    # Check write and read
    bleAdapter.writeCharacteristic(runTest.DUT_OPEN_CHAR_UUID, runTest.DUT_OPEN_DESCR_UUID)
    bleAdapter.readCharacteristic(runTest.DUT_OPEN_CHAR_UUID)

    # Enable and receive notification and indication then disable.
    bleAdapter.subscribeForNotification(runTest.DUT_NOTIFY_CHAR_UUID)
    bleAdapter.subscribeForNotification(runTest.DUT_INDICATE_CHAR_UUID) #subscribe for next test

    time.sleep(2) #wait for connection parameters update

    # Check Notification and Indication
    bleAdapter.setNotificationCallBack(runTest.notificationCb)
    isTestSuccessFull = True
    runTest.mainloop.run()
    runTest.submitTestResult(isTestSuccessFull, runTest.notification)

    bleAdapter.setNotificationCallBack(runTest.indicationCb)
    isTestSuccessFull = True
    runTest.mainloop.run()
    runTest.submitTestResult(isTestSuccessFull, runTest.indication)

    isTestSuccessFull &= bleAdapter.disconnect()

    # Second time connection
    bleAdapter.startDiscovery(runTest.discoveryStartedCb)#wait for DUT to start advertising
    runTest.mainloop.run()
    bleAdapter.stopDiscovery()
    isConnectSuccessFull = bleAdapter.connect(testDevice)
    isTestSuccessFull &= isConnectSuccessFull
    runTest.submitTestResult(isTestSuccessFull, runTest.reConnection)

    bleAdapter.subscribeForNotification(runTest.DUT_NOTIFY_CHAR_UUID)
    bleAdapter.subscribeForNotification(runTest.DUT_INDICATE_CHAR_UUID) #subscribe for next test

    # Check write and read after reconnection
    bleAdapter.writeCharacteristic(runTest.DUT_OPEN_CHAR_UUID, runTest.DUT_OPEN_DESCR_UUID)
    bleAdapter.readCharacteristic(runTest.DUT_OPEN_CHAR_UUID)

    # Check Notification and Indication after reconnection
    bleAdapter.setNotificationCallBack(runTest.notificationCb)
    isTestSuccessFull = True
    runTest.mainloop.run()
    runTest.submitTestResult(isTestSuccessFull, runTest.notification)

    bleAdapter.setNotificationCallBack(runTest.indicationCb)
    isTestSuccessFull = True
    runTest.mainloop.run()
    runTest.submitTestResult(isTestSuccessFull, runTest.indication)

    isTestSuccessFull = bleAdapter.subscribeForNotification(runTest.DUT_NOTIFY_CHAR_UUID, subscribe = False) #unsubscribe
    isTestSuccessFull = True
    runTest.submitTestResult(isTestSuccessFull, runTest.removeNotification)

    isTestSuccessFull = bleAdapter.subscribeForNotification(runTest.DUT_INDICATE_CHAR_UUID, subscribe = False) #unsubscribe
    isTestSuccessFull = True
    runTest.submitTestResult(isTestSuccessFull, runTest.removeIndication)

    isTestSuccessFull &= bleAdapter.disconnect()
    testutils.removeBondedDevices()

    time.sleep(2) #wait for connection parameters update
    runTest.submitTestResult(isTestSuccessFull, runTest.disconnect)
    runTest.printTestsSummary()
