#!/bin/sh

result=`sqlite3-arm /usr/zeus/ontom.db "select config_value from configs where config_name='ontom_configured'"`
if [ $result == 'false' ];then
	echo "启动配置页面..."
	/usr/zeus/browser -qws http://127.0.0.1:8080/settings.html &
else
	echo "正在进入系统..."
	/usr/zeus/browser -qws http://127.0.0.1:8080/index.html &
fi

echo "地狱犬已启动，启动主程序...."

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
