/*
 * 定时器为系统中大量的状态机自动退出提供了重要参考依据。
 * 推荐电影《忠犬八公》，谨以这份源码赞颂狗的忠诚。
 *
 * REF:
 *  http://man7.org/linux/man-pages/man2/timer_create.2.html
 *  http://linux.die.net/man/2/timer_settime
 *
 * 定时器粒度调整，为了满足串口快速切换和发送数据完成的判断，所以将定时器的分辨率从10ms
 * 提高到1ms。
 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include "Hachiko.h"
#include "log.h"
#include "config.h"
#include "error.h"
#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN
#define NR_POOL 32

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                       } while (0)
struct Hachiko_food *pool[NR_POOL] = {NULL};

/*
 * 定时器处理过程
 *
 * 按照设定的分辨率，定时进入该过程完成，相应条件的判定和处理
 */
static void Hachiko_wangwang(int sig, siginfo_t *si, void *uc)
{
    int i, refresh = 0;

    for ( i = 0; (unsigned int)i < (sizeof(pool)/sizeof(struct Hachiko_food *)); i ++ ) {
        if ( pool[i] == NULL ) continue;

        // 下面的逻辑判定顺序不可更改，详情请参考README文件
        if ( pool[i]->status == HACHIKO_INVALID ) continue;
        if ( pool[i]->status == HACHIKO_KILLED ) {
            pool[i] = NULL;
            refresh ++;
            log_printf(DBG_LV3, "HACHIKO: watch dog killed.");
            continue;
        }
        if ( pool[i]->status == HACHIKO_RESUME ) {
            Hachiko_feed(pool[i]);
            pool[i]->status = HACHIKO_NORMAL;
            continue;
        }
        if ( pool[i]->status == HACHIKO_PAUSE ) {
            Hachiko_feed(pool[i]);
            continue;
        }

        if ( pool[i]->remain ) pool[i]->remain --;

        if ( pool[i]->remain == 0 ) {
            pool[i]->Hachiko_notify_proc(HACHIKO_TIMEOUT,
                                         pool[i]->private, pool[i]);
            if ( pool[i]->type == HACHIKO_AUTO_FEED ) {
                pool[i]->remain = pool[i]->ttl;
                pool[i]->Hachiko_notify_proc(HACHIKO_FEEDED,
                                             pool[i]->private, pool[i]);
                continue;
            }
            if ( pool[i]->type == HACHIKO_ONECE ) {
                pool[i]->Hachiko_notify_proc(HACHIKO_DIE,
                                             pool[i]->private, pool[i]);
                pool[i] = NULL;
                refresh ++;
                log_printf(DBG_LV3, "HACHIKO: watch dog die.");
                continue;
            }
        }
        pool[i]->Hachiko_notify_proc(HACHIKO_ROLLING,
                                     pool[i]->private, pool[i]);
    }
    if ( 0 == refresh ) return;
}

// 设定内部功能性定时器
int _Hachiko_new(struct Hachiko_food *thiz, Hachiko_Type type,
                 unsigned int ttl, Hachiko_status status, void *private)
{
    int err = ERR_TIMER_BEMAX;
    int i = 0;

    if ( thiz->Hachiko_notify_proc == NULL ) {
        err = ERR_WRONG_PARAM;
        goto die;
    }

    for ( i = 0; (unsigned int)i < (sizeof(pool)/sizeof(struct Hachiko_food *)); i ++ ) {
        if ( pool[i] != NULL ) continue;
        thiz->type = type;
        thiz->ttl = ttl;
        thiz->remain = ttl;
        thiz->private = private;
        thiz->status = status;
        err = ERR_OK;
        pool[i] = thiz;
        log_printf(DBG_LV2, "set timer ok, type: %X, ttl: %d", type, ttl);
        break;
    }
die:
    return err;
}

// 定时器初始化
void Hachiko_init()
{
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its;
    long long freq_nanosecs;
    sigset_t mask;
    struct sigaction sa;

    memset(pool, 0, sizeof(pool));
    /* Establish handler for timer signal */

    log_printf(INF, "Hachiko: Establishing handler for signal %d", SIG);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = Hachiko_wangwang;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIG, &sa, NULL) == -1) {
        log_printf(ERR, "Hachiko: sigaction faile.");
        errExit("Hachiko: sigaction");
    }

    /* Block timer signal temporarily */

    log_printf(INF, "Hachiko: Blocking signal %d", SIG);
    sigemptyset(&mask);
    sigaddset(&mask, SIG);
    if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1) {
        log_printf(ERR, "Hachiko: sigprocmask faile.");
        errExit("Hachiko: sigprocmask");
    }

    /* Create the timer */

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIG;
    sev.sigev_value.sival_ptr = &timerid;
    if (timer_create(CLOCKID, &sev, &timerid) == -1) {
        log_printf(ERR, "Hachiko: timer_create faile.");
        errExit("Hachiko: timer_create");
    }

    log_printf(INF, "Hachiko: timer ID is 0x%lx", (long) timerid);

    /* Start the timer , 默认10ms*/
    freq_nanosecs = atoll(config_read("HachikoTTL"));
    if ( freq_nanosecs <= 100000 ) {
        freq_nanosecs = 100000;
        // 定时器最小分辨率为0.1ms
    }
    its.it_value.tv_sec = freq_nanosecs / 1000000000;
    its.it_value.tv_nsec = freq_nanosecs % 1000000000;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    log_printf(INF, "HACHIKO: timer resolution: %ld", freq_nanosecs);

    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        log_printf(ERR, "Hachiko: timer_settime faile.");
         errExit("Hachiko: timer_settime");
    }

    /* Unlock the timer signal, so that timer notification
       can be delivered */

    log_printf(INF, "Hachiko: Unblocking signal %d", SIG);
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
        log_printf(ERR, "Hachiko: sigprocmask faile.");
        errExit("Hachiko: sigprocmask");
    }
}
