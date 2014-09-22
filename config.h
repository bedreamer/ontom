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

//}}}}}}}}}}}}}

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
    unsigned char config_item_name[32];
    // 变量索引，变量名为空时使用
    unsigned int  config_item_index;
    // 变量索引号
    unsigned char config_item_type;
    // 配置值
    unsigned char config_item_value[CONFIG_MAX_VALUE];
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

#if CONFIG_DEBUG_CONFIG >= 1
/*打印配置数据*/
void config_print();
#else
#define config_print
#endif /*CONFIG_DEBUG_CONFIG*/

#define CONFIG_DOMAIN_BEGIN static struct config_struct configs[]={
#define CONFIG_DOMAIN_END {NULL, 0, 0, 0, {0}} };

#endif /*CONFIG_INCLUDED_H_*/
