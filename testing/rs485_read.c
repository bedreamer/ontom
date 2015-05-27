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

int main(int argc, const char *argv[])
{
	int n = 1;
	int dev = -1, retval;
	unsigned char code[1024] = {0};
	size_t len = 0;
	fd_set rfds;
    struct timeval tv ;

	if ( argc < 2 ) return 1;

	dev = open(argv[1], O_RDWR | O_NOCTTY);
	FD_ZERO(&rfds);
	FD_SET(dev, &rfds);
	tv.tv_sec = 3;
	tv.tv_usec = 0;
	retval = select(dev+1, &rfds, NULL, NULL, &tv);
	if ( -1 == retval ) {
		goto out;
	} else if ( retval != 0 ) {
		if ( ! FD_ISSET(dev, &rfds) ) {
			goto out;
		}
	} else { // ³¬Ê±s
		goto out;
	}
	
	retval = read(def, code, 256);
	for ( i = 0; i < retval; i ++ ) {
		printf("%02X", code[ i ] );
		if ( i != retval - 1 ) {
			printf(" ");
		} else {
			printf("\n");
		}
	}
out:
	close(dev);
	return 0;
}