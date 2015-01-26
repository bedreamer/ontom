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

int __log_printf(unsigned int level, unsigned int line, const char *fmt, ...);
#define log_printf(lv, fmt, ...) __log_printf(lv, line, fmt, __VA_ARGS__)
#define GRN(s) "\033[32m"s"\033[0m"
#define RED(s) "\033[31m"s"\033[0m"
#define YEL(s) "\033[33m"s"\033[0m"
#define debug_log __log_printf

#endif
