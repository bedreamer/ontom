#include "test.h"
#include "stm32f10x.h"


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

    SCB->VTOR = FLASH_BASE | 0x00000000;// 指定中断向量表的起始位置在内部FLASH起始处 0x08000000，向量表起始的值必须是128words对齐处
}

void WWDG_IRQHandler(void)
{
	unsigned char k,d;
	k++;
	d++;
}
struct mystruct0
{
	unsigned short i;
	unsigned int j;
}ms0;

struct mystruct1
{
	unsigned short i;
	__packed unsigned int j;
}ms1;

unsigned int fun(void)
{
	unsigned int i;
	unsigned int nu[5]={1,2,3,4,5};
	nu[3]=nu[2];
	nu[2]=nu[1];
	nu[1]++;
	nu[0]++;
	nu[4]++;
	return ++i;
}

int main(void)
{
	const unsigned char a=0x03;
	unsigned char k;
	
	unsigned int num;
	unsigned int nu[6]={1,2,3,4,5,7};
	nu[3]=nu[2];
	nu[2]=nu[1];
	nu[1]++;
	nu[0]++;
	nu[4]++;
	nu[5]++;
	if(k != a)
	num = __current_pc();
	num = num & 0x0003;
	fun();

	return 54;
}
