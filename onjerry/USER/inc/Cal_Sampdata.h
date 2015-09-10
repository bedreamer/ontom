#ifndef _CAL_SAMPDATA_
#define _CAL_SAMPDATA_

#include "stm32f10x.h"
#include "AD_Sample.h"
#include "MyTypedef.h"
/*SAMP_Stream的cal_status控制校准流程，
每两位值分别代表校准启动01、校准准备10、
校准值写入完毕00、每两位分别代表
MX0U、BAT0U、MX1U、BAT1U、BAT0I、BAT1I校准流程状态*/

#define CALIBRATEDATA_SEGNUM 13 //每种采样数据校准段总数(13段)
#define CALIBRATEDATA_COLNUM 6 //需要校准的采样数据类型种类个数(MX0U、BAT0U、MX1U、BAT1U、BAT0I、BAT1I)
#define CALIBRATEDATA_ROWNUM (CALIBRATEDATA_SEGNUM*2)//每种采样数据所有校准段的k、b值总数(分为13段数据，每一段包含一组k、b值)
#define CALIBRATEDATA_FINISH 0x0000 //校准结束值
#define CALIBRATESTREAM_START SAM_STRUCT_MX0U//校准状态下采样数据起始位置
#define CALIBRATESTREAM_END SAM_STRUCT_BAT1U //校准状态下采样数据结束位置


#define MX0U_CALIBRATE_INDEX 0
#define BAT0U_CALIBRATE_INDEX 1
#define MX1U_CALIBRATE_INDEX 2
#define BAT1U_CALIBRATE_INDEX 3
#define BAT0I_CALIBRATE_INDEX 4
#define BAT1I_CALIBRATE_INDEX 5

#define CALIBRATEDATA_UADPIONTNUM (CALIBRATEDATA_SEGNUM)//电压数据校准点的个数
#define U_CALIBRATE_SEGVAL 50000000//电压校准段每段实际电压的1000000倍(V)
#define U_CALIBRATE_LOWVAL 200000000//电压最小的校准点，为实际电压的1000000倍(V)
#define U_CALIBRATE_HIGVAL (U_CALIBRATE_LOWVAL+(CALIBRATEDATA_UADPIONTNUM-1)*U_CALIBRATE_SEGVAL)



#define CALIBRATEDATA_IADPIONTNUM (10)//电流数据校准点的个数
#define I_CALIBRATE_SEGVAL 15000000//电流校准段每段实际电流，为实际电流的1000000倍(A)
#define I_CALIBRATE_LOWVAL 0
#define I_CALIBRATE_HIGVAL (I_CALIBRATE_LOWVAL+CALIBRATEDATA_IADPIONTNUM*I_CALIBRATE_SEGVAL)




//extern uint32_t DATA_SAMPLE_CAL[CALIBRATEDATA_COLNUM][CALIBRATEDATA_ROWNUM];//所有种类数据所有段的k、b

extern uint16_t DATA_SAMPLE_ADPOINT[CALIBRATEDATA_COLNUM][CALIBRATEDATA_SEGNUM];//所有种类数据校准点的AD值

//void Cal_kb_Init(uint16_t * ADpointdata, uint32_t (* KBdata)[CALIBRATEDATA_ROWNUM]);
uint32_t Cal_Val_Get(uint16_t ADdata, uint8_t index, uint32_t segval, uint8_t min);
void Cal_ADdata_Copy(uint8_t ADcalcol, SAMP_Stream * streamdata);
uint8_t Cal_ADscream_Change(SAMP_Stream * streamdata);
void Cal_Init(uint8_t * calmin, uint16_t* dataflasharr, uint16_t length0, uint16_t length1);
uint8_t Cal_DataPro(uint16_t* pdata, SAMP_Stream *dirdata);

#endif
