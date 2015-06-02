# !/usr/bin/bash

prefix="install"
target="zeus.update.tar"
copydir="/media/sf_E_DRIVE/www/"
installdb="scripts/install.db"
VERSION=`svn info $0|grep Revision|grep -o '[0-9]\+'`

config='no'
drivers='no'
ext='no'
html='no'
library='no'
profile='no'
qtlib='no'
script='no'
zeus='no'

while getopts acdlpqstuvzhP:T:W:C: param; do
	# commands
	case "$param" in
	a|-all) # 安装全部
		config='yes'
		drivers='yes'
		ext='yes'
		html='yes'
		library='yes'
		profile='yes'
		qtlib='yes'
		script='yes'
		zeus='yes'
	;;
	c|-config)
		config='yes';
	;;
	d|-drivers) # 仅安装驱动
		drivers='yes';
	;;
	e|-ext) # 仅安装插件
		ext='yes';
	;;
	l|-library) # 仅安装库文件
		library='yes';
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
	u|-zeus) # 安装程序
		zeus='yes';
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

if [ $config == "yes" ];then
	do_install "config"
fi

function do_install() {
	P=`sqlite3 $installdb "SELECT path FROM dirs WHERE class LIKE \"%$1%\""`
	if [ ${#P} -eq 0 ];then
		echo "没有找到需要安装的配置文件目录, 忽略{$P}."
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
								printf "    安装 $comment $prefix$des"
								cp `readlink $src` $prefix/$des
								chmod $attr $prefix$des
								if [ -e $prefix$des ];then
									echo "成功."
								else
									echo "失败!."
								fi
							;;
							"file")
								printf "    安装 $comment $prefix$des"
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
				olddir=`pwd`
				i='0'
				for f in $Lsrc;do
					case $i in
						'0') target=$f;i='1';;
						'1') link=$f;i='2';;
						'2') newdir=$f;i='0';;
							echo $target
							echo $link
							echo $newdir
							cd $WORKDIR/$prefix$newdir
							if (( $? != 0 ));then
								echo " ** 无法切换到目录$prefix$newdir, 目录不存在"
							else
								printf "    创建链接  $link  --> $target "
								ln -s $target $link
								i='0'
								if (( $? != 0 ));then
									echo "失败!"
								else
									echo "成功."
								fi
							fi
							i='0';
						;;
						*)
							echo "exception.."
						;;
					esac
				done
				cd $oldpwd
			fi
		fi
	fi
}

do_install "qtlib"
