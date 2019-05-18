#!/bin/sh

scp * root@192.168.1.4: 
ssh -t -t 192.168.1.4 -l root << 'ENDSSH'
rm -rf "/var/lib/bluetooth/*"
hciconfig hci0 reset
python test1.py
sleep 1
ENDSSH

