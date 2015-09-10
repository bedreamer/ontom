#include "ADS795x.h"
#include "AD_Sample.h"

uint32_t Samp_Databuff;
/*__irq void SPI1_IRQHandler(void)
{
    uint16_t flag,temp = 0xcccc;
    static uint8_t addcount=10;
    flag = SPI1->SR;
    if(flag & ((uint16_t)(SPI_SR_BSY|SPI_SR_MODF |SPI_SR_OVR |SPI_SR_CRCERR)))//错误和忙处理
    {
        if(flag & ((uint16_t)(SPI_SR_MODF)))//MODE FAULT
        {
            SPI1->CR1 |=(uint16_t)0x0044;//置位MSTR、SPE
            return ;
        }
        else if(flag & ((uint16_t)(SPI_SR_OVR)))//OVERRUN ERROR
        {
            temp = SPI1->DR;
            return ;
        }
        else if(flag & ((uint16_t)(SPI_SR_CRCERR)))//CRC ERROR
        {
            return ;
        }
        else if(flag & ((uint16_t)(SPI_SR_BSY)))//读通讯忙标志，一般不会进入
        {
            while(SPI1->SR & (uint16_t)(SPI_SR_BSY));//等待退出忙
        }
    }
    NSS_SET;
    temp = SPI1->DR;//读取收到的数据
    if(addcount-- != 0x00)
    {
        NSS_RSET;//准备发送下一帧数据
        SPI1->DR = 0x0000;
    }
    else
    {
        SPI_DISENRXIN;//关闭SPI接收中断
        ADS7953_CHsel(AD_CH2);
        //ADS7953_SMstart();
        addcount = 10;
    }

    
}*/


__irq void SPI1_IRQHandler(void)
{
    uint16_t temp = 0xcccc,temp1;
    static uint8_t addcount=(uint8_t)(SAM_EVERYDATA_COUNT+2);
    static uint16_t max,min;
    static uint32_t addnum;
    
    NSS_SET;
    temp = SPI1->SR;
    if(temp & ((uint16_t)(SPI_SR_BSY|SPI_SR_MODF |SPI_SR_OVR |SPI_SR_CRCERR)))//错误和忙处理
    {
        if(temp & ((uint16_t)(SPI_SR_MODF)))//MODE FAULT
        {
            SPI1->CR1 |=(uint16_t)0x0044;//置位MSTR、SPE
            goto Lable0_SPI1_IRQHandler;
        }
        else if(temp & ((uint16_t)(SPI_SR_OVR)))//OVERRUN ERROR
        {
            temp = SPI1->DR;
            goto Lable0_SPI1_IRQHandler;
        }
        else if(temp & ((uint16_t)(SPI_SR_CRCERR)))//CRC ERROR
        {
            goto Lable0_SPI1_IRQHandler;
        }
        else if(temp & ((uint16_t)(SPI_SR_BSY)))//读通讯忙标志，一般不会进入
        {
            while(SPI1->SR & (uint16_t)(SPI_SR_BSY));//等待退出忙
        }
    }
    temp1 = SPI1->DR;
    temp = (temp1)&0x0FFF;//读取收到的数据
    temp *= AD_DATA_EXPAND;
    switch(addcount)
    {
        case SAM_EVERYDATA_COUNT+2://给min,max赋初值
            min = temp;
            max = temp;
            goto Lable0_SPI1_IRQHandler;
        case SAM_EVERYDATA_COUNT+1:
            if(temp > max)
            {
                max = temp;
            }
            else
            {
                min = temp;
            }
            goto Lable0_SPI1_IRQHandler;
        default:
        break;
    }

    if(temp > max)
    {
        addnum += max; 
        max = temp;   
    }
    else if(temp < min)
    {
        addnum += min;
        min = temp;
    }
    else
    {
        addnum += temp;
    }
    
    if(addcount == (uint8_t)0x01)//已处理完10组数据
    {
        SPI_DISENRXIN;//关闭SPI接收中断
        max = 0x0000;
        min = 0x0000;
        addcount = (uint8_t)(SAM_EVERYDATA_COUNT+2);
        Samp_Databuff += (uint32_t)(addnum/SAM_EVERYDATA_COUNT + (uint32_t)((addnum % SAM_EVERYDATA_COUNT)/(uint32_t)(SAM_EVERYDATA_COUNT/2)));
        //Samp_DataArr[Samp_ADdata_Stream.now].ADdata = addnum/SAM_EVERYDATA_COUNT + (uint16_t)(!(addnum % SAM_EVERYDATA_COUNT)<(uint16_t)(SAM_EVERYDATA_COUNT/2));
        addnum = (uint32_t)0x00000000;
    }
    else//继续接收下一AD采样数据
    {
        Lable0_SPI1_IRQHandler:  
        NSS_RSET;//准备发送下一帧数据
        addcount--;
        SPI1->DR = AD_MODE_CONTI;
    }
}

void SPI_Init(void)
{
    //SPI1相关外设时钟使能
    RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN |RCC_APB2ENR_IOPAEN |RCC_APB2ENR_AFIOEN);//使能SPI1、GPIOA、AFIO时钟0x00001005
    //配置SPI1对应的GPIO
    GPIOA->BSRR = GPIO_BSRR_BS4;//NSS初始化输出为高电平
    GPIOA->BSRR = GPIO_BSRR_BR5;//SCK初始化输出为低电平
    //GPIOA->ODR |= GPIO_ODR_ODR6;
    GPIOA->CRL &= ~(GPIO_CRL_MODE & 0xFFFF0000);
    GPIOA->CRL |= (GPIO_CRL_MODE4|GPIO_CRL_MODE5|GPIO_CRL_MODE7);//设置SPI1的NSS、SCK、MOSI引脚为50MHz输出模式，MISO为输入模式
    GPIOA->CRL &=~(GPIO_CRL_CNF & 0xFFFF0000);
    GPIOA->CRL |= (GPIO_CRL_CNF5_1|GPIO_CRL_CNF6_0|GPIO_CRL_CNF7_1);//设置SPI1的NSS为GPIO推挽输出，SCK、MOSI为复用推挽输出模式，MISO为悬浮输入

    //SPI1配置
    SPI1->CR1 &= ~((uint16_t)0x0007 << 3);
    SPI1->CR1 |= ((uint16_t)0x0002 << 3);//设置SPI1 速率为APB/8，为9MHz
    SPI1->CR1 &= ~((uint16_t)0x0003);
    SPI1->CR1 |= (uint16_t)0x0000;//设置SPI1的脉冲序列的极性CPOL、CPAH
    SPI1->CR1 &= ~((uint16_t)0x0001<<11);
    SPI1->CR1 |= ((uint16_t)0x0001<<11);//设置数据为16位传输
    SPI1->CR1 &= ~((uint16_t)0x0001<<7);
    SPI1->CR1 |= ((uint16_t)0x0000<<7);//设置传输MSB第一位
    SPI1->CR1 &= ~((uint16_t)0x0003<<8);
    SPI1->CR1 |= ((uint16_t)0x0003<<8);//设置NSS信号为Software模式，且SSI置1
    SPI1->CR1 &= ~((uint16_t)0x00F4<<10);
    SPI1->CR1 |= ((uint16_t)0x0000<<14);//设置全双工模式，发送时可以接收数据,不使用CRC
    SPI1->CR2 = (uint16_t)0x0000;//禁止DMA、发送接收中断、SSOE
    SPI1->CR1 |=(uint16_t)0x0044;//置位MSTR、SPE，配置SPI1为主机
}


uint16_t SPI_TRAN(uint16_t cmd)//未中断情况下的传输
{
    uint16_t  temp;
    NSS_SET;
    __nop(); 
    NSS_RSET;
    __nop();
    temp = SPI1->SR;
    if(temp & ((uint16_t)(SPI_SR_BSY|SPI_SR_MODF |SPI_SR_OVR |SPI_SR_CRCERR)))//错误和忙处理
    {
        if(temp & ((uint16_t)(SPI_SR_MODF)))//MODE FAULT
        {
            SPI1->CR1 |=(uint16_t)0x0044;//置位MSTR、SPE
            return 0x0001;
        }
        else if(temp & ((uint16_t)(SPI_SR_OVR)))//OVERRUN ERROR
        {
            temp = SPI1->DR;
            return 0x0002;
        }
        else if(temp & ((uint16_t)(SPI_SR_CRCERR)))//CRC ERROR
        {
            return 0x0003;
        } 
        else if(temp & ((uint16_t)(SPI_SR_BSY)))//读通讯忙标志，一般不会进入
        {
            while(SPI1->SR & (uint16_t)(SPI_SR_BSY));//等待退出忙
        }
    }
    
    if((temp& (uint16_t)(SPI_SR_TXE )) == 0x0002)
    {
        SPI1->DR = cmd;
        //检测发送BUFFER是否为空、通信忙标志、接收BUFFER是否不为空
        while((SPI1->SR & (uint16_t)(SPI_SR_TXE | SPI_SR_BSY | SPI_SR_RXNE)) != 0x0003);
        temp = SPI1->DR;
        //接收BUFFER是否为空
        while((SPI1->SR & (uint16_t)(SPI_SR_TXE | SPI_SR_BSY | SPI_SR_RXNE)) != 0x0002);
        
    } 
    NSS_SET;
    __nop();
    return 0x0000;
    
}

/*ADS7953功能:FRAM1:输入采样通道CH1，FRAM2:从当前通道切换到CH1并在之后选中，
FRAM3:采样CH1，FRAM4:输出采样值。每一帧都会处理这4帧描述功能，只是对应通
道与之前帧指定的有关，形成流水线。上电时CH0已经选中*/


uint16_t  ADS7953_CHsel(uint16_t chan)//AD_USER0的方式下，改变通道流程
{
    uint16_t temp=0x0000;
    
    if(SPI1->CR2 & 0x0040)//接收中断未关闭的情况(SPI1中断数据还未处理完毕)下退出
    {
        return 0x0004;
    }

    temp |= SPI_TRAN(AD_USER0(chan));//初始化AD选用USER0模式，选择通道chan，采样通道pre-chan，但得到AD值无效(刚上电时)或者是上一次采样值
    temp |= SPI_TRAN(AD_MODE_CONTI);//继续一次SPI操作，将会得到通道0的AD值，选中通道chan
    temp |= SPI_TRAN(AD_MODE_CONTI);//继续一次SPI操作，将会得到通道0的AD值，转化通道chan
    SPI_TRAN(AD_MODE_CONTI);//继续一次SPI操作，将会得到通道chan的AD值
    if(temp != 0x0000)//通道切换失败
    {
        return 0x0005;
    }
    
    temp = SPI1->SR;
    if(temp & ((uint16_t)(SPI_SR_MODF |SPI_SR_OVR |SPI_SR_CRCERR)))//接收错误
    {
        if(temp & ((uint16_t)(SPI_SR_MODF)))//MODE FAULT
        {
            SPI1->CR1 |=(uint16_t)0x0044;//置位MSTR、SPE
            return 0x0001;
        }
        else if(temp & ((uint16_t)(SPI_SR_OVR)))//OVERRUN ERROR
        {
            temp = SPI1->DR;
            return 0x0002;
        }
        else if(temp & ((uint16_t)(SPI_SR_CRCERR)))//CRC ERROR
        {
            return 0x0003;
        } 
    }
    
    return 0x0000;
}

uint16_t  ADS7953_CHflush(uint8_t num)//冲刷当前选中通道转换AD值,num为冲刷次数
{
    uint16_t temp=0x0000;
    if(SPI1->CR2 & 0x0040)//接收中断未关闭的情况(SPI1中断数据还未处理完毕)下退出
    {
        return 0x0004;
    }
    for(;num != (uint8_t)0x00;num--)
    {
        temp |= SPI_TRAN(AD_MODE_CONTI);//继续一次SPI操作，冲刷一次选中通道chan AD采样值  
    }

    if(temp != 0x0000)//数据未成功冲刷到num次数
    {
        return 0x0005;
    }
    
    temp = SPI1->SR;
    if(temp & ((uint16_t)(SPI_SR_MODF |SPI_SR_OVR |SPI_SR_CRCERR)))//接收错误
    {
        if(temp& ((uint16_t)(SPI_SR_MODF)))//MODE FAULT
        {
            SPI1->CR1 |=(uint16_t)0x0044;//置位MSTR、SPE
            return 0x0001;
        }
        else if(temp& ((uint16_t)(SPI_SR_OVR)))//OVERRUN ERROR
        {
            temp = SPI1->DR;
            return 0x0002;
        }
        else if(temp& ((uint16_t)(SPI_SR_CRCERR)))//CRC ERROR
        {
            return 0x0003;
        } 
    }
    
    return 0x0000;
}

uint16_t  ADS7953_SMstart(void)//启动AD中断采样
{
    SPI_ENRXIN;//开SPI1接收中断
    NSS_SET;
    __nop();
    NSS_RSET;
    __nop();
    SPI1->DR = AD_MODE_CONTI;//启动SPI1传输
    return 0x0000;
}



void ADS7953_Init(void)
{
    SPI_Init();
    ADS7953_CHsel(Samp_DataArr[Samp_ADdata_Stream.now].Ain);//选择通道0,主要用于初始化配置AD模式
    
}



/*static void delay(void)
{
    uint32_t temp1;
    for(temp1 = 0x0000FFFF;temp1!= 0x00000000;temp1--)
    {
        __nop();
        __nop();
        __nop();
        __nop();
    }
}

uint16_t spi_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->BSRR = GPIO_BSRR_BS4;//NSS初始化输出为高电平
    GPIOA->BSRR = GPIO_BSRR_BR5;//SCK初始化输出为低电平
    GPIOA->ODR |= GPIO_ODR_ODR6;
    GPIOA->CRL &= ~(GPIO_CRL_MODE & 0xFFFF0000);
    GPIOA->CRL |= (GPIO_CRL_MODE4|GPIO_CRL_MODE5|GPIO_CRL_MODE7);//设置SPI1的NSS、SCK、MOSI引脚为50MHz输出模式，MISO为输入模式
    GPIOA->CRL &=~(GPIO_CRL_CNF & 0xFFFF0000);
    GPIOA->CRL |= GPIO_CRL_CNF6_1;//设置SPI1的NSS为GPIO推挽输出，SCK、MOSI为复用推挽输出模式，MISO为悬浮输入
}

uint16_t spi_tran(uint16_t cmd)
{
    uint8_t num;
    uint16_t ret;
    NSS_SET;
    SCK_RSET;
    delay();
    NSS_RSET;
    delay();
    
    for(num=16,ret=0x0000;num!=0x00;num--)
    {
        if(GPIOA->IDR & 0x0040)
        {
            ret<<=1;
            ret |= 0x0001;
        }
        else
        {
            ret<<=1;
        }

        ((cmd & 0x8000) == 0x8000)?MOSI_SET : MOSI_RSET;
        cmd <<= 1;
        delay();
        SCK_SET;
        delay();
        SCK_RSET;
        
    }
    
    delay();
    NSS_SET;
    
}*/




