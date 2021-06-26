import os
import datetime
import pytz
import cryptography
import argparse

from cryptography import x509
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import ec

crypto_be = cryptography.hazmat.backends.default_backend()

def random_cert_sn(size):
    """Create a positive, non-trimmable serial number for X.509 certificates"""
    raw_sn = bytearray(os.urandom(size))
    raw_sn[0] = raw_sn[0] & 0x7F # Force MSB bit to 0 to ensure positive integer
    raw_sn[0] = raw_sn[0] | 0x40 # Force next bit to 1 to ensure the integer won't be trimmed in ASN.1 DER encoding
    return int.from_bytes(raw_sn, byteorder='big', signed=False)


def load_or_create_key(filename):
    # Create or load a root CA key pair
    priv_key = None
    if os.path.isfile(filename):
        # Load existing key
        with open(filename, 'rb') as f:
            priv_key = serialization.load_pem_private_key(
                data=f.read(),
                password=None,
                backend=crypto_be)
    if priv_key == None:
        # No private key loaded, generate new one
        priv_key = ec.generate_private_key(ec.SECP256R1(), crypto_be)
        # Save private key to file
        with open(filename, 'wb') as f:
            pem_key = priv_key.private_bytes(
                encoding=serialization.Encoding.PEM,
                format=serialization.PrivateFormat.PKCS8,
                encryption_algorithm=serialization.NoEncryption())
            f.write(pem_key)
    return priv_key


def create_root_cert_key(certfile, keyfile, pubfile):
    # Create or load a root CA key pair
    print('\nLoading root CA key')
    root_ca_priv_key = load_or_create_key(keyfile)

    # Create root CA certificate
    print('\nGenerating self-signed root CA certificate')
    builder = x509.CertificateBuilder()
    builder = builder.serial_number(random_cert_sn(16))
    # Please note that the name of the root CA is also part of the signer certificate and thus, it's
    # part of certificate definition in the firmware (g_cert_elements_1_signer). If this name is
    # changed, it will also need to be changed in the firmware.
    builder = builder.issuer_name(x509.Name([
        x509.NameAttribute(x509.oid.NameOID.ORGANIZATION_NAME, u'Example Inc'),
        x509.NameAttribute(x509.oid.NameOID.COMMON_NAME, u'Example Root CA')]))
    builder = builder.not_valid_before(datetime.datetime.now(tz=pytz.utc))
    builder = builder.not_valid_after(builder._not_valid_before.replace(year=builder._not_valid_before.year + 25))
    builder = builder.subject_name(builder._issuer_name)
    builder = builder.public_key(root_ca_priv_key.public_key())
    builder = builder.add_extension(
        x509.SubjectKeyIdentifier.from_public_key(root_ca_priv_key.public_key()),
        critical=False)
    builder = builder.add_extension(
        x509.BasicConstraints(ca=True, path_length=None),
        critical=True)
    # Self-sign certificate
    root_ca_cert = builder.sign(
        private_key=root_ca_priv_key,
        algorithm=hashes.SHA256(),
        backend=crypto_be)

    # Write root CA certificate to file
    with open(certfile, 'wb') as f:
        print('    Saving to ' + f.name)
        f.write(root_ca_cert.public_bytes(encoding=serialization.Encoding.PEM))

    # Save root public key
    with open(pubfile, 'wb') as f:
        f.write(root_ca_cert.public_key().public_bytes(serialization.Encoding.PEM, serialization.PublicFormat.SubjectPublicKeyInfo))

    print('\nDone')

if __name__ == '__main__':
    # Create argument parser to document script use
    parser = argparse.ArgumentParser(description='Create a root certificate and key')
    parser.add_argument('--out', default='root-ca.crt', help='Device Certificate (PEM)')
    parser.add_argument('--key', default='root-ca.key', help='Root public key (PEM)')
    parser.add_argument('--pubkey', default='root-pub.pem', help='Root public key (PEM)')
    args = parser.parse_args()

    create_root_cert_key(args.out, args.key, args.pubkey)