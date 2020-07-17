#!/usr/bin/env python3
# FreeRTOS Common IO V0.1.2
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

import serial
from time import sleep
import csv
import os, sys
import argparse
import usb.core
import usb.util
import re
import threading
import random

scriptdir = os.path.dirname(os.path.realpath(__file__))
parentdir = os.path.dirname(scriptdir)
if parentdir not in sys.path:
    print("Script Dir: %s" % scriptdir)
    print("Parent Dir: %s" % parentdir)
    sys.path.append(parentdir)
from test_iot_test_template import test_template


class TestUSBDeviceAssisted(test_template):
    """
    Test class for usb device tests.
    """

    def __init__(self, serial, ip, login, pwd, csv_handler):
        self._func_list = [self.test_IotUsbDeviceHidAttachAssisted,
                           self.test_IotUsbDeviceWriteSyncAssisted,
                           self.test_IotUsbDeviceWriteAsyncAssisted,
                           self.test_IotUsbDeviceReadAsyncAssisted,
                           self.test_IotUsbDeviceReadSyncAssisted,
                           self.test_IotUsbDeviceHidDetachAssisted
                           ]

        self._serial = serial
        self._ip = ip
        self._login = login
        self._pwd = pwd
        self._cr = csv_handler
        self.vid = self.pid = None
        self.reattach = None
        self.host_bytes = []
        self.dev = []
        self.epo = self.epi = None

    def test_IotUsbDeviceHidAttachAssisted(self):
        """
        Test body for attach test.
        :return:
        """
        self._serial.reset_input_buffer()
        self._serial.write('\r\n'.encode('utf-8'))
        self._serial.write("iot_tests test 17 11".encode('utf-8'))
        self._serial.write('\r\n'.encode('utf-8'))
        res = self._serial.read_until(terminator=serial.to_bytes([ord(c) for c in 'Ignored '])).decode('utf-8')

        # Grab vendor id and product id.
        self.vid, self.pid = map(lambda x: int(x, 16), re.findall(r'[vid|pid]:(\w+)', res))

        sleep(5)
        # find usb device
        self.dev = usb.core.find(idVendor=self.vid, idProduct=self.pid)

        if self.dev is None:
            raise ValueError('Device not found')

        print(self.dev)

        if self.dev.is_kernel_driver_active(0):
            self.reattach = True
            self.dev.detach_kernel_driver(0)

        # set the active configuration. With no arguments, the first
        # configuration will be the active one
        self.dev.set_configuration()

        # get an endpoint instance
        cfg = self.dev.get_active_configuration()
        intf = usb.util.find_descriptor(cfg, bInterfaceClass=3)

        self.epo = usb.util.find_descriptor(
            intf,
            # match the first OUT endpoint
            custom_match= \
                lambda e: \
                    usb.util.endpoint_direction(e.bEndpointAddress) == \
                    usb.util.ENDPOINT_OUT)
        self.epi = usb.util.find_descriptor(
            intf,
            # match the first IN endpoint
            custom_match= \
                lambda e: \
                    usb.util.endpoint_direction(e.bEndpointAddress) == \
                    usb.util.ENDPOINT_IN)

        assert self.epo and self.epi is not None

        return 'Pass'

    def usb_device_interrupt_type(self, cmd, host_func):
        """Test body for interrupt type tests."""
        t_host = threading.Thread(target=host_func)
        t_host.start()
        sleep(3)
        self._serial.reset_input_buffer()
        self._serial.write('\r\n'.encode('utf-8'))
        self._serial.write(cmd.encode('utf-8'))
        self._serial.write('\r\n'.encode('utf-8'))
        sleep(1)
        res = self._serial.read_until(terminator=serial.to_bytes([ord(c) for c in 'Ignored '])).decode('utf-8')
        print(repr(res))
        t_host.join()

        assert res.find('Ignored ') is not -1

        dut_bytes = []
        for x in re.sub('\r', '', res).split('\n'):
            if x.find('IGNORE') != -1:
                dut_bytes = [int(s, 16) for s in x.split(',') if len(s) == 2]
                break

        if self.compare_host_dut_result(self.host_bytes, dut_bytes) == -1:
            print(repr(res))
            return "Fail"

        return "Pass"

    def host_read_thread(self):
        """
        Host read thread.
        :return:
        """
        self.host_bytes = []
        self.host_bytes = list(self.epi.read(16, timeout=10000))
        # print(self.r_bytes)

    def test_IotUsbDeviceWriteSyncAssisted(self):
        return self.usb_device_interrupt_type("iot_tests test 17 14", self.host_read_thread)

    def test_IotUsbDeviceWriteAsyncAssisted(self):
        return self.usb_device_interrupt_type("iot_tests test 17 12", self.host_read_thread)

    def host_write_thread(self):
        """
        Host write thread.
        :return:
        """
        self.host_bytes = [random.randrange(0, 128) for i in range(0, 16)]
        self.epo.write(self.host_bytes, 10000)
        # print(self.w_bytes)

    def test_IotUsbDeviceReadAsyncAssisted(self):
        return self.usb_device_interrupt_type("iot_tests test 17 13", self.host_write_thread)

    def test_IotUsbDeviceReadSyncAssisted(self):
        return self.usb_device_interrupt_type("iot_tests test 17 15", self.host_write_thread)

    def test_IotUsbDeviceHidDetachAssisted(self):
        """
        Test body for detach test.
        :return:
        """
        # This is needed to release interface, otherwise attach_kernel_driver fails due to "Resource busy"
        usb.util.dispose_resources(self.dev)
        # It may raise USBError if there's e.g. no kernel driver loaded at all
        if self.reattach:
            self.dev.attach_kernel_driver(0)

        self._serial.reset_input_buffer()
        self._serial.write('\r\n'.encode('utf-8'))
        self._serial.write("iot_tests test 17 2".encode('utf-8'))
        self._serial.write('\r\n'.encode('utf-8'))
        res = self._serial.read_until(terminator=serial.to_bytes([ord(c) for c in 'Ignored '])).decode('utf-8')
        print(repr(res))

        assert res.find('Ignored ') is not -1
        sleep(5)
        # Make sure device is detached.
        self.dev = usb.core.find(idVendor=self.vid, idProduct=self.pid)
        # print(self.dev)

        if self.dev is None:
            return "Pass"

        return "Fail"


# unit test
if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument('-i', '--ip', nargs=1, default=[''], help='ip address of rpi')
    parser.add_argument('-l', '--login_name', nargs=1, default=[''], help='login name of rpi')
    parser.add_argument('-s', '--password', nargs=1, default=[''], help='password of rpi')
    parser.add_argument('-p', '--port', nargs=1, default=[''], help='serial port of connected platform')

    args = parser.parse_args()

    try:
        serial_port = serial.Serial(port=args.port[0], baudrate=115200, timeout=5)
    except Exception as e:
        print(e)
        exit()

    rpi_ip = args.ip[0]
    rpi_login = args.login_name[0]
    rpi_pwd = args.password[0]

    with open(scriptdir + '/test_result.csv', 'w', newline='') as csvfile:
        field_name = ['test name', 'test result']
        writer = csv.DictWriter(csvfile, fieldnames=field_name)
        writer.writeheader()
        t_handler = TestUSBDeviceAssisted(serial_port, rpi_ip, rpi_login, rpi_pwd, writer)
        t_handler.auto_run()

    serial_port.close()
