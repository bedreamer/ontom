#include <stdio.h>
#include <stdarg.h>
#include "log.h"
#include "config.h"

int log_printf(unsigned int level, const char *fmt, ...)
{
	char buff[256]={0};
	va_list ap;
	char *lvlstr[] = {"INF", "WRN", "ERR", "DBG"};
    const char *cfg;

	va_start(ap, fmt);
	vsnprintf(buff, sizeof(buff), fmt, ap);
	va_end(ap);

    switch (level) {
    case INF:
        printf("\033[32m[INF]\033[0m %s\n", buff);
        break;
    case WRN:
        printf("\033[33m[WRN]\033[0m %s\n", buff);
        break;
    case ERR:
        printf("\033[31m[ERR]\033[0m %s\n", buff);
        break;
    case DBG:
        printf("\033[34m[DBG]\033[0m %s\n", buff);
        break;
    case DBG_LV0:
        cfg = config_read("debug_lvA");
        if ( cfg == NULL ) return;
        if ( strcmp(cfg, "false") == 0 ||
             strcmp(cfg, "FALSE") == 0 ) {
            return;
        }
        printf("\033[34m[DBG_LV0]\033[0m %s\n", buff);
        break;
    case DBG_LV1:
        cfg = config_read("debug_lvB");
        if ( cfg == NULL ) return;
        if ( strcmp(cfg, "false") == 0 ||
             strcmp(cfg, "FALSE") == 0 ) {
            return;
        }
        printf("\033[34m[DBG_LV1]\033[0m %s\n", buff);
        break;
    case DBG_LV2:
        cfg = config_read("debug_lvC");
        if ( cfg == NULL ) return;
        if ( strcmp(cfg, "false") == 0 ||
             strcmp(cfg, "FALSE") == 0 ) {
            return;
        }
        printf("\033[34m[DBG_LV2]\033[0m %s\n", buff);
        break;
    case DBG_LV3:
        cfg = config_read("debug_lvD");
        if ( cfg == NULL ) return;
        if ( strcmp(cfg, "false") == 0 ||
             strcmp(cfg, "FALSE") == 0 ) {
            return;
        }
        printf("\033[34m[DBG_LV3]\033[0m %s\n", buff);
        break;
    default:
        //return printf("%s\n", buff);
        break;
    }
}
