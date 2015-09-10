#ifndef _ADSAMPLE_H_
#define _ADSAMPLE_H_


#include "stm32f10x.h"
#include "MyTypedef.h"



//复用采样口，对应AD通道CH4、CH5

#define SAM_CH0 (0x0300)//三极管BA 00,选中母线电压、GND to GND、辅助电源电压
#define SAM_CH1 (0x0100)//三极管BA 01,选中电池0电压、电池1电压、LINK1
#define SAM_CH2 (0x0200)//三极管BA 10,选中绝缘电池0正对地、绝缘电池1正对地、LINK0
#define SAM_CH3 (0x0000)//三极管BA 11,选中绝缘电池0负对地、绝缘电池1负对地、GND to GND
#define SAM_SEL(sel) (GPIOC->ODR = (GPIOC->ODR & (~(uint32_t)(GPIO_ODR_ODR8 | GPIO_ODR_ODR9)))|(uint32_t)sel)

//绝缘检测
#define SAM_JYPSET   (GPIOC->BSRR = GPIO_BSRR_BS7)
#define SAM_JYPRSET (GPIOC->BSRR = GPIO_BSRR_BR7)
#define SAM_JYNSET   (GPIOC->BSRR = GPIO_BSRR_BS6)
#define SAM_JYNRSET (GPIOC->BSRR = GPIO_BSRR_BR6)


#define SAM_DATA_NUM 15 //采样数据种类个数
#define SAM_EVERYDATA_COUNT 5//每种数据连续AD采样次数

#define SAM_STRUCT_HUMI     0x0004 //湿度数据在Samp_DataArr偏移
#define SAM_HUMIDATA_AIN   0x0004//对应AD采样通道
#define SAM_HUMIDATA_DEFHIGH (uint16_t)4096
#define SAM_HUMIDATA_DEFLOW  (uint16_t)0
#define SAM_HUMIDATA_RESOLUTION 1 //AD一位对应实际湿度的倍


#define SAM_STRUCT_TEMP     0x0005 //温度数据在Samp_DataArr偏移
#define SAM_TEMPDATA_AIN   0x0005//对应AD采样通道
#define SAM_TEMPDATA_DEFHIGH (uint16_t)4096
#define SAM_TEMPDATA_DEFLOW  (uint16_t)0
#define SAM_TEMPDATA_RESOLUTION  1//AD一位对应实际温度的倍



#define SAM_STRUCT_BAT0I      0x0003 //电池0组电流数据在Samp_DataArr偏移
#define SAM_BAT0IDATA_AIN   0x0003//电池0组电流对应AD采样通道
#define SAM_BAT0IDATA_DEFHIGH   (uint16_t)1250
#define SAM_BAT0IDATA_DEFLOW    (uint16_t)0
#define SAM_BAT0IDATA_RESOLUTION  //电池0组电流AD一位对应实际电流的倍

#define SAM_STRUCT_BAT1I       0x0006//电池1组电流数据在Samp_DataArr偏移
#define SAM_BAT1IDATA_AIN   SAM_BAT0IDATA_AIN//电池1组对应AD采样通道
#define SAM_BAT1IDATA_DEFHIGH   SAM_BAT0IDATA_DEFHIGH
#define SAM_BAT1IDATA_DEFLOW    SAM_BAT0IDATA_DEFLOW
#define SAM_BAT1IDATA_RESOLUTION  //电池1组电流AD一位对应实际电流的倍
#define SAM_BATIDATA_RESOLUTION 414299//电池电流的电压信号AD一位对应实际电流的10000000倍mV(所有电池组电流的采样电路是一样的情况)
#define SAM_BATIDATA_SENSORSOLUTION 2//分流器比值A/mV


#define SAM_STRUCT_MX0U       0x0000 //母线0电压数据在Samp_DataArr偏移
#define SAM_MX0UDATA_AIN   0x0000//母线0电压对应AD采样通道
#define SAM_MX0UDATA_DEFHIGH  (uint16_t)7600
#define SAM_MX0UDATA_DEFLOW   (uint16_t)2000
#define SAM_MX0UDATA_RESOLUTION //母线0电压AD一位对应实际母线电压的1000000倍

#define SAM_STRUCT_MX1U       0x0001 //母线1电压数据在Samp_DataArr偏移
#define SAM_MX1UDATA_AIN   SAM_MX0UDATA_AIN //母线1电压对应AD采样通道
#define SAM_MX1UDATA_DEFHIGH  SAM_MX0UDATA_DEFHIGH
#define SAM_MX1UDATA_DEFLOW   SAM_MX0UDATA_DEFLOW
#define SAM_MX1UDATA_RESOLUTION //母线1电压AD一位对应实际母线电压的1000000倍
#define SAM_MXUDATA_RESOLUTION 397713//母线电压AD一位对应实际电压的倍(所有母线电压的采样电路是一样的情况)



#define SAM_STRUCT_BAT0U     0x0007 //电池0组电压数据在Samp_DataArr偏移
#define SAM_BAT0UDATA_AIN   0x0000//电池0组电压对应AD采样通道
#define SAM_BAT0UDATA_DEFHIGH (uint16_t)7600
#define SAM_BAT0UDATA_DEFLOW  (uint16_t)2000
#define SAM_BAT0UDATA_RESOLUTION //电池0组电压AD一位对应实际电池电压的1000000倍

#define SAM_STRUCT_BAT1U     0x0008 //电池1组电压数据在Samp_DataArr偏移
#define SAM_BAT1UDATA_AIN   0x0001//电池1组对应AD采样通道
#define SAM_BAT1UDATA_DEFHIGH (uint16_t)7600
#define SAM_BAT1UDATA_DEFLOW  (uint16_t)2000
#define SAM_BAT1UDATA_RESOLUTION //电池1组AD一位对应实际电池电压的1000000倍
#define SAM_BATUDATA_RESOLUTION 397713//电池电压AD一位对应实际电池电压的1000000倍(所有电池电压的采样电路是一样的情况)




#define SAM_JYBATRESDATA_DEFSAMPLERES 2040//电池组绝缘平衡桥投切电阻扩大10倍K(所有电池组绝缘平衡桥投切采样电阻是一样的情况)
#define SAM_STRUCT_JYBAT0RES     //电池0组绝缘接地电阻数据在Samp_DataArr偏移
#define SAM_JYBAT0RESDATA_AIN   //对应AD采样通道
#define SAM_JYBAT0RESDATA_DEFHIGH  
#define SAM_JYBAT0RESDATA_DEFLOW  (uint16_t)100//绝缘接地下限最小100Ω/V
#define SAM_JYBAT0RESDATA_RESOLUTION
#define SAM_JYBAT0RESDATA_DEFSAMPLERES SAM_JYBATRESDATA_DEFSAMPLERES//电池0组绝缘平衡桥投切采样电阻204K

#define SAM_STRUCT_JYBAT1RES     //绝缘接地电阻数据在Samp_DataArr偏移
#define SAM_JYBAT1RESDATA_AIN   //对应AD采样通道
#define SAM_JYBAT1RESDATA_DEFHIGH  
#define SAM_JYBAT1RESDATA_DEFLOW  (uint16_t)100//绝缘接地下限最小100Ω/V
#define SAM_JYBAT1RESDATA_RESOLUTION
#define SAM_JYBAT1RESDATA_DEFSAMPLERES SAM_JYBATRESDATA_DEFSAMPLERES//电池1组绝缘平衡桥投切采样电阻204K

#define SAM_STRUCT_JYBAT0P  0x000A //电池0组绝缘正对地数据在Samp_DataArr偏移
#define SAM_JYBAT0PDATA_AIN   0x0000//电池0组绝缘正对地对应AD采样通道
#define SAM_JYBAT0PDATA_DEFHIGH  SAM_STRUCT_BAT0U//电池0组电压数据在Samp_DataArr偏移，放于上限变量中
#define SAM_JYBAT0PDATA_DEFLOW   (uint16_t)SAM_JYBAT0RESDATA_DEFSAMPLERES//电池0组绝缘平衡桥投切采样电阻K的100倍 ，放于上限变量中
#define SAM_JYBAT0PDATA_RESOLUTION

#define SAM_STRUCT_JYBAT1P  0x000B //电池1组绝缘正对地数据在Samp_DataArr偏移
#define SAM_JYBAT1PDATA_AIN   0x0001//电池1组绝缘正对地对应AD采样通道
#define SAM_JYBAT1PDATA_DEFHIGH  SAM_STRUCT_BAT1U//电池1组电压数据在Samp_DataArr偏移
#define SAM_JYBAT1PDATA_DEFLOW   (uint16_t)SAM_JYBAT1RESDATA_DEFSAMPLERES//电池1组绝缘平衡桥投切采样电阻K的100倍 ，放于上限变量中
#define SAM_JYBAT1PDATA_RESOLUTION
#define SAM_JYBATPDATA_RESOLUTION //电池绝缘正对地AD一位对应实际电池电压的倍(所有电池绝缘正对地的采样电路是一样的情况)



#define SAM_STRUCT_JYBAT0N  0x000D //电池0组绝缘负对地数据在Samp_DataArr偏移
#define SAM_JYBAT0NDATA_AIN   0x0000//对应AD采样通道
#define SAM_JYBAT0NDATA_DEFHIGH  SAM_STRUCT_JYBAT0P//电池0组绝缘正对地电压数据在Samp_DataArr偏移
#define SAM_JYBAT0NDATA_DEFLOW   SAM_JYBAT0RESDATA_DEFLOW
#define SAM_JYBAT0NDATA_RESOLUTION

#define SAM_STRUCT_JYBAT1N  0x000E //电池1组绝缘负对地数据在Samp_DataArr偏移
#define SAM_JYBAT1NDATA_AIN   0x0001//对应AD采样通道
#define SAM_JYBAT1NDATA_DEFHIGH  SAM_STRUCT_JYBAT1P//电池1组绝缘正对地电压数据在Samp_DataArr偏移
#define SAM_JYBAT1NDATA_DEFLOW   SAM_JYBAT1RESDATA_DEFLOW
#define SAM_JYBAT1NDATA_RESOLUTION
#define SAM_JYBATNDATA_RESOLUTION //电池绝缘负对地AD一位对应实际电池电压的倍(所有电池绝缘负对地的采样电路是一样的情况)







#define SAM_STRUCT_FZDYU    0x0002 //辅助电源电压数据在Samp_DataArr偏移
#define SAM_FZDYUDATA_AIN   0x0002//对应AD采样通道
#define SAM_FZDYUDATA_DEFHIGH (uint16_t)250
#define SAM_FZDYUDATA_DEFLOW  (uint16_t)230
#define SAM_FZDYUDATA_RESOLUTION  18311//AD一位对应实际辅助电源电压的1000000倍

#define SAM_STRUCT_LINK1U   0x0009 //LIKE1连接电压数据在Samp_DataArr偏移
#define SAM_LINK1UDATA_AIN   0x0002//对应AD采样通道
#define SAM_LINK1UDATA_DEFHIGH (uint16_t)120
#define SAM_LINK1UDATA_DEFLOW  (uint16_t)0
#define SAM_LINK1UDATA_RESOLUTION 

#define SAM_STRUCT_LINK0U   0x000C //LIKE0连接电压数据在Samp_DataArr偏移
#define SAM_LINK0UDATA_AIN   0x0002//对应AD采样通道
#define SAM_LINK0UDATA_DEFHIGH (uint16_t)120
#define SAM_LINK0UDATA_DEFLOW  (uint16_t)0
#define SAM_LINK0UDATA_RESOLUTION  
#define SAM_LINKUDATA_RESOLUTION //LINK连接电压AD一位对应实际电池电压的倍(所有LINK连接电压的采样电路是一样的情况)




/*每个通道定时采样次数，
注:这个次数必须符合绝缘母线
对地信号50Hz的一个周期采样，
以确保滤除这个信号*/
#define SAM_CHANNEL_TIMERCOUNT 40

//采样得到的状态数据结构体定义
#define SAM_DATASTATUS_LEN 40

#define SAMPDATABITINDEX_MX0USTAT  0//母线0电压状态数据在字节中的偏移
#define SAMPDATABITMASK_MX0USTAT  (~((uint8_t)0x07<<SAMPDATABITINDEX_MX0USTAT))

#define SAMPDATABITINDEX_MX1USTAT  3//母线1电压状态数据在字节中的偏移
#define SAMPDATABITMASK_MX1USTAT  (~((uint8_t)0x07<<SAMPDATABITINDEX_MX1USTAT))

#define SAMPDATABITINDEX_BAT0USTAT  0//电池0组电压状态数据在字节中的偏移
#define SAMPDATABITMASK_BAT0USTAT   (~((uint8_t)0x0F<<SAMPDATABITINDEX_BAT0USTAT))

#define SAMPDATABITINDEX_BAT1USTAT  0//电池1组电压状态数据在字节中的偏移
#define SAMPDATABITMASK_BAT1USTAT   (~((uint8_t)0x0F<<SAMPDATABITINDEX_BAT1USTAT))

#define SAMPDATABITINDEX_BAT0ISTAT  5//电池0组电流状态数据在字节中的偏移
#define SAMPDATABITMASK_BAT0ISTAT  (~((uint8_t)0x01<<SAMPDATABITINDEX_BAT0ISTAT))

#define SAMPDATABITINDEX_BAT1ISTAT  5//电池1组电流状态数据在字节中的偏移
#define SAMPDATABITMASK_BAT1ISTAT  (~((uint8_t)0x01<<SAMPDATABITINDEX_BAT1ISTAT))

#define SAMPDATABITINDEX_JYBAT0PSTAT    //电池0组绝缘正对地状态数据在字节中偏移
#define SAMPDATABITMASK_JYBAT0PSTAT

#define SAMPDATABITINDEX_JYBAT1PSTAT    //电池1组绝缘正对地状态数据在字节中偏移
#define SAMPDATABITMASK_JYBAT1PSTAT

#define SAMPDATABITINDEX_JYBAT0NSTAT     //电池0组绝缘负对地状态数据在字节中偏移
#define SAMPDATABITMASK_JYBAT0NSTAT 

#define SAMPDATABITINDEX_JYBAT1NSTAT     //电池1组绝缘负对地状态数据在字节中偏移
#define SAMPDATABITMASK_JYBAT1NSTAT

#define SAMPDATABITINDEX_JYRESBAT0STAT  4//电池0组绝缘对地电阻状态数据在字节中偏移
#define SAMPDATABITMASK_JYRESBAT0STAT   (~((uint8_t)0x01<<SAMPDATABITINDEX_JYRESBAT0STAT))

#define SAMPDATABITINDEX_JYRESBAT1STAT  4//电池1组绝缘对地电阻状态数据在字节中偏移
#define SAMPDATABITMASK_JYRESBAT1STAT   (~((uint8_t)0x01<<SAMPDATABITINDEX_JYRESBAT1STAT))

#define SAMPDATABITINDEX_TEMPSTAT  1//温度状态数据在字节中的偏移
#define SAMPDATABITMASK_TEMPSTAT  (~((uint8_t)0x03<<SAMPDATABITINDEX_TEMPSTAT))

#define SAMPDATABITINDEX_HUMISTAT  3//湿度状态数据在字节中的偏移
#define SAMPDATABITMASK_HUMISTAT  (~((uint8_t)0x03<<SAMPDATABITINDEX_HUMISTAT))

#define SAMPDATABITINDEX_FZDYUSTAT     0//辅助电源电压状态数据在字节中的偏移
#define SAMPDATABITMASK_FZDYUSTAT      (~((uint8_t)0x01<<SAMPDATABITINDEX_FZDYUSTAT))

#define SAMPDATABITINDEX_LINK0USTAT     5//LIKE0连接电压状态数据在字节中的偏移
#define SAMPDATABITMASK_LINK0USTAT      (~((uint8_t)0x03<<SAMPDATABITINDEX_LINK0USTAT))

#define SAMPDATABITINDEX_LINK1USTAT     1//LIKE1连接电压状态数据在字节中的偏移
#define SAMPDATABITMASK_LINK1USTAT      (~((uint8_t)0x03<<SAMPDATABITINDEX_LINK1USTAT))

//充电断开保护类状态偏移(异步状态输入字节1部分)
#define SAMPDATABIT_KMMAINRDQSTAT ((uint8_t)0x01<<0)
#define SAMPDATABIT_KMMAINTRIPSTAT ((uint8_t)0x01<<1)
#define SAMPDATABIT_KMBRANCH1TRIPSTAT ((uint8_t)0x01<<2)
#define SAMPDATABIT_KMBRANCH2TRIPSTAT ((uint8_t)0x01<<3)
#define SAMPDATABIT_FLQSTAT ((uint8_t)0x01<<4)
#define SAMPDATABIT_ACINTRIPSTAT ((uint8_t)0x01<<5)
#define SAMPDATABIT_STOPSTAT ((uint8_t)0x01<<6)

//运行状态字节偏移定义
#define SW_AC_IN_OFFSET ((uint8_t)0x01)//交流输入合闸状态在RUNSTATUS1的偏移
#define SW_KM_MAIN_OFFSET ((uint8_t)0x01<<3) //总直流输出合闸状态在RUNSTATUS1的偏移
#define SW_KM_BRANCH1_OFFSET ((uint8_t)0x01<<4)//LINK0合闸状态在RUNSTATUS1的偏移
#define SW_FZDY_BRANCH1_OFFSET ((uint8_t)0x01<<7)//FZDY在LINK0一侧继电器状态在RUNSTATUS1的偏移

#define SW_KM_BRANCH2_OFFSET ((uint8_t)0x01<<0)//LINK1合闸状态在RUNSTATUS2的偏移
#define SW_FZDY_BRANCH2_OFFSET ((uint8_t)0x01<<3)//FZDY在LINK1一侧继电器状态在RUNSTATUS2的偏移


typedef struct 
{
    uint16_t VinKM0; //母线0电压 实际电压*10
    uint16_t VinKM1;//母线1电压 实际电压*10
    uint16_t VinBAT0;//电池0组电压，实际电压*10
    uint16_t VinBAT1;//电池1组电压，实际电压*10
    uint16_t IoutBAT0; //电池0组电流，实际电流*10
    uint16_t IoutBAT1; //电池1组电流，实际电流*10
    uint16_t VinBAT0RESP;//电池0组正对地电阻，实际电阻*10（K）
    uint16_t VinBAT1RESP;//电池1组正对地电阻，实际电阻*10（K）
    uint16_t VinBAT0RESN;//电池0组负对地电阻，实际电阻*10（K）
    uint16_t VinBAT1RESN;//电池1组负对地电阻，实际电阻*10（K）
    uint16_t Temp ;//当前温度，实际温度*10
    uint16_t Humi; //当前湿度，实际湿度*10
    uint16_t Reserve2;// 预留	   
    uint16_t Reserve3; //预留 
    uint8_t Ver; //软件版本号， 如10 表示 Ver= 1.0;  数据一共29字节
    uint8_t Flag_prtc1;   //充电断开类保护状态字节1：（同步状态）
    //BIT0：母线0过压状态位；0 = 无故障，1 = 母线过压； 
    //BIT1：母线0欠压状态位；0 = 无故障，1 = 母线欠压； 
    //BIT2：母线0短路状态位；0 = 无故障，1 = 母线短路；
    //BIT3：母线1过压状态位；0 = 无故障，1 = 母线过压； 
    //BIT4：母线1欠压状态位；0 = 无故障，1 = 母线欠压； 
    //BIT5：母线1短路状态位；0 = 无故障，1 = 母线短路；
    //剩余bit保留
    uint8_t Flag_prtc2;   //充电断开类保护状态字节2：（同步状态） 
    //BIT0：电池0组过压状态位；0 = 无故障，1 = 电池过压； 
    //BIT1：电池0组欠压状态位：0 = 无故障，1 = 电池欠压； 
    //BIT2：电池0组短路状态位：0 = 无故障，1 = 电池短路； 
    //BIT3：电池0组反接状态位；0 = 无故障，1 = 电池反接； 
    //BIT4：电池0组绝缘状态位；0 = 无故障，1 = 电池绝缘故障； 
    //BIT5：电池0组过流状态位；0 = 无故障，1 = 电池过流； 
    //剩余bit保留
    uint8_t Flag_prtc3;   //充电断开类保护状态字节3：（同步状态） 
    //BIT0：电池1组过压状态位；0 = 无故障，1 = 电池过压； 
    //BIT1：电池1组欠压状态位：0 = 无故障，1 = 电池欠压； 
    //BIT2：电池1组短路状态位：0 = 无故障，1 = 电池短路； 
    //BIT3：电池1组反接状态位；0 = 无故障，1 = 电池反接； 
    //BIT4：电池1组绝缘状态位；0 = 无故障，1 = 电池绝缘故障； 
    //BIT5：电池1组过流状态位；0 = 无故障，1 = 电池过流；
    //剩余bit保留
    uint8_t Flag_prtc4;   //充电断开类保护状态字节4：（同步状态）
    //BIT0：辅助电源状态位；0 = 无故障，1 = 辅助电源异常； 
    //BIT1..2：温度状态位；0 0= 无故障，01 = 温度过高，10= 温度过低
    //BIT3..4：湿度状态位；0 0= 无故障，01 = 湿度过大，10=湿度过低
    //BIT5：通讯超时故障（一般采样板自用）
    //剩余bit保留
    uint8_t Flag_prtc5;   //充电断开类保护状态字节5：保留（同步状态）
    uint8_t Flag_prtc6;   //充电断开类保护状态字节4：（异步状态）
    //BIT0：总直流输出熔断器状态位：0 = 无故障，1 = 熔断； 
    //BIT1：总直流输出跳闸状态位：0 = 无故障，1 = 跳闸；
    //BIT2：充电1路输出跳闸状态位；0 = 无故障，1 = 跳闸；
    //BIT3：充电2路输出跳闸状态位；0 = 无故障，1 = 跳闸；
    //BIT4：防雷器状态位；0 = 无故障，1 = 防雷器故障；
    //BIT5：交流输入跳闸；0 = 无故障，1 = 跳闸；
    //BIT6：急停状态位；0 = 无故障，1 = 急停；
    //BIT7：保留
    uint8_t Flag_prtc7;   //充电断开类保护状态字节5：（异步状态）
    //保留
    uint8_t Flag_run1;   //当前运行状态字节1：
    //BIT 0: 交流输入是否合闸： 0 = 分；1 = 合； 
    //BIT 1：加热状态：    0 = 未加热；1 = 加热； 
    //BIT 2：风扇状态：  0 = 未启动； 1 = 启动； 
    //BIT 3：总直流输出是否合闸：0 =分；1 =合； 
    //BIT 4：充电枪1合闸状态： 0 = 分； 1 = 合； 
    //BIT 5..6：充电枪1连接状态：00= 未连接； 1 1= 连接；01=连接保护状态；
    //10= 连接异常
    //BIT 7：充电枪1通信辅助电源合闸状态： 0 = 分； 1 = 合；
    uint8_t Flag_run2;   //当前运行状态字节2：
    //BIT 0：充电枪2合闸状态：0 = 分； 1 = 合；
    //BIT 1..2：充电枪2连接状态：00 = 未连接； 11 = 连接；01=连接保护状态；10= 连接异常
    //BIT 3：充电枪2通信辅助电源合闸状态： 0 = 分； 1 = 合；
    uint8_t Flag_run3;   //当前运行状态字节3：保留 状态一共10字节
    uint8_t Reserve4;
}Stu_ADSample_DATAStat;

typedef union
{
    Stu_ADSample_DATAStat DataStat;
    uint8_t data[SAM_DATASTATUS_LEN];

}Un_ADSamp_Stat;

typedef struct AD_Sample_Status
{
    uint8_t ready;//准备选择的采样的数据偏移标号
    uint8_t now;//当前选择的采样的数据偏移标号
    uint16_t cmd;//当前继电器控制字节
    //uint8_t flash_status;//flash写状态
    uint16_t SysFault;//当前系统运行故障，bit0 UART通讯故障
    uint32_t Cal_UsegVal;//电压校准每段实际电压*1000000
    uint32_t Cal_IsegVal;//电流校准每段实际电流*1000000

    uint8_t Cal_UIchmin[2];//已经校准过的电压电流最小值
    uint16_t cal_status;//校准数据流程标志
    uint16_t cal_ch0;//0组数据期望校准值，母线0组和电池0组电压
    uint16_t cal_ch1;//1组数据期望校准值，母线1组和电池1组电压
    uint16_t cal_ch2;//2组数据期望校准值，母线0段和1段电流
}SAMP_Stream;

typedef struct AD_Sample_Option
{
    uint16_t Chan;//模拟采样输入通道
    uint16_t Ain;//模拟采样输出通道，即AD通道
    uint16_t ADdata;//当前采样AD值
    uint16_t highdata;//对应采样值的上限
    uint16_t lowdata;//对应采样值的下限
    uint8_t ADcalarrcol;//对应校准值数组列号
    uint16_t (*Opratefun)(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata);//对应采样通道的处理函数
    
}SAMP_Opx;//对应数值采样的数据处理和功能实现结构体

extern SAMP_Opx Samp_DataArr[SAM_DATA_NUM];
extern SAMP_Stream Samp_ADdata_Stream;
extern Un_ADSamp_Stat Samp_DataStat;

uint16_t DEFAULT_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata);
uint16_t HUMI_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata);
uint16_t TEMP_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata);
uint16_t BATI_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata);
uint16_t MXU_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata);
uint16_t BATU_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata);
uint16_t JYBATP_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata);
uint16_t JYBATN_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata);
uint16_t FZDYU_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata);
uint16_t LINKU_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata);
uint16_t JYRES_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata);

uint8_t ADSampe_LimitDataPro(uint16_t* pdata, SAMP_Stream *streamdata);
uint16_t ADSample_Change(SAMP_Stream *sam_status);
uint16_t ADSample_AnalogCHsel(uint32_t chan);
void ADSample_Scream_Col(Stu_ADSample_DATAStat *sampdata, SAMP_Stream* sam_status);
void ADSample_Init(void);

#endif



