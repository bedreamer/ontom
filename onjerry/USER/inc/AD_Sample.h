#ifndef _ADSAMPLE_H_
#define _ADSAMPLE_H_


#include "stm32f10x.h"
#include "MyTypedef.h"



//���ò����ڣ���ӦADͨ��CH4��CH5

#define SAM_CH0 (0x0300)//������BA 00,ѡ��ĸ�ߵ�ѹ��GND to GND��������Դ��ѹ
#define SAM_CH1 (0x0100)//������BA 01,ѡ�е��0��ѹ�����1��ѹ��LINK1
#define SAM_CH2 (0x0200)//������BA 10,ѡ�о�Ե���0���Եء���Ե���1���Եء�LINK0
#define SAM_CH3 (0x0000)//������BA 11,ѡ�о�Ե���0���Եء���Ե���1���Եء�GND to GND
#define SAM_SEL(sel) (GPIOC->ODR = (GPIOC->ODR & (~(uint32_t)(GPIO_ODR_ODR8 | GPIO_ODR_ODR9)))|(uint32_t)sel)

//��Ե���
#define SAM_JYPSET   (GPIOC->BSRR = GPIO_BSRR_BS7)
#define SAM_JYPRSET (GPIOC->BSRR = GPIO_BSRR_BR7)
#define SAM_JYNSET   (GPIOC->BSRR = GPIO_BSRR_BS6)
#define SAM_JYNRSET (GPIOC->BSRR = GPIO_BSRR_BR6)


#define SAM_DATA_NUM 15 //���������������
#define SAM_EVERYDATA_COUNT 5//ÿ����������AD��������

#define SAM_STRUCT_HUMI     0x0004 //ʪ��������Samp_DataArrƫ��
#define SAM_HUMIDATA_AIN   0x0004//��ӦAD����ͨ��
#define SAM_HUMIDATA_DEFHIGH (uint16_t)4096
#define SAM_HUMIDATA_DEFLOW  (uint16_t)0
#define SAM_HUMIDATA_RESOLUTION 1 //ADһλ��Ӧʵ��ʪ�ȵı�


#define SAM_STRUCT_TEMP     0x0005 //�¶�������Samp_DataArrƫ��
#define SAM_TEMPDATA_AIN   0x0005//��ӦAD����ͨ��
#define SAM_TEMPDATA_DEFHIGH (uint16_t)4096
#define SAM_TEMPDATA_DEFLOW  (uint16_t)0
#define SAM_TEMPDATA_RESOLUTION  1//ADһλ��Ӧʵ���¶ȵı�



#define SAM_STRUCT_BAT0I      0x0003 //���0�����������Samp_DataArrƫ��
#define SAM_BAT0IDATA_AIN   0x0003//���0�������ӦAD����ͨ��
#define SAM_BAT0IDATA_DEFHIGH   (uint16_t)1250
#define SAM_BAT0IDATA_DEFLOW    (uint16_t)0
#define SAM_BAT0IDATA_RESOLUTION  //���0�����ADһλ��Ӧʵ�ʵ����ı�

#define SAM_STRUCT_BAT1I       0x0006//���1�����������Samp_DataArrƫ��
#define SAM_BAT1IDATA_AIN   SAM_BAT0IDATA_AIN//���1���ӦAD����ͨ��
#define SAM_BAT1IDATA_DEFHIGH   SAM_BAT0IDATA_DEFHIGH
#define SAM_BAT1IDATA_DEFLOW    SAM_BAT0IDATA_DEFLOW
#define SAM_BAT1IDATA_RESOLUTION  //���1�����ADһλ��Ӧʵ�ʵ����ı�
#define SAM_BATIDATA_RESOLUTION 414299//��ص����ĵ�ѹ�ź�ADһλ��Ӧʵ�ʵ�����10000000��mV(���е��������Ĳ�����·��һ�������)
#define SAM_BATIDATA_SENSORSOLUTION 2//��������ֵA/mV


#define SAM_STRUCT_MX0U       0x0000 //ĸ��0��ѹ������Samp_DataArrƫ��
#define SAM_MX0UDATA_AIN   0x0000//ĸ��0��ѹ��ӦAD����ͨ��
#define SAM_MX0UDATA_DEFHIGH  (uint16_t)7600
#define SAM_MX0UDATA_DEFLOW   (uint16_t)2000
#define SAM_MX0UDATA_RESOLUTION //ĸ��0��ѹADһλ��Ӧʵ��ĸ�ߵ�ѹ��1000000��

#define SAM_STRUCT_MX1U       0x0001 //ĸ��1��ѹ������Samp_DataArrƫ��
#define SAM_MX1UDATA_AIN   SAM_MX0UDATA_AIN //ĸ��1��ѹ��ӦAD����ͨ��
#define SAM_MX1UDATA_DEFHIGH  SAM_MX0UDATA_DEFHIGH
#define SAM_MX1UDATA_DEFLOW   SAM_MX0UDATA_DEFLOW
#define SAM_MX1UDATA_RESOLUTION //ĸ��1��ѹADһλ��Ӧʵ��ĸ�ߵ�ѹ��1000000��
#define SAM_MXUDATA_RESOLUTION 397713//ĸ�ߵ�ѹADһλ��Ӧʵ�ʵ�ѹ�ı�(����ĸ�ߵ�ѹ�Ĳ�����·��һ�������)



#define SAM_STRUCT_BAT0U     0x0007 //���0���ѹ������Samp_DataArrƫ��
#define SAM_BAT0UDATA_AIN   0x0000//���0���ѹ��ӦAD����ͨ��
#define SAM_BAT0UDATA_DEFHIGH (uint16_t)7600
#define SAM_BAT0UDATA_DEFLOW  (uint16_t)2000
#define SAM_BAT0UDATA_RESOLUTION //���0���ѹADһλ��Ӧʵ�ʵ�ص�ѹ��1000000��

#define SAM_STRUCT_BAT1U     0x0008 //���1���ѹ������Samp_DataArrƫ��
#define SAM_BAT1UDATA_AIN   0x0001//���1���ӦAD����ͨ��
#define SAM_BAT1UDATA_DEFHIGH (uint16_t)7600
#define SAM_BAT1UDATA_DEFLOW  (uint16_t)2000
#define SAM_BAT1UDATA_RESOLUTION //���1��ADһλ��Ӧʵ�ʵ�ص�ѹ��1000000��
#define SAM_BATUDATA_RESOLUTION 397713//��ص�ѹADһλ��Ӧʵ�ʵ�ص�ѹ��1000000��(���е�ص�ѹ�Ĳ�����·��һ�������)




#define SAM_JYBATRESDATA_DEFSAMPLERES 2040//������Եƽ����Ͷ�е�������10��K(���е�����Եƽ����Ͷ�в���������һ�������)
#define SAM_STRUCT_JYBAT0RES     //���0���Ե�ӵص���������Samp_DataArrƫ��
#define SAM_JYBAT0RESDATA_AIN   //��ӦAD����ͨ��
#define SAM_JYBAT0RESDATA_DEFHIGH  
#define SAM_JYBAT0RESDATA_DEFLOW  (uint16_t)100//��Ե�ӵ�������С100��/V
#define SAM_JYBAT0RESDATA_RESOLUTION
#define SAM_JYBAT0RESDATA_DEFSAMPLERES SAM_JYBATRESDATA_DEFSAMPLERES//���0���Եƽ����Ͷ�в�������204K

#define SAM_STRUCT_JYBAT1RES     //��Ե�ӵص���������Samp_DataArrƫ��
#define SAM_JYBAT1RESDATA_AIN   //��ӦAD����ͨ��
#define SAM_JYBAT1RESDATA_DEFHIGH  
#define SAM_JYBAT1RESDATA_DEFLOW  (uint16_t)100//��Ե�ӵ�������С100��/V
#define SAM_JYBAT1RESDATA_RESOLUTION
#define SAM_JYBAT1RESDATA_DEFSAMPLERES SAM_JYBATRESDATA_DEFSAMPLERES//���1���Եƽ����Ͷ�в�������204K

#define SAM_STRUCT_JYBAT0P  0x000A //���0���Ե���Ե�������Samp_DataArrƫ��
#define SAM_JYBAT0PDATA_AIN   0x0000//���0���Ե���Եض�ӦAD����ͨ��
#define SAM_JYBAT0PDATA_DEFHIGH  SAM_STRUCT_BAT0U//���0���ѹ������Samp_DataArrƫ�ƣ��������ޱ�����
#define SAM_JYBAT0PDATA_DEFLOW   (uint16_t)SAM_JYBAT0RESDATA_DEFSAMPLERES//���0���Եƽ����Ͷ�в�������K��100�� ���������ޱ�����
#define SAM_JYBAT0PDATA_RESOLUTION

#define SAM_STRUCT_JYBAT1P  0x000B //���1���Ե���Ե�������Samp_DataArrƫ��
#define SAM_JYBAT1PDATA_AIN   0x0001//���1���Ե���Եض�ӦAD����ͨ��
#define SAM_JYBAT1PDATA_DEFHIGH  SAM_STRUCT_BAT1U//���1���ѹ������Samp_DataArrƫ��
#define SAM_JYBAT1PDATA_DEFLOW   (uint16_t)SAM_JYBAT1RESDATA_DEFSAMPLERES//���1���Եƽ����Ͷ�в�������K��100�� ���������ޱ�����
#define SAM_JYBAT1PDATA_RESOLUTION
#define SAM_JYBATPDATA_RESOLUTION //��ؾ�Ե���Ե�ADһλ��Ӧʵ�ʵ�ص�ѹ�ı�(���е�ؾ�Ե���ԵصĲ�����·��һ�������)



#define SAM_STRUCT_JYBAT0N  0x000D //���0���Ե���Ե�������Samp_DataArrƫ��
#define SAM_JYBAT0NDATA_AIN   0x0000//��ӦAD����ͨ��
#define SAM_JYBAT0NDATA_DEFHIGH  SAM_STRUCT_JYBAT0P//���0���Ե���Եص�ѹ������Samp_DataArrƫ��
#define SAM_JYBAT0NDATA_DEFLOW   SAM_JYBAT0RESDATA_DEFLOW
#define SAM_JYBAT0NDATA_RESOLUTION

#define SAM_STRUCT_JYBAT1N  0x000E //���1���Ե���Ե�������Samp_DataArrƫ��
#define SAM_JYBAT1NDATA_AIN   0x0001//��ӦAD����ͨ��
#define SAM_JYBAT1NDATA_DEFHIGH  SAM_STRUCT_JYBAT1P//���1���Ե���Եص�ѹ������Samp_DataArrƫ��
#define SAM_JYBAT1NDATA_DEFLOW   SAM_JYBAT1RESDATA_DEFLOW
#define SAM_JYBAT1NDATA_RESOLUTION
#define SAM_JYBATNDATA_RESOLUTION //��ؾ�Ե���Ե�ADһλ��Ӧʵ�ʵ�ص�ѹ�ı�(���е�ؾ�Ե���ԵصĲ�����·��һ�������)







#define SAM_STRUCT_FZDYU    0x0002 //������Դ��ѹ������Samp_DataArrƫ��
#define SAM_FZDYUDATA_AIN   0x0002//��ӦAD����ͨ��
#define SAM_FZDYUDATA_DEFHIGH (uint16_t)250
#define SAM_FZDYUDATA_DEFLOW  (uint16_t)230
#define SAM_FZDYUDATA_RESOLUTION  18311//ADһλ��Ӧʵ�ʸ�����Դ��ѹ��1000000��

#define SAM_STRUCT_LINK1U   0x0009 //LIKE1���ӵ�ѹ������Samp_DataArrƫ��
#define SAM_LINK1UDATA_AIN   0x0002//��ӦAD����ͨ��
#define SAM_LINK1UDATA_DEFHIGH (uint16_t)120
#define SAM_LINK1UDATA_DEFLOW  (uint16_t)0
#define SAM_LINK1UDATA_RESOLUTION 

#define SAM_STRUCT_LINK0U   0x000C //LIKE0���ӵ�ѹ������Samp_DataArrƫ��
#define SAM_LINK0UDATA_AIN   0x0002//��ӦAD����ͨ��
#define SAM_LINK0UDATA_DEFHIGH (uint16_t)120
#define SAM_LINK0UDATA_DEFLOW  (uint16_t)0
#define SAM_LINK0UDATA_RESOLUTION  
#define SAM_LINKUDATA_RESOLUTION //LINK���ӵ�ѹADһλ��Ӧʵ�ʵ�ص�ѹ�ı�(����LINK���ӵ�ѹ�Ĳ�����·��һ�������)




/*ÿ��ͨ����ʱ����������
ע:�������������Ͼ�Եĸ��
�Ե��ź�50Hz��һ�����ڲ�����
��ȷ���˳�����ź�*/
#define SAM_CHANNEL_TIMERCOUNT 40

//�����õ���״̬���ݽṹ�嶨��
#define SAM_DATASTATUS_LEN 40

#define SAMPDATABITINDEX_MX0USTAT  0//ĸ��0��ѹ״̬�������ֽ��е�ƫ��
#define SAMPDATABITMASK_MX0USTAT  (~((uint8_t)0x07<<SAMPDATABITINDEX_MX0USTAT))

#define SAMPDATABITINDEX_MX1USTAT  3//ĸ��1��ѹ״̬�������ֽ��е�ƫ��
#define SAMPDATABITMASK_MX1USTAT  (~((uint8_t)0x07<<SAMPDATABITINDEX_MX1USTAT))

#define SAMPDATABITINDEX_BAT0USTAT  0//���0���ѹ״̬�������ֽ��е�ƫ��
#define SAMPDATABITMASK_BAT0USTAT   (~((uint8_t)0x0F<<SAMPDATABITINDEX_BAT0USTAT))

#define SAMPDATABITINDEX_BAT1USTAT  0//���1���ѹ״̬�������ֽ��е�ƫ��
#define SAMPDATABITMASK_BAT1USTAT   (~((uint8_t)0x0F<<SAMPDATABITINDEX_BAT1USTAT))

#define SAMPDATABITINDEX_BAT0ISTAT  5//���0�����״̬�������ֽ��е�ƫ��
#define SAMPDATABITMASK_BAT0ISTAT  (~((uint8_t)0x01<<SAMPDATABITINDEX_BAT0ISTAT))

#define SAMPDATABITINDEX_BAT1ISTAT  5//���1�����״̬�������ֽ��е�ƫ��
#define SAMPDATABITMASK_BAT1ISTAT  (~((uint8_t)0x01<<SAMPDATABITINDEX_BAT1ISTAT))

#define SAMPDATABITINDEX_JYBAT0PSTAT    //���0���Ե���Ե�״̬�������ֽ���ƫ��
#define SAMPDATABITMASK_JYBAT0PSTAT

#define SAMPDATABITINDEX_JYBAT1PSTAT    //���1���Ե���Ե�״̬�������ֽ���ƫ��
#define SAMPDATABITMASK_JYBAT1PSTAT

#define SAMPDATABITINDEX_JYBAT0NSTAT     //���0���Ե���Ե�״̬�������ֽ���ƫ��
#define SAMPDATABITMASK_JYBAT0NSTAT 

#define SAMPDATABITINDEX_JYBAT1NSTAT     //���1���Ե���Ե�״̬�������ֽ���ƫ��
#define SAMPDATABITMASK_JYBAT1NSTAT

#define SAMPDATABITINDEX_JYRESBAT0STAT  4//���0���Ե�Եص���״̬�������ֽ���ƫ��
#define SAMPDATABITMASK_JYRESBAT0STAT   (~((uint8_t)0x01<<SAMPDATABITINDEX_JYRESBAT0STAT))

#define SAMPDATABITINDEX_JYRESBAT1STAT  4//���1���Ե�Եص���״̬�������ֽ���ƫ��
#define SAMPDATABITMASK_JYRESBAT1STAT   (~((uint8_t)0x01<<SAMPDATABITINDEX_JYRESBAT1STAT))

#define SAMPDATABITINDEX_TEMPSTAT  1//�¶�״̬�������ֽ��е�ƫ��
#define SAMPDATABITMASK_TEMPSTAT  (~((uint8_t)0x03<<SAMPDATABITINDEX_TEMPSTAT))

#define SAMPDATABITINDEX_HUMISTAT  3//ʪ��״̬�������ֽ��е�ƫ��
#define SAMPDATABITMASK_HUMISTAT  (~((uint8_t)0x03<<SAMPDATABITINDEX_HUMISTAT))

#define SAMPDATABITINDEX_FZDYUSTAT     0//������Դ��ѹ״̬�������ֽ��е�ƫ��
#define SAMPDATABITMASK_FZDYUSTAT      (~((uint8_t)0x01<<SAMPDATABITINDEX_FZDYUSTAT))

#define SAMPDATABITINDEX_LINK0USTAT     5//LIKE0���ӵ�ѹ״̬�������ֽ��е�ƫ��
#define SAMPDATABITMASK_LINK0USTAT      (~((uint8_t)0x03<<SAMPDATABITINDEX_LINK0USTAT))

#define SAMPDATABITINDEX_LINK1USTAT     1//LIKE1���ӵ�ѹ״̬�������ֽ��е�ƫ��
#define SAMPDATABITMASK_LINK1USTAT      (~((uint8_t)0x03<<SAMPDATABITINDEX_LINK1USTAT))

//���Ͽ�������״̬ƫ��(�첽״̬�����ֽ�1����)
#define SAMPDATABIT_KMMAINRDQSTAT ((uint8_t)0x01<<0)
#define SAMPDATABIT_KMMAINTRIPSTAT ((uint8_t)0x01<<1)
#define SAMPDATABIT_KMBRANCH1TRIPSTAT ((uint8_t)0x01<<2)
#define SAMPDATABIT_KMBRANCH2TRIPSTAT ((uint8_t)0x01<<3)
#define SAMPDATABIT_FLQSTAT ((uint8_t)0x01<<4)
#define SAMPDATABIT_ACINTRIPSTAT ((uint8_t)0x01<<5)
#define SAMPDATABIT_STOPSTAT ((uint8_t)0x01<<6)

//����״̬�ֽ�ƫ�ƶ���
#define SW_AC_IN_OFFSET ((uint8_t)0x01)//���������բ״̬��RUNSTATUS1��ƫ��
#define SW_KM_MAIN_OFFSET ((uint8_t)0x01<<3) //��ֱ�������բ״̬��RUNSTATUS1��ƫ��
#define SW_KM_BRANCH1_OFFSET ((uint8_t)0x01<<4)//LINK0��բ״̬��RUNSTATUS1��ƫ��
#define SW_FZDY_BRANCH1_OFFSET ((uint8_t)0x01<<7)//FZDY��LINK0һ��̵���״̬��RUNSTATUS1��ƫ��

#define SW_KM_BRANCH2_OFFSET ((uint8_t)0x01<<0)//LINK1��բ״̬��RUNSTATUS2��ƫ��
#define SW_FZDY_BRANCH2_OFFSET ((uint8_t)0x01<<3)//FZDY��LINK1һ��̵���״̬��RUNSTATUS2��ƫ��


typedef struct 
{
    uint16_t VinKM0; //ĸ��0��ѹ ʵ�ʵ�ѹ*10
    uint16_t VinKM1;//ĸ��1��ѹ ʵ�ʵ�ѹ*10
    uint16_t VinBAT0;//���0���ѹ��ʵ�ʵ�ѹ*10
    uint16_t VinBAT1;//���1���ѹ��ʵ�ʵ�ѹ*10
    uint16_t IoutBAT0; //���0�������ʵ�ʵ���*10
    uint16_t IoutBAT1; //���1�������ʵ�ʵ���*10
    uint16_t VinBAT0RESP;//���0�����Եص��裬ʵ�ʵ���*10��K��
    uint16_t VinBAT1RESP;//���1�����Եص��裬ʵ�ʵ���*10��K��
    uint16_t VinBAT0RESN;//���0�鸺�Եص��裬ʵ�ʵ���*10��K��
    uint16_t VinBAT1RESN;//���1�鸺�Եص��裬ʵ�ʵ���*10��K��
    uint16_t Temp ;//��ǰ�¶ȣ�ʵ���¶�*10
    uint16_t Humi; //��ǰʪ�ȣ�ʵ��ʪ��*10
    uint16_t Reserve2;// Ԥ��	   
    uint16_t Reserve3; //Ԥ�� 
    uint8_t Ver; //����汾�ţ� ��10 ��ʾ Ver= 1.0;  ����һ��29�ֽ�
    uint8_t Flag_prtc1;   //���Ͽ��ౣ��״̬�ֽ�1����ͬ��״̬��
    //BIT0��ĸ��0��ѹ״̬λ��0 = �޹��ϣ�1 = ĸ�߹�ѹ�� 
    //BIT1��ĸ��0Ƿѹ״̬λ��0 = �޹��ϣ�1 = ĸ��Ƿѹ�� 
    //BIT2��ĸ��0��·״̬λ��0 = �޹��ϣ�1 = ĸ�߶�·��
    //BIT3��ĸ��1��ѹ״̬λ��0 = �޹��ϣ�1 = ĸ�߹�ѹ�� 
    //BIT4��ĸ��1Ƿѹ״̬λ��0 = �޹��ϣ�1 = ĸ��Ƿѹ�� 
    //BIT5��ĸ��1��·״̬λ��0 = �޹��ϣ�1 = ĸ�߶�·��
    //ʣ��bit����
    uint8_t Flag_prtc2;   //���Ͽ��ౣ��״̬�ֽ�2����ͬ��״̬�� 
    //BIT0�����0���ѹ״̬λ��0 = �޹��ϣ�1 = ��ع�ѹ�� 
    //BIT1�����0��Ƿѹ״̬λ��0 = �޹��ϣ�1 = ���Ƿѹ�� 
    //BIT2�����0���·״̬λ��0 = �޹��ϣ�1 = ��ض�·�� 
    //BIT3�����0�鷴��״̬λ��0 = �޹��ϣ�1 = ��ط��ӣ� 
    //BIT4�����0���Ե״̬λ��0 = �޹��ϣ�1 = ��ؾ�Ե���ϣ� 
    //BIT5�����0�����״̬λ��0 = �޹��ϣ�1 = ��ع����� 
    //ʣ��bit����
    uint8_t Flag_prtc3;   //���Ͽ��ౣ��״̬�ֽ�3����ͬ��״̬�� 
    //BIT0�����1���ѹ״̬λ��0 = �޹��ϣ�1 = ��ع�ѹ�� 
    //BIT1�����1��Ƿѹ״̬λ��0 = �޹��ϣ�1 = ���Ƿѹ�� 
    //BIT2�����1���·״̬λ��0 = �޹��ϣ�1 = ��ض�·�� 
    //BIT3�����1�鷴��״̬λ��0 = �޹��ϣ�1 = ��ط��ӣ� 
    //BIT4�����1���Ե״̬λ��0 = �޹��ϣ�1 = ��ؾ�Ե���ϣ� 
    //BIT5�����1�����״̬λ��0 = �޹��ϣ�1 = ��ع�����
    //ʣ��bit����
    uint8_t Flag_prtc4;   //���Ͽ��ౣ��״̬�ֽ�4����ͬ��״̬��
    //BIT0��������Դ״̬λ��0 = �޹��ϣ�1 = ������Դ�쳣�� 
    //BIT1..2���¶�״̬λ��0 0= �޹��ϣ�01 = �¶ȹ��ߣ�10= �¶ȹ���
    //BIT3..4��ʪ��״̬λ��0 0= �޹��ϣ�01 = ʪ�ȹ���10=ʪ�ȹ���
    //BIT5��ͨѶ��ʱ���ϣ�һ����������ã�
    //ʣ��bit����
    uint8_t Flag_prtc5;   //���Ͽ��ౣ��״̬�ֽ�5��������ͬ��״̬��
    uint8_t Flag_prtc6;   //���Ͽ��ౣ��״̬�ֽ�4�����첽״̬��
    //BIT0����ֱ������۶���״̬λ��0 = �޹��ϣ�1 = �۶ϣ� 
    //BIT1����ֱ�������բ״̬λ��0 = �޹��ϣ�1 = ��բ��
    //BIT2�����1·�����բ״̬λ��0 = �޹��ϣ�1 = ��բ��
    //BIT3�����2·�����բ״̬λ��0 = �޹��ϣ�1 = ��բ��
    //BIT4��������״̬λ��0 = �޹��ϣ�1 = ���������ϣ�
    //BIT5������������բ��0 = �޹��ϣ�1 = ��բ��
    //BIT6����ͣ״̬λ��0 = �޹��ϣ�1 = ��ͣ��
    //BIT7������
    uint8_t Flag_prtc7;   //���Ͽ��ౣ��״̬�ֽ�5�����첽״̬��
    //����
    uint8_t Flag_run1;   //��ǰ����״̬�ֽ�1��
    //BIT 0: ���������Ƿ��բ�� 0 = �֣�1 = �ϣ� 
    //BIT 1������״̬��    0 = δ���ȣ�1 = ���ȣ� 
    //BIT 2������״̬��  0 = δ������ 1 = ������ 
    //BIT 3����ֱ������Ƿ��բ��0 =�֣�1 =�ϣ� 
    //BIT 4�����ǹ1��բ״̬�� 0 = �֣� 1 = �ϣ� 
    //BIT 5..6�����ǹ1����״̬��00= δ���ӣ� 1 1= ���ӣ�01=���ӱ���״̬��
    //10= �����쳣
    //BIT 7�����ǹ1ͨ�Ÿ�����Դ��բ״̬�� 0 = �֣� 1 = �ϣ�
    uint8_t Flag_run2;   //��ǰ����״̬�ֽ�2��
    //BIT 0�����ǹ2��բ״̬��0 = �֣� 1 = �ϣ�
    //BIT 1..2�����ǹ2����״̬��00 = δ���ӣ� 11 = ���ӣ�01=���ӱ���״̬��10= �����쳣
    //BIT 3�����ǹ2ͨ�Ÿ�����Դ��բ״̬�� 0 = �֣� 1 = �ϣ�
    uint8_t Flag_run3;   //��ǰ����״̬�ֽ�3������ ״̬һ��10�ֽ�
    uint8_t Reserve4;
}Stu_ADSample_DATAStat;

typedef union
{
    Stu_ADSample_DATAStat DataStat;
    uint8_t data[SAM_DATASTATUS_LEN];

}Un_ADSamp_Stat;

typedef struct AD_Sample_Status
{
    uint8_t ready;//׼��ѡ��Ĳ���������ƫ�Ʊ��
    uint8_t now;//��ǰѡ��Ĳ���������ƫ�Ʊ��
    uint16_t cmd;//��ǰ�̵��������ֽ�
    //uint8_t flash_status;//flashд״̬
    uint16_t SysFault;//��ǰϵͳ���й��ϣ�bit0 UARTͨѶ����
    uint32_t Cal_UsegVal;//��ѹУ׼ÿ��ʵ�ʵ�ѹ*1000000
    uint32_t Cal_IsegVal;//����У׼ÿ��ʵ�ʵ���*1000000

    uint8_t Cal_UIchmin[2];//�Ѿ�У׼���ĵ�ѹ������Сֵ
    uint16_t cal_status;//У׼�������̱�־
    uint16_t cal_ch0;//0����������У׼ֵ��ĸ��0��͵��0���ѹ
    uint16_t cal_ch1;//1����������У׼ֵ��ĸ��1��͵��1���ѹ
    uint16_t cal_ch2;//2����������У׼ֵ��ĸ��0�κ�1�ε���
}SAMP_Stream;

typedef struct AD_Sample_Option
{
    uint16_t Chan;//ģ���������ͨ��
    uint16_t Ain;//ģ��������ͨ������ADͨ��
    uint16_t ADdata;//��ǰ����ADֵ
    uint16_t highdata;//��Ӧ����ֵ������
    uint16_t lowdata;//��Ӧ����ֵ������
    uint8_t ADcalarrcol;//��ӦУ׼ֵ�����к�
    uint16_t (*Opratefun)(Stu_ADSample_DATAStat *sampdata, SAMP_Stream *streamdata);//��Ӧ����ͨ���Ĵ�����
    
}SAMP_Opx;//��Ӧ��ֵ���������ݴ���͹���ʵ�ֽṹ��

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



