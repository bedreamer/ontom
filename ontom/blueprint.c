#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include "config.h"
#include "log.h"
#include "error.h"
#include "Hachiko.h"
#include "blueprint.h"
#include "charge.h"
#include "ajax.h"

static int uart4_bp_evt_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
static int uart4_charger_yaoce_0_49_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
static int uart4_charger_yaoce_50_100_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
static int uart4_charger_config_evt_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
static int uart4_charger_module_evt_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
static int uart4_charger_date_evt_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
static int uart4_simple_box_evt_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
static int uart5_background_evt_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param);
// 共计两个串口
struct bp_uart uarts[2];
// 串口4 使用者为充电机和采样盒
struct bp_user down_user[] = {
    {50 * 100,    0, 3, 0, 0, 0, 0, 0, 0, 0, uart4_simple_box_evt_handle},     // 采样
    {50 * 100, 1000, 5, 0, 0, 0, 0, 0, 0, 0, uart4_charger_module_evt_handle}, // 充电机参数寄存器(模块控制)，读写
#if 1
    {50 * 100, 2000, 5, 0, 0, 0, 0, 0, 0, 0, uart4_charger_config_evt_handle}, // 充电机参数寄存器(参数控制)，读写
    {50 * 100, 3000, 2, 0, 0, 0, 0, 0, 0, 0, uart4_charger_date_evt_handle},   // 充电机参数寄存器(日期时间)，读写
    {50 * 100, 4000, 5, 0, 0, 0, 0, 0, 0, 0, uart4_charger_yaoce_0_49_handle},      // 盒充电机运行寄存器，只读
    {50 * 100, 4000, 5, 0, 0, 0, 0, 0, 0, 0, uart4_charger_yaoce_50_100_handle},// 盒充电机运行寄存器，只读
#endif
    {0,  0, 0, 0, 0, 0, 0, 0, 0, 0, NULL}
};
// 串口5 使用者为上位机
struct bp_user up_user[] = {
    {100, 0, 5, 0, 0, 0, 0, 0, 0, 0, uart5_background_evt_handle},    // 采样盒
    {0,  0, 0, 0, 0, 0, 0, 0, 0, 0, NULL}
};

#define GPIO_TO_PIN(bank, gpio)	(32 * (bank) + (gpio))
#define	SERIAL4_CTRL_PIN	GPIO_TO_PIN(0, 19)
#define	SERIAL5_CTRL_PIN	GPIO_TO_PIN(0, 20)

#define	RX_LOW_LEVEL			0
#define	TX_HIGH_LEVEL			1

int configure_uart(int fd, int baud_rate, int databits, int stopbits, int parity)
{
    struct termios options;
    int   status;

    if (tcgetattr(fd, &options) != 0) {
        perror("SetupSerial 1");
        return ERR_UART_CONFIG_FAILE;
    }

    tcflush(fd, TCIOFLUSH);
    cfsetispeed(&options, baud_rate);
    cfsetospeed(&options, baud_rate);
    status = tcsetattr(fd, TCSANOW, &options);

    if (tcgetattr(fd, &options) != 0) {
        perror("SetupSerial 1");
        return ERR_UART_CONFIG_FAILE;
    }
    options.c_cflag &= ~CSIZE;

    switch (databits) {
        default:
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
    }

    switch (parity) {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;   /* Clear parity enable */
            options.c_iflag &= ~INPCK;     /* Enable parity checking */
            break;
        case 'o':
        case 'O':
        case 1:
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK;             /* Disnable parity checking */
            break;
        case 'e':
        case 'E':
        case 2:
            options.c_cflag |= PARENB;     /* Enable parity */
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;      /* Disnable parity checking */
            break;
        default:
        case 'S':
        case 's':  /*as no parity*/
        case 0:
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
    }

    switch (stopbits) {
        default:
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
    }

    /* Set input parity option */
    if (parity != 'n')
        options.c_iflag |= INPCK;

    //options.c_cflag   |= CRTSCTS;
    options.c_iflag &=~(IXON | IXOFF | IXANY);
    options.c_iflag &=~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //options.c_lflag |= ISIG;

    tcflush(fd,TCIFLUSH);
    options.c_oflag = 0;
    //options.c_lflag = 0;
    options.c_cc[VTIME] = 0; 						// delay 15 seconds
    options.c_cc[VMIN] = 0; 						// Update the options and do it NOW

    if (tcsetattr(fd,TCSANOW,&options) != 0) {
        perror("SetupSerial 3");
        return ERR_UART_CONFIG_FAILE;
    }

    return ERR_OK;
}

/********************************************************
Name:				gpio_export
Description:		导出所需要控制的GPIO的引脚编号
Input:
Return:
*********************************************************/
void gpio_export(int pin)
{
    // 导出所需要设置的GPIO口
    FILE *fp = fopen("/sys/class/gpio/export","w");
    rewind(fp);
    fprintf(fp, "%d\n", pin);
    fclose(fp);
    return;
}

/********************************************************
Name:				gpio_unexport
Description:		释放所需要控制的GPIO的引脚编号
Input:
Return:
*********************************************************/
void gpio_unexport(int pin)
{
    // 释放导出的GPIO口
    FILE *fp = fopen("/sys/class/gpio/unexport","w");
    rewind(fp);
    fprintf(fp, "%d\n", pin);
    fclose(fp);
    return;
}

/***********************************************************
Name:				set_gpio_output
Description:		设置gpio引脚为输出，并设定输出值
Input:
    1、pin:			gpio引脚号
    2、value:		0:低电平 1:高电平
Return:				0
************************************************************/
int set_gpio_output(int pin, int value)
{
    char file[40], direction[5];

    sprintf(file, "/sys/class/gpio/gpio%d/direction", pin);
    FILE *fp = fopen(file, "w");

    sprintf(direction,"out");

    rewind(fp);
    if (fprintf(fp, "%s",direction) < 0)
        return -2;

    fclose(fp);

    sprintf(file, "/sys/class/gpio/gpio%d/value", pin);
    fp = fopen(file, "w");
    rewind(fp);
    fprintf(fp, "%d\n", value);
    fclose(fp);

    return ERR_OK;
}

/*
 * 串口4作为互动设备是需要该信号的，因此在这里可以不用做判断是那个串口产生了该信号
 */
void uarts_async_sigio(int param)
{
#if (CONFIG_SUPPORT_SIGIO > 0)
    struct bp_uart * thiz = &uarts[0];
    struct Hachiko_food *p;
    p = & (thiz->tx_seed);

    log_printf(INF, "UART: "RED("SIGIO")" fetched. %d", param);
    if ( BP_UART_STAT_WR == thiz->hw_status && thiz->tx_param.payload_size ) {
        thiz->bp_evt_handle(thiz, BP_EVT_TX_FRAME_DONE, &thiz->tx_param);
        thiz->tx_param.payload_size = 0;
        Hachiko_pause(p);
        memset(thiz->rx_param.buff.rx_buff, 0, thiz->rx_param.buff_size);
        thiz->status = BP_UART_STAT_RD;
        if ( thiz->role == BP_UART_MASTER ) {
            // 主动设备，需要进行接收超时判定
            Hachiko_resume(&thiz->rx_seed);
        }
    } else{
        log_printf(DBG_LV1, "UART: hw_status: %X, payload: %d",
                   thiz->hw_status, thiz->tx_param.payload_size);
    }
#endif
}

// 串口4的超时响应
void uart4_Hachiko_notify_proc(Hachiko_EVT evt, void *private,
                            const struct Hachiko_food *self)
{
    struct bp_uart * thiz = (struct bp_uart * __restrict__)private;
    struct Hachiko_food *p;

    if ( evt != HACHIKO_TIMEOUT ) return;
    p = & (thiz->rx_seed);

    if ( self == p ) {
        Hachiko_pause(&thiz->rx_seed);
        if ( thiz->rx_param.need_bytes == thiz->rx_param.payload_size ) {
            log_printf(INF, "UART: rx packet TIME-OUT.need: %d, fetched: "GRN("%d"),
                       thiz->rx_param.need_bytes,
                        thiz->rx_param.payload_size);
        } else {
            log_printf(WRN, "UART: rx packet TIME-OUT.need: %d, fetched: "YEL("%d")/*"gave crc: %02X%02X need: %04X"*/,
                       thiz->rx_param.need_bytes,
                        thiz->rx_param.payload_size/*,
                       thiz->rx_param.buff.rx_buff[thiz->rx_param.need_bytes-1],
                    thiz->rx_param.buff.rx_buff[thiz->rx_param.need_bytes],
                    load_crc(thiz->rx_param.need_bytes-2, thiz->rx_param.buff.rx_buff)*/);
            log_printf(DBG_LV1,
                       "UART: RD:<"
                       "%02X %02X %02X %02X %02X %02X %02X %02X "
                       "%02X %02X %02X %02X %02X %02X %02X %02X "
                       "%02X %02X %02X %02X>",
                       thiz->rx_param.buff.rx_buff[0], thiz->rx_param.buff.rx_buff[1],
                       thiz->rx_param.buff.rx_buff[2], thiz->rx_param.buff.rx_buff[3],
                       thiz->rx_param.buff.rx_buff[4], thiz->rx_param.buff.rx_buff[5],
                       thiz->rx_param.buff.rx_buff[6], thiz->rx_param.buff.rx_buff[7],
                       thiz->rx_param.buff.rx_buff[0+8], thiz->rx_param.buff.rx_buff[1+8],
                       thiz->rx_param.buff.rx_buff[2+8], thiz->rx_param.buff.rx_buff[3+8],
                       thiz->rx_param.buff.rx_buff[4+8], thiz->rx_param.buff.rx_buff[5+8],
                       thiz->rx_param.buff.rx_buff[6+8], thiz->rx_param.buff.rx_buff[7+8],
                       thiz->rx_param.buff.rx_buff[8+8], thiz->rx_param.buff.rx_buff[9+8],
                       thiz->rx_param.buff.rx_buff[10+8], thiz->rx_param.buff.rx_buff[11+8]);
        }
        if ( thiz->rx_param.payload_size == 0 ) {
            thiz->bp_evt_handle(thiz, BP_EVT_RX_BYTE_TIMEOUT, &thiz->rx_param);
        } else if ( thiz->rx_param.payload_size < thiz->rx_param.need_bytes ) {
            thiz->bp_evt_handle(thiz, BP_EVT_RX_FRAME_TIMEOUT, &thiz->rx_param);
        } else {
            // all thing is ok.
        }
        thiz->status = BP_UART_STAT_WR;
        return;
    }

#if (CONFIG_SUPPORT_SIGIO > 0)
    /*
     * 串口发送完成事件由系统提供SIGIO信号来确定，具体逻辑见函数
     *  void uarts_async_sigio(int param)
     */
#else
    #if (CONFIG_SUPPORT_ASYNC_UART_TX == 1)
    p = & thiz->tx_seed;
    if ( self == p ) {
        Hachiko_pause(p);
        static struct bp_user *pre = NULL;
        if ( thiz->master != pre ) {
            pre = thiz->master;
        } else {
            log_printf(WRN, "HACHIKO: renter.... %p != %p", pre, thiz->master);
        }
        log_printf(DBG_LV0, "UART: packet send done.");
        thiz->bp_evt_handle(thiz, BP_EVT_TX_FRAME_DONE, &thiz->tx_param);
        thiz->tx_param.payload_size = 0;
        memset(thiz->rx_param.buff.rx_buff, 0, thiz->rx_param.buff_size);
        thiz->status = BP_UART_STAT_RD;
        if ( thiz->role == BP_UART_MASTER ) {
            // 主动设备，需要进行接收超时判定
            Hachiko_resume(&thiz->rx_seed);
        }
        return;
    }
    #endif
#endif
}

// 串口4的发送节奏定时器
void uart4_Hachiko_speed_proc(Hachiko_EVT evt, void *private,
                            const struct Hachiko_food *self)
{
    struct bp_uart * thiz = (struct bp_uart * __restrict__)private;
    struct bp_user *u;

    for ( u = thiz->users; u->user_evt_handle; u ++ ) {
        if ( u->seed <= u->frame_freq ) {
            u->seed ++;
        }
        //log_printf(DBG, "%p: %d-%d", u->user_evt_handle, u->frame_freq, u->seed);
    }
}

/*
 * 串口事件响应函数
 */
static int uart4_bp_evt_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_OK;
    int i, nr;
    struct bp_user *hit;

    switch ( evt ) {
    // 串口数据结构初始化
    case BP_EVT_INIT:
        self->role = BP_UART_MASTER;
        self->rx_seed.private = (void*)self;
        self->rx_seed.Hachiko_notify_proc = uart4_Hachiko_notify_proc;
        self->rx_param.buff.rx_buff = self->rx_buff;
        self->rx_param.cursor = 0;
        self->rx_param.payload_size = 0;
        self->rx_param.buff_size = sizeof(self->rx_buff);

        self->users = &down_user[0];
        self->master =NULL;// &self->users[0];
        self->sequce = 10;
        self->continues_nr = 0;

        ret = _Hachiko_new(&self->rx_seed, HACHIKO_AUTO_FEED,
                     2000, HACHIKO_PAUSE, (void*)self);
        if ( ret != ERR_OK ) {
            log_printf(ERR, "UART: create uart reciever's timer faile.");
        }
#if (CONFIG_SUPPORT_ASYNC_UART_TX == 1)
        self->tx_seed.private = (void*)self;
        self->tx_seed.Hachiko_notify_proc = uart4_Hachiko_notify_proc;
        self->tx_param.payload_size = 0;
        self->tx_param.cursor = 0;
        self->tx_param.buff.tx_buff = self->tx_buff;
        self->tx_param.buff_size = sizeof(self->tx_buff);
        ret = _Hachiko_new(&self->tx_seed, HACHIKO_AUTO_FEED,
                     2, HACHIKO_PAUSE, (void*)self);
        if ( ret != ERR_OK ) {
            log_printf(ERR, "UART: create uart transfer's timer faile.");
        }
#endif
        self->tx_speed.private = (void*)self;
        self->tx_speed.Hachiko_notify_proc = uart4_Hachiko_speed_proc;
        ret = _Hachiko_new(&self->tx_speed, HACHIKO_AUTO_FEED,
                     12, HACHIKO_PAUSE, (void*)self);
        if ( ret != ERR_OK ) {
            log_printf(ERR, "UART: create uart transfer's speed timer faile.");
        }
        break;
    // 串口配置
    case BP_EVT_CONFIGURE:
        gpio_export(SERIAL4_CTRL_PIN);
        self->dev_handle = open(self->dev_name,
                                O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
        if ( self->dev_handle == -1 ) {
            return ERR_UART_OPEN_FAILE;
        } else {
            log_printf(DBG, "UART: self->dev_handle: %d", self->bp_evt_handle);
        }
#if (CONFIG_SUPPORT_SIGIO > 0)
        if ( -1 == fcntl(self->dev_handle, F_SETFL, FASYNC) ) {
            log_printf(ERR,
                    "UART: set uart to async mode failed!!! errno: %d", errno);
        }
#endif
        ret = configure_uart(self->dev_handle, B9600, 8, 1, 'N');
        if ( ret == (int)ERR_UART_CONFIG_FAILE ) {
            log_printf(ERR, "UART: configure uart faile.");
            return ERR_UART_CONFIG_FAILE;
        }
        self->status = BP_UART_STAT_WR;
        self->hw_status = BP_UART_STAT_INVALID;

        // 开始发送节奏计数
        Hachiko_resume(&self->tx_speed);
        break;
    // 关闭串口
    case BP_EVT_KILLED:
        gpio_unexport(SERIAL4_CTRL_PIN);
        close(self->dev_handle);
        break;
    // 串口数据帧校验
    case BP_EVT_FRAME_CHECK:
        ret = ERR_ERR;
        if ( self->master && self->master->user_evt_handle ) {
            ret = self->master->user_evt_handle(self, BP_EVT_FRAME_CHECK, param);
        }
        break;
    // 切换到发送模式
    case BP_EVT_SWITCH_2_TX:
        ret = set_gpio_output(SERIAL4_CTRL_PIN, TX_HIGH_LEVEL);
        if ( self->master ) {
            //self->master->seed = 0;
        }
        if ( ret != ERR_OK ) {
            log_printf(ERR, "UART: set uart to TX mode faile");
            break;
        }
        break;
    // 切换到接收模式
    case BP_EVT_SWITCH_2_RX:
        ret = set_gpio_output(SERIAL4_CTRL_PIN, RX_LOW_LEVEL);
        if ( self->master ) {
            //self->master->seed = 0;
        }
        if ( ret != ERR_OK ) {
            log_printf(ERR, "UART: set uart to RX mode faile");
            break;
        }
        break;

    // 串口接收到新数据
    case BP_EVT_RX_DATA:
        if ( self->master && self->master->user_evt_handle ) {
            ret = self->master->user_evt_handle(self, BP_EVT_RX_DATA, param);
        }
        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
        /*
         * 数据帧格式兼容性问题：
         * 目前可能出现的数据帧格式为：
         *   格式1： 魔数 + 地址 + 长度 + 数据 + 校验   或者
         *   格式2： 地址 + 长度 + 数据 + 校验
         * 因此数据帧识别问题被归结为数据帧长度的识别，格式1中为了降低数据与控制字节之间
         * 的重叠问题，所以采用了魔数方式填充数据帧头，魔数长度不定，因此需要在数据帧定义
         * 的数据结构中加上帧头的魔数长度描述
         */
        if ( self->master && self->master->user_evt_handle ) {
            ret = self->master->user_evt_handle(self, BP_EVT_RX_FRAME, param);
        }

        self->master->rcv_ok_cnt ++;
        self->master->check_err_cnt = 0;
        self->master->died = 0;
        break ;

    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        if ( param->payload_size ) return ERR_ERR;
#if (CONFIG_SUPPORT_FRAME_BALANCE == 0)
        for ( u = self->users; u->user_evt_handle; u ++ ) {
            if ( u->seed > u->frame_freq && self->master != u ) {
                self->master = u;
                /*
                 * 需要在私有事件处理过程中进行事件处理，数据填充
                 * 数据填充格式按照下面的步骤进行：
                 * param->attrib = BP_FRAME_UNSTABLE;
                 *    ......
                 * memcpy(param->buff.tx_buff, &qry, sizeof(qry));
                 * param->payload_size = sizeof(qry);
                 */
                ret = u->user_evt_handle(self, BP_EVT_TX_FRAME_REQUEST, param);
                log_printf(DBG_LV0, "UART: ret: %d, load: %d", ret, param->payload_size);
                break;
            }
        }
#elif ( CONFIG_SUPPORT_FRAME_BALANCE == 1 )
        /*
         * 采用帧发送均衡算法，该串口上的使用者帧率之和为10000
         */
        //self->master = NULL;
        nr = (sizeof(down_user) - sizeof(struct bp_user)) / sizeof(struct bp_user);
        i = self->sequce % nr;
        log_printf(DBG_LV0, "下一个发送序列为: %d:%d", self->sequce, i);
        hit = &down_user[ i ];
        /*
        for ( u = self->users, hit = self->users; u->user_evt_handle; u ++ ) {
            if ( u->seed > u->frame_freq && self->master != u &&
                 u->sent_frames < hit->sent_frames ) {
                hit = u;
            }
        }
        */
        self->sequce ++;
        if ( self->master != hit || (self->master == hit && self->continues_nr) ) {
            self->master = hit;
            self->master->seed = 0;
            ret = hit->user_evt_handle(self, BP_EVT_TX_FRAME_REQUEST, param);
            log_printf(DBG_LV1, "UART: ret: %d, load: %d, sent: %d",
                       ret, param->payload_size, hit->sent_frames);
        } else {
            self->sequce --;
            ret = ERR_ERR;
        }
        break;
#else
        param->attrib = BP_FRAME_UNSTABLE;
        struct MDATA_QRY qry;
        qry.magic[0] = 0xF0;
        qry.magic[1] = 0xE1;
        qry.magic[2] = 0xD2;
        qry.magic[3] = 0xC3;
        qry.magic[4] = 0xB4;
        qry.addr = 0x05;
        qry.len = 16;
        qry.crc = 0xFFFF;
        memcpy(param->buff.tx_buff, &qry, sizeof(qry));
        param->payload_size = sizeof(qry);
#endif
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        if ( self->master && self->master->user_evt_handle ) {
            ret = self->master->user_evt_handle(self, BP_EVT_TX_FRAME_CONFIRM, param);
        } else {
            log_printf(WRN, "UART: "RED("BP_EVT_TX_FRAME_CONFIRM")" without signal procedure.");
        }
        break;
    // 串口数据发送完成事件
    case BP_EVT_TX_FRAME_DONE:
        self->master->sent_frames ++;

        if ( self->master ) {
            //self->master->seed = 0;
        }
        if ( self->master && self->master->user_evt_handle ) {
            //self->master->sent_frames ++;
            log_printf(DBG_LV0, "BP_EVT_TX_FRAME_DONE %p, %d",
                       self->master,
                       self->master->sent_frames);
            ret = self->master->user_evt_handle(self, BP_EVT_TX_FRAME_DONE, param);
        } else {
            log_printf(WRN, "UART: "RED("BP_EVT_TX_FRAME_DONE")" without signal procedure.");
        }
        break;

    // 串口接收单个字节超时，出现在接收帧的第一个字节
    case BP_EVT_RX_BYTE_TIMEOUT:
        self->master->died ++;
        self->master->died_total ++;

        log_printf(DBG_LV1, "UART: no data fetched.");
        if ( self->master && self->master->user_evt_handle ) {
            ret = self->master->user_evt_handle(self, BP_EVT_RX_BYTE_TIMEOUT, param);
        } else {
            log_printf(WRN, "UART: "RED("BP_EVT_RX_BYTE_TIMEOUT")" without signal procedure.");
        }
        break;
    // 串口接收帧超时, 接受的数据不完整
    case BP_EVT_RX_FRAME_TIMEOUT:
        self->master->died ++;
        self->master->died_total ++;

        log_printf(DBG_LV1, "UART: not all data fetched yet.");
        if ( self->master && self->master->user_evt_handle ) {
            ret = self->master->user_evt_handle(self, BP_EVT_RX_FRAME_TIMEOUT, param);
        } else {
            log_printf(WRN, "UART: "RED("BP_EVT_RX_FRAME_TIMEOUT")" without signal procedure.");
        }
        break;

    // 串口IO错误
    case BP_EVT_IO_ERROR:
        break;
    // 帧校验失败
    case BP_EVT_FRAME_CHECK_ERROR:
        self->master->check_err_cnt ++;
        self->master->check_err_total ++;

        log_printf(DBG_LV1, "UART: all data fetched but CRC check failed.");
        if ( self->master && self->master->user_evt_handle ) {
            ret = self->master->user_evt_handle(self, BP_EVT_FRAME_CHECK_ERROR, param);
        } else {
            log_printf(WRN, "UART: "RED("BP_EVT_FRAME_CHECK_ERROR")" without signal procedure.");
        }
        break;
    }
    return ret;
}

// 只读数据段
static int uart4_charger_yaoce_0_49_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_ERR;
    char buff[8];

    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size < param->need_bytes ) {
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
        } else {
            unsigned short crc = load_crc(param->need_bytes-2, param->buff.rx_buff);
            unsigned short check = param->buff.rx_buff[ param->need_bytes - 2 ] |
                    param->buff.rx_buff[ param->need_bytes - 1] << 8;
            log_printf(DBG_LV2, "UART: CRC cheke result: need: %04X, gave: %04X",
                       crc, check);
            if ( crc != check ) {
                ret = ERR_FRAME_CHECK_ERR;
            } else {
                ret = ERR_OK;
            }
        }
        break;
    // 串口接收到新数据
    case BP_EVT_RX_DATA:
        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
        if ( self->master->died >= self->master->died_line ) {
            bit_set(task, S_CHARGER_YX_1_COMM_DOWN);
            log_printf(INF, "UART: "GRN("充电机监控通讯(次要0-49)恢复"));
        }
        memcpy(&task->chargers, &param->buff.rx_buff[3], 100);
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        param->attrib = BP_FRAME_UNSTABLE;
        buff[0] = 0x01;
        buff[1] = 0x04;
        buff[2] = buff[3] = 0x00;
        buff[4] = 0x00;
        buff[5] = 0x32;
        buff[6] = 0x71;
        buff[7] = 0xDF;
        memcpy(param->buff.tx_buff, buff, sizeof(buff));

        param->payload_size = sizeof(buff);
        self->rx_param.need_bytes = 105;
        self->master->time_to_send = param->payload_size * 1000 / 960 + 1;

        ret = ERR_OK;
        log_printf(DBG_LV3, "UART: %s sent", __FUNCTION__);
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        ret = ERR_OK;
        break;
    // 串口数据发送完成事件
    case BP_EVT_TX_FRAME_DONE:
        log_printf(DBG_LV3, "UART: %s packet send done", __FUNCTION__);
        break;
    // 串口接收单个字节超时，出现在接收帧的第一个字节
    case BP_EVT_RX_BYTE_TIMEOUT:
    // 串口接收帧超时, 接受的数据不完整
    case BP_EVT_RX_FRAME_TIMEOUT:
        //self->master->died ++;
        if ( self->master->died >= self->master->died_line ) {
            bit_set(task, S_CHARGER_YX_1_COMM_DOWN);
            log_printf(ERR, "UART: "RED("充电机监控通讯(次要0-49)中断"));
        }
        log_printf(WRN, "UART: %s get signal TIMEOUT", __FUNCTION__);
        break;
    // 串口IO错误
    case BP_EVT_IO_ERROR:
        break;
    // 帧校验失败
    case BP_EVT_FRAME_CHECK_ERROR:
        break;
    default:
        log_printf(WRN, "UART: unreliable EVENT %08Xh", evt);
        break;
    }
    return ret;
}

static int uart4_charger_yaoce_50_100_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_ERR;
    char buff[8];

    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size < param->need_bytes ) {
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
        } else {
            unsigned short crc = load_crc(param->need_bytes-2, param->buff.rx_buff);
            unsigned short check = param->buff.rx_buff[ param->need_bytes - 2 ] |
                    param->buff.rx_buff[ param->need_bytes - 1] << 8;
            log_printf(DBG_LV2, "UART: CRC cheke result: need: %04X, gave: %04X",
                       crc, check);
            if ( crc != check ) {
                ret = ERR_FRAME_CHECK_ERR;
            } else {
                ret = ERR_OK;
            }
        }
        break;
    // 串口接收到新数据
    case BP_EVT_RX_DATA:
        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
        if ( self->master->died >= self->master->died_line ) {
            bit_set(task, S_CHARGER_YX_2_COMM_DOWN);
            log_printf(INF, "UART: "GRN("充电机监控通讯(次要50-100)恢复"));
        }
        memcpy(&task->chargers.charge_module_status, &param->buff.rx_buff[3], 100);
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        param->attrib = BP_FRAME_UNSTABLE;
        buff[0] = 0x01;
        buff[1] = 0x04;
        buff[2] = 0x00;
        buff[3] = 0x32;
        buff[4] = 0x00;
        buff[5] = 0x32;
        buff[6] = 0xD0;
        buff[7] = 0x10;
        memcpy(param->buff.tx_buff, buff, sizeof(buff));

        param->payload_size = sizeof(buff);
        self->rx_param.need_bytes = 105;
        self->master->time_to_send = param->payload_size * 1000 / 960 + 1;

        ret = ERR_OK;
        log_printf(DBG_LV3, "UART: %s sent", __FUNCTION__);
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        ret = ERR_OK;
        break;
    // 串口数据发送完成事件
    case BP_EVT_TX_FRAME_DONE:
        log_printf(DBG_LV3, "UART: %s packet send done", __FUNCTION__);
        break;
    // 串口接收单个字节超时，出现在接收帧的第一个字节
    case BP_EVT_RX_BYTE_TIMEOUT:
    // 串口接收帧超时, 接受的数据不完整
    case BP_EVT_RX_FRAME_TIMEOUT:
        //self->master->died ++;
        if ( self->master->died >= self->master->died_line ) {
            bit_set(task, S_CHARGER_YX_2_COMM_DOWN);
            log_printf(ERR, "UART: "RED("充电机监控通讯(次要50-100)中断"));
        }
        log_printf(WRN, "UART: %s get signal TIMEOUT", __FUNCTION__);
        break;
    // 串口IO错误
    case BP_EVT_IO_ERROR:
        break;
    // 帧校验失败
    case BP_EVT_FRAME_CHECK_ERROR:
        break;
    default:
        log_printf(WRN, "UART: unreliable EVENT %08Xh", evt);
        break;
    }
    return ret;
}

// 配置数据,系统需求电压，需求电流配置
static int uart4_charger_config_evt_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_ERR;
    unsigned int val;
    unsigned char buff[32], nr = 0, s;

    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size < param->need_bytes ) {
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
        } else {
            unsigned short crc = load_crc(param->need_bytes-2, param->buff.rx_buff);
            unsigned short check = param->buff.rx_buff[ param->need_bytes - 2 ] |
                    param->buff.rx_buff[ param->need_bytes - 1] << 8;
            log_printf(DBG_LV2, "UART: CRC cheke result: need: %04X, gave: %04X",
                       crc, check);
            if ( crc != check ) {
                ret = ERR_FRAME_CHECK_ERR;
            } else {
                ret = ERR_OK;
            }
        }
        break;
    // 串口接收到新数据
    case BP_EVT_RX_DATA:
        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
        if ( bit_read(task, S_CHARGER_COMM_DOWN) ) {
            bit_clr(task, S_CHARGER_COMM_DOWN);
            log_printf(INF, "UART: "GRN("充电桩监控通信(主要)恢复"));
        }
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        param->attrib = BP_FRAME_UNSTABLE;
        buff[nr ++] = 0x01;
        buff[nr ++] = 0x10;
        buff[nr ++] = 0x00;
        buff[nr ++] = 0x12;
        buff[nr ++] = 0x00;
        buff[nr ++] = 0x04;
        buff[nr ++] = 0x08;

        val = (unsigned int)atoi(config_read("初始电压"));
        // 初始电压
        buff[nr ++] = l2b((unsigned short)val);
        buff[nr ++] = l2b((unsigned short)val) >> 8;

        // 充电命令
        buff[nr ++] = 0x00;
        buff[nr ++] = 0x01;

        val = (unsigned int)atoi(config_read("需求电压"));
        // 需求电压
        buff[nr ++] = l2b((unsigned short)val);
        buff[nr ++] = l2b((unsigned short)val) >> 8;

        val = (unsigned int)atoi(config_read("需求电流"));
        // 需求电流
        buff[nr ++] = l2b((unsigned short)val);
        buff[nr ++] = l2b((unsigned short)val) >> 8;
        s = nr;

        // CRC
        buff[nr ++] = load_crc(s, (char*)buff);
        buff[nr ++] = load_crc(s, (char*)buff) >> 8;
        memcpy(param->buff.tx_buff, buff, nr);
        param->payload_size = nr;
        ret = ERR_OK;

        self->rx_param.need_bytes = 8;
        self->master->time_to_send = param->payload_size * 1000 / 960 + 2;

        log_printf(DBG_LV3, "UART: %s sent", __FUNCTION__);
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        ret = ERR_OK;
        break;
    // 串口数据发送完成事件
    case BP_EVT_TX_FRAME_DONE:
        log_printf(DBG_LV3, "UART: %s packet send done", __FUNCTION__);
        break;
    // 串口接收单个字节超时，出现在接收帧的第一个字节
    case BP_EVT_RX_BYTE_TIMEOUT:
    // 串口接收帧超时, 接受的数据不完整
    case BP_EVT_RX_FRAME_TIMEOUT:
        //self->master->died ++;
        if ( self->master->died >= self->master->died_line ) {
            bit_set(task, S_CHARGER_COMM_DOWN);
            log_printf(ERR, "UART: "RED("充电机监控通讯(主要)中断"));
        }
        log_printf(WRN, "UART: %s get signal TIMEOUT", __FUNCTION__);
        break;
    // 串口IO错误
    case BP_EVT_IO_ERROR:
        break;
    // 帧校验失败
    case BP_EVT_FRAME_CHECK_ERROR:
        break;
    default:
        log_printf(WRN, "UART: unreliable EVENT %08Xh", evt);
        break;
    }
    return ret;
}

// 模块操作
static int uart4_charger_module_evt_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_ERR;
    unsigned char buff[8];

    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        ret = ERR_ERR;
        if ( param->payload_size < param->need_bytes ) {
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
        } else {
            unsigned short crc = load_crc(param->need_bytes-2, param->buff.rx_buff);
            unsigned short check = param->buff.rx_buff[ param->need_bytes - 2 ] |
                    param->buff.rx_buff[ param->need_bytes - 1] << 8;
            log_printf(DBG_LV2, "UART: CRC cheke result: need: %04X, gave: %04X",
                       crc, check);
            if ( crc != check ) {
                ret = ERR_FRAME_CHECK_ERR;
            } else {
                ret = ERR_OK;
            }
        }
        break;
    // 串口接收到新数据
    case BP_EVT_RX_DATA:
        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        param->attrib = BP_FRAME_UNSTABLE;
        buff[0] = 0x01;
        buff[1] = 0x04;
        buff[2] = buff[3] = 0x00;
        buff[4] = 0x00;
        buff[5] = 0x06;
        buff[6] = 0x70;
        buff[7] = 0x08;

        self->rx_param.need_bytes = 17;

        memcpy(param->buff.tx_buff, buff, sizeof(buff));
        param->payload_size = sizeof(buff);
        self->master->time_to_send = param->payload_size * 1000 / 960;
        ret = ERR_ERR;
        log_printf(DBG_LV3, "UART: %s sent", __FUNCTION__);
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        ret = ERR_OK;
        break;
    // 串口数据发送完成事件
    case BP_EVT_TX_FRAME_DONE:
        log_printf(DBG_LV3, "UART: %s packet send done", __FUNCTION__);
        break;
    // 串口接收单个字节超时，出现在接收帧的第一个字节
    case BP_EVT_RX_BYTE_TIMEOUT:
    // 串口接收帧超时, 接受的数据不完整
    case BP_EVT_RX_FRAME_TIMEOUT:
        //self->master->died ++;
        log_printf(WRN, "UART: %s get signal TIMEOUT", __FUNCTION__);
        break;
    // 串口IO错误
    case BP_EVT_IO_ERROR:
        break;
    // 帧校验失败
    case BP_EVT_FRAME_CHECK_ERROR:
        break;
    default:
        log_printf(WRN, "UART: unreliable EVENT %08Xh", evt);
        break;
    }
    return ret;
}

// 系统校时
static int uart4_charger_date_evt_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_ERR;
    char buff[8];

    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size < param->need_bytes ) {
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
        } else {
            unsigned short crc = load_crc(param->need_bytes-2, param->buff.rx_buff);
            unsigned short check = param->buff.rx_buff[ param->need_bytes - 2 ] |
                    param->buff.rx_buff[ param->need_bytes - 1] << 8;
            log_printf(DBG_LV2, "UART: CRC cheke result: need: %04X, gave: %04X",
                       crc, check);
            if ( crc != check ) {
                ret = ERR_FRAME_CHECK_ERR;
            } else {
                ret = ERR_OK;
            }
        }
        break;
    // 串口接收到新数据
    case BP_EVT_RX_DATA:
        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        param->attrib = BP_FRAME_UNSTABLE;
        buff[0] = 0x01;
        buff[1] = 0x04;
        buff[2] = buff[3] = 0x00;
        buff[4] = 0x00;
        buff[5] = 0x06;
        buff[6] = 0x70;
        buff[7] = 0x08;

        self->rx_param.need_bytes = 17;

        memcpy(param->buff.tx_buff, buff, sizeof(buff));
        param->payload_size = sizeof(buff);
        self->master->time_to_send = param->payload_size * 1000 / 960;
        ret = ERR_ERR;
        log_printf(DBG_LV3, "UART: %s sent", __FUNCTION__);
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        ret = ERR_OK;
        break;
    // 串口数据发送完成事件
    case BP_EVT_TX_FRAME_DONE:
        log_printf(DBG_LV3, "UART: %s packet send done", __FUNCTION__);
        break;
    // 串口接收单个字节超时，出现在接收帧的第一个字节
    case BP_EVT_RX_BYTE_TIMEOUT:
    // 串口接收帧超时, 接受的数据不完整
    case BP_EVT_RX_FRAME_TIMEOUT:
        //self->master->died ++;
        log_printf(WRN, "UART: %s get signal TIMEOUT", __FUNCTION__);
        break;
    // 串口IO错误
    case BP_EVT_IO_ERROR:
        break;
    // 帧校验失败
    case BP_EVT_FRAME_CHECK_ERROR:
        break;
    default:
        log_printf(WRN, "UART: unreliable EVENT %08Xh", evt);
        break;
    }
    return ret;
}

static int uart4_simple_box_evt_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_ERR, nr = 0, len = 0, errnr = 0;
    char buff[32] = {0};
    struct MDATA_ACK *me, *me_pre;
    char errstr[1024] = {0};
    char infstr[1024] = {0};
    char cmd = 0;
    int need_echo = 0;

    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size < param->need_bytes ) {
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
        } else {
            unsigned short crc = load_crc(param->need_bytes-2, param->buff.rx_buff);
            unsigned short check = param->buff.rx_buff[ param->need_bytes - 2 ] |
                    param->buff.rx_buff[ param->need_bytes - 1] << 8;
            log_printf(DBG_LV2, "UART: CRC cheke result: need: %04X, gave: %04X",
                       crc, check);
            if ( crc != check ) {
                ret = ERR_FRAME_CHECK_ERR;
            } else {
                ret = ERR_OK;
            }
        }
        break;
    // 串口接收到新数据
    case BP_EVT_RX_DATA:
        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
        if ( bit_read(task, S_MEASURE_COMM_DOWN) ) {
            log_printf(INF, "UART: "GRN("综合采样盒通信恢复."));
        }
        bit_clr(task, S_MEASURE_COMM_DOWN);
        self->master->died = 0;

        memcpy(&task->measure, param->buff.rx_buff, sizeof(struct MDATA_ACK));
        // 故障判定
        me = &task->measure;
        me_pre = &task->measure_pre_copy;
        if ( me->yx_mx_V_high ) {
            len += sprintf(&errstr[len], "[%d: 母线过压] ", ++errnr);
            bit_set(task, S_BUS_V_HI);
        } else {
            bit_clr(task, S_BUS_V_HI);
        }
        if ( me->yx_mx_V_low ) {
            len += sprintf(&errstr[len], "[%d: 母线欠压] ", ++errnr);
            bit_set(task, S_BUS_V_LO);
        } else {
            bit_clr(task, S_BUS_V_LO);
        }
        if ( me->yx_mx_short_fault ) {
            len += sprintf(&errstr[len], "[%d: 母线短路] ", ++errnr);
            bit_set(task, S_BUS_SHORTED);
        } else {
            bit_clr(task, S_BUS_SHORTED);
        }

        if ( me->yx_bat_V_high ) {
            len += sprintf(&errstr[len], "[%d: 电池过压] ", ++errnr);
            bit_set(task, S_BAT_V_HI);
        } else {
            bit_clr(task, S_BAT_V_HI);
        }
        if ( me->yx_bat_V_low ) {
            len += sprintf(&errstr[len], "[%d: 电池欠压] ", ++errnr);
            bit_set(task, S_BAT_V_LO);
        } else {
            bit_clr(task, S_BAT_V_LO);
        }
        if ( me->yx_bat_short_fault ) {
            len += sprintf(&errstr[len], "[%d: 电池链接短路] ", ++errnr);
            bit_set(task, S_BAT_SHORTED);
        } else {
            bit_clr(task, S_BAT_SHORTED);
        }
        if ( me->yx_bat_revers_conn ) {
            len += sprintf(&errstr[len], "[%d: 电池反接] ", ++errnr);
            bit_set(task, S_BAT_REVERT_CONN);
        } else {
            bit_clr(task, S_BAT_REVERT_CONN);
        }
        if ( me->yx_bat_I_high ) {
            len += sprintf(&errstr[len], "[%d: 电池过流] ", ++errnr);
            bit_set(task, S_BAT_I_HI);
        } else {
            bit_clr(task, S_BAT_I_HI);
        }

        if ( me->yx_bat_institude_fault ) {
            len += sprintf(&errstr[len], "[%d: 电池绝缘接地] ", ++errnr);
            bit_set(task, S_INSTITUDE_ERR);
        } else {
            bit_clr(task, S_INSTITUDE_ERR);
        }
        if ( me->yx_assit_power_stat ) {
            len += sprintf(&errstr[len], "[%d: 辅助电源故障] ", ++errnr);
            bit_set(task, S_ASSIT_POWER_DOWN);
        } else {
            bit_clr(task, S_ASSIT_POWER_DOWN);
        }
        if ( me->yx_temprature == 1 ) {
            len += sprintf(&errstr[len], "[%d: 温度过高] ", ++errnr);
            bit_set(task, S_CHARGE_BOX_TEMP_HI);
        } else if ( me->yx_temprature == 2 ) {
            len += sprintf(&errstr[len], "[%d: 温度过低] ", ++errnr);
            bit_set(task, S_CHARGE_BOX_TEMP_LO);
        } else {
            bit_clr(task, S_CHARGE_BOX_TEMP_HI);
            bit_clr(task, S_CHARGE_BOX_TEMP_LO);
        }
        if ( me->yx_wet_rate == 1 ) {
            len = sprintf(&errstr[len], "[%d: 湿度过高] ", ++errnr);
            bit_set(task, S_CHARGE_BOX_WET_HI);
        } else if ( me->yx_wet_rate == 2 ) {
            len += sprintf(&errstr[len], "[%d: 湿度过低] ", ++errnr);
            bit_set(task, S_CHARGE_BOX_WET_LO);
        } else {
            bit_clr(task, S_CHARGE_BOX_WET_HI);
            bit_clr(task, S_CHARGE_BOX_WET_LO);
        }

        if ( me->yx_rdq ) {
            len += sprintf(&errstr[len], "[%d: 总输出熔断器熔断] ", ++errnr);
            bit_set(task, S_DC_RDQ_BREAK);
        } else {
            bit_clr(task, S_DC_RDQ_BREAK);
        }
        if ( me->yx_dc_output_tiaozha ) {
            len += sprintf(&errstr[len], "[%d: 总输出跳闸] ", ++errnr);
            bit_set(task, S_DC_SW_BREAK);
        } else {
            bit_clr(task, S_DC_SW_BREAK);
        }
        if ( me->yx_dc_output_tiaozha1 ) {
            len += sprintf(&errstr[len], "[%d: 一路输出跳闸] ", ++errnr);
            bit_set(task, S_GUN_1_SW_BREAK);
        } else {
            bit_clr(task, S_GUN_1_SW_BREAK);
        }
        if ( me->yx_dc_output_tiaozha2 ) {
            len += sprintf(&errstr[len], "[%d: 二路输出跳闸] ", ++errnr);
            bit_set(task, S_GUN_2_SW_BREAK);
        } else {
            bit_clr(task, S_GUN_2_SW_BREAK);
        }
        if ( me->yx_flq ) {
            len += sprintf(&errstr[len], "[%d: 防雷器故障] ", ++errnr);
            bit_set(task, S_FANGLEIQI_BREAK);
        } else {
            bit_clr(task, S_FANGLEIQI_BREAK);
        }

        if ( errnr ) {
            log_printf(ERR, "Fault: "RED("%s"), errstr);
        }
        // 输入状态，遥信
        len = 0;
        if ( me->yx_ac_hezha ) {
            if ( ! me_pre->yx_ac_hezha ) {
                log_printf(INF, "采样盒: 交流开关合闸.");
            }
            bit_set(task, S_AC_INPUT_DOWN);
            len += sprintf(&infstr[len], "[交流"GRN("合闸")"] ");
        } else {
            if ( me_pre->yx_ac_hezha ) {
                log_printf(INF, "采样盒: 交流开关分闸.");
            }
            bit_clr(task, S_AC_INPUT_DOWN);
            len += sprintf(&infstr[len], "[交流"RED("分闸")"] ");
        }
        if ( me->yx_heater_stat ) {
            if ( ! me_pre->yx_heater_stat ) {
                log_printf(INF, "采样盒: 开始加热.");
            }
            len += sprintf(&infstr[len], "[加热] ");
        } else {
            if ( me_pre->yx_heater_stat ) {
                log_printf(INF, "采样盒: 停止加热.");
            }
            len += sprintf(&infstr[len], "[未加热] ");
        }
        if ( me->yx_fan_stat ) {
            if ( ! me_pre->yx_fan_stat ) {
                log_printf(INF, "采样盒: 开始通风.");
            }
            len += sprintf(&infstr[len], "[通风] ");
        } else {
            if ( me_pre->yx_fan_stat ) {
                log_printf(INF, "采样盒: 停止通风.");
            }
            len += sprintf(&infstr[len], "[未通风] ");
        }
        if ( me->yx_dc_output_hz ) {
            if ( ! me_pre->yx_dc_output_hz ) {
                log_printf(INF, "采样盒: 总输出"GRN("合闸"));
            }
            bit_set(task, F_DC_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[总输出"GRN("合闸")"] ");
        } else {
            if ( me_pre->yx_dc_output_hz ) {
                log_printf(INF, "采样盒: 总输出"RED("分闸"));
            }
            bit_clr(task, F_DC_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[总输出"RED("分闸")"] ");
        }
        if ( me->yx_gun_1_hezha_stat ) {
            if ( ! me_pre->yx_gun_1_hezha_stat ) {
                log_printf(INF, "采样盒: 1#枪输出"GRN("合闸")".");
            }
            bit_set(task, F_GUN_1_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[1#枪输出"GRN("合闸")"] ");
        } else {
            if ( me_pre->yx_gun_1_hezha_stat ) {
                log_printf(INF, "采样盒: 1#枪输出"RED("分闸"));
            }
            bit_clr(task, F_GUN_1_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[1#枪输出"RED("分闸")"] ");
        }
        if ( me->yx_gun_1_conn_stat == 0 ) {
            if ( me_pre->yx_gun_1_conn_stat != 0 ) {
                log_printf(INF, "采样盒: 1#枪断开连接");
                need_echo ++;
            }
            bit_clr(task, F_GUN_1_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[1#枪未链接] ");
        } else if (me->yx_gun_1_conn_stat == GUN_CONN_PROTECTED ) {
            bit_clr(task, F_GUN_1_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[1#枪链接保护] ");
        } else if ( me->yx_gun_1_conn_stat == GUN_CONN_EXCEPTION ) {
            bit_clr(task, F_GUN_1_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[1#枪连接异常] ");
        } else if ( me->yx_gun_1_conn_stat == GUN_CONN_CONNECTIVE ) {
            if ( me_pre->yx_gun_1_conn_stat != GUN_CONN_CONNECTIVE ) {
                log_printf(INF, "采样盒: 1#枪连接完成.");
                need_echo ++;
            }
            bit_set(task, F_GUN_1_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[1#枪链接"GRN("正常")"] ");
        }
        if ( me->yx_gun_1_assit_power_hezha ) {
            if ( !me_pre->yx_gun_1_assit_power_hezha ) {
                log_printf(INF, "采样盒: 1#枪辅助电源合闸.");
            }
            bit_set(task, F_GUN_1_ASSIT_PWN_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[1#枪辅助电源"GRN("合闸")"] ");
        } else {
            if ( me_pre->yx_gun_1_assit_power_hezha ) {
                log_printf(INF, "采样盒: 1#枪辅助电源分闸.");
            }
            bit_clr(task, F_GUN_1_ASSIT_PWN_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[1#枪辅助电源"RED("分闸")"] ");
        }
        if ( me->yx_gun_2_hezha_stat ) {
            if ( !me_pre->yx_gun_2_hezha_stat ) {
                log_printf(INF, "采样盒: 2#枪输出"GRN("合闸")"");
            }
            bit_set(task, F_GUN_2_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[2#枪输出"GRN("合闸")"] ");
        } else {
            if ( me_pre->yx_gun_2_hezha_stat ) {
                log_printf(INF, "采样盒: 2#枪输出"RED("分闸")"");
            }
            bit_clr(task, F_GUN_2_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[2#枪输出"RED("分闸")"] ");
        }
        if ( me->yx_gun_2_conn_stat == 0 ) {
            if ( me_pre->yx_gun_2_conn_stat != 0 ) {
                log_printf(INF, "采样盒: 2#枪断开连接");
                need_echo ++;
            }
            bit_clr(task, F_GUN_2_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[2#枪未链接] ");
        } else if (me->yx_gun_2_conn_stat == GUN_CONN_PROTECTED ) {
            bit_clr(task, F_GUN_2_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[2#枪链接保护] ");
        } else if ( me->yx_gun_2_conn_stat == GUN_CONN_EXCEPTION ) {
            bit_clr(task, F_GUN_2_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[2#枪连接异常] ");
        } else if ( me->yx_gun_2_conn_stat == GUN_CONN_CONNECTIVE ) {
            if ( me_pre->yx_gun_2_conn_stat != GUN_CONN_CONNECTIVE ) {
                log_printf(INF, "采样盒: 2#枪连接完成.");
                need_echo ++;
            }
            bit_set(task, F_GUN_2_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[2#枪链接"GRN("正常")"] ");
        }
        if ( me->yx_gun_2_assit_power_hezha ) {
            if ( !me_pre->yx_gun_2_assit_power_hezha ) {
                log_printf(INF, "采样盒: 2#枪辅助电源"GRN("合闸"));
            }
            bit_set(task, F_GUN_2_ASSIT_PWN_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[2#枪辅助电源"GRN("合闸")"] ");
        } else {
            if ( me_pre->yx_gun_2_assit_power_hezha ) {
                log_printf(INF, "采样盒: 2#枪辅助电源"RED("分闸"));
            }
            bit_clr(task, F_GUN_2_ASSIT_PWN_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[2#枪辅助电源"RED("分闸")"] ");
        }
        log_printf(DBG_LV3, "采样盒: 遥信: %s", infstr);
        memcpy(me_pre, me, sizeof(struct MDATA_ACK));
        if ( need_echo ) {
            ret = ERR_NEED_ECHO;
        } else {
            ret = ERR_OK;
        }
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        param->attrib = BP_FRAME_UNSTABLE;

        if ( task->measure.yx_gun_1_conn_stat == GUN_CONN_CONNECTIVE ) {
            cmd |= GUN1_ASSIT_PWN_ON;
        } else {
            cmd &= ~GUN1_ASSIT_PWN_ON;
        }
        if ( task->measure.yx_gun_2_conn_stat == GUN_CONN_CONNECTIVE ) {
            cmd |= GUN2_ASSIT_PWN_ON;
        } else {
            cmd &= ~GUN2_ASSIT_PWN_ON;
        }

       // if ( bit_read(task, ))

        buff[ nr ++ ] = 0xF0;
        buff[ nr ++ ] = 0xE1;
        buff[ nr ++ ] = 0xD2;
        buff[ nr ++ ] = 0xC3;
        buff[ nr ++ ] = 0xB4;
        buff[ nr ++ ] = 0x05;
        buff[ nr ++ ] = 16;
        buff[ nr ++ ] = cmd;
        buff[ nr ++ ] = cmd;
        nr += 14;
        len = nr;
        buff[ nr ++ ] = load_crc(len, buff);
        buff[ nr ++ ] = load_crc(len, buff) >> 8;

        memcpy(param->buff.tx_buff, buff, nr);
        param->payload_size = nr;

        self->rx_param.need_bytes = 32;
        self->master->time_to_send = param->payload_size * 1000 / 960;
        ret = ERR_OK;
        log_printf(DBG_LV3, "UART: %s sent.", __FUNCTION__);
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        ret = ERR_OK;
        break;
    // 串口数据发送完成事件
    case BP_EVT_TX_FRAME_DONE:
        log_printf(DBG_LV3, "UART: %s packet send done", __FUNCTION__);
        break;
    // 串口接收单个字节超时，出现在接收帧的第一个字节
    case BP_EVT_RX_BYTE_TIMEOUT:
    // 串口接收帧超时, 接受的数据不完整
    case BP_EVT_RX_FRAME_TIMEOUT:
        if ( self->master->died < self->master->died_line ) {
            //self->master->died ++;
        } else {
            //self->master->died ++;
            if ( ! bit_read(task, S_MEASURE_COMM_DOWN) ) {
            }
            log_printf(ERR, "UART: "RED("综合采样盒通信中断, 请排查故障,"
                                        " 已禁止充电(%d)."), self->master->died);
            bit_set(task, S_MEASURE_COMM_DOWN);
        }
        log_printf(WRN, "UART: %s get signal TIMEOUT", __FUNCTION__);
        break;
    // 串口IO错误
    case BP_EVT_IO_ERROR:
        break;
    // 帧校验失败
    case BP_EVT_FRAME_CHECK_ERROR:
        break;
    default:
        log_printf(WRN, "UART: unreliable EVENT %08Xh", evt);
        break;
    }
    return ret;
}

static int uart5_background_evt_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_ERR;
    switch (evt) {
    case BP_EVT_FRAME_CHECK:
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
    // 串口接收帧超时, 接受的数据不完整
    case BP_EVT_RX_FRAME_TIMEOUT:
        //self->master->died ++;
        log_printf(WRN, "UART: %s get signal TIMEOUT", __FUNCTION__);
        break;
    // 串口IO错误
    case BP_EVT_IO_ERROR:
        break;
    // 帧校验失败
    case BP_EVT_FRAME_CHECK_ERROR:
        break;
    default:
        log_printf(WRN, "UART: unreliable EVENT %08Xh", evt);
        break;
    }
    return ret;
}

void *thread_uart_service(void *arg) ___THREAD_ENTRY___
{
    int *done = (int *)arg;
    int mydone = 0, ret;
    struct bp_uart *thiz = &uarts[0];
    int retval, max_handle = 0;
    size_t cursor;

    if ( done == NULL ) done = &mydone;

    memset(uarts, 0, sizeof(uarts));

    uarts[0].bp_evt_handle = uart4_bp_evt_handle;
    uarts[0].dev_handle = -1;
    uarts[0].dev_name = "/dev/ttyO4";

    uarts[1].bp_evt_handle = NULL;
    uarts[1].dev_handle = -1;
    uarts[1].dev_name = "/dev/ttyO5";

    if ( thiz ) {
        // 出错误后尝试的次数
        thiz->init_magic = 5;
    }

#if (CONFIG_SUPPORT_SIGIO > 0)
    if ( SIG_ERR == signal(SIGIO, uarts_async_sigio) ) {
        log_printf(ERR, "UART: signal(SIGIO, uarts_async_sigio) failed!!!"
                   "errno: %d", errno);
    }
#endif

    while ( ! *done ) {
        usleep(1000);
        if ( thiz == NULL ) continue;
        if ( thiz->bp_evt_handle == NULL ) continue;
        if ( thiz->status == BP_UART_STAT_ALIENT ) continue;

        //signal(SIGIO, uarts_async_sigio);

        if ( thiz->status == BP_UART_STAT_INVALID ) {
            if ( thiz->init_magic <= 0 ) {
                thiz->status = BP_UART_STAT_ALIENT;
                log_printf(ERR, "UART: open UART faile, thread panic.....");
                continue;
            }

            // 初始化数据结构, 设定串口的初始状态
            // 串口的初始状态决定了串口的工作模式
            thiz->bp_evt_handle(thiz, BP_EVT_INIT, NULL);

            // 打开并配置串口
            // 如果初始化失败，则会不断的尝试
            ret = thiz->bp_evt_handle(thiz, BP_EVT_CONFIGURE, NULL);
            if ( ret == (int)ERR_UART_OPEN_FAILE ) {
                thiz->status = BP_UART_STAT_INVALID;
                log_printf(ERR, "UART: try open %s faile.", thiz->dev_name);
                thiz->init_magic --;
                continue;
            }
            if ( ret == (int)ERR_UART_CONFIG_FAILE ) {
                thiz->status = BP_UART_STAT_INVALID;
                // 首先关闭串口，然后才能进行下一次尝试
                thiz->bp_evt_handle(thiz, BP_EVT_KILLED, NULL);
                log_printf(ERR, "UART: configure %s faile.", thiz->dev_name);
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
                    ret = thiz->bp_evt_handle(thiz, BP_EVT_SWITCH_2_RX, NULL);
                } while ( ret != ERR_OK && trynr ++ < 100 );
            }
            if ( thiz->status == BP_UART_STAT_WR ) {
                int trynr = 0;
                do {
                    usleep(50000);
                    ret = thiz->bp_evt_handle(thiz, BP_EVT_SWITCH_2_TX, NULL);
                } while ( ret != ERR_OK && trynr ++ < 100  );
            }
            if ( ret != ERR_OK ) {
                thiz->bp_evt_handle(thiz, BP_EVT_KILLED, NULL);
                thiz->status = BP_UART_STAT_ALIENT;
                log_printf(ERR, "UART: switch to defaute mode faile, thread panic..");
                continue;
            }

            if ( thiz->dev_handle > max_handle ) {
                max_handle = thiz->dev_handle;
            }

            log_printf(INF, "UART: open UART %d:%s correct.",
                       thiz->dev_handle, thiz->dev_name);
            continue;
        }

        /*
         * 串口在这里扮演的角色必定是主动设备或者从动设备
         * 因此读取数据是否需要进行计时操作完全取决于扮演的角色类型。
         *
         * 如果是主动设备，那么必须要设定超时计时器，以确定接收数据是否超时
         * 如果是从动设备，设定超时计时器并非是必须的，而是根据是否需要接收
         * 连续的串口数据流来判定，若不需要连续的数据流，那么就不必要设定
         * 超时计时器，反之需要设定超时计时器来确定能否在指定时间内收到完整
         * 的数据帧。
         */
        if ( thiz->status == BP_UART_STAT_RD ) {
            char *buff = thiz->rx_param.buff.rx_buff;
            int rd = 0;
            static int nr = 0;

            if ( thiz->hw_status != BP_UART_STAT_RD ) {
                errno = 0;
                thiz->bp_evt_handle(thiz, BP_EVT_SWITCH_2_RX, NULL);
                thiz->hw_status = BP_UART_STAT_RD;
                thiz->rx_param.cursor = 0;
                thiz->rx_param.payload_size = 0;
                nr = 0;
                memset(thiz->rx_buff, 0, sizeof(thiz->rx_buff));
                tcflush(thiz->dev_handle, TCIFLUSH);
                log_printf(DBG_LV0, "UART: switch to RX mode.");
            }

            do {
                errno = 0;
                cursor = thiz->rx_param.cursor;
                rd = read(thiz->dev_handle,
                          &thiz->rx_param.buff.rx_buff[cursor], 32);
                if ( rd > 0 ) {
                    Hachiko_feed(&thiz->rx_seed);
                    thiz->rx_param.payload_size += rd;
                    thiz->rx_param.cursor = thiz->rx_param.payload_size;
                    nr += rd;
                    log_printf(DBG_LV1,
                               "UART: RD:%d:%d:%d <"
                               "%02X %02X %02X %02X %02X %02X %02X %02X "
                               "%02X %02X %02X %02X %02X %02X %02X %02X "
                               "%02X %02X %02X %02X>",
                               rd, nr, cursor,
                               buff[0 + cursor], buff[1 + cursor], buff[2 + cursor], buff[3 + cursor],
                               buff[4 + cursor], buff[5 + cursor], buff[6 + cursor], buff[7 + cursor],
                               buff[0+8 + cursor], buff[1+8 + cursor], buff[2+8 + cursor], buff[3+8 + cursor],
                               buff[4+8 + cursor], buff[5+8 + cursor], buff[6+8 + cursor], buff[7+8 + cursor],
                               buff[8+8 + cursor], buff[9+8 + cursor], buff[10+8 + cursor], buff[11+8 + cursor]);
                }
                ret = thiz->bp_evt_handle(thiz, BP_EVT_FRAME_CHECK,
                                          &thiz->rx_param);
                switch ( ret ) {
                // 数据接收，校验完成, 完成数据接收过程，停止接收
                case ERR_OK:
                    thiz->status = BP_UART_STAT_WR;
                    Hachiko_pause(&thiz->rx_seed);
                    log_printf(DBG_LV0, "UART: fetched a "GRN("new")" frame.");
                    ret = thiz->bp_evt_handle(thiz, BP_EVT_RX_FRAME,
                                                          &thiz->rx_param);
                    if ( (unsigned)ret == ERR_NEED_ECHO ) {
                        thiz->continues_nr ++;
                        if ( thiz->continues_nr < 5 ) {
                            thiz->sequce --;
                        } else {
                            thiz->continues_nr = 0;
                        }
                        log_printf(INF, "UART: 需要立即发送回应帧: %d-%d",
                                   thiz->continues_nr,
                                   thiz->sequce);
                    } else {
                        thiz->continues_nr = 0;
                    }
                    //thiz->master->rcv_ok_cnt ++;
                    break;
                // 数据接收完成，但校验失败, 停止接收
                case ERR_FRAME_CHECK_ERR:
                    thiz->bp_evt_handle(thiz, BP_EVT_FRAME_CHECK_ERROR,
                                                              &thiz->rx_param);
                    //thiz->master->check_err_cnt ++;
                    thiz->status = BP_UART_STAT_WR;
                    Hachiko_pause(&thiz->rx_seed);
                    log_printf(DBG_LV2,
                               "UART: lenth fetched but check "RED("faile."));
                    break;
                // 数据接收长度不足，继续接收
                case ERR_FRAME_CHECK_DATA_TOO_SHORT:
                    if ( rd > 0 ) {
                        thiz->bp_evt_handle(thiz, BP_EVT_RX_DATA, &thiz->rx_param);
                    }
                    break;
                default:
                    break;
                }
#if 0
                if ( thiz->rx_param.payload_size >= thiz->rx_param.need_bytes
                     /*(size_t)(thiz->rx_param.buff.rx_buff[1] + 4)*/ ) {
                    thiz->status = BP_UART_STAT_WR;
                    Hachiko_pause(&thiz->rx_seed);
                    log_printf(DBG_LV1, "UART: recv done.need: %d, fetched: %d",
                               thiz->rx_param.need_bytes,
                            thiz->rx_param.payload_size);
                }
#endif
            } while ( thiz->status == BP_UART_STAT_RD &&
                      (unsigned)ret == ERR_FRAME_CHECK_DATA_TOO_SHORT &&
                      thiz->rx_seed.remain );
            continue;
        }

        // 程序默认采用9600 的波特率， 大致估算出每发送一个字节耗时1.04ms
        // 抛去程序运行时的延迟，发送延迟，可估计每发送一个字节耗时1.1 ms
        if ( thiz->status == BP_UART_STAT_WR ) {

            if ( thiz->hw_status != BP_UART_STAT_WR ) {
                thiz->bp_evt_handle(thiz, BP_EVT_SWITCH_2_TX, NULL);
                tcflush(thiz->dev_handle, TCOFLUSH);
                tcflush(thiz->dev_handle, TCIFLUSH);
                thiz->hw_status = BP_UART_STAT_WR;
                memset(thiz->tx_buff, 0, sizeof(thiz->tx_buff));
                log_printf(DBG_LV0, "UART: switch to TX mode.");
                continue;
            }

            if ( thiz->tx_param.cursor < thiz->tx_param.payload_size &&
                 thiz->tx_param.payload_size > 0 ) {
                // 前一次没有发送完成， 继续发送
                //log_printf(DBG_LV0, "UART: goto continue_to_send");
                goto continue_to_send;
            }
            if ( thiz->tx_param.payload_size ) {
                //log_printf(DBG_LV0, "UART: continue becouse: thiz->tx_param.payload_size > 0");
                continue;
            }

            thiz->tx_param.buff.tx_buff = thiz->tx_buff;
            thiz->tx_param.buff_size = sizeof(thiz->tx_buff);
            thiz->tx_param.payload_size = 0;
            thiz->tx_param.cursor = 0;

            // 减缓节奏
            usleep(600 * 1000);
            ret = thiz->bp_evt_handle(thiz, BP_EVT_TX_FRAME_REQUEST,
                                      &thiz->tx_param);
            if ( ret != ERR_OK || thiz->tx_param.payload_size <= 0 ||
                 thiz->tx_param.payload_size > 255 ) {
                thiz->tx_param.buff.tx_buff = thiz->tx_buff;
                thiz->tx_param.buff_size = sizeof(thiz->tx_buff);
                thiz->tx_param.payload_size = 0;
                thiz->tx_param.cursor = 0;
                continue;
            }

            ret = thiz->bp_evt_handle(thiz, BP_EVT_TX_FRAME_CONFIRM,
                                      &thiz->tx_param);
            if ( ret != ERR_OK ) {
                thiz->tx_param.buff.tx_buff = thiz->tx_buff;
                thiz->tx_param.buff_size = sizeof(thiz->tx_buff);
                thiz->tx_param.payload_size = 0;
                thiz->tx_param.cursor = 0;
                continue;
            }

            if ( thiz->tx_param.cursor > thiz->tx_param.payload_size ) {
                thiz->tx_param.buff.tx_buff = thiz->tx_buff;
                thiz->tx_param.buff_size = sizeof(thiz->tx_buff);
                thiz->tx_param.payload_size = 0;
                thiz->tx_param.cursor = 0;
                continue;
            }

continue_to_send:
            cursor = thiz->tx_param.cursor;
            retval = write(thiz->dev_handle,
                           & thiz->tx_param.buff.tx_buff[cursor],
                           thiz->tx_param.payload_size - cursor);
            if ( retval <= 0 ) {
                log_printf(ERR, "UART: send error, TX REQUEST AUTOMATIC ABORTED.");
                thiz->tx_param.buff.tx_buff = thiz->tx_buff;
                thiz->tx_param.buff_size = sizeof(thiz->tx_buff);
                thiz->tx_param.payload_size = 0;
                thiz->tx_param.cursor = 0;
                continue;
            }

            if ( retval == (int)(thiz->tx_param.payload_size - cursor) ) {
                // 发送完成，但仅仅是数据写入到发送缓冲区，此时数据没有完全通过传输介质
                // 此时启动发送计时器，用来确定数据发送完成事件
                thiz->tx_param.cursor = thiz->tx_param.payload_size;
#if 0
                thiz->tx_seed.ttl = thiz->tx_param.payload_size +
                        (thiz->tx_param.payload_size % 10 ? 2 : 1);
#else
                thiz->tx_seed.ttl = thiz->master->time_to_send;
#endif

#if (CONFIG_SUPPORT_ASYNC_UART_TX == 1)
                Hachiko_resume( & thiz->tx_seed );
                // 睡眠20us 引起内核线程切换, 快速切换至Hachiko线程
                usleep(20);
                log_printf(DBG_LV0, "UART: send data len: %d, TX ttl: %d unit",
                           thiz->tx_param.payload_size,
                           thiz->tx_seed.ttl);
#else
                usleep(1000 * thiz->master->time_to_send);
                thiz->bp_evt_handle(thiz, BP_EVT_TX_FRAME_DONE, &thiz->tx_param);
                thiz->tx_param.payload_size = 0;
                log_printf(DBG_LV0, "UART: packet send done.");
                memset(thiz->rx_param.buff.rx_buff, 0, thiz->rx_param.buff_size);
                thiz->status = BP_UART_STAT_RD;
                if ( thiz->role == BP_UART_MASTER ) {
                    // 主动设备，需要进行接收超时判定
                    Hachiko_resume(&thiz->rx_seed);
                }
#endif
            } else if ( retval < (int)(thiz->tx_param.payload_size - cursor) ) {
                // 发送了一部分
                thiz->tx_param.cursor = retval;
            } else {
                // Unexpected. Exception
                log_printf(ERR, "UART: send error, TX AUTO ABORTED.");
                thiz->tx_param.buff.tx_buff = thiz->tx_buff;
                thiz->tx_param.buff_size = sizeof(thiz->tx_buff);
                thiz->tx_param.payload_size = 0;
                thiz->tx_param.cursor = 0;
                continue;
            }
            continue;
        }
    }
    return NULL;
}

// 生成串口通信统计页面
int ajax_uart_debug_page(struct ajax_xml_struct *thiz)
{
    int output_len = 0, i;
    struct bp_user *me = &down_user[0];
    struct MDATA_ACK *self;
    char errstr[1024] = {0}, infstr[1024] = {0};
    int errnr = 0, len = 0;

    thiz->ct = "application/json";

    output_len += sprintf(&thiz->iobuff[output_len], "{\"uartusers\":[");
    for (; me->user_evt_handle; me ++ ) {
        if (me->user_evt_handle == uart4_charger_module_evt_handle ) {
            output_len += sprintf(&thiz->iobuff[output_len], "{\"obj\":\"模块操作\",");
         } else if (me->user_evt_handle == uart4_charger_config_evt_handle) {
            output_len += sprintf(&thiz->iobuff[output_len], "{\"obj\":\"监控配置\",");
         } else if (me->user_evt_handle == uart4_charger_date_evt_handle) {
            output_len += sprintf(&thiz->iobuff[output_len], "{\"obj\":\"日期配置\",");
         } else if (me->user_evt_handle == uart4_charger_yaoce_0_49_handle) {
            output_len += sprintf(&thiz->iobuff[output_len], "{\"obj\":\"监控遥信0\",");
        } else if ( me->user_evt_handle == uart4_charger_yaoce_50_100_handle) {
            output_len += sprintf(&thiz->iobuff[output_len], "{\"obj\":\"监控遥信1\",");
        } else if ( me->user_evt_handle == uart4_simple_box_evt_handle) {
            output_len += sprintf(&thiz->iobuff[output_len], "{\"obj\":\"综合采样\",");
         } else {
            log_printf(ERR, "AJAX: bad request abort UART debug page. %p:(%p:%p:%p:%p)",
                       me->user_evt_handle,
                       uart4_charger_module_evt_handle,
                       uart4_charger_config_evt_handle,
                       uart4_charger_date_evt_handle,
                       uart4_charger_yaoce_0_49_handle);
            return ERR_ERR;
         }
        output_len += sprintf(&thiz->iobuff[output_len], "\"freq\":%d,", me->frame_freq);
        output_len += sprintf(&thiz->iobuff[output_len], "\"seed\":%d,", me->seed);
        output_len += sprintf(&thiz->iobuff[output_len], "\"died_line\":%d,", me->died_line);
        output_len += sprintf(&thiz->iobuff[output_len], "\"died\":%d,", me->died);
        output_len += sprintf(&thiz->iobuff[output_len], "\"died_total\":%d,", me->died_total);
        output_len += sprintf(&thiz->iobuff[output_len], "\"sent\":%d,", me->sent_frames);
        output_len += sprintf(&thiz->iobuff[output_len], "\"checkerr\":%d,", me->check_err_cnt);
        output_len += sprintf(&thiz->iobuff[output_len], "\"check_err_total\":%d,", me->check_err_total);
        output_len += sprintf(&thiz->iobuff[output_len], "\"recv_cnt\":%d}", me->rcv_ok_cnt);
        if ( (me + 1)->user_evt_handle ) {
            output_len += sprintf(&thiz->iobuff[output_len], ",");
        } else {
            output_len += sprintf(&thiz->iobuff[output_len], "]");
        }
    }

    // 充电机遥信，遥测量
    output_len += sprintf(&thiz->iobuff[output_len], ", \"chargers\":{");
    output_len += sprintf(&thiz->iobuff[output_len], "\"charger_sn\":\"%04X\",", task->chargers.charger_sn);
    output_len += sprintf(&thiz->iobuff[output_len], "\"charger_status\":\"%04X\",", task->chargers.charger_status);
    output_len += sprintf(&thiz->iobuff[output_len], "\"charger_self_status\":\"%04X\",", task->chargers.charger_self_status);
    output_len += sprintf(&thiz->iobuff[output_len], "\"charger_max_v_out\":%d,", b2l(task->chargers.charger_max_v_out));
    output_len += sprintf(&thiz->iobuff[output_len], "\"charger_min_v_out\":%d,", b2l(task->chargers.charger_min_v_out));
    output_len += sprintf(&thiz->iobuff[output_len], "\"charger_max_i_out\":%d,", b2l(task->chargers.charger_max_i_out));
    output_len += sprintf(&thiz->iobuff[output_len], "\"charger_v_out\":%d,", b2l(task->chargers.charger_v_out));
    output_len += sprintf(&thiz->iobuff[output_len], "\"charger_i_out\":%d,", b2l(task->chargers.charger_i_out));
    output_len += sprintf(&thiz->iobuff[output_len], "\"modules\":[");
    for (i=0; i < CONFIG_SUPPORT_CHARGE_MODULE; i ++ ) {
        output_len += sprintf(&thiz->iobuff[output_len], "{\"voltage\":%d,", b2l(task->chargers.charge_module_v[i]));
        output_len += sprintf(&thiz->iobuff[output_len], "\"current\":%d,", b2l(task->chargers.charge_module_i[i]));
        output_len += sprintf(&thiz->iobuff[output_len], "\"temp\":%d,", b2l(task->chargers.charge_module_t[i]));
        output_len += sprintf(&thiz->iobuff[output_len], "\"sn\":\"%04X%04X%04X\"}",
                              b2l(task->chargers.charge_module_sn[i][0]),
                              b2l(task->chargers.charge_module_sn[i][1]),
                              b2l(task->chargers.charge_module_sn[i][2]));
        if ( i != CONFIG_SUPPORT_CHARGE_MODULE - 1 ) {
            output_len += sprintf(&thiz->iobuff[output_len], ",");
        }
    }
    output_len += sprintf(&thiz->iobuff[output_len], "]");
    output_len += sprintf(&thiz->iobuff[output_len], "}");

    // 扩展采样盒
    self = &task->measure;
    len = 0;
    i = 0;
    if ( self->yx_mx_V_high ) {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len += sprintf(&errstr[len], "[%d: 母线过压] \"},", ++errnr);
    }
    if ( self->yx_mx_V_low ) {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len += sprintf(&errstr[len], "[%d: 母线欠压] \"},", ++errnr);
    }
    if ( self->yx_mx_short_fault ) {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len += sprintf(&errstr[len], "[%d: 母线短路] \"},", ++errnr);
    }

    if ( self->yx_bat_V_high ) {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len += sprintf(&errstr[len], "[%d: 电池过压] \"},", ++errnr);
    }
    if ( self->yx_bat_V_low ) {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len += sprintf(&errstr[len], "[%d: 电池欠压] \"},", ++errnr);
    }
    if ( self->yx_bat_short_fault ) {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len += sprintf(&errstr[len], "[%d: 电池链接短路] \"},", ++errnr);
    }
    if ( self->yx_bat_revers_conn ) {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len += sprintf(&errstr[len], "[%d: 电池反接] \"},", ++errnr);
    }
    if ( self->yx_bat_I_high ) {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len += sprintf(&errstr[len], "[%d: 电池过流] \"},", ++errnr);
    }

    if ( self->yx_bat_institude_fault ) {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len += sprintf(&errstr[len], "[%d: 电池绝缘接地] \"},", ++errnr);
    }
    if ( self->yx_assit_power_stat ) {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len += sprintf(&errstr[len], "[%d: 辅助电源故障] \"},", ++errnr);
    }
    if ( self->yx_temprature == 1 ) {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len += sprintf(&errstr[len], "[%d: 温度过高] \"},", ++errnr);
    } else if ( self->yx_temprature == 2 ) {
        len += sprintf(&errstr[len], "[%d: 温度过低] \"},", ++errnr);
    }
    if ( self->yx_wet_rate == 1 ) {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len = sprintf(&errstr[len], "[%d: 湿度过高] \"},", ++errnr);
    } else if ( self->yx_wet_rate == 2 ) {
        len += sprintf(&errstr[len], "[%d: 湿度过低] \"},", ++errnr);
    }

    if ( self->yx_rdq ) {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len += sprintf(&errstr[len], "[%d: 总输出熔断器熔断] \"},", ++errnr);
    }
    if ( self->yx_dc_output_tiaozha ) {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len += sprintf(&errstr[len], "[%d: 总输出跳闸] \"},", ++errnr);
    }
    if ( self->yx_dc_output_tiaozha1 ) {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len += sprintf(&errstr[len], "[%d: 一路输出跳闸] \"},", ++errnr);
    }
    if ( self->yx_dc_output_tiaozha2 ) {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len += sprintf(&errstr[len], "[%d: 二路输出跳闸] \"},", ++errnr);
    }
    if ( self->yx_flq ) {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len += sprintf(&errstr[len], "[%d: 防雷器故障] \"},", ++errnr);
    }
    if ( i > 0 ) {
        errstr[len --] = '\0';
    } else {
        len += sprintf(&errstr[len], "{\"no\":%d,\"error\":\"", ++i);
        len += sprintf(&errstr[len], "[%d: 无故障] \"}", ++errnr);
    }
    if ( i ) {
        output_len += sprintf(&thiz->iobuff[output_len], ",\"errors\":[%s]", errstr);
    }
    // 输入状态，遥信
    len = 0;
    i = 0;
    len += sprintf(&infstr[len], ",\"yaoxin\":[{\"no\":%d,\"stat\":\"", ++i);
    if ( self->yx_ac_hezha ) {
        len += sprintf(&infstr[len], "[交流合闸] \",\"color\":\"green\"},");
    } else {
        len += sprintf(&infstr[len], "[交流分闸] \",\"color\":\"yellow\"},");
    }
    len += sprintf(&infstr[len], "{\"no\":%d,\"stat\":\"", ++i);
    if ( self->yx_heater_stat ) {
        len += sprintf(&infstr[len], "[加热] ,\"color\":\"yellow\"},");
    } else {
        len += sprintf(&infstr[len], "[未加热] \",\"color\":\"green\"},");
    }
    len += sprintf(&infstr[len], "{\"no\":%d,\"stat\":\"", ++i);
    if ( self->yx_fan_stat ) {
        len += sprintf(&infstr[len], "[通风] \",\"color\":\"yellow\"},");
    } else {
        len += sprintf(&infstr[len], "[未通风] \",\"color\":\"green\"},");
    }
    len += sprintf(&infstr[len], "{\"no\":%d,\"stat\":\"", ++i);
    if ( self->yx_dc_output_hz ) {
        len += sprintf(&infstr[len], "[总输出合闸] \",\"color\":\"green\"},");
    } else {
        len += sprintf(&infstr[len], "[总输出分闸] \",\"color\":\"yellow\"},");
    }
    len += sprintf(&infstr[len], "{\"no\":%d,\"stat\":\"", ++i);
    if ( self->yx_gun_1_hezha_stat ) {
        len += sprintf(&infstr[len], "[1#枪输出合闸] \",\"color\":\"green\"},");
    } else {
        len += sprintf(&infstr[len], "[1#枪输出分闸] \",\"color\":\"yellow\"},");
    }
    len += sprintf(&infstr[len], "{\"no\":%d,\"stat\":\"", ++i);
    if ( self->yx_gun_1_conn_stat == 0 ) {
        len += sprintf(&infstr[len], "[1#枪未链接] \",\"color\":\"yellow\"},");
    } else if (self->yx_gun_1_conn_stat == 1 ) {
        len += sprintf(&infstr[len], "[1#枪链接保护] \",\"color\":\"yellow\"},");
    } else if ( self->yx_gun_1_conn_stat == 2 ) {
        len += sprintf(&infstr[len], "[1#枪连接异常] \",\"color\":\"red\"},");
    } else if ( self->yx_gun_1_conn_stat == 3 ) {
        len += sprintf(&infstr[len], "[1#枪链接正常] \",\"color\":\"green\"},");
    }
    len += sprintf(&infstr[len], "{\"no\":%d,\"stat\":\"", ++i);
    if ( self->yx_gun_1_assit_power_hezha ) {
        len += sprintf(&infstr[len], "[1#枪辅助电源合闸] \",\"color\":\"green\"},");
    } else {
        len += sprintf(&infstr[len], "[1#枪辅助电源分闸] \",\"color\":\"yellow\"},");
    }
    len += sprintf(&infstr[len], "{\"no\":%d,\"stat\":\"", ++i);
    if ( self->yx_gun_2_hezha_stat ) {
        len += sprintf(&infstr[len], "[2#枪输出合闸] \",\"color\":\"green\"},");
    } else {
        len += sprintf(&infstr[len], "[2#枪输出分闸] \",\"color\":\"yellow\"},");
    }
    len += sprintf(&infstr[len], "{\"no\":%d,\"stat\":\"", ++i);
    if ( self->yx_gun_2_conn_stat == 0 ) {
        len += sprintf(&infstr[len], "[2#枪未链接] \",\"color\":\"yellow\"},");
    } else if (self->yx_gun_2_conn_stat == 1 ) {
        len += sprintf(&infstr[len], "[2#枪链接保护] \",\"color\":\"yellow\"},");
    } else if ( self->yx_gun_2_conn_stat == 2 ) {
        len += sprintf(&infstr[len], "[2#枪连接异常] \",\"color\":\"red\"},");
    } else if ( self->yx_gun_2_conn_stat == 3 ) {
        len += sprintf(&infstr[len], "[2#枪链接正常] \",\"color\":\"green\"},");
    }
    len += sprintf(&infstr[len], "{\"no\":%d,\"stat\":\"", ++i);
    if ( self->yx_gun_2_assit_power_hezha ) {
        len += sprintf(&infstr[len], "[2#枪辅助电源合闸] \",\"color\":\"green\"}");
    } else {
        len += sprintf(&infstr[len], "[2#枪辅助电源分闸] \",\"color\":\"yellow\"}");
    }
    len += sprintf(&infstr[len], "]");

    output_len += sprintf(&thiz->iobuff[output_len], "%s", infstr);

    // 终结符
    output_len += sprintf(&thiz->iobuff[output_len], "}");
    thiz->xml_len = output_len;
    return ERR_OK;
}
