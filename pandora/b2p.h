#ifndef __Z2B_H_
#define __Z2B_H_

void * protocol_client_proc(void *p);

void protocol_server_step(struct pandoras_box *, struct pandora_client *);

enum {
    TSP_LOGIN,
    TSP_HEARTBEAT,
    TSP_SYNCTIME,
    TSP_DEVSTATACK,
    TSP_CHARGESTATACK,
    TSP_QUERYABBOUNTACK,
    TSP_SYSTEM_YX,
    TSP_SYSTEM_ERR_YX,
    TSP_JOB_YX,
    TSP_SYSTEM_YC,
    TSP_JOB_YC,
    TSP_1_BMS_YC,
    TSP_1_BMS_V_YC,
    TSP_2_BMS_YC,
    TSP_2_BMS_V_YC,
    TSP_MODULES_YC
}TSP_TX;

// socket 连接会话
struct socket_session {
    // 服务端连接句柄
    int s_srv;
    // 服务端地址
    char s_srv_ip[32];
    // 服务端端口
    int s_srv_port;

    // 最后一次尝试连接的时间戳
    time_t last_conn_tsp;
    // 连接建立的时间戳
    time_t established_tsp;

    // 数据包序号
    unsigned char pack_seq;
    // 当前心跳计数
    unsigned short heart_beate;

    // 发送缓冲区
    unsigned char rx_buff[1024];
    size_t rx_buff_size;
    size_t this_frame_len;

    // 接收缓冲区
    unsigned char tx_buff[1024];
    size_t tx_buff_size;

    // 接收临时缓冲区
    FILE *tmp_rx;
    // 发送临时缓冲区
    FILE *tmp_tx;

    // 数据帧无回应的次数
    unsigned char no_ack[16];

    // 最后一次数据包发送时间戳
    time_t tx_tsp[32];
    // 最后一次数据包接收时间戳
    time_t rx_tsp[32];
};

void *search_pack(void *pool, size_t count, msg_t id, int seq);

typedef enum {
    FDR_B2P,  // box 到协议服务端
    FDR_P2B,  // 协议服务端到box
    FDR_Z2B,  // zeus 到box
    FDR_B2Z   // box 到  zeus
}FDR;

// 帧结构
struct frame_struct {
    msg_t id;
    char aligs[32];
    char name[32];
    FDR fdr;

    unsigned short head;
    unsigned short len;
};

// 回收应答或超时后的阻塞队列中的数据帧
int do_frame_recycle(struct pandoras_box *pb, struct pandora_client *pc, struct socket_session *ss, msg_t id);

// 充电请求
struct msg_id_0x0A_stop_charge {
    unsigned char gun_sn;
    unsigned char action;
    unsigned char deal_sn[8];
    unsigned char account_type;
    unsigned char account[32];
    unsigned char passwd[16];
    int pre_billing;
    time_t time;
    int remain;
    int workmode;
    int limit_vol;
    int limit_current;
    int chargemode;
    int param;
};

// 停止充电请求
struct msg_id_0x0A_charge {
    unsigned char gun_sn;
    unsigned char action;
};

#endif /*__Z2B_H_*/
