//
//  cantp.c
//  ontom
//
//  Created by 李杰 on 15/7/8.
//
//

#include "cantp.h"

static inline struct frame_node *__search_tp_list(struct list_head **fl)
{
    do {
        struct frame_node *thiz_tp = NULL;
        struct list_head *thiz_tp_ln;
        
        if ( *fl == NULL ) {
            return 1;
        }
        thiz_tp_ln = *fl;
        thiz_tp = list_load(struct frame_node, ln, thiz_tp_ln);
        while ( ((thiz_tp->frame.can_id & 0x00FF0000) >> 16 ) != 0xEC ) {
            thiz_tp_ln = thiz_tp->ln.next;
            thiz_tp = list_load(struct frame_node, ln, thiz_tp_ln);
            if ( thiz_tp_ln == *fl ) break;
        }
        
        if ( ((thiz_tp->frame.can_id & 0x00FF0000) >> 16 ) != 0xEC ) {
            return NULL;
        }
        
        return thiz_tp;

    } while (0);
    return NULL;
}

int canframe_push(struct bmsdriver *d, struct list_head **fl, const struct can_frame *f, struct timeval *tv)
{
    if ( ((frame.can_id & 0x00FF0000) >> 16 ) == 0xEC ) {
        if ( 0x10 == frame.data[0] ) {
            struct can_frame scf;
            struct timeval stv;
            int push_ret;

            scf.data[0] = 0x11;
            // 目前的多数据包发送策略是： 无论要发送多少数据包，都一次传输完成
            scf.data[1] = frame.data[3];
            scf.data[2] = 1;
            scf.data[3] = 0xFF;
            scf.data[4] = 0xFF;
            scf.data[5] = frame.data[5];
            scf.data[6] = frame.data[6];
            scf.data[7] = frame.data[7];
            scf.can_id = 0x1CECF456 | CAN_EFF_FLAG;
            gettimeofday(&stv, NULL);

            push_ret = canframe_push(d, &(d->tx_frames), &scf, &stv);
            if ( push_ret ) {
                return 1;
            }
            
            struct frame_node *newframe =
            (struct frame_node *)malloc(sizeof(struct frame_node));
            if ( newframe == NULL ) {
                return 1;
            }

            memset(newframe, 0, sizeof(struct frame_node));
            memcpy(&newframe->tsp, tv, sizeof(struct frame_node));
            memcpy(&newframe->frame, f, sizeof(struct can_frame));
            list_init(newframe->ln);

            if ( *fl == NULL ) {
                *fl = newframe;
            } else {
                list_inserttail(*fl, newframe);
            }
            
            return 0;
        } else if ( 0xff == frame.data[0] ) {
            // connection aborted by BMS.
            canframe_tpcancel(d, &(d->tx_frames), frame.data[7]<<16|
                              frame.data[6]<<8|frame.data[5]);
            return 0;
        } else {
        }
    } else if ( ((frame.can_id & 0x00FF0000) >> 16 ) == 0xEB ) {
        return 0;
    } else {
    }
    
    struct frame_node *newframe =
    (struct frame_node *)malloc(sizeof(struct frame_node));
    if ( newframe == NULL ) {
        return 1;
    }
    
    memset(newframe, 0, sizeof(struct frame_node));
    memcpy(&newframe->tsp, tv, sizeof(struct frame_node));
    memcpy(&newframe->frame, f, sizeof(struct can_frame));
    list_init(newframe->ln);
    
    if ( *fl == NULL ) {
        *fl = newframe;
    } else {
        list_inserttail(*fl, newframe);
    }

    return 0;
}

int canframe_pull(struct bmsdriver *d, struct list_head **fl, struct can_frame *fl, struct frame_node *f)
{
    return 0;
}

int canframe_pop(struct bmsdriver *d, struct list_head **fl, struct can_frame *fl, struct frame_node *f)
{
    return 0;
}

int canframe_tpcancel(struct bmsdriver *d, struct list_head **fl, unsigned int pgn)
{
    return 0;
}













