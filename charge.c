#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "bms.h"
#include "charge.h"
#include "config.h"
#include "log.h"
#include "error.h"

struct charge_task tom;

// 充电任务结构
struct charge_task *task = &tom;

// 创建充电任务
struct charge_task * charge_task_create(void)
{
    struct charge_task *thiz =
            (struct charge_task*)malloc(sizeof(struct charge_task));

    if ( NULL == thiz ) {
        log_printf(ERR, "create charge task faile, due to low memory!");
        return NULL;
    }

    memset(thiz, 0, sizeof(struct charge_task));
    thiz->charge_sn = INVALID_TIMESTAMP;
    thiz->charge_stage = CHARGE_STAGE_INVALID;
    thiz->charge_begin_timestamp = INVALID_TIMESTAMP;
    thiz->charge_implemention_timestamp = INVALID_TIMESTAMP;
    thiz->charge_bms_establish_timestamp = INVALID_TIMESTAMP;
    thiz->charge_stop_timestamp = INVALID_TIMESTAMP;

    log_printf(INF, "charge task created, serial number: %08X",
               thiz->charge_sn);
    return thiz;
}

// 实施充电任务
// 开始发送充电机辨识报文，并接收BMS车辆辨识报文
// 检查charge_task 参数有效性，并将充电状态从无效转为握手
void charge_task_implemention(struct charge_task *thiz)
{
}

// 析构充电任务
void charge_task_destroy(struct charge_task *thiz)
{
}

// 重置充电任务
// 重新初始化充电任务
void charge_task_reset(struct charge_task *thiz)
{
}

// 等待首次刷卡，触发创建充电任务事件
static inline void wait_for_triger_charge_task(struct charge_task *thiz)
{
    while ( thiz->card.begin_timestamp == INVALID_TIMESTAMP ||
            thiz->card.begin_synced == INVALID_TIMESTAMP ) {
        // 休眠 500 微秒
        usleep(500);
    }
}

// 充电任务服务线程
void *thread_charge_task_service(void *arg) ___THREAD_ENTRY___
{
    int *done = (int *)arg;
    int mydone = 0;
    if ( done == NULL ) done = &mydone;
    log_printf(INF, "%s running...sizeof(struct charge_task)=%d",
               __FUNCTION__,
               sizeof(struct charge_task));

    //task = charge_task_create();
    if ( task == NULL ) {
        log_printf(ERR, "default task struct create faile, panic!");
        while ( ! *done ) {
            sleep(1);
        }
        return NULL;
    }

    while ( ! *done ) {
        // 等待刷卡触发充电任务
        wait_for_triger_charge_task(task);
        usleep(5000);
    }

    return NULL;
}
