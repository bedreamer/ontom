////////////////////////////////////////////Copyright (c)//////////////////////////////////////////
//                                   广州周立功单片机发展有限公司
//                                         研    究    所
//                                           金卡产品线
//
//                                     http://www.zlgmcu.com
//
//-------------- 文件信息--------------------------------------------------------------------------
// 文 件 名:    StatusCode.h
// 文件描述:    状态代码
// 版    本:    V1.0 
// 创 建 人:    曾本森
// 创建日期:    2008.01.09
//=================================================================================================
//-------------- 修改记录--------------------------------------------------------------------------
// 修改内容:	将有符号数改为无符号数
// 当前版本:    
// 修 改 人:	曾本森
// 修改日期:	2009.06.24
// 注    意: 
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __STATUS_CODE_H
#define __STATUS_CODE_H

//============= 读写器类状态 ======================================================================================
#define STATUS_SUCCESS                  (0)		// 操作成功
#define STATUS_COMM_OK					0x00	// 成功
#define STATUS_COMM_TIMEOUT				0x01	// 通信超时
#define STATUS_COMM_DVC_NOT_ONLINE		0x02	// 设备不在线
#define STATUS_COMM_ERROR				0x03	// 通信错误
#define STATUS_CMD_ERROR				0x04	// 命令错误
// #define STATUS_RFU05
// #define STATUS_RFU06
// #define STATUS_RFU07
// #define STATUS_RFU08
// #define STATUS_RFU09
// #define STATUS_RFU0A
// #define STATUS_RFU0B
// #define STATUS_RFU0C
// #define STATUS_RFU0D
// #define STATUS_RFU0E
// #define STATUS_RFU0F
// 
// #define STATUS_RFU10
// #define STATUS_RFU11
// #define STATUS_RFU12
// #define STATUS_RFU13
// #define STATUS_RFU14
// #define STATUS_RFU15
// #define STATUS_RFU16
// #define STATUS_RFU17
// #define STATUS_RFU18
// #define STATUS_RFU19
// #define STATUS_RFU1A
// #define STATUS_RFU1B
// #define STATUS_RFU1C
// #define STATUS_RFU1D
// #define STATUS_RFU1E
// #define STATUS_RFU1F
// 
// #define STATUS_RFU20
// #define STATUS_RFU21
// #define STATUS_RFU22
// #define STATUS_RFU23
// #define STATUS_RFU24
// #define STATUS_RFU25
// #define STATUS_RFU26
// #define STATUS_RFU27
// #define STATUS_RFU28
// #define STATUS_RFU29
// #define STATUS_RFU2A
// #define STATUS_RFU2B
// #define STATUS_RFU2C
// #define STATUS_RFU2D
// #define STATUS_RFU2E
// #define STATUS_RFU2F
// 
// #define STATUS_RFU30
// #define STATUS_RFU31
// #define STATUS_RFU32
// #define STATUS_RFU33
// #define STATUS_RFU34
// #define STATUS_RFU35
// #define STATUS_RFU36
// #define STATUS_RFU37
// #define STATUS_RFU38
// #define STATUS_RFU39
// #define STATUS_RFU3A
// #define STATUS_RFU3B
// #define STATUS_RFU3C
// #define STATUS_RFU3D
// #define STATUS_RFU3E
// #define STATUS_RFU3F
// 
// #define STATUS_RFU40
// #define STATUS_RFU41
// #define STATUS_RFU42
// #define STATUS_RFU43
// #define STATUS_RFU44
// #define STATUS_RFU45
// #define STATUS_RFU46
// #define STATUS_RFU47
// #define STATUS_RFU48
// #define STATUS_RFU49
// #define STATUS_RFU4A
// #define STATUS_RFU4B
// #define STATUS_RFU4C
// #define STATUS_RFU4D
// #define STATUS_RFU4E
// #define STATUS_RFU4F
// 
// #define STATUS_RFU50
// #define STATUS_RFU51
// #define STATUS_RFU52
// #define STATUS_RFU53
// #define STATUS_RFU54
// #define STATUS_RFU55
// #define STATUS_RFU56
// #define STATUS_RFU57
// #define STATUS_RFU58
// #define STATUS_RFU59
// #define STATUS_RFU5A
// #define STATUS_RFU5B
// #define STATUS_RFU5C
// #define STATUS_RFU5D
// #define STATUS_RFU5E
// #define STATUS_RFU5F
// 
// #define STATUS_RFU60
// #define STATUS_RFU61
// #define STATUS_RFU62
// #define STATUS_RFU63
// #define STATUS_RFU64
// #define STATUS_RFU65
// #define STATUS_RFU66
// #define STATUS_RFU67
// #define STATUS_RFU68
// #define STATUS_RFU69
// #define STATUS_RFU6A
// #define STATUS_RFU6B
// #define STATUS_RFU6C
// #define STATUS_RFU6D
// #define STATUS_RFU6E
// #define STATUS_RFU6F
// 
// #define STATUS_RFU70
// #define STATUS_RFU71
// #define STATUS_RFU72
// #define STATUS_RFU73
// #define STATUS_RFU74
// #define STATUS_RFU75
// #define STATUS_RFU76
// #define STATUS_RFU77
// #define STATUS_RFU78
// #define STATUS_RFU79
// #define STATUS_RFU7A
// #define STATUS_RFU7B
// #define STATUS_RFU7C
// #define STATUS_RFU7D
// #define STATUS_RFU7E
// #define STATUS_RFU7F
// 
// #define STATUS_RFU80
// #define STATUS_RFU81
// #define STATUS_RFU82
// #define STATUS_RFU83
// #define STATUS_RFU84
// #define STATUS_RFU85
// #define STATUS_RFU86
// #define STATUS_RFU87
// #define STATUS_RFU88
// #define STATUS_RFU89
// #define STATUS_RFU8A
// #define STATUS_RFU8B
// #define STATUS_RFU8C
// #define STATUS_RFU8D
// #define STATUS_RFU8E
// #define STATUS_RFU8F
		
#define STATUS_RFU90					0x90	// RFU 0x90
#define STATUS_RFU91					0x91	// RFU 0x91	
#define STATUS_RFU92 					0x92	// RFU 0x92
#define STATUS_RFU93					0x93	// RFU 0x93	
#define STATUS_RFU94					0x94	// RFU 0x94	
#define STATUS_RCV_LENGTH_ERR			0x95	// 接收的长度有误
#define STATUS_MAC_ERR					0x96	// MAC校验有误
#define STATUS_USER_DATA_LEN_ERR		0x97	// 用户输入数据长度有误
#define STATUS_USER_PARAMETER_ERR		0x98	// 用户输入的参数有误
#define STATUS_USER_DATA_TOO_MANY		0x99	// 用户数据过多
#define STATUS_USER_BUF_TOO_SMALL		0x9A	// 用户缓冲区太小
#define STATUS_FUNCTION_ERR				0x9B	// 功能错误 
#define STATUS_INVALID_COMMAND			0x9C	// 无效命令/不支持的命令
#define STATUS_NOT_SAME_CARD			0x9D	// 不是同一张卡				
#define STATUS_TRAN_NOT_COMPLETED		0x9E	// 交易未完成
#define STATUS_APP_NOT_ENABLED_ERR		0x9F	// 应用未启用

#define STATUS_CARD_TYPE_ERR			0xA0	// 卡类型错误
#define STATUS_RFUA1					0xA1	// RFU 0xA1
#define STATUS_RFUA2					0xA2	// RFU 0xA2
#define STATUS_RFUA3					0xA3	// RFU 0xA3
#define STATUS_RFUA4					0xA4	// RFU 0xA4
#define STATUS_RFUA5					0xA5	// RFU 0xA5
#define STATUS_RFUA6					0xA6	// RFU 0xA6
#define STATUS_RFUA7					0xA7	// RFU 0xA7
#define STATUS_RFUA8					0xA8	// RFU 0xA8
#define STATUS_RFUA9					0xA9	// RFU 0xA9
#define STATUS_RFUAA					0xAA	// RFU 0xAA
#define STATUS_RFUAB					0xAB	// RFU 0xAB
#define STATUS_PLUS_IID_ERR				0xAC	// IID错误(不支持的IID)
#define STATUS_PLUS_VPC_TIMEOUT			0xAD	// 存在中继攻击
#define STATUS_PLUS_NOT_SUPPORT_VC		0xAE	// 不支持虚拟卡操作
#define STATUS_PLUS_PCD_CAP_ERROR		0xAF	// PCD能力有误

// PLUS卡错误代码 (0xB0 ~ 0xBF)
#define STATUS_RFUB0					0xB0	// RFU 0xB0
#define STATUS_RFUB1					0xB1	// RFU 0xB1
#define STATUS_RFUB2					0xB2	// RFU 0xB2
#define STATUS_RFUB3					0xB3	// RFU 0xB3
#define STATUS_RFUB4					0xB4	// RFU 0xB4
#define STATUS_RFUB5					0xB5	// RFU 0xB5
#define STATUS_PLUS_AUTH_ERROR			0xB6	// 验证错误
#define STATUS_PLUS_CMD_OVERFLOW		0xB7	// 命令(数据)溢出
#define STATUS_PLUS_MAC_INVALID			0xB8	// 无效的MAC
#define STATUS_PLUS_BNR_INVALID			0xB9	// 无效的块地址
#define STATUS_PLUS_NO_BNR				0xBA	// 不存在的块
#define STATUS_PLUS_NOT_SATISFIED		0xBB	// 安全等级不足
#define STATUS_PLUS_LENGTH_ERROR		0xBC	// 长度错误
#define STATUS_RFUBD					0xBD	// RFU 0xBD
#define STATUS_PLUS_GENERAL_ERROR		0xBF	// 普通操作错误

// ISO14443 命令错误代码
#define STATUS_RFUC0					0xC0	// RFU 0xC0
#define STATUS_RFUC1					0xC1	// RFU 0xC1
#define STATUS_NAD_ERROR				0xC2	// NAD错误
#define STATUS_INVALID_VALUE			0xC3	// 值块格式错误
#define STATUS_PERORM_ERROR				0xC4	// 命令执行结果错误
#define STATUS_EDC_ERROR				0xC5	// 校验字节有误
#define STATUS_APDU_ERROR				0xC6	// APDU命令错误
#define STATUS_SEND_ERROR				0xC7	// 发送错误
#define STATUS_LEN_ERROR				0xC8	// 长度错误
#define STATUS_NAK_ERROR				0xC9	// NAK错误
#define STATUS_PCB_ERROR				0xCA	// PCB错误
#define STATUS_CID_ERROR  			    0xCB	// CID错误
#define STATUS_BLOCK_FORMAT_ERROR       0xCC	// 分组帧格式错误 
#define STATUS_PPS_ERROR  			    0xCD	// PPSS错误
#define STATUS_ATS_ERROR			    0xCE	// ATS错误(ISO14443-4); 复位应答错(ISO7816-3)
#define STATUS_CARD_COMM_ERROR			0xCF	// 与卡片之间的通信出错

#define STATUS_RFUD0					0xD0	// RFU 0xD0
#define STATUS_DATA_DIFFERENT			0xD1	// 数据不相同
#define STATUS_BANNED					0xD2	// 禁止访问
#define STATUS_ACCESS_ERROR				0xD3 	// 存储器访问错误
#define STATUS_BCC_ERROR	            0xD4 	// BCC校验错误
#define STATUS_NO_TARGET                0xD5   	// 无目标
#define STATUS_RFUD6					0xD6	// RFU 0xD6
#define STATUS_NO_RF_FIELD	            0xD7   	// 无RF场错误
#define STATUS_ACCESS_TIMEOUT			0xD8	// 访问超时
#define STATUS_NY_IMPLEMENTED			0xD9	// 不支持的命令
#define STATUS_SERNR_ERROR				0xDA	// 不支持的防碰撞等级
#define STATUS_NO_BITWISE_ANTICOLL		0xDB	// 不支持Bit帧防冲突
#define STATUS_INIT_ERROR				0xDC	// 初始化错误
#define STATUS_TEMP_ERROR        	    0xDD	// 温度有误
#define STATUS_INVALID_DEVICE_STATE     0xDE	// 驱动错误
#define STATUS_INSUFFICIENT_RESOURCES   0xDF	// 系统资源不足

#define STATUS_OTHER_ERROR              0xE0	// 其他错误
#define STATUS_NO_ID_AVAILABLE          0xE1	// 无空闲ID可用
#define STATUS_ID_NOT_IN_USE            0xE2	// 指定的ID不存在
#define STATUS_INSTANCE_ALREADY_IN_USE  0xE3	// INSTANCE 已被使用
#define STATUS_ID_ALREADY_IN_USE        0xE4	// ID已被使用
#define STATUS_WRONG_HALT_FORMAT        0xE5	// 挂起格式错误
#define STATUS_WRONG_UID_CHECKBYTE      0xE6	// 目标UID检测错误
#define STATUS_TARGET_RESET_TOX         0xE7	// 目标复位
#define STATUS_TARGET_SET_TOX           0xE8	// 目标设置超时
#define STATUS_TARGET_DEADLOCKED        0xE9	// 目标死锁
#define STATUS_BLOCKNR_NOT_EQUAL        0xEA	// 通信块错误
#define STATUS_ACK_SUPPOSED             0xEB	// ACK ????
#define STATUS_AUTHENT_ERROR            0xEC	// 验证错误
#define STATUS_INTERFACE_NOT_ENABLED    0xED	// 接口未激活
#define STATUS_INVALID_FORMAT           0xEE	// 无效格式
#define STATUS_INTERFACE_ERROR          0xEF	// 主机接口错误

#define STATUS_UNSUPPORTED_COMMAND      0xF0	// 不支持的命令
#define STATUS_UNSUPPORTED_PARAMETER    0xF1	// 不支持的参数
#define STATUS_INVALID_PARAMETER        0xF2	// 无效参数
#define STATUS_USERBUFFER_FULL          0xF3	// 用户缓冲区满
#define STATUS_FIFO_WRITE_ERROR         0xF4	// FIFO写错误
#define STATUS_ERROR_NY_IMPLEMENTED     0xF5	// 未执行
#define STATUS_PROTOCOL_ERROR           0xF6	// 通信协议有误
#define STATUS_BUFFER_OVERFLOW          0xF7	// BUF溢出错误
#define STATUS_ACCESS_DENIED            0xF8	// 写禁止
#define STATUS_BUFFER_TOO_SMALL         0xF9	// 缓冲区过小
#define STATUS_COLLISION_ERROR          0xFA	// 位冲突错误
#define STATUS_FRAMING_ERROR            0xFB	// 帧错误
#define STATUS_BITCOUNT_ERROR           0xFC	// 接收位计数器错误
#define STATUS_PARITY_ERROR             0xFD	// 奇偶校验错误
#define STATUS_CRC_ERROR                0xFE	// CRC校验错误
#define STATUS_IO_TIMEOUT               0xFF	// 无应答错误
//===========================================================================================================
#endif          // __STATUS_CODE_H


 



