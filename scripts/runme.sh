#!/bin/sh

echo "监控系统正在启动..."
echo "执行数据库预配置..."
/usr/zeus/preconfig.sh /usr/zeus/ontom.db
echo "开始启动主程序..."

if [ ! -d /tmp/hellhound ] && [ ! -e /tmp/hellhound ];then 
    mkdir /tmp/hellhound
fi

if [ ! -e /tmp/hellhound/zeus_pid ];then
	echo $$ > /tmp/hellhound/zeus_pid
else
    if [ ! -e "/proc/`cat "/tmp/hellhound/"$MD5"_pid"`" ];then
        echo "无效zeus进程, 自动清理."
        rm "/tmp/hellhound/zeus_pid"
    else
        echo "系统程序正常, PID=`cat "/tmp/hellhound/zeus_pid"`"
        exit 1;
    fi
fi

while [ true ]; do
	for PID in `pidof browser` `pidof zeus`;do
		kill -9 $PID
	done
	workmode=`sqlite3 /usr/zeus/ontom.db "SELECT current_value FROM settings WHERE key='work_mode'"`
	if [ $workmode == "normal" ]; then
		echo "启动到主页面"
		hellhound.sh browser -qws "http://127.0.0.1:8080/" &
		BROPID=$!
	elif [ $workmode == "correct" ]; then
		echo "启动到校准页面"
		hellhound.sh browser -qws "http://127.0.0.1:8080/jiaozhun.html" &
		BROPID=$!
	elif [ $workmode == "install" ]; then
		echo "启动到安装配置页面"
		hellhound.sh browser -qws "http://127.0.0.1:8080/settings.html" &
		BROPID=$!
	else
		echo "启动到默认页面"
		hellhound.sh browser -qws "http://127.0.0.1:8080/" &
		BROPID=$!
	fi
	/usr/zeus/zeus >> /tmp/zeus.log
	echo "检测到程序主动退出, 1秒后自动重启..."
	kill -9 $BROPID
	sleep 1;
	echo "关闭地狱犬.."
	echo "重新预配置数据库"
	/usr/zeus/preconfig.sh /usr/zeus/ontom.db
	echo "正在重启...."
done
