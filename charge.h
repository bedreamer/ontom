/*
 * 充电控制描述结构
 * All rights reserved Hangzhou Power Equipment Co., Ltd.
 * author: LiJie
 * email:  bedreamer@163.com
 * 2014-08-22
*/

#ifndef _CHARGE_INCLUDED_H_
#define _CHARGE_INCLUDED_H_

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
    #define INVALID_TIMESTAMP  0xFFFFFFFF // 无效时戳，初始化时默认赋值

    // 充电结果
    unsigned int charge_result;
    // 充电阶段
    unsigned int charge_stage;
    #define CHARGE_INVALID      0x00 // 充电状态无效，可能是在初始化或者析构
    #define CHARGE_HANDSHACKING 0x01 // 充电握手
    #define CHARGE_CONFIGURE    0x02 // 充电配置
    #define CHARGE_CHARGING     0x03 // 充电
    #define CHARGE_DONE         0x04 // 充电结束
};

// 创建充电任务
struct charge_task * charge_task_create(void);
// 实施充电任务
void charge_task_implemention(struct charge_task *thiz);
// 析构充电任务
void charge_task_destroy(struct charge_task *thiz);

#endif /*_CHARGE_INCLUDED_H_*/
