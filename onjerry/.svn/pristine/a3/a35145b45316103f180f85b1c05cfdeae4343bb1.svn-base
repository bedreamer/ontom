#include "SYS_Init.h"

void System_Myinit(void)
{
    //系统复位后默认为HSI 8MHz 作为SYSCLK且AHB、APB1、APB2都为8MHz
    RCC->CR |= (uint32_t) 0x00000001;//重新选中HSI
    RCC->CR &= (uint32_t) 0xFEF6FFFF;//重新关闭CSS、PLL、HSE
    RCC->CR &= (uint32_t) 0xFFFBFFFF;//重新选择HSE not bypass
    RCC->CIR = (uint32_t) 0x009F0000;//清除所有中断标志并且关闭所有中断
    RCC->CFGR &= (uint32_t) 0xFF80C00C;
    /*重新选择USBPRE为PLL的1.5分频、PLLXTPRE选为HSE、
    PLLSCR选为HSI/2的时钟、PLLMUL为X2、SW选择HSI输出作为SYSCLK(最高72MHz)
    AHB、APB1、APB2不分频*/

    //开始配置用外部HSE 8MHz(not-bypass) 通过PLL生成72MHz SYSCLK ，PLLMUL配置为X9
    RCC->CFGR |= (uint32_t) 0x001d0000;//PLLXTPRE选为HSE、PLLSCR选为PLLXTPRE输出时钟、PLLMUL为X9、SW依然选择HSI输入(PLL选择使能后再选择PLLMUL为输入)
    RCC->CR |= (uint32_t) 0x00010000;//使能HSE
    while(!(RCC->CR & (uint32_t) 0x00020000));//等待HSE ready标志


    //开始配置AHB、APB1、APB2分频
    RCC->CFGR |= (uint32_t) 0x00000400;//APB1  /2,其他不分频
    
    FLASH->ACR  &= (uint32_t) 0xFFFFFFF0;//清FLITF外设的LATENCY[2:0]和HLFCYA位
    
    FLASH->ACR |= (uint32_t) 0x00000010;//使能Prefetch Buffer
    FLASH->ACR |= (uint32_t) 0x00000002;//设置LATENCY[2:0]为010，Two wait states满足if 48 MHz < SYSCLK ≤ 72 MHz

    
    RCC->CR |= (uint32_t) 0x01000000;//使能PLL
    while(!(RCC->CR & (uint32_t) 0x02000000));//等待PLL ready标志
    
    RCC->CFGR |= (uint32_t) 0x00000002;//SW选为PLL输入作为SYSCLK
    while(RCC->CFGR & (uint32_t) 0x0000000c != 0x00000008);//判断SW当前是否选中了PLL
    
    RCC->CR &= (uint32_t) 0xFFFFFFFE;//关闭HSI，注意:当程序向内部FLASH写或者擦除时要开启HSI

    /*外部中断优先级定义，启动文件中已经对组优先级
    位域长度进行了配置(3位组优先级、1位次优先级)*/

    //SCB->VTOR = FLASH_BASE | 0x00000000;// 指定中断向量表的起始位置在内部FLASH起始处 0x08000000，向量表起始的值必须是128words对齐处
}

void NVIC_Myinit(void)
{
    NVIC->IP[USART1_IRQn] = (uint8_t)0x70;//分配UART1中断组优先级为3，次优先级为1
    NVIC->IP[TIM3_IRQn] = (uint8_t)0x60;//分配Timer3中断组优先级为3，次优先级为0
    NVIC->IP[TIM2_IRQn] = (uint8_t)0x40;//分配Timer2中断组优先级为2，次优先级为0
    NVIC->IP[CAN1_RX1_IRQn] = (uint8_t)0x40;//分配CAN1 FIFO1接收中断组优先级为2，次优先级为0，可抢占UART1中断
    #if defined STM32F10X_MD
    NVIC->IP[USB_LP_CAN1_RX0_IRQn] = (uint8_t)0x40;//分配CAN1 FIFO0接收中断组优先级为2，次优先级为0，可抢占UART1中断
    NVIC->IP[USB_HP_CAN1_TX_IRQn] = (uint8_t)0x40;//分配CAN1发送中断组优先级为2，次优先级为0，可抢占UART1中断
    #elif defined STM32F10X_CL
    NVIC->IP[CAN1_RX0_IRQn] = (uint8_t)0x40;//分配CAN1 FIFO0接收中断组优先级为2，次优先级为0，可抢占UART1中断
    NVIC->IP[CAN1_TX_IRQn] = (uint8_t)0x50;//分配CAN1发送中断组优先级为2，次优先级为1，可抢占UART1中断
    NVIC->IP[CAN2_RX1_IRQn] = (uint8_t)0x40;//分配CAN2 FIFO1接收中断组优先级为2，次优先级为0，可抢占UART1中断
    NVIC->IP[CAN2_RX0_IRQn] = (uint8_t)0x40;//分配CAN2 FIFO0接收中断组优先级为2，次优先级为0，可抢占UART1中断
    NVIC->IP[CAN2_TX_IRQn] = (uint8_t)0x50;//分配CAN2发送中断组优先级为2，次优先级为1，可抢占UART1中断
    #endif
    NVIC->IP[SPI1_IRQn] = (uint8_t)0x20;//分配SPI1中断组优先级为1，次优先级为0，只能被TIM1抢占，可抢占UART1、CAN收发中断
    NVIC->IP[TIM1_UP_IRQn] = (uint8_t)0x00;//分配TIM1 update中断组优先级为0，次优先级为0，从而可以抢占UART1、SPI1、CAN收发中断


    NVIC->ISER[USART1_IRQn / 32] |= ((uint32_t)0x00000001 << (USART1_IRQn % 32));//NVIC设置UART1中断使能
    NVIC->ISER[TIM3_IRQn / 32] |= ((uint32_t)0x00000001 << (TIM3_IRQn % 32));//NVIC设置Timer3中断使能
    NVIC->ISER[SPI1_IRQn / 32] |= ((uint32_t)0x00000001 << (SPI1_IRQn % 32));//NVIC设置SPI1中断使能
    NVIC->ISER[TIM2_IRQn / 32] |= ((uint32_t)0x00000001 << (TIM2_IRQn % 32));//NVIC设置Timer2中断使能
    NVIC->ISER[CAN1_RX1_IRQn / 32] |= ((uint32_t)0x00000001 << (CAN1_RX1_IRQn % 32));//NVIC设置CAN1 FIFO1接收中断使能
    #if defined STM32F10X_MD
    NVIC->ISER[USB_LP_CAN1_RX0_IRQn / 32] |= ((uint32_t)0x00000001 << (USB_LP_CAN1_RX0_IRQn % 32));//NVIC设置CAN1 FIFO0接收中断使能
    NVIC->ISER[USB_HP_CAN1_TX_IRQn / 32] |= ((uint32_t)0x00000001 << (USB_HP_CAN1_TX_IRQn % 32));//NVIC设置CAN1发送中断使能
    #elif defined STM32F10X_CL
    NVIC->ISER[CAN1_RX0_IRQn / 32] |= ((uint32_t)0x00000001 << (CAN1_RX0_IRQn % 32));//NVIC设置CAN1 FIFO0接收中断使能
    NVIC->ISER[CAN1_TX_IRQn / 32] |= ((uint32_t)0x00000001 << (CAN1_TX_IRQn % 32));//NVIC设置CAN1发送中断使能
    NVIC->ISER[CAN2_RX1_IRQn / 32] |= ((uint32_t)0x00000001 << (CAN2_RX1_IRQn % 32));//NVIC设置CAN2 FIFO1接收中断使能
    NVIC->ISER[CAN2_RX0_IRQn / 32] |= ((uint32_t)0x00000001 << (CAN2_RX0_IRQn % 32));//NVIC设置CAN2 FIFO0接收中断使能
    NVIC->ISER[CAN2_TX_IRQn / 32] |= ((uint32_t)0x00000001 << (CAN2_TX_IRQn % 32));//NVIC设置CAN2发送中断使能
    #endif
    NVIC->ISER[TIM1_UP_IRQn / 32] |= ((uint32_t)0x00000001 << (TIM1_UP_IRQn % 32));//NVIC设置Timer1 的UEV更新事件中断使能
}

__ASM void System_Reset(void)
{
    CPSID f //执行优先级提高到-1，屏蔽所有中断(除Reset和NMI)
    LDR R0, =0x05FA0004
    LDR R1, =0xE000ED0C
    STR R0, [R1] //软件复位,向AIRCR.SYSRESETREQ置1
    NOP
    B        .         //等待复位
}



