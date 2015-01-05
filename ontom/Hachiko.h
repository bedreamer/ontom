/*
 */
#ifndef _HACHIKO_INCLUDED_H_
#define _HACHIKO_INCLUDED_H_
#include "log.h"
struct Hachiko_food;

typedef enum {
    // 超时事件
    HACHIKO_TIMEOUT = 0,
    // ttl递减事件
    HACHIKO_ROLLING = 1,
    // 定时器失效事件
    HACHIKO_DIE     = 2,
    // 自动设定事件
    HACHIKO_FEEDED  = 3
}Hachiko_EVT;

typedef enum {
    // 一次性的
    HACHIKO_ONECE     = 0x40,
    // 自动循环定时器
    HACHIKO_AUTO_FEED = 0x80
}Hachiko_Type;

typedef enum {
    // 无效定时器, 初始状态
    HACHIKO_INVALID   = 0x00,
    // 正常使用
    HACHIKO_NORMAL    = 0x01,
    // 计时器暂停
    HACHIKO_PAUSE     = 0x02,
    // 计时器恢复
    HACHIKO_RESUME    = 0x03,
    // 计时器死亡，已经删除
    HACHIKO_KILLED    = 0x04
}Hachiko_status;

struct Hachiko_food {
    // 定时器事件回调
    void (*Hachiko_notify_proc)(Hachiko_EVT evt, void *private,
                                const struct Hachiko_food *self);
    // 定时器类型
    Hachiko_Type type;
    // 定时器时长, 最小1ms的精度，根据设置而定，默认为10ms
    unsigned int ttl;
    // 定时器剩余时长
    volatile unsigned int remain;
    // 定时器状态
    Hachiko_status status;
    // 私有数据
    void *private;
};

typedef enum {
    // 无效
    PRIVATE_INVALID = 0x00,
    // 已经使用
    PRIVATE_BUSY    = 0x01,
    // 闲置等待
    PRIVATE_STANDBY = 0x02
}PRIVATE_STATUS;
// CAN 连接传输管理超时参数组
struct Hachiko_CNA_TP_private {
    PRIVATE_STATUS status;
};

void Hachiko_init();
int _Hachiko_new(struct Hachiko_food *thiz, Hachiko_Type type,
                 unsigned int ttl, Hachiko_status status, void *private);
#define Hachiko_new(food, type, ttl, private) \
    _Hachiko_new(food, type, ttl, HACHIKO_NORMAL, private)

static inline void Hachiko_kill(struct Hachiko_food *dog)
{
    if ( dog->status != HACHIKO_INVALID )
        dog->status = HACHIKO_KILLED;
}

static inline void Hachiko_pause(struct Hachiko_food *dog)
{
    dog->status = HACHIKO_PAUSE;
}


static inline void Hachiko_resume(struct Hachiko_food *dog)
{
    dog->status = HACHIKO_RESUME;
}

static inline void Hachiko_feed(struct Hachiko_food *dog)
{
    dog->remain = dog->ttl;
}

#endif // _HACHIKO_INCLUDED_H_
