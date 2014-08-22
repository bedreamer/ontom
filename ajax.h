#ifndef _AJAX_DEF_INCLUDED_H
#define _AJAX_DEF_INCLUDED_H

#define XML_IO_SIZE   4*1024  // 4K

struct ajax_xml_struct {
	// 动态生成的XML文件长度， 最长为31字节
	char xml_name[32];
	// 生成的XML文件长度
	unsigned int  xml_len;
	// XML缓冲区，最大为4K
	char iobuff[XML_IO_SIZE];
};

// 生成xml文件
int ajax_gen_xml(struct ajax_xml_struct *thiz);

#endif /*_AJAX_DEF_INCLUDED_H*/