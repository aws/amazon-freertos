import sys
import subprocess

board = sys.argv[1]

com_port = ''
serial_id = ''
mbedls_rslt = subprocess.run(['mbedls', '--simple'], encoding='utf-8', capture_output=True)
for line in mbedls_rslt.stdout.split('\n'):
    if board in line:
        array = line.split()
        serial_id = array[4]

print(serial_id)