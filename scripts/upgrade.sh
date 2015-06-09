#!/bin/sh

updatefile=zeus.update.tar
HOST="N/A"

# U_USB --> upgrade from USB device one time.
# U_WEB --> upgrade from WEB server one time.
# UC_WEB --> upgrade from WEB server cycle.
MODE="U_USB"
DEMO='FALSE'
UPATH="www"

# 如果当前版本和服务器版本相同，加上这个参数则强行更新
FORCE="FALSE" 

while getopts D:dfH:h param;do
	case $param in
	'd') DEMO="TRUE" ;;
	'f') FORCE="TRUE" ;;
	'D') UPATH=$OPTARG;;
	'H') MODE="U_WEB";HOST=$OPTARG ;;
	'h'|*)
		echo "upgrade.sh 用于升级系统."
		echo "Usage:"
		echo "   upgrade.sh {-d|-h} {-H|-D}"
		echo "     -d:"
		echo "     -H: set upgrade server host address.(eg: www.oneps.com/zeus/)"
		echo "     -h: show this help message."
		echo "Author:"
		echo "     LiJie <bedreamer@163.com> 2015/06/09 12:24"
		exit 0;
	;;
	esac	
done
shift $(( OPTIND - 1 ));

if [ "TRUE" = $DEMO ];then
	if [ "N/A" = $HOST ];then
		echo "自动更新需要设定更新服务器地址!"
		exit 1;
	fi
	MODE="UC_WEB"
fi

if [ "U_WEB" = $MODE ] || [ "UC_WEB" = $MODE ] ;then
	SQ_PATH=`which sqlite3`
	if [ ${#SQ_PATH} -le 0 ];then
		echo "需要首先配置sqlite3的工作目录。"
		exit 1;
	fi
	if [ ! -e "/usr/zeus/ontom.db" ];then
		echo "/usr/zeus/ontom.db 数据库文件不存在."
		exit 1;
	fi

	if [ "N/A" = $HOST ];then
		HOST=`sqlite3 /usr/zeus/ontom.db "SELECT value FROM Gaea WHERE key='upgrade_host'"`
	fi
	if [ ${#HOST} -le 0 ];then
		echo "无效的服务器地址<$HOST>，更新失败"
		exit 1;
	fi

	UPATH=`sqlite3 /usr/zeus/ontom.db "SELECT value FROM Gaea WHERE key='upgrade_path'"`
	if [ ${#UPATH} -le 0 ];then
		echo "设定更新目录失败，采用默认目录</www>"
		UPATH="www"
	fi

	if [ "UC_WEB" = $MODE ];then
		CHECK_f=`sqlite3 /usr/zeus/ontom.db "SELECT value FROM Gaea WHERE key='upgrade_freq'"`
		if [ ${#CHECK_f} -le 0 ];then
			echo "使用默认检查更新周期 <1小时>"
			CHECK_f=3600
		else
			case $CHECK_f in
				"every_1min") CHECK_f=60;;
				"every_15min") CHECK_f=900;;
				"never") 
					echo "系统配置了不自动更新，3秒后退出"
					CHECK_f=0
					for i in `seq 1 1 3`;do
						print ".";
						sleep 1;
					done
					exit 0;
				;;
				"every_hour"|*) CHECK_f=3600;;
			esac
		fi
	fi
fi

DONE="FALSE"
if [ ! "U_USB" = $MODE ]; then
	while [ "FALSE" = $DONE ];do
		if [ -e "/tmp/zeus.version" ];then
			rm /tmp/zeus.version
		fi
		wget "http://$HOST/$UPATH/VERSION" -O /tmp/zeus.version >/dev/null 2>&1
		if [ -e "/tmp/zeus.version" ];then
			if [ -e "/tmp/zeus.update.tar" ];then
				rm /tmp/zeus.update.tar
			fi
			wget http://$HOST/$UPATH/`cat /tmp/zeus.version`/zeus.update.tar \
								-O /tmp/zeus.update.tar > /dev/null 2>&1
			if [ -e "/tmp/zeus.version" ];then
#				tar -C / -xvf /tmp/zeus.update.tar > /usr/zeus/update.log
				echo "Upgrade success version "`cat /tmp/zeus.version`
				mv /tmp/zeus.version /usr/zeus/zeus.version
			else
				echo "fetch zeus.update.tar from $HOST failed!"
			fi
		else
			echo "Upgrade failed!"
		fi
		if [ "UC_WEB" = $MODE ];then
			sleep $CHECK_f
		else
			exit 0;
		fi
	done
else
	for d in `ls /media`;do
		if [ -e /media/$d/$updatefile ];then
#			tar -C / -xvf /media/$d/$updatefile > /usr/zeus/update.log
			echo "Upgrade success."
			exit 0
		fi
	done
fi

exit 1
