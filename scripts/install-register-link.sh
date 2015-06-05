#!/bin/bash

DB="./scripts/install.db"
SQLITE=sqlite3

while getopts D:P:C:L:T:hHv param; do
        # commands
        case "$param" in
                'D') DB=$OPTARG ;;
                'P') RP=$OPTARG ;;
                'C') CLS=$OPTARG ;;
                'L') LNK=$OPTARG ;;
				'T') TAG=$OPTARG ;;
                'h'|'H')
                echo -e "充电桩监控系统链接注册程序\r\n"\
                        "Usage:\r\n"\
                        "    install-register-link.sh {COMMANDS}...\r\n"\
                        "COMMANDS:\r\n"\
                        "       -D ==> database: 指定安装配置数据库，默认为$DB\r\n"\
                        "       -C ==> class: 安装目录的安装类别\r\n"\
                        "       -L ==> link: 链接名称;\r\n"\
						"       -T ==> target: 链接目标文件\r\n"\
                        "       -P ==> path: 安装目录的目标位置,eg: /usr/zeus/\r\n"\
                        "       -h ==> help: 显示这条帮助信息;\r\n"\
                        "       -v ==> version: 显示安装器的版本\r\n"\
                        "BROTHERS:\r\n"\
                        "       install-register-file.sh   文件安装工具\r\n"\
                        "           use <install-register-file.sh -h> for more infomation\r\n"\
                        "       install-register-dir.sh   目录安装工具\r\n"\
                        "           use <install-register-dir.sh -h> for more infomation\r\n"\
                        "AUTHOR:\r\n"\
                        "       LiJie <cuplision@163.com> 2015/06/05 10:39\r\n"
                exit 0;
                ;;
                'v')
                                echo "install-register-link.sh  revision 1.0"
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

if (( ${#DB} <= 0 || ${#RP} <= 0 || ${#CLS} <= 0 || ${#LNK} <= 0 || ${#TAG} <= 0 ));then
        echo "need correct paramaters -D,-C,-T,-L and -P, use -h for more information."
        exit 1
fi

$SQLITE $DB "INSERT INTO links VALUES('"$TAG"','"$TAG"','"$CLS"','"$RP"');"
if (( $? == 0 ));then
        echo "install link $LNK ==> $TAG success!"
        exit 0;
else
        echo "install link $LNK ==> $TAG failed!"
        exit 1
fi
