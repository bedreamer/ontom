#ifndef INCLUDED_104_H
#define INCLUDED_104_H

typedef enum {
    P104_BEGIN = 0,

    // 已经登录
    P104_LOGINED,
    // 登录超时
    P104_LOGIN_TIMEOUT,

    P104_END = 63
}P104SINGLE;

typedef enum {
    // 确认 / 否认
    AFN_CONFIRM_DENNED = 0x00,
    // 链路检测
    AFN_LINK_TEST = 0x01,
    AFN_PARAM_SETTING = 0x02,
    AFN_CTRL_CMD = 0x03,
    AFN_QUERY_CMD = 0x04,
    AFN_EXP_POST = 0x05,
    AFN_CARD_OP = 0x06,
    AFN_TERMINAL_QUERY = 0x07,
    AFN_CSD_POST = 0x08,
    AFN_MSG_CHARGE = 0x09,
    AFX_UPGRADE = 0x0A
}AFN;

#define RX_TX_NR   512

// 104 协议控制面板
struct p104_control_panel {
    // 主站套接字
    int master_socket;
    // 主站地址信息
    struct sockaddr_in master_addr;

    // 登录超时定时器
    // 建立socket 连接后若在该定时器结束前未登录成功则关闭链接
    struct Hachiko_food login_seed;

    // 地区编码 0571 BCD 码
    unsigned short section_code;
    // 本机地址 1
    unsigned short slave_addr;
    // 主站及组地址
    unsigned char master_group_addr;

    char rx_buff[RX_TX_NR];
    char tx_buff[RX_TX_NR];

    unsigned char single[8];
};

#pragma pack(1)
// 链路控制结构
struct p104_link_controller {
    unsigned char s_68h;
    unsigned short length;
    unsigned char s_68hr;
    unsigned char ctrl_seg;

    unsigned short section_code_bcd;
    unsigned short terminal_addr;
    unsigned char master_group_addr;

    unsigned char app_buff[256];
};
#pragma pack()

#endif // INCLUDED_104_H
