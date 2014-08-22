#include <stdio.h>
#include <stdarg.h>

int log_printf(unsigned int level, const char *fmt, ...)
{
	char buff[256]={0};
	va_list ap;
	char *lvlstr[] = {"INF", "WRN", "ERR", "DBG"};
	
	va_start(ap, fmt);
	vsnprintf(buff, sizeof(buff), fmt, ap);
	va_end(ap);

	if ( level <= 3 ) {
		return printf("%s->%s\n", lvlstr[level], buff);
	} else {
		return printf("%s\n", buff);
	}
}