import os
import platform
import serial
import sys
import subprocess

from serial_scan import get_board_info

board = sys.argv[1]
baud = 115200
com_port, ignore = get_board_info(board)

with serial.Serial(com_port, baudrate=baud, timeout=10) as port:
    port.reset_input_buffer()
