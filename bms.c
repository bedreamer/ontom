#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
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
#if 0
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
#endif
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

    log_printf(INF, "%s running...s=%d", __FUINCTON__, s);
    while ( ! *done ) {
        usleep(90000);

        if ( task->can_bms_status == CAN_INVALID ) {
            continue;
        }

        nbytes = write(s, &frame, sizeof(struct can_frame));
        if ( nbytes != sizeof(struct can_frame) ) {
            continue;
        }
        if ( frame.can_id & 0xFFFF != CAN_RCV_ID_MASK ) {
            continue;
        }

        /*
         * SAE J1939-21 Revised December 2006 版协议中规定
         * TP.DT.PGN 为 0x00EB00  连接管理
         * TP.CM.PGN 为 0x00EC00  数据传输
         * CAN 的数据包大小最大为8字节，因此需要传输大于等于9字节的数据包
         * 时需要使用连接管理功能来传输数据包。
         * 首先由数据发送方，发送一个数据发送请求包，请求包中包含的数据内容有
         * 消息的总长度，需要发送的数据包个数（必须大于1），最大的数据包编号，
         * 这个消息的PGN等
         */
        if ( frame.can_id & 0x00FF0000 == 0xEB ) {
            // Data transfer
        } else if ( frame.can_id & 0x00FF0000 == 0xEC ) {
            // Connection managment
        } else {

        }
#if 0
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
#endif
    }
}
