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
#include <stdint.h>
#include <execinfo.h>

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

struct pandoras_box pb;

int pandora_init(void *p)
{
    int ret;
    pack_t pid;
    pthread_t sniffer_thread, protocol_thread;
    struct charge_task *tsk = (struct charge_task *)p;

    pb.zeus_port = 8082;
    pb.tsk = (struct charge_task *)p;
    log_printf(INF, "BOX.INIT: 初始化...");
    pthread_mutex_init(&pb.msg_tx_lck, NULL);
    pthread_mutex_init(&pb.msg_rx_lck, NULL);
    pthread_mutex_init(&pb.clients_lck, NULL);
    pthread_mutex_init(&pb.sh_blocks_lck, NULL);

    log_printf(INF, "BOX.INIT: 初始化共享字段空间...");
    ret = shared_block_touch(&pb.sh_blocks, 19);
    if ( ret ) {
        goto faile;
    }
    ret = shared_block_fill(pb.sh_blocks[0], (pack_t)PID_WORKING_JOB_01, "进程1", 1251 + 5);
    if ( ret ) {
        goto faile;
    }
    ret = shared_block_fill(pb.sh_blocks[1], (pack_t)PID_WORKING_JOB_02, "进程2", 1251 + 5);
    if ( ret ) {
        goto faile;
    }

    for ( pid = PID_PEDDING_JOB_01; pid <= (pack_t)PID_PEDDING_JOB_16; pid ++ ) {
        char buff[32];

        sprintf(buff, "程序%d", pid & 0xFF);
        ret = shared_block_fill(pb.sh_blocks[(pid & 0xFF)-1], pid, buff, 61 + 3);
        if ( ret ) goto faile;
    }
    ret = shared_block_fill(pb.sh_blocks[18], (pack_t)PID_CHARGER_INFO, "充电桩", 612 + 0);
    if ( ret ) goto faile;

    ret = pthread_create( & protocol_thread, &tsk->attr, protocol_client_proc, &pb);
    if ( 0 != ret ) {
        ret  = 0x1006;
        log_printf(ERR,
                   "创建协议处理现成失败.");
        goto faile;
    }

#ifdef _USE_P_BJNR_PROTOCOL
    do {
        struct pandora_client pc;
        struct socket_session *ss;
        int ret;

        memset(&pc, 0, sizeof(pc));

        strncpy(pc.protocol_name, "北京南瑞协议", 31);
        list_ini(pc.nd);
        pc.bpc.via_way = PI_VIA_SOCKETS;
        pthread_mutex_init(&pc.rx_msg_lck, NULL);
        pthread_mutex_init(&pc.tx_msg_lck, NULL);
        pthread_mutex_init(&pc.pedding_msg_lck, NULL);

        ss = (struct socket_session*)malloc(sizeof(struct socket_session));
        if ( ! ss ) {
            log_printf(WRN, "BOX.RESOURCE.ALLOC: 资源分配失败!");
            goto faile;
        }

        memset(ss, 0, sizeof(struct socket_session));

        ss->established_tsp = 0;
        ss->last_conn_tsp = 0;
        ss->s_srv = -1;
        strncpy(ss->s_srv_ip, tsk->server_addr, 32);
        ss->s_srv_port = tsk->server_port;

        pc.op.do_initialize = BJNR_initialize;
        pc.op.do_working = BJNR_working;
        pc.op.do_pause = BJNR_pause;
        pc.op.do_exit = BJNR_exit;
        pc.op.do_destroy = BJNR_destroy;

        log_printf(INF, "BOX.CLIENT: 注册客户端...");
        ret = pandora_register_client(&pb, &pc, ss);
        if ( ret ) {
            log_printf(WRN, "BOX.PROTOCOL.REG: 注册客户端失败!");
        } else {
            log_printf(INF, "BOX.PROTOCOL.REG: 客户端注册成功!");
        }
    } while (0);
#endif
    return 0;
faile:
    log_printf(ERR, "BOX: 异常退出!");
    return 1;
}

int pandora_register_client(struct pandoras_box *pb, struct pandora_client *pc, void *_private)
{
    struct pandora_client *new_pc;
    if ( pb == NULL || pc == NULL ) return 1;

    new_pc = (struct pandora_client *)malloc(sizeof(struct pandora_client));
    if ( new_pc == NULL ) return 1;
    memset(new_pc, 0, sizeof(struct pandora_client));

    memcpy(new_pc, pc, sizeof(struct pandora_client));
    list_ini(new_pc->nd);
    new_pc->_private = _private;

    pthread_mutex_lock(&pb->clients_lck);

    if ( pb->clients ) {
        list_inserttail(pb->clients, &new_pc->nd);
    } else {
        pb->clients = & new_pc->nd;
    }

    pthread_mutex_unlock(&pb->clients_lck);

    return 0;
}

int shared_block_touch(struct shared_blocks ***sb, size_t shared_blocks_nr)
{
    size_t ms = (
                  sizeof(struct shared_blocks **) +
                  sizeof(struct shared_blocks)
                ) * shared_blocks_nr;

    if ( sb == NULL ) return 1;

    size_t i;
    char * mm = (char  *)malloc(ms);
    if ( ! mm ) return 1;

    memset(mm, 0, ms);

    *sb = (struct shared_blocks **)(void *)mm;

    mm = mm + sizeof(struct shared_blocks **) * shared_blocks_nr;
    for ( i = 0; i < shared_blocks_nr; i ++ ) {
        (*sb)[ i ] = (struct shared_blocks *)(void *)mm;
        mm = mm + sizeof(struct shared_blocks);
        //printf("\t %-8d    0x%08x\n", i + 1, (*sb)[ i ]);
    }

    return 0;
}

int shared_block_fill(struct shared_blocks *sb, pack_t pid, const char *pname, size_t bs)
{
    if ( sb == NULL ) return 1;


    sb->pid = pid;
    strncpy(sb->shared_name, pname, 31);

    sb->shared_buff_size = bs;
    sb->shared_buff_base_addr = (char *)malloc(bs);
    if ( sb->shared_buff_base_addr == NULL ) return 1;

    //printf("\t%-16s %08X  %-8d  0x%08x  0x%08x\n",
    //       pname, pid, bs, sb, sb->shared_buff_base_addr);

    return 0;
}

struct shared_blocks* shared_block_name_search(struct shared_blocks **sb,
                                               size_t pnr, const char *pname)
{
    struct shared_blocks *thiz = NULL ;
    size_t i;

    if ( sb == NULL ) return NULL;

    for ( i = 0; i < pnr; i ++ ) {
        thiz = sb[ i ];
        if ( thiz == NULL ) continue;

        if ( strcmp(pname, thiz->shared_name) ) break;
        thiz = NULL;
    }

    return thiz;
}

struct shared_blocks* shared_block_pid_search(struct shared_blocks **sb,
                                              size_t pnr, pack_t pid)
{
    struct shared_blocks *thiz = NULL ;
    size_t i;

    if ( sb == NULL ) return NULL;

    for ( i = 0; i < pnr; i ++ ) {
        thiz = sb[ i ];
        if ( thiz == NULL ) continue;

        if ( pid == thiz->pid ) break;
        thiz = NULL;
    }

    return thiz;
}

int pack_section_touch(struct pack_section ***ps, size_t section_nr)
{
    size_t ms = (
                  sizeof(struct pack_section **) +
                  sizeof(struct pack_section)
                ) * section_nr;

    if ( ps == NULL ) return 1;

    size_t i;
    char * mm = (char  *)malloc(ms);
    if ( ! mm ) return 1;

    memset(mm, 0, ms);

    *ps = (struct pack_section **)(void *)mm;

    mm = mm + sizeof(struct pack_section **);
    for ( i = 0; i < section_nr; i ++ ) {
        (*ps)[ i ] = (struct pack_section *)(void *)mm;
        mm = mm + sizeof(struct pack_section);
    }

    return 0;
}

int pack_section_fill(struct pack_section *ps, unit_t uid, const char *uname,
                      void *base, size_t offset,
                      size_t usize, const char *type, const char *format)
{
    if ( ps == NULL ) return 1;

    ps->uid = uid;
    strncpy(ps->u_name, uname, 15);
    ps->u_base_addr = base;
    ps->u_offset = offset;
    ps->u_size = usize;
    strncpy(ps->u_type, type, 15);
    strncpy(ps->u_format, format, 15);

    ps->u_user = ZEUS_HANDS;
    ps->u_pre_updated = time(NULL);
    ps->u_last_updated = ps->u_pre_updated;

    return 0;
}

struct pack_section* pack_section_name_search(struct pack_section **ps,
                                              size_t unr, const char *uname)
{
    struct pack_section *thiz = NULL ;
    size_t i;

    if ( ps == NULL ) return NULL;

    for ( i = 0; i < unr; i ++ ) {
        thiz = ps[ i ];
        if ( thiz == NULL ) continue;

        if ( strcmp(uname, thiz->u_name) ) break;
        thiz = NULL;
    }

    return thiz;
}

struct pack_section* pack_section_uid_search(struct pack_section **ps,
                                             size_t unr, unit_t uid)
{
    struct pack_section *thiz = NULL;
    size_t i;

    if ( ps == NULL ) return NULL;

    for ( i = 0; i < unr; i ++ ) {
        thiz = ps[ i ];
        if ( thiz == NULL ) continue;

        if ( uid == thiz->uid ) break;
        thiz = NULL;
    }

    return thiz;
}

// 将消息信息放进列表中， 需要在操作前获得互斥对象
int pandora_frame_push(struct list_head **l, msg_t id, size_t len, const void *msg)
{
    struct pandoras_msg *thiz;
    size_t s;
    if ( l == NULL ) return 1;

    s = len + sizeof(struct pandoras_msg);

    thiz = (struct pandoras_msg *)malloc(s);
    memset(thiz, 0, s);

    thiz->tsp = time(NULL);
    thiz->id = id;
    thiz->msg_size = len;
    thiz->msg_body = (void *)(((char *)thiz)+sizeof(struct pandoras_msg));
    list_ini(thiz->nd);
    memcpy(thiz->msg_body, msg, len);

    if ( *l == NULL ) {
        *l =  & thiz->nd;
    } else {
        list_inserttail( *l, &thiz->nd );
    }

    return 0;
}

// 从消息队列中弹出一个消息帧
struct pandoras_msg* pandora_frame_pop(struct list_head **l)
{
    struct pandoras_msg *thiz;
    volatile struct list_head *tn, *obj;

    if ( l == NULL ) return NULL;
    if ( *l == NULL ) return NULL;

    tn = (*l)->next;
    obj = *l;

    list_remove( *l );

    if ( tn == *l ) {
        *l = NULL;
    } else {
        *l = tn;
    }

    thiz = list_load(struct pandoras_msg, nd, obj);

    return thiz;
}

// 将现有的消息加入到另外一个队列中
int pandora_frame_join(struct list_head **l, struct list_head *n)
{
    if ( l == NULL ) return 1;
    if ( n == NULL ) return 1;
    if ( *l == NULL ) {
        *l = n;
    } else {
        list_inserttail(*l, n);
    }
    return 0;
}

