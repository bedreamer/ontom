#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEV_NAME  "/dev/am335x_buzzer"

#define	BEEP_ON			0
#define	BEEP_OFF		1

int main(int argc, char * argv)
{
	int fd;
	int i;
	int o = 0, f = 0;

	fd = open(DEV_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("can't open %s!\n", DEV_NAME);
		exit(1);
	}
	
	while (1) {
		scanf("%d %d", &o, &f);
		for (i = 0; i < 2; i++) {
			ioctl(fd, BEEP_ON, 0);
			usleep(o);		
			ioctl(fd, BEEP_OFF, 0);
			usleep(f);
		}
	}

	close(fd);

	return 0;
}
