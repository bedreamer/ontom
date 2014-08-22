#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "config.h"
#include "error.h"
#include "log.h"

static char *value_type[] = {"STRING", "INT", "FLOAT", "BOOL", "N/A"};
static char *value_system[] = {"NO", "YES", "N/A"};
static char *value_status[] = {"INVALID", "VALID", "MODIFY", "N/A"};
//{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{
CONFIG_DOMAIN_BEGIN
//数据名称      |数据类型|系统配置项？|nnn数据状态|数据默认值|		数据值
// ! 下面这一项需要在初始化时手动设置
{"workingroot", 				C_STRING,	yes,	C_VALID,	.cuv.n=0,		{"/usr/bin/"}},
{"default_cfg", 				C_STRING,	yes,	C_VALID,	.cuv.n=0,		{"ontom.cfg"}},
{"xmlsrv_port",					C_INT,		no,		C_INVALID,	.cuv.i=9989,	{"9989"}},
{"version_xml", 				C_STRING,	no,		C_INVALID,	.cuv.n=0,		{"version.xml"}},
{"socket_config", 				C_BOOL,		no,		C_INVALID,  .cuv.b=true,    {"TRUE"}},
{"socket_config_port", 			C_INT,		no,		C_INVALID,  .cuv.i=9990,    {"9990"}},
CONFIG_DOMAIN_END
//}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}

static struct config_struct *config_search(const char *name)
{
	struct config_struct *head = configs;
	int nr = 0;

	if ( name == NULL ) goto die;

	for ( ; head && head->config_name; head ++, nr ++ ) {
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
		thiz = config_search("/ontom/default_cfg");
		if ( thiz == NULL ) {
			log_printf(WRN, 
				"not give config-file,"
				" but no default config-file specfied alse. "
				"use all item as default.");
			return ERR_OK;
		} else {
			if ( thiz->config_type != C_STRING ) {
				int i = (thiz->config_type >= 0 && thiz->config_type <= 3)? 
					thiz->config_type : 4;
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
			log_printf(ERR, "could not read %s, exit.", thefile);
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

		log_printf(DBG, "%s", fbuf);

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
					if ( A_l >= sizeof(A) ) break;
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
					B[ B_l ] = '\n';
					break;
				}
				if ( B_l >= sizeof(B) ) break;
				
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

/*同步配置数据, 将配置数据写入配置文件*/
int config_writeout(const char *cfgfile)
{
	return ERR_ERR;
}

/*读取配置数据*/
const char *config_read(const char *name)
{
	struct config_struct *thiz;

	if ( NULL == name ) return NULL;

	thiz = config_search(name);
	if ( thiz == NULL ) return NULL;
	
	if ( thiz->config_status == C_INVALID ) {
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

	if (thiz->config_system == yes) {
		/*系统配置项不能修改*/
		return NULL;
	}
#if 0
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
			strcpy("TRUE", thiz->config_value);
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
			int i, len;

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

#if CONFIG_DEBUG_CONFIG >= 1
/*打印配置数据*/
void config_print()
{
	struct config_struct *head = configs;
	int nr = 0, i;

	for ( ; head && head->config_name && *head->config_name; head ++, nr ++ ) {
		printf("%-32s", head->config_name);

		i = (head->config_type >=0 && head->config_type <= 3)? head->config_type : 4;
		printf("  %-6s", value_type[ i ]);

		i = (head->config_system == 0 || head->config_system == 1) ? head->config_system : 2;
		printf("  %-6s", value_system[ i ]);

		i = (head->config_status >=0 && head->config_status <= 2) ? head->config_status : 3;
		printf("  %-8s", value_status[ i ]);
		
		printf("  %s\n", head->config_value);
	}
	
	if ( nr == 0 ) {
		printf("****** There is no config item.");
	}
}
#endif /*CONFIG_DEBUG_CONFIG*/
