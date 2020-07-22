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

import argparse
import serial
import time
import re
import socket

HOST = ''
PORT = 50007

# open GPIO pins UART on PI. Make sure that you have enabled the PI in raspi-config.
serialport = serial.Serial(
    port="/dev/serial0",
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=10,
)


def write_async(s):
    print("write_async")
    # Notify host rpi is ready.
    conn, addr = s.accept()
    # Read from UART until '\n' line ending is found
    rcv = serialport.read_until()
    print(str(rcv))


def write_read_sync(s):
    print("write_read_sync")
    # Notify host rpi is ready.
    conn, addr = s.accept()
    # Read from UART until '\n' line ending is found
    rcv = serialport.read_until()
    rcv_str = str(repr(rcv))
    print(rcv_str)
    # Echo message back to UART
    serialport.write(rcv)


def baud_change_test(s):
    print("baud_change_test")
    # Pattern to find baudrate to switch to. The new baudrate for tests are signaled by DUT
    baud_pattern = re.compile(r"Baudrate:\s(\d+)")

    # Notify host rpi is ready.
    conn, addr = s.accept()
    for i in range(4):
        # Read from UART until '\n' line ending is found
        rcv = serialport.read_until()
        rcv_str = str(repr(rcv))
        baudrate = baud_pattern.search(rcv_str)
        # Last message from UART was to switch to a different baudrate
        if baudrate:
            # switch RPI UART baudrate to baudrate indicated by DUT
            serialport.baudrate = baudrate.group(1)
        else:
            # If message was not signaling a new baudrate, the DUT sent a message with the intention of receiving an echo back
            serialport.write(rcv)
        print(rcv_str)


if __name__ == "__main__":

    parser = argparse.ArgumentParser("default is input test")

    parser.add_argument(
        "d",
        nargs="+",
        type=int,
        default=0,
        help="select an assisted UART test: 1 for WriteReadSync, 2 for BaudChange, 3 for WriteAsync",
    )

    args = parser.parse_args()
    socket.setdefaulttimeout(5)

    # Use socket to signal RPI is ready
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((HOST, PORT))
    s.listen(1)

    if args.d[0] == 1:
        write_read_sync(s)
    elif args.d[0] == 2:
        baud_change_test(s)
    elif args.d[0] == 3:
        write_async(s)

    s.close()
