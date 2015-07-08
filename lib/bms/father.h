#ifndef BMS_FATHER_H
#define BMS_FATHER_H

/*
 * 不同厂商的BMS通信逻辑可能不同，但又不能在程序中将不同BMS场上的特性完全包含进来，
 * 因为如果这么做，只会导致程序发生膨胀，将导致无法扩展等问题，因此在这里定一个胶合
 * 层来连接控制逻辑和BMS管理逻辑，对不同的BMS通信逻辑来说只需要提供相应的接口即可，
 * 而程序的更新也不会因为控制逻辑的变动而影响到BMS通信逻辑。
 * 定义：
 *    BMS通信逻辑的实现又外部不同的动态链接库实现，该动态链接库成为BMS驱动程序。
 *    其文件名必须为  bmsdrv_$(vendorid)_$(version).so的形式
 *
 * 使用该驱动接口的前提条件是，所有BMS的通信基本满足GB/T27930-2011, 且多包传输满足
 * SAE J1939-21 标准，且握手阶段必须满足.
 *
 * 为了实现兼容，所有数据字段定义均放在 charge.h 中定义，若BMS上传数据包有不同之处
 * 需要主动做相关数据移位。
 */
// 充电机地址
#define CAN_ADDR_CHARGER   0x56  // 86
// BMS地址
#define CAN_ADDR_BMS       0xF4  // 244
#define CAN_RCV_ID_MASK    ((CAN_ADDR_CHARGER<<8)|CAN_ADDR_BMS)
#define CAN_TX_ID_MASK    ((CAN_ADDR_CHARGER)|CAN_ADDR_BMS<<8)

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
    EVENT_CAN_RESET  = 12,
    
    // 销毁事件
    EVENT_CAN_DESTROY= 13
}BMS_EVENT_CAN;

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
}BMS_EVT_PARAM;

struct charge_job;
struct charge_task;

// 事件通知结构
struct bms_event_struct {
    // 事件参数
    BMS_EVT_PARAM evt_param;

    struct {
        // 发送缓冲区地址， 针对EVENT_TX_REQUEST设置
        unsigned char tx_buff[32];
        // 接收缓冲区地址，针对EVENT_RX_DONE设置
        unsigned char rx_buff[256];
    }buff;
    // 缓冲区大小
    unsigned int buff_size;
    // CAN ID
    unsigned int can_id;
    // 缓冲区载荷大小
    unsigned int buff_payload;
};
static inline void __dump_can_param(struct bms_event_struct *p) {
    char fmt[512] = {0};
    unsigned int i;
    if ( p == NULL ) return;
    if ( p->buff_payload == 0 ) return;

    for ( i = 0; i < p->buff_payload; i ++ ) {
        sprintf(fmt, "%02X ", p->buff.rx_buff[i]);
    }

    log_printf(DBG_LV3, "BMS: %s", fmt);
}

/*
 * BMS 通信驱动接口
 */
struct bmsdriver {
    unsigned int vendor_id;
    char version[16];
    char vendor_name[64];

    void *handle;

    // 被加载时间
    time_t loaded;
    // 只有一个CAN 因此最多能绑定一个作业
    // 不排除以后多枪充电的可能
    // binder[x][0] ==> 读数据线程
    // binder[x][1] ==> 写数据线程
    struct charge_job *binder[1][2];

    /* 安装驱动到系统中, 做相关的初始化动作 */
    int (*driver_install)(struct charge_task *);
    /* 卸载系统中的驱动, 做相关的清理动作 */
    int (*driver_uninstall)();

    /* 驱动主函数，接收驱动事件 */
    int (*driver_main_proc)(struct charge_job *, BMS_EVENT_CAN,
                            struct bms_event_struct *, struct bmsdriver *);

    // 数据包生成器个数
    unsigned int can_pack_gen_nr_copy;
    // 数据包生成器数据库拷贝
    struct can_pack_generator *generator_copy;
#ifdef _USE_NEW_FRAMEWORK
    struct frame_node *rx_frames;
    struct frame_node *tx_frames;
#endif //_USE_NEW_FRAMEWORK
    struct bmsdriver *next;
};

// 搜索指定PGN的生成器
struct can_pack_generator *gen_search(struct can_pack_generator *p, unsigned int nr, unsigned int pgn);

/* 搜索对应生产商的BMS驱动
 * 先从bms驱动缓存中搜索，如果没有搜索到则到bms驱动目录搜索，若没有则会导致
 * 驱动不匹配而无法充电。
 */
struct bmsdriver *bmsdriver_search(struct charge_task *tsk, unsigned int vendor_id, const char *ver);

/*
 * 作业和驱动进行绑定
 * 一个驱动可以在同一时间驱动两个作业，因此将驱动和作业进行绑定可以提高效率
 */
int bind_bmsdriver(struct bmsdriver *drv, struct charge_job *job);

/*
 * BMS 驱动初始化
 */
int bmsdriver_init(struct charge_task *);

#endif // BMS_FATHER_H
