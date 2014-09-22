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

struct list_head {struct list_head *pre, *next;};
#define list_ini(lst) (lst).pre = (lst).next = & (lst)

// 节点大小分类, 共计9类大小
typedef enum {
    NODE_16bytes  = 0, // 最小管理节点大小为 16 bytes
    NODE_32bytes  = 1,
    NODE_64bytes  = 2,
    NODE_128bytes = 3,
    NODE_256bytes = 4,
    NODE_512bytes = 5,
    NODE_1Kbytes  = 6,
    NODE_2Kbytes  = 7,
    NODE_4Kbyts   = 8   // 最大管理节点大小为4KB
}NODE_KINDS;
#define KINDS_TOTAL   9

// 描述缓冲区管理结构
struct zeus_struct {
	// 节点缓冲区大小, in bytes.
	size_t zeus_size;

    // 对应每种节点索引的个数
    size_t kind_nr[KINDS_TOTAL];
    // 所有待管理的节点链表
    struct list_head *head[KINDS_TOTAL];
    // 节点管理头，各种类型的节点头, 该链表提供节点分割、合并的依据
    struct list_head *god;
};

typedef enum {
        // 节点状态无效, 新节点生成时，的默认状态
        NODE_INVALID         = 0,
        // 节点状态闲置，可以被分配或是合并
        NODE_FREELY          = 1,
        // 节点被分配锁定标记，即将被分配出去
        NODE_ALLOC_MARK      = 2,
        // 节点已经被分配标记，不能再被合并、拆分或是分配
        NODE_ALLOCED         = 3,
        // 碎片节点， 最后一个节点，不能出现在链表中间
        NODE_FRAGMENT        = 4
}NODE_STATUS;

// 分配的缓冲区头结构
struct zeus_node_head {
    // 分配的节点大小类型, 用来做节点管理
    unsigned char node_kind;

    // 节点状态
    NODE_STATUS status;
    // 申请的节点大小， 用来做释放验证
    size_t need_size;

    // 同类别的链表节点
    struct list_head kink_node;
    // 作为单一管理单位的节点
    struct list_head zeus_node;
};

// 缓冲区初始化
struct zeus_struct * zeus_initlize(size_t);
// 按需从缓冲区中获取缓冲
void *zeus_malloc(struct zeus_struct *, size_t size);
// 向缓冲区释放空间
int zeus_free(struct zeus_struct *, void *, size_t);

#endif /*_ZEUS_INCLUDED_H_*/
