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
    scan_filter = dict()

    bleAdapter.init()
    agent = securityAgent.createSecurityAgent(agent=agent)

    scan_filter.update({ "UUIDs": [runTest.DUT_UUID]})
    bleAdapter.setDiscoveryFilter(scan_filter)

    #KPI test
    isTestSuccessFull = True
    startToReceivedSum = 0
    startToConnectedSum = 0
    numberOfReconnect = 3
    numberOfSuccess = 0

    for i in range(numberOfReconnect):
        if i == 0:
            bleAdapter.startDiscovery(discoveryEventCb)
        else:
            bleAdapter.startDiscovery(discoveryStartedCb)   #wait for DUT to start advertising
        tStartScan = time.time()
        mainloop.run()
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
    agent = teardown_test(agent)

def errorConnectCb():
    print("Connection error")
    sys.stdout.flush()
    connectEvent.put(0)
