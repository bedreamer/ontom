#!/bin/sh

updatefile=zeus.update.tar

if [ $# -eq 1 ]; then
	if [ -e "/tmp/zeus.version" ];then
		rm /tmp/zeus.version
	fi
	wget "http://$1/www/VERSION" -O /tmp/zeus.version >/dev/null 2>&1
	if [ -e "/tmp/zeus.version" ];then
		if [ -e "/tmp/zeus.update.tar" ];then
			rm /tmp/zeus.update.tar
		fi
		wget "http://$1/www/"`cat /tmp/zeus.version`"/zeus.update.tar" -O /tmp/zeus.update.tar > /dev/null 2>&1
		if [ -e "/tmp/zeus.version" ];then
			tar -C / -xvf /tmp/zeus.update.tar > /usr/zeus/update.log
			echo "Upgrade success version "`cat /tmp/zeus.version`
			mv /tmp/zeus.version /usr/zeus/zeus.version
		else
			echo "fetch zeus.update.tar from $1 failed!"
		fi
	else
		echo "Upgrade failed!"
	fi
else
	for d in `ls /media`;do
		if [ -e /media/$d/$updatefile ];then
			tar -C / -xvf /media/$d/$updatefile > /usr/zeus/update.log
			echo "Upgrade success."
			exit 0
		fi
	done
fi

exit 1
