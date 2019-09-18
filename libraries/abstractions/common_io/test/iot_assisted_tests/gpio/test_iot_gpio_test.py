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



import serial
from time import sleep
import csv
import threading
import os, sys
import argparse
parentdir = os.path.dirname(os.getcwd())
sys.path.insert(0,parentdir)
from test_iot_test_template import test_template


class gpio_test(test_template):
    """
    Test class for gpio tests.
    """
    outputfile = './gpio_output_RP3.txt'

    def __init__(self, serial, ip, login, pwd, csv_handler):
        self._func_list = [self.test_AssistedIotGpioModeWritePushPullTrue,
                           self.test_AssistedIotGpioModeWritePushPullFalse,
                           self.test_AssistedIotGpioModeWriteOpenDrainFalse,
                           self.test_AssistedIotGpioModeWriteOpenDrainTrue,
                           self.test_AssistedIotGpioModeReadTrue,
                           self.test_AssistedIotGpioModeReadFalse
                           ]

        self._serial = serial
        self._ip = ip
        self._login = login
        self._pwd = pwd
        self._cr = csv_handler

    shell_script = './test_iot_runonPI_gpio.sh'

    def test_AssistedIotGpioModeWritePushPullTrue(self):
        """
        GPIO is set as push pull and output high.
        :return: 'Pass' or 'Fail'
        """
        # RPi is supposed to have a valid button request.
        rpi_button_read = 'True'
        cmd = "iot_tests test 5 1"

        self._serial.write('\r\n'.encode('utf-8'))

        self._serial.write(cmd.encode('utf-8'))

        self._serial.write('\r\n'.encode('utf-8'))
        # Set RPi gpio to pull down. So the test will pass only if platform side is high.
        self.run_shell_script(" ".join([self.shell_script, self._ip, self._login, self._pwd, '-w', '0']))

        fp = open(self.outputfile)
        line = fp.readline()
        line = line.strip()

        fp.close()
        self.clean()

        return 'Pass' if line == rpi_button_read else 'Fail'

    def test_AssistedIotGpioModeWritePushPullFalse(self):
        """
        GPIO is set as push pull and output low.
        :return: 'Pass' or 'Fail'
        """
        # RPi is supposed to have a valid button request.
        rpi_button_read = 'True'
        cmd = "iot_tests test 5 2"

        self._serial.write('\r\n'.encode('utf-8'))

        self._serial.write(cmd.encode('utf-8'))

        self._serial.write('\r\n'.encode('utf-8'))
        # Set RPi gpio to pull up. So the test will pass only if platform side is low.
        self.run_shell_script(" ".join([self.shell_script, self._ip, self._login, self._pwd, '-w', '1']))

        fp = open(self.outputfile)
        line = fp.readline()
        line = line.strip()

        fp.close()
        self.clean()

        return 'Pass' if line == rpi_button_read else 'Fail'

    def test_AssistedIotGpioModeWriteOpenDrainTrue(self):
        """
        GPIO is set as open drain high.
        :return: 'Pass' or 'Fail'
        """
        # RPi is supposed to have an invalid button request.
        # Since the pull up resistor is required for open drain output, Other than output low, the RPi gpio is always
        # high, which will not trigger a button request.
        rpi_button_read = 'False'
        cmd = "iot_tests test 5 3"

        self._serial.write('\r\n'.encode('utf-8'))

        self._serial.write(cmd.encode('utf-8'))

        self._serial.write('\r\n'.encode('utf-8'))
        # Set RPi gpio to pull up as open drain pull up resistor.
        self.run_shell_script(" ".join([self.shell_script, self._ip, self._login, self._pwd, '-w', '1']))

        fp = open(self.outputfile)
        line = fp.readline()
        line = line.strip()

        fp.close()
        self.clean()

        return 'Pass' if line == rpi_button_read else 'Fail'

    def test_AssistedIotGpioModeWriteOpenDrainFalse(self):
        """
        GPIO is set as open drain low.
        :return: 'Pass' or 'Fail'
        """
        # RPi is supposed to have a valid button request.
        rpi_button_read = 'True'
        cmd = "iot_tests test 5 4"

        self._serial.write('\r\n'.encode('utf-8'))

        self._serial.write(cmd.encode('utf-8'))

        self._serial.write('\r\n'.encode('utf-8'))
        # Set RPi gpio to pull up as open drain pull up resistor.
        self.run_shell_script(" ".join([self.shell_script, self._ip, self._login, self._pwd, '-w', '1']))

        fp = open(self.outputfile)
        line = fp.readline()
        line = line.strip()

        fp.close()
        self.clean()

        return 'Pass' if line == rpi_button_read else 'Fail'

    def test_AssistedIotGpioModeReadTrue(self):
        """
        GPIO is set as input and expecting to read high.
        :return: 'Pass' or 'Fail'
        """
        t_shell = threading.Thread(target=self.run_shell_script,
                                   args=(" ".join([self.shell_script, self._ip, self._login, self._pwd, '-r 1']),))
        t_shell.start()
        sleep(1)
        self._serial.reset_input_buffer()

        cmd = "iot_tests test 5 5"
        self._serial.write('\r\n'.encode('utf-8'))

        self._serial.write(cmd.encode('utf-8'))

        self._serial.write('\r\n'.encode('utf-8'))

        # wait for the script on rpi to finish. The gpio pin on the rpi needs to be properly closed before moving on.
        t_shell.join()
        res = str(self._serial.read(100))

        if res.find('PASS\\n') != -1:
            return 'Pass'
        else:
            return 'Fail'

    def test_AssistedIotGpioModeReadFalse(self):
        """
        GPIO is set as input and expecting to read low.
        :return: 'Pass' or 'Fail'
        """
        t_shell = threading.Thread(target=self.run_shell_script,
                                   args=(" ".join([self.shell_script, self._ip, self._login, self._pwd, '-r 0']),))
        t_shell.start()
        sleep(1)
        self._serial.reset_input_buffer()

        cmd = "iot_tests test 5 6"
        self._serial.write('\r\n'.encode('utf-8'))

        self._serial.write(cmd.encode('utf-8'))

        self._serial.write('\r\n'.encode('utf-8'))

        # wait the script on rpi to finish. The gpio pin on the rpi needs to be properly closed before moving on.
        t_shell.join()
        res = str(self._serial.read(100))

        if res.find('PASS\\n') != -1:
            return 'Pass'
        else:
            return 'Fail'

    def clean(self):
        os.remove("./gpio_output_RP3.txt")
        self.run_shell_script(" ".join([self.shell_script, self._ip, self._login, self._pwd, '-c']))


# unit test
if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument('-i', '--ip', nargs=1, help='ip address of rpi')
    parser.add_argument('-l', '--login_name', nargs=1, help='login name of rpi')
    parser.add_argument('-s', '--password', nargs=1, help='password of rpi')
    parser.add_argument('-p', '--port', nargs=1, help='serial port of connected platform')

    args = parser.parse_args()

    try:
        serial_port = serial.Serial(port=args.port[0], timeout=5)
    except Exception as e:
        print(e)
        exit()

    rpi_ip = args.ip[0]
    rpi_login = args.login_name[0]
    rpi_pwd = args.password[0]

    with open('test_result.csv', 'w', newline='') as csvfile:
        field_name = ['test name', 'test result']
        writer = csv.DictWriter(csvfile, fieldnames=field_name)
        writer.writeheader()
        t_handler = gpio_test(serial_port, rpi_ip, rpi_login, rpi_pwd, writer)
        t_handler.auto_run()

    serial_port.close()
