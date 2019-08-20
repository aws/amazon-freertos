#! /usr/bin/env python
# Copyright (C) 2018 Marvell International Ltd.
# All Rights Reserved.

# Load application to ram helper script
# Note: sys.stdout.flush() and sys.stderr.flush() are required for proper
# console output in eclipse

import os, sys, platform, getopt, subprocess
from sys import platform as _platform

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

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

def print_usage():
    print ""
    print "Usage:"
    print sys.argv[0] + " <app.axf> [options]"
    print "Optional Usage:"
    print " [<-i | --interface> <JTAG hardware interface name>]"
    print "          Supported ones are ftdi, jlink, amontec, malink and stlink. Default is ftdi."
    print " [-s | --semihosting]"
    print "          Enable semihosting based console output"
    print " [-h | --help]"
    print "          Display usage"
    sys.stdout.flush()

def main():
    global SCRIPT_DIR
    SCRIPT_DIR = file_path(SCRIPT_DIR)
    SEMIHOST = False
    IFC_FILE = (os.getenv("DEBUG_INTERFACE", "ftdi") or "ftdi") + '.cfg'
    get_openocd()

    if len(sys.argv) <= 1:
        print_usage()
        sys.exit()

    try:
        opts, args = getopt.gnu_getopt(sys.argv[1:], "i:sh", ["interface=","semihosting","help"])
        if (len(args) != 1):
            print_usage()
            sys.exit()
    except getopt.GetoptError as e:
        print e
        print_usage()
        sys.exit()

    for opt, arg in opts:
        if opt in ("-i", "--interface"):
            IFC_FILE = arg + '.cfg'
        elif opt in ("-s", "--semihosting"):
            SEMIHOST = True
        elif opt in ("-h", "--help"):
            print_usage()
            sys.exit()

    FILE = file_path(args[0])

    if (os.path.isfile(FILE) == False):
        print "Error: Could not find file", FILE
        print_usage()
        sys.exit()

    READELF = which('arm-none-eabi-readelf')

    if not len(READELF):
        READELF = which('readelf')
        if not len(READELF):
            print "Error: readelf utility not found, please install binutils or appropriate package"
            sys.exit()

    readelf_output = subprocess.Popen([READELF, FILE, '-h'], stdout=subprocess.PIPE)
    readelf_output = readelf_output.stdout.read()

    if ('Entry point' in readelf_output) is False:
        sys.exit("Error: Not an ELF file")

    for item in readelf_output.split('\n'):
        if 'Entry point' in item:
            entry_point = item.strip()
            i = entry_point.index(':')
            entry_point = entry_point[i+1:].strip()

    if ((int(entry_point, 16) & 0x1f000000) == 0x1f000000):
        print "Error: Provided firmware image (.axf) is XIP enabled and hence ramload is not possible, please use flashprog to flash the image (.bin)"
        sys.exit()

    NM = which('arm-none-eabi-nm')

    if not len(NM):
        NM = which('nm')
        if not len(NM):
            print "Error: nm utility not found, please install binutils or appropriate package"
            sys.exit()

    nm_output = subprocess.Popen([NM, FILE], stdout=subprocess.PIPE)
    nm_output = nm_output.stdout.read()

    for item in nm_output.split('\n'):
        if '_ol_ovl' in item:
            print "Error: Provided firmware image (.axf) is overlays enabled and hence ramload is not possible, please use flashprog to flash the image."
            sys.exit()

    print "Using OpenOCD interface file", IFC_FILE
    sys.stdout.flush()

    if SEMIHOST is False:
        subprocess.call ([OPENOCD, '-s', SCRIPT_DIR + '/interface', '-f', IFC_FILE, '-s', SCRIPT_DIR, '-f','openocd.cfg', '-c', 'init', '-c', 'load ' + FILE + ' ' + str(entry_point) + ' ', '-c', 'shutdown'])
    else:
        subprocess.call ([OPENOCD, '-s', SCRIPT_DIR + '/interface', '-f', IFC_FILE, '-s', SCRIPT_DIR, '-f','openocd.cfg', '-c', 'init', '-c', 'sh_load ' + FILE + ' ' + str(entry_point)])
    sys.stderr.flush()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
