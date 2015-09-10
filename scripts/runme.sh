#!/bin/sh

echo "监控系统正在启动..."
echo "执行数据库预配置..."
/usr/zeus/script/preconfig.sh /usr/zeus/ontom.db
echo "开始启动主程序..."

if [ ! -d /tmp/hellhound ] && [ ! -e /tmp/hellhound ];then 
    mkdir /tmp/hellhound
	mkdir /tmp/zeus
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

ifconfig eth0 up
ifconfig eth1 up
# 调试用端口，该端口靠近USB口
#ifconfig eth1 up
#ifconfig eth1 10.100.121.200/16

while [ true ]; do
	for PID in `pidof browser` `pidof zeus`;do
		kill -9 $PID
	done
	ip link set can0 down
    ip link set can1 down
    ip link set can0 type can bitrate 250000
    ip link set can1 type can bitrate 250000
    ifconfig can0 up
    ifconfig can1 up
	workmode=`sqlite3 /usr/zeus/ontom.db "SELECT current_value FROM settings WHERE key='work_mode'"`
	if [ $workmode == "normal" ]; then
		echo "启动到主页面"
		hellhound.sh browser -qws "http://127.0.0.1:8080/" &
		BROPID=$!
	elif [ $workmode == "correct" ]; then
		echo "启动到校准页面"
		hellhound.sh browser -qws "http://127.0.0.1:8080/jiaozhun.html" &
		BROPID=$!
	elif [ $workmode == "calibrate" ]; then
		echo "启动屏幕校准程序"
		ts_calibrate
		echo "重新设定工作模式为正常模式"
		sqlite3 /usr/zeus/ontom.db "UPDATE settings SET current_value='normal'  WHERE key='work_mode'"
		echo "启动到主页面"
		hellhound.sh browser -qws "http://127.0.0.1:8080/" &
		BROPID=$!
	else
		echo "启动到默认页面"
		hellhound.sh browser -qws "http://127.0.0.1:8080/" &
		BROPID=$!
	fi
	/usr/zeus/zeus > /dev/null
	echo "检测到程序主动退出, 1秒后自动重启..."
	kill -9 $BROPID
	sleep 1;
	echo "关闭地狱犬.."
	echo "重新预配置数据库"
	/usr/zeus/script/preconfig.sh /usr/zeus/ontom.db
	echo "正在重启...."
done
