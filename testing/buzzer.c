#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEV_NAME  "/dev/am335_buzzer"

#define	BEEP_ON			0
#define	BEEP_OFF		1

int main(int argc, char * argv)
{
	int fd;
	int i;

	fd = open(DEV_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("can't open /dev/gpio_test!\n");
		exit(1);
	}
	
	printf("buzzer test.\n");
	for (i = 0; i < 2; i++) 
	{
		ioctl(fd, BEEP_ON, 0);
		sleep(1);		
		ioctl(fd, BEEP_OFF, 0);
		sleep(1);
	}

	close(fd);

	return 0;
}
