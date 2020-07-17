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


import subprocess
import os
from abc import ABC, abstractmethod
from datetime import datetime


class test_template(ABC):
    @abstractmethod
    def __init__(self, serial, ip, login, pwd, csv_handler):
        self._func_list = []

        self._serial = serial
        self._ip = ip
        self._login = login
        self._pwd = pwd
        self._cr = csv_handler

    shell_script = ''
    rpi_output_file = ''

    def print_result(self, testcase, result, extra = None):
        if extra == None:
            extra = "%s %sed" % (testcase, result)
        print("%s [INFO] %s 0  %s  :  %s" % (str(datetime.now()), testcase, result.upper(), extra))
        self._cr.writerow({'test name': testcase, 'test result': result})
    def auto_run(self):
        # print('hardware ready? (y/n)')
        # string = str(input())
        string = 'y'

        if string == 'y':
            skip_test = False
        else:
            skip_test = True

        if skip_test == True:
            for f in self._func_list:
                self.print_result(f.__name__, "Skip")
        else:
            print("tests start")
            if self.shell_script:
                subprocess.call([self.shell_script, self._ip, self._login, self._pwd, '-p'])
            for f in self._func_list:
                result = f()
                self.print_result(f.__name__, result)

    def run_shell_script(self, cmd):
        cmd_l = cmd.split()
        subprocess.call(cmd_l)

    def compare_host_dut_result(self, hl, dl):
        """
        Compare host and dut bytes.
        :param hl: a list of host bytes.
        :param dl: a list of dut bytes.
        :return: -1-unmatch 0-mathced
        """
        if len(hl) != len(dl) or len(hl) == 0:
            print("Host and dut have different number of bytes.\nhost:", hl, "\ndut:", dl)
            return -1

        for h, d in zip(hl, dl):
            if h != d:
                print("Host and dut have different bytes.\nhost:", hl, "\ndut:", dl)
                return -1

        return 0

    def clean(self):
        if os.path.isfile(self.rpi_output_file):
            os.remove(self.rpi_output_file)
            self.run_shell_script(" ".join([self.shell_script, self._ip, self._login, self._pwd, '-c']))
