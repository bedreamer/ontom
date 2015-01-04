#!/bin/sh

host_eth0="08:00:27:8c:93:17"

echo "will find out which ip binded with $host_eth0"

for i in `seq 2 1 240`;
        do 
                ping -W 1 -c 2 "192.168.1.$i" > /dev/null  & 
        done

printf "progressing"
for t in 1 2 3 4 5;
        do
                sleep 1
                printf "."
        done
echo -e ".\ndone."

host_ip1=$(cat /proc/net/arp | grep -i $host_eth0 | awk '{if ($3 != 0) print $1}')

if [ ${#host_ip1} -gt 0 ]; then
        echo -e "HOST's \033[31meth0\033[0m binded ip is: \033[31m$host_ip1\033[0m";
fi

if [ ${#host_ip1} -eq 0 ]; then
        echo -e "\033[31mNOT FOUND.\033[0m"
fi

echo "do clean up....."
ps | grep ping | awk '{print $1}' | xargs kill -9 >/dev/null 2>&1

echo "attempt to automount NFS from $host_ip1 to $HOME/srv/..."
mount -t nfs -o proto=tcp,port=2049,nolock $host_ip1:/home/tom/workspace srv/ >/dev/null 2>&1

mount_result=$(mount | grep -i $host_ip1)

if [ ${#mount_result} -gt 0 ]; then
        echo "automount done."
else
        echo "automount failed!"
fi

exit
