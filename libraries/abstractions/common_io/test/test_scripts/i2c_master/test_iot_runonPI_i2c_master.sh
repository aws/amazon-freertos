#!/usr/bin/env bash

# FreeRTOS Common IO V0.1.2
# Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# http://aws.amazon.com/freertos
# http://www.FreeRTOS.org


DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
IP=$1
shift
LOGINID=$1
shift
PASSWD=$1
shift

if [ "$1" == "-p" ]; then
    #Secure copy the test_iot_adc_rp3.py from Host to RP3
    sshpass -p "$PASSWD" ssh "$LOGINID"@"$IP" "mkdir -p /home/pi/Tests"
    sshpass -p "$PASSWD" scp ${DIR}/test_iot_i2c_master_rp3.py "$LOGINID"@"$IP":/home/pi/Tests
elif [ "$1" == "-s" ]; then
    #Run demon
    sshpass -p ${PASSWD} ssh ${LOGINID}@${IP} 'sudo pigpiod -s 1 -b 200'
    sleep .1
    #Run rpi script
    sshpass -p "$PASSWD" ssh "$LOGINID"@"$IP" "python /home/pi/Tests/test_iot_i2c_master_rp3.py"
    #Kill demon
    sshpass -p ${PASSWD} ssh ${LOGINID}@${IP} "sudo killall pigpiod"
elif [ "$1" == "-c" ]; then
    sshpass -p "$PASSWD" ssh "$LOGINID"@"$IP" "rm /home/pi/Tests/i2c_master_res.txt"
fi