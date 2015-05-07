#!/bin/sh

updatefile=zeus.update.tar

for d in `ls /media`;do
	if [ -e /media/$d/$updatefile ];then
		cp /usr/zeus/update.log /media/$d/
		cp /usr/zeus/*.cfg /media/$d/
		cp /usr/zeus/ontom.db /media/$d/
		cp /tmp/zeus.log /media/$d/
		exit 0
	fi
done

exit 1
