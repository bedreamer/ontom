#!/bin/sh

echo "监控系统正在启动..."
echo "执行数据库预配置..."
/usr/zeus/preconfig.sh /usr/zeus/ontom.db
echo "开始启动主程序..."

while [ true ]; do
	/usr/zeus/zeus
	echo "检测到程序主动退出, 1秒后自动重启..."
	sleep 1;
	echo "重新预配置数据库"
	/usr/zeus/preconfig.sh /usr/zeus/ontom.db
	echo "正在重启...."
	kill -9 `pidof browser`
	mode=`sqlite3 ontom.db "SELECT current_value FROM settings WHERE key='work_mode'"`
	if [ mode -eq "normal" ]; then
		browser -qws "http://127.0.0.1:8080/" &
	elif [ mode -eq "correct" ]; then
		browser -qws "http://127.0.0.1:8080/jiaozhun.html" &
	elif [ mode -eq "install" ]; then
		browser -qws "http://127.0.0.1:8080/settings.html" &
	else
		browser -qws "http://127.0.0.1:8080/" &
	fi
done
