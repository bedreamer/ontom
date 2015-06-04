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
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <arpa/inet.h>

#include "../../ontom/error.h"
#include "../../thirdpart/sqlite/sqlite3.h"
#include "../../thirdpart/extso/exso.h"
#include "../../lib/lj/stdafx.h"
#include "../../lib/bms/father.h"
#include "../../ontom/charge.h"

#define DEV_NAME  "/dev/am335x_buzzer"
#define	BEEP_ON			0
#define	BEEP_OFF		1

#include "buzzer.h"

struct charge_task * zeus = NULL;
void * buzzer_thread_proc(void *p);

int exso_buzzer_init(void *p)
{
    int ret;
    zeus = (struct charge_task *)p;
    printf("******  Plugin  buzzer  initialized  ******\n");
    ret = pthread_create( & zeus->tid, &zeus->attr, buzzer_thread_proc, NULL);
    if ( 0 != ret ) {
        log_printf(ERR,
                   "buzzer thread framework start up.        FAILE!!!!");
    }
    return ERR_OK;
}

int exso_buzzer_exit(void *p)
{
    printf("******  Plugin  default  exited  ******\n");
    return ERR_OK;
}

void * buzzer_thread_proc(void *p)
{
    int fd;
    int i;

    fd = open(DEV_NAME, O_RDWR);
    if (fd < 0)
    {
        log_printf(ERR, "BUZZER: can't open %s!\n", DEV_NAME);
        return NULL;
    }

    log_printf(INF, "buzzer test running.");
    for (i = 0; i < 2; i++)
    {
        ioctl(fd, BEEP_ON, 0);
        sleep(1);
        ioctl(fd, BEEP_OFF, 0);
        sleep(1);
    }

    close(fd);

    return NULL;
}
