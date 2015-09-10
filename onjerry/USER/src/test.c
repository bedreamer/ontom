#include "test.h"
#include "stm32f10x.h"


void System_Myinit(void)
{
    //ϵͳ��λ��Ĭ��ΪHSI 8MHz ��ΪSYSCLK��AHB��APB1��APB2��Ϊ8MHz
    RCC->CR |= (uint32_t) 0x00000001;//����ѡ��HSI
    RCC->CR &= (uint32_t) 0xFEF6FFFF;//���¹ر�CSS��PLL��HSE
    RCC->CR &= (uint32_t) 0xFFFBFFFF;//����ѡ��HSE not bypass
    RCC->CIR = (uint32_t) 0x009F0000;//��������жϱ�־���ҹر������ж�
    RCC->CFGR &= (uint32_t) 0xFF80C00C;
    /*����ѡ��USBPREΪPLL��1.5��Ƶ��PLLXTPREѡΪHSE��
    PLLSCRѡΪHSI/2��ʱ�ӡ�PLLMULΪX2��SWѡ��HSI�����ΪSYSCLK(���72MHz)
    AHB��APB1��APB2����Ƶ*/

    //��ʼ�������ⲿHSE 8MHz(not-bypass) ͨ��PLL����72MHz SYSCLK ��PLLMUL����ΪX9
    RCC->CFGR |= (uint32_t) 0x001d0000;//PLLXTPREѡΪHSE��PLLSCRѡΪPLLXTPRE���ʱ�ӡ�PLLMULΪX9��SW��Ȼѡ��HSI����(PLLѡ��ʹ�ܺ���ѡ��PLLMULΪ����)
    RCC->CR |= (uint32_t) 0x00010000;//ʹ��HSE
    while(!(RCC->CR & (uint32_t) 0x00020000));//�ȴ�HSE ready��־


    //��ʼ����AHB��APB1��APB2��Ƶ
    RCC->CFGR |= (uint32_t) 0x00000400;//APB1  /2,��������Ƶ
    
    FLASH->ACR  &= (uint32_t) 0xFFFFFFF0;//��FLITF�����LATENCY[2:0]��HLFCYAλ
    
    FLASH->ACR |= (uint32_t) 0x00000010;//ʹ��Prefetch Buffer
    FLASH->ACR |= (uint32_t) 0x00000002;//����LATENCY[2:0]Ϊ010��Two wait states����if 48 MHz < SYSCLK �� 72 MHz

    
    RCC->CR |= (uint32_t) 0x01000000;//ʹ��PLL
    while(!(RCC->CR & (uint32_t) 0x02000000));//�ȴ�PLL ready��־
    
    RCC->CFGR |= (uint32_t) 0x00000002;//SWѡΪPLL������ΪSYSCLK
    while(RCC->CFGR & (uint32_t) 0x0000000c != 0x00000008);//�ж�SW��ǰ�Ƿ�ѡ����PLL
    
    RCC->CR &= (uint32_t) 0xFFFFFFFE;//�ر�HSI��ע��:���������ڲ�FLASHд���߲���ʱҪ����HSI

    SCB->VTOR = FLASH_BASE | 0x00000000;// ָ���ж����������ʼλ�����ڲ�FLASH��ʼ�� 0x08000000����������ʼ��ֵ������128words���봦
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
