#ifndef LOG__INCLUDED_H
#define LOG__INCLUDED_H
#define INF  0xFF
#define WRN  0xFE
#define ERR  0xFD
#define DBG  0xFC

//  分类调试信息
// 输出速度最快
#define DBG_LV0 0x00
#define DBG_LV1 0x01
#define DBG_LV2 0x02
// 输出速度最慢
#define DBG_LV3 0x03

int log_printf(unsigned int level, const char *fmt, ...);
#define GRN(s) "\033[32m"s"\033[0m"
#define RED(s) "\033[31m"s"\033[0m"
#define YEL(s) "\033[33m"s"\033[0m"
#define debug_log log_printf

static inline void __get_timestamp(char *outstring)
{
    time_t timep;
    int n;
    struct tm *p;
    timep = time(NULL);
    p =localtime(&timep);
    struct timeval t_start;
    gettimeofday(&t_start, NULL);
    n = sprintf(outstring, "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
            p->tm_year + 1900,
            p->tm_mon + 1,
            p->tm_mday,
            p->tm_hour,
            p->tm_min,
            p->tm_sec,
            t_start.tv_usec);
    outstring[ n ] = 0;
}

#endif
