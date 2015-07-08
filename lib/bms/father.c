#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dlfcn.h>
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
#include "father.h"
#include "../../ontom/charge.h"

// bms 通信 写 服务线程
// 提供bms通信服务
void *thread_bms_write_service(void *arg) ___THREAD_ENTRY___
{
    int *done = (int *)arg;
    int mydone = 0;
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    unsigned char txbuff[32];
    int nbytes;
    struct charge_task *tsk = (struct charge_task *)arg;
    struct bmsdriver *driver;
    struct charge_job *thiz;
    int keep_alive = 0;

    if ( done == NULL ) done = &mydone;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if ( s == -1 ) {
        log_printf(ERR, "BMSDRV: 打开CAN失败!");
    }

    strcpy(ifr.ifr_name, "can0" );
    if ( 0 != ioctl(s, SIOCGIFINDEX, &ifr) ) {
        log_printf(ERR, "BMSDRV: 配置CAN 失败.");
    }

    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if ( 0 != bind(s, (struct sockaddr *)&addr, sizeof(addr)) ) {
        log_printf(ERR, "BMSDRV: 绑定CAN 失败.");
    }

    log_printf(INF, "BMS: %s running...s=%d", __FUNCTION__, s);

#if 1
    while ( 1 ) {
        usleep(5000);
        // 轮询所有的驱动
        for ( driver = tsk->bmsdriver; driver ; driver = driver->next ) {
            unsigned int i;
            for ( i = 0; driver->binder[i][1] &&
                  i < sizeof(driver->binder)/(2*sizeof(struct charge_job *)); i ++ ) {
                thiz = driver->binder[i][1];
                if ( thiz->job_status == JOB_DETACHING ) {
                    log_printf(INF, "JOB exited, unbind BMS.write driver automatic!");
                    driver->binder[i][1] = NULL;
                    thiz->bms.bms_write_init_ok = 0xFF;
                    continue;
                }

                if ( 0x7F != thiz->bms.bms_write_init_ok ) {
                    // 进行数据结构的初始化操作
                    driver->driver_main_proc(thiz, EVENT_CAN_INIT, &thiz->param, driver);
                    thiz->bms.bms_write_init_ok = 0x7f;
                    thiz->ref_nr ++;
                }

                /*
                 * 写线程同时负责写数据和进行连接管理时的控制数据写出，这里需要对当前CAN的
                 * 状态进行判定，当CAN处于CAN_NORMAL时进行普通的写操作，当CAN处于CAN_TP_RD
                 * 时，采用EVENT_TX_REQUEST 当CAN处于CAN_TP_RD时采用EVENT_TX_TP_REQUEST
                 */
                thiz->param.buff_size = sizeof(thiz->param.buff.tx_buff);
                thiz->param.evt_param = EVT_RET_INVALID;
                if ( thiz->bms.can_bms_status & CAN_NORMAL ) {
                    driver->driver_main_proc(thiz, EVENT_TX_REQUEST, &thiz->param, driver);
                } else if ( thiz->bms.can_bms_status & CAN_TP_RD ) {
                    switch ( thiz->bms.can_bms_status & 0xF0 ) {
                    case CAN_TP_CTS:
                        driver->driver_main_proc(thiz, EVENT_TX_TP_CTS, &thiz->param, driver);
                        break;
                    case CAN_TP_TX:
                    case CAN_TP_RX:
                        break;
                    case CAN_TP_ACK:
                        driver->driver_main_proc(thiz, EVENT_TX_TP_ACK, &thiz->param, driver);
                        break;
                    case CAN_TP_ABRT:
                        driver->driver_main_proc(thiz, EVENT_TX_TP_ABRT, &thiz->param, driver);
                        break;
                    default:
                        log_printf(WRN, "BMS: can_bms_status crashed(%d).",
                                   thiz->bms.can_bms_status);
                        continue;
                        break;
                    }
                } else if ( thiz->bms.can_bms_status & CAN_TP_WR ) {
                    // 当前协议没有用到
                    log_printf(WRN, "BMS: CAN_TP_WRITE not implement.");
                    continue;
                } else if ( thiz->bms.can_bms_status == CAN_INVALID ) {
                    log_printf(DBG_LV0, "BMS: invalid can_bms_status: %d.",
                               thiz->bms.can_bms_status);
                    continue;
                } else {
                    log_printf(DBG_LV0, "BMS: invalid can_bms_status: %d.",
                               thiz->bms.can_bms_status);
                    continue;
                }

                keep_alive = 0;
                if ( EVT_RET_OK != thiz->param.evt_param ) {
                    if ( thiz->bms.can_bms_status & CAN_NORMAL != CAN_NORMAL ) {
                        driver->driver_main_proc(thiz, EVENT_TX_REQUEST, &thiz->param, driver);
                        if ( EVT_RET_OK == thiz->param.evt_param ) {
                            keep_alive = 1;
                        } else continue;
                    } else {
                        continue;
                    }
                }

                thiz->param.evt_param = EVT_RET_INVALID;
                // 链接模式下的数据包发送不需要确认, 并且也不能被中止
                if ( thiz->bms.can_bms_status == CAN_NORMAL || keep_alive ) {
                    driver->driver_main_proc(thiz, EVENT_TX_PRE, &thiz->param, driver);
                    if ( EVT_RET_TX_ABORT == thiz->param.evt_param ) {
                        // packet sent abort.
                        continue;
                    } else if ( EVT_RET_OK != thiz->param.evt_param ) {
                        continue;
                    } else {
                        // confirm to send.
                    }
                }

                /* 根据GB/T 27930-2011 中相关规定，充电机向BMS发送数据包都没有超过
                 * 8字节限制，因此这里不用进行连接管理通信。
                 */
                if ( thiz->param.buff_payload <= 8 && thiz->param.buff_payload > 0 ) {
                    frame.can_id = thiz->param.can_id;
                    frame.can_dlc= thiz->param.buff_payload;
                    memcpy(frame.data, thiz->param.buff.tx_buff, 8);
                    nbytes = write(s, &frame, sizeof(struct can_frame));
                    if ( (unsigned int)nbytes < thiz->param.buff_payload ) {
                        thiz->param.evt_param = EVT_RET_ERR;
                        driver->driver_main_proc(thiz, EVENT_TX_FAILS, &thiz->param, driver);
                    } else {
                        thiz->bms.frame_tx ++;
                        thiz->bms.tx_seed ++;

                        thiz->param.evt_param = EVT_RET_OK;
                        driver->driver_main_proc(thiz, EVENT_TX_DONE, &thiz->param, driver);
                    }
                } else if ( thiz->param.buff_payload > 8 ) {
                    // 大于8字节的数据包在这里处理，程序向后兼容
                    static unsigned int notimplement = 0;
                    if ( notimplement % 10000 == 0 ) {
                        log_printf(INF,
                                   "BMS: Connection manage for send has not implemented.");
                    }
                    notimplement ++;
                } else {
                }

                thiz->param.buff_payload = 0;
                if ( thiz->bms.frame_speed_rx < thiz->bms.frame_speed_tx &&
                     thiz->bms.frame_tx > 0 && thiz->bms.frame_speed_rx <= 0 ) {
                    log_printf(INF,
                               "BMS.DRV: CAN device dizzy, reconfig manual，Vrx: %d, Vtx: %d.",
                               thiz->bms.frame_speed_rx,
                               thiz->bms.frame_speed_tx);
                    thiz->bms.frame_speed_tx = 0;
                    thiz->bms.frame_tx = 0;
                    system("ip link set can0 down;");
                    system("ip link set can0 type can bitrate 250000;");
                    system("ip link set can0 up;");
                }

                // 准备接收完成
                if ( thiz->bms.can_bms_status == (CAN_TP_RD | CAN_TP_CTS) ) {
                    thiz->bms.can_bms_status = (CAN_TP_RD | CAN_TP_RX);
                    log_printf(DBG_LV3, "BMS: ready for data transfer.");
                }
                // 应答结束
                if ( thiz->bms.can_bms_status == (CAN_TP_RD | CAN_TP_ACK) ) {
                    thiz->bms.can_bms_status = CAN_NORMAL;
                    log_printf(DBG_LV0, "BMS: connection closed normally.");
                }
                // 传输终止
                if ( thiz->bms.can_bms_status == (CAN_TP_RD | CAN_TP_ABRT) ) {
                    thiz->bms.can_bms_status = CAN_NORMAL;
                    log_printf(DBG_LV2, "BMS: connection aborted.");
                }

            }
        }
    }
#endif
    return NULL;
}


#if 1
// CAN 数据发送报文
void Hachiko_CAN_TP_notify_proc(Hachiko_EVT evt, void *_private,
                            const struct Hachiko_food *self)
{
    if ( evt == HACHIKO_TIMEOUT ) {
        struct charge_job * thiz = (struct charge_job *)_private;
        log_printf(WRN, "BMS: CAN data transfer terminal due to time out.");
        thiz->bms.can_bms_status = CAN_NORMAL;
    } else if ( evt == HACHIKO_DIE ) {

    }
}

void dump_can_pack(struct can_frame *f)
{
}

// bms 通信 读 服务线程
// 提供bms通信服务
void *thread_bms_read_service(void *arg) ___THREAD_ENTRY___
{
    int *done = (int *)arg;
    int mydone = 0;
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    int nbytes;
    // 用于链接管理的数据缓冲
    unsigned char tp_buff[2048];
    struct charge_task *tsk = (struct charge_task *)arg;
    struct bmsdriver *driver;
    struct charge_job *thiz;
    fd_set rfds;
    struct timeval tv ;
    int retval;

    // 数据包个数
    unsigned int tp_packets_nr = 0;
    // 数据包字节总数
    unsigned int tp_packets_size = 0;
    // 数据包对应的PGN
    unsigned int tp_packet_PGN = 0;

    unsigned int dbg_packets = 0;

    if ( done == NULL ) done = &mydone;
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if ( s == -1 ) {
        log_printf(ERR, "打开CAN失败!");
    }
    if ( -1 == fcntl(s, F_SETFL, FASYNC) ) {
        log_printf(ERR, "BMS.read: set read socketCAN to ASYNC failed!");
    }
    strcpy(ifr.ifr_name, "can0" );
    if ( 0 != ioctl(s, SIOCGIFINDEX, &ifr) ) {
        log_printf(ERR, "BMSDRV: 配置CAN失败.");
    }
    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(s, (struct sockaddr *)&addr, sizeof(addr));
    log_printf(INF, "BMS: %s running...s=%d", __FUNCTION__, s);

    while ( 1 ) {
        usleep(5000);
        // 轮询所有的驱动
        for ( driver = tsk->bmsdriver; driver ; driver = driver->next ) {
            unsigned int i;
            for ( i = 0; driver->binder[i][0] &&
                  i < sizeof(driver->binder)/(2*sizeof(struct charge_job *)); i ++ ) {
                thiz = driver->binder[i][0];
                if ( thiz->job_status == JOB_DETACHING ) {
                    log_printf(INF, "JOB exited, unbind BMS driver.read automatic!");
                    thiz->bms.bms_read_init_ok = 0xFF;
                    Hachiko_kill(&thiz->bms.can_tp_bomb);
                    Hachiko_kill(&thiz->bms.can_heart_beat);
                    driver->binder[i][0] = NULL;
                    continue;
                }

                if ( thiz->bms.can_bms_status  == CAN_INVALID ) {
                    continue;
                }

                if ( thiz->bms.bms_read_init_ok != 0x7f ) {
                    thiz->bms.bms_read_init_ok = 0x7F;
                    thiz->ref_nr ++;
                    thiz->bms.can_tp_bomb.status = HACHIKO_INVALID;
                }

                FD_ZERO(&rfds);
                FD_SET(s, &rfds);
                tv.tv_sec = 0;
                tv.tv_usec = 500;
                retval = select(s+1, &rfds, NULL, NULL, &tv);
                if (  -1 == retval ) {
                    continue;
                } else if ( 0 != retval ) {
                    if ( ! FD_ISSET(s, &rfds) ) {
                        continue;
                    }
                } else {
                    // 超 时
                    continue;
                }
                memset(&frame, 0, sizeof(frame));
                nbytes = read(s, &frame, sizeof(struct can_frame));
                if ( nbytes == sizeof(struct can_frame) ) {
                    thiz->bms.frame_rx ++;
                    thiz->bms.rx_seed ++;

                    if ( thiz->bms.frame_rx < thiz->bms.frame_tx ) {

                    }
                }
                if ( (frame.can_id & 0xFFFF) != CAN_RCV_ID_MASK ) {
                    #if 0
                    log_printf(DBG_LV0, "BMS: id not accept %x", frame.can_id);
                    #endif
                    if ( (frame.can_id & 0xFFFF) == CAN_TX_ID_MASK ) {
                        //driver->driver_main_proc(thiz, EVENT_TX_DONE, &thiz->param, driver);
                    }
                    continue;
                }

                dbg_packets ++;
                /*
                 * 非阻塞方式读
                 */

                if ( nbytes != sizeof(struct can_frame) ) {
                    thiz->param.evt_param = EVT_RET_ERR;
                    log_printf(DBG_LV3, "BMS: read frame error %x", frame.can_id);
                    driver->driver_main_proc(thiz, EVENT_RX_ERROR, &thiz->param, driver);
                    continue;
                }

                debug_log(DBG_LV2,
                           "BMS: get %dst packet %08X:%02X%02X%02X%02X%02X%02X%02X%02X",
                           dbg_packets,
                           frame.can_id,
                           frame.data[0], frame.data[1], frame.data[2], frame.data[3],
                           frame.data[4], frame.data[5], frame.data[6], frame.data[7]);

                /*
                 * CAN通信处于普通模式
                 *
                 * SAE J1939-21 Revised December 2006 版协议中规定
                 * TP.DT.PGN 为 0x00EB00  连接管理
                 * TP.CM.PGN 为 0x00EC00  数据传输
                 * CAN 的数据包大小最大为8字节，因此需要传输大于等于9字节的数据包
                 * 时需要使用连接管理功能来传输数据包。
                 * 首先由数据发送方，发送一个数据发送请求包，请求包中包含的数据内容有
                 * 消息的总长度，需要发送的数据包个数（必须大于1），最大的数据包编号，
                 * 这个消息的PGN等
                 */
                if ( ((frame.can_id & 0x00FF0000) >> 16) == 0xEB ) {
                    /* Data transfer
                     * byte[1]: 数据包编号
                     * byte[2:8]: 数据
                     */
                    if ( (thiz->bms.can_bms_status & CAN_TP_RD) != CAN_TP_RD ) {
                        thiz->bms.can_bms_status = CAN_NORMAL;
                        log_printf(WRN, "BMS: timing crashed.");
                        continue;
                    }
                    Hachiko_feed(&thiz->bms.can_tp_bomb);
                    memcpy(&tp_buff[ (frame.data[0] - 1) * 7 ], &frame.data[1], 7);
                    log_printf(DBG_LV1, "BMS: BM data tansfer fetch the %dst packet.",
                               frame.data[0]);
                    thiz->bms.can_tp_param.tp_rcv_pack_nr ++;
                    if ( thiz->bms.can_tp_param.tp_rcv_pack_nr >=
                         thiz->bms.can_tp_param.tp_pack_nr ) {
                        // 数据接收完成后即可关闭定时器
                        Hachiko_kill(&thiz->bms.can_tp_bomb);

                        thiz->param.buff_payload = thiz->bms.can_tp_param.tp_size;
                        thiz->param.evt_param = EVT_RET_INVALID;
                        thiz->param.can_id = thiz->bms.can_tp_param.tp_pgn;
                        memcpy(thiz->param.buff.rx_buff, tp_buff, thiz->param.buff_payload);
                        log_printf(DBG_LV3,
                                   "BMS: data transfer complete PGN=%08X change to ACK",
                                   thiz->bms.can_tp_param.tp_pgn);
                        driver->driver_main_proc(thiz, EVENT_RX_DONE, &thiz->param, driver);
                        // 数据链接接受完成
                        thiz->bms.can_bms_status = CAN_TP_RD | CAN_TP_ACK;
                    }
                } else if ( ((frame.can_id & 0x00FF0000) >> 16 ) == 0xEC ) {
                    // Connection managment
                    if ( 0x10 == frame.data[0] ) {
        #if 0
                        if ( task->can_tp_buff_nr ) {
                            /*
                             * 数据传输太快，还没将缓冲区的数据发送出去
                             */
                            log_printf(WRN, "BMS: CAN data transfer too fast.");
                            continue;
                        }
        #endif
                        /* request a connection. TP.CM_RTS
                         * byte[1]: 0x10
                         * byte[2:3]: 消息大小，字节数目
                         * byte[4]: 全部数据包的数目
                         * byte[5]: 0xFF
                         * byte[6:8]: PGN
                         */
                        tp_packets_size = frame.data[2] * 256 + frame.data[1];
                        tp_packets_nr = frame.data[3];
                        tp_packet_PGN = frame.data[5] +
                                (frame.data[6] << 8) + (frame.data[7] << 16);
                        /*
                         * 接收到这个数据包后向BMS发送准备发送数据包
                         *
                         * byte[1]: 0x11
                         * byte[2]: 可发送的数据包个数
                         * byte[3]: 下一个要发送的数据包编号
                         * byte[4:5]: 0xFF
                         * byte[6:8]: PGN
                         */
                        if ( tp_packets_size <= 8 ) {
                            log_printf(WRN,
                                 "BMS: detect a BMS transfer error, pack size < 8 bytes");
                            continue;
                        }
                        if ( tp_packets_nr <= 1 ) {
                            log_printf(WRN,
                                       "BMS: detect a BMS transfer error, pack count < 2");
                            continue;
                        }

                        if ( thiz->bms.can_tp_private_status == PRIVATE_BUSY ) {
                            log_printf(WRN, "BMS: previous connection not exit,"
                                       " do new connection instead.");
                            Hachiko_feed( &thiz->bms.can_tp_bomb );
                        } else if ( thiz->bms.can_tp_bomb.status == HACHIKO_INVALID ) {
                            thiz->bms.can_tp_bomb.Hachiko_notify_proc =
                                    Hachiko_CAN_TP_notify_proc;
                            // 根据SAE J1939-21中关于CAN总线数据传输链接的说明，中间传输
                            // 过程最大不超过1250ms
                            int ret = Hachiko_new( & thiz->bms.can_tp_bomb,
                                                   HACHIKO_ONECE, 1250,
                                                   (void*)thiz);
                            if ( ret == (int)ERR_WRONG_PARAM ) {
                                log_printf(ERR,
                                           "BMS: set new timer error, with code:%d",
                                           ret);
                                continue;
                            }
                            if ( ret == (int)ERR_TIMER_BEMAX ) {
                                log_printf(ERR,
                                           "BMS: set new timer error, with code:%d",
                                           ret);
                                continue;
                            }
                        } else {
                            continue;
                        }

                        thiz->bms.can_tp_param.tp_pack_nr = tp_packets_nr;
                        thiz->bms.can_tp_param.tp_size = tp_packets_size;
                        thiz->bms.can_tp_param.tp_pgn = tp_packet_PGN;
                        thiz->bms.can_tp_param.tp_rcv_bytes = 0;
                        thiz->bms.can_tp_param.tp_rcv_pack_nr = 0;
                        thiz->bms.can_bms_status = CAN_TP_RD | CAN_TP_CTS;
                        log_printf(DBG_LV2,
                                   "BMS: data connection accepted, rolling..."
                                   "PGN: %X, total: %d packets, %d bytes",
                                   tp_packet_PGN, tp_packets_nr, tp_packets_size);
                    } else if ( 0xFF == frame.data[0] ) {
                        /* connection abort.
                         * byte[1]: 0xFF
                         * byte[2:5]: 0xFF
                         * byte[6:8]: PGN
                         */
                        int *d = (int *)&frame.data[0];
                        log_printf(DBG_LV2, "BMS: %08X", *d);
                    } else {
                        //omited.
                        int *d = (int *)&frame.data[0];
                        log_printf(DBG_LV3, "BMS: %08X", *d);
                    }
                } else {
                    thiz->param.can_id = (frame.can_id & 0x00FF0000) >> 8;
                    thiz->param.buff_payload = frame.can_dlc;
                    thiz->param.evt_param = EVT_RET_INVALID;
                    memcpy((void * __restrict__)thiz->param.buff.rx_buff, frame.data, 8);
                    driver->driver_main_proc(thiz, EVENT_RX_DONE, &thiz->param, driver);
                    log_printf(DBG_LV0, "BMS: read a frame done. %08X", frame.can_id);
                }

                if ( thiz->bms.can_bms_status == CAN_NORMAL ) {
                } else if ( thiz->bms.can_bms_status == CAN_TP_RD ) {
                    // CAN通信处于连接管理模式
                }
            }
        }
    }

    return NULL;
}
#endif

/*
 * BMS 驱动初始化
 */
int bmsdriver_init(struct charge_task *tsk)
{
    int ret = 0;
    // BMS 数据包写线程，从队列中取出要写的数据包并通过CAN总线发送出去
    ret = pthread_create( & tsk->tid_write, &tsk->attr, thread_bms_write_service, tsk);
    if ( 0 != ret ) {
        log_printf(ERR, "CAN-BUS reader start up.                       FAILE!!!!");
    }

    // BMS读书举报线程，从CAN总线读取数据包后将数据存入读入数据队列等待处理。
    ret = pthread_create( & tsk->tid_read, &tsk->attr, thread_bms_read_service, tsk);
    if ( 0 != ret ) {
        log_printf(ERR, "CAN-BUS writer start up.                       FAILE!!!!");
    }
    return 0;
}

/* 搜索对应生产商的BMS驱动
 * 先从bms驱动缓存中搜索，如果没有搜索到则到bms驱动目录搜索，若没有则会导致
 * 驱动不匹配而无法充电。
 */
struct bmsdriver *bmsdriver_search(struct charge_task *tsk, unsigned int vendor_id, const char *ver)
{
    char driver_name[64];
    struct bmsdriver drv, *real =NULL;

    memset(&drv, 0, sizeof(struct bmsdriver));

    real = tsk->bmsdriver;
    while ( NULL != real ) {
        if ( real->vendor_id == vendor_id &&
             0 == strcmp(real->version, ver) ) {
            // matched
            break;
        }
        real = real->next;
    }
    if ( real ) return real;

    // not found driver in cache, search driver in path next.
    real = NULL;
    errno = 0;
    sprintf(driver_name, "/usr/zeus/drivers/bmsdrv_%d_%s.so", vendor_id, ver);
    drv.handle = dlopen(driver_name, RTLD_LAZY);
    if ( drv.handle == NULL ) {
        log_printf(ERR, "BMSDRVIER: 无法加载bms驱动程序: %s, <%s>",
                   driver_name, strerror(errno));
        return NULL;
    }

    drv.driver_main_proc = (int (*)(struct charge_job *, BMS_EVENT_CAN,
                          struct bms_event_struct *, struct bmsdriver *))dlsym(drv.handle, "driver_main_proc");
    if ( dlerror() ) {
        log_printf(ERR, "BMSDRVIER: find  entry <driver_main_proc> faile!\n");
        goto die;
    }

    log_printf(INF, "BMSDRVIER: 开始加载数据包生成器...");
    do {
        char sql[1024];
        char **rst, *msg = NULL;
        int nr = 0, pn = 0;

        sprintf(sql,
                "SELECT symbol_define.symbol_value,"
                    "bms_can_pack_generator.pgn,"
                    "bms_can_pack_generator.prioriy,"
                    "bms_can_pack_generator.datalen,"
                    "bms_can_pack_generator.period,"
                    "bms_can_pack_generator.timeout "
                "FROM bms_vendor, bms_can_pack_generator, symbol_define "
                "WHERE bms_can_pack_generator.bms_id=bms_vendor.id AND "
                      "bms_can_pack_generator.disabled='FALSE' AND "
                      "bms_can_pack_generator.stage=symbol_define.symbol_name AND "
                      "bms_vendor.bms_version='%s' AND "
                      "bms_vendor.id=%d ORDER BY pgn;",
                ver, vendor_id);
        int ret = sqlite3_get_table(tsk->database, sql, &rst, &nr, &pn, &msg);
        if ( ret != 0 ) {
            log_printf(ERR, "没有查询到注册的驱动数据 %s", msg);
            goto die;
        }

        drv.can_pack_gen_nr_copy = nr;
        drv.generator_copy = (struct can_pack_generator*)malloc(sizeof(struct can_pack_generator)*nr);
        if ( drv.generator_copy == NULL ) {
            log_printf(ERR, "BMSDRVIER: 内存不足，无法加载数据包生成器.");
            goto die;
        }
        int l;
        for ( l = 1; l < nr; l ++ ) {
            drv.generator_copy[l-1].stage = __atoh(rst[l * 6 + 0]);
            drv.generator_copy[l-1].can_pgn = atoi(rst[l * 6 + 1]);
            drv.generator_copy[l-1].prioriy = atoi(rst[l * 6 + 2]);
            drv.generator_copy[l-1].datalen = atoi(rst[l * 6 + 3]);
            drv.generator_copy[l-1].period = atoi(rst[l * 6 + 4]);
            drv.generator_copy[l-1].can_tolerate_silence = atoi(rst[l * 6 + 5]);
            drv.generator_copy[l-1].heartbeat = 0;
            drv.generator_copy[l-1].can_silence = 0;
            drv.generator_copy[l-1].can_counter = 0;
        }

    } while (0);

    strncpy(drv.vendor_name, "default", 64);
    drv.vendor_id = vendor_id;
    strncpy(drv.version, ver, 16);
    drv.loaded = time(NULL);
    real = (struct bmsdriver *)malloc(sizeof(struct bmsdriver));
    if ( real == NULL ) {
        log_printf(ERR, "BMSDRVIER: memory low, could not load driver: bmsdrv_%d_%s.so",
                   vendor_id, ver);
        goto die;
    }
    memcpy(real, &drv, sizeof(struct bmsdriver));

    log_printf(INF, "BMSDRVIER: driver loaded: %s", driver_name);
    return real;
die:
    if ( drv.generator_copy ) {
        free(drv.generator_copy);
    }
    if ( real ) {
        free(real);
    }
    if ( drv.handle ) {
        dlclose(drv.handle);
    }
    return NULL;
}

/*
 * 作业和驱动进行绑定
 * 一个驱动可以在同一时间驱动两个作业，因此将驱动和作业进行绑定可以提高效率
 */
int bind_bmsdriver(struct bmsdriver *drv, struct charge_job *job)
{
    if ( !drv || ! job ) return ERR_ERR;

    if ( drv->binder[0][0] || drv->binder[0][1] ) {
        log_printf(ERR, "绑定BMS驱动失败!");
        return ERR_ERR;
    }
    job->bms.generator =
            (struct can_pack_generator*)malloc(sizeof(struct can_pack_generator)*drv->can_pack_gen_nr_copy);
    if ( job->bms.generator == NULL ) {
        log_printf(ERR, "BMSDRVIER: 内存不足，无法拷贝数据包生成器副本.");
        return ERR_ERR;
    }
    job->bms.can_pack_gen_nr = drv->can_pack_gen_nr_copy;
    memcpy(job->bms.generator, drv->generator_copy,
           sizeof(struct can_pack_generator)*drv->can_pack_gen_nr_copy);

    log_printf(INF, "绑定BMS驱动成功!");

    job->bms.driver = drv;
    drv->binder[0][0] = job;
    drv->binder[0][1] = job;
    return ERR_OK;
}

// 搜索指定PGN的生成器
struct can_pack_generator *gen_search(struct can_pack_generator *p, unsigned int nr, unsigned int pgn)
{
    struct can_pack_generator *gen = NULL;

    for ( ;p && nr; nr --, p ++ ) {
        if ( p->can_pgn == pgn ) {
            gen = p;
            break;
        }
    }

    return gen;
}
