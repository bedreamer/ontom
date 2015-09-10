#include "stdafx.h"

int log_printf(unsigned int level, const char *fmt, ...)
{
    char buff[1024]={0};
	va_list ap;
    //char *lvlstr[] = {"INF", "WRN", "ERR", "DBG"};
    const char *cfg;
    char timestamp[20] = {0};

	va_start(ap, fmt);
    __get_timestamp(timestamp);

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

#define MAX_LOG_LEN  512
char **log_pool = NULL;
unsigned int max_records = 0;
unsigned int sync_triger_nr = 0;
pthread_mutex_t log_pool_lck;
unsigned int records_nr = 0;
sqlite3 *database = NULL;

// 系统操作日志初始化
int initlize_system_log(sqlite3 *db, unsigned int maxlog, unsigned int sync_triger)
{
    unsigned int i;
    char *pool_base;
    size_t pool_size = sizeof(char *)*maxlog + (MAX_LOG_LEN+1)*maxlog;

    if ( maxlog == 0 ) return ERR_WRONG_PARAM;
    if ( sync_triger > maxlog ) return ERR_WRONG_PARAM;
    if ( db == NULL ) return ERR_WRONG_PARAM;

    log_pool = (char **)malloc(pool_size);
    if ( log_pool == NULL ) {
        log_printf(ERR, "LOG: 初始化系统操作日志失败.");
        return ERR_ERR;
    }
    memset(log_pool, 0, pool_size);
    pool_base = (char *)log_pool + sizeof(char *)*maxlog;

    for ( i = 0; i < maxlog; i ++ ) {
        log_pool[i] = pool_base + i * (MAX_LOG_LEN + 1);
    }
    pthread_mutex_init(&log_pool_lck, NULL);
    max_records = maxlog;
    sync_triger_nr = sync_triger;
    database = db;

    log_printf(INF, "LOG: 系统日志初始化乘成功，大小: %d 条， 同步触发 %d 条",
               maxlog, sync_triger);

    return ERR_OK;
}

/* 记录系统操作日志 */
int system_log(unsigned short type, const char *fmt, ...)
{
    char log[512] = {0};
    va_list ap;
    char tstp[64] = {0};

    if ( log_pool == NULL || database == NULL ) return ERR_ERR;

    if ( records_nr >= sync_triger_nr && records_nr > 0 ) {
        sync_system_log();
    }

    __get_timestamp(tstp);
    va_start(ap, fmt);
    vsnprintf(log, MAX_LOG_LEN, fmt, ap);
    va_end(ap);

    pthread_mutex_lock(&log_pool_lck);
    sprintf(log_pool[ records_nr ++ ], "%s # %s", tstp, log);
    pthread_mutex_unlock(&log_pool_lck);

    return ERR_OK;
}

/* 将系统日志写入数据库操作日志缓冲区 */
int sync_system_log()
{
    if ( log_pool == NULL || database == NULL ) return ERR_ERR;

    if ( records_nr ) {
        unsigned int i = 0, j;
        char tstp[64];
        char *l = NULL;
        char sql[1024] = {0};

        pthread_mutex_lock(&log_pool_lck);
        for ( i = 0; i < records_nr ; i ++ ) {
            j = 0;
            while ( log_pool[i][j] != '#' && j < 64 ) {
                tstp[j] = log_pool[i][j];
                j ++;
            }
            if ( j < 64 ) {
                log_pool[i][j - 1] = '\0';
                j ++;
                l = &log_pool[i][j];
            } else continue;

            sprintf(sql, "INSERT INTO log VALUES(\'%s\',\'%s\')", tstp, l);
            i = sqlite3_exec(database, sql, NULL, NULL, &l);
            if ( i != 0 ) {
                log_printf(ERR, "LOG: sync system log failes <%s>", l);
                goto faile;
            }
            records_nr = 0;
        }
faile:
        pthread_mutex_unlock(&log_pool_lck);

        return ERR_OK;
    }
    return ERR_ERR;
}
