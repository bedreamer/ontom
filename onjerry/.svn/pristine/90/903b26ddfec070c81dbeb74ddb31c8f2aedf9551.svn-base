#include "Flash.h"
#include "UART.h"
#include "SYS_Init.h"
#include "Stat_control.h"
/*FLASH的SR寄存器EOP、PGERR、WRPRTERR标志
可用于中断类型的FLASH编程；
FLASH的CR寄存器中OPTER、OPTPG、MER、PER、PG功能
硬件上无法在选中一个功能的情况下再选另一种，
即不能同时将两个或者两个以上功能位set*/
//修改OPT Bytes ，必须先解锁FPEC，即CR寄存器，之后再解锁CR寄存器的OPTWRE位

void Flash_Init(void)//Flash初始化
{
    RCC->CR |= (uint32_t) 0x00000001;//开启HSI
    while(!(RCC->CR & (uint32_t) 0x00000002));//等待HSI ready标志
    /*if(FLASH->OBR & FLASH_OBR_WDG_SW)//是否未启用硬件看门狗
    {
        KM_BRANCH1_ON;   
        Flash_OPTBYTE_Erase();
        Flash_OPTBYTE_Program((uint16_t*)FLASHOPTION_ADDRUSER, 0x00FE);//设置硬件看门狗
        Flash_OPTBYTE_Program((uint16_t*)FLASHOPTION_ADDRDATA0, 0x00BB);
        //Flash_OPTBYTE_Program((uint16_t*)FLASHOPTION_ADDRDATA1, 0xFE0B);
        //Flash_OPTBYTE_Program((uint16_t*)FLASHOPTION_ADDRWRP0, 0x00FF);//设置写保护页
        //Flash_OPTBYTE_Program((uint16_t*)FLASHOPTION_ADDRWRP1, 0x00FF);
        //Flash_OPTBYTE_Program((uint16_t*)FLASHOPTION_ADDRWRP2, 0x00FF);
        //Flash_OPTBYTE_Program((uint16_t*)FLASHOPTION_ADDRWRP3, 0x00FF);
        Flash_OPT_Lock();
        Flash_FPEC_Lock();
        System_Reset();//产生system reset 使读保护生效,程序烧写之后，由于直接运行了程序需要Power on Restet
    }*/
}

uint8_t Flash_FPEC_Unlock(void)//Flash Program Erase Controller 解锁
{
    if(FLASH->CR & (uint32_t)FLASH_CR_LOCK)//Flash_CR是否为lock
    {
        FLASH->KEYR = UNLOCKKEY1;
        FLASH->KEYR = UNLOCKKEY2;
        if(FLASH->CR & (uint32_t)FLASH_CR_LOCK)//解锁失败，需要reset才能再次解锁
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
    if((Flash_FPEC_Unlock()==0x00)&&(!(FLASH->CR & (uint32_t)FLASH_CR_OPTWRE)))//Flash Program Erase Controller 解锁成功且邋Option Byte Write Enable位未使能
    {
        FLASH->OPTKEYR = UNLOCKKEY1;
        FLASH->OPTKEYR = UNLOCKKEY2;
        if(!(FLASH->CR & (uint32_t)FLASH_CR_OPTWRE))//Option Byte Write 使能失败
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
    if((!(FLASH->SR & (uint32_t)FLASH_SR_BSY))&&((addr <= FLASHUSERPAGE_ADDREND)&&(addr >= FLASHUSERPAGE_ADDRSTART)))//确保写在MAIN FLASH区域
    {
        FLASH->CR &= FLASH_CR_MASK0;//清除所有功能位
        FLASH->CR |= (uint32_t)FLASH_CR_PG;//设置编程功能
        *addr = data;
        while(FLASH->SR & (uint32_t)FLASH_SR_BSY);
        if(FLASH->SR & (uint32_t)(FLASH_SR_PGERR|FLASH_SR_WRPRTERR))//是否编程无错误
        {
            FLASH->SR = (uint32_t)(FLASH_SR_PGERR|FLASH_SR_WRPRTERR);//清错误
            return 0x01;
        }
        if(*addr == data && (FLASH->SR & (uint32_t)FLASH_SR_EOP))//数据写入成功
        {
            FLASH->SR = (uint32_t)FLASH_SR_EOP;//清编程结束标志
            return 0x00;
        }    
    }
    return 0x01;//数据写入失败
}

uint8_t Flash_Page_Erase(uint32_t* page)
{
    uint16_t size;
    if((!(FLASH->SR & (uint32_t)FLASH_SR_BSY))&&(Flash_FPEC_Unlock()==0x00))
    {
        FLASH->CR &= FLASH_CR_MASK0;//清除所有功能位
        FLASH->CR |= (uint32_t)FLASH_CR_PER;//设置页擦除功能
        FLASH->AR = (uint32_t)page;//设置需要包含在擦除页中地址
        FLASH->CR |= (uint32_t)FLASH_CR_STRT;//启动擦除操作
        while(FLASH->SR & (uint32_t)FLASH_SR_BSY);
        for(size = FLASHPAGE_WORDSIZE, page = (uint32_t*)((uint32_t)page & FLASHPAGE_ADDRMASK);size > 0;size--)
        {
            if(*page++ != 0xFFFFFFFF)//擦除失败
            {
                return 0x01;
            }
        }
        return 0x00;//擦除成功
    }
    return 0x01;//擦除失败
}

uint8_t Flash_Mass_Erase(void)//MAIN FLASH整块擦除，Information block不受影响
{
    if((!(FLASH->SR & FLASH_SR_BSY))&&(Flash_FPEC_Unlock()==0x00))
    {
        FLASH->CR &= FLASH_CR_MASK0;//清除所有功能位
        FLASH->CR |= (uint32_t)FLASH_CR_MER;//设置整块FLASH擦除功能
        FLASH->CR |= (uint32_t)FLASH_CR_STRT;//启动擦除操作
        while(FLASH->SR & (uint32_t)FLASH_SR_BSY);        
    }

    return 0x00;
}

/*OPTION Bytes编程，addr 2字节对齐，否则会出现HardFault(硬件错误)；
data 可以只需指定数据部分，反码部分由芯片自动写入，
不会置位FLASH_OBR寄存器的OPTERR标志，且最后需要进行
system reset启用对应设置*/
uint8_t Flash_OPTBYTE_Program(uint16_t *addr,uint16_t data)
{
    if((!(FLASH->SR & (uint32_t)FLASH_SR_BSY))&&(Flash_OPT_Unlock()==0x00)&&((addr <= FLASHOPTION_ADDREND)&&(addr >= FLASHOPTION_ADDRSTART)))//确保写在OPT Bytes区域
    {
        FLASH->CR &= FLASH_CR_MASK1;//清除所有OPT所有功能位
        FLASH->CR |= (uint32_t)FLASH_CR_OPTPG;//设置Option Byte Erase编程功能
        *addr = data;
        while(FLASH->SR & (uint32_t)FLASH_SR_BSY);
        if(FLASH->SR & (uint32_t)(FLASH_SR_PGERR|FLASH_SR_WRPRTERR))//是否编程无错误
        {
            FLASH->SR = (uint32_t)(FLASH_SR_PGERR|FLASH_SR_WRPRTERR);//清错误
            return 0x01;
        }
        if(*addr == data && (FLASH->SR & (uint32_t)FLASH_SR_EOP))//数据写入成功
        {
            FLASH->SR = (uint32_t)FLASH_SR_EOP;//清编程结束标志
            return 0x00;
        }  
    }
    return 0x00;
}

/*擦除Option Byte 后，RDP值变为0xFF，FLASH读保护使能,
其它Option Byte未编程字节都为0xFF；
擦除操作虽会使得数据字节和数据反码字节都为0xFF，
但不会导致置位FLASH_OBR寄存器的OPTERR标志，且对应0xFF时的相应功能
*/
uint8_t Flash_OPTBYTE_Erase(void)
{
    if((!(FLASH->SR & (uint32_t)FLASH_SR_BSY))&&(Flash_OPT_Unlock()==0x00))
    {
        FLASH->CR &= FLASH_CR_MASK1;//清除所有OPT所有功能位
        FLASH->CR |= (uint32_t)FLASH_CR_OPTER;//设置Option Byte Erase擦除功能
        FLASH->CR |= (uint32_t)FLASH_CR_STRT;//启动擦除操作
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
        if(Flash_Halfword_Program(dirflashaddr, 0x0453) == 0x01)//再写一个半字，表示FLASH已经写过
        {
            return 0x01;
        }
        Flash_FPEC_Lock();
        return 0x00;
    }
    return 0x01;
}

