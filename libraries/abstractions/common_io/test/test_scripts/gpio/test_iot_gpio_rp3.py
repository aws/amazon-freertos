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



# This script will read the GPIO pin on RP3 and prints the Pin status
import argparse
from gpiozero import LED, Button
from time import sleep
import socket

HOST = ''
PORT = 50007

if __name__ == "__main__":

    parser = argparse.ArgumentParser("default is input test")

    parser.add_argument('-o', nargs=1, type=int, metavar='level',
                        help='set rpi gpio 20 to output mode. level=1: high 0: low')
    parser.add_argument('-i', nargs=1, type=int, metavar='pull_up_down',
                        help='set rpi gpio 21 to input mode. 1: pull up 0: pull down')

    args = parser.parse_args()

    # Check if gpio output or input is requested.
    if args.o:
        GPIO = 20
        socket.setdefaulttimeout(10)
        # Create socket server.
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, PORT))
        s.listen(1)
        led = LED(GPIO)
        # Set gpio high if platform is performing input high test, otherwise set gpio to low.
        if args.o[0] > 0:
            led.on()
        else:
            led.off()
        # Start to accept connection from client. It can notify host process, rpi is ready.
        conn, addr = s.accept()
        # End process until received data from host or time out.
        data = conn.recv(1024)
        s.close()

    elif args.i:
        GPIO = 21
        # If platform gpio is push pull high, set RPi gpio as pull down, otherwise pull up.
        if args.i[0] > 0:
            button = Button(GPIO)
        else:
            button = Button(GPIO, pull_up=False)
        sleep(1)
        print(button.is_pressed)
