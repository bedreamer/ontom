#include "test.h"


void WWDG_IRQHandler(void)
{
	unsigned char k;
	k++;
	
}
struct mystruct0
{
	unsigned char i,k;
	__packed unsigned short j;
}ms0;

struct mystruct1
{
	unsigned char i;
	__packed unsigned short j;
}ms1;

unsigned int fun(void)
{
	unsigned int i;
	return ++i;
}

static unsigned char buff[4] = {0,1,2,3};

int main(void)
{
	unsigned int num,nu;
	unsigned short temp;
	temp = *(unsigned short *)(buff+2);
	ms0.j = 12;
	ms1.j = 13;
	num = __current_pc();
	num = num & 0x0003;
	nu = __current_pc();
	fun();
	return 54;
}
