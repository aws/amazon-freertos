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
import threading
import socket
import re

scriptdir = os.path.dirname(os.path.realpath(__file__))
parentdir = os.path.dirname(scriptdir)
if parentdir not in sys.path:
    print("Script Dir: %s" % scriptdir)
    print("Parent Dir: %s" % parentdir)
    sys.path.append(parentdir)
from test_iot_test_template import test_template



class TestI2cMasterAssisted(test_template):
    """
    Test class for i2c master tests.
    """

    def __init__(self, serial, ip, login, pwd, csv_handler):
        self._func_list = [self.test_IotI2CWriteSyncAssisted,
                           self.test_IotI2CWriteAsyncAssisted,
                           self.test_IotI2CReadSyncAssisted,
                           self.test_IotI2CReadAsyncAssisted
                           ]

        self._serial = serial
        self._ip = ip
        self._login = login
        self._pwd = pwd
        self._cr = csv_handler

    shell_script = "%s/test_iot_runonPI_i2c_master.sh" % scriptdir
    port = 50007

    def i2c_write_test(self, cmd):
        """
        Test body of write test.
        :param cmd: iot test cmd
        :return:
        """
        t_shell = threading.Thread(target=self.run_shell_script,
                                   args=(" ".join([self.shell_script, self._ip, self._login, self._pwd, '-s']),))
        t_shell.start()

        socket.setdefaulttimeout(10)
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        time_out = 10
        # Wait until connection with the process on rpi is established.
        while s.connect_ex((self._ip, self.port)) != 0 and time_out > 0:
            time_out -= 1
            sleep(1)
        if time_out == 0:
            print("Socket connection cannot be established")
            s.close()
            return "Fail"

        self._serial.reset_input_buffer()
        self._serial.write('\r\n'.encode('utf-8'))

        self._serial.write(cmd.encode('utf-8'))

        self._serial.write('\r\n'.encode('utf-8'))

        res = self._serial.read_until(terminator=serial.to_bytes([ord(c) for c in 'Ignored '])).decode('utf-8')

        w_bytes = []
        for x in re.sub(r'\r', '', res).split('\n'):
            if x.find('IGNORE') != -1:
                w_bytes = [s for s in x.split(',') if len(s) == 2]
                break

        # Retrieve bytes read by rpi.
        s.sendall(b's')
        try:
            r_bytes = s.recv(1024)
        except:
            print("No data received from rpi.\n", repr(res))
            s.close()
            return 'Fail'
        r_bytes = ["{:02X}".format(b) for b in r_bytes]
        # End process on the rpi.
        s.sendall(b'E')

        t_shell.join()
        s.close()

        # Compare read and write bytes.
        if self.compare_host_dut_result(r_bytes, w_bytes) == -1:
            print(repr(res))
            return "Fail"

        return 'Pass'

    def test_IotI2CWriteSyncAssisted(self):
        return self.i2c_write_test("iot_tests test 11 1")

    def test_IotI2CWriteAsyncAssisted(self):
        return self.i2c_write_test("iot_tests test 11 2")

    def i2c_read_test(self, cmd):
        """
        Test body for read test. The i2c slave callback function in the rpi library is only called after i2c stop. The
        register address cannot be read by rpi before restart so the data to send can only be loaded to rpi fifo after
        stop. As a result, the first read from host is always the data loaded from last request or some random value if
        fifo is never loaded before.
        The solution with the current rpi library is to read rpi twice. Compare the second dut read data with the first
        rpi send data.
        :param cmd: iot test cmd
        :return:
        """
        w_bytes, r_bytes = ([] for i in range(2))

        t_shell = threading.Thread(target=self.run_shell_script,
                                   args=(" ".join([self.shell_script, self._ip, self._login, self._pwd, '-s']),))
        t_shell.start()

        socket.setdefaulttimeout(10)
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        time_out = 10
        # Wait until connection with the process on rpi is established.
        while s.connect_ex((self._ip, self.port)) != 0 and time_out > 0:
            time_out -= 1
            sleep(1)
        if time_out == 0:
            print("Socket connection cannot be established")
            s.close()
            return "Fail"

        for i in range(2):
            self._serial.reset_input_buffer()
            self._serial.write('\r\n'.encode('utf-8'))

            self._serial.write(cmd.encode('utf-8'))

            self._serial.write('\r\n'.encode('utf-8'))

            res = self._serial.read_until(terminator=serial.to_bytes([ord(c) for c in 'Ignored '])).decode('utf-8')

            for x in re.sub(r'\r', '', res).split('\n'):
                if x.find('IGNORE') != -1:
                    r_bytes.append([s for s in x.split(',') if len(s) == 2])
                    break

            # Retrieve bytes sent by rpi
            s.sendall(b's')
            try:
                data = s.recv(1024)
            except:
                print("No data from pi")
                s.close()
                return 'Fail'

            w_bytes.append(["{:02X}".format(b) for b in data])
            # Exit if failed to read bytes from DUT.
            if len(r_bytes) != i + 1:
                print("No data read by DUT.\n", repr(res))
                break
        # End process on the rpi.
        s.sendall(b'E')

        t_shell.join()
        s.close()

        if len(r_bytes) != 2 or len(w_bytes) != 2:
            print("Write and read different number of bytes.\npi:", w_bytes, "\ndut:", r_bytes)
            return 'Fail'

        # Compare read and write bytes.
        if self.compare_host_dut_result(w_bytes[0], r_bytes[1]) == -1:
            print(repr(res))
            return "Fail"

        return 'Pass'

    def test_IotI2CReadSyncAssisted(self):
        return self.i2c_read_test("iot_tests test 11 3")

    def test_IotI2CReadAsyncAssisted(self):
        return self.i2c_read_test("iot_tests test 11 4")


# unit test
if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument('-i', '--ip', nargs=1, default=[''], help='ip address of rpi')
    parser.add_argument('-l', '--login_name', nargs=1, default=[''], help='login name of rpi')
    parser.add_argument('-s', '--password', nargs=1, default=[''], help='password of rpi')
    parser.add_argument('-p', '--port', nargs=1, default=[''], help='serial port of connected platform')

    args = parser.parse_args()

    try:
        serial_port = serial.Serial(port=args.port[0], timeout=5)
    except Exception as e:
        print(e)
        exit()

    rpi_ip = args.ip[0]
    rpi_login = args.login_name[0]
    rpi_pwd = args.password[0]

    with open(scriptdir + 'test_result.csv', 'w', newline='') as csvfile:
        field_name = ['test name', 'test result']
        writer = csv.DictWriter(csvfile, fieldnames=field_name)
        writer.writeheader()
        t_handler = TestI2cMasterAssisted(serial_port, rpi_ip, rpi_login, rpi_pwd, writer)
        t_handler.auto_run()

    serial_port.close()
