import os
import platform
import serial
import sys
import subprocess

def get_board_info(board):
    com_port = ''
    serial_id = ''
    if (board == 'CY8CKIT_064S0S2_4343W'):
        if platform.system() == 'Linux':
            com_port = '/dev/ttyACM0'
        elif platform.system() == 'Windows':
            com_port = 'COM3'
        serial_id = '190A1301f3180a1502f3180a00000000000000002e127069'
    else:
        mbedls_rslt = subprocess.run(['mbedls', '--simple'], encoding='utf-8', stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        for line in mbedls_rslt.stdout.split('\n'):
            if board in line:
                array = line.split()
                com_port = array[3]
                serial_id = array[4]
    return com_port, serial_id