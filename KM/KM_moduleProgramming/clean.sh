#!/bin/sh
for a in $(ls -d ~/Desktop/LinuxDeviceDriver/moduleProgramming/*/)
do
	sudo make -C $a clean
done
