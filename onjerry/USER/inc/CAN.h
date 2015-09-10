#ifndef _CAN_H_
#define _CAN_H_

#include "stm32f10x.h"
#include "BMSCom.h"

typedef struct
{
    uint8_t* Txframestatus;
}Stu_SendBox;

/*CAN位时间NominalBitTime = 1 tq+tBS1+ tBS2,
tBS1 = tq x (TS1[3:0] + 1)
tBS2 = tq x (TS2[2:0] + 1)
tq = (BRP[9:0] + 1) x tPCLK
CAN外设用APB1,则这边tPCLK=1/32MHz, TS1[3:0]定为8, TS1[2:0]定为5, 
则位时间为16个tq, BRP[9:0]定义如下*/

#define CAN_BAUD_50K 44
#define CAN_BAUD_125K 17
#define CAN_BAUD_250K 8



extern Stu_CAN_FIFOMan CAN_RxFIFO_Mange[CAN_PERIPH_NUM];
extern Stu_CAN_FIFOMan CAN_TxFIFO_Mange[CAN_PERIPH_NUM];
extern Stu_CAN_RXFIFO CAN_ReceFIFO[CAN_PERIPH_NUM][CAN_RXFIFO_LENGTH];
extern Stu_CAN_TXFIFO CAN_SendFIFO[CAN_PERIPH_NUM][CAN_TXFIFO_LENGTH];

extern Stu_CAN_FIFOMan CAN_RxSAEFIFO_Mange[CAN_PERIPH_NUM];
extern Stu_CAN_FIFOMan CAN_TxSAEFIFO_Mange[CAN_PERIPH_NUM];
extern Stu_CAN_RXFIFO CAN_ReceSAEFIFO[CAN_PERIPH_NUM][CAN_RXSAEFIFO_LENGTH];
extern Stu_CAN_TXFIFO CAN_SendSAEFIFO[CAN_PERIPH_NUM][CAN_TXSAEFIFO_LENGTH];

extern void CAN_FIFO_Init(uint8_t cansel);
extern uint32_t CAN_FrameProc_RxFIFORece(uint8_t cansel, Stu_CAN_FIFOMan *fifomange, Stu_CAN_RXFIFO *fifo, uint32_t (**proc)(uint8_t cansel, Stu_CAN_RXFIFO *fifonode));
extern uint32_t CAN_FrameProc_TxFIFOSend(uint8_t cansel, Stu_CAN_FIFOMan *fifomange, Stu_CAN_TXFIFO *fifo);
extern uint32_t CAN_FrameProc_TxFIFOAdd(uint8_t cansel, Stu_CAN_FIFOMan *fifomange, Stu_CAN_TXFIFO *fifonode, Stu_CAN_TXFIFO *fifo);
extern uint32_t CAN_FrameProc_RxFIFOAdd(uint8_t cansel, Stu_CAN_FIFOMan *fifomange, Stu_CAN_RXFIFO *fifonode, Stu_CAN_RXFIFO *fifo);
extern void CAN_FilterInit(void);
extern void CAN_FuncInit(void);



#endif

