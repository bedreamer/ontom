#!/bin/sh

echo "监控系统正在启动..."
echo "执行数据库预配置..."
/usr/zeus/preconfig.sh /usr/zeus/ontom.db
echo "开始启动主程序..."

while [ true ]; do
	for PID in `pidof browser` `pidof zeus`;do
		kill -9 $PID
	done
	workmode=`sqlite3 /usr/zeus/ontom.db "SELECT current_value FROM settings WHERE key='work_mode'"`
	if [ $workmode == "normal" ]; then
		echo "启动到主页面"
		browser -qws "http://127.0.0.1:8080/" &
	elif [ $workmode == "correct" ]; then
		echo "启动到校准页面"
		browser -qws "http://127.0.0.1:8080/jiaozhun.html" &
	elif [ $workmode == "install" ]; then
		echo "启动到安装配置页面"
		browser -qws "http://127.0.0.1:8080/settings.html" &
	else
		echo "启动到默认页面"
		browser -qws "http://127.0.0.1:8080/" &
	fi
	/usr/zeus/zeus >> /tmp/zeus.log
	echo "检测到程序主动退出, 1秒后自动重启..."
	sleep 1;
	echo "重新预配置数据库"
	/usr/zeus/preconfig.sh /usr/zeus/ontom.db
	echo "正在重启...."

done
