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
    print sys.argv[0]
    print "Optional Usage:"
    print " [<-i | --interface> <JTAG hardware interface name>]"
    print "          Supported ones are ftdi, jlink, amontec, malink and stlink. Default is ftdi."
    print " [-t | --tcp]"
    print "          Start in TCP/IP mode. Default is pipe mode."
    print " [-h | --help]"
    print "          Display usage"
    sys.stdout.flush()

def main():
    global SCRIPT_DIR
    SCRIPT_DIR = file_path(SCRIPT_DIR)
    IFC_FILE = (os.getenv("DEBUG_INTERFACE", "ftdi") or "ftdi") + '.cfg'
    TCPIP_MODE = 0
    get_openocd()
    try:
        opts, args = getopt.gnu_getopt(sys.argv[1:], "i:th", ["interface=","tcp","help"])
        if len(args):
            print_usage()
            sys.exit()
    except getopt.GetoptError as e:
        print e
        print_usage()
        sys.exit()

    for opt, arg in opts:
        if opt in ("-i", "--interface"):
            IFC_FILE = arg + '.cfg'
        elif opt in ("-t", "--tcp"):
            TCPIP_MODE = 1
        elif opt in ("-h", "--help"):
            print_usage()
            sys.exit()

    print "Using OpenOCD interface file", IFC_FILE
    sys.stdout.flush()
    if (TCPIP_MODE == 1):
        subprocess.call ([OPENOCD, '-s', SCRIPT_DIR + '/interface', '-f', IFC_FILE, '-s', SCRIPT_DIR, '-f','openocd.cfg'])
    else:
        subprocess.call ([OPENOCD, '-s', SCRIPT_DIR + '/interface', '-f', IFC_FILE, '-s', SCRIPT_DIR, '-f','openocd.cfg', '-c', 'gdb_port pipe; log_output openocd.log'])
    sys.stderr.flush()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
