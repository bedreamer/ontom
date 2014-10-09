#ifndef LOG__INCLUDED_H
#define LOG__INCLUDED_H
#define INF  0xFF
#define WRN  0xFE
#define ERR  0xFD
#define DBG  0xFC

//  分类调试信息
#define DBG_LV0 0x00
#define DBG_LV1 0x01
#define DBG_LV2 0x02
#define DBG_LV3 0x03

int log_printf(unsigned int level, const char *fmt, ...);

#endif
