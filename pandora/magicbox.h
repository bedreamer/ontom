#ifndef _PANDORA_S_BOX_INCLUDED_H__
#define _PANDORA_S_BOX_INCLUDED_H__

#include "../lib/lj/list.h"


/*
 * 对外协议通信接口
 *
 * +----------------P0
 * +      zeus       +               +--------------HW
 * +----+-----+------+               + Any interface +
 *      |     |                      +-------+-------+
 *      | in  | out                          |
 *      |     |                              |
 * +----+-----+-----P1                 +-----+----P[2:n]
 * +                 <------ VCAN ----->           +
 * +                 <------  CAN ----->  Protocol +
 * +  pandoras box   <------SOCKET----->   Server  +
 * +                 <------ other ---->   Process +
 * +                 <------ ..... ---->           +
 * +-----------------+                 +-----------+
 *
 *
 * 定义zeus 和 pandoras box 通信协议过程为 z2b
 * 定义pandoras box 和 protocol Server Process通信协议过程为b2b
 */
typedef enum {
    PI_VIA_VCAN = 0,
    PI_VIA_CAN,
    PI_VIA_SOCKETS,
    PI_VIA_OTHER
}CLIENT_INTERFACE;

// 每进入消息队列一条消息，该数值自动加以
typedef size_t msg_t;

// zeus 和 pandoras box通信配置接口
struct z2b_config {
    void *_private;
};

// pandoras box 和 protocol Server Process通信配置接口
struct b2p_config {
    // 协议通路
    CLIENT_INTERFACE via_way;
    void *_private;
};

// 本地配置信息
struct local_config {
    void *_private;
};

// 协议通信消息
struct pandoras_msg {
    // 消息ID
    msg_t id;
    // 消息时戳
    time_t tsp;
    // 消息处理超时时长, 秒
    size_t second;

    // 消息体大小
    size_t msg_size;
    // 消息体
    void * msg_body;

    // 消息体队列节点
    struct list_head nd;
};

// 针对客户端的操作对象集合
struct server_ops {
    int (*initialize)(void *);
    int (*destroy)(void *);

    int (*connect)(void *);
    int (*disconnect)(void *);

    int (*register_eventproc)(int (*callback)(int), void *);
    int (*read)(struct pandoras_msg *, size_t, void *);
    int (*write)(const struct pandoras_msg *, void *);

    int (*status)(void *);
};

// 协议客户端
struct pandora_client {
    // 协议名
    char protocol_name[32];
    // 配置结构
    struct b2p_config bpc;
    // 操作接口
    struct client_ops op;
    // 私有数据
    void *_private;

    // 消息队列
    struct list_head *msg_head;
    pthread_mutex_t client_lck;

    // 阻塞的消息队列
    struct list_head *msg_pedding;
    pthread_mutex_t pedding_msg_lck;

    // 协议客户端队列节点
    struct list_head nd;
};

// 数据集中器
struct pandoras_hub {
};

// 协议服务主体结构
// zeus 主动登录pandora
struct pandoras_box {
    // zeus 登录端口
    int zeus_port;
    // zeus 操作句柄
    int zeus_handle;
    // zeus 数据
    int zeus_data;

    // pandora's box -->  zeus
    struct list_head *msg_tx;
    pthread_mutex_t msg_tx_lck;
    size_t msg_tx_nr;

    // zeus -->  pandora's box
    struct list_head *msg_rx;
    pthread_mutex_t msg_rx_lck;
    size_t msg_rx_nr;

    // 协议客户端个数
    size_t pandora_client_nr;

    // 协议服务器列表
    struct list_head *clients;
    pthread_mutex_t clients_lck;
};

int pandora_register_client(struct pandora_client *, void *_private);
#endif /*_PANDORA_S_BOX_INCLUDED_H__*/
