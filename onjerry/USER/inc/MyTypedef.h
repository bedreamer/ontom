#ifndef _MYTYPEDEF_H_
#define _MYTYPEDEF_H_



typedef struct 
{
    uint16_t Cmd1;// 命令字节1
    //bit0: 总直流输出是否闭合：0：断开 1：闭合
    //bit1: 充电枪1辅助电源是否闭合：0：断开 1：闭合
    //bit2：充电枪1充电开关是否闭合：0：断开 1：闭合
    //bit3: 充电枪1电子锁是否闭合：0：断开 1：闭合
    //bit4：充电枪2辅助电源是否闭合：0：断开 1：闭合
    //bit5：充电枪2充电开关是否闭合：0：断开 1：闭合
    //bit6: 充电枪2电子锁是否闭合：0：断开 1：闭合
    //bit7: 充电运行：0：断开 1：闭合
    //bit8: 故障灯控制：0：断开 1：闭合
    //剩余bit保留
    uint16_t Cmd1_Backup;//值与Cmd1一样
    uint16_t Max_VinKM0;//母线0电压上限，实际电压*10
    uint16_t Min_VinKM0;//母线0电压下限，实际电压*10
    uint16_t Max_VinKM1;//母线1电压上限，实际电压*10
    uint16_t Min_VinKM1;//母线1电压下限，实际电压*10
    uint16_t Max_VinBAT0;//电池0电压上限，实际电压*10
    uint16_t Min_VinBAT0;//电池0电压下限，实际电压*10
    uint16_t Max_VinBAT1;//电池1电压上限，实际电压*10
    uint16_t Min_VinBAT1;//电池1电压下限，实际电压*10
    uint16_t Max_IoutBAT0 ;//电池0组过流上限，实际电流*10
    uint16_t Max_IoutBAT1 ;//电池1组过流上限，实际电流*10
    uint16_t Min_VinBAT0RES ;//电池0组绝缘告警值Ω/V
    uint16_t Min_VinBAT1RES ; //电池1组绝缘告警值Ω/V
    uint16_t Max_Isen ;//分流器最大电流A
    uint16_t Cal_CH0 ;//KM0和BAT0当前校准电压，0为不校准
    uint16_t Cal_CH1 ; //KM1和BAT1当前校准电压，0为不校准
    uint16_t Cal_CH2 ; // BAT0和BAT1当前校准电流，0为不校准
    uint8_t  New_Addr;//地址新设置值
    uint8_t  Reserve;//保留
    /*输出IO配置输出IO配置，Byte0: 充电枪1辅助电源，
    Byte1: 充电枪1充电开关，Byte2: 充电枪1电子锁，
    Byte3: 充电枪2辅助电源，Byte4: 充电枪2充电开关，
    Byte5: 充电枪2电子锁，Byte6: 充电运行，
    Byte7: 故障灯，剩余保留每个字节表示对应继电器号*/
    uint8_t OutMemb[16];



}Stu_ADSample_SetData;

#define BIGTOLIT_U16(dirnum) ((((uint16_t)(dirnum) << 8)|((uint16_t)(dirnum) >> 8)) & 0x0000FFFF)//大端转小端




#endif

