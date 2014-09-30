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
/*    struct charge_task *thiz =
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
    */
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
    while ( thiz->card.triger_timestamp == INVALID_TIMESTAMP ||
            thiz->card.triger_synced_timestamp == INVALID_TIMESTAMP ) {
        // 休眠 500 微秒
        usleep(500);
    }
}

/* 充电任务服务线程
 * 充电状态的转换触发条件都由UI转发过来，这样做的目的是为了保证触发唯一和触发条件的同步。
 *
 * 1. 触发信号的采集过程由服务器完成；
 * 2. 将该信号传递（同步）给浏览器，浏览器确认后向服务器；
 * 3. 发送状态转换触发条件，完成状态转换。
 */
void *thread_charge_task_service(void *arg) ___THREAD_ENTRY___
{
    int *done = (int *)arg;
    int mydone = 0;
    if ( done == NULL ) done = &mydone;
    log_printf(INF, "%s running...sizeof(struct charge_task)=%d",
               __FUNCTION__,
               sizeof(struct charge_task));

    //task = charge_task_create();
    task->charge_task_stat = CHARGE_STAT_INVALID;

    while ( ! *done ) {
        switch ( task->charge_task_stat) {
        // 无效任务状态
        case CHARGE_STAT_INVALID:
            log_printf(INF, "charge task status RESET.");
            config_write("triger_card_sn", "N/A");
            config_write("confirm_card_sn", "N/A");
            config_write("settle_card_sn", "N/A");
            task->charge_task_stat = CHARGE_STAT_TRIGER_PEDDING;
            break;
        // 空闲等待,界面停止于充电模式选择界面
        case CHARGE_STAT_TRIGER_PEDDING:
            break;
        // 充电确认, 刷卡完成，进行充电确认等待
        case CHARGE_STAT_CONFIRM_PEDDING:
            break;
        // BMS 连接等待, 已经确认充电
        case CHARGE_STAT_WAIT_BMS:
            break;
        // BMS 已经连接，进行充电参数配置
        case CHARGE_STAT_READY:
            break;
        // 充电阶段
        case CHARGE_STAT_CHARGING:
            break;
        // 充电结束阶段, 断开接触器
        case CHARGE_STAT_DONE:
            break;
        // 计费阶段, 等待刷卡付费阶段
        case CHARGE_STAT_BILLING_PEDDING:
            break;
        // 充电任务退出阶段，日志记录，清理
        case CHARGE_STAT_EXIT:
            break;
        // 充电任务被终止
        case CHARGE_STAT_ABORT:
            task->charge_task_stat = CHARGE_STAT_INVALID;
            break;
        // 充电异常退出
        case CHARGE_STAT_EXCEPTION:
            break;
        }
        usleep(5000);
    }

    return NULL;
}
