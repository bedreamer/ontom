////////////////////////////////////////////////Copyright (c)////////////////////////////////////////////////
//                                      广州周立功单片机发展有限公司
//                                            研    究    所
//                                              金卡产品线 
//
//                                          http://www.zlgmcu.com
//
//--------------文件信息-------------------------------------------------------------------------------------
// 文 件 名:    GlobalDefine.h
// 文件描述:    全局定义
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
#ifndef __GLOBAL_DEFINE_H
#define __GLOBAL_DEFINE_H

#ifdef __cplusplus
	extern "C" {
#endif
//============= 常量定义 ====================================================================================
// 模块类型定义
#define MT_ZLG500S					0
#define MT_ZLG522S					1
#define MT_ZLG522SP					2			// 标准帧模式	
#define MT_ZLG522SP_L				3			// 长帧模式	
#define MT_ZLG522SP_EX				4			// 扩展帧模式

// PCD配置模式
#define ISO14443_TYPEA				0			// 
			
// 帧结构
#define CMD_STX						0x20		// 帧起始字符
#define CMD_ETX						0x03		// 帧结束字符
#define CMD_ACK						0x06		// 应答字符
#define CMD_NAK						0x15		// 非应答字符

//  命令/回应帧结构
// 不同的模块，帧长度不一致，标准帧长度分以下3种：
#define CMD_SHORT_FRAME_SIZE		(64 + 6)	// 标准帧最长度(ZLG500S)
#define CMD_STAN_FRAME_SIZE			(64 + 8)	// 标准帧最长度(ZLG522S，低版本的ZLG522SP也使用这种长度)
#define CMD_LONG_FRAME_SIZE			255			// 标准帧最大长度(ZLG522SP)
		
#define CMD_STAN_PRTC_LEN			6			// 标准帧协议长度
// 扩展帧格式		
#define CMD_MAX_SIZE	            300			// 扩展帧的最大长度
#define CMD_PRTC_LEN				7
#define CMD_INFO_MAX_SIZE	        (CMD_MAX_SIZE - CMD_PRTC_LEN)			
												// 最大命令长度 APDU( 256 + 8 ) + CMD_PRTC_LEN( 6/8 )
												// 数据最大长度 = 256
												// APDU协议长度 = 8
												// ExChangeBlock3函数协议长度 = 6
// 命令参数	
#define ISO14443_3A_REQALL          0x52		// 请求所有的卡
#define ISO14443_3A_REQIDL          0x26		// 请求空闲的卡
#define SELECT_LEVEL_1				0x93		// 一级防碰撞/选择
#define SELECT_LEVEL_2				0x95		// 二级防碰撞/选择
#define SELECT_LEVEL_3				0x97		// 三级防碰撞/选择

#define KEY_TYPE_A            		0x60		// 密钥A
#define KEY_TYPE_B            		0x61		// 密钥B
#define KEY_SOURCE_EXT				0x00		// 使用外部输入的密钥验证
#define KEY_SOURCE_E2				0x80		// 使用内部E2的密钥验证
	
#define MIFARE_INCREMENT            0xC1		// 值增模式
#define MIFARE_DECREMENT            0xC0		// 值减模式
#define MIFARE_RESTORE              0xC2		// 复制模式
#define MIFARE_TRANSFER             0xB0		// 传输模式
//============= 全局变量/函数定义 ===========================================================================
typedef struct 
{
	unsigned char ATQ[2];						// ISO14443A卡请求回应
	unsigned char UIDLen;						// ISO14443A卡UID的字节数
	unsigned char UID[10];						// ISO14443A卡UID
	unsigned char SAK;							// ISO14443A卡选择应答
} ACTIVEPARAA;
	
typedef struct									// 自动检测参数
{	unsigned char ADMode;						// 自动检测模式	
												// ADMode.0			= 0 -- 串口不发送；		1 -- 串口主动发送
												// ADMode.1			= 0 -- 不产生中断；		1 -- 产生中断
												// ADMode.2			= 0 -- 数据输出后停止；	1 -- 数据输出后继续检测
												// ADMode.3			= 0 -- 最后执行Halt()；	1 -- 最后不执行Halt()
	unsigned char TXMode;						// 天线驱动模式		
												// ADMode.1:0		= 00 -- TX1、TX2交替驱动
												//					= 01 -- 仅TX1驱动
												//					= 10 -- 仅TX2驱动
												//					= 11 -- TX1、TX2同时驱动
	unsigned char ReqCode;						// 请求代码			0x26 -- IDLE;	0x52 -- ALL
	unsigned char AuthMode;						//认证模式		
												// AuthMode.6:0		= 0x60 -- 密钥A
												//					= 0x61 -- 密钥B
												//					= 其他 -- 不认证
												// AuthMode.7		= 0 -- 外部密钥认证
												//					= 1 -- E2PRAM密钥认证
	unsigned char Key[16];						// 密钥(1/16字节)	若是E2PRAM密钥认证，则为密钥区号（1字节）
												//					若外部密钥认证，则为密钥（16字节）
	unsigned char BNr;							// 读的起始块号
	unsigned char Ext;							// 读的块数			取值范围1~3(同一扇区内)
}AUTO_DETECT_PAR;
	
typedef struct
{
	ACTIVEPARAA TAInfo;					    	// A型卡信息
	unsigned int nBytes;						// 读出数据的字节数
	unsigned char Buf[48];						// 读出的数据
}AUTO_DETECT_RES;

// 标准帧格式
typedef struct
{
	unsigned char FarmeLen;						// 帧长度
	unsigned char SeqOrType;                 	// 包号和命令类型
	unsigned char CmdOrStatus;                	// 命令码/状态
	unsigned char InfoLen;						// 帧信息长度
	unsigned char Info[CMD_INFO_MAX_SIZE];		// 帧数据信息和帧尾(帧尾包括BCC和结束字符)
}CMD_STAN_FRAME;
// 扩展帧格式
typedef struct
{
	unsigned char SeqOrType;                 	// 包号和命令类型
	unsigned char CmdOrStatus;                	// 命令码/状态
	unsigned short InfoLen;						// 帧信息长度
	unsigned char Info[CMD_INFO_MAX_SIZE];		// 帧数据信息和帧尾(帧尾包括BCC和结束字符)
}CMD_EX_FRAME;

typedef union
{
	unsigned char Buf[CMD_MAX_SIZE];
	CMD_EX_FRAME Stru;
}CDU;	
//============= 全局变量/函数定义 ===========================================================================

//===========================================================================================================
#ifdef __cplusplus
	}
#endif

#endif				// __GLOBAL_DEFINE_H
