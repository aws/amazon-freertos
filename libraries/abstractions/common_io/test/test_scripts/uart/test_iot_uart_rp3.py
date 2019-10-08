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

import argparse
import serial
import time
import re

# open GPIO pins UART on PI. Make sure that you have enabled the PI in raspi-config.
serialport = serial.Serial(
    port="/dev/ttyAMA0",
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=15,
)


def write_async():
    rcv = serialport.read_until()
    print(rcv)


def write_read_sync():
    rcv = serialport.read_until()
    rcv_str = str(repr(rcv))
    print(rcv_str)
    serialport.write(rcv)


def baud_change_test():
    baud_pattern = re.compile(r"Baudrate:\s(\d+)")
    for i in range(4):
        rcv = serialport.read_until()
        rcv_str = str(repr(rcv))
        baudrate = baud_pattern.search(rcv_str)
        if baudrate:
            serialport.baudrate = baudrate.group(1)
        else:
            serialport.write(rcv)
        print(rcv_str)


if __name__ == "__main__":
    parser = argparse.ArgumentParser("default is input test")

    parser.add_argument(
        "d",
        nargs="+",
        type=int,
        default=0,
        help="select an assisted UART test: 0 for WriteReadSync, 1 for BaudChange, 2 for WriteAsync",
    )

    args = parser.parse_args()
    if args.d[0] == 0:
        write_read_sync()
    elif args.d[0] == 1:
        baud_change_test()
    elif args.d[0] == 2:
        write_async()
