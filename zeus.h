/*
 * 节点管理分配数据管理模块
 *
 * All rights reserved Hangzhou Power Equipment Co., Ltd.
 *
 * Author: LiJie
 * Email: bedreamer@163.com
 * 2014/09//22 12:26
 */
#ifndef _ZEUS_INCLUDED_H_
#define _ZEUS_INCLUDED_H_

/*
 * 描述缓冲区管理结构
 */
struct zeus_struct {
	// 节点缓冲区大小, in bytes.
	size_t zeus_size;
	// 缓冲区ID, 用于配置管理
	unsigned long zues_id;
};

#endif /*_ZEUS_INCLUDED_H_*/
