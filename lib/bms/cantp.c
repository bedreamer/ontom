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

void SAE_J1939_21_gen_RTS(struct can_frame *f, size_t s, size_t np, size_t maxnp, size_t pgn, size_t canid)
{
    f->can_id = canid;
    f->data[0] = 0x10;
    f->data[1] = (s >> 0) & 0xff;
    f->data[2] = (s >> 8) & 0xff;
    f->data[3] = np;
    f->data[4] = maxnp;
    f->data[5] = (pgn >> 0) & 0xFF;
    f->data[6] = (pgn >> 8) & 0xff;
    f->data[7] = (pgn >> 16) & 0xff;
}

void SAE_J1939_21_gen_CTS(struct can_frame *f, size_t np, size_t bp, size_t pgn, size_t canid)
{
    f->can_id = canid;
    f->data[0] = 0x11;
    f->data[1] = np;
    f->data[2] = bp;
    f->data[3] = 0xff;
    f->data[4] = 0xff;
    f->data[5] = (pgn >> 0) & 0xFF;
    f->data[6] = (pgn >> 8) & 0xff;
    f->data[7] = (pgn >> 16) & 0xff;
}

void SAE_J1939_21_gen_ACK(struct can_frame *f, size_t s, size_t np, size_t pgn, size_t canid)
{
    f->can_id = canid;
    f->data[0] = 0x13;
    f->data[1] = (s >> 0) & 0xff;
    f->data[2] = (s >> 8) & 0xff;
    f->data[3] = np;
    f->data[4] = 0xff;
    f->data[5] = (pgn >> 0) & 0xFF;
    f->data[6] = (pgn >> 8) & 0xff;
    f->data[7] = (pgn >> 16) & 0xff;
}

void SAE_J1939_21_gen_ABORT(struct can_frame *f, int reason, size_t pgn, size_t canid)
{
    f->can_id = canid;
    f->data[0] = 0xff;
    f->data[1] = reason;
    f->data[2] = 0xff;
    f->data[3] = 0xff;
    f->data[4] = 0xff;
    f->data[5] = (pgn >> 0) & 0xFF;
    f->data[6] = (pgn >> 8) & 0xff;
    f->data[7] = (pgn >> 16) & 0xff;
}

static int __do_canframe_push(struct bmsdriver *d, struct list_head **fl, const struct can_frame *f, struct timeval *tv, int ddr)
{    
    struct frame_node *newframe = (struct frame_node *)malloc(sizeof(struct frame_node));
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

static inline int __do_SAE_J1939_21_TP_RTS_push(struct bmsdriver *d, struct list_head **fl, const struct can_frame *f, struct timeval *tv, int ddr)
{
    struct can_frame scf;
    struct timeval stv;
    int push_ret;

    gettimeofday(&stv, NULL);

    if ( ddr == DDR_RX ) {
        // 请求大数据包传输
        SAE_J1939_21_gen_CTS(&scf, frame.data[3], 1,
                mkpgn(frame.data[5],frame.data[6],frame.data[7]), 0x1CECF456 | CAN_EFF_FLAG);
        push_ret = canframe_push(d, &(d->tx_frames), &scf, &stv, DDR_TX);
        if ( push_ret ) {
            return 1;
        }
    } else if ( ddr == DDR_TX ) {
        // 请求发送大数据包
        SAE_J1939_21_gen_CTS(&scf, frame.data[3], 1,
                mkpgn(frame.data[5],frame.data[6],frame.data[7]), 0x1CECF456 | CAN_EFF_FLAG);
        push_ret = canframe_push(d, &(d->tx_frames), &scf, &stv, DDR_TX);
        if ( push_ret ) {
            return 1;
        }
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

static inline int __do_SAE_J1939_21_TP_CTS_push(struct bmsdriver *d, struct list_head **fl, const struct can_frame *f, struct timeval *tv, int ddr)
{
    return 0;
}

static inline int __do_SAE_J1939_21_TP_ACK_push(struct bmsdriver *d, struct list_head **fl, const struct can_frame *f, struct timeval *tv, int ddr)
{
    return 0;
}

static inline int __do_SAE_J1939_21_TP_ABORT_push(struct bmsdriver *d, struct list_head **fl, const struct can_frame *f, struct timeval *tv, int ddr)
{
    return 0;
}

static inline int __do_SAE_J1939_21_TP_BAM_push(struct bmsdriver *d, struct list_head **fl, const struct can_frame *f, struct timeval *tv, int ddr)
{
    return 0;
}

static inline int __do_SAE_J1939_21_TP_CM_push(struct bmsdriver *d, struct list_head **fl, const struct can_frame *f, struct timeval *tv, int ddr)
{
    if ( 0x10 == f->data[0] ) {
        // RTS
        return __do_SAE_J1939_21_TP_RTS_push(d, fl, f, tv, ddr);
    } else if ( 0x11 == f->data[0] ) {
        // CTS
        return __do_SAE_J1939_21_TP_CTS_push(d, fl, f, tv, ddr);
    } else if ( 0x13 == f->data[0] ) {
        // ACK
        return __do_SAE_J1939_21_TP_ACK_push(d, fl, f, tv, ddr);
    } else if ( 0xff == f->data[0] ) {
        // ABORT
        return __do_SAE_J1939_21_TP_ABORT_push(d, fl, f, tv, ddr);
    } else if ( 0x20 == f->data[0] ) {
        // BAM
        return __do_SAE_J1939_21_TP_BAM_push(d, fl, f, tv, ddr);
    } else {
        return 1;
    }
}

static int __do_SAE_J1939_21_TP_DT_push(struct bmsdriver *d, struct list_head **fl, const struct can_frame *f, struct timeval *tv, int ddr)
{
    return 0;
}


int canframe_push(struct bmsdriver *d, struct list_head **fl, const struct can_frame *f, struct timeval *tv, int ddr)
{
    if ( ((frame.can_id & 0x00FF0000) >> 16 ) == 0xEC ) {
        return __do_SAE_J1939_21_TP_CM_push(d, *fl, f, tv, ddr);
    } else if ( ((frame.can_id & 0x00FF0000) >> 16 ) == 0xEB ) {
        return __do_SAE_J1939_21_TP_DT_push(d, *fl, f, tv, ddr);
    } else {
        return __do_canframe_push(d, *fl, f, tv, ddr);
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













