#ifndef _AJAX_DEF_INCLUDED_H
#define _AJAX_DEF_INCLUDED_H

#define XML_IO_SIZE   4*1024  // 4K

#include "mongoose.h"

struct ajax_xml_struct {
    // 动态生成的XML文件长度， 最长为31字节
	char xml_name[32];
	// 生成的XML文件长度
	unsigned int  xml_len;

    // 返回页面类型，兼容WEB端配置
    const char *ct;

    // http 请求连接
    struct mg_connection *xml_conn;
	// XML缓冲区，最大为4K
	char iobuff[XML_IO_SIZE];
};

// 生成xml文件
int ajax_gen_xml(struct ajax_xml_struct *thiz);

#endif /*_AJAX_DEF_INCLUDED_H*/
