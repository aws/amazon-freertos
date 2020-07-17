#!/usr/bin/env python3
# FreeRTOS Common IO V0.1.2
# Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# http://aws.amazon.com/freertos
# http://www.FreeRTOS.org


import serial
import csv
import argparse
import os, sys
import re
import socket
from time import sleep
import threading

scriptdir = os.path.dirname(os.path.realpath(__file__))
parentdir = os.path.dirname(scriptdir)
if parentdir not in sys.path:
    print("Script Dir: %s" % scriptdir)
    print("Parent Dir: %s" % parentdir)
    sys.path.append(parentdir)
from test_iot_test_template import test_template


class TestUartAssisted(test_template):
    """
    Test class for uart tests.
    """

    rpi_output_file = "%s/uart_res.txt" % scriptdir

    def __init__(self, serial, ip, login, pwd, csv_handler):
        self._func_list = [self.test_IotUartWriteReadAsync,
                           self.test_IotUartBaudChange,
                           self.test_IotUartWriteAsync
                           ]

        self._serial = serial
        self._ip = ip
        self._login = login
        self._pwd = pwd
        self._cr = csv_handler

    shell_script = "%s/test_iot_runonPI_uart.sh" % scriptdir
    port = 50007

    def test_IotUartWriteReadAsync(self):
        """
        Test body of uart write then read async test. Return pass if expected message is recieved
        :return: 'Pass' or 'Fail'
        """
        dut_result = None
        pi_result = None
        dut_res_pattern = re.compile(r"AFQP_AssistedIotUARTWriteReadSync.*?PASS")
        pi_regex = re.compile(r"UART\sread-write")

        t_shell = threading.Thread(target=self.run_shell_script,
                                   args=(" ".join([self.shell_script, self._ip, self._login, self._pwd, '1']),))
        t_shell.start()

        # IMPORTANT: without this, the socket won't connect on mac; reason not known.
        sleep(1)

        # Create a tcp type socket with AF_INET address family.
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

        print("connnected successfully")

        self._serial.reset_input_buffer()

        cmd = "iot_tests test 1 1"
        self._serial.write("\r\n".encode("utf-8"))
        self._serial.write(cmd.encode("utf-8"))
        self._serial.write("\r\n".encode("utf-8"))

        dut_res = str(self._serial.read_until(terminator=serial.to_bytes([ord(c) for c in 'Ignored '])))
        dut_result = dut_res_pattern.search(dut_res)
        print(dut_res)
        t_shell.join()

        with open(self.rpi_output_file, "r") as f:
            rpi_output = str(f.read())
            print(rpi_output)
            pi_result = pi_regex.search(rpi_output)

        s.send(b'E')
        s.close()
        self.clean()

        return "Pass" if pi_result and dut_result else "Fail"

    def test_IotUartBaudChange(self):
        """
        Test body of uart baud change test. Return pass if expected messages are recieved
        :return: 'Pass' or 'Fail'
        """
        result_matches = []
        dut_result = None
        dut_res_pattern = re.compile(r"AFQP_AssistedIotUARTBaudChange.*?PASS")
        pi_res_pattern = re.compile(r"UART\sread-write")

        t_shell = threading.Thread(target=self.run_shell_script,
                                   args=(" ".join([self.shell_script, self._ip, self._login, self._pwd, '2']),))
        t_shell.start()

        # IMPORTANT: without this, the socket won't connect on mac; reason not known.
        sleep(1)

        # Create a tcp type socket with AF_INET address family.
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

        print("connnected successfully")

        self._serial.reset_input_buffer()

        cmd = "iot_tests test 1 2"
        self._serial.write("\r\n".encode("utf-8"))
        self._serial.write(cmd.encode("utf-8"))
        self._serial.write("\r\n".encode("utf-8"))

        dut_res = str(self._serial.read_until(terminator=serial.to_bytes([ord(c) for c in 'Ignored '])))
        dut_result = dut_res_pattern.search(dut_res)

        print(dut_res)
        t_shell.join()

        with open(self.rpi_output_file, "r") as f:
            result_matches = pi_res_pattern.findall(str(f.read()))
        s.send(b'E')
        s.close()
        self.clean()

        return "Pass" if len(result_matches) >= 2 and dut_result else "Fail"

    def test_IotUartWriteAsync(self):
        """
        Test body of uart write async test. Return pass if expected message is received
        :return: 'Pass' or 'Fail'
        """
        pi_result = None
        dut_result = None
        dut_res_pattern = re.compile(r"AFQP_AssistedIotUARTWriteAsync.*?PASS")
        # 199 corresponds to the config testIotUART_BUFFER_LENGTH_LARGE - 1 on the DUT
        pi_res_pattern = re.compile(r"(?:\\xaa){199}")

        t_shell = threading.Thread(target=self.run_shell_script,
                                   args=(" ".join([self.shell_script, self._ip, self._login, self._pwd, '3']),))
        t_shell.start()

        # IMPORTANT: without this, the socket won't connect on mac; reason not known.
        sleep(1)

        # Create a tcp type socket with AF_INET address family.
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

        print("connnected successfully")

        self._serial.reset_input_buffer()

        cmd = "iot_tests test 1 3"
        self._serial.write("\r\n".encode("utf-8"))
        self._serial.write(cmd.encode("utf-8"))
        self._serial.write("\r\n".encode("utf-8"))

        dut_res = str(self._serial.read_until(terminator=serial.to_bytes([ord(c) for c in 'Ignored '])))
        dut_result = dut_res_pattern.search(dut_res)
        print(dut_res)
        t_shell.join()

        with open(self.rpi_output_file, "r") as f:
            rpi_output = str(f.read())
            print(rpi_output)
            pi_result = pi_res_pattern.search(rpi_output)

        s.send(b'E')
        s.close()
        self.clean()

        return "Pass" if pi_result and dut_result else "Fail"


# unit test
if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument('-i', '--ip', nargs=1, help='ip address of rpi')
    parser.add_argument('-l', '--login_name', nargs=1, help='login name of rpi')
    parser.add_argument('-s', '--password', nargs=1, help='password of rpi')
    parser.add_argument('-p', '--port', nargs=1, help='serial port of connected platform')

    args = parser.parse_args()

    try:
        serial_port = serial.Serial(port=args.port[0], baudrate=115200, timeout=20)
    except Exception as e:
        print(e)
        exit()

    rpi_ip = args.ip[0]
    rpi_login = args.login_name[0]
    rpi_pwd = args.password[0]

    with open("test_result.csv", "w", newline="") as csvfile:
        field_name = ["test name", "test result"]
        writer = csv.DictWriter(csvfile, fieldnames=field_name)
        writer.writeheader()
        t_handler = TestUartAssisted(serial_port, rpi_ip, rpi_login, rpi_pwd, writer)
        t_handler.auto_run()

    serial_port.close()
