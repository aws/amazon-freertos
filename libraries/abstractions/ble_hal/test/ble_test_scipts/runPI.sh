#!/bin/sh

scp * root@192.168.1.3:
ssh -t -t 192.168.1.3 -l root << 'ENDSSH'
rm -rf "/var/lib/bluetooth/*"
hciconfig hci0 reset
python test1.py
sleep 1
ENDSSH

