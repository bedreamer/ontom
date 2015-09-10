#ifndef _PANDORA_S_BOX_INCLUDED_H__
#define _PANDORA_S_BOX_INCLUDED_H__

#include "../lib/lj/list.h"

#define _USE_P_BJNR_PROTOCOL
typedef size_t pack_t;
typedef size_t unit_t;
typedef size_t func_t;
typedef size_t msg_t;

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
struct pandoras_box;
struct pandora_client;
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
struct   {
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
struct client_ops {
    int (*do_initialize)(struct pandoras_box *pb, struct pandora_client *pc);
    int (*do_working)(struct pandoras_box *pb, struct pandora_client *pc);
    int (*do_pause)(struct pandoras_box *pb, struct pandora_client *pc);
    int (*do_exit)(struct pandoras_box *pb, struct pandora_client *pc);
    int (*do_destroy)(struct pandoras_box *pb, struct pandora_client *pc);

    int (*do_connect)(void *);
    int (*do_disconnect)(void *);

    int (*register_eventproc)(int (*callback)(int), void *);
    int (*do_read)(struct pandoras_msg *, size_t, void *);
    int (*do_write)(const struct pandoras_msg *, void *);

    int (*status)(void *);
};

typedef enum {
    STAGE_INIT,
    STAGE_WORKING,
    STAGE_PAUSE,
    STAGE_EXIT,
    STAGE_PURGE
}CLIENT_STAGE;

typedef enum {
    CF_BEGIN = 0x00,

    // {{ 连接故障定义
    // 1: 连接丢失， 0: 连接正常
    CS_LOSS_CONN,
    // 1: 登录回应超时, 0: 未超时
    CS_LOGIN_TIMEOUT,
    // 1: 心跳回应超时，0: 未超时
    CS_HEARTBEAT_TIMEOUT,
    // 1: 计费回应超时，0: 未超时
    CS_BILLING_TIMEOUT,
    // 1: 请求充电超时，0: 未超时
    CS_REQUEST_CHARGE_TIMEOUT,
    // }}

    // 1: 超时之前已经发送的登录报文,0:反之
    CF_TX_LOGIN,
    // 1: 已经登录， 0: 未登录或登录失败
    CF_LOGED_IN,
    // 1: 已经发送了计费数据包
    CF_TX_BILLING,

    CF_END   = 0x3F
}CLIENT_FLAG_SINGLE;

#if 0
// 位设置
static inline void __bit_set(volatile unsigned char *byte, CLIENT_FLAG_SINGLE single)
{
    if ( ! byte ) return;
    byte += single / 8;
    * byte |= (1 << (single % 8 ));
}
#define bit_set(tsk, bits) __bit_set(tsk->single, bits)
#define bit_set_pre(tsk, bits) __bit_set(tsk->pre_single, bits)
// 位清除
static inline void __bit_clr(volatile unsigned char *byte, CLIENT_FLAG_SINGLE single)
{
    if ( ! byte ) return;
    byte += single / 8;
    * byte &= (~(1 << (single % 8 )));
}
#define bit_clr(tsk, bits) __bit_clr(tsk->single, bits)
#define bit_clr_pre(tsk, bits) __bit_clr(tsk->pre_single, bits)
// 位读取
static inline int __bit_read(volatile unsigned char *byte, CLIENT_FLAG_SINGLE single)
{
    if ( ! byte ) return 0;
    byte += single / 8;

    return (* byte & (1 << (single % 8 ))) ? 1 : 0;
}
#define bit_read(tsk, bits) __bit_read(tsk->single, bits)
#define bit_read_pre(tsk, bits) __bit_read(tsk->pre_single, bits)
#endif

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

    // 运行阶段
    CLIENT_STAGE stage;
    volatile unsigned char single[8];

    // 接收到的消息队列
    struct list_head *rx_msg_head;
    pthread_mutex_t rx_msg_lck;
    // 待发送的消息队列
    struct list_head *tx_msg_head;
    pthread_mutex_t tx_msg_lck;

    // 阻塞的消息队列
    struct list_head *msg_pedding;
    pthread_mutex_t pedding_msg_lck;

    // 协议客户端队列节点
    struct list_head nd;
};

// 操作对象
typedef enum {
    // 被zeus 主动更新
    ZEUS_HANDS,
    // 被人为更新
    MASTER_HANDS,
    // 被后台程序更新
    PANDORAS_HANDS
}WRITE_HANDS;

// 字段定义
struct pack_section {
    // 字段ID
    unit_t uid;
    // 字段名称
    char u_name[32];
    // 字段基地址
    void *u_base_addr;
    // 字段偏移
    size_t u_offset;
    // 字段长度
    size_t u_size;
    // 字段类型
    char u_type[16];
    // 字段格式
    char u_format[16];

    // 更新对象
    WRITE_HANDS u_user;
    // 前一次更新时戳
    time_t u_pre_updated;
    // 更新时戳
    time_t u_last_updated;
};

// 共享数据字段
struct shared_blocks {
    // PID
    pack_t pid;
    // 共享字段名称
    char shared_name[32];

    // 共享字段个数
    size_t segs_nr;
    // 共享数据字段描述结构
    struct pack_section **segs;

    //  共享数据块大小
    size_t shared_buff_size;
    // 共享数据基地址
    char *shared_buff_base_addr;
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

    // 共享字段个数
    size_t sh_blocks_nr;
    // 共享字段描述符
    struct shared_blocks **sh_blocks;
    pthread_mutex_t sh_blocks_lck;

    // 任务管理单元
    struct charge_task *tsk;
};

int pandora_register_client(struct pandoras_box *, struct pandora_client *, void *_private);
int pandora_init(void *p);
int shared_block_touch(struct shared_blocks ***sb, size_t shared_blocks_nr);
int shared_block_fill(struct shared_blocks *, pack_t, const char *, size_t);
struct shared_blocks* shared_block_name_search(struct shared_blocks **, size_t, const char *);
struct shared_blocks* shared_block_pid_search(struct shared_blocks **, size_t, pack_t);

int pack_section_touch(struct pack_section ***ps, size_t section_nr);
int pack_section_fill(struct pack_section *, unit_t, const char *, void *base, size_t offset,
                      size_t usize, const char *type, const char *format);
struct pack_section* pack_section_name_search(struct pack_section **, size_t, const char *);
struct pack_section* pack_section_uid_search(struct pack_section **, size_t, unit_t);

#ifdef _USE_P_BJNR_PROTOCOL
int BJNR_initialize(struct pandoras_box *pb, struct pandora_client *pc);
int BJNR_working(struct pandoras_box *pb, struct pandora_client *pc);
int BJNR_pause(struct pandoras_box *pb, struct pandora_client *pc);
int BJNR_exit(struct pandoras_box *pb, struct pandora_client *pc);
int BJNR_destroy(struct pandoras_box *pb, struct pandora_client *pc);
#endif

int pandora_frame_push(struct list_head **, msg_t, size_t, const void *);
struct pandoras_msg* pandora_frame_pop(struct list_head **);
int pandora_frame_join(struct list_head **, struct list_head *);


#endif /*_PANDORA_S_BOX_INCLUDED_H__*/
