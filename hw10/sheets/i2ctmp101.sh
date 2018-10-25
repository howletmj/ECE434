#!/bin/bash
for i in 1 2 3 4 5
do
	temp=`i2cget -y 1 0x48`
	temp2=$(( $temp * 9 / 5 + 32 ))
	./demo.py $temp2 0
	sleep 2
done
