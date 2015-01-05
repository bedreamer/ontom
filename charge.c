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
void deal_with_system_protection(struct charge_task *thiz);
void deal_with_job_business(struct charge_task *thiz);
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

#define true_express(i) (keyerr[i] == '1' || keyerr[i] == 't' || keyerr[i] =='T'|| keyerr[i] == 'y' || keyerr[i] == 'Y')
#define false_express(i) (keyerr[i] == '0' || keyerr[i] == 'f' || keyerr[i] =='F'|| keyerr[i] == 'n' || keyerr[i] == 'N')
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
    int mydone = 0, i;
    int charging = 0, len = 0, fault, warn, omit;
    char errstr[1024] = {0};
    const char *keyerr = config_read("keyfault");
    if ( keyerr == NULL ) {
        // 参考文档 充电桩相关信息.xlsx 充电桩故障对照表
        // [0nNfF]:标识非关键故障， [1yYtT]:关键故障, [^0nNfF1yYtT]:其他
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
    memset((char *)task->jobs, 0, sizeof(task->jobs));
    task->nr_jobs = 0;
    task->this_job = NULL;
    bit_set(task, F_MANUAL_CHARGE_ALLOW);

    while ( ! *done ) {
#if 0
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
        // 充电确认, 刷卡完成， 进行充电确认等待
        case CHARGE_STAT_CONFIRM_PEDDING:
            break;
        // BMS 连接等待, 已经确认充电
        case CHARGE_STAT_WAIT_BMS:
            if ( bit_read(task, F_BMS_RECOGNIZED) ) {
                task->charge_task_stat = CHARGE_STAT_READY;
                log_printf(INF, "ZEUS: BMS 已经识别..");
            } else {
                bit_set(task, F_BMS_RECOGNIZED);
            }
            break;
        // BMS 已经连接，进行充电参数配置, 连接条件等待
        case CHARGE_STAT_READY:
            if ( ! bit_read(task, F_MANUAL_CHARGE_ALLOW) ) {
                log_printf(INF, "ZEUS: 系统禁止充电(人工).");
                charging --;
            }

            fault = 0;
            warn = 0;
            omit = 0;
            memset(errstr, 0, sizeof(errstr));
            for ( i = 1; i < S_ERR_END - S_ERROR; i ++ ) {
                if ( bit_read(task, i + S_ERROR) ) {
                    switch ( i + S_ERROR ) {
                    case S_GUN_1_PYH_CONN_DOWN:
                    case S_GUN_1_SW_BREAK:
                        if ( task->can_charge_gun_sn == GUN_SN0 && true_express(i) ) {
                            fault ++;
                            len += sprintf(errstr, "[E: 0x%04X] ", i + S_ERROR);
                        } else {
                            omit ++;
                            len += sprintf(errstr, "[O: 0x%04X] ", i + S_ERROR);
                        }
                        break;
                    case S_GUN_2_PYH_CONN_DOWN:
                    case S_GUN_2_SW_BREAK:
                        if ( task->can_charge_gun_sn == GUN_SN1 && true_express(i) ) {
                            fault ++;
                            len += sprintf(errstr, "[E: 0x%04X] ", i + S_ERROR);
                        } else {
                            omit ++;
                            len += sprintf(errstr, "[O: 0x%04X] ", i + S_ERROR);
                        }
                        break;
                    default:
                        if ( false_express(i) ) { // 非关键
                            warn ++;
                            len += sprintf(errstr, "[W: 0x%04X] ", i + S_ERROR);
                        } else if ( true_express(i) ) { // 关键
                            fault ++;
                            len += sprintf(errstr, "[E: 0x%04X] ", i + S_ERROR);
                        } else { // 可忽略
                            omit ++;
                            len += sprintf(errstr, "[O: 0x%04X] ", i + S_ERROR);
                        }
                        break;
                    }
                }
            }

            if ( fault + warn + omit ) {
                log_printf(WRN, "故障代码： %s", errstr);
            }

            if ( fault + warn ) {
                bit_set(task, S_ERROR);
            } else {
                bit_clr(task, S_ERROR);
            }

            // 因故障无法充电原因参考文档 充电桩相关信息.xlsx 充电桩故障对照表
            if ( bit_read(task, S_ERROR) && fault ) {
                log_printf(WRN, "ZEUS: 系统禁止充电(故障).");
                charging --;
            }

            if ( !bit_read(task, F_SYSTEM_CHARGE_ALLOW) ) {
                log_printf(WRN, "ZEUS: 系统禁止充电(条件).");
                charging --;
            }

            if ( bit_read(task, S_ASSIT_POWER_DOWN) ) {
                bit_clr(task, CMD_GUN_1_ASSIT_PWN_ON);
                bit_clr(task, CMD_GUN_2_ASSIT_PWN_ON);
            } else {
                if ( task->can_charge_gun_sn == GUN_SN0 ) {
                }
                if ( task->can_charge_gun_sn == GUN_SN1 ) {
                }
            }

            if ( charging >= 0 ) {

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
#endif
        deal_with_system_protection(task);
        if ( task && task->this_job ) {
            deal_with_job_business(task);
        }

        usleep(5000);
    }

    return NULL;
}

static inline int __is_gun_phy_conn_ok(struct charge_task *thiz)
{
    if ( ! thiz->this_job ) return GUN_INVALID;
    if ( thiz->this_job->job_gun_sn == GUN_SN0 ) {
        if ( bit_read(thiz, F_GUN_1_PHY_CONN_STATUS) ) {
            return GUN_SN0;
        } else return GUN_INVALID;
    } else if ( thiz->this_job->job_gun_sn == GUN_SN1 ) {
        if ( bit_read(thiz, F_GUN_2_PHY_CONN_STATUS) ) {
            return GUN_SN1;
        } else return GUN_INVALID;
    } else if ( thiz->this_job->job_gun_sn == GUN_UNDEFINE ) {
        return GUN_UNDEFINE;
    } else {
        return GUN_INVALID;
    }
}

void deal_with_system_protection(struct charge_task *thiz)
{
}

void deal_with_job_business(struct charge_task *thiz)
{

}
