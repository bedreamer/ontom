#include "CAN.h"
#include "BMSCom_GB.h"
#include "BMSCom_SAE.h"
static Stu_SendBox SendMBS[CAN_PERIPH_NUM][3];
Stu_CAN_FIFOMan CAN_RxFIFO_Mange[CAN_PERIPH_NUM] = {{CAN_RXFIFO_LENGTH},{CAN_RXFIFO_LENGTH}};
Stu_CAN_FIFOMan CAN_TxFIFO_Mange[CAN_PERIPH_NUM] = {{CAN_TXFIFO_LENGTH},{CAN_TXFIFO_LENGTH}};
Stu_CAN_RXFIFO CAN_ReceFIFO[CAN_PERIPH_NUM][CAN_RXFIFO_LENGTH];
Stu_CAN_TXFIFO CAN_SendFIFO[CAN_PERIPH_NUM][CAN_TXFIFO_LENGTH];

Stu_CAN_FIFOMan CAN_RxSAEFIFO_Mange[CAN_PERIPH_NUM] = {{CAN_RXSAEFIFO_LENGTH},{CAN_RXSAEFIFO_LENGTH}};
Stu_CAN_FIFOMan CAN_TxSAEFIFO_Mange[CAN_PERIPH_NUM] = {{CAN_TXSAEFIFO_LENGTH},{CAN_TXSAEFIFO_LENGTH}};
Stu_CAN_RXFIFO CAN_ReceSAEFIFO[CAN_PERIPH_NUM][CAN_RXSAEFIFO_LENGTH];
Stu_CAN_TXFIFO CAN_SendSAEFIFO[CAN_PERIPH_NUM][CAN_TXSAEFIFO_LENGTH];

/*uint8_t CAN_FrameSend(uint8_t cansel, uint8_t frame)
{
    CAN_TypeDef *Txcan;
    uint32 smbnum;
    if(cansel ==  || cansel == )
    {
        Txcan = CAN1 + (CAN2 - CAN1) * cansel;
        if(Txcan->TSR & CAN_TSR_TME)
        {
            smbnum = (Txcan->TSR & CAN_TSR_CODE) >> 24;
            SendMBS[cansel][smbnum] = frame;//拷贝对应发送帧的帧号
            BMSCom_TxData[frame].TxFrame.status |= 0x01;//发送初始化完成
            Txcan->sTxMailBox[smbnum].TDTR = BMSCom_TxData[frame].TxFrame.Len;//装载DLC
            Txcan->sTxMailBox[smbnum].TDLR = BMSCom_TxData[frame].TxFrame.Pro_Data.data[0];//装载低4字节
            Txcan->sTxMailBox[smbnum].TDHR= BMSCom_TxData[frame].TxFrame.Pro_Data.data[1];//装载高4字节
            Txcan->sTxMailBox[smbnum].TIR = BMSCom_TxData[frame].TxFrame.Pro_ID.data | 0x00000001;//装载ID并触发发送
        }
        else
        {
            return 0x02;
        }
    }
    else
    {
        return 0x01;
    }
    return 0x00;
}*/

static void CAN_Interrupt_EnorDis(uint8_t cansel, uint32_t intersel, uint8_t enordis)
{
    CAN_TypeDef *Canch;
    Canch = (CAN_TypeDef *)(CAN1_BASE + (CAN2_BASE - CAN1_BASE)*cansel);
    Canch ->IER = (!enordis) ? Canch ->IER & ~intersel : Canch ->IER | intersel;
}


void CAN_FIFO_Init(uint8_t cansel)
{
    //关中断
    //接收队列的添加在中断中直接执行，所以需要关闭中断修改队列相关属性
    CAN_Interrupt_EnorDis(cansel, CAN_IER_FMPIE0 |CAN_IER_FMPIE1, 0);
    //CAN1->IER &= ~(CAN_IER_FMPIE0 |CAN_IER_FMPIE1);
    CAN_RxFIFO_Mange[cansel].len = 0;
    CAN_RxFIFO_Mange[cansel].fifohead = 0;
    CAN_RxFIFO_Mange[cansel].fifotail = 0;
    CAN_Interrupt_EnorDis(cansel, CAN_IER_FMPIE0 |CAN_IER_FMPIE1, 1);
    CAN_TxFIFO_Mange[cansel].len = 0;
    CAN_TxFIFO_Mange[cansel].fifohead = 0;
    CAN_TxFIFO_Mange[cansel].fifotail = 0;
}

uint32_t CAN_FrameProc_RxFIFORece(uint8_t cansel, Stu_CAN_FIFOMan *fifomange, Stu_CAN_RXFIFO *fifo, uint32_t (**proc)(uint8_t cansel, Stu_CAN_RXFIFO *fifonode))
{
    Stu_CAN_RXFIFO *frameindex;
    //uint8_t timerindex;
    if(fifomange[cansel].len != 0)
    {
        frameindex = &fifo[fifomange[cansel].fifohead];
        switch(proc[frameindex->frame](cansel, frameindex))
        {
            case 0://成功操作
            case 1://阶段错误
            case 2://数据异常
            goto Lable0_CAN_FrameProc_RxFIFORece;//去除FIFO的成员
            default:
            break;          
        }
    }
    return 1;
    Lable0_CAN_FrameProc_RxFIFORece: 
    //关中断修改
    //接收队列的添加在中断中直接执行，所以需要关闭中断修改队列相关属性
    CAN_Interrupt_EnorDis(cansel, CAN_IER_FMPIE0 |CAN_IER_FMPIE1, 0);
    if(++fifomange[cansel].fifohead == fifomange[cansel].maxlen)
    {
        fifomange[cansel].fifohead = 0;
    }   
    fifomange[cansel].len--;
    CAN_Interrupt_EnorDis(cansel, CAN_IER_FMPIE0 |CAN_IER_FMPIE1, 1);
    return 0;
}

/*uint32_t CAN_FrameProc_RxFIFORece(uint8_t cansel, Stu_CAN_FIFOMan *fifomange, Stu_CAN_RXFIFO *fifo)
{
    Stu_CAN_RXFIFO *frameindex;
    uint32_t retnum;
    uint8_t timerindex;
    if(fifomange[cansel].len != 0)
    {
        frameindex = &fifo[cansel][fifomange[cansel].fifohead];
        timerindex = BMSCom_RxData[cansel][frameindex->frame].RxFrame.Timerindex;
        if(BMSCom_NC[cansel].NowStage != BMSCom_RxData[cansel][frameindex->frame].RxFrame.stage)//判断对应定时器条件(定时帧不匹配或者已经超时)
        {
            goto: Lable0_CAN_FrameProc_RxFIFORece;//去除FIFO的成员
        }
        retnum = BMSCom_RxData[cansel][frameindex->frame].RxFIFODataProc();
        switch(retnum)
        {
            case 0://成功操作
            //BMSCom_RxData[cansel][frameindex->frame].RxFrame.status &= 0xFE;//关闭处理，需要等到下次发送之后开启
            case 1://数据异常
            goto: Lable0_CAN_FrameProc_RxFIFORece;//去除FIFO的成员
            default:
            break;          
        }
    }
    return 1;
    Lable0_CAN_FrameProc_RxFIFORece: 
    
    if(++CAN_RxFIFO_Mange[cansel].fifohead == CAN_RXFIFO_LENGTH)
    {
        CAN_RxFIFO_Mange[cansel].fifohead = 0;
    }
    //关中断修改
    CAN_RxFIFO_Mange[cansel].len--;
    return 0;
}*/

/*uint32_t CAN_FrameProc_RxSAEFIFORece(uint8_t cansel)
{
    Stu_CAN_RXFIFO *frameindex;
    uint32_t retnum;
    uint8_t timerindex;
    if(CAN_RxSAEFIFO_Mange[cansel].len != 0)
    {
        frameindex = &CAN_ReceSAEFIFO[cansel][CAN_RxSAEFIFO_Mange[cansel].fifohead];
   
        retnum = SAEReceFrame[cansel][frameindex->frame].RxFIFODataProc();
        switch(retnum)
        {
            case 0://成功操作
            //BMSCom_RxData[cansel][frameindex->frame].RxFrame.status &= 0xFE;//关闭处理，需要等到下次发送之后开启
            case 1://数据异常
            goto: Lable0_CAN_FrameProc_RxSAEFIFORece;//去除FIFO的成员
            default:
            break;          
        }
    }
    return 1;
    Lable0_CAN_FrameProc_RxSAEFIFORece: 
    
    if(++CAN_RxSAEFIFO_Mange[cansel].fifohead == CAN_RXSAEFIFO_LENGTH)
    {
        CAN_RxSAEFIFO_Mange[cansel].fifohead = 0;
    }
    //关中断修改
    CAN_RxSAEFIFO_Mange[cansel].len--;
    return 0;
}*/


static uint32_t CAN_FrameTestSend(uint8_t cansel, uint32_t *psmbnum)
{
    CAN_TypeDef *Txcan;
    if(cansel == 0 || cansel == 1)
    {
        Txcan = (CAN_TypeDef *)(CAN1_BASE + (CAN2_BASE - CAN1_BASE)*cansel);
        if((Txcan->TSR & CAN_TSR_TME) != 0)
        {
            *psmbnum = (Txcan->TSR & CAN_TSR_CODE) >> 24;
            return 0;
        }
        else
        {
            return 2;
        }
    }
    else
    {
        return 1;
    }
    //return 3;
}


/*uint32_t CAN_FrameProc_TxFIFOSend(uint8_t cansel)
{
    Stu_CAN_TXFIFO *frameindex;
    uint32_t retnum;
    if(CAN_TxFIFO_Mange[cansel].len != 0)
    {
        frameindex = &CAN_SendFIFO[cansel][CAN_TxFIFO_Mange[cansel].fifohead];
        if(!(BMSCom_TxData[cansel][frameindex->frame].TxFrame.status & 0x01) || BMSCom_NC[cansel].NowStage != BMSCom_TxData[cansel][frameindex->frame].TxFrame.stage)
        {
            goto: Lable0_CAN_FrameProc_TxFIFOSend;
        }

        if(CAN_FrameTestSend(cansel) == 0)//是否能发送
        {
            switch(BMSCom_TxData[cansel][CAN_TxFIFO_Mange[cansel].fifohead].TxFIFODataProc())
            {
                case 0://成功处理并发送
                goto: Lable0_CAN_FrameProc_TxFIFOSend;//去除FIFO的成员
                case 1:
                break;
            }
        }
    
    }
   
    return 1;
    Lable0_CAN_FrameProc_TxFIFOSend: 
    if(++CAN_TxFIFO_Mange[cansel].fifohead == CAN_TXFIFO_LENGTH)
    {
        CAN_TxFIFO_Mange[cansel].fifohead = 0;
    }
    //关中断修改
    CAN_TxFIFO_Mange[cansel].len--;
    return 0;
}*/
static void CAN_FrameSend(uint8_t cansel, uint32_t smbnum, Stu_CAN_TXFIFO *senddata)
{
    CAN_TypeDef *Txcan;
    Txcan = (CAN_TypeDef *)(CAN1_BASE + (CAN2_BASE - CAN1_BASE)*cansel);
    SendMBS[cansel][smbnum].Txframestatus = senddata->framestatus;//拷贝对应发送帧的状态地址
    //SendMBS[cansel][smbnum].Pframe = pframe;//拷贝对应发送后需要开启接收的帧号
    Txcan->sTxMailBox[smbnum].TDTR = 8;//装载DLC
    Txcan->sTxMailBox[smbnum].TDLR = senddata->Pro_Data.data[0];//装载低4字节
    Txcan->sTxMailBox[smbnum].TDHR= senddata->Pro_Data.data[1];//装载高4字节
    Txcan->sTxMailBox[smbnum].TIR = (senddata->ID << 3) | 0x00000005;//装载ID并触发发送
}

uint32_t CAN_FrameProc_TxFIFOSend(uint8_t cansel, Stu_CAN_FIFOMan *fifomange, Stu_CAN_TXFIFO *fifo)
{
    //Stu_CAN_TXFIFO *frameindex;
    //Stu_CAN_TXFIFO senddata = {0xFFFFFFFF, 0xFFFFFFFF};
    Stu_CAN_TXFIFO *senddata;
    uint32_t smbnum;
    if(fifomange[cansel].len != 0)
    {
        //frameindex = fifo[fifomange[cansel].fifohead].frame;
        if(CAN_FrameTestSend(cansel, &smbnum) == 0)//是否能发送
        {
            senddata = &fifo[fifomange[cansel].fifohead];
            CAN_FrameSend(cansel, smbnum, senddata);
            goto Lable0_CAN_FrameProc_TxFIFOSend;//去除FIFO的成员
            /*switch(proc[frameindex](cansel, &senddata))
            {
                case 0://成功处理
                CAN_FrameSend(cansel, smbnum, &senddata);
                case 1:goto: Lable0_CAN_FrameProc_TxFIFOSend;//去除FIFO的成员
                case 2://发生其它错误，维持该队列(如内部添加队列错误)
                break;
                default:
                break;
            }*/
        }
    
    }
   
    return 1;
    Lable0_CAN_FrameProc_TxFIFOSend: 
    //关中断修改
    //所有的发送队列添加都在主程序中进行，中断中只是传出一个标志
    //CAN_Interrupt_Disable(cansel, CAN_IER_TMEIE);
    if(++fifomange[cansel].fifohead == fifomange[cansel].maxlen)
    {
        fifomange[cansel].fifohead = 0;
    }
    
    fifomange[cansel].len--;
    //CAN_Interrupt_Enable(cansel, CAN_IER_TMEIE);
    return 0;
}

/*uint32_t CAN_FrameProc_TxSAEFIFOSend(uint8_t cansel)
{
    Stu_CAN_TXFIFO *frameindex;
    uint32_t retnum;
    if(CAN_TxSAEFIFO_Mange[cansel].len != 0)
    {
        frameindex = &CAN_SendSAEFIFO[cansel][CAN_TxSAEFIFO_Mange[cansel].fifohead];

        if(CAN_FrameTestSend(cansel) == 0)//是否能发送
        {
            switch(SAESendFrame[cansel][CAN_TxSAEFIFO_Mange[cansel].fifohead].TxFIFODataProc())
            {
                case 0://成功处理并发送
                goto: Lable0_CAN_FrameProc_TxSAEFIFOSend;//去除FIFO的成员
                case 1:
                break;
            }
        }
    
    }
   
    return 1;
    Lable0_CAN_FrameProc_TxSAEFIFOSend: 
    if(++CAN_TxSAEFIFO_Mange[cansel].fifohead == CAN_TXSAEFIFO_LENGTH)
    {
        CAN_TxSAEFIFO_Mange[cansel].fifohead = 0;
    }
    //关中断修改
    CAN_TxSAEFIFO_Mange[cansel].len--;
    return 0;
}






void CAN1_RX0_IRQHandler(void)//用于GB标准的单帧接收
{
    uint32_t receindex;
    if(!(CAN1->RF0R & CAN_RF0R_RFOM0) && (CAN1->RF0R & CAN_RF0R_FMP0))
    {
        receindex = (CAN1->sFIFOMailBox[0].RDTR & 0x0000FF00) >> 8;
        BMSCom_RxData[0][receindex].RxFrame.status &= 0xFC;//清接收超时标志以及接收超时定时启动标志
        BMSCom_RxData[0][receindex].RxFrame.status |= 0x04;//置位收到对应帧标志
        BMSCom_RxData[0][receindex].RxFrame.Len = CAN1->sFIFOMailBox[0].RDTR & 0x0000000F;
        BMSCom_RxData[0][receindex].RxFrame.Pro_Data.data[0] = CAN1->sFIFOMailBox[0].RDLR;
        BMSCom_RxData[0][receindex].RxFrame.Pro_Data.data[1] = CAN1->sFIFOMailBox[0].RDHR;
        CAN1->RF0R |= CAN_RF0R_RFOM0;//释放当前fifo0输出mailbox
        BMSCom_RxData[0][receindex].FrameDataProc();//函数处理
        
    }
 
}*/

uint32_t CAN_FrameProc_TxFIFOAdd(uint8_t cansel, Stu_CAN_FIFOMan *fifomange, Stu_CAN_TXFIFO *fifonode, Stu_CAN_TXFIFO *fifo)
{
    //关中断
    //CAN_Interrupt_Disable(cansel, CAN_IER_TMEIE);
    //所有的发送队列添加都在主程序中进行，中断中只是传出一个标志
    if(fifomange[cansel].len != fifomange[cansel].maxlen)
    {
        //节点拷贝
        fifo[fifomange[cansel].fifotail].Pro_Data.data[0] = fifonode->Pro_Data.data[0];
        fifo[fifomange[cansel].fifotail].Pro_Data.data[1] = fifonode->Pro_Data.data[1];
        fifo[fifomange[cansel].fifotail].ID = fifonode->ID;
        fifo[fifomange[cansel].fifotail].framestatus = fifonode->framestatus;
        if(++fifomange[cansel].fifotail == fifomange[cansel].maxlen)
        {
            fifomange[cansel].fifotail = 0;
        }
        fifomange[cansel].len++;
        //CAN_Interrupt_Enable(cansel, CAN_IER_TMEIE);
        return 0;
    }
    //CAN_Interrupt_Enable(cansel, CAN_IER_TMEIE);
    return 1;
}

uint32_t CAN_FrameProc_RxFIFOAdd(uint8_t cansel, Stu_CAN_FIFOMan *fifomange, Stu_CAN_RXFIFO *fifonode, Stu_CAN_RXFIFO *fifo)
{
    //关中断
    //接收队列的添加在中断中直接执行，所以需要关闭中断修改队列相关属性
    CAN_Interrupt_EnorDis(cansel, CAN_IER_FMPIE0 |CAN_IER_FMPIE1, 0);
    if(fifomange[cansel].len != fifomange[cansel].maxlen)
    {
        fifo[fifomange[cansel].fifotail].frame = fifonode->frame;
        fifo[fifomange[cansel].fifotail].Pro_Data.data[0] = fifonode->Pro_Data.data[0];
        fifo[fifomange[cansel].fifotail].Pro_Data.data[1] = fifonode->Pro_Data.data[1];
        fifo[fifomange[cansel].fifotail].len = fifonode->len;
        if(++fifomange[cansel].fifotail == fifomange[cansel].maxlen)
        {
            fifomange[cansel].fifotail = 0;
        }
        fifomange[cansel].len++;
        CAN_Interrupt_EnorDis(cansel, CAN_IER_FMPIE0 |CAN_IER_FMPIE1, 1);
        return 0;
    }
    CAN_Interrupt_EnorDis(cansel, CAN_IER_FMPIE0 |CAN_IER_FMPIE1, 1);
    return 1;
}

static void CAN_Rx_HandlerProc(Stu_CAN_FIFOMan *fifoman, Stu_CAN_RXFIFO *fifo, __IO uint32_t *canrxflag, CAN_FIFOMailBox_TypeDef *canrxmailbox)
{
    uint32_t receindex;
    if((fifoman->len != CAN_RXFIFO_LENGTH) && (*canrxflag & 0x03))
    {
        receindex = (canrxmailbox->RDTR & 0x0000FF00) >> 8;
        //if(CAN_RXTimer[0][BMSCom_ReceFrameGBMan[0][receindex].RxFrame.Timerindex].frame == (uint8_t)receindex && (CAN_RXTimer[0][BMSCom_ReceFrameGBMan[0][receindex].RxFrame.Timerindex].status & 0x01))
        //{
            
            fifo[fifoman->fifotail].frame = (uint8_t)receindex;
            fifo[fifoman->fifotail].len = (canrxmailbox->RDTR & 0x0000000F);
            fifo[fifoman->fifotail].Pro_Data.data[0] = canrxmailbox->RDLR;
            fifo[fifoman->fifotail].Pro_Data.data[1] = canrxmailbox->RDHR;
            
            if(++fifoman->fifotail == fifoman->maxlen)
            {
                fifoman->fifotail = 0;
            }
            fifoman->len++;
        //}      
    }
    *canrxflag |= 0x20;//释放当前fifo0输出mailbox
}

__irq void CAN1_RX0_IRQHandler(void)//用于GB标准的单帧接收CAN_ReceFIFO
{
    CAN_Rx_HandlerProc(&CAN_RxFIFO_Mange[0], CAN_ReceFIFO[0], &CAN1->RF0R, &CAN1->sFIFOMailBox[0]);
 /*   uint32_t receindex;
    if((CAN_RxFIFO_Mange[0].len != CAN_RXFIFO_LENGTH) && !(CAN1->RF0R & CAN_RF0R_RFOM0) && (CAN1->RF0R & CAN_RF0R_FMP0))
    {
        receindex = (CAN1->sFIFOMailBox[0].RDTR & 0x0000FF00) >> 8;
        //if(CAN_RXTimer[0][BMSCom_ReceFrameGBMan[0][receindex].RxFrame.Timerindex].frame == (uint8_t)receindex && (CAN_RXTimer[0][BMSCom_ReceFrameGBMan[0][receindex].RxFrame.Timerindex].status & 0x01))
        //{
            
            CAN_ReceFIFO[0][CAN_RxFIFO_Mange[0].fifotail].frame = (uint8_t)receindex;
            CAN_ReceFIFO[0][CAN_RxFIFO_Mange[0].fifotail].len = (CAN1->sFIFOMailBox[0].RDTR & 0x0000000F);
            CAN_ReceFIFO[0][CAN_RxFIFO_Mange[0].fifotail].data[0] = CAN1->sFIFOMailBox[0].RDLR;
            CAN_ReceFIFO[0][CAN_RxFIFO_Mange[0].fifotail].data[1] = CAN1->sFIFOMailBox[0].RDHR;
            
            if(++CAN_RxFIFO_Mange[0].fifotail == CAN_RXFIFO_LENGTH)
            {
                CAN_RxFIFO_Mange[0].fifotail = 0;
            }
            CAN_RxFIFO_Mange[0].len++;
        //}      
    }
    CAN1->RF0R |= CAN_RF0R_RFOM0;//释放当前fifo0输出mailbox*/
}



__irq void CAN1_RX1_IRQHandler(void)//用于接收SAE标准的单帧接收
{
    CAN_Rx_HandlerProc(&CAN_RxSAEFIFO_Mange[0], CAN_ReceSAEFIFO[0], &CAN1->RF1R, &CAN1->sFIFOMailBox[1]);
    /*uint32_t receindex;
    if((CAN_RxSAEFIFO_Mange[0].len != CAN_RXFIFO_LENGTH) && !(CAN1->RF1R & CAN_RF1R_RFOM1) && (CAN1->RF1R & CAN_RF1R_FMP1))
    {
        receindex = (CAN1->sFIFOMailBox[1].RDTR & 0x0000FF00) >> 8;
        CAN_ReceSAEFIFO[0][CAN_RxSAEFIFO_Mange[0].fifotail].frame = (uint8_t)receindex;
        CAN_ReceSAEFIFO[0][CAN_RxSAEFIFO_Mange[0].fifotail].len = (CAN1->sFIFOMailBox[1].RDTR & 0x0000000F);
        CAN_ReceSAEFIFO[0][CAN_RxSAEFIFO_Mange[0].fifotail].data[0] = CAN1->sFIFOMailBox[1].RDLR;
        CAN_ReceSAEFIFO[0][CAN_RxSAEFIFO_Mange[0].fifotail].data[1] = CAN1->sFIFOMailBox[1].RDHR;
        if(++ CAN_RxSAEFIFO_Mange[0].fifotail== CAN_RXSAEFIFO_LENGTH)
        {
            CAN_RxSAEFIFO_Mange[0].fifotail = 0;
        }
        CAN_RxSAEFIFO_Mange[0].len++;
    }
    CAN1->RF1R |= CAN_RF1R_RFOM1;//释放当前fifo1输出mailbox*/
}


static void CAN_Tx_HandlerProc(__IO uint32_t *cantxflag, Stu_SendBox *sendbox)
{
    uint8_t boxindex;
    uint32_t tsr_txokmask = CAN_TSR_TXOK0;
    uint32_t tsr_rqcpmask = CAN_TSR_RQCP0;
    for(boxindex = 0; boxindex < 3; boxindex++)
    {
        if(*cantxflag & tsr_txokmask) //是否发送成功
        {
            if(sendbox[boxindex].Txframestatus != (uint8_t *)0)//发送的为GB帧
            {
               *(sendbox[boxindex].Txframestatus) |= 0x01;//置相应发送标志
            }
        }
        else//Abort 触发
        {
            ;
        }
        *cantxflag |= tsr_rqcpmask;//清发送请求
        tsr_txokmask <<= 8;
        tsr_rqcpmask <<= 8;
    }

}

__irq void CAN1_TX_IRQHandler(void)
{
    CAN_Tx_HandlerProc(&CAN1->TSR, SendMBS[0]);
/*    uint8_t timerindex;
    if(CAN1->TSR & CAN_TSR_TXOK0) //是否发送成功
    {
        CAN1->TSR |= CAN_TSR_RQCP0;
        if(SendMBS[0][0].Txframestatus != (uint8_t *)0)//发送的为GB帧
        {
            *(SendMBS[0][0].Txframestatus) |= 0x01;//置相应发送标志
        }
    }
    else//Abort 触发
    {
        CAN1->TSR |= CAN_TSR_RQCP0;
    }

    if(CAN1->TSR & CAN_TSR_TXOK1) //是否发送成功
    {
        CAN1->TSR |= CAN_TSR_RQCP1;
        if(SendMBS[0][1].Txframestatus != (uint8_t *)0)//发送的为GB帧
        {
            *(SendMBS[0][1].Txframestatus) |= 0x01;//置相应发送标志
        }
    }
    else//Abort 触发
    {
        ;
    }

    if(CAN1->TSR & CAN_TSR_TXOK2) //是否发送成功
    {
        CAN1->TSR |= CAN_TSR_RQCP2;
        if(SendMBS[0][2].Txframestatus != (uint8_t *)0)//发送的为GB帧
        {
            *(SendMBS[0][2].Txframestatus) |= 0x01;//置相应发送标志
        }
    }
    else//Abort 触发
    {
        CAN1->TSR |= CAN_TSR_RQCP2;
    }*/
}

__irq void CAN2_RX0_IRQHandler(void)//用于GB标准的单帧接收CAN_ReceFIFO
{
    CAN_Rx_HandlerProc(&CAN_RxFIFO_Mange[1], CAN_ReceFIFO[1], &CAN2->RF0R, &CAN2->sFIFOMailBox[0]);
}

__irq void CAN2_RX1_IRQHandler(void)//用于接收SAE标准的单帧接收
{
    CAN_Rx_HandlerProc(&CAN_RxSAEFIFO_Mange[1], CAN_ReceSAEFIFO[1], &CAN2->RF1R, &CAN2->sFIFOMailBox[1]);
}

__irq void CAN2_TX_IRQHandler(void)
{
    CAN_Tx_HandlerProc(&CAN2->TSR, SendMBS[1]);
}

void CAN_FilterInit(void)
{
    uint8_t len;
    __IO uint32_t *filterreg = (uint32_t *)(CAN1->sFilterRegister);
    CAN1->FA1R = 0x00000000;//禁用所有CAN滤波块
    //分配GB的滤波块
    for(len = 0; len < GB_RXFRAME_FilterNum; len++)//GB单帧滤波块配置
    {
        *filterreg = (BMSCom_FrameGB_ReceDataAtrb[len].Pro_ID.data << 3) | (0x00000004);//去除RTR加IDE ，can1部分
        *(filterreg+28) = *filterreg;//去除RTR加IDE，can2部分
        filterreg++;
    }
    if(GB_RXFRAME_FilterNum % 2)//如果参与滤波块的帧数为奇数，最后一个滤波块为MASK类型
    {
        *filterreg = 0xFFFFFFFF;
        *(filterreg+28) = 0xFFFFFFFF;
        filterreg++;
    }

    //分配SAE的滤波块
    //MP部分
    *filterreg = (BMSCom_MPAtrb.Pro_RXMPID.data << 3) | (0x00000004);//去除RTR加IDE ，can1部分
    *(filterreg+28) = *filterreg;//去除RTR加IDE，can2部分
    filterreg++;
    *filterreg = (BMSCom_MPAtrb.Pro_RXDTID.data << 3) | (0x00000004);//去除RTR加IDE ，can1部分
    *(filterreg+28) = *filterreg;//去除RTR加IDE，can2部分
    filterreg++;
    //Req和Ack部分
    *filterreg = (BMSCom_RAAtrb.Pro_RXReqID.data << 3) | (0x00000004);//去除RTR加IDE ，can1部分
    *(filterreg+28) = *filterreg;//去除RTR加IDE，can2部分
    filterreg++;
    *filterreg = (BMSCom_RAAtrb.Pro_RXAckID.data << 3) | (0x00000004);//去除RTR加IDE ，can1部分
    *(filterreg+28) = *filterreg;//去除RTR加IDE，can2部分
    CAN1->FA1R |= ~(0xFFFFFFFF << (((GB_RXFRAME_FilterNum / 2) + (GB_RXFRAME_FilterNum % 2)) + ((SAE_RXFRAME_FilterNum / 2) + (SAE_RXFRAME_FilterNum % 2))));
    CAN1->FA1R |= CAN1->FA1R << 14;
}

void CAN_FuncInit(void)
{
    //CAN1相关外设时钟使能
    RCC->APB1ENR |= (RCC_APB1ENR_CAN1EN | RCC_APB1ENR_CAN2EN);//使能CAN1、CAN2
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN |RCC_APB2ENR_IOPBEN |RCC_APB2ENR_AFIOEN);//使能GPIOA、AFIO时钟
    //配置CAN1对应的GPIO
    GPIOA->CRH &= ~(GPIO_CRH_MODE & 0x000FF000);
    GPIOA->CRH |= GPIO_CRH_MODE12;//设置CAN1的TX引脚为50MHz输出模式，RX为输入模式
    GPIOA->BSRR = GPIO_BSRR_BS11;
    GPIOA->CRH &=~(GPIO_CRL_CNF & 0x000FF000);
    GPIOA->CRH |= (GPIO_CRH_CNF12_1|GPIO_CRH_CNF11_1);//设置CAN1的TX引脚为复用推挽输出模式,RX引脚为上拉输入
    
    
    CAN1->MCR ^= (CAN_MCR_INRQ | CAN_MCR_SLEEP);//退出sleep mode, 进入initialization mode请求
    while((CAN1->MSR & (CAN_MSR_SLAK | CAN_MSR_INAK)) != 0x0000001);//等待退出sleep mode, 进入initialization mode请求响应
    CAN1->MCR |=  (CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP);//配置自动恢复bus-off状态、自动唤醒、自动重发、按发送请求的顺序发送数据帧
    CAN1->BTR = (((uint32_t)8<<16) | ((uint32_t)5<<20) | ((uint32_t)1<<24) | (uint32_t)CAN_BAUD_125K);//配置波特率
    //CAN1->BTR = (CAN_BTR_LBKM | ((uint32_t)8<<16) | ((uint32_t)5<<20) | ((uint32_t)1<<24) | (uint32_t)CAN_BAUD_125K);//配置波特率、回环测试模式

    CAN1->IER |= CAN_IER_TMEIE | CAN_IER_FMPIE0 |CAN_IER_FMPIE1;//使能发送请求响应且TXMailBox Empty中断(由TXOK标志区别Abort或者成功发送)、FIFO0 Pending中断(FMP0不为00b)、FIFO1 Pending中断(FMP1不为00b)
    
    CAN1->FMR |= CAN_FMR_FINIT;//初始化滤波块,CAN2滤波块从14开始
    CAN1->FS1R = 0x0FFFFFFF;//将filter bank0-27配置为 Single 32-bit scale 
    CAN1->FM1R = 0x00000000;
    CAN1->FM1R |= (~(0xFFFFFFFF << (SAE_RXFRAME_FilterNum / 2))) << ((GB_RXFRAME_FilterNum / 2) + (GB_RXFRAME_FilterNum % 2));//SAE滤波块模式选择
    CAN1->FM1R |= (~(0xFFFFFFFF << (GB_RXFRAME_FilterNum / 2)));//GB滤波块模式选择
    CAN1->FM1R |= (CAN1->FM1R << 14);//CAN2设置
    CAN1->FFA1R = (~(0xFFFFFFFF << ((SAE_RXFRAME_FilterNum / 2)+(SAE_RXFRAME_FilterNum % 2)))) << ((GB_RXFRAME_FilterNum / 2) + (GB_RXFRAME_FilterNum % 2));
    CAN1->FFA1R |= (CAN1->FFA1R << 14);//CAN2设置
    CAN1->FMR &= ~CAN_FMR_FINIT;//退出初始化滤波块
    //CAN2->FMR &= ~CAN_FMR_FINIT;//退出初始化滤波块
    CAN_FilterInit();
    CAN1->MCR &= ~CAN_MCR_INRQ;//退出initialization mode
    while((CAN1->MSR & CAN_MSR_INAK) == 0x0000001);

    //配置CAN2对应的GPIO
    AFIO->MAPR |= AFIO_MAPR_CAN2_REMAP;//Remap
    GPIOB->CRL &= ~(GPIO_CRL_MODE & 0x0FF00000);
    GPIOB->CRL |= GPIO_CRL_MODE6;//设置CAN2的TX引脚为50MHz输出模式，RX为输入模式
    GPIOB->BSRR = GPIO_BSRR_BS5;
    GPIOB->CRL &=~(GPIO_CRL_CNF & 0x0FF00000);
    GPIOB->CRL |= (GPIO_CRL_CNF6_1|GPIO_CRL_CNF5_1);//设置CAN1的TX引脚为复用推挽输出模式,RX引脚为上拉输入
    CAN2->MCR ^= (CAN_MCR_INRQ | CAN_MCR_SLEEP);//退出sleep mode, 进入initialization mode请求
    while((CAN2->MSR & (CAN_MSR_SLAK | CAN_MSR_INAK)) != 0x0000001);//等待退出sleep mode, 进入initialization mode请求响应
    CAN2->MCR |=  (CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP);//配置自动恢复bus-off状态、自动唤醒、自动重发、按发送请求的顺序发送数据帧
    CAN2->BTR = (((uint32_t)8<<16) | ((uint32_t)5<<20) | ((uint32_t)1<<24) | (uint32_t)CAN_BAUD_125K);//配置波特率
    //CAN1->BTR = (CAN_BTR_LBKM | ((uint32_t)8<<16) | ((uint32_t)5<<20) | ((uint32_t)1<<24) | (uint32_t)CAN_BAUD_125K);//配置波特率、回环测试模式
    CAN2->IER |= CAN_IER_TMEIE | CAN_IER_FMPIE0 |CAN_IER_FMPIE1;//使能发送请求响应且TXMailBox Empty中断(由TXOK标志区别Abort或者成功发送)、FIFO0 Pending中断(FMP0不为00b)、FIFO1 Pending中断(FMP1不为00b)
    CAN2->MCR &= ~CAN_MCR_INRQ;//退出initialization mode
    while((CAN2->MSR & CAN_MSR_INAK) == 0x0000001);
    
}






