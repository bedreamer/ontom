#ifndef _ADS795x_H_
#define _ADS795x_H_


#include "stm32f10x.h"

#define NSS_SET (GPIOA->BSRR = GPIO_BSRR_BS4)
#define NSS_RSET (GPIOA->BSRR = GPIO_BSRR_BR4)
#define SCK_SET (GPIOA->BSRR = GPIO_BSRR_BS5)
#define SCK_RSET (GPIOA->BSRR = GPIO_BSRR_BR5)
#define MOSI_SET (GPIOA->BSRR = GPIO_BSRR_BS7)
#define MOSI_RSET (GPIOA->BSRR = GPIO_BSRR_BR7)

#define SPI_ENRXIN (SPI1->CR2 |= (uint16_t)0x0040)
#define SPI_DISENRXIN (SPI1->CR2 &= (uint16_t)0xFFBF)

#define AD_CH0 0x0000
#define AD_CH1 0x0001
#define AD_CH2 0x0002
#define AD_CH3 0x0003
#define AD_CH4 0x0004
#define AD_CH5 0x0005
#define AD_CH6 0x0006
#define AD_CH7 0x0007

#define AD_DATA_EXPAND 10
#define AD_DATA_MAX (uint16_t)4095
#define AD_DATA_MIN (uint16_t)0

//模式配置时FRAM
//SPI帧bit15-bit12
#define AD_MODE_MANUAL ((uint16_t)0x1000)//通常模式
#define AD_MODE_AUTO1 ((uint16_t)0x2000)//自动1模式
#define AD_MODE_AUTO2 ((uint16_t)0x3000)//自动2模式
#define AD_MODE_CONTI ((uint16_t)0x0000)//继续上一次配置下的采样
/*继续上一次配置下的采样模式:在已经配置MANUAL模式
时继续上一次选中的通道采样；在已经配置 AUTO1 和AUTO2模
式时，自动增加通道号*/
//SPI帧bit11
#define AD_MODE_SETPR ((uint16_t)0x0800)
#define AD_MODE_RSTPR ((uint16_t)0x0000)
/*使能或者禁止编程range、channel、输出序列高四位的属性(当前
采样通道或者是输入GPIO的值)、下一帧需要输出的GPIO状态*/

//SPI帧bit10-bit7
#define AD_MODE_CHANSEL(CH) ((uint16_t)(CH)<<7)//通道选择

//SPI帧bit6
#define AD_MODE_RANGE1 ((uint16_t)0x0000)//RANGE 2.5V
#define AD_MODE_RANGE2 ((uint16_t)0x0040)//RANGE 5V

//SPI帧bit5
#define AD_MODE_SETPD ((uint16_t)0x0020)//POWER DOWN enable
#define AD_MODE_RSTPD ((uint16_t)0x0000)//POWER DOWN disable

//SPI帧bit4
#define AD_MODE_CURCHAN ((uint16_t)0x0000)//SDO输出 bit15-bit12为当前选择的通道
#define AD_MODE_INGPIO ((uint16_t)0x0010)//SDO输出bit15-bit12为输入GPIO的值

//SPI帧bit3-bit0
#define AD_MODE_OUTGPIO(Val) ((uint16_t) Val)//输出GPIO 的输出值设置

//通用配置方式0，带通道选择
#define AD_USER0(Val) (AD_MODE_MANUAL |AD_MODE_SETPR|AD_MODE_CHANSEL(Val)|AD_MODE_RANGE2 |AD_MODE_RSTPD|AD_MODE_CURCHAN|AD_MODE_OUTGPIO(0x0000))
#define AD_USER1(Val) (AD_MODE_MANUAL |AD_MODE_RSTPR|AD_MODE_CHANSEL(Val)|AD_MODE_RANGE2 |AD_MODE_RSTPD|AD_MODE_CURCHAN|AD_MODE_OUTGPIO(0x0000))



//AUTO1模式下自动通道切换序列配置
#define AD_AUTO1_SEQCMD ((uint16_t)0x8000)//FRAM1 :自动采样通道序列配置FRAM1 
#define AD_AUTO1_SEQSEL(Val) ((uint16_t)Val)//FRAM2 :每一位表示对应通道，1表示加入到采样序列，0表示不加入

//AUTO2模式下自动通道切换的通道界限FRAM
#define AD_AUTO2_OVERHEAD(Val) ((uint16_t)0x9000 | ((uint16_t)Val <<6))//采样从CH0到CH Val

//ALARM属性配置
#define AD_ALARM_GROUPSEL(Val) ((uint16_t)0xC000 | ((uint16_t)Val <<12))//FRAM1:对Val组进行ALARM上下限配置
//FRAM2:对FRAM1中选中组的通道进行具体配置
//bit15-bit14
#define AD_ALARM_CHANSEL(Val) ((uint16_t)Val << 14)//选中FRAM1中组的第Val通道
//bit13
#define AD_ALARM_THRE(Val) ((uint16_t)Val << 13)//选中配置HIGH/LOW ALARM HIGH:1 LOW:0
//bit12
#define AD_ALARM_EXIT(Val) ((uint16_t)Val << 12)//选择CONTINUE/EXIT ALARM配置CONTINUE:0 EXIT:1
//bit9-bit0
#define AD_ALARM_THREVAL(Val) ((uint16_t)Val)//对应通道上下限的值


extern uint32_t Samp_Databuff;

__irq void SPI1_IRQHandler(void);
uint16_t SPI_TRAN(uint16_t cmd);
void SPI_Init(void);


uint16_t ADS7953_CHsel(uint16_t chan);
uint16_t  ADS7953_CHflush(uint8_t num);
uint16_t  ADS7953_SMstart(void);
void ADS7953_Init(void);

#endif

