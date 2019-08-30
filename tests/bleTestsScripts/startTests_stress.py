import Queue
import sys
import os
import threading
import securityAgent
import time
from testClass import runTest
from bleAdapter import bleAdapter
import testutils
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

def teardown_test(agent):
    securityAgent.removeSecurityAgent()

    os.system("sudo rm -rf \"/var/lib/bluetooth/*\"")
    os.system("sudo hciconfig hci0 reset")

    testutils.removeBondedDevices()
    return agent

def main():
    agent = None
    numberOfReconnect = 1
    numberOfInit = 1
    numberOfEnable = 1
    isTestSuccessFull = True

    scan_filter = dict()

    bleAdapter.init()
    agent = securityAgent.createSecurityAgent(agent=agent)

    scan_filter.update({ "UUIDs": [runTest.DUT_UUID]})

    for i in range(numberOfInit * numberOfEnable):
        bleAdapter.setDiscoveryFilter(scan_filter)
        for j in range(numberOfReconnect):
            if i == 0 and j == 0:
                bleAdapter.startDiscovery(discoveryEventCb)
            else:
                bleAdapter.startDiscovery(discoveryStartedCb) 
            mainloop.run() 
            bleAdapter.stopDiscovery()
            
            testDevice = runTest.getTestDevice()
            isConnectSuccessFull = bleAdapter.connect(testDevice)
            
            isTestSuccessFull &= isConnectSuccessFull
            runTest.stopAdvertisement(scan_filter)
            isTestSuccessFull &= bleAdapter.disconnect()
        
        if numberOfReconnect:
            runTest.submitTestResult(isTestSuccessFull, runTest.reConnection)
        # runTest.setTestDevice([]);
        time.sleep(2)

    runTest.printTestsSummary()
    agent = teardown_test(agent)

def errorConnectCb():
    print("Connection error")
    sys.stdout.flush()
    connectEvent.put(0)
