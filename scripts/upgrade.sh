#!/bin/sh

updatefile=zeus.update.tar


for d in `ls /media`;do
	if [ -e /media/$d/$updatefile ];then
		tar -C / -xvf /media/$d/$updatefile > /usr/zeus/update.log
		exit 0
	fi
done

exit 1
