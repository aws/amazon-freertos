#!/bin/bash

# Copyright (C) 2008-2018, Marvell International Ltd.
# All Rights Reserved.
#
# This script can be invoked to add linux user to groups 'plugdev'
# and 'dialout', and add udev rules for OpenOCD. After this step,
# sudo is not required for invoking OpenOCD or serial console.
# For eclipse and arm-none-eabi-gdb users, this step is mandatory.
#
#set -x

if [ `uname` != "Linux" ]; then
	echo "This script is meant for only Linux Platforms"
	echo "Exiting .... "
	exit 1
fi

SCRIPT_DIR=`dirname $0`
USER_NAME=$USER

add_user_groups()
{
	echo ""
	echo "* To use OpenOCD and serial console without root privileges, user '$USER_NAME' needs"
	echo "* to be added to groups 'plugdev' and 'dialout'. After this, user needs to logout"
	echo "* and login once for settings to take effect."
	echo "** For eclipse and arm-none-eabi-gdb users, this step is mandatory."
	read -p "> Add user '$USER_NAME' to groups 'plugdev' and 'dialout' [y/N]? " OK
	if [ "$OK" == 'y' ] || [ "$OK" == 'Y' ]; then
		if [ ! -e /etc/udev/rules.d/99-openocd.rules ]; then
			sudo cp $SCRIPT_DIR/99-openocd.rules /etc/udev/rules.d/
			sudo udevadm control --reload-rules
			sudo udevadm trigger
		fi
		getent group plugdev > /dev/null || sudo groupadd plugdev
		sudo usermod -a -G dialout,plugdev $USER_NAME
		read -p "> Please logout and login again after script is finished. OK? " OK
	else
		read -p "> Please use sudo to invoke OpenOCD and serial console. OK? " OK
	fi
	echo ""
}

add_user_groups
