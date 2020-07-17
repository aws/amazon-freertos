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
import argparse
import os, sys
import re

scriptdir = os.path.dirname(os.path.realpath(__file__))
parentdir = os.path.dirname(scriptdir)
if parentdir not in sys.path:
    print("Script Dir: %s" % scriptdir)
    print("Parent Dir: %s" % parentdir)
    sys.path.append(parentdir)
from test_iot_test_template import test_template


class TestAdcAssisted(test_template):
    """
    Test class for adc tests.
    """
    ADC_READING_ERR = 20

    def __init__(self, serial, ip, login, pwd, csv_handler):
        self._func_list = [self.test_IotAdcPrintReadSample]

        self._serial = serial
        self._ip = ip
        self._login = login
        self._pwd = pwd
        self._cr = csv_handler

        # print("Please input platform ADC reference voltage:")
        # self.platform_ref_voltage = float(input())
        self.platform_ref_voltage = 1.8

    shell_script = "%s/test_iot_runonPI_adc.sh" % scriptdir

    def test_IotAdcPrintReadSample(self):
        """
        Test body of adc reading test. It takes average of 5 adc samples and compares with the converted dac input based
        on the reference voltage. Return pass if error is smaller than defined error.
        :return: 'Pass' or 'Fail'
        """
        dac_inputs = ['500', '1000', '1500', '2000']
        dac_ref_voltage = 3.3

        for di in dac_inputs:
            self.run_shell_script(" ".join([self.shell_script, self._ip, self._login, self._pwd, di]))
            adc = []

            # Read five times for each dac input
            for j in range(0, 5):

                self._serial.reset_input_buffer()

                cmd = "iot_tests test 7 1"
                self._serial.write('\r\n'.encode('utf-8'))

                self._serial.write(cmd.encode('utf-8'))

                self._serial.write('\r\n'.encode('utf-8'))

                res = self._serial.read_until(terminator=serial.to_bytes([ord(c) for c in 'Ignored '])).decode('utf-8')

                for x in re.sub('\r', '', res).split('\n'):
                    # Look for the line with ADC reading.
                    if x.find('Expected -1') != -1:
                        adc.append(int(x.split()[-1]))
                        break
                # Wait serial to flush out.
                sleep(0.2)

            # Derive theoretical reading on the device under test based on ref voltage.
            ideal_adc_reading = int(di) / self.platform_ref_voltage * dac_ref_voltage

            # Compare the average reading with the ideal reading
            if abs(ideal_adc_reading - sum(adc) / len(adc)) > self.ADC_READING_ERR:
                print(ideal_adc_reading, sum(adc) / len(adc))
                return 'Fail'

        return 'Pass'


# unit test
if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument('-i', '--ip', nargs=1, help='ip address of rpi')
    parser.add_argument('-l', '--login_name', nargs=1, help='login name of rpi')
    parser.add_argument('-s', '--password', nargs=1, help='password of rpi')
    parser.add_argument('-p', '--port', nargs=1, help='serial port of connected platform')

    args = parser.parse_args()
    print(args.port[0])
    try:
        serial_port = serial.Serial(port=args.port[0], timeout=5, baudrate=115200)
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
        t_handler = TestAdcAssisted(serial_port, rpi_ip, rpi_login, rpi_pwd, writer)
        t_handler.auto_run()

    serial_port.close()
