#!/bin/bash

echo "监控系统正在启动..."
echo "执行数据库预配置..."
/usr/zeus/preconfig.sh /usr/zeus/ontom.db
echo "开始启动主程序..."

while [ true ]; do
	/usr/zeus/zeus
	echo "检测到程序主动退出, 2秒后自动重启..."
	sleep 2;
	echo "重新预配置数据库"
	/usr/zeus/preconfig.sh /usr/zeus/ontom.db
	echo "正在重启...."
done
