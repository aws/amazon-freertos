import os
import platform
import serial
import sys
import subprocess

def get_board_info(board):
    com_port = ''
    serial_id = ''
    
    # workaround for BSP-2213
    if (board == 'CY8CKIT_064S0S2_4343W'):
        board= 'CY8CKIT064S0S2_4343W'

    mbedls_rslt = subprocess.run(['mbedls', '--simple'], encoding='utf-8', stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    for line in mbedls_rslt.stdout.split('\n'):
        if board in line:
            array = line.split()
            com_port = array[3]
            serial_id = array[4]
    return com_port, serial_id