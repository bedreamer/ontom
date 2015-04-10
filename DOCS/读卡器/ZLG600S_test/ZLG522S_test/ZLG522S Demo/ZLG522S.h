////////////////////////////////////////////////Copyright (c)////////////////////////////////////////////////
//                                      广州周立功单片机发展有限公司
//                                            研    究    所
//                                              金卡产品线 
//
//                                          http://www.zlgmcu.com
//
//--------------文件信息-------------------------------------------------------------------------------------
// 文 件 名:    ZLG522S(P).h
// 文件描述:    ZLG522S(P) 模块动态库
// 版    本:    V1.00 
// 创 建 人:    曾本森
// 创建日期:    2010.11.23
//===========================================================================================================
//-----------------修改记录----------------------------------------------------------------------------------
// 修改内容:
// 当前版本:    
// 修 改 人:
// 修改日期:
//-----------------------------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __ZLG522S_H
#define __ZLG522S_H

#ifdef __cplusplus
	extern "C" {
#endif
//===========================================================================================================
#include "GlobalDefine.h"
#include "StatusCode.h"
//============= 常量定义 ====================================================================================	
//============= 全局变量/函数定义 ===========================================================================
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:	bool ZLG522S_Init(const char *pszComName, unsigned int uiBaudRate, unsigned int uiModuleType)
// 函数功能:	ZLG5xxS系列模块初始化
// 入口参数:	const char *pszComName			// 以'\0'结束的串口名称，如"COM1"
//				unsigned int uiBaudRate			// 设置波特率
//				unsigned int uiModuleType		// 模块类型				
// 出口参数:	-
// 返 回 值:	true -- 初始化成功;	false -- 初始化失败
// 说    明:	波特率只能是CBR_9600, CBR_19200, CBR_28800, CBR_38400, CBR_57600, CBR_115200
//				模块类型为MT_ZLG500S, MT_ZLG522S, MT_ZLG522SP, MT_ZLG522SP_L, MT_ZLG522SP_EX
//				只有ZLG522SP V1.01以上(包括V1.01)才能选择MT_ZLG522SP_L和MT_ZLG522SP_EX2种模式
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool __stdcall ZLG522S_Init(const char *pszComName, unsigned int uiBaudRate, unsigned int uiModuleType);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:	HANDLE ZLG522S_Open(unsigned int uiModuleAdr)
// 函数功能:	打开ZLG522S
// 入口参数:	unsigned int uiModuleAdr		// 地址(仅限于扩展通讯帧，0x00为通用地址)				
// 出口参数:	-
// 返 回 值:	非负数 -- 成功；	-1 -- 失败
// 说    明:	若没有初始化，则默认选择COM1
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
HANDLE __stdcall ZLG522S_Open(unsigned int uiModuleAdr);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:	void ZLG522S_Exit(HANDLE hCtrl)
// 函数功能:	ZLG522S关闭退出
// 入口参数:	HANDLE hCtrl					// 通讯控制				
// 出口参数:	-
// 返 回 值:	-
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void __stdcall ZLG522S_Exit(HANDLE hCtrl);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_TransmitCmd(HANDLE hCtrl, CDU *RWCmd, CDU *RWRes)
// 函数功能:    向读写器发送命令并接收回应
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				CDU *RWCmd						// 命令				
// 出口参数:    CDU *RWRes						// 回应
// 返 回 值:    -
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_TransmitCmd(HANDLE hCtrl, CDU *pRWCmd, CDU *pRWRes);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_GetResponse(HANDLE hCtrl, unsigned int uiTimeOver, CDU *pRWRes)
// 函数功能:    接收回应
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned int uiTimeOverCnt		// 超时时间(ms)				
// 出口参数:    CDU *RWRes						// 回应
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_GetResponse(HANDLE hCtrl, unsigned int uiTimeOver, CDU *pRWRes);

//===========================================================================================================
// DevCtrl
//===========================================================================================================
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:	void  ZLG522S_GetDllInfo(HANDLE hCtrl, char *pDllInfo,unsigned int nRBufSize)
// 函数功能:	获取DLL版本信息
// 入口参数:	HANDLE hCtrl					// 通讯控制
//				unsigned int nRBufSize			// 接收缓冲区大小			
// 出口参数:	char *pDllInfo					// DLL版本信息
//
// 返 回 值:
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void __stdcall ZLG522S_GetDllInfo(HANDLE hCtrl, char *pDllInfo, unsigned int nRBufSize);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:	unsigned char ZLG522S_GetDevInfo(HANDLE hCtrl, unsigned char *pDevInfo, 
//												 unsigned int nRBufSize, unsigned int *pInfoLen)
// 函数功能:	获取设备信息
// 入口参数:	HANDLE hCtrl					// 通讯控制
//				unsigned int nRBufSize			// 接收缓冲区的长度				
// 出口参数:	unsigned char *pDevInfo			// 设备信息
//				unsigned int *pInfoLen			// 设备信息长度
// 返 回 值:	STATUS_SUCCESS -- 操作成功，其他值 -- 操作失败	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_GetDevInfo(HANDLE hCtrl, unsigned char *pDevInfo, unsigned int nRBufSize, 
										   unsigned int *pInfoLen);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:	unsigned char ZLG522S_PcdConfig(HANDLE hCtrl, unsigned char ucType)
// 函数功能:	配置读写器的PCD
// 入口参数:	HANDLE hCtrl					// 通讯控制
//				unsigned char ucType			//  保留
// 出口参数:	-
// 返 回 值:	STATUS_SUCCESS -- 操作成功，其他值 -- 操作失败
// 说    明:    ZLG522S系列模块只支持ISO14443_TYPEA	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_PcdConfig(HANDLE hCtrl, unsigned char ucType);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:	unsigned char ZLG522S_PcdClose(HANDLE hCtrl)
// 函数功能:	关闭读写器的PCD
// 入口参数:	HANDLE hCtrl					// 通讯控制
// 出口参数:	-
// 返 回 值:	STATUS_SUCCESS -- 操作成功，其他值 -- 操作失败	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_PcdClose(HANDLE hCtrl);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_PcdLoadKey(HANDLE hCtrl, unsigned char ucKeyType, 
//												 unsigned char ucKeySector, unsigned char ucKeyLen, 
//												 const unsigned char *pKey)
// 函数功能:    将Mifare 卡验证密钥存放到读写器内
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucKeyType			// 密钥类型 KEY_TYPE_A -- 密钥A;	KEY_TYPE_B -- 密钥B;
//              unsigned char ucKeySector		// 密钥存放扇区
//				unsigned char ucKeyLen			// 密钥的字节数(6/16字节)
//				unsigned char *pKey				// 写入的密钥
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 操作成功，其他值 -- 操作失败
// 说    明:	该函数是对读写器操作，不是对卡片执行验证密钥操作，
//				ZLG522SP支持2种密钥长度,若是输入6字节的密钥，则模块自动复制2次取前16字节作为密钥
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_PcdLoadKey(HANDLE hCtrl, unsigned char ucKeyType, unsigned char ucKeySector,
										   unsigned char ucKeyLen, const unsigned char *pKey);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_PcdWriteReg(HANDLE hCtrl, unsigned char ucRegAdr, unsigned char ucValue)
// 函数功能:    写非接触式读写卡芯片的寄存器
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucRegAdr          // 寄存器地址
//              unsigned char ucValue           // 写入的数据
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 操作成功，其他值 -- 操作失败
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_PcdWriteReg(HANDLE hCtrl, unsigned char ucRegAdr, unsigned char ucValue);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_PcdReadReg(HANDLE hCtrl, unsigned char ucRegAdr, unsigned char *pValue)
// 函数功能:    读非接触式读写卡芯片的寄存器
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucRegAdr			// 寄存器地址
// 出口参数:    unsigned char *pValue			// 读出的数据
// 返 回 值:    STATUS_SUCCESS -- 操作成功，其他值 -- 操作失败
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_PcdReadReg(HANDLE hCtrl, unsigned char ucRegAdr, unsigned char *pValue);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SetDevWorkMode(HANDLE hCtrl, unsigned char ucChangeMode, 
//													 unsigned char ucWorkMode, unsigned char ucBaudRateNO,
//													 unsigned char ucModuleAdr, AUTO_DETECT_PAR AutoDetPar)
// 函数功能:    设置模块工作模式(ZLG522SP)
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucChangeMode		// ucChangeMode.0	= 1		修改工作模式
//												// ucChangeMode.1	= 1		修改波特率 
//												// ucChangeMode.2	= 1		修改模块地址 
//												// ucChangeMode.3	= 1		修改卡片自动检测模式 
//				unsigned char ucWorkMode		// ucWorkMode.2:0	= 000	自动检测模式
//												//					= 001	UART通讯模式
//												//					= 010	I2C通讯信模式 
//												//					= 其他	保留
//												// ucWorkMode.3		= 0 -- 标准通讯帧模式；1 -- 扩展通讯帧模式
//												// ucWorkMode.4		= 0	-- 命令--回应模式；1 -- 卡片自动检测模式		
//				unsigned char ucBaudRateNO		// 波特率编号：取值范围0～7(默认值为1)
//												// 编号:   0	 1	   2	 3	   4	 5	    6(不支持)	7(不支持)
//												// 波特率: 9600  19200 28800 38400 57600 115200 172800		230400
//				unsigned char ucModuleAdr		// 模块地址，该地址用于UART的扩展通信帧或I2C通信的地址
//				OTO_DETECT_PAR AutoDetPar		// 自动检测参数
// 出口参数:	-
// 返 回 值:    STATUS_SUCCESS -- 操作成功，其他值 -- 操作失败
// 说	 明:	只有设置为UART通信或I2C通信模式才能设置为卡片自动检测模式
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SetDevWorkMode(HANDLE hCtrl,  unsigned char ucChangeMode, 
											   unsigned char ucWorkMode, unsigned char ucBaudRateNO,
											   unsigned char ucModuleAdr, AUTO_DETECT_PAR AutoDetPar);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_PcdSetTX(HANDLE hCtrl, unsigned char ucTXMode)
// 函数功能:    设置天线驱动模式(ZLG522SP)
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucTXMode			// 0x03 -- TX1、TX2同时驱动天线
//												// 0x02 -- TX2驱动天线
//												// 0x01 -- TX1驱动天线
//												// 0x00 -- 天线输出关闭
// 出口参数:	-
// 返 回 值:    STATUS_SUCCESS -- 操作成功，其他值 -- 操作失败
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_PcdSetTX(HANDLE hCtrl, unsigned char ucTXMode);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_GetDevWorkMode(HANDLE hCtrl, unsigned char *pWorkMode, 
//													 unsigned char *pBaudRateNO, unsigned char *pModuleAdr, 
//													 AUTO_DETECT_PAR *pAutoDetPar)
// 函数功能:    获取模块工作模式(ZLG522SP)
// 入口参数:    -				
// 出口参数:	HANDLE hCtrl					// 通讯控制
//				unsigned char *pWorkMode		// ucWorkMode.2:0	= 000	自动检测模式
//												//					= 001	UART通讯模式
//												//					= 010	I2C通讯信模式 
//												//					= 其他	保留
//												// ucWorkMode.3		= 0 -- 标准通讯帧模式；1 -- 扩展通讯帧模式
//												// ucWorkMode.4		= 0	-- 命令--回应模式；1 -- 卡片自动检测模式	
//				unsigned char *pBaudRateNO		// 波特率编号：取值范围0～7(默认值为1)
//												// 编号:   0	 1	   2	 3	   4	 5	    6(不支持)	7(不支持)
//												// 波特率: 9600  19200 28800 38400 57600 115200 172800		230400
//				unsigned char *pModuleAdr		// 模块地址，该地址用于UART的扩展通信帧或I2C通信的地址
//				OTO_DETECT_PAR *pAutoDetPar		// 自动检测参数
// 返 回 值:    STATUS_SUCCESS -- 操作成功，其他值 -- 操作失败
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char ZLG522S_GetDevWorkMode(HANDLE hCtrl, unsigned char *pWorkMode, unsigned char *pBaudRateNO,
									 unsigned char *pModuleAdr, AUTO_DETECT_PAR *pAutoDetPar);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:	unsigned char CardReset(HANDLE hCtrl, unsigned char ucPause_1ms,unsigned char ucWait_1ms)
// 函数功能:	使RF场产生1个暂停，让卡复位
// 入口参数:	HANDLE hCtrl					// 通讯控制
//				unsigned char ucPause_1ms				// 暂停时间，关闭RF场该时间后重新打开，
//														// 若为0则不重新打开
//				unsigned char ucWait_1ms				// RF场重新打开后等待时间，若为0则不等待
// 出口参数:	-
// 返 回 值:	-
// 说    明:	等待时间可根据卡的功耗而定，如Mifare1卡的功耗较小，等待数毫秒即可，
//				而CPU卡功耗较大，需要等待80毫秒左右。
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_CardReset(HANDLE hCtrl, unsigned char ucPause_1ms, unsigned char ucWait_1ms);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_ExChangeBlock(HANDLE hCtrl, unsigned char WTXM_CRC, unsigned char ucFWI, 
//													const unsigned char *pSBuf, unsigned int uiSBytes,
//													unsigned char *pRBuf, unsigned int uiRBufSize, 
//													unsigned int *pRBytes)
// 函数功能:    数据交互
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char WTXM_CRC			// WTXM_CRC.0	= 0  -- CRC禁止；	1 -- CRC使能
//												// WTXM_CRC.7:2	= WTXM，帧超时等待倍增系数
//				unsigned char FWI				// 帧等待系数取值范围(1-14)
//				unsigned char *pSBuf			// 发送数据缓冲区
//				unsigned int uiSBytes			// 发送的字节数
//				unsigned int uiRBufSize			// 接收缓冲区大小
// 出口参数:    unsigned char *pRBuf			// 接收数据缓冲区
//				unsigned int *pRBytes			// 接收的字节数
// 返 回 值:    STATUS_SUCCESS -- 成功；	其它值 -- 失败。
// 说    明:	帧等待时间为 ((0x01 << FWI) * WTXM) * 302us
//				0x01 << FWI) * WTXM的值不能超过0xFFFF
//				不同的模块，允许输入的长度不同
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_ExChangeBlock(HANDLE hCtrl, unsigned char WTXM_CRC, unsigned char ucFWI, 
											  const unsigned char *pSBuf, unsigned int uiSBytes,
											  unsigned char *pRBuf, unsigned int uiRBufSize, 
											  unsigned int *pRBytes);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_ExChangeBlock2(HANDLE hCtrl, unsigned char CRC_EN, unsigned int uiTre,
//													 const unsigned char *pSBuf, unsigned int nSendBits,
//													 unsigned int *pTime, unsigned char *pRBuf,
//													 unsigned int nRBufSize, unsigned int *pRcvBits)
// 函数功能:    PCD与PICC交换数据块
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char CRC_EN,			// CRC_EN.7 = 1 -- 发送使能CRC;	0 -- 发送禁止CRC; 
//												// CRC_EN.6 = 1 -- 接收使能CRC; 0 -- 接收禁止CRC; 
//				unsigned int uiTre				// 超时时间 = uiTre *  4.7us
//				unsigned char *pSBuf			// 发送的数据
//				unsigned int nSendBits			// 发送的位数，若发送的位数不是8的整数倍，则发送CRC使能无效		
//				unsigned int nRBufSize			// 接收缓冲区的大小
// 出口参数:    unsigned int *pTime				// 执行的时间 = pTime *  4.7us
//				unsigned char *pRBuf			// 接收的数据
//				unsigned int *pRcvBits			// 接收的位数
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	ZLG500S和ZLG522S不支持该命令 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_ExChangeBlock2(HANDLE hCtrl,  unsigned char CRC_EN, unsigned int uiTre,
												const unsigned char *pSBuf, unsigned int nSendBits,
												unsigned int *pTime, unsigned char *pRBuf,
												unsigned int nRBufSize, unsigned int *pRcvBits);
//===========================================================================================================
// Mifare
//===========================================================================================================
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    void ZLG522S_ValueToData(HANDLE hCtrl, unsigned char *pBuf, unsigned char usBNr, long lValue)
// 功能描述:    将值转化为数据块模式
// 输入参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char usBNr				// 值存放地址
//				long iValue						// 值
// 输出参数:    unsigned char *pBuf				// 16字节值块格式数据
// 返 回 值:    -
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void __stdcall ZLG522S_ValueToData(unsigned char *pBuf, unsigned char ucBNr, long lValue);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_DataToValue(unsigned char *pBuf, long *pValue)
// 功能描述:    将数据块转化为值
// 输入参数:    unsigned char *pBuf				// 16字节值块格式数据	
// 输出参数:    long *pValue					// 值()
// 返 回 值:    -
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_DataToValue(unsigned char *pBuf, long *pValue);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_MFActivate(HANDLE hCtrl,  unsigned char ucMode, unsigned char ucReqCode,  
//												 ACTIVEPARAA *pAInfo)
// 函数功能:   	Mifare系列卡激活
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucMode			// 保留
//				unsigned char ucReqCode			// 请求代码
// 出口参数:    ACTIVEPARAA *pAInfo				// ISO14443 A卡激活信息
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。  
// 说    明:	该函数自动识别M1 和PLUS CPU卡的SL1、SL2和SL3
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_MFActivate(HANDLE hCtrl, unsigned char ucMode, unsigned char ucReqCode, 
										   ACTIVEPARAA *pAInfo);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char MFDeSelect(HANDLE hCtrl)
// 函数功能:    将激活的卡挂起(ISO14443-4A)
// 输入参数:     HANDLE hCtrl					// 通讯控制
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_MFDeSelect(HANDLE hCtrl);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_PiccHalt(HANDLE hCtrl)
// 函数功能:   	将卡置为HALT状态(ISO14443-3A)
// 入口参数:    -
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_MFHalt(HANDLE hCtrl);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_MFAuth(HANDLE hCtrl, unsigned char ucAuthMode, unsigned char ucBNr, 
//											 const unsigned char *pKey)
// 函数功能:   	Mifare卡密钥验证
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucAuthMode		// 验证模式 
//												//	KEY_TYPE_A | KEY_SOURCE_EXT	外部输入的密钥A验证
//												//	KEY_TYPE_A | KEY_SOURCE_E2	内部E2的密钥A验证
//												//	KEY_TYPE_B | KEY_SOURCE_EXT	外部输入的密钥B验证
//												//	KEY_TYPE_B | KEY_SOURCE_E2	内部E2的密钥B验证
//				unsigned char ucBNr				// 验证的块号
//				unsigned char *pKey				// 使用KEY_SOURCE_E2模式时，pKey[0]为密钥存放的扇区,其它无效
//												// 使用KEY_SOURCE_EXT模式时，pKey为16字节密钥
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	该函数自动识别自动识别M1 和PLUS CPU卡的SL1、SL2和SL3
//				对于只有6字节密钥的情况(如M1 S50/70卡)则截取输入的16字节密钥的前6字节作为密钥
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_MFAuth(HANDLE hCtrl, unsigned char ucAuthMode, unsigned char ucBNr, 
									   const unsigned char *pKey);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_MFRead(HANDLE hCtrl, unsigned char ucBNr, unsigned char ucExt, 
//											 unsigned char *pBuf)
// 函数功能:   	MF卡多块读
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucBNr				// 读的起始块号
//				unsigned char ucExt				// 读的块数(取值范围1～3)
// 出口参数:    unsigned char *pBuf				// 读出的数据
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	当模块为ZLG522S时，该函数只支持M1 S50/70卡和PLUS SL1的卡,且ucExt固定为1
//				当模块为ZLG522SP时，该函数自动识别自动识别M1 和PLUS CPU卡的SL1、SL2和SL3
//				对于MF1和PLUS SL1的卡，所有被操作的块需要在同一扇区内，否则会出错
//				对于PLUS SL2/SL3的卡，建议所操作的块均在同一扇区内(因为操作块数大于1时，会自动跳过配置块)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_MFRead(HANDLE hCtrl, unsigned char ucBNr, 
									   unsigned char ucExt, unsigned char *pBuf);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_MFWrite(HANDLE hCtrl, unsigned char ucBNr, unsigned char ucExt, 
//											  const unsigned char *pBuf)
// 函数功能:   	MF卡多块写
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucBNr				// 写的起始块号
//				unsigned char ucExt				// 写的块数(取值范围1～3)
// 出口参数:    unsigned char *pBuf				// 写入的数据
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	当模块为ZLG522S时，该函数只支持M1 S50/70卡和PLUS SL1的卡
//				当模块为ZLG522SP时，该函数自动识别自动识别M1 和PLUS CPU卡的SL1、SL2和SL3
//				对于MF1和PLUS SL1的卡，所有被操作的块需要在同一扇区内，否则会出错
//				对于PLUS SL2/SL3的卡，建议所操作的块均在同一扇区内(因为操作块数大于1时，会自动跳过配置块)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_MFWrite(HANDLE hCtrl, unsigned char ucBNr, unsigned char ucExt, 
										const unsigned char *pBuf);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_MFReadValue(HANDLE hCtrl, unsigned char ucBNr, long *pValue)
// 函数功能:   	MF卡读取值块(扩展函数)
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucBNr				// 读的块号
// 出口参数:    long *pValue					// 读出的值
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	当模块为ZLG522S时，该函数只支持M1 S50/70卡和PLUS SL1的卡
//				当模块为ZLG522SP时，该函数自动识别自动识别M1 和PLUS CPU卡的SL1、SL2和SL3
//				PLUS S/X SL2 和 PLUS S SL3不支持值块
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_MFReadValue(HANDLE hCtrl, unsigned char ucBNr, long *pValue);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_MFWriteValue(HANDLE hCtrl, unsigned char ucBNr, long lValue)
// 函数功能:   	MF卡写入值
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucBNr				// 写的块号
//				long lValue						// 写入的值
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	当模块为ZLG522S时，该函数只支持M1 S50/70卡和PLUS SL1的卡
//				当模块为ZLG522SP时，该函数自动识别自动识别M1 和PLUS CPU卡的SL1、SL2和SL3
//				PLUS S/X SL2 和 PLUS S SL3不支持值块
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_MFWriteValue(HANDLE hCtrl, unsigned char ucBNr, long lValue);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_MFValueOper(HANDLE hCtrl, unsigned char ucOperMode, unsigned char ucSBNr, 
//												  unsigned char ucDBNr, long lValue)
// 函数功能:    Mifare值操作命令(PLUS CPU卡的SL2无值操作)
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucOperMode		// 值操作模式
//													MIFARE_INCREMENT 将源块的值加上指定的值存放到目的块
//													MIFARE_DECREMENT 将源块的值减去指定的值存放到目的块
//													MIFARE_RESTORE   将源块的值复制到目的块
//				unsigned char ucSBNr			// 操作的源块地址。
//				unsigned char ucDBNr			// 值操作的目地块
//				long lValue						// 非负的有符号数。
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	当模块为ZLG522S时，该函数只支持M1 S50/70卡和PLUS SL1的卡
//				当模块为ZLG522SP时，该函数自动识别自动识别M1 和PLUS CPU卡的SL1、SL2和SL3
//				MIFARE_RESTORE 操作模式是将指定的值块复制到目的块，一般用于值块操作的备份处理(该模式下，
//				参数值无效，为任意值均可)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_MFValueOper(HANDLE hCtrl, unsigned char ucOperMode, unsigned char ucSBNr, 
											unsigned char ucDBNr, long lValue);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_MF0ULWrite(HANDLE hCtrl, unsigned char ucBNr, const unsigned char *pBuf)
// 函数功能:    MF0 UL写命令
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucBNr				// 写入的块地址。
//				unsigned char *pBuf				// 4字节写数据缓冲区首址。
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_MF0ULWrite(HANDLE hCtrl, unsigned char ucBNr, const unsigned char *pBuf);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_MF1Auth(HANDLE hCtrl, unsigned char ucAuthMode, unsigned char ucBNr, 
//											  const unsigned char *pKey)
// 函数功能:   	MF1卡密钥验证
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucAuthMode		// 验证模式 
//												//	KEY_TYPE_A | KEY_SOURCE_EXT	外部输入的密钥A验证
//												//	KEY_TYPE_A | KEY_SOURCE_E2	内部E2的密钥A验证
//												//	KEY_TYPE_B | KEY_SOURCE_EXT	外部输入的密钥B验证
//												//	KEY_TYPE_B | KEY_SOURCE_E2	内部E2的密钥B验证
//				unsigned char ucBNr				// 验证的块号
//				unsigned char *pKey				// 使用KEY_SOURCE_E2模式时，pKey[0]为密钥存放的扇区,其它无效
//												// 使用KEY_SOURCE_EXT模式时，pKey为6字节密钥
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	该函数只支持M1 S50/70卡和PLUS SL1的卡
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_MF1Auth(HANDLE hCtrl, unsigned char ucAuthMode, unsigned char ucBNr, 
									   const unsigned char *pKey);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_MFRead(HANDLE hCtrl, unsigned char ucBNr, unsigned char *pBuf)
// 函数功能:   	MF1卡读
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucBNr				// 读的块号
// 出口参数:    unsigned char *pBuf				// 读出的数据
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	该函数只支持M1 S50/70卡和PLUS SL1的卡
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_MF1Read(HANDLE hCtrl, unsigned char ucBNr, unsigned char *pBuf);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_MF1Write(HANDLE hCtrl, unsigned char ucBNr, const unsigned char *pBuf)
// 函数功能:   	MF1卡写
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucBNr				// 写的块号
// 出口参数:    unsigned char *pBuf				// 写入的数据
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	该函数只支持M1 S50/70卡和PLUS SL1的卡
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_MF1Write(HANDLE hCtrl, unsigned char ucBNr, const unsigned char *pBuf);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_MF1ReadValue(HANDLE hCtrl, unsigned char ucBNr, long *pValue)
// 函数功能:   	MF1卡读取值块
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucBNr				// 读的块号
// 出口参数:    long *pValue					// 读出的值
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	该函数只支持M1 S50/70卡和PLUS SL1的卡
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_MF1ReadValue(HANDLE hCtrl, unsigned char ucBNr, long *pValue);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_MFWriteValue(HANDLE hCtrl, unsigned char ucBNr, long lValue)
// 函数功能:   	MF1卡写入值
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucBNr				// 写的块号
//				long lValue						// 写入的值
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	当模块为ZLG522S时，该函数只支持M1 S50/70卡和PLUS SL1的卡
//				当模块为ZLG522SP时，该函数自动识别自动识别M1 和PLUS CPU卡的SL1、SL2和SL3
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_MF1WriteValue(HANDLE hCtrl, unsigned char ucBNr, long lValue);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_MF1ValueOper(HANDLE hCtrl, unsigned char ucOperMode, unsigned char ucSBNr, 
//												  unsigned char ucDBNr, long lValue)
// 函数功能:    Mifare值操作命令
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucOperMode		// 值操作模式
//													MIFARE_INCREMENT 将源块的值加上指定的值存放到目的块
//													MIFARE_DECREMENT 将源块的值减去指定的值存放到目的块
//													MIFARE_RESTORE   将源块的值复制到目的块
//				unsigned char ucSBNr			// 操作的源块地址。
//				unsigned char ucDBNr			// 值操作的目地块
//				long lValue						// 非负的有符号数。
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	MIFARE_RESTORE 操作模式是将指定的值块复制到目的块，一般用于值块操作的备份处理(该模式下，
//				参数值无效，为任意值均可)
//				该函数只支持M1 S50/70卡和PLUS SL1的卡
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_MF1ValueOper(HANDLE hCtrl, unsigned char ucOperMode, unsigned char ucSBNr, 
											unsigned char ucDBNr, long lValue);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SetAutoDetectPar(HANDLE hCtrl, const AUTO_DETECT_PAR *pAutoDetectPar)
// 函数功能:    设置A型卡自动检测参数
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				AUTO_DETECT_PAR *pAutoDetectPar	// A型卡自动检测参数
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	ZLG522S只支持读一块数据
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SetAutoDetectPar(HANDLE hCtrl, const AUTO_DETECT_PAR *pAutoDetectPar);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_GetAutoDetectRes(HANDLE hCtrl, unsigned char ucMode, 
//													   unsigned char *pTxMode, AUTO_DETECT_RES *pAutoDetectRes)
// 函数功能:    获取A型卡自动检测的结果（用于检测到卡后不主动上传数据模式）
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucMode			// ucMode.0		= 0  -- 取消自动检测；	1 -- 继续检测
// 出口参数:    unsigned char *pTxMode			// pTxMode.1:0	= 01 -- TX1驱动
//												//				= 10 -- TX2驱动
//												//				= 11 -- TX1、TX2同时驱动
//				AUTO_DETECT_RES *pAutoDetectRes	// 卡片信息
// 返 回 值:    STATUS_SUCCESS -- 成功；	
//				STATUS_NO_TARGET -- 未检测到卡
//				STATUS_NY_IMPLEMENTED -- 不支持该命令（没有设置自动检测命令）；
//				其它值 -- 失败。
// 说    明:	该函数和ZLG522S_ReadAutoDetectRes()是互斥的，不能同时调用
//				ZLG500S、ZLG522S和ZLG522SP系列模块支持
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_GetAutoDetectRes(HANDLE hCtrl, unsigned char ucMode, unsigned char *pTxMode,
												 AUTO_DETECT_RES *pAutoDetectRes);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ReadAutoDetectRes(HANDLE hCtrl, unsigned int uiTimeOver, 
//												unsigned char *pTxMode, AUTO_DETECT_RES *pAutoDetectRes);
// 函数功能:    读取A型卡自动检测的结果（用于检测到卡后数据主动上传模式）
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned int uiTimeOver			// ucMode.0		= 0  -- 取消自动检测；	1 -- 继续检测
// 出口参数:    unsigned char *pTxMode			// pTxMode.1:0	= 01 -- TX1驱动
//												//				= 10 -- TX2驱动
//												//				= 11 -- TX1、TX2同时驱动
//				AUTO_DETECT_RES *pAutoDetectRes	// 卡片信息
// 返 回 值:    STATUS_SUCCESS -- 成功；
//				STATUS_COMM_ERROR -- 未检测到卡/通讯错误	
//				其它值 -- 失败。
// 说    明:	该函数和ZLG522S_GetAutoDetectRes()是互斥的，不能同时调用
//				ZLG500S、ZLG522S和ZLG522SP系列模块支持
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_ReadAutoDetectRes(HANDLE hCtrl, unsigned int uiTimeOver, 
												  unsigned char *pTxMode, AUTO_DETECT_RES *pAutoDetectRes);
//===========================================================================================================
// ISO14443 
//===========================================================================================================
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_PiccRequest(HANDLE hCtrl, unsigned char ucReqCode,unsigned char *pATQ)
// 函数功能:    请求(ISO14443-3A)
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucReqCode			// 请求代码	ISO14443_3A_REQIDL	0x26	IDLE
//												// 			ISO14443_3A_REQALL	0x52	ALL
// 出口参数:    unsigned char *pATQ				// 请求回应码,2字节
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	ZLG500S和ZLG522S没有该函数，用MFRequest()函数来实现 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_PiccRequest(HANDLE hCtrl, unsigned char ucReqCode, unsigned char *pATQ);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_PiccAnticoll(HANDLE hCtrl,  unsigned char unMode, unsigned char ucSelCode,  
//													unsigned char ucBitCnt, unsigned char *pUID)
// 函数功能:    位方式防碰撞(ISO14443-3A)
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char unMode			// 保留（设置为1）
//				unsigned char ucSelCode			// SELECT_LEVEL_1--防碰撞等级1
//												// SELECT_LEVEL_2--防碰撞等级2
//												// SELECT_LEVEL_3--防碰撞等级3
//				unsigned char ucBitCnt			// 已知序列号的位数
// 出口参数:    unsigned char *pUID				// 返回序列号缓冲区首址，4字节。
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	ZLG500S和ZLG522S没有该函数，用MFAnticoll()函数来实现    
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_PiccAnticoll(HANDLE hCtrl, unsigned char unMode, unsigned char ucSelCode, 
											 unsigned char ucBitCnt, unsigned char *pUID);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_PiccSelect(HANDLE hCtrl, unsigned char ucSelCode, const unsigned char *pUID, 
//												 unsigned char *pSAK)
// 函数功能:    选择卡(ISO14443-3A)
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucSelCode			// 防碰撞代码	SELECT_LEVEL_1：第1级
//												// 				SELECT_LEVEL_2：第2级
//												// 				SELECT_LEVEL_3：第3级
//				unsigned char *pUID				// 4字节序列号。
// 出口参数:    unsigned char *pSAK				// 选择应答
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	ZLG500S和ZLG522S没有该函数，用MFSelect()函数来实现   
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_PiccSelect(HANDLE hCtrl, unsigned char ucSelCode, const unsigned char *pUID, 
										   unsigned char *pSAK);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_PiccHaltA(HANDLE hCtrl)
// 函数功能:   	将卡置为HALT状态(ISO14443-3A)
// 入口参数:    HANDLE hCtrl					// 通讯控制
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	ZLG500S和ZLG522S没有该函数，用MFHalt()函数来实现 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_PiccHaltA(HANDLE hCtrl);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_PiccRequestATS(HANDLE hCtrl, unsigned char ucCID, unsigned char *pATS, 
//													 unsigned int nATSBufSize, unsigned int *pATSBytes)
// 函数功能:    请求选择以应答
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucCID				// 被寻址的PICC逻辑编号 0x00~0x0E
//				unsigned int nATSBufSize		// ATS缓冲区大小		
// 出口参数:    unsigned char *pATS				// 应答以选择（ATS）
//				unsigned int *pATSBytes			// ATS的字节数
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	ZLG500S和ZLG522S没有该函数，用ExChangeBlock()函数来实现
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_PiccRequestATS(HANDLE hCtrl, unsigned char ucCID, unsigned char *pATS, 
											   unsigned int nATSBufSize, unsigned int *pATSBytes);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_PiccPPSRequest(HANDLE hCtrl, unsigned char ucDSI_DRI)
// 函数功能:    协议和参数选择请求
// 输入参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucDSI_DRI			// b3:2 -- PCD  -> PICC的位率编码(只支持0)
//												// b1:0 -- PICC -> PCD 的位率编码(只支持0)
// 输出参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	ZLG500S和ZLG522S没有该函数，用ExChangeBlock()函数来实现
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_PiccPPSRequest(HANDLE hCtrl, unsigned char ucDSI_DRI);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_PiccDeSelect(HANDLE hCtrl)
// 函数功能:    将激活的卡挂起
// 输入参数:    HANDLE hCtrl					// 通讯控制		
// 输出参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	ZLG500S和ZLG522S没有该函数，用ExChangeBlock()函数来实现
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_PiccDeSelect(HANDLE hCtrl);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_PiccTPCL(HANDLE hCtrl, const unsigned char *pSBuf, unsigned int nSBytes, 
//									   unsigned char *pRBuf, unsigned int nRBufSize, unsigned int *pRBytes)
// 函数功能:    实现非接触式IC卡传输协议(T = CL)，即ISO14443-4 
// 输入参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char *pSBuf			// 发送的数据	
//				unsigned int  nSBytes			// 发送的字节数
//				unsigned int nRBufSize			// 接收缓冲区大小	
// 输出参数:    unsigned char *pRBuf			// 接收的数据
//				unsigned int *pRBytes			// 接收的字节数
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	ZLG500S和ZLG522S没有该函数，用ExChangeBlock()函数来实现
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_PiccTPCL(HANDLE hCtrl, const unsigned char *pSBuf, unsigned int nSBytes, 
										 unsigned char *pRBuf, unsigned int nRBufSize, unsigned int *pRBytes);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_PiccActivateTypeA(HANDLE hCtrl, unsigned char ucMode,  
//														unsigned char ucReqCode, ACTIVEPARAA *pAInfo)
// 函数功能:   	A型卡激活
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucMode			// 保留，设置为1
//				unsigned char ucReqCode			// 请求代码
// 出口参数:    ACTIVEPARAA *pAInfo				// ISO14443 A卡激活信息
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	ZLG500S和ZLG522S没有该函数，用MFActivate()函数来实现  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_PiccActivateTypeA(HANDLE hCtrl, unsigned char ucMode, 
												  unsigned char ucReqCode, ACTIVEPARAA *pAInfo);
//===========================================================================================================
// PLUS CPU
//===========================================================================================================
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL0WritePerso(HANDLE hCtrl, unsigned int uiBNr, const unsigned char *pBuf)
// 函数功能:    SL0 个人化更新数据
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned int uiBNr				// 块地址
//				unsigned char *pBuf				// 写入的数据
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL0WritePerso(HANDLE hCtrl, unsigned int uiBNr, const unsigned char *pBuf);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL0CommitPerso(HANDLE hCtrl)
// 函数功能:    SL0 提交个人化
// 入口参数:    HANDLE hCtrl					// 通讯控制
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	在SL0的PLUS CPU卡，只有修改了以下地址才能执行"SL0提交个人化"命令：
//				0x9000(主控密钥)
//				0x9001(配置块密钥)
//				0x9002(SL2提升密钥，只有支持SL2的卡才有该密钥)
//				0x9003(SL3主控密钥，只有支持SL3的卡才有该密钥)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL0CommitPerso(HANDLE hCtrl);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL1Auth(HANDLE hCtrl, unsigned char ucAuthMode, unsigned char ucBNr, 
//											 const unsigned char *pKey)
// 函数功能:    SL1验证密钥
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucAuthMode		// 验证模式 
//													KEY_TYPE_A | KEY_SOURCE_EXT	外部输入的密钥A验证
//													KEY_TYPE_A | KEY_SOURCE_E2	内部E2的密钥A验证
//													KEY_TYPE_B | KEY_SOURCE_EXT	外部输入的密钥B验证
//													KEY_TYPE_B | KEY_SOURCE_E2	内部E2的密钥B验证
//				unsigned char ucBNr				// 验证的块号	MF1 S50——0～63
//												//				MF1 S70——0～255
//				unsigned char *pKey				// 使用KEY_SOURCE_E2模式时，pKey[0]为密钥存放的扇区,其它无效
//												// 使用KEY_SOURCE_EXT模式时，pKey为6字节密钥
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL1Auth(HANDLE hCtrl, unsigned char ucAuthMode, unsigned char ucBNr, 
										const unsigned char *pKey);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL1Read(HANDLE hCtrl, unsigned char ucBNr, unsigned char *pBuf)
// 函数功能:   	SL1读数据块
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucBNr				// 读的块号
// 出口参数:    unsigned char *pBuf				// 读出的数据
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL1Read(HANDLE hCtrl, unsigned char ucBNr, unsigned char *pBuf);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL1Write(HANDLE hCtrl, unsigned char ucBNr, const unsigned char *pBuf)
// 函数功能:   	SL1写数据块
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucBNr				// 写的块号
//				unsigned char *pBuf				// 写入的数据
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL1Write(HANDLE hCtrl, unsigned char ucBNr, const unsigned char *pBuf);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL1ReadValue(HANDLE hCtrl, unsigned char ucBNr, long *pValue)
// 函数功能:   	SL1卡读取值块
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucBNr				// 读的块号
// 出口参数:    long *pValue					// 读出的值
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL1ReadValue(HANDLE hCtrl, unsigned char ucBNr, long *pValue);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL1WriteValue(HANDLE hCtrl, unsigned char ucBNr, long lValue)
// 函数功能:   	SL1卡写入值
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucBNr				// 写的块号
//				long *pValue					// 写出的值
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL1WriteValue(HANDLE hCtrl, unsigned char ucBNr, long lValue);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL1ValueOper(HANDLE hCtrl, unsigned char ucOperMode, unsigned char ucSBNr, 
//													unsigned char ucDBNr, long lValue)
// 函数功能:    SL1值操作
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucOperMode		// 值操作模式
//													MIFARE_INCREMENT 将源块的值加上指定的值存放到目的块
//													MIFARE_DECREMENT 将源块的值减去指定的值存放到目的块
//													MIFARE_RESTORE   将源块的值复制到目的块
//													MIFARE_TRANSFER	 将缓冲区中的数据传输到指定的块	
//				unsigned char ucSBNr			// 操作的源块地址。
//				unsigned char ucDBNr			// 值操作的目地块
//				long lValue						// 非负的有符号数。
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	模式为MIFARE_RESTORE时，参数‘lValue’无效；
//				模式为MIFARE_TRANSFER时，参数‘lValue’无效 和‘ucSBNr’无效；
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall  ZLG522S_SL1ValueOper(HANDLE hCtrl, unsigned char ucOperMode, unsigned char ucSBNr, 
											  unsigned char ucDBNr, long lValue);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL2AESAuth(HANDLE hCtrl, unsigned char ucAuthMode, unsigned int uiKNr, 
//												 const unsigned char *pAESKey)
// 函数功能:    SL2验证密钥
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucAuthMode		// 验证模式 
//													KEY_SOURCE_EXT	外部输入的密钥验证
//													KEY_SOURCE_E2	内部E2的密钥验证
//				unsigned int uiKNr				// 2KByte的卡：取值范围0x4000～0x403F
//												// 4KByte的卡：取值范围0x4000～0x404F
//				unsigned char *pKey				// 使用KEY_SOURCE_E2模式时，pKey[0]为密钥存放的扇区,其它无效
//												// 使用KEY_SOURCE_EXT模式时，pKey为6字节密钥
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	uiKNr的取值不止0x4000～0x404F，详细内容请参阅PLUS CPU卡的资料
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL2AESAuth(HANDLE hCtrl, unsigned char ucAuthMode, unsigned int uiKNr, 
										   const unsigned char *pAESKey);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL2CryptoAuth(HANDLE hCtrl, unsigned char ucAuthMode, unsigned int uiKNr, 
//													const unsigned char *pKey)
// 函数功能:    SL2验证密钥
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucAuthMode		// 验证模式 
//													KEY_TYPE_A | KEY_SOURCE_EXT	外部输入的密钥A验证
//													KEY_TYPE_A | KEY_SOURCE_E2	内部E2的密钥A验证
//													KEY_TYPE_B | KEY_SOURCE_EXT	外部输入的密钥B验证
//													KEY_TYPE_B | KEY_SOURCE_E2	内部E2的密钥B验证
//				unsigned char ucBNr			// 2KByte的卡：取值范围0～127
//												// 4KByte的卡：取值范围0～255
//				unsigned char *pKey				// 使用KEY_SOURCE_E2模式时，pKey[0]为密钥存放的扇区,其它无效
//												// 使用KEY_SOURCE_EXT模式时，pKey为6字节密钥
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL2CryptoAuth(HANDLE hCtrl, unsigned char ucAuthMode, unsigned char ucBNr, 
											  const unsigned char *pKey);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL2MultiBlockRead(HANDLE hCtrl, unsigned char ucBNr, unsigned char ucExt, 
//														unsigned char *pBuf)
// 函数功能:   	SL2多块读
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucBNr				// 读的起始块号
//				unsigned char ucExt				// 读的块数(取值范围1～3)
// 出口参数:    unsigned char *pBuf				// 读出的数据(ucExt * 16)
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL2MultiBlockRead(HANDLE hCtrl, unsigned char ucBNr, unsigned char ucExt, 
												  unsigned char *pBuf);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL2MultiBlockWrite(HANDLE hCtrl, unsigned char ucBNr, 
//														 unsigned char ucExt, const unsigned char *pBuf)
// 函数功能:   	SL2 多块写
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucBNr				// 写的起始块号
//				unsigned char ucExt				// 写的块数(取值范围1～3)
//				unsigned char *pBuf				// 写入的数据
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL2MultiBlockWrite(HANDLE hCtrl, unsigned char ucBNr, unsigned char ucExt, 
												   const unsigned char *pBuf);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL3FirstAuth(HANDLE hCtrl,  unsigned char ucAuthMode, unsigned int uiKNr, 
//													const unsigned char *pAESKey)
// 函数功能:    SL3 首次验证
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucAuthMode		// 验证模式 
//													KEY_SOURCE_EXT	外部输入的密钥验证
//													KEY_SOURCE_E2	内部E2的密钥验证
//				unsigned int uiKNr				// 密钥地址
//				unsigned char *pKey				// 使用KEY_SOURCE_E2模式时，pKey[0]为密钥存放的扇区,其它无效
//												// 使用KEY_SOURCE_EXT模式时，pKey为6字节密钥
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	uiKNr的取值不止0x4000～0x404F，详细内容请参阅PLUS CPU卡的资料
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL3FirstAuth(HANDLE hCtrl, unsigned char ucAuthMode, unsigned int uiKNr, 
											 const unsigned char *pAESKey);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL3FollowingAuth(HANDLE hCtrl,  unsigned char ucAuthMode,
//													   unsigned int uiKNr, const unsigned char *pAESKey)
// 函数功能:    SL3 跟随验证
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucAuthMode		// 验证模式 
//													KEY_SOURCE_EXT	外部输入的密钥验证
//													KEY_SOURCE_E2	内部E2的密钥验证
//				unsigned int uiKNr				// 密钥地址
//				unsigned char *pKey				// 使用KEY_SOURCE_E2模式时，pKey[0]为密钥存放的扇区,其它无效
//												// 使用KEY_SOURCE_EXT模式时，pKey为16字节密钥
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	数据块的扇区密钥地址为0x4000～0x404F偶数地址为密钥A,奇数地址为密钥B
//				uiKNr的取值不止0x4000～0x404F，详细内容请参阅PLUS CPU卡的资料
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL3FollowingAuth(HANDLE hCtrl, unsigned char ucAuthMode, unsigned int uiKNr, 
												 const unsigned char *pAESKey);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL3ResetAuth(HANDLE hCtrl)
// 函数功能:    SL3 复位验证
// 入口参数:    HANDLE hCtrl					// 通讯控制
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL3ResetAuth(HANDLE hCtrl);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL3Read(HANDLE hCtrl, unsigned char ucMode, unsigned int uiBNr, 
//											  unsigned char ucExt, unsigned char *pBuf)
// 函数功能:   	SL3 读数据块
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucMode			// 读的模式	0x31 -- 命令有MAC，数据密文，回应有MAC
//												//			0x33 -- 命令有MAC，数据明文，回应有MAC
//				unsigned int uiBNr				// 读的起始块号
//				unsigned char ucExt				// 读的块数
// 出口参数:    unsigned char *pBuf				// 读出的数据
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	模式中的密文/明文是指RF场中的数据传输是否加密，读写器与PC之间传输的数据始终是明文
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL3Read(HANDLE hCtrl, unsigned char ucMode, unsigned int uiBNr, 
										unsigned char ucExt, unsigned char *pBuf);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL3Write(HANDLE hCtrl, unsigned char ucMode, unsigned int uiBNr, 
//												unsigned char ucExt, const unsigned char *pBuf)
// 函数功能:   	SL3 写
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucMode			// 写的模式	0xA1 -- 命令有MAC，数据密文，回应有MAC；
//															0xA3 -- 命令有MAC，数据明文，回应有MAC；
//				unsigned int uiBNr				// 写的起始块号
//				unsigned char ucExt				// 写的块数(取值范围1～3)
//				unsigned char *pBuf				// 写入的数据
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	模式中的密文/明文是指RF场中的数据传输是否加密，读写器与PC之间传输的数据始终是明文
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL3Write(HANDLE hCtrl, unsigned char ucMode, unsigned int uiBNr, 
										 unsigned char ucExt, const unsigned char *pBuf);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL3ReadValue(HANDLE hCtrl, unsigned char ucMode, 
//													unsigned int uiBNr, long *pValue)
// 函数功能:   	SL3卡读取值块
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucMode			// 读的模式	0x31 -- 命令有MAC，数据密文，回应有MAC
//												//			0x33 -- 命令有MAC，数据明文，回应有MAC
//				unsigned int uiBNr				// 读的块号
// 出口参数:    long *pValue					// 读出的值
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL3ReadValue(HANDLE hCtrl, unsigned char ucMode, 
											 unsigned int uiBNr, long *pValue);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL3WriteValue(HANDLE hCtrl, unsigned char ucMode, 
//													unsigned int uiBNr, long lValue)
// 函数功能:   	SL3卡写入值
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucMode			// 写的模式	0xA1 -- 命令有MAC，数据密文，回应有MAC；
//															0xA3 -- 命令有MAC，数据明文，回应有MAC；
//				unsigned int uiBNr				// 写的块号
//				long *pValue					// 写出的值
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL3WriteValue(HANDLE hCtrl, unsigned char ucMode, 
											  unsigned int uiBNr, long lValue);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL3ValueOper(HANDLE hCtrl, unsigned char ucMode, unsigned int uiSBNr, 
//													unsigned int uiDBNr, long lValue)
// 函数功能:    SL3值操作
// 入口参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucOperMode		// 值操作模式
//													SL3_INCREMENT	将源块的值加上指定的值存放到目的块
//													SL3_DECREMENT	将源块的值减去指定的值存放到目的块
//													SL3_RESTORE		将源块的值复制到目的块
//													SL3_TRANSFER	将缓冲区中的数据传输到指定的块	
//				unsigned int uiSBNr				// 操作的源块地址(只能是数据块)
//				unsigned int uiDBNr				// 值操作的目地块(只能是数据块)
//				long lValue						// 非负的有符号数。
// 出口参数:    -
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	模式为SL3_RESTORE时，参数‘lValue’无效；
//				模式为SL3_TRANSFER时，参数‘lValue’无效 和‘ucSBNr’无效；
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL3ValueOper(HANDLE hCtrl, unsigned char ucOperMode,  unsigned int uiSBNr, 
											 unsigned int uiDBNr, long lValue);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL3ProximityCheck(HANDLE hCtrl, unsigned char ucKeyMode, 
//														const unsigned char *pProxCheckKey, 
//														unsigned int *pTime)
// 功能描述:    中继攻击检查
// 输入参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char ucKeyMode			// 0 -- 使用PICC验证时的会话Key, 
//												// 1 -- 使用ProximityCheckKey
//				unsigned char *pProxCheckKey	// ProximityCheckKey(密钥地址:0xA001),仅当ucKeyMode=1时有效
// 输出参数:    unsigned int *pTime				// 函数执行时间(us)
// 返 回 值:    命令执行结果
// 说    明:	PPS1固定为0x00，该程序不支持变波特率
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL3ProximityCheck(HANDLE hCtrl, unsigned char ucKeyMode, 
												  const unsigned char *pProxCheckKey, unsigned int *pTime);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL3VCSupport(HANDLE hCtrl, const unsigned char *pIID)
// 功能描述:    选择应用(虚拟卡)
// 输入参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char *pIID				// 应用标识(Installation Identifier,0xB001)
// 输出参数:    -
// 返 回 值:    命令执行结果
// 说    明:	PLUS S不支持该命令
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL3VCSupport(HANDLE hCtrl, const unsigned char *pIID);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL3VCSupportLast(HANDLE hCtrl, const unsigned char *pIID, 
//													   const unsigned char *pENCKey, 
//													   const unsigned char *pMACKey, unsigned char *pCardInfo)
// 功能描述:    选择应用(虚拟卡，返回实际UID等信息)
// 输入参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char *pIID				// 应用标识(Installation Identifier,0xB001)
//				unsigned char *pENCKey			// 数据加密密钥(密钥地址:0xA080)
//				unsigned char *pMACKey			// MAC计算密钥 (密钥地址:0xA081)
// 输出参数:    unsigned char *pCardInfo		// 返回16字节数据 供选择卡片使用
// 返 回 值:    命令执行结果
// 说    明:	若IID不符合，则返回的信息为16字节的随机数，若IID有效，则信息如下
//				pCardInfo[1:2] -- PiccCap2
//				pCardInfo[0] = 0x83		4字节卡号	pCardInfo[3:6] -- 4字节卡号
//				pCardInfo[0] = 0x03		7字节卡号	pCardInfo[3:9] -- 7字节卡号
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL3VCSupportLast(HANDLE hCtrl, const unsigned char *pIID, 
												 const unsigned char *pENCKey, const unsigned char *pMACKey, 
												 unsigned char *pCardInfo);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL3VCSelect(HANDLE hCtrl, unsigned char *pCardInfo, 
//												  unsigned char *pMACKey)
// 功能描述:    选择卡(虚拟卡)
// 输入参数:    HANDLE hCtrl					// 通讯控制
//				unsigned char *pCardInfo		// SupportLast返回16字节数据 供选择卡片使用
//              unsigned char *pMACKey			// 计算MAC的密钥(密钥地址: 0xA000)
// 输出参数:    -             
// 返 回 值:    命令执行结果	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL3VCSelect(HANDLE hCtrl, const unsigned char *pCardInfo, 
											const unsigned char *pMACKey);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 函数原型:    unsigned char ZLG522S_SL3VCDeselect(HANDLE hCtrl)
// 功能描述:    取消虚拟卡选择
// 输入参数:    HANDLE hCtrl					// 通讯控制
// 输出参数:    -             
// 返 回 值:    命令执行结果	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char __stdcall ZLG522S_SL3VCDeselect(HANDLE hCtrl);

//===========================================================================================================
#ifdef __cplusplus
	}
#endif

#endif				// __ZLG522S_DEV_CTRL_H
