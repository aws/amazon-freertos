#!/bin/bash

# Copyright (C) 2008-2015, Marvell International Ltd.
# All Rights Reserved.
#
# This script can be invoked to resolve the dependencies required for wmsdk
# application development.
#
#set -x

if [ `uname` != "Linux" ]; then
	echo "This script is meant for only Linux Platforms"
	echo "Exiting .... "
	exit 1
fi

connectivity=0
FILE="/tmp/tmpcon"
SCRIPT_DIR=`dirname $0`

fed=$(cat /etc/issue | grep -c "Fedora")
ubu=$(cat /etc/issue | grep -c "Ubuntu")
FED_PKGS="gcc-c++ glibc-static libusb libftdi ncurses-libs python texlive-latex openssl-devel ncurses-devel"
FED_64_PKGS="glibc.i686 libftdi.i686 libusb.i686 ncurses-libs.i686"
UBUNTU_PKGS="gcc libusb-dev libftdi-dev libncursesw5 python libssl-dev"
UBUNTU_64_PKGS="lib32ncursesw5 lib32z1 lib32ncurses5 libbz2-1.0:i386 libncurses5-dev lib32ncurses5-dev libncursesw5-dev"
VERSION=`uname -m`

check_connectivity()
{
	if [ $fed -gt 0 ]; then
		URL="www.fedoraproject.org"
	fi

	if [ $ubu -gt 0 ]; then
		URL="www.ubuntu.com"
	fi

	echo -n "Checking for internet connection..."
	wget $URL -O /dev/null -o $FILE
	if [ $(tail -5  $FILE | grep -c "saved") ]; then
		echo "ok."
		connectivity=1
	else
		echo "failed."
		connectivity=0
	fi
}

check_connectivity
if [ $fed -gt 0 ]; then
	if [ $connectivity == 1 ]; then
		echo ""
		echo "Installing packages, enter root password:"
		su - root -c "yum install $FED_PKGS"
		if [ $VERSION == "x86_64" ]; then
		    su - root -c "yum install $FED_64_PKGS"
		fi
	else
		echo "Cannot access the said package servers. Please install the following packages manually"
	        echo "$FED_PKGS"
	fi
fi

if [ $ubu -gt 0 ]; then
	if [ $connectivity == 1 ]; then
		sudo apt-get update
		sudo apt-get install $UBUNTU_PKGS
		if [ $VERSION == "x86_64" ]; then
		    sudo apt-get install $UBUNTU_64_PKGS
		fi
	else
		echo "Cannot access the said package servers. Please install the following packages manually:"
		echo "$UBUNTU_PKGS"
	fi
fi

rm -f $FILE

