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

#ifndef u8
#define u8 unsigned char
#endif
#ifndef u16
#define u16 unsigned short
#endif
#ifndef u32
#define u32 unsigned int
#endif

#pragma pack(1)
// 握手阶段
// 充电机辨识报文
struct pgn256_CRM {
    // 充电机辨识结果 @ enum recognize_result
    u8 spn2560_recognize;

    // 充电机编号， 范围1-100
    u8 spn2561_charger_sn;
    // 充电机/充电站所在区域编码
    unsigned char spn2562_charger_region_code[6];
};
enum recognize_result {
    BMS_RECOGNIZED     =  0xAA, // 充电机能识别BMS
    BMS_NOT_RECOGNIZED =  0x00  // 充电机不能识别BMS
};

// BMS辨识报文
struct pgn512_BRM {
    // BMS 通信协议版本号, 0x000100
    unsigned char spn2565_bms_version[3];
    /* 电池类型
     * 0x01: 铅酸电池
     * 0x02: 镍氢电池
     * 0x03: 磷酸铁锂电池
     * 0x04: 锰酸锂电池
     * 0x05: 钴酸锂电池
     * 0x06: 三元材料电池
     * 0x07: 聚合物锂离子电池
     * 0x08: 钛酸锂电池
     * 0xFF: 其他电池
     */
    u8 spn2566_battery_type;
    // 蓄电池额定容量, 0.1A.H 每位， 0-1000A.H
    u16 spn2567_capacity;
    // 蓄电池额定电压， 0.1V 每位，0-750V
    u16 spn2568_volatage;

    // 以下成员结构为可选结构, 若没有需要使用1填充
    // 电池生产商名称,ASCII 码
    unsigned char spn2569_battery_manufacturer[4];
    // 电池组序列号
    unsigned char spn2570_battery_sn[4];
    // 电池组生产日期
    u8 spn2571_year; // 1985年偏移
    u8 spn2571_month;
    u8 spn2571_day;
    // 电池组充电次数
    unsigned char spn2572_charge_count[3];
    // 电池组产权标识 @ enum battery_property
    u8 spn2573_battery_property;

    u8 spn2574_reserved; // 保留
    // 车辆识别码
    unsigned char spn2575_vin[17];
};
enum battery_property {
    BATTERY_LEASE  = 0x00, // 租赁
    BATTERY_OWNER  = 0x01  // 私有
};

// 充电参数配置阶段
// 动力蓄电池充电参数
struct pgn1536_BCP {
    // 单节电池允许最高充电电压, 0.01V 每位， 0-24V
    u16 spn2816_max_charge_volatage_single_battery;
    // 最高允许充电电流, 0.1A每位，-400A偏移 -400A - 0A
    u16 spn2817_max_charge_current;
    // 蓄电池标称总能量, 0.1kW。h每位， 0-1000 kW。h
    u16 spn2818_total_energy;
    // 最高允许充电电压, 0.1V 每位， 0-750V
    u16 spn2819_max_charge_voltage;
    // 最高允许温度，1 度每位，-50度偏移，-50 度 - 200度
    u8 spn2820_max_temprature;
    // 整车蓄电池荷电状态，0.1% 每位，0-100%
    u16 spn2821_soc;
    // 整车蓄电池总电压， 0.1V 每位，0-750V
    u16 spn2822_total_voltage;
};

// 充电机发送时间同步信息
struct pgn1792_CTS {
    // 压缩的BCD码
    // 第6-7字节为年
    u8 spn2823_bcd_sec;
    u8 spn2823_bcd_min;
    u8 spn2823_bcd_hour;
    u8 spn2823_bcd_day;
    u8 spn2823_bcd_mon;
    u8 spn2823_bcd_year_h;
    u8 spn2823_bcd_year_l;
};

// 充电机最大输出能力
struct pgn2048_CML {
    // 最大输出电压, 0.1V 每位， 0-750V
    u16 spn2824_max_output_voltage;
    // 最低输出电压，0.1V 每位，0-750V
    u16 spn2825_min_output_voltage;
    // 最大输出电流，0.1A 每位，-400A偏移，范围-400-0 A
    u16 spn2826_max_output_current;
};

// 电池充电准备就绪状态
struct pgn2304_BRO {
    // BMS 充电准备完成 @ enum charger_charge_status
    u8 spn2829_bms_ready_for_charge;
};
enum bms_charge_status {
    BMS_NOT_READY_FOR_CHARGE =  0x00, // 没有准备好
    BMS_READY_FOR_CHARGE     =  0xAA, // 已准备好
    BMS_INVALID              =  0xFF // 无效
};

// 充电机输出准备就绪状态
struct pgn2560_CRO {
    // 充电机充电准备完成 @ enum charger_charge_status
    u8 spn2830_charger_ready_for_charge;
};
enum charger_charge_status {
    CHARGER_NOT_READY_FOR_CHARGE = 0x00, // 没有准备好
    CHARGER_READY_FOR_CHARGE     = 0xAA, // 已准备好
    CHARGER_INVALID              = 0xFF // 无效
};

// 充电阶段
// 电池充电需求
struct pgn4096_BCL {
    // 充电电压需求，0.1V 每位，0-750V
    u16 spn3072_need_voltage;
    // 充电电流需求，0.1A 每位，-400V偏移，-400A-0A
    u16 spn3073_need_current;
    // 充电模式 @ enum charge_mode
    u8  spn3074_charge_mode;
};
enum charge_mode {
    CHARGE_WITH_CONST_VOLTAGE  = 0x01,  // 恒流充电
     CHARGE_WITH_CONST_CURRENT = 0x02  // 恒压充电
};

// 电池充电总状态
struct pgn4352_BCS {
    // BMS 的充电电压测量值，0.1V 每位，0V偏移， 0-750V
    u16 spn3075_charge_voltage;
    // BMS的充电电流测量值，0.1A 每位， -400A偏移， -400A-0A
    u16 spn3076_charge_current;
    // 最高单体动力蓄电池电压及其组号，1-12位： 最高单体电池电压， 0.01V 每位， 0-24V
    // 13-16： 最高电压电池所在组号， 1/位， 1-16
    u16 spn3077_max_v_g_number;
    // 当前荷电容量, 1% 每位， 0-100%
    u8  spn3078_soc;
    // 剩余充电时间, 1Min 每位 0-600min
    u16 spn3079_need_time;
};

// 充电机充电状态
struct pgn4608_CCS {
    // 充电机输出电压，0.1V/位， 0V偏移量，0-750V
    u16 spn3081_output_voltage;
    // 充电机输出电流，0.1A/位，-400A偏移，-400A-0A
    u16 spn3082_outpu_current;
    // 充电持续时间，1min/位，0偏移，0-600min
    u16 spn3083_charge_time;
};

// 动力蓄电池状态信息
struct pgn4864_BSM {
    // 最高单体电压的蓄电池编号
    u8 sn_of_max_voltage_battery;
    // 最高蓄电池温度
    u8 max_temperature_of_battery;
    // 最高温度采样点编号
    u8 sn_of_max_temperature_point;
    // 最低蓄电池温度
    u8 min_temperature_of_battery;
    // 最低温度采样点编号
    u8 sn_of_min_temperature_point;
    // 遥信 @ enum pgn4846_remote_single
    u16 remote_single;
};
enum pgn4846_remote_single {
    // 单体蓄电池电压正常
    SINGLE_BATTERY_VOLTAGE_NORMAL = 0x00000000,
    // 单体蓄电池电压过高
    SINGLE_BATTERY_VOLTAGE_HIGH   = 0x00000001,
    // 单体学电池电压过低
    SINGLE_BATTERY_VOLTAGE_LOW    = 0x00000002,

    // 蓄电池组荷电状态正常
    BATTERY_SOC_NORMAL            = 0x00000000,
    // 蓄电池组荷电状态过高
    BATTERY_SOC_HIGH              = 0x00000004,
    // 蓄电池组荷电状态过低
    BATTERY_SOC_LOW               = 0x00000008,

    // 充电电流正常
    BATTERY_CHARGE_CURRENT_NORMAL = 0x00000000,
    // 充电电流过高
    BATTERY_CHARGE_CURRENT_HIGH   = 0x00000010,
    // 充电电流过低
    BATTERY_CHARGE_CURRENT_LOW    = 0x00000020,

    // 电池温度正常
    BATTERY_TEMPRATUEE_NORAML     = 0x00000000,
    // 电池温度过高
    BATTERY_TEMPRATURE_HIGH       = 0x00000040,
    // 电池温度过低
    BATTERY_TEMPRATURE_LOW        = 0x00000080,

    // 电池绝缘状态正常
    INSULATION_NORMAL             = 0x00000000,
    // 电池有绝缘故障
    INSULATION_FAULT              = 0x00000100,
    // 绝缘信号不可信
    INSULATION_UNRELIABLE         = 0x00000200,

    // 冲电连接器状态正常
    CONNECTOR_STATUS_NORMAL       = 0x00000000,
    // 充电连接器不正常
    CONNECTOR_STATUS_FAULT        = 0x00000400,
    // 冲电连接器状态不可信
    CONNECTOR_STATUS_UNRELIABLE   = 0x00000800,

    // 允许充电
    CHARGE_ALLOWED                = 0x00000000,
    // 禁止充电
    CHARGE_FORBIDEN               = 0x00001000
};

// 单体动力蓄电池电压
struct pgn5376_BMV {
    // bit[0:11]: 电压， 0.01V 每位, 0V偏移，范围 0- 24V
    // bit[12:15]: 蓄电池编号，1 / 每位， 范围 1-16
    u16 bat_v[256];
};

// 动力蓄电池温度
struct pgn5632_BMT {
    // 1 度每位， -50度偏移， 范围 -50 - 200
    u16 bat_tmp[128];
};

// 动力蓄电池预留报文
struct pgn5888_BSP {
    u16 reseved[16];
};

// BMS终止充电
struct pgn6400_BST {
    // BMS 中止充电原因 @ enum REASON_PGN6400
    u8 reason;
    // BMS 中止充电故障原因 enum ERROR_PGN6400
    u16 error;
    // BMS 中止充电错误原因 enum FAULT_PGN6400
    u16 fault;
};
enum REASON_PGN6400 {
    // 达到所需SOC值 bit[0:1]
    REASON_UN_REACH_SOC_VAL  = 0x00,
    REASON_REACH_SOC_VAL     = 0x01,
    REASON_SOC_UNRELIABLE        = 0x02,

    // 达到总电压设定值 bit[2:3]
    REASON_UN_REACH_VOL_VAL  = 0x00,
    REASON_REACH_VOL_VAL     = 0x04,
    REASON_VOL_UNRELIABLE        = 0x08,
    //                bit[4:5]
    REASON_UN_REACH_SINGLE_BAT_VOL = 0x00,
    REASON_REACH_SINGLE_BAT_VOL    = 0x10,
    REASON_SINGLE_VOL_UNRELIABLE   = 0x20
};
//enum ERROR_PGN6400 {};
//enum FAULT_PGN6400 {};

// 充电机终止充电
struct pgn6656 {

};

// 充电结束阶段
// BMS统计数据
struct pgn7168 {

};

// 充电机统计数据
struct pgn7424 {

};

// 错误报文分类
// BMS 错误报文
struct pgn7680 {

};

// 充电机错误报文
struct pgn7936 {

};

#pragma pack()

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

#include "Hachiko.h"
#include "charge.h"

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

// CAN 链接临时参数
struct can_tp_param {
    // 传输的数据包PGN
    unsigned int tp_pgn;
    // 即将传输的数据包大小
    unsigned int tp_size;
    // 即将传输的数据包个数
    unsigned int tp_pack_nr;

    // 已经接收的数据字节数
    unsigned int tp_rcv_bytes;
    // 已经接收的数据包个数
    unsigned int tp_rcv_pack_nr;
};

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


int about_packet_reciev_done(struct charge_task *thiz,
                             struct event_struct *param);
int gen_packet_PGN256(struct charge_task * thiz,
                       struct event_struct* param);
int gen_packet_PGN1792(struct charge_task * thiz,
                        struct event_struct* param);
int gen_packet_PGN2048(struct charge_task * thiz,
                        struct event_struct* param);
int gen_packet_PGN2560(struct charge_task * thiz,
                        struct event_struct* param);
int gen_packet_PGN4608(struct charge_task * thiz,
                        struct event_struct* param);
int gen_packet_PGN6656(struct charge_task * thiz,
                        struct event_struct* param);
int gen_packet_PGN7424(struct charge_task * thiz,
                        struct event_struct* param);
int gen_packet_PGN7936(struct charge_task * thiz,
                        struct event_struct* param);

#endif /*_BMS_PACKAGE_INCLUDED_H_*/
