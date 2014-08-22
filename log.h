#ifndef LOG__INCLUDED_H
#define LOG__INCLUDED_H
#define INF  0
#define WRN  1
#define ERR  2
#define DBG  3
int log_printf(unsigned int level, const char *fmt, ...);

#endif