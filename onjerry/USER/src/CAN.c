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
            SendMBS[cansel][smbnum] = frame;//������Ӧ����֡��֡��
            BMSCom_TxData[frame].TxFrame.status |= 0x01;//���ͳ�ʼ�����
            Txcan->sTxMailBox[smbnum].TDTR = BMSCom_TxData[frame].TxFrame.Len;//װ��DLC
            Txcan->sTxMailBox[smbnum].TDLR = BMSCom_TxData[frame].TxFrame.Pro_Data.data[0];//װ�ص�4�ֽ�
            Txcan->sTxMailBox[smbnum].TDHR= BMSCom_TxData[frame].TxFrame.Pro_Data.data[1];//װ�ظ�4�ֽ�
            Txcan->sTxMailBox[smbnum].TIR = BMSCom_TxData[frame].TxFrame.Pro_ID.data | 0x00000001;//װ��ID����������
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
    //���ж�
    //���ն��е�������ж���ֱ��ִ�У�������Ҫ�ر��ж��޸Ķ����������
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
            case 0://�ɹ�����
            case 1://�׶δ���
            case 2://�����쳣
            goto Lable0_CAN_FrameProc_RxFIFORece;//ȥ��FIFO�ĳ�Ա
            default:
            break;          
        }
    }
    return 1;
    Lable0_CAN_FrameProc_RxFIFORece: 
    //���ж��޸�
    //���ն��е�������ж���ֱ��ִ�У�������Ҫ�ر��ж��޸Ķ����������
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
        if(BMSCom_NC[cansel].NowStage != BMSCom_RxData[cansel][frameindex->frame].RxFrame.stage)//�ж϶�Ӧ��ʱ������(��ʱ֡��ƥ������Ѿ���ʱ)
        {
            goto: Lable0_CAN_FrameProc_RxFIFORece;//ȥ��FIFO�ĳ�Ա
        }
        retnum = BMSCom_RxData[cansel][frameindex->frame].RxFIFODataProc();
        switch(retnum)
        {
            case 0://�ɹ�����
            //BMSCom_RxData[cansel][frameindex->frame].RxFrame.status &= 0xFE;//�رմ�����Ҫ�ȵ��´η���֮����
            case 1://�����쳣
            goto: Lable0_CAN_FrameProc_RxFIFORece;//ȥ��FIFO�ĳ�Ա
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
    //���ж��޸�
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
            case 0://�ɹ�����
            //BMSCom_RxData[cansel][frameindex->frame].RxFrame.status &= 0xFE;//�رմ�����Ҫ�ȵ��´η���֮����
            case 1://�����쳣
            goto: Lable0_CAN_FrameProc_RxSAEFIFORece;//ȥ��FIFO�ĳ�Ա
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
    //���ж��޸�
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

        if(CAN_FrameTestSend(cansel) == 0)//�Ƿ��ܷ���
        {
            switch(BMSCom_TxData[cansel][CAN_TxFIFO_Mange[cansel].fifohead].TxFIFODataProc())
            {
                case 0://�ɹ���������
                goto: Lable0_CAN_FrameProc_TxFIFOSend;//ȥ��FIFO�ĳ�Ա
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
    //���ж��޸�
    CAN_TxFIFO_Mange[cansel].len--;
    return 0;
}*/
static void CAN_FrameSend(uint8_t cansel, uint32_t smbnum, Stu_CAN_TXFIFO *senddata)
{
    CAN_TypeDef *Txcan;
    Txcan = (CAN_TypeDef *)(CAN1_BASE + (CAN2_BASE - CAN1_BASE)*cansel);
    SendMBS[cansel][smbnum].Txframestatus = senddata->framestatus;//������Ӧ����֡��״̬��ַ
    //SendMBS[cansel][smbnum].Pframe = pframe;//������Ӧ���ͺ���Ҫ�������յ�֡��
    Txcan->sTxMailBox[smbnum].TDTR = 8;//װ��DLC
    Txcan->sTxMailBox[smbnum].TDLR = senddata->Pro_Data.data[0];//װ�ص�4�ֽ�
    Txcan->sTxMailBox[smbnum].TDHR= senddata->Pro_Data.data[1];//װ�ظ�4�ֽ�
    Txcan->sTxMailBox[smbnum].TIR = (senddata->ID << 3) | 0x00000005;//װ��ID����������
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
        if(CAN_FrameTestSend(cansel, &smbnum) == 0)//�Ƿ��ܷ���
        {
            senddata = &fifo[fifomange[cansel].fifohead];
            CAN_FrameSend(cansel, smbnum, senddata);
            goto Lable0_CAN_FrameProc_TxFIFOSend;//ȥ��FIFO�ĳ�Ա
            /*switch(proc[frameindex](cansel, &senddata))
            {
                case 0://�ɹ�����
                CAN_FrameSend(cansel, smbnum, &senddata);
                case 1:goto: Lable0_CAN_FrameProc_TxFIFOSend;//ȥ��FIFO�ĳ�Ա
                case 2://������������ά�ָö���(���ڲ���Ӷ��д���)
                break;
                default:
                break;
            }*/
        }
    
    }
   
    return 1;
    Lable0_CAN_FrameProc_TxFIFOSend: 
    //���ж��޸�
    //���еķ��Ͷ�����Ӷ����������н��У��ж���ֻ�Ǵ���һ����־
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

        if(CAN_FrameTestSend(cansel) == 0)//�Ƿ��ܷ���
        {
            switch(SAESendFrame[cansel][CAN_TxSAEFIFO_Mange[cansel].fifohead].TxFIFODataProc())
            {
                case 0://�ɹ���������
                goto: Lable0_CAN_FrameProc_TxSAEFIFOSend;//ȥ��FIFO�ĳ�Ա
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
    //���ж��޸�
    CAN_TxSAEFIFO_Mange[cansel].len--;
    return 0;
}






void CAN1_RX0_IRQHandler(void)//����GB��׼�ĵ�֡����
{
    uint32_t receindex;
    if(!(CAN1->RF0R & CAN_RF0R_RFOM0) && (CAN1->RF0R & CAN_RF0R_FMP0))
    {
        receindex = (CAN1->sFIFOMailBox[0].RDTR & 0x0000FF00) >> 8;
        BMSCom_RxData[0][receindex].RxFrame.status &= 0xFC;//����ճ�ʱ��־�Լ����ճ�ʱ��ʱ������־
        BMSCom_RxData[0][receindex].RxFrame.status |= 0x04;//��λ�յ���Ӧ֡��־
        BMSCom_RxData[0][receindex].RxFrame.Len = CAN1->sFIFOMailBox[0].RDTR & 0x0000000F;
        BMSCom_RxData[0][receindex].RxFrame.Pro_Data.data[0] = CAN1->sFIFOMailBox[0].RDLR;
        BMSCom_RxData[0][receindex].RxFrame.Pro_Data.data[1] = CAN1->sFIFOMailBox[0].RDHR;
        CAN1->RF0R |= CAN_RF0R_RFOM0;//�ͷŵ�ǰfifo0���mailbox
        BMSCom_RxData[0][receindex].FrameDataProc();//��������
        
    }
 
}*/

uint32_t CAN_FrameProc_TxFIFOAdd(uint8_t cansel, Stu_CAN_FIFOMan *fifomange, Stu_CAN_TXFIFO *fifonode, Stu_CAN_TXFIFO *fifo)
{
    //���ж�
    //CAN_Interrupt_Disable(cansel, CAN_IER_TMEIE);
    //���еķ��Ͷ�����Ӷ����������н��У��ж���ֻ�Ǵ���һ����־
    if(fifomange[cansel].len != fifomange[cansel].maxlen)
    {
        //�ڵ㿽��
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
    //���ж�
    //���ն��е�������ж���ֱ��ִ�У�������Ҫ�ر��ж��޸Ķ����������
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
    *canrxflag |= 0x20;//�ͷŵ�ǰfifo0���mailbox
}

__irq void CAN1_RX0_IRQHandler(void)//����GB��׼�ĵ�֡����CAN_ReceFIFO
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
    CAN1->RF0R |= CAN_RF0R_RFOM0;//�ͷŵ�ǰfifo0���mailbox*/
}



__irq void CAN1_RX1_IRQHandler(void)//���ڽ���SAE��׼�ĵ�֡����
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
    CAN1->RF1R |= CAN_RF1R_RFOM1;//�ͷŵ�ǰfifo1���mailbox*/
}


static void CAN_Tx_HandlerProc(__IO uint32_t *cantxflag, Stu_SendBox *sendbox)
{
    uint8_t boxindex;
    uint32_t tsr_txokmask = CAN_TSR_TXOK0;
    uint32_t tsr_rqcpmask = CAN_TSR_RQCP0;
    for(boxindex = 0; boxindex < 3; boxindex++)
    {
        if(*cantxflag & tsr_txokmask) //�Ƿ��ͳɹ�
        {
            if(sendbox[boxindex].Txframestatus != (uint8_t *)0)//���͵�ΪGB֡
            {
               *(sendbox[boxindex].Txframestatus) |= 0x01;//����Ӧ���ͱ�־
            }
        }
        else//Abort ����
        {
            ;
        }
        *cantxflag |= tsr_rqcpmask;//�巢������
        tsr_txokmask <<= 8;
        tsr_rqcpmask <<= 8;
    }

}

__irq void CAN1_TX_IRQHandler(void)
{
    CAN_Tx_HandlerProc(&CAN1->TSR, SendMBS[0]);
/*    uint8_t timerindex;
    if(CAN1->TSR & CAN_TSR_TXOK0) //�Ƿ��ͳɹ�
    {
        CAN1->TSR |= CAN_TSR_RQCP0;
        if(SendMBS[0][0].Txframestatus != (uint8_t *)0)//���͵�ΪGB֡
        {
            *(SendMBS[0][0].Txframestatus) |= 0x01;//����Ӧ���ͱ�־
        }
    }
    else//Abort ����
    {
        CAN1->TSR |= CAN_TSR_RQCP0;
    }

    if(CAN1->TSR & CAN_TSR_TXOK1) //�Ƿ��ͳɹ�
    {
        CAN1->TSR |= CAN_TSR_RQCP1;
        if(SendMBS[0][1].Txframestatus != (uint8_t *)0)//���͵�ΪGB֡
        {
            *(SendMBS[0][1].Txframestatus) |= 0x01;//����Ӧ���ͱ�־
        }
    }
    else//Abort ����
    {
        ;
    }

    if(CAN1->TSR & CAN_TSR_TXOK2) //�Ƿ��ͳɹ�
    {
        CAN1->TSR |= CAN_TSR_RQCP2;
        if(SendMBS[0][2].Txframestatus != (uint8_t *)0)//���͵�ΪGB֡
        {
            *(SendMBS[0][2].Txframestatus) |= 0x01;//����Ӧ���ͱ�־
        }
    }
    else//Abort ����
    {
        CAN1->TSR |= CAN_TSR_RQCP2;
    }*/
}

__irq void CAN2_RX0_IRQHandler(void)//����GB��׼�ĵ�֡����CAN_ReceFIFO
{
    CAN_Rx_HandlerProc(&CAN_RxFIFO_Mange[1], CAN_ReceFIFO[1], &CAN2->RF0R, &CAN2->sFIFOMailBox[0]);
}

__irq void CAN2_RX1_IRQHandler(void)//���ڽ���SAE��׼�ĵ�֡����
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
    CAN1->FA1R = 0x00000000;//��������CAN�˲���
    //����GB���˲���
    for(len = 0; len < GB_RXFRAME_FilterNum; len++)//GB��֡�˲�������
    {
        *filterreg = (BMSCom_FrameGB_ReceDataAtrb[len].Pro_ID.data << 3) | (0x00000004);//ȥ��RTR��IDE ��can1����
        *(filterreg+28) = *filterreg;//ȥ��RTR��IDE��can2����
        filterreg++;
    }
    if(GB_RXFRAME_FilterNum % 2)//��������˲����֡��Ϊ���������һ���˲���ΪMASK����
    {
        *filterreg = 0xFFFFFFFF;
        *(filterreg+28) = 0xFFFFFFFF;
        filterreg++;
    }

    //����SAE���˲���
    //MP����
    *filterreg = (BMSCom_MPAtrb.Pro_RXMPID.data << 3) | (0x00000004);//ȥ��RTR��IDE ��can1����
    *(filterreg+28) = *filterreg;//ȥ��RTR��IDE��can2����
    filterreg++;
    *filterreg = (BMSCom_MPAtrb.Pro_RXDTID.data << 3) | (0x00000004);//ȥ��RTR��IDE ��can1����
    *(filterreg+28) = *filterreg;//ȥ��RTR��IDE��can2����
    filterreg++;
    //Req��Ack����
    *filterreg = (BMSCom_RAAtrb.Pro_RXReqID.data << 3) | (0x00000004);//ȥ��RTR��IDE ��can1����
    *(filterreg+28) = *filterreg;//ȥ��RTR��IDE��can2����
    filterreg++;
    *filterreg = (BMSCom_RAAtrb.Pro_RXAckID.data << 3) | (0x00000004);//ȥ��RTR��IDE ��can1����
    *(filterreg+28) = *filterreg;//ȥ��RTR��IDE��can2����
    CAN1->FA1R |= ~(0xFFFFFFFF << (((GB_RXFRAME_FilterNum / 2) + (GB_RXFRAME_FilterNum % 2)) + ((SAE_RXFRAME_FilterNum / 2) + (SAE_RXFRAME_FilterNum % 2))));
    CAN1->FA1R |= CAN1->FA1R << 14;
}

void CAN_FuncInit(void)
{
    //CAN1�������ʱ��ʹ��
    RCC->APB1ENR |= (RCC_APB1ENR_CAN1EN | RCC_APB1ENR_CAN2EN);//ʹ��CAN1��CAN2
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN |RCC_APB2ENR_IOPBEN |RCC_APB2ENR_AFIOEN);//ʹ��GPIOA��AFIOʱ��
    //����CAN1��Ӧ��GPIO
    GPIOA->CRH &= ~(GPIO_CRH_MODE & 0x000FF000);
    GPIOA->CRH |= GPIO_CRH_MODE12;//����CAN1��TX����Ϊ50MHz���ģʽ��RXΪ����ģʽ
    GPIOA->BSRR = GPIO_BSRR_BS11;
    GPIOA->CRH &=~(GPIO_CRL_CNF & 0x000FF000);
    GPIOA->CRH |= (GPIO_CRH_CNF12_1|GPIO_CRH_CNF11_1);//����CAN1��TX����Ϊ�����������ģʽ,RX����Ϊ��������
    
    
    CAN1->MCR ^= (CAN_MCR_INRQ | CAN_MCR_SLEEP);//�˳�sleep mode, ����initialization mode����
    while((CAN1->MSR & (CAN_MSR_SLAK | CAN_MSR_INAK)) != 0x0000001);//�ȴ��˳�sleep mode, ����initialization mode������Ӧ
    CAN1->MCR |=  (CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP);//�����Զ��ָ�bus-off״̬���Զ����ѡ��Զ��ط��������������˳��������֡
    CAN1->BTR = (((uint32_t)8<<16) | ((uint32_t)5<<20) | ((uint32_t)1<<24) | (uint32_t)CAN_BAUD_125K);//���ò�����
    //CAN1->BTR = (CAN_BTR_LBKM | ((uint32_t)8<<16) | ((uint32_t)5<<20) | ((uint32_t)1<<24) | (uint32_t)CAN_BAUD_125K);//���ò����ʡ��ػ�����ģʽ

    CAN1->IER |= CAN_IER_TMEIE | CAN_IER_FMPIE0 |CAN_IER_FMPIE1;//ʹ�ܷ���������Ӧ��TXMailBox Empty�ж�(��TXOK��־����Abort���߳ɹ�����)��FIFO0 Pending�ж�(FMP0��Ϊ00b)��FIFO1 Pending�ж�(FMP1��Ϊ00b)
    
    CAN1->FMR |= CAN_FMR_FINIT;//��ʼ���˲���,CAN2�˲����14��ʼ
    CAN1->FS1R = 0x0FFFFFFF;//��filter bank0-27����Ϊ Single 32-bit scale 
    CAN1->FM1R = 0x00000000;
    CAN1->FM1R |= (~(0xFFFFFFFF << (SAE_RXFRAME_FilterNum / 2))) << ((GB_RXFRAME_FilterNum / 2) + (GB_RXFRAME_FilterNum % 2));//SAE�˲���ģʽѡ��
    CAN1->FM1R |= (~(0xFFFFFFFF << (GB_RXFRAME_FilterNum / 2)));//GB�˲���ģʽѡ��
    CAN1->FM1R |= (CAN1->FM1R << 14);//CAN2����
    CAN1->FFA1R = (~(0xFFFFFFFF << ((SAE_RXFRAME_FilterNum / 2)+(SAE_RXFRAME_FilterNum % 2)))) << ((GB_RXFRAME_FilterNum / 2) + (GB_RXFRAME_FilterNum % 2));
    CAN1->FFA1R |= (CAN1->FFA1R << 14);//CAN2����
    CAN1->FMR &= ~CAN_FMR_FINIT;//�˳���ʼ���˲���
    //CAN2->FMR &= ~CAN_FMR_FINIT;//�˳���ʼ���˲���
    CAN_FilterInit();
    CAN1->MCR &= ~CAN_MCR_INRQ;//�˳�initialization mode
    while((CAN1->MSR & CAN_MSR_INAK) == 0x0000001);

    //����CAN2��Ӧ��GPIO
    AFIO->MAPR |= AFIO_MAPR_CAN2_REMAP;//Remap
    GPIOB->CRL &= ~(GPIO_CRL_MODE & 0x0FF00000);
    GPIOB->CRL |= GPIO_CRL_MODE6;//����CAN2��TX����Ϊ50MHz���ģʽ��RXΪ����ģʽ
    GPIOB->BSRR = GPIO_BSRR_BS5;
    GPIOB->CRL &=~(GPIO_CRL_CNF & 0x0FF00000);
    GPIOB->CRL |= (GPIO_CRL_CNF6_1|GPIO_CRL_CNF5_1);//����CAN1��TX����Ϊ�����������ģʽ,RX����Ϊ��������
    CAN2->MCR ^= (CAN_MCR_INRQ | CAN_MCR_SLEEP);//�˳�sleep mode, ����initialization mode����
    while((CAN2->MSR & (CAN_MSR_SLAK | CAN_MSR_INAK)) != 0x0000001);//�ȴ��˳�sleep mode, ����initialization mode������Ӧ
    CAN2->MCR |=  (CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP);//�����Զ��ָ�bus-off״̬���Զ����ѡ��Զ��ط��������������˳��������֡
    CAN2->BTR = (((uint32_t)8<<16) | ((uint32_t)5<<20) | ((uint32_t)1<<24) | (uint32_t)CAN_BAUD_125K);//���ò�����
    //CAN1->BTR = (CAN_BTR_LBKM | ((uint32_t)8<<16) | ((uint32_t)5<<20) | ((uint32_t)1<<24) | (uint32_t)CAN_BAUD_125K);//���ò����ʡ��ػ�����ģʽ
    CAN2->IER |= CAN_IER_TMEIE | CAN_IER_FMPIE0 |CAN_IER_FMPIE1;//ʹ�ܷ���������Ӧ��TXMailBox Empty�ж�(��TXOK��־����Abort���߳ɹ�����)��FIFO0 Pending�ж�(FMP0��Ϊ00b)��FIFO1 Pending�ж�(FMP1��Ϊ00b)
    CAN2->MCR &= ~CAN_MCR_INRQ;//�˳�initialization mode
    while((CAN2->MSR & CAN_MSR_INAK) == 0x0000001);
    
}






