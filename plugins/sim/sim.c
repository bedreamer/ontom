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
#include "libtcc.h"

struct charge_task * zeus = NULL;
typedef unsigned int item_t;
struct item_struct {
    item_t item_id;       // ID
    const char *numeric;  // 助记符
    double value;         // 值
};

// 模拟值
struct item_struct items_table[] = {
    {1, "UNDEF", 0.0f},
    {2, "UNDEF", 0.0f},
    {1, "UNDEF", 0.0f},
    {2, "UNDEF", 0.0f},
    {1, "UNDEF", 0.0f},
    {2, "UNDEF", 0.0f},
    {1, "UNDEF", 0.0f},
    {2, "UNDEF", 0.0f},
    {0, "UNDEF", 0.0f}
};

int exso_sim_init(void *p)
{
    zeus = (struct charge_task *)p;
    printf("******  Plugin  sim  initialized  ******\n");
    return ERR_OK;
}

int exso_sim_main_loop(void *p)
{
    return ERR_OK;
}

int exso_sim_exit(void *p)
{
    printf("******  Plugin  sim exited  ******\n");
    return ERR_OK;
}

