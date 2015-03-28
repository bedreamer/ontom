#!/bin/sh
SQLITE3=sqlite3
DB=ontom.db

if [ $# -eq 1 ];then
	echo "使用数据库 $1"
	DB=$1
fi

SYSTYPE=`$SQLITE3 $DB "SELECT current_value FROM settings WHERE key='system_type'"`
MODULE=`$SQLITE3 $DB "SELECT current_value FROM settings WHERE key='module_kind'"`

# 根据系统选型配置是否有充电屏监控通信模组
if [ $SYSTYPE -eq 0 ]; then
	# 一体式
	echo "系统配置: 一体式充电机"
	echo "    关闭     充电屏配置功能...."
	$SQLITE3 $DB "UPDATE RS485_config set disabled='true' where id='00000004'"
	$SQLITE3 $DB "UPDATE RS485_config set disabled='true' where id='00000005'"
	$SQLITE3 $DB "UPDATE RS485_config set disabled='true' where id='00000006'"
else
	# 分体式
	# 目前只支持公司自产模块的分体式充电桩
	echo "系统配置: 分体式充电机"
	echo "    打开     充电屏配置功能...."
	$SQLITE3 $DB "UPDATE RS485_config set disabled='false' where id='00000004'"
	$SQLITE3 $DB "UPDATE RS485_config set disabled='false' where id='00000005'"
	$SQLITE3 $DB "UPDATE RS485_config set disabled='false' where id='00000006'"

	# 模块仅仅支持AN10750
	echo "系统强制选择模块: AN10750"
	$SQLITE3 $DB "UPDATE settings set current_value='0' where key='module_kind'"
	
	echo "分体式系统预配置完成."
fi

# 再次选择系统中配置的模块型号
MODULE=`$SQLITE3 $DB "SELECT current_value FROM settings WHERE key='module_kind'"`

# 根据模块型号选择要使用的串口模组
if [ $MODULE -eq 0 ];then
	# AN10750 模块
	echo "系统采用AN10750模块"
	echo "    打开     奥能协议转换盒.."
	$SQLITE3 $DB "UPDATE RS485_config set disabled='false' where id='00000007'"
	$SQLITE3 $DB "UPDATE RS485_config set disabled='false' where id='00000008'"
	
	echo "关闭冲突的串口模组..."
	echo "    关闭     英瑞克模块协议转换盒模组"
	$SQLITE3 $DB "UPDATE RS485_config set disabled='true' where id='00000009'"
	$SQLITE3 $DB "UPDATE RS485_config set disabled='true' where id='0000000A'"
elif [ $MODULE -eq 4 ];then
	# 英瑞克 EVR400-7500
	echo "系统采用英瑞克 EVR400-7500模块，选择协议转换盒.."
	$SQLITE3 $DB "UPDATE RS485_config set disabled='false' where id='00000009'"
	$SQLITE3 $DB "UPDATE RS485_config set disabled='false' where id='0000000A'"
	
	echo "关闭冲突的串口模组..."
	echo "    关闭     奥能协议转换盒模组"
	$SQLITE3 $DB "UPDATE RS485_config set disabled='true' where id='00000007'"
	$SQLITE3 $DB "UPDATE RS485_config set disabled='true' where id='00000008'"
else
	echo "预配置系统出问题了"
	exit 1;
fi

echo "默认配置项..."
echo "    打开     奥能-综合采样盒"
$SQLITE3 $DB "UPDATE RS485_config set disabled='false' where id='00000001'"
$SQLITE3 $DB "UPDATE RS485_config set disabled='false' where id='0000000E'"
echo "    打开     周立功-7816读卡器"
$SQLITE3 $DB "UPDATE RS485_config set disabled='false' where id='00000002'"
echo "    打开     华立-三相电能表"
$SQLITE3 $DB "UPDATE RS485_config set disabled='false' where id='0000000B'"
$SQLITE3 $DB "UPDATE RS485_config set disabled='false' where id='0000000C'"

echo "系统预配置完成..."
exit 0;
