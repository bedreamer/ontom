#!/bin/sh

echo "地狱犬已启动，启动程序 $*"

while true
	do
		ps | grep 'S \+zeus' > /dev/null
		if [ $? == 0 ]; then
			sleep 1;
		else
			/usr/zeus/zeus
			echo "主程序已退出..."
		fi
	done
