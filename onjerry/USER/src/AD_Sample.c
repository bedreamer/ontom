#include "AD_Sample.h"
#include "ADS795x.h"
#include "Cal_Sampdata.h"
#include "Timer.h"
#include "UART.h"
SAMP_Opx Samp_DataArr[SAM_DATA_NUM]=
{{SAM_CH0,SAM_MX0UDATA_AIN,40960,SAM_MX0UDATA_DEFHIGH,SAM_MX0UDATA_DEFLOW, MX0U_CALIBRATE_INDEX, MXU_SampData_fun},
{SAM_CH0,SAM_MX1UDATA_AIN,40960,SAM_MX1UDATA_DEFHIGH,SAM_MX1UDATA_DEFLOW, MX1U_CALIBRATE_INDEX, MXU_SampData_fun},
{SAM_CH0,SAM_FZDYUDATA_AIN,40960,SAM_FZDYUDATA_DEFHIGH,SAM_FZDYUDATA_DEFLOW, 0xFF, FZDYU_SampData_fun},
{SAM_CH0,SAM_BAT0IDATA_AIN,40970,SAM_BAT0IDATA_DEFHIGH,SAM_BAT0IDATA_DEFLOW, BAT0I_CALIBRATE_INDEX,BATI_SampData_fun},
{SAM_CH0,SAM_HUMIDATA_AIN,0x0000,SAM_HUMIDATA_DEFHIGH,SAM_HUMIDATA_DEFLOW, 0xFF, HUMI_SampData_fun},
{SAM_CH0,SAM_TEMPDATA_AIN,0x0000,SAM_TEMPDATA_DEFHIGH,SAM_TEMPDATA_DEFLOW, 0xFF, TEMP_SampData_fun},

{SAM_CH0,SAM_BAT1IDATA_AIN,40970,SAM_BAT1IDATA_DEFHIGH,SAM_BAT1IDATA_DEFLOW, BAT1I_CALIBRATE_INDEX, BATI_SampData_fun},
{SAM_CH1,SAM_BAT0UDATA_AIN,40960,SAM_BAT0UDATA_DEFHIGH,SAM_BAT0UDATA_DEFLOW, BAT0U_CALIBRATE_INDEX, BATU_SampData_fun},
{SAM_CH1,SAM_BAT1UDATA_AIN,40960,SAM_BAT1UDATA_DEFHIGH,SAM_BAT1UDATA_DEFLOW, BAT1U_CALIBRATE_INDEX, BATU_SampData_fun},
{SAM_CH1,SAM_LINK1UDATA_AIN,40960,SAM_LINK1UDATA_DEFHIGH,SAM_LINK1UDATA_DEFLOW, 0xFF, LINKU_SampData_fun},

{SAM_CH2,SAM_JYBAT0PDATA_AIN,30720,SAM_JYBAT0PDATA_DEFHIGH,SAM_JYBAT0PDATA_DEFLOW, 0xFF, JYBATP_SampData_fun},
{SAM_CH2,SAM_JYBAT1PDATA_AIN,30720,SAM_JYBAT1PDATA_DEFHIGH,SAM_JYBAT1PDATA_DEFLOW, 0xFF, JYBATP_SampData_fun},
{SAM_CH2,SAM_LINK0UDATA_AIN,40960,SAM_LINK0UDATA_DEFHIGH,SAM_LINK0UDATA_DEFLOW, 0xFF, LINKU_SampData_fun},

{SAM_CH3,SAM_JYBAT0NDATA_AIN,15360,SAM_JYBAT0NDATA_DEFHIGH,SAM_JYBAT0NDATA_DEFLOW, BAT0U_CALIBRATE_INDEX, JYRES_SampData_fun},
{SAM_CH3,SAM_JYBAT1NDATA_AIN,15360,SAM_JYBAT1NDATA_DEFHIGH,SAM_JYBAT1NDATA_DEFLOW, BAT1U_CALIBRATE_INDEX, JYRES_SampData_fun},


};//采样数据和数据处理函数结构体

SAMP_Stream Samp_ADdata_Stream={0x01,0x00,0x0000,0x0001, U_CALIBRATE_SEGVAL , I_CALIBRATE_SEGVAL, CALIBRATEDATA_UADPIONTNUM - 1, CALIBRATEDATA_IADPIONTNUM};

Un_ADSamp_Stat Samp_DataStat = {0,0,0,0,0,0,0xFFFF,0xFFFF,0xFFFF,0xFFFF,240,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0};

uint16_t HUMI_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata)
{
    return 0x0000;
}

uint16_t TEMP_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata)
{
    return 0x0000;
}

uint16_t BATI_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata)
{
    uint32_t dattemp,dathigh;
    uint8_t status=0x00, nowsam = streamdata->now;
    dattemp = (uint32_t)(Samp_DataArr[nowsam].ADdata);
    dathigh = (uint32_t)(Samp_DataArr[nowsam].highdata)* 100000;
    if(dattemp > 20480)
    {
        dattemp -= 20480;
    }
    else
    {
        dattemp = 20480 - dattemp;
        status |= 0x08;//电流反向
    }

    dattemp = Cal_Val_Get(dattemp, Samp_DataArr[nowsam].ADcalarrcol, streamdata->Cal_IsegVal, streamdata->Cal_UIchmin[1]);
    if(dattemp > dathigh)//过流
    {
        status |= 0x01;
    }

    dattemp = (dattemp / 100000)+((dattemp % 100000)/50000);//实际电流的10倍 
    
    if(status & 0x08)
    {
        dattemp |= 0x00008000;
        status &= 0xF7;
    }
    switch(nowsam)
    {
       
        case SAM_STRUCT_BAT0I:
        sampdata->Flag_prtc2 &= SAMPDATABITMASK_BAT0ISTAT;
        sampdata->Flag_prtc2 |= (status<< SAMPDATABITINDEX_BAT0ISTAT);
        sampdata->IoutBAT0 = (uint16_t)(dattemp);
        break;
        case SAM_STRUCT_BAT1I: 
        sampdata->Flag_prtc2 &= SAMPDATABITMASK_BAT1ISTAT;
        sampdata->Flag_prtc2 |= (status<< SAMPDATABITINDEX_BAT1ISTAT);
        sampdata->IoutBAT1 = (uint16_t)(dattemp);
        break;
        default:
        break;
    }
   
    return 0x0000;
}

uint16_t MXU_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata)
{
    uint32_t dattemp,datlow,dathigh;
    uint8_t status=0x00, nowsam = streamdata->now;
    dattemp = (uint32_t)(Samp_DataArr[nowsam].ADdata);
    datlow =  (uint32_t)(Samp_DataArr[nowsam].lowdata)* 100000;
    dathigh =  (uint32_t)(Samp_DataArr[nowsam].highdata)* 100000;

    if(dattemp >= 20480)
    {
        dattemp -= 20480;
    }
    else
    {
        dattemp = 20480 - dattemp;
        status |= 0x08;//母线负电压状态
    }

    dattemp = Cal_Val_Get(dattemp, Samp_DataArr[nowsam].ADcalarrcol, streamdata->Cal_UsegVal, streamdata->Cal_UIchmin[0]);
    
    if(dattemp>dathigh)//过压
    {
        status |= 0x01;
    }
    else if(dattemp<datlow)//欠压
    {
        status |= 0x02;
    }

    dattemp = (dattemp / 100000)+((dattemp % 100000)/50000);//实际电压的10倍    
    if(status & 0x08)
    {
        dattemp |= 0x00008000;
        status &= 0xF7;//母线不会有反接故障，清除负电压标志
    }
    switch(nowsam)
    {
        case SAM_STRUCT_MX0U:
        sampdata->Flag_prtc1 &= SAMPDATABITMASK_MX0USTAT;
        sampdata->Flag_prtc1 |= (status << SAMPDATABITINDEX_MX0USTAT);
        sampdata->VinKM0 = (uint16_t)(dattemp);
        break;
        case SAM_STRUCT_MX1U:
        sampdata->Flag_prtc1 &= SAMPDATABITMASK_MX1USTAT;
        sampdata->Flag_prtc1 |= (status << SAMPDATABITINDEX_MX1USTAT);
        sampdata->VinKM1 = (uint16_t)(dattemp);
        break;
        default:
        break;
    }

    return 0x0000;
}

uint16_t BATU_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata)
{
    uint32_t dattemp,datlow,dathigh;
    uint8_t status=0x00, nowsam = streamdata->now;
    dattemp = (uint32_t)(Samp_DataArr[nowsam].ADdata);
    datlow =  (uint32_t)(Samp_DataArr[nowsam].lowdata)* 100000;
    dathigh =  (uint32_t)(Samp_DataArr[nowsam].highdata)* 100000;
    
    
    if(dattemp >= 20480)
    {
        dattemp -= 20480;
    }
    else
    {
        dattemp = 20480 - dattemp;
        status |= 0x10;//负电压
    }
    
    dattemp = Cal_Val_Get(dattemp, Samp_DataArr[nowsam].ADcalarrcol, streamdata->Cal_UsegVal, streamdata->Cal_UIchmin[0]);

    if((status & 0x10) && dattemp > (uint32_t)10000000)//-10V以下反接
    {
        status |= 0x08;//置反接
    }
    
    if(dattemp < (uint32_t)1000000)//1V以下，短路
    {
        status |= 0x04;
    }
    if(dattemp>dathigh)//过压
    {
        status |= 0x01;
    }
    else if(dattemp<datlow)//欠压
    {
        status |= 0x02;
    }

    dattemp = (dattemp / 100000)+((dattemp % 100000)/50000);//实际电压的10倍

    if(status & 0x10)
    {
        dattemp |= 0x00008000;
        status &= 0xEF;//清负电压状态
    }
    switch(nowsam)
    {
        case SAM_STRUCT_BAT0U:
        sampdata->Flag_prtc2 &= SAMPDATABITMASK_BAT0USTAT;
        sampdata->Flag_prtc2 |= (status << SAMPDATABITINDEX_BAT0USTAT);
        sampdata->VinBAT0= (uint16_t)(dattemp);
        break;
        case SAM_STRUCT_BAT1U:
        sampdata->Flag_prtc3 &= SAMPDATABITMASK_BAT1USTAT;
        sampdata->Flag_prtc3 |= (status << SAMPDATABITINDEX_BAT1USTAT);
        sampdata->VinBAT1= (uint16_t)(dattemp);
        break;
        default:
        break;
    }
    return 0x0000;
    
}


/*  R+ = (U-  U+  -U-)/U-  *Rsamp ;R- = (U-  U+  -U-)/U+  *Rsamp   ;
必须先计算出每一个成员相对参考0电位的差值，
0电位参考在此应用中为2048，即设计时AD采样范围
(电路设计中-800V - +800V  变为0V - +5V采样，2.5V表示实际电压0V) 的中间值*/
uint16_t JYBATP_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata)
{
    return 0x0000;
}

uint16_t JYBATN_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata)
{
    return 0x0000;
}

uint16_t JYRES_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata)
{
    uint32_t P_dat,N_dat,RV_temp1,RV_temp2;
    static uint8_t Errornum=0x00;
    uint8_t nowsam = streamdata->now;
    RV_temp1 = (uint32_t)Samp_DataArr[Samp_DataArr[Samp_DataArr[nowsam].highdata].highdata].ADdata;
    P_dat = (uint32_t)Samp_DataArr[Samp_DataArr[nowsam].highdata].ADdata;//正投切后正对地电压值
    N_dat = (uint32_t)Samp_DataArr[nowsam].ADdata;                                        //负投切后负对地电压值
    if(RV_temp1 >= 20600 /*||RV_temp1 <= 20360*/ )//电池电压存在的情况下(可适当改认为此电压有效的限度)，即电池连接时，绝缘有效
    {
        if((P_dat == 20480)&&(N_dat == 20480))//正负同时接地短路
        {
            P_dat = 0;
            N_dat = 0;
            RV_temp1 = 0;
            RV_temp2 = 0;
            goto Lable0_JYRES_SampData_fun;
        }
        else if((P_dat == 20480)&&(N_dat != 20480))//正和地短路
        {
            P_dat = 0;
            N_dat = 0xFFFFFFFF;
            RV_temp1 = 0;
            RV_temp2 = 0xFFFFFFFF;
            goto Lable0_JYRES_SampData_fun;
        }
        else if((N_dat == 20480)&&(P_dat!= 20480))//负和地短路
        {
            N_dat = 0;
            P_dat = 0xFFFFFFFF;
            RV_temp1 = 0xFFFFFFFF;
            RV_temp2 = 0;
            goto Lable0_JYRES_SampData_fun;
        }
        else//无短路状态，开始测绝缘
        {
            if(Errornum >= 6) //未知错误计数达到限度
            {
                P_dat = 0;
                N_dat = 0xFEEEEEEE;
                RV_temp1 = 0;
                RV_temp2 = 0xFEEEEEEE;
                goto Lable0_JYRES_SampData_fun;
            }         
            else if(((RV_temp1 > 20480)&&(P_dat  > 20480)&&(N_dat > 20480))||((RV_temp1 < 20480)&&(P_dat  < 20480)&&(N_dat < 20480)))//电压方向一致时
            {
                RV_temp1 = Cal_Val_Get((uint16_t) RV_temp1-20480, Samp_DataArr[nowsam].ADcalarrcol, streamdata->Cal_UsegVal, streamdata->Cal_UIchmin[0]);
                RV_temp2 = RV_temp1;
                P_dat = Cal_Val_Get((uint16_t) P_dat-20480, Samp_DataArr[nowsam].ADcalarrcol, streamdata->Cal_UsegVal, streamdata->Cal_UIchmin[0]);
                N_dat = Cal_Val_Get((uint16_t) N_dat-20480, Samp_DataArr[nowsam].ADcalarrcol, streamdata->Cal_UsegVal, streamdata->Cal_UIchmin[0]);//所有电压值都扩大了1000000倍
                if(RV_temp1 < (P_dat + N_dat)) //公式分子计算结果为负数无效
                {
                    Errornum++;
                    return 0x0001;//错误返回
                }
            }
            else//各电压不是同一个方向或者公式分子为负,但未到错误计数值
            {
                Errornum++;
                return 0x0001;//错误返回
            }

        }
    }
    else//电池电压不存在的情况下，即电池未连接时，绝缘无效
    {
        P_dat = 0xFFFFFFFF;
        N_dat = 0xFFFFFFFF;
        RV_temp1 = 0xFFFFFFFF;
        RV_temp2 = 0xFFFFFFFF;
        goto Lable0_JYRES_SampData_fun;
    }

     /*正负绝缘电阻计算*/
    /*U_dat -= (P_dat + N_dat);
    U_dat = U_dat / 1000 + ((U_dat % 1000) / 500);//电压值为1000倍
    P_dat = P_dat / 1000 + ((P_dat % 1000) / 500);//电压值为1000倍
    N_dat = N_dat / 1000 + ((N_dat % 1000) / 500);//电压值为1000倍
    U_dat = U_dat*Samp_DataArr[Samp_DataArr[nowsam].highdata].lowdata;//此时数据已经扩大10000倍
    P_dat = U_dat / P_dat + ((U_dat2 % P_dat) /(P_dat / 2));//此时数据已经扩大10倍,为负对地电阻值,且四舍五入
    N_dat = U_dat / N_dat + ((U_dat % N_dat) /(N_dat / 2));//此时数据已经扩大10倍,为正对地电阻值,且四舍五入
    U_dat = P_dat;
    P_dat = N_dat;
    N_dat = U_dat;//正负电阻值交换*/

    RV_temp2 -= (P_dat + N_dat);//被除数
    RV_temp2 = RV_temp2 /1000 + (RV_temp2 % 1000) /500;
    RV_temp2 *= (uint32_t)(Samp_DataArr[Samp_DataArr[nowsam].highdata].lowdata);
    N_dat = N_dat/1000 + (N_dat% 1000) /500; 
    N_dat = RV_temp2 / N_dat + ((RV_temp2 % N_dat) /(N_dat / 2));

    P_dat = P_dat/1000 + (P_dat% 1000) /500; 
    P_dat = RV_temp2 / P_dat + ((RV_temp2 % P_dat) /(P_dat / 2));

    RV_temp2 = P_dat;
    P_dat = N_dat;
    N_dat = RV_temp2;//正负电阻值交换,*10K
    
    /*RV_temp2 -= (P_dat + N_dat);//被除数
    N_dat = RV_temp2 / N_dat + ((RV_temp2 % N_dat) /(N_dat / 2));
    N_dat *= ((uint32_t)(Samp_DataArr[Samp_DataArr[nowsam].highdata].lowdata) * 1000);//此时数据已经扩大10000倍
    P_dat = RV_temp2 / P_dat  + ((RV_temp2  % P_dat) /(P_dat  / 2));
    P_dat *= ((uint32_t)(Samp_DataArr[Samp_DataArr[nowsam].highdata].lowdata) * 1000);//此时数据已经扩大10000倍
    RV_temp2 = P_dat;
    P_dat = N_dat;
    N_dat = RV_temp2;//正负电阻值交换*/

    RV_temp1 = RV_temp1 / 100000 + ((RV_temp1 % 100000) / 50000);//电压值计算
    RV_temp2 = (N_dat * 1000) / RV_temp1;//计算负对地的Ω/V
    //RV_temp2 = RV_temp2 / 10 + ((RV_temp2 % 10) / 5);//计算负对地的Ω/V
    RV_temp1 = (P_dat * 1000) / RV_temp1;//计算正对地的Ω/V
    //RV_temp1 = RV_temp1 / 10 + ((RV_temp1 % 10) / 5);//计算正对地的Ω/V

    P_dat = (P_dat > 0x0000FFFF) ? 0x0000FFFF : P_dat;
    N_dat = (N_dat > 0x0000FFFF) ? 0x0000FFFF : N_dat;

    //P_dat = P_dat / 1000 + ((P_dat% 1000) / 500);//正对地电阻值已经扩大10倍
    //N_dat = N_dat / 1000 + ((N_dat% 1000) / 500);//负对地电阻值已经扩大10倍
    Lable0_JYRES_SampData_fun:
    switch(nowsam)
    {
        case SAM_STRUCT_JYBAT0N:
        sampdata->VinBAT0RESP = (uint16_t)P_dat;
        sampdata->VinBAT0RESN = (uint16_t)N_dat;
        sampdata->Flag_prtc2 &= SAMPDATABITMASK_JYRESBAT0STAT;//清状态
         /*正负绝缘电阻与告警值比较*/
        if((RV_temp1 < (uint32_t)Samp_DataArr[SAM_STRUCT_JYBAT0N].lowdata) || (RV_temp2 < (uint32_t)Samp_DataArr[SAM_STRUCT_JYBAT0N].lowdata))
        {   
            sampdata->Flag_prtc2 |= (0x01 << SAMPDATABITINDEX_JYRESBAT0STAT);
        }
        break;
        case SAM_STRUCT_JYBAT1N:
        sampdata->VinBAT1RESP = (uint16_t)P_dat;
        sampdata->VinBAT1RESN = (uint16_t)N_dat;
        sampdata->Flag_prtc3 &= SAMPDATABITMASK_JYRESBAT1STAT;//清状态
         /*正负绝缘电阻与告警值比较*/
        if((RV_temp1 < (uint32_t)Samp_DataArr[SAM_STRUCT_JYBAT1N].lowdata) || (RV_temp2 < (uint32_t)Samp_DataArr[SAM_STRUCT_JYBAT1N].lowdata))
        {   
            sampdata->Flag_prtc3 |= (0x01 << SAMPDATABITINDEX_JYRESBAT1STAT);
        }
        break;
        default:
        return 0x0001;
    }
    Errornum = 0x00;//清未知错误计数
    return 0x0000;
}


uint16_t FZDYU_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata)
{
    uint32_t dattemp,datlow,dathigh;
    
    dattemp = (uint32_t)(Samp_DataArr[SAM_STRUCT_FZDYU].ADdata - 20480) * SAM_FZDYUDATA_RESOLUTION ;
    datlow =  (uint32_t)(Samp_DataArr[SAM_STRUCT_FZDYU].lowdata);
    dathigh =  (uint32_t)(Samp_DataArr[SAM_STRUCT_FZDYU].highdata);

    dattemp = (dattemp / 100000) + ((dattemp % 100000) /50000);//实际电压的10倍
    sampdata->Flag_prtc4 &= SAMPDATABITMASK_FZDYUSTAT;//清标志
    if((dattemp <= datlow) || (dattemp >= dathigh))
    {        
        sampdata->Flag_prtc4 |= (0x01 << SAMPDATABITINDEX_FZDYUSTAT);
    }

    return 0x0000;
}

uint16_t LINKU_SampData_fun(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata)
{
    uint32_t dattemp;
    uint8_t status=0x00, nowsam = streamdata->now;
    dattemp = (uint32_t)(Samp_DataArr[nowsam].ADdata);

    if(dattemp >= 20480)
    {
        if((dattemp >= 34130) && (dattemp <= 37270))//未连接,实际电压在12V左右(此处考虑到了运放饱和)，理想的下限应该为3641
        {
            status = 0x00;
        }
        else if((dattemp >= 28140) && (dattemp <= 29200))//连接保护状态,实际电压在6V左右
        {
            status = 0x01;
        }
        else if((dattemp >= 25410) && (dattemp <= 26470))//连接,实际电压在4V左右
        {
            status = 0x03;
        }
        else//连接异常
        {
            status = 0x02;
        }
    }
    else
    {
         status = 0x02;//连接异常
    }

    /*dattemp = (uint32_t)Samp_DataArr[SAM_STRUCT_LINK0U].ADdata;
    if(dattemp >= 0x000009D0)//未连接
    {
        status = 0x00;
    }
    else if(dattemp >= 0x00000910)//连接保护状态
    {
        status = 0x01;
    }
    else if(dattemp >= 0x000008B0)//连接
    {
        status = 0x03;
    }
    else//连接异常
    {
        status = 0x02;
    }*/
    switch(nowsam)
    {
        case SAM_STRUCT_LINK0U:
        sampdata->Flag_run1 &= SAMPDATABITMASK_LINK0USTAT;
        sampdata->Flag_run1 |= (status << SAMPDATABITINDEX_LINK0USTAT);
        break;
        case SAM_STRUCT_LINK1U:
        sampdata->Flag_run2 &= SAMPDATABITMASK_LINK1USTAT;
        sampdata->Flag_run2 |= (status << SAMPDATABITINDEX_LINK1USTAT);
        break;
        default:
        return 0x0001;
    }
    return 0x0000;
}

static __ASM uint32_t get_sampmemb(uint32_t membdata)
{
    LDR R1, =SAM_DATA_NUM
    LDR R3, =0x80000000
    SUB R2, R1, #1
    ASR R3, R3, R2
    LDR R2, =32
    SUB R2, R2, R1
    LSR R3, R3, R2

    MVN R0, R0
    AND R0, R0, R3
    BX LR
}

uint8_t ADSampe_LimitDataPro(uint16_t* pdata, SAMP_Stream *streamdata)//AD数据通讯设置
{
    SAMP_Opx *dirdata = Samp_DataArr;
    dirdata[SAM_STRUCT_MX0U].highdata=  BIGTOLIT_U16(*pdata);
    pdata++;
    dirdata[SAM_STRUCT_MX0U].lowdata= BIGTOLIT_U16(*pdata);
    pdata++;
    dirdata[SAM_STRUCT_MX1U].highdata = BIGTOLIT_U16(*pdata);
    pdata++;
    dirdata[SAM_STRUCT_MX1U].lowdata = BIGTOLIT_U16(*pdata);
    pdata++;
    dirdata[SAM_STRUCT_BAT0U].highdata = BIGTOLIT_U16(*pdata);
    pdata++;
    dirdata[SAM_STRUCT_BAT0U].lowdata = BIGTOLIT_U16(*pdata);
    pdata++;
    dirdata[SAM_STRUCT_BAT1U].highdata = BIGTOLIT_U16(*pdata);
    pdata++;
    dirdata[SAM_STRUCT_BAT1U].lowdata = BIGTOLIT_U16(*pdata);
    pdata++;
    dirdata[SAM_STRUCT_BAT0I].highdata = BIGTOLIT_U16(*pdata);
    pdata++;
    dirdata[SAM_STRUCT_BAT1I].highdata = BIGTOLIT_U16(*pdata);
    pdata++;
    dirdata[SAM_STRUCT_JYBAT0N].lowdata = BIGTOLIT_U16(*pdata);
    pdata++;
    dirdata[SAM_STRUCT_JYBAT1N].lowdata = BIGTOLIT_U16(*pdata);
    pdata++;
    streamdata->Cal_IsegVal = BIGTOLIT_U16(*pdata) * 1000000/CALIBRATEDATA_IADPIONTNUM;
    return 0x00;
}


void ADSample_Scream_Col(Stu_ADSample_DATAStat *sampdata, SAMP_Stream* sam_status)
{
    uint32_t allsampmemb = (uint32_t)0x01 << SAM_STRUCT_FZDYU;//不采辅助电源
    static uint8_t lastsamp;
    if((uint8_t)(sampdata->Flag_run1 | SAMPDATABITMASK_LINK0USTAT) != 0xFF)//LINK0未连接
    {
        sampdata->VinKM0 = 0x0000;
        sampdata->VinBAT0 = 0x0000;
        sampdata->IoutBAT0 = 0x0000;
        sampdata->VinBAT0RESP= 0xFFFF;
        sampdata->VinBAT0RESN = 0xFFFF;
        sampdata->Flag_prtc1 &= SAMPDATABITMASK_MX0USTAT;
        sampdata->Flag_prtc2 &=  (SAMPDATABITMASK_BAT0USTAT & SAMPDATABITMASK_BAT0ISTAT & SAMPDATABITMASK_JYRESBAT0STAT ); 
        allsampmemb |= (((uint32_t)0x01 << SAM_STRUCT_BAT0I) | //获取LINK0未连接时不需要采样的成员
        ((uint32_t)0x01 << SAM_STRUCT_BAT0U) | 
        ((uint32_t)0x01 << SAM_STRUCT_MX0U) | 
        ((uint32_t)0x01 << SAM_STRUCT_JYBAT0P) | 
        ((uint32_t)0x01 << SAM_STRUCT_JYBAT0N));
        
    }
    if((uint8_t)(sampdata->Flag_run2 | SAMPDATABITMASK_LINK1USTAT) != 0xFF)//LINK1未连接
    {
        sampdata->VinKM1 = 0x0000;
        sampdata->VinBAT1 = 0x0000;
        sampdata->IoutBAT1 = 0x0000;
        sampdata->VinBAT1RESP= 0xFFFF;
        sampdata->VinBAT1RESN = 0xFFFF;
        sampdata->Flag_prtc1 &= SAMPDATABITMASK_MX1USTAT;
        sampdata->Flag_prtc3 &=  (SAMPDATABITMASK_BAT1USTAT & SAMPDATABITMASK_BAT1ISTAT & SAMPDATABITMASK_JYRESBAT1STAT );
        allsampmemb |= (((uint32_t)0x01 << SAM_STRUCT_BAT1I) | //获取LINK1未连接时不需要采样的成员
        ((uint32_t)0x01 << SAM_STRUCT_BAT1U) | 
        ((uint32_t)0x01 << SAM_STRUCT_MX1U) | 
        ((uint32_t)0x01 << SAM_STRUCT_JYBAT1P) | 
        ((uint32_t)0x01 << SAM_STRUCT_JYBAT1N));
    }
    
    
    if(allsampmemb == 0)//LINK0和LINK1都连接，没有不需要采样的成员
    {
        lastsamp = 0x00;//清除上次LINK0和LINK1未全部连接情况下的采样流
        return;
    }

    allsampmemb = get_sampmemb(allsampmemb);//得到当前需要采样的成员

    
    if(!(allsampmemb >> (lastsamp + 1)))//采样流转换后无任何需要采样成员
    {
        sam_status->ready = 0x00;//回到起始采样成员
        lastsamp = 0x00;
    }
    else
    {
        allsampmemb >>= ++lastsamp;//采样流加1转换，匹配下一个采样成员
        for(; !(allsampmemb & (uint32_t)0x01); lastsamp++)
        {
            allsampmemb >>= 1;
        }   
        sam_status->ready = lastsamp;
    }

}


uint16_t ADSample_Change(SAMP_Stream *sam_status)
{
    uint16_t retnum=0x0000;

    if(Samp_DataArr[sam_status->ready].Ain != Samp_DataArr[sam_status->now].Ain)
    {
        if(ADS7953_CHsel(Samp_DataArr[sam_status->ready].Ain) != 0x0000)//尝试切换AD通道,主要为了判断SPI1中断未运行
        {
            return 0x0001;//理想的通道切换失败，可能是SPI1中断还在处理中，或者通道已经切换，但主机接收错误
        }
            switch (sam_status->ready)
            {
                case SAM_STRUCT_JYBAT0P:
                case SAM_STRUCT_JYBAT1P:
                SAM_JYPSET;
                SAM_JYNRSET;
                break;
                case SAM_STRUCT_JYBAT0N:
                case SAM_STRUCT_JYBAT1N:
                SAM_JYNSET;
                SAM_JYPRSET;  
                break;
                default:
                SAM_JYPRSET;
                SAM_JYNRSET;
                break;
            }
        ADSample_AnalogCHsel(Samp_DataArr[sam_status->ready].Chan);//模拟采样输入通道选择
        retnum = ADS7953_CHflush(0x02);//尝试冲刷1次当前通道
        
    }
    else
    {
        if(ADS7953_CHflush(0x01) != 0x0000)//尝试冲刷当前AD通道,主要为了判断SPI1中断未运行
        {
            return 0x0001;//理想的通道切换失败，可能是SPI1中断还在处理中，或者通道已经切换，但主机接收错误
        }

           switch (sam_status->ready)
            {
                case SAM_STRUCT_JYBAT0P:
                case SAM_STRUCT_JYBAT1P:
                SAM_JYPSET;
                SAM_JYNRSET;
                break;
                case SAM_STRUCT_JYBAT0N:
                case SAM_STRUCT_JYBAT1N:
                SAM_JYNSET;
                SAM_JYPRSET;  
                break;
                default:
                SAM_JYPRSET;
                SAM_JYNRSET;
                break;
            }
        ADSample_AnalogCHsel(Samp_DataArr[sam_status->ready].Chan);//模拟采样输入通道选择
        retnum = ADS7953_CHflush(0x01);//尝试冲刷1次当前通道
    }

    if(retnum == 0x0000)//通道处理完成，且确保了下一次采样为有效数据
    {
       // retnum = sam_status->now;
        sam_status->now = sam_status->ready;
        //sam_status->ready = retnum;
        //ADS7953_SMstart();//开始采样
        ADSMstartcont = SAM_CHANNEL_TIMERCOUNT+5;//采样计数次数

        TIMER1_UPDATEIDISEN;//加锁修改
        Systimememb |= 0x01;//开启定时采样
        TIMER1_UPDATEIEN;
        
        if(sam_status->ready >= (SAM_DATA_NUM-1))
        {
            sam_status->ready = 0x0000;
        }
        else
        {
            sam_status->ready++;
            //sam_status->ready = (sam_status->ready == SAM_STRUCT_LINK0U)?SAM_STRUCT_LINK1U : SAM_STRUCT_LINK0U;
        }
    }
    else//由于任意错误(通道可能也已经冲刷完成，只是最后一次冲刷主机接收错误)
    {
        return 0x0001;//退出函数，准备下一次切换尝试
    }
    return 0x0000;
}


uint16_t ADSample_AnalogCHsel(uint32_t chan)
{
    static uint32_t nowchan = 0xFFFFFFFF;
    uint32_t temp;
    if(chan == nowchan)//当前模拟通道为需要切换的通道时不做切换
    {
        return 0x0001;
    }
    SAM_SEL(chan);
    for(temp = 0x0003FFFC;temp!= 0x00000000;temp--);//模拟通道切换延时循环
    nowchan = chan;
    return 0x0000;
}



void ADSample_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;//使能GPIOC时钟
    SAM_JYPRSET;//关闭正绝缘检测电阻
    SAM_JYNRSET;//关闭负绝缘检测电阻
    GPIOC->CRL &= ~(GPIO_CRL_MODE & 0xFF000000);
    GPIOC->CRL |= (GPIO_CRL_MODE6_1|GPIO_CRL_MODE7_1);//配置控制两个绝缘检测输入选择线为输出模式2MHz
    GPIOC->CRL &=~(GPIO_CRL_CNF & 0xFF000000);//配置控制两个绝缘检测输入线为推挽输出模式 
    //GPIOC->CRL |= (GPIO_CRL_CNF6_0|GPIO_CRL_CNF7_0);//配置控制两个绝缘检测输入选择线为开漏输出模式
    
    //SAM_SEL(SAM_CH3);
    GPIOC->CRH &= ~(GPIO_CRH_MODE & 0x000000FF);
    GPIOC->CRH |= (GPIO_CRL_MODE0_1|GPIO_CRL_MODE1_1);//配置控制两个采样输入选择线为输出模式2MHz
    GPIOC->CRH &=~(GPIO_CRH_CNF & 0x000000FF);//配置控制两个采样输入选择线为推挽输出模式 
    ADSample_AnalogCHsel(Samp_DataArr[Samp_ADdata_Stream.now].Chan);   
    ADS7953_Init();
}



