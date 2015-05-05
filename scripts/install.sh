#/bin/bash

PACKFILE=zeus.update.tar
INSTALLDIRS="/usr/zeus /usr/zeus/plugins /usr/zeus/drivers /srv/www"
CFGS="ontom.db ontom.cfg user.cfg"
VERSION=`svn info $0|grep Revision|grep -o '[0-9]\+'`

config='no'
drivers='no'
library='no'
plugins='no'
html='no'
qtlib='no'
prefix="$WORKDIR/install"
target="zeus.update.tar"

cd $WORKDIR
while getopts acdlpuqvhP:T: param; do
	case "$param" in
	a|-all) # 安装全部
		config='yes';
		drivers='yes';
		library='yes';
		plugins='yes';
		html='yes';
		qtlib='yes';
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
	u|-html) # 仅安装html文件
		html='yes';
	;;
	q|-qtlib) # 仅安装QT库文件
		qtlib='yes';
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
			"	-a ==> all: 安装全部系统文件(驱动，库，UI，服务端);\r\n"\
			"	-c ==> config: 安装配置文件及配置数据库;\r\n"\
			"	-d ==> drivers: 安装BMS驱动程序;\r\n"\
			"	-l ==> library: 安装系统库文件\r\n"\
			"	-p ==> plugins: 安装系统插件\r\n"\
			"	-u ==> html: 安装显示用html文件;\r\n"\
			"	-q ==> qtlibrary: 安装QT库文件;\r\n"\
			"	-v ==> version: 显示安装器的版本\r\n"\
			"	-h ==> help: 显示这条帮助信息;\r\n"\
			"OPTIONS:\r\n"\
			"	-P ==> prefix: 指定安装目录(默认: `pwd`/install/);\r\n"\
			"	-T ==> target: 指定输出目标文件名(默认: zeus.update.tar);\r\n"\
			"AUTHOR:\r\n"\
			"	LiJie <cuplision@163.com> 2015/05/05 09:00"
		exit 0;
	;;

	P|-prefix)
		prefix=$OPTARG
	;;
	T|-target)
		target=$OPTARG
	;;
	*)
		echo "无法识别的参数 $param=$OPTARG, 使用install -h 查看帮助."
		exit 1;
	;;
	esac
done
shift $(( OPTIND - 1 ));
exit 0;

echo "create system directories...."
for d in $INSTALLDIRS;do
	mkdir -p install$d
	echo "  create ./install$d"	
done

echo "copy depends librarys...."
for f in `ls lib*.so`;do
	cp $f 'install/usr/zeus/'
	echo "  update ./install/usr/zeus/$f"
done

echo "copy extern librarys...."
for f in `ls exso*.so`;do
	cp $f 'install/usr/zeus/plugins/'
	echo "  update ./install/usr/zeus/plugins/$f"
done

echo "copy bms drivers....."
for f in `ls bmsdrv_*.so`;do
	cp $f 'install/usr/zeus/drivers/'
	echo "  update ./install/usr/zeus/drivers/$f"
done

echo "copy configuration database and files...."
for f in $CFGS;do
	cp $f 'install/usr/zeus/'
	echo "  update ./install/usr/zeus/$f"
done

echo "copy main program file `readlink me`"
cp `readlink me` 'install/usr/zeus/zeus'

echo "copy UI files..."
rsync -r UI_html/* 'install/srv/www/'

cd install

echo "CREATE INSTALL/UPDATE PACKET: $PACKFILE"
tar --exclude-vcs -cf $PACKFILE `ls`
cp zeus.update.tar /media/sf_E_DRIVE/www/
printf "`date` \033[31minstall/$PACKFILE\033[0m packed.\n"
