#!/usr/bin/env python

import argparse
from enum import Enum
import sys

class CredentialType(Enum):
    PRIVATE_KEY = 1
    CLIENT_CERT = 2
    ROOT_CA_CERT = 3

CREDS_MACRO_MAP = {
    CredentialType.CLIENT_CERT: 'keyCLIENT_CERTIFICATE_PEM',
    CredentialType.ROOT_CA_CERT: 'keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM',
    CredentialType.PRIVATE_KEY: 'keyCLIENT_PRIVATE_KEY_PEM'
}

# Prints the passed credential file as a C string. If a specific type of the
# credential is passed, then its associated configuration macro
# from demos/include/aws_clientcredential_keys.h is also mentioned.
def printCredentialAsCString(file_path, type=None):
    with open(str(file_path), 'r') as f:
        content = f.read()

    print('============ Formatted', file_path,'===========\n')

    if type is not None:
        print("#define "+CREDS_MACRO_MAP[type]+" \\")

    # Format and print the PEM credential as C string.
    split_cred_list = content.rstrip('\n').split("\n")  
    for entry in split_cred_list[:-1]:  # Iterate over all lines of credentials except last one.
        print("\""+entry+"\\n\" \\")
    print("\""+split_cred_list[-1]+"\\n\"") # Print last line without the trailing '\'

    print('\n====================================================================\n\n')

parser = argparse.ArgumentParser( description="format-credential-as-c-string.py - Print PEM credentials as C strings" )
parser.add_argument("--private-key-file", type=str, help="The path (relative or absolute) to Device Private Key file")
parser.add_argument("--cert-file", type=str, help="The path (relative or absolute) to Device Certificate File")
parser.add_argument("--root-ca-file", type=str, help="The path (relative or absolute) to the Server Root CA file")
parser.add_argument("--pem-file", type=str, help="The path (relative or absolute) to any PEM credential file")
arg = parser.parse_args()

if arg.pem_file is not None:
    printCredentialAsCString(arg.pem_file)
if arg.private_key_file is not None:
    printCredentialAsCString(arg.private_key_file, CredentialType.PRIVATE_KEY)
if arg.cert_file is not None:
    printCredentialAsCString(arg.cert_file, CredentialType.CLIENT_CERT)
if arg.root_ca_file is not None:
    printCredentialAsCString(arg.root_ca_file, CredentialType.ROOT_CA_CERT)

if len(sys.argv) == 1:
    parser.print_help()
    exit(1)