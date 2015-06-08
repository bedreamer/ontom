#!/bin/sh

if [ ! -d /tmp/hellhound ] && [ ! -e /tmp/hellhound ];then 
	mkdir /tmp/hellhound
fi

RP=`which $1`
MD5=`md5sum $RP|awk '{print $1}'`

if [ -e "/tmp/hellhound/"$MD5"_pid" ];then
	if [ ! -e "/proc/`cat "/tmp/hellhound/"$MD5"_pid"`" ];then
		echo "地狱犬嗅探到无效进程, 自动清理."
		rm "/tmp/hellhound/"$MD5"_pid"
	else
		echo "程序正常, PID=`cat "/tmp/hellhound/"$MD5"_pid"`"
		exit 1;
	fi
fi

if [ -e "`which $1`" ] && [ -x "`which $1`" ];then
	while [ true ];do
		if [ ! -e "/tmp/hellhound/"$MD5"_pid" ];then
			$* &
			PID=$!
			echo $PID > "/tmp/hellhound/"$MD5"_pid"
			echo "地狱犬已启动，看守进程 <PID=$PID;$*>"
		else
			if [ ! -e "/proc/$PID" ];then
				echo "地狱犬嗅探到<$1>退出，1秒后重启..."
				rm "/tmp/hellhound/"$MD5"_pid"
			else
				sleep 1
				continue
			fi
		fi
		sleep 1
	done
else
	exit 1;
fi

exit 1
