#include "stdafx.h"

int log_printf(unsigned int level, const char *fmt, ...)
{
    char buff[1024]={0};
	va_list ap;
    //char *lvlstr[] = {"INF", "WRN", "ERR", "DBG"};
    const char *cfg;
    char timestamp[20] = {0}, btid[16] = {0};
    pthread_t tid;

    tid = pthread_self();
    sprintf(btid, ".%lu", tid);
	va_start(ap, fmt);
    __get_timestamp(timestamp);
    strcat(timestamp, btid);

#if 0
    sprintf(".%d", line);
    strcat(timestamp, line);
#endif
    switch (level) {
    case INF:
        vsnprintf(buff, sizeof(buff), fmt, ap);
        printf("\033[32m[INF@%u %s]\033[0m %s\n",
               (unsigned int)pthread_self(),
               timestamp,
               buff);
        break;
    case WRN:
        vsnprintf(buff, sizeof(buff), fmt, ap);
        printf("\033[33m[WRN@%u %s]\033[0m %s\n",
               (unsigned int)pthread_self(),
               timestamp,
               buff);
        break;
    case ERR:
        vsnprintf(buff, sizeof(buff), fmt, ap);
        printf("\033[31m[ERR@%u %s]\033[0m %s\n",
               (unsigned int)pthread_self(),
               timestamp,
               buff);
        break;
    case DBG:
        vsnprintf(buff, sizeof(buff), fmt, ap);
        printf("\033[34m[DBG@%u %s]\033[0m %s\n",
               (unsigned int)pthread_self(),
               timestamp,
                buff);
        break;
    case DBG_LV0:
        vsnprintf(buff, sizeof(buff), fmt, ap);
        cfg = config_read("debug_lvA");
        if ( cfg == NULL ) return 1;
        if ( strcmp(cfg, "false") == 0 ||
             strcmp(cfg, "FALSE") == 0 ) {
            return 1;
        }
        printf("\033[35m[DBG_LV0@%u %s]\033[0m %s\n",
               (unsigned int)pthread_self(),
               timestamp,
               buff);
        break;
    case DBG_LV1:
        vsnprintf(buff, sizeof(buff), fmt, ap);
        cfg = config_read("debug_lvB");
        if ( cfg == NULL ) return 1;
        if ( strcmp(cfg, "false") == 0 ||
             strcmp(cfg, "FALSE") == 0 ) {
            return 1;
        }
        printf("\033[35m[DBG_LV1@%u %s]\033[0m %s\n",
               (unsigned int)pthread_self(),
               timestamp,
               buff);
        break;
    case DBG_LV2:
        vsnprintf(buff, sizeof(buff), fmt, ap);
        cfg = config_read("debug_lvC");
        if ( cfg == NULL ) return 1;
        if ( strcmp(cfg, "false") == 0 ||
             strcmp(cfg, "FALSE") == 0 ) {
            return 1;
        }
        printf("\033[36m[DBG_LV2@%u %s]\033[0m %s\n",
               (unsigned int)pthread_self(),
               timestamp,
               buff);
        break;
    case DBG_LV3:
        vsnprintf(buff, sizeof(buff), fmt, ap);
        cfg = config_read("debug_lvD");
        if ( cfg == NULL ) return 1;
        if ( strcmp(cfg, "false") == 0 ||
             strcmp(cfg, "FALSE") == 0 ) {
            return 1;
        }
        printf("\033[36m[DBG_LV3@%u %s]\033[0m %s\n",
              (unsigned int)pthread_self(),
               timestamp,
               buff);
        break;
    default:
        //return printf("%s\n", buff);
        break;
    }
    va_end(ap);
    return 0;
}
