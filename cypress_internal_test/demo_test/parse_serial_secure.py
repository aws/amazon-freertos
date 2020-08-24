# Script takes 4 arguments
# argv[1] serial port for board
# argv[2] timeout before test exit with failure
# argv[3] message that indicates test passed
# argv[4] message that indocate test failed

import re
import serial
import sys
import time
import subprocess


error = 0

com_port = sys.argv[1]
global_timeout = int(sys.argv[2])
pass_msg = sys.argv[3]
fail_msg = sys.argv[4]

t_start = time.time()

with serial.Serial(com_port, baudrate=115200, timeout=10) as port:
    port.reset_input_buffer()
    line = ""

    while True:
        try:
            sys.stdout.flush()
            bytes = port.readline()
            line = str(bytes, 'utf-8').strip()

            if line != '':
                print(line)
                
                if pass_msg in line:
                    sys.exit(0)
                elif fail_msg in line:
                    sys.exit(1)

            t_current = time.time()
            if t_start + global_timeout < t_current:
                print("script timed out after %s seconds while reading from serial" % global_timeout)
                sys.exit(2)

        except UnicodeDecodeError:
            print("Invalid UTF-8 characters %s" % bytes)

sys.exit(error)