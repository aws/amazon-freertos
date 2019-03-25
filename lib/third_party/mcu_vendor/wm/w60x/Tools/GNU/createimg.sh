#!/bin/sh  

#cp W600.bin ../../Bin/WM_VENUS.bin
#cd ../../Tools
#
#cp ../Bin/version.txt ../Bin/version_bk.txt
#
#./makeimg "../Bin/WM_VENUS.bin" "../Bin/WM_VENUS.img" 0 0 "../Bin/version.txt" E000
#
#./makeimg "../Bin/WM_VENUS.bin" "../Bin/WM_VENUS_SEC.img" 0 0 "../Bin/version.txt" 7E800
#./makeimg_all "../Bin/secboot.img" "../Bin/WM_VENUS.img" "../Bin/WM_VENUS.FLS"
#rm -f "../Bin/WM_VENUS.img"

SOBJ=W600
TARGET=W600
_ENV_TARGET=""
_REVI_TARGET=""

MKIMG=makeimg
MKIMG_ALL=makeimg_all
MKIMG_DBG=makeimg_dbg
LOGGER=logger

OS=`uname -o`
[ "${OS}" = "Msys" -o "${OS}" = "Cygwin" ] && {
	MKIMG=makeimg.exe
	MKIMG_ALL=makeimg_all.exe
    MKIMG_DBG=makeimg_dbg.exe
	LOGGER=echo
}

SHELL_DO_CMD_FLAG=1

shell_do_cmd()
{
	local retval=0
	#echo "Execute -- $@"
	[ 1 = ${SHELL_DO_CMD_FLAG} ] && {
		$@
	} || {
		$@ 0>/dev/null 1>/dev/null 2>/dev/null
	}
	retval=$?
	[ 0 != $retval ] && {
		${LOGGER} "Execute $@"
		echo -e "Execute \033[31mFAILURE\033[0m: $@"
		exit $retval
	}
}


main()
{
	shell_do_cmd cp ${SOBJ}.map ../../Bin
	shell_do_cmd cp ${SOBJ}.bin ../../Bin
	shell_do_cmd cd ../../Tools
	
	shell_do_cmd cp ../Bin/version.txt ../Bin/version_bk.txt
	cp ../Bin/${SOBJ}.bin ../Bin/${SOBJ}.bin.bk
	shell_do_cmd gzip -fv "../Bin/${SOBJ}.bin"
	mv ../Bin/${SOBJ}.bin.bk ../Bin/${SOBJ}.bin
	shell_do_cmd ./${MKIMG} "../Bin/${SOBJ}.bin" "../Bin/${TARGET}.img" 0 0 "../Bin/version.txt" 90000 10100
	shell_do_cmd ./${MKIMG} "../Bin/${SOBJ}.bin.gz" "../Bin/${TARGET}_GZ.img" 0 1 "../Bin/version.txt" 90000 10100 "../Bin/${SOBJ}.bin" 
	shell_do_cmd ./${MKIMG} "../Bin/${SOBJ}.bin" "../Bin/${TARGET}_SEC.img" 0 0 "../Bin/version.txt" 90000 10100
    shell_do_cmd ./${MKIMG_DBG} "../Bin/${TARGET}_SEC.img" "../Bin/${TARGET}_DBG.img"
	 ./${MKIMG_ALL} "../Bin/secboot.img" "../Bin/${TARGET}.img" "../Bin/${TARGET}.FLS"
	#rm -rf "../Bin/${TARGET}.img"
	#rm -rf "../Bin/${SOBJ}.bin.gz"
}

usage()
{
	echo -ne "\n$0 [-e ENV | -r RVERSION | -h]\n\n"
	echo -ne "\t-e ENV   Compiler Environment, default is NOT SETTED\n"
	echo -ne "\t-h       This help information\n"
	echo -ne "\t-r REVISION    SVN-Reversion number, default is NOT SETTED\n"
	echo -ne "\n"
}

while getopts ":e:hr:" opt
do
	case ${opt} in
		e)
			_ENV_TARGET="${OPTARG}-"
			;;
		h)
			usage
			exit 0
			;;
		r)
			_REVI_TARGET="-${OPTARG}"
			;;
	esac
done

TARGET=${_ENV_TARGET}${TARGET}${_REVI_TARGET}
main
