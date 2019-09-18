#! /bin/bash
# Amazon FreeRTOS V1.2.3
# Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# http://aws.amazon.com/freertos
# http://www.FreeRTOS.org



# This is a shell script to secure copy the GPIO_RP3 to RP3

IP=$1
shift
LOGINID=$1
shift
PASSWD=$1
shift
if [ "$1" == "-p" ]; then
    #Secure copy the test_iot_gpio_rp3.py from Host ubuntu to RP3
    sshpass -p ${PASSWD} ssh ${LOGINID}@${IP} "mkdir -p /home/pi/Tests"
    sshpass -p ${PASSWD} scp ./test_iot_gpio_rp3.py ${LOGINID}@${IP}:/home/pi/Tests/
elif [ "$1" == "-w" ]; then

    #Open a SSH connection to RP3 and run the test_iot_gpio_rp3.py to read the GPIO pin status
    sshpass -p ${PASSWD} ssh ${LOGINID}@${IP} "python /home/pi/Tests/test_iot_gpio_rp3.py -i $2 > /home/pi/Tests/gpio_output_RP3.txt"

    #Copy the GPIO pin status to host ubuntu for validation
    sshpass -p ${PASSWD} scp ${LOGINID}@${IP}:/home/pi/Tests/gpio_output_RP3.txt .

elif [ "$1" == "-r" ]; then
    #Open a SSH connection to RP3 and run the test_iot_gpio_rp3.py to set the GPIO pin output
    sshpass -p ${PASSWD} ssh ${LOGINID}@${IP} "python /home/pi/Tests/test_iot_gpio_rp3.py -o $2"

elif [ "$1" == "-c" ]; then
    sshpass -p ${PASSWD} ssh ${LOGINID}@${IP} "rm /home/pi/Tests/gpio_output_RP3.txt"
fi

exit 0
