#!/bin/bash

shopt -s nullglob

while true
do
    ./ArduinoSerialDatagramHub.py /dev/ttyACM* /dev/ttyUSB* -s 9600
    echo
    for i in $(seq 9 -1 1)
    do
	read -p "Starting Python script in $i seconds. Press <p> to pause or <n> to start now " -t 1 -n 1
	echo
	if [ "$REPLY" = "p" ] || [ "$REPLY" = "P" ]
	then
	    read -p "Press enter to start"
	    break
	fi
	if [ "$REPLY" = "n" ] || [ "$REPLY" = "N" ]
	then
	    break
	fi
    done
done
