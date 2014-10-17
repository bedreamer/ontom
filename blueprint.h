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
    BP_EVT_SWITVH_2_RX,

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

// 帧属性
typedef enum {
    // 数据帧特性不确定
    BP_FRAME_UNSTABLE = 0x00,
    // 发送的数据帧需要接收响应帧
    BP_FRAME_NEED_RX_ACK,
    // 接收的数据帧需要发送响应帧
    BP_FRAME_NEED_TX_ACK,
}BP_FRAME_ATTRIB;

// 串口事件参数
struct bp_evt_param {
    enum _unamed {
        const char *rx_buff;
        char *tx_buff;
    }buff;

    // 帧属性，仅在发送帧和接收到正确帧时有效
    BP_FRAME_ATTRIB attrib;
    // 缓冲区大小
    size_t buff_size;
    // 缓冲区载荷大小
    size_t payload_size;
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
    BP_UART_STAT status;
    // 初始化标识
    unsigned int init_magic;

    // 接收缓冲区
    char rx_buff[CONFIG_BP_IO_BUFF_SIZE];
    // 发送缓冲区
    char tx_buff[CONFIG_BP_IO_BUFF_SIZE];
};

#endif // _BLUE_PRINT_INCLUED_H_
