#!/bin/bash

DB="./scripts/install.db"
SQLITE=sqlite3
SVN=svn

VER="undefined"
MSG="undefined"
ATR="666"
TYPE="file"

while getopts f:t:s:a:V:D:P:C:L:T:M:hHv param; do
        # commands
        case "$param" in
                'D') DB=$OPTARG ;;
				's') SRC=$OPTARG ;;
                'T') TYPE=$OPTARG ;;
				't') DES=$OPTARG ;;
				'f') FNM=$OPTARG ;;
				'a') ATR=$OPTARG ;;
				'V') VER=$OPTARG ;;
                'C') CLS=$OPTARG ;;
                'M') MSG=$OPTARG ;;
                'h'|'H')
                echo -e "充电桩监控系统文件注册程序\r\n"\
                        "Usage:\r\n"\
                        "    install-register-file.sh {COMMANDS}...\r\n"\
                        "COMMANDS:\r\n"\
                        "       -D ==> database: 指定安装配置数据库，默认为$DB\r\n"\
                        "       -s ==> source: 文件源位置\r\n"\
                        "       -T ==> type: 安装文件类型,默认: $TYPE;\r\n"\
                        "       -t ==> desination: 文件目标位置 eg: /usr/zeus/\r\n"\
						"       -f ==> filename: 目标安装文件名\r\n"\
                        "       -a ==> attribute: 安装文件默认属性, 默认值: $ATR\r\n"\
                        "       -V ==> version: 安装文件版本号, 默认值: $VER\r\n"\
                        "       -C ==> class: 安装文件从属类型\r\n"\
                        "       -M ==> message: 安装提示信息, 默认值: $MSG\r\n"\
                        "       -h ==> help: 显示这条帮助信息;\r\n"\
                        "       -v ==> version: 显示安装器的版本\r\n"\
                        "BROTHERS:\r\n"\
                        "       install-register-link.sh   符号链接安装工具\r\n"\
                        "           use <install-register-file.sh -h> for more infomation\r\n"\
                        "       install-register-dir.sh   目录安装工具\r\n"\
                        "           use <install-register-dir.sh -h> for more infomation\r\n"\
                        "AUTHOR:\r\n"\
                        "       LiJie <cuplision@163.com> 2015/06/05 10:39\r\n"
                exit 0;
                ;;
                'v')
                                echo "install-register-file.sh  revision 1.0"
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

if (( ${#DB} <= 0 || ${#SRC} <= 0 || ${#DES} <= 0 || ${#CLS} <= 0 || ${#FNM} <= 0 ));then
        echo "need correct paramaters -D,-s,-T,-t,-f,-a and -P etc., use -h for more information."
        exit 1
fi

$SQLITE $DB "INSERT INTO files VALUES('"$SRC"','"$TYPE"','"$DES/$FNM"','"$ATR"','"$VER"','"$CLS"','"$MSG"');"
if (( $? == 0 ));then
        echo "install $SRC into $DES/$FNM success!"
        exit 0;
else
        echo "install $SRC into $DES/$FNM failed!"
        exit 1
fi
