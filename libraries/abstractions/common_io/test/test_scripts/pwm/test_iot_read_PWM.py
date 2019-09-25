#!/usr/bin/env python
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

import time
import pigpio

GPIO = 4
sample_counts = 50
sample_duration = 0.2

rising_edge = -1
period = -1
high = -1


def callback_function(gpio, level, tick):
    if gpio == GPIO:
        global rising_edge, high, period
        if level == 1:
            if rising_edge != -1:
                period = pigpio.tickDiff(rising_edge, tick)

            rising_edge = tick

        else:
            if rising_edge != -1:
                high = pigpio.tickDiff(rising_edge, tick)


if __name__ == "__main__":

    pi = pigpio.pi()
    pi.set_mode(GPIO, pigpio.INPUT)

    callback_handler = pi.callback(GPIO, pigpio.EITHER_EDGE, callback_function)

    for i in range(0, sample_counts):
        time.sleep(sample_duration)
        # Calculate frequency with period in micro seconds.
        frequency = 1000000.0 / period if period > 0 else 0
        # Calculate duty cycle.
        duty_cycle = 100.0 * high / period if period > 0 else 0

        print("{:.5f} {:.5f}".format(frequency, duty_cycle))
        # Reset variables for next round.
        rising_edge = period = high = -1

    callback_handler.cancel()
    pi.stop()
