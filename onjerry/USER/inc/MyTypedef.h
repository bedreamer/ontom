#ifndef _MYTYPEDEF_H_
#define _MYTYPEDEF_H_



typedef struct 
{
    uint16_t Cmd1;// �����ֽ�1
    //bit0: ��ֱ������Ƿ�պϣ�0���Ͽ� 1���պ�
    //bit1: ���ǹ1������Դ�Ƿ�պϣ�0���Ͽ� 1���պ�
    //bit2�����ǹ1��翪���Ƿ�պϣ�0���Ͽ� 1���պ�
    //bit3: ���ǹ1�������Ƿ�պϣ�0���Ͽ� 1���պ�
    //bit4�����ǹ2������Դ�Ƿ�պϣ�0���Ͽ� 1���պ�
    //bit5�����ǹ2��翪���Ƿ�պϣ�0���Ͽ� 1���պ�
    //bit6: ���ǹ2�������Ƿ�պϣ�0���Ͽ� 1���պ�
    //bit7: ������У�0���Ͽ� 1���պ�
    //bit8: ���ϵƿ��ƣ�0���Ͽ� 1���պ�
    //ʣ��bit����
    uint16_t Cmd1_Backup;//ֵ��Cmd1һ��
    uint16_t Max_VinKM0;//ĸ��0��ѹ���ޣ�ʵ�ʵ�ѹ*10
    uint16_t Min_VinKM0;//ĸ��0��ѹ���ޣ�ʵ�ʵ�ѹ*10
    uint16_t Max_VinKM1;//ĸ��1��ѹ���ޣ�ʵ�ʵ�ѹ*10
    uint16_t Min_VinKM1;//ĸ��1��ѹ���ޣ�ʵ�ʵ�ѹ*10
    uint16_t Max_VinBAT0;//���0��ѹ���ޣ�ʵ�ʵ�ѹ*10
    uint16_t Min_VinBAT0;//���0��ѹ���ޣ�ʵ�ʵ�ѹ*10
    uint16_t Max_VinBAT1;//���1��ѹ���ޣ�ʵ�ʵ�ѹ*10
    uint16_t Min_VinBAT1;//���1��ѹ���ޣ�ʵ�ʵ�ѹ*10
    uint16_t Max_IoutBAT0 ;//���0��������ޣ�ʵ�ʵ���*10
    uint16_t Max_IoutBAT1 ;//���1��������ޣ�ʵ�ʵ���*10
    uint16_t Min_VinBAT0RES ;//���0���Ե�澯ֵ��/V
    uint16_t Min_VinBAT1RES ; //���1���Ե�澯ֵ��/V
    uint16_t Max_Isen ;//������������A
    uint16_t Cal_CH0 ;//KM0��BAT0��ǰУ׼��ѹ��0Ϊ��У׼
    uint16_t Cal_CH1 ; //KM1��BAT1��ǰУ׼��ѹ��0Ϊ��У׼
    uint16_t Cal_CH2 ; // BAT0��BAT1��ǰУ׼������0Ϊ��У׼
    uint8_t  New_Addr;//��ַ������ֵ
    uint8_t  Reserve;//����
    /*���IO�������IO���ã�Byte0: ���ǹ1������Դ��
    Byte1: ���ǹ1��翪�أ�Byte2: ���ǹ1��������
    Byte3: ���ǹ2������Դ��Byte4: ���ǹ2��翪�أ�
    Byte5: ���ǹ2��������Byte6: ������У�
    Byte7: ���ϵƣ�ʣ�ౣ��ÿ���ֽڱ�ʾ��Ӧ�̵�����*/
    uint8_t OutMemb[16];



}Stu_ADSample_SetData;

#define BIGTOLIT_U16(dirnum) ((((uint16_t)(dirnum) << 8)|((uint16_t)(dirnum) >> 8)) & 0x0000FFFF)//���תС��




#endif

