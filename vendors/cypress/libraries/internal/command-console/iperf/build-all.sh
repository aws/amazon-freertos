#!/bin/sh

#======================================================
# Joshua Spence
# 23 February 2012
# build-all.sh: Builds all different variants of iperf.
#======================================================

# Check working directory
ROOT_DIR=$(readlink -f "`dirname "$0"`")
if [ $ROOT_DIR != $(pwd) ]; then
    echo "You must run this script from $ROOT_DIR" >&2
    exit 1
fi

#======================================================
# Configuration
#======================================================
BUILD_DIR=$ROOT_DIR/build
SRC_DIR=$ROOT_DIR/src
SRC_EXE_NAME=iperf
#======================================================

# Arguments
VERBOSE=            # verbose output?
REDIR=">/dev/null"  # redirect output?

# Perform a clean
echo "Cleaning..."
[ -n "$REDIR" ] && ./clean.sh >/dev/null || ./clean.sh
[ -n "$REDIR" ] && rm --force --recursive $VERBOSE $BUILD_DIR >/dev/null || rm --force --recursive $VERBOSE $BUILD_DIR
[ -n "$VERBOSE" ] && echo

echo "Making build direcory"
[ -n "$REDIR" ] && mkdir --parents $VERBOSE $BUILD_DIR >/dev/null || mkdir --parents $VERBOSE $BUILD_DIR
[ -n "$VERBOSE" ] && echo

do_build() {
    CONFIGURE_ARGS=$1
    DST_EXE_NAME=$2
    
    ./configure $CONFIGURE_ARGS
    if [ $? -ne 0 ]; then
        echo "./configure failed!" >&2
        exit 1    
    fi
    
    EXEEXT=$(sed -n 's/EXEEXT\s*=\s\(.*\)$/\1/p' $ROOT_DIR/Makefile)
    
    make clean
    if [ $? -ne 0 ]; then
        echo "make clean failed!" >&2
        exit 1    
    fi
    make
    if [ $? -ne 0 ]; then
        echo "make failed!" >&2
        exit 1    
    fi
    mv $SRC_DIR/$SRC_EXE_NAME$EXEEXT $BUILD_DIR/$DST_EXE_NAME$EXEEXT
    if [ $? -ne 0 ]; then
        echo "mv failed!" >&2
        exit 1    
    fi
}

echo "Building configuration: standard"
[ -n "$REDIR" ] && do_build "" "iperf" >/dev/null || do_build "" "iperf"
[ -n "$VERBOSE" ] && echo

echo "Building configuration: debug"
[ -n "$REDIR" ] && do_build "--enable-debug --enable-iperf-debug" "iperf-debug" >/dev/null || do_build "--enable-debug --enable-iperf-debug" "iperf-debug"
[ -n "$VERBOSE" ] && echo

echo "Building configuration: singlethreaded"
[ -n "$REDIR" ] && do_build "--disable-threads" "iperf-singlethreaded" >/dev/null || do_build "--disable-threads" "iperf-singlethreaded"
[ -n "$VERBOSE" ] && echo

echo "Building configuration: singlethreaded-debug"
[ -n "$REDIR" ] && do_build "--disable-threads --enable-debug --enable-iperf-debug" "iperf-singlethreaded-debug" >/dev/null || do_build "--disable-threads --enable-debug --enable-iperf-debug" "iperf-singlethreaded-debug"
[ -n "$VERBOSE" ] && echo

echo "Cleaning..."
[ -n "$REDIR" ] && ./clean.sh >/dev/null || ./clean.sh
[ -n "$VERBOSE" ] && echo

echo "SUCCESS!"
exit 0
