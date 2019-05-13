#!/usr/bin/env python

import argparse
import sys
from secureboot import which

"""
This function will print input message and exit.
"""
def Exit(inMsg):
	print inMsg
	sys.exit(1)

"""
This function will populate toolchain version.
"""
def GettoolchainVersion(inVersionCommand):
	import subprocess
	p = subprocess.Popen(inVersionCommand.split(), stdout=subprocess.PIPE)
	toolchainVersion = p.communicate()[0].rstrip()
	return toolchainVersion

"""
This function will return absolute path of toolchain.
If the path comes out to be empty then exit with error.
"""
def customWhich(command):
	absCommand = which(command)
	if absCommand == "":
		Exit("Command: " + command + " does not exist")

	return absCommand

"""
This function will check
lto_state and ARM_GCC toolchain version
______________________
lto_state | required |
          | version  |
----------------------
n/<empty> | >= 4.9.3 |
----------------------
y         | 4.9.3    |
----------------------
"""

def ARM_GCCCheck(inOpts):
	toolchain = "arm-none-eabi-gcc"
	lto_state = inOpts.lto_state[0]
	versionCommand = " ".join([customWhich(toolchain), "-dumpversion"])
	"""
	Dots from the string are removed (using replace method) for comparision purpose
	"""
	toolchainVersion = GettoolchainVersion(versionCommand).replace(".", "")

	if ((lto_state == "n") or (lto_state == "")) and (toolchainVersion < "493"):
		str = ["Please use 4.9.3 or higher version", toolchain]
		Exit(" ".join(str))

	elif (lto_state == "y") and (toolchainVersion != "493"):
		str = ["Please use 4.9.3 only version", toolchain]
		Exit(" ".join(str))

def VersionCheck(inOpts):
	if inOpts.toolchain[0] == "arm_gcc":
		ARM_GCCCheck(inOpts)
	elif inOpts.toolchain[0] != "iar":
		Exit("Incorrect toolchain selected")

def ArchNameCheck(inOpts):
	if inOpts.arch_name[0] not in ['mc200', 'mw300']:
		Exit("Incorrect arch_name selected");

def ValueCheck(inValue, inValueName):
	if inValue is None:
		Exit("Please specify " + inValueName)

def OptsCheck(inOpts):
	ValueCheck(inOpts.toolchain, "toolchain")
	ValueCheck(inOpts.lto_state, "lto_state")
	ValueCheck(inOpts.arch_name, "arch_name")

def main():
	parser = argparse.ArgumentParser(description='This will check toolchain version')

	parser.add_argument("-V", "--version",
			action="version",
			version="%(prog)s v1.0")

	checker = parser.add_argument_group()

	checker.add_argument("-t", "--toolchain",
			action="store",
			nargs=1,
			dest="toolchain",
			help="Specify toolchain")

	checker.add_argument("-l", "--lto_state",
			action="store",
			nargs=1,
			dest="lto_state",
			help="Specify lto_state")

	checker.add_argument("-a", "--arch_name",
			action="store",
			nargs=1,
			dest="arch_name",
			help="Specify arch_name")

	opts, unknown = parser.parse_known_args()

	if len(sys.argv) == 1:
		parser.print_help()
		parser.exit(1)

	OptsCheck(opts)
	ArchNameCheck(opts)
	VersionCheck(opts)

if __name__ == '__main__':
	main()
