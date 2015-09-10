#ifndef _STAT_CONTROL_H_
#define _STAT_CONTROL_H_

#include "stm32f10x.h"
#include "MyTypedef.h"
#include "AD_Sample.h"
#define IONUM_FZDY_BRANCH1 0
#define IONUM_KM_BRANCH1 1
#define IONUM_ELELOCK1 2
#define IONUM_FZDY_BRANCH2 3
#define IONUM_KM_BRANCH2 4
#define IONUM_ELELOCK2 5
#define IONUM_LED_RUN 6
#define IONUM_LED_FAULT 7
#define IONUM_LED_ACIN 8

/*#define LED_RUN_ON (GPIOA->BSRR = GPIO_BSRR_BS1)
#define LED_RUN_OFF (GPIOA->BSRR = GPIO_BSRR_BR1)
#define LED_FAULT_ON (GPIOA->BSRR = GPIO_BSRR_BS2)
#define LED_FAULT_OFF (GPIOA->BSRR = GPIO_BSRR_BR2)
#define ELELOCK2_ON (GPIOC->BSRR = GPIO_BSRR_BS2)
#define ELELOCK2_OFF (GPIOC->BSRR = GPIO_BSRR_BR2)
#define ELELOCK1_ON (GPIOC->BSRR = GPIO_BSRR_BS3)
#define ELELOCK1_OFF (GPIOC->BSRR = GPIO_BSRR_BR3)
#define FZDY_BRANCH2_ON (GPIOC->BSRR = GPIO_BSRR_BS5)
#define FZDY_BRANCH2_OFF (GPIOC->BSRR = GPIO_BSRR_BR5)
#define FZDY_BRANCH1_ON (GPIOA->BSRR = GPIO_BSRR_BS0)
#define FZDY_BRANCH1_OFF (GPIOA->BSRR = GPIO_BSRR_BR0)
#define KM_BRANCH2_ON (GPIOA->BSRR = GPIO_BSRR_BS3)
#define KM_BRANCH2_OFF (GPIOA->BSRR = GPIO_BSRR_BR3)
#define KM_BRANCH1_ON (GPIOC->BSRR = GPIO_BSRR_BS4)
#define KM_BRANCH1_OFF (GPIOC->BSRR = GPIO_BSRR_BR4)


#define KM_BRANCH1_STAT (GPIOC->ODR & GPIO_ODR_ODR4)
#define KM_BRANCH2_STAT (GPIOA->ODR & GPIO_ODR_ODR3)
#define FZDY_BRANCH1_STAT (GPIOA->ODR & GPIO_ODR_ODR0)
#define FZDY_BRANCH2_STAT (GPIOC->ODR & GPIO_ODR_ODR5)*/

//#define RESERV1_ON (GPIOA->BSRR = GPIO_BSRR_BS3)
//#define RESERV1_OFF (GPIOA->BSRR = GPIO_BSRR_BR3)
//#define RESERV2_ON (GPIOA->BSRR = GPIO_BSRR_BS2)
//#define RESERV2_OFF (GPIOA->BSRR = GPIO_BSRR_BR2)



//³äµçÖÕÖ¹Àà¹ÊÕÏ×´Ì¬
/*#define BIT_RDQ_KMMAIN GPIO_IDR_IDR15
#define BIT_TRIP_KMMAIN GPIO_IDR_IDR14
#define BIT_TRIP_KMBRANCH1 GPIO_IDR_IDR13
#define BIT_TRIP_KMBRANCH2 GPIO_IDR_IDR11
#define BIT_FLQ GPIO_IDR_IDR1*/

#define BIT_STOP GPIO_IDR_IDR15
#define BIT_TRIP_ACIN GPIO_IDR_IDR13
#define BIT_FLQ GPIO_IDR_IDR11
#define BIT_RDQ_KMMAIN GPIO_IDR_IDR1
//³äµçÔËÐÐ×´Ì¬
/*#define BIT_SW_KMMAIN GPIO_IDR_IDR7
#define BIT_SW_KMBRANCH1 GPIO_IDR_IDR8
#define BIT_SW_KMBRANCH2 GPIO_IDR_IDR9*/

#define BIT_SW_ACIN GPIO_IDR_IDR14

#define GET_STATUS(stat) ((uint16_t)(GPIOB->IDR) & (stat))

#define LED1_ON (GPIOC->BSRR = GPIO_BSRR_BR13)
#define LED1_OFF (GPIOC->BSRR = GPIO_BSRR_BS13)
#define LED2_ON (GPIOC->BSRR = GPIO_BSRR_BR14)
#define LED2_OFF (GPIOC->BSRR = GPIO_BSRR_BS14)

#define IOMemb_OutNum 16
#define IOMemb_InNum 8
typedef struct
{
    uint8_t gpioindex;
    uint8_t bitnum;
}Stu_IOmap;

typedef struct
{
    uint8_t IOnum;
}Stu_OutNumAss;

void LED_IOinit(void);
void Relay_IOinit(void);
void Coupler_IOinit(void);

uint8_t OutputCmd_DataPro(uint16_t *pdata, SAMP_Stream *dirdata);
uint8_t IOMemb_DataPro(uint8_t *pdata);
void Coupler_Updata(Stu_ADSample_DATAStat *sampdata);
uint16_t Relay_Control(Stu_ADSample_DATAStat *sampdata, SAMP_Stream* Controlbuff);

#endif
