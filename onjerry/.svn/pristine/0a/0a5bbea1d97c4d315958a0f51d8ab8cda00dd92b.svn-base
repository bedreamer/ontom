#include "Cal_Sampdata.h"
#include "Flash.h"


/*uint32_t DATA_SAMPLE_CAL[CALIBRATEDATA_COLNUM][CALIBRATEDATA_ROWNUM]=
{0};*/
static uint16_t DATA_SAMPLE_ADPOINT[CALIBRATEDATA_COLNUM][CALIBRATEDATA_SEGNUM]=
{{5060,6280,7500,8720,9940,11160,12370,13540,14690,15770,17340,18910,20470},
{5060,6280,7500,8720,9940,11160,12370,13540,14690,15770,17340,18910,20470},
{5060,6280,7500,8720,9940,11160,12370,13540,14690,15770,17340,18910,20470},
{5060,6280,7500,8720,9940,11160,12370,13540,14690,15770,17340,18910,20470},
{0x0000,1850,3650,5440,7240,9010,10820,12500,14240,15870,17330,18900,20460},
{0x0000,1850,3650,5440,7240,9010,10820,12500,14240,15870,17330,18900,20460}};

/*uint16_t  const DATA_FLASH_ADPOINT[CALIBRATEDATA_COLNUM][CALIBRATEDATA_SEGNUM] __attribute__((at(FLASHADDR_CALSTART))) =
{{0x03E2,0x045C,0x04D5,0x054A,0x05BD,0x0629,0x06C6,0x0763,0x7FF},
{0x03E2,0x045B,0x04D4,0x0549,0x05BC,0x0628,0x06C5,0x0762,0x7FE},
{0x03E6,0x0461,0x04DB,0x0552,0x05C6,0x0634,0x06C6,0x0763,0x7FF},
{0x03E6,0x0461,0x04DB,0x0552,0x05C6,0x0634,0x06C5,0x0762,0x7FE}};*/

/*void Cal_kb_Init(uint16_t * ADpointdata, uint32_t (* KBdata)[CALIBRATEDATA_ROWNUM])
{
    uint8_t i,j;
    uint32_t temp0,temp1;
    for(i=0;i<CALIBRATEDATA_COLNUM;i++)
    {
        temp0 = (uint32_t)(*ADpointdata-2048);
        KBdata[i][0] = U_CALIBRATE_LOWVAL / temp0 + ((U_CALIBRATE_LOWVAL % temp0) /(temp0 / 2));//0V到最小校准电压段计算k
        KBdata[i][1] = 0;//0V到最小校准电压段b为0
        for(j=2, temp1 = U_CALIBRATE_LOWVAL; j<CALIBRATEDATA_ROWNUM; j++)
        {
            temp0 = (uint32_t)(*(ADpointdata+1) - *ADpointdata);
            KBdata[i][j] = U_CALIBRATE_SEGVAL /temp0 + ((U_CALIBRATE_SEGVAL % temp0) /(temp0 / 2));//计算k值
            temp0 = KBdata[i][j++] * (uint32_t)(*(++ADpointdata)-2048);
            temp1 += U_CALIBRATE_SEGVAL;
            //KBdata[i][j] = (temp1>=temp0)?(temp1-temp0):((temp0-temp1) | 0x80000000);//计算b值,正负由最高位区分
            KBdata[i][j] = temp1-temp0;
        }
        ADpointdata++;
    }
}*/

void Cal_Init(uint8_t * calmin, uint16_t* dataflasharr, uint16_t length0, uint16_t length1)
{
    uint16_t* caldata = (uint16_t *)DATA_SAMPLE_ADPOINT;
    if(*(dataflasharr+length0+length1) != 0x0453)
    {
        return;
    }
    for(;length0!=0;length0--)
    {
        *caldata++ = *dataflasharr++;
    }
    for(;length1!=0;length1--)
    {
        *calmin++ = (uint8_t)*dataflasharr++;
    }
}

uint8_t Cal_DataPro(uint16_t* pdata, SAMP_Stream *dirdata)//校准数据通讯设置处理
{
    static uint8_t cal_flag;
    uint16_t* caldataarr = (uint16_t *)DATA_SAMPLE_ADPOINT;
    if(!(dirdata->cal_status & 0x000F))//MX0U、BAT0U 新校准可进行
    {
        if(BIGTOLIT_U16(*pdata) != CALIBRATEDATA_FINISH)//是否为结束校准数据
        {
            if((BIGTOLIT_U16(*pdata) >= (U_CALIBRATE_LOWVAL/100000) && BIGTOLIT_U16(*pdata) <= (U_CALIBRATE_HIGVAL/100000)) && (BIGTOLIT_U16(*pdata) % (U_CALIBRATE_SEGVAL/100000) == 0x00))//校准值是否符合要求
            {
                if((BIGTOLIT_U16(*pdata) /(U_CALIBRATE_SEGVAL/100000)-(U_CALIBRATE_LOWVAL / U_CALIBRATE_SEGVAL)) < dirdata->Cal_UIchmin[0])
                {
                    dirdata->Cal_UIchmin[0] = (uint8_t)(BIGTOLIT_U16(*pdata) / (U_CALIBRATE_SEGVAL/100000)-(U_CALIBRATE_LOWVAL / U_CALIBRATE_SEGVAL));
                }
                dirdata->cal_status |= 0x0005;//启动校准
                cal_flag |= 0x01;//置位需写入Flash标志
            }
            else
            {
                return 0x01;
            }                   
        }
        dirdata->cal_ch0 = BIGTOLIT_U16(*pdata);  //赋值校准数据，包括结束校准数据
    }
    else if(dirdata->cal_ch0 != BIGTOLIT_U16(*pdata))//正在校准值和新期望校准值不一样
    {
        return 0x01;
    }
    pdata++;
    
    if(!(dirdata->cal_status & 0x00F0))//MX1U、BAT1U 新校准可进行
    {
        if((BIGTOLIT_U16(*pdata) != CALIBRATEDATA_FINISH))//是否为结束校准数据
        {
            if((BIGTOLIT_U16(*pdata) >= (U_CALIBRATE_LOWVAL/100000) && BIGTOLIT_U16(*pdata) <= (U_CALIBRATE_HIGVAL/100000)) && (BIGTOLIT_U16(*pdata) % (U_CALIBRATE_SEGVAL/100000) == 0x00))//校准值是否符合要求
            {
                if((BIGTOLIT_U16(*pdata) /(U_CALIBRATE_SEGVAL/100000)-(U_CALIBRATE_LOWVAL / U_CALIBRATE_SEGVAL)) < dirdata->Cal_UIchmin[0])
                {
                    dirdata->Cal_UIchmin[0] = (uint8_t)(BIGTOLIT_U16(*pdata) / (U_CALIBRATE_SEGVAL/100000)-(U_CALIBRATE_LOWVAL / U_CALIBRATE_SEGVAL));
                }
                dirdata->cal_status |= 0x0050;//启动校准
                cal_flag |= 0x01;//置位需写入Flash标志
            }
            else
            {
                return 0x01;
            }                   
        }
        dirdata->cal_ch1 = BIGTOLIT_U16(*pdata);  //赋值校准数据，包括结束校准数据
    }
    else if(dirdata->cal_ch1 != BIGTOLIT_U16(*pdata))//正在校准值和新期望校准值不一样
    {
        return 0x01;
    }
    pdata++;
    
    if(!(dirdata->cal_status & 0x0F00))//BAT0I、BAT1I 新校准可进行
    {
        if((BIGTOLIT_U16(*pdata) != CALIBRATEDATA_FINISH))//是否为结束校准数据
        {
            if((BIGTOLIT_U16(*pdata) >= (I_CALIBRATE_LOWVAL/100000) && BIGTOLIT_U16(*pdata) <= (dirdata->Cal_IsegVal * CALIBRATEDATA_IADPIONTNUM /100000)) && (BIGTOLIT_U16(*pdata) % (dirdata->Cal_IsegVal /100000) == 0x00))//校准值是否符合要求
            {
                if((BIGTOLIT_U16(*pdata) /(I_CALIBRATE_SEGVAL/100000)-(I_CALIBRATE_LOWVAL / I_CALIBRATE_SEGVAL)) < dirdata->Cal_UIchmin[1])
                {
                    dirdata->Cal_UIchmin[1] = (uint8_t)(BIGTOLIT_U16(*pdata)  / (I_CALIBRATE_SEGVAL/100000)-(I_CALIBRATE_LOWVAL / I_CALIBRATE_SEGVAL));
                }
                dirdata->cal_status |= 0x0500;//启动校准
                cal_flag |= 0x01;//置位需写入Flash标志
            }
            else 
            {
                return 0x01;
            }               
        }
        dirdata->cal_ch2 = BIGTOLIT_U16(*pdata);//赋值校准数据，包括结束校准数据
    }
    else if(dirdata->cal_ch2 != BIGTOLIT_U16(*pdata))//正在校准值和新期望校准值不一样
    {
        return 0x01;
    }
    
    if(!(uint16_t)(dirdata->cal_status | dirdata->cal_ch0 | dirdata->cal_ch1 |dirdata->cal_ch2) && (cal_flag == 0x01))//捕获校准结束命令
    {
        if(Flash_Pagedata_Write(caldataarr, dirdata->Cal_UIchmin, (uint16_t* )FLASHADDR_CALSTART, CALIBRATEDATA_COLNUM*CALIBRATEDATA_SEGNUM, 2))//校准值写入flash失败
        {
            return 0x01;//返回错误，准备下次收到校准结束命令时再写入
        }
        else
        {
            cal_flag &= 0xFE;//清需写入Flash标志
        }
    }
    return 0x00;
}

uint32_t Cal_Val_Get(uint16_t ADdata, uint8_t index, uint32_t segval, uint8_t min)
{
    uint8_t temp;
    uint8_t max;
    uint32_t k_temp, b_temp, lowval,calval;
    
   /* switch(index)//当前采样数据类型转换到相应校准k、b种类
    {
        case SAM_STRUCT_MX0U: index = MX0U_CALIBRATE_INDEX; break;
        case SAM_STRUCT_MX1U: index = MX1U_CALIBRATE_INDEX; break;
        case SAM_STRUCT_BAT0U: 
        case SAM_STRUCT_JYBAT0N: index = BAT0U_CALIBRATE_INDEX; break;
        case SAM_STRUCT_BAT1U: 
        case SAM_STRUCT_JYBAT1N: index = BAT1U_CALIBRATE_INDEX; break;
        case SAM_STRUCT_BAT0I: index = BAT0I_CALIBRATE_INDEX; break;
        case SAM_STRUCT_BAT1I: index = BAT1I_CALIBRATE_INDEX; break;
        
        default:
        return 0x00000000;
    }*/

    if((index == BAT0I_CALIBRATE_INDEX) || (index == BAT1I_CALIBRATE_INDEX))//为电流校准
    {
        lowval = I_CALIBRATE_LOWVAL;
        calval = (uint32_t)min * I_CALIBRATE_SEGVAL + lowval;
        max = CALIBRATEDATA_IADPIONTNUM;
    }
    else//为电压校准
    {
        lowval = U_CALIBRATE_LOWVAL;
        calval = (uint32_t)min * U_CALIBRATE_SEGVAL + lowval;
        max = CALIBRATEDATA_UADPIONTNUM -1;
    }
    
    if(DATA_SAMPLE_ADPOINT[index][min] >= ADdata)//AD值小于最小校准点AD值
    {
        k_temp = (uint32_t)(DATA_SAMPLE_ADPOINT[index][min]);
        k_temp = calval / k_temp + ((calval % k_temp) /(k_temp / 2));
        return (uint32_t)(ADdata * k_temp);
        //return ((uint32_t)(ADdata-2048)*DATA_SAMPLE_CAL[index][0]);
    }
    
    temp = (max + min)/2;
    do
    {
        if(DATA_SAMPLE_ADPOINT[index][temp] > ADdata)
        {
            max = temp;
        }
        else if(DATA_SAMPLE_ADPOINT[index][temp] < ADdata)
        {
            min = temp;
        }
        else
        {
            max = temp;
            break;
        }
        temp = (max+min) /2;
    }while(temp != min);
    k_temp = (uint32_t)(DATA_SAMPLE_ADPOINT[index][max]-DATA_SAMPLE_ADPOINT[index][max-1]);
    k_temp = segval/k_temp + ((segval % k_temp) /(k_temp / 2));//计算k值
    b_temp = segval*max + lowval;
    b_temp -= (k_temp * (uint32_t)(DATA_SAMPLE_ADPOINT[index][max]));//计算b值
    return(uint32_t)(ADdata * k_temp + b_temp);
    //max *= 2;
    //return ((uint32_t)(ADdata-2048)*DATA_SAMPLE_CAL[index][max]+DATA_SAMPLE_CAL[index][max+1]);
}

void Cal_ADdata_Copy(uint8_t ADcalcol, SAMP_Stream * streamdata)
{
    uint8_t ADcalrow;
    if(((streamdata->cal_status >> (ADcalcol << 1) ) & 0x0003) == 0x0001)//已启动校准校准，进入准备校准状态
    {
        streamdata->cal_status &= ~(0x0003 << (ADcalcol << 1));
        streamdata->cal_status |= (0x0002 << (ADcalcol << 1));
        return;
    }
    else if(((streamdata->cal_status >> (ADcalcol << 1) ) & 0x0003) == 0x0002)//准备校准状态已结束，进入校准数值拷贝
    {
        switch(streamdata->now)//当前采样数据类型转换到相应校准ch种类
        {
            case SAM_STRUCT_MX0U: 
            case SAM_STRUCT_BAT0U: 
            ADcalrow = (streamdata->cal_ch0 - (U_CALIBRATE_LOWVAL / 100000)) / (streamdata->Cal_UsegVal / 100000); 
            break;
            case SAM_STRUCT_MX1U: 
            case SAM_STRUCT_BAT1U: 
            ADcalrow = (streamdata->cal_ch1 - (U_CALIBRATE_LOWVAL / 100000)) / (streamdata->Cal_UsegVal / 100000); 
            break;
            case SAM_STRUCT_BAT0I: 
            case SAM_STRUCT_BAT1I: 
            ADcalrow = (streamdata->cal_ch2 - (I_CALIBRATE_LOWVAL / 100000)) / (streamdata->Cal_IsegVal / 100000); 
            break;       
            default:
            return;
        }
        DATA_SAMPLE_ADPOINT[ADcalcol][ADcalrow] = Samp_DataArr[streamdata->now].ADdata - 20480;
        streamdata->cal_status &= ~(0x0003 << (ADcalcol << 1));
    }
}

uint8_t Cal_ADscream_Change(SAMP_Stream * streamdata)
{
    switch(streamdata->now)
    {
        case SAM_STRUCT_MX0U: 
        streamdata->ready = SAM_STRUCT_BAT0U;
        break;
        case SAM_STRUCT_BAT0U: 
        streamdata->ready = SAM_STRUCT_MX1U;
        break;
        case SAM_STRUCT_MX1U: 
        streamdata->ready = SAM_STRUCT_BAT1U;
        break;
        case SAM_STRUCT_BAT1U:
        streamdata->ready = SAM_STRUCT_BAT0I;
        break;
        case SAM_STRUCT_BAT0I: 
        streamdata->ready = SAM_STRUCT_BAT1I;
        break;
        case SAM_STRUCT_BAT1I:
        streamdata->ready = SAM_STRUCT_MX0U;
        break;
        default://未匹配校准采样流
        return 0x01;
    }
    return 0x00;
}





