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
#define INVALID_TIMESTAMP  0x00000000 // 无效时戳，初始化时默认赋值， 用于time_t默认值

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

// 充电阶段定义
typedef enum {
    CHARGE_STAGE_INVALID      =0x00, // 充电状态无效，可能是在初始化或者析构
    CHARGE_STAGE_HANDSHACKING =0x01, // 充电握手
    CHARGE_STAGE_CONFIGURE    =0x02, // 充电配置
    CHARGE_STAGE_CHARGING     =0x03, // 充电
    CHARGE_STAGE_DONE         =0x04 // 充电结束
}CHARGE_STAGE;

// 充电计费模式
typedef enum {
    CHARGE_MODE_INVALID    =0x00,  // 无效充电模式，默认值
    CHARGE_MODE_AS_AUTO    =0x01,  // 自动充电
    CHARGE_MODE_AS_MONEY   =0x02,  // 按金额充电
    CHARGE_MODE_AS_TIME    =0x03,  // 按时间充电
    CHARGE_MODE_AS_CAP     =0x04  // 按容量充电
}CHARGE_MODE;

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
    // 充电异常退出
    CHARGE_STAT_EXCEPTION    =0xFF
}CHARGE_TASK_STAT;

/*
 * 充电任务描述
 */
struct charge_task {
    /* 充电任务编号，唯一，创建充电任务的那一刻
    */
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
    CHARGE_TASK_STAT charge_task_state;

    // 充电结果
    unsigned int charge_result;
    // 充电阶段
    CHARGE_STAGE charge_stage;

    // 充电模式
    struct {
        // 充电方式
        CHARGE_MODE mode;
        union {
            // 设定充电金额, 0 - 9999
            double set_money;
            // 设定充电时长, 0 - 600
            unsigned int set_time;
            // 设定充电目标容量, 0 - 100
            unsigned int set_cap;
        };
    }charge_mode;

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
};

// 创建充电任务
struct charge_task * charge_task_create(void);
// 实施充电任务
void charge_task_implemention(struct charge_task *thiz);
// 析构充电任务
void charge_task_destroy(struct charge_task *thiz);

#endif /*_CHARGE_INCLUDED_H_*/
