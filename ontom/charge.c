#include "stdafx.h"

struct charge_task tom;

// 充电任务结构
struct charge_task *task = &tom;

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
        log_printf(DBG_LV1, "ZEUS: DB READ [%s=%s]", text[1], text[3]);
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
        } else if ( 0 == strcmp(text[1], "system_type") ) {
            task->sys_type = SYSTEM_FENTISHI;
            if ( 0 == strcmp(text[3], "分体式") ) {
                task->sys_type = SYSTEM_FENTISHI;
            } else if ( 0 == strcmp(text[3], "一体式") ) {
                task->sys_type = SYSTEM_YITISHI;
            } else ;
        } else if( 0 == strcmp(text[1], "limit_output_I") ) {
            task->limit_output_I = atof(text[3]);
        } else if ( 0 == strcmp(text[1], "limit_max_V") ) {
            task->limit_max_V = atof(text[3]);
        } else if( 0 == strcmp(text[1], "limit_min_V") ) {
            task->limit_min_V = atof(text[3]);
        } else if ( 0 == strcmp(text[1], "running_V") ) {
            task->running_V = atof(text[3]);
        } else if( 0 == strcmp(text[1], "modules_nr") ) {
            task->modules_nr = (unsigned short)atoi(text[3]);
        } else if ( 0 == strcmp(text[1], "modules_on_off") ) {
            task->modules_on_off = (unsigned short)atoi(text[3]);
        }
    }
_done:
    (*(int *)param) ++;
    return 0;
}

// 获取下一个查询的数据集个数
int sql_query_result_conter(void *param, int nr, char **text, char **name)
{
    if ( param ) {
        *(int *)param = atoi( text[0] );
    }
    return 0;
}

// 获取BMS数据包生成信息
int sql_query_BMS_pack_gen(void *param, int nr, char **text, char **name)
{
    struct charge_job *thiz = (struct charge_job *)param;
    struct can_pack_generator *me = NULL;
    if ( thiz ) {
        if ( thiz->bms.readed >= thiz->bms.can_pack_gen_nr )
            return 0;
        me = thiz->bms.generator + (thiz->bms.readed ++);
        me->stage = atoh( text[0] );
        me->can_pgn = atoi( text[1] );
        me->prioriy = atoi( text[2] );
        me->datalen = atoi( text[3] );
        me->period = atoi( text[4] );
        me->can_tolerate_silence = atoi( text[5] );
        me->heartbeat = 0;
    }
    return 0;
}

void print_POST_configure()
{
    unsigned int x = 0, y;
    printf("读取数据库配置数据完成， 清单如下：\n");
    printf("-----------------------BEGIN--------------------------\n");
    printf("  * 充电枪个数:  %4d  把\n", task->sys_config_gun_nr);
    printf("  * 充电冲突对照表:\n");
    if ( task->sys_config_gun_nr ) {
        printf("                 ");
        for ( x = 0; x < task->sys_config_gun_nr; x ++ ) {
            printf("  %02d#   ", x + 1);
        }
        printf("\n");
    }
    for ( y = 0; y < task->sys_config_gun_nr; y ++ ) {
        printf("            %02d#  ", y + 1);
        for ( x = 0; x < task->sys_config_gun_nr; x ++ ) {
            if ( task->sys_conflict_map[y][x] ) {
                printf("  "GRN("兼容")"  ");
            } else {
                printf("  "RED("冲突")"  ");
            }
        }
        printf("\n");
    }
    printf("  * %-16s  %4d  个\n", "采样盒个数:", task->sys_simple_box_nr);
    printf("  * %-16s  %4d  组\n", "充电机组数:", task->sys_charge_group_nr);
    printf("  * %-16s  %4d  个\n", "RS485个数:", task->sys_rs485_dev_nr);
    printf("  * %-16s  %s\n"     , "充电方式:",
           task->sys_charge_group_nr==1?
               task->sys_config_gun_nr>1?"分时充电":"独立充电":
                "独立充电");
    printf("-----------------------END---------------------------\n");
}

// 重新获得设置值
int sql_db_settings_result(void *param, int nr, char **text, char **name)
{
    if ( nr <= 0 ) return 0;

    if ( 0 == strcmp(text[0], "system_type") ) {
        task->sys_type = atoi(text[1]);
        log_printf(INF, "ZEUS: 系统类型: %d", task->sys_type);
    } else if ( 0 == strcmp(text[0], "module_kind") ) {
        task->module_model = atoi(text[1]);
        log_printf(INF, "ZEUS: 模块型号: %d", task->module_model);
    } else if ( 0 == strcmp(text[0], "module_count") ) {
        task->modules_nr = atoi(text[1]);
        log_printf(INF, "ZEUS: 模块个数: %d", task->modules_nr);
    } else if ( 0 == strcmp(text[0], "kwh_price") ) {
        task->kwh_price = atof(text[1]);
        log_printf(INF, "ZEUS: 单位电价: %.1f", task->kwh_price);
    }
    return 0;
}

// 添加串口使用模组
int sql_rs485_result(void *param, int nr, char **text, char **name) {
    const struct binder_proc {
        char *id;
        int (*user_evt_handle)(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                               struct bp_evt_param *param);
    }plugins[] = {
        {"00000001", uart4_simple_box_1_evt_handle},
        {"00000002", card_reader_handle},
        {"00000003", card_init_handle},
        {"00000004", uart4_charger_config_evt_handle},
        {"00000005", uart4_charger_yaoce_0_49_handle},
        {"00000006", uart4_charger_yaoce_50_100_handle},
        {"00000007", ANC01_convert_box_read_evt_handle},
        {"00000008", ANC01_convert_box_write_evt_handle},
        {"00000009", Increase_convert_box_write_evt_handle},
        {"0000000A", Increase_convert_box_read_evt_handle},
        {"0000000B", kwh_meter_read_evt_handle},
        {"0000000C", voltage_meter_read_evt_handle},
        {"0000000D", uart4_simple_box_correct_evt_handle},
        {"0000000E", uart4_simple_box_write_evt_handle},
        {NULL, NULL}
    };
    struct bp_user u;
    struct bp_uart *bp = NULL;
    int i = 0;
    struct charge_task *t = (struct charge_task *)param;
    int d, p, s, rs485;
    struct binder_proc *binder;

    if ( nr <= 0 ) return 0;
    // "id, name, private, RS485, bps, parity, datalen, stop_bit, frame_freq, "
    // "died_line, swap2rx_time, ttw"
    memset(&u, 0, sizeof(struct bp_user));

    for ( i = 0; plugins[i].id != NULL; i ++ ) {
        if ( 0 != strcmp(plugins[i].id, text[0]) ) continue;
        u.user_evt_handle = plugins[i].user_evt_handle;
    }
    if ( u.user_evt_handle == NULL ) {
        log_printf(WRN, "ZEUS: 不能绑定串口模组， 名称为%s 类为%s的模组找到.", text[1], text[0]);
        return 0;
    }
    strncpy(u.name, text[1], 32);
    u._private = atoi(text[2]);
    rs485 = atoi(text[3]);
    u.hw_bps = atoi(text[4]);
    d = atoi(text[6]);
    p = text[5][0];
    s = atoi(text[7]);
    u.hw_other = MAKE_UART_CFG(d, p, s);
    u.frame_freq = atoi(text[8]);
    u.died_line = atoi(text[9]);
    u.swap_time_modify = atoi(text[10]);
    u.ttw = atoi(text[11]);

    for ( i = 0; i < 2; i ++ ) {
        if ( rs485 == 4 && t->uarts[i]->hw_port == SERIAL4_CTRL_PIN ) {
            u.uart = t->uarts[i];
            bp = t->uarts[i];
        } else if ( rs485 ==5 && t->uarts[i]->hw_port == SERIAL5_CTRL_PIN) {
            u.uart = t->uarts[i];
            bp = t->uarts[i];
        }
    }
    if ( bp == NULL ) {
        log_printf(WRN, "ZEUS: 不能绑定串口模组[%s]到串口%d.", text[1], rs485);
        return 0;
    }

    u.chargers = t->chargers[0];
    u.measure = t->measure[0];
    u.user_evt_handle = uart4_simple_box_1_evt_handle;

    log_printf(INF, "ZEUS: 绑定串口模组[ %s.%-32s ] <==>  %s.",
               text[0], text[1], bp->dev_name);

    bp_user_bind(bp, &u); // 采样读
    return 0;
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

    log_printf(DBG_LV1, "ZUES: %s running...sizeof(struct charge_task)=%d",
            __FUNCTION__, sizeof(struct charge_task));
    // 恢复默认配置
    task->max_output_I = 10.0f; // 10.0A
    task->limit_output_I = 0.0f;
    task->limit_max_V = 750.0f;
    task->limit_min_V = 400.0f;
    task->running_V = 450.0f;
    task->running_I = 0.0;
    task->modules_nr = 14;
    task->charge_stat = 0x0000; // 不充电
    task->modules_on_off = 0x0000; // 全开机
    task->uipage = UI_PAGE_MAIN;

    sprintf(sql, "SELECT * FROM configs");
    ret = sqlite3_exec(task->database, sql, sql_db_config_result, &done, &errmsg);
    if ( ret ) {
        log_printf(ERR, "TOM: SQL error: %s", errmsg);
    }
    sprintf(sql, "SELECT key,current_value FROM settings");
    ret = sqlite3_exec(task->database, sql, sql_db_settings_result, &done, &errmsg);
    if ( ret ) {
        log_printf(ERR, "TOM: SQL error: %s", errmsg);
    }

    log_printf(DBG_LV1, "ZEUS: 数据库初始化完成....%d", done);
    print_POST_configure();

    // 启动八公定时器
    Hachiko_init();

    /* 方案1：
     *   一组充电机， 一个采样盒，两把枪
     *   两把枪分时充电
     *
     *  串口服务线程:   2 个{电度表,[采样盒,充电机] }
     *    暂时先忽略电度表线程,后期可能会添加串口IO方式的读卡器
     */
    if ( task->sys_charge_group_nr == 1 &&
         task->sys_simple_box_nr == 1   &&
         task->sys_config_gun_nr == 2 ) {
        struct bp_uart * bp = (struct bp_uart*)malloc(sizeof(struct bp_uart));
        if ( NULL == bp ) {
            ret = ERR_LOW_MEMORY;
            log_printf(ERR, "ZEUS: 分配系统内存失败");
            goto __panic;
        }
        task->uarts[0] = bp;
        memset(bp, 0, sizeof(struct bp_uart));
        bp->bp_evt_handle = uart4_bp_evt_handle;
        if ( ! task->sys_uart_name[0][0] ) {
            log_printf(WRN, "ZEUS: 未配置采样盒通信用RS485设备文件, 使用默认值.");
            strcpy(task->sys_uart_name[0], "/dev/ttyO4");
        }
        strcpy(bp->dev_name, task->sys_uart_name[0]);
        bp->dev_handle = -1;
        bp->status = BP_UART_STAT_INVALID;
        bp->hw_status = BP_UART_STAT_INVALID;
        bp->role = BP_UART_MASTER;
        bp->init_magic = 0;
        bp->hw_port = SERIAL4_CTRL_PIN;
        bp->users_nr = 0;
        bp->hw_bps = 9600;
        bp->hw_other = MAKE_UART_CFG(8, 'N', 1);
        bit_set(task, F_SYSTEM_CHARGE_ALLOW);

        task->chargers[0] = (struct charger_struct *)malloc(sizeof(struct charger_struct));
        task->measure[0] = (struct measure_struct *)malloc(sizeof(struct measure_struct));
        task->sys_type = SYSTEM_YITISHI;
    }

    do {
        struct bp_uart * bp = (struct bp_uart*)malloc(sizeof(struct bp_uart));
        if ( NULL == bp ) {
            ret = ERR_LOW_MEMORY;
            log_printf(ERR, "ZEUS: 分配系统内存失败");
            goto __panic;
        }
        task->uarts[1] = bp;
        memset(bp, 0, sizeof(struct bp_uart));
        bp->bp_evt_handle = uart4_bp_evt_handle;
        if ( ! task->sys_uart_name[1][0] ) {
            log_printf(WRN, "ZEUS: 未配置电表/读卡器通信用RS485设备文件, 使用默认值.");
            strcpy(task->sys_uart_name[1], "/dev/ttyO5");
        }
        strcpy(bp->dev_name, task->sys_uart_name[1]);
        bp->dev_handle = -1;
        bp->status = BP_UART_STAT_INVALID;
        bp->hw_status = BP_UART_STAT_INVALID;
        bp->role = BP_UART_MASTER;
        bp->init_magic = 0;
        bp->hw_port = SERIAL5_CTRL_PIN;
        bp->hw_bps = 2400;
        bp->hw_other = MAKE_UART_CFG(8, 'O', 1);
    } while ( 0 );

    sprintf(sql,
            "SELECT "
            "id, name, private, RS485, bps, parity, datalen, stop_bit, frame_freq, "
            "died_line, swap2rx_time, ttw"
            " FROM RS485_config WHERE disabled='false' AND class='normal'"
            );
    ret = sqlite3_exec(task->database, sql, sql_rs485_result, task, &errmsg);
    if ( ret ) {
        log_printf(ERR, "TOM: SQL error: %s", errmsg);
    }
    log_printf(INF, "ZEUS: 绑定串口模组完成%s: %s",
               task->uarts[0]->dev_name,
            task->uarts[1]->dev_name);

#if 1
    // 串口通信线程
    ret = pthread_create( & task->tid, &task->attr, thread_uart_service, (void*)task->uarts[0]);
    if ( 0 != ret ) {
        ret  = 0x1006;
        log_printf(ERR,
                   "UART framework start up.                       FAILE!!!!");
        goto __panic;
    }
    // 串口通信线程
    ret = pthread_create( & task->tid, &task->attr, thread_uart_service, (void*)task->uarts[1]);
    if ( 0 != ret ) {
        ret  = 0x1006;
        log_printf(ERR,
                   "UART framework start up.                       FAILE!!!!");
        goto __panic;
    }
#endif

    do {sleep(1);} while (1);
    task->commit_head = NULL;
    task->wait_head = NULL;
    task->wait_job_nr = 0;
    memset(task->job, 0, sizeof(task->job));
    pthread_mutex_init(&task->commit_lck, NULL);
    pthread_mutex_init(&task->wait_lck, NULL);
    pthread_mutex_init(&task->err_list_lck, NULL);

    //memset(task->single, 255, sizeof(task->single));

    while ( 1 ) {
        // 清除无效的作业
        job_detach_wait(task);

        deal_with_system_protection(task, NULL);

        do {
            // 处理提交事件
            struct job_commit_data *thiz = NULL;
            thiz = job_select_commit(task);
            while ( thiz ) {
                switch ( thiz->cmd ) {
                case COMMIT_CMD_FORK:
                    job_fork(task, thiz);
                    break;
                case COMMIT_CMD_DONE:
                    break;
                case COMMIT_CMD_ABORT:
                    break;
                }
                memset(thiz, 0, sizeof(struct job_commit_data));
                thiz = job_select_commit(task);
                free(thiz);
            }
        } while ( 0 );

        do {
            // 执行作业
            int i = 0;
            for( i = 0; i < task->sys_config_gun_nr; i ++ ) {
                if ( task->job[ i ] == NULL ) continue;
                job_running(task, task->job[ i ]);
                if ( task->job[i]->job_status == JOB_ABORTING ||
                     task->job[i]->job_status == JOB_DETACHING ) {
                    free(task->job[i]);
                    log_printf(INF, "ZEUS: 作业中止");
                    task->job[i] = NULL;
                    continue;
                }
            }
        } while ( 0 );

        do {
            // 选取可执行的作业并准备执行
            struct charge_job *job;
            if ( task->sys_config_gun_nr == 2 && task->sys_charge_group_nr == 1 ) {
                if ( task->job[0] || task->job[1] ) break;

                job = job_select_wait(task, GUN_SN0);
                if ( job ) {
                    // ...
                    task->job[0] = job;
                    log_printf(INF, "ZEUS: 开始执行作业 @ GUN%d:%d",
                               job->job_gun_sn, task->wait_job_nr);
                    break;
                }

                job = job_select_wait(task, GUN_SN1);
                if ( job ) {
                    task->job[1] = job;
                    log_printf(INF, "ZEUS: 开始执行作业 @ GUN%d:%d",
                               job->job_gun_sn, task->wait_job_nr);
                }
            }
            if ( task->sys_config_gun_nr == 2 && task->sys_charge_group_nr == 2 ) {
                job = job_select_wait(task, GUN_SN0);
                if ( job ) {
                    task->job[0] = job;
                    // ...
                }

                job = job_select_wait(task, GUN_SN1);
                if ( job ) {
                    task->job[1] = job;
                    // ...
                }
            }
            // {{{ 没实现
            if ( task->sys_config_gun_nr == 4 && task->sys_charge_group_nr == 2 ) {
            }
            if ( task->sys_config_gun_nr == 4 && task->sys_charge_group_nr == 4 ) {
            }
            //}}} 没实现
        } while ( 0 );

        usleep(50000);
    }
__panic:
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
    int ei, n = 0;

    for ( ei = S_ERROR + 1; ei < S_END; ei ++ ) {
        if ( bit_read(tsk, ei) ) {
            error_history_begin(thiz, ei, "N/A");
            n ++;
        } else {
            error_history_recover(thiz, ei);
        }
    }

    if ( n ) {
        bit_set(task, S_ERROR);
    } else {
        bit_clr(task, S_ERROR);
    }
    return;
}

void job_running(struct charge_task *tsk, struct charge_job *job)
{
    int ret;
    char buff[64] = {0};
    char sql[256];
    int start  = 0, end = 0;

    if ( job == NULL ) return;

    switch ( job->job_status ) {
    case JOB_IDLE:
    case JOB_SETTING:
        break;
    case JOB_WAITTING:
        bit_clr(tsk, CMD_DC_OUTPUT_SWITCH_ON);
        bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
        bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
        job->job_status = JOB_STANDBY;
        break;
    case JOB_STANDBY:
        bit_clr(tsk, CMD_DC_OUTPUT_SWITCH_ON);
        bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
        bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
        ret = __is_gun_phy_conn_ok(job);
        if ( ret == GUN_UNDEFINE || ret == GUN_INVALID ) {
            break;
        }

        // 充电模式为自动充电，需要和BMS通信，此时才需要将辅助电源合闸
        if ( job->charge_mode == CHARGE_AUTO ) {
            if ( ret == GUN_SN0 ) {
                if ( ! bit_read(tsk, F_GUN_1_ASSIT_PWN_SWITCH_STATUS) ) {
                    if ( !bit_read(tsk, S_ASSIT_PWN_ERR) ) {
                        if ( ! bit_read(tsk, CMD_GUN_1_ASSIT_PWN_ON) ) {
                            log_printf(INF, "ZEUS: 1# 充电枪辅助电源开始供电");
                        }
                        bit_set(tsk, CMD_GUN_1_ASSIT_PWN_ON);
                    }
                }
            } else {
                if ( ! bit_read(tsk, F_GUN_2_ASSIT_PWN_SWITCH_STATUS) ) {
                    if ( !bit_read(tsk, S_ASSIT_PWN_ERR) ) {
                        if ( ! bit_read(tsk, CMD_GUN_2_ASSIT_PWN_ON) ) {
                            log_printf(INF, "ZEUS: 2# 充电枪辅助电源开始供电");
                        }
                        bit_set(tsk, CMD_GUN_2_ASSIT_PWN_ON);
                    }
                }
            }
            if ( ! bit_read(tsk, F_BMS_RECOGNIZED) ) {
                break;
            }
        }

        if ( ! bit_read(tsk, F_SYSTEM_CHARGE_ALLOW) ) {
            job->job_status = JOB_ERR_PAUSE;
            log_printf(WRN, "ZEUS: 系统发生关键故障, 自动暂停作业(%X)",
                       job->job_status);
            break;
        }

        job->job_status = JOB_WORKING;
        sprintf(sql, "UPDATE jobs set job_status='%d' where job_id='%ld'",
                job->job_status, job->job_url_commit_timestamp);
        (void)sqlite3_exec(task->database, sql, NULL, NULL, NULL);
        log_printf(INF, "***** ZEUS(关键): 作业转为正式开始执行, 正在执行.");
        break;
    case JOB_WORKING:
        if ( job->charge_mode != CHARGE_AUTO ) {
            sprintf(buff, "%d", (unsigned int)(job->need_V * 10.0f) );
            config_write("需求电压", buff);
            config_write("初始电压", buff);
            sprintf(buff, "%d", (unsigned int)(job->need_I * 10.0f) );
            config_write("需求电流", buff);
        }

        if ( ! bit_read(tsk, F_SYSTEM_CHARGE_ALLOW) ) {
            bit_clr(tsk, CMD_DC_OUTPUT_SWITCH_ON);
            bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
            bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
            job->status_befor_fault = JOB_WORKING;
            job->job_status = JOB_ERR_PAUSE;
            log_printf(WRN, "ZEUS: 系统发生关键故障, 自动暂停作业(JOB_WORKING)");
            break;
        } else {
            bit_set(tsk, CMD_DC_OUTPUT_SWITCH_ON);
            ret = __is_gun_phy_conn_ok(job);
            if ( ret  == GUN_SN0 ) {
                bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);

                if ( ! bit_read(tsk, CMD_GUN_1_OUTPUT_ON) ) {
                    job->charge_begin_kwh_data = task->meter[0].kwh_zong;
                    job->charge_begin_timestamp = time(NULL);
                    start ++;
                }
                bit_set(tsk, CMD_GUN_1_OUTPUT_ON);
            }
            if ( ret  == GUN_SN1 ) {
                bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);

                if ( ! bit_read(tsk, CMD_GUN_2_OUTPUT_ON) ) {
                    job->charge_begin_kwh_data = task->meter[0].kwh_zong;
                    job->charge_begin_timestamp = time(NULL);
                    start ++;
                }
                bit_set(tsk, CMD_GUN_2_OUTPUT_ON);
            }

            // 有新的充电状态变化
            if ( start ) {
                sprintf(sql, "INSERT INTO job_billing VALUES("
                        "'%ld','%ld','0','0','%.2f','0.00','0.00','%.2f')",
                        job->job_url_commit_timestamp,
                        job->charge_begin_timestamp,
                        job->charge_begin_kwh_data,
                        task->kwh_price);
                (void)sqlite3_exec(task->database, sql, NULL, NULL, NULL);
            }

            //{{ 在这做是否充完判定
            if (job->charge_billing.mode == BILLING_MODE_AS_CAP ) {
                if ( task->meter[0].kwh_zong - job->charge_begin_kwh_data >=
                     job->charge_billing.option.set_kwh ) {
                    job->charge_exit_kwh_data = task->meter[0].kwh_zong;
                    job->charge_stop_timestamp = time(NULL);
                    log_printf(INF,
                               "ZEUS: 充电结束, 起始电量: %.2f KWH, "
                               "终止电量: %.2f KWH, 充电电量: %.2f KWH",
                               job->charge_begin_kwh_data,
                               task->meter[0].kwh_zong,
                               task->meter[0].kwh_zong -
                                job->charge_begin_kwh_data);
                    job->job_status = JOB_DONE;
                    end ++;
                }
            } else if ( job->charge_billing.mode == BILLING_MODE_AS_MONEY ) {
                double used_kwh = task->meter[0].kwh_zong - job->charge_begin_kwh_data;
                if ( used_kwh * task->kwh_price >= job->charge_billing.option.set_money ) {
                    job->charge_exit_kwh_data = task->meter[0].kwh_zong;
                    job->charge_stop_timestamp = time(NULL);
                    log_printf(INF,
                               "ZEUS: 充电结束, 起始电量: %.2f KWH, "
                               "终止电量: %.2f KWH, 充电电量: %.2f KWH",
                               job->charge_begin_kwh_data,
                               task->meter[0].kwh_zong,
                               task->meter[0].kwh_zong -
                                job->charge_begin_kwh_data);
                    job->job_status = JOB_DONE;
                    end ++;
                }
            } else if ( job->charge_billing.mode == BILLING_MODE_AS_TIME ) {
                if ( time(NULL) - job->charge_begin_timestamp >=
                     job->charge_billing.option.set_time ) {
                    job->charge_exit_kwh_data = task->meter[0].kwh_zong;
                    job->charge_stop_timestamp = time(NULL);
                    log_printf(INF,
                               "ZEUS: 充电结束, 起始时戳: %ld, "
                               "终止时戳: %ld, 充电时长: %ld 秒",
                               job->charge_begin_timestamp,
                               job->charge_stop_timestamp,
                               job->charge_stop_timestamp -
                                job->charge_begin_timestamp);
                    job->job_status = JOB_DONE;
                    end ++;
                }
            } else if ( job->charge_billing.mode == BILLING_MODE_AS_FREE ) {

            } else if ( job->charge_billing.mode == BILLING_MODE_AS_AUTO ) {

            } else {

            }

            //}}
            if ( bit_read(job, CMD_JOB_ABORT) ) {
                bit_clr(job, CMD_JOB_ABORT);
                job->status_befor_fault = JOB_WORKING;
                job->job_status = JOB_ABORTING;
                job->charge_exit_kwh_data = task->meter[0].kwh_zong;
                job->charge_stop_timestamp = time(NULL);
                end ++;
                log_printf(INF, "***** ZEUS(关键): 作业中止(人为), 正在中止");
            }
            if ( bit_read(job, CMD_JOB_MAN_PAUSE) ) {
                job->status_befor_fault = JOB_WORKING;
                job->job_status = JOB_MAN_PAUSE;
                job->charge_exit_kwh_data = task->meter[0].kwh_zong;
                job->charge_stop_timestamp = time(NULL);
                end ++;
                log_printf(WRN, "ZEUS: 人工暂停作业(JOB_WORKING)");
            }

            // 充电作业发生状态变化
            if ( end ) {
                sprintf(sql,
                        "UPDATE job_billing SET b_end_timestamp='%ld',"
                        "b_end_kwh='%.2f' WHERE job_id='%ld' AND b_begin_timestamp='%ld'",
                        job->charge_stop_timestamp,
                        job->charge_exit_kwh_data,
                        job->job_url_commit_timestamp,
                        job->charge_begin_timestamp);
                (void)sqlite3_exec(task->database, sql, NULL, NULL, NULL);

                sprintf(sql,
                        "UPDATE jobs SET jos_status='%d' WHERE job_id='%ld'",
                        job->job_status,
                        job->job_url_commit_timestamp);
                (void)sqlite3_exec(task->database, sql, NULL, NULL, NULL);
            }

        }
        break;
    case JOB_ERR_PAUSE:
        bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
        bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
        if ( ! bit_read(tsk, F_SYSTEM_CHARGE_ALLOW) ) {
            if ( bit_read(job, CMD_JOB_ABORT) ) {
                bit_clr(job, CMD_JOB_ABORT);
                log_printf(INF, "ZEUS: 充电任务中止(%X)",
                           job->status_befor_fault);
                job->job_status = JOB_ABORTING;
            }
        } else {
            job->job_status = JOB_RESUMING;
            log_printf(INF, "ZEUS: 故障消除, 充电作业继续进行(%X)",
                       job->status_befor_fault);
        }
        break;
    case JOB_MAN_PAUSE:
        bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
        bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
        if ( bit_read(job, CMD_JOB_ABORT) ) {
            bit_clr(job, CMD_JOB_ABORT);
            log_printf(INF, "ZEUS: 充电任务中止(%X:JOB_MAN_PAUSE)",
                       job->status_befor_fault);
            job->job_status = JOB_ABORTING;
        }
        if ( bit_read(job, CMD_JOB_RESUME) ) {
            bit_clr(job, CMD_JOB_RESUME);
            job->job_status = JOB_RESUMING;
            log_printf(INF, "ZEUS: 人工恢复作业(%X), 正在恢复",
                       job->status_befor_fault);
        }
        if ( ! bit_read(tsk, F_SYSTEM_CHARGE_ALLOW) ) {

        } else {
            if ( bit_read(job, CMD_JOB_MAN_PAUSE) ) {
                break;
            }

        }
        break;
    case JOB_RESUMING:
        job->job_status = JOB_WORKING;
        break;
    case JOB_ABORTING:
        bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
        bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
        job->job_status = JOB_DETACHING;
        break;
    case JOB_DONE:
        bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
        bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
        break;
    case JOB_EXITTING:
        bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
        bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
        break;
    case JOB_DETACHING:
        if ( job->job_status > JOB_WORKING ){
            bit_clr(tsk, CMD_DC_OUTPUT_SWITCH_ON);
            bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
            bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
        }
        job = NULL;
        break;
    }

    if ( bit_read(job, CMD_JOB_ABORT) ) {
        bit_clr(job, CMD_JOB_ABORT);
        job->status_befor_fault = JOB_WORKING;
        job->job_status = JOB_ABORTING;
        job->charge_exit_kwh_data = task->meter[0].kwh_zong;
        job->charge_stop_timestamp = time(NULL);
        end ++;
        log_printf(INF, "***** ZEUS(关键): 作业中止(人为), 正在中止");
    }
    if ( bit_read(job, CMD_JOB_MAN_PAUSE) ) {
        bit_clr(job, CMD_JOB_MAN_PAUSE);
        job->status_befor_fault = JOB_WORKING;
        job->job_status = JOB_MAN_PAUSE;
        job->charge_exit_kwh_data = task->meter[0].kwh_zong;
        job->charge_stop_timestamp = time(NULL);
        end ++;
        log_printf(WRN, "ZEUS: 人工暂停作业(JOB_WORKING)");
    }

    // 充电作业发生状态变化
    if ( end ) {
        sprintf(sql,
                "UPDATE job_billing SET b_end_timestamp='%ld',"
                "b_end_kwh='%.2f' WHERE job_id='%ld' AND b_begin_timestamp='%ld'",
                job->charge_stop_timestamp,
                job->charge_exit_kwh_data,
                job->job_url_commit_timestamp,
                job->charge_begin_timestamp);
        (void)sqlite3_exec(task->database, sql, NULL, NULL, NULL);

        sprintf(sql,
                "UPDATE jobs SET jos_status='%d' WHERE job_id='%ld'",
                job->job_status,
                job->job_url_commit_timestamp);
        (void)sqlite3_exec(task->database, sql, NULL, NULL, NULL);
    }

}

int job_commit(struct charge_task *tsk, const struct job_commit_data *jc, COMMIT_CMD cmd)
{
    struct job_commit_data *thiz = NULL;
    switch ( cmd ) {
    case COMMIT_CMD_FORK:
        thiz = (struct job_commit_data *)malloc(sizeof(struct job_commit_data));
        memcpy(thiz, jc, sizeof(struct job_commit_data));
        thiz->cmd = cmd;
        list_ini(thiz->job_node);
        pthread_mutex_lock(&task->commit_lck);
        if ( tsk->commit_head == NULL ) {
            tsk->commit_head = &thiz->job_node;
        } else {
            list_inserttail(tsk->commit_head, &thiz->job_node);
        }
        pthread_mutex_unlock (&task->commit_lck);
        break;
    case COMMIT_CMD_ABORT:
        break;
    }
    return 0;
}

void dump_commit_struct (struct job_commit_data *need)
{
    printf("\tcmd:%x\n", need->cmd);
    printf("\turl_commit_timestamp:%ld\n",need->url_commit_timestamp);
    printf("\tontom_commit_date_time:%ld\n", need->ontom_commit_date_time);
    printf("\tcharge_gun:%x\n", need->charge_gun);
    printf("\tcharge_mode:%x\n", need->charge_mode);
    printf("\tmanual_set_charge_volatage:%f\n", need->manual_set_charge_volatage);
    printf("\tmanual_set_charge_current:%f\n", need->manual_set_charge_current);
    printf("\tcard_passwd:%s\n", need->card_passwd);
    printf("\tcard_money_remain:%.2f\n", need->card_remain);
}

struct charge_job * job_fork(struct charge_task *tsk, struct job_commit_data *need)
{
    struct charge_job* thiz = NULL;
    char sql[512] = {0};
    char str_cmode[32], str_bmode[32];
    int ret, nr_gen = 0, s = 0, i;
    char *errmsg = NULL;

    log_printf(INF, "开始创建作业");

    if ( need->charge_mode == CHARGE_AUTO ) {
        sprintf(sql,
                "SELECT COUNT(*) from symbol_define,bms_can_pack_generator "
                "WHERE bms_can_pack_generator.bms_can_status='ENABLE' AND "
                "symbol_define.symbol_name=bms_can_pack_generator.bms_can_stage");
        ret = sqlite3_exec(tsk->database, sql, sql_query_result_conter, &nr_gen, &errmsg);
        if ( ret ) {
            log_printf(ERR, "ZEUS: DATABASE error: %s", errmsg);
        }
        if ( nr_gen <= 0 ) {
            log_printf(ERR, "ZEUS: DATA LOST, job aborted!!");
            // 中止作业
        }
    }

    s = sizeof(struct charge_job);
    s = s + sizeof(struct can_pack_generator) * nr_gen;
    thiz = (struct charge_job *)malloc(s);
    if (thiz == NULL) {
        log_printf(ERR, "ZEUS: LOW memory, job create faile, aborted.");
        // 中止作业
    }
    memset(thiz, 0, s);
    thiz->bms.can_dev = "can0";
    thiz->bms.can_bms_status = CAN_INVALID;
    thiz->bms.job = thiz;
    list_ini(thiz->job_node);
    thiz->job_url_commit_timestamp = need->url_commit_timestamp;
    thiz->charge_billing.mode = need->biling_mode;
    if ( thiz->charge_billing.mode == BILLING_MODE_AS_CAP ) {
       thiz->charge_billing.option.set_kwh = need->as_kwh;
    }
    if ( thiz->charge_billing.mode == BILLING_MODE_AS_MONEY ) {
        thiz->charge_billing.option.set_money = need->as_money;
    }
    if ( thiz->charge_billing.mode == BILLING_MODE_AS_TIME ) {
        thiz->charge_billing.option.set_time = need->as_time;
    }

    thiz->charge_mode = need->charge_mode;
    thiz->job_gun_sn = need->charge_gun;
    strcpy(thiz->card.triger_card_sn, need->card_sn);
    __card_write_passwd(&thiz->card, need->card_passwd);
    __card_write_remain(&thiz->card, need->card_remain);
    thiz->card.card.sector_4.data.magic = 0x4F4E5057;
    thiz->card.card.sector_4.data.reserved = 0;
    thiz->card.card.sector_4.data.unuesed = 0;

    if ( thiz->charge_mode != CHARGE_AUTO ) {
        thiz->need_I = need->manual_set_charge_current;
        thiz->need_V = need->manual_set_charge_volatage;
    }

    thiz->charge_bms_establish_timestamp = rand() % 10000 + 5000;

    thiz->bms.readed = 0; // 用户操作数据记录时的临时记录
    thiz->bms.can_pack_gen_nr = nr_gen;
    thiz->bms.generator =
       (struct can_pack_generator*)(((char *)thiz) + sizeof(struct charge_job));
    sprintf(sql,
            "SELECT symbol_define.symbol_value,"
            "bms_can_pack_generator.bms_can_pgn,"
            "bms_can_pack_generator.bms_can_prioriy,"
            "bms_can_pack_generator.bms_can_datalen,"
            "bms_can_pack_generator.bms_can_period,"
            "bms_can_pack_generator.bms_can_tolerate_silence "
            "FROM symbol_define,bms_can_pack_generator "
            "WHERE symbol_define.symbol_name=bms_can_pack_generator.bms_can_stage");
    ret = sqlite3_exec(tsk->database, sql, sql_query_BMS_pack_gen, thiz, &errmsg);
    if ( ret ) {
        log_printf(ERR, "ZEUS: DATABASE error: %s", errmsg);
    }

#if 0
    for ( i = 0; i < thiz->bms.can_pack_gen_nr; i ++) {
        printf("\t%4d  %08X  %2d   %9d  %5d   %d\n",
               thiz->bms.generator[i].stage,
               thiz->bms.generator[i].can_pgn,
               thiz->bms.generator[i].prioriy,
               thiz->bms.generator[i].datalen,
               thiz->bms.generator[i].period,
               thiz->bms.generator[i].can_tolerate_silence);
    }
    // BMS 数据包写线程，从队列中取出要写的数据包并通过CAN总线发送出去
    ret = pthread_create( & thiz->tid_write, &task->attr, thread_bms_write_service, thiz);
    if ( 0 != ret ) {
        log_printf(ERR, "CAN-BUS reader start up.                       FAILE!!!!");
        goto die;
    }

    // BMS读书举报线程，从CAN总线读取数据包后将数据存入读入数据队列等待处理。
    ret = pthread_create( & thiz->tid_read, &task->attr, thread_bms_read_service, thiz);
    if ( 0 != ret ) {
        log_printf(ERR, "CAN-BUS writer start up.                       FAILE!!!!");
        goto die;
    }
#endif
    thiz->job_status = JOB_WAITTING;

    pthread_mutex_lock(&task->wait_lck);
    if (task->wait_head == NULL) {
        task->wait_head = &thiz->job_node;
        task->wait_job_nr = 1;
    } else {
        list_inserttail(task->wait_head, &thiz->job_node);
        task->wait_job_nr ++;
    }
    pthread_mutex_unlock (&task->wait_lck);

    sprintf(str_cmode, "auto");
    sprintf(str_bmode, "auto");
    if ( thiz->charge_mode == CHARGE_BV ) {
        sprintf(str_cmode, "BV;%.1f V;%.1f A;",
                thiz->need_V, thiz->need_I);
    } else if ( thiz->charge_mode == CHARGE_BI ) {
        sprintf(str_cmode, "BI;%.1f A;%.1f V;",
                thiz->need_I, thiz->need_V);
    }

    if ( thiz->charge_billing.mode == BILLING_MODE_AS_MONEY ) {
        sprintf(str_bmode, "AS_MONEY;%.1f 元;",
                thiz->charge_billing.option.set_money);
    } else if ( thiz->charge_billing.mode == BILLING_MODE_AS_TIME ) {
        sprintf(str_bmode, "AS_TIME;%d 秒;",
                thiz->charge_billing.option.set_time);
    } else if ( thiz->charge_billing.mode == BILLING_MODE_AS_CAP ) {
        sprintf(str_bmode, "AS_CAP;%.1f 度;",
                thiz->charge_billing.option.set_kwh);
    }
    sprintf(sql, "INSERT INTO jobs VALUES("
            "\'%ld\',\'%d\',\'%d\',\'%s\',\'%s\',\'%s\')",
            thiz->job_url_commit_timestamp,
            thiz->job_status,
            thiz->job_gun_sn,
            thiz->card.triger_card_sn,
            str_cmode,
            str_bmode
            );
    ret = sqlite3_exec(tsk->database, sql, NULL, NULL, &errmsg);
    if ( ret ) {
        log_printf(ERR, "ZEUS: DATABASE error: %s", errmsg);
    }

    dump_commit_struct(need);

    thiz->charge_job_create_timestamp = time(NULL);

    log_printf(INF, "ZEUS: 作业创建完成(%p:%d:%ld).",
               thiz, task->wait_job_nr, thiz->job_url_commit_timestamp);
    return thiz;

die:
    log_printf(ERR, "ZEUS: 创建作业失败");
    free(thiz);
    thiz = NULL;
    return thiz;
}

// 检查作业是否存在
int job_exsit(time_t id)
{
    int i = 0;
    struct list_head *thiz;
    struct charge_job *j = NULL;
    struct job_commit_data *c = NULL;

    for ( i = 0; i < sizeof(task->job)/sizeof(struct charge_job*); i ++) {
        if ( task->job[i] == NULL ) continue;
        if ( task->job[i]->job_url_commit_timestamp == id ) {
            return (int)task->job[i];
        }
    }

    pthread_mutex_lock(&task->commit_lck);
    if ( task->commit_head ) {
        thiz = task->commit_head;
        do {
            c = list_load(struct job_commit_data, job_node, thiz);
            if ( c->url_commit_timestamp == id ) {
                break;
            }
            thiz = thiz->next;
            c = NULL;
        } while ( thiz != task->commit_head );
    }
    pthread_mutex_unlock (&task->commit_lck);
    if ( c ) return (int)c;

    pthread_mutex_lock(&task->wait_lck);
    if ( task->wait_head ) {
        thiz = task->wait_head;
        do {
            j = list_load(struct charge_job, job_node, thiz);
            if ( j->job_url_commit_timestamp == id ) {
                break;
            }
            thiz = thiz->next;
            j = NULL;
        } while ( thiz != task->wait_head );
    }
    pthread_mutex_unlock (&task->wait_lck);

    return (int)j;
}

struct charge_job* job_search(time_t ci_timestamp)
{
    int i = 0;
    struct list_head *thiz;
    struct charge_job *j = NULL;

    log_printf(INF, "search for id %08X", ci_timestamp);

    for ( i = 0; i < sizeof(task->job)/sizeof(struct charge_job*); i ++) {
        if ( task->job[i] == NULL ) continue;
        if ( task->job[i]->job_url_commit_timestamp == ci_timestamp ) {
            return task->job[i];
        }
    }

    pthread_mutex_lock(&task->wait_lck);
    if ( task->wait_head ) {
        thiz = task->wait_head;
        do {
            j = list_load(struct charge_job, job_node, thiz);
            if ( j->job_url_commit_timestamp == ci_timestamp ) {
                break;
            }
            thiz = thiz->next;
            j = NULL;
        } while ( thiz != task->wait_head );
    }
    pthread_mutex_unlock (&task->wait_lck);

    return j;
}

// 从等待链表中选出最先提交的作业
struct charge_job * job_select_wait(struct charge_task *tsk, CHARGE_GUN_SN gun)
{
    struct charge_job *thiz = NULL;
    struct list_head *p, *next;
    pthread_mutex_lock(&tsk->wait_lck);
    if ( tsk->wait_head != NULL ) {
        p = tsk->wait_head;
        do {
            next = p->next;
            thiz = list_load(struct charge_job, job_node, p);
            if ( thiz->job_gun_sn != gun ) {
                p = p->next;
                thiz = NULL;
                continue;
            }
            list_remove(p);
            if ( p == tsk->wait_head ) {
                if ( next == tsk->wait_head ) {
                    tsk->wait_head = NULL;
                    tsk->wait_job_nr = 0;
                } else {
                    tsk->wait_head = next;
                    tsk->wait_job_nr --;
                }
            }
            break;
        } while ( p != tsk->wait_head);
    }
    pthread_mutex_unlock (&tsk->wait_lck);

    return thiz;
}

// 从等待列表中删除
void job_detach_wait(struct charge_task *tsk)
{
    struct charge_job *thiz = NULL;
    struct list_head *p, *next;
    pthread_mutex_lock(&tsk->wait_lck);
    if ( tsk->wait_head != NULL ) {
        p = tsk->wait_head;
        do {
            next = p->next;
            thiz = list_load(struct charge_job, job_node, p);
            //log_printf(INF, "id: %08X %x", thiz->job_url_commit_timestamp,
            //           thiz->job_status);
            if ( thiz->job_status != JOB_DETACHING ) {
                p = p->next;
                thiz = NULL;
                continue;
            }
            list_remove(p);
            if ( p == tsk->wait_head ) {
                if ( next == tsk->wait_head ) {
                    tsk->wait_head = NULL;
                    tsk->wait_job_nr = 0;
                } else {
                    tsk->wait_head = next;
                    tsk->wait_job_nr --;
                }
            }
            log_printf(INF, "ZEUS: 作业 %ld 被释放", thiz->job_url_commit_timestamp);
            free(thiz);
            break;
        } while ( p != tsk->wait_head);
    }
    pthread_mutex_unlock (&tsk->wait_lck);
}

// 从提交链表中取出第一个提交事件
struct job_commit_data *job_select_commit(struct charge_task *tsk)
{
    struct job_commit_data *thiz = NULL;
    pthread_mutex_lock(&tsk->commit_lck);
    if ( tsk->commit_head ) {
        struct list_head *next = tsk->commit_head->next;
        thiz = list_load(struct job_commit_data, job_node, tsk->commit_head);
        if ( next = tsk->commit_head ) {
            next = NULL;
        }
        list_remove(tsk->commit_head);
        tsk->commit_head = next;
    }
    pthread_mutex_unlock (&tsk->commit_lck);
    return thiz;
}

unsigned int error_history_begin(struct charge_job *job, unsigned int error_id, char *error_string)
{
    struct error_history *thiz;
    struct list_head *head;
    char sql[128], timestamp[20];
    int ret;

    pthread_mutex_lock(&task->err_list_lck);
    if ( task->err_head != NULL ) {
        head = task->err_head;
        do {
            thiz = list_load(struct error_history, error_me, head);
            if ( thiz->error_id == error_id ) {
                goto out;
            }
            head = head->next;
        } while ( head != task->err_head );
    }

    thiz = (struct error_history*)malloc(sizeof(struct error_history));
    if ( thiz == NULL ) goto out;
    list_ini(thiz->error_me);
    if ( task->err_head == NULL ) {
        task->err_head = & thiz->error_me;
    } else {
        list_inserttail(task->err_head, &thiz->error_me);
    }
    task->err_nr ++;
    thiz->error_seqid = task->err_seq_id_next ++;
    thiz->error_id = error_id;
    strncpy(thiz->error_string, error_string, 32);
    strcpy(thiz->error_recover, "0000-00-00 00:00:00");

    log_printf(INF, "ZEUS: 故障总数为: %d", task->err_nr);

    __get_timestamp(timestamp);
    strcpy(thiz->error_begin, timestamp);
    sprintf(sql, "INSERT INTO errors VALUES('%d','%d','%s','%s','ERROR')",
            thiz->error_seqid,
            thiz->error_id,
            thiz->error_begin,
            thiz->error_recover);
    ret = sqlite3_exec(task->database, sql, NULL, NULL, NULL);
    log_printf(INF, "ZEUS: %s:%d", sql, ret);
out:
    pthread_mutex_unlock (&task->err_list_lck);

    return error_id;
}

void error_history_recover(struct charge_job *job, unsigned int error_id)
{
    struct error_history *thiz;
    struct list_head *head;
    char sql[128], errname[32], timestamp[20];
    int ret;

    pthread_mutex_lock(&task->err_list_lck);

    if ( task->err_head == NULL ) goto out;

    head = task->err_head;
    do {
        thiz = list_load(struct error_history, error_me, head);
        if ( thiz->error_id == error_id ) {
            goto del;
        }
        head = head->next;
    } while ( head != task->err_head );
    goto out;
del:
    if ( task->err_head == & thiz->error_me ) {
        task->err_head = thiz->error_me.next;
    }
    list_remove(&thiz->error_me);
    task->err_nr --;
    if ( task->err_nr == 0 ) {
        task->err_head = NULL;
    }
    __get_timestamp(timestamp);
    sprintf(sql,
            "UPDATE errors SET "
            "error_recover='%s',recover_reason='NORMAL' "
            "WHERE error_id='%d' AND error_begin='%s'",
            timestamp, thiz->error_id,
            thiz->error_begin);
    ret = sqlite3_exec(task->database, sql, NULL, NULL, NULL);
    log_printf(INF, "ZEUS: %s:%d", sql, ret);
out:
    pthread_mutex_unlock (&task->err_list_lck);
}
