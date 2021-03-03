# Script takes 3 arguments
# argv[1] message that indicates test passed
# argv[2 message that indocate test failed
# argv[3] timeout before test exit with failure

import re
import serial
import sys
import time
import subprocess

def detect_serial_port(board):
    # workaround for BSP-2213
    if (board == 'CY8CKIT_064S0S2_4343W'):
        board='CY8CKIT064S0S2_4343W'
    mbedls_rslt = subprocess.run(['mbedls', '--simple'], encoding='utf-8', capture_output=True)
    print("mbedls output (stdout): %s\n", mbedls_rslt.stdout)
    print("mbedls output (stderr): %s\n", mbedls_rslt.stderr)
    for line in mbedls_rslt.stdout.split('\n'):
        if board in line:
            array = line.split()
            return array[3]

error = 0

# first argument is location of file to append test result
board = sys.argv[1]
global_timeout = int(sys.argv[2])
pass_msg = sys.argv[3]
fail_msg = sys.argv[4]

com_port = detect_serial_port(board)
baud = 115200

t_start = time.time()

with serial.Serial(com_port, baudrate=baud, timeout=10) as port:
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
