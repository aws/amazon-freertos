#!/bin/sh

scp * root@10.60.172.116:
ssh -t -t 10.60.172.116 -l root << 'ENDSSH'
rm -rf "/var/lib/bluetooth/*"
hciconfig hci0 reset
python test1.py
sleep 1
ENDSSH

