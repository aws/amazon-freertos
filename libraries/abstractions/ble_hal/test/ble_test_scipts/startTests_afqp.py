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
import threading
import securityAgent
import bleAdapter
import time
from testClass import runTest
from bleAdapter import bleAdapter

ENABLE_TC_AFQP_READ_WRITE_LONG = 1

def main():
    scan_filter = dict()

    bleAdapter.init()
    agent = securityAgent.createSecurityAgent()

    scan_filter.update({"UUIDs": [runTest.DUT_UUID_128]})
    bleAdapter.setDiscoveryFilter(scan_filter)
    # Discovery test
    bleAdapter.startDiscovery(runTest.discoveryEventCb)
    runTest.mainloop.run()
    isTestSuccessFull = True
    runTest.submitTestResult(isTestSuccessFull, runTest.advertisement)
    bleAdapter.stopDiscovery()

    # Simple Connection test
    testDevice = runTest.getTestDevice()
    isTestSuccessFull = bleAdapter.connect(testDevice)
    runTest.submitTestResult(isTestSuccessFull, runTest.simpleConnection)
    time.sleep(2)  # wait for connection parameters update

    # Discover all primary services
    isTestSuccessFull = runTest.discoverPrimaryServices()
    runTest.submitTestResult(
        isTestSuccessFull,
        runTest.discoverPrimaryServices)

    bleAdapter.gatt.updateLocalAttributeTable( True )

    # Check device not present. After discovery of services, advertisement
    # should have stopped.
    runTest.stopAdvertisement(scan_filter)

    # Change MTU size
    # bleAdapter.readLocalMTU()

    # Check attribute table UUIDs
    bleAdapter.gatt.updateLocalAttributeTable( True )
    isTestSuccessFull = runTest.checkUUIDs(bleAdapter.gatt, True)
    runTest.submitTestResult(isTestSuccessFull, runTest.checkUUIDs)

    # Check attribute table properties
    isTestSuccessFull = runTest.checkProperties(bleAdapter.gatt)
    runTest.submitTestResult(isTestSuccessFull, runTest.checkProperties)

    # CHeck long write
    if ENABLE_TC_AFQP_READ_WRITE_LONG == 1:
        isTestSuccessFull = runTest.writereadLongCharacteristic()
        runTest.submitTestResult(
            isTestSuccessFull,
            runTest.writereadLongCharacteristic)

    # Check read/write, simple connection
    isTestSuccessFull = runTest.readWriteSimpleConnection()
    runTest.submitTestResult(
        isTestSuccessFull,
        runTest.readWriteSimpleConnection)

    # check write without response
    isTestSuccessFull = runTest.writeWithoutResponse()
    runTest.submitTestResult(isTestSuccessFull, runTest.writeWithoutResponse)

    # Enable and receive notification and indication then disable.
    bleAdapter.setNotificationCallBack(runTest.notificationCb)
    bleAdapter.subscribeForNotification(
        runTest.DUT_NOTIFY_CHAR_UUID)  # subscribe for next test
    bleAdapter.subscribeForNotification(
        runTest.DUT_INDICATE_CHAR_UUID)  # subscribe for next test
    isTestSuccessFull = True
    runTest.mainloop.run()
    runTest.submitTestResult(isTestSuccessFull, runTest.notification)

    bleAdapter.setNotificationCallBack(runTest.indicationCb)
    isTestSuccessFull = True
    runTest.mainloop.run()
    runTest.submitTestResult(isTestSuccessFull, runTest.indication)

    isTestSuccessFull = bleAdapter.subscribeForNotification(
        runTest.DUT_NOTIFY_CHAR_UUID, subscribe=False)  # unsubscribe
    isTestSuccessFull = True
    runTest.submitTestResult(isTestSuccessFull, runTest.removeNotification)

    isTestSuccessFull = bleAdapter.subscribeForNotification(
        runTest.DUT_INDICATE_CHAR_UUID, subscribe=False)  # unsubscribe
    isTestSuccessFull = True
    runTest.submitTestResult(isTestSuccessFull, runTest.removeIndication)

    # Check writing to protected characteristic triggers pairing
    isTestSuccessFull = runTest.pairing()
    runTest.submitTestResult(isTestSuccessFull, runTest.pairing)
    bleAdapter.bondToRemoteDevice()

    # Check writing to protected characteristic after successfull pairing
    # succeed
    time.sleep(2)  # wait before starting next test
    isTestSuccessFull = runTest.readWriteProtectedAttributesWhilePaired()
    runTest.submitTestResult(isTestSuccessFull,
                             runTest.readWriteProtectedAttributesWhilePaired)

    # disconnect, Note it is not a test happening on bluez, the DUT is waiting
    # for a disconnect Cb
    runTest.disconnect()

    # reconnect! Since devices bonded, it should not ask for pairing again.
    # Security agent can be destroyed
    # remove security agent so as not to trigger auto pairing.
    securityAgent.removeSecurityAgent()
    bleAdapter.setDiscoveryFilter(scan_filter)
    # wait for DUT to start advertising
    bleAdapter.startDiscovery(runTest.discoveryStartedCb)
    runTest.mainloop.run()
    bleAdapter.stopDiscovery()
    runTest.reconnectWhileBonded()

    # Test to wait for a disconnect from DUT.
    runTest.waitForDisconnect()

    # reconnect while not bonded. Pairing should fail since Just works is not
    # accepted
    bleAdapter.removeBondedDevices()
    time.sleep(2)  # wait for bonded devices to be deleted
    bleAdapter.setDiscoveryFilter(scan_filter)
    bleAdapter.startDiscovery(runTest.discoveryEventCb)
    runTest.mainloop.run()
    bleAdapter.stopDiscovery()

    agent = securityAgent.createSecurityAgent("NoInputNoOutput", agent)
    runTest.reconnectWhileNotBonded()

    time.sleep(2)
    runTest.printTestsSummary()
