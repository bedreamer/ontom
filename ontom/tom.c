#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include "../thirdpart/mongoose/mongoose.h"
#include "../thirdpart/D8/librf.h"
#include "../thirdpart/sqlite/sqlite3.h"
#include "list.h"
#include "ajax.h"
#include "config.h"
#include "error.h"
#include "log.h"
#include "Hachiko.h"
#include "charge.h"

extern void * thread_xml_service(void *) ___THREAD_ENTRY___;
extern void * thread_bms_write_service(void *) ___THREAD_ENTRY___;
extern void * thread_bms_read_service(void *) ___THREAD_ENTRY___;
extern void * thread_measure_service(void *) ___THREAD_ENTRY___;
extern void * thread_charger_service(void *) ___THREAD_ENTRY___;
extern void * thread_backgroud_service(void *) ___THREAD_ENTRY___;
extern void * thread_charge_task_service(void *) ___THREAD_ENTRY___;
extern void * thread_uart_service(void *arg) ___THREAD_ENTRY___;

// 串口通信 服务线程
// 提供串口通信服务
void *thread_measure_service(void *arg) ___THREAD_ENTRY___
{
    int *done = (int *)arg;
    int mydone = 0;
    int icdev, ret, initok = 1;
    unsigned long _Snr;
    char buff[32] = {0};

    if ( done == NULL ) done = &mydone;
    log_printf(INF, "%s running...", __FUNCTION__);
____reinit:
    icdev = dc_init(100, 9600);//初始化串口1，波特率9600

    if ( icdev > 0 ) {
        log_printf(INF, "open D8 reader OK...");
        dc_beep(icdev, 100);
        initok = 1;
    } else if ( icdev < 0 ) {
        initok = 0;
        log_printf(ERR, "open D8 reader ERROR: %d", icdev);
    }

    while ( ! *done ) {

        sleep(2);

        if ( 0 == initok ) {
            goto ____reinit;
            continue;
        }
        ret = dc_card(icdev, 0, &_Snr);
        if ( ret > 0 ) {
            continue;
        } else {
            initok = 0;
            continue;
        }
        log_printf(INF, "GET CARD: %08X", _Snr);
        dc_beep(icdev, 50);
        usleep(100000);
        dc_beep(icdev, 50);

        sprintf(buff, "%08X", (unsigned int)_Snr);
        if ( config_read("triger_card_sn")[0] == 'N' ) {
            config_write("triger_card_sn", buff);
            log_printf(INF, "card trigerd.");
            continue;
        }
        if ( 0 != strcmp(config_read("triger_card_sn"), buff) ) {
            continue;
        } else if ( config_read("confirm_card_sn")[0] == 'N' ) {
            config_write("confirm_card_sn", buff);
            log_printf(INF, "card confirmed.");
            continue;
        }

        if ( 0 == strcmp(config_read("triger_card_sn"), buff) &&
             0 == strcmp(config_read("confirm_card_sn"), buff) ) {
            config_write("settle_card_sn", buff);
            log_printf(INF, "card settled.");
            continue;
        }
    }

    return NULL;
}

// 充电机通信服务线程
void *thread_charger_service(void *arg) ___THREAD_ENTRY___
{
    int *done = (int *)arg;
    int mydone = 0;
    if ( done == NULL ) done = &mydone;
    log_printf(INF, "%s running...", __FUNCTION__);

    while ( ! *done ) {
        usleep(5000);
    }
    return NULL;
}

// 后台通信服务线程
void *thread_backgroud_service(void *arg) ___THREAD_ENTRY___
{
    int *done = (int *)arg;
    int mydone = 0;
    if ( done == NULL ) done = &mydone;
    log_printf(INF, "%s running...", __FUNCTION__);

    while ( ! *done ) {
        usleep(5000);
    }

    return NULL;
}

// 捕捉中止信号，保存重要数据
void sig_interrupt(int signo)
{
    if ( task && task->database ) {
        log_printf(WRN, "TOM: 捕捉到键盘CTRL+C信号, 进程即将中止, 开始保存重要数据...");
        char sql[128] = {0};
        time_t timep;
        struct tm *p;
        time(&timep);
        p =localtime(&timep);

        sprintf(sql,
                "insert into log values('%04d-%02d-%02d %02d:%02d:%02d', '系统中止')",
                p->tm_year + 1990,
                p->tm_mon,
                p->tm_mday,
                p->tm_hour,
                p->tm_min,
                p->tm_sec);
        sqlite3_exec(task->database, sql, NULL, NULL, NULL);
        sqlite3_close(task->database);
    }
    exit(0);
}

int sql_result(void *param, int nr, char **text, char **name)
{
    if ( nr > 0 ) {
        for ( ; text && name && *text && &name ; text ++ ) {
            printf("   %16s -- %16s\n", *text, *name);
        }
        task->err_seq_id_next = atoi(*text);
    } else {
        task->err_seq_id_next = 1;
    }
    return 0;
}

int main()
{
    const char *user_cfg = NULL;
    pthread_t tid = 0;
    pthread_attr_t attr;
    int s;
    int thread_done[ 8 ] = {0};
    char buff[32];
    int errcode = 0, ret;

    signal(SIGINT, sig_interrupt);

    ret = sqlite3_open(DEFAULT_DB, &task->database);
    if ( ret != SQLITE_OK ) {
        log_printf(ERR, "TOM: 打开数据库失败..");
        exit(1);
    } else {
        char sql[128] = {0};
        time_t timep;
        struct tm *p;
        time(&timep);
        p =localtime(&timep);

        sprintf(sql,
                "insert into log values('%04d-%02d-%02d %02d:%02d:%02d', '系统启动')",
                p->tm_year + 1990,
                p->tm_mon,
                p->tm_mday,
                p->tm_hour,
                p->tm_min,
                p->tm_sec);
        sqlite3_exec(task->database, sql, NULL, NULL, NULL);

        sprintf(sql, "select MAX(error_seq_id)+1 from errors");
        sqlite3_exec(task->database, sql, sql_result, NULL, NULL);
    }

    task->err_head = NULL;
    task->err_nr = 0;
    ret = pthread_mutex_init(&task->err_list_lck, NULL);
    if ( ret == -1 ) {
        log_printf(ERR, "TOM: 互斥锁初始化失败.");
        exit(1);
    }

    printf(
            "           ___        _       ____\n"
            "          / _ \\ _ __ | |_   _|  _ \\ _____      _____ _ __\n"
            "         | | | | '_ \\| | | | | |_) / _ \\ \\ /\\ / / _ \\ '__|\n"
            "         | |_| | | | | | |_| |  __/ (_) \\ V  V /  __/ |\n"
            "          \\___/|_| |_|_|\\__, |_|   \\___/ \\_/\\_/ \\___|_|\n"
            "                        |___/\n"
           );
    printf( "           \nCopyright © 2014 杭州奥能电源设备股份有限公司版权所有\n");
    printf( "                            %s %s\n\n", __DATE__, __TIME__);
    printf("系统启动中(Revision: %d).....\n\n\n\n", VERSION);

    // 读取配置文件的顺序必须是
    // 1. 系统配置文件
    // 2. 用户参数配置文件
    // 需要注意的是，用户配置数据和用户配置数据可以有相同的配置项
    // 但优先级最高的是用户配置数据，如果某个配置项同时出现在系统配置
    // 和用户配置中，那么系统最终采用的值将是用户配置数据中的值
    // 因此这里需要注意的是：
    // * 有两个配置文件是一种冗余设计
    // * 非必要的情况下，分别将系统配置和用户配置分开存储到两个文件中
    config_initlize("ontom.cfg");
    user_cfg = config_read("user_config_file");
    if ( user_cfg == NULL ) {
        log_printf(WRN, "not gave user config file,"
                   "use 'user.cfg' by default.");
        user_cfg = "user.cfg";
    }
    config_initlize(user_cfg);

    s = pthread_attr_init(&attr);
    if ( 0 != s ) {
        log_printf(WRN, "could not set thread stack size, use default.");
    }

    if ( 0 == s ) {
        int stack_KB;
        const char *stack_size = config_read("thread_stack_size");
        if ( stack_size == NULL ) {
            log_printf(INF, "thread_stack_size not set, use 1024K");
            stack_size = "1024";
        }
        stack_KB = atoi(stack_size);
        if ( stack_KB <= 10 || stack_KB >= 20480 ) {
            log_printf(WRN, "stack size not statisfied(%d KB)."
                       "use default 1024 K instead.", stack_KB);
            stack_KB = 1024;
        }
        if ( 0 == pthread_attr_setstacksize(&attr, stack_KB * 1024) ) {
            log_printf(INF, "set all thread stack size to: %d KB", stack_KB);
        } else {
            log_printf(ERR, "set all thread stack_size to %d KB faile, "
                       "use system default size.", stack_KB);
        }
    }

    // 检查是否需要开启SOCKET端的配置服务器以接受网络端的配置
    user_cfg = config_read("socket_config");
    if ( strcmp(user_cfg, "TURE") ||
         strcmp(user_cfg, "true") ) {
        pthread_create( & tid, &attr, config_drive_service, NULL);
        sprintf(buff, "%d", (int)tid);
        config_write("thread_config_server_id", buff);
    }

    // 启动八公定时器
    Hachiko_init();

    // mongoose 线程，用来处理AJAX请求，解析由客户提交的请求，返回应答的xml文件或其他数据
    ret = pthread_create( & tid, &attr, thread_xml_service, &thread_done[0]);
    if ( 0 != ret ) {
        errcode  = 0x1000;
        log_printf(ERR,
                   "mongoose service start up.                     FAILE!!!!");
        goto die;
    }
    log_printf(INF, "mongoose service start up.                         DONE.");

    // BMS 数据包写线程，从队列中取出要写的数据包并通过CAN总线发送出去
    ret = pthread_create( & tid, &attr, thread_bms_write_service,
                          &thread_done[1]);
    if ( 0 != ret ) {
        errcode  = 0x1001;
        log_printf(ERR,
                   "CAN-BUS reader start up.                       FAILE!!!!");
        goto die;
    }
    log_printf(INF, "CAN-BUS reader start up.                           DONE.");

    // BMS读书举报线程，从CAN总线读取数据包后将数据存入读入数据队列等待处理。
    ret = pthread_create( & tid, &attr, thread_bms_read_service,
                          &thread_done[2]);
    if ( 0 != ret ) {
        errcode  = 0x1002;
        log_printf(ERR,
                   "CAN-BUS writer start up.                       FAILE!!!!");
        goto die;
    }
    log_printf(INF, "CAN-BUS writer start up.                           DONE.");

    // 串口服务线程，和读卡器，采样盒，电能表进行数据交换，测量
    ret = pthread_create( & tid, &attr, thread_measure_service, &thread_done[3]);
    if ( 0 != ret ) {
        errcode  = 0x1003;
        log_printf(ERR,
                   "EX-measure service start up.                   FAILE!!!!");
        goto die;
    }
    log_printf(INF, "EX-measure service start up.                       DONE.");

    // 串口服务线程，和充电机进行通信
    ret = pthread_create( & tid, &attr, thread_charger_service,
                          &thread_done[4]);
    if ( 0 != ret ) {
        errcode  = 0x1004;
        log_printf(ERR,
                   "charger service start up.                      FAILE!!!!");
        goto die;
    }
    log_printf(INF, "charger service start up.                          DONE.");

    // 后台通信线程
    ret = pthread_create( & tid, &attr, thread_backgroud_service,
                          &thread_done[5]);
    if ( 0 != ret ) {
        errcode  = 0x1004;
        log_printf(ERR,
                   "backgroud service start up.                    FAILE!!!!");
        goto die;
    }
    log_printf(INF, "backgroud service start up.                        DONE.");

    // 充电线程，负责充电逻辑
    ret = pthread_create( & tid, &attr, thread_charge_task_service,
                          &thread_done[6]);
    if ( 0 != ret ) {
        errcode  = 0x1005;
        log_printf(ERR,
                   "charge service start up.                       FAILE!!!!");
        goto die;
    }
    log_printf(INF, "charge service start up.                           DONE.");

    // 串口通信线程
    ret = pthread_create( & tid, &attr, thread_uart_service,
                          &thread_done[7]);
    if ( 0 != ret ) {
        errcode  = 0x1006;
        log_printf(ERR,
                   "UART framework start up.                       FAILE!!!!");
        goto die;
    }
    log_printf(INF, "UART framework start up.                           DONE.");

    if ( s == 0 ) {
        pthread_attr_destroy(&attr);
    }

#if CONFIG_DEBUG_CONFIG >= 1
    config_print();
#endif
    // 主循环中放置看门狗代码
    for ( ;; ) {
        sleep(3);
    }
    return 0;
die:
    log_printf(ERR, "going to die. system aborted!");
    return errcode;
}
