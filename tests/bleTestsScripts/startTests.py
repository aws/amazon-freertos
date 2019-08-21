import Queue
import sys
import threading
import securityAgent
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

def notificationCb(uuid, value):
    isNotificationTestSuccessFull = runTest.notification(uuid, value)
    if isNotificationTestSuccessFull == True:
        #notificationEvent.set()
        mainloop.quit()

    isIndicationTestSuccessFull = runTest.indication(uuid, value)
    if isIndicationTestSuccessFull == True:
        #indicationEvent.set()
        mainloop.quit()

def main():
    scan_filter = dict()

    bleAdapter.init()
    agent = securityAgent.createSecurityAgent()

    scan_filter.update({ "UUIDs": [runTest.DUT_UUID]})
    bleAdapter.setDiscoveryFilter(scan_filter)

    #Discovery test
    bleAdapter.startDiscovery(discoveryEventCb)
    mainloop.run()
    isTestSuccessFull = True
    runTest.submitTestResult(isTestSuccessFull, runTest.advertisement)
    bleAdapter.stopDiscovery()

    #Simple Connection test
    testDevice = runTest.getTestDevice()
    isTestSuccessFull = bleAdapter.connect(testDevice)
    runTest.submitTestResult(isTestSuccessFull, runTest.simpleConnection)
    time.sleep(2) #wait for connection parameters update

    #Discover all primary services
    isTestSuccessFull = runTest.discoverPrimaryServices()
    runTest.submitTestResult(isTestSuccessFull, runTest.discoverPrimaryServices)

    bleAdapter.gatt.updateLocalAttributeTable()

    #Check device not present. After discovery of services, advertisement should have stopped.
    runTest.stopAdvertisement(scan_filter)


    #Change MTU size
    #bleAdapter.readLocalMTU()

    #Check attribute table UUIDs
    bleAdapter.gatt.updateLocalAttributeTable()
    isTestSuccessFull = runTest.checkUUIDs(bleAdapter.gatt)
    runTest.submitTestResult(isTestSuccessFull, runTest.checkUUIDs)

    #Check attribute table properties
    isTestSuccessFull = runTest.checkProperties(bleAdapter.gatt)
    runTest.submitTestResult(isTestSuccessFull, runTest.checkProperties)

    #Check read/write, simple connection
    isTestSuccessFull = runTest.readWriteSimpleConnection()
    runTest.submitTestResult(isTestSuccessFull, runTest.readWriteSimpleConnection)

    #check write without response
    isTestSuccessFull = runTest.writeWithoutResponse()
    runTest.submitTestResult(isTestSuccessFull, runTest.writeWithoutResponse)

    #Enable and receive notification and indication then disable.
    bleAdapter.setNotificationCallBack(notificationCb)
    bleAdapter.subscribeForNotification(runTest.DUT_NOTIFY_CHAR_UUID) #subscribe for next test
    bleAdapter.subscribeForNotification(runTest.DUT_INDICATE_CHAR_UUID) #subscribe for next test

    isTestSuccessFull = True
    mainloop.run()
    runTest.submitTestResult(isTestSuccessFull, runTest.notification)

    isTestSuccessFull = True
    mainloop.run()
    runTest.submitTestResult(isTestSuccessFull, runTest.indication)

    isTestSuccessFull = bleAdapter.subscribeForNotification(runTest.DUT_NOTIFY_CHAR_UUID, subscribe = False) #unsubscribe
    isTestSuccessFull = True
    runTest.submitTestResult(isTestSuccessFull, runTest.removeNotification)

    isTestSuccessFull = bleAdapter.subscribeForNotification(runTest.DUT_INDICATE_CHAR_UUID, subscribe = False) #unsubscribe
    isTestSuccessFull = True
    runTest.submitTestResult(isTestSuccessFull, runTest.removeIndication)

    #Check writing to protected characteristic triggers pairing
    isTestSuccessFull = runTest.pairing()
    runTest.submitTestResult(isTestSuccessFull, runTest.pairing)
    bleAdapter.bondToRemoteDevice()

    #Check writing to protected characteristic after successfull pairing succeed
    time.sleep(2) #wait before starting next test
    isTestSuccessFull = runTest.readWriteProtectedAttributesWhilePaired()
    runTest.submitTestResult(isTestSuccessFull, runTest.readWriteProtectedAttributesWhilePaired)

    #disconnect, Note it is not a test happening on bluez, the DUT is waiting for a disconnect Cb
    runTest.disconnect()

    #reconnect! Since devices bonded, it should not ask for pairing again. Security agent can be destroyed
    securityAgent.removeSecurityAgent() #remove security agent so as not to trigger auto pairing.
    bleAdapter.setDiscoveryFilter(scan_filter)
    bleAdapter.startDiscovery(discoveryStartedCb)#wait for DUT to start advertising
    mainloop.run()
    bleAdapter.stopDiscovery()
    runTest.reconnectWhileBonded()

    #reconnect while not bonded. Pairing should fail since Just works is not accepted
    bleAdapter.disconnect()
    bleAdapter.removeBondedDevices()
    time.sleep(2) #wait for bonded devices to be deleted
    bleAdapter.setDiscoveryFilter(scan_filter)
    bleAdapter.startDiscovery(discoveryEventCb)
    mainloop.run()
    bleAdapter.stopDiscovery()

    agent = securityAgent.createSecurityAgent("NoInputNoOutput", agent)
    runTest.reconnectWhileNotBonded()

    time.sleep(2)
    runTest.printTestsSummary()


def errorConnectCb():
    print("Connection error")
    sys.stdout.flush()
    connectEvent.put(0)
