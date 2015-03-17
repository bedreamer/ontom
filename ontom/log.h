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

int _log_printf(size_t line, unsigned int level, const char *fmt, ...);
#define GRN(s) "\033[32m"s"\033[0m"
#define RED(s) "\033[31m"s"\033[0m"
#define YEL(s) "\033[33m"s"\033[0m"
#define debug_log log_printf
static inline __log_printf(int l, const char * f)
{
    _log_printf(__LINE__, DBG, "%s:%d tracked.", f, l);
}

#define debug_track() __log_printf(__LINE__, __FILE__)

#define log_printf(lv, fmt, ...) _log_printf(__LINE__, lv, fmt, __VA_ARGS__)

#endif
