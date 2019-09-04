import Queue
import sys
import os
import threading
import securityAgent
import testutils
import time
from testClass import runTest
from bleAdapter import bleAdapter
import dbus.mainloop.glib
try:
  from gi.repository import GObject
except ImportError:
  import gobject as GObject

mainloop = GObject.MainLoop()
notificationEvent = threading.Event()
indicationEvent = threading.Event()
discoveryEvent = threading.Event()
servicesResolvedEvent = threading.Event()
pairingEvent = threading.Event()

def discoveryStartedCb(testDevice):
    mainloop.quit()


def discoveryEventCb(testDevice):
    isTestSuccessFull = runTest.advertisement(testDevice)

    if isTestSuccessFull == True:
        runTest.setTestDevice(testDevice)
        #discoveryEvent.set()
        mainloop.quit()

def notificationMTUCb(uuid, value, flag):
    notification = runTest.notificationMTU2(uuid, value, flag)
    if notification == runTest.DUT_FAIL_STRING:
        mainloop.quit()
        runTest.isNotificationDeclinedSuccessFull = True
    if notification == runTest.DUT_MTU_2_STRING:
        mainloop.quit()
        runTest.isNotificationDeclinedSuccessFull = False

def teardown_test(agent):
    securityAgent.removeSecurityAgent()

    os.system("sudo rm -rf \"/var/lib/bluetooth/*\"")
    os.system("sudo hciconfig hci0 reset")

    testutils.removeBondedDevices()
    return agent
    
def main():
    scan_filter = dict()

    bleAdapter.init()
    agent = securityAgent.createSecurityAgent()

    scan_filter.update({ "UUIDs": [runTest.DUT_UUID]})

    # default DUT_name: nimble(without set_property)
    # TODO: check DUT with MAC address instead of name.
    runTest.DUT_NAME = "nimb"
    bleAdapter.setDiscoveryFilter(scan_filter)


    # Advertisement interval consistent after reset test
    # TODO: the first time uses different callback to get/check test device information. we can choose to use the second time and third time KPI to compare.
    # First time connection
    isTestSuccessFull = True
    bleAdapter.startDiscovery(discoveryEventCb)
    # firstStartScan = time.time()
    mainloop.run()
    # firstKPI = time.time() - firstStartScan
    runTest.submitTestResult(isTestSuccessFull, runTest.advertisement)
    bleAdapter.stopDiscovery()

    testDevice = runTest.getTestDevice()
    isTestSuccessFull = bleAdapter.connect(testDevice)
    runTest.submitTestResult(isTestSuccessFull, runTest.simpleConnection)
    time.sleep(2) #wait for connection parameters update

    isTestSuccessFull &= bleAdapter.disconnect()

    # Second time connection
    bleAdapter.startDiscovery(discoveryStartedCb)#wait for DUT to start advertising
    secondStartScan = time.time()
    mainloop.run()
    secondKPI = time.time() - secondStartScan
    bleAdapter.stopDiscovery()
    isConnectSuccessFull = bleAdapter.connect(testDevice)
    isTestSuccessFull &= isConnectSuccessFull
    runTest.submitTestResult(isTestSuccessFull, runTest.reConnection)
    time.sleep(2) #wait for connection parameters update
    # Second time disconnect
    isTestSuccessFull &= bleAdapter.disconnect()

    #Third time connection
    bleAdapter.startDiscovery(discoveryStartedCb)#wait for DUT to start advertising
    thirdStartScan = time.time()
    mainloop.run()
    thirdKPI = time.time() - thirdStartScan
    bleAdapter.stopDiscovery()
    isConnectSuccessFull = bleAdapter.connect(testDevice)
    isTestSuccessFull &= isConnectSuccessFull
    runTest.submitTestResult(isTestSuccessFull, runTest.reConnection)

    if thirdKPI > secondKPI * 10:
        isTestSuccessFull &= false
    # write result back to peripheral
    bleAdapter.gatt.updateLocalAttributeTable()
    isTestSuccessFull &= runTest.writeResultWithoutResponse(chr(isTestSuccessFull + 48))
    runTest.submitTestResult(isTestSuccessFull, runTest.writeWithoutResponse)


    # Data size > MTU - 3 send notification test
    # Subscribe notification on Char E by CCCD with the same UUID.
    bleAdapter.setNotificationCallBack(notificationMTUCb)
    bleAdapter.subscribeForNotification(runTest.DUT_NOTIFY_CHAR_UUID) #subscribe for next test
    isTestSuccessFull = True
    mainloop.run()
    isTestSuccessFull = runTest.isNotificationDeclinedSuccessFull
    runTest.submitTestResult(isTestSuccessFull, runTest.notification)

    isTestSuccessFull = bleAdapter.subscribeForNotification(runTest.DUT_NOTIFY_CHAR_UUID, subscribe = False) #unsubscribe
    runTest.submitTestResult(isTestSuccessFull, runTest.removeNotification)

    isTestSuccessFull &= bleAdapter.disconnect()
    time.sleep(6) #wait for connection parameters update
    runTest.submitTestResult(isTestSuccessFull, runTest.disconnect)
    runTest.printTestsSummary()
    agent = teardown_test(agent)

def errorConnectCb():
    print("Connection error")
    sys.stdout.flush()
    connectEvent.put(0)
