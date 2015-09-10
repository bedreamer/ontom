#include "Flash.h"
#include "UART.h"
#include "SYS_Init.h"
#include "Stat_control.h"
/*FLASH��SR�Ĵ���EOP��PGERR��WRPRTERR��־
�������ж����͵�FLASH��̣�
FLASH��CR�Ĵ�����OPTER��OPTPG��MER��PER��PG����
Ӳ�����޷���ѡ��һ�����ܵ��������ѡ��һ�֣�
������ͬʱ�����������������Ϲ���λset*/
//�޸�OPT Bytes �������Ƚ���FPEC����CR�Ĵ�����֮���ٽ���CR�Ĵ�����OPTWREλ

void Flash_Init(void)//Flash��ʼ��
{
    RCC->CR |= (uint32_t) 0x00000001;//����HSI
    while(!(RCC->CR & (uint32_t) 0x00000002));//�ȴ�HSI ready��־
    /*if(FLASH->OBR & FLASH_OBR_WDG_SW)//�Ƿ�δ����Ӳ�����Ź�
    {
        KM_BRANCH1_ON;   
        Flash_OPTBYTE_Erase();
        Flash_OPTBYTE_Program((uint16_t*)FLASHOPTION_ADDRUSER, 0x00FE);//����Ӳ�����Ź�
        Flash_OPTBYTE_Program((uint16_t*)FLASHOPTION_ADDRDATA0, 0x00BB);
        //Flash_OPTBYTE_Program((uint16_t*)FLASHOPTION_ADDRDATA1, 0xFE0B);
        //Flash_OPTBYTE_Program((uint16_t*)FLASHOPTION_ADDRWRP0, 0x00FF);//����д����ҳ
        //Flash_OPTBYTE_Program((uint16_t*)FLASHOPTION_ADDRWRP1, 0x00FF);
        //Flash_OPTBYTE_Program((uint16_t*)FLASHOPTION_ADDRWRP2, 0x00FF);
        //Flash_OPTBYTE_Program((uint16_t*)FLASHOPTION_ADDRWRP3, 0x00FF);
        Flash_OPT_Lock();
        Flash_FPEC_Lock();
        System_Reset();//����system reset ʹ��������Ч,������д֮������ֱ�������˳�����ҪPower on Restet
    }*/
}

uint8_t Flash_FPEC_Unlock(void)//Flash Program Erase Controller ����
{
    if(FLASH->CR & (uint32_t)FLASH_CR_LOCK)//Flash_CR�Ƿ�Ϊlock
    {
        FLASH->KEYR = UNLOCKKEY1;
        FLASH->KEYR = UNLOCKKEY2;
        if(FLASH->CR & (uint32_t)FLASH_CR_LOCK)//����ʧ�ܣ���Ҫreset�����ٴν���
        {
            return 0x01;
        }
    }
    return 0x00;
}

void Flash_FPEC_Lock(void)
{
    FLASH->CR |= (uint32_t)FLASH_CR_LOCK;
}


uint8_t Flash_OPT_Unlock(void)
{
    if((Flash_FPEC_Unlock()==0x00)&&(!(FLASH->CR & (uint32_t)FLASH_CR_OPTWRE)))//Flash Program Erase Controller �����ɹ�����Option Byte Write Enableλδʹ��
    {
        FLASH->OPTKEYR = UNLOCKKEY1;
        FLASH->OPTKEYR = UNLOCKKEY2;
        if(!(FLASH->CR & (uint32_t)FLASH_CR_OPTWRE))//Option Byte Write ʹ��ʧ��
        {
            return 0x01;
        }
    }
    return 0x00;
}

void Flash_OPT_Lock(void)
{
    if((FLASH->CR & (uint32_t)FLASH_CR_OPTWRE)&&(Flash_FPEC_Unlock()==0x00))
    {
        FLASH->CR &= ~(uint32_t)FLASH_CR_OPTWRE;
    }
}

uint8_t Flash_Halfword_Program(uint16_t *addr,uint16_t data)
{
    if((!(FLASH->SR & (uint32_t)FLASH_SR_BSY))&&((addr <= FLASHUSERPAGE_ADDREND)&&(addr >= FLASHUSERPAGE_ADDRSTART)))//ȷ��д��MAIN FLASH����
    {
        FLASH->CR &= FLASH_CR_MASK0;//������й���λ
        FLASH->CR |= (uint32_t)FLASH_CR_PG;//���ñ�̹���
        *addr = data;
        while(FLASH->SR & (uint32_t)FLASH_SR_BSY);
        if(FLASH->SR & (uint32_t)(FLASH_SR_PGERR|FLASH_SR_WRPRTERR))//�Ƿ����޴���
        {
            FLASH->SR = (uint32_t)(FLASH_SR_PGERR|FLASH_SR_WRPRTERR);//�����
            return 0x01;
        }
        if(*addr == data && (FLASH->SR & (uint32_t)FLASH_SR_EOP))//����д��ɹ�
        {
            FLASH->SR = (uint32_t)FLASH_SR_EOP;//���̽�����־
            return 0x00;
        }    
    }
    return 0x01;//����д��ʧ��
}

uint8_t Flash_Page_Erase(uint32_t* page)
{
    uint16_t size;
    if((!(FLASH->SR & (uint32_t)FLASH_SR_BSY))&&(Flash_FPEC_Unlock()==0x00))
    {
        FLASH->CR &= FLASH_CR_MASK0;//������й���λ
        FLASH->CR |= (uint32_t)FLASH_CR_PER;//����ҳ��������
        FLASH->AR = (uint32_t)page;//������Ҫ�����ڲ���ҳ�е�ַ
        FLASH->CR |= (uint32_t)FLASH_CR_STRT;//������������
        while(FLASH->SR & (uint32_t)FLASH_SR_BSY);
        for(size = FLASHPAGE_WORDSIZE, page = (uint32_t*)((uint32_t)page & FLASHPAGE_ADDRMASK);size > 0;size--)
        {
            if(*page++ != 0xFFFFFFFF)//����ʧ��
            {
                return 0x01;
            }
        }
        return 0x00;//�����ɹ�
    }
    return 0x01;//����ʧ��
}

uint8_t Flash_Mass_Erase(void)//MAIN FLASH���������Information block����Ӱ��
{
    if((!(FLASH->SR & FLASH_SR_BSY))&&(Flash_FPEC_Unlock()==0x00))
    {
        FLASH->CR &= FLASH_CR_MASK0;//������й���λ
        FLASH->CR |= (uint32_t)FLASH_CR_MER;//��������FLASH��������
        FLASH->CR |= (uint32_t)FLASH_CR_STRT;//������������
        while(FLASH->SR & (uint32_t)FLASH_SR_BSY);        
    }

    return 0x00;
}

/*OPTION Bytes��̣�addr 2�ֽڶ��룬��������HardFault(Ӳ������)��
data ����ֻ��ָ�����ݲ��֣����벿����оƬ�Զ�д�룬
������λFLASH_OBR�Ĵ�����OPTERR��־���������Ҫ����
system reset���ö�Ӧ����*/
uint8_t Flash_OPTBYTE_Program(uint16_t *addr,uint16_t data)
{
    if((!(FLASH->SR & (uint32_t)FLASH_SR_BSY))&&(Flash_OPT_Unlock()==0x00)&&((addr <= FLASHOPTION_ADDREND)&&(addr >= FLASHOPTION_ADDRSTART)))//ȷ��д��OPT Bytes����
    {
        FLASH->CR &= FLASH_CR_MASK1;//�������OPT���й���λ
        FLASH->CR |= (uint32_t)FLASH_CR_OPTPG;//����Option Byte Erase��̹���
        *addr = data;
        while(FLASH->SR & (uint32_t)FLASH_SR_BSY);
        if(FLASH->SR & (uint32_t)(FLASH_SR_PGERR|FLASH_SR_WRPRTERR))//�Ƿ����޴���
        {
            FLASH->SR = (uint32_t)(FLASH_SR_PGERR|FLASH_SR_WRPRTERR);//�����
            return 0x01;
        }
        if(*addr == data && (FLASH->SR & (uint32_t)FLASH_SR_EOP))//����д��ɹ�
        {
            FLASH->SR = (uint32_t)FLASH_SR_EOP;//���̽�����־
            return 0x00;
        }  
    }
    return 0x00;
}

/*����Option Byte ��RDPֵ��Ϊ0xFF��FLASH������ʹ��,
����Option Byteδ����ֽڶ�Ϊ0xFF��
�����������ʹ�������ֽں����ݷ����ֽڶ�Ϊ0xFF��
�����ᵼ����λFLASH_OBR�Ĵ�����OPTERR��־���Ҷ�Ӧ0xFFʱ����Ӧ����
*/
uint8_t Flash_OPTBYTE_Erase(void)
{
    if((!(FLASH->SR & (uint32_t)FLASH_SR_BSY))&&(Flash_OPT_Unlock()==0x00))
    {
        FLASH->CR &= FLASH_CR_MASK1;//�������OPT���й���λ
        FLASH->CR |= (uint32_t)FLASH_CR_OPTER;//����Option Byte Erase��������
        FLASH->CR |= (uint32_t)FLASH_CR_STRT;//������������
        while(FLASH->SR & (uint32_t)FLASH_SR_BSY);        
    }
    return 0x00;
}

uint8_t Flash_Pagedata_Write(uint16_t* opdata0, uint8_t * opdata1, uint16_t* dirflashaddr, uint16_t length0, uint16_t length1)
{
    if(!Flash_Page_Erase((uint32_t* )dirflashaddr))
    {
        for(; length0 != 0;length0--)
        {
            if(Flash_Halfword_Program(dirflashaddr++, *opdata0++) == 0x01)
            {
                return 0x01;
            }
        }
        for(; length1 != 0;length1--)
        {
            if(Flash_Halfword_Program(dirflashaddr++, *opdata1++) == 0x01)
            {
                return 0x01;
            }
        }
        if(Flash_Halfword_Program(dirflashaddr, 0x0453) == 0x01)//��дһ�����֣���ʾFLASH�Ѿ�д��
        {
            return 0x01;
        }
        Flash_FPEC_Lock();
        return 0x00;
    }
    return 0x01;
}

