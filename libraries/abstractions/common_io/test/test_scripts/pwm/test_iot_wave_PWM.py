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

import pigpio
import time


def start_pwm(pi, gpio=21, frequency=1000, duty_cycle=30):
    """
    Setup gpio and start to generate pwm.
    :param pi: pigpio handler
    :param gpio: gpio
    :param frequency: pwm frequency
    :param duty_cycle: pwm duty cycle
    :return: wave id
    """
    if duty_cycle > 100:
        duty_cycle = 100

    pi.set_mode(gpio, pigpio.OUTPUT)

    period = int(1000000.0 / frequency)
    # Calculate micro seconds of falling edge.
    falling_edge = int(duty_cycle / 100 * period)
    # Setup one cycle of pwm.
    pi.wave_add_generic([
        pigpio.pulse(0, 1 << gpio, 0),
        pigpio.pulse(1 << gpio, 0, falling_edge),
        pigpio.pulse(0, 1 << gpio, period - falling_edge),
    ])
    # Start pwm.
    wid = pi.wave_create()
    pi.wave_send_repeat(wid)

    return wid


if __name__ == "__main__":
    GPIO = 21

    pi = pigpio.pi()

    wid = start_pwm(pi, GPIO)

    time.sleep(10)

    pi.wave_tx_stop()
    pi.wave_delete(wid)

    pi.stop()
