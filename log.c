#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include "log.h"
#include "config.h"

int log_printf(unsigned int level, const char *fmt, ...)
{
	char buff[256]={0};
	va_list ap;
	char *lvlstr[] = {"INF", "WRN", "ERR", "DBG"};
    const char *cfg;
    time_t timep;
    struct tm *p;

    time(&timep);
    p =localtime(&timep);
	va_start(ap, fmt);
	vsnprintf(buff, sizeof(buff), fmt, ap);
	va_end(ap);

    switch (level) {
    case INF:
        printf("\033[32m[INF@%d %04d-%02d-%02d %02d:%02d:%02d]\033[0m %s\n",
               gettid(),
               p->tm_year + 1990,
               p->tm_mon,
               p->tm_mday,
               p->tm_hour,
               p->tm_min,
               p->tm_sec,
               buff);
        break;
    case WRN:
        printf("\033[33m[WRN@%d %04d-%02d-%02d %02d:%02d:%02d]\033[0m %s\n",
               gettid(),
               p->tm_year + 1990,
               p->tm_mon,
               p->tm_mday,
               p->tm_hour,
               p->tm_min,
               p->tm_sec,
               buff);
        break;
    case ERR:
        printf("\033[31m[ERR@%d %04d-%02d-%02d %02d:%02d:%02d]\033[0m %s\n",
               p->tm_year + 1990,
               p->tm_mon,
               p->tm_mday,
               p->tm_hour,
               p->tm_min,
               p->tm_sec,
               buff);
        break;
    case DBG:
        printf("\033[34m[DBG@%d %04d-%02d-%02d %02d:%02d:%02d]\033[0m %s\n",
               gettid(),
               p->tm_year + 1990,
               p->tm_mon,
               p->tm_mday,
               p->tm_hour,
               p->tm_min,
               p->tm_sec,
                buff);
        break;
    case DBG_LV0:
        cfg = config_read("debug_lvA");
        if ( cfg == NULL ) return;
        if ( strcmp(cfg, "false") == 0 ||
             strcmp(cfg, "FALSE") == 0 ) {
            return;
        }
        printf("\033[35m[DBG_LV0@%d %04d-%02d-%02d %02d:%02d:%02d]\033[0m %s\n",
               gettid(),
               p->tm_year + 1990,
               p->tm_mon,
               p->tm_mday,
               p->tm_hour,
               p->tm_min,
               p->tm_sec,
               buff);
        break;
    case DBG_LV1:
        cfg = config_read("debug_lvB");
        if ( cfg == NULL ) return;
        if ( strcmp(cfg, "false") == 0 ||
             strcmp(cfg, "FALSE") == 0 ) {
            return;
        }
        printf("\033[35m[DBG_LV1@%d %04d-%02d-%02d %02d:%02d:%02d]\033[0m %s\n",
               gettid(),
               p->tm_year + 1990,
               p->tm_mon,
               p->tm_mday,
               p->tm_hour,
               p->tm_min,
               p->tm_sec,
               buff);
        break;
    case DBG_LV2:
        cfg = config_read("debug_lvC");
        if ( cfg == NULL ) return;
        if ( strcmp(cfg, "false") == 0 ||
             strcmp(cfg, "FALSE") == 0 ) {
            return;
        }
        printf("\033[36m[DBG_LV2@%d %04d-%02d-%02d %02d:%02d:%02d]\033[0m %s\n",
               gettid(),
               p->tm_year + 1990,
               p->tm_mon,
               p->tm_mday,
               p->tm_hour,
               p->tm_min,
               p->tm_sec,
               buff);
        break;
    case DBG_LV3:
        cfg = config_read("debug_lvD");
        if ( cfg == NULL ) return;
        if ( strcmp(cfg, "false") == 0 ||
             strcmp(cfg, "FALSE") == 0 ) {
            return;
        }
        printf("\033[36m[DBG_LV3@%d %04d-%02d-%02d %02d:%02d:%02d]\033[0m %s\n",
               gettid(),
               p->tm_year + 1990,
               p->tm_mon,
               p->tm_mday,
               p->tm_hour,
               p->tm_min,
               p->tm_sec,
               buff);
        break;
    default:
        //return printf("%s\n", buff);
        break;
    }
}
