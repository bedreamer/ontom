#include "UART.h"
#include "CRC.h"
#include "Timer.h"
#include "DATA_pro.h"
#include "AD_Sample.h"
#include "Cal_Sampdata.h"
#include "Stat_control.h"

static Data_RtoT Head_Data;

static uint8_t* bigenddata_copy(uint8_t *buff,uint8_t *dirbuff,uint8_t length)
{
    for(;length != 0;length -= 2)
    {
        *(dirbuff+1) = *buff++;
        *dirbuff = *buff++;
        dirbuff += 2;
    }
    return dirbuff;
}


static uint16_t TXdata_proc(Un_U1FIFOBuff *FIFOBuff, uint8_t *ADSampStat)
{
    uint8_t *crcaddr;
    switch(FIFOBuff->TxBuff.Txfifodata[U1_TXFIFOMDINDEX_FUNCODE] = Head_Data.funcode)//发送fifohead 拷贝
    {
        case SELFMB_FUNCODE_READHOLDREG:
        FIFOBuff->TxBuff.Txfifodata[U1_TXFIFOMDINDEX_BYTECOUNT] = Head_Data.addr_qoreg * 2;
        crcaddr = bigenddata_copy(ADSampStat,FIFOBuff->TxBuff.Txfifodata + SELFMB_PDULEN_FUNCODE + SELFMB_PDULEN_BYTECOUNT, Head_Data.addr_qoreg * 2);
        *(uint16_t *)crcaddr = CRC16_CAL(FIFOBuff->FIFO, Head_Data.addr_qoreg * 2 + SELFMB_READHOLDREG_PDURESLEN + U1_FRAME_SENDHEADLEN);      
        break;
        case SELFMB_FUNCODE_WRITMULREG:
        FIFOBuff->TxBuff.Txfifodata[U1_TXFIFOMDINDEX_STARTADDR] = BIGTOLIT_U16(Head_Data.addr_regstart);
        FIFOBuff->TxBuff.Txfifodata[U1_TXFIFOMDINDEX_QUANTITYOFREG] = BIGTOLIT_U16(Head_Data.addr_qoreg);
        crcaddr = FIFOBuff->FIFO + SELFMB_WRITMULREG_PDURESLEN + U1_FRAME_SENDHEADLEN;
        *(uint16_t*)crcaddr = CRC16_CAL(FIFOBuff->FIFO, SELFMB_WRITMULREG_PDURESLEN + U1_FRAME_SENDHEADLEN);
        break;
        default://异常错误的发送fifohead 拷贝
        FIFOBuff->TxBuff.Txfifodata[U1_TXFIFOMDINDEX_EXCEPFUNCODE] = Head_Data.excepcode;
        crcaddr = FIFOBuff->FIFO + SELFMB_PDULEN_EXCEPFUNCODE + SELFMB_PDULEN_EXCEPCODE + U1_FRAME_SENDHEADLEN;
        *(uint16_t*)crcaddr = CRC16_CAL(FIFOBuff->FIFO, SELFMB_PDULEN_EXCEPFUNCODE + SELFMB_PDULEN_EXCEPCODE + U1_FRAME_SENDHEADLEN);
        break;
    }
    return (uint16_t)((crcaddr - FIFOBuff->FIFO) + U1_FRAME_CRCDATA);
}



static uint8_t RXdata_proc(uint8_t *RXfifodata, SAMP_Stream *streamdata)
{
    uint8_t retval = 0x00;
    if(Head_Data.funcode == SELFMB_FUNCODE_WRITMULREG)
    {
        RXfifodata += SELFMB_WRITMULREG_PDUREQLEN;
        switch(Head_Data.addr_regstart)
        {
            case U1_RXFIFOINDEX_CMD:
            if(Head_Data.addr_qoreg == U1_RXFIFOINDEX_CMDLEN)
            retval = OutputCmd_DataPro((uint16_t*)(RXfifodata), streamdata);
            //retval = cmddatapro((uint16_t*)(RXfifodata), streamdata);
            else
            retval = 0x01;
            break;
            case U1_RXFIFOINDEX_LIMIT:
            if(Head_Data.addr_qoreg == U1_RXFIFOINDEX_LIMITLEN)
            retval = ADSampe_LimitDataPro((uint16_t*)(RXfifodata), streamdata);
            //retval = limitdatapro((uint16_t*)(RXfifodata), ADsamparr, streamdata);
            else
            retval = 0x01;
            break;
            case U1_RXFIFOINDEX_CAL:
            if(Head_Data.addr_qoreg == U1_RXFIFOINDEX_CALLEN)
            retval = Cal_DataPro((uint16_t*)(RXfifodata), streamdata);
            //retval = caldatapro((uint16_t*)(RXfifodata), caldataarr, streamdata);
            else
            retval = 0x01;
            break;
            case U1_RXFIFOINDEX_ADDR:
            break;
            case U1_RXFIFOINDEX_IOMEM:
            if(Head_Data.addr_qoreg == U1_RXFIFOINDEX_IOMEMLEN)
            retval = IOMemb_DataPro(RXfifodata);
            else
            retval = 0x01;
            break;
            default:
            break;
        }
        if(retval)
        {
            Head_Data.funcode |= 0x80;
            Head_Data.excepcode = SELFMB_EXCEPCODE_WRITDATAFAULT;
        }
    }
    return retval;
}

static uint8_t RXdata_confirm(Un_U1FIFOBuff *FIFOBuff, uint16_t RXlength)
{
    Head_Data.funcode = FIFOBuff->RxBuff.Rxfifodata[U1_RXFIFOMDINDEX_FUNCODE];
    if(U1_RXfin & 0x80)//数据接收发生过溢出
    {
        Head_Data.funcode |= 0x80;
        Head_Data.excepcode = SELFMB_EXCEPCODE_OVERFLOWFAULT;
        return 0x05;//数据发生溢出，接收完成，但收到长度为0
    }
    if(RXlength < U1_FRAME_RECEHEADLEN)//收到的帧数据不可靠
    {
        return 0x06;
    }
    if(*(uint16_t*)(FIFOBuff->FIFO+RXlength-U1_FRAME_CRCDATA) != CRC16_CAL(FIFOBuff->FIFO, RXlength - U1_FRAME_CRCDATA))
    {
        Head_Data.funcode |= 0x80;
        Head_Data.excepcode = SELFMB_EXCEPCODE_CRCFAULT;
        return 0x01;//CRC错误
    }
    
    switch(Head_Data.funcode)//功能码判断
    {
        case SELFMB_FUNCODE_READHOLDREG:
        Head_Data.addr_regstart = *(uint16_t *)(FIFOBuff->RxBuff.Rxfifodata+U1_RXFIFOMDINDEX_STARTADDR);
        Head_Data.addr_qoreg = *(uint16_t *)(FIFOBuff->RxBuff.Rxfifodata+U1_RXFIFOMDINDEX_QUANTITYOFREG);
        Head_Data.addr_regstart = BIGTOLIT_U16(Head_Data.addr_regstart);
        Head_Data.addr_qoreg = BIGTOLIT_U16(Head_Data.addr_qoreg);
        if(Head_Data.addr_qoreg < SELFMB_READHOLDREG_EXCEPQORMIN || Head_Data.addr_qoreg > SELFMB_READHOLDREG_EXCEPQORMAX)
        {
            Head_Data.funcode |= 0x80;
            Head_Data.excepcode = SELFMB_EXCEPCODE_QORFAULT;
            return 0x03;
        }
        if(Head_Data.addr_regstart < SELFMB_READHOLDREG_REGADDRSTART || (Head_Data.addr_regstart +Head_Data.addr_qoreg) > SELFMB_READHOLDREG_REGADDREND)
        {
            Head_Data.funcode |= 0x80;
            Head_Data.excepcode = SELFMB_EXCEPCODE_ADDRFAULT;
            return 0x04;
        }
        break;
        
        case SELFMB_FUNCODE_WRITMULREG:
        Head_Data.addr_regstart = *(uint16_t *)(FIFOBuff->RxBuff.Rxfifodata+U1_RXFIFOMDINDEX_STARTADDR);
        Head_Data.addr_qoreg = *(uint16_t *)(FIFOBuff->RxBuff.Rxfifodata+U1_RXFIFOMDINDEX_QUANTITYOFREG);
        Head_Data.reg_bytelen= *(uint8_t *)(FIFOBuff->RxBuff.Rxfifodata+U1_RXFIFOMDINDEX_BYTECOUNT);
        Head_Data.addr_regstart = BIGTOLIT_U16(Head_Data.addr_regstart);
        Head_Data.addr_qoreg = BIGTOLIT_U16(Head_Data.addr_qoreg);
        if(Head_Data.addr_qoreg < SELFMB_WRITMULREG_EXCEPQORMIN || Head_Data.addr_qoreg > SELFMB_WRITMULREG_EXCEPQORMAX || Head_Data.addr_qoreg*2 !=(uint16_t)(Head_Data.reg_bytelen))
        {
            Head_Data.funcode |= 0x80;
            Head_Data.excepcode = SELFMB_EXCEPCODE_QORFAULT;
            return 0x03;
        }
        if(Head_Data.addr_regstart < SELFMB_WRITMULREG_REGADDRSTART || (Head_Data.addr_regstart + Head_Data.addr_qoreg) > SELFMB_WRITMULREG_REGADDREND)
        {
            Head_Data.funcode |= 0x80;
            Head_Data.excepcode = SELFMB_EXCEPCODE_ADDRFAULT;
            return 0x04;
        }
        break;
        
        default://接收异常错误处理
        Head_Data.funcode |= 0x80;
        Head_Data.excepcode = SELFMB_EXCEPCODE_FUNFAULT;
        return 0x02;
    }
    return 0x00;
}

uint8_t DATA_RecProc(Un_U1FIFOBuff *FIFOBuff, Un_U1TRANSFLEN *FIFOLen, SAMP_Stream* sam_status)
{
    /* if((RXlength == U1_FRAME_RECEDATALEN)&&((U1_RXfin & 0x03) == 0x03)&&((U1_TXfin & 0x01) == 0x00))
    {
        if(!RXdata_proc(UART1_RXfifohead, UART1_RXfifo, &Head_Data))//正确收到帧
        {
            RXdata_copy(UART1_RXfifo, Samp_DataArr, &Head_Data);//数据拷贝和非法操作地址处理
            U1_RXfin = 0x02;//CRC通过，数据拷贝完成
            U1_TXfin  |= 0x01;//发送使能,接收帧CRC正确
            TXtimememb |= 0x02;//开启开始发送计时
            UART1_TXEN;//开启UART发送,如果之前UART发送关闭的话硬件上会使串口先发送一个Idle帧
        }
        else//收到的帧错误
        {
            U1_RXfin = 0x02;//重新开始接收
            UART1_RXEN;//开启UART接收
            return 0x01;
        }
    }*/
    if(RXdata_confirm(FIFOBuff, FIFOLen->Rec_len) != 0x06)//收到帧处理,且CRC通过
    {
        COMMbreaktime = U1_COMMBREAKCOUNT;
        
        TIMER1_UPDATEIDISEN;//加锁修改
        Systimememb &= 0xFD;//通讯中断计数关闭
        TIMER1_UPDATEIEN;
        
        sam_status->SysFault &= 0xFFFE;//清通讯故障标志
        RXdata_proc(FIFOBuff->RxBuff.Rxfifodata, sam_status);//数据拷贝和非法操作地址处理
        U1_RXfin = 0x02;//CRC判断结束，数据拷贝工作完成，同时清可能的溢出标志
        U1_TXfin  |= 0x01;//发送使能,准备发送
        U1_TXresettime = U1_TXSTARTCOUNT;
        TXtimememb = 0x02;//只开启开始发送计时
        UART1_TXEN;//开启UART发送,如果之前UART发送关闭的话硬件上会使串口先发送一个Idle帧   
    }
    else//重新开始接收
    {
        U1_RXfin = 0x02;
        UP485_RXEN;//开启485接收
        UART1_RXEN;//开启UART接收
    }

    return 0x00;
}

uint8_t DATA_TraProc(Un_U1FIFOBuff *FIFOBuff, Un_U1TRANSFLEN *FIFOLen, Un_ADSamp_Stat *sampdataarr)
{
    FIFOLen->Tran_len = TXdata_proc(FIFOBuff, sampdataarr->data);//装载发送TXfifo
    U1_TXfin |= 0x04;//置位CRC校验完成
    
    TIMER1_UPDATEIDISEN;//加锁修改
    Systimememb |= 0x02;//通讯中断计数使能
    TIMER1_UPDATEIEN;
    
    USART1_Send();
    return 0x00;
}


