#ifndef _AJAX_DEF_INCLUDED_H
#define _AJAX_DEF_INCLUDED_H

#define XML_IO_SIZE   16*1024  // 4K

#include "../thirdpart/mongoose/mongoose.h"

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
    // 输出临时文件
    FILE * ofile;
};

int ajax_search_file(const char *fn);
int ajax_version_xml_proc(struct ajax_xml_struct *);
int ajax_uart_debug_page(struct ajax_xml_struct *thiz);
int ajax_system_query_json_proc(struct ajax_xml_struct *thiz);
int ajax_system_history_proc(struct ajax_xml_struct *thiz);
int ajax_system_error_proc(struct ajax_xml_struct *thiz);
int ajax_system_do_active(struct ajax_xml_struct *thiz);
int ajax_system_detail_proc(struct ajax_xml_struct *thiz);
int ajax_system_config_save_proc(struct ajax_xml_struct *thiz);
int ajax_system_config_proc(struct ajax_xml_struct *thiz);
int ajax_system_config_options_proc(struct ajax_xml_struct *thiz);
int ajax_system_about_proc(struct ajax_xml_struct *thiz);
int ajax_update_proc(struct ajax_xml_struct *thiz);
int ajax_export_proc(struct ajax_xml_struct *thiz);
int ajax_hot_restart_proc(struct ajax_xml_struct *thiz);
int ajax_query_xml_proc(struct ajax_xml_struct *thiz);
int ajax_query_json_proc(struct ajax_xml_struct *thiz);
int ajax_module_query_proc(struct ajax_xml_struct *thiz);
int ajax_job_resume_json_proc(struct ajax_xml_struct *thiz);
int ajax_job_query_json_proc(struct ajax_xml_struct *thiz);
int ajax_job_detail_json_proc(struct ajax_xml_struct *thiz);
int ajax_job_manpause_json_proc(struct ajax_xml_struct *thiz);
int ajax_job_edit_json_proc(struct ajax_xml_struct *thiz);
int ajax_job_delete_json_proc(struct ajax_xml_struct *thiz);
int ajax_job_create_json_proc(struct ajax_xml_struct *thiz);
int ajax_job_abort_json_proc(struct ajax_xml_struct *thiz);
int ajax_jiaozhun_proc(struct ajax_xml_struct *thiz);
int ajax_gen_xml(struct ajax_xml_struct *thiz);
int ajax_debug_list(struct ajax_xml_struct *thiz);
int ajax_debug_json_list(struct ajax_xml_struct *thiz);
int ajax_debug_commit(struct ajax_xml_struct *thiz);
int ajax_debug_bit_write(struct ajax_xml_struct *thiz);
int ajax_debug_bit_read(struct ajax_xml_struct *thiz);
int ajax_deal_xml_proc(struct ajax_xml_struct *);
int ajax_confirm_charge_xml_proc(struct ajax_xml_struct *thiz);
int ajax_charge_status_xml_proc(struct ajax_xml_struct *);
int ajax_card_init_proc(struct ajax_xml_struct *thiz);
int ajax_battery_status_xml_proc(struct ajax_xml_struct *);
int ajax_autheticate_xml_proc(struct ajax_xml_struct *);
int ajax_alarm_xml_proc(struct ajax_xml_struct *);
int ajax_auth_proc(struct ajax_xml_struct *thiz);

#endif /*_AJAX_DEF_INCLUDED_H*/
