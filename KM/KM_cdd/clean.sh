#!/bin/sh
for a in $(find * -type d)
do
	#echo $a
	sudo make -C $a clean
done
