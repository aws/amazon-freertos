import os
import pytz
import argparse
import datetime
import cryptography

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


def create_intermediate_cert(reg_code, verifyfile, certfile, keyfile, rootfile, rootkeyfile):
    print('\nLoading signer CA key')
    signer_ca_priv_key = load_or_create_key(keyfile)

    print('\nLoading root CA key')
    if not os.path.isfile(rootkeyfile):
        raise Exception('Failed to find root CA key file, ' + rootkeyfile + '. Have you run ca_create_root first?')
    with open(rootkeyfile, 'rb') as f:
        print('    Loading from ' + f.name)
        root_ca_priv_key = serialization.load_pem_private_key(
            data=f.read(),
            password=None,
            backend=crypto_be)

    print('\nLoading Root CA certificate')
    if not os.path.isfile(rootfile):
        raise Exception('Failed to find root CA certificate file, ' + rootfile + '. Have you run ca_create_root first?')
    with open(rootfile, 'rb') as f:
        print('    Loading from ' + f.name)
        root_ca_cert = x509.load_pem_x509_certificate(f.read(), crypto_be)

    # Create signer CA certificate
    print('\nGenerating Signer/Intermediate Certificate')
    
    # Please note that the structure of the signer certificate is part of certificate definition in the firmware
    # If any part of it is changed, it will also need to be changed in the firmware.
    builder = x509.CertificateBuilder()
    builder = builder.serial_number(random_cert_sn(16))
    builder = builder.subject_name(x509.Name([
        x509.NameAttribute(x509.oid.NameOID.ORGANIZATION_NAME, u'Example Inc'),
        x509.NameAttribute(x509.oid.NameOID.COMMON_NAME, u'Example Signer FFFF')]))    
    builder = builder.issuer_name(root_ca_cert.subject)
    builder = builder.not_valid_before(datetime.datetime.now(tz=pytz.utc))
    builder = builder.not_valid_after(builder._not_valid_before.replace(year=builder._not_valid_before.year + 10))
    builder = builder.public_key(signer_ca_priv_key.public_key())
    
    builder = builder.add_extension(
        x509.BasicConstraints(ca=True, path_length=0),
        critical=True)

    builder = builder.add_extension(
        x509.KeyUsage(
            digital_signature=True,
            content_commitment=False,
            key_encipherment=False,
            data_encipherment=False,
            key_agreement=False,
            key_cert_sign=True,
            crl_sign=True,
            encipher_only=False,
            decipher_only=False),
        critical=True)
    
    builder = builder.add_extension(
        x509.SubjectKeyIdentifier.from_public_key(signer_ca_priv_key.public_key()),
        critical=False)

    issuer_ski = root_ca_cert.extensions.get_extension_for_class(x509.SubjectKeyIdentifier)
    builder = builder.add_extension(
        x509.AuthorityKeyIdentifier.from_issuer_subject_key_identifier(issuer_ski),
        critical=False)

    # Sign signer certificate with root
    signer_ca_cert = builder.sign(
        private_key=root_ca_priv_key,
        algorithm=hashes.SHA256(),
        backend=crypto_be)

    # Write signer CA certificate to file
    with open(certfile, 'wb') as f:
        print('    Saving to ' + f.name)
        f.write(signer_ca_cert.public_bytes(encoding=serialization.Encoding.PEM))
        
    # Generate a verification certificate around the registration code (subject common name)
    print('\nGenerating signer CA AWS verification certificate')
    builder = x509.CertificateBuilder()
    builder = builder.serial_number(random_cert_sn(16))
    builder = builder.issuer_name(signer_ca_cert.subject)
    builder = builder.not_valid_before(datetime.datetime.now(tz=pytz.utc))
    builder = builder.not_valid_after(datetime.datetime.now(tz=pytz.utc) + datetime.timedelta(days=1))
    builder = builder.subject_name(x509.Name([x509.NameAttribute(x509.oid.NameOID.COMMON_NAME, reg_code)]))
    builder = builder.public_key(signer_ca_cert.public_key())
    signer_ca_ver_cert = builder.sign(
        private_key=signer_ca_priv_key,
        algorithm=hashes.SHA256(),
        backend=crypto_be)

    # Write signer CA certificate to file for reference
    with open(verifyfile, 'wb') as f:
        print('    Saved to ' + f.name)
        f.write(signer_ca_ver_cert.public_bytes(encoding=serialization.Encoding.PEM))

    print('\nDone')

if __name__ == '__main__':
    # Create argument parser to document script use
    parser = argparse.ArgumentParser(description='Create a Signer/Intermediate Certificate')
    parser.add_argument('code', help='AWS Verification Code')
    parser.add_argument('--vcert', default='verificationCert.crt', help='AWS Verification Certificate')
    parser.add_argument('--cert', default='signer-ca.crt', help='Certificate file of the signer')
    parser.add_argument('--key', default='signer-ca.key', help='Private Key file of the signer')
    parser.add_argument('--root', default='root-ca.crt', help='Root Certificate')
    parser.add_argument('--rootkey', default='root-ca.key', help='Root Key')
    args = parser.parse_args()

    create_intermediate_cert(args.code, args.vcert, args.cert, args.key, args.root, args.rootkey)
