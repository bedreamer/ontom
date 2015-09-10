#ifndef _LIB__STDAFX_H_
#define _LIB__STDAFX_H_

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

#include "../../thirdpart/sqlite/sqlite3.h"
#include "../../ontom/error.h"
#include "../lj/list.h"
#include "../lj/config.h"
#include "../lj/Hachiko.h"
#include "../lj/log.h"
#include "../bms/father.h"
#include "../../ontom/charge.h"

#endif// _LIB__STDAFX_H_
