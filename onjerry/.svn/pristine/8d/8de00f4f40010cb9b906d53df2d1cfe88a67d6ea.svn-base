#include "Stat_control.h"
static Stu_OutNumAss OutSetNumArr[IOMemb_OutNum] = 
{5,3,7,4,2,6,0,1,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};//输出映射号
static Stu_IOmap OutMemb[IOMemb_OutNum] = 
{{0x08,1},{0x08,2},{0x08,3},{0x10,4},{0x10,5},{0x08,0},{0x10,2},{0x10,3},
{0xFF,0xFF},{0xFF,0xFF},{0xFF,0xFF},{0xFF,0xFF},{0xFF,0xFF},{0xFF,0xFF},{0xFF,0xFF},{0xFF,0xFF}};//输入输出映射号对应的实际GPIO和控制位

static void IOMemb_Write(uint8_t membnum, uint8_t setorreset)
{
    uint32_t gpio;
    uint32_t gpiobit;
    GPIO_TypeDef *pgpio;
    if(OutMemb[membnum].gpioindex == 0xFF || membnum == 0xFF)
    return;
    gpio = (uint32_t)(OutMemb[membnum].gpioindex);
    gpio = 0x40010000 | (gpio << 8);
    pgpio = (GPIO_TypeDef *)(gpio);
    gpiobit = 1 << OutMemb[membnum].bitnum;
    gpiobit <<= ((setorreset == 0)?16:0);
    pgpio ->BSRR = gpiobit;
}

static uint8_t IOMemb_Read(uint8_t membnum, uint8_t ioflag)
{
    uint32_t gpio;
    uint32_t gpiobit;
    GPIO_TypeDef *pgpio;
    gpiobit = 1 << OutMemb[membnum].bitnum;
    gpio = (uint32_t)(OutMemb[membnum].gpioindex);
    gpio = 0x40010000 | (gpio << 8);
    pgpio = (GPIO_TypeDef *)(gpio);
    if(ioflag == 0x01)//读输出IO
    {
        if(pgpio->ODR & gpiobit)//为1
        return 0x01;
        else
        return 0x00;
    }
    else//读输入IO
    {
        return 0x00;
    }
}

void LED_IOinit(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    GPIOC->CRH &= ~(GPIO_CRH_MODE & 0x0FF00000);//配置输出方式
    GPIOC->CRH |= 0x02200000;// 2MHz最高输出
    GPIOC->CRH &= ~(GPIO_CRH_CNF & 0x0FF00000);
    GPIOC->CRH |= 0x04400000;//配置为开漏输出
}
void Relay_IOinit(void)
{
    RCC->APB2ENR |= (RCC_APB2ENR_IOPCEN|RCC_APB2ENR_IOPAEN);//使能GPIOC和GPIOA时钟 0x00000014
    //GPIOA和GPIOC对应IO配置
    GPIOC->CRL &= ~(GPIO_CRL_MODE & 0x00FFFF00);//配置输出方式
    GPIOC->CRL |= 0x00222200;// 2MHz最高输出
    GPIOC->CRL &= ~(GPIO_CRL_CNF & 0x00FFFF00);//推挽输出
    GPIOA->CRL &= ~(GPIO_CRL_MODE & 0x0000FFFF);//配置输出方式
    GPIOA->CRL |= 0x00002222;// 2MHz最高输出
    GPIOA->CRL &= ~(GPIO_CRL_CNF & 0x0000FFFF);//推挽输出
}

void Coupler_IOinit(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;//使能GPIOB时钟  0x0000008
    //光耦输入配置
    GPIOB->CRL &= ~(GPIO_CRL_MODE & 0xF00000F0);//配置为输入方式
    GPIOB->CRL &= ~(GPIO_CRL_CNF & 0xF00000F0);
    GPIOB->CRL |= 0x40000040;//配置为悬浮输入
    GPIOB->CRH &= ~(GPIO_CRH_MODE & 0xFFF0F0FF);//配置为输入方式
    GPIOB->CRH &= ~(GPIO_CRH_CNF & 0xFFF0F0FF);
    GPIOB->CRH |= 0x44404044;//配置为悬浮输入
}

uint8_t OutputCmd_DataPro(uint16_t *pdata, SAMP_Stream *dirdata)//开出命令通讯设置
{
    static uint8_t errornum;
    if(*pdata == *(pdata+1))
    {
        dirdata->cmd = BIGTOLIT_U16(*pdata);
    }
    else if(errornum++ > 5)
    {
        dirdata->cmd = 0x0000;
        return 0x01;
    }
    else
    {      
        return 0x01;
    }
    return 0x00;
}

uint8_t IOMemb_DataPro(uint8_t *pdata)//开入开出控制通讯设置
{
    uint8_t num;
    for(num = 0; num < IOMemb_OutNum;)
    {
        if(pdata[num] < IOMemb_OutNum)//开出号是否符合要求
        {
            OutSetNumArr[num+1].IOnum= pdata[num];//大小端转换
        }
        if(pdata[num+1] < IOMemb_OutNum)
        {
            OutSetNumArr[num].IOnum= pdata[num+1];
        }
        num+= 2;
    }
    return 0x00;
}

void Coupler_Updata(Stu_ADSample_DATAStat *sampdata)
{
    //更新保护状态字节异步输入,只取5位状态
    sampdata->Flag_prtc6 = (uint8_t)(~((GET_STATUS(BIT_RDQ_KMMAIN)>>1)|(GET_STATUS(BIT_FLQ)>>7)|(GET_STATUS(BIT_TRIP_ACIN)>>8)|(GET_STATUS(BIT_STOP)>>9))) & 0x71;
    sampdata->Flag_run1 = (sampdata->Flag_run1 | SW_AC_IN_OFFSET) & (uint8_t)(~(GET_STATUS(BIT_SW_ACIN)>>14));

    sampdata->Flag_run1 = (sampdata->Flag_run1 & 0x67) | ((IOMemb_Read(OutSetNumArr[IONUM_KM_BRANCH1].IOnum ,1) << 4) |0x08|(IOMemb_Read(OutSetNumArr[IONUM_FZDY_BRANCH1].IOnum ,1) << 7));
    sampdata->Flag_run2 = (sampdata->Flag_run2 & 0xF6) | (IOMemb_Read(OutSetNumArr[IONUM_KM_BRANCH2].IOnum ,1)) |((IOMemb_Read(OutSetNumArr[IONUM_FZDY_BRANCH2].IOnum ,1) << 3));
    
    /*Databuff[U0_TXFIFOINDEX_RUNSTATUS1] &= (~(SW_KM_MAIN_OFFSET|SW_KM_BRANCH1_OFFSET));//清总直流输出、LINK0合闸状态
    Databuff[U0_TXFIFOINDEX_RUNSTATUS1] |= ((GET_STATUS(BIT_SW_KMMAIN)) ? 0x00 : SW_KM_MAIN_OFFSET);//更新总直流输出合闸状态
    Databuff[U0_TXFIFOINDEX_RUNSTATUS1] |= ((GET_STATUS(BIT_SW_KMBRANCH1)) ? 0x00 : SW_KM_BRANCH1_OFFSET);//更新LINK0合闸状态

    Databuff[U0_TXFIFOINDEX_RUNSTATUS2] &= (~SW_KM_BRANCH2_OFFSET);//清LINK1合闸状态
    Databuff[U0_TXFIFOINDEX_RUNSTATUS2] |= ((GET_STATUS(BIT_SW_KMBRANCH2)) ? 0x00 : SW_KM_BRANCH2_OFFSET);//更新LINK1合闸状态*/
}

uint16_t Relay_Control(Stu_ADSample_DATAStat *sampdata, SAMP_Stream* Controlbuff)
{
    uint16_t cmd = Controlbuff->cmd;
    //故障发生，处理
    if(Controlbuff->SysFault != 0x0000)//系统故障，包括通讯故障
    {
        IOMemb_Write(OutSetNumArr[IONUM_KM_BRANCH1].IOnum,0);
        IOMemb_Write(OutSetNumArr[IONUM_KM_BRANCH2].IOnum,0);
        cmd = 0x0100;//清相应命令
    }
    if((sampdata->Flag_prtc6 != 0x00)||(sampdata->Flag_prtc1 !=0x00)||(sampdata->Flag_prtc2 != 0x00)||(sampdata->Flag_prtc3 !=0x00))
    {
        //
        //充电停止类故障发生，停止充电
        if((sampdata->Flag_prtc6 & (SAMPDATABIT_KMMAINRDQSTAT |SAMPDATABIT_KMMAINTRIPSTAT | SAMPDATABIT_ACINTRIPSTAT | SAMPDATABIT_STOPSTAT )))//总运行故障
        {
            //KM_MAIN_OFF;
            IOMemb_Write(OutSetNumArr[IONUM_KM_BRANCH1].IOnum,0);
            IOMemb_Write(OutSetNumArr[IONUM_KM_BRANCH2].IOnum,0);
            cmd &= 0x01DB;//清相应命令
            Controlbuff->cmd &= 0x01DB;
        }
        if((sampdata->Flag_prtc6 & SAMPDATABIT_KMBRANCH1TRIPSTAT) ||(sampdata->Flag_prtc1 & ~SAMPDATABITMASK_MX0USTAT) ||((sampdata->Flag_prtc2 & SAMPDATABITMASK_BAT0ISTAT) !=0x00))//充电1段故障
        {
            IOMemb_Write(OutSetNumArr[IONUM_KM_BRANCH1].IOnum,0);
            cmd &= 0x01FB;//清相应命令
            Controlbuff->cmd &= 0x01FB;
        }
        if((sampdata->Flag_prtc6 & SAMPDATABIT_KMBRANCH2TRIPSTAT) ||(sampdata->Flag_prtc1 & ~SAMPDATABITMASK_MX1USTAT) ||((sampdata->Flag_prtc3 & SAMPDATABITMASK_BAT1ISTAT) !=0x00))//充电2段故障
        {
            IOMemb_Write(OutSetNumArr[IONUM_KM_BRANCH2].IOnum,0);
            cmd &= 0x01DF;//清相应命令
            Controlbuff->cmd &= 0x01DF;
        }             
    }
    
    if(((sampdata->Flag_run1 >> SAMPDATABITINDEX_LINK0USTAT) & 0x03) != 0x03)//link0 未连接
    {
        cmd &= 0x01F3;//清除闭合命令
        //Controlbuff->cmd &= 0x01F9;//同时清外部命令,但是除电子锁，使得连接之后直接锁住，充电母线和辅助电源需连接后再次受到命令才能动作
    }
        
    if(((sampdata->Flag_run2 >> SAMPDATABITINDEX_LINK1USTAT) & 0x03) != 0x03)//link1 未连接
    {
        cmd &= 0x019F;//清除闭合命令
        //Controlbuff->cmd &= 0x01CF;//同时清外部命令,但是除电子锁，使得连接之后直接锁住，充电母线和辅助电源需连接后再次受到命令才能动作
    }
    
    if(cmd & 0x0002)
    IOMemb_Write(OutSetNumArr[IONUM_FZDY_BRANCH1].IOnum,1);
    else
    IOMemb_Write(OutSetNumArr[IONUM_FZDY_BRANCH1].IOnum,0);


    if(cmd & 0x0004)
    IOMemb_Write(OutSetNumArr[IONUM_KM_BRANCH1].IOnum,1);
    else
    IOMemb_Write(OutSetNumArr[IONUM_KM_BRANCH1].IOnum,0);
  
    
    if(cmd & 0x0010)
    IOMemb_Write(OutSetNumArr[IONUM_FZDY_BRANCH2].IOnum,1);
    else
    IOMemb_Write(OutSetNumArr[IONUM_FZDY_BRANCH2].IOnum,0);

    if(cmd & 0x0020)
    IOMemb_Write(OutSetNumArr[IONUM_KM_BRANCH2].IOnum,1);
    else
    IOMemb_Write(OutSetNumArr[IONUM_KM_BRANCH2].IOnum,0); 
    
    if(cmd & 0x0008)
    IOMemb_Write(OutSetNumArr[IONUM_ELELOCK1].IOnum,1);
    else
    IOMemb_Write(OutSetNumArr[IONUM_ELELOCK1].IOnum,0);

    if(cmd & 0x0040)
    IOMemb_Write(OutSetNumArr[IONUM_ELELOCK2].IOnum,1);
    else
    IOMemb_Write(OutSetNumArr[IONUM_ELELOCK2].IOnum,0);

    if(cmd & 0x0080)
    IOMemb_Write(OutSetNumArr[IONUM_LED_RUN].IOnum,1);
    else
    IOMemb_Write(OutSetNumArr[IONUM_LED_RUN].IOnum,0);

    if(cmd & 0x0100)
    IOMemb_Write(OutSetNumArr[IONUM_LED_FAULT].IOnum,1);
    else
    IOMemb_Write(OutSetNumArr[IONUM_LED_FAULT].IOnum,0);

    if(cmd & 0x0200)
    IOMemb_Write(OutSetNumArr[IONUM_LED_ACIN].IOnum,1);
    else
    IOMemb_Write(OutSetNumArr[IONUM_LED_ACIN].IOnum,0);
    
    //系统状态正常，执行主机命令
    /*if(((Databuff[U0_TXFIFOINDEX_LINK0USTAT]>>U0_TXFIFOBITINDEX_LINK0USTAT) & 0x03) == 0x03)//link0 连接
    {
        if(cmd & 0x06)
        {
            if(cmd & 0x02)
            FZDY_BRANCH1_ON;
            else
            FZDY_BRANCH1_OFF;

            if(cmd & 0x04)
            KM_BRANCH1_ON;
            else
            KM_BRANCH1_OFF;     
        }
        else
        {
            FZDY_BRANCH1_OFF;
            KM_BRANCH1_OFF;           
        }
    }
    else
    {
        FZDY_BRANCH1_OFF;
        KM_BRANCH1_OFF;
        Controlbuff->cmd &= 0xF9;//清除闭合命令
    }
    
    if(((Databuff[U0_TXFIFOINDEX_LINK1USTAT]>>U0_TXFIFOBITINDEX_LINK1USTAT) & 0x03) == 0x03)//link1 连接
    {
        if(cmd & 0x30)
        {
            if(cmd & 0x10)
            FZDY_BRANCH2_ON;
            else
            FZDY_BRANCH2_OFF;

            if(cmd & 0x20)
            KM_BRANCH2_ON;
            else
            KM_BRANCH2_OFF;     
        }
        else
        {
            FZDY_BRANCH2_OFF;
            KM_BRANCH2_OFF;           
        }
    }
    else
    {
        FZDY_BRANCH2_OFF;
        KM_BRANCH2_OFF;
        Controlbuff->cmd &= 0xCF;//清除闭合命令
    }*/
    return 0x0000;
}

/*uint16_t Relay_Control(Stu_ADSample_DATAStat *sampdata, SAMP_Stream* Controlbuff)
{
    uint16_t cmd = Controlbuff->cmd;
    //故障发生，处理
    if((sampdata->Flag_prtc6 != 0x00)||(sampdata->Flag_prtc1 !=0x00)||(sampdata->Flag_prtc2 != 0x00)||(sampdata->Flag_prtc3 !=0x00) ||(Controlbuff->SysFault != 0x0000) || (cmd & 0x0100))
    {
        LED_FAULT_ON;
        //充电停止类故障发生，停止充电
        if((sampdata->Flag_prtc6 & (SAMPDATABIT_KMMAINRDQSTAT |SAMPDATABIT_KMMAINTRIPSTAT | SAMPDATABIT_ACINTRIPSTAT | SAMPDATABIT_STOPSTAT )) || (Controlbuff->SysFault != 0x0000))//总运行故障
        {
            //KM_MAIN_OFF;
            KM_BRANCH1_OFF;
            KM_BRANCH2_OFF;
            ELELOCK1_OFF;
            ELELOCK2_OFF;
            cmd &= 0x0113;//清相应命令
            Controlbuff->cmd &= 0x0113;
        }
        if((sampdata->Flag_prtc6 & SAMPDATABIT_KMBRANCH1TRIPSTAT) ||(sampdata->Flag_prtc1 & ~SAMPDATABITMASK_MX0USTAT) ||(sampdata->Flag_prtc2 !=0x00))//充电1段故障
        {
            KM_BRANCH1_OFF;
            ELELOCK1_OFF;
            cmd &= 0x0173;//清相应命令
            Controlbuff->cmd &= 0x0173;
        }
        if((sampdata->Flag_prtc6 & SAMPDATABIT_KMBRANCH2TRIPSTAT) ||(sampdata->Flag_prtc1 & ~SAMPDATABITMASK_MX1USTAT) ||(sampdata->Flag_prtc3 !=0x00))//充电2段故障
        {
            KM_BRANCH2_OFF;
            ELELOCK2_OFF; 
            cmd &= 0x011F;//清相应命令
            Controlbuff->cmd &= 0x011F;
        }             
    }
    else
    {       
        LED_FAULT_OFF;
    }
    
    if(((sampdata->Flag_run1 >> SAMPDATABITINDEX_LINK0USTAT) & 0x03) != 0x03)//link0 未连接
    {
        cmd &= 0x01F1;//清除闭合命令
        Controlbuff->cmd &= 0x01F9;//同时清外部命令,但是除电子锁，使得连接之后直接锁住，充电母线和辅助电源需连接后再次受到命令才能动作
    }
        
    if(((sampdata->Flag_run2 >> SAMPDATABITINDEX_LINK1USTAT) & 0x03) != 0x03)//link1 未连接
    {
        cmd &= 0x018F;//清除闭合命令
        Controlbuff->cmd &= 0x01CF;//同时清外部命令,但是除电子锁，使得连接之后直接锁住，充电母线和辅助电源需连接后再次受到命令才能动作
    }
    
    if(cmd & 0x0002)
    FZDY_BRANCH1_ON;
    else
    FZDY_BRANCH1_OFF;

    if(cmd & 0x0004)
    KM_BRANCH1_ON;
    else
    KM_BRANCH1_OFF;  
    
    if(cmd & 0x0010)
    FZDY_BRANCH2_ON;
    else
    FZDY_BRANCH2_OFF;

    if(cmd & 0x0020)
    KM_BRANCH2_ON;
    else
    KM_BRANCH2_OFF;   
    
    if(cmd & 0x0008)
    ELELOCK1_ON;
    else
    ELELOCK1_OFF;

    if(cmd & 0x0040)
    ELELOCK2_ON;
    else
    ELELOCK2_OFF;

    if(cmd & 0x0080)
    LED_RUN_ON;
    else
    LED_RUN_OFF;
    
    //系统状态正常，执行主机命令
    if(((Databuff[U0_TXFIFOINDEX_LINK0USTAT]>>U0_TXFIFOBITINDEX_LINK0USTAT) & 0x03) == 0x03)//link0 连接
    {
        if(cmd & 0x06)
        {
            if(cmd & 0x02)
            FZDY_BRANCH1_ON;
            else
            FZDY_BRANCH1_OFF;

            if(cmd & 0x04)
            KM_BRANCH1_ON;
            else
            KM_BRANCH1_OFF;     
        }
        else
        {
            FZDY_BRANCH1_OFF;
            KM_BRANCH1_OFF;           
        }
    }
    else
    {
        FZDY_BRANCH1_OFF;
        KM_BRANCH1_OFF;
        Controlbuff->cmd &= 0xF9;//清除闭合命令
    }
    
    if(((Databuff[U0_TXFIFOINDEX_LINK1USTAT]>>U0_TXFIFOBITINDEX_LINK1USTAT) & 0x03) == 0x03)//link1 连接
    {
        if(cmd & 0x30)
        {
            if(cmd & 0x10)
            FZDY_BRANCH2_ON;
            else
            FZDY_BRANCH2_OFF;

            if(cmd & 0x20)
            KM_BRANCH2_ON;
            else
            KM_BRANCH2_OFF;     
        }
        else
        {
            FZDY_BRANCH2_OFF;
            KM_BRANCH2_OFF;           
        }
    }
    else
    {
        FZDY_BRANCH2_OFF;
        KM_BRANCH2_OFF;
        Controlbuff->cmd &= 0xCF;//清除闭合命令
    }*/
/*    return 0x0000;
}*/


