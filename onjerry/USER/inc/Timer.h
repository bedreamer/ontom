#ifndef _TIMER_H_
#define _TIMER_H_

#include "stm32f10x.h"

//单位TIMER3定时时间500us
#define U1_RXQUITCOUNT 0x000A
#define U1_RXTIMEBUSOFFCOUNT 0x0014
#define U1_RXBYTETIMEOUTCOUNT 4
#define U1_TXTIMEOUTCOUNT 110
#define U1_TXSTARTCOUNT 20

//单位TIMER1定时时间1ms
#define U1_COMMBREAKCOUNT 5000

#define TIMER1_UPDATEIEN (TIM1->DIER |= (uint16_t)0x0001)
#define TIMER1_UPDATEIDISEN (TIM1->DIER &= (uint16_t)0xFFFE)

#define TIMER_Interrupt_Enable(timer, interr) (timer->DIER |= (interr))
#define TIMER_Interrupt_Disable(timer, interr) (timer->DIER &= ~(interr))

extern uint8_t RXtimememb;
extern uint8_t TXtimememb;
extern uint8_t Systimememb;
//extern uint16_t U0_RXquittime;
extern uint16_t U1_RXresettime;
extern uint16_t U1_TXresettime;
extern uint16_t U1_TXstarttime;
extern uint16_t ADSMstartcont;
extern uint16_t COMMbreaktime;

void Timer_Init(void);
__irq void TIM1_UP_IRQHandler(void);






#endif

