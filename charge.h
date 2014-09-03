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
    /* 充电任务编号，唯一
       充电编号由时间构成，由系统自动生成, 共计32位
       charge_sn
          [26:31]: 年（1975年-2039年）
          [22:25]: 月（1-12）
          [17:21]: 日(1-31)
          [12:16]: 时（0-23）
          [6:11]: 分（0-59）
          [0:5]:  秒（0-59）
    */
    unsigned int charge_sn;
    // 充电起始时间，位定义同charge_sn
    unsigned int charge_begin;
    // 充电结束时间，位定义同charge_sn
    unsigned int charge_stop;

    // 充电结果
    unsigned int charge_result;
};



#endif /*_CHARGE_INCLUDED_H_*/
