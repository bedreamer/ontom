#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

struct user_card {
    // 触发任务时的卡号
    char triger_card_sn[64];
    struct {
        unsigned char id[16];
        unsigned char sector_num;
        union {
            unsigned char buff[16];
            struct {
                unsigned int magic;           /*奥能魔数 'ONPW', Only-Power 0x4F4E5057 */
                unsigned char remain_money[3]; /*余额， 范围 0.00 ~ 167772.15 */
                unsigned char remain_sum;    /*余额校验位 */
                unsigned char passwd_code[3];  /*6 位BCD 码密码*/
                unsigned char passwd_sum;    /*密码校验和 */
                unsigned short reserved;
                unsigned char unuesed;
                unsigned char sum;            /*校验和 */
            }data;
        }sector_4;
    }card;
};

#define SECT   4

// 串口IO事件
typedef enum {
    // 串口数据结构初始化
    BP_EVT_INIT                  = 0x00,
    // 串口配置
    BP_EVT_CONFIGURE,
    // 关闭串口
    BP_EVT_KILLED,
    // 串口数据帧校验
    BP_EVT_FRAME_CHECK,
    // 切换到发送模式
    BP_EVT_SWITCH_2_TX,
    // 切换到接收模式
    BP_EVT_SWITCH_2_RX,

    // 串口接收到新数据
    BP_EVT_RX_DATA               = 0x10,
    // 串口收到完整的数据帧
    BP_EVT_RX_FRAME,

    // 串口发送数据请求
    BP_EVT_TX_FRAME_REQUEST      = 0x20,
    // 串口发送确认
    BP_EVT_TX_FRAME_CONFIRM,
    // 串口数据发送完成事件
    BP_EVT_TX_FRAME_DONE,

    // 串口接收单个字节超时，出现在接收帧的第一个字节
    BP_EVT_RX_BYTE_TIMEOUT       = 0x40,
    // 串口接收帧超时, 接受的数据不完整
    BP_EVT_RX_FRAME_TIMEOUT,

    // 串口IO错误
    BP_EVT_IO_ERROR              = 0x80,
    // 帧校验失败
    BP_EVT_FRAME_CHECK_ERROR
}BP_UART_EVENT;

typedef enum {
    // 寻卡模式
    SEQ_FIND_CARD = 0,
    // 读扇区密码认证
    SEQ_SECTOR_RD_AUTH,
    // 写扇区密码认证
    SEQ_SECTOR_WR_AUTH,
    // 读取公开数据区
    SEQ_READ_PUBLIC_BLK,
    // 写入公开数据区
    SEQ_WRITE_PUBLIC_BLK,
    // 读取私密数据区1
    SEQ_READ_PRIVATE_BLK1,
    // 读取私密数据区2
    SEQ_READ_PRIVATE_BLK2,
    // 读取私密数据区3
    SEQ_READ_PRIVATE_BLK3,
    // 读取私密数据区4
    SEQ_READ_PRIVATE_BLK4,
    // 读取私密数据区5
    SEQ_READ_PRIVATE_BLK5,
    // 读取私密数据区6
    SEQ_READ_PRIVATE_BLK6,
    // 读取私密数据区7
    SEQ_READ_PRIVATE_BLK7,
    SEQ_INVALID
}QUERY_STAT;

unsigned char BCC_code(unsigned char *da,size_t len) {
    size_t i  = 0;
    unsigned char BCC = 0;

    for ( ; i < len; i ++ ) {
        BCC ^= da[i];
    }
    return ~BCC;
}
unsigned char check_sum(unsigned char *buff, size_t len) {
    unsigned int i = 0;
    unsigned char sum = 0;
    while ( i < len ) {
        sum = sum + buff[i ++];
    }
    return sum;
}

void __dump_uart_hex(unsigned char *buff, unsigned char *hex, int len)
{
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
}

#if 0
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
            printf("读卡器通信中断, 请排查故障,(%d)");
        }
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
#endif

void show_help(int cmd);


int __read_card_sector(unsigned char *obuf, int dev, unsigned char *id, unsigned char *passwd, unsigned char sec)
{
	unsigned char buff[32] = {0};
	unsigned char rx_buff[64] = {0};
	unsigned char bcc;
	int l, nr = 0, ret;

	buff[ nr ++ ] = 0x07;
	buff[ nr ++ ] = 0x02;
	buff[ nr ++ ] = 0x47;
	buff[ nr ++ ] = 0x01;
	buff[ nr ++ ] = 0x04;  // 默认存放于第四扇区
	l = nr;
	buff[ nr ++ ] = BCC_code(buff, l);
	buff[ nr ++ ] = 0x03;
	ret = write(dev, buff, nr);
	if ( 0 == ret ) {
		printf("读扇区失败!\n");
		return 0;
	}
	tcdrain(dev);
	return __read_result(dev, obuf);
}

int __read_result(int dev, unsigned char *obuf)
{
    fd_set rfds;
    struct timeval tv ;
	int retval;
	
	usleep(100 * 1000); // 100 ms

	FD_ZERO(&rfds);
	FD_SET(dev, &rfds);
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	retval = select(dev+1, &rfds, NULL, NULL, &tv);
	if ( 0 >= retval ) {
		return 0;
	}

	return read(dev, obuf, 64);
}

int find_card(int dev, unsigned char *id)
{
	unsigned char buff[32] = {0};
	unsigned char rx_buff[64] = {0};
	unsigned char bcc;
	int l, nr = 0, ret;

	buff[ nr ++ ] = 0x08;
	buff[ nr ++ ] = 0x02;
	buff[ nr ++ ] = 0x4D;
	buff[ nr ++ ] = 0x02;
	buff[ nr ++ ] = 0x00;
	buff[ nr ++ ] = 0x26;
	l = nr;
	buff[ nr ++ ] = BCC_code(buff, l);
	buff[ nr ++ ] = 0x03;

	ret = write(dev, buff, nr);
	if ( 0 == ret ) {
		printf("寻卡失败!\n");
		return 0;
	}
	tcdrain(dev);

	ret = __read_result(dev, rx_buff);
	if ( ret == 0 ) {
		return 0;
	}
	
	if ( rx_buff[0] <= 8 ) return 0;

	bcc = BCC_code(rx_buff, ret - 2);
	if ( bcc != rx_buff[ ret - 2] ) {
		printf("无法识别的卡!\n");
		return 0;
	}

	memcpy(id, &rx_buff[8], rx_buff[7]);

	return 1;
}

int auth_card(int dev, unsigned char * id, unsigned char *passwd, unsigned char sect)
{
	unsigned char buff[32] = {0};
	unsigned char rx_buff[64] = {0};
	unsigned char bcc;
	int l, nr = 0, ret;
	
	buff[ nr ++ ] = 0x12;
	buff[ nr ++ ] = 0x02;
	buff[ nr ++ ] = 0x46;
	buff[ nr ++ ] = 0x0C;  // 6字节密钥
	buff[ nr ++ ] = 0x60;  // 密钥A
	buff[ nr ++ ] = id[0];
	buff[ nr ++ ] = id[1];
	buff[ nr ++ ] = id[2];
	buff[ nr ++ ] = id[3];

	buff[ nr ++ ] = passwd[0];
	buff[ nr ++ ] = passwd[1];
	buff[ nr ++ ] = passwd[2];
	buff[ nr ++ ] = passwd[3];
	buff[ nr ++ ] = passwd[4];
	buff[ nr ++ ] = passwd[5];

	buff[ nr ++ ] = sect;  // 默认存放于第四扇区

	l = nr;
	buff[ nr ++ ] = BCC_code(buff, l);
	buff[ nr ++ ] = 0x03;
	
	ret = write(dev, buff, nr);
	if ( 0 == ret ) {
		printf("卡认证失败!\n");
		return 0;
	}
	tcdrain(dev);

	ret = __read_result(dev, rx_buff);
	if ( ret == 0 ) {
		return 0;
	}
	bcc = BCC_code(rx_buff, ret - 2);
	if ( bcc != rx_buff[ ret - 2] ) {
		printf("无法确定认证结果!\n");
		return 0;
	}
	if ( rx_buff[2] != 0x00 ) {
		printf("认证失败： %d\n", rx_buff[2]);
		return 1;
	}

	printf("卡认证成功!\n");
	return 1;
}

int read_card(int dev, unsigned char *id, unsigned char *passwd, unsigned char sec)
{
	unsigned char rx_buff[128]  = {0};
	unsigned char rx_hex[512] = {0};
	struct user_card cd;

	if ( ! auth_card(dev, id, passwd, sec) ) {
		return 0;
	}
	
	if ( ! __read_card_sector(rx_buff, dev, id, passwd, sec) ) {
		printf("读取扇区%d失败!\n", sec);
		return 0;
	}

	memcpy(cd.card.sector_4.buff, &rx_buff[4], 16);
	__dump_uart_hex(rx_hex, cd.card.sector_4.buff, 16);
	printf("读取成功(%s)!\n", rx_hex);
	printf("\t%16s: %02X%02X%02X%02X\n", "ID", id[3], id[2], id[1], id[0]);
	if ( cd.card.sector_4.data.magic != 0x4F4E5057 ) {
		printf("\t%16s: %08X\n", "Magic", cd.card.sector_4.data.magic);
		printf("\t%16s: %s\n", "Status", "Unrecognized");
		return 0;
	} else if ( cd.card.sector_4.data.sum !=
				check_sum(cd.card.sector_4.buff, 15) ) {
		char str[32];

		printf("\t%16s: %08X\n", "Magic", cd.card.sector_4.data.magic);
		printf("\t%16s: %s\n", "Status", "Fault");
		sprintf(str, "Faile, gave: %02X, need: %02X", 
		cd.card.sector_4.data.sum, check_sum(cd.card.sector_4.buff, 15));
		printf("\t%16s: %s\n", "Check Code", str);
	} else {
		int faile = 0;
		char str[32];

		printf("\t%16s: %08X\n", "Magic", cd.card.sector_4.data.magic);
		printf("\t%16s: %s\n", "Status", "OK");
		sprintf(str, "OK, gave: %02X, need: %02X", 
		cd.card.sector_4.data.sum, check_sum(cd.card.sector_4.buff, 15));
		printf("\t%16s: %s\n", "Check Code", str);

		if ( cd.card.sector_4.data.remain_sum !=
				check_sum(cd.card.sector_4.data.remain_money, 3) ) {
			faile ++;
			printf("\t%16s: %s\n", "Money Check Code", str);		
		}
		if ( cd.card.sector_4.data.passwd_sum !=
					check_sum(cd.card.sector_4.data.passwd_code, 3) ) {
			faile ++;
			printf("\t%16s: %s\n", "Password Check Code", str);	
		}
		if ( ! faile ) {
			unsigned int money = *(unsigned int *)(void*)cd.card.sector_4.data.remain_money;
			char buff[32];
			money &= 0x00FFFFFF;
			sprintf(buff, "%.2f", money / 100.0f);
			printf("\t%16s: %s\n", "Reamin money", buff);
			sprintf(buff, "%d%d%d%d%d%d",
					cd.card.sector_4.data.passwd_code[2]>>4,
					cd.card.sector_4.data.passwd_code[2]&0xF,
					cd.card.sector_4.data.passwd_code[1]>>4,
					cd.card.sector_4.data.passwd_code[1]&0xF,
					cd.card.sector_4.data.passwd_code[0]>>4,
					cd.card.sector_4.data.passwd_code[0]&0xF);
			printf("\t%16s: %s\n", "Password", buff);
		}
	}
	return 0;
}

int format_card(int dev, unsigned char *id, unsigned char *def_passwd, unsigned int money, unsigned int sec)
{
	return 0;
}

int write_card(int dev, unsigned char *id, unsigned char *new_passwd, unsigned int money, unsigned int sec)
{
	return 0;
}

typedef enum {
	READ_CARD,
	FORMAT_CARD,
	WRITE_CARD
}RUNMOD;

void show_help(int cmd)
{
	printf(
	"Usage:\n"
	"    card [-D DEVICE] [CMD] {OPTIONS}\n"
	"DEVICE: tty device.\n"
	"CMD:\n"
	"    -r: read card data.\n"
	"    -f: format a card.\n"
	"    -w: write card data.\n"
	"OPTIONS:\n"
	"    -I: infinite mode.\n"
	"    -P: password for card auth.\n"
	"    -M: set new money value to card.\n"
	"    -h: show this help\n\n"
	"AUTHOR:\n"
	"	 LiJie <bedreamer@163.com> 2015/05/15"
	);
}


int main(int argc, char **argv)
{
	unsigned char passwd[16] = {255, 255, 255, 255, 255, 255};
	int money = 0.0f;
	char device[256] = {0};
	unsigned char id[16] = {0};
	int dev = -1;
	unsigned char frame_tx[128];
	unsigned char frame_rx[128];
	int done = 0;
	int inifinal = 0;
	int ok = 0;
	int oc;
	RUNMOD mode = READ_CARD;

	while((oc = getopt(argc, argv, "rfwhID:P:M:")) != -1)    
	{         
		switch(oc)        
		{
		case 'r':
			mode = READ_CARD;
		break;
		case 'f':
			mode = FORMAT_CARD;
		break;
		case 'w':
			mode = WRITE_CARD;
		break;
		case 'I':
			inifinal = 1;
		break;
		case 'D':
			strncpy(device, optarg, 256);
		break;
		case 'P':
			strncpy(passwd, optarg, 6);
			do {
				int len = strlen(passwd);
				int i;
				for ( i = 0; i < len; i ++ )
					passwd[i] = passwd[i] - '0';
			} while ( 0);
		break;
		case 'M':
			money = (int)atof(optarg) * 100;
		break;
		case 'h':
			show_help(0);
			exit(0);
		break;
		default:
			show_help(1);
			exit(1);
		break;  
		}
	}

	dev = open(device, O_RDWR | O_NOCTTY);
	if ( dev == -1 ) {
		printf("open device %s faile!\n", device);
		exit(1);
	}

	while ( ! done )
	{
		memset(id, 0, sizeof(id));
		while ( ! find_card(dev, id) ) {
			sleep(1);
		}
	
		printf("寻卡成功.\n");
		
		switch ( mode ) {
		case READ_CARD:
			read_card(dev, id, passwd, SECT);
		break;
		case FORMAT_CARD:
		ok = format_card(dev, id, passwd, money, SECT);
		if ( ok ) {
			getchar();
		}
		break;
		case WRITE_CARD:
		ok = write_card(dev, id, passwd, money, SECT);
		if ( ok ) {
			getchar();
		}
		break;
		}

		if ( inifinal ) continue;
		done = 1;
	}

	return 0;
}