﻿# !/usr/bin/bash

prefix="install"
target="zeus.update.tar"
copydir="/media/sf_E_DRIVE/www/"
installdb="scripts/install.db"
VERSION=`svn info $0|grep Revision|grep -o '[0-9]\+'`
CMDCACHE=".instal.cache"

config='no'
drivers='no'
database='no'
ext='no'
html='no'
library='no'
profile='no'
qtlib='no'
script='no'
zeus='no'
vpnsurpport='no'

while getopts acdDelpqstuvzmhP:T:W:C: param; do
	# commands
	case "$param" in
	a|-all) # 安装全部
		config='yes'
		drivers='yes'
		database='yes'
		ext='yes'
		html='yes'
		library='yes'
		profile='yes'
		qtlib='yes'
		script='yes'
		zeus='yes'
		vpnsurpport='yes'
	;;
	c|-config)
		config='yes';
	;;
	d|-drivers) # 仅安装驱动
		drivers='yes';
	;;
	D|-database) # 安装数据配置文件
		database='yes'
	;;
	e|-ext) # 仅安装插件
		ext='yes';
	;;
	l|-library) # 仅安装库文件
		library='yes';
	;;
	p|-profile) # 系统配置脚本
		profile='yes'
	;;
	q|-qtlib) # 仅安装QT库文件
		qtlib='yes';
	;;
	s|-script) # 仅安装脚本
		script='yes';
	;;
	t|-tools) # 安装工具及脚本
		tools='yes';
	;;
	u|-html) # 仅安装html文件
		html='yes';
	;;
	z|-zeus) # 安装程序
		zeus='yes';
	;;
	m|-modules) # 仅安装VPN支持文件
		vpnsurpport='yes';
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

function do_install() {
	P=`sqlite3 $installdb "SELECT path FROM dirs WHERE class LIKE \"%$1%\""`
	if (( $? != 0 ));then
		echo "ERR: $?"
		sqlite3 $installdb "SELECT path FROM dirs"
		echo $?
	fi
	if [ ${#P} -eq 0 ];then
		echo "没有找到需要安装的配置文件目录, 忽略{$P:$1}."
	else
		for d in $P;do
			printf "创建目录 $d"
			mkdir -p $prefix/$d
			if [ $? -eq 0 ];then
				echo "   成功."
			else
				echo "   失败 ($?) !"
			fi
		done
		Fsrc=`sqlite3 -separator ' ' $installdb "SELECT * FROM files WHERE class LIKE \"%$1%\""`
		i='0'
		if (( ${#Fsrc} != 0 ));then
			for f in $Fsrc;do
				case $i in
					'0') src=$f;i='1';;
					'1') typ=$f;i='2';;
					'2') des=$f;i='3';;
					'3') attr=$f;i='4';;
					'4') version=$f;i='5';;
					'5') class=$f;i='6';;
					'6')
						comment=$f
						case $typ in
							"link")
								printf "    安装 $comment $prefix$des  "
								cp `readlink $src` $prefix/$des
								chmod $attr $prefix$des
								if [ -e $prefix$des ];then
									echo "成功."
								else
									echo "失败!."
								fi
							;;
							"file")
								printf "    安装 $comment $prefix$des  "
								cp $src $prefix$des
								chmod $attr $prefix$des
								if [ -e $prefix$des ];then
									echo "成功."
								else
									echo "失败!."
								fi
							;;
							*)
								echo "unsurpport file type. $src"
							;;
						esac
						i='0'
					;;
					*)
						echo "catch exceptions."
					;;
				esac
			done
			Lsrc=`sqlite3 -separator ' ' $installdb "SELECT target,link,dir FROM links WHERE class LIKE \"%$1%\""`
			if (( ${#Lsrc} != 0 ));then
				i='0'
				for f in $Lsrc;do
					case $i in
						'0') tag=$f;i='1';;
						'1') link=$f;i='2';;
						'2') 
							newdir=$f
							cd $WORKDIR/$prefix$newdir
							if (( $? != 0 ));then
								echo " ** 无法切换到目录$prefix$newdir, 目录不存在"
							else
								printf "    链接  $link  --> $tag "
								i='0'
								ln -s $tag $link
								if (( $? != 0 ));then
									echo "失败!"
								else
									echo "成功."
								fi
								cd - > /dev/null
							fi
							i='0';
						;;
						*)
							echo "exception.."
						;;
					esac
				done
			fi
		fi
	fi
}

rm install/* -rf

sleep 0.1
if [ $script == "yes" ];then
	do_install "scripts"
fi
sleep 0.1
if [ $zeus == "yes" ];then
	do_install "program"
fi
if [ $config == "yes" ];then
	do_install "config"
fi
sleep 0.1
if [ $drivers == "yes" ];then
	do_install "drivers"
fi
sleep 0.1
if [ $database == "yes" ];then
	do_install "database"
fi
sleep 0.1
if [ $ext == "yes" ];then
	do_install "plugins"
fi
sleep 0.1
if [ $html == "yes" ];then
	./scripts/installui.sh
fi
sleep 0.1
if [ $library == "yes" ];then
	do_install "library"
fi
sleep 0.1
if [ $profile == "yes" ];then
	do_install "profile"
fi
sleep 0.1
if [ $qtlib == "yes" ];then
	do_install "qtlib"
fi

if [ $vpnsurpport == "yes" ];then
	echo "	decompress VPN surpport files..."
	tar -C $prefix -xf scripts/VPN.update.tar
fi

cd $WORKDIR/$prefix
echo "CREATE INSTALL/UPDATE PACKET: $prefix/$target"
pwd
tar --exclude-vcs -czf $target `ls`
printf "`date` \033[31m$prefix/$target\033[0m packed.\n"
mkdir $copydir$VERSION
echo $VERSION > $copydir'VERSION'
echo $copydir"VERSION updated!"
cp $target $copydir$VERSION/
echo "target: $target copyied to $copydir$VERSION/"

exit 0
