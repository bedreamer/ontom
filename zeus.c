/*
 * 该文件用来进行通信数据包缓冲区管理
 * 所有异步通信节点都通过该模块进行分配，释放，回收
 *
 * All rights reserved Hangzhou Power Equipment Co., Ltd.
 *
 * Author: LiJie
 * Email: bedreamer@163.com
 * 2014/09/22 12:24
 */
#include <malloc.h>
#include "zeus.h"
