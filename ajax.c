#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ajax.h"
#include "config.h"
#include "error.h"
#include "log.h"

// 生成xml文件，并返回文件大小，头部由调用者生成
int ajax_deal_xml_proc(struct ajax_xml_struct *);
int ajax_charge_status_xml_proc(struct ajax_xml_struct *);
int ajax_battery_status_xml_proc(struct ajax_xml_struct *);
int ajax_alarm_xml_proc(struct ajax_xml_struct *);
int ajax_version_xml_proc(struct ajax_xml_struct *);
int ajax_autheticate_xml_proc(struct ajax_xml_struct *);

struct xml_generator {
	// xml 文件名
	char * xml_name;
	// xml 生成过程地址
	int (*xml_gen_proc)(struct ajax_xml_struct *);
}xmls[]={
    {"/deal.xml",               ajax_deal_xml_proc},
    {"/chargestatus.xml",       ajax_charge_status_xml_proc},
    {"/battrystatus.xml",       ajax_battery_status_xml_proc},
    {"/alarm.xml",              ajax_alarm_xml_proc},
    {"/version.xml",            ajax_version_xml_proc},
    {"/autheticate.xml",        ajax_autheticate_xml_proc},
    {"", NULL}
};

// 生成xml文件
int ajax_gen_xml(struct ajax_xml_struct *thiz)
{
	if ( thiz == NULL ) return ERR_WRONG_PARAM;
	else {
		struct xml_generator *cursor = xmls;

        for ( ; cursor->xml_gen_proc ; cursor ++ ) {
            log_printf(DBG, "%s", cursor->xml_name);
            if ( ! strstr(thiz->xml_name, cursor->xml_name) ) continue;
            return cursor->xml_gen_proc(thiz);
		}

		return ERR_FUN_FAILE;
	}
}

// 返回当前交易信息 deal.xml
int ajax_deal_xml_proc(struct ajax_xml_struct *thiz)
{
    log_printf(DBG, "get into %s", __FUNCTION__);
    return ERR_ERR;
}

// 返回充电状态信息 chargestatus.xml
int ajax_charge_status_xml_proc(struct ajax_xml_struct  *thiz)
{
    log_printf(DBG, "get into %s", __FUNCTION__);
    return ERR_ERR;
}

// 返回电池状态信息 batterystatus.xml
int ajax_battery_status_xml_proc(struct ajax_xml_struct  *thiz)
{
    log_printf(DBG, "get into %s", __FUNCTION__);
    return ERR_ERR;
}

// 返回告警信息 alarm.xml
int ajax_alarm_xml_proc(struct ajax_xml_struct  *thiz)
{
    log_printf(DBG, "get into %s", __FUNCTION__);
    return ERR_ERR;
}

// 返回版本信息 version.xml
int ajax_version_xml_proc(struct ajax_xml_struct *thiz)
{
    thiz->xml_len = sprintf(thiz->iobuff,
        "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n"
        "<VERSION>"
        "<THTTPD>%s</THTTPD>"
        "<BROWSER>%s</BROWSER>"
        "<TOM>%s</TOM>"
        "<GOD>%s</GOD>\r\n"
        "</VERSION>\r\n"
        "\r\n",
        config_read("version_httpd"),
        config_read("version_browser"),
        config_read("version_tomd"),
        config_read("version_godd")
    );
    log_printf(DBG, "gen done");
	return ERR_OK;
}

// 操作认证 autheticate.xml
int ajax_autheticate_xml_proc(struct ajax_xml_struct *thiz)
{
    log_printf(DBG, "get into %s", __FUNCTION__);
    return ERR_OK;
}
