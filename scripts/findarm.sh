#!/bin/bash

arm_eth0="1C:BA:8C:B6:69:E3"
arm_eth1="1C:BA:8C:B6:69:E5"

echo "will find out which ip binded with $arm_eth0"
echo "will find out which ip binded with $arm_eth1"

for ((i=2;i<235;i++));
	do 
		ping -W 1 -c 2 "192.168.1.$i" > /dev/null  & 
	done

printf "progressing"
for ((t=0;t<5;t++));
	do
		sleep 1
		printf "."
	done
echo -e ".\ndone."

arm_ip1=$(cat /proc/net/arp | grep -i $arm_eth0 | awk '{if ($3 != 0) print $1}')
arm_ip2=$(cat /proc/net/arp | grep -i $arm_eth1 | awk '{if ($3 != 0) print $1}')

if [ ${#arm_ip1} -gt 0 ]; then
	echo -e "ARM-dev board \033[31meth0\033[0m binded ip is: \033[31m$arm_ip1\033[0m";
fi

if [ ${#arm_ip2} -gt 0 ]; then
	echo -e "ARM-dev board \033[31meth1\033[0m binded ip is: \033[31m$arm_ip2\033[0m";
fi

if [ ${#arm_ip1} -eq 0 ] && [ ${#arm_ip2} -eq 0 ]; then
	echo -e "\033[31mNOT FOUND.\033[0m"
fi

echo "do clean up....."
pkill ping &2> /dev/null
echo "done."
exit
