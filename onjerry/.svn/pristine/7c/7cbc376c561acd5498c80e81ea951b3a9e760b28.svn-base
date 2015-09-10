#ifndef _FLASH_H_
#define _FLASH_H_

#include "stm32f10x.h"

#define RDPRTKEY 0x00A5
#define UNLOCKKEY1 0x45670123
#define UNLOCKKEY2 0xCDEF89AB
#define FLASHUSERPAGE_ADDRSTART ((uint16_t *)0x08000000)
#define FLASHOPTION_ADDRSTART ((uint16_t *)0x1FFFF800)
#define FLASHOPTION_ADDREND ((uint16_t *)0x1FFFF80F)

#define FLASHOPTION_ADDRUSER ((uint16_t *)0x1FFFF802)
#define FLASHOPTION_ADDRDATA0 ((uint16_t *)0x1FFFF804)
#define FLASHOPTION_ADDRDATA1 ((uint16_t *)0x1FFFF806)
#define FLASHOPTION_ADDRWRP0 ((uint16_t *)0x1FFFF808)
#define FLASHOPTION_ADDRWRP1 ((uint16_t *)0x1FFFF80A)
#define FLASHOPTION_ADDRWRP2 ((uint16_t *)0x1FFFF80C)
#define FLASHOPTION_ADDRWRP3 ((uint16_t *)0x1FFFF80E)

#if defined STM32F10X_MD
 #define FLASHPAGE_WORDSIZE 256
 #define FLASHPAGE_ADDRMASK 0xFFFFFC00
 #define FLASHUSERPAGE_ADDREND ((uint16_t *)0x0800FFFF)
 #define FLASHADDR_CALSTART 0x0800FC00 //第63页,每页1K，选用的为64K F103R8T6型号
#elif defined STM32F10X_CL
 #define FLASHPAGE_WORDSIZE 512
 #define FLASHPAGE_ADDRMASK 0xFFFFF800
 #define FLASHUSERPAGE_ADDREND ((uint16_t *)0x0801FFFF)
 #define FLASHADDR_CALSTART 0x0801F800 //第63页，每页2K，选用的为128K F105RBT6型号
#endif

#define FLASH_CR_MASK0 (~(uint32_t)(FLASH_CR_PG|FLASH_CR_PER|FLASH_CR_MER))
#define FLASH_CR_MASK1 (~(uint32_t)(FLASH_CR_OPTPG|FLASH_CR_OPTER))

void Flash_Init(void);
uint8_t Flash_FPEC_Unlock(void);
void Flash_FPEC_Lock(void);
uint8_t Flash_OPT_Unlock(void);
uint8_t Flash_Halfword_Program(uint16_t *addr,uint16_t data);
void Flash_OPT_Lock(void);
uint8_t Flash_Page_Erase(uint32_t* page);
uint8_t Flash_Mass_Erase(void);
uint8_t Flash_OPTBYTE_Program(uint16_t *addr,uint16_t data);
uint8_t Flash_OPTBYTE_Erase(void);

uint8_t Flash_Pagedata_Write(uint16_t* opdata0, uint8_t * opdata1, uint16_t* dirflashaddr, uint16_t length0, uint16_t length1);

#endif
