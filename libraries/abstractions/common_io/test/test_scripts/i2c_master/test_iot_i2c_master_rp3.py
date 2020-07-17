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


import time
import pigpio
import random
import socket

SDA = 18
SCL = 19

I2C_ADDR = 9
HOST = ''
PORT = 50007


def i2c(id, tick):
    global pi, wr_bytes, rd_bytes

    s, b, d = pi.bsc_i2c(0x9)
    if b > 1:
        wr_bytes = d
    elif b == 1:
        if d[0] == 0x80:
            rd_bytes = [random.randrange(0, 128) for i in range(0, 16)]
            s, b, d = pi.bsc_i2c(0x9, ''.join('{}'.format(chr(b)) for b in rd_bytes))


if __name__ == "__main__":

    pi = pigpio.pi()

    if not pi.connected:
        exit()

    # Add pull-ups in case external pull-ups haven't been added
    pi.set_pull_up_down(SDA, pigpio.PUD_UP)
    pi.set_pull_up_down(SCL, pigpio.PUD_UP)

    socket.setdefaulttimeout(10)
    # Create socket server.
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((HOST, PORT))
    s.listen(1)

    wr_bytes, rd_bytes = ([] for i in range(2))
    # Respond to BSC slave activity
    e = pi.event_callback(pigpio.EVENT_BSC, i2c)
    # Configure BSC as I2C slave
    st, b, d = pi.bsc_i2c(0x9)

    conn, addr = s.accept()

    # Stay in the process until host request to end.
    time_out = 30
    while time_out > 0:
        try:
            req = conn.recv(1024)
        except:
            print("No request from host.")
            break

        if len(req) > 0 and req[0] == 's':
            if wr_bytes != []:
                conn.sendall(wr_bytes)
            elif rd_bytes != []:
                conn.sendall(bytearray(rd_bytes))
        else:
            break

    e.cancel()

    pi.bsc_i2c(0)  # Disable BSC peripheral
    pi.stop()

    s.close()
