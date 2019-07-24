import sys
import bleAdapter
from bleAdapter import bleAdapter
import time
import dbus.mainloop.glib
try:
  from gi.repository import GObject
except ImportError:
  import gobject as GObject

mainloop = GObject.MainLoop()
testResult = None

def discoveryStoppedCb(testDevice = None):
    global testResult
    testResult = False
    if testDevice == None:
        testResult = True
    mainloop.quit()

class runTest:
    DUT_GENERIC_STRING = "hello"
    DUT_OPEN_CHAR_UUID = "8a7f1168-48af-4efb-83b5-e679f9320002"
    DUT_OPEN_DESCR_UUID = "8a7f1168-48af-4efb-83b5-e679f9320008"
    DUT_WRITE_NO_RESP_CHAR_UUID = "8a7f1168-48af-4efb-83b5-e679f9320005"
    DUT_NOTIFY_CHAR_UUID = "8a7f1168-48af-4efb-83b5-e679f9320006"
    DUT_INDICATE_CHAR_UUID = "8a7f1168-48af-4efb-83b5-e679f9320007"

    DUT_ENCRYPT_CHAR_UUID = "8a7f1168-48af-4efb-83b5-e679f9320003"
    DUT_ENCRYPT_DESCR_UUID = "8a7f1168-48af-4efb-83b5-e679f9320009"

    DUT_UUID = "8a7f1168-48af-4efb-83b5-e679f932db5a"
    DUT_SERVICEB_UUID = "8a7f1168-48af-4efb-83b5-e679f9320001"
    DUT_CHAR= {"8a7f1168-48af-4efb-83b5-e679f9320002": {"Flags":"read, write"},
               "8a7f1168-48af-4efb-83b5-e679f9320003": {"Flags":"read, write"},
               "8a7f1168-48af-4efb-83b5-e679f9320004": {"Flags":"read, write"},
               "8a7f1168-48af-4efb-83b5-e679f9320005": {"Flags":"write-without-response"},
               "8a7f1168-48af-4efb-83b5-e679f9320006": {"Flags":"notify"},
               "8a7f1168-48af-4efb-83b5-e679f9320007": {"Flags":"indicate"}}

    DUT_DESCR = {"8a7f1168-48af-4efb-83b5-e679f9320008":  None,
                 "8a7f1168-48af-4efb-83b5-e679f9320009":  None,
                 "8a7f1168-48af-4efb-83b5-e679f932000a":  None,
                 "8a7f1168-48af-4efb-83b5-e679f932000b":  None  }

    DUT_NAME = "TEST"

    TEST_GROUP = "Full_BLE"
    TEST_NAME_PREFIX = "RaspberryPI"

    SHORT_LOCAL_NAME_SIZE = 4
    ADVERTISEMENT_TEST_TIMEOUT = 120
    STOP_ADVERTISEMENT_TEST_TIMEOUT = 2000 #2 seconds
    SIMPLE_CONNECTION_TEST_TIMEOUT = 120
    SERVICE_DISCOVERY_TEST_TIMEOUT = 120
    PAIRING_TEST_TIMEOUT = 120
    GENERIC_TEST_TIMEOUT = 120

    numberOfTests = 0
    numberOfFailedTests = 0

    testDevice = []

    @staticmethod
    def stopAdvertisement(scan_filter):
        #Do one cycle of discovery to remove cached messages.
        timerHandle = GObject.timeout_add(runTest.STOP_ADVERTISEMENT_TEST_TIMEOUT, discoveryStoppedCb)
        bleAdapter.setDiscoveryFilter(scan_filter)
        bleAdapter.startDiscovery(discoveryStoppedCb)#wait for DUT to start advertising
        mainloop.run()
        bleAdapter.stopDiscovery()

        #All cached message have been remove. Try again a discovery.
        timerHandle = GObject.timeout_add(runTest.STOP_ADVERTISEMENT_TEST_TIMEOUT, discoveryStoppedCb)
        bleAdapter.setDiscoveryFilter(scan_filter)
        bleAdapter.startDiscovery(discoveryStoppedCb)#wait for DUT to start advertising
        mainloop.run()
        runTest.submitTestResult(testResult, runTest.stopAdvertisement)
        bleAdapter.stopDiscovery()

    @staticmethod
    def reconnectWhileNotBonded():
        isTestSuccessFull = bleAdapter.connect(runTest.testDevice)
        if isTestSuccessFull == False:
            print("reconnectWhileNotBonded test: Could not connect")
            sys.stdout.flush()
            runTest.submitTestResult(False, runTest.reconnectWhileNotBonded)
            return

        #Since secure connection only are accepted, pairing in "just works" shoud get rejected
        if bleAdapter.pair() == True:
            print("reconnectWhileNotBonded test: Able to pair in just Works mode")
            sys.stdout.flush()
            runTest.submitTestResult(False, runTest.reconnectWhileNotBonded)
            return

        runTest.submitTestResult(True, runTest.reconnectWhileNotBonded)

    @staticmethod
    def reconnectWhileBonded():
        isTestSuccessFull = bleAdapter.connect(runTest.testDevice)
        #since there is a bond with DUT, pairing is automatic
        if( isTestSuccessFull == True):
            isTestSuccessfull = bleAdapter.writeCharacteristic(runTest.DUT_ENCRYPT_CHAR_UUID, runTest.DUT_ENCRYPT_CHAR_UUID)
        runTest.submitTestResult(isTestSuccessFull, runTest.reconnectWhileBonded)


    @staticmethod
    def disconnect():
        isTestSuccessFull = bleAdapter.disconnect()
        runTest.submitTestResult(isTestSuccessFull, runTest.disconnect)

    @staticmethod
    def pairing():
        isTestSuccessFull = True
        if bleAdapter.isPaired() == False:
            bleAdapter.writeCharacteristic(runTest.DUT_ENCRYPT_CHAR_UUID, runTest.DUT_ENCRYPT_CHAR_UUID) #should trigger a pairing event
            isTestSuccessFull = bleAdapter.isPaired()
        else:
            isTestSuccessFull = False
        return isTestSuccessFull

    @staticmethod
    def _readWriteProtectedAttributes(pairingStatus):

        if pairingStatus == True:
            expectedSuccess = True
        else:
            expectedSuccess = False

        isTestSuccessfull = bleAdapter.writeDescriptor(runTest.DUT_ENCRYPT_DESCR_UUID, runTest.DUT_ENCRYPT_DESCR_UUID)
        if isTestSuccessfull != expectedSuccess:
            print("readWriteProtectedAttributes test: Error while reading protect descriptor, pairing status was "+ str(pairingStatus) + " Operation success was " + str(isTestSuccessfull))
            sys.stdout.flush()
            return False


        isTestSuccessfull = bleAdapter.writeCharacteristic(runTest.DUT_ENCRYPT_CHAR_UUID, runTest.DUT_ENCRYPT_CHAR_UUID)
        if isTestSuccessfull != expectedSuccess:
            print("readWriteProtectedAttributes test: Error while writing protect characteristic, pairing status was "+ str(pairingStatus) + " Operation success was " + str(isTestSuccessfull))
            sys.stdout.flush()
            return False

        return True

    # Expect writing/Reading to protect attribute to succeed.
    @staticmethod
    def readWriteProtectedAttributesWhilePaired():
        isPaired = bleAdapter.isPaired()
        if isPaired != True:
            print("readWriteProtectedCharacteristicWhileNotPaired test: Expected paired:1, got:"+str(isPaired))
            sys.stdout.flush()
            return False

        return runTest._readWriteProtectedAttributes(True)

    # Expect writing/Reading to protect attribute to fail.
    @staticmethod
    def readWriteProtectedAttributesWhileNotPaired():
        isPaired = bleAdapter.isPaired()
        if isPaired != False:
            print("readWriteProtectedCharacteristicWhileNotPaired test: Expected paired:0, got:"+str(isPaired))
            sys.stdout.flush()
            return False

        return runTest._readWriteProtectedAttributes(False)


    @staticmethod
    def indication(uuid, value):
        isSuccessfull = False
        if (uuid == runTest.DUT_INDICATE_CHAR_UUID) and (value == runTest.DUT_GENERIC_STRING):
            isSuccessfull = True

        return isSuccessfull

    @staticmethod
    def notification(uuid, value):
        isSuccessfull = False
        if (uuid == runTest.DUT_NOTIFY_CHAR_UUID) and (value == runTest.DUT_GENERIC_STRING):
            isSuccessfull = True

        return isSuccessfull

    @staticmethod
    def writeWithoutResponse():
        return bleAdapter.writeCharacteristic(runTest.DUT_WRITE_NO_RESP_CHAR_UUID, runTest.DUT_WRITE_NO_RESP_CHAR_UUID, False)

    @staticmethod
    def _readWriteChecks(charUUID, descrUUID):
        bleAdapter.writeCharacteristic(charUUID, charUUID)
        bleAdapter.writeDescriptor(descrUUID, descrUUID)

        (isTestSuccessfull, charRead) = bleAdapter.readCharacteristic(charUUID)
        (isTestSuccessfull, descrRead) = bleAdapter.readDescriptor(descrUUID)

        if charRead != charUUID:
            isTestSuccessfull = False
            print("readWriteSimpleConnection test: Expected char uuid:"+charUUID+" got:"+charRead)

        if descrRead != descrUUID:
            isTestSuccessfull = False
            print("readWriteSimpleConnection test: Expected descr uuid:"+descrUUID+" got:"+descrRead)

        sys.stdout.flush()
        return isTestSuccessfull

    @staticmethod
    def readWriteSimpleConnection():
        isTestSuccessfull = runTest._readWriteChecks(runTest.DUT_OPEN_CHAR_UUID, runTest.DUT_OPEN_DESCR_UUID)

        isPaired = bleAdapter.isPaired()

        if isPaired != False:
            isTestSuccessfull = False
            print("readWriteSimpleConnection test: Expected paired:0, got:"+str(isPaired))

        sys.stdout.flush()
        return isTestSuccessfull

    @staticmethod
    def discoverPrimaryServices():
        return bleAdapter.getPropertie(runTest.testDevice, "ServicesResolved")

    @staticmethod
    def checkProperties(gatt):
        isTestSuccessfull = True

        for uuid in runTest.DUT_CHAR.keys():
            if runTest.DUT_CHAR[uuid]["Flags"] != gatt.characteristics[uuid]["Flags"]:
                print("checkProperties test: incorrect flags, expected: "+runTest.DUT_CHAR[uuid]["Flags"]+" was: "+gatt.characteristics[uuid]["Flags"])
                isTestSuccessfull = False

        sys.stdout.flush()
        return isTestSuccessfull

    @staticmethod
    def checkUUIDs(gatt):
        isTestSuccessfull = True
        if runTest.DUT_SERVICEB_UUID not in gatt.services.keys():
            print("checkUUIDs test: missing service UUID: "+runTest.DUT_SERVICEB_UUID)
            isTestSuccessfull = False

        #Check characteristics UUIDs
        for uuid in runTest.DUT_CHAR.keys():
            if uuid not in gatt.characteristics.keys():
                print("checkUUIDs test: missing characteristic UUID: "+uuid)
                isTestSuccessfull = False


        #Check descriptors
        for uuid in runTest.DUT_DESCR.keys():
            if uuid not in gatt.descriptors.keys():
                print("checkUUIDs test: missing descriptors UUID: "+uuid)
                isTestSuccessfull = False

        sys.stdout.flush()
        return isTestSuccessfull

    @staticmethod
    def simpleConnection(isConnected):
        return isConnected

    @staticmethod
    def removeIndication(isSuccessfull):
        return isSuccessfull

    @staticmethod
    def removeNotification(isSuccessfull):
        return isSuccessfull

    @staticmethod
    def advertisement(testDevice):

        if (bleAdapter.getPropertie(testDevice, "Address") == None):
            print("Advertisement test: Waiting for Address")
            sys.stdout.flush()
            return False

        UUIDs = bleAdapter.getPropertie(testDevice, "UUIDs")
        if (UUIDs == None):
            print("Advertisement test: Waiting for UUIDs")
            sys.stdout.flush()
            return False
        else:
            if (runTest.DUT_UUID not in UUIDs):
                print("Advertisement test: Waiting for device UUID")
                sys.stdout.flush()
                return False

            #Remove test for service B. Advertisement messages were too small.
            #Should look into improving this part if it can be done.
            #if (runTest.DUT_SERVICEB_UUID not in UUIDs):
            #   print("Advertisement test: Waiting for serviceB UUID")
            #   sys.stdout.flush()
            #   return False

        name =  bleAdapter.getPropertie(testDevice, "Name")
        if(name == None):
            print("Advertisement test: Waiting name")
            sys.stdout.flush()
            return False
        else:
            #Names can be cached. So the complete local name may still be in memory. Check the 4 first letter which constitutes the short name
            if (runTest.DUT_NAME != name[:runTest.SHORT_LOCAL_NAME_SIZE]):
                print("Advertisement test: name is incorrect: " + name)
                sys.stdout.flush()
                return False

        if (bleAdapter.getPropertie(testDevice, "TxPower") == None):
            print("Advertisement test: Waiting for TxPower")
            sys.stdout.flush()
            return False

        if( bleAdapter.getPropertie(testDevice, "RSSI") == None):
            print("Advertisement test: Waiting for RSSI")
            sys.stdout.flush()
            return False

        return True

    @staticmethod
    def submitTestResult(isSuccessfull, testMethod):
        switch = {
            runTest.advertisement: "_advertisement",
            runTest.discoverPrimaryServices: "_discoverPrimaryServices",
            runTest.simpleConnection: "_simpleConnection",
            runTest.checkProperties: "_checkProperties",
            runTest.checkUUIDs: "_checkUUIDs",
            runTest.readWriteSimpleConnection: "_readWriteSimpleConnection",
            runTest.writeWithoutResponse: "_writeWithoutResponse",
            runTest.notification:"_notification",
            runTest.indication:"_indication",
            runTest.removeNotification:"_removeNotification",
            runTest.removeIndication:"_removeIndication",
            runTest.readWriteProtectedAttributesWhileNotPaired:"_readWriteProtectedAttributesWhileNotPaired",
            runTest.readWriteProtectedAttributesWhilePaired:"_readWriteProtectedAttributesWhilePaired",
            runTest.pairing: "_pairing",
            runTest.disconnect: "_disconnect",
            runTest.reconnectWhileBonded: "_reconnectWhileBonded",
            runTest.reconnectWhileNotBonded: "_reconnectWhileNotBonded",
            runTest.stopAdvertisement: "_stopAdvertisement"
        }

        runTest.numberOfTests += 1

        if(isSuccessfull == True):
            successString = "PASS"
        else:
            successString = "FAIL"
            runTest.numberOfFailedTests += 1

        print("TEST("+runTest.TEST_GROUP+", "+runTest.TEST_NAME_PREFIX + switch.get(testMethod, "methodNotFound")+") "+successString)
        sys.stdout.flush()

    @staticmethod
    def printTestsSummary():
        print("-----------------------")
        print(str(runTest.numberOfTests)+ " Tests "+str(runTest.numberOfFailedTests)+" Failures 0 Ignored")
        sys.stdout.flush()

    @staticmethod
    def setTestDevice(testDeviceTmp):
        runTest.testDevice = testDeviceTmp

    @staticmethod
    def getTestDevice():
        return runTest.testDevice
