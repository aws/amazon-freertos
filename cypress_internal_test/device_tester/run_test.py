import os
import platform
import re
import sys
import subprocess
import shutil

from serial_scan import get_board_info

wifi_ssid = os.getenv('DEVICE_TESTER_WIFI_SSID', default='no_ssid')
wifi_password = os.getenv('DEVICE_TESTER_WIFI_PASSWORD', default='no_password')
ble_pi_ip = os.getenv('DEVICE_TESTER_BLE_PI_IP', default='0.0.0.0')
wifi_encryption_type = os.getenv('DEVICE_TESTER_WIFI_ENCRYPTION_TYPE', default='eWiFiSecurityWPA2')

if wifi_ssid == 'no_ssid' or wifi_password == 'no_password':
    # Tests such as AFRTOS integrity does not requires WIFI
    print('Missing wifi information: the enviromnent variables DEVICE_TESTER_WIFI_SSID, DEVICE_TESTER_WIFI_PASSWORD, DEVICE_TESTER_WIFI_ENCRYPTION_TYPE are not specified.')
    print('Tests that requires WIFI will not be able to run successfully.')

if ble_pi_ip == '0.0.0.0':
    print('Missing BLE Pi IP address: the environment variable DEVICE_TESTER_BLE_PI_IP is not specified.')
    print('BLE tests will not be able to run successfully.')

if platform.system() == 'Linux':
    device_tester_os = 'linux'
elif platform.system() == 'Windows':
    device_tester_os = 'win'
elif platform.system() == 'Darwin':
    device_tester_os = 'mac'
else:
    print('unknown os')
    sys.exit(1)

# The device tester directory cannot be contained in the afr directroy.
# Device tester create a copy of amazon-freertos and put in the device tester directory.
# If device tester is in the amazon-freertos directory, it will cause a recursive copy per test suite.
# Use the user home directory.
user_home_dir = os.path.expanduser('~').replace('\\', '/')
devicer_tester_folder_name = user_home_dir + '/device_tester/devicetester_freertos_' + device_tester_os
config_dir = devicer_tester_folder_name + '/configs'
results_dir = devicer_tester_folder_name + '/results'
test_result_dir = 'test_results'
userdata_schema_dir = devicer_tester_folder_name + '/tests/FRQ_1.1.0/suite'

board = sys.argv[1]
compiler = sys.argv[2]
idt_ver = sys.argv[3]
ota_enb = sys.argv[4]
additional_args = sys.argv[5:]

def generate_replace_dict():
    afr_path = os.path.normpath(os.path.join(os.getcwd(), '../..')).replace('\\', '/')
    com_port, serial_id = get_board_info(board)

    if not com_port or not serial_id:
        print("COM port or Serial id could not be detected. Please make sure you have pyocd and mbedls modules installed and the device is in daplink mode (fast blinky)")
        sys.exit(1)

    return {
        '{cy_home_dir}' : user_home_dir,
        '{cy_wifi_ssid}' : wifi_ssid,
        '{cy_wifi_password}' : wifi_password,
        '{cy_wifi_security_type}' : wifi_encryption_type,
        '{cy_afr_path}' : afr_path,
        '{cy_board_name}' : board,
        '{cy_com_port}' : com_port,
        '{cy_board_id}' : serial_id,
        '{cy_compiler}' : compiler,
        '{cy_ble_pi_ip}' : ble_pi_ip,
        '{cy_ota_en}' : ota_enb}

def setup_device_tester_dir():
    subprocess.run(['rm', '-rf', results_dir])
    subprocess.run(['rm', '-rf', config_dir])
    subprocess.run(['cp', '-rf', 'configs', config_dir])
    if (idt_ver == '3.1.0'):
        subprocess.run(['cp', '-rf', 'userdata_schema.json', userdata_schema_dir])

    if (ota_enb == 'OTA_YES'):
        shutil.move(os.path.join(config_dir, 'device_ota.json'), os.path.join(config_dir, 'device.json'))
        if (not "--group-id" in additional_args):
            additional_args.append("--group-id")
            additional_args.append("OTACore,OTADataplaneMQTT,OTADataplaneHTTP,OTADataplaneBoth")

    if (board == 'CY8CKIT_064S0S2_4343W'):
        shutil.move(os.path.join(config_dir, 'userdata_secure.json'), os.path.join(config_dir, 'userdata.json'))

def replace_cypress_vars():
    replacements = generate_replace_dict()

    os.chdir(config_dir)
    config_files = os.listdir('.')

    for config_file in config_files:
        with open(config_file, 'r', encoding='utf-8') as f:
            filedata = f.read()
            for k, v in replacements.items():
                filedata = filedata.replace(k, v)

        with open(config_file, 'w', encoding='utf-8') as f:
            f.write(filedata)
    os.chdir(sys.path[0])

def setup_test():
    subprocess.run(['bash', 'setup_test.sh', idt_ver])

def run_test():
    os.chdir(devicer_tester_folder_name)
    rslt = subprocess.run(['bin/devicetester_' + device_tester_os + '_x86-64', 'run-suite', '--userdata', 'userdata.json'] + additional_args)
    os.chdir(sys.path[0])
    return rslt.returncode

def check_results():
    subprocess.run(['rm', '-rf', test_result_dir])
    subprocess.run(['cp', '-rf', devicer_tester_folder_name + '/results', test_result_dir])
    # get number of failed and skipped tests to generate a return code
    # device tester always returns 0 even if there are tests failures
    # copy the artifact
    result_dir = os.listdir(test_result_dir)

    error_count = 0
    test_failed_re = re.compile('^Tests Failed:\s+(\d+)$')
    test_skipped_re = re.compile('^Tests Skipped:\s+(\d+)$')
    with open(test_result_dir + '/' + result_dir[0] + '/logs/test_manager.log') as f:
        for line in f:
            search_result = test_failed_re.search(line)
            if search_result:
                error_count = error_count + int(search_result.group(1))
            search_result = test_skipped_re.search(line)
            if search_result:
                error_count = error_count + int(search_result.group(1))
    return error_count

os.chdir(sys.path[0])

setup_test()
setup_device_tester_dir()
replace_cypress_vars()
run_test()
error_count = check_results()

sys.exit(error_count)