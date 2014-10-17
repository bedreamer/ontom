#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
/* According to POSIX.1-2001 */
#include <sys/select.h>
/* According to earlier standards */
#include <sys/time.h>
#include "config.h"
#include "log.h"
#include "error.h"
#include "blueprint.h"

struct bp_uart uarts[] = {
    {"/dev/ttyO4", -1, BP_FRAME_UNSTABLE, 0},
    {"/dev/ttyO5", -1, BP_FRAME_UNSTABLE, 0}
};

static int bp_evt_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    switch ( evt ) {
    // 串口数据结构初始化
    case BP_EVT_INIT:
        break;
    // 串口配置
    case BP_EVT_CONFIGURE:
        return ERR_UART_OPEN_FAILE;
    // 关闭串口
    case BP_EVT_KILLED:
        break;
    // 串口数据帧校验
    case BP_EVT_FRAME_CHECK:
        break;
    // 切换到发送模式
    case BP_EVT_SWITCH_2_TX:
        break;
    // 切换到接收模式
    case BP_EVT_SWITVH_2_RX:
        break;

    // 串口接收到新数据
    case BP_EVT_RX_DATA:
        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
        break;

    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        break;
    // 串口数据发送完成事件
    case BP_EVT_TX_FRAME_DONE:
        break;

    // 串口接收单个字节超时，出现在接收帧的第一个字节
    case BP_EVT_RX_BYTE_TIMEOUT:
        break;
    // 串口接收帧超时, 接受的数据不完整
    case BP_EVT_RX_FRAME_TIMEOUT:
        break;

    // 串口IO错误
    case BP_EVT_IO_ERROR:
        break;
    // 帧校验失败
    case BP_EVT_FRAME_CHECK_ERROR:
        break;
    }
    return ERR_OK;
}

void *thread_uart_service(void *arg) ___THREAD_ENTRY___
{
    int *done = (int *)arg;
    int mydone = 0, ret;
    struct bp_uart *thiz = NULL;
    fd_set rd_set, wr_set;
    struct timeval tv;
    int retval, max_handle = 0;

    tv.tv_sec  = 0;
    tv.tv_usec = 100 * 1000; // 100 ms.

    FD_ZERO(&rd_set);
    FD_ZERO(&wr_set);
    if ( done == NULL ) done = &mydone;

    if ( thiz ) {
        // 出错误后尝试的次数
        thiz->init_magic = 5;
    }

    while ( ! *done ) {
        usleep(5000);
        if ( thiz == NULL ) continue;
        if ( thiz->status == BP_UART_STAT_ALIENT ) continue;

        if ( thiz->status == BP_UART_STAT_INVALID ) {
            if ( thiz->init_magic <= 0 ) {
                thiz->status = BP_UART_STAT_ALIENT;
                log_printf(ERR, "open UART faile, thread panic.....");
                continue;
            }

            // 初始化数据结构, 设定串口的初始状态
            // 串口的初始状态决定了串口的工作模式
            bp_evt_handle(thiz, BP_EVT_INIT, NULL);

            // 打开并配置串口
            // 如果初始化失败，则会不断的尝试
            ret = bp_evt_handle(thiz, BP_EVT_CONFIGURE, NULL);
            if ( ret == ERR_UART_OPEN_FAILE ) {
                thiz->status = BP_UART_STAT_INVALID;
                log_printf(ERR, "try open %s faile.", thiz->dev_name);
                thiz->init_magic --;
                continue;
            }
            if ( ret == ERR_UART_CONFIG_FAILE ) {
                thiz->status = BP_UART_STAT_INVALID;
                // 首先关闭串口，然后才能进行下一次尝试
                bp_evt_handle(thiz, BP_EVT_KILLED, NULL);
                log_printf(ERR, "configure %s faile.", thiz->dev_name);
                thiz->init_magic --;
                continue;
            }

            if ( thiz->status != BP_UART_STAT_RD &&
                 thiz->status != BP_UART_STAT_WR ) {
                // 默认采用被动方式
                thiz->status = BP_UART_STAT_RD;
            }

            ret = ERR_ERR;
            if ( thiz->status == BP_UART_STAT_RD ) {
                int trynr = 0;
                do {
                    usleep(50000);
                    ret = bp_evt_handle(thiz, BP_EVT_SWITVH_2_RX, NULL);
                } while ( ret != ERR_OK && trynr ++ < 100 );
            }
            if ( thiz->status == BP_UART_STAT_WR ) {
                int trynr = 0;
                do {
                    usleep(50000);
                    ret = bp_evt_handle(thiz, BP_EVT_SWITCH_2_TX, NULL);
                } while ( ret != ERR_OK && trynr ++ < 100  );
            }
            if ( ret != ERR_OK ) {
                bp_evt_handle(thiz, BP_EVT_KILLED, NULL);
                thiz->status = BP_UART_STAT_ALIENT;
                log_printf(ERR, "switch to defaute mode faile, thread panic..");
                continue;
            }

            if ( thiz->dev_handle > max_handle ) {
                max_handle = thiz->dev_handle;
            }

            FD_SET(thiz->dev_handle, &rd_set);
            FD_SET(thiz->dev_handle, &wr_set);

            log_printf(INF, "open UART correct.");
            continue;
        }

        retval =
            select(max_handle + 1, &rd_set, &wr_set, NULL, &tv);
        if ( retval == -1 ) {
            continue;
        }

        if ( thiz->status == BP_UART_STAT_RD ) {
            if ( FD_ISSET(thiz->dev_handle, &rd_set) ) {

            }
            continue;
        }

        if ( thiz->status == BP_UART_STAT_WR ) {
            if ( FD_ISSET(thiz->dev_handle, &wr_set) ) {

            }
            continue;
        }
    }
}
