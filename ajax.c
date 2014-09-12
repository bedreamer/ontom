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
int ajax_query_card_xml_proc(struct ajax_xml_struct *thiz);
int ajax_confirm_charge_xml_proc(struct ajax_xml_struct *thiz);

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
    {"/querycard.xml",          ajax_query_card_xml_proc},
    {"/confirm.xml",            ajax_confirm_charge_xml_proc},
    {"", NULL}
};

// 生成xml文件
int ajax_gen_xml(struct ajax_xml_struct *thiz)
{
	if ( thiz == NULL ) return ERR_WRONG_PARAM;
	else {
        static struct xml_generator *cursor = xmls;

        if ( cursor->xml_gen_proc == NULL )
            cursor = xmls;

        if ( 0 == strcmp(thiz->xml_name, cursor->xml_name) )
            return cursor->xml_gen_proc(thiz);

        cursor = xmls;

        for ( ; cursor->xml_gen_proc ; cursor ++ ) {
            log_printf(DBG, "%s:%s", thiz->xml_name, cursor->xml_name);
            if ( 0 != strcmp(thiz->xml_name, cursor->xml_name) ) continue;
            return cursor->xml_gen_proc(thiz);
		}

		return ERR_FUN_FAILE;
	}
}

/* 充电刷卡事件查询
 * 不同的使用阶段需要检查不同的字段
 * <start>..</start>:
 *    刷卡触发充电服务响应。 可以是自动或是选择充电方式后获得
 * <confirm>..</confirm>:
 *    充电服务触发后确认开始充电服务。即将开始计费
 * <settle_accounts>..</settle_accounts>:
 *    结账刷卡。达到结束充电条件后触发。
 *               首页/按金钱/按时间/按容量充电页面
 *  UI ------- GET /querycard.xml?mode=auto ------->>> ontom   没有刷卡
 *  or UI ---- GET /querycard.xml?mode=asmoney ---->>> ontom   没有刷卡
 *  or UI ---- GET /querycard.xml?mode=astime ----->>> ontom   没有刷卡
 *  or UI ---- GET /querycard.xml?mode=ascap ------>>> ontom   没有刷卡
 *  UI <<<--------RETURN /querycard.xml -------------- ontom   没有刷卡
 *  UI ------- GET /querycard.xml?mode=auto ------->>> ontom   刷   卡
 *  or UI ---- GET /querycard.xml?mode=asmoney ---->>> ontom   刷   卡
 *  or UI ---- GET /querycard.xml?mode=astime ----->>> ontom   刷   卡
 *  or UI ---- GET /querycard.xml?mode=ascap ------>>> ontom   刷   卡
 *  UI <<<---------RETURN /querycard.xml ------------- ontom   有 刷卡
 *  UI ----GET /querycard.xml?start=yes&bm=auto --->>> ontom   有 刷卡
 *                    页面跳转至充电确认页面
 */
int ajax_query_card_xml_proc(struct ajax_xml_struct *thiz)
{
    thiz->xml_len = sprintf(thiz->iobuff,
        "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n"
        "<tom>\r\n"
        "<start>\r\n"
        "  <valid>yes</valid>\r\n"
        "  <cardid>12345678</cardid>\r\n"
        "  <remaind>4567.9</remaind>\r\n"
        "</start>\r\n"
        "<confirm>\r\n"
        "  <valid>no</valid>\r\n"
        "  <cardid>FFFFFFFF</cardid>\r\n"
        "  <remaind>FFFFFFFF</remaind>\r\n"
        "</confirm>\r\n"
        "<settle_accounts>\r\n"
        "  <valid>no</valid>\r\n"
        "  <cardid>FFFFFFFF</cardid>\r\n"
        "  <remaind>FFFFFFFF</remaind>\r\n"
        "  <god>no</god>\r\n"
        "  <timeout>no</timeout>\r\n"
        "</settle_accounts>\r\n"
        "</tom>\r\n"
        "\r\n" );
    return ERR_OK;
}

/* 充电确认
 * 最先获得刷卡事件的应该是监控程序，而非UI程序
 * 因此在这里需要注意一下：
 * 确认充电应该在监控程序探测到正常的刷卡事件，并且UI程序主动获取到该事件后
 * 再次由UI程序发送开始充电命令
 *
 *                      充电确认页面
 *  UI --------- GET /confirm.xml?start=no ------->>> ontom  没有刷卡  没有开始充电
 *  UI <<<----------RETURN /confirm.xml ------------- ontom  没有刷卡  没有开始充电
 *  UI --------- GET /confirm.xml?start=no ------->>> ontom  刷卡     没有开始充电
 *  UI <<<----------RETURN /confirm.xml ------------- ontom  有刷卡    没有开始充电
 *  UI ----GET /confirm.xml?start=yes&bm=auto ---->>> ontom  有刷卡    开始自动充电
 *  or UI ---GET /confirm.xml?start=yes&bm=money-->>> ontom  有刷卡    开始按金钱充电
 *  or UI ---GET /confirm.xml?start=yes&bm=time -->>> ontom  有刷卡    开始时间充电
 *  or UI ---GET /confirm.xml?start=yes&bm=cap --->>> ontom  有刷卡    开始容量充电
 *  UI <<<----------RETURN /confirm.xml ------------- ontom  有刷卡    开始充电
 *                    页面跳转至充电页面
 */
int ajax_confirm_charge_xml_proc(struct ajax_xml_struct *thiz)
{
    return ERR_ERR;
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
                      "<p>The requested URL was not found on this server.</p>\r\n"
                      "<p>Additionally, a 404 Not Found\r\n"
                      "error was encountered while trying to use an ErrorDocument to handle the request.</p>\r\n"
                      "</body></html>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n");
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

    //printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
    for (; ! *done; ) {
        mg_poll_server(server, 1000);
    }
    mg_destroy_server(&server);

    log_printf(INF, "%s exit.", __FUNCTION__);
    return NULL;
}
