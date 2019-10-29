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

import sys
import bleAdapter
from bleAdapter import bleAdapter
import time
import testutils
import dbus.mainloop.glib
try:
  from gi.repository import GObject
except ImportError:
  import gobject as GObject

class runTest:
    mainloop = GObject.MainLoop()

    DUT_GENERIC_STRING = "hello"
    DUT_FAIL_STRING = "fail"
    DUT_OPEN_CHAR_UUID = "8a7f1168-48af-4efb-83b5-e679f9320002"
    DUT_OPEN_DESCR_UUID = "8a7f1168-48af-4efb-83b5-e679f9320008"
    DUT_WRITE_NO_RESP_CHAR_UUID = "8a7f1168-48af-4efb-83b5-e679f9320005"
    DUT_NOTIFY_CHAR_UUID = "8a7f1168-48af-4efb-83b5-e679f9320006"
    DUT_INDICATE_CHAR_UUID = "8a7f1168-48af-4efb-83b5-e679f9320007"

    DUT_ENCRYPT_CHAR_UUID = "8a7f1168-48af-4efb-83b5-e679f9320003"
    DUT_ENCRYPT_DESCR_UUID = "8a7f1168-48af-4efb-83b5-e679f9320009"

    DUT_UUID_128 = "8a7f1168-48af-4efb-83b5-e679f932db5a"
    DUT_UUID_16 = "abcd"
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
    MTU_SIZE = 200

    numberOfTests = 0
    numberOfFailedTests = 0

    # Manufacturer-specific Data
    # First two bytes are company ID (randomly select Espressif(741) for test purpose)
    # Next bytes are defined by the company (randomly select unit8_t 5 for test purpose)
    COMPANY_ID = 741
    MANU_DATA = 5

    testDevice = []

    DUT_MTU_2_STRING = "a" * (MTU_SIZE - 3)
    DUT_LONG_STRING = ["A" * (MTU_SIZE - 3), "B" * (MTU_SIZE - 3), "C" * (MTU_SIZE - 3)]
    DUT_CHAR_E_STRING = "E"
    DUT_CHAR_F_STRING = "F"
    isNotificationDeclinedSuccessFull = False

    testResult = False

    @staticmethod
    def discoveryStoppedCb(testDevice = None):
        global testResult
        testResult = False
        if testDevice == None:
            testResult = True
        runTest.mainloop.quit()

    @staticmethod
    def discoveryStartedCb(testDevice):
        runTest.mainloop.quit()

    @staticmethod
    def discoveryEventCb(testDevice):
        isTestSuccessFull = runTest.advertisement(testDevice)

        if isTestSuccessFull == True:
            runTest.setTestDevice(testDevice)
            #discoveryEvent.set()
            runTest.mainloop.quit()

    @staticmethod
    def discoveryEventCb_16bit(testDevice):
        isTestSuccessFull = runTest.advertisement_16bit(testDevice)

        if isTestSuccessFull == True:
            runTest.setTestDevice(testDevice)
            #discoveryEvent.set()
            runTest.mainloop.quit()

    @staticmethod
    def notificationCb(uuid, value, flag):
        isNotificationTestSuccessFull = runTest.notification(uuid, value, flag)
        if isNotificationTestSuccessFull == True:
            #notificationEvent.set()
            runTest.mainloop.quit()

    @staticmethod
    def indicationCb(uuid, value, flag):
        isIndicationTestSuccessFull = runTest.indication(uuid, value, flag)
        if isIndicationTestSuccessFull == True:
            #indicationEvent.set()
            runTest.mainloop.quit()

    @staticmethod
    def notificationMTUCb(uuid, value, flag):
        notification = runTest.notificationMTU2(uuid, value, flag)
        if notification == runTest.DUT_FAIL_STRING:
            runTest.mainloop.quit()
            runTest.isNotificationDeclinedSuccessFull = True
        if notification == runTest.DUT_MTU_2_STRING:
            runTest.mainloop.quit()
            runTest.isNotificationDeclinedSuccessFull = False

    @staticmethod
    def errorConnectCb():
        print("Connection error")
        sys.stdout.flush()
        connectEvent.put(0)

    @staticmethod
    def stopAdvertisement(scan_filter):
        #Do one cycle of discovery to remove cached messages.
        timerHandle = GObject.timeout_add(runTest.STOP_ADVERTISEMENT_TEST_TIMEOUT, runTest.discoveryStoppedCb)
        bleAdapter.setDiscoveryFilter(scan_filter)
        bleAdapter.startDiscovery(runTest.discoveryStoppedCb)#wait for DUT to start advertising
        runTest.mainloop.run()
        bleAdapter.stopDiscovery()

        #All cached message have been remove. Try again a discovery.
        timerHandle = GObject.timeout_add(runTest.STOP_ADVERTISEMENT_TEST_TIMEOUT, runTest.discoveryStoppedCb)
        bleAdapter.setDiscoveryFilter(scan_filter)
        bleAdapter.startDiscovery(runTest.discoveryStoppedCb)#wait for DUT to start advertising
        runTest.mainloop.run()
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
    def indication(uuid, value, flag):
        isSuccessfull = False
        if (uuid == runTest.DUT_INDICATE_CHAR_UUID) and (value == runTest.DUT_GENERIC_STRING) and (flag == "indicate"):
            isSuccessfull = True

        return isSuccessfull

    @staticmethod
    def notification(uuid, value, flag):
        isSuccessfull = False
        if (uuid == runTest.DUT_NOTIFY_CHAR_UUID) and (value == runTest.DUT_GENERIC_STRING) and (flag == "notify"):
            isSuccessfull = True

        return isSuccessfull

    @staticmethod
    def notificationMTU2(uuid, value, flag):
        if (uuid == runTest.DUT_NOTIFY_CHAR_UUID) and (flag == "notify"):
            return value

    @staticmethod
    def notificationOnCharE(uuid, value, flag):
        isSuccessfull = False
        if (uuid == runTest.DUT_NOTIFY_CHAR_UUID) and (value == runTest.DUT_CHAR_E_STRING) and (flag == "notify"):
            isSuccessfull = True

        return isSuccessfull

    @staticmethod
    def indicationOnCharF(uuid, value, flag):
        isSuccessfull = False
        if (uuid == runTest.DUT_INDICATE_CHAR_UUID) and (value == runTest.DUT_CHAR_F_STRING) and (flag == "indicate"):
            isSuccessfull = True

        return isSuccessfull

    @staticmethod
    def writeWithoutResponse():
        return bleAdapter.writeCharacteristic(runTest.DUT_WRITE_NO_RESP_CHAR_UUID, runTest.DUT_WRITE_NO_RESP_CHAR_UUID, False)

    @staticmethod
    def writeResultWithoutResponse(result):
        return bleAdapter.writeCharacteristic(runTest.DUT_WRITE_NO_RESP_CHAR_UUID, result, False)

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
    def reConnection(isConnected):
        return isConnected

    @staticmethod
    def removeIndication(isSuccessfull):
        return isSuccessfull

    @staticmethod
    def removeNotification(isSuccessfull):
        return isSuccessfull

    @staticmethod
    def advertisement(testDevice, DUT_UUID=None):
        if (DUT_UUID == None): DUT_UUID = runTest.DUT_UUID_128

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
            if (DUT_UUID not in UUIDs):
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
                # return False

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
    def get_manufacture_data(testDevice, DUT_UUID=None):
        manufacture_data_dict = bleAdapter.getPropertie(testDevice, "ManufacturerData")

        # If manufacture data doesn't exist, return None
        if( manufacture_data_dict == None ):
            print("No Manufacture Data")
            sys.stdout.flush()
            return None

        # If manufacture data exists, return manufacture data
        else:
            print( "Manufacturer Specific Data: " + str(manufacture_data_dict.items()) )
            sys.stdout.flush()
            manufacture_data = manufacture_data_dict[runTest.COMPANY_ID]
            return manufacture_data

    @staticmethod
    def _advertisement_start(scan_filter, UUID, discoveryEvent_Cb, bleAdapter):
        scan_filter.update({ "UUIDs": [UUID]})
        bleAdapter.setDiscoveryFilter(scan_filter)

        #Discovery test
        bleAdapter.startDiscovery(discoveryEvent_Cb)
        runTest.mainloop.run()
        bleAdapter.stopDiscovery()

    @staticmethod
    def _simple_connect():
        #Simple Connection test
        testDevice = runTest.getTestDevice()
        isTestSuccessFull = bleAdapter.connect(testDevice)
        time.sleep(2) #wait for connection parameters update

    @staticmethod
    def _advertisement_connection_tests(scan_filter,
            bleAdapter, 
            UUID, 
            discoveryEvent_Cb):
        runTest._advertisement_start(scan_filter=scan_filter, 
                                     UUID=UUID,
                                     discoveryEvent_Cb=discoveryEvent_Cb,
                                     bleAdapter=bleAdapter)
        runTest._simple_connect()
        runTest.stopAdvertisement(scan_filter)
        bleAdapter.disconnect()

        testutils.removeBondedDevices()

    @staticmethod
    def Advertise_Without_Properties(scan_filter,
            bleAdapter):

        DUT_NAME_ORIGINAL = runTest.DUT_NAME
        runTest.DUT_NAME = "nimb"
        runTest._advertisement_connection_tests(scan_filter=scan_filter,
                                                bleAdapter=bleAdapter,
                                                UUID=runTest.DUT_UUID_128,
                                                discoveryEvent_Cb=runTest.discoveryEventCb)
        runTest.DUT_NAME = DUT_NAME_ORIGINAL
        return True

    @staticmethod
    def Advertise_With_Manufacture_Data(scan_filter,
            bleAdapter):
        isTestSuccessFull = True

        # Check when manufacture data length is 0, but pointer is valid
        runTest._advertisement_start(scan_filter=scan_filter, 
                                    UUID=runTest.DUT_UUID_128,
                                    discoveryEvent_Cb=runTest.discoveryEventCb,
                                    bleAdapter=bleAdapter)
        manufacture_data = runTest.get_manufacture_data(runTest.testDevice)
        if manufacture_data != None:
            isTestSuccessFull = False
        runTest._simple_connect()
        runTest.stopAdvertisement(scan_filter)
        isTestSuccessFull &= bleAdapter.disconnect()
        testutils.removeBondedDevices()

        # Check when manufacture data pointer is NULL, but length is not 0
        runTest._advertisement_start(scan_filter=scan_filter, 
                                    UUID=runTest.DUT_UUID_128,
                                    discoveryEvent_Cb=runTest.discoveryEventCb,
                                    bleAdapter=bleAdapter)
        manufacture_data = runTest.get_manufacture_data(runTest.testDevice)
        if manufacture_data != None:
            isTestSuccessFull = False
        runTest._simple_connect()
        runTest.stopAdvertisement(scan_filter)
        isTestSuccessFull &= bleAdapter.disconnect()
        testutils.removeBondedDevices()

        # Check when manufacture data length is not 0, and pointer is valid
        runTest._advertisement_start(scan_filter=scan_filter, 
                                    UUID=runTest.DUT_UUID_128,
                                    discoveryEvent_Cb=runTest.discoveryEventCb,
                                    bleAdapter=bleAdapter)
        manufacture_data = runTest.get_manufacture_data(runTest.testDevice)
        for data in manufacture_data:
            if data != runTest.MANU_DATA:
                isTestSuccessFull = False
        runTest._simple_connect()
        runTest.stopAdvertisement(scan_filter)
        isTestSuccessFull &= bleAdapter.disconnect()
        testutils.removeBondedDevices()

        return isTestSuccessFull

    @staticmethod
    def Advertise_With_16bit_ServiceUUID(scan_filter,
            bleAdapter):
        runTest._advertisement_connection_tests(scan_filter=scan_filter,
                                                bleAdapter=bleAdapter,
                                                UUID=runTest.DUT_UUID_16,
                                                discoveryEvent_Cb=runTest.discoveryEventCb_16bit)
        return True

    @staticmethod
    def Write_Notification_Size_Greater_Than_MTU_3(scan_filter,
                                                        bleAdapter):
        runTest._advertisement_connection_tests(scan_filter=scan_filter,
                                                bleAdapter=bleAdapter,
                                                UUID=runTest.DUT_UUID_128,
                                                discoveryEvent_Cb=runTest.discoveryEventCb)
        runTest._simple_connect()
        runTest.stopAdvertisement(scan_filter)
        bleAdapter.gatt.updateLocalAttributeTable()

        # Data size > MTU - 3 send notification test
        bleAdapter.subscribeForNotification(runTest.DUT_NOTIFY_CHAR_UUID) #subscribe for next test
        isTestSuccessFull = True
        runTest.mainloop.run()
        isTestSuccessFull &= runTest.isNotificationDeclinedSuccessFull

        # unsubscribe
        isTestSuccessFull &= bleAdapter.subscribeForNotification(runTest.DUT_NOTIFY_CHAR_UUID, subscribe = False) #unsubscribe

        isTestSuccessFull &= bleAdapter.disconnect()
        testutils.removeBondedDevices()

        return isTestSuccessFull

    @staticmethod
    def advertisement_16bit(testDevice):
        return runTest.advertisement(testDevice, DUT_UUID=runTest.UUID_16to128(runTest.DUT_UUID_16))

    @staticmethod
    def UUID_16to128(UUID_16bit):
        return "0000"+UUID_16bit+"-0000-1000-8000-00805f9b34fb"

    @staticmethod
    def setTestDevice(testDeviceTmp):
        runTest.testDevice = testDeviceTmp

    @staticmethod
    def getTestDevice():
        return runTest.testDevice

    @staticmethod
    def submitTestResult(isSuccessfull, testMethod):
        switch = {
            runTest.advertisement: "_advertisement",
            runTest.discoverPrimaryServices: "_discoverPrimaryServices",
            runTest.simpleConnection: "_simpleConnection",
            runTest.reConnection: "_reConnection",
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
            runTest.stopAdvertisement: "_stopAdvertisement",
            runTest.Advertise_Without_Properties: "_Advertise_Without_Properties",
            runTest.Advertise_With_16bit_ServiceUUID: "_Advertise_With_16bit_ServiceUUID",
            runTest.Advertise_With_Manufacture_Data: "_Advertise_With_Manufacture_Data",
            # runTest.Advertise_Interval_Consistent_After_BT_Reset: "_Advertise_Interval_Consistent_After_BT_Reset",
            runTest.Write_Notification_Size_Greater_Than_MTU_3: "_Write_Notification_Size_Greater_Than_MTU_3"
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
