/*
 */
#ifndef _HACHIKO_INCLUDED_H_
#define _HACHIKO_INCLUDED_H_

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
int Hachiko_new(struct Hachiko_food *, Hachiko_Type type,
                 unsigned int ttl, void *private);
static inline void Hachiko_feed(struct Hachiko_food *dog)
{
    dog->remain = dog->ttl;
}

#endif // _HACHIKO_INCLUDED_H_
