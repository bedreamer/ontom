#ifndef CONFIG_INCLUDED_H_
#define CONFIG_INCLUDED_H_

//{{{{{{{{{{{{{
#define _DEBUG  0

#define ___THREAD_ENTRY___

// 控制配置模块的调试
#if _DEBUG >= 1
	#define CONFIG_DEBUG_CONFIG  1
    #define CONFIG_DEBUG_ZEUS    1
#else
	#define CONFIG_DEBUG_CONFIG  0
    #define CONFIG_DEBUG_ZEUS    0
#endif

// 配置串口是否接收SIGIO事件
#define CONFIG_SUPPORT_SIGIO           0
// 采用真发送均衡算法
#define CONFIG_SUPPORT_FRAME_BALANCE   1
// 支持的充电模块个数
#define CONFIG_SUPPORT_CHARGE_MODULE   14
/* 采用异步发送超时, 若采用定时器来进行发送完成超时计算
 *可能会出现丢帧情况，且该情况较为严重，丢帧率可达到1%-2%
 *采用同步等待发送方式发送数据后，现象得到改善。
 */
#define CONFIG_SUPPORT_ASYNC_UART_TX   0
// 充电管理中可添加的最多的充电作业个数, 4把枪，最多四个任务同时执行
#define CONFIG_SUPPORT_CHARGE_JOBS     4
// 最多可预约充电作业个数
#define CONFIG_SUPPORT_BOOK_JOBS       8
/* 支持的BMS个数
 * 安徽合肥电动公交车项目的配置需求是一个监控器管理两把充电枪
 * 但是由于充电限流问题的限制，同一时刻只能有一把枪在进行充电
 * 现在也不排除以后可能需要一个监控同时给一个或多个电池组充电
 * 进行管理，因此在这里将支持的BMS配置成可变参数的形式。
 */
#define CONFIG_SUPPORT_BMS_NR          4
#define CONFIG_SUPPORT_SIMPLE_BOX_NR   CONFIG_SUPPORT_BMS_NR
/*系统支持的485串口设备个数*/
#define CONFIG_SUPPORT_RS485_NR        2
// 系统支持的充电机组数
#define CONFIG_SUPPORT_CHARGE_GRP_NR   2
//}}}}}}}}}}}}}

// 默认数据库文件
#define DEFAULT_DB  "ontom.db"

#ifndef bool
	#define bool unsigned char
#endif
#ifndef true
	#define true (bool)(1)
#endif
#ifndef false
	#define false (bool)(0)
#endif
#ifndef yes
	#define yes true
#endif
#ifndef no
	#define no false
#endif

// 配置项数值最大长度为127
#define CONFIG_MAX_VALUE   128
struct config_struct {
	// 配置项名称
	char *config_name;

	// 配置项类型, 
	// string: 字符串型数据
	#define C_STRING   0x00
	// int： 整形数据，可正可负
	#define C_INT      0x01
	// float：浮点型数据
	#define C_FLOAT    0x02
	// bool：布尔型数据
	#define C_BOOL     0x03
	char config_type;

	// 数据是否来自配置文件
    // 有些用户配置项，默认不需要进行配置
    bool config_user;

	// 配置数据状态,
	// 0 -- 初始状态，无效
	#define C_INVALID   0
	// 1 -- 有效，数据已经读入、写出或默认值
	#define C_VALID     1
	// 2 -- 已经修改，未保存
	#define C_MODIFY    2
	char config_status;

	// 配置数据值
	union{
		float f; // float
		int   i; // int
		int   b; // bool
		int   n; // N/A
	}cuv;

	// 配置项值，统一使用字符存储，使用时按类型转换
	char config_value[CONFIG_MAX_VALUE];
};

/*配置数据初始化, 从配置文件读入配置数据*/
int config_initlize(const char *cfgfile);
/*同步配置数据, 将配置数据写入配置文件*/
int config_writeout();
/*读取配置数据*/
const char *config_read(const char *name);
/*修改配置数据, 只能修改数据值，不能修改数据类型.*/
const char *config_write(const char *name, const char *value);
// 套接字传递的数据包格式
struct socket_config_request {
    // 用户名
    char config_username[8];
    // 密码
    char config_passwd[16];
    // 操作类型
    unsigned int config_cmd;
    #define CONFIG_RD       0xAA  // 读变量
    #define CONFIG_WR       0x55  // 写变量
    // 变量名
    char config_item_name[32];
    // 变量索引，变量名为空时使用
    unsigned int  config_item_index;
    // 变量索引号
    unsigned char config_item_type;
    // 配置值
    char config_item_value[CONFIG_MAX_VALUE];
};
// 套接字传递的数据包格式
struct socket_config_ack {
    // 操作类型
    unsigned int config_cmd;
    // 操作结果, 0: 成功， 其他： 失败
    unsigned int config_result;
    // 配置项名称
    char config_name[32];
    // 配置项类型,
    char config_type;
    // 数据是否来自配置文件
    // 有些用户配置项，默认不需要进行配置
    bool config_user;
    // 配置数据状态,
    char config_status;
    // 配置项值，统一使用字符存储，使用时按类型转换
    char config_value[CONFIG_MAX_VALUE];
};
/*套接字配置服务程序*/
void *config_drive_service(void *arg);

// 数据库配置结构
struct db_config_struct {
    char * config_attrib ;	    // 配置项属性（工厂配置项|系统配置项|用户配置项|信息|标志|核心配置）
    char * config_name ;         // 配置项名称
    char * config_type ;         // 配置项数据类型（字符串|数字|选项|布尔）
    char * config_value ;	    // 配置项当前值
    char * config_options ;	    // 选项列表,用‘:’作为选项分隔符（针对选项有效）
    char * config_value_max ;    // 配置项最大值(针对数字有效)
    char * config_value_min ;    // 配置项最小值(针对数字有效)
    char * config_value_default ;// 配置项默认值
    char * config_value_pre ;    // 配置项前一次的值
    bool   config_writeable ;    // 配置项是否用户可写
    bool   config_readable ;	    // 配置项用户是否可读
    char * config_last_update ;  // 最后一次修改时间
    char * config_comment ;	    // 配置项解释说明
    bool   config_dis ;            // 是否在配置页面中显示
    char * config_dis_name ;	    // 配置项的显示名(在配置页面生效)
    char * config_dis_comment ;   // 配置项的显示说明文字(在配置页面生效)
};

#if CONFIG_DEBUG_CONFIG >= 1
/*打印配置数据*/
void config_print();
#else
#define config_print
#endif /*CONFIG_DEBUG_CONFIG*/

#define CONFIG_DOMAIN_BEGIN static struct config_struct configs[]={
#define CONFIG_DOMAIN_END {"", 0,   0,     0,  .cuv.i=0,    {""}} };

#endif /*CONFIG_INCLUDED_H_*/
