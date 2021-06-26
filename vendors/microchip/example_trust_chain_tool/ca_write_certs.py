import os
import base64
import argparse
import pytz
import binascii
import ctypes

import datetime
import cryptography
from cryptoauthlib import *
from cryptography import x509
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization

ROOT_PUBKEY_SLOT = 15

ATCACERT_DEF_SIGNER_CONFIG_ELEMENTS = (atcacert_cert_element_t*3)(
    atcacert_cert_element_t(
#        id='IssueDate',
        device_loc = atcacert_device_loc_t(**{
            'zone': atcacert_device_zone_t.DEVZONE_DATA,
            'slot': 14,
            'is_genkey': 0,
            'offset': 35-13,
            'count': 13}),
        cert_loc= atcacert_cert_loc_t(offset=97, count=13)
    ),
    atcacert_cert_element_t(
#        id='ExpireDate',
        device_loc = atcacert_device_loc_t(**{
            'zone': atcacert_device_zone_t.DEVZONE_DATA,
            'slot': 14,
            'is_genkey': 0,
            'offset': 50-13,
            'count': 13}),
        cert_loc = atcacert_cert_loc_t(offset=112, count=13)
    ),
    atcacert_cert_element_t(
#        id='AuthorityKeyId',
        device_loc = atcacert_device_loc_t(**{
            'zone': atcacert_device_zone_t.DEVZONE_DATA,
            'slot': 8,
            'is_genkey': 0,
            'offset': 0,
            'count': 20}),
        cert_loc = atcacert_cert_loc_t(offset=354, count=20)
    )
)


ATCACERT_DEF_SIGNER_CONFIG = {
    'type': atcacert_cert_type_t.CERTTYPE_X509,
    'template_id': 1,
    'chain_id': 0,
    'private_key_slot': 0,
    'sn_source': atcacert_cert_sn_src_t.SNSRC_STORED,
    'cert_sn_dev_loc': {
        'zone': atcacert_device_zone_t.DEVZONE_DATA,
        'slot': 14,
        'is_genkey': 0,
        'offset': 20-16,
        'count': 16
    },
    'issue_date_format': atcacert_date_format_t.DATEFMT_RFC5280_UTC,
    'expire_date_format': atcacert_date_format_t.DATEFMT_RFC5280_UTC,
    'tbs_cert_loc': {'offset': 4, 'count': 370},
    'expire_years': 10,
    'public_key_dev_loc': {
        'zone': atcacert_device_zone_t.DEVZONE_DATA,
        'slot': 11,
        'is_genkey': 0,
        'offset': 0,
        'count': 72
    },
    'comp_cert_dev_loc': {
        'zone': atcacert_device_zone_t.DEVZONE_DATA,
        'slot': 12,
        'is_genkey': 0,
        'offset': 0,
        'count': 72
    },
    'std_cert_elements' : [
        {'offset': 206, 'count': 64},
        {'offset': 386, 'count': 74},
        {'offset': 97, 'count': 13},
        {'offset': 112, 'count': 13},
        {'offset': 175, 'count': 4},
        {'offset': 15, 'count': 16},
        {'offset': 354, 'count': 20},
        {'offset': 321, 'count': 20},
    ],
    'cert_elements': ctypes.cast(ATCACERT_DEF_SIGNER_CONFIG_ELEMENTS, ctypes.POINTER(atcacert_cert_element_t)),
    'cert_elements_count': len(ATCACERT_DEF_SIGNER_CONFIG_ELEMENTS)
}

ATCACERT_DEF_DEVICE_CONFIG = {
    'type': atcacert_cert_type_t.CERTTYPE_X509,
    'template_id': 2,
    'chain_id': 0,
    'private_key_slot': 0,
    'sn_source': atcacert_cert_sn_src_t.SNSRC_PUB_KEY_HASH,
    'cert_sn_dev_loc': {
        'zone': atcacert_device_zone_t.DEVZONE_NONE,
        'slot': 0,
        'is_genkey': 0,
        'offset': 0,
        'count': 0
    },
    'issue_date_format': atcacert_date_format_t.DATEFMT_RFC5280_UTC,
    'expire_date_format': atcacert_date_format_t.DATEFMT_RFC5280_GEN,
    'tbs_cert_loc': {'offset': 4, 'count': 339},
    'expire_years': 0,
    'public_key_dev_loc': {
        'zone': atcacert_device_zone_t.DEVZONE_DATA,
        'slot': 0,
        'is_genkey': 1,
        'offset': 0,
        'count': 64
    },
    'comp_cert_dev_loc': {
        'zone': atcacert_device_zone_t.DEVZONE_DATA,
        'slot': 10,
        'is_genkey': 0,
        'offset': 0,
        'count': 72
    },
    'std_cert_elements' : [
        {'offset': 211, 'count': 64},
        {'offset': 355, 'count': 75},
        {'offset': 101, 'count': 13},
        {'offset': 0, 'count': 0},
        {'offset': 93, 'count': 4},
        {'offset': 15, 'count': 16},
        {'offset': 323, 'count': 20},
        {'offset': 290, 'count': 20},
    ]
}

def write_root(root_file):
    # Load the Signing Certificate from the file
    with open(root_file, 'rb') as f:
        cert = x509.load_pem_x509_certificate(f.read(), default_backend())

    root_pubkey = bytearray(cert.public_key().public_bytes(encoding=serialization.Encoding.X962, 
            format=serialization.PublicFormat.UncompressedPoint)[1:])
    
    assert Status.ATCA_SUCCESS == atcab_write_pubkey(ROOT_PUBKEY_SLOT, root_pubkey)

def write_signer(signer_file):
    # Load the Signing Certificate from the file
    with open(signer_file, 'rb') as f:
        cert = x509.load_pem_x509_certificate(f.read(), default_backend())

    cert_def = atcacert_def_t(**ATCACERT_DEF_SIGNER_CONFIG)
    cert = cert.public_bytes(encoding=serialization.Encoding.DER)
    assert Status.ATCA_SUCCESS == atcacert_write_cert(cert_def, cert, len(cert))

def write_device(device_file):
    # Load the Signing Certificate from the file
    with open(device_file, 'rb') as f:
        cert = x509.load_pem_x509_certificate(f.read(), default_backend())

    cert_def = atcacert_def_t(**ATCACERT_DEF_DEVICE_CONFIG)
    cert = cert.public_bytes(encoding=serialization.Encoding.DER)

    # Write the device certificate
    assert Status.ATCA_SUCCESS == atcacert_write_cert(cert_def, cert, len(cert))


if __name__ == '__main__':
    # Create argument parser to document script use
    parser = argparse.ArgumentParser(description='Programs a certificate chain into a device using the provided definitions')
    parser.add_argument('--cert', default='device.crt', help='Certificate file of the device (PEM)')
    parser.add_argument('--signer', default='signer-ca.crt', help='Certificate file of the signer (PEM)')
    parser.add_argument('--root', default='root-ca.crt', help='Device Certificate (PEM)')
    args = parser.parse_args()

    assert atcab_init(cfg_ateccx08a_kithid_default()) == Status.ATCA_SUCCESS
#    write_root(args.root)
    write_signer(args.signer)
    write_device(args.cert)

