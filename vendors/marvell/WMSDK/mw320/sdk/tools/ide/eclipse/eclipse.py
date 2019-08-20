#! /usr/bin/env python
# Copyright (C) 2018 Marvell International Ltd.
# All Rights Reserved.

# Eclipse wrapper/helper script to modify eclipse project files.
import os, sys, getopt, re

interfaces = ['ftdi', 'jlink', 'amontec', 'malink']
debug_if = ''
layout_file = ''
new_pr_name = ''
work_dir = ''

eclipse_files = ['.project', '.cproject']

def print_usage():
    print ""
    print "Usage:"
    print sys.argv[0] + " [options]"
    print "Optional Usage:"
    print " [-C | --directory] <path/to/dir>"
    print "          Change current working directory to <path/to/dir>"
    print " [<-i | --interface> <JTAG hardware interface name>]"
    print "          Supported ones are ftdi, jlink, amontec, malink. Default is ftdi."
    print " [<-L | --Layout> </path/to/layout_file>]"
    print "          Layout file <layout_file> to use for flashing"
    print " [<-p | --project> <New project name>]"
    print "          Sets the project name to new value in eclipse project files."
    print "          Hint: Names of the format sdkname_bundle[-x.y.z] guarantees correct operation"
    print " [-h | --help]"
    print "          Display usage"
    sys.stdout.flush()

def sed_inplace(file, pattern, repl):
    with open(file, "r") as sources:
        lines = sources.readlines()
    with open(file, "w") as sources:
        for line in lines:
            sources.write(re.sub(pattern, repl, line))

def get_project_name():
    with open(".project", "r") as sources:
        for line in sources:
            if line.strip().startswith("<projectDescription>"):
                for line in sources:
                    if line.strip().startswith("<name>"):
                        return re.split("<name>|</name>", line.strip())[1]
    return ''

if len(sys.argv) <= 1:
    print_usage()
    sys.exit()

try:
    opts, args = getopt.gnu_getopt(sys.argv[1:], "C:i:L:p:h", ["directory=","interface=","Layout=","project=","help"])
    if len(args):
        print_usage()
        sys.exit()
except getopt.GetoptError as e:
    print e
    print_usage()
    sys.exit()

for opt, arg in opts:
    if opt in ("-C", "--directory"):
        work_dir = arg
    elif opt in ("-i", "--interface"):
        if arg in interfaces:
            debug_if = arg
        else:
            print "Debug interface " + arg + " not supported"
            sys.exit()
    elif opt in ("-L", "--Layout"):
        layout_file = arg
    elif opt in ("-p", "--project"):
        new_pr_name = arg
    elif opt in ("-h", "--help"):
        print_usage()
        sys.exit()

if len(work_dir):
    if (os.path.exists(work_dir) == False):
        print "Please provide a valid working directory."
        sys.exit()
    else:
        os.chdir(work_dir)

if (os.path.exists(".project") == False) or (os.path.exists(".cproject") == False):
    print (".project or .cproject file is not present. Please run the script from top-level directory.")
    sys.exit()

try:
    for dirpath, dirnames, filenames in os.walk(os.getcwd(), followlinks=True):
        for fname in filenames:
            if fname.endswith('.launch'):
                eclipse_files += [os.path.join(dirpath, fname)]
except:
    pass

if len(debug_if):
    dbg_str = "<mapEntry key=\"DEBUG_INTERFACE\" value=\".*\"/>"
    new_dbg_str = "<mapEntry key=\"DEBUG_INTERFACE\" value=\"" + debug_if + "\"/>"

    for file in eclipse_files:
        sed_inplace(file, dbg_str, new_dbg_str)

    dbg_str = "DEBUG_INTERFACE/value=.*"
    new_dbg_str = "DEBUG_INTERFACE/value=" + debug_if
    if os.path.exists(".settings/org.eclipse.cdt.core.prefs"):
        sed_inplace(".settings/org.eclipse.cdt.core.prefs", dbg_str, new_dbg_str)

    print "Debug interface changed successfully. Please restart the eclipse IDE."

if len(layout_file):
    layout_str = "<mapEntry key=\"LAYOUT_FILE\" value=\".*\"/>"
    new_layout_str = "<mapEntry key=\"LAYOUT_FILE\" value=\"" + layout_file + "\"/>"

    for file in eclipse_files:
        sed_inplace(file, layout_str, new_layout_str)

    layout_str = "LAYOUT_FILE/value=.*"
    new_layout_str = "LAYOUT_FILE/value=" + layout_file
    if os.path.exists(".settings/org.eclipse.cdt.core.prefs"):
        sed_inplace(".settings/org.eclipse.cdt.core.prefs", layout_str, new_layout_str)

    print "Layout file changed successfully. Please restart the eclipse IDE."

if len(new_pr_name):
    cur_pr_name = get_project_name()

    if len(cur_pr_name):
        for file in eclipse_files:
            sed_inplace(file, cur_pr_name, new_pr_name)
        print "Project name set successfully."
    else:
        print "Current project name could not found. Project name set failed."
