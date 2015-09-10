#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

#include "stm32f10x.h"

#define IWDOG_RELOAD (IWDG->KR = 0x0000AAAA)

void Watchdog_Init(void);


#endif
