#include "UART.h"
#include "AD_Sample.h"
#include "Stat_control.h"
#include "Timer.h"
#include "DATA_pro.h"
#include "ADS795x.h"
#include "Flash.h"
#include "Cal_Sampdata.h"
#include "CAN.h"
#include "BMSCom_GB.h"
#include "BMSCom_SAE.h"
#include "Watchdog.h"

int main(void)
{
    //uint8_t cannum = 0;
    //uint32_t can = CAN2-CAN1;
    ADSample_Init();
    UART_Init();
    LED_IOinit();
    Relay_IOinit();
    Coupler_IOinit();
    Timer_Init();
    Watchdog_Init();
    Flash_Init();    
    //CAN_FuncInit();
    Cal_Init(Samp_ADdata_Stream.Cal_UIchmin, (uint16_t* )FLASHADDR_CALSTART, CALIBRATEDATA_COLNUM*CALIBRATEDATA_SEGNUM, 2);
    __enable_irq();
   /*if(Flash_FPEC_Unlock()==0x00)
    {
        if(Flash_Halfword_Program((uint16_t*)0x0801FFFE,0x0876)==0x00)
        {
            Flash_Page_Erase((uint32_t*)0x0801FC00);
        }        
    }*/
    //CAN_FrameProc_TxFIFOSend(cannum, CAN_TxFIFO_Mange, CAN_SendFIFO[cannum]);
    while(1)
    {
        IWDOG_RELOAD;
        if(!(Systimememb & 0x01))//上一次数据采样完成
        {
            Samp_DataArr[Samp_ADdata_Stream.now].Opratefun(&(Samp_DataStat.DataStat), &Samp_ADdata_Stream);  
            if((Samp_ADdata_Stream.cal_status) && (!Cal_ADscream_Change(&Samp_ADdata_Stream)))//校准非未启动的情况下
            {
                Cal_ADdata_Copy(Samp_DataArr[Samp_ADdata_Stream.now].ADcalarrcol, &Samp_ADdata_Stream);            
            }
            else
            {
                ADSample_Scream_Col(&(Samp_DataStat.DataStat), &Samp_ADdata_Stream);               
            }
            
            ADSample_Change(&Samp_ADdata_Stream);           
        }

       /* for(cannum = 0; cannum < CAN_PERIPH_NUM; cannum++)
        {
            //BMSCom_SAEFrame_TXFifoCtrlProc(cannum);//SAE帧发送队列处理
            BMSCom_GBFrame_TXFifoCtrlProc(cannum);//GB帧发送队列处理
           // CAN_FrameProc_TxFIFOSend(cannum, CAN_TxSAEFIFO_Mange, CAN_SendSAEFIFO[cannum]);//发送SAE帧队列
           CAN_FrameProc_TxFIFOSend(cannum, CAN_TxFIFO_Mange, CAN_SendFIFO[cannum]);//发送GB帧队列

            CAN_FrameProc_RxFIFORece(cannum, CAN_RxSAEFIFO_Mange,  CAN_ReceSAEFIFO[cannum], BMSCom_SAEFrameRXProc);//SAE接收队列处理
            //CAN_FrameProc_RxFIFORece(cannum, CAN_RxFIFO_Mange,  CAN_ReceFIFO[cannum], BMSCom_GBFrameRXProc);//GB接收队列处理
            BMSCom_GBStageCtrlProc[BMSCom_NC[cannum].NowStage](cannum);
        }*/
        
        Coupler_Updata(&(Samp_DataStat.DataStat));
        Relay_Control(&(Samp_DataStat.DataStat), &Samp_ADdata_Stream);

        if(((U1_RXfin & 0x03) == 0x03)&&((U1_TXfin & 0x01) == 0x00))//接收到数据
        {
            DATA_RecProc(&U1_FIFOBuff, &U1_RxTxLen, &Samp_ADdata_Stream);
        }
        
        if((U1_TXfin & 0x0D) == 0x09)//发送使能，开始发送延时到但数据和CRC未准备好
        {
            DATA_TraProc(&U1_FIFOBuff, &U1_RxTxLen, &Samp_DataStat);
        }
        
    }
}

