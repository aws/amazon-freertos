#!/bin/sh

# clean.sh: Remove all "unnecessary" files.

IPERF_ROOT_DIR=$(readlink -f "`dirname "$0"`")

DIR_TO_CLEAN=$IPERF_ROOT_DIR/autom4te.cache
echo "Cleaning $DIR_TO_CLEAN."
rm -rfv $DIR_TO_CLEAN

DIR_TO_CLEAN=$IPERF_ROOT_DIR/autom4te.cache
echo "Cleaning $DIR_TO_CLEAN."
rm -rfv compat/.deps/ compat/Makefile compat/*.o compat/*.a

DIR_TO_CLEAN=$IPERF_ROOT_DIR/doc
echo "Cleaning $DIR_TO_CLEAN."
rm -rfv $DIR_TO_CLEAN/Makefile

DIR_TO_CLEAN=$IPERF_ROOT_DIR/include
echo "Cleaning $DIR_TO_CLEAN."
rm -rfv $DIR_TO_CLEAN/iperf-int.h $DIR_TO_CLEAN/Makefile

DIR_TO_CLEAN=$IPERF_ROOT_DIR/man
echo "Cleaning $DIR_TO_CLEAN."
rm -rfv $DIR_TO_CLEAN/Makefile

DIR_TO_CLEAN=$IPERF_ROOT_DIR/src
echo "Cleaning $DIR_TO_CLEAN."
rm -rfv $DIR_TO_CLEAN/.deps/ $DIR_TO_CLEAN/Makefile $DIR_TO_CLEAN/*.o $DIR_TO_CLEAN/iperf $DIR_TO_CLEAN/iperf.exe

DIR_TO_CLEAN=$IPERF_ROOT_DIR/wiced
echo "Cleaning $DIR_TO_CLEAN."
rm -rf $DIR_TO_CLEAN/Makefile $DIR_TO_CLEAN/*.o $DIR_TO_CLEAN/*.a

DIR_TO_CLEAN=$IPERF_ROOT_DIR
echo "Cleaning $DIR_TO_CLEAN."
rm -rf $DIR_TO_CLEAN/config.h $DIR_TO_CLEAN/config.h.in~ $DIR_TO_CLEAN/config.log $DIR_TO_CLEAN/config.status $DIR_TO_CLEAN/Makefile $DIR_TO_CLEAN/stamp-h1

echo "Done!"
