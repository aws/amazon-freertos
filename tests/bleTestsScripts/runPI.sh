#!/bin/sh

sshpass -p "raspberry" scp * pi@10.49.106.35: 
sshpass -p "raspberry" ssh -t -t 10.49.106.35 -l pi << 'ENDSSH'
sudo rm -rf "/var/lib/bluetooth/*"
sudo hciconfig hci0 reset
sudo python test1.py
sleep 1
ENDSSH

