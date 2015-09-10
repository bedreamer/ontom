#include "Timer.h"
#include "UART.h"
#include "ADS795x.h"
#include "AD_Sample.h"
#include "Stat_control.h"
#include "BMSCom_GB.h"
#include "BMSCom_SAE.h"

/*RXtimemembΪ���չ���ʹ�ö�ʱ�������ĳ�Ա��
bit0��ʾ����״̬�˳������Ƿ���������
bit1��ʾ����״̬�ָ���ʱ��
*/
/*TXtimemembΪ���͹���ʹ�ö�ʱ�������ĳ�Ա��
bit0��ʾ�����˳���ʱ��
bit1��ʾ�����굽���Ϳ�ʼ����ʱ��ʱ
*/
/*SystimemembΪ��ǰͨ����ʱ������ʱ�������ĳ�Ա��
bit0��ʾAD��ʱ����ʹ��
bit1��ʾͨѶ�жϼ���ʹ��
ע��:����ÿλ��ʱ�Ĺ��ܲ�ͬ����Ҫ���ⲿ��ȡд��
����ʱ����(��ȫ���жϻ����Ƕ�ʱ��1�ж�)
*/
uint8_t RXtimememb = 0x00;
uint8_t TXtimememb = 0x00;
uint8_t Systimememb = 0x00;
//uint16_t U0_RXquittime = 0xFFFF;//����״̬�˳���ʱ
uint16_t U1_RXresettime= 0xFFFF;//����״̬���ü�ʱ
uint16_t U1_TXresettime = 0xFFFF;//����״̬���ü�ʱ
//uint16_t U1_TXstarttime = U1_TXSTARTCOUNT;//����״̬��ʼ��ʱ
uint16_t ADSMstartcont = SAM_CHANNEL_TIMERCOUNT;//AD��������
uint16_t COMMbreaktime = U1_COMMBREAKCOUNT;//ͨѶ�жϼ�ʱ

//Stu_CAN_TimerArr CAN_RXTimer[CAN_PERIPH_NUM][CAN_RXTIMERARR_LENGTH];
//Stu_CAN_TimerArr CAN_TXTimer[CAN_PERIPH_NUM][CAN_TXTIMERARR_LENGTH];

//�ù�����TIMER1�ж�Ϊ����ռ����߼��жϣ����Բ���Ҫ��ȫ���Եı����޸ļ���
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
    if((Systimememb & 0x01) == 0x01)//AD������������
    {
        ADSMstartcont--;
        if(ADSMstartcont > SAM_CHANNEL_TIMERCOUNT)//����������������ʱ
        {
            ;
        }
        else if(ADSMstartcont != 0x0000)
        {
            ADS7953_SMstart();
        }
        else//��ǰ�������ݲ����Ѿ���������ʼ���ݴ���
        {
            if((predata - Samp_Databuff) < 100 * AD_DATA_EXPAND ||(Samp_Databuff-predata) < 100 * AD_DATA_EXPAND)//��һ�β���ֵ�뵱ǰ����ֵ�Ƚϣ��ɱ�֤�л�ͨ�����ѹֵ�ȶ�
            {
                Samp_DataArr[Samp_ADdata_Stream.now].ADdata = Samp_Databuff / SAM_CHANNEL_TIMERCOUNT + (uint32_t)((Samp_Databuff % SAM_CHANNEL_TIMERCOUNT) / (uint32_t)(SAM_CHANNEL_TIMERCOUNT/2));
                Systimememb &= 0xFE;
                ADSMstartcont = 0x0000;
                Samp_Databuff = 0x00000000;
                predata = 0xFFFFFFFF;
                cont = 0x00;
            
            }
            else//��ǰ����ֵ��û���ȶ���������һ�β���
            {
                predata =  Samp_Databuff;
                ADSMstartcont = SAM_CHANNEL_TIMERCOUNT;
                Samp_Databuff = 0x00000000;
                cont++;
                ADS7953_SMstart();
            }
        }
    }

    if(((Systimememb & 0x02) == 0x02) && (COMMbreaktime-- == 0x0000))//ͨѶ�жϼ���
    {
         Samp_ADdata_Stream.SysFault |= 0x0001;
         Systimememb &= 0xFD;
         COMMbreaktime = U1_COMMBREAKCOUNT;
    }
    
    
}

__irq void TIM2_IRQHandler(void)//����CANͨѶ���ֵĶ�ʱ
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
        if(BMSCom_MC_Rsp[cannum].multitranstatus & 0x03)//��������Ѿ�ʹ��
        {
            if((BMSCom_MC_Rsp[cannum].trantimercotrl & 0x07) == 0x06 && !(BMSCom_MC_Rsp[cannum].timerstatus & 0x02))//���Ͷ�ʱ��trantimercotrlֻ�����������޸ģ���Ϊtimerstatus����
            {
                if(BMSCom_MC_Rsp[cannum].timecount-- == 0)//ʱ�䵽
                {
                    BMSCom_MC_Rsp[cannum].timecount = 0;
                    BMSCom_MC_Rsp[cannum].timerstatus = 0x02;
                }
            }
            else if((BMSCom_MC_Rsp[cannum].trantimercotrl & 0x07) == 0x05 && !(BMSCom_MC_Rsp[cannum].timerstatus & 0x01))//���ճ�ʱ��ʱ��trantimercotrlֻ�����������޸ģ���Ϊtimerstatus����
            {
                /*if(BMSCom_MC_Rsp[cannum].timecount-- == 0)//��ʱ
                {
                    BMSCom_MC_Rsp[cannum].timecount = 0;
                    BMSCom_MC_Rsp[cannum].abortreason = SAE_FRAME_TP_ABORTREASON_TIMEOUT;
                    BMSCom_MC_Rsp[cannum].timerstatus = 0x01;
                }*/
            }
        }
    }
}

__irq void TIM3_IRQHandler(void)//����UARTͨѶ���ֵĶ�ʱ
{ 
    TIM3->SR &= 0xFFFE;
    switch(RXtimememb)
    {
        case 0x00://�޽���״̬�µļ���
        break;
        case 0x02://�ֽڼ䳬ʱ����״̬
        if(((U1_RXfin & 0x03) == 0x00) && (U1_RXresettime-- == 0x0000))//���߽��չرճ�ʱ����ʱ�䵽�����ֽڼ���ճ�ʱ����ʱ�䵽
        {
            U1_RXfin |= 0x01;//��λ�ֽڼ䳬ʱ��־,����֡�䳬ʱ����״̬���жϽ��յ�����֡�Ƿ�Ϊ�ɿ�֡
            U1_RXresettime = U1_RXQUITCOUNT;//Ϊ����״̬���ü�ʱ��ֵ�����˳�ʱ��
            RXtimememb = 0x01;//��������֡�ɿ�ȷ�ϼ���(��������״̬�˳���ʱ)
        }
        return;
        case 0x01://����״̬�˳���ʱ
         if(((U1_RXfin & 0x03) == 0x01) && (U1_RXresettime-- == 0x0000))//���յ�����֡Ϊ�ɿ�֡
        {
            U1_RXfin |= 0x02;//��λ�ɿ����յ�����֡��־
            RXtimememb = 0x00;//�ر��˽���֡�ɿ�ȷ�ϼ�ʱ
            U1_RXresettime = 0xFFFF;//��������ʱ�������
            UART1_RXDISEN;//�ر�UART����
        }
        return;
        case 0x03://���մ������߹رյ��ָ���ʱ
        if(((U1_RXfin & 0x03) == 0x00) && (U1_RXresettime-- == 0x0000))
        {
            RXtimememb &= 0xFC;
            U1_RXfin = 0x02;//ת�������¿�ʼ����״̬
            UART1_RXEN;//��������
            U1_RXresettime = 0xFFFF;//��������ʱ�������
        }
        return;
        default:
        break;
    }
   
    switch(TXtimememb)//����֡���ͳ�ʱ���ܼ������
    {
        case 0x00://�޷���״̬�µļ���
        break;
        case 0x01:
        if(((U1_TXfin & 0x03) == 0x01) && (U1_TXresettime -- == 0x0000))//����֡���ͳ�ʱ����ʱ�䵽
        {
            TXtimememb = 0x00; 
            U1_TXfin |= 0x02;//��λ���ͳ�ʱ��־���ñ�־�Է�����Ӱ�죬ֻ��һ���澯
            U1_TXresettime = U1_TXTIMEOUTCOUNT;
        }
        return;
        case 0x02:
        if(((U1_TXfin & 0x03) == 0x01) && (U1_TXresettime -- == 0x0000))//����֡��ʼ������ʱʱ�䵽
        {
            TXtimememb = 0x00; 
            U1_TXfin |= 0x08;//��λ��ʼ����ʹ�ܱ�־;
            U1_TXresettime = U1_TXSTARTCOUNT;
        }
        return;
        default:
        break;
    }
}

void Timer_Init(void)
{
    
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;//ʹ��Timer1ʱ��
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;//ʹ��Timer2ʱ��
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;//ʹ��Timer3ʱ��
    DBGMCU->CR |= (DBGMCU_CR_DBG_TIM1_STOP |DBGMCU_CR_DBG_TIM3_STOP |DBGMCU_CR_DBG_TIM3_STOP);//����ʱֹͣTimer1,Timer2,Timer3����
    
    //�ڼ���Ƶ��Ϊ72MHz������£�����ʹ�ܶ�ʱ��1�����жϣ�������Ϊ1ms
    TIM1->ARR = (uint16_t)720; 
    TIM1->PSC = (uint16_t)10;
    TIM1->RCR = (uint16_t)10;

    //�ڼ���Ƶ��Ϊ72MHz������£�����ʹ�ܶ�ʱ��2�����жϣ�������Ϊ2.5ms
    TIM2->ARR = (uint16_t)720; 
    TIM2->PSC = (uint16_t)250;

    //�ڼ���Ƶ��Ϊ72MHz������£�����ʹ�ܶ�ʱ��3�����жϣ�������Ϊ500us
    TIM3->ARR = (uint16_t)360; 
    TIM3->PSC = (uint16_t)100;
    
    TIM1->DIER = (uint16_t)0x0001;//ʹ�ܸ����¼��ж�
    TIM1->CR1 = (uint16_t)0x0095;//���ü�����ģʽ����Ե����ģʽ��ʹ���Զ����ػ���ģʽʹ�ܸ��¹��ܡ���������Դ�����ж�ֻΪ������/�������ʹ�ܶ�ʱ��
    
    TIM2->DIER = (uint16_t)0x0001;//ʹ�ܸ����¼��ж�
    TIM2->CR1 = (uint16_t)0x0095;//���ü�����ģʽ����Ե����ģʽ��ʹ���Զ����ػ���ģʽʹ�ܸ��¹��ܡ���������Դ�����ж�ֻΪ������/�������ʹ�ܶ�ʱ��
    
    TIM3->DIER = (uint16_t)0x0001;//ʹ�ܸ����¼��ж�
    TIM3->CR1 = (uint16_t)0x0095;//���ü�����ģʽ����Ե����ģʽ��ʹ���Զ����ػ���ģʽʹ�ܸ��¹��ܡ���������Դ�����ж�ֻΪ������/�������ʹ�ܶ�ʱ��
}



