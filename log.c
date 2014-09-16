#include <stdio.h>
#include <stdarg.h>
#include "log.h"
#include "config.h"

int log_printf(unsigned int level, const char *fmt, ...)
{
	char buff[256]={0};
	va_list ap;
	char *lvlstr[] = {"INF", "WRN", "ERR", "DBG"};

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
    default:
        return printf("%s\n", buff);
        break;
    }
}
