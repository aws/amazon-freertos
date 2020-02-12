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

import dbus
import os
import sys
import testutils
from gattClient import gattClient
import Queue
import time
import dbus.mainloop.glib
import threading

try:
    from gi.repository import GObject
except ImportError:
    import gobject as GObject

pairingEvent = Queue.Queue()
cancelpairingEvent = Queue.Queue()
connectEvent = Queue.Queue()
disconnectEvent = Queue.Queue()
attributeAccessEvent = Queue.Queue()
mainloop = GObject.MainLoop()


class bleAdapter:
    DBUS_HANDLER_GENERIC_TIMEOUT = 3000  # 3 seconds timeout
    adapter = None
    discoveryEventCb = None
    discoverAllPrimaryServicesEvent = False
    notificationCb = None
    remoteDevice = None
    propertyName = None
    propertyDesiredValue = None

    gatt = gattClient()

    @staticmethod
    def init():
        bleAdapter.adapter = testutils.find_adapter()
        bus = dbus.SystemBus()
        bus.add_signal_receiver(
            bleAdapter.interfaces_added,
            dbus_interface="org.freedesktop.DBus.ObjectManager",
            signal_name="InterfacesAdded")
        bus.add_signal_receiver(
            bleAdapter.properties_changed,
            dbus_interface="org.freedesktop.DBus.Properties",
            signal_name="PropertiesChanged",
            arg0="org.bluez.Device1",
            path_keyword="path")

        bus.add_signal_receiver(
            bleAdapter.properties_changed,
            dbus_interface="org.freedesktop.DBus.Properties",
            signal_name="PropertiesChanged",
            arg0=testutils.CHARACTERISTIC_INTERFACE,
            path_keyword="path")

    @staticmethod
    def discoverAllPrimaryServices(discoverAllPrimaryServicesEventCb):
        bleAdapter.discoverAllPrimaryServicesEventCb = discoverAllPrimaryServicesEventCb

    @staticmethod
    def setDiscoveryFilter(scan_filter):
        bleAdapter.adapter.SetDiscoveryFilter(scan_filter)

    @staticmethod
    def startDiscovery(discoveryEventCb):
        bleAdapter.discoveryEventCb = discoveryEventCb
        bleAdapter.adapter.StartDiscovery()

    @staticmethod
    def stopDiscovery():
        bleAdapter.discoveryEventCb = None
        bleAdapter.adapter.StopDiscovery()

    @staticmethod
    def connect(testDevice):
        bleAdapter.remoteDevice = testDevice
        isSuccessfull = False

        try:
            while (isSuccessfull == False):
                bleAdapter.discoverAllPrimaryServicesEvent = True
                bleAdapter.getDeviceInterface(
                    bleAdapter.remoteDevice).Connect(
                    reply_handler=connectSuccess,
                    error_handler=connectError,
                    timeout=bleAdapter.DBUS_HANDLER_GENERIC_TIMEOUT)

                mainloop.run()
                isSuccessfull = connectEvent.get()
                sys.stdout.flush()
                time.sleep(2)  # Should wait instead for services resolved
        except dbus.exceptions.DBusException as error:
            print("Adapter could not connect:" + str(error))
            print("Retrying after 1s")
            sys.stdout.flush()
            time.sleep(1)
            pass

        return isSuccessfull

    @staticmethod
    def disconnect():
        isSuccessfull = False
        bleAdapter.getDeviceInterface(
            bleAdapter.remoteDevice).Disconnect(
            reply_handler=disconnectSuccess,
            error_handler=disconnectError,
            timeout=bleAdapter.DBUS_HANDLER_GENERIC_TIMEOUT)
        mainloop.run()
        isSuccessfull = disconnectEvent.get()
        return isSuccessfull

    @staticmethod
    def waitForDevicePropertyChange(
            propertyName,
            desiredPropertyValue,
            timeout=None):

        value = bleAdapter.getPropertie(bleAdapter.remoteDevice, propertyName)

        if value == desiredPropertyValue or timeout is None:
            return value
        else:
            bleAdapter.propertyName = propertyName
            bleAdapter.propertyDesiredValue = desiredPropertyValue
            GObject.timeout_add(timeout * 1000, timeoutHandler)
            mainloop.run()
            bleAdapter.propertyName = None
            return bleAdapter.getPropertie(
                bleAdapter.remoteDevice, propertyName)

    @staticmethod
    def isDisconnected(timeout=None):
        ret = False
        try:
            value = bleAdapter.waitForDevicePropertyChange(
                "Connected", 0, timeout)
            ret = (value == 0)
        except Exception as ex:
            print("Caught exception while waiting for disconnect event " + str(ex))
            pass
        return ret

    @staticmethod
    def isPaired(timeout=None):
        ret = False
        try:
            value = bleAdapter.waitForDevicePropertyChange(
                "Paired", 1, timeout)
            ret = (value == 1)
        except Exception as ex:
            print("Caught exception while waiting for paired event" + str(ex))
            pass

        return ret

    @staticmethod
    def isPairable():
        return bleAdapter.getPropertie(
            bleAdapter.adapter,
            "Pairable",
            interface=testutils.ADAPTER_INTERFACE)

    @staticmethod
    def pair_cancelpairing():
        isSuccessfull = True
        try:
            bleAdapter.getDeviceInterface(
                bleAdapter.remoteDevice).Pair(
                reply_handler=pairingSuccess,
                error_handler=pairingError,
                timeout=bleAdapter.DBUS_HANDLER_GENERIC_TIMEOUT)
            bleAdapter.getDeviceInterface(
                bleAdapter.remoteDevice).CancelPairing(
                reply_handler=cancelpairingSuccess,
                error_handler=cancelpairingError,
                timeout=bleAdapter.DBUS_HANDLER_GENERIC_TIMEOUT)
            mainloop.run()
            isSuccessfull = not (pairingEvent.get())
            mainloop.run()
            isSuccessfull &= cancelpairingEvent.get()
        except Exception as e:
            print("BLE ADAPTER: Unable to pair, error: " + str(e))
            sys.stdout.flush()
            isSuccessfull = False
        return isSuccessfull

    @staticmethod
    def pair():
        isSuccessfull = True
        try:
            bleAdapter.getDeviceInterface(
                bleAdapter.remoteDevice).Pair(
                reply_handler=pairingSuccess,
                error_handler=pairingError,
                timeout=bleAdapter.DBUS_HANDLER_GENERIC_TIMEOUT)
            mainloop.run()
            isSuccessfull = pairingEvent.get()
        except Exception as e:
            print("BLE ADAPTER: Unable to pair, error: " + str(e))
            sys.stdout.flush()
            isSuccessfull = False
        return isSuccessfull

    @staticmethod
    def __processEventDeviceInterface(path):
        bus = dbus.SystemBus()
        obj = bus.get_object(testutils.SERVICE_NAME, path)

        if bleAdapter.propertyName is not None:
            propertyValue = bleAdapter.getPropertie(
                obj, bleAdapter.propertyName)
            if propertyValue == bleAdapter.propertyDesiredValue:
                mainloop.quit()

        if bleAdapter.discoveryEventCb is not None:
            bleAdapter.discoveryEventCb.im_func(obj)

        if bleAdapter.discoverAllPrimaryServicesEvent:
            servicesResolved = bleAdapter.getPropertie(
                obj, "ServicesResolved")

            if servicesResolved:
                mainloop.quit()
                bleAdapter.discoverAllPrimaryServicesEvent = False

    @staticmethod
    def __processEventCharInterface(path):
        bus = dbus.SystemBus()
        obj = bus.get_object(testutils.SERVICE_NAME, path)
        if bleAdapter.notificationCb is not None:
            uuid = bleAdapter.getPropertie(
                obj, "UUID", interface=testutils.CHARACTERISTIC_INTERFACE)
            flag = bleAdapter.getPropertie(
                obj, "Flags", interface=testutils.CHARACTERISTIC_INTERFACE)[0]
            value = testutils.convert_dbus_array_to_string(bleAdapter.getPropertie(
                obj, "Value", interface=testutils.CHARACTERISTIC_INTERFACE))
            bleAdapter.notificationCb.im_func(uuid, value, flag)

    @staticmethod
    def interfaces_added(path, interfaces):
        if testutils.DEVICE_INTERFACE in interfaces:
            bleAdapter.__processEventDeviceInterface(path)

        if testutils.CHARACTERISTIC_INTERFACE in interfaces:
            bleAdapter.__processEventCharInterface(path)

    @staticmethod
    def properties_changed(interface, changed, invalidated, path):
        if interface == testutils.DEVICE_INTERFACE:
            bleAdapter.__processEventDeviceInterface(path)
        elif interface == testutils.CHARACTERISTIC_INTERFACE:
            bleAdapter.__processEventCharInterface(path)

    @staticmethod
    def getDeviceInterface(obj):
        return dbus.Interface(obj, testutils.DEVICE_INTERFACE)

    @staticmethod
    def getPropertie(obj, propertie, interface=testutils.DEVICE_INTERFACE):
        objInterface = dbus.Interface(
            obj, dbus_interface='org.freedesktop.DBus.Properties')
        property = None
        try:
            property = objInterface.Get(interface, propertie)
        except Exception as e:
            property = None

        return property

    @staticmethod
    def setPropertie(
            obj,
            propertie,
            value,
            interface=testutils.DEVICE_INTERFACE):
        objInterface = dbus.Interface(
            obj, dbus_interface='org.freedesktop.DBus.Properties')
        return objInterface.Set(interface, propertie, value)

    @staticmethod
    def _writeAttribute(
            objInterface,
            uuid,
            value,
            response=True,
            prepareWrite=False):
        isSuccessfull = True
        try:
            if response:
                objInterface.WriteValue(value,
                                        {"prepare-authorize": prepareWrite},
                                        reply_handler=attributeWrite,
                                        error_handler=attributeAccessError,
                                        timeout=bleAdapter.DBUS_HANDLER_GENERIC_TIMEOUT)
                mainloop.run()
                isSuccessfull = attributeAccessEvent.get()
            else:
                fd, dummy = objInterface.AcquireWrite()
                realfd = fd.take()
                os.write(realfd, value)
                os.close(realfd)
        except Exception as e:
            print(
                "BLE ADAPTER: Can't write attribute: " +
                uuid +
                " error: " +
                str(e))
            sys.stdout.flush()
            isSuccessfull = False
        return isSuccessfull

    @staticmethod
    def isServicesResolved(timeout=None):
        ret = False
        try:
            value = bleAdapter.waitForDevicePropertyChange(
                "ServicesResolved", 1, timeout)
            ret = (value == 1)
        except exception as ex:
            print("Caught exception while waiting for resolved services " + str(ex))
            pass

        return ret

    @staticmethod
    def _readAttribute(objInterface, uuid):
        isSuccessfull = True
        value = ""
        try:
            objInterface.ReadValue(
                {},
                reply_handler=attributeRead,
                error_handler=attributeAccessError,
                timeout=bleAdapter.DBUS_HANDLER_GENERIC_TIMEOUT)
            mainloop.run()
            (isSuccessfull, getAttrValue) = attributeAccessEvent.get()
            value = testutils.convert_dbus_array_to_string(getAttrValue)
        except Exception as e:
            print(
                "BLE ADAPTER: Can't read attribute: " +
                uuid +
                " error: " +
                str(e))
            sys.stdout.flush()
            isSuccessfull = False

        return (isSuccessfull, value)

    @staticmethod
    def writeCharacteristic(
            uuid,
            value,
            response=True,
            offset=0,
            prepareWrite=False,
            reliableWrite=False):
        objInterface = dbus.Interface(
            bleAdapter.gatt.characteristics[uuid]["obj"],
            dbus_interface=testutils.CHARACTERISTIC_INTERFACE)
        return bleAdapter._writeAttribute(
            objInterface, uuid, value, prepareWrite=prepareWrite)

    @staticmethod
    def readCharacteristic(uuid):
        objInterface = dbus.Interface(
            bleAdapter.gatt.characteristics[uuid]["obj"],
            dbus_interface=testutils.CHARACTERISTIC_INTERFACE)
        return bleAdapter._readAttribute(objInterface, uuid)

    @staticmethod
    def writeDescriptor(uuid, value, response=True):
        objInterface = dbus.Interface(
            bleAdapter.gatt.descriptors[uuid]["obj"],
            dbus_interface=testutils.DESCRIPTOR_INTERFACE)
        return bleAdapter._writeAttribute(objInterface, uuid, value)

    @staticmethod
    def readDescriptor(uuid):
        objInterface = dbus.Interface(
            bleAdapter.gatt.descriptors[uuid]["obj"],
            dbus_interface=testutils.DESCRIPTOR_INTERFACE)
        return bleAdapter._readAttribute(objInterface, uuid)

    @staticmethod
    def removeBondedDevices():
        testutils.removeBondedDevices()

    @staticmethod
    def confirmIndicate(uuid):
        objInterface = dbus.Interface(
            bleAdapter.gatt.characteristics[uuid]["obj"],
            dbus_interface=testutils.CHARACTERISTIC_INTERFACE)
        objInterface.Confirm()

    @staticmethod
    def readLocalMTU():
        print(bleAdapter.gatt.characteristics.values()[0])
        sys.stdout.flush()
        objInterface = dbus.Interface(
            bleAdapter.gatt.characteristics["8a7f1168-48af-4efb-83b5-e679f9320004"],
            dbus_interface=testutils.CHARACTERISTIC_INTERFACE)
        print(objInterface)
        sys.stdout.flush()
        fd, dummy = objInterface.AcquireWrite({"MTU": 50})
        realfd = fd.take()
        os.write(realfd, "50")
        os.close(realfd)

    @staticmethod
    def setNotificationCallBack(notificationCb=None):
        bleAdapter.notificationCb = notificationCb

    @staticmethod
    def bondToRemoteDevice():
        bleAdapter.setPropertie(bleAdapter.remoteDevice, "Trusted", True)

    @staticmethod
    def subscribeForNotification(uuid, notificationCb=None, subscribe=True):

        objInterface = dbus.Interface(
            bleAdapter.gatt.characteristics[uuid]["obj"],
            dbus_interface=testutils.CHARACTERISTIC_INTERFACE)
        isSuccessfull = True
        try:
            if subscribe:
                objInterface.StartNotify()
            else:
                objInterface.StopNotify()

        except Exception as e:
            print(
                "BLE ADAPTER: Can't subscribe for notification: " +
                uuid +
                " error: " +
                str(e))
            isSuccessfull = False

        return isSuccessfull

    @staticmethod
    def _notificationErrorCb(uuid, notificationCb):
        print("BLE ADAPTER: Notification error")


def print_normal(address, properties):
    print("[ " + address + " ]")

    for key in properties.keys():
        value = properties[key]
        if isinstance(value, dbus.String):
            value = unicode(value).encode('ascii', 'replace')
        if (key == "Class"):
            print("    %s = 0x%06x" % (key, value))
        else:
            print("    %s = %s" % (key, value))

    print()

    properties["Logged"] = True


def genericReply():
    return


def genericError(error):
    err_name = error.get_dbus_name()
    print("Generic error: %s" % (error))
    sys.stdout.flush()


def attributeWrite():
    attributeAccessEvent.put(True)
    mainloop.quit()


def attributeRead(ReadValue):
    attributeAccessEvent.put((True, ReadValue))
    mainloop.quit()


def attributeAccessError(error):
    print("Error in accessing attribute: %s" % (error))
    sys.stdout.flush()
    attributeAccessEvent.put(False, None)
    mainloop.quit()


def pairingSuccess():
    pairingEvent.put(True)
    mainloop.quit()


def pairingError(error):
    print("Error in pairing: %s" % (error))
    sys.stdout.flush()
    pairingEvent.put(False)
    mainloop.quit()


def cancelpairingSuccess():
    cancelpairingEvent.put(True)
    mainloop.quit()


def cancelpairingError(error):
    print("Error in cancel pairing: %s" % (error))
    sys.stdout.flush()
    cancelpairingEvent.put(False)
    mainloop.quit()


def connectSuccess():
    connectEvent.put(True)
    # don't quit the loop, wait for services to be resolved


def connectError(error):
    print("Error in connect: %s" % (error))
    sys.stdout.flush()
    connectEvent.put(False)
    mainloop.quit()


def disconnectSuccess():
    disconnectEvent.put(True)
    mainloop.quit()


def timeoutHandler():
    print("Timedout Waiting for device property change")
    mainloop.quit()


def disconnectError(error):
    print("Error in connect: %s" % (error))
    sys.stdout.flush()
    disconnectEvent.put(False)
    mainloop.quit()
