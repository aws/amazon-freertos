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
import sys

SERVICE_NAME = "org.bluez"
ADAPTER_INTERFACE = SERVICE_NAME + ".Adapter1"
DEVICE_INTERFACE = SERVICE_NAME + ".Device1"
SERVICE_INTERFACE = SERVICE_NAME + ".GattService1"
CHARACTERISTIC_INTERFACE = SERVICE_NAME + ".GattCharacteristic1"
DESCRIPTOR_INTERFACE = SERVICE_NAME + ".GattDescriptor1"
AGENT_INTERFACE = SERVICE_NAME + ".Agent1"
AGENT_MANAGER_INTERFACE = SERVICE_NAME + ".AgentManager1"

ENABLE_TC_AFQP_ADD_INCLUDED_SERVICE = 0

def get_managed_objects():
    bus = dbus.SystemBus()
    manager = dbus.Interface(bus.get_object("org.bluez", "/"),
                             "org.freedesktop.DBus.ObjectManager")
    return manager.GetManagedObjects()


def find_adapter(pattern=None):
    return find_adapter_in_objects(get_managed_objects(), pattern)


def find_adapter_in_objects(objects, pattern=None):
    bus = dbus.SystemBus()
    for path, ifaces in objects.items():
        adapter = ifaces.get(ADAPTER_INTERFACE)
        if adapter is None:
            continue
        if not pattern or pattern == adapter["Address"] or \
                path.endswith(pattern):
            obj = bus.get_object(SERVICE_NAME, path)
            return dbus.Interface(obj, ADAPTER_INTERFACE)
    raise Exception("Bluetooth adapter not found")


def removeBondedDevices():
    bus = dbus.SystemBus()
    adapter = find_adapter()
    om = dbus.Interface(bus.get_object("org.bluez", "/"),
                        "org.freedesktop.DBus.ObjectManager")
    objects = om.GetManagedObjects()
    for path, interfaces in objects.iteritems():
        if "org.bluez.Device1" in interfaces:
            adapter.RemoveDevice(path)


def find_device(device_address, adapter_pattern=None):
    return find_device_in_objects(get_managed_objects(), device_address,
                                  adapter_pattern)


def find_device_in_objects(objects, device_address, adapter_pattern=None):
    bus = dbus.SystemBus()
    path_prefix = ""
    if adapter_pattern:
        adapter = find_adapter_in_objects(objects, adapter_pattern)
        path_prefix = adapter.object_path
    for path, ifaces in objects.items():
        device = ifaces.get(DEVICE_INTERFACE)
        if device is None:
            continue
        if (device["Address"] == device_address and
                path.startswith(path_prefix)):
            obj = bus.get_object(SERVICE_NAME, path)
            return dbus.Interface(obj, DEVICE_INTERFACE)

    raise Exception("Bluetooth device not found")


BLUEZ_FLAGS = ["broadcast",
               "read",
               "write-without-response",
               "write",
               "notify",
               "indicate",
               "authenticated-signed-writes",
               "reliable-write",
               "writable-auxiliaries",
               "encrypt-read",
               "encrypt-write",
               "encrypt-authenticated-read",
               "encrypt-authenticated-write",
               "secure-read",
               "secure-write",
               "authorize"]


def createFlagArray(dbusFlagString):
    flags = ""

    for flag in BLUEZ_FLAGS:
        if flag in dbusFlagString:
            if flags != "":
                flags += ", "
            flags += flag

    return flags


def find_gatt_service_in_objects(services, characteristics, descriptors, bEnableIncludedService):
    bus = dbus.SystemBus()
    objects = get_managed_objects()

    for path, ifaces in objects.items():
        obj = bus.get_object(SERVICE_NAME, path)
        objInterface = dbus.Interface(
            obj, dbus_interface='org.freedesktop.DBus.Properties')

        if ifaces.get(SERVICE_INTERFACE) is not None:
            uuid = str(objInterface.Get(SERVICE_INTERFACE, "UUID"))
            primary = objInterface.Get(SERVICE_INTERFACE, "Primary")
            if bEnableIncludedService == True and ENABLE_TC_AFQP_ADD_INCLUDED_SERVICE == 1:
                if uuid == "8a7f1168-48af-4efb-83b5-e679f9320001":
                    included_service = objInterface.Get(SERVICE_INTERFACE, "Includes")
                    services[uuid]["Includes"] = included_service
                    print("Included service: " + str(included_service))

            services[uuid] = {}
            services[uuid]["obj"] = obj
            services[uuid]["Primary"] = primary
            print("Service: "+uuid)

        if ifaces.get(CHARACTERISTIC_INTERFACE) is not None:
            uuid = str(objInterface.Get(CHARACTERISTIC_INTERFACE, "UUID"))
            flags = createFlagArray(
                objInterface.Get(
                    CHARACTERISTIC_INTERFACE,
                    "Flags"))

            characteristics[uuid] = {}
            characteristics[uuid]["obj"] = obj
            characteristics[uuid]["Flags"] = flags
            print("Char: "+uuid)

        if ifaces.get(DESCRIPTOR_INTERFACE) is not None:
            uuid = str(objInterface.Get(DESCRIPTOR_INTERFACE, "UUID"))
            #flags = str(objInterface.Get(DESCRIPTOR_INTERFACE, "Flags"))

            descriptors[uuid] = {}
            descriptors[uuid]["obj"] = obj
            #descriptors[uuid]["Flags"] = flags
            print("Dscr: "+uuid)

        sys.stdout.flush()

    return services


def convert_dbus_array_to_string(dbusArray):
    string = ''.join([chr(byte) for byte in dbusArray])
    return string
