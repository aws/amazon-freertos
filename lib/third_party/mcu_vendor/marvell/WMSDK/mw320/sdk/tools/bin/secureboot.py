#! /usr/bin/env python
# Copyright (C) 2015 Marvell International Ltd.
# All Rights Reserved.

# Secureboot wrapper script
import os, sys, getopt, subprocess, tempfile, re
from sys import platform as _platform

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
SECUREBOOT = ''

parsed_sec_conf_file = ''
enum_dict = {}

class SecBootError(Exception):
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

def get_secureboot():
    global SECUREBOOT
    if _platform == "linux" or _platform == "linux2":
        SECUREBOOT = which(SCRIPT_DIR + "/Linux/secureboot")
    elif _platform == "darwin":
        SECUREBOOT = which(SCRIPT_DIR + "/Darwin/secureboot")
    elif _platform == "win32" or _platform == "win64" or _platform == "cygwin":
        SECUREBOOT = which(SCRIPT_DIR + "/Windows/secureboot")
    if not len(SECUREBOOT):
        raise SecBootError("Error: secureboot is not available for your platform")

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
    print sys.argv[0] + " [-c | --config] <sec_conf_file> [options]"
    print "Optional Usage:"
    print " [-s | --kshdr] </path/to/keystore.h>"
    print "          Specify keystore header file"
    print "          [Must if security config file contains application specific keys]"
    print " [-d | --outdir] </path/to/outdir/>"
    print "          Specify directory where output files will be placed"
    print "          [If not specified, output files are generated where input files are placed"
    print "           or at the specified output file path]"
    print " [-b | --boot2] </path/to/boot2.bin>"
    print "          Generate secure boot2 image for input <boot2.bin> binary file"
    print " [-m | --mcufw] </path/to/mcufw.bin>"
    print "          Generate secure MCU firmware image for input <mcufw.bin> binary file"
    print " [-r | --rawboot2] <path/to/raw boot2.bin>"
    print "          Generate secure boot2 image for input <raw boot2.bin> binary file"
    print "          [Raw boot2 is 2nd-stage bootloader/firmware without bootrom header)"
    print " [-o | --otpprog] <path/to/otpprog/header>"
    print "          Generate a header file <otpprog/header> to be used by otp_prog application"
    print " [-M | --Makefile] <path/to/secboot.mk>"
    print "          Generate a helper makefile <secboot.mk> that can be used by build system"
    print " [-k | --ksblob] <path/to/keystore blob>"
    print "          Generate a stand-alone (unencrypted) keystore binary for debug purpose"
    print " [--key]"
    print "           Print value of a key from security config file"
    print " [-v | --verbose]"
    print "          Print verbose information"
    print " [-h | --help]"
    print "          Display usage"
    sys.stdout.flush()

# Cleanup only in case of error
def cleanup():
    if os.path.isfile(parsed_sec_conf_file):
        os.remove(parsed_sec_conf_file)

# A simple parser that parses enum ks_tlv_type in keystore.h header file
def parse_keystore_hdr(kshdr):
    global enum_dict
    try:
        with open(kshdr, 'r') as hfile:
            lines = hfile.readlines()
    except Exception as e:
        raise SecBootError(e)

    # Parse for enum, value pairs
    multi_line_comm = False
    enum_found = False
    enum_name = 'ks_tlv_type'
    enum_list = []
    for line in lines:
        # Remove comments first
        parsed_line = ''
        if multi_line_comm is False:
            if '/*' in line:
                multi_line_comm = True
                parsed_line = line[:line.find('/*')]
            elif '//' in line:
                parsed_line = line[:line.find('//')]
            else:
                parsed_line = line
        if multi_line_comm is True and '*/' in line:
            multi_line_comm = False
            parsed_line += line[line.find('*/') + 2:]

        # Remove extra white-spaces
        parsed_tokens = parsed_line.split()
        parsed_line = ''.join(parsed_tokens)

        # Find enum start
        if 'enum' in parsed_tokens:
            idx = parsed_tokens.index('enum')
            try:
                if parsed_tokens[idx + 1] in enum_name and parsed_tokens[idx + 2] in '{':
                    enum_found = True
                    parsed_line = ''.join(parsed_tokens[idx + 3:])
            except:
                raise SecBootError("Error: \"enum " + enum_name + " {\" must on the same line")

        # Skip if enum is not found
        if enum_found is False:
            continue
        # Mark enum end if found, will break after processing current line
        if '}' in parsed_line:
            enum_found = False
            parsed_line = parsed_line[:parsed_line.find('}')]

        # Convert enums to a (enum, val) dictionary
        enum_pairs = parsed_line.split(',')
        for pair in enum_pairs:
            if not pair:
                continue
            if '=' in pair:
                if pair.split('=')[1].isdigit():
                    val = pair.split('=')[1]
                elif pair.split('=')[1] in enum_dict:
                    val = enum_dict[pair.split('=')[1]]
                else:
                    raise SecBootError("Error: Undefined enum value " + pair.split('=')[0])
                enum_dict[pair.split('=')[0]] = val
                enum_list.append(pair.split('=')[0])
            else:
                if not enum_list:
                    val = '0'
                else:
                    val = str(int(enum_dict[enum_list[-1]]) + 1)
                enum_dict[pair] = val
                enum_list.append(pair)

        # Break if enum end is marked
        if enum_found is False:
            break

    del enum_list

    # Now restrict dictionary only to [KEY_APP_DATA_START, KEY_APP_DATA_END] range
    if 'KEY_APP_DATA_START' not in enum_dict or 'KEY_APP_DATA_END' not in enum_dict:
        raise SecBootError("Error: Application key guards KEY_APP_DATA_START and KEY_APP_DATA_END are not defined")

    for e in enum_dict.keys():
        if e in 'KEY_APP_DATA_START' or e in 'KEY_APP_DATA_END':
            continue
        if int(enum_dict[e]) < int(enum_dict['KEY_APP_DATA_START']) or int(enum_dict[e]) > int(enum_dict['KEY_APP_DATA_END']):
            del enum_dict[e]

# Replaces occurances of application key enums with integers and creates a parsed file
def parse_sec_conf(sec_conf_file):
    global enum_dict
    try:
        with open(sec_conf_file, 'r') as sfile:
            lines = sfile.readlines()
    except Exception as e:
        raise SecBootError(e)
    try:
        with tempfile.NamedTemporaryFile('w', prefix=os.path.basename(sec_conf_file) + '.', delete=False) as pfile:
            for line in lines:
                for e in enum_dict:
                    line = line.replace(e, enum_dict[e])
                pfile.write(line)
    except Exception as e:
        raise SecBootError(e)

    return file_path(pfile.name)

# Find the value of a key in security config file
def find_key_sec_conf(sec_conf_file, find_key):
    try:
        with open(sec_conf_file, 'r') as sfile:
            lines = sfile.readlines()
    except Exception as e:
        raise SecBootError(e)
    for line in lines:
        if line[0].startswith('#'):
            continue
        parsed_tokens = re.split(';|,|\t+|\n', line)
        if parsed_tokens[0] == find_key:
            print parsed_tokens[2]
            return

def main():
    global parsed_sec_conf_file
    global SCRIPT_DIR
    SCRIPT_DIR = file_path(SCRIPT_DIR)
    sec_conf_file = ''
    sboot_args = []
    find_key = ''

    if len(sys.argv) <= 1:
        print_usage()
        sys.exit(1)

    try:
        opts, args = getopt.gnu_getopt(sys.argv[1:], "c:d:s:b:m:r:o:M:t:k:vh", ["config=","kshdr=","outdir=","boot2=","mcufw=","rawboot2=","otpprog=","Makefile=","test=","ksblob=","key=","verbose","help"])
        if len(args):
            print_usage()
            sys.exit(1)
    except getopt.GetoptError as e:
        print e
        print_usage()
        sys.exit(1)

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            print_usage()
            sys.exit()

    for opt, arg in opts:
        if opt in ("-c", "--config"):
            sec_conf_file = file_path(arg)
        elif opt in ("-s", "--kshdr"):
            parse_keystore_hdr(file_path(arg))
        elif opt in ("-d", "--outdir"):
            sboot_args += ['-d', file_path(arg)]
        elif opt in ("-b", "--boot2"):
            sboot_args += ['-b', file_path(arg)]
        elif opt in ("-m", "--mcufw"):
            sboot_args += ['-m', file_path(arg)]
        elif opt in ("-r", "--rawboot2"):
            sboot_args += ['-r', file_path(arg)]
        elif opt in ("-o", "--optprog"):
            sboot_args += ['-o', file_path(arg)]
        elif opt in ("-M", "--Makefile"):
            sboot_args += ['-M', file_path(arg)]
        elif opt in ("-t", "--test"):
            sboot_args += ['-t', file_path(arg)]
        elif opt in ("-k", "--ksblob"):
            sboot_args += ['-k', file_path(arg)]
        elif opt in ("-v", "--verbose"):
            sboot_args += ['-v']
        elif opt in ("--key"):
            find_key = arg

    if not sec_conf_file:
        sys.exit("Error: Security config file is not specified")

    if len(find_key):
        find_key_sec_conf(sec_conf_file, find_key)

    if sboot_args:
        get_secureboot()
        parsed_sec_conf_file = parse_sec_conf(sec_conf_file)
        sboot_args = [SECUREBOOT, '-c', parsed_sec_conf_file] + sboot_args

        if '-v' in sboot_args:
            print ' '.join(sboot_args)

        try:
            subprocess.check_call(sboot_args)
        except subprocess.CalledProcessError as e:
            print e
            sys.exit(1)

        cleanup()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
    except SecBootError as e:
        print e
        cleanup()
