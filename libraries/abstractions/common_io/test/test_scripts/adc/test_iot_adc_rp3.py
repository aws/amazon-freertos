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


import smbus
import argparse

MAX_OUT = int((1.8 / 3.3) * 0xFFF)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('d', nargs='?', type=int, default=0, help='dac input')
    args = parser.parse_args()

    data = args.d & 0xFFF

    if data > MAX_OUT:
        data = MAX_OUT

    # I2C channel 1 is connected to the GPIO pins
    channel = 1

    # MCP4725 on adafruit board defaults to address 0x62
    address = 0x62

    # Register addresses (with "normal mode" power-down bits)
    reg_write_dac = 0x40

    bus = smbus.SMBus(channel)

    # 12 bits data is aligned into two bytes with bit 11 as msb i.e. bit 11-4 in the byte 3,
    # bit 3-0 in the byte 4 up four bits.
    lo4 = (data & 0xF) << 4
    hi8 = data >> 4

    bus.write_i2c_block_data(address, reg_write_dac, [hi8, lo4])
