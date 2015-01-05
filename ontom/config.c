/**************************************************************************
 * 配置接口
 * All rights reserved Hangzhou Power Equipment Co., Ltd.
 * author: LiJie
 * email:  bedreamer@163.com
 * 2014-08-22
 *
 * 配置文件语法：
 * ==================
 * 配置文件语法基本遵循LINUX系统中的配置文件格式，配置项基本格式为：
 *     A=B
 * A的正则表达式为 [a-zA-Z_]{1,32}, 也就是由字母，下划线组成的字符串
 * 且长度不超过32字节。
 * A必须顶格写起，否则视为语法错误。
 * B可以是任意非空格开头的字符串，在‘=’ 和 B 之间全部的空格将被忽略。
 * 暂定B的有效长度最长为127字节。
 * 以 ';', '#' 开头的行将被视为注释行。
 * 如下是一个配置文件例子，纵向边框为 '+'
 *
 * +-----------------------------------------------------------------------
 * +# 因为开始字符为‘#’，所以这是个注释行
 * +;  因为开始字符为‘;’，所以也是个注释行
 * +  # 行开头只接受的表达式为 [a-zA-Z_]， 因此这行有语法错误 
 * +# 下面这几行是正确的配置写法
 * +config_item=198888
 * +_config_item=    199999
 * +_config_=asdfasdfas231$&%^$^%*21f
 * +# 下面的写法是错的
 * +; 配置项名称和'='之间不能有空格
 * +config_item =123123
 * +; 开始字符不能是空格
 * +  config_item=123123
 * +; 配置项名称只接受下划线和字母 
 * +8987_a=fasdf
 * +------------------------------------------------------------------------
 *******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>  /* system data type definitions */
#include <sys/socket.h> /* socket specific definitions */
#include <netinet/in.h> /* INET constants and stuff */
#include <arpa/inet.h>  /* IP address conversion stuff */
#include "config.h"
#include "error.h"
#include "log.h"
#include "ajax.h"

static char *value_type[] = {"STRING", "INT", "FLOAT", "BOOL", "N/A"};
//static char *value_system[] = {"NO", "YES", "N/A"};
//static char *value_status[] = {"INVALID", "VALID", "MODIFY", "N/A"};
//{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{
CONFIG_DOMAIN_BEGIN
//数据名称                   |数据类型| 用户配置项？|nnn数据状态|数据默认值|	数据值
// ! 下面这一项需要在初始化时手动设置
{"xmlsrv_port",					C_INT,		no,		C_INVALID,	.cuv.i=8081,	{"8081"}},
{"socket_config", 				C_BOOL,		no,		C_INVALID,  .cuv.b=true,    {"TRUE"}},
{"socket_config_port", 			C_INT,		no,		C_INVALID,  .cuv.i=9990,    {"9990"}},
{"socket_config_autheticate", 	C_BOOL,     no,		C_INVALID,  .cuv.n=0,       {"TRUE"}},
{"version_httpd",               C_STRING,   no,     C_INVALID,  .cuv.n=0,       {"N/A"}},
{"version_browser",             C_STRING,   no,     C_INVALID,  .cuv.n=0,       {"N/A"}},
{"version_tomd",                C_STRING,   no,     C_INVALID,  .cuv.n=0,       {"N/A"}},
{"version_godd",                C_STRING,   no,     C_INVALID,  .cuv.n=0,       {"N/A"}},
{"manual_passwd",               C_STRING,   no,     C_INVALID,  .cuv.n=0,       {"00000"}},
{"system_passwd",               C_STRING,   no,     C_INVALID,  .cuv.n=0,       {"11111"}},
{"manufacturer_passwd",         C_STRING,   no,     C_INVALID,  .cuv.n=0,       {"8989889"}},
{"user_config_file",            C_STRING,   no,     C_INVALID,  .cuv.n=0,       {"user.cfg"}},
   // 充电模块个数
{"charge_module_nr",            C_INT,      yes,    C_VALID,    .cuv.i=14,      {"14"}},
{"初始电压",                     C_INT,      yes,    C_VALID,    .cuv.i=14,      {"5500"}},
{"需求电压",                     C_INT,      yes,    C_VALID,    .cuv.i=14,      {"5500"}},
{"需求电流",                     C_INT,      yes,    C_VALID,    .cuv.i=14,      {"300"}},

// 系统参数不应该出现在配置文件中, 仅供程序内部使用, 不公开
{"thread_xml_server_id",        C_INT,      no,     C_INVALID,  .cuv.i=0,       {"N/A"}},
{"thread_bms_write_service",    C_INT,      no,     C_INVALID,  .cuv.i=0,       {"N/A"}},
{"thread_bms_read_service",     C_INT,      no,     C_INVALID,  .cuv.i=0,       {"N/A"}},
{"thread_uart_server_id",       C_INT,      no,     C_INVALID,  .cuv.i=0,       {"N/A"}},
{"thread_config_server_id",     C_INT,      no,     C_INVALID,  .cuv.i=0,       {"N/A"}},
// 定时器分辨率
{"HachikoTTL",                  C_INT,      no,     C_VALID,    .cuv.i=0,       {"10000000"}},
// 线程栈大小，单位KB
{"thread_stack_size",           C_INT,      no,     C_VALID,    .cuv.i=0,       {"1024"}},
{"debug_lvA",                   C_BOOL,     no,     C_VALID,    .cuv.i=0,       {"FALSE"}},
{"debug_lvB",                   C_BOOL,     no,     C_VALID,    .cuv.i=0,       {"FALSE"}},
{"debug_lvC",                   C_BOOL,     no,     C_VALID,    .cuv.i=0,       {"FALSE"}},
{"debug_lvD",                   C_BOOL,     no,     C_VALID,    .cuv.i=0,       {"FALSE"}},


// 开始充电任务的刷卡卡号
{"triger_card_sn",              C_STRING,   no,     C_VALID,    .cuv.i=0,       {"N/A"}},
   // 确认充电的刷卡卡号
{"confirm_card_sn",             C_STRING,   no,     C_VALID,    .cuv.i=0,       {"N/A"}},
   // 终止充电的刷卡卡号
{"settle_card_sn",              C_STRING,   no,     C_VALID,    .cuv.i=0,       {"N/A"}},
   // 卡内余额
{"card_remaind_money",          C_FLOAT,    no,     C_VALID,    .cuv.f=0.0f,    {"0.0"}},
// 用户配置数据
   // 超级卡序列号
{"super_card_sn",               C_STRING,  yes,     C_INVALID,  .cuv.i=0,       {"N/A"}},
{"socket_config_username", 		C_STRING,  yes,		C_INVALID,  .cuv.n=0,       {"god"}},
{"socket_config_passwd", 		C_STRING,  yes,		C_INVALID,  .cuv.n=0,       {"god"}},
{"charge_pile_serial",          C_STRING,  yes,     C_INVALID,  .cuv.n=0,       {"N/A"}},
{"price",                       C_FLOAT,   yes,     C_INVALID,  .cuv.f=0.0f,    {"0.0"}},

// 关键故障标记, 标识是否可进行充电, 参考文档 充电桩相关信息.xlsx 充电桩故障对照表
{"keyfault",                    C_STRING,  no,      C_VALID,    .cuv.i=0,       {"X11100111111100100110000011111"}},
// 故障号对应字符串
{"E0100",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"**系统总故障**"}},
{"E0101",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"交流输入停电故障"}},
{"E0102",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"辅助电源故障"}},
{"E0103",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"充电屏监控通信中断"}},
{"E0104",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"充电屏监控次要要信1通信中断"}},
{"E0105",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"充电屏监控次要要信2通信终端"}},
{"E0106",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"充电模块通信故障"}},
{"E0107",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"采样单元通信中断"}},
{"E0108",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"BMS通信故障"}},
{"E0109",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"充电模块故障"}},
{"E010A",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"1# 充电枪物理连接断开故障"}},
{"E010B",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"2# 充电枪物理连接断开故障"}},
{"E010C",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"绝缘故障"}},
{"E010D",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"母线过压"}},
{"E010E",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"母线欠压"}},
{"E010F",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"母线短路"}},
{"E0110",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"电池过压"}},
{"E0111",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"电池欠压"}},
{"E0112",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"电池短路"}},
{"E0113",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"电池反接故障"}},
{"E0114",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"电池过流"}},
{"E0115",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"充电桩温度过高"}},
{"E0116",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"充电桩温度过低"}},
{"E0117",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"充电桩湿度过高"}},
{"E0118",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"充电桩湿度过低"}},
{"E0119",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"总输出熔断器熔断"}},
{"E011A",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"总输出开关跳闸"}},
{"E011B",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"1#枪输出开关跳闸"}},
{"E011C",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"2#枪输出开关跳闸"}},
{"E011D",                       C_STRING,  no,      C_VALID,    .cuv.i=0,       {"防雷器故障"}},
CONFIG_DOMAIN_END
//}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}

static struct config_struct *config_search(const char *name)
{
	struct config_struct *head = configs;
	int nr = 0;

	if ( name == NULL ) goto die;

    for ( ; head && head->config_name && head->config_name[0]; head ++, nr ++ ) {
        if ( 0 == strcmp(head->config_name, name) ) return head;
	}

die:
	return NULL;
}

/*配置数据初始化, 从配置文件读入配置数据*/
int config_initlize(const char *cfgfile)
{
	struct config_struct *thiz;
	const char *thefile = NULL;
	int len = 0, line = 0;
	FILE *fp;
	char fbuf[256];

	if ( cfgfile == NULL ) {
        thiz = config_search("default_cfg");
		if ( thiz == NULL ) {
			log_printf(WRN, 
				"not give config-file,"
				" but no default config-file specfied alse. "
				"use all item as default.");
			return ERR_OK;
		} else {
			if ( thiz->config_type != C_STRING ) {
                int i = 4;
                //if (thiz->config_type >= 0 && thiz->config_type <= 3) {
                    i = thiz->config_type;
                //}
				log_printf(ERR, "config_type need string give %s", 
					value_type[i]);
				return ERR_WRONG_PARAM;
			}
			thefile = thiz->config_value;
		}
	} else {
		thefile = cfgfile;
	}

	len = strlen(thefile);
	if ( len <= 0 ) {
		return ERR_WRONG_PARAM;
	}

	fp = fopen(thefile, "r");
	if ( NULL == fp ) {
		if ( thefile == cfgfile ) {
            log_printf(ERR,
                       "could not read %s, faile."
                       "all item will use by default value,"
                       " this may cause an error.",
                       thefile);
            return ERR_FILE_IO_FAILE;
		} else {
			log_printf(WRN, 
			"could not read default %s, use confige-item all default.", thefile);
			return ERR_OK;
		}
	}

	while ( !feof(fp) ) {
		fgets(fbuf, sizeof(fbuf), fp);
		line ++;
		len = strlen(fbuf);
		char A[32] = {0}, B[128] = {0};
		int A_l = 0, B_l = 0, i, eq = 0;
#if 0
		log_printf(DBG, "%s", fbuf);
#endif
		// 至少应该是  a=b 类型的
		if ( len <= 3 ) continue;
		// 以 ';', '#' 开头的视为注释
		if ( fbuf[0] == ';' || fbuf[0] == '#' ) continue;
		// 匹配 A = B 模式
		// A： 只能是字母组成的
		// B:  可以为任意组合
		for ( i = 0; i < len ; i ++ ) {
			if ( eq == 0 ) {
				/* 还未匹配到 '=' */
				if ( (fbuf[i] >= 'a' && fbuf[i] <= 'z') ||
					 (fbuf[i] >= 'A' && fbuf[i] <= 'Z') ||
					  fbuf[i] == '_' ){
                    if ( (unsigned int)A_l >= sizeof(A) ) break;
					A[ A_l ++ ] = fbuf[i];
				} else if ( fbuf[i] == '=' && A_l ){
					// 找到 '='
					eq = i;
				} else {
					// syntax error.
					log_printf(WRN, 
						"config file %s syntax error @%d line, omited!", 
						thefile, line);
					break;
				}
			} else {
				if ( fbuf[i] == ' ' && !B_l ) {
					// '=' 后紧跟的空格视为无效
					continue;
				}
				if ( fbuf[i] == '\r' || fbuf[i] == '\n' ) {
					// 换行即视为结束
                    B[ B_l ] = '\0';
					break;
				}
                if ( (unsigned int)B_l >= sizeof(B) ) break;

				if ( fbuf[i] >= 0x20 && fbuf[i] <= 0x7F )
					B[ B_l ++ ] = fbuf[i];
				else {
					break;
					B[ B_l ] = '\0';
				}
			}
		}
		if (A_l && B_l) {
			config_write(A, B);
		}
	}
	
	fclose(fp);

	return ERR_OK;
}

/*同步用户配置数据, 将配置数据写入配置文件*/
int config_writeout()
{
    struct config_struct *head = configs;
    const char *user_cfg = NULL;
    int nr = 0, cfg_err = 0;
    FILE *fp = NULL;

    user_cfg = config_read("user_config_file");
    if ( user_cfg == NULL ) {
        // 初始状态时，配置数据表中必须有这个配置项
        log_printf(ERR,
                   "need 'config_file' config item,"
                   " program maybe incrrect!!");
        goto die;
    }
    if ( *user_cfg == '\0' ) {
        cfg_err = 1;
        user_cfg = "user.cfg";
        if ( NULL == config_write("config_file", "user.cfg") ) {
            log_printf(ERR,
                   "'user_cfg' not gave, "
                   "use default 'user.cfg' faile also!"
                   "will try write out to file 'user.cfg'");
        } else {
            log_printf(ERR,
                   "'user_cfg' not gave, "
                   "use 'user.cfg' by default.");
        }
    }

    fp = fopen(user_cfg, "w");
    if ( fp == NULL && cfg_err == 1 ) {
        log_printf(ERR, "something wrong with write default file %s",
                   user_cfg);
        goto die;
    }
    if ( fp == NULL && cfg_err != 0 ) {
        log_printf(ERR, "something wrong with write user's file %s",
                   user_cfg);
        goto die;
    }
    for ( ; head && head->config_name; head ++, nr ++ ) {
        if ( head->config_user == no ) continue;
        fprintf(fp, "%s=%s", head->config_name, head->config_value);
    }
    log_printf(INF, "write file %s done, total %d items.",
               user_cfg, nr);
    fclose(fp);

    return ERR_OK;
die:
	return ERR_ERR;
}

/*读取配置数据*/
const char *config_read(const char *name)
{
	struct config_struct *thiz;

	if ( NULL == name ) return NULL;

	thiz = config_search(name);
	if ( thiz == NULL ) return NULL;
	
    if ( thiz->config_status == C_INVALID && thiz->config_user == no ) {
		log_printf(WRN, "read uninitialized config-item [%s] use default %s", 
			name, thiz->config_value ? thiz->config_value : "<NULL>");
	}
	return thiz->config_value;
}

/*修改配置数据, 只能修改数据值，不能修改数据类型.*/
const char *config_write(const char *name, const char *value)
{
	struct config_struct *thiz;
	int len = 0;
	const char * pvalue = value;

	if ( NULL == name ) return NULL;

    thiz = config_search(name);
	if ( thiz == NULL ) return NULL;

#if 0
    // FIXED: 变量修改通用接口，可以设置系统值或是用户变量值
    //        此时，这个变量的作用仅仅是用来区分配置数据来自哪个配置文件
    if (thiz->config_user == yes) {
        /*用户配置项不能修改, 只能通过其他方式修改.*/
		return NULL;
	}
#endif
#if 0
    // FIXED: 可以在初始状态时将变量设为无效状态，以供初始化时设为有效
	if ( C_INVALID == thiz->config_status ) {
		/*无效的配置项不能修改*/
		return NULL;
	}
#endif

	len = strlen(value);
	if ( C_STRING == thiz->config_type ) {
		if ( len > CONFIG_MAX_VALUE ) {
			memcpy(thiz->config_value, pvalue, CONFIG_MAX_VALUE - 1);
			thiz->config_value[CONFIG_MAX_VALUE-1] = '\0';
		} else  if ( len == 0 ) {
			memset(thiz->config_value, 0, sizeof(thiz->config_value));
		} else {
			strcpy(thiz->config_value, pvalue);
		}
	} else if ( C_BOOL == thiz->config_type ) {
		if ( 0 == strcmp("false", pvalue) ||
			 0 == strcmp("FALSE", pvalue) ) {
			strcpy(thiz->config_value, "FALSE");
		} else if ( 0 == strcmp("true", pvalue) ||
					0 == strcmp("TRUE", pvalue) ) {
            strcpy(thiz->config_value, "TRUE");
		} else {
			return NULL;
		}
	} else if ( C_INT == thiz->config_type ) {
		/*
		 * EXPR:  [+-]?[0-9]{1,8}
		 */
		 if ( value == NULL || len <= 0 ) return NULL;
		 else
		 {
			int sign = 0;
			int len = 0;

			if ( value[0] == '-' ) sign = -1;
			if ( value[0] == '+' ) sign = 1;
			if ( sign != 0 ) {
				value ++;
			}

			while ( *value && len < 9 ) {
				if ( *value >= '0' && *value <= '9' ) {
					len ++;
					value ++;
				} else {
					break;
				}
			}

			if ( len <= 0 ) return NULL;
			len = len + (sign ? 1 : 0);
			thiz->config_value[ len ] = '\0';
			memcpy(thiz->config_value, pvalue, len);
		 }
	} else if ( C_FLOAT == thiz->config_type ) {
		/*
		 * EXPR:  [+-]?[0-9]{1,8}+\.[0-9]{1,8}
		 */
		 if ( value == NULL || len <= 0 ) return NULL;
		 else
		 {
			int sign = 0;
			int dot = 0;
			int zhengshu = 0, xiaoshu = 0;
            int len;

			if ( value[0] == '-') sign = -1;
			if ( value[0] == '+') sign = 1;
			if ( sign ) value ++;

			while ( * value && zhengshu < 9 && xiaoshu < 9 ) {
					if ( * value >= '0' && *value <= '9' ) {
							if ( dot == 0 ) zhengshu ++;
							else xiaoshu ++;
					} else if ( * value == '.' ) {
							if ( dot ) break;
							else dot = 1;
					} else break;
			}

			if ( zhengshu + xiaoshu <= 0 ) return NULL;
			len = zhengshu + xiaoshu;
			len += (sign ? 1 : 0);
			len += (dot ? 1 : 0);
			thiz->config_value[ len ] = '\0';
			memcpy(thiz->config_value, pvalue, len);
		 }
	} else {
		/*数据崩溃*/
		log_printf(ERR, "unexpected exception @%s:%d", __FILE__, __LINE__);
		return NULL;
	}

	if ( C_VALID == thiz->config_status ) {
		thiz->config_status = C_MODIFY;
	}

	if ( C_INVALID == thiz->config_status ) {
		thiz->config_status = C_VALID;
	}

	return thiz->config_value;
}

/*套接字配置服务程序*/
void *config_drive_service(void *arg)
{
    int ok = 1;
    int srv_port;
    const char *pport = config_read((const char *)"socket_config_port");
    int s_config = -1;
    int need_autheticate = 0, good_request = 0;
    struct sockaddr_in si_me, si_other;
    struct socket_config_request request;
    struct socket_config_ack ack;
    const char * autheticate, *username, *passwd, *value;
    struct config_struct *thiz;
    int recv_len, slen;

    srv_port = atoi(pport);
    if ( srv_port <= 2048 ) {
        log_printf(ERR, "socket config service port %d invalid. abort!",
                   srv_port);
        goto thread_die;
    }

    s_config = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if ( -1 == s_config ) {
        log_printf(ERR, "create socket confige server faile!");
        goto thread_die;
    }

    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(srv_port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if( bind(s_config , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1) {
        log_printf(ERR, "bind to port %d faile, abort.", srv_port);
        goto thread_die;
    }

    while ( ok ) {
        usleep(5000);
        slen = sizeof(si_other);
        memset(&ack, 0, sizeof(ack));
        recv_len = recvfrom(s_config, &request,
                 sizeof(struct socket_config_request),
                 0, (struct sockaddr *) &si_other, (socklen_t * __restrict__)&slen);
        if ( recv_len <= 0 ) continue;

        request.config_username[ 7 ] = '\0';
        request.config_passwd[15] = '0';
        request.config_item_name[31] = '\0';
        request.config_item_value[127] = '\0';

        need_autheticate = 0;
        autheticate = config_read("socket_config_autheticate");
        if ( 0 == strcmp( autheticate, "TRUE") || 0 == strcmp( autheticate, "true") ) {
            need_autheticate = 1;
            username = config_read("socket_config_username");
            passwd = config_read("socket_config_passwd");
        }
        if ( need_autheticate ) {
            if ( strcmp(username, request.config_username) == 0 &&
                 strcmp(passwd, request.config_passwd ) == 0 ) {
                good_request = 1;
            } else {
                ack.config_result = 300;
                good_request = 0;
                log_printf(INF, "autheticate faile, user: %s, passwd: %s",
                           request.config_username, request.config_passwd);
                goto bad_request;
            }
        }

        if ( request.config_cmd != CONFIG_RD &&
             request.config_cmd != CONFIG_WR ) {
            ack.config_result = 505;
            good_request = 0;
            goto bad_request;
        }

        thiz = NULL;
        // 读配置数据可以使用索引或是数据名，数据名优先
        if ( request.config_item_name[0] != '\0' ) {
            thiz = config_search(request.config_item_name);
        }

        if ( request.config_cmd == CONFIG_RD ) {
            if ( thiz == NULL ) {
                if ( request.config_item_index >=
                     sizeof(configs)/sizeof(struct config_struct)) {
                    ack.config_result = 404;
                    good_request = 0;
                    goto bad_request;
                } else {
                    thiz = &configs[ request.config_item_index ];
                }
            }
            if ( thiz->config_name == NULL ) {
                ack.config_result = 404;
                good_request = 0;
                log_printf(DBG, "read the last config item.");
                goto bad_request;
            }
            ack.config_cmd = CONFIG_RD;
            ack.config_result = 0;
            strncpy(ack.config_name, thiz->config_name, 31);
            ack.config_name[31] = '\0';
            ack.config_status = thiz->config_status;
            ack.config_type = thiz->config_type;
            ack.config_user = thiz->config_user;
            strncpy(ack.config_value, thiz->config_value, 127);
            ack.config_value[127] = '\0';
        }
        if ( request.config_cmd == CONFIG_WR ) {
            if ( thiz == NULL ) {
                ack.config_result = 404;
                log_printf(DBG, "not found %s to write",
                           request.config_item_name);
                good_request = 0;
                goto bad_request;
            }
            if ( thiz->config_type != request.config_item_type ) {
                ack.config_result = 404;
                log_printf(DBG, "type conflict.");
                good_request = 0;
                goto bad_request;
            }
            request.config_item_value[ 127 ] = '\0';
            request.config_item_name[ 31 ] = '\0';
            value =
             config_write(request.config_item_name, request.config_item_value);
            if ( NULL == value ) {
                ack.config_result = 303;
                good_request = 0;
                log_printf(DBG, "write value faile.");
                goto bad_request;
            }
            ack.config_cmd = CONFIG_WR;
            ack.config_result = 0;
            strncpy(ack.config_name, thiz->config_name, 31);
            ack.config_name[31] = '\0';
            ack.config_status = thiz->config_status;
            ack.config_type = thiz->config_type;
            ack.config_user = thiz->config_user;
            strncpy(ack.config_value, thiz->config_value, 127);
            ack.config_value[127] = '\0';
        }
        recv_len = sendto(s_config, &ack, sizeof(struct socket_config_ack),
                          0, (struct sockaddr*) &si_other, slen);
        //log_printf(DBG, "send %d bytes", recv_len);
        continue;
bad_request:
        log_printf(DBG, "bad request.user: %s, passwd: %s, name:%s, give: %s",
                   request.config_username, request.config_passwd,
                   request.config_item_name, request.config_item_value);
        if ( good_request  <= 0 ) {
            //now reply to server socket/the client with the same data
            ack.config_cmd = request.config_cmd;
            if ( ack.config_result == 0 ) {
                ack.config_result = 500;
            }
            recv_len = sendto(s_config, &ack, sizeof(struct socket_config_ack),
                              0, (struct sockaddr*) &si_other, slen);
            continue;
        }
    }

thread_die:
    if ( s_config != -1  ) {
        close(s_config);
    }
    return NULL;
}


#if CONFIG_DEBUG_CONFIG >= 1
/*打印配置数据*/
void config_print()
{
	struct config_struct *head = configs;
	int nr = 0, i;

    printf("%-32s  %-6s  %-6s  %-8s  %s\n",
           "NAME", "TYPE", "USER", "STATUS", "VALUE");
	for ( ; head && head->config_name && *head->config_name; head ++, nr ++ ) {
		printf("%-32s", head->config_name);

        i = (head->config_type >=0 && head->config_type <= 3)?
                    head->config_type : 4;
		printf("  %-6s", value_type[ i ]);

        i = (head->config_user == 0 || head->config_user == 1) ?
                    head->config_user : 2;
		printf("  %-6s", value_system[ i ]);

        i = (head->config_status >=0 && head->config_status <= 2) ?
                    head->config_status : 3;
		printf("  %-8s", value_status[ i ]);
		
		printf("  %s\n", head->config_value);
	}
	
	if ( nr == 0 ) {
		printf("****** There is no config item.");
	}
}
#endif /*CONFIG_DEBUG_CONFIG*/

// 生成动态设置页面
int ajax_debug_list(struct ajax_xml_struct *thiz)
{
    struct config_struct *head = configs;
    int nr = 0, output_len = 0;
    char tag[128] = {0}, val[128] ={0}, de_val[128]={0};

    mg_get_var(thiz->xml_conn, "t", tag, 128);
    mg_get_var(thiz->xml_conn, "v", val, 128);

    if ( tag[0] && val[0] ) {
        mg_url_decode(val, 128, de_val, 128, 0);
        log_printf(DBG_LV1, "WEB SET %s: %s", tag, de_val);
        if ( de_val[0] ) {
            config_write(tag, val);
        }
    }

    thiz->ct = "text/html";
    output_len = sprintf(&thiz->iobuff[output_len],
                            "<html><title>直流充电桩内部调试接口</title>"
                            "<script>function c(e,t){"
                            "window.location.href=\"list.html?t=\""
                            "+document.getElementById(t).innerHTML+\"&v=\""
                            "+document.getElementById(e).value;"
                            "}</script>"
                            "<body><table border=\"1px\" align=\"center\">");
    for ( ; head && head->config_name != NULL && head->config_name[0] && nr < 37; head ++, nr ++ ) {
        log_printf(DBG_LV1, "get configure list from WEB. %d:%p:%d", nr, head, output_len);
        output_len += sprintf(&thiz->iobuff[output_len], "<tr><td id=\"t%d\">%s</td>",
                              nr,
                              head->config_name);
        output_len += sprintf(&thiz->iobuff[output_len],
                              "<td>%s</td>",
                              value_type[(unsigned int)(head->config_type)]);
        output_len += sprintf(&thiz->iobuff[output_len],
                              "<td><input id=\"e%d\" type=\"text\" value=\"%s\""
                              "/><a href=\"javascript:c(\'e%d\',\'t%d\')\">SET</a></td></tr>",
                              nr,
                              head->config_value, nr, nr);
        continue;
        output_len += sprintf(&thiz->iobuff[output_len],
        "<td><a href=\"/debug/list.html?mode=set&tag=%s&seed=%s\"></td>",
                                 head->config_name, head->config_value);
    }
    log_printf(DBG_LV0, "general done..");

    output_len += sprintf(&thiz->iobuff[output_len], "</table></body></html>");
    thiz->xml_len = output_len;

    return ERR_OK;
}

// 生成动态设置页面
int ajax_debug_json_list(struct ajax_xml_struct *thiz)
{
    struct config_struct *head = configs;
    int nr = 0, output_len = 0;
    char tag[128] = {0}, val[128] ={0}, de_val[128]={0};

    mg_get_var(thiz->xml_conn, "t", tag, 128);
    mg_get_var(thiz->xml_conn, "v", val, 128);

    if ( tag[0] && val[0] ) {
        mg_url_decode(val, 128, de_val, 128, 0);
        log_printf(DBG_LV1, "WEB SET %s: %s", tag, de_val);
        if ( de_val[0] ) {
            config_write(tag, val);
        }
    }

    thiz->ct = "application/json";
    output_len += sprintf(&thiz->iobuff[output_len], "{\"configs\":[");
    for ( ; head && head->config_name != NULL && head->config_name[0]; head ++, nr ++ ) {
        if ( C_STRING == head->config_type ) {
            output_len += sprintf(&thiz->iobuff[output_len],
                                  "{\"n\":\"%s\",\"t\":\"%s\",\"v\":\"%s\"},",
                                  head->config_name,
                                  value_type[(unsigned int)(head->config_type)],
                                  head->config_value);
        } else if ( C_BOOL == head->config_type ) {
            output_len += sprintf(&thiz->iobuff[output_len],
                                  "{\"n\":\"%s\",\"type\":\"%s\",\"v\":%s},",
                                  head->config_name,
                                  value_type[(unsigned int)(head->config_type)],
                    (head->config_value[0] == 't' || head->config_value[0] == 'T') ?"true":"false");
        } else {
            output_len += sprintf(&thiz->iobuff[output_len],
                                  "{\"n\":\"%s\",\"t\":\"%s\",\"v\":%s},",
                                  head->config_name,
                                  value_type[(unsigned int)(head->config_type)],
                    head->config_value[0]=='N'?"0":head->config_value);
        }
    }
    thiz->iobuff[output_len--] = '\0';
    output_len += sprintf(&thiz->iobuff[output_len], "]}");
    thiz->xml_len = output_len;

    return ERR_OK;
}

// 提交并跳转
int ajax_debug_commit(struct ajax_xml_struct *thiz)
{
    thiz->ct = "text/html";
    return ERR_ERR;
}
