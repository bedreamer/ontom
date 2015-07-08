/*
 * 串口通信框架
 *
 * 串口通信框架旨在简化串口的I/O逻辑。框架主要针对串口的一问一答式的
 * 通信方式，通过实现异步I/O逻辑达到高效，快速的数据响应。
 *
 * 可能用到该框架的模块有：
 *       充电机
 *       读卡器
 *       集中器
 * 使用该框架的条件是，一问一答，或是只接收，或者只发送，或者选择型应答
 * 帧数据长度确定或变长，有校验手段。
 *
 * Designed By: LiJie <bedreamer@163.com>
 * Date: 2014/10/16
 */
#ifndef _BLUE_PRINT_INCLUED_H_
#define _BLUE_PRINT_INCLUED_H_

struct charge_job;
struct bp_uart;
#define CONFIG_BP_IO_BUFF_SIZE   512

#define GPIO_TO_PIN(bank, gpio)	(32 * (bank) + (gpio))
#define	SERIAL4_CTRL_PIN	GPIO_TO_PIN(0, 19)
#define	SERIAL5_CTRL_PIN	GPIO_TO_PIN(0, 20)

#define	RX_LOW_LEVEL			0
#define	TX_HIGH_LEVEL			1

// 串口IO事件
typedef enum {
    // 串口数据结构初始化
    BP_EVT_INIT                  = 0x00,
    // 串口配置
    BP_EVT_CONFIGURE,
    // 关闭串口
    BP_EVT_KILLED,
    // 串口数据帧校验
    BP_EVT_FRAME_CHECK,
    // 切换到发送模式
    BP_EVT_SWITCH_2_TX,
    // 切换到接收模式
    BP_EVT_SWITCH_2_RX,

    // 串口接收到新数据
    BP_EVT_RX_DATA               = 0x10,
    // 串口收到完整的数据帧
    BP_EVT_RX_FRAME,

    // 串口发送数据请求
    BP_EVT_TX_FRAME_REQUEST      = 0x20,
    // 串口发送确认
    BP_EVT_TX_FRAME_CONFIRM,
    // 串口数据发送完成事件
    BP_EVT_TX_FRAME_DONE,

    // 串口接收单个字节超时，出现在接收帧的第一个字节
    BP_EVT_RX_BYTE_TIMEOUT       = 0x40,
    // 串口接收帧超时, 接受的数据不完整
    BP_EVT_RX_FRAME_TIMEOUT,

    // 串口IO错误
    BP_EVT_IO_ERROR              = 0x80,
    // 帧校验失败
    BP_EVT_FRAME_CHECK_ERROR
}BP_UART_EVENT;

// 串口状态
typedef enum {
    // 无效状态
    BP_UART_STAT_INVALID = 0x00,
    // 错误状态
    BP_UART_STAT_ALIENT,
    // 读数据状态
    BP_UART_STAT_RD,
    // 写数据状态
    BP_UART_STAT_WR
}BP_UART_STAT;

// 串口角色
typedef enum {
    // 主动设备
    BP_UART_MASTER,
    // 被动设备
    BP_UART_SLAVE
}BP_UART_ROLE;

// 帧属性
typedef enum {
    // 数据帧特性不确定
    BP_FRAME_UNSTABLE = 0x00,
    // 发送的数据帧需要接收响应帧
    BP_FRAME_NEED_RX_ACK,
    // 接收的数据帧需要发送响应帧
    BP_FRAME_NEED_TX_ACK
}BP_FRAME_ATTRIB;

// 串口事件参数
struct bp_evt_param {
    union _unamed {
        unsigned char *rx_buff;
        unsigned char *tx_buff;
    }buff;

    // 帧属性，仅在发送帧和接收到正确帧时有效
    BP_FRAME_ATTRIB attrib;
    // {{帧附加属性
    // 帧头魔数长度
    unsigned char magic_len;
    // 帧数据长度所谓位置索引
    unsigned char len_index;
    // }}

    // 接收数据包需求长度
    // 需要接收数据包的预期长度
    size_t need_bytes;

    // 缓冲区大小
    size_t buff_size;
    // 缓冲区载荷大小
    size_t payload_size;
    // 读写游标
    size_t cursor;
};

/*
 * 串口使用者
 *
 * 同一个串口可能会有多个设备使用，
 * 该结构主要用来描述主动设备下挂接的从动设备相关信息
 */
struct bp_user {
    /* 帧频率，取值范围0-10000, 所有使用者帧率加和为10000
     * 该值越大，发送频率越高
     */
    unsigned int frame_freq;
    // 计时种子
    unsigned int seed;
    // 允许连续丢帧的最大次数
    unsigned char died_line;
    // 连续丢失数据的次数
    unsigned int died;
    // 累计超时丢帧数
    unsigned int died_total;
    // 超时等待时长 毫秒
    unsigned int ttw;

    // {{ 需要串口配置数据
    unsigned int hw_bps;
    unsigned int hw_other;
    //  bit[0:7]:   停止位
    //  bit[8:15]:  校验位
    //  bit[16:23]: 数据位
    // }}
#define MAKE_UART_CFG(d, c, s) (((((d)&0xFF) <<16) | (((c)&0xFF) << 8) | ((s)&0xFF))&0x00FFFFFF)
    // 对应的作业
    struct charge_job *job;
    // 对应的充电机管理单元
    struct charger_struct *chargers;
    // 对应的采样管理单元
    struct measure_struct *measure;
    // 对应绑定的串口
    struct bp_uart *uart;

    // 已经发送的帧数
    unsigned int sent_frames;
    // 帧校验失败次数
    unsigned int check_err_cnt;
    // 累计帧校验失败次数
    unsigned int check_err_total;
    // 已经接收帧数
    unsigned int rcv_ok_cnt;
    // 发送数据帧所需时间
    // 为了适配不同数据真发送时接受发送模式切换时间长度问题
    unsigned int time_to_send;

    // 转换为发送状态的时间修正
    int swap_time_modify;
    // 私有数据
    void * _private;
    // 名称
    char name[32];

    // 使用者事件通知响应函数
    // 可接受的事件包括：
    // 串口接收到新数据
    // BP_EVT_RX_DATA               = 0x10,
    // 串口收到完整的数据帧
    // BP_EVT_RX_FRAME,
    // 串口发送数据请求
    // BP_EVT_TX_FRAME_REQUEST      = 0x20,
    // 串口发送确认
    // BP_EVT_TX_FRAME_CONFIRM,
    // 串口数据发送完成事件
    // BP_EVT_TX_FRAME_DONE,
    // 串口接收单个字节超时，出现在接收帧的第一个字节
    // BP_EVT_RX_BYTE_TIMEOUT       = 0x40,
    // 串口接收帧超时, 接受的数据不完整
    // BP_EVT_RX_FRAME_TIMEOUT,
    // 串口IO错误
    // BP_EVT_IO_ERROR              = 0x80,
    // 帧校验失败
    // BP_EVT_FRAME_CHECK_ERROR

    int (*user_evt_handle)(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                           struct bp_evt_param *param);
};

typedef enum {
    UART_MODE_NORMAL, // 普通模式
    UART_MODE_SESSION // 会话模式
}UART_MODE;

// 串口上最多可以绑定16个适用对象
#define MAX_BINDER    16
/*
 * 串口描述结构
 */
struct bp_uart {
    // 串口名，/dev/uart1, etc
    char dev_name[128];
    // 文件描述序号
    int dev_handle;
    // 串口状态
    volatile BP_UART_STAT status;
    // 硬件状态,仅针对RS485有效
    volatile BP_UART_STAT hw_status;
    // 作为485串口时的收发转换端口
    unsigned int hw_port;
    // 串口工作模式
    UART_MODE uart_mode;
    // {{ 当前串口配置数据
    unsigned int hw_bps;
    unsigned int hw_other;
    //  bit[0:7]:   停止位
    //  bit[8:15]:  校验位
    //  bit[16:23]: 数据位
    // }}

    // 初始化标识
    unsigned int init_magic;
    int (*bp_evt_handle)(struct bp_uart *self, BP_UART_EVENT evt,
                         struct bp_evt_param *param);
    BP_UART_ROLE role;
    // 接收超时定时器
    struct Hachiko_food rx_seed;
    // 发送完成定时器
    struct Hachiko_food tx_seed;
    // 发送节奏定时器
    struct Hachiko_food tx_speed;

    // 接收参数
    struct bp_evt_param rx_param;
    // 接收缓冲区
    unsigned char rx_buff[CONFIG_BP_IO_BUFF_SIZE];
    // 发送参数
    struct bp_evt_param tx_param;
    // 发送缓冲区
    unsigned char tx_buff[CONFIG_BP_IO_BUFF_SIZE];

    // 发送序列
    unsigned int sequce;
    // 连续发送次数
    unsigned int continues_nr;
    // 使用者信息
    struct bp_user *users[MAX_BINDER];
    // 使用者个数
    unsigned int users_nr;
    // 当前使用者
    struct bp_user *master;
};

static inline double __usperbyte(const struct bp_uart *bp) {
    double bitsperbyte = 10.0;

    if ( ( bp->hw_other & 0x00FF00 ) >> 8 == 'E' ||
         ( bp->hw_other & 0x00FF00 ) >> 8 == 'O' ) {
        bitsperbyte += 1.0f;
    }

    return 1000000.0 * bitsperbyte / bp->hw_bps;
}

static inline int bp_user_bind(struct bp_uart *bp, struct bp_user *u) {
    if ( !bp || !u ) return ERR_WRONG_PARAM;
    if ( bp->users_nr >= MAX_BINDER ) return ERR_WRONG_PARAM;
    do {
        struct bp_user * thiz = (struct bp_user *)malloc(sizeof(struct bp_user));
        if ( thiz == NULL ) return ERR_LOW_MEMORY;

        memcpy(thiz, u, sizeof(struct bp_user));
        bp->users[ bp->users_nr ++ ] = thiz;
    } while (0);

    return ERR_OK;
}

int uart4_bp_evt_handle(struct bp_uart *self, BP_UART_EVENT evt, struct bp_evt_param *param);
int uart4_charger_yaoce_0_49_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int uart4_charger_yaoce_50_100_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int uart4_charger_config_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int uart4_charger_module_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int uart4_charger_date_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int simple_box_read_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int simple_box_configwrite_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int simple_box_write_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int simple_box_correct_write_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int simple_box_correct_read_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int simple_box_correct_refer_V_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int ANC01_convert_box_read_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int ANC01_convert_box_write_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int ANC01_convert_box_module_on_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int ANC01_convert_box_module_off_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int Increase_module_write_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int Increase_module_read_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int Increase_convert_box_write_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int huali_3AC_kwh_meter_read_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int huali_3AV_voltage_meter_read_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int huali_3AC_kwh_meter_install_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int yada_DC_kwh_meter_read_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int yada_DC_current_meter_read_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int card_reader_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int card_init_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int cardreader_install_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int uart_sniffer_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
int uart5_background_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
void * thread_uart_service(void *arg) ___THREAD_ENTRY___;
void uart4_Hachiko_speed_proc(Hachiko_EVT evt, void *_private,
                            const struct Hachiko_food *self);
#endif // _BLUE_PRINT_INCLUED_H_
