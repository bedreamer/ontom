#！/bin/sh
echo "开始安装读卡器....."
stty /dev/ttyO4 ispeed 19200 ospeed 19200
./uartwrite /dev/ttyO4 07 01 48 01 00 b0 03
echo "读卡器安装完成"
