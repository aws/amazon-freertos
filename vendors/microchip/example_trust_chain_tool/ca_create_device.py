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
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives import serialization

from common import *

def pubnums_to_bytes(pub_nums):
    try:
        pubkey = pub_nums.x.to_bytes(32, byteorder='big', signed=False)
        pubkey += pub_nums.y.to_bytes(32, byteorder='big', signed=False)
    except AttributeError:
        pubkey = bytes(bytearray.fromhex(hex(pub_nums.x)[2:-1] + hex(pub_nums.y)[2:-1]))
    return pubkey

def device_cert_sn(size, builder):
    """Cert serial number is the SHA256(Subject public key + Encoded dates)"""

    # Get the public key as X and Y integers concatenated
    pubkey = pubnums_to_bytes(builder._public_key.public_numbers())

    # Get the encoded dates
    expire_years = 0
    enc_dates = bytearray(b'\x00'*3)
    enc_dates[0] = (enc_dates[0] & 0x07) | ((((builder._not_valid_before.year - 2000) & 0x1F) << 3) & 0xFF)
    enc_dates[0] = (enc_dates[0] & 0xF8) | ((((builder._not_valid_before.month) & 0x0F) >> 1) & 0xFF)
    enc_dates[1] = (enc_dates[1] & 0x7F) | ((((builder._not_valid_before.month) & 0x0F) << 7) & 0xFF)
    enc_dates[1] = (enc_dates[1] & 0x83) | (((builder._not_valid_before.day & 0x1F) << 2) & 0xFF)
    enc_dates[1] = (enc_dates[1] & 0xFC) | (((builder._not_valid_before.hour & 0x1F) >> 3) & 0xFF)
    enc_dates[2] = (enc_dates[2] & 0x1F) | (((builder._not_valid_before.hour & 0x1F) << 5) & 0xFF)
    enc_dates[2] = (enc_dates[2] & 0xE0) | ((expire_years & 0x1F) & 0xFF)
    enc_dates = bytes(enc_dates)

    # SAH256 hash of the public key and encoded dates
    digest = hashes.Hash(hashes.SHA256(), backend=default_backend())
    digest.update(pubkey)
    digest.update(enc_dates)
    raw_sn = bytearray(digest.finalize()[:size])
    raw_sn[0] = raw_sn[0] & 0x7F # Force MSB bit to 0 to ensure positive integer
    raw_sn[0] = raw_sn[0] | 0x40 # Force next bit to 1 to ensure the integer won't be trimmed in ASN.1 DER encoding

    try:
        return int.from_bytes(raw_sn, byteorder='big', signed=False)
    except AttributeError:
        return int(binascii.hexlify(raw_sn), 16)


def create_device_cert(public_key, device_id, signer_file, signer_key_file):
    # Check device_id
    if len(device_id) != 18:
        raise ValueError('Device Serial Number must be 18 hex characters')
    else:
        device_id = to_unicode(device_id)

    # Make sure files exist
    if not (os.path.isfile(signer_file) and os.path.isfile(signer_key_file)):
        raise FileNotFoundError('Failed to find {} or {}'.format(signer_file, signer_key_file))

    # Load the Signing key from the file
    with open(signer_key_file, 'rb') as f:
        signer_ca_priv_key = serialization.load_pem_private_key(data=f.read(), password=None, backend=default_backend())
        signer_ca_pub_key = bytearray(signer_ca_priv_key.public_key().public_bytes(encoding=serialization.Encoding.X962, 
            format=serialization.PublicFormat.UncompressedPoint)[1:])

    # Load the Signing Certificate from the file
    with open(signer_file, 'rb') as f:
        signer_ca_cert = x509.load_pem_x509_certificate(f.read(), default_backend())

    # Build certificate
    builder = x509.CertificateBuilder()

    builder = builder.issuer_name(signer_ca_cert.subject)

    # Device cert must have minutes and seconds set to 0
    builder = builder.not_valid_before(datetime.datetime.now(tz=pytz.utc).replace(minute=0,second=0))

    # Should be year 9999, but this doesn't work on windows
    builder = builder.not_valid_after(datetime.datetime(3000, 12, 31, 23, 59, 59))

    builder = builder.subject_name(x509.Name([
        x509.NameAttribute(x509.oid.NameOID.ORGANIZATION_NAME, u'Example Inc'),
        x509.NameAttribute(x509.oid.NameOID.COMMON_NAME, device_id)]))

    builder = builder.public_key(public_key)

    # Device certificate is generated from certificate dates and public key
    builder = builder.serial_number(device_cert_sn(16, builder))

    # Subject Key ID is used as the thing name and MQTT client ID and is required for this demo
    builder = builder.add_extension(
        x509.SubjectKeyIdentifier.from_public_key(public_key),
        critical=False)

    issuer_ski = signer_ca_cert.extensions.get_extension_for_class(x509.SubjectKeyIdentifier)
    builder = builder.add_extension(
        x509.AuthorityKeyIdentifier.from_issuer_public_key(signer_ca_priv_key.public_key()),
        critical=False)

    # Sign certificate 
    device_cert = builder.sign(private_key=signer_ca_priv_key, algorithm=hashes.SHA256(), backend=default_backend())

    with open('device.crt', 'wb') as f:
        f.write(device_cert.public_bytes(encoding=serialization.Encoding.PEM))

    print('Done');


def read_from_device():
    assert atcab_init(cfg_ateccx08a_kithid_default()) == Status.ATCA_SUCCESS

    # Load device public key
    public_key = bytearray(64)
    assert Status.ATCA_SUCCESS == atcab_get_pubkey(0, public_key)

    device_id = bytearray(9)
    assert Status.ATCA_SUCCESS == atcab_read_serial_number(device_id)
    device_id = binascii.hexlify(device_id).decode('ascii').upper()

    # Convert to the key to PEM format
    public_key_pem = convert_ec_pub_to_pem(public_key)

    # Convert the key into the cryptography format
    public_key = serialization.load_pem_public_key(public_key_pem.encode('ascii'), default_backend())

    return public_key, device_id


def read_from_file(filename, device_id):
    with open(filename, 'rb') as f:
        content = f.read()

        if b'REQUEST' in content:
            csr = x509.load_pem_x509_csr(content, default_backend())
            return csr.public_key(), csr.subject.get_attributes_for_oid(x509.oid.NameOID.COMMON_NAME)[0].value
        elif b'PUBLIC' in content:
            if device_id is None:
                raise ValueError('Device serial number (--sn) must be provided with public key')
            return serialization.load_pem_public_key(content, default_backend()), device_id
        else:
            raise ValueError('Unknown file content')

if __name__ == '__main__':
    # Create argument parser to document script use
    parser = argparse.ArgumentParser(description='Provisions the kit by requesting a CSR and returning signed certificates.')
    parser.add_argument('--file', default=None, help='Input Public key or Certificate Signing Request')
    parser.add_argument('--sn', default=None, help='Device Serial number if --file provided is a public key')
    parser.add_argument('--cert', default='signer-ca.crt', help='Certificate file of the signer')
    parser.add_argument('--key', default='signer-ca.key', help='Private Key file of the signer')
    args = parser.parse_args()

    if args.file is None:
        public_key, device_id = read_from_device()
    else:
        public_key, device_id = read_from_file(args.file, args.sn)

    create_device_cert(public_key, device_id, args.cert, args.key)
