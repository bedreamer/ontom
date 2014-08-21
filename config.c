#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "config.h"
#include "error.h"
#include "log.h"

//{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{
CONFIG_DOMAIN_BEGIN
//数据名称      |数据类型|存于文件?|nnn数据状态|数据默认值|		数据值
// ! 下面这一项需要在初始化时手动设置
{"workingroot", C_STRING,	no,		C_VALID,	.cuv.n=0,		{"/usr/bin/"}},
{"default_cfg", C_STRING,	no,		C_VALID,	.cuv.n=0,		{"ontom.cfg"}},
{"xmlsrvport",     C_INT,	yes,	C_INVALID,	.cuv.i=9989,	{"9989"}},
{"version_xml", C_STRING,	yes,	C_INVALID,	.cuv.n=0,		{"version.xml"}},
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
	return ERR_OK;
}

/*同步配置数据, 将配置数据写入配置文件*/
int config_writeout(const char *cfgfile)
{
	return ERR_OK;
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

	if ( NULL == name ) return NULL;

	thiz = config_search(name);
	if ( thiz == NULL ) return NULL;

	if (thiz->config_exsit == yes) {
		/*系统配置项不能修改*/
		return NULL;
	}

	if ( C_INVALID == thiz->config_status ) {
		/*无效的配置项不能修改*/
		return NULL;
	}

	len = strlen(value);
	if ( C_STRING == thiz->config_type ) {
		if ( len > CONFIG_MAX_VALUE ) {
			memcpy(thiz->config_value, value, CONFIG_MAX_VALUE - 1);
			thiz->config_value[CONFIG_MAX_VALUE-1] = '\0';
		} else  if ( len == 0 ) {
			memset(thiz->config_value, 0, sizeof(thiz->config_value));
		} else {
			strcpy(thiz->config_value, value);
		}
	} else if ( C_BOOL == thiz->config_type ) {
		if ( 0 == strcmp("false", value) ||
			 0 == strcmp("FALSE", value) ) {
			strcpy(thiz->config_value, "FALSE");
		} else if ( 0 == strcmp("true", value) ||
					0 == strcmp("TRUE", value) ) {
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
			memcpy(thiz->config_value, value, len);
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
			memcpy(thiz->config_value, value, len);
		 }
	} else {
		/*数据崩溃*/
		log_printf(ERR, "unexpected exception @%s:%d\n", __FILE__, __LINE__);
		return NULL;
	}

	if ( C_VALID == thiz->config_status ) {
		thiz->config_status = C_MODIFY;
	}

	return thiz->config_value;
}

#if CONFIG_DEBUG_CONFIG >= 1
/*打印配置数据*/
void config_print()
{
	struct config_struct *head = configs;
	int nr = 0, i;
	char *value_type[] = {"STRING", "INT", "FLOAT", "BOOL", "N/A"};
	char *value_system[] = {"NO", "YES", "N/A"};
	char *value_status[] = {"INVALID", "VALID", "MODIFY", "N/A"};

	for ( ; head && head->config_name; head ++, nr ++ ) {
		printf("%16s", head->config_name);

		i = (head->config_type >=0 && head->config_type <= 3)? head->config_type : 4;
		printf("  %6s", value_type[ i ]);

		i = (head->config_exsit == 0 || head->config_exsit == 1) ? head->config_exsit : 2;
		printf("  %6s", value_system[ i ]);

		i = (head->config_status >=0 && head->config_status <= 2) ? head->config_status : 3;
		printf("  %6s", value_status[ i ]);
		
		printf("  %s", head->config_value);
	}
	
	if ( nr == 0 ) {
		printf("****** There is no config item.\n");
	}
}
#endif /*CONFIG_DEBUG_CONFIG*/
