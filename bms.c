/*
 * BMS - CAN 通信过程
 * 后台 - 串口通信过程
 * 充电机 - 串口通信过程
 * 读卡器 - 串口通信过程
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <time.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "charge.h"
#include "Hachiko.h"
#include "bms.h"
#include "config.h"
#include "log.h"
#include "error.h"

// 数据包生成器信息
struct can_pack_generator generator[] = {
    {
    .stage      =  CHARGE_STAGE_HANDSHACKING,
    .pgn        =  0x000100,
    .prioriy    =  6,
    .datalen    =  8,
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "CRM"
    },
    {
    .stage      =  CHARGE_STAGE_CONFIGURE,
    .pgn        =  0x000700,
    .prioriy    =  6,
    .datalen    =  7,
    .period     =  500,
    .heartbeat   =  0,
    .mnemonic   =  "CTS"
    },
    {
    .stage      =  CHARGE_STAGE_CONFIGURE,
    .pgn        =  0x000800,
    .prioriy    =  6,
    .datalen    =  6,
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "CML"
    },
    {
    .stage      =  CHARGE_STAGE_CONFIGURE,
    .pgn        =  0x00A00,
    .prioriy    =  4,
    .datalen    =  1,
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "CRO"
    },
    {
    .stage      =  CHARGE_STAGE_CHARGING,
    .pgn        =  0x001200,
    .prioriy    =  6,
    .datalen    =  6,
    .period     =  50,
    .heartbeat   =  0,
    .mnemonic   =  "CCS"
    },
    {
    .stage      =  CHARGE_STAGE_CHARGING,
    .pgn        =  0x001A00,
    .prioriy    =  4,
    .datalen    =  4,
    .period     =  10,
    .heartbeat   =  0,
    .mnemonic   =  "CST"
    },
    {
    .stage      =  CHARGE_STAGE_DONE,
    .pgn        =  0x001D00,
    .prioriy    =  6,
    .datalen    =  5,
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "CSD"
    },
    {
    .stage      =  CHARGE_STAGE_ANY,
    .pgn        =  0x001F00,
    .prioriy    =  2,
    .datalen    =  4,
    .period     =  250,
    .heartbeat   =  0,
    .mnemonic   =  "CEM"
    }
};

// CAN 数据包统计结构
struct bms_statistics statistics[] = {
    {
    .can_pgn = PGN_CRM,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BRM,
    .can_silence = 0,
    .can_tolerate_silence = 250,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BCP,
    .can_silence = 0,
    .can_tolerate_silence = 500,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_CTS,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_CML,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BRO,
    .can_silence = 0,
    .can_tolerate_silence = 250,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_CRO,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BCL,
    .can_silence = 0,
    .can_tolerate_silence = 50,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BCS,
    .can_silence = 0,
    .can_tolerate_silence = 250,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_CCS,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BSM,
    .can_silence = 0,
    .can_tolerate_silence = 250,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BMV,
    .can_silence = 0,
    .can_tolerate_silence = 1000,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BMT,
    .can_silence = 0,
    .can_tolerate_silence = 1000,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BSP,
    .can_silence = 0,
    .can_tolerate_silence = 1000,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BST,
    .can_silence = 0,
    .can_tolerate_silence = 10,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_CST,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BSD,
    .can_silence = 0,
    .can_tolerate_silence = 250,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_CSD,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_BEM,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    },
    {
    .can_pgn = PGN_CEM,
    .can_silence = 0,
    .can_tolerate_silence = 0,
    .can_counter = 0
    }
};

// 数据包超时心跳包, 定时器自动复位, 一个单位时间一次
void Hachiko_packet_heart_beart_notify_proc(Hachiko_EVT evt, void *private,
                            const struct Hachiko_food *self)
{
    if (evt == HACHIKO_TIMEOUT ) {
        int i = 0;
        struct can_pack_generator *thiz;
        struct bms_statistics *me;
        for ( i = 0;
              (unsigned int)i < sizeof(generator) / sizeof(struct can_pack_generator); i++ ) {
            thiz = &generator[i];
            if ( thiz->stage == task->charge_stage ) {
                if ( thiz->heartbeat < thiz->period ) {
                    //thiz->heartbeat += 10;
                    thiz->heartbeat += 1;
                } else {
                    thiz->heartbeat = thiz->period;
                }
            } else {
                thiz->heartbeat = 0;
            }
        }

        /*
         * 为了能够侦探到接受数据包的超时事件，需要在这里进行一个计数操作
         * 当can_silence 计数大于等于 can_tolerate_silence 时认为对应数据包接收超时，需要在BMS逻辑主线程
         * 中做相应处理.
         *
         * BEM和CEM不在超时统计范围内
         */
        for ( i = 0;
              (unsigned int)i < (sizeof(statistics) / sizeof(struct bms_statistics) ) - 2; i++ ) {
            me = &statistics[i];
            me->can_silence ++;
            if ( me->can_tolerate_silence < me->can_silence ) {
                switch (task->charge_stage) {
                case CHARGE_STAGE_HANDSHACKING:
                    if (me->can_pgn != PGN_BRM) break;
                    if (bit_read(task, F_GUN_1_PHY_CONN_STATUS)) {
                        if ( !bit_read(task, S_BMS_COMM_DOWN) ) {
                            bit_set(task, S_BMS_COMM_DOWN);
                            log_printf(WRN, "BMS: 握手阶段BMS通信"RED("故障"));
                            task->charge_stage = CHARGE_STAGE_HANDSHACKING;
                        }
                    }
                    break;
                case CHARGE_STAGE_CONFIGURE:
                    if (me->can_pgn != PGN_BCP) break;
                    if (bit_read(task, F_GUN_1_PHY_CONN_STATUS)) {
                        if ( !bit_read(task, S_BMS_COMM_DOWN) ) {
                            bit_set(task, S_BMS_COMM_DOWN);
                            log_printf(WRN, "BMS: 配置阶段BMS通信"RED("故障"));
                            task->charge_stage = CHARGE_STAGE_HANDSHACKING;
                        }
                    }
                    break;
                case CHARGE_STAGE_CHARGING:
                    if (me->can_pgn != PGN_BCL) break;
                    if (bit_read(task, F_GUN_1_PHY_CONN_STATUS)) {
                        if ( !bit_read(task, S_BMS_COMM_DOWN) ) {
                            bit_set(task, S_BMS_COMM_DOWN);
                            log_printf(WRN, "BMS: 充电阶段BMS通信"RED("故障"));
                            task->charge_stage = CHARGE_STAGE_HANDSHACKING;
                        }
                    }
                    break;
                case CHARGE_STAGE_DONE:
                    if (me->can_pgn != PGN_BSD) break;
                    if (bit_read(task, F_GUN_1_PHY_CONN_STATUS)) {
                        if ( !bit_read(task, S_BMS_COMM_DOWN) ) {
                            bit_set(task, S_BMS_COMM_DOWN);
                            log_printf(WRN, "BMS: 充电完成阶段BMS通信"RED("故障"));
                            task->charge_stage = CHARGE_STAGE_HANDSHACKING;
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
}

/*
 * 本可以将串口通信，SOCKET通信方式也纳入该事件函数，但本着CAN通信优先的原则，暂时将
 * CAN通信时间数据独立出来进行处理。
 *
 * CAN数据包消息处理函数，必须是可重入函数
 * 该函数处于所有需要互斥对象的临界点，有多个线程会直接调用该函数。
 */
static int can_packet_callback(
        struct charge_task * thiz, EVENT_CAN ev, struct event_struct* param)
{
    switch ( ev ) {
    case EVENT_CAN_INIT:
        // 事件循环函数初始化
        thiz->can_bms_status = CAN_NORMAL;
        thiz->can_heart_beat.Hachiko_notify_proc=
                Hachiko_packet_heart_beart_notify_proc;
        Hachiko_new(&thiz->can_heart_beat, HACHIKO_AUTO_FEED, 4, NULL);
        log_printf(INF, "BMS: CHARGER change stage to "RED("CHARGE_STAGE_HANDSHACKING"));
        thiz->charge_stage = CHARGE_STAGE_HANDSHACKING;
        //thiz->charge_stage = CHARGE_STAGE_CONFIGURE;
        break;
    case EVENT_CAN_RESET:
        // 事件循环函数复位
        // 当发生通信中断时会收到该事件
        break;
    case EVENT_INVALID:
        // 无效事件，事件填充
        break;
    case EVENT_RX_DONE:
        // 数据包接受成功了
        about_packet_reciev_done(thiz, param);
        break;
    case EVENT_RX_ERROR:
        // 数据包接受失败了
        break;
    case EVENT_TX_FAILS:
        // 数据包发送失败了
        break;
    case EVENT_TX_DONE:
        // 数据包发送完成了
        log_printf(DBG_LV0, "BMS: packet sent. %08X", param->can_id);
        if ( (param->can_id & 0x00FF0000) == (PGN_CRM << 8) &&
             bit_read(thiz, F_BMS_RECOGNIZED ) &&
             bit_read(thiz, F_VEHICLE_RECOGNIZED ) &&
             thiz->charge_stage == CHARGE_STAGE_HANDSHACKING) {
            thiz->charge_stage = CHARGE_STAGE_CONFIGURE;
            log_printf(INF, "BMS: CHARGER change stage to "RED("CHARGE_STAGE_CONFIGURE"));
        }
        if ( (param->can_id & 0x00FF0000) == (PGN_CRO << 8) &&
             bit_read(thiz, F_CHARGER_READY) &&
             bit_read(thiz, F_BMS_READY ) &&
             thiz->charge_stage == CHARGE_STAGE_CONFIGURE ) {
            thiz->charge_stage = CHARGE_STAGE_CHARGING;
            log_printf(INF,
              "BMS: CHARGER change stage to "RED("CHARGE_STAGE_CHARGING"));
        }
        break;
    case EVENT_TX_PRE:
        // 决定是否要发送刚刚准备发送的数据包
        param->evt_param = EVT_RET_OK;
        break;
    case EVENT_TX_REQUEST:
        /*
         * 在这里进行CAN数据包的发送处理
         * 进行数据包发送的条件是：充电枪物理连接正常，进入车辆识别过程，或充电过程。
         *
         * 数据包的发送，优先级最高的是错误报文输出，若是遇到周期性发送的数据包在发送
         * 时序上有重叠的问题，那么在这里的处理方式是，先到先处理，例如在若干个循环内
         * 数据包A，B的发送周期是10ms, 30ms，那么A，B的发送时时序应该是如下方式
         * T (ms)      数据包
         * |             |
         * 0             A
         * 0  + $$       B
         * |             |
         * |             |
         * 10            A
         * |             |
         * |             |
         * 20            A
         * |             |
         * |             |
         * 30            A
         * 30 + $$       B
         * |             |
         * |             |
         * ...         ...
         *
         * $$ 表示最小的循环控制周期，一般来说是绝对小于数据包的发送周期的
         * 所以会有如下的控制逻辑结构
         * if ( ... ) {
         * } else if ( ... ) {
         * } else if ( ... ) {
         * } else ;
         * 在若干个循环控制周期内，数据包都能按照既定的周期发送完成.
         */
        switch ( thiz->charge_stage ) {
        case CHARGE_STAGE_INVALID:
            param->evt_param = EVT_RET_ERR;
            break;
        case CHARGE_STAGE_HANDSHACKING:
            if ( generator[0].heartbeat >= generator[0].period ) {
                gen_packet_PGN256(thiz, param);
                generator[0].heartbeat = 0;
            }
            if ( generator[7].heartbeat >= generator[7].period ) {
                gen_packet_PGN7936(thiz, param);
                generator[7].heartbeat = 0;
            }
            break;
        case CHARGE_STAGE_CONFIGURE:
            if ( generator[1].heartbeat >= generator[1].period ) {
                gen_packet_PGN1792(thiz, param);
                generator[1].heartbeat = 0;
            }
            else if ( generator[2].heartbeat >= generator[2].period ) {
                gen_packet_PGN2048(thiz, param);
                generator[2].heartbeat = 0;
            }
            else if ( generator[3].heartbeat >= generator[3].period ) {
                gen_packet_PGN2560(thiz, param);
                generator[3].heartbeat = 0;
            }
            if ( generator[7].heartbeat >= generator[7].period ) {
                gen_packet_PGN7936(thiz, param);
                generator[7].heartbeat = 0;
            }
            break;
        case CHARGE_STAGE_CHARGING:
            if ( generator[4].heartbeat >= generator[4].period ) {
                gen_packet_PGN4608(thiz, param);
                generator[4].heartbeat = 0;
            }
            else if ( generator[5].heartbeat >= generator[5].period ) {
                gen_packet_PGN6656(thiz, param);
                generator[5].heartbeat = 0;
            }
            if ( generator[7].heartbeat >= generator[7].period ) {
                gen_packet_PGN7936(thiz, param);
                generator[7].heartbeat = 0;
            }
            break;
        case CHARGE_STAGE_DONE:
            if ( generator[6].heartbeat >= generator[6].period ) {
                gen_packet_PGN7424(thiz, param);
                generator[6].heartbeat = 0;
            }
            if ( generator[7].heartbeat >= generator[7].period ) {
                gen_packet_PGN7936(thiz, param);
                generator[7].heartbeat = 0;
            }
            break;
        default:
            break;
        }
        break;
    case EVENT_TX_TP_RTS: // 本系统中BMS通信暂时不会使用
        //串口处于连接管理状态时，将会收到该传输数据报请求。
        break;
    case EVENT_TX_TP_CTS:
    {
        /*串口处于连接管理状态时，将会收到该传输数据报请求。
         * 当数据包接收完成后向BMS发送消息结束应答数据包
         *
         * byte[1]: 0x13
         * byte[2:3]: 消息大小，字节数目
         * byte[4]: 全部数据包数目
         * byte[5]: 0xFF
         * byte[6:8]: PGN
         */

        param->buff.tx_buff[0] = 0x11;
        // 目前的多数据包发送策略是： 无论要发送多少数据包，都一次传输完成
        param->buff.tx_buff[1] = thiz->can_tp_param.tp_pack_nr;
        param->buff.tx_buff[2] = 1;
        param->buff.tx_buff[3] = 0xFF;
        param->buff.tx_buff[4] = 0xFF;
        param->buff.tx_buff[5] = (thiz->can_tp_param.tp_pgn >> 16) & 0xFF;
        param->buff.tx_buff[6] = (thiz->can_tp_param.tp_pgn >> 8 ) & 0xFF;
        param->buff.tx_buff[7] = thiz->can_tp_param.tp_pgn & 0xFF;
        param->buff_payload = 8;
        param->can_id = 0x1cecf456 | CAN_EFF_FLAG;
        param->evt_param = EVT_RET_OK;
    }
        break;
    case EVENT_TX_TP_ACK:
    {
        //串口处于连接管理状态时，将会收到该传输数据报请求。
        param->buff.tx_buff[0] = 0x13;
        // 目前的多数据包发送策略是： 无论要发送多少数据包，都一次传输完成
        param->buff.tx_buff[1] = thiz->can_tp_param.tp_size & 0xFF;
        param->buff.tx_buff[2] = (thiz->can_tp_param.tp_size >> 8) & 0xFF;
        param->buff.tx_buff[3] = thiz->can_tp_param.tp_pack_nr;
        param->buff.tx_buff[4] = 0xFF;
        param->buff.tx_buff[5] = (thiz->can_tp_param.tp_pgn >> 16) & 0xFF;
        param->buff.tx_buff[6] = (thiz->can_tp_param.tp_pgn >> 8 ) & 0xFF;
        param->buff.tx_buff[7] = thiz->can_tp_param.tp_pgn & 0xFF;
        param->buff_payload = 8;
        param->can_id = 0x1cecf456 | CAN_EFF_FLAG;
        param->evt_param = EVT_RET_OK;
    }
        break;
    case EVENT_TX_TP_ABRT:
        //串口处于连接管理状态时，将会收到该传输数据报请求。
        break;
    default:
        break;
    }

    return 0;
}

// CAN数据包接受完成
int about_packet_reciev_done(struct charge_task *thiz,
                             struct event_struct *param)
{
    switch ( param->can_id & 0x00FF00 ) {
    case PGN_CRM :// 0x000100,
        break;
    case PGN_CTS :// 0x000700,
        break;
    case PGN_CML :// 0x000800,
        break;
    case PGN_CCS :// 0x001200,
        break;
    case PGN_CST :// 0x001A00,
        break;
    case PGN_CSD :// 0x001D00,
        break;
    case PGN_CRO :// 0x000A00,
        break;
    case PGN_CEM :// 0x001F00
        break;
    case PGN_BRM :// 0x000200, BMS 车辆辨识报文
        statistics[I_BRM].can_counter ++;
        statistics[I_BRM].can_silence = 0;
        if ( bit_read(task, S_BMS_COMM_DOWN) ) {
            log_printf(INF, "BMS: BMS 通信"GRN("恢复"));
        }
        bit_clr(task, S_BMS_COMM_DOWN);

        if ( param->buff_payload == 8 ) {
            memcpy(&thiz->vehicle_info, param->buff.rx_buff, 8);
        } else if ( param->buff_payload == sizeof(struct pgn512_BRM) ) {
            memcpy(&thiz->vehicle_info,
                   param->buff.rx_buff, sizeof(struct pgn512_BRM));
        }

        if ( thiz->vehicle_info.spn2565_bms_version[0] == 0x00 &&
             thiz->vehicle_info.spn2565_bms_version[1] == 0x01 &&
             thiz->vehicle_info.spn2565_bms_version[2] == 0x00 ) {

        } else {
            log_printf(WRN,
                  "BMS not recognized due to invalid BMS VERSION(SPN2565).");
            bit_clr(thiz, F_BMS_RECOGNIZED);
            break;
        }

        if ( thiz->vehicle_info.spn2566_battery_type == 0 ||
             (thiz->vehicle_info.spn2566_battery_type > 0x08 &&
              thiz->vehicle_info.spn2566_battery_type < 0xFF) ) {
            log_printf(WRN,
                   "BMS not recognized due to invalid BATTERY TYPE(SPN2566)");
            bit_clr(thiz, F_BMS_RECOGNIZED);
            break;
        }

        if ( thiz->vehicle_info.spn2567_capacity / 10.0f > 1000.0f ) {
            log_printf(WRN,
                   "BMS not recognized due to invalid CAP INFO(SPN2567)");
            bit_clr(thiz, F_BMS_RECOGNIZED);
            break;
        }

        if ( thiz->vehicle_info.spn2568_volatage / 10.0f > 750.0f ) {
            log_printf(WRN,
                  "BMS not recognized due to invalid VOLTAGE INFO(SPN2568)");
            bit_clr(thiz, F_BMS_RECOGNIZED);
            break;
        }
        log_printf(INF, "BMS recognized....CAP: %d A.H, VOL: %d V",
                   thiz->vehicle_info.spn2567_capacity,
                   thiz->vehicle_info.spn2568_volatage);
        if ( ! bit_read(thiz, F_BMS_RECOGNIZED ) ) {
            // send recognized event from here.
        }
        bit_set(thiz, F_BMS_RECOGNIZED);
        break;
    case PGN_BCP :// 0x000600, BMS 配置报文
        statistics[I_BCP].can_counter ++;
        statistics[I_BCP].can_silence = 0;
        if ( bit_read(task, S_BMS_COMM_DOWN) ) {
            log_printf(INF, "BMS: BMS 通信"GRN("恢复"));
        }
        bit_clr(task, S_BMS_COMM_DOWN);

        if ( param->buff_payload != 13 ) {
            log_printf(WRN, "BMS: BCP packet size crash, need 13 gave %d",
                       param->buff_payload);
            break;
        }
        memcpy(&thiz->bms_config_info, param->buff.rx_buff,
               sizeof(struct pgn1536_BCP));

        if ( thiz->bms_config_info.spn2816_max_charge_volatage_single_battery /
                100.0f > 24.0f ) {
            log_printf(WRN,
                       "BMS: max_charge_volatage_single_battery out of rang(0-24)"
                       "gave %.2f V (SPN2816)",
             thiz->bms_config_info.spn2816_max_charge_volatage_single_battery /
                       100.0f);
            break;
        }

        if ( thiz->bms_config_info.spn2817_max_charge_current / 10.0f >
             400.0f ) {
            log_printf(WRN, "BMS: max_charge_current out of rang(-400-0)"
                       "gave %.1f V (SPN2816)",
                     thiz->bms_config_info.spn2817_max_charge_current / 10.0f);
            break;
        }

        if ( thiz->bms_config_info.spn2818_total_energy / 10.0f > 1000.0f ) {
            log_printf(WRN, "BMS: total_energy out of rang(0-1000 KW.H)"
                       "gave %.1f KW.H (SPN2818)",
                     thiz->bms_config_info.spn2818_total_energy / 10.0f);
            break;
        }

        if ( thiz->bms_config_info.spn2819_max_charge_voltage / 10.0f >
                750.0f ) {
            log_printf(WRN, "BMS: max_charge_voltage out of rang(0-750 V)"
                       "gave %.1f V (SPN2819)",
                     thiz->bms_config_info.spn2819_max_charge_voltage / 10.0f);
            break;
        }

        log_printf(INF, "BMS: BCP done, BSVH: %d V, MAXi: %d A, "
                   "CAP: %d KW.H, M-V-C: %d V, M-T: %d C, CAP-statu: %d %%",
                   "V: %d V",
                   thiz->bms_config_info.spn2816_max_charge_volatage_single_battery,
                   thiz->bms_config_info.spn2817_max_charge_current,
                   thiz->bms_config_info.spn2818_total_energy,
                   thiz->bms_config_info.spn2819_max_charge_voltage,
                   thiz->bms_config_info.spn2820_max_temprature,
                   thiz->bms_config_info.spn2821_soc,
                   thiz->bms_config_info.spn2822_total_voltage);
        break;
    case PGN_BRO :// 0x000900, BMS 充电准备就绪报文
        statistics[I_BRO].can_counter ++;
        statistics[I_BRO].can_silence = 0;

        log_printf(INF, "BMS is %s for charge.",
                   param->buff.rx_buff[0] == 0x00 ?
                    "not ready" :
                    param->buff.rx_buff[0] == 0xAA ?
                    "ready" : "<unkown status>");
        if ( param->buff.rx_buff[0] == 0x00 ) {
            bit_clr(thiz, F_BMS_READY);
            bit_clr(thiz, F_CHARGER_READY);
        } else if ( param->buff.rx_buff[0] == 0xAA ) {
            bit_set(thiz, F_BMS_READY);
            bit_set(thiz, F_CHARGER_READY);
        } else {
            log_printf(WRN, "BMS: wrong can package data.");
        }
        break;
    case PGN_BCL :// 0x001000, BMS 电池充电需求报文
        statistics[I_BCL].can_counter ++;
        statistics[I_BCL].can_silence = 0;
        if ( bit_read(task, S_BMS_COMM_DOWN) ) {
            log_printf(INF, "BMS: BMS 通信"GRN("恢复"));
        }
        bit_clr(task, S_BMS_COMM_DOWN);

        memcpy(&thiz->bms_charge_need_now,
               param->buff.rx_buff, sizeof(struct pgn4096_BCL));
        if ( thiz->bms_charge_need_now.spn3072_need_voltage/10.0f > 750 ) {
            log_printf(WRN, "BMS: spn3072 range 0-750V gave: %d V",
                       thiz->bms_charge_need_now.spn3072_need_voltage);
        }
        if ( thiz->bms_charge_need_now.spn3073_need_current/10.0f > 400 ) {
            log_printf(WRN, "BMS: spn3073 range -400-0A gave: %d A",
                       thiz->bms_charge_need_now.spn3073_need_current);
        }

        log_printf(INF, "BMS: PGN_BCL fetched, V-need: %.1f V, I-need: %d mode: %s",
                   thiz->bms_charge_need_now.spn3072_need_voltage/10.0,
                   thiz->bms_charge_need_now.spn3073_need_current,
                   thiz->bms_charge_need_now.spn3074_charge_mode ==
                    CHARGE_WITH_CONST_VOLTAGE ? "恒压充电" :
                   thiz->bms_charge_need_now.spn3074_charge_mode ==
                        CHARGE_WITH_CONST_CURRENT ? "恒流充电" : "无效模式");
        break;
    case PGN_BCS :// 0x001100, BMS 电池充电总状态报文
        statistics[I_BCS].can_counter ++;
        statistics[I_BCS].can_silence = 0;

        log_printf(INF, "PGN_BCS fetched.");
        memcpy(&thiz->bms_all_battery_status, param->buff.rx_buff,
               sizeof(struct pgn4352_BCS));
        if (thiz->bms_all_battery_status.spn3075_charge_voltage/10.0 > 750.0f) {
            log_printf(WRN, "BMS: spn3075 range 0-750 gave: %.1f V",
                     thiz->bms_all_battery_status.spn3075_charge_voltage/10.0);
        }
        if (thiz->bms_all_battery_status.spn3076_charge_current/10.0 > 400.0f) {
            log_printf(WRN, "BMS: spn3076 range -400-0 gave: %.1f V",
                   -(thiz->bms_all_battery_status.spn3076_charge_current/10.0));
        }
        if (thiz->bms_all_battery_status.spn3077_max_v_g_number/100.0 > 24.0 ) {
            log_printf(WRN, "BMS: spn3077 range 0-24 gave: %.1f V",
                   -(thiz->bms_all_battery_status.spn3077_max_v_g_number/100.0));
        }
        if (thiz->bms_all_battery_status.spn3078_soc > 100 ) {
            log_printf(WRN, "BMS: spn3078 range 0%%-100%% gave: %d%%",
                   -(thiz->bms_all_battery_status.spn3078_soc));
        }
        break;
    case PGN_BSM :// 0x001300, 动力蓄电池状态信息报文
        statistics[I_BSM].can_counter ++;
        statistics[I_BSM].can_silence = 0;

        log_printf(INF, "BMS: PGN_BSM fetched.");
        memcpy(&thiz->bms_battery_status, param->buff.rx_buff,
               sizeof(struct pgn4864_BSM));
        if ( SINGLE_BATTERY_VOLTAGE_HIGH ==
             (thiz->bms_battery_status.remote_single&SINGLE_BATTERY_VOLTAGE_HIGH)){
        }
        if (SINGLE_BATTERY_VOLTAGE_LOW ==
             (thiz->bms_battery_status.remote_single&SINGLE_BATTERY_VOLTAGE_LOW)){

        }

        if (BATTERY_CHARGE_CURRENT_HIGH ==
             (thiz->bms_battery_status.remote_single&BATTERY_CHARGE_CURRENT_HIGH)){

        }
        if (BATTERY_CHARGE_CURRENT_LOW ==
             (thiz->bms_battery_status.remote_single&BATTERY_CHARGE_CURRENT_LOW)){

        }

        if (BATTERY_TEMPRATURE_HIGH ==
                (thiz->bms_battery_status.remote_single&BATTERY_TEMPRATURE_HIGH)){

        }
        if (BATTERY_TEMPRATURE_LOW ==
                (thiz->bms_battery_status.remote_single&BATTERY_TEMPRATURE_LOW)) {

        }

        if (INSULATION_FAULT ==
                (thiz->bms_battery_status.remote_single&INSULATION_FAULT)){

        }
        if (INSULATION_UNRELIABLE==
                (thiz->bms_battery_status.remote_single&INSULATION_UNRELIABLE)){

        }

        if (CONNECTOR_STATUS_FAULT==
                (thiz->bms_battery_status.remote_single&CONNECTOR_STATUS_FAULT)){

        }
        if (CONNECTOR_STATUS_UNRELIABLE==
                (thiz->bms_battery_status.remote_single&CONNECTOR_STATUS_UNRELIABLE)){

        }

        if (CHARGE_ALLOWED==
                (thiz->bms_battery_status.remote_single&CHARGE_ALLOWED)){

        }
        if (CHARGE_FORBIDEN==
                (thiz->bms_battery_status.remote_single&CHARGE_FORBIDEN)){

        }
        break;
    case PGN_BMV :// 0x001500, 单体动力蓄电池电压报文
        statistics[I_BMV].can_counter ++;
        statistics[I_BMV].can_silence = 0;

        log_printf(INF, "BMS: PGN_BMV fetched.");
        break;
    case PGN_BMT :// 0x001600, 单体动力蓄电池温度报文
        statistics[I_BMT].can_counter ++;
        statistics[I_BMT].can_silence = 0;

        log_printf(INF, "BMS: PGN_BMT fetched.");
        break;
    case PGN_BSP :// 0x001700, 动力蓄电池预留报文
        statistics[I_BSP].can_counter ++;
        statistics[I_BSP].can_silence = 0;

        log_printf(INF, "BMS: PGN_BSP fetched.");
        break;
    case PGN_BST :// 0x001900, BMS 中止充电报文
        statistics[I_BST].can_counter ++;
        statistics[I_BST].can_silence = 0;

        log_printf(INF, "BMS: PGN_BST fetched.");
        break;
    case PGN_BSD :// 0x001C00, BMS 统计数据报文
        statistics[I_BSD].can_counter ++;
        statistics[I_BSD].can_silence = 0;
        if ( bit_read(task, S_BMS_COMM_DOWN) ) {
            log_printf(INF, "BMS: BMS 通信"GRN("恢复"));
        }
        bit_clr(task, S_BMS_COMM_DOWN);

        log_printf(INF, "BMS: PGN_BSD fetched.");
        break;
    case PGN_BEM :// 0x001E00, BMS 错误报文
        statistics[I_BEM].can_counter ++;
        statistics[I_BEM].can_silence = 0;

        log_printf(INF, "BMS: PGN_BEM fetched.");
        break;
    default:
        log_printf(WRN, "BMS: un-recognized PGN %08X",
                   param->can_id);
        break;
    }
    return ERR_OK;
}

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
    struct event_struct param;
    unsigned char txbuff[32];
    int nbytes;
    if ( done == NULL ) done = &mydone;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    strcpy(ifr.ifr_name, "can1" );
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(s, (struct sockaddr *)&addr, sizeof(addr));

    log_printf(INF, "%s running...s=%d", __FUNCTION__, s);

    param.buff_payload = 0;
    param.evt_param = EVT_RET_INVALID;

    // 进行数据结构的初始化操作
    can_packet_callback(task, EVENT_CAN_INIT, &param);

    while ( ! *done ) {
        usleep(5000);

        /*
         * 写线程同时负责写数据和进行连接管理时的控制数据写出，这里需要对当前CAN的
         * 状态进行判定，当CAN处于CAN_NORMAL时进行普通的写操作，当CAN处于CAN_TP_RD
         * 时，采用EVENT_TX_REQUEST 当CAN处于CAN_TP_RD时采用EVENT_TX_TP_REQUEST
         */
        param.buff.tx_buff = txbuff;
        param.buff_size = sizeof(txbuff);
        param.evt_param = EVT_RET_INVALID;
        if ( task->can_bms_status & CAN_NORMAL ) {
            can_packet_callback(task, EVENT_TX_REQUEST, &param);
        } else if ( task->can_bms_status & CAN_TP_RD ) {
            switch ( task->can_bms_status & 0xF0 ) {
            case CAN_TP_CTS:
                can_packet_callback(task, EVENT_TX_TP_CTS, &param);
                break;
            case CAN_TP_TX:
            case CAN_TP_RX:
                break;
            case CAN_TP_ACK:
                can_packet_callback(task, EVENT_TX_TP_ACK, &param);
                break;
            case CAN_TP_ABRT:
                can_packet_callback(task, EVENT_TX_TP_ABRT, &param);
                break;
            default:
                log_printf(WRN, "BMS: can_bms_status crashed(%d).",
                           task->can_bms_status);
                continue;
                break;
            }
        } else if ( task->can_bms_status & CAN_TP_WR ) {
            // 当前协议没有用到
            log_printf(WRN, "BMS: CAN_TP_WRITE not implement.");
            continue;
        } else if ( task->can_bms_status == CAN_INVALID ) {
            log_printf(DBG_LV0, "BMS: invalid can_bms_status: %d.",
                       task->can_bms_status);
            continue;
        } else {
            log_printf(DBG_LV0, "BMS: invalid can_bms_status: %d.",
                       task->can_bms_status);
            continue;
        }

        if ( EVT_RET_OK != param.evt_param ) {
            continue;
        }

        param.evt_param = EVT_RET_INVALID;
        // 链接模式下的数据包发送不需要确认, 并且也不能被中止
        if ( task->can_bms_status == CAN_NORMAL ) {
            can_packet_callback(task, EVENT_TX_PRE, &param);
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
                can_packet_callback(task, EVENT_TX_FAILS, &param);
            } else {
                param.evt_param = EVT_RET_OK;
                can_packet_callback(task, EVENT_TX_DONE, &param);
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
        if ( task->can_bms_status == (CAN_TP_RD | CAN_TP_CTS) ) {
            task->can_bms_status = (CAN_TP_RD | CAN_TP_RX);
            log_printf(DBG_LV3, "BMS: ready for data transfer.");
        }
        // 应答结束
        if ( task->can_bms_status == (CAN_TP_RD | CAN_TP_ACK) ) {
            task->can_bms_status = CAN_NORMAL;
            log_printf(DBG_LV0, "BMS: connection closed normally.");
        }
        // 传输终止
        if ( task->can_bms_status == (CAN_TP_RD | CAN_TP_ABRT) ) {
            task->can_bms_status = CAN_NORMAL;
            log_printf(DBG_LV2, "BMS: connection aborted.");
        }
    }
    return NULL;
}

// CAN 数据发送报文
void Hachiko_CAN_TP_notify_proc(Hachiko_EVT evt, void *private,
                            const struct Hachiko_food *self)
{
    if ( evt == HACHIKO_TIMEOUT ) {
        log_printf(WRN, "BMS: CAN data transfer terminal due to time out.");
        task->can_bms_status = CAN_NORMAL;
    } else if ( evt == HACHIKO_DIE ) {

    }
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
    struct event_struct param;
    // 用于链接管理的数据缓冲
    unsigned char tp_buff[2048];

    // 缓冲区数据字节数
    unsigned int tp_cnt = 0;
    // 数据包个数
    unsigned int tp_packets_nr = 0;
    // 数据包字节总数
    unsigned int tp_packets_size = 0;
    // 数据包对应的PGN
    unsigned int tp_packet_PGN = 0;

    unsigned int dbg_packets = 0;

    task->can_tp_private.status = PRIVATE_INVALID;
    task->can_tp_bomb.private = (void *)&task->can_tp_private;

    if ( done == NULL ) done = &mydone;
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    strcpy(ifr.ifr_name, "can1" );
    ioctl(s, SIOCGIFINDEX, &ifr);
    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(s, (struct sockaddr *)&addr, sizeof(addr));
    log_printf(INF, "BMS: %s running...s=%d", __FUNCTION__, s);

    param.buff.rx_buff = tp_buff;
    param.buff_size = sizeof(tp_buff);
    param.buff_payload = 0;
    while ( ! *done ) {
        usleep(5000);

        if ( task->can_bms_status  == CAN_INVALID ) {
            continue;
        }

        memset(&frame, 0, sizeof(frame));
        nbytes = read(s, &frame, sizeof(struct can_frame));
        if ( (frame.can_id & 0xFFFF) != CAN_RCV_ID_MASK ) {
            #if 0
            log_printf(DBG_LV0, "BMS: id not accept %x", frame.can_id);
            #endif
            continue;
        }

        dbg_packets ++;

        if ( nbytes != sizeof(struct can_frame) ) {
            param.evt_param = EVT_RET_ERR;
            log_printf(DBG_LV3, "BMS: read frame error %x", frame.can_id);
            can_packet_callback(task, EVENT_RX_ERROR, &param);
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
            if ( (task->can_bms_status & CAN_TP_RD) != CAN_TP_RD ) {
                task->can_bms_status = CAN_NORMAL;
                log_printf(WRN, "BMS: timing crashed.");
                continue;
            }
            Hachiko_feed(&task->can_tp_bomb);
            memcpy(&tp_buff[ (frame.data[0] - 1) * 7 ], &frame.data[1], 7);
            log_printf(DBG_LV1, "BM data tansfer fetch the %dst packet.",
                       frame.data[0]);
            task->can_tp_param.tp_rcv_pack_nr ++;
            if ( task->can_tp_param.tp_rcv_pack_nr >=
                 task->can_tp_param.tp_pack_nr ) {
                // 数据接收完成后即可关闭定时器
                Hachiko_kill(&task->can_tp_bomb);

                param.buff_payload = task->can_tp_param.tp_size;
                param.evt_param = EVT_RET_INVALID;
                param.can_id = task->can_tp_param.tp_pgn;
                log_printf(DBG_LV3,
                           "BMS: data transfer complete PGN=%08X change to ACK",
                           task->can_tp_param.tp_pgn);
                can_packet_callback(task, EVENT_RX_DONE, &param);
                // 数据链接接受完成
                task->can_bms_status = CAN_TP_RD | CAN_TP_ACK;
            }
        } else if ( ((frame.can_id & 0x00FF0000) >> 16 ) == 0xEC ) {
            // Connection managment
            if ( 0x10 == frame.data[0] ) {
                if ( task->can_tp_buff_nr ) {
                    /*
                     * 数据传输太快，还没将缓冲区的数据发送出去
                     */
                    log_printf(WRN, "BMS: CAN data transfer too fast.");
                    continue;
                }
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

                if ( task->can_tp_private.status == PRIVATE_BUSY ) {
                    log_printf(WRN, "BMS: previous connection not exit,"
                               " do new connection instead.");
                    Hachiko_feed( &task->can_tp_bomb );
                } else {
                    task->can_tp_bomb.Hachiko_notify_proc =
                            Hachiko_CAN_TP_notify_proc;
                    // 根据SAE J1939-21中关于CAN总线数据传输链接的说明，中间传输
                    // 过程最大不超过1250ms
                    int ret = Hachiko_new( & task->can_tp_bomb,
                                           HACHIKO_ONECE, 1250,
                                           &task->can_tp_private);
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

                task->can_tp_param.tp_pack_nr = tp_packets_nr;
                task->can_tp_param.tp_size = tp_packets_size;
                task->can_tp_param.tp_pgn = tp_packet_PGN;
                task->can_tp_param.tp_rcv_bytes = 0;
                task->can_tp_param.tp_rcv_pack_nr = 0;
                task->can_bms_status = CAN_TP_RD | CAN_TP_CTS;
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
            can_packet_callback(task, EVENT_RX_DONE, &param);
            log_printf(DBG_LV0, "BMS: read a frame done. %08X", frame.can_id);
        }

        if ( task->can_bms_status == CAN_NORMAL ) {
        } else if ( task->can_bms_status == CAN_TP_RD ) {
            // CAN通信处于连接管理模式
        }
    }

    return NULL;
}

/* 充电阶段改变事件处理过程
 *
 * 充电阶段发生改变后，会影响系统中数据包的发送逻辑等其他数据通信逻辑
 * 因此需要用单独的事件捕捉函数来进行处理，作相应的状态机修改
 **/
void on_charge_stage_change(CHARGE_STAGE_CHANGE_EVENT evt,
                            CHARGE_STAGE pre,
                            struct charge_task *thiz)
{
    switch (evt) {
    case STAGE_CHANGE_2_INVALID:
        break;
    case STAGE_CHANGE_2_HANDSHAKING:
        break;
    case STAGE_CHANGE_2_CHARGING:
        break;
    case STAGE_CHANGE_2_DONE:
        break;
    case STAGE_CHANGE_2_EXIT:
        break;
    case STAGE_CHANGE_2_ABORT:
    default:
        break;
    }
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
// 握手-CRM-充电机辨识报文
int gen_packet_PGN256(struct charge_task * thiz, struct event_struct* param)
{
    struct can_pack_generator *gen = &generator[0];

    if ( 0 == bit_read(thiz, F_BMS_RECOGNIZED) ) {
        param->buff.tx_buff[0] = BMS_NOT_RECOGNIZED;
    } else {
        param->buff.tx_buff[0] = BMS_RECOGNIZED;
        bit_set(thiz, F_VEHICLE_RECOGNIZED);
    }

    param->buff.tx_buff[1] = 0x01;
    strcpy((char * __restrict__)&param->buff.tx_buff[2], "ZH-CN");
    param->buff.tx_buff[7] = 0xFF;
    param->buff_payload = gen->datalen;
    param->can_id = gen->prioriy << 26 | gen->pgn << 8 | CAN_TX_ID_MASK | CAN_EFF_FLAG;

    param->evt_param = EVT_RET_OK;

    statistics[I_BRM].can_counter ++;

    return 0;
}

// 配置-CTS-充电机发送时间同步信息
int gen_packet_PGN1792(struct charge_task * thiz, struct event_struct* param)
{
    struct can_pack_generator *gen = &generator[1];
    struct pgn1792_CTS cts;
    time_t timep;
    struct tm *p;

    time(&timep);
    p =localtime(&timep);
    if ( p == NULL ) {
        param->evt_param = EVT_RET_ERR;
        return 0;
    }
    cts.spn2823_bcd_sec = (((p->tm_sec / 10 ) & 0x0F ) << 4) |
            ((p->tm_sec % 10) & 0x0F);
    cts.spn2823_bcd_min = (((p->tm_min / 10 ) & 0x0F ) << 4) |
            ((p->tm_min % 10) & 0x0F);
    cts.spn2823_bcd_hour = (((p->tm_hour / 10 ) & 0x0F ) << 4) |
            ((p->tm_hour % 10) & 0x0F);
    cts.spn2823_bcd_day = (((p->tm_mday / 10 ) & 0x0F ) << 4) |
            ((p->tm_mday % 10) & 0x0F);
    cts.spn2823_bcd_mon = (((p->tm_mon / 10 ) & 0x0F ) << 4) |
            ((p->tm_mon % 10) & 0x0F);
    cts.spn2823_bcd_year_h = (((p->tm_year / 100 ) & 0x0F ) << 4) |
            ((p->tm_year % 100) & 0x0F);
    cts.spn2823_bcd_year_l = (((p->tm_year / 10 ) & 0x0F ) << 4) |
            ((p->tm_year % 10) & 0x0F);

    memset(param->buff.tx_buff, 0xFF, 8);
    memcpy(param->buff.tx_buff, &cts, sizeof(struct pgn1792_CTS));

    param->buff_payload = gen->datalen;
    param->can_id =  gen->prioriy << 26 | gen->pgn << 8 | CAN_TX_ID_MASK | CAN_EFF_FLAG;

    param->evt_param = EVT_RET_OK;

    statistics[I_CTS].can_counter ++;

    return 0;
}

// 配置-CML-充电机最大输出能力
int gen_packet_PGN2048(struct charge_task * thiz, struct event_struct* param)
{
    struct can_pack_generator *gen = &generator[2];
    struct pgn2048_CML cml;

    cml.spn2824_max_output_voltage = 7500;
    cml.spn2825_min_output_voltage = 2400;
    cml.spn2826_max_output_current = 3000;
    memset(param->buff.tx_buff, 0xFF, 8);
    memcpy((void * __restrict__)param->buff.rx_buff, &cml, sizeof(struct pgn2048_CML));

    param->buff_payload = gen->datalen;
    param->can_id =  gen->prioriy << 26 | gen->pgn << 8 | CAN_TX_ID_MASK | CAN_EFF_FLAG;

    param->evt_param = EVT_RET_OK;

    statistics[I_CML].can_counter ++;

    return 0;
}

// 配置-CRO-充电机输出准备就绪状态
int gen_packet_PGN2560(struct charge_task * thiz, struct event_struct* param)
{
    struct can_pack_generator *gen = &generator[3];
    struct pgn2560_CRO cro;

    cro.spn2830_charger_ready_for_charge = CHARGER_READY_FOR_CHARGE;

    memset(param->buff.tx_buff, 0xFF, 8);
    memcpy(param->buff.tx_buff, &cro, sizeof(struct pgn2560_CRO));

    param->buff_payload = gen->datalen;
    param->can_id =  gen->prioriy << 26 | gen->pgn << 8 | CAN_TX_ID_MASK | CAN_EFF_FLAG;

    param->evt_param = EVT_RET_OK;

    statistics[I_CRO].can_counter ++;

    return 0;
}

// 充电-CCS-充电机充电状态
int gen_packet_PGN4608(struct charge_task * thiz, struct event_struct* param)
{
    struct can_pack_generator *gen = &generator[4];
    struct pgn4608_CCS ccs;

    ccs.spn3081_output_voltage = 7500;
    ccs.spn3082_outpu_current  = 1200;
    ccs.spn3083_charge_time = 122;

    memset(param->buff.tx_buff, 0xFF, 8);
    memcpy((void * __restrict__)param->buff.rx_buff, &ccs, sizeof(struct pgn4608_CCS));

    param->buff_payload = gen->datalen;
    param->can_id =  gen->prioriy << 26 | gen->pgn << 8 | CAN_TX_ID_MASK | CAN_EFF_FLAG;

    param->evt_param = EVT_RET_OK;

    statistics[I_CCS].can_counter ++;

    return 0;
}

// 充电-CST-充电机中止充电
int gen_packet_PGN6656(struct charge_task * thiz, struct event_struct* param)
{
    struct can_pack_generator *gen = &generator[5];
    (void)thiz;
    (void)param;
    (void)gen;
    return 0;

}

// 结束-CSD-充电机统计数据
int gen_packet_PGN7424(struct charge_task * thiz, struct event_struct* param)
{
    struct can_pack_generator *gen = &generator[6];
    (void)thiz;
    (void)param;
    (void)gen;
    return 0;

}

// 错误-CEM-充电机错误报文
int gen_packet_PGN7936(struct charge_task * thiz, struct event_struct* param)
{
    struct can_pack_generator *gen = &generator[7];
    (void)thiz;
    (void)param;
    (void)gen;
    return 0;
}

