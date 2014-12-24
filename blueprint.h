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

#include "Hachiko.h"

struct bp_uart;
#define CONFIG_BP_IO_BUFF_SIZE   512

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
        char *rx_buff;
        char *tx_buff;
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

    int (*user_evt_handle)(struct bp_uart *self, BP_UART_EVENT evt,
                           struct bp_evt_param *param);
};

/*
 * 串口描述结构
 */
struct bp_uart {
    // 串口名，/dev/uart1, etc
    const char *dev_name;
    // 文件描述序号
    int dev_handle;
    // 串口状态
    volatile BP_UART_STAT status;
    // 硬件状态,仅针对RS485有效
    volatile BP_UART_STAT hw_status;

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
    char rx_buff[CONFIG_BP_IO_BUFF_SIZE];
    // 发送参数
    struct bp_evt_param tx_param;
    // 发送缓冲区
    char tx_buff[CONFIG_BP_IO_BUFF_SIZE];

    // 发送序列
    unsigned int sequce;
    // 连续发送次数
    unsigned int continues_nr;
    // 使用者信息
    struct bp_user *users;
    // 当前使用者
    struct bp_user *master;
};

#endif // _BLUE_PRINT_INCLUED_H_
