#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

#pragma pack(1)
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
#pragma pack()

#define SECT   4

unsigned char dump_buff[1024];

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

double __card_read_remain(const struct user_card *c) {
    unsigned int imoney = 0;

    imoney += c->card.sector_4.data.remain_money[0];
    imoney += c->card.sector_4.data.remain_money[1] * 256;
    imoney += c->card.sector_4.data.remain_money[2] * 256 * 256;

    return imoney / 100.0f;
}

double __card_write_remain(struct user_card * c, double money) {
    unsigned int imoney = money * 100;
    c->card.sector_4.data.remain_money[0] = imoney & 0xFF;
    c->card.sector_4.data.remain_money[1] = (imoney >> 8) & 0xFF;
    c->card.sector_4.data.remain_money[2] = (imoney >> 16) & 0xFF;
    c->card.sector_4.data.remain_sum =
            check_sum(c->card.sector_4.data.remain_money, 3);
    return money;
}

void __card_read_passwd(const struct user_card *c, char *passwd) {
    sprintf(passwd, "%d%d%d%d%d%d",
            c->card.sector_4.data.passwd_code[2]>>4,
            c->card.sector_4.data.passwd_code[2]&0xF,
            c->card.sector_4.data.passwd_code[1]>>4,
            c->card.sector_4.data.passwd_code[1]&0xF,
            c->card.sector_4.data.passwd_code[0]>>4,
            c->card.sector_4.data.passwd_code[0]&0xF);
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
            options.c_iflag |= (INPCK | ISTRIP);
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            break;

        case 'o':
        case 'O':
        case 2:
            options.c_cflag |= PARENB;
            options.c_cflag |= PARODD;
            options.c_iflag |= (INPCK | ISTRIP);
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

    printf("UART.config: %d %d %d", databits, parity, stopbits);

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
	ret = write_frame(dev, buff, nr);
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
	retval = read(dev, obuf, 64);
	
	__dump_uart_hex(dump_buff, obuf, retval);
	printf("RX: %s\n", dump_buff);
	return retval;
}

int write_frame(int dev, void *buff, size_t len)
{
	int ret;

	ret = write(dev, buff, len);
	if ( 0 == ret ) {
		return 0;
	}
	tcdrain(dev);
	__dump_uart_hex(dump_buff, buff, ret);
	printf("TX: %s\n", dump_buff);
	return ret;
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
	
	ret = write_frame(dev, buff, nr);
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
			printf("\t%16s: %s\n", "Money Check Status", "Untrusted");
		}
		sprintf(str, "OK, gave: %02X, need: %02X", 
			cd.card.sector_4.data.remain_sum, check_sum(cd.card.sector_4.data.remain_money, 3));
		printf("\t%16s: %s\n", "Money Check Code", str);
		
		if ( cd.card.sector_4.data.passwd_sum !=
					check_sum(cd.card.sector_4.data.passwd_code, 3) ) {
			faile ++;
			printf("\t%16s: %s\n", "Password Check Status", "Untrusted");
		}
		sprintf(str, "OK, gave: %02X, need: %02X", 
			cd.card.sector_4.data.passwd_sum,
					check_sum(cd.card.sector_4.data.passwd_code, 3));
		printf("\t%16s: %s\n", "Password Check Code", str);	
		if ( 1 ) {
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

int format_card(int dev, unsigned char *id, unsigned char *def_passwd, double money, unsigned int sec)
{
	unsigned char buff[32] = {0};
	unsigned char rx_buff[64] = {0};
	unsigned char bcc;
	int l, nr = 0, ret;
	struct user_card cd={0};
	unsigned char init_passwd[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	buff[ nr ++ ] = 0x17;
	buff[ nr ++ ] = 0x02;
	buff[ nr ++ ] = 0x48;
	buff[ nr ++ ] = 0x11;
	buff[ nr ++ ] = sec; // 写第4扇区

	do {
		int i = 0;
		cd.card.sector_4.data.magic = 0x4F4E5057;
		__card_write_remain(&cd, money);
		cd.card.sector_4.data.passwd_code[0] = 0x11;
		cd.card.sector_4.data.passwd_code[1] = 0x11;
		cd.card.sector_4.data.passwd_code[2] = 0x11;
		cd.card.sector_4.data.sum = check_sum(cd.card.sector_4.buff, 15);
		for ( i = 0; i < 16; i ++ ) {
			buff[ nr ++ ] = cd.card.sector_4.buff[i];
		}
	} while (0);

	l = nr;
	buff[ nr ++ ] = BCC_code(buff, l);
	buff[ nr ++ ] = 0x03;
	
	if ( ! auth_card(dev, id, init_passwd, sec) ) {
		printf("认证失败, 格式化卡失败!\n");
		return 0;	
	}
	
	ret = write_frame(dev, buff, nr);
	if ( 0 == ret ) {
		printf("格式化卡失败!\n");
		return 0;
	}
	tcdrain(dev);

	ret = __read_result(dev, rx_buff);
	if ( ret == 0 ) {
		return 0;
	}
	bcc = BCC_code(rx_buff, ret - 2);
	if ( bcc != rx_buff[ ret - 2] ) {
		printf("无法确定结果!\n");
		return 0;
	}
	if ( rx_buff[2] == 0x00 ) {
		printf("格式化成功!\n");
		return 1;
	}
	printf("格式化失败!\n");
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
	set_other_attribute(dev, 9600, 8, 1, 'N');

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