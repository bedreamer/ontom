/*
 * BMS 通信数据包定义
 * 版本 V 1.0
 * 遵循标准： GB/T 27930-2011
 */
#ifndef _BMS_PACKAGE_INCLUDED_H_
#define _BMS_PACKAGE_INCLUDED_H_

// 充电机地址
#define CAN_ADDR_CHARGER   0x56  // 86
// BMS地址
#define CAN_ADDR_BMS       0xF4  // 244
#define CAN_RCV_ID_MASK    (CAN_ADDR_CHARGER|CAN_ADDR_BMS)

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
    EVENT_TX_TP_REQUEST = 4,

    /* 数据包准备发送。当EVENT_TX_REQUEST返回结果是需要发送时，经发送线程
     * 经发送线程确认后，将会受到该消息，表示发送线程已经准备发送该消息了，此时
     * 可以返回取消发送指令，实现数据包的取消发送。
     */
    EVENT_TX_PRE     = 5,

    /* 数据包发送完成。当确认后的数据包发送完成后，将会受到该消息，表征数据包
     * 已经正确的发送完成。
     */
    EVENT_TX_DONE    = 6,

    /* 数据包发送失败。当确认后的数据包发送失败后，将会受到改小。
     */
    EVENT_TX_FAILS   = 7,

    // CAN 消息函数初始化。当第一次运行函数时将会收到该消息，可重复发送。
    EVENT_CAN_INIT   = 8,

    // CAN 消息复位。再次执行初始化操作。
    EVENT_CAN_RESET  = 9
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
    // 缓冲区载荷大小
    unsigned int buff_payload;
};

// SAE J1939-21 连接管理协议结构
union SAE_J1939_21_TP {
    struct {
        // always equal 16/10h/0x10
        u8 control_byte;
        // 参数组共计多少字节
        u16 total_size;
        // 参数组共计多少个数据包
        u8 total_packets;
        // 数据包中最大的包编号
        u8 max_sn_packet;
        // 请求的参数组编号
        u8 pgn_hi, pgn_mid, pgn_lo;
    }TP_CM_RTS;
    struct {
        u8 control_byte;
    }TP_CM_CTS;
    struct {
        u8 control_byte;

    }TP_CM_EndOfMsgACK;
    struct {
        u8 control_byte;

    }TP_Conn_Abort;
    struct {
        u8 control_byte;
        u8 noused[7];
    }TP_CM_BAM;
};

// 握手阶段
// 充电机辨识报文
struct pgn256_CRM {
    // 充电机辨识结果
    u8 spn2560_recognize;
    #define BMS_RECOGNIZED       0xAA // 充电机能识别BMS
    #define BMS_NOT_RECOGNIZED   0x00 // 充电机不能识别BMS

    // 充电机编号， 范围1-100
    u8 spn2561_charger_sn;
    // 充电机/充电站所在区域编码
    unsigned char spn2562_charger_region_code[6];
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
    // 电池组产权标识
    u8 spn2573_battery_property;
    #define BATTERY_LEASE   0x00 // 租赁
    #define BATTERY_OWNER   0x01 // 私有
    u8 spn2574_reserved; // 保留
    // 车辆识别码
    unsigned char spn2575_vin[17];
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
    u16 spn2819_max_chage_voltage;
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
    unsigned char spn2823_date[7];
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
    // BMS 充电准备完成
    u8 spn2829_bms_ready_for_charge;
    #define BMS_NOT_READY_FOR_CHARGE  0x00 // 没有准备好
    #define BMS_READY_FOR_CHARGE      0xAA // 已准备好
    #define BMS_INVALID               0xFF // 无效
};

// 充电机输出准备就绪状态
struct pgn2560_CRO {
    // 充电机充电准备完成
    u8 spn2830_charger_ready_for_charge;
    #define CHARGER_NOT_READY_FOR_CHARGE  0x00 // 没有准备好
    #define CHARGER_READY_FOR_CHARGE      0xAA // 已准备好
    #define CHARGER_INVALID               0xFF // 无效
};

// 充电阶段
// 电池充电需求
struct pgn4096_BCL {
    // 充电电压需求，0.1V 每位，0-750V
    u16 spn3072_need_voltage;
    // 充电电流需求，0.1A 每位，-400V偏移，-400A-0A
    u16 spn3073_need_current;
    // 充电模式
    u16 spn3074_charge_mode;
    #define CHARGE_WITH_CONST_VOLTAGE  0x01  // 恒流充电
    #define CHARGE_WITH_CONST_CURRENT  0x02  // 恒压充电
};

// 电池充电总状态
struct pgn4352 {
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
struct pgn4608 {

};

// 动力蓄电池状态信息
struct pgn4864 {

};

// 单体动力蓄电池电压
struct pgn5376 {

};

// 动力蓄电池温度
struct pgn5632 {

};

// 动力蓄电池预留报文
struct pgn5888 {

};

// BMS终止充电
struct pgn6400 {

};

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

#endif /*_BMS_PACKAGE_INCLUDED_H_*/
