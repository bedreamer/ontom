#!/bin/bash
echo "直流充电桩安装程序..."

echo "安装页面文件."
cp -R UI_html/* /install/srv/www/

echo "安装配置文件."
cp ontom.db /install/usr/zeus/
cp ontom.cfg /install/usr/zeus/
cp user.cfg /install/usr/zeus/

echo "安装工具-sqlite3"
cp 

echo "安装主程序-浏览器"
cp browser/browser /install/usr/zeus/
echo "安装主程序-服务端"
cp `readlink me` /install/usr/zeus/