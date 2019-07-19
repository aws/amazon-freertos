import dbus
import os, sys
import testutils
from gattClient import gattClient
import Queue
import time
import dbus.mainloop.glib
try:
  from gi.repository import GObject
except ImportError:
  import gobject as GObject

pairingEvent = Queue.Queue()
connectEvent = Queue.Queue()
disconnectEvent = Queue.Queue()
attributeAccessEvent = Queue.Queue()
mainloop = GObject.MainLoop()

class bleAdapter:
    DBUS_HANDLER_GENERIC_TIMEOUT = 3000 #3 seconds timeout
    adapter = None
    discoveryEventCb = None
    serviceResolvedEventCb = None
    discoverAllPrimaryServicesEvent = False
    notificationCb = None
    remoteDevice = None

    gatt = gattClient()

    @staticmethod
    def init():
        bleAdapter.adapter = testutils.find_adapter()
        bus = dbus.SystemBus()
        bus.add_signal_receiver(bleAdapter.interfaces_added,
                                 dbus_interface = "org.freedesktop.DBus.ObjectManager",
                                 signal_name = "InterfacesAdded")
        bus.add_signal_receiver(bleAdapter.properties_changed,
                dbus_interface = "org.freedesktop.DBus.Properties",
                signal_name = "PropertiesChanged",
                arg0 = "org.bluez.Device1",
                path_keyword = "path")

        bus.add_signal_receiver(bleAdapter.properties_changed,
                dbus_interface = "org.freedesktop.DBus.Properties",
                signal_name = "PropertiesChanged",
                arg0 = testutils.CHARACTERISTIC_INTERFACE,
                path_keyword = "path")

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
                bleAdapter.getDeviceInterface(bleAdapter.remoteDevice).Connect(reply_handler=connectSuccess, error_handler=connectError,
                                timeout=bleAdapter.DBUS_HANDLER_GENERIC_TIMEOUT)

                mainloop.run()
                isSuccessfull = connectEvent.get()
                sys.stdout.flush()
                time.sleep(2)#Should wait instead for services resolved
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
        bleAdapter.getDeviceInterface(bleAdapter.remoteDevice).Disconnect(reply_handler=disconnectSuccess, error_handler=disconnectError,
                                timeout=bleAdapter.DBUS_HANDLER_GENERIC_TIMEOUT)
        mainloop.run()
        isSuccessfull = disconnectEvent.get()
        return isSuccessfull

    @staticmethod
    def isPaired():
        return bleAdapter.getPropertie(bleAdapter.remoteDevice, "Paired")

    @staticmethod
    def isPairable():
        return bleAdapter.getPropertie(bleAdapter.adapter, "Pairable", interface=testutils.ADAPTER_INTERFACE )

    @staticmethod
    def pair():
        isSuccessfull = True
        try:
            bleAdapter.getDeviceInterface(bleAdapter.remoteDevice).Pair(reply_handler=pairingSuccess, error_handler=pairingError,
                                timeout=bleAdapter.DBUS_HANDLER_GENERIC_TIMEOUT)
            mainloop.run()
            isSuccessfull = pairingEvent.get()
        except Exception as e:
            print("BLE ADAPTER: Unable to pair, error: "+ str(e))
            sys.stdout.flush()
            isSuccessfull = False
        return isSuccessfull

    @staticmethod
    def __processEventDeviceInterface(path):
        bus = dbus.SystemBus()
        obj = bus.get_object(testutils.SERVICE_NAME, path)

        if bleAdapter.discoveryEventCb != None:
            bleAdapter.discoveryEventCb.im_func(obj)

        if bleAdapter.discoverAllPrimaryServicesEvent != False:
            isServicesResolved = bleAdapter.getPropertie(obj, "ServicesResolved")

            if isServicesResolved == True:
                mainloop.quit()
                bleAdapter.discoverAllPrimaryServicesEvent = False

    @staticmethod
    def __processEventCharInterface(path):
        bus = dbus.SystemBus()
        obj = bus.get_object(testutils.SERVICE_NAME, path)
        if bleAdapter.notificationCb != None:
            uuid = bleAdapter.getPropertie(obj, "UUID", interface=testutils.CHARACTERISTIC_INTERFACE)
            value = testutils.convert_dbus_array_to_string(bleAdapter.getPropertie(obj, "Value", interface=testutils.CHARACTERISTIC_INTERFACE))
            bleAdapter.notificationCb.im_func(uuid, value)

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
        elif  interface == testutils.CHARACTERISTIC_INTERFACE:
            bleAdapter.__processEventCharInterface(path)

    @staticmethod
    def getDeviceInterface(obj):
        return dbus.Interface(obj, testutils.DEVICE_INTERFACE)

    @staticmethod
    def getPropertie(obj, propertie, interface = testutils.DEVICE_INTERFACE):
        objInterface  =  dbus.Interface(obj, dbus_interface='org.freedesktop.DBus.Properties')
        property = None
        try:
            property = objInterface.Get(interface, propertie)
        except Exception as e:
            property = None

        return property

    @staticmethod
    def setPropertie(obj, propertie, value, interface = testutils.DEVICE_INTERFACE):
        objInterface  =  dbus.Interface(obj, dbus_interface='org.freedesktop.DBus.Properties')
        return objInterface.Set(interface, propertie, value)

    @staticmethod
    def _writeAttribute(objInterface, uuid, value, response = True, prepareWrite = False):
        isSuccessfull = True
        try:
            if response == True:
                objInterface.WriteValue(value, {"prepare-authorize": prepareWrite}, reply_handler=attributeWrite, error_handler=attributeAccessError,
                                timeout=bleAdapter.DBUS_HANDLER_GENERIC_TIMEOUT)
                mainloop.run()
                isSuccessfull = attributeAccessEvent.get()
            else:
                fd, dummy = objInterface.AcquireWrite()
                realfd = fd.take()
                os.write(realfd, value)
                os.close(realfd)
        except Exception as e:
            print("BLE ADAPTER: Can't write attribute: "+uuid+" error: "+str(e))
            sys.stdout.flush()
            isSuccessfull = False
        return isSuccessfull

    @staticmethod
    def _readAttribute(objInterface, uuid):
        isSuccessfull = True
        value = ""
        try:
            objInterface.ReadValue({}, reply_handler=attributeRead, error_handler=attributeAccessError,
                                timeout=bleAdapter.DBUS_HANDLER_GENERIC_TIMEOUT)
            mainloop.run()
            (isSuccessfull, getAttrValue) = attributeAccessEvent.get()
            value = testutils.convert_dbus_array_to_string(getAttrValue)
        except Exception as e:
            print("BLE ADAPTER: Can't read attribute: "+uuid+" error: "+str(e))
            sys.stdout.flush()
            isSuccessfull = False

        return (isSuccessfull, value)

    @staticmethod
    def writeCharacteristic(uuid, value, response = True, prepareWrite=False):
        objInterface  =  dbus.Interface(bleAdapter.gatt.characteristics[uuid]["obj"], dbus_interface=testutils.CHARACTERISTIC_INTERFACE)
        return bleAdapter._writeAttribute(objInterface, uuid, value, prepareWrite = prepareWrite)

    @staticmethod
    def readCharacteristic(uuid):
        objInterface  =  dbus.Interface(bleAdapter.gatt.characteristics[uuid]["obj"], dbus_interface=testutils.CHARACTERISTIC_INTERFACE)
        return bleAdapter._readAttribute(objInterface, uuid)

    @staticmethod
    def writeDescriptor(uuid, value, response = True):
        objInterface  =  dbus.Interface(bleAdapter.gatt.descriptors[uuid]["obj"], dbus_interface=testutils.DESCRIPTOR_INTERFACE)
        return bleAdapter._writeAttribute(objInterface, uuid, value)

    @staticmethod
    def readDescriptor(uuid):
        objInterface  =  dbus.Interface(bleAdapter.gatt.descriptors[uuid]["obj"], dbus_interface=testutils.DESCRIPTOR_INTERFACE)
        return bleAdapter._readAttribute(objInterface, uuid)

    @staticmethod
    def removeBondedDevices():
        testutils.removeBondedDevices()

    @staticmethod
    def confirmIndicate(uuid):
        objInterface  =  dbus.Interface(bleAdapter.gatt.characteristics[uuid]["obj"], dbus_interface=testutils.CHARACTERISTIC_INTERFACE)
        objInterface.Confirm()

    @staticmethod
    def readLocalMTU():
        print(bleAdapter.gatt.characteristics.values()[0])
        sys.stdout.flush()
        objInterface  =  dbus.Interface(bleAdapter.gatt.characteristics["8a7f1168-48af-4efb-83b5-e679f9320004"], dbus_interface=testutils.CHARACTERISTIC_INTERFACE)
        print(objInterface)
        sys.stdout.flush()
        fd, dummy = objInterface.AcquireWrite({"MTU": 50})
        realfd = fd.take()
        os.write(realfd, "50")
        os.close(realfd)

    @staticmethod
    def setNotificationCallBack(notificationCb = None):
        bleAdapter.notificationCb = notificationCb

    @staticmethod
    def bondToRemoteDevice():
        bleAdapter.setPropertie(bleAdapter.remoteDevice, "Trusted", True)

    @staticmethod
    def subscribeForNotification(uuid, notificationCb = None, subscribe = True):

        objInterface  =  dbus.Interface(bleAdapter.gatt.characteristics[uuid]["obj"], dbus_interface=testutils.CHARACTERISTIC_INTERFACE)        
        isSuccessfull = True
        try:
            if subscribe == True:
                objInterface.StartNotify()
            else:
                objInterface.StopNotify()

        except Exception as e:
            print("BLE ADAPTER: Can't subscribe for notification: "+uuid+" error: "+str(e))
            isSuccessfull = False

        return isSuccessfull

    @staticmethod
    def _notificationErrorCb(uuid, notificationCb):
        print("BLE ADAPTER: Notification error")


def print_normal(address, properties):
    print("[ " + address + " ]")

    for key in properties.keys():
        value = properties[key]
        if type(value) is dbus.String:
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

def connectSuccess():
    connectEvent.put(True)
    #don't quit the loop, wait for services to be resolved

def connectError(error):
    print("Error in connect: %s" % (error))
    sys.stdout.flush()
    connectEvent.put(False)
    mainloop.quit()

def disconnectSuccess():
    disconnectEvent.put(True)
    mainloop.quit()

def disconnectError(error):
    print("Error in connect: %s" % (error))
    sys.stdout.flush()
    disconnectEvent.put(False)
    mainloop.quit()
