#! /usr/bin/env python
# Copyright (C) 2018 Marvell International Ltd.
# All Rights Reserved.

# Flashprog wrapper/helper script
# Note: sys.stdout.flush() and sys.stderr.flush() are required for proper
# console output in eclipse

import os, sys, platform, getopt, subprocess, shutil
from sys import platform as _platform

IFC_FILE = (os.getenv("DEBUG_INTERFACE", "ftdi") or "ftdi") + '.cfg'
LAYOUT_FILE = "flashprog.layout"
CONFIG_FILE = "flashprog.config"
MFG_FILE = "flashprog.mfg"
MAX_FLASH_TYPE = 2

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
OPENOCD = ""

class WrongArgs(Exception):
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

def get_openocd():
    global OPENOCD
    if _platform == "linux" or _platform == "linux2":
        if (platform.machine() == "i686"):
            OPENOCD = which(SCRIPT_DIR + "/Linux/openocd")
        else:
            OPENOCD = which(SCRIPT_DIR + "/Linux/openocd64")
        if not len(OPENOCD):
            OPENOCD = which("openocd")
    elif _platform == "darwin":
        OPENOCD = which("openocd")
    elif _platform == "win32" or _platform == "win64" or _platform == "cygwin":
        OPENOCD = which(SCRIPT_DIR + "/Windows/openocd")
    if not len(OPENOCD):
        print "Error: Please install OpenOCD for your platform"
        sys.exit()

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

def cleanup():
    if (os.path.exists(LAYOUT_FILE)):
        os.remove(LAYOUT_FILE)
    if (os.path.exists(CONFIG_FILE)):
        os.remove(CONFIG_FILE)
    if (os.path.exists(MFG_FILE)):
        os.remove(MFG_FILE)

def exit():
    print_usage()
    cleanup()
    sys.exit()

def print_usage():
    print ""
    print "Usage:"
    print sys.argv[0] + " [options]"
    print "Optional Usage:"
    print " [<-i | --interface> <JTAG hardware interface name>]"
    print "          Supported ones are ftdi, jlink, amontec, malink and stlink. Default is ftdi."
    print " [<-l | --new-layout> </path/to/layout_file>]"
    print "          Flash partition using layout file <layout.txt>"
    print " [<--comp_name> </path/to/comp_file>]"
    print "          Write any component <comp_name> file <comp_file> to flash"
    print " [-d | --debug]"
    print "          Debug mode, dump current flash contents"
    print " [<-b | --batch> </path/to/config_file>]"
    print "          Batch processing mode config file <config.txt>"
    print " [--read <flash_id>,<start_address>,<length>,<path/to/output_file>]"
    print "          Read flash <flash_id> (0: Internal QSPI, 1: External QSPI, 2: External SPI)"
    print "          starting from <start_address> of length <length> into output file <output_file>"
    print " [--write <flash_id>,<start_address>,<path/to/input_file>]"
    print "          Write to flash <flash_id> (0: Internal QSPI, 1: External QSPI, 2: External SPI)"
    print "          starting from <start_address> from input file <input_file>"
    print "          Note: '-w | --write' option does not erase the flash before writing."
    print "          If required, explicitly erase the flash before."
    print " [--erase <flash_id>,<start_address>,<length>]"
    print "          Erase flash <flash_id> (0: Internal QSPI, 1: External QSPI, 2: External SPI)"
    print "          starting from <start_address> and length <length>"
    print " [--erase-all <flash_id>]"
    print "          Erase entire flash <flash_id> (0: Internal QSPI, 1: External QSPI, 2: External SPI)"
    print " [<-p | --prod> </path/to/mfg_file>]"
    print "          Batch processing mode mfg file <mfg.txt>"
    print " [-r | --reset]"
    print "          Reset board"
    print " [--interactive]"
    print "          Run flashprog in interactive mode (Default: non-interactive)"
    print " [-h | --help]"
    print "          Display usage"
    sys.stdout.flush()

def copy_file(src, dest):
    try:
        shutil.copy2(file_path(src), dest)
    except (shutil.Error, IOError) as e:
        print e
        cleanup()
        sys.exit()

def copy_config_option(comp_name, comp_file):
    if (comp_name != 'debug') and (os.path.exists(file_path(comp_file)) == False):
        print "File " + comp_file + " does not exist"
        cleanup()
        sys.exit()
    with open (CONFIG_FILE, 'a') as cfile:
        cfile.write(comp_name + ' ' + file_path(comp_file) + '\n')

def copy_mfg_option(mfg_cmd, arg, no_subargs):
    try:
        subargs = arg.strip().split(',')

        if (len(subargs) != no_subargs):
            raise WrongArgs("Invalid number of arguments")
        for subarg in subargs:
            if (len(subarg) == 0):
                raise WrongArgs("Invalid number of arguments")
        try:
            if (int(subargs[0], 0) < 0) or (int(subargs[0], 0) >= MAX_FLASH_TYPE):
                raise WrongArgs("Flash id is out of range [0-" + (str)(MAX_FLASH_TYPE - 1) + "]")
            if (mfg_cmd != 'erase-all'):
                if (int(subargs[1], 0) < 0):
                    raise WrongArgs("Invalid value of address")
            if (mfg_cmd != 'erase-all') and (mfg_cmd != 'write'):
                if (int(subargs[2], 0) < 0):
                    raise WrongArgs("Invalid value of length")
            if (mfg_cmd == 'write'):
                if (os.path.exists(file_path(subargs[2])) == False):
                    raise WrongArgs("File " + subargs[2] + " does not exist")
        except ValueError as e:
            print "Invalid number as argument"
            cleanup()
            sys.exit()

        if (mfg_cmd == 'read'):
            line = 'read ' + subargs[0] + ' ' + subargs[1] + ' ' + subargs[2] + ' ' + file_path(subargs[3])
        elif (mfg_cmd == 'write'):
            line = 'write ' + subargs[0] + ' ' + subargs[1] + ' ' + file_path(subargs[2])
        elif (mfg_cmd == 'erase'):
            line = 'erase ' + subargs[0] + ' ' + subargs[1] + ' ' + subargs[2]
        elif (mfg_cmd == 'erase-all'):
            line = 'erase-all ' + subargs[0]

        with open (MFG_FILE, 'a') as mfile:
            mfile.write(line + '\n')
    except WrongArgs as e:
        print e
        cleanup()
        sys.exit()

def get_chip_id():
    print "Using OpenOCD interface file", IFC_FILE
    sys.stdout.flush()
    p = subprocess.Popen([OPENOCD, '-s', SCRIPT_DIR + '/interface', '-f', IFC_FILE, '-s', SCRIPT_DIR, '-f', 'openocd.cfg', '-c', ' init', '-c', 'chip_id', '-c', 'shutdown'], stdout=subprocess.PIPE)
    id = p.communicate()[0]
    sys.stderr.flush()
    if p.returncode:
        print "OpenOCD failed! Aborting..."
        cleanup()
        sys.exit()
    if (id == "unknown"):
        print "Unsupported Board! Aborting..."
        cleanup()
        sys.exit()
    return id

def run_flashprog(chip_id, interactive):
    READELF = which('arm-none-eabi-readelf')

    if not len(READELF):
        READELF = which('readelf')
        if not len(READELF):
            print "Error: readelf utility not found, please install binutils or appropriate package"
            cleanup()
            sys.exit()

    flashprog_axf = SCRIPT_DIR + '/' + chip_id + '/flashprog.axf'
    readelf_output = subprocess.Popen([READELF, flashprog_axf, '-h'], stdout=subprocess.PIPE)
    readelf_output = readelf_output.stdout.read()

    if ('Entry point' in readelf_output) is False:
        cleanup()
        sys.exit("Error: Not an ELF file")

    for item in readelf_output.split('\n'):
        if 'Entry point' in item:
            entry_point = item.strip()
            i = entry_point.index(':')
            entry_point = entry_point[i+1:].strip()

    print "Using OpenOCD interface file", IFC_FILE
    sys.stdout.flush()
    if (interactive):
        p = subprocess.Popen([OPENOCD, '-s', SCRIPT_DIR + '/interface', '-f', IFC_FILE, '-s', SCRIPT_DIR, '-f', 'openocd.cfg', '-c', 'init', '-c', 'sh_load ' + flashprog_axf + ' ' + entry_point])
        p.communicate()
    else:
        p = subprocess.Popen([OPENOCD, '-s', SCRIPT_DIR + '/interface', '-f', IFC_FILE, '-s', SCRIPT_DIR, '-f', 'openocd.cfg', '-c', 'init', '-c', 'sh_load ' + flashprog_axf + ' ' + entry_point], stdin=subprocess.PIPE)
        p.communicate(input = 'q')
    sys.stderr.flush()
    if p.returncode:
        print "OpenOCD failed! Aborting..."
    sys.stdout.flush()

def reset_board():
    msg = "Resetting board"
    print msg + "..."

    print "Using OpenOCD interface file", IFC_FILE
    sys.stdout.flush()
    p = subprocess.call([OPENOCD, '-s', SCRIPT_DIR + '/interface', '-f', IFC_FILE, '-s', SCRIPT_DIR, '-f', 'openocd.cfg', '-c', 'init', '-c', 'reset', '-c', 'shutdown'])
    sys.stderr.flush()
    if (p==0):
        print msg + " done..."
    else:
        print msg + " failed..."
    sys.stdout.flush()

def get_comp_longopts(args, non_comp_longopts):
    comp_longopts = []
    while args:
        if args[0] == '--':
            break
        if args[0][:2] == '--':
            possibilities = [o for o in non_comp_longopts if o.startswith(args[0][2:])]
            if not possibilities:
                comp_longopts.append(args[0][2:] + '=')
        args = args[1:]
    return comp_longopts

def main():
    global IFC_FILE
    global SCRIPT_DIR
    SCRIPT_DIR = file_path(SCRIPT_DIR)
    RESET_BOARD = 0
    interactive = 0
    get_openocd()
    non_comp_shortopts = "i:l:c:db:p:rh"
    non_comp_longopts = ["interface=","new-layout=","debug","batch=","read=","write=","erase=","erase-all=","prod=","reset","interactive","help"]
    comp_longopts = get_comp_longopts(sys.argv[1:], non_comp_longopts)
    try:
        opts, args = getopt.gnu_getopt(sys.argv[1:], non_comp_shortopts, non_comp_longopts + comp_longopts)
        if len(args):
            exit()
    except getopt.GetoptError as e:
        print e
        exit()

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            print_usage()
            sys.exit()

    cleanup()

    # Process short options first in the loop
    for opt, arg in opts:
        if opt in ("-i", "--interface"):
            IFC_FILE = arg + '.cfg'
        elif opt in ("-l", "--new-layout"):
            copy_file(arg, LAYOUT_FILE)
        elif opt in ("-d", "--debug"):
            copy_config_option('debug', 'flash')
        elif opt in ("-b", "--batch"):
            copy_file(arg, CONFIG_FILE)
        elif opt in ("-p", "--prod"):
            copy_file(arg, MFG_FILE)
        elif opt in ("-r", "--reset"):
            RESET_BOARD = 1
        elif opt in ("--interactive"):
            interactive = 1
        elif opt in ("--read"):
            copy_mfg_option('read', arg, 4)
        elif opt in ("--write"):
            copy_mfg_option('write', arg, 3)
        elif opt in ("--erase"):
            copy_mfg_option('erase', arg, 3)
        elif opt in ("--erase-all"):
            copy_mfg_option('erase-all', arg, 1)
        else:
            copy_config_option(opt[2:], arg)

    if ((os.path.exists(CONFIG_FILE)) or (os.path.exists(LAYOUT_FILE)) or (os.path.exists(MFG_FILE)) or interactive):
        chip_id = get_chip_id()
        run_flashprog(chip_id, interactive)
    elif (RESET_BOARD == 0):
        chip_id = get_chip_id()
        run_flashprog(chip_id, 1)

    cleanup()

    if (RESET_BOARD == 1):
        reset_board()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        cleanup()
