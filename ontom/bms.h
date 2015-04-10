/*
 * BMS 通信数据包定义
 * 版本 V 1.0
 * 遵循标准： GB/T 27930-2011, SAE J1939-21
 */
#ifndef _BMS_PACKAGE_INCLUDED_H_
#define _BMS_PACKAGE_INCLUDED_H_

// 充电机地址
#define CAN_ADDR_CHARGER   0x56  // 86
// BMS地址
#define CAN_ADDR_BMS       0xF4  // 244
#define CAN_RCV_ID_MASK    ((CAN_ADDR_CHARGER<<8)|CAN_ADDR_BMS)
#define CAN_TX_ID_MASK    ((CAN_ADDR_CHARGER)|CAN_ADDR_BMS<<8)

typedef enum {
    PGN_CRM = 0x000100,
    PGN_BRM = 0x000200,
    PGN_BCP = 0x000600,
    PGN_CTS = 0x000700,
    PGN_CML = 0x000800,
    PGN_BRO = 0x000900,
    PGN_CRO = 0x000A00,
    PGN_BCL = 0x001000,
    PGN_BCS = 0x001100,
    PGN_CCS = 0x001200,
    PGN_BSM = 0x001300,
    PGN_BMV = 0x001500,
    PGN_BMT = 0x001600,
    PGN_BSP = 0x001700,
    PGN_BST = 0x001900,
    PGN_CST = 0x001A00,
    PGN_BSD = 0x001C00,
    PGN_CSD = 0x001D00,
    PGN_BEM = 0x001E00,
    PGN_CEM = 0x001F00
}CAN_PGN;

/*
 * 读到一个完整数据包后调用该函数
 */
typedef enum {
    // 无效事件，初始化完成前收到该消息
    EVENT_INVALID    = 0,

    /* 数据包接受完成。系统中存在两个和CAN接口相关的线程，一个
     * 是CAN数据读取线程，还有一个是CAN数据发送线程，这两个线程
     * 对CAN的操作都是同步的，因此，在读取数据包时读线程会被阻塞
     * 因此需要通过该事件来通知系统，已经完成了数据包的读取。
     */
    EVENT_RX_DONE    = 1,

    /* 数据包接收错误。在阻塞读取CAN数据包时，发生数据包读取错误，
     * 会收到该事件，表征数据包有问题，应该作相应处理。
     */
    EVENT_RX_ERROR   = 2,

    /* 数据包发送请求。系统中CAN数据的发送线程是同步的，可以通过
     * 该事件来提醒系统，目前可以进行数据包的发送，若系统有数据包发送
     * 则设定发送数据，交由发送县城进行发送，若没有数据要发送，则返回一个
     * 不需要发送的结果即可。
     */
    EVENT_TX_REQUEST = 3,

    /* 连接管理模式下的请求发送数据包，进行连接数据控制。
     */
    EVENT_TX_TP_RTS = 4,
    /* 连接管理模式下的准备发送数据包，进行连接数据控制。
     */
    EVENT_TX_TP_CTS = 5,
    /* 连接管理模式下的接收数据包完成应答，进行连接数据控制。
     */
    EVENT_TX_TP_ACK = 6,
    /* 连接管理模式下的传输中止，进行连接数据控制。
     */
    EVENT_TX_TP_ABRT= 7,

    /* 数据包准备发送。当EVENT_TX_REQUEST返回结果是需要发送时，经发送线程
     * 经发送线程确认后，将会受到该消息，表示发送线程已经准备发送该消息了，此时
     * 可以返回取消发送指令，实现数据包的取消发送。
     */
    EVENT_TX_PRE     = 8,

    /* 数据包发送完成。当确认后的数据包发送完成后，将会受到该消息，表征数据包
     * 已经正确的发送完成。
     */
    EVENT_TX_DONE    = 9,

    /* 数据包发送失败。当确认后的数据包发送失败后，将会受到改小。
     */
    EVENT_TX_FAILS   = 10,

    // CAN 消息函数初始化。当第一次运行函数时将会收到该消息，可重复发送。
    EVENT_CAN_INIT   = 11,

    // CAN 消息复位。再次执行初始化操作。
    EVENT_CAN_RESET  = 12
}EVENT_CAN;

// 事件通知返回/传入参数
typedef enum {
    // 无效
    EVT_RET_INVALID  = 0,
    // 一切OK
    EVT_RET_OK       = 1,
    // 失败了
    EVT_RET_ERR      = 2,
    // 超时
    EVT_RET_TIMEOUT  = 3,
    // 终止发送，EVENT_CAN.EVENT_TX_PRE的可用参数
    EVT_RET_TX_ABORT = 4
}EVT_PARAM;

// 事件通知结构
struct event_struct {
    // 事件参数
    EVT_PARAM evt_param;

    union {
        // 发送缓冲区地址， 针对EVENT_TX_REQUEST设置
        unsigned char *tx_buff;
        // 接收缓冲区地址，针对EVENT_RX_DONE设置
        const unsigned char* rx_buff;
    }buff;
    // 缓冲区大小
    unsigned int buff_size;
    // CAN ID
    unsigned int can_id;
    // 缓冲区载荷大小
    unsigned int buff_payload;
};

// BMS通信统计技术结构
struct bms_statistics {
    // 数据包PGN
    CAN_PGN can_pgn;
    // 数据包静默时间，只针对接受数据包有效
    unsigned int can_silence;
    // 接受数据包容忍的沉默时常，针对接收数据有效
    unsigned int can_tolerate_silence;
    // 计数器，接收或发送的次数
    unsigned int can_counter;
};
typedef enum {
    I_CRM = 0,
    I_BRM = 1,
    I_BCP = 2,
    I_CTS = 3,
    I_CML = 4,
    I_BRO = 5,
    I_CRO = 6,
    I_BCL = 7,
    I_BCS = 8,
    I_CCS = 9,
    I_BSM = 10,
    I_BMV = 11,
    I_BMT = 12,
    I_BSP = 13,
    I_BST = 14,
    I_CST = 15,
    I_BSD = 16,
    I_CSD = 17,
    I_BEM = 18,
    I_CEM = 19
}CAN_PGN_STATISTICS;

int about_packet_reciev_done(struct charge_job *thiz, struct event_struct *param);
int gen_packet_PGN256(struct charge_job * thiz, struct event_struct* param);
int gen_packet_PGN1792(struct charge_job * thiz, struct event_struct* param);
int gen_packet_PGN2048(struct charge_job * thiz, struct event_struct* param);
int gen_packet_PGN2560(struct charge_job * thiz, struct event_struct* param);
int gen_packet_PGN4608(struct charge_job * thiz, struct event_struct* param);
int gen_packet_PGN6656(struct charge_job * thiz, struct event_struct* param);
int gen_packet_PGN7424(struct charge_job * thiz, struct event_struct* param);
int gen_packet_PGN7936(struct charge_job * thiz, struct event_struct* param);
extern void * thread_bms_write_service(void *) ___THREAD_ENTRY___;
extern void * thread_bms_read_service(void *) ___THREAD_ENTRY___;
#endif /*_BMS_PACKAGE_INCLUDED_H_*/
