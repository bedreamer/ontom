/*
 * 定时器为系统中大量的状态机自动退出提供了重要参考依据。
 * 推荐电影《忠犬八公》，谨以这份源码赞颂狗的忠诚。
 *
 *
 */
#include <stdlib.h>
#include <pthread.h>
#include "Hachiko.h"
#include "log.h"
#include "config.h"

static void Hachiko_wangwang(int sig, siginfo_t *si, void *uc)
{
    static int cc = 0;
    log_printf(DBG, "%d", cc++);
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

    /* Establish handler for timer signal */

    log_printf(INF, "Hachiko: Establishing handler for signal %d\n", SIG);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = Hachiko_wangwang;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIG, &sa, NULL) == -1) {
        log_printf(ERR, "Hachiko: sigaction faile.");
        errExit("Hachiko: sigaction");
    }

    /* Block timer signal temporarily */

    log_printf(INF, "Hachiko: Blocking signal %d\n", SIG);
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

    log_printf(INF, "Hachiko: timer ID is 0x%lx\n", (long) timerid);

    /* Start the timer , 默认10ms*/
    freq_nanosecs = atoll(config_read("HachikoTTL"));
    if ( freq_nanosecs <= 1000000 ) {
        freq_nanosecs = 1000000;
        // 定时器最小分辨率为1ms
    }
    its.it_value.tv_sec = freq_nanosecs / 1000000000;
    its.it_value.tv_nsec = freq_nanosecs % 1000000000;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        log_printf(ERR, "Hachiko: timer_settime faile.");
         errExit("Hachiko: timer_settime");
    }

    /* Unlock the timer signal, so that timer notification
       can be delivered */

    log_printf(INF, "Hachiko: Unblocking signal %d\n", SIG);
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
        log_printf(ERR, "Hachiko: sigprocmask faile.");
        errExit("Hachiko: sigprocmask");
    }
}
