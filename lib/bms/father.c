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


// BMS 运行时服务
void *thread_bms_runtime_service(void *arg) ___THREAD_ENTRY___
{
    int *done = (int *)arg;
    int mydone = 0;
    int s = -1;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    int nbytes;
    // 用于链接管理的数据缓冲
    unsigned char tp_buff[2048];
    struct charge_task *tsk = (struct charge_task *)arg;
    struct bmsdriver *driver;
    struct charge_job *thiz;
    fd_set rfds, wrds;
    struct timeval tv ;
    int retval;

    // 数据包个数
    unsigned int tp_packets_nr = 0;
    // 数据包字节总数
    unsigned int tp_packets_size = 0;
    // 数据包对应的PGN
    unsigned int tp_packet_PGN = 0;

    if ( done == NULL ) done = &mydone;
    log_printf(INF, "BMS: %s running...s=%d", __FUNCTION__, s);
__reinit:
    if ( s != -1 ) {
        shutdown(s, SHUT_RDWR);
        s = -1;
    }
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

    while ( 1 ) {
        usleep(5000);
        // 轮询所有的驱动
        for ( driver = tsk->bmsdriver; driver ; driver = driver->next ) {
            unsigned int i;
            for ( i = 0; driver->binder[i][0] &&
                  i < sizeof(driver->binder)/(2*sizeof(struct charge_job *)); i ++ ) {
                thiz = driver->binder[i][0];
                thiz->can_read_handle = s;
                if ( thiz->job_status == JOB_DETACHING ) {
                    log_printf(INF, "JOB exited, unbind BMS driver.read automatic!");
                    thiz->bms.bms_read_init_ok = 0xFF;
                    Hachiko_kill(&thiz->bms.can_tp_bomb);
                    Hachiko_kill(&thiz->bms.can_heart_beat);
                    driver->binder[i][0] = NULL;
                    thiz->can_read_handle = -1;

                    log_printf(INF, "JOB exited, unbind BMS.write driver automatic!");
                    driver->binder[i][1] = NULL;
                    thiz->bms.bms_write_init_ok = 0xFF;
                    thiz->can_write_handle = -1;
                    goto __reinit;
                }

                if ( thiz->bms.bms_read_init_ok != 0x7f ) {
                    thiz->bms.bms_read_init_ok = 0x7F;
                    thiz->bms.bms_write_init_ok = 0x7f;
                    thiz->ref_nr ++;
                    thiz->bms.can_tp_bomb.status = HACHIKO_INVALID;
                    log_printf(INF, "BMS.DRV: BMS驱动读写数据线程绑定完成.");
                    driver->driver_main_proc(thiz, EVENT_CAN_INIT, &thiz->param, driver);
                }

                if ( thiz->bms.can_bms_status  == CAN_INVALID ) {
                    continue;
                }


                FD_ZERO(&rfds);
                FD_ZERO(&wrds);
                FD_SET(s, &rfds);
                FD_SET(s, &wrds);
                tv.tv_sec = 3;
                tv.tv_usec = 500;
                retval = select(s+1, &rfds, &wrds, NULL, &tv);
                if (  -1 == retval ) {
                    log_printf(WRN, "select timeout.");
                    continue;
                } else if ( 0 == retval ) {
                    // 超 时
                    log_printf(WRN, "select timeout.");
                    continue;
                }
                if ( FD_ISSET(s, &rfds) ) {
                    memset(&frame, 0, sizeof(frame));
                    nbytes = read(s, &frame, sizeof(struct can_frame));
                    if ( nbytes == sizeof(struct can_frame) ) {
                        thiz->bms.frame_rx ++;
                        thiz->bms.rx_seed ++;
                    }

                    if ( (frame.can_id & 0xFFFF) != CAN_RCV_ID_MASK ) {
                        if ( (frame.can_id & 0xFFFF) == CAN_TX_ID_MASK ) {
                            driver->driver_main_proc(thiz, EVENT_TX_DONE, &thiz->param, driver);
                        }
                    } else {
                        if ( ((frame.can_id & 0x00FF0000) >> 16) == 0xEB ) {
                            /* Data transfer
                             * byte[1]: 数据包编号
                             * byte[2:8]: 数据
                             */
                            memcpy(&tp_buff[ (frame.data[0] - 1) * 7 ], &frame.data[1], 7);

                            thiz->bms.can_tp_param.tp_rcv_pack_nr ++;
                            if ( thiz->bms.can_tp_param.tp_rcv_pack_nr >=
                                 thiz->bms.can_tp_param.tp_pack_nr ) {
                                thiz->param.buff_payload = thiz->bms.can_tp_param.tp_size;
                                thiz->param.evt_param = EVT_RET_INVALID;
                                thiz->param.can_id = thiz->bms.can_tp_param.tp_pgn;
                                memcpy(thiz->param.buff.rx_buff, tp_buff, thiz->param.buff_payload);
                                driver->driver_main_proc(thiz, EVENT_RX_DONE, &thiz->param, driver);
                                // 数据链接接受完成
                                thiz->bms.can_bms_status = CAN_NORMAL;

                                //串口处于连接管理状态时，将会收到该传输数据报请求。
                                thiz->param.buff.tx_buff[0] = 0x13;
                                // 目前的多数据包发送策略是： 无论要发送多少数据包，都一次传输完成
                                thiz->param.buff.tx_buff[1] = thiz->bms.can_tp_param.tp_size & 0xFF;
                                thiz->param.buff.tx_buff[2] = (thiz->bms.can_tp_param.tp_size >> 8) & 0xFF;
                                thiz->param.buff.tx_buff[3] = thiz->bms.can_tp_param.tp_pack_nr;
                                thiz->param.buff.tx_buff[4] = 0xFF;
                                thiz->param.buff.tx_buff[5] = (thiz->bms.can_tp_param.tp_pgn >> 16) & 0xFF;
                                thiz->param.buff.tx_buff[6] = (thiz->bms.can_tp_param.tp_pgn >> 8 ) & 0xFF;
                                thiz->param.buff.tx_buff[7] = thiz->bms.can_tp_param.tp_pgn & 0xFF;
                                thiz->param.buff_payload = 8;
                                thiz->param.can_id = 0x1cecf456 | CAN_EFF_FLAG;

                                frame.can_id = thiz->param.can_id;
                                frame.can_dlc= thiz->param.buff_payload;
                                memcpy(frame.data, thiz->param.buff.tx_buff, 8);
                                nbytes = write(s, &frame, sizeof(struct can_frame));
                            }
                        } else if ( ((frame.can_id & 0x00FF0000) >> 16) == 0xEC ) {
                            if ( 0x10 == frame.data[0] ) {
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
                                } else if ( tp_packets_nr <= 1 ) {
                                    log_printf(WRN,
                                               "BMS: detect a BMS transfer error, pack count < 2");
                                } else {
                                    thiz->bms.can_tp_param.tp_pack_nr = tp_packets_nr;
                                    thiz->bms.can_tp_param.tp_size = tp_packets_size;
                                    thiz->bms.can_tp_param.tp_pgn = tp_packet_PGN;
                                    thiz->bms.can_tp_param.tp_rcv_bytes = 0;
                                    thiz->bms.can_tp_param.tp_rcv_pack_nr = 0;
                                    thiz->bms.can_bms_status = CAN_TP_RD;

                                    thiz->param.buff.tx_buff[0] = 0x11;
                                    // 目前的多数据包发送策略是： 无论要发送多少数据包，都一次传输完成
                                    thiz->param.buff.tx_buff[1] = thiz->bms.can_tp_param.tp_pack_nr;
                                    thiz->param.buff.tx_buff[2] = 1;
                                    thiz->param.buff.tx_buff[3] = 0xFF;
                                    thiz->param.buff.tx_buff[4] = 0xFF;
                                    thiz->param.buff.tx_buff[5] = (thiz->bms.can_tp_param.tp_pgn >> 16) & 0xFF;
                                    thiz->param.buff.tx_buff[6] = (thiz->bms.can_tp_param.tp_pgn >> 8 ) & 0xFF;
                                    thiz->param.buff.tx_buff[7] = thiz->bms.can_tp_param.tp_pgn & 0xFF;
                                    thiz->param.buff_payload = 8;
                                    thiz->param.can_id = 0x1cecf456 | CAN_EFF_FLAG;
                                    frame.can_id = thiz->param.can_id;
                                    frame.can_dlc= thiz->param.buff_payload;
                                    memcpy(frame.data, thiz->param.buff.tx_buff, 8);
                                    nbytes = write(s, &frame, sizeof(struct can_frame));
                                }
                            } else if ( 0xFF == frame.data[0] ) {
                                /* connection abort.
                                 * byte[1]: 0xFF
                                 * byte[2:5]: 0xFF
                                 * byte[6:8]: PGN
                                 */
                            } else {
                            }
                        } else {
                            thiz->param.can_id = (frame.can_id & 0x00FF0000) >> 8;
                            thiz->param.buff_payload = frame.can_dlc;
                            thiz->param.evt_param = EVT_RET_INVALID;
                            memcpy((void * __restrict__)thiz->param.buff.rx_buff, frame.data, 8);
                            driver->driver_main_proc(thiz, EVENT_RX_DONE, &thiz->param, driver);
                        }
                    }
                }

                if ( FD_ISSET(s, &wrds) ) {
					thiz->param.evt_param = EVT_RET_ERR;
                    driver->driver_main_proc(thiz, EVENT_TX_REQUEST, &thiz->param, driver);
                    if ( thiz->param.evt_param != EVT_RET_OK ) continue;
					thiz->param.evt_param = EVT_RET_ERR;
                    driver->driver_main_proc(thiz, EVENT_TX_PRE, &thiz->param, driver);
                    if ( thiz->param.evt_param != EVT_RET_OK ) continue;

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
                }
            }
        }
    }

    return NULL;
}

/*
 * BMS 驱动初始化
 */
int bmsdriver_init(struct charge_task *tsk)
{
    int ret = 0;

    // BMS读书举报线程，从CAN总线读取数据包后将数据存入读入数据队列等待处理。
    ret = pthread_create( & tsk->tid_read, &tsk->attr, thread_bms_runtime_service, tsk);
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
