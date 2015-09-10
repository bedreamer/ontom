#include "Watchdog.h"

void Watchdog_Init(void)
{   
    RCC->CSR |= RCC_CSR_LSION;//硬件看门狗启用的情况下的LSI会自动开启
    while(!(RCC->CSR & RCC_CSR_LSIRDY));
    DBGMCU->CR |= DBGMCU_CR_DBG_IWDG_STOP;//调试时中断看门狗计数
    IWDOG_RELOAD;
    IWDG->KR = 0x00005555;//配置看门狗复位时间为3276.8ms
    while(IWDG->SR & IWDG_SR_PVU);
    IWDG->PR = 0x00000003; 
    while(IWDG->SR & IWDG_SR_RVU);
    IWDG->RLR = 0x00000FFF; 

    if(FLASH->OBR & FLASH_OBR_WDG_SW)//判断是否为软件看门狗
        IWDG->KR = 0x0000CCCC;//启动独立看门狗,更新PR、RLR寄存器值
    else
        IWDOG_RELOAD;
}


