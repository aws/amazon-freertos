#!/usr/bin/python3
"""
Copyright (c) 2019 Cypress Semiconductor Corporation

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

from cysecuretools import CySecureTools
import sys, argparse
import os
from shutil import copyfile, move


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
                        required=True)

    parser.add_argument('-d', '--device',
                        dest='device',
                        action='store',
                        type=str,
                        help="device target",
                        required=True)

    parser.add_argument('-s', '--s_hex',
                        dest='s_hex_file',
                        action='store',
                        default='',
                        type=str,
                        help="TFM SPE image to sign in hex format")

    parser.add_argument('-ns', '--ns_hex',
                        dest='ns_hex_file',
                        action='store',
                        default='',
                        type=str,
                        help="TFM NSPE image to sign in hex format")

    options = parser.parse_args(argv)
    return options


def main(argv):

    options = myargs(argv)
    print("options={}".format(options))

    if not options.s_hex_file and not options.ns_hex_file:
        print('Error: no files to sign')
        exit(1)

    tools = CySecureTools(options.device, options.policy_file)

    if options.s_hex_file:
        print('signing tfm_s image:', options.s_hex_file)
        tools.sign_image(options.s_hex_file, 1)

        # rename signed image to *_signed.hex
        name, ext = os.path.splitext(options.s_hex_file)
        s_hex_signed_file = name + '_signed' + ext
        try:
            move(options.s_hex_file, s_hex_signed_file)
        except IOError as e:
            print("Failed to copy file '{}' to '{}' ({})"
                   .format(options.s_hex_file, s_hex_signed_file, e.message))
            raise
        print('Signed TFM-S image:', s_hex_signed_file)

    if options.ns_hex_file:
        print('signing tfm_ns image:', options.ns_hex_file)
        tools.sign_image(options.ns_hex_file, 16)

        # rename signed image to *_signed.hex
        name, ext = os.path.splitext(options.ns_hex_file)
        ns_hex_signed_file = name + '_signed' + ext
        try:
            move(options.ns_hex_file, ns_hex_signed_file)
        except IOError as e:
            print("Failed to copy file '{}' to '{}' ({})"
                   .format(options.ns_hex_file, ns_hex_signed_file, e.message))
            raise
        print('Signed TFM-NS image:', ns_hex_signed_file)

        # for CM4, sign_image creates an unsigned copy of the image
        # named <image to sign>_cm4.hex. Delete it to avoid confusion.
        file_name = name + '_cm4' + ext
        if os.path.isfile(file_name):
            try:
                os.remove(file_name)
            except IOError:
                print("Could not erase '{}'"
                          .format(file_name))

    print('Done.')

if __name__ == "__main__":
   main(sys.argv[1:])
