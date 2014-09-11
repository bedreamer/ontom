#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "bms.h"
#include "charge.h"
#include "config.h"
#include "log.h"
#include "error.h"

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
    thiz->charge_stage = CHARGER_INVALID;
    thiz->charge_begin_timestamp = INVALID_TIMESTAMP;
    thiz->charge_implemention_timestamp = INVALID_TIMESTAMP;
    thiz->charge_bms_establish_timestamp = INVALID_TIMESTAMP;
    thiz->charge_stop_timestamp = INVALID_TIMESTAMP;

    log_printf(INF, "charge task created, serial number: %08X",
               thiz->charge_sn);
    thiz->charge_sn = time();
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
