#!/bin/bash 
#get network interface ip
flag=0
echo $1
while true; do 
#ip=`ifconfig | grep -A 1 $1 | tail -1 | cut -d ':' -f 2 | cut -d ' ' -f 1`
ip=`ifconfig eth0 | grep 'inet addr:' | cut -d: -f2 | awk '{print $1}'`
#mask=`ifconfig | grep -A 1 $1 | tail -1 | cut -d ':' -f 4`
gw=`route -n | grep 'UG[ \t]' | awk '{print $2}'`
echo "ip=$ip" > /dev/ttyAMA0
echo "netmask=$mask" > /dev/ttyAMA0
echo "gateway=$gw" > /dev/ttyAMA0
#mask="255.255.255.0"
#res="${mask//[^255]}"

#subnet_oct=`grep -o '255' <<<"$res" | grep -c .`
#let subnet_rev=5-subnet_oct
#echo $subnet_oct

#gw=`echo $ip | rev | cut -f$subnet_rev- -d "." | rev`
#case "$subnet_oct" in
#1)	gw+=".1.1.1"
#	;;
#2)	gw+=".1.1"
#	;;
#3)	gw+=".1"
#esac
#echo $gw
# -q quiet
# -c nb of pings to perform
ping -c 2 $gw > /dev/null

if [ $? -eq 0 ]
then
	echo "network is ok" > /dev/ttyAMA0
	flag=1
fi
if [ $flag -eq 1 ]
then
	./home/pi/Smart_Classroom/Media_Hub/src/MediaHub &
	wait 
	echo "started MediaHub" > /dev/ttyAMA0
	killall MediaHub
	flag=0
fi
done

