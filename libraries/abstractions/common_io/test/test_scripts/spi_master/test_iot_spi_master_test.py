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
import csv
from time import sleep
import sys
import os
import argparse
import subprocess
import ast
import random
import re

scriptdir = os.path.dirname(os.path.realpath(__file__))
parentdir = os.path.dirname(scriptdir)
if parentdir not in sys.path:
    print("Script Dir: %s" % scriptdir)
    print("Parent Dir: %s" % parentdir)
    sys.path.append(parentdir)
from test_iot_test_template import test_template


class pyboard_spi:
    """
    pyboard spi slave.
    """
    def __init__(self, _port):
        self._serial = serial.Serial(port=_port, timeout=5)
        self._serial.reset_input_buffer()
        self._serial.write('\r\n'.encode('utf-8'))
        self._serial.write('from pyb import SPI\r\n'.encode('utf-8'))

    def recv(self):
        """
        Receive 16 bytes from master.
        :return:
        """
        self._serial.write('spi = SPI(2, SPI.SLAVE, baudrate=500000, polarity=0, phase=0)\r\n'.encode('utf-8'))
        self._serial.write('list(spi.recv(16, timeout=50000))\r\n'.encode('utf-8'))
        sleep(1)

    def send(self, data):
        """
        Send data to master
        :param data: a list of bytes.
        :return:
        """
        self._serial.write('spi = SPI(2, SPI.SLAVE, baudrate=500000, polarity=0, phase=0)\r\n'.encode('utf-8'))
        self._serial.write('data=bytearray({})\r\n'.format(data).encode('utf-8'))
        self._serial.write('spi.send(data, timeout=50000)\r\n'.encode('utf-8'))
        sleep(1)

    def send_recv(self, data):
        """
        Send and revceive data.
        :param data: a list of bytes
        :return:
        """
        self._serial.write('spi = SPI(2, SPI.SLAVE, baudrate=500000, polarity=0, phase=0)\r\n'.encode('utf-8'))
        self._serial.write('data=bytearray({})\r\n'.format(data).encode('utf-8'))
        self._serial.write('list(spi.send_recv(data, timeout=50000))\r\n'.encode('utf-8'))
        sleep(1)

    def deinit(self):
        """
        Deinitialize pyboard and clear input buffer.
        :return:
        """
        self._serial.write('spi.deinit()\r\n'.encode('utf-8'))
        sleep(2)
        self._serial.reset_input_buffer()

    def kill(self):
        """
        Kill the process running on the pyboard.
        :return:
        """
        self._serial.write('\x03\r\n'.encode('utf-8'))
        self._serial.write('\x03\r\n'.encode('utf-8'))
        sleep(2)
        self._serial.reset_input_buffer()


class TestSPIMasterAssisted(test_template):
    """
    Test class for spi master tests.
    """
    shell_script = "%s/test_iot_spi_master_pyb.sh" % scriptdir

    def __init__(self, serial, ip, login, pwd, csv_handler):
        self._func_list = [self.test_IotSPI_WriteSyncAssisted,
                           self.test_IotSPI_ReadSyncAssisted,
                           self.test_IotSPI_TransferSyncAssisted,
                           self.test_IotSPI_WriteAsyncAssisted,
                           self.test_IotSPI_ReadAsyncAssisted,
                           self.test_IotSPI_TransferAsyncAssisted
                           ]

        self._serial = serial
        self._ip = ip
        self._login = login
        self._pwd = pwd
        self._cr = csv_handler
        # Identify pyboard serial port with shell script.
        _pyb_port = subprocess.check_output([self.shell_script, '-t']).decode("utf-8").rstrip()
        self._pyb = pyboard_spi(_pyb_port)


    def write_test(self, cmd):
        """
        Test body for write test.
        :param cmd: iot test cmd
        :return:
        """
        self._pyb.recv()
        self._serial.reset_input_buffer()
        self._serial.write('\r\n'.encode('utf-8'))
        self._serial.write(cmd.encode('utf-8'))
        self._serial.write('\r\n'.encode('utf-8'))

        pyb_res = self._pyb._serial.read_until(terminator=b']').decode("utf-8").splitlines()
        dut_res = self._serial.read_until(terminator=serial.to_bytes([ord(c) for c in 'Ignored '])).decode('utf-8')

        for line in pyb_res:
            try:
                r_bytes = ast.literal_eval(line)
                if isinstance(r_bytes, list):
                    break
            except:
                pass

        self._pyb.deinit()

        w_bytes = []
        for x in re.sub('\r', '',dut_res).split('\n'):
            if x.find('IGNORE') != -1:
                w_bytes = [int(s, 16) for s in x.split(',') if len(s) == 2]
                break

        if 'r_bytes' not in locals() or not isinstance(r_bytes, list):
            print("No data is received by spi slave.\n", pyb_res)
            self._pyb.kill()
            return "Fail"

        if self.compare_host_dut_result(w_bytes, r_bytes) == -1:
            print(repr(pyb_res), '\n', repr(dut_res))
            return "Fail"

        return "Pass"

    def test_IotSPI_WriteSyncAssisted(self):
        return self.write_test("iot_tests test 16 9")

    def test_IotSPI_WriteAsyncAssisted(self):
        return self.write_test("iot_tests test 16 13")

    def read_test(self, cmd):
        """
        Test body for read test.
        :param cmd: iot test cmd
        :return:
        """
        w_bytes = [random.randrange(0, 128) for i in range(0, 16)]
        self._pyb.send(w_bytes)
        self._serial.reset_input_buffer()
        self._serial.write('\r\n'.encode('utf-8'))
        self._serial.write(cmd.encode('utf-8'))
        self._serial.write('\r\n'.encode('utf-8'))

        res = self._serial.read_until(terminator=serial.to_bytes([ord(c) for c in 'Ignored '])).decode('utf-8')
        self._pyb.deinit()

        r_bytes = []
        for x in re.sub('\r', '', res).split('\n'):
            if x.find('IGNORE') != -1:
                r_bytes = [int(s, 16) for s in x.split(',') if len(s) == 2]
                break

        if self.compare_host_dut_result(w_bytes, r_bytes) == -1:
            print(repr(res))
            return "Fail"

        return "Pass"

    def test_IotSPI_ReadSyncAssisted(self):
        return self.read_test("iot_tests test 16 7")

    def test_IotSPI_ReadAsyncAssisted(self):
        return self.read_test("iot_tests test 16 8")

    def transfer_test(self, cmd):
        """
        Test body for read/write test.
        :param cmd: iot test cmd
        :return:
        """
        pyb_wbytes = [random.randrange(0, 128) for i in range(0, 8)]
        self._pyb.send_recv(pyb_wbytes)
        self._serial.reset_input_buffer()
        self._serial.write('\r\n'.encode('utf-8'))
        self._serial.write(cmd.encode('utf-8'))
        self._serial.write('\r\n'.encode('utf-8'))

        dut_res = self._serial.read_until(terminator=serial.to_bytes([ord(c) for c in 'Ignored '])).decode("utf-8")
        pyb_res = self._pyb._serial.read_until(terminator=b']\r\n').decode("utf-8").splitlines()

        self._pyb.deinit()

        for line in pyb_res:
            try:
                pyb_rbytes = ast.literal_eval(line)
                if isinstance(pyb_rbytes, list):
                    break
            except:
                pass

        dut_wbytes = []
        for x in re.sub('\r', '',dut_res).split('\n'):
            if x.find('IGNORE') != -1:
                dut_wbytes = [int(s, 16) for s in x.split(',') if len(s) == 2]
                break

        dut_rbytes = dut_wbytes[len(dut_wbytes) // 2:]
        dut_wbytes = dut_wbytes[:len(dut_wbytes) // 2]

        if 'pyb_rbytes' not in locals() or not isinstance(pyb_rbytes, list):
            print("No data is received by spi slave.\n{}\n{}".format(repr(pyb_res), repr(dut_res)))
            self._pyb.kill()
            return "Fail"

        # Compare bytes dut write and pyb read.
        if self.compare_host_dut_result(dut_wbytes, pyb_rbytes) == -1:
            print("dut_wbytes:{}\npyb_rbytes:{}\n{}\n{}".format(dut_wbytes, pyb_rbytes, pyb_res, dut_res))
            return "Fail"

        # Compare bytes dut read and pyb write.
        if self.compare_host_dut_result(pyb_wbytes, dut_rbytes) == -1:
            print("pyb_wbytes:{}\ndut_rbytes:{}\n{}\n{}".format(pyb_wbytes, dut_rbytes, pyb_res, dut_res))
            return "Fail"

        return "Pass"

    def test_IotSPI_TransferSyncAssisted(self):
        return self.transfer_test("iot_tests test 16 14")

    def test_IotSPI_TransferAsyncAssisted(self):
        return self.transfer_test("iot_tests test 16 15")


# unit test
if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument('-i', '--ip', nargs=1, default=[''], help='ip address of rpi')
    parser.add_argument('-l', '--login_name', nargs=1, default=[''], help='login name of rpi')
    parser.add_argument('-s', '--password', nargs=1, default=[''], help='password of rpi')
    parser.add_argument('-p', '--port', nargs=1, default=[''], help='serial port of connected platform')

    args = parser.parse_args()

    try:
        serial_port = serial.Serial(port=args.port[0], timeout=5, baudrate=115200)
    except Exception as e:
        print(e)
        exit()

    rpi_ip = args.ip[0]
    rpi_login = args.login_name[0]
    rpi_pwd = args.password[0]

    with open(scriptdir+'/test_result.csv', 'w', newline='') as csvfile:
        field_name = ['test name', 'test result']
        writer = csv.DictWriter(csvfile, fieldnames=field_name)
        writer.writeheader()
        t_handler = TestSPIMasterAssisted(serial_port, rpi_ip, rpi_login, rpi_pwd, writer)
        t_handler.auto_run()

    serial_port.close()
