#include "stdafx.h"
#include "charge.h"
#include <stdint.h>
#include <execinfo.h>
#include <ucontext.h>

extern void * thread_xml_service(void *) ___THREAD_ENTRY___;
extern void * thread_charger_service(void *) ___THREAD_ENTRY___;
extern void * thread_backgroud_service(void *) ___THREAD_ENTRY___;
extern void * thread_charge_task_service(void *) ___THREAD_ENTRY___;

// 串口通信 服务线程

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
        char buff[32] = {0};

        __get_timestamp(buff);
        sprintf(sql, "INSERT INTO log VALUES('%s', '系统中止 CTRL+C')", buff);
        sqlite3_exec(task->database, sql, NULL, NULL, NULL);
        sqlite3_close(task->database);
    }
    exit(0);
}

void sig_dbg_interrupt(int n, struct siginfo *siginfo, void *myact)
{
    int j, nptrs;
    #define SIZE 100
    void *buffer[100];
    char **strings;

    nptrs = backtrace(buffer, SIZE);
    printf("backtrace() returned %d addresses\n", nptrs);

    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
      would produce similar output to the following: */

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
       perror("backtrace_symbols");
       exit(EXIT_FAILURE);
    }

    for (j = 0; j < nptrs; j++)
       printf("%s\n", strings[j]);

    free(strings);
    exit(1);
}

int sql_result(void *param, int nr, char **text, char **name)
{
    if ( nr > 0 && text ) {
        if ( text[0] ) {
            task->err_seq_id_next = atoi(text[0]);
        } else {
            //log_printf(INF, "TOM: SQL init err_seq=%s", text[0]);
        }
    } else {
        task->err_seq_id_next = 1;
    }
    return 0;
}

void dumping(void)
{
 void *buffer[30] = {0};
 size_t size;
 char **strings = NULL;
 size_t i = 0;


 size = backtrace(buffer, 30);
 fprintf(stdout, "Obtained %zd stack frames.nm\n", size);
 strings = backtrace_symbols(buffer, size);
 if (strings == NULL)
 {
  perror("backtrace_symbols.");
  exit(EXIT_FAILURE);
 }

 for (i = 0; i < size; i++)
 {
  fprintf(stdout, "%s\n", strings[i]);
 }
 free(strings);
 strings = NULL;


}

void dump(unsigned int pc)
{
    /* 动态链接库的映射地址是动态的，需要将maps文件打印出来 */
    char file[64], buffer[1032];
    char pcstr[32];

    pid_t pid = getpid();

    snprintf(file, sizeof(file), "/proc/%d/maps", pid);
    sprintf(pcstr, "%08x", pc);

    FILE *fp = fopen(file, "r");

    if (NULL != fp) {
        while(fgets(buffer, 1024, fp)) {
            if ( 1 <= strcmp(pcstr, buffer ) &&
                 -1 >= strcmp(pcstr, & buffer[9]) ) {
                printf(RED("0x%08x  ==>   %s"), pc,  buffer);
            } else {
                printf("                  %s", buffer);
            }
        }
    } else {
        printf("读取MAPS文件失败!\n");
    }
    static int iTime = 0;

    if (iTime++ >= 1) {
        /* 容错处理：如果访问 ucontext_t 结构体时产生错误会进入该分支 */
        printf("ReEnter %s is not allowed!\n", __FUNCTION__);
    }
}

static void sigsegv_handler(int signum, siginfo_t* info, void*ptr)

{
#ifdef  ARM

    static const char *si_codes[3] = {"", "SEGV_MAPERR", "SEGV_ACCERR"};
    int i;
    ucontext_t *ucontext = (ucontext_t*)ptr;
    void *bt[100];
    char **strings;

    printf("Segmentation Fault Trace:\n");
    printf("info.si_signo = %d\n", signum);
    printf("info.si_errno = %d\n", info->si_errno);
    printf("info.si_code  = %d (%s)\n", info->si_code, si_codes[info->si_code]);
    printf("info.si_addr  = %p\n", info->si_addr);

    /*for arm*/
    printf("the arm_fp 0x%p\n",(void*)ucontext->uc_mcontext.arm_fp);
    printf("the arm_ip 0x%p\n",(void*)ucontext->uc_mcontext.arm_ip);
    printf("the arm_sp 0x%p\n",(void*)ucontext->uc_mcontext.arm_sp);
    printf("the arm_lr 0x%p\n",(void*)ucontext->uc_mcontext.arm_lr);
    printf("the arm_pc 0x%p\n",(void*)ucontext->uc_mcontext.arm_pc);
    printf("the in_code 0x%p\n", (void*)*(unsigned int *)(void*)ucontext->uc_mcontext.arm_pc);
    printf("the arm_cpsr 0x%p\n",(void*)ucontext->uc_mcontext.arm_cpsr);
    printf("the falut_address 0x%p\n",(void*)ucontext->uc_mcontext.fault_address);

    dump((unsigned int)ucontext->uc_mcontext.arm_pc);

    printf("Stack trace (non-dedicated):");
    int sz = backtrace(bt, 20);
    printf("the stack trace is %d\n",sz);
    strings = backtrace_symbols(bt, sz);
    for(i = 0; i < sz; ++i){
         printf("%s\n", strings[i]);
    }
#endif
    _exit (-1);
}


static void  catch_sigsegv()

{

       struct sigaction action;

       memset(&action, 0, sizeof(action));

       action.sa_sigaction = sigsegv_handler;

       action.sa_flags = SA_SIGINFO;       // 注意这里，flag 是 SA_SIGINFO，这样信号处理函数就会多一些信息。

       if(sigaction(SIGSEGV, &action, NULL) < 0){

                  perror("sigaction");

    }

}

struct args_items {
    const char *cmd_line;
    const struct args_items * params;
    const char *comment;
};

struct args_items args[] = {
    {"--allways_conn1",           NULL,          "1#充电枪始终连接."},
    {"--allways_conn2",           NULL,          "2#充电枪始终连接."},
    {"--random_mx_V Vlo Vhi",     NULL,          "随机母线电压，范围 Vlo-Vhi."},
    {"--random_bat1_V Vlo Vhi",   NULL,          "随机电池1电压，范围 Vlo-Vhi."},
    {"--random_bat2_V Vlo Vhi",   NULL,          "随机电池2电压，范围 Vlo-Vhi."},
    {"--random_bat1_I Ilo Ihi",   NULL,          "随机电池1电流，范围 Ilo-Ihi."},
    {"--random_bat2_I Ilo Ihi",   NULL,          "随机电池2电流，范围 Ilo-Ihi."},
    {"--no_timeout",              NULL,          "不进行设备超时统计,只针对采样盒，读卡器，电表，模块等串口设备."},
    {"--no_error",                NULL,          "不进行任何故障统计."},
    {"--version",                 NULL,          "显示主程序版本号, 执行后退出."},
    {"--help",                    NULL,          "显示帮助信息, 执行后退出."},
    {NULL, NULL}
};

void parase_args(int argc, const char *argv[])
{
    int i ;
    for ( i = 1; i < argc; i ++ ) {
        printf("\t%d:<%s>\n", i, argv[i]);
    }
}

int main(int argc, const char *argv[])
{
    const char *user_cfg = NULL;
    int s;
    int thread_done[ 8 ] = {0};
    char buff[32];
    int errcode = 0, ret;
    char *errmsg = NULL;
    struct sigaction act;

    // 读取配置文件的顺序必须是
    // 1. 系统配置文件
    // 2. 用户参数配置文件
    // 需要注意的是，用户配置数据和用户配置数据可以有相同的配置项
    // 但优先级最高的是用户配置数据，如果某个配置项同时出现在系统配置
    // 和用户配置中，那么系统最终采用的值将是用户配置数据中的值
    // 因此这里需要注意的是：
    // * 有两个配置文件是一种冗余设计
    // * 非必要的情况下，分别将系统配置和用户配置分开存储到两个文件中
    config_initlize("/usr/zeus/ontom.cfg");
    user_cfg = config_read("user_config_file");
    if ( user_cfg == NULL ) {
        log_printf(WRN, "not gave user config file,"
                   "use 'user.cfg' by default.");
        user_cfg = "user.cfg";
    }
    config_initlize(user_cfg);
    parase_args(argc, argv);

    signal(SIGINT, sig_interrupt);
    sigemptyset(&act.sa_mask);
    //act.sa_flags=SA_SIGINFO;
    //act.sa_sigaction=sig_dbg_interrupt;
    //sigaction(SIGSEGV,&act,NULL);
    //signal(SIGSEGV, &dump );
    catch_sigsegv();

    memset(task, 0, sizeof(struct charge_task));

    log_printf(DBG_LV1, "TOM: 系统准备启动...");
    ret = sqlite3_open(DEFAULT_DB, &task->database);
    if ( ret != SQLITE_OK ) {
        log_printf(ERR, "TOM: 打开数据库失败..");
        exit(1);
    } else {
        char sql[128] = {0};

        __get_timestamp(buff);
        sprintf(sql, "INSERT INTO log VALUES('%s', '系统启动')", buff);
        ret = sqlite3_exec(task->database, sql, NULL, NULL, &errmsg);
        if ( ret ) {
            log_printf(ERR, "TOM: SQL error: %s", errmsg);
        }

        sprintf(sql, "SELECT MAX(error_seq_id)+1 FROM errors");
        ret = sqlite3_exec(task->database, sql, sql_result, NULL, &errmsg);
        if ( ret ) {
            log_printf(ERR, "TOM: SQL error: %s", errmsg);
        }

        sprintf(sql, "UPDATE errors SET recover_reason='DISCARD' WHERE recover_reason='ERROR'");
        ret = sqlite3_exec(task->database, sql, NULL, NULL, &errmsg);
        if ( ret ) {
            log_printf(ERR, "TOM: SQL error: %s", errmsg);
        }
    }

    //task->err_head = NULL;
    // task->err_nr = 0;
    //ret = pthread_mutex_init(&task->err_list_lck, NULL);
    //if ( ret == -1 ) {
    //    log_printf(ERR, "TOM: 互斥锁初始化失败.");
    //    exit(1);
    //}

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

    s = pthread_attr_init(&task->attr);
    if ( 0 != s ) {
        log_printf(WRN, "could not set thread stack size, use default.");
    }

    if ( 0 == s ) {
        int stack_KB;
        const char *stack_size = config_read("thread_stack_size");
        if ( stack_size == NULL ) {
            log_printf(DBG_LV1, "thread_stack_size not set, use 1024K");
            stack_size = "1024";
        }
        stack_KB = atoi(stack_size);
        if ( stack_KB <= 10 || stack_KB >= 20480 ) {
            log_printf(WRN, "stack size not statisfied(%d KB)."
                       "use default 1024 K instead.", stack_KB);
            stack_KB = 1024;
        }
        if ( 0 == pthread_attr_setstacksize(&task->attr, stack_KB * 1024) ) {
            log_printf(DBG_LV1, "set all thread stack size to: %d KB", stack_KB);
        } else {
            log_printf(ERR, "set all thread stack_size to %d KB faile, "
                       "use system default size.", stack_KB);
        }
    }

    // 检查是否需要开启SOCKET端的配置服务器以接受网络端的配置
    user_cfg = config_read("socket_config");
    if ( strcmp(user_cfg, "TURE") ||
         strcmp(user_cfg, "true") ) {
        pthread_create( & task->tid, &task->attr, config_drive_service, NULL);
        sprintf(buff, "%d", (int)task->tid);
        config_write("thread_config_server_id", buff);
    }
#if 0
    // BMS 数据包写线程，从队列中取出要写的数据包并通过CAN总线发送出去
    ret = pthread_create( & task->tid, &task->attr, thread_bms_write_service,
                          &thread_done[1]);
    if ( 0 != ret ) {
        errcode  = 0x1001;
        log_printf(ERR,
                   "CAN-BUS reader start up.                       FAILE!!!!");
        goto die;
    }
    log_printf(INF, "CAN-BUS reader start up.                           DONE.");

    // BMS读书举报线程，从CAN总线读取数据包后将数据存入读入数据队列等待处理。
    ret = pthread_create( & task->tid, &task->attr, thread_bms_read_service,
                          &thread_done[2]);
    if ( 0 != ret ) {
        errcode  = 0x1002;
        log_printf(ERR,
                   "CAN-BUS writer start up.                       FAILE!!!!");
        goto die;
    }
    log_printf(INF, "CAN-BUS writer start up.                           DONE.");
#endif
#if 0
    // 串口服务线程，和读卡器，采样盒，电能表进行数据交换，测量
    ret = pthread_create( & task->tid, &task->attr, thread_measure_service, &thread_done[3]);
    if ( 0 != ret ) {
        errcode  = 0x1003;
        log_printf(ERR,
                   "EX-measure service start up.                   FAILE!!!!");
        goto die;
    }
    log_printf(INF, "EX-measure service start up.                       DONE.");
#endif
#if 0
    // 串口服务线程，和充电机进行通信
    ret = pthread_create( & task->tid, &task->attr, thread_charger_service,
                          &thread_done[4]);
    if ( 0 != ret ) {
        errcode  = 0x1004;
        log_printf(ERR,
                   "charger service start up.                      FAILE!!!!");
        goto die;
    }
    log_printf(INF, "charger service start up.                          DONE.");
#endif
#if 0
    // 后台通信线程
    ret = pthread_create( & task->tid, &task->attr, thread_backgroud_service,
                          &thread_done[5]);
    if ( 0 != ret ) {
        errcode  = 0x1004;
        log_printf(ERR,
                   "backgroud service start up.                    FAILE!!!!");
        goto die;
    }
    log_printf(INF, "backgroud service start up.                        DONE.");
#endif
    // 充电线程，负责充电逻辑
    ret = pthread_create( & task->tid, &task->attr, thread_charge_task_service,
                          &thread_done[6]);
    if ( 0 != ret ) {
        errcode  = 0x1005;
        log_printf(ERR,
                   "charge service start up.                       FAILE!!!!");
        goto die;
    }
    log_printf(DBG_LV1, "charge service start up.                           DONE.");
#if 0
    // 串口通信线程
    ret = pthread_create( & task->tid, &task->attr, thread_uart_service,
                          &thread_done[7]);
    if ( 0 != ret ) {
        errcode  = 0x1006;
        log_printf(ERR,
                   "UART framework start up.                       FAILE!!!!");
        goto die;
    }
    log_printf(INF, "UART framework start up.                           DONE.");
#endif

#if 1
    // mongoose 线程，用来处理AJAX请求，解析由客户提交的请求，返回应答的xml文件或其他数据
    ret = pthread_create( & task->tid, &task->attr, thread_xml_service, &thread_done[0]);
    if ( 0 != ret ) {
        errcode  = 0x1000;
        log_printf(ERR,
                   "mongoose service start up.                     FAILE!!!!");
        goto die;
    }
    log_printf(DBG_LV1, "mongoose service start up.                         DONE.");
#endif

#if CONFIG_DEBUG_CONFIG >= 1
    config_print();
#endif
    // 主循环中放置看门狗代码
    for ( ;; ) {
        sleep(3);
    }
    if ( s == 0 ) {
        pthread_attr_destroy(&task->attr);
    }
    return 0;
die:
    log_printf(ERR, "going to die. system aborted!");
    return errcode;
}
