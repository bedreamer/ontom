/*
 * 充电控制描述结构
 * All rights reserved Hangzhou Power Equipment Co., Ltd.
 * author: LiJie
 * email:  bedreamer@163.com
 * 2014-08-22
*/

#ifndef _CHARGE_INCLUDED_H_
#define _CHARGE_INCLUDED_H_

struct charge_task;
extern struct charge_task *task;
struct MDATA_ACK;
struct bp_uart;
#include <pthread.h>
#include "list.h"
#include "Hachiko.h"
#include "bms.h"
#include "config.h"

#include "../thirdpart/sqlite/sqlite3.h"
// 无效时戳，初始化时默认赋值， 用于time_t默认值
#define INVALID_TIMESTAMP  0x00000000
// BMS 通信时的缓冲区
#define CAN_BUFF_SIZE         1024
#define BUFF_NR               CONFIG_SUPPORT_BMS_NR
typedef enum {
    // 0# 充电枪
    GUN_SN0 = 0,
    // 1# 充电枪
    GUN_SN1 = 1,
    // 2# 充电枪
    GUN_SN2 = 2,
    // 3# 充电枪
    GUN_SN3 = 3,
    // 默认充电枪
    GUN_DEFAULT = 0,
    // 无效枪
    GUN_INVALID = 344,
    // 未指定充电枪
    GUN_UNDEFINE = 177
}CHARGE_GUN_SN;


#pragma pack(1)
/* 卡信息
 */
struct user_card {
    // 触发任务时的卡号
    char triger_card_sn[64];
    struct {
        unsigned char id[16];
        unsigned char sector_num;
        union {
            unsigned char buff[16];
            struct {
                unsigned int magic;           /*奥能魔数 'ONPW', Only-Power 0x4F4E5057 */
                unsigned char remain_money[3]; /*余额， 范围 0.00 ~ 167772.15 */
                unsigned char remain_sum;    /*余额校验位 */
                unsigned char passwd_code[3];  /*6 位BCD 码密码*/
                unsigned char passwd_sum;    /*密码校验和 */
                unsigned short reserved;
                unsigned char unuesed;
                unsigned char sum;            /*校验和 */
            }data;
        }sector_4;
    }card;
};
static inline double __card_read_remain(const struct user_card *c) {
    unsigned int imoney = 0;

    imoney += c->card.sector_4.data.remain_money[0];
    imoney += c->card.sector_4.data.remain_money[1] * 256;
    imoney += c->card.sector_4.data.remain_money[2] * 256 * 256;

    return imoney / 100.0f;
}

static inline double __card_write_remain(struct user_card * c, double money) {
    unsigned int imoney = money * 100;
    c->card.sector_4.data.remain_money[0] = imoney & 0xFF;
    c->card.sector_4.data.remain_money[1] = (imoney >> 8) & 0xFF;
    c->card.sector_4.data.remain_money[2] = (imoney >> 16) & 0xFF;
    c->card.sector_4.data.remain_sum =
            check_sum(c->card.sector_4.data.remain_money, 3);
    return money;
}

static inline void __card_read_passwd(const struct user_card *c, char *passwd) {
    sprintf(passwd, "%d%d%d%d%d%d",
            c->card.sector_4.data.passwd_code[2]>>4,
            c->card.sector_4.data.passwd_code[2]&0xF,
            c->card.sector_4.data.passwd_code[1]>>4,
            c->card.sector_4.data.passwd_code[1]&0xF,
            c->card.sector_4.data.passwd_code[0]>>4,
            c->card.sector_4.data.passwd_code[0]&0xF);
}

static inline unsigned char __chars2bcd(char h, char l) {
    if ( h > '9' || h <'0' ) h = '0';
    if ( l > '9' || l <'0' ) l = '0';
    return (((h - '0')&0xF)<<4)|((l-'0')&0x0F);
}

static inline void __card_write_passwd(struct user_card * c, const char* passwd) {
    c->card.sector_4.data.passwd_code[2] = __chars2bcd(passwd[0], passwd[1]);
    c->card.sector_4.data.passwd_code[1] = __chars2bcd(passwd[2], passwd[3]);
    c->card.sector_4.data.passwd_code[0] = __chars2bcd(passwd[4], passwd[5]);
    c->card.sector_4.data.passwd_sum =
            check_sum(c->card.sector_4.data.passwd_code, 3);
}

typedef enum {
    // 寻卡模式
    SEQ_FIND_CARD = 0,
    // 读扇区密码认证
    SEQ_SECTOR_RD_AUTH,
    // 写扇区密码认证
    SEQ_SECTOR_WR_AUTH,
    // 读取公开数据区
    SEQ_READ_PUBLIC_BLK,
    // 写入公开数据区
    SEQ_WRITE_PUBLIC_BLK,
    // 读取私密数据区1
    SEQ_READ_PRIVATE_BLK1,
    // 读取私密数据区2
    SEQ_READ_PRIVATE_BLK2,
    // 读取私密数据区3
    SEQ_READ_PRIVATE_BLK3,
    // 读取私密数据区4
    SEQ_READ_PRIVATE_BLK4,
    // 读取私密数据区5
    SEQ_READ_PRIVATE_BLK5,
    // 读取私密数据区6
    SEQ_READ_PRIVATE_BLK6,
    // 读取私密数据区7
    SEQ_READ_PRIVATE_BLK7,
    SEQ_INVALID
}QUERY_STAT;

/*
 * 综合采样盒通信数据定义
 */
struct MDATA_ACK {
#if 0
    // 数据包起始魔数
    unsigned char magic[5];
    // 地址
    unsigned char addr;
    // 载荷长度
    unsigned char len;

    // 母线电压
    unsigned short V_mx;
    // 电池电压
    unsigned short V_bat;
    // 电池电流
    unsigned short I_bat;
    // 温度
    unsigned short temp;
    // 湿度
    unsigned short wet;
    // 保留字
    unsigned short resv[2];
    // 采样盒软件版本号
    unsigned char  version;

    // 母线电压过高故障，0： 正常，1：过高
    unsigned char yx_mx_V_high:1;
    // 母线欠压故障，0: 正常，1：欠压
    unsigned char yx_mx_V_low:1;
    // 母线短路故障
    unsigned char yx_mx_short_fault:1;
    // 电池电压过高
    unsigned char yx_bat_V_high:1;
    // 电池电压过低
    unsigned char yx_bat_V_low:1;
    // 电池链接短路
    unsigned char yx_bat_short_fault:1;
    // 电池反接
    unsigned char yx_bat_revers_conn:1;
    // 电池电流过高
    unsigned char yx_bat_I_high:1;

    // 电池绝缘故障
    unsigned char yx_bat_institude_fault:1;
    // 辅助电源状态
    unsigned char yx_assit_power_stat:1;
    // 温度状态，00: 正常，01： 过高 10： 过低
    unsigned char yx_temprature:2;
    // 湿度状态，00： 正常，01：过湿, 10: 过干
    unsigned char yx_wet_rate:2;
    unsigned char yx_prtc2_rsv:2;

    unsigned char yx_prtc3_rsv;

    // 总输出熔断器故障，0：无故障，1：熔断
    unsigned char yx_rdq:1;
    // 总直流输出跳闸, 0: 正常， 1： 跳闸
    unsigned char yx_dc_output_tiaozha:1;
    // 一路输出跳闸，0：正常，1：跳闸
    unsigned char yx_dc_output_tiaozha1:1;
    // 二路输出跳闸，0：正常，1：跳闸
    unsigned char yx_dc_output_tiaozha2:1;
    // 防雷器故障，0：无故障，1：防雷器故障
    unsigned char yx_flq:1;
    unsigned char yx_prtc4_rsv:3;

    unsigned char yx_prtc5_rsv;

    // 交流输入合闸状态，0：分闸，1：合闸
    unsigned char yx_ac_hezha:1;
    // 加热状态，0：未加热，1：加热
    unsigned char yx_heater_stat:1;
    // 风扇状态，0：未启动，1：启动
    unsigned char yx_fan_stat:1;
    // 直流输入总开关合闸，0：分闸，1：合闸
    unsigned char yx_dc_output_hz:1;
    // 1#充电枪输出合闸状态，0：未合闸，1：合闸
    unsigned char yx_gun_1_hezha_stat:1;
    // 1#充电枪连接状态，00：未连接，11：链接,01:连接保护，10：连接异常
    // @ GUN_CONN_STAT
    unsigned char yx_gun_1_conn_stat:2;
    // 1#充电枪辅助电源合闸状态，0：未合闸，1：合闸
    unsigned char yx_gun_1_assit_power_hezha:1;

    // 2#充电枪输出合闸状态，0：未合闸，1：合闸
    unsigned char yx_gun_2_hezha_stat:1;
    // 2#充电枪连接状态，00：未连接，11：链接,01:连接保护，10：连接异常
    // @ GUN_CONN_STAT
    unsigned char yx_gun_2_conn_stat:2;
    // 2#充电枪辅助电源合闸状态，0：未合闸，1：合闸
    unsigned char yx_gun_2_assit_power_hezha:1;
    unsigned char yx_run2_rsv:4;

    unsigned char yx_run3_rsv;
    unsigned char unused;

    unsigned short crc;
#else
    // 数据包起始魔数
    unsigned char magic[4];
    // 地址
    unsigned char addr;
    // 功能吗
    unsigned char op;
    // 载荷长度
    unsigned char len;

    unsigned short VinKM0; //母线0电压 实际电压*10
    unsigned short VinKM1;//母线1电压 实际电压*10
    unsigned short VinBAT0;//电池0组电压，实际电压*10
    unsigned short VinBAT1;//电池1组电压，实际电压*10
    unsigned short IoutBAT0; //电池0组电流，实际电流*10
    unsigned short IoutBAT1; //电池1组电流，实际电流*10
    unsigned short VinBAT0RESP;//电池0组正对地电阻，实际电阻*10（K）
    unsigned short VinBAT1RESP;//电池1组正对地电阻，实际电阻*10（K）
    unsigned short VinBAT0RESN;//电池0组负对地电阻，实际电阻*10（K）
    unsigned short VinBAT1RESN;//电池1组负对地电阻，实际电阻*10（K）
    unsigned short Temp ;//当前温度，实际温度*10
    unsigned short Humi; //当前湿度，实际湿度*10
    unsigned short Reserve2;// 预留
    unsigned short Reserve3; //预留
    unsigned char Ver; //软件版本号， 如10 表示 Ver= 1.0;  数据一共29字节


    unsigned char Flag_prtc1;   //充电断开类保护状态字节1：（同步状态）
    //BIT0：母线0过压状态位；0 = 无故障，1 = 母线过压；
    //BIT1：母线0欠压状态位；0 = 无故障，1 = 母线欠压；
    //BIT2：母线0短路状态位；0 = 无故障，1 = 母线短路；
    //BIT3：母线1过压状态位；0 = 无故障，1 = 母线过压；
    //BIT4：母线1欠压状态位；0 = 无故障，1 = 母线欠压；
    //BIT5：母线1短路状态位；0 = 无故障，1 = 母线短路；
    //剩余bit保留

    unsigned char Flag_prtc2;   //充电断开类保护状态字节2：（同步状态）
    //BIT0：电池0组过压状态位；0 = 无故障，1 = 电池过压；
    //BIT1：电池0组欠压状态位：0 = 无故障，1 = 电池欠压；
    //BIT2：电池0组短路状态位：0 = 无故障，1 = 电池短路；
    //BIT3：电池0组反接状态位；0 = 无故障，1 = 电池反接；
    //BIT4：电池0组绝缘状态位；0 = 无故障，1 = 电池绝缘故障；
    //BIT5：电池0组过流状态位；0 = 无故障，1 = 电池过流；
    //剩余bit保留

    unsigned char Flag_prtc3;   //充电断开类保护状态字节3：（同步状态）
    //BIT0：电池1组过压状态位；0 = 无故障，1 = 电池过压；
    //BIT1：电池1组欠压状态位：0 = 无故障，1 = 电池欠压；
    //BIT2：电池1组短路状态位：0 = 无故障，1 = 电池短路；
    //BIT3：电池1组反接状态位；0 = 无故障，1 = 电池反接；
    //BIT4：电池1组绝缘状态位；0 = 无故障，1 = 电池绝缘故障；
    //BIT5：电池1组过流状态位；0 = 无故障，1 = 电池过流；
    //剩余bit保留

    unsigned char Flag_prtc4;   //充电断开类保护状态字节4：（同步状态）
    //BIT0：辅助电源状态位；0 = 无故障，1 = 辅助电源异常；
    //BIT1..2：温度状态位；0 0= 无故障，01 = 温度过高，10= 温度过低
    //BIT3..4：湿度状态位；0 0= 无故障，01 = 湿度过大，10=湿度过低
    //剩余bit保留

    unsigned char Flag_prtc5;   //充电断开类保护状态字节5：保留（同步状态）

    unsigned char Flag_prtc6;   //充电断开类保护状态字节4：（异步状态）
    //BIT0：总直流输出熔断器状态位：0 = 无故障，1 = 熔断；
    //BIT1：总直流输出跳闸状态位：0 = 无故障，1 = 跳闸；
    //BIT2：充电1路输出跳闸状态位；0 = 无故障，1 = 跳闸；
    //BIT3：充电2路输出跳闸状态位；0 = 无故障，1 = 跳闸；
    //BIT4：防雷器状态位；0 = 无故障，1 = 防雷器故障；
    //剩余bit保留

    unsigned char Flag_prtc7;   //充电断开类保护状态字节5：（异步状态）保留

    unsigned char Flag_run1;   //当前运行状态字节1：
    //BIT 0: 交流输入是否合闸： 0 = 分；1 = 合； //一般情况常1
    //BIT 1：加热状态：    0 = 未加热；1 = 加热；
    //BIT 2：风扇状态：  0 = 未启动； 1 = 启动；
    //BIT 3：总直流输出是否合闸：0 =分；1 =合；
    //BIT 4：充电枪1合闸状态： 0 = 分； 1 = 合；
    //BIT 5..6：充电枪1连接状态：00= 未连接； 1 1= 连接；01=连接保护状态；10= 连接异常
    //BIT 7：充电枪1通信辅助电源合闸状态： 0 = 分； 1 = 合；

    unsigned char Flag_run2;   //当前运行状态字节2：
    //BIT 0：充电枪2合闸状态：0 = 分； 1 = 合；
    //BIT 1..2：充电枪2连接状态：00 = 未连接； 11 = 连接；01=连接保护状态；10= 连接异常
    //BIT 3：充电枪2通信辅助电源合闸状态： 0 = 分； 1 = 合；

    unsigned char Flag_run3;   //当前运行状态字节3：保留 状态一共10字节
    unsigned char unused;

    unsigned short crc;
#endif
};
typedef enum {
    // 分离的，未连接
    GUN_CONN_DETACHED = 0,
    // 连接保护
    GUN_CONN_PROTECTED = 1,
    // 连接异常
    GUN_CONN_EXCEPTION = 2,
    // 已连接
    GUN_CONN_CONNECTIVE=3
}GUN_CONN_STAT;

// 下发数据包
struct MDATA_QRY {
    // 数据包起始魔数
    unsigned char magic[5];
    // 地址
    unsigned char addr;
    // 载荷长度
    unsigned char len;
#if 1
    // 直流总开关输出闭合，0：断开，1：闭合
    unsigned char dc_output_hezha:1;
    // 1#充电枪辅助电源上电，0：断电，1：上电
    unsigned char gun_1_assit_power_on:1;
    // 1#充电枪输出开关合闸，0：分闸，1：合闸
    unsigned char gun_1_output_hezha:1;
    // 2#充电枪辅助电源上电，0：断电，1：上电
    unsigned char gun_2_assit_power_on:1;
    // 2#充电枪输出开关合闸，0：分闸，1：合闸
    unsigned char gun_2_output_hezha:1;
    unsigned char cmd_rsv:3;
#else
    unsigned char cmd_rsv;
#endif
    unsigned char cmd_copy;
    unsigned char rsvs[14];

    unsigned short crc;
};
#define DC_SWITCH_ON        0x01
#define GUN1_ASSIT_PWN_ON   0x02
#define GUN1_OUTPUT_ON      0x04
#define GUN2_ASSIT_PWN_ON   0x08
#define GUN2_OUTPUT_ON      0x10
#pragma pack()

// 充电屏数据读写数据结构
struct charger_config_03h_04h_10h {

};

// 充电屏数据结构
struct charger_config_10h {
    // {{ 充电屏监控返回数据
    // 充电屏编号
    unsigned short charger_sn;
    // 充电机组充电状态
    unsigned short charger_status;
    // 充电及自身状态
    unsigned short charger_self_status;
    // 充电机组最高输出电压
    unsigned short charger_max_v_out;
    // 充电机组最低输出电压
    unsigned short charger_min_v_out;
    // 充电机组最大输出电流
    unsigned short charger_max_i_out;
    // 充电机组实际输出电压值
    unsigned short charger_v_out;
    // 充电机组实际输出电流值
    unsigned short charger_i_out;

    // 充电机模块输出电压
    unsigned short charge_module_v[CONFIG_SUPPORT_CHARGE_MODULE];
    // 充电机模块输出电流
    unsigned short charge_module_i[CONFIG_SUPPORT_CHARGE_MODULE];
    // 充电机模块温度
    unsigned short charge_module_t[CONFIG_SUPPORT_CHARGE_MODULE];
    // 充电机模块状态
    unsigned short charge_module_status[CONFIG_SUPPORT_CHARGE_MODULE/2];
    // 充电机模块机身编号
    unsigned short charge_module_sn[CONFIG_SUPPORT_CHARGE_MODULE][3];
    unsigned short charge_kaichu;
    unsigned short charge_kairu;
    // }}
};

// 充电计费模式
typedef enum {
    // 无效充电模式，默认值
    BILLING_MODE_INVALID    =0x00,
    // 自动充电
    BILLING_MODE_AS_AUTO    =0x01,
    // 按金额充电
    BILLING_MODE_AS_MONEY   =0x02,
    // 按时间充电
    BILLING_MODE_AS_TIME    =0x03,
    // 按容量充电
    BILLING_MODE_AS_CAP     =0x04,
    // 自由充电，不需要刷卡
    BILLING_MODE_AS_FREE    =0x05
}BILLING_MODE;

// 充电任务 状态
typedef enum {
    // 无效任务状态
    CHARGE_STAT_INVALID             =0x00,
    // 空闲等待,界面停止于充电模式选择界面
    CHARGE_STAT_TRIGER_PEDDING      =0x01,
    // 刷卡准备, 于自动模式下已经刷卡，或者手动选择了充电计费模式
    //CHARGE_STAT_PREPARE      =0x02,
    // 充电确认, 刷卡完成，进行充电确认等待
    CHARGE_STAT_CONFIRM_PEDDING     =0x03,
    // BMS 连接等待, 已经确认充电
    CHARGE_STAT_WAIT_BMS            =0x04,
    // BMS 已经连接，我收拾别完成，进行充电参数配置
    CHARGE_STAT_READY               =0x05,
    // 充电阶段
    CHARGE_STAT_CHARGING            =0x06,
    // 充电结束阶段, 断开接触器
    CHARGE_STAT_DONE                =0x07,
    // 计费阶段, 等待刷卡付费阶段
    CHARGE_STAT_BILLING_PEDDING     =0x08,
    // 充电任务退出阶段，日志记录，清理
    CHARGE_STAT_EXIT                =0x09,
    // 充电任务被终止
    CHARGE_STAT_ABORT               =0x0A,
    // 充电异常退出
    CHARGE_STAT_EXCEPTION           =0xFF
}CHARGE_TASK_STAT;

// 充电阶段定义
typedef enum {
    // 充电状态无效，可能是在初始化或者析构
    CHARGE_STAGE_INVALID      =0x00,
    // 充电握手
    CHARGE_STAGE_HANDSHACKING =0x01,
    // 充电配置
    CHARGE_STAGE_CONFIGURE    =0x02,
    // 充电
    CHARGE_STAGE_CHARGING     =0x03,
    // 充电结束
    CHARGE_STAGE_DONE         =0x04,
    // 任意阶段
    CHARGE_STAGE_ANY          =0x05
}CHARGE_STAGE;

// 充电阶段变更事件
typedef enum {
    // 充电状态变为无效状态
    STAGE_CHANGE_2_INVALID     = 0x00,
    // 充电状态变为握手阶段
    STAGE_CHANGE_2_HANDSHAKING = 0x01,
    // 充电状态变为配置阶段
    STAGE_CHANGE_2_CONFIGURE   = 0x02,
    // 充电阶段变为充电阶段
    STAGE_CHANGE_2_CHARGING    = 0x03,
    // 充电阶段变为充电完成阶段
    STAGE_CHANGE_2_DONE        = 0x04,
    // 充电阶段变为充电正常退出阶段
    STAGE_CHANGE_2_EXIT        = 0x05,
    // 充电阶段变为中止充电推出阶段
    STAGE_CHANGE_2_ABORT       = 0x06
}CHARGE_STAGE_CHANGE_EVENT;
// 充电阶段改变事件处理过程
void on_charge_stage_change(CHARGE_STAGE_CHANGE_EVENT evt,
                            CHARGE_STAGE pre,
                            struct charge_task *thiz);

// BMS CAN通信状态定义
typedef enum {
    // 无效模式
    CAN_INVALID               =0x00,
    // 普通模式
    CAN_NORMAL                =0x01,
    // 连接管理读模式, 多数据包读
    CAN_TP_RD                 =0x02,
    // 连接管理写模式，多数据包写，当前的协议标准没有用到
    CAN_TP_WR                 =0x04
}CAN_BMS_STATUS;

// CAN 传输链接状态
typedef enum {
    // 链接请求发送
    CAN_TP_RTS = 0x10,
    // 链接准备发送
    CAN_TP_CTS = 0x20,
    // 数据发送中
    CAN_TP_TX  = 0x30,
    // 数据接收中
    CAN_TP_RX  = 0x40,
    // 链接发送完成
    CAN_TP_ACK = 0x50,
    // 链接发送中止
    CAN_TP_ABRT= 0x60
}CAN_TP_STATUS;

// 通信报文生成依据
struct can_pack_generator {
    // 所属阶段
    CHARGE_STAGE stage;
    // 生成PGN
    unsigned int can_pgn;
    // 数据包优先级
    unsigned int prioriy;
    // 数据包长度
    unsigned int datalen;
    // 数据包发送周期
    unsigned int period;
    // 心跳计数
    unsigned int heartbeat;

    // 数据包静默时间，只针对接受数据包有效
    unsigned int can_silence;
    // 接受数据包容忍的沉默时常，针对接收数据有效
    unsigned int can_tolerate_silence;
    // 计数器，接收或发送的次数
    unsigned int can_counter;
};

// 充电作业状态
typedef enum {
    // 作业状态为空
    JOB_IDLE = 0x00,
    // 作业状态正在设置
    JOB_SETTING,
    // 正在等待前驱作业完成
    JOB_WAITTING,
    // 作业就绪等待
    JOB_STANDBY,

    // 作业正在执行
    JOB_WORKING,
    // 作业因故暂停
    JOB_ERR_PAUSE,
    // 人工暂停作业
    JOB_MAN_PAUSE,
    // 作业从暂停状态恢复
    JOB_RESUMING,
    // 作业中止
    JOB_ABORTING,
    // 作业完成
    JOB_DONE,
    // 作业退出，正在进行清理工作
    JOB_EXITTING,

    // 作业销毁，等待其他线程引用退出
    JOB_DETACHING
}JOBSTATUS;

// 充电计费方式
struct billing_methord {
    // 计费方式
    BILLING_MODE mode;
    union {
        // 设定充电金额, 0 - 9999
        double set_money;
        // 设定充电时长, 0 - 600
        unsigned int set_time;
        // 设定充电目标电量, 0 - 100
        double set_kwh;
    }option;
};

// 通信管理单元状态
typedef enum {
    // 管理但愿正在初始化
    COMM_M_INIT = 0,
    // 管理单元正在执行
    COMM_M_WORKING = 1,
    // 管理单元正在停止
    COMM_M_STOPPING = 2,
    // 管理单元已经终止
    COMM_M_DIED = 3
}COMM_M_STRUCT_STATUS;

typedef enum {
    // 无效
    PRIVATE_INVALID = 0x00,
    // 已经使用
    PRIVATE_BUSY    = 0x01,
    // 闲置等待
    PRIVATE_STANDBY = 0x02
}PRIVATE_STATUS;

// BMS 通讯管理描述结构，JOB的下属成员结构
struct bms_struct {
    COMM_M_STRUCT_STATUS status;
    // 充电作业所处阶段,BMS 链接阶段
    CHARGE_STAGE charge_stage;

    struct charge_job *job;
    // BMS通讯设备名, "can1|can2"
    char *can_dev;
    // BMS写初始化完成
    int bms_write_init_ok;
    // BMS读初始化完成
    int bms_read_init_ok;
    // CAN BMS 通信所处状态
    CAN_BMS_STATUS can_bms_status;
    // 连接管理的传输控制参数
    struct can_tp_param can_tp_param;
    // 连接管理超时控制器
    struct Hachiko_food can_tp_bomb;
    // 连接超时参数组状态
    PRIVATE_STATUS can_tp_private_status;
    // CAN数据包心跳时钟
    struct Hachiko_food can_heart_beat;

    // 车辆基本信息
    struct pgn512_BRM  vehicle_info;
    // BMS充电配置信息
    struct pgn1536_BCP bms_config_info;
    // BMS当前充电需求信息
    struct pgn4096_BCL bms_charge_need_now;
    // BMS 电池充电总状态信息
    struct pgn4352_BCS bms_all_battery_status;
    // BMS 动力蓄电池状态信息
    struct pgn4864_BSM bms_battery_status;

    // 数据库操作计数器
    unsigned int readed;
    unsigned int can_pack_gen_nr;
    struct can_pack_generator *generator;
};

// 采样盒 通讯管理描述结构，JOB的下属成员结构
struct measure_struct {
    COMM_M_STRUCT_STATUS status;
    // 扩展测量值
    struct MDATA_ACK measure;
    // 前一次测量拷贝值
    struct MDATA_ACK measure_pre_copy;
    // 前一次读取扩展测量得到的时间戳, 通过对比时间戳来确定扩展测量是否已经更新了数据
    time_t pre_stamp_ex_measure;
};

// 充电机 通讯管理描述结构，JOB的下属成员结构
struct charger_struct {
    COMM_M_STRUCT_STATUS status;
    // 充电屏信息
    struct charger_config_10h chargers;
};

// 电表数据
struct meter_data {
    // A 相电压
    double Va;
    // B 相电压
    double Vb;
    // C 相电压
    double Vc;

    // 正向总电能
    double kwh_zong;
    // 尖电能
    double kwh_jian;
    // 峰电能
    double kwh_feng;
    // 平电能
    double kwh_ping;
    // 谷电能
    double kwh_gu;
};

// 提交命令
typedef enum {
    // 创建任务
    COMMIT_CMD_FORK,
    // 完成任务
    COMMIT_CMD_DONE,
    // 中止任务
    COMMIT_CMD_ABORT
}COMMIT_CMD;
// 充电模式
typedef enum {
    // 自动充电，带BMS管理
    CHARGE_AUTO,
    // 手动恒压充电，不需要BMS
    CHARGE_BV,
    // 手动恒流充电，不需要BMS
    CHARGE_BI
}CHARGE_MODE;

// 作业提交结构
struct job_commit_data {
    // 作业提交命令
    COMMIT_CMD cmd;
    // URL端提交的时间戳
    time_t url_commit_timestamp;
    // 服务端提交的时间日期
    time_t ontom_commit_date_time;

    //{{ 创建作业参数
    char card_sn[32];            // 所刷卡卡号
    double card_remain;          // 卡内余额
    char card_passwd[16];        // 卡片密码
    CHARGE_GUN_SN charge_gun;    // 充电枪选择
    CHARGE_MODE charge_mode;     // 选择自动或是手动
    double manual_set_charge_volatage; // 手动设置充电电压, 手动充电有效
    double manual_set_charge_current;  // 手动设置充电电流, 手动充电有效
    BILLING_MODE  biling_mode; // 充电计费方式
    double as_kwh;   // 按电量充，KW.h, 自动充电，默认充满
    double as_money; // 按金额充，元
    unsigned int as_time;  // 按时间充电，分钟数
    //}}

    struct list_head job_node;
};
int job_commit(struct charge_task *tsk, const struct job_commit_data *jc, COMMIT_CMD cmd);

/*
 * 充电作业描述，充电管理的最小单位
 */
struct charge_job {
    //  作业状态
    JOBSTATUS job_status;
    // 产生故障时的状
    JOBSTATUS status_befor_fault;
    // 作业序号
    time_t job_url_commit_timestamp;
    // 作业充电枪
    CHARGE_GUN_SN job_gun_sn;
    // 充电模式
    CHARGE_MODE charge_mode;
    // 通信CAN名称
    char job_can_dev_name[32];
    // 充电起始时戳， 闭合充电开关的那一刻
    time_t charge_begin_timestamp;
    // 充电结束时戳， 断开充电开关的那一刻
    time_t charge_stop_timestamp;
    // 转为实施状态时的时戳, 执行实施函数的那一刻
    time_t charge_implemention_timestamp;
    // BMS握手成功的时戳, 接收到第一次BRM的时刻
    time_t charge_bms_establish_timestamp;
    // 作业创建的时间戳
    time_t charge_job_create_timestamp;
    // 开始充电时的电表度数
    double charge_begin_kwh_data;
    // 终止充电时的电表度数
    double charge_exit_kwh_data;

    // 已经消耗的电能
    double charged_kwh;
    // 已经充电的时间长度，按秒计
    unsigned int charged_seconds;
    // 已经消费的数额, 按元记，精确到分
    double charged_money;

    // 结构体引用计数
    unsigned int ref_nr;

    // {{ 充电控制参数
    double need_V;
    double need_I;
    // }}

    // 充电计费方式
    struct billing_methord charge_billing;
    // 刷卡状态
    struct user_card card;
    // BMS 管理模块
    struct bms_struct bms;

    // BMS 读线程ID
    pthread_t tid_read;
    // BMS 写线程ID
    pthread_t tid_write;

    // 作业状态字
    volatile unsigned char single[8];

    struct list_head job_node;
};
typedef enum {
    // 暂停作业命令
    CMD_JOB_MAN_PAUSE = 0x00,
    // 恢复作业
    CMD_JOB_RESUME = 0x01,
    // 中止作业
    CMD_JOB_ABORT
}JOB_FLAGS;

// 故障恢复原因
typedef  enum {
    // 故障消除后恢复
    REASON_NORMAL,
    // 系统做初始化时恢复前一次运行时的故障
    REASON_MANAUL
}ERR_RECOVER_REASON;

// 历史故障定义
struct error_history {
    // 故障唯一ID
    unsigned int error_seqid;
    // 故障代码编号
    unsigned int error_id;
    // 故障起始日期, 时间
    char error_begin[20];
    // 故障恢复日期，时间
    char error_recover[20];
    // 故障参数
    char error_string[32];
    // 故障恢复原因
    ERR_RECOVER_REASON error_recover_reason;

    struct list_head error_me;
};
unsigned int error_history_begin(struct charge_job *job, unsigned int error_id, char *error_string);
void error_history_recover(struct charge_job *job, unsigned int error_id);

// 充电系统类型
typedef enum {
    SYSTEM_FENTISHI, // 分体式
    SYSTEM_YITISHI   // 一体式
}SYSTEM_TYPE;

// 模块类型
typedef enum {
    MODEL_AN10750,
    MODEL_AN20750,
    MODEL_AN30750,
    MODEL_AN40750,
    MODEL_INCREASE,
    MODEL_UNKOWN
}MODULE_MODEL;

// UI 界面
typedef enum {
    UI_PAGE_OTHER,
    UI_PAGE_MAIN,
    UI_PAGE_JOBS_POOL,
    UI_PAGE_JOBS
}UI_PAGE;

/*
 * 充电任务描述, 详细描述了系统的配置参数
 */
struct charge_task {
    // 已经编译为多线程安全模式，所以不用加锁
    sqlite3 *database;
    pthread_t tid;
    pthread_attr_t attr;

    // 作业任务提交列表
    struct list_head *commit_head;
    // 作业任务提交列表锁
    pthread_mutex_t commit_lck;

    // 刷卡数据临时存储区
    struct user_card card;
    // 刷卡数据临时存储区
    struct user_card op_card;
    // 当前界面显示的页面号
    UI_PAGE uipage;
    // 当处于作业详情页刷卡时的当前作业ID
    unsigned int ui_job_id;

    // 当前正在执行的充电任务
    struct charge_job *job[CONFIG_SUPPORT_CHARGE_JOBS];

    // 等待作业列表
    struct list_head *wait_head;
    // 作业任务等待列表锁
    pthread_mutex_t wait_lck;
    // 等待作业个数
    unsigned int wait_job_nr;

    // 当前故障列表
    pthread_mutex_t err_list_lck;
    unsigned int err_nr;
    struct list_head *err_head;

    // 任务记录故障总数
    unsigned int err_seq_id_next;

    // 串口设备管理模块
    struct bp_uart *uarts[CONFIG_SUPPORT_RS485_NR];
    // 采样单元管理模块
    struct measure_struct *measure[CONFIG_SUPPORT_SIMPLE_BOX_NR];
    // 充电机管理模块
    struct charger_struct *chargers[CONFIG_SUPPORT_CHARGE_GRP_NR];
    // 电表电能
    struct meter_data meter[CONFIG_SUPPORT_CHARGE_GRP_NR];

    // 系统信号, 最多支持64 * 8 种信号标记
    // 前面 16 * 8 = 128 个信号是系统内部使用信号标记
    // 后面 的为遥信 信号定义 @ enum ONTOM_FLAG_SINGLE
    volatile unsigned char single[64];
    // 系统前一次信号状态，用来做状态跳变比较
    volatile unsigned char pre_single[64];

    // {{ 以下为充电桩系统监控的配置数据
    SYSTEM_TYPE sys_type;
    /* 充电冲突映射表
     * 充电冲突根据系统配置的充电枪个数确定, 目前系统最多支持4把枪
     *   -------------+-------------+---------------+-----------+----------+
     *        \冲突枪  |             |               |           |          |
     *   充电枪 \      |  1 #充电枪   |   2 #充电枪   | 3 #充电枪  | 4 #充电枪 |
     *   -------------+-------------+---------------+-----------+----------+
     *      1 #充电枪  |     /      |    CONFLICT   |    OK      |    OK    |
     *   -------------+-------------+---------------+-----------+----------+
     *      2 #充电枪  |  CONFLICT   |       /      |    OK      |    OK    |
     *   -------------+-------------+---------------+-----------+----------+
     *      3 #充电枪  |     OK      |      OK       |     /     | CONFLICT |
     *   -------------+-------------+---------------+-----------+----------+
     *      4 #充电枪  |     OK      |      OK       |  CONFLICT |    /     |
     *   -------------+-------------+---------------+-----------+----------+
     * 对应位置上 / 和 OK为TRUE 表示不冲突, CONFLICT 对应位置为FALSE表示冲突
     * 默认表格，两把枪
     *   -------------+-------------+---------------+
     *        \冲突枪  |             |               |
     *   充电枪 \      |  1 #充电枪   |   2 #充电枪   |
     *   -------------+-------------+---------------+
     *      1 #充电枪  |     /       |    CONFLICT   |
     *   -------------+-------------+---------------+
     *      2 #充电枪  |  CONFLICT   |       /      |
     *   -------------+-------------+---------------+
     */
    bool sys_conflict_map[CONFIG_SUPPORT_BMS_NR][CONFIG_SUPPORT_BMS_NR];
    /*系统配置的充电枪个数*/
    unsigned int sys_config_gun_nr;
    /* 不同充电接口对应的CAN设备名称*/
    char sys_can_name[CONFIG_SUPPORT_BMS_NR][128];
    /*采样单元个数, 默认为1个，若配置两个采样盒则可以*/
    unsigned int sys_simple_box_nr;
    /*充电机组数， 默认1组.*/
    unsigned int sys_charge_group_nr;
    /*通信用485串口数个数*/
    unsigned int sys_rs485_dev_nr;
    /*通信用485串口设备文件名*/
    char sys_uart_name[CONFIG_SUPPORT_RS485_NR][128];
    // 每度电的价格
    double kwh_price;

    // 以下适用于采样盒配置值
    double bus_1_v_hi;
    double bus_1_v_lo;
    double bus_2_v_hi;
    double bus_2_v_lo;
    double bat_1_v_hi;
    double bat_1_v_lo;
    double bat_2_v_hi;
    double bat_2_v_lo;
    double bat_1_I_hi;
    double bat_2_I_lo;
    double bat_1_insti_ohm_v;
    double bat_2_insti_ohm_v;
    double bus1_bat1_check_v;
    double bus2_bat2_check_v;
    double bat1_bat2_check_I;

    /*额定电流*/
    double max_output_I;
    /*输出限流*/
    double limit_output_I;
    /*电压上限值*/
    double limit_max_V;
    /*电压下限值*/
    double limit_min_V;
    /*工作电压值*/
    double running_V;
    /*当前电流值*/
    double running_I;
    /*模块个数*/
    unsigned short modules_nr;
    /*充电状态*/
    unsigned short charge_stat;
    /*模块型号*/
    MODULE_MODEL module_model;

    /*模块开关机*/
    unsigned short modules_on_off;
    //}}
};

/* 系统信号定义
 * 前 128个信号为系统标记
 */
typedef enum {
    FLAG_BEGIN,
    // 扩展测量值刷新标记
    F_MEASURE_DATA_NEW,

    // BMS 结构体初始化完成
    F_BMS_INIT_DONE,
    // 创建BMS写线程完成
    F_BMS_FORK_WRITE_DONE,
    // 创建BMS读线程完成
    F_BMS_FORK_READ_DONE,

    //{{{ 状态标记
    // BMS 已经识别
    F_BMS_RECOGNIZED,
    // 已经发送车辆已经识别数据包
    F_VEHICLE_RECOGNIZED,
    // 已经收到BMS准备充电报文
    F_BMS_READY,
    // 充电机充电准备完成
    F_CHARGER_READY,
    // 接收到电池充电总状态报文
    F_PCK_BAT_STATUS,
    // 接收到BMS终止充电报文
    F_PCK_BMS_TRM,
    // 接收电池充电总状态报文超时
    F_PCK_BAT_STATUS_TIMEOUT,

    // 触发充电刷卡完成
    F_CARDING_TRIGER,
    // 确认充电刷卡完成
    F_CARDING_CONFIRM,
    // 充电结束刷卡完成
    F_CARDING_SETTLE,

    // 系统人为条件可以充电
    F_MANUAL_CHARGE_ALLOW, // 人为禁止充电
    // 系统硬件条件可以充电
    F_SYSTEM_CHARGE_ALLOW, // 系统无故障, 关键故障
    // 直流输出总开关闭合状态
    F_DC_OUTPUT_SWITCH_STATUS,
    // 1#枪物理连接完成状态
    F_GUN_1_PHY_CONN_STATUS,
    // 1#枪输出开关状态
    F_GUN_1_OUTPUT_SWITCH_STATUS,
    // 1#枪辅助电源开关状态
    F_GUN_1_ASSIT_PWN_SWITCH_STATUS,
    // 2#枪物理连接完成状态
    F_GUN_2_PHY_CONN_STATUS,
    // 2#枪输出开关状态
    F_GUN_2_OUTPUT_SWITCH_STATUS,
    // 2#枪辅助电源开关状态
    F_GUN_2_ASSIT_PWN_SWITCH_STATUS,
    // }}}

    // {{ 输出控制命令状态
    // 直流输出总开关打开
    CMD_DC_OUTPUT_SWITCH_ON  = 0x0080,
    // 1#枪辅助电源打开
    CMD_GUN_1_ASSIT_PWN_ON,
    // 1#枪输出开关打开
    CMD_GUN_1_OUTPUT_ON,
    // 2#枪辅助电源打开
    CMD_GUN_2_ASSIT_PWN_ON,
    // 2#枪输出开关打开
    CMD_GUN_2_OUTPUT_ON,
    // }}
    // {{ 充电作业操作命令
    // 中止当前充电作业
    //CMD_JOB_ABORT,
    // 暂停当前充电作业
    //CMD_JOB_MAN_PAUSE,
    // }}
    //{{ 模块操作
    CMD_MODULE_ON,
    CMD_MODULE_OFF,
    //}}

    // {{卡操作
    CMD_CARD_FORMAT,
    CMD_CARD_SET,
    // }}

    // {{{ 故障标记
    // BMS通信故障
    S_BMS_COMM_DOWN,
    // }}}
    FLAG_END,

    //总故障, 参照文档 -- 充电桩故障对照表
    S_ERROR = 0x100,
    S_BUS_0_VHI,
    S_BUS_0_VLO,
    S_BUS_0_SHORT,
    S_BUS_1_VHI,
    S_BUS_1_VLO,
    S_BUS_1_SHORT,
    S_BAT_0_VHI,
    S_BAT_0_VLO,
    S_BAT_0_SHORT,
    S_BAT_0_REVERT,
    S_BAT_0_INSTITUDE,
    S_BAT_0_IHI,
    S_BAT_1_VHI,
    S_BAT_1_VLO,
    S_BAT_1_SHORT,
    S_BAT_1_REVERT,
    S_BAT_1_INSTITUDE,
    S_BAT_1_IHI,
    S_ASSIT_PWN_ERR,
    S_TEMP_HI,
    S_TEMP_LO,
    S_WET_HI,
    S_WET_LO,
    S_DC_OUTPUT_RD,
    S_DC_OUTPUT_TRIP,
    S_DC_OUTPUT_0_TRIP,
    S_DC_OUTPUT_1_TRIP,
    S_FLQ_ERROR,
    S_CHARGE_GROUP_ERR,
    S_CHARGE_M_1_ERR,
    S_CHARGE_M_2_ERR,
    S_CHARGE_M_3_ERR,
    S_CHARGE_M_4_ERR,
    S_CHARGE_M_5_ERR,
    S_CHARGE_M_6_ERR,
    S_CHARGE_M_7_ERR,
    S_CHARGE_M_8_ERR,
    S_CHARGE_M_9_ERR,
    S_CHARGE_M_10_ERR,
    S_CHARGE_M_11_ERR,
    S_CHARGE_M_12_ERR,
    S_CHARGE_M_13_ERR,
    S_CHARGE_M_14_ERR,
    S_CHARGE_M_15_ERR,
    S_CHARGE_M_16_ERR,
    S_CHARGE_M_17_ERR,
    S_CHARGE_M_18_ERR,
    S_CHARGE_M_19_ERR,
    S_CHARGE_M_20_ERR,
    system_reserve0,
    system_reserve1,
    system_reserve2,
    system_reserve3,
    system_reserve4,
    system_reserve5,
    system_reserve6,
    system_reserve7,
    system_reserve8,
    system_reserve9,
    system_reserve10,
    system_reserve11,
    system_reserve12,
    system_reserve13,
    system_reserve14,
    system_reserve15,
    system_reserve16,
    system_reserve17,
    system_reserve18,
    system_reserve19,
    system_reserve20,
    S_CHARGER_COMM_DOWN,
    S_CHARGER_YX_1_COMM_DOWN,
    S_CHARGER_YX_2_COMM_DOWN,
    S_MEASURE_1_COMM_DOWN,
    S_CONVERT_BOX_COMM_DOWN,
    S_MEASURE_2_COMM_DOWN,
    S_CARD_READER_COMM_DOWN,
    S_KWH_METER_COMM_DOWN,
    system_reserve24,
    system_reserve25,
    system_reserve26,
    system_reserve27,
    system_reserve28,
    system_reserve29,
    system_reserve30,
    system_reserve31,
    system_reserve32,
    system_reserve33,
    system_reserve34,
    system_reserve35,
    system_reserve36,
    system_reserve37,
    system_reserve38,
    system_reserve39,
    system_reserve40,
    system_reserve41,
    system_reserve42,
    system_reserve43,
    system_reserve44,
    system_reserve45,
    S_END
}ONTOM_FLAG_SINGLE;

// 位设置
static inline void __bit_set(volatile unsigned char *byte, ONTOM_FLAG_SINGLE single)
{
    if ( ! byte ) return;
    byte += single / 8;
    * byte |= (1 << (single % 8 ));
}
#define bit_set(tsk, bits) __bit_set(tsk->single, bits)
#define bit_set_pre(tsk, bits) __bit_set(tsk->pre_single, bits)
// 位清除
static inline void __bit_clr(volatile unsigned char *byte, ONTOM_FLAG_SINGLE single)
{
    if ( ! byte ) return;
    byte += single / 8;
    * byte &= (~(1 << (single % 8 )));
}
#define bit_clr(tsk, bits) __bit_clr(tsk->single, bits)
#define bit_clr_pre(tsk, bits) __bit_clr(tsk->pre_single, bits)
// 位读取
static inline int __bit_read(volatile unsigned char *byte, ONTOM_FLAG_SINGLE single)
{
    if ( ! byte ) return 0;
    byte += single / 8;

    return (* byte & (1 << (single % 8 ))) ? 1 : 0;
}
#define bit_read(tsk, bits) __bit_read(tsk->single, bits)
#define bit_read_pre(tsk, bits) __bit_read(tsk->pre_single, bits)

//1字节crc16计算
static inline void calc_crc16(unsigned short *crc, unsigned short  crcbuf)
{
     unsigned char  i,TT;

    *crc=*crc^crcbuf;
    for(i=0;i<8;i++)
    {
        TT=*crc&1;
        *crc=*crc>>1;
        *crc=*crc&0x7fff;
        if (TT==1)
            (*crc)=(*crc)^0xa001;
        *crc=*crc&0xffff;
    }
}

//多字节CRC16计算
static inline unsigned short load_crc(unsigned short cnt, char *dat)
{
     unsigned short i;
     unsigned short crc=0xffff;
    for(i=0;i<cnt&&i<0xff;i++)
    {
        calc_crc16(&crc,dat[i]);
    }
    return crc;
}

// 大小端转换
static inline unsigned short swap_hi_lo_bytes(unsigned short b)
{
    unsigned char h = b >> 8, l = b & 0xFF;
    return (l << 8 | h);
}
#define b2l swap_hi_lo_bytes
#define l2b swap_hi_lo_bytes

static inline CHARGE_GUN_SN __is_gun_phy_conn_ok(struct charge_job *thiz)
{
    if ( thiz->job_gun_sn == GUN_SN0 ) {
        if ( bit_read(task, F_GUN_1_PHY_CONN_STATUS) ) {
            return GUN_SN0;
        } else return GUN_INVALID;
    } else if ( thiz->job_gun_sn == GUN_SN1 ) {
        if ( bit_read(task, F_GUN_2_PHY_CONN_STATUS) ) {
            return GUN_SN1;
        } else return GUN_INVALID;
    } else if ( thiz->job_gun_sn == GUN_UNDEFINE ) {
        return GUN_UNDEFINE;
    } else {
        return GUN_INVALID;
    }
}

static inline void __get_timestamp(char *outstring)
{
    time_t timep;
    struct tm *p;
    timep = time(NULL);
    p =localtime(&timep);
    sprintf(outstring, "%04d-%02d-%02d %02d:%02d:%02d",
            p->tm_year + 1900,
            p->tm_mon + 1,
            p->tm_mday,
            p->tm_hour,
            p->tm_min,
            p->tm_sec);
}

static inline unsigned int __atoh(const char *hex)
{
    unsigned int v = 0;

    if ( !hex ) return 0;
    if ( hex[0] == '0' && (hex[1] == 'X' || hex[1] == 'x' ) )
        hex ++, hex ++;

    while ( *hex ) {
        if (*hex >= '0' && *hex <= '9') {
            v = v * 16 + (*hex) - '0';
        } else if (*hex >= 'A' && *hex <= 'F') {
            v = v * 16 + (*hex) - 'A' + 10;
        } else if (*hex >= 'a' && *hex <= 'f') {
            v = v * 16 + (*hex) - 'a' + 10;
        } else break;
        hex ++;
    }

    return v;
}
#define atoh __atoh
static inline double __bytes2double(unsigned short bytes)
{
    if ( 0x8000 & bytes ) {
        return (bytes&0x7FFF)/(-10.0f);
    }
    return (bytes & 0x7FFF) / 10.0f;
}

static inline unsigned short double2short(double df, unsigned int acc) {
    return  (unsigned short)(df * acc * 1.0f);
}

static inline double bytes2double(unsigned char h, unsigned l, unsigned int acc) {
    return (h * 256 + l) / (1.0f * acc);
}

void deal_with_system_protection(struct charge_task *tsk, struct charge_job *thiz);
void job_running(struct charge_task *, struct charge_job *);
void job_running(struct charge_task *tsk, struct charge_job *thiz);
int job_commit(struct charge_task *tsk, const struct job_commit_data *jc, COMMIT_CMD cmd);
struct charge_job * job_fork(struct charge_task *tsk, struct job_commit_data *need);
struct charge_job* job_search(time_t ci_timestamp);
struct charge_job * job_select_wait(struct charge_task *tsk, CHARGE_GUN_SN gun);
struct job_commit_data *job_select_commit(struct charge_task *tsk);
void job_detach_wait(struct charge_task *tsk);
#endif /*_CHARGE_INCLUDED_H_*/
