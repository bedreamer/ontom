#!/bin/sh

updatefile=zeus.update.tar

if [ $# -eq 1 ]; then
	wget $1 -O /tmp/zeus.update.tar
	tar -C / -xvf /tmp/zeus.update.tar > /usr/zeus/update.log
else
	for d in `ls /media`;do
		if [ -e /media/$d/$updatefile ];then
			tar -C / -xvf /media/$d/$updatefile > /usr/zeus/update.log
			exit 0
		fi
	done
fi

exit 1
