#!/usr/bin/python3

"""
Copyright (c) 2020 Cypress Semiconductor Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""

import os
import platform
import sys, argparse
import subprocess
import time
import getpass
import json

from cysecuretools import CySecureTools
from cysecuretools.execute.sys_call import get_prov_details
from cysecuretools.execute.programmer.programmer import ProgrammingTool
from cysecuretools.execute.provisioning_lib.cyprov_pem import PemKey
from OpenSSL import crypto, SSL


def myargs(argv):
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument('-h', '--help',
                        dest='show_help',
                        action='help',
                        help='Print this help message and exit')

    parser.add_argument('-p', '--policy',
                        dest='policy_file',
                        action='store',
                        type=str,
                        help="Device policy file",
                        required=False)

    parser.add_argument('-d', '--device',
                        dest='device',
                        action='store',
                        type=str,
                        help="Device manufacturing part number",
                        required=False)

    parser.add_argument('-o', '--oocd',
                        dest='oocd_path',
                        action='store',
                        type=str,
                        help="Path to OpenOCD tool",
                        required=False)

    options = parser.parse_args(argv)
    return options


def create_app_keys():
    cytools.create_keys()


def read_device_pub_key():
    # Read Device Key and save
    print('Reading public key from device')
    key=cytools.read_public_key(1, "jwk")
    print("key: {}".format(key))
    if not key:
        print('Error: Cannot read device public key.')
        return 1

    pub_key_json = 'keys/device_pub_key.json'

    with open(pub_key_json, 'w') as json_file:
        json.dump(key, json_file)

    # Change from JWK to PEM
    pub_key_pem = 'keys/device_pub_key.pem'
    if os.path.exists(pub_key_json) and os.stat(pub_key_json).st_size > 0:
        pem = PemKey(pub_key_json)
        pem.save(pub_key_pem, private_key=False)
    else:
        print('Failed to read device public key')
        return 1
    print('Device public key has been read successfully.')
    return 0


def generate_device_cert(
    dev_pub_key_path="keys/device_pub_key.pem",
    ca_priv_key_path="certificates/rootCA.key",
    ca_cert_path="certificates/rootCA.pem"):

    if True:
        # read device public key from previously read from the device
        dev_pub_key = crypto.load_publickey(crypto.FILETYPE_PEM,
                                            open(dev_pub_key_path, 'r').read())
    else:
        # for development only, use public key from self generated private key
        dev_priv_key = crypto.load_privatekey(crypto.FILETYPE_ASN1,
                                              open("keys/device_priv_key.der",
                                                   'rb').read())
        dev_pub_key = crypto.load_publickey(crypto.FILETYPE_PEM,
                          crypto.dump_publickey(crypto.FILETYPE_PEM,
                                                dev_priv_key))
    ca_privatekey = crypto.load_privatekey(crypto.FILETYPE_PEM,
                                           open(ca_priv_key_path, 'r').read())
    ca_cert = crypto.load_certificate(crypto.FILETYPE_PEM,
                                      open(ca_cert_path, 'r').read())

    dev_serial_num = \
        input("\r\nSelect unique device serial number for {} (digits only):\n"
                            .format(cytools.target_name.upper()))
    if not dev_serial_num.isnumeric():
        print('Error: device serial number not number')
        return 1

    # create cert signed by ca_cert
    cert = crypto.X509()
    cert.set_subject(ca_cert.get_subject())
    cert.get_subject().CN = cytools.target_name.upper() + '-' + str(dev_serial_num)
    cert.set_serial_number(int(dev_serial_num))
    cert.gmtime_adj_notBefore(0)
    cert.gmtime_adj_notAfter(10*365*24*60*60)
    cert.set_issuer(ca_cert.get_subject())
    cert.set_pubkey(dev_pub_key)
    cert.sign(ca_privatekey, 'sha256')

    open("certificates/device_cert.pem", "wb")\
        .write(crypto.dump_certificate(crypto.FILETYPE_PEM, cert))
    open("certificates/device_cert.der", "wb")\
        .write(crypto.dump_certificate(crypto.FILETYPE_ASN1, cert))
    print('Device certificate generated successfully.')
    return 0


def create_provisioning_packet():
    cytools.create_provisioning_packet()


def re_provision_device(device, policy):
    answer = input('Reprovision the device. Are you sure? (y/N): ')
    if answer == 'y' or answer == 'Y':
        cytools.re_provision_device()
    else:
        print('Reprovision skipped.')


def oocd_command(cmd):
    if not isinstance(cmd, list) or not cmd:
        raise Exception("Command must be in a non-empty list")

    output = []
    print("Executing command: {}".format(' '.join(cmd)))
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                              stderr=subprocess.STDOUT)
    for line in iter(p.stdout.readline, b''):
        output.append(line.decode('utf-8'))
        print("{}".format(line.decode('utf-8')), end='')
    p.stdout.close()
    ret = p.wait()
    print("Command completed (ret={})".format(ret))
    return ret, ''.join(output)


def erase_user_images(oocd_path):
    #TBD: replace oocd it with pyocd
    if not oocd_path:
        oocd_path = '/tools/openocd'
        print("Path to OOCD was not provided, trying {}".format(oocd_path))

    if platform.system() == "Windows":
        openocd_bin = 'bin/openocd.exe'
    else:
        openocd_bin = 'bin/openocd'

    openocd = os.path.join(oocd_path,
                              openocd_bin)
    if os.path.isfile(openocd):
        print("OOCD location: {}".format(openocd))
    else:
        print("Failed to find OOCD at {}".format(openocd))
        return 1


    max_tries = 3
    for retries in range(0, max_tries):
        ret, output = oocd_command([
            openocd,
            '-s', os.path.join(oocd_path, 'scripts'),
            '-f', 'interface/kitprog3.cfg',
            '-f', 'target/psoc6_2m_secure.cfg',
            '-c', 'init; reset init',
            '-c', 'flash erase_address 0x10000000 0x1d0000',
            '-c', 'resume; reset; exit' ])
        if ret == 0:
            # success
            return (ret, output)
        else:
            print("OOCD communication error, retrying")

    print("OOCD communication error, giving up after {} retries"
           .format(max_tries))
    return (ret, output)


def main(argv):
    options = myargs(argv)
    print("options: {}\r\n".format(options))

    if not options.policy_file:
        options.policy_file = 'policy_multi_img_CM0p_CM4_debug_2M.json'
        options.policy_file = os.path.join('policy',
                                           options.policy_file)
        answer = \
            input('Policy file name was not provided, use default {}? (Y/n): '
                   .format(options.policy_file))
        if answer == 'N' or answer == 'n':
            exit(1)
    if not os.path.isfile(options.policy_file):
        print("Policy file {} doesn't exit.".format(options.policy_file))
        exit(1)

    if not options.device:
        options.device = "CY8CKIT-064S0S2-4343W"
        answer = input("\r\nDevice is not provided, use default {}? (Y/n): "
                        .format(options.device))
        if answer == 'N' or answer == 'n':
            exit(1)

#   Create cysecuretools object
    global cytools
    cytools = CySecureTools(options.device, options.policy_file)

#   erase existing user images
    print("\r\nPlease make sure the board is in CMSIS-DAP mode (LED is on)")
    answer = input("and press any key")
    print("Erasing the user images...")
    ret, output = erase_user_images(options.oocd_path)
    if ret != 0:
        return 1

    print("Wait while the board completes reboot...")
    time.sleep(5)

    print("\r\nPlease switch to DAP Link mode (LED is blinking)")
    input('and press any key when ready')
    print("Wait for 10 seconds while device is being enumareated...")
    time.sleep(10)

#   signing keys
    answer = \
        input('\r\nDo you want to create a new set of keys (y/N): ')
    if answer == 'Y' or answer == 'y':
        print('Create new keys.')
        create_app_keys()
    else:
        # TBD: check if the keys exist (open json, read keys)
        print('Will use existing keys.')

    ret = read_device_pub_key()
    if ret != 0:
        return 1

    ret = generate_device_cert()
    if ret != 0:
        return 1

    create_provisioning_packet()

    ret = re_provision_device(options.device, options.policy_file)
    if ret != 0:
        return 1

    print('\r\nSwitch back to the CMSIS-DAP mode (mode LED is on)')
    return 0


if __name__ == "__main__":
    main(sys.argv[1:])
