#!/usr/bin/env python

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
import socket
import threading

GPIO_R = 4
GPIO_W = 23

sample_counts = 50
sample_duration = 0.2

rising_edge = -1
period = -1
high = -1

HOST = ''
PORT = 50007

process_end = False


def callback_function(gpio, level, tick):
    """
    Interrrupt call back function triggered by pwm input pulses.
    :param gpio: gpio pin num
    :param level: gpio level
    :param tick: The number of microseconds since boot
    :return:
    """
    if gpio == GPIO_R:
        global rising_edge, high, period
        if level == 1:
            if rising_edge != -1:
                period = pigpio.tickDiff(rising_edge, tick)

            rising_edge = tick

        else:
            if rising_edge != -1:
                high = pigpio.tickDiff(rising_edge, tick)


def socket_thread(s):
    """
    Thread function to monitor socket.
    :param s: Socket handler.
    :return:
    """
    global process_end
    try:
        conn, addr = s.accept()
        conn.recv(1024)
    except Exception as e:
        print(e)
    # Notify the main thread to end process.
    process_end = True


def start_pwm(pi, gpio=23, frequency=1000, duty_cycle=30):
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
    # Calculate period in micro seconds.
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

    pi = pigpio.pi()
    pi.set_mode(GPIO_R, pigpio.INPUT)

    callback_handler = pi.callback(GPIO_R, pigpio.EITHER_EDGE, callback_function)

    socket.setdefaulttimeout(10)
    # Create socket server.
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((HOST, PORT))
    s.listen(1)
    # Start to generate pwm output.
    wid = start_pwm(pi, GPIO_W)

    # Create a thread to monitor socket.
    t_s = threading.Thread(target=socket_thread, args=(s,))
    t_s.start()

    time_out = 100
    # Polling pwm input.
    while not process_end and time_out > 0:
        time.sleep(sample_duration)
        # Calculate frequency with period in micro seconds.
        frequency = 1000000.0 / period if period > 0 else 0
        # Calculate duty cycle.
        duty_cycle = 100.0 * high / period if period > 0 else 0

        print("{:.5f} {:.5f}".format(frequency, duty_cycle))
        # Reset variables for next round.
        rising_edge = period = high = -1
        time_out -= 1

    pi.wave_tx_stop()
    pi.wave_delete(wid)

    callback_handler.cancel()
    pi.stop()
    s.close()
