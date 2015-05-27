#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdarg.h>
#include <malloc.h>
#include <ifaddrs.h>
#include <signal.h>
#include <netdb.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/stat.h>

static unsigned int __atoh(const char *hex)
{
    unsigned int v = 0;

    if ( !hex ) return 0;
    if ( hex[0] == '0' && (hex[1] == 'X' || hex[1] == 'x' ) )
        hex ++, hex ++;

    while ( *hex ) {
        if (*hex >= '0' && *hex <= '9') {
            v = v * 16 + (*hex) - '0';
        } else if (*hex >= 'A' && *hex <= 'F') {
            v = v * 16 + (*hex) - 'A' + 10;
        } else if (*hex >= 'a' && *hex <= 'f') {
            v = v * 16 + (*hex) - 'a' + 10;
        } else break;
        hex ++;
    }

    return v;
}
#define atoh __atoh

int main(int argc, const char *argv[])
{
	int n = 1;
	int dev = -1, retval;
	unsigned char code[1024] = {0};
	size_t len = 0;
	fd_set wrds;
    struct timeval tv;

	if ( argc < 3 ) return 1;

	for ( n =2; n < argc; n ++ ) {
		code[ len ++ ] = atoh( argv[n] );
	}

	dev = open(argv[1], O_RDWR | O_NOCTTY);
	FD_ZERO(&wrds);
	FD_SET(dev, &wrds);
	tv.tv_sec = 3;
	tv.tv_usec = 0;
	retval = select(dev+1, NULL, &wrds, NULL, &tv);
	if ( -1 == retval ) {
		goto out;
	} else if ( retval != 0 ) {
		if ( ! FD_ISSET(dev, &wrds) ) {
			goto out;
		}
	} else { // ³¬Ê±s
		goto out;
	}
	
	write(def, code, len);
out:
	close(dev);
	return 0;
}