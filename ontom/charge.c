#include "stdafx.h"

struct charge_task tom;

// 充电任务结构
struct charge_task *task = &tom;
void deal_with_system_protection(struct charge_task *tsk, struct charge_job *thiz);
void deal_with_job_business(struct charge_task *, struct charge_job *);

/*
 * 扩展测量数据刷新
 * 刷新数据，标记
 */
void deal_with_measure_data(struct charge_task *thiz)
{
}

int sql_db_config_result(void *param, int nr, char **text, char **name)
{
    int i = 0;

    if ( nr > 0 && text ) {
        log_printf(INF, "ZEUS: DB READ [%s=%s]", text[1], text[3]);
        if ( 0 == strcmp(text[1], "sys_conflict_map") ) {
            int x = 0, y = 0;
            i = 0;
            while (text[3][i]) {
                if ( x > CONFIG_SUPPORT_BMS_NR ||
                     y > CONFIG_SUPPORT_BMS_NR ) {
                    task->sys_conflict_map[0][0] = true;
                    task->sys_conflict_map[0][1] = false;
                    task->sys_conflict_map[1][0] = false;
                    task->sys_conflict_map[1][1] = true;
                    log_printf(WRN, "ZEUS: 输出据初始化错误，充电枪冲突配置异常. 使用默认值");
                    break;
                }
                if ( text[3][i] == 'C' ) {
                    task->sys_conflict_map[y][x++] = false;
                }
                if ( text[3][i] == 'N' ) {
                    task->sys_conflict_map[y][x++] = true;
                }
                if ( text[3][i] == ';' ) {
                    y ++;
                    x = 0;
                }
                i ++;
            }
        }else if( 0 == strcmp(text[1], "sys_config_gun_nr") ){
            int nr = atoi(text[3]);
            int ma = atoi(text[5]);
            int mi = atoi(text[6]);
            if ( ma < 0 || mi < 0 || ma < mi || nr > ma || nr < mi ) {
                log_printf(WRN, "ZEUS: 数据库初始化失败，充电枪个数异常，使用默认值");
                task->sys_config_gun_nr = 2;
                goto _done;
            }
            task->sys_config_gun_nr = nr;
        }else if( 0 == strcmp(text[1], "sys_can_name") ){
            int x = 0, y = 0;
            i = 0;
            memset(task->sys_can_name, 0, sizeof(task->sys_can_name));
            while ( text[3][i] && i < (int)sizeof(task->sys_can_name) ) {
                if ( text[3][i] == ';' ) {
                    x = 0;
                    y ++;
                } else {
                    task->sys_can_name[y][x++] = text[3][i];
                }
                i ++;
            }
            if ( ! task->sys_can_name[0][0] ) {
                log_printf(WRN, "ZEUS: 数据库初始化失败，CAN设备文件名异常，使用默认值");
                memset(task->sys_can_name, 0, sizeof(task->sys_can_name));
                strcpy(task->sys_can_name[0], "can0");
            }
        }else if( 0 == strcmp(text[1], "sys_simple_box_nr") ){
            int nr = atoi(text[3]);
            int ma = atoi(text[5]);
            int mi = atoi(text[6]);
            if ( ma < 0 || mi < 0 || ma < mi || nr > ma || nr < mi ) {
                log_printf(WRN, "ZEUS: 数据库初始化失败，采样盒个数异常，使用默认值");
                task->sys_simple_box_nr = 1;
                goto _done;
            }
            task->sys_simple_box_nr = nr;
        }else if( 0 == strcmp(text[1], "sys_charge_group_nr") ){
            int nr = atoi(text[3]);
            int ma = atoi(text[5]);
            int mi = atoi(text[6]);
            if ( ma < 0 || mi < 0 || ma < mi || nr > ma || nr < mi ) {
                log_printf(WRN, "ZEUS: 数据库初始化失败，充电机组数异常，使用默认值");
                task->sys_charge_group_nr = 1;
                goto _done;
            }
            task->sys_charge_group_nr = nr;
        }else if( 0 == strcmp(text[1], "sys_rs485_dev_nr") ){
            int nr = atoi(text[3]);
            int ma = atoi(text[5]);
            int mi = atoi(text[6]);
            if ( ma < 0 || mi < 0 || ma < mi || nr > ma || nr < mi ) {
                log_printf(WRN, "ZEUS: 数据库初始化失败，RS485设备个数异常，使用默认值");
                task->sys_rs485_dev_nr = 2;
                goto _done;
            }
            task->sys_rs485_dev_nr = nr;
        }else if( 0 == strcmp(text[1], "sys_uart_name") ){
            int x = 0, y = 0;
            i = 0;
            memset(task->sys_can_name, 0, sizeof(task->sys_can_name));
            while ( text[3][i] && i < (int)sizeof(task->sys_can_name) ) {
                if ( text[3][i] == ';' ) {
                    x = 0;
                    y ++;
                } else {
                    task->sys_uart_name[y][x++] = text[3][i];
                }
                i ++;
            }
            if ( ! task->sys_uart_name[0][0] ) {
                log_printf(WRN, "ZEUS: 数据库初始化失败，RS485设备文件名异常，使用默认值");
                memset(task->sys_uart_name, 0, sizeof(task->sys_uart_name));
                strcpy(task->sys_uart_name[0], "/dev/ttyO4");
                strcpy(task->sys_uart_name[1], "/dev/ttyO5");
            }
        } else {
        }
    }
_done:
    (*(int *)param) ++;
    return 0;
}

void print_POST_configure()
{
    unsigned int x = 0, y;
    log_printf(INF, "ZEUS: 读取数据库配置数据完成， 清单如下：");
    printf("-----------------------BEGIN--------------------------\n");
    printf("  * 充电枪个数:  %4d  把\n", task->sys_config_gun_nr);
    printf("  * 充电冲突对照表:\n");
    if ( task->sys_config_gun_nr ) {
        printf("\t\t", task->sys_config_gun_nr);
        for ( x = 0; x < task->sys_config_gun_nr; x ++ ) {
            printf("\t%02d#\t", x + 1);
        }
        printf("\n");
    }
    for ( y = 0; y < task->sys_config_gun_nr; y ++ ) {
        printf("\t%02d#\t", y + 1);
        for ( x = 0; x < task->sys_config_gun_nr; x ++ ) {
            if ( task->sys_conflict_map[y][x] ) {
                printf("\t"GRN("兼容")"\t");
            } else {
                printf("\t"RED("冲突")"\t");
            }
        }
        printf("\n");
    }
    printf("  * %16s  %4d  个\n", "采样盒个数:", task->sys_simple_box_nr);
    printf("  * %16s  %4d  组\n", "充电机组数:", task->sys_charge_group_nr);
    printf("  * %13s  %4d  个\n", "RS485个数:", task->sys_rs485_dev_nr);
    printf("-----------------------END---------------------------\n");
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
    char sql[256];
    char buff[32] = {0};

    int ret, done = 0;
    char *errmsg;

    log_printf(INF, "ZUES: %s running...sizeof(struct charge_task)=%d",
            __FUNCTION__, sizeof(struct charge_task));

    sprintf(sql, "SELECT * FROM configs");
    ret = sqlite3_exec(task->database, sql, sql_db_config_result, &done, &errmsg);
    if ( ret ) {
        log_printf(ERR, "TOM: SQL error: %s", errmsg);
    }

    while ( ! done ) {
        log_printf(INF, "ZEUS: 等待数据库初始化完成...." );
    }

    log_printf(INF, "ZEUS: 数据库初始化完成....%d", done);
    print_POST_configure();

    // 启动八公定时器
    Hachiko_init();

    /*  只有一组充电机
     * 最多配置两把枪，且两把枪必须是互斥的，即不能同时进行充电操作
     * 采样盒需要一个
     */
    if ( task->sys_charge_group_nr == 1 ) {
        void *tp = malloc(sizeof(void*) * (1 + 1 + 1));
        if ( tp == NULL ) {
            ret = ERR_LOW_MEMORY;
            goto panic;
        }
    }

    // 两组充电机
    if ( task->sys_charge_group_nr == 2 ) {
    }

    while ( 1 );

    task->nr_jobs = 0;
    task->this_job[0] = NULL;
    task->uarts[0].bp_evt_handle = uart4_bp_evt_handle;
    task->uarts[0].dev_handle = -1;
    strcpy(task->uarts[0].dev_name, "/dev/ttyO4");
    task->uarts[0].hw_port = SERIAL4_CTRL_PIN;
    task->uarts[1].bp_evt_handle = NULL;
    task->uarts[1].dev_handle = -1;
    strcpy(task->uarts[1].dev_name, "/dev/ttyO5");
    task->uarts[1].hw_port = SERIAL5_CTRL_PIN;

    while ( 1 ) {
        // 扩展测量数据刷新
        deal_with_measure_data(task);
        if ( task && task->this_job[0] ) {
            deal_with_system_protection(task, task->this_job[0]);
            deal_with_job_business(task, task->this_job[0]);
        }

        usleep(5000);
    }
panic:
    log_printf(ERR, "ZEUS: 关键错误，系统退出 %d", ret);
    __get_timestamp(buff);
    sprintf(sql, "INSERT INTO log VALUES('%s', '关键错误，系统退出 %d", buff, ret);
    sqlite3_exec(task->database, sql, NULL, NULL, NULL);
    sqlite3_close(task->database);
    exit(ret);
    return NULL;
}

void deal_with_system_protection(struct charge_task *tsk, struct charge_job *thiz)
{
    int fault_nr = 0; // 系统关键故障
    int err_nr = 0;   // 系统故障数

    if ( bit_read(thiz, S_AC_INPUT_DOWN) ) {
        fault_nr ++;
        error_history_begin(thiz, S_AC_INPUT_DOWN, "N/A");
    } else {
        error_history_recover(thiz, S_AC_INPUT_DOWN);
    }
    if ( bit_read(thiz, S_ASSIT_POWER_DOWN) ) {
        fault_nr ++;
        error_history_begin(thiz, S_ASSIT_POWER_DOWN, "N/A");
    } else {
        error_history_recover(thiz, S_ASSIT_POWER_DOWN);
    }
    if ( bit_read(thiz, S_CHARGER_COMM_DOWN) ) {
        fault_nr ++;
        error_history_begin(thiz, S_CHARGER_COMM_DOWN, "N/A");
    } else {
        error_history_recover(thiz, S_CHARGER_COMM_DOWN);
    }
    if ( bit_read(thiz, S_CHARGER_YX_1_COMM_DOWN) ) {
        err_nr ++;
        error_history_begin(thiz, S_CHARGER_YX_1_COMM_DOWN, "N/A");
    } else {
        error_history_recover(thiz, S_CHARGER_YX_1_COMM_DOWN);
    }
    if ( bit_read(thiz, S_CHARGER_YX_2_COMM_DOWN) ) {
        err_nr ++;
        error_history_begin(thiz, S_CHARGER_YX_2_COMM_DOWN, "N/A");
    } else {
        error_history_recover(thiz, S_CHARGER_YX_2_COMM_DOWN);
    }
    if ( bit_read(thiz, S_MEASURE_COMM_DOWN) ) {
        fault_nr ++;
        error_history_begin(thiz, S_MEASURE_COMM_DOWN, "N/A");
    } else {
        error_history_recover(thiz, S_MEASURE_COMM_DOWN);
    }
    if ( bit_read(thiz, S_BMS_COMM_DOWN) ) {
        fault_nr ++;
        error_history_begin(thiz, S_BMS_COMM_DOWN, "N/A");
    } else {
        error_history_recover(thiz, S_BMS_COMM_DOWN);
    }

    if ( bit_read(thiz, S_CHARGE_MODULE_DOWN) ) {
        err_nr ++;
        error_history_begin(thiz, S_CHARGE_MODULE_DOWN, "N/A");
    } else {
        error_history_recover(thiz, S_CHARGE_MODULE_DOWN);
    }
    // 1# 充电枪物理连接故障
    if ( bit_read(thiz, S_GUN_1_PYH_CONN_DOWN) ) {
        err_nr ++;
        error_history_begin(thiz, S_GUN_1_PYH_CONN_DOWN, "N/A");
    } else {
        error_history_recover(thiz, S_GUN_1_PYH_CONN_DOWN);
    }
    // 2# 充电枪物理连接故障
    if ( bit_read(thiz, S_GUN_2_PYH_CONN_DOWN) ) {
        err_nr ++;
        error_history_begin(thiz, S_GUN_2_PYH_CONN_DOWN, "N/A");
    } else {
        error_history_recover(thiz, S_GUN_2_PYH_CONN_DOWN);
    }
    // 绝缘故障
    if ( bit_read(thiz, S_INSTITUDE_ERR) ) {
        err_nr ++;
        error_history_begin(thiz, S_INSTITUDE_ERR, "N/A");
    } else {
        error_history_recover(thiz, S_INSTITUDE_ERR);
    }
    // 母线过压
    if ( bit_read(thiz, S_BUS_V_HI) ) {
        err_nr ++;
        error_history_begin(thiz, S_BUS_V_HI, "N/A");
    } else {
        error_history_recover(thiz, S_BUS_V_HI);
    }
    // 母线欠压
    if ( bit_read(thiz, S_BUS_V_LO) ) {
        err_nr ++;
        error_history_begin(thiz, S_BUS_V_LO, "N/A");
    } else {
        error_history_recover(thiz, S_BUS_V_LO);
    }
    // 母线短路
    if ( bit_read(thiz, S_BUS_SHORTED) ) {
        err_nr ++;
        error_history_begin(thiz, S_BUS_SHORTED, "N/A");
    } else {
        error_history_recover(thiz, S_BUS_SHORTED);
    }
    // 电池过压
    if ( bit_read(thiz, S_BAT_V_HI) ) {
        err_nr ++;
        error_history_begin(thiz, S_BAT_V_HI, "N/A");
    } else {
        error_history_recover(thiz, S_BAT_V_HI);
    }
    // 电池欠压
    if ( bit_read(thiz, S_BAT_V_LO) ) {
        err_nr ++;
        error_history_begin(thiz, S_BAT_V_LO, "N/A");
    } else {
        error_history_recover(thiz, S_BAT_V_LO);
    }
    // 电池短路
    if ( bit_read(thiz, S_BAT_SHORTED) ) {
        err_nr ++;
        error_history_begin(thiz, S_BAT_SHORTED, "N/A");
    } else {
        error_history_recover(thiz, S_BAT_SHORTED);
    }
    // 电池反接故障
    if ( bit_read(thiz, S_BAT_REVERT_CONN) ) {
        err_nr ++;
        error_history_begin(thiz, S_BAT_REVERT_CONN, "N/A");
    } else {
        error_history_recover(thiz, S_BAT_REVERT_CONN);
    }
    // 电池过流
    if ( bit_read(thiz, S_BAT_I_HI) ) {
        err_nr ++;
        error_history_begin(thiz, S_BAT_I_HI, "N/A");
    } else {
        error_history_recover(thiz, S_BAT_I_HI);
    }
    // 充电桩温度过高
    if ( bit_read(thiz, S_CHARGE_BOX_TEMP_HI) ) {
        err_nr ++;
        error_history_begin(thiz, S_CHARGE_BOX_TEMP_HI, "N/A");
    } else {
        error_history_recover(thiz, S_CHARGE_BOX_TEMP_HI);
    }
    // 充电桩温度过低
    if ( bit_read(thiz, S_CHARGE_BOX_TEMP_LO) ) {
        err_nr ++;
        error_history_begin(thiz, S_CHARGE_BOX_TEMP_LO, "N/A");
    } else {
        error_history_recover(thiz, S_CHARGE_BOX_TEMP_LO);
    }
    // 充电桩湿度过高
    if ( bit_read(thiz, S_CHARGE_BOX_WET_HI) ) {
        err_nr ++;
        error_history_begin(thiz, S_CHARGE_BOX_WET_HI, "N/A");
    } else {
        error_history_recover(thiz, S_CHARGE_BOX_WET_HI);
    }
    // 充电桩湿度过低
    if ( bit_read(thiz, S_CHARGE_BOX_WET_LO) ) {
        err_nr ++;
        error_history_begin(thiz, S_CHARGE_BOX_WET_LO, "N/A");
    } else {
        error_history_recover(thiz, S_CHARGE_BOX_WET_LO);
    }
    // 总输出熔断器熔断
    if ( bit_read(thiz, S_DC_RDQ_BREAK) ) {
        err_nr ++;
        error_history_begin(thiz, S_DC_RDQ_BREAK, "N/A");
    } else {
        error_history_recover(thiz, S_DC_RDQ_BREAK);
    }
    // 总输出开关跳闸
    if ( bit_read(thiz, S_DC_SW_BREAK) ) {
        err_nr ++;
        error_history_begin(thiz, S_DC_SW_BREAK, "N/A");
    } else {
        error_history_recover(thiz, S_DC_SW_BREAK);
    }
    // 1#枪输出开关跳闸
    if ( bit_read(thiz, S_GUN_1_SW_BREAK) ) {
        err_nr ++;
        error_history_begin(thiz, S_GUN_1_SW_BREAK, "N/A");
    } else {
        error_history_recover(thiz, S_GUN_1_SW_BREAK);
    }
    // 2#枪输出开关跳闸
    if ( bit_read(thiz, S_GUN_2_SW_BREAK) ) {
        err_nr ++;
        error_history_begin(thiz, S_GUN_2_SW_BREAK, "N/A");
    } else {
        error_history_recover(thiz, S_GUN_2_SW_BREAK);
    }
    // 防雷器故障
    if ( bit_read(thiz, S_FANGLEIQI_BREAK) ) {
        err_nr ++;
        error_history_begin(thiz, S_FANGLEIQI_BREAK, "N/A");
    } else {
        error_history_recover(thiz, S_FANGLEIQI_BREAK);
    }

    if ( fault_nr ) {
        bit_set(thiz, F_SYSTEM_CHARGE_ALLOW);
    } else {
        bit_clr(thiz, F_SYSTEM_CHARGE_ALLOW);
    }

    if ( fault_nr + err_nr ) {
        bit_set(thiz, S_ERROR);
    } else {
        bit_clr(thiz, S_ERROR);
    }
}

void deal_with_job_business(struct charge_task *tsk, struct charge_job *thiz)
{
    int ret;

    if ( thiz == NULL ) return;

    switch ( thiz->job_status ) {
    case JOB_IDLE:
    case JOB_SETTING:
    case JOB_WAITTING:
        bit_clr(thiz, CMD_DC_OUTPUT_SWITCH_ON);
        bit_clr(thiz, CMD_GUN_1_OUTPUT_ON);
        bit_clr(thiz, CMD_GUN_2_OUTPUT_ON);
        break;
    case JOB_STANDBY:
        bit_clr(thiz, CMD_DC_OUTPUT_SWITCH_ON);
        bit_clr(thiz, CMD_GUN_1_OUTPUT_ON);
        bit_clr(thiz, CMD_GUN_2_OUTPUT_ON);
        ret = __is_gun_phy_conn_ok(thiz);
        if ( ret == GUN_UNDEFINE || ret == GUN_INVALID ) {
            break;
        }
        if ( ret == GUN_SN0 ) {
            if ( ! bit_read(thiz, F_GUN_1_ASSIT_PWN_SWITCH_STATUS) ) {
                if ( !bit_read(thiz, S_ASSIT_POWER_DOWN) ) {
                    if ( ! bit_read(thiz, CMD_GUN_1_ASSIT_PWN_ON) ) {
                        log_printf(INF, "ZEUS: 1# 充电枪辅助电源开始供电");
                    }
                    bit_set(thiz, CMD_GUN_1_ASSIT_PWN_ON);
                }
            }
        } else {
            if ( ! bit_read(thiz, F_GUN_2_ASSIT_PWN_SWITCH_STATUS) ) {
                if ( !bit_read(thiz, S_ASSIT_POWER_DOWN) ) {
                    if ( ! bit_read(thiz, CMD_GUN_2_ASSIT_PWN_ON) ) {
                        log_printf(INF, "ZEUS: 2# 充电枪辅助电源开始供电");
                    }
                    bit_set(thiz, CMD_GUN_2_ASSIT_PWN_ON);
                }
            }
        }
        if ( ! bit_read(thiz, F_BMS_RECOGNIZED) ) {
            break;
        }
        if ( ! bit_read(thiz, F_SYSTEM_CHARGE_ALLOW) ) {
            thiz->job_status = thiz->job_status;
            thiz->job_status = JOB_ERR_PAUSE;
            log_printf(WRN, "ZEUS: 系统发生关键故障, 自动暂停作业(%X)",
                       thiz->job_status);
            break;
        }
        if ( ! bit_read(thiz, F_CARDING_TRIGER) ) {
            break;
        }
        if ( ! bit_read(thiz, F_CARDING_CONFIRM) ) {
            break;
        }
        thiz->job_status = JOB_WORKING;
        log_printf(INF, "***** ZEUS(关键): 作业转为正式开始执行, 正在执行.");
        break;
    case JOB_WORKING:
        if ( ! bit_read(thiz, F_SYSTEM_CHARGE_ALLOW) ) {
            bit_clr(thiz, CMD_DC_OUTPUT_SWITCH_ON);
            bit_clr(thiz, CMD_GUN_1_OUTPUT_ON);
            bit_clr(thiz, CMD_GUN_2_OUTPUT_ON);
            thiz->status_befor_fault = JOB_WORKING;
            thiz->job_status = JOB_ERR_PAUSE;
            log_printf(WRN, "ZEUS: 系统发生关键故障, 自动暂停作业(JOB_WORKING)");
            break;
        } else {
            bit_set(thiz, CMD_DC_OUTPUT_SWITCH_ON);
            ret = __is_gun_phy_conn_ok(thiz);
            if ( ret  == GUN_SN0 ) {
                bit_clr(thiz, CMD_GUN_2_OUTPUT_ON);
                bit_set(thiz, CMD_GUN_1_OUTPUT_ON);
            }
            if ( ret  == GUN_SN1 ) {
                bit_clr(thiz, CMD_GUN_1_OUTPUT_ON);
                bit_set(thiz, CMD_GUN_2_OUTPUT_ON);
            }
            //{{ 在这做是否充完判定
            //}}
            if ( bit_read(thiz, CMD_JOB_ABORT) ) {
                bit_clr(thiz, CMD_JOB_ABORT);
                thiz->status_befor_fault = JOB_WORKING;
                thiz->job_status = JOB_ABORTING;
                log_printf(INF, "***** ZEUS(关键): 作业中止(人为), 正在中止");
                break;
            }
            if ( bit_read(thiz, CMD_JOB_MAN_PAUSE) ) {
                thiz->status_befor_fault = JOB_WORKING;
                thiz->job_status = JOB_MAN_PAUSE;
                log_printf(WRN, "ZEUS: 人工暂停作业(JOB_WORKING)");
                break;
            }
        }
        break;
    case JOB_ERR_PAUSE:
        bit_clr(thiz, CMD_GUN_1_OUTPUT_ON);
        bit_clr(thiz, CMD_GUN_2_OUTPUT_ON);
        if ( ! bit_read(thiz, F_SYSTEM_CHARGE_ALLOW) ) {
            if ( bit_read(thiz, CMD_JOB_ABORT) ) {
                bit_clr(thiz, CMD_JOB_ABORT);
                log_printf(INF, "ZEUS: 充电任务中止(%X)",
                           thiz->status_befor_fault);
                thiz->job_status = JOB_ABORTING;
            }
        } else {
            thiz->job_status = JOB_RESUMING;
            log_printf(INF, "ZEUS: 故障消除, 充电作业继续进行(%X)",
                       thiz->status_befor_fault);
        }
        break;
    case JOB_MAN_PAUSE:
        bit_clr(thiz, CMD_GUN_1_OUTPUT_ON);
        bit_clr(thiz, CMD_GUN_2_OUTPUT_ON);
        if ( ! bit_read(thiz, F_SYSTEM_CHARGE_ALLOW) ) {
            if ( bit_read(thiz, CMD_JOB_ABORT) ) {
                bit_clr(thiz, CMD_JOB_ABORT);
                log_printf(INF, "ZEUS: 充电任务中止(%X:JOB_MAN_PAUSE)",
                           thiz->status_befor_fault);
                thiz->job_status = JOB_ABORTING;
            }
        } else {
            if ( bit_read(thiz, CMD_JOB_MAN_PAUSE) ) {
                break;
            }
            thiz->job_status = JOB_RESUMING;
            log_printf(INF, "ZEUS: 人工恢复作业(%X), 正在恢复",
                       thiz->status_befor_fault);
        }
        break;
    case JOB_RESUMING:
        break;
    case JOB_ABORTING:
        bit_clr(thiz, CMD_GUN_1_OUTPUT_ON);
        bit_clr(thiz, CMD_GUN_2_OUTPUT_ON);
        break;
    case JOB_DONE:
        break;
    case JOB_EXITTING:
        bit_clr(thiz, CMD_GUN_1_OUTPUT_ON);
        bit_clr(thiz, CMD_GUN_2_OUTPUT_ON);
        break;
    case JOB_DETACHING:
        bit_clr(thiz, CMD_DC_OUTPUT_SWITCH_ON);
        bit_clr(thiz, CMD_GUN_1_OUTPUT_ON);
        bit_clr(thiz, CMD_GUN_2_OUTPUT_ON);
        thiz->job_status = JOB_IDLE;
        thiz = NULL;
        break;
    }
}

unsigned int error_history_begin(struct charge_job *job, unsigned int error_id, char *error_string)
{
    struct error_history *thiz;
    struct list_head *head;
    char sql[128], errname[32], timestamp[20];
    int ret;

    pthread_mutex_lock(&job->err_list_lck);
    if ( job->err_head != NULL ) {
        head = job->err_head;
        do {
            thiz = list_load(struct error_history, error_me, head);
            if ( thiz->error_id == error_id ) {
                goto out;
            }
            head = head->next;
        } while ( head != job->err_head );
    }

    thiz = (struct error_history*)malloc(sizeof(struct error_history));
    if ( thiz == NULL ) goto out;
    list_ini(thiz->error_me);
    if ( job->err_head == NULL ) {
        job->err_head = & thiz->error_me;
    } else {
        list_inserttail(job->err_head, &thiz->error_me);
    }
    job->err_nr ++;
    thiz->error_seqid = task->err_seq_id_next ++;
    thiz->error_id = error_id;
    strncpy(thiz->error_string, error_string, 32);
    strcpy(thiz->error_recover, "0000-00-00 00:00:00");

    log_printf(INF, "ZEUS: 故障总数为: %d", job->err_nr);

    __get_timestamp(timestamp);
    strcpy(thiz->error_begin, timestamp);
    sprintf(errname, "E%04X", thiz->error_id);
    sprintf(sql, "INSERT INTO errors VALUES('%d','E%04X','%s','%s','ERROR','%s')",
            thiz->error_seqid,
            thiz->error_id,
            thiz->error_begin,
            thiz->error_recover,
            config_read(errname));
    ret = sqlite3_exec(task->database, sql, NULL, NULL, NULL);
    log_printf(INF, "ZEUS: %s:%d", sql, ret);
out:
    pthread_mutex_unlock (&job->err_list_lck);

    return error_id;
}

void error_history_recover(struct charge_job *job, unsigned int error_id)
{
    struct error_history *thiz;
    struct list_head *head;
    char sql[128], errname[32], timestamp[20];
    int ret;

    pthread_mutex_lock(&job->err_list_lck);

    if ( job->err_head == NULL ) goto out;

    head = job->err_head;
    do {
        thiz = list_load(struct error_history, error_me, head);
        if ( thiz->error_id == error_id ) {
            goto del;
        }
        head = head->next;
    } while ( head != job->err_head );
    goto out;
del:
    if ( job->err_head == & thiz->error_me ) {
        job->err_head = thiz->error_me.next;
    }
    list_remove(&thiz->error_me);
    job->err_nr --;
    if ( job->err_nr == 0 ) {
        job->err_head = NULL;
    }
    __get_timestamp(timestamp);
    sprintf(errname, "E%04X", thiz->error_id);
    sprintf(sql,
            "UPDATE errors SET "
            "error_recover='%s',recover_reason='NORMAL' "
            "WHERE error_id='E%04X' AND error_begin='%s'",
            timestamp, thiz->error_id,
            thiz->error_begin);
    ret = sqlite3_exec(task->database, sql, NULL, NULL, NULL);
    log_printf(INF, "ZEUS: %s:%d", sql, ret);
out:
    pthread_mutex_unlock (&job->err_list_lck);
}
