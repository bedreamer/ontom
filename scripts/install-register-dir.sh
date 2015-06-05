#!/bin/bash

DB="./scripts/install.db"
SQLITE=sqlite3

while getopts D:P:C:M:hHv param; do
        # commands
        case "$param" in
		'D') DB=$OPTARG ;;
		'P') RP=$OPTARG ;;
		'C') CLS=$OPTARG ;;
		'M') MSG=$OPTARG ;;
		'h'|'H')
                echo -e "充电桩监控系统目录注册程序\r\n"\
                        "Usage:\r\n"\
                        "    install-register-dir.sh {COMMANDS}...\r\n"\
                        "COMMANDS:\r\n"\
                        "       -D ==> database: 指定安装配置数据库，默认为$DB\r\n"\
                        "       -C ==> class: 安装目录的安装类别\r\n"\
                        "       -M ==> message: 安装过程中的输出提示;\r\n"\
                        "       -P ==> path: 安装目录的目标位置,eg: /usr/zeus/\r\n"\
                        "       -h ==> help: 显示这条帮助信息;\r\n"\
                        "       -v ==> version: 显示安装器的版本\r\n"\
						"BROTHERS:\r\n"\
						"       install-register-file.sh   文件安装工具\r\n"\
						"           use <install-register-file.sh -h> for more infomation\r\n"\
						"       install-register-link.sh   符号链接安装工具\r\n"\
						"           use <install-register-link.sh -h> for more infomation\r\n"\
						"AUTHOR:\r\n"\
						"       LiJie <cuplision@163.com> 2015/06/05 10:39\r\n"
				exit 0;
		;;
		'v')
				echo "install-register-dir.sh  revision 1.0"
			exit 0;
		;;
		*)
				echo "无法识别的参数 $param=$OPTARG, 使用install -h 查看帮助."
				exit 1;
		;;
		esac
done
shift $(( OPTIND - 1 ));

if [ ! -e "$DB" ];then
	echo "could not open install configure database. $DB"
	exit 1;
fi

if (( ${#DB} <= 0 || ${#RP} <= 0 || ${#CLS} <= 0 ));then
	echo "need correct paramaters -D,-C and -P, use -h for more information."
	exit 1
fi

$SQLITE $DB "INSERT INTO dirs VALUES('"$RP"','"$CLS"','"$MSG"');"
if (( $? == 0 ));then
	echo "install dir $RP success!"
	exit 0;
else
	echo "install dir $RP failed!"
	exit 1
fi
