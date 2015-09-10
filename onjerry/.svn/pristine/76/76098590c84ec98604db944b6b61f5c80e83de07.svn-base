#ifndef _DATA_PRO_H_
#define _DATA_PRO_H_

#include "stm32f10x.h"
#include "UART.h"
#include "AD_Sample.h"


typedef struct DATA_RXtoTXstatus
{
    uint8_t funcode;
    uint8_t excepcode;
    uint8_t reg_bytelen;
    uint16_t addr_regstart;
    uint16_t addr_qoreg;
} Data_RtoT;

uint8_t DATA_RecProc(Un_U1FIFOBuff *FIFOBuff, Un_U1TRANSFLEN *FIFOLen, SAMP_Stream* sam_status);

uint8_t DATA_TraProc(Un_U1FIFOBuff *FIFOBuff, Un_U1TRANSFLEN *FIFOLen, Un_ADSamp_Stat *sampdataarr);


#endif

