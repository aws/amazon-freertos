#!/usr/bin/env python3
# Amazon FreeRTOS V1.2.3
# Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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



import csv
import argparse
import serial
from gpio.test_iot_gpio_test import gpio_test
from pwm.test_iot_pwm_test import pwm_test
from adc.test_iot_adc_test import adc_test
import os



test_class_list=[(gpio_test, "./gpio"),
                 (pwm_test, "./pwm"),
                 (adc_test, "./adc")]


if __name__ == "__main__":
    parser=argparse.ArgumentParser()

    parser.add_argument('-i', '--ip', nargs=1, help='ip address of rpi')
    parser.add_argument('-l', '--login_name', nargs=1, help='login name of rpi')
    parser.add_argument('-s', '--password', nargs=1, help='password of rpi')
    parser.add_argument('-p', '--port', nargs=1, help='com port of starlite')

    args=parser.parse_args()

    try:
        serial_port = serial.Serial(port=args.port[0], timeout=5)
    except Exception as e:
        print(e)
        exit()

    rpi_ip=args.ip[0]
    rpi_login=args.login_name[0]
    rpi_pwd=args.password[0]

    with open('test_result.csv', 'w', newline='') as csvfile:
        field_name = ['test name', 'test result']
        writer = csv.DictWriter(csvfile, fieldnames=field_name)
        writer.writeheader()
        root_dir=os.getcwd()
        for i in range(0, len(test_class_list)):
            print(test_class_list[i][0].__name__)
            os.chdir(test_class_list[i][1])
            test_obj=test_class_list[i][0](serial_port, rpi_ip, rpi_login, rpi_pwd, writer)
            test_obj.auto_run()
            os.chdir(root_dir)
            print("****************************")

    serial_port.close()
