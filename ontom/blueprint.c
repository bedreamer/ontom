#include "stdafx.h"
#include "blueprint.h"
#include "charge.h"

int configure_uart(int fd, int baud_rate, int databits, int stopbits, int parity)
{
    struct termios options;
    char dbg[256] = {0};
    int l = 0;

    bzero(&options, sizeof(options));
    if (tcgetattr(fd, &options) != 0) {
        perror("SetupSerial 1");
        return ERR_UART_CONFIG_FAILE;
    }
    cfsetispeed(&options, B19200);
    cfsetospeed(&options, B19200);

    switch (databits) {
        default:
        case 7:
            options.c_cflag |= CS7;
            l += sprintf(&dbg[l], "[数据位: 7 bits]");
            break;
        case 8:
            options.c_cflag |= CS8;
            l += sprintf(&dbg[l], "[数据位: 8 bits]");
            break;
    }
    switch (stopbits) {
        default:
        case 1:
            options.c_cflag &= ~CSTOPB;
            l += sprintf(&dbg[l], "[停止位: 1 bit]");
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            l += sprintf(&dbg[l], "[停止位: 2 bits]");
            break;
    }
    switch (parity) {
        case 's':
        case 'S':
            l += sprintf(&dbg[l], "[无校验]");
            options.c_cflag &= ~PARENB;   /* Clear parity enable */
            break;
        case 'o':
        case 'O':
        case 1:
            options.c_cflag |= (PARODD | PARENB);/* 设置为奇效验*/
            break;
        case 'e':
        case 'E':
        case 2:
            options.c_cflag |= PARENB;     /* Enable parity */
            options.c_cflag &= ~PARODD;     /* 转换为偶效验*/
            break;
        default:
        case 'n':
        case 'N':  /*as no parity*/
        case 0:
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
    }
    options.c_cflag &= ~CSIZE;

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;

    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN] = 0;
    tcflush(fd,TCIOFLUSH);
    if (tcsetattr(fd,TCSANOW,&options) != 0) {
        perror("SetupSerial 3");
        return ERR_UART_CONFIG_FAILE;
    }
    log_printf(INF, "%d:{%d, %s}", fd, baud_rate, dbg);
    return ERR_OK;
}
int set_speed(int fd, int speed)
{
    unsigned int   i;
    int   status;
    struct termios   Opt;
    int speed_arr[] = {B230400, B115200, B57600, B38400, B19200, \
                            B9600, B4800, B2400, B1800, B1200, B600, B300};
    int name_arr[]  = {230400,  115200,  57600,  38400,  19200,  \
                            9600,  4800,  2400,  1800,  1200,  600,  300};

    tcgetattr(fd, &Opt);

    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
    {
        if (speed == name_arr[i])
        {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &Opt);
            if  (status != 0)
            {
                perror("tcsetattr fd");
                return -1;
            }
            tcflush(fd,TCIOFLUSH);
            return 0;
        }
    }
    return -1;
}

int set_other_attribute(int fd, int speed, int databits, int stopbits, int parity)
{
    struct termios options;


    if (tcgetattr(fd, &options) != 0)
    {
        perror("SetupSerial 1");
        return -1;
    }
    bzero( &options, sizeof( options ) );

    switch(speed){  //设置数据传输率
    case 2400:
       cfsetispeed(&options,B2400);
       cfsetospeed(&options,B2400);
       break;
    case 4800:
       cfsetispeed(&options,B4800);
       cfsetospeed(&options,B4800);
       break;
    case 9600:
       cfsetispeed(&options,B9600);
       cfsetospeed(&options,B9600);
       break;
    case 115200:
       cfsetispeed(&options,B115200);
       cfsetospeed(&options,B115200);
       break;
    case 460800:
       cfsetispeed(&options,B460800);
       cfsetospeed(&options,B460800);
       break;
    default:
       cfsetispeed(&options,B9600);
       cfsetospeed(&options,B9600);
       break;
    }

    options.c_cflag  |=  CLOCAL | CREAD;
    options.c_cflag &= ~CSIZE;

    switch (databits)
    {
        case 7:
            options.c_cflag |= CS7;
            break;

        case 8:
            options.c_cflag |= CS8;
            break;

        default:
            fprintf(stderr,"Unsupported data size\n");
            return -1;
    }

    switch (parity)
    {
        case 'e':
        case 'E':
        case 1:
            //options.c_iflag |= (INPCK | ISTRIP);
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            break;

        case 'o':
        case 'O':
        case 2:
            options.c_cflag |= PARENB;
            options.c_cflag |= PARODD;
            //options.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'n':
        case 'N':
        case 0:
            options.c_cflag &= ~PARENB;   /* Clear parity enable */
            break;

        default:
            fprintf(stderr,"Unsupported parity\n");
            return -1;
    }

    switch (stopbits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;

        case 2:
            options.c_cflag |= CSTOPB;
            break;

        default:
            fprintf(stderr,"Unsupported stop bits\n");
            return -1;
    }

    log_printf(DBG_LV3, "UART.config: %d %d %d", databits, parity, stopbits);

    options.c_cc[VTIME]  = 0;
    options.c_cc[VMIN] = 0;
    options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
    options.c_oflag  &= ~OPOST;   /*Output*/
    options.c_oflag &= ~(ONLCR | OCRNL); //添加的

    options.c_iflag &= ~(ICRNL | INLCR);
    options.c_iflag &= ~(IXON | IXOFF | IXANY); //添加的
    cfmakeraw(&options);

    if (tcsetattr(fd,TCSANOW,&options) != 0) {
        perror("SetupSerial 3");
        return -1;
    }

    tcflush(fd,TCIOFLUSH);
    return 0;
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

static inline void __dump_uart_hex(unsigned char *hex, int len, unsigned int lv)
{
    char buff[1024] = {0};
    int i = 0 ,l = 0, j =0;

    while ( i < len) {
        //if ( j ++ < 20 )
            l += sprintf(&buff[l], "%02X ", hex[i]);
        //else {
            j = 0;
        //    l += sprintf(&buff[l], "%02X\n", hex[i]);
       // }
        i ++;
    }

    log_printf(lv, "UART:%d [ %s ]", len, buff);
}

// 串口4的超时响应
void uart4_Hachiko_notify_proc(Hachiko_EVT evt, void *_private, const struct Hachiko_food *self)
{
    struct bp_uart * thiz = (struct bp_uart * __restrict__)_private;
    struct Hachiko_food *p;

    if ( evt != HACHIKO_TIMEOUT ) return;
    p = & (thiz->rx_seed);

    if ( self == p ) {
        Hachiko_pause(&thiz->rx_seed);
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
void uart4_Hachiko_speed_proc(Hachiko_EVT evt, void *_private,
                            const struct Hachiko_food *self)
{
    struct bp_uart * thiz = (struct bp_uart * __restrict__)_private;
    struct bp_user *u;
    unsigned int i;

    for ( i = 0; i < thiz->users_nr; i ++) {
        u = thiz->users[ i ];
        if ( u->seed <= u->frame_freq ) {
            u->seed ++;
        }
        //log_printf(DBG, "%p: %d-%d", u->user_evt_handle, u->frame_freq, u->seed);
    }
}

/*
 * 串口事件响应函数
 */
int uart4_bp_evt_handle(struct bp_uart *self, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_OK;
    int i;
    struct bp_user *hit;

    switch ( evt ) {
    // 串口数据结构初始化
    case BP_EVT_INIT:
        self->role = BP_UART_MASTER;
        self->rx_seed._private = (void*)self;
        self->rx_seed.Hachiko_notify_proc = uart4_Hachiko_notify_proc;
        self->rx_param.buff.rx_buff = self->rx_buff;
        self->rx_param.cursor = 0;
        self->rx_param.payload_size = 0;
        self->rx_param.buff_size = sizeof(self->rx_buff);

        self->master =NULL;// &self->users[0];
        self->sequce = 0;
        self->continues_nr = 0;

        ret = _Hachiko_new(&self->rx_seed, HACHIKO_AUTO_HOLD,
                     100, HACHIKO_PAUSE, (void*)self);
        if ( ret != ERR_OK ) {
            log_printf(ERR, "UART: create uart reciever's timer faile.");
        }
        break;
    // 串口配置
    case BP_EVT_CONFIGURE:
        //gpio_export(self->hw_port);
        self->dev_handle = open(self->dev_name,
                                O_RDWR | O_NOCTTY);
        if ( self->dev_handle == -1 ) {
            return ERR_UART_OPEN_FAILE;
        } else {
            log_printf(DBG, "UART: self->dev_handle: %d", self->bp_evt_handle);
        }

        if ( self->hw_port == SERIAL4_CTRL_PIN ) {
            //set_speed(self->dev_handle, );
            set_other_attribute(self->dev_handle, 9600, 8, 1, 'N');
            log_printf(INF, "UART: %s 9600,8,1,N", self->dev_name);
        } else if ( self->hw_port == SERIAL5_CTRL_PIN ) {
            //set_speed(self->dev_handle, 2400);
            set_other_attribute(self->dev_handle, 2400, 8, 1, 'O');
            log_printf(INF, "UART: %s 2400,8,1,O", self->dev_name);
        }
        self->status = BP_UART_STAT_WR;
        self->hw_status = BP_UART_STAT_INVALID;
        break;
    // 关闭串口
    case BP_EVT_KILLED:
        close(self->dev_handle);
        break;
    // 串口数据帧校验
    case BP_EVT_FRAME_CHECK:
        ret = ERR_ERR;
        if ( self->master && self->master->user_evt_handle ) {
            ret = self->master->user_evt_handle(self, self->master, BP_EVT_FRAME_CHECK, param);
        }
        if ( !self->master ) {
            log_printf(ERR, "UART.DRIVER : self->master == NULL seq-%d", self->sequce);
        } else if ( ! self->master->user_evt_handle ) {
            log_printf(ERR, "UART.DRIVER: seq-%d", self->sequce);
        } else {};
        break;
    // 切换到发送模式
    case BP_EVT_SWITCH_2_TX:
    case BP_EVT_SWITCH_2_RX:
        break;

    // 串口接收到新数据
    case BP_EVT_RX_DATA:
        if ( self->master && self->master->user_evt_handle ) {
            ret = self->master->user_evt_handle(self, self->master, BP_EVT_RX_DATA, param);
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
            ret = self->master->user_evt_handle(self, self->master, BP_EVT_RX_FRAME, param);
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
            i = self->sequce % self->users_nr;
            log_printf(DBG_LV0, "下一个发送序列为: %d:%d", self->sequce, i);
            hit = self->users[i];

            self->sequce ++;
            if ( self->master != hit || (self->master == hit && self->continues_nr) ) {
                self->master = hit;
                self->master->seed = 0;
                /*鄙视下面的代码*/


                /*
                 *  从没见过在同一个485总线上使用不同的通信配置，这个做法是为了在这个框架里
                 *  兼容英可瑞的整流模块转换单元.
                 * PS：
                 *  我就操蛋了，妈的！换个毛整流模块啊，英可瑞的模块转换盒采用的是9600,8,E,1的
                 *  通信模式，非要在这里做个兼容，整个串口通信框架被玩的像傻叉一样！！
                 *  FUCK!  FUCK!
                 */
                if ( self->master->hw_other != self->hw_other ) {
                    self->hw_other = self->master->hw_other;
                    self->hw_bps = self->master->hw_bps;
                    set_other_attribute(self->dev_handle,
                                        self->master->hw_bps,
                                        self->hw_other>>16,
                                        self->hw_other & 0xFF,
                                        (self->hw_other >> 8)&0xFF);

                    log_printf(DBG_LV3, "%s: %d %d,'%C',%d",
                               self->dev_name,
                               self->hw_bps,
                               self->hw_other>>16,
                               (self->hw_other >> 8)&0xFF,
                               self->hw_other & 0xFF);
                }


                /* 鄙视上面的代码 */
                ret = hit->user_evt_handle(self, self->master, BP_EVT_TX_FRAME_REQUEST, param);
                log_printf(DBG_LV1, "UART: %s BP_EVT_TX_FRAME_REQUEST ret: %d, load: %d, sent: %d",
                           self->master->name,
                           ret, param->payload_size, hit->sent_frames);
            } else {
                log_printf(WRN, "UART: 发生程序时序故障");
                self->master = NULL;
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
            ret = self->master->user_evt_handle(self, self->master, BP_EVT_TX_FRAME_CONFIRM, param);
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
            ret = self->master->user_evt_handle(self, self->master, BP_EVT_TX_FRAME_DONE, param);
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
            ret = self->master->user_evt_handle(self, self->master, BP_EVT_RX_BYTE_TIMEOUT, param);
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
            ret = self->master->user_evt_handle(self, self->master, BP_EVT_RX_FRAME_TIMEOUT, param);
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
            ret = self->master->user_evt_handle(self, self->master, BP_EVT_FRAME_CHECK_ERROR, param);
        } else {
            log_printf(WRN, "UART: "RED("BP_EVT_FRAME_CHECK_ERROR")" without signal procedure.");
        }
        break;
    }
    return ret;
}

// 只读数据段
int uart4_charger_yaoce_0_49_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_ERR;
    unsigned char buff[8];

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
            bit_clr(task, S_CHARGER_YX_1_COMM_DOWN);
            log_printf(INF, "UART: "GRN("充电机监控通讯(次要0-49)恢复"));
        }
        memcpy(&me->chargers->chargers, &param->buff.rx_buff[3], 100);
        if ( me->chargers->chargers.charger_status!= 0) {
            log_printf(WRN, "UART: 充电机组故障.");
            bit_set(task, S_CHARGE_GROUP_ERR);
        } else {
            bit_clr(task, S_CHARGE_GROUP_ERR);
        }
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        param->attrib = BP_FRAME_UNSTABLE;
        buff[0] = 0x01;
        buff[1] = 0x04;
        buff[2] = 0x00;
        buff[3] = 0x00;
        buff[4] = 0x00;
        buff[5] = 0x32;
        buff[6] = load_crc(6, buff);
        buff[7] = load_crc(6, buff) >>8;
        memcpy(param->buff.tx_buff, buff, sizeof(buff));

        param->payload_size = sizeof(buff);
        self->rx_param.need_bytes = 105;
        self->master->time_to_send = param->payload_size * 1000 / 960 + self->master->swap_time_modify;

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

int uart4_charger_yaoce_50_100_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_ERR, i;
    unsigned char buff[8];

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
            bit_clr(task, S_CHARGER_YX_2_COMM_DOWN);
            log_printf(INF, "UART: "GRN("充电机监控通讯(次要50-100)恢复"));
        }
        memcpy(&me->chargers->chargers.charge_module_status, &param->buff.rx_buff[3], 100);
        for (i = 0; i < CONFIG_SUPPORT_CHARGE_MODULE; i ++ ) {
            // 判断模块故障
            if ( i % 2 == 0 ) {
                param->buff.rx_buff[3 + i] = param->buff.rx_buff[3 + i] ^ param->buff.rx_buff[4 + i];
                param->buff.rx_buff[4 + i] = param->buff.rx_buff[3 + i] ^ param->buff.rx_buff[4 + i];
                param->buff.rx_buff[3 + i] = param->buff.rx_buff[3 + i] ^ param->buff.rx_buff[4 + i];
            }
            if ( param->buff.rx_buff[i + 3] & 0x0F ) {
                bit_set(task, S_CHARGE_M_1_ERR + i);
            } else {
                bit_clr(task, S_CHARGE_M_1_ERR + i);
            }
        }
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
        buff[6] = load_crc(6, buff);
        buff[7] = load_crc(6, buff) >>8;
        memcpy(param->buff.tx_buff, buff, sizeof(buff));

        param->payload_size = sizeof(buff);
        self->rx_param.need_bytes = 105;
        self->master->time_to_send = param->payload_size * 1000 / 960 + self->master->swap_time_modify;

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
int uart4_charger_config_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
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
            log_printf(WRN, "UART: CRC cheke result: need: %04X, gave: %04X",
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
        buff[nr ++] = (unsigned short)val >> 8;
        buff[nr ++] = (unsigned short)val;

        // 充电命令
        buff[nr ++] = 0x00;
        buff[nr ++] = 0x01;

        val = (unsigned int)atoi(config_read("需求电压"));
        // 需求电压
        buff[nr ++] = (unsigned short)val >> 8;
        buff[nr ++] = (unsigned short)val;

        val = (unsigned int)atoi(config_read("需求电流"));
        // 需求电流
        buff[nr ++] = (unsigned short)val >> 8;
        buff[nr ++] = (unsigned short)val;
        s = nr;

        log_printf(INF, "初始电压:%.1f V, 需求电压: %.1f V, 需求电流: %.1f A",
                   atof(config_read("初始电压"))/10.0f, atof(config_read("需求电压"))/10.0f,
                   atof(config_read("需求电流"))/10.0f);

        // CRC
        buff[nr ++] = load_crc(s, buff);
        buff[nr ++] = load_crc(s, buff) >> 8;
        memcpy(param->buff.tx_buff, buff, nr);
        param->payload_size = nr;
        ret = ERR_OK;

        self->rx_param.need_bytes = 8;
        self->master->time_to_send = param->payload_size * 1000 / 960;

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
int uart4_charger_module_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
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
int uart4_charger_date_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_ERR;
    unsigned char buff[8];

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


int simple_box_read_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_ERR, nr = 0, len = 0, errnr = 0;
    unsigned char buff[32] = {0};
    struct MDATA_ACK *box, *me_pre;
    char errstr[1024] = {0};
    char infstr[1024] = {0};

    int need_echo = 0;

    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size < param->need_bytes ) {
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
        } else {
            unsigned short crc = load_crc(param->need_bytes-2, param->buff.rx_buff);
            unsigned short check = param->buff.rx_buff[ param->need_bytes - 2 ] |
                    param->buff.rx_buff[ param->need_bytes - 1] << 8;
            log_printf(WRN, "UART: CRC cheke result: need: %04X, gave: %04X",
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
        if ( bit_read(task, S_MEASURE_1_COMM_DOWN) ) {
            log_printf(INF, "UART: "GRN("综合采样盒通信恢复."));
        }
        bit_clr(task, S_MEASURE_1_COMM_DOWN);
        self->master->died = 0;

        // 字节序转置
        do {
            int f = (unsigned int)(&(((struct MDATA_ACK *)0)->Ver));
            int t = (unsigned int)(&(((struct MDATA_ACK *)0)->unused));
            int j;
            for ( j = f; j < t; j = j + 2 ) {
                param->buff.rx_buff[ j ] = param->buff.rx_buff[ j ] ^ param->buff.rx_buff[ j + 1];
                param->buff.rx_buff[ j + 1 ] = param->buff.rx_buff[ j ] ^ param->buff.rx_buff[ j + 1];
                param->buff.rx_buff[ j ] = param->buff.rx_buff[ j ] ^ param->buff.rx_buff[ j + 1];
            }
        } while (0);

        memcpy(&me->measure->measure_pre_copy, &me->measure->measure, sizeof(struct MDATA_ACK));
        memcpy(&me->measure->measure, param->buff.rx_buff, sizeof(struct MDATA_ACK));

        // 故障判定
        box = &me->measure->measure;
        me_pre = &me->measure->measure_pre_copy;
        if ( box->Flag_prtc1 & 0x01 ) {
            len += sprintf(&errstr[len], "[%d: 母线0过压] ", ++errnr);
            bit_set(task, S_BUS_0_VHI);
        } else {
            bit_clr(task, S_BUS_0_VHI);
        }
        if ( box->Flag_prtc1 & 0x02 ) {
            len += sprintf(&errstr[len], "[%d: 母线0欠压] ", ++errnr);
            bit_set(task, S_BUS_0_VLO);
        } else {
            bit_clr(task, S_BUS_0_VLO);
        }
        if ( box->Flag_prtc1 & 0x04 ) {
            len += sprintf(&errstr[len], "[%d: 母线0短路] ", ++errnr);
            bit_set(task, S_BUS_0_SHORT);
        } else {
            bit_clr(task, S_BUS_0_SHORT);
        }
        if ( box->Flag_prtc1 & 0x08 ) {
            len += sprintf(&errstr[len], "[%d: 母线1过压] ", ++errnr);
            bit_set(task, S_BUS_1_VHI);
        } else {
            bit_clr(task, S_BUS_1_VHI);
        }
        if ( box->Flag_prtc1 & 0x10 ) {
            len += sprintf(&errstr[len], "[%d: 母线1欠压] ", ++errnr);
            bit_set(task, S_BUS_1_VLO);
        } else {
            bit_clr(task, S_BUS_1_VLO);
        }
        if ( box->Flag_prtc1 & 0x20 ) {
            len += sprintf(&errstr[len], "[%d: 母线1短路] ", ++errnr);
            bit_set(task, S_BUS_1_SHORT);
        } else {
            bit_clr(task, S_BUS_1_SHORT);
        }

        if ( box->Flag_prtc2 & 0x01 ) {
            len += sprintf(&errstr[len], "[%d: 电池0过压] ", ++errnr);
            bit_set(task, S_BAT_0_VHI);
        } else {
            bit_clr(task, S_BAT_0_VHI);
        }
        if ( box->Flag_prtc2 & 0x02 ) {
            len += sprintf(&errstr[len], "[%d: 电池0欠压] ", ++errnr);
            bit_set(task, S_BAT_0_VLO);
        } else {
            bit_clr(task, S_BAT_0_VLO);
        }
        if ( box->Flag_prtc2 & 0x04 ) {
            len += sprintf(&errstr[len], "[%d: 电池0链接短路] ", ++errnr);
            bit_set(task, S_BAT_0_SHORT);
        } else {
            bit_clr(task, S_BAT_0_SHORT);
        }
        if ( box->Flag_prtc2 & 0x08 ) {
            len += sprintf(&errstr[len], "[%d: 电池0反接] ", ++errnr);
            bit_set(task, S_BAT_0_REVERT);
        } else {
            bit_clr(task, S_BAT_0_REVERT);
        }
        if ( box->Flag_prtc2 & 0x10 ) {
            len += sprintf(&errstr[len], "[%d: 电池0绝缘接地] ", ++errnr);
            bit_set(task, S_BAT_0_INSTITUDE);
        } else {
            bit_clr(task, S_BAT_0_INSTITUDE);
        }
        if ( box->Flag_prtc2 & 0x20 ) {
            len += sprintf(&errstr[len], "[%d: 电池0过流] ", ++errnr);
            bit_set(task, S_BAT_0_IHI);
        } else {
            bit_clr(task, S_BAT_0_IHI);
        }
        if ( box->Flag_prtc3 & 0x01 ) {
            len += sprintf(&errstr[len], "[%d: 电池1过压] ", ++errnr);
            bit_set(task, S_BAT_1_VHI);
        } else {
            bit_clr(task, S_BAT_1_VHI);
        }
        if ( box->Flag_prtc3 & 0x02 ) {
            len += sprintf(&errstr[len], "[%d: 电池1欠压] ", ++errnr);
            bit_set(task, S_BAT_1_VLO);
        } else {
            bit_clr(task, S_BAT_1_VLO);
        }
        if ( box->Flag_prtc3 & 0x04 ) {
            len += sprintf(&errstr[len], "[%d: 电池1链接短路] ", ++errnr);
            bit_set(task, S_BAT_1_SHORT);
        } else {
            bit_clr(task, S_BAT_1_SHORT);
        }
        if ( box->Flag_prtc3 & 0x08 ) {
            len += sprintf(&errstr[len], "[%d: 电池1反接] ", ++errnr);
            bit_set(task, S_BAT_1_REVERT);
        } else {
            bit_clr(task, S_BAT_1_REVERT);
        }
        if ( box->Flag_prtc3 & 0x10 ) {
            len += sprintf(&errstr[len], "[%d: 电池1绝缘接地] ", ++errnr);
            bit_set(task, S_BAT_1_INSTITUDE);
        } else {
            bit_clr(task, S_BAT_1_INSTITUDE);
        }
        if ( box->Flag_prtc3 & 0x20 ) {
            len += sprintf(&errstr[len], "[%d: 电池1过流] ", ++errnr);
            bit_set(task, S_BAT_1_IHI);
        } else {
            bit_clr(task, S_BAT_1_IHI);
        }

        if ( box->Flag_prtc4 & 0x01 ) {
            len += sprintf(&errstr[len], "[%d: 辅助电源故障] ", ++errnr);
            bit_set(task, S_ASSIT_PWN_ERR);
        } else {
            bit_clr(task, S_ASSIT_PWN_ERR);
        }
        if ( ((box->Flag_prtc4 & (0x02|0x04)) >> 1) == 1 ) {
            len += sprintf(&errstr[len], "[%d: 温度过高] ", ++errnr);
            bit_set(task, S_TEMP_HI);
        } else if ( ((box->Flag_prtc4 & (0x02|0x04)) >> 1) == 2 ) {
            len += sprintf(&errstr[len], "[%d: 温度过低] ", ++errnr);
            bit_set(task, S_TEMP_LO);
        } else {
            bit_clr(task, S_TEMP_HI);
            bit_clr(task, S_TEMP_LO);
        }
        if ( ((box->Flag_prtc4 & (0x02|0x04)) >> 3) == 1 ) {
            len = sprintf(&errstr[len], "[%d: 湿度过高] ", ++errnr);
            bit_set(task, S_WET_HI);
        } else if ( ((box->Flag_prtc4 & (0x02|0x04)) >> 3) == 2 ) {
            len += sprintf(&errstr[len], "[%d: 湿度过低] ", ++errnr);
            bit_set(task, S_WET_LO);
        } else {
            bit_clr(task, S_WET_HI);
            bit_clr(task, S_WET_LO);
        }

        if ( box->Flag_prtc6 & 0x01 ) {
            len += sprintf(&errstr[len], "[%d: 总输出熔断器熔断] ", ++errnr);
            bit_set(task, S_DC_OUTPUT_RD);
        } else {
            bit_clr(task, S_DC_OUTPUT_RD);
        }
        if ( box->Flag_prtc6 & 0x02 ) {
            len += sprintf(&errstr[len], "[%d: 总输出跳闸] ", ++errnr);
            bit_set(task, S_DC_OUTPUT_TRIP);
        } else {
            bit_clr(task, S_DC_OUTPUT_TRIP);
        }
        if ( box->Flag_prtc6 & 0x04 ) {
            len += sprintf(&errstr[len], "[%d: 一路输出跳闸] ", ++errnr);
            bit_set(task, S_DC_OUTPUT_0_TRIP);
        } else {
            bit_clr(task, S_DC_OUTPUT_0_TRIP);
        }
        if ( box->Flag_prtc6 & 0x08 ) {
            len += sprintf(&errstr[len], "[%d: 二路输出跳闸] ", ++errnr);
            bit_set(task, S_DC_OUTPUT_1_TRIP);
        } else {
            bit_clr(task, S_DC_OUTPUT_1_TRIP);
        }
        if ( box->Flag_prtc6 & 0x10 ) {
            len += sprintf(&errstr[len], "[%d: 防雷器故障] ", ++errnr);
            bit_set(task, S_FLQ_ERROR);
        } else {
            bit_clr(task, S_FLQ_ERROR);
        }
        if ( box->Flag_prtc6 & 0x20 ) {
            len += sprintf(&errstr[len], "[%d: 交流输入跳闸] ", ++errnr);
            bit_set(task, S_AC_SWITCH_TRIP);
        } else {
            bit_clr(task, S_AC_SWITCH_TRIP);
        }
        if ( box->Flag_prtc6 & 0x40 ) {
            len += sprintf(&errstr[len], "[%d: 急停] ", ++errnr);
            bit_set(task, S_HALT);
        } else {
            bit_clr(task, S_HALT);
        }

        if ( errnr ) {
            log_printf(DBG_LV3, "Fault: "RED("%s"), errstr);
        }
        // 输入状态，遥信
        len = 0;
        if ( box->Flag_run1 & 0x01 ) {
            if ( ! (me_pre->Flag_run1 & 0x01) ) {
                log_printf(INF, "采样盒: 交流开关合闸.");
            }
            len += sprintf(&infstr[len], "[交流"GRN("合闸")"] ");
        } else {
            if ( me_pre->Flag_run1 & 0x01 ) {
                log_printf(INF, "采样盒: 交流开关分闸.");
            }
            len += sprintf(&infstr[len], "[交流"RED("分闸")"] ");
        }
        if ( box->Flag_run1 & 0x02 ) {
            if ( ! (me_pre->Flag_run1 & 0x02) ) {
                log_printf(INF, "采样盒: 开始加热.");
            }
            len += sprintf(&infstr[len], "[加热] ");
        } else {
            if ( me_pre->Flag_run1 & 0x02 ) {
                log_printf(INF, "采样盒: 停止加热.");
            }
            len += sprintf(&infstr[len], "[未加热] ");
        }
        if ( box->Flag_run1 & 0x04 ) {
            if ( ! (me_pre->Flag_run1 & 0x04) ) {
                log_printf(INF, "采样盒: 开始通风.");
            }
            len += sprintf(&infstr[len], "[通风] ");
        } else {
            if ( me_pre->Flag_run1 & 0x04 ) {
                log_printf(INF, "采样盒: 停止通风.");
            }
            len += sprintf(&infstr[len], "[未通风] ");
        }
        if ( box->Flag_run1 & 0x08 ) {
            if ( ! (me_pre->Flag_run1 & 0x08) ) {
                log_printf(INF, "采样盒: 总输出"GRN("合闸"));
            }
            bit_set(task, F_DC_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[总输出"GRN("合闸")"] ");
        } else {
            if ( me_pre->Flag_run1 & 0x08 ) {
                log_printf(INF, "采样盒: 总输出"RED("分闸"));
            }
            bit_clr(task, F_DC_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[总输出"RED("分闸")"] ");
        }
        if ( box->Flag_run1 & 0x10 ) {
            if ( ! (me_pre->Flag_run1 & 0x10) ) {
                log_printf(INF, "采样盒: 1#枪输出"GRN("合闸")".");
            }
            bit_set(task, F_GUN_1_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[1#枪输出"GRN("合闸")"] ");
        } else {
            if ( me_pre->Flag_run1 & 0x10 ) {
                log_printf(INF, "采样盒: 1#枪输出"RED("分闸"));
            }
            bit_clr(task, F_GUN_1_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[1#枪输出"RED("分闸")"] ");
        }
        if ( ((box->Flag_run1 & (0x20 | 0x40)) >> 5) == 0 ) {
            if ( ((me_pre->Flag_run1 & (0x20 | 0x40)) >> 5) != 0 ) {
                log_printf(INF, "采样盒: 1#枪断开连接");
                need_echo ++;
            }
            bit_clr(task, F_GUN_1_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[1#枪未链接] ");
        } else if (((box->Flag_run1 & (0x20 | 0x40)) >> 5) == GUN_CONN_PROTECTED ) {
            bit_clr(task, F_GUN_1_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[1#枪链接保护] ");
        } else if ( ((box->Flag_run1 & (0x20 | 0x40)) >> 5) == GUN_CONN_EXCEPTION ) {
            bit_clr(task, F_GUN_1_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[1#枪连接异常] ");
        } else if ( ((box->Flag_run1 & (0x20 | 0x40)) >> 5) == GUN_CONN_CONNECTIVE ) {
            if ( ((me_pre->Flag_run1 & (0x20 | 0x40)) >> 5) != GUN_CONN_CONNECTIVE ) {
                log_printf(INF, "采样盒: 1#枪连接完成.");
                need_echo ++;
            }
            bit_set(task, F_GUN_1_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[1#枪链接"GRN("正常")"] ");
        }
        if ( box->Flag_run1 & 0x80 ) {
            if ( !(me_pre->Flag_run1 & 0x80) ) {
                log_printf(INF, "采样盒: 1#枪辅助电源合闸.");
            }
            bit_set(task, F_GUN_1_ASSIT_PWN_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[1#枪辅助电源"GRN("合闸")"] ");
        } else {
            if ( me_pre->Flag_run1 & 0x80 ) {
                log_printf(INF, "采样盒: 1#枪辅助电源分闸.");
            }
            bit_clr(task, F_GUN_1_ASSIT_PWN_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[1#枪辅助电源"RED("分闸")"] ");
        }
        if ( box->Flag_run2 & 0x01 ) {
            if ( !(me_pre->Flag_run2 & 0x01) ) {
                log_printf(INF, "采样盒: 2#枪输出"GRN("合闸")"");
            }
            bit_set(task, F_GUN_2_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[2#枪输出"GRN("合闸")"] ");
        } else {
            if ( me_pre->Flag_run2 & 0x01 ) {
                log_printf(INF, "采样盒: 2#枪输出"RED("分闸")"");
            }
            bit_clr(task, F_GUN_2_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[2#枪输出"RED("分闸")"] ");
        }
        if ( (box->Flag_run2 & (0x02|0x04)) == 0 ) {
            if ( (me_pre->Flag_run2 & (0x02|0x04)) != 0 ) {
                log_printf(INF, "采样盒: 2#枪断开连接");
                need_echo ++;
            }
            bit_clr(task, F_GUN_2_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[2#枪未链接] ");
        } else if ((box->Flag_run2 & (0x02|0x04)) >> 1 == GUN_CONN_PROTECTED ) {
            bit_clr(task, F_GUN_2_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[2#枪链接保护] ");
        } else if ( (box->Flag_run2 & (0x02|0x04)) >> 1 == GUN_CONN_EXCEPTION ) {
            bit_clr(task, F_GUN_2_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[2#枪连接异常] ");
        } else if ( (box->Flag_run2 & (0x02|0x04)) >> 1 == GUN_CONN_CONNECTIVE ) {
            if ( (me_pre->Flag_run2 & (0x02|0x04)) >> 1 != GUN_CONN_CONNECTIVE ) {
                log_printf(INF, "采样盒: 2#枪连接完成.");
                need_echo ++;
            }
            bit_set(task, F_GUN_2_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[2#枪链接"GRN("正常")"] ");
        }
        if ( box->Flag_run2 & 0x08 ) {
            if ( !(me_pre->Flag_run2 & 0x08) ) {
                log_printf(INF, "采样盒: 2#枪辅助电源"GRN("合闸"));
            }
            bit_set(task, F_GUN_2_ASSIT_PWN_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[2#枪辅助电源"GRN("合闸")"] ");
        } else {
            if ( me_pre->Flag_run2 & 0x08 ) {
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

        buff[ nr ++ ] = 0xF0;
        buff[ nr ++ ] = 0xE1;
        buff[ nr ++ ] = 0xD2;
        buff[ nr ++ ] = 0xC3;
        buff[ nr ++ ] = 0x05;
        buff[ nr ++ ] = 0x03;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x20;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x14;
        len = nr;
        buff[ nr ++ ] = load_crc(len, buff);
        buff[ nr ++ ] = load_crc(len, buff) >> 8;

        memcpy(param->buff.tx_buff, buff, nr);
        param->payload_size = nr;

        self->rx_param.need_bytes = 49;
        self->master->time_to_send = param->payload_size * 1000 / 960 + self->master->swap_time_modify;
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
            if ( ! bit_read(task, S_MEASURE_1_COMM_DOWN) ) {
                log_printf(ERR, "UART: "RED("综合采样盒通信中断, 请排查故障,"
                                            " 已禁止充电(%d)."), self->master->died);
            }
            bit_set(task, S_MEASURE_1_COMM_DOWN);
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

#define DC_SWITCH_ON        0x01
#define GUN1_ASSIT_PWN_ON   0x02
#define GUN1_OUTPUT_ON      0x04
#define GUN2_ASSIT_PWN_ON   0x08
#define GUN2_OUTPUT_ON      0x10
int simple_box_write_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_ERR;
    unsigned char buff[64];
    unsigned short cmd = 0;
    int nr = 0, len = 0;

    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size < param->need_bytes ) {
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
        } else {
            unsigned short crc = load_crc(param->need_bytes-2, param->buff.rx_buff);
            unsigned short check = param->buff.rx_buff[ param->need_bytes - 2 ] |
                    param->buff.rx_buff[ param->need_bytes - 1] << 8;
            log_printf(WRN, "UART: CRC cheke result: need: %04X, gave: %04X",
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

        if ( bit_read(task, CMD_GUN_1_ASSIT_PWN_ON) ) {
            cmd |= GUN1_ASSIT_PWN_ON;
            cmd &= ~GUN2_ASSIT_PWN_ON;
        } else {
            cmd &= ~GUN1_ASSIT_PWN_ON;
        }
        if ( bit_read(task, CMD_GUN_2_ASSIT_PWN_ON) ) {
            cmd |= GUN2_ASSIT_PWN_ON;
            cmd &= ~GUN1_ASSIT_PWN_ON;
        } else {
            cmd &= ~GUN2_ASSIT_PWN_ON;
        }
        if ( bit_read(task, CMD_GUN_1_OUTPUT_ON) ) {
            cmd |= GUN1_OUTPUT_ON;
            cmd &= ~GUN2_OUTPUT_ON;
        } else {
            cmd &= ~GUN1_OUTPUT_ON;
        }
        if ( bit_read(task, CMD_GUN_2_OUTPUT_ON) ) {
            cmd |= GUN2_OUTPUT_ON;
            cmd &= ~GUN1_OUTPUT_ON;
        } else {
            cmd &= ~GUN2_OUTPUT_ON;
        }

        if ( bit_read(task, CMD_DC_OUTPUT_SWITCH_ON) ) {
            cmd |= DC_SWITCH_ON;
        } else {
            cmd &= ~DC_SWITCH_ON;
        }

        if ( bit_read(task, F_CHARGE_LED) ) {
            cmd |= 0x80;
        } else {
            cmd &= ~0x80;
        }

        if ( bit_read(task, S_ERROR) ) {
            cmd |= 0x100;
        } else {
            cmd &= ~0x100;
        }

        buff[ nr ++ ] = 0xF0;
        buff[ nr ++ ] = 0xE1;
        buff[ nr ++ ] = 0xD2;
        buff[ nr ++ ] = 0xC3;
        buff[ nr ++ ] = 0x05;
        buff[ nr ++ ] = 0x10;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x02;
        buff[ nr ++ ] = 0x04;
        buff[ nr ++ ] = cmd >> 8;
        buff[ nr ++ ] = cmd;
        buff[ nr ++ ] = cmd >> 8;
        buff[ nr ++ ] = cmd;
        len = nr;
        buff[ nr ++ ] = load_crc(len, buff);
        buff[ nr ++ ] = load_crc(len, buff) >> 8;

        memcpy(param->buff.tx_buff, buff, nr);
        param->payload_size = nr;

        self->rx_param.need_bytes = 12;
        self->master->time_to_send = (param->payload_size + 1) * 1000 / 960 + self->master->swap_time_modify;
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

// 配置上下限
int simple_box_configwrite_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_ERR;
    unsigned char buff[64];
    int nr = 0, len = 0;

    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size < param->need_bytes ) {
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
        } else {
            unsigned short crc = load_crc(param->need_bytes-2, param->buff.rx_buff);
            unsigned short check = param->buff.rx_buff[ param->need_bytes - 2 ] |
                    param->buff.rx_buff[ param->need_bytes - 1] << 8;
            log_printf(WRN, "UART: CRC cheke result: need: %04X, gave: %04X",
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

        buff[ nr ++ ] = 0xF0;
        buff[ nr ++ ] = 0xE1;
        buff[ nr ++ ] = 0xD2;
        buff[ nr ++ ] = 0xC3;
        buff[ nr ++ ] = 0x05;
        buff[ nr ++ ] = 0x10;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x02;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 13;
        buff[ nr ++ ] = 26;
        buff[ nr ++ ] = double2short(task->bus_1_v_hi, 10) >>8;
        buff[ nr ++ ] = double2short(task->bus_1_v_hi, 10);
        buff[ nr ++ ] = double2short(task->bus_1_v_lo, 10) >>8;
        buff[ nr ++ ] = double2short(task->bus_1_v_lo, 10);
        buff[ nr ++ ] = double2short(task->bus_2_v_hi, 10) >>8;
        buff[ nr ++ ] = double2short(task->bus_2_v_hi, 10);
        buff[ nr ++ ] = double2short(task->bus_2_v_lo, 10) >>8;
        buff[ nr ++ ] = double2short(task->bus_2_v_lo, 10);
        buff[ nr ++ ] = double2short(task->bat_1_v_hi, 10) >>8;
        buff[ nr ++ ] = double2short(task->bat_1_v_hi, 10);
        buff[ nr ++ ] = double2short(task->bat_1_v_lo, 10) >>8;
        buff[ nr ++ ] = double2short(task->bat_1_v_lo, 10);
        buff[ nr ++ ] = double2short(task->bat_2_v_hi, 10) >>8;
        buff[ nr ++ ] = double2short(task->bat_2_v_hi, 10);
        buff[ nr ++ ] = double2short(task->bat_2_v_lo, 10) >>8;
        buff[ nr ++ ] = double2short(task->bat_2_v_lo, 10);
        buff[ nr ++ ] = double2short(task->bat_1_I_hi, 10) >>8;
        buff[ nr ++ ] = double2short(task->bat_1_I_hi, 10);
        buff[ nr ++ ] = double2short(task->bat_2_I_hi, 10) >>8;
        buff[ nr ++ ] = double2short(task->bat_2_I_hi, 10);
        buff[ nr ++ ] = double2short(task->bat_1_insti_ohm_v, 10) >>8;
        buff[ nr ++ ] = double2short(task->bat_1_insti_ohm_v, 10);
        buff[ nr ++ ] = double2short(task->bat_2_insti_ohm_v, 10) >>8;
        buff[ nr ++ ] = double2short(task->bat_2_insti_ohm_v, 10);
        buff[ nr ++ ] = task->flq_xishu >> 8;
        buff[ nr ++ ] = task->flq_xishu;
        len = nr;
        buff[ nr ++ ] = load_crc(len, buff);
        buff[ nr ++ ] = load_crc(len, buff) >> 8;

        memcpy(param->buff.tx_buff, buff, nr);
        param->payload_size = nr;

        self->rx_param.need_bytes = 12;
        self->master->time_to_send = (param->payload_size + 1) * 1000 / 960 + self->master->swap_time_modify;
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

int simple_box_correct_write_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_ERR, nr  = 0, len;
    unsigned char buff[32];

    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size < param->need_bytes ) {
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
        } else {
            unsigned short crc = load_crc(param->need_bytes-2, param->buff.rx_buff);
            unsigned short check = param->buff.rx_buff[ param->need_bytes - 2 ] |
                    param->buff.rx_buff[ param->need_bytes - 1] << 8;
            log_printf(WRN, "UART: CRC cheke result: need: %04X, gave: %04X",
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
        if ( bit_read(task, CMD_JIAOZHUN_BUS1_V) &&
             bit_read(task, CMD_JIAOZHUN_BUS2_V) &&
             bit_read(task, CMD_JIAOZHUN_BAT_I) ) {
            log_printf(INF, "UART: 校准数据已保存.");
            ret = ERR_OK;
            task->bus2_correct_V = 199999.9;
            task->bus1_correct_V = 199999.9;
            task->bus_correct_I = 199999.9;
            }
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        param->attrib = BP_FRAME_UNSTABLE;
        if ( bit_read(task, CMD_JIAOZHUN_BUS1_V) &&
             bit_read(task, CMD_JIAOZHUN_BUS2_V) &&
             bit_read(task, CMD_JIAOZHUN_BAT_I) ) {
            buff[ nr ++ ] = 0xF0;
            buff[ nr ++ ] = 0xE1;
            buff[ nr ++ ] = 0xD2;
            buff[ nr ++ ] = 0xC3;
            buff[ nr ++ ] = 0x05;
            buff[ nr ++ ] = 0x10;
            buff[ nr ++ ] = 0x00;
            buff[ nr ++ ] = 15;
            buff[ nr ++ ] = 0x00;
            buff[ nr ++ ] = 0x03;
            buff[ nr ++ ] = 0x06;
            buff[ nr ++ ] = 0;
            buff[ nr ++ ] = 0;
            buff[ nr ++ ] = 0;
            buff[ nr ++ ] = 0;
            buff[ nr ++ ] = 0;
            buff[ nr ++ ] = 0;
            self->rx_param.need_bytes = 12;

            len = nr;
            buff[ nr ++ ] = load_crc(len, buff);
            buff[ nr ++ ] = load_crc(len, buff) >> 8;

            memcpy(param->buff.tx_buff, buff, nr);
            param->payload_size = nr;

            self->master->time_to_send = param->payload_size * 1000 / 960;
            if ( bit_read(task, CMD_JIAOZHUN_BUS1_V) ||
                 bit_read(task, CMD_JIAOZHUN_BUS2_V) ||
                 bit_read(task, CMD_JIAOZHUN_BAT_I) ) {
                log_printf(DBG_LV3, "UART: %s sent", __FUNCTION__);
                ret = ERR_OK;
                break;
            } else {
                ret = ERR_ERR;
                break;
            }
            log_printf(DBG_LV3, "UART: %s sent", __FUNCTION__);
            ret = ERR_OK;
        } else {
            buff[ nr ++ ] = 0xF0;
            buff[ nr ++ ] = 0xE1;
            buff[ nr ++ ] = 0xD2;
            buff[ nr ++ ] = 0xC3;
            buff[ nr ++ ] = 0x05;
            buff[ nr ++ ] = 0x10;
            buff[ nr ++ ] = 0x00;
            buff[ nr ++ ] = 15;
            buff[ nr ++ ] = 0x00;
            buff[ nr ++ ] = 3;
            buff[ nr ++ ] = 6;
            buff[ nr ++ ] = double2short(task->bus1_correct_V, 1) >> 8;
            buff[ nr ++ ] = double2short(task->bus1_correct_V, 1) & 0xFF;
            buff[ nr ++ ] = double2short(task->bus2_correct_V, 1) >> 8;
            buff[ nr ++ ] = double2short(task->bus2_correct_V, 1) & 0xFF;
            buff[ nr ++ ] = double2short(task->bus_correct_I, 1) >> 8;
            buff[ nr ++ ] = double2short(task->bus_correct_I, 1) & 0xFF;
            self->rx_param.need_bytes = 12;

            len = nr;
            buff[ nr ++ ] = load_crc(len, buff);
            buff[ nr ++ ] = load_crc(len, buff) >> 8;

            memcpy(param->buff.tx_buff, buff, nr);
            param->payload_size = nr;

            self->master->time_to_send = param->payload_size * 1000 / 960;
            if ( bit_read(task, CMD_JIAOZHUN_BUS1_V) ||
                 bit_read(task, CMD_JIAOZHUN_BUS2_V) ||
                 bit_read(task, CMD_JIAOZHUN_BAT_I) ) {
                log_printf(DBG_LV3, "UART: %s sent", __FUNCTION__);
                ret = ERR_OK;
            } else {
                ret = ERR_ERR;
            }
        }
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

// 读取校正时的数据
int simple_box_correct_read_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_ERR, nr = 0, len = 0, errnr = 0;
    unsigned char buff[32] = {0};
    struct MDATA_ACK *box, *me_pre;
    char errstr[1024] = {0};
    char infstr[1024] = {0};

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
        if ( bit_read(task, S_MEASURE_1_COMM_DOWN) ) {
            log_printf(INF, "UART: "GRN("综合采样盒通信恢复."));
        }
        bit_clr(task, S_MEASURE_1_COMM_DOWN);
        self->master->died = 0;

        // 字节序转置
        do {
            int f = (unsigned int)(&(((struct MDATA_ACK *)0)->Ver));
            int t = (unsigned int)(&(((struct MDATA_ACK *)0)->unused));
            int j;
            for ( j = f; j < t; j = j + 2 ) {
                param->buff.rx_buff[ j ] = param->buff.rx_buff[ j ] ^ param->buff.rx_buff[ j + 1];
                param->buff.rx_buff[ j + 1 ] = param->buff.rx_buff[ j ] ^ param->buff.rx_buff[ j + 1];
                param->buff.rx_buff[ j ] = param->buff.rx_buff[ j ] ^ param->buff.rx_buff[ j + 1];
            }
        } while (0);

        memcpy(&me->measure->measure_pre_copy, &me->measure->measure, sizeof(struct MDATA_ACK));
        memcpy(&me->measure->measure, param->buff.rx_buff, sizeof(struct MDATA_ACK));

        task->bus1_read_V = __bytes2double(b2l(task->measure[0]->measure.VinKM0));
        task->bus2_read_V = __bytes2double(b2l(task->measure[0]->measure.VinKM1));
        task->bus_read_I = __bytes2double(b2l(task->measure[0]->measure.IoutBAT0));
        // 故障判定
        box = &me->measure->measure;
        me_pre = &me->measure->measure_pre_copy;
        if ( box->Flag_prtc1 & 0x01 ) {
            len += sprintf(&errstr[len], "[%d: 母线0过压] ", ++errnr);
            bit_set(task, S_BUS_0_VHI);
        } else {
            bit_clr(task, S_BUS_0_VHI);
        }
        if ( box->Flag_prtc1 & 0x02 ) {
            len += sprintf(&errstr[len], "[%d: 母线0欠压] ", ++errnr);
            bit_set(task, S_BUS_0_VLO);
        } else {
            bit_clr(task, S_BUS_0_VLO);
        }
        if ( box->Flag_prtc1 & 0x04 ) {
            len += sprintf(&errstr[len], "[%d: 母线0短路] ", ++errnr);
            bit_set(task, S_BUS_0_SHORT);
        } else {
            bit_clr(task, S_BUS_0_SHORT);
        }
        if ( box->Flag_prtc1 & 0x08 ) {
            len += sprintf(&errstr[len], "[%d: 母线1过压] ", ++errnr);
            bit_set(task, S_BUS_1_VHI);
        } else {
            bit_clr(task, S_BUS_1_VHI);
        }
        if ( box->Flag_prtc1 & 0x10 ) {
            len += sprintf(&errstr[len], "[%d: 母线1欠压] ", ++errnr);
            bit_set(task, S_BUS_1_VLO);
        } else {
            bit_clr(task, S_BUS_1_VLO);
        }
        if ( box->Flag_prtc1 & 0x20 ) {
            len += sprintf(&errstr[len], "[%d: 母线1短路] ", ++errnr);
            bit_set(task, S_BUS_1_SHORT);
        } else {
            bit_clr(task, S_BUS_1_SHORT);
        }

        if ( box->Flag_prtc2 & 0x01 ) {
            len += sprintf(&errstr[len], "[%d: 电池0过压] ", ++errnr);
            bit_set(task, S_BAT_0_VHI);
        } else {
            bit_clr(task, S_BAT_0_VHI);
        }
        if ( box->Flag_prtc2 & 0x02 ) {
            len += sprintf(&errstr[len], "[%d: 电池0欠压] ", ++errnr);
            bit_set(task, S_BAT_0_VLO);
        } else {
            bit_clr(task, S_BAT_0_VLO);
        }
        if ( box->Flag_prtc2 & 0x04 ) {
            len += sprintf(&errstr[len], "[%d: 电池0链接短路] ", ++errnr);
            bit_set(task, S_BAT_0_SHORT);
        } else {
            bit_clr(task, S_BAT_0_SHORT);
        }
        if ( box->Flag_prtc2 & 0x08 ) {
            len += sprintf(&errstr[len], "[%d: 电池0反接] ", ++errnr);
            bit_set(task, S_BAT_0_REVERT);
        } else {
            bit_clr(task, S_BAT_0_REVERT);
        }
        if ( box->Flag_prtc2 & 0x10 ) {
            len += sprintf(&errstr[len], "[%d: 电池0绝缘接地] ", ++errnr);
            bit_set(task, S_BAT_0_INSTITUDE);
        } else {
            bit_clr(task, S_BAT_0_INSTITUDE);
        }
        if ( box->Flag_prtc2 & 0x20 ) {
            len += sprintf(&errstr[len], "[%d: 电池0过流] ", ++errnr);
            bit_set(task, S_BAT_0_IHI);
        } else {
            bit_clr(task, S_BAT_0_IHI);
        }
        if ( box->Flag_prtc3 & 0x01 ) {
            len += sprintf(&errstr[len], "[%d: 电池1过压] ", ++errnr);
            bit_set(task, S_BAT_1_VHI);
        } else {
            bit_clr(task, S_BAT_1_VHI);
        }
        if ( box->Flag_prtc3 & 0x02 ) {
            len += sprintf(&errstr[len], "[%d: 电池1欠压] ", ++errnr);
            bit_set(task, S_BAT_1_VLO);
        } else {
            bit_clr(task, S_BAT_1_VLO);
        }
        if ( box->Flag_prtc3 & 0x04 ) {
            len += sprintf(&errstr[len], "[%d: 电池1链接短路] ", ++errnr);
            bit_set(task, S_BAT_1_SHORT);
        } else {
            bit_clr(task, S_BAT_1_SHORT);
        }
        if ( box->Flag_prtc3 & 0x08 ) {
            len += sprintf(&errstr[len], "[%d: 电池1反接] ", ++errnr);
            bit_set(task, S_BAT_1_REVERT);
        } else {
            bit_clr(task, S_BAT_1_REVERT);
        }
        if ( box->Flag_prtc3 & 0x10 ) {
            len += sprintf(&errstr[len], "[%d: 电池1绝缘接地] ", ++errnr);
            bit_set(task, S_BAT_1_INSTITUDE);
        } else {
            bit_clr(task, S_BAT_1_INSTITUDE);
        }
        if ( box->Flag_prtc3 & 0x20 ) {
            len += sprintf(&errstr[len], "[%d: 电池1过流] ", ++errnr);
            bit_set(task, S_BAT_1_IHI);
        } else {
            bit_clr(task, S_BAT_1_IHI);
        }

        if ( box->Flag_prtc4 & 0x01 ) {
            len += sprintf(&errstr[len], "[%d: 辅助电源故障] ", ++errnr);
            bit_set(task, S_ASSIT_PWN_ERR);
        } else {
            bit_clr(task, S_ASSIT_PWN_ERR);
        }
        if ( ((box->Flag_prtc4 & (0x02|0x04)) >> 1) == 1 ) {
            len += sprintf(&errstr[len], "[%d: 温度过高] ", ++errnr);
            bit_set(task, S_TEMP_HI);
        } else if ( ((box->Flag_prtc4 & (0x02|0x04)) >> 1) == 2 ) {
            len += sprintf(&errstr[len], "[%d: 温度过低] ", ++errnr);
            bit_set(task, S_TEMP_LO);
        } else {
            bit_clr(task, S_TEMP_HI);
            bit_clr(task, S_TEMP_LO);
        }
        if ( ((box->Flag_prtc4 & (0x02|0x04)) >> 3) == 1 ) {
            len = sprintf(&errstr[len], "[%d: 湿度过高] ", ++errnr);
            bit_set(task, S_WET_HI);
        } else if ( ((box->Flag_prtc4 & (0x02|0x04)) >> 3) == 2 ) {
            len += sprintf(&errstr[len], "[%d: 湿度过低] ", ++errnr);
            bit_set(task, S_WET_LO);
        } else {
            bit_clr(task, S_WET_HI);
            bit_clr(task, S_WET_LO);
        }

        if ( box->Flag_prtc6 & 0x01 ) {
            len += sprintf(&errstr[len], "[%d: 总输出熔断器熔断] ", ++errnr);
            bit_set(task, S_DC_OUTPUT_RD);
        } else {
            bit_clr(task, S_DC_OUTPUT_RD);
        }
        if ( box->Flag_prtc6 & 0x02 ) {
            len += sprintf(&errstr[len], "[%d: 总输出跳闸] ", ++errnr);
            bit_set(task, S_DC_OUTPUT_TRIP);
        } else {
            bit_clr(task, S_DC_OUTPUT_TRIP);
        }
        if ( box->Flag_prtc6 & 0x04 ) {
            len += sprintf(&errstr[len], "[%d: 一路输出跳闸] ", ++errnr);
            bit_set(task, S_DC_OUTPUT_0_TRIP);
        } else {
            bit_clr(task, S_DC_OUTPUT_0_TRIP);
        }
        if ( box->Flag_prtc6 & 0x08 ) {
            len += sprintf(&errstr[len], "[%d: 二路输出跳闸] ", ++errnr);
            bit_set(task, S_DC_OUTPUT_1_TRIP);
        } else {
            bit_clr(task, S_DC_OUTPUT_1_TRIP);
        }
        if ( box->Flag_prtc6 & 0x10 ) {
            len += sprintf(&errstr[len], "[%d: 防雷器故障] ", ++errnr);
            bit_set(task, S_FLQ_ERROR);
        } else {
            bit_clr(task, S_FLQ_ERROR);
        }
        if ( box->Flag_prtc6 & 0x20 ) {
            len += sprintf(&errstr[len], "[%d: 交流输入跳闸] ", ++errnr);
            bit_set(task, S_AC_SWITCH_TRIP);
        } else {
            bit_clr(task, S_AC_SWITCH_TRIP);
        }
        if ( box->Flag_prtc6 & 0x40 ) {
            len += sprintf(&errstr[len], "[%d: 急停] ", ++errnr);
            bit_set(task, S_HALT);
        } else {
            bit_clr(task, S_HALT);
        }

        if ( errnr ) {
            log_printf(DBG_LV3, "Fault: "RED("%s"), errstr);
        }
        // 输入状态，遥信
        len = 0;
        if ( box->Flag_run1 & 0x01 ) {
            if ( ! (me_pre->Flag_run1 & 0x01) ) {
                log_printf(INF, "采样盒: 交流开关合闸.");
            }
            len += sprintf(&infstr[len], "[交流"GRN("合闸")"] ");
        } else {
            if ( me_pre->Flag_run1 & 0x01 ) {
                log_printf(INF, "采样盒: 交流开关分闸.");
            }
            len += sprintf(&infstr[len], "[交流"RED("分闸")"] ");
        }
        if ( box->Flag_run1 & 0x02 ) {
            if ( ! (me_pre->Flag_run1 & 0x02) ) {
                log_printf(INF, "采样盒: 开始加热.");
            }
            len += sprintf(&infstr[len], "[加热] ");
        } else {
            if ( me_pre->Flag_run1 & 0x02 ) {
                log_printf(INF, "采样盒: 停止加热.");
            }
            len += sprintf(&infstr[len], "[未加热] ");
        }
        if ( box->Flag_run1 & 0x04 ) {
            if ( ! (me_pre->Flag_run1 & 0x04) ) {
                log_printf(INF, "采样盒: 开始通风.");
            }
            len += sprintf(&infstr[len], "[通风] ");
        } else {
            if ( me_pre->Flag_run1 & 0x04 ) {
                log_printf(INF, "采样盒: 停止通风.");
            }
            len += sprintf(&infstr[len], "[未通风] ");
        }
        if ( box->Flag_run1 & 0x08 ) {
            if ( ! (me_pre->Flag_run1 & 0x08) ) {
                log_printf(INF, "采样盒: 总输出"GRN("合闸"));
            }
            bit_set(task, F_DC_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[总输出"GRN("合闸")"] ");
        } else {
            if ( me_pre->Flag_run1 & 0x08 ) {
                log_printf(INF, "采样盒: 总输出"RED("分闸"));
            }
            bit_clr(task, F_DC_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[总输出"RED("分闸")"] ");
        }
        if ( box->Flag_run1 & 0x10 ) {
            if ( ! (me_pre->Flag_run1 & 0x10) ) {
                log_printf(INF, "采样盒: 1#枪输出"GRN("合闸")".");
            }
            bit_set(task, F_GUN_1_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[1#枪输出"GRN("合闸")"] ");
        } else {
            if ( me_pre->Flag_run1 & 0x10 ) {
                log_printf(INF, "采样盒: 1#枪输出"RED("分闸"));
            }
            bit_clr(task, F_GUN_1_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[1#枪输出"RED("分闸")"] ");
        }
        if ( ((box->Flag_run1 & (0x20 | 0x40)) >> 5) == 0 ) {
            if ( ((me_pre->Flag_run1 & (0x20 | 0x40)) >> 5) != 0 ) {
                log_printf(INF, "采样盒: 1#枪断开连接");
            }
            bit_clr(task, F_GUN_1_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[1#枪未链接] ");
        } else if (((box->Flag_run1 & (0x20 | 0x40)) >> 5) == GUN_CONN_PROTECTED ) {
            bit_clr(task, F_GUN_1_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[1#枪链接保护] ");
        } else if ( ((box->Flag_run1 & (0x20 | 0x40)) >> 5) == GUN_CONN_EXCEPTION ) {
            bit_clr(task, F_GUN_1_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[1#枪连接异常] ");
        } else if ( ((box->Flag_run1 & (0x20 | 0x40)) >> 5) == GUN_CONN_CONNECTIVE ) {
            if ( ((me_pre->Flag_run1 & (0x20 | 0x40)) >> 5) != GUN_CONN_CONNECTIVE ) {
                log_printf(INF, "采样盒: 1#枪连接完成.");
            }
            bit_set(task, F_GUN_1_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[1#枪链接"GRN("正常")"] ");
        }
        if ( box->Flag_run1 & 0x80 ) {
            if ( !(me_pre->Flag_run1 & 0x80) ) {
                log_printf(INF, "采样盒: 1#枪辅助电源合闸.");
            }
            bit_set(task, F_GUN_1_ASSIT_PWN_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[1#枪辅助电源"GRN("合闸")"] ");
        } else {
            if ( me_pre->Flag_run1 & 0x80 ) {
                log_printf(INF, "采样盒: 1#枪辅助电源分闸.");
            }
            bit_clr(task, F_GUN_1_ASSIT_PWN_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[1#枪辅助电源"RED("分闸")"] ");
        }
        if ( box->Flag_run2 & 0x01 ) {
            if ( !(me_pre->Flag_run2 & 0x01) ) {
                log_printf(INF, "采样盒: 2#枪输出"GRN("合闸")"");
            }
            bit_set(task, F_GUN_2_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[2#枪输出"GRN("合闸")"] ");
        } else {
            if ( me_pre->Flag_run2 & 0x01 ) {
                log_printf(INF, "采样盒: 2#枪输出"RED("分闸")"");
            }
            bit_clr(task, F_GUN_2_OUTPUT_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[2#枪输出"RED("分闸")"] ");
        }
        if ( (box->Flag_run2 & (0x02|0x04)) == 0 ) {
            if ( (me_pre->Flag_run2 & (0x02|0x04)) != 0 ) {
                log_printf(INF, "采样盒: 2#枪断开连接");
            }
            bit_clr(task, F_GUN_2_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[2#枪未链接] ");
        } else if ((box->Flag_run2 & (0x02|0x04)) >> 1 == GUN_CONN_PROTECTED ) {
            bit_clr(task, F_GUN_2_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[2#枪链接保护] ");
        } else if ( (box->Flag_run2 & (0x02|0x04)) >> 1 == GUN_CONN_EXCEPTION ) {
            bit_clr(task, F_GUN_2_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[2#枪连接异常] ");
        } else if ( (box->Flag_run2 & (0x02|0x04)) >> 1 == GUN_CONN_CONNECTIVE ) {
            if ( (me_pre->Flag_run2 & (0x02|0x04)) >> 1 != GUN_CONN_CONNECTIVE ) {
                log_printf(INF, "采样盒: 2#枪连接完成.");
            }
            bit_set(task, F_GUN_2_PHY_CONN_STATUS);
            len += sprintf(&infstr[len], "[2#枪链接"GRN("正常")"] ");
        }
        if ( box->Flag_run2 & 0x08 ) {
            if ( !(me_pre->Flag_run2 & 0x08) ) {
                log_printf(INF, "采样盒: 2#枪辅助电源"GRN("合闸"));
            }
            bit_set(task, F_GUN_2_ASSIT_PWN_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[2#枪辅助电源"GRN("合闸")"] ");
        } else {
            if ( me_pre->Flag_run2 & 0x08 ) {
                log_printf(INF, "采样盒: 2#枪辅助电源"RED("分闸"));
            }
            bit_clr(task, F_GUN_2_ASSIT_PWN_SWITCH_STATUS);
            len += sprintf(&infstr[len], "[2#枪辅助电源"RED("分闸")"] ");
        }
        log_printf(DBG_LV3, "采样盒: 遥信: %s", infstr);

        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        param->attrib = BP_FRAME_UNSTABLE;

        buff[ nr ++ ] = 0xF0;
        buff[ nr ++ ] = 0xE1;
        buff[ nr ++ ] = 0xD2;
        buff[ nr ++ ] = 0xC3;
        buff[ nr ++ ] = 0x05;
        buff[ nr ++ ] = 0x03;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x20;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x14;
        len = nr;
        buff[ nr ++ ] = load_crc(len, buff);
        buff[ nr ++ ] = load_crc(len, buff) >> 8;

        memcpy(param->buff.tx_buff, buff, nr);
        param->payload_size = nr;

        self->rx_param.need_bytes = 49;
        self->master->time_to_send = param->payload_size * 1000 / 960 + self->master->swap_time_modify;
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
            if ( ! bit_read(task, S_MEASURE_1_COMM_DOWN) ) {
            }
            log_printf(ERR, "UART: "RED("综合采样盒通信中断, 请排查故障,"
                                        " 已禁止充电(%d)."), self->master->died);
            bit_set(task, S_MEASURE_1_COMM_DOWN);
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

// 采样盒校准输出参考电压
int simple_box_correct_refer_V_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    unsigned char buff[32];
    int nr = 0, len;

    int ret = ERR_ERR;
    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        ret = ERR_OK;
        break;
    // 串口接收到新数据
    case BP_EVT_RX_DATA:
        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        buff[nr ++] = 0xFF;
        buff[nr ++] = 0x10;
        buff[nr ++] = 0x00;
        buff[nr ++] = 0x00;
        buff[nr ++] = 0x00;
        buff[nr ++] = 0x09;
        buff[nr ++] = 0x12;

        buff[nr ++] = (unsigned short)((10 * (task->max_output_I))) >> 8;
        buff[nr ++] = (unsigned short)((10 * (task->max_output_I))) & 0xFF;
        buff[nr ++] = (unsigned short)((10 * (task->limit_output_I))) >> 8;
        buff[nr ++] = (unsigned short)((10 * (task->limit_output_I))) & 0xFF;
        buff[nr ++] = (unsigned short)((10 * (task->limit_max_V))) >> 8;
        buff[nr ++] = (unsigned short)((10 * (task->limit_max_V))) & 0xFF;
        buff[nr ++] = (unsigned short)((10 * (task->limit_min_V))) >> 8;
        buff[nr ++] = (unsigned short)((10 * (task->limit_min_V))) & 0xFF;
        if ( task->bus1_correct_V > 0 && task->bus1_correct_V <= 9999.9 ) {
            buff[nr ++] = double2short(task->bus1_correct_V, 1) >> 8;
            buff[nr ++] = double2short(task->bus1_correct_V, 1) & 0xFF;
            buff[nr ++] = double2short(task->bus1_correct_V, 1) >> 8;
            buff[nr ++] = double2short(task->bus1_correct_V, 1) & 0xFF;
        } else if ( task->bus2_correct_V > 0 && task->bus2_correct_V <= 9999.9 ) {
            buff[nr ++] = double2short(task->bus2_correct_V, 1) >> 8;
            buff[nr ++] = double2short(task->bus2_correct_V, 1) & 0xFF;
            buff[nr ++] = double2short(task->bus2_correct_V, 1) >> 8;
            buff[nr ++] = double2short(task->bus2_correct_V, 1) & 0xFF;
        } else {
            buff[nr ++] = 4000 >> 8;
            buff[nr ++] = 4000 & 0xFF;
            buff[nr ++] = 4000 >> 8;
            buff[nr ++] = 4000 & 0xFF;
        }

        if ( task->bus_correct_I > 0 && task->bus_correct_I <= 999.9 ) {
            buff[nr ++] = double2short(task->bus_correct_I, 1) >> 8;
            buff[nr ++] = double2short(task->bus_correct_I, 1) & 0xFF;
        } else {
            buff[nr ++] = 0;
            buff[nr ++] = 0;
        }
        buff[nr ++] = task->modules_nr >> 8;
        buff[nr ++] = task->modules_nr & 0xFF;
        buff[nr ++] = task->charge_stat >> 8;
        buff[nr ++] = task->charge_stat & 0xFF;

        self->rx_param.need_bytes = 0;

        len = nr;
        buff[ nr ++ ] = load_crc(len, buff);
        buff[ nr ++ ] = load_crc(len, buff) >> 8;

        memcpy(param->buff.tx_buff, buff, nr);
        param->payload_size = nr;

        self->master->time_to_send = param->payload_size * 1000 / 960 /*+ self->master->swap_time_modify*/;

        ret = ERR_OK;
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

// 一体机直接读取转换盒通信
int ANC01_convert_box_read_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    unsigned char buff[32];
    int nr = 0, len;
    int i, j;
    unsigned short kn, on =0, off = 0;

    int ret = ERR_ERR;
    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size < param->need_bytes ) {
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
        } else {
            unsigned short crc = load_crc(param->need_bytes-2, param->buff.rx_buff);
            unsigned short check = param->buff.rx_buff[ param->need_bytes - 2 ] |
                    param->buff.rx_buff[ param->need_bytes - 1] << 8;
            log_printf(WRN, "UART: CRC cheke result: need: %04X, gave: %04X",
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
        if ( bit_read(task, S_CONVERT_BOX_COMM_DOWN) ) {
            log_printf(INF, "UART: "GRN("协议转换盒通信恢复."));
        }
        bit_clr(task, S_CONVERT_BOX_COMM_DOWN);

        memcpy(me->chargers->chargers.charge_module_v, &param->buff.rx_buff[3], 91 * 2);
        do {

            for ( i = 0; i < CONFIG_SUPPORT_CHARGE_MODULE; i ++ ) {
                me->chargers->chargers.charge_module_v[i] =
                        b2l(me->chargers->chargers.charge_module_v[i]);
                me->chargers->chargers.charge_module_i[i] =
                        b2l(me->chargers->chargers.charge_module_i[i]);
                me->chargers->chargers.charge_module_t[i] =
                        b2l(me->chargers->chargers.charge_module_t[i]);
                for ( j = 0; j < 3; j ++ ) {
                    me->chargers->chargers.charge_module_sn[i][j] =
                            b2l(me->chargers->chargers.charge_module_sn[i][j]);
                }
            }
            for ( i = 0; i < CONFIG_SUPPORT_CHARGE_MODULE / 2; i ++ ) {
                me->chargers->chargers.charge_module_status[i] =
                        b2l(me->chargers->chargers.charge_module_status[i]);
            }
        } while (0);

        for (i = 0; i < CONFIG_SUPPORT_CHARGE_MODULE && i < task->modules_nr; i ++ ) {
            // 判断模块故障
            if ( param->buff.rx_buff[i + 87] & 0x0F ) {
                bit_set(task, S_CHARGE_M_1_ERR + i);
            } else {
                bit_clr(task, S_CHARGE_M_1_ERR + i);
            }
        }
        for (; i < CONFIG_SUPPORT_CHARGE_MODULE; i ++ ) {
            bit_clr(task, S_CHARGE_M_1_ERR + i);
            task->modules_on_off[i] = 0x00;
        }

        if ( bit_read(task, CMD_MODULE_OFF) || bit_read(task, CMD_MODULE_ON) ) {
            for (i = 0; i < CONFIG_SUPPORT_CHARGE_MODULE && i < task->modules_nr; i ++ ) {
                // 清理模块开关机标志
                kn = b2l(task->chargers[0]->chargers.charge_module_status[i/2]);
                if ( i % 2 ) {
                    kn = kn >> 8;
                } else {
                    kn = kn & 0xFF;
                }
                if ( task->modules_on_off[i] == 0x80 && !(kn>>4) ) {
                    log_printf(INF, "UART: 模块%d已经开机", i + 1);
                    task->modules_on_off[i] = 0x00;
                }
                if ( task->modules_on_off[i] == 0x81 && (kn>>4) ) {
                    log_printf(INF, "UART: 模块%d已经关机", i + 1);
                    task->modules_on_off[i] = 0x00;
                }
            }
            for (i = 0; i < CONFIG_SUPPORT_CHARGE_MODULE && i < task->modules_nr; i ++ ) {
                if ( task->modules_on_off[i] == 0x80 ) {
                    on ++;
                } else if ( task->modules_on_off[i] == 0x81 ) {
                    off ++;
                } else {
                }
            }
            if ( on ) {
                bit_set(task, CMD_MODULE_ON);
            } else {
                bit_clr(task, CMD_MODULE_ON);
            }
            if ( off ) {
                bit_set(task, CMD_MODULE_OFF);
            } else {
                bit_set(task, CMD_MODULE_OFF);
            }
        }
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        buff[nr ++] = 0xFF;
        buff[nr ++] = 0x04;
        buff[nr ++] = 0x03;
        buff[nr ++] = 0xe8;
        buff[nr ++] = 0x00;
        buff[nr ++] = 0x5b;
        len = nr;
        buff[ nr ++ ] = load_crc(len, buff);
        buff[ nr ++ ] = load_crc(len, buff) >> 8;

        memcpy(param->buff.tx_buff, buff, nr);
        self->rx_param.need_bytes = 187;
        param->payload_size = nr;
        self->master->time_to_send = (param->payload_size + 0) * 1000 / 960 /*+ self->master->swap_time_modify*/;
        ret = ERR_OK;
        log_printf(DBG_LV3, "UART: %s requested.", __FUNCTION__);
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
        } else {
            if ( ! bit_read(task, S_CONVERT_BOX_COMM_DOWN) ) {
                log_printf(ERR, "UART: "RED("协议转换盒通信中断, 请排查故障,(%d:%d)"),
                           self->master->died,
                           self->master->swap_time_modify);
            }
            bit_set(task, S_CONVERT_BOX_COMM_DOWN);
        }
        log_printf(WRN, "UART: %s:%d get signal TIMEOUT", __FUNCTION__, evt);
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

// 配置转换盒数据
int ANC01_convert_box_write_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    unsigned char buff[32];
    int nr = 0, len;

    int ret = ERR_ERR;
    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        ret = ERR_OK;
        break;
    // 串口接收到新数据
    case BP_EVT_RX_DATA:
        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        buff[nr ++] = 0xFF;
        buff[nr ++] = 0x10;
        buff[nr ++] = 0x00;
        buff[nr ++] = 0x00;
        buff[nr ++] = 0x00;
        buff[nr ++] = 0x09;
        buff[nr ++] = 0x12;

        // 额定电流值
        switch ( task->module_model ) {
        case MODEL_AN10680:
        default:
            buff[nr ++] = 0x00;
            buff[nr ++] = 0x6E;
            break;
        }

        // 需求电流值
        buff[nr ++] = ((unsigned short)(((atof(config_read("需求电流")))))) >> 8;
        buff[nr ++] = ((unsigned short)(((atof(config_read("需求电流")))))) & 0xFF;

        // 需求电压值
        buff[nr ++] = (unsigned int)atoi(config_read("需求电压")) >> 8;
        buff[nr ++] = (unsigned int)atoi(config_read("需求电压")) & 0xFF;

        log_printf(DBG_LV3, "UART.ANC-01, 需求电流: %.1f A. 需求电压: %.1f V",
                   atof(config_read("需求电流")),
                   atof(config_read("需求电压"))/10.0f);

        // 电压下限值
        buff[nr ++] = (unsigned short)((10 * (task->limit_min_V))) >> 8;
        buff[nr ++] = (unsigned short)((10 * (task->limit_min_V))) & 0xFF;

        // 目标电压值
        buff[nr ++] = 0x12;
        buff[nr ++] = 0xC0;

        // 初始电压
        buff[nr ++] = 0;
        buff[nr ++] = 0;

        // 当前电流
        buff[nr ++] = 0;
        buff[nr ++] = 0;

        // 模块个数
        buff[nr ++] = (unsigned short)(task->modules_nr) >> 8;
        buff[nr ++] = (unsigned short)(task->modules_nr) & 0xFF;

        // 充电状态
        buff[nr ++] = 0;
        buff[nr ++] = (bit_read(task, CMD_GUN_1_OUTPUT_ON)||bit_read(task, CMD_GUN_2_OUTPUT_ON))?1:0;

        self->rx_param.need_bytes = 0;
        len = nr;
        buff[ nr ++ ] = load_crc(len, buff);
        buff[ nr ++ ] = load_crc(len, buff) >> 8;

        memcpy(param->buff.tx_buff, buff, nr);
        param->payload_size = nr;

        self->master->time_to_send = param->payload_size * 1000 / 960 /*+ self->master->swap_time_modify*/;
        ret = ERR_OK;
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
        log_printf(WRN, "UART: %s:d get signal TIMEOUT", __FUNCTION__, evt);
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

// 配置转换盒数据-模块开机
int ANC01_convert_box_module_on_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    unsigned char buff[32];
    int nr = 0, len, i, ok = 0;
    static int on_seq = 0;

    int ret = ERR_ERR;
    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        ret = ERR_OK;
        break;
    // 串口接收到新数据
    case BP_EVT_RX_DATA:
        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        if ( bit_read(task, CMD_MODULE_ON ) ) {
            for (i = 0; i < CONFIG_SUPPORT_CHARGE_MODULE && i < task->modules_nr; i ++) {
                if ( task->modules_on_off[ on_seq ] == 0x80 ) {
                    ok = 1;
                    break;
                }
                if ( ++ on_seq >= task->modules_nr ) on_seq = 0;
            }
            if ( ok == 0 ) {
                // 没有要开机的模块
                break;
            }
            buff[nr ++] = 0xFF;
            buff[nr ++] = 0x06;
            buff[nr ++] = 0x00;
            buff[nr ++] = 0x64;
            buff[nr ++] = (on_seq + 1) >> 8;
            buff[nr ++] = (on_seq + 1) & 0xFF;
            self->rx_param.need_bytes = 0;
            on_seq ++;
        } else {
            break;
        }
        len = nr;
        buff[ nr ++ ] = load_crc(len, buff);
        buff[ nr ++ ] = load_crc(len, buff) >> 8;

        memcpy(param->buff.tx_buff, buff, nr);
        param->payload_size = nr;

        self->master->time_to_send = param->payload_size * 1000 / 960 /*+ self->master->swap_time_modify*/;

        ret = ERR_OK;
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
        log_printf(WRN, "UART: %s:d get signal TIMEOUT", __FUNCTION__, evt);
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

// 配置转换盒数据-模块关机
int ANC01_convert_box_module_off_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    unsigned char buff[32];
    int nr = 0, len, i, ok = 0;
    static int off_seq = 0;

    int ret = ERR_ERR;
    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        ret = ERR_OK;
        break;
    // 串口接收到新数据
    case BP_EVT_RX_DATA:
        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        if ( bit_read(task, CMD_MODULE_OFF) ) {
            for (i = 0; i < CONFIG_SUPPORT_CHARGE_MODULE && i < task->modules_nr; i ++) {
                if ( task->modules_on_off[ off_seq ] == 0x81 ) {
                    ok = 1;
                    break;
                }
                if ( ++ off_seq >= task->modules_nr ) off_seq = 0;
            }
            if ( ok == 0 ) {
                // 没有要开机的模块
                break;
            }
            buff[nr ++] = 0xFF;
            buff[nr ++] = 0x06;
            buff[nr ++] = 0x00;
            buff[nr ++] = 0x65;
            buff[nr ++] = (off_seq + 1) >> 8;
            buff[nr ++] = (off_seq + 1) & 0xFF;
            self->rx_param.need_bytes = 0;
        } else {
            break;
        }
        len = nr;
        buff[ nr ++ ] = load_crc(len, buff);
        buff[ nr ++ ] = load_crc(len, buff) >> 8;

        memcpy(param->buff.tx_buff, buff, nr);
        param->payload_size = nr;

        self->master->time_to_send = param->payload_size * 1000 / 960 /*+ self->master->swap_time_modify*/;

        ret = ERR_OK;
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        ret = ERR_OK;
        break;
    // 串口数据发送完成事件
    case BP_EVT_TX_FRAME_DONE:
        break;
    // 串口接收单个字节超时，出现在接收帧的第一个字节
    case BP_EVT_RX_BYTE_TIMEOUT:
    // 串口接收帧超时, 接受的数据不完整
    case BP_EVT_RX_FRAME_TIMEOUT:
        log_printf(WRN, "UART: %s:d get signal TIMEOUT", __FUNCTION__, evt);
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


//CRC码表高字节
unsigned char Increase_gabyCRCHi[] =
{
    0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x01,0xc0,
    0x80,0x41,0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
    0x00,0xc1,0x81,0x40,0x00,0xc1,0x81,0x40,0x01,0xc0,
    0x80,0x41,0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
    0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x00,0xc1,
    0x81,0x40,0x01,0xc0,0x80,0x41,0x01,0xc0,0x80,0x41,
    0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x00,0xc1,
    0x81,0x40,0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
    0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x01,0xc0,
    0x80,0x41,0x00,0xc1,0x81,0x40,0x00,0xc1,0x81,0x40,
    0x01,0xc0,0x80,0x41,0x01,0xc0,0x80,0x41,0x00,0xc1,
    0x81,0x40,0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
    0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x01,0xc0,
    0x80,0x41,0x00,0xc1,0x81,0x40,0x00,0xc1,0x81,0x40,
    0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,0x01,0xc0,
    0x80,0x41,0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,
    0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x01,0xc0,
    0x80,0x41,0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
    0x00,0xc1,0x81,0x40,0x00,0xc1,0x81,0x40,0x01,0xc0,
    0x80,0x41,0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
    0x01,0xc0,0x80,0x41,0x00,0xc1,0x81,0x40,0x01,0xc0,
    0x80,0x41,0x00,0xc1,0x81,0x40,0x00,0xc1,0x81,0x40,
    0x01,0xc0,0x80,0x41,0x01,0xc0,0x80,0x41,0x00,0xc1,
    0x81,0x40,0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,
    0x00,0xc1,0x81,0x40,0x01,0xc0,0x80,0x41,0x01,0xc0,
    0x80,0x41,0x00,0xc1,0x81,0x40
};

//CRC码表高字节
unsigned char	Increase_gabyCRCLo[] =
{
    0x00,0xc0,0xc1,0x01,0xc3,0x03,0x02,0xc2,0xc6,0x06,
    0x07,0xc7,0x05,0xc5,0xc4,0x04,0xcc,0x0c,0x0d,0xcd,
    0x0f,0xcf,0xce,0x0e,0x0a,0xca,0xcb,0x0b,0xc9,0x09,
    0x08,0xc8,0xd8,0x18,0x19,0xd9,0x1b,0xdb,0xda,0x1a,
    0x1e,0xde,0xdf,0x1f,0xdd,0x1d,0x1c,0xdc,0x14,0xd4,
    0xd5,0x15,0xd7,0x17,0x16,0xd6,0xd2,0x12,0x13,0xd3,
    0x11,0xd1,0xd0,0x10,0xf0,0x30,0x31,0xf1,0x33,0xf3,
    0xf2,0x32,0x36,0xf6,0xf7,0x37,0xf5,0x35,0x34,0xf4,
    0x3c,0xfc,0xfd,0x3d,0xff,0x3f,0x3e,0xfe,0xfa,0x3a,
    0x3b,0xfb,0x39,0xf9,0xf8,0x38,0x28,0xe8,0xe9,0x29,
    0xeb,0x2b,0x2a,0xea,0xee,0x2e,0x2f,0xef,0x2d,0xed,
    0xec,0x2c,0xe4,0x24,0x25,0xe5,0x27,0xe7,0xe6,0x26,
    0x22,0xe2,0xe3,0x23,0xe1,0x21,0x20,0xe0,0xa0,0x60,
    0x61,0xa1,0x63,0xa3,0xa2,0x62,0x66,0xa6,0xa7,0x67,
    0xa5,0x65,0x64,0xa4,0x6c,0xac,0xad,0x6d,0xaf,0x6f,
    0x6e,0xae,0xaa,0x6a,0x6b,0xab,0x69,0xa9,0xa8,0x68,
    0x78,0xb8,0xb9,0x79,0xbb,0x7b,0x7a,0xba,0xbe,0x7e,
    0x7f,0xbf,0x7d,0xbd,0xbc,0x7c,0xb4,0x74,0x75,0xb5,
    0x77,0xb7,0xb6,0x76,0x72,0xb2,0xb3,0x73,0xb1,0x71,
    0x70,0xb0,0x50,0x90,0x91,0x51,0x93,0x53,0x52,0x92,
    0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,0x9c,0x5c,
    0x5d,0x9d,0x5f,0x9f,0x9e,0x5e,0x5a,0x9a,0x9b,0x5b,
    0x99,0x59,0x58,0x98,0x88,0x48,0x49,0x89,0x4b,0x8b,
    0x8a,0x4a,0x4e,0x8e,0x8f,0x4f,0x8d,0x4d,0x4c,0x8c,
    0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,0x82,0x42,
    0x43,0x83,0x41,0x81,0x80,0x40
};

unsigned short Increase_ModbusCRC(unsigned char * pData, unsigned char len)
{
    unsigned char byCRCHi = 0xff;
    unsigned char byCRCLo = 0xff;
    unsigned char byIdx;
    unsigned short crc;

    while(len--)
    {
        byIdx = byCRCHi ^* pData++;
        byCRCHi = byCRCLo ^ Increase_gabyCRCHi[byIdx];
        byCRCLo = Increase_gabyCRCLo[byIdx];
    }

    crc = byCRCHi;
    crc <<= 8;
    crc += byCRCLo;
    return crc;
}

// 英可瑞模块协议转换盒读取
// id: M000000A
int Increase_module_read_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret = ERR_ERR;
    unsigned char buff[32];
    int nr = 0, len;

    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size < param->need_bytes ) {
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
        } else {
            unsigned short crc = Increase_ModbusCRC(param->buff.rx_buff, param->need_bytes-2);
            unsigned short check = param->buff.rx_buff[ param->need_bytes - 1 ] |
                    param->buff.rx_buff[ param->need_bytes - 2] << 8;
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
        if ( bit_read(task, S_CHARGE_M_1_ERR + (unsigned int)(me->_private)-1) ) {
            log_printf(INF, "UART: "GRN("模块%d通信恢复."), (unsigned char)(unsigned int)(me->_private));
        }
        bit_clr(task, S_CHARGE_M_1_ERR + (unsigned int)(me->_private)-1);
        self->master->died = 0;
        do {
            unsigned int module_sn = (unsigned int)(me->_private) & 0x7FF;
            module_sn --;
            if ( task->chargers[0] ) {
                task->chargers[0]->chargers.charge_module_v[ module_sn ] =
                        param->buff.rx_buff[4] << 8 | param->buff.rx_buff[3];
                task->chargers[0]->chargers.charge_module_i[ module_sn ] =
                        param->buff.rx_buff[6] << 8 | param->buff.rx_buff[5];
                task->chargers[0]->chargers.charge_module_t[ module_sn ] = 200 << 8;
                if ( param->buff.rx_buff[14] & 0x01 ) {
                    if ( task->modules_on_off[module_sn] == 0x81 ) {
                        task->modules_on_off[module_sn] = 0;
                        log_printf(INF, "UART: #%d 模块已关机", module_sn + 1);
                    }
                    __module_set_off(&task->chargers[0]->chargers, module_sn);
                } else {
                    if ( task->modules_on_off[module_sn] == 0x80 ) {
                        task->modules_on_off[module_sn] = 0;
                        log_printf(INF, "UART: #%d 模块已开机", module_sn + 1);
                    }
                    __module_set_on(&task->chargers[0]->chargers, module_sn);
                }
                if ( param->buff.rx_buff[14] & 0x08 ) {
                    __module_set_err(&task->chargers[0]->chargers, module_sn);
                } else {
                    __module_clr_err(&task->chargers[0]->chargers, module_sn);
                }
            }
        } while (0);
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        buff[ nr ++ ] = (unsigned char)(unsigned int)(me->_private);
        buff[ nr ++ ] = 0x03;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x06;
        len = nr;
        buff[ nr ++ ] = Increase_ModbusCRC(buff, len) >> 8;
        buff[ nr ++ ] = Increase_ModbusCRC(buff, len);

        memcpy(param->buff.tx_buff, buff, nr);
        param->payload_size = nr;
        self->master->time_to_send = param->payload_size * 1000 / 960;
        self->rx_param.need_bytes = 17;
        ret = ERR_OK;
        log_printf(DBG_LV3, "UART: %s requested.", __FUNCTION__);
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        ret = ERR_OK;
        break;
    // 串口数据发送完成事件
    case BP_EVT_TX_FRAME_DONE:
        break;
    // 串口接收单个字节超时，出现在接收帧的第一个字节
    case BP_EVT_RX_BYTE_TIMEOUT:
    // 串口接收帧超时, 接受的数据不完整
    case BP_EVT_RX_FRAME_TIMEOUT:
        if ( self->master->died < self->master->died_line ) {
            self->master->died ++;
        } else {
            //self->master->died ++;
            if ( ! bit_read(task, S_CHARGE_M_1_ERR + (unsigned int)(me->_private) -1) ) {
            }
            log_printf(ERR, "UART: "RED("%d#模块通信中断, 请排查故障,(%d)"),
                       me->_private,
                       self->master->died);
            bit_set(task, S_CHARGE_M_1_ERR + (unsigned int)(me->_private) -1 );
        }
        log_printf(WRN, "UART: %s:%d get signal TIMEOUT", __FUNCTION__, evt);
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

// 英可瑞模块写, 开关机
// id: M0000015
int Increase_module_write_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    unsigned char buff[32];
    int nr = 0, len;
    int ret = ERR_ERR;
    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size < param->need_bytes ) {
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
        } else {
            unsigned short crc = Increase_ModbusCRC(param->buff.rx_buff, param->need_bytes-2);
            unsigned short check = param->buff.rx_buff[ param->need_bytes - 1 ] |
                    param->buff.rx_buff[ param->need_bytes - 2] << 8;
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
        buff[ nr ++ ] = (unsigned char)(unsigned int)(me->_private);
#if 1
        buff[ nr ++ ] = 0x06;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x05;
        buff[ nr ++ ] = 0x00;
        if ( task->modules_on_off[ buff[ 0 ] - 1 ] == 0x81 ) {
            buff[ nr ++ ] = 1; // 开机
        } else {
            buff[ nr ++ ] = 0; // 开机
        }
#else
        unsigned int rat = (unsigned int)atoi(config_read("need_I1"));
        buff[ nr ++ ] = 0x06;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x02;
        buff[ nr ++ ] = rat >> 8;
        buff[ nr ++ ] = rat & 0xFF;
#endif
        len = nr;
        buff[ nr ++ ] = Increase_ModbusCRC(buff, len) >> 8;
        buff[ nr ++ ] = Increase_ModbusCRC(buff, len) ;

        memcpy(param->buff.tx_buff, buff, nr);
        param->payload_size = nr;
        self->master->time_to_send = param->payload_size * 1000 / 960;
        self->rx_param.need_bytes = 8;
        log_printf(DBG_LV3, "UART: %s requested.", __FUNCTION__);
        ret = ERR_OK;
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        ret = ERR_OK;
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

// 英可瑞模块协议转换盒设置
// id: 00000009
int Increase_convert_box_write_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    unsigned char buff[32];
    int nr = 0, len;
    int ret = ERR_ERR;
    static unsigned int wrap_V_I = 0;

    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size < param->need_bytes ) {
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
        } else {
            unsigned short crc = Increase_ModbusCRC(param->buff.rx_buff, param->need_bytes-2);
            unsigned short check = param->buff.rx_buff[ param->need_bytes - 1 ] |
                    param->buff.rx_buff[ param->need_bytes - 2] << 8;
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
        wrap_V_I ++;
        if ( wrap_V_I > 4 ) {
            wrap_V_I = 3;
        }

        buff[ nr ++ ] = 0xFF;
        buff[ nr ++ ] = 0x06;

        if ( wrap_V_I < 3 ) {
            // 广播开机
            buff[ nr ++ ] = 0x00;
            buff[ nr ++ ] = 0x05;
            buff[ nr ++ ] = 0x00;
            buff[ nr ++ ] = 0x00;
        } else if ( wrap_V_I % 2 ) {
            // 广播限流值
            buff[ nr ++ ] = 0x00;
            buff[ nr ++ ] = 0x02;
            if ( bit_read(task, CMD_JIAOZHUN_BAT_I ) ) {
                buff[ nr ++ ] = ((unsigned short)task->bus_correct_I) >> 8;
                buff[ nr ++ ] = ((unsigned short)task->bus_correct_I) & 0xFF;
            } else {
#if 0
                double maxI = 0;
                unsigned short rat;
                if ( task->modules_nr == 0 || task->measure[0]->measure.VinBAT0 < 2000 ) {
                    buff[ nr ++ ] = 0;
                    buff[ nr ++ ] = 0;
                } else {
                    unsigned int needI = (unsigned int)atof(config_read("需求电流"));
                    needI = needI / task->modules_nr;
                    maxI = 7500 / task->measure[0]->measure.VinBAT0;
                    rat = needI * 1000 / maxI;

                    if ( rat >= 1000 ) {
                        rat = 1000;
                    }
                    if ( rat <= 10 ) {
                        rat = 10;
                    }

                    log_printf(DBG_LV3, "UART.NEED_I:MAXI: %.1f A, needI: %.1f A, %.1f %% %.1f A",
                               maxI, needI,
                               rat / 10.0, maxI * rat/1000.0);

                    buff[ nr ++ ] = rat >> 8;
                    buff[ nr ++ ] = rat & 0xFF;
                }
#endif
                unsigned int rat = (unsigned int)atoi(config_read("need_I1"));
                log_printf(INF, "UART.Increa: rat: %d", rat);

                buff[ nr ++ ] = rat >> 8;
                buff[ nr ++ ] = rat & 0xFF;
            }
        } else {
            // 广播限压值
            buff[ nr ++ ] = 0x00;
            buff[ nr ++ ] = 0x00;
            if ( bit_read(task, CMD_JIAOZHUN_BUS1_V ) ) {
                buff[ nr ++ ] = ((unsigned short)task->bus1_correct_V) >> 8;
                buff[ nr ++ ] = ((unsigned short)task->bus1_correct_V) & 0xFF;
            } else if ( bit_read(task, CMD_JIAOZHUN_BUS2_V ) ) {
                buff[ nr ++ ] = ((unsigned short)task->bus2_correct_V) >> 8;
                buff[ nr ++ ] = ((unsigned short)task->bus2_correct_V) & 0xFF;
            } else {
                buff[ nr ++ ] = (unsigned short)atoi(config_read("需求电压")) >> 8;
                buff[ nr ++ ] = (unsigned short)atoi(config_read("需求电压")) & 0xFF;
            }
        }

        len = nr;
        buff[ nr ++ ] = Increase_ModbusCRC(buff, len) >> 8;
        buff[ nr ++ ] = Increase_ModbusCRC(buff, len) ;

        memcpy(param->buff.tx_buff, buff, nr);
        param->payload_size = nr;
        self->master->time_to_send = param->payload_size * 1000 / 960;
        self->rx_param.need_bytes = 0;
        log_printf(DBG_LV3, "UART: %s requested.", __FUNCTION__);
        ret = ERR_OK;
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        ret = ERR_OK;
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

// 从电表读取电能数据
int kwh_meter_read_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    unsigned char buff[32];
    int nr = 0, l;
    int ret = ERR_ERR;
    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size < param->need_bytes ) {
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
        } else {
            unsigned char sum = check_sum(param->buff.rx_buff, param->need_bytes-2);
            unsigned char check = param->buff.rx_buff[ param->need_bytes - 2 ];
            log_printf(DBG_LV3, "UART: CRC cheke result: need: %02X, gave: %02X",
                       sum, check);
            if ( sum != check ) {
                ret = ERR_FRAME_CHECK_ERR;
            } else {
                int hwh_zong, hwh_jian, hwh_feng, hwh_ping, hwh_gu, i;

                for ( i = 0; i < 20; i ++ ) {
                    param->buff.rx_buff[14 + i] -= 0x33;
                }

                hwh_zong = (param->buff.rx_buff[14] >> 4 ) * 10 + (param->buff.rx_buff[14] & 0x0F );
                hwh_zong += (param->buff.rx_buff[15] >> 4 ) * 1000 + (param->buff.rx_buff[15] & 0x0F ) * 100;
                hwh_zong += (param->buff.rx_buff[16] >> 4 ) * 100000 + (param->buff.rx_buff[16] & 0x0F ) * 10000;
                hwh_zong += (param->buff.rx_buff[17] >> 4 ) * 10000000 + (param->buff.rx_buff[17] & 0x0F ) * 1000000;

                hwh_jian = (param->buff.rx_buff[18] >> 4 ) * 10 + (param->buff.rx_buff[18] & 0x0F );
                hwh_jian += (param->buff.rx_buff[19] >> 4 ) * 1000 + (param->buff.rx_buff[19] & 0x0F ) * 100;
                hwh_jian += (param->buff.rx_buff[20] >> 4 ) * 100000 + (param->buff.rx_buff[20] & 0x0F ) * 10000;
                hwh_jian += (param->buff.rx_buff[21] >> 4 ) * 10000000 + (param->buff.rx_buff[21] & 0x0F ) * 1000000;

                hwh_feng = (param->buff.rx_buff[22] >> 4 ) * 10 + (param->buff.rx_buff[22] & 0x0F );
                hwh_feng += (param->buff.rx_buff[23] >> 4 ) * 1000 + (param->buff.rx_buff[23] & 0x0F ) * 100;
                hwh_feng += (param->buff.rx_buff[24] >> 4 ) * 100000 + (param->buff.rx_buff[24] & 0x0F ) * 10000;
                hwh_feng += (param->buff.rx_buff[25] >> 4 ) * 10000000 + (param->buff.rx_buff[25] & 0x0F ) * 1000000;

                hwh_ping = (param->buff.rx_buff[26] >> 4 ) * 10 + (param->buff.rx_buff[26] & 0x0F );
                hwh_ping += (param->buff.rx_buff[27] >> 4 ) * 1000 + (param->buff.rx_buff[27] & 0x0F ) * 100;
                hwh_ping += (param->buff.rx_buff[28] >> 4 ) * 100000 + (param->buff.rx_buff[28] & 0x0F ) * 10000;
                hwh_ping += (param->buff.rx_buff[29] >> 4 ) * 10000000 + (param->buff.rx_buff[29] & 0x0F ) * 1000000;

                hwh_gu = (param->buff.rx_buff[30] >> 4 ) * 10 + (param->buff.rx_buff[30] & 0x0F );
                hwh_gu += (param->buff.rx_buff[31] >> 4 ) * 1000 + (param->buff.rx_buff[31] & 0x0F ) * 100;
                hwh_gu += (param->buff.rx_buff[32] >> 4 ) * 100000 + (param->buff.rx_buff[32] & 0x0F ) * 10000;
                hwh_gu += (param->buff.rx_buff[33] >> 4 ) * 10000000 + (param->buff.rx_buff[33] & 0x0F ) * 1000000;

                log_printf(DBG_LV3, "UART: %.2fKWH  %.2fKWH  %.2fKWH  %.2fKWH  %.2fKWH",
                           hwh_zong / 100.0f, hwh_jian / 100.0f, hwh_feng / 100.0f
                           , hwh_ping / 100.0f, hwh_gu / 100.0f);
                task->meter[0].kwh_zong = hwh_zong / 100.0f;
                task->meter[0].kwh_jian = hwh_jian / 100.0f;
                task->meter[0].kwh_feng = hwh_feng / 100.0f;
                task->meter[0].kwh_ping = hwh_ping / 100.0f;
                task->meter[0].kwh_gu = hwh_gu / 100.0f;
                ret = ERR_OK;
            }
        }
        break;
    // 串口接收到新数据
    case BP_EVT_RX_DATA:
        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
        if ( bit_read(task, S_KWH_METER_COMM_DOWN) ) {
            log_printf(INF, "UART: "GRN("电表通信恢复(%d)"), self->master->died);
        }
        self->master->died = 0;
        bit_clr(task, S_KWH_METER_COMM_DOWN);
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        buff[ nr ++ ] = 0xFE;
        buff[ nr ++ ] = 0xFE;
        buff[ nr ++ ] = 0xFE;
        buff[ nr ++ ] = 0xFE;
        buff[ nr ++ ] = 0x68;

        // 电表通信地址为 000000000001
        /*
        buff[ nr ++ ] = 0x31;
        buff[ nr ++ ] = 0x17;
        buff[ nr ++ ] = 0x08;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x00;
        */
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[10], task->meter[0].addr[11]);
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[8], task->meter[0].addr[9]);
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[6], task->meter[0].addr[7]);
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[4], task->meter[0].addr[5]);
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[2], task->meter[0].addr[3]);
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[0], task->meter[0].addr[1]);


        buff[ nr ++ ] = 0x68;
        buff[ nr ++ ] = 0x11;
        buff[ nr ++ ] = 0x04;
        buff[ nr ++ ] = 0x00 + 0x33;
        buff[ nr ++ ] = (unsigned char)(0xFF + 0x33);
        buff[ nr ++ ] = 0x00 + 0x33;
        buff[ nr ++ ] = 0x00 + 0x33;

        l = nr;
        buff[ nr ++ ] = check_sum(&buff[4], l - 4);
        buff[ nr ++ ] = 0x16;

        memcpy(param->buff.tx_buff, buff, nr);
        param->payload_size = nr;
        self->master->time_to_send = ( param->payload_size + 1) * 1000 / 218;
        self->rx_param.need_bytes = 36;
        log_printf(DBG_LV3, "UART: %s requested.", __FUNCTION__);
        ret = ERR_OK;
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        ret = ERR_OK;
        break;
    // 串口数据发送完成事件
    case BP_EVT_TX_FRAME_DONE:
        break;
    // 串口接收单个字节超时，出现在接收帧的第一个字节
    case BP_EVT_RX_BYTE_TIMEOUT:
    // 串口接收帧超时, 接受的数据不完整
    case BP_EVT_RX_FRAME_TIMEOUT:
        if ( self->master->died < self->master->died_line ) {
            self->master->died ++;
        } else {
            //self->master->died ++;
            if ( ! bit_read(task, S_KWH_METER_COMM_DOWN) ) {
            }
            log_printf(ERR, "UART: "RED("电表通信中断, 请排查故障,(%d)"),
                       self->master->died);
            bit_set(task, S_KWH_METER_COMM_DOWN);
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

// 从电表读取电压数据
int voltage_meter_read_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    unsigned char buff[32];
    int nr = 0, l;
    int ret = ERR_ERR;
    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size < param->need_bytes ) {
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
        } else {
            unsigned char sum = check_sum(param->buff.rx_buff, param->need_bytes-2);
            unsigned char check = param->buff.rx_buff[ param->need_bytes - 2 ];
            log_printf(DBG_LV2, "UART: SUM check result: need: %02X, gave: %02X",
                       sum, check);
            if ( sum != check ) {
                ret = ERR_FRAME_CHECK_ERR;
            } else {
                int va, vb, vc, i;

                for ( i = 0; i < 6; i ++ ) {
                    param->buff.rx_buff[14 + i] -= 0x33;
                }

                va = (param->buff.rx_buff[15] >> 4 ) * 1000 + (param->buff.rx_buff[15] & 0x0F ) * 100;
                va += (param->buff.rx_buff[14] >> 4 ) * 10 + (param->buff.rx_buff[14] & 0x0F );

                vb = (param->buff.rx_buff[17] >> 4 ) * 1000 + (param->buff.rx_buff[17] & 0x0F ) * 100;
                vb += (param->buff.rx_buff[16] >> 4 ) * 10 + (param->buff.rx_buff[16] & 0x0F );

                vc = (param->buff.rx_buff[19] >> 4 ) * 1000 + (param->buff.rx_buff[19] & 0x0F ) * 100;
                vc += (param->buff.rx_buff[18] >> 4 ) * 10 + (param->buff.rx_buff[18] & 0x0F );

                log_printf(DBG_LV3, "UART: %.2fV  %.2fV  %.2f V",
                           va / 10.0f, vb / 10.0f, vc / 10.0f);

                task->meter[0].Va = va / 10.0f;
                task->meter[0].Vb = vb / 10.0f;
                task->meter[0].Vc = vc / 10.0f;
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
        buff[ nr ++ ] = 0xFE;
        buff[ nr ++ ] = 0xFE;
        buff[ nr ++ ] = 0xFE;
        buff[ nr ++ ] = 0xFE;
        buff[ nr ++ ] = 0x68;

        // 电表通信地址为 000000000001
        /*
        buff[ nr ++ ] = 0x31;
        buff[ nr ++ ] = 0x17;
        buff[ nr ++ ] = 0x08;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x00;
        */
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[10], task->meter[0].addr[11]);
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[8], task->meter[0].addr[9]);
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[6], task->meter[0].addr[7]);
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[4], task->meter[0].addr[5]);
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[2], task->meter[0].addr[3]);
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[0], task->meter[0].addr[1]);

        buff[ nr ++ ] = 0x68;
        buff[ nr ++ ] = 0x11;
        buff[ nr ++ ] = 0x04;
        buff[ nr ++ ] = 0x00 + 0x33;
        buff[ nr ++ ] = (unsigned char)(0xFF + 0x33);
        buff[ nr ++ ] = 0x01 + 0x33;
        buff[ nr ++ ] = 0x02 + 0x33;

        l = nr;
        buff[ nr ++ ] = check_sum(&buff[4], l - 4);
        buff[ nr ++ ] = 0x16;

        memcpy(param->buff.tx_buff, buff, nr);
        param->payload_size = nr;
        self->master->time_to_send = (param->payload_size + 1) * 1000 / 218;
        self->rx_param.need_bytes = 22;
        log_printf(DBG_LV3, "UART: %s requested.", __FUNCTION__);
        ret = ERR_OK;
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        ret = ERR_OK;
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

// 电能表安装过程.
int kwh_meter_install_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    unsigned char buff[32];
    int nr = 0, l;
    int ret = ERR_ERR;
    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size < param->need_bytes ) {
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
        } else {
            unsigned char sum = check_sum(param->buff.rx_buff, param->need_bytes-2);
            unsigned char check = param->buff.rx_buff[ param->need_bytes - 2 ];
            log_printf(DBG_LV2, "UART: SUM check result: need: %02X, gave: %02X",
                       sum, check);
            if ( sum != check ) {
                ret = ERR_FRAME_CHECK_ERR;
            } else {
                int va, vb, vc, i;

                for ( i = 0; i < 6; i ++ ) {
                    param->buff.rx_buff[14 + i] -= 0x33;
                }

                va = (param->buff.rx_buff[15] >> 4 ) * 1000 + (param->buff.rx_buff[15] & 0x0F ) * 100;
                va += (param->buff.rx_buff[14] >> 4 ) * 10 + (param->buff.rx_buff[14] & 0x0F );

                vb = (param->buff.rx_buff[17] >> 4 ) * 1000 + (param->buff.rx_buff[17] & 0x0F ) * 100;
                vb += (param->buff.rx_buff[16] >> 4 ) * 10 + (param->buff.rx_buff[16] & 0x0F );

                vc = (param->buff.rx_buff[19] >> 4 ) * 1000 + (param->buff.rx_buff[19] & 0x0F ) * 100;
                vc += (param->buff.rx_buff[18] >> 4 ) * 10 + (param->buff.rx_buff[18] & 0x0F );

                log_printf(DBG_LV3, "UART: %.2fV  %.2fV  %.2f V",
                           va / 10.0f, vb / 10.0f, vc / 10.0f);

                task->meter[0].Va = va / 10.0f;
                task->meter[0].Vb = vb / 10.0f;
                task->meter[0].Vc = vc / 10.0f;
                ret = ERR_OK;
            }
        }
        break;
    // 串口接收到新数据
        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
    case BP_EVT_RX_DATA:
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        buff[ nr ++ ] = 0xFE;
        buff[ nr ++ ] = 0xFE;
        buff[ nr ++ ] = 0xFE;
        buff[ nr ++ ] = 0xFE;
        buff[ nr ++ ] = 0xFE;
        buff[ nr ++ ] = 0xFE;
        buff[ nr ++ ] = 0x68;

        // 电表通信地址为 000000000001
        /*
        buff[ nr ++ ] = 0x31;
        buff[ nr ++ ] = 0x17;
        buff[ nr ++ ] = 0x08;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x00;
        buff[ nr ++ ] = 0x00;
        */
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[10], task->meter[0].addr[11]);
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[8], task->meter[0].addr[9]);
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[6], task->meter[0].addr[7]);
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[4], task->meter[0].addr[5]);
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[2], task->meter[0].addr[3]);
        buff[ nr ++ ] = __chars2bcd(task->meter[0].addr[0], task->meter[0].addr[1]);

        buff[ nr ++ ] = 0x68;
        buff[ nr ++ ] = 0x11;
        buff[ nr ++ ] = 0x04;
        buff[ nr ++ ] = 0x00 + 0x33;
        buff[ nr ++ ] = (unsigned char)(0xFF + 0x33);
        buff[ nr ++ ] = 0x01 + 0x33;
        buff[ nr ++ ] = 0x02 + 0x33;

        l = nr;
        buff[ nr ++ ] = check_sum(&buff[4], l - 4);
        buff[ nr ++ ] = 0x16;

        memcpy(param->buff.tx_buff, buff, nr);
        param->payload_size = nr;
        self->master->time_to_send = (param->payload_size + 1) * 1000 / 218;
        self->rx_param.need_bytes = 22;
        log_printf(DBG_LV3, "UART: %s requested.", __FUNCTION__);
        ret = ERR_OK;
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        ret = ERR_OK;
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

unsigned char BCC_code(unsigned char *da,size_t len) {
    size_t i  = 0;
    unsigned char BCC = 0;

    for ( ; i < len; i ++ ) {
        BCC ^= da[i];
    }
    return ~BCC;
}

int card_reader_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    static QUERY_STAT query_stat = SEQ_FIND_CARD;
    static struct charge_job *job = NULL;
    static char ID[16], id_len = 0, def_passwd[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    int ret = ERR_ERR;
    unsigned char buff[64];
    int nr = 0, l;

    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size > 1 && param->payload_size >= param->buff.rx_buff[0] ) {
            if ( param->buff.rx_buff[ param->payload_size - 2 ] ==
                 BCC_code(param->buff.rx_buff, param->payload_size - 2) ) {
                return ERR_OK;
            } else {
                log_printf(DBG_LV2, "UART: SUM check result: need: %02X, gave: %02X",
                           BCC_code(param->buff.rx_buff, param->payload_size - 1),
                           param->buff.rx_buff[ param->payload_size - 2 ]);
                return ERR_FRAME_CHECK_ERR;
            }
        } else {
            return ERR_FRAME_CHECK_DATA_TOO_SHORT;
        }
        break;
    // 串口接收到新数据
    case BP_EVT_RX_DATA:
        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
        if ( bit_read(task, S_CARD_READER_COMM_DOWN) ) {
            log_printf(INF, "读卡器通信恢复");
            bit_clr(task, S_CARD_READER_COMM_DOWN);
        }
        switch ( query_stat ) {
        case SEQ_FIND_CARD:
            if ( param->buff.rx_buff[0] <= 8 ) return ERR_OK;
            memcpy(ID, &param->buff.rx_buff[8], param->buff.rx_buff[7]);
            id_len = param->buff.rx_buff[7];
            log_printf(INF, "UART: 寻到卡，ID:(%02X%02X%02X%02X).",
                        ID[3], ID[2], ID[1], ID[0]);
            query_stat = SEQ_SECTOR_RD_AUTH;
            ret = ERR_NEED_ECHO;
            break;
        case SEQ_SECTOR_RD_AUTH:
            if ( param->buff.rx_buff[2] == 0x00 ) {
                // 认证成功
                log_printf(INF, "UART: 读认证成功");
                query_stat = SEQ_READ_PUBLIC_BLK;
                ret = ERR_NEED_ECHO;
            } else {
                // 认证失败
                log_printf(WRN, "UART: 读认证失败");
                query_stat = SEQ_FIND_CARD;
            }
            break;
        case SEQ_SECTOR_WR_AUTH:
            if ( param->buff.rx_buff[2] == 0x00 ) {
                // 认证成功
                log_printf(INF, "UART: 写认证成功");
                query_stat = SEQ_WRITE_PUBLIC_BLK;
                ret = ERR_NEED_ECHO;
            } else {
                // 认证失败
                log_printf(WRN, "UART: 写认证失败, 扣费失败!");
                query_stat = SEQ_FIND_CARD;
            }
            break;
        case SEQ_READ_PUBLIC_BLK:
            if ( param->buff.rx_buff[2] != 0 ) {
                log_printf(WRN, "UART: 读卡器读取数据区失败, 错误码: %d",
                           param->buff.rx_buff[2]);
                query_stat = SEQ_FIND_CARD;
                ret = ERR_OK;
            } else {
                struct user_card cd;

                query_stat = SEQ_FIND_CARD;
                ret = ERR_OK;
                memcpy(cd.card.sector_4.buff, &param->buff.rx_buff[4], 16);
                if ( cd.card.sector_4.data.magic != 0x4F4E5057 ) {
                    log_printf(WRN, "UART: 无法识别的卡 %08X.", cd.card.sector_4.data.magic);
                } else if ( cd.card.sector_4.data.sum !=
                            check_sum(cd.card.sector_4.buff, 15) ) {
                    log_printf(WRN, "UART: 卡数据损坏%02X, 校验失败 %02X.",
                               cd.card.sector_4.data.sum, check_sum(cd.card.sector_4.buff, 15));
                } else {
                    int faile = 0;
                    if ( cd.card.sector_4.data.remain_sum !=
                            check_sum(cd.card.sector_4.data.remain_money, 3) ) {
                        log_printf(WRN, "UART: 卡数据字段： 余额校验失败.");
                        faile ++;
                    }
                    if ( cd.card.sector_4.data.passwd_sum !=
                                check_sum(cd.card.sector_4.data.passwd_code, 3) ) {
                        log_printf(WRN, "UART: 卡数据字段： 密码校验失败.");
                        faile ++;
                    }

                    if ( ! faile ) {
                        unsigned int money = *(unsigned int *)(void*)cd.card.sector_4.data.remain_money;
                        money &= 0x00FFFFFF;
                        log_printf(INF, GRN("UART: 刷卡完成[卡号: %02X%02X%02X%02X, 余额: %.2f]"),
                                   ID[3], ID[2], ID[1], ID[0], money / 100.0f);
                        char buff[32];
                        config_write("card_status", "NORMAL");
                        sprintf(buff, "%.2f", money / 100.0f);
                        config_write("card_remaind_money", buff);
                        sprintf(buff, "%d%d%d%d%d%d",
                                cd.card.sector_4.data.passwd_code[2]>>4,
                                cd.card.sector_4.data.passwd_code[2]&0xF,
                                cd.card.sector_4.data.passwd_code[1]>>4,
                                cd.card.sector_4.data.passwd_code[1]&0xF,
                                cd.card.sector_4.data.passwd_code[0]>>4,
                                cd.card.sector_4.data.passwd_code[0]&0xF);
                        config_write("card_passwd", buff);
                        sprintf(buff, "%02X%02X%02X%02X", ID[3], ID[2], ID[1], ID[0]);

                        if ( task->uipage == UI_PAGE_JOBS ) {
                            job = job_search(task->ui_job_id);
                            if ( job == NULL ) {
                                log_printf(WRN, "无效的刷卡.");
                            } else {
                                if ( 0 == strcmp(job->card.triger_card_sn, buff) ) {
                                    if ( job->charge_job_create_timestamp + 10 > time(NULL) ) {
                                        log_printf(INF, "UART: 刷卡太快，作业保护.");
                                        return ERR_OK;
                                    } else {
                                        log_printf(INF, "任务中止，开始扣费。");
                                        query_stat = SEQ_SECTOR_WR_AUTH;
                                        ret = ERR_NEED_ECHO;
                                    }
                                }
                                config_write("triger_card_sn", buff);
                            }
                        } else {
                            config_write("triger_card_sn", buff);
                            //log_printf(INF, "fasdfafdsfasdfasdfasfasdfsad");
                        }
                    }
                }
            }
            break;
        case SEQ_WRITE_PUBLIC_BLK:
            log_printf(INF, "UART: 刷卡扣费成功");
            query_stat = SEQ_FIND_CARD;
            ret = ERR_OK;
            break;
        default:
            break;
        }
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        switch ( query_stat ) {
        case SEQ_FIND_CARD:
            buff[ nr ++ ] = 0x08;
            buff[ nr ++ ] = 0x02;
            buff[ nr ++ ] = 0x4D;
            buff[ nr ++ ] = 0x02;
            buff[ nr ++ ] = 0x00;
            buff[ nr ++ ] = 0x26;
            l = nr;
            buff[ nr ++ ] = BCC_code(buff, l);
            buff[ nr ++ ] = 0x03;

            memcpy(param->buff.tx_buff, buff, nr);
            param->payload_size = nr;
            self->master->time_to_send = param->payload_size * 1000 / 960;
            self->rx_param.need_bytes = 15;
            log_printf(DBG_LV3, "UART: %s:SEQ_FIND_CARD requested.", __FUNCTION__);
            ret = ERR_OK;
            break;
        case SEQ_SECTOR_WR_AUTH:
        case SEQ_SECTOR_RD_AUTH:
            buff[ nr ++ ] = 0x12;
            buff[ nr ++ ] = 0x02;
            buff[ nr ++ ] = 0x46;
            buff[ nr ++ ] = 0x0C;  // 6字节密钥
            buff[ nr ++ ] = 0x60;  // 密钥A
            buff[ nr ++ ] = ID[0];
            buff[ nr ++ ] = ID[1];
            buff[ nr ++ ] = ID[2];
            buff[ nr ++ ] = ID[3];

            buff[ nr ++ ] = def_passwd[0];
            buff[ nr ++ ] = def_passwd[1];
            buff[ nr ++ ] = def_passwd[2];
            buff[ nr ++ ] = def_passwd[3];
            buff[ nr ++ ] = def_passwd[4];
            buff[ nr ++ ] = def_passwd[5];

            buff[ nr ++ ] = 0x04;  // 默认存放于第四扇区

            l = nr;
            buff[ nr ++ ] = BCC_code(buff, l);
            buff[ nr ++ ] = 0x03;

            memcpy(param->buff.tx_buff, buff, nr);
            param->payload_size = nr;
            self->master->time_to_send = (param->payload_size + 1 ) * 1000 / 960;
            self->rx_param.need_bytes = 7;
            log_printf(DBG_LV3, "UART: %s:SEQ_SECTOR_RD_AUTH requested.", __FUNCTION__);
            ret = ERR_OK;
            break;
        case SEQ_READ_PUBLIC_BLK:
            buff[ nr ++ ] = 0x07;
            buff[ nr ++ ] = 0x02;
            buff[ nr ++ ] = 0x47;
            buff[ nr ++ ] = 0x01;
            buff[ nr ++ ] = 0x04;  // 默认存放于第四扇区
            l = nr;
            buff[ nr ++ ] = BCC_code(buff, l);
            buff[ nr ++ ] = 0x03;

            memcpy(param->buff.tx_buff, buff, nr);
            param->payload_size = nr;
            self->master->time_to_send = param->payload_size * 1000 / 960;
            self->rx_param.need_bytes = 23;
            log_printf(DBG_LV3, "UART: %s:SEQ_READ_PUBLIC_BLK requested.", __FUNCTION__);
            ret = ERR_OK;
            break;
        case SEQ_WRITE_PUBLIC_BLK:

            if ( job == NULL ) {
                log_printf(WRN, "UART: 读卡器时序错乱.");
                query_stat = SEQ_FIND_CARD;
                return ERR_ERR;
            }
            buff[ nr ++ ] = 0x17;
            buff[ nr ++ ] = 0x02;
            buff[ nr ++ ] = 0x48;
            buff[ nr ++ ] = 0x11;
            buff[ nr ++ ] = 0x04; // 写第4扇区

            do {
                int i = 0;
                // 调试每次扣1.5
                __card_write_remain(&job->card, __card_read_remain(&job->card) - 1.5f);
                job->card.card.sector_4.data.sum =
                        check_sum(job->card.card.sector_4.buff, 15);
                for ( i = 0; i < 16; i ++ ) {
                    buff[ nr ++ ] = job->card.card.sector_4.buff[i];
                }
            } while (0);

            l = nr;
            buff[ nr ++ ] = BCC_code(buff, l);
            buff[ nr ++ ] = 0x03;

            memcpy(param->buff.tx_buff, buff, nr);
            param->payload_size = nr;
            self->master->time_to_send = (param->payload_size + 1) * 1000 / 960;
            self->rx_param.need_bytes = 7;
            log_printf(DBG_LV3, "UART: %s:SEQ_WRITE_PUBLIC_BLK requested.", __FUNCTION__);
            ret = ERR_OK;
            break;
        default:
            query_stat = SEQ_FIND_CARD;
            break;
        }
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        ret = ERR_OK;
        break;
    // 串口数据发送完成事件
    case BP_EVT_TX_FRAME_DONE:
        break;
    // 串口接收单个字节超时，出现在接收帧的第一个字节
    case BP_EVT_RX_BYTE_TIMEOUT:
    // 串口接收帧超时, 接受的数据不完整
    case BP_EVT_RX_FRAME_TIMEOUT:
        //self->master->died ++;
        if ( self->master->died < self->master->died_line ) {
            //self->master->died ++;
        } else {
            //self->master->died ++;
            if ( ! bit_read(task, S_CARD_READER_COMM_DOWN) ) {
            }
            log_printf(ERR, "UART: "RED("读卡器通信中断, 请排查故障,(%d)"),
                       self->master->died);
            bit_set(task, S_CARD_READER_COMM_DOWN);
        }
        log_printf(WRN, "UART: %s get signal TIMEOUT", __FUNCTION__);
        query_stat = SEQ_FIND_CARD;
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

// 卡片初始化
int card_init_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    static QUERY_STAT query_stat = SEQ_FIND_CARD;
    static char ID[16], id_len = 0, def_passwd[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    int ret = ERR_ERR;
    unsigned char buff[64];
    int nr = 0, l;

    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size > 1 && param->payload_size >= param->buff.rx_buff[0] ) {
            if ( param->buff.rx_buff[ param->payload_size - 2 ] ==
                 BCC_code(param->buff.rx_buff, param->payload_size - 2) ) {
                return ERR_OK;
            } else {
                log_printf(DBG_LV2, "UART: SUM check result: need: %02X, gave: %02X",
                           BCC_code(param->buff.rx_buff, param->payload_size - 1),
                           param->buff.rx_buff[ param->payload_size - 2 ]);
                return ERR_FRAME_CHECK_ERR;
            }
        } else {
            return ERR_FRAME_CHECK_DATA_TOO_SHORT;
        }
        break;
    // 串口接收到新数据
    case BP_EVT_RX_DATA:

        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
        if ( bit_read(task, S_CARD_READER_COMM_DOWN) ) {
            log_printf(INF, "读卡器通信恢复");
            bit_clr(task, S_CARD_READER_COMM_DOWN);
        }
        switch ( query_stat ) {
        case SEQ_FIND_CARD:
            if ( param->buff.rx_buff[0] <= 8 ) return ERR_OK;
            memcpy(ID, &param->buff.rx_buff[8], param->buff.rx_buff[7]);
            id_len = param->buff.rx_buff[7];
            if ( task->uipage == UI_PAGE_MAIN ) {
                log_printf(INF, "UART: 寻到新卡，进行读扇区密码验证.");
                query_stat = SEQ_SECTOR_RD_AUTH;
            } else if ( task->uipage == UI_PAGE_JOBS ) {
                query_stat = SEQ_WRITE_PUBLIC_BLK;
            }
            ret = ERR_NEED_ECHO;
            break;
        case SEQ_SECTOR_WR_AUTH:
            if ( param->buff.rx_buff[2] == 0x00 ) {
                // 认证成功
                log_printf(INF, "UART: 认证成功");
                query_stat = SEQ_READ_PUBLIC_BLK;
                ret = ERR_NEED_ECHO;
            } else {
                // 认证失败
                log_printf(WRN, "UART: 认证失败");
                query_stat = SEQ_FIND_CARD;
            }
            break;
        case SEQ_WRITE_PUBLIC_BLK:
            if ( param->buff.rx_buff[2] != 0 ) {
                log_printf(WRN, "UART: 读卡器读取数据区失败, 错误码: %d",
                           param->buff.rx_buff[2]);
            } else {
                struct user_card cd;

                memcpy(cd.card.sector_4.buff, param->buff.rx_buff, 16);
                if ( cd.card.sector_4.data.magic != 0x4F4E5057 ) {
                    log_printf(WRN, "UART: 无法识别的卡: %08X.", cd.card.sector_4.data.magic);
                } else if ( cd.card.sector_4.data.sum !=
                            check_sum(param->buff.rx_buff, 15) ) {
                    log_printf(WRN, "UART: 卡数据损坏, 校验失败.");
                } else {
                    int faile = 0;
                    if ( cd.card.sector_4.data.remain_sum !=
                            check_sum(cd.card.sector_4.data.remain_money, 3) ) {
                        log_printf(WRN, "UART: 卡数据字段： 余额校验失败.");
                        faile ++;
                    }
                    if ( cd.card.sector_4.data.passwd_sum !=
                                check_sum(cd.card.sector_4.data.passwd_code, 3) ) {
                        log_printf(WRN, "UART: 卡数据字段： 密码校验失败.");
                        faile ++;
                    }

                    if ( ! faile ) {
                        log_printf(INF, "UART: 刷卡完成[卡号: %02X%02X%02X%02X, 余额: %d]",
                                   ID[0], ID[1], ID[2], ID[3],
                                cd.card.sector_4.data.remain_money);
                    }
                }
            }
            query_stat = SEQ_FIND_CARD;
            ret = ERR_OK;
            break;
            break;
        default:
            break;
        }
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        return ERR_ERR;
        switch ( query_stat ) {
        case SEQ_FIND_CARD:
            buff[ nr ++ ] = 0x08;
            buff[ nr ++ ] = 0x02;
            buff[ nr ++ ] = 0x4D;
            buff[ nr ++ ] = 0x02;
            buff[ nr ++ ] = 0x00;
            buff[ nr ++ ] = 0x26;
            l = nr;
            buff[ nr ++ ] = BCC_code(buff, l);
            buff[ nr ++ ] = 0x03;

            memcpy(param->buff.tx_buff, buff, nr);
            param->payload_size = nr;
            self->master->time_to_send = param->payload_size * 1000 / 960;
            self->rx_param.need_bytes = 15;
            log_printf(DBG_LV3, "UART: %s:SEQ_FIND_CARD requested.", __FUNCTION__);
            ret = ERR_OK;
            break;
        case SEQ_SECTOR_RD_AUTH:
            buff[ nr ++ ] = 0x12;
            buff[ nr ++ ] = 0x02;
            buff[ nr ++ ] = 0x46;
            buff[ nr ++ ] = 0x0C;  // 6字节密钥
            buff[ nr ++ ] = 0x60;  // 密钥A
            buff[ nr ++ ] = ID[0];
            buff[ nr ++ ] = ID[1];
            buff[ nr ++ ] = ID[2];
            buff[ nr ++ ] = ID[3];

            buff[ nr ++ ] = def_passwd[0];
            buff[ nr ++ ] = def_passwd[1];
            buff[ nr ++ ] = def_passwd[2];
            buff[ nr ++ ] = def_passwd[3];
            buff[ nr ++ ] = def_passwd[4];
            buff[ nr ++ ] = def_passwd[5];

            buff[ nr ++ ] = 0x04;  // 默认存放于第四扇区

            l = nr;
            buff[ nr ++ ] = BCC_code(buff, l);
            buff[ nr ++ ] = 0x03;

            memcpy(param->buff.tx_buff, buff, nr);
            param->payload_size = nr;
            self->master->time_to_send = param->payload_size * 1000 / 960;
            self->rx_param.need_bytes = 7;
            log_printf(DBG_LV3, "UART: %s:SEQ_SECTOR_RD_AUTH requested.", __FUNCTION__);
            ret = ERR_OK;
            break;
        case SEQ_READ_PUBLIC_BLK:
            buff[ nr ++ ] = 0x07;
            buff[ nr ++ ] = 0x02;
            buff[ nr ++ ] = 0x47;
            buff[ nr ++ ] = 0x01;
            buff[ nr ++ ] = 0x04;  // 默认存放于第四扇区
            l = nr;
            buff[ nr ++ ] = BCC_code(buff, l);
            buff[ nr ++ ] = 0x03;

            memcpy(param->buff.tx_buff, buff, nr);
            param->payload_size = nr;
            self->master->time_to_send = param->payload_size * 1000 / 960;
            self->rx_param.need_bytes = 23;
            log_printf(DBG_LV3, "UART: %s:SEQ_READ_PUBLIC_BLK requested.", __FUNCTION__);
            ret = ERR_OK;
            break;
        default:
            query_stat = SEQ_FIND_CARD;
            break;
        }
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        ret = ERR_OK;
        break;
    // 串口数据发送完成事件
    case BP_EVT_TX_FRAME_DONE:
        break;
    // 串口接收单个字节超时，出现在接收帧的第一个字节
    case BP_EVT_RX_BYTE_TIMEOUT:
    // 串口接收帧超时, 接受的数据不完整
    case BP_EVT_RX_FRAME_TIMEOUT:
        log_printf(WRN, "UART: %s get signal TIMEOUT", __FUNCTION__);
        query_stat = SEQ_FIND_CARD;
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

// 读卡器安装
int card_install_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    static QUERY_STAT query_stat = SEQ_FIND_CARD;
    static char ID[16], id_len = 0, def_passwd[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    int ret = ERR_ERR;
    unsigned char buff[64];
    int nr = 0, l;

    switch (evt) {
    case BP_EVT_FRAME_CHECK:
        if ( param->payload_size > 1 && param->payload_size >= param->buff.rx_buff[0] ) {
            if ( param->buff.rx_buff[ param->payload_size - 2 ] ==
                 BCC_code(param->buff.rx_buff, param->payload_size - 2) ) {
                return ERR_OK;
            } else {
                log_printf(DBG_LV2, "UART: SUM check result: need: %02X, gave: %02X",
                           BCC_code(param->buff.rx_buff, param->payload_size - 1),
                           param->buff.rx_buff[ param->payload_size - 2 ]);
                return ERR_FRAME_CHECK_ERR;
            }
        } else {
            return ERR_FRAME_CHECK_DATA_TOO_SHORT;
        }
        break;
    // 串口接收到新数据
    case BP_EVT_RX_DATA:

        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
        if ( bit_read(task, S_CARD_READER_COMM_DOWN) ) {
            log_printf(INF, "读卡器通信恢复");
            bit_clr(task, S_CARD_READER_COMM_DOWN);
        }
        switch ( query_stat ) {
        case SEQ_FIND_CARD:
            if ( param->buff.rx_buff[0] <= 8 ) return ERR_OK;
            memcpy(ID, &param->buff.rx_buff[8], param->buff.rx_buff[7]);
            id_len = param->buff.rx_buff[7];
            if ( task->uipage == UI_PAGE_MAIN ) {
                log_printf(INF, "UART: 寻到新卡，进行读扇区密码验证.");
                query_stat = SEQ_SECTOR_RD_AUTH;
            } else if ( task->uipage == UI_PAGE_JOBS ) {
                query_stat = SEQ_WRITE_PUBLIC_BLK;
            }
            ret = ERR_NEED_ECHO;
            break;
        case SEQ_SECTOR_WR_AUTH:
            if ( param->buff.rx_buff[2] == 0x00 ) {
                // 认证成功
                log_printf(INF, "UART: 认证成功");
                query_stat = SEQ_READ_PUBLIC_BLK;
                ret = ERR_NEED_ECHO;
            } else {
                // 认证失败
                log_printf(WRN, "UART: 认证失败");
                query_stat = SEQ_FIND_CARD;
            }
            break;
        case SEQ_WRITE_PUBLIC_BLK:
            if ( param->buff.rx_buff[2] != 0 ) {
                log_printf(WRN, "UART: 读卡器读取数据区失败, 错误码: %d",
                           param->buff.rx_buff[2]);
            } else {
                struct user_card cd;

                memcpy(cd.card.sector_4.buff, param->buff.rx_buff, 16);
                if ( cd.card.sector_4.data.magic != 0x4F4E5057 ) {
                    log_printf(WRN, "UART: 无法识别的卡: %08X.", cd.card.sector_4.data.magic);
                } else if ( cd.card.sector_4.data.sum !=
                            check_sum(param->buff.rx_buff, 15) ) {
                    log_printf(WRN, "UART: 卡数据损坏, 校验失败.");
                } else {
                    int faile = 0;
                    if ( cd.card.sector_4.data.remain_sum !=
                            check_sum(cd.card.sector_4.data.remain_money, 3) ) {
                        log_printf(WRN, "UART: 卡数据字段： 余额校验失败.");
                        faile ++;
                    }
                    if ( cd.card.sector_4.data.passwd_sum !=
                                check_sum(cd.card.sector_4.data.passwd_code, 3) ) {
                        log_printf(WRN, "UART: 卡数据字段： 密码校验失败.");
                        faile ++;
                    }

                    if ( ! faile ) {
                        log_printf(INF, "UART: 刷卡完成[卡号: %02X%02X%02X%02X, 余额: %d]",
                                   ID[0], ID[1], ID[2], ID[3],
                                cd.card.sector_4.data.remain_money);
                    }
                }
            }
            query_stat = SEQ_FIND_CARD;
            ret = ERR_OK;
            break;
        default:
            break;
        }
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        switch ( query_stat ) {
        case SEQ_FIND_CARD:
            buff[ nr ++ ] = 0x08;
            buff[ nr ++ ] = 0x02;
            buff[ nr ++ ] = 0x4D;
            buff[ nr ++ ] = 0x02;
            buff[ nr ++ ] = 0x00;
            buff[ nr ++ ] = 0x26;
            l = nr;
            buff[ nr ++ ] = BCC_code(buff, l);
            buff[ nr ++ ] = 0x03;

            memcpy(param->buff.tx_buff, buff, nr);
            param->payload_size = nr;
            self->master->time_to_send = param->payload_size * 1000 / 960;
            self->rx_param.need_bytes = 15;
            log_printf(DBG_LV3, "UART: %s:SEQ_FIND_CARD requested.", __FUNCTION__);
            ret = ERR_OK;
            break;
        case SEQ_SECTOR_RD_AUTH:
            buff[ nr ++ ] = 0x12;
            buff[ nr ++ ] = 0x02;
            buff[ nr ++ ] = 0x46;
            buff[ nr ++ ] = 0x0C;  // 6字节密钥
            buff[ nr ++ ] = 0x60;  // 密钥A
            buff[ nr ++ ] = ID[0];
            buff[ nr ++ ] = ID[1];
            buff[ nr ++ ] = ID[2];
            buff[ nr ++ ] = ID[3];

            buff[ nr ++ ] = def_passwd[0];
            buff[ nr ++ ] = def_passwd[1];
            buff[ nr ++ ] = def_passwd[2];
            buff[ nr ++ ] = def_passwd[3];
            buff[ nr ++ ] = def_passwd[4];
            buff[ nr ++ ] = def_passwd[5];

            buff[ nr ++ ] = 0x04;  // 默认存放于第四扇区

            l = nr;
            buff[ nr ++ ] = BCC_code(buff, l);
            buff[ nr ++ ] = 0x03;

            memcpy(param->buff.tx_buff, buff, nr);
            param->payload_size = nr;
            self->master->time_to_send = param->payload_size * 1000 / 960;
            self->rx_param.need_bytes = 7;
            log_printf(DBG_LV3, "UART: %s:SEQ_SECTOR_RD_AUTH requested.", __FUNCTION__);
            ret = ERR_OK;
            ret = ERR_ERR;
            break;
        case SEQ_READ_PUBLIC_BLK:
            buff[ nr ++ ] = 0x07;
            buff[ nr ++ ] = 0x02;
            buff[ nr ++ ] = 0x47;
            buff[ nr ++ ] = 0x01;
            buff[ nr ++ ] = 0x04;  // 默认存放于第四扇区
            l = nr;
            buff[ nr ++ ] = BCC_code(buff, l);
            buff[ nr ++ ] = 0x03;

            memcpy(param->buff.tx_buff, buff, nr);
            param->payload_size = nr;
            self->master->time_to_send = param->payload_size * 1000 / 960;
            self->rx_param.need_bytes = 23;
            log_printf(DBG_LV3, "UART: %s:SEQ_READ_PUBLIC_BLK requested.", __FUNCTION__);
            ret = ERR_OK;
            break;
        default:
            query_stat = SEQ_FIND_CARD;
            break;
        }
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        ret = ERR_OK;
        break;
    // 串口数据发送完成事件
    case BP_EVT_TX_FRAME_DONE:
        break;
    // 串口接收单个字节超时，出现在接收帧的第一个字节
    case BP_EVT_RX_BYTE_TIMEOUT:
    // 串口接收帧超时, 接受的数据不完整
    case BP_EVT_RX_FRAME_TIMEOUT:
        log_printf(WRN, "UART: %s get signal TIMEOUT", __FUNCTION__);
        query_stat = SEQ_FIND_CARD;
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

int uart_sniffer_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                     struct bp_evt_param *param)
{
    int ret;
    switch (evt) {
    case BP_EVT_FRAME_CHECK:
    // 串口接收到新数据
    case BP_EVT_RX_DATA:
        break;
    // 串口收到完整的数据帧
    case BP_EVT_RX_FRAME:
        break;
    // 串口发送数据请求
    case BP_EVT_TX_FRAME_REQUEST:
        return ERR_ERR;
        break;
    // 串口发送确认
    case BP_EVT_TX_FRAME_CONFIRM:
        ret = ERR_OK;
        break;
    // 串口数据发送完成事件
    case BP_EVT_TX_FRAME_DONE:
        break;
    // 串口接收单个字节超时，出现在接收帧的第一个字节
    case BP_EVT_RX_BYTE_TIMEOUT:
    // 串口接收帧超时, 接受的数据不完整
    case BP_EVT_RX_FRAME_TIMEOUT:
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

int uart5_background_evt_handle(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
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
    int mydone = 0;
    int ret = 0;
    struct bp_uart *thiz = (struct bp_uart *)arg;
    int retval;
    size_t cursor;
    fd_set rfds;
    struct timeval tv ;

    log_printf(INF, "UART %s framework start up.              DONE(%ld).",
               thiz->dev_name,
               (unsigned int)pthread_self());

    if ( done == NULL ) done = &mydone;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    FD_ZERO(&rfds);

    if ( thiz ) {
        // 出错误后尝试的次数
        thiz->init_magic = 5;
    }

    while ( 1 ) {
        usleep(5000);
        if ( thiz == NULL ) continue;
        if ( thiz->bp_evt_handle == NULL ) continue;
        if ( thiz->status == BP_UART_STAT_ALIENT ) continue;
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

            log_printf(INF, "UART: open UART %d:%s correct(%X).",
                       thiz->dev_handle, thiz->dev_name, thiz->status);
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
___fast_switch_2_rx:
        if ( thiz->status == BP_UART_STAT_RD ) {
            char *buff = (char *)thiz->rx_param.buff.rx_buff;
            int rd = 0;
            int rddone = 0;
            int nr = 0, cursor = 0;

            thiz->rx_param.payload_size = 0;
            thiz->rx_param.cursor = 0;
            thiz->rx_param.buff.rx_buff = thiz->rx_buff;
            thiz->rx_param.buff_size = sizeof(thiz->rx_buff);
            ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;

            Hachiko_feed(&thiz->rx_seed);
            for (;
                     thiz->status == BP_UART_STAT_RD &&
                     (unsigned)ret == ERR_FRAME_CHECK_DATA_TOO_SHORT &&
                     thiz->rx_seed.remain
                 ; )
            {
                FD_ZERO(&rfds);
                FD_SET(thiz->dev_handle, &rfds);
                tv.tv_sec = 1;
                tv.tv_usec = 0;
                retval = select(thiz->dev_handle+1, &rfds, NULL, NULL, &tv);
                if ( -1 == retval ) {
                    log_printf(INF, "select error.");
                    continue;
                } else if ( retval != 0 ) {
                    if ( ! FD_ISSET(thiz->dev_handle, &rfds) ) {
                        log_printf(ERR, "UART.DRIVER: somethind happended.");
                        continue;
                    }
                } else { // 超时s
                    ret == (int)(ERR_FRAME_CHECK_DATA_TOO_SHORT);
                    continue;
                }
                errno = 0;
                cursor = thiz->rx_param.cursor;
                rd = read(thiz->dev_handle,
                          &thiz->rx_param.buff.rx_buff[cursor], 16);
                if ( rd > 0 ) {
                    Hachiko_feed(&thiz->rx_seed);
                    thiz->rx_param.payload_size += rd;
                    thiz->rx_param.cursor = thiz->rx_param.payload_size;
                    nr += rd;
                }
                ret = thiz->bp_evt_handle(thiz, BP_EVT_FRAME_CHECK,
                                          &thiz->rx_param);
                thiz->uart_mode = UART_MODE_NORMAL;
                switch ( ret ) {
                // 数据接收，校验完成, 完成数据接收过程，停止接收
                case ERR_OK:
                    __dump_uart_hex((unsigned char*)buff, nr, DBG_LV3);
                    thiz->status = BP_UART_STAT_WR;
                    Hachiko_pause(&thiz->rx_seed);
                    log_printf(DBG_LV0, "UART: fetched a "GRN("new")" frame.");
                    ret = thiz->bp_evt_handle(thiz, BP_EVT_RX_FRAME,
                                                          &thiz->rx_param);
                    if ( (unsigned)ret == ERR_NEED_ECHO ) {
                        thiz->uart_mode = UART_MODE_SESSION;
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

                    rddone ++;
                    ret = ERR_OK;
                    break;
                // 数据接收完成，但校验失败, 停止接收
                case ERR_FRAME_CHECK_ERR:
                    __dump_uart_hex((unsigned char*)buff, nr, WRN);
                    thiz->bp_evt_handle(thiz, BP_EVT_FRAME_CHECK_ERROR,
                                                              &thiz->rx_param);
                    thiz->status = BP_UART_STAT_WR;
                    thiz->continues_nr = 0;
                    Hachiko_pause(&thiz->rx_seed);
                    log_printf(WRN,
                               "UART: lenth fetched but check "RED("faile."));
                    rddone ++;
                    break;
                // 数据接收长度不足，继续接收
                default:
                case ERR_FRAME_CHECK_DATA_TOO_SHORT:
                    ret = ERR_FRAME_CHECK_DATA_TOO_SHORT;
                    if ( rd > 0 ) {
                        thiz->bp_evt_handle(thiz, BP_EVT_RX_DATA, &thiz->rx_param);
                    }
                    break;
                }
                if ( rddone  ) break;
            }
            if ( ret == ERR_OK ) {
                // every thing is ok
                log_printf(DBG_LV1, "UART: rx packet TIME-OUT.need: %d, fetched: "GRN("%d"),
                           thiz->rx_param.need_bytes,
                            thiz->rx_param.payload_size);
            } else if ( ret == ERR_FRAME_CHECK_ERR ) {
                // every thing is ok
                log_printf(WRN, "UART: rx packet TIME-OUT.need: %d, fetched: "YEL("%d")/*"gave crc: %02X%02X need: %04X"*/,
                           thiz->rx_param.need_bytes, thiz->rx_param.payload_size);
            } else if ( ret == ERR_FRAME_CHECK_DATA_TOO_SHORT ) {
                // recieve timeout.
                __dump_uart_hex(thiz->rx_param.buff.rx_buff, thiz->rx_param.need_bytes, WRN);
                if ( thiz->rx_param.payload_size == 0 ) {
                    thiz->bp_evt_handle(thiz, BP_EVT_RX_BYTE_TIMEOUT, &thiz->rx_param);
                } else { // ( thiz->rx_param.payload_size < thiz->rx_param.need_bytes ) {
                    thiz->bp_evt_handle(thiz, BP_EVT_RX_FRAME_TIMEOUT, &thiz->rx_param);
                }
            } else {
                // could not to be here.
                log_printf(ERR, "UART.driver: Crashed %X@ %s:%d", ret, __FILE__, __LINE__);
            }
            thiz->status = BP_UART_STAT_WR;
        }

        // 程序默认采用9600 的波特率， 大致估算出每发送一个字节耗时1.04ms
        // 抛去程序运行时的延迟，发送延迟，可估计每发送一个字节耗时1.1 ms
        if ( thiz->status == BP_UART_STAT_WR ) {
            thiz->tx_param.buff.tx_buff = thiz->tx_buff;
            thiz->tx_param.buff_size = sizeof(thiz->tx_buff);
            thiz->tx_param.payload_size = 0;
            thiz->tx_param.cursor = 0;

            if ( thiz->uart_mode == UART_MODE_NORMAL ) {
                if ( thiz->hw_bps == 2400 ) {
                    usleep(1500 * 1000);
                } else {
                    usleep(120 * 1000);
                }
            }

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
            tcdrain(thiz->dev_handle);
            tcflush(thiz->dev_handle, TCIOFLUSH);

continue_to_send:
            cursor = thiz->tx_param.cursor;
            retval = 0;
            __dump_uart_hex(thiz->tx_param.buff.tx_buff, thiz->tx_param.payload_size, DBG_LV3);
            cursor = thiz->tx_param.cursor;
            retval = write(thiz->dev_handle,
                           & thiz->tx_param.buff.tx_buff[cursor],
                           thiz->tx_param.payload_size - cursor);
            if ( retval <= 0 ) {
                log_printf(ERR, "UART: send error %d, TX REQUEST AUTOMATIC ABORTED. ", retval);
                thiz->tx_param.buff.tx_buff = thiz->tx_buff;
                thiz->tx_param.buff_size = sizeof(thiz->tx_buff);
                thiz->tx_param.payload_size = 0;
                thiz->tx_param.cursor = 0;
                continue;
            }
            log_printf(INF, "uart %d bytes sent", retval);
            if ( retval == (int)(thiz->tx_param.payload_size - cursor) ) {
                thiz->tx_param.cursor = thiz->tx_param.payload_size;
                memset(thiz->rx_param.buff.rx_buff, 0, thiz->rx_param.buff_size);
                thiz->bp_evt_handle(thiz, BP_EVT_TX_FRAME_DONE, &thiz->tx_param);
                thiz->tx_param.payload_size = 0;
                if ( thiz->rx_param.need_bytes ) {
                    thiz->status = BP_UART_STAT_RD;
                    if ( thiz->role == BP_UART_MASTER ) {
                        // 主动设备，需要进行接收超时判定
                        thiz->rx_seed.ttl = thiz->master->ttw;
                        Hachiko_resume(&thiz->rx_seed);
                    }
                    goto ___fast_switch_2_rx;
                } else {
                    thiz->tx_param.buff.tx_buff = thiz->tx_buff;
                    thiz->tx_param.buff_size = sizeof(thiz->tx_buff);
                    thiz->tx_param.payload_size = 0;
                    thiz->tx_param.cursor = 0;
                    memset(thiz->tx_buff, 0, sizeof(thiz->tx_buff));
                    log_printf(DBG_LV3, "不需要帧回应");
                    usleep(4 * 1000);
                }
            } else if ( retval < (int)(thiz->tx_param.payload_size - cursor) ) {
                // 发送了一部分
                log_printf(INF, "UART.driver: send childs");
                thiz->tx_param.cursor = thiz->tx_param.cursor + retval;
                //goto continue_to_send;
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
