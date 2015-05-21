#!/bin/bash

prefix="install"
target="zeus.update.tar"
copydir="/media/sf_E_DRIVE/www/"
CFGS="ontom.db ontom.cfg user.cfg"
TOOLS="testing/sqlite3-arm scripts/runme.sh scripts/automount.sh scripts/export.sh scripts/upgrade.sh scripts/preconfig.sh"
VERSION=`svn info $0|grep Revision|grep -o '[0-9]\+'`

config='no'
drivers='no'
library='no'
plugins='no'
html='no'
qtlib='no'
browser='no'
tools='no'

while getopts abcdlpqtuvhP:T:W:C: param; do
	# commands
	case "$param" in
	a|-all) # 安装全部
		config='yes';
		drivers='yes';
		library='yes';
		plugins='yes';
		html='yes';
		qtlib='yes';
		browser='yes';
		tools='yes';
	;;
	b|-browser) # 安装浏览器
		browser='yes';
	;;
	c|-config)
		config='yes';
	;;
	d|-drivers) # 仅安装驱动
		drivers='yes';
	;;
	l|-library) # 仅安装库文件
		library='yes';
	;;
	p|-plugins) # 仅安装插件
		plugins='yes';
	;;
	q|-qtlib) # 仅安装QT库文件
		qtlib='yes';
	;;
	t|-tools) # 安装工具及脚本
		tools='yes';
	;;
	u|-html) # 仅安装html文件
		html='yes';
	;;
	v|-version)
		echo "installer of zeus version: $VERSION"
		exit 0;
	;;
	h|-help)
		echo -e "充电桩监控系统文件安装程序\r\n"\
			"Usage:\r\n"\
			"    install {COMMANDS}... {OPTIONS}...\r\n"\
			"COMMANDS:\r\n"\
			"	-a ==> all: 安装全部系统文件(驱动，库，UI，服务端, 工具);\r\n"\
			"	-b ==> browser: 安装浏览器;\r\n"\
			"	-c ==> config: 安装配置文件及配置数据库;\r\n"\
			"	-d ==> drivers: 安装BMS驱动程序;\r\n"\
			"	-l ==> library: 安装系统库文件\r\n"\
			"	-p ==> plugins: 安装系统插件\r\n"\
			"	-q ==> qtlibrary: 安装QT库文件;\r\n"\
			"	-t ==> tools: 安装脚本工具集;\r\n"\
			"	-u ==> html: 安装显示用html文件;\r\n"\
			"	-h ==> help: 显示这条帮助信息;\r\n"\
			"	-v ==> version: 显示安装器的版本\r\n"\
			"OPTIONS:\r\n"\
			"	-C ==> copydir: 指定输出目录(默认: $copydir)\r\n"\
			"	-P ==> prefix: 指定安装目录(默认: `pwd`/install/);\r\n"\
			"	-T ==> target: 指定输出目标文件名(默认: zeus.update.tar);\r\n"\
			"	-W ==> workdir: 指定当前工作目录(默认: `pwd`)\r\n"\
			"AUTHOR:\r\n"\
			"	LiJie <cuplision@163.com> 2015/05/05 09:00"
		exit 0;
	;;
	# options
	P|-prefix)
		prefix=$OPTARG
	;;
	T|-target)
		target=$OPTARG
	;;
	W|-workdir)
		WORKDIR=$OPTARG
	;;
	C|-copydir)
		copydir=$OPTARG
	;;
	*)
		echo "无法识别的参数 $param=$OPTARG, 使用install -h 查看帮助."
		exit 1;
	;;
	esac
done
shift $(( OPTIND - 1 ));

if [ ${#WORKDIR} -eq 0 ];then
	echo "没有找到环境变量WORKDIR，使用默认目录`pwd`, 请使用-W指定工作目录"
	WORKDIR=`pwd`
fi

if [ $browser == 'yes' ];then
	echo "create directory $prfix/usr/zeus"
	mkdir -p $prefix/usr/zeus
	echo "	copy browser..."
	cp browser/browser $prefix/usr/zeus/browser
fi

if [ $tools == 'yes' ];then
	echo "create directory $prefix/usr/zeus/script"
	mkdir -p $prefix/usr/zeus/script
	for f in $TOOLS;do
		cp $f "$prefix/usr/zeus/script/"
		echo "  update $prefix/usr/zeus/script/$f"
	done
	chmod 777 $prefix/usr/zeus/script/*.sh
	chmod 777 $prefix/usr/zeus/script/*
	echo "  install sqlite3...."
	mkdir $prefix/usr/bin/
	cp testing/sqlite3-arm $prefix/usr/bin/sqlite3
	chmod 777 $prefix/usr/bin/sqlite3
fi

if [ $library == "yes" ];then
	echo "create directory $prfix/usr/zeus"
	mkdir -p $prefix/usr/zeus
	echo "	copy depends librarys...."
	for f in `ls lib*.so`;do
		cp $f "$prefix/usr/zeus/"
		echo "  update $prefix/usr/zeus/$f"
	done
	chmod 777 $prefix/usr/zeus/*.so
fi

if [ $library == "yes" ];then
	echo "create directory $prefix/usr/zeus/plugins"
	mkdir -p $prefix/usr/zeus/plugins
	echo "	copy plugins...."
	for f in `ls exso*.so`;do
		cp $f "$prefix/usr/zeus/plugins/"
		echo "  update $prefix/usr/zeus/plugins/$f"
	done
	chmod 777 $prefix/usr/zeus/plugins/*.so
fi

if [ $drivers == "yes" ];then
	echo "create directory $prefix/usr/zeus/drivers"
	mkdir -p $prefix/usr/zeus/drivers
	echo "	copy bms drivers....."
	for f in `ls bmsdrv_*.so`;do
		cp $f "$prefix/usr/zeus/drivers/"
		echo "  update $prefix/usr/zeus/drivers/$f"
	done
	chmod 777 $prefix/usr/zeus/drivers/*.so
fi

if [ $config == "yes" ];then
	echo "create directory $prefix/usr/zeus"
	mkdir -p $prefix/usr/zeus
	echo "	copy configuration database and files...."
	for f in $CFGS;do
		cp $f "$prefix/usr/zeus/"
		echo "  update $prefix/usr/zeus/$f"
	done
	chmod 777 $prefix/usr/zeus/*.sh
	chmod 777 $prefix/usr/zeus/script/*.sh
	echo "  copy profile to /etc/profile overlaped..."
	mkdir $prefix/etc/
	cp scripts/profile $prefix/etc/ 
fi

if [ $html == "yes" ];then
	echo "create directory $prefix/srv/www"
	mkdir -p $prefix/srv/www
	echo "	copy UI files..."
	rsync -r UI_html/* "$prefix/srv/www/"
	chmod 777 $prefix/srv/www/*.sh
fi

if [ $qtlib == "yes" ];then
	echo "create directory $prefix/usr/gui"
	mkdir -p $prefix/usr/gui
	echo "	copy qt library files..."
	rsync -r testing/gui/* $prefix/usr/gui/
fi

mkdir -p $prefix/usr/zeus
echo "	copy main program file `readlink me`"
cp `readlink me` "$prefix/usr/zeus/zeus"

cd $prefix

echo "CREATE INSTALL/UPDATE PACKET: $prefix/$target"
tar --exclude-vcs -cf $target `ls`
printf "`date` \033[31m$prefix/$target\033[0m packed.\n"
mkdir $copydir$VERSION
echo $VERSION > $copydir'VERSION'
echo $copydir"VERSION updated!"
cp $target $copydir$VERSION/
echo "target: $target copyied to $copydir$VERSION/"
