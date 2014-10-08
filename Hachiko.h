/*
 */
#ifndef _HACHIKO_INCLUDED_H_
#define _HACHIKO_INCLUDED_H_

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
    HACHIKO_ONECE = 0,
    // 自动循环定时器
    HACHIKO_AUTO_FEED = 1
}Hachiko_Type;

struct Hachiko_food {
    // 定时器事件回调
    void (*Hachiko_notify_proc)(Hachiko_EVT evt, void *private,
                                const Hachiko_food *self);
    // 定时器类型
    Hachiko_Type type;
    // 定时器时长, 最小1ms的精度，根据设置而定，默认为10ms
    unsigned int ttl;
    // 定时器剩余时长
    volatile unsigned int remain;
    // 私有数据
    void *private;
};

void Hachiko_init();
int Hachiko_feed(struct Hachiko_food *, Hachiko_Type type,
                 unsigned int ttl, void *private);

#endif // _HACHIKO_INCLUDED_H_
