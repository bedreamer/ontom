//
//  cantp.h
//  ontom
//
//  Created by 李杰 on 15/7/8.
//
//

#ifndef __ontom__cantp__
#define __ontom__cantp__

#include <stdio.h>

/*
 采用 SAE J1939-21 多包传输协议
 
 独立线程运行
 */

struct frame_node {
    struct timeval tsp;
    struct can_frame frame;

    // 用于连接传输的数据包链表
    struct frame_node *tp_frames;
    struct list_head ln;
};

#define DDR_TX  0xF0  // 数据方向为发送
#define DDR_RX  0x0F  // 数据方向为接收

struct push_struct {
    struct bmsdriver *d;
    struct list_head **fl;
    const struct can_frame *f;
    struct timeval *tv;
    int ddr;
};

int canframe_push(struct bmsdriver *, struct list_head **fl, const struct can_frame *f, struct timeval *tv);
int canframe_pull(struct bmsdriver *, struct list_head **fl, struct frame_node *f);
int canframe_pop(struct bmsdriver *, struct list_head **fl, struct frame_node *f);
int canframe_tpcancel(struct bmsdriver *, struct list_head **fl, unsigned int pgn);

void SAE_J1939_21_gen_RTS(struct can_frame *f, size_t s, size_t np, size_t maxnp, size_t pgn);
void SAE_J1939_21_gen_CTS(struct can_frame *f, size_t np, size_t bp, size_t pgn);
void SAE_J1939_21_gen_ACK(struct can_frame *f, size_t s, size_t np, size_t pgn);
void SAE_J1939_21_gen_ABORT(struct can_frame *f, int reason, size_t pgn);

#define mkpgn(l,m,h) ((l&0xff)|((m&0xff)<<8)|((h&0xff)<<16))

#endif /* defined(__ontom__cantp__) */
