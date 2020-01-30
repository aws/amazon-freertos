#!/usr/bin/env python
#
# Tests for espsecure.py
#
# Assumes openssl binary is in the PATH
import unittest
import subprocess
import os
import os.path
import io
import sys
import tempfile
import zlib
from collections import namedtuple

TEST_DIR = os.path.abspath(os.path.dirname(__file__))
os.chdir(TEST_DIR)

try:
    import espsecure
except ImportError:
    sys.path.insert(0, os.path.join(TEST_DIR, ".."))
    import espsecure

import esptool

class EspSecureTestCase(unittest.TestCase):

    def run_espsecure(self, args):
        """ Run espsecure.py with the specified arguments

        Returns output as a string if there is any, raises an exception if espsecure.py fails
        """
        cmd = [sys.executable, ESPSECURE_PY ] + args.split(" ")
        print("Running %s..." % (" ".join(cmd)))

        try:
            output = subprocess.check_output([str(s) for s in cmd],
                                             cwd=TEST_DIR,
                                             stderr=subprocess.STDOUT)
            print(output)
            return output.decode("utf-8")
        except subprocess.CalledProcessError as e:
            print(e.output)
            raise e

    def setUp(self):
        self.cleanup_files = []  # keep a list of files _open()ed by each test case

    def tearDown(self):
        for f in self.cleanup_files:
            f.close()

    def _open(self, image_file):
        f = open(os.path.join('secure_images', image_file), 'rb')
        self.cleanup_files.append(f)
        return f


class ESP32SecureBootloaderTests(EspSecureTestCase):

    def test_digest_bootloader(self):
        DBArgs = namedtuple('digest_bootloader_args', [
            'keyfile',
            'output',
            'iv',
            'image' ])

        try:
            output_file = tempfile.NamedTemporaryFile(delete=False)
            output_file.close()

            args = DBArgs(self._open('256bit_key.bin'),
                          output_file.name,
                          self._open('256bit_iv.bin'),
                          self._open('bootloader.bin'))
            espsecure.digest_secure_bootloader(args)

            with open(output_file.name, 'rb') as of:
                with self._open('bootloader_digested.bin') as ef:
                    self.assertEqual(ef.read(), of.read())
        finally:
            os.unlink(output_file.name)


class ECDSASigningTests(EspSecureTestCase):

    VerifyArgs = namedtuple('verify_signature_args', [
        'keyfile',
        'datafile' ])

    def test_sign_data(self):
        SignArgs = namedtuple('sign_data_args', [
            'keyfile',
            'output',
            'datafile' ])

        try:
            output_file = tempfile.NamedTemporaryFile(delete=False)
            output_file.close()

            # Note: signing bootloader is not actually needed
            # for ESP32, it's just a handy file to sign
            args = SignArgs(self._open('ecdsa_secure_boot_signing_key.pem'),
                            output_file.name,
                            self._open('bootloader.bin'))
            espsecure.sign_data(args)

            with open(output_file.name, 'rb') as of:
                with self._open('bootloader_signed.bin') as ef:
                    self.assertEqual(ef.read(), of.read())

        finally:
            os.unlink(output_file.name)

    def test_verify_signature_signing_key(self):
        # correct key
        args = self.VerifyArgs(self._open('ecdsa_secure_boot_signing_key.pem'),
                          self._open('bootloader_signed.bin'))
        espsecure.verify_signature(args)

        # wrong key
        args = self.VerifyArgs(self._open('ecdsa_secure_boot_signing_key2.pem'),
                               self._open('bootloader_signed.bin'))
        with self.assertRaises(esptool.FatalError) as cm:
            espsecure.verify_signature(args)
        self.assertIn("Signature is not valid", str(cm.exception))

    def test_verify_signature_public_key(self):
        # correct key
        args = self.VerifyArgs(self._open('ecdsa_secure_boot_signing_pubkey.pem'),
                               self._open('bootloader_signed.bin'))
        espsecure.verify_signature(args)

        # wrong key
        args = self.VerifyArgs(self._open('ecdsa_secure_boot_signing_pubkey2.pem'),
                               self._open('bootloader_signed.bin'))
        with self.assertRaises(esptool.FatalError) as cm:
            espsecure.verify_signature(args)
        self.assertIn("Signature is not valid", str(cm.exception))

    def test_extract_binary_public_key(self):
        ExtractKeyArgs = namedtuple('extract_public_key_args',
                                    [ 'keyfile', 'public_keyfile' ])

        pub_keyfile = tempfile.NamedTemporaryFile(delete=False)
        pub_keyfile2 = tempfile.NamedTemporaryFile(delete=False)
        try:
            args = ExtractKeyArgs(self._open('ecdsa_secure_boot_signing_key.pem'),
                                  pub_keyfile)
            espsecure.extract_public_key(args)

            args = ExtractKeyArgs(self._open('ecdsa_secure_boot_signing_key2.pem'),
                                  pub_keyfile2)
            espsecure.extract_public_key(args)

            pub_keyfile.seek(0)
            pub_keyfile2.seek(0)

            # use correct extracted public key to verify
            args = self.VerifyArgs(pub_keyfile, self._open('bootloader_signed.bin'))
            espsecure.verify_signature(args)

            # use wrong extracted public key to try and verify
            args = self.VerifyArgs(pub_keyfile2, self._open('bootloader_signed.bin'))
            with self.assertRaises(esptool.FatalError) as cm:
                espsecure.verify_signature(args)
            self.assertIn("Signature is not valid", str(cm.exception))

        finally:
            os.unlink(pub_keyfile.name)
            os.unlink(pub_keyfile2.name)


class ESP32FlashEncryptionTests(EspSecureTestCase):

    def test_encrypt_decrypt_bootloader(self):
        self._test_encrypt_decrypt('bootloader.bin',
                                   'bootloader-encrypted.bin',
                                   '256bit_key.bin',
                                   0x1000,
                                   0xf)

    def test_encrypt_decrypt_app(self):
        self._test_encrypt_decrypt('hello-world-signed.bin',
                                   'hello-world-signed-encrypted.bin',
                                   'ef-flashencryption-key.bin',
                                   0x20000,
                                   0xf)

    def test_encrypt_decrypt_non_default_conf(self):
        """ Try some non-default (non-recommended) flash_crypt_conf settings """
        for conf in [ 0x0, 0x3, 0x9, 0xc ]:
            self._test_encrypt_decrypt('bootloader.bin',
                                       'bootloader-encrypted-conf%x.bin' % conf,
                                       '256bit_key.bin',
                                       0x1000,
                                       conf)

    def _test_encrypt_decrypt(self, input_plaintext, expected_ciphertext, key_path, offset, flash_crypt_conf=0xf):
        EncryptArgs = namedtuple('encrypt_flash_data_args',
                                 [ 'keyfile',
                                   'output',
                                   'address',
                                   'flash_crypt_conf',
                                   'plaintext_file'
                                 ])

        DecryptArgs = namedtuple('decrypt_flash_data_args',
                                 [ 'keyfile',
                                   'output',
                                   'address',
                                   'flash_crypt_conf',
                                   'encrypted_file'
                                 ])

        original_plaintext = self._open(input_plaintext)
        keyfile = self._open(key_path)
        ciphertext = io.BytesIO()

        args = EncryptArgs(keyfile,
                           ciphertext,
                           offset,
                           flash_crypt_conf,
                           original_plaintext)
        espsecure.encrypt_flash_data(args)

        original_plaintext.seek(0)
        self.assertNotEqual(original_plaintext.read(), ciphertext.getvalue())
        with self._open(expected_ciphertext) as f:
            self.assertEqual(f.read(), ciphertext.getvalue())

        ciphertext.seek(0)
        keyfile.seek(0)
        plaintext = io.BytesIO()
        args = DecryptArgs(keyfile,
                           plaintext,
                           offset,
                           flash_crypt_conf,
                           ciphertext)
        espsecure.decrypt_flash_data(args)

        original_plaintext.seek(0)
        self.assertEqual(original_plaintext.read(), plaintext.getvalue())


if __name__ == '__main__':
    print("Running espsecure tests...")
    print("Using espsecure %s at %s" % (esptool.__version__, os.path.abspath(espsecure.__file__)))
    unittest.main(buffer=True)
