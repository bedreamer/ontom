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

static int ev_handler(struct mg_connection *conn, enum mg_event ev) {
    int result = MG_FALSE, err;
    struct ajax_xml_struct thiz = {0};
    thiz.xml_conn = conn;
    if (ev == MG_REQUEST) {
        log_printf(DBG, "%s", conn->uri);
        strncpy(thiz.xml_name, conn->uri, 31);
        err = ajax_gen_xml( & thiz );
        if ( err == ERR_OK ) {
            log_printf(DBG, "prepare...");
            mg_printf(conn,
                      "HTTP/1.1 200 HTTP\r\n"
                      "Server: thttpd/2.21b PHP/20030920\r\n"
                      "Access-Control-Allow-Origin: *\r\n"
                      "Content-Type: text/xml\r\n"
                      "Date: Wed, 20 Aug 2014 03:29:12 GMT\r\n"
                      "Last-Modified: Tue, 19 Aug 2014 09:23:50 GMT\r\n"
                      "Accept-Ranges: bytes\r\n"
                      "Content-Length: %d\r\n"
                      "Connection: keep-alive\r\n"
                      "\r\n", thiz.xml_len);
            mg_write(conn, thiz.iobuff, thiz.xml_len);
            log_printf(DBG, "done");
        } else {
            mg_printf(conn,
                      "HTTP/1.1 404 Not Found\r\n"
                      "Date: Fri, 22 Aug 2014 06:38:28 GMT\r\n"
                      "Server: Apache\r\n"
                      "Access-Control-Allow-Origin: *\r\n"
                      "Keep-Alive: timeout=3\r\n"
                      "Connection: Keep-Alive\r\n"
                      "Content-Type: text/html\r\n"
                      "Content-Length: 328\r\n"
                      "\r\n"
                      "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                      "<html><head>\r\n"
                      "<title>404 Not Found</title>\r\n"
                      "</head><body>\r\n"
                      "<h1>Not Found</h1>\r\n"
                      "<p>The requested URL /version.xml was not found on this server.</p>\r\n"
                      "<p>Additionally, a 404 Not Found\r\n"
                      "error was encountered while trying to use an ErrorDocument to handle the request.</p>\r\n"
                      "</body></html>\r\n\r\n"
            );
        }
        result = MG_TRUE;
    } else if (ev == MG_AUTH) {
        result = MG_TRUE;
    }

  return result;
}

// xml 服务线程
// 提供xml文件输出，设置参数输入服务
void *thread_xml_service(void *arg) ___THREAD_ENTRY___
{
    int *done = (int *)arg;
    int mydone = 0;
    struct mg_server *server;
    if ( done == NULL ) done = &mydone;

    log_printf(INF, "%s running...", __FUNCTION__);
    server = mg_create_server(NULL, ev_handler);
    mg_set_option(server, "listening_port", "8081");

    printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
    for (; ! *done; ) {
        mg_poll_server(server, 1000);
    }
    mg_destroy_server(&server);

    log_printf(INF, "%s exit.", __FUNCTION__);
    return NULL;
}
