#ifndef _Z2B_INCLUDED_H_
#define _Z2B_INCLUDED_H_

// 消息传递头
struct pack_header {
    // 包ID， 发送接收独立计数
    pack_t pid;
    // 数据包时戳
    time_t tid;

    // 数据包个数
    size_t pack_nr;
    // 字段个数
    size_t section_nr;

    // 数据包体大小，byte
    size_t pack_size;
};

// 数据包传输控制结构, 有效载荷比 96.8%
#define TP_BUFF_SIZE  512
#define TP_PAYLOAD_SIZE 496
#pragma pack(1)
struct pack_tp_struct {
    // 数据包ID, 4 bytes
    unit_t pid;
    // 数据包字段偏移, 2bytes
    short offset;
    // 这个数据包的大小(0-496), 2bytes
    short size;
    // 数据包更新时戳, 4bytes
    time_t timestamp;
    // 填充字段, 4bytes, 用于串接数据包
    unsigned int notused;

    union {
        char buff[TP_PAYLOAD_SIZE];
    }load;
};
#pragma pack()

// 包ID
enum {
    // 正在执行的作业
    PID_WORKING_JOB_01 =  0x80000001,
    PID_WORKING_JOB_02 =  0x80000002,
    // 正在等待的作业
    PID_PEDDING_JOB_01 =  0x80000003,
    PID_PEDDING_JOB_02 =  0x80000004,
    PID_PEDDING_JOB_03 =  0x80000005,
    PID_PEDDING_JOB_04 =  0x80000006,
    PID_PEDDING_JOB_05 =  0x80000007,
    PID_PEDDING_JOB_06 =  0x80000008,
    PID_PEDDING_JOB_07 =  0x80000009,
    PID_PEDDING_JOB_08 =  0x8000000A,
    PID_PEDDING_JOB_09 =  0x8000000B,
    PID_PEDDING_JOB_10 =  0x8000000C,
    PID_PEDDING_JOB_11 =  0x8000000D,
    PID_PEDDING_JOB_12 =  0x8000000E,
    PID_PEDDING_JOB_13 =  0x8000000F,
    PID_PEDDING_JOB_14 =  0x80000010,
    PID_PEDDING_JOB_15 =  0x80000011,
    PID_PEDDING_JOB_16 =  0x80000012,
    // 充电桩信息
    PID_CHARGER_INFO   =  0x80000013,

    // 充电桩控制
    PID_CHARGER_CTRL   =  0x80000014,
    // 参数配置
    PID_PARAM_CONFIG   =  0x80000015,
    // 作业控制
    PID_JOB_CTRL       =  0x80000016,
    // 连接控制
    PID_CONN_CTRL      =  0x80000017
}PID;

// 会话类型
typedef enum {
    // 未确定的会话
    SESSION_UNDEFINED,
    // 正常业务通讯
    SESSION_NORMAL,
    // 后台管理通讯会话
    SESSION_TROY
}SESSIONTYPE;

// 会话 输入输出缓冲区
#define BUFF_SIZE   8 * 1024
#define BUFF_POSITION_MASK  0x03FF
#define position_index(bin) ((bin)&BUFF_POSITION_MASK)
struct session_buff {
    unsigned short io_cursor;
    int buff_size;
    char buff[8 * 1024];
    pthread_mutex_t buff_rw_lck;
};

// 和 box 通讯会话
struct session_struct {
    // 请求开始时间戳, 超过60秒还未进行链接请求则关闭连接
    time_t connected_tstp;
    // 建立链接的时间戳
    time_t establish_tstp;
    // 最后一次接收到数据包的时戳
    time_t last_pack_tstp;
    // 会话ID
    size_t session_id;
    // 会话句柄
    int socket_handle;
    // 会话类型
    SESSIONTYPE stype;

    // 数据帧头接收完成
    int frame_head_rx_ok;
    // 下一个数据包读取的数据大小
    size_t next_read_size;

    struct session_buff session_rx_buff;
    struct session_buff session_tx_buff;

    // 会话节点
    struct list_head nd;
};

#endif // _Z2B_INCLUDED_H_
