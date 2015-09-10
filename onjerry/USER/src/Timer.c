#include "Timer.h"
#include "UART.h"
#include "ADS795x.h"
#include "AD_Sample.h"
#include "Stat_control.h"
#include "BMSCom_GB.h"
#include "BMSCom_SAE.h"

/*RXtimememb为接收过程使用定时器计数的成员，
bit0表示接收状态退出功能是否加入计数，
bit1表示接收状态恢复计时，
*/
/*TXtimememb为发送过程使用定时器计数的成员，
bit0表示发送退出计时，
bit1表示接收完到发送开始的延时定时
*/
/*Systimememb为当前通道定时采样定时器计数的成员，
bit0表示AD定时采样使能
bit1表示通讯中断计数使能
注意:由于每位定时的功能不同，需要在外部读取写回
操作时加锁(关全局中断或者是定时器1中断)
*/
uint8_t RXtimememb = 0x00;
uint8_t TXtimememb = 0x00;
uint8_t Systimememb = 0x00;
//uint16_t U0_RXquittime = 0xFFFF;//接收状态退出计时
uint16_t U1_RXresettime= 0xFFFF;//接收状态重置计时
uint16_t U1_TXresettime = 0xFFFF;//发送状态重置计时
//uint16_t U1_TXstarttime = U1_TXSTARTCOUNT;//发送状态开始计时
uint16_t ADSMstartcont = SAM_CHANNEL_TIMERCOUNT;//AD采样计数
uint16_t COMMbreaktime = U1_COMMBREAKCOUNT;//通讯中断计时

//Stu_CAN_TimerArr CAN_RXTimer[CAN_PERIPH_NUM][CAN_RXTIMERARR_LENGTH];
//Stu_CAN_TimerArr CAN_TXTimer[CAN_PERIPH_NUM][CAN_TXTIMERARR_LENGTH];

//该工程中TIMER1中断为可抢占的最高级中断，所以不需要对全局性的变量修改加锁
__irq void TIM1_UP_IRQHandler(void)
{
    static uint32_t predata=0xFFFFFFFF;
    static uint8_t cont;
    static uint16_t cont1;
    TIM1->SR &= 0xFFFE;
    if(cont1++ >= (uint16_t)500)
    {
        GPIOC->ODR ^= GPIO_ODR_ODR13;        
        cont1 = (uint16_t)0;
    }
    if((Systimememb & 0x01) == 0x01)//AD采样计数控制
    {
        ADSMstartcont--;
        if(ADSMstartcont > SAM_CHANNEL_TIMERCOUNT)//超过计数部分算延时
        {
            ;
        }
        else if(ADSMstartcont != 0x0000)
        {
            ADS7953_SMstart();
        }
        else//当前这组数据采样已经结束，开始数据处理
        {
            if((predata - Samp_Databuff) < 100 * AD_DATA_EXPAND ||(Samp_Databuff-predata) < 100 * AD_DATA_EXPAND)//上一次采样值与当前采样值比较，可保证切换通道后电压值稳定
            {
                Samp_DataArr[Samp_ADdata_Stream.now].ADdata = Samp_Databuff / SAM_CHANNEL_TIMERCOUNT + (uint32_t)((Samp_Databuff % SAM_CHANNEL_TIMERCOUNT) / (uint32_t)(SAM_CHANNEL_TIMERCOUNT/2));
                Systimememb &= 0xFE;
                ADSMstartcont = 0x0000;
                Samp_Databuff = 0x00000000;
                predata = 0xFFFFFFFF;
                cont = 0x00;
            
            }
            else//当前采样值还没有稳定，继续下一次采样
            {
                predata =  Samp_Databuff;
                ADSMstartcont = SAM_CHANNEL_TIMERCOUNT;
                Samp_Databuff = 0x00000000;
                cont++;
                ADS7953_SMstart();
            }
        }
    }

    if(((Systimememb & 0x02) == 0x02) && (COMMbreaktime-- == 0x0000))//通讯中断计数
    {
         Samp_ADdata_Stream.SysFault |= 0x0001;
         Systimememb &= 0xFD;
         COMMbreaktime = U1_COMMBREAKCOUNT;
    }
    
    
}

__irq void TIM2_IRQHandler(void)//用于CAN通讯部分的定时
{
    uint8_t cannum;
    TIM2->SR &= 0xFFFE;
    for(cannum = 0; cannum < CAN_PERIPH_NUM; cannum++)
    {
        if(BMSCom_NC[cannum].NowStage != BMSCOM_STAGE_TIMEOUT && BMSCom_NC[cannum].NowStage != BMSCOM_STAGE_IDLE)
        {
            //BMSCom_GBStageRxTimerProc[BMSCom_NC[cannum].NowStage](cannum);
            BMSCom_GBStageTxTimerProc[BMSCom_NC[cannum].NowStage](cannum);
        }
        if(BMSCom_MC_Rsp[cannum].multitranstatus & 0x03)//多包传输已经使能
        {
            if((BMSCom_MC_Rsp[cannum].trantimercotrl & 0x07) == 0x06 && !(BMSCom_MC_Rsp[cannum].timerstatus & 0x02))//发送定时，trantimercotrl只在主程序中修改，作为timerstatus的锁
            {
                if(BMSCom_MC_Rsp[cannum].timecount-- == 0)//时间到
                {
                    BMSCom_MC_Rsp[cannum].timecount = 0;
                    BMSCom_MC_Rsp[cannum].timerstatus = 0x02;
                }
            }
            else if((BMSCom_MC_Rsp[cannum].trantimercotrl & 0x07) == 0x05 && !(BMSCom_MC_Rsp[cannum].timerstatus & 0x01))//接收超时定时，trantimercotrl只在主程序中修改，作为timerstatus的锁
            {
                /*if(BMSCom_MC_Rsp[cannum].timecount-- == 0)//超时
                {
                    BMSCom_MC_Rsp[cannum].timecount = 0;
                    BMSCom_MC_Rsp[cannum].abortreason = SAE_FRAME_TP_ABORTREASON_TIMEOUT;
                    BMSCom_MC_Rsp[cannum].timerstatus = 0x01;
                }*/
            }
        }
    }
}

__irq void TIM3_IRQHandler(void)//用于UART通讯部分的定时
{ 
    TIM3->SR &= 0xFFFE;
    switch(RXtimememb)
    {
        case 0x00://无接收状态下的计数
        break;
        case 0x02://字节间超时计数状态
        if(((U1_RXfin & 0x03) == 0x00) && (U1_RXresettime-- == 0x0000))//总线接收关闭超时计数时间到或者字节间接收超时计数时间到
        {
            U1_RXfin |= 0x01;//置位字节间超时标志,进入帧间超时计数状态，判断接收的数据帧是否为可靠帧
            U1_RXresettime = U1_RXQUITCOUNT;//为接收状态重置计时赋值接收退出时间
            RXtimememb = 0x01;//开启接收帧可靠确认计数(开启接收状态退出计时)
        }
        return;
        case 0x01://接收状态退出计时
         if(((U1_RXfin & 0x03) == 0x01) && (U1_RXresettime-- == 0x0000))//接收的数据帧为可靠帧
        {
            U1_RXfin |= 0x02;//置位可靠接收到数据帧标志
            RXtimememb = 0x00;//关闭了接收帧可靠确认计时
            U1_RXresettime = 0xFFFF;//接收重置时间置最大
            UART1_RXDISEN;//关闭UART接收
        }
        return;
        case 0x03://接收错误总线关闭到恢复计时
        if(((U1_RXfin & 0x03) == 0x00) && (U1_RXresettime-- == 0x0000))
        {
            RXtimememb &= 0xFC;
            U1_RXfin = 0x02;//转化到重新开始接收状态
            UART1_RXEN;//开启接收
            U1_RXresettime = 0xFFFF;//接收重置时间置最大
        }
        return;
        default:
        break;
    }
   
    switch(TXtimememb)//数据帧发送超时功能加入计数
    {
        case 0x00://无发送状态下的计数
        break;
        case 0x01:
        if(((U1_TXfin & 0x03) == 0x01) && (U1_TXresettime -- == 0x0000))//数据帧发送超时计数时间到
        {
            TXtimememb = 0x00; 
            U1_TXfin |= 0x02;//置位发送超时标志，该标志对发送无影响，只是一个告警
            U1_TXresettime = U1_TXTIMEOUTCOUNT;
        }
        return;
        case 0x02:
        if(((U1_TXfin & 0x03) == 0x01) && (U1_TXresettime -- == 0x0000))//数据帧开始发送延时时间到
        {
            TXtimememb = 0x00; 
            U1_TXfin |= 0x08;//置位开始发送使能标志;
            U1_TXresettime = U1_TXSTARTCOUNT;
        }
        return;
        default:
        break;
    }
}

void Timer_Init(void)
{
    
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;//使能Timer1时钟
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;//使能Timer2时钟
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;//使能Timer3时钟
    DBGMCU->CR |= (DBGMCU_CR_DBG_TIM1_STOP |DBGMCU_CR_DBG_TIM3_STOP |DBGMCU_CR_DBG_TIM3_STOP);//调试时停止Timer1,Timer2,Timer3运行
    
    //在计数频率为72MHz的情况下，配置使能定时器1更新中断，且周期为1ms
    TIM1->ARR = (uint16_t)720; 
    TIM1->PSC = (uint16_t)10;
    TIM1->RCR = (uint16_t)10;

    //在计数频率为72MHz的情况下，配置使能定时器2更新中断，且周期为2.5ms
    TIM2->ARR = (uint16_t)720; 
    TIM2->PSC = (uint16_t)250;

    //在计数频率为72MHz的情况下，配置使能定时器3更新中断，且周期为500us
    TIM3->ARR = (uint16_t)360; 
    TIM3->PSC = (uint16_t)100;
    
    TIM1->DIER = (uint16_t)0x0001;//使能更新事件中断
    TIM1->CR1 = (uint16_t)0x0095;//配置减计数模式、边缘对齐模式、使能自动重载缓冲模式使能更新功能、更新请求源触发中断只为计数上/下溢出、使能定时器
    
    TIM2->DIER = (uint16_t)0x0001;//使能更新事件中断
    TIM2->CR1 = (uint16_t)0x0095;//配置减计数模式、边缘对齐模式、使能自动重载缓冲模式使能更新功能、更新请求源触发中断只为计数上/下溢出、使能定时器
    
    TIM3->DIER = (uint16_t)0x0001;//使能更新事件中断
    TIM3->CR1 = (uint16_t)0x0095;//配置减计数模式、边缘对齐模式、使能自动重载缓冲模式使能更新功能、更新请求源触发中断只为计数上/下溢出、使能定时器
}



