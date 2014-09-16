/*
 * 充电控制描述结构
 * All rights reserved Hangzhou Power Equipment Co., Ltd.
 * author: LiJie
 * email:  bedreamer@163.com
 * 2014-08-22
*/

#ifndef _CHARGE_INCLUDED_H_
#define _CHARGE_INCLUDED_H_

#include "bms.h"
// 无效时戳，初始化时默认赋值， 用于time_t默认值
#define INVALID_TIMESTAMP  0x00000000

/* 卡信息
 * 三个阶段分别用三个缓冲区来存储卡号的目的是
 * 为了可以用某些特殊的卡，也就是万能卡号实现终止充电任务中的操作
 * 例如：
 *  使用卡号为 7667987878 的普通卡号触发了充电任务和确认充电
 *  可以使用   8888888888 的管理员卡进行终止充电操作
 *  另外，该冗余设计可以在UI 界面进行刷卡问询时，通过判定卡号是否一致
 *  来进行出错提示。
 *  需要注意的是：
 *    普通卡和管理员卡都可以触发充电任务，管理员卡触发的任务只能又管理员卡或密码终止
 *    普通卡触发的充电任务可以被当前普通卡，管理员卡或密码终止。
 */
struct user_card {
    // 触发任务时的卡号
    char begin_card_sn[64];
    // 触发充电任务时的刷卡时戳
    time_t begin_timestamp;
    // UI界面确认刷卡时戳
    time_t begin_synced;

    // 确认充电时的卡号
    char confirm_card_sn[64];
    // 确认充电任务的刷卡时戳
    time_t confirm_timestamp;
    // UI界面确认刷卡时戳
    time_t confirm_synced;

    // 终止充电时的卡号
    char end_card_sn[64];
    // 终止充电任务的刷卡时戳
    time_t end_timestamp;
    // UI界面确认刷卡时戳
    time_t end_synced;

    // 卡私有信息
    void *private;
};

// 充电扩展测量结果
struct charge_ex_measure {
    // 数据更新时间戳, 每更新一次，时间戳也随之更新
    time_t update_stamp;

    // 充电机输出电压
    double charger_voltage;
    // 充电机输出电流
    double charger_current;
    // 温度
    double temprature;
    // 湿度
    double wet_degree;
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
    CHAEGE_STAT_INVALID      =0x00,
    // 空闲等待,界面停止于充电模式选择界面
    CHARGE_STAT_WAITING      =0x01,
    // 刷卡准备, 于自动模式下已经刷卡，或者手动选择了充电计费模式
    CHARGE_STAT_PREPARE      =0x02,
    // 充电确认, 刷卡完成，进行充电确认等待
    CHARGE_STAT_CONFIRM      =0x03,
    // BMS 连接等待, 已经确认充电
    CHARGE_STAT_WAIT_BMS     =0x04,
    // BMS 已经连接，我收拾别完成，进行充电参数配置
    CHARGE_STAT_READY        =0x05,
    // 充电阶段
    CHARGE_STAT_CHARGING     =0x06,
    // 充电结束阶段
    CHARGE_STAT_DONE         =0x07,
    // 充电异常退出
    CHARGE_STAT_EXCEPTION    =0xFF
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
    CHARGE_STAGE_DONE         =0x04
}CHARGE_STAGE;

/*
 * 充电任务描述
 */
struct charge_task {
    // 充电任务编号，唯一，创建充电任务的那一刻
    time_t charge_sn;
    // 充电起始时戳， 闭合充电开关的那一刻
    time_t charge_begin_timestamp;
    // 充电结束时戳， 断开充电开关的那一刻
    time_t charge_stop_timestamp;
    // 转为实施状态时的时戳, 执行实施函数的那一刻
    time_t charge_implemention_timestamp;
    // BMS握手成功的时戳, 接收到第一次BRM的时刻
    time_t charge_bms_establish_timestamp;

    // 任务状态
    CHARGE_TASK_STAT charge_task_stat;
    // 充电任务所处阶段
    CHARGE_STAGE charge_stage;

    // 充电计费
    struct {
        // 计费方式
        BILLING_MODE mode;
        union {
            // 设定充电金额, 0 - 9999
            double set_money;
            // 设定充电时长, 0 - 600
            unsigned int set_time;
            // 设定充电目标容量, 0 - 100
            unsigned int set_cap;
        }option;
    }charge_billing;

    /* 刷卡信息
     * 该结构主要包含若干卡信息，最基本的信息是卡号
     * 正常充电需要刷卡三次，
     * 第一次： 触发系统开始一个新的充电任务
     * 第二次： 确认实施充电任务
     * 第三次： 终止充电任务并计费
     */
    struct user_card card;

    // 扩展测量值
    struct charge_ex_measure *ex_measure;
    // 前一次读取扩展测量得到的时间戳, 通过对比时间戳来确定扩展测量是否已经更新了数据
    time_t pre_stamp_ex_measure;

    // 车辆基本信息
    struct pgn512_BRM  vehicle_info;
    // BMS充电配置信息
    struct pgn1536_BCP bms_config_info;
    // BMS当前充电需求信息
    struct pgn4096_BCL bms_charge_need_now;
    // BMS 电池充电总状态信息
    //struct pgn4352_BCS bms_all_battery_status;
    // BMS 动力蓄电池状态信息
    //struct pgn4864_BSM bms_battery_status;
};

// 创建充电任务
struct charge_task * charge_task_create(void);
// 实施充电任务
void charge_task_implemention(struct charge_task *thiz);
// 析构充电任务
void charge_task_destroy(struct charge_task *thiz);
// 重置充电任务
void charge_task_reset(struct charge_task *thiz);
extern charge_task *task;

#endif /*_CHARGE_INCLUDED_H_*/
