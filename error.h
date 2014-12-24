#ifndef _ERROR_H_
#define _ERROR_H_

// 没有错误
#define ERR_OK    0x00000000
// 有错误
#define ERR_ERR   0x80000000
// 参数检查失败
#define ERR_WRONG_PARAM  (ERR_ERR|0x00000001)
// 函数执行失败
#define ERR_FUN_FAILE    (ERR_ERR|0x00000002)
// 文件读写失败
#define ERR_FILE_IO_FAILE (ERR_ERR|0x00000003)
// 达到定时器最大个数
#define ERR_TIMER_BEMAX  (ERR_ERR|0x0000000004)

// 串口数据帧校验失败
#define ERR_FRAME_CHECK_ERR (ERR_ERR|0x00000005)
// 串口数据帧校验数据长度不足, 数据还未接受完成
#define ERR_FRAME_CHECK_DATA_TOO_SHORT (ERR_ERR|0x00000006)
// 串口打开失败
#define ERR_UART_OPEN_FAILE (ERR_ERR|0x00000007)
// 串口配置失败
#define ERR_UART_CONFIG_FAILE (ERR_ERR|0x00000008)

// 数据帧需要立即回应
#define ERR_NEED_ECHO   (ERR_ERR|0x01000000)

#endif /*_ERROR_H_*/
