#! /usr/bin/env python
# Copyright (C) 2015 Marvell International Ltd.
# All Rights Reserved.

# Secure configuration generation script
import os, sys, getopt, errno, subprocess
from sys import platform as _platform

sec_conf_file = ''
sec_conf_dir = os.getcwd()
sec_conf_pfx = 'security-c'
sec_conf_sfx = '.txt'
sample_keys_dir = os.getcwd()
boot2_pemkey = 'boot2-prvKey.pem'
boot2_prvkey = 'boot2-prvKey.der'
boot2_pubkey = 'boot2-pubKey.der'
mcufw_pemkey = 'mcufw-prvKey.pem'
mcufw_prvkey = 'mcufw-prvKey.der'
mcufw_pubkey = 'mcufw-pubKey.der'

# List of all supported configurations
# Note: secure configs 1 to 50 are developer configs where sample or fixed keys are used.
# secure configs 51 to 100 are manufacturing configs where unique set of keys is created
supported_sec_confs = ['1','2','3','4','50']
supported_sec_confs += ['51','52','53','54']

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
OPENSSL = ''

class SecConfError(Exception):
    def __init__(self, msg):
        self.msg = msg
    def __str__(self):
        return repr(self.msg)

# We define which as it may not be available on Windows
def which(program):
    if _platform == "win32" or _platform == "win64" or _platform == "cygwin":
        program = program + '.exe'

    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file
    return ""

def get_openssl():
    global OPENSSL
    OPENSSL = which("openssl")
    if not len(OPENSSL):
        if _platform == "win32" or _platform == "win64":
            OPENSSL = which(SCRIPT_DIR + "/GnuWin32/bin/openssl")
    if not len(OPENSSL):
        raise SecConfError("Error: Please install openssl for your platform")

def file_path(file_name):
    if _platform == "win32" or _platform == "win64":
        if len(which("cygpath")):
            return subprocess.Popen(['cygpath', '-m', file_name], stdout = subprocess.PIPE).communicate()[0].strip()
        else:
            return file_name.replace('\\', '/')
    elif _platform == "cygwin":
        return subprocess.Popen(['cygpath', '-m', file_name], stdout = subprocess.PIPE).communicate()[0].strip()
    else:
        return file_name

def print_usage():
    print ""
    print "Usage:"
    print sys.argv[0] + " [-d | --directory] <sec_conf_dir> [options]"
    print "Optional Usage:"
    print " [<-f | --filename> <value/of/SECURE_BOOT>]"
    print "          Prints absolute path of secure config file for SECURE_BOOT configuration"
    print " [<-c | --configuration> <value/of/SECURE_BOOT>]"
    print "          Generate secure boot configuration files for SECURE_BOOT configuration"
    print " [<-s | --sampledir> <path/to/sample keys/dir>]"
    print "          Specify directory that contains pre-generated keys to be used for given configuration"
    print " [-h | --help]"
    print "          Display usage"
    sys.stdout.flush()

# Cleanup only in case of error
def cleanup():
    if os.path.isfile(sec_conf_file):
        os.remove(sec_conf_file)

def write_conf_file(str):
    try:
        with open(sec_conf_file, 'a') as sfile:
            sfile.write(str)
    except Exception as e:
        print e
        cleanup()
        sys.exit(1)

# arg: binary = 'boot2', 'mcufw'
def check_sample_keys(binary):
    if binary in 'boot2':
        prvkey = sample_keys_dir + '/' + boot2_prvkey
        pubkey = sample_keys_dir + '/' + boot2_pubkey
    elif binary in 'mcufw':
        prvkey = sample_keys_dir + '/' + mcufw_prvkey
        pubkey = sample_keys_dir + '/' + mcufw_pubkey
    else:
        raise SecConfError("Error: Invalid check_sample_keys() argument")

    if os.path.isfile(prvkey) and os.path.isfile(pubkey):
        return
    else:
        raise SecConfError("Error: Sample RSA keys for %s are not found" % binary)

# arg: binary = 'boot2', 'mcufw'
def gen_rsa_keypair(binary):
    if binary in 'boot2':
        pemkey = sec_conf_dir + '/' + boot2_pemkey
        prvkey = sec_conf_dir + '/' + boot2_prvkey
        pubkey = sec_conf_dir + '/' + boot2_pubkey
    elif binary in 'mcufw':
        pemkey = sec_conf_dir + '/' + mcufw_pemkey
        prvkey = sec_conf_dir + '/' + mcufw_prvkey
        pubkey = sec_conf_dir + '/' + mcufw_pubkey
    else:
        raise SecConfError("Error: Invalid gen_rsa_keypair() argument")

    get_openssl()
    try:
        if os.path.isfile(prvkey) == False:
            subprocess.check_call([OPENSSL, 'genrsa', '-out', pemkey, '2048'])
            subprocess.check_call([OPENSSL, 'rsa', '-inform', 'PEM', '-in', pemkey, '-outform', 'DER', '-out', prvkey])
            os.remove(pemkey)
        if os.path.isfile(pubkey) == False:
            subprocess.check_call([OPENSSL, 'rsa', '-inform', 'DER', '-in', prvkey, '-outform', 'DER', '-out', pubkey, '-pubout'])
    except subprocess.CalledProcessError as e:
        print e
        cleanup()
        sys.exit(1)

# arg: val_len = length of random number sequence to generate
def gen_random(val_len):
    return os.urandom(val_len).encode('hex').upper()

# arg: val = 'yes', 'no'
def jtag_disable(val):
    lines = '\n'
    if val in 'no':
        lines += "# JTAG is not disabled\n"
    elif val in 'yes':
        lines += "# JTAG is disabled\n"
        lines += 'KEY_OTP_JTAG_ENABLE' + '\t' + 'DEC' + '\t' + '0' + '\n'
    else:
        raise SecConfError("Error: Invalid jtag_disable() argument")

    write_conf_file(lines)
    return

def nonflash_boot_disable(val):
    lines = '\n'
    if val in 'no':
        lines += "# UART/USB boot is not disabled\n"
    elif val in 'yes':
        lines += "# UART/USB boot is disabled\n"
        lines += 'KEY_OTP_UART_USB_BOOT_ENABLE' + '\t' + 'DEC' + '\t' + '0' + '\n'
    else:
        raise SecConfError("Error: Invalid nonflash_boot_disable() argument")

    write_conf_file(lines)
    return

def boot2_sign(val):
    lines = '\n'
    if val in 'no_sign':
        lines += "# Boot2 is not signed\n"
    elif val in 'dev_sign':
        check_sample_keys('boot2')
        lines += "# Boot2 is signed using RSA-2048\n"
        lines += 'KEY_BOOT2_SIGNING_ALGO' + '\t' + 'STR' + '\t' + 'RSA_SIGN' + '\n'
        lines += 'KEY_BOOT2_PRIVATE_KEY' + '\t' + 'FILE' + '\t' + sample_keys_dir + '/' + boot2_prvkey + '\n'
        lines += 'KEY_BOOT2_PUBLIC_KEY' + '\t' + 'FILE' + '\t' + sample_keys_dir + '/' + boot2_pubkey + '\n'
    elif val in 'mfg_sign':
        gen_rsa_keypair('boot2')
        lines += "# Boot2 is signed using RSA-2048\n"
        lines += 'KEY_BOOT2_SIGNING_ALGO' + '\t' + 'STR' + '\t' + 'RSA_SIGN' + '\n'
        lines += 'KEY_BOOT2_PRIVATE_KEY' + '\t' + 'FILE' + '\t' + sec_conf_dir + '/' + boot2_prvkey + '\n'
        lines += 'KEY_BOOT2_PUBLIC_KEY' + '\t' + 'FILE' + '\t' + sec_conf_dir + '/' + boot2_pubkey + '\n'
    else:
        raise SecConfError("Error: Invalid boot2_sign() argument")

    write_conf_file(lines)
    return

def boot2_encrypt(val):
    lines = '\n'
    if val in 'no_encrypt':
        lines += "# Boot2 is not encrypted\n"
    elif val in 'dev_encrypt':
        lines += "# Boot2 is encrypted using AES CCM-256\n"
        lines += 'KEY_BOOT2_ENCRYPT_ALGO' + '\t' + 'STR' + '\t' + 'AES_CCM_256_ENCRYPT' + '\n'
        lines += 'KEY_BOOT2_ENCRYPT_KEY' + '\t' + 'HEX' + '\t' + '0123456776543210010101011212121212345678232323238765432100112233' + '\n'
        lines += 'KEY_BOOT2_NONCE' + '\t\t' + 'HEX' + '\t' + '11111111111111111111111200' + '\n'
    elif val in 'mfg_encrypt':
        lines += "# Boot2 is encrypted using AES CCM-256\n"
        lines += 'KEY_BOOT2_ENCRYPT_ALGO' + '\t' + 'STR' + '\t' + 'AES_CCM_256_ENCRYPT' + '\n'
        lines += 'KEY_BOOT2_ENCRYPT_KEY' + '\t' + 'HEX' + '\t' + gen_random(32) + '\n'
    else:
        raise SecConfError("Error: Invalid boot2_encrypt() argument")

    write_conf_file(lines)
    return

def mcufw_sign(val):
    lines = '\n'
    if val in 'no_sign':
        lines += "# MCUFW is not signed\n"
    elif val in 'dev_sign':
        check_sample_keys('mcufw')
        lines += "# MCUFW is signed using RSA-2048\n"
        lines += 'KEY_MCUFW_SIGNING_ALGO' + '\t' + 'STR' + '\t' + 'RSA_SIGN' + '\n'
        lines += 'KEY_MCUFW_HASH_ALGO' + '\t' + 'STR' + '\t' + 'SHA256_HASH' + '\n'
        lines += 'KEY_MCUFW_PRIVATE_KEY' + '\t' + 'FILE' + '\t' + sample_keys_dir + '/' + mcufw_prvkey + '\n'
        lines += 'KEY_MCUFW_PUBLIC_KEY' + '\t' + 'FILE' + '\t' + sample_keys_dir + '/' + mcufw_pubkey + '\n'
    elif val in 'mfg_sign':
        gen_rsa_keypair('mcufw')
        lines += "# MCUFW is signed using RSA-2048\n"
        lines += 'KEY_MCUFW_SIGNING_ALGO' + '\t' + 'STR' + '\t' + 'RSA_SIGN' + '\n'
        lines += 'KEY_MCUFW_HASH_ALGO' + '\t' + 'STR' + '\t' + 'SHA256_HASH' + '\n'
        lines += 'KEY_MCUFW_PRIVATE_KEY' + '\t' + 'FILE' + '\t' + sec_conf_dir + '/' + mcufw_prvkey + '\n'
        lines += 'KEY_MCUFW_PUBLIC_KEY' + '\t' + 'FILE' + '\t' + sec_conf_dir + '/' + mcufw_pubkey + '\n'
    else:
        raise SecConfError("Error: Invalid mcufw_sign() argument")

    write_conf_file(lines)
    return

def mcufw_encrypt(val):
    lines = '\n'
    if val in 'no_encrypt':
        lines += "# MCUFW is not encrypted\n"
    elif val in 'dev_encrypt':
        lines += "# MCUFW is encrypted using AES CTR-128\n"
        lines += 'KEY_MCUFW_ENCRYPT_ALGO' + '\t' + 'STR' + '\t' + 'AES_CTR_128_ENCRYPT' + '\n'
        lines += 'KEY_MCUFW_ENCRYPT_KEY' + '\t' + 'HEX' + '\t' + '2B7E151628AED2A6ABF7158809CF4F3C' + '\n'
        lines += 'KEY_MCUFW_NONCE' + '\t\t' + 'HEX' + '\t' + 'F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF' + '\n'
    elif val in 'mfg_encrypt':
        lines += "# MCUFW is encrypted using AES CTR-128\n"
        lines += 'KEY_MCUFW_ENCRYPT_ALGO' + '\t' + 'STR' + '\t' + 'AES_CTR_128_ENCRYPT' + '\n'
        lines += 'KEY_MCUFW_ENCRYPT_KEY' + '\t' + 'HEX' + '\t' + gen_random(16) + '\n'
    else:
        raise SecConfError("Error: Invalid mcufw_encrypt() argument")

    write_conf_file(lines)
    return

def psm_encrypt(val):
    lines = '\n'
    if val in 'no_encrypt':
        lines += "# PSM is not encrypted\n"
    elif val in 'dev_encrypt':
        lines += "# PSM is encrypted using AES CTR-128\n"
        lines += 'KEY_PSM_ENCRYPT_KEY' + '\t' + 'HEX' + '\t' + '21D85243231C2A9A4AB667A946AA1F89' + '\n'
        lines += 'KEY_PSM_NONCE' + '\t\t' + 'HEX' + '\t' + 'F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF' + '\n'
    elif val in 'mfg_encrypt':
        lines += "# PSM is encrypted using AES CTR-128\n"
        lines += 'KEY_PSM_ENCRYPT_KEY' + '\t' + 'HEX' + '\t' + gen_random(16) + '\n'
        lines += 'KEY_PSM_NONCE' + '\t\t' + 'HEX' + '\t' + gen_random(16) + '\n'
    else:
        raise SecConfError("Error: Invalid psm_encrypt() argument")

    write_conf_file(lines)
    return

def generate_sec_conf():
    global sec_conf_file, sec_conf_dir

    if (os.path.isfile(sec_conf_file)):
        return

    conf_file = os.path.basename(sec_conf_file)
    if conf_file.startswith(sec_conf_pfx) and conf_file.endswith(sec_conf_sfx):
        sec_conf_num = conf_file[len(sec_conf_pfx):len(conf_file) - len(sec_conf_sfx)]
    else:
        sys.exit("Error: Security config file %s does not exist." % sec_conf_file)

    if sec_conf_num not in supported_sec_confs:
        sys.exit("Error: This security configuration is not supported.")

    try:
        os.mkdir(sec_conf_dir)
    except OSError as e:
        if e.errno is not errno.EEXIST:
            print e
            sys.exit(1)

    if sec_conf_num in '1':
        # This configuation is a duplicate of sec_conf_num = 4
        # Copy below the configuration that you want to set as devel default
        write_conf_file("# Security Configuration 1\n")
        jtag_disable('no')
        nonflash_boot_disable('no')
        boot2_sign('dev_sign')
        boot2_encrypt('dev_encrypt')
        mcufw_sign('dev_sign')
        mcufw_encrypt('no_encrypt')
        psm_encrypt('dev_encrypt')
    elif sec_conf_num in '2':
        write_conf_file("# Security Configuration 2\n")
        jtag_disable('no')
        nonflash_boot_disable('no')
        boot2_sign('no_sign')
        boot2_encrypt('dev_encrypt')
        mcufw_sign('dev_sign')
        mcufw_encrypt('no_encrypt')
        psm_encrypt('dev_encrypt')
    elif sec_conf_num in '3':
        write_conf_file("# Security Configuration 3\n")
        jtag_disable('no')
        nonflash_boot_disable('no')
        boot2_sign('dev_sign')
        boot2_encrypt('dev_encrypt')
        mcufw_sign('dev_sign')
        mcufw_encrypt('dev_encrypt')
        psm_encrypt('dev_encrypt')
    elif sec_conf_num in '4':
        write_conf_file("# Security Configuration 4\n")
        jtag_disable('no')
        nonflash_boot_disable('no')
        boot2_sign('dev_sign')
        boot2_encrypt('dev_encrypt')
        mcufw_sign('dev_sign')
        mcufw_encrypt('no_encrypt')
        psm_encrypt('dev_encrypt')
    elif sec_conf_num in '50':
        write_conf_file("# Security Configuration 50\n")
        jtag_disable('no')
        nonflash_boot_disable('no')
        boot2_sign('no_sign')
        boot2_encrypt('no_encrypt')
        mcufw_sign('no_sign')
        mcufw_encrypt('no_encrypt')
        psm_encrypt('dev_encrypt')
    elif sec_conf_num in '51':
        # This configuation is a duplicate of sec_conf_num = 54
        # Copy below the configuration that you want to set as mfg default
        write_conf_file("# Security Configuration 51\n")
        jtag_disable('yes')
        nonflash_boot_disable('yes')
        boot2_sign('mfg_sign')
        boot2_encrypt('mfg_encrypt')
        mcufw_sign('mfg_sign')
        mcufw_encrypt('no_encrypt')
        psm_encrypt('mfg_encrypt')
    elif sec_conf_num in '52':
        write_conf_file("# Security Configuration 52\n")
        jtag_disable('yes')
        nonflash_boot_disable('yes')
        boot2_sign('no_sign')
        boot2_encrypt('mfg_encrypt')
        mcufw_sign('mfg_sign')
        mcufw_encrypt('no_encrypt')
        psm_encrypt('mfg_encrypt')
    elif sec_conf_num in '53':
        write_conf_file("# Security Configuration 53\n")
        jtag_disable('yes')
        nonflash_boot_disable('yes')
        boot2_sign('mfg_sign')
        boot2_encrypt('mfg_encrypt')
        mcufw_sign('mfg_sign')
        mcufw_encrypt('mfg_encrypt')
        psm_encrypt('mfg_encrypt')
    elif sec_conf_num in '54':
        write_conf_file("# Security Configuration 54\n")
        jtag_disable('yes')
        nonflash_boot_disable('yes')
        boot2_sign('mfg_sign')
        boot2_encrypt('mfg_encrypt')
        mcufw_sign('mfg_sign')
        mcufw_encrypt('no_encrypt')
        psm_encrypt('mfg_encrypt')

    return

def get_sec_conf_path(sboot_val):
    global sec_conf_dir
    try:
        int(sboot_val)
        if sboot_val in supported_sec_confs:
            return sec_conf_dir + '/' + sec_conf_pfx + sboot_val + sec_conf_sfx
        else:
            sys.exit("Error: This security configuration is not supported.")
    except ValueError as e:
        if os.path.isfile(file_path(sboot_val)):
            return file_path(sboot_val)
        else:
            sys.exit("Error: Security config file %s does not exist." % sboot_val)

def main():
    global sec_conf_file, sec_conf_dir, sample_keys_dir

    if len(sys.argv) <= 1:
        print_usage()
        sys.exit(1)

    try:
        opts, args = getopt.gnu_getopt(sys.argv[1:], "d:f:c:s:h", ["directory=","filename=","configuration=","sampledir=","help"])
        if len(args):
            print_usage()
            sys.exit(1)
    except getopt.GetoptError as e:
        print e
        print_usage()
        sys.exit(1)

    for opt, arg in opts:
        if opt in ("-d", "--directory"):
            sec_conf_dir = arg
        elif opt in ("-s", "--sampledir"):
            sample_keys_dir = arg

    sec_conf_dir = file_path(sec_conf_dir)
    sample_keys_dir = file_path(sample_keys_dir)

    for opt, arg in opts:
        if opt in ("-f", "--filename"):
            print get_sec_conf_path(arg)
            sys.stdout.flush()
            sys.exit()
        elif opt in ("-c", "--configuration"):
            sec_conf_file = get_sec_conf_path(arg)
            generate_sec_conf()
            sys.exit()
        elif opt in ("-h", "--help"):
            print_usage()
            sys.exit()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
    except SecConfError as e:
        print e
        cleanup()
