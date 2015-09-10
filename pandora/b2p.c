/*
 * 对外协议处理线程
 */
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

void * protocol_client_proc(void *p)
{
    struct pandoras_box *pb = (struct pandoras_box*)p;
    struct pandora_client *thiz_client;
    struct list_head *thiz_node;

    if ( p == NULL ) return NULL;
    while ( 1 ) {
#if 1
        pthread_mutex_lock(&pb->clients_lck);
        thiz_node = pb->clients;
        pthread_mutex_unlock(&pb->clients_lck);

        do {
            thiz_client = NULL;
            if ( thiz_node ) {
                thiz_client = list_load(struct pandora_client, nd, thiz_node);

                pthread_mutex_lock(&pb->clients_lck);
                thiz_node = thiz_node->next;
                pthread_mutex_unlock(&pb->clients_lck);
            }

            if ( thiz_client ) {
                protocol_server_step(pb, thiz_client);
            }
            usleep(5000);
        } while (thiz_client && thiz_node && thiz_node != pb->clients);
#endif
        usleep(5000);
    }

    return NULL;
}

void protocol_server_step(struct pandoras_box *pb, struct pandora_client *pc)
{
    if ( pb == NULL || pc == NULL ) return;

    switch ( pc->stage ) {
    case STAGE_INIT:
        bit_set(pb->tsk, S_BACKCOMM_DOWN);
        if ( pc->op.do_initialize ) {
            int ret = pc->op.do_initialize(pb, pc);
            if ( !ret ) {
                pc->stage = STAGE_WORKING;
            }
        } else {
            pc->stage = STAGE_WORKING;
        }
        break;
    case STAGE_WORKING:
        if ( pc->op.do_working ) {
            pc->op.do_working(pb, pc);
        } else {
            pc->stage = STAGE_EXIT;
        }
        break;
    case STAGE_PAUSE:
        if ( pc->op.do_pause ) {
            pc->op.do_pause(pb, pc);
        } else {
            pc->stage = STAGE_EXIT;
        }
        break;
    case STAGE_EXIT:
        if ( pc->op.do_exit ) {
            pc->op.do_exit(pb, pc);
        } else {
            pc->stage = STAGE_PURGE;
        }
        break;
    case STAGE_PURGE:
        if ( pc->op.do_destroy ) {
            pc->op.do_destroy(pb, pc);
        } else {
            // do cleanup manual.
        }
        break;
    default:
        break;
    }
}

// 回收数据包
int do_frame_recycle(struct pandoras_box *pb, struct pandora_client *pc, struct socket_session *ss, msg_t id)
{
    struct pandoras_msg* msg = NULL;
    struct list_head *l = NULL, *n = NULL;

    if ( ! pc->msg_pedding ) return 0;
    pthread_mutex_lock(&pc->pedding_msg_lck);
    l = pc->msg_pedding;
    do {
        msg = list_load(struct pandoras_msg, nd, l);

        n = l->next;
        if ( id != msg->id ) {
            l = l->next;
        } else {
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
        }
    } while ( pc->msg_pedding && l != pc->msg_pedding );

    pthread_mutex_unlock (&pc->pedding_msg_lck);
    return 0;
}




















