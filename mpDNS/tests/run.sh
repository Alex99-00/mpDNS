#!/bin/bash
#
#
# "run.sh 10" -> 10 times the test
#

echo " " > run.out

i=0
while ( [ $i -ne $1 ] ) do
        dig @127.0.0.1 -p5300 nathan.ax A 1>>run.out &
        dig @127.0.0.1 -p5300 nathan.ax NS 1>>run.out &
        dig @127.0.0.1 -p5300 xx.nathan.ax CNAME 1>>run.out &
        dig @127.0.0.1 -p5300 -x 192.168.10.4 1>>run.out &
        dig @127.0.0.1 -p5300 nathan.ax SOA 1>>run.out &
        dig @127.0.0.1 -p5300 nathan.ax MX 1>>run.out &
        i=$[$i+1]
        echo "$i"
done
