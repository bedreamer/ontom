#!/bin/sh
SQLITE3
DB=ontom.dh

SYSTYPE=`$SQLITE3 $DB "SELECT current_value FROM settings WHERE key='system_type'"`
MODULE=`$SQLITE3 $DB SELECT current_value FROM settings WHERE key='module_kind'`

# 根据系统选型配置是否有充电屏监控通信模组
if [ $SYSTYPE == 0 ]; then
	# 一体式
	echo "系统配置: 一体式充电机"
	echo "关闭充电屏配置功能...."
	$SQLITE3 $DB "UPDATE RS485_config set disable='true' where id='00000004'"
	$SQLITE3 $DB "UPDATE RS485_config set disable='true' where id='00000005'"
	$SQLITE3 $DB "UPDATE RS485_config set disable='true' where id='00000006'"
else
	# 分体式
	# 目前只支持公司自产模块的分体式充电桩
	echo "系统配置: 分体式充电机"
	echo "打开充电屏配置功能...."
	$SQLITE3 $DB "UPDATE RS485_config set disable='false' where id='00000004'"
	$SQLITE3 $DB "UPDATE RS485_config set disable='false' where id='00000005'"
	$SQLITE3 $DB "UPDATE RS485_config set disable='false' where id='00000006'"

	# 模块仅仅支持AN10750
	echo "系统强制选择模块: AN10750"
	$SQLITE3 $DB "UPDATE settings set current_value='0' where key='module_kind'"
	
	echo "分体式系统预配置完成."
fi

# 再次选择系统中配置的模块型号
MODULE=`$SQLITE3 $DB SELECT current_value FROM settings WHERE key='module_kind'`

# 根据模块型号选择要使用的串口模组
if [ $MODULE == 0 ];then
	# AN10750 模块
	echo "系统采用AN10750模块，选择奥能协议转换盒.."
	$SQLITE3 $DB "UPDATE RS485_config set disable='false' where id='00000007'"
	$SQLITE3 $DB "UPDATE RS485_config set disable='false' where id='00000008'"
	
	echo "关闭冲突的串口模组..."
	echo "   关闭     英瑞克模块协议转换盒模组"
	$SQLITE3 $DB "UPDATE RS485_config set disable='true' where id='00000009'"
	$SQLITE3 $DB "UPDATE RS485_config set disable='true' where id='0000000A'"
elif [ $MODULE == 4];then
	# 英瑞克 EVR400-7500
	echo "系统采用英瑞克 EVR400-7500模块，选择协议转换盒.."
	$SQLITE3 $DB "UPDATE RS485_config set disable='false' where id='00000009'"
	$SQLITE3 $DB "UPDATE RS485_config set disable='false' where id='0000000A'"
	
	echo "关闭冲突的串口模组..."
	echo "   关闭     奥能协议转换盒模组"
	$SQLITE3 $DB "UPDATE RS485_config set disable='true' where id='00000007'"
	$SQLITE3 $DB "UPDATE RS485_config set disable='true' where id='00000008'"
else;
	echo "预配置系统出问题了"
	exit 1;
fi

echo "系统预配置完成..."
exit 0;