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

int canframe_push(struct bmsdriver *, struct list_head **fl, const struct can_frame *f, struct timeval *tv);
int canframe_pull(struct bmsdriver *, struct list_head **fl, struct frame_node *f);
int canframe_pop(struct bmsdriver *, struct list_head **fl, struct frame_node *f);
int canframe_tpcancel(struct bmsdriver *, struct list_head **fl, unsigned int pgn);


#endif /* defined(__ontom__cantp__) */
