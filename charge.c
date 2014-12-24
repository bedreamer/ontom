#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include "charge.h"
#include "bms.h"
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
    return NULL;
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

/*
 * 扩展测量数据刷新
 * 刷新数据，标记
 */
void deal_with_measure_data(struct charge_task *thiz)
{
    if ( 0 == bit_read(thiz, F_MEASURE_DATA_NEW) ) {
        return;
    }
}

/*
 * 后台控制逻辑处理
 * 刷新数据，标记
 */
void deal_with_master_contrl_logic(struct charge_task *thiz)
{

}

/*
 * BMS 控制逻辑处理
 * 刷新数据，标记，刷新下发数据、标记
 */
void deal_with_BMS_logic(struct charge_task *thiz)
{

}

/*
 * 充电动作逻辑处理
 * 处理充电动作，设置标记，刷新数据
 */
void deal_with_charge_logic(struct charge_task *thiz)
{

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
    int charging = 0, len = 0;
    char errstr[1024] = {0};
    char *keyerr = config_read("keyfault");
    if ( keyerr == NULL ) {
        // 参考文档 充电桩相关信息.xlsx 充电桩故障对照表
        // 0 标识非关键故障， 1 关键故障, X 其他
        keyerr = "X11100111111100100110000011111";
    }
    if ( done == NULL ) done = &mydone;
    log_printf(INF, "ZUES: %s running...sizeof(struct charge_task)=%d",
               __FUNCTION__,
               sizeof(struct charge_task));

    //task = charge_task_create();
    task->charge_task_stat = CHARGE_STAT_INVALID;
    memset((char *)task->single, 0, sizeof(task->single));
    memset((char *)&task->measure, 0, sizeof(task->measure));

    while ( ! *done ) {
        switch ( task->charge_task_stat) {
        // 无效任务状态
        case CHARGE_STAT_INVALID:
            log_printf(INF, "ZUES: charge task status RESET.");
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
            if ( bit_read(task, F_BMS_RECOGNIZED) ) {
                task->charge_task_stat = CHARGE_STAT_READY;
            }
            break;
        // BMS 已经连接，进行充电参数配置, 连接条件等待
        case CHARGE_STAT_READY:
            if ( ! bit_read(task, F_MANUAL_CHARGE_ALLOW) ) {
                log_printf(INF, "ZEUS: 系统禁止充电(人工).");
                charging --;
            }

            // 因故障无法充电原因参考文档 充电桩相关信息.xlsx 充电桩故障对照表
            if ( bit_read(task, S_ERROR) ) {
                log_printf(WRN, "ZEUS: 系统禁止充电(故障).");
                charging --;
            }

            if ( !bit_read(task, F_SYSTEM_CHARGE_ALLOW) ) {
                log_printf(WRN, "ZEUS: 系统禁止充电(条件).");
                charging --;
            }

            if ( charging >= 0 ) {

            }

            if ( ! bit_read(task, F_SYSTEM_CHARGE_ALLOW) ) {
                memset(errstr, 0, sizeof(errstr));
                len = 0;
                if ( bit_read(task, S_AC_INPUT_DOWN)) {
                    len += sprintf(errstr, "系统交流停电 ");
                }
                if ( bit_read(task, S_ASSIT_POWER_DOWN )) {
                    len += sprintf(errstr, "辅助电源故障 ");
                }
                if ( bit_read(task, S_INSTITUDE_ERR )) {
                    len += sprintf(errstr, "系统绝缘故障 ");
                }
                if ( bit_read(task, S_BMS_COMM_DOWN )) {
                    len += sprintf(errstr, "BMS通信故障");
                }
                if ( bit_read(task, S_MEASURE_COMM_DOWN) ) {
                    len += sprintf(errstr, "综合采样盒通讯故障 ");
                }
                if ( bit_read(task, S_CHARGER_COMM_DOWN) ) {
                    len += sprintf(errstr, "充电机组通讯故障 ");
                }
                log_printf(WRN, "ZEUS: 系统无法充电(故障), [%s]", errstr);
                break;
            }
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

        // 扩展测量数据刷新
        deal_with_measure_data(task);
        // 后台控制逻辑处理
        deal_with_master_contrl_logic(task);
        // BMS 控制逻辑处理
        deal_with_BMS_logic(task);
        // 充电动作逻辑处理
        deal_with_charge_logic(task);
        usleep(5000);
    }

    return NULL;
}
