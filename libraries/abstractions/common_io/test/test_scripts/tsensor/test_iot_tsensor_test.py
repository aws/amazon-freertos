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

scriptdir = os.path.dirname(os.path.realpath(__file__))
parentdir = os.path.dirname(scriptdir)
if parentdir not in sys.path:
    print("Script Dir: %s" % scriptdir)
    print("Parent Dir: %s" % parentdir)
    sys.path.append(parentdir)
from test_iot_test_template import test_template


class TestTsensorAssisted(test_template):
    """
    Test class for temperature sensor.
    """

    def __init__(self, serial, ip, login, pwd, csv_handler):
        self._func_list = [self.test_IotTsensorTemp]

        self._serial = serial
        self._ip = ip
        self._login = login
        self._pwd = pwd
        self._cr = csv_handler

    shell_script = "%s/test_iot_runonPI_tsensor.sh" % scriptdir
    rpi_output_file = "%s/tsensor_rpi_res.txt" % scriptdir

    def test_IotTsensorTemp(self):
        """
        Test to verify the temperature reading on the device under test board. Five consecutive reading is performed
        for both external and on board sensors. Average of five readings is compared.
        :return:
        """
        temp_list = []
        ref_temp = 0
        sample_times = 5
        for i in range(0, sample_times):
            self._serial.reset_input_buffer()

            cmd = "iot_tests test 12 1"
            self._serial.write('\r\n'.encode('utf-8'))

            self._serial.write(cmd.encode('utf-8'))

            self._serial.write('\r\n'.encode('utf-8'))

            res = self._serial.read_until(terminator=serial.to_bytes([ord(c) for c in 'Ignored '])).decode('utf-8')
            # print(res)
            temp = []
            # Extract temperature readings from all of on board sensors.
            for x in res.split('\n'):
                # Look for the line with ADC reading.
                if x.find('TEST(TEST_IOT_TSENSOR, AFQP_IotTsensorTemp)') != -1:
                    for s in x.split():
                        if s.isdigit():
                            temp.append(int(s))

            temp_list.append(temp)
            # Wait serial to flush out.
            sleep(0.2)

            # Get reference temperature.
            self.run_shell_script(" ".join([self.shell_script, self._ip, self._login, self._pwd]))
            fp = open(self.rpi_output_file)
            line = fp.readline()
            fp.close()

            try:
                ref_temp = ref_temp + float(line)
            except:
                print("Failed to get ref temp")
                return 'Fail'

        avg_temp = [sum([_list[i] for _list in temp_list]) / len(temp_list) for i in range(len(temp_list[0]))]
        ref_temp = ref_temp / sample_times

        if len(temp) == 0:
            print("Failed to get temp from DUT.")
            return 'Fail'

        for temp in avg_temp:
            if abs(temp / 1000 - ref_temp) > 5:
                print("DUT temp:", temp / 1000, "Ref temp:", ref_temp)
                self.clean()
                return 'Fail'

        # print(avg_temp)
        self.clean()
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
        t_handler = TestTsensorAssisted(serial_port, rpi_ip, rpi_login, rpi_pwd, writer)
        t_handler.auto_run()

    serial_port.close()
