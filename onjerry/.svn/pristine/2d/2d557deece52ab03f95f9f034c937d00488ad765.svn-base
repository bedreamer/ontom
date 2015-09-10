#include "UART.h"
#include "Timer.h"
#include "DATA_pro.h"
/*union
{
    uint8_t UART1_TXfifo[U1_FRAME_SENDDATALEN]={0x1D,0x4C,0x1D,0x4C,0x00,0x00,0x01,0x54,0x00,0x00,0x00,0x00,0x00,0x00,0x0A};//发送队列数据部分，结构定义看协议
    Stu_Cur_State All_Status_Data;
}*/
//uint8_t UART1_TXfifo[U1_FRAME_SENDDATALEN]={0x4C,0x1D,0x4C,0x1D,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x27,0x0F,0x27,0x0F,0x27,0x0F,0x27,0xFA,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0A};//发送队列数据部分，结构定义看协议

//uint8_t UART1_RXfifo[U1_FRAME_RECEHEADLEN+U1_FRAME_RECEDATALEN]={0xF0,0xE1,0xD2,0xC3,U0_FRAME_ADD};//接收队列数据部分，结构定义看协议
Un_U1FIFOBuff U1_FIFOBuff = {0xF0,0xE1,0xD2,0xC3,U1_FRAME_ADD};
Un_U1TRANSFLEN U1_RxTxLen = {0};

/*bit0 为接收到完整一帧数据标志,
bit1 为接收到的一帧数据为可靠的数据帧标志(不是其他节点发送的数据帧的一部分),
10->00->01->11->10,此为收发处理完整一帧的过程11在主函数处理
00表示正在接收、01表示收到规定帧但未进行可靠数据帧判断
11表示可靠数据帧判断通过，10表示CRC通过，数据拷贝完成，
应答数据发送完成，开始接收
bit7为接收溢出标志*/
uint8_t U1_RXfin = 0x02;

/*发送状态标志
bit0发送可进行
bit1发送超时
bit2发送数据是否CRC校验(由AD数据处理函数置0，由发送CRC校验函数置1)
bit3开始发送延时时间到*/
uint8_t U1_TXfin = 0x00;

/*typedef struct
{
  __IO uint32_t CRL;
  __IO uint32_t CRH;
  __IO uint32_t IDR;
  __IO uint32_t ODR;
  __IO uint32_t BSRR;
  __IO uint32_t BRR;
  __IO uint32_t LCKR;
} GPIO_TypeDef;

typedef struct
{
  __IO uint32_t EVCR;
  __IO uint32_t MAPR;
  __IO uint32_t EXTICR[4];
  uint32_t RESERVED0;
  __IO uint32_t MAPR2;  
} AFIO_TypeDef;

typedef struct
{
  __IO uint16_t SR;
  uint16_t  RESERVED0;
  __IO uint16_t DR;
  uint16_t  RESERVED1;
  __IO uint16_t BRR;
  uint16_t  RESERVED2;
  __IO uint16_t CR1;
  uint16_t  RESERVED3;
  __IO uint16_t CR2;
  uint16_t  RESERVED4;
  __IO uint16_t CR3;
  uint16_t  RESERVED5;
  __IO uint16_t GTPR;
  uint16_t  RESERVED6;
} USART_TypeDef;*/

/*static uint8_t LittleEndianData_Send(uint8_t *LittleEndbuff, uint32_t length)
{
    for(;(!(U1_TXfin & 0x02)) && (length != 0);)//超时停止发送
    {
        if((USART1->SR & (uint16_t)0x0080)==(uint16_t)0x0080)
        {
            USART1->DR = (uint16_t )(*LittleEndbuff++);
            length--;
        }
    }
    if(length != 0)
    return 0x01;
    else
    return 0x00;
}

static uint8_t BigEndianData_Send(uint8_t *LittleEndbuff, uint32_t length)
{
    uint8_t flag = 0x00;
    for(;(!(U1_TXfin & 0x02)) && (length != 0);)//超时停止发送
    {
        if(((USART1->SR & (uint16_t)0x0080)==(uint16_t)0x0080))
        {
            if(!(flag & 0x01))//发高字节
            {
                USART1->DR = (uint16_t )(*(LittleEndbuff+1));
                flag = 0x01;//高字节已发送
            }
            else//发低字节
            {
                USART1->DR = (uint16_t )(*LittleEndbuff++);
                flag = 0x00;//高字节已发送
                LittleEndbuff ++;
            }
            length--;
        }
    }
    
    if(length != 0)
    return 0x01;
    else
    return 0x00;
}*/

void USART1_Send(void)
{
    if((U1_TXfin & 0x0F)== 0x0D)//发送CRC已经校验，发送使能，且UART发送已经使能,开始发送延时时间到，可进行发送
    {
        U1_TXfin &= 0xF5;//清发送延时标志和超时标志
        U1_TXresettime = U1_TXTIMEOUTCOUNT;
        TXtimememb = 0x01; //只开启发送超时计数
        UP485_TXEN;//开启485发送
        UART1_TXEIEEN;////使能发送数据寄存器空中断,由于发送数据寄存器空，直接会触发中断 

    }
}

__irq void USART1_IRQHandler(void)
{
    /*Rec_len当前接收数据长度；U1_RXflag第0位为错误标志位，
    第1位为是否接收到起始帧标志；Errcount接收帧单字节出错计数；
    Headcapfail接收帧头失败计数*/
    static uint8_t U1_RXflag,Errcount,Headcapfail;
    static uint16_t U1_TXlen;
    uint8_t buff;
    //发送中断处理部分
    if((USART1->CR1 & (USART_CR1_TE | USART_CR1_TXEIE)) == (USART_CR1_TE | USART_CR1_TXEIE) && (USART1->SR & USART_SR_TXE))//有发送且DR寄存器值已经移入shift 寄存器
    {
        USART1->DR = U1_FIFOBuff.FIFO[U1_TXlen++];
        if(U1_TXlen == U1_RxTxLen.Tran_len)//已经赋值最后一个字节
        {
            UART1_TXEIEDISEN;//关闭发送数据寄存器空中断
            UART1_TCIEEN;//使能发送完成中断
        }
        return;//由于是485，直接返回
    }
    else if((USART1->CR1 & (USART_CR1_TE | USART_CR1_TCIE)) == (USART_CR1_TE | USART_CR1_TCIE) && (USART1->SR & USART_SR_TC))//字节发送完成
    {
        UART1_TCIEDISEN;//关闭发送完成中断
        USART1->SR &= ~USART_SR_TC;//清发送完成标志
        GPIOC->ODR ^= GPIO_ODR_ODR14;
        U1_TXlen = 0x0000;
        TXtimememb = 0x00; //关闭发送超时计数(关闭发送状态退出计数)
        U1_TXresettime = 0xFFFF;
        U1_TXfin &= 0xF8;//清发送可进行标志,CRC通过标志,发送超时标志
        U1_RXfin = 0x02;
        U1_RxTxLen.Rec_len = 0x0000;
        UP485_RXEN;//开启485接收
        UART1_RXEN;//开启UART接收
        return;//由于是485，直接返回
    }

    //接收中断处理部分
    if((USART1->CR1 & (USART_CR1_RE | USART_CR1_RXNEIE)) == (USART_CR1_RE | USART_CR1_RXNEIE) && (USART1->SR & USART_SR_RXNE))//接收和接收中断使能
    {
        switch(U1_RXfin & 0x03)
        {
            case 0x00: //正在接收
            U1_RXresettime = U1_RXBYTETIMEOUTCOUNT;//更新接收状态重置计时字节间接收超时时间
            RXtimememb = 0x02;//只开启接收超时计时(开启接收状态恢复计时)
            break;
            case 0x02://检测到接收起始标志10
            U1_RXflag = 0x00;
            Errcount = 0x00;
            Headcapfail = 0x00;
            U1_RxTxLen.Rec_len = 0x0000;
            U1_RXfin = 0x00;//转为正在接收状态
            U1_RXresettime = U1_RXBYTETIMEOUTCOUNT;//为接收状态重置计时赋值字节间接收超时时间
            RXtimememb = 0x02;//只开启接收超时计时(开启接收状态恢复计时)
            break;
            case 0x01://在可靠帧确认期间又收到数据    
            buff = USART1->SR;
            buff = USART1->DR; //清可能的硬件接收或者错误标志
            U1_RXfin = 0x02;//置位接收起始标志10，重新开始接收
            RXtimememb = 0x00;//关闭UART计时，等待下一次接收后启动
            return;
            //在定时器处理可靠接收状态退出时收到数据接收中断悬起
            case 0x03:
            buff = USART1->SR;
            buff = USART1->DR; //清可能的硬件接收或者错误标志
            return;
            default:
            break;
        }
        if((USART1->SR & (uint16_t)0x000F) != (uint16_t)0x0000)//接收Frame出错
        {
            buff = USART1->DR;
            buff = USART1->SR; //清硬件错误标志
            U1_RxTxLen.Rec_len = 0x0000;
            Headcapfail = 0x00;//各值和标志清零重新接收
            if(++Errcount >= (uint8_t)5)//连续出错5次,关闭总线
            {
                U1_RXfin = 0x00;
                UART1_RXDISEN;//关闭UART接收
                U1_RXresettime = U1_RXTIMEBUSOFFCOUNT;//为接收状态重置计时赋值接收关闭时间
                RXtimememb = 0x03;//只开启接收状态重置计时
            }
            return;
        }
        else//接收Frame正确
        {
            if(Errcount-- == (uint8_t)0)//错误计数减
            {
                Errcount = 0x00;
            }

            buff = USART1->DR;
            if(!(U1_RXflag & 0x02))//判断头是否接收完
            {
                if(U1_RxTxLen.Rec_len == 0 && Headcapfail++ == (uint8_t)10)//长时间未接受到正确的帧头，关闭总线一段时间
                {
                    U1_RXfin = 0x00;
                    UART1_RXDISEN;//关闭UART接收
                    U1_RXresettime = U1_RXTIMEBUSOFFCOUNT;//为接收状态重置计时赋值接收关闭时间
                    RXtimememb = 0x03;//只开启接收状态恢复计时
                    return;
                }
                
                if(U1_FIFOBuff.FIFO[U1_RxTxLen.Rec_len++] == buff)
                {
                    if(U1_RxTxLen.Rec_len == U1_FRAME_STARTDATA+U1_FRAME_ADDDATA)
                    {
                        U1_RXflag |= 0x02;//帧头接收完毕
                    }
                }
                else
                {
                    U1_RxTxLen.Rec_len = 0x0000;
                }
            }
            else if(U1_RXfin & 0x80)
            {
                return;//等待总线静止
            }
            else if(U1_RxTxLen.Rec_len < U1_FRAME_RECEHEADLEN+U1_FRAME_RECEDATALEN)
            {
                U1_FIFOBuff.FIFO[U1_RxTxLen.Rec_len++] = buff;
            }
            else//数据接收溢出
            {
                U1_RxTxLen.Rec_len = 0x0000;
                U1_RXfin |= 0x80;//置位接收溢出标志 
            }
        }
    }
}


void UART_Init(void)
{
	//UART1相关时钟使能
	RCC->APB2ENR |= (RCC_APB2ENR_USART1EN |RCC_APB2ENR_IOPAEN |RCC_APB2ENR_AFIOEN);//使能UART1、GPIOA、AFIO时钟0x00004005
	//UART1对应IO设置
	GPIOA->BSRR = GPIO_BSRR_BR8; //对UP485EN脚输出寄存器置0
	GPIOA->BSRR = GPIO_BSRR_BS9;//TX IO输出寄存器置1
	GPIOA->BSRR = GPIO_BSRR_BS10;//RX IO输出寄存器置1，在配置输入模式后，能够启用上拉输入
	GPIOA->CRH &= ~(GPIO_CRH_MODE & 0x00000FFF);//选择配置GPIOA_Pin8、GPIOA_Pin9、GPIOA_Pin10三个脚的MODE
	GPIOA->CRH |= (GPIO_CRH_MODE8_1|GPIO_CRH_MODE9);//配置GPIOA_Pin8、GPIOA_Pin9、GPIOA_Pin10为输出最高2MHz、50MHz，和输入模式
	
	GPIOA->CRH &= ~(GPIO_CRH_CNF & 0x00000FFF);//选择配置GPIOA_Pin8、GPIOA_Pin9、GPIOA_Pin10三个脚的CNF
	GPIOA->CRH |= (GPIO_CRH_CNF9_1|GPIO_CRH_CNF10_1);//配置GPIOA_Pin8、GPIOA_Pin9、GPIOA_Pin10和MODE、ODR配合为输出推挽、复用推挽输出、上拉输入
	AFIO->MAPR &= ~(AFIO_MAPR_USART1_REMAP);//配置UART1的TX、RX为GPIOA_Pin9、GPIOA_Pin10
	
	
	//UART1设置
	USART1->BRR = ((uint16_t)468<<4)|((uint16_t)0x000C);//设置9600波特率
	USART1->GTPR = (uint16_t)0x0000;
	USART1->CR3 = (uint16_t)0x0000;
	USART1->CR2 = (uint16_t)0x0000;//主要设置1位停止位
	USART1->CR1 = (uint16_t)0x202C;//设置UART1为1位起始位、8位数据位，无奇偶校验，禁止唤醒模式，使能RXENIE中断，使能UART接收和UART发送
	UP485_RXEN;//开启485接收
}


