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

#include "../ontom/error.h"
#include "../thirdpart/sqlite/sqlite3.h"

#include "../lib/lj/list.h"
#include "../lib/lj/config.h"
#include "../lib/lj/Hachiko.h"
#include "../lib/lj/log.h"

#include "../lib/bms/father.h"
#include "../ontom/charge.h"
#include "magicbox.h"
#include "b2p.h"
#include "z2b.h"
//#include "upload.h"

int BJNR_initialize(struct pandoras_box *pb, struct pandora_client *pc);
int BJNR_working(struct pandoras_box *pb, struct pandora_client *pc);
int BJNR_pause(struct pandoras_box *pb, struct pandora_client *pc);
int BJNR_exit(struct pandoras_box *pb, struct pandora_client *pc);
int BJNR_destroy(struct pandoras_box *pb, struct pandora_client *pc);
static int BJNR_do_read(struct pandoras_box *pb, struct pandora_client *pc, struct socket_session *ss);
static int BJNR_do_write(struct pandoras_box *pb, struct pandora_client *pc, struct socket_session *ss);

static int process_frame_tx(struct pandoras_box *pb, struct pandora_client *pc, struct socket_session *ss);
static int process_frame_rx(struct pandoras_box *pb, struct pandora_client *pc, struct socket_session *ss);
static inline void __dump_buff_hex(unsigned char *hex, int len, unsigned int lv)
{
    char buff[1024] = {0};
    int i = 0 ,l = 0, j =0;

    while ( i < len) {
        //if ( j ++ < 20 )
            l += sprintf(&buff[l], "%02X ", hex[i]);
        //else {
            j = 0;
        //    l += sprintf(&buff[l], "%02X\n", hex[i]);
       // }
        i ++;
    }

    log_printf(lv, "BOX:%d [ %s ]", len, buff);
}

int BJNR_initialize(struct pandoras_box *pb, struct pandora_client *pc)
{
    int retry = 0, ret;
    struct socket_session *ss = (struct socket_session *)pc->_private;
    struct sockaddr_in server_addr;
#if 1
    if ( ss->s_srv > 10 ) {
        close(ss->s_srv);
        ss->s_srv = -1;
    }
#else
    if ( ss->s_srv != -1 ) {
        close(ss->s_srv);
        log_printf(INF, "BOX.SOCKET: %d", ss->s_srv);
        ss->s_srv = -1;
    }
#endif
    memset(pc->single, 0, sizeof(pc->single));
    bit_set(pc, CS_LOSS_CONN);

    retry = 0;
    if ( ss->last_conn_tsp && ss->last_conn_tsp + 5 > time(NULL) ) {
        return 1;
    }
_resocket:
    //if ( ss->s_srv == -1 || ss->s_srv == 0 ) {
	ss->s_srv = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //}
    if ( ss->s_srv == -1 && retry < 5 ) {
        retry ++;
        goto _resocket;
    } else {
        log_printf(INF, "创建套接字: %d", ss->s_srv);
    }

    retry = 0;
_reconnect:
    memset(&server_addr, 0, sizeof(server_addr));       /* Clear struct */
    server_addr.sin_family = AF_INET;                  /* Internet/IP */
    server_addr.sin_addr.s_addr = inet_addr(ss->s_srv_ip);  /* IP address */
    server_addr.sin_port = htons(ss->s_srv_port);       /* server port */

    ret = connect(ss->s_srv, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if ( ret != 0 && retry < 5 ) {
        retry ++;
        log_printf(WRN, "链接服务端失败,第 %d 次重试连接", retry);
        goto _reconnect;
    } else if ( retry >= 5 ) {
        //close(ss->s_srv);
        ss->last_conn_tsp = time(NULL);
		log_printf(WRN, "连接服务端失败 关闭套接字: %d!", ss->s_srv);
        return 1;
    } else if ( ret == 0 ){
        ss->last_conn_tsp = 0;
        ss->established_tsp = time(NULL);
        bit_clr(pc, CS_LOSS_CONN);
        log_printf(INF, "链接服务器成功!");
        ss->pack_seq = 1;
    } else {

    }

    return 0;
}

int BJNR_working(struct pandoras_box *pb, struct pandora_client *pc)
{
    struct socket_session *ss = (struct socket_session *)pc->_private;

    BJNR_do_write(pb, pc, ss);
    BJNR_do_read(pb, pc, ss);

    process_frame_tx(pb, pc, ss);
    process_frame_rx(pb, pc, ss);

    return 0;
}

int BJNR_pause(struct pandoras_box *pb, struct pandora_client *pc)
{
    if ( pb == NULL || pc == NULL ) return 1;

    return 0;
}

int BJNR_exit(struct pandoras_box *pb, struct pandora_client *pc)
{
    if ( pb == NULL || pc == NULL ) return 1;

    return 0;
}

int BJNR_destroy(struct pandoras_box *pb, struct pandora_client *pc)
{
    if ( pb == NULL || pc == NULL ) return 1;

    return 0;
}

static int BJNR_do_read(struct pandoras_box *pb, struct pandora_client *pc, struct socket_session *ss)
{

    fd_set rfds;
    int retval;
    struct timeval  tv;

    if ( pb == NULL || pc == NULL ) return 1;

    FD_ZERO(&rfds);
    FD_SET(ss->s_srv, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 500 * 1000; // 500 ms
    retval = select(ss->s_srv+1, &rfds, NULL, NULL, &tv);

    if ( retval == -1 ) {
        switch (errno) {
        case EBADF: // 无效的描述符，连接断开
            break;
        case EINTR: // 被打断
            break;
        case EINVAL: // 参数无效
            break;
        case ENOMEM: // 内存不足
            break;
        default:
            break;
        }
    } else if ( retval == 0 ) {
        // timeout
    } else if ( retval > 0 ) {
        if ( ss->rx_buff_size >= sizeof(ss->rx_buff) ) {
            memset(ss->rx_buff, 0, sizeof(ss->rx_buff));
            ss->rx_buff_size = 0;
        }
        retval = read(ss->s_srv,
                      &ss->rx_buff[ss->rx_buff_size],
                sizeof(ss->rx_buff) - ss->rx_buff_size);
        if ( retval > 0 ) {
            int frames = 0;
            ss->rx_buff_size += retval;

            frames = 0;
            do {
                if ( ss->rx_buff_size < 1 ) return 0;
                __dump_buff_hex(ss->rx_buff, ss->rx_buff_size, DBG_LV3);

                if ( ss->rx_buff[0] != 0xEA ) {
                    memset(ss->rx_buff,0, 16);
                    ss->rx_buff_size = 0;
                    ss->this_frame_len = 0;
                    return 0;
                }

                if ( ss->rx_buff[1] != 0xEB ) {
                    memset(ss->rx_buff,0, 16);
                    ss->rx_buff_size = 0;
                    ss->this_frame_len = 0;
                    return 0;
                }

                if ( ss->rx_buff_size > 6 ) {
                    switch (ss->rx_buff[5]) {
                    case 0x11: ss->this_frame_len = 6 + 1; break;
                    case 0x12: ss->this_frame_len = 6 + 1; break;
                    case 0x03: ss->this_frame_len = 6 + 4; break;
                    case 0x14: ss->this_frame_len = 6 + 1; break;
                    case 0x15: ss->this_frame_len = 6 + 1; break;
                    case 0x16: ss->this_frame_len = 6 + 5; break;
                    case 0x17: ss->this_frame_len = 6 + 48; break;
                    case 0x18: ss->this_frame_len = 6 + 9; break;
                    case 0x09: ss->this_frame_len = 6 + 76; break;
                    case 0x0A: ss->this_frame_len = 6 + 106; break;
                    case 0x0B: ss->this_frame_len = 6 + 24; break;
                    case 0x0C: ss->this_frame_len = 6 + 1; break;
                    case 0x0D: ss->this_frame_len = 6 + 1; break;
                    default:
                        memset(ss->rx_buff,0, 16);
                        ss->rx_buff_size = 0;
                        ss->this_frame_len = 0;
                        return 0;
                    }
                }

                frames ++;

                if ( ss->rx_buff_size > 6 && ss->this_frame_len <= ss->rx_buff_size ) {
                    int i;
                    // 当前帧接收完成
                    pthread_mutex_lock(&pc->rx_msg_lck);
                    pandora_frame_push( &pc->rx_msg_head, ss->rx_buff[5], ss->this_frame_len, ss->rx_buff);
                    pthread_mutex_unlock(&pc->rx_msg_lck);
                    for ( i = 0; i < ss->rx_buff_size - ss->this_frame_len; i ++ ) {
                        ss->rx_buff[ i ] = ss->rx_buff[ i + ss->this_frame_len ];
                    }
                    ss->rx_buff_size = ss->rx_buff_size - ss->this_frame_len;
                    ss->this_frame_len = 0;
                    __dump_buff_hex(ss->rx_buff, ss->rx_buff_size, DBG_LV2);
                    log_printf(INF, "帧接受完成, %d, %02X", ss->rx_buff_size, ss->rx_buff[5]);
                } else {
                    // 数据帧还未接收完成
                    return 0;
                }
            } while (ss->rx_buff_size > 6 && frames < 16 );

        } else {
            pc->stage = STAGE_INIT;
            log_printf(WRN, "连接断开!");
        }
    } else {

    }

    return 0;
}

static int BJNR_need_echo(struct pandoras_msg *msg)
{
    int i;
    msg_t need_echo_id[] = {0x01, 0x02, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00};
    if ( ! msg ) return 0;

    for ( i = 0; need_echo_id[i] != 0; i ++ ) {
        if ( need_echo_id[i] != msg->id ) continue;
        return need_echo_id[i];
    }

    return 0;
}

static int BJNR_do_write(struct pandoras_box *pb, struct pandora_client *pc, struct socket_session *ss)
{

    fd_set wfds;
    int retval;
    struct timeval  tv;
    unsigned char *p;

    if ( pb == NULL || pc == NULL ) return 1;

    FD_ZERO(&wfds);
    FD_SET(ss->s_srv, &wfds);
    tv.tv_sec = 0;
    tv.tv_usec = 500 * 1000; // 500 ms
    retval = select(ss->s_srv+1, NULL, &wfds, NULL, &tv);

    if ( retval == -1 ) {
        switch (errno) {
        case EBADF: // 无效的描述符，连接断开
            break;
        case EINTR: // 被打断
            break;
        case EINVAL: // 参数无效
            break;
        case ENOMEM: // 内存不足
            break;
        default:
            break;
        }
    } else if ( retval == 0 ) {
        // timeout
    } else if ( retval > 0 ) {
        // 可写
        pthread_mutex_lock(&pc->tx_msg_lck);
        struct pandoras_msg* msg =
                pandora_frame_pop(&pc->tx_msg_head);
        pthread_mutex_unlock (&pc->tx_msg_lck);

        if ( ! msg ) {
            return 0;
        }
        p = (unsigned char *)msg->msg_body;
        p[2] = (msg->msg_size >> 8) & 0xff;
        p[3] = msg->msg_size & 0xff;

        int wr = write(ss->s_srv, msg->msg_body, msg->msg_size);
        if ( wr != msg->msg_size ) {
            log_printf(WRN, "something must be wrong!");
        }

        __dump_buff_hex(msg->msg_body, msg->msg_size, DBG_LV3);

        if ( BJNR_need_echo(msg) ) {
            pthread_mutex_lock(&pc->pedding_msg_lck);
            list_ini(msg->nd);
            pandora_frame_join(&pc->msg_pedding, &msg->nd);
            pthread_mutex_unlock (&pc->pedding_msg_lck);
        } else {
            memset(msg, 0, sizeof(struct pandoras_msg));
            free(msg);
        }
    } else {

    }

    return 0;
}

// 10 times word to float
static float x10_high_word2lowfloat(unsigned short hi_x10_ushort)
{
    return b2l(hi_x10_ushort) / 10.0f;
}

static unsigned char get_floatbyte(float f, int i)
{
    volatile float real_float = f;
    unsigned char * p = (unsigned char *)&real_float;
    
    return *(p+i);
}

// 发送帧
static int process_frame_tx(struct pandoras_box *pb, struct pandora_client *pc, struct socket_session *ss)
{
    time_t tsp = time(NULL);
    int ret;
    unsigned char buff[2048] = {0};

    // 登录
    if ( ! bit_read(pc, CF_LOGED_IN) ) {
        char loginbuff[32] = {0};
        int ret;

        if ( bit_read(pc, CF_TX_LOGIN) ) return 0;

        memset(loginbuff, '0', 32);

        loginbuff[0] = 0xEA;
        loginbuff[1] = 0xEB;
        loginbuff[2] = 0;
        loginbuff[3] = 19;
        loginbuff[4] = ss->pack_seq ++;
        loginbuff[5] = 0x01;
        loginbuff[16] = 1;
        loginbuff[17] = 1;
        loginbuff[18] = 4;

        pthread_mutex_lock(&pc->tx_msg_lck);
        ret = pandora_frame_push(&pc->tx_msg_head, 0x01, 25, loginbuff);
        pthread_mutex_unlock (&pc->tx_msg_lck);
        if ( ret ) {
            log_printf(WRN, "BOX: send package failed!");
        } else {
            bit_set(pc, CF_TX_LOGIN);
        }

        ss->tx_tsp[ TSP_LOGIN ] = tsp;

        // 没有登录，不再发送其他数据包
        return 0;
    }

    // 心跳
    if ( tsp - ss->tx_tsp[ TSP_HEARTBEAT ] > 5 ) {
        char heartbuff[32] = {0};

        heartbuff[0] = 0xEA;
        heartbuff[1] = 0xEB;
        heartbuff[2] = 0;
        heartbuff[3] = 2;
        heartbuff[4] = ss->pack_seq ++;
        heartbuff[5] = 0x02;
        heartbuff[6] = ss->heart_beate >> 8;
        heartbuff[7] = ss->heart_beate;
        pthread_mutex_lock(&pc->tx_msg_lck);
        ret = pandora_frame_push(&pc->tx_msg_head, 0x02, 8, heartbuff);
        pthread_mutex_unlock (&pc->tx_msg_lck);

        if ( ret ) {
            log_printf(WRN, "BOX: send package failed!");
        }

        ss->heart_beate ++;

        ss->tx_tsp[ TSP_HEARTBEAT ] = tsp;
    }

    // 设备状态上报
    if ( tsp - ss->tx_tsp[ TSP_DEVSTATACK ] > 5 ) {
        ss->tx_tsp[ TSP_DEVSTATACK ] = tsp;
    }

    // 1#枪实时充电状态上报
    if ( tsp - ss->tx_tsp[ TSP_CHARGESTATACK ] > 5 ) {
        int need_send = 0, i;
        struct charge_job *job;
        unsigned char cs[128];
        int n = 0;
		static int soc = 1;
		int tmp;

        for ( i = 0; i < 2; i ++ ) {
            job = pb->tsk->job[i];
            if ( !job ) continue;
            n = 0;
            cs[ n ++ ] = 0xEA;
            cs[ n ++ ] = 0xEB;
            cs[ n ++ ] = 0;
            cs[ n ++ ] = 44;
            cs[ n ++ ] = ss->pack_seq ++;
            cs[ n ++ ] = 0x05;
            cs[ n ++ ] = job->job_gun_sn;
            cs[ n ++ ] = (double2short(job->charge_voltage, 10) >> 8) & 0xFF;
            cs[ n ++ ] = double2short(job->charge_voltage, 10) & 0xFF;
            cs[ n ++ ] = (double2short(job->charge_current, 10) >> 8) & 0xFF;
            cs[ n ++ ] = double2short(job->charge_current, 10) & 0xFF;
			tmp = (int)((job->charged_kwh + job->section_kwh)*10.0);
            cs[ n ++ ] = tmp >> 24;
            cs[ n ++ ] = tmp >> 16;
            cs[ n ++ ] = tmp >> 8;
            cs[ n ++ ] = tmp >> 0;

            if ( i == 0 ) {
                cs[ n ++ ] = bit_read(pb->tsk, S_BAT_0_VHI) ? 1 : 0;
                cs[ n ++ ] = bit_read(pb->tsk, S_BAT_0_VLO) ? 1 : 0;
                cs[ n ++ ] = bit_read(pb->tsk, S_BAT_0_IHI) ? 1 : 0;
            } else {
                cs[ n ++ ] = bit_read(pb->tsk, S_BAT_1_VHI) ? 1 : 0;
                cs[ n ++ ] = bit_read(pb->tsk, S_BAT_1_VHI) ? 1 : 0;
                cs[ n ++ ] = bit_read(pb->tsk, S_BAT_1_IHI) ? 1 : 0;
            }

            // 单体电压
            cs[ n ++ ] = 0;
            cs[ n ++ ] = 0;

            // 最低单体电压
            cs[ n ++ ] = 0;
            cs[ n ++ ] = 0;
            // 最高单体电压
            cs[ n ++ ] = (job->bms.bcs.spn3077_max_v_g_number & 0x0FFF) >> 8;
            cs[ n ++ ] = (job->bms.bcs.spn3077_max_v_g_number & 0x0FFF) & 0xFF;

            cs[ n ++ ] = job->bms.bsm.max_temperature_of_battery;
            cs[ n ++ ] = job->bms.bsm.min_temperature_of_battery;

            if ( bit_read(pb->tsk, S_ERROR) && 
				(
				  (job->job_status == JOB_WORKING ) ||
				  (job->job_status == JOB_ERR_PAUSE) ||
				  (job->job_status == JOB_RESUMING)
				)
			) {
                cs[ n ++ ] = 0x01;
            } else if (job->job_status <= JOB_WORKING) {
                cs[ n ++ ] = 0x03;
            } else {
				cs[ n ++ ] = 0x00;
                        }

            cs[ n ++ ] = job->bms.bcs.spn3078_soc;
            // 电池采样过温
            cs[ n ++ ] = (job->bms.bsm.remote_single & 0xC0) == 0x01 ? 1 : 0;

            // 是否连接电池, 电压高于50.0V 即认为连接了电池
            if ( i == 0 ) {
                if ( bit_read(pb->tsk, F_GUN_1_PHY_CONN_STATUS) ) {
                    cs[ n ++ ] = pb->tsk->measure[0]->measure.VinBAT0 > 500 ? 1 : 0;
                } else {
                    cs[ n ++ ] = 0;
                }
            } else {
                if ( bit_read(pb->tsk, F_GUN_2_PHY_CONN_STATUS) ) {
                    cs[ n ++ ] = pb->tsk->measure[0]->measure.VinBAT1 > 500 ? 1 : 0;
                } else {
                    cs[ n ++ ] = 0;
                }
            }
            cs[ n ++ ] = bit_read(job, JF_BMS_BRM_OK) ? 0 : 1;
            cs[ n ++ ] = bit_read(job, JS_BMS_COMM_ERR) ? 1 : 0;

            if ( i == 0 ) {
                cs[ n ++ ] = (job->tsk->measure[0]->measure.Flag_run1 >> 2) & 0xFF;
            } else {
                cs[ n ++ ] = (job->tsk->measure[0]->measure.Flag_run2 >> 2) & 0xFF;
            }

            cs[ n ++ ] = ((unsigned int)(job->need_V * 10) >> 24) & 0xFF;
            cs[ n ++ ] = ((unsigned int)(job->need_V * 10) >> 16) & 0xFF;
            cs[ n ++ ] = ((unsigned int)(job->need_V * 10) >> 8) & 0xFF;
            cs[ n ++ ] = ((unsigned int)(job->need_V * 10) >> 0) & 0xFF;

            cs[ n ++ ] = ((unsigned int)(job->need_I * 10) >> 24) & 0xFF;
            cs[ n ++ ] = ((unsigned int)(job->need_I * 10) >> 16) & 0xFF;
            cs[ n ++ ] = ((unsigned int)(job->need_I * 10) >> 8) & 0xFF;
            cs[ n ++ ] = ((unsigned int)(job->need_I * 10) >> 0) & 0xFF;

            cs[ n ++ ] = 0;
            cs[ n ++ ] = 0;
            cs[ n ++ ] = (job->bms.bcs.spn3079_need_time >> 8) & 0xFF;
            cs[ n ++ ] = job->bms.bcs.spn3079_need_time & 0xFF;

            if ( i == 0 ) {
                cs[ n ++ ] = bit_read(pb->tsk, CMD_GUN_1_OUTPUT_ON);
            } else {
                cs[ n ++ ] = bit_read(pb->tsk, CMD_GUN_2_OUTPUT_ON);
            }

            cs[ n ++ ] = 0x00;
            cs[ n ++ ] = 0x00;
            cs[ n ++ ] = 0x00;
            cs[ n ++ ] = 0x00;

            pthread_mutex_lock(&pc->tx_msg_lck);
            ret = pandora_frame_push(&pc->tx_msg_head, 0x05, n, cs);
            pthread_mutex_unlock (&pc->tx_msg_lck);

            need_send ++;
        }
        
        if ( need_send ) {
            ss->tx_tsp[ TSP_CHARGESTATACK ] = tsp;
        }
    }

    // 2#枪实时充电状态上报
    if ( tsp - ss->tx_tsp[ TSP_CHARGESTATACK ] > 5 ) {
        int need_send = 0, i;
        struct charge_job *job;
        unsigned char cs[128];
        int n = 0;
                static int soc = 1;
                int tmp;

        for ( i = 0; i < 2; i ++ ) {
            job = pb->tsk->job[i];
            if ( !job ) continue;
            n = 0;
            cs[ n ++ ] = 0xEA;
            cs[ n ++ ] = 0xEB;
            cs[ n ++ ] = 0;
            cs[ n ++ ] = 44;
            cs[ n ++ ] = ss->pack_seq ++;
            cs[ n ++ ] = 0x05;
            cs[ n ++ ] = job->job_gun_sn;
            cs[ n ++ ] = (double2short(job->charge_voltage, 10) >> 8) & 0xFF;
            cs[ n ++ ] = double2short(job->charge_voltage, 10) & 0xFF;
            cs[ n ++ ] = (double2short(job->charge_current, 10) >> 8) & 0xFF;
            cs[ n ++ ] = double2short(job->charge_current, 10) & 0xFF;
                        tmp = (int)((job->charged_kwh + job->section_kwh)*10.0);
            cs[ n ++ ] = tmp >> 24;
            cs[ n ++ ] = tmp >> 16;
            cs[ n ++ ] = tmp >> 8;
            cs[ n ++ ] = tmp >> 0;

            if ( i == 0 ) {
                cs[ n ++ ] = bit_read(pb->tsk, S_BAT_0_VHI) ? 1 : 0;
                cs[ n ++ ] = bit_read(pb->tsk, S_BAT_0_VLO) ? 1 : 0;
                cs[ n ++ ] = bit_read(pb->tsk, S_BAT_0_IHI) ? 1 : 0;
            } else {
                cs[ n ++ ] = bit_read(pb->tsk, S_BAT_1_VHI) ? 1 : 0;
                cs[ n ++ ] = bit_read(pb->tsk, S_BAT_1_VHI) ? 1 : 0;
                cs[ n ++ ] = bit_read(pb->tsk, S_BAT_1_IHI) ? 1 : 0;
            }

            // 单体电压
            cs[ n ++ ] = 0;
            cs[ n ++ ] = 0;

            // 最低单体电压
            cs[ n ++ ] = 0;
            cs[ n ++ ] = 0;
            // 最高单体电压
            cs[ n ++ ] = (job->bms.bcs.spn3077_max_v_g_number & 0x0FFF) >> 8;
            cs[ n ++ ] = (job->bms.bcs.spn3077_max_v_g_number & 0x0FFF) & 0xFF;

            cs[ n ++ ] = job->bms.bsm.max_temperature_of_battery;
            cs[ n ++ ] = job->bms.bsm.min_temperature_of_battery;

            if ( bit_read(pb->tsk, S_ERROR) &&
                                (
                                  (job->job_status == JOB_WORKING ) ||
                                  (job->job_status == JOB_ERR_PAUSE) ||
                                  (job->job_status == JOB_RESUMING)
                                )
                        ) {
                cs[ n ++ ] = 0x01;
            } else if (job->job_status <= JOB_WORKING) {
                cs[ n ++ ] = 0x03;
            } else {
                                cs[ n ++ ] = 0x00;
                        }

            cs[ n ++ ] = job->bms.bcs.spn3078_soc;
            // 电池采样过温
            cs[ n ++ ] = (job->bms.bsm.remote_single & 0xC0) == 0x01 ? 1 : 0;

            // 是否连接电池, 电压高于50.0V 即认为连接了电池
            if ( i == 0 ) {
                if ( bit_read(pb->tsk, F_GUN_1_PHY_CONN_STATUS) ) {
                    cs[ n ++ ] = pb->tsk->measure[0]->measure.VinBAT0 > 500 ? 1 : 0;
                } else {
                    cs[ n ++ ] = 0;
                }
            } else {
                if ( bit_read(pb->tsk, F_GUN_2_PHY_CONN_STATUS) ) {
                    cs[ n ++ ] = pb->tsk->measure[0]->measure.VinBAT1 > 500 ? 1 : 0;
                } else {
                    cs[ n ++ ] = 0;
                }
            }
            cs[ n ++ ] = bit_read(job, JF_BMS_BRM_OK) ? 0 : 1;
            cs[ n ++ ] = bit_read(job, JS_BMS_COMM_ERR) ? 1 : 0;

            if ( i == 0 ) {
                cs[ n ++ ] = (job->tsk->measure[0]->measure.Flag_run1 >> 2) & 0xFF;
            } else {
                cs[ n ++ ] = (job->tsk->measure[0]->measure.Flag_run2 >> 2) & 0xFF;
            }

            cs[ n ++ ] = ((unsigned int)(job->need_V * 10) >> 24) & 0xFF;
            cs[ n ++ ] = ((unsigned int)(job->need_V * 10) >> 16) & 0xFF;
            cs[ n ++ ] = ((unsigned int)(job->need_V * 10) >> 8) & 0xFF;
            cs[ n ++ ] = ((unsigned int)(job->need_V * 10) >> 0) & 0xFF;

            cs[ n ++ ] = ((unsigned int)(job->need_I * 10) >> 24) & 0xFF;
            cs[ n ++ ] = ((unsigned int)(job->need_I * 10) >> 16) & 0xFF;
            cs[ n ++ ] = ((unsigned int)(job->need_I * 10) >> 8) & 0xFF;
            cs[ n ++ ] = ((unsigned int)(job->need_I * 10) >> 0) & 0xFF;

            cs[ n ++ ] = 0;
            cs[ n ++ ] = 0;
            cs[ n ++ ] = (job->bms.bcs.spn3079_need_time >> 8) & 0xFF;
            cs[ n ++ ] = job->bms.bcs.spn3079_need_time & 0xFF;

            if ( i == 0 ) {
                cs[ n ++ ] = bit_read(pb->tsk, CMD_GUN_1_OUTPUT_ON);
            } else {
                cs[ n ++ ] = bit_read(pb->tsk, CMD_GUN_2_OUTPUT_ON);
            }

            cs[ n ++ ] = 0x00;
            cs[ n ++ ] = 0x00;
            cs[ n ++ ] = 0x00;
            cs[ n ++ ] = 0x00;

            pthread_mutex_lock(&pc->tx_msg_lck);
            ret = pandora_frame_push(&pc->tx_msg_head, 0x05, n, cs);
            pthread_mutex_unlock (&pc->tx_msg_lck);

            need_send ++;
        }

        if ( need_send ) {
            ss->tx_tsp[ TSP_CHARGESTATACK ] = tsp;
        }
    }

    // 计费请求
   do {
        int i = 0;
        struct charge_job *job;
        unsigned char bl[256] = {0};
        int n = 0;
        size_t kwh;
#if 0
        if ( !bit_read(pb->tsk, F_NEED_BILLING) ) break;
#endif
        for ( i = 0; i < 2; i ++ ) {
            job = pb->tsk->job[i];

            if ( ! job ) continue;
            if ( job->job_status != JOB_DONE ) continue;
            if ( bit_read(job, JF_BILLING_DONE) ) continue;
            if ( bit_read(job, JF_BILLING_TX) ) continue;
            if ( !bit_read(job, JS_BILLING_TIMEOUT) ) continue;

            n = 0;
            bl[ n ++ ] = 0xEA;
            bl[ n ++ ] = 0xEB;
            bl[ n ++ ] = 0;
            bl[ n ++ ] = 172;
            bl[ n ++ ] = ss->pack_seq ++;
            bl[ n ++ ] = 0x08;

            memcpy(&bl[n], job->charge_billing.billing_sn, 32);
            n = n + 32;
            bl[ n ++ ] = job->job_gun_sn;
            bl[ n ++ ] = job->charge_billing.account_type;
            n = n + 32 + 8 + 17 + 4 + 4;

            kwh = (size_t)((job->charged_kwh + job->section_kwh) * 10);
            bl[ n ++ ] = (kwh >> 24) & 0xFF;
            bl[ n ++ ] = (kwh >> 16) & 0xFF;
            bl[ n ++ ] = (kwh >> 8 ) & 0xFF;
            bl[ n ++ ] = (kwh >> 0 ) & 0xFF;

            n = 178;

            pthread_mutex_lock(&pc->tx_msg_lck);
            ret = pandora_frame_push(&pc->tx_msg_head, 0x08, n, bl);
            pthread_mutex_unlock (&pc->tx_msg_lck);

            bit_set(job, JF_BILLING_TX);
            log_printf(INF, "发送计费请求");
        }
    } while (0);

    // 账户认证
    do {
        char auth[64] = {0};
        int n = 0;
        // 离线式不进行认证
        if ( ! bit_read(pb->tsk, F_SYSTEM_COMM_MODE) ) break; // 不需要认证
        if ( ! pb->tsk->auth.auth_request_tsp ) break; // 未提出请求
        if ( pb->tsk->auth.auth_tx_tsp ) break; // 已发送请求

        n = 0;
        auth[ n ++ ] = 0xEA;
        auth[ n ++ ] = 0xEB;
        auth[ n ++ ] = 0;
        auth[ n ++ ] = 0;
        auth[ n ++ ] = ss->pack_seq ++;
        auth[ n ++ ] = 0x06;

        auth[ n ++ ] = pb->tsk->auth.card_type;
        memcpy(&auth[n], pb->tsk->auth.card_sn, 20);
        n = n + 20;
        memcpy(&auth[n], pb->tsk->auth.user_input_passwd, 16);

        pthread_mutex_lock(&pc->tx_msg_lck);
        ret = pandora_frame_push(&pc->tx_msg_head, 0x06, n, auth);
        pthread_mutex_unlock (&pc->tx_msg_lck);
        log_printf(INF, "发送账户认证请求");

        pb->tsk->auth.auth_tx_tsp = time(NULL);
    } while (0);

    // 充电请求
    do {
        char request[128] = {0};
        int n = 0;
        if ( ! bit_read(pb->tsk, F_SYSTEM_COMM_MODE) ) break; // 不需要认证
        if ( ! pb->tsk->request.request_tsp ) break;
        if ( pb->tsk->request.tx_request_tsp ) break; // 已经发送

        bit_clr(pc, CS_REQUEST_CHARGE_TIMEOUT);
        pb->tsk->request.ack_tsp = 0;

        n = 0;
        request[ n ++ ] = 0xEA;
        request[ n ++ ] = 0xEB;
        request[ n ++ ] = 0;
        request[ n ++ ] = 0;
        request[ n ++ ] = ss->pack_seq ++;
        request[ n ++ ] = 0x07;

        request[ n ++ ] = pb->tsk->request.gun;

        memcpy(&request[n], pb->tsk->request.vin, 17);
        n += 17;

        memcpy(&request[n], pb->tsk->request.licence, 8);
        n += 8;

        request[ n ++ ] = pb->tsk->request.card_type;

        memcpy(&request[n], pb->tsk->request.account, 32);
        n += 32;

        memcpy(&request[n], pb->tsk->request.passwd, 16);
        n += 16;

        request[ n ++ ] = pb->tsk->request.service_mode;
        request[ n ++ ] = pb->tsk->request.service_param >> 8;
        request[ n ++ ] = pb->tsk->request.service_param & 0xFF;

        pthread_mutex_lock(&pc->tx_msg_lck);
        ret = pandora_frame_push(&pc->tx_msg_head, 0x07, n, request);
        pthread_mutex_unlock (&pc->tx_msg_lck);
        log_printf(INF, "发送充电请求");

        pb->tsk->request.tx_request_tsp = time(NULL);
    } while (0);

#define PATCH_VALUE(id, signal) do {\
            buff[ n ++ ] = id >> 24;\
            buff[ n ++ ] = id >> 16;\
            buff[ n ++ ] = id >> 8;\
            buff[ n ++ ] = id >> 0;\
            buff[ n ++ ] = signal >> 24;\
            buff[ n ++ ] = signal >> 16;\
            buff[ n ++ ] = signal >> 8;\
            buff[ n ++ ] = signal >> 0;\
            point_count ++;\
        } while (0)
#define PATCH_YX(id, signal) do {\
        buff[ n ++ ] = id >> 24;\
        buff[ n ++ ] = id >> 16;\
        buff[ n ++ ] = id >> 8;\
        buff[ n ++ ] = id >> 0;\
		buff[n++] = get_floatbyte(signal * 1.0f, 3); \
		buff[n++] = get_floatbyte(signal * 1.0f, 2); \
		buff[n++] = get_floatbyte(signal * 1.0f, 1); \
		buff[n++] = get_floatbyte(signal * 1.0f, 0); \
		point_count++; \
    } while (0)
#define PATCH_YC PATCH_VALUE
#define PATCH_FYC(id, v) do {\
        buff[ n ++ ] = id >> 24;\
        buff[ n ++ ] = id >> 16;\
        buff[ n ++ ] = id >> 8;\
        buff[ n ++ ] = id >> 0;\
        buff[ n ++ ] = get_floatbyte(v, 3);\
        buff[ n ++ ] = get_floatbyte(v, 2);\
        buff[ n ++ ] = get_floatbyte(v, 1);\
        buff[ n ++ ] = get_floatbyte(v, 0);\
        point_count ++;\
   } while (0)

    // 系统遥信, T = 10S
    do {
        int point_count = 0;
        int count_index = 0;
        int n = 0;

        if ( tsp - ss->tx_tsp[TSP_SYSTEM_YX] < 10 ) break;

        buff[ n ++ ] = 0xEA;
        buff[ n ++ ] = 0xEB;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = ss->pack_seq ++;
        buff[ n ++ ] = 0x0E;
        count_index = n;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;

        do {
			// 傻逼，遥信用浮点数？！
            PATCH_YX(0x20, (float)bit_read(task, F_GUN_1_PHY_CONN_STATUS));
			PATCH_YX(0x24, (float)bit_read(task, CMD_GUN_1_OUTPUT_ON));
			PATCH_YX(0x28, (float)bit_read(task, CMD_GUN_1_ASSIT_PWN_ON));
			PATCH_YX(0x2C, (float)bit_read(task, CMD_GUN_1_LOCK_ON));
        } while ( 0 );

        if ( task->sys_config_gun_nr == 2 ) {
			PATCH_YX(0x21, (float)bit_read(task, F_GUN_2_PHY_CONN_STATUS));
			PATCH_YX(0x25, (float)bit_read(task, CMD_GUN_2_OUTPUT_ON));
			PATCH_YX(0x29, (float)bit_read(task, CMD_GUN_2_ASSIT_PWN_ON));
			PATCH_YX(0x2D, (float)bit_read(task, CMD_GUN_2_LOCK_ON));
        }

        buff[count_index] = point_count >> 8;
        buff[count_index+1] = point_count >> 0;

        pthread_mutex_lock(&pc->tx_msg_lck);
        ret = pandora_frame_push(&pc->tx_msg_head, 0x0E, n, buff);
        pthread_mutex_unlock (&pc->tx_msg_lck);
        log_printf(INF, "发送系统遥信");
        ss->tx_tsp[TSP_SYSTEM_YX] = tsp;
    } while (0);

    // 系统故障遥信, T = 10S
    do {
        int point_count = 0;
        int count_index = 0;
        int n = 0, i = 0;
        unsigned int ei = 0;

        if ( ! bit_read(pb->tsk, S_ERROR) ) break;
        if ( tsp - ss->tx_tsp[TSP_SYSTEM_ERR_YX] < 10 ) break;

        buff[ n ++ ] = 0xEA;
        buff[ n ++ ] = 0xEB;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = ss->pack_seq ++;
        buff[ n ++ ] = 0x0E;
        count_index = n;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;

        for ( i = 0, ei = S_ERROR; ei < S_END; ei ++, i ++ ) {
            if ( bit_read(pb->tsk, ei) ) {
                PATCH_YX(i + 0x40, 1);
            }
        }
        buff[count_index] = point_count >> 8;
        buff[count_index+1] = point_count >> 0;

        pthread_mutex_lock(&pc->tx_msg_lck);
        ret = pandora_frame_push(&pc->tx_msg_head, 0x0E, n, buff);
        pthread_mutex_unlock (&pc->tx_msg_lck);
        log_printf(INF, "发送系统故障遥信");
        ss->tx_tsp[TSP_SYSTEM_ERR_YX] = tsp;
    } while (0);

    // 作业遥信, T = 8S
    do {
        int point_count = 0;
        int count_index = 0;
        int n = 0, i = 0;
        unsigned int ei = 0;

        if ( tsp - ss->tx_tsp[TSP_JOB_YX] < 8 ) break;
		buff[n++] = 0xEA;
		buff[n++] = 0xEB;
		buff[n++] = 0;
		buff[n++] = 0;
		buff[n++] = ss->pack_seq++;
		buff[n++] = 0x0E;
		count_index = n;
		buff[n++] = 0;
		buff[n++] = 0;
		if (!pb->tsk->job[0] && !pb->tsk->job[1]) {
			break;
		} else {
			if (pb->tsk->job[0]) {
				if (pb->tsk->job[0]->job_status >= JOB_STANDBY &&
					pb->tsk->job[0]->job_status < JOB_EXITTING) {
					PATCH_YX(0xA5, (float)1);
				}
				else {
					PATCH_YX(0xA5, (float)0);
				}
				if (bit_read(pb->tsk->job[0], JF_BMS_BRM_OK)) {
					PATCH_YX(0xA9, (float)1);
				}
				else {
					PATCH_YX(0xA9, (float)0);
				}
				if (pb->tsk->job[0]->job_status == JOB_WORKING) {
					PATCH_YX(0xAD, (float)1);
				}
				else {
					PATCH_YX(0xAD, (float)0);
				}
				if (bit_read(pb->tsk->job[0], JS_BMS_COMM_ERR)) {
					PATCH_YX(0xB1, (float)1);
				}
				else {
					PATCH_YX(0xB1, (float)0);
				}
			}
			if (pb->tsk->job[1]) {
				if (pb->tsk->job[1]->job_status >= JOB_STANDBY &&
					pb->tsk->job[1]->job_status < JOB_EXITTING) {
					PATCH_YX(0xA6, (float)1);
				}
				else {
					PATCH_YX(0xA6, (float)0);
				}
				if (bit_read(pb->tsk->job[1], JF_BMS_BRM_OK)) {
					PATCH_YX(0xAA, (float)1);
				}
				else {
					PATCH_YX(0xAA, (float)0);
				}
				if (pb->tsk->job[1]->job_status == JOB_WORKING) {
					PATCH_YX(0xAE, (float)1);
				}
				else {
					PATCH_YX(0xAE, (float)0);
				}
				if (bit_read(pb->tsk->job[1], JS_BMS_COMM_ERR)) {
					PATCH_YX(0xB2, (float)1);
				}
				else {
					PATCH_YX(0xB2, (float)0);
				}
			}
		}
        buff[count_index] = point_count >> 8;
        buff[count_index+1] = point_count >> 0;

        pthread_mutex_lock(&pc->tx_msg_lck);
        ret = pandora_frame_push(&pc->tx_msg_head, 0x0E, n, buff);
        pthread_mutex_unlock (&pc->tx_msg_lck);
        log_printf(INF, "发送作业遥信");
        ss->tx_tsp[TSP_JOB_YX] = tsp;
    } while (0);

    // 系统遥测, T=5S
    do {
        int point_count = 0;
        int count_index = 0;
        int n = 0, i = 0;
        unsigned int ei = 0;
	unsigned short v;

        if ( tsp - ss->tx_tsp[TSP_SYSTEM_YC] < 5 ) break;

        buff[ n ++ ] = 0xEA;
        buff[ n ++ ] = 0xEB;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = ss->pack_seq ++;
        buff[ n ++ ] = 0x0E;
        count_index = n;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;

        //
	v = pb->tsk->measure[0]->measure.VinKM0;
        PATCH_FYC(0xC0, x10_high_word2lowfloat(v));
	v = pb->tsk->measure[0]->measure.IoutBAT0;
        PATCH_FYC(0xC1, x10_high_word2lowfloat(v));
	v = pb->tsk->measure[0]->measure.VinBAT0;
        PATCH_FYC(0xC2, x10_high_word2lowfloat(v));
	v = pb->tsk->measure[0]->measure.IoutBAT0;
        PATCH_FYC(0xC3, x10_high_word2lowfloat(v));
	v = pb->tsk->measure[0]->measure.VinBAT0RESP;
        PATCH_FYC(0xC4, x10_high_word2lowfloat(v));
	v = pb->tsk->measure[0]->measure.VinBAT0RESN;
        PATCH_FYC(0xC5, x10_high_word2lowfloat(v));

	v = pb->tsk->measure[0]->measure.VinKM1;
        PATCH_FYC(0xC6, x10_high_word2lowfloat(v));
	v = pb->tsk->measure[0]->measure.IoutBAT1;
        PATCH_FYC(0xC7, x10_high_word2lowfloat(v));
	v = pb->tsk->measure[0]->measure.VinBAT1;
        PATCH_FYC(0xC8, x10_high_word2lowfloat(v));
	v = pb->tsk->measure[0]->measure.IoutBAT1;
        PATCH_FYC(0xC9, x10_high_word2lowfloat(v));
	v = pb->tsk->measure[0]->measure.VinBAT1RESP;
        PATCH_FYC(0xCA, x10_high_word2lowfloat(v));
	v = pb->tsk->measure[0]->measure.VinBAT1RESN;
        PATCH_FYC(0xCB, x10_high_word2lowfloat(v));

        PATCH_FYC(0xCC, (float)(pb->tsk->meter[0].kwh_zong));
        PATCH_FYC(0xCD, (float)(pb->tsk->meter[0].kwh_jian));
        PATCH_FYC(0xCE, (float)(pb->tsk->meter[0].kwh_feng));
        PATCH_FYC(0xCF, (float)(pb->tsk->meter[0].kwh_ping));
        PATCH_FYC(0xD0, (float)(pb->tsk->meter[0].kwh_gu));

        // 暂时现不上送交流输入信息

        buff[count_index] = point_count >> 8;
        buff[count_index+1] = point_count >> 0;

        pthread_mutex_lock(&pc->tx_msg_lck);
        ret = pandora_frame_push(&pc->tx_msg_head, 0x0E, n, buff);
        pthread_mutex_unlock (&pc->tx_msg_lck);
        log_printf(INF, "发送系统遥测");

        ss->tx_tsp[TSP_SYSTEM_YC] = tsp;
    } while (0);

    // 作业遥测
    do {
        int point_count = 0;
        int count_index = 0;
        int n = 0, i = 0;
        unsigned int ei = 0;
	static float test = 1.1f;
	
	test = test + 1.0f;

        if ( tsp - ss->tx_tsp[TSP_JOB_YC] < 5 ) break;
	if ( ! pb->tsk->job[0] && ! pb->tsk->job[1] ) break;

        buff[ n ++ ] = 0xEA;
        buff[ n ++ ] = 0xEB;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = ss->pack_seq ++;
        buff[ n ++ ] = 0x0E;
        count_index = n;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;

        if ( pb->tsk->job[0] ) {
            PATCH_YC(0xD6, bb2ll(pb->tsk->job[0]->jid));
            if ( pb->tsk->job[0]->job_status == JOB_WORKING ) {
                PATCH_YC(0xD7, bb2ll(2));
            } else if ( pb->tsk->job[0]->job_status == JOB_ERR_PAUSE ) {
                PATCH_YC(0xD7, bb2ll(1));
            } else {
                PATCH_YC(0xD7, 0);
            }
            PATCH_YC(0xD8, bb2ll(pb->tsk->job[0]->jid));
            PATCH_FYC(0xDA, (float)(pb->tsk->job[0]->section_seconds+pb->tsk->job[0]->charged_seconds)/60.0f);
            PATCH_FYC(0xDB, (float)(pb->tsk->job[0]->section_kwh+pb->tsk->job[0]->charged_kwh));
            PATCH_FYC(0xDE, x10_high_word2lowfloat(pb->tsk->measure[0]->measure.VinKM0));
            PATCH_FYC(0xDF, x10_high_word2lowfloat(pb->tsk->measure[0]->measure.IoutBAT0));
        }

        if ( pb->tsk->job[1] ) {
            PATCH_YC(0xE7, bb2ll(pb->tsk->job[1]->jid));
            if ( pb->tsk->job[1]->job_status == JOB_WORKING ) {
                PATCH_YC(0xE8, bb2ll(2));
            } else if ( pb->tsk->job[1]->job_status == JOB_ERR_PAUSE ) {
                PATCH_YC(0xE8, bb2ll(1));
            } else {
                PATCH_YC(0xE8, 0);
            }
            PATCH_YC(0xE9, bb2ll(pb->tsk->job[1]->jid));
            PATCH_FYC(0xEB, (float)(pb->tsk->job[1]->section_seconds+pb->tsk->job[1]->charged_seconds)/60.0f);
            PATCH_FYC(0xEC, (float)(pb->tsk->job[1]->section_kwh+pb->tsk->job[1]->charged_kwh));
	    PATCH_FYC(0xEF, x10_high_word2lowfloat(pb->tsk->measure[0]->measure.VinKM1));
            PATCH_FYC(0xF0, x10_high_word2lowfloat(pb->tsk->measure[0]->measure.IoutBAT1));	  
	}

        buff[count_index] = point_count >> 8;
        buff[count_index+1] = point_count >> 0;

        pthread_mutex_lock(&pc->tx_msg_lck);
        ret = pandora_frame_push(&pc->tx_msg_head, 0x0E, n, buff);
        pthread_mutex_unlock (&pc->tx_msg_lck);
        log_printf(INF, "发送作业遥测");

        ss->tx_tsp[TSP_JOB_YC] = tsp;
    } while (0);

    // 1#枪BMS遥测
    do {
        int point_count = 0;
        int count_index = 0;
        int n = 0, i = 0, v;
        unsigned int ei = 0;
	static float test = 1.1f;
	
	test = test + 1.0f;

        if ( tsp - ss->tx_tsp[TSP_1_BMS_YC] < 5 ) break;
        if ( ! pb->tsk->job[0] ) break;

        buff[ n ++ ] = 0xEA;
        buff[ n ++ ] = 0xEB;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = ss->pack_seq ++;
        buff[ n ++ ] = 0x0E;
        count_index = n;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;

        // BRM
        v = pb->tsk->job[0]->bms.brm.spn2565_bms_version[0] << 16 |
                pb->tsk->job[0]->bms.brm.spn2565_bms_version[1] << 8 |
                pb->tsk->job[0]->bms.brm.spn2565_bms_version[2];
        PATCH_YC(0x00F7, v);
        PATCH_YC(0x00F8, pb->tsk->job[0]->bms.brm.spn2566_battery_type);
        PATCH_FYC(0x00F9, pb->tsk->job[0]->bms.brm.spn2567_capacity/10.0f);
        PATCH_FYC(0x00FA, pb->tsk->job[0]->bms.brm.spn2568_volatage/10.0f);
        PATCH_YC(0x00FB, *(unsigned int*)pb->tsk->job[0]->bms.brm.spn2569_battery_manufacturer);
        PATCH_YC(0x00FC, *(unsigned int*)pb->tsk->job[0]->bms.brm.spn2570_battery_sn);
        PATCH_YC(0x00FD, pb->tsk->job[0]->bms.brm.spn2571_year);
        PATCH_YC(0x00FE, pb->tsk->job[0]->bms.brm.spn2571_month);
        PATCH_YC(0x00FF, pb->tsk->job[0]->bms.brm.spn2571_day);
        v = pb->tsk->job[0]->bms.brm.spn2572_charge_count[0] << 16 |
                pb->tsk->job[0]->bms.brm.spn2572_charge_count[1] << 8 |
                pb->tsk->job[0]->bms.brm.spn2572_charge_count[2];
        PATCH_YC(0x0100, v);
        PATCH_YC(0x0101, pb->tsk->job[0]->bms.brm.spn2573_battery_property);

        // BCP
        PATCH_FYC(0x0103, pb->tsk->job[0]->bms.bcp.spn2816_max_charge_volatage_single_battery/100.0f);
        PATCH_FYC(0x0104, (pb->tsk->job[0]->bms.bcp.spn2817_max_charge_current-4000)/100.0f);
        PATCH_FYC(0x0105, pb->tsk->job[0]->bms.bcp.spn2818_total_energy/10.0f);
        PATCH_FYC(0x0106, pb->tsk->job[0]->bms.bcp.spn2819_max_charge_voltage/10.0f);
        PATCH_FYC(0x0107, (pb->tsk->job[0]->bms.bcp.spn2820_max_temprature-50)/1.0f);
        PATCH_FYC(0x0108, pb->tsk->job[0]->bms.bcp.spn2821_soc/10.0f);
        PATCH_FYC(0x0109, pb->tsk->job[0]->bms.bcp.spn2822_total_voltage/10.0f);

        // BCL
        PATCH_FYC(0x010A, pb->tsk->job[0]->need_V);
        PATCH_FYC(0x010B, pb->tsk->job[0]->need_I);
        PATCH_YC(0x010C, pb->tsk->job[0]->bms.bcl.spn3074_charge_mode);

        // BCS
        PATCH_FYC(0x010D, pb->tsk->job[0]->bms.bcs.spn3075_charge_voltage/10.0f);
        PATCH_FYC(0x010E, (pb->tsk->job[0]->bms.bcs.spn3076_charge_current-4000)/100.0f);
        PATCH_FYC(0x010F, pb->tsk->job[0]->bms.bcs.spn3078_soc/1.0);
        PATCH_FYC(0x0110, (float)pb->tsk->job[0]->bms.bcs.spn3079_need_time/1.0f);
        PATCH_YC(0x0111, pb->tsk->job[0]->bms.bcs.spn3077_max_v_g_number);

        // BSM
        PATCH_FYC(0x0112, pb->tsk->job[0]->bms.bsm.max_temperature_of_battery/1.0f);
        PATCH_FYC(0x0113, pb->tsk->job[0]->bms.bsm.sn_of_max_temperature_point/1.0f);
        PATCH_FYC(0x0114, pb->tsk->job[0]->bms.bsm.min_temperature_of_battery/1.0f);
        PATCH_FYC(0x0115, pb->tsk->job[0]->bms.bsm.sn_of_min_temperature_point/1.0f);
       // 无单体电池总数， 温度探头总数
        PATCH_FYC(0x0118, (float)(pb->tsk->job[0]->bms.bcs.spn3077_max_v_g_number&0x0FFF));
        PATCH_FYC(0x0119, pb->tsk->job[0]->bms.bsm.sn_of_max_voltage_battery/1.0f);

        buff[count_index] = point_count >> 8;
        buff[count_index+1] = point_count >> 0;

        pthread_mutex_lock(&pc->tx_msg_lck);
        ret = pandora_frame_push(&pc->tx_msg_head, 0x0E, n, buff);
        pthread_mutex_unlock (&pc->tx_msg_lck);
        log_printf(INF, "发送1#枪BMS遥测");
        ss->tx_tsp[TSP_1_BMS_YC] = tsp;
    } while (0);

    // 1#枪BMS电池电压遥测 T = 60S
    do {
        int point_count = 0;
        int count_index = 0;
        int n = 0, i = 0;
        unsigned int ei = 0;
        if ( tsp - ss->tx_tsp[TSP_1_BMS_V_YC] < 60 ) break;
        if ( ! pb->tsk->job[0] ) break;
        if ( ! bit_read(pb->tsk->job[0], JF_BMS_RX_BMV) ) break;

        buff[ n ++ ] = 0xEA;
        buff[ n ++ ] = 0xEB;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = ss->pack_seq ++;
        buff[ n ++ ] = 0x0E;
        count_index = n;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;

        //
        for (i=0; i < 256; i ++ ) {
            if ( pb->tsk->job[0]->bms.bmv.bat_v[i] == 0xFFFF ) break;
            PATCH_YC(0x0181 + i, pb->tsk->job[0]->bms.bmv.bat_v[i]);
        }

        buff[count_index] = point_count >> 8;
        buff[count_index+1] = point_count >> 0;

        pthread_mutex_lock(&pc->tx_msg_lck);
        ret = pandora_frame_push(&pc->tx_msg_head, 0x0E, n, buff);
        pthread_mutex_unlock (&pc->tx_msg_lck);
        log_printf(INF, "发送1#枪BMS电池电压遥测");
        ss->tx_tsp[TSP_1_BMS_YC] = tsp;
    } while (0);

    // ２#枪BMS遥测, T = 10S
    do {
        int point_count = 0;
        int count_index = 0;
        int n = 0, i = 0, v;
        unsigned int ei = 0;
        if ( tsp - ss->tx_tsp[TSP_2_BMS_YC] < 10 ) break;
        if ( ! pb->tsk->job[1] ) break;

        buff[ n ++ ] = 0xEA;
        buff[ n ++ ] = 0xEB;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = ss->pack_seq ++;
        buff[ n ++ ] = 0x0E;
        count_index = n;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;

        //
        // BRM
        v = pb->tsk->job[1]->bms.brm.spn2565_bms_version[0] << 16 |
                pb->tsk->job[1]->bms.brm.spn2565_bms_version[1] << 8 |
                pb->tsk->job[1]->bms.brm.spn2565_bms_version[2];
        PATCH_YC(0x011E, v);
        PATCH_YC(0x011F, pb->tsk->job[1]->bms.brm.spn2566_battery_type);
        PATCH_FYC(0x0120, pb->tsk->job[1]->bms.brm.spn2567_capacity/10.0f);
        PATCH_FYC(0x0121, pb->tsk->job[1]->bms.brm.spn2568_volatage/10.0f);
        PATCH_YC(0x0122, *(unsigned int*)pb->tsk->job[1]->bms.brm.spn2569_battery_manufacturer);
        PATCH_YC(0x0123, *(unsigned int*)pb->tsk->job[1]->bms.brm.spn2570_battery_sn);
        PATCH_YC(0x0124, pb->tsk->job[1]->bms.brm.spn2571_year);
        PATCH_YC(0x0125, pb->tsk->job[1]->bms.brm.spn2571_month);
        PATCH_YC(0x0126, pb->tsk->job[1]->bms.brm.spn2571_day);
        v = pb->tsk->job[1]->bms.brm.spn2572_charge_count[0] << 16 |
                pb->tsk->job[1]->bms.brm.spn2572_charge_count[1] << 8 |
                pb->tsk->job[1]->bms.brm.spn2572_charge_count[2];
        PATCH_YC(0x0127, v);
        PATCH_YC(0x0128, pb->tsk->job[1]->bms.brm.spn2573_battery_property);

        // BCP
        PATCH_FYC(0x012A, pb->tsk->job[1]->bms.bcp.spn2816_max_charge_volatage_single_battery/100.0f);
        PATCH_FYC(0x012B, (pb->tsk->job[1]->bms.bcp.spn2817_max_charge_current-4000)/10.0f);
        PATCH_FYC(0x012C, pb->tsk->job[1]->bms.bcp.spn2818_total_energy/10.0f);
        PATCH_FYC(0x012D, pb->tsk->job[1]->bms.bcp.spn2819_max_charge_voltage/10.0f);
        PATCH_FYC(0x012E, (pb->tsk->job[1]->bms.bcp.spn2820_max_temprature-50)/1.0f);
        PATCH_FYC(0x012F, pb->tsk->job[1]->bms.bcp.spn2821_soc/10.0f);
        PATCH_FYC(0x0130, pb->tsk->job[1]->bms.bcp.spn2822_total_voltage/10.0f);

        // BCL
        PATCH_FYC(0x0131, pb->tsk->job[1]->bms.bcl.spn3072_need_voltage/10.0f);
        PATCH_FYC(0x0132, (pb->tsk->job[1]->bms.bcl.spn3073_need_current-4000)/10.0f);
        PATCH_YC(0x0133, pb->tsk->job[1]->bms.bcl.spn3074_charge_mode);

        // BCS
        PATCH_FYC(0x0134, pb->tsk->job[1]->bms.bcs.spn3075_charge_voltage/10.0f);
        PATCH_FYC(0x0135, (pb->tsk->job[1]->bms.bcs.spn3076_charge_current-4000)/10.0f);
        PATCH_FYC(0x0136, pb->tsk->job[1]->bms.bcs.spn3078_soc/1.0f);
        PATCH_FYC(0x0137, pb->tsk->job[1]->bms.bcs.spn3079_need_time/1.0f);
        PATCH_FYC(0x0138, pb->tsk->job[1]->bms.bcs.spn3077_max_v_g_number);

        // BSM
        PATCH_FYC(0x0139, pb->tsk->job[1]->bms.bsm.max_temperature_of_battery/1.0f);
        PATCH_FYC(0x013A, pb->tsk->job[1]->bms.bsm.sn_of_max_temperature_point/1.0f);
        PATCH_FYC(0x013B, pb->tsk->job[1]->bms.bsm.min_temperature_of_battery/1.0f);
        PATCH_FYC(0x013C, pb->tsk->job[1]->bms.bsm.sn_of_min_temperature_point/1.0f);
       // 无单体电池总数， 温度探头总数
        PATCH_FYC(0x013F, pb->tsk->job[1]->bms.bcs.spn3077_max_v_g_number&0x0FFF);
        PATCH_FYC(0x0140, pb->tsk->job[1]->bms.bsm.sn_of_max_voltage_battery/1.0f);

        buff[count_index] = point_count >> 8;
        buff[count_index+1] = point_count >> 0;

        pthread_mutex_lock(&pc->tx_msg_lck);
        ret = pandora_frame_push(&pc->tx_msg_head, 0x0E, n, buff);
        pthread_mutex_unlock (&pc->tx_msg_lck);
        log_printf(INF, "发送２#枪BMS遥测");
        ss->tx_tsp[TSP_2_BMS_YC] = tsp;
    } while (0);

    // 2#枪BMS电池电压遥测, T = 60S
    do {
        int point_count = 0;
        int count_index = 0;
        int n = 0, i = 0;
        unsigned int ei = 0;
        if ( tsp - ss->tx_tsp[TSP_2_BMS_V_YC] < 60 ) break;
        if ( ! pb->tsk->job[1] ) break;
        if ( ! bit_read(pb->tsk->job[1], JF_BMS_RX_BMV) ) break;

        buff[ n ++ ] = 0xEA;
        buff[ n ++ ] = 0xEB;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = ss->pack_seq ++;
        buff[ n ++ ] = 0x0E;
        count_index = n;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;

        //
        //
        for (i=0; i < 256; i ++ ) {
            if ( pb->tsk->job[1]->bms.bmv.bat_v[i] == 0xFFFF ) break;
            PATCH_YC(0x0281 + i, pb->tsk->job[1]->bms.bmv.bat_v[i]);
        }

        buff[count_index] = point_count >> 8;
        buff[count_index+1] = point_count >> 0;

        pthread_mutex_lock(&pc->tx_msg_lck);
        ret = pandora_frame_push(&pc->tx_msg_head, 0x0E, n, buff);
        pthread_mutex_unlock (&pc->tx_msg_lck);
        log_printf(INF, "发送2#枪BMS电池电压遥测");
        ss->tx_tsp[TSP_2_BMS_V_YC] = tsp;
    } while (0);

    // 充电模块遥测, T = 15S
    do {
        int point_count = 0;
        int count_index = 0;
        int n = 0, i = 0;
        unsigned int ei = 0;
        if ( tsp - ss->tx_tsp[TSP_MODULES_YC] < 15 ) break;

        buff[ n ++ ] = 0xEA;
        buff[ n ++ ] = 0xEB;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = ss->pack_seq ++;
        buff[ n ++ ] = 0x0E;
        count_index = n;
        buff[ n ++ ] = 0;
        buff[ n ++ ] = 0;

        //
        for ( i = 0; i < CONFIG_SUPPORT_CHARGE_MODULE && i < task->modules_nr; i ++ ) {
            PATCH_FYC(0x145 + i, x10_high_word2lowfloat(task->chargers[0]->chargers.charge_module_v[i]));
            PATCH_FYC(0x159 + i, x10_high_word2lowfloat(task->chargers[0]->chargers.charge_module_i[i]));
            PATCH_FYC(0x16D + i, x10_high_word2lowfloat(task->chargers[0]->chargers.charge_module_t[i]));
        }

        buff[count_index] = point_count >> 8;
        buff[count_index+1] = point_count >> 0;

        pthread_mutex_lock(&pc->tx_msg_lck);
        ret = pandora_frame_push(&pc->tx_msg_head, 0x0E, n, buff);
        pthread_mutex_unlock (&pc->tx_msg_lck);
        log_printf(INF, "发送充电模块遥测");
        ss->tx_tsp[TSP_MODULES_YC] = tsp;
    } while (0);

    return 0;
}

// 开始充电
int start_charge(struct pandoras_box *pb, struct pandora_client *pc, struct socket_session *ss, struct pandoras_msg* msg)
{
    char cid[32] = {0};
    char c_mode[16] = {0};
    char set_V[8] = {0};
    char set_I[8] = {0};
    char b_mode[16] = {0};
    char b_kwh[8] = {0};
    char b_money[8] = {0};
    char b_time[8] = {0};
    char c_remain[16] = {0};
    char c_passwd[16] = {0};
    char c_assit[16] = {0};
    char cmd[512];
    char *body = msg->msg_body;
#pragma pack(1)
        struct charge_pack {
            unsigned char gun;
            unsigned char billing_sn[32];
            unsigned char accounttype;
            unsigned char account[32];
            unsigned char passwd[16];
            float pre_hold_amount;
            size_t  time;
            size_t remain;
            unsigned char workmode;
            size_t lV;
            size_t lI;
            unsigned char chargemode;
            unsigned short param;
        }*p;
#pragma pack()
    p = (struct charge_pack*)(&msg->msg_body[6]);

    strcpy(cid, "1234565");
    strcpy(c_mode, "BV");
    strcpy(set_V, "559.9");
    strcpy(set_I, "40.3");
    strcpy(b_mode, "auto");
    strcpy(b_kwh, "1234565");
    strcpy(b_money, "1234565");
    strcpy(b_time, "1234565");
    strcpy(c_remain, "99.8");
    strcpy(c_passwd, "123456");
    strcpy(c_assit, "auto");

    if ( 0 == p->account[0] ) {
        strncpy(p->account, "ONADMIN", 16);
    }

    sprintf(cmd,
            "curl 'http://127.0.0.1:8081/job/create.json?t=%ld&"
            "cid=%s&gun=%d&remain=%s&passwd=%s&assit=%s&",
            time(NULL),
            p->account, p->gun, c_remain, c_passwd, c_assit
            );
    if ( p->workmode == 0x10 ) {
        char buff[128];
        sprintf(buff, "c_mode=BV&set_V=%.1f&set_I=%.1f&", p->lV*1.0f, p->lI*1.0f);
        strcat(cmd, buff);
    } else if ( p->workmode == 0x20 ) {
        char buff[128];
        sprintf(buff, "c_mode=BI&set_V=%.1f&set_I=%.1f&", p->lV*1.0f, p->lI*1.0f);
        strcat(cmd, buff);
    } else {
        strcat(cmd, "c_mode=auto&");
    }

    if ( p->chargemode == 0x20 ) {
        char buff[128];
        sprintf(buff, "b_mode=kwh&b_kwh=%.1f'", p->param*1.0f);
        strcat(cmd, buff);
    } if ( p->chargemode == 0x30 ) {
        char buff[128];
        sprintf(buff, "b_mode=time&b_time=%.1f'", p->param*1.0f);
        strcat(cmd, buff);
    } else if ( p->chargemode == 0x40 ) {
        char buff[128];
        sprintf(buff, "b_mode=money&b_money=%.1f'", p->param*1.0f);
        strcat(cmd, buff);
    } else {
        strcat(cmd, "b_mode=auto'");
    }

    printf("%s", cmd);

    system(cmd);
    log_printf(INF, "job create %s", pb->tsk->job[ p->gun ] ? "未完成":"完成");

    do {
        char buff[32];
        int n = 0;

        buff[ n ++ ] = 0xEA;
        buff[ n ++ ] = 0xEB;
        buff[ n ++ ] = 0x00;
        buff[ n ++ ] = 0x01;
        buff[ n ++ ] = ss->pack_seq ++;
        buff[ n ++ ] = 0x1A;
        buff[ n ++ ] = 0x00;

        pthread_mutex_lock(&pc->tx_msg_lck);
        pandora_frame_push(&pc->tx_msg_head, 0x1A, n, buff);
        pthread_mutex_unlock (&pc->tx_msg_lck);
    } while (0);

    if ( pb->tsk->job[ p->gun ] ) {
        struct charge_job *job = pb->tsk->job[ p->gun ];

        memcpy(job->charge_billing.billing_sn, p->billing_sn, 32);
        job->charge_billing.account_type = p->accounttype;
        memcpy(job->charge_billing.account, p->account, 32);
        memcpy(job->charge_billing.passwd, p->param, 16);

    }

    return 0;
}

// 停止充电
int stop_charge(struct pandoras_box *pb, struct pandora_client *pc, struct socket_session *ss, struct pandoras_msg* msg)
{
    do {
        char buff[32];
        int n = 0;
        unsigned char gun = ((unsigned char *)msg->msg_body)[6];


        buff[ n ++ ] = 0xEA;
        buff[ n ++ ] = 0xEB;
        buff[ n ++ ] = 0x00;
        buff[ n ++ ] = 0x01;
        buff[ n ++ ] = ss->pack_seq ++;
        buff[ n ++ ] = 0x1D;
        if ( pb->tsk->job[gun] ) {
            buff[ n ++ ] = 0x00;
            bit_set(pb->tsk->job[gun], JF_CMD_ABORT);
            log_printf(INF, "后台停止充电");
        } else {
            buff[ n ++ ] = 0xFF;
            log_printf(INF, "后台控制错误，无效的充电作业");
        }

        pthread_mutex_lock(&pc->tx_msg_lck);
        pandora_frame_push(&pc->tx_msg_head, 0x1D, n, buff);
        pthread_mutex_unlock (&pc->tx_msg_lck);
    } while (0);
    return 0;
}

// 收帧
static int process_frame_rx(struct pandoras_box *pb, struct pandora_client *pc, struct socket_session *ss)
{
    time_t tsp = time(NULL);
    struct pandoras_msg* msg = NULL;
    struct list_head *l = NULL, *n = NULL;
    int needrelease = 0, deal = 0;
    unsigned char *body;

    // 处理接受数据包
    if ( pc->rx_msg_head ) {
        pthread_mutex_lock(&pc->rx_msg_lck);
        do {
            msg = pandora_frame_pop(&pc->rx_msg_head);
            deal ++;
            switch (msg->id) {
            case 0x11: // 登录
                if ( ((char *)msg->msg_body)[6] == 0x00 ) {
                    bit_set(pc, CF_LOGED_IN);
                    bit_set(pc, CF_TX_LOGIN);
                    bit_clr(pc, CS_LOGIN_TIMEOUT);
                    log_printf(INF, "INF: 登录成功.");
                    bit_clr(pb->tsk, S_BACKCOMM_DOWN);
                } else {
                    bit_clr(pc, CF_TX_LOGIN);
                    log_printf(WRN, "登录失败， 错误码: %d", ((char *)msg->msg_body)[6]);
                }
                do_frame_recycle(pb, pc, ss, 0x01);
                break;
            case 0x12: // 心跳
                if ( ((char *)msg->msg_body)[6] == 0x00 ) {
                    bit_clr(pc, CS_HEARTBEAT_TIMEOUT);
                    log_printf(INF, "BOX: 心跳回应.");
                    bit_clr(pb->tsk, S_BACKCOMM_DOWN);
                } else {
                    log_printf(WRN, "心跳回应失败，故障码: %d", ((char *)msg->msg_body)[6]);
                }
                do_frame_recycle(pb, pc, ss, 0x02);
                break;
            case 0x03: // 时间同步
                log_printf(INF, "请求时间同步");
                do {
                    time_t t;
                    body = (char *)msg->msg_body;
                    char buff[32];
                    int n = 0;

                    t = body[6] | (body[7] << 8) | (body[8] << 16) | (body[9] << 24);

                    buff[ n ++ ] = 0xEA;
                    buff[ n ++ ] = 0xEB;
                    buff[ n ++ ] = 0x00;
                    buff[ n ++ ] = 0x01;
                    buff[ n ++ ] = ss->pack_seq ++;
                    buff[ n ++ ] = 0x13;
                    buff[ n ++ ] = 0x00;

                    pthread_mutex_lock(&pc->tx_msg_lck);
                    pandora_frame_push(&pc->tx_msg_head, 0x13, n, buff);
                    pthread_mutex_unlock (&pc->tx_msg_lck);
                } while (0);
                break;
            case 0x14: // 设备状态应答
                log_printf(INF, "设备状态应答");
                do_frame_recycle(pb, pc, ss, 0x04);
                break;
            case 0x15: // 实时充电状态应答
                log_printf(INF, "实时充电状态应答");
                do_frame_recycle(pb, pc, ss, 0x05);
                break;
            case 0x16: // 账户查询应答
                log_printf(INF, "账户查询应答");
                body = (char *)msg->msg_body;
                do {
                    unsigned char status = body[6];
                    unsigned int remain;
                    unsigned int remain_kwh;

                    remain = (body[7] << 24) | (body[8] << 16) | (body[9] << 8) | body[10];
                    remain_kwh = (body[11] << 24) | (body[12] << 16) | (body[13] << 8) | body[14];

                    switch ( status ) {
                    case 0x00: // 成功
                        pb->tsk->auth.auth_return_value = status;
                        pb->tsk->auth.auth_msg = "认证成功";
                        pb->tsk->auth.auth_allow_charge = 1;
                        pb->tsk->auth.auth_return_remain = remain;
                        pb->tsk->auth.auth_return_kwh = remain_kwh;
                    break;
                    case 0x01: // 密码错误
                        pb->tsk->auth.auth_return_value = status;
                        pb->tsk->auth.auth_msg = "密码错误!";
                        pb->tsk->auth.auth_allow_charge = 0;
                        pb->tsk->auth.auth_return_remain = 0;
                        pb->tsk->auth.auth_return_kwh = 0;
                    break;
                    case 0x02: // 无效卡
                        pb->tsk->auth.auth_return_value = status;
                        pb->tsk->auth.auth_msg = "无效卡!";
                        pb->tsk->auth.auth_allow_charge = 0;
                        pb->tsk->auth.auth_return_remain = 0;
                        pb->tsk->auth.auth_return_kwh = 0;
                    break;
                    case 0x03: // 卡锁定
                        pb->tsk->auth.auth_return_value = status;
                        pb->tsk->auth.auth_msg = "卡被锁定!";
                        pb->tsk->auth.auth_allow_charge = 0;
                        pb->tsk->auth.auth_return_remain = 0;
                        pb->tsk->auth.auth_return_kwh = 0;
                    break;
                    case 0x04: // 余额不足
                        pb->tsk->auth.auth_return_value = status;
                        pb->tsk->auth.auth_msg = "余额不足!";
                        pb->tsk->auth.auth_allow_charge = 0;
                        pb->tsk->auth.auth_return_remain = 0;
                        pb->tsk->auth.auth_return_kwh = 0;
                    break;
                    case 0x05: // 无效车辆
                        pb->tsk->auth.auth_return_value = status;
                        pb->tsk->auth.auth_msg = "无效车辆!";
                        pb->tsk->auth.auth_allow_charge = 0;
                        pb->tsk->auth.auth_return_remain = 0;
                        pb->tsk->auth.auth_return_kwh = 0;
                    break;
                    case 0x07: // BOSS响应超时
                        pb->tsk->auth.auth_return_value = status;
                        pb->tsk->auth.auth_msg = "BOSS响应超时!";
                        pb->tsk->auth.auth_allow_charge = 0;
                        pb->tsk->auth.auth_return_remain = 0;
                        pb->tsk->auth.auth_return_kwh = 0;
                    break;
                    case 0x0a: // 服务器响应超时
                        pb->tsk->auth.auth_return_value = status;
                        pb->tsk->auth.auth_msg = "服务器响应超时!";
                        pb->tsk->auth.auth_allow_charge = 0;
                        pb->tsk->auth.auth_return_remain = 0;
                        pb->tsk->auth.auth_return_kwh = 0;
                    break;
                    case 0x0b: // 网关离线
                        pb->tsk->auth.auth_return_value = status;
                        pb->tsk->auth.auth_msg = "网关离线!";
                        pb->tsk->auth.auth_allow_charge = 0;
                        pb->tsk->auth.auth_return_remain = 0;
                        pb->tsk->auth.auth_return_kwh = 0;
                    break;
                    case 0xFF: // 未知错误
                        pb->tsk->auth.auth_return_value = status;
                        pb->tsk->auth.auth_msg = "未知错误!";
                        pb->tsk->auth.auth_allow_charge = 0;
                        pb->tsk->auth.auth_return_remain = 0;
                        pb->tsk->auth.auth_return_kwh = 0;
                    break;
                    default:
                        pb->tsk->auth.auth_return_value = status;
                        pb->tsk->auth.auth_msg = "认证异常!";
                        pb->tsk->auth.auth_allow_charge = 0;
                        pb->tsk->auth.auth_return_remain = 0;
                        pb->tsk->auth.auth_return_kwh = 0;
                    break;
                    }
                } while(0);

                pb->tsk->auth.auth_tx_tsp = 0;
                pb->tsk->auth.auth_return_tsp = time(NULL);
                do_frame_recycle(pb, pc, ss, 0x06);
                break;
            case 0x17: // 请求服务应答
                log_printf(INF, "请求充电应答");
                do {
                    char *body = msg->msg_body;
                    pb->tsk->request.result = body[6]; // BOSS 超时
                    pb->tsk->request.action = body[7];
                    pb->tsk->request.pre_pay = (body[8]<<24)|(body[9]<<16)|(body[10]<<8)|body[11];
                    pb->tsk->request.remian = (body[12]<<24)|(body[13]<<16)|(body[14]<<8)|body[15];
                    pb->tsk->request.allow_time = (body[16]<<24)|(body[17]<<16)|(body[18]<<8)|body[19];
                    pb->tsk->request.allow_kwh = (body[20]<<8)|body[21];
                    memcpy(pb->tsk->request.billing_sn, body[22], 32);
                    pb->tsk->request.tx_request_tsp = 0;
                    pb->tsk->request.request_tsp = 0;
                    bit_clr(pc, CS_REQUEST_CHARGE_TIMEOUT);
                } while (0);
                pb->tsk->request.ack_tsp = time(NULL);
                do_frame_recycle(pb, pc, ss, 0x07);
                break;
            case 0x18: // 计费应答
                log_printf(INF, "后台计费应答");
                do {
                    char *b = msg->msg_body;
                    struct charge_job *job = NULL;
                    if ( pb->tsk->job[0] ) {
                        job = pb->tsk->job[0];
                    } else if ( pb->tsk->job[1] ) {
                        job = pb->tsk->job[1];
                    }

                    if ( job ) {
                        bit_set(job, JF_BILLING_DONE);
                    }
                } while (0);
                do_frame_recycle(pb, pc, ss, 0x08);
                break;
            case 0x09: // 预约服务
                log_printf(INF, "后台预约充电");
                break;
            case 0x0A: // 充电控制
                log_printf(INF, "后台充电控制");
                body = (char *)msg->msg_body;
                if ( body[6] >= 0 && body[6] <= 1 ) {
                    if ( pb->tsk->job[ body[6] ] ) {
                        char buff[32];
                        int n = 0;
                        buff[ n ++ ] = 0xEA;
                        buff[ n ++ ] = 0xEB;
                        buff[ n ++ ] = 0x00;
                        buff[ n ++ ] = 0x01;
                        buff[ n ++ ] = ss->pack_seq ++;
                        buff[ n ++ ] = 0x1A;
                        buff[ n ++ ] = 0xFF;
                        pthread_mutex_lock(&pc->tx_msg_lck);
                        pandora_frame_push(&pc->tx_msg_head, 0x1A, n, buff);
                        pthread_mutex_unlock (&pc->tx_msg_lck);
                        log_printf(WRN, "前一个任务还未终止");
                    } else {
                        start_charge(pb, pc, ss, msg);
                        log_printf(WRN, "开始充电");
                    }
                } else {
                    char buff[32];
                    int n = 0;
                    buff[ n ++ ] = 0xEA;
                    buff[ n ++ ] = 0xEB;
                    buff[ n ++ ] = 0x00;
                    buff[ n ++ ] = 0x01;
                    buff[ n ++ ] = ss->pack_seq ++;
                    buff[ n ++ ] = 0x1A;
                    buff[ n ++ ] = 0xFF;
                    pthread_mutex_lock(&pc->tx_msg_lck);
                    pandora_frame_push(&pc->tx_msg_head, 0x1A, n, buff);
                    pthread_mutex_unlock (&pc->tx_msg_lck);

                    log_printf(WRN, "无效的充电枪(1,2,3,4)");
                }
                break;
            case 0x0B: // 下发计费参数
                log_printf(INF, "后台下发计费参数");
                break;
            case 0x0C: // 查询状态
                log_printf(INF, "后台下发查询状态");
                do {
                    char buff[32];
                    int n = 0;
                    buff[ n ++ ] = 0xEA;
                    buff[ n ++ ] = 0xEB;
                    buff[ n ++ ] = 0x00;
                    buff[ n ++ ] = 0x01;
                    buff[ n ++ ] = ss->pack_seq ++;
                    buff[ n ++ ] = 0x1C;
                    buff[ n ++ ] = 0x00;
                    pthread_mutex_lock(&pc->tx_msg_lck);
                    pandora_frame_push(&pc->tx_msg_head, 0x1C, n, buff);
                    pthread_mutex_unlock (&pc->tx_msg_lck);

                    log_printf(WRN, "查询信息(1,2,3,4)");
                } while (0);
                break;
            case 0x0D: // 停止充电
                log_printf(INF, "后台下发停止充电");
                stop_charge(pb, pc, ss, msg);
                break;
            case 0x1E: // 实时状态应答
                do_frame_recycle(pb, pc, ss, 0x0E);
                break;
            default:
                break;
            }
            memset(msg, 0, sizeof(struct pandoras_msg));
            free(msg);
        } while ( pc->rx_msg_head && deal < 16 );

        pthread_mutex_unlock (&pc->rx_msg_lck);
    }

    // 处理超时
    if ( !pc->msg_pedding ) {
        return 0;
    } else {
        pthread_mutex_lock(&pc->pedding_msg_lck);

        l = pc->msg_pedding;
        do {
            msg = list_load(struct pandoras_msg, nd, l);

            n = l->next;
            needrelease = 0;
            // 5 秒超时
            if ( tsp - msg->tsp >= 5 ) {
                switch (msg->id) {
                case 0x01: // 登录
                    if ( ! bit_read(pc, CS_LOGIN_TIMEOUT) ) {
                        log_printf(WRN, "BOX: 登录超时，重试.");
                    }
                    bit_set(pc, CS_LOGIN_TIMEOUT);
                    bit_clr(pc, CF_TX_LOGIN);
                    bit_set(pb->tsk, S_BACKCOMM_DOWN);
                    break;
                case 0x02: // 心跳
                    bit_clr(pc, CF_TX_LOGIN);
                    bit_set(pc, CS_HEARTBEAT_TIMEOUT);
                    bit_set(pb->tsk, S_BACKCOMM_DOWN);
                    log_printf(WRN, "BOX: 心跳回应超时.");
                    break;
                case 0x04: // 设备状态应答
                    log_printf(WRN, "设备状态上报回应超时!");
                    break;
                case 0x05: // 实时充电状态应答
                    log_printf(WRN, "实时充电数据回应超时!");
                    break;
                case 0x06: // 账户查询应答
                    if ( pb->tsk->auth.auth_request_tsp ) {
                        log_printf(WRN, "账户查询回应超时!");
                        pb->tsk->auth.auth_return_value = 0xff;
                        pb->tsk->auth.auth_msg = "认证超时!";
                        pb->tsk->auth.auth_allow_charge = 0;
                        pb->tsk->auth.auth_return_remain = 0;
                        pb->tsk->auth.auth_return_kwh = 0;
                        pb->tsk->auth.auth_return_tsp = time(NULL);
                    }
                    pb->tsk->auth.auth_tx_tsp = 0;
                    break;
                case 0x07: // 请求服务应答
                    if ( pb->tsk->request.request_tsp ) {
                        pb->tsk->request.result = 0x07; // BOSS 超时
                        pb->tsk->request.action = 0;
                        pb->tsk->request.pre_pay = 0.0f;
                        pb->tsk->request.remian = 0.0f;
                        pb->tsk->request.allow_time = 0;
                        pb->tsk->request.allow_kwh = 0;
                        memset(pb->tsk->request.billing_sn, 0,
                               sizeof(pb->tsk->request.billing_sn));
                        pb->tsk->request.tx_request_tsp = 0;
                        pb->tsk->request.ack_tsp = 0;
                        pb->tsk->request.request_tsp = 0;
                    } else {
                        log_printf(WRN, "无效报文 0x07!");
                    }
                    bit_set(pc, CS_REQUEST_CHARGE_TIMEOUT);
                    log_printf(WRN, "请求充电服务应答超时!");
                    break;
                case 0x08: // 计费应答
                    bit_set(pc, CS_BILLING_TIMEOUT);
                    log_printf(WRN, "请求计费应答超时");
                    do {
                        char *b = msg->msg_body;
                        struct charge_job *job = NULL;
                        if ( pb->tsk->job[0] ) {
                            job = pb->tsk->job[0];
                        } else if ( pb->tsk->job[1] ) {
                            job = pb->tsk->job[1];
                        }

                        if ( job ) {
                            bit_set(job, JS_BILLING_TIMEOUT);
                        }
                    } while (0);
                    break;
                case 0x0E: // 实时上报应答
                    log_printf(WRN, "实时状态应答超时");
                    break;
                default:
                    break;
                }
                needrelease ++;
            }

            if ( needrelease ) {
                if ( l == pc->msg_pedding ) {
                    if ( l == n ) {
                        pc->msg_pedding = NULL;
                    } else {
                        pc->msg_pedding = n;
                    }
                    list_remove(l);
                    free(msg);
                } else {
                    list_remove(l);
                    free(msg);
                }
                l = n;
            } else {
                l = l->next;
            }

        } while ( pc->msg_pedding && l != pc->msg_pedding );

        pthread_mutex_unlock (&pc->pedding_msg_lck);
    }

    return 0;
}




















































