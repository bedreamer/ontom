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

#endif /*_ERROR_H_*/