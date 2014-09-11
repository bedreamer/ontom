#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "bms.h"
#include "charge.h"
#include "config.h"
#include "log.h"
#include "error.h"

// bms 通信 写 服务线程
// 提供bms通信服务
void *thread_bms_write_service(void *arg) ___THREAD_ENTRY___
{
    int *done = (int *)arg;
    int mydone = 0;
    if ( done == NULL ) done = &mydone;

    int s, ti = 0;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    int nbytes;

    strcpy(ifr.ifr_name, "can0" );
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    log_printf(INF, "%s RX:TX = %X:%X",
               __FUNCTION__,
               addr.can_addr.tp.rx_id,
               addr.can_addr.tp.tx_id);

    bind(s, (struct sockaddr *)&addr, sizeof(addr));

    log_printf(INF, "%s running...s=%d", __FUNCTION__, s);
    while ( ! *done ) {
        frame.can_id  = 0x188 | CAN_EFF_FLAG;
        frame.can_dlc = 5;
        frame.data[0] = 0x11;
        frame.data[1] = 0x11;
        frame.data[2] = 0x11;
        frame.data[3] = 0x11;
        frame.data[4] = 0x11;
        frame.data[5] = 0x11;

        nbytes = write(s, &frame, sizeof(struct can_frame));

        log_printf(DBG, "TX%d---%X:%d %02X %02X %02X %02X %02X %02X %02X %02X",
                    ti++,
                    frame.can_id,
                    nbytes,
                    frame.data[0],
                    frame.data[1],
                    frame.data[2],
                    frame.data[3],
                    frame.data[4],
                    frame.data[5],
                    frame.data[6],
                    frame.data[7]
                );
        usleep(90000);
    }
}

// bms 通信 读 服务线程
// 提供bms通信服务
void *thread_bms_read_service(void *arg) ___THREAD_ENTRY___
{
    int *done = (int *)arg;
    int mydone = 0;
    if ( done == NULL ) done = &mydone;

    int s, ti = 0;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    int nbytes;

    strcpy(ifr.ifr_name, "can0" );
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    log_printf(INF, "%s RX:TX = %X:%X", __FUNCTION__, addr.can_addr.tp.rx_id,
               addr.can_addr.tp.tx_id);

    bind(s, (struct sockaddr *)&addr, sizeof(addr));

    log_printf(INF, "%s running...s=%d", __FUNCTION__, s);
    while ( ! *done ) {
        frame.can_id  = 0x133 | CAN_EFF_FLAG;
        frame.can_dlc = 2;
        frame.data[0] = 0x02;
        frame.data[1] = 0x02;

        nbytes = write(s, &frame, sizeof(struct can_frame));

        log_printf(DBG, "TX%d---%X:%d %02X %02X %02X %02X %02X %02X %02X %02X",
                    ti++,
                    frame.can_id,
                    nbytes,
                    frame.data[0],
                    frame.data[1],
                    frame.data[2],
                    frame.data[3],
                    frame.data[4],
                    frame.data[5],
                    frame.data[6],
                    frame.data[7]
                );
        usleep(90000);
    }
}
