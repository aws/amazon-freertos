#!/bin/sh

scp * pi@192.168.1.4: 
ssh -t -t 192.168.1.4 -l pi << 'ENDSSH'
sudo rm -rf "/var/lib/bluetooth/*"
sudo hciconfig hci0 reset
python test1.py
sleep 1
ENDSSH

