#ifndef ___STDAFX_H_
#define ___STDAFX_H_
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
#include <netinet/in.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <arpa/inet.h>

#include "../thirdpart/mongoose/mongoose.h"
#include "../thirdpart/sqlite/sqlite3.h"
#include "../thirdpart/extso/exso.h"

#include "error.h"

#include "../lib/lj/list.h"
#include "../lib/lj/log.h"
#include "../lib/lj/config.h"
#include "../lib/lj/Hachiko.h"
#include "../lib/bms/father.h"

#endif// ___STDAFX_H_
