#!/bin/bash
temp=`i2cget -y 2 0x48`
echo "$(( $temp * 9 / 5 + 32 )) ˚F";
temp=`i2cget -y 2 0x49`
echo "$(( $temp * 9 / 5 + 32 )) ˚F";

