#include "stdafx.h"
#include "../../thirdpart/sqlite/sqlite3.h"
#include "../lj/stdafx.h"
#include "../../ontom/charge.h"
#include "father.h"

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
    struct bms_event_struct param;
    unsigned char txbuff[32];
    int nbytes;
    struct charge_job *thiz = (struct charge_job *)arg;
    if ( done == NULL ) done = &mydone;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    strcpy(ifr.ifr_name, thiz->bms.can_dev );
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(s, (struct sockaddr *)&addr, sizeof(addr));

    log_printf(INF, "%s running...s=%d", __FUNCTION__, s);

    param.buff_payload = 0;
    param.evt_param = EVT_RET_INVALID;

    while ( 1 ) {
        usleep(5000);
#if 0
        if ( 0x7F != thiz->bms.bms_write_init_ok ) {
            // 进行数据结构的初始化操作
            can_packet_callback(thiz, EVENT_CAN_INIT, &param);
            thiz->bms.bms_write_init_ok = 0x7f;
            thiz->ref_nr ++;
        } else if ( thiz->job_status == JOB_DETACHING ) {
            thiz->ref_nr --;
            thiz->bms.bms_write_init_ok = 0;
            continue;
        }

        /*
         * 写线程同时负责写数据和进行连接管理时的控制数据写出，这里需要对当前CAN的
         * 状态进行判定，当CAN处于CAN_NORMAL时进行普通的写操作，当CAN处于CAN_TP_RD
         * 时，采用EVENT_TX_REQUEST 当CAN处于CAN_TP_RD时采用EVENT_TX_TP_REQUEST
         */
        param.buff.tx_buff = txbuff;
        param.buff_size = sizeof(txbuff);
        param.evt_param = EVT_RET_INVALID;
        if ( thiz->bms.can_bms_status & CAN_NORMAL ) {
            can_packet_callback(thiz, EVENT_TX_REQUEST, &param);
        } else if ( thiz->bms.can_bms_status & CAN_TP_RD ) {
            switch ( thiz->bms.can_bms_status & 0xF0 ) {
            case CAN_TP_CTS:
                can_packet_callback(thiz, EVENT_TX_TP_CTS, &param);
                break;
            case CAN_TP_TX:
            case CAN_TP_RX:
                break;
            case CAN_TP_ACK:
                can_packet_callback(thiz, EVENT_TX_TP_ACK, &param);
                break;
            case CAN_TP_ABRT:
                can_packet_callback(thiz, EVENT_TX_TP_ABRT, &param);
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

        if ( EVT_RET_OK != param.evt_param ) {
            continue;
        }

        param.evt_param = EVT_RET_INVALID;
        // 链接模式下的数据包发送不需要确认, 并且也不能被中止
        if ( thiz->bms.can_bms_status == CAN_NORMAL ) {
            can_packet_callback(thiz, EVENT_TX_PRE, &param);
            if ( EVT_RET_TX_ABORT == param.evt_param ) {
                // packet sent abort.
                continue;
            } else if ( EVT_RET_OK != param.evt_param ) {
                continue;
            } else {
                // confirm to send.
            }
        }

        /* 根据GB/T 27930-2011 中相关规定，充电机向BMS发送数据包都没有超过
         * 8字节限制，因此这里不用进行连接管理通信。
         */
        if ( param.buff_payload <= 8 && param.buff_payload > 0 ) {
            frame.can_id = param.can_id;
            frame.can_dlc= param.buff_payload;
            memcpy(frame.data, param.buff.tx_buff, 8);
            nbytes = write(s, &frame, sizeof(struct can_frame));
            if ( (unsigned int)nbytes < param.buff_payload ) {
                param.evt_param = EVT_RET_ERR;
                can_packet_callback(thiz, EVENT_TX_FAILS, &param);
            } else {
                param.evt_param = EVT_RET_OK;
                can_packet_callback(thiz, EVENT_TX_DONE, &param);
            }
        } else if ( param.buff_payload > 8 ) {
            // 大于8字节的数据包在这里处理，程序向后兼容
            static unsigned int notimplement = 0;
            if ( notimplement % 10000 == 0 ) {
                log_printf(INF,
                           "BMS: Connection manage for send has not implemented.");
            }
            notimplement ++;
        } else {
        }

        param.buff_payload = 0;

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
#endif
    }
    return NULL;
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
    struct bms_event_struct param;
    // 用于链接管理的数据缓冲
    unsigned char tp_buff[2048];
    struct charge_job * thiz = (struct charge_job *)arg;

    // 缓冲区数据字节数
    unsigned int tp_cnt = 0;
    // 数据包个数
    unsigned int tp_packets_nr = 0;
    // 数据包字节总数
    unsigned int tp_packets_size = 0;
    // 数据包对应的PGN
    unsigned int tp_packet_PGN = 0;

    unsigned int dbg_packets = 0;

    thiz->bms.can_tp_bomb._private = (void *)thiz;

    if ( done == NULL ) done = &mydone;
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    fcntl(s, F_SETFL, FASYNC);
    strcpy(ifr.ifr_name, thiz->bms.can_dev );
    ioctl(s, SIOCGIFINDEX, &ifr);
    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(s, (struct sockaddr *)&addr, sizeof(addr));
    log_printf(INF, "BMS: %s running...s=%d", __FUNCTION__, s);

    param.buff.rx_buff = tp_buff;
    param.buff_size = sizeof(tp_buff);
    param.buff_payload = 0;
    while ( 1 ) {
        usleep(5000);
#if 0
        if ( thiz->bms.can_bms_status  == CAN_INVALID ) {
            continue;
        }

        if ( thiz->bms.bms_read_init_ok != 0x7f ) {
            thiz->bms.bms_read_init_ok = 0x7F;
            thiz->ref_nr ++;
        }

        memset(&frame, 0, sizeof(frame));
        nbytes = read(s, &frame, sizeof(struct can_frame));
        if ( (frame.can_id & 0xFFFF) != CAN_RCV_ID_MASK ) {
            #if 1
            log_printf(DBG_LV0, "BMS: id not accept %x", frame.can_id);
            #endif
            continue;
        }

        dbg_packets ++;
        /*
         * 非阻塞方式读
         */
        if ( thiz->job_status == JOB_DETACHING ) {
            thiz->bms.bms_read_init_ok = 0;
            thiz->ref_nr --;
            continue;
        }

        if ( nbytes != sizeof(struct can_frame) ) {
            param.evt_param = EVT_RET_ERR;
            log_printf(DBG_LV3, "BMS: read frame error %x", frame.can_id);
            can_packet_callback(thiz, EVENT_RX_ERROR, &param);
            continue;
        }

        debug_log(DBG_LV1,
                   "BMS: get %dst packet %08X:%02X%02X%02X%02X%02X%02X%02X%02X",
                   dbg_packets,
                   frame.can_id,
                   frame.data[0],
                   frame.data[1],
                   frame.data[2],
                   frame.data[3],
                   frame.data[4],
                   frame.data[5],
                   frame.data[6],
                   frame.data[7]);

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

                param.buff_payload = thiz->bms.can_tp_param.tp_size;
                param.evt_param = EVT_RET_INVALID;
                param.can_id = thiz->bms.can_tp_param.tp_pgn;
                log_printf(DBG_LV3,
                           "BMS: data transfer complete PGN=%08X change to ACK",
                           thiz->bms.can_tp_param.tp_pgn);
                can_packet_callback(thiz, EVENT_RX_DONE, &param);
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
                tp_cnt = 0;
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
                } else {
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
            param.can_id = (frame.can_id & 0x00FF0000) >> 8;
            param.buff_payload = frame.can_dlc;
            param.evt_param = EVT_RET_INVALID;
            memcpy((void * __restrict__)param.buff.rx_buff, frame.data, 8);
            can_packet_callback(thiz, EVENT_RX_DONE, &param);
            log_printf(DBG_LV0, "BMS: read a frame done. %08X", frame.can_id);
        }

        if ( thiz->bms.can_bms_status == CAN_NORMAL ) {
        } else if ( thiz->bms.can_bms_status == CAN_TP_RD ) {
            // CAN通信处于连接管理模式
        }
#endif
    }

    return NULL;
}

